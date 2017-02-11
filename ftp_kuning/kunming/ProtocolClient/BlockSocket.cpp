#include "BlockSocket.h"
#include <string.h>


#ifndef __linux__
#define close closesocket
#define ioctl ioctlsocket
#define socklen_t int
#endif
#define CONNECT_TIMEOUT 2


CBlockSocket::CBlockSocket()
{
    m_nSocket = -1;

    static bool inited = false;

    if (!inited)
    {
	#ifdef __linux__
		signal(SIGPIPE, SIGPipeHandler_FTP);
	#else
        WSADATA wsaData; 

        int ret = WSAStartup(MAKEWORD(2,2), &wsaData); 
        if(ret != 0) 
        { 
            printf("init winsock error\n");
        } 
	#endif
		
        inited = true;
    }
}

CBlockSocket::~CBlockSocket()
{

}

void CBlockSocket::SIGPipeHandler_FTP(int/* sig*/)
{

}

int CBlockSocket::TCPServer(unsigned int nPort, int nMaxConnectNum/* =5 */)
{
    int servfd = -1;

    if (nPort <= 0 || nMaxConnectNum <= 0)
    {
        return -1;
    }

    servfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("server fd = %d\n", servfd);
    if (servfd == -1)
    {
        perror("socket:");
        return -1;
    }
    int on = 1;
    setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(nPort);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(servAddr.sin_zero), 0, 8);

    if (bind(servfd, (struct sockaddr *) &servAddr, sizeof(struct sockaddr)) == -1)
    {
        perror("Bind:");
        close(servfd);
        return -1;
    }

    if (listen(servfd, nMaxConnectNum) == -1)
    {
        perror("Listen:");
        close(servfd);
        return -1;
    }

    m_nSocket = servfd;

    return 0;
}

int CBlockSocket::ConnectToHost(const string &strIP, unsigned int nPort)
{
    int clientfd = -1;

    if (strIP.empty() || nPort <= 0)
    {
        return -1;
    }

    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1)
    {
        return -1;
    }

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(nPort);
    servAddr.sin_addr.s_addr = inet_addr(strIP.c_str());
    memset(&(servAddr.sin_zero), 0, 8);

    //printf("BlockSocket connect begin------------------------------------------------------------------------\n");
    //if (connect(clientfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr)) == -1)
    //{
    //	printf("BlockSocket connect fail\n");
    //	close(clientfd);
    //	return -1;
    //}
    //printf("BlockSocket connect ok\n");


    unsigned long ul = 1;
    ioctl(clientfd, FIONBIO, &ul); 
    bool ret = false;
    //printf("BlockSocket connect begin------------------------------------------------------------------------\n");
    if (connect(clientfd, (struct sockaddr *) &servAddr, sizeof(struct sockaddr)) == -1)
    {
        timeval tv;
        tv.tv_sec = CONNECT_TIMEOUT;
        tv.tv_usec = 0;
        fd_set set;
        FD_ZERO(&set);
        FD_SET(clientfd, &set);
        if (select(clientfd + 1, NULL, &set, NULL, &tv) > 0)
        {
            int error = -1;

#ifdef WIN32
            int len = sizeof(int);
            getsockopt(clientfd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
#endif
            
#ifdef __linux__
            socklen_t len = sizeof(socklen_t);
            getsockopt(clientfd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
#endif


            if (error == 0)
            {
                ret = true;
            }
            else
            {
                ret = false;
                //cout<<"getsockopt"<<endl;
            }
        }
        else
        {
            ret = false;
            //cout<<"select"<<endl;
        }
    }
    else
    {
        ret = true;
        //cout<<"connect ok!!!"<<endl;
    }

    ul = 0;
    ioctl(clientfd, FIONBIO, &ul);

    if (!ret)
    {
        close(clientfd);
        //printf("BlockSocket connect fail\n");
        return -1;
    }
    //printf("BlockSocket connect ok\n");


    m_nSocket = clientfd;
    return 0;
}

int CBlockSocket::Accept(CBlockSocket &connection)
{
    if (m_nSocket <= 0)
    {
        return -1;
    }

    struct sockaddr_in clientAddr;
    socklen_t clientLength = sizeof(clientAddr);

    connection.m_nSocket = accept(m_nSocket, (struct sockaddr *) &clientAddr, &clientLength);

    if (connection.m_nSocket != -1)
    {
        printf("accept IP %s	Port %d\t", inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
        printf("accept fd:%d\n", connection.m_nSocket);
		if(!strcmp(inet_ntoa(clientAddr.sin_addr),"192.168.1.88")){
			connection.type = 1;
			return 88;
		}
		if(!strcmp(inet_ntoa(clientAddr.sin_addr),"192.168.1.87")){
			connection.type = 0;
			return 87;
		}
    }

    return -2;
}

bool CBlockSocket::CheckReadability()
{
    if (m_nSocket <= 0)
    {
        return false;
    }

    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100*1000;

    fd_set rdSet;
    FD_ZERO(&rdSet);
    FD_SET(m_nSocket, &rdSet);

    const int nRet = select(m_nSocket + 1, &rdSet, NULL, NULL, &tv);

    return nRet == 1;
}

int CBlockSocket::Send(const char *pBuf, int len, int nsec/* =30 */)
{
    if (m_nSocket <= 0)
    {
        return -1;
    }

    timeval tv;
    tv.tv_sec = nsec;
    tv.tv_usec = 0;

    fd_set wrSet;
    FD_ZERO(&wrSet);
    FD_SET(m_nSocket, &wrSet);

    if (select(m_nSocket + 1, NULL, &wrSet, NULL, &tv) == 0)
    {
        cout << "CBlockSocket::Send timeout" << endl;
        return -1; //timeout
    }

    int size = -1;
    int offset = 0;
    while ((offset != len) && (size != 0))
    {
        if ((size = send(m_nSocket, pBuf + offset, len - offset, 0)) == -1)
        {
            cout << "CBlockSocket::Send send len= " << size << endl;
            return -1;
        }
        offset += size;
    }

    return offset;
}

int CBlockSocket::Recv(char *pBuf, int len, int nsec/* =30 */)
{
    if (m_nSocket <= 0)
    {
        return -1;
    }

    timeval tv;
    tv.tv_sec = nsec;
    tv.tv_usec = 0;

    fd_set rdSet;
    FD_ZERO(&rdSet);
    FD_SET(m_nSocket, &rdSet);

    if (select(m_nSocket + 1, &rdSet, NULL, NULL, &tv) == 0)
    {
        cout << "CBlockSocket::Recv timeout" << endl;
        return -1;
    }

	int offset = 0;

	while ( len )
	{
		int re = recv(m_nSocket, pBuf+offset, (len>4096?4096:len), 0);
		if ( re <= 0 )
		{
			return -1;
		}
		len -= re;
		offset += re;
	}

    return offset;
}

void CBlockSocket::Close()
{
    if (m_nSocket > 0)
    {
        close(m_nSocket);
        m_nSocket = 0;
    }
}

//may be only connected connection can get ip and port
void CBlockSocket::GetSockAddrIP(string &strIP)
{
    strIP.clear();

    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);
    if (getsockname(m_nSocket, (struct sockaddr*) (&sa), &len) != -1)
    {
        strIP = inet_ntoa(sa.sin_addr);
        //cout<<"get IP:"<<strIP.c_str()<<endl;
    }

    return;
}

