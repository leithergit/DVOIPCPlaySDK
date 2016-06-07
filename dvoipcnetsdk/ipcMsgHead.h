
/**
*  COPYRIGHT NOTICE 
*  Copyright (c) 2015, Diveo Technology Co.,Ltd (www.diveo.com.cn)
*  All rights reserved.
*
*  @file        ipcMsgHead.h
*  @author      luo_ws(luo_ws@diveo.com.cn)
*  @date        2015/08/13 09:34
*
*  @brief       IPC通信协议相关的数据结构定义
*  @note        无
*
*  @version
*    - v0.16    2015/08/13 09:34    luo_ws 
*/

#ifndef DVO_IPC_MSG_HEAD_H_
#define DVO_IPC_MSG_HEAD_H_


#include "dvoipcnetsdk_def.h"



//////////////////////////////////////////////////////////////////////////

typedef unsigned char  	    u8;	/**< UNSIGNED 8-bit data type */
typedef unsigned short 	    u16;/**< UNSIGNED 16-bit data type */
typedef unsigned int   	   	u32;/**< UNSIGNED 32-bit data type */
typedef UINT64           	u64;/**< UNSIGNED 64-bit data type */

typedef u8      U8;
typedef u16     U16;
typedef u32     U32;
typedef u64     U64;

//////////////////////////////////////////////////////////////////////////


#define		NET_TYPE_DISCOVERY_SUB_SEARCH_DEVICE_ACK        (0x0002)    //搜索设备命令响应
#define		NET_TYPE_DISCOVERY_SUB_MODIFY_MAC_ACK			(0x0004)    //修改MAC地址响应
#define		NET_TYPE_DISCOVERY_SUB_MODIFY_IP_ACK			(0x0006)    //修改IP地址响应

//////////////////////////////////////////////////////////////////////////

#define CHN_MAX     4


#define     APP_COM_NUM_CHN_MAX   4

//typedef enum
//{
//    VDEC_H264 = 0,
//    ADEC_G711U = 1,
//    ADEC_HISG711A = 2,
//    VDEC_HISH264 = 3,
//    ADEC_HISG711U = 4,
//    ADEC_HISADPCM = 5,
//    VDEC_MJPEG = 6,
//}DVO_DEC_TYPE;


#pragma pack(4)

typedef struct tagloginfo{
    int		    state;		        //0：成功 1：失败。
    char		dev_type[64];       //设备类型，字符串。
    char		dev_name[64];       //设备名，字符串。
    char		dev_id[64];	        //设备ID，字符串。
    char		hw_version[32];     //设备硬件版本，字符串。
    char		sdk_sw_version[32]; //设备软件SDK版本，字符串。
    char		app_sw_version[32]; //设备应用软件版本，字符串。
    char		rev1[32];
}app_net_tcp_sys_logo_info_t;


typedef struct{
    int		    chn;                //视频输入通道号，取值0~MAX-1.
}app_net_tcp_com_chn_t;


typedef struct{
    int         tm_sec;         	//* seconds *
    int         tm_min;         	//* minutes *
    int         tm_hour;       	 	//* hours *
    int         tm_mday;        	//* day of the month *
    int         tm_mon;         	//* month *
    int         tm_year;        	//* year *
    int         tm_wday;        	//* day of the week *
    int         tm_yday;        	//* day in the year *
    int         tm_isdst;       	//* daylight saving time *
}app_net_tcp_sys_time_t;


typedef struct{
    u32			chn;                //视频输入通道号，取值0~MAX-1.
    u32			schn;               //码流编号：0:主码流，1：子码流，2：第三码流。
    u32			rev[2];
}app_net_tcp_com_schn_t;


//申请流 VideoStream
typedef struct{
    U32			chn;                //视频输入通道号，取值0~MAX-1.
    U32			stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
    U32			enable;             //0:停止发送，1：开始发送。
    U32			nettype;            //0:tcp,1:udp.
    U32			port;               //udp port.
    U32			rev[3];
}app_net_tcp_enc_ctl_t;

typedef struct{
    U32			chn;                //视频输入通道号，取值0~MAX-1.
    U32			stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
    U32			state;              //0:成功，1：失败。
    U32			rev;
}app_net_tcp_enc_ctl_ack_t;

//IPC 返回视频流包头
typedef struct{
    U32		    chn;	            //视频输入通道号，取值0~MAX-1.
    U32		    stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
    U32		    frame_type;         //帧类型，范围参考APP_NET_TCP_STREAM_TYPE
    U32		    frame_num;          //帧序号，范围0~0xFFFFFFFF.
    U32		    sec;	            //时间戳,单位：秒，为1970年以来的秒数。
    U32		    usec;               //时间戳,单位：微秒。
    U32		    rev[2];
}app_net_tcp_enc_stream_head_t;


typedef struct{
    U32			chn;                //视频输入通道号，取值0~MAX-1.
    U32			type;               //OSD序号，取值：0,时间 1 字符串
    U32			rev[2];
}app_net_tcp_enc_chn_t;


