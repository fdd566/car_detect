#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "BlockSocket.h"
#include "pkg.h"
#include "pkg_vehicle.h"
#include "utf8_gb2312.h"
#include "mycurl.h"

#include <sys/stat.h> //mkdir声明
#include <dirent.h>
#include <pthread.h>

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



using namespace std;


string g_ip = "192.168.1.88";

#define SYS_TYPE_PARK 7 // 停车场道闸系统
#define CLIENT_VERSION 2

/* 每个数据包头部长度，包含4字节的T(封装消息ID，系统类型，版本号等) 和4字节的L(表示后面数据长度) */
#define TL_HEADER_SIZE 8

/* 协议主、次版本号 */
#define PROTOCOL_MAJOR	2
#define PROTOCOL_MINOR	1 

#define MESSAGE_ID_HEART_BEAT	     2		/* 心跳包 */
#define MESSAGE_ID_REPLY	         4		/* 统一应答 */
#define MESSAGE_ID_CAR		         5		/* 抓拍数据包 */
#define MESSAGE_ID_LIVE_IMAGE        103	/* 实时图像 */
#define MESSAGE_ID_SIMPLE_CARINFO   126       /*精简版车辆数据包*/

#define MESSAGE_ID_GET_SIMPLE_WLIST 127     /*获取精简版本白名单*/
#define MESSAGE_ID_SET_SIMPLE_WLIST 128     /*设置精简版本白名单*/

#define MESSAGE_ID_GET_REGINFO 129          /*获取注册信息*/

#define MESSAGE_ID_SOFT_TRIGG       211    /*软触发*/
#define MESSAGE_ID_HASX_LIVE_IMAGE 505  /* 实时图像,历史原因，两个都是图像包 */



/* 以下是统一应答码的定义 */
#define REPLYCODE_OK		(0)				/* 已成功处理消息 */
#define REPLYCODE_PROTOCOL_UNMATCH	(1)		/* 所接收消息与本地协议不匹配 */
#define REPLYCODE_MESSAGEID_ERROR	(2)		/* 无法识别的消息ID */
#define REPLYCODE_ERROR_VALUE		(3)		/* 消息内容中存在错误的值 */


struct HA_SimpleWList
{
    char plate[16];
    unsigned short year;
    unsigned char month;
    unsigned char day;
};


struct VERSION
{
    char protocl_version[64];

    char arm_version[64];
    char arm_buildtime[64];
    char arm_code_version[64];

    char dsp_version[64];
    char dsp_buildtime[64];
    char dsp_code_version[64];
};


CBlockSocket server;


static void ReleaseDetectInfo(struct pkg_vehicle* pkg_v);


static void SendCarPacketAck(CBlockSocket& s, char* packetID)
{
    unsigned char buffer[128];
    struct Packet pkg;
    InitPkg(&pkg, buffer, 128, little_endian);

    int type = CreateType(SYS_TYPE_PARK, PROTOCOL_MAJOR, PROTOCOL_MINOR,MESSAGE_ID_REPLY);
    PutInt(&pkg, type);
    PutInt(&pkg, 0);
    PutInt(&pkg, MESSAGE_ID_CAR);
    PutInt(&pkg, REPLYCODE_OK);
    PutBytes(&pkg, (unsigned char*)packetID, 64);
    PkgEnd(&pkg);

    s.Send((char *)pkg.buffer, pkg.pkgSize);
}



