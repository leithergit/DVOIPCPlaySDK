/**
*  COPYRIGHT NOTICE 
*  Copyright (c) 2015, Diveo Technology Co.,Ltd (www.diveo.com.cn)
*  All rights reserved.
*
*  @file        dvoipcnetsdk_def.h
*  @author      luo_ws(luo_ws@diveo.com.cn)
*  @date        2015/08/11 13:54
*
*  @brief       dvoipcnetsdk���ݽṹ�Ķ���
*  @note        ��
*
*  @version
*    - v1.0.1.0    2015/08/11 13:54    luo_ws 
*/

#ifndef DVO_IPC_NET_SDK_DEF_H_
#define DVO_IPC_NET_SDK_DEF_H_


#ifndef DVO_INVALID_HANDLE
#define  DVO_INVALID_HANDLE (-1)
#endif

#ifndef PARAM_IN
#define PARAM_IN		///< �������
#endif

#ifndef PARAM_OUT
#define PARAM_OUT		///< �������
#endif

#ifndef PARAM_INOUT
#define PARAM_INOUT		///< ����/�������
#endif


#ifndef RET_SUCCESS
#define RET_SUCCESS      0
#endif

#ifndef RET_FAIL
#define RET_FAIL         -1
#endif

///////////////////////////DVO NET SDK ERROR CODE///////////////////////////////////////////////
/** �������ʹ��Ŷ��壬��Ӧ����DVOIPCNETSDK_API�ӿڻ�DVO_NET_GetLastError�ķ���ֵ */
#define _ERC(x)						            (0x00000000|x)
#define DVOAPI_ERRNO_NOERROR 				    0			///< û�д���
#define DVOAPI_ERRNO_UNKNOWN				    -1			///< δ֪�쳣
#define DVOAPI_ERRNO_INVALIDPARAM			    _ERC(1)     ///< �Ƿ�����
#define DVOAPI_ERRNO_SYSTEM_ERROR			    _ERC(2)	    ///< Windowsϵͳ����
#define DVOAPI_ERRNO_NOTINIT				    _ERC(3)	    ///< δ��ʼ��
#define DVOAPI_ERRNO_INVALIDHANDLE              _ERC(4)	    ///< �Ƿ��ľ��
#define DVOAPI_ERRNO_DEV_VER_NOMATCH		    _ERC(5)		///< �豸Э�鲻ƥ��
#define DVOAPI_ERRNO_NOTSUPPORT			        _ERC(6)	    ///< ��֧�ֵĲ���
#define DVOAPI_ERRNO_MALLOC_MEMORY			    _ERC(7)	    ///< �����ڴ�ʧ��
#define DVOAPI_ERRNO_SYSTEMBUSY			        _ERC(8)   	///< ϵͳ��æ
#define DVOAPI_ERRNO_DATA_TOOLONGH			    _ERC(9)		///< ���ݳ���

#define DVOAPI_ERRNO_CONNECTTIMEOUT             _ERC(10)    ///< ���ӳ�ʱ����¼�豸��ʱ���������粢����
#define DVOAPI_ERRNO_NET_SEND_TIMEOUT           _ERC(11)    ///< SOCKET ���ͳ�ʱ
#define DVOAPI_ERRNO_NET_RECV_TIMEOUT           _ERC(12)    ///< SOCKET ���ճ�ʱ
#define DVOAPI_ERRNO_NETWORK_ERROR              _ERC(13)    ///< ������󣬿�������Ϊ���糬ʱ
#define DVOAPI_ERRNO_SOCKET_SEND                _ERC(14)    ///< SOCKET ���ʹ���
#define DVOAPI_ERRNO_SOCKET_RECV                _ERC(15)    ///< SOCKET ���մ���
#define DVOAPI_ERRNO_TRANPORT_BIND		        _ERC(16)   	///< �˿��Ѿ��󶨣�����ʹ��
#define DVOAPI_ERRNO_RECV_INVALIDDATA	        _ERC(17)   	///< �յ���������
#define DVOAPI_ERRNO_NETWORK_DISCONN	        _ERC(18)   	///< �������
#define DVOAPI_ERRNO_NETWORK_DATA_TOOLONGH      _ERC(19)   	///< �������ݳ���

