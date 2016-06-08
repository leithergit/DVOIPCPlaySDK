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
//#include <vld.h>
#include <new>
#include <assert.h>
#include <list>
#include <memory>
#include <algorithm>
#include <windows.h>
#include <winsock2.h>
#include <process.h>
#include <Shlwapi.h>
#include <MMSystem.h>
//#include <VersionHelpers.h> // Windows SDK 8.1 �����
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
#include "Runlog.h"
#include "DirectDraw.h"

#ifdef _DEBUG
#define _New	new
#else
#define _New	new (std::nothrow)
#endif

using namespace std;
using namespace std::tr1;
#pragma comment(lib,"ws2_32")
#pragma warning (disable:4018)

// ����ת���°��֡ͷ
#define FrameSize(p)	(((DVOFrameHeaderEx*)p)->nLength + sizeof(DVOFrameHeaderEx))	
#define Frame(p)		((DVOFrameHeaderEx *)p)

// ����ת���ɰ��֡ͷ
#define FrameSize2(p)	(((DVOFrameHeader*)p)->nLength + sizeof(DVOFrameHeader))	
#define Frame2(p)		((DVOFrameHeader *)p)

#ifdef _DEBUG
#define _MaxTimeCount	125
/// @brief	����׷�ٴ���ִ��ʱ����࣬��Ҫ���ڵ���
/// @remark ��Щʱ�䣬Ҫ׷�������Щ�����ִ��ʱ�䳬��Ԥ�ڣ������ÿִ��һ�ξ�����Ļ���
/// ��Դ�����������Ӱ��,���Ҫ����һ��������������Ļ����������Ӱ�콵�͵����Խ��ܵĳ̶�
/// @code Sample:
///     ��ʼ�������
///		TimeTrace DecodeTimeTrace("DecodeTime", __FUNCTION__);
///		....
///		��¼����ʱ��
///		DecodeTimeTrace.SaveTime(dfDecodeTimespa);
///		if (DecodeTimeTrace.IsFull())
///			DecodeTimeTrace.OutputTime(0.005f);
struct TimeTrace
{
	char szName[128];
	char szFunction[128];
	double dfTimeArray[100];
	int	   nTimeCount;
	double dfInputTime;
	TimeTrace(char *szNameT, char *szFunctionT)
	{
		ZeroMemory(this, sizeof(TimeTrace));
		strcpy(szName, szNameT);
		strcpy(szFunction, szFunctionT);
	}
	void Zero()
	{
		ZeroMemory(this, sizeof(TimeTrace));
	}
	inline bool IsFull()
	{
		return (nTimeCount >= _MaxTimeCount);
	}
	inline void SaveTime()
	{
		if (dfInputTime != 0.0f)
			dfTimeArray[nTimeCount++] = GetExactTime() - dfInputTime;
		dfInputTime = GetExactTime();
	}
	inline void SaveTime(double dfTimeSpan)
	{
		dfTimeArray[nTimeCount++] = dfTimeSpan;
	}
	void OutputTime(float fMaxTime = 0.0f, bool bReset = true)
	{
		char szOutputText[1024] = { 0 };
		double dfAvg = 0.0f;
		int nCount = 0;
		if (nTimeCount < 1)
			return;
		TraceMsgA("%s %s Interval:\n", szFunction, szName);
		for (int i = 0; i < nTimeCount; i++)
		{
			if (fMaxTime > 0)
			{
				if (dfTimeArray[i] >= fMaxTime)
				{
					sprintf(&szOutputText[strlen(szOutputText)], "%.3f\t", dfTimeArray[i]);
					nCount++;
				}
			}
			else
			{
				sprintf(&szOutputText[strlen(szOutputText)], "%.3f\t", dfTimeArray[i]);
				nCount++;
			}

			dfAvg += dfTimeArray[i];
			if ((nCount + 1) % 25 == 0)
			{
				TraceMsgA("%s %s\n", szFunction, szOutputText);
				ZeroMemory(szOutputText, 1024);
			}
		}
		dfAvg /= nTimeCount;
		//if (dfAvg >= fMaxTime)
			TraceMsgA("%s Avg %s = %.6f.\n", szFunction, szName, dfAvg / nTimeCount);
		if (bReset)
			nTimeCount = 0;
	}
};
#endif

/// @brief  ��ͼ�࿪������FFMPEG�����֡�Ľ�ͼ,Ŀǰֻ֧��jpeg��bmp���ָ�ʽ�Ľ�ͼ
/// @code ��� CDvoPlayer::SnapShot(IN CHAR *szFileName, IN SNAPSHOT_FORMAT nFileFormat = XIFF_JPG)����
class CSnapshot
{
	AVFrame* pAvFrame;
	byte*	 pImage;
	int		 nImaageSize;
	int		 nVideoWidth;
	int		 nVideoHeight;
public:
	CSnapshot(AVPixelFormat nAvFormat, int nWidth, int nHeight)
	{
		ZeroMemory(this, sizeof(CSnapshot));
		pAvFrame = av_frame_alloc();
		int nImageSize = av_image_get_buffer_size(nAvFormat, nWidth, nHeight, 16);
		if (nImageSize < 0)
			return;
		pImage = (byte *)_aligned_malloc(nImageSize, 32);
		if (!pImage)
		{
			return;
		}
		// ����ʾͼ����YUV֡����
		av_image_fill_arrays(pAvFrame->data, pAvFrame->linesize, pImage, nAvFormat, nWidth, nHeight, 16);
		pAvFrame->width = nWidth;
		pAvFrame->height = nHeight;
		pAvFrame->format = nAvFormat;
	}
	~CSnapshot()
	{
		if (pImage)
		{
			_aligned_free(pImage);
			pImage = NULL;
		}
		av_free(pAvFrame);
		pAvFrame = NULL;
	}
	
	int CopyFrame(AVFrame *pFrame)
	{
		if (pFrame && pAvFrame)
		{
			av_frame_copy(pAvFrame, pFrame);
			av_frame_copy_props(pAvFrame, pFrame);
			return -1;
		}
		else
			return 0;
	}

	bool SaveJpeg(char *szJpegFile)
	{
		AVFormatContext* pJpegFormatCtx = nullptr;
		AVOutputFormat* fmt = nullptr;
		AVStream* JpegStream = nullptr;
		AVCodecContext* pJpegCodecCtx = nullptr;
		AVCodec* pCodec = nullptr;
		AVPacket pkt;
		ZeroMemory(&pkt, sizeof(AVPacket));
		int got_picture = 0;
		int nAvError = 0;
		char szAvError[1024];
		__try
		{
			pJpegFormatCtx = avformat_alloc_context();
			//Guess format
			fmt = av_guess_format("mjpeg", NULL, NULL);
			pJpegFormatCtx->oformat = fmt;
			//Output URL
			if (avio_open(&pJpegFormatCtx->pb, szJpegFile, AVIO_FLAG_READ_WRITE) < 0)
			{
				TraceMsg("Couldn't open output file.");
				__leave;
			}

			fmt = pJpegFormatCtx->oformat;
			JpegStream = avformat_new_stream(pJpegFormatCtx, 0);
			if (JpegStream == NULL){
				__leave;
			}
			pCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
			if (!pCodec)
			{
				TraceMsg("Codec not found.");
				__leave;
			}
			pJpegCodecCtx = avcodec_alloc_context3(pCodec);
			if (pJpegCodecCtx)
				avcodec_parameters_to_context(pJpegCodecCtx, JpegStream->codecpar);
			else
			{
				TraceMsg("%s avcodec_alloc_context3 Failed.\n", __FUNCTION__);
				__leave;
			}

			pJpegCodecCtx->codec_id		 = fmt->video_codec;
			pJpegCodecCtx->codec_type	 = AVMEDIA_TYPE_VIDEO;
			pJpegCodecCtx->pix_fmt		 = AV_PIX_FMT_YUVJ420P;//(AVPixelFormat)pAvFrame->format;
			pJpegCodecCtx->width		 = pAvFrame->width;
			pJpegCodecCtx->height		 = pAvFrame->height;
			pJpegCodecCtx->time_base.num = 1;
			pJpegCodecCtx->time_base.den = 25;
			//Output some information
			av_dump_format(pJpegFormatCtx, 0, szJpegFile, 1);

			if (nAvError = avcodec_open2(pJpegCodecCtx, pCodec, NULL) < 0)
			{
				av_strerror(nAvError, szAvError, 1024);
				TraceMsg("Could not open codec:%s.",szAvError);
				__leave;
			}
			//Write Header
			avformat_write_header(pJpegFormatCtx, NULL);

			int y_size = pJpegCodecCtx->width * pJpegCodecCtx->height;

			av_new_packet(&pkt, y_size * 3);
			//Encode
			int ret = avcodec_encode_video2(pJpegCodecCtx, &pkt, pAvFrame, &got_picture);
			if (ret < 0)
			{
				TraceMsg("Encode Error.\n");
				__leave;
			}
			if (got_picture == 1)
			{
				pkt.stream_index = JpegStream->index;
				ret = av_write_frame(pJpegFormatCtx, &pkt);
				av_write_trailer(pJpegFormatCtx);
			}
		}
		__finally
		{
			if (pkt.buf)
				av_packet_unref(&pkt);
			
			if (JpegStream)
				avcodec_parameters_free(&JpegStream->codecpar);
			if (pJpegCodecCtx)
				avcodec_close(pJpegCodecCtx);
			if (pJpegFormatCtx)
			{
				avio_close(pJpegFormatCtx->pb);
				avformat_free_context(pJpegFormatCtx);
			}
		}
		return true;
	}

	// 
	bool CreateBmp(const char *filename, uint8_t *pRGBBuffer, int width, int height, int bpp)
	{
		BITMAPFILEHEADER bmpheader;
		BITMAPINFOHEADER bmpinfo;
		FILE *fp = NULL;

		fp = fopen(filename, "wb");
		if (fp == NULL)
		{
			return false;
		}

		bmpheader.bfType = ('M' << 8) | 'B';
		bmpheader.bfReserved1 = 0;
		bmpheader.bfReserved2 = 0;
		bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		bmpheader.bfSize = bmpheader.bfOffBits + width*height*bpp / 8;

		bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
		bmpinfo.biWidth = width;
		bmpinfo.biHeight = 0 - height;
		bmpinfo.biPlanes = 1;
		bmpinfo.biBitCount = bpp;
		bmpinfo.biCompression = BI_RGB;
		bmpinfo.biSizeImage = 0;
		bmpinfo.biXPelsPerMeter = 100;
		bmpinfo.biYPelsPerMeter = 100;
		bmpinfo.biClrUsed = 0;
		bmpinfo.biClrImportant = 0;

		fwrite(&bmpheader, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bmpinfo, sizeof(BITMAPINFOHEADER), 1, fp);
		fwrite(pRGBBuffer, width*height*bpp / 8, 1, fp);
		fclose(fp);
		fp = NULL;
		return true;
	}

	bool SaveBmp(char *szBmpFile)
	{
		shared_ptr<PixelConvert> pVideoScale = make_shared< PixelConvert>(pAvFrame, D3DFMT_R8G8B8, GQ_BICUBIC);
		pVideoScale->ConvertPixel(pAvFrame);
		return CreateBmp(szBmpFile, (uint8_t *)pVideoScale->pImage, pAvFrame->width, pAvFrame->height, 24);
	}
};

/// @brief CDxSurface �ķ�װ�࣬���ڰ�CDxSurface�����ָ����ϵͳ�����еĴ�ȡ
struct DxSurfaceWrap
{
	DxSurfaceWrap(CDxSurface *pSurface)
	{
		dfInput = GetExactTime();
		pDxSurface = pSurface;
	}
	~DxSurfaceWrap()
	{
		if (pDxSurface)
			delete pDxSurface;
	}
	CDxSurface* Strip()		// ����CDxSurface����ָ��
	{
		if (pDxSurface)
		{
			CDxSurface *pTemp = pDxSurface;
			pDxSurface = nullptr;
			return pTemp;
		}
		else
		{
			assert(false);
			return nullptr;
		}
	}
	bool CompareSizeAndFormat(int nWidth, int nHeight, D3DFORMAT nPixFormat)
	{
		if (pDxSurface)
			return (MAKEUINT64(MAKELONG(nWidth, nHeight), nPixFormat) == pDxSurface->GetVideoSizeAndFormat());
		else
			return false;
	}
	double dfInput;
	CDxSurface	*pDxSurface;
};
//typedef shared_ptr<DxSurfaceWrap> DxSurfaceWrapPtr;

struct StreamFrame;
typedef shared_ptr<StreamFrame> StreamFramePtr;

/// @brief ���ڱ�����Ƶ�豸���Ե���
struct AudioPlayDevice
{
	GUID  *pGuid = nullptr;
	TCHAR *pszDevDesc = nullptr;
	TCHAR *pszDrvName = nullptr;
	AudioPlayDevice(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName)
	{
		if (lpGUID)
		{
			pGuid = new GUID;
			if (pGuid)
				memcpy(pGuid, lpGUID, sizeof(GUID));
		}
		pszDevDesc = new TCHAR[_tcslen(lpszDesc) + 1];
		if (!pszDevDesc)
			_tcscpy(pszDevDesc, lpszDesc);
		pszDrvName = new TCHAR[_tcslen(lpszDrvName) + 1];
		if (!pszDrvName)
			_tcscpy(pszDrvName, lpszDrvName);
	}
	~AudioPlayDevice()
	{
		if (pGuid)
			delete pGuid;
		if (pszDevDesc)
			delete[]pszDevDesc;
		if (pszDrvName)
			delete[]pszDrvName;
	}
};

/// @brief ��Ƶ�豸ö����
class CDSoundEnum
{
public:
	CDSoundEnum()
	{
		DirectSoundEnumerate((LPDSENUMCALLBACK)DSEnumProc, (VOID*)this);
	}
	~CDSoundEnum()
	{
	}
	static BOOL CALLBACK DSEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext)
	{
		CDSoundEnum *pThis = (CDSoundEnum *)lpContext;
		if (lpGUID)
			TraceMsg("%s Guid = {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\tDrvName = %s\tDesc = %s.\n",
					__FUNCTION__, 
					lpGUID->Data1, lpGUID->Data2, lpGUID->Data3, lpGUID->Data4[1], lpGUID->Data4[0], lpGUID->Data4[7], lpGUID->Data4[6], lpGUID->Data4[5], lpGUID->Data4[4], lpGUID->Data4[3], lpGUID->Data4[2], 
					lpszDrvName, lpszDesc);
		else
			TraceMsg("%s Guid = nullptr\tDrvName = %s\tDesc = %s.\n", __FUNCTION__,  lpszDrvName, lpszDesc);

		shared_ptr<AudioPlayDevice> pPlayDevice = make_shared<AudioPlayDevice>(lpGUID, lpszDesc, lpszDrvName);
		pThis->m_listPlayDev.push_back(pPlayDevice);
		return(TRUE);
	}
	int GetAudioPlayDevices()
	{
		return m_listPlayDev.size();
	}
private:
	list<shared_ptr<AudioPlayDevice>> m_listPlayDev;
};

/// @brief ���ܵ�һ�ļ򵥴����࣬������Ҫ���ڣ�ȴ���ܻ���Ҫ��ʾ���ڵĳ��ϣ���DirectX��DirectSound�ĳ�ʼ��
class  CSimpleWnd
{
public:
	CSimpleWnd(int nWidth = 0, int nHeight = 0)
	{
		m_hWnd = CreateSimpleWindow(nullptr,nWidth,nHeight);
		if (!m_hWnd)
		{
			m_hWnd = nullptr;
			assert(false);
		}
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
	HWND CreateSimpleWindow(IN HINSTANCE hInstance = NULL,int nWidth = 0,int nHeight = 0)
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
 		if (!nWidth || !nHeight)
 		{
			nWidth = GetSystemMetrics(SM_CXSCREEN);
			nHeight = GetSystemMetrics(SM_CYSCREEN);
		}
		TraceMsgA("%s nWidth = %d\tnHeight = %d", __FUNCTION__, nWidth, nHeight);
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
	/// @brief	�����ѷ�װ�õĴ�DVOFrameHeaderͷ��DVOFrameHeaderExͷ������
	StreamFrame(byte *pBuffer, int nLenth, INT nFrameInterval, int nSDKVersion = DVO_IPC_SDK_VERSION_2015_12_16)
	{
		assert(pBuffer != nullptr);
		assert(nLenth >= sizeof(DVOFrameHeader));
		nSize = nLenth;
		pInputData = (byte *)_aligned_malloc(nLenth, 16);
		//pInputData = _New byte[nLenth];
		if (pInputData)
			memcpy_s(pInputData,nLenth, pBuffer, nLenth);
		DVOFrameHeader* pHeader = (DVOFrameHeader *)pInputData;
		if (nSDKVersion >= DVO_IPC_SDK_VERSION_2015_12_16 && nSDKVersion != DVO_IPC_SDK_GSJ_HEADER)
			pHeader->nTimestamp = ((DVOFrameHeaderEx* )pHeader)->nFrameID*nFrameInterval * 1000;
	}
	/// @brief	�����������������ʵʱ����������
	StreamFrame(IN byte *pFrame, IN int nFrameType, IN int nFrameLength, int nFrameNum, time_t nFrameTime )
	{
		nSize = sizeof(DVOFrameHeaderEx) + nFrameLength;
		pInputData = (byte *)_aligned_malloc(nSize, 16);
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
#ifdef _DEBUG
		pHeader->dfRecvTime		 = GetExactTime();
#endif
		memcpy_s(pInputData + sizeof(DVOFrameHeaderEx),nFrameLength, pFrame, nFrameLength);
#ifdef _DEBUG
		//OutputMsg("%s pInputData = %08X size = %d.\n", __FUNCTION__, (long)pInputData,nSize);
#endif
	}

	inline const DVOFrameHeaderEx* FrameHeader()
	{
		return (DVOFrameHeaderEx*)pInputData;
	}
	inline const byte *Framedata(int nSDKVersion = DVO_IPC_SDK_VERSION_2015_12_16)
	{
		if (nSDKVersion >= DVO_IPC_SDK_VERSION_2015_12_16 && nSDKVersion != DVO_IPC_SDK_GSJ_HEADER)
			return (pInputData + sizeof(DVOFrameHeaderEx));
		else
			return (pInputData + sizeof(DVOFrameHeader));
	}
	~StreamFrame()
	{
		if (pInputData)
		{
			_aligned_free(pInputData);
			//delete[]pInputData;
		}
#ifdef _DEBUG
		//OutputMsg("%s pInputData = %08X.\n", __FUNCTION__, (long)pInputData);
		//ZeroMemory(this, sizeof(DVOFrameHeaderEx));
#endif
	}
	int		nSize;	
	byte	*pInputData;
	static bool IsIFrame(StreamFramePtr FramePtr)
	{
		//if (FramePtr->FrameHeader()->nType == FRAME_I)
		// TraceMsgA("%s Get an  Frame [%d].\n", __FUNCTION__, FramePtr->FrameHeader()->nType);
		return (FramePtr->FrameHeader()->nType == 0
				||FramePtr->FrameHeader()->nType == FRAME_IDR 
				|| FramePtr->FrameHeader()->nType == FRAME_I);
	}
};

#define _Frame_PERIOD			5.0f		///< һ��֡�ʼ�������

/// @brief ����DVO˽��֡�ṹ��
struct FrameParser
{
	union  
	{
		DVOFrameHeader		*pHeader;		///< DVO�ɰ�˽��¼���֡����
		DVOFrameHeaderEx	*pHeaderEx;		///< DVO�°�˽��¼���֡����
	};
	
	UINT				nFrameSize;		///< pFrame�����ݳ���
	byte				*pRawFrame;		///< ԭʼ��������
	UINT				nRawFrameSize;	///< ԭʼ�������ݳ���
};

/// @brief �ļ�֡��Ϣ�����ڱ�ʶÿһ֡���ļ���λ��
struct FileFrameInfo
{
	UINT	nOffset;	///< ֡�������ڵ��ļ�ƫ��
	UINT	nFrameSize;	///< ���� DVOFrameHeaderEx���ڵ�֡���ݵĳߴ�
	bool	bIFrame;	///< �Ƿ�I֡
	time_t	tTimeStamp;
};
// //���Կ���
// struct _DebugPin
// {
// 	bool bSwitch;
// 	double dfLastTime;
// 	_DebugPin()
// 	{
// 		bSwitch = true;
// 		dfLastTime = GetExactTime();
// 	}
// 
// };
// 
// struct _DebugSwitch
// {
// 	_DebugPin InputIpcStream;	
// 	_DebugPin DecodeSucceed;
// 	_DebugPin DecodeFailure;
// 	_DebugPin RenderSucceed;
// 	_DebugPin RenderFailure;
// };
// #ifdef _DEBUG
// #define SwitchTrace(pDebugSwitch,member) { if (pDebugSwitch && pDebugSwitch->##member##.bSwitch) if (TimeSpanEx(pDebugSwitch->##member##.dfLastTime) >= 5.0f) {	DxTraceMsg("%s %d %s.\n", __FUNCTION__, __LINE__,#member);	pDebugSwitch->##member##.dfLastTime = GetExactTime();}}
// #endif 
struct _OutputTime
{
	DWORD nDecode;
	DWORD nInputStream;
	DWORD nRender;
	DWORD nQueueSize;
	_OutputTime()
	{
		// �Զ���ʼ������DWORD�ͱ�������ʹ�������ӱ���Ҳ���������Ķ�
		DWORD *pThis = (DWORD *)this;
		for (int i = 0; i < sizeof(_OutputTime) / 4; i++)
			pThis[i] = timeGetTime();
		nInputStream = 0;
	}
};

/// @brief ��������̽�����
struct StreamProbe
{
	StreamProbe(int nBuffSize = 256 * 1024)
	{
		ZeroMemory(this, sizeof(StreamProbe));
		pProbeBuff = (byte *)_aligned_malloc(nBuffSize, 16);
		nProbeBuffSize = nBuffSize;
	}
	~StreamProbe()
	{
		_aligned_free(pProbeBuff);
	}
	bool GetProbeStream(byte *pStream, int nStreamLength)
	{
		if (!pStream || !nStreamLength)
			return false;
		if ((nProbeDataLength + nStreamLength) > nProbeBuffSize)
			return false;
		memcpy_s(&pProbeBuff[nProbeDataLength], nProbeBuffSize - nProbeDataLength, pStream, nStreamLength);
		nProbeDataLength += nStreamLength;
		nProbeDataRemained = nProbeDataLength;
		nProbeOffset = 0;
		return true;
	}

