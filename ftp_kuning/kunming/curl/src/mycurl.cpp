#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include "mycurl.h"
#include <unistd.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "BlockSocket.h"

char controller_name_in[256];
char controller_name_out[256];
char controller_mac_in[256];
char controller_mac_out[256];
char controller_sn_in[256];
char controller_sn_out[256];
char token_in[256];
char token_out[256];

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)  
{
	char *buffer = (char *)stream;
	strcpy(buffer,(char *)ptr);
    return size*nmemb;
}

int ToServerWasPaied(char *buffer, const char *PlateId, const char *pilling_id)
{
	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;

	char *url = "http://120.76.209.159:8080/controller/wasPaied";
	char jsonstr[1024] = "{\r\n\t\"controller_name\": \"";
		//"WY-W-002-Enter";
	strcat(jsonstr,controller_name_out);
	char *jsonstr_0 = "\",\r\n\t\"controller_mac_addr\":\"";
	strcat(jsonstr,jsonstr_0);
			//"88:C2:55:A4:99:48"
	strcat(jsonstr,controller_mac_out);

			
	char *jsonstr1 = "\",\r\n\t\"plate_number\":\"";
	char *jsonstr2 = "\",\r\n\t\"billing_id\":\"";
	char *jsonstr3 = "\"\r\n}";
	strcat(jsonstr,jsonstr1);
	strcat(jsonstr,PlateId);
	strcat(jsonstr,jsonstr2);
	strcat(jsonstr,pilling_id);
	strcat(jsonstr,jsonstr3);
	//char *jsonstr = "{\r\n\t\"controller_name\": \"YC-D-WY45\", \r\n\t\"controller_mac_addr\":\"12:12:12:12:12:12:12\",\r\n\t\"plate_number\":\"婊嘇12345\"\r\n}\r\n";

	slist1 = NULL;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");
	//slist1 = curl_slist_append(slist1, "Authorization: CONTROLLER:1212");
	char token_header[1024] = "Authorization: CONTROLLER:";
	char token[256] = "";
	strcpy(token,token_out);
	strcat(token_header,token);
	slist1 = curl_slist_append(slist1, token_header);

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonstr);
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(hnd,CURLOPT_WRITEFUNCTION,write_data); //????????????в???????????
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)buffer);

	ret = curl_easy_perform(hnd);

	curl_easy_cleanup(hnd);
	hnd = NULL;
	curl_slist_free_all(slist1);
	slist1 = NULL;	
	return ret;
}


int ToServerGetAuth(char *buffer, const char *PlateId, int Type)
{
	char controller_name[256] = "";
	char controller_mac[256] = "";
	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;
	char *url = "http://120.76.209.159:8080/controller/getAuthByPlateNumber";
	
	if(Type == 0){
		strcpy(controller_name,controller_name_in);
		strcpy(controller_mac,controller_mac_in);
	}
	else{	
		strcpy(controller_name,controller_name_out);
		strcpy(controller_mac,controller_mac_out);
	}

	char jsonstr[1024] = "{\r\n\t\"controller_name\": \"";
		//"WY-W-002-Enter";
	strcat(jsonstr,controller_name);
	char *jsonstr_0 = "\",\r\n\t\"controller_mac_addr\":\"";
	strcat(jsonstr,jsonstr_0);
			//"88:C2:55:A4:99:48"
	strcat(jsonstr,controller_mac);


	char *jsonstr1 = "\",\r\n\t\"plate_number\":\"";
	char *jsonstr2 = "\"\r\n}";	
	strcat(jsonstr,jsonstr1);
	strcat(jsonstr,PlateId);
	strcat(jsonstr,jsonstr2);
	//char *jsonstr = "{\r\n\t\"controller_name\": \"YC-D-WY45\", \r\n\t\"controller_mac_addr\":\"12:12:12:12:12:12:12\",\r\n\t\"plate_number\":\"婊嘇12345\"\r\n}\r\n";

	slist1 = NULL;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");
	//slist1 = curl_slist_append(slist1, "Authorization: CONTROLLER:1212");
	char token_header[1024] = "Authorization: CONTROLLER:";
	char token[256] = "";
	if(Type == 0)
		strcpy(token,token_in);
	else
		strcpy(token,token_out);
	strcat(token_header,token);
	slist1 = curl_slist_append(slist1, token_header);


	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonstr);
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION,write_data); //????????????в???????????
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)buffer);

	ret = curl_easy_perform(hnd);

	curl_easy_cleanup(hnd);
	hnd = NULL;
	curl_slist_free_all(slist1);
	slist1 = NULL;	
	return ret;
}

int ToServerNotify(char *buffer, const char *PlateId, int Type)
{
	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;
	char *url = "http://120.76.209.159:8080/controller/notifyCarPassGate";

	char controller_name[256] = "";
	char controller_mac[256] = "";
	if(Type == 0){
		strcpy(controller_name,controller_name_in);
		strcpy(controller_mac,controller_mac_in);
	}
	else{	
		strcpy(controller_name,controller_name_out);
		strcpy(controller_mac,controller_mac_out);
	}


	char jsonstr[1024] = "{\r\n\t\"controller_name\": \"";
		//"WY-W-002-Enter";
	strcat(jsonstr,controller_name);
	char *jsonstr_0 = "\",\r\n\t\"controller_mac_addr\":\"";
	strcat(jsonstr,jsonstr_0);
			//"88:C2:55:A4:99:48"
	strcat(jsonstr,controller_mac);


	char *jsonstr1 = "\",\r\n\t\"plate_number\":\"";
	char *jsonstr2 = "\"\r\n}";	
	strcat(jsonstr,jsonstr1);
	strcat(jsonstr,PlateId);
	strcat(jsonstr,jsonstr2);

	slist1 = NULL;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");
	//slist1 = curl_slist_append(slist1, "Authorization: CONTROLLER:1212");
	char token_header[1024] = "Authorization: CONTROLLER:";
	char token[256] = "";
	if(Type == 0)
		strcpy(token,token_in);
	else
		strcpy(token,token_out);
	strcat(token_header,token);
	slist1 = curl_slist_append(slist1, token_header);


	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonstr);
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION,write_data); //????????????в???????????
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)buffer);

	ret = curl_easy_perform(hnd);

	curl_easy_cleanup(hnd);
	hnd = NULL;
	curl_slist_free_all(slist1);
	slist1 = NULL;	
	return ret;
}

int ToBaidu(char *buffer)
{
	CURLcode ret;
	CURL *hnd;
	char *url = "220.181.57.217";

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION,write_data); //????????????в???????????
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)buffer);

	ret = curl_easy_perform(hnd);

	curl_easy_cleanup(hnd);
	hnd = NULL;
	return ret;
}

int ToServerLogin(char *buffer,int Type)
{
	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;
	char *url = "http://120.76.209.159:8080/controller/login";
	char controller_name[256] = "";
	char controller_sn[256] = "";



	char jsonstr[1024] = "{\r\n\t\"controller_sn\": \"";
//"WY-W-002-Enter";
	if(Type == 0)
		strcpy(controller_sn,controller_sn_in);
	else
		strcpy(controller_sn,controller_sn_out);


	char *jsonstr_1 = "\",\r\n\t\"controller_version\":10000\r\n}\r\n";

	strcat(jsonstr,controller_sn);
	strcat(jsonstr,jsonstr_1);
	

	slist1 = NULL;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonstr);
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION,write_data); //????????????в???????????
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)buffer);

	ret = curl_easy_perform(hnd);

	curl_easy_cleanup(hnd);
	hnd = NULL;
	curl_slist_free_all(slist1);
	slist1 = NULL;	
	return ret;
}

int get_uuid(char *buf)
{
	FILE *fp;
	if((fp = popen("cat /proc/sys/kernel/random/uuid","r")) == NULL){
		perror("fail to open popen\n");
		return -1;
	}
	while(fgets(buf, 1024, fp) != NULL){
		printf("uuid:%s\n", buf);
	}
	pclose(fp);
	return 0;
}

#define file_name "sn.txt"


int read_file(char *buf)
{
	int fd = -1;        // fd 就是file descriptor，文件描述符
	//char buf[1024] = {0};
	char writebuf[20] = "l love linux";
	int ret = -1;

	// 第一步：打开文件
	fd = open(file_name, O_RDONLY);//注意之前自己定义个a.txt
	if (-1 == fd)        // 有时候也写成： (fd < 0)
	{
		printf("文件打开错误\n");
		return -1;
	}
	else
	{
		printf("文件打开成功，fd = %d.\n", fd);
	}
/*	   
	// 第二步：读写文件
	// 写文件
	ret = write(fd, writebuf, strlen(writebuf));
	if (ret < 0)
	{
		printf("write失败.\n");
	}
	else
	{
		printf("write成功，写入了%d个字符\n", ret);
	}   
*/
	// 读文件
	ret = read(fd, buf, 1024);
	if (ret < 0)
	{
		printf("read失败\n");
		return -1;
	}
	else
	{
		printf("实际读取了%d字节.\n", ret);
		printf("文件内容是：[%s].\n", buf);
	}
	   
	// 第三步：关闭文件
	close(fd);
	return 0;
}

int write_file(char *writebuf)
{
	int fd = -1;        // fd 就是file descriptor，文件描述符
	//char buf[1024] = {0};
	//char writebuf[20] = "l love linux";
	int ret = -1;

	// 第一步：打开文件
	fd = open(file_name, O_WRONLY);//注意之前自己定义个a.txt
	if (-1 == fd)        // 有时候也写成： (fd < 0)
	{
		printf("文件打开错误\n");
		return -1;
	}
	else
	{
		printf("文件打开成功，fd = %d.\n", fd);
	}
	   
	// 写文件
	ret = write(fd, writebuf, strlen(writebuf));
	if (ret < 0)
	{
		printf("write失败.\n");
		return -1;
	}
	else
	{
		printf("write成功，写入了%d个字符\n", ret);
	}   
	   
	// 第三步：关闭文件
	close(fd);
	return 0;
}

int read_sn()
{
	char buffer[1024] = "";
	int ret = read_file(buffer);
	if(buffer[0] != 0){
		char *p = strstr(buffer,"\n");
		if(p == NULL)
			return -1;
		*p = 0;
		p = p + 1;
		strcpy(controller_sn_in,buffer);
		strcpy(controller_sn_out,p);
		if(*(controller_sn_in + strlen(controller_sn_in) - 1) == '\n')
			*(controller_sn_in + strlen(controller_sn_in) - 1) = 0;
		if(*(controller_sn_out + strlen(controller_sn_out) - 1) == '\n')
			*(controller_sn_out + strlen(controller_sn_out) - 1) = 0;
		return 0;
	}

	char write_buffer[1024] = "";
	get_uuid(controller_sn_in);
	strcat(write_buffer,controller_sn_in);
	get_uuid(controller_sn_out);
	*(controller_sn_out + strlen(controller_sn_out) - 1) = 0;
	strcat(write_buffer,controller_sn_out);
	ret = write_file(write_buffer);
	*(controller_sn_in + strlen(controller_sn_in) - 1) = 0;
	
	return ret;
}

int find_mac(char *buffer, char *name)
{
	char *str = "controller_mac\":\"";
	char *p = strstr(buffer,str);
	if(p == NULL)
		return -1;
	p += strlen(str);
	char *pp = strstr(p,"\"");
	if(pp == NULL)
		return -2;
	*pp = 0;
	strcpy(name,p);
	return 0;
}