#define DVOAPI_ERRNO_DEVICE_SET_FAIL            _ERC(20)   	///< ����ʱ�豸����ʧ��

#define DVOAPI_ERRNO_USER_PASSWORD			    _ERC(30)   	///< �û������������
#define DVOAPI_ERRNO_LOGIN_TRYTIME              _ERC(31)    ///< ����������󳬹����ƴ���
#define DVOAPI_ERRNO_LOGIN_TIMEOUT              _ERC(32)	///< �ȴ���¼���س�ʱ
#define DVOAPI_ERRNO_LOGIN_RELOGGIN             _ERC(33)	///< �ʺ��ѵ�¼
#define DVOAPI_ERRNO_LOGIN_LOCKED               _ERC(34)	///< �ʺ��ѱ�����
#define DVOAPI_ERRNO_LOGIN_BLACKLIST            _ERC(35)	///< �ʺ��ѱ���Ϊ������
#define DVOAPI_ERRNO_LOGIN_MAXCONNECT           _ERC(36)    ///< �������������
#define DVOAPI_ERRNO_LOGIN_UNKNOWN              _ERC(37)    ///< ��¼�쳣
#define DVOAPI_ERRNO_LOGOUT                     _ERC(38)    ///< �Ѿ��ǳ�


#define DVOAPI_ERRNO_VIDEODISPLAY			    _ERC(40)   	///< ����Ƶ��ʾ����
#define DVOAPI_ERRNO_VIDEODECODE			    _ERC(41)   	///< ����Ƶ�������
#define DVOAPI_ERRNO_VIDEOENCODE			    _ERC(42)   	///< ����Ƶ�������
#define DVOAPI_ERRNO_VIDEOSHOW			        _ERC(43)   	///< ����Ƶ��ʾ����
#define DVOAPI_ERRNO_VIDEOCODEC			        _ERC(44)   	///< ����Ƶ������ʧ��
#define DVOAPI_ERRNO_AUDIOCAPTURE			    _ERC(45)   	///< ����Ƶ�ɼ�����
#define DVOAPI_ERRNO_AUDIOOUTPUT			    _ERC(46)   	///< ����Ƶ�������
#define DVOAPI_ERRNO_AUDIODECODE			    _ERC(47)   	///< ����Ƶ�������
#define DVOAPI_ERRNO_AUDIOENCODE			    _ERC(48)   	///< ����Ƶ�������

#define DVOAPI_ERRNO_TALK_REJECT                _ERC(50)	///< �ܾ��Խ�
#define DVOAPI_ERRNO_TALK_OPENED				_ERC(51)	///< �Խ��������ͻ��˴�
#define DVOAPI_ERRNO_TALK_RESOURCE_CONFLICIT    _ERC(52)	///< �Խ���Դ��ͻ
#define DVOAPI_ERRNO_TALK_UNSUPPORTED_ENCODE    _ERC(53)	///< ��֧�ֵ����������ʽ
#define DVOAPI_ERRNO_TALK_RIGHTLESS			    _ERC(54)	///< ��Ȩ�޶Խ�
#define DVOAPI_ERRNO_TALK_FAILED				_ERC(55)	///< ����Խ�ʧ��

#define DVOAPI_ERRNO_OPENFILE				    _ERC(61)   	///< ���ļ�����
#define DVOAPI_ERRNO_INVALIDFILETYPE		    _ERC(62)   	///< ��Ч���ļ�����
#define DVOAPI_ERRNO_INVALID_JSON			    _ERC(63)	///< ��Ч��JSON��ʽ
#define DVOAPI_ERRNO_JSON_REQUEST               _ERC(64)    ///< ���ɵ�jason�ַ�������
#define DVOAPI_ERRNO_JSON_RESPONSE              _ERC(65)    ///< ��Ӧ��jason�ַ�������
#define DVOAPI_ERRNO_INVALID_UPGRADE 	        _ERC(66)   	///< �Ƿ���������
#define DVOAPI_ERRNO_UPGRADE_LOCK    	        _ERC(67)   	///< ������

