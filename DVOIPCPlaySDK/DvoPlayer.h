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
#include <windows.h>
#include <winsock2.h>
#include <process.h>
#include <thread>
#include <chrono>
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")
#include "DVOMedia.h"
#include "dvoipcnetsdk.h"
#include "DVOIPCPlaySDK.h"
#include "./DxSurface/DxSurface.h"
#include "AutoLock.h"
#include "./VideoDecoder/VideoDecoder.h"
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

#define FrameSize(p)	(((DVOFrameHeaderEx*)pBuffer)->nLength + sizeof(DVOFrameHeaderEx))	
#define Frame(p)		((DVOFrameHeaderEx *)p)

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
	StreamFrame(IN byte *pFrame, IN int nFrameType, IN int nFrameLength, int nFrameNum, time_t nFrameTime = 0)
	{
		nSize = sizeof(DVOFrameHeaderEx) + nFrameLength;
		pInputData = _New byte[nSize];
		if (!pInputData)
		{
			assert(false);
			return;
		}
		DVOFrameHeaderEx *pHeader = (DVOFrameHeaderEx *)pInputData;	
		switch (nFrameType)
		{
		case 0:									// 海思I帧号为0，这是固件的一个BUG，有待修正
		case APP_NET_TCP_COM_DST_IDR_FRAME: 	// IDR帧。
		case APP_NET_TCP_COM_DST_I_FRAME:		// I帧。
			pHeader->nType = FRAME_I;
			break;
		case APP_NET_TCP_COM_DST_P_FRAME:       // P帧。
		case APP_NET_TCP_COM_DST_B_FRAME:       // B帧。
			pHeader->nType = FRAME_B;
			break;
		case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A律编码帧
		case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U律编码帧
		case APP_NET_TCP_COM_DST_726:           // 726编码帧
		case APP_NET_TCP_COM_DST_AAC:           // AAC编码帧。
			pHeader->nType = FRAME_AUDIO;
			break;
		default:
		{
			assert(false);
			break;
		}
		}
		pHeader->nType			 = nFrameType;
		pHeader->nFrameTag		 = DVO_TAG;		///< DVO_TAG
		if (!nFrameTime)
			pHeader->nFrameUTCTime = time(NULL);
		else
			pHeader->nFrameUTCTime = nFrameTime;
		pHeader->nTimestamp		 = (__int64)GetExactTime()*1000*1000;
		pHeader->nLength		 = nFrameLength;
		pHeader->nFrameID		 = nFrameNum;		
		memcpy(pInputData + sizeof(DVOFrameHeaderEx), pFrame, nFrameLength);
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
		ZeroMemory(this, sizeof(DVOFrameHeaderEx));
#endif
	}
	int		nSize;	
	byte	*pInputData;
};
typedef shared_ptr<StreamFrame> StreamFramePtr;

#define _Frame_PERIOD			30.0f		///< 一个帧率区间
#pragma pack(push)
#pragma pack(1)
struct FrameInfo
{
	time_t  tRecvTime;				///< 收到帧的时间 单位毫秒
	int		nFrameSize;
};

/// @brief 解析DVO私有帧结构体
struct FrameParser
{
	DVOFrameHeaderEx	*pHeaderEx;		///< DVO私有录像的帧数据
	UINT				nFrameSize;		///< pFrame的数据长度
	byte				*pLawFrame;		///< 原始码流数据
	UINT				nLawFrameSize;	///< 原始码流数据长度
};

struct StreamInfo
{
	UINT	nFrameID;
	bool	bRecvIFrame;		///< 是否收到第个I帧
	int		nVideoFrameCount;
	int		nAudioFrameCount;
	__int64 nVideoBytes;		///< 收到视频总字节数
	__int64 nAudioBytes;		///< 收到音频总字节数
	time_t  tFirstTime;			///< 接收数据的起始时间	单位毫秒
	time_t	tLastTime;			///< 最后一次接收数据时间 单位毫秒
	CRITICAL_SECTION	csFrameList;
	list<FrameInfo>FrameInfoList;
	StreamInfo()
	{
		ZeroMemory(this, offsetof(StreamInfo, FrameInfoList));
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
			return nSize * 1000 / (int)(tSpan);
		}
		else
			return 1;
	}

	int	GetVideoCodeRate(int nUnit = 1024)	///< 取得视频码率(Kbps)
	{
		time_t tSpan = 25;
		CAutoLock lock(&csFrameList);
		if (FrameInfoList.size() > 1)
			tSpan = FrameInfoList.back().tRecvTime - FrameInfoList.front().tRecvTime;
		__int64 nTotalBytes = 0;

		for (list<FrameInfo>::iterator it = FrameInfoList.begin();
			it != FrameInfoList.end();
			it++)
			nTotalBytes += (*it).nFrameSize;

		if (tSpan == 0)
			tSpan = 1;
		__int64 nRate = (nTotalBytes * 1000 * 8 / ((int)tSpan*nUnit));
		return (int)nRate;
	}
	int GetAudioCodeRate(int nUnit = 1024)
	{
		time_t tSpan = (tLastTime - tFirstTime);
		if (tSpan == 0)
			tSpan = 1;
		return (int)(nAudioBytes * 1000 / (tSpan*nUnit));
	}
	void inline Reset()
	{
		ZeroMemory(this, offsetof(StreamInfo, csFrameList));
		CAutoLock lock(&csFrameList);
		FrameInfoList.clear();
	}
	void PushFrameInfo(int nFrameSize)
	{
		time_t tNow = (time_t)(GetExactTime() * 1000);
		CAutoLock lock(&csFrameList);
		if (FrameInfoList.size() > 1)
		{
			for (list<FrameInfo>::iterator it = FrameInfoList.begin();
				it != FrameInfoList.end();)
			{
				if ((tNow - (*it).tRecvTime) > _Frame_PERIOD * 1000)
					it = FrameInfoList.erase(it);
				else
					it++;
			}
		}

		FrameInfo fi;
		fi.tRecvTime = tNow;
		fi.nFrameSize = nFrameSize;
		FrameInfoList.push_back(fi);
	}
};

