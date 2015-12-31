/**
*  COPYRIGHT NOTICE 
*  Copyright (c) 2015, Diveo Technology Co.,Ltd (www.diveo.com.cn)
*  All rights reserved.
*
*  @file        dvoipcnetsdk.h
*  @author      luo_ws(luo_ws@diveo.com.cn)
*  @date        2015/08/11 15:18
*
*  @brief       dvoipcnetsdkͷ�ļ�
*  @note        ��
*
*  @version
*    - v1.0.1.0    2015/08/11 15:18    luo_ws 
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



///////////////////�ӿ�begin///////////////////////////////////////////////////////
 
/**
*  @date        2015/08/12 13:26
*  @brief       ����SDK ��ʼ��������һ�汾����һ�£����ǵ��� DVO2_NET_Init
*  @param[in]   localport,�°汾�У�����Ч
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������      
*  @remarks     SDKģ���һ��ʹ��ʱ���ã�ͬһ����ֻ��Ҫ����һ��       
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_Init();

/**
*  @date        2015/08/12
*  @brief       �ͷ�SDK������ص���Դ
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������       
*  @remarks     ����SDKģ��ʱ���ã���DVO2_NET_Init�ĵ��ö�Ӧ����������޷�ж��DLL�� 
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_Release();

/**
*  @date        2015/08/12
*  @brief       ��ȡ����SDK�İ汾
*  @param[out]  pMainVersion,���汾��
*  @param[out]  pSubVersion,�Ӱ汾��
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������      
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetSDKVersion(PARAM_OUT int *pMainVersion, PARAM_OUT int *pSubVersion);

/**
*  @date        2015/08/12
*  @brief       ��ȡ�������
*  @param[in]   lUserID,DVO2_NET_Login����ֵ
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������      
*  @remarks     ����ȡ���û�������صĴ����ʱ����Ҫ����lUserID���������Ϊ0ʱ��ȡSDK��ȫ�ִ���     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetLastError(PARAM_IN USER_HANDLE lUserID = 0);

/**
*  @date        2015/08/27
*  @brief       ��ͣ��־
*  @param[in]   bEnable,TRUEʱ������־�ļ������FALSEʱ�ر���־�ļ����
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������      
*  @remarks     ������־���ʱ�����ȡ���ڽ����ļ��ĵ�ǰ·���е���־�����ļ�dvoipcnetsdk.cfg��
                �Ҳ�������־�����ļ�ʱ����Ĭ����־���á������ļ��ɶ�̬�������ã����������־�ȼ����Ƿ�����ļ���  
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetSDKLogToFile(PARAM_IN BOOL bEnable);


//////////////////////////////////////////////////////////////////////////

/**
*  @date        2015/08/12
*  @brief       ��¼�豸    : 0x01 0x01
*  @param[in]   pDevIP      : �豸IP 
*  @param[in]   wDevPort    : �豸�˿ڣ�Ĭ��6001 
*  @param[in]   pUserName   : �û�����admin 
*  @param[in]   pPassword   : �û�����, 
*  @param[out]  pDevBaseInfo: �豸������Ϣ.��¼�ɹ���������
*  @param[out]  pErrNo      : ���ص�¼������,���������سɹ�ʱ,�ò�����ֵ������.
*  @param[in]   nWaitTimeSec: ��¼��ʱ��ʱ��,��λΪ���� 
*  @return      USER_HANDLE,ʧ�ܷ���DVO_INVALID_HANDLE -1���ɹ������豸ID     
*  @remarks     �豸Ĭ�϶˿�Ϊ6001����Ҫ��ipcͨ��ʱ�ĵ�¼�����¼�ɹ�֮����豸�Ĳ���������ͨ����ֵ(�豸���)��Ӧ����Ӧ���豸��
*/
DVOIPCNETSDK_API USER_HANDLE CALLMETHOD DVO2_NET_Login(PARAM_IN const char* pDevIP,
                                                      PARAM_IN WORD wDevPort,
                                                      PARAM_IN const char* pUserName,
                                                      PARAM_IN const char* pPassword,
                                                      PARAM_OUT app_net_tcp_sys_logo_info_t* pDevBaseInfo,
                                                      PARAM_OUT int* pErrNo,
                                                      PARAM_IN  int  nWaitTimeMsecs = 5000
                                                      );