int find_name(char *buffer, char *mac)
{
	char *str = "controller_name\":\"";
	char *p = strstr(buffer,str);
	if(p == NULL)
		return -1;
	p += strlen(str);
	char *pp = strstr(p,"\"");
	if(pp == NULL)
		return -2;
	*pp = 0;
	strcpy(mac,p);
	return 0;
}

int find_token(char *buffer, char *token)
{
	char *str = "token\":\"";
	char *p = strstr(buffer,str);
	if(p == NULL)
		return -1;
	p += strlen(str);
	char *pp = strstr(p,"\"");
	if(pp == NULL)
		return -2;
	*pp = 0;
	strcpy(token,p);
	return 0;
}

extern CBlockSocket server;
extern int buttons_fd;
int init_curl()
{
	read_sn();
	printf("controller_sn_in = %s\n",controller_sn_in);
	printf("controller_sn_out = %s\n",controller_sn_out);

	char buffer[4096] = "";
	ToServerLogin(buffer,0);
	if(buffer[0] == 0)
		return -1;
	printf("login_in == %s\n",buffer);
	char *p = strstr(buffer, "\"need_update\":");
	if(p != NULL){
		p += strlen("\"need_update\":");
		printf("need update:%c\n", *p);
		if(*p == '1'){
			char *pp = strstr(buffer, "\"update_address\":\"");
			if(pp != NULL){
				pp += strlen("\"update_address\":\"");
				char *pp_end = strstr(pp, "\"");
				*pp_end = 0;
				printf("update_addr:%s\n", pp);
				char buffer_ftp[1024] = {0};
				sprintf(buffer_ftp, "./start_update.sh %s", pp);
				close(buttons_fd);
				server.Close();
				system(buffer_ftp);
			}
		}
	}
	//char buffer[4098] = {0};
	//ToServerLogin(buffer,int Type)
	char tmp[4096] = "";
	strcpy(tmp,buffer);
	find_mac(tmp, controller_mac_in);
	strcpy(tmp,buffer);
	find_name(tmp, controller_name_in);
	strcpy(tmp,buffer);
	find_token(tmp, token_in);
	if(token_in[0] == 0)
		return -1;

	printf("controller_mac_in = %s\n",controller_mac_in);
	printf("controller_name_in = %s\n",controller_name_in);
	printf("token_in = %s\n",token_in);

	buffer[0] = 0;
	ToServerLogin(buffer,1);
	if(buffer[0] == 0)
		return -2;
	printf("login_out == %s\n",buffer);
	strcpy(tmp,buffer);
	find_mac(tmp, controller_mac_out);
	strcpy(tmp,buffer);
	find_name(tmp, controller_name_out);
	strcpy(tmp,buffer);
	find_token(tmp, token_out);
	if(token_out[0] == 0)
		return -1;

	printf("controller_mac_out = %s\n",controller_mac_out);
	printf("controller_name_out = %s\n",controller_name_out);
	printf("token_out = %s\n",token_out);
	
	
	return 0;
}


int ToServerHeartBeat(char *buffer,int Type)
{
	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;
	char *url = "http://120.76.209.159:8080/controller/keepalive";
	char controller_name[256] = "";
	char controller_sn[256] = "";

	char jsonstr[1024] = "{\r\n\t\"controller_name\": \"";
//"WY-W-002-Enter";
	if(Type == 0)
		strcpy(controller_name,controller_name_in);
	else
		strcpy(controller_name,controller_name_out);
	
	char *jsonstr_0 = "\",\r\n\t\"controller_sn\": \""; 
//"WY-W-002-Enter";
	if(Type == 0)
		strcpy(controller_sn,controller_sn_in);
	else
		strcpy(controller_sn,controller_sn_out);

	char *jsonstr_1 = "\"\r\n}\r\n";

	strcat(jsonstr,controller_name);
	strcat(jsonstr,jsonstr_0);
	strcat(jsonstr,controller_sn);
	strcat(jsonstr,jsonstr_1);
	

	slist1 = NULL;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");


	char token_header[1024] = "Authorization: CONTROLLER:";
		//"1212";
	char token[256] = "";
	if(Type == 0)
		strcpy(token,token_in);
	else
		strcpy(token,token_out);
	strcat(token_header,token);
	
	slist1 = curl_slist_append(slist1, token_header);
	//slist1 = curl_slist_append(slist1, "Authorization: CONTROLLER");

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonstr);
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION,write_data); //????????????в???????????
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)buffer);

	ret = curl_easy_perform(hnd);

	curl_easy_cleanup(hnd);
	hnd = NULL;
	curl_slist_free_all(slist1);
	slist1 = NULL;	
	return ret;
}



