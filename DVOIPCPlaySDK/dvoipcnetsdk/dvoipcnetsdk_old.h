/*
*  COPYRIGHT NOTICE 
*  Copyright (c) 2015, Diveo Technology Co.,Ltd (www.diveo.com.cn)
*  All rights reserved.
*
*  @file        dvoipcnetsdk1.h
*  @author      jag(luo_ws@diveo.com.cn)
*  @date        2015/11/10 13:42
*
*  @brief       dvoipcnetsdk1头文件
*  @note        对外接口，和以前版本保持兼容
*
*  @version
*    - v1.0.1.0    2015/11/10 13:42    jag 
*/

#ifndef DVO_IPC_NET_SDK_OLD_H_
#define DVO_IPC_NET_SDK_OLD_H_

#include "dvoipcnetsdk.h"
#include "dvoipcnetsdk1_define.h"


#ifdef	__cplusplus
extern "C" {
#endif
///////////////////接口begin///////////////////////////////////////////////////////

/*
*  @date        2015/11/11 
*  @brief       网络SDK 初始化 
*  @param[in]   localport,新版本中，该无效
*  @return      true：成功，fasle：失败    
*  @remarks     和DVO2_NET_Init相同       
*/
DVOIPCNETSDK_API BOOL __stdcall	DVO_NET_Init(long localPort=0);
 
/*
*  @date        2015/08/23 
*  @brief       释放SDK所有相关的资源
*  @return      true：成功，fasle：失败    
*  @remarks     和DVO2_NET_Release相同
*/
DVOIPCNETSDK_API BOOL __stdcall	DVO_NET_Release();  

/*
*  @date        2015/11/11
*  @brief       登录设备                : 0x01 0x01
*  @param[in]   sIP                     :
*  @param[in]   dwPort                  :
*  @param[in]   sUser                   :
*  @param[in]   sPassword               :
*  @return      USER_HANDLE,失败返回DVO_INVALID_HANDLE -1，成功返回设备ID             
*  @remarks     调用DVO2_NET_Login
*/
DVOIPCNETSDK_API USER_HANDLE CALLMETHOD	DVO_NET_Login(const char *sIP,int dwPort,const char *sUser,const char*sPassword);

/*
*  @date        2015/11/11
*  @brief       登录设备                : 0x01 0x02
*  @param[in]   lUserID                 :
*  @return      true：成功，fasle：失败    
*  @remarks     调用DVO2_NET_Logout
*/
DVOIPCNETSDK_API BOOL __stdcall	DVO_NET_Logout(USER_HANDLE lUserID); 


//获取当前用户使用的窗口数，沿用高视捷sdk，DVOSDK每获取一个窗口进行内部计数。 -1 error get channel sum
/*
*  @date        2015/11/11
*  @brief       查询当前用户使用的窗口数
*  @param[in]   lUserID                  :
*  @return      当前用户使用的窗口数      
*/
DVOIPCNETSDK_API long CALLMETHOD  DVO_NET_GetWindowCount(USER_HANDLE lUserID);

/*
*  @date        2015/11/11
*  @brief       获取网络SDK的版本
*  @param[out]  pMainVersion:   主版本号
*  @param[out]  pSubVersion :   子版本号
*  @return      true：成功，fasle：失败    
*  @remarks     直接调用新版本DVO_NET_GetSDKVersion
*/
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_GetNetVersion(long *pMainVersion,
                                                        long *pSubVersion);

/*
*  @date        2015/11/11
*  @brief       获取错误代码
*  @return      返回错误代码    
*  @remarks     调用DVO2_NET_GetLastError
*/
DVOIPCNETSDK_API long __stdcall  DVO_NET_GetLastError();   

/*
*  @date         2015/11/23
*  @brief        重启设备            : 0x01 0x07    
*  @param[in]    lUserID    
*  @remarks      调用DVO2_NET_Reboot
*/
DVOIPCNETSDK_API BOOL __stdcall   DVO_NET_Reboot(USER_HANDLE lUserID); 
 
/*
*  @date         2015/11/23
*  @brief        关闭相机             : 0x01 0x08    
*  @param[in]    lUserID    
*  @remarks      调用DVO2_NET_ShutDown
*/
DVOIPCNETSDK_API BOOL __stdcall DVO_NET_ShutDown(USER_HANDLE lUserID);

/*
*  @date         2015/11/23
*  @brief        恢复出厂设置         : 0x01 0x09    
*  @param[in]    lUserID              : 
*  @param[in]    type                 : type 0 为不包含ip恢复，1为包含ip恢复默认出厂值。
*  @remarks      调用DVO2_NET_Restore
*/
DVOIPCNETSDK_API BOOL __stdcall  DVO_NET_Restore(USER_HANDLE lUserID,int type);

/*
*  @date         2015/11/23
*  @brief        设置连接网络超时     : 
*  @param[in]    timeout_millisec     : 
*  @remarks      几个超时类型时间相同
*/
DVOIPCNETSDK_API void __stdcall DVO_NET_SetTimeout(long timeout_millisec);



/*
*  @date        2015/11/11
*  @brief       开始视频，调用DVO_NET_StartRealPlay
*  @param[in]   lUserID     : DVO_NET_Login成功的返回值
*  @param[in]   slot        : 设备通道号，从0开始
*  @param[in]   issub       : 码流ID，从0开始，0为主码流，1为子码流，2为三码流等
*  @param[in]   connet_mode : 通信协议类型，0--TCP ;1--UDP(UDP暂不支持)
*  @param[in]   funcRealData: 回调函数,用于回调视频数据到上层应用,具体定义参考fnDVOCallback_RealAVData_T
*  @param[in]   encodetype  : 编码类型，0--H264，1--JPEG，2--265， 目前ipc只支持2种以后可以扩展
*  @param[in]   hWnd        : 为传入显示预览的窗口句柄,该版本传NULL,只获取码流    
*  @param[in]   pUser       : 用户数据,为上层传入标识，在视频回调函数中返回给上层
*  @param[out]  pErrNo      : 返回错误码,当函数返回成功时,该参数的值无意义.
*  @return      REAL_HANDLE :失败返回DVO_INVALID_HANDLE -1，成功返回视频会话ID 
*  @remarks     该接口允许调用多次 ,每一次开启一路视频连接 ，支持ipc存在多个通道时调用的情况。  
*/

DVOIPCNETSDK_API REAL_HANDLE  CALLMETHOD	DVO_NET_OpenAVStreamEx2(USER_HANDLE lUserID,
                                                                    int slot,
                                                                    int issub,
                                                                    int connet_mode,
                                                                    void(CALLBACK*func)(long lVideoID,char*msgdata,int len,int errortype,long nUser),
                                                                    HWND hWnd,
                                                                    int encodetype ,
                                                                    long nUser
                                                                    );
 
/*
*  @date        2015/11/11
*  @brief       关闭视频,,调用DVO_NET_StopRealPlay;
*  @param[in]   lStreamHandle, DVO_NET_OpenAVStreamEx2/DVO_NET_StartRealPlay成功的返回值
*  @param[in]   hWnd,不用
*  @return      true：成功，fasle：失败    
*  @remarks     与DVO_NET_OpenAVStreamEx2/DVO_NET_StartRealPlay对应.     
*/
DVOIPCNETSDK_API BOOL	CALLMETHOD	DVO_NET_CloseAVStreamEx2(REAL_HANDLE lRealID,
                                                             HWND hWnd);

/*
*  @date        2015/11/11
*  @brief       打开视频预览/关闭视频预览 , 新版本中该接口不再实现        
*/
DVOIPCNETSDK_API REAL_HANDLE  CALLMETHOD DVO_NET_OpenAVStream_MutiMainStream(USER_HANDLE lUserID,
                                                                             int slot,
                                                                             char* issub,
                                                                             int connet_mode,
                                                                             void(CALLBACK*func)(long lVideoID,char*msgdata,int len,int errortype,long nUser),
                                                                             HWND hWnd,
                                                                             int encodetype ,
                                                                             long nUser);
DVOIPCNETSDK_API BOOL	CALLMETHOD	DVO_NET_CloseAVStreamEx_MutiMainStream(REAL_HANDLE lRealID,
                                                                           HWND hWnd);


/*
*  @date        2015/11/11
*  @brief       打开/关闭声音            : 0x06 0x01
*  @param[in]   lRealID                  :
*  @return      true：成功，fasle：失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_OpenSound(REAL_HANDLE lRealID); 	
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_CloseSound(REAL_HANDLE lRealID); //

/*
*  @date        2015/11/11
*  @brief       视频颜色查询            : 0x03,0x02
*  @param[in]   lRealID                 :
*  @param[out]  pVideoCOlor             : 视频的颜色参数  
*  @return      true成功,false失败
*/	
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GetVideoColor(REAL_HANDLE lRealID,
                                                       dvonetSDK::tVideoColor *pVideoCOlor);

/*
*  @date        2015/11/11
*  @brief       视频颜色设置            : 0x03,0x01
*  @param[in]   lRealID                 : 
*  @param[in]   pVideoColor             : 视频的颜色参数  
*  @return      true成功,false失败
*/	
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SetVideoColor(REAL_HANDLE lRealID,
                                                          dvonetSDK:: tVideoColor *pVideoColor);
/*
*  @date        2015/11/11
*  @brief       获取视频状态,DVR中使用接口,暂不实现
*/		 
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_GetVideoStatus(REAL_HANDLE lRealID,
                                                           dvonetSDK:: tVideoStatus *pVideoStatus);

/*
*  @date        2015/11/11
*  @brief       播放器显示设置,暂不实现    
*/		 
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_SET_PLAYER_SCALE(REAL_HANDLE lRealID,
                                                       const char * scale);

/*
*  @date        2015/11/11
*  @brief       ptz 控制,a暂不实现 
*/		
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_ControlPtz(REAL_HANDLE lRealID,
                                                       dvonetSDK:: tPtzControl *pPtzControl);

/*
*  @date        2015/11/11
*  @brief      snap 保存bmp,a暂不实现    
*/			
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_Snap(REAL_HANDLE lRealID,
                                                 const char * szFile);

/*
*  @date        2015/11/11
*  @brief     /snap 保存jpg,a暂不实现     
*/		
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_SnapJpg(REAL_HANDLE lRealID,
                                                  const char*szFile);

/*   +---    
*  @date     2015/11/11
*  @brief    启动预览录像 /停止预览录,已不用   
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_StartRecord(REAL_HANDLE lRealID,
                                                        const char *szFile);	
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_StopRecord(REAL_HANDLE lRealID);

/*rec video*/ //该部分内容保留以后实现
/*
*  @date      2015/11/11
*  @brief     暂不实现。    
*/
///打开网络播放视频录像
DVOIPCNETSDK_API REC_HANDLE CALLMETHOD	DVO_NET_PLAY_OpenVideo(USER_HANDLE lUserID,
                                                               struct tRecFile *pRecFile,
                                                               HWND hWnd,
                                                               int nPlaySlot); 

DVOIPCNETSDK_API REC_HANDLE CALLMETHOD	DVO_NET_PLAY_OpenVideoEx(USER_HANDLE lUserID,
                                                                 struct tRecFile *pRecFile,
                                                                 void(CALLBACK*func)(long lPlayID,char*buf,int len,int videoType,long nUser),
                                                                 long nUser);
//关闭网络视频录像
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_PLAY_CloseVideo(REC_HANDLE lPlayID); 

//================子码流========================//
//DVOIPCNETSDK_API REAL_HANDLE  CALLMETHOD	DVO_NET_OpenSubVideo(USER_HANDLE lUserID,int slot,HWND hWnd,int nPlaySlot);
//DVOIPCNETSDK_API REAL_HANDLE  CALLMETHOD	DVO_NET_OpenSubVideoEx(USER_HANDLE lUserID,int slot,void(CALLBACK*func)(long lVideoID,char*buf,int len,int videoType,long nUser),long nUser);
//DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetSubChannelSet(USER_HANDLE lUserID,tSubChannelSet* pSubChannelSet);
//DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetSubChannelSet(USER_HANDLE lUserID,tSubChannelSet* pSubChannelSet);

//================= IPCAM ========================该部分需要相机支持，暂时保留//


/*
*  @date         2015/11/11
*  @brief        获取相机属性查询            : 0x03 0x04;0x03 0x06;0x03 0x08
*  @param[in]    lRealID                     :
*  @param[in out]pFeature                    : 相机属性
*  @return       true成功，false失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_IPCam_GetFeature(USER_HANDLE lUserID,dvonetSDK::tIPCam_feature *pFeature);

/*
*  @date         2015/11/11
*  @brief        相机属性设置               : 0x03 0x03;0x03 0x05;0x03 0x07
*  @param[in]    lRealID                    :
*  @param[in]    pFeature                   : 相机属性
*  @return       true成功，false失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_IPCam_SetFeature(USER_HANDLE lUserID,dvonetSDK::tIPCam_feature *pFeature);

/*
*  @date         2015/11/12                 : 有疑问，暂不实现
*  @brief        相机功能型号设置           : 0x01 0x23
*  @param[in]    lRealID                    :
*  @param[in]    pModeID                    : 相机功能
*  @return       true成功，false失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD NET_SET_Func_Mode(USER_HANDLE lUserID,dvonetSDK::IPCModeType *pModeID);

/*
*  @date         2015/11/12                 : 有疑问，暂不实现
*  @brief        相机功能型号查询           : 0x01 0x24
*  @param[in]    lUserID                    :
*  @param[out]    pModeID                    :相机功能
*  @return       true成功，false失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD NET_GET_Func_Mode(USER_HANDLE lUserID,dvonetSDK::IPCModeType *pModeID);

/* 
*  @date         2015/11/12                 : 暂不实现
*  @brief        强光抑制，老版本中未实现，协议中没有发现相关内容
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_IPCAM_GetBlackMaskBLC(USER_HANDLE lUserID,dvonetSDK::tBlackMaskBLC *pBMB);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_IPCAM_SetBlackMaskBLC(USER_HANDLE lUserID,dvonetSDK::tBlackMaskBLC *pBMB);

/* 
*  @date         2015/11/12                  : 暂不实现
*  @brief        数据迁移 ，老版本中未实现，协议中没有发现相关内容
*/ 
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_IPCAM_GetLostDataTransfer(USER_HANDLE lUserID,struct tLostDataTransfer* pLostDataTransfer);
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_SET_IPCAM_SetLostDataTransfer(USER_HANDLE lUserID,struct tLostDataTransfer* pLostDataTransfer);

/* 
*  @date        2015/11/12                  : 暂不实现
*  @brief       自动聚焦 ,老版本中未实现，协议中没有发现相关内容，
*/ 
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_IPCAM_AutoFocusStart(USER_HANDLE lUserID,struct tAutoFocus* pAutoFocus);

/*
*  @date        2015/11/12                  : 暂不实现
*  @brief       保留之前接口
*/  
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetCenterInfo(USER_HANDLE lUserID,dvonetSDK::tCenter* pCenterInfo);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetCenterInfo(USER_HANDLE lUserID,dvonetSDK::tCenter* pCenterInfo);

/* 
*  @date        2015/11/12                  : 暂不实现
*  @brief       强制i帧
*/ 
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_ForceIFrame(USER_HANDLE lUserID,int slot);

/*  
*  @date        2015/11/12                  : 暂不实现
*  @brief       获取报警状态 dvr中接口预留
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetAlarmState(USER_HANDLE lUserID,dvonetSDK::tAlarmState* pAlarmState);


/*
*  @date         2015/11/12
*  @brief        设备信息/资源查询          : 0x01 0x0f,0x01 0d
*  @param[in]    lUserID:   
*  @param[out]   pDevConfig                 : 设备/资源查信息
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetDevConfig(USER_HANDLE lUserID,
                                                             dvonetSDK::tDevConfig* pDevConfig);

/*
*  @date         2015/11/12
*  @brief        设备信息设置                : 0x01 0x10
*  @param[in]    lUserID                     :   
*  @param[in]    pDevConfig                  : 设备信息
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetDevConfig(USER_HANDLE lUserID,
                                                             dvonetSDK::tDevConfig* pDevConfig);

/*
*  @date         2015/11/12
*  @brief        白天黑夜模式查询            : 0x03 0x08       
*  @param[in]    lUserID                     :   
*  @param[out]   gray_mode                   : 白天黑夜模式
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetGrayMode(USER_HANDLE lUserID,
                                                            dvonetSDK::tgray_whitevmode* gray_mode);

/*
*  @date         2015/11/12
*  @brief        白天黑夜模式设置            : 0x03 0x07
*  @param[in]    lUserID                     :   
*  @param[int]   gray_mode                   : 白天黑夜模式
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetGrayMode(USER_HANDLE lUserID,
                                                            dvonetSDK::tgray_whitevmode* gray_mode);

/*
*  @date         2015/11/12
*  @brief        图像曝光参数设置            : 0x03 0x03
*  @param[in]    lUserID                     :   
*  @param[in]    pbea                        : 图像曝光参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_IPCam_Seteba(USER_HANDLE lUserID,
                                                          dvonetSDK::tIPCAM_BEA *pbea);

/*
*  @date         2015/11/12
*  @brief        图像曝光参数查询            : 0x03 0x04
*  @param[in]    lUserID                     :    
*  @param[in]    pbea                        : 图像曝光参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_IPCam_Geteba(USER_HANDLE lUserID,
                                                          dvonetSDK::tIPCAM_BEA *pbea);
 
/*
*  @date         2015/11/12
*  @brief        图像白平衡参数设置            : 0x03 0x05
*  @param[in]    lUserID                       :    
*  @param[in]    pbla                          : 图像白平衡参数
*  @return      true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_IPCam_Setblankbanlace(USER_HANDLE lUserID,
                                                                   dvonetSDK::tIPCAM_BLANCEBLACK *pbla);

/*
*  @date         2015/11/12
*  @brief        图像白平衡参数查询            : 0x03 0x06
*  @param[in]    lUserID                       :   
*  @param[out]   pbla                          : 图像白平衡参数
*  @return      true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_IPCam_Getblankbanlace(USER_HANDLE lUserID,
                                                                   dvonetSDK::tIPCAM_BLANCEBLACK *pbla);

/*
*  @date         2015/11/12
*  @brief        视频输出接口设置            : 0x01 0x17
*  @param[in]    lUserID                     :   
*  @param[in]    vout_mode                   : 视频输出接口 
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetVoutMode(USER_HANDLE lUserID,
                                                            dvonetSDK::tvout_interface* vout_mode);

/*
*  @date         2015/11/12
*  @brief        视频输出接口查询            : 0x01 0x18
*  @param[in]    lUserID                     :   
*  @param[out]   vout_mode                   : 视频输出接口
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetVoutMode(USER_HANDLE lUserID,
                                                            dvonetSDK::tvout_interface* vout_mode);


//=================提供系统升级============================//

DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_UpgradeRequest(USER_HANDLE lUserID,int *ready);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SendUpgradeFile(USER_HANDLE lUserID,const char* fileName);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_StopSendUpgFile(USER_HANDLE lUserID);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetUpgradeState(USER_HANDLE lUserID,int *upgrade_state);

/*
*  @date         2015/11/12                 :暂不实现 需要实现
*  @brief        获得登录信息               :
*  @param[in]    lUserID                    :   
*  @param[out]   inf                        :登录信息
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_RES_INFO(USER_HANDLE lUserID,
                                                  dvonetSDK::app_net_tcp_sys_logo_info_t *info);

/*remote set*/
/*
*  @date         2015/11/13
*  @brief        OSD参数查询                :0x02 0x06
*  @param[in]    lUserID                    :   
*  @param[out]   pOsdName                   : OSD参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetChannelName(USER_HANDLE lUserID,
                                                               dvonetSDK:: tOsdChannel *pOsdName);

/*
*  @date         2015/11/13
*  @brief        OSD参数设置                :0x02 0x05
*  @param[in]    lUserID                    :   
*  @param[in]    pOsdName                   :OSD参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetChannelName(USER_HANDLE lUserID,
                                                            dvonetSDK::tOsdChannel *pOsdName);


//DVR接口预留暂不实现
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetDvrVersion(USER_HANDLE lUserID,long *pVersion);
//DVR接口预留暂不实现
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetDvrNetVersion(USER_HANDLE lUserID,long *pNetVersion);

/*
*  @date         2015/11/16
*  @brief        网络状态   :暂未实现
*  @param[in]    lUserID                :   
*  @param[in]    pIntervalMillSec       :
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetNetState(USER_HANDLE lUserID,long *pIntervalMillSec);

 
/*
*  @date         2015/11/16
*  @brief        和获取远程视频颜色接口DVO_NET_GetVideoColor相同，
*  @param[in]    lUserID                :   
*  @param[in]    pVideoColor            :
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetVideoColor(USER_HANDLE lUserID,
                                                              dvonetSDK::tVideoColor *pVideoColor);


/*
*  @date         2015/11/16
*  @brief        获取远程视频颜色,和接口DVO_NET_SetVideoColor相同，
*  @param[in]    lUserID                :   
*  @param[in]    pVideoColor            :
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetVideoColor(USER_HANDLE lUserID,
                                                              dvonetSDK::tVideoColor *pVideoColor);


/*
*  @date         2015/11/16
*  @brief        远程视频质量参数查询   :0x02 0x02
*  @param[in]    lUserID                :   
*  @param[in]    pVideoQuality          :
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetVideoQuality(USER_HANDLE lUserID,
                                                                dvonetSDK::tVideoQuality *pVideoQuality);

/*
*  @date         2015/11/16
*  @brief        远程视频质量参数设置   : 0x02 0x01
*  @param[in]    lUserID                :   
*  @param[in]    pVideoQuality          :
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetVideoQuality(USER_HANDLE lUserID,
                                                                dvonetSDK:: tVideoQuality *pVideoQuality);

/*
*  @date         2015/11/18             :
*  @brief        osd 时间查询(OSD参数)  : 0x02 0x06
*  @param[in]    lUserID                :   
*  @param[in]    pOsdDate               : osd 时间(OSD参数)
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetOsdDate(USER_HANDLE lUserID,dvonetSDK:: tOsdDate *pOsdDate);

/*
*  @date         2015/11/18               :
*  @brief        osd 时间设置(OSD参数)    : 0x02 0x05
*  @param[in]    lUserID                  :   
*  @param[in]    pOsdDate                 : osd 时间(OSD参数)
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetOsdDate(USER_HANDLE lUserID,
                                                           dvonetSDK:: tOsdDate *pOsdDate);

//暂不实现
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetVideoStandard(USER_HANDLE lUserID,
                                                                 struct tVideoStandard *pVideoStandard);

//设置ptz控制，枪机暂不实现
/*
*  @date        2015/11/18   老版本中没有相应代码，暂未实现
*  @brief       设置ptz控制    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_ControlPTZ(USER_HANDLE lUserID,struct tPtzControl *pPtzControl);
////获取移动侦测参数预留以扩展方式调用
//DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetMotion(USER_HANDLE lUserID,dvonetSDK:: tMotion *pMotionSet);
////设置移动侦测参数预留以扩展方式调用
//DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetMotion(USER_HANDLE lUserID,dvonetSDK:: tMotion *pMotionSet);

/*
*  @date         2015/11/18               : 暂未实现
*  @brief        视频遮盖信息设置(PM参数  : 0x02 0x07 协议有疑问
*  @param[in]    lUserID                  :   
*  @param[in]    pMask                    : 视频遮盖信息
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetMask(USER_HANDLE lUserID,dvonetSDK:: tMask *pMask);

/*
*  @date         2015/11/18               :暂未实现
*  @brief        视频遮盖信息查询         : 0x02 0x08  协议有疑问
*  @param[in]    lUserID                  :   
*  @param[in]    pMask                    : 视频遮盖信息
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetMask(USER_HANDLE lUserID,dvonetSDK:: tMask *pMask);
 
//远程录像，接口预留
/*
*  @date        2015/11/18           :暂未实现
*  @brief       启动/关闭远程录像      
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_StartRecord(USER_HANDLE lUserID,int slot);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_StopRecord(USER_HANDLE lUserID,int slot);

//报警回调设置
//直接用userid，防止一个设备2次登录无法区分，之前的接口直接去的
//DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetAlarmCallback(void (CALLBACK* func)(const char *ip,int type,char *buf,int len,long nUser),long nUser);
//报警回调返回userid

/*
*  @date         2015/11/18 
*  @brief        消息(报警)回调           : 
*  @param[in]    lUserID                  :   
*  @param[in]    func                     :   
*  @param[in]    nUser                    : 在调用层，指向必须是唯一对象，如DVO_NET_SET_SetAlarmCallbackEx多次调用，总是保存最后一个，在使用是需要注意
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetAlarmCallbackEx(void (CALLBACK* func)(USER_HANDLE lUserID,int type,char *buf,int len,long nUser),long nUser);

/*
*  @date         2015/11/18 
*  @brief        开启报警                 :
*  @param[in]    lUserID                  :   
*  @param[in]    bRegister                :  0 关闭，1开启,
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_RegisterAlarm(USER_HANDLE lUserID,BOOL bRegister); //开启报警bRegister 0 关闭，1开启,

/*
*  @date         2015/11/18 
*  @brief        音频输入码流控制(录像)设置 : 0x06 0x01
*  @param[in]    lUserID                  :   
*  @param[in]    pStreamType              : 频输入码流控制
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetStreamType(USER_HANDLE lUserID,dvonetSDK:: tStreamType *pStreamType);
/*
*  @date         2015/11/18  
*  @brief        音频输入码流控制(录像)查询 : 0x06 0x02
*  @param[in]    lUserID                  :   
*  @param[in out]    pStreamType              : 频输入码流控制
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetStreamType(USER_HANDLE lUserID, dvonetSDK:: tStreamType *pStreamType);

/*
*  @date         2015/11/17  
*  @brief        时间查询             : 0x01 0x06
*  @param[in]    lUserID              :   
*  @param[in]    sysTime              : 时间
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_SET_GetSystime(USER_HANDLE lUserID, SYSTEMTIME* sysTime);
/*
*  @date         2015/11/17  
*  @brief        时间设置             : 0x01 0x05
*  @param[in]    lUserID              :   
*  @param[in]    sysTime              : 时间
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetSystime(USER_HANDLE lUserID, SYSTEMTIME* sysTime);
/*
*  @date         2015/11/17  
*  @brief        网络设置             : 0x01 0x04
*  @param[in]    lUserID              :   
*  @param[in out]networkSetInfo       : IO网络设置
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetNetworkSetting(USER_HANDLE lUserID, dvonetSDK::tNetworkSetInfo* networkSetInfo);
/*
*  @date         2015/11/17  
*  @brief        网络设置             :0x01 0x03
*  @param[in]    lUserID              :   
*  @param[in]    networkSetInfo       : 网络设置
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_SET_SetNetworkSetting(USER_HANDLE lUserID, dvonetSDK::tNetworkSetInfo* networkSetInfo);
/**********************************************************/


//================= NVR 暂时预留该接口 ==========================//
//获取连接ipcam通道的通道设置信息
/*
*  @date        2015/11/17   老版本中没有相应代码，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetChannelSet(USER_HANDLE lUserID,struct tChannelSet *pChanSet);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetChannelSet(USER_HANDLE lUserID,struct tChannelSet *pChanSet);
//获取通道连接ipcam的状态，0-未连接或未开启，1-正常连接，参数*pChanStatus是len为dvr通道数的byte数组
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetChannelStatus(USER_HANDLE lUserID,struct tChannelStatus* pChanStatus);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetChannelType(USER_HANDLE lUserID,struct tChannelType *pChanType);
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetSubChannelSet(USER_HANDLE lUserID,dvonetSDK::tSubChannelSet* pSubChannelSet);
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetSubChannelSet(USER_HANDLE lUserID,dvonetSDK::tSubChannelSet* pSubChannelSet);

//=================head data======================//
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_GetVideoHead(REAL_HANDLE lRealID,char* head,int len,int* head_len); //获取保存文件的头 40个字节
/*
*  @date        2015/11/17   老版本中没有相应代码，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_PLAY_GetVideoHead(REC_HANDLE lPlayID,char* head,int len,int* head_len);//获取播放器的视频头
/*
*  @date        2015/11/17   老版本中没有相应代码，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetDownloadHead(DOWNLOAD_HANDLE lDownloadID,char* head,int len,int* head_len);//设置、获取下载文件的视频头

//===============================================//
//手动开启下载数据的捕获，必须调用该接口才会有数据
/*
*  @date        2015/11/17   老版本中没有相应代码，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_StartDownloadCapture(long lDownloadID);	//下载数据保留该接口
//================报警输出(继电器)状态===========//
/*
*  @date         2015/11/17  
*  @brief        IO报警输出查询       :0x05 0x02
*  @param[in]    lUserID              :   
*  @param[out]   pAlarmOutState       : IO报警输出
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetAlarmOutState(USER_HANDLE lUserID,dvonetSDK:: tAlarmOutState* pAlarmOutState);

//lAlarmOutPort:0xff-所有继电器  其他-指定继电器
//AlarmOutState: 1-有效 0- 无效
/*
*  @date         2015/11/17  
*  @brief        IO报警输出设置       :0x05 0x01
*  @param[in]    lUserID              :   
*  @param[out]   AlarmOutState        : IO报警输出
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetAlarmOutState(USER_HANDLE lUserID, /*LONG lAlarmOutPort,LONG*/ dvonetSDK:: tAlarmOutState* AlarmOutState);

//================报警输入设置===================//
/*
*  @date        2015/11/17   老版本中没有相应代码，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetAlarmInConfig(USER_HANDLE lUserID,dvonetSDK::tAlarmInCfg* pAlarmInConfig);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetAlarmInConfig(USER_HANDLE lUserID,dvonetSDK::tAlarmInCfg* pAlarmInConfig);

//================手动布撤防=====================//
//lAlarmInPort: 报警输入号
//DefendType: 0- 定时  1-布防  2-撤防
/*
*  @date        2015/11/17   老版本中没有相应代码，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetAlarmDefend(USER_HANDLE lUserID,LONG lAlarmInPort,LONG* DefendType);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetAlarmDefend(USER_HANDLE lUserID,LONG lAlarmInPort,LONG DefendType);

//================RS232设置======================//
/*
*  @date        2015/11/17   老版本中没有相应代码，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetRS232Config(USER_HANDLE lUserID,dvonetSDK::tRs232Cfg* pRS232Config);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetRS232Config(USER_HANDLE lUserID,dvonetSDK::tRs232Cfg* pRS232Config);

//================透明通道=======================//
//DVOIPCNETSDK_API dvonetSDK::SERIAL_HANDLE CALLMETHOD   DVO_NET_SET_StartRS232Send(USER_HANDLE lUserID,int Rs232No);
/*
*  @date         2015/11/17     
*  @brief        透传发送             : 0x04 0x02 
*  @param[in]    lUserID              :   
*  @param[out]   pRS232Send           : 透传数据
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SendRS232Data(dvonetSDK::SERIAL_HANDLE lSerialID,dvonetSDK::tRs232Send* pRS232Send);
//DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_StopRS232Send(dvonetSDK::SERIAL_HANDLE lSerialID);

//================PTZ RS232设置=================//
/*
*  @date         2015/11/17  
*  @brief        串口查询             :0x01 0x0c
*  @param[in]    lUserID              :   
*  @param[out]   pPtzRSConfig         :串口参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetPtzRSConfig(USER_HANDLE lUserID,dvonetSDK::tPtzRs232Cfg * pPtzRSConfig);
/*
*  @date         2015/11/17  
*  @brief        串口设置             :0x01 0x0b
*  @param[in]    lUserID              :   
*  @param[out]   pPtzRSConfig         :串口参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetPtzRSConfig(USER_HANDLE lUserID,dvonetSDK::tPtzRs232Cfg * pPtzRSConfig);


//================视频丢失报警设置==============//
//=================目前底层未实现接口暂时保留====//
/*
*  @date        2015/11/17   老版本中没有相应代码，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetVideoLostConfig(USER_HANDLE lUserID,dvonetSDK::tVideoLostCfg* pVideoLostConfig);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetVideoLostConfig(USER_HANDLE lUserID,dvonetSDK::tVideoLostCfg* pVideoLostConfig);

//================语音对讲======================后期实现//
//VoiceComType: 0-客户端到中心 1-双向
/*
*  @date        2015/11/17   老版本中没有相应代码，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API dvonetSDK::VOICE_HANDLE CALLMETHOD DVO_NET_SET_StartVoiceCom(USER_HANDLE lUserID,int VoiceComType);
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_StopVoiceCom(dvonetSDK::VOICE_HANDLE lVoiceComHandle);
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_RegisterVoiceComCallBack(dvonetSDK::VOICE_HANDLE lVoiceComHandle,void(CALLBACK* fun)(long lVoiceComHandle,char *buf,int len,int flag,long dwUser),long dwUser);
  

//================移动侦测扩展=================//
/*
*  @date         2015/11/17  
*  @brief        MD区域查询          :0x05 0x04
*  @param[in]    lUserID             :   
*  @param[out]   pMotionEx           :MD区域
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetMotionRowCols(USER_HANDLE lUserID,dvonetSDK::tMotionRowCols* pMotionRowCols);
/*
*  @date         2015/11/17  
*  @brief        MD区域设置          :0x05 0x03
*  @param[in]    lUserID             :   
*  @param[out]   pMotionRowCols      :MD区域
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetMotionRowCols(USER_HANDLE lUserID,dvonetSDK::tMotionRowCols* pMotionRowCols);
/*
*  @date         2015/11/17  
*  @brief        MD参数查询          :0x05 0x06
*  @param[in]    lUserID             :   
*  @param[out]   pMotionEx           :MD参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetMotionEx(USER_HANDLE lUserID,dvonetSDK::tMotionEx* pMotionEx);
/*
*  @date         2015/11/17  
*  @brief        MD参数设置          :0x05 0x05
*  @param[in]    lUserID             :   
*  @param[out]   pMotionEx           :MD参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetMotionEx(USER_HANDLE lUserID,dvonetSDK::tMotionEx* pMotionEx);
/*
*  @date        2015/11/17   老版本中没有相应代码，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetSmartSearchFileEx(USER_HANDLE lUserID, dvonetSDK::tSmartSearchEx *pSearchTime, tRecFile *pSaveSSFile, long maxSSFileNum, long* rtnNum);
/*
*  @date        2015/11/17   老版本中没有相应代码，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API long CALLMETHOD DVO_NET_SET_FindSmartSearchFileEx(USER_HANDLE lUserID,dvonetSDK::tSmartSearchEx *pSearch);


//================遮挡报警=====================//
/*
*  @date        2015/11/17   对应协议不明，暂未实现
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetAlarmMask(USER_HANDLE lUserID,dvonetSDK::tAlarmMask* pAlarmMask);
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetAlarmMask(USER_HANDLE lUserID,dvonetSDK::tAlarmMask* pAlarmMask);


//=================硬盘信息和格式化============//
/*
*  @date         2015/11/17  
*  @brief        磁盘信息查询        :0x01 0x11
*  @param[in]    lUserID             :   
*  @param[out]   pHarddiskInfo       :磁盘信息
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetHarddiskState(USER_HANDLE lUserID,dvonetSDK::tHarddiskInfo* pHarddiskInfo);

/*
*  @date         2015/11/17   
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_FormatHarddisk(USER_HANDLE lUserID,int disk_idx); 
/*
*  @date         2015/11/17   
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetFormatProgress(USER_HANDLE lUserID,int* progress);//0-100


//加密芯片匹配
/*
*  @date         2015/11/17  
*  @brief        加密芯片匹配        :0x07 0x01
*  @param[in]    lUserID             :   
*  @param[out]   result              :result 0 为匹配，其他为匹配
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_Get_MatchChip(USER_HANDLE lUserID,int *result);//result 0 为匹配，其他为匹配

//wifi 
/*
*  @date         2015/11/17  
*  @brief        WIFI获取链接状态命令:0x07 0x02
*  @param[in]    lUserID             :   
*  @param[out]    result              :result 0 wifi链接上，其他未连上
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_Get_WIFI(USER_HANDLE lUserID,int *result); 

//wifi set 
/*
*  @date         2015/11/17  
*  @brief        WIFI命令设置        :0x07 0x02
*  @param[in]    lUserID             :   
*  @param[in]    pInfo               :WIFI命令
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_Set_WIFI(USER_HANDLE lUserID,dvonetSDK::tWifiInfo *pInfo); 

//启动音频输出端口
/*
*  @date         2015/11/17  
*  @brief        启动音频输出端口    :0x07 0x04
*  @param[in]    lUserID             :   
*  @param[in out]pInfo               :音频输出
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SETAVout(USER_HANDLE lUserID,dvonetSDK::tAouInfo *pInfo); 

//set audio encode methord
/*
*  @date         2015/11/17  有疑问，暂未实现
*  @brief        音频输出码流控制（对讲）设置      :0x06 0x03
*  @param[in]    lUserID             :   
*  @param[in out]pInfo               :音频输出码流控制
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetAudioEnc(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_audio_config_t *pInfo);

//set video flep 
/*
*  @date         2015/11/17
*  @brief        视频模式设置        :0x03 0x09
*  @param[in]    lUserID             :   
*  @param[in out]pInfo               :视频模式
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetFlep(USER_HANDLE lUserID,dvonetSDK::tImgeFelp *pInfo);

/*
*  @date         2015/11/17
*  @brief        视频模式查询        :0x03 0x0a
*  @param[in]    lUserID             :   
*  @param[in out]pInfo               :视频模式
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetFlep(USER_HANDLE lUserID,dvonetSDK::tImgeFelp *pInfo);

//设置环境接口
/*
*  @date         2015/11/17   
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_Env(USER_HANDLE lUserID,dvonetSDK::tEvnParam *pInfo);

//Get环境接口
/*
*  @date         2015/11/17   
*  @brief       暂未实现    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_Env(USER_HANDLE lUserID,dvonetSDK::tEvnParam *pInfo);

//获取osd区域
/*
*  @date         2015/11/17
*  @brief        自定义OSD区域参数查询 :0x02 0x09
*  @param[in]    lUserID             :   
*  @param[in out]osd_area            :OSD区域参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_CustomOSD(USER_HANDLE lUserID,dvonetSDK::tCustomAreaOSD *osd_area);

//设置osd区域
/*
*  @date         2015/11/17
*  @brief        自定义OSD区域参数设置 :0x02 0x08
*  @param[in]    lUserID             :   
*  @param[in out]osd_area            :OSD区域参数
*  @return       true,成功，false,失败
*  @remarks      有疑问，指令类型与视频遮盖信息冲突
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_CustomOSD(USER_HANDLE lUserID,dvonetSDK::tCustomAreaOSD *osd_area);

//获取osd参数设置.
/*
*  @date         2015/11/17
*  @brief        自定义OSD行参数查询 :0x02 0x0b
*  @param[in]    lUserID             :   
*  @param[in out]osdrow              :OSD行参数
*  @return       true,成功，false,失败

*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_CustomOSDRow(USER_HANDLE lUserID,dvonetSDK::tCustomRow *osdrow);

//设置行的参数(包括钩边,设置颜色,目前只支持一种默认红色,修改其他颜色需要IPC底层升级)b
/*
*  @date         2015/11/17
*  @brief        自定义OSD行参数设置 :0x02 0x0a
*  @param[in]    lUserID             :   
*  @param[in]    osdrow              :OSD行参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_CustomOSDRow(USER_HANDLE lUserID,dvonetSDK::tCustomRow *osdrow);

//获取osd行文本信息
/*
*  @date         2015/11/17
*  @brief        自定义OSD行数据查询 :0x02 0x0d
*  @param[in]    lUserID             :   
*  @param[in out]osdrowtext          :OSD行数据
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_CustomOSDRowText(USER_HANDLE lUserID,dvonetSDK::tCustomRow_Text *osdrowtext);
//设置行的文本
/*
*  @date         2015/11/17
*  @brief        自定义OSD行数据设置 :0x02 0x0c
*  @param[in]    lUserID             :   
*  @param[in]    osdrowtext          :OSD行数据
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_CustomOSDRowText(USER_HANDLE lUserID,dvonetSDK::tCustomRow_Text  *osdrowtext);

//获取osd行文本信息 ---->包含行颜色
/*
*  @date         2015/11/17
*  @brief        自定义OSD行数据查询 :0x02 0x0d
*  @param[in]    lUserID             :   
*  @param[in out]osdrowtext          :SD行数据
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_CustomOSDRowText2(USER_HANDLE lUserID,dvonetSDK::tCustomRow_Text2 *osdrowtext);

//设置行的文本=====>包含行颜色
/*
*  @date         2015/11/17
*  @brief        自定义OSD行数据设置 :0x02 0x0c
*  @param[in]    lUserID             :   
*  @param[in]    osdrowtext          :SD行数据
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_CustomOSDRowText2(USER_HANDLE lUserID,dvonetSDK::tCustomRow_Text2  *osdrowtext);
/////////////////////////////////////////////////////////////////温控与湿度相关接口 /////////////////////////////////////////////////

//设置温控报警输出--->根据设备类型设置参数,如果是消防泵 data_type 的含义为对应设备的参数指标 上层应用根据DVO_NET_GET_DEVICE_FUNCTION_LIST来判断设备功能
/*
*  @date         2015/11/17
*  @brief        输入报警输出设置    :0x05 0x07
*  @param[in]    lUserID             :   
*  @param[in]    pParam              :报警输出
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_TEMPR_CTRL_AlarmOut(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_event_act_485_t *pState);  


//获取温控报警输出
/*
*  @date         2015/11/17
*  @brief        输入报警输出查询    :0x05 0x08
*  @param[in]    lUserID             :   
*  @param[in out]pParam             :报警输出
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_TEMPR_CTRL_AlarmOut(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_event_act_485_t *pState);

//设置温控报警参数
/*
*  @date         2015/11/17
*  @brief        输入报警参数设置    :0x05 0x09
*  @param[in]    lUserID             :   
*  @param[in]    pParam              :报警参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_TEMPR_CTRL_AlarmParam(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_alarm_para_t *pParam); 

//获取温控报警参数 
/*
*  @date         2015/11/17
*  @brief        输入报警参数获取    :0x05 0x0a
*  @param[in]    lUserID             :   
*  @param[out]   pParam              :报警参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_TEMPR_CTRL_AlarmParam(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_alarm_para_t *pParam); 


//设置温控报警设备配置
/*
*  @date         2015/11/17
*  @brief        设备信息获取        :0x07 0x05
*  @param[in]    lUserID             :   
*  @param[in]    pDevconfig          :设备信息
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_TEMPR_CTRL_DeviceConfig(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_dev_t *pDevconfig); 

//获取温控报警设备配置
/*
*  @date         2015/11/17
*  @brief        设备信息获取        :0x07 0x06
*  @param[in]    lUserID             :   
*  @param[out]   pDevconfig          :设备信息
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_TEMPR_CTRL_DeviceConfig(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_dev_t *pDevconfig); 


//设置温控OSD信息
/*
*  @date         2015/11/17
*  @brief        输入数据OSD参数设置 :  0x07 0x07
*  @param[in]    lUserID             :   
*  @param[in]    pOsddata            : OSD参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_TEMPR_CTRL_OSDData(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_osd_para_t *pOsddata); 

//获取温控OSD信息--->消防泵 设置对应的osd开关
/*
*  @date         2015/11/17
*  @brief        输入数据OSD参数查询 :  0x07 0x08
*  @param[in]    lUserID             :   
*  @param[out]   pOsddata            : OSD参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_TEMPR_CTRL_OSDData(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_osd_para_t *pOsddata); 

/*
*  @date         2015/11/17
*  @brief        获取温控信息        :  0x07 0x09
*  @param[in]    lUserID             :   
*  @param[out]   pData               :  温控数据
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_TEMPR_CTRL_485_inputData(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_input_data_t *pData);


//NTP校时设置
/*
*  @date         2015/11/17
*  @brief        设置NTP参数         :  0x01 0x19
*  @param[in]    lUserID             :   
*  @param[in]   pNtpInfo            :  NTP参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_NTP(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_sys_ntp_para_t *pNtpInfo);

/*
*  @date         2015/11/17
*  @brief        NTP参数获取         :  0x01 0x1a
*  @param[in]    lUserID             :   
*  @param[out]   pNtpInfo            :  NTP参数
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_NTP(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_sys_ntp_para_t *pNtpInfo);

/*
*  @date         2015/11/17
*  @brief        开始NTP功能测试     :0x01 0x1d
*  @param[in]    lUserID             :   
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_Check_NTP(USER_HANDLE lUserID);

///////////////////////////////////////////////温控与湿度相关接口///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////获取设备的功能列表////////////////////////////////////////////////////////////

 
/*
*  @date         2015/11/16
*  @brief        获取设备的功能列表  :0x01 0x1b
*  @param[in]    lUserID             :   
*  @param[out]   pListInfo           : 设备的功能
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_DEVICE_FUNCTION_LIST(USER_HANDLE lUserID,/*dvonetSDK::list_*/ dvonetSDK::app_net_tcp_sys_func_list_t *pListInfo);
 
/*
*  @date         2015/11/16
*  @brief        查询IO数量       :0x01 0x1e
*  @param[in]    lUserID          :   
*  @param[out]   pio_num     : IO数量
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_GET_IO_NUM(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_io_alarm_num_t *pio_num);

/*
*  @date         2015/11/16
*  @brief        查询IO属性       :0x01 0x1f
*  @param[in]    lUserID          :   
*  @param[out]   pio_property     : IO属性
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_GET_IO_Property(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_io_alarm_attr_t *pio_property);
/*
*  @date         2015/11/16
*  @brief        设置IO属性       :0x01 0x20
*  @param[in]    lUserID          :   
*  @param[in]   pio_property      : IO属性
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_SET_IO_Property(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_io_alarm_attr_t *pio_property);

/*
*  @date         2015/11/16
*  @brief        查询IO报警状态   :0x01 0x22
*  @param[in]    lUserID          :   
*  @param[out]    pio_status       : IO报警状态
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_GET_IO_IsEnable(USER_HANDLE lUserID, dvonetSDK::app_net_tcp_set_io_alarm_state_t *pio_status);
 
/*
*  @date         2015/11/16
*  @brief        设置IO报警状态   :0x01 0x21
*  @param[in]    lUserID          :   
*  @param[in]    pio_status       : IO报警状态
*  @return       true,成功，false,失败
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_SET_IO_IsEnable(USER_HANDLE lUserID, dvonetSDK::app_net_tcp_set_io_alarm_state_t *pio_status);
/////////////////////////////////////////////////////////////////////消防泵/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////消防泵///////////////////////////////////////////////////////////////


//设置 MAC 和SN 序列号 
//MAC地址，字符串，字符的取值范围为：'0'-'9','A'-'F','a'-'f',':'，每两个字符表示一个8位二进制数的十六进制格式，用“:”分隔，例如：E2:02:6E:AD:17:19，
// MAC 不多于39个字符串 SN:不多于31个字符串
//ＳＮ默认为空　ｐｏｒｔ　为６００２
 
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_MAC_SN(char * ip,int port,char *sn,char *mac); //

DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_MAC_SN(char * ip,int port,char *sn,char *mac);
//DVOIPCNETSDK_API double CALLMETHOD GetHighPerformanceTime();
///////////////////接口end///////////////////////////////////////////////////////

 
/////////////////////////////////////////////新版本接口 end///////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif

