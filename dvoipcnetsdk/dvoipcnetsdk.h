/**
*  COPYRIGHT NOTICE 
*  Copyright (c) 2015, Diveo Technology Co.,Ltd (www.diveo.com.cn)
*  All rights reserved.
*
*  @file        dvoipcnetsdk.h
*  @author      luo_ws(luo_ws@diveo.com.cn)
*  @date        2015/08/11 15:18
*
*  @brief       dvoipcnetsdk头文件
*  @note        无
*
*  @version
*    - v1.0.1.0    2015/08/11 15:18    luo_ws 
*    - v2.0.5.1    2016/03/30 09:18    luo_ws 
*/

#ifndef DVO_IPC_NET_SDK_H_
#define DVO_IPC_NET_SDK_H_


#if (defined(WIN32) || defined(WIN64))

#ifdef DVOIPCNETSDK_EXPORTS

#ifndef DVOIPCNETSDK_API
#define DVOIPCNETSDK_API extern "C" __declspec(dllexport)
#endif

#else

#ifndef DVOIPCNETSDK_API
#define DVOIPCNETSDK_API __declspec(dllimport)
#endif

#endif

#define CALLMETHOD __stdcall
#define CALLBACK   __stdcall

#else	//linux

#define DVOIPCNETSDK_API extern "C"
#define CALLMETHOD
#define CALLBACK

#endif


#include "dvoipcnetsdk_def.h"
#include "ipcMsgHead.h"