/*
*  @date         2015/11/24                 :
*  @brief        ��¼��Ϣ��ѯ               :
*  @param[in]    lUserID                    :   
*  @param[out]   info                        :��¼��Ϣ
*  @return       true,�ɹ���false,ʧ��
*/
DVOIPCNETSDK_API BOOL CALLMETHOD DVO2_NET_RES_INFO(PARAM_IN USER_HANDLE lUserID,
                                                   PARAM_OUT  app_net_tcp_sys_logo_info_t *info);

/**
*  @date        2015/08/12
*  @brief       �ǳ��豸    : 0x01 0x02
*  @param[in]   lUserID : DVO2_NET_Login�ɹ��ķ���ֵ
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������    
*  @remarks     ��DVO2_NET_Login��Ӧ��������IPCͨ��ʱ���á���loginһ��Ҫ��logout�����������Դй©.     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_Logout(PARAM_IN USER_HANDLE lUserID);

/**
*  @date        2015/08/12
*  @brief       �������糬ʱ����
*  @param[in]   lUserID             : DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[in]   nSendTimeoutMsecs   : ���ͳ�ʱ����λΪ����,Ĭ��Ϊ5��
*  @param[in]   nRecvTimeoutMsecs   : ���ճ�ʱ����λΪ����,Ĭ��Ϊ5��
*  @param[in]   nConnTimeoutMsecs   : ���ӳ�ʱ����λΪ����,Ĭ��Ϊ5��
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������
*  @remarks     �����ÿ���ỰlUserID��أ������Ự֮��ĳ�ʱ�����໥����    
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetTimeout(PARAM_IN USER_HANDLE lUserID,
                                                   PARAM_IN int nSendTimeoutMsecs = 5000,    ///< ���ͳ�ʱ������
                                                   PARAM_IN int nRecvTimeoutMsecs = 5000,    ///< ���ճ�ʱ������
                                                   PARAM_IN int nConnTimeoutMsecs = 5000     ///< ���ӳ�ʱ������
                                                   );


/**
*  @date        2015/08/27
*  @brief       ���ö����Զ�����
*  @param[in]   lUserID                 : DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[in]   bAutoReConnect          : �Ƿ�����Զ�������Ĭ��ΪTRUE�Զ�������FALSEΪ�Ͽ�������
*  @param[in]   nReConnectIntervalMsecs : �����������λΪ����,Ĭ��Ϊ5�룬bAutoReConnectΪtrueʱ��Ч��
*  @param[in]   nReConnectType          : �������ͣ�Ĭ��Ϊ0��0--�ûỰ���е�����,1--�Ự������,2--��Ƶ���ӡ�
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������
*  @remarks     �����ÿ���ỰlUserID��أ��ϲ㲻����ʱ�˽ӿ�ʱ���ڲ�Ĭ��Ϊ�����Զ�����   
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetReconnect(PARAM_IN USER_HANDLE lUserID,
                                                     PARAM_IN BOOL bAutoReConnect          = TRUE,     ///< �����Զ�����
                                                     PARAM_IN int  nReConnectIntervalMsecs = 5000,     ///< �������������
                                                     PARAM_IN int  nReConnectType          = 0         ///< ��������
                                                     );

/**
*  @date        2015/08/21
*  @brief       ��Ϣ�ص�����,fnDVOCallback_AlarmData_T
*  @param[in]   lUserID       : �ỰID��DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[in]   nMessageType  : enum DVO_CALLBACK_MESSAGE_TYPE ���
*  @param[in]   nValue        : �򵥵���Ϣֵ,��Ͽ�����������Ƶ��ID��
*  @param[in]   pBuf          : ��Ϣ���ݻ�IO��������
*  @param[in]   nDataSize     : ���ݵĳ���
*  @param[in]   pUser         : �û�����,Ϊ�ϲ�DVO2_NET_SetMessageCallbackʱ�����ʶ
*  @return      �޷���ֵ 
*  @remarks     �����ڻص������н��к�ʱ�Ĵ���     
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
*  @brief       ������Ϣ�ص�����
*  @param[in]   lUserID       : DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[in]   funcMessage   : ��Ϣ�ص�����,���ڻص���Ƶ���ݵ��ϲ�Ӧ��,���嶨��ο�fnDVOCallback_Message_T
*  @param[in]   pUser         : �û�����,Ϊ�ϲ㴫���ʶ���ڻص������з��ظ��ϲ�
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������
*  @remarks     ��Ҫ���ڻỰ�쳣��Ϣ��֪ͨ�������ӶϿ������½������ӵ�֪ͨ��
                ��DVO2_NET_RegisterAlarm����������IPC�ϴ��������ݺ�Ҳͨ���˻ص��������ظ��ϲ㡣     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetMessageCallback(PARAM_IN USER_HANDLE lUserID,
                                                           PARAM_IN fnDVOCallback_Message_T funcMessage,
                                                           PARAM_IN void* pUser
                                                           );

/**
*  @date        2015/08/21
*  @brief       ���ñ���
*  @param[in]   lUserID     : DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[in]   bRegister   : �������� 0--�رգ�1--������
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������
*  @remarks     �ýӿ�������ö�� ,ÿһ�ο���һ·��Ƶ���� ��֧��ipc���ڶ��ͨ��ʱ���õ������     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_RegisterAlarm(PARAM_IN USER_HANDLE lUserID,
                                                      PARAM_IN BOOL        bRegister   //��������bRegister 0 �رգ�1����,
                                                      ); 


//////////////////////////////////////////////////////////////////////////


/**
*  @date        2015/08/12
*  @brief       ͨ��ʵʱ�����ݻص�����,fnDVOCallback_RealAVData_T
*  @param[in]   lUserID       : �ỰID��DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[in]   lStreamHandle : �����,DVO2_NET_StartRealPlay�ɹ���ķ���ֵ
*  @param[in]   nErrorType    : 0��ʾ���쳣��1��ʾ���ߣ�2��ʾ���ݳ���3��ʾ����ʧ�ܡ�4��ʾdirectdraw��ʼ��ʧ��...enum DVO_STREAM_ERROR_TYPE ���
*  @param[in]   pBuf          : ������(����֡ͷ,app_net_tcp_enc_stream_head_t + stream_data)
*  @param[in]   nDataSize     : �����ݵĳ���(����֡ͷ����,sizeof(app_net_tcp_enc_stream_head_t) + frame_len)
*  @param[in]   pUser         : �û�����,Ϊ�ϲ�DVO2_NET_StartRealPlayʱ�����ʶ
*  @return      �޷���ֵ 
*  @remarks     �����ڻص������н��к�ʱ�Ĵ���     
*/
typedef void (CALLBACK* fnDVOCallback_RealAVData_T)(PARAM_IN USER_HANDLE  lUserID,
                                                     PARAM_IN REAL_HANDLE lStreamHandle,
                                                     PARAM_IN int         nErrorType,
                                                     PARAM_IN const char* pBuf,
                                                     PARAM_IN int         nDataSize,
                                                     PARAM_IN void*       pUser);


/**
*  @date        2015/08/12
*  @brief       ��ʼ��Ƶ
*  @param[in]   lUserID     : DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[in]   nChannelID  : �豸ͨ���ţ���0��ʼ
*  @param[in]   nStreamID   : ����ID����0��ʼ��0Ϊ��������1Ϊ��������2Ϊ��������
*  @param[in]   nProtocol   : ͨ��Э�����ͣ�0--TCP ;1--UDP(UDP�ݲ�֧��)
*  @param[in]   nEncodeType : �������ͣ�0--H264��1--JPEG��2--265�� Ŀǰipcֻ֧��2���Ժ������չ
*  @param[in]   hWnd        : Ϊ������ʾԤ���Ĵ��ھ��,�ð汾��NULL,ֻ��ȡ����
*  @param[in]   funcRealData: �ص�����,���ڻص���Ƶ���ݵ��ϲ�Ӧ��,���嶨��ο�fnDVOCallback_RealAVData_T
*  @param[in]   pUser       : �û�����,Ϊ�ϲ㴫���ʶ������Ƶ�ص������з��ظ��ϲ�
*  @param[out]  pErrNo      : ���ش�����,���������سɹ�ʱ,�ò�����ֵ������.
*  @return      REAL_HANDLE,ʧ�ܷ���DVO_INVALID_HANDLE -1���ɹ�������Ƶ�ỰID 
*  @remarks     �ýӿ�������ö�� ,ÿһ�ο���һ·��Ƶ���� ��֧��ipc���ڶ��ͨ��ʱ���õ������     
*/
DVOIPCNETSDK_API REAL_HANDLE CALLMETHOD	DVO2_NET_StartRealPlay(PARAM_IN USER_HANDLE lUserID,
                                                              PARAM_IN int nChannelID,
                                                              PARAM_IN int nStreamID,
                                                              PARAM_IN int nProtocol,
                                                              PARAM_IN int nEncodeType,
                                                              PARAM_IN HWND hWnd,
                                                              PARAM_IN fnDVOCallback_RealAVData_T funcRealData,
                                                              PARAM_IN void* pUser,
                                                              PARAM_OUT int* pErrNo);


