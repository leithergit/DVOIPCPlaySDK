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
#include <algorithm>
#include <windows.h>
#include <winsock2.h>
#include <process.h>
#include <thread>
#include <chrono>
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
	// ����һ���򵥵Ĳ��ɼ��Ĵ���
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
		return ::CreateWindow(szWindowClass,			// ������
							  szWindowClass,			// ���ڱ��� 
							  WS_EX_TOPMOST | WS_POPUP,	// ���ڷ��
							  CW_USEDEFAULT, 			// �������Ͻ�X����
							  0, 						// �������Ͻ�Y����
							  nWidth, 					// ���ڿ��
							  nHeight, 					// ���ڸ߶�
							  NULL, 					// �����ھ��
							  NULL,						// �˵����
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
		case 0:									// ��˼I֡��Ϊ0�����ǹ̼���һ��BUG���д�����
		case APP_NET_TCP_COM_DST_IDR_FRAME: 	// IDR֡��
		case APP_NET_TCP_COM_DST_I_FRAME:		// I֡��
			pHeader->nType = FRAME_I;
			break;
		case APP_NET_TCP_COM_DST_P_FRAME:       // P֡
		case APP_NET_TCP_COM_DST_B_FRAME:       // B֡
		case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A�ɱ���֡
		case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U�ɱ���֡
		case APP_NET_TCP_COM_DST_726:           // 726����֡
		case APP_NET_TCP_COM_DST_AAC:           // AAC����֡��
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


#define _Frame_PERIOD			30.0f		///< һ��֡������

/// @brief ����DVO˽��֡�ṹ��
struct FrameParser
{
	DVOFrameHeaderEx	*pHeaderEx;		///< DVO˽��¼���֡����
	UINT				nFrameSize;		///< pFrame�����ݳ���
	byte				*pLawFrame;		///< ԭʼ��������
	UINT				nLawFrameSize;	///< ԭʼ�������ݳ���
};

/// @brief �߳���Ϣ��
enum SleepWay
{
	Sys_Sleep = 0,		///< ֱ�ӵ���ϵͳ����Sleep
	Wmm_Sleep = 1,		///< ʹ�ö�ý��ʱ����߾���
	Std_Sleep = 2		///< ʹC++11�ṩ���߳����ߺ���
};

struct FileFrameInfo
{
	UINT	nOffset;
	bool	bIFrame;
	time_t	tTimeStamp;
};
class CDvoPlayer
{
public:
	int		nSize;
private:
	shared_ptr<DVO_MEDIAINFO>m_pMediaHeader;/// ý���ļ�ͷ	
	list<StreamFramePtr>::iterator ItLoop;
	list<StreamFramePtr>m_listAudioCache;///< ������֡����
	list<StreamFramePtr>m_listVideoCache;///< ������֡����	
	CRITICAL_SECTION	m_csVideoCache;
	CRITICAL_SECTION	m_csAudioCache;	
	int					m_nMaxFrameCache;///< �����Ƶ��������,Ĭ��ֵ125
										///< ��m_FrameCache�е���Ƶ֡��������m_nMaxFrameCacheʱ�����޷��ټ�������������
private:
	// ��Ƶ������ر���
	int			m_nTotalFrames;			///< ��ǰ�ļ�����Ч��Ƶ֡������,���������ļ�ʱ��Ч
	HWND		m_hWnd;					///< ������Ƶ�Ĵ��ھ��
	bool		m_bProbeMode;			///< �Ƿ���̽������ģʽ����̽��ģʽ�£�����ɾ��I֡�������
	bool		m_bIFrameRecved;		///< �Ƿ��Ѿ��յ���һ��I֡
	int			m_nDisardFrames;		///< �յ���һ��I֡ǰ����������֡����
	int			m_nFrameOffset;			///< ��Ƶ֡����ʱ��֡��ƫ��

	DVO_CODEC	m_nVideoCodec;			///< ��Ƶ�����ʽ @see DVO_CODEC	
	static		CAvRegister avRegister;	
	static shared_ptr<CDSoundPlayer> m_pDsPlayer;///< DirectSound���Ŷ���ָ��
	CDxSurface* m_pDxSurface;			///< Direct3d Surface��װ��,������ʾ��Ƶ
	static shared_ptr<CSimpleWnd>m_pWndDxInit;///< ��Ƶ��ʾʱ�����Գ�ʼ��DirectX�����ش��ڶ���
	bool		m_bRefreshWnd;			///< ֹͣ����ʱ�Ƿ�ˢ�»���
	int			m_nVideoWidth;			///< ��Ƶ���
	int			m_nVideoHeight;			///< ��Ƶ�߶�	
	int			m_nFrameEplased;		///< �Ѿ�����֡��
	int			m_nCurVideoFrame;		///< ��ǰ�����ŵ���Ƶ֡ID
	time_t		m_tCurFrameTimeStamp;
	time_t		m_tLastFrameTime;
	int			m_nPlayRate;			///< ��ǰ���ŵı���ȡֵ-32~32,-32ʱΪ32֮һ�٣�32��Ϊ32����
	double		m_dfLastTimeVideoPlay;	///< ǰһ����Ƶ���ŵ�ʱ��
	double		m_dfTimesStart;			///< ��ʼ���ŵ�ʱ��
	bool		m_bEnableHaccel;		///< �Ƿ�����Ӳ����
	bool		m_bFitWindow;			///< ��Ƶ��ʾ�Ƿ���������
										///< Ϊtrueʱ�������Ƶ��������,�������ͼ������,���ܻ����ͼ�����
										///< Ϊfalseʱ����ֻ��ͼ��ԭʼ�����ڴ�������ʾ,������������,���Ժ�ɫ������ʾ
private:
// 	USER_HANDLE	m_hUserHandle;
// 	REAL_HANDLE	m_hStreamHandle;

private:	// ��Ƶ������ر���

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
	volatile bool m_bThreadPlayVideoRun;
	volatile bool m_bThreadPlayAudioRun;
	volatile bool m_bThreadFileAbstractRun;
	bool		m_bPause;				///< �Ƿ�����ͣ״̬
	byte		*m_pYUV;				///< YVU��׽ר���ڴ�
	int			m_nYUVSize ;			///<
	shared_ptr<byte>m_pYUVPtr;
private:	// �ļ�������ر���
	HANDLE		m_hDvoFile;				///< ���ڲ��ŵ��ļ����
	char		*m_pszFileName;			///< ���ڲ��ŵ��ļ���,���ֶν����ļ�����ʱ����Ч
	FileFrameInfo	*m_pFrameOffsetTable;///< ��Ƶ֡ID��Ӧ�ļ�ƫ�Ʊ�
	volatile LONG m_nSeekOffset;		///< ���ļ���ƫ��
	CRITICAL_SECTION	m_csSeekOffset;
	int			m_nSDKVersion;			///< ����¼���ļ���SDK�汾	
	int			m_nMaxFrameSize;		///< ���I֡�Ĵ�С�����ֽ�Ϊ��λ,Ĭ��ֵ256K
	int			m_nFPS;	
	int			m_nPlayInterval;		///< ֡���ż��,��λ����
private:
	CaptureFrame	m_pfnCaptureFrame;
	void*			m_pUserCaptureFrame;
	CaptureYUV		m_pfnCaptureYUV;
	void*			m_pUserCaptureYUV;
	CaptureYUVEx	m_pfnCaptureYUVEx;
	void*			m_pUserCaptureYUVEx;
	FilePlayProc	m_pFilePlayCallBack;
	void*			m_pUserFilePlaye;
	
	SleepWay		m_nSleepWay;		///< �߳����ߵķ�ʽ,@see SleepWay
public:
	
private:
	CDvoPlayer()
	{
		ZeroMemory(&m_csVideoCache, sizeof(CDvoPlayer) - offsetof(CDvoPlayer, m_csVideoCache));
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
	// �����ļ�Ѱַ
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
	/// @brief �ж�����֡�Ƿ�ΪDVO˽�е���Ƶ֡
	/// @param[in]	pFrameHeader	DVO˽��֡�ṹָ�����@see DVOFrameHeaderEx
	/// @param[out] bIFrame			�ж�����֡�Ƿ�ΪI֡
	/// -# true			����֡ΪI֡
	///	-# false		����֡Ϊ����֡
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
	/// @brief ȡ����Ƶ�ļ���һ֡��ID,�൱����Ƶ�ļ��а�������Ƶ��֡��
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
			SetFilePointer(m_hDvoFile, -m_nMaxFrameSize,nullptr, FILE_END) == INVALID_SET_FILE_POINTER)
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
		// �����������m_nMaxFrameSize�����ڵ�����֡
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
			case FRAME_G711A:      // G711 A�ɱ���֡
			case FRAME_G711U:      // G711 U�ɱ���֡
			case FRAME_G726:       // G726����֡
			case FRAME_AAC:        // AAC����֡��
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
public:

	void SetPlayCallBack(FilePlayProc pFilePlayCallBack,void *pUserPtr)
	{
		m_pFilePlayCallBack = pFilePlayCallBack;
		m_pUserFilePlaye = pUserPtr;
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
		CAutoLock lock(&m_csVideoCache);
		if (szFileName)
		{
			m_pszFileName = _New char[MAX_PATH];
			strcpy(m_pszFileName, szFileName);
		}
		if (hWnd && IsWindow(hWnd))
		{// �ز�Ҫ�д��ڲŻᲥ����Ƶ������
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
		m_nMaxFrameSize	 = 1024 * 256;
		m_nFPS			 = 25;				// FPS��Ĭ��ֵΪ25
		m_nPlayRate		 = 1;
		m_nPlayInterval = 25;
		m_nVideoCodec	 = CODEC_H264;		// ��ƵĬ��ʹ��H.264����
		m_nAudioCodec	 = CODEC_G711U;		// ��ƵĬ��ʹ��G711U����
#ifdef _DEBUG
		m_nMaxFrameCache = 500;				// Ĭ�������Ƶ��������Ϊ500
#else
		m_nMaxFrameCache = 100;				// Ĭ�������Ƶ��������Ϊ100
#endif
		nSize = sizeof(CDvoPlayer);
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

	void SetRefresh(bool bRefresh = true)
	{
		m_bRefreshWnd = bRefresh;
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
		
	/// @brief			��ʼ����
	/// @param [in]		bEnaleAudio		�Ƿ�����Ƶ����
	/// #- true			������Ƶ����
	/// #- false		�ر���Ƶ����
	/// @param [in]		bEnableHaccel	�Ƿ���Ӳ���빦��
	/// #- true			����Ӳ����
	/// #- false		����Ӳ����
	/// @param [in]		bFitWindow		��Ƶ�Ƿ���Ӧ����
	/// #- true			��Ƶ��������,�������ͼ������,���ܻ����ͼ�����
	/// #- false		ֻ��ͼ��ԭʼ�����ڴ�������ʾ,������������,���Ժ�ɫ������ʾ
	/// @retval			0	�����ɹ�
	/// @retval			1	������������
	/// @retval			-1	���������Ч
	/// @remark			����������ʱ����Ӧ��֡������ʵ��δ�������ţ����Ǳ����˲��Ŷ����У�Ӧ�ø���dvoplay_PlayStream
	///					�ķ���ֵ���жϣ��Ƿ�������ţ���˵��������������Ӧ����ͣ����
	int StartPlay(bool bEnaleAudio = false,bool bEnableHaccel = false,bool bFitWindow = true)
	{
		m_bPause = false;
		m_bFitWindow = bFitWindow;
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
			if (m_hDvoFile == INVALID_HANDLE_VALUE)
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
				//m_nFPS = m_pMediaHeader->nFps;		// FPS���ٴ��ļ�ͷ�л��,���Ǵ�֡���������
				break;
			default:
				return DVO_Error_InvalidSDKVersion;
			}
			//m_nFrameInterval = 1000 / m_nFPS;
			int nDvoError = GetLastFrameID(m_nTotalFrames);
			if (nDvoError != DVO_Succeed)
			{
				CloseHandle(m_hDvoFile);
				return nDvoError;
			}
			// GetLastFrameIDȡ�õ������һ֡��ID����֡����Ҫ�ڴ˻�����+1
			m_nTotalFrames++;
			if (m_pDxSurface)
				m_pDxSurface->DisableVsync();		// ���ô�ֱͬ��������֡���п��ܳ�����ʾ����ˢ���ʣ��Ӷ��ﵽ���ٲ��ŵ�Ŀ��
			
			// �����ļ������߳�
			m_bThreadParserRun = true;
			m_hThreadParser = (HANDLE)_beginthreadex(nullptr, 0, ThreadParser, this, 0, 0);
			// �����ļ���ϢժҪ�߳�
			m_bThreadFileAbstractRun = true;
			m_hThreadFileAbstract = (HANDLE)_beginthreadex(nullptr, 0, ThreadFileAbstract, this, 0, 0);
		}
		else
		{
//  		if (!m_pMediaHeader)
//  			return DVO_Error_NotInputStreamHeader;		// δ����������ͷ

			m_listVideoCache.clear();
			m_listAudioCache.clear();
		}
		if (m_hWnd)
		{
			// �����������߳�
			m_bThreadPlayVideoRun = true;			
			m_hThreadPlayVideo = (HANDLE)_beginthreadex(nullptr, 0, ThreadPlayVideo, this, 0, 0);			
			
			if (bEnaleAudio)
			{
				m_bThreadPlayAudioRun = true;
				m_hThreadPlayAudio = (HANDLE)_beginthreadex(nullptr, 0, ThreadPlayAudio, this, CREATE_SUSPENDED, 0);
				if (!m_pDsPlayer->IsInitialized())
					m_pDsPlayer->Initialize(m_hWnd, Audio_Play_Segments);
				m_pDsPlayer->StartPlay();
				m_bEnableAudio = true;	
			}
		}
		return DVO_Succeed;
	}
	
	void FitWindow(bool bFitWindow)
	{
		m_bFitWindow = bFitWindow;
	}

	/// @brief			����DVO˽��ʽ����������
	/// @retval			0	�����ɹ�
	/// @retval			1	������������
	/// @retval			-1	���������Ч
	/// @remark			����������ʱ����Ӧ��֡������ʵ��δ�������ţ����Ǳ����˲��Ŷ����У�Ӧ�ø���dvoplay_PlayStream
	///					�ķ���ֵ���жϣ��Ƿ�������ţ���˵��������������Ӧ����ͣ����
	int InputStream(unsigned char *szFrameData, int nFrameSize)
	{
		if (!szFrameData || nFrameSize < sizeof(DVOFrameHeaderEx))
			return DVO_Error_InvalidFrame;
		DVOFrameHeaderEx *pHeaderEx = (DVOFrameHeaderEx *)szFrameData;
		switch (pHeaderEx->nType)
		{
			case FRAME_P:
			case FRAME_B:
			case FRAME_I:
			{
				if (!m_hThreadPlayVideo)
					return DVO_Error_VideoThreadNotRun;
				CAutoLock lock(&m_csVideoCache);
				if (m_listVideoCache.size() >= m_nMaxFrameCache)
					return DVO_Error_FrameCacheIsFulled;
				StreamFramePtr pStream = make_shared<StreamFrame>(szFrameData, nFrameSize);
				if (!pStream)
					return DVO_Error_InsufficentMemory;
				m_listVideoCache.push_back(pStream);
				break;
			}
			case FRAME_G711A:      // G711 A�ɱ���֡
			case FRAME_G711U:      // G711 U�ɱ���֡
			case FRAME_G726:       // G726����֡
			case FRAME_AAC:        // AAC����֡��
			{
				if (!m_hThreadPlayVideo)
					return DVO_Error_AudioThreadNotRun;
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

	/// @brief			����DVO IPC��������
	/// @retval			0	�����ɹ�
	/// @retval			1	������������
	/// @retval			-1	���������Ч
	/// @remark			����������ʱ����Ӧ��֡������ʵ��δ�������ţ����Ǳ����˲��Ŷ����У�Ӧ�ø���dvoplay_PlayStream
	///					�ķ���ֵ���жϣ��Ƿ�������ţ���˵��������������Ӧ����ͣ����
	int InputStream(IN byte *pFrameData, IN int nFrameType, IN int nFrameLength, int nFrameNum, time_t nFrameTime)
	{
		switch (nFrameType)
		{
			case 0:									// ��˼I֡��Ϊ0�����ǹ̼���һ��BUG���д�����
			case APP_NET_TCP_COM_DST_IDR_FRAME: 	// IDR֡��
			case APP_NET_TCP_COM_DST_I_FRAME:		// I֡��		
			case APP_NET_TCP_COM_DST_P_FRAME:       // P֡��
			case APP_NET_TCP_COM_DST_B_FRAME:       // B֡��
			{
				CAutoLock lock(&m_csVideoCache);
				if (m_listVideoCache.size() >= m_nMaxFrameCache)
					return DVO_Error_FrameCacheIsFulled;
				StreamFramePtr pStream = make_shared<StreamFrame>(pFrameData, nFrameType, nFrameLength, nFrameNum, nFrameTime);
				if (!pStream)
					return DVO_Error_InsufficentMemory;

				m_listVideoCache.push_back(pStream);
				//DxTraceMsg("%s VideoCache size = %d.\n", __FUNCTION__, m_listVideoCache.size());
			}
				break;
			case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A�ɱ���֡
			case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U�ɱ���֡
			case APP_NET_TCP_COM_DST_726:           // 726����֡
			case APP_NET_TCP_COM_DST_AAC:           // AAC����֡��
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
		m_bThreadFileAbstractRun = false;
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
		if (m_hThreadFileAbstract)
		{
			WaitForSingleObject(m_hThreadFileAbstract, INFINITE);
			CloseHandle(m_hThreadFileAbstract);
			DxTraceMsg("%s ThreadFileAbstract has exit.\n", __FUNCTION__);
		}
		if (m_pDsPlayer && m_bEnableAudio)
		{
			m_pDsPlayer->StopPlay();
		}
		if (m_pFrameOffsetTable)
			delete []m_pFrameOffsetTable;
		
#ifdef _DEBUG
		m_hThreadPlayVideo = nullptr;		
		m_hThreadParser = nullptr;
		m_hThreadFileAbstract = nullptr;
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
		UINT nAdapter = D3DADAPTER_DEFAULT;
		if (!pDecoder->InitD3D(nAdapter))
			return false;
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
	inline int GetCacheSize(int &nCacheSize)
	{
		if (m_hThreadPlayVideo)
		{
			CAutoLock lock(&m_csVideoCache);
			nCacheSize = m_listVideoCache.size();
			return DVO_Succeed;
		}
		else
			return DVO_Error_PlayerNotStart;
	}

	/// @brief			��ȡ�ѷ�ʱ��,��λ����
	/// @param [in]		hPlayHandle		��OpenFile��OpenStream���صĲ��ž��
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	inline LONGLONG GetTimeEplased(time_t tTimeEplased)
	{
		if (m_hThreadPlayVideo)
		{
			tTimeEplased = (time_t)(1000 * (GetExactTime() - m_dfTimesStart));
			return DVO_Succeed; 
		}
		else
			return DVO_Error_PlayerNotStart;
	}

	/// @brief			ȡ���ļ����ŵ���Ƶ֡��
	inline int  GetFps(int &nFPS)
	{
		if (m_hThreadPlayVideo)
		{
			nFPS  = m_nFPS;
			return DVO_Succeed;
		}
		else
			return DVO_Error_PlayerNotStart;
	}

	/// @brief			ȡ���ļ��а�������Ч��Ƶ֡������
	/// @remark			ֻ�����ļ�����ʱ����Ч
	inline int  GetFrames(int &nFrames)
	{
		if (m_hDvoFile && m_hThreadPlayVideo)
		{
			nFrames = m_nTotalFrames;
			return DVO_Succeed;
		}
		else
			return DVO_Error_NotFilePlayer;
	}

	/// @brief			ȡ�õ�ǰ������Ƶ��֡ID
	int  GetCurrentFrameID(int &nFrameID)
	{
		if (m_hThreadPlayVideo)
		{
			nFrameID = m_nCurVideoFrame;
			return DVO_Succeed;
		}
		else
			return DVO_Error_PlayerNotStart;
	}
	time_t GetGurrentFrameTime(time_t &tTimeStamp)
	{
		if (m_hThreadPlayVideo)
		{
			if (m_pFrameOffsetTable)
			{
				tTimeStamp = (m_tCurFrameTimeStamp - m_pFrameOffsetTable[0].tTimeStamp) / 1000;
				return DVO_Succeed;
			}
			else
				return DVO_Error_PlayerNotStart;
		}
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
		case Rate_One20th:
		case Rate_One16th:
		case Rate_One10th:
		case Rate_One08th:
		case Rate_Quarter:
		case Rate_Half:
		{
			m_nPlayInterval = 1000* (-1)*nPlayRate / m_nFPS;
			break;
		}
		case Rate_01X:
		case Rate_02X:
		case Rate_04X:
		case Rate_08X:
		case Rate_10X:
		case Rate_16X:
		case Rate_20X:
		case Rate_24X:
		case Rate_32X:
		{
			m_nPlayInterval = 1000 / (m_nFPS*nPlayRate);
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
		ItLoop = m_listVideoCache.begin();
		m_nFrameOffset = 0;
		LeaveCriticalSection(&m_csVideoCache);

		EnterCriticalSection(&m_csAudioCache);
		m_listAudioCache.clear();
		LeaveCriticalSection(&m_csAudioCache);
		
		
		// ���ļ�ժҪ�У�ȡ���ļ�ƫ����Ϣ
		// ���������I֡
		int nForward = nFrameID, nBackWord = nFrameID;
		while (nForward < m_nTotalFrames)
		{
			if (m_pFrameOffsetTable[nForward].bIFrame)
				break;
			nForward++;
		}
		while (nBackWord > 0 )
		{
			if (m_pFrameOffsetTable[nBackWord].bIFrame)
				break;
			nBackWord --;
		}
		if ((nForward - nFrameID) <= (nFrameID - nBackWord ))
			SetSeekOffset(m_pFrameOffsetTable[nForward].nOffset);
		else
			SetSeekOffset(m_pFrameOffsetTable[nBackWord].nOffset);
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
		
		if (bEnable)
		{
			m_pDsPlayer->StartPlay();
			if (!m_hThreadPlayAudio)
			{
				m_bThreadPlayAudioRun = true;
				m_hThreadPlayAudio = (HANDLE)_beginthreadex(nullptr, 0, ThreadPlayAudio, this, 0, 0);
			}
			if (!m_pDsPlayer->IsInitialized())
				m_pDsPlayer->Initialize(m_hWnd, Audio_Play_Segments);
			
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
		
		// ֡ͷ��Ϣ������
		if ((nOffset + sizeof(DVOFrameHeaderEx)) >= nDataSize)
			return false;

		*ppBuffer += nOffset;
		
		// ֡���ݲ�����
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
	///< @brief ��Ƶ�ļ������߳�
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
		pThis->m_tLastFrameTime = 0;
		
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
			bool bIFrame = false;
			bool bFrameInput = true;
			while (pThis->m_bThreadParserRun)
			{
				if (pThis->m_bPause)
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
					{
						if (pThis->m_pfnCaptureFrame(pThis, (byte *)Parser.pHeaderEx, Parser.nFrameSize, pThis->m_pUserCaptureFrame) == 1)
						{
							bPause = true;
							Sleep(10);
						}
					}
					if (!pThis->m_hWnd)		// û�д��ھ�����򲻽��в���
					{
						Sleep(10);
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
					case DVO_Error_FrameCacheIsFulled:	// ����������
						bFrameInput = false;
						break;
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
					case DVO_Error_FrameCacheIsFulled:	// ����������
						bFrameInput = false;					
						Sleep(10);
						break;
					}
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

	/// @brief			��ȡ��������	
	/// @param [in]		pVideoCodec		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
	/// @param [out]	pAudioCodec	���ص�ǰhPlayHandle�Ƿ��ѿ���Ӳ���빦��
	/// @remark �������Ͷ�����ο�:@see DVO_CODEC
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	/// @retval			DVO_Error_PlayerNotStart	��������δ����,�޷�ȡ�ò��Ź��̵���Ϣ������
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

	/// @brief			����֡ID
	/// @param [in,out]	pBuffer			����DVO˽��¼���ļ��е�������
	/// @param [in,out]	nDataSize		pBuffer����Ч���ݵĳ���
	/// @param [in]		nStartOffset	pBuffer�����ݵ����ļ��е�ƫ��
	/// @retval			true	�����ɹ�
	/// @retval			false	ʧ�ܣ�pBuffer�Ѿ�û����Ч��֡����
	/*
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
			if (IsDVOVideoFrame(Frame(&pBuffer[nOffset])))			
			{
				m_pFrameOffsetTable[nCurFrames++].nOffset = (nOffset + nStartFileOffset);
			}
			nOffset += FrameSize(&pBuffer[nOffset]);
		}

		return true;
	}*/
	
	/// �ļ�ժҪ�߳�
	static UINT __stdcall ThreadFileAbstract(void *p)
	{
		CDvoPlayer* pThis = (CDvoPlayer *)p;
		if (pThis->m_nTotalFrames == 0)
		{
			assert(false);
			return 0;
		}
#ifdef _DEBUG
		double dfTimeStart = GetExactTime();
#endif
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
		
		LARGE_INTEGER liFileSize;
		DWORD nBufferSize = 1024 * 1024 * 32;
		if (!GetFileSizeEx(hDvoFile, &liFileSize))
			 return 0;
		if (liFileSize.QuadPart <= nBufferSize)
			nBufferSize = liFileSize.LowPart;
		// ���ٷ����ļ�����ΪStartPlay�Ѿ�����������ȷ��
		DWORD nOffset = sizeof(DVO_MEDIAINFO);		
		if (SetFilePointer(hDvoFile, nOffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			assert(false);
			return 0;
		}
		
		pThis->m_pFrameOffsetTable = _New FileFrameInfo[pThis->m_nTotalFrames];
		ZeroMemory(pThis->m_pFrameOffsetTable, sizeof(FileFrameInfo)*pThis->m_nTotalFrames);
		
		byte *pBuffer = _New byte[nBufferSize];
		while (!pBuffer)
		{
			if (nBufferSize <= 1024 * 1024)
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
		int nFrames = 0;
		LONG nSeekOffset = 0;
		DWORD nBytesRead = 0;
		DWORD nDataLength = 0;
		byte *pFrameBuffer = nullptr;
		FrameParser Parser;
		int nFrameOffset = sizeof(DVO_MEDIAINFO);
		bool bIFrame = false;
		while (pThis->m_bThreadFileAbstractRun)
		{
			if (!ReadFile(hDvoFile, &pBuffer[nDataLength], (nBufferSize - nDataLength), &nBytesRead, nullptr))
			{
				DxTraceMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return 0;
			}
			if (nBytesRead == 0)		// δ��ȡ�κ����ݣ��Ѿ��ﵽ�ļ���β
				break;
			pFrameBuffer = pBuffer;
			nDataLength += nBytesRead;
			while (pThis->m_bThreadFileAbstractRun)
			{
				if (!pThis->ParserFrame(&pFrameBuffer, nDataLength, &Parser))
					break;
				if (IsDVOVideoFrame(Parser.pHeaderEx, bIFrame))	// ֻ��¼��Ƶ֡���ļ�ƫ��
				{
					pThis->m_pFrameOffsetTable[nFrames].nOffset		 = nFrameOffset;
					pThis->m_pFrameOffsetTable[nFrames].bIFrame		 = bIFrame;
					pThis->m_pFrameOffsetTable[nFrames].tTimeStamp	 = Parser.pHeaderEx->nTimestamp;
					nFrames++;
				}
				nFrameOffset += Parser.nFrameSize;
			}
			nOffset += nBytesRead;
			// �������ݳ�ΪnDataLength
			memcpy(pBuffer, pFrameBuffer, nDataLength);
		}
#ifdef _DEBUG
		DxTraceMsg("%s TimeSpan = %.3f.\n", __FUNCTION__, TimeSpanEx(dfTimeStart));
#endif
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


	/// @brief			����̽���ȡ���ݰ��ص�����
	/// @param [in]		opaque		�û�����Ļص���������ָ��
	/// @param [in]		buf			��ȡ���ݵĻ���
	/// @param [in]		buf_size	����ĳ���
	/// @return			ʵ�ʶ�ȡ���ݵĳ���
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
						   FramePtr->FrameHeader()->nType == 0;		// ��˼I֡����ֵΪ0
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
					if (!bIFrame)		// ��̽��ģʽ�£��������е�һ��I֮֡ǰ��֡
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

	static UINT __stdcall ThreadPlayVideo(void *p)
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
		// �ȴ�I֡
		double tFirst = GetExactTime();
#ifdef _DEBUG
		double dfTimeout = 900.0f;
#else
		double dfTimeout = 4.0f;		// �ȴ�I֡ʱ��
#endif
		while (true)
		{
			if ((TimeSpanEx(tFirst)) < dfTimeout)
			{
				ThreadSleep(10);
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
		// ̽����������
		pThis->ItLoop = pThis->m_listVideoCache.begin();
		pThis->m_bProbeMode = true;
		if (pDecodec->ProbeStream(pThis,ReadAvData, 1024 * 256) != 0)
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
			pThis->m_nVideoCodec = CODEC_H264;
		else if (pDecodec->m_nCodecId == AV_CODEC_ID_HEVC)
			pThis->m_nVideoCodec = CODEC_H265;
		else
		{
			pThis->m_nVideoCodec = CODEC_UNKNOWN;
			DxTraceMsg("%s Unsupported Video Codec.\n", __FUNCTION__);
			assert(false);
			return 0;
		}

		pThis->ItLoop = pThis->m_listVideoCache.begin();

		// �ָ���Ƶ�߳�
		::EnterCriticalSection(&pThis->m_csAudioCache);
		pThis->m_listAudioCache.clear();
		::LeaveCriticalSection(&pThis->m_csAudioCache);
		if (pThis->m_hThreadPlayAudio)
			ResumeThread(pThis->m_hThreadPlayAudio);

		double dfT1 = GetExactTime() - pThis->m_nPlayInterval;
		double dfTimeSpan = 0.0f;
		AVPacket *pAvPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
		StreamFramePtr FramePtr;
		int nGot_picture = 0;
		AVFrame *pAvFrame = av_frame_alloc();
		DWORD nResult = 0;
		int nTimeSpan = 0;
		DWORD dwSleepPricision = GetSleepPricision();		// ȡSleep������ʱ�侫��
		int nFrameInterval = 40;
		//StreamFramePtr FramePtr;
		byte *pYUV = nullptr;
		int nYUVSize = 0;
		pThis->m_dfTimesStart = GetExactTime();
		double dfT2 = GetExactTime();
		av_init_packet(pAvPacket);
#ifdef _DEBUG
		EnterCriticalSection(&pThis->m_csVideoCache);
		DxTraceMsg("%s Size of Video cache = %d .\n", __FUNCTION__, pThis->m_listVideoCache.size());
		LeaveCriticalSection(&pThis->m_csVideoCache);
#endif
		RECT rtRender;
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
// 			if (pThis->m_nPlayRate >= 10)
// 			{// ��ʼ��֡ģʽ
// 
// 			}
// 			else
			{
				nTimeSpan = (int)(TimeSpanEx(dfT1) * 1000);
				if (nTimeSpan >= (pThis->m_nPlayInterval))
				{
					pThis->m_nFPS = 1000 / nTimeSpan;					
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
						/*Thread*/Sleep(10);
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
						/*Thread*/Sleep(nSleepTime);
					continue;
				}
			}
			// ��̬����֡�����֡��
			if (pThis->m_tLastFrameTime)
			{
				int nFrameInterval = (FramePtr->FrameHeader()->nTimestamp - pThis->m_tLastFrameTime)/1000;
				if (nFrameInterval > 0)
				{
					if (pThis->m_nPlayRate >= Rate_01X && pThis->m_nPlayRate <= Rate_32X)
						pThis->m_nPlayInterval = nFrameInterval / pThis->m_nPlayRate;
					else if (pThis->m_nPlayRate >= Rate_One32th && pThis->m_nPlayRate <= Rate_Half)
						pThis->m_nPlayInterval = nFrameInterval *(-pThis->m_nPlayRate);
					else
						pThis->m_nPlayInterval = 40;
					/*pThis->m_nFPS = 1000  / pThis->m_nPlayInterval;*/
				}
			}
			pThis->m_tLastFrameTime = FramePtr->FrameHeader()->nTimestamp;
// ��Ϊ���֡������Դ���,���鲻Ҫɾ��
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
				// ��ʼ��ʾ���
				// ʹ���߳���CDxSurface������ʾͼ��
				if (!pThis->m_pDxSurface->IsInited())		// D3D�豸��δ��ʼ��
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
				}				
				
				if (pThis->m_bFitWindow)
					pThis->m_pDxSurface->Render(pAvFrame, pThis->m_hWnd, nullptr);
				else
				{
					GetWindowRect(pThis->m_hWnd, &rtRender);
					POINT ptLeftTop, ptRightBottom;
					ptLeftTop.x		 = rtRender.left;
					ptLeftTop.y		 = rtRender.top;
					ptRightBottom.x	 = rtRender.right;
					ptRightBottom.y	 = rtRender.bottom;

					ScreenToClient(pThis->m_hWnd, &ptLeftTop);
					ScreenToClient(pThis->m_hWnd, &ptRightBottom);
					rtRender.left	 = ptLeftTop.x;
					rtRender.top	 = ptLeftTop.y;
					rtRender.right	 = ptRightBottom.x;
					rtRender.bottom	 = ptRightBottom.y;
					int nWndWidth	 = rtRender.right - rtRender.left;
					int nWndHeight	 = rtRender.bottom - rtRender.top;
					float fScaleWnd	 = (float)nWndWidth / nWndHeight;
					float fScaleVideo= (float)pAvFrame->width / pAvFrame->height;
					if (fScaleVideo > fScaleWnd)			// ���ڸ߶ȳ�������,��Ҫȥ��һ���ָ߶�,��Ƶ��Ҫ���¾���
					{
						int nNewHeight = (int)nWndWidth/fScaleVideo;
						int nOverHeight = nWndHeight - nNewHeight;
						if ((float)nOverHeight / nWndHeight > 0.1f)	// ���ڸ߶ȳ���10%,������߶ȣ�������Ըò���
						{
							rtRender.top += nOverHeight / 2;
							rtRender.bottom -= nOverHeight / 2;
						}
					}
					else if (fScaleVideo < fScaleWnd)		// ���ڿ�ȳ�������,��Ҫȥ��һ���ֿ�ȣ���Ƶ��Ҫ���Ҿ���
					{
						int nNewWidth = nWndHeight*fScaleVideo;
						int nOverWidth = nWndWidth- nNewWidth;
						if ((float)nOverWidth / nWndWidth > 0.1f)
						{
							rtRender.left += nOverWidth / 2;
							rtRender.right -= nOverWidth / 2;
						}
					}
				
					pThis->m_pDxSurface->Render(pAvFrame, pThis->m_hWnd, &rtRender);
				}
				pThis->ProcessYVUCapture(pAvFrame, (LONGLONG)GetExactTime()*1000);
				if (pThis->m_pFilePlayCallBack)
					pThis->m_pFilePlayCallBack(pThis, pThis->m_pUserFilePlaye);
				av_frame_unref(pAvFrame);
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
			if (pThis->m_nPlayRate > 4 || pThis->m_nPlayRate < -4)
			{// ���ű��ʳ���4��С��1/4�����ٲ�����Ƶ,��Ϊ��ʱ��Ƶ����ʧ�棬Ӱ������
				if (m_pDsPlayer->IsPlaying())
					m_pDsPlayer->StopPlay();
				::EnterCriticalSection(&pThis->m_csAudioCache);
				if (pThis->m_listAudioCache.size() > 0)	
					pThis->m_listAudioCache.pop_front();
				::LeaveCriticalSection(&pThis->m_csAudioCache);
			}
			if (nTimeSpan >= nAudioFrameInterval)
			{
				if (!m_pDsPlayer->IsPlaying())
					m_pDsPlayer->StartPlay();
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
					ThreadSleep(10);
			}
			else
			{	// ���Ʋ����ٶ�
				nTimeSpan = (int)(1000 * (GetExactTime() - dfT1));
				int nSleepTime = nAudioFrameInterval - nTimeSpan;
				if (nSleepTime > dwSleepPricision)
					ThreadSleep(nSleepTime);
				continue;
			}
			if (!FramePtr)
				continue;
			if (pAudioDecoder->GetCodecType() == CODEC_UNKNOWN)
			{
				const DVOFrameHeaderEx *pHeader = FramePtr->FrameHeader();
				nDecodeSize = pHeader->nLength * 2;		//G711 ѹ����Ϊ2��
				switch (pHeader->nType)
				{
				case FRAME_G711A:			//711 A�ɱ���֡
				{
					pAudioDecoder->SetACodecType(CODEC_G711A, SampleBit16);
					pThis->m_nAudioCodec = CODEC_G711A;
					DxTraceMsg("%s Audio Codec:G711A.\n", __FUNCTION__);
					break;
				}
				case FRAME_G711U:			//711 U�ɱ���֡
				{
					pAudioDecoder->SetACodecType(CODEC_G711U, SampleBit16);
					pThis->m_nAudioCodec = CODEC_G711U;
					DxTraceMsg("%s Audio Codec:G711U.\n", __FUNCTION__);
					break;
				}

				case FRAME_G726:			//726����֡
				{
					// ��ΪĿǰDVO�����G726����,��Ȼ���õ���16λ��������ʹ��32λѹ�����룬��˽�ѹ��ʹ��SampleBit32
					pAudioDecoder->SetACodecType(CODEC_G726, SampleBit32);
					pThis->m_nAudioCodec = CODEC_G726;
					nDecodeSize = FramePtr->FrameHeader()->nLength * 8;		//G726���ѹ���ʿɴ�8��
					DxTraceMsg("%s Audio Codec:G726.\n", __FUNCTION__);
					break;
				}
				case FRAME_AAC:				//AAC����֡��
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

			if (m_pDsPlayer->IsPlaying() && 
				pAudioDecoder->Decode(pPCM, nPCMSize, (byte *)FramePtr->Framedata(), FramePtr->FrameHeader()->nLength) != 0)
			{
				m_pDsPlayer->PutWaveData(pPCM, nPCMSize);
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