/// @brief 线程休息试
enum SleepWay
{
	Sys_Sleep = 0,		///< 直接调用系统函数Sleep
	Wmm_Sleep = 1,		///< 使用多媒体时间提高精度
	Std_Sleep = 2		///< 使C++11提供的线程休眠函数
};
typedef shared_ptr<StreamInfo> StreamInfoPtr;
#pragma pack(pop)


class CDvoPlayer
{
public:
	int		nSize;
private:
	StreamInfoPtr m_pStreamInfo;		///< 视频流信息	
	shared_ptr<DVO_MEDIAINFO>m_pMediaHeader;/// 媒体文件头
	list		<StreamFramePtr>m_listVideoCache;///< 流播放帧缓冲
	list		<StreamFramePtr>m_listAudioCache;///< 流播放帧缓冲
	CRITICAL_SECTION	m_csVideoCache;
	CRITICAL_SECTION	m_csAudioCache;
	int			m_nMaxFrameCache;		///< 最大视频缓冲数量,默认值100
										///< 当m_FrameCache中的视频帧数量超过m_nMaxFrameCache时，便无法再继续输入流数据
private:
	// 视频播放相关变量
	int			m_nVideoFrames;			///< 当前缓存中视频帧数量	
	int			m_nTotalFrames;			///< 当前文件中有效音频帧的数量,仅当播放文件时有效
	HWND		m_hWnd;					///< 播放视频的窗口句柄
	DVO_CODEC	m_nVideoCodec;			///< 视频编码格式 @see DVO_CODEC	
	static		CAvRegister avRegister;
	static shared_ptr<CDSoundPlayer> m_pDsPlayer;///< DirectSound播放对象指针
	CDxSurface* m_pDxSurface;			///< Direct3d Surface封装类,用于显示视频
	int			m_nVideoWidth;			///< 视频宽度
	int			m_nVideoHeight;			///< 视频高度
	int			m_nCurFrameID;			///< 当前视频帧ID	
	int			m_nFrameEplased;		///< 已经播放帧数
	int			m_nCurVideoFrame;		///< 当前正播放的视频帧ID		
	int			m_nPlayRate;			///< 当前播放的倍率取值-32~32,-32时为32之一束，32则为32倍速
	double		m_dfLastTimeVideoPlay;	///< 前一次视频播放的时间
	double		m_dfTimesStart;			///< 开始播放的时间
	bool		m_bEnableHaccel;		///< 是否启用硬解码

private:	// 音频播放相关变量
	
	CAudioDecoder *m_pAudioDecoder;		///< 音频解码器
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
	HANDLE		m_hThreadFileAbstract;	///< 文件信息摘要线程
	volatile bool m_bThreadParserRun;
	volatile bool m_bThreadPlayRun;
	volatile bool m_bThreadFileAbstractRun;
	bool		m_bPause;				///< 是否处于暂停状态
	byte		*m_pYUV;				///< YVU捕捉专用内存
	int			m_nYUVSize ;			///<
	shared_ptr<byte>m_pYUVPtr;
private:	// 文件播放相关变量
	HANDLE		m_hDvoFile;				///< 正在播放的文件句柄
	char		*m_pszFileName;			///< 正在播放的文件名,该字段仅在文件播放时间有效
	UINT		*m_pFrameOffsetTable;	///< 视频帧ID对应文件偏移表
	volatile LONG m_nSeekOffset;		///< 读文件的偏移
	CRITICAL_SECTION	m_csSeekOffset;
	int			m_nSDKVersion;			///< 生成录像文件的SDK版本	
	int			m_nMaxFrameSize;		///< 最大I帧的大小，以字节为单位,默认值256K
	int			m_nFPS;	
	int			m_nFrameInterval;		///< 帧播放间隔,单位毫秒
private:
	CaptureFrame	m_pfnCaptureFrame;
	void*			m_pUserCaptureFrame;
	CaptureYUV		m_pfnCaptureYUV;
	void*			m_pUserCaptureYUV;
	CaptureYUVEx	m_pfnCaptureYUVEx;
	void*			m_pUserCaptureYUVEx;
	SleepWay		m_nSleepWay;		///< 线程休眠的方式,@see SleepWay
public:
	
private:
	CDvoPlayer()
	{
		ZeroMemory(&m_listVideoCache, sizeof(CDvoPlayer) - offsetof(CDvoPlayer, m_listVideoCache));
		InitializeCriticalSection(&m_csVideoCache);
		InitializeCriticalSection(&m_csAudioCache);
		m_nMaxFrameSize = 1024 * 256;
		nSize = sizeof(CDvoPlayer);
	}
	LONG GetSeekOffset()
	{
		CAutoLock lock(&m_csSeekOffset);
		return m_nSeekOffset;
	}
	void SetSeekOffset(UINT nSeekOffset)
	{
		CAutoLock lock(&m_csSeekOffset);
		m_nSeekOffset = nSeekOffset;
	}

	/// @brief 取得视频文件最一帧的ID,相当于视频文件中包含的视频总帧数
	int GetLastFrameID(int &nLastFrameID)
	{
		if (!m_hDvoFile)
			return DVO_Error_FileNotOpened;		
		byte *pBuffer = _New byte[m_nMaxFrameSize];
		shared_ptr<byte>TempBuffPtr(pBuffer);
		if (SetFilePointer(m_hDvoFile, m_nMaxFrameSize, nullptr, FILE_END)==INVALID_SET_FILE_POINTER)
			return GetLastError();
		DWORD nBytesRead = 0;
		DWORD nDataLength = m_nMaxFrameSize;
		if (!ReadFile(m_hDvoFile, pBuffer, nDataLength, &nBytesRead, nullptr))
		{
			DxTraceMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
			return GetLastError();
		}
		
		char *szKey = "MOVD";
		int nOffset = KMP_StrFind(pBuffer, nDataLength, (byte *)szKey, 4);
		if (nOffset < 0)
			return DVO_Error_MaxFrameSizeNotEnough;
		nOffset -= offsetof(DVOFrameHeaderEx, nFrameTag);
		if (nOffset < 0)
			return DVO_Error_NotDvoVideoFile;
		// 遍历所有最后m_nMaxFrameSize长度内的所有帧
		bool bFoundVideo = false;
		FrameParser Parser;
#ifdef _DEBUG
		int nAudioFrames = 0;
		int nVideoFrames = 0;
		while (ParserFrame(&pBuffer, nDataLength, &Parser))
		{
			if (Parser.pHeaderEx->nType == FRAME_B ||
				Parser.pHeaderEx->nType == FRAME_I ||
				Parser.pHeaderEx->nType == FRAME_P )
			{
				nVideoFrames++;
				nLastFrameID = Parser.pHeaderEx->nFrameID;
				bFoundVideo = true;
			}
			else if (Parser.pHeaderEx->nType == FRAME_AUDIO)
				nAudioFrames ++;

			nOffset += FrameSize(&pBuffer[nOffset]);
		}
		DxTraceMsg("%s In Last %d bytes:VideoFrames:%d\tAudioFrames:%d.\n",__FUNCTION__,nVideoFrames,nAudioFrames);
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
public:

	CDvoPlayer(HWND hWnd,CHAR *szFileName =  NULL)
	{
// 		assert(hWnd != nullptr);
// 		assert(IsWindow(hWnd));
		ZeroMemory(&m_listVideoCache, sizeof(CDvoPlayer) - offsetof(CDvoPlayer, m_listVideoCache));
		InitializeCriticalSection(&m_csVideoCache);
		InitializeCriticalSection(&m_csAudioCache);
		InitializeCriticalSection(&m_csSeekOffset);			
		if (szFileName)
		{
			m_pszFileName = _New char[MAX_PATH];
			strcpy(m_pszFileName, szFileName);
		}
		if (hWnd && IsWindow(hWnd))
		{// 必采要有窗口才会播放视频和声音
			m_hWnd = hWnd;
			m_pDxSurface = _New CDxSurface();
			//m_DsPlayer = _New CDSoundPlayer();
			//m_DsPlayer.Initialize(m_hWnd, Audio_Play_Segments);
		}
		m_nMaxFrameSize = 1024 * 256;
		m_nFPS		  = 25;				// FPS的默认值为25
		m_nVideoCodec = CODEC_H264;		// 视频默认使用H.264编码
		m_nAudioCodec = CODEC_G711U;	// 音频默认使用G711U编码
		m_nMaxFrameCache = 100;			// 默认最大视频缓冲数量为100
		nSize = sizeof(CDvoPlayer);
	}
	
	~CDvoPlayer()
	{
		StopPlay();
		if (m_hWnd)
		{
			if (m_pDxSurface)
				delete m_pDxSurface;
// 			if (m_DsPlayer)
// 				delete m_DsPlayer;
#ifdef _DEBUG
			m_pDxSurface = nullptr;
			//m_DsPlayer = nullptr;
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

	// 设置流播放头
	// 在流播放时，播放之前，必须先设置流头
	int SetStreamHeader(CHAR *szStreamHeader, int nHeaderSize, IN int nMaxFramesCache)
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
	void SetMaxFrameSize(int nMaxFrameSize = 256*1024)
	{
		m_nMaxFrameSize = nMaxFrameSize;
	}
	inline int GetMaxFrameSize()
	{
		return m_nMaxFrameSize;
	}
	// 开始播放
	int StartPlay(bool bEnaleAudio = false,bool bEnableHaccel = false)
	{
		m_bPause = false;
		if (m_pszFileName)
		{
			// 打开文件
			m_hDvoFile = CreateFileA(m_pszFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_ARCHIVE,
				NULL);
			if (!m_hDvoFile)
			{
				return GetLastError();
			}
			
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
					m_nFPS = m_pMediaHeader->nFps;
				break;
			case DVO_IPC_SDK_VERSION_2015_10_20:
			case DVO_IPC_SDK_VERSION_2015_12_16:
				m_nFPS = m_pMediaHeader->nFps;
				break;
			default:
				return DVO_Error_InvalidSDKVersion;
			}
			m_nFrameInterval = 1000 / m_nFPS;
			int nDvoError = GetLastFrameID(m_nTotalFrames);
			if (nDvoError != DVO_Succeed)
			{
				CloseHandle(m_hDvoFile);
				return nDvoError;
			}
			// GetLastFrameID取得的是最后一帧的ID，总帧数还要在此基础上+1
			m_nTotalFrames++;
			
			// 启动文件解析线程
			m_bThreadParserRun = true;
			m_hThreadParser = (HANDLE)_beginthreadex(nullptr, 0, ThreadParser, this, 0, 0);
			// 启动文件信息摘要线程
			m_bThreadFileAbstractRun = true;
			m_hThreadFileAbstract = (HANDLE)_beginthreadex(nullptr, 0, ThreadFileAbstract, this, 0, 0);
		}
		else
		{
			if (!m_pMediaHeader)
				return DVO_Error_NotInputStreamHeader;		// 未设置流头
		}
		if (m_hWnd)
		{
			// 启动流播放线程
			m_bThreadPlayRun = true;			
			m_hThreadPlayVideo = (HANDLE)_beginthreadex(nullptr, 0, ThreadPlayVideo, this, 0, 0);			
			m_hThreadPlayVideo = (HANDLE)_beginthreadex(nullptr, 0, ThreadPlayAudio, this, 0, 0);
			if (bEnaleAudio/* && m_DsPlayer*/)
			{
				m_pDsPlayer->StartPlay();
				m_bEnableAudio = true;	
			}
		}
		
		return DVO_Succeed;
	}
		

	/// @brief			输入DVO私格式的码流数据
	/// @retval			0	操作成功
	/// @retval			1	流缓冲区已满
	/// @retval			-1	输入参数无效
	/// @remark			播放流数据时，相应的帧数据其实并未立即播放，而是被放了播放队列中，应该根据dvoplay_PlayStream
	///					的返回值来判断，是否继续播放，若说明队列已满，则应该暂停播放
	int InputStream(unsigned char *szFrameData, int nFrameSize)
	{
		StreamFramePtr pStream = make_shared<StreamFrame>(szFrameData, nFrameSize);
		if (!pStream)
			return DVO_Error_InsufficentMemory;
		const DVOFrameHeaderEx* pHeaderEx = pStream->FrameHeader();
		switch (pHeaderEx->nType)
		{
			case FRAME_P:
			case FRAME_B:
			case FRAME_I:
			{
				CAutoLock lock(&m_csVideoCache);
				if (m_nVideoFrames >= m_nMaxFrameCache)
					return DVO_Error_FrameCacheIsFulled;
				
				m_nVideoFrames++;
				m_listVideoCache.push_back(pStream);
			}
			case FRAME_AUDIO:
			{
				CAutoLock lock(&m_csAudioCache);
				m_listAudioCache.push_back(pStream);
				m_nAudioFrames++;
			}
			default:
			{
				assert(false);
				return DVO_Error_InvalidFrameType;
				break;
			}
		}
	}

	/// @brief			输入DVO IPC码流数据
	/// @retval			0	操作成功
	/// @retval			1	流缓冲区已满
	/// @retval			-1	输入参数无效
	/// @remark			播放流数据时，相应的帧数据其实并未立即播放，而是被放了播放队列中，应该根据dvoplay_PlayStream
	///					的返回值来判断，是否继续播放，若说明队列已满，则应该暂停播放
	int InputStream(IN byte *pFrameData, IN int nFrameType, IN int nFrameLength, int nFrameNum, time_t nFrameTime = 0)
	{
		StreamFramePtr pStream = make_shared<StreamFrame>(pFrameData, nFrameType, nFrameLength, nFrameNum, nFrameTime);
		if (!pStream)
			return DVO_Error_InsufficentMemory;
		switch (nFrameType)
		{
			case 0:									// 海思I帧号为0，这是固件的一个BUG，有待修正
			case APP_NET_TCP_COM_DST_IDR_FRAME: 	// IDR帧。
			case APP_NET_TCP_COM_DST_I_FRAME:		// I帧。		
			case APP_NET_TCP_COM_DST_P_FRAME:       // P帧。
			case APP_NET_TCP_COM_DST_B_FRAME:       // B帧。
			{
				CAutoLock lock(&m_csVideoCache);
				if (m_nVideoFrames >= m_nMaxFrameCache)
					return DVO_Error_FrameCacheIsFulled;
				m_nVideoFrames++;				
				m_listVideoCache.push_back(pStream);
			}
				break;
			case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A律编码帧
			case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U律编码帧
			case APP_NET_TCP_COM_DST_726:           // 726编码帧
			case APP_NET_TCP_COM_DST_AAC:           // AAC编码帧。
			{
				CAutoLock lock(&m_csAudioCache);				
				m_nAudioFrames++;
				m_listAudioCache.push_back(pStream);
			}
				break;
			default:
			{
				assert(false);
				break;
			}
		}
	}

	void StopPlay()
	{
		m_bThreadParserRun = false;
		m_bThreadPlayRun = false;
		m_bThreadFileAbstractRun = false;
		m_bPause = false;
		if (m_hThreadParser)
		{
			WaitForSingleObject(m_hThreadParser, INFINITE);
			CloseHandle(m_hThreadParser);
		}
		if (m_hThreadPlayVideo)
		{
			WaitForSingleObject(m_hThreadPlayVideo, INFINITE);
			CloseHandle(m_hThreadPlayVideo);
		}
		if (m_hThreadPlayAudio)
		{
			WaitForSingleObject(m_hThreadPlayAudio, INFINITE);
			CloseHandle(m_hThreadPlayAudio);
		}
		if (m_hThreadFileAbstract)
		{
			WaitForSingleObject(m_hThreadFileAbstract, INFINITE);
			CloseHandle(m_hThreadFileAbstract);
		}
		if (/*m_DsPlayer && */m_bEnableAudio)
		{
			m_pDsPlayer->StopPlay();
		}
		
#ifdef _DEBUG
		m_hThreadPlayVideo = nullptr;		
		m_hThreadParser = nullptr;
		m_hThreadFileAbstract = nullptr;
		m_hThreadPlayAudio = nullptr;
#endif
		m_listVideoCache.clear();
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
		pDecoder->InitDecoder(1280, 720, nAvCodec, true);
		if (pDecoder->CodecIsSupported(nAvCodec) == S_OK)		
			return true;
		else
			return false;
	}

	/// @brief			返回流播放时，当前播放队列中的视频帧的数量
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	/// @remark			播放流数据时，相应的帧数据其实并未立即播放，而是被放了播放队列中，应该根据PlayStream
	///					的返回值来判断，是否继续播放，若说明队列已满，则应该暂停播放
	inline int GetCacheSize(int &nCacheCount)
	{
		return m_nVideoFrames;
	}

	/// @brief			取得文件播放的视频帧率
	inline int  GetFps()
	{
		return m_nFPS;
	}

	/// @brief			取得文件中包括的有效视频帧总数量
	/// @remark			只有在文件播放时才有效
	inline int  GetFrames()
	{
		if (m_hDvoFile)
			return m_nTotalFrames;
		else
			return DVO_Error_NotFilePlayer;
	}

	/// @brief			取得当前播放视频的帧ID
	int  GetCurrentFrameID()
	{
		if (m_hThreadPlayVideo)
			return m_nCurVideoFrame;
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
		if (m_bEnableAudio/* && m_DsPlayer*/)
		{
			int nDsVolume = nVolume * 100 - 10000;
			m_pDsPlayer->SetVolume(nDsVolume);
		}
	}

	/// @brief			取得当前播放的音量
	int  GetVolume()
	{
		if (m_bEnableAudio/* && m_DsPlayer*/)
			return (m_pDsPlayer->GetVolume() + 10000) / 100;
		else
			return 0;
	}

	/// @brief			取得当前播放的速度的倍率
	inline int  GetRate()
	{
		return m_nPlayRate;
	}

	/// @brief			设置当前播放的速度的倍率
	/// @param [in]		nPlayRate		当前的播放的倍率,@see PlayRate
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	inline int  SetRate(IN PlayRate nPlayRate)
	{
		switch (nPlayRate)
		{
		case Rate_One32th:		
		case Rate_One24th:
		case Rate_One16th:
		case Rate_One10th:
		case Rate_One08th:
		case Rate_Quarter:
		case Rate_Half:
		{
			m_nFrameInterval = 1000* (-1)*nPlayRate / m_nFPS;
			break;
		}
		case Rate_01X:
		case Rate_02X:
		case Rate_04X:
		case Rate_08X:
		case Rate_10X:
		case Rate_16X:
		case Rate_20X:
		case Rate_32X:
		{
			m_nFrameInterval = 1000 / m_nFPS*nPlayRate;
			break;
		}
		default:
		{
			assert(false);
			return -1;
		}
		}
		m_nPlayRate = nPlayRate;
		return DVO_Succeed;
	}

	/// @brief			跳跃到指视频帧进行播放
	/// @param [in]		nFrameID		要播放帧的起始ID
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	/// @remark			若所指定帧为非关键帧，帧自动移动到就近的关键帧进行播放
	int  SeekFrame(IN int nFrameID)
	{
		if (!m_hDvoFile || !m_pFrameOffsetTable)
			return DVO_Error_NotFilePlayer;
		if (nFrameID < 0 || nFrameID > m_nTotalFrames)
			return DVO_Error_InvalidFrame;		

		EnterCriticalSection(&m_csVideoCache);
		m_listVideoCache.clear();
		LeaveCriticalSection(&m_csVideoCache);

		EnterCriticalSection(&m_csAudioCache);
		m_listAudioCache.clear();
		LeaveCriticalSection(&m_csAudioCache);
		
		// 从文件摘要中，取得文件偏移信息
		SetSeekOffset(m_pFrameOffsetTable[nFrameID]);
		return DVO_Succeed;
	}

	/// @brief			跳跃到指定时间偏移进行播放
	/// @param [in]		nTimeSet		要播放的起始时间,单位秒,如FPS=25,则0.04秒为第2帧，1.00秒，为第25帧
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	/// @remark			若所指定时间点对应帧为非关键帧，帧自动移动到就近的关键帧进行播放
	int  SeekTime(IN double nTimeSet)
	{
		if (!m_hDvoFile || !m_pFrameOffsetTable)
			return DVO_Error_NotFilePlayer;
		
		int nFrameID = (int)(nTimeSet * m_nFPS);
		return SeekFrame(nFrameID);
	}

	/// @brief			播放下一帧
	/// @param [in]		nTimeSet		要播放的起始时间
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	/// @remark			该函数仅适用于单帧播放
	int  SeekNextFrame()
	{

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
		if (m_nPlayRate > 4)
			return DVO_Succeed;
		if (m_bEnableAudio)
			return DVO_Succeed;
		if (bEnable)
		{
			m_bEnableAudio = true;
			m_dfLastTimeAudioPlay = 0.0f;
			m_dfLastTimeAudioSample = 0.0f;
			m_pDsPlayer->StartPlay();
		}
		else
		{
			m_bEnableAudio = false;
			m_pDsPlayer->StopPlay();
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

	/// @brief			获取已放时间,单位毫秒
	/// @param [in]		hPlayHandle		由OpenFile或OpenStream返回的播放句柄
	/// @retval			0	操作成功
	/// @retval			-1	输入参数无效
	inline LONGLONG GetEplaseTime()
	{
		return (LONGLONG)(1000 * (GetExactTime() - m_dfTimesStart));
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
	bool ParserFrame(INOUT byte **pBuffer,
					 INOUT DWORD &nDataSize, 
					 FrameParser* pFrameParser)
	{
		int nOffset = 0;
		if (Frame(*pBuffer)->nFrameTag != DVO_TAG)
		{
			static char *szKey = "MOVD";
			nOffset = KMP_StrFind(*pBuffer, nDataSize, (byte *)szKey, 4);
			nOffset -= offsetof(DVOFrameHeaderEx, nFrameTag);
		}
		else
			return false;
		if (nOffset < 0)
			return false;
		
		// 帧头信息不完整
		if ((nOffset + sizeof(DVOFrameHeaderEx)) >= nDataSize)
			return false;

		*pBuffer += nOffset;
		
		// 帧数据不完整
		if (nOffset + FrameSize(*pBuffer) >= nDataSize)
			return false;
		
		if (pFrameParser)
		{
			pFrameParser->pHeaderEx = (DVOFrameHeaderEx *)(*pBuffer);
			pFrameParser->nFrameSize = FrameSize(*pBuffer);
			pFrameParser->pLawFrame = *pBuffer + sizeof(DVOFrameHeaderEx);
			pFrameParser->nLawFrameSize = Frame(*pBuffer)->nLength;
		}
		*pBuffer += FrameSize(*pBuffer);
		nDataSize -= FrameSize(*pBuffer);
		return true;
	}
	static UINT __stdcall ThreadParser(void *p)
	{// 若指定了有效的窗口句柄，则把解析后的文件数据放入播放队列，否则不放入播放队列
		CDvoPlayer* pThis = (CDvoPlayer *)p;
		LONG nSeekOffset = 0;
		DWORD nBufferSize = 1024 * 1024;
		DWORD nBytesRead = 0;
		DWORD nDataLength = 0;
		byte *pBuffer = _New byte[nBufferSize];
		shared_ptr<byte>BufferPtr(pBuffer);
		FrameParser Parser;
		while (pThis->m_bThreadParserRun)
		{
			if (nSeekOffset = pThis->GetSeekOffset())
			{
				pThis->SetSeekOffset(0);
				if (SetFilePointer(pThis->m_hDvoFile, nSeekOffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)				
					DxTraceMsg("%s SetFilePointer Failed,Error = %d.\n",__FUNCTION__, GetLastError());
			}
			if (!ReadFile(pThis->m_hDvoFile, &pBuffer[nDataLength], (nBufferSize - nDataLength), &nBytesRead, nullptr))
			{
				DxTraceMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return 0;
			}
			nDataLength += nBytesRead;
			byte *pFrameBuffer = pBuffer;
			bool bPause = false;
			while (pThis->m_bThreadParserRun)
			{
				if (!pThis->ParserFrame(&pFrameBuffer, nDataLength, &Parser))
					break;
				if (pThis->m_pfnCaptureFrame)
				{
					if (pThis->m_pfnCaptureFrame(pThis, (byte *)Parser.pHeaderEx, Parser.nFrameSize, pThis->m_pUserCaptureFrame) == 1)
					{
						bPause = true;
						Sleep(20);
					}
				}
				if (!pThis->m_hWnd)		// 没有窗口句柄，则不进行播放
				{
					continue;
				}
				if (pThis->InputStream((byte *)Parser.pHeaderEx, Parser.nFrameSize) == DVO_Error_InvalidFrameType)
				{
					Sleep(10);
				}
			}
			// 残留数据长为nDataLength
			memmove(pBuffer, pFrameBuffer, nDataLength);
#ifdef _DEBUG
			ZeroMemory(&pBuffer[nDataLength],nBufferSize - nDataLength);
#endif
			// 若是单纯解析数据线程，则需要暂缓读取数据
			if (!pThis->m_hWnd || !bPause)
			{
				Sleep(10);
			}
		}
		return 0;
	}

	/// @brief			解析帧ID
	/// @param [in,out]	pBuffer			来自DVO私有录像文件中的数据流
	/// @param [in,out]	nDataSize		pBuffer中有效数据的长度
	/// @param [in]		nStartOffset	pBuffer中数据的在文件中的偏移
	/// @retval			true	操作成功
	/// @retval			false	失败，pBuffer已经没有有效的帧数据
	bool ParserFrameID(INOUT byte *pBuffer,INOUT DWORD nDataSize,DWORD nStartFileOffset,int &nCurFrames)
	{
		DVOFrameHeaderEx *pFrameHeader = (DVOFrameHeaderEx*)(pBuffer);
		int nOffset = 0;
		if (pFrameHeader->nFrameTag != DVO_TAG)
		{
			static char *szKey = "MOVD";
			nOffset = KMP_StrFind(pBuffer, nDataSize, (byte *)szKey, 4);
			nOffset -= offsetof(DVOFrameHeaderEx, nFrameTag);
		}
		else
			return false;
		if (nOffset < 0)
			return false;
		
		// 遍历所有最后pBuffer内的所有帧
		while (true)
		{
			if (nOffset >= nDataSize)
				return false;

			// 帧头信息不完整
			if ((nOffset + sizeof(DVOFrameHeaderEx)) >= nDataSize)
				return false;

			pBuffer += nOffset;

			// 帧数据不完整
			if (nOffset + FrameSize(*pBuffer) >= nDataSize)
				return false;

			if (Frame(&pBuffer[nOffset])->nType == FRAME_B ||
				Frame(&pBuffer[nOffset])->nType == FRAME_I ||
				Frame(&pBuffer[nOffset])->nType == FRAME_P)
			{
				m_pFrameOffsetTable[nCurFrames++] = (nOffset + nStartFileOffset);
			}
			nOffset += FrameSize(&pBuffer[nOffset]);
		}

		return true;
	}
	
	/// 文件摘要线程
	static UINT __stdcall ThreadFileAbstract(void *p)
	{
		CDvoPlayer* pThis = (CDvoPlayer *)p;
		if (pThis->m_nTotalFrames == 0)
		{
			assert(false);
			return 0;
		}
		pThis->m_pFrameOffsetTable = _New UINT[pThis->m_nTotalFrames];		
		DWORD nBufferSize = 1024 * 1024*16;
		byte *pBuffer = _New byte[nBufferSize];		
		while (!pBuffer)
		{
			if (nBufferSize == 1024 * 1024)
			{// 连1MB的内存都无法申请的话，则退出
				DxTraceMsg("%s Can't alloc enough memory.\n", __FUNCTION__);
				assert(false);
				return 0;
			}
			nBufferSize /= 2;
			pBuffer = _New byte[nBufferSize];
		}
		shared_ptr<byte>BufferPtr(pBuffer);
		byte *pFrame = nullptr;
		int nFrameSize = 0;
		HANDLE	hDvoFile = CreateFileA(pThis->m_pszFileName,
									   GENERIC_READ,
									   FILE_SHARE_READ,
									   NULL,
									   OPEN_ALWAYS,
									   FILE_ATTRIBUTE_ARCHIVE,
									   NULL);
		if (!hDvoFile)
		{
			return 0;
		}
		shared_ptr<void> DvoFilePtr(hDvoFile, CloseHandle);
		
		// 不再分析文件，因为StartPlay已经作过分析的确认
		DWORD nOffset = sizeof(DVO_MEDIAINFO);
		if (SetFilePointer(hDvoFile, nOffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			assert(false);
			return 0;
		}
		
		int nFrames = 0;
		LONG nSeekOffset = 0;
		DWORD nBytesRead = 0;
		DWORD nDataLength = 0;
		byte *pFrameBuffer = nullptr;
		FrameParser Parser;
		while (pThis->m_bThreadFileAbstractRun)
		{
			if (!ReadFile(pThis->m_hDvoFile, &pBuffer[nDataLength], (nBufferSize - nDataLength), &nBytesRead, nullptr))
			{
				DxTraceMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return 0;
			}
			pFrameBuffer = pBuffer;
			nDataLength += nBytesRead;
			while (pThis->m_bThreadFileAbstractRun)
			{
				if (!pThis->ParserFrame(&pFrameBuffer, nDataLength, &Parser))
				{
					break;
				}
				// 只记录视频帧的文件偏移
				if (Parser.pHeaderEx->nType == FRAME_B ||
					Parser.pHeaderEx->nType == FRAME_I ||
					Parser.pHeaderEx->nType == FRAME_P)
					pThis->m_pFrameOffsetTable[nFrames++] = nOffset;
				nOffset += Parser.nFrameSize;
			}
			nOffset += nBytesRead;
			// 残留数据长为nDataLength
			memmove(pBuffer, pFrameBuffer, nDataLength);
		}
		return 0;
	}
	
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

	/// @brief 把Dxva硬解码帧转换成NV12图像
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

	static UINT __stdcall ThreadPlayVideo(void *p)
	{
		CDvoPlayer* pThis = (CDvoPlayer *)p;	
		int nAvError = 0;
		char szAvError[1024] = { 0 };
		if (!pThis->m_hWnd)
		{
			DxTraceMsg("%s Please assign a Window.\n", __FUNCTION__);
			assert(false);
			return -1;
		}	
		AVCodecID nAvCodecID = AV_CODEC_ID_H264;
		switch (pThis->m_nVideoCodec)
		{
		case CODEC_H264:
			nAvCodecID = AV_CODEC_ID_H264;
			break;
		case CODEC_H265:
			nAvCodecID = AV_CODEC_ID_H265;
			break;
		case CODEC_MJPEG:
			nAvCodecID = AV_CODEC_ID_MJPEG;
			break;
		default:
		{
			DxTraceMsg("%s Can't find a decoder for CodecID:%d.\n", __FUNCTION__, pThis->m_nVideoCodec);
			assert(false);
			return 0; 
		}
		}
		shared_ptr<CVideoDecoder>pDecodec = make_shared<CVideoDecoder>();
		if (!pDecodec)
		{
			DxTraceMsg("%s Failed in allocing memory for Decoder.\n", __FUNCTION__);
			assert(false);
			return 0;
		}

		if (pDecodec->InitDecoder(pThis->m_nVideoWidth, pThis->m_nVideoHeight, nAvCodecID, pThis->m_bEnableHaccel) != S_OK)
		{
			DxTraceMsg("%s Failed in Initializing Decoder.\n", __FUNCTION__);
			assert(false);
			return 0;
		}

		double dfT1 = GetExactTime() - pThis->m_nFrameInterval;
		double dfTimeSpan = 0.0f;
		AVPacket *pAvPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
			
		int nGot_picture = 0;
		AVFrame *pAvFrame = av_frame_alloc();
		DWORD nResult = 0;
		int nTimeSpan = 0;
		DWORD dwSleepPricision = GetSleepPricision();
		int nFrameInterval = 40;
		StreamFramePtr FramePtr;
		byte *pYUV = nullptr;
		int nYUVSize = 0;
		pThis->m_dfTimesStart = GetExactTime();
				
		while (pThis->m_bThreadPlayRun)
		{
			nTimeSpan = (int)((GetExactTime() - dfT1)*1000);
			if (nTimeSpan >= pThis->m_nFrameInterval)
			{
				CAutoLock lock(&pThis->m_csVideoCache);
				if (pThis->m_listVideoCache.size() > 0)
				{
					FramePtr = pThis->m_listVideoCache.front();
					pThis->m_listVideoCache.pop_front();
				}
			}
			else 
			{
				nTimeSpan = (int)(1000 * (GetExactTime() - dfT1));
				int nSleepTime = pThis->m_nFrameInterval - nTimeSpan;
				if (nSleepTime > dwSleepPricision)
					ThreadSleep(nSleepTime);
				continue;
			}
			av_init_packet(pAvPacket);		
			pAvPacket->data = (uint8_t *)FramePtr->Framedata();
			pAvPacket->size = FramePtr->FrameHeader()->nLength;
			pAvPacket->flags = AV_PKT_FLAG_KEY;
			

			nAvError = pDecodec->Decode(pAvFrame, nGot_picture, pAvPacket);
			if (nAvError < 0)
			{
				av_strerror(nAvError, szAvError, 1024);
				DxTraceMsg("%s Decode error:%s.\n", __FUNCTION__, szAvError);
				continue;
			}
			if (nGot_picture)
			{
				// 初始显示组件
				// 使用线程内CDxSurface对象显示图象
				if (!pThis->m_pDxSurface->IsInited())		// D3D设备尚未创建,说明未初始化
				{
					DxSurfaceInitInfo InitInfo;
					InitInfo.nFrameWidth = pAvFrame->width;
					InitInfo.nFrameHeight = pAvFrame->height;
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
					//::SendMessageTimeout(pThis->m_hWnd, WM_INITDXSURFACE, (WPARAM)TPPtr->pDxSurface, (LPARAM)&InitInfo, SMTO_BLOCK, 500, (PDWORD_PTR)&nResult);
				}				
				pThis->ProcessYVUCapture(pAvFrame, FramePtr->FrameHeader()->nTimestamp);
				//::SendMessageTimeout(pThis->m_hWnd, WM_RENDERFRAME, (WPARAM)TPPtr->pDxSurface, (LPARAM)pAvFrame, SMTO_BLOCK, 500, (PDWORD_PTR)&nResult);
				pThis->m_pDxSurface->Render(pAvFrame);
				pThis->m_nCurVideoFrame = FramePtr->FrameHeader()->nFrameID;
			}
			else
			{
				DxTraceMsg("%s Decodec failed,FrameID:%d.\n",__FUNCTION__,FramePtr->FrameHeader()->nFrameID);
			}
		}

		av_frame_free(&pAvFrame);
		av_free(pAvPacket);
		if (pYUV)
			av_free(pYUV);

		return 0;
	}
	static UINT __stdcall ThreadPlayAudio(void *p)
	{
		CDvoPlayer *pThis = (CDvoPlayer *)p;
		int nAudioFrameInterval = pThis->m_nFrameInterval / 2;
		double dfT1 = GetExactTime() - nAudioFrameInterval;
		double dfTimeSpan = 0.0f;		
		DWORD nResult = 0;
		int nTimeSpan = 0;
		DWORD dwSleepPricision = GetSleepPricision();
		int nFrameInterval = 40;
		StreamFramePtr FramePtr;
		int nAudioError = 0;
		byte *pPCM = nullptr;
		int nPCMSize = 0;
		while (pThis->m_bThreadPlayRun)
		{
			nTimeSpan = (int)((GetExactTime() - dfT1) * 1000);
			if (pThis->m_nPlayRate > 4 || pThis->m_nPlayRate < -4)
			{// 播放倍率超过4或小于1/4都不再播放音频,因为此时音频严重失真，影响听觉
				if (m_pDsPlayer->IsPlaying())
					m_pDsPlayer->StopPlay();
				CAutoLock lock(&pThis->m_csAudioCache);
				if (pThis->m_listAudioCache.size() > 0)	
					pThis->m_listAudioCache.pop_front();
			}
			if (nTimeSpan >= nAudioFrameInterval)
			{
				if (!m_pDsPlayer->IsPlaying())
					m_pDsPlayer->StartPlay();
				CAutoLock lock(&pThis->m_csAudioCache);
				if (pThis->m_listAudioCache.size() > 0)
				{
					FramePtr = pThis->m_listAudioCache.front();
					pThis->m_listAudioCache.pop_front();
				}
			}
			else
			{	// 控制播放速度
				nTimeSpan = (int)(1000 * (GetExactTime() - dfT1));
				int nSleepTime = nAudioFrameInterval - nTimeSpan;
				if (nSleepTime > dwSleepPricision)
					ThreadSleep(nSleepTime);
				continue;
			}
			if (m_pDsPlayer->IsPlaying() && 
				pThis->m_pAudioDecoder->Decode(pPCM, nPCMSize, (byte *)FramePtr->Framedata(), FramePtr->FrameHeader()->nLength) == DVO_Succeed)
				m_pDsPlayer->PutWaveData(pPCM, nPCMSize);
		}
		return 0;
	}
	// 相机实时码流捕捉回调函数
	void static __stdcall StreamCallBack(IN USER_HANDLE  lUserID,
										IN REAL_HANDLE lStreamHandle,
										IN int         nErrorType,
										IN const char* pBuffer,
										IN int         nDataLen,
										IN void*       pUser)
	{
		CDvoPlayer *pThis = (CDvoPlayer *)pUser;
		app_net_tcp_enc_stream_head_t *pStreamHeader = (app_net_tcp_enc_stream_head_t *)pBuffer;
		if (!pThis->m_pStreamInfo->bRecvIFrame) // 从未收到过I帧
		{
			if (pStreamHeader->frame_type != 0 &&	// 海思I帧号为0，这是固件的一个BUG，有待修正
				pStreamHeader->frame_type != APP_NET_TCP_COM_DST_I_FRAME &&
				pStreamHeader->frame_type != APP_NET_TCP_COM_DST_IDR_FRAME)	//当前帧不是I帧
			{
				switch (pStreamHeader->frame_type)
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
				return;
			}
			else
			{
				printf("Recv the First I Frame Now begin decoding.\n");
				pThis->m_pStreamInfo->bRecvIFrame = true;
				pThis->m_pStreamInfo->tFirstTime = (time_t)(GetExactTime() * 1000);//time(0);
			}
		}
		// 解码必须从I帧开始
		int nSizeofHeader = sizeof(app_net_tcp_enc_stream_head_t);
		byte *pFrameData = (byte *)(pStreamHeader)+nSizeofHeader;

		// 得到协议帧头
		MSG_HEAD *pPkHeader = (MSG_HEAD *)((byte *)pStreamHeader - sizeof(MSG_HEAD));
		
		int nFrameType = pStreamHeader->frame_type;
		UINT nPkLen = ntohl(pPkHeader->Pktlen);
		int nFrameLength = nDataLen - sizeof(app_net_tcp_enc_stream_head_t);

		pThis->m_pStreamInfo->nFrameID = pStreamHeader->frame_num;
		pThis->m_pStreamInfo->tLastTime = (time_t)(GetExactTime() * 1000); //time(0)	/*pStreamHeader->sec*1000 + pStreamHeader->usec*/;
		double dfTLast = GetExactTime();

		if (pStreamHeader->frame_type == 0 ||									// 海思I帧号为0，这是固件的一个BUG，有待修正
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_IDR_FRAME ||		// IDR帧
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_I_FRAME ||			// I帧
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_P_FRAME ||			// P帧
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_B_FRAME)			// B帧
		{
			pThis->m_pStreamInfo->PushFrameInfo(nFrameLength);
			pThis->m_pStreamInfo->nVideoFrameCount++;
			pThis->m_pStreamInfo->nVideoBytes += nFrameLength;
			StreamFramePtr pStream = make_shared<StreamFrame>(pFrameData,nFrameType,nFrameLength,pStreamHeader->frame_num,pThis->m_pStreamInfo->tLastTime);	
			CAutoLock lock(&pThis->m_csVideoCache);
			pThis->m_listVideoCache.push_back(pStream);
		}
		else if (pStreamHeader->frame_type == APP_NET_TCP_COM_DST_711_ALAW ||    // 711 A律编码帧
				pStreamHeader->frame_type == APP_NET_TCP_COM_DST_711_ULAW ||     // 711 U律编码帧
				pStreamHeader->frame_type == APP_NET_TCP_COM_DST_726 ||          // 726编码帧
				pStreamHeader->frame_type == APP_NET_TCP_COM_DST_AAC)			 // AAC编码帧
		{
			pThis->m_pStreamInfo->nAudioBytes += nFrameLength;
			pThis->m_pStreamInfo->nAudioFrameCount++;
		}
	}
};