/**
*  @date        2015/08/12
*  @brief       �ر���Ƶ
*  @param[in]   lStreamHandle,DVO2_NET_StartRealPlay�ɹ��ķ���ֵ
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������    
*  @remarks     ��DVO2_NET_StartRealPlay��Ӧ.     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_StopRealPlay(PARAM_IN REAL_HANDLE lStreamHandle);


/**
*  @date        2015/09/06
*  @brief       ��ȡÿ�������ͳ�ƣ��ֽڣ�
*  @param[in]   lStreamHandle,DVO2_NET_StartRealPlay�ɹ��ķ���ֵ 
*  @param[out]  pFluxBytes,��������ÿ�������,��λΪ�ֽ�Byte 
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������    
*  @remarks          
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetStatiscFlux(PARAM_IN REAL_HANDLE lStreamHandle, PARAM_OUT UINT64* pFluxBytes);


/**
*  @date        2015/09/06
*  @brief       �����豸��Ƶ���Ĵ���
*  @param[in]   lUserID     : �豸��½��� ��DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[in]   nChannelID  : �豸ͨ���ţ���0��ʼ
*  @param[in]   bEnable     : ���ش����豸��Ƶ�� FALSE--ֹͣ��TRUE--������
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������
*  @remarks     ����ΪTRUE���Ż�����Ƶ������ʵʱ�����ݵ�fnDVOCallback_RealAVData_T�ص��г���    
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_RequestAudioStream(PARAM_IN USER_HANDLE lUserID,
                                                           PARAM_IN int         nChannelID,
                                                           PARAM_IN BOOL        bEnable   // 0 ֹͣ��1����,
                                                           ); 

/**
*  @date        2015/09/06
*  @brief       �����豸��Ƶ���Ĵ���״̬
*  @param[in]   lUserID     : �豸��½��� ��DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[in]   nChannelID  : �豸ͨ���ţ���0��ʼ
*  @param[out]  pEnable     : ���ش����豸��Ƶ��״̬ 0--ֹͣ��1--������
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������
*  @remarks        
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetAudioStreamState(PARAM_IN USER_HANDLE lUserID,
                                                            PARAM_IN int         nChannelID,
                                                            PARAM_IN int*        pEnable   //bEnable 0 ֹͣ��1����,
                                                            ); 

//////////////////////////////////////////////////////////////////////////

/**
*  @brief       ��ȡ�豸��ǰ��ϵͳʱ��
*  @param[in]   lUserID     : �豸��½��� ��DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[out]  pDevTime    : ϵͳʱ�䡣
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������
*  @remarks        
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetDevSystime(PARAM_IN USER_HANDLE lUserID,
                                                      PARAM_OUT app_net_tcp_sys_time_t* pDevTime
                                                      ); 

/**
*  @brief       �����豸��ǰ��ϵͳʱ��
*  @param[in]   lUserID     : �豸��½��� ��DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[out]  pDevTime    : ϵͳʱ��
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������
*  @remarks        
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetDevSystime(PARAM_IN USER_HANDLE lUserID,
                                                      PARAM_IN app_net_tcp_sys_time_t* pDevTime
                                                      ); 

/**
*  @date        2015/08/18
*  @brief       DVO2_NET_GetDevConfig,��ȡ�豸��������Ϣ
*  @param[in]   lUserID         : �豸��½��� 
*  @param[in]   dwCommand       : �豸��������.�μ���������,EDVO_DEVICE_PARAMETER_TYPE 
*  @param[in]   lpInBuffer      : ��������Ļ���ָ�� 
*  @param[in]   nInBufferSize   : ��������Ļ��峤��(���ֽ�Ϊ��λ) 
*  @param[out]  lpOutBuffer     : �������ݻ���ָ�� 
*  @param[in]   nOutBufferSize  : �������ݻ��峤��(���ֽ�Ϊ��λ) 
*  @param[out]  lpBytesReturned : ʵ���յ����ݵĳ��� 
*  @param[in]   nWaitTimeMsecs  : �ȴ���ʱʱ�� 
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������
*  @pre         
*  @remarks     ��ͬdwCommand(wCmdMain | (wCmdSub<<16) )��lpOutBuffer��Ӧ�Ľṹ�彫�᲻ͬ�������EDVO_DEVICE_PARAMETER_TYPE     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_GetDevConfig(PARAM_IN  USER_HANDLE lUserID,
                                                     PARAM_IN  DWORD   dwCommand,
                                                     PARAM_IN  LPVOID  lpInBuffer,  
                                                     PARAM_IN  int     nInBufferSize,
                                                     PARAM_OUT LPVOID  lpOutBuffer,  
                                                     PARAM_IN  int     nOutBufferSize,
                                                     PARAM_OUT int*    pBytesReturned,  
                                                     PARAM_IN  int     nWaitTimeMsecs=1000);

/**
*  @date        2015/08/18
*  @brief       �����豸��������Ϣ 
*  @param[in]   lUserID         : �豸��½��� 
*  @param[in]   dwCommand       : �豸��������.�μ���������,EDVO_DEVICE_PARAMETER_TYPE 
*  @param[in]   lpInBuffer      : ���ݻ���ָ�� 
*  @param[in]   nInBufferSize   : ���ݻ��峤��(���ֽ�Ϊ��λ) 
*  @param[in]   nWaitTimeMsecs  : �ȴ�ʱ�� 
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������
*  @pre         
*  @remarks     ��ͬdwCommand(wCmdMain | (wCmdSub<<16) )��lpOutBuffer��Ӧ�Ľṹ�彫�᲻ͬ�������EDVO_DEVICE_PARAMETER_TYPE 
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_SetDevConfig(PARAM_IN  USER_HANDLE lUserID,  
                                                     PARAM_IN  DWORD   dwCommand, 
                                                     PARAM_OUT LPVOID  lpInBuffer,  
                                                     PARAM_OUT int     nInBufferSize,  
                                                     PARAM_IN  int     nWaitTimeMsecs=1000);



//////////////////////////////////////////////////////////////////////////


/**
*  @brief       DVO2_NET_Restore,�ָ���������
*  @param[in]   lUserID �豸��½��� 
*  @param[in]   nTypeOperator ,�ָ����������ͣ�0Ϊ������ip�ָ���1Ϊ����ip�ָ�Ĭ�ϳ���ֵ
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������        
*  @remarks     
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_Restore(PARAM_IN USER_HANDLE lUserID, PARAM_IN int nTypeOperator);

/**
*  @brief       DVO2_NET_Reboot,�������
*  @param[in]   lUserID �豸��½��� 
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������        
*  @remarks          
**/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_Reboot(PARAM_IN USER_HANDLE lUserID);

