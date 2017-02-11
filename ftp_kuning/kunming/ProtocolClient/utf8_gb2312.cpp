#include "utf8_gb2312.h"
#include <stdio.h>
#include <string.h>

#include "table_gb2312_unicode.h"
#include "table_unicode_gb2312.h"
#include "utf8_unicode.h"


static int Unicode2Gb2312(unsigned short unicode, unsigned short* gb2312)
{
   	int mid, low, high, len;
	len = sizeof(table_ug)/sizeof(table_ug[0]);
	low = 0;
	high = len - 1;

	while(low <= high)
	{
	    mid = (low + high) / 2;
		if(table_ug[mid].unicode > unicode)
		    high = mid - 1;
		else if(table_ug[mid].unicode < unicode)
		    low =  mid + 1;
		else
        {
            *gb2312 = table_ug[mid].gb2312;
		    return 0;
        }
	}
	return -1;
}

static int Gb23122Unicode(unsigned short gb2312, unsigned short* unicode)
{
    int mid, low, high, len;
	len = sizeof(table_gu)/sizeof(table_gu[0]);
	low = 0;
	high = len - 1;

	while(low <= high)
	{
	    mid = (low + high) / 2;
		if(table_gu[mid].gb2312 > gb2312)
		    high = mid - 1;
		else if(table_gu[mid].gb2312 < gb2312)
		    low =  mid + 1;
		else
        {
            *unicode = table_gu[mid].unicode;
		    return 0;
        }
	}
	return -1;
}

// utf8 -> unicode --> gb2312
int Utf82Gb2312(const char* utf8, int utf8Size, char* gb2312, int gb2312BufSize)
{
    int i = 0;
    int gIndex = 0;
    
    while (i < utf8Size)
    {
        int byteCount = GetUTF8ByteCount(utf8[i]);
        if (byteCount <= 0)
        {
            printf("invalid utf8\n");
            return -1;
        }
        
        if (byteCount == 1)
        {
            gb2312[gIndex] = utf8[i];
            gIndex++;
            i++;
            continue;
        }
        
        if (byteCount > 1)
        {
            char tmp[6];
            memset(tmp, 0, 6);
            memcpy(tmp, utf8+i, byteCount);
            
            int unicode[1];
            int unicodeCount = 0;
            UTF8toUnicode(tmp, byteCount, unicode, &unicodeCount);
            
            unsigned short tGb2312 = 0;
            Unicode2Gb2312((unsigned short)unicode[0], &tGb2312);
            
            *(unsigned short*)(gb2312+gIndex) = tGb2312;
            gIndex += 2;
            i += byteCount;
            
            continue;
        }
    }
    
    gb2312[gIndex] = 0;
    
    return 0;
}

// gb2312 -> unicode --> utf8
int Gb23122Utf8(const char* gb2312, int gb2312Size, char* utf8, int utf8BufSize)
{
    int i = 0;
    int uIndex = 0;
    
    while (i < gb2312Size)
    {
        if (gb2312[i] >> 7)
        {
            char tUtf8[6];
            int tUtf8Len = 0;
            unsigned short t = *(unsigned short*)(gb2312+i);
            unsigned short unicode = 0;
            Gb23122Unicode(t, &unicode);
            
            UnicodetoUTF8(unicode, tUtf8, 6);
            
            tUtf8Len = strlen(tUtf8);
            memcpy(utf8+uIndex, tUtf8, tUtf8Len);
            
            i += 2;
            uIndex += tUtf8Len;
        }
        else
        {
            utf8[uIndex] = gb2312[i];
            uIndex++;
            i++;
        }
    }
    
    utf8[uIndex] = 0;
    
    return 0;
}








