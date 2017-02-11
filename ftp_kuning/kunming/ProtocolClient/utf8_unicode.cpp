#include "utf8_unicode.h"
#include <stdio.h>
#include <string.h>


//********************************************************
//Description:
//	����utf8��ͷȡ�ñ����ɼ����ֽڹ���һ��������UTF�ַ�
//Return:
//	0 Ϊ����ȷ��UTF8ͷ
//	>0 �ֽ���
//********************************************************
int GetUTF8ByteCount(const char head)
{
    int count = 2;
    int zeroFlag = 0x20;

    int i = 3;

	//0XXXXXXX
	if ((head&0x80) == 0)
	{
		return 1;
	}

	//10XXXXXX
	if ((head&0x40) == 0)
	{
		return 0;
	}

	for(i=3; i<=7; i++)
	{
		if ((head&zeroFlag) == 0)
		{
			return count;
		}

		zeroFlag = zeroFlag>>1;
		count++;
	}

	return 0;
}


int UTF8toUnicode(const char* strTempUTF8, int utf8Len, int wUnicode[], int* unicodeSize)
{
    int bHasInValidCode = 0;

    int index = 0;

    int i = 0;


    if (strTempUTF8 == NULL || wUnicode == NULL || unicodeSize == NULL)
    {
        return 0;
    }

    *unicodeSize = 0;

    for (i=0; i<utf8Len; i++)
    {
        int unicode = 0;
        
        //���ڱ�ʶ����utf8��unicode��ת���Ƿ�������
        int bFinish = 1;

        int count = 1;
        
        int byteCount = GetUTF8ByteCount(strTempUTF8[i]);
        if (byteCount <= 0)
        {
            bHasInValidCode = 1;
            break;;
        }
       
        //��ȡ��ͷ���ݣ�������(byteCount-1)*6λ
        unicode = (strTempUTF8[i]&(0xFF>>byteCount))<<((byteCount-1)*6);

        //ȡ������λ
        for (count=1; count<byteCount; count++)
        {
            i++;
            //����Ϊ10XXXXXX
            if ((strTempUTF8[i]&0x80)!=0 && (strTempUTF8[i]&0x40)==0)
            {
                unicode |= (strTempUTF8[i]&0x3F)<<((byteCount-1-count)*6);
            }
            else
            {
                bHasInValidCode = 1;
                bFinish = 0;
                break;
            }
        }

        if (bFinish)
        {
            wUnicode[index] = unicode;
            index++;
        }
    }

    //wUnicode[index] = '\0';

    *unicodeSize = index;

    return bHasInValidCode?-1:0;
}


//**************************************************
//Parameter:
//	nUnicode	[in] :unicode����
//	pUtf8		[out]:�����߷���ռ䣬����6���ֽھ��㹻��
//**************************************************
void UnicodetoUTF8(unsigned int nUnicode, char* pUtf8, int utf8BufSize)
{
	int count = 0;
	unsigned int head = 0x00;

    int offset = 0;
    int byteCount = 1;
 
    if (pUtf8 == NULL)
    {
        return;
    }

    memset(pUtf8, 0, utf8BufSize);

	//ȷ���ֽ���
	if (/*nUnicode>=0x00 && */nUnicode<0x80)
	{
		pUtf8[0] = nUnicode;
		count = 1;
		return;
	}
	else if(nUnicode>=0x80 && nUnicode<0x800)
	{
		head = 0xC0;
		count = 2;
	}
	else if(nUnicode>=0x800 && nUnicode<0x10000)
	{
		head = 0xE0;
		count = 3;
	}
	else if(nUnicode>=0x10000 && nUnicode<0x200000)
	{
		head = 0xF0;
		count = 4;
	}
	else if(nUnicode>=0x200000 && nUnicode<0x4000000)
	{
		head = 0xF8;
		count = 5;
	}
	else if(nUnicode>=0x4000000 && nUnicode<=0x7FFFFFFF)
	{
		head = 0xFC;
		count = 6;
	}else
	{
		return;
	}

	pUtf8[0] = head|(nUnicode>>((count-1)*6));
	//printf("pUtf8[0] = %d\n", (unsigned char)pUtf8[0]);

	for (byteCount=1; byteCount<count; byteCount++)
	{
		offset = (count-1-byteCount)*6;
		if (offset > 0)
		{
			pUtf8[byteCount] = 0x80|((nUnicode>>offset)&0x3F);
		}
		else
		{
			pUtf8[byteCount] = 0x80|(nUnicode&0x3F);
		}
		//printf("pUtf8[%d] = %d\n", byteCount, (unsigned char)pUtf8[byteCount]);
	}
}


