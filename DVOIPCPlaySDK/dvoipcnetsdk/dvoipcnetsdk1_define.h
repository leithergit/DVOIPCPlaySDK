/**
*  COPYRIGHT NOTICE 
*  Copyright (c) 2015, Diveo Technology Co.,Ltd (www.diveo.com.cn)
*  All rights reserved.
*
*  @file        dvoipcnetsdk1_define.h
*  @author      jag(luo_ws@diveo.com.cn)
*  @date        2015/11/10 10:00
*
*  @brief       支持上一个版本的数据结构
*  @note        无
*
*  @version
*    - v1.0.1.0    2015/11/11 10:00    jag 
*/

#ifndef DVO_IPC_NET_SDK_DEFFINE_H_
#define DVO_IPC_NET_SDK_DEFFINE_H_


#include "dvoipcnetsdk_def.h"


#define IP_LENGTH				32
#define	MAX_NAME_LEN			32
#define	MAX_PASS_LEN			16
#define INVALID_HANDLE          -1
#ifndef CHN_MAX
#define CHN_MAX 4
#endif




#define ERROR_INVALIDARG								0X08	// 无效的参数
#define ERROR_PWD										0x09
#define ERROR_USER										0x10
#define ERROR_CONNECT									0x08
//Error CODE begin
#define ERROR_INIT_MGR_INSTANCE							0x11    //网络管理实无实例化
#define ERROR_LOGIN_SERVER_ERROR_SOCKET_REQ				0x12	//登录ipc 服务请求失败
#define ERROR_CONN_STREAM_AV							0x13	//发送流连接失败
#define ERROR_CMD_CONN_NETDOWN							0x14	//命令socket 掉线
#define ERROR_LOGIN_RES									0x15    //recv Login RES state -1 or res none error
#define ERROR_BUF_TYPE                                  0x16	//Error get 的videodata
#define ERROR_OPEN_SOUND                                0x17	//打开语音失败
#define ERROR_CLOSE_SOUND                               0x18	//关闭语音失败
#define ERROR_GET_IPCM_FEATURE                          0x19    //获取ipc的特性出错
#define ERROR_SET_IPCM_FEATURE                          0x20	//设置ipc的特性出错
#define ERROR_FILENAME_UPGRADE                          0x21	//升级文件名出错
#define ERROR_NEW_MEMCORY                               0x22	//申请内存失败
#define ERROR_TO_STOP_UPGRADE                           0x23	//停止升级失败
#define ERROR_SET_REGISTER                              0x24	//注册报警失败
#define ERROR_SET_HEART                                 0x25	//设置心跳失败
#define ERROR_GET_INSTANCE_CONNIPC                      0x26	//连接实例CConnIPC对象失败或非法的user_Handle
#define ERROR_GET_SYS_TIME                              0x27	//获取系统时间失败
#define ERROR_GET_OSD_DATE                              0x28	//获取osddate失败
#define ERROR_GET_OSD_NAME                              0x29	//获取osdname失败
#define ERROR_GET_MOTION_SEND                           0x31	//获取motion 信息失败
#define ERROR_GET_MOTION_RECV                           0x32	//获取motion 信息接收失败
#define ERROR_SET_MOTION_SEND                           0x33	// set motion infosend error
#define ERROR_SET_MOTION_RECV                           0x34	//set motion info recv error
#define ERROR_GET_NET_WORK                              0x35	//get network recv error;
#define ERROR_GET_SEND_NET_WORK                         0x36	//send network recv error;
#define ERROR_SET_SNED_NET_WORK                         0x37	//send set network error
#define ERROR_SET_NET_WORK_RECV                         0x38	//set network recv error
#define ERROR_SET_CONF_RS232_SEND                       0x39 
#define ERROR_SET_CONF_RS232_RECV						0x40
#define ERROR_SEND_DATA_RS232_SEND                      0x41
#define ERROR_SEND_DATA_RS232_RECV_SEND                 0x42
#define ERROR_SEND_DISK_GET								0x43
#define ERROR_RECV_DIS_GET								0x44

#define ERROR_SET_STREAM_TYPE_RECV						0x45
#define ERROR_SET_STREAM_TYPE_SEND						0x46
#define ERROR_SET_CUSTOM_AREA_SEND						0x47
#define ERROR_SET_CUSTOM_AREA_RECV						0x48
#define ERROR_GET_CUSTOM_AREA_SEND						0x49
#define ERROR_GET_CUSTOM_AREA_RECV						0x50
#define ERROR_SET_CUSTOM_ROW_SEND						0x51
#define ERROR_SET_CUSTOM_ROW_RECV						0x52
#define ERROR_GET_CUSTOM_ROW_SEND						0x53
#define ERROR_GET_CUSTOM_ROW_RECV						0x54
#define ERROR_GET_CUSTOM_ROW_TEXT_SEND					0x55
#define ERROR_GET_CUSTOM_ROW_TEXT_RECV					0x56
#define ERROR_SET_CUSTOM_ROW_TEXT_SEND					0x57
#define ERROR_SET_CUSTOM_ROW_TEXT_RECV					0x58
#define ERROR_GET_FLEP_SEND								0x59
#define ERROR_GET_FLEP_RECV								0x61
#define ERROR_SET_FLEP_SEND								0x62
#define ERROR_SET_FLEP_RECV								0x63
#define ERROR_SET_VIDEO_OUT_MODE_SEND					0x64
#define ERROR_SET_VIDEO_OUT_MODE_RECV					0x65
#define ERROR_GET_VIDEO_OUT_MODE_SEND					0x66
#define ERROR_GET_VIDEO_OUT_MODE_RECV					0x67

#define ERROR_GET_TEMPRCTRL_ALARMOUT_SEND				0x68
#define ERROR_GET_TEMPRCTRL_ALARMOUT_RECV				0x69
#define ERROR_SET_TEMPRCTRL_ALARMOUT_SEND				0x6a
#define ERROR_SET_TEMPRCTRL_ALARMOUT_RECV				0x6b

#define ERROR_GET_TEMPRCTRL_ALARMPARAM_SEND				0x6c
#define ERROR_GET_TEMPRCTRL_ALARMPARAM_RECV				0x6d
#define ERROR_SET_TEMPRCTRL_ALARMPARAM_SEND				0x6e
#define ERROR_SET_TEMPRCTRL_ALARMPARAM_RECV				0x6f
#define ERROR_SET_TEMPRCTRL_DEVCONFIG_SEND				0x70
#define ERROR_SET_TEMPRCTRL_DEVCONFIG_RECV				0x71
#define ERROR_GET_TEMPRCTRL_DEVCONFIG_SEND				0x72
#define ERROR_GET_TEMPRCTRL_DEVCONFIG_RECV				0x73
#define ERROR_GET_TEMPRCTRL_OSD_DATA_SEND				0x74
#define ERROR_GET_TEMPRCTRL_OSD_DATA_RECV				0x75
#define ERROR_SET_TEMPRCTRL_OSD_DATA_SEND				0x76
#define ERROR_SET_TEMPRCTRL_OSD_DATA_RECV				0x77
#define ERROR_GET_TEMPRCTRL_INPUTDATA_SEND				0x78
#define ERROR_GET_TEMPRCTRL_INPUTDATA_RECV				0x79
#define ERROR_GET_CONF_RS232_SEND                       0x7a 
#define ERROR_GET_CONF_RS232_RECV						0x7b
#define ERROR_NET_SET_NTP_SEND							0x7c
#define ERROR_NET_GET_NTP_RECV							0x7d
#define ERROR_NET_SET_NTP_RECV							0x7e
#define ERROR_NET_GET_NTP_SEND							0x7f
#define RECV_SET_OSD_NAME_ERROR							0x80
#define SEND_SET_OSD_NAME_EROOR							0x81
#define ERROR_GET_DEV_FUN_LIST_SEND						0x82
#define EERROR_NET_GET_DEV_FUN_LIST_RECV				0x83
#define ERROR_GET_OSD_CUSTOM_TEXT_COLOR_SEND			0x84
#define ERROR_GET_OSD_CUSTOM_TEXT_COLOR_RECV			0x85
#define ERROR_SET_OSD_CUSTOM_TEXT_COLOR_SEND			0x86
#define ERROR_SET_OSD_CUSTOM_TEXT_COLOR_RECV			0x87
#define ERROR_CHECKNTP_SEND								0x88
#define ERROR_CHECKNTP_RECV								0x89
#define ERROR_IO_NUM_SEND								0x8a
#define ERROR_IO_NUM_RECV								0x8b
#define ERROR_IO_GET_PROPERTY_SEND						0x8c
#define ERROR_IO_GET_PROPERTY_RECV						0x8d
#define ERROR_IO_GET_IO_ISENABLE_SEND					0x8e
#define ERROR_IO_GET_IO_ISENABLE_RECV					0x8f
#define ERROR_IO_SET_IO_ISENABLE_SEND					0x90
#define ERROR_IO_SET_IO_ISENABLE_RECV					0x91

#define ERROR_SET_FUNC_MODE_SEND						0x92
#define ERROR_SET_FUNC_MODE_RECV						0x93
#define ERROR_GET_FUNC_MODE_SEND						0x94
#define ERROR_GET_FUNC_MODE_RECV						0x95

//#define 
//ENd 
/*Video Data Type*/
#define DVO_FRAME_VIDEO_HEAD	         1
#define DVO_FRAME_VIDEO_I                3
#define DVO_FRAME_VIDEO_P                4
#define DVO_FRAME_VIDEO_BP               5
#define DVO_FRAME_AUDIO                  2
#define DVO_FRAME_VIDEO_SUB_HEAD         7
#define DVO_FRAME_VIDEO_SUB_I            8
#define DVO_FRAME_VIDEO_SUB_P            9
#define DVO_FRAME_VIDEO_SUB_BP          10

#define DVO_FRAME_VIDEO_BBP             11
#define DVO_FRAME_VIDEO_SUB_BBP         12
#define DVO_FRAME_MOTION_FRAME          13
#define DVO_FRAME_VIDEO_B               14
#define DVO_FRAME_VIDEO_SUB_B           15

#define DVO_FRAME_VIDEO_STREM_LOST		16

#define STREAM_AUDIO_FRAME				2
#define STREAM_I_FRAME					1
#define STREAM_P_FRAME					0
#define STREAM_MOTION_FRAME				3
#define STREAM_B_FRAME					4
typedef unsigned char u8;


enum 
{
    UPGRADE_ERROR=-1,
    READY_UPGRADE,

    UPGRADE_FINISH,
    UPGRADE_ING,
    ERROR_FILE_NAME=6,
    ERROR_UPGRADE_SENDING,
};

enum 
{
    GET_VIDEO_COLOR,
    SET_VIDEO_COLOR,
    GET_VIDEO_STATUAS,
    SET_VIDEO_STATUS,
    GET_IPCM_FEATURE,
    SET_IPCM_FEATURE,
    GET_EXP_IPCM,
    SET_EXP_IPCM,
    SET_BLANKBANLACE,
    GET_BLANKBANLACE,
    GET_DEVICE_CONFIGINFO,
    SET_DEVICE_CONFIGINFO,
    UPGRADE_READY,
    SENDFILETOUPGRADE,
    STOPSEND_FILETOUPGRADE,

