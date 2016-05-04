
#ifndef _DVR_NET_SDK_H_
#define _DVR_NET_SDK_H_

#include "dvrdvstypedef.h"
#include "dvrdvsconfig.h"
#include "dvrdvsdefine.h"

#ifdef WIN32

#define CALL_METHOD __stdcall

#ifdef __cplusplus
#define __EXTERN_C extern "C"
#else
#define __EXTERN_C extern
#endif

#ifdef DVR_NET_SDK_EXPORTS
#define NET_SDK_API __EXTERN_C __declspec(dllexport)
#else
#define NET_SDK_API __EXTERN_C __declspec(dllimport)
#endif

#else

#define CALL_METHOD
#define NET_SDK_API

#endif

//LastError定义
typedef enum
{
	NET_SDK_SUCCESS,
	NET_SDK_PASSWORD_ERROR,
	NET_SDK_NOENOUGH_AUTH,
	NET_SDK_NOINIT,
	NET_SDK_CHANNEL_ERROR,
	NET_SDK_OVER_MAXLINK,
	NET_SDK_LOGIN_REFUSED,
	NET_SDK_VERSION_NOMATCH,
	NET_SDK_NETWORK_FAIL_CONNECT,
	NET_SDK_NETWORK_NOT_CONNECT,
	NET_SDK_NETWORK_SEND_ERROR,
	NET_SDK_NETWORK_RECV_ERROR,
	NET_SDK_NETWORK_RECV_TIMEOUT,
	NET_SDK_NETWORK_ERRORDATA,
	NET_SDK_ORDER_ERROR,
	NET_SDK_OPER_BY_OTHER,
	NET_SDK_OPER_NOPERMIT,
	NET_SDK_COMMAND_TIMEOUT,
	NET_SDK_ERROR_SERIALPORT,
	NET_SDK_ERROR_ALARMPORT,
	NET_SDK_PARAMETER_ERROR,
	NET_SDK_CHAN_EXCEPTION,
	NET_SDK_NODISK,
	NET_SDK_ERROR_DISKNUM,
	NET_SDK_DISK_FULL,
	NET_SDK_DISK_ERROR,
	NET_SDK_NOSUPPORT,
	NET_SDK_BUSY,
	NET_SDK_MODIFY_FAIL,
	NET_SDK_PASSWORD_FORMAT_ERROR,
	NET_SDK_DISK_FORMATING,
	NET_SDK_DVR_NORESOURCE,
	NET_SDK_DVR_OPRATE_FAILED,
	NET_SDK_OPEN_HOSTSOUND_FAIL,
	NET_SDK_DVR_VOICEOPENED,
	NET_SDK_TIME_INPUTERROR,
	NET_SDK_NOSPECFILE,
	NET_SDK_CREATEFILE_ERROR,
	NET_SDK_FILEOPENFAIL,
	NET_SDK_OPERNOTFINISH,
	NET_SDK_GETPLAYTIMEFAIL,
	NET_SDK_PLAYFAIL,
	NET_SDK_FILEFORMAT_ERROR,
	NET_SDK_DIR_ERROR,
	NET_SDK_ALLOC_RESOURCE_ERROR,
	NET_SDK_AUDIO_MODE_ERROR,
	NET_SDK_NOENOUGH_BUF,
	NET_SDK_CREATESOCKET_ERROR,
	NET_SDK_SETSOCKET_ERROR,
	NET_SDK_MAX_NUM,
	NET_SDK_USERNOTEXIST,
	NET_SDK_WRITEFLASHERROR,
	NET_SDK_UPGRADEFAIL,
	NET_SDK_CARDHAVEINIT,
	NET_SDK_PLAYERFAILED,
	NET_SDK_MAX_USERNUM,
	NET_SDK_GETLOCALIPANDMACFAIL,
	NET_SDK_NOENCODEING,
	NET_SDK_IPMISMATCH,
	NET_SDK_MACMISMATCH,
	NET_SDK_UPGRADELANGMISMATCH,
	NET_SDK_MAX_PLAYERPORT,
	NET_SDK_NOSPACEBACKUP,
	NET_SDK_NODEVICEBACKUP,
	NET_SDK_PICTURE_BITS_ERROR,
	NET_SDK_PICTURE_DIMENSION_ERROR,
	NET_SDK_PICTURE_SIZ_ERROR,
	NET_SDK_LOADPLAYERSDKFAILED,
	NET_SDK_LOADPLAYERSDKPROC_ERROR,
	NET_SDK_LOADDSSDKFAILED,
	NET_SDK_LOADDSSDKPROC_ERROR,
	NET_SDK_DSSDK_ERROR,
	NET_SDK_VOICEMONOPOLIZE,
	NET_SDK_JOINMULTICASTFAILED,
	NET_SDK_CREATEDIR_ERROR,
	NET_SDK_BINDSOCKET_ERROR,
	NET_SDK_SOCKETCLOSE_ERROR,
	NET_SDK_USERID_ISUSING,
	NET_SDK_PROGRAM_EXCEPTION,
	NET_SDK_WRITEFILE_FAILED,
	NET_SDK_FORMAT_READONLY,
	NET_SDK_WITHSAMEUSERNAME,
	NET_SDK_DEVICETYPE_ERROR,
	NET_SDK_LANGUAGE_ERROR,
	NET_SDK_PARAVERSION_ERROR,
	NET_SDK_FILE_SUCCESS,
	NET_SDK_FILE_NOFIND,
	NET_SDK_NOMOREFILE,
	NET_SDK_FILE_EXCEPTION,
	NET_SDK_TRY_LATER,
}NET_SDK_ERROR;

