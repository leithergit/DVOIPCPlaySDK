/**
*  COPYRIGHT NOTICE 
*  Copyright (c) 2015, Diveo Technology Co.,Ltd (www.diveo.com.cn)
*  All rights reserved.
*
*  @file        dvoipcnetsdk1_define.h
*  @author      jag(luo_ws@diveo.com.cn)
*  @date        2015/11/10 10:00
*
*  @brief       ֧����һ���汾�����ݽṹ
*  @note        ��
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




#define ERROR_INVALIDARG								0X08	// ��Ч�Ĳ���
#define ERROR_PWD										0x09
#define ERROR_USER										0x10
#define ERROR_CONNECT									0x08
//Error CODE begin
#define ERROR_INIT_MGR_INSTANCE							0x11    //�������ʵ��ʵ����
#define ERROR_LOGIN_SERVER_ERROR_SOCKET_REQ				0x12	//��¼ipc ��������ʧ��
#define ERROR_CONN_STREAM_AV							0x13	//����������ʧ��
#define ERROR_CMD_CONN_NETDOWN							0x14	//����socket ����
#define ERROR_LOGIN_RES									0x15    //recv Login RES state -1 or res none error
#define ERROR_BUF_TYPE                                  0x16	//Error get ��videodata
#define ERROR_OPEN_SOUND                                0x17	//������ʧ��
#define ERROR_CLOSE_SOUND                               0x18	//�ر�����ʧ��
#define ERROR_GET_IPCM_FEATURE                          0x19    //��ȡipc�����Գ���
#define ERROR_SET_IPCM_FEATURE                          0x20	//����ipc�����Գ���
#define ERROR_FILENAME_UPGRADE                          0x21	//�����ļ�������
#define ERROR_NEW_MEMCORY                               0x22	//�����ڴ�ʧ��
#define ERROR_TO_STOP_UPGRADE                           0x23	//ֹͣ����ʧ��
#define ERROR_SET_REGISTER                              0x24	//ע�ᱨ��ʧ��
#define ERROR_SET_HEART                                 0x25	//��������ʧ��
#define ERROR_GET_INSTANCE_CONNIPC                      0x26	//����ʵ��CConnIPC����ʧ�ܻ�Ƿ���user_Handle
#define ERROR_GET_SYS_TIME                              0x27	//��ȡϵͳʱ��ʧ��
#define ERROR_GET_OSD_DATE                              0x28	//��ȡosddateʧ��
#define ERROR_GET_OSD_NAME                              0x29	//��ȡosdnameʧ��
#define ERROR_GET_MOTION_SEND                           0x31	//��ȡmotion ��Ϣʧ��
#define ERROR_GET_MOTION_RECV                           0x32	//��ȡmotion ��Ϣ����ʧ��
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
#define DVO_ALARM_IOIN                  1	//Ioin[0]����

#define DVO_NO_ALARM_IOIN               2	//Ioin[0]������
#define DVO_ALARM_MOTION				3	//MD[0]����
#define DVO_ALARM_NO_MOTION		        4	//MD[0]������
#define DVO_ALARM_IO_MOTION		        5	//Ioin[0]���� MD[0]���� <=>Ioin[0]��· MD[0]����
#define DVO_ALARM_IOIN_NO_MOTION	    6	//Ioin[0]���� MD[0]������<=> Ioin[0]��· MD[0]������
#define DVO_ALARM_NO_IOIN_MOTION	    7	//Ioin[0]������ MD[0]���� <=>Ioin[0]ͨ· MD[0]����
#define DVO_ALARM_HEARTBEAT 	        8	//Ioin[0]������ MD[0]������ <=>Ioin[0]ͨ· MD[0]������
#define DVO_ALARM_HEARTBEAT_SEND_FAILED 9		//����������ʧ��
#define DVO_ALARM_TEMPR_CTRL_MOTION     0x0A	//�¿ر�������
#define DVO_ALARM_TEMPR_CTRL_ERROR_     0x0B	//�¿��쳣�ϱ�

    struct tSubChannelSet{
        int slot;
        int used; //�Ƿ�������������0-������ 1-����,�������ã����������Ч

        int encodeType; /*0-CIF,1-D1,2-720p 3-1080p,0xff-qcif*/
        int qualityLev;/* 0-���,1-�κã�2-�Ϻã�3һ��*/
        int maxBps;		//0-����,1-��С 2-�� 3-�ϴ� 4-���CIF(2000,400,600,800,1000),D1(4000,800,1000,1400,1600),qcif(600,100,200,300,400)
        int vbr;		//0-cbr 1-vbr
        int framerate;  //0-ȫ֡�� 1-29 ʵ��֡��

        int reserve[256];
    };

    /*****************		lzs		****************/
    /*network setting*/
    struct tNetworkSetInfo 
    {
        int		port;
        char	sDvrIp[IP_LENGTH];				//dvr ip��ַ
        char	sDvrMaskIp[IP_LENGTH];			//dvr��������
        char	gateway[IP_LENGTH];
        int		byMACAddr[6];					//�������������ַ
        char	dns[IP_LENGTH];
        char	sMultiCastIP[IP_LENGTH];		 //�ಥ���ַ
        int		dwPPPOE;						//0-������,1-����
        char	sPPPoEUser[MAX_NAME_LEN];		//PPPoE�û���
        char	sPPPoEPassword[MAX_PASS_LEN];	// PPPoE����
        char	sPPPoEIP[IP_LENGTH];			//PPPoE IP��ַ(ֻ��)
        int     iscard ;//0 ���ߣ�1����
        int		reserve[31];
    };

    typedef struct tagloginfo{
        int		state	;		//0���ɹ� 1��ʧ�ܡ�
        char		dev_type[64];//�豸���ͣ��ַ�����
        char		dev_name[64];//�豸�����ַ�����
        char		dev_id[64];	//�豸ID���ַ�����
        char		dvo_version[32];//�豸Ӳ���汾���ַ�����
        char		sdk_sw_version[32];//�豸���SDK�汾���ַ�����
        char		app_sw_version[32];//�豸Ӧ������汾���ַ�����
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
        int value;			//��������Ƶͨ�������ΪӲ�̱�����ΪӲ�����
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
        int type;   /*����ʽ,����ʽ��"��"���,����AlarmHandleType*/
        u8 byRelAlarmOut[DVO_MAX_ALARM_OUT];  /*  �������������ͨ��,�������������,Ϊ1 ��ʾ��������� */ 
        u8 byRelRecord[MAX_SLOT];// ����¼��	, ����������¼��ͨ��,Ϊ1��ʾ������ͨ�� //�ƶ������Բ�֧��
        u8 byRelSnap[MAX_SLOT];// ����ץͼ			
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
        u8 state[DVO_MAX_ALARM_OUT]; //�������״̬ 0:��Ч 1����Ч
        int reserved[32];
    };

    struct tAlarmOutCfg
    {
        int alarmout_no;//0xff-ȫ������������������
        u8 state;//1- ��Ч��0- ��Ч
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

    //�ڵ�����
    struct tAlarmMask
    {
        int slot;
        int enabled;//�Ƿ�����
        tHandle handle;
        int reserved[32];
    };
    struct tIoOutAlaram
    {
        int slot;
        int state; //0 ������ 1 ����
        int serv[32];
    };

    struct tWifiInfo
    {
        int type ;///wifi�������͡�0:open,1:wep,2:wpa,3:tkip.
        char ssid[36];//WIFI��SSID��
        char pwd[68];//wifi����
        char resver[32];
    };
    struct tAouInfo
    {
        int type;//0 audio 1 video
        int enable; //0 start 1 stop
        int result; //0 success other failed
    };
    //͸��ͨ������,ֻ���ڴ��ڵ�work_mode=2(͸��ͨ��)ʱ��Ч
    struct tRs232Send
    {
        int rs232_no;//��slotС�ڱ���ͨ������ʱ��,Ϊ���ش��ںţ�����Ϊ������ͨ����Ӧ�Ĵ��ں�
        int protocol;//Э�����ͣ�����protocolType,���=0xffffffff,��������Э�飬ֻҪֱ��ת�����ɡ�
        int len;//���ݳ���
        u8 data[256];
        int reserve[32];
    };
    //�����豸����
    struct tRs232Cfg
    {
        int rs232_no;//��slotС�ڱ���ͨ������ʱ��,Ϊ���ش��ںţ�����Ϊ������ͨ����Ӧ�Ĵ��ں�
        int rate; /*  ������(bps)��0��50��1��75��2��110��3��150��4��300��5��600��6��1200��7��2400��8��4800��9��9600��10��19200�� 11��38400��12��57600��13��76800��14��115.2k;*/ 
        u8 data_bit; //0-5λ 1-6λ 2-7λ 3-8λ
        u8 stop_bit; //0-1λ 1-2λ
        u8 parity; //0-��У�� 1-��У�� 2-żУ��
        u8 flow_control;//0-�� 1-������ 2-Ӳ����
        int work_mode; //����ģʽ 0-��̨ 1-�������� 2-͸��ͨ��
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
        int alarmin_no;//��������� 
        int type;//���������� 0-���� 1-����
        tHandle handle;//������������
        DVO_NET_SCHEDTIME schedule[MAX_DAY][MAX_SEGMENT];//���������ʱ��Σ�����Щʱ�����Ŵ�����
        int reserved[32];
    };

    struct tAlarminDefend{
        int alarmin_no;//��������� 
        int type;//0-��ʱ  1-�ֶ����� 2-�ֶ�����
        int reserved[32];
    };
#define DVO_MAX_ALARM_IN			16
#define DVO_MAX_DISKNUM			8
#define DVO_MAX_DSPNUM			32
    /* alarm state */  //�ýӿ�Ϊģ��dvrʹ����ʱ����
    struct tAlarmState 
    {
        int		alarm_in_state[DVO_MAX_ALARM_IN];		//��������״̬
        int		alarm_out_state[DVO_MAX_ALARM_OUT];		//�������״̬
        int		rec_state[MAX_SLOT];					//¼��״̬ 0-��¼��1-¼��
        int		signal_state[MAX_SLOT];					//��Ƶ�ź�״̬ 0-�� 1-��
        int		disk_state[DVO_MAX_DISKNUM];				//Ӳ��״̬	0-���� 1-��Ӳ�� 2-Ӳ�̻� 3-Ӳ��δ��ʽ�� 4-Ӳ���� 
        int		motion_state[MAX_SLOT];					//�ƶ���� 0-û���ƶ���ⱨ�� 1-��
        int 	dsp_state[DVO_MAX_DSPNUM];				//dsp״̬  0-�쳣 1-����
        char	reserve[32];
    };


#define DVO_MAX_SERIALID			32
    /* devcie configuration Ŀǰ����ֻ�������豸������*/
    struct tDevConfig 
    {
        char	devName[MAX_NAME_LEN];				//�豸����
        char	devSerialID[DVO_MAX_SERIALID];		//�豸���к�
        int		window_count;		//������
        int		alarm_in_count;		//����������
        int		alarm_out_count;	//���������		
        int		disk_count;			//Ӳ����--SD��
        int		dsp_count;			//dsp�� ��ʱû�и��ֶ�
        int		rs232_count;		//������
        char	channelname[MAX_SLOT][32]; // ͨ������ --->��ʱû�и��ֶ�����

        int		net_channel_count;  //����ͨ���� ��ʱΪ������
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
    //ptz��̨����,ֻ���ڴ��ڵ������˸ýӿں���Ч
    struct tPtzRs232Cfg
    {
        int slot;
        int rs232_no;	//��slotС�ڱ���ͨ������ʱ��,Ϊ���ش��ںţ�����Ϊ������ͨ����Ӧ�Ĵ��ں�
        int rate;		/*  ������(bps)������APP_NET_TCP_SERIAL_BAUDRATE*/ 
        u8 data_bit;	//0-5λ 1-6λ 2-7λ 3-8λ   ��ֵ:5,6,7,8 default 8
        u8 stop_bit;	//0-1λ 1-2λ    // 1or 2   defalut 1 
        u8 parity;	//0-��У�� 1-��У�� 2-żУ��  // //��ֵ'N','O','E'  default 'N'
        //u8 flow_control;//0-�� 1-������ 2-Ӳ����  //
        int protocol;	//Э������, ���� APP_NET_TCP_SERIAL_TYPE 
        //	int address; //��ַ 0-255
        int reserve[32];
    };
    struct tVideoLostCfg
    {
        int slot;
        tHandle handle;
        DVO_NET_SCHEDTIME schedule[MAX_DAY][MAX_SEGMENT];//��Ƶ��ʧ�����ʱ��Σ�ֻ������Щʱ��βŴ���
        int reserve[32];
    };
    /*motion*/
    //struct tMotion{
    //	int slot;
    //	int lev ;		/*0���-5��ͣ�6�ر�*/
    //	int	recDelay;	//0-6:10s,20s,30s,1m,2m,5m,10m
    //	int data[18];	//�ӵ�λ����λΪ0-21
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
        int lev ;		//0-100������
        int	recDelay;	//0-6:10s,20s,30s,1m,2m,5m,10m	���� 0-100
        char data[DVO_MOTION_MAX_ROWS * DVO_MOTION_MAX_COLS / 8];
        tHandle handle;
        int reserved[32];//int resved[0] x ��intresved[1] y int reserved[2] width��reserved[3] height
    };

    struct tPMotion
    {
        int chns;
        int x;
        int y;
        int w;
        int h;
    };

    struct tAlarmMotionDataEx//�����ϴ�
    {
        unsigned int slot; //��Ƶ���ںţ���0��ʼ
        long tmSec;        //ʱ�䣬��1970��1��1�ž���������
        unsigned int tmMSec;//���룬0-1000	
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
        RECT rt[MAX_SMARTSEARCH_RECT]; //����704 * 576
        int direction;//0-ȫ��  4-�� 6-��  8-�� 2-��
        int reserved[32];
    };
    //Ӳ����Ϣ��ȡ
    struct tEachHarddiskInfo
    {
        INT64 volume; //Ӳ������
        INT64 free;//��������
        int state;//Ӳ��״̬ 0- ���� 1-���� 2-������
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
        APP_NET_TCP_IMG_WB_AUTO 		= 0, //�Զ���ƽ�⡣
        APP_NET_TCP_IMG_WB_CUSTOM_1,		//�Զ����ƽ��ģʽ1.
        APP_NET_TCP_IMG_WB_CUSTOM_2,		//�Զ����ƽ��ģʽ2.
        APP_NET_TCP_IMG_WB_CUSTOM_3,		//�Զ����ƽ��ģʽ3.
        APP_NET_TCP_IMG_WB_MODE_ALL,
    } ;
    enum APP_NET_TCP_IMG_EXP_MODE
    {
        APP_NET_TCP_IMG_EXP_AUTO		= 0, //�Զ��ع�
        APP_NET_TCP_IMG_EXP_MANUAL	= 1, //�ֶ��ع�
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
        int		bae;		//�Ƿ��Զ��ع�
        int		eshutter;//����HW_IPCAM_ESHUTTER_E
        int		agcgain;//0-30
        int		blackwhitemode;		//��ת��
        int		badj;		//�Զ�����
        int		blacklevel;		//�ڵ�ƽ[-128,128]��badj��Чʱ�ò�����Ч

        int     bAutoWB; //�Զ���ƽ��
        int     wb_mode; //��ƽ��ģʽ APP_NET_TCP_IMG_WB_MODE
        int     rgb_gain[3];//rgb���� 0-16384

        int		noisefilter;		//��������0-5��5���badj��Чʱ�ò�����Ч
        int		sharp;				//���0-5��5���badj��Чʱ�ò�����Ч

        int		luma;				//����

        int		reserved[13];
    };
    struct tIPCAM_BLANCEBLACK{
        int solt;//��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
        int wb_mode ;//��ƽ��ģʽ����Χ�ο�APP_NET_TCP_IMG_WB_MODE
        //int blackleavel;
        int rgb_gain[3];//R���棬ȡֵ��Χ��0~16000 G���棬ȡֵ��Χ��0~16000 B���棬ȡֵ��Χ��0~16000
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
        int bae; //�ع�ģʽ����Χ�ο�APP_NET_TCP_IMG_EXP_MODE
        int eshutter; //APP_NET_TCP_ESHUTTER_E
        int agcgain; //������档��Χ��1~60
    };
    /*mask*/
#define MAX_MASK_RECT 4
    struct tMask
    {
        int slot;
        int count;		//�ڵ�������������4��
        int lev;		//��ʱδʹ��
        RECT rt[MAX_MASK_RECT];
    };

    //ǿ������,��ǰֻ��enable��max_luma_night ����
    struct tBlackMaskBLC{
        int slot;

        int enable;

        /* ҹ���luma���ֵ,С�ڸ�ֵ������������� */
        int max_luma_night;

        /* ������ԭ����block��yֵӦ���ڴ˷�Χ��  */
        int block_y_max;
        int block_y_min;

        /* ���������yֵС�ڸ�ֵ��˵�������ȥ����Ҫ�ָ��Զ��ع� */
        int block_y_night_normal;

        int mode; //0: �̶����� 1:�̶����� 2:�Զ�
        int fixedAgc;
        int fixedEshutter;

        int max_eshutter;
        int min_eshutter;
        int max_agc;
        int min_agc;

        /* 
        * ����ѡ��3x3��9������ ��λ��ʾѡ���ѡ��
        * �� (1<<0|1<<3) ��ѡ���˵�1������͵�4������
        */
        int area_map; 

        /* �������� */
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
        u8		b; //b������ȡֵ��Χ0~255.
        u8		g;//g������ȡֵ��Χ0~255.
        u8		r;//r������ȡֵ��Χ0~255.
        u8		a;//͸���ȣ�ȡֵ��Χ0~255.


    }app_net_tcp_osd_color_t;
    /*osd channel struct*/
    struct tOsdChannel
    {
        int slot;
        int isEnable;		//�Ƿ���ʾ
        int left;			//0-703
        int top;			//0-575
        char name[32];		//ͨ������
        int fontsize;  //�����С���� 16-60��Χ
        app_net_tcp_osd_color_t color; //������ɫ
        int type;//0 channel 1 ���� APP_NET_TCP_OSD_AREA_UPPER_RIGHT 2 APP_NET_TCP_OSD_AREA_BOTTOM_RIGHT
        int outline ;//�Ƿ񹴱� 0�������ߣ�1������

    };

    /*osd struct*/
    struct tOsdDate
    {
        int slot;				//��0��ʼ
        int isEnable;			//�Ƿ���ʾ
        int left;				//0-703
        int top;				//0-575
        int type;				//		��Ч
        int fontsize;			//�����С���� 16-60��Χ
        app_net_tcp_osd_color_t color;  //������ɫ
        int isShowWeek;			//�Ƿ���ʾ����
        int outline;			//�Ƿ񹴱� 0�������ߣ�1������

    };
    /*encode quality struct*/
    struct tVideoQuality
    {
        int slot;			// 0��h264 1080p 1:  h264 D1	2:  h265 1080p
        int encodeType;		/*0-CIF, 1-D1 2-720p,3-1080p,4-1280*960,5-1024*768,0xff-qcif*/         
        int qualityLev;		/* 0-���,1-�κã�2-�Ϻã�3һ��*/
        int maxBps;			//0-����,1-��С 2-�� 3-�ϴ� 4-���CIF(2000,400,600,800,1000),D1(4000,800,1000,1400,1600),720p(8000,2000,2500,3000,4000)

        int vbr;			//0-cbr 1-vbr 0�����ʣ�1������
        int framerate;		//0-ȫ֡�� 1-24 ʵ��֡��
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
        int presetno;//Ԥ�õ�Ż����ƶ����ٶȣ�0-64��
        RECT rect;			//����Ŵ������704��576
    };


    struct app_net_tcp_audio_config_t
    {
        int			chn; //��Ƶ����ͨ����ȡֵ0~MAX-1.
        int			enc_type;//��Ƶ�������ͣ�0��711_a��1��711_u��2��726��3��AAC
        int			samplerate;//�����ʣ��ο�APP_NET_TCP_AUDIO_AUDIO_FS��
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
    };//��Ƶ�����ʡ�
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
        APP_NET_MIRROR_MODE	mirror_mode;//��תģʽ
        UINT							rev[6];
    };

    struct tEvnParam
    {
        int chn;
        int type; //0 ����1 ���� 2 �Զ�
    };

    struct tCustomAreaOSD
    {
        unsigned int		chn; //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
        unsigned int	    enable; //OSD��ʾ���أ�1����ʾ��0���رա�
        unsigned int		x_edge;//x������߾ࡣȡֵ�����������
        unsigned int		y_edge; //y�����ϱ߾ࡣȡֵ���������߶�	
        unsigned int	    size;//�����С��ȡֵ��16~60
        unsigned int	    max_line_num;  //�Զ���OSD���������ʾ����
        unsigned int       max_text_len; //ÿ������ַ�����
        unsigned int         stream ;//0 ������ 1������,2 ������
        unsigned int		rev[8];	

    };
    struct tCustomRow
    {
        unsigned int				  chn; //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
        unsigned int			      row_no; //�к� ��0��ʼ

        app_net_tcp_osd_color_t		  color;//������ɫ
        unsigned int			      outline;//�Ƿ񹵱�
        unsigned int         stream ;//0 ������ 1������,2 ������
        unsigned int			   rev[7];	
    };
    struct tCustomRow_Text //�����ı���
    {
        unsigned int chn;
        unsigned int row_no;
        char ctext_str[256];
        unsigned int         stream ;//0 ������ 1������,2 ������
        int rev[7];

    };
    struct tCustomRow_Text2 //�����ı���
    {
        unsigned int chn;
        unsigned int row_no;
        char ctext_str[256];
        unsigned int         stream ;//0 ������ 1������,2 ������
        u32       set_color; //�Ƿ�������ɫ 0:������ 1:����
        u32				 outline;//�Ƿ񹴱� 0�������ߣ�1������
        app_net_tcp_osd_color_t		 color;//������ɫ
        int rev[4];

    };
    typedef struct{
        u32		chn; //�������ͨ���š�ȡֵ0~MAX-1.
        u32     data_type; // ==>datatype���豸���Ͷ�Ӧ����ĵ�����A
        int		state; //���״̬��0����������1��������
        int 		rev[5];
    }app_net_tcp_event_act_485_t;

    //typedef struct{
    //	u32		chn; //�������ͨ���š�ȡֵ0~MAX-1.
    //    u32     data_type; //0���¶ȣ�1��ʪ�ȣ�2��CO2
    //}app_net_tcp_com_chn_485_t;
    //typedef struct{
    //	u32		chn;//��Ƶ����ͨ���ţ�ȡֵ0~	MAX-1.
    //    u32     data_type; //0���¶ȣ�1��ʪ�ȣ�2��CO2
    //	int		value[256]; //���¶ȣ�ʪ�ȣ�CO2���ԣ���1λ�����ޣ���2λ������
    //}app_net_tcp_485_alarm_para_t;
    ////�޶����ݽṹ����ֵ �趨���Ϊ��Ч��Ч �ϲ���������Чֵ�жϲ�����׼ȷ��
    typedef struct{
        u32		chn;//��Ƶ����ͨ���ţ�ȡֵ0~	MAX-1.
        u32     data_type; // ==>datatype���豸���Ͷ�Ӧ����ĵ�����A
        int		value[255]; //���¶ȣ�ʪ�ȣ�CO2���ԣ���1λ�����ޣ���2λ������
        int status;//�Ƿ���Ч��0����Ч��1����Ч
    }app_net_tcp_485_alarm_para_t;
    typedef struct{
        int enable;//0:����,1:�ر�.
        int dev_type; //0-ICPDAS DL-100TM485
        int dev_id;//�豸ID
        int interval;//�ɼ�ʱ����
        char rev[512];
    }app_net_tcp_485_dev_t;
    /*

    */
    typedef struct{
        u32     data_type; // ==>datatype���豸���Ͷ�Ӧ����ĵ�����A
        int row; //��ʾ��
        int col;//��ʾ��
        char prefix[32];//��ʾǰ׺
        char suffix[32];//��ʾ��׺
        int dev_type;//
        int osd;//�Ƿ�osd 0 ������ 1 ����
        char rev[504];
    }app_net_tcp_485_osd_para_t;
    typedef struct{
        u32     data_type; // ==>datatype���豸���Ͷ�Ӧ����ĵ�����A
    }app_net_tcp_485_osd_data_type_t;

    typedef struct{
        u32 dev_type;//485�����豸���� 0ΪICPDAS DL-100TM485
        u32 data[255]; //����DL-100���ԣ���һλΪ�¶ȣ��ڶ�λΪʪ��
        u32 status; //�Ƿ���Ч��0����Ч��1����Ч
    }app_net_tcp_485_input_data_t;
    ////////////////�������� Ϊ ��DVO_ALARM_TEMPR_CTRL_MOTION ʱ�����¿ر�����Ϣ���øýṹ����data���� //////////////////
    //ע��ýṹ���ϲ�����λ����ԭʼ���ݶ�4�ֽ�������Ҫ��С��ת����
    typedef struct{
        u32     dev_type;//485�����豸���� 0ΪICPDAS DL-100TM485
        char	 	_485_alarm[16]; //0:�ޱ�����1������ ��DL-100���ԣ���һλΪ�����¶ȱ������ڶ�λΪ�����¶ȱ���������λΪʪ�ȱ���������λΪCO2����
        u32     _485_data[16]; //��DL-100���ԣ���һλΪ�����¶ȣ��ڶ�λΪ�����¶ȣ�����λΪʪ�ȣ�����λΪCO2
        char	 	rev[428];
    }app_tempr_ctrl_alram_data;

    typedef struct {
        unsigned	   enable; //0:�رգ�1����
        char        addr[128]; //NTP������IP������
        unsigned    port; //NTP�������˿�
        unsigned    inteval; //ͬ�����
        unsigned    set_time_zone; //�Ƿ�����ʱ����0�������� 1����
        int         time_zone;
        char        rev[248];
    }app_net_tcp_sys_ntp_para_t;


    /////////////////////////
    ///DVO_ALARM_TEMPR_CTRL_ERROR_ �¼��з��ظ��¼��ṹ
    //˵����alarm_id��Ŵ�1000��ʼ��1000-485���빦�ܹ��ϣ�1001-485���빦�ָܻ���1002-NTP���ܹ��ϣ�1003-NTP���ָܻ�  1004-����NTP������������
    typedef struct {
        unsigned    alarm_id; 
        unsigned    alarm_type; //0-�豸�����ϱ���1-�豸���ϻָ��ϱ���2-�豸�¼��ϱ�
        unsigned    para_num; //��������
        unsigned    alarm_para[4]; //�豸��ز���
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
        unsigned  index; //ͬ��
        unsigned  active_level; //0-��· 1-��·
        unsigned  set_in_out; //�ܷ�ı䱨������ 0-���� 1-�ܣ����ֶ�Ϊ���ܸķ���ʱ������IO��������Ͳ������ڽ�����
        unsigned  in_out; //0-�������� 1-�������
        unsigned  rev[12];
    }app_net_tcp_io_alarm_attr_t;

    typedef struct{
        unsigned  state; //0-�����ã�1-����
        unsigned  rev[7];
    }app_net_tcp_set_io_alarm_state_t;

    // ����/��ȡ�����ͺŽṹ
    typedef struct
    {
        /*
        DataType:0x0001
        CmdSubType:0x0023
        */
        char      name[128];	//�����ͺ����� ���嶨������
        /*
        �����ͺ�����	��������
        (null)			�����ͺ�Ϊ�գ���ʾ���Ϊ��ͨ����������⹦��
        OSD-IPC			֧��OSD���ӹ���
        FC-IPC			֧�������á����ܱ�״̬���ӹ���
        TH-IPC			֧�ֻ������¶ȡ�ʪ�ȵļ�⹦��
        THC-IPC			֧�ֻ������¶ȡ�ʪ�ȡ�������̼�ļ�⹦��
        CM-IPC			֧������ģʽ����
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
        IPCModeType	nMode;		// �ͺŴ���
        char*	pszModel;		// �ͺ�����
        char*	pszName;		// ��ʾ����
        char*	pszFunction;	// ��������
    };

}


#endif
