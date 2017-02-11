#ifndef PKG_VEHICLE_H
#define PKG_VEHICLE_H


#define MAX_IMG_NUM 4
#define MAX_VIDEO_NUM 2
#define MAX_PLATE_IMG_NUM 2
#define MAX_FACE_IMG_NUM 32

/*
* @brief   车辆信息包
*/
struct pkg_vehicle {
	unsigned int sequence_id; // 包序号, 递增
	unsigned int realtime_data; // -
	char packet_id[64]; // - 包ID

	char device_id[32];	// 设备编号
	char addr_id[32];	// 地点编号
	char direction_id[32]; // 方向编号
	char camera_id[32];	// 相机编号
	char lane_id[32];	// 车道编号
	char addr_name[96];	// 地点名称
	char direction_name[96]; // 方向名称

	// 车牌, 车牌坐标, 车牌置信度
	char plate[16];	// 车牌号码, utf8编码
	unsigned short plate_x; // 车牌在画面中的左上角x坐标
	unsigned short plate_y; // 车牌在画面中的左上角y坐标
	unsigned short plate_w;	// 车牌在画面中的像素宽度
	unsigned short plate_h; // 车牌在画面中的像素高度
	int plate_confidence;	// - 置信度

	unsigned char plate_color; // 车牌颜色 // 0-蓝色 1-黑色 2-黄色 3-白色 4-绿色 5-红色 6-灰色 7-紫色(KISE)
	unsigned char vehicle_style; // - 车辆类型
	unsigned char vehicle_color; // - 车身颜色
	unsigned char triger_type; // - 触发类型

	int redlight_begin_sec;
	int redlight_begin_usec;

	unsigned int system_type;
	unsigned int vio_type;

	unsigned short vehicle_speed;
	unsigned short limit_speed;

	int recoImageIndex;
	unsigned short vehicle_x[4];
	unsigned short vehicle_y[4];
	unsigned short vehicle_w[4];
	unsigned short vehicle_h[4];

	unsigned short red_lamp_x[4];      // 4方向灯区域
	unsigned short red_lamp_y[4];
	unsigned short red_lamp_w[4];
	unsigned short red_lamp_h[4];
	unsigned short vio_red_lamp_index; // 大于等于0时为当前闯红灯对应灯索引
	unsigned short direction;           // 车辆行驶方向
	char reserved[56];

	unsigned char img_num; // 本次携带的图片数目
	unsigned char video_num; // 本次携带的视频片段数目
	unsigned char plate_img_num; // 本次携带的车牌图片数目
	unsigned char face_num; // 本次携带的人脸图片数目

	unsigned int img_len[MAX_IMG_NUM]; // 对应每张图片的长度
	unsigned int video_len[MAX_VIDEO_NUM]; // 对应每段视频数据的长度
	unsigned int plate_img_len[MAX_PLATE_IMG_NUM]; // 对应每张车牌图片的长度
	unsigned int face_img_len[MAX_FACE_IMG_NUM]; // 对应每张人脸图片的长度

	char img_format[8];	// 图片的格式, 例如 "jpg"
	char video_format[8]; // 视频格式，例如 "avi"
	char plate_img_format[8]; // 车牌图片格式，例如 "jpg"
	char face_img_format[8]; // 人脸图片格式，例如 "jpg"

	int img_time_sec[MAX_IMG_NUM];
	int img_time_usec[MAX_IMG_NUM];

	int video_start_sec[MAX_VIDEO_NUM];
	int video_start_usec[MAX_VIDEO_NUM];
	int video_end_sec[MAX_VIDEO_NUM];
	int video_end_usec[MAX_VIDEO_NUM];

	unsigned short face_x[MAX_FACE_IMG_NUM];
	unsigned short face_y[MAX_FACE_IMG_NUM];
	unsigned short face_w[MAX_FACE_IMG_NUM];
	unsigned short face_h[MAX_FACE_IMG_NUM];

	unsigned char* img[MAX_IMG_NUM];
	unsigned char* video[MAX_VIDEO_NUM];
	unsigned char* plate_img[MAX_PLATE_IMG_NUM];
	unsigned char* face_img[MAX_FACE_IMG_NUM];
};

#endif