enum NET_DEVICE_STATUS
{
	NET_DEVICE_STATUS_CONNECTED,
	NET_DEVICE_STATUS_DISCONNECT,
};

enum NET_SDK_STREAM_TYPE
{
	NET_SDK_MAIN_STREAM,
	NET_SDK_SUB_STREAM,
};

enum NET_SDK_PLAYCTRL_TYPE
{
	NET_SDK_PLAYCTRL_PAUSE,
	NET_SDK_PLAYCTRL_FF,
	NET_SDK_PLAYCTRL_REW,
	NET_SDK_PLAYCTRL_RESUME,
	NET_SDK_PLAYCTRL_STOP,
	NET_SDK_PLAYCTRL_FRAME,
	NET_SDK_PLAYCTRL_NORMAL,
	NET_SDK_PLAYCTRL_STARTAUDIO,	//该控制类型及以下两个控制类型只有在SDK内部解码显示时才会起作用
	NET_SDK_PLAYCTRL_STOPAUDIO,
	NET_SDK_PLAYCTRL_AUDIOVOLUME,
	NET_SDK_PLAYCTRL_SETPOS,
};

enum NET_SDK_RPB_SPEED
{
	NET_SDK_RPB_SPEED_1_32X = 1,
	NET_SDK_RPB_SPEED_1_16X,
	NET_SDK_RPB_SPEED_1_8X,		//1/8
	NET_SDK_RPB_SPEED_1_4X,		//1/4
	NET_SDK_RPB_SPEED_1_2X,		//1/2
	NET_SDK_RPB_SPEED_1X,		//1
	NET_SDK_RPB_SPEED_2X,
	NET_SDK_RPB_SPEED_4X,
	NET_SDK_RPB_SPEED_8X,
	NET_SDK_RPB_SPEED_16X,
	NET_SDK_RPB_SPEED_32X,
};

const long MAX_NAMELEN			= 36;

//登录结构定义
typedef struct _net_sdk_deviceinfo
{
	unsigned char	localVideoInputNum;		//本地视频输入通道数目
	unsigned char	audioInputNum;			//音频输入通道数目
	unsigned char	sensorInputNum;			//传感器输入通道数目
	unsigned char	sensorOutputNum;		//继电器输出数目
	unsigned long   displayResolutionMask;  //监视器可选择的分辨率

	unsigned char	videoOuputNum;			//视频输出数目（及支持回放最大通道数）
	unsigned char	netVideoOutputNum;		//网络回放最大通道数目
	unsigned char	netVideoInputNum;		//数字信号接入通道数目
	unsigned char	IVSNum;					//智能分析通道数目

	unsigned char	presetNumOneCH;			//每个通道预置点数目
	unsigned char	cruiseNumOneCH;			//每个通道巡航线数目
	unsigned char	presetNumOneCruise;		//每个巡航线的预置点数目
	unsigned char	trackNumOneCH;			//每个通道轨迹数目

	unsigned char	userNum;				//用户数目 
	unsigned char	netClientNum;			//最多客户端数目
	unsigned char	netFirstStreamNum;		//主码流传输的通道最大数目，即同时可以有几个客户端查看主码流
	unsigned char	deviceType;				//设备类型

	unsigned char	doblueStream;			//是否有提供双码流
	unsigned char	audioStream;			//是否有提供音频流
	unsigned char	talkAudio;				//是否有对讲功能: 1表示有对讲功能;0表示没有
	unsigned char	bPasswordCheck;			//操作是否要输入密码

	unsigned char	defBrightness;			//缺省亮度
	unsigned char	defContrast;			//缺省对比度
	unsigned char	defSaturation;			//缺省饱和度
	unsigned char	defHue;					//缺省色调

	unsigned short	videoInputNum;			//视频输入通道数目（本地加网络）
	unsigned short  deviceID;				//设备ID号
	unsigned long   videoFormat;            //系统当前制式

	//假如是FUNC_REMOTE_UPGRADE对应的功能，那么第FUNC_REMOTE_UPGRADE个比特为1，否则为零。
	unsigned long	function[8];			//功能描述

	unsigned long	deviceIP;				//设备网络地址
	unsigned char	deviceMAC[6];			//设备物理地址
	unsigned short	devicePort;				//设备端口

	unsigned long	buildDate;				//创建日期:year<<16 + month<<8 + mday
	unsigned long	buildTime;				//创建时间:hour<<16 + min<<8 + sec

	char			deviceName[36];			//设备名称

	char	firmwareVersion[36];			//固件版本
	char	kernelVersion[64];				//内核版本
	char	hardwareVersion[36];			//硬件版本
	char	MCUVersion[36];					//单片机版本
}NET_SDK_DEVICEINFO, *LPNET_SDK_DEVICEINFO;