static void SavePkgVehicle(struct pkg_vehicle* pkgv)
{

    //车牌汉字编码为utf8

    char gb2312[64];
    memset(gb2312, 0, sizeof(gb2312));
    Utf82Gb2312(pkgv->plate, strlen(pkgv->plate), gb2312, sizeof(gb2312));

    printf("plate = %s, plate color = %d, img num = %d, img len = %d, triger_type = %d\n", pkgv->plate, pkgv->plate_color, pkgv->img_num, pkgv->img_len[0], pkgv->triger_type);

    static int i = 0;
    char filename[128];

#ifdef WIN32
    sprintf(filename, "D:\\img\\plate\\%d.jpg", i++);
    _mkdir("D:\\img");
    _mkdir("D:\\img\\plate");
#endif

#ifdef __linux__
    sprintf(filename, "img/plate/%d.jpg", i++);
    mkdir("img", S_IRWXU);
    mkdir("img/plate", S_IRWXU);
#endif

    FILE* fp = fopen(filename, "wb");
    if (fp)
    {
        fwrite(pkgv->img[0], pkgv->img_len[0], 1, fp);
        fclose(fp);
    }
}


static void ProcessCarPacket(CBlockSocket s, const char* data, int dataSize)
{

    struct Packet packet, *pkg;
    pkg = &packet;

    struct pkg_vehicle* pkg_v = new struct pkg_vehicle;

    InitPkg(pkg, (unsigned char*)data, dataSize, little_endian);

    int T, L;
    GetInt(pkg, &T);
    GetInt(pkg, &L);

    GetUInt(pkg, &pkg_v->sequence_id);

    // printf("sequence_id = %d\n", pkg_v->sequence_id);

    GetUInt(pkg, &pkg_v->realtime_data);
    GetString(pkg, pkg_v->packet_id, 64);
    GetString(pkg, pkg_v->device_id, 32);
    GetString(pkg, pkg_v->addr_id, 32);
    GetString(pkg, pkg_v->direction_id, 32);
    GetString(pkg, pkg_v->camera_id, 32);
    GetString(pkg, pkg_v->lane_id, 32);
    GetString(pkg, pkg_v->addr_name, 32*3);
    GetString(pkg, pkg_v->direction_name, 32*3);
    GetString(pkg, pkg_v->plate, 16);

    GetUShort(pkg, &pkg_v->plate_x);
    GetUShort(pkg, &pkg_v->plate_y);
    GetUShort(pkg, &pkg_v->plate_w);
    GetUShort(pkg, &pkg_v->plate_h);

    GetInt(pkg, &pkg_v->plate_confidence);

    GetUChar(pkg, &pkg_v->plate_color);
    GetUChar(pkg, &pkg_v->vehicle_style);
    GetUChar(pkg, &pkg_v->vehicle_color);
    GetUChar(pkg, &pkg_v->triger_type);


    GetInt(pkg, &pkg_v->redlight_begin_sec);
    GetInt(pkg, &pkg_v->redlight_begin_usec);


    GetUInt(pkg, &pkg_v->system_type);
    GetUInt(pkg, &pkg_v->vio_type);

    GetUShort(pkg, &pkg_v->vehicle_speed);
    GetUShort(pkg, &pkg_v->limit_speed);

    GetInt(pkg, &pkg_v->recoImageIndex);
    GetUShort(pkg, &pkg_v->vehicle_x[0]);
    GetUShort(pkg, &pkg_v->vehicle_y[0]);
    GetUShort(pkg, &pkg_v->vehicle_w[0]);
    GetUShort(pkg, &pkg_v->vehicle_h[0]);

    for (int i = 0; i < 4; i++)
    {
        GetUShort(pkg, &pkg_v->red_lamp_x[i]);
        GetUShort(pkg, &pkg_v->red_lamp_y[i]);
        GetUShort(pkg, &pkg_v->red_lamp_w[i]);
        GetUShort(pkg, &pkg_v->red_lamp_h[i]);
    }
    GetUShort(pkg, &pkg_v->vio_red_lamp_index);
    GetUShort(pkg, &pkg_v->direction);

    for (int i = 1; i < 4; i++)
    {
        GetUShort(pkg, &pkg_v->vehicle_x[i]);
        GetUShort(pkg, &pkg_v->vehicle_y[i]);
        GetUShort(pkg, &pkg_v->vehicle_w[i]);
        GetUShort(pkg, &pkg_v->vehicle_h[i]);
    }

    char reserved[128];
    GetString(pkg, pkg_v->reserved, 56);

    GetUChar(pkg, &pkg_v->img_num);
    GetUChar(pkg, &pkg_v->video_num);
    GetUChar(pkg, &pkg_v->plate_img_num);
    GetUChar(pkg, &pkg_v->face_num);

    int i = 0;
    for (i=0; i<pkg_v->img_num; i++)
    {
        GetUInt(pkg, &pkg_v->img_len[i]);
        //printf("img_len[%d] = %d\n", i, pkg_v->img_len[i]);
    }

    for (i=0; i<pkg_v->video_num; i++)
    {
        GetUInt(pkg, &pkg_v->video_len[i]);
        // printf("video_len[%d] = %d\n", i, pkg_v->video_len[i]);
    }

    for (i=0; i<pkg_v->plate_img_num; i++)
    {
        GetUInt(pkg, &pkg_v->plate_img_len[i]);
        // printf("plate_img_len[%d] = %d\n", i, pkg_v->plate_img_len[i]);
    }

    for (i=0; i<pkg_v->face_num; i++)
    {
        GetUInt(pkg, &pkg_v->face_img_len[i]);
        // printf("face_img_len[%d] = %d\n", i, pkg_v->face_img_len[i]);
    }

    if (pkg_v->img_num > 0)
    {
        GetString(pkg, pkg_v->img_format, 8);
    }

    if (pkg_v->video_num > 0)
    {
        GetString(pkg, pkg_v->video_format, 8);
    }

    if (pkg_v->plate_img_num)
    {
        GetString(pkg, pkg_v->plate_img_format, 8);
    }

    if (pkg_v->face_num > 0)
    {
        GetString(pkg, pkg_v->face_img_format, 8);
    }

    //图片时间
    for (i=0; i<pkg_v->img_num; i++)
    {
        GetInt(pkg, &pkg_v->img_time_sec[i]);

    }
    for (i=0; i<pkg_v->img_num; i++)
    {
        GetInt(pkg, &pkg_v->img_time_usec[i]);
    }


    //视频开始时间
    for (i=0; i<pkg_v->video_num; i++)
    {
        GetInt(pkg, &pkg_v->video_start_sec[i]);
    }
    for (i=0; i<pkg_v->video_num; i++)
    {
        GetInt(pkg, &pkg_v->video_start_usec[i]);
    }

    //视频结束时间
    for (i=0; i<pkg_v->video_num; i++)
    {
        GetInt(pkg, &pkg_v->video_end_sec[i]);
    }
    for (i=0; i<pkg_v->video_num; i++)
    {
        GetInt(pkg, &pkg_v->video_end_usec[i]);
    }

    //人脸
    for (i=0; i<pkg_v->face_num; i++)
    {
        GetUShort(pkg, &pkg_v->face_x[i]);
    }
    for (i=0; i<pkg_v->face_num; i++)
    {
        GetUShort(pkg, &pkg_v->face_y[i]);
    }
    for (i=0; i<pkg_v->face_num; i++)
    {
        GetUShort(pkg, &pkg_v->face_w[i]);
    }
    for (i=0; i<pkg_v->face_num; i++)
    {
        GetUShort(pkg, &pkg_v->face_h[i]);
    }


    for (i=0; i<pkg_v->img_num; i++)
    {
        pkg_v->img[i] = new unsigned char[pkg_v->img_len[i]];
        GetBytes(pkg, pkg_v->img[i], pkg_v->img_len[i]);
    }

    for (i=0; i<pkg_v->video_num; i++)
    {
        pkg_v->video[i] = new unsigned char[pkg_v->video_len[i]];
        GetBytes(pkg, pkg_v->video[i], pkg_v->video_len[i]);
    }

    for (i=0; i<pkg_v->plate_img_num; i++)
    {
        pkg_v->plate_img[i] = new unsigned char[pkg_v->plate_img_len[i]];
        GetBytes(pkg, pkg_v->plate_img[i], pkg_v->plate_img_len[i]);
    }

    for (i=0; i<pkg_v->face_num; i++)
    {
        pkg_v->face_img[i] = new unsigned char[pkg_v->face_img_len[i]];
        GetBytes(pkg, pkg_v->face_img[i], pkg_v->face_img_len[i]);
    }


    SendCarPacketAck(s, pkg_v->packet_id);

    SavePkgVehicle(pkg_v);
    
    ReleaseDetectInfo(pkg_v);
}