/**
*  @brief       DVO2_NET_ShutDown,�ر����
*  @param[in]   lUserID �豸��½��� 
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������        
*  @remarks          
**/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_ShutDown(PARAM_IN USER_HANDLE lUserID);



///////////////////////�����Խ�,��δʵ��///////////////////////////////////////////////////

/** fnDVOCallback_AudioData_T
 * ��Ƶ�Խ�������ݻص�����
	lTalkHandle   : �Խ����
	pBuf          : ��Ƶ����
	nDataSize     : ���ݳ���
	pts           : ʱ���(����)
	userdata      : �û�����
*/
typedef void (*fnDVOCallback_AudioData_T)(PARAM_IN USER_HANDLE  lUserID,
                                          PARAM_IN REAL_HANDLE  lTalkHandle,
                                          PARAM_IN BYTE*        pBuf,
                                          PARAM_IN int          nDataSize,
                                          PARAM_IN int          pts,
                                          PARAM_IN void*        pUser
                                          );

/** 
 *  @brief       ���������Խ���DVO2_NET_TalkStart
 *  @param[in]   lUserID        : DVO2_NET_Login�ɹ��ķ���ֵ
 *  @param[in]   pAudioInfo		: ��Ƶ����
 *  @param[in]   nTimeroutMs	: ��ʱʱ��(����)
 *  @param[in]   bTwoWayData	: ˫��Խ���־
 *  @param[in]   funcAudioData  : ���նԽ����ݻص�����,���ڻص��Խ����ݵ��ϲ�Ӧ��,���嶨��ο�fnDVOCallback_AudioData_T
 *  @param[in]   pUser          : �û�����,Ϊ�ϲ㴫���ʶ������Ƶ�ص������з��ظ��ϲ�
 *  @param[out]  pErrNo         : ���ش�����,���������سɹ�ʱ,�ò�����ֵ������.
 *  @return      REAL_HANDLE,ʧ�ܷ���DVO_INVALID_HANDLE -1���ɹ����������Խ��ỰID 
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
 *  @brief       ���ͶԽ�����,DVO2_NET_TalkSendData
 *  @param[in]   lTalkHandle		: �Խ����, DVO2_NET_TalkStart�����ֵ
 *  @param[in]   pBuf   			: ��Ƶ����
 *  @param[in]   nDataSize			: ���ݴ�С
 *  @param[in]   pts				: ʱ���
 *  @return      �ɹ�����0�� ����Ϊ�������
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_TalkSendData(PARAM_IN REAL_HANDLE lTalkHandle,
                                                     PARAM_IN BYTE* pBuf,
                                                     PARAM_IN int   nDataSize,
                                                     PARAM_IN int   pts
                                                     );

/* DVO2_NET_TalkStop
 * ֹͣ�Խ�
	lTalkHandle		: �Խ����, DVO2_NET_TalkStart �����ֵ
 * �ɹ�����0�� ����Ϊ�������
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_TalkStop(PARAM_IN USER_HANDLE lTalkHandle);

//////////////////////////////////////////////////////////////////////////

//=================�ṩ�豸����============================//
// ��δʵ�֣���ع�����ʹ��DVO��������

/** 
*  @brief       �����豸�ص�,fnDVOCallback_SearchDevice_T
*  @param[in]   nType         : 0Ϊ������Ϣ,0x0002Ϊ�����豸����Ϣ,
*  @param[in]   nState        : nType=0ʱ��Ч��Ϊ����ID
*  @param[in]   pBuf          : ����,multicast_dev_info_t_v2
*  @param[in]   nDataSize     : ���ݳ���
*  @param[in]   pUser         : �û�����,Ϊ�ϲ�DVO2_NET_StartSearchDeviceʱ�����ʶ
*  @return      �޷���ֵ 
*  @remarks     �ڻص������У���Ҫ���к�ʱ����������ܻ�������������������     
*/
typedef void (CALLBACK *fnDVOCallback_SearchDevice_T)(PARAM_IN int         nType,
                                                      PARAM_IN int         nState,
                                                      PARAM_IN const char* pBuf, 
                                                      PARAM_IN int         nDataSize, 
                                                      PARAM_IN void*       pUser
                                                      );

/** 
 *  @brief       ��ʼ�����豸,DVO2_NET_StartSearchDevice
 *  @param[in]   funcSearchDevice : �ص�����,���ڻص����������״̬���ϲ�Ӧ��,���嶨��ο�fnDVOCallback_SearchDevice_T
 *  @param[in]   pUser            : �û�����,Ϊ�ϲ㴫���ʶ���ڻص������з��ظ��ϲ�
 *  @return      �ɹ�����0������Ϊʧ�ܵĴ������ 
 *  @remarks     �����豸����Ϊ�첽,ͨ���ص�������������״̬��ֻ��DVO2_NET_StopSearchDevice֮�󣬲����ٴε��ô˽ӿ�.   
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_StartSearchDevice(PARAM_IN fnDVOCallback_SearchDevice_T funcSearchDevice,
                                                          PARAM_IN void *pUser
                                                          );

/** 
 *  @brief       ֹͣ�����豸
 *  @return      �ɹ�����0������Ϊʧ�ܵĴ������ 
 *  @remarks     DVO2_NET_StartSearchDevice֮�󣬲��ܵ��ô˽ӿ�  
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_StopSearchDevice();

/** 
 *  @brief       �ٴ������豸
 *  @return      �ɹ�����0������Ϊʧ�ܵĴ������ 
 *  @remarks     ��������������ģ��,DVO2_NET_StartSearchDevice�����ܵ��ô˽ӿڣ�������Ч.  
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_AgainSearchDevice();

/** 
 *  @brief       �޸��豸����
 *  @param[in]   pDevInfo       : �豸��Ϣ
 *  @param[in]   nModifyType    : �޸ĵ��������ͣ�0Ϊ�޸��豸IP��ַ,1Ϊ�޸��豸MAC��ַ
 *  @return      �ɹ�����0������Ϊʧ�ܵĴ������ 
 *  @remarks     ��������������ģ��,DVO2_NET_StartSearchDevice.  
*/
DVOIPCNETSDK_API int CALLMETHOD DVO2_NET_ModifySearchDevice(const multicast_dev_info_t_v2* pDevInfo, int nModifyType);

//////////////////////////////////////////////////////////////////////////

//=================�ṩϵͳ����============================//
// ��δʵ�֣���ع�����ʹ��DVO��������

/**
*  @brief       �ļ�����ص�,fnDVOCallback_AlarmData_T
*  @param[in]   lUserID       : �ỰID��DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[in]   nTransType    : enum DVO_CALLBACK_TRANSFILE_STATE_TYPE ���
*  @param[in]   nState        : nTransType=DVO_DEV_UPGRADEFILETRANS_ERRORʱ��Ч��Ϊ����ID
*  @param[in]   nSendSize     : �ѷ��ͳ���
*  @param[in]   nTotalSize    : �ļ��ܳ���
*  @param[in]   pUser         : �û�����,Ϊ�ϲ�DVO2_NET_StartUpgradeʱ�����ʶ
*  @return      �޷���ֵ 
*  @remarks     �����ڻص������н��к�ʱ�Ĵ���     
*/
typedef void (CALLBACK *fnDVOCallback_TransFile_T)(PARAM_IN USER_HANDLE lUserID, 
                                                   PARAM_IN int         nTransType, //DVO_CALLBACK_TRANSFILE_STATE_TYPE
                                                   PARAM_IN int         nState,     
                                                   PARAM_IN int         nSendSize, 
                                                   PARAM_IN int         nTotalSize, 
                                                   PARAM_IN void*       pUser);

/**
*  @brief       ��ʼ����
*  @param[in]   lUserID       : DVO2_NET_Login�ɹ��ķ���ֵ
*  @param[in]   szFileName    : �����ļ�
*  @param[in]   funcTransFile : �ص�����,���ڻص�����״̬���ϲ�Ӧ��,���嶨��ο�fnDVOCallback_TransFile_T
*  @param[in]   pUser         : �û�����,Ϊ�ϲ㴫���ʶ���ڻص������з��ظ��ϲ�
*  @param[in]   wDevPort      : �豸�˿�,����Ĭ����6002 
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������ 
*  @remarks     ��������Ϊ�첽,ͨ���ص�������������״̬     
*/
DVOIPCNETSDK_API int CALLMETHOD	DVO2_NET_StartUpgrade(PARAM_IN USER_HANDLE               lUserID, 
                                                     PARAM_IN const char*               szFileName, 
                                                     PARAM_IN fnDVOCallback_TransFile_T funcTransFile, 
                                                     PARAM_IN PARAM_IN void*            pUser,
                                                     PARAM_IN WORD                      wDevPort = 6002
                                                     );

/**
*  @brief       ֹͣ����
*  @param[in]   lUserID : DVO2_NET_Login�ɹ��ķ���ֵ
*  @return      �ɹ�����0������Ϊʧ�ܵĴ������    
*  @remarks     ��DVO2_NET_StartUpgrade��Ӧ,����ֹ�����ļ��Ĵ��䣬��������ļ������Ѿ���ɣ��˽ӿڵ�����Ч     
*/
DVOIPCNETSDK_API int CALLMETHOD	DVO2_NET_StopUpgrade(PARAM_IN USER_HANDLE lUserID);
 
///////////////////�ӿ�end///////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif
