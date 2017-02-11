/* ************************************************************************
 *       Filename:  a.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年01月07日 20时42分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int myexec(char *name[2]) {
	char buffer[4096*2] = {0};
	int i = 0;
	FILE *pp = popen("ifconfig -a", "r"); //建立管道
	if (!pp) {
		return -1;
	}
	//设置一个合适的长度，以存储每一行输出
	while(fgets(buffer, 4096*2, pp) != NULL){
		char *p = strstr(buffer,"HWaddr");
		if(p == NULL)
			continue;
		p = p + strlen("HWaddr") + 1;
		        if (p[strlen(p) - 1] == '\n') {
					    p[strlen(p) - 1] = '\0'; //去除换行符
				}
		p[strlen(p) - 2] = '\0';
		strcpy(name[i],p);
		printf("name[%d] == %s@\n",i,name[i]);
		i++;
		if(i == 2)
			return 0;
	}
	pclose(pp); //关闭管道
	return 0;
}

int main(int argc, char *argv[])
{
	char *name[2];
	char name_in[4096] = "in:";
	char name_out[4096] = "out:";
	char mac_in[4096] = "in:";
	char mac_out[4096] = "out:";




	name[0] = (char *)malloc(4096);
	name[1] = (char *)malloc(4096);
	myexec(name);
	strcat(name_in,name[0]);
	strcat(name_out,name[0]);
	strcat(mac_in,name[1]);
	strcat(mac_out,name[1]);
	printf("name_in == %s\n",name_in);
	printf("name_out == %s\n",name_out);
	printf("mac_in == %s\n",mac_in);
	printf("mac_out == %s\n",mac_out);

	return 0;
}
