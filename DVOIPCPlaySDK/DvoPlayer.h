#pragma once
/////////////////////////////////////////////////////////////////////////
/// @file  DVOIPCPlaySDK.hpp
/// Copyright (c) 2015, 上海迪维欧科技
/// All rights reserved.  
/// @brief DVOIPC相机播放SDK实现
/// @version 1.0  
/// @author  xionggao.lee
/// @date  2015.12.17
///   
/// 修订说明：最初版本 
/////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <new>
#include <assert.h>
#include <list>
#include <memory>
#include <algorithm>
#include <windows.h>
#include <winsock2.h>
#include <process.h>
#include <thread>
#include <chrono>
#include <Shlwapi.h>
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")
#include "DVOMedia.h"
#include "DVOIPCPlaySDK.h"
#include "./DxSurface/DxSurface.h"
#include "AutoLock.h"
#include "./VideoDecoder/VideoDecoder.h"
#include "../dvoipcnetsdk/dvoipcnetsdk.h"
#include "TimeUtility.h"
#include "Utility.h"
#include "DSoundPlayer.hpp"
#include "AudioDecoder.h"

#ifdef _DEBUG
#define _New	new
#else
#define _New	new (std::nothrow)
#endif

using namespace std;
using namespace std::tr1;
#pragma comment(lib,"ws2_32")
#pragma warning (disable:4018)

#define FrameSize(p)	(((DVOFrameHeaderEx*)p)->nLength + sizeof(DVOFrameHeaderEx))	
#define Frame(p)		((DVOFrameHeaderEx *)p)

struct StreamFrame;

typedef shared_ptr<StreamFrame> StreamFramePtr;

class  CSimpleWnd
{
public:
	CSimpleWnd()
	{
		m_hWnd = CreateSimpleWindow();
		if (!m_hWnd)
			assert(false);
		ShowWindow(m_hWnd, SW_HIDE);
	}
	~CSimpleWnd()
	{
		if (m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd = nullptr;
		}
	}
	// 创建一个简单的不可见的窗口
	HWND CreateSimpleWindow(IN HINSTANCE hInstance = NULL)
	{
		WNDCLASSEX wcex;

		TCHAR *szWindowClass = _T("SimpleWnd");
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wcex.lpfnWndProc = DefWindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = NULL;
		RegisterClassEx(&wcex);
		int nWidth = GetSystemMetrics(SM_CXSCREEN);
		int nHeight = GetSystemMetrics(SM_CYSCREEN);
		return ::CreateWindow(szWindowClass,			// 窗口类
							  szWindowClass,			// 窗口标题 
							  WS_EX_TOPMOST | WS_POPUP,	// 窗口风格
							  CW_USEDEFAULT, 			// 窗口左上角X坐标
							  0, 						// 窗口左上解Y坐标
							  nWidth, 					// 窗口宽度
							  nHeight, 					// 窗口高度
							  NULL, 					// 父窗口句柄
							  NULL,						// 菜单句柄
							  hInstance,
							  NULL);
	}
	const HWND &GetSafeHwnd()
	{
		if (!m_hWnd)
			assert(false);
		return m_hWnd;
	}
private:
	HWND m_hWnd;
};


/// @struct StreamFrame
/// StreamFrame仅用于流媒体播放的数据帧
struct StreamFrame
{
	StreamFrame()
	{
		ZeroMemory(this, sizeof(StreamFrame));
	}
	/// @brief	接收已封装好的带DVOFrameHeaderEx头的数据
	StreamFrame(byte *pBuffer, int nLenth)
	{
		assert(pBuffer != nullptr);
		assert(nLenth >= sizeof(DVOFrameHeaderEx));
		nSize = nLenth;
		pInputData = _New byte[nLenth];
		if (pInputData)
			memcpy(pInputData, pBuffer, nLenth);
	}
	/// @brief	接收来自相机或其它实时码流的数据
	StreamFrame(IN byte *pFrame, IN int nFrameType, IN int nFrameLength, int nFrameNum, time_t nFrameTime )
	{
		nSize = sizeof(DVOFrameHeaderEx) + nFrameLength + 1;
		pInputData = _New byte[nSize ];
		if (!pInputData)
		{
			assert(false);
			return;
		}

		DVOFrameHeaderEx *pHeader = (DVOFrameHeaderEx *)pInputData;	
#ifdef _DEBUG
		ZeroMemory(pHeader, nSize);
#endif
		switch (nFrameType)
		{
		case 0:									// 海思I帧号为0，这是固件的一个BUG，有待修正
		case APP_NET_TCP_COM_DST_IDR_FRAME: 	// IDR帧。
		case APP_NET_TCP_COM_DST_I_FRAME:		// I帧。
			pHeader->nType = FRAME_I;
			break;
		case APP_NET_TCP_COM_DST_P_FRAME:       // P帧
		case APP_NET_TCP_COM_DST_B_FRAME:       // B帧
		case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A律编码帧
		case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U律编码帧
		case APP_NET_TCP_COM_DST_726:           // 726编码帧
		case APP_NET_TCP_COM_DST_AAC:           // AAC编码帧。
			pHeader->nType = nFrameType; 
			break;
		default:
		{
			assert(false);
			break;
		}
		}
		pHeader->nLength		 = nFrameLength;
		pHeader->nType			 = nFrameType;
		pHeader->nTimestamp		 = nFrameTime;
		pHeader->nFrameTag		 = DVO_TAG;		///< DVO_TAG
		if (!nFrameTime)
			pHeader->nFrameUTCTime = time(NULL);
		else
			pHeader->nFrameUTCTime = nFrameTime;
		
		pHeader->nFrameID		 = nFrameNum;		
		memcpy(pInputData + sizeof(DVOFrameHeaderEx), pFrame, nFrameLength);
#ifdef _DEBUG
		//DxTraceMsg("%s pInputData = %08X size = %d.\n", __FUNCTION__, (long)pInputData,nSize);
#endif
	}

	inline const DVOFrameHeaderEx* FrameHeader()
	{
		return (DVOFrameHeaderEx*)pInputData;
	}
	inline const byte *Framedata()
	{
		return (pInputData + sizeof(DVOFrameHeaderEx));
	}
	~StreamFrame()
	{
		if (pInputData)
		{
			delete[]pInputData;
		}
#ifdef _DEBUG
		//DxTraceMsg("%s pInputData = %08X.\n", __FUNCTION__, (long)pInputData);
		//ZeroMemory(this, sizeof(DVOFrameHeaderEx));
#endif
	}
	int		nSize;	
	byte	*pInputData;
	static bool IsIFrame(StreamFramePtr FramePtr)
	{
		//if (FramePtr->FrameHeader()->nType == FRAME_I)
		//TraceMsgA("%s Get an  Frame [%d].\n", __FUNCTION__, FramePtr->FrameHeader()->nType);
		return (FramePtr->FrameHeader()->nType == 0
				||FramePtr->FrameHeader()->nType == FRAME_IDR 
				|| FramePtr->FrameHeader()->nType == FRAME_I);
	}
};

// class IsIFrame
// {
// public:
// 	bool operator()(StreamFramePtr FramePtr)
// 	{
// 		return (FramePtr->FrameHeader()->nType == FRAME_I);	
// 	}
// };


#define _Frame_PERIOD			30.0f		///< 一个帧率区间

/// @brief 解析DVO私有帧结构体
struct FrameParser
{
	DVOFrameHeaderEx	*pHeaderEx;		///< DVO私有录像的帧数据
	UINT				nFrameSize;		///< pFrame的数据长度
	byte				*pLawFrame;		///< 原始码流数据
	UINT				nLawFrameSize;	///< 原始码流数据长度
};

/// @brief 线程休息试
enum SleepWay
{
	Sys_Sleep = 0,		///< 直接调用系统函数Sleep
	Wmm_Sleep = 1,		///< 使用多媒体时间提高精度
	Std_Sleep = 2		///< 使C++11提供的线程休眠函数
};

struct FileFrameInfo
{
	UINT	nOffset;	///< 帧数据所在的文件偏移
	UINT	nFrameSize;	///< 包含 DVOFrameHeaderEx在内的帧数据的尺寸
	bool	bIFrame;	///< 是否I帧
	time_t	tTimeStamp;
};
class CDvoPlayer
{
public:
	int		nSize;
private:
	list<StreamFramePtr>::iterator ItLoop;
	list<StreamFramePtr>m_listAudioCache;///< 流播放帧缓冲
	list<StreamFramePtr>m_listVideoCache;///< 流播放帧缓冲	
	CRITICAL_SECTION	m_csVideoCache;
	CRITICAL_SECTION	m_csAudioCache;	
	int					m_nMaxFrameCache;///< 最大视频缓冲数量,默认值125
										///< 当m_FrameCache中的视频帧数量超过m_nMaxFrameCache时，便无法再继续输入流数据
private:
	// 视频播放相关变量
	int			m_nTotalFrames;			///< 当前文件中有效音频帧的数量,仅当播放文件时有效
	HWND		m_hWnd;					///< 播放视频的窗口句柄
	bool		m_bProbeMode;			///< 是否牌探测码流模式，在探测模式下，不会删除I帧后的码流
	bool		m_bIFrameRecved;		///< 是否已经收到第一个I帧
	int			m_nDisardFrames;		///< 收到第一个I帧前，被抛弃的帧数量
	int			m_nFrameOffset;			///< 视频帧复制时的帧内偏移
	volatile bool m_bIpcStream;			///< 输入流为IPC流

	DVO_CODEC	m_nVideoCodec;			///< 视频编码格式 @see DVO_CODEC	
	static		CAvRegister avRegister;	
	static shared_ptr<CDSound> m_pDsPlayer;///< DirectSound播放对象指针
	//shared_ptr<CDSound> m_pDsPlayer;	///< DirectSound播放对象指针
	CDSoundBuffer* m_pDsBuffer;
	CDxSurface* m_pDxSurface;			///< Direct3d Surface封装类,用于显示视频
	shared_ptr<CVideoDecoder>m_pDecodec;
	static shared_ptr<CSimpleWnd>m_pWndDxInit;///< 视频显示时，用以初始化DirectX的隐藏窗口对象
	bool		m_bRefreshWnd;			///< 停止播放时是否刷新画面
	int			m_nVideoWidth;			///< 视频宽度
	int			m_nVideoHeight;			///< 视频高度	
	int			m_nFrameEplased;		///< 已经播放帧数
	int			m_nCurVideoFrame;		///< 当前正播放的视频帧ID
	time_t		m_tCurFrameTimeStamp;
	time_t		m_tLastFrameTime;
	USHORT		m_nPlayFPS;				///< 播放时帧率
	USHORT		m_nPlayFrameInterval;	///< 播放时帧间隔
	double		m_dfLastTimeVideoPlay;	///< 前一次视频播放的时间
	double		m_dfTimesStart;			///< 开始播放的时间
	bool		m_bEnableHaccel;		///< 是否启用硬解码
	bool		m_bFitWindow;			///< 视频显示是否填满窗口
										///< 为true时，则把视频填满窗口,这样会把图像拉伸,可能会造成图像变形
										///< 为false时，则只按图像原始比例在窗口中显示,超出比例部分,则以黑色背景显示
private:	// 音频播放相关变量

	DVO_CODEC	m_nAudioCodec;			///< 音频编码格式 @see DVO_CODEC
	bool		m_bEnableAudio;			///< 是否启用音频播放
	DWORD		m_dwAudioOffset;		///< 音频缓中区中的偏移地址
	DWORD		m_dwAudioBuffLength;	///< 
	int			m_nNotifyNum;			///< 音频分区段数量
	double		m_dfLastTimeAudioPlay;	///< 前一次播放音频的时间
	double		m_dfLastTimeAudioSample;///< 前一次音频采样的时间
	int			m_nAudioFrames;			///< 当前缓存中音频帧数量
	int			m_nCurAudioFrame;		///< 当前正播放的音频帧ID
private:
	HANDLE		m_hThreadParser;		///< 解析DVO私有格式录像的线程
	HANDLE		m_hThreadPlayVideo;		///< 视频解码和播放线程
	HANDLE		m_hThreadPlayAudio;		///< 音频解码和播放线程
	//HANDLE		m_hThreadGetFileSummary;	///< 文件信息摘要线程
	volatile bool m_bThreadParserRun;
	volatile bool m_bThreadPlayVideoRun;
	volatile bool m_bThreadPlayAudioRun;
	byte*		m_pParserBuffer;		///< 数据解析缓冲区
	UINT		m_nParserBufferSize;	///< 数据解析缓冲区尺寸
	DWORD		m_nParserDataLength;	///< 数据解析缓冲区中的有效数据长度
	UINT		m_nParserOffset;		///< 数据解析缓冲区尺寸当前已经解析的偏移
	CRITICAL_SECTION m_csParser;		
	//volatile bool m_bThreadFileAbstractRun;
	bool		m_bPause;				///< 是否处于暂停状态
	byte		*m_pYUV;				///< YVU捕捉专用内存
	int			m_nYUVSize ;			///<
	shared_ptr<byte>m_pYUVPtr;
private:	// 文件播放相关变量
	HANDLE		m_hDvoFile;				///< 正在播放的文件句柄
	shared_ptr<DVO_MEDIAINFO>m_pMediaHeader;/// 媒体文件头
	UINT		m_nFrametoRead;			///< 当前将要读取的视频帧ID
	char		*m_pszFileName;			///< 正在播放的文件名,该字段仅在文件播放时间有效
	FileFrameInfo	*m_pFrameOffsetTable;///< 视频帧ID对应文件偏移表
	volatile LONGLONG m_nSeekOffset;	///< 读文件的偏移
	CRITICAL_SECTION	m_csSeekOffset;
	float		m_fPlayRate;			///< 当前的播放的倍率,大于1时为加速播放,小于1时为减速播放，不能为0或小于0
	int			m_nSDKVersion;			///< 生成录像文件的SDK版本	
	int			m_nMaxFrameSize;		///< 最大I帧的大小，以字节为单位,默认值256K
	int			m_nFileFPS;				///< 文件中,视频帧的原始帧率
	USHORT		m_nFileFrameInterval;	///< 文件中,视频帧的原始帧间隔
	int			m_nPlayInterval;		///< 帧播放间隔,单位毫秒
private:
	CaptureFrame	m_pfnCaptureFrame;
	void*			m_pUserCaptureFrame;
	CaptureYUV		m_pfnCaptureYUV;
	void*			m_pUserCaptureYUV;
	CaptureYUVEx	m_pfnCaptureYUVEx;
	void*			m_pUserCaptureYUVEx;
	FilePlayProc	m_pFilePlayCallBack;
	void*			m_pUserFilePlayer;
	SleepWay		m_nSleepWay;		///< 线程休眠的方式,@see SleepWay
public:
	
private:
	CDvoPlayer()
	{
		ZeroMemory(&m_csVideoCache, sizeof(CDvoPlayer) - offsetof(CDvoPlayer, m_csVideoCache));
		InitializeCriticalSection(&m_csVideoCache);
		InitializeCriticalSection(&m_csAudioCache);
		InitializeCriticalSection(&m_csParser);
		m_nMaxFrameSize = 1024 * 256;
		nSize = sizeof(CDvoPlayer);
	}
	LONGLONG GetSeekOffset()
	{
		CAutoLock lock(&m_csSeekOffset);
		return m_nSeekOffset;
	}
	void SetSeekOffset(LONGLONG nSeekOffset)
	{
		CAutoLock lock(&m_csSeekOffset);
		m_nSeekOffset = nSeekOffset;
	}
	// 超大文件寻址
	__int64 LargerFileSeek(HANDLE hFile, __int64 nOffset64, DWORD MoveMethod)
	{
		LARGE_INTEGER Offset;
		Offset.QuadPart = nOffset64;
		Offset.LowPart = SetFilePointer(hFile, Offset.LowPart, &Offset.HighPart,MoveMethod);

		if (Offset.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
		{
			Offset.QuadPart = -1;
		}
		return Offset.QuadPart;
	}
	/// @brief 判断输入帧是否为DVO私有的视频帧
	/// @param[in]	pFrameHeader	DVO私有帧结构指针详见@see DVOFrameHeaderEx
	/// @param[out] bIFrame			判断输入帧是否为I帧
	/// -# true			输入帧为I帧
	///	-# false		输入帧为其它帧
	static bool IsDVOVideoFrame(DVOFrameHeaderEx *pFrameHeader,bool &bIFrame)
	{
		switch (pFrameHeader->nType)
		{
		case 0:
		case FRAME_IDR:
		case FRAME_I:
			bIFrame = true;
			return true;
		case FRAME_P:
		case FRAME_B:
			bIFrame = false;
			return true;
		default:
			return false;
		}
	}
	/// @brief 取得视频文件最一帧的ID,相当于视频文件中包含的视频总帧数
	int GetLastFrameID(int &nLastFrameID)
	{
		if (!m_hDvoFile)
			return DVO_Error_FileNotOpened;		
		byte *pBuffer = _New byte[m_nMaxFrameSize];
		shared_ptr<byte>TempBuffPtr(pBuffer);
		LARGE_INTEGER liFileSize;
		if (!GetFileSizeEx(m_hDvoFile, &liFileSize))
			return GetLastError();
		
		if (liFileSize.QuadPart >= m_nMaxFrameSize && 
			LargerFileSeek(m_hDvoFile, -m_nMaxFrameSize, FILE_END) == INVALID_SET_FILE_POINTER)
			return GetLastError();
		DWORD nBytesRead = 0;
		DWORD nDataLength = m_nMaxFrameSize;
		
		if (!ReadFile(m_hDvoFile, pBuffer, nDataLength, &nBytesRead, nullptr))
		{
			DxTraceMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
			return GetLastError();
		}
		nDataLength = nBytesRead;
		char *szKey = "MOVD";
		int nOffset = KMP_StrFind(pBuffer, nDataLength, (byte *)szKey, 4);
		if (nOffset < 0)
			return DVO_Error_MaxFrameSizeNotEnough;
		nOffset -= offsetof(DVOFrameHeaderEx, nFrameTag);
		if (nOffset < 0)
			return DVO_Error_NotDvoVideoFile;
		// 遍历所有最后m_nMaxFrameSize长度内的所有帧
		pBuffer += nOffset;
		nDataLength -= nOffset;
		bool bFoundVideo = false;
		
		FrameParser Parser;
#ifdef _DEBUG
		int nAudioFrames = 0;
		int nVideoFrames = 0;
		while (ParserFrame(&pBuffer, nDataLength, &Parser))
		{
			switch (Parser.pHeaderEx->nType)
			{
			case 0:
			case FRAME_B:
			case FRAME_I:
			case FRAME_P:
			{
				nVideoFrames++;
				nLastFrameID = Parser.pHeaderEx->nFrameID;
				bFoundVideo = true;
				break;
			}
			case FRAME_G711A:      // G711 A律编码帧
			case FRAME_G711U:      // G711 U律编码帧
			case FRAME_G726:       // G726编码帧
			case FRAME_AAC:        // AAC编码帧。
			{
				nAudioFrames ++;
				break;
			}
			default:
			{
				assert(false);
				break;
			}
			}

			nOffset += Parser.nFrameSize;
		}
		DxTraceMsg("%s In Last %d bytes:VideoFrames:%d\tAudioFrames:%d.\n", __FUNCTION__, m_nMaxFrameSize, nVideoFrames, nAudioFrames);
#else
		while (ParserFrame(&pBuffer, nDataLength, &Parser))
		{
			if (Parser.pHeaderEx->nType == FRAME_B ||
				Parser.pHeaderEx->nType == FRAME_I ||
				Parser.pHeaderEx->nType == FRAME_P)
			{
				nLastFrameID = Parser.pHeaderEx->nFrameID;
				bFoundVideo = true;
			}			
		}
#endif
		if (SetFilePointer(m_hDvoFile, sizeof(DVO_MEDIAINFO), nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			return GetLastError();
		if (bFoundVideo)
			return DVO_Succeed;
		else
			return DVO_Error_MaxFrameSizeNotEnough;
	}
	/// @brief 渲染一帧
	void RenderFrame(AVFrame *pAvFrame)
	{
		if (m_bFitWindow)
			m_pDxSurface->Render(pAvFrame, m_hWnd, nullptr);
		else
		{
			RECT rtRender;
			GetWindowRect(m_hWnd, &rtRender);
			POINT ptLeftTop, ptRightBottom;
			ptLeftTop.x = rtRender.left;
			ptLeftTop.y = rtRender.top;
			ptRightBottom.x = rtRender.right;
			ptRightBottom.y = rtRender.bottom;

			ScreenToClient(m_hWnd, &ptLeftTop);
			ScreenToClient(m_hWnd, &ptRightBottom);
			rtRender.left = ptLeftTop.x;
			rtRender.top = ptLeftTop.y;
			rtRender.right = ptRightBottom.x;
			rtRender.bottom = ptRightBottom.y;
			int nWndWidth = rtRender.right - rtRender.left;
			int nWndHeight = rtRender.bottom - rtRender.top;
			float fScaleWnd = (float)nWndWidth / nWndHeight;
			float fScaleVideo = (float)pAvFrame->width / pAvFrame->height;
			if (fScaleVideo > fScaleWnd)			// 窗口高度超出比例,需要去掉一部分高度,视频需要上下居中
			{
				int nNewHeight = (int)nWndWidth / fScaleVideo;
				int nOverHeight = nWndHeight - nNewHeight;
				if ((float)nOverHeight / nWndHeight > 0.1f)	// 窗口高度超出10%,则调整高度，否则忽略该差异
				{
					rtRender.top += nOverHeight / 2;
					rtRender.bottom -= nOverHeight / 2;
				}
			}
			else if (fScaleVideo < fScaleWnd)		// 窗口宽度超出比例,需要去掉一部分宽度，视频需要左右居中
			{
				int nNewWidth = nWndHeight*fScaleVideo;
				int nOverWidth = nWndWidth - nNewWidth;
				if ((float)nOverWidth / nWndWidth > 0.1f)
				{
					rtRender.left += nOverWidth / 2;
					rtRender.right -= nOverWidth / 2;
				}
			}
			m_pDxSurface->Render(pAvFrame, m_hWnd, &rtRender);
		}
	}
	// 二分查找
	int BinarySearch(time_t tTime)
	{
		int low = 0;
		int high = m_nTotalFrames - 1;
		int middle = 0;
		while (low <= high)
		{
			middle = (low + high) / 2;
			if (tTime >= m_pFrameOffsetTable[middle].tTimeStamp &&
				tTime <= m_pFrameOffsetTable[middle + 1].tTimeStamp)
				return middle;
			//在左半边
			else if (tTime < m_pFrameOffsetTable[middle].tTimeStamp)
				high = middle - 1;
			//在右半边
			else if (tTime > m_pFrameOffsetTable[middle + 1].tTimeStamp)
				low = middle + 1;
		}
		//没找到
		return -1;
	}
public:
	void SetPlayCallBack(FilePlayProc pFilePlayCallBack,void *pUserPtr)
	{
		m_pFilePlayCallBack = pFilePlayCallBack;
		m_pUserFilePlayer = pUserPtr;
	}

	CDvoPlayer(HWND hWnd,CHAR *szFileName =  NULL)
	{
// 		assert(hWnd != nullptr);
// 		assert(IsWindow(hWnd));
		ZeroMemory(&m_csVideoCache, sizeof(CDvoPlayer) - offsetof(CDvoPlayer, m_csVideoCache));
		nSize = sizeof(CDvoPlayer);
		InitializeCriticalSection(&m_csVideoCache);
		InitializeCriticalSection(&m_csAudioCache);
		InitializeCriticalSection(&m_csSeekOffset);
		InitializeCriticalSection(&m_csParser);
		
		m_nMaxFrameSize	 = 1024 * 256;
		m_nFileFPS		 = 25;				// FPS的默认值为25
		m_fPlayRate		 = 1;
		m_nPlayInterval	 = 40;
		m_nVideoCodec	 = CODEC_H264;		// 视频默认使用H.264编码
		m_nAudioCodec	 = CODEC_G711U;		// 音频默认使用G711U编码
#ifdef _DEBUG
		m_nMaxFrameCache = 200;				// 默认最大视频缓冲数量为200
#else
		m_nMaxFrameCache = 100;				// 默认最大视频缓冲数量为100
#endif
		nSize = sizeof(CDvoPlayer);
		if (szFileName)
		{
			m_pszFileName = _New char[MAX_PATH];
			strcpy(m_pszFileName, szFileName);
			// 打开文件
			m_hDvoFile = CreateFileA(m_pszFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_ARCHIVE,
				NULL);
			if (m_hDvoFile != INVALID_HANDLE_VALUE)
			{
				// 取得文件的概要的信息,如文件总帧数,文件偏移表
				int nError = GetFileSummary();
				if (nError != DVO_Succeed)
					SetLastError(nError);
			}
			m_nParserBufferSize	 = m_nMaxFrameSize * 4;
			m_pParserBuffer		 = new byte[m_nParserBufferSize];
		}
		if (hWnd && IsWindow(hWnd))
		{// 必采要有窗口才会播放视频和声音
			m_hWnd = hWnd;
			OSVERSIONINFOEX osVer;
			osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			GetVersionEx((OSVERSIONINFO *)&osVer);
			if (osVer.dwMajorVersion < 6)
				m_pDxSurface = _New CDxSurface();
 			else
 				m_pDxSurface = _New CDxSurfaceEx();
			//m_DsPlayer = _New CDSoundPlayer();
			//m_DsPlayer.Initialize(m_hWnd, Audio_Play_Segments);
		}

	}
	
	~CDvoPlayer()
	{
		StopPlay();
		/*
		if (m_hWnd)
		{
			if (m_bRefreshWnd)
			{
				PAINTSTRUCT ps;
				HDC hdc;
				hdc = ::BeginPaint(m_hWnd, &ps);
				HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
				::SetBkColor(hdc, RGB(0, 0, 0));
				RECT rtWnd;
				GetWindowRect(m_hWnd, &rtWnd);
				::ScreenToClient(m_hWnd, (LPPOINT)&rtWnd);
				::ScreenToClient(m_hWnd, ((LPPOINT)&rtWnd) + 1);
				if (GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
					swap(rtWnd.left, rtWnd.right);

				::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rtWnd, NULL, 0, NULL);
				::EndPaint(m_hWnd, &ps);
			}
		}
		*/
		if (m_pDsBuffer)
		{
			m_pDsPlayer->DestroyDsoundBuffer(m_pDsBuffer);
#ifdef _DEBUG
			m_pDsBuffer = nullptr;
#endif
		}
		if (m_pDxSurface)
		{
			delete m_pDxSurface;
#ifdef _DEBUG
			m_pDxSurface = nullptr;
#endif
		}
		m_listVideoCache.clear();
		if (m_pszFileName)
			delete[]m_pszFileName;
		if (m_hDvoFile)
		{
			CloseHandle(m_hDvoFile);
		}
		DeleteCriticalSection(&m_csVideoCache);
		DeleteCriticalSection(&m_csAudioCache);
		DeleteCriticalSection(&m_csSeekOffset);
		DeleteCriticalSection(&m_csParser);
	}
	/// @brief  是否为文件播放
	/// @retval			true	文件播放
	/// @retval			false	流播放
	bool IsFilePlayer()
	{
		if (m_hDvoFile || m_pszFileName)
			return true;
		else
			return false;
	}

	void SetRefresh(bool bRefresh = true)
	{
		m_bRefreshWnd = bRefresh;
	}

	// 设置流播放头
	// 在流播放时，播放之前，必须先设置流头
	int SetStreamHeader(CHAR *szStreamHeader, int nHeaderSize)
	{
		if (nHeaderSize != sizeof(DVO_MEDIAINFO))
			return DVO_Error_InvalidParameters;
		DVO_MEDIAINFO *pMediaHeader = (DVO_MEDIAINFO *)szStreamHeader;
		if (pMediaHeader->nMediaTag != DVO_TAG)
			return DVO_Error_InvalidParameters;
		m_pMediaHeader = make_shared<DVO_MEDIAINFO>();
		if (m_pMediaHeader)
		{
			memcpy(m_pMediaHeader.get(), szStreamHeader, sizeof(DVO_MEDIAINFO));
			return 0;
		}
		else
			return DVO_Error_InsufficentMemory;
	}

	int SetMaxFrameSize(int nMaxFrameSize = 256*1024)
	{
		if (m_hThreadParser || m_hThreadPlayVideo)
			return DVO_Error_PlayerHasStart;
		m_nMaxFrameSize = nMaxFrameSize;
		return DVO_Succeed;
	}

	inline int GetMaxFrameSize()
	{
		return m_nMaxFrameSize;
	}

	inline void SetMaxFrameCache(int nMaxFrameCache = 100)
	{
		m_nMaxFrameCache = nMaxFrameCache;
	}

	void ClearFrameCache()
	{
		EnterCriticalSection(&m_csVideoCache);
		m_listVideoCache.clear();
		LeaveCriticalSection(&m_csVideoCache);

		EnterCriticalSection(&m_csAudioCache);
		m_listAudioCache.clear();
		LeaveCriticalSection(&m_csAudioCache);
	}
		
	/// @brief			开始播放
	/// @param [in]		bEnaleAudio		是否开启音频播放
	/// #- true			开启音频播放
	/// #- false		关闭音频播放
	/// @param [in]		bEnableHaccel	是否开启硬解码功能
	/// #- true			启用硬解码
	/// #- false		禁用硬解码
	/// @param [in]		bFitWindow		视频是否适应窗口
	/// #- true			视频填满窗口,这样会把图像拉伸,可能会造成图像变形
	/// #- false		只按图像原始比例在窗口中显示,超出比例部分,则以黑色背景显示
	/// @retval			0	操作成功
	/// @retval			1	流缓冲区已满
	/// @retval			-1	输入参数无效
	/// @remark			播放流数据时，相应的帧数据其实并未立即播放，而是被放了播放队列中，应该根据dvoplay_PlayStream
	///					的返回值来判断，是否继续播放，若说明队列已满，则应该暂停播放
	int StartPlay(bool bEnaleAudio = false,bool bEnableHaccel = false,bool bFitWindow = true)
	{
		m_bPause = false;
		m_bFitWindow = bFitWindow;
		if (!m_hWnd || !IsWindow(m_hWnd))
		{
			return DVO_Error_InvalidWindow;
		}
		if (m_pszFileName )
		{
			if (m_hDvoFile == INVALID_HANDLE_VALUE)
			{
				return GetLastError();
			}
 			if (m_pDxSurface)
 				m_pDxSurface->DisableVsync();		// 禁用垂直同步，播放帧才有可能超过显示器的刷新率，从而达到高速播放的目的
			
			// 启动文件解析线程
			m_bThreadParserRun = true;
			//m_hThreadParser = (HANDLE)_beginthreadex(nullptr, 0, ThreadParser, this, 0, 0);
			m_hThreadParser = CreateThread(nullptr, 0, ThreadParser, this, 0, 0);
		}
		else
		{
//  		if (!m_pMediaHeader)
//  			return DVO_Error_NotInputStreamHeader;		// 未设置流播放头

			m_listVideoCache.clear();
			m_listAudioCache.clear();
		}
		if (m_hWnd)
		{
			// 启动流播放线程
			m_bThreadPlayVideoRun = true;			
			//m_hThreadPlayVideo = (HANDLE)_beginthreadex(nullptr, 0, ThreadPlayVideo, this, 0, 0);
			m_hThreadPlayVideo = CreateThread(nullptr, 0, ThreadPlayVideo, this, 0, 0);
			
			if (bEnaleAudio)
			{
				if (m_pDsPlayer)
				{
					if (!m_pDsPlayer->IsInitialized())
						m_pDsPlayer->Initialize(m_hWnd, Audio_Play_Segments);
					m_pDsBuffer = m_pDsPlayer->CreateDsoundBuffer();
					if (m_pDsPlayer && m_pDsBuffer)
					{
						m_bThreadPlayAudioRun = true;
						//m_hThreadPlayAudio = (HANDLE)_beginthreadex(nullptr, 0, ThreadPlayAudio, this, CREATE_SUSPENDED, 0);
						m_hThreadPlayAudio = CreateThread(nullptr, 0, ThreadPlayAudio, this, CREATE_SUSPENDED, 0);
						m_pDsBuffer->StartPlay();
						m_bEnableAudio = true;
					}
					else
						m_bEnableAudio = false;
				}
			}
		}
		return DVO_Succeed;
	}

	int GetFileHeader()
	{
		if (!m_hDvoFile)
			return DVO_Error_FileNotOpened;
		DWORD dwBytesRead = 0;
		m_pMediaHeader = make_shared<DVO_MEDIAINFO>();
		if (!m_pMediaHeader)
		{
			CloseHandle(m_hDvoFile);
			return -1;
		}

		if (!ReadFile(m_hDvoFile, (void *)m_pMediaHeader.get(), sizeof(DVOFrameHeaderEx), &dwBytesRead, nullptr))
		{
			CloseHandle(m_hDvoFile);
			return GetLastError();
		}
		// 分析视频文件头
		if (m_pMediaHeader->nMediaTag != DVO_TAG ||
			dwBytesRead != sizeof(DVOFrameHeaderEx))
		{
			CloseHandle(m_hDvoFile);
			return DVO_Error_NotDvoVideoFile;
		}

		switch (m_pMediaHeader->nSDKversion)
		{
		case DVO_IPC_SDK_VERSION_2015_09_07:
			if (m_pMediaHeader->nFps != 0 && m_pMediaHeader->nFps != 1)
				m_nFileFPS = m_pMediaHeader->nFps;
			break;
		case DVO_IPC_SDK_VERSION_2015_10_20:
		case DVO_IPC_SDK_VERSION_2015_12_16:
			//m_nFPS = m_pMediaHeader->nFps;		// FPS不再从文件头中获得,而是从帧间隔中推算
			break;
		default:
			return DVO_Error_InvalidSDKVersion;
		}
		return DVO_Succeed;
	}
	
	void FitWindow(bool bFitWindow)
	{
		m_bFitWindow = bFitWindow;
	}

	/// @brief			输入DVO私格式的码流数据
	/// @retval			0	操作成功
	/// @retval			1	流缓冲区已满
	/// @retval			-1	输入参数无效
	/// @remark			播放流数据时，相应的帧数据其实并未立即播放，而是被放了播放队列中，应该根据dvoplay_PlayStream
	///					的返回值来判断，是否继续播放，若说明队列已满，则应该暂停播放
	int InputStream(unsigned char *szFrameData, int nFrameSize)
	{
		if (!szFrameData || nFrameSize < sizeof(DVOFrameHeaderEx))
			return DVO_Error_InvalidFrame;
		m_bIpcStream = false;
		DVOFrameHeaderEx *pHeaderEx = (DVOFrameHeaderEx *)szFrameData;
		switch (pHeaderEx->nType)
		{
			case FRAME_P:
			case FRAME_B:
			case FRAME_I:
			{
// 				if (!m_hThreadPlayVideo)
// 					return DVO_Error_VideoThreadNotRun;
				CAutoLock lock(&m_csVideoCache);
				if (m_listVideoCache.size() >= m_nMaxFrameCache)
					return DVO_Error_FrameCacheIsFulled;
				StreamFramePtr pStream = make_shared<StreamFrame>(szFrameData, nFrameSize);
				if (!pStream)
					return DVO_Error_InsufficentMemory;
				m_listVideoCache.push_back(pStream);
				break;
			}
			case FRAME_G711A:      // G711 A律编码帧
			case FRAME_G711U:      // G711 U律编码帧
			case FRAME_G726:       // G726编码帧
			case FRAME_AAC:        // AAC编码帧。
			{
// 				if (!m_hThreadPlayVideo)
// 					return DVO_Error_AudioThreadNotRun;
				if (!m_bEnableAudio)
					break;
				StreamFramePtr pStream = make_shared<StreamFrame>(szFrameData, nFrameSize);
				if (!pStream)
					return DVO_Error_InsufficentMemory;

				::EnterCriticalSection(&m_csAudioCache);
				m_listAudioCache.push_back(pStream);
				m_nAudioFrames++;
				::LeaveCriticalSection(&m_csAudioCache);
				break;
			}
			default:
			{
				assert(false);
				return DVO_Error_InvalidFrameType;
				break;
			}
		}
		return DVO_Succeed;
	}

	/// @brief			输入DVO IPC码流数据
	/// @retval			0	操作成功
	/// @retval			1	流缓冲区已满
	/// @retval			-1	输入参数无效
	/// @remark			播放流数据时，相应的帧数据其实并未立即播放，而是被放了播放队列中，应该根据dvoplay_PlayStream
	///					的返回值来判断，是否继续播放，若说明队列已满，则应该暂停播放
	int InputStream(IN byte *pFrameData, IN int nFrameType, IN int nFrameLength, int nFrameNum, time_t nFrameTime)
	{
		m_bIpcStream = true;
		switch (nFrameType)
		{
			case 0:									// 海思I帧号为0，这是固件的一个BUG，有待修正
			case APP_NET_TCP_COM_DST_IDR_FRAME: 	// IDR帧。
			case APP_NET_TCP_COM_DST_I_FRAME:		// I帧。		
			case APP_NET_TCP_COM_DST_P_FRAME:       // P帧。
			case APP_NET_TCP_COM_DST_B_FRAME:       // B帧。
			{
				CAutoLock lock(&m_csVideoCache);
				if (m_listVideoCache.size() >= m_nMaxFrameCache)
					return DVO_Error_FrameCacheIsFulled;
				StreamFramePtr pStream = make_shared<StreamFrame>(pFrameData, nFrameType, nFrameLength, nFrameNum, nFrameTime);
				if (!pStream)
					return DVO_Error_InsufficentMemory;
				m_listVideoCache.push_back(pStream);
			}
				break;
			case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A律编码帧
			case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U律编码帧
			case APP_NET_TCP_COM_DST_726:           // 726编码帧
			case APP_NET_TCP_COM_DST_AAC:           // AAC编码帧。
			{
				m_nAudioCodec = (DVO_CODEC)nFrameType;
				if (!m_bEnableAudio)
					break;
				StreamFramePtr pStream = make_shared<StreamFrame>(pFrameData, nFrameType, nFrameLength, nFrameNum, nFrameTime);
				if (!pStream)
					return DVO_Error_InsufficentMemory;
				::EnterCriticalSection(&m_csAudioCache);
				m_nAudioFrames++;
				m_listAudioCache.push_back(pStream);
				::LeaveCriticalSection(&m_csAudioCache);
			}
				break;
			default:
			{
				assert(false);
				break;
			}
		}
		return 0;
	}

	void StopPlay()
	{
		m_bThreadParserRun = false;
		m_bThreadPlayVideoRun = false;
		m_bThreadPlayAudioRun = false;
		m_bPause = false;
		if (m_hThreadParser)
		{
			WaitForSingleObject(m_hThreadParser, INFINITE);			
			CloseHandle(m_hThreadParser);
			DxTraceMsg("%s ThreadParser has exit.\n", __FUNCTION__);
		}
		if (m_hThreadPlayVideo)
		{
			WaitForSingleObject(m_hThreadPlayVideo, INFINITE);
			CloseHandle(m_hThreadPlayVideo);
			DxTraceMsg("%s ThreadPlayVideo has exit.\n", __FUNCTION__);
		}
		if (m_hThreadPlayAudio)
		{
			ResumeThread(m_hThreadPlayAudio);
			WaitForSingleObject(m_hThreadPlayAudio, INFINITE);
			CloseHandle(m_hThreadPlayAudio);
			DxTraceMsg("%s ThreadPlayAudio has exit.\n", __FUNCTION__);
		}
		
		if (m_pDsBuffer)
		{
			m_pDsPlayer->DestroyDsoundBuffer(m_pDsBuffer);
			m_pDsBuffer = nullptr;
		}
// 		if (m_pDsPlayer)
// 		{
// 			m_pDsPlayer.reset();
// 			m_pDsPlayer = nullptr;
// 		}
		if (m_pFrameOffsetTable)
			delete []m_pFrameOffsetTable;
		
#ifdef _DEBUG
		m_hThreadPlayVideo = nullptr;		
		m_hThreadParser = nullptr;
//		m_hThreadGetFileSummary = nullptr;
		m_hThreadPlayAudio = nullptr;
		m_pFrameOffsetTable = nullptr;
#endif
		m_listVideoCache.clear();
		m_listAudioCache.clear();
	}

	void Pause()
	{
		m_bPause = !m_bPause;
	}

	/// @brief			开启硬解码功能
	/// @param [in]		bEnableHaccel	是否开启硬解码功能
	/// #- true			开启硬解码功能
	/// #- false		关闭硬解码功能
	/// @remark			开启和关闭硬解码功能必须要Start之前调用才能生效
	int  EnableHaccel(IN bool bEnableHaccel = false)
	{
		if (m_hThreadPlayVideo)
			return DVO_Error_PlayerHasStart;
		else
		{
			m_bEnableHaccel = bEnableHaccel;
			return DVO_Succeed;
		}
	}

	/// @brief			获取硬解码状态
	/// @retval			true	已开启硬解码功能
	/// @retval			flase	未开启硬解码功能
	inline bool  GetHaccelStatus()
	{
		return m_bEnableHaccel;
	}

	/// @brief			检查是否支持特定视频编码的硬解码
	/// @param [in]		nCodec		视频编码格式,@see DVO_CODEC
	/// @retval			true		支持指定视频编码的硬解码
	/// @retval			false		不支持指定视频编码的硬解码
	static bool  IsSupportHaccel(IN DVO_CODEC nCodec)
	{	
		enum AVCodecID nAvCodec = AV_CODEC_ID_NONE;
		switch (nCodec)
		{		
		case CODEC_H264:
			nAvCodec = AV_CODEC_ID_H264;
			break;
		case CODEC_H265:
			nAvCodec = AV_CODEC_ID_H264;
			break;		
		default:
			return false;
		}
		shared_ptr<CVideoDecoder>pDecoder = make_shared<CVideoDecoder>();
		UINT nAdapter = D3DADAPTER_DEFAULT;
		if (!pDecoder->InitD3D(nAdapter))
			return false;
		if (pDecoder->CodecIsSupported(nAvCodec) == S_OK)		
			return true;
		else
			return false;
	}

	/// @brief			取得当前播放视频的即时信息
	int GetPlayerInfo(PlayerInfo *pPlayInfo)
	{
		if (!pPlayInfo)
			return DVO_Error_InvalidParameters;
		if (m_hThreadPlayVideo|| m_hDvoFile)
		{
			ZeroMemory(pPlayInfo, sizeof(pPlayInfo));
			pPlayInfo->nVideoCodec	 = m_nVideoCodec;
			pPlayInfo->nVideoWidth	 = m_nVideoWidth;
			pPlayInfo->nVideoHeight	 = m_nVideoHeight;
			pPlayInfo->nAudioCodec	 = m_nAudioCodec;
			pPlayInfo->bAudioEnabled = m_bEnableAudio;
			pPlayInfo->tTimeEplased	 = (time_t)(1000 * (GetExactTime() - m_dfTimesStart));
			pPlayInfo->nFPS			 = m_nFileFPS;
			pPlayInfo->nPlayFPS		 = m_nPlayFPS;
			EnterCriticalSection(&m_csVideoCache);
			pPlayInfo->nCacheSize = m_listVideoCache.size();
			LeaveCriticalSection(&m_csVideoCache);
	
			if (m_pszFileName && ::PathFileExistsA(m_pszFileName))
			{
				if (/*m_nCurVideoFrame == 0 ||*/
					!m_pFrameOffsetTable)
				{
					int nError = GetFileSummary();
					if (nError != DVO_Succeed)
						return DVO_Error_SummaryNotReady;
				}
				pPlayInfo->nCurFrameID = m_nCurVideoFrame;
				pPlayInfo->nTotalFrames = m_nTotalFrames;
				pPlayInfo->tCurFrameTime = (m_tCurFrameTimeStamp - m_pFrameOffsetTable[0].tTimeStamp) / 1000;
				pPlayInfo->tTotalTime = (m_pFrameOffsetTable[m_nTotalFrames - 1].tTimeStamp - m_pFrameOffsetTable[0].tTimeStamp) / 1000;
				return DVO_Succeed;
			}
			else
				return DVO_Error_FileNotExist;
		}
		else
			return DVO_Error_PlayerNotStart;
	}
	
	/// @brief			截取正放播放的视频图像
	/// @param [in]		szFileName		要保存的文件名
	/// @param [in]		nFileFormat		保存文件的编码格式,@see SNAPSHOT_FORMAT定义
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	inline int  SnapShot(IN WCHAR *szFileName, IN SNAPSHOT_FORMAT nFileFormat = XIFF_JPG)
	{
		if (m_pDxSurface && m_hThreadPlayVideo)
		{
			m_pDxSurface->SaveSurfaceToFileW(szFileName, (D3DXIMAGE_FILEFORMAT)nFileFormat);
			return DVO_Succeed;
		}
		else
			return DVO_Error_PlayerNotStart;
	}

	/// @brief			设置播放的音量
	/// @param [in]		nVolume			要设置的音量值，取值范围0~100，为0时，则为静音
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	void  SetVolume(IN int nVolume)
	{
		if (m_bEnableAudio && m_pDsBuffer)
		{
			int nDsVolume = nVolume * 100 - 10000;
			m_pDsBuffer->SetVolume(nDsVolume);
		}
	}

	/// @brief			取得当前播放的音量
	int  GetVolume()
	{
		if (m_bEnableAudio && m_pDsBuffer)
			return (m_pDsBuffer->GetVolume() + 10000) / 100;
		else
			return 0;
	}

	/// @brief			设置当前播放的速度的倍率
	/// @param [in]		fPlayRate		当前的播放的倍率,大于1时为加速播放,小于1时为减速播放，不能为0或小于0
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	inline int  SetRate(IN float fPlayRate)
	{
		if (m_bIpcStream)
		{
			return DVO_Error_NotFilePlayer;
		}
		m_nPlayInterval = (int)(1000 / (m_nFileFPS*fPlayRate));
		m_nPlayFPS = 1000 / m_nPlayInterval;
		m_fPlayRate = fPlayRate;
		return DVO_Succeed;
	}

	/// @brief			跳跃到指视频帧进行播放
	/// @param [in]		nFrameID	要播放帧的起始ID
	/// @param [in]		bUpdate		是否更新画面,bUpdate为true则予以更新画面,画面则不更新
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	/// @remark			1.若所指定时间点对应帧为非关键帧，帧自动移动到就近的关键帧进行播放
	///					2.若所指定帧为非关键帧，帧自动移动到就近的关键帧进行播放
	///					3.只有在播放暂时,bUpdate参数才有效
	int  SeekFrame(IN int nFrameID,bool bUpdate = false)
	{
		if (!m_hDvoFile || !m_pFrameOffsetTable)
			return DVO_Error_NotFilePlayer;
		if (nFrameID < 0 || nFrameID > m_nTotalFrames)
			return DVO_Error_InvalidFrame;	

		EnterCriticalSection(&m_csVideoCache);
		m_listVideoCache.clear();
		ItLoop = m_listVideoCache.begin();
		m_nFrameOffset = 0;
		LeaveCriticalSection(&m_csVideoCache);

		EnterCriticalSection(&m_csAudioCache);
		m_listAudioCache.clear();
		LeaveCriticalSection(&m_csAudioCache);
		
		// 从文件摘要中，取得文件偏移信息
		// 查找最近的I帧
		int nForward = nFrameID, nBackWord = nFrameID;
		while (nForward < m_nTotalFrames)
		{
			if (m_pFrameOffsetTable[nForward].bIFrame)
				break;
			nForward++;
		}
		if (nForward >= m_nTotalFrames)
			nForward--;
		
		while (nBackWord > 0 && nBackWord < m_nTotalFrames)
		{
			if (m_pFrameOffsetTable[nBackWord].bIFrame)
				break;
			nBackWord --;
		}
		if (nBackWord < 0)
			nBackWord = 0;
		if ((nForward - nFrameID) <= (nFrameID - nBackWord))
			m_nFrametoRead = nForward;
		else
			m_nFrametoRead = nBackWord;
		if (m_hThreadParser)
			SetSeekOffset(m_pFrameOffsetTable[m_nFrametoRead].nOffset);
		else
		{// 只用于单纯的解析文件时移动文件指针
			CAutoLock lock(&m_csParser);
			m_nParserOffset = 0;
			m_nParserDataLength = 0;
			LONGLONG nOffset = m_pFrameOffsetTable[m_nFrametoRead].nOffset;
			if (LargerFileSeek(m_hDvoFile, nOffset, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			{
				DxTraceMsg("%s LargerFileSeek  Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return GetLastError();
			}
		}

		if (bUpdate && 
			m_hThreadPlayVideo &&	// 必须启动播放线程
			m_bPause &&				// 必须是暂停模式			
			m_pDecodec)				// 解码器必须已启动
		{
			// 读取一帧,并予以解码,显示
			DWORD nBufferSize = m_pFrameOffsetTable[m_nFrametoRead].nFrameSize;
			LONGLONG nOffset = m_pFrameOffsetTable[m_nFrametoRead].nOffset;

			byte *pBuffer = _New byte[nBufferSize + 1];
			if (!pBuffer)
				return DVO_Error_InsufficentMemory;

			unique_ptr<byte>BufferPtr(pBuffer);			
			DWORD nBytesRead = 0;
			if (LargerFileSeek(m_hDvoFile, nOffset, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			{
				DxTraceMsg("%s LargerFileSeek  Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return GetLastError();
			}

			if (!ReadFile(m_hDvoFile, pBuffer, nBufferSize, &nBytesRead, nullptr))
			{
				DxTraceMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return GetLastError();
			}
			AVPacket *pAvPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
			shared_ptr<AVPacket>AvPacketPtr(pAvPacket, av_free);
			AVFrame *pAvFrame = av_frame_alloc();
			shared_ptr<AVFrame>AvFramePtr(pAvFrame, av_free);
			av_init_packet(pAvPacket);
			pAvPacket->size = Frame(pBuffer)->nLength;
			pAvPacket->data = pBuffer + sizeof(DVOFrameHeaderEx);
			int nGotPicture = 0;
			char szAvError[1024] = { 0 };
			int nAvError = m_pDecodec->Decode(pAvFrame, nGotPicture, pAvPacket);
			if (nAvError < 0)
			{
				av_strerror(nAvError, szAvError, 1024);
				DxTraceMsg("%s Decode error:%s.\n", __FUNCTION__, szAvError);
				return DVO_Error_DecodeFailed;
			}
			av_packet_unref(pAvPacket);
			if (nGotPicture)
			{
				RenderFrame(pAvFrame);
			}
			return DVO_Succeed;
		}
		return DVO_Succeed;
	}
	
	/// @brief			跳跃到指定时间偏移进行播放
	/// @param [in]		tTimeOffset		要播放的起始时间,单位秒,如FPS=25,则0.04秒为第2帧，1.00秒，为第25帧
	/// @param [in]		bUpdate		是否更新画面,bUpdate为true则予以更新画面,画面则不更新
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	/// @remark			1.若所指定时间点对应帧为非关键帧，帧自动移动到就近的关键帧进行播放
	///					2.若所指定帧为非关键帧，帧自动移动到就近的关键帧进行播放
	///					3.只有在播放暂时,bUpdate参数才有效
	int  SeekTime(IN time_t tTimeOffset, bool bUpdate)
	{
		if (!m_hDvoFile)
			return DVO_Error_NotFilePlayer;

		if (!m_pFrameOffsetTable)
		{
			int nError = GetFileSummary();
			if (nError != DVO_Succeed)
				return nError;
		}

		int nFrameID = 0;
		if (m_nFileFPS == 0 || m_nFileFrameInterval == 0)
		{// 使用二分法查找
			nFrameID = BinarySearch(tTimeOffset);
			if (nFrameID == -1)
				return DVO_Error_InvalidTimeOffset;
		}
		else
		{
			int nTimeDiff = tTimeOffset - m_pFrameOffsetTable[0].tTimeStamp;
			if (nTimeDiff < 0)
				return DVO_Error_InvalidTimeOffset;
			nFrameID = nTimeDiff / m_nFileFrameInterval;
		}
		return SeekFrame(nFrameID, bUpdate);
	}
	/// @brief 从文件中读取一帧，读取的起点默认值为0,SeekFrame或SeekTime可设定其起点位置
	/// @param [in,out]		pBuffer		帧数据缓冲区,可设置为null
	/// @param [in,out]		nBufferSize 帧缓冲区的大小
	int GetFrame(INOUT byte **pBuffer, OUT UINT &nBufferSize)
	{
		if (!m_hDvoFile)
			return DVO_Error_NotFilePlayer;
		if (m_hThreadPlayVideo || m_hThreadParser)
			return DVO_Error_PlayerHasStart;
		if (!m_pFrameOffsetTable || !m_nTotalFrames)
			return DVO_Error_SummaryNotReady;

		DWORD nBytesRead = 0;
		FrameParser Parser;
		CAutoLock lock(&m_csParser);
		byte *pFrameBuffer = &m_pParserBuffer[m_nParserOffset];
		if (!ParserFrame(&pFrameBuffer, m_nParserDataLength, &Parser))
		{
			// 残留数据长为nDataLength
			memmove(m_pParserBuffer, pFrameBuffer, m_nParserDataLength);
			if (!ReadFile(m_hDvoFile, &m_pParserBuffer[m_nParserDataLength], (m_nParserBufferSize - m_nParserDataLength), &nBytesRead, nullptr))
			{
				DxTraceMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return GetLastError();
			}
			m_nParserOffset = 0;
			m_nParserDataLength += nBytesRead;
			pFrameBuffer = m_pParserBuffer;
			if (!ParserFrame(&pFrameBuffer, m_nParserDataLength, &Parser))
			{
				return DVO_Error_NotDvoVideoFile;
			}
		}
		m_nParserOffset += Parser.nFrameSize;
		*pBuffer = (byte *)Parser.pHeaderEx;
		nBufferSize = Parser.nFrameSize;
		return DVO_Succeed;
	}
	
	/// @brief			播放下一帧
	/// @param [in]		nTimeSet		要播放的起始时间
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	/// @remark			该函数仅适用于单帧播放
	int  SeekNextFrame()
	{
		return 0;
	}

	/// @brief			开/关音频播放
	/// @param [in]		bEnable			是否播放音频
	/// -#	true		开启音频播放
	/// -#	false		禁用音频播放
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	int  EnableAudio(bool bEnable = true)
	{
// 		if (!m_DsPlayer)
// 			return DVO_Succeed;		
		if (m_fPlayRate > 4)
			return DVO_Succeed;
		
		if (bEnable)
		{
// 			if (!m_pDsPlayer)
// 			{
				//m_pDsPlayer = make_shared<CDSound>(nullptr);
				if (!m_pDsPlayer->IsInitialized())
					m_pDsPlayer->Initialize(m_hWnd, Audio_Play_Segments);
//			}
			if (!m_pDsBuffer)			
				m_pDsBuffer = m_pDsPlayer->CreateDsoundBuffer();
			
			m_pDsBuffer->StartPlay();
			if (!m_hThreadPlayAudio)
			{
				m_bThreadPlayAudioRun = true;
				//m_hThreadPlayAudio = (HANDLE)_beginthreadex(nullptr, 0, ThreadPlayAudio, this, 0, 0);
				m_hThreadPlayAudio = CreateThread(nullptr, 0, ThreadPlayAudio, this, 0, 0);
			}	
			m_dfLastTimeAudioPlay = 0.0f;
			m_dfLastTimeAudioSample = 0.0f;
			m_bEnableAudio = true;
		}
		else
		{
			m_bEnableAudio = false;			
			m_bThreadPlayAudioRun = false;
			if (m_hThreadPlayAudio)
			{
				ResumeThread(m_hThreadPlayAudio);
				WaitForSingleObject(m_hThreadPlayAudio, INFINITE);
				CloseHandle(m_hThreadPlayAudio);
				m_hThreadPlayAudio = nullptr;
				DxTraceMsg("%s ThreadPlayAudio has exit.\n", __FUNCTION__);
			}

			if (m_pDsBuffer)
			{
				m_pDsPlayer->DestroyDsoundBuffer(m_pDsBuffer);
				m_pDsBuffer = nullptr;
			}
// 			if (m_pDsPlayer)
// 			{
// 				m_pDsPlayer.reset();
// 				m_pDsPlayer = nullptr;
// 			}
		}
		return DVO_Succeed;
	}

	/// @brief			刷新播放窗口
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	/// @remark			该功能一般用于播放结束后，刷新窗口，把画面置为黑色
	inline void  Refresh()
	{
		if (m_hWnd)
			::InvalidateRect(m_hWnd, nullptr, true);
	}
	
	/// @brief			设置获取YUV数据回调接口,通过此回调，用户可直接获取解码后的YUV数据
	void SetYUVCapture(IN CaptureYUV pYUVProc, IN void *pUserPtr)
	{
		if (!pYUVProc)
			return ;
		m_pfnCaptureYUV = pYUVProc;
		m_pUserCaptureYUV = pUserPtr;
		
	}

	/// @brief			设置获取YUV数据回调接口,通过此回调，用户可直接获取解码后的YUV数据
	void SetYUVCaptureEx(IN CaptureYUVEx pYUVExProc, IN void *pUserPtr)
	{
		if (!pYUVExProc)
			return;
		m_pfnCaptureYUVEx = pYUVExProc;
		m_pUserCaptureYUVEx = pUserPtr;
	}

	/// @brief			设置DVO私用格式录像，帧解析回调,通过此回，用户可直接获取原始的帧数据
	void SetFrameParserCapture(IN CaptureFrame pFrameParserProc,IN void *pUserPtr)
	{
		if (!pFrameParserProc)
			return;
		m_pfnCaptureFrame = pFrameParserProc;
		m_pUserCaptureFrame = pUserPtr;

	}
	/// @brief			设置外部绘制回调接口
	int SetExternDraw(void *pExternDrawProc, void *pUserPtr)
	{
		if (m_pDxSurface)
		{
			m_pDxSurface->SetExternDraw(pExternDrawProc, (long)pUserPtr);
			return 0;
		}
		else
			return -1;
	}
	
	/// @brief			解析帧数据
	/// @param [in,out]	pBuffer			来自DVO私有录像文件中的数据流
	/// @param [in,out]	nDataSize		pBuffer中有效数据的长度
	/// @param [out]	pFrameParser	DVO私有录像的帧数据	
	/// @retval			true	操作成功
	/// @retval			false	失败，pBuffer已经没有有效的帧数据
	bool ParserFrame(INOUT byte **ppBuffer,
					 INOUT DWORD &nDataSize, 
					 FrameParser* pFrameParser)
	{
		int nOffset = 0;
		if (nDataSize < sizeof(DVOFrameHeaderEx))
			return false;
		if (Frame(*ppBuffer)->nFrameTag != DVO_TAG)
		{
			static char *szKey = "MOVD";
			nOffset = KMP_StrFind(*ppBuffer, nDataSize, (byte *)szKey, 4);
			nOffset -= offsetof(DVOFrameHeaderEx, nFrameTag);
		}

		if (nOffset < 0)
			return false;
		
		// 帧头信息不完整
		if ((nOffset + sizeof(DVOFrameHeaderEx)) >= nDataSize)
			return false;

		*ppBuffer += nOffset;
		
		// 帧数据不完整
		if (nOffset + FrameSize(*ppBuffer) >= nDataSize)
			return false;
		
		if (pFrameParser)
		{
			pFrameParser->pHeaderEx = (DVOFrameHeaderEx *)(*ppBuffer);
			pFrameParser->nFrameSize = FrameSize(*ppBuffer);
			pFrameParser->pLawFrame = *ppBuffer + sizeof(DVOFrameHeaderEx);
			pFrameParser->nLawFrameSize = Frame(*ppBuffer)->nLength;
		}
		nDataSize -= FrameSize(*ppBuffer);
		*ppBuffer += FrameSize(*ppBuffer);
		
		return true;
	}
	///< @brief 视频文件解析线程
	static DWORD WINAPI ThreadParser(void *p)
	{// 若指定了有效的窗口句柄，则把解析后的文件数据放入播放队列，否则不放入播放队列
		CDvoPlayer* pThis = (CDvoPlayer *)p;
		LONGLONG nSeekOffset = 0;
		DWORD nBufferSize = pThis->m_nMaxFrameSize * 4;
		DWORD nBytesRead = 0;
		DWORD nDataLength = 0;		
		byte *pBuffer = _New byte[nBufferSize];		
		shared_ptr<byte>BufferPtr(pBuffer);
		FrameParser Parser;
		pThis->m_tLastFrameTime = 0;
	
		if (SetFilePointer(pThis->m_hDvoFile, (LONG)sizeof(DVO_MEDIAINFO), nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			DxTraceMsg("%s SetFilePointer Failed,Error = %d.\n", __FUNCTION__, GetLastError());
			assert(false);
			return 0;
		}
		
		while (pThis->m_bThreadParserRun)
		{
			if (pThis->m_bPause)
			{
				Sleep(20);
				continue;
			}
			if (nSeekOffset = pThis->GetSeekOffset())
			{
				DxTraceMsg("Detect SeekFrame Operation.\n");
				pThis->SetSeekOffset(0);
				nDataLength = 0;
				if (SetFilePointer(pThis->m_hDvoFile, (LONG)nSeekOffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)				
					DxTraceMsg("%s SetFilePointer Failed,Error = %d.\n",__FUNCTION__, GetLastError());
			}
			if (!ReadFile(pThis->m_hDvoFile, &pBuffer[nDataLength], (nBufferSize - nDataLength), &nBytesRead, nullptr))
			{
				DxTraceMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return 0;
			}
			nDataLength += nBytesRead;
			byte *pFrameBuffer = pBuffer;

			bool bIFrame = false;
			bool bFrameInput = true;
			while (pThis->m_bThreadParserRun)
			{
				if (pThis->m_bPause)		// 通过pause 函数，暂停数据读取
				{
					Sleep(20);
					continue;
				}
				if (bFrameInput)
				{
					bFrameInput = false;
					if (!pThis->ParserFrame(&pFrameBuffer, nDataLength, &Parser))
						break;

					if (pThis->m_pfnCaptureFrame)					
						pThis->m_pfnCaptureFrame(pThis, (byte *)Parser.pHeaderEx, Parser.nFrameSize, pThis->m_pUserCaptureFrame);				
					if (!pThis->m_hWnd)		// 没有窗口句柄，则不放入播放队列
					{
						bFrameInput = true;
						continue;
					}
					
					int nResult = pThis->InputStream((byte *)Parser.pHeaderEx, Parser.nFrameSize);
					switch (nResult)
					{
					case DVO_Succeed:
					case DVO_Error_InvalidFrameType:
					default:
						bFrameInput = true;
						break;
					case DVO_Error_FrameCacheIsFulled:	// 缓冲区已满
						bFrameInput = false;
						Sleep(10);
						break;
					}
				}
				else
				{
					int nResult = pThis->InputStream((byte *)Parser.pHeaderEx, Parser.nFrameSize);
					switch (nResult)
					{
					case DVO_Succeed:
					case DVO_Error_InvalidFrameType:
					default:
						bFrameInput = true;
						break;
					case DVO_Error_FrameCacheIsFulled:	// 缓冲区已满
						bFrameInput = false;					
						Sleep(10);
						break;
					}
				}
			}
			// 残留数据长为nDataLength
			memmove(pBuffer, pFrameBuffer, nDataLength);
#ifdef _DEBUG
			ZeroMemory(&pBuffer[nDataLength],nBufferSize - nDataLength);
#endif
			// 若是单纯解析数据线程，则需要暂缓读取数据
			if (!pThis->m_hWnd )
			{
				Sleep(10);
			}
		}
		return 0;
	}

	/// @brief			获取码流类型	
	/// @param [in]		pVideoCodec		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
	/// @param [out]	pAudioCodec	返回当前hPlayHandle是否已开启硬解码功能
	/// @remark 码流类型定义请参考:@see DVO_CODEC
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	/// @retval			DVO_Error_PlayerNotStart	播放器尚未启动,无法取得播放过程的信息或属性
	int GetCodec(DVO_CODEC *pVideoCodec, DVO_CODEC *pAudioCodec)
	{
		if (!m_hThreadPlayVideo)
			return DVO_Error_PlayerNotStart;
		if (pVideoCodec)
			*pVideoCodec = m_nVideoCodec;
		if (pAudioCodec)
			*pAudioCodec = m_nAudioCodec;
		return DVO_Succeed;
	}

	// 探测视频码流类型,要求必须输入I帧
	bool ProbeStream(byte *szFrameBuffer,int nBufferLength)
	{	
		shared_ptr<CVideoDecoder>pDecodec = make_shared<CVideoDecoder>();
		InputStream(szFrameBuffer, nBufferLength);
		auto ItLoop = m_listVideoCache.begin();
		m_bProbeMode = true;
		if (pDecodec->ProbeStream(this, ReadAvData, m_nMaxFrameSize) != 0)
		{
			DxTraceMsg("%s Failed in ProbeStream,you may input a unknown stream.\n", __FUNCTION__);
			assert(false);
			return false;
		}
		pDecodec->CancelProbe();		
		m_bProbeMode = false;
		::EnterCriticalSection(&m_csAudioCache);
		m_listVideoCache.clear();		
		::LeaveCriticalSection(&m_csAudioCache);
		if (pDecodec->m_nCodecId == AV_CODEC_ID_NONE)
		{
			DxTraceMsg("%s Unknown Video Codec or not found any codec in the stream.\n", __FUNCTION__);
			assert(false);
			return false;
		}
		if (pDecodec->m_nCodecId == AV_CODEC_ID_H264)
		{
			m_nVideoCodec = CODEC_H264;
			DxTraceMsg("%s Video Codec:%H.264 Width = %d\tHeight = %d.\n", __FUNCTION__, pDecodec->m_pAVCtx->width, pDecodec->m_pAVCtx->height);
		}
		else if (pDecodec->m_nCodecId == AV_CODEC_ID_HEVC)
		{
			m_nVideoCodec = CODEC_H265;
			DxTraceMsg("%s Video Codec:%H.265 Width = %d\tHeight = %d.\n", __FUNCTION__, pDecodec->m_pAVCtx->width, pDecodec->m_pAVCtx->height);
		}
		else
		{
			m_nVideoCodec = CODEC_UNKNOWN;
			DxTraceMsg("%s Unsupported Video Codec.\n", __FUNCTION__);
			assert(false);
			return false;
		}
		m_nVideoWidth = pDecodec->m_pAVCtx->width;
		m_nVideoHeight = pDecodec->m_pAVCtx->height;
		return true;
	}
	/// @brief			取得文件的概要的信息,如文件总帧数,文件偏移表
	int GetFileSummary()
	{
#ifdef _DEBUG
		double dfTimeStart = GetExactTime();
#endif
		if (strlen(m_pszFileName) <= 0 || !PathFileExistsA(m_pszFileName))
			return DVO_Error_FileNotOpened;
		if (!m_hDvoFile)
		{
			m_hDvoFile = CreateFileA(m_pszFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_ARCHIVE,
				NULL);
			if (!m_hDvoFile)
			{
				return 0;
			}
		}
		
		int nError = GetFileHeader();
		if (nError != DVO_Succeed)
			return nError;

		int nDvoError = GetLastFrameID(m_nTotalFrames);
		if (nDvoError != DVO_Succeed)
		{
			CloseHandle(m_hDvoFile);
			return nDvoError;
		}
		// GetLastFrameID取得的是最后一帧的ID，总帧数还要在此基础上+1
		m_nTotalFrames++;

		DWORD nBufferSize = 1024 * 1024 * 32;
		LARGE_INTEGER liFileSize;		
		if (!GetFileSizeEx(m_hDvoFile, &liFileSize))
			return 0;
		if (liFileSize.QuadPart <= nBufferSize)
			nBufferSize = liFileSize.LowPart;
		// 不再分析文件，因为StartPlay已经作过分析的确认
		DWORD nOffset = sizeof(DVO_MEDIAINFO);
		if (SetFilePointer(m_hDvoFile, nOffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			assert(false);
			return 0;
		}
		if (!m_pFrameOffsetTable)
		{
			m_pFrameOffsetTable = _New FileFrameInfo[m_nTotalFrames];
			ZeroMemory(m_pFrameOffsetTable, sizeof(FileFrameInfo)*m_nTotalFrames);
		}
		
		byte *pBuffer = _New byte[nBufferSize];
		while (!pBuffer)
		{
			if (nBufferSize <= 1024 * 1024)
			{// 连1MB的内存都无法申请的话，则退出
				DxTraceMsg("%s Can't alloc enough memory.\n", __FUNCTION__);
				assert(false);
				return DVO_Error_InsufficentMemory;
			}
			nBufferSize /= 2;
			pBuffer = _New byte[nBufferSize];
		}
		shared_ptr<byte>BufferPtr(pBuffer);
		byte *pFrame = nullptr;
		int nFrameSize = 0;
		int nFrames = 0;
		LONG nSeekOffset = 0;
		DWORD nBytesRead = 0;
		DWORD nDataLength = 0;
		byte *pFrameBuffer = nullptr;
		FrameParser Parser;
		int nFrameOffset = sizeof(DVO_MEDIAINFO);
		bool bIFrame = false;
		bool bStreamProbed = false;		// 是否已经探测过码流
		while (true)
		{
			if (!ReadFile(m_hDvoFile, &pBuffer[nDataLength], (nBufferSize - nDataLength), &nBytesRead, nullptr))
			{
				DxTraceMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return DVO_Error_ReadFileFailed;
			}
			if (nBytesRead == 0)		// 未读取任何内容，已经达到文件结尾
				break;
			pFrameBuffer = pBuffer;
			nDataLength += nBytesRead;
			while (true)
			{
				if (!ParserFrame(&pFrameBuffer, nDataLength, &Parser))
					break;
				if (IsDVOVideoFrame(Parser.pHeaderEx, bIFrame))	// 只记录视频帧的文件偏移
				{
					if (bIFrame && !bStreamProbed)
					{// 尝试探测码流
						bStreamProbed = ProbeStream((byte *)Parser.pHeaderEx, Parser.nFrameSize);
					}
					m_pFrameOffsetTable[nFrames].nOffset = nFrameOffset;
					m_pFrameOffsetTable[nFrames].nFrameSize = Parser.nFrameSize;
					m_pFrameOffsetTable[nFrames].bIFrame = bIFrame;
					m_pFrameOffsetTable[nFrames].tTimeStamp = Parser.pHeaderEx->nTimestamp;
					nFrames++;
				}
				else
					m_nAudioCodec = (DVO_CODEC)Parser.pHeaderEx->nType;
				nFrameOffset += Parser.nFrameSize;
			}
			nOffset += nBytesRead;
			// 残留数据长为nDataLength
			memcpy(pBuffer, pFrameBuffer, nDataLength);
		}
#ifdef _DEBUG
		DxTraceMsg("%s TimeSpan = %.3f.\n", __FUNCTION__, TimeSpanEx(dfTimeStart));
#endif
		// 查找最一个I帧
		int nLastIFrameID = m_nTotalFrames - 1;
		for (; nLastIFrameID > 0; nLastIFrameID--)
		{
			if (m_pFrameOffsetTable[nLastIFrameID].bIFrame)
				break;
		}
		// 根据录像总时间和总有完整帧数计算FPS
		m_nFileFPS = nLastIFrameID / ((m_pFrameOffsetTable[nLastIFrameID - 1].tTimeStamp - m_pFrameOffsetTable[0].tTimeStamp) / (1000 * 1000));
		m_nFileFrameInterval = 1000 / m_nFileFPS;
		// 把文件指针重新移回到文件头部
		nOffset = sizeof(DVO_MEDIAINFO);
		if (SetFilePointer(m_hDvoFile, nOffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			assert(false);
			return 0;
		}
		return DVO_Succeed;
	}
	
	/// 文件摘要线程
// 	static DWORD WINAPI ThreadGetFileSummary(void *p)
// 	{
// 		CDvoPlayer* pThis = (CDvoPlayer *)p;
// 		if (pThis->m_nTotalFrames == 0)
// 		{
// 			assert(false);
// 			return 0;
// 		}
// #ifdef _DEBUG
// 		double dfTimeStart = GetExactTime();
// #endif
// 		HANDLE	hDvoFile = CreateFileA(pThis->m_pszFileName,
// 									   GENERIC_READ,
// 									   FILE_SHARE_READ,
// 									   NULL,
// 									   OPEN_ALWAYS,
// 									   FILE_ATTRIBUTE_ARCHIVE,
// 									   NULL);
// 		if (!hDvoFile)
// 		{
// 			return 0;
// 		}
// 
// 		shared_ptr<void> DvoFilePtr(hDvoFile, CloseHandle);
// 		
// 		LARGE_INTEGER liFileSize;
// 		DWORD nBufferSize = 1024 * 1024 * 32;
// 		if (!GetFileSizeEx(hDvoFile, &liFileSize))
// 			 return 0;
// 		if (liFileSize.QuadPart <= nBufferSize)
// 			nBufferSize = liFileSize.LowPart;
// 		// 不再分析文件，因为StartPlay已经作过分析的确认
// 		DWORD nOffset = sizeof(DVO_MEDIAINFO);		
// 		if (SetFilePointer(hDvoFile, nOffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
// 		{
// 			assert(false);
// 			return 0;
// 		}
// 		
// 		pThis->m_pFrameOffsetTable = _New FileFrameInfo[pThis->m_nTotalFrames];
// 		ZeroMemory(pThis->m_pFrameOffsetTable, sizeof(FileFrameInfo)*pThis->m_nTotalFrames);
// 		
// 		byte *pBuffer = _New byte[nBufferSize];
// 		while (!pBuffer)
// 		{
// 			if (nBufferSize <= 1024 * 1024)
// 			{// 连1MB的内存都无法申请的话，则退出
// 				DxTraceMsg("%s Can't alloc enough memory.\n", __FUNCTION__);
// 				assert(false);
// 				return 0;
// 			}
// 			nBufferSize /= 2;
// 			pBuffer = _New byte[nBufferSize];
// 		}
// 		shared_ptr<byte>BufferPtr(pBuffer);
// 		byte *pFrame = nullptr;
// 		int nFrameSize = 0;
// 		int nFrames = 0;
// 		LONG nSeekOffset = 0;
// 		DWORD nBytesRead = 0;
// 		DWORD nDataLength = 0;
// 		byte *pFrameBuffer = nullptr;
// 		FrameParser Parser;
// 		int nFrameOffset = sizeof(DVO_MEDIAINFO);
// 		bool bIFrame = false;
// 		while (pThis->m_bThreadFileAbstractRun)
// 		{
// 			if (!ReadFile(hDvoFile, &pBuffer[nDataLength], (nBufferSize - nDataLength), &nBytesRead, nullptr))
// 			{
// 				DxTraceMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
// 				return 0;
// 			}
// 			if (nBytesRead == 0)		// 未读取任何内容，已经达到文件结尾
// 				break;
// 			pFrameBuffer = pBuffer;
// 			nDataLength += nBytesRead;
// 			while (pThis->m_bThreadFileAbstractRun)
// 			{
// 				if (!pThis->ParserFrame(&pFrameBuffer, nDataLength, &Parser))
// 					break;
// 				if (IsDVOVideoFrame(Parser.pHeaderEx, bIFrame))	// 只记录视频帧的文件偏移
// 				{
// 					pThis->m_pFrameOffsetTable[nFrames].nOffset		 = nFrameOffset;
// 					pThis->m_pFrameOffsetTable[nFrames].bIFrame		 = bIFrame;
// 					pThis->m_pFrameOffsetTable[nFrames].tTimeStamp	 = Parser.pHeaderEx->nTimestamp;
// 					nFrames++;
// 				}
// 				nFrameOffset += Parser.nFrameSize;
// 			}
// 			nOffset += nBytesRead;
// 			// 残留数据长为nDataLength
// 			memcpy(pBuffer, pFrameBuffer, nDataLength);
// 		}
// #ifdef _DEBUG
// 		DxTraceMsg("%s TimeSpan = %.3f.\n", __FUNCTION__, TimeSpanEx(dfTimeStart));
// #endif
// 		return 0;
// 	}
	
	/// @brief 把NV12图像转换为YV420P图像
	void CopyNV12ToYUV420P(byte *pYV12, byte *pNV12[2], int src_pitch[2], unsigned width, unsigned height)
	{
		byte* dstV = pYV12 + width*height;
		byte* dstU = pYV12 + width*height / 4;
		UINT heithtUV = height / 2;
		UINT widthUV = width / 2;
		byte *pSrcUV = pNV12[1];
		byte *pSrcY = pNV12[0];
		int &nYpitch = src_pitch[0];
		int &nUVpitch = src_pitch[1];

		// 复制Y分量
		for (int i = 0; i < height; i++)
			memcpy(pYV12 + i*width, pSrcY + i*nYpitch, width);

		// 复制VU分量
		for (int i = 0; i < heithtUV; i++)
		{
			for (int j = 0; j < width; j++)
			{
				dstU[i*widthUV + j] = pSrcUV[i*nUVpitch + 2 * j];
				dstV[i*widthUV + j] = pSrcUV[i*nUVpitch + 2 * j + 1];
			}
		}
	}

	/// @brief 把Dxva硬解码帧转换成YUV420图像
	void CopyDxvaFrame(byte *pYuv420, AVFrame *pAvFrameDXVA)
	{
		if (pAvFrameDXVA->format != AV_PIX_FMT_DXVA2_VLD)
			return;

		IDirect3DSurface9* pSurface = (IDirect3DSurface9 *)pAvFrameDXVA->data[3];
		D3DLOCKED_RECT lRect;
		D3DSURFACE_DESC SurfaceDesc;
		pSurface->GetDesc(&SurfaceDesc);
		HRESULT hr = pSurface->LockRect(&lRect, nullptr, D3DLOCK_READONLY);
		if (FAILED(hr))
		{
			DxTraceMsg("%s IDirect3DSurface9::LockRect failed:hr = %08.\n", __FUNCTION__, hr);
			return;
		}

		// Y分量图像
		byte *pSrcY = (byte *)lRect.pBits;
		// UV分量图像
		//byte *pSrcUV = (byte *)lRect.pBits + lRect.Pitch * SurfaceDesc.Height;
		byte *pSrcUV = (byte *)lRect.pBits + lRect.Pitch * pAvFrameDXVA->height;

		byte* dstY = pYuv420;
		byte* dstV = pYuv420 + pAvFrameDXVA->width*pAvFrameDXVA->height;
		byte* dstU = pYuv420 + pAvFrameDXVA->width*pAvFrameDXVA->height/ 4;


		UINT heithtUV = pAvFrameDXVA->height / 2;
		UINT widthUV = pAvFrameDXVA->width / 2;

		// 复制Y分量
		for (int i = 0; i < pAvFrameDXVA->height; i++)
			memcpy(&dstY[i*pAvFrameDXVA->width], &pSrcY[i*lRect.Pitch], pAvFrameDXVA->width);

		// 复制VU分量
		for (int i = 0; i < heithtUV; i++)
		{
			for (int j = 0; j < widthUV; j++)
			{
				dstU[i*widthUV + j] = pSrcUV[i*lRect.Pitch + 2 * j];
				dstV[i*widthUV + j] = pSrcUV[i*lRect.Pitch + 2 * j + 1];
			}
		}

		pSurface->UnlockRect();
	}

	
	void ProcessYVUCapture(AVFrame *pAvFrame,LONGLONG nTimestamp)
	{
		if (m_pfnCaptureYUV)
		{
			if (!m_pYUV)
			{
				m_nYUVSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pAvFrame->width, pAvFrame->height, 8);
				m_pYUV = (byte *)av_malloc(m_nYUVSize);
				m_pYUVPtr = shared_ptr<byte>(m_pYUV, av_free);
			}
			// 合并YUV数据
			int nPicSize = pAvFrame->linesize[0] * pAvFrame->height;
						
			if (pAvFrame->format == AV_PIX_FMT_DXVA2_VLD)
			{// 暂不支持dxva 硬解码帧
				CopyDxvaFrame(m_pYUV, pAvFrame);	
			}
			else
			{
				memcpy(m_pYUV, pAvFrame->data[0], nPicSize);
				memcpy(&m_pYUV[nPicSize], pAvFrame->data[1], nPicSize / 2);
				memcpy(&m_pYUV[nPicSize + nPicSize / 2], pAvFrame->data[2], nPicSize / 2);	
			}
			m_pfnCaptureYUV(this, m_pYUV, m_nYUVSize, pAvFrame->width, pAvFrame->height, nTimestamp, m_pUserCaptureYUV);		
		}
		if (m_pfnCaptureYUVEx)
		{
			if (pAvFrame->format == AV_PIX_FMT_DXVA2_VLD)
			{// dxva 硬解码帧
				CopyDxvaFrame(m_pYUV, pAvFrame);				
				byte* pU = m_pYUV + pAvFrame->width*pAvFrame->height;
				byte* pV = m_pYUV + pAvFrame->width*pAvFrame->height / 4;
				m_pfnCaptureYUVEx(this,
								  m_pYUV,
								  pU,
								  pV,
								  pAvFrame->linesize[0],
								  pAvFrame->linesize[1],
								  pAvFrame->width,
								  pAvFrame->height,
								  nTimestamp,
								  m_pUserCaptureYUVEx);
			}
			else
				m_pfnCaptureYUVEx(this,
								 pAvFrame->data[0],
								 pAvFrame->data[1],
								 pAvFrame->data[2],
								 pAvFrame->linesize[0],
								 pAvFrame->linesize[1],
								 pAvFrame->width,
								 pAvFrame->height,
								 nTimestamp,
								 m_pUserCaptureYUVEx);
		}
	}


	/// @brief			码流探测读取数据包回调函数
	/// @param [in]		opaque		用户输入的回调函数参数指针
	/// @param [in]		buf			读取数据的缓存
	/// @param [in]		buf_size	缓存的长度
	/// @return			实际读取数据的长度
	static int ReadAvData(void *opaque, uint8_t *buf, int buf_size)
	{
		AvQueue *pAvQueue = (AvQueue *)opaque;
		CDvoPlayer *pThis = (CDvoPlayer *)pAvQueue->pUserData;
		StreamFramePtr FramePtr;
		int nReturnVal = buf_size;
		int nRemainedLength = 0;
		pAvQueue->pAvBuffer = buf;
		CAutoLock lock(&pThis->m_csVideoCache);	
		if (pThis->m_listVideoCache.size() == 0)
			return 0;
#ifdef _DEBUG
		int nCacheSize = pThis->m_listVideoCache.size();
		//DxTraceMsg("%s Video Cache size = %d.\n", __FUNCTION__, nCacheSize);
#endif
		FramePtr = pThis->m_listVideoCache.front();
		if (pThis->m_bProbeMode)
		{
			if (pThis->m_bIFrameRecved)
			{
				if (pThis->ItLoop == pThis->m_listVideoCache.end())
					return 0;
				FramePtr = *pThis->ItLoop;
			}
			bool bIFrame = FramePtr->FrameHeader()->nType == FRAME_IDR|| 
						   FramePtr->FrameHeader()->nType == FRAME_I|| 
						   FramePtr->FrameHeader()->nType == 0;		// 海思I帧类型值为0
			nRemainedLength = FramePtr->FrameHeader()->nLength - pThis->m_nFrameOffset;

			if (nRemainedLength > buf_size)
			{
				memcpy(buf, &FramePtr->Framedata()[pThis->m_nFrameOffset], buf_size);
				pThis->m_nFrameOffset += buf_size;
			}
			else
			{
				memcpy(buf, &FramePtr->Framedata()[pThis->m_nFrameOffset], nRemainedLength);
				pThis->m_nFrameOffset = 0;
				nReturnVal = nRemainedLength;

				if (pThis->m_bIFrameRecved)
				{
					pThis->ItLoop++;
				}
				else
				{
					if (!bIFrame)		// 在探测模式下，抛弃所有第一个I帧之前的帧
					{
						pThis->m_listVideoCache.pop_front();
						pThis->m_nDisardFrames++;						
					}
					else
					{
						pThis->m_bIFrameRecved = true;
						pThis->ItLoop = pThis->m_listVideoCache.begin();
					}
				}
			}
		}
		else
		{
			pThis->m_nCurVideoFrame = FramePtr->FrameHeader()->nFrameID;
			pThis->m_tCurFrameTimeStamp = FramePtr->FrameHeader()->nTimestamp;
			nRemainedLength = FramePtr->FrameHeader()->nLength - pThis->m_nFrameOffset;
			if (nRemainedLength > buf_size)
			{
				memcpy(buf, &FramePtr->Framedata()[pThis->m_nFrameOffset], buf_size);
				pThis->m_nFrameOffset += buf_size;
			}
			else
			{
				memcpy(buf, &FramePtr->Framedata()[pThis->m_nFrameOffset], nRemainedLength);
				pThis->m_nFrameOffset = 0;
				nReturnVal = nRemainedLength;
				pThis->m_listVideoCache.pop_front();
			}
		}
		return nReturnVal;
	}

	static DWORD WINAPI ThreadPlayVideo(void *p)
	{
		CDvoPlayer* pThis = (CDvoPlayer *)p;
		int nAvError = 0;
		char szAvError[1024] = { 0 };
		if (!pThis->m_hWnd)
		{
			DxTraceMsg("%s Please assign a Window.\n", __FUNCTION__);
			assert(false);
			return 0;
		}	
		HWND hWndDxInit = pThis->m_pWndDxInit->GetSafeHwnd();
		if (!hWndDxInit)
		{
			DxTraceMsg("%s The Window for Direct3d is not ready.\n", __FUNCTION__);
			assert(false);
			return 0;
		}
		shared_ptr<CVideoDecoder>pDecodec = make_shared<CVideoDecoder>();
		if (!pDecodec)
		{
			DxTraceMsg("%s Failed in allocing memory for Decoder.\n", __FUNCTION__);
			assert(false);
			return 0;
		}
		// 等待I帧
		double tFirst = GetExactTime();
#ifdef _DEBUG
		double dfTimeout = 8.0f;
#else
		double dfTimeout = 2.0f;		// 等待I帧时间
#endif
		while (true)
		{
			if ((TimeSpanEx(tFirst)) < dfTimeout)
			{
				Sleep(10);
				CAutoLock lock(&pThis->m_csVideoCache);
				if (find_if(pThis->m_listVideoCache.begin(), pThis->m_listVideoCache.end(), StreamFrame::IsIFrame) != pThis->m_listVideoCache.end())
					break;
			}
			else
			{
				DxTraceMsg("%s Warning!!!\nNot receive an I frame in %d second.\n", __FUNCTION__, (int)dfTimeout);
				assert(false);
				return 0;
			}
		}
		// 探测码流类型
		pThis->ItLoop = pThis->m_listVideoCache.begin();
		pThis->m_bProbeMode = true;
		if (pDecodec->ProbeStream(pThis,ReadAvData, pThis->m_nMaxFrameSize) != 0)
		{
			DxTraceMsg("%s Failed in ProbeStream,you may input a unknown stream.\n", __FUNCTION__);
			assert(false);
			return 0;
		}
		pDecodec->CancelProbe();
		DxTraceMsg("%s Discard %d Non-I-Frames in Probe mode.\n", __FUNCTION__,pThis->m_nDisardFrames);
		pThis->m_bProbeMode = false;

		if (!pDecodec->InitDecoder(pThis->m_bEnableHaccel))
		{
			DxTraceMsg("%s Failed in Initializing Decoder.\n", __FUNCTION__);
			assert(false);
			return 0;
		}
		if (pDecodec->m_nCodecId == AV_CODEC_ID_NONE )
		{
			DxTraceMsg("%s Unknown Video Codec or not found any codec in the stream.\n", __FUNCTION__);
			assert(false);
			return 0;
		}
		if (pDecodec->m_nCodecId == AV_CODEC_ID_H264)
		{
			pThis->m_nVideoCodec = CODEC_H264;
			DxTraceMsg("%s Video Codec:%H.264 Width = %d\tHeight = %d.\n", __FUNCTION__, pDecodec->m_pAVCtx->width, pDecodec->m_pAVCtx->height);
		}
		else if (pDecodec->m_nCodecId == AV_CODEC_ID_HEVC)
		{
			pThis->m_nVideoCodec = CODEC_H265;
			DxTraceMsg("%s Video Codec:%H.265 Width = %d\tHeight = %d.\n", __FUNCTION__, pDecodec->m_pAVCtx->width, pDecodec->m_pAVCtx->height);
		}
		else
		{
			pThis->m_nVideoCodec = CODEC_UNKNOWN;
			DxTraceMsg("%s Unsupported Video Codec.\n", __FUNCTION__);
			assert(false);
			return 0;
		}
		pThis->m_nVideoWidth = pDecodec->m_pAVCtx->width;
		pThis->m_nVideoHeight = pDecodec->m_pAVCtx->height;
		// 初始显示组件
		// 使用线程内CDxSurface对象显示图象
		if (pThis->m_pDxSurface)		// D3D设备尚未初始化
		{
			DxSurfaceInitInfo InitInfo;
			InitInfo.nFrameWidth = pDecodec->m_pAVCtx->width;
			InitInfo.nFrameHeight = pDecodec->m_pAVCtx->height;
			if (pThis->m_bEnableHaccel)
				InitInfo.nD3DFormat = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
			else
				InitInfo.nD3DFormat = (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2');
			InitInfo.bWindowed = TRUE;
			InitInfo.hPresentWnd = pThis->m_hWnd;

			if (!pThis->m_pDxSurface->InitD3D(InitInfo.hPresentWnd,
				InitInfo.nFrameWidth,
				InitInfo.nFrameHeight,
				InitInfo.bWindowed,
				InitInfo.nD3DFormat))
			{
				DxTraceMsg("%s Initialize DxSurface failed.\n", __FUNCTION__);
				assert(false);
				return 0;
			}
		}

		pThis->m_pDecodec = pDecodec;
		pThis->ItLoop = pThis->m_listVideoCache.begin();
		// 恢复音频线程
		::EnterCriticalSection(&pThis->m_csAudioCache);
		pThis->m_listAudioCache.clear();
		::LeaveCriticalSection(&pThis->m_csAudioCache);
		if (pThis->m_hThreadPlayAudio)
			ResumeThread(pThis->m_hThreadPlayAudio);

		double dfT1 = GetExactTime() - pThis->m_nPlayInterval;
		double dfTimeSpan = 0.0f;
		AVPacket *pAvPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
		shared_ptr<AVPacket>AvPacketPtr(pAvPacket, av_free);		
		AVFrame *pAvFrame = av_frame_alloc();
		shared_ptr<AVFrame>AvFramePtr(pAvFrame, av_free);
		
		StreamFramePtr FramePtr;
		int nGot_picture = 0;
		DWORD nResult = 0;
		int nTimeSpan = 0;
		DWORD dwSleepPricision = GetSleepPricision();		// 取Sleep函数的时间精度
		int nFrameInterval = pThis->m_nFileFrameInterval;
		//StreamFramePtr FramePtr;
		pThis->m_dfTimesStart = GetExactTime();
		int nFramesProcessed = 0;	// 已处理的帧数

		// 取得当前显示器的刷新率，显示器的刷新率决定了，显示图像的最高帧数
		// 通过统计每显示一帧图像(含解码和显示)耗费的时间
		DEVMODE   dm;
		dm.dmSize = sizeof(DEVMODE);
		::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);		
		int nRefreshInvertal = 1000 / dm.dmDisplayFrequency;	// 显示器刷新间隔

		double dfT2 = GetExactTime();
		av_init_packet(pAvPacket);
#ifdef _DEBUG
		EnterCriticalSection(&pThis->m_csVideoCache);
		DxTraceMsg("%s Size of Video cache = %d .\n", __FUNCTION__, pThis->m_listVideoCache.size());
		LeaveCriticalSection(&pThis->m_csVideoCache);
#endif
#ifdef _DEBUG
		int TPlayArray[100] = { 0 };
		int nPlayCount = 1;
		int nFrames = 0;
#endif
		
		while (pThis->m_bThreadPlayVideoRun)
		{
			if (pThis->m_bPause)
			{
				Sleep(50);
				continue;
			}
			if (!pThis->m_bIpcStream)
			{// 文件或流媒体播放，可调节播放速度
				nTimeSpan = (int)(TimeSpanEx(dfT1) * 1000);
				if (nTimeSpan >= (pThis->m_nPlayInterval))
				{
					bool bPopFrame = false;
					if (pThis->m_nPlayInterval < nRefreshInvertal)	//播放间隔小于显示器刷新间隔，即播速度快于显示器刷新速度,则需要跳帧
					{// 查找时间上最匹配的帧,并删除不匹配的非I帧
						int nSkipFrames = 0;
						CAutoLock lock(&pThis->m_csVideoCache);
						for (auto it = pThis->m_listVideoCache.begin(); it != pThis->m_listVideoCache.end();)
						{
							time_t tFrameSpan = ((*it)->FrameHeader()->nTimestamp - pThis->m_tLastFrameTime)/1000;
							if (tFrameSpan >= nRefreshInvertal*pThis->m_fPlayRate
								|| StreamFrame::IsIFrame(*it))
							{
								bPopFrame = true;
								nFramesProcessed++;
								break;
							}
							else
							{
								it = pThis->m_listVideoCache.erase(it);
								nSkipFrames++; 
							}
						}
						DxTraceMsg("%s Skip Frames = %d bPopFrame = %s.\n", __FUNCTION__, nSkipFrames,bPopFrame?"true":"false");
						nFramesProcessed += nSkipFrames;
						if (bPopFrame)
						{
							FramePtr = pThis->m_listVideoCache.front();
							pThis->m_listVideoCache.pop_front();
							nFramesProcessed++;
						}
					}
					else
					{
						CAutoLock lock(&pThis->m_csVideoCache);
						if (pThis->m_listVideoCache.size() > 0)
						{
							FramePtr = pThis->m_listVideoCache.front();
							pThis->m_listVideoCache.pop_front();
							bPopFrame = true;
						}
					}
					
					if (!bPopFrame)
					{
						Sleep(10);
						continue;
					}
					else
					{
						dfT1 = GetExactTime();
						pAvPacket->data = (uint8_t *)FramePtr->Framedata();
						pAvPacket->size = FramePtr->FrameHeader()->nLength;
					}
				}
				else
				{
					int nSleepTime = pThis->m_nPlayInterval - nTimeSpan;
					if (nSleepTime >= dwSleepPricision)
						Sleep(nSleepTime);
					continue;
				}
			}
			else
			{// IPC 码流，则直接播放
				bool bPopFrame = false;
				::EnterCriticalSection(&pThis->m_csVideoCache);
				if (pThis->m_listVideoCache.size() > 0)
				{
					FramePtr = pThis->m_listVideoCache.front();
					pThis->m_listVideoCache.pop_front();
					bPopFrame = true;
				}
				::LeaveCriticalSection(&pThis->m_csVideoCache);
				if (!bPopFrame)
				{
					Sleep(10);
					continue;
				}
			}
			pAvPacket->data = (uint8_t *)FramePtr->Framedata();
			pAvPacket->size = FramePtr->FrameHeader()->nLength;
			pThis->m_tLastFrameTime = FramePtr->FrameHeader()->nTimestamp;
// 此为最大帧率真测试代码,建议不要删除
// xionggao.lee @2016.01.15 
// 			int nFPS = 25;
// 			int nTimespan1 = (int)(TimeSpanEx(pThis->m_dfTimesStart) * 1000);
// 			if (nTimespan1)
// 				nFPS  = nFrames * 1000 / nTimespan1;
// 	
// 			nTimeSpan = (int)(TimeSpanEx(dfT1) * 1000);
// 			TPlayArray[nPlayCount++] = nTimeSpan;
// 			TPlayArray[0] = nFPS;
// 			if (nPlayCount >= 50)
// 			{
// 				DxTraceMsg("%sPlay Interval:\n", __FUNCTION__);
// 				for (int i = 0; i < nPlayCount; i++)
// 				{
// 					DxTraceMsg("%02d\t", TPlayArray[i]);
// 					if ((i + 1) % 10 == 0)
// 						DxTraceMsg("\n");
// 				}
// 				DxTraceMsg(".\n");
// 				nPlayCount = 0;
// 			}
// 			dfT1 = GetExactTime();
// 			nFrames++;
// 			::EnterCriticalSection(&pThis->m_csVideoCache);
// 			if (pThis->m_listVideoCache.size() > 0)
// 			{
// 				FramePtr = pThis->m_listVideoCache.front();
// 				pThis->m_listVideoCache.pop_front();
// 			}
// 			::LeaveCriticalSection(&pThis->m_csVideoCache);
// 			
// 			pAvPacket->data = (uint8_t *)FramePtr->Framedata();
// 			pAvPacket->size = FramePtr->FrameHeader()->nLength;
			nAvError = pDecodec->Decode(pAvFrame, nGot_picture, pAvPacket);
			if (nAvError < 0)
			{
				av_strerror(nAvError, szAvError, 1024);
				DxTraceMsg("%s Decode error:%s.\n", __FUNCTION__, szAvError);
				continue;
			}
			av_packet_unref(pAvPacket);
			if (nGot_picture)
			{
				pThis->m_nCurVideoFrame = FramePtr->FrameHeader()->nFrameID;
				pThis->m_tCurFrameTimeStamp = FramePtr->FrameHeader()->nTimestamp;
				pThis->RenderFrame(pAvFrame);
				pThis->ProcessYVUCapture(pAvFrame, (LONGLONG)GetExactTime()*1000);
				if (pThis->m_pFilePlayCallBack)
					pThis->m_pFilePlayCallBack(pThis, pThis->m_pUserFilePlayer);
				av_frame_unref(pAvFrame);
			}
		}
		return 0;
	}
	static DWORD WINAPI ThreadPlayAudio(void *p)
	{
		CDvoPlayer *pThis = (CDvoPlayer *)p;
		int nAudioFrameInterval = pThis->m_nPlayInterval / 2;
		double dfT1 = GetExactTime() - nAudioFrameInterval;
		double dfTimeSpan = 0.0f;		
		DWORD nResult = 0;
		int nTimeSpan = 0;
		DWORD dwSleepPricision = GetSleepPricision();
		int nFrameInterval = 40;
		StreamFramePtr FramePtr;
		int nAudioError = 0;
		byte *pPCM = nullptr;
		shared_ptr<CAudioDecoder> pAudioDecoder = make_shared<CAudioDecoder>();
		bool bAudioDecoderInitialized = false;
		int nPCMSize = 0;
		int nDecodeSize = 0;
#ifdef _DEBUG
		double dfTPlay = GetExactTime();
		double TPlayArray[50] = { 0 };
		int nPlayCount = 0;
#endif
		while (pThis->m_bThreadPlayAudioRun)
		{
			nTimeSpan = (int)((GetExactTime() - dfT1) * 1000);
			if (pThis->m_fPlayRate != 1.0f)
			{// 只有正常倍率才播放声音
				if (pThis->m_pDsBuffer->IsPlaying())
					pThis->m_pDsBuffer->StopPlay();
				::EnterCriticalSection(&pThis->m_csAudioCache);
				if (pThis->m_listAudioCache.size() > 0)	
					pThis->m_listAudioCache.pop_front();
				::LeaveCriticalSection(&pThis->m_csAudioCache);
			}
			
			if (nTimeSpan >= nAudioFrameInterval)
			{
				if (nTimeSpan > 100)			// 连续100ms没有音频数据，则视为音频暂停
					pThis->m_pDsBuffer->StopPlay();
				else if(!pThis->m_pDsBuffer->IsPlaying())
					pThis->m_pDsBuffer->StartPlay();
				
				bool bPopFrame = false;
				::EnterCriticalSection(&pThis->m_csAudioCache);
				if (pThis->m_listAudioCache.size() > 0)
				{
					FramePtr = pThis->m_listAudioCache.front();
					pThis->m_listAudioCache.pop_front();
					bPopFrame = true;
				}
				::LeaveCriticalSection(&pThis->m_csAudioCache);
				if (!bPopFrame)
				{
					Sleep(10);
					continue;
				}
				dfT1 = GetExactTime();
			}
			else
			{	// 控制播放速度
				int nSleepTime = nAudioFrameInterval - nTimeSpan;
				if (nSleepTime > dwSleepPricision)
					Sleep(nSleepTime);
				continue;
			}
			
			if (pAudioDecoder->GetCodecType() == CODEC_UNKNOWN)
			{
				const DVOFrameHeaderEx *pHeader = FramePtr->FrameHeader();
				nDecodeSize = pHeader->nLength * 2;		//G711 压缩率为2倍
				switch (pHeader->nType)
				{
				case FRAME_G711A:			//711 A律编码帧
				{
					pAudioDecoder->SetACodecType(CODEC_G711A, SampleBit16);
					pThis->m_nAudioCodec = CODEC_G711A;
					DxTraceMsg("%s Audio Codec:G711A.\n", __FUNCTION__);
					break;
				}
				case FRAME_G711U:			//711 U律编码帧
				{
					pAudioDecoder->SetACodecType(CODEC_G711U, SampleBit16);
					pThis->m_nAudioCodec = CODEC_G711U;
					DxTraceMsg("%s Audio Codec:G711U.\n", __FUNCTION__);
					break;
				}

				case FRAME_G726:			//726编码帧
				{
					// 因为目前DVO相机的G726编码,虽然采用的是16位采样，但使用32位压缩编码，因此解压得使用SampleBit32
					pAudioDecoder->SetACodecType(CODEC_G726, SampleBit32);
					pThis->m_nAudioCodec = CODEC_G726;
					nDecodeSize = FramePtr->FrameHeader()->nLength * 8;		//G726最大压缩率可达8倍
					DxTraceMsg("%s Audio Codec:G726.\n", __FUNCTION__);
					break;
				}
				case FRAME_AAC:				//AAC编码帧。
				{
					pAudioDecoder->SetACodecType(CODEC_AAC, SampleBit16);
					pThis->m_nAudioCodec = CODEC_AAC;
					nDecodeSize = FramePtr->FrameHeader()->nLength * 24;
					DxTraceMsg("%s Audio Codec:AAC.\n", __FUNCTION__);
					break;
				}
				default:
				{
					assert(false);
					DxTraceMsg("%s Unspported audio codec.\n", __FUNCTION__);
					return 0;
					break;
				}
				}
			}
			if (nPCMSize < nDecodeSize)
			{
				if (pPCM)
					delete[]pPCM;
				pPCM = new byte[nDecodeSize];
				nPCMSize = nDecodeSize;
			}

			if (pThis->m_pDsBuffer->IsPlaying() &&
				pAudioDecoder->Decode(pPCM, nPCMSize, (byte *)FramePtr->Framedata(), FramePtr->FrameHeader()->nLength) != 0)
			{
				if (!pThis->m_pDsBuffer->WritePCM(pPCM, nPCMSize))
				{
					DxTraceMsg("%s Write PCM Failed.\n", __FUNCTION__);
				}
// #ifdef _DEBUG
// 				TPlayArray[nPlayCount++] = TimeSpanEx(dfT1);	
// 				dfT1 = GetExactTime();
// 				if (nPlayCount >= 50)
// 				{
// 					DxTraceMsg("%sPlay Interval:\n", __FUNCTION__);
// 					for (int i = 0; i < nPlayCount; i++)
// 					{
// 						DxTraceMsg("%.3f\t", TPlayArray[i]);
// 						if ((i+1) % 10 == 0) 
// 							DxTraceMsg("\n");
// 					}
// 					DxTraceMsg(".\n");
// 					nPlayCount = 0;
// 				}
// #endif
			}
		}
		if (pPCM)
			delete[]pPCM;
		return 0;
	}
	
};