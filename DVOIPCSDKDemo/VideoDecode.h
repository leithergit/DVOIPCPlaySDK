#ifndef _VIDEO_DECODE_2015_08_30
#define _VIDEO_DECODE_2015_08_30
#pragma once

#include "stdafx.h"
#include "DVOIPCSDKDemo.h"


// #include <sdl.h>
// #pragma comment(lib,"SDL2")
//#pragma comment(lib,"SDL2main")
#ifdef _DEBUG
#pragma comment(lib,"../Hisilicon/HW_H265dec_Win32D")
#else
#pragma comment(lib,"../Hisilicon/HW_H265dec_Win32")
#endif

#define _Max_Image_Width	(4096)
#define _Max_Image_Height	(2160)

#define _ViewVideoWidth		640
#define _ViewVideoHeight	360

#define _FullScreenWidth	1920
#define _FullScreenHeight	1080

#define DVO_SUCCEED			0

#define _OSD_LEFT			1300
#define _OSD_TOP			20
#define _OSD_H_INTERVAL		60

enum Decoder_Manufacturer
{
	UnKnown = 0,
	HISILICON = 1,
	FFMPEG = 2,
	DVO_HACCEL = 3
};
// 更新解码器信息 WPARAM为CVideoDecoder对象指针
#define WM_UPDATE_DECODERINFO	WM_USER + 2048

enum ScaleOperation
{
	Zoom_In,
	Zoom_Out,
	Zoom_Unkown
};

#define _Frame_PERIOD			30.0f		// 一个帧率区间
#pragma pack(push)
#pragma pack(1)
struct FrameInfo
{
	time_t  tRecvTime;				// 收到帧的时间 单位毫秒
	int		nFrameSize;
};

struct StreamInfo
{	
	UINT	nFrameID;
	bool	bRecvIFrame;		// 是否收到第个I帧
	int		nVideoFrameCount;
	int		nAudioFrameCount;
	__int64 nVideoBytes;		// 收到视频总字节数
	__int64 nAudioBytes;		// 收到音频总字节数
	time_t  tFirstTime;			// 接收数据的起始时间	单位毫秒
	time_t	tLastTime;			// 最后一次接收数据时间 单位毫秒
	CRITICAL_SECTION	csFrameList;
	list<FrameInfo>FrameInfoList;
	StreamInfo()
	{
		ZeroMemory(this,offsetof(StreamInfo,FrameInfoList));
		::InitializeCriticalSection(&csFrameList);
	}
	~StreamInfo()
	{
		::DeleteCriticalSection(&csFrameList);
	}

	int GetFrameRate()
	{
		CAutoLock lock(&csFrameList);
		if (FrameInfoList.size() > 2)
		{
			time_t tSpan = FrameInfoList.back().tRecvTime - FrameInfoList.front().tRecvTime;
			if (!tSpan)
				tSpan = 1000;
			int nSize = FrameInfoList.size();
			return nSize*1000 /(int)(tSpan);
		}
		else
			return 1;
	}

	int	GetVideoCodeRate(int nUnit = 1024)	// 取得视频码率(Kbps)
	{
		time_t tSpan = 25;
		CAutoLock lock(&csFrameList);
		if (FrameInfoList.size() > 1)		
			tSpan =  FrameInfoList.back().tRecvTime - FrameInfoList.front().tRecvTime;
		__int64 nTotalBytes = 0;

		for (list<FrameInfo>::iterator it = FrameInfoList.begin();
			it != FrameInfoList.end();
			it ++)
			nTotalBytes += (*it).nFrameSize;

		if (tSpan == 0)
			tSpan = 1;
		__int64 nRate = (nTotalBytes*1000*8/((int)tSpan*nUnit));
		return (int)nRate;
	}
	int GetAudioCodeRate(int nUnit = 1024)
	{
		time_t tSpan = (tLastTime - tFirstTime);
		if (tSpan == 0)
			tSpan = 1;
		return (int)(nAudioBytes*1000/(tSpan*nUnit));
	}
	void inline Reset()
	{
		ZeroMemory(this,offsetof(StreamInfo,csFrameList));
		CAutoLock lock(&csFrameList);
		FrameInfoList.clear();
	}
	void PushFrameInfo(int nFrameSize)
	{
		time_t tNow = (time_t)(GetExactTime()*1000);
		CAutoLock lock(&csFrameList);
		if (FrameInfoList.size() > 1)
		{
			for (list<FrameInfo>::iterator it = FrameInfoList.begin();
				it != FrameInfoList.end();)
			{
				if ((tNow - (*it).tRecvTime) > _Frame_PERIOD*1000)
					it = FrameInfoList.erase(it);
				else
					it ++;
			}
// 			while ((tNow - FrameInfoList.front().tRecvTime) > _Frame_PERIOD*1000)
// 				FrameInfoList.pop_front();
		}

		FrameInfo fi;
		fi.tRecvTime = tNow;
		fi.nFrameSize = nFrameSize;
		FrameInfoList.push_back(fi);
	}

};
#pragma pack(pop)

typedef LRESULT  (__stdcall *myWndProc)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class CVideoPlayer;

struct PlayContext
{
	PlayContext()
	{
		ZeroMemory(this,sizeof(PlayContext));
		nSize = sizeof(PlayContext);
	}
	PlayContext(const PlayContext &pc)
	{
		memcpy(this,&pc,sizeof(PlayContext));
		nSize = sizeof(PlayContext);
	}
	int		nSize;
	HWND	hVideoWnd;
	int		nMaxVideoWidth;
	int		nMaxVideoHeight;
	int		nWndWidth;
	int		nWndHeight;
	DVO_CODEC	nCodecType;
};

