#ifndef KISE_NET_FTP_BLOCKSOCKET_H
#define KISE_NET_FTP_BLOCKSOCKET_H

#ifdef __linux__
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#else
#include <sys/types.h>
#include <WinSock2.h>
#endif

#include "signal.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
using namespace std;


class CBlockSocket
{
public:
    CBlockSocket();
    ~CBlockSocket();

    int TCPServer(unsigned int nPort, int nMaxConnectNum = 5);
    int ConnectToHost(const string &strIP, unsigned int nPort);

    int Accept(CBlockSocket &connection);

    //block send/recv data, data length equal len
    int Send(const char *pBuf, int len, int nsec = 5);
    int Recv(char *pBuf, int len, int nsec = 5);

    bool CheckReadability();
    void Close();

    void GetSockAddrIP(string &strIP);

    int GetFd() { return m_nSocket; }

private:
    static void SIGPipeHandler_FTP(int/* sig*/);

public:
    int m_nSocket;
	int type;
};


#endif //KISE_NET_FTP_BLOCKSOCKET_H

