#ifndef MY_CURL_H
#define MY_CURL_H

int GetAuth(const char *PlateId, int Type);
int ToBaidu(char *buffer);
int init_curl();
int ToServerHeartBeat(char *buffer,int Type);

#endif