//数据帧头定义
typedef struct _net_sdk_frame_info
{
	unsigned long		deviceID;
	unsigned long       channel;
	unsigned long		frameType;	//参考DD_FRAME_TYPE
	unsigned long		length;
	unsigned long		keyFrame;	//0：非关键帧 1：关键帧
	unsigned long       width;
	unsigned long       height;
	unsigned long		frameIndex;
	unsigned long		frameAttrib;//参考DD_FRAME_ATTRIB
	unsigned long		streamID;
	LONGLONG			time;	   //从1970年1月1日0点0分0秒开始的绝对时间，单位微秒
	LONGLONG			relativeTime;//相对时间，单位微秒
}NET_SDK_FRAME_INFO;

//录像时间信息定义
typedef struct _net_sdk_rec_time
{
	DWORD			dwChannel;	//通道号
	DD_TIME			startTime;	//该段录像开始时间
	DD_TIME			stopTime;	//该段录像结束时间
}NET_SDK_REC_TIME;

typedef struct _net_sdk_rec_file
{
	DWORD			dwChannel;
	DWORD			bFileLocked;
	DD_TIME			startTime;
	DD_TIME			stopTime;
	DWORD			dwRecType;
	DWORD			dwPartition;
	DWORD			dwFileIndex;
}NET_SDK_REC_FILE;

typedef struct _net_sdk_rec_event
{
	DWORD			dwChannel;
	DD_TIME			startTime;
	DD_TIME			stopTime;
	DWORD			dwRecType;	//DD_RECORD_TYPE
}NET_SDK_REC_EVENT;

//日志信息定义
typedef struct _net_sdk_log
{
	DD_TIME			strLogTime;
	DWORD			dwMajorType;
	DWORD			dwMinorType;
	char			sNetUser[MAX_NAMELEN];
	DWORD			dwRemoteHostAddr;

}NET_SDK_LOG,*LPNET_SDK_LOG;

typedef struct _net_sdk_event
{
	unsigned short chnn;			//事件对应的通道
	unsigned short type;			//事件类型：DD_EVENT_TYPE
	DD_TIME        startTime;		//事件产生的开始时间
	DD_TIME        endTime;			//事件的结束时间
}NET_SDK_EVENT,*LPNET_SDK_EVENT;

typedef struct _net_sdk_clientinfo
{
	LONG    lChannel;
	LONG    streamType;
	HWND    hPlayWnd;
}NET_SDK_CLIENTINFO, *LPNET_SDK_CLIENTINFO;

//设备上传信息类型
enum NET_SDK_DEVICE_MSG_TYPE
{
	NET_SDK_ALARM,		//设备报警消息
	NET_SDK_RECORD,		//设备录像信息
	NET_SDK_IVM_RULE,	//智能行为分析信息(暂时保留)
	NET_SDK_TRADEINFO,	//ATM交易信息(暂时保留)
	NET_SDK_IPCCFG,		//混合型DVR的IPC信息变更(暂时保留)
};

//设备上传信息为报警类型时的具体报警类型
enum NET_SDK_ALARM_TYPE
{
	NET_SDK_ALARM_TYPE_MOTION,		//移动侦测
	NET_SDK_ALARM_TYPE_SENSOR,		//信号量报警
	NET_SDK_ALARM_TYPE_VLOSS,		//信号丢失
	NET_SDK_ALARM_TYPE_SHELTER,		//遮挡报警
	NET_SDK_ALARM_TYPE_DISK_FULL,	//硬盘满
	NET_SDK_ALARM_TYPE_DISK_UNFORMATTED,//硬盘未格式化
	NET_SDK_ALARM_TYPE_DISK_WRITE_FAIL,	//读写硬盘出错
	NET_SDK_ALARM_TYPE_EXCEPTION,
};

//设备上传信息为报警类型时的上传信息结构
typedef struct _net_sdk_alarminfo
{
	DWORD		dwAlarmType;
	DWORD		dwSensorIn;		//传感器报警输入端口号
	DWORD		dwChannel;		//在报警与通道相关时，表示报警的通道号
	DWORD		dwDisk;			//在磁盘报警时，表示产生报警的磁盘号
}NET_SDK_ALARMINFO;

typedef struct _net_sdk_record_status
{
	DWORD		dwRecordType;		//DD_RECORD_TYPE
	DWORD		dwChannel;
}NET_SDK_RECORD_STATUS;

typedef enum _net_sdk_disk_status
{	
	NET_SDK_DISK_NORMAL,				//正常 
	NET_SDK_DISK_NO_FORMAT,				//未格式化
}NET_SDK_DISK_STATUS;

typedef enum _net_sdkdisk_property
{
	NET_SDK_DISK_READ_WRITE = 0,			//可正常读写
	NET_SDK_DISK_ONLY_READ,					//只能读，不能写
	NET_SDK_DISK_REDUNDANCY					//冗余
}NET_SDK_DISK_PROPERTY;

typedef struct _net_sdk_disk_info
{
	LONG		diskIndex;			//磁盘编号
	short		diskStatus;			//磁盘状态，参考NET_SDK_DISK_STATUS
	short		diskProperty;		//磁盘状态，参考NET_SDK_DISK_PROPERTY
	DWORD		diskTotalSpace;		//磁盘总空间，单位MB
	DWORD		diskFreeSpace;		//磁盘剩余空间，单位MB
}NET_SDK_DISK_INFO;

typedef struct
{
	WORD     wPicSize;
	WORD     wPicQuality;
}NET_SDK_JPEGPARA,*LPNET_SDK_JPEGPARA;

typedef enum _net_sdk_exception_type
{
	NETWORK_DISCONNECT = 0,
	NETWORK_RECONNECT,
}NET_SDK_EXCEPTION_TYPE;

enum NET_SDK_DEVICE_TYPE
{
	NET_SDK_DVR,
	NET_SDK_DVS,
	NET_SDK_IPCAMERA,
	NET_SDK_SUPERDVR,
	NET_SDK_DECODER,
};

typedef struct _net_sdk_device_discovery_info
{
	unsigned long		deviceType;		//NET_SDK_DEVICE_TYPE
	char				productType[16];
	char				strIP[16];
	char				strNetMask[16];
	char				strGateWay[16];
	unsigned char		byMac[8];
	unsigned short		netPort;
	unsigned short		httpPort;
	unsigned long		softVer;
	unsigned long		softBuildDate;
}NET_SDK_DEVICE_DISCOVERY_INFO;

//回调函数定义
typedef void (CALLBACK *EXCEPTION_CALLBACK)(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser);

typedef void (CALLBACK *DRAW_FUN_CALLBACK)(LONG lLiveHandle, HDC hDC, void *pUser);

typedef void (CALLBACK *LIVE_DATA_CALLBACK)(LONG lLiveHandle, NET_SDK_FRAME_INFO frameInfo, BYTE *pBuffer, void *pUser);

typedef BOOL (CALLBACK *NET_MESSAGE_CALLBACK)(LONG lCommand, LONG lUserID, char *pBuf, DWORD dwBufLen, void *pUser);

typedef void (CALLBACK *PLAY_DATA_CALLBACK)(LONG lPlayHandle, NET_SDK_FRAME_INFO frameInfo, BYTE *pBuffer, void *pUser);

typedef void (CALLBACK *TALK_DATA_CALLBACK)(LONG lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, void* pUser);

//////////////////////////////////////////////////////////////////////////
//2012-3-15增加SDK接收DVR主动注册的功能
typedef void (CALLBACK *ACCEPT_REGISTER_CALLBACK)(LONG lUserID, LONG lRegisterID, LPNET_SDK_DEVICEINFO pDeviceInfo, void *pUser);

//////////////////////////////////////////////////////////////////////////
//接口定义
/********************************SDK接口函数声明*********************************/
//SDK初始化及退出
NET_SDK_API BOOL CALL_METHOD NET_SDK_Init();
NET_SDK_API BOOL CALL_METHOD NET_SDK_Cleanup();

//局域网前端设备发现
NET_SDK_API long CALL_METHOD NET_SDK_DiscoverDevice(NET_SDK_DEVICE_DISCOVERY_INFO *pDeviceInfo, long bufNum, long waitSeconds);

//通过备名称获取IP地址
NET_SDK_API BOOL CALL_METHOD NET_SDK_GetDeviceIPByName(char *sSerIP,DWORD wSerPort,char *sDvrName,char *sDvrIP);

//SDK操作异常回调
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetSDKMessageCallBack(UINT nMessage, HWND hWnd, EXCEPTION_CALLBACK fExceptionCallBack, void *pUser);

//DVR报警或状态回调
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetDVRMessageCallBack(NET_MESSAGE_CALLBACK fMessageCallBack, void *pUser);

//设置重连参数
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetConnectTime(DWORD dwWaitTime = 5000, DWORD dwTryTimes = 3);
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetReconnect(DWORD dwInterval = 5000, BOOL bEnableRecon = TRUE);

//版本信息
NET_SDK_API DWORD CALL_METHOD NET_SDK_GetSDKVersion();
NET_SDK_API DWORD CALL_METHOD NET_SDK_GetSDKBuildVersion();

NET_SDK_API LONG CALL_METHOD NET_SDK_GetProductSubID(char *sDVRIP,WORD wDVRPort);

//设备登录登出
NET_SDK_API LONG CALL_METHOD NET_SDK_Login(char *sDVRIP,WORD wDVRPort,char *sUserName,char *sPassword,LPNET_SDK_DEVICEINFO lpDeviceInfo);
NET_SDK_API BOOL CALL_METHOD NET_SDK_Logout(LONG lUserID);

//错误查询
NET_SDK_API DWORD CALL_METHOD NET_SDK_GetLastError();
NET_SDK_API char* CALL_METHOD NET_SDK_GetErrorMsg(LONG *pErrorNo = NULL);

//预览相关接口
NET_SDK_API LONG CALL_METHOD NET_SDK_LivePlay(LONG lUserID, LPNET_SDK_CLIENTINFO lpClientInfo, LIVE_DATA_CALLBACK fLiveDataCallBack = NULL, void* pUser = NULL);
NET_SDK_API BOOL CALL_METHOD NET_SDK_StopLivePlay(LONG lLiveHandle);
NET_SDK_API BOOL CALL_METHOD NET_SDK_RegisterDrawFun(LONG lLiveHandle, DRAW_FUN_CALLBACK fDrawFun, void *pUser);
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetPlayerBufNumber(LONG lLiveHandle,DWORD dwBufNum);

NET_SDK_API BOOL CALL_METHOD NET_SDK_OpenSound(LONG lLiveHandle);
NET_SDK_API BOOL CALL_METHOD NET_SDK_CloseSound();
NET_SDK_API BOOL CALL_METHOD NET_SDK_Volume(LONG lLiveHandle,WORD wVolume);

NET_SDK_API BOOL CALL_METHOD NET_SDK_SaveLiveData(LONG lLiveHandle,char *sFileName);
NET_SDK_API BOOL CALL_METHOD NET_SDK_StopSaveLiveData(LONG lLiveHandle);
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetLiveDataCallBack(LONG lLiveHandle,LIVE_DATA_CALLBACK fLiveDataCallBack, void *pUser);
NET_SDK_API BOOL CALL_METHOD NET_SDK_CapturePicture(LONG lLiveHandle,char *sPicFileName);//bmp
NET_SDK_API BOOL CALL_METHOD NET_SDK_CapturePicture_Other(LONG lUserID, LONG lChannel,char *sPicFileName);//bmp

//动态生成I帧
NET_SDK_API BOOL CALL_METHOD NET_SDK_MakeKeyFrame(LONG lUserID, LONG lChannel);//主码流
NET_SDK_API BOOL CALL_METHOD NET_SDK_MakeKeyFrameSub(LONG lUserID, LONG lChannel);//子码流

//云台控制相关接口
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZControl(LONG lLiveHandle, DWORD dwPTZCommand, DWORD dwSpeed);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZControl_Other(LONG lUserID, LONG lChannel, DWORD dwPTZCommand, DWORD dwSpeed);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZPreset(LONG lLiveHandle, DWORD dwPTZPresetCmd, DWORD dwPresetIndex);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZPreset_Other(LONG lUserID, LONG lChannel, DWORD dwPTZPresetCmd, DWORD dwPresetIndex);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZCruise(LONG lLiveHandle,DWORD dwPTZCruiseCmd,BYTE byCruiseRoute);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZCruise_Other(LONG lUserID,LONG lChannel,DWORD dwPTZCruiseCmd,BYTE byCruiseRoute);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZTrack(LONG lLiveHandle, DWORD dwPTZTrackCmd);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZTrack_Other(LONG lUserID, LONG lChannel, DWORD dwPTZTrackCmd);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZAutoScan(LONG lLiveHandle, DWORD dwPTZAutoScanCmd);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZAutoScan_Other(LONG lUserID, LONG lChannel, DWORD dwPTZAutoScanCmd);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZSetCruise(LONG lLiveHandle, BYTE byCruiseRoute, DD_CRUISE_POINT_INFO *pCruisePoint, WORD pointNum);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PTZSetCruise_Other(LONG lUserID, LONG lChannel, BYTE byCruiseRoute, DD_CRUISE_POINT_INFO *pCruisePoint, WORD pointNum);

//文件查找与回放
NET_SDK_API LONG CALL_METHOD NET_SDK_FindRecDate(LONG lUserID);
NET_SDK_API LONG CALL_METHOD NET_SDK_FindNextRecDate(LONG lFindHandle, DD_DATE *lpRecDate);
NET_SDK_API BOOL CALL_METHOD NET_SDK_FindRecDateClose(LONG lFindHandle);
NET_SDK_API LONG CALL_METHOD NET_SDK_FindFile(LONG lUserID,LONG lChannel, DD_TIME * lpStartTime, DD_TIME * lpStopTime);
NET_SDK_API LONG CALL_METHOD NET_SDK_FindNextFile(LONG lFindHandle,NET_SDK_REC_FILE* lpFindData);
NET_SDK_API BOOL CALL_METHOD NET_SDK_FindClose(LONG lFindHandle);

NET_SDK_API LONG CALL_METHOD NET_SDK_FindEvent(LONG lUserID,LONG lChannel,DWORD dwRecType, DD_TIME * lpStartTime, DD_TIME * lpStopTime);
NET_SDK_API LONG CALL_METHOD NET_SDK_FindNextEvent(LONG lFindHandle,NET_SDK_REC_EVENT *lpRecEvent);
NET_SDK_API BOOL CALL_METHOD NET_SDK_FindEventClose(LONG lFindHandle);

NET_SDK_API LONG CALL_METHOD NET_SDK_FindTime(LONG lUserID,LONG lChannel, DD_TIME * lpStartTime, DD_TIME * lpStopTime);
NET_SDK_API LONG CALL_METHOD NET_SDK_FindNextTime(LONG lFindHandle, NET_SDK_REC_TIME *lpRecTime);
NET_SDK_API BOOL CALL_METHOD NET_SDK_FindTimeClose(LONG lFindHandle);

NET_SDK_API BOOL CALL_METHOD NET_SDK_LockFile(LONG lUserID, NET_SDK_REC_FILE *pFileToLock, LONG fileNum);
NET_SDK_API BOOL CALL_METHOD NET_SDK_UnlockFile(LONG lUserID, NET_SDK_REC_FILE *pFileToUnlock, LONG fileNum);
NET_SDK_API BOOL CALL_METHOD NET_SDK_DeleteRecFile(LONG lUserID, NET_SDK_REC_FILE *pFileToUnlock, LONG fileNum);

NET_SDK_API LONG CALL_METHOD NET_SDK_PlayBackByTime(LONG lUserID,LONG *pChannels, LONG channelNum, DD_TIME * lpStartTime, DD_TIME * lpStopTime, HWND *hWnds);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PlayBackControl(LONG lPlayHandle,DWORD dwControlCode,DWORD dwInValue,DWORD *lpOutValue);
NET_SDK_API BOOL CALL_METHOD NET_SDK_StopPlayBack(LONG lPlayHandle);
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetPlayDataCallBack(LONG lPlayHandle,PLAY_DATA_CALLBACK fPlayDataCallBack, void *pUser);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PlayBackSaveData(LONG lPlayHandle, LONG lChannel, char *sFileName);
NET_SDK_API BOOL CALL_METHOD NET_SDK_StopPlayBackSave(LONG lPlayHandle, LONG lChannel);
NET_SDK_API BOOL CALL_METHOD NET_SDK_GetPlayBackOsdTime(LONG lPlayHandle, DD_TIME * lpOsdTime);
NET_SDK_API BOOL CALL_METHOD NET_SDK_PlayBackCaptureFile(LONG lPlayHandle, LONG lChannel, char *sFileName);

NET_SDK_API LONG CALL_METHOD NET_SDK_GetFileByTime(LONG lUserID,LONG lChannel, DD_TIME * lpStartTime, DD_TIME * lpStopTime, char *sSavedFileName);
NET_SDK_API BOOL CALL_METHOD NET_SDK_StopGetFile(LONG lFileHandle);
NET_SDK_API int CALL_METHOD NET_SDK_GetDownloadPos(LONG lFileHandle);

//升级
NET_SDK_API LONG CALL_METHOD NET_SDK_Upgrade(LONG lUserID, char *sFileName);
NET_SDK_API int CALL_METHOD NET_SDK_GetUpgradeState(LONG lUpgradeHandle);
NET_SDK_API int CALL_METHOD NET_SDK_GetUpgradeProgress(LONG lUpgradeHandle);
NET_SDK_API BOOL CALL_METHOD NET_SDK_CloseUpgradeHandle(LONG lUpgradeHandle);

//远程管理及格式化硬盘
NET_SDK_API LONG CALL_METHOD NET_SDK_FindDisk(LONG lUserID);
NET_SDK_API BOOL CALL_METHOD NET_SDK_GetNextDiskInfo(LONG lDiskHandle, NET_SDK_DISK_INFO *pDiskInfo);
NET_SDK_API BOOL CALL_METHOD NET_SDK_FindDiskClose(LONG lDiskHandle);
NET_SDK_API BOOL CALL_METHOD NET_SDK_ChangeDiskProperty(LONG lUserID, LONG lDiskNumber, short newProperty);
NET_SDK_API LONG CALL_METHOD NET_SDK_FormatDisk(LONG lUserID,LONG lDiskNumber);
NET_SDK_API BOOL CALL_METHOD NET_SDK_GetFormatProgress(LONG lFormatHandle, LONG *pCurrentFormatDisk,LONG *pCurrentDiskPos,LONG *pFormatStatic);
NET_SDK_API BOOL CALL_METHOD NET_SDK_CloseFormatHandle(LONG lFormatHandle);

//报警
NET_SDK_API LONG CALL_METHOD NET_SDK_SetupAlarmChan(LONG lUserID);
NET_SDK_API BOOL CALL_METHOD NET_SDK_CloseAlarmChan(LONG lAlarmHandle);

//语音对讲
NET_SDK_API LONG CALL_METHOD NET_SDK_StartVoiceCom(LONG lUserID, BOOL bNeedCBNoEncData, TALK_DATA_CALLBACK fVoiceDataCallBack, void* pUser);
NET_SDK_API BOOL CALL_METHOD NET_SDK_GetAudioInfo(LONG lVoiceComHandle, void *pAudioInfo, LONG infoLen);
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetVoiceComClientVolume(LONG lVoiceComHandle, WORD wVolume);
NET_SDK_API BOOL CALL_METHOD NET_SDK_StopVoiceCom(LONG lVoiceComHandle);

//语音转发
NET_SDK_API LONG CALL_METHOD NET_SDK_StartVoiceCom_MR(LONG lUserID, BOOL bNeedNoEncodeData, TALK_DATA_CALLBACK fVoiceDataCallBack, void* pUser);
NET_SDK_API BOOL CALL_METHOD NET_SDK_VoiceComSendData(LONG lVoiceComHandle, char *pSendBuf, DWORD dwBufSize);

//////////////////////////////////////////////////////////////////////////
//透明串口功能未提供

//音频解码
NET_SDK_API LONG CALL_METHOD NET_SDK_InitAudioDecoder(void *pAudioInfo, LONG infoLen);
NET_SDK_API BOOL CALL_METHOD NET_SDK_DecodeAudioFrame(LONG lDecHandle, unsigned char* pInBuffer, LONG inLen, unsigned char* pOutBuffer, LONG *pOutLen);
NET_SDK_API void CALL_METHOD NET_SDK_ReleaseAudioDecoder(LONG lDecHandle);
//音频编码
NET_SDK_API LONG CALL_METHOD NET_SDK_InitAudioEncoder(void *pAudioInfo, LONG infoLen);
NET_SDK_API BOOL CALL_METHOD NET_SDK_EncodeAudioFrame(LONG lEncodeHandle,unsigned char* pInBuffer, LONG inLen, unsigned char* pOutBuffer, LONG *pOutLen);
NET_SDK_API void CALL_METHOD NET_SDK_ReleaseAudioEncoder(LONG lEncodeHandle);

//远程控制设备端手动录像
NET_SDK_API BOOL CALL_METHOD NET_SDK_StartDVRRecord(LONG lUserID,LONG lChannel,LONG lRecordType);
NET_SDK_API BOOL CALL_METHOD NET_SDK_StopDVRRecord(LONG lUserID,LONG lChannel);

//日志
NET_SDK_API LONG CALL_METHOD NET_SDK_FindDVRLog(LONG lUserID, DWORD dwType, DD_TIME * lpStartTime, DD_TIME * lpStopTime);
NET_SDK_API LONG CALL_METHOD NET_SDK_FindNextLog(LONG lLogHandle, LPNET_SDK_LOG lpLogData);
NET_SDK_API BOOL CALL_METHOD NET_SDK_FindLogClose(LONG lLogHandle);

//JPEG抓图到内存
NET_SDK_API BOOL CALL_METHOD NET_SDK_CaptureJPEGPicture(LONG lUserID, LONG lChannel, LPNET_SDK_JPEGPARA lpJpegPara, char *sJpegPicBuffer, DWORD dwPicSize,  LPDWORD lpSizeReturned);
NET_SDK_API BOOL CALL_METHOD NET_SDK_CaptureJpeg(LONG lUserID, LONG lChannel, LONG dwResolution, char *sJpegPicBuffer, DWORD dwPicBufSize,  LPDWORD lpSizeReturned);
//end
NET_SDK_API BOOL CALL_METHOD NET_SDK_RefreshPlay(LONG lPlayHandle);
//恢复默认值
NET_SDK_API BOOL CALL_METHOD NET_SDK_RestoreConfig(LONG lUserID);
//保存参数
NET_SDK_API BOOL CALL_METHOD NET_SDK_SaveConfig(LONG lUserID);
//重启
NET_SDK_API BOOL CALL_METHOD NET_SDK_RebootDVR(LONG lUserID);
//关闭DVR
NET_SDK_API BOOL CALL_METHOD NET_SDK_ShutDownDVR(LONG lUserID);

//参数配置
NET_SDK_API LONG CALL_METHOD NET_SDK_EnterDVRConfig(LONG lUserID);
NET_SDK_API BOOL CALL_METHOD NET_SDK_GetDVRConfig(LONG lUserID, DWORD dwCommand, LONG lChannel, LPVOID lpOutBuffer, DWORD dwOutBufferSize, LPDWORD lpBytesReturned, BOOL bDefautlConfig);
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetDVRConfig(LONG lUserID, DWORD dwCommand, LONG lChannel, LPVOID lpInBuffer,  DWORD dwInBufferSize);
NET_SDK_API BOOL CALL_METHOD NET_SDK_ExitDVRConfig(LONG lUserID);
NET_SDK_API BOOL CALL_METHOD NET_SDK_GetVideoEffect(LONG lUserID, LONG lChannel, DWORD *pBrightValue, DWORD *pContrastValue, DWORD *pSaturationValue, DWORD *pHueValue);
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetVideoEffect(LONG lUserID, LONG lChannel, DWORD dwBrightValue, DWORD dwContrastValue, DWORD dwSaturationValue, DWORD dwHueValue);
NET_SDK_API BOOL CALL_METHOD NET_SDK_SaveVideoEffect(LONG lUserID, LONG lChannel, DWORD dwBrightValue, DWORD dwContrastValue, DWORD dwSaturationValue, DWORD dwHueValue);
NET_SDK_API BOOL CALL_METHOD NET_SDK_GetDefaultVideoEffect(LONG lUserID, DWORD *pBrightValue, DWORD *pContrastValue, DWORD *pSaturationValue, DWORD *pHueValue);

//修改设备系统时间
NET_SDK_API BOOL CALL_METHOD NET_SDK_ChangTime(LONG lUserID, unsigned long time);

//配置文件导出导入
NET_SDK_API BOOL CALL_METHOD NET_SDK_GetConfigFile(LONG lUserID, char *sFileName);
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetConfigFile(LONG lUserID, char *sFileName);

//启用日志文件写入接口
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetLogToFile(BOOL bLogEnable = FALSE, char * strLogDir = NULL, BOOL bAutoDel = TRUE);

//获取设备信息
NET_SDK_API BOOL CALL_METHOD NET_SDK_GetDeviceInfo(LONG lUserID, LPNET_SDK_DEVICEINFO pdecviceInfo);
NET_SDK_API LONG CALL_METHOD NET_SDK_GetDeviceTypeName(LONG lUserID, char *pNameBuffer, long bufferLen);

//////////////////////////////////////////////////////////////////////////
//2012-3-15增加SDK接收DVR主动注册的功能

//设备接收DVR主动注册的本地平台端口号
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetRegisterPort(WORD wRegisterPort);
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetRegisterCallback(ACCEPT_REGISTER_CALLBACK fRegisterCBFun, void *pUser);

//////////////////////////////////////////////////////////////////////////
//2012-2-18增加整型时间转换为格式化的时间的接口
NET_SDK_API void CALL_METHOD NET_SDK_FormatTime(LONGLONG intTime, DD_TIME *pFormatTime);

/////////////////////////////////////////////////////////////////////////
//2012-6-4增加事件搜索接口
NET_SDK_API LONG CALL_METHOD NET_SDK_FindEventInfo(LONG lUserID,DWORD dwType, ULONGLONG channlMask, DD_TIME *lpStartTime, DD_TIME *lpStopTime);
NET_SDK_API LONG CALL_METHOD NET_SDK_FindNextEventInfo(LONG lEventHandle,LPNET_SDK_EVENT lpEventData);
NET_SDK_API BOOL CALL_METHOD NET_SDK_FindEventInfoClose(LONG lEventHandle);



//////////////////////////////////////////////////////////////////////////
//2012-6-19增加获取设备当前时间接口
NET_SDK_API BOOL CALL_METHOD NET_SDK_GetDeviceTime(LONG lUserID,DD_TIME *pTime);

//2014-10-25 增加手动开启\关闭报警相关接口
NET_SDK_API BOOL CALL_METHOD NET_SDK_SetDeviceManualAlarm(LONG lUserID, LONG *pAramChannel, LONG *pValue, LONG lAramChannelCount);

#endif
