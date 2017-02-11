#include "pkg.h"

#include <string.h>
#include <assert.h>

#ifdef __linux__
#include <netinet/in.h>
#else
#include <WinSock2.h>
#endif

#define INITTYPE(type)          ( (type)&=0 )//初始化type字段

#define PUT_SYS_TYPE(type, sysType) ((type) |=(sysType<<24))
#define GET_SYS_TYPE(type) ((type)>>24)

#define PUT_PROTOCOL_MAJOR(type, major) ((type)|=(major)<<14)
#define GET_PROTOCOL_MAJOR(type) (((type)&0x00FFC000)>>14)

#define PUT_PROTOCOL_MINOR(type, minor) ((type)|=(minor)<<10)
#define GET_PROTOCOL_MINOR(type) (((type)&0x3C00)>>10)

#define PUT_MSG_TYPE(type, msgType) ((type)|=(msgType))
#define GUT_MSG_TYPE(type) ((type)&0x3FF)

// type  = 32 bit
// sys_type + major protocol + minor protocol + msg_type
// 8bit         10bit           4bit              10bit
int CreateType(int sysType,int majorProtocol,int minorProtocol,int msgType)
{
    int type;
    INITTYPE(type);
    PUT_SYS_TYPE(type, sysType);
    PUT_PROTOCOL_MAJOR(type, majorProtocol);
    PUT_PROTOCOL_MINOR(type, minorProtocol);
    PUT_MSG_TYPE(type, msgType);

    return type;
}


void GetType(int type,int *sysType,int *majorProtocol,int *minorProtocol,int *msgType)
{
    if (sysType != NULL)
    {
        *sysType = GET_SYS_TYPE(type);
    }

    if (majorProtocol != NULL)
    {
        *majorProtocol = GET_PROTOCOL_MAJOR(type);
    }

    if (minorProtocol != NULL)
    {
        *minorProtocol = GET_PROTOCOL_MINOR(type);
    }

    if (msgType != NULL)
    {
        *msgType = GUT_MSG_TYPE(type);
    }
}


void InitPkg(struct Packet* pkg, unsigned char* buffer, int buffSize, enum Endian endian)
{
    pkg->buffer = buffer;
    pkg->buffSize = buffSize;
    pkg->pkgSize = 0;
    pkg->pkgEndian = endian;
}


void PkgEnd(struct Packet* pkg)
{
    int *p = (int *)(pkg->buffer + sizeof(int));

    //V的长度 = 整个数据包的长度-TL的长度
    if (pkg->pkgEndian == big_endian)
    {
        *p = htonl(pkg->pkgSize - 2* sizeof (int));
    }
    else
    {
        *p = pkg->pkgSize - 2* sizeof (int);
    }
}


int PkgLen(struct Packet* pkg)
{
    return pkg->pkgSize;
}


void PutInt(struct Packet* pkg, int value)
{
    assert(pkg->buffSize - pkg->pkgSize >= sizeof(int));

    if (pkg->pkgEndian == big_endian)
    {
        *(int*)(pkg->buffer+pkg->pkgSize) = htonl(value);
    }
    else
    {
        *(int*)(pkg->buffer+pkg->pkgSize) = value;
    }

    pkg->pkgSize += sizeof(int);
}

void PutUInt(struct Packet* pkg, unsigned int value)
{
    PutInt(pkg, (int)value);
}


void PutLong(struct Packet* pkg, long value)
{
    assert(pkg->buffSize - pkg->pkgSize >= sizeof(long));

    if (pkg->pkgEndian == big_endian)
    {
        *(long*)(pkg->buffer+pkg->pkgSize) = htonl(value);
    }
    else
    {
        *(long*)(pkg->buffer+pkg->pkgSize) = value;
    }

    pkg->pkgSize += sizeof(long);
}

void PutULong(struct Packet* pkg, unsigned long value)
{
    PutLong(pkg, (long)value);
}


void PutShort(struct Packet* pkg, short value)
{
    assert(pkg->buffSize - pkg->pkgSize >= sizeof(short));

    if (pkg->pkgEndian == big_endian)
    {
        *(short*)(pkg->buffer+pkg->pkgSize) = htons(value);
    }
    else
    {
        *(short*)(pkg->buffer+pkg->pkgSize) = value;
    }

    pkg->pkgSize += sizeof(short);
}

void PutUShort(struct Packet* pkg, unsigned short value)
{
    PutShort(pkg, (unsigned short)value);
}

void PutChar(struct Packet* pkg, char value)
{
    assert(pkg->buffSize - pkg->pkgSize >= sizeof(char));

    *(pkg->buffer+pkg->pkgSize) = value;

    pkg->pkgSize += sizeof(char);
}

void PutUChar(struct Packet* pkg, unsigned char value)
{
    PutChar(pkg, (char)value);
}

void PutFloat(struct Packet* pkg, float value)
{
    int tmp;
    memcpy(&tmp, &value, sizeof(float));

    PutInt(pkg, tmp);
}

void PutString(struct Packet* pkg, char* str, int strLen)
{
    assert(pkg->buffSize - pkg->pkgSize >= strLen);

    memcpy(pkg->buffer+pkg->pkgSize, (unsigned char*)str, strLen);

    pkg->pkgSize += strLen;
}

void PutBytes(struct Packet* pkg, unsigned char* str, int strLen)
{
    PutString(pkg, (char*)str, strLen);
}


int GetInt(struct Packet* pkg, int* value)
{
    //assert(pkg->buffSize - pkg->pkgSize >= sizeof(int));
    *value = 0;
    if (pkg->buffSize - pkg->pkgSize < sizeof(int))
    {
        return -1;
    }

    if (pkg->pkgEndian == big_endian)
    {
        *value = ntohl(*(int*)(pkg->buffer+pkg->pkgSize));
    }
    else
    {
        *value = *(int*)(pkg->buffer+pkg->pkgSize);
    }

    pkg->pkgSize += sizeof(int);

    return 0;
}

int GetUInt(struct Packet* pkg, unsigned int* value)
{
    return GetInt(pkg, (int*)value);
}

int GetLong(struct Packet* pkg, long* value)
{
    //assert(pkg->buffSize - pkg->pkgSize >= sizeof(long));
    *value = 0;

    if (pkg->buffSize - pkg->pkgSize < sizeof(long))
    {
        return -1;
    }

    if (pkg->pkgEndian == big_endian)
    {
        *value = ntohl(*(long*)(pkg->buffer+pkg->pkgSize));
    }
    else
    {
        *value = *(long*)(pkg->buffer+pkg->pkgSize);
    }

    pkg->pkgSize += sizeof(long);

    return 0;
}

int GetULong(struct Packet* pkg, unsigned long* value)
{
    return GetLong(pkg, (long*)value);
}

int GetShort(struct Packet* pkg, short* value)
{
    //assert(pkg->buffSize - pkg->pkgSize >= sizeof(short));
    *value = 0;
    if (pkg->buffSize - pkg->pkgSize < sizeof(short))
    {
        return -1;
    }

    if (pkg->pkgEndian == big_endian)
    {
        *value = ntohs(*(short*)(pkg->buffer+pkg->pkgSize));
    }
    else
    {
        *value = *(short*)(pkg->buffer+pkg->pkgSize);
    }

    pkg->pkgSize += sizeof(short);

    return 0;
}


int GetUShort(struct Packet* pkg, unsigned short* value)
{
    return GetShort(pkg, (short*)value);
}

int GetChar(struct Packet* pkg, char* value)
{
    //assert(pkg->buffSize - pkg->pkgSize >= sizeof(char));
    *value = 0;
    if (pkg->buffSize - pkg->pkgSize < sizeof(char))
    {
        return -1;
    }

    *value = *(char*)(pkg->buffer+pkg->pkgSize);

    pkg->pkgSize += sizeof(char);

    return 0;
}


int GetUChar(struct Packet* pkg, unsigned char* value)
{
    return GetChar(pkg, (char*)value);
}

int GetFloat(struct Packet* pkg, float* value)
{
    int ret = 0;
    int v = 0;
    ret = GetInt(pkg, &v);
    memcpy(value, &v, sizeof(int));

    return ret;
}

int GetString(struct Packet* pkg, char* str, int strLen)
{
    //assert(pkg->buffSize - pkg->pkgSize >= strLen);
    int ret = 0;
    
    int copyLen = strLen;

    if (pkg->buffSize - pkg->pkgSize < strLen)
    {
        ret = -1;
        copyLen = pkg->buffSize - pkg->pkgSize;
    }

    if (copyLen <= 0)
    {
        return -1;
    }


    memcpy(str, pkg->buffer+pkg->pkgSize, copyLen);

    pkg->pkgSize += copyLen;

    return ret;
}

int GetBytes(struct Packet* pkg, unsigned char* str, int strLen)
{ 
    return GetString(pkg, (char*)str, strLen);
}