static void ReleaseDetectInfo(struct pkg_vehicle* pkg_v)
{
    int i = 0;

    for (i=0; i<pkg_v->img_num; i++)
    {
        delete[] pkg_v->img[i];
    }

    for (i=0; i<pkg_v->video_num; i++)
    {
        delete[] pkg_v->video[i];
    }

    for (i=0; i<pkg_v->plate_img_num; i++)
    {
        delete[] pkg_v->plate_img[i];
    }

    for (i=0; i<pkg_v->face_num; i++)
    {
        delete[] pkg_v->face_img[i];
    }

    delete pkg_v;
}


static void ProcessGetSimpleWList(const char* data, unsigned int dataSize)
{
    struct Packet packet, *pkg;
    pkg = &packet;

    InitPkg(pkg, (unsigned char*)data, dataSize, little_endian);

    int type, length;
    int reply_message;
    int reply_code;

    GetInt(pkg,&type);
    GetInt(pkg,&length);
    GetInt(pkg, &reply_message);
    GetInt(pkg, &reply_code);

    int count = 0;
    char plate[16];
    unsigned short year = 0;
    unsigned char month = 0;
    unsigned char day = 0;

    GetInt(pkg, &count);
    printf("wlist count = %d\n", count);
    for(int i=0; i<count; i++)
    {
        GetString(pkg, plate, 16);
        GetUShort(pkg, &year);
        GetUChar(pkg, &month);
        GetUChar(pkg, &day);

        printf("wlist[%d], plate = %s, %.4d-%.2d-%.2d\n", i, plate, year, month, day);
    }
}