extern int flag0;
extern int flag1;


static char PlateIdNotify[200] = {0};
static char PlateIdNotifyIn[200] = {0};


int GetAuth(const char *PlateId, int Type)
{
	if(PlateId[0] == 0)
		return -2;
	char buffer[1024] = {0};
	printf("------------------->%d\n",__LINE__);
	if(Type == 7){
		printf("------------------->%d %s\n",__LINE__,PlateId);
		ToServerGetAuth(buffer, PlateId, 0);
		if(buffer[0] == 0)
			return -1;
		printf("------------------->%d\n",__LINE__);
		printf("%s\n",buffer);
		char *p = strstr(buffer,"\"code\":");
		if(p == NULL)
			return -2;
		p = p + strlen("\"code\":");
		char flag = *p;
		printf("%c\n",flag);
		if(flag == '1'){
			flag0 = 0;
			system("./o0.sh");
			strcpy(PlateIdNotifyIn,PlateId);
		}
	}
	else if(Type == 6){
		printf("notify6 : %s\n", PlateId);
		ToServerNotify(buffer, PlateIdNotifyIn, 0);
		if(buffer[0] == 0)
			return -1;
		printf("%s\n",buffer);
	}
	printf("------------------->%d\n",__LINE__);

	if(Type == 5){
		printf("------------------->%d\n",__LINE__);
		ToServerGetAuth(buffer, PlateId, 1);
		printf("------------------->%d\n",__LINE__);
		if(buffer[0] == 0)
			return -1;
		printf("------------------->%s\n",buffer);
		printf("------------------->%d\n",__LINE__);

		char *p = strstr(buffer,"成功");
		if(p != NULL){
			flag1 = 0;
			system("./o1.sh");
			strcpy(PlateIdNotify,PlateId);
			return 0;
		}
		
		p = strstr(buffer,"wait_pay:");
		if(p == NULL)
			return -2;
		p += strlen("wait_pay:");
		char *p_end = strstr(p,"\"");
		int len = p_end - p;
		char pilling_id[256] = {0};
		strncpy(pilling_id,p,len);
		printf("%s\n%s\n",buffer,pilling_id);
		int waitpay_count = 0;
		while(1){
			ToServerWasPaied(buffer, PlateId, pilling_id);
			if(buffer[0] == 0)
				return -1;
			printf("%s\n",buffer);
			char *p = strstr(buffer,"\"code\":");
			if(p == NULL)
				return -3;
			p = p + strlen("\"code\":");
			char flag = *p;
			printf("%c\n",flag);
			if(flag == '1'){
				flag1 = 0;
				system("./o1.sh");
				strcpy(PlateIdNotify,PlateId);
				break;
			}
			waitpay_count++;
			if(waitpay_count == 24)
				break;
			sleep(5);
		}
	}
	else if(Type == 4){
		printf("notify : %s\n", PlateId);
		
		ToServerNotify(buffer, PlateIdNotify, 1);
		if(buffer[0] == 0)
			return -1;
		printf("%s\n",buffer);
	}
	return 0;		
}

