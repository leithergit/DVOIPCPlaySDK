#include "stdafx.h"
#include "TVTChan_1.h"
#include "./sdk/include/DVR_NET_SDK.h"
#pragma comment(lib,"DVR_NET_SDK.lib")
CTVTChan_1::CTVTChan_1(void)
{
    m_hUserHandle = -1;
    m_lLiveHandle = -1;
	m_hPlayHandle = nullptr;
	NET_SDK_Init();
}

CTVTChan_1::~CTVTChan_1(void)
{
}

BOOL CTVTChan_1::Play(string ip,int nPort,HWND hwnd)
{

    m_hWnd = hwnd;

	m_hUserHandle = NET_SDK_Login((char *)ip.c_str(), nPort, "admin", "123456", &m_SystemParam);

    if(m_hUserHandle == -1)
    {
        return false;
    }
    NET_SDK_CLIENTINFO clientInfo = {0};
    clientInfo.hPlayWnd =0;
    clientInfo.lChannel = 0;
    clientInfo.streamType =NET_SDK_MAIN_STREAM;// NET_SDK_SUB_STREAM;  
	StartPlayer();
    m_lLiveHandle = NET_SDK_LivePlay(m_hUserHandle, &clientInfo, &CTVTChan_1::LiveCallBack, this);
    if(m_lLiveHandle == -1)
    {
        int nRet = NET_SDK_GetLastError();        
        return FALSE;
    } 
    NET_SDK_MakeKeyFrame(m_hUserHandle, m_lLiveHandle);
    return TRUE;
}
BOOL CTVTChan_1::StartPlayer()
{
    if(m_hPlayHandle)
    {
        return true;
    } 
	DVO_MEDIAINFO mediaheader;
	mediaheader.nVideoWidth = 1280;
	mediaheader.nVideoHeight = 960;
	mediaheader.nVideoCodec = CODEC_H264;
	mediaheader.nAudioCodec = CODEC_G711U;
    m_hPlayHandle = dvoplay_OpenStream(m_hWnd,nullptr,0,128,NULL);
	
    bool bEnableHaccel=false;

    //¿ªÊ¼²¥·Å
    int nRe = dvoplay_Start(m_hPlayHandle,FALSE,TRUE,bEnableHaccel);

    return true;

}
typedef enum {
    APP_NET_TCP_COM_DST_IDR_FRAME = 1,  ///< IDRÖ¡¡£
    APP_NET_TCP_COM_DST_I_FRAME,        ///< IÖ¡¡£
    APP_NET_TCP_COM_DST_P_FRAME,        ///< PÖ¡¡£
    APP_NET_TCP_COM_DST_B_FRAME,        ///< BÖ¡¡£
    APP_NET_TCP_COM_DST_JPEG_FRAME,     ///< JPEGÖ¡
    APP_NET_TCP_COM_DST_711_ALAW,       ///< 711 AÂÉ±àÂëÖ¡
    APP_NET_TCP_COM_DST_711_ULAW,       ///< 711 UÂÉ±àÂëÖ¡
    APP_NET_TCP_COM_DST_726,            ///< 726±àÂëÖ¡
    APP_NET_TCP_COM_DST_AAC,            ///< AAC±àÂëÖ¡¡£
    APP_NET_TCP_COM_DST_MAX,
} APP_NET_TCP_STREAM_TYPE;


void CTVTChan_1::DealLiveCallBack(NET_SDK_FRAME_INFO frameInfo, BYTE *pBuffer)
{
     int nType ;
    if(frameInfo.frameType ==DD_FRAME_TYPE_VIDEO_FORMAT)
    {  
         nType =APP_NET_TCP_COM_DST_I_FRAME;

    }
    else  if(frameInfo.frameType ==DD_FRAME_TYPE_VIDEO)
    {
        nType =APP_NET_TCP_COM_DST_P_FRAME;
        if(frameInfo.keyFrame == 1)
            nType =APP_NET_TCP_COM_DST_I_FRAME;

       // TRACE("[%s],len = %d\n\r",__FUNCSIG__,frameInfo.length);

    }
    else if(frameInfo.frameType ==DD_FRAME_TYPE_AUDIO_FORMAT)
    {
          return ;
    }
    else
        return ;

    if (m_hPlayHandle)
    {
         int  nRe =dvoplay_InputIPCStream(m_hPlayHandle, pBuffer, nType, 
            frameInfo.length, frameInfo.streamID,frameInfo.time);

    }
 
}
void CALLBACK  CTVTChan_1::LiveCallBack(LONG lLiveHandle, NET_SDK_FRAME_INFO frameInfo, BYTE *pBuffer, void *pUser)
{
    CTVTChan_1 *pThis = reinterpret_cast<CTVTChan_1 *>(pUser);
    if(pThis)
        pThis->DealLiveCallBack(frameInfo,pBuffer);

}