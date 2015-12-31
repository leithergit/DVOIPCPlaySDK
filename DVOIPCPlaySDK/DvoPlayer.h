#pragma once
/////////////////////////////////////////////////////////////////////////
/// @file  DVOIPCPlaySDK.hpp
/// Copyright (c) 2015, �Ϻ���άŷ�Ƽ�
/// All rights reserved.  
/// @brief DVOIPC�������SDKʵ��
/// @version 1.0  
/// @author  xionggao.lee
/// @date  2015.12.17
///   
/// �޶�˵��������汾 
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
/// StreamFrame��������ý�岥�ŵ�����֡
struct StreamFrame
{
	StreamFrame()
	{
		ZeroMemory(this, sizeof(StreamFrame));
	}
	/// @brief	�����ѷ�װ�õĴ�DVOFrameHeaderExͷ������
	StreamFrame(byte *pBuffer, int nLenth)
	{
		assert(pBuffer != nullptr);
		assert(nLenth >= sizeof(DVOFrameHeaderEx));
		nSize = nLenth;
		pInputData = _New byte[nLenth];
		if (pInputData)
			memcpy(pInputData, pBuffer, nLenth);
	}
	/// @brief	�����������������ʵʱ����������
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
		case 0:									// ��˼I֡��Ϊ0�����ǹ̼���һ��BUG���д�����
		case APP_NET_TCP_COM_DST_IDR_FRAME: 	// IDR֡��
		case APP_NET_TCP_COM_DST_I_FRAME:		// I֡��
			pHeader->nType = FRAME_I;
			break;
		case APP_NET_TCP_COM_DST_P_FRAME:       // P֡��
		case APP_NET_TCP_COM_DST_B_FRAME:       // B֡��
			pHeader->nType = FRAME_B;
			break;
		case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A�ɱ���֡
		case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U�ɱ���֡
		case APP_NET_TCP_COM_DST_726:           // 726����֡
		case APP_NET_TCP_COM_DST_AAC:           // AAC����֡��
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

#define _Frame_PERIOD			30.0f		///< һ��֡������
#pragma pack(push)
#pragma pack(1)
struct FrameInfo
{
	time_t  tRecvTime;				///< �յ�֡��ʱ�� ��λ����
	int		nFrameSize;
};

/// @brief ����DVO˽��֡�ṹ��
struct FrameParser
{
	DVOFrameHeaderEx	*pHeaderEx;		///< DVO˽��¼���֡����
	UINT				nFrameSize;		///< pFrame�����ݳ���
	byte				*pLawFrame;		///< ԭʼ��������
	UINT				nLawFrameSize;	///< ԭʼ�������ݳ���
};

struct StreamInfo
{
	UINT	nFrameID;
	bool	bRecvIFrame;		///< �Ƿ��յ��ڸ�I֡
	int		nVideoFrameCount;
	int		nAudioFrameCount;
	__int64 nVideoBytes;		///< �յ���Ƶ���ֽ���
	__int64 nAudioBytes;		///< �յ���Ƶ���ֽ���
	time_t  tFirstTime;			///< �������ݵ���ʼʱ��	��λ����
	time_t	tLastTime;			///< ���һ�ν�������ʱ�� ��λ����
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

	int	GetVideoCodeRate(int nUnit = 1024)	///< ȡ����Ƶ����(Kbps)
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

/// @brief �߳���Ϣ��
enum SleepWay
{
	Sys_Sleep = 0,		///< ֱ�ӵ���ϵͳ����Sleep
	Wmm_Sleep = 1,		///< ʹ�ö�ý��ʱ����߾���
	Std_Sleep = 2		///< ʹC++11�ṩ���߳����ߺ���
};
typedef shared_ptr<StreamInfo> StreamInfoPtr;
#pragma pack(pop)


class CDvoPlayer
{
public:
	int		nSize;
private:
	StreamInfoPtr m_pStreamInfo;		///< ��Ƶ����Ϣ	
	shared_ptr<DVO_MEDIAINFO>m_pMediaHeader;/// ý���ļ�ͷ
	list		<StreamFramePtr>m_listVideoCache;///< ������֡����
	list		<StreamFramePtr>m_listAudioCache;///< ������֡����
	CRITICAL_SECTION	m_csVideoCache;
	CRITICAL_SECTION	m_csAudioCache;
	int			m_nMaxFrameCache;		///< �����Ƶ��������,Ĭ��ֵ100
										///< ��m_FrameCache�е���Ƶ֡��������m_nMaxFrameCacheʱ�����޷��ټ�������������
private:
	// ��Ƶ������ر���
	int			m_nVideoFrames;			///< ��ǰ��������Ƶ֡����	
	int			m_nTotalFrames;			///< ��ǰ�ļ�����Ч��Ƶ֡������,���������ļ�ʱ��Ч
	HWND		m_hWnd;					///< ������Ƶ�Ĵ��ھ��
	DVO_CODEC	m_nVideoCodec;			///< ��Ƶ�����ʽ @see DVO_CODEC	
	static		CAvRegister avRegister;
	static shared_ptr<CDSoundPlayer> m_pDsPlayer;///< DirectSound���Ŷ���ָ��
	CDxSurface* m_pDxSurface;			///< Direct3d Surface��װ��,������ʾ��Ƶ
	int			m_nVideoWidth;			///< ��Ƶ���
	int			m_nVideoHeight;			///< ��Ƶ�߶�
	int			m_nCurFrameID;			///< ��ǰ��Ƶ֡ID	
	int			m_nFrameEplased;		///< �Ѿ�����֡��
	int			m_nCurVideoFrame;		///< ��ǰ�����ŵ���Ƶ֡ID		
	int			m_nPlayRate;			///< ��ǰ���ŵı���ȡֵ-32~32,-32ʱΪ32֮һ����32��Ϊ32����
	double		m_dfLastTimeVideoPlay;	///< ǰһ����Ƶ���ŵ�ʱ��
	double		m_dfTimesStart;			///< ��ʼ���ŵ�ʱ��
	bool		m_bEnableHaccel;		///< �Ƿ�����Ӳ����

private:	// ��Ƶ������ر���
	
	CAudioDecoder *m_pAudioDecoder;		///< ��Ƶ������
	DVO_CODEC	m_nAudioCodec;			///< ��Ƶ�����ʽ @see DVO_CODEC
	bool		m_bEnableAudio;			///< �Ƿ�������Ƶ����
	DWORD		m_dwAudioOffset;		///< ��Ƶ�������е�ƫ�Ƶ�ַ
	DWORD		m_dwAudioBuffLength;	///< 
	int			m_nNotifyNum;			///< ��Ƶ����������
	double		m_dfLastTimeAudioPlay;	///< ǰһ�β�����Ƶ��ʱ��
	double		m_dfLastTimeAudioSample;///< ǰһ����Ƶ������ʱ��
	int			m_nAudioFrames;			///< ��ǰ��������Ƶ֡����
	int			m_nCurAudioFrame;		///< ��ǰ�����ŵ���Ƶ֡ID
private:
	HANDLE		m_hThreadParser;		///< ����DVO˽�и�ʽ¼����߳�
	HANDLE		m_hThreadPlayVideo;		///< ��Ƶ����Ͳ����߳�
	HANDLE		m_hThreadPlayAudio;		///< ��Ƶ����Ͳ����߳�
	HANDLE		m_hThreadFileAbstract;	///< �ļ���ϢժҪ�߳�
	volatile bool m_bThreadParserRun;
	volatile bool m_bThreadPlayRun;
	volatile bool m_bThreadFileAbstractRun;
	bool		m_bPause;				///< �Ƿ�����ͣ״̬
	byte		*m_pYUV;				///< YVU��׽ר���ڴ�
	int			m_nYUVSize ;			///<
	shared_ptr<byte>m_pYUVPtr;
private:	// �ļ�������ر���
	HANDLE		m_hDvoFile;				///< ���ڲ��ŵ��ļ����
	char		*m_pszFileName;			///< ���ڲ��ŵ��ļ���,���ֶν����ļ�����ʱ����Ч
	UINT		*m_pFrameOffsetTable;	///< ��Ƶ֡ID��Ӧ�ļ�ƫ�Ʊ�
	volatile LONG m_nSeekOffset;		///< ���ļ���ƫ��
	CRITICAL_SECTION	m_csSeekOffset;
	int			m_nSDKVersion;			///< ����¼���ļ���SDK�汾	
	int			m_nMaxFrameSize;		///< ���I֡�Ĵ�С�����ֽ�Ϊ��λ,Ĭ��ֵ256K
	int			m_nFPS;	
	int			m_nFrameInterval;		///< ֡���ż��,��λ����
private:
	CaptureFrame	m_pfnCaptureFrame;
	void*			m_pUserCaptureFrame;
	CaptureYUV		m_pfnCaptureYUV;
	void*			m_pUserCaptureYUV;
	CaptureYUVEx	m_pfnCaptureYUVEx;
	void*			m_pUserCaptureYUVEx;
	SleepWay		m_nSleepWay;		///< �߳����ߵķ�ʽ,@see SleepWay
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

	/// @brief ȡ����Ƶ�ļ���һ֡��ID,�൱����Ƶ�ļ��а�������Ƶ��֡��
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
		// �����������m_nMaxFrameSize�����ڵ�����֡
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
		{// �ز�Ҫ�д��ڲŻᲥ����Ƶ������
			m_hWnd = hWnd;
			m_pDxSurface = _New CDxSurface();
			//m_DsPlayer = _New CDSoundPlayer();
			//m_DsPlayer.Initialize(m_hWnd, Audio_Play_Segments);
		}
		m_nMaxFrameSize = 1024 * 256;
		m_nFPS		  = 25;				// FPS��Ĭ��ֵΪ25
		m_nVideoCodec = CODEC_H264;		// ��ƵĬ��ʹ��H.264����
		m_nAudioCodec = CODEC_G711U;	// ��ƵĬ��ʹ��G711U����
		m_nMaxFrameCache = 100;			// Ĭ�������Ƶ��������Ϊ100
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
	/// @brief  �Ƿ�Ϊ�ļ�����
	/// @retval			true	�ļ�����
	/// @retval			false	������
	bool IsFilePlayer()
	{
		if (m_hDvoFile || m_pszFileName)
			return true;
		else
			return false;
	}

	// ����������ͷ
	// ��������ʱ������֮ǰ��������������ͷ
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
	// ��ʼ����
	int StartPlay(bool bEnaleAudio = false,bool bEnableHaccel = false)
	{
		m_bPause = false;
		if (m_pszFileName)
		{
			// ���ļ�
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
			// ������Ƶ�ļ�ͷ
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
			// GetLastFrameIDȡ�õ������һ֡��ID����֡����Ҫ�ڴ˻�����+1
			m_nTotalFrames++;
			
			// �����ļ������߳�
			m_bThreadParserRun = true;
			m_hThreadParser = (HANDLE)_beginthreadex(nullptr, 0, ThreadParser, this, 0, 0);
			// �����ļ���ϢժҪ�߳�
			m_bThreadFileAbstractRun = true;
			m_hThreadFileAbstract = (HANDLE)_beginthreadex(nullptr, 0, ThreadFileAbstract, this, 0, 0);
		}
		else
		{
			if (!m_pMediaHeader)
				return DVO_Error_NotInputStreamHeader;		// δ������ͷ
		}
		if (m_hWnd)
		{
			// �����������߳�
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
		

	/// @brief			����DVO˽��ʽ����������
	/// @retval			0	�����ɹ�
	/// @retval			1	������������
	/// @retval			-1	���������Ч
	/// @remark			����������ʱ����Ӧ��֡������ʵ��δ�������ţ����Ǳ����˲��Ŷ����У�Ӧ�ø���dvoplay_PlayStream
	///					�ķ���ֵ���жϣ��Ƿ�������ţ���˵��������������Ӧ����ͣ����
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

	/// @brief			����DVO IPC��������
	/// @retval			0	�����ɹ�
	/// @retval			1	������������
	/// @retval			-1	���������Ч
	/// @remark			����������ʱ����Ӧ��֡������ʵ��δ�������ţ����Ǳ����˲��Ŷ����У�Ӧ�ø���dvoplay_PlayStream
	///					�ķ���ֵ���жϣ��Ƿ�������ţ���˵��������������Ӧ����ͣ����
	int InputStream(IN byte *pFrameData, IN int nFrameType, IN int nFrameLength, int nFrameNum, time_t nFrameTime = 0)
	{
		StreamFramePtr pStream = make_shared<StreamFrame>(pFrameData, nFrameType, nFrameLength, nFrameNum, nFrameTime);
		if (!pStream)
			return DVO_Error_InsufficentMemory;
		switch (nFrameType)
		{
			case 0:									// ��˼I֡��Ϊ0�����ǹ̼���һ��BUG���д�����
			case APP_NET_TCP_COM_DST_IDR_FRAME: 	// IDR֡��
			case APP_NET_TCP_COM_DST_I_FRAME:		// I֡��		
			case APP_NET_TCP_COM_DST_P_FRAME:       // P֡��
			case APP_NET_TCP_COM_DST_B_FRAME:       // B֡��
			{
				CAutoLock lock(&m_csVideoCache);
				if (m_nVideoFrames >= m_nMaxFrameCache)
					return DVO_Error_FrameCacheIsFulled;
				m_nVideoFrames++;				
				m_listVideoCache.push_back(pStream);
			}
				break;
			case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A�ɱ���֡
			case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U�ɱ���֡
			case APP_NET_TCP_COM_DST_726:           // 726����֡
			case APP_NET_TCP_COM_DST_AAC:           // AAC����֡��
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

	/// @brief			����Ӳ���빦��
	/// @param [in]		bEnableHaccel	�Ƿ���Ӳ���빦��
	/// #- true			����Ӳ���빦��
	/// #- false		�ر�Ӳ���빦��
	/// @remark			�����͹ر�Ӳ���빦�ܱ���ҪStart֮ǰ���ò�����Ч
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

	/// @brief			��ȡӲ����״̬
	/// @retval			true	�ѿ���Ӳ���빦��
	/// @retval			flase	δ����Ӳ���빦��
	inline bool  GetHaccelStatus()
	{
		return m_bEnableHaccel;
	}

	/// @brief			����Ƿ�֧���ض���Ƶ�����Ӳ����
	/// @param [in]		nCodec		��Ƶ�����ʽ,@see DVO_CODEC
	/// @retval			true		֧��ָ����Ƶ�����Ӳ����
	/// @retval			false		��֧��ָ����Ƶ�����Ӳ����
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

	/// @brief			����������ʱ����ǰ���Ŷ����е���Ƶ֡������
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	/// @remark			����������ʱ����Ӧ��֡������ʵ��δ�������ţ����Ǳ����˲��Ŷ����У�Ӧ�ø���PlayStream
	///					�ķ���ֵ���жϣ��Ƿ�������ţ���˵��������������Ӧ����ͣ����
	inline int GetCacheSize(int &nCacheCount)
	{
		return m_nVideoFrames;
	}

	/// @brief			ȡ���ļ����ŵ���Ƶ֡��
	inline int  GetFps()
	{
		return m_nFPS;
	}

	/// @brief			ȡ���ļ��а�������Ч��Ƶ֡������
	/// @remark			ֻ�����ļ�����ʱ����Ч
	inline int  GetFrames()
	{
		if (m_hDvoFile)
			return m_nTotalFrames;
		else
			return DVO_Error_NotFilePlayer;
	}

	/// @brief			ȡ�õ�ǰ������Ƶ��֡ID
	int  GetCurrentFrameID()
	{
		if (m_hThreadPlayVideo)
			return m_nCurVideoFrame;
		else
			return DVO_Error_PlayerNotStart;
	}

	/// @brief			��ȡ���Ų��ŵ���Ƶͼ��
	/// @param [in]		szFileName		Ҫ������ļ���
	/// @param [in]		nFileFormat		�����ļ��ı����ʽ,@see SNAPSHOT_FORMAT����
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
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

	/// @brief			���ò��ŵ�����
	/// @param [in]		nVolume			Ҫ���õ�����ֵ��ȡֵ��Χ0~100��Ϊ0ʱ����Ϊ����
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	void  SetVolume(IN int nVolume)
	{
		if (m_bEnableAudio/* && m_DsPlayer*/)
		{
			int nDsVolume = nVolume * 100 - 10000;
			m_pDsPlayer->SetVolume(nDsVolume);
		}
	}

	/// @brief			ȡ�õ�ǰ���ŵ�����
	int  GetVolume()
	{
		if (m_bEnableAudio/* && m_DsPlayer*/)
			return (m_pDsPlayer->GetVolume() + 10000) / 100;
		else
			return 0;
	}

	/// @brief			ȡ�õ�ǰ���ŵ��ٶȵı���
	inline int  GetRate()
	{
		return m_nPlayRate;
	}

	/// @brief			���õ�ǰ���ŵ��ٶȵı���
	/// @param [in]		nPlayRate		��ǰ�Ĳ��ŵı���,@see PlayRate
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
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

	/// @brief			��Ծ��ָ��Ƶ֡���в���
	/// @param [in]		nFrameID		Ҫ����֡����ʼID
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	/// @remark			����ָ��֡Ϊ�ǹؼ�֡��֡�Զ��ƶ����ͽ��Ĺؼ�֡���в���
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
		
		// ���ļ�ժҪ�У�ȡ���ļ�ƫ����Ϣ
		SetSeekOffset(m_pFrameOffsetTable[nFrameID]);
		return DVO_Succeed;
	}

	/// @brief			��Ծ��ָ��ʱ��ƫ�ƽ��в���
	/// @param [in]		nTimeSet		Ҫ���ŵ���ʼʱ��,��λ��,��FPS=25,��0.04��Ϊ��2֡��1.00�룬Ϊ��25֡
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	/// @remark			����ָ��ʱ����Ӧ֡Ϊ�ǹؼ�֡��֡�Զ��ƶ����ͽ��Ĺؼ�֡���в���
	int  SeekTime(IN double nTimeSet)
	{
		if (!m_hDvoFile || !m_pFrameOffsetTable)
			return DVO_Error_NotFilePlayer;
		
		int nFrameID = (int)(nTimeSet * m_nFPS);
		return SeekFrame(nFrameID);
	}

	/// @brief			������һ֡
	/// @param [in]		nTimeSet		Ҫ���ŵ���ʼʱ��
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	/// @remark			�ú����������ڵ�֡����
	int  SeekNextFrame()
	{

	}

	/// @brief			��/����Ƶ����
	/// @param [in]		bEnable			�Ƿ񲥷���Ƶ
	/// -#	true		������Ƶ����
	/// -#	false		������Ƶ����
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
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

	/// @brief			ˢ�²��Ŵ���
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	/// @remark			�ù���һ�����ڲ��Ž�����ˢ�´��ڣ��ѻ�����Ϊ��ɫ
	inline void  Refresh()
	{
		if (m_hWnd)
			::InvalidateRect(m_hWnd, nullptr, true);

	}

	/// @brief			��ȡ�ѷ�ʱ��,��λ����
	/// @param [in]		hPlayHandle		��OpenFile��OpenStream���صĲ��ž��
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	inline LONGLONG GetEplaseTime()
	{
		return (LONGLONG)(1000 * (GetExactTime() - m_dfTimesStart));
	}

	/// @brief			���û�ȡYUV���ݻص��ӿ�,ͨ���˻ص����û���ֱ�ӻ�ȡ������YUV����
	void SetYUVCapture(IN CaptureYUV pYUVProc, IN void *pUserPtr)
	{
		if (!pYUVProc)
			return ;
		m_pfnCaptureYUV = pYUVProc;
		m_pUserCaptureYUV = pUserPtr;
		
	}

	/// @brief			���û�ȡYUV���ݻص��ӿ�,ͨ���˻ص����û���ֱ�ӻ�ȡ������YUV����
	void SetYUVCaptureEx(IN CaptureYUVEx pYUVExProc, IN void *pUserPtr)
	{
		if (!pYUVExProc)
			return;
		m_pfnCaptureYUVEx = pYUVExProc;
		m_pUserCaptureYUVEx = pUserPtr;
	}

	/// @brief			����DVO˽�ø�ʽ¼��֡�����ص�,ͨ���˻أ��û���ֱ�ӻ�ȡԭʼ��֡����
	void SetFrameParserCapture(IN CaptureFrame pFrameParserProc,IN void *pUserPtr)
	{
		if (!pFrameParserProc)
			return;
		m_pfnCaptureFrame = pFrameParserProc;
		m_pUserCaptureFrame = pUserPtr;

	}
	/// @brief			�����ⲿ���ƻص��ӿ�
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
	
	/// @brief			����֡����
	/// @param [in,out]	pBuffer			����DVO˽��¼���ļ��е�������
	/// @param [in,out]	nDataSize		pBuffer����Ч���ݵĳ���
	/// @param [out]	pFrameParser	DVO˽��¼���֡����	
	/// @retval			true	�����ɹ�
	/// @retval			false	ʧ�ܣ�pBuffer�Ѿ�û����Ч��֡����
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
		
		// ֡ͷ��Ϣ������
		if ((nOffset + sizeof(DVOFrameHeaderEx)) >= nDataSize)
			return false;

		*pBuffer += nOffset;
		
		// ֡���ݲ�����
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
	{// ��ָ������Ч�Ĵ��ھ������ѽ�������ļ����ݷ��벥�Ŷ��У����򲻷��벥�Ŷ���
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
				if (!pThis->m_hWnd)		// û�д��ھ�����򲻽��в���
				{
					continue;
				}
				if (pThis->InputStream((byte *)Parser.pHeaderEx, Parser.nFrameSize) == DVO_Error_InvalidFrameType)
				{
					Sleep(10);
				}
			}
			// �������ݳ�ΪnDataLength
			memmove(pBuffer, pFrameBuffer, nDataLength);
#ifdef _DEBUG
			ZeroMemory(&pBuffer[nDataLength],nBufferSize - nDataLength);
#endif
			// ���ǵ������������̣߳�����Ҫ�ݻ���ȡ����
			if (!pThis->m_hWnd || !bPause)
			{
				Sleep(10);
			}
		}
		return 0;
	}

	/// @brief			����֡ID
	/// @param [in,out]	pBuffer			����DVO˽��¼���ļ��е�������
	/// @param [in,out]	nDataSize		pBuffer����Ч���ݵĳ���
	/// @param [in]		nStartOffset	pBuffer�����ݵ����ļ��е�ƫ��
	/// @retval			true	�����ɹ�
	/// @retval			false	ʧ�ܣ�pBuffer�Ѿ�û����Ч��֡����
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
		
		// �����������pBuffer�ڵ�����֡
		while (true)
		{
			if (nOffset >= nDataSize)
				return false;

			// ֡ͷ��Ϣ������
			if ((nOffset + sizeof(DVOFrameHeaderEx)) >= nDataSize)
				return false;

			pBuffer += nOffset;

			// ֡���ݲ�����
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
	
	/// �ļ�ժҪ�߳�
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
			{// ��1MB���ڴ涼�޷�����Ļ������˳�
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
		
		// ���ٷ����ļ�����ΪStartPlay�Ѿ�����������ȷ��
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
				// ֻ��¼��Ƶ֡���ļ�ƫ��
				if (Parser.pHeaderEx->nType == FRAME_B ||
					Parser.pHeaderEx->nType == FRAME_I ||
					Parser.pHeaderEx->nType == FRAME_P)
					pThis->m_pFrameOffsetTable[nFrames++] = nOffset;
				nOffset += Parser.nFrameSize;
			}
			nOffset += nBytesRead;
			// �������ݳ�ΪnDataLength
			memmove(pBuffer, pFrameBuffer, nDataLength);
		}
		return 0;
	}
	
	/// @brief ��NV12ͼ��ת��ΪYV420Pͼ��
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

		// ����Y����
		for (int i = 0; i < height; i++)
			memcpy(pYV12 + i*width, pSrcY + i*nYpitch, width);

		// ����VU����
		for (int i = 0; i < heithtUV; i++)
		{
			for (int j = 0; j < width; j++)
			{
				dstU[i*widthUV + j] = pSrcUV[i*nUVpitch + 2 * j];
				dstV[i*widthUV + j] = pSrcUV[i*nUVpitch + 2 * j + 1];
			}
		}
	}

	/// @brief ��DxvaӲ����֡ת����NV12ͼ��
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

		// Y����ͼ��
		byte *pSrcY = (byte *)lRect.pBits;
		// UV����ͼ��
		//byte *pSrcUV = (byte *)lRect.pBits + lRect.Pitch * SurfaceDesc.Height;
		byte *pSrcUV = (byte *)lRect.pBits + lRect.Pitch * pAvFrameDXVA->height;

		byte* dstY = pYuv420;
		byte* dstV = pYuv420 + pAvFrameDXVA->width*pAvFrameDXVA->height;
		byte* dstU = pYuv420 + pAvFrameDXVA->width*pAvFrameDXVA->height/ 4;


		UINT heithtUV = pAvFrameDXVA->height / 2;
		UINT widthUV = pAvFrameDXVA->width / 2;

		// ����Y����
		for (int i = 0; i < pAvFrameDXVA->height; i++)
			memcpy(&dstY[i*pAvFrameDXVA->width], &pSrcY[i*lRect.Pitch], pAvFrameDXVA->width);

		// ����VU����
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
			// �ϲ�YUV����
			int nPicSize = pAvFrame->linesize[0] * pAvFrame->height;
						
			if (pAvFrame->format == AV_PIX_FMT_DXVA2_VLD)
			{// �ݲ�֧��dxva Ӳ����֡
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
			{// dxva Ӳ����֡
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
				// ��ʼ��ʾ���
				// ʹ���߳���CDxSurface������ʾͼ��
				if (!pThis->m_pDxSurface->IsInited())		// D3D�豸��δ����,˵��δ��ʼ��
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
			{// ���ű��ʳ���4��С��1/4�����ٲ�����Ƶ,��Ϊ��ʱ��Ƶ����ʧ�棬Ӱ������
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
			{	// ���Ʋ����ٶ�
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
	// ���ʵʱ������׽�ص�����
	void static __stdcall StreamCallBack(IN USER_HANDLE  lUserID,
										IN REAL_HANDLE lStreamHandle,
										IN int         nErrorType,
										IN const char* pBuffer,
										IN int         nDataLen,
										IN void*       pUser)
	{
		CDvoPlayer *pThis = (CDvoPlayer *)pUser;
		app_net_tcp_enc_stream_head_t *pStreamHeader = (app_net_tcp_enc_stream_head_t *)pBuffer;
		if (!pThis->m_pStreamInfo->bRecvIFrame) // ��δ�յ���I֡
		{
			if (pStreamHeader->frame_type != 0 &&	// ��˼I֡��Ϊ0�����ǹ̼���һ��BUG���д�����
				pStreamHeader->frame_type != APP_NET_TCP_COM_DST_I_FRAME &&
				pStreamHeader->frame_type != APP_NET_TCP_COM_DST_IDR_FRAME)	//��ǰ֡����I֡
			{
				switch (pStreamHeader->frame_type)
				{
				case APP_NET_TCP_COM_DST_IDR_FRAME:		// IDR֡
					printf("Recv a IDR Frame, discarded.\n");
					break;
				case APP_NET_TCP_COM_DST_P_FRAME:       // P֡
					printf("Recv a P Frame, discarded.\n");
					break;
				case APP_NET_TCP_COM_DST_B_FRAME:       // B֡
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
		// ��������I֡��ʼ
		int nSizeofHeader = sizeof(app_net_tcp_enc_stream_head_t);
		byte *pFrameData = (byte *)(pStreamHeader)+nSizeofHeader;

		// �õ�Э��֡ͷ
		MSG_HEAD *pPkHeader = (MSG_HEAD *)((byte *)pStreamHeader - sizeof(MSG_HEAD));
		
		int nFrameType = pStreamHeader->frame_type;
		UINT nPkLen = ntohl(pPkHeader->Pktlen);
		int nFrameLength = nDataLen - sizeof(app_net_tcp_enc_stream_head_t);

		pThis->m_pStreamInfo->nFrameID = pStreamHeader->frame_num;
		pThis->m_pStreamInfo->tLastTime = (time_t)(GetExactTime() * 1000); //time(0)	/*pStreamHeader->sec*1000 + pStreamHeader->usec*/;
		double dfTLast = GetExactTime();

		if (pStreamHeader->frame_type == 0 ||									// ��˼I֡��Ϊ0�����ǹ̼���һ��BUG���д�����
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_IDR_FRAME ||		// IDR֡
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_I_FRAME ||			// I֡
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_P_FRAME ||			// P֡
			pStreamHeader->frame_type == APP_NET_TCP_COM_DST_B_FRAME)			// B֡
		{
			pThis->m_pStreamInfo->PushFrameInfo(nFrameLength);
			pThis->m_pStreamInfo->nVideoFrameCount++;
			pThis->m_pStreamInfo->nVideoBytes += nFrameLength;
			StreamFramePtr pStream = make_shared<StreamFrame>(pFrameData,nFrameType,nFrameLength,pStreamHeader->frame_num,pThis->m_pStreamInfo->tLastTime);	
			CAutoLock lock(&pThis->m_csVideoCache);
			pThis->m_listVideoCache.push_back(pStream);
		}
		else if (pStreamHeader->frame_type == APP_NET_TCP_COM_DST_711_ALAW ||    // 711 A�ɱ���֡
				pStreamHeader->frame_type == APP_NET_TCP_COM_DST_711_ULAW ||     // 711 U�ɱ���֡
				pStreamHeader->frame_type == APP_NET_TCP_COM_DST_726 ||          // 726����֡
				pStreamHeader->frame_type == APP_NET_TCP_COM_DST_AAC)			 // AAC����֡
		{
			pThis->m_pStreamInfo->nAudioBytes += nFrameLength;
			pThis->m_pStreamInfo->nAudioFrameCount++;
		}
	}
};