#define _Max_Monitors	10		// Windows系统最多只能接10个显示器
#define _StaticText		2
class CVideoPlayer
{

#ifndef FFALIGN
#define FFALIGN(x, a) (((x)+(a)-1)&~((a)-1))
#endif

#ifndef DXVA2_SURFACE_BASE_ALIGN
#define DXVA2_SURFACE_BASE_ALIGN 16
#endif

	
public:
	CVideoPlayer(USER_HANDLE hUser,TCHAR *szIp)
	{
		ZeroMemory(this,offsetof(CVideoPlayer,m_pStreamInfo));
		m_hUserHandle = hUser;
		m_hStreamHandle = -1;
		m_nEnableFullScreen = -1;	// 未启用全屏
		
		_tcscpy_s(m_szIpAddress,32,szIp);
		::InitializeCriticalSection(&m_csRecFile);
		::InitializeCriticalSection(&m_csStreamList);
		::InitializeCriticalSection(&m_csD3DRender);
	}

	~CVideoPlayer()
	{
		m_bThreadRun = false;
		StopReceive();

		if (m_hFullScreen)
		{
			m_hFullScreen = NULL;
		}

		if (m_hWndView)
			m_hWndView = NULL;
		if (m_hStreamHandle != -1)
		{
			DVO_NET_StopRealPlay(m_hStreamHandle);
			m_hStreamHandle = -1;
		}
		if (m_hUserHandle != -1)
		{
			DVO_NET_Logout(m_hUserHandle);
			m_hUserHandle = -1;
		}		
		m_StreamList.clear();
					
		DeleteCriticalSection(&m_csRecFile);
		DeleteCriticalSection(&m_csStreamList);
		DeleteCriticalSection(&m_csD3DRender);
	}

	DWORD GetAlignedDimension(DWORD dim)
	{
		int align = DXVA2_SURFACE_BASE_ALIGN;

		// MPEG-2 needs higher alignment on Intel cards, and it doesn't seem to harm anything to do it for all cards.
		if (m_pAvCodec->id == AV_CODEC_ID_MPEG2VIDEO)
			align <<= 1;
		else if (m_pAvCodec->id == AV_CODEC_ID_HEVC)
			align = 128;

		return FFALIGN(dim, align);
	}
	void ClearFrameCacke()
	{
		CAutoLock lock(&m_csStreamList);
		m_StreamList.clear();
	}
	// 码流接收线程
	// 若录像文件指针有效则进行录像
	// 若渲染对象有效，则进行解码并显示图像
	static UINT	PlayThread(void *p)
	{
		CVideoPlayer *pThis = (CVideoPlayer *)p;
		// 设置图像格式转换参数
		AVPacket *pAvPacket = (AVPacket *)av_malloc(sizeof(AVPacket));	
		DVOFramePtr pFrame;
		int nQueueSize = 0;
		int nGot_picture = 0;
		int nAvError = 0;
		char szAvError[1024] = {0};
		bool bRenderCreated = false;
		int nImageWidth = 0,nImageHeight = 0;
		AVFrame		*pScaleFrame = NULL;
		DWORD nResult = 0;
		UINT nTimeoutms = 250;		// 最长等待250ms后返回

		if (pThis->m_nDecodecManufacturer == DVO_HACCEL)
		{
			pThis->m_pHaccelFrame = av_frame_alloc();
			pScaleFrame = pThis->m_pHaccelFrame;
		}
		else
			pScaleFrame = pThis->m_pAvFrame;

		while (pThis->m_bThreadRun)
		{
			::EnterCriticalSection(&pThis->m_csStreamList);
			nQueueSize = pThis->m_StreamList.size();
			if (nQueueSize > 0)
			{
				pFrame = pThis->m_StreamList.front();
				pThis->m_StreamList.pop_front();
			}
// 			if (nQueueSize > 10 && pFrame->nType == 0)
// 			{// 缓存大于10，则丢弃一些P帧
// 				for (list<DVOFramePtr>::iterator it = pThis->m_StreamList.begin();
// 					it != pThis->m_StreamList.end();)
// 				{
// 					if ((*it)->nType == 2)
// 					{
// 						it = pThis->m_StreamList.erase(it);
// 					}
// 					else
// 						it ++;
// 				}
// 				nQueueSize = pThis->m_StreamList.size();
// 			}
			::LeaveCriticalSection(&pThis->m_csStreamList);
			pThis->m_nFraceCache = nQueueSize;
			if (!nQueueSize)
			{
				Sleep(10);
				continue;
			}
			::PostMessage(pThis->m_hRecvMsg,WM_UPDATE_DECODERINFO,(WPARAM)pThis,0);
			if (pThis->m_pRecFile)
			{// 必须要有打开的文件指针，才予以录像
				if (!pThis->m_bRecIFrame)		// 从未收到过录像I帧
				{
					if (pFrame->nType == 0 ||// 海思I帧号为0，这是固件的一个BUG，有待修正
						pFrame->nType == APP_NET_TCP_COM_DST_IDR_FRAME ||
						pFrame->nType == APP_NET_TCP_COM_DST_I_FRAME)
					{
						pThis->m_bRecIFrame = true;
						printf(_T("Recv the First I Frame for Record,Now begin write file.\n"));
					}
				}
				if (pThis->m_bRecIFrame)
				{
					try
					{// 录像和显示分开进行,因此无法获知是否有解码的I帧
						CAutoLock lock(&pThis->m_csRecFile);
						if (pThis->m_pRecFile)
						{
 							pThis->m_pRecFile->Write(pFrame->pData,pFrame->nLength);
 							pThis->m_nRecFileLength = pThis->m_pRecFile->GetLength();
							//nWriteLength += fwrite(pFrame->pData,1,pFrame->nLength,pThis->m_pRecFile);
						}
					}
					catch (std::exception* e)
					{
						TCHAR szError[256] = {0};
						printf("Exception on Write record file:%s.\n",e->what());
					}
				}

			}
						
			if (pThis->m_bRender)
			{
				CTryLock Lock;				
				if (Lock.TryLock(&pThis->m_csD3DRender))
				{// 必须要有视频渲染器才进行解码
					if (pThis->m_nDecodecManufacturer == HISILICON)
					{
						if (pThis->m_pPlayContext->nCodecType == DVO_CODEC_H265)
						{
							IH265DEC_INARGS stInArgs;
							stInArgs.pStream = pFrame->pData;
							stInArgs.uiStreamLen = pFrame->nLength;
							stInArgs.uiTimeStamp = time(NULL);
							IH265DEC_OUTARGS stOutArgs;
							nGot_picture = 0;
							nAvError = IHW265D_DecodeAU(pThis->m_hDecoder265, &stInArgs, &stOutArgs);
							if (nAvError!= IHW265D_OK)
							{
								HisiconError(nAvError,szAvError,1024);
								printf("IHW265D_DecodeAU Decodec Error:%s.\n",szAvError);
								continue;
							}

							if(stOutArgs.eDecodeStatus == IH265D_NO_PICTURE)
							{
								continue;;
							}
							// output decoded pictures
							if (stOutArgs.eDecodeStatus == IH265D_GETDISPLAY)
							{
								HisiconH265Frame2FFmpegAvFrame(&stOutArgs,pThis->m_pAvFrame);
								nGot_picture = 1;
							}
						}
					}
					else
					{
						av_init_packet(pAvPacket);
						pAvPacket->data = (byte *)pFrame->pData;
						pAvPacket->size = pFrame->nLength;
						pAvPacket->flags= AV_PKT_FLAG_KEY;
						int nFrameID = pFrame->nFrameID;
						int nFrameLen = pFrame->nLength;
						byte *pFrameBuff = pFrame->pData;
						nAvError = avcodec_decode_video2(pThis->m_pAvCodecCtx,pThis->m_pAvFrame,&nGot_picture,pAvPacket);
						if (nAvError < 0)
						{
							av_strerror(nAvError, szAvError, 1024);		
							printf("Decode error:%s.\n",szAvError);
							continue;
						}
					}
					if (nGot_picture)
					{
						printf("Get a Decoded Frame,format = %d.\n",pThis->m_pAvFrame->format);
						if (!pThis->m_pDxSurface)
						{
							pThis->m_pDxSurface = new CDxSurface; 
							DxSurfaceInitInfo &InitInfo = pThis->m_DxInitInfo;
							InitInfo.nSize			 = sizeof(DxSurfaceInitInfo);
							InitInfo.bWindowed		 = TRUE;
							InitInfo.hPresentWnd	 = pThis->m_pPlayContext->hVideoWnd;
							InitInfo.nFrameWidth	 = pThis->m_pAvFrame->width;
							InitInfo.nFrameHeight	 = pThis->m_pAvFrame->height;
							InitInfo.nD3DFormat		 = (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2');
							if (pThis->m_nDecodecManufacturer == DVO_HACCEL)
							{
								InitInfo.nFrameWidth = pThis->GetAlignedDimension(pThis->m_pAvFrame->width);
								InitInfo.nFrameHeight= pThis->GetAlignedDimension(pThis->m_pAvFrame->height);
								InitInfo.nD3DFormat	 = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
							}
							::SendMessageTimeout(pThis->m_hRecvMsg,WM_INITDXSURFACE,(WPARAM)pThis->m_pDxSurface,(LPARAM)&InitInfo,SMTO_BLOCK,250,(PDWORD_PTR)&nResult);
						}
						DxSurfaceRenderInfo RenderInfo;
						RenderInfo.nSize = sizeof(DxSurfaceRenderInfo);	
						RenderInfo.pAvFrame = pThis->m_pAvFrame;
						if (pThis->m_hFullScreen)
							RenderInfo.hPresentWnd = pThis->m_hFullScreen;
						else
							RenderInfo.hPresentWnd = NULL;
						
						dxva2_retrieve_data(pThis->m_pAvCodecCtx,pThis->m_pAvFrame);

						::SendMessageTimeout(pThis->m_hRecvMsg,WM_RENDERFRAME,(WPARAM)pThis->m_pDxSurface,(LPARAM)&RenderInfo,SMTO_BLOCK,250,(PDWORD_PTR)&nResult);
//						if (pThis->m_nEnableFullScreen >=0 )// 全屏渲染	
//						{
//							pThis->m_pDxSurface->Render(pThis->m_pAvFrame,pThis->m_hFullScreen);
// 							if (pThis->m_pConvertFullScreen)
// 							{								
// 								nAvError = pThis->m_pConvertFullScreen->ConertScale(pThis->m_pAvFrame,pThis->m_pFrameYUVFullScreen,pThis->m_nGQP);
// 								
// 								if (nAvError == 0)
// 								{
// 									av_strerror(nAvError, szAvError, 1024);		
// 									printf("sws_scale error:%s.\n",szAvError);
// 									continue;
// 								}
// 
// 								if (pThis->m_pConvertFullScreen->pImage)
// 								{
// 									DWORD dwTick1 = GetTickCount();
// 									DWORD dwTick2 = 0,dwTick3 = 0;
// 									pThis->m_pD3DRender->UpdateImage(pThis->m_hImageFullScreen, (BYTE *)pThis->m_pConvertFullScreen->pImage);
// 									dwTick2 = GetTickCount();
// 									pThis->m_pD3DRender->BeginRender(pThis->m_hCanvasFullScreen);
// 									static RECT rtFull = {0,0,_FullScreenHeight,_FullScreenWidth};
// 									pThis->m_pD3DRender->DrawImage(pThis->m_hImageFullScreen, 1.0f, GS_NONE/*, &rtFull*/);
// 									dwTick3 = GetTickCount();
// // 									for (int i = 0;i < _StaticText;i ++)
// // 										pThis->m_pD3DRender->DrawOverlayText (pThis->m_TextArray[i].hText, D3DCOLOR_ARGB(0xff,0xFF,0,0), &pThis->m_TextArray[i].rtText );
// 									
// 									TCHAR szText[64] = {0};	
// 									int nOsdTop = _OSD_TOP;
// 									pThis->m_pD3DRender->DrawDynamicText(_OSD_LEFT, nOsdTop, D3DCOLOR_XRGB(0xFF,0,0), pThis->m_szIpAddress);
// 
// 									nOsdTop += _OSD_H_INTERVAL;
// 									_stprintf_s(szText,64,_T("%s"),pThis->m_nCodecType == DVO_CODEC_H264?_T("H.264"):_T("H.265"));
// 									pThis->m_pD3DRender->DrawDynamicText(_OSD_LEFT, nOsdTop, D3DCOLOR_XRGB(0xFF,0,0), szText);
// 
// 									nOsdTop += _OSD_H_INTERVAL;
// 									int nStreamRate = pThis->m_pStreamInfo->GetVideoCodeRate();
// 									_stprintf_s(szText,64,_T("%d kbps"),nStreamRate);
// 									pThis->m_pD3DRender->DrawDynamicText(_OSD_LEFT, nOsdTop, D3DCOLOR_XRGB(0xFF,0,0), szText);
// 
// 									nOsdTop += _OSD_H_INTERVAL;
// 									int nFps = pThis->m_pStreamInfo->GetFrameRate();
// 									_stprintf_s(szText,64,_T("%d fps"),nFps);
// 									pThis->m_pD3DRender->DrawDynamicText(_OSD_LEFT, nOsdTop, D3DCOLOR_XRGB(0xFF,0,0), szText);
// 									
// 									nOsdTop += _OSD_H_INTERVAL;
// 									if (!pThis->m_pRecFile)
// 										_stprintf_s(szText,64,_T("RecFile:0 KB"),pThis->m_nRecFileLength);
// 									else
// 										_stprintf_s(szText,64,_T("RecFile:%d KB"),pThis->m_nRecFileLength/1024);
// 									pThis->m_pD3DRender->DrawDynamicText(_OSD_LEFT, nOsdTop, D3DCOLOR_XRGB(0xFF,0,0), szText);
// 
// 									nOsdTop += _OSD_H_INTERVAL;
// 									_stprintf_s(szText,64,_T("CacheSize:%d"),nQueueSize);
// 									pThis->m_pD3DRender->DrawDynamicText(_OSD_LEFT, nOsdTop, D3DCOLOR_XRGB(0xFF,0,0), szText);
// 
// 									nOsdTop += _OSD_H_INTERVAL;
// 									TCHAR szDecoder[64] = {0};
// 									switch(pThis->m_nDecodecManufacturer)
// 									{
// 									case FFMPEG:
// 										_tcscpy_s(szDecoder,64,_T("FFMPEG"));
// 										break;
// 									case HISILICON:
// 										_tcscpy_s(szDecoder,64,_T("HISILICON"));
// 										break;
// 									default:
// 									case DVO_HACCEL:
// 										_tcscpy_s(szDecoder,64,_T("DVO_HACCEL"));
// 										break;
// 									}
// 								
// 									_stprintf_s(szText,64,_T("Decoder:%s"),szDecoder);
// 									pThis->m_pD3DRender->DrawDynamicText(_OSD_LEFT, nOsdTop, D3DCOLOR_XRGB(0xFF,0,0), szText);
// 
// 									if (pThis->m_hFullScreen)
// 										pThis->m_pD3DRender->EndRender(pThis->m_hFullScreen) ;
// 									DWORD dwTick4 = GetTickCount();
// 									DWORD dwTimeSpan = GetTickCount() - dwTick1;
// 									DWORD dwUpdateImage = dwTick2 - dwTick1;
// 									DWORD dwDrawImage = dwTick3 - dwTick2;
// 									DWORD dwRender = dwTick4 - dwTick3;
// 									//TraceMsg(_T("TimeSpan = %d\tdwUpdateImage = %d\tdwDrawImage = %d\tdwRender = %d.\n"),dwTimeSpan,dwUpdateImage,dwDrawImage,dwRender);
// 								}
// 							}
// 						}
// 						else
// 						{
//							pThis->m_pDxSurface->Render(pThis->m_pAvFrame);
// 							nAvError = pThis->m_pConvertView->ConertScale(pThis->m_pAvFrame,pThis->m_pFrameYUV,pThis->m_nGQP);
// 							if (nAvError == 0)
// 							{
// 								av_strerror(nAvError, szAvError, 1024);		
// 								printf("sws_scale error:%s.\n",szAvError);
// 								continue;
// 							}
// 							if (pThis->m_pConvertView->pImage)
// 							{
// 								pThis->m_pD3DRender->UpdateImage(pThis->m_hImageView, (BYTE *)pThis->m_pConvertView->pImage);
// 								pThis->m_pD3DRender->BeginRender();
// 								pThis->m_pD3DRender->DrawImage(pThis->m_hImageView, 1.0f, GS_NONE );
// 								//pThis->m_pD3DRender->DrawDynamicText(2, 40, D3DCOLOR_XRGB(0xFF,0,0), "Test" ) ;
// 								pThis->m_pD3DRender->EndRender() ;
// 							}							
//						}	
					}	// if (nGot_picture)
				}	// if (Lock.TryLock(pThis->m_csD3DRender))
			}	// if pThis->m_pD3DRender

			else if (pThis->m_pDxSurface)
			{
				delete pThis->m_pDxSurface;
				pThis->m_pDxSurface = NULL;
			}
		}

		if (pThis->m_pDxSurface)
		{
			delete pThis->m_pDxSurface;
			pThis->m_pDxSurface = NULL;
		}
		av_free(pAvPacket);
		return 0;
	}

	// 创建一个简单的不可见的窗口
	HWND CreateSimpleWindow(IN HINSTANCE hInstance = NULL)
	{
		WNDCLASSEX wcex;
		TCHAR *szWindowClass = _T("FullScreen");
		wcex.cbSize			= sizeof(WNDCLASSEX);
		wcex.style			= CS_HREDRAW | CS_VREDRAW|CS_DBLCLKS;
		wcex.lpfnWndProc	= WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= szWindowClass;
		wcex.hIconSm		= NULL;
		RegisterClassEx(&wcex);
		HWND hWnd = ::CreateWindow(szWindowClass,// 窗口类
							szWindowClass,		// 窗口标题 
							WS_EX_TOPMOST | WS_POPUP,// 窗口风格
							CW_USEDEFAULT, 		// 窗口左上角X坐标
							0, 					// 窗口左上解Y坐标
							640, 				// 窗口宽度
							480, 				// 窗口高度
							NULL, 				// 父窗口句柄
							NULL,				// 菜单句柄
							hInstance, 			
							NULL);
		DWORD dwError = GetLastError();
		return hWnd;
	}

	bool StartPlayStream(PlayContext *pPc,Decoder_Manufacturer nManufacturer = HISILICON)
	{
		if (!m_pPlayContext)
			m_pPlayContext = std::tr1::shared_ptr<PlayContext>(pPc);
		
		m_hWndView = pPc->hVideoWnd;
		m_bRender = true;
		return true;
		
	}
	
	void StopPlayStream()
	{
		CAutoLock lock(&m_csD3DRender);
		m_bRender = false;
		while(m_pDxSurface)	// 等待线程销毁渲染对象
		{
			Sleep(10);
			continue;
		}
		if (m_nEnableFullScreen >= 0)
		{				
			::EnterCriticalSection(&m_csArrayFullScreen);
			if (m_nFullScreenInUsing >=1)
			{
				CFullScreen *pScreenInfo = &m_ArrayFullScreen[m_nEnableFullScreen];	
				::DestroyWindow(m_hFullScreen);
				m_hFullScreen = NULL;
				pScreenInfo->Reset();
				ShowWindow(pScreenInfo->GetWnd(),SW_HIDE);
				m_nFullScreenInUsing--;	
			}
			::LeaveCriticalSection(&m_csArrayFullScreen);
			m_nEnableFullScreen = -1;
		}
		FreeAvFrame(m_pAvFrame);
		FreeAvFrame(m_pHaccelFrame);
			
		DestroyDecoder();
// 		HDC hDC = ::GetDC(m_hWndView);
// 		RECT rt;
// 		GetWindowRect(m_hWndView,&rt);
// 		::SetBkColor(hDC, RGB(0,0,0));
// 		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rt, NULL, 0, NULL);
	}

	bool StartReceive(HWND hWndRecvMsg,DVO_CODEC nCodecType,int nStream)
	{		
		if (m_pStreamInfo != NULL)
			m_pStreamInfo->Reset();
		else
			m_pStreamInfo = std::tr1::shared_ptr<StreamInfo>(new StreamInfo());
		int nError = 0;
		m_nStreamNo = nStream;
		m_nCodecType = nCodecType;
		m_hStreamHandle = DVO_NET_StartRealPlay(m_hUserHandle,
												0,
												m_nStreamNo,
												DVO_TCP,
												0,
												NULL,
												(fnDVOCallback_RealAVData_T)StreamCallBack,
												(void *)this,
												&nError);
		if (m_hStreamHandle != -1)
		{
			m_bThreadRun = true;
			m_hRecvMsg = hWndRecvMsg;
			// 启动码流接收线程
			BeginWinThread(m_pThread,PlayThread,this,THREAD_PRIORITY_NORMAL);
			return true;
		}
		else
		{
			printf("DVO_NET_StartRealPlay Error:%d.\n",nError);
			return false;
		}
	}

	void StopReceive()
	{
		DVO_NET_StopRealPlay(m_hStreamHandle);
		m_hStreamHandle = -1;
		if (m_pThread)
		{
			m_bThreadRun = false;
			WaitForSingleObject(m_pThread->m_hThread,INFINITE);
			delete m_pThread;
			m_pThread = NULL;
		}
		// 停止播放
		StopPlayStream();
		// 如果正在录像，则终止录像
		StopRecord();
	}

	void StartRecord(TCHAR *szFileName,int nRecTime = 5*60)
	{
		try
		{
			CAutoLock lock(&m_csRecFile);
			m_bRecIFrame = false;
			m_tRecStartTime = time(NULL);
// 			if (fopen_s(&m_pRecFile,szFileName,"wb+"))
// 			{
// 				_tcscpy_s(m_szFileName,MAX_PATH,szFileName);
// 				return true;
// 			}
// 			else
// 				return false;
			m_pRecFile = new CFile(szFileName,CFile::modeCreate|CFile::modeWrite);
			_tcscpy_s(m_szFileName,MAX_PATH,szFileName);
		}			
		catch (/*std::exception* e*/CException *e)
		{
 			TCHAR szError[256] = {0};
 			e->GetErrorMessage(szError,256);
			printf(_T("%s %d Exception:%s.\n"),__FILE__,__LINE__,szError/*e->what()*/);
			e->Delete();
		}
//		return false;
	}

	void StopRecord()
	{
		if (!m_pRecFile)
			return;
		try
		{	
			CAutoLock lock(&m_csRecFile);
			//fclose(m_pRecFile);
			delete m_pRecFile;
			m_pRecFile = NULL;
			ZeroMemory(m_szFileName,sizeof(m_szFileName));
			m_nRecFileLength = 0;
			m_tRecStartTime = 0;
		}
		catch (/*std::exception* e*/CException *e)
		{
 			TCHAR szError[256] = {0};
 			e->GetErrorMessage(szError,256);
			printf(_T("%s %d Exception:%s.\n"),__FILE__,__LINE__,szError/*e->what()*/);
			e->Delete();
			m_pRecFile = NULL;
		}
	}

// 	bool CreateRender(HWND hViewWnd,int nWidth,int nHeight)
// 	{
// 		CVideoRender *pRender = NULL;
// 		bool bRetVal = false;
// 		__try
// 		{
// 			pRender = VideoRender::CreateD3DRender();
// 			if (!pRender)
// 				__leave;		
// 
// 			m_hCanvasView = pRender->CreateCanvas (hViewWnd, nWidth,nHeight) ;
// 			if (m_hCanvasView == NULL )
// 			{
// 				printf("Failed in CreateCanvas.\n");
// 				__leave;
// 			}
// 			m_hCanvasFullScreen = pRender->CreateCanvas (hViewWnd, _FullScreenWidth,_FullScreenHeight) ;
// 			m_hWndView = hViewWnd;
// 			CRect rtWnd ;
// 			::GetWindowRect(hViewWnd,&rtWnd);
// 			int nCenterX = rtWnd.left + rtWnd.Width() /2;
// 			int nCenterY = rtWnd.top + rtWnd.Height() /2;
// 			rtWnd.left = nCenterX - nWidth/2;
// 			rtWnd.right = nCenterX + nWidth/2;
// 			rtWnd.top = nCenterY - nHeight/2;
// 			rtWnd.bottom = nCenterY + nHeight/2;
// 			if (rtWnd.Width() < nWidth)
// 				rtWnd.right += (nWidth - rtWnd.Width());
// 			else if (rtWnd.Width()> nWidth)
// 				rtWnd.right -= (rtWnd.Width() - nWidth);
// 			if (rtWnd.Height() < nHeight)
// 				rtWnd.bottom += (nHeight - rtWnd.Height());
// 			else
// 				rtWnd.bottom -= (rtWnd.Height() - nHeight);
// 
// 			//m_hImageView = pRender->CreateImage (nWidth,nHeight, PF_I420, &rtWnd ) ;
// 			m_hImageView = pRender->CreateImage (nWidth,nHeight, PF_I420, &rtWnd ) ;
// 			if (m_hImageView == NULL )
// 			{
// 				printf("Failed in CreateImage.\n");
// 				__leave;
// 			}
// 			CRect rtFull(0,0,_FullScreenWidth,_FullScreenHeight);
// 			m_hImageFullScreen = pRender->CreateImage (_FullScreenWidth,_FullScreenHeight, PF_I420, &rtFull ) ;
// 			//m_hImageFullScreen = pRender->CreateImage (_FullScreenWidth,_FullScreenHeight, PF_NV12, &rtFull ) ;
// 			if (m_hImageFullScreen == NULL )
// 			{
// 				printf("Failed in CreateImage.\n");
// 				__leave;
// 			}
// 			// 创建字体
// // 			LOGFONT font ;
// // 			ZeroMemory (&font,sizeof(font));
// // 			font.lfHeight		 = 48 ;
// // 			font.lfWidth		 = 48 ;
// // 			font.lfEscapement	 = 0 ;
// // 			font.lfOrientation	 = 0 ;
// // 			font.lfWeight		 = FW_BOLD ;
// // 			font.lfItalic		 = FALSE ;
// // 			font.lfUnderline	 = FALSE ;
// // 			font.lfStrikeOut	 = FALSE ;
// // 			font.lfCharSet		 = DEFAULT_CHARSET ;
// // 			font.lfOutPrecision	 = OUT_DEFAULT_PRECIS ;
// // 			font.lfClipPrecision = CLIP_DEFAULT_PRECIS ;
// // 			font.lfQuality		 = ANTIALIASED_QUALITY ;
// // 			font.lfPitchAndFamily= VARIABLE_PITCH ;
// // 			strcpy_s ( font.lfFaceName, LF_FACESIZE, "宋体");
// // 			TCHAR szCodec[32] = {0};
// // 			_stprintf_s(szCodec,32,_T("%s"),m_nCodecType == DVO_CODEC_H264?_T("H.264"):_T("H.265"));
// // 			CHAR *szText[] = {m_szIpAddress,szCodec,"静态文本3","静态文本4"};
// // 
// // 			for (int i = 0;i < _StaticText;i ++)
// // 			{
// // 				if ((m_TextArray[i].hText = pRender->CreateOverlayText (szText[i], &font )) == NULL)
// // 				{
// // 					printf("Failed in CreateOverlayText.\n");
// // 					__leave;
// // 				}
// // 
// // 				if (!pRender->GetOverlayTextWH (m_TextArray[i].hText, &m_TextArray[i].nTextW, &m_TextArray[i].nTextH))
// // 				{
// // 					printf("Failed in GetOverlayTextWH.\n");
// // 					__leave;
// // 				}
// // 				m_TextArray[i].rtText.left		= _OSD_LEFT;//_FullScreenWidth - m_TextArray[i].nTextW/2;
// // 				m_TextArray[i].rtText.top		= i*60;
// // 				m_TextArray[i].rtText.right		= _OSD_LEFT +  m_TextArray[i].nTextW;//_FullScreenWidth;
// // 				m_TextArray[i].rtText.bottom	= m_TextArray[i].rtText.top + m_TextArray[i].nTextH;
// // 			}
// 
// 			if (!pRender->CreateDynamicFont ("宋体", 48 ))
// 			{
// 				printf("Failed in CreateDynamicFont.\n");
// 				__leave;
// 			}
// 
// 			if (!m_pD3DRender)
// 				m_pD3DRender = pRender;
// 			return true;
// 		}
// 		__finally
// 		{
// 			if (!m_pD3DRender && pRender)
// 			{
// 				VideoRender::DestroyD3DRender(pRender);
// 				bRetVal = false;
// 			}
// 			else
// 				bRetVal = true;
// 		}
// 		return bRetVal;
// 	}
	static void InitFullScreenInfo()
	{
		::InitializeCriticalSection(&m_csArrayFullScreen);
		ZeroMemory(m_ArrayFullScreen,sizeof(m_ArrayFullScreen));
		m_nFullScreenInUsing = 0;
	}

	static void SetFullScreenInfo(CVideoPlayer *pDecoder)
	{		
		HWND hWndFullScreen = pDecoder->CreateSimpleWindow(theApp.m_hInstance);		
		pDecoder->m_hFullScreen = hWndFullScreen;
		POINT pt;
		pt.x = 0;
		pt.y = 0;
		MoveWnd2Monitor(hWndFullScreen,pDecoder->m_nMonitorIndex,pt);
		::EnterCriticalSection(&m_csArrayFullScreen);
		m_ArrayFullScreen[m_nFullScreenInUsing].SetWnd(hWndFullScreen);
		m_ArrayFullScreen[m_nFullScreenInUsing].pUserPtr = pDecoder;
		m_ArrayFullScreen[m_nFullScreenInUsing].ToggleFullScreen();
		pDecoder->m_nEnableFullScreen = m_nFullScreenInUsing;
		m_nFullScreenInUsing ++;
		::LeaveCriticalSection(&m_csArrayFullScreen);
		pDecoder->ResetDxSurface(hWndFullScreen);
		::ShowWindow(hWndFullScreen,SW_SHOW);
	}
	bool ResetDxSurface(HWND hWnd = NULL)
	{
		if (m_pDxSurface)
		{
			if (!hWnd)
			{
				if (m_pDxSurface->ResetDevice())
					return m_pDxSurface->RestoreDevice();
				else
					return false;
			}
			else
			{
				if (m_DxInitInfo.nSize == sizeof(DxSurfaceInitInfo))
				{
					m_pDxSurface->DxCleanup();
					return m_pDxSurface->InitD3D( hWnd,
												  m_DxInitInfo.nFrameWidth,
												  m_DxInitInfo.nFrameHeight,
												  TRUE,
												  m_DxInitInfo.nD3DFormat);
				}
				else
					return false;
			}
		}
		else
			return false;

	}
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SYSKEYDOWN:
			{
			}
			break;
		case WM_KEYDOWN:
			{
				switch(wParam)	// 忽略ESC和回车键
				{
				case VK_ESCAPE:
				case VK_RETURN:
					{
						//printf(_T("Capture Escape or Return Key.\n"));
						return 0;
					}
					break;
				}
			}
			break;
		case WM_LBUTTONDBLCLK:	// 双击恢复监视
			{
						
				CVideoPlayer *pPlayer = NULL;
				int nIndex = -1;
				::EnterCriticalSection(&m_csArrayFullScreen);
				for (int i = 0;i < _Max_Monitors;i ++)
				{
					if (m_ArrayFullScreen[i].GetWnd() == hWnd)
					{
						pPlayer = (CVideoPlayer *)m_ArrayFullScreen[i].pUserPtr;
						nIndex = i;
						break;
					}
				}
				::LeaveCriticalSection(&m_csArrayFullScreen);
					
				if (!pPlayer ||nIndex == -1)
					return 0;
				pPlayer->ResetDxSurface(pPlayer->m_hWndView);
				ShowWindow(hWnd,SW_HIDE);
				DestroyWindow(hWnd);	// 销毁窗口	
				CFullScreen *pScreenCtrl = NULL;
				pPlayer->m_nEnableFullScreen = -1;	
				pPlayer->m_hFullScreen = NULL;
				::EnterCriticalSection(&m_csArrayFullScreen);
				if (m_nFullScreenInUsing >=1)
				{
					m_ArrayFullScreen[nIndex].Reset();					
					m_nFullScreenInUsing--;	
				}
				::LeaveCriticalSection(&m_csArrayFullScreen);
				return 0;
			}		
		default:
				return DefWindowProc(hWnd, message, wParam, lParam);		// 必须要有这一句，不然窗口可能无法创建成功
		}
		return 0l;
	}

	// 码流捕捉回调函数
	void static __stdcall StreamCallBack(PARAM_IN USER_HANDLE  lUserID,
										PARAM_IN REAL_HANDLE lStreamHandle,
										PARAM_IN int         nErrorType,
										PARAM_IN const char* pBuffer,
										PARAM_IN int         nDataLen,
										PARAM_IN void*       pUser)
	{
		//return ;
		CVideoPlayer *pThis = (CVideoPlayer *)pUser;	
		app_net_tcp_enc_stream_head_t *pStreamHeader = (app_net_tcp_enc_stream_head_t *)pBuffer;
		if (!pThis->m_pStreamInfo->bRecvIFrame ) // 从未收到过I帧
		{
			if (pStreamHeader->frame_type != 0 &&	// 海思I帧号为0，这是固件的一个BUG，有待修正
				pStreamHeader->frame_type != APP_NET_TCP_COM_DST_I_FRAME &&
				pStreamHeader->frame_type != APP_NET_TCP_COM_DST_IDR_FRAME)	//当前帧不是I帧
			{
				switch(pStreamHeader->frame_type)
				{
				case APP_NET_TCP_COM_DST_IDR_FRAME:		// IDR帧
					printf("Recv a IDR Frame, discarded.\n");
					break;			
				case APP_NET_TCP_COM_DST_P_FRAME:       // P帧
					printf("Recv a P Frame, discarded.\n");
					break;
				case APP_NET_TCP_COM_DST_B_FRAME:       // B帧
					printf("Recv a B Frame, discarded.\n");
					break;
				}			
				return ;
			}
			else
			{
// 				if (!g_hStdout)
// 					printf("Recv the First I Frame Now begin decoding.\n");
// 				else
				printf("Recv the First I Frame Now begin decoding.\n");
				pThis->m_pStreamInfo->bRecvIFrame = true;
				pThis->m_pStreamInfo->tFirstTime = (time_t)(GetExactTime()*1000);//time(0);
			}
		}
		// 解码必须从I帧开始
		int nSizeofHeader = sizeof(app_net_tcp_enc_stream_head_t);
		byte *pFrameData = (byte *)(pStreamHeader) + nSizeofHeader;

		// 得到协议帧头
		MSG_HEAD *pPkHeader = (MSG_HEAD *)((byte *)pStreamHeader- sizeof(MSG_HEAD));
		//_TraceProtocol((byte *)pPkHeader,sizeof(MSG_HEAD));
		// 错误 得到的指针数据是 nSizeofHeader*(nSizeofHeader-1)之后的数据
		// byte *pFrameData1 = (byte *)(pStreamHeader + nSizeofHeader);

		int nFrameType = pStreamHeader->frame_type;
		UINT nPkLen = ntohl(pPkHeader->Pktlen);
		int nFrameLength = nDataLen - sizeof(app_net_tcp_enc_stream_head_t);
// 		printf("PackLen = %d(%08X)\tStreamLen = %d(%08X)\tFrameLen = %d(%08X)\n",
// 				nPkLen,nPkLen,
// 				nDataLen,nDataLen,
// 				nFrameLength,nFrameLength);
		//printf("Frame Type = %d\tFrameLen = %d\n",nFrameType,nFrameLength);
	
		pThis->m_pStreamInfo->nFrameID = pStreamHeader->frame_num;
		pThis->m_pStreamInfo->tLastTime = (time_t)(GetExactTime()*1000); //time(0)	/*pStreamHeader->sec*1000 + pStreamHeader->usec*/;
		double dfTLast = GetExactTime();

		if (pStreamHeader->frame_type == 0 ||	// 海思I帧号为0，这是固件的一个BUG，有待修正
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_IDR_FRAME||		//IDR帧
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_I_FRAME||			//I帧
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_P_FRAME||			//P帧
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_B_FRAME)			//B帧
		{
			pThis->m_pStreamInfo->PushFrameInfo(nFrameLength);
			pThis->m_pStreamInfo->nVideoFrameCount ++;
			pThis->m_pStreamInfo->nVideoBytes += nFrameLength;
			//DVOFramePtr pStream = boost::make_shared<DVOFrame>(pFrameData,nFrameType,nFrameLength,pStreamHeader->frame_num,pThis->m_pStreamInfo->tLastTime);	
			DVOFramePtr pStream = std::tr1::shared_ptr<DVOFrame>(new DVOFrame(pFrameData,nFrameType,nFrameLength,pStreamHeader->frame_num,pThis->m_pStreamInfo->tLastTime));	
			CAutoLock lock(&pThis->m_csStreamList);
			pThis->m_StreamList.push_back(pStream);
		}
		else if (pStreamHeader->frame_type == APP_NET_TCP_COM_DST_711_ALAW||    //711 A律编码帧
				pStreamHeader->frame_type == APP_NET_TCP_COM_DST_711_ULAW||     //711 U律编码帧
				pStreamHeader->frame_type == APP_NET_TCP_COM_DST_726||          //726编码帧
				pStreamHeader->frame_type == APP_NET_TCP_COM_DST_AAC)			//AAC编码帧
		{
			pThis->m_pStreamInfo->nAudioBytes += nFrameLength;
			pThis->m_pStreamInfo->nAudioFrameCount ++;
		}

	}
public:
	DVO_CODEC					m_nCodecType;		// 编码类型
	int							m_nStreamNo;		// 码流序号

	// 画面显示相关变量
	std::tr1::shared_ptr<PlayContext>m_pPlayContext;
// 	CVideoRender				*m_pD3DRender;
// 	HCANVAS						m_hCanvasView;	
// 	HIMAGE						m_hImageView;
	HWND						m_hWndView;			// 监视窗口句柄，未全屏时，使用此窗口进行渲染
	UINT						m_nMonitorIndex;	// 显示器序号,0为第一显示器
	
	short						m_nEnableFullScreen;// 全屏窗口序号,若未启用全屏，则为-1
// 	HCANVAS						m_hCanvasFullScreen;		
// 	HIMAGE						m_hImageFullScreen;	
//	StaticTextInfo				m_TextArray[_StaticText];	
	HWND						m_hFullScreen;		// 全屏窗口句柄,发生全屏切换时，所有图像渲染到此窗口
	volatile  bool				m_bRender;			// 是否进行画面渲染

	
	// 网络连接相关变量
	TCHAR						m_szIpAddress[32];
	USER_HANDLE					m_hUserHandle;
	REAL_HANDLE					m_hStreamHandle;
	app_net_tcp_sys_logo_info_t	m_loginInfo;
	CRITICAL_SECTION			m_csRecFile;
	CRITICAL_SECTION			m_csStreamList;	
	
	// 录像相关变量
	CFile						*m_pRecFile;		// 录像文件
	//FILE*						m_pRecFile;
	bool						m_bRecIFrame;		// 是否已收到I帧
	int							m_nStreamRate;		// 码率 以KB/s为单位
	time_t						m_tRecStartTime;	// 录像起始时间
	long						m_nFileLength;		// 录像文件长度
	
	HWND						m_hRecvMsg;			// 收接解码线程发出消息的窗口
	int							m_nItem;
	int							m_nFraceCache;		// 缓存数量
	TCHAR						m_szFileName[MAX_PATH];
	ULONGLONG					m_nRecFileLength;
	bool volatile				m_bThreadRun;
	static CFullScreen			m_ArrayFullScreen[_Max_Monitors];
	static int					m_nFullScreenInUsing;// 正在使用的全屏显示器数量
	static CRITICAL_SECTION		m_csArrayFullScreen;
	CWinThread					*m_pThread;
	std::tr1::shared_ptr<StreamInfo>m_pStreamInfo;		// 视频流信息	
	
};


#endif	//_VIDEO_DECODE_2015_08_30