typedef enum{
    APP_NET_TCP_OSD_AREA_UPPER_LEFT = 1,//左上。
    APP_NET_TCP_OSD_AREA_UPPER_RIGHT,	//右上。
    APP_NET_TCP_OSD_AREA_BOTTOM_LEFT,   //左下。
    APP_NET_TCP_OSD_AREA_BOTTOM_RIGHT,  //右下。
}APP_NET_TCP_OSD_AREA;


//typedef struct{
//    u8		    b;                  //b分量，取值范围0~255.
//    u8		    g;                  //g分量，取值范围0~255.
//    u8		    r;                  //r分量，取值范围0~255.
//    u8		    a;                  //透明度，取值范围0~255.
//}pm_color_t;

typedef struct{
    u8		    b;                  //b分量，取值范围0~255.
    u8		    g;                  //g分量，取值范围0~255.
    u8		    r;                  //r分量，取值范围0~255.
    u8		    a;                  //透明度，取值范围0~255.

}app_net_tcp_enc_color_t;

typedef app_net_tcp_enc_color_t app_net_tcp_osd_color_t;
typedef app_net_tcp_enc_color_t pm_color_t;


typedef struct{
    U32         chn;                //视频输入通道号，取值0~MAX-1.
    //u32       sn;                 //OSD序号，取值：0,1,2，代表第几路OSD。
    U32         enable;             //OSD显示开关，1：显示，0：关闭。
    //u32       area;               //osd位置，参考APP_NET_TCP_OSD_AREA
    U32         x_edge;             //x方向边距。取值：0-720.
    U32         y_edge;             //y方向边距。取值：0-575	
    pm_color_t  color;              //OSD颜色，参考app_net_tcp_osd_color_t结构。
    U32         size;               //字体大小。取值：16~60
    U32         type;               //类型 0：时间，1：自定义字符串。
    U32         len;                //添加字符长度。
    U32         outline;            //是否勾边 0：不勾边，1：勾边
    U32         rev[7];
    //char      tag[64];            //添加字符内容。最大长度64字节。
    char        tag[32];
}app_net_tcp_osd_cfg_t;


typedef struct{
    u32			chn;//视频输入通道号，取值0~MAX-1.
    u32         stream;
    u32			rev[2];
}app_net_tcp_custom_osd_zone_chn_t;

typedef struct{
    u32		    chn;                //视频输入通道号，取值0~MAX-1.
    u32	        stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
    u32		    enable;             //OSD显示开关，1：显示，0：关闭。
    u32		    x_edge;             //x方向左边距。取值：主码流宽度
    u32		    y_edge;             //y方向上边距。取值：主码流高度	
    u32		    size;               //字体大小。取值：16~60
    u32 	    max_line_num;       //自定义OSD区域最大显示行数
    u32         max_text_len;       //每行最大字符长度
    u32		    rev[8];
}app_net_tcp_custom_osd_zone_para_t;


typedef struct{
    u32		    chn;                //视频输入通道号，取值0~MAX-1.
    u32	       	stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
    u32			enable;             //OSD显示开关，1：显示，0：关闭。
    u32			x_edge;             //x方向左边距。取值：主码流宽度
    u32			y_edge;             //y方向上边距。取值：主码流高度	
    u32			size;               //字体大小。取值：16~60
    u32			rev[10];
}app_net_tcp_custom_osd_zone_cfg_t;

typedef struct{
    u32	        chn;                //视频输入通道号，取值0~MAX-1.
    u32	        stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
    u32         max_line_num;       //自定义OSD区域最大显示行数,0：为设置失败
    u32         max_text_len;       //每行最大字符长度
    u32	        rev[4];
}app_net_tcp_custom_osd_zone_cfg_ack_t;

typedef struct{
    u32         chn;                //视频输入通道号，取值0~MAX-1.
    u32	        stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
    u32         row_no;             //行号
    u32	        rev[5];
}app_net_tcp_custom_osd_line_chn_t;

typedef struct{
    u32         chn;                //视频输入通道号，取值0~MAX-1.
    u32         stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
    u32         row_no;             //行号 从0开始,不能超过app_net_tcp_custom_osd_zone_cfg_ack_t.max_line_num
    pm_color_t  color;              //字体颜色
    u32         outline;            //是否沟边
    u32         rev[7];
}app_net_tcp_custom_osd_line_cfg_t;

typedef struct{
    u32		    chn;                //视频输入通道号，取值0~MAX-1.
    u32	        stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
    u32		    row_no;             //行号 从0开始,不能超过app_net_tcp_custom_osd_zone_cfg_ack_t.max_line_num
    u32		    length;             //字符长度
    u8          text[256];          //不超过255个字符
    u32         set_color;          //是否设置颜色 0:不设置 1:设置
    u32         outline;            //是否勾边 0：不勾边，1：勾边
    pm_color_t  color;              //字体颜色
    u32		    rev[5];
}app_net_tcp_custom_osd_line_data_t;


typedef struct
{
    U32         lines;		        // 要设置OSD文本的行数
    U32         rev[8];
    app_net_tcp_custom_osd_line_data_t*	pLineArray; // OSD行数组指针
    /* OsdArray[1~MaxLine-1];1~MaxLines-1,MaxLines可根据OSD参数max_line_num得出，pLineArray中的一个元素为一行OSD文本，文本的行数必须与lines值相同*/
}app_net_tcp_custom_osd_line_data_array_t;


typedef enum {
    APP_NET_TCP_ENC_FMT_FR_NONE = 0,    //无变化
    APP_NET_TCP_ENC_FMT_FR_H = (1 << 0),//镜像
    APP_NET_TCP_ENC_FMT_FR_V = (1 << 1),//翻转
    APP_COM_ENC_FMT_FR_ALL,
}APP_COM_ENC_FMT_FR; //镜像翻转取值，逻辑或。

typedef struct {
    u32	        enc_mode;           //编码尺寸，取值参考APP_NET_TCP_COM_VIDEO_MODE
    u32	        enc_fps;            //编码帧率，取值1~30。
    u32	        rev[6];
}app_net_tcp_enc_fmt_base_t;

typedef struct {
    u8		    idr_interval;       //I帧间隔，取值1~20.
    u8		    profile;            //H264编码profile类型，0：main profile 1：base profile.
    u8		    brc_mode;           //编码模式，0：CBR，1：VBR	。
    u8		    rev1;
    u32		    maxbps;		        //码率，取值64000~8000000。CBR 设置 maxbps
    u32		    minbps;		        //码率，取值64000~8000000。
    u32		    rev2[6];
}app_net_tcp_enc_h264_base_t;

typedef struct {
    u32		    quality;            //图像质量，取值：1~100.
    u32		    rev[7];
}app_net_tcp_enc_jpeg_base_t;

typedef struct{
    u32			chn;                //视频输入通道号，取值0~MAX-1.
    u32			stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
    u32			enc_type;           //编码类型：0：H264，1：JPEG.
    u32			rev[5];
    app_net_tcp_enc_fmt_base_t	fmt;    //编码格式定义，参考结构说明。
    app_net_tcp_enc_h264_base_t	h264;   //H264属性设置，参考结构说明。
    app_net_tcp_enc_jpeg_base_t	jpeg;   //jpeg属性设置，参考结构说明。
}app_net_tcp_enc_info_t;

typedef enum{
    APP_NET_TCP_PM_COLOR_BLACK = 0,
    APP_NET_TCP_PM_COLOR_RED,
    APP_NET_TCP_PM_COLOR_BLUE,
    APP_NET_TCP_PM_COLOR_GREEN,
    APP_NET_TCP_PM_COLOR_YELLOW,
    APP_NET_TCP_PM_COLOR_MAGENTA,
    APP_NET_TCP_PM_COLOR_CYAN,
    APP_NET_TCP_PM_COLOR_WHITE,
}APP_NET_TCP_PM_COLOR_TYPE;//PM颜色。


typedef struct{
    u32			chn;                //视频输入通道号，取值0~MAX-1.
    u32			enable;             //0:关闭，1：开启。关闭将清除所有设定的PM.
    u32			x;                  //PM的X坐标，为占视频尺寸的百分比，范围0~100.
    u32			y;                  //PM的Y坐标，为占视频尺寸的百分比，范围0~100.
    u32			w;                  //PM的宽度，为占视频尺寸的百分比，范围0~100.
    u32			h;                  //PM的高度，为占视频尺寸的百分比，范围0~100.
    pm_color_t	color;              //PM颜色,取值参考pm_color_t结构。
    int			rev;                //保留。
}app_net_tcp_pm_cfg_t;



//图像处理命令
//图像处理控制命令的命令字CmdType 为0x0003
typedef struct{
    int		    chn;                //视频输入通道号，取值0~MAX-1.
    int		    brightness;         //亮度，取值：0~255.
    int		    saturation;         //饱和度，取值：0~255.
    int		    contrast;           //对比度，取值：0~255.
    int		    sharpness;          //锐度，取值：0~255.
    int		    hue;                //色调，取值：0~255.
    int		    rev[2];
}app_net_tcp_img_color_t;


//曝光获取请求
typedef struct{
    U32			chn;                //视频输入通道号，取值0~MAX-1.
    U32			mode;               //曝光模式。范围参考APP_NET_TCP_IMG_EXP_MODE
}app_net_tcp_com_chn_t_puguang;


typedef enum{
    APP_NET_TCP_IMG_EXP_AUTO = 0,   //自动曝光
    APP_NET_TCP_IMG_EXP_MANUAL = 1, //手动曝光
    APP_NET_TCP_IMG_EXP_MODE_ALL,
}APP_NET_TCP_IMG_EXP_MODE;

typedef enum {
    APP_NET_TCP_ESHUTTER_1_SEC = 0, // 1 sec *
    APP_NET_TCP_ESHUTTER_10_SEC,    // 1/10 sec *
    APP_NET_TCP_ESHUTTER_12_SEC,    // 1/12 sec *
    APP_NET_TCP_ESHUTTER_15_SEC,
    APP_NET_TCP_ESHUTTER_20_SEC,
    APP_NET_TCP_ESHUTTER_25_SEC,
    APP_NET_TCP_ESHUTTER_30_SEC,
    APP_NET_TCP_ESHUTTER_50_SEC,
    APP_NET_TCP_ESHUTTER_60_SEC,
    APP_NET_TCP_ESHUTTER_100_SEC,
    APP_NET_TCP_ESHUTTER_120_SEC,
    APP_NET_TCP_ESHUTTER_240_SEC,
    APP_NET_TCP_ESHUTTER_480_SEC,
    APP_NET_TCP_ESHUTTER_960_SEC,
    APP_NET_TCP_ESHUTTER_1024_SEC,
    APP_NET_TCP_ESHUTTER_COUNT,
}APP_NET_TCP_ESHUTTER_E;

typedef struct{
    int		    chn;                //视频输入通道号，取值0~MAX-1.
    int		    mode;               //曝光模式。范围参考APP_NET_TCP_IMG_EXP_MODE
    int		    shutter;            //取值参考APP_NET_TCP_ESHUTTER_E。
    int		    gain;               //最大增益。范围：1~60
    int		    rev[16];

}app_net_tcp_img_exp_t;

/*
<6>图像白平衡参数获取
DataType:
　　0x0001
  CmdSubType：
  　　0x0006
    CmdPkt：
    　　typedef struct{
      　　	u32			chn;//视频输入通道号，取值0~MAX-1.
            U32			mode;//白平衡模式，范围参考APP_NET_TCP_IMG_WB_MODE
        }app_net_tcp_com_chn_t;
*/

typedef struct{
    int		    chn;                //视频输入通道号，取值0~MAX-1.
    int		    mode;               //白平衡模式，范围参考APP_NET_TCP_IMG_WB_MODE
    int		    rgain;              //R增益，取值范围：0~16000 
    int		    ggain;              //G增益，取值范围：0~16000
    int		    bgain;              //B增益，取值范围：0~16000
    int		    rev[3];
}app_net_tcp_img_wb_t;

//<8>白天黑夜模式获取
//DataType:
//　　0x0001
//CmdSubType：  
//　　0x0008
//CmdPkt：
//typedef struct{
//u32			chn; //视频输入通道号，取值0~MAX-1.
//}app_net_tcp_com_chn_t;
//AckPkt：
typedef struct{
    int		chn;                    //视频输入通道号，取值0~MAX-1.
    int		mode;                   //白天黑夜模式，0：关闭，1：启用，2：自动。
    int		rev[6];
}app_net_tcp_img_ircut_t;

//<9>视频模式
typedef enum {
    APP_NET_MIRROR_HORRIZONTALLY_VERTICALLY = 0,
    APP_NET_MIRROR_HORRIZONTALLY,
    APP_NET_MIRROR_VERTICALLY,
    APP_NET_MIRROR_NONE,
    APP_NET_MIRROR_ALL,
}APP_NET_MIRROR_MODE;

typedef struct{
    int         chn;                //视频输入通道号，取值0~MAX-1.
    u32         mirror_mode;        //翻转模式，APP_NET_MIRROR_MODE
    u32         rev[6];
}app_net_tcp_img_mode_t;



//////////////////////////////////////////////////////////////////////////

//
//音频相关的命令CmdType 为0x0006
//<1>音频输入码流控制（录像）
//DataType:
//　　0x0001
//CmdSubType：  
//　　0x0001
//CmdPkt：
typedef struct{
    U32			chn;                //音频输入通道，取值0~MAX-1.
    U32			enable;             //0：停止发送，1：开始发送。
    U32			rev[6];
}app_net_tcp_audio_ctl_t;
//AckPkt：
typedef struct{
    U32			chn;	            //音频输入通道，取值0~MAX-1.
    U32			state;              // 0:成功，1：失败。
}app_net_tcp_audio_ctl_ack_t;


typedef struct{
    int			chn;                //音频输入通道，取值0~MAX-1.
    int			enc_type;           //音频编码类型：0：711_a，1：711_u，2：726，3：AAC
    int			samplerate;         //采样率，参考APP_NET_TCP_AUDIO_AUDIO_FS。
    int			rev[5];
}app_net_tcp_audio_config_t;

typedef struct{
    int			chn;		        // 通道
    int	        enc_type;	        // 压缩类型：0：711_a，1：711_u，2：726，3：AAC
    int			nSampleBits;	    // 采样位数, 8 or 16 
    int			nSampleRate;	    // 采样频率  8000, 16000 ，参考APP_NET_TCP_AUDIO_AUDIO_FS。
    int			nBitRate;		    // 压缩码率
}app_net_tcp_audio_config_t_v2;


#define	APP_NET_TCP_SYS_NUM_IOIN_CHN_MAX    (1)
#define	APP_NET_TCP_SYS_NUM_VIN_CHN_MAX	    (1)
#define	APP_NET_TCP_SYS_NUM_NET_CARD_MAX	(2)
#define	APP_NET_TCP_SYS_NUM_DISK_MAX		(1)


//////////////////////////////////////////////////////////////////////////

//3>网络设置
//DataType:
//　　0x0001
//CmdSubType：  
//　　0x0003
//CmdPkt：
typedef struct {
    char		ipv4[30];           //IP，字符串。
    char		mac_addr[40];       //MAC，字符串。
    char		net_mask[30];       //MASK，字符串。
    char		gate_way[30];       //GATE WAY，字符串。
    char		dns[30];            //DNS，字符串。
}app_net_tcp_sys_ipv4_t;

typedef struct tagGetNetWork_REQ{
    int						chn;    // 网卡通道号，取值0~MAX-1.
    //int					card;   //0:有线网卡，1:无线网卡，系统网卡数量通过查询设备资源信息获取。
    int					    exit;   //0:网卡不存在，1：网卡存在，仅查询时使用。
    int						rev[6];
    app_net_tcp_sys_ipv4_t 	ipv4;
}app_net_tcp_sys_net_info_t;

//AckPkt：
/*typedef enum{
　　	APP_NET_TCP_STATE_SUCCESS = 0,
    　　	APP_NET_TCP_STATE_FAIL,
        　　}APP_NET_TCP_STATE;
*/

typedef struct tagRes{
    int result;
} GET_NETWORK_RESPONE;

//<4>网络查询
//DataType:
//　　0x0001
//CmdSubType：  
//　　0x0004
//CmdPkt：
typedef struct{
    U32		    chn;                //0:有线网卡，1:无线网卡。
}app_net_tcp_com_chn_t_net;


typedef struct{
    int 	    ip_include;         // 0:include ip, 1:not include ip.
    int		    rev[3];
}app_net_tcp_sys_default_t;

typedef struct{
    char        sn[32];             //序列号
    char        mac[40];            //MAC地址，字符串，字符的取值范围为：'0'-'9','A'-'F','a'-'f',':'，每两个字符表示一个8位二进制数的十六进制格式，用“:”分隔，例如：E2:02:6E:AD:17:19，
    char        rev[88];
}app_net_tcp_sn_mac_t;


//<13>设备资源查询//　0x000D
typedef struct{
    int	        vin_num;            //视频输入通道数
    int	        vout_num;           //视频输出通道数
    int	        ain_num;            //音频输入通道数
    int	        aout_num;           //视频输出通道数
    int         serial_num;         //串口通道数
    int	        ioin_num;           //IO报警输入通道数
    int	        ioout_num;          //IO报警输出通道数
    int	        netcard_num;        //网卡通道数
    int	        disk_num;           //SD卡通道数

    int	        vin_type[APP_COM_NUM_CHN_MAX];
    int	        vout_type[APP_COM_NUM_CHN_MAX];
    int	        ain_type[APP_COM_NUM_CHN_MAX];
    int	        aout_type[APP_COM_NUM_CHN_MAX];
    int	        serial_type[APP_COM_NUM_CHN_MAX];
    int	        ioin_type[APP_COM_NUM_CHN_MAX];
    int	        ioout_type[APP_COM_NUM_CHN_MAX];
    int	        netcard_type[APP_COM_NUM_CHN_MAX];
    int	        disk_type[APP_COM_NUM_CHN_MAX];
    int	        rev[40];
}app_net_tcp_cap_t;

//<15>设备信息查询
typedef struct {
    char		dev_type[64];       //设备类型，字符串。
    char		dev_name[64];       //设备名，字符串。
    char		dev_id[64];	        //设备ID，字符串。
    char		hw_version[32];     //设备硬件版本，字符串。
    char		sdk_sw_version[32]; //设备软件SDK版本，字符串。
    char		app_sw_version[32]; //设备应用软件版本，字符串。
    char		rev1[32];
}app_net_tcp_sys_dev_info_t;


//<16>设备信息设置 0x0010
typedef struct tag{
    char        dev_name[64];       //设备名，字符串
    char        rev[32];
}app_net_tcp_sys_dev_info_t_st;


typedef struct{
    u32		    chn;                //串口通道号，从0开始，到系统最大串口通道数减一。
    u32		    bitrate;            //波特率，范围参考APP_NET_TCP_SERIAL_BAUDRATE。
    u32		    parity;             //奇偶校验，范围参考APP_NET_TCP_SERIAL_BAUDRATE。
    u32		    len;                //数据长度，范围参考APP_NET_TCP_SERIAL_DLENGTH
    u32		    stop;               //停止位，范围参考APP_NET_TCP_SERIAL_STOPBIT
    u32		    rev[3];
}app_net_tcp_serial_cfg_t;

