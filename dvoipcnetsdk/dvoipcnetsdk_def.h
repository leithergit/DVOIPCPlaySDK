/**
*  COPYRIGHT NOTICE 
*  Copyright (c) 2015, Diveo Technology Co.,Ltd (www.diveo.com.cn)
*  All rights reserved.
*
*  @file        dvoipcnetsdk_def.h
*  @author      luo_ws(luo_ws@diveo.com.cn)
*  @date        2015/08/11 13:54
*
*  @brief       dvoipcnetsdk数据结构的定义
*  @note        无
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
#define PARAM_IN		///< 输入参数
#endif

#ifndef PARAM_OUT
#define PARAM_OUT		///< 输出参数
#endif

#ifndef PARAM_INOUT
#define PARAM_INOUT		///< 输入/输出参数
#endif


#ifndef RET_SUCCESS
#define RET_SUCCESS      0
#endif

#ifndef RET_FAIL
#define RET_FAIL         -1
#endif

///////////////////////////DVO NET SDK ERROR CODE///////////////////////////////////////////////
/** 错误类型代号定义，对应所有DVOIPCNETSDK_API接口或DVO_NET_GetLastError的返回值 */
#define _ERC(x)						            (0x00000000|x)
#define DVOAPI_ERRNO_NOERROR 				    0			///< 没有错误
#define DVOAPI_ERRNO_UNKNOWN				    -1			///< 未知异常
#define DVOAPI_ERRNO_INVALIDPARAM			    _ERC(1)     ///< 非法参数
#define DVOAPI_ERRNO_SYSTEM_ERROR			    _ERC(2)	    ///< Windows系统出错
#define DVOAPI_ERRNO_NOTINIT				    _ERC(3)	    ///< 未初始化
#define DVOAPI_ERRNO_INVALIDHANDLE              _ERC(4)	    ///< 非法的句柄
#define DVOAPI_ERRNO_DEV_VER_NOMATCH		    _ERC(5)		///< 设备协议不匹配
#define DVOAPI_ERRNO_NOTSUPPORT			        _ERC(6)	    ///< 不支持的操作
#define DVOAPI_ERRNO_MALLOC_MEMORY			    _ERC(7)	    ///< 分配内存失败
#define DVOAPI_ERRNO_SYSTEMBUSY			        _ERC(8)   	///< 系统正忙
#define DVOAPI_ERRNO_DATA_TOOLONGH			    _ERC(9)		///< 数据超长

#define DVOAPI_ERRNO_CONNECTTIMEOUT             _ERC(10)    ///< 连接超时，登录设备超时，请检查网络并重试
#define DVOAPI_ERRNO_NET_SEND_TIMEOUT           _ERC(11)    ///< SOCKET 发送超时
#define DVOAPI_ERRNO_NET_RECV_TIMEOUT           _ERC(12)    ///< SOCKET 接收超时
#define DVOAPI_ERRNO_NETWORK_ERROR              _ERC(13)    ///< 网络错误，可能是因为网络超时
#define DVOAPI_ERRNO_SOCKET_SEND                _ERC(14)    ///< SOCKET 发送错误
#define DVOAPI_ERRNO_SOCKET_RECV                _ERC(15)    ///< SOCKET 接收错误
#define DVOAPI_ERRNO_TRANPORT_BIND		        _ERC(16)   	///< 端口已经绑定，不能使用
#define DVOAPI_ERRNO_RECV_INVALIDDATA	        _ERC(17)   	///< 收到错误数据
#define DVOAPI_ERRNO_NETWORK_DISCONN	        _ERC(18)   	///< 网络断线
#define DVOAPI_ERRNO_NETWORK_DATA_TOOLONGH      _ERC(19)   	///< 网络数据超长

#define DVOAPI_ERRNO_DEVICE_SET_FAIL            _ERC(20)   	///< 设置时设备返回失败
#define DVOAPI_ERRNO_BUFFUF_NOENOUGH            _ERC(21)   	///< 缓冲区太小
#define DVOAPI_ERRNO_NOLOGIN                    _ERC(22)    ///< 没登录
#define DVOAPI_ERRNO_DATA_OPERATOR              _ERC(23)    ///< 数据操作异常

#define DVOAPI_ERRNO_USER_PASSWORD			    _ERC(30)   	///< 用户名或密码错误
#define DVOAPI_ERRNO_LOGIN_TRYTIME              _ERC(31)    ///< 输入密码错误超过限制次数
#define DVOAPI_ERRNO_LOGIN_TIMEOUT              _ERC(32)	///< 等待登录返回超时
#define DVOAPI_ERRNO_LOGIN_RELOGGIN             _ERC(33)	///< 帐号已登录
#define DVOAPI_ERRNO_LOGIN_LOCKED               _ERC(34)	///< 帐号已被锁定
#define DVOAPI_ERRNO_LOGIN_BLACKLIST            _ERC(35)	///< 帐号已被列为黑名单
#define DVOAPI_ERRNO_LOGIN_MAXCONNECT           _ERC(36)    ///< 会话超过最大连接数
#define DVOAPI_ERRNO_LOGIN_UNKNOWN              _ERC(37)    ///< 登录异常
#define DVOAPI_ERRNO_LOGOUT                     _ERC(38)    ///< 已经登出
#define DVOAPI_ERRNO_STREAM_MAXCONNECT          _ERC(39)    ///< 码流超过最大连接数

#define DVOAPI_ERRNO_VIDEODISPLAY			    _ERC(40)   	///< 打开视频显示错误
#define DVOAPI_ERRNO_VIDEODECODE			    _ERC(41)   	///< 打开视频解码错误
#define DVOAPI_ERRNO_VIDEOENCODE			    _ERC(42)   	///< 打开视频编码错误
#define DVOAPI_ERRNO_VIDEOSHOW			        _ERC(43)   	///< 打开视频显示错误
#define DVOAPI_ERRNO_VIDEOCODEC			        _ERC(44)   	///< 打开视频解码器失败
#define DVOAPI_ERRNO_AUDIOCAPTURE			    _ERC(45)   	///< 打开音频采集错误
#define DVOAPI_ERRNO_AUDIOOUTPUT			    _ERC(46)   	///< 打开音频输出错误
#define DVOAPI_ERRNO_AUDIODECODE			    _ERC(47)   	///< 打开音频解码错误
#define DVOAPI_ERRNO_AUDIOENCODE			    _ERC(48)   	///< 打开音频编码错误

#define DVOAPI_ERRNO_TALK_REJECT                _ERC(50)	///< 拒绝对讲
#define DVOAPI_ERRNO_TALK_OPENED				_ERC(51)	///< 对讲被其他客户端打开
#define DVOAPI_ERRNO_TALK_RESOURCE_CONFLICIT    _ERC(52)	///< 对讲资源冲突
#define DVOAPI_ERRNO_TALK_UNSUPPORTED_ENCODE    _ERC(53)	///< 不支持的语音编码格式
#define DVOAPI_ERRNO_TALK_RIGHTLESS			    _ERC(54)	///< 无权限对讲
#define DVOAPI_ERRNO_TALK_FAILED				_ERC(55)	///< 请求对讲失败

#define DVOAPI_ERRNO_OPENFILE				    _ERC(61)   	///< 打开文件错误
#define DVOAPI_ERRNO_INVALIDFILETYPE		    _ERC(62)   	///< 无效的文件类型
#define DVOAPI_ERRNO_INVALID_JSON			    _ERC(63)	///< 无效的JSON格式
#define DVOAPI_ERRNO_JSON_REQUEST               _ERC(64)    ///< 生成的jason字符串错误
#define DVOAPI_ERRNO_JSON_RESPONSE              _ERC(65)    ///< 响应的jason字符串错误
#define DVOAPI_ERRNO_INVALID_UPGRADE 	        _ERC(66)   	///< 非法的升级包
#define DVOAPI_ERRNO_UPGRADE_LOCK    	        _ERC(67)   	///< 升级中

#define DVOAPI_ERRNO_DISCOVERY_RESTART          _ERC(70)	///< 搜索已启动
#define DVOAPI_ERRNO_DISCOVERY_NOTSTART         _ERC(71)	///< 搜索未初始化
#define DVOAPI_ERRNO_DISCOVERY_GET_ADAPTERS     _ERC(72)	///< 搜索获取网卡错误

//////////////////////////////////////////////////////////////////////////

#if (defined(WIN32) || defined(WIN64))
#include <windows.h>


#ifndef INT64
#define INT64               __int64
#endif

#ifndef UINT64
#define UINT64              unsigned __int64
#endif


#ifndef LLONG
#ifdef WIN32
#define LLONG               LONG
#else //WIN64 
#define LLONG               INT64
#endif
#endif

#ifndef LDWORD
#ifdef WIN32
#define LDWORD              DWORD
#else //WIN64 
#define LDWORD              INT64
#endif
#endif

#else	//linux


typedef int                 INT;
typedef unsigned long       ULONG;
typedef unsigned long       DWORD;

#define WORD	            unsigned short
#define LONG	            int
#define LPDWORD	            DWORD*
#define BOOL	            int
#define TRUE	            1
#define FALSE	            0
#define BYTE	            unsigned char
#define UINT	            unsigned int
#define HDC		            void*
#define HWND	            void*
#define LPVOID	            void*
#ifndef NULL
#define NULL	0
#endif
#define LLONG	            long
#define LDWORD	            long 
#define	DWORD_PTR			DWORD
#define	INT_PTR				int

#define	UINT_PTR			UINT
#define	LONG_PTR			LONG
#define ULONG_PTR			ULONG

#define INT64               long long
#define UINT64              unsigned long long
typedef UINT64              ULONG64;


/* Types use for passing & returning polymorphic values */
typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;
typedef void*               HANDLE;
typedef void*               HMODULE;


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

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

#endif


typedef unsigned char  	    u8;	/**< UNSIGNED 8-bit data type */
typedef unsigned short 	    u16;/**< UNSIGNED 16-bit data type */
typedef unsigned int   	   	u32;/**< UNSIGNED 32-bit data type */
typedef UINT64           	u64;/**< UNSIGNED 64-bit data type */


typedef LLONG               USER_HANDLE;
typedef LLONG               REAL_HANDLE;
typedef LLONG               REC_HANDLE;
typedef LLONG               DOWNLOAD_HANDLE;



#ifndef MAKEINT32
#define MAKEINT32(a, b)     ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
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


/////add at 20151123
#ifndef MAKELONG
#define MAKELONG(a, b)       ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#endif
#ifndef MAKELPARAM
#define MAKELPARAM(l, h)     ((LPARAM)(DWORD)MAKELONG(l, h))
#endif



/// 消息回调的消息类型
enum DVO_CALLBACK_MESSAGE_TYPE
{
    DVO_MSG_SESSION_DISCONNECT              =  0x01,    // 用户交互连接断开
    DVO_MSG_SESSION_RECONNECT_OK            =  0x02,    // 用户交互连接重新连上
    DVO_MSG_REALPLAY_DISCONNECT             =  0x03,    // 视频连接断开
    DVO_MSG_REALPLAY_RECONNECT_OK           =  0x04,    // 视频连接重新连上

    DVO_MSG_ALARM_HEARTBEAT                 =  0x08,    // 心跳报警消息,buf对应数据结构体app_net_tcp_sys_event_trig_state_t
    DVO_MSG_FUNCTION_STATE_REPORT           =  0x09,    // 设备功能状态上报消息,buf对应数据结构体app_net_tcp_sys_func_state_report_t
};


#define DVO_FRAME_VIDEO_STREM_LOST      16


/// 配置类型，对应DVO_NET_GetDevConfig和DVO_NET_SetDevConfig接口
enum EDVO_DEVICE_PARAMETER_TYPE
{
    /// 系统级命令
    DVO_DEV_CMD_SYS_NETCFG_SET              = MAKEINT32(0x001,0x003),   // 网络配置设置,app_net_tcp_sys_net_info_t
    DVO_DEV_CMD_SYS_NETCFG_GET              = MAKEINT32(0x001,0x004),   // 网络配置查询,app_net_tcp_com_chn_t;app_net_tcp_sys_net_info_t

    DVO_DEV_CMD_SYS_COMMCFG_SET             = MAKEINT32(0x001,0x00B),   // 串口配置,app_net_tcp_serial_cfg_t;
    DVO_DEV_CMD_SYS_COMMCFG_GET             = MAKEINT32(0x001,0x00C),
    DVO_DEV_CMD_SYS_DEVCAP_GET              = MAKEINT32(0x001,0x00D),   // 设备资源查询,app_net_tcp_dev_cap_t;
    DVO_DEV_CMD_SYS_DEVLOG_GET              = MAKEINT32(0x001,0x00E),   // 日志查询,保留;
    DVO_DEV_CMD_SYS_DEVATTR_GET             = MAKEINT32(0x001,0x00F),   // 设备信息查询,app_net_tcp_sys_dev_info_t;
    DVO_DEV_CMD_SYS_DEVATTR_SET             = MAKEINT32(0x001,0x010),   // 设备名称设置,app_net_tcp_sys_dev_name_t;
    DVO_DEV_CMD_SYS_DISK_GET                = MAKEINT32(0x001,0x011),   // 磁盘信息查询,app_net_tcp_sys_disk_t;

    DVO_DEV_CMD_SYS_SNMAC_SET               = MAKEINT32(0x001,0x015),   // 序列号MAC配置,app_net_tcp_sn_mac_t;
    DVO_DEV_CMD_SYS_SNMAC_GET               = MAKEINT32(0x001,0x016),
    DVO_DEV_CMD_SYS_MISC_SET                = MAKEINT32(0x001,0x017),   // 视频输出接口配置,app_net_tcp_sys_net_misc_t;
    DVO_DEV_CMD_SYS_MISC_GET                = MAKEINT32(0x001,0x018),

    DVO_DEV_CMD_SYS_NTPCFG_SET              = MAKEINT32(0x001,0x019),	// NTP配置,app_net_tcp_sys_ntp_para_t
    DVO_DEV_CMD_SYS_NTPCFG_GET              = MAKEINT32(0x001,0x01A),	// NTP配置,app_net_tcp_sys_ntp_para_t

    DVO_DEV_CMD_SYS_FEATURES_GET            = MAKEINT32(0x001,0x01B),   // 设备功能获取,app_net_tcp_sys_func_list_t

    DVO_DEV_CMD_SYS_IONUMBER_GET            = MAKEINT32(0x001,0x01E),   // IO报警端口数量获取,app_net_tcp_io_alarm_num_t
    DVO_DEV_CMD_SYS_IOATTR_GET              = MAKEINT32(0x001,0x01F),   // IO报警端口获取,app_net_tcp_io_alarm_index_t,app_net_tcp_io_alarm_attr_t;
    DVO_DEV_CMD_SYS_IOATTR_SET              = MAKEINT32(0x001,0x020),   // IO报警端口设置,app_net_tcp_set_io_alarm_attr_t,

    DVO_DEV_CMD_SYS_IOUSE_SET               = MAKEINT32(0x001,0x021),   // IO报警端口使能设置,app_net_tcp_set_io_alarm_state_t;
    DVO_DEV_CMD_SYS_IOUSE_GET               = MAKEINT32(0x001,0x022),   // IO报警端口使能查询,app_net_tcp_set_io_alarm_state_t

    DVO_DEV_CMD_SYS_FUNCMODE_SET            = MAKEINT32(0x001,0x023),   // 设备功能型号设置,app_net_tcp_func_model_t;
    DVO_DEV_CMD_SYS_FUNCMODE_GET            = MAKEINT32(0x001,0x024),   // 设备功能型号查询,app_net_tcp_func_model_t


    /// 视频编码命令                        
    DVO_DEV_CMD_STREAM_VIDEO_ENC_SET        = MAKEINT32(0x002,0x001),   // 视频编码参数设置,app_net_tcp_enc_info_t;
    DVO_DEV_CMD_STREAM_VIDEO_ENC_GET        = MAKEINT32(0x002,0x002),   // 视频编码参数查询,app_net_tcp_com_schn_t,app_net_tcp_enc_info_t

    DVO_DEV_CMD_STREAM_OSD_BASE_SET         = MAKEINT32(0x002,0x005),   // 视频OSD设置,app_net_tcp_osd_cfg_t;
    DVO_DEV_CMD_STREAM_OSD_BASE_GET         = MAKEINT32(0x002,0x006),   // 视频OSD查询,app_net_tcp_enc_chn_t,app_net_tcp_osd_cfg_t
    DVO_DEV_CMD_STREAM_VIDEO_PM_SET         = MAKEINT32(0x002,0x007),   // PM参数设置,app_net_tcp_pm_cfg_t;

    DVO_DEV_CMD_STREAM_OSD_CUSTOM_ZONE_SET  = MAKEINT32(0x002,0x008),   // 自定义OSD区域设置,app_net_tcp_custom_osd_zone_cfg_t;app_net_tcp_custom_osd_zone_cfg_ack_t
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_ZONE_GET  = MAKEINT32(0x002,0x009),   // 自定义OSD区域查询,app_net_tcp_custom_osd_zone_chn_t;app_net_tcp_custom_osd_zone_para_t;
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_LINE_SET  = MAKEINT32(0x002,0x00A),   // 自定义OSD行参数设置,app_net_tcp_custom_osd_line_cfg_t;
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_LINE_GET  = MAKEINT32(0x002,0x00B),   // 自定义OSD行参数查询,app_net_tcp_custom_osd_line_chn_t;app_net_tcp_custom_osd_line_cfg_t;
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_DATA_SET  = MAKEINT32(0x002,0x00C),   // 自定义OSD行数据设置,app_net_tcp_custom_osd_line_data_t;
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_DATA_GET  = MAKEINT32(0x002,0x00D),   // 自定义OSD行数据查询,app_net_tcp_custom_osd_line_chn_t;app_net_tcp_custom_osd_line_data_t;
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_CLEAR_SET = MAKEINT32(0x002,0x00E),   // 自定义OSD清除,app_net_tcp_com_schn_t,
    DVO_DEV_CMD_STREAM_OSD_CUSTOM_ARRAY_SET = MAKEINT32(0x002,0x00E),   // 自定义OSD批量设置,app_net_tcp_custom_osd_line_data_array_t,

    /// 图像处理命令
    DVO_DEV_CMD_IMAGE_COLOR_SET             = MAKEINT32(0x003,0x001),   // 图像颜色参数设置,app_net_tcp_img_color_t;
    DVO_DEV_CMD_IMAGE_COLOR_GET             = MAKEINT32(0x003,0x002),   // 图像颜色参数查询,app_net_tcp_com_chn_t;app_net_tcp_img_color_t
    DVO_DEV_CMD_IMAGE_EXPOSURE_SET          = MAKEINT32(0x003,0x003),   // 图像曝光参数设置,app_net_tcp_img_exp_t;
    DVO_DEV_CMD_IMAGE_EXPOSURE_GET          = MAKEINT32(0x003,0x004),   // 图像曝光参数查询,app_net_tcp_com_chn_t;app_net_tcp_img_exp_t
    DVO_DEV_CMD_IMAGE_WHITE_BALANCE_SET     = MAKEINT32(0x003,0x005),   // 图像白平衡参数设置,app_net_tcp_img_wb_t
    DVO_DEV_CMD_IMAGE_WHITE_BALANCE_GET     = MAKEINT32(0x003,0x006),   // 图像白平衡参数获取,app_net_tcp_com_chn_t;app_net_tcp_img_wb_t
    DVO_DEV_CMD_IMAGE_DAY_MODE_SET          = MAKEINT32(0x003,0x007),   // 图像白天黑夜模式设置,app_net_tcp_img_ircut_t
    DVO_DEV_CMD_IMAGE_DAY_MODE_GET          = MAKEINT32(0x003,0x008),   // 图像白天黑夜模式获取,app_net_tcp_com_chn_t;app_net_tcp_img_ircut_t
    DVO_DEV_CMD_IMAGE_MIIROR_MODE_SET       = MAKEINT32(0x003,0x009),   // 图像视频模式设置,app_net_tcp_img_mode_t
    DVO_DEV_CMD_IMAGE_MIIROR_MODE_GET       = MAKEINT32(0x003,0x00A),   // 图像视频模式获取,app_net_tcp_com_chn_t;app_net_tcp_img_mode_t

    /// PTZ
    DVO_DEV_CMD_PTZ_PARAM_SET               = MAKEINT32(0x004,0x001),   // PTZ设置,app_net_tcp_ptz_t
    DVO_DEV_CMD_PTZ_TRANS_DATA              = MAKEINT32(0x004,0x002),   // PTZ透传,app_net_tcp_ptz_tran_t

    /// 报警相关命令
    DVO_DEV_CMD_ALARM_IOOUT_ENABLE_SET      = MAKEINT32(0x005,0x001),   // IO报警输出使能设置,app_net_tcp_event_act_ioout_t
    DVO_DEV_CMD_ALARM_IOOUT_ENABLE_GET      = MAKEINT32(0x005,0x002),   // IO报警输出使能查询,app_net_tcp_com_chn_t;app_net_tcp_event_act_ioout_t

    DVO_DEV_CMD_ALARM_MD_RECT_SET           = MAKEINT32(0x005,0x003),   // MD区域设置,app_net_tcp_event_trig_md_base_rc_t
    DVO_DEV_CMD_ALARM_MD_RECT_GET           = MAKEINT32(0x005,0x004),   // MD区域查询,app_net_tcp_com_chn_t;app_net_tcp_event_trig_md_base_rc_t
    DVO_DEV_CMD_ALARM_MD_PARAM_SET          = MAKEINT32(0x005,0x005),   // MD参数设置,app_net_tcp_event_trig_md_t
    DVO_DEV_CMD_ALARM_MD_PARAM_GET          = MAKEINT32(0x005,0x006),   // MD参数查询,app_net_tcp_com_chn_t;app_net_tcp_event_trig_md_t

    DVO_DEV_CMD_ALARM_485_ENABLE_SET        = MAKEINT32(0x005,0x007),   // 485输入报警的输出使能设置,app_net_tcp_event_act_485_t
    DVO_DEV_CMD_ALARM_485_ENABLE_GET        = MAKEINT32(0x005,0x008),   // 485输入报警的输出使能查询,app_net_tcp_com_chn_485_t;app_net_tcp_event_act_485_t
    DVO_DEV_CMD_ALARM_485_PARAM_SET			= MAKEINT32(0x005,0x009),   // 485输入报警的参数设置,app_net_tcp_485_alarm_para_t
    DVO_DEV_CMD_ALARM_485_PARAM_GET			= MAKEINT32(0x005,0x00A),   // 485输入报警的参数查询,app_net_tcp_com_chn_485_t;app_net_tcp_485_alarm_para_t

    /// 音频命令
    DVO_DEV_CMD_STREAM_AUDIO_ENC_IN_SET     = MAKEINT32(0x006,0x005),   // 输入音频的编码参数设置,app_net_tcp_audio_config_t;
    DVO_DEV_CMD_STREAM_AUDIO_ENC_IN_GET     = MAKEINT32(0x006,0x006),   // 输入音频的编码参数查询,app_net_tcp_com_chn_t;app_net_tcp_audio_config_t
    DVO_DEV_CMD_STREAM_AUDIO_ENC_OUT_SET    = MAKEINT32(0x006,0x007),   // 输出音频的编码参数设置,app_net_tcp_audio_config_t;
    DVO_DEV_CMD_STREAM_AUDIO_ENC_OUT_GET    = MAKEINT32(0x006,0x008),   // 输出音频的编码参数查询,app_net_tcp_com_chn_t;app_net_tcp_audio_config_t

    /// 其它命令
    DVO_DEV_CMD_MSC_SUB_CHIPSET_MATCH_GET   = MAKEINT32(0x007,0x001),   // 加密芯片匹配查询,in参数为NULL
    DVO_DEV_CMD_MSC_SUB_WIFI_PARAM_SET		= MAKEINT32(0x007,0x002),   // WIFI参数设置,mw_misc_wifi_t
    DVO_DEV_CMD_MSC_SUB_WIFI_STATE_GET		= MAKEINT32(0x007,0x003),   // WIFI链接状态查询,,in参数为NULL
    DVO_DEV_CMD_MSC_SUB_AUDIO_OUT_SET		= MAKEINT32(0x007,0x004),   // 启动音频输出设置,mw_misc_ao_t

    DVO_DEV_CMD_MSC_SUB_485_DEV_SET		    = MAKEINT32(0x007,0x005),   // 485设备的参数设置,app_net_tcp_485_dev_t
    DVO_DEV_CMD_MSC_SUB_485_DEV_GET		    = MAKEINT32(0x007,0x006),   // 485设备的参数查询,NULL,app_net_tcp_485_dev_t
    DVO_DEV_CMD_MSC_SUB_485_OSD_SET		    = MAKEINT32(0x007,0x007),   // 485输入的OSD参数设置,app_net_tcp_485_osd_para_t
    DVO_DEV_CMD_MSC_SUB_485_OSD_GET		    = MAKEINT32(0x007,0x008),   // 485输入的OSD参数查询,app_net_tcp_485_osd_data_type_t,app_net_tcp_485_osd_para_t
    DVO_DEV_CMD_MSC_SUB_485_DATA_GET		= MAKEINT32(0x007,0x009),   // 获取485输入的数据,NULL,app_net_tcp_485_input_data_t


};


/// @brief 视频编码类型, 0：H264，1：JPEG，2：H265
enum DVO_VIDEOCODEC_TYPE
{
    DVO_VCODEC_H264 		= 0,	    ///< H264 default
    DVO_VCODEC_JPEG 		= 1,
    DVO_VCODEC_H265 		= 2,
    DVO_VCODEC_MPEG4 		= 3,

    DVO_VCODEC_NUMS,                    ///< 视频编码类型数量
};

/// @brief 音频编码类型, 0：711_u，1：711_a，2：726，3：AAC
enum DVO_AUDIOCODEC_TYPE
{
    DVO_ACODEC_G711U		= 0,		///< G711U default
    DVO_ACODEC_G711A 		= 1,		///< G711A
    DVO_ACODEC_G726			= 2,		///< G726
    DVO_ACODEC_AAC			= 3,		///< AAC

    DVO_ACODEC_NUMS,                    ///< 音频编码类型数量
};

#define DVO_CODEC_UNKNOWN   (-1)		///未知的编码类型

/// @brief 帧类型
enum DVO_STREAM_FRAME_TYPE
{
    DVO_FRAMETYPE_UNKNOWN   = 0,         ///< 未知帧类型(旧版本录像中,0默认为视频P帧)
    DVO_FRAMETYPE_VIDEO_I   = 1,         ///< I帧
    DVO_FRAMETYPE_AUDIO     = 2,         ///< 音频帧
    DVO_FRAMETYPE_VIDEO_P   = 3,         ///< P帧
    DVO_FRAMETYPE_VIDEO_B   = 4,         ///< B帧
    DVO_FRAMETYPE_VIDEO_IDR = 5,         ///< IDR帧

    DVO_FRAMETYPE_DATA      = 10,        ///< 其它数据
    DVO_FRAMETYPE_HEAD	    = 11,        ///< 头信息
};

/// 帧原始类型
typedef enum {
    APP_NET_TCP_COM_DST_IDR_FRAME = 1,  ///< IDR帧。
    APP_NET_TCP_COM_DST_I_FRAME,        ///< I帧。
    APP_NET_TCP_COM_DST_P_FRAME,        ///< P帧。
    APP_NET_TCP_COM_DST_B_FRAME,        ///< B帧。
    APP_NET_TCP_COM_DST_JPEG_FRAME,     ///< JPEG帧
    APP_NET_TCP_COM_DST_711_ALAW,       ///< 711 A律编码帧
    APP_NET_TCP_COM_DST_711_ULAW,       ///< 711 U律编码帧
    APP_NET_TCP_COM_DST_726,            ///< 726编码帧
    APP_NET_TCP_COM_DST_AAC,            ///< AAC编码帧。
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
}APP_NET_TCP_COM_VIDEO_MODE; //视频编码尺寸。


typedef enum {
    APP_NET_TCP_AUDIO_FS_48000 = 48000,
    APP_NET_TCP_AUDIO_FS_44100 = 44100,
    APP_NET_TCP_AUDIO_FS_32000 = 32000,
    APP_NET_TCP_AUDIO_FS_24000 = 24000,
    APP_NET_TCP_AUDIO_FS_16000 = 16000,
    APP_NET_TCP_AUDIO_FS_11025 = 11025,
    APP_NET_TCP_AUDIO_FS_8000  = 8000,
}APP_NET_TCP_AUDIO_AUDIO_FS;//音频采样率。


/// 实时视频流回调的错误类型
enum DVO_STREAM_ERROR_TYPE
{ 
    DVO_NET_STREAM_NO_ERROR=0,
    DVO_NET_STREAM_DISCONNECT_ERROR,
    DVO_NET_DATA_ERROR,
    DVO_NET_STREAM_INIT_ERROR,
    DVO_NET_DIRECTDRAW_INIT_ERROR,
};


// fnDVOCallback_TransFile_T接口传输文件类型
enum DVO_CALLBACK_TRANSFILE_STATE_TYPE
{
    DVO_DEV_UPGRADEFILETRANS_START    = 0x0000,    // 开始升级文件上传
    DVO_DEV_UPGRADEFILETRANS_SENDING  = 0x0001,    // 正在发送升级文件,此时回调返回已发送的文件大小，以此可计算文件发送进度  
    DVO_DEV_UPGRADEFILETRANS_FINISH   = 0x0002,    // 完成发送升级文件
    DVO_DEV_UPGRADEFILETRANS_STOP     = 0x0003,    // 停止发送升级文件
    DVO_DEV_UPGRADEFILETRANS_ERROR    = 0x0004,    // 发送升级文件出错
    //DVO_DEV_UPGRADEFILETRANS_SUCCESS  = 0x0005,    // 升级成功
    //DVO_DEV_UPGRADEFILETRANS_FAIL     = 0x0006,    // 升级失败
};





#endif