	byte		*pProbeBuff;			///< �����ؼ�̽�⻺����
	int			nProbeBuffSize;			///< ̽�⻺��ĳߴ�
	int			nProbeDataLength;		///< ̽�⻺�������ӵĳ���
	int			nProbeDataRemained;
	AVCodecID	nProbeAvCodecID;		///< ̽�⵽������ID
	int			nProbeWidth;
	int			nProbeHeight;
	int			nProbeCount;			///< ̽�����
	int			nProbeOffset;			///< ����̽��ʱ��Ƶ֡����ʱ��֡��ƫ��
};
extern volatile bool g_bThread_ClosePlayer/* = false*/;
extern list<DVO_PLAYHANDLE > g_listPlayertoFree;
extern CRITICAL_SECTION  g_csListPlayertoFree;
extern double	g_dfProcessLoadTime ;
extern CDxSurface* GetDxInCache(int nWidth, int nHeight, D3DFORMAT nPixFormat);
extern void PutDxCache(CDxSurface *pDxSurface);
extern CDxSurfaceEx* g_dx;
extern HWND g_hSnapShotWnd;

/// DVOIPCPlay SDK��Ҫ����ʵ����
class CDvoPlayer
{
public:
	int		nSize;
private:
	list<StreamFramePtr>m_listAudioCache;///< ������֡����
	list<StreamFramePtr>m_listVideoCache;///< ������֡����
	CRITICAL_SECTION	m_csVideoCache;
	CRITICAL_SECTION	m_csAudioCache;	
	int					m_nMaxFrameCache;///< �����Ƶ��������,Ĭ��ֵ100
		///< ��m_FrameCache�е���Ƶ֡��������m_nMaxFrameCacheʱ�����޷��ټ�����������
public:
	CHAR		m_szLogFileName[512];
#ifdef _DEBUG
	static CriticalSectionPtr m_pCSGlobalCount;
	int			m_nObjIndex;			///< ��ǰ����ļ���
	static int	m_nGloabalCount;		///< ��ǰ������CDvoPlayer��������
private:
	DWORD		m_nLifeTime;			///< ��ǰ�������ʱ��
	_OutputTime	m_OuputTime;
#endif
private:
	// ��Ƶ������ر���
	int			m_nTotalFrames;			///< ��ǰ�ļ�����Ч��Ƶ֡������,���������ļ�ʱ��Ч
	int			m_nTotalTime;			///< ��ǰ�ļ��в�����ʱ��,���������ļ�ʱ��Ч
	shared_ptr<StreamProbe>m_pStreamProbe;
	HWND		m_hWnd;					///< ������Ƶ�Ĵ��ھ��
	
	volatile bool m_bIpcStream;			///< ������ΪIPC��

	DVO_CODEC	m_nVideoCodec;			///< ��Ƶ�����ʽ @see DVO_CODEC	
	static		CAvRegister avRegister;	
	//static shared_ptr<CDSound> m_pDsPlayer;///< DirectSound���Ŷ���ָ��
	shared_ptr<CDSound> m_pDsPlayer;///< DirectSound���Ŷ���ָ��
	//shared_ptr<CDSound> m_pDsPlayer;	///< DirectSound���Ŷ���ָ��
	CDSoundBuffer* m_pDsBuffer;
	DxSurfaceInitInfo	m_DxInitInfo;
	CDxSurface* m_pDxSurface;			///< Direct3d Surface��װ��,������ʾ��Ƶ
	CDirectDraw *m_pDDraw;				///< DirectDraw��װ�����������xp����ʾ��Ƶ
	shared_ptr<ImageSpace> m_pYUVImage = NULL;
// 	bool		m_bDxReset;				///< �Ƿ�����DxSurface
// 	HWND		m_hDxReset;
// 	CRITICAL_SECTION m_csDxSurface;
	shared_ptr<CVideoDecoder>m_pDecodec;
	//static shared_ptr<CSimpleWnd>m_pWndDxInit;///< ��Ƶ��ʾʱ�����Գ�ʼ��DirectX�����ش��ڶ���
	bool		m_bRefreshWnd;			///< ֹͣ����ʱ�Ƿ�ˢ�»���
	int			m_nVideoWidth;			///< ��Ƶ���
	int			m_nVideoHeight;			///< ��Ƶ�߶�	
	AVPixelFormat	m_nDecodePirFmt;	///< ���������ظ�ʽ
	int			m_nFrameEplased;		///< �Ѿ�����֡��
	int			m_nCurVideoFrame;		///< ��ǰ�����ŵ���Ƶ֡ID
	time_t		m_tCurFrameTimeStamp;
	time_t		m_tLastFrameTime;
	USHORT		m_nPlayFPS;				///< ����ʱ֡��
	USHORT		m_nPlayFrameInterval;	///< ����ʱ֡���	
// 	int			*m_pSkipFramesArray;	///< ���ٲ���ʱҪ�õ�����֡��
// 	CRITICAL_SECTION	m_csSkipFramesArray;
	HANDLE		m_hEventDecodeStart;	///< ��Ƶ�����Ѿ���ʼ�¼�
	int			m_nSkipFrames;			///< ��֡���е�Ԫ������
	bool		m_bEnableD3DCache;		///< �Ƿ�����D3D����
	double		m_dfLastTimeVideoPlay;	///< ǰһ����Ƶ���ŵ�ʱ��
	double		m_dfTimesStart;			///< ��ʼ���ŵ�ʱ��
	bool		m_bEnableHaccel;		///< �Ƿ�����Ӳ����
	bool		m_bFitWindow;			///< ��Ƶ��ʾ�Ƿ���������
										///< Ϊtrueʱ�������Ƶ��������,�������ͼ������,���ܻ����ͼ�����
										///< Ϊfalseʱ����ֻ��ͼ��ԭʼ�����ڴ�������ʾ,������������,���Ժ�ɫ������ʾ
	CRITICAL_SECTION m_csBorderRect;
	shared_ptr<RECT>m_pBorderRect;		///< ��Ƶ��ʾ�ı߽磬�߽����ͼ������ʾ
private:	// ��Ƶ������ر���

	DVO_CODEC	m_nAudioCodec;			///< ��Ƶ�����ʽ @see DVO_CODEC
	bool		m_bEnableAudio;			///< �Ƿ�������Ƶ����
	DWORD		m_dwAudioOffset;		///< ��Ƶ�������е�ƫ�Ƶ�ַ
	HANDLE		m_hAudioFrameEvent[2];	///< ��Ƶ֡�Ѳ����¼�
	DWORD		m_dwAudioBuffLength;	///< 
	int			m_nNotifyNum;			///< ��Ƶ����������
	double		m_dfLastTimeAudioPlay;	///< ǰһ�β�����Ƶ��ʱ��
	double		m_dfLastTimeAudioSample;///< ǰһ����Ƶ������ʱ��
	int			m_nAudioFrames;			///< ��ǰ��������Ƶ֡����
	int			m_nCurAudioFrame;		///< ��ǰ�����ŵ���Ƶ֡ID
	
	static shared_ptr<CDSoundEnum> m_pDsoundEnum;	///< ��Ƶ�豸ö����
	static CriticalSectionPtr m_csDsoundEnum;
private:
	HANDLE		m_hThreadParser;		///< ����DVO˽�и�ʽ¼����߳�
	HANDLE		m_hThreadPlayVideo;		///< ��Ƶ����Ͳ����߳�
	HANDLE		m_hThreadPlayAudio;		///< ��Ƶ����Ͳ����߳�
	HANDLE		m_hThreadGetFileSummary;///< �ļ���ϢժҪ�߳�
	UINT		m_nVideoCache;
	UINT		m_nAudioCache;
	HANDLE		m_hCacheFulled;			///< �����ڸ�Ҫ�̱߳��������¼�
	UINT		m_nHeaderFrameID;		///< �����е�1֡��ID
	UINT		m_nTailFrameID;			///< ���������һ֡��ID
	bool		m_bThreadSummaryRun;
	bool		m_bSummaryIsReady;		///< �ļ�ժҪ��Ϣ׼�����
	bool		m_bStopFlag;			///< ������ֹͣ��־�����ٽ�������
	volatile bool m_bThreadParserRun;
	volatile bool m_bThreadPlayVideoRun;
	volatile bool m_bThreadPlayAudioRun;
	byte*		m_pParserBuffer;		///< ���ݽ���������
	UINT		m_nParserBufferSize;	///< ���ݽ����������ߴ�
	DWORD		m_nParserDataLength;	///< ���ݽ����������е���Ч���ݳ���
	UINT		m_nParserOffset;		///< ���ݽ����������ߴ統ǰ�Ѿ�������ƫ��
	CRITICAL_SECTION m_csParser;		
	//volatile bool m_bThreadFileAbstractRun;
	bool		m_bPause;				///< �Ƿ�����ͣ״̬
	byte		*m_pYUV;				///< YVU��׽ר���ڴ�
	int			m_nYUVSize ;			///<
	shared_ptr<byte>m_pYUVPtr;
	// ��ͼ������ؾ����
	HANDLE		m_hEvnetYUVReady;		///< YUV���ݾ����¼�
	HANDLE		m_hEventYUVRequire;		///< YUV���������¼�,�����ѵ�ǰ����֡���и���m_pAvFrameSnapshot
	HANDLE		m_hEventFrameCopied;		///< ��ͼ��������ɸ����¼�
	shared_ptr<CSnapshot>m_pSnapshot;
	
private:	// �ļ�������ر���
	HANDLE		m_hDvoFile;				///< ���ڲ��ŵ��ļ����
	INT64		m_nSummaryOffset;		///< �ڶ�ȡ��Ҫʱ��õ��ļ�����ƫ��
#ifdef _DEBUG
	bool		m_bSeekSetDetected = false;///< �Ƿ��������֡����
#endif
	shared_ptr<DVO_MEDIAINFO>m_pMediaHeader;/// ý���ļ�ͷ
	long		m_nSDKVersion;
	DVOFrameHeader m_FirstFrame, m_LastFrame;
	UINT		m_nFrametoRead;			///< ��ǰ��Ҫ��ȡ����Ƶ֡ID
	char		*m_pszFileName;			///< ���ڲ��ŵ��ļ���,���ֶν����ļ�����ʱ����Ч
	FileFrameInfo	*m_pFrameOffsetTable;///< ��Ƶ֡ID��Ӧ�ļ�ƫ�Ʊ�
	volatile LONGLONG m_nSeekOffset;	///< ���ļ���ƫ��
	CRITICAL_SECTION	m_csSeekOffset;
	float		m_fPlayRate;			///< ��ǰ�Ĳ��ŵı���,����1ʱΪ���ٲ���,С��1ʱΪ���ٲ��ţ�����Ϊ0��С��0
	int			m_nMaxFrameSize;		///< ���I֡�Ĵ�С�����ֽ�Ϊ��λ,Ĭ��ֵ256K
	int			m_nFileFPS;				///< �ļ���,��Ƶ֡��ԭʼ֡��
	USHORT		m_nFileFrameInterval;	///< �ļ���,��Ƶ֡��ԭʼ֡���
	float		m_fPlayInterval;		///< ֡���ż��,��λ����
private:
// 	CaptureFrame	m_pfnCaptureFrame;
// 	void*			m_pUserCaptureFrame;
	CaptureYUV		m_pfnCaptureYUV;
	void*			m_pUserCaptureYUV;
	CaptureYUVEx	m_pfnCaptureYUVEx;
	void*			m_pUserCaptureYUVEx;
	FilePlayProc	m_pFilePlayCallBack;
	void*			m_pUserFilePlayer;
	CaptureYUVEx	m_pfnYUVFilter;
	void*			m_pUserYUVFilter;
private:
	shared_ptr<CRunlog> m_pRunlog;	///< ������־
#define __countof(array) (sizeof(array)/sizeof(array[0]))
#pragma warning (disable:4996)
	void OutputMsg(char *pFormat, ...)
	{
		int nBuff;
		CHAR szBuffer[4096];
		va_list args;
		va_start(args, pFormat);		
		nBuff = _vsnprintf(szBuffer, __countof(szBuffer), pFormat, args);
		//::wvsprintf(szBuffer, pFormat, args);
		//assert(nBuff >=0);
#ifdef _DEBUG
		OutputDebugStringA(szBuffer);
#endif
		if (m_pRunlog)
			m_pRunlog->Runlog(szBuffer);
		va_end(args);
	}
public:
	
private:
	CDvoPlayer()
	{
		ZeroMemory(&m_csVideoCache, sizeof(CDvoPlayer) - offsetof(CDvoPlayer, m_csVideoCache));
		InitializeCriticalSection(&m_csVideoCache);
		InitializeCriticalSection(&m_csAudioCache);
		InitializeCriticalSection(&m_csParser);
		InitializeCriticalSection(&m_csBorderRect);
		m_nMaxFrameSize = 1024 * 256;
		nSize = sizeof(CDvoPlayer);
	}
	LONGLONG GetSeekOffset()
	{
		CAutoLock lock(&m_csSeekOffset, false,__FILE__, __FUNCTION__, __LINE__);
		return m_nSeekOffset;
	}
	void SetSeekOffset(LONGLONG nSeekOffset)
	{
		CAutoLock lock(&m_csSeekOffset, false, __FILE__, __FUNCTION__, __LINE__);
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
	static bool IsDVOVideoFrame(DVOFrameHeader *pFrameHeader,bool &bIFrame,int nSDKVersion)
	{
		bIFrame = false;
		if (nSDKVersion >= DVO_IPC_SDK_VERSION_2015_12_16 && nSDKVersion != DVO_IPC_SDK_GSJ_HEADER)
		{
			switch (pFrameHeader->nType)
			{
			case FRAME_P:		// BP֡������࣬����ǰ�ã��Լ��ٱȽϴ���
			case FRAME_B:
				return true;
			case 0:
			case FRAME_IDR:
			case FRAME_I:
				bIFrame = true;
				return true;
			default:
				return false;
			}
		}
		else
		{
			switch (pFrameHeader->nType)
			{// �ɰ�SDK�У�0Ϊbbp֡ ,1ΪI֡ ,2Ϊ��Ƶ֡
			case 0:
				return true;
				break;
			case 1:
				bIFrame = true;
				return true;
				break;
			default:
				return false;
				break;
			}
		}
	}

	/// @brief ȡ����Ƶ�ļ���һ��I֡�����һ����Ƶ֡ʱ��
	/// @param[out]	֡ʱ��
	/// @param[in]	�Ƿ�ȡ��һ��I֡ʱ�䣬��Ϊtrue��ȡ��һ��I֡��ʱ�����ȡ��һ����Ƶ֡ʱ��
	/// @remark �ú���ֻ���ڴӾɰ��DVO¼���ļ���ȡ�õ�һ֡�����һ֡
	int GetFrame(DVOFrameHeader *pFrame, bool bFirstFrame = true)
	{
		if (!m_hDvoFile)
			return DVO_Error_FileNotOpened;
		LARGE_INTEGER liFileSize;
		if (!GetFileSizeEx(m_hDvoFile, &liFileSize))
			return GetLastError();
		byte *pBuffer = _New byte[m_nMaxFrameSize];
		shared_ptr<byte>TempBuffPtr(pBuffer);
		DWORD nMoveMothod = FILE_BEGIN;
		__int64 nMoveOffset = sizeof(DVO_MEDIAINFO);

		if (!bFirstFrame)
		{
			nMoveMothod = FILE_END;
			nMoveOffset = -m_nMaxFrameSize;
		}
		
		if (liFileSize.QuadPart >= m_nMaxFrameSize &&
			LargerFileSeek(m_hDvoFile, nMoveOffset, nMoveMothod) == INVALID_SET_FILE_POINTER)
			return GetLastError();
		DWORD nBytesRead = 0;
		DWORD nDataLength = m_nMaxFrameSize;

		if (!ReadFile(m_hDvoFile, pBuffer, nDataLength, &nBytesRead, nullptr))
		{
			OutputMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
			return GetLastError();
		}
		nDataLength = nBytesRead;
		char *szKey1 = "MOVD";
		char *szKey2 = "IMWH";
		int nOffset = KMP_StrFind(pBuffer, nDataLength, (byte *)szKey1, 4);
		if (nOffset < 0)
		{
			nOffset = KMP_StrFind(pBuffer, nDataLength, (byte *)szKey2, 4);
			if (nOffset < 0)
				return DVO_Error_MaxFrameSizeNotEnough;
		}
		nOffset -= offsetof(DVOFrameHeader, nFrameTag);	// ���˵�֡ͷ
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
			switch (Parser.pHeader->nType)
			{
			case 0:
			case 1:
			{
				nVideoFrames++;
				bFoundVideo = true;
				break;
			}
			case 2:      // G711 A�ɱ���֡
			case FRAME_G711U:      // G711 U�ɱ���֡
			{
				nAudioFrames++;
				break;
			}
			default:
			{
				assert(false);
				break;
			}
			}
			if (bFoundVideo && bFirstFrame)
				break;
			
			nOffset += Parser.nFrameSize;
		}
		OutputMsg("%s In Last %d bytes:VideoFrames:%d\tAudioFrames:%d.\n", __FUNCTION__, m_nMaxFrameSize, nVideoFrames, nAudioFrames);
#else
		while (ParserFrame(&pBuffer, nDataLength, &Parser))
		{
			if (Parser.pHeaderEx->nType == 0 ||
				Parser.pHeaderEx->nType == 1 )
			{
				bFoundVideo = true;
			}
			if (bFoundVideo && bFirstFrame)
			{
				break;
			}
			nOffset += Parser.nFrameSize;
		}
#endif
		if (SetFilePointer(m_hDvoFile, sizeof(DVO_MEDIAINFO), nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			return GetLastError();
		if (bFoundVideo)
		{
			memcpy_s(pFrame, sizeof(DVOFrameHeader), Parser.pHeader, sizeof(DVOFrameHeader));
			return DVO_Succeed;
		}
		else
			return DVO_Error_MaxFrameSizeNotEnough;
	}
	/// @brief ȡ����Ƶ�ļ�֡��ID��ʱ��,�൱����Ƶ�ļ��а�������Ƶ��֡��
	int GetLastFrameID(int &nLastFrameID)
	{
		if (!m_hDvoFile)
			return DVO_Error_FileNotOpened;		
		LARGE_INTEGER liFileSize;
		if (!GetFileSizeEx(m_hDvoFile, &liFileSize))
			return GetLastError();
		byte *pBuffer = _New byte[m_nMaxFrameSize];
		shared_ptr<byte>TempBuffPtr(pBuffer);
				
		if (liFileSize.QuadPart >= m_nMaxFrameSize && 
			LargerFileSeek(m_hDvoFile, -m_nMaxFrameSize, FILE_END) == INVALID_SET_FILE_POINTER)
			return GetLastError();
		DWORD nBytesRead = 0;
		DWORD nDataLength = m_nMaxFrameSize;
		
		if (!ReadFile(m_hDvoFile, pBuffer, nDataLength, &nBytesRead, nullptr))
		{
			OutputMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
			return GetLastError();
		}
		nDataLength = nBytesRead;
		char *szKey1 = "MOVD";		// �°��DVO¼���ļ�ͷ
		char *szKey2 = "IMWH";		// �ϰ汾��DVO¼���ļ���ʹ���˸��ӽݵ��ļ�ͷ
		int nOffset = KMP_StrFind(pBuffer, nDataLength, (byte *)szKey1, 4);
		if (nOffset < 0)
		{
			nOffset = KMP_StrFind(pBuffer, nDataLength, (byte *)szKey2, 4);
			if (nOffset < 0)
				return DVO_Error_MaxFrameSizeNotEnough;
		}
		nOffset -= offsetof(DVOFrameHeader, nFrameTag);	// ���˵�֡ͷ
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
			case FRAME_IDR:
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
		OutputMsg("%s In Last %d bytes:VideoFrames:%d\tAudioFrames:%d.\n", __FUNCTION__, m_nMaxFrameSize, nVideoFrames, nAudioFrames);
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
			nOffset += Parser.nFrameSize;
		}
#endif
		if (SetFilePointer(m_hDvoFile, sizeof(DVO_MEDIAINFO), nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			return GetLastError();
		if (bFoundVideo)
			return DVO_Succeed;
		else
			return DVO_Error_MaxFrameSizeNotEnough;
	}

	
	bool InitizlizeDisplay()
	{
		// ��ʼ��ʾ���
		if (GetOsMajorVersion() < 6)
		{
			if (m_pDDraw)
			{
				//����DirectDraw����  
				DDSURFACEDESC2 ddsd = { 0 };
				FormatYV12::Build(ddsd, m_nVideoWidth, m_nVideoHeight);
				//m_pDDraw = _New CDirectDraw>;
				m_pDDraw->Create<FormatYV12>(m_hWnd, ddsd);
				m_pYUVImage = make_shared<ImageSpace>();
				m_pYUVImage->dwLineSize[0] = m_nVideoWidth;
				m_pYUVImage->dwLineSize[1] = m_nVideoWidth >> 1;
				m_pYUVImage->dwLineSize[2] = m_nVideoWidth >> 1;
			}
			return true;
		}
		else
		{
			// ʹ���߳���CDxSurface������ʾͼ��
			if (m_pDxSurface)		// D3D�豸��δ��ʼ��
			{
				DxSurfaceInitInfo &InitInfo = m_DxInitInfo;
				InitInfo.nSize = sizeof(DxSurfaceInitInfo);
				if (m_bEnableHaccel)
				{
					AVCodecID nCodecID = AV_CODEC_ID_H264;
					if (m_nVideoCodec == CODEC_H265)
						nCodecID = AV_CODEC_ID_HEVC;
					InitInfo.nFrameWidth = CVideoDecoder::GetAlignedDimension(nCodecID,m_nVideoWidth);
					InitInfo.nFrameHeight = CVideoDecoder::GetAlignedDimension(nCodecID,m_nVideoHeight);
					InitInfo.nD3DFormat = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
				}
				else
				{
					///if (GetOsMajorVersion() < 6)
					///	InitInfo.nD3DFormat = D3DFMT_A8R8G8B8;		// ��XP�����£�ĳЩ������ʾ����ʾYV12����ʱ���ᵼ��CPUռ���ʻ����������������D3D9��ü�����ʾ��һ��BUG
					///else
					InitInfo.nFrameWidth = m_nVideoWidth;
					InitInfo.nFrameHeight = m_nVideoHeight;
					InitInfo.nD3DFormat = (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2');
				}
				InitInfo.bWindowed = TRUE;
				//if (!pWndDxInit->GetSafeHwnd())
				InitInfo.hPresentWnd = m_hWnd;
				//else
				//	InitInfo.hPresentWnd = pWndDxInit->GetSafeHwnd();
				m_pDxSurface->DisableVsync();		// ���ô�ֱͬ��������֡���п��ܳ�����ʾ����ˢ���ʣ��Ӷ��ﵽ���ٲ��ŵ�Ŀ��
				if (!m_pDxSurface->InitD3D(InitInfo.hPresentWnd,
					InitInfo.nFrameWidth,
					InitInfo.nFrameHeight,
					InitInfo.bWindowed,
					InitInfo.nD3DFormat))
				{
					OutputMsg("%s Initialize DxSurface failed.\n", __FUNCTION__);
#ifdef _DEBUG
					OutputMsg("%s \tObject:%d DxSurface failed,Line %d Time = %d.\n", __FUNCTION__, m_nObjIndex, __LINE__, timeGetTime() - m_nLifeTime);
#endif
					return false;
				}
				return true;
			}
			else
				return false;
		}
	}
#ifdef _DEBUG
	int m_nRenderFPS = 0;
	int m_nRenderFrames = 0;
	double dfTRender = 0.0f;
#endif
	/// @brief ��Ⱦһ֡
	void RenderFrame(AVFrame *pAvFrame)
	{
		if (m_bFitWindow)
		{
			if (m_pDxSurface)
			{
				RECT rtBorder;
				CAutoLock lock(&m_csBorderRect);
				if (m_pBorderRect)
				{
					CopyRect(&rtBorder, m_pBorderRect.get());
					lock.Unlock();
					m_pDxSurface->Render(pAvFrame, m_hWnd, &rtBorder);
				}
				else
					m_pDxSurface->Render(pAvFrame, m_hWnd);
			}
			else if (m_pDDraw)
			{
				m_pYUVImage->pBuffer[0] = (PBYTE)pAvFrame->data[0];
				m_pYUVImage->pBuffer[1] = (PBYTE)pAvFrame->data[1];
				m_pYUVImage->pBuffer[2] = (PBYTE)pAvFrame->data[2];
				m_pYUVImage->dwLineSize[0] = pAvFrame->linesize[0];
				m_pYUVImage->dwLineSize[1] = pAvFrame->linesize[1];
				m_pYUVImage->dwLineSize[2] = pAvFrame->linesize[2];
				RECT rtBorder;
				CAutoLock lock(&m_csBorderRect);
				if (m_pBorderRect)
				{
					CopyRect(&rtBorder, m_pBorderRect.get());
					lock.Unlock();
					m_pDDraw->Draw(*m_pYUVImage, &rtBorder, nullptr, true);
				}
				else
					m_pDDraw->Draw(*m_pYUVImage,nullptr,nullptr,true);
			}
		}
		else
		{
			RECT rtRender;
			GetWindowRect(m_hWnd, &rtRender);
			int nWndWidth = rtRender.right - rtRender.left;
			int nWndHeight = rtRender.bottom - rtRender.top;
			float fScaleWnd = (float)nWndHeight / nWndWidth;
			float fScaleVideo = (float)pAvFrame->height / pAvFrame->width;
			if (fScaleVideo < fScaleWnd)			// ���ڸ߶ȳ�������,��Ҫȥ��һ���ָ߶�,��Ƶ��Ҫ���¾���
			{
				int nNewHeight = (int)nWndWidth * fScaleVideo;
				int nOverHeight = nWndHeight - nNewHeight;
				if ((float)nOverHeight / nWndHeight > 0.01f)	// ���ڸ߶ȳ���1%,������߶ȣ�������Ըò���
				{
					rtRender.top += nOverHeight / 2;
					rtRender.bottom -= nOverHeight / 2;
				}
			}
			else if (fScaleVideo > fScaleWnd)		// ���ڿ�ȳ�������,��Ҫȥ��һ���ֿ�ȣ���Ƶ��Ҫ���Ҿ���
			{
				int nNewWidth = nWndHeight/fScaleVideo;
				int nOverWidth = nWndWidth - nNewWidth;
				if ((float)nOverWidth / nWndWidth > 0.01f)
				{
					rtRender.left += nOverWidth / 2;
					rtRender.right -= nOverWidth / 2;
				}
			}
			if (m_pDxSurface)
			{
				ScreenToClient(m_hWnd, (LPPOINT)&rtRender);
				ScreenToClient(m_hWnd, ((LPPOINT)&rtRender) + 1);
				RECT rtBorder;
				CAutoLock lock(&m_csBorderRect);
				if (m_pBorderRect)
				{
					CopyRect(&rtBorder, m_pBorderRect.get());
					lock.Unlock();
					m_pDxSurface->Render(pAvFrame, m_hWnd, &rtBorder,&rtRender);
				}
				else
					m_pDxSurface->Render(pAvFrame, m_hWnd, nullptr, &rtRender);
			}
			else if (m_pDDraw)
			{
				m_pYUVImage->pBuffer[0] = (PBYTE)pAvFrame->data[0];
				m_pYUVImage->pBuffer[1] = (PBYTE)pAvFrame->data[1];
				m_pYUVImage->pBuffer[2] = (PBYTE)pAvFrame->data[2];
				m_pYUVImage->dwLineSize[0] = pAvFrame->linesize[0];
				m_pYUVImage->dwLineSize[1] = pAvFrame->linesize[1];
				m_pYUVImage->dwLineSize[2] = pAvFrame->linesize[2];
				RECT rtBorder;
				CAutoLock lock(&m_csBorderRect);
				if (m_pBorderRect)
				{
					CopyRect(&rtBorder, m_pBorderRect.get());
					lock.Unlock();
					m_pDDraw->Draw(*m_pYUVImage, &rtBorder, &rtRender, true);
				}
				else
					m_pDDraw->Draw(*m_pYUVImage,nullptr, &rtRender, true);
			}
		}
	}
	// ���ֲ���
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
			//������
			else if (tTime < m_pFrameOffsetTable[middle].tTimeStamp)
				high = middle - 1;
			//���Ұ��
			else if (tTime > m_pFrameOffsetTable[middle + 1].tTimeStamp)
				low = middle + 1;
		}
		//û�ҵ�
		return -1;
	}
public:
	/// @brief  ������־
	/// @param	szLogFile		��־�ļ���,���ò���Ϊnull���������־
	void EnableRunlog(const char *szLogFile)
	{
		char szFileLog[MAX_PATH] = { 0 };
		if (szLogFile && strlen(szLogFile) != 0)
		{
			strcpy(szFileLog, szLogFile);
			m_pRunlog = make_shared<CRunlog>(szFileLog);
		}
		else
			m_pRunlog = nullptr;
	}

	void ClearOnException()
	{
		if (m_pszFileName)
		{
			delete[]m_pszFileName;
			m_pszFileName = nullptr;
		}
		if (m_pRunlog)
			m_pRunlog = nullptr;
		DeleteCriticalSection(&m_csVideoCache);
		DeleteCriticalSection(&m_csAudioCache);
		DeleteCriticalSection(&m_csSeekOffset);
		DeleteCriticalSection(&m_csParser);
		DeleteCriticalSection(&m_csBorderRect);
		if (m_hDvoFile)
			CloseHandle(m_hDvoFile);
	}
	CDvoPlayer(HWND hWnd, CHAR *szFileName = nullptr, char *szLogFile = nullptr) 
	{
		ZeroMemory(&m_csVideoCache, sizeof(CDvoPlayer) - offsetof(CDvoPlayer, m_csVideoCache));
#ifdef _DEBUG
		m_pCSGlobalCount->Lock();
		m_nObjIndex = m_nGloabalCount++;
		m_pCSGlobalCount->Unlock();
		m_nLifeTime = timeGetTime();
		
// 		m_OuputTime.nDecode = m_nLifeTime;
// 		m_OuputTime.nInputStream = m_nLifeTime;
// 		m_OuputTime.nRender = m_nLifeTime;

		OutputMsg("%s \tObject:%d m_nLifeTime = %d.\n", __FUNCTION__, m_nObjIndex, m_nLifeTime);
#endif 
		m_nSDKVersion = DVO_IPC_SDK_VERSION_2015_12_16;
		if (szLogFile)
		{
			strcpy(m_szLogFileName, szLogFile);
			m_pRunlog = make_shared<CRunlogA>(szLogFile);
		}
		m_hEvnetYUVReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		m_hEventDecodeStart = CreateEvent(nullptr, TRUE, FALSE, nullptr);
 		m_hEventYUVRequire = CreateEvent(nullptr, FALSE, FALSE, nullptr);
 		m_hEventFrameCopied = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		InitializeCriticalSection(&m_csVideoCache);
		InitializeCriticalSection(&m_csAudioCache);
		InitializeCriticalSection(&m_csSeekOffset);
		InitializeCriticalSection(&m_csParser);
		InitializeCriticalSection(&m_csBorderRect);
		m_csDsoundEnum->Lock();
		if (!m_pDsoundEnum)
			m_pDsoundEnum = make_shared<CDSoundEnum>();	///< ��Ƶ�豸ö����
		m_csDsoundEnum->Unlock();
		
#ifdef _DEBUG
		OutputMsg("%s Alloc a \tObject:%d.\n", __FUNCTION__, m_nObjIndex);
#endif
		nSize = sizeof(CDvoPlayer);
		m_nMaxFrameSize	 = 1024 * 256;
		m_nFileFPS		 = 25;				// FPS��Ĭ��ֵΪ25
		m_fPlayRate		 = 1;
		m_fPlayInterval	 = 40.0f;
		//m_nVideoCodec	 = CODEC_H264;		// ��ƵĬ��ʹ��H.264����
		m_nVideoCodec = CODEC_UNKNOWN;
		m_nAudioCodec	 = CODEC_G711U;		// ��ƵĬ��ʹ��G711U����
//#ifdef _DEBUG
//		m_nMaxFrameCache = 200;				// Ĭ�������Ƶ��������Ϊ200
// #else
 		m_nMaxFrameCache = 100;				// Ĭ�������Ƶ��������Ϊ100
// #endif
		if (szFileName)
		{
			m_pszFileName = _New char[MAX_PATH];
			strcpy(m_pszFileName, szFileName);
			// ���ļ�
			m_hDvoFile = CreateFileA(m_pszFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_ARCHIVE,
				NULL);
			if (m_hDvoFile != INVALID_HANDLE_VALUE)
			{
				int nError = GetFileHeader();
				if (nError != DVO_Succeed)
				{
					OutputMsg("%s %d(%s):Not a DVO Media File.\n", __FILE__, __LINE__, __FUNCTION__);
					ClearOnException();
					throw std::exception("Not a DVO Media File.");
				}
				
				// GetLastFrameIDȡ�õ������һ֡��ID����֡����Ҫ�ڴ˻�����+1
				
				if (m_pMediaHeader)
				{
					m_nSDKVersion = m_pMediaHeader->nSDKversion;
					switch (m_nSDKVersion)
					{
					case DVO_IPC_SDK_VERSION_2015_09_07:
					case DVO_IPC_SDK_VERSION_2015_10_20:
					case DVO_IPC_SDK_GSJ_HEADER:
					{
						m_nFileFPS = 25;
						m_nVideoCodec = CODEC_UNKNOWN;
						m_nVideoWidth = 0;
						m_nVideoHeight = 0;
						
						// ȡ�õ�һ֡�����һ֡����Ϣ
						if (GetFrame(&m_FirstFrame, true) != DVO_Succeed || 
							GetFrame(&m_LastFrame, false) != DVO_Succeed)
						{
							OutputMsg("%s %d(%s):Can't get the First or Last.\n", __FILE__, __LINE__, __FUNCTION__);
							ClearOnException();
							throw std::exception("Can't get the First or Last.");
						}
						// ȡ���ļ���ʱ��(ms)
						__int64 nTotalTime = 0;
						__int64 nTotalTime2 = 0;
						if (m_pMediaHeader->nCameraType == 1)	// ��Ѷʿ���
						{
							nTotalTime = (m_LastFrame.nFrameUTCTime - m_FirstFrame.nFrameUTCTime) * 100;
							nTotalTime2 = (m_LastFrame.nTimestamp - m_FirstFrame.nTimestamp) / 10000;
						}
						else
						{
							nTotalTime = (m_LastFrame.nFrameUTCTime - m_FirstFrame.nFrameUTCTime) * 1000;
							nTotalTime2 = (m_LastFrame.nTimestamp - m_FirstFrame.nTimestamp) / 1000;
						}
						if (nTotalTime < 0)
						{
							OutputMsg("%s %d(%s):The Frame timestamp is invalid.\n", __FILE__, __LINE__, __FUNCTION__);
							ClearOnException();
							throw std::exception("The Frame timestamp is invalid.");
						}
						if (nTotalTime2 > 0)
						{
							m_nTotalTime = nTotalTime2;
							// ������ʱ��Ԥ����֡��
							m_nTotalFrames = m_nTotalTime / 40;		// �ϰ��ļ�ʹ�ù̶�֡��,ÿ֡���Ϊ40ms
							m_nTotalFrames += 25;
						}
						else if (nTotalTime > 0)
						{
							m_nTotalTime = nTotalTime;
							// ������ʱ��Ԥ����֡��
							m_nTotalFrames = m_nTotalTime / 40;		// �ϰ��ļ�ʹ�ù̶�֡��,ÿ֡���Ϊ40ms
							m_nTotalFrames += 50;
						}
						else
						{
							OutputMsg("%s %d(%s):Frame timestamp error.\n", __FILE__, __LINE__, __FUNCTION__);
							ClearOnException();
							throw std::exception("Frame timestamp error.");
						}
						break;
					}
					
					case DVO_IPC_SDK_VERSION_2015_12_16:
					{
						int nDvoError = GetLastFrameID(m_nTotalFrames);
						if (nDvoError != DVO_Succeed)
						{
							OutputMsg("%s %d(%s):Can't get last FrameID .\n", __FILE__, __LINE__, __FUNCTION__);
							ClearOnException();
							throw std::exception("Can't get last FrameID.");
						}
						m_nTotalFrames++;
						m_nVideoCodec = m_pMediaHeader->nVideoCodec;
						m_nAudioCodec = m_pMediaHeader->nAudioCodec;
						if (m_nVideoCodec == CODEC_UNKNOWN)
						{
							m_nVideoWidth = 0;
							m_nVideoHeight = 0;
						}
						else
						{
							m_nVideoWidth = m_pMediaHeader->nVideoWidth;
							m_nVideoHeight = m_pMediaHeader->nVideoHeight;
							if (!m_nVideoWidth || !m_nVideoHeight)
							{
// 								OutputMsg("%s %d(%s):Invalid Mdeia File Header.\n", __FILE__, __LINE__, __FUNCTION__);
// 								ClearOnException();
// 								throw std::exception("Invalid Mdeia File Header.");
								m_nVideoCodec = CODEC_UNKNOWN;
							}
						}
						if (m_pMediaHeader->nFps == 0)
							m_nFileFPS = 25;
						else
							m_nFileFPS = m_pMediaHeader->nFps;
					}
					break;
					default:
					{
						OutputMsg("%s %d(%s):Invalid SDK Version.\n", __FILE__, __LINE__, __FUNCTION__);
						ClearOnException();
						throw std::exception("Invalid SDK Version.");
					}
					}
					m_nFileFrameInterval = 1000 / m_nFileFPS;
				}
				m_hCacheFulled = CreateEvent(nullptr, true, false, nullptr);
				m_bThreadSummaryRun = true;
				m_hThreadGetFileSummary = (HANDLE)_beginthreadex(nullptr, 0, ThreadGetFileSummary, this, 0, 0);
				if (!m_hThreadGetFileSummary)
				{
					OutputMsg("%s %d(%s):_beginthreadex ThreadGetFileSummary Failed.\n", __FILE__, __LINE__, __FUNCTION__);
					ClearOnException();
					throw std::exception("_beginthreadex ThreadGetFileSummary Failed.");
				}
// ȡ���ļ��ĸ�Ҫ����Ϣ,���ļ���֡��,�ļ�ƫ�Ʊ�
// 				int nError = GetFileSummary();
// 				if (nError != DVO_Succeed)
// 				{
// 					OutputMsg("%s %d(%s):Not a DVO Media File.\n", __FILE__,__LINE__,__FUNCTION__);
// 					ClearOnException();
// 					throw std::exception("Not a DVO Media File.");
// 				}

				m_nParserBufferSize	 = m_nMaxFrameSize * 4;
				m_pParserBuffer = (byte *) _aligned_malloc(m_nParserBufferSize,16);
			}
			else
			{
				OutputMsg("%s %d(%s):Open file failed.\n", __FILE__, __LINE__, __FUNCTION__);
				ClearOnException();
				throw std::exception("Open file failed.");
			}
		}
		if (hWnd && IsWindow(hWnd))
		{// �ز�Ҫ�д��ڲŻᲥ����Ƶ������
			m_hWnd = hWnd;
			if (TimeSpanEx(g_dfProcessLoadTime) < 15)
			{// ��������15���ڣ����ӻ�����ȡCDxSurface ����
				//if (IsWindowsVistaOrGreater())
				if (GetOsMajorVersion() >= 6)
					m_pDxSurface = _New CDxSurfaceEx();
				else
					m_pDDraw = _New CDirectDraw();
			}
			else
				m_pDxSurface = nullptr;
			//m_DsPlayer = _New CDSoundPlayer();
			//m_DsPlayer.Initialize(m_hWnd, Audio_Play_Segments);
		}
	}
	
	~CDvoPlayer()
	{
#ifdef _DEBUG
		OutputMsg("%s \tReady to Free a \tObject:%d.\n", __FUNCTION__, m_nObjIndex);
#endif
		StopPlay(m_bEnableD3DCache);
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
		if (m_pParserBuffer)
		{
			//delete[]m_pParserBuffer;
			_aligned_free(m_pParserBuffer);
#ifdef _DEBUG
			m_pParserBuffer = nullptr;
#endif
		}
		if (m_pDsBuffer)
		{
			m_pDsPlayer->DestroyDsoundBuffer(m_pDsBuffer);
#ifdef _DEBUG
			m_pDsBuffer = nullptr;
#endif
		}
		if (m_pDxSurface)
		{
// 			if (m_pDxSurface->IsInited() && m_bEnableD3DCache)
// 			{
// 				PutDxCache(m_pDxSurface);
// 				OutputMsg("%s nVideoWidth = %d\tnVideoHeight = %d\n",__FUNCTION__, m_nVideoWidth, m_nVideoHeight);
// 			}
// 			else
// 			{
				delete m_pDxSurface;
				m_pDxSurface = nullptr;
			//}
		}
		if (m_pDDraw)
		{
			delete m_pDDraw;
		}
		if (m_hCacheFulled)
		{
			CloseHandle(m_hCacheFulled);
#ifdef _DEBUG
			m_hCacheFulled = nullptr;
#endif		
		}
		
		m_listVideoCache.clear();
		if (m_pszFileName)
			delete[]m_pszFileName;
		if (m_hDvoFile)
			CloseHandle(m_hDvoFile);

		if (m_hEvnetYUVReady)
			CloseHandle(m_hEvnetYUVReady);
		if (m_hEventDecodeStart)
			CloseHandle(m_hEventDecodeStart);

 		if (m_hEventYUVRequire)
 			CloseHandle(m_hEventYUVRequire);
		if (m_hEventFrameCopied)
			CloseHandle(m_hEventFrameCopied);

		if (m_hThreadGetFileSummary)
		{
			m_bThreadSummaryRun = false;		// ���Ҫ�߳������˳�
			WaitForSingleObject(m_hThreadGetFileSummary, INFINITE);
			CloseHandle(m_hThreadGetFileSummary);
		}
		DeleteCriticalSection(&m_csVideoCache);
		DeleteCriticalSection(&m_csAudioCache);
		DeleteCriticalSection(&m_csSeekOffset);
		DeleteCriticalSection(&m_csParser);
		DeleteCriticalSection(&m_csBorderRect);

#ifdef _DEBUG
		OutputMsg("%s \tFinish Free a \tObject:%d.\n", __FUNCTION__, m_nObjIndex);
		OutputMsg("%s \tObject:%d Exist Time = %u(ms).\n", __FUNCTION__, m_nObjIndex, timeGetTime() - m_nLifeTime);
#endif
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
			memcpy_s(m_pMediaHeader.get(), sizeof(DVO_MEDIAINFO), szStreamHeader, sizeof(DVO_MEDIAINFO));
			m_nSDKVersion = m_pMediaHeader->nSDKversion;
			switch (m_nSDKVersion)
			{
			case DVO_IPC_SDK_VERSION_2015_09_07:
			case DVO_IPC_SDK_VERSION_2015_10_20:
			case DVO_IPC_SDK_GSJ_HEADER:
			{
				m_nFileFPS = 25;
				m_nVideoCodec = CODEC_UNKNOWN;
				m_nVideoWidth = 0;
				m_nVideoHeight = 0;
				break;
			}
			case DVO_IPC_SDK_VERSION_2015_12_16:
			{
				m_nVideoCodec = m_pMediaHeader->nVideoCodec;
				m_nAudioCodec = m_pMediaHeader->nAudioCodec;
				if (m_nVideoCodec == CODEC_UNKNOWN)
				{
					m_nVideoWidth = 0;
					m_nVideoHeight = 0;
				}
				else
				{
					m_nVideoWidth = m_pMediaHeader->nVideoWidth;
					m_nVideoHeight = m_pMediaHeader->nVideoHeight;
					if (!m_nVideoWidth || !m_nVideoHeight)
						//return DVO_Error_MediaFileHeaderError;
						m_nVideoCodec = CODEC_UNKNOWN;
				}
				if (m_pMediaHeader->nFps == 0)
					m_nFileFPS = 25;
				else
					m_nFileFPS = m_pMediaHeader->nFps;
			}
				break;
			default:
				return DVO_Error_InvalidSDKVersion;
			}
			m_nFileFrameInterval = 1000 / m_nFileFPS;
			return DVO_Succeed;
		}
		else
			return DVO_Error_InsufficentMemory;
	}
	inline bool GetD3DCache()
	{
		return m_bEnableD3DCache;
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
		_MyEnterCriticalSection(&m_csVideoCache);
		m_listVideoCache.clear();
		_MyLeaveCriticalSection(&m_csVideoCache);

		_MyEnterCriticalSection(&m_csAudioCache);
		m_listAudioCache.clear();
		_MyLeaveCriticalSection(&m_csAudioCache);
	}

	/// @brief ������ʾ�߽�,�߽����ͼ�񽫲�������ʾ
	/// @param rtBorder	�߽���� �������ͼ��
	/// left	��߽����
	/// top		�ϱ߽����
	/// right	�ұ߽����
	/// bottom  �±߽���� 
	///��������������������������������������������������������������������������
	///��                  ��                   ��
	///��                 top                  ��
	///��                  ��                   ���������������� the source rect
	///��       ������������������������������������������        ��
	///��       ��                     ��        ��
	///��       ��                     ��        ��
	///����left�� ��  the clipped rect   ����right�� ��
	///��       ��                     ��        ��
	///��       ��                     ��        ��
	///��       ������������������������������������������        ��
	///��                  ��                   ��
	///��                bottom                ��
	///��                  ��                   ��
	///��������������������������������������������������������������������������
	/// @remark �߽����������λ�ò��ɵߵ�
	void SetBorderRect(RECT rtBorder)
	{
		RECT rtVideo = {0};
		rtVideo.left = rtBorder.left;
		rtVideo.right = m_nVideoWidth - rtBorder.right;
		rtVideo.top += rtBorder.top;
		rtVideo.bottom = m_nVideoHeight - rtBorder.bottom;
		CAutoLock lock(&m_csBorderRect);
		if (!m_pBorderRect)
			m_pBorderRect = make_shared<RECT>();

		CopyRect(m_pBorderRect.get(), &rtVideo);
	}

	void RemoveBorderRect()
	{
		CAutoLock lock(&m_csBorderRect);
		if (m_pBorderRect)
			m_pBorderRect = nullptr;
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
#ifdef _DEBUG
		OutputMsg("%s \tObject:%d Time = %d.\n", __FUNCTION__, m_nObjIndex, timeGetTime() - m_nLifeTime);
#endif
		m_bPause = false;
		m_bFitWindow = bFitWindow;	
		if (GetOsMajorVersion() >= 6)
			m_bEnableHaccel = bEnableHaccel;
		
// 		if (!m_hWnd || !IsWindow(m_hWnd))
// 		{
// 			return DVO_Error_InvalidWindow;
// 		}
		if (m_pszFileName )
		{
			if (m_hDvoFile == INVALID_HANDLE_VALUE)
			{
				return GetLastError();
			}
		
			// �����ļ������߳�
			m_bThreadParserRun = true;
			m_hThreadParser = (HANDLE)_beginthreadex(nullptr, 0, ThreadParser, this, 0, 0);
			//m_hThreadParser = CreateThread(nullptr, 0, ThreadParser, this, 0, 0);
		}
		else
		{
//  		if (!m_pMediaHeader)
//  			return DVO_Error_NotInputStreamHeader;		// δ����������ͷ
			m_listVideoCache.clear();
			m_listAudioCache.clear();
		}
		m_bStopFlag = false;
		// �����������߳�
		m_bThreadPlayVideoRun = true;			
		//m_hThreadPlayVideo = CreateThread(nullptr, 0, ThreadPlayVideo, this, 0, 0);
		m_hThreadPlayVideo = (HANDLE)_beginthreadex(nullptr, 0, ThreadPlayVideo, this, 0, 0);
		if (!m_hThreadPlayVideo)
		{
#ifdef _DEBUG
			OutputMsg("%s \tObject:%d ThreadPlayVideo Start failed:%d.\n", __FUNCTION__, m_nObjIndex, GetLastError());
#endif
			return DVO_Error_VideoThreadStartFailed;
		}
		if (m_hWnd)
			EnableAudio(bEnaleAudio);
		else
			EnableAudio(false);
		return DVO_Succeed;
	}

	/// @brief ��λ���Ŵ���
	bool Reset(HWND hWnd = nullptr, int nWidth = 0, int nHeight = 0)
	{
// 		CAutoLock lock(&m_csDxSurface, false, __FILE__, __FUNCTION__, __LINE__);
// 		if (m_pDxSurface)
// 		{
// 			m_bDxReset = true;
// 			m_hDxReset = hWnd;
// 			return true;
// 		}
// 		else
// 			return false;
		return true;
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

		if (SetFilePointer(m_hDvoFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			assert(false);
			return 0;
		}

		if (!ReadFile(m_hDvoFile, (void *)m_pMediaHeader.get(), sizeof(DVO_MEDIAINFO), &dwBytesRead, nullptr))
		{
			CloseHandle(m_hDvoFile);
			return GetLastError();
		}
		// ������Ƶ�ļ�ͷ
		if ((m_pMediaHeader->nMediaTag != DVO_TAG && 
			 m_pMediaHeader->nMediaTag != GSJ_TAG) ||
			dwBytesRead != sizeof(DVO_MEDIAINFO))
		{
			CloseHandle(m_hDvoFile);
			return DVO_Error_NotDvoVideoFile;
		}
		m_nSDKVersion = m_pMediaHeader->nSDKversion;
		switch (m_nSDKVersion)
		{

		case DVO_IPC_SDK_VERSION_2015_09_07:
		case DVO_IPC_SDK_VERSION_2015_10_20:
		case DVO_IPC_SDK_GSJ_HEADER:
		{
			m_nFileFPS = 25;
			m_nVideoCodec = CODEC_UNKNOWN;
			m_nVideoWidth = 0;
			m_nVideoHeight = 0;
		}
			break;
		case DVO_IPC_SDK_VERSION_2015_12_16:
		{
			m_nVideoCodec = m_pMediaHeader->nVideoCodec;
			m_nAudioCodec = m_pMediaHeader->nAudioCodec;
			if (m_nVideoCodec == CODEC_UNKNOWN)
			{
				m_nVideoWidth = 0;
				m_nVideoHeight = 0;
			}
			else
			{
				m_nVideoWidth = m_pMediaHeader->nVideoWidth;
				m_nVideoHeight = m_pMediaHeader->nVideoHeight;
				if (!m_nVideoWidth || !m_nVideoHeight)
					//return DVO_Error_MediaFileHeaderError;
					m_nVideoCodec = CODEC_UNKNOWN;
			}
			if (m_pMediaHeader->nFps == 0)
				m_nFileFPS = 25;
			else
				m_nFileFPS = m_pMediaHeader->nFps;
		}
		break;
		default:
		
			return DVO_Error_InvalidSDKVersion;
		}
		m_nFileFrameInterval = 1000 / m_nFileFPS;
		
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
	int InputStream(unsigned char *szFrameData, int nFrameSize, UINT nCacheSize = 0, bool bNoThreadCheck = false)
	{
		if (!szFrameData || nFrameSize < sizeof(DVOFrameHeader))
			return DVO_Error_InvalidFrame;
		m_bIpcStream = false;
		int nMaxCacheSize = m_nMaxFrameCache;
		if (nCacheSize != 0)
			nMaxCacheSize = nCacheSize;
		if (m_bStopFlag)
			return DVO_Error_PlayerHasStop;
		if (!bNoThreadCheck)
		{
			if (!m_bThreadPlayVideoRun || !m_hThreadPlayVideo)
			{
#ifdef _DEBUG
//				OutputMsg("%s \tObject:%d Video decode thread not run.\n", __FUNCTION__, m_nObjIndex);
#endif
				return DVO_Error_VideoThreadNotRun;
			}
		}
		
		m_bIpcStream = false;		// ��IPC����
		DVOFrameHeader *pHeaderEx = (DVOFrameHeader *)szFrameData;
		if (m_nSDKVersion >= DVO_IPC_SDK_VERSION_2015_12_16 && m_nSDKVersion != DVO_IPC_SDK_GSJ_HEADER)
		{
			switch (pHeaderEx->nType)
			{
				case 0:
				case FRAME_P:
				case FRAME_B:
				case FRAME_I:
				case FRAME_IDR:
				{
	// 				if (!m_hThreadPlayVideo)
	// 					return DVO_Error_VideoThreadNotRun;
					CAutoLock lock(&m_csVideoCache, false, __FILE__, __FUNCTION__, __LINE__);
					if (m_listVideoCache.size() >= nMaxCacheSize)
						return DVO_Error_FrameCacheIsFulled;
					StreamFramePtr pStream = make_shared<StreamFrame>(szFrameData, nFrameSize,m_nFileFrameInterval,m_nSDKVersion);
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
	// 				if (!m_hThreadPlayVideo)
	// 					return DVO_Error_AudioThreadNotRun;
					if (!m_bEnableAudio && !bNoThreadCheck)
						break;
					if (m_fPlayRate != 1.0f)
						break;
					CAutoLock lock(&m_csAudioCache, false, __FILE__, __FUNCTION__, __LINE__);
					if (m_listAudioCache.size() >= nMaxCacheSize*2)
						return DVO_Error_FrameCacheIsFulled;
					StreamFramePtr pStream = make_shared<StreamFrame>(szFrameData, nFrameSize, m_nFileFrameInterval/2);
					if (!pStream)
						return DVO_Error_InsufficentMemory;
					m_listAudioCache.push_back(pStream);
					m_nAudioFrames++;
					break;
				}
				default:
				{
					assert(false);
					return DVO_Error_InvalidFrameType;
					break;
				}
			}
		}
		else
		{
			switch (pHeaderEx->nType)
			{
			case 0:				// ��ƵBP֡
			case 1:				// ��ƵI֡
			{
				CAutoLock lock(&m_csVideoCache, false, __FILE__, __FUNCTION__, __LINE__);
				if (m_listVideoCache.size() >= nMaxCacheSize)
					return DVO_Error_FrameCacheIsFulled;
				StreamFramePtr pStream = make_shared<StreamFrame>(szFrameData, nFrameSize, m_nFileFrameInterval, m_nSDKVersion);
				if (!pStream)
					return DVO_Error_InsufficentMemory;
				m_listVideoCache.push_back(pStream);
				break;
			}
			
			case 2:				// ��Ƶ֡
			case FRAME_G711U:
			//case FRAME_G726:    // G726����֡
			{
				if (!m_bEnableAudio)
					break;
				if (m_fPlayRate != 1.0f)
					break;
				CAutoLock lock(&m_csAudioCache, false, __FILE__, __FUNCTION__, __LINE__);
				if (m_listAudioCache.size() >= nMaxCacheSize*2)
					return DVO_Error_FrameCacheIsFulled;
				Frame(szFrameData)->nType = CODEC_G711U;			// �ɰ�SDKֻ֧��G711U���룬��������ǿ��ת��ΪG711U������ȷ����
				StreamFramePtr pStream = make_shared<StreamFrame>(szFrameData, nFrameSize, m_nFileFrameInterval / 2);
				if (!pStream)
					return DVO_Error_InsufficentMemory;
				m_listAudioCache.push_back(pStream);
				m_nAudioFrames++;
				break;
			}
			default:
			{
				//assert(false);
				return DVO_Error_InvalidFrameType;
				break;
			}
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
// 	UINT m_nAudioFrames1 = 0;
// 	UINT m_nVideoFraems = 0;
// 	DWORD m_dwInputStream = timeGetTime();
	int InputStream(IN byte *pFrameData, IN int nFrameType, IN int nFrameLength, int nFrameNum, time_t nFrameTime)
	{
// #ifdef _DEBUG
// 		if (m_OuputTime.nInputStream == 0 ||
// 			(timeGetTime() - m_OuputTime.nInputStream) >= 5000)
// 		{
// 			OutputMsg("%s \tObject:%d.\n", __FUNCTION__, m_nObjIndex);
// 			m_OuputTime.nInputStream = timeGetTime();
// 		}
// #endif
		
		if (m_bStopFlag)
			return DVO_Error_PlayerHasStop;

		if (!m_bThreadPlayVideoRun ||!m_hThreadPlayVideo)
		{
#ifdef _DEBUG
			OutputMsg("%s \tObject:%d Video decode thread not run.\n", __FUNCTION__, m_nObjIndex);
#endif
			return DVO_Error_VideoThreadNotRun;
		}
		DWORD dwThreadCode = 0;
		GetExitCodeThread(m_hThreadPlayVideo, &dwThreadCode);
		if (dwThreadCode != STILL_ACTIVE)		// �߳����˳�
		{
			return DVO_Error_VideoThreadAbnormalExit;
		}
		m_bIpcStream = true;
		switch (nFrameType)
		{
			case 0:									// ��˼I֡��Ϊ0�����ǹ̼���һ��BUG���д�����
			case APP_NET_TCP_COM_DST_IDR_FRAME: 	// IDR֡��
			case APP_NET_TCP_COM_DST_I_FRAME:		// I֡��		
			case APP_NET_TCP_COM_DST_P_FRAME:       // P֡��
			case APP_NET_TCP_COM_DST_B_FRAME:       // B֡��
			{
				//m_nVideoFraems++;
				StreamFramePtr pStream = make_shared<StreamFrame>(pFrameData, nFrameType, nFrameLength, nFrameNum, nFrameTime);
				CAutoLock lock(&m_csVideoCache, false, __FILE__, __FUNCTION__, __LINE__);
				if (m_listVideoCache.size() >= m_nMaxFrameCache)
				{
// #ifdef _DEBUG
// 					OutputMsg("%s \tObject:%d Video Frame cache is Fulled.\n", __FUNCTION__, m_nObjIndex);
// #endif
					return DVO_Error_FrameCacheIsFulled;
				}	
				if (!pStream)
				{
// #ifdef _DEBUG
// 					OutputMsg("%s \tObject:%d InsufficentMemory when alloc memory for video frame.\n", __FUNCTION__, m_nObjIndex);
// #endif
					return DVO_Error_InsufficentMemory;
				}
				m_listVideoCache.push_back(pStream);
			}
				break;
			case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A�ɱ���֡
			case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U�ɱ���֡
			case APP_NET_TCP_COM_DST_726:           // 726����֡
			case APP_NET_TCP_COM_DST_AAC:           // AAC����֡��
			{
				m_nAudioCodec = (DVO_CODEC)nFrameType;
				//m_nAudioFrames1++;
// 				if ((timeGetTime() - m_dwInputStream) >= 20000)
// 				{
// 					TraceMsgA("%s VideoFrames = %d\tAudioFrames = %d.\n", __FUNCTION__, m_nVideoFraems, m_nAudioFrames1);
// 					m_dwInputStream = timeGetTime();
// 				}
				if (!m_bEnableAudio)
					break;
				StreamFramePtr pStream = make_shared<StreamFrame>(pFrameData, nFrameType, nFrameLength, nFrameNum, nFrameTime);
				if (!pStream)
					return DVO_Error_InsufficentMemory;
				CAutoLock lock(&m_csAudioCache, false, __FILE__, __FUNCTION__, __LINE__);
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
		return 0;
	}

	bool StopPlay(bool bCacheD3DCache = true,DWORD nTimeout = 1000)
	{
#ifdef _DEBUG
		TraceFunction();
		OutputMsg("%s \tObject:%d Time = %d.\n", __FUNCTION__, m_nObjIndex, timeGetTime() - m_nLifeTime);
#endif
// 		if (!m_bIpcStream)			// �����ļ���������ʹ���첽�رյķ�ʽ
// 			nTimeout = INFINITE;
		m_bStopFlag = true;
		m_bThreadParserRun = false;
		m_bThreadPlayVideoRun = false;
		m_bThreadPlayAudioRun = false;
		m_bThreadSummaryRun = false;
		HANDLE hArray[4] = { 0 };
		int nHandles = 0;
		m_bEnableD3DCache = bCacheD3DCache;
		
		m_bPause = false;
		if (m_hThreadParser)
			hArray[nHandles++] = m_hThreadParser;
		if (m_hThreadPlayVideo)
			hArray[nHandles++] = m_hThreadPlayVideo;
		if (m_hThreadPlayAudio)
		{
			hArray[nHandles++] = m_hThreadPlayAudio;
			ResumeThread(m_hThreadPlayAudio);
		}
		if (m_hThreadGetFileSummary)
			hArray[nHandles++] = m_hThreadGetFileSummary;
		EnterCriticalSection(&m_csAudioCache);
		m_listAudioCache.clear();
		LeaveCriticalSection(&m_csAudioCache);

		EnterCriticalSection(&m_csVideoCache);
		m_listVideoCache.clear();
		LeaveCriticalSection(&m_csVideoCache);

		if (WaitForMultipleObjects(nHandles, hArray, true, nTimeout) == WAIT_TIMEOUT)
		{
			OutputMsg("%s Wait for thread exit timeout.\n", __FUNCTION__);
			return false;
		}
		if (m_hThreadParser)
		{
			CloseHandle(m_hThreadParser);
			m_hThreadParser = nullptr;
			OutputMsg("%s ThreadParser has exit.\n", __FUNCTION__);
		}
		if (m_hThreadPlayVideo)
		{
			CloseHandle(m_hThreadPlayVideo);
			m_hThreadPlayVideo = nullptr;
#ifdef _DEBUG
			OutputMsg("%s ThreadPlayVideo Object:%d has exit.\n", __FUNCTION__,m_nObjIndex);
#endif
		}
		if (m_hThreadPlayAudio)
		{
			CloseHandle(m_hThreadPlayAudio);
			m_hThreadPlayAudio = nullptr;
			OutputMsg("%s ThreadPlayAudio has exit.\n", __FUNCTION__);
		}
		EnableAudio(false);
		if (m_pFrameOffsetTable)
		{
			delete[]m_pFrameOffsetTable;
			m_pFrameOffsetTable = nullptr;
		}
		
#ifdef _DEBUG
		m_hThreadPlayVideo = nullptr;		
		m_hThreadParser = nullptr;
//		m_hThreadGetFileSummary = nullptr;
		m_hThreadPlayAudio = nullptr;
		m_pFrameOffsetTable = nullptr;
#endif
		return true;
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
			if (bEnableHaccel)
			{
				if (GetOsMajorVersion() >= 6)
					m_bEnableHaccel = bEnableHaccel;
				else
					return DVO_Error_UnsupportHaccel;
			}
			else
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

	/// @brief			ȡ�õ�ǰ������Ƶ�ļ�ʱ��Ϣ
	int GetPlayerInfo(PlayerInfo *pPlayInfo)
	{
		if (!pPlayInfo)
			return DVO_Error_InvalidParameters;
		if (m_hThreadPlayVideo|| m_hDvoFile)
		{
			ZeroMemory(pPlayInfo, sizeof(PlayerInfo));
			pPlayInfo->nVideoCodec	 = m_nVideoCodec;
			pPlayInfo->nVideoWidth	 = m_nVideoWidth;
			pPlayInfo->nVideoHeight	 = m_nVideoHeight;
			pPlayInfo->nAudioCodec	 = m_nAudioCodec;
			pPlayInfo->bAudioEnabled = m_bEnableAudio;
			pPlayInfo->tTimeEplased	 = (time_t)(1000 * (GetExactTime() - m_dfTimesStart));
			pPlayInfo->nFPS			 = m_nFileFPS;
			pPlayInfo->nPlayFPS		 = m_nPlayFPS;
			pPlayInfo->nCacheSize	 = m_nVideoCache;
			pPlayInfo->nCacheSize2	 = m_nAudioCache;
			if (m_pszFileName )
			{
				pPlayInfo->nCurFrameID = m_nCurVideoFrame;
				pPlayInfo->nTotalFrames = m_nTotalFrames;
				if (m_nSDKVersion >= DVO_IPC_SDK_VERSION_2015_12_16 && m_nSDKVersion != DVO_IPC_SDK_GSJ_HEADER)
				{
					pPlayInfo->tTotalTime = m_nTotalFrames*1000/m_nFileFPS;
					pPlayInfo->tCurFrameTime = m_nCurVideoFrame * 1000 / m_nFileFPS;
				}
				else
				{
					pPlayInfo->tTotalTime = m_nTotalTime;
					if (m_pMediaHeader->nCameraType == 1)	// ��Ѷʿ���
						pPlayInfo->tCurFrameTime = (m_tCurFrameTimeStamp - m_FirstFrame.nTimestamp) / (1000 * 1000);
					else
					{
						pPlayInfo->tCurFrameTime = (m_tCurFrameTimeStamp - m_FirstFrame.nTimestamp) / 1000;
						pPlayInfo->nCurFrameID = pPlayInfo->tCurFrameTime*m_nFileFPS/1000;
					}
				}
			}
			else
				pPlayInfo->tTotalTime = 0;
			
			return DVO_Succeed;
		}
		else
			return DVO_Error_PlayerNotStart;
	}
// 	static void CloseShareMemory(HANDLE &hMemHandle, void *pMemory)
// 	{
// 		if (!pMemory || !hMemHandle)
// 			return;
// 		if (pMemory)
// 		{
// 			UnmapViewOfFile(pMemory);
// 			pMemory = NULL;
// 		}
// 		if (hMemHandle)
// 		{
// 			CloseHandle(hMemHandle);
// 			hMemHandle = NULL;
// 		}
// 	}
// 
// 	static void *OpenShareMemory(HANDLE &hMemHandle, TCHAR *szUniqueName)
// 	{
// 		void *pShareSection = NULL;
// 		bool bResult = false;
// 		__try
// 		{
// 			__try
// 			{
// 				if (!szUniqueName || _tcslen(szUniqueName) < 1)
// 					__leave;
// 				hMemHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, szUniqueName);
// 				if (hMemHandle == INVALID_HANDLE_VALUE ||
// 					hMemHandle == NULL)
// 				{
// 					_TraceMsg(_T("%s %d MapViewOfFile Failed,ErrorCode = %d.\r\n"), __FILEW__, __LINE__, GetLastError());
// 					__leave;
// 				}
// 				pShareSection = MapViewOfFile(hMemHandle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
// 				if (pShareSection == NULL)
// 				{
// 					_TraceMsg(_T("%s %d MapViewOfFile Failed,ErrorCode = %d.\r\n"), __FILEW__, __LINE__, GetLastError());
// 					__leave;
// 				}
// 				bResult = true;
// 				_TraceMsg(_T("Open Share memory map Succeed."));
// 			}
// 			__finally
// 			{
// 				if (!bResult)
// 				{
// 					CloseShareMemory(hMemHandle, pShareSection);
// 					pShareSection = NULL;
// 				}
// 			}
// 		}
// 		__except (1)
// 		{
// 			TraceMsg(_T("Exception in OpenShareMemory.\r\n"));
// 		}
// 		return pShareSection;
// 	}
	/// @brief			����ͼ�����Ƿ����
	/// @param [inout]	hSnapWnd		���ؽ�ͼ���̵Ĵ���
	/// @retval			0	�����ɹ����������ʧ��
// 	static int CheckSnapshotWnd(HWND &hSnapWnd)
// 	{
// 		// ���ҽ�ͼ����
// 		if (hSnapWnd && IsWindow(hSnapWnd))
// 			return DVO_Succeed;
// 		hSnapWnd = FindWindow(nullptr, _T("DVO SnapShot"));
// 		if (!hSnapWnd)
// 		{// ׼��������ͼ����
// 			TCHAR szAppPath[1024] = { 0 };
// 			GetAppPath(szAppPath, 1024);
// 			_tcscat(szAppPath, _T("\\DVOSnapshot.exe"));
// 			if (!PathFileExists(szAppPath))
// 			{// ��ͼ�����ļ�������
// 				return DVO_Error_SnapShotProcessFileMissed;
// 			}
// 			//////////////////////////////////////////////////////////////////////////
// 			///�����������ͨ��_stprintf��ʽ��������_SnapShotCmdLine���Ƶ�szCmdLine�в�����Ч
// 			///ԭ����,��������ô��������Ŀ������޷��յ������в���
// 			//////////////////////////////////////////////////////////////////////////
// 			TCHAR szCmdLine[128] = { 0 };
// 			_stprintf(szCmdLine, _T(" %s"), _SnapShotCmdLine);
// 			STARTUPINFO si;
// 			PROCESS_INFORMATION pi;
// 			ZeroMemory(&si, sizeof(si));
// 			si.cb = sizeof(si);
// 			ZeroMemory(&pi, sizeof(pi));
// 			if (!CreateProcess(szAppPath, //module name 
// 				szCmdLine,
// 				NULL,             // Process handle not inheritable. 
// 				NULL,             // Thread handle not inheritable. 
// 				FALSE,            // Set handle inheritance to FALSE. 
// 				0,                // No creation flags. 
// 				NULL,             // Use parent's environment block. 
// 				NULL,             // Use parent's starting directory. 
// 				&si,              // Pointer to STARTUPINFO structure.
// 				&pi))             // Pointer to PROCESS_INFORMATION structure.
// 			{
// 				return DVO_Error_SnapShotProcessStartFailed;
// 			}
// 			CloseHandle(pi.hProcess);
// 			CloseHandle(pi.hThread);
// 		}
// 		int nRetry = 0;
// 		while (!hSnapWnd && nRetry < 5)
// 		{
// 			hSnapWnd = FindWindow(nullptr, _T("DVO SnapShot"));
// 			nRetry++;
// 			Sleep(100);
// 		}
// 		if (!hSnapWnd || !IsWindow(hSnapWnd))
// 			return DVO_Error_SnapShotProcessNotRun;
// 		return DVO_Succeed;
// 	}

	/// @brief			��ȡ���Ų��ŵ���Ƶͼ��
	/// @param [in]		szFileName		Ҫ������ļ���
	/// @param [in]		nFileFormat		�����ļ��ı����ʽ,@see SNAPSHOT_FORMAT����
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
// 	inline int  SnapShot2(IN WCHAR *szFileName, IN SNAPSHOT_FORMAT nFileFormat = XIFF_JPG)
// 	{
// 		if (!szFileName || !wcslen(szFileName))
// 			return -1;
// 		if (m_hThreadPlayVideo)
// 		{
// 			if (WaitForSingleObject(m_hEvnetYUVReady, 5000) == WAIT_TIMEOUT)
// 				return DVO_Error_PlayerNotStart;
// 			if (m_pDxSurface)
// 			{
// 				int nResult = CheckSnapshotWnd(g_hSnapShotWnd);
// 				if (nResult != DVO_Succeed)
// 					return nResult;
// 				wcscpy(m_szSnapShotFile, szFileName);
// 				TraceMsgA("%s Input FileName:%s\tOuput FileName:%s\n", __FUNCTION__, szFileName, m_szSnapShotFile);
// 				SetEvent(m_hEventYUVRequire);		// ����YUV����
// 				m_nD3DImageFormat = nFileFormat;
// 				// �ȴ���ͼ���
// 				if (WaitForSingleObject(m_hEventSnapShot, 5000) == WAIT_TIMEOUT)
// 					return DVO_Error_SnapShotFailed;
// 				return DVO_Succeed;
// 			}
// 			else
// 				return DVO_Error_WindowNotAssigned;
// 		}
// 		else
// 			return DVO_Error_PlayerNotStart;
// 		return DVO_Succeed;
// 	}


	

	/// @brief			��ȡ���Ų��ŵ���Ƶͼ��
	/// @param [in]		szFileName		Ҫ������ļ���
	/// @param [in]		nFileFormat		�����ļ��ı����ʽ,@see SNAPSHOT_FORMAT����
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	inline int  SnapShot(IN CHAR *szFileName, IN SNAPSHOT_FORMAT nFileFormat = XIFF_JPG)
	{
		if (!szFileName || !strlen(szFileName))
			return -1;
		if (m_hThreadPlayVideo)
		{
			if (WaitForSingleObject(m_hEvnetYUVReady, 5000) == WAIT_TIMEOUT)
				return DVO_Error_PlayerNotStart;
			char szAvError[1024] = { 0 };
			if (!m_pSnapshot)
			{
				m_pSnapshot = make_shared<CSnapshot>(m_nDecodePirFmt, m_nVideoWidth, m_nVideoHeight);
				//m_pSnapshot = make_shared<CSnapshot>(AV_PIX_FMT_YUV420P, m_nVideoWidth, m_nVideoHeight);
				if (!m_pSnapshot)
					return DVO_Error_InsufficentMemory;
			}
			SetEvent(m_hEventYUVRequire);
			if (WaitForSingleObject(m_hEventFrameCopied, 1000) == WAIT_TIMEOUT)
				return DVO_Error_SnapShotFailed;
			int nResult = DVO_Succeed;
			switch (nFileFormat)
			{
			case XIFF_BMP:
				if (!m_pSnapshot->SaveBmp(szFileName))
					nResult = DVO_Error_SnapShotFailed;
				break;
			case XIFF_JPG:
				if (!m_pSnapshot->SaveJpeg(szFileName))
					nResult = DVO_Error_SnapShotFailed;
				break;
			default:
				nResult = DVO_Error_UnsupportedFormat;
				break;
			}
			m_pSnapshot.reset();
			return nResult;
		}
		else
			return DVO_Error_PlayerNotStart;
		return DVO_Succeed;
	}
	/// @brief			�����ͼ����
	/// remark			������ɺ󣬽�����m_hEventSnapShot�¼�
	void ProcessSnapshotRequire(AVFrame *pAvFrame)
	{
		if (!pAvFrame)
			return;
		if (WaitForSingleObject(m_hEventYUVRequire, 0) == WAIT_TIMEOUT)
			return;
		
		if (pAvFrame->format == AV_PIX_FMT_YUV420P ||
			pAvFrame->format == AV_PIX_FMT_YUVJ420P)
// 		{// �ݲ�֧��dxva Ӳ����֡
// 			//CopyDxvaFrame(pYUV, pAvFrame);
// 		}
// 		else
		{
			
			m_pSnapshot->CopyFrame(pAvFrame);
			SetEvent(m_hEventFrameCopied);
		}
	}

	/// @brief			���ò��ŵ�����
	/// @param [in]		nVolume			Ҫ���õ�����ֵ��ȡֵ��Χ0~100��Ϊ0ʱ����Ϊ����
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	void  SetVolume(IN int nVolume)
	{
		if (m_bEnableAudio && m_pDsBuffer)
		{
			int nDsVolume = nVolume * 100 - 10000;
			m_pDsBuffer->SetVolume(nDsVolume);
		}
	}

	/// @brief			ȡ�õ�ǰ���ŵ�����
	int  GetVolume()
	{
		if (m_bEnableAudio && m_pDsBuffer)
			return (m_pDsBuffer->GetVolume() + 10000) / 100;
		else
			return 0;
	}

	/// @brief			���õ�ǰ���ŵ��ٶȵı���
	/// @param [in]		fPlayRate		��ǰ�Ĳ��ŵı���,����1ʱΪ���ٲ���,С��1ʱΪ���ٲ��ţ�����Ϊ0��С��0
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	inline int  SetRate(IN float fPlayRate)
	{
#ifdef _DEBUG
		m_nRenderFPS = 0;
		dfTRender = 0.0f;
		m_nRenderFrames = 0;
#endif
		if (m_bIpcStream)
		{
			return DVO_Error_NotFilePlayer;
		}
		if (fPlayRate > (float)m_nFileFPS)
			fPlayRate = m_nFileFPS;
		// ȡ�õ�ǰ��ʾ����ˢ���ʣ���ʾ����ˢ���ʾ����ˣ���ʾͼ������֡��
		// ͨ��ͳ��ÿ��ʾһ֡ͼ��(���������ʾ)�ķѵ�ʱ��
		
		DEVMODE   dm;
		dm.dmSize = sizeof(DEVMODE);
		::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
		m_fPlayInterval = (int)(1000 / (m_nFileFPS*fPlayRate));
/// marked by xionggao.lee @2016.03.23
// 		float nMinPlayInterval = ((float)1000) / dm.dmDisplayFrequency;
// 		if (m_fPlayInterval < nMinPlayInterval)
// 			m_fPlayInterval = nMinPlayInterval;
		m_nPlayFPS = 1000 / m_fPlayInterval;
		m_fPlayRate = fPlayRate;

		return DVO_Succeed;
	}

	/// @brief			��Ծ��ָ��Ƶ֡���в���
	/// @param [in]		nFrameID	Ҫ����֡����ʼID
	/// @param [in]		bUpdate		�Ƿ���»���,bUpdateΪtrue�����Ը��»���,�����򲻸���
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	/// @remark			1.����ָ��ʱ����Ӧ֡Ϊ�ǹؼ�֡��֡�Զ��ƶ����ͽ��Ĺؼ�֡���в���
	///					2.����ָ��֡Ϊ�ǹؼ�֡��֡�Զ��ƶ����ͽ��Ĺؼ�֡���в���
	///					3.ֻ���ڲ�����ʱ,bUpdate��������Ч
	///					4.���ڵ�֡����ʱֻ����ǰ�ƶ�
	int  SeekFrame(IN int nFrameID,bool bUpdate = false)
	{
		if (!m_bSummaryIsReady)
			return DVO_Error_SummaryNotReady;

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
		// ���������I֡
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
	
		if ((nForward - nFrameID) <= (nFrameID - nBackWord))
			m_nFrametoRead = nForward;
		else
			m_nFrametoRead = nBackWord;
		m_nCurVideoFrame = m_nFrametoRead;
		//TraceMsgA("%s Seek to Frame %d\tFrameTime = %I64d\n", __FUNCTION__, m_nFrametoRead, m_pFrameOffsetTable[m_nFrametoRead].tTimeStamp/1000);
		if (m_hThreadParser)
			SetSeekOffset(m_pFrameOffsetTable[m_nFrametoRead].nOffset);
		else
		{// ֻ���ڵ����Ľ����ļ�ʱ�ƶ��ļ�ָ��
			CAutoLock lock(&m_csParser, false, __FILE__, __FUNCTION__, __LINE__);
			m_nParserOffset = 0;
			m_nParserDataLength = 0;
			LONGLONG nOffset = m_pFrameOffsetTable[m_nFrametoRead].nOffset;
			if (LargerFileSeek(m_hDvoFile, nOffset, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			{
				OutputMsg("%s LargerFileSeek  Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return GetLastError();
			}
		}
		if (bUpdate && 
			m_hThreadPlayVideo &&	// �������������߳�
			m_bPause &&				// ��������ͣģʽ			
			m_pDecodec)				// ����������������
		{
			// ��ȡһ֡,�����Խ���,��ʾ
			DWORD nBufferSize = m_pFrameOffsetTable[m_nFrametoRead].nFrameSize;
			LONGLONG nOffset = m_pFrameOffsetTable[m_nFrametoRead].nOffset;

			byte *pBuffer = _New byte[nBufferSize + 1];
			if (!pBuffer)
				return DVO_Error_InsufficentMemory;

			unique_ptr<byte>BufferPtr(pBuffer);			
			DWORD nBytesRead = 0;
			if (LargerFileSeek(m_hDvoFile, nOffset, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			{
				OutputMsg("%s LargerFileSeek  Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return GetLastError();
			}

			if (!ReadFile(m_hDvoFile, pBuffer, nBufferSize, &nBytesRead, nullptr))
			{
				OutputMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return GetLastError();
			}
			AVPacket *pAvPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
			shared_ptr<AVPacket>AvPacketPtr(pAvPacket, av_free);
			AVFrame *pAvFrame = av_frame_alloc();
			shared_ptr<AVFrame>AvFramePtr(pAvFrame, av_free);
			av_init_packet(pAvPacket);
			m_nCurVideoFrame = Frame(pBuffer)->nFrameID;
			pAvPacket->size = Frame(pBuffer)->nLength;
			if (m_nSDKVersion >= DVO_IPC_SDK_VERSION_2015_12_16 && m_nSDKVersion != DVO_IPC_SDK_GSJ_HEADER)
				pAvPacket->data = pBuffer + sizeof(DVOFrameHeaderEx);
			else
				pAvPacket->data = pBuffer + sizeof(DVOFrameHeader);
			int nGotPicture = 0;
			char szAvError[1024] = { 0 };
			int nAvError = m_pDecodec->Decode(pAvFrame, nGotPicture, pAvPacket);
			if (nAvError < 0)
			{
				av_strerror(nAvError, szAvError, 1024);
				OutputMsg("%s Decode error:%s.\n", __FUNCTION__, szAvError);
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
	
	/// @brief			��Ծ��ָ��ʱ��ƫ�ƽ��в���
	/// @param [in]		tTimeOffset		Ҫ���ŵ���ʼʱ��,��λ��,��FPS=25,��0.04��Ϊ��2֡��1.00�룬Ϊ��25֡
	/// @param [in]		bUpdate		�Ƿ���»���,bUpdateΪtrue�����Ը��»���,�����򲻸���
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	/// @remark			1.����ָ��ʱ����Ӧ֡Ϊ�ǹؼ�֡��֡�Զ��ƶ����ͽ��Ĺؼ�֡���в���
	///					2.����ָ��֡Ϊ�ǹؼ�֡��֡�Զ��ƶ����ͽ��Ĺؼ�֡���в���
	///					3.ֻ���ڲ�����ʱ,bUpdate��������Ч
	int  SeekTime(IN time_t tTimeOffset, bool bUpdate)
	{
		if (!m_hDvoFile)
			return DVO_Error_NotFilePlayer;
		if (!m_bSummaryIsReady)
			return DVO_Error_SummaryNotReady;

		int nFrameID = 0;
		if (m_nFileFPS == 0 || m_nFileFrameInterval == 0)
		{// ʹ�ö��ַ�����
			nFrameID = BinarySearch(tTimeOffset);
			if (nFrameID == -1)
				return DVO_Error_InvalidTimeOffset;
		}
		else
		{
			int nTimeDiff = tTimeOffset;// -m_pFrameOffsetTable[0].tTimeStamp;
			if (nTimeDiff < 0)
				return DVO_Error_InvalidTimeOffset;
			nFrameID = nTimeDiff / m_nFileFrameInterval;
		}
		return SeekFrame(nFrameID, bUpdate);
	}
	/// @brief ���ļ��ж�ȡһ֡����ȡ�����Ĭ��ֵΪ0,SeekFrame��SeekTime���趨�����λ��
	/// @param [in,out]		pBuffer		֡���ݻ�����,������Ϊnull
	/// @param [in,out]		nBufferSize ֡�������Ĵ�С
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
		CAutoLock lock(&m_csParser, false, __FILE__, __FUNCTION__, __LINE__);
		byte *pFrameBuffer = &m_pParserBuffer[m_nParserOffset];
		if (!ParserFrame(&pFrameBuffer, m_nParserDataLength, &Parser))
		{
			// �������ݳ�ΪnDataLength
			memmove(m_pParserBuffer, pFrameBuffer, m_nParserDataLength);
			if (!ReadFile(m_hDvoFile, &m_pParserBuffer[m_nParserDataLength], (m_nParserBufferSize - m_nParserDataLength), &nBytesRead, nullptr))
			{
				OutputMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
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
	
	/// @brief			������һ֡
	/// @retval			0	�����ɹ�
	/// @retval			-24	������δ��ͣ
	/// @remark			�ú����������ڵ�֡����
	int  SeekNextFrame()
	{
		if (m_hThreadPlayVideo &&	// �������������߳�
			m_bPause &&				// ��������ͣģʽ			
			m_pDecodec)				// ����������������
		{
			if (!m_hDvoFile || !m_pFrameOffsetTable)
				return DVO_Error_NotFilePlayer;

			_MyEnterCriticalSection(&m_csVideoCache);
			m_listVideoCache.clear();			
			//m_nFrameOffset = 0;
			_MyLeaveCriticalSection(&m_csVideoCache);

			_MyEnterCriticalSection(&m_csAudioCache);
			m_listAudioCache.clear();
			_MyLeaveCriticalSection(&m_csAudioCache);

			// ��ȡһ֡,�����Խ���,��ʾ
			DWORD nBufferSize = m_pFrameOffsetTable[m_nCurVideoFrame].nFrameSize;
			LONGLONG nOffset = m_pFrameOffsetTable[m_nCurVideoFrame].nOffset;

			byte *pBuffer = _New byte[nBufferSize + 1];
			if (!pBuffer)
				return DVO_Error_InsufficentMemory;

			unique_ptr<byte>BufferPtr(pBuffer);
			DWORD nBytesRead = 0;
			if (LargerFileSeek(m_hDvoFile, nOffset, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			{
				OutputMsg("%s LargerFileSeek  Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return GetLastError();
			}

			if (!ReadFile(m_hDvoFile, pBuffer, nBufferSize, &nBytesRead, nullptr))
			{
				OutputMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return GetLastError();
			}
			AVPacket *pAvPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
			shared_ptr<AVPacket>AvPacketPtr(pAvPacket, av_free);
			AVFrame *pAvFrame = av_frame_alloc();
			shared_ptr<AVFrame>AvFramePtr(pAvFrame, av_free);
			av_init_packet(pAvPacket);
			pAvPacket->size = Frame(pBuffer)->nLength;
			if (m_nSDKVersion >= DVO_IPC_SDK_VERSION_2015_12_16 && m_nSDKVersion != DVO_IPC_SDK_GSJ_HEADER)
				pAvPacket->data = pBuffer + sizeof(DVOFrameHeaderEx);
			else
				pAvPacket->data = pBuffer + sizeof(DVOFrameHeader);
			int nGotPicture = 0;
			char szAvError[1024] = { 0 };
			int nAvError = m_pDecodec->Decode(pAvFrame, nGotPicture, pAvPacket);
			if (nAvError < 0)
			{
				av_strerror(nAvError, szAvError, 1024);
				OutputMsg("%s Decode error:%s.\n", __FUNCTION__, szAvError);
				return DVO_Error_DecodeFailed;
			}
			av_packet_unref(pAvPacket);
			if (nGotPicture)
			{
				RenderFrame(pAvFrame);
				ProcessYVUCapture(pAvFrame, (LONGLONG)GetExactTime() * 1000);
				m_tCurFrameTimeStamp = m_pFrameOffsetTable[m_nCurVideoFrame].tTimeStamp;
				m_nCurVideoFrame++;
				if (m_pFilePlayCallBack)
					m_pFilePlayCallBack(this, m_pUserFilePlayer);
			}
			return DVO_Succeed;
		}
		else
			return DVO_Error_PlayerIsNotPaused;
	}

	/// @brief			��/����Ƶ����
	/// @param [in]		bEnable			�Ƿ񲥷���Ƶ
	/// -#	true		������Ƶ����
	/// -#	false		������Ƶ����
	/// @retval			0	�����ɹ�
	/// @retval			-1	���������Ч
	/// @retval			DVO_Error_AudioFailed	��Ƶ�����豸δ����
	int  EnableAudio(bool bEnable = true)
	{
		TraceFunction();
		if (m_fPlayRate != 1.0f)
			return DVO_Error_AudioFailed;
		if (m_pDsoundEnum->GetAudioPlayDevices() <= 0)
			return DVO_Error_AudioFailed;
		if (bEnable)
		{
			if (!m_pDsPlayer)
				m_pDsPlayer = make_shared<CDSound>(m_hWnd);
			if (!m_pDsPlayer->IsInitialized())
			{
				if (!m_pDsPlayer->Initialize(m_hWnd, Audio_Play_Segments))
				{
					m_pDsPlayer = nullptr;
					m_bEnableAudio = false;
					return DVO_Error_AudioFailed;
				}
			}
			m_pDsBuffer = m_pDsPlayer->CreateDsoundBuffer();
			if (!m_pDsBuffer)			
			{
				m_bEnableAudio = false;
				assert(false);
				return DVO_Error_AudioFailed;
			}
			m_bThreadPlayAudioRun = true;
			m_hAudioFrameEvent[0] = CreateEvent(nullptr, false, true, nullptr);
			m_hAudioFrameEvent[1] = CreateEvent(nullptr, false, true, nullptr);
			m_hThreadPlayAudio = (HANDLE)_beginthreadex(nullptr, 0, ThreadPlayAudio, this, 0, 0);
			m_pDsBuffer->StartPlay();
			m_dfLastTimeAudioPlay = 0.0f;
			m_dfLastTimeAudioSample = 0.0f;
			m_bEnableAudio = true;
		}
		else
		{
			if (m_hThreadPlayAudio)
			{
				m_bEnableAudio = false;			
				if (m_bThreadPlayAudioRun)		// ��δִ��ֹͣ��Ƶ�����̵߳Ĳ���
				{
					m_bThreadPlayAudioRun = false;
					ResumeThread(m_hThreadPlayAudio);
					WaitForSingleObject(m_hThreadPlayAudio, INFINITE);
					CloseHandle(m_hThreadPlayAudio);
					m_hThreadPlayAudio = nullptr;
					OutputMsg("%s ThreadPlayAudio has exit.\n", __FUNCTION__);
					EnterCriticalSection(&m_csAudioCache);
					m_listAudioCache.clear();
					LeaveCriticalSection(&m_csAudioCache);
				}
				CloseHandle(m_hAudioFrameEvent[0]);
				CloseHandle(m_hAudioFrameEvent[1]);
			}

			if (m_pDsBuffer)
			{
				m_pDsPlayer->DestroyDsoundBuffer(m_pDsBuffer);
				m_pDsBuffer = nullptr;
			}
			if (m_pDsPlayer)
				m_pDsPlayer = nullptr;
			
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

	int SetCallBack(DVO_CALLBACK nCallBackType, IN void *pUserCallBack, IN void *pUserPtr)
	{
		if (!pUserCallBack)
			return DVO_Error_InvalidParameters;
		switch (nCallBackType)
		{
		case ExternDcDraw:
		{
			if (m_pDxSurface)
			{
				m_pDxSurface->SetExternDraw(pUserCallBack, pUserPtr);
				return DVO_Succeed;
			}
			else
				return DVO_Error_DxError;
		}
			break;
		case ExternDcDrawEx:
			break;
		case YUVCapture:
		{
			m_pfnCaptureYUV = (CaptureYUV)pUserCallBack;
			m_pUserCaptureYUV = pUserPtr;
		}
			break;
		case YUVCaptureEx:
		{
			m_pfnCaptureYUVEx = (CaptureYUVEx)pUserCallBack;
			m_pUserCaptureYUVEx = pUserPtr;
		}
			break;
		case YUVFilter:
		{
			m_pfnYUVFilter = (CaptureYUVEx)pUserCallBack;
			m_pUserYUVFilter = pUserPtr;
		}
			break;
		case FramePaser:
		{
// 			m_pfnCaptureFrame = (CaptureFrame)pUserCallBack;
// 			m_pUserCaptureFrame = pUserPtr;
		}
			break;
		case FilePlayer:
		{
			m_pFilePlayCallBack = (FilePlayProc)pUserCallBack;
			m_pUserFilePlayer = pUserPtr;
		}
			break;
		default:
			return DVO_Error_InvalidParameters;
			break;
		}
		return DVO_Succeed;
	}
	

	/// @brief			ȡ���ļ��ĸ�Ҫ����Ϣ,���ļ���֡��,�ļ�ƫ�Ʊ�
	/// ֻ��ȡ֡ͷ�ķ�ʽ��ȡ֡��Ϣ��ʵ��֤�����ַ���Ч�ʺܵ�
	int GetFileSummary0()
	{
#ifdef _DEBUG
		double dfTimeStart = GetExactTime();
#endif
		if (strlen(m_pszFileName) <= 0 /*|| !PathFileExistsA(m_pszFileName)*/)
			return DVO_Error_FileNotOpened;
		HANDLE hDvoFile = CreateFileA(m_pszFileName,
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
	
		DWORD nBufferSize = sizeof(DVOFrameHeader);
		if (m_nSDKVersion >= DVO_IPC_SDK_VERSION_2015_12_16 && m_nSDKVersion != DVO_IPC_SDK_GSJ_HEADER)
			nBufferSize = sizeof(DVOFrameHeaderEx);
		LARGE_INTEGER liFileSize;
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
		if (!m_pFrameOffsetTable)
		{
			m_pFrameOffsetTable = _New FileFrameInfo[m_nTotalFrames];
			ZeroMemory(m_pFrameOffsetTable, sizeof(FileFrameInfo)*m_nTotalFrames);
		}

		byte *pBuffer = _New byte[nBufferSize];
		shared_ptr<byte>BufferPtr(pBuffer);
		byte *pFrame = nullptr;
		int nFrameSize = 0;
		int nFrames = 0;
		LONG nSeekOffset = 0;
		DWORD nBytesRead = 0;
		DWORD nDataLength = 0;
		byte *pFrameBuffer = nullptr;
		int nFrameOffset = sizeof(DVO_MEDIAINFO);
		bool bIFrame = false;
		bool bStreamProbed = false;		// �Ƿ��Ѿ�̽�������
		DVOFrameHeaderEx *pHeaderEx = nullptr;
		DVOFrameHeader *pHeader = nullptr;
		while (true && m_bThreadSummaryRun)
		{
			if (!ReadFile(hDvoFile, pBuffer, nBufferSize, &nBytesRead, nullptr))
			{
				OutputMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return DVO_Error_ReadFileFailed;
			}
			if (nBytesRead == 0)		// δ��ȡ�κ����ݣ��Ѿ��ﵽ�ļ���β
				break;
			pHeaderEx = (DVOFrameHeaderEx *)pBuffer;
			nFrameSize = pHeaderEx->nLength + nBufferSize;
			if (IsDVOVideoFrame((DVOFrameHeaderEx *)pHeaderEx, bIFrame,m_nSDKVersion))	// ֻ��¼��Ƶ֡���ļ�ƫ��
			{
				if (m_nVideoCodec == CODEC_UNKNOWN &&
					bIFrame &&
					!bStreamProbed)
				{// ����̽������,������������������̽��������ʽ
					pFrameBuffer = _New byte[nFrameSize];
					shared_ptr<byte> FrameBufferPtr(pFrameBuffer);
					memcpy_s(pFrameBuffer, nFrameSize, pHeaderEx, nBufferSize);
					if (!ReadFile(hDvoFile, &pFrameBuffer[nBufferSize], pHeaderEx->nLength, &nBytesRead, nullptr))
					{
						OutputMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
						return DVO_Error_ReadFileFailed;
					}
					bStreamProbed = ProbeStream(pFrameBuffer, nFrameSize);
				}
				if (nFrames < m_nTotalFrames)
				{
					if (m_pFrameOffsetTable)
					{
						m_pFrameOffsetTable[nFrames].nOffset	 = nFrameOffset;
						m_pFrameOffsetTable[nFrames].nFrameSize	 = nFrameSize;
						m_pFrameOffsetTable[nFrames].bIFrame	 = bIFrame;
						// ����֡ID���ļ����ż������ȷ����ÿһ֡�Ĳ���ʱ��
						if (m_nSDKVersion >= DVO_IPC_SDK_VERSION_2015_12_16 && m_nSDKVersion != DVO_IPC_SDK_GSJ_HEADER)
							m_pFrameOffsetTable[nFrames].tTimeStamp = nFrames*m_nFileFrameInterval * 1000;
						else
							m_pFrameOffsetTable[nFrames].tTimeStamp = pHeaderEx->nTimestamp;
					}
				}
				else
					OutputMsg("%s %d(%s) Frame (%d) overflow.\n", __FILE__, __LINE__, __FUNCTION__, nFrames);
				nFrames++;
			}
			else
				m_nAudioCodec = (DVO_CODEC)pHeaderEx->nType;
			nFrameOffset += nFrameSize;
			if (SetFilePointer(hDvoFile, nFrameOffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			{
				OutputMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return DVO_Error_ReadFileFailed;
			}
		}
		m_nTotalFrames = nFrames;
#ifdef _DEBUG
		OutputMsg("%s TimeSpan = %.3f\tnFrames = %d.\n", __FUNCTION__, TimeSpanEx(dfTimeStart),nFrames);
#endif		
		m_bSummaryIsReady = true;
		return DVO_Succeed;
	}
	/// @brief			ȡ���ļ��ĸ�Ҫ����Ϣ,���ļ���֡��,�ļ�ƫ�Ʊ�
	/// �Զ�ȡ����ļ����ݵ���ʽ����ȡ֡��Ϣ��ִ��Ч���ϱ�GetFileSummary0Ҫ��
	/// Ϊ���Ч�ʣ���ֹ�ļ������̺߳͸�Ҫ�߳̾������̶�ȡȨ���ڵ�ȡ��Ҫ��Ϣ��ͬʱ��
	/// �򻺳���Ͷ��16��(400֡)����Ƶ���ݣ��൱�ڽ����߳̿����ӳ�8-10��ſ�ʼ������
	/// �����˾������������ٶ�;���ͬʱ���û����������������棬�������û����飻
	int GetFileSummary()
	{
//#ifdef _DEBUG
		double dfTimeStart = GetExactTime();
//#endif
		if (strlen(m_pszFileName) <= 0 /*|| !PathFileExistsA(m_pszFileName)*/)
			return DVO_Error_FileNotOpened;
		HANDLE hDvoFile = CreateFileA(m_pszFileName,
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
		DWORD nBufferSize = 1024 * 1024*16;
		LARGE_INTEGER liFileSize;		
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
		if (!m_pFrameOffsetTable)
		{
			m_pFrameOffsetTable = _New FileFrameInfo[m_nTotalFrames];
			ZeroMemory(m_pFrameOffsetTable, sizeof(FileFrameInfo)*m_nTotalFrames);
		}
		
		byte *pBuffer = _New byte[nBufferSize];
		while (!pBuffer)
		{
			if (nBufferSize <= 1024 * 512)
			{// ��512K���ڴ涼�޷�����Ļ������˳�
				OutputMsg("%s Can't alloc enough memory.\n", __FUNCTION__);
				assert(false);
				return DVO_Error_InsufficentMemory;
			}
			nBufferSize /= 2;
			pBuffer = _New byte[nBufferSize];
		}
		shared_ptr<byte>BufferPtr(pBuffer);
		byte *pFrame = nullptr;
		int nFrameSize = 0;
		int nVideoFrames = 0;
		int nAudioFrames = 0;
		LONG nSeekOffset = 0;
		DWORD nBytesRead = 0;
		DWORD nDataLength = 0;
		byte *pFrameBuffer = nullptr;
		FrameParser Parser;
		int nFrameOffset = sizeof(DVO_MEDIAINFO);
		bool bIFrame = false;
		bool bStreamProbed = false;		// �Ƿ��Ѿ�̽�������
		const UINT nMaxCache = 100;
		bool bFirstBlockIsFilled = true;
		int nAllFrames = 0;
		//m_bEnableAudio = true;			// �ȿ�����Ƶ��ǣ���������Ƶ����,�����ڹر���Ƶ���򻺴����ݻ��Զ�ɾ��
		
		while (true && m_bThreadSummaryRun)
		{
			double dfT1 = GetExactTime();
			if (!ReadFile(hDvoFile, &pBuffer[nDataLength], (nBufferSize - nDataLength), &nBytesRead, nullptr))
			{
				OutputMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return DVO_Error_ReadFileFailed;
			}
			dfT1 = GetExactTime();
			if (nBytesRead == 0)		// δ��ȡ�κ����ݣ��Ѿ��ﵽ�ļ���β
				break;
			pFrameBuffer = pBuffer;
			nDataLength += nBytesRead;
			int nLength1 = 0;
			while (true && m_bThreadSummaryRun)
			{
				if (!ParserFrame(&pFrameBuffer, nDataLength, &Parser))
					break;
				nAllFrames++;
				nLength1 = 16 * 1024 * 1024 - (pFrameBuffer - pBuffer);
				if (nLength1 != nDataLength)
				{
					int nBreak = 3;
				}
				if (bFirstBlockIsFilled)
				{
					if (InputStream((byte *)Parser.pHeader, Parser.nFrameSize, (UINT)nMaxCache,true) == DVO_Error_FrameCacheIsFulled && m_bThreadSummaryRun)
					{
						SetEvent(m_hCacheFulled);
						m_nSummaryOffset = nFrameOffset;
						EnterCriticalSection(&m_csVideoCache);
						int nVideoSize = m_listVideoCache.size();
						LeaveCriticalSection(&m_csVideoCache);
						EnterCriticalSection(&m_csAudioCache);
						int nAudioSize = m_listAudioCache.size();
						LeaveCriticalSection(&m_csAudioCache);
						m_nHeaderFrameID = m_listVideoCache.front()->FrameHeader()->nFrameID;
						TraceMsgA("HeadFrame ID = %d.\n", m_nHeaderFrameID);
						bFirstBlockIsFilled = false;
					}
				}

				if (IsDVOVideoFrame(Parser.pHeader, bIFrame, m_nSDKVersion))	// ֻ��¼��Ƶ֡���ļ�ƫ��
				{
// 					if (m_nVideoCodec == CODEC_UNKNOWN &&
// 						bIFrame &&
// 						!bStreamProbed)
// 					{// ����̽������
// 						bStreamProbed = ProbeStream((byte *)Parser.pRawFrame, Parser.nRawFrameSize);
// 					}
					if (nVideoFrames < m_nTotalFrames)
					{
						if (m_pFrameOffsetTable)
						{
							m_pFrameOffsetTable[nVideoFrames].nOffset = nFrameOffset;
							m_pFrameOffsetTable[nVideoFrames].nFrameSize = Parser.nFrameSize;
							m_pFrameOffsetTable[nVideoFrames].bIFrame = bIFrame;
							// ����֡ID���ļ����ż������ȷ����ÿһ֡�Ĳ���ʱ��
							if (m_nSDKVersion >= DVO_IPC_SDK_VERSION_2015_12_16 && m_nSDKVersion != DVO_IPC_SDK_GSJ_HEADER)
								m_pFrameOffsetTable[nVideoFrames].tTimeStamp = nVideoFrames*m_nFileFrameInterval * 1000;
							else
								m_pFrameOffsetTable[nVideoFrames].tTimeStamp = Parser.pHeader->nTimestamp;
						}
					}
					else
						OutputMsg("%s %d(%s) Frame (%d) overflow.\n", __FILE__, __LINE__, __FUNCTION__, nVideoFrames);
					nVideoFrames++;
				}
				else
				{
					m_nAudioCodec = (DVO_CODEC)Parser.pHeaderEx->nType;
					nAudioFrames++;
				}

				nFrameOffset += Parser.nFrameSize;
			}
			nOffset += nBytesRead;
// 			if (bFirstBlockIsFilled && m_bThreadSummaryRun)
// 			{
// 				SetEvent(m_hCacheFulled);
// 				m_nSummaryOffset = nFrameOffset;
// 				CAutoLock lock(&m_csVideoCache);
// 				m_nHeaderFrameID = m_listVideoCache.front()->FrameHeader()->nFrameID;
// 				TraceMsgA("VideoCache = %d\tAudioCache = %d.\n", m_listVideoCache.size(), m_listAudioCache.size());
// 				bFirstBlockIsFilled = false;
// 			}
			// �������ݳ�ΪnDataLength
			memcpy(pBuffer, pFrameBuffer, nDataLength);
			ZeroMemory(&pBuffer[nDataLength] ,nBufferSize - nDataLength);
		}
		m_nTotalFrames = nVideoFrames;
//#ifdef _DEBUG
		OutputMsg("%s TimeSpan = %.3f\tnVideoFrames = %d\tnAudioFrames = %d.\n", __FUNCTION__, TimeSpanEx(dfTimeStart), nVideoFrames,nAudioFrames);
//#endif		
		m_bSummaryIsReady = true;
		return DVO_Succeed;
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
		if (Frame(*ppBuffer)->nFrameTag != DVO_TAG &&
			Frame(*ppBuffer)->nFrameTag != GSJ_TAG)
		{
			static char *szKey1 = "MOVD";
			static char *szKey2 = "IMWH";
			nOffset = KMP_StrFind(*ppBuffer, nDataSize, (byte *)szKey1, 4);
			if (nOffset < 0)
				nOffset = KMP_StrFind(*ppBuffer, nDataSize, (byte *)szKey2, 4);
			nOffset -= offsetof(DVOFrameHeader, nFrameTag);
		}

		if (nOffset < 0)
			return false;

		byte *pFrameBuff = *ppBuffer;
		if (m_nSDKVersion < DVO_IPC_SDK_VERSION_2015_12_16 || m_nSDKVersion == DVO_IPC_SDK_GSJ_HEADER)
		{// �ɰ��ļ�
			// ֡ͷ��Ϣ������
			if ((nOffset + sizeof(DVOFrameHeader)) >= nDataSize)
				return false;
			pFrameBuff += nOffset;
			// ֡���ݲ�����
			if (nOffset + FrameSize2(pFrameBuff) >= nDataSize)
				return false;
			if (pFrameParser)
			{
				pFrameParser->pHeader = (DVOFrameHeader *)(pFrameBuff);
				bool bIFrame = false;
// 				if (IsDVOVideoFrame(pFrameParser->pHeaderEx, bIFrame,m_nSDKVersion))
// 					OutputMsg("Frame ID:%d\tType = Video:%d.\n", pFrameParser->pHeaderEx->nFrameID, pFrameParser->pHeaderEx->nType);
// 				else
// 					OutputMsg("Frame ID:%d\tType = Audio:%d.\n", pFrameParser->pHeaderEx->nFrameID, pFrameParser->pHeaderEx->nType);
				pFrameParser->nFrameSize = FrameSize2(pFrameBuff);
				pFrameParser->pRawFrame = *ppBuffer + sizeof(DVOFrameHeader);
				pFrameParser->nRawFrameSize = Frame2(pFrameBuff)->nLength;
			}
			nDataSize -= (FrameSize2(pFrameBuff) + nOffset);
			pFrameBuff += FrameSize2(pFrameBuff);
		}
		else
		{// �°��ļ�
			// ֡ͷ��Ϣ������
			if ((nOffset + sizeof(DVOFrameHeaderEx)) >= nDataSize)
				return false;

			pFrameBuff += nOffset;
		
			// ֡���ݲ�����
			if (nOffset + FrameSize(pFrameBuff) >= nDataSize)
				return false;
		
			if (pFrameParser)
			{
				pFrameParser->pHeaderEx = (DVOFrameHeaderEx *)pFrameBuff;
				bool bIFrame = false;
// 				if (IsDVOVideoFrame(pFrameParser->pHeaderEx, bIFrame,m_nSDKVersion))
// 					OutputMsg("Frame ID:%d\tType = Video:%d.\n", pFrameParser->pHeaderEx->nFrameID, pFrameParser->pHeaderEx->nType);
// 				else
// 					OutputMsg("Frame ID:%d\tType = Audio:%d.\n", pFrameParser->pHeaderEx->nFrameID, pFrameParser->pHeaderEx->nType);
				pFrameParser->nFrameSize = FrameSize(pFrameBuff);
				pFrameParser->pRawFrame = pFrameBuff + sizeof(DVOFrameHeaderEx);
				pFrameParser->nRawFrameSize = Frame(pFrameBuff)->nLength;
			}
			nDataSize -= (FrameSize(pFrameBuff) + nOffset);
			pFrameBuff += FrameSize(pFrameBuff);
		}
		*ppBuffer = pFrameBuff;
		return true;
	}

	///< @brief ��Ƶ�ļ������߳�
	static UINT __stdcall ThreadParser(void *p)
	{// ��ָ������Ч�Ĵ��ھ������ѽ�������ļ����ݷ��벥�Ŷ��У����򲻷��벥�Ŷ���
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
			pThis->OutputMsg("%s SetFilePointer Failed,Error = %d.\n", __FUNCTION__, GetLastError());
			assert(false);
			return 0;
		}
#ifdef _DEBUG
		double dfT1 = GetExactTime();
		bool bOuputTime = false;
#endif
		DVOFrameHeaderEx HeaderEx;
		int nInputResult = 0;
		while (pThis->m_bThreadParserRun)
		{
			if (WaitForSingleObject(pThis->m_hCacheFulled,20) == WAIT_TIMEOUT)
				continue;
			if (pThis->m_bPause)
			{
				Sleep(20);
				continue;
			}
			if (pThis->m_nSummaryOffset)
			{
				CAutoLock lock(&pThis->m_csVideoCache);
				if (pThis->m_listVideoCache.size() < pThis->m_nMaxFrameCache)
				{
					if (SetFilePointer(pThis->m_hDvoFile, (LONG)pThis->m_nSummaryOffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
						pThis->OutputMsg("%s SetFilePointer Failed,Error = %d.\n", __FUNCTION__, GetLastError());
					pThis->m_nSummaryOffset = 0;
					lock.Unlock();
					Sleep(20);
				}
				else
				{
					lock.Unlock();
					Sleep(20);
					continue;
				}
			}
			else if (nSeekOffset = pThis->GetSeekOffset())	// �Ƿ���Ҫ�ƶ��ļ�ָ��,��nSeekOffset��Ϊ0������Ҫ�ƶ��ļ�ָ��
			{
				pThis->OutputMsg("Detect SeekFrame Operation.\n");

				EnterCriticalSection(&pThis->m_csVideoCache);
				pThis->m_listVideoCache.clear();
				LeaveCriticalSection(&pThis->m_csVideoCache);

				EnterCriticalSection(&pThis->m_csAudioCache);
				pThis->m_listAudioCache.clear();
				LeaveCriticalSection(&pThis->m_csAudioCache);

				pThis->SetSeekOffset(0);
				nDataLength = 0;

#ifdef _DEBUG
				pThis->m_bSeekSetDetected = true;
#endif
				if (SetFilePointer(pThis->m_hDvoFile, (LONG)nSeekOffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)				
					pThis->OutputMsg("%s SetFilePointer Failed,Error = %d.\n", __FUNCTION__, GetLastError());
			}
			if (!ReadFile(pThis->m_hDvoFile, &pBuffer[nDataLength], (nBufferSize - nDataLength), &nBytesRead, nullptr))
			{
				pThis->OutputMsg("%s ReadFile Failed,Error = %d.\n", __FUNCTION__, GetLastError());
				return 0;
			}
			nDataLength += nBytesRead;
			byte *pFrameBuffer = pBuffer;

			bool bIFrame = false;
			bool bFrameInput = true;
			while (pThis->m_bThreadParserRun)
			{
				if (pThis->m_bPause)		// ͨ��pause ��������ͣ���ݶ�ȡ
				{
					Sleep(20);
					continue;
				}
				if (bFrameInput)
				{
					bFrameInput = false;
					if (!pThis->ParserFrame(&pFrameBuffer, nDataLength, &Parser))
						break;	
					nInputResult = pThis->InputStream((byte *)Parser.pHeader, Parser.nFrameSize);
					switch (nInputResult)
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
				else
				{
					nInputResult = pThis->InputStream((byte *)Parser.pHeader, Parser.nFrameSize);
					switch (nInputResult)
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
// 			if (!pThis->m_hWnd )
// 			{
// 				Sleep(10);
// 			}
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

	// ̽����Ƶ��������,Ҫ���������I֡
	bool ProbeStream(byte *szFrameBuffer,int nBufferLength)
	{	
		shared_ptr<CVideoDecoder>pDecodec = make_shared<CVideoDecoder>();
		if (!m_pStreamProbe)
			m_pStreamProbe = make_shared<StreamProbe>();
		
		if (!m_pStreamProbe)
			return false;
		m_pStreamProbe->nProbeCount++;
		m_pStreamProbe->GetProbeStream(szFrameBuffer, nBufferLength);
		if (m_pStreamProbe->nProbeDataLength <= 128)
			return false;
		if (pDecodec->ProbeStream(this, ReadAvData, m_nMaxFrameSize) != 0)
		{
			OutputMsg("%s Failed in ProbeStream,you may need to input more stream.\n", __FUNCTION__);
			assert(false);
			return false;
		}
		pDecodec->CancelProbe();	
		if (pDecodec->m_nCodecId == AV_CODEC_ID_NONE)
		{
			OutputMsg("%s Unknown Video Codec or not found any codec in the stream.\n", __FUNCTION__);
			assert(false);
			return false;
		}

		if (!pDecodec->m_pAVCtx->width || !pDecodec->m_pAVCtx->height)
		{
			assert(false);
			return false;
		}
		if (pDecodec->m_nCodecId == AV_CODEC_ID_H264)
		{
			m_nVideoCodec = CODEC_H264;
			OutputMsg("%s Video Codec:%H.264 Width = %d\tHeight = %d.\n", __FUNCTION__, pDecodec->m_pAVCtx->width, pDecodec->m_pAVCtx->height);
		}
		else if (pDecodec->m_nCodecId == AV_CODEC_ID_HEVC)
		{
			m_nVideoCodec = CODEC_H265;
			OutputMsg("%s Video Codec:%H.265 Width = %d\tHeight = %d.\n", __FUNCTION__, pDecodec->m_pAVCtx->width, pDecodec->m_pAVCtx->height);
		}
		else
		{
			m_nVideoCodec = CODEC_UNKNOWN;
			OutputMsg("%s Unsupported Video Codec.\n", __FUNCTION__);
			assert(false);
			return false;
		}
		m_pStreamProbe->nProbeAvCodecID = pDecodec->m_nCodecId;
		m_pStreamProbe->nProbeWidth = pDecodec->m_pAVCtx->width;
		m_pStreamProbe->nProbeHeight = pDecodec->m_pAVCtx->height;
		m_nVideoHeight = pDecodec->m_pAVCtx->height;
		m_nVideoWidth = pDecodec->m_pAVCtx->width;
		return true;
	}

	static UINT __stdcall ThreadGetFileSummary(void *p)
	{
		CDvoPlayer* pThis = (CDvoPlayer *)p;
		if (pThis->m_nTotalFrames == 0)
		{
			assert(false);
			return 0;
		}
		//pThis->GetFileSummary0();
		pThis->GetFileSummary();
		
		pThis->OutputMsg("%s %d(%s) Exit.\n",__FILE__,__LINE__, __FUNCTION__);
		return 0;
	}
			
	/// @brief ��NV12ͼ��ת��ΪYUV420Pͼ��
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

	/// @brief ��DxvaӲ����֡ת����YUV420ͼ��
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
			OutputMsg("%s IDirect3DSurface9::LockRect failed:hr = %08.\n", __FUNCTION__, hr);
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

	// ��YUVC420P֡���Ƶ�YV12������
	void CopyFrameYUV420(byte *pYUV420, int nYUV420Size, AVFrame *pFrame420P)
	{
		byte *pDest = pYUV420;
		int nStride = pFrame420P->width;
		int nSize = nStride * nStride;
		int nHalfSize = (nSize) >> 1;
		byte *pDestY = pDest;										// Y������ʼ��ַ

		byte *pDestU = pDest + nSize;								// U������ʼ��ַ
		int nSizeofU = nHalfSize >> 1;

		byte *pDestV = pDestU + (size_t)(nHalfSize >> 1);			// V������ʼ��ַ
		int nSizeofV = nHalfSize >> 1;

		// YUV420P��U��V�����Ե������ΪYV12��ʽ
		// ����Y����
		for (int i = 0; i < pFrame420P->height; i++)
			memcpy_s(pDestY + i * nStride, nSize * 3 / 2 - i*nStride, pFrame420P->data[0] + i * pFrame420P->linesize[0], pFrame420P->width);

		// ����YUV420P��U������Ŀ���YV12��U����
		for (int i = 0; i < pFrame420P->height / 2; i++)
			memcpy_s(pDestU + i * nStride / 2, nSizeofU - i*nStride / 2, pFrame420P->data[1] + i * pFrame420P->linesize[1], pFrame420P->width / 2);

		// ����YUV420P��V������Ŀ���YV12��V����
		for (int i = 0; i < pFrame420P->height / 2; i++)
			memcpy_s(pDestV + i * nStride / 2, nSizeofV - i*nStride / 2, pFrame420P->data[2] + i * pFrame420P->linesize[2], pFrame420P->width / 2);
	}
	void ProcessYUVFilter(AVFrame *pAvFrame, LONGLONG nTimestamp)
	{
		if (m_pfnYUVFilter)
		{// ��m_pfnYUVFileter�У��û���Ҫ��YUV���ݴ���֣��ٷֳ�YUV����
			if (pAvFrame->format == AV_PIX_FMT_DXVA2_VLD)
			{// dxva Ӳ����֡
				CopyDxvaFrame(m_pYUV, pAvFrame);
				byte* pU = m_pYUV + pAvFrame->width*pAvFrame->height;
				byte* pV = m_pYUV + pAvFrame->width*pAvFrame->height / 4;
				m_pfnYUVFilter(this,
								m_pYUV,
								pU,
								pV,
								pAvFrame->linesize[0],
								pAvFrame->linesize[1],
								pAvFrame->width,
								pAvFrame->height,
								nTimestamp,
								m_pUserYUVFilter);
			}
			else
				m_pfnYUVFilter(this,
								pAvFrame->data[0],
								pAvFrame->data[1],
								pAvFrame->data[2],
								pAvFrame->linesize[0],
								pAvFrame->linesize[1],
								pAvFrame->width,
								pAvFrame->height,
								nTimestamp,
								m_pUserYUVFilter);
		}
	}
	
	void ProcessYVUCapture(AVFrame *pAvFrame,LONGLONG nTimestamp)
	{
		if (m_pfnCaptureYUV)
		{
			if (!m_pYUV)
			{
				m_nYUVSize = pAvFrame->width * pAvFrame->height * 3 / 2;
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
				memcpy_s(m_pYUV, m_nYUVSize, pAvFrame->data[0], nPicSize);
				memcpy_s(&m_pYUV[nPicSize], m_nYUVSize - nPicSize, pAvFrame->data[1], nPicSize / 4);
				memcpy_s(&m_pYUV[nPicSize + nPicSize / 4], m_nYUVSize - nPicSize - nPicSize / 4, pAvFrame->data[2], nPicSize / 4);
			}
			TraceMsgA("%s m_pfnCaptureYUV = %p", __FUNCTION__, m_pfnCaptureYUV);
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
		
		int nReturnVal = buf_size;
		int nRemainedLength = 0;
		pAvQueue->pAvBuffer = buf;
		if (!pThis->m_pStreamProbe)
			return 0;
		int &nDataLength = pThis->m_pStreamProbe->nProbeDataRemained;
		byte *pProbeBuff = pThis->m_pStreamProbe->pProbeBuff;
		int &nProbeOffset = pThis->m_pStreamProbe->nProbeOffset;
		if (nDataLength > 0)
		{
			nRemainedLength = nDataLength - nProbeOffset;
			if (nRemainedLength > buf_size)
			{
				memcpy_s(buf,buf_size, &pProbeBuff[nProbeOffset], buf_size);
				nProbeOffset += buf_size;
				nDataLength -= buf_size;
			}
			else
			{
				memcpy_s(buf, buf_size, &pProbeBuff[nProbeOffset], nRemainedLength);
				nDataLength -= nRemainedLength;
				nProbeOffset = 0;
				nReturnVal = nRemainedLength;
			}
			return nReturnVal;
		}
		else
			return 0;
	}

	/// @brief			ʵ��ָʱʱ������ʱ
	/// @param [in]		dwDelay		��ʱʱ������λΪms
	/// @param [in]		bStopFlag	����ֹͣ�ı�־,Ϊfalseʱ����ֹͣ��ʱ
	/// @param [in]		nSleepGranularity	��ʱʱSleep������,����ԽС����ֹ��ʱԽѸ�٣�ͬʱҲ����CPU����֮��Ȼ
	static void Delay(DWORD dwDelay, volatile bool &bStopFlag, WORD nSleepGranularity = 20)
	{
		DWORD dwTotal = 0;
		while (bStopFlag && dwTotal < dwDelay)
		{
			Sleep(nSleepGranularity);
			dwTotal += nSleepGranularity;
		}
	}
	struct DecodeFrameStat
	{
		int nTotalFrame;			// ������֡��
		double dfTotalDecodeTime;	// ������ʱ��
		double dfAvgDecodeTime;		// ƽ������ʱ��
		DecodeFrameStat()
		{
			ZeroMemory(this, sizeof(DecodeFrameStat));
		}
		void Stat(double dfTimespan)
		{
			nTotalFrame++;
			dfTotalDecodeTime += dfTimespan;
			dfAvgDecodeTime = dfTotalDecodeTime / nTotalFrame;
		}
	};

	
	
	static UINT __stdcall ThreadPlayVideo(void *p)
	{
		CDvoPlayer* pThis = (CDvoPlayer *)p;
#ifdef _DEBUG
		pThis->OutputMsg("%s \tObject:%d Enter ThreadPlayVideo m_nLifeTime = %d.\n", __FUNCTION__, pThis->m_nObjIndex, timeGetTime() - pThis->m_nLifeTime);
#endif
		int nAvError = 0;
		char szAvError[1024] = { 0 };
#ifdef _DEBUG
		sprintf_s(szAvError, 1024, "Object:%d.\n", pThis->m_nObjIndex);
		TraceFunction1(szAvError);
#endif
		if (!pThis->m_hWnd)
		{
			pThis->OutputMsg("%s Warning!!!A Windows handle is Needed otherwith the video Will not showed..\n", __FUNCTION__);
			//assert(false);
			//return 0;
		}	
		shared_ptr<CVideoDecoder>pDecodec = make_shared<CVideoDecoder>();
		if (!pDecodec)
		{
			pThis->OutputMsg("%s Failed in allocing memory for Decoder.\n", __FUNCTION__);
			//assert(false);
			return 0;
		}
		// �ȴ�I֡
		long tFirst = timeGetTime();
		DWORD dfTimeout = 3000;
		if (!pThis->m_bIpcStream)	// ֻ��IPC��������Ҫ��ʱ��ȴ�
			dfTimeout = 1000;
		AVCodecID nCodecID = AV_CODEC_ID_NONE;
		int nDiscardFrames = 0;
		bool bProbeSucced = false;
		if (pThis->m_nVideoCodec == CODEC_UNKNOWN ||		/// ����δ֪����̽����
			!pThis->m_nVideoWidth||
			!pThis->m_nVideoHeight)
		{
			while (!bProbeSucced && pThis->m_bThreadPlayVideoRun)
			{
				if ((timeGetTime() - tFirst) < dfTimeout)
				{
					Sleep(5);
					CAutoLock lock(&pThis->m_csVideoCache, false, __FILE__, __FUNCTION__, __LINE__);
					auto it = find_if(pThis->m_listVideoCache.begin(), pThis->m_listVideoCache.end(), StreamFrame::IsIFrame);
					if (it != pThis->m_listVideoCache.end())
					{// ̽����������
						if (bProbeSucced = pThis->ProbeStream((byte *)(*it)->Framedata(pThis->m_nSDKVersion), (*it)->FrameHeader()->nLength))
							break;
					}
				}
				else
				{
#ifdef _DEBUG
					pThis->OutputMsg("%s Warning!!!\nNot receive an I frame in %d second.m_listVideoCache.size() = %d.\n", __FUNCTION__, (int)dfTimeout / 1000, pThis->m_listVideoCache.size());
					pThis->OutputMsg("%s \tObject:%d Line %d Time = %d.\n", __FUNCTION__, pThis->m_nObjIndex, __LINE__, timeGetTime() - pThis->m_nLifeTime);
#endif
					if (pThis->m_hWnd)
						::PostMessage(pThis->m_hWnd, WM_DVOPLAYER_MESSAGE, DVOPLAYER_NOTRECVIFRAME, 0);
					return 0;
				}
			}
			if (!pThis->m_bThreadPlayVideoRun)
				return 0;
			
			if (!bProbeSucced)		// ̽��ʧ��
			{
				pThis->OutputMsg("%s Failed in ProbeStream,you may input a unknown stream.\n", __FUNCTION__);
#ifdef _DEBUG
				pThis->OutputMsg("%s \tObject:%d Line %d Time = %d.\n", __FUNCTION__, pThis->m_nObjIndex, __LINE__, timeGetTime() - pThis->m_nLifeTime);
#endif
				if (pThis->m_hWnd)
					::PostMessage(pThis->m_hWnd, WM_DVOPLAYER_MESSAGE, DVOPLAYER_UNKNOWNSTREAM, 0);
				return 0;
			}
			// ��ffmpeg������IDתΪDVO������ID,����ֻ֧��H264��HEVC
			nCodecID = pThis->m_pStreamProbe->nProbeAvCodecID;
			if (nCodecID != AV_CODEC_ID_H264 && 
				nCodecID != AV_CODEC_ID_HEVC)
			{
				pThis->m_nVideoCodec = CODEC_UNKNOWN;
				pThis->OutputMsg("%s Probed a unknown stream,Decode thread exit.\n", __FUNCTION__);
				if (pThis->m_hWnd)
					::PostMessage(pThis->m_hWnd, WM_DVOPLAYER_MESSAGE, DVOPLAYER_UNKNOWNSTREAM, 0);
				return 0;
			}
		}
		
		switch (pThis->m_nVideoCodec)
		{
		case CODEC_H264:
			nCodecID = AV_CODEC_ID_H264;
			break;
		case CODEC_H265:
			nCodecID = AV_CODEC_ID_H265;
			break;
		default:
		{
			pThis->OutputMsg("%s You Input a unknown stream,Decode thread exit.\n", __FUNCTION__);
			if (pThis->m_hWnd)	// ���߳��о�������ʹ��SendMessage����Ϊ���ܻᵼ������
				::PostMessage(pThis->m_hWnd, WM_DVOPLAYER_MESSAGE, DVOPLAYER_UNSURPPORTEDSTREAM, 0);
			return 0;
			break;
		}
		}
		
		int nRetry = 0;
		// ʹ�õ��߳̽���,���߳̽�����ĳ�˱Ƚ�����CPU�Ͽ��ܻ����Ч����������I5 2GHZ���ϵ�CPU�ϵĶ��߳̽���Ч���������Է�����ռ�ø�����ڴ�
		pDecodec->SetDecodeThreads(1);		
		// ��ʼ��������
		while (pThis->m_bThreadPlayVideoRun )
		{// ĳ��ʱ����ܻ���Ϊ�ڴ����Դ�������³�ʼ�����������,��˿����ӳ�һ��ʱ����ٴγ�ʼ��������γ�ʼ���Բ��ɹ��������˳��߳�
			if (!pDecodec->InitDecoder(nCodecID, pThis->m_nVideoWidth, pThis->m_nVideoHeight, pThis->m_bEnableHaccel))
			{
				pThis->OutputMsg("%s Failed in Initializing Decoder.\n", __FUNCTION__);
#ifdef _DEBUG
				pThis->OutputMsg("%s \tObject:%d Line %d Time = %d.\n", __FUNCTION__, pThis->m_nObjIndex, __LINE__, timeGetTime() - pThis->m_nLifeTime);
#endif
				nRetry++;
				if (nRetry >= 3)
				{
					if (pThis->m_hWnd)// ���߳��о�������ʹ��SendMessage����Ϊ���ܻᵼ������
						::PostMessage(pThis->m_hWnd, WM_DVOPLAYER_MESSAGE, DVOPLAYER_INITDECODERFAILED, 0);
					return 0;
				}
				Delay(2500,pThis->m_bThreadPlayVideoRun);
			}
			else
				break;
		}
		if (!pThis->m_bThreadPlayVideoRun)
			return 0;

		// ����ָ���˴��ھ��������Ҫ��ʼ��ʾ����
		if (pThis->m_hWnd)
		{
			bool bCacheDxSurface = false;		// �Ƿ�Ϊ������ȡ�õ�Surface����
			if (GetOsMajorVersion() < 6)
			{
				if (!pThis->m_pDDraw)
					pThis->m_pDDraw = _New CDirectDraw();
				pThis->InitizlizeDisplay();
			}
			else
			{
				if (!pThis->m_pDxSurface)
				{
					D3DFORMAT nPixFormat = (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2');
					if (pThis->m_bEnableHaccel)
						nPixFormat = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
					pThis->m_pDxSurface = GetDxInCache(pThis->m_nVideoWidth, pThis->m_nVideoHeight, nPixFormat);
					if (pThis->m_pDxSurface)
						bCacheDxSurface = true;
					else
						pThis->m_pDxSurface = _New CDxSurfaceEx();
				}
				if (!bCacheDxSurface)
				{
					nRetry = 0;
					while (pThis->m_bThreadPlayVideoRun)
					{
						if (!pThis->InitizlizeDisplay())
						{
							nRetry++;
							Delay(2500, pThis->m_bThreadPlayVideoRun);
							if (nRetry >= 3)
							{
								if (pThis->m_hWnd)
									::PostMessage(pThis->m_hWnd, WM_DVOPLAYER_MESSAGE, DVOPLAYER_INITD3DFAILED, 0);
								return 0;
							}
						}
						else
							break;
					}
				}
			}

			RECT rtWindow;
			GetWindowRect(pThis->m_hWnd, &rtWindow);
			pThis->OutputMsg("%s Window Width = %d\tHeight = %d.\n", __FUNCTION__, (rtWindow.right - rtWindow.left), (rtWindow.bottom - rtWindow.top));
#ifdef _DEBUG
			pThis->OutputMsg("%s \tObject:%d Line %d Time = %d.\n", __FUNCTION__, pThis->m_nObjIndex, __LINE__, timeGetTime() - pThis->m_nLifeTime);
#endif
		}
		
		if (pThis->m_pStreamProbe)
			pThis->m_pStreamProbe = nullptr;
		pThis->m_pDecodec = pDecodec;
	
		AVPacket *pAvPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
		shared_ptr<AVPacket>AvPacketPtr(pAvPacket, av_free);	
		av_init_packet(pAvPacket);	
		AVFrame *pAvFrame = av_frame_alloc();
		shared_ptr<AVFrame>AvFramePtr(pAvFrame, av_free);
		
		StreamFramePtr FramePtr;
		int nGot_picture = 0;
		DWORD nResult = 0;
		float fTimeSpan = 0;
		int nFrameInterval = pThis->m_nFileFrameInterval;
		pThis->m_dfTimesStart = GetExactTime();
		
//		 ȡ�õ�ǰ��ʾ����ˢ����,�ڴ�ֱͬ��ģʽ��,��ʾ����ˢ���ʾ����ˣ���ʾͼ������֡��
//		 ͨ��ͳ��ÿ��ʾһ֡ͼ��(���������ʾ)�ķѵ�ʱ��
// 		DEVMODE   dm;
// 		dm.dmSize = sizeof(DEVMODE);
// 		::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);		
// 		int nRefreshInvertal = 1000 / dm.dmDisplayFrequency;	// ��ʾ��ˢ�¼��

		double dfDecodeStartTime = GetExactTime();
		double dfRenderTime = GetExactTime() - pThis->m_fPlayInterval;	// ͼ����ʾ��ʱ��
		double dfRenderStartTime = 0.0f;
		double dfRenderTimeSpan = 0.000f;
		double dfTimeSpan = 0.0f;
		
#ifdef _DEBUG
		EnterCriticalSection(&pThis->m_csVideoCache);
		TraceMsgA("%s Video cache Size = %d .\n", __FUNCTION__, pThis->m_listVideoCache.size());
		LeaveCriticalSection(&pThis->m_csVideoCache);
		pThis->OutputMsg("%s \tObject:%d Start Decoding.\n", __FUNCTION__,pThis->m_nObjIndex);
#endif
//	    ���´������Բ��Խ������ʾռ��ʱ�䣬���鲻Ҫɾ��		
// 		TimeTrace DecodeTimeTrace("DecodeTime", __FUNCTION__);
// 		TimeTrace RenderTimeTrace("RenderTime", __FUNCTION__);

#ifdef _DEBUG
		int nFrames = 0;
		int nFirstID = 0;
		time_t dfDelayArray[100] = { 0 };
#endif
		int nIFrameTime = 0;
		DecodeFrameStat FrameStat;		// ����ͳ��
		DecodeFrameStat IFrameStat;		// I֡����ͳ��

		int nFramesAfterIFrame = 0;		// ���I֡�ı��,I֡��ĵ�һ֡Ϊ1���ڶ�֡Ϊ2��������
		int nSkipFrames = 0;
		bool bDecodeSucceed = false;
		double dfDecodeTimespan = 0.0f;	// �������ķ�ʱ��
		double dfDecodeITimespan = 0.0f; // I֡�������ʾ���ķ�ʱ��
		double dfTimeDecodeStart = 0.0f;
		while (pThis->m_bThreadPlayVideoRun)
		{
			if (pThis->m_bPause)
			{
				Sleep(100);
				continue;
			}
			dfDecodeStartTime = GetExactTime();
			if (!pThis->m_bIpcStream)
			{// �ļ�����ý�岥�ţ��ɵ��ڲ����ٶ�
				bool bPopFrame = false;
				// ����ʱ������ƥ���֡,��ɾ����ƥ��ķ�I֡
				int nSkipFrames = 0;
				CAutoLock lock(&pThis->m_csVideoCache, false, __FILE__, __FUNCTION__, __LINE__);
				for (auto it = pThis->m_listVideoCache.begin(); it != pThis->m_listVideoCache.end();)
				{
					time_t tFrameSpan = ((*it)->FrameHeader()->nTimestamp - pThis->m_tLastFrameTime) / 1000;
					if (tFrameSpan / pThis->m_fPlayRate >= max(pThis->m_fPlayInterval/**pThis->m_fPlayRate*/, FrameStat.dfAvgDecodeTime * 1000)
						|| StreamFrame::IsIFrame(*it))
					{
						bPopFrame = true;
						break;
					}
					else
					{
						it = pThis->m_listVideoCache.erase(it);
						nSkipFrames++;
					}
				}
				if (nSkipFrames)
					pThis->OutputMsg("%s Skip Frames = %d bPopFrame = %s.\n", __FUNCTION__, nSkipFrames, bPopFrame ? "true" : "false");
				if (bPopFrame)
				{
					FramePtr = pThis->m_listVideoCache.front();
					pThis->m_listVideoCache.pop_front();
					//TraceMsgA("%s Pop a Frame ,FrameID = %d\tFrameTimestamp = %d.\n", __FUNCTION__, FramePtr->FrameHeader()->nFrameID, FramePtr->FrameHeader()->nTimestamp);
				}
				pThis->m_nVideoCache = pThis->m_listVideoCache.size();
				if (!bPopFrame)
				{
					lock.Unlock();	// ����ǰ��������ȻSleep��Ż���������������ط�����
					Sleep(10);
					continue;
				}
				lock.Unlock();
				pAvPacket->data = (uint8_t *)FramePtr->Framedata(pThis->m_nSDKVersion);
				pAvPacket->size = FramePtr->FrameHeader()->nLength;
				pThis->m_tLastFrameTime = FramePtr->FrameHeader()->nTimestamp;
				nAvError = pDecodec->Decode(pAvFrame, nGot_picture, pAvPacket);
				if (nAvError < 0)
				{
					av_strerror(nAvError, szAvError, 1024);
					dfDecodeStartTime = GetExactTime();
					continue;
				}
				av_packet_unref(pAvPacket);
				dfDecodeTimespan = TimeSpanEx(dfDecodeStartTime);
				if (StreamFrame::IsIFrame(FramePtr))			// ͳ��I֡����ʱ��
					IFrameStat.Stat(dfDecodeTimespan);
				FrameStat.Stat(dfDecodeTimespan);	// ͳ������֡����ʱ��
				fTimeSpan = (TimeSpanEx(dfRenderTime) + dfRenderTimeSpan )* 1000;
				int nSleepTime = 0;
				if (fTimeSpan  < pThis->m_fPlayInterval)
				{
					nSleepTime =(int) (pThis->m_fPlayInterval - fTimeSpan);
					Sleep(nSleepTime);
				}
			}
			else
			{// IPC ��������ֱ�Ӳ���
				bool bPopFrame = false;
				CAutoLock lock(&pThis->m_csVideoCache);		
				if (pThis->m_listVideoCache.size() > 0)
				{
					FramePtr = pThis->m_listVideoCache.front();
					pThis->m_listVideoCache.pop_front();
					bPopFrame = true;
				}
				lock.Unlock();
				if (!bPopFrame)
				{
					Sleep(10);
					continue;
				}
				pAvPacket->data = (uint8_t *)FramePtr->Framedata(pThis->m_nSDKVersion);
				pAvPacket->size = FramePtr->FrameHeader()->nLength;
				pThis->m_tLastFrameTime = FramePtr->FrameHeader()->nTimestamp;
				nAvError = pDecodec->Decode(pAvFrame, nGot_picture, pAvPacket);
				if (nAvError < 0)
				{
					av_strerror(nAvError, szAvError, 1024);
					dfDecodeTimespan = TimeSpanEx(dfDecodeStartTime);
					dfDecodeStartTime = GetExactTime();
					continue;
				}
				av_packet_unref(pAvPacket);
				dfDecodeTimespan = TimeSpanEx(dfDecodeStartTime);
			}
//			���´������Բ��Խ���ռ��ʱ�䣬���鲻Ҫɾ��	
// 			if (pThis->m_nVideoWidth > 1024)
// 			{
// 				DecodeTimeTrace.AddTime(dfDecodeTimespan);
// 				if (DecodeTimeTrace.IsFull())
// 					DecodeTimeTrace.OutputTime(0.005f);
// 			}
#ifdef _DEBUG
			if (pThis->m_bSeekSetDetected)
			{
				int nFrameID = FramePtr->FrameHeader()->nFrameID;
				int nTimeStamp = FramePtr->FrameHeader()->nTimestamp/1000;
				pThis->OutputMsg("%s First Frame after SeekSet:ID = %d\tTimeStamp = %d.\n", __FUNCTION__, nFrameID, nTimeStamp);
				pThis->m_bSeekSetDetected = false;
			}
#endif
// ��Ϊ���֡�ʲ��Դ���,���鲻Ҫɾ��
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
// 				OutputMsg("%sPlay Interval:\n", __FUNCTION__);
// 				for (int i = 0; i < nPlayCount; i++)
// 				{
// 					OutputMsg("%02d\t", TPlayArray[i]);
// 					if ((i + 1) % 10 == 0)
// 						OutputMsg("\n");
// 				}
// 				OutputMsg(".\n");
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
// 			pAvPacket->data = (uint8_t *)FramePtr->Framedata(pThis->m_nSDKVersion);
// 			pAvPacket->size = FramePtr->FrameHeader()->nLength;
			dfRenderStartTime = GetExactTime();
 			if (nGot_picture)
 			{
				pThis->m_nDecodePirFmt = (AVPixelFormat)pAvFrame->format;
				SetEvent(pThis->m_hEvnetYUVReady);
				SetEvent(pThis->m_hEventDecodeStart);
 				pThis->m_nCurVideoFrame = FramePtr->FrameHeader()->nFrameID;
 				pThis->m_tCurFrameTimeStamp = FramePtr->FrameHeader()->nTimestamp;
				pThis->ProcessYUVFilter(pAvFrame, (LONGLONG)pThis->m_nCurVideoFrame);

				if (!pThis->m_bIpcStream &&
					1.0f == pThis->m_fPlayRate  && 
					pThis->m_bEnableAudio && 
					pThis->m_hAudioFrameEvent[0] &&
					pThis->m_hAudioFrameEvent[1])
					WaitForMultipleObjects(2, pThis->m_hAudioFrameEvent, TRUE, 40);
				pThis->RenderFrame(pAvFrame);
				if (!bDecodeSucceed)
				{
					bDecodeSucceed = true;
#ifdef _DEBUG
					pThis->OutputMsg("%s \tObject:%d  SetEvent Snapshot  m_nLifeTime = %d.\n", __FUNCTION__, pThis->m_nObjIndex, timeGetTime() - pThis->m_nLifeTime);
#endif
				}

				pThis->ProcessSnapshotRequire(pAvFrame);
				pThis->ProcessYVUCapture(pAvFrame, (LONGLONG)pThis->m_nCurVideoFrame);
 				if (pThis->m_pFilePlayCallBack)
 					pThis->m_pFilePlayCallBack(pThis, pThis->m_pUserFilePlayer);
 			}
			else
			{
#ifdef _DEBUG
				TraceMsgA("%s Decode Succeed but Not get a picture ,FrameID = %d\tFrameTimestamp = %d.\n", __FUNCTION__, FramePtr->FrameHeader()->nFrameID, FramePtr->FrameHeader()->nTimestamp);
#endif
			}
			av_frame_unref(pAvFrame);
			dfRenderTimeSpan = TimeSpanEx(dfRenderStartTime);
			dfRenderTime = GetExactTime();
//#ifdef _DEBUG
//			���´������Բ�����ʾռ��ʱ�䣬���鲻Ҫɾ��	
// 			if (pThis->m_nVideoWidth > 1024)
// 			{
// 				fTimeSpan = TimeSpanEx(dfDecodeStartTime) * 1000;
// 				RenderTimeTrace.AddTime(dfRenderTimeSpan);
// 				if (RenderTimeTrace.IsFull())
// 					RenderTimeTrace.OutputTime(0.005f);
// 			}
//#endif
		}
		return 0;
	}
	static UINT __stdcall ThreadPlayAudio(void *p)
	{
		CDvoPlayer *pThis = (CDvoPlayer *)p;
		int nAudioFrameInterval = pThis->m_fPlayInterval / 2;

		DWORD nResult = 0;
		int nTimeSpan = 0;
		StreamFramePtr FramePtr;
		int nAudioError = 0;
		byte *pPCM = nullptr;
		shared_ptr<CAudioDecoder> pAudioDecoder = make_shared<CAudioDecoder>();
		int nPCMSize = 0;
		int nDecodeSize = 0;
		__int64 nFrameEvent = 0;

		// Ԥ����һ֡���Գ�ʼ����Ƶ������
		while (pThis->m_bThreadPlayAudioRun)
		{
			if (!FramePtr)
			{
				CAutoLock lock(&pThis->m_csAudioCache, false, __FILE__, __FUNCTION__, __LINE__);
				if (pThis->m_listAudioCache.size() > 0)
				{
					FramePtr = pThis->m_listAudioCache.front();
					break;
				}
			}
			Sleep(10);
		}
		if (!FramePtr)
			return 0;
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
				pThis->OutputMsg("%s Audio Codec:G711A.\n", __FUNCTION__);
				break;
			}
			case FRAME_G711U:			//711 U�ɱ���֡
			{
				pAudioDecoder->SetACodecType(CODEC_G711U, SampleBit16);
				pThis->m_nAudioCodec = CODEC_G711U;
				pThis->OutputMsg("%s Audio Codec:G711U.\n", __FUNCTION__);
				break;
			}

			case FRAME_G726:			//726����֡
			{
				// ��ΪĿǰDVO�����G726����,��Ȼ���õ���16λ��������ʹ��32λѹ�����룬��˽�ѹ��ʹ��SampleBit32
				pAudioDecoder->SetACodecType(CODEC_G726, SampleBit32);
				pThis->m_nAudioCodec = CODEC_G726;
				nDecodeSize = FramePtr->FrameHeader()->nLength * 8;		//G726���ѹ���ʿɴ�8��
				pThis->OutputMsg("%s Audio Codec:G726.\n", __FUNCTION__);
				break;
			}
			case FRAME_AAC:				//AAC����֡��
			{
				pAudioDecoder->SetACodecType(CODEC_AAC, SampleBit16);
				pThis->m_nAudioCodec = CODEC_AAC;
				nDecodeSize = FramePtr->FrameHeader()->nLength * 24;
				pThis->OutputMsg("%s Audio Codec:AAC.\n", __FUNCTION__);
				break;
			}
			default:
			{
				assert(false);
				pThis->OutputMsg("%s Unspported audio codec.\n", __FUNCTION__);
				return 0;
				break;
			}
			}
		}
		if (nPCMSize < nDecodeSize)
		{
			pPCM = new byte[nDecodeSize];
			nPCMSize = nDecodeSize;
		}
		double dfLastPlayTime = 0.0f;
		double dfPlayTimeSpan = 0.0f;
		UINT nFramesPlayed = 0;
		WaitForSingleObject(pThis->m_hEventDecodeStart,1000);
		::EnterCriticalSection(&pThis->m_csAudioCache);
		pThis->m_nAudioCache = pThis->m_listAudioCache.size();
		::LeaveCriticalSection(&pThis->m_csAudioCache);
		TraceMsgA("%s Audio Cache Size = %d.\n", __FUNCTION__, pThis->m_nAudioCache);
		time_t tLastFrameTime = 0;
		double dfDecodeStart = GetExactTime();
		DWORD dwOsMajorVersion = GetOsMajorVersion();
		while (pThis->m_bThreadPlayAudioRun)
		{
			if (pThis->m_bPause)
			{
				if (pThis->m_pDsBuffer->IsPlaying())
					pThis->m_pDsBuffer->StopPlay();
				Sleep(100);
				continue;
			}

			nTimeSpan = (int)((GetExactTime() - dfLastPlayTime) * 1000);
			if (pThis->m_fPlayRate != 1.0f)
			{// ֻ���������ʲŲ�������
				if (pThis->m_pDsBuffer->IsPlaying())
					pThis->m_pDsBuffer->StopPlay();
				::EnterCriticalSection(&pThis->m_csAudioCache);
				if (pThis->m_listAudioCache.size() > 0)	
					pThis->m_listAudioCache.pop_front();
				::LeaveCriticalSection(&pThis->m_csAudioCache);
				Sleep(5);
				continue;
			}
			
			if (nTimeSpan > 100)			// ����100msû����Ƶ���ݣ�����Ϊ��Ƶ��ͣ
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
			pThis->m_nAudioCache = pThis->m_listAudioCache.size();
			::LeaveCriticalSection(&pThis->m_csAudioCache);

			if (!bPopFrame)
			{
				Sleep(10);
				continue;
			}
			nFramesPlayed++;
// 			if (nFramesPlayed && nFramesPlayed % 10 == 0 && nFramesPlayed <= 100)
// 			{
// 				TraceMsgA("%s TimeSpan:%.3f\t%d AudioFrames is Played,Audio Cache Size = %d.\n", __FUNCTION__, TimeSpanEx(dfDecodeStart), nFramesPlayed, pThis->m_nAudioCache);
// 			}

			if (nFramesPlayed < 50 && dwOsMajorVersion < 6)
			{// ������XPϵͳ�У�ǰ50֡�ᱻ˲�䶪��������
				if (((TimeSpanEx(dfLastPlayTime) + dfPlayTimeSpan)*1000) < nAudioFrameInterval)
					Sleep(nAudioFrameInterval - (TimeSpanEx(dfLastPlayTime)*1000));
			}

			dfPlayTimeSpan = GetExactTime();
			if (pThis->m_pDsBuffer->IsPlaying())
			{
				if (pAudioDecoder->Decode(pPCM, nPCMSize, (byte *)FramePtr->Framedata(pThis->m_nSDKVersion), FramePtr->FrameHeader()->nLength) != 0)
				{
					if (!pThis->m_pDsBuffer->WritePCM(pPCM, nPCMSize))
						pThis->OutputMsg("%s Write PCM Failed.\n", __FUNCTION__);
					SetEvent(pThis->m_hAudioFrameEvent[nFrameEvent++ % 2]);
				}
				else
					TraceMsgA("%s Audio Decode Failed Is.\n", __FUNCTION__);
			}
			dfPlayTimeSpan = TimeSpanEx(dfPlayTimeSpan);
			dfLastPlayTime = GetExactTime();
			tLastFrameTime = FramePtr->FrameHeader()->nTimestamp;
		}
		if (pPCM)
			delete[]pPCM;
		return 0;
	}
	
};