typedef struct{
    u32			type;           //0:虚拟，1：串口。
    u32			chn;            //if type is serial,chn is serial chn,else is video in chn.
    u32			protocl;        //1：pelco-d,2 ：pelco-p.
    u32			cmd1;           //refer to pelco-d.
    u32			cmd2;           //refer to pelco-d.
    u32			ps;	            //refer to pelco-d.
    u32			ts;	            //refer to pelco-d.
    u32			rev[9];
}app_net_tcp_ptz_t;

typedef struct{
    u32			chn;                //串口通道号，取值0~MAX-1.
    u32			len;                //数据长度，取值1~256
    u32			rev[6];
    char		data[256];          //数据内容。
}app_net_tcp_ptz_tran_t;


typedef struct {
    U32         part_num;	        //分区号：取值0,1,2,3。
    U64		    part_volume; 	    //分区容量，unit (M)
    U64		    part_free;		    //分区剩余，unit (M)
    U32		    part_state;		    //分区状态，0:正常 1:未格式化 2:异常.
    U32		    rev[9];
}app_net_tcp_sys_disk_part_t;

typedef struct {
    u32         chn;                //磁盘通道号，取值0~最大磁盘数量-1。
    u32         exit;               //0:磁盘不存在，1：存在，查询后需判断。
    u32         rev[6];
    app_net_tcp_sys_disk_part_t	 part[4]; //磁盘分区信息，最大支持4个分区，多余分区系统不使用。
}app_net_tcp_sys_disk_t;


typedef struct {
    u32		    chn;                //0:CVBS,1,HDMI.
    u32 		enable;             //0:关闭，1：使能。
}app_net_tcp_sys_net_misc_t_videoout;


typedef struct {
    u32         enable;             //0:关闭，1：打开
    char        addr[128];          //NTP服务器IP或域名
    u32         port;               //NTP服务器端口
    u32         inteval;            //同步间隔
    u32         set_time_zone;      //是否设置时区，0：不设置 1设置
    int         time_zone;
    char        rev[248];
}app_net_tcp_sys_ntp_para_t;

typedef struct {
    u32         alarm_id;
    u32         alarm_type;         //0-设备故障上报，1-设备故障恢复上报，2-设备事件上报
    u32         para_num;           //参数个数
    u32         alarm_para[4];      //设备相关参数
    u32         rev[256];
}app_net_tcp_sys_func_state_report_t;

typedef struct {
    char        func_list[1600];
    char        rev[256];
}app_net_tcp_sys_func_list_t;


typedef struct{
    u32         io_alarm_num;
    u32         rev[7];
}app_net_tcp_io_alarm_num_t;

typedef struct{
    u32         index;              //0-IO报警端口数量-1
    u32         rev[7];
}app_net_tcp_io_alarm_index_t;

typedef struct{
    u32         index;              //同上
    u32         active_level;       //0-开路 1-闭路
    u32         set_in_out;         //能否改变报警方向 0-不能 1-能；该字段为不能改方向时，设置IO报警方向就不出现在界面上
    u32         in_out;             //0-报警输入 1-报警输出
    u32         rev[12];
}app_net_tcp_io_alarm_attr_t;

typedef struct{
    u32         state;              //0-不启用，1-启用
    u32         rev[7];
}app_net_tcp_set_io_alarm_state_t;

typedef struct{
    u32         index;              //同上
    u32         active_level;       //0-开路 1-闭路
    u32         in_out;             //0-报警输入 1-报警输出
    u32         rev[13];
}app_net_tcp_set_io_alarm_attr_t;


typedef struct{
    u32		    enable;             //1:开启，0：关闭。//默认关闭。
    u32		    interval;           //心跳间隔，单位100ms
    u32		    rev[2];
}app_net_tcp_sys_heart_t;


/// 报警消息类型
enum DVO_CALLBACK_MESSAGE_ALARM_TYPE
{
    DVO_MSG_ALARM_IOIN                  = 0x01,	//Ioin[0]报警
    DVO_MSG_ALARM_IOIN_NOTHING          = 0x02,	//Ioin[0]不报警
    DVO_MSG_ALARM_MOTION				= 0x03,	//MD[0]报警
    DVO_MSG_ALARM_NO_MOTION		        = 0x04,	//MD[0]不报警
    DVO_MSG_ALARM_IO_MOTION		        = 0x05,	//Ioin[0]报警 MD[0]报警 <=>Ioin[0]断路 MD[0]报警
    DVO_MSG_ALARM_IOIN_NO_MOTION	    = 0x06,	//Ioin[0]报警 MD[0]不报警<=> Ioin[0]断路 MD[0]不报警
    DVO_MSG_ALARM_NO_IOIN_MOTION	    = 0x07,	//Ioin[0]不报警 MD[0]报警 <=>Ioin[0]通路 MD[0]报警
    DVO_MSG_ALARM_ONLY_HEARTBEAT 	    = 0x08,	//Ioin[0]不报警 MD[0]不报警 <=>Ioin[0]通路 MD[0]不报警

    DVO_MSG_ALARM_TEMPR_CTRL_MOTION     = 0x0A, // 温控报警类型
    DVO_MSG_ALARM_TEMPR_CTRL_ERROR      = 0x0B, // 温控异常上报
};

typedef struct{
    int		    type;               // 报警消息类型。填充值DVO_CALLBACK_MESSAGE_ALARM_TYPE
    char        ioin[CHN_MAX];      // 0:无报警，1：IO报警
    char        md[CHN_MAX];        // 0:无移动，1：移动报警
    u32         dev_type;           // 485输入设备类型 0为ICPDAS DL-100TM485
    char        _485_alarm[16];     // 0:无报警，1：报警 对DL-100而言，第一位为摄氏温度报警，第二位为华氏温度报警，第三位为湿度报警，第四位为CO2报警
    u32         _485_data[16];      // 对DL-100而言，第一位为摄氏温度，第二位为华氏温度，第三位为湿度，第四位为CO2
    char        rev[428];
}app_net_tcp_sys_event_trig_state_t;



//////////////////////////////////////////////////////////////////////////
//5.事件相关命令CmdType 为0x0005

//IO报警输出设置
typedef struct{
    u32		    chn;                //报警输出通道号。取值0~MAX-1.
    u32		    state;              //输出状态，0：不报警，1：报警。
    u32 		rev[6];
}app_net_tcp_event_act_ioout_t;


//MD区域
typedef struct{
    u32		    x;                  //MD区域的X坐标，取值0~32。
    u32		    y;                  //MD区域的Y坐标，取值0~18。
    u32		    w;                  //MD区域的宽度，取值0~32。
    u32		    h;                  //MD区域的高度，取值0~18。
}app_net_tcp_com_rect_t;

typedef struct{
    u32	        chn;                //视频输入通道号，取值0~	MAX-1.
    u32		    rows;               //区域行数，范围1~64.
    u32		    cols;               //区域列数，范围1~64.
    int		    rev[5];
}app_net_tcp_event_trig_md_base_rc_t;


typedef struct{
    u32		    chn;                //视频输入通道号，取值0~	MAX-1.
    u32		    threshold;          //门限，取值0~100.
    u32		    sensitivity;        //灵敏度，取值0~100.
    u32		    rev[5];
    char	 	en[64 * 64 / 8];    //0:关闭，1：启用。0,1为bit,每一个BIT代表相应是否		开启。
}app_net_tcp_event_trig_md_t;


typedef struct{
    u32		    chn;                //报警输出通道号。取值0~MAX-1.
    u32         data_type;          //0：温度，1：湿度，2：CO2
}app_net_tcp_com_chn_485_t;

typedef struct{
    u32         data_type;          //0：温度，1：湿度，2：CO2
}app_net_tcp_485_osd_data_type_t;

typedef struct{
    u32		    chn;                //报警输出通道号。取值0~MAX-1.
    u32         data_type;          //0：温度，1：湿度，2：CO2
    int		    state;              //输出状态，0：不报警，1：报警。
    int 	    rev[5];
}app_net_tcp_event_act_485_t;


typedef struct{
    u32		    chn;                //视频输入通道号，取值0~	MAX-1.
    u32         data_type;          //0：温度，1：湿度，2：CO2
    int		    value[255];         //对温度，湿度，CO2而言，第1位是上限，第2位是下行
    int         status;             //0 有效 1无效
}app_net_tcp_485_alarm_para_t;


typedef struct{
    int         enable;             //0:启动,1:关闭.
    int         dev_type;           //0-ICPDAS DL-100TM485
    int         dev_id;             //设备ID
    int         interval;           //采集时间间隔
    char rev[512];
}app_net_tcp_485_dev_t;

typedef struct{
    u32         data_type;          //0：温度，1：湿度，2：CO2
    int         row;                //显示行
    int         col;                //显示列
    char        prefix[32];         //显示前缀
    char        suffix[32];         //显示后缀
    int         dev_type;
    int         osd;
    char rev[504];
}app_net_tcp_485_osd_para_t;


typedef struct{
    u32         dev_type;           //485输入设备类型 0为ICPDAS DL-100TM485
    u32         data[255];          //对于DL-100而言，第一位为温度，第二位为湿度
    u32         status;
}app_net_tcp_485_input_data_t;


// 透传,cmdmain=0x007,cmdsub=0x00A,0x00B
typedef struct{
    U32			chn;                //串口通道号，取值0~MAX-1. 
    U32			len;                //数据长度
    U32			rev[8];
}app_net_tcp_port_tran_head_t;//包含数据包头和数据内容两部分，数据紧跟app_net_tcp_port_tran_head_t头之后

// 透传,cmdmain=0x007,cmdsub=0x00B
typedef struct{
    U32			chn;                //串口通道号，取值0~MAX-1. 
    U32			rev[8];
}app_net_tcp_port_tran_chn_t;

//////////////////////////////////////////////////////////////////////////


typedef struct{
    u32         proto;              //wifi加密类型。0:open,1:wep,2:wpa,3:tkip.
    char        ssid[36];           //WIFI的SSID。
    char        passwd[68];         //wifi密码
}mw_misc_wifi_t;

typedef struct{
    int enable;                     //0:启动,1:关闭.
}mw_misc_ao_t;


typedef struct{
    u32         tempUuid;           //临时设备ID，由搜寻响应设备填写
    u32         mac_addr_state;     //0:无效，1：有效
    char		mac_addr[40];       //MAC，字符串
    char		ipv4[30];           //IP，字符串。
    char		net_mask[30];       //MASK，字符串。
    char		gate_way[30];       //GATE WAY，字符串。
    char        rev[110];
}multicast_dev_info_t;

typedef struct{
    u32         tempUuid;           //临时设备ID，由搜寻响应设备填写
    u32         mac_addr_state;     //0:无效，1：有效
    char		mac_addr[40];       //MAC，字符串
    char		ipv4[30];           //IP，字符串。
    char		net_mask[30];       //MASK，字符串。
    char		gate_way[30];       //GATE WAY，字符串。
    char        version[30];
    u32         http_port;
    char        rev[76];
}multicast_dev_info_t_v2;


typedef struct {
    int 	    ip_include;         // 0:include ip, 1:not include ip.
    int		    file_len;		    //文件总长度，单位：字节。
    int		    total_frame;	    //总包数。
    int		    cur_frame;	        //当前包序号，从1开始。
    int         crc;
    int		    rev[4];
}app_net_tcp_sys_update_head_t;

typedef struct{
	u32	        chn;                //视频输入通道号，取值0~MAX-1.
	u32	        stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
	u32         max_line_num;       //自定义OSD区域最大显示行数,0：为设置失败
	u32         max_text_len;       //每行最大字符长度
	u32	        rev[4];
}ack_pack_set_custom;



/*
 * 设置/获取功能型号的结构体
 *
 * 功能型号名称,具体定义由设备产品确定:
 * name=空，表示相机为普通相机，无特殊功能
 * name=OSD-IPC,支持OSD叠加功能
 * name=FC-IPC, 支持消防泵、喷淋泵状态叠加功能
 * name=TH-IPC, 支持环境中温度、湿度的监测功能
 * name=THC-IPC,支持环境中温度、湿度、二氧化碳的监测功能
 * name=CM-IPC, 支持走廊模式功能
*/
typedef struct
{
    char        name[128];	        //功能型号名称 
    char        rev[128];
}app_net_tcp_func_model_t;

//////////////////////////////////////////////////////////////////////////
//系统日志相关定义

// 日志文件下载,cmdmain=0x001,cmdsub=0x000E
// CmdPkt：
typedef struct {
    int log_type;
    int tm_year;
    int tm_mon;
    int tm_mday;
    int rev[5];
} app_net_tcp_sys_log_t;

// AckPkt：
typedef struct {
    int available;
    int log_size;
    int rev[5];
}app_net_tcp_sys_log_info_t;

//日志文件上传,cmdmain=0x001,cmdsub=0x0025
typedef struct{
    int         cur_frame;
    int         fram_size;
    int         rev[10];
}app_net_tcp_sys_log_download_head_t;


// 日志查询,cmdmain=0x001,cmdsub=0x0026
// CmdPkt：
typedef struct{
    U8					    log_type;	// 日志类型
    U8				        rev1[3];	// 保留 对齐
    app_net_tcp_sys_time_t  starttime;  // 开始时间
    app_net_tcp_sys_time_t  endtime;    // 结果时间
    U32					    rev[4];
}app_net_tcp_sys_log_query_para_t;


// 日志记录集上传,cmdmain=0x001,cmdsub=0x0027
typedef struct {
    U32			            item_num;	// 日志记录条数
    U32			            log_size;	// 本次记录集的数据长度
    U8			            finished;	// 1-上传完成，0-未传完
    U8			            rev1[3];	// 保留 对齐
    U32			            rev[4];		// 保留
}app_net_tcp_sys_log_info_head_t;

// 日志记录
typedef struct {
    U8					    log_type;	// 日志类型
    U8				        rev1[3];	// 保留 对齐
    app_net_tcp_sys_time_t  logtime;    // 日志记录的时间戳
    char				    user[32];   // 用户名
    U32					    context_size;//日志内容长度
    U32					    rev[4];
}app_net_tcp_sys_log_item_t;

/*
说明：日志记录包含数据包头和数据内容两部分，每条日志数据的内容context紧跟app_net_tcp_sys_log_item_t头之后，
如：app_net_tcp_sys_log_item_t+context。

日志记录集app_net_tcp_sys_log_info_head_t包含数据包头和数据内容两部分，日志数据紧跟app_net_tcp_sys_log_info_head_t头之后，如：
app_net_tcp_sys_log_info_head_t+app_net_tcp_sys_log_item_t+context1+app_net_tcp_sys_log_item_t+context2+app_net_tcp_sys_log_item_t+context3
*/
//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif