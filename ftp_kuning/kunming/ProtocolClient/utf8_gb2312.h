#ifndef UTF8_GB2312_H
#define UTF8_GB2312_H

#ifdef __cplusplus
extern "C" {
#endif

int Utf82Gb2312(const char* utf8, int utf8Size, char* gb2312, int gb2312BufSize);

int Gb23122Utf8(const char* gb2312, int gb2312Size, char* utf8, int utf8BufSize);

#ifdef __cplusplus
}
#endif

#endif


