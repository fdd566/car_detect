#ifndef UTF8_UNICODE_H
#define UTF8_UNICODE_H

#ifdef __cplusplus
extern "C" {
#endif

//-1ת��ʧ�ܣ�0ת���ɹ�
int UTF8toUnicode(const char* strUTF8, int utf8Len, int unicode[], int* unicodeCount);

void UnicodetoUTF8(unsigned int nUnicode, char* pUtf8, int utf8BufSize);

int GetUTF8ByteCount(const char head);

#ifdef __cplusplus
}
#endif

#endif