static void PrintVersion(struct VERSION *v)
{
    printf("\n\t ========version info begin========\n");
    //printf("\n\t=========sub system: %s========\n", subsys);
    printf("\t protocl_version = %s\n", v->protocl_version);
    printf("\t arm_version = %s\n", v->arm_version);
    printf("\t arm_buildtime = %s\n", v->arm_buildtime);
    printf("\t arm_code_version = %s\n", v->arm_code_version);

    printf("\n");

    printf("\t dsp_version = %s\n", v->dsp_version);
    printf("\t dsp_buildtime = %s\n", v->dsp_buildtime);
    printf("\t dsp_versionctrl = %s\n", v->dsp_code_version);

    printf("\t ========version info end========\n\n");
}



static void ProcessGetRegInfo(const char* data, unsigned int dataSize)
{
    struct Packet packet, *pkg;
    pkg = &packet;

    InitPkg(pkg, (unsigned char*)data, dataSize, little_endian);

    int type, length;
    int reply_message;
    int reply_code;

    GetInt(pkg,&type);
    GetInt(pkg,&length);
    GetInt(pkg, &reply_message);
    GetInt(pkg, &reply_code);

    int snLen = 0;
    unsigned char sn[16];
    GetInt(pkg, &snLen);
    GetBytes(pkg, sn, snLen);

    struct VERSION v;
    GetBytes(pkg, (unsigned char*)(&v), sizeof(struct VERSION));
    
    PrintVersion(&v);
}


static void ProcessLiveImage(const char* data, unsigned int size)
{
    unsigned short w, h;
    unsigned int h264Len;
    unsigned int seqNo;
    unsigned int cySeqNo;

    static int lastSeqNo = 0;
    static bool bFirst = 1;

    static unsigned char* h264Buffer = NULL;
    if (h264Buffer == NULL)
    {
        h264Buffer = new unsigned char[1024*1024*2];
    }

    struct Packet packet, *pkg;
    pkg = &packet;

    int type = 0;
    int len = 0;
    int format;
    int i = 0;

    InitPkg(pkg, (unsigned char*)data, size, little_endian);

    GetInt(pkg, &type);
    GetInt(pkg, &len);

    GetInt(pkg, &format);

    GetUShort(pkg, &w);
    GetUShort(pkg, &h);

    GetUInt(pkg, &seqNo);
    GetUInt(pkg, &cySeqNo); 

    GetUInt(pkg, &h264Len);

    GetBytes(pkg, h264Buffer, h264Len);

    //printf("h264 len = %d\n", h264Len);

    //m_pH264DecodeThread->Put((char*)m_pH264Buffer, h264Len, seqNo);
}

char PlateId[128];
static void ProcessSimpleCarPacket(CBlockSocket s, const char* data, int size)
{
    unsigned int sequence_id = 0;
    char plate[16] = {0};
    unsigned char plateColor = 0;
    unsigned char triggerType = 0;

    struct Packet packet, *pkg;
    pkg = &packet;

    InitPkg(pkg, (unsigned char*)data, size, little_endian);

    int T, L;
    GetInt(pkg, &T);
    GetInt(pkg, &L);

    int ret = GetUInt(pkg, &sequence_id);
    GetString(pkg, plate, 16);
    GetUChar(pkg, &plateColor);
    GetUChar(pkg, &triggerType);

	if(plate[0] == 0)
		return;
	Gb23122Utf8(plate, 16, PlateId, 128);
    printf("sequence_id = %d, plate = %s, plateColor = %d, triggerType = %d\n", sequence_id, PlateId, plateColor, triggerType);
	printf("s.type =%d\n",s.type);
	if(s.type == 0)
		GetAuth(PlateId, 7);
	else
		GetAuth(PlateId, 5);

   // printf("sequence_id = %d, plate = %s, plateColor = %d, triggerType = %d\n", sequence_id, PlateId, plateColor, triggerType);
}


static void SendCmd_Trigger(CBlockSocket s)
{
    unsigned char buffer[128];
    struct Packet pkg;
    InitPkg(&pkg, buffer, 128, little_endian);

    int type = CreateType(SYS_TYPE_PARK, PROTOCOL_MAJOR, PROTOCOL_MINOR, MESSAGE_ID_SOFT_TRIGG);
    PutInt(&pkg, type);
    PutInt(&pkg, 0);

    PutChar(&pkg, 1);
    PutChar(&pkg, 0);
    PkgEnd(&pkg);

    printf("send soft trigger cmd\n");
    s.Send((char *)pkg.buffer, pkg.pkgSize);
}


static void SendCmd_Reg(CBlockSocket s)
{
    unsigned char buffer[128];
    struct Packet pkg;
    InitPkg(&pkg, buffer, 128, little_endian);

    int type = CreateType(SYS_TYPE_PARK, PROTOCOL_MAJOR, PROTOCOL_MINOR, MESSAGE_ID_GET_REGINFO);
    PutInt(&pkg, type);
    PutInt(&pkg, 0);

    PkgEnd(&pkg);

    printf("send get reg cmd\n");
    s.Send((char *)pkg.buffer, pkg.pkgSize);
}


static void SendCmd_GetSimpleWList(CBlockSocket s)
{
    unsigned char buffer[128];
    struct Packet pkg;
    InitPkg(&pkg, buffer, 128, little_endian);

    int type = CreateType(SYS_TYPE_PARK, PROTOCOL_MAJOR, PROTOCOL_MINOR, MESSAGE_ID_GET_SIMPLE_WLIST);
    PutInt(&pkg, type);
    PutInt(&pkg, 0);

    PkgEnd(&pkg);

    printf("send get simple wlist cmd\n");
    s.Send((char *)pkg.buffer, pkg.pkgSize);
}



static void SendCmd_SetSimpleWList(CBlockSocket s)
{
    int count = 2;
    struct HA_SimpleWList wList[2];
   
    strcpy(wList[0].plate, "川A12345");
    wList[0].year = 2016;
    wList[0].month = 5;
    wList[0].day = 6;

    strcpy(wList[1].plate, "川A12348");
    wList[1].year = 2016;
    wList[1].month = 5;
    wList[1].day = 6;


    //封包发送
    unsigned char buffer[128];
    struct Packet pkg;
    InitPkg(&pkg, buffer, 128, little_endian);

    int type = CreateType(SYS_TYPE_PARK, PROTOCOL_MAJOR, PROTOCOL_MINOR, MESSAGE_ID_SET_SIMPLE_WLIST);
    PutInt(&pkg, type);
    PutInt(&pkg, 0);

    PutInt(&pkg, count);

    for(int i=0; i<count; i++)
    {
        PutString(&pkg, wList[i].plate, 16);
        PutUShort(&pkg, wList[i].year);
        PutUChar(&pkg, wList[i].month);
        PutUChar(&pkg, wList[i].day);
    }

    PkgEnd(&pkg);

    printf("send set simple wlist cmd\n");
    s.Send((char *)pkg.buffer, pkg.pkgSize);
}


static void ProcessHeartBeat(CBlockSocket s, const char* data, int size)
{
    unsigned char buffer[128];
    struct Packet pkg;
    InitPkg(&pkg, buffer, 128, little_endian);

    int type = CreateType(SYS_TYPE_PARK, PROTOCOL_MAJOR, PROTOCOL_MINOR,MESSAGE_ID_HEART_BEAT);
    PutInt(&pkg, type);
    PutInt(&pkg, 0);
    PkgEnd(&pkg);

    printf("get heart beat\n");

    printf("%.8x\n", *(int*)data);

    s.Send((char *)pkg.buffer, pkg.pkgSize);
}



static void ProcessReplyMessage(const char* data, int dataSize)
{
    struct Packet packet, *pkg;
    pkg = &packet;

    InitPkg(pkg, (unsigned char*)data, dataSize, little_endian);

    int type, length;
    int reply_message;
    int reply_code;

    GetInt(pkg,&type);
    GetInt(pkg,&length);
    GetInt(pkg, &reply_message);
    GetInt(pkg, &reply_code);

    int sys_type, major, minor, msg_id;

    GetType(type, &sys_type, &major, &minor, &msg_id);

    switch(reply_message)
    {
    case MESSAGE_ID_GET_SIMPLE_WLIST:
        ProcessGetSimpleWList(data, dataSize);
        break;
    case MESSAGE_ID_GET_REGINFO:
        ProcessGetRegInfo(data, dataSize);
        break;
    default:
        break;
    }
}




static void ParsePacket(const CBlockSocket& s, const char* data, int dataSize)
{
    int type = *(int*)data; //小端
    int sys_type;
    int major;
    int minor;
    int msg_type;

    GetType(type, &sys_type, &major, &minor, &msg_type);

    //printf("msg type = %d\n", msg_type);

    switch (msg_type)
    {
    case MESSAGE_ID_HEART_BEAT:
        ProcessHeartBeat(s, data, dataSize);
        break;
    case MESSAGE_ID_REPLY:
        ProcessReplyMessage(data, dataSize);
        break;
    case MESSAGE_ID_CAR:
        //ProcessCarPacket(s, data, dataSize);
        break;
    case MESSAGE_ID_LIVE_IMAGE:
    case MESSAGE_ID_HASX_LIVE_IMAGE:
        //ProcessLiveImage(data, dataSize);
        break;
    case MESSAGE_ID_SIMPLE_CARINFO:
        ProcessSimpleCarPacket(s, data, dataSize);
        break;
    default:
        break;
    }
}


void* WorkThread(void* arg)
{

    unsigned int port = *(unsigned int*)arg;

    CBlockSocket socket;
    int ret = socket.ConnectToHost(g_ip, port);
    if (ret == -1)
    {
        printf("can't connect to cam[ip:%s, port:%d]\n", g_ip.c_str(), port);
        return 0;
    }

    printf("connectted to cam, port = %d\n",  port);
    
    char* buf = new char[1024*1024*2];
    
    while (1)
    {
        bool canRead = socket.CheckReadability();
        if (!canRead)
        {

           usleep(50*1000);
           continue;
        }

        int tl[2];

        //接收TLV中的前8个字节
        ret = socket.Recv((char*)tl, 8);
        if (ret == -1)
        {
            printf("recv tl error, port:%d\n", port);
            continue;//break
        }

        //tl[1]为包长度信息
        if (tl[1] > 0)
        {
            ret = socket.Recv(buf+8, tl[1]);
            if (ret == -1)
            {
                printf("recv value error, func:%s\n", __FUNCTION__);
                continue;//break
            }
        }

        //解包
        memcpy(buf, (char*)tl, 8);
        ParsePacket(socket, buf, tl[1]+8);
    }

    delete[] buf;

    return 0;
}


//接收简单车牌信息
void* WorkThread_HA(void* arg)
{

	CBlockSocket socket = *(CBlockSocket *)arg;
    char* buf = new char[1024*1024*2];

    time_t t = time(0);

    while (1)
    {
        if (time(0)-t >= 5)
        {
            //测试
            //SendCmd_Reg(socket);

            //SendTriggerCmd(socket);

            //SendCmd_GetSimpleWList(socket);

            //SendCmd_SetSimpleWList(socket);

            t = time(0);
        }

        bool canRead = socket.CheckReadability();
        if (!canRead)
        {
            usleep(50*1000);
            continue;
        }

        int tl[2];

        //接收TLV中的前8个字节
        int ret = socket.Recv((char*)tl, 8);
        if (ret == -1)
        {
            printf("recv tl error\n");
            break;
        }

        //tl[1]为包长度信息
        if (tl[1] > 0)
        {   
            ret = socket.Recv(buf+8, tl[1]);
            if (ret == -1)
            {
                printf("recv value error, func:%s\n", __FUNCTION__);
                break;
            }
        }

        //解包
        memcpy(buf, (char*)tl, 8);
        ParsePacket(socket, buf, tl[1]+8);
    }

    delete[] buf;

    return 0;
}


int button_is_down(int buttons_fd)
{
	static char buttons[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};
	static char current_buttons[8];
	int key_4_6 = 0;
	int count_of_changed_key = 0;
	int i;
	//printf("------->%d\n",__LINE__);
	if (read(buttons_fd, current_buttons, sizeof(current_buttons)) != sizeof(current_buttons)) {
		//perror("read buttons:");
		return -1;
	}
	//printf("------->%d\n",buttons[7]);

	for (i = 0, count_of_changed_key = 0; i < 8; i++) {
		if (buttons[i] != current_buttons[i]) {
			buttons[i] = current_buttons[i];
			printf("%skey %d is %s\n", count_of_changed_key? ", ": "", i, buttons[i] == '0' ? "up" : "down");
			if(i == 4 || i == 6){
				if(i == 4)
					key_4_6 = 4;
				else if((key_4_6 == 4) && (i == 6)) 
					key_4_6 = 4+6;
				else
					key_4_6 = 6;
			}
			if((key_4_6 == 0) && (buttons[i] != '0'))
				return i;
			count_of_changed_key++;
		}
	}
	if(key_4_6 > 0)
		return key_4_6;
	
	return -1;
}

int flag0 = 1;
int flag1 = 1;


int buttons_fd = NULL;

void* button_thread(void* arg)
{
	CBlockSocket **socket_array;
	socket_array = (CBlockSocket**)arg;

	if(buttons_fd == NULL){
		buttons_fd = open("/dev/buttons", O_RDONLY);
		if (buttons_fd < 0) {
			perror("open device buttons");
		}
	}

	int buttonNum = -1;
	while(1){
		buttonNum = button_is_down(buttons_fd); 
		printf("buttonNUm == %d\n",buttonNum);
		if((buttonNum == 7) && (flag0 == 1)){
			printf("7in\n");
				//flag = 0;
			for(int i=0; i < 2; i++)
				if((socket_array[i] != NULL) && (socket_array[i]->type == 0)){
					printf("socket 0 OK\n");
					SendCmd_Trigger(*socket_array[i]);
				}
		}
		else if((buttonNum == 6 || buttonNum == 6+4) && (flag0 == 0)){
			flag0 = 1;
			printf("\n6out\n");
			GetAuth(PlateId, 6);
		}

		if((buttonNum == 5) && (flag1 == 1)){
			printf("5in\n");
			//flag = 0;
			for(int i=0; i < 2; i++)
				if((socket_array[i] != NULL) && (socket_array[i]->type == 1)){
					printf("socket 1 OK\n");
					SendCmd_Trigger(*socket_array[i]);
				}
		}
		else if((buttonNum == 4 || buttonNum == 6+4) && (flag1 == 0)){
			flag1 = 1;
			printf("\n4out\n");
			GetAuth(PlateId, 4);
		}


		
	}

}


void* baidu_thread(void* arg)
{
	static int heart_time = 60;
	int count = 0;
	char buffer[4096] = {0};
	while(1)
	{
		ToServerHeartBeat(buffer,0);
		printf("buffer_in == %s\n",buffer);
		char *p = strstr(buffer,"\"interval\":");
		if(p != NULL){
			p += strlen("\"interval\":");
			char *p_end = strstr(p,",");
			if(p_end != NULL){
				*p_end = 0;
				if(heart_time != atoi(p))
					heart_time = atoi(p);
				printf("heart_time = %d\n", heart_time);
			}
		}
		ToServerHeartBeat(buffer,1);
		printf("buffer_out == %s\n",buffer);
		p = strstr(buffer,"\"need_action\":");
		if(p != NULL){
			p += strlen("\"need_action\":");
			printf("check up: %c\n", *p);
			if(*p > '0'){
				close(buttons_fd);
				server.Close();
				system("./re_start.sh");
			}
		}
		sleep(heart_time);
		if(buffer[0] == 0)
			count++;
		else
			count = 0;
		buffer[0] = 0;
		if(count == 10)
			system("reboot");
	}
}

int init_all()
{
	int cout = 0;
	while(init_curl() < 0){
		cout++;
		sleep(10);
		if(cout == 10)
			system("reboot");
		printf("iiiiiii\n");
	}
	return 0;
}

int main()
{
    pthread_t th[2];
	init_all();


    unsigned int ports[2];
    ports[0] = 20000; //实时画面端口
    ports[1] = 9527; //抓拍数据端口

    int listenPort = 20001;

    printf("listen port = %d\n", listenPort);


	CBlockSocket *socket_array[10] = {0};
	pthread_t button_thread_id;
	pthread_create(&button_thread_id, NULL, button_thread, (void *)socket_array);

	pthread_t baidu_id;
	pthread_create(&baidu_id, NULL, baidu_thread, (void *)socket_array);

    int ret = server.TCPServer(listenPort);

	int i = 0;
	while(1){

		CBlockSocket *socket = new CBlockSocket;
		socket->type = -1;
	    ret = server.Accept(*socket);
		
		if(ret < 0 || (socket->type == -1))
			continue;
		printf("type== %d\n", socket->type);
	    pthread_t simpleTh;
	    int status = pthread_create(&simpleTh, NULL, WorkThread_HA, (void *)socket);
		if(status != 0)
		{
			perror("pthread_create error");
		}
 		pthread_detach(simpleTh);
		//socket_array[i++] = socket;
		//sleep(1);
		//SendCmd_Trigger(*socket);
	}

    return 0;
}