#define DVOAPI_ERRNO_DISCOVERY_RESTART          _ERC(70)	///< ����������
#define DVOAPI_ERRNO_DISCOVERY_NOTSTART         _ERC(71)	///< ����δ��ʼ��
#define DVOAPI_ERRNO_DISCOVERY_GET_ADAPTERS     _ERC(72)	///< ������ȡ��������

//////////////////////////////////////////////////////////////////////////

#if (defined(WIN32) || defined(WIN64))
#include <windows.h>


#ifndef INT64
#define INT64   __int64
#endif

#ifndef UINT64
#define UINT64   unsigned __int64
#endif


#ifndef LLONG
#ifdef WIN32
#define LLONG   LONG
#else //WIN64 
#define LLONG   INT64
#endif
#endif

#ifndef LDWORD
#ifdef WIN32
#define LDWORD  DWORD
#else //WIN64 
#define LDWORD  INT64
#endif
#endif

#else	//linux


#define WORD	unsigned short
#define DWORD	unsigned int
#define LONG	int
#define LPDWORD	DWORD*
#define BOOL	int
#define TRUE	1
#define FALSE	0
#define BYTE	unsigned char
#define UINT	unsigned int
#define HDC		void*
#define HWND	void*
#define LPVOID	void*
#define NULL	0
#define LLONG	long
#define LDWORD	long 
#define INT64   long long
#define UINT64  unsigned long long

#ifndef MAX_PATH
#define MAX_PATH    260
#endif

typedef struct tagRECT
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
}RECT;

#endif


typedef unsigned char  	    u8;	/**< UNSIGNED 8-bit data type */
typedef unsigned short 	    u16;/**< UNSIGNED 16-bit data type */
typedef unsigned int   	   	u32;/**< UNSIGNED 32-bit data type */
typedef UINT64           	u64;/**< UNSIGNED 64-bit data type */


typedef LLONG   USER_HANDLE;
typedef LLONG   REAL_HANDLE;
typedef LLONG   REC_HANDLE;
typedef LLONG   DOWNLOAD_HANDLE;



#ifndef MAKEINT32
#define MAKEINT32(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#endif

#ifndef LOWORD
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#endif
#ifndef HIWORD
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#endif

#ifndef MAKEINT64
#define MAKEINT64(a, b)      ((INT64)(((DWORD)(((DWORD_PTR)(a)) & 0xffffffff)) | ((INT64)((DWORD)(((DWORD_PTR)(b)) & 0xffffffff))) << 32))
#endif

#ifndef LODWORD
#define LODWORD(l)           ((LONG)(((DWORD_PTR)(l)) & 0xffffffff))
#endif
#ifndef HIDWORD
#define HIDWORD(l)           ((LONG)((((UINT64)(l)) >> 32) & 0xffffffff))
#endif




/// ��Ϣ�ص�����Ϣ����
enum DVO_CALLBACK_MESSAGE_TYPE
{
    DVO_MSG_SESSION_DISCONNECT              =  0x01,    // �û��������ӶϿ�
    DVO_MSG_SESSION_RECONNECT_OK            =  0x02,    // �û�����������������
    DVO_MSG_REALPLAY_DISCONNECT             =  0x03,    // ��Ƶ���ӶϿ�
    DVO_MSG_REALPLAY_RECONNECT_OK           =  0x04,    // ��Ƶ������������

    DVO_MSG_ALARM_HEARTBEAT                 =  0x08,    // Ioin[0]������ MD[0]������ <=>Ioin[0]ͨ· MD[0]������
    DVO_MSG_FUNCTION_REPORT                 =  0x09,    // ��������ϱ�����
    DVO_MSG_ALARM_TEMPR_CTRL_MOTION         =  0x0A,    // �¿ر�������
    DVO_MSG_ALARM_TEMPR_CTRL_ERROR          =  0x0B,    // �¿��쳣�ϱ�
};


#define DVO_FRAME_VIDEO_STREM_LOST      16


/// �������ͣ���ӦDVO_NET_GetDevConfig��DVO_NET_SetDevConfig�ӿ�
enum EDVO_DEVICE_PARAMETER_TYPE
{
    /// ϵͳ������
    DVO_DEV_CMD_SYS_NETCFG_SET              = MAKEINT32(0x001,0x003),   // ������������,app_net_tcp_sys_net_info_t
    DVO_DEV_CMD_SYS_NETCFG_GET              = MAKEINT32(0x001,0x004),   // �������ò�ѯ,app_net_tcp_com_chn_t;app_net_tcp_sys_net_info_t

    DVO_DEV_CMD_SYS_COMMCFG_SET             = MAKEINT32(0x001,0x00B),   // ��������,app_net_tcp_serial_cfg_t;
    DVO_DEV_CMD_SYS_COMMCFG_GET             = MAKEINT32(0x001,0x00C),
    DVO_DEV_CMD_SYS_DEVCAP_GET              = MAKEINT32(0x001,0x00D),   // �豸��Դ��ѯ,app_net_tcp_dev_cap_t;
    DVO_DEV_CMD_SYS_DEVLOG_GET              = MAKEINT32(0x001,0x00E),   // ��־��ѯ,����;
    DVO_DEV_CMD_SYS_DEVATTR_GET             = MAKEINT32(0x001,0x00F),   // �豸��Ϣ��ѯ,app_net_tcp_sys_dev_info_t;
    DVO_DEV_CMD_SYS_DEVATTR_SET             = MAKEINT32(0x001,0x010),   // �豸��������,app_net_tcp_sys_dev_name_t;
    DVO_DEV_CMD_SYS_DISK_GET                = MAKEINT32(0x001,0x011),   // ������Ϣ��ѯ,app_net_tcp_sys_disk_t;

    DVO_DEV_CMD_SYS_SNMAC_SET               = MAKEINT32(0x001,0x015),   // ���к�MAC����,app_net_tcp_sn_mac_t;
    DVO_DEV_CMD_SYS_SNMAC_GET               = MAKEINT32(0x001,0x016),
    DVO_DEV_CMD_SYS_MISC_SET                = MAKEINT32(0x001,0x017),   // ��Ƶ����ӿ�����,app_net_tcp_sys_net_misc_t;
    DVO_DEV_CMD_SYS_MISC_GET                = MAKEINT32(0x001,0x018),

    DVO_DEV_CMD_SYS_NTPCFG_SET              = MAKEINT32(0x001,0x019),	// NTP����
    DVO_DEV_CMD_SYS_NTPCFG_GET              = MAKEINT32(0x001,0x01A),	// NTP����

    DVO_DEV_CMD_SYS_FEATURES_GET            = MAKEINT32(0x001,0x01B),   // �豸���ܻ�ȡ

    DVO_DEV_CMD_SYS_IONUMBER_GET            = MAKEINT32(0x001,0x01E),   // IO�����˿�������ȡ,app_net_tcp_io_alarm_num_t
    DVO_DEV_CMD_SYS_IOATTR_GET              = MAKEINT32(0x001,0x01F),   // IO�����˿ڻ�ȡ,app_net_tcp_set_io_alarm_attr_t;
    DVO_DEV_CMD_SYS_IOATTR_SET              = MAKEINT32(0x001,0x020),   // IO�����˿�����

    DVO_DEV_CMD_SYS_IOUSE_SET               = MAKEINT32(0x001,0x021),   // IO�����˿�ʹ������,app_net_tcp_set_io_alarm_state_t;
    DVO_DEV_CMD_SYS_IOUSE_GET               = MAKEINT32(0x001,0x022),   // IO�����˿�ʹ�ܲ�ѯ


    /// ��Ƶ��������                        
    DVO_DEV_CMD_STREAM_VIDEO_ENC_SET        = MAKEINT32(0x002,0x001),   // ��Ƶ�����������,app_net_tcp_enc_info_t;
    DVO_DEV_CMD_STREAM_VIDEO_ENC_GET        = MAKEINT32(0x002,0x002),   // ��Ƶ���������ѯ

    DVO_DEV_CMD_STREAM_OSD_BASE_SET         = MAKEINT32(0x002,0x005),   // ��ƵOSD����,app_net_tcp_osd_cfg_t;
    DVO_DEV_CMD_STREAM_OSD_BASE_GET         = MAKEINT32(0x002,0x006),   // ��ƵOSD��ѯ,app_net_tcp_enc_chn_t,app_net_tcp_osd_cfg_t
    DVO_DEV_CMD_STREAM_VIDEO_PM_SET         = MAKEINT32(0x002,0x007),   // PM��������,app_net_tcp_pm_cfg_t;

    DVO_DEV_CMD_STREAM_OSD_CUSTOM_ZONE_SET  = MAKEINT32(0x002,0x008),   // �Զ���OSD��������,app_net_tcp_custom_osd_zone_cfg_t;app_net_tcp_custom_osd_zone_cfg_ack_t
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_ZONE_GET  = MAKEINT32(0x002,0x009),   // �Զ���OSD�����ѯ,app_net_tcp_custom_osd_zone_chn_t;app_net_tcp_custom_osd_zone_para_t;
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_LINE_SET  = MAKEINT32(0x002,0x00A),   // �Զ���OSD�в�������,app_net_tcp_custom_osd_line_cfg_t;
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_LINE_GET  = MAKEINT32(0x002,0x00B),   // �Զ���OSD�в�����ѯ,app_net_tcp_custom_osd_line_chn_t;app_net_tcp_custom_osd_line_cfg_t;
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_DATA_SET  = MAKEINT32(0x002,0x00C),   // �Զ���OSD����������,app_net_tcp_custom_osd_line_data_t;
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_DATA_GET  = MAKEINT32(0x002,0x00D),   // �Զ���OSD�����ݲ�ѯ,app_net_tcp_custom_osd_line_chn_t;app_net_tcp_custom_osd_line_data_t;

    /// ͼ��������
    DVO_DEV_CMD_IMAGE_COLOR_SET             = MAKEINT32(0x003,0x001),   // ͼ����ɫ��������,app_net_tcp_img_color_t;
    DVO_DEV_CMD_IMAGE_COLOR_GET             = MAKEINT32(0x003,0x002),   // ͼ����ɫ������ѯ,app_net_tcp_com_chn_t;app_net_tcp_img_color_t
    DVO_DEV_CMD_IMAGE_EXPOSURE_SET          = MAKEINT32(0x003,0x003),   // ͼ���ع��������,app_net_tcp_img_exp_t;
    DVO_DEV_CMD_IMAGE_EXPOSURE_GET          = MAKEINT32(0x003,0x004),   // ͼ���ع������ѯ,app_net_tcp_com_chn_t;app_net_tcp_img_exp_t
    DVO_DEV_CMD_IMAGE_WHITE_BALANCE_SET     = MAKEINT32(0x003,0x005),   // ͼ���ƽ���������,app_net_tcp_img_wb_t
    DVO_DEV_CMD_IMAGE_WHITE_BALANCE_GET     = MAKEINT32(0x003,0x006),   // ͼ���ƽ�������ȡ,app_net_tcp_com_chn_t;app_net_tcp_img_wb_t
    DVO_DEV_CMD_IMAGE_DAY_MODE_SET          = MAKEINT32(0x003,0x007),   // ͼ������ҹģʽ����,app_net_tcp_img_ircut_t
    DVO_DEV_CMD_IMAGE_DAY_MODE_GET          = MAKEINT32(0x003,0x008),   // ͼ������ҹģʽ��ȡ,app_net_tcp_com_chn_t;app_net_tcp_img_ircut_t
    DVO_DEV_CMD_IMAGE_MIIROR_MODE_SET       = MAKEINT32(0x003,0x009),   // ͼ����Ƶģʽ����,app_net_tcp_img_mode_t
    DVO_DEV_CMD_IMAGE_MIIROR_MODE_GET       = MAKEINT32(0x003,0x00A),   // ͼ����Ƶģʽ��ȡ,app_net_tcp_com_chn_t;app_net_tcp_img_mode_t

    /// PTZ
    DVO_DEV_CMD_PTZ_PARAM_SET               = MAKEINT32(0x004,0x001),   // PTZ����
    DVO_DEV_CMD__PTZ_TRANS_DATA             = MAKEINT32(0x004,0x002),   // PTZ͸��

    /// �����������
    DVO_DEV_CMD_ALARM_IOOUT_ENABLE_SET      = MAKEINT32(0x005,0x0001),  // IO�������ʹ������,app_net_tcp_event_act_ioout_t
    DVO_DEV_CMD_ALARM_IOOUT_ENABLE_GET      = MAKEINT32(0x005,0x0002),  // IO�������ʹ�ܲ�ѯ,app_net_tcp_com_chn_t;app_net_tcp_event_act_ioout_t

    DVO_DEV_CMD_ALARM_MD_RECT_SET           = MAKEINT32(0x005,0x0003),  // MD��������,app_net_tcp_event_trig_md_base_rc_t
    DVO_DEV_CMD_ALARM_MD_RECT_GET           = MAKEINT32(0x005,0x0004),  // MD�����ѯ,app_net_tcp_com_chn_t;app_net_tcp_event_trig_md_base_rc_t
    DVO_DEV_CMD_ALARM_MD_PARAM_SET          = MAKEINT32(0x005,0x0005),  // MD��������,app_net_tcp_event_trig_md_t
    DVO_DEV_CMD_ALARM_MD_PARAM_GET          = MAKEINT32(0x005,0x0006),  // MD������ѯ,app_net_tcp_com_chn_t;app_net_tcp_event_trig_md_t

    DVO_DEV_CMD_ALARM_485_ENABLE_SET        = MAKEINT32(0x005,0x0007),  // 485���뱨�������ʹ������,app_net_tcp_event_act_485_t
    DVO_DEV_CMD_ALARM_485_ENABLE_GET        = MAKEINT32(0x005,0x0008),  // 485���뱨�������ʹ�ܲ�ѯ,app_net_tcp_com_chn_485_t;app_net_tcp_event_act_485_t
    DVO_DEV_CMD_ALARM_485_PARAM_SET			= MAKEINT32(0x005,0x0009),  // 485���뱨���Ĳ�������,app_net_tcp_485_alarm_para_t
    DVO_DEV_CMD_ALARM_485_PARAM_GET			= MAKEINT32(0x005,0x000A),  // 485���뱨���Ĳ�����ѯ,app_net_tcp_com_chn_485_t;app_net_tcp_485_alarm_para_t

    /// ��Ƶ����
    DVO_DEV_CMD_STREAM_AUDIO_ENC_IN_SET     = MAKEINT32(0x006,0x005),   // ������Ƶ�ı����������,app_net_tcp_audio_config_t;
    DVO_DEV_CMD_STREAM_AUDIO_ENC_IN_GET     = MAKEINT32(0x006,0x006),   // ������Ƶ�ı��������ѯ,app_net_tcp_com_chn_t;app_net_tcp_audio_config_t
    DVO_DEV_CMD_STREAM_AUDIO_ENC_OUT_SET    = MAKEINT32(0x006,0x007),   // �����Ƶ�ı����������,app_net_tcp_audio_config_t;
    DVO_DEV_CMD_STREAM_AUDIO_ENC_OUT_GET    = MAKEINT32(0x006,0x008),   // �����Ƶ�ı��������ѯ,app_net_tcp_com_chn_t;app_net_tcp_audio_config_t

    /// ��������
    DVO_DEV_CMD_MSC_SUB_CHIPSET_MATCH_GET   = MAKEINT32(0x007,0x0001),  // ����оƬƥ���ѯ
    DVO_DEV_CMD_MSC_SUB_WIFI_PARAM_SET		= MAKEINT32(0x007,0x0002),  // WIFI��������
    DVO_DEV_CMD_MSC_SUB_WIFI_STATE_GET		= MAKEINT32(0x007,0x0003),  // WIFI����״̬��ѯ
    DVO_DEV_CMD_MSC_SUB_AUDIO_OUT_SET		= MAKEINT32(0x007,0x0004),  // ������Ƶ�������

    DVO_DEV_CMD_MSC_SUB_485_DEV_SET		    = MAKEINT32(0x007,0x0005),  // 485�豸�Ĳ�������
    DVO_DEV_CMD_MSC_SUB_485_DEV_GET		    = MAKEINT32(0x007,0x0006),  // 485�豸�Ĳ�����ѯ
    DVO_DEV_CMD_MSC_SUB_485_OSD_SET		    = MAKEINT32(0x007,0x0007),  // 485�����OSD��������
    DVO_DEV_CMD_MSC_SUB_485_OSD_GET		    = MAKEINT32(0x007,0x0008),  // 485�����OSD������ѯ
    DVO_DEV_CMD_MSC_SUB_485_DATA_GET		= MAKEINT32(0x007,0x0009),  // ��ȡ485���������


};


/// ��Ƶ��������
enum DVO_VIDEOCODEC_TYPE
{
    DVO_VCODEC_H264 = 0,
    DVO_VCODEC_JPEG = 1,
    DVO_VCODEC_H265 = 2,
    DVO_VCODEC_MPEG4,
};

/// ��Ƶ��������
enum DVO_AUDIOCODEC_TYPE
{
    DVO_ACODEC_AAC	= 0,		        ///< AAC
    DVO_ACODEC_G711U,			        ///< G711U
    DVO_ACODEC_G711A,			        ///< G711A
    DVO_ACODEC_G722,			        ///< G722
    DVO_ACODEC_G726,			        ///< G726
    DVO_ACODEC_PCM,		                ///< ����PCM
    DVO_ACODEC_MP3,				        ///< MP3
    DVO_ACODEC_NUMS,
};

/// ֡����
enum DVO_STREAM_FRAME_TYPE
{
    DVO_FRAMETYPE_DATA    = 0,          ///< ��������
    DVO_FRAMETYPE_HEAD	  = 1,          ///< ͷ��Ϣ
    DVO_FRAMETYPE_AUDIO   = 2,          ///< ��Ƶ����
    DVO_FRAMETYPE_VIDEO_I = 3,          ///< ��Ƶ����,i֡
    DVO_FRAMETYPE_VIDEO_P = 4,          ///< ��Ƶ����,p֡
    DVO_FRAMETYPE_VIDEO_B = 5,          ///< ��Ƶ����,b֡
};

typedef enum {
    APP_NET_TCP_COM_DST_IDR_FRAME = 1,  ///< IDR֡��
    APP_NET_TCP_COM_DST_I_FRAME,        ///< I֡��
    APP_NET_TCP_COM_DST_P_FRAME,        ///< P֡��
    APP_NET_TCP_COM_DST_B_FRAME,        ///< B֡��
    APP_NET_TCP_COM_DST_JPEG_FRAME,     ///< JPEG֡
    APP_NET_TCP_COM_DST_711_ALAW,       ///< 711 A�ɱ���֡
    APP_NET_TCP_COM_DST_711_ULAW,       ///< 711 U�ɱ���֡
    APP_NET_TCP_COM_DST_726,            ///< 726����֡
    APP_NET_TCP_COM_DST_AAC,            ///< AAC����֡��
    APP_NET_TCP_COM_DST_MAX,
} APP_NET_TCP_STREAM_TYPE;


typedef enum {
    APP_NET_TCP_COM_VIDEO_MODE_352_288 = 0,
    APP_NET_TCP_COM_VIDEO_MODE_704_576,
    APP_NET_TCP_COM_VIDEO_MODE_1280_720,
    APP_NET_TCP_COM_VIDEO_MODE_1920_1080,
    APP_NET_TCP_COM_VIDEO_MODE_1280_960,
    APP_NET_TCP_COM_VIDEO_MODE_1024_768,
    APP_NET_TCP_COM_VIDEO_MODE_176_144 = 0xFF,
    APP_NET_TCP_COM_VIDEO_MODE_MAX,
}APP_NET_TCP_COM_VIDEO_MODE; //��Ƶ����ߴ硣


typedef enum {
    APP_NET_TCP_AUDIO_FS_48000 = 48000,
    APP_NET_TCP_AUDIO_FS_44100 = 44100,
    APP_NET_TCP_AUDIO_FS_32000 = 32000,
    APP_NET_TCP_AUDIO_FS_24000 = 24000,
    APP_NET_TCP_AUDIO_FS_16000 = 16000,
    APP_NET_TCP_AUDIO_FS_11025 = 11025,
    APP_NET_TCP_AUDIO_FS_8000  = 8000,
}APP_NET_TCP_AUDIO_AUDIO_FS;//��Ƶ�����ʡ�


/// ʵʱ��Ƶ���ص��Ĵ�������
enum DVO_STREAM_ERROR_TYPE
{ 
    DVO_NET_STREAM_NO_ERROR=0,
    DVO_NET_STREAM_DISCONNECT_ERROR,
    DVO_NET_DATA_ERROR,
    DVO_NET_STREAM_INIT_ERROR,
    DVO_NET_DIRECTDRAW_INIT_ERROR,
};


/// ֡��Ϣ
struct DVO_AVFrame
{
    WORD	nChannelID;                 ///< ��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
    WORD	wStreamID;                  ///< ������ţ�0:��������1����������2������������
    WORD	wFrameType;                 ///< ֡���ͣ�DVO_STREAM_FRAME_TYPE��
    u32     uDvoMediaTag;               ///< dvo�����ݱ�ʶ"DVOM"��0x44564F4D������¼���ļ���Ҫ
    u32		uFrameNo;                   ///< ֡��ţ���Χ0~0xFFFFFFFF.
    u64 	u64TimeStamp;               ///< ʱ���,Ϊ1970��������΢��,(��32λΪ1970����������������32λΪ΢��)��
    int 	nSize;		                ///< ֡����
    BYTE*   pData;		                ///< ֡����,AV������
};

/// ��ͷ,�̶�Ϊ40���ֽ�
struct DVO_MEDIAINFO_HEAD
{
    u32     uDvoMediaTag;	            ///< "DVOM": 0x44564F4D DVO Media Information,
    u32     uDvoVersion;                
    u32     vdec_code;                  ///< ��Ƶ�������ͣ�DVO_VIDEOCODEC_TYPE
    u32     adec_code;                  ///< ��Ƶ�������ͣ�DVO_AUDIOCODEC_TYPE
    u32     uVideoWidth;	            ///< ��Ƶ���
    u32     uVideoHeight;	            ///< ��Ƶ�߶�
    u32     nFrameRate;		            ///< ��Ƶ֡��
    u32     uAudioSampleRate;           ///< ��Ƶ������,APP_NET_TCP_AUDIO_AUDIO_FS
    u32     reserved[2];                ///< ����
};


// fnDVOCallback_TransFile_T�ӿڴ����ļ�����
enum DVO_CALLBACK_TRANSFILE_STATE_TYPE
{
    DVO_DEV_UPGRADEFILETRANS_START    = 0x0000,    // ��ʼ�����ļ��ϴ�
    DVO_DEV_UPGRADEFILETRANS_SENDING  = 0x0001,    // ���ڷ��������ļ�,��ʱ�ص������ѷ��͵��ļ���С���Դ˿ɼ����ļ����ͽ���  
    DVO_DEV_UPGRADEFILETRANS_FINISH   = 0x0002,    // ��ɷ��������ļ�
    DVO_DEV_UPGRADEFILETRANS_STOP     = 0x0003,    // ֹͣ���������ļ�
    DVO_DEV_UPGRADEFILETRANS_ERROR    = 0x0004,    // ���������ļ�����
};





#endif