#ifdef	__cplusplus
extern "C" {
#endif



///////////////////接口begin///////////////////////////////////////////////////////


/**
*  @date        2015/08/12 13:26
*  @brief       网络SDK 初始化，和上一版本保持一致，就是调用 DVO2_NET_Init
*  @param[in]   bUseThreadPool,启用线程池处理,TRUE为启用,FALSE不启用,默认为FALSE
*  @return      成功返回0，否则为失败的错误代码      
*  @remarks     SDK模块第一次使用时调用，同一进程只需要调用一次,
*               bUseThreadPool参数只有进程第一次调用Init时有效,
*               当上层在SDK的回调函数中有耗时处理时,可设置bUseThreadPool为FALSE,以免影响其他会话;
*               当设置bUseThreadPool为TRUE,上层在SDK的回调函数中不能有耗时处理,以免影响其他会话,
*               大量会话时使用线程池会更高效,占用的系统资源更少。
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_Init(BOOL bUseThreadPool = FALSE);

/**
*  @date        2015/08/12
*  @brief       释放SDK所有相关的资源
*  @return      成功返回0，否则为失败的错误代码       
*  @remarks     结束SDK模块时调用，与DVO2_NET_Init的调用一一对应，否则可能无法卸载DLL或被提前卸载。 
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_Release();

/**
*  @date        2015/08/12
*  @brief       获取网络SDK的版本
*  @param[inout]  pMainVersion,主版本号
*  @param[inout]  pSubVersion,子版本号
*  @return      成功返回0，否则为失败的错误代码      
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetSDKVersion(PARAM_OUT int *pMainVersion, PARAM_OUT int *pSubVersion);

/**
*  @date        2015/08/12
*  @brief       获取错误代码
*  @param[in]   lUserID,DVO2_NET_Login返回值
*  @return      成功返回0，否则为失败的错误代码      
*  @remarks     当获取与用户交互相关的错误号时，需要带上lUserID，否则参数为0时获取SDK的全局错误     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetLastError(PARAM_IN USER_HANDLE lUserID = 0);

/**
*  @date        2015/08/27
*  @brief       启停日志
*  @param[in]   bEnable,TRUE时启用日志文件输出，FALSE时关闭日志文件输出
*  @return      成功返回0，否则为失败的错误代码      
*  @remarks     启用日志输出时，会读取所在进程文件的当前路径中的日志配置文件dvoipcnetsdk.cfg，
                找不到该日志配置文件时，按默认日志配置。配置文件可动态更新设置，设置输出日志等级或是否输出文件。  
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetSDKLogToFile(PARAM_IN BOOL bEnable);


//////////////////////////////////////////////////////////////////////////

/**
*  @date        2015/08/12
*  @brief       登录设备    : 0x01 0x01
*  @param[in]   pDevIP      : 设备IP 
*  @param[in]   wDevPort    : 设备端口，默认6001 
*  @param[in]   pUserName   : 用户名，admin 
*  @param[in]   pPassword   : 用户密码, 
*  @param[inout]  pDevBaseInfo: 设备基本信息.登录成功才有意义
*  @param[inout]  pErrNo      : 返回登录错误码,当函数返回成功时,该参数的值无意义.
*  @param[in]   nWaitTimeSec: 登录超时的时间,单位为毫秒 
*  @return      USER_HANDLE,失败返回DVO_INVALID_HANDLE -1，成功返回设备ID     
*  @remarks     设备默认端口为6001，需要和ipc通信时的登录命令，登录成功之后对设备的操作都可以通过此值(设备句柄)对应到相应的设备，
*/
DVOIPCNETSDK_API USER_HANDLE CALLMETHOD DVO2_NET_Login(PARAM_IN const char* pDevIP,
                                                      PARAM_IN WORD wDevPort,
                                                      PARAM_IN const char* pUserName,
                                                      PARAM_IN const char* pPassword,
                                                      PARAM_INOUT app_net_tcp_sys_logo_info_t* pDevBaseInfo,
                                                      PARAM_INOUT int* pErrNo,
                                                      PARAM_IN  int  nWaitTimeMsecs = 5000
                                                      );

/*
*  @date         2015/11/24                 :
*  @brief        登录信息查询               :
*  @param[in]    lUserID                    :   
*  @param[inout] info                        :登录信息
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO2_NET_RES_INFO(PARAM_IN USER_HANDLE lUserID,
                                                   PARAM_INOUT app_net_tcp_sys_logo_info_t *info);

/**
*  @date        2015/08/12
*  @brief       登出设备    : 0x01 0x02
*  @param[in]   lUserID : DVO2_NET_Login成功的返回值
*  @return      成功返回0，否则为失败的错误代码    
*  @remarks     与DVO2_NET_Login对应，结束与IPC通信时调用。有login一定要有logout，否则会有资源泄漏.     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_Logout(PARAM_IN USER_HANDLE lUserID);

/**
*  @date        2015/08/12
*  @brief       设置网络超时参数
*  @param[in]   lUserID             : DVO2_NET_Login成功的返回值
*  @param[in]   nSendTimeoutMsecs   : 发送超时，单位为毫秒,默认为5秒
*  @param[in]   nRecvTimeoutMsecs   : 接收超时，单位为毫秒,默认为5秒
*  @param[in]   nConnTimeoutMsecs   : 连接超时，单位为毫秒,默认为5秒
*  @return      成功返回0，否则为失败的错误代码
*  @remarks     与具体每个会话lUserID相关，各个会话之间的超时参数相互独立    
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetTimeout(PARAM_IN USER_HANDLE lUserID,
                                                   PARAM_IN int nSendTimeoutMsecs = 5000,    ///< 发送超时，毫秒
                                                   PARAM_IN int nRecvTimeoutMsecs = 5000,    ///< 接收超时，毫秒
                                                   PARAM_IN int nConnTimeoutMsecs = 5000     ///< 连接超时，毫秒
                                                   );


/**
*  @date        2015/08/27
*  @brief       设置断线自动重连
*  @param[in]   lUserID                 : DVO2_NET_Login成功的返回值
*  @param[in]   bAutoReConnect          : 是否断线自动重连，默认为TRUE自动重连，FALSE为断开不重连
*  @param[in]   nReConnectIntervalMsecs : 重连间隔，单位为毫秒,默认为5秒，bAutoReConnect为true时有效。
*  @param[in]   nReConnectType          : 连接类型，默认为0，0--该会话所有的连接,1--会话主连接,2--视频连接。
*  @return      成功返回0，否则为失败的错误代码
*  @remarks     与具体每个会话lUserID相关，上层不调用时此接口时，内部默认为断线自动重连   
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetReconnect(PARAM_IN USER_HANDLE lUserID,
                                                     PARAM_IN BOOL bAutoReConnect          = TRUE,     ///< 断线自动重连
                                                     PARAM_IN int  nReConnectIntervalMsecs = 5000,     ///< 重连间隔，毫秒
                                                     PARAM_IN int  nReConnectType          = 0         ///< 连接类型
                                                     );

/**
*  @date        2015/08/21
*  @brief       消息回调函数,fnDVOCallback_AlarmData_T
*  @param[in]   lUserID       : 会话ID，DVO2_NET_Login成功的返回值
*  @param[in]   nMessageType  : enum DVO_CALLBACK_MESSAGE_TYPE 填充
*  @param[in]   nValue        : 简单的消息值,如断开或重连的视频流ID等
*  @param[in]   pBuf          : 消息数据或IO报警数据
*  @param[in]   nDataSize     : 数据的长度
*  @param[in]   pUser         : 用户数据,为上层DVO2_NET_SetMessageCallback时传入标识
*  @return      无返回值 
*  @remarks     避免在回调函数中进行耗时的处理。     
*/
typedef void (CALLBACK* fnDVOCallback_Message_T)(PARAM_IN USER_HANDLE lUserID,
                                                 PARAM_IN int           nMessageType,
                                                 PARAM_IN int           nValue,
                                                 PARAM_IN const char*   pBuf,
                                                 PARAM_IN int           nDataSize,
                                                 PARAM_IN void*         pUser
                                                 );

/**
*  @date        2015/08/21
*  @brief       设置消息回调函数
*  @param[in]   lUserID       : DVO2_NET_Login成功的返回值
*  @param[in]   funcMessage   : 消息回调函数,用于回调视频数据到上层应用,具体定义参考fnDVOCallback_Message_T
*  @param[in]   pUser         : 用户数据,为上层传入标识，在回调函数中返回给上层
*  @return      成功返回0，否则为失败的错误代码
*  @remarks     主要用于会话异常消息的通知，如连接断开，重新建立连接等通知。
                当DVO2_NET_RegisterAlarm开启报警后，IPC上传报警数据后也通过此回调函数返回给上层。     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetMessageCallback(PARAM_IN USER_HANDLE lUserID,
                                                           PARAM_IN fnDVOCallback_Message_T funcMessage,
                                                           PARAM_IN void* pUser
                                                           );

/**
*  @date        2015/08/21
*  @brief       设置报警
*  @param[in]   lUserID     : DVO_NET_Login成功的返回值
*  @param[in]   bRegister   : 开启报警 0--关闭，1--开启。
*  @return      成功返回0，否则为失败的错误代码
*  @remarks     该接口允许调用多次 ,每一次开启一路视频连接 ，支持ipc存在多个通道时调用的情况。     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_RegisterAlarm(PARAM_IN USER_HANDLE lUserID,
                                                      PARAM_IN BOOL        bRegister   //开启报警bRegister 0 关闭，1开启,
                                                      ); 


//////////////////////////////////////////////////////////////////////////


/**
*  @date        2015/08/12
*  @brief       通道实时流数据回调函数,fnDVOCallback_RealAVData_T
*  @param[in]   lUserID       : 会话ID，DVO2_NET_Login成功的返回值
*  @param[in]   lStreamHandle : 流句柄,DVO2_NET_StartRealPlay成功后的返回值
*  @param[in]   nErrorType    : 0表示无异常，1表示断线，2表示数据出错，3表示启动失败。4表示directdraw初始化失败...enum DVO_STREAM_ERROR_TYPE 填充
*  @param[in]   pBuf          : 流数据(包含帧头,app_net_tcp_enc_stream_head_t + stream_data)
*  @param[in]   nDataSize     : 流数据的长度(包含帧头长度,sizeof(app_net_tcp_enc_stream_head_t) + frame_len)
*  @param[in]   pUser         : 用户数据,为上层DVO2_NET_StartRealPlay时传入标识
*  @return      无返回值 
*  @remarks     避免在回调函数中进行耗时的处理。     
*/
typedef void (CALLBACK* fnDVOCallback_RealAVData_T)(PARAM_IN USER_HANDLE  lUserID,
                                                     PARAM_IN REAL_HANDLE lStreamHandle,
                                                     PARAM_IN int         nErrorType,
                                                     PARAM_IN const char* pBuf,
                                                     PARAM_IN int         nDataSize,
                                                     PARAM_IN void*       pUser);


/**
*  @date        2015/08/12
*  @brief       开始视频
*  @param[in]   lUserID     : DVO2_NET_Login成功的返回值
*  @param[in]   nChannelID  : 设备通道号，从0开始
*  @param[in]   nStreamID   : 码流ID，从0开始，0为主码流，1为子码流，2为三码流等
*  @param[in]   nProtocol   : 通信协议类型，0--TCP ;1--UDP(UDP暂不支持)
*  @param[in]   nEncodeType : 编码类型，0--H264，1--JPEG，2--265， 目前ipc只支持2种以后可以扩展
*  @param[in]   hWnd        : 为传入显示预览的窗口句柄,该版本传NULL,只获取码流
*  @param[in]   funcRealData: 回调函数,用于回调视频数据到上层应用,具体定义参考fnDVOCallback_RealAVData_T
*  @param[in]   pUser       : 用户数据,为上层传入标识，在视频回调函数中返回给上层
*  @param[inout]  pErrNo    : 返回错误码,当函数返回成功时,该参数的值无意义.
*  @return      REAL_HANDLE,失败返回DVO_INVALID_HANDLE -1，成功返回视频会话ID 
*  @remarks     该接口允许调用多次 ,每一次开启一路视频连接 ，支持ipc存在多个通道时调用的情况。     
*/
DVOIPCNETSDK_API REAL_HANDLE CALLMETHOD	DVO2_NET_StartRealPlay(PARAM_IN USER_HANDLE lUserID,
                                                              PARAM_IN int nChannelID,
                                                              PARAM_IN int nStreamID,
                                                              PARAM_IN int nProtocol,
                                                              PARAM_IN int nEncodeType,
                                                              PARAM_IN HWND hWnd,
                                                              PARAM_IN fnDVOCallback_RealAVData_T funcRealData,
                                                              PARAM_IN void* pUser,
                                                              PARAM_INOUT int* pErrNo);


/**
*  @date        2015/08/12
*  @brief       关闭视频
*  @param[in]   lStreamHandle,DVO2_NET_StartRealPlay成功的返回值
*  @return      成功返回0，否则为失败的错误代码    
*  @remarks     与DVO2_NET_StartRealPlay对应.     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_StopRealPlay(PARAM_IN REAL_HANDLE lStreamHandle);


/**
*  @date        2015/09/06
*  @brief       获取每秒的流量统计（字节）
*  @param[in]   lStreamHandle,DVO2_NET_StartRealPlay成功的返回值 
*  @param[inout]  pFluxBytes,返回码流每秒的流量,单位为字节Byte 
*  @return      成功返回0，否则为失败的错误代码    
*  @remarks          
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetStatiscFlux(PARAM_IN REAL_HANDLE lStreamHandle, PARAM_INOUT UINT64* pFluxBytes);


/**
*  @date        2015/09/06
*  @brief       开关设备音频流的传输
*  @param[in]   lUserID     : 设备登陆句柄 ，DVO2_NET_Login成功的返回值
*  @param[in]   nChannelID  : 设备通道号，从0开始
*  @param[in]   bEnable     : 开关传输设备音频流 FALSE--停止，TRUE--开启。
*  @return      成功返回0，否则为失败的错误代码
*  @remarks     设置为TRUE，才会有音频数据在实时流数据的fnDVOCallback_RealAVData_T回调中出来    
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_RequestAudioStream(PARAM_IN USER_HANDLE lUserID,
                                                           PARAM_IN int         nChannelID,
                                                           PARAM_IN BOOL        bEnable   // 0 停止，1开启,
                                                           ); 

/**
*  @date         2015/09/06
*  @brief        开关设备音频流的传输状态
*  @param[in]    lUserID     : 设备登陆句柄 ，DVO2_NET_Login成功的返回值
*  @param[in]    nChannelID  : 设备通道号，从0开始
*  @param[inout] pEnable     : 开关传输设备音频流状态 0--停止，1--开启。
*  @return       成功返回0，否则为失败的错误代码
*  @remarks        
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetAudioStreamState(PARAM_IN USER_HANDLE lUserID,
                                                            PARAM_IN int         nChannelID,
                                                            PARAM_INOUT int*     pEnable   //bEnable 0 停止，1开启,
                                                            ); 

//////////////////////////////////////////////////////////////////////////

/**
*  @brief        获取设备当前的系统时间
*  @param[in]    lUserID     : 设备登陆句柄 ，DVO2_NET_Login成功的返回值
*  @param[inout] pDevTime    : 系统时间。
*  @return       成功返回0，否则为失败的错误代码
*  @remarks        
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetDevSystime(PARAM_IN USER_HANDLE lUserID,
                                                       PARAM_INOUT app_net_tcp_sys_time_t* pDevTime
                                                       ); 

/**
*  @brief       设置设备当前的系统时间
*  @param[in]   lUserID     : 设备登陆句柄 ，DVO2_NET_Login成功的返回值
*  @param[out]  pDevTime    : 系统时间
*  @return      成功返回0，否则为失败的错误代码
*  @remarks        
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetDevSystime(PARAM_IN USER_HANDLE lUserID,
                                                      PARAM_IN app_net_tcp_sys_time_t* pDevTime
                                                      ); 

/**
*  @date        2015/08/18
*  @brief       DVO2_NET_GetDevConfig,读取设备的配置信息
*  @param[in]   lUserID         : 设备登陆句柄 
*  @param[in]   dwCommand       : 设备配置命令.参见配置命令,EDVO_DEVICE_PARAMETER_TYPE 
*  @param[in]   lpInBuffer      : 输出参数的缓冲指针 
*  @param[in]   nInBufferSize   : 输出参数的缓冲长度(以字节为单位) 
*  @param[inout] lpOutBuffer     : 接受数据缓冲指针 
*  @param[in]    nOutBufferSize  : 接收数据缓冲长度(以字节为单位) 
*  @param[inout] lpBytesReturned : 实际收到数据的长度 
*  @param[in]   nWaitTimeMsecs  : 等待超时时间 
*  @return      成功返回0，否则为失败的错误代码
*  @pre         
*  @remarks     不同dwCommand(wCmdMain | (wCmdSub<<16) )，lpOutBuffer对应的结构体将会不同，具体见EDVO_DEVICE_PARAMETER_TYPE     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetDevConfig(PARAM_IN  USER_HANDLE lUserID,
                                                     PARAM_IN  DWORD    dwCommand,
                                                     PARAM_IN  LPVOID   lpInBuffer,  
                                                     PARAM_IN  int      nInBufferSize,
                                                     PARAM_INOUT LPVOID lpOutBuffer,  
                                                     PARAM_IN  int      nOutBufferSize,
                                                     PARAM_INOUT int*   pBytesReturned,  
                                                     PARAM_IN  int      nWaitTimeMsecs=3000);

/**
*  @date        2015/08/18
*  @brief       设置设备的配置信息 
*  @param[in]   lUserID         : 设备登陆句柄 
*  @param[in]   dwCommand       : 设备配置命令.参见配置命令,EDVO_DEVICE_PARAMETER_TYPE 
*  @param[in]   lpInBuffer      : 数据缓冲指针 
*  @param[in]   nInBufferSize   : 数据缓冲长度(以字节为单位) 
*  @param[in]   nWaitTimeMsecs  : 等待时间 
*  @return      成功返回0，否则为失败的错误代码
*  @pre         
*  @remarks     不同dwCommand(wCmdMain | (wCmdSub<<16) )，lpOutBuffer对应的结构体将会不同，具体见EDVO_DEVICE_PARAMETER_TYPE 
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetDevConfig(PARAM_IN  USER_HANDLE lUserID,  
                                                     PARAM_IN  DWORD   dwCommand, 
                                                     PARAM_OUT LPVOID  lpInBuffer,  
                                                     PARAM_OUT int     nInBufferSize,  
                                                     PARAM_IN  int     nWaitTimeMsecs=3000);

/**
*  @brief       设备的配置信息扩展接口，需返回数据的通信(设置) 
*  @param[in]   lUserID         : 设备登陆句柄 
*  @param[in]   dwCommand       : 设备配置命令.参见配置命令,EDVO_DEVICE_PARAMETER_TYPE 
*  @param[in]   lpInBuffer      : 输出参数的缓冲指针 
*  @param[in]   nInBufferSize   : 输出参数的缓冲长度(以字节为单位) 
*  @param[inout] lpOutBuffer     : 接受数据缓冲指针 
*  @param[in]    nOutBufferSize  : 接收数据缓冲长度(以字节为单位) 
*  @param[inout] lpBytesReturned : 实际收到数据的长度 
*  @param[in]   nWaitTimeMsecs  : 等待超时时间 
*  @return      成功返回0，否则为失败的错误代码
*  @pre         
*  @remarks     不同dwCommand(wCmdMain | (wCmdSub<<16) )，lpOutBuffer对应的结构体将会不同，具体见EDVO_DEVICE_PARAMETER_TYPE. 
*               既可用于设置，也可用于获取
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_DevConfig(PARAM_IN  USER_HANDLE lUserID,
                                                   PARAM_IN  DWORD    dwCommand,
                                                   PARAM_IN  LPVOID   lpInBuffer,  
                                                   PARAM_IN  int      nInBufferSize,
                                                   PARAM_INOUT LPVOID lpOutBuffer,  
                                                   PARAM_IN  int      nOutBufferSize,
                                                   PARAM_INOUT int*   pBytesReturned,
                                                   PARAM_IN  int      nWaitTimeMsecs=3000);

/*
*  @date         2016/01/08
*  @brief        相机功能型号设置           : 0x01 0x23
*  @param[in]    lUserID                    :
*  @param[in]    pModeInfo                  : 相机功能
*  @return       成功返回0，否则为失败的错误代码
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetFuncMode(PARAM_IN USER_HANDLE lUserID, PARAM_IN app_net_tcp_func_model_t* pModeInfo);

/*
*  @date         2016/01/08
*  @brief        相机功能型号查询           : 0x01 0x24
*  @param[in]    lUserID                    :
*  @param[inout] pModeInfo                  :相机功能
*  @return       成功返回0，否则为失败的错误代码
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetFuncMode(PARAM_IN USER_HANDLE lUserID, PARAM_INOUT app_net_tcp_func_model_t* pModeInfo);

/**
*  @brief       获取自定义区域OSD参数
*  @param[in]   lUserID     : 设备登陆句柄 ，DVO2_NET_Login成功的返回值
*  @param[in]   pOsdChn     : 查询条件
*  @param[inout]  pCustomOSD : 设备自定义区域OSD参数。
*  @return      成功返回0，否则为失败的错误代码
*  @remarks        
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetDevCustomOSD(PARAM_IN USER_HANDLE lUserID,
                                                         PARAM_IN app_net_tcp_custom_osd_zone_chn_t* pOsdChn,
                                                         PARAM_INOUT app_net_tcp_custom_osd_zone_para_t* pCustomOSD
                                                         ); 

/**
*  @brief       设置自定义区域OSD参数
*  @param[in]   lUserID     : 设备登陆句柄 ，DVO2_NET_Login成功的返回值
*  @param[in]   pCustomOSD  : 设备自定义区域OSD参数。
*  @param[inout] pCustomOSDAck : OSD返回值
*  @return      成功返回0，否则为失败的错误代码
*  @remarks        
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetDevCustomOSD(PARAM_IN USER_HANDLE lUserID,
                                                         PARAM_IN app_net_tcp_custom_osd_zone_cfg_t* pCustomOSD,
                                                         PARAM_INOUT app_net_tcp_custom_osd_zone_cfg_ack_t* pCustomOSDAck
                                                         ); 

/**
*  @brief       设置自定义OSD每行的文本
*  @param[in]   lUserID     : 设备登陆句柄 ，DVO2_NET_Login成功的返回值
*  @param[in]   pCustomOSD  : 设备自定义区域OSD每行的文本参数。
*  @return      成功返回0，否则为失败的错误代码
*  @remarks        
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetDevCustomOSDLineData(PARAM_IN USER_HANDLE lUserID,
                                                                 PARAM_IN app_net_tcp_custom_osd_line_data_t* pCustomOSDLineData
                                                                 );

/**
*  @brief       批量设置多行OSD文本 
*  @param[in]   lUserID     : 设备登陆句柄 ，DVO2_NET_Login成功的返回值
*  @param[in]   pCustomOSD  : 设备自定义区域多行OSD的文本参数。
*  @return      成功返回0，否则为失败的错误代码
*  @remarks     批量设置多行OSD文本   
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetDevCustomOSDLineArray(PARAM_IN USER_HANDLE lUserID,
                                                                  PARAM_IN app_net_tcp_custom_osd_line_data_array_t* pCustomOSDLineArray
                                                                  );

//////////////////////////////////////////////////////////////////////////


/**
*  @brief       DVO2_NET_Restore,恢复出厂设置
*  @param[in]   lUserID 设备登陆句柄 
*  @param[in]   nTypeOperator ,恢复操作的类型：0为不包含ip恢复，1为包含ip恢复默认出厂值
*  @return      成功返回0，否则为失败的错误代码        
*  @remarks     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_Restore(PARAM_IN USER_HANDLE lUserID, PARAM_IN int nTypeOperator);

/**
*  @brief       DVO2_NET_Reboot,相机重启
*  @param[in]   lUserID 设备登陆句柄 
*  @return      成功返回0，否则为失败的错误代码        
*  @remarks          
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_Reboot(PARAM_IN USER_HANDLE lUserID);

/**
*  @brief       DVO2_NET_ShutDown,关闭相机
*  @param[in]   lUserID 设备登陆句柄 
*  @return      成功返回0，否则为失败的错误代码        
*  @remarks          
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_ShutDown(PARAM_IN USER_HANDLE lUserID);



///////////////////////语音对讲,暂未实现///////////////////////////////////////////////////

/** fnDVOCallback_AudioData_T
 * 音频对讲获得数据回调函数
	lTalkHandle   : 对讲句柄
	pBuf          : 音频数据
	nDataSize     : 数据长度
	pts           : 时间戳(毫秒)
	userdata      : 用户数据
*/
typedef void (*fnDVOCallback_AudioData_T)(PARAM_IN USER_HANDLE  lUserID,
                                          PARAM_IN REAL_HANDLE  lTalkHandle,
                                          PARAM_IN BYTE*        pBuf,
                                          PARAM_IN int          nDataSize,
                                          PARAM_IN int          pts,
                                          PARAM_IN void*        pUser
                                          );

/** 
 *  @brief       启动语音对讲，DVO2_NET_TalkStart
 *  @param[in]   lUserID        : DVO2_NET_Login成功的返回值
 *  @param[in]   pAudioInfo		: 音频参数
 *  @param[in]   nTimeroutMs	: 超时时间(毫秒)
 *  @param[in]   bTwoWayData	: 双向对讲标志
 *  @param[in]   funcAudioData  : 接收对讲数据回调函数,用于回调对讲数据到上层应用,具体定义参考fnDVOCallback_AudioData_T
 *  @param[in]   pUser          : 用户数据,为上层传入标识，在视频回调函数中返回给上层
 *  @param[out]  pErrNo         : 返回错误码,当函数返回成功时,该参数的值无意义.
 *  @return      REAL_HANDLE,失败返回DVO_INVALID_HANDLE -1，成功返回语音对讲会话ID 
 *  @remarks     
*/
DVOIPCNETSDK_API REAL_HANDLE CALLMETHOD DVO2_NET_TalkStart(PARAM_IN USER_HANDLE lUserID,
                                                          PARAM_IN app_net_tcp_audio_config_t_v2 *pAudioInfo,
								                          PARAM_IN int nTimeroutMs,
								                          PARAM_IN BOOL bTwoWayData,
								                          PARAM_IN fnDVOCallback_AudioData_T funcAudioData,
                                                          PARAM_IN void* pUser,
                                                          PARAM_OUT int* pErrNo
                                                          );

/** 
 *  @brief       发送对讲数据,DVO2_NET_TalkSendData
 *  @param[in]   lTalkHandle		: 对讲句柄, DVO2_NET_TalkStart的输出值
 *  @param[in]   pBuf   			: 音频数据
 *  @param[in]   nDataSize			: 数据大小
 *  @param[in]   pts				: 时间戳
 *  @return      成功返回0， 其他为错误代码
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_TalkSendData(PARAM_IN REAL_HANDLE lTalkHandle,
                                                     PARAM_IN BYTE* pBuf,
                                                     PARAM_IN int   nDataSize,
                                                     PARAM_IN int   pts
                                                     );

/* DVO2_NET_TalkStop
 * 停止对讲
	lTalkHandle		: 对讲句柄, DVO2_NET_TalkStart 的输出值
 * 成功返回0， 其他为错误代码
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_TalkStop(PARAM_IN USER_HANDLE lTalkHandle);

//////////////////////////////////////////////////////////////////////////

//=================提供设备搜索============================//
// 暂未实现，相关功能请使用DVO搜索工具

/**
*  @brief       搜索设备回调,fnDVOCallback_SearchDevice_T
*  @param[in]   nType         : 0为错误信息,0x0002为搜索设备的信息,
*  @param[in]   nState        : nType=0时有效，为错误ID
*  @param[in]   pBuf          : 数据,multicast_dev_info_t_v2
*  @param[in]   nDataSize     : 数据长度
*  @param[in]   pUser         : 用户数据,为上层DVO2_NET_StartSearchDevice时传入标识
*  @return      无返回值 
*  @remarks     在回调函数中，不要进行耗时处理，否则可能会造成搜索结果不完整。     
*/
typedef void (CALLBACK *fnDVOCallback_SearchDevice_T)(PARAM_IN int         nType,
                                                      PARAM_IN int         nState,
                                                      PARAM_IN const char* pBuf, 
                                                      PARAM_IN int         nDataSize, 
                                                      PARAM_IN void*       pUser
                                                      );

/** 
 *  @brief       开始搜索设备,DVO2_NET_StartSearchDevice
 *  @param[in]   funcSearchDevice : 回调函数,用于回调搜索结果和状态到上层应用,具体定义参考fnDVOCallback_SearchDevice_T
 *  @param[in]   pUser            : 用户数据,为上层传入标识，在回调函数中返回给上层
 *  @return      成功返回0，否则为失败的错误代码 
 *  @remarks     搜索设备处理为异步,通过回调函数反馈处理状态。只有DVO2_NET_StopSearchDevice之后，才能再次调用此接口.   
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_StartSearchDevice(PARAM_IN fnDVOCallback_SearchDevice_T funcSearchDevice,
                                                          PARAM_IN void *pUser
                                                          );

/** 
 *  @brief       停止搜索设备
 *  @return      成功返回0，否则为失败的错误代码 
 *  @remarks     DVO2_NET_StartSearchDevice之后，才能调用此接口  
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_StopSearchDevice();

/** 
 *  @brief       再次搜索设备
 *  @return      成功返回0，否则为失败的错误代码 
 *  @remarks     必须先启动搜索模块,DVO2_NET_StartSearchDevice，才能调用此接口，否则无效.  
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_AgainSearchDevice();

/** 
 *  @brief       修改设备配置
 *  @param[in]   pDevInfo       : 设备信息
 *  @param[in]   nModifyType    : 修改的配置类型，0为修改设备IP地址,1为修改设备MAC地址
 *  @return      成功返回0，否则为失败的错误代码 
 *  @remarks     必须先启动搜索模块,DVO2_NET_StartSearchDevice.  
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_ModifySearchDevice(const multicast_dev_info_t_v2* pDevInfo, int nModifyType);

//////////////////////////////////////////////////////////////////////////

//=================提供系统升级============================//
// 暂未实现，相关功能请使用DVO搜索工具

/**
*  @brief       文件传输回调,fnDVOCallback_AlarmData_T
*  @param[in]   lUserID       : 会话ID，DVO2_NET_Login成功的返回值
*  @param[in]   nTransType    : enum DVO_CALLBACK_TRANSFILE_STATE_TYPE 填充
*  @param[in]   nState        : nTransType=DVO_DEV_UPGRADEFILETRANS_ERROR时有效，为错误ID
*  @param[in]   nSendSize     : 已发送长度
*  @param[in]   nTotalSize    : 文件总长度
*  @param[in]   pUser         : 用户数据,为上层DVO2_NET_StartUpgrade时传入标识
*  @return      无返回值 
*  @remarks     避免在回调函数中进行耗时的处理。     
*/
typedef void (CALLBACK *fnDVOCallback_TransFile_T)(PARAM_IN USER_HANDLE lUserID, 
                                                   PARAM_IN int         nTransType, //DVO_CALLBACK_TRANSFILE_STATE_TYPE
                                                   PARAM_IN int         nState,     
                                                   PARAM_IN int         nSendSize, 
                                                   PARAM_IN int         nTotalSize, 
                                                   PARAM_IN void*       pUser);

/**
*  @brief       开始升级
*  @param[in]   lUserID       : DVO2_NET_Login成功的返回值
*  @param[in]   szFileName    : 升级文件
*  @param[in]   funcTransFile : 回调函数,用于回调传输状态到上层应用,具体定义参考fnDVOCallback_TransFile_T
*  @param[in]   pUser         : 用户数据,为上层传入标识，在回调函数中返回给上层
*  @param[in]   wDevPort      : 设备端口,升级默认用6002 
*  @return      成功返回0，否则为失败的错误代码 
*  @remarks     升级处理为异步,通过回调函数反馈处理状态     
*/
DVOIPCNETSDK_API int CALLMETHOD	DVO2_NET_StartUpgrade(PARAM_IN USER_HANDLE               lUserID, 
                                                     PARAM_IN const char*               szFileName, 
                                                     PARAM_IN fnDVOCallback_TransFile_T funcTransFile, 
                                                     PARAM_IN PARAM_IN void*            pUser,
                                                     PARAM_IN WORD                      wDevPort = 6002
                                                     );

/**
*  @brief       停止升级
*  @param[in]   lUserID : DVO2_NET_Login成功的返回值
*  @return      成功返回0，否则为失败的错误代码    
*  @remarks     与DVO2_NET_StartUpgrade对应,会中止升级文件的传输，如果升级文件传输已经完成，此接口调用无效     
*/
DVOIPCNETSDK_API int CALLMETHOD	DVO2_NET_StopUpgrade(PARAM_IN USER_HANDLE lUserID);
 
///////////////////接口end///////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif
