/*
*  COPYRIGHT NOTICE 
*  Copyright (c) 2015, Diveo Technology Co.,Ltd (www.diveo.com.cn)
*  All rights reserved.
*
*  @file        dvoipcnetsdk1.h
*  @author      jag(luo_ws@diveo.com.cn)
*  @date        2015/11/10 13:42
*
*  @brief       dvoipcnetsdk1ͷ�ļ�
*  @note        ����ӿڣ�����ǰ�汾���ּ���
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
///////////////////�ӿ�begin///////////////////////////////////////////////////////

/*
*  @date        2015/11/11 
*  @brief       ����SDK ��ʼ�� 
*  @param[in]   localport,�°汾�У�����Ч
*  @return      true���ɹ���fasle��ʧ��    
*  @remarks     ��DVO2_NET_Init��ͬ       
*/
DVOIPCNETSDK_API BOOL __stdcall	DVO_NET_Init(long localPort=0);
 
/*
*  @date        2015/08/23 
*  @brief       �ͷ�SDK������ص���Դ
*  @return      true���ɹ���fasle��ʧ��    
*  @remarks     ��DVO2_NET_Release��ͬ
*/
DVOIPCNETSDK_API BOOL __stdcall	DVO_NET_Release();  

/*
*  @date        2015/11/11
*  @brief       ��¼�豸                : 0x01 0x01
*  @param[in]   sIP                     :
*  @param[in]   dwPort                  :
*  @param[in]   sUser                   :
*  @param[in]   sPassword               :
*  @return      USER_HANDLE,ʧ�ܷ���DVO_INVALID_HANDLE -1���ɹ������豸ID             
*  @remarks     ����DVO2_NET_Login
*/
DVOIPCNETSDK_API USER_HANDLE CALLMETHOD	DVO_NET_Login(const char *sIP,int dwPort,const char *sUser,const char*sPassword);

/*
*  @date        2015/11/11
*  @brief       ��¼�豸                : 0x01 0x02
*  @param[in]   lUserID                 :
*  @return      true���ɹ���fasle��ʧ��    
*  @remarks     ����DVO2_NET_Logout
*/
DVOIPCNETSDK_API BOOL __stdcall	DVO_NET_Logout(USER_HANDLE lUserID); 


//��ȡ��ǰ�û�ʹ�õĴ����������ø��ӽ�sdk��DVOSDKÿ��ȡһ�����ڽ����ڲ������� -1 error get channel sum
/*
*  @date        2015/11/11
*  @brief       ��ѯ��ǰ�û�ʹ�õĴ�����
*  @param[in]   lUserID                  :
*  @return      ��ǰ�û�ʹ�õĴ�����      
*/
DVOIPCNETSDK_API long CALLMETHOD  DVO_NET_GetWindowCount(USER_HANDLE lUserID);

/*
*  @date        2015/11/11
*  @brief       ��ȡ����SDK�İ汾
*  @param[out]  pMainVersion:   ���汾��
*  @param[out]  pSubVersion :   �Ӱ汾��
*  @return      true���ɹ���fasle��ʧ��    
*  @remarks     ֱ�ӵ����°汾DVO_NET_GetSDKVersion
*/
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_GetNetVersion(long *pMainVersion,
                                                        long *pSubVersion);

/*
*  @date        2015/11/11
*  @brief       ��ȡ�������
*  @return      ���ش������    
*  @remarks     ����DVO2_NET_GetLastError
*/
DVOIPCNETSDK_API long __stdcall  DVO_NET_GetLastError();   

/*
*  @date         2015/11/23
*  @brief        �����豸            : 0x01 0x07    
*  @param[in]    lUserID    
*  @remarks      ����DVO2_NET_Reboot
*/
DVOIPCNETSDK_API BOOL __stdcall   DVO_NET_Reboot(USER_HANDLE lUserID); 
 
/*
*  @date         2015/11/23
*  @brief        �ر����             : 0x01 0x08    
*  @param[in]    lUserID    
*  @remarks      ����DVO2_NET_ShutDown
*/
DVOIPCNETSDK_API BOOL __stdcall DVO_NET_ShutDown(USER_HANDLE lUserID);

/*
*  @date         2015/11/23
*  @brief        �ָ���������         : 0x01 0x09    
*  @param[in]    lUserID              : 
*  @param[in]    type                 : type 0 Ϊ������ip�ָ���1Ϊ����ip�ָ�Ĭ�ϳ���ֵ��
*  @remarks      ����DVO2_NET_Restore
*/
DVOIPCNETSDK_API BOOL __stdcall  DVO_NET_Restore(USER_HANDLE lUserID,int type);

/*
*  @date         2015/11/23
*  @brief        �����������糬ʱ     : 
*  @param[in]    timeout_millisec     : 
*  @remarks      ������ʱ����ʱ����ͬ
*/
DVOIPCNETSDK_API void __stdcall DVO_NET_SetTimeout(long timeout_millisec);



/*
*  @date        2015/11/11
*  @brief       ��ʼ��Ƶ������DVO_NET_StartRealPlay
*  @param[in]   lUserID     : DVO_NET_Login�ɹ��ķ���ֵ
*  @param[in]   slot        : �豸ͨ���ţ���0��ʼ
*  @param[in]   issub       : ����ID����0��ʼ��0Ϊ��������1Ϊ��������2Ϊ��������
*  @param[in]   connet_mode : ͨ��Э�����ͣ�0--TCP ;1--UDP(UDP�ݲ�֧��)
*  @param[in]   funcRealData: �ص�����,���ڻص���Ƶ���ݵ��ϲ�Ӧ��,���嶨��ο�fnDVOCallback_RealAVData_T
*  @param[in]   encodetype  : �������ͣ�0--H264��1--JPEG��2--265�� Ŀǰipcֻ֧��2���Ժ������չ
*  @param[in]   hWnd        : Ϊ������ʾԤ���Ĵ��ھ��,�ð汾��NULL,ֻ��ȡ����    
*  @param[in]   pUser       : �û�����,Ϊ�ϲ㴫���ʶ������Ƶ�ص������з��ظ��ϲ�
*  @param[out]  pErrNo      : ���ش�����,���������سɹ�ʱ,�ò�����ֵ������.
*  @return      REAL_HANDLE :ʧ�ܷ���DVO_INVALID_HANDLE -1���ɹ�������Ƶ�ỰID 
*  @remarks     �ýӿ�������ö�� ,ÿһ�ο���һ·��Ƶ���� ��֧��ipc���ڶ��ͨ��ʱ���õ������  
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
*  @brief       �ر���Ƶ,,����DVO_NET_StopRealPlay;
*  @param[in]   lStreamHandle, DVO_NET_OpenAVStreamEx2/DVO_NET_StartRealPlay�ɹ��ķ���ֵ
*  @param[in]   hWnd,����
*  @return      true���ɹ���fasle��ʧ��    
*  @remarks     ��DVO_NET_OpenAVStreamEx2/DVO_NET_StartRealPlay��Ӧ.     
*/
DVOIPCNETSDK_API BOOL	CALLMETHOD	DVO_NET_CloseAVStreamEx2(REAL_HANDLE lRealID,
                                                             HWND hWnd);

/*
*  @date        2015/11/11
*  @brief       ����ƵԤ��/�ر���ƵԤ�� , �°汾�иýӿڲ���ʵ��        
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
*  @brief       ��/�ر�����            : 0x06 0x01
*  @param[in]   lRealID                  :
*  @return      true���ɹ���fasle��ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_OpenSound(REAL_HANDLE lRealID); 	
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_CloseSound(REAL_HANDLE lRealID); //

/*
*  @date        2015/11/11
*  @brief       ��Ƶ��ɫ��ѯ            : 0x03,0x02
*  @param[in]   lRealID                 :
*  @param[out]  pVideoCOlor             : ��Ƶ����ɫ����  
*  @return      true�ɹ�,falseʧ��
*/	
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GetVideoColor(REAL_HANDLE lRealID,
                                                       dvonetSDK::tVideoColor *pVideoCOlor);

/*
*  @date        2015/11/11
*  @brief       ��Ƶ��ɫ����            : 0x03,0x01
*  @param[in]   lRealID                 : 
*  @param[in]   pVideoColor             : ��Ƶ����ɫ����  
*  @return      true�ɹ�,falseʧ��
*/	
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SetVideoColor(REAL_HANDLE lRealID,
                                                          dvonetSDK:: tVideoColor *pVideoColor);
/*
*  @date        2015/11/11
*  @brief       ��ȡ��Ƶ״̬,DVR��ʹ�ýӿ�,�ݲ�ʵ��
*/		 
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_GetVideoStatus(REAL_HANDLE lRealID,
                                                           dvonetSDK:: tVideoStatus *pVideoStatus);

/*
*  @date        2015/11/11
*  @brief       ��������ʾ����,�ݲ�ʵ��    
*/		 
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_SET_PLAYER_SCALE(REAL_HANDLE lRealID,
                                                       const char * scale);

/*
*  @date        2015/11/11
*  @brief       ptz ����,a�ݲ�ʵ�� 
*/		
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_ControlPtz(REAL_HANDLE lRealID,
                                                       dvonetSDK:: tPtzControl *pPtzControl);

/*
*  @date        2015/11/11
*  @brief      snap ����bmp,a�ݲ�ʵ��    
*/			
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_Snap(REAL_HANDLE lRealID,
                                                 const char * szFile);

/*
*  @date        2015/11/11
*  @brief     /snap ����jpg,a�ݲ�ʵ��     
*/		
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_SnapJpg(REAL_HANDLE lRealID,
                                                  const char*szFile);

/*   +---    
*  @date     2015/11/11
*  @brief    ����Ԥ��¼�� /ֹͣԤ��¼,�Ѳ���   
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_StartRecord(REAL_HANDLE lRealID,
                                                        const char *szFile);	
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_StopRecord(REAL_HANDLE lRealID);

/*rec video*/ //�ò������ݱ����Ժ�ʵ��
/*
*  @date      2015/11/11
*  @brief     �ݲ�ʵ�֡�    
*/
///�����粥����Ƶ¼��
DVOIPCNETSDK_API REC_HANDLE CALLMETHOD	DVO_NET_PLAY_OpenVideo(USER_HANDLE lUserID,
                                                               struct tRecFile *pRecFile,
                                                               HWND hWnd,
                                                               int nPlaySlot); 

DVOIPCNETSDK_API REC_HANDLE CALLMETHOD	DVO_NET_PLAY_OpenVideoEx(USER_HANDLE lUserID,
                                                                 struct tRecFile *pRecFile,
                                                                 void(CALLBACK*func)(long lPlayID,char*buf,int len,int videoType,long nUser),
                                                                 long nUser);
//�ر�������Ƶ¼��
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_PLAY_CloseVideo(REC_HANDLE lPlayID); 

//================������========================//
//DVOIPCNETSDK_API REAL_HANDLE  CALLMETHOD	DVO_NET_OpenSubVideo(USER_HANDLE lUserID,int slot,HWND hWnd,int nPlaySlot);
//DVOIPCNETSDK_API REAL_HANDLE  CALLMETHOD	DVO_NET_OpenSubVideoEx(USER_HANDLE lUserID,int slot,void(CALLBACK*func)(long lVideoID,char*buf,int len,int videoType,long nUser),long nUser);
//DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetSubChannelSet(USER_HANDLE lUserID,tSubChannelSet* pSubChannelSet);
//DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetSubChannelSet(USER_HANDLE lUserID,tSubChannelSet* pSubChannelSet);

//================= IPCAM ========================�ò�����Ҫ���֧�֣���ʱ����//


/*
*  @date         2015/11/11
*  @brief        ��ȡ������Բ�ѯ            : 0x03 0x04;0x03 0x06;0x03 0x08
*  @param[in]    lRealID                     :
*  @param[in out]pFeature                    : �������
*  @return       true�ɹ���falseʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_IPCam_GetFeature(USER_HANDLE lUserID,dvonetSDK::tIPCam_feature *pFeature);

/*
*  @date         2015/11/11
*  @brief        �����������               : 0x03 0x03;0x03 0x05;0x03 0x07
*  @param[in]    lRealID                    :
*  @param[in]    pFeature                   : �������
*  @return       true�ɹ���falseʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_IPCam_SetFeature(USER_HANDLE lUserID,dvonetSDK::tIPCam_feature *pFeature);

/*
*  @date         2015/11/12                 : �����ʣ��ݲ�ʵ��
*  @brief        ��������ͺ�����           : 0x01 0x23
*  @param[in]    lRealID                    :
*  @param[in]    pModeID                    : �������
*  @return       true�ɹ���falseʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD NET_SET_Func_Mode(USER_HANDLE lUserID,dvonetSDK::IPCModeType *pModeID);

/*
*  @date         2015/11/12                 : �����ʣ��ݲ�ʵ��
*  @brief        ��������ͺŲ�ѯ           : 0x01 0x24
*  @param[in]    lUserID                    :
*  @param[out]    pModeID                    :�������
*  @return       true�ɹ���falseʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD NET_GET_Func_Mode(USER_HANDLE lUserID,dvonetSDK::IPCModeType *pModeID);

/* 
*  @date         2015/11/12                 : �ݲ�ʵ��
*  @brief        ǿ�����ƣ��ϰ汾��δʵ�֣�Э����û�з����������
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_IPCAM_GetBlackMaskBLC(USER_HANDLE lUserID,dvonetSDK::tBlackMaskBLC *pBMB);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_IPCAM_SetBlackMaskBLC(USER_HANDLE lUserID,dvonetSDK::tBlackMaskBLC *pBMB);

/* 
*  @date         2015/11/12                  : �ݲ�ʵ��
*  @brief        ����Ǩ�� ���ϰ汾��δʵ�֣�Э����û�з����������
*/ 
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_IPCAM_GetLostDataTransfer(USER_HANDLE lUserID,struct tLostDataTransfer* pLostDataTransfer);
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_SET_IPCAM_SetLostDataTransfer(USER_HANDLE lUserID,struct tLostDataTransfer* pLostDataTransfer);

/* 
*  @date        2015/11/12                  : �ݲ�ʵ��
*  @brief       �Զ��۽� ,�ϰ汾��δʵ�֣�Э����û�з���������ݣ�
*/ 
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_IPCAM_AutoFocusStart(USER_HANDLE lUserID,struct tAutoFocus* pAutoFocus);

/*
*  @date        2015/11/12                  : �ݲ�ʵ��
*  @brief       ����֮ǰ�ӿ�
*/  
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetCenterInfo(USER_HANDLE lUserID,dvonetSDK::tCenter* pCenterInfo);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetCenterInfo(USER_HANDLE lUserID,dvonetSDK::tCenter* pCenterInfo);

/* 
*  @date        2015/11/12                  : �ݲ�ʵ��
*  @brief       ǿ��i֡
*/ 
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_ForceIFrame(USER_HANDLE lUserID,int slot);

/*  
*  @date        2015/11/12                  : �ݲ�ʵ��
*  @brief       ��ȡ����״̬ dvr�нӿ�Ԥ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetAlarmState(USER_HANDLE lUserID,dvonetSDK::tAlarmState* pAlarmState);


/*
*  @date         2015/11/12
*  @brief        �豸��Ϣ/��Դ��ѯ          : 0x01 0x0f,0x01 0d
*  @param[in]    lUserID:   
*  @param[out]   pDevConfig                 : �豸/��Դ����Ϣ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetDevConfig(USER_HANDLE lUserID,
                                                             dvonetSDK::tDevConfig* pDevConfig);

/*
*  @date         2015/11/12
*  @brief        �豸��Ϣ����                : 0x01 0x10
*  @param[in]    lUserID                     :   
*  @param[in]    pDevConfig                  : �豸��Ϣ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetDevConfig(USER_HANDLE lUserID,
                                                             dvonetSDK::tDevConfig* pDevConfig);

/*
*  @date         2015/11/12
*  @brief        �����ҹģʽ��ѯ            : 0x03 0x08       
*  @param[in]    lUserID                     :   
*  @param[out]   gray_mode                   : �����ҹģʽ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetGrayMode(USER_HANDLE lUserID,
                                                            dvonetSDK::tgray_whitevmode* gray_mode);

/*
*  @date         2015/11/12
*  @brief        �����ҹģʽ����            : 0x03 0x07
*  @param[in]    lUserID                     :   
*  @param[int]   gray_mode                   : �����ҹģʽ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetGrayMode(USER_HANDLE lUserID,
                                                            dvonetSDK::tgray_whitevmode* gray_mode);

/*
*  @date         2015/11/12
*  @brief        ͼ���ع��������            : 0x03 0x03
*  @param[in]    lUserID                     :   
*  @param[in]    pbea                        : ͼ���ع����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_IPCam_Seteba(USER_HANDLE lUserID,
                                                          dvonetSDK::tIPCAM_BEA *pbea);

/*
*  @date         2015/11/12
*  @brief        ͼ���ع������ѯ            : 0x03 0x04
*  @param[in]    lUserID                     :    
*  @param[in]    pbea                        : ͼ���ع����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_IPCam_Geteba(USER_HANDLE lUserID,
                                                          dvonetSDK::tIPCAM_BEA *pbea);
 
/*
*  @date         2015/11/12
*  @brief        ͼ���ƽ���������            : 0x03 0x05
*  @param[in]    lUserID                       :    
*  @param[in]    pbla                          : ͼ���ƽ�����
*  @return      true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_IPCam_Setblankbanlace(USER_HANDLE lUserID,
                                                                   dvonetSDK::tIPCAM_BLANCEBLACK *pbla);

/*
*  @date         2015/11/12
*  @brief        ͼ���ƽ�������ѯ            : 0x03 0x06
*  @param[in]    lUserID                       :   
*  @param[out]   pbla                          : ͼ���ƽ�����
*  @return      true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_IPCam_Getblankbanlace(USER_HANDLE lUserID,
                                                                   dvonetSDK::tIPCAM_BLANCEBLACK *pbla);

/*
*  @date         2015/11/12
*  @brief        ��Ƶ����ӿ�����            : 0x01 0x17
*  @param[in]    lUserID                     :   
*  @param[in]    vout_mode                   : ��Ƶ����ӿ� 
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetVoutMode(USER_HANDLE lUserID,
                                                            dvonetSDK::tvout_interface* vout_mode);

/*
*  @date         2015/11/12
*  @brief        ��Ƶ����ӿڲ�ѯ            : 0x01 0x18
*  @param[in]    lUserID                     :   
*  @param[out]   vout_mode                   : ��Ƶ����ӿ�
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetVoutMode(USER_HANDLE lUserID,
                                                            dvonetSDK::tvout_interface* vout_mode);


//=================�ṩϵͳ����============================//

DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_UpgradeRequest(USER_HANDLE lUserID,int *ready);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SendUpgradeFile(USER_HANDLE lUserID,const char* fileName);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_StopSendUpgFile(USER_HANDLE lUserID);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetUpgradeState(USER_HANDLE lUserID,int *upgrade_state);

/*
*  @date         2015/11/12                 :�ݲ�ʵ�� ��Ҫʵ��
*  @brief        ��õ�¼��Ϣ               :
*  @param[in]    lUserID                    :   
*  @param[out]   inf                        :��¼��Ϣ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_RES_INFO(USER_HANDLE lUserID,
                                                  dvonetSDK::app_net_tcp_sys_logo_info_t *info);

/*remote set*/
/*
*  @date         2015/11/13
*  @brief        OSD������ѯ                :0x02 0x06
*  @param[in]    lUserID                    :   
*  @param[out]   pOsdName                   : OSD����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetChannelName(USER_HANDLE lUserID,
                                                               dvonetSDK:: tOsdChannel *pOsdName);

/*
*  @date         2015/11/13
*  @brief        OSD��������                :0x02 0x05
*  @param[in]    lUserID                    :   
*  @param[in]    pOsdName                   :OSD����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetChannelName(USER_HANDLE lUserID,
                                                            dvonetSDK::tOsdChannel *pOsdName);


//DVR�ӿ�Ԥ���ݲ�ʵ��
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetDvrVersion(USER_HANDLE lUserID,long *pVersion);
//DVR�ӿ�Ԥ���ݲ�ʵ��
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetDvrNetVersion(USER_HANDLE lUserID,long *pNetVersion);

/*
*  @date         2015/11/16
*  @brief        ����״̬   :��δʵ��
*  @param[in]    lUserID                :   
*  @param[in]    pIntervalMillSec       :
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetNetState(USER_HANDLE lUserID,long *pIntervalMillSec);

 
/*
*  @date         2015/11/16
*  @brief        �ͻ�ȡԶ����Ƶ��ɫ�ӿ�DVO_NET_GetVideoColor��ͬ��
*  @param[in]    lUserID                :   
*  @param[in]    pVideoColor            :
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetVideoColor(USER_HANDLE lUserID,
                                                              dvonetSDK::tVideoColor *pVideoColor);


/*
*  @date         2015/11/16
*  @brief        ��ȡԶ����Ƶ��ɫ,�ͽӿ�DVO_NET_SetVideoColor��ͬ��
*  @param[in]    lUserID                :   
*  @param[in]    pVideoColor            :
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetVideoColor(USER_HANDLE lUserID,
                                                              dvonetSDK::tVideoColor *pVideoColor);


/*
*  @date         2015/11/16
*  @brief        Զ����Ƶ����������ѯ   :0x02 0x02
*  @param[in]    lUserID                :   
*  @param[in]    pVideoQuality          :
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetVideoQuality(USER_HANDLE lUserID,
                                                                dvonetSDK::tVideoQuality *pVideoQuality);

/*
*  @date         2015/11/16
*  @brief        Զ����Ƶ������������   : 0x02 0x01
*  @param[in]    lUserID                :   
*  @param[in]    pVideoQuality          :
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetVideoQuality(USER_HANDLE lUserID,
                                                                dvonetSDK:: tVideoQuality *pVideoQuality);

/*
*  @date         2015/11/18             :
*  @brief        osd ʱ���ѯ(OSD����)  : 0x02 0x06
*  @param[in]    lUserID                :   
*  @param[in]    pOsdDate               : osd ʱ��(OSD����)
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetOsdDate(USER_HANDLE lUserID,dvonetSDK:: tOsdDate *pOsdDate);

/*
*  @date         2015/11/18               :
*  @brief        osd ʱ������(OSD����)    : 0x02 0x05
*  @param[in]    lUserID                  :   
*  @param[in]    pOsdDate                 : osd ʱ��(OSD����)
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetOsdDate(USER_HANDLE lUserID,
                                                           dvonetSDK:: tOsdDate *pOsdDate);

//�ݲ�ʵ��
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetVideoStandard(USER_HANDLE lUserID,
                                                                 struct tVideoStandard *pVideoStandard);

//����ptz���ƣ�ǹ���ݲ�ʵ��
/*
*  @date        2015/11/18   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ����ptz����    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_ControlPTZ(USER_HANDLE lUserID,struct tPtzControl *pPtzControl);
////��ȡ�ƶ�������Ԥ������չ��ʽ����
//DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetMotion(USER_HANDLE lUserID,dvonetSDK:: tMotion *pMotionSet);
////�����ƶ�������Ԥ������չ��ʽ����
//DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetMotion(USER_HANDLE lUserID,dvonetSDK:: tMotion *pMotionSet);

/*
*  @date         2015/11/18               : ��δʵ��
*  @brief        ��Ƶ�ڸ���Ϣ����(PM����  : 0x02 0x07 Э��������
*  @param[in]    lUserID                  :   
*  @param[in]    pMask                    : ��Ƶ�ڸ���Ϣ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetMask(USER_HANDLE lUserID,dvonetSDK:: tMask *pMask);

/*
*  @date         2015/11/18               :��δʵ��
*  @brief        ��Ƶ�ڸ���Ϣ��ѯ         : 0x02 0x08  Э��������
*  @param[in]    lUserID                  :   
*  @param[in]    pMask                    : ��Ƶ�ڸ���Ϣ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetMask(USER_HANDLE lUserID,dvonetSDK:: tMask *pMask);
 
//Զ��¼�񣬽ӿ�Ԥ��
/*
*  @date        2015/11/18           :��δʵ��
*  @brief       ����/�ر�Զ��¼��      
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_StartRecord(USER_HANDLE lUserID,int slot);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_StopRecord(USER_HANDLE lUserID,int slot);

//�����ص�����
//ֱ����userid����ֹһ���豸2�ε�¼�޷����֣�֮ǰ�Ľӿ�ֱ��ȥ��
//DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetAlarmCallback(void (CALLBACK* func)(const char *ip,int type,char *buf,int len,long nUser),long nUser);
//�����ص�����userid

/*
*  @date         2015/11/18 
*  @brief        ��Ϣ(����)�ص�           : 
*  @param[in]    lUserID                  :   
*  @param[in]    func                     :   
*  @param[in]    nUser                    : �ڵ��ò㣬ָ�������Ψһ������DVO_NET_SET_SetAlarmCallbackEx��ε��ã����Ǳ������һ������ʹ������Ҫע��
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetAlarmCallbackEx(void (CALLBACK* func)(USER_HANDLE lUserID,int type,char *buf,int len,long nUser),long nUser);

/*
*  @date         2015/11/18 
*  @brief        ��������                 :
*  @param[in]    lUserID                  :   
*  @param[in]    bRegister                :  0 �رգ�1����,
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_RegisterAlarm(USER_HANDLE lUserID,BOOL bRegister); //��������bRegister 0 �رգ�1����,

/*
*  @date         2015/11/18 
*  @brief        ��Ƶ������������(¼��)���� : 0x06 0x01
*  @param[in]    lUserID                  :   
*  @param[in]    pStreamType              : Ƶ������������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetStreamType(USER_HANDLE lUserID,dvonetSDK:: tStreamType *pStreamType);
/*
*  @date         2015/11/18  
*  @brief        ��Ƶ������������(¼��)��ѯ : 0x06 0x02
*  @param[in]    lUserID                  :   
*  @param[in out]    pStreamType              : Ƶ������������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetStreamType(USER_HANDLE lUserID, dvonetSDK:: tStreamType *pStreamType);

/*
*  @date         2015/11/17  
*  @brief        ʱ���ѯ             : 0x01 0x06
*  @param[in]    lUserID              :   
*  @param[in]    sysTime              : ʱ��
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_SET_GetSystime(USER_HANDLE lUserID, SYSTEMTIME* sysTime);
/*
*  @date         2015/11/17  
*  @brief        ʱ������             : 0x01 0x05
*  @param[in]    lUserID              :   
*  @param[in]    sysTime              : ʱ��
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetSystime(USER_HANDLE lUserID, SYSTEMTIME* sysTime);
/*
*  @date         2015/11/17  
*  @brief        ��������             : 0x01 0x04
*  @param[in]    lUserID              :   
*  @param[in out]networkSetInfo       : IO��������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetNetworkSetting(USER_HANDLE lUserID, dvonetSDK::tNetworkSetInfo* networkSetInfo);
/*
*  @date         2015/11/17  
*  @brief        ��������             :0x01 0x03
*  @param[in]    lUserID              :   
*  @param[in]    networkSetInfo       : ��������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD  DVO_NET_SET_SetNetworkSetting(USER_HANDLE lUserID, dvonetSDK::tNetworkSetInfo* networkSetInfo);
/**********************************************************/


//================= NVR ��ʱԤ���ýӿ� ==========================//
//��ȡ����ipcamͨ����ͨ��������Ϣ
/*
*  @date        2015/11/17   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetChannelSet(USER_HANDLE lUserID,struct tChannelSet *pChanSet);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_SetChannelSet(USER_HANDLE lUserID,struct tChannelSet *pChanSet);
//��ȡͨ������ipcam��״̬��0-δ���ӻ�δ������1-�������ӣ�����*pChanStatus��lenΪdvrͨ������byte����
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetChannelStatus(USER_HANDLE lUserID,struct tChannelStatus* pChanStatus);
DVOIPCNETSDK_API BOOL CALLMETHOD	DVO_NET_SET_GetChannelType(USER_HANDLE lUserID,struct tChannelType *pChanType);
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetSubChannelSet(USER_HANDLE lUserID,dvonetSDK::tSubChannelSet* pSubChannelSet);
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetSubChannelSet(USER_HANDLE lUserID,dvonetSDK::tSubChannelSet* pSubChannelSet);

//=================head data======================//
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_GetVideoHead(REAL_HANDLE lRealID,char* head,int len,int* head_len); //��ȡ�����ļ���ͷ 40���ֽ�
/*
*  @date        2015/11/17   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_PLAY_GetVideoHead(REC_HANDLE lPlayID,char* head,int len,int* head_len);//��ȡ����������Ƶͷ
/*
*  @date        2015/11/17   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetDownloadHead(DOWNLOAD_HANDLE lDownloadID,char* head,int len,int* head_len);//���á���ȡ�����ļ�����Ƶͷ

//===============================================//
//�ֶ������������ݵĲ��񣬱�����øýӿڲŻ�������
/*
*  @date        2015/11/17   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_StartDownloadCapture(long lDownloadID);	//�������ݱ����ýӿ�
//================�������(�̵���)״̬===========//
/*
*  @date         2015/11/17  
*  @brief        IO���������ѯ       :0x05 0x02
*  @param[in]    lUserID              :   
*  @param[out]   pAlarmOutState       : IO�������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetAlarmOutState(USER_HANDLE lUserID,dvonetSDK:: tAlarmOutState* pAlarmOutState);

//lAlarmOutPort:0xff-���м̵���  ����-ָ���̵���
//AlarmOutState: 1-��Ч 0- ��Ч
/*
*  @date         2015/11/17  
*  @brief        IO�����������       :0x05 0x01
*  @param[in]    lUserID              :   
*  @param[out]   AlarmOutState        : IO�������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetAlarmOutState(USER_HANDLE lUserID, /*LONG lAlarmOutPort,LONG*/ dvonetSDK:: tAlarmOutState* AlarmOutState);

//================������������===================//
/*
*  @date        2015/11/17   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetAlarmInConfig(USER_HANDLE lUserID,dvonetSDK::tAlarmInCfg* pAlarmInConfig);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetAlarmInConfig(USER_HANDLE lUserID,dvonetSDK::tAlarmInCfg* pAlarmInConfig);

//================�ֶ�������=====================//
//lAlarmInPort: ���������
//DefendType: 0- ��ʱ  1-����  2-����
/*
*  @date        2015/11/17   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetAlarmDefend(USER_HANDLE lUserID,LONG lAlarmInPort,LONG* DefendType);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetAlarmDefend(USER_HANDLE lUserID,LONG lAlarmInPort,LONG DefendType);

//================RS232����======================//
/*
*  @date        2015/11/17   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetRS232Config(USER_HANDLE lUserID,dvonetSDK::tRs232Cfg* pRS232Config);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetRS232Config(USER_HANDLE lUserID,dvonetSDK::tRs232Cfg* pRS232Config);

//================͸��ͨ��=======================//
//DVOIPCNETSDK_API dvonetSDK::SERIAL_HANDLE CALLMETHOD   DVO_NET_SET_StartRS232Send(USER_HANDLE lUserID,int Rs232No);
/*
*  @date         2015/11/17     
*  @brief        ͸������             : 0x04 0x02 
*  @param[in]    lUserID              :   
*  @param[out]   pRS232Send           : ͸������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SendRS232Data(dvonetSDK::SERIAL_HANDLE lSerialID,dvonetSDK::tRs232Send* pRS232Send);
//DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_StopRS232Send(dvonetSDK::SERIAL_HANDLE lSerialID);

//================PTZ RS232����=================//
/*
*  @date         2015/11/17  
*  @brief        ���ڲ�ѯ             :0x01 0x0c
*  @param[in]    lUserID              :   
*  @param[out]   pPtzRSConfig         :���ڲ���
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetPtzRSConfig(USER_HANDLE lUserID,dvonetSDK::tPtzRs232Cfg * pPtzRSConfig);
/*
*  @date         2015/11/17  
*  @brief        ��������             :0x01 0x0b
*  @param[in]    lUserID              :   
*  @param[out]   pPtzRSConfig         :���ڲ���
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetPtzRSConfig(USER_HANDLE lUserID,dvonetSDK::tPtzRs232Cfg * pPtzRSConfig);


//================��Ƶ��ʧ��������==============//
//=================Ŀǰ�ײ�δʵ�ֽӿ���ʱ����====//
/*
*  @date        2015/11/17   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_GetVideoLostConfig(USER_HANDLE lUserID,dvonetSDK::tVideoLostCfg* pVideoLostConfig);
DVOIPCNETSDK_API BOOL CALLMETHOD   DVO_NET_SET_SetVideoLostConfig(USER_HANDLE lUserID,dvonetSDK::tVideoLostCfg* pVideoLostConfig);

//================�����Խ�======================����ʵ��//
//VoiceComType: 0-�ͻ��˵����� 1-˫��
/*
*  @date        2015/11/17   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API dvonetSDK::VOICE_HANDLE CALLMETHOD DVO_NET_SET_StartVoiceCom(USER_HANDLE lUserID,int VoiceComType);
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_StopVoiceCom(dvonetSDK::VOICE_HANDLE lVoiceComHandle);
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_RegisterVoiceComCallBack(dvonetSDK::VOICE_HANDLE lVoiceComHandle,void(CALLBACK* fun)(long lVoiceComHandle,char *buf,int len,int flag,long dwUser),long dwUser);
  

//================�ƶ������չ=================//
/*
*  @date         2015/11/17  
*  @brief        MD�����ѯ          :0x05 0x04
*  @param[in]    lUserID             :   
*  @param[out]   pMotionEx           :MD����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetMotionRowCols(USER_HANDLE lUserID,dvonetSDK::tMotionRowCols* pMotionRowCols);
/*
*  @date         2015/11/17  
*  @brief        MD��������          :0x05 0x03
*  @param[in]    lUserID             :   
*  @param[out]   pMotionRowCols      :MD����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetMotionRowCols(USER_HANDLE lUserID,dvonetSDK::tMotionRowCols* pMotionRowCols);
/*
*  @date         2015/11/17  
*  @brief        MD������ѯ          :0x05 0x06
*  @param[in]    lUserID             :   
*  @param[out]   pMotionEx           :MD����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetMotionEx(USER_HANDLE lUserID,dvonetSDK::tMotionEx* pMotionEx);
/*
*  @date         2015/11/17  
*  @brief        MD��������          :0x05 0x05
*  @param[in]    lUserID             :   
*  @param[out]   pMotionEx           :MD����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetMotionEx(USER_HANDLE lUserID,dvonetSDK::tMotionEx* pMotionEx);
/*
*  @date        2015/11/17   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetSmartSearchFileEx(USER_HANDLE lUserID, dvonetSDK::tSmartSearchEx *pSearchTime, tRecFile *pSaveSSFile, long maxSSFileNum, long* rtnNum);
/*
*  @date        2015/11/17   �ϰ汾��û����Ӧ���룬��δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API long CALLMETHOD DVO_NET_SET_FindSmartSearchFileEx(USER_HANDLE lUserID,dvonetSDK::tSmartSearchEx *pSearch);


//================�ڵ�����=====================//
/*
*  @date        2015/11/17   ��ӦЭ�鲻������δʵ��
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetAlarmMask(USER_HANDLE lUserID,dvonetSDK::tAlarmMask* pAlarmMask);
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetAlarmMask(USER_HANDLE lUserID,dvonetSDK::tAlarmMask* pAlarmMask);


//=================Ӳ����Ϣ�͸�ʽ��============//
/*
*  @date         2015/11/17  
*  @brief        ������Ϣ��ѯ        :0x01 0x11
*  @param[in]    lUserID             :   
*  @param[out]   pHarddiskInfo       :������Ϣ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetHarddiskState(USER_HANDLE lUserID,dvonetSDK::tHarddiskInfo* pHarddiskInfo);

/*
*  @date         2015/11/17   
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_FormatHarddisk(USER_HANDLE lUserID,int disk_idx); 
/*
*  @date         2015/11/17   
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetFormatProgress(USER_HANDLE lUserID,int* progress);//0-100


//����оƬƥ��
/*
*  @date         2015/11/17  
*  @brief        ����оƬƥ��        :0x07 0x01
*  @param[in]    lUserID             :   
*  @param[out]   result              :result 0 Ϊƥ�䣬����Ϊƥ��
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_Get_MatchChip(USER_HANDLE lUserID,int *result);//result 0 Ϊƥ�䣬����Ϊƥ��

//wifi 
/*
*  @date         2015/11/17  
*  @brief        WIFI��ȡ����״̬����:0x07 0x02
*  @param[in]    lUserID             :   
*  @param[out]    result              :result 0 wifi�����ϣ�����δ����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_Get_WIFI(USER_HANDLE lUserID,int *result); 

//wifi set 
/*
*  @date         2015/11/17  
*  @brief        WIFI��������        :0x07 0x02
*  @param[in]    lUserID             :   
*  @param[in]    pInfo               :WIFI����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_Set_WIFI(USER_HANDLE lUserID,dvonetSDK::tWifiInfo *pInfo); 

//������Ƶ����˿�
/*
*  @date         2015/11/17  
*  @brief        ������Ƶ����˿�    :0x07 0x04
*  @param[in]    lUserID             :   
*  @param[in out]pInfo               :��Ƶ���
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SETAVout(USER_HANDLE lUserID,dvonetSDK::tAouInfo *pInfo); 

//set audio encode methord
/*
*  @date         2015/11/17  �����ʣ���δʵ��
*  @brief        ��Ƶ����������ƣ��Խ�������      :0x06 0x03
*  @param[in]    lUserID             :   
*  @param[in out]pInfo               :��Ƶ�����������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetAudioEnc(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_audio_config_t *pInfo);

//set video flep 
/*
*  @date         2015/11/17
*  @brief        ��Ƶģʽ����        :0x03 0x09
*  @param[in]    lUserID             :   
*  @param[in out]pInfo               :��Ƶģʽ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_SetFlep(USER_HANDLE lUserID,dvonetSDK::tImgeFelp *pInfo);

/*
*  @date         2015/11/17
*  @brief        ��Ƶģʽ��ѯ        :0x03 0x0a
*  @param[in]    lUserID             :   
*  @param[in out]pInfo               :��Ƶģʽ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_GetFlep(USER_HANDLE lUserID,dvonetSDK::tImgeFelp *pInfo);

//���û����ӿ�
/*
*  @date         2015/11/17   
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_Env(USER_HANDLE lUserID,dvonetSDK::tEvnParam *pInfo);

//Get�����ӿ�
/*
*  @date         2015/11/17   
*  @brief       ��δʵ��    
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_Env(USER_HANDLE lUserID,dvonetSDK::tEvnParam *pInfo);

//��ȡosd����
/*
*  @date         2015/11/17
*  @brief        �Զ���OSD���������ѯ :0x02 0x09
*  @param[in]    lUserID             :   
*  @param[in out]osd_area            :OSD�������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_CustomOSD(USER_HANDLE lUserID,dvonetSDK::tCustomAreaOSD *osd_area);

//����osd����
/*
*  @date         2015/11/17
*  @brief        �Զ���OSD����������� :0x02 0x08
*  @param[in]    lUserID             :   
*  @param[in out]osd_area            :OSD�������
*  @return       true,�ɹ���false,ʧ��
*  @remarks      �����ʣ�ָ����������Ƶ�ڸ���Ϣ��ͻ
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_CustomOSD(USER_HANDLE lUserID,dvonetSDK::tCustomAreaOSD *osd_area);

//��ȡosd��������.
/*
*  @date         2015/11/17
*  @brief        �Զ���OSD�в�����ѯ :0x02 0x0b
*  @param[in]    lUserID             :   
*  @param[in out]osdrow              :OSD�в���
*  @return       true,�ɹ���false,ʧ��

*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_CustomOSDRow(USER_HANDLE lUserID,dvonetSDK::tCustomRow *osdrow);

//�����еĲ���(��������,������ɫ,Ŀǰֻ֧��һ��Ĭ�Ϻ�ɫ,�޸�������ɫ��ҪIPC�ײ�����)b
/*
*  @date         2015/11/17
*  @brief        �Զ���OSD�в������� :0x02 0x0a
*  @param[in]    lUserID             :   
*  @param[in]    osdrow              :OSD�в���
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_CustomOSDRow(USER_HANDLE lUserID,dvonetSDK::tCustomRow *osdrow);

//��ȡosd���ı���Ϣ
/*
*  @date         2015/11/17
*  @brief        �Զ���OSD�����ݲ�ѯ :0x02 0x0d
*  @param[in]    lUserID             :   
*  @param[in out]osdrowtext          :OSD������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_CustomOSDRowText(USER_HANDLE lUserID,dvonetSDK::tCustomRow_Text *osdrowtext);
//�����е��ı�
/*
*  @date         2015/11/17
*  @brief        �Զ���OSD���������� :0x02 0x0c
*  @param[in]    lUserID             :   
*  @param[in]    osdrowtext          :OSD������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_CustomOSDRowText(USER_HANDLE lUserID,dvonetSDK::tCustomRow_Text  *osdrowtext);

//��ȡosd���ı���Ϣ ---->��������ɫ
/*
*  @date         2015/11/17
*  @brief        �Զ���OSD�����ݲ�ѯ :0x02 0x0d
*  @param[in]    lUserID             :   
*  @param[in out]osdrowtext          :SD������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_CustomOSDRowText2(USER_HANDLE lUserID,dvonetSDK::tCustomRow_Text2 *osdrowtext);

//�����е��ı�=====>��������ɫ
/*
*  @date         2015/11/17
*  @brief        �Զ���OSD���������� :0x02 0x0c
*  @param[in]    lUserID             :   
*  @param[in]    osdrowtext          :SD������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_CustomOSDRowText2(USER_HANDLE lUserID,dvonetSDK::tCustomRow_Text2  *osdrowtext);
/////////////////////////////////////////////////////////////////�¿���ʪ����ؽӿ� /////////////////////////////////////////////////

//�����¿ر������--->�����豸�������ò���,����������� data_type �ĺ���Ϊ��Ӧ�豸�Ĳ���ָ�� �ϲ�Ӧ�ø���DVO_NET_GET_DEVICE_FUNCTION_LIST���ж��豸����
/*
*  @date         2015/11/17
*  @brief        ���뱨���������    :0x05 0x07
*  @param[in]    lUserID             :   
*  @param[in]    pParam              :�������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_TEMPR_CTRL_AlarmOut(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_event_act_485_t *pState);  


//��ȡ�¿ر������
/*
*  @date         2015/11/17
*  @brief        ���뱨�������ѯ    :0x05 0x08
*  @param[in]    lUserID             :   
*  @param[in out]pParam             :�������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_TEMPR_CTRL_AlarmOut(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_event_act_485_t *pState);

//�����¿ر�������
/*
*  @date         2015/11/17
*  @brief        ���뱨����������    :0x05 0x09
*  @param[in]    lUserID             :   
*  @param[in]    pParam              :��������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_TEMPR_CTRL_AlarmParam(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_alarm_para_t *pParam); 

//��ȡ�¿ر������� 
/*
*  @date         2015/11/17
*  @brief        ���뱨��������ȡ    :0x05 0x0a
*  @param[in]    lUserID             :   
*  @param[out]   pParam              :��������
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_TEMPR_CTRL_AlarmParam(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_alarm_para_t *pParam); 


//�����¿ر����豸����
/*
*  @date         2015/11/17
*  @brief        �豸��Ϣ��ȡ        :0x07 0x05
*  @param[in]    lUserID             :   
*  @param[in]    pDevconfig          :�豸��Ϣ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_TEMPR_CTRL_DeviceConfig(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_dev_t *pDevconfig); 

//��ȡ�¿ر����豸����
/*
*  @date         2015/11/17
*  @brief        �豸��Ϣ��ȡ        :0x07 0x06
*  @param[in]    lUserID             :   
*  @param[out]   pDevconfig          :�豸��Ϣ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_TEMPR_CTRL_DeviceConfig(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_dev_t *pDevconfig); 


//�����¿�OSD��Ϣ
/*
*  @date         2015/11/17
*  @brief        ��������OSD�������� :  0x07 0x07
*  @param[in]    lUserID             :   
*  @param[in]    pOsddata            : OSD����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_TEMPR_CTRL_OSDData(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_osd_para_t *pOsddata); 

//��ȡ�¿�OSD��Ϣ--->������ ���ö�Ӧ��osd����
/*
*  @date         2015/11/17
*  @brief        ��������OSD������ѯ :  0x07 0x08
*  @param[in]    lUserID             :   
*  @param[out]   pOsddata            : OSD����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_TEMPR_CTRL_OSDData(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_osd_para_t *pOsddata); 

/*
*  @date         2015/11/17
*  @brief        ��ȡ�¿���Ϣ        :  0x07 0x09
*  @param[in]    lUserID             :   
*  @param[out]   pData               :  �¿�����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_TEMPR_CTRL_485_inputData(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_485_input_data_t *pData);


//NTPУʱ����
/*
*  @date         2015/11/17
*  @brief        ����NTP����         :  0x01 0x19
*  @param[in]    lUserID             :   
*  @param[in]   pNtpInfo            :  NTP����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_NTP(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_sys_ntp_para_t *pNtpInfo);

/*
*  @date         2015/11/17
*  @brief        NTP������ȡ         :  0x01 0x1a
*  @param[in]    lUserID             :   
*  @param[out]   pNtpInfo            :  NTP����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_NTP(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_sys_ntp_para_t *pNtpInfo);

/*
*  @date         2015/11/17
*  @brief        ��ʼNTP���ܲ���     :0x01 0x1d
*  @param[in]    lUserID             :   
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_Check_NTP(USER_HANDLE lUserID);

///////////////////////////////////////////////�¿���ʪ����ؽӿ�///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////��ȡ�豸�Ĺ����б�////////////////////////////////////////////////////////////

 
/*
*  @date         2015/11/16
*  @brief        ��ȡ�豸�Ĺ����б�  :0x01 0x1b
*  @param[in]    lUserID             :   
*  @param[out]   pListInfo           : �豸�Ĺ���
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_DEVICE_FUNCTION_LIST(USER_HANDLE lUserID,/*dvonetSDK::list_*/ dvonetSDK::app_net_tcp_sys_func_list_t *pListInfo);
 
/*
*  @date         2015/11/16
*  @brief        ��ѯIO����       :0x01 0x1e
*  @param[in]    lUserID          :   
*  @param[out]   pio_num     : IO����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_GET_IO_NUM(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_io_alarm_num_t *pio_num);

/*
*  @date         2015/11/16
*  @brief        ��ѯIO����       :0x01 0x1f
*  @param[in]    lUserID          :   
*  @param[out]   pio_property     : IO����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_GET_IO_Property(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_io_alarm_attr_t *pio_property);
/*
*  @date         2015/11/16
*  @brief        ����IO����       :0x01 0x20
*  @param[in]    lUserID          :   
*  @param[in]   pio_property      : IO����
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_SET_IO_Property(USER_HANDLE lUserID,dvonetSDK::app_net_tcp_io_alarm_attr_t *pio_property);

/*
*  @date         2015/11/16
*  @brief        ��ѯIO����״̬   :0x01 0x22
*  @param[in]    lUserID          :   
*  @param[out]    pio_status       : IO����״̬
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_GET_IO_IsEnable(USER_HANDLE lUserID, dvonetSDK::app_net_tcp_set_io_alarm_state_t *pio_status);
 
/*
*  @date         2015/11/16
*  @brief        ����IO����״̬   :0x01 0x21
*  @param[in]    lUserID          :   
*  @param[in]    pio_status       : IO����״̬
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_SET_IO_IsEnable(USER_HANDLE lUserID, dvonetSDK::app_net_tcp_set_io_alarm_state_t *pio_status);
/////////////////////////////////////////////////////////////////////������/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////������///////////////////////////////////////////////////////////////


//���� MAC ��SN ���к� 
//MAC��ַ���ַ������ַ���ȡֵ��ΧΪ��'0'-'9','A'-'F','a'-'f',':'��ÿ�����ַ���ʾһ��8λ����������ʮ�����Ƹ�ʽ���á�:���ָ������磺E2:02:6E:AD:17:19��
// MAC ������39���ַ��� SN:������31���ַ���
//�ӣ�Ĭ��Ϊ�ա��������Ϊ��������
 
DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_SET_MAC_SN(char * ip,int port,char *sn,char *mac); //

DVOIPCNETSDK_API BOOL CALLMETHOD DVO_NET_GET_MAC_SN(char * ip,int port,char *sn,char *mac);
//DVOIPCNETSDK_API double CALLMETHOD GetHighPerformanceTime();
///////////////////�ӿ�end///////////////////////////////////////////////////////

 
/////////////////////////////////////////////�°汾�ӿ� end///////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif

