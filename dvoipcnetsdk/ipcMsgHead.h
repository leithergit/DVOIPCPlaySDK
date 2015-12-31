
/**
*  COPYRIGHT NOTICE 
*  Copyright (c) 2015, Diveo Technology Co.,Ltd (www.diveo.com.cn)
*  All rights reserved.
*
*  @file        ipcMsgHead.h
*  @author      luo_ws(luo_ws@diveo.com.cn)
*  @date        2015/08/13 09:34
*
*  @brief       IPCͨ��Э����ص����ݽṹ����
*  @note        ��
*
*  @version
*    - v0.16    2015/08/13 09:34    luo_ws 
*/

#ifndef DVO_IPC_MSG_HEAD_H_
#define DVO_IPC_MSG_HEAD_H_


#include "dvoipcnetsdk_def.h"


///////////////////////�豸ͨ��Э��/���� begin///////////////////////////////////////////////////

// ������������Ƶ����Ƶ
#define		APP_NET_TCP_TYPE_DATA_CMD					    (0x0001)
#define		APP_NET_TCP_TYPE_DATA_VIDEO					    (0x0002)
#define		APP_NET_TCP_TYPE_DATA_AUDIO					    (0x0003)

/*
������ϵͳ����Ϣ��Ϊ�����࣬��Ϊϵͳ�����룬ͼ��PTZ���¼�����Ƶ���������豸�������ã���Ӧ��������CmdType���£�
 */
#define		APP_NET_TCP_TYPE_SYS						    (0x0001)
#define		APP_NET_TCP_TYPE_ENC						    (0x0002)
#define		APP_NET_TCP_TYPE_IMG						    (0x0003)
#define		APP_NET_TCP_TYPE_PTZ						    (0x0004)
#define		APP_NET_TCP_TYPE_EVT						    (0x0005)
#define		APP_NET_TCP_TYPE_ADO						    (0x0006)
#define		APP_NET_TCP_TYPE_MSC						    (0x0007)
#define		APP_NET_TCP_TYPE_DEV						    (0x0008)


//ϵͳ����Ϣ
#define		APP_NET_TCP_TYPE_SYS_SUB_LOGIN				    (0x0001)    //�û���¼
#define		APP_NET_TCP_TYPE_SYS_SUB_LOGOUT				    (0x0002)    //�û��˳�
#define		APP_NET_TCP_TYPE_SYS_SUB_NETINFO_SET		    (0x0003)    //��������
#define		APP_NET_TCP_TYPE_SYS_SUB_NETINFO_GET		    (0x0004)    //�������ò�ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_DEVTIME_SET		    (0x0005)    //ʱ������
#define		APP_NET_TCP_TYPE_SYS_SUB_DEVTIME_GET		    (0x0006)    //ʱ���ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_DEV_RESTART		    (0x0007)    //ϵͳ����
#define		APP_NET_TCP_TYPE_SYS_SUB_DEV_SHUTDOWN		    (0x0008)    //ϵͳ�ػ�
#define		APP_NET_TCP_TYPE_SYS_SUB_DEVINFO_RESET 		    (0x0009)    //�ָ�ϵͳĬ������
#define		APP_NET_TCP_TYPE_SYS_SUB_DEVINFO_UPDATE 	    (0x000A)    //ϵͳ����
#define		APP_NET_TCP_TYPE_SYS_SUB_SERIALPORT_SET 	    (0x000B)    //��������
#define		APP_NET_TCP_TYPE_SYS_SUB_SERIALPORT_GET 	    (0x000C)    //���ڲ�ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_DEV_RESOURCE 	        (0x000D)    //�豸��Դ��ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_DEV_LOG_GET 	        (0x000E)    //�豸��־��ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_DEVINFO_GET 	        (0x000F)    //�豸��Ϣ��ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_DEVINFO_SET 	        (0x0010)    //�豸��Ϣ����
#define		APP_NET_TCP_TYPE_SYS_SUB_DISKINFO_GET 	        (0x0011)    //������Ϣ��ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_HEARTBEAT_SET 	        (0x0012)    //ϵͳ��������
#define		APP_NET_TCP_TYPE_SYS_SUB_HEARTBEAT_GET 	        (0x0013)    //ϵͳ������ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_HEARTBEAT  	        (0x0014)    //ϵͳ����
#define		APP_NET_TCP_TYPE_SYS_SUB_SERIAL_MAC_SET 	    (0x0015)    //���к�MAC��ַ����
#define		APP_NET_TCP_TYPE_SYS_SUB_SERIAL_MAC_GET 	    (0x0016)    //���к�MAC��ַ��ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_VIDEO_OUT_SET 	        (0x0017)    //��Ƶ����ӿ�����
#define		APP_NET_TCP_TYPE_SYS_SUB_VIDEO_OUT_GET 	        (0x0018)    //��Ƶ����ӿڲ�ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_NTP_SET 	            (0x0019)    //NTP��������
#define		APP_NET_TCP_TYPE_SYS_SUB_NTP_GET 	            (0x001A)    //NTP������ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_FUNCTION_GET           (0x001B)    //�豸�����б��ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_FUNCTION_REPORT        (0x001C)    //�豸����״̬�ϱ�
#define		APP_NET_TCP_TYPE_SYS_SUB_NTP_TEST 	            (0x001D)    //��ʼNTP���ܲ���

#define		APP_NET_TCP_TYPE_SYS_SUB_ALARM_IO_NUM_GET       (0x001E)    //IO�����˿�������ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_ALARM_IO_ATTR_GET      (0x001F)    //IO�����˿����Բ�ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_ALARM_IO_ATTR_SET      (0x0020)    //IO�����˿���������
#define		APP_NET_TCP_TYPE_SYS_SUB_ALARM_IO_USE_GET       (0x0022)    //IO�����˿�����״̬��ѯ
#define		APP_NET_TCP_TYPE_SYS_SUB_ALARM_IO_USE_SET       (0x0021)    //IO�����˿���������


/// ��Ƶ������Ϣ
#define		APP_NET_TCP_TYPE_VIDEO_SUB_ENC_SET			    (0x0001)    //������Ƶ����
#define		APP_NET_TCP_TYPE_VIDEO_SUB_ENC_GET			    (0x0002)    //��ȡ������Ϣ
#define		APP_NET_TCP_TYPE_VIDEO_SUB_STREAM_ENABLE        (0x0003)    //��������

#define		APP_NET_TCP_TYPE_VIDEO_SUB_OSD_CFG_SET	        (0x0005)    //OSD��������
#define		APP_NET_TCP_TYPE_VIDEO_SUB_OSD_CFG_GET	        (0x0006)    //OSD������ȡ
#define		APP_NET_TCP_TYPE_VIDEO_SUB_PM_CFG_SET	        (0x0007)    //PM��������
#define		APP_NET_TCP_TYPE_VIDEO_SUB_OSD_CUSTOM_ZONE_SET  (0x0008)    //�Զ���OSD��������
#define		APP_NET_TCP_TYPE_VIDEO_SUB_OSD_CUSTOM_ZONE_GET  (0x0009)    //�Զ���OSD�����ȡ
#define		APP_NET_TCP_TYPE_VIDEO_SUB_OSD_CUSTOM_LINE_SET  (0x000A)    //�Զ���OSD�в�������
#define		APP_NET_TCP_TYPE_VIDEO_SUB_OSD_CUSTOM_LINE_GET  (0x000B)    //�Զ���OSD�в�����ȡ
#define		APP_NET_TCP_TYPE_VIDEO_SUB_OSD_CUSTOM_DATA_SET  (0x000C)    //�Զ���OSD����������
#define		APP_NET_TCP_TYPE_VIDEO_SUB_OSD_CUSTOM_DATA_GET  (0x000D)    //�Զ���OSD�����ݻ�ȡ

/// ͼ������Ϣ
#define		APP_NET_TCP_TYPE_IMAGE_SUB_COLOR_SET            (0x0001)    //ͼ����ɫ��������color
#define		APP_NET_TCP_TYPE_IMAGE_SUB_COLOR_GET            (0x0002)    //ͼ����ɫ������ȡ
#define		APP_NET_TCP_TYPE_IMAGE_SUB_EXPOSURE_SET         (0x0003)    //ͼ���ع��������expose
#define		APP_NET_TCP_TYPE_IMAGE_SUB_EXPOSURE_GET         (0x0004)    //ͼ���ع������ȡ
#define		APP_NET_TCP_TYPE_IMAGE_SUB_WHITE_BALANCE_SET    (0x0005)    //ͼ���ƽ���������white balance
#define		APP_NET_TCP_TYPE_IMAGE_SUB_WHITE_BALANCE_GET    (0x0006)    //ͼ���ƽ�������ȡ
#define		APP_NET_TCP_TYPE_IMAGE_SUB_DAY_MODE_SET         (0x0007)    //ͼ������ҹģʽ����day or night mode
#define		APP_NET_TCP_TYPE_IMAGE_SUB_DAY_MODE_GET         (0x0008)    //ͼ������ҹģʽ��ȡ
#define		APP_NET_TCP_TYPE_IMAGE_SUB_MIIROR_MODE_SET      (0x0009)    //ͼ����Ƶģʽ����mirror mode
#define		APP_NET_TCP_TYPE_IMAGE_SUB_MIIROR_MODE_GET      (0x000A)    //ͼ����Ƶģʽ��ȡ

/// PTZ
#define		APP_NET_TCP_TYPE_PTZ_SUB_PARAM_SET              (0x0001)    //PTZ����
#define		APP_NET_TCP_TYPE_PTZ_SUB_TRANS_DATA             (0x0002)    //PTZ͸��

/// ���������Ϣ
#define		APP_NET_TCP_TYPE_EVT_SUB_IOOUT_ENABLE_SET       (0x0001)    //IO�������ʹ������
#define		APP_NET_TCP_TYPE_EVT_SUB_IOOUT_ENABLE_GET       (0x0002)    //IO�������ʹ�ܲ�ѯ

#define		APP_NET_TCP_TYPE_EVT_SUB_MD_RECT_SET			(0x0003)    //MD��������
#define		APP_NET_TCP_TYPE_EVT_SUB_MD_RECT_GET			(0x0004)    //MD�����ѯ
#define		APP_NET_TCP_TYPE_EVT_SUB_MD_PARAM_SET			(0x0005)    //MD��������
#define		APP_NET_TCP_TYPE_EVT_SUB_MD_PARAM_GET			(0x0006)    //MD������ѯ

#define		APP_NET_TCP_TYPE_EVT_SUB_485_ENABLE_SET         (0x0007)    //485���뱨�������ʹ������
#define		APP_NET_TCP_TYPE_EVT_SUB_485_ENABLE_GET         (0x0008)    //485���뱨�������ʹ�ܲ�ѯ
#define		APP_NET_TCP_TYPE_EVT_SUB_485_PARAM_SET			(0x0009)    //485���뱨���Ĳ�������
#define		APP_NET_TCP_TYPE_EVT_SUB_485_PARAM_GET			(0x000A)    //485���뱨���Ĳ�����ѯ

/// ��Ƶ�����Ϣ,inΪ������Ƶ(ʰ����)��outΪ�����Ƶ(�����Խ�)
#define		APP_NET_TCP_TYPE_AUDIO_SUB_IN_ENABLE_SET        (0x0001)    //����������Ƶ�����Ŀ���
#define		APP_NET_TCP_TYPE_AUDIO_SUB_IN_ENABLE_GET        (0x0002)    //��ȡ������Ƶ�������ص�����
#define		APP_NET_TCP_TYPE_AUDIO_SUB_OUT_ENABLE_SET       (0x0003)    //���������Ƶ�����Ŀ���
#define		APP_NET_TCP_TYPE_AUDIO_SUB_OUT_ENABLE_GET       (0x0004)    //��ȡ�����Ƶ�������ص�����
#define		APP_NET_TCP_TYPE_AUDIO_SUB_IN_ENC_SET			(0x0005)    //����������Ƶ����
#define		APP_NET_TCP_TYPE_AUDIO_SUB_IN_ENC_GET			(0x0006)    //��ȡ������Ƶ������Ϣ
#define		APP_NET_TCP_TYPE_AUDIO_SUB_OUT_ENC_SET			(0x0007)    //���������Ƶ����
#define		APP_NET_TCP_TYPE_AUDIO_SUB_OUT_ENC_GET			(0x0008)    //��ȡ�����Ƶ������Ϣ
#define		APP_NET_TCP_TYPE_AUDIO_SUB_IN_STREAM			(0x0009)    //������Ƶ������(¼��,�豸�ֳ�����)
#define		APP_NET_TCP_TYPE_AUDIO_SUB_OUT_STREAM			(0x000A)    //�����Ƶ������(�Խ�)

/// ������Ϣ
#define		APP_NET_TCP_TYPE_MSC_SUB_CHIPSET_MATCH_GET      (0x0001)    //����оƬƥ���ѯ
#define		APP_NET_TCP_TYPE_MSC_SUB_WIFI_PARAM_SET			(0x0002)    //WIFI��������
#define		APP_NET_TCP_TYPE_MSC_SUB_WIFI_STATE_GET			(0x0003)    //WIFI����״̬��ѯ
#define		APP_NET_TCP_TYPE_MSC_SUB_AUDIO_OUT_SET			(0x0004)    //������Ƶ�������

#define		APP_NET_TCP_TYPE_MSC_SUB_485_DEV_SET			(0x0005)    //485�豸�Ĳ�������
#define		APP_NET_TCP_TYPE_MSC_SUB_485_DEV_GET			(0x0006)    //485�豸�Ĳ�����ѯ
#define		APP_NET_TCP_TYPE_MSC_SUB_485_OSD_SET			(0x0007)    //485�����OSD��������
#define		APP_NET_TCP_TYPE_MSC_SUB_485_OSD_GET			(0x0008)    //485�����OSD������ѯ
#define		APP_NET_TCP_TYPE_MSC_SUB_485_DATA_GET			(0x0009)    //��ȡ485���������


///////////////////////�豸ͨ��Э��/���� end///////////////////////////////////////////////////

/// �����豸����
#define		APP_NET_DISCOVERY_TYPE_DEV						    (0xFFFF)

#define		APP_NET_TYPE_DISCOVERY_SUB_SEARCH_DEVICE_REQ        (0x0001)    //�����豸����
#define		APP_NET_TYPE_DISCOVERY_SUB_SEARCH_DEVICE_ACK        (0x0002)    //�����豸������Ӧ
#define		APP_NET_TYPE_DISCOVERY_SUB_MODIFY_MAC_REQ			(0x0003)    //�޸�MAC��ַ����
#define		APP_NET_TYPE_DISCOVERY_SUB_MODIFY_MAC_ACK			(0x0004)    //�޸�MAC��ַ��Ӧ
#define		APP_NET_TYPE_DISCOVERY_SUB_MODIFY_IP_REQ			(0x0005)    //�޸�IP��ַ����
#define		APP_NET_TYPE_DISCOVERY_SUB_MODIFY_IP_ACK			(0x0006)    //�޸�IP��ַ��Ӧ

//////////////////////////////////////////////////////////////////////////

typedef unsigned char  	    u8;	/**< UNSIGNED 8-bit data type */
typedef unsigned short 	    u16;/**< UNSIGNED 16-bit data type */
typedef unsigned int   	   	u32;/**< UNSIGNED 32-bit data type */
typedef UINT64           	u64;/**< UNSIGNED 64-bit data type */

typedef u8      U8;
typedef u16     U16;
typedef u32     U32;
typedef u64     U64;

#define NET_MSG_HEAD_MAGIC          {0xF5, 0x5A, 0xA5, 0x5F}
#define DVONET_MSG_HEAD_STREAM_TAG  0x44564F4D

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


//#pragma pack(1)
//msg head 
struct MSG_HEAD
{
    U8  Magic1[4];  // 	���ֽ� �̶����ݣ�0xF5��0x5A��0xA5��0x5F��
    U32	Pktlen;		//	����U32 ���ݰ��ܳ��ȡ�
    U16	Version;	//  ����U16 Э��汾�ţ���8λΪ���汾�ţ���8λΪ�Ӱ汾�š�
    U16	Hwtype;		//	����U16 Ӳ������(����)
    U32	Sn;		    //  ����U32 ֡���к�(����)
    U16	CmdType;	//	����U16 ��ʾ��������
    U16 CmdSubType;	//	����U16 ��ʾ����������
    U16 DataType;	//  ����U16 ���ݰ�����1���������2����Ƶ����3����Ƶ��
    U16 Rev1;		//	����U16 �����ֶ�
    U32 Rev2;		//	����U32 �����ֶ�
    U32 Rev3;		//	����U32 �����ֶ�
};

//304+32
//ϵͳ�����������������CmdType Ϊ0x0001
//<1>�û���¼
//DataType:
//��0x0001
//CmdSubType��  
//��0x0001
//CmdPkt��
typedef struct
{
    char	user_name[20];          //�ַ����� 
    char	user_pwd[20];           //�ַ����� 
    U32		rev[8];
}app_net_tcp_sys_login_t;

//AckPkt��
typedef struct tagloginfo{
    int		    state;		        //0���ɹ� 1��ʧ�ܡ�
    char		dev_type[64];       //�豸���ͣ��ַ�����
    char		dev_name[64];       //�豸�����ַ�����
    char		dev_id[64];	        //�豸ID���ַ�����
    char		hw_version[32];     //�豸Ӳ���汾���ַ�����
    char		sdk_sw_version[32]; //�豸���SDK�汾���ַ�����
    char		app_sw_version[32]; //�豸Ӧ������汾���ַ�����
    char		rev1[32];
}app_net_tcp_sys_logo_info_t;


typedef struct{
    int		    chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
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
    u32			chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32			schn;               //������ţ�0:��������1����������2������������
    u32			rev[2];
}app_net_tcp_com_schn_t;


//������ VideoStream
typedef struct{
    U32			chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    U32			stream;             //������ţ�0:��������1����������2������������
    U32			enable;             //0:ֹͣ���ͣ�1����ʼ���͡�
    U32			nettype;            //0:tcp,1:udp.
    U32			port;               //udp port.
    U32			rev[3];
}app_net_tcp_enc_ctl_t;

typedef struct{
    U32			chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    U32			stream;             //������ţ�0:��������1����������2������������
    U32			state;              //0:�ɹ���1��ʧ�ܡ�
    U32			rev;
}app_net_tcp_enc_ctl_ack_t;

//IPC ������Ƶ����ͷ
typedef struct{
    U32		    chn;	            //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    U32		    stream;             //������ţ�0:��������1����������2������������
    U32		    frame_type;         //֡���ͣ���Χ�ο�APP_NET_TCP_STREAM_TYPE
    U32		    frame_num;          //֡��ţ���Χ0~0xFFFFFFFF.
    U32		    sec;	            //ʱ���,��λ���룬Ϊ1970��������������
    U32		    usec;               //ʱ���,��λ��΢�롣
    U32		    rev[2];
}app_net_tcp_enc_stream_head_t;


typedef struct{
    U32			chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    U32			type;               //OSD��ţ�ȡֵ��0,ʱ�� 1 �ַ���
    U32			rev[2];
}app_net_tcp_enc_chn_t;


typedef enum{
    APP_NET_TCP_OSD_AREA_UPPER_LEFT = 1,//���ϡ�
    APP_NET_TCP_OSD_AREA_UPPER_RIGHT,	//���ϡ�
    APP_NET_TCP_OSD_AREA_BOTTOM_LEFT,   //���¡�
    APP_NET_TCP_OSD_AREA_BOTTOM_RIGHT,  //���¡�
}APP_NET_TCP_OSD_AREA;


//typedef struct{
//    u8		    b;                  //b������ȡֵ��Χ0~255.
//    u8		    g;                  //g������ȡֵ��Χ0~255.
//    u8		    r;                  //r������ȡֵ��Χ0~255.
//    u8		    a;                  //͸���ȣ�ȡֵ��Χ0~255.
//}pm_color_t;

typedef struct{
    u8		    b;                  //b������ȡֵ��Χ0~255.
    u8		    g;                  //g������ȡֵ��Χ0~255.
    u8		    r;                  //r������ȡֵ��Χ0~255.
    u8		    a;                  //͸���ȣ�ȡֵ��Χ0~255.

}app_net_tcp_enc_color_t;

typedef app_net_tcp_enc_color_t app_net_tcp_osd_color_t;
typedef app_net_tcp_enc_color_t pm_color_t;


typedef struct{
    U32         chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    //u32       sn;                 //OSD��ţ�ȡֵ��0,1,2������ڼ�·OSD��
    U32         enable;             //OSD��ʾ���أ�1����ʾ��0���رա�
    //u32       area;               //osdλ�ã��ο�APP_NET_TCP_OSD_AREA
    U32         x_edge;             //x����߾ࡣȡֵ��0-720.
    U32         y_edge;             //y����߾ࡣȡֵ��0-575	
    pm_color_t  color;              //OSD��ɫ���ο�app_net_tcp_osd_color_t�ṹ��
    U32         size;               //�����С��ȡֵ��16~60
    U32         type;               //���� 0��ʱ�䣬1���Զ����ַ�����
    U32         len;                //����ַ����ȡ�
    U32         outline;            //�Ƿ񹴱� 0�������ߣ�1������
    U32         rev[7];
    //char      tag[64];            //����ַ����ݡ���󳤶�64�ֽڡ�
    char        tag[32];
}app_net_tcp_osd_cfg_t;


typedef struct{
    u32			chn;//��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32         stream;
    u32			rev[2];
}app_net_tcp_custom_osd_zone_chn_t;

typedef struct{
    u32		    chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32	        stream;             //������ţ�0:��������1����������2������������
    u32		    enable;             //OSD��ʾ���أ�1����ʾ��0���رա�
    u32		    x_edge;             //x������߾ࡣȡֵ�����������
    u32		    y_edge;             //y�����ϱ߾ࡣȡֵ���������߶�	
    u32		    size;               //�����С��ȡֵ��16~60
    u32 	    max_line_num;       //�Զ���OSD���������ʾ����
    u32         max_text_len;       //ÿ������ַ�����
    u32		    rev[8];
}app_net_tcp_custom_osd_zone_para_t;


typedef struct{
    u32		    chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32	       	stream;             //������ţ�0:��������1����������2������������
    u32			enable;             //OSD��ʾ���أ�1����ʾ��0���رա�
    u32			x_edge;             //x������߾ࡣȡֵ�����������
    u32			y_edge;             //y�����ϱ߾ࡣȡֵ���������߶�	
    u32			size;               //�����С��ȡֵ��16~60
    u32			rev[10];
}app_net_tcp_custom_osd_zone_cfg_t;

typedef struct{
    u32	        chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32	        stream;             //������ţ�0:��������1����������2������������
    u32         max_line_num;       //�Զ���OSD���������ʾ����,0��Ϊ����ʧ��
    u32         max_text_len;       //ÿ������ַ�����
    u32	        rev[4];
}app_net_tcp_custom_osd_zone_cfg_ack_t;

typedef struct{
    u32         chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32	        stream;             //������ţ�0:��������1����������2������������
    u32         row_no;             //�к�
    u32	        rev[5];
}app_net_tcp_custom_osd_line_chn_t;

typedef struct{
    u32         chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32         stream;             //������ţ�0:��������1����������2������������
    u32         row_no;             //�к� ��0��ʼ
    pm_color_t  color;              //������ɫ
    u32         outline;            //�Ƿ񹵱�
    u32         rev[7];
}app_net_tcp_custom_osd_line_cfg_t;
/*
typedef struct{
    u32         chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32         stream;             //������ţ�0:��������1����������2������������
    u32         row_no;             //�к� ��0��ʼ
    u32         length;             //�ַ�����
    u8          text[256];          //������255���ַ�
    u32         rev[8];
}app_net_tcp_custom_osd_line_data_t;


typedef struct{
    u32		    chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32	        stream;             //������ţ�0:��������1����������2������������
    u32		    row_no;             //�к� ��0��ʼ
    u32		    length;             //�ַ�����
    u8          text[256];          //������255���ַ�
    u32         set_color;          //�Ƿ�������ɫ 0:������ 1:����
    u32         outline;            //�Ƿ񹴱� 0�������ߣ�1������
    pm_color_t  color;              //������ɫ
    u32		    rev[5];
}app_net_tcp_custom_osd_line_data_t2;
*/

typedef struct{
    u32		    chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32	        stream;             //������ţ�0:��������1����������2������������
    u32		    row_no;             //�к� ��0��ʼ
    u32		    length;             //�ַ�����
    u8          text[256];          //������255���ַ�
    u32         set_color;          //�Ƿ�������ɫ 0:������ 1:����
    u32         outline;            //�Ƿ񹴱� 0�������ߣ�1������
    pm_color_t  color;              //������ɫ
    u32		    rev[5];
}app_net_tcp_custom_osd_line_data_t;



//typedef enum {
//APP_NET_TCP_COM_VIDEO_MODE_320_240   = 1,
//APP_NET_TCP_COM_VIDEO_MODE_640_480,
//APP_NET_TCP_COM_VIDEO_MODE_800_600,
//APP_NET_TCP_COM_VIDEO_MODE_1024_768,
//APP_NET_TCP_COM_VIDEO_MODE_1280_960,
//APP_NET_TCP_COM_VIDEO_MODE_1280_1024,
//
//APP_NET_TCP_COM_VIDEO_MODE_320_176	= 10,
//APP_NET_TCP_COM_VIDEO_MODE_640_360,
//APP_NET_TCP_COM_VIDEO_MODE_800_450,
//APP_NET_TCP_COM_VIDEO_MODE_1280_720,
//APP_NET_TCP_COM_VIDEO_MODE_1366_768,
//APP_NET_TCP_COM_VIDEO_MODE_1920_1080,
//
//APP_NET_TCP_COM_VIDEO_MODE_720_576	= 20,
//APP_NET_TCP_COM_VIDEO_MODE_704_576,
//APP_NET_TCP_COM_VIDEO_MODE_704_288,
//APP_NET_TCP_COM_VIDEO_MODE_352_288,
//APP_NET_TCP_COM_VIDEO_MODE_MAX,
//} APP_NET_TCP_COM_VIDEO_MODE;//��Ƶ����ߴ硣
//
//typedef enum{
//APP_NET_TCP_COM_VIDEO_MODE_1920_1080 	= 15 ,
//APP_NET_TCP_COM_VIDEO_MODE_1280_720 	= 13,
//APP_NET_TCP_COM_VIDEO_MODE_1280_960	= 5,
//APP_NET_TCP_COM_VIDEO_MODE_1024_768	= 4,
//}APP_NET_TCP_COM_VIDEO_FST_MODE2; //�������ߴ�ȡֵ��Χ��
//
//typedef enum{
//APP_NET_TCP_COM_VIDEO_MODE_720_576	= 20,
//APP_NET_TCP_COM_VIDEO_MODE_704_576	= 21,
//APP_NET_TCP_COM_VIDEO_MODE_352_288	= 23,
//}APP_NET_TCP_COM_VIDEO_FST_MODE3; //�������ߴ�ȡֵ��Χ��
//
//typedef enum{
// APP_NET_TCP_COM_VIDEO_MODE_352_288	= 23,
//}APP_NET_TCP_COM_VIDEO_FST_MODE4; //���������ߴ�ȡֵ��Χ��

typedef enum {
    APP_NET_TCP_ENC_FMT_FR_NONE = 0,    //�ޱ仯
    APP_NET_TCP_ENC_FMT_FR_H = (1 << 0),//����
    APP_NET_TCP_ENC_FMT_FR_V = (1 << 1),//��ת
    APP_COM_ENC_FMT_FR_ALL,
}APP_COM_ENC_FMT_FR; //����תȡֵ���߼���

typedef struct {
    u32	        enc_mode;           //����ߴ磬ȡֵ�ο�APP_NET_TCP_COM_VIDEO_MODE
    u32	        enc_fps;            //����֡�ʣ�ȡֵ1~30��
    u32	        rev[6];
}app_net_tcp_enc_fmt_base_t;

typedef struct {
    u8		    idr_interval;       //I֡�����ȡֵ1~20.
    u8		    profile;            //H264����profile���ͣ�0��main profile 1��base profile.
    u8		    brc_mode;           //����ģʽ��0��CBR��1��VBR	��
    u8		    rev1;
    u32		    maxbps;		        //���ʣ�ȡֵ64000~8000000��CBR ���� maxbps
    u32		    minbps;		        //���ʣ�ȡֵ64000~8000000��
    u32		    rev2[6];
}app_net_tcp_enc_h264_base_t;

typedef struct {
    u32		    quality;            //ͼ��������ȡֵ��1~100.
    u32		    rev[7];
}app_net_tcp_enc_jpeg_base_t;

typedef struct{
    u32			chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32			stream;             //������ţ�0:��������1����������2������������
    u32			enc_type;           //�������ͣ�0��H264��1��JPEG.
    u32			rev[5];
    app_net_tcp_enc_fmt_base_t	fmt;    //�����ʽ���壬�ο��ṹ˵����
    app_net_tcp_enc_h264_base_t	h264;   //H264�������ã��ο��ṹ˵����
    app_net_tcp_enc_jpeg_base_t	jpeg;   //jpeg�������ã��ο��ṹ˵����
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
}APP_NET_TCP_PM_COLOR_TYPE;//PM��ɫ��


typedef struct{
    u32			chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32			enable;             //0:�رգ�1���������رս���������趨��PM.
    u32			x;                  //PM��X���꣬Ϊռ��Ƶ�ߴ�İٷֱȣ���Χ0~100.
    u32			y;                  //PM��Y���꣬Ϊռ��Ƶ�ߴ�İٷֱȣ���Χ0~100.
    u32			w;                  //PM�Ŀ�ȣ�Ϊռ��Ƶ�ߴ�İٷֱȣ���Χ0~100.
    u32			h;                  //PM�ĸ߶ȣ�Ϊռ��Ƶ�ߴ�İٷֱȣ���Χ0~100.
    pm_color_t	color;              //PM��ɫ,ȡֵ�ο�pm_color_t�ṹ��
    int			rev;                //������
}app_net_tcp_pm_cfg_t;



//ͼ��������
//ͼ������������������CmdType Ϊ0x0003
typedef struct{
    int		    chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    int		    brightness;         //���ȣ�ȡֵ��0~255.
    int		    saturation;         //���Ͷȣ�ȡֵ��0~255.
    int		    contrast;           //�Աȶȣ�ȡֵ��0~255.
    int		    sharpness;          //��ȣ�ȡֵ��0~255.
    int		    hue;                //ɫ����ȡֵ��0~255.
    int		    rev[2];
}app_net_tcp_img_color_t;


//�ع��ȡ����
typedef struct{
    U32			chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    U32			mode;               //�ع�ģʽ����Χ�ο�APP_NET_TCP_IMG_EXP_MODE
}app_net_tcp_com_chn_t_puguang;


typedef enum{
    APP_NET_TCP_IMG_EXP_AUTO = 0,   //�Զ��ع�
    APP_NET_TCP_IMG_EXP_MANUAL = 1, //�ֶ��ع�
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
    int		    chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    int		    mode;               //�ع�ģʽ����Χ�ο�APP_NET_TCP_IMG_EXP_MODE
    int		    shutter;            //ȡֵ�ο�APP_NET_TCP_ESHUTTER_E��
    int		    gain;               //������档��Χ��1~60
    int		    rev[16];

}app_net_tcp_img_exp_t;

/*
<6>ͼ���ƽ�������ȡ
DataType:
����0x0001
  CmdSubType��
  ����0x0006
    CmdPkt��
    ����typedef struct{
      ����	u32			chn;//��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
            U32			mode;//��ƽ��ģʽ����Χ�ο�APP_NET_TCP_IMG_WB_MODE
        }app_net_tcp_com_chn_t;
*/

typedef struct{
    int		    chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    int		    mode;               //��ƽ��ģʽ����Χ�ο�APP_NET_TCP_IMG_WB_MODE
    int		    rgain;              //R���棬ȡֵ��Χ��0~16000 
    int		    ggain;              //G���棬ȡֵ��Χ��0~16000
    int		    bgain;              //B���棬ȡֵ��Χ��0~16000
    int		    rev[3];
}app_net_tcp_img_wb_t;

//<8>�����ҹģʽ��ȡ
//DataType:
//����0x0001
//CmdSubType��  
//����0x0008
//CmdPkt��
//typedef struct{
//u32			chn; //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
//}app_net_tcp_com_chn_t;
//AckPkt��
typedef struct{
    int		chn;                    //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    int		mode;                   //�����ҹģʽ��0���رգ�1�����ã�2���Զ���
    int		rev[6];
}app_net_tcp_img_ircut_t;

//<9>��Ƶģʽ
typedef enum {
    APP_NET_MIRROR_HORRIZONTALLY_VERTICALLY = 0,
    APP_NET_MIRROR_HORRIZONTALLY,
    APP_NET_MIRROR_VERTICALLY,
    APP_NET_MIRROR_NONE,
    APP_NET_MIRROR_ALL,
}APP_NET_MIRROR_MODE;

typedef struct{
    int         chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    u32         mirror_mode;        //��תģʽ��APP_NET_MIRROR_MODE
    u32         rev[6];
}app_net_tcp_img_mode_t;



//////////////////////////////////////////////////////////////////////////

//
//��Ƶ��ص�����CmdType Ϊ0x0006
//<1>��Ƶ�����������ƣ�¼��
//DataType:
//����0x0001
//CmdSubType��  
//����0x0001
//CmdPkt��
typedef struct{
    U32			chn;                //��Ƶ����ͨ����ȡֵ0~MAX-1.
    U32			enable;             //0��ֹͣ���ͣ�1����ʼ���͡�
    U32			rev[6];
}app_net_tcp_audio_ctl_t;
//AckPkt��
typedef struct{
    U32			chn;	            //��Ƶ����ͨ����ȡֵ0~MAX-1.
    U32			state;              // 0:�ɹ���1��ʧ�ܡ�
}app_net_tcp_audio_ctl_ack_t;


typedef struct{
    int			chn;                //��Ƶ����ͨ����ȡֵ0~MAX-1.
    int			enc_type;           //��Ƶ�������ͣ�0��711_a��1��711_u��2��726��3��AAC
    int			samplerate;         //�����ʣ��ο�APP_NET_TCP_AUDIO_AUDIO_FS��
    int			rev[5];
}app_net_tcp_audio_config_t;

typedef struct{
    int			chn;		        // ͨ��
    int	        enc_type;	        // ѹ�����ͣ�0��711_a��1��711_u��2��726��3��AAC
    int			nSampleBits;	    // ����λ��, 8 or 16 
    int			nSampleRate;	    // ����Ƶ��  8000, 16000 ���ο�APP_NET_TCP_AUDIO_AUDIO_FS��
    int			nBitRate;		    // ѹ������
}app_net_tcp_audio_config_t_v2;


#define	APP_NET_TCP_SYS_NUM_IOIN_CHN_MAX    (1)
#define	APP_NET_TCP_SYS_NUM_VIN_CHN_MAX	    (1)
#define	APP_NET_TCP_SYS_NUM_NET_CARD_MAX	(2)
#define	APP_NET_TCP_SYS_NUM_DISK_MAX		(1)


//////////////////////////////////////////////////////////////////////////

//3>��������
//DataType:
//����0x0001
//CmdSubType��  
//����0x0003
//CmdPkt��
typedef struct {
    char		ipv4[30];           //IP���ַ�����
    char		mac_addr[40];       //MAC���ַ�����
    char		net_mask[30];       //MASK���ַ�����
    char		gate_way[30];       //GATE WAY���ַ�����
    char		dns[30];            //DNS���ַ�����
}app_net_tcp_sys_ipv4_t;

typedef struct tagGetNetWork_REQ{
    int						chn;    // ����ͨ���ţ�ȡֵ0~MAX-1.
    //int					card;   //0:����������1:����������ϵͳ��������ͨ����ѯ�豸��Դ��Ϣ��ȡ��
    int					    exit;   //0:���������ڣ�1���������ڣ�����ѯʱʹ�á�
    int						rev[6];
    app_net_tcp_sys_ipv4_t 	ipv4;
}app_net_tcp_sys_net_info_t;

//AckPkt��
/*typedef enum{
����	APP_NET_TCP_STATE_SUCCESS = 0,
    ����	APP_NET_TCP_STATE_FAIL,
        ����}APP_NET_TCP_STATE;
*/

typedef struct tagRes{
    int result;
} GET_NETWORK_RESPONE;

//<4>�����ѯ
//DataType:
//����0x0001
//CmdSubType��  
//����0x0004
//CmdPkt��
typedef struct{
    U32		    chn;                //0:����������1:����������
}app_net_tcp_com_chn_t_net;


typedef struct{
    int 	    ip_include;         // 0:include ip, 1:not include ip.
    int		    rev[3];
}app_net_tcp_sys_default_t;

typedef struct{
    char        sn[32];             //���к�
    char        mac[40];            //MAC��ַ���ַ������ַ���ȡֵ��ΧΪ��'0'-'9','A'-'F','a'-'f',':'��ÿ�����ַ���ʾһ��8λ����������ʮ�����Ƹ�ʽ���á�:���ָ������磺E2:02:6E:AD:17:19��
    char        rev[88];
}app_net_tcp_sn_mac_t;


//<13>�豸��Դ��ѯ//��0x000D
typedef struct{
    int	        vin_num;            //��Ƶ����ͨ����
    int	        vout_num;           //��Ƶ���ͨ����
    int	        ain_num;            //��Ƶ����ͨ����
    int	        aout_num;           //��Ƶ���ͨ����
    int         serial_num;         //����ͨ����
    int	        ioin_num;           //IO��������ͨ����
    int	        ioout_num;          //IO�������ͨ����
    int	        netcard_num;        //����ͨ����
    int	        disk_num;           //SD��ͨ����

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

//<15>�豸��Ϣ��ѯ
typedef struct {
    char		dev_type[64];       //�豸���ͣ��ַ�����
    char		dev_name[64];       //�豸�����ַ�����
    char		dev_id[64];	        //�豸ID���ַ�����
    char		hw_version[32];     //�豸Ӳ���汾���ַ�����
    char		sdk_sw_version[32]; //�豸���SDK�汾���ַ�����
    char		app_sw_version[32]; //�豸Ӧ������汾���ַ�����
    char		rev1[32];
}app_net_tcp_sys_dev_info_t;


//<16>�豸��Ϣ���� 0x0010
typedef struct tag{
    char        dev_name[64];       //�豸�����ַ���
    char        rev[32];
}app_net_tcp_sys_dev_info_t_st;


typedef struct{
    u32		    chn;                //����ͨ���ţ���0��ʼ����ϵͳ��󴮿�ͨ������һ��
    u32		    bitrate;            //�����ʣ���Χ�ο�APP_NET_TCP_SERIAL_BAUDRATE��
    u32		    parity;             //��żУ�飬��Χ�ο�APP_NET_TCP_SERIAL_BAUDRATE��
    u32		    len;                //���ݳ��ȣ���Χ�ο�APP_NET_TCP_SERIAL_DLENGTH
    u32		    stop;               //ֹͣλ����Χ�ο�APP_NET_TCP_SERIAL_STOPBIT
    u32		    rev[3];
}app_net_tcp_serial_cfg_t;

typedef struct{
    u32			type;           //0:���⣬1�����ڡ�
    u32			chn;            //if type is serial,chn is serial chn,else is video in chn.
    u32			protocl;        //1��pelco-d,2 ��pelco-p.
    u32			cmd1;           //refer to pelco-d.
    u32			cmd2;           //refer to pelco-d.
    u32			ps;	            //refer to pelco-d.
    u32			ts;	            //refer to pelco-d.
    u32			rev[9];
}app_net_tcp_ptz_t;

typedef struct{
    u32			chn;                //����ͨ���ţ�ȡֵ0~MAX-1.
    u32			len;                //���ݳ��ȣ�ȡֵ1~256
    u32			rev[6];
    char		data[256];          //�������ݡ�
}app_net_tcp_ptz_tran_t;


typedef struct {
    U32         part_num;	        //�����ţ�ȡֵ0,1,2,3��
    U64		    part_volume; 	    //����������unit (M)
    U64		    part_free;		    //����ʣ�࣬unit (M)
    U32		    part_state;		    //����״̬��0:���� 1:δ��ʽ�� 2:�쳣.
    U32		    rev[9];
}app_net_tcp_sys_disk_part_t;

typedef struct {
    u32         chn;                //����ͨ���ţ�ȡֵ0~����������-1��
    u32         exit;               //0:���̲����ڣ�1�����ڣ���ѯ�����жϡ�
    u32         rev[6];
    app_net_tcp_sys_disk_part_t	 part[4]; //���̷�����Ϣ�����֧��4���������������ϵͳ��ʹ�á�
}app_net_tcp_sys_disk_t;


typedef struct {
    u32		    chn;                //0:CVBS,1,HDMI.
    u32 		enable;             //0:�رգ�1��ʹ�ܡ�
}app_net_tcp_sys_net_misc_t_videoout;


typedef struct {
    u32         enable;             //0:�رգ�1����
    char        addr[128];          //NTP������IP������
    u32         port;               //NTP�������˿�
    u32         inteval;            //ͬ�����
    u32         set_time_zone;      //�Ƿ�����ʱ����0�������� 1����
    int         time_zone;
    char        rev[248];
}app_net_tcp_sys_ntp_para_t;

typedef struct {
    u32         alarm_id;
    u32         alarm_type;         //0-�豸�����ϱ���1-�豸���ϻָ��ϱ���2-�豸�¼��ϱ�
    u32         para_num;           //��������
    u32         alarm_para[4];      //�豸��ز���
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
    u32         index;              //0-IO�����˿�����-1
    u32         rev[7];
}app_net_tcp_io_alarm_index_t;

typedef struct{
    u32         index;              //ͬ��
    u32         active_level;       //0-��· 1-��·
    u32         set_in_out;         //�ܷ�ı䱨������ 0-���� 1-�ܣ����ֶ�Ϊ���ܸķ���ʱ������IO��������Ͳ������ڽ�����
    u32         in_out;             //0-�������� 1-�������
    u32         rev[12];
}app_net_tcp_io_alarm_attr_t;

typedef struct{
    u32         state;              //0-�����ã�1-����
    u32         rev[7];
}app_net_tcp_set_io_alarm_state_t;

typedef struct{
    u32         index;              //ͬ��
    u32         active_level;       //0-��· 1-��·
    u32         in_out;             //0-�������� 1-�������
    u32         rev[13];
}app_net_tcp_set_io_alarm_attr_t;


typedef struct{
    u32		    enable;             //1:������0���رա�//Ĭ�Ϲرա�
    u32		    interval;           //�����������λ100ms
    u32		    rev[2];
}app_net_tcp_sys_heart_t;



typedef struct{
    int		    type;               // ��Ϣ���͡�
    char        ioin[CHN_MAX];      // 0:�ޱ�����1��IO����
    char        md[CHN_MAX];        // 0:���ƶ���1���ƶ�����
    u32         dev_type;           // 485�����豸���� 0ΪICPDAS DL-100TM485
    char        _485_alarm[16];     // 0:�ޱ�����1������ ��DL-100���ԣ���һλΪ�����¶ȱ������ڶ�λΪ�����¶ȱ���������λΪʪ�ȱ���������λΪCO2����
    u32         _485_data[16];      // ��DL-100���ԣ���һλΪ�����¶ȣ��ڶ�λΪ�����¶ȣ�����λΪʪ�ȣ�����λΪCO2
    char        rev[428];
}app_net_tcp_sys_event_trig_state_t;


typedef struct{
    u32         dev_type;           //485�����豸���� 0ΪICPDAS DL-100TM485
    char        _485_alarm[16];     //0:�ޱ�����1������ ��DL-100���ԣ���һλΪ�����¶ȱ������ڶ�λΪ�����¶ȱ���������λΪʪ�ȱ���������λΪCO2����
    u32         _485_data[16];      //��DL-100���ԣ���һλΪ�����¶ȣ��ڶ�λΪ�����¶ȣ�����λΪʪ�ȣ�����λΪCO2
    char        rev[428];
}app_tempr_ctrl_alram_data;


typedef struct {
    int         type;               //�ϱ���Ϣ����
    char        data[256];          //�����Ϣ��־
    char        rev[512];
}alarm_req_data;//��ʽ�����ϲ㴦��


//////////////////////////////////////////////////////////////////////////
//5.�¼��������CmdType Ϊ0x0005

//IO�����������
typedef struct{
    u32		    chn;                //�������ͨ���š�ȡֵ0~MAX-1.
    u32		    state;              //���״̬��0����������1��������
    u32 		rev[6];
}app_net_tcp_event_act_ioout_t;


//MD����
typedef struct{
    u32		    x;                  //MD�����X���꣬ȡֵ0~32��
    u32		    y;                  //MD�����Y���꣬ȡֵ0~18��
    u32		    w;                  //MD����Ŀ�ȣ�ȡֵ0~32��
    u32		    h;                  //MD����ĸ߶ȣ�ȡֵ0~18��
}app_net_tcp_com_rect_t;

typedef struct{
    u32	        chn;                //��Ƶ����ͨ���ţ�ȡֵ0~	MAX-1.
    u32		    rows;               //������������Χ1~64.
    u32		    cols;               //������������Χ1~64.
    int		    rev[5];
}app_net_tcp_event_trig_md_base_rc_t;


typedef struct{
    u32		    chn;                //��Ƶ����ͨ���ţ�ȡֵ0~	MAX-1.
    u32		    threshold;          //���ޣ�ȡֵ0~100.
    u32		    sensitivity;        //�����ȣ�ȡֵ0~100.
    u32		    rev[5];
    char	 	en[64 * 64 / 8];    //0:�رգ�1�����á�0,1Ϊbit,ÿһ��BIT������Ӧ�Ƿ�		������
}app_net_tcp_event_trig_md_t;


typedef struct{
    u32		    chn;                //�������ͨ���š�ȡֵ0~MAX-1.
    u32         data_type;          //0���¶ȣ�1��ʪ�ȣ�2��CO2
}app_net_tcp_com_chn_485_t;

typedef struct{
    u32         data_type;          //0���¶ȣ�1��ʪ�ȣ�2��CO2
}app_net_tcp_485_osd_data_type_t;

typedef struct{
    u32		    chn;                //�������ͨ���š�ȡֵ0~MAX-1.
    u32         data_type;          //0���¶ȣ�1��ʪ�ȣ�2��CO2
    int		    state;              //���״̬��0����������1��������
    int 	    rev[5];
}app_net_tcp_event_act_485_t;


typedef struct{
    u32		    chn;                //��Ƶ����ͨ���ţ�ȡֵ0~	MAX-1.
    u32         data_type;          //0���¶ȣ�1��ʪ�ȣ�2��CO2
    int		    value[255];         //���¶ȣ�ʪ�ȣ�CO2���ԣ���1λ�����ޣ���2λ������
    int         status;             //0 ��Ч 1��Ч
}app_net_tcp_485_alarm_para_t;


typedef struct{
    int         enable;             //0:����,1:�ر�.
    int         dev_type;           //0-ICPDAS DL-100TM485
    int         dev_id;             //�豸ID
    int         interval;           //�ɼ�ʱ����
    char rev[512];
}app_net_tcp_485_dev_t;

typedef struct{
    u32         data_type;          //0���¶ȣ�1��ʪ�ȣ�2��CO2
    int         row;                //��ʾ��
    int         col;                //��ʾ��
    char        prefix[32];         //��ʾǰ׺
    char        suffix[32];         //��ʾ��׺
    int         dev_type;
    int         osd;
    char rev[504];
}app_net_tcp_485_osd_para_t;


typedef struct{
    u32         dev_type;           //485�����豸���� 0ΪICPDAS DL-100TM485
    u32         data[255];          //����DL-100���ԣ���һλΪ�¶ȣ��ڶ�λΪʪ��
    u32         status;
}app_net_tcp_485_input_data_t;


//////////////////////////////////////////////////////////////////////////


typedef struct{
    u32         proto;              //wifi�������͡�0:open,1:wep,2:wpa,3:tkip.
    char        ssid[36];           //WIFI��SSID��
    char        passwd[68];         //wifi����
}mw_misc_wifi_t;

typedef struct{
    int enable;                     //0:����,1:�ر�.
}mw_misc_ao_t;


typedef struct{
    u32         tempUuid;           //��ʱ�豸ID������Ѱ��Ӧ�豸��д
    u32         mac_addr_state;     //0:��Ч��1����Ч
    char		mac_addr[40];       //MAC���ַ���
    char		ipv4[30];           //IP���ַ�����
    char		net_mask[30];       //MASK���ַ�����
    char		gate_way[30];       //GATE WAY���ַ�����
    char        rev[110];
}multicast_dev_info_t;

typedef struct{
    u32         tempUuid;           //��ʱ�豸ID������Ѱ��Ӧ�豸��д
    u32         mac_addr_state;     //0:��Ч��1����Ч
    char		mac_addr[40];       //MAC���ַ���
    char		ipv4[30];           //IP���ַ�����
    char		net_mask[30];       //MASK���ַ�����
    char		gate_way[30];       //GATE WAY���ַ�����
    char        version[30];
    char        rev[80];
}multicast_dev_info_t_v2;


typedef struct {
    int 	    ip_include;         // 0:include ip, 1:not include ip.
    int		    file_len;		    //�ļ��ܳ��ȣ���λ���ֽڡ�
    int		    total_frame;	    //�ܰ�����
    int		    cur_frame;	        //��ǰ����ţ���1��ʼ��
    int         crc;
    int		    rev[4];
}app_net_tcp_sys_update_head_t;

typedef struct{
	u32	chn;                        //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
	u32	stream;                     //������ţ�0:��������1����������2������������
	u32 max_line_num;               //�Զ���OSD���������ʾ����,0��Ϊ����ʧ��
	u32 max_text_len;               //ÿ������ַ�����
	u32	rev[4];
}ack_pack_set_custom;


#endif