    GET_UPGRADE_STATE,
    GET_OSD_NAME,
    SET_OSD_NAME,
    GET_NET_STATE,
    GET_VIDEO_QUILATY,
    SET_VIDEO_QUILATY,
    GET_OSD_DATE,
    SET_OSD_DATE,
    GET_MASK,
    SET_MASK,
    SET_REGISTER,
    GET_STREAM_TYPE,
    SET_STREAM_TYPE,
    SET_SYS_TIME,
    GET_SYS_TIME,
    GET_NETWORKSETTING,
    SET_NETWORKSETTING,
    GET_ALARM_MASK,
    SET_ALARM_MASK,
    GET_MOTION_ROWCOLS,
    SET_MOTION_ROWCOLS,
    SET_MOTIONEX,
    GET_MOTIONEX,
    SET_RS232_CONFIG,
    GET_RS232_CONFIG,
    SET_SENDR232_DATA,
    Get_DISK_INFO,
    GET_GRAY_VIDEO_MODE,
    SET_GRAY_VIDEO_MODE,
    SET_VIDEO_OUT_MODE,
    GET_VIDEO_OUT_MODE,
    GET_MATCH_CHIP,
    GET_WIFI_INFO,
    SET_WIFI_INFO,
    SET_OUT_AVDATA,
    SET_IO_OUT,
    GET_IO_OUT,
    SET_AUDIO_ENC,
    GET_AUDIO_ENC,
    GET_FLEP,
    SET_FLEP,
    GET_ENV,
    SET_ENV,
    GET_CUSTOM_OSD_AREA,
    SET_CUSTOM_OSD_AREA,
    GET_LINE_OSD_PRAM,
    SET_LINE_OSD_PRAM,
    GET_LINE_OSD_TEXT,
    SET_LINE_OSD_TEXT,
    GET_TEMPRCTRL_ALARMOUT,
    SET_TEMPRCTRL_ALARMOUT,
    GET_TEMPRCTRL_ALARMPARAM,
    SET_TEMPRCTRL_ALARMPARAM,
    GET_TEMPRCTRL_DEVICECONFIG,
    SET_TEMPRCTRL_DEVICECONFIG,
    GET_TEMPRCTRL_OSDDATA,
    SET_TEMPRCTRL_OSDDATA,
    GET_TEMPRCTRL_INPUTDATA,
    SET_NTP_INFO,
    GET_NTP_INFO,
    GET_DEVICE_FUNC_LIST,
    SET_CUSTOM_TEXT_COLOR,
    GET_CUSTOM_TEXT_COLOR,
    CHECK_NTP_INFO,
    GET_IO_NUM,
    GET_IO_PROPERTY,
    SET_IO_PROPERTY,
    GET_IO_ENABLE,
    SET_IO_ENABLE,
    SET_MODEL_TYPE,
    GET_MODEL_TYPE
};

namespace dvonetSDK
{
    typedef enum
    {
        TEMPRATURE_CELS_TYPE=0,
        TEMPRATURE_FAHR_TYPE,
        HUMIDITY_TYPE,
        CO2_TYPE,
    }DATA_TYPE_CTRL;

#ifndef u32
#define 	u32       unsigned int 
#endif
    typedef long SERIAL_HANDLE;
#define DVO_MAX_ALARM_OUT		16
#define MAX_SLOT				64
    typedef long VOICE_HANDLE;
    /*work sheet  2 in 1  davinci & linux*/
#define MAX_SEGMENT 3
#define MAX_DAY     7
    /*alarm data type*/
#define DVO_ALARM_IOIN                  1	//Ioin[0]报警

#define DVO_NO_ALARM_IOIN               2	//Ioin[0]不报警
#define DVO_ALARM_MOTION				3	//MD[0]报警
#define DVO_ALARM_NO_MOTION		        4	//MD[0]不报警
#define DVO_ALARM_IO_MOTION		        5	//Ioin[0]报警 MD[0]报警 <=>Ioin[0]断路 MD[0]报警
#define DVO_ALARM_IOIN_NO_MOTION	    6	//Ioin[0]报警 MD[0]不报警<=> Ioin[0]断路 MD[0]不报警
#define DVO_ALARM_NO_IOIN_MOTION	    7	//Ioin[0]不报警 MD[0]报警 <=>Ioin[0]通路 MD[0]报警
#define DVO_ALARM_HEARTBEAT 	        8	//Ioin[0]不报警 MD[0]不报警 <=>Ioin[0]通路 MD[0]不报警
#define DVO_ALARM_HEARTBEAT_SEND_FAILED 9		//发送心跳包失败
#define DVO_ALARM_TEMPR_CTRL_MOTION     0x0A	//温控报警类型
#define DVO_ALARM_TEMPR_CTRL_ERROR_     0x0B	//温控异常上报

    struct tSubChannelSet{
        int slot;
        int used; //是否启用子码流，0-不启用 1-启用,当不启用，下面参数无效

        int encodeType; /*0-CIF,1-D1,2-720p 3-1080p,0xff-qcif*/
        int qualityLev;/* 0-最好,1-次好，2-较好，3一般*/
        int maxBps;		//0-不限,1-较小 2-大 3-较大 4-最大，CIF(2000,400,600,800,1000),D1(4000,800,1000,1400,1600),qcif(600,100,200,300,400)
        int vbr;		//0-cbr 1-vbr
        int framerate;  //0-全帧率 1-29 实际帧率

        int reserve[256];
    };

    /*****************		lzs		****************/
    /*network setting*/
    struct tNetworkSetInfo 
    {
        int		port;
        char	sDvrIp[IP_LENGTH];				//dvr ip地址
        char	sDvrMaskIp[IP_LENGTH];			//dvr子网掩码
        char	gateway[IP_LENGTH];
        int		byMACAddr[6];					//服务器的物理地址
        char	dns[IP_LENGTH];
        char	sMultiCastIP[IP_LENGTH];		 //多播组地址
        int		dwPPPOE;						//0-不启用,1-启用
        char	sPPPoEUser[MAX_NAME_LEN];		//PPPoE用户名
        char	sPPPoEPassword[MAX_PASS_LEN];	// PPPoE密码
        char	sPPPoEIP[IP_LENGTH];			//PPPoE IP地址(只读)
        int     iscard ;//0 有线，1无线
        int		reserve[31];
    };

    typedef struct tagloginfo{
        int		state	;		//0：成功 1：失败。
        char		dev_type[64];//设备类型，字符串。
        char		dev_name[64];//设备名，字符串。
        char		dev_id[64];	//设备ID，字符串。
        char		dvo_version[32];//设备硬件版本，字符串。
        char		sdk_sw_version[32];//设备软件SDK版本，字符串。
        char		app_sw_version[32];//设备应用软件版本，字符串。
        char		rev1[32];
    }app_net_tcp_sys_logo_info_t;

    ///*video status*/
    //struct tVideoStatus{
    //	int slot;
    //	int recStatus;/*0- not rec, 1- rec*/
    //	int signalStatus;/*0 - no signal, 1- signal*/
    //	int motionStatus;/*0- no motion alarm, 1- motion alarm*/
    //};

    /*alarm */
    struct tAlarmData{
        int value;			//报警的视频通道，如果为硬盘报警则为硬盘序号
        int status;	    
        int reserved[32];
    };
    /*stream type*/
    struct tStreamType{
        int slot;
        int type;/*1- video, 2-audio,3-both*/
    };
    typedef enum{
        ALARM_DO_NOTHING = 0,
        ALARM_TO_MONITOR = (1<<0),
        ALARM_WITH_AUDIO = (1<<1),
        ALARM_TO_CENTER =  (1<<2),
        ALARM_TO_OUT =  (1<<3),
        ALARM_TO_RECORD = (1<<4),
        ALARM_TO_SNAP = (1<<5),
    }AlarmHandleType;

    typedef struct 
    { 
        int type;   /*处理方式,处理方式的"或"结果,参照AlarmHandleType*/
        u8 byRelAlarmOut[DVO_MAX_ALARM_OUT];  /*  报警触发的输出通道,报警触发的输出,为1 表示触发该输出 */ 
        u8 byRelRecord[MAX_SLOT];// 联动录象	, 报警触发的录象通道,为1表示触发该通道 //移动侦测可以不支持
        u8 byRelSnap[MAX_SLOT];// 联动抓图			
    }tHandle; 

    /*center information*/
    struct tCenter{
        char ip[32];
        int port;
        int listenType;/*0 - udp ,1 - tcp*/
        int reserved[32];
    };
    struct tAlarmOutState{
        int solt;
        u8 state[DVO_MAX_ALARM_OUT]; //报警输出状态 0:无效 1：有效
        int reserved[32];
    };

    struct tAlarmOutCfg
    {
        int alarmout_no;//0xff-全部，否则表明报警输出
        u8 state;//1- 有效，0- 无效
        int reserved[32];
    };
    struct tgray_whitevmode
    {
        int slot;
        int modetype;
        int reserved[32];
    };
    struct tvout_interface
    {
        int slot;
        int devicetype; //0 CVBS 1 HDMI
        int state;// 0 close 1 open
        int reserved[32];
    };

    //遮挡报警
    struct tAlarmMask
    {
        int slot;
        int enabled;//是否启用
        tHandle handle;
        int reserved[32];
    };
    struct tIoOutAlaram
    {
        int slot;
        int state; //0 不报警 1 报警
        int serv[32];
    };

    struct tWifiInfo
    {
        int type ;///wifi加密类型。0:open,1:wep,2:wpa,3:tkip.
        char ssid[36];//WIFI的SSID。
        char pwd[68];//wifi密码
        char resver[32];
    };
    struct tAouInfo
    {
        int type;//0 audio 1 video
        int enable; //0 start 1 stop
        int result; //0 success other failed
    };
    //透明通道发送,只有在串口的work_mode=2(透明通道)时有效
    struct tRs232Send
    {
        int rs232_no;//当slot小于本地通道数的时候,为本地串口号，否则为该网络通道对应的串口号
        int protocol;//协议类型，参照protocolType,如果=0xffffffff,表明其他协议，只要直接转发即可。
        int len;//数据长度
        u8 data[256];
        int reserve[32];
    };
    //串口设备设置
    struct tRs232Cfg
    {
        int rs232_no;//当slot小于本地通道数的时候,为本地串口号，否则为该网络通道对应的串口号
        int rate; /*  波特率(bps)，0－50，1－75，2－110，3－150，4－300，5－600，6－1200，7－2400，8－4800，9－9600，10－19200， 11－38400，12－57600，13－76800，14－115.2k;*/ 
        u8 data_bit; //0-5位 1-6位 2-7位 3-8位
        u8 stop_bit; //0-1位 1-2位
        u8 parity; //0-无校验 1-奇校验 2-偶校验
        u8 flow_control;//0-无 1-软流控 2-硬流控
        int work_mode; //工作模式 0-云台 1-报警接收 2-透明通道
        int annunciator_type;/*0-Howell 1-VISTA 120 ... */	
        int reserved[32];
    };
    typedef struct
    {
        u8 byStartHour; //0-23
        u8 byStartMin; //0-59
        u8 byStopHour; //0-23
        u8 byStopMin; //0-59
    }DVO_NET_SCHEDTIME;
    struct tAlarmInCfg
    {
        int alarmin_no;//报警器序号 
        int type;//报警器类型 0-常开 1-常闭
        tHandle handle;//报警联动处理
        DVO_NET_SCHEDTIME schedule[MAX_DAY][MAX_SEGMENT];//报警处理的时间段，在这些时间段里才处理报警
        int reserved[32];
    };

    struct tAlarminDefend{
        int alarmin_no;//报警器序号 
        int type;//0-定时  1-手动不妨 2-手动撤防
        int reserved[32];
    };
#define DVO_MAX_ALARM_IN			16
#define DVO_MAX_DISKNUM			8
#define DVO_MAX_DSPNUM			32
    /* alarm state */  //该接口为模拟dvr使用暂时不用
    struct tAlarmState 
    {
        int		alarm_in_state[DVO_MAX_ALARM_IN];		//报警输入状态
        int		alarm_out_state[DVO_MAX_ALARM_OUT];		//报警输出状态
        int		rec_state[MAX_SLOT];					//录像状态 0-不录，1-录像
        int		signal_state[MAX_SLOT];					//视频信号状态 0-无 1-有
        int		disk_state[DVO_MAX_DISKNUM];				//硬盘状态	0-正常 1-无硬盘 2-硬盘坏 3-硬盘未格式化 4-硬盘满 
        int		motion_state[MAX_SLOT];					//移动侦测 0-没有移动侦测报警 1-有
        int 	dsp_state[DVO_MAX_DSPNUM];				//dsp状态  0-异常 1-正常
        char	reserve[32];
    };


#define DVO_MAX_SERIALID			32
    /* devcie configuration 目前设置只能设置设备的名称*/
    struct tDevConfig 
    {
        char	devName[MAX_NAME_LEN];				//设备名称
        char	devSerialID[DVO_MAX_SERIALID];		//设备序列号
        int		window_count;		//窗口数
        int		alarm_in_count;		//报警输入数
        int		alarm_out_count;	//报警输出数		
        int		disk_count;			//硬盘数--SD卡
        int		dsp_count;			//dsp数 暂时没有改字段
        int		rs232_count;		//串口数
        char	channelname[MAX_SLOT][32]; // 通道名称 --->暂时没有改字段设置

        int		net_channel_count;  //网络通道数 暂时为网卡数
        char	reserve[28];
    };

    enum APP_NET_TCP_SERIAL_TYPE
    {
        APP_NET_TCP_SERIAL_TYPE_RS232	= 1, //setdefault
        APP_NET_TCP_SERIAL_TYPE_RS422,
        APP_NET_TCP_SERIAL_TYPE_RS485,
    };

    enum APP_NET_TCP_SERIAL_BAUDRATE
    {
        APP_NET_TCP_SERIAL_2400		= 2400, //set default
        APP_NET_TCP_SERIAL_4800		= 4800,
        APP_NET_TCP_SERIAL_9600		= 9600,
        APP_NET_TCP_SERIAL_19200 	= 19200,
        APP_NET_TCP_SERIAL_38400		= 38400,
        APP_NET_TCP_SERIAL_57600		= 57600,
        APP_NET_TCP_SERIAL_115200	= 115200,
        APP_NET_TCP_SERIAL_230400	= 230400,
    };
    //ptz云台设置,只有在串口的设置了该接口后有效
    struct tPtzRs232Cfg
    {
        int slot;
        int rs232_no;	//当slot小于本地通道数的时候,为本地串口号，否则为该网络通道对应的串口号
        int rate;		/*  波特率(bps)，参照APP_NET_TCP_SERIAL_BAUDRATE*/ 
        u8 data_bit;	//0-5位 1-6位 2-7位 3-8位   传值:5,6,7,8 default 8
        u8 stop_bit;	//0-1位 1-2位    // 1or 2   defalut 1 
        u8 parity;	//0-无校验 1-奇校验 2-偶校验  // //传值'N','O','E'  default 'N'
        //u8 flow_control;//0-无 1-软流控 2-硬流控  //
        int protocol;	//协议类型, 参照 APP_NET_TCP_SERIAL_TYPE 
        //	int address; //地址 0-255
        int reserve[32];
    };
    struct tVideoLostCfg
    {
        int slot;
        tHandle handle;
        DVO_NET_SCHEDTIME schedule[MAX_DAY][MAX_SEGMENT];//视频丢失处理的时间段，只有在这些时间段才处理
        int reserve[32];
    };
    /*motion*/
    //struct tMotion{
    //	int slot;
    //	int lev ;		/*0最高-5最低，6关闭*/
    //	int	recDelay;	//0-6:10s,20s,30s,1m,2m,5m,10m
    //	int data[18];	//从低位往高位为0-21
    //};

#define  DVO_MOTION_MAX_ROWS  128
#define  DVO_MOTION_MAX_COLS 128
    struct tMotionRowCols
    {
        int slot;
        int rows;//0-HW_MOTION_MAX_ROWS
        int cols;//0-HW_MOTION_MAX_COLS
        int reserved[32];
    };
    struct tMotionEx
    {
        int slot;
        int lev ;		//0-100灵敏度
        int	recDelay;	//0-6:10s,20s,30s,1m,2m,5m,10m	门限 0-100
        char data[DVO_MOTION_MAX_ROWS * DVO_MOTION_MAX_COLS / 8];
        tHandle handle;
        int reserved[32];//int resved[0] x ，intresved[1] y int reserved[2] width，reserved[3] height
    };

    struct tPMotion
    {
        int chns;
        int x;
        int y;
        int w;
        int h;
    };

    struct tAlarmMotionDataEx//主动上传
    {
        unsigned int slot; //视频窗口号，从0开始
        long tmSec;        //时间，从1970年1月1号经过的秒数
        unsigned int tmMSec;//毫秒，0-1000	
        char data[DVO_MOTION_MAX_ROWS * DVO_MOTION_MAX_COLS / 8];
        int reserved[32];
    };

    /*rec file*/
    struct tRecFile
    {
        int slot;
        SYSTEMTIME beg;
        SYSTEMTIME end;
        int type;/*0=all 1=normal file 2=mot file 3=alarm 4=mot&&alarm 5=ipcam lost*/
    };

#define  MAX_SMARTSEARCH_RECT 5
    struct tSmartSearchEx
    {
        tRecFile recFile;
        int rt_count;
        RECT rt[MAX_SMARTSEARCH_RECT]; //按照704 * 576
        int direction;//0-全部  4-左 6-右  8-上 2-下
        int reserved[32];
    };
    //硬盘信息获取
    struct tEachHarddiskInfo
    {
        INT64 volume; //硬盘容量
        INT64 free;//可用容量
        int state;//硬盘状态 0- 正常 1-休眠 2-不正常
        int reserved[32];
    };

#define  MAX_HARD_DISK_COUNT 16
    struct tHarddiskInfo
    {
        tEachHarddiskInfo disk[MAX_HARD_DISK_COUNT];
        int reserved[32];
    };

    enum DISPLAY_STREAM_ERROR
    { 
        SUCCESS_DISPLAY=0,
        NET_DWON_ERROR,
        NET_DATA_ERROR,
        NET_CONNET_INIT_ERROR,
        NET_DIRECTDRAW_INIT_ERROR,
    };

    enum APP_NET_TCP_IMG_WB_MODE
    {
        APP_NET_TCP_IMG_WB_AUTO 		= 0, //自动白平衡。
        APP_NET_TCP_IMG_WB_CUSTOM_1,		//自定义白平衡模式1.
        APP_NET_TCP_IMG_WB_CUSTOM_2,		//自定义白平衡模式2.
        APP_NET_TCP_IMG_WB_CUSTOM_3,		//自定义白平衡模式3.
        APP_NET_TCP_IMG_WB_MODE_ALL,
    } ;
    enum APP_NET_TCP_IMG_EXP_MODE
    {
        APP_NET_TCP_IMG_EXP_AUTO		= 0, //自动曝光
        APP_NET_TCP_IMG_EXP_MANUAL	= 1, //手动曝光
        APP_NET_TCP_IMG_EXP_MODE_ALL,
    };

    //typedef enum {		
    //APP_NET_TCP_ESHUTTER_1_SEC=0, /* 1 sec */	
    //APP_NET_TCP_ESHUTTER_10_SEC, /* 1/10 sec */		
    //APP_NET_TCP_ESHUTTER_12_SEC, /* 1/12 sec */	
    //APP_NET_TCP_ESHUTTER_15_SEC,	
    //APP_NET_TCP_ESHUTTER_20_SEC,
    //APP_NET_TCP_ESHUTTER_25_SEC,	
    //APP_NET_TCP_ESHUTTER_30_SEC,	
    //APP_NET_TCP_ESHUTTER_50_SEC,	
    //APP_NET_TCP_ESHUTTER_60_SEC,	
    //APP_NET_TCP_ESHUTTER_100_SEC,		
    //APP_NET_TCP_ESHUTTER_120_SEC,	
    //APP_NET_TCP_ESHUTTER_240_SEC,	
    //APP_NET_TCP_ESHUTTER_480_SEC,	
    //APP_NET_TCP_ESHUTTER_960_SEC,	
    //APP_NET_TCP_ESHUTTER_1024_SEC,
    //APP_NET_TCP_ESHUTTER_COUNT,	
    //}APP_NET_TCP_ESHUTTER_E;

    struct tIPCam_feature{
        int		slot;
        int		bae;		//是否自动曝光
        int		eshutter;//参照HW_IPCAM_ESHUTTER_E
        int		agcgain;//0-30
        int		blackwhitemode;		//彩转灰
        int		badj;		//自动调节
        int		blacklevel;		//黑电平[-128,128]，badj无效时该参数有效

        int     bAutoWB; //自动白平衡
        int     wb_mode; //白平衡模式 APP_NET_TCP_IMG_WB_MODE
        int     rgb_gain[3];//rgb增益 0-16384

        int		noisefilter;		//噪声抑制0-5，5最大，badj无效时该参数有效
        int		sharp;				//锐度0-5，5最大，badj无效时该参数有效

        int		luma;				//流明

        int		reserved[13];
    };
    struct tIPCAM_BLANCEBLACK{
        int solt;//视频输入通道号，取值0~MAX-1.
        int wb_mode ;//白平衡模式，范围参考APP_NET_TCP_IMG_WB_MODE
        //int blackleavel;
        int rgb_gain[3];//R增益，取值范围：0~16000 G增益，取值范围：0~16000 B增益，取值范围：0~16000
        int rev[3];
    };

    enum APP_NET_TCP_ESHUTTER_E
    {
        APP_NET_TCP_ESHUTTER_1_SEC=0, /* 1 sec */	
        APP_NET_TCP_ESHUTTER_10_SEC, /* 1/10 sec */		
        APP_NET_TCP_ESHUTTER_12_SEC, /* 1/12 sec */	
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
    };

    struct tIPCAM_BEA{
        int slot;
        int bae; //曝光模式。范围参考APP_NET_TCP_IMG_EXP_MODE
        int eshutter; //APP_NET_TCP_ESHUTTER_E
        int agcgain; //最大增益。范围：1~60
    };
    /*mask*/
#define MAX_MASK_RECT 4
    struct tMask
    {
        int slot;
        int count;		//遮挡个数，不大于4个
        int lev;		//暂时未使用
        RECT rt[MAX_MASK_RECT];
    };

    //强光抑制,当前只有enable和max_luma_night 有用
    struct tBlackMaskBLC{
        int slot;

        int enable;

        /* 夜间的luma最大值,小于该值则启用逆光抑制 */
        int max_luma_night;

        /* 调整后，原高亮block的y值应该在此范围内  */
        int block_y_max;
        int block_y_min;

        /* 调整后如果y值小于该值，说明亮光过去，需要恢复自动曝光 */
        int block_y_night_normal;

        int mode; //0: 固定增益 1:固定快门 2:自动
        int fixedAgc;
        int fixedEshutter;

        int max_eshutter;
        int min_eshutter;
        int max_agc;
        int min_agc;

        /* 
        * 区域选择，3x3共9个区域。 用位表示选择或不选择
        * 如 (1<<0|1<<3) 即选择了第1个区域和第4个区域
        */
        int area_map; 

        /* 设置掩码 */
        /* 0: nothing to set */
        /* 1<<0: enable */
        /* 1<<1: area_map */
        /* 1<<2: max_luma_night */
        /* 1<<3: block_y_max */
        /* 1<<4: block_y_min */
        /* 1<<5: block_y_night_normal */
        /* 1<<6: mode */
        /* 1<<7: fixedAgc */
        /* 1<<8: fixedEshutter */
        /* 1<<9: max_eshutter */
        /* 1<<10: min_eshutter */
        /* 1<<11: max_agc */
        /* 1<<12: min_agc */
        int set_mask;

        char reserve[32];

    };
    /*video color struct*/
    struct tVideoColor
    {
        int slot;
        int brightness/*0-255*/;
        int contrast/*0-255*/;
        int saturation/*0-255*/;
        int hue/*0-255*/;
        int acuity;/*0-255*/
    };

    /*video status*/
    struct tVideoStatus{
        int slot;
        int recStatus;/*0- not rec, 1- rec*/
        int signalStatus;/*0 - no signal, 1- signal*/
        int motionStatus;/*0- no motion alarm, 1- motion alarm*/
    };

    typedef struct cl{
        u8		b; //b分量，取值范围0~255.
        u8		g;//g分量，取值范围0~255.
        u8		r;//r分量，取值范围0~255.
        u8		a;//透明度，取值范围0~255.


    }app_net_tcp_osd_color_t;
    /*osd channel struct*/
    struct tOsdChannel
    {
        int slot;
        int isEnable;		//是否显示
        int left;			//0-703
        int top;			//0-575
        char name[32];		//通道名称
        int fontsize;  //字体大小设置 16-60范围
        app_net_tcp_osd_color_t color; //字体颜色
        int type;//0 channel 1 其他 APP_NET_TCP_OSD_AREA_UPPER_RIGHT 2 APP_NET_TCP_OSD_AREA_BOTTOM_RIGHT
        int outline ;//是否勾边 0：不勾边，1：勾边

    };

    /*osd struct*/
    struct tOsdDate
    {
        int slot;				//从0开始
        int isEnable;			//是否显示
        int left;				//0-703
        int top;				//0-575
        int type;				//		无效
        int fontsize;			//字体大小设置 16-60范围
        app_net_tcp_osd_color_t color;  //字体颜色
        int isShowWeek;			//是否显示星期
        int outline;			//是否勾边 0：不勾边，1：勾边

    };
    /*encode quality struct*/
    struct tVideoQuality
    {
        int slot;			// 0：h264 1080p 1:  h264 D1	2:  h265 1080p
        int encodeType;		/*0-CIF, 1-D1 2-720p,3-1080p,4-1280*960,5-1024*768,0xff-qcif*/         
        int qualityLev;		/* 0-最好,1-次好，2-较好，3一般*/
        int maxBps;			//0-不限,1-较小 2-大 3-较大 4-最大，CIF(2000,400,600,800,1000),D1(4000,800,1000,1400,1600),720p(8000,2000,2500,3000,4000)

        int vbr;			//0-cbr 1-vbr 0定码率，1变码率
        int framerate;		//0-全帧率 1-24 实际帧率
        int reserved;		//i-interval
        int en_mode;		//0 H264 1jpeg
        int streamtype ;	//0 main stream 1,sub stream 2 third stream
        int min_bps;
        //int iQuality;
        //int pQuality;
        //int bQuality;
    };


    /*ptz*/
    struct tPtzControl
    {
        int slot;
        int controlType;/*0-direct 1- len 2-AUTO zoom in,3-preset*/
        int cmd;/*direct: 7-left up
                8-up
                9-right up
                4-left
                5-stop
                6-right
                1-left down
                2-down
                3-right down

                len:     1-iris open
                2-iris close
                3-len tele
                4-len wide
                5-focus far
                6-focus near
                7-stop
                AUTO zoom in:
                1-auto zoom in
                preset:
                1-set
                2-clear
                3-goto
                */
        int presetno;//预置点号或方向移动的速度（0-64）
        RECT rect;			//点击放大，相对于704×576
    };


    struct app_net_tcp_audio_config_t
    {
        int			chn; //音频输入通道，取值0~MAX-1.
        int			enc_type;//音频编码类型：0：711_a，1：711_u，2：726，3：AAC
        int			samplerate;//采样率，参考APP_NET_TCP_AUDIO_AUDIO_FS。
        int			rev[5];
    };
    enum APP_NET_TCP_AUDIO_AUDIO_FS
    {
        APP_NET_TCP_AUDIO_FS_48000		= 48000,
        APP_NET_TCP_AUDIO_FS_44100 		= 44100,
        APP_NET_TCP_AUDIO_FS_32000		= 32000,
        APP_NET_TCP_AUDIO_FS_24000		= 24000,
        APP_NET_TCP_AUDIO_FS_16000		= 16000,
        APP_NET_TCP_AUDIO_FS_11025		= 11025,
        APP_NET_TCP_AUDIO_FS_8000		= 8000,
    };//音频采样率。
    enum APP_NET_MIRROR_MODE
    {
        APP_NET_MIRROR_HORRIZONTALLY_VERTICALLY      = 0,
        APP_NET_MIRROR_HORRIZONTALLY,
        APP_NET_MIRROR_VERTICALLY,
        APP_NET_MIRROR_NONE,
        APP_NET_MIRROR_ALL,
    };

    struct tImgeFelp
    {
        int chn;
        APP_NET_MIRROR_MODE	mirror_mode;//翻转模式
        UINT							rev[6];
    };

    struct tEvnParam
    {
        int chn;
        int type; //0 室内1 室外 2 自动
    };

    struct tCustomAreaOSD
    {
        unsigned int		chn; //视频输入通道号，取值0~MAX-1.
        unsigned int	    enable; //OSD显示开关，1：显示，0：关闭。
        unsigned int		x_edge;//x方向左边距。取值：主码流宽度
        unsigned int		y_edge; //y方向上边距。取值：主码流高度	
        unsigned int	    size;//字体大小。取值：16~60
        unsigned int	    max_line_num;  //自定义OSD区域最大显示行数
        unsigned int       max_text_len; //每行最大字符长度
        unsigned int         stream ;//0 主码流 1次码率,2 三码流
        unsigned int		rev[8];	

    };
    struct tCustomRow
    {
        unsigned int				  chn; //视频输入通道号，取值0~MAX-1.
        unsigned int			      row_no; //行号 从0开始

        app_net_tcp_osd_color_t		  color;//字体颜色
        unsigned int			      outline;//是否沟边
        unsigned int         stream ;//0 主码流 1次码率,2 三码流
        unsigned int			   rev[7];	
    };
    struct tCustomRow_Text //设置文本。
    {
        unsigned int chn;
        unsigned int row_no;
        char ctext_str[256];
        unsigned int         stream ;//0 主码流 1次码率,2 三码流
        int rev[7];

    };
    struct tCustomRow_Text2 //设置文本。
    {
        unsigned int chn;
        unsigned int row_no;
        char ctext_str[256];
        unsigned int         stream ;//0 主码流 1次码率,2 三码流
        u32       set_color; //是否设置颜色 0:不设置 1:设置
        u32				 outline;//是否勾边 0：不勾边，1：勾边
        app_net_tcp_osd_color_t		 color;//字体颜色
        int rev[4];

    };
    typedef struct{
        u32		chn; //报警输出通道号。取值0~MAX-1.
        u32     data_type; // ==>datatype和设备类型对应祥见文档附件A
        int		state; //输出状态，0：不报警，1：报警。
        int 		rev[5];
    }app_net_tcp_event_act_485_t;

    //typedef struct{
    //	u32		chn; //报警输出通道号。取值0~MAX-1.
    //    u32     data_type; //0：温度，1：湿度，2：CO2
    //}app_net_tcp_com_chn_485_t;
    //typedef struct{
    //	u32		chn;//视频输入通道号，取值0~	MAX-1.
    //    u32     data_type; //0：温度，1：湿度，2：CO2
    //	int		value[256]; //对温度，湿度，CO2而言，第1位是上限，第2位是下行
    //}app_net_tcp_485_alarm_para_t;
    ////修订数据结构参数值 设定标记为有效无效 上层根据这个有效值判断参数的准确性
    typedef struct{
        u32		chn;//视频输入通道号，取值0~	MAX-1.
        u32     data_type; // ==>datatype和设备类型对应祥见文档附件A
        int		value[255]; //对温度，湿度，CO2而言，第1位是上限，第2位是下行
        int status;//是否有效，0：有效，1：无效
    }app_net_tcp_485_alarm_para_t;
    typedef struct{
        int enable;//0:启动,1:关闭.
        int dev_type; //0-ICPDAS DL-100TM485
        int dev_id;//设备ID
        int interval;//采集时间间隔
        char rev[512];
    }app_net_tcp_485_dev_t;
    /*

    */
    typedef struct{
        u32     data_type; // ==>datatype和设备类型对应祥见文档附件A
        int row; //显示行
        int col;//显示列
        char prefix[32];//显示前缀
        char suffix[32];//显示后缀
        int dev_type;//
        int osd;//是否osd 0 不叠加 1 叠加
        char rev[504];
    }app_net_tcp_485_osd_para_t;
    typedef struct{
        u32     data_type; // ==>datatype和设备类型对应祥见文档附件A
    }app_net_tcp_485_osd_data_type_t;

    typedef struct{
        u32 dev_type;//485输入设备类型 0为ICPDAS DL-100TM485
        u32 data[255]; //对于DL-100而言，第一位为温度，第二位为湿度
        u32 status; //是否有效，0：有效，1：无效
    }app_net_tcp_485_input_data_t;
    ////////////////报警类型 为 ：DVO_ALARM_TEMPR_CTRL_MOTION 时发送温控报警消息调用该结构处理data数据 //////////////////
    //注意该结构给上层数据位网络原始数据对4字节数据需要大小端转换。
    typedef struct{
        u32     dev_type;//485输入设备类型 0为ICPDAS DL-100TM485
        char	 	_485_alarm[16]; //0:无报警，1：报警 对DL-100而言，第一位为摄氏温度报警，第二位为华氏温度报警，第三位为湿度报警，第四位为CO2报警
        u32     _485_data[16]; //对DL-100而言，第一位为摄氏温度，第二位为华氏温度，第三位为湿度，第四位为CO2
        char	 	rev[428];
    }app_tempr_ctrl_alram_data;

    typedef struct {
        unsigned	   enable; //0:关闭，1：打开
        char        addr[128]; //NTP服务器IP或域名
        unsigned    port; //NTP服务器端口
        unsigned    inteval; //同步间隔
        unsigned    set_time_zone; //是否设置时区，0：不设置 1设置
        int         time_zone;
        char        rev[248];
    }app_net_tcp_sys_ntp_para_t;


    /////////////////////////
    ///DVO_ALARM_TEMPR_CTRL_ERROR_ 事件中返回该事件结构
    //说明：alarm_id编号从1000开始，1000-485输入功能故障，1001-485输入功能恢复，1002-NTP功能故障，1003-NTP功能恢复  1004-测试NTP功能正常结束
    typedef struct {
        unsigned    alarm_id; 
        unsigned    alarm_type; //0-设备故障上报，1-设备故障恢复上报，2-设备事件上报
        unsigned    para_num; //参数个数
        unsigned    alarm_para[4]; //设备相关参数
        unsigned    rev[256];
    }app_net_tcp_sys_func_state_report_t;

    typedef struct {
        char        func_list[1600]; 
        char        rev[256];
    }app_net_tcp_sys_func_list_t;

    typedef struct{
        unsigned  io_alarm_num;
        unsigned  rev[7];
    }app_net_tcp_io_alarm_num_t;
    typedef struct{
        unsigned  index; //同上
        unsigned  active_level; //0-开路 1-闭路
        unsigned  set_in_out; //能否改变报警方向 0-不能 1-能；该字段为不能改方向时，设置IO报警方向就不出现在界面上
        unsigned  in_out; //0-报警输入 1-报警输出
        unsigned  rev[12];
    }app_net_tcp_io_alarm_attr_t;

    typedef struct{
        unsigned  state; //0-不启用，1-启用
        unsigned  rev[7];
    }app_net_tcp_set_io_alarm_state_t;

    // 设置/获取功能型号结构
    typedef struct
    {
        /*
        DataType:0x0001
        CmdSubType:0x0023
        */
        char      name[128];	//功能型号名称 具体定义如下
        /*
        功能型号名称	功能描述
        (null)			功能型号为空，表示相机为普通相机，无特殊功能
        OSD-IPC			支持OSD叠加功能
        FC-IPC			支持消防泵、喷淋泵状态叠加功能
        TH-IPC			支持环境中温度、湿度的监测功能
        THC-IPC			支持环境中温度、湿度、二氧化碳的监测功能
        CM-IPC			支持走廊模式功能
        */
        char      rev[128];
    }app_net_tcp_func_model_t;

    enum APP_NET_TCP_STATE
    {
        APP_NET_TCP_STATE_SUCCESS = 0,
        APP_NET_TCP_STATE_FAIL,
    };

    enum IPCModeType
    {
        MODE_NULL = 0,
        OSD_IPC,
        FC_IPC,
        TH_IPC,
        THC_IPC,
        CM_IPC
    };

    struct FuntionModeType
    {
        IPCModeType	nMode;		// 型号代码
        char*	pszModel;		// 型号名称
        char*	pszName;		// 显示名称
        char*	pszFunction;	// 功能描述
    };

}


#endif
