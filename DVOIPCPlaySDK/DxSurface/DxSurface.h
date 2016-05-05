#pragma once
#include <d3d9.h>
#include <d3dx9tex.h>
//#include <ppl.h>
#include <assert.h>
#include <memory>
#include <map>
#include <MMSystem.h>
#include <windows.h>
#include <tchar.h>
#include <wtypes.h>
#include <process.h>
#include "gpu_memcpy_sse4.h"
#include "DxTrace.h"
#include "../AutoLock.h"
#include "../Runlog.h"
#ifdef _DEBUG
#include "../TimeUtility.h"
#endif

#pragma warning(push)
#pragma warning(disable:4244)
#ifdef __cplusplus
extern "C" {
#endif
#define __STDC_CONSTANT_MACROS
#define FF_API_PIX_FMT 0
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
//#include "libavutil/pixfmt.h"	

//#define _ENABLE_FFMPEG_STAITC_LIB

#ifdef _ENABLE_FFMPEG_STAITC_LIB
#pragma comment(lib,"libgcc.a")
#pragma comment(lib,"libmingwex.a")
#pragma comment(lib,"libcoldname.a")
#pragma comment(lib,"libavcodec.a")
#pragma comment(lib,"libavformat.a")
#pragma comment(lib,"libavutil.a")
#pragma comment(lib,"libswscale.a")
#pragma comment(lib,"libswresample.a")
#pragma comment(lib,"WS2_32.lib")	
#else
#pragma comment(lib,"avcodec.lib")
	//#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swscale.lib")	
#endif	
#ifdef __cplusplus
}
#endif
#pragma warning(pop)

using namespace  std;
using namespace  std::tr1;
#pragma warning(push)
#pragma warning(disable:4244)
#ifdef __cplusplus
extern "C" {
#endif
#define __STDC_CONSTANT_MACROS
#define FF_API_PIX_FMT 0
#include "libavutil/frame.h"
#include "libavutil/cpu.h"
#ifdef __cplusplus
}
#endif
#pragma warning(pop)

//#pragma comment ( lib, "d3d9.lib" )
//#pragma comment ( lib, "d3dx9.lib" )
#pragma comment(lib,"winmm.lib")

#ifndef SafeDelete
#define SafeDelete(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SafeDeleteArray
#define SafeDeleteArray(p)  { if(p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef SafeRelease
#define SafeRelease(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

typedef void(*CopyFrameProc)(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch);
extern CopyFrameProc CopyFrameNV12;
//extern CopyFrameProc CopyFrameYUV420P;


#define WM_RENDERFRAME		WM_USER + 1024		// ֡��Ⱦ��Ϣ	WPARAMΪCDxSurfaceָ��,LPARAMΪһ��ָ��DxSurfaceRenderInfo�ṹ��ָ��,
#define	WM_INITDXSURFACE	WM_USER + 1025		// DXSurface��ʼ����Ϣ	WPARAMΪCDxSurfaceָ�룬LPARAMΪDxSurfaceInitInfo�ṹ��ָ��
struct DxSurfaceInitInfo
{
	int		nSize;
	HWND	hPresentWnd;
	int		nFrameWidth;
	int		nFrameHeight;
	BOOL	bWindowed;
	D3DFORMAT	nD3DFormat;
};
struct DxSurfaceRenderInfo
{
	int			nSize;
	HWND		hPresentWnd;
	AVFrame		*pAvFrame;
};
class CDxSurface;
class CriticalSectionWrap;
typedef map<HWND,CDxSurface*> WndSurfaceMap;
typedef shared_ptr<CriticalSectionWrap>CriticalSectionPtr;
typedef void (CALLBACK *ExternDrawProc)(HWND hWnd, HDC hDc, RECT rt, void *pUserPtr);
class CriticalSectionWrap
{
public:
	CriticalSectionWrap()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CriticalSectionWrap()
	{
		DeleteCriticalSection(&m_cs);
	}
	inline BOOL TryLock()
	{
		return TryEnterCriticalSection(&m_cs);
	}
	inline void Lock()
	{
		EnterCriticalSection(&m_cs);
	}
	void Unlock()
	{
		LeaveCriticalSection(&m_cs);
	}
	inline CRITICAL_SECTION *Get()
	{
		return &m_cs;
	}
	
private:
	CRITICAL_SECTION	m_cs;
};

enum GraphicQulityParameter
{
	GQ_SINC = -3,		//30		�������һ�㷨��ϸ��Ҫ����һЩ��
	GQ_SPLINE = -2,		//47		����һ���㷨���ҿ���������
	GQ_LANCZOS = -1,		//70		�������һ�㷨��Ҫƽ��(Ҳ����˵��ģ��)һ��㣬����������
	GQ_BICUBIC = 0,		//80		�о���࣬�������㷨��ԵҪƽ��������һ�㷨Ҫ������	
	GQ_GAUSS,			//80		�������һ�㷨��Ҫƽ��(Ҳ����˵��ģ��)һЩ��
	GQ_BICUBLIN,		//87		ͬ�ϡ�
	GQ_X,				//91		����һͼ���ҿ���������
	GQ_BILINEAR,		//95		�о�Ҳ�ܲ�������һ���㷨��Եƽ��һЩ��
	GQ_AREA,			//116		�������㷨���ҿ���������
	GQ_FAST_BILINEAR,	//228		ͼ��������ʧ�棬�о�Ч���ܲ���
	GQ_POINT			//427		ϸ�ڱȽ�������ͼ��Ч������ͼ�Բ�һ��㡣
};

#define _TraceMemory

#if defined(_DEBUG) && defined(_TraceMemory)
#define TraceFunction()	CTraceFunction Tx(__FUNCTION__);
#define TraceFunction1(szText)	CTraceFunction Tx(__FUNCTION__,true,szText);
#else 
#define TraceFunction()	
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <psapi.h>
#pragma comment(lib,"psapi.lib")

#ifdef _DEBUG
class CTraceFunction
{
	explicit CTraceFunction(){};
public:
	CTraceFunction(CHAR *szFunction, bool bDeconstructOut = true,CHAR *szTxt = nullptr)
	{
		ZeroMemory(this, sizeof(CTraceFunction));
		m_dfTimeIn = GetExactTime();
		m_bDeconstructOut = bDeconstructOut;
		HANDLE handle = GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
		CloseHandle(handle);
		m_nMemoryCount = pmc.WorkingSetSize / 1024;
		strcpy_s(m_szFunction, 256, szFunction);
		if (szTxt)
			strcpy_s(m_szText, 1024, szTxt);
		CHAR szText[1024] = { 0 };
		sprintf_s(szText, 1024, "%s\t_IN_ %s \tMemory = %d KB.\n",__FUNCTION__, szFunction, m_nMemoryCount);
		OutputDebugStringA(szText);
	}
	~CTraceFunction()
	{
		if (m_bDeconstructOut)
		{
			CHAR szText[4096] = { 0 };
			HANDLE handle = GetCurrentProcess();
			PROCESS_MEMORY_COUNTERS pmc;
			GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
			CloseHandle(handle);
			m_nMemoryCount = pmc.WorkingSetSize / 1024;
			if (TimeSpanEx(m_dfTimeIn) > 0.200f)
			{
				if (strlen(m_szText) ==0)
					sprintf_s(szText, 4096, "%s\t_OUT_ %s \tMemory = %d KB\tTimeSpan = %.3f.\n",__FUNCTION__, m_szFunction, m_nMemoryCount,TimeSpanEx(m_dfTimeIn));
				else
					sprintf_s(szText, 4096, "%s\t_OUT_ %s %s\tMemory = %d KB\tTimeSpan = %.3f.\n", __FUNCTION__, m_szFunction,m_szText, m_nMemoryCount, TimeSpanEx(m_dfTimeIn));
				OutputDebugStringA(szText);
			}
		}
	}
private:
	INT		m_nMemoryCount;
	double	m_dfTimeIn;
	bool	m_bDeconstructOut;
	CHAR	m_szFile[MAX_PATH];
	CHAR	m_szText[1024];
	CHAR	m_szFunction[256];
};
#endif


struct LineTime
{
	CHAR szFile[256];
	int nLine;
	DWORD nTime;
	LineTime(char *pszFile,int nFileLine)
	{
		nTime = timeGetTime();
		strcpy(szFile, pszFile);
		nLine = nFileLine;
	}
};
#ifdef _DEBUG
#define SaveRunTime()		//LineSave.SaveLineTime(__FILE__,__LINE__);
#define DeclareRunTime()	//CLineRunTime LineSave;
#else
#define SaveRunTime()
#define DeclareRunTime()			
#endif

#include <vector>
using namespace  std;
class CLineRunTime
{
public:
	CLineRunTime(CRunlogA *plog = nullptr)
	{
		pRunlog = plog;
	}
	~CLineRunTime()
	{
		DWORD dwTotalSpan = 0;
		int nSize = pTimeArray.size();
		if (nSize < 1)
			return;
		else if (nSize < 2 )
			dwTotalSpan = timeGetTime() - pTimeArray[0]->nTime;
		if (dwTotalSpan >= _LockOverTime * 2)
		{
			OutputMsg("%s @File:%s line %d Total Runtime span = %d.\n", __FUNCTION__, pTimeArray[0]->szFile, pTimeArray[0]->nLine, dwTotalSpan);
		}
		for (int i = 1; i < nSize;i ++)
		{
			DWORD dwSpan = pTimeArray[i]->nTime - pTimeArray[i - 1]->nTime;
			if (dwSpan >= _LockOverTime)
				OutputMsg("%s @File:%s line %d Runtime span = %d.\n", __FUNCTION__, pTimeArray[i]->szFile, pTimeArray[i]->nLine, dwSpan);
		}
	}
	void SaveLineTime(char *szFile, int nLine)
	{
		shared_ptr<LineTime> pLineTime = make_shared<LineTime>(szFile, nLine);
		pTimeArray.push_back(pLineTime);
	}
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
		if (pRunlog)
			pRunlog->Runlog(szBuffer);
		va_end(args);
	}
public:
	vector<shared_ptr<LineTime>> pTimeArray;
	CRunlogA *pRunlog = nullptr;
};

// ��FFMPEG����ת��ΪD3DFomrat����
struct PixelConvert
{
private:
	explicit PixelConvert()
	{
	}
	SwsContext	*pConvertCtx;	
	GraphicQulityParameter	nGQP;
	int		nScaleFlag;
// 	int		nSrcImageWidth,nSrcImageHeight;
 	AVPixelFormat nDstAvFormat;	
public:
	int nImageSize;
	byte *pImage ;
	AVFrame *pFrameNew;
	//ConvertInfo(AVFrame *pSrcFrame,AVPixelFormat DestPixfmt = AV_PIX_FMT_YUV420P,GraphicQulityParameter nGQ = GQ_FAST_BILINEAR)
	PixelConvert(AVFrame *pSrcFrame,D3DFORMAT nDestD3dFmt = (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),GraphicQulityParameter nGQ = GQ_BICUBIC)
	{
		ZeroMemory(this,sizeof(PixelConvert));
		switch(nDestD3dFmt)
		{//Direct3DSurface::GetDC() only supports D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5, D3DFMT_R8G8B8, D3DFMT_X8R8G8B8, and D3DFMT_A8R8G8B8.
		case (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'):
		default:
			nDstAvFormat = AV_PIX_FMT_YUV420P;
			break;
		case D3DFMT_R5G6B5:
			nDstAvFormat = AV_PIX_FMT_RGB565;	// ���п����ǡ�AV_PIX_FMT_BGR565LE��AV_PIX_FMT_BGR565BE
			break;
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
			nDstAvFormat = AV_PIX_FMT_RGB555;	// ���п�����AV_PIX_FMT_BGR555BE
			break;
		case D3DFMT_R8G8B8:
			nDstAvFormat = AV_PIX_FMT_BGR24;
			break;
		case D3DFMT_X8R8G8B8:
			nDstAvFormat = AV_PIX_FMT_BGR0;
			break;
		case D3DFMT_A8R8G8B8:
			nDstAvFormat = AV_PIX_FMT_BGRA;
			break;
		}
		
		pFrameNew = av_frame_alloc();
		nImageSize	 = av_image_get_buffer_size(nDstAvFormat, pSrcFrame->width,pSrcFrame->height,8); 
		if (nImageSize < 0)
		{
			char szAvError[256] = {0};
			av_strerror(nImageSize, szAvError, 1024);
			DxTraceMsg("%s av_image_get_buffer_size failed:%s.\n",__FUNCTION__,szAvError);
			assert(false);
		}
		//pImage = new (std::nothrow) byte[nImageSize];
		pImage = (byte *)_aligned_malloc(nImageSize, 32);
		if (!pImage)
		{
			DxTraceMsg("%s Alloc memory failed @%s %d.\n", __FUNCTION__, __FUNCTION__, __LINE__);
			assert(false);
			return;
		}
		DxTraceMsg("%s Image size = %d.\n",__FUNCTION__,nImageSize);
		// ����ʾͼ����YUV֡����
		av_image_fill_arrays(pFrameNew->data,pFrameNew->linesize, pImage, nDstAvFormat, pSrcFrame->width,pSrcFrame->height,8);
		pFrameNew->width	 = pSrcFrame->width;
		pFrameNew->height	 = pSrcFrame->height;
		pFrameNew->format	 = nDstAvFormat;
		nGQP = nGQ;
		switch(nGQP)
		{
		default:
		case GQ_BICUBIC:
			nScaleFlag = SWS_BICUBIC;
			break;
		case GQ_SINC:
			nScaleFlag = SWS_SINC;
			break;
		case GQ_SPLINE:
			nScaleFlag = SWS_SPLINE;
			break;
		case GQ_LANCZOS:
			nScaleFlag = SWS_LANCZOS;
			break;			
		case GQ_GAUSS:
			nScaleFlag = SWS_GAUSS;
			break;
		case GQ_BICUBLIN:
			nScaleFlag = SWS_BICUBLIN;
			break;
		case GQ_X:
			nScaleFlag = SWS_X;
			break;
		case GQ_BILINEAR:
			nScaleFlag = SWS_BILINEAR;
			break;
		case GQ_AREA:
			nScaleFlag = SWS_AREA;
			break;
		case GQ_FAST_BILINEAR:
			nScaleFlag = SWS_FAST_BILINEAR;
			break;
		case GQ_POINT:
			nScaleFlag = SWS_POINT;
			break;
		}
		
// 		nDstAvFormat = nDstFormat;
// 		nSrcImageWidth = nImageWidth;
// 		nSrcImageHeight = nImageHeight;
		pConvertCtx = sws_getContext(pSrcFrame->width,					// src Width
									pSrcFrame->height,					// src Height
									(AVPixelFormat)pSrcFrame->format,	// src format
									pSrcFrame->width,					// dst Width
									pSrcFrame->height,					// dst Height
									nDstAvFormat,					// dst format
									nScaleFlag, 
									NULL, 
									NULL, 
									NULL);

		/*		
		sws_scaleת�����������㷨ת��(��С)Ч�ʺͻ��ʶԱ����(������ο�:http://blog.csdn.net/leixiaohua1020/article/details/12029505)
		�㷨				֡��	ͼ�����۸���
		SWS_FAST_BILINEAR	228		ͼ��������ʧ�棬�о�Ч���ܲ���
		SWS_BILINEAR		95		�о�Ҳ�ܲ�������һ���㷨��Եƽ��һЩ��
		SWS_BICUBIC			80		�о���࣬�������㷨��ԵҪƽ��������һ�㷨Ҫ������
		SWS_X				91		����һͼ���ҿ���������
		SWS_POINT			427		ϸ�ڱȽ�������ͼ��Ч������ͼ�Բ�һ��㡣
		SWS_AREA			116		�������㷨���ҿ���������
		SWS_BICUBLIN		87		ͬ�ϡ�
		SWS_GAUSS			80		�������һ�㷨��Ҫƽ��(Ҳ����˵��ģ��)һЩ��
		SWS_SINC			30		�������һ�㷨��ϸ��Ҫ����һЩ��
		SWS_LANCZOS			70		�������һ�㷨��Ҫƽ��(Ҳ����˵��ģ��)һ��㣬����������
		SWS_SPLINE			47		����һ���㷨���ҿ���������

		sws_scaleת�����������㷨ת��(�Ŵ�)Ч�ʺͻ��ʶԱ����
		�㷨				֡��	ͼ�����۸���
		SWS_FAST_BILINEAR	103		ͼ��������ʧ�棬�о�Ч���ܲ���
		SWS_BILINEAR		100		����ͼ����������
		SWS_BICUBIC			78		�����ͼ���о�ϸ������һ��㡣
		SWS_X				106		������ͼ������
		SWS_POINT			112		��Ե�����Ծ�ݡ�
		SWS_AREA			114		��Ե�в����Ծ�ݡ�
		SWS_BICUBLIN		95		��������ͼ����������
		SWS_GAUSS			86		����ͼ��Ե��΢���һ�㡣
		SWS_SINC			20		������ͼ������
		SWS_LANCZOS			64		����ͼ������
		SWS_SPLINE			40		����ͼ������
		���ۺͽ���:
			�����ͼ������ţ�Ҫ׷���Ч������˵����Ƶͼ��Ĵ����ڲ���ȷ�ǷŴ�����Сʱ��
		ֱ��ʹ��SWS_FAST_BILINEAR�㷨���ɡ������ȷ��Ҫ��С����ʾ������ʹ��Point�㷨�����
		����ȷҪ�Ŵ���ʾ����ʵʹ��CImage��Strech����Ч��
			��Ȼ����������ٶ�׷������������������㷨�У�ѡ��֡����͵��Ǹ����ɣ�����Ч
			��һ������õġ�
		*/
	}
	~PixelConvert()
	{
		if (pImage)
		{
			//delete[]pImage;
			_aligned_free(pImage);
			pImage = NULL;
		}
		av_free(pFrameNew);
		pFrameNew = NULL;
		sws_freeContext(pConvertCtx);
		pConvertCtx = NULL;
		DxTraceMsg("%s FreeImage size %d.\n",__FUNCTION__,nImageSize);
	}
	// ��������ת��
	int inline ConvertPixel(AVFrame *pSrcFrame,GraphicQulityParameter nGQ = GQ_BICUBIC)
	{
		if (!pImage)
			return -1;
		DxTrace("@File:%s %s pSrcFrame(%d,%d)->LineSize(%d,%d,%d).\n", __FILE__, __FUNCTION__, pSrcFrame->width, pSrcFrame->height, pSrcFrame->linesize[0], pSrcFrame->linesize[1], pSrcFrame->linesize[2]);
		if (nGQP != nGQ)
		{
			// ת���㷨����
			sws_freeContext(pConvertCtx);
			pConvertCtx = NULL;
			nGQP = nGQ;
			switch(nGQP)
			{
			default:
			case GQ_BICUBIC:
				nScaleFlag = SWS_BICUBIC;
				break;
			case GQ_SINC:
				nScaleFlag = SWS_SINC;
				break;
			case GQ_SPLINE:
				nScaleFlag = SWS_SPLINE;
				break;
			case GQ_LANCZOS:
				nScaleFlag = SWS_LANCZOS;
				break;			
			case GQ_GAUSS:
				nScaleFlag = SWS_GAUSS;
				break;
			case GQ_BICUBLIN:
				nScaleFlag = SWS_BICUBLIN;
				break;
			case GQ_X:
				nScaleFlag = SWS_X;
				break;
			case GQ_BILINEAR:
				nScaleFlag = SWS_BILINEAR;
				break;
			case GQ_AREA:
				nScaleFlag = SWS_AREA;
				break;
			case GQ_FAST_BILINEAR:
				nScaleFlag = SWS_FAST_BILINEAR;
				break;
			case GQ_POINT:
				nScaleFlag = SWS_POINT;
				break;
			}
			pConvertCtx = sws_getContext(pSrcFrame->width,					// src Width
										pSrcFrame->height,					// src Height
										(AVPixelFormat)pSrcFrame->format,	// src format
										pSrcFrame->width,					// dst Width
										pSrcFrame->height,					// dst Height
										nDstAvFormat,					// dst format
										nScaleFlag, 
										NULL, 
										NULL, 
										NULL);
		}
		return sws_scale(pConvertCtx,
						(const byte * const *)pSrcFrame->data,
						pSrcFrame->linesize,
						0,
						pSrcFrame->height,
						pFrameNew->data,
						pFrameNew->linesize);

	}
	inline AVPixelFormat GetDestPixelFormat()
	{
		return nDstAvFormat;
	}
};

typedef IDirect3D9* WINAPI pDirect3DCreate9(UINT);
typedef HRESULT WINAPI pDirect3DCreate9Ex(UINT, IDirect3D9Ex**);
// ע��:
//		ʹ��CDxSurface������ʾͼ��ʱ�������ڴ����߳�����ʾͼ�����򵱷���DirectX�豸��ʧʱ���޷�����DirectX����Դ
class CDxSurface
{
protected:	
	long					m_nVtableAddr;		// �麯�����ַ���ñ�����ַλ���麯����֮�󣬽��������ʼ��������ƶ��ñ�����λ��
	
	D3DPRESENT_PARAMETERS	m_d3dpp;
	CRITICAL_SECTION		m_csRender;			// ��Ⱦ�ٽ���
	CRITICAL_SECTION		m_csSnapShot;		// ��ͼ�ٽ���
	bool					m_bD3DShared;		// IDirect3D9�ӿ��Ƿ�Ϊ���� 	
	/*HWND					m_hWnd;*/
	DWORD					m_dwExStyle;
	DWORD					m_dwStyle;
	bool					m_bFullScreen;		// �Ƿ�ȫ��,Ϊtrueʱ�������ȫ��/���ڵ��л�,�л���ɺ�,m_bSwitchScreen������Ϊfalse
	WINDOWPLACEMENT			m_WndPlace;
	HMENU					m_hMenu;
	IDirect3DSurface9		*m_pDirect3DSurfaceRender/* = NULL*/;
	IDirect3DSurface9		*m_pSnapshotSurface;	/* = NULL*/;	//��ͼר�ñ���
	D3DFORMAT				m_nD3DFormat;
	UINT					m_nVideoWidth;
	UINT					m_nVideoHeight;
	bool					m_bInitialized;	
	HMODULE					m_hD3D9;
	shared_ptr<PixelConvert>m_pPixelConvert;
	IDirect3D9				*m_pDirect3D9		/* = NULL*/;
	IDirect3DDevice9		*m_pDirect3DDevice	/*= NULL*/;	
	HANDLE					m_hEventSnapShot;	// ��ͼ�����¼�
	HANDLE					m_hEventCopySurface;// �������ݸ����¼�
	// �ⲿ���ƽӿڣ��ṩ�ⲿ�ӿڣ�������÷����л���ͼ��
	ExternDrawProc			m_pExternDraw;
	void*					m_pUserPtr;			// �ⲿ�������Զ���ָ��
	
	bool					m_bFullWnd;			// �Ƿ���������,ΪTrueʱ��������������ڿͻ���������ֻ����Ƶ������ʾ	
	HWND					m_hFullScreenWindow;// αȫ������
	// ��ͼ��ر���

	D3DXIMAGE_FILEFORMAT	m_D3DXIFF;			// ��ͼ����,Ĭ��Ϊbmp��ʽ
 	WCHAR					m_szSnapShotPath[MAX_PATH];
	bool					m_bEnableVsync;		// ���ô�ֱͬ��,Ĭ��Ϊtrue,����Ҫ��ʾ��֡�ʴ�����ʾ��ʵ��֡��ʱ,��Ҫ���ô�ֱͬ��

	bool					m_bVideoScaleFixed;	// ��m_bVideoScaleFixedΪtrue,����m_fWHScale = 0ʱ,��ʹ��ͼ��ԭʼ����,����ֵΪWidth/Height
												// ��m_bVideoScaleFixedΪtrue,����m_fWHScale ����0ʱ,��ʹ��dfWHScale�ṩ�ı�����ʾ��ͼ����ܻᱻ�������
												// ��m_bVideoScaleFixedΪfalse,m_fWHScale������������,��ʱ���������ڿͻ���
	float					m_fWHScale;	
	WNDPROC					m_pOldWndProc;
	static WndSurfaceMap	m_WndSurfaceMap;
	static CriticalSectionPtr m_WndSurfaceMapcs;
	static	int				m_nObjectCount;
	static CriticalSectionPtr m_csObjectCount;
	bool					m_bWndSubclass;		// �Ƿ����໯��ʾ����,Ϊtureʱ������ʾ�������໯,��ʱ������Ϣ���ȱ�CDxSurface::WndProc���ȴ���,���ɴ��ں�������
	pDirect3DCreate9*		m_pDirect3DCreate9;
public:
	
	explicit CDxSurface(IDirect3D9 *pD3D9)
	{
		ZeroMemory(&m_nVtableAddr, sizeof(CDxSurface) - offsetof(CDxSurface,m_nVtableAddr));
		InitializeCriticalSection(&m_csRender);
		InitializeCriticalSection(&m_csSnapShot);
		if (pD3D9)
		{
			m_bD3DShared = true;
			m_pDirect3D9 = pD3D9;
		}
	}
	CDxSurface()
	{
		TraceFunction();
		// ������m_nVtableAddr�������ܿ����麯����ĳ�ʼ��
		// ��������΢���Visual C++������
		ZeroMemory(&m_nVtableAddr, sizeof(CDxSurface) - offsetof(CDxSurface,m_nVtableAddr));
		m_csObjectCount->Lock();
		m_nObjectCount++;
		DxTraceMsg("%s CDxSurface Count = %d.\n", __FUNCTION__, m_nObjectCount);
		m_csObjectCount->Unlock();
		m_bEnableVsync = true;
		if (!m_hD3D9)
			m_hD3D9 = LoadLibraryA("d3d9.dll");
		if (!m_hD3D9)
		{
			DxTraceMsg("%s Failed load D3d9.dll.\n",__FUNCTION__);
			assert(false);
			return;
		}
		m_pDirect3DCreate9 = (pDirect3DCreate9*)GetProcAddress(m_hD3D9, "Direct3DCreate9");
		if (m_pDirect3DCreate9 == NULL)
		{
			DxTraceMsg("%s Can't locate the Procedure \"Direct3DCreate9\".\n",__FUNCTION__);
			assert(false);
			return;
		}
		m_pDirect3D9 = m_pDirect3DCreate9(D3D_SDK_VERSION);
		if (!m_pDirect3D9)
		{
			DxTraceMsg("%s Direct3DCreate9 failed.\n",__FUNCTION__);
			assert(false);
		}
		InitializeCriticalSection(&m_csRender);
		InitializeCriticalSection(&m_csSnapShot);
		m_hEventCopySurface		 = CreateEvent(NULL,FALSE,FALSE,NULL);
		m_hEventSnapShot	 = CreateEvent(NULL,FALSE,FALSE,NULL);
	}
	void ReleaseOffSurface()
	{
		if (m_pDirect3DSurfaceRender)
			SafeRelease(m_pDirect3DSurfaceRender);
	}
	virtual ~CDxSurface()
	{
		TraceFunction();
		//DetachWnd();
		DxCleanup();
		SafeRelease(m_pDirect3D9);
		if (m_hD3D9)
		{
			FreeLibrary(m_hD3D9);
			m_hD3D9 = NULL;
		}
		DeleteCriticalSection(&m_csRender);
		DeleteCriticalSection(&m_csSnapShot);
		if (m_hEventCopySurface)
		{
			CloseHandle(m_hEventCopySurface);
			m_hEventCopySurface = NULL;
		}
		if (m_hEventSnapShot)
		{
			CloseHandle(m_hEventSnapShot);
			m_hEventSnapShot = NULL;
		}
	}

	// ���ô�ֱͬ��,ֻ���ڳ�ʼ��֮ǰ����,�Ż���Ч
	// Ĭ������£���ֱͬ��������, ����Ҫ��ʾ��֡�ʴ�����ʾ��ʵ��֡��ʱ,��Ҫ���ô�ֱͬ��
	bool	DisableVsync()
	{
		if (!m_bInitialized)
		{
			m_bEnableVsync = false;
			return true;
		}
		else
			return false;
	}

	void SetExternDraw(void *pExternDrawProc,void *pUserPtr)
	{
		m_pExternDraw = (ExternDrawProc)pExternDrawProc;
		m_pUserPtr = pUserPtr;
	}

	// �����ⲿ���ƺ���
	void ExternDrawCall(HWND hWnd,RECT *pRect)
	{
		if (!m_pDirect3DSurfaceRender)
			return;
		
		if (m_pExternDraw)
		{
			D3DSURFACE_DESC Desc;
			if (FAILED(m_pDirect3DSurfaceRender->GetDesc(&Desc)))
				return ;
			switch(Desc.Format)
			{//Direct3DSurface::GetDC() only supports D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5, D3DFMT_R8G8B8, D3DFMT_X8R8G8B8, and D3DFMT_A8R8G8B8.
			// �ⲿͼ����ƽ�֧���������ظ�ʽ
			case D3DFMT_R5G6B5:
			case D3DFMT_X1R5G5B5:
			case D3DFMT_A1R5G5B5:
			case D3DFMT_R8G8B8:
			case D3DFMT_X8R8G8B8:
			case D3DFMT_A8R8G8B8:
				break;
			default:
				{
					DxTraceMsg("%s Unsupported Pixel format.\r\n",__FUNCTION__);
					assert(false);
					return;
				}
			}
			HDC hDc = NULL;
			if (SUCCEEDED(m_pDirect3DSurfaceRender->GetDC(&hDc)))
			{
				RECT MotionRect;
				HWND hMotionWnd = m_d3dpp.hDeviceWindow;
				if (hWnd)
					hMotionWnd = hWnd;
				if (pRect)
					memcpy(&MotionRect,pRect,sizeof(RECT));
				else
				{
					MotionRect.left		= 0;
					MotionRect.right		= Desc.Width;
					MotionRect.top			= 0;
					MotionRect.bottom		= Desc.Height;
				}
				m_pExternDraw(hMotionWnd,hDc,MotionRect,m_pUserPtr);
				m_pDirect3DSurfaceRender->ReleaseDC(hDc);
			}
			else
			{
				DxTraceMsg("%s Get DC(Device Context) from DxSurface failed.\r\n", __FUNCTION__);
				assert(false);
			}
		}
	}

	virtual bool ResetDevice()
	{
		TraceFunction();
		SafeRelease(m_pDirect3DSurfaceRender);	
#ifdef _DEBUG
		HRESULT hr = m_pDirect3DDevice->Reset(&m_d3dpp);
		if (SUCCEEDED(hr))
		{
			DxTraceMsg("%s Direct3DDevice::Reset Succceed.\n",__FUNCTION__);
			return true;
		}
		else
		{
			DxTraceMsg("%s Direct3DDevice::Reset Failed,hr = %08X.\n",__FUNCTION__,hr);
			return false;
		}
#else
		return SUCCEEDED(m_pDirect3DDevice->Reset(&m_d3dpp));
#endif
	}
	// ȡ��Ƶ�ߴ�,��16 bitΪ��ȣ���16bitΪ�߶�
	DWORD GetVideoSize()
	{
		return MAKELONG(m_nVideoWidth, m_nVideoHeight);
	}
	// D3dDirect9Ex�£��ó�Ա������Ч
	virtual bool RestoreDevice()
	{// �ָ��豸������ԭʼ�����ؽ���Դ
		TraceFunction();
		if (!m_pDirect3D9 || !m_pDirect3DDevice)
			return false;
#ifdef _DEBUG
		HRESULT hr = m_pDirect3DDevice->CreateOffscreenPlainSurface(m_nVideoWidth, m_nVideoHeight, m_nD3DFormat, D3DPOOL_DEFAULT, &m_pDirect3DSurfaceRender, NULL);
		if (SUCCEEDED(hr))
		{
			DxTraceMsg("%s Direct3DDevice::CreateOffscreenPlainSurface Succeed.\n",__FUNCTION__);
			return true;
		}
		else
		{
			DxTraceMsg("%s Direct3DDevice::CreateOffscreenPlainSurface Failed,hr = %08X.\n",__FUNCTION__,hr);
			return false;
		}
#else
		return SUCCEEDED(m_pDirect3DDevice->CreateOffscreenPlainSurface(m_nVideoWidth, m_nVideoHeight, m_nD3DFormat, D3DPOOL_DEFAULT, &m_pDirect3DSurfaceRender, NULL));
#endif
	}

#ifdef _DEBUG
	virtual void OutputDxPtr()
	{
		DxTraceMsg("%s m_pDirect3D9 = %p\tm_pDirect3DDevice = %p\tm_pDirect3DSurfaceRender = %p.\n", __FUNCTION__, m_pDirect3D9, m_pDirect3DDevice, m_pDirect3DSurfaceRender);
	}
#endif
	virtual void DxCleanup()
	{
		SafeRelease(m_pDirect3DSurfaceRender);
		SafeRelease(m_pDirect3DDevice);
		SafeRelease(m_pSnapshotSurface);
	}

	// ���ͽ�ͼ���󣬼����Ž�ͼ�¼�
	void RequireSnapshot()
	{
		SetEvent(m_hEventSnapShot);
	}

	// �ѽ���֡pAvFrame�е�ͼ���͵���ͼ����
	void TransferSnapShotSurface(AVFrame *pAvFrame)
	{
		if (m_pSnapshotSurface && WaitForSingleObject(m_hEventSnapShot, 0) == WAIT_OBJECT_0)						// �յ���ͼ����
		{
			//ResetEvent(m_hEventCreateSurface);
			// ����ʹ��StretchRect���Դ���渴�Ƶ�ϵͳ�ڴ����
			// hr = m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, &srcrt, m_pSnapshotSurface, &srcrt, D3DTEXF_LINEAR);
			D3DLOCKED_RECT D3dRect;
			m_pSnapshotSurface->LockRect(&D3dRect, NULL, D3DLOCK_DONOTWAIT);
			// FFMPEG���ر����ֽ�����DirectX���ص��ֽ����෴,���
			// D3DFMT_A8R8G8B8(A8:R8:G8:B8)==>AV_PIX_FMT_BGRA(B8:G8:R8:A8)
#if _MSC_VER >= 1600
			shared_ptr<PixelConvert> pVideoScale = make_shared<PixelConvert>(pAvFrame,D3DFMT_A8R8G8B8,GQ_BICUBIC);
#else
			shared_ptr<PixelConvert> pVideoScale (new PixelConvert(pAvFrame,D3DFMT_A8R8G8B8,GQ_BICUBIC));
#endif
			pVideoScale->ConvertPixel(pAvFrame);
			memcpy(D3dRect.pBits, pVideoScale->pImage,pVideoScale->nImageSize);
			m_pSnapshotSurface->UnlockRect();
			SetEvent(m_hEventCopySurface);
			DxTraceMsg("%s Surface is Transfered.\n", __FUNCTION__);
		}
	}
	
	// ����ץͼ����Surface�е�ͼ�����ݱ��浽�ļ��У��˽�ͼ�õ���ͼ����ԭʼ��ͼ��
	virtual bool SaveSurfaceToFileA(CHAR *szFilePath,D3DXIMAGE_FILEFORMAT D3DImageFormat = D3DXIFF_JPG)
	{
		if (!szFilePath || strlen(szFilePath) <= 0)
			return false;
		
		WCHAR szFilePathW[1024] = { 0 };
		MultiByteToWideChar(CP_ACP, 0, szFilePath, -1, szFilePathW, 1024);
		return SaveSurfaceToFileW(szFilePathW, D3DImageFormat);
	}
	
	// ����ץͼ����Surface�е�ͼ�����ݱ��浽�ļ��У��˽�ͼ�õ���ͼ����ԭʼ��ͼ��
	virtual bool SaveSurfaceToFileW(WCHAR *szFilePath, D3DXIMAGE_FILEFORMAT D3DImageFormat = D3DXIFF_JPG)
	{
		if (!m_pDirect3DDevice ||
			!m_pDirect3DSurfaceRender)
			return false;
		if (!szFilePath || wcslen(szFilePath) <= 0)
			return false;
		CAutoLock lock(&m_csSnapShot);
		
		m_D3DXIFF = D3DImageFormat;
		HRESULT hr = S_OK;
		if (!m_pSnapshotSurface)
		{
			HRESULT hr = m_pDirect3DDevice->CreateOffscreenPlainSurface(m_nVideoWidth,
				m_nVideoHeight,
				D3DFMT_A8R8G8B8,
				D3DPOOL_SYSTEMMEM,
				&m_pSnapshotSurface,
				NULL);
			if (FAILED(hr))
			{
				DxTraceMsg("%s IDirect3DSurface9::GetDesc failed,hr = %08X.\n", __FUNCTION__, hr);
				return false;
			}
		}

		wcscpy(m_szSnapShotPath, szFilePath);
		bool bSnapReady = false;
		// ��ͼ������δ����,�����Ž�ͼ�¼�
		if (WaitForSingleObject(m_hEventCopySurface, 0) == WAIT_TIMEOUT)
		{
			SetEvent(m_hEventSnapShot);
			// ���´���ͼ����
			if (WaitForSingleObject(m_hEventCopySurface, 1000) == WAIT_OBJECT_0)
				bSnapReady = true;
			else
				return false;
		}
		else // ��ͼ�����Ѿ���
			bSnapReady = true;
		if (!bSnapReady)
			return false;

		hr = D3DXSaveSurfaceToFileW(szFilePath, m_D3DXIFF, m_pSnapshotSurface, NULL, NULL);
		if (FAILED(hr))
		{
			DxTraceMsg("%s D3DXSaveSurfaceToFile Failed,hr = %08X.\n", __FUNCTION__, hr);
			return false;
		}
		return true;
	}
	
	// ��Ļץͼ������ʾ����Ļ�ϵ�ͼ�󣬱��浽�ļ���,�˽�ͼ�õ����п��ܲ���ԭʼ��ͼ�񣬿����Ǳ�����������ͼ�� 
	virtual void CaptureScreen(TCHAR *szFilePath,D3DXIMAGE_FILEFORMAT D3DImageFormat = D3DXIFF_JPG)		
	{
		if (!m_pDirect3DDevice)
			return ;

		D3DDISPLAYMODE mode;
		HRESULT hr = m_pDirect3DDevice->GetDisplayMode(0,&mode);
		if (FAILED(hr))
		{
			DxTraceMsg("%s IDirect3DSurface9::GetDesc GetDisplayMode,hr = %08X.\n",__FUNCTION__,hr);
			return ;
		}

		IDirect3DSurface9 *pSnapshotSurface = NULL;	
		hr = m_pDirect3DDevice->CreateOffscreenPlainSurface(mode.Width,
															mode.Height, 
															D3DFMT_A8R8G8B8, 
															D3DPOOL_SYSTEMMEM, 
															&pSnapshotSurface, 
															NULL);
		if (FAILED(hr))
		{
			DxTraceMsg("%s IDirect3DSurface9::GetDesc failed,hr = %08X.\n",__FUNCTION__,hr);
			return ;
		}
		
		hr = m_pDirect3DDevice->GetFrontBufferData(0,pSnapshotSurface);
		if (FAILED(hr))
		{
			DxTraceMsg("%s IDirect3DDevice9::GetFrontBufferData failed,hr = %08X.\n",__FUNCTION__,hr);
			SafeRelease(pSnapshotSurface);
			return ;
		}

		RECT *rtSaved = NULL;
		WINDOWINFO windowInfo ;
		if (m_d3dpp.Windowed)
		{
			windowInfo.cbSize = sizeof(WINDOWINFO);
			GetWindowInfo(m_d3dpp.hDeviceWindow, &windowInfo);
			rtSaved = &windowInfo.rcWindow;
		}
		//_tcscpy_s(m_szSnapShotPath,MAX_PATH,szPath);
		// m_D3DXIFF = D3DXIFF_JPG;
		hr = D3DXSaveSurfaceToFile(szFilePath,D3DImageFormat,pSnapshotSurface,NULL,rtSaved);
		if (FAILED(hr))		
			DxTraceMsg("%s D3DXSaveSurfaceToFile Failed,hr = %08X.\n",__FUNCTION__,hr);
		SafeRelease(pSnapshotSurface);
	}
	/*
	D3DXIFF_BMP          = 0,
	D3DXIFF_JPG          = 1,
	D3DXIFF_TGA          = 2,
	D3DXIFF_PNG          = 3,
	D3DXIFF_DDS          = 4,
	D3DXIFF_PPM          = 5,
	D3DXIFF_DIB          = 6,
	D3DXIFF_HDR          = 7,
	D3DXIFF_PFM          = 8,
	*/
	void SavetoFile(TCHAR *szPath,D3DXIMAGE_FILEFORMAT nD3DXIFF)
	{
		//_tcscpy_s(m_szSnapShotPath,MAX_PATH,szPath);
#ifdef _UNICODE
		SaveSurfaceToFileW(szPath,nD3DXIFF);
#else
		SaveSurfaceToFileA(szPath, nD3DXIFF);
#endif
	}

	void SavetoBmp(TCHAR *szPath)
	{
		//_tcscpy_s(m_szSnapShotPath,MAX_PATH,szPath);
#ifdef _UNICODE
		SaveSurfaceToFileW(szPath,D3DXIFF_BMP);
#else
		SaveSurfaceToFileA(szPath, D3DXIFF_BMP);
#endif
	}
	
	// ����InitD3D֮ǰ�����ȵ���AttachWnd����������Ƶ��ʾ����
	// nD3DFormat ����Ϊ���¸�ʽ֮һ
	// MAKEFOURCC('Y', 'V', '1', '2')	Ĭ�ϸ�ʽ,���Ժܷ������YUV420Pת���õ�,��YUV420P��FFMPEG�����õ���Ĭ�����ظ�ʽ
	// MAKEFOURCC('N', 'V', '1', '2')	��DXVAӲ����ʹ�øø�ʽ
	// D3DFMT_R5G6B5
	// D3DFMT_X1R5G5B5
	// D3DFMT_A1R5G5B5
	// D3DFMT_R8G8B8
	// D3DFMT_X8R8G8B8
	// D3DFMT_A8R8G8B8
	virtual bool InitD3D(HWND hWnd,
				 int nVideoWidth,
				 int nVideoHeight,
				 BOOL bIsWindowed = TRUE,
				 D3DFORMAT nD3DFormat = (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'))
	{
		TraceFunction();
		assert(hWnd != NULL);
		assert(IsWindow(hWnd));
		assert(nVideoWidth != 0 || nVideoHeight != 0);
		bool bSucceed = false;
		
		D3DCAPS9 caps;
		m_pDirect3D9->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
		int vp = 0;
		if (caps.DevCaps& D3DDEVCAPS_HWTRANSFORMANDLIGHT)		
			vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else		
			vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		HRESULT hr = S_OK;		
		D3DDISPLAYMODE d3ddm;
		if (FAILED(hr = m_pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
		{
			DxTraceMsg("%s GetAdapterDisplayMode Failed.\nhr=%x", __FUNCTION__, hr);
			goto _Failed;
		}

		ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
		m_d3dpp.BackBufferFormat		= d3ddm.Format;
		m_d3dpp.BackBufferCount			= 1;
		m_d3dpp.Flags					= 0;
		m_d3dpp.Windowed				= bIsWindowed;
		m_d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_ONE;// D3DPRESENT_INTERVAL_IMMEDIATE;
		m_d3dpp.hDeviceWindow			= hWnd;
		m_d3dpp.MultiSampleQuality		= 0;
		m_d3dpp.MultiSampleType			= D3DMULTISAMPLE_NONE;					// ��ʾ��Ƶʱ������ʹ�ö��ز��������򽫵��»������
		m_d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;				// ָ��ϵͳ��ν���̨�����������ݸ��Ƶ�ǰ̨������ D3DSWAPEFFECT_DISCARD:�����̨���������
#pragma warning(push)
#pragma warning(disable:4800)
		m_bFullScreen					= (bool)bIsWindowed;
#pragma warning(pop)		

		if (bIsWindowed)//����ģʽ
		{
			if (m_dwStyle)
				SetWindowLong(m_d3dpp.hDeviceWindow, GWL_STYLE, m_dwStyle);
			if (m_dwExStyle)
				SetWindowLong(m_d3dpp.hDeviceWindow, GWL_EXSTYLE, m_dwExStyle);
			SetWindowPlacement(m_d3dpp.hDeviceWindow, &m_WndPlace) ;
			if (!m_bEnableVsync)
				m_d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
			else
				m_d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_DEFAULT;
			m_d3dpp.FullScreen_RefreshRateInHz	= 0;							// ��ʾ��ˢ���ʣ�����ģʽ��ֵ����Ϊ0
			
			m_d3dpp.BackBufferHeight			= nVideoHeight;
			m_d3dpp.BackBufferWidth				= nVideoWidth;
			m_d3dpp.EnableAutoDepthStencil		= FALSE;							// �ر��Զ���Ȼ���
		}
		else
		{
			//ȫ��ģʽ
			m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
			m_d3dpp.FullScreen_RefreshRateInHz	= d3ddm.RefreshRate;	
			
			m_d3dpp.EnableAutoDepthStencil		= FALSE;			
			m_d3dpp.BackBufferWidth				= GetSystemMetrics(SM_CXSCREEN);		// �����Ļ��
			m_d3dpp.BackBufferHeight			= GetSystemMetrics(SM_CYSCREEN);		// �����Ļ��

			GetWindowPlacement(m_d3dpp.hDeviceWindow, &m_WndPlace ) ;
			m_dwExStyle	 = GetWindowLong( m_d3dpp.hDeviceWindow, GWL_EXSTYLE ) ;
			m_dwStyle	 = GetWindowLong( m_d3dpp.hDeviceWindow, GWL_STYLE ) ;
			m_dwStyle	 &= ~WS_MAXIMIZE & ~WS_MINIMIZE; // remove minimize/maximize style
			m_hMenu		 = GetMenu( m_d3dpp.hDeviceWindow ) ;
		}
		/*
		CreateDevice��BehaviorFlags����ѡ�
		D3DCREATE_ADAPTERGROUP_DEVICEֻ�����Կ���Ч�����豸�������������ӵ�е�������ʾ���
		D3DCREATE_DISABLE_DRIVER_MANAGEMENT�����豸������������Դ�������ڷ�����Դ����ʱD3D���ò���ʧ��
		D3DCREATE_DISABLE_PRINTSCREEN:��ע�������ݼ���ֻ��Direct3D 9Ex
		D3DCREATE_DISABLE_PSGP_THREADING��ǿ�Ƽ��㹤�����������߳��ϣ�vista������Ч
		D3DCREATE_ENABLE_PRESENTSTATS������GetPresentStatistics�ռ�ͳ����Ϣֻ��Direct3D 9Ex
		D3DCREATE_FPU_PRESERVE��ǿ��D3D���߳�ʹ����ͬ�ĸ��㾫�ȣ��ή������
		D3DCREATE_HARDWARE_VERTEXPROCESSING��ָ��Ӳ�����ж��㴦���������D3DCREATE_PUREDEVICE
		D3DCREATE_MIXED_VERTEXPROCESSING��ָ����϶��㴦��
		D3DCREATE_SOFTWARE_VERTEXPROCESSING��ָ������Ķ��㴦��
		D3DCREATE_MULTITHREADED��Ҫ��D3D���̰߳�ȫ�ģ����߳�ʱ
		D3DCREATE_NOWINDOWCHANGES��ӵ�в��ı䴰�ڽ���
		D3DCREATE_PUREDEVICE��ֻ��ͼʹ�ô�Ӳ������Ⱦ
		D3DCREATE_SCREENSAVER�������������ֻ��Direct3D 9Ex
		D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DCREATE_MIXED_VERTEXPROCESSING, and D3DCREATE_SOFTWARE_VERTEXPROCESSING��������һ��һ��Ҫ����
		*/
		if (FAILED(hr = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			m_d3dpp.hDeviceWindow,
			vp ,
			&m_d3dpp,
			/* NULL,*/
			&m_pDirect3DDevice)))
		{
			DxTraceMsg("%s CreateDevice Failed.\nhr=%x", __FUNCTION__, hr);
			goto _Failed;
		}

		D3DPOOL nPoolArray[] = { D3DPOOL_DEFAULT, D3DPOOL_MANAGED ,	D3DPOOL_SYSTEMMEM ,	D3DPOOL_SCRATCH };
		bSucceed = false;
		int nPool = 0;
		while (nPool <= 3)
		{
			if (FAILED(hr = m_pDirect3DDevice->CreateOffscreenPlainSurface(nVideoWidth,
				nVideoHeight,
				nD3DFormat,
				nPoolArray[nPool],
				&m_pDirect3DSurfaceRender,
				NULL)))
			{
				nPool++;
				continue;
			}
			else
			{
				bSucceed = true;
				break;
			}
		}
		if (!bSucceed)
		{
			DxTraceMsg("%s CreateOffscreenPlainSurface Failed.\nhr=%x", __FUNCTION__, hr);
			goto _Failed;
		}
		D3DSURFACE_DESC SrcSurfaceDesc;			
		m_pDirect3DSurfaceRender->GetDesc(&SrcSurfaceDesc);
		// �������
		m_nVideoWidth	 = nVideoWidth;
		m_nVideoHeight	 = nVideoHeight;
		m_nD3DFormat = nD3DFormat;		
		bSucceed = true;
		m_bInitialized = true;
_Failed:
		if (!bSucceed)
		{
			DxCleanup();
		}
		return bSucceed;
	}

	bool CreateSurface(UINT nVideoWidth, UINT nVideoHeight, D3DFORMAT nD3DFormat/* = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2')*/)
	{
		if (!m_pDirect3D9 || !m_pDirect3DDevice)
			return false;
		int nScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CXFULLSCREEN);
		HRESULT hr = m_pDirect3DDevice->CreateOffscreenPlainSurface(nVideoWidth, nVideoHeight, nD3DFormat, D3DPOOL_DEFAULT, &m_pDirect3DSurfaceRender, NULL);		

		if (FAILED(hr))
			return false;

		if (FAILED(hr))
		{
			DxCleanup();
			return false;
		}
		else
		{
			m_nVideoWidth = nVideoWidth;
			m_nVideoHeight = nVideoHeight;
			m_nD3DFormat = nD3DFormat;
			return true;
		}
	}
	// �ж��Ƿ���Ҫ��Ŀ�괰������ʾͼ��
	// �ڱ����ػ���С���Ĵ�������ʾͼ���ٶȷǳ���,������Ӱ��������Ⱦ���̣����
	// �����ڻ�������ڴ������ػ���С��״̬ʱ����Ӧ�ڸô����ϻ���ͼ��
	bool IsNeedRender(HWND hRenderWnd)
	{
		// �����ڱ����ػ���С��������ʾͼ��
		if (IsIconic(hRenderWnd))		// ������С��	
		{
			//DxTraceMsg("%s hRenderWnd is Iconic.\n", __FUNCTION__);
			return false;
		}
		if (!IsWindowVisible(hRenderWnd))// ��������
		{
			//DxTraceMsg("%s hRenderWnd is Unvisible Window.\n", __FUNCTION__);
			return false;
		}
		// ����ǰ���ڵĸ����ڱ����ػ���С���಻��ʾͼ��
		HWND hRoot = GetAncestor(hRenderWnd,GA_ROOT);
		if (hRoot)
		{
			if (IsIconic(hRoot))			// ������С��
			{
				//DxTraceMsg("%s hRoot is Iconic.\n", __FUNCTION__);
				return false;
			}
			if (!IsWindowVisible(hRoot))	// ��������
			{
				//DxTraceMsg("%s hRoot is WindowUnvisible.\n", __FUNCTION__);
				ShowWindow(hRoot, SW_SHOW);
				return false;
			}
		}
		return true;
	}

	virtual inline IDirect3DDevice9 *GetD3DDevice()
	{
		return m_pDirect3DDevice;
	}

	virtual inline IDirect3D9 *GetD3D9()
	{
		return m_pDirect3D9;
	}
	inline void SetD3DShared(bool bD3dShared = false)
	{
		m_bD3DShared = bD3dShared;
	}
	void SetFullScreenWnd(HWND hWnd = NULL)
	{
		CAutoLock lock(&m_csRender);
		m_hFullScreenWindow = hWnd;
	}

	virtual inline bool SetD3DShared(IDirect3D9 *pD3D9)
	{
		if (pD3D9)
		{
			m_bD3DShared = true;
			m_pDirect3D9 = pD3D9;
			return true;
		}
		else
			return false;
	}
	inline bool GetD3DShared()
	{
		return m_bD3DShared;
	}

	bool IsInited()
	{
		return m_bInitialized;
	}
	
	/*
	RECT	m_rcWindow;
#define RectWidth(rt)	(rt.right - rt.left)
#define RectHeight(rt)	(rt.bottom - rt.top)
	WINDOWPLACEMENT	m_windowedPWP;
	HWND	m_hParentWnd;
	// ��ʱ��Ҫʹ��������ܣ���ΪĿǰ��δ�ҵ�ȫ�����ش���ģʽ�ķ���
	inline void SwitchFullScreen(HWND hWnd = NULL)
	{
		CAutoLock lock(&m_csRender);
		m_d3dpp.Windowed = !m_d3dpp.Windowed;
		WNDPROC pOldWndProc = (WNDPROC)GetWindowLong(m_d3dpp.hDeviceWindow,GWL_WNDPROC);
		if (m_d3dpp.Windowed)
		{
			m_d3dpp.BackBufferFormat = m_nD3DFormat;
			m_d3dpp.FullScreen_RefreshRateInHz = 0;	
			if ( m_dwStyle != 0 )
				SetWindowLong(m_d3dpp.hDeviceWindow, GWL_STYLE, m_dwStyle );
			if ( m_dwExStyle != 0 )
				SetWindowLong(m_d3dpp.hDeviceWindow, GWL_EXSTYLE, m_dwExStyle );

			if ( m_hMenu != NULL )
				SetMenu(m_d3dpp.hDeviceWindow, m_hMenu );

			if ( m_windowedPWP.length == sizeof( WINDOWPLACEMENT ) )
			{
				if ( m_windowedPWP.showCmd == SW_SHOWMAXIMIZED )
				{
					ShowWindow (m_d3dpp.hDeviceWindow, SW_HIDE );
					m_windowedPWP.showCmd = SW_HIDE ;
					SetWindowPlacement(m_d3dpp.hDeviceWindow, &m_windowedPWP) ;
					ShowWindow (m_d3dpp.hDeviceWindow, SW_SHOWMAXIMIZED ) ;
					m_windowedPWP.showCmd = SW_SHOWMAXIMIZED ;
				}
				else
					SetWindowPlacement(m_d3dpp.hDeviceWindow, &m_windowedPWP ) ;
			}
		}
		else
		{
			m_d3dpp.FullScreen_RefreshRateInHz	 = D3DPRESENT_RATE_DEFAULT;
			m_d3dpp.BackBufferWidth				 = GetSystemMetrics(SM_CXSCREEN);
			m_d3dpp.BackBufferHeight			 = GetSystemMetrics(SM_CYSCREEN);

			GetWindowPlacement(m_d3dpp.hDeviceWindow, &m_windowedPWP ) ;
			m_dwExStyle = GetWindowLong(m_d3dpp.hDeviceWindow, GWL_EXSTYLE ) ;
			m_dwStyle = GetWindowLong(m_d3dpp.hDeviceWindow, GWL_STYLE ) ;
			m_dwStyle &= ~WS_MAXIMIZE & ~WS_MINIMIZE;
			SetWindowLong (m_d3dpp.hDeviceWindow, GWL_STYLE, WS_EX_TOPMOST | WS_POPUP |CS_DBLCLKS);
			SetWindowPos(m_d3dpp.hDeviceWindow,HWND_TOPMOST,0,0,1920,1080,SWP_SHOWWINDOW);
		}
		if (ResetDevice() &&
			RestoreDevice())
		{
			//m_pOldWndProc = (WNDPROC)GetWindowLong(m_d3dpp.hDeviceWindow,GWL_WNDPROC);
			//AttachWnd();			
		}
	}
	*/
	
	// �����豸��ʧ
	// m_bManualReset	Ĭ��Ϊfalse,ֻ���������豸��ʧ,Ϊtrueʱ���������������豸��ʧ,�Ա���Ӧ���ڳߴ�仯����ȫ���Ŵ��
	virtual bool HandelDevLost()
	{
		HRESULT hr = S_OK;

		if (!m_pDirect3DDevice)
			return false;
		
		hr = m_pDirect3DDevice->TestCooperativeLevel();
		if (FAILED(hr))
		{
			if (hr == D3DERR_DEVICELOST)
			{
				Sleep(25);
				return false;
			}
			else if (hr == D3DERR_DEVICENOTRESET)
			{
				if(ResetDevice())
				{
					return RestoreDevice();
				}
				else				
				{
					DxTraceMsg("%s ResetDevice Failed.\nhr=%x", __FUNCTION__,hr);
					return false;
				}
			}
			else
			{
				DxTraceMsg("%s UNKNOWN DEVICE ERROR!\nhr=%x", __FUNCTION__,hr);
				return false;
			}			
		}
		
				
		return true;
	}
	
	void CopyFrameYUV420P(byte *pDest,int nStride,AVFrame *pFrame420P)
	{
		int nSize = pFrame420P->height * nStride;			
		const size_t nHalfSize = (nSize) >> 1;	
		byte *pDestY = pDest;										// Y������ʼ��ַ
		byte *pDestV = pDest + nSize;								// U������ʼ��ַ
		byte *pDestU = pDestV + (size_t)(nHalfSize >> 1);			// V������ʼ��ַ

		// YUV420P��U��V�����Ե������ΪYV12��ʽ
		// ����Y����
		for (int i = 0; i < pFrame420P->height; i++)
			memcpy(pDestY + i * nStride, pFrame420P->data[0] + i * pFrame420P->linesize[0], pFrame420P->linesize[0]);

		// ����YUV420P��U������Ŀ���YV12��U����
		for (int i = 0; i < pFrame420P->height / 2; i++)
			memcpy(pDestU + i * nStride / 2, pFrame420P->data[1] + i * pFrame420P->linesize[1] , pFrame420P->linesize[1]);

		// ����YUV420P��V������Ŀ���YV12��V����
		for (int i = 0; i < pFrame420P->height / 2; i++)
			memcpy(pDestV + i * nStride / 2, pFrame420P->data[2] + i * pFrame420P->linesize[2] , pFrame420P->linesize[2]);
	}

	void CopyFrameARGB(byte *pDest,int nStride,AVFrame *pFrameARGB)
	{	
		for (int i = 0; i < pFrameARGB->height; i++)
			memcpy(pDest + i * nStride, pFrameARGB->data[0] + i * pFrameARGB->linesize[0], pFrameARGB->linesize[0]);
	}
	
	virtual bool Render(AVFrame *pAvFrame,HWND hWnd = NULL,RECT *pRenderRt = NULL)
	{
		//TraceMemory();
		if (!pAvFrame)
			return false;
		CTryLock Trylock;
		if (!Trylock.TryLock(&m_csRender))
			return false;

		HWND hRenderWnd = m_d3dpp.hDeviceWindow;
		if (hWnd)
			hRenderWnd = hWnd;
		if (m_d3dpp.Windowed && !IsNeedRender(hRenderWnd)  )
			return true;
			
		if (!HandelDevLost())
			return false;
		// HandelDevLost���޷�ʹ��m_pDirect3DDevice����ֱ�ӷ���false
		if (!m_pDirect3DDevice)
			return false;
		HRESULT hr = -1;
#ifdef _DEBUG
		double dfT1 = GetExactTime();
#endif
		//CAutoLock lock(&m_csRender,false,__FUNCTION__,__LINE__);	
		switch(pAvFrame->format)
		{
		case  AV_PIX_FMT_DXVA2_VLD:
			{// Ӳ����֡������ֱ����ʾ
				IDirect3DSurface9* pRenderSurface = m_pDirect3DSurfaceRender;	
				IDirect3DSurface9* pSurface = (IDirect3DSurface9 *)pAvFrame->data[3];
				if (m_bD3DShared)
				{		
					if (!pSurface)
						return false;
					pRenderSurface = pSurface;
				}
				else
				{
					D3DLOCKED_RECT SrcRect;
					D3DLOCKED_RECT DstRect;
					D3DSURFACE_DESC SrcSurfaceDesc, DstSurfaceDesc;
					pSurface->GetDesc(&SrcSurfaceDesc);
					m_pDirect3DSurfaceRender->GetDesc(&DstSurfaceDesc);
					hr = pSurface->LockRect(&SrcRect, nullptr, D3DLOCK_READONLY);					
					hr |= m_pDirect3DSurfaceRender->LockRect(&DstRect, NULL, D3DLOCK_DONOTWAIT);
					//DxTraceMsg("hr = %08X.\n", hr);

					if (FAILED(hr))
					{
						DxTraceMsg("%s line(%d) IDirect3DSurface9::LockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
						return false;
					}
					// ���Ʊ���ʱ�������YUV�������ֱ��ƣ�ֱ�Ӹ��Ʊ��漴�ɣ�ǰ���Ǳ���Ĳ���������ȫһ��
					gpu_memcpy(DstRect.pBits, SrcRect.pBits, SrcRect.Pitch*SrcSurfaceDesc.Height*3/2);
					/*
					// Y����ͼ��
					uint8_t *pY = (uint8_t*)DstRect.pBits;
					// UV����ͼ��
					uint8_t *pUV = (uint8_t*)DstRect.pBits + DstRect.Pitch * DstSurfaceDesc.Height;
					*/
	
					hr |= m_pDirect3DSurfaceRender->UnlockRect();
					hr |= pSurface->UnlockRect();
					if (FAILED(hr))
					{
						DxTraceMsg("%s line(%d) IDirect3DSurface9::UnlockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
						return false;
					}
				}
				// �����ͼ����
				TransferSnapShotSurface(pAvFrame);
				
				// �����ⲿ�ֻ��ƽӿ�
				ExternDrawCall(hWnd,pRenderRt);

				IDirect3DSurface9 * pBackSurface = NULL;
				m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
				m_pDirect3DDevice->BeginScene();
				hr = m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurface);
				D3DSURFACE_DESC desc;
				
				if (FAILED(hr))
				{
					m_pDirect3DDevice->EndScene();
					DxTraceMsg("%s line(%d) IDirect3DDevice9::GetBackBuffer failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return true;
				}
				pBackSurface->GetDesc(&desc);
				RECT dstrt = { 0, 0, desc.Width, desc.Height };
				RECT srcrt = { 0, 0, pAvFrame->width, pAvFrame->height };	
				hr = m_pDirect3DDevice->StretchRect(pRenderSurface, &srcrt, pBackSurface, &dstrt, D3DTEXF_LINEAR);
			
				pBackSurface->Release();
				m_pDirect3DDevice->EndScene();
#ifdef _DEBUG
				double dfT2 = GetExactTime();
				//DxTraceMsg("%s TimeSpan(T2-T1)\t%.6f\n",__FUNCTION__,dfT2 - dfT1);
#endif
				break;
			}
		case AV_PIX_FMT_YUV420P:
		case AV_PIX_FMT_YUVJ420P:		
			{// �����֡��ֻ֧��YUV420P��ʽ					
				TransferSnapShotSurface(pAvFrame);
				D3DLOCKED_RECT d3d_rect;
				D3DSURFACE_DESC Desc;
				hr = m_pDirect3DSurfaceRender->GetDesc(&Desc);
				hr |= m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
				//DxTraceMsg("hr = %08X.\n",hr);
				if (FAILED(hr))
				{
					DxTraceMsg("%s line(%d) IDirect3DSurface9::LockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return false;
				}
 				if ((pAvFrame->format == AV_PIX_FMT_YUV420P ||
					pAvFrame->format == AV_PIX_FMT_YUVJ420P) &&
					Desc.Format == (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'))
 					CopyFrameYUV420P((byte *)d3d_rect.pBits,d3d_rect.Pitch,pAvFrame);
 				else
				{
					if (!m_pPixelConvert)
#if _MSC_VER > 1600
						m_pPixelConvert = make_shared<PixelConvert>(pAvFrame,Desc.Format);
#else
						m_pPixelConvert = shared_ptr<PixelConvert>(new PixelConvert(pAvFrame,Desc.Format));
#endif
					m_pPixelConvert->ConvertPixel(pAvFrame);
					if (m_pPixelConvert->GetDestPixelFormat() == AV_PIX_FMT_YUV420P)
						CopyFrameYUV420P((byte *)d3d_rect.pBits,d3d_rect.Pitch,m_pPixelConvert->pFrameNew);
					else					
						memcpy((byte *)d3d_rect.pBits,m_pPixelConvert->pImage,m_pPixelConvert->nImageSize);
				}
				hr = m_pDirect3DSurfaceRender->UnlockRect();
				if (FAILED(hr))
				{
					DxTraceMsg("%s line(%d) IDirect3DSurface9::UnlockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return false;
				}

				// �����ⲿ�ֻ��ƽӿ�
				ExternDrawCall(hWnd,pRenderRt);

				IDirect3DSurface9 * pBackSurface = NULL;	
				m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
				m_pDirect3DDevice->BeginScene();
				hr = m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurface);
				if (FAILED(hr))
				{
					m_pDirect3DDevice->EndScene();
					DxTraceMsg("%s line(%d) IDirect3DDevice9::GetBackBuffer failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return false;
				}	
				pBackSurface->GetDesc(&Desc);
				RECT dstrt = { 0, 0, Desc.Width, Desc.Height };
				RECT srcrt = { 0, 0, pAvFrame->width, pAvFrame->height };	
				hr = m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, &srcrt, pBackSurface, &dstrt, D3DTEXF_LINEAR);
				
				SafeRelease(pBackSurface);
				m_pDirect3DDevice->EndScene();
#ifdef _DEBUG
				double dfT2 = GetExactTime();
				//DxTraceMsg("%s TimeSpan(T2-T1)\t%.6f\n",__FUNCTION__,dfT2 - dfT1);
#endif
			}
			break;
		case AV_PIX_FMT_NONE:
			{
				DxTraceMsg("*************************************.\n");
				DxTraceMsg("*%s	Get a None picture Frame error	*.\n",__FUNCTION__);
				DxTraceMsg("*************************************.\n");
				return true;
			}
			break;
		default:
			{
				DxTraceMsg("%s Get a unsupport format frame:%d.\n",pAvFrame->format);
				return true;
			}
		}
#ifdef _DEBUG
		double dfT3 = GetExactTime();
#endif
		// Present(RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
		
		hr = m_pDirect3DDevice->Present(NULL, pRenderRt, hRenderWnd, NULL);
		
#ifdef _DEBUG
		double dfT4 = GetExactTime();
		//DxTraceMsg("%s TimeSpan(T3-T4)\t%.6f\n",__FUNCTION__,dfT4 - dfT3);
#endif
		if (SUCCEEDED(hr))
			return true;
		else
			return HandelDevLost();	
	}

	// �������Թ̶��ȱ���ʾ��Ƶ
	// ��bScaleFixedΪtrue,����dfWHScale = 0ʱ,��ʹ��ͼ��ԭʼ����,����ֵΪWidth/Height
	// ��bScaleFixedΪtrue,����dfWHScale ����0ʱ,��ʹ��dfWHScale�ṩ�ı�����ʾ��ͼ����ܻᱻ�������
	// ��bScaleFixedΪfalse,dfWHScale������������,��ʱ���Զ��������ڿͻ���
	void SetScaleFixed(bool bSaleFixed = true,float fWHScale = 0.0f)
	{
		m_bVideoScaleFixed = bSaleFixed;
		m_fWHScale = fWHScale;
	}

	bool GetScale(float &fWHScale)
	{
		fWHScale = m_fWHScale;
		return m_bVideoScaleFixed;
	}
/*
	// ������ʾ�����Ƿ�Ҫ�����໯
	// bWndSubclassΪture����AttachWndʱ����ʾ���ڽ������໯�����򲻻�ִ�����໯����
	// �˺���������AttachWndǰִ�в���Ч
	inline void SetWndSubclass(bool bWndSubclass = true)
	{
		m_bWndSubclass = bWndSubclass;
	}
	inline bool IsWndSubclass()
	{
		return m_bWndSubclass;
	}
	
	bool AttachWnd()
	{
		CAutoLock lock(m_WndSurfaceMapcs->Get());
		WndSurfaceMap::iterator itFind = m_WndSurfaceMap.find(m_d3dpp.hDeviceWindow);
		if (itFind != m_WndSurfaceMap.end())
			return false;
		
 		m_pOldWndProc = (WNDPROC)GetWindowLong(m_d3dpp.hDeviceWindow,GWL_WNDPROC);		
 		if (!m_pOldWndProc)
 			return false;
 		else
 		{
 			if (SetWindowLong(m_d3dpp.hDeviceWindow,GWL_WNDPROC,(long)WndProc))
 			{
 				m_WndSurfaceMap.insert(pair<HWND,CDxSurface*>(m_d3dpp.hDeviceWindow,this));
 				return true;
 			}
 			else			
  				return false;
 		}
	}

	void DetachWnd()
	{
		if (!m_pOldWndProc)
			return;

		if (m_pOldWndProc)
			SetWindowLong(m_d3dpp.hDeviceWindow,GWL_WNDPROC,(long)m_pOldWndProc);
		CAutoLock lock(m_WndSurfaceMapcs->Get());
		m_WndSurfaceMap.erase(m_d3dpp.hDeviceWindow);
		m_pOldWndProc = NULL;
	}
	
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		DxTraceMsg("%s Message = %d(%04X).\n",__FUNCTION__,message,message);
		switch (message)
		{
// 		case WM_RENDERFRAME:
// 			{
// 				if (!wParam || !lParam)
// 					return -1;
// 
// 				AVFrame *pAvFrame = (AVFrame *)lParam;
// 				CDxSurface *pSurface = (CDxSurface *)wParam;
// 				if (!pSurface->IsInited())		// D3D�豸��δ����,˵��δ��ʼ��
// 				{
// 					if (!pSurface->InitD3D(pAvFrame->width,pAvFrame->height))
// 					{
// 						assert(false);
// 						return 0;
// 					}
// 				}
//  				pSurface->Render(pAvFrame);	
// 				break;
// 			}
		case WM_LBUTTONDBLCLK:
			{// �л�����ģʽ,Ҫ��m_WndSurfaceMapɾ�����ھ��
				CAutoLock lock(m_WndSurfaceMapcs->Get());
				WndSurfaceMap::iterator itFind = m_WndSurfaceMap.find(hWnd);
				if (itFind == m_WndSurfaceMap.end())					
					return DefWindowProc(hWnd, message, wParam, lParam);	
				else
				{
					CDxSurface *pSurface = itFind->second;
					m_WndSurfaceMap.erase(itFind);
					SetWindowLong(pSurface->m_d3dpp.hDeviceWindow,GWL_WNDPROC,(LONG)pSurface->m_pOldWndProc);
					pSurface->SwitchFullScreen();
					return 0L;
				}
				break;
			}
		default:
			{
				CAutoLock lock(m_WndSurfaceMapcs->Get());
				WndSurfaceMap::iterator itFind = m_WndSurfaceMap.find(hWnd);
				if (itFind == m_WndSurfaceMap.end())					
					return DefWindowProc(hWnd, message, wParam, lParam);	
				else
				{
					CDxSurface *pSurface = itFind->second;
					return pSurface->m_pOldWndProc(hWnd, message, wParam, lParam);
				}
			}
		}
		return 0l;
	}
*/
public:
	// 1.���ָ���ı������ظ�ʽ���Ƿ���ָ�������������͡����������ظ�ʽ�¿��á�
	// GetAdapterDisplayMode,CheckDeviceType��Ӧ��
	bool GetBackBufferFormat(D3DDEVTYPE deviceType,BOOL bWindow, D3DFORMAT &fmt)
	{
		if(m_pDirect3D9 == NULL)
			return false;

		D3DDISPLAYMODE adapterMode;
		m_pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&adapterMode);
		if(D3D_OK != m_pDirect3D9->CheckDeviceType(D3DADAPTER_DEFAULT,deviceType, adapterMode.Format, fmt, bWindow))
			fmt = adapterMode.Format;
		return true;
	}

	// 2.�������������ͣ���ȡ��������(�任�͹�������)�ĸ�ʽ
	// D3DCAPS9�ṹ�壬GetDeviceCaps��Ӧ��
	bool GetDisplayVertexType(D3DDEVTYPE deviceType, int &nVertexType)
	{
		if(m_pDirect3D9 == NULL)
			return false;

		D3DCAPS9 caps;
		m_pDirect3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType,&caps);
		if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
			nVertexType = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
			nVertexType = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		return true;

	}

	char* StringFromGUID(GUID *pGuid)
	{
		static char szGuidStringA[64] = {0};
		WCHAR szGuidStringW[64] = {0};
		StringFromGUID2(*pGuid,szGuidStringW,64);
		WideCharToMultiByte(CP_ACP,0,szGuidStringW,0,(LPSTR)szGuidStringA,64,NULL,NULL);
		return szGuidStringA;
	}

	// 3.����Կ���Ϣ,Description�����������ͺţ�Dircet3D������Driver�汾�ţ��Կ���Ψһ��ʶ�ţ�DeviceIdentifier
	// GetAdapterCount()��GetAdapterIdentifier��ʹ�á�
	void PrintDisplayInfo()
	{
		if(m_pDirect3D9 == NULL)
			return;

		D3DADAPTER_IDENTIFIER9 adapterID; // Used to store device info
		DWORD dwDisplayCount = m_pDirect3D9->GetAdapterCount();
		for(DWORD i = 0; i < dwDisplayCount; i++)
		{
			if( m_pDirect3D9->GetAdapterIdentifier( i/*D3DADAPTER_DEFAULT*/, 0,&adapterID ) != D3D_OK )
				return;

			DxTraceMsg("Driver: %s.\n",adapterID.Driver);
			DxTraceMsg("Description: %s\n",adapterID.Description);
			DxTraceMsg("Device Name: %s\n",adapterID.DeviceName);
			DxTraceMsg("Vendor id:%4x\n",adapterID.VendorId);
			DxTraceMsg("Device id: %4x\n",adapterID.DeviceId);
			DxTraceMsg("Product: %x\n",HIWORD(adapterID.DriverVersion.HighPart));
			DxTraceMsg("Version:%x\n",LOWORD(adapterID.DriverVersion.HighPart));
			DxTraceMsg("SubVersion: %x\n",HIWORD(adapterID.DriverVersion.LowPart));
			DxTraceMsg("Build: %x %d.%d.%d.%d\n",LOWORD(adapterID.DriverVersion.LowPart),
												 HIWORD(adapterID.DriverVersion.HighPart),
												 LOWORD(adapterID.DriverVersion.HighPart),
												 HIWORD(adapterID.DriverVersion.LowPart),
												 LOWORD(adapterID.DriverVersion.LowPart));
			DxTraceMsg("SubSysId: %x\n, Revision: %x\n,GUID %s\n, WHQLLevel:%d\n",
						adapterID.SubSysId, 
						adapterID.Revision,
						StringFromGUID(&adapterID.DeviceIdentifier), 
						adapterID.WHQLLevel);
		}
	}

	// 4.���ָ��Adapter���Կ�����ģʽ(�����뻺������ʽ�����ݿ���)���Կ�������ģʽ��Ϣ
	// GetAdapterModeCount,EnumAdapterModes��ʹ��
	void PrintDisplayModeInfo(D3DFORMAT fmt)
	{
		if(m_pDirect3D9 == NULL)
		{
			DxTraceMsg("%s Direct3D9 not initialized.\n",__FUNCTION__);
			return;
		}
		// �Կ�������ģʽ�ĸ�������Ҫ�Ƿֱ��ʵĲ���
		DWORD nAdapterModeCount=m_pDirect3D9->GetAdapterModeCount(D3DADAPTER_DEFAULT, fmt);
		if(nAdapterModeCount == 0)
		{
			DxTraceMsg("%s D3DFMT_��ʽ��%x��֧��", __FUNCTION__,fmt);
		}
		for(DWORD i = 0; i < nAdapterModeCount; i++)
		{
			D3DDISPLAYMODE mode;
			if(D3D_OK == m_pDirect3D9->EnumAdapterModes(D3DADAPTER_DEFAULT,fmt, i,&mode))
				DxTraceMsg( "D3DDISPLAYMODE info, width:%u,height:%u, freshRate:%u, Format:%d \n",
							mode.Width, 
							mode.Height, 
							mode.RefreshRate,
							mode.Format);
		}
	}

	// 5.����ָ������Դ���ͣ������Դ��ʹ�÷�ʽ����Դ���ظ�ʽ����Ĭ�ϵ��Կ����������Ƿ�֧��
	// GetAdapterDisplayMode��CheckDeviceFormat��ʹ��
	bool CheckResourceFormat(DWORD nSrcUsage,D3DRESOURCETYPE srcType, D3DFORMAT srcFmt)
	{
		if(m_pDirect3D9 == NULL)
			return false;

		D3DDISPLAYMODE displayMode;
		m_pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&displayMode);
		if(D3D_OK == m_pDirect3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL, displayMode.Format, nSrcUsage, srcType, srcFmt))
			return true;
		return false;
	}

	// 6.��ָ���ı������ظ�ʽ������ģʽ�����Կ�����ģʽ������ָ���Ķ��ز�������֧�ֲ����ҷ�������ˮƽ�ȼ�
	// CheckDeviceMultiSampleType��Ӧ��
	bool CheckMultiSampleType(D3DFORMAT surfaceFmt,BOOL bWindow, D3DMULTISAMPLE_TYPE &eSampleType, DWORD *pQualityLevel)
	{
		//����MultiSampleType��ֵ��ΪD3DMULTISAMPLE_NONMASKABLE���ͱ����趨��Ա����MultiSampleQuality�������ȼ�ֵ
		for (int i = eSampleType;i >= D3DMULTISAMPLE_NONE;i --)
		{
			if( SUCCEEDED(m_pDirect3D9->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT/*caps.AdapterOrdinal*/,
						D3DDEVTYPE_HAL/*caps.DeviceType*/,
						surfaceFmt,
						bWindow,
						(D3DMULTISAMPLE_TYPE)i,
						pQualityLevel)))
					{
						eSampleType = (D3DMULTISAMPLE_TYPE)i;
						return true;
					}
		}
		
		return false;
	}

	// 7.�����Կ���������Ŀ�껺�����ͣ����ָ����Ȼ���ĸ�ʽ�Ƿ�֧��
	// CheckDepthStencilMatch��Ӧ��
	bool CheckDepthBufferFormt(D3DFORMAT targetBufferFmt, D3DFORMAT depthFmt)
	{
		if(m_pDirect3D9 == NULL)
			return false;

		D3DDISPLAYMODE mode;
		m_pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
		if(D3D_OK == m_pDirect3D9->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mode.Format, targetBufferFmt, depthFmt))
			return true;
		return false;
	}

	bool TestDxCheck(HWND hWnd,int nWidth,int nHeight)
	{
		// 1.CheckDeviceFormat,�Կ�ģʽ�ͱ���������ݴ���
		D3DFORMAT backBufferFormat = D3DFMT_A8R8G8B8;
		BOOL bIsWindowed = FALSE;
		D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;

		if(!GetBackBufferFormat(D3DDEVTYPE_HAL, bIsWindowed, backBufferFormat))
		{
			DxTraceMsg("%s GetBackBufferFormat - failed.\n", __FUNCTION__);
			return false;
		}

		// 2.Check Vertex Proccessing Type
		int vp = 0;
		if(!GetDisplayVertexType(deviceType, vp))
		{
			DxTraceMsg("%s GetDisplayVertexType - failed.\n", __FUNCTION__);
			return false;
		}
		// 3.��ʾ�Կ�����Ϣ
		PrintDisplayInfo();

		D3DDISPLAYMODE d3ddm;
		if(FAILED(m_pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm)))
			return false;
		// 4.����Կ�������ģʽ��Ϣ,�����뻺������ʽ�����ݿ���
		PrintDisplayModeInfo(d3ddm.Format);

		// 5.�Կ�ģʽ����Դ����������ݴ���
		int nUsageTexture = D3DUSAGE_WRITEONLY;
		D3DFORMAT fmtTexture = D3DFMT_A8R8G8B8;

		if(!CheckResourceFormat(nUsageTexture/*D3DUSAGE_DEPTHSTENCIL*/,
			D3DRTYPE_TEXTURE/*D3DRTYPE_SURFACE*/,d3ddm.Format/*D3DFMT_D15S1*/))
		{
			DxTraceMsg("%s CheckResourceFormat Texture Resource FMT failed.\n"__FUNCTION__);
			return false;
		}

		// 6.��������
		D3DMULTISAMPLE_TYPE eSampleType = D3DMULTISAMPLE_16_SAMPLES;// ���Խ������ֻ��֧��4,2���͵Ĳ�������
		DWORD dwQualityLevel = 0;
		if(!CheckMultiSampleType(d3ddm.Format, bIsWindowed,  eSampleType,&dwQualityLevel))
		{
			eSampleType = D3DMULTISAMPLE_NONE;
		}

		// 7.��Ȼ�����
		D3DFORMAT depthStencilFmt = D3DFMT_D24X8/*D3DFMT_D15S1*/;
		if(!CheckDepthBufferFormt(d3ddm.Format, depthStencilFmt))
		{
			DxTraceMsg("%s CheckDepthBufferFormt Texture Resource FMT Failed.\n", __FUNCTION__);
			return false;
		}

		// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp.BackBufferWidth           = nWidth;
		d3dpp.BackBufferHeight          = nHeight;
		d3dpp.BackBufferFormat          = backBufferFormat;
		d3dpp.BackBufferCount           = 1;
		d3dpp.MultiSampleType           = eSampleType;				//D3DMULTISAMPLE_NONE;
		d3dpp.MultiSampleQuality        = dwQualityLevel;				// ������dwQualityLevel
		d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow             = hWnd;
		d3dpp.Windowed                  = bIsWindowed;
		d3dpp.EnableAutoDepthStencil    = true;
		d3dpp.AutoDepthStencilFormat    = depthStencilFmt;
		d3dpp.Flags                     = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval      = D3DPRESENT_INTERVAL_IMMEDIATE;
		return true;
	}
};

// CDxSurfaceEx�࣬������Windows Vista�����ϲ���ϵͳ��ʹ��
// ���������ȶ��Ա�CDxSurfaceҪǿ��ά��Ҳ������

class CDxSurfaceEx :public CDxSurface
{
private:
	IDirect3D9Ex			*m_pDirect3D9Ex		/* = NULL*/;
	IDirect3DDevice9Ex		*m_pDirect3DDeviceEx	/*= NULL*/;
	pDirect3DCreate9Ex*		m_pDirect3DCreate9Ex;
public:
	explicit CDxSurfaceEx(IDirect3D9Ex *pD3D9Ex)
		:m_pDirect3D9Ex(NULL)
		,m_pDirect3DDeviceEx(NULL)
		,m_pDirect3DCreate9Ex(NULL)
	{
		InitializeCriticalSection(&m_csRender);
		if (pD3D9Ex)
		{
			m_bD3DShared = true;
			m_pDirect3D9Ex = pD3D9Ex;
		}
	}
	CDxSurfaceEx()
		:m_pDirect3D9Ex(NULL)
		,m_pDirect3DDeviceEx(NULL)
		,m_pDirect3DCreate9Ex(NULL)
	{
		TraceFunction();		
		if (!m_hD3D9)
			m_hD3D9 = LoadLibraryA("d3d9.dll");
		if (!m_hD3D9)
		{
			DxTraceMsg("%s Failed load D3d9.dll.\n",__FUNCTION__);
			assert(false);
			return;
		}
		// �ͷ��ɻ��ഴ����Direct3D9����
		SafeRelease(m_pDirect3D9);
		m_pDirect3DCreate9Ex = (pDirect3DCreate9Ex*)GetProcAddress(m_hD3D9, "Direct3DCreate9Ex");
		if (!m_pDirect3DCreate9Ex)
		{
			DxTraceMsg("%s Can't locate the Procedure \"Direct3DCreate9Ex\".\n",__FUNCTION__);
			assert(false);
			return;
		}		
		HRESULT hr = m_pDirect3DCreate9Ex(D3D_SDK_VERSION, &m_pDirect3D9Ex);
		if (FAILED(hr))
		{
			DxTraceMsg("%s Direct3DCreate9Ex failed.\n",__FUNCTION__);
			assert(false);
		}
	}
#ifdef _DEBUG
	virtual void OutputDxPtr()
	{
		DxTraceMsg("%s m_pDirect3D9Ex = %p\tm_pDirect3DDeviceEx = %p\tm_pDirect3DSurfaceRender = %p.\n", __FUNCTION__, m_pDirect3D9Ex, m_pDirect3DDeviceEx, m_pDirect3DSurfaceRender);
	}
#endif
	virtual void DxCleanup()
	{
		SafeRelease(m_pDirect3DSurfaceRender);
		SafeRelease(m_pDirect3DDeviceEx);
	}
	~CDxSurfaceEx()
	{
		DxCleanup();
		SafeRelease(m_pDirect3D9Ex);
		if (m_hD3D9)
		{
			FreeLibrary(m_hD3D9);
			m_hD3D9 = NULL;
		}
	}

	// ����InitD3D֮ǰ�����ȵ���AttachWnd����������Ƶ��ʾ����
	// nD3DFormat ����Ϊ���¸�ʽ֮һ
	// MAKEFOURCC('Y', 'V', '1', '2')	Ĭ�ϸ�ʽ,���Ժܷ������YUV420Pת���õ�,��YUV420P��FFMPEG�����õ���Ĭ�����ظ�ʽ
	// MAKEFOURCC('N', 'V', '1', '2')	��DXVAӲ����ʹ�øø�ʽ
	// D3DFMT_R5G6B5
	// D3DFMT_X1R5G5B5
	// D3DFMT_A1R5G5B5
	// D3DFMT_R8G8B8
	// D3DFMT_X8R8G8B8
	// D3DFMT_A8R8G8B8
	bool InitD3D(HWND hWnd,
		int nVideoWidth,
		int nVideoHeight,
		BOOL bIsWindowed = TRUE,
		D3DFORMAT nD3DFormat = (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'))
	{
		TraceFunction();
		assert(hWnd != NULL);
		assert(IsWindow(hWnd));
		assert(nVideoWidth != 0 || nVideoHeight != 0);

		RECT rt;
		bool bZoomWnd = false;			// �Ƿ���Ҫ���󴰿�,DirectX�����ڴ����������Ϊ0�Ĵ����Ϲ���
		GetWindowRect(hWnd, &rt);
		if ((rt.right - rt.left) == 0)
		{
			rt.right = rt.left + nVideoWidth;
			bZoomWnd = true;
		}
		if ((rt.bottom - rt.top) == 0)
		{
			rt.bottom = rt.top + nVideoHeight;
			bZoomWnd = true;
		}

		if (bZoomWnd)
			::MoveWindow(hWnd, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top,false);

		bool bSucceed = false;
#ifdef _DEBUG
		double dfTStart = GetExactTime();		
#endif
		D3DCAPS9 caps;
		m_pDirect3D9Ex->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
		int vp = 0;
		if (caps.DevCaps& D3DDEVCAPS_HWTRANSFORMANDLIGHT)		
			vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else		
			vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		/*
		caps.DevCaps��ȡֵ������
		D3DDEVCAPS_CANBLTSYSTONONLOCAL		Device supports blits from system-memory textures to nonlocal video-memory textures.
		D3DDEVCAPS_CANRENDERAFTERFLIP		Device can queue rendering commands after a page flip. Applications do not change their behavior if this flag is set; this capability means that the device is relatively fast.
		D3DDEVCAPS_DRAWPRIMITIVES2			Device can support at least a DirectX 5-compliant driver.
		D3DDEVCAPS_DRAWPRIMITIVES2EX		Device can support at least a DirectX 7-compliant driver.
		D3DDEVCAPS_DRAWPRIMTLVERTEX			Device exports an IDirect3DDevice9::DrawPrimitive-aware hal.
		D3DDEVCAPS_EXECUTESYSTEMMEMORY		Device can use execute buffers from system memory.
		D3DDEVCAPS_EXECUTEVIDEOMEMORY		Device can use execute buffers from video memory.
		D3DDEVCAPS_HWRASTERIZATION			Device has hardware acceleration for scene rasterization.
		D3DDEVCAPS_HWTRANSFORMANDLIGHT		Device can support transformation and lighting in hardware.
		D3DDEVCAPS_NPATCHES					Device supports N patches.
		D3DDEVCAPS_PUREDEVICE				Device can support rasterization, transform, lighting, and shading in hardware.
		D3DDEVCAPS_QUINTICRTPATCHES			Device supports quintic B��zier curves and B-splines.
		D3DDEVCAPS_RTPATCHES				Device supports rectangular and triangular patches.
		D3DDEVCAPS_RTPATCHHANDLEZERO		When this device capability is set, the hardware architecture does not require caching of any information, and uncached patches (handle zero) will be drawn as efficiently as cached ones. Note that setting D3DDEVCAPS_RTPATCHHANDLEZERO does not mean that a patch with handle zero can be drawn. A handle-zero patch can always be drawn whether this cap is set or not.
		D3DDEVCAPS_SEPARATETEXTUREMEMORIES	Device is texturing from separate memory pools.
		D3DDEVCAPS_TEXTURENONLOCALVIDMEM	Device can retrieve textures from non-local video memory.
		D3DDEVCAPS_TEXTURESYSTEMMEMORY		Device can retrieve textures from system memory.
		D3DDEVCAPS_TEXTUREVIDEOMEMORY		Device can retrieve textures from device memory.
		D3DDEVCAPS_TLVERTEXSYSTEMMEMORY		Device can use buffers from system memory for transformed and lit vertices.
		D3DDEVCAPS_TLVERTEXVIDEOMEMORY		Device can use buffers from video memory for transformed and lit vertices.
		*/
		HRESULT hr = S_OK;		
		D3DDISPLAYMODE d3ddm;
		if (FAILED(hr = m_pDirect3D9Ex->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
		{
			DxTraceMsg("%s GetAdapterDisplayMode Failed.\nhr=%x", __FUNCTION__, hr);
			goto _Failed;
		}
		
		ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
		m_d3dpp.BackBufferFormat		= d3ddm.Format;
		m_d3dpp.BackBufferCount			= 1;
		m_d3dpp.Flags					= 0;
		m_d3dpp.Windowed				= bIsWindowed;
		m_d3dpp.hDeviceWindow			= hWnd;
		m_d3dpp.MultiSampleQuality		= 0;
		m_d3dpp.MultiSampleType			= D3DMULTISAMPLE_NONE;					// ��ʾ��Ƶʱ������ʹ�ö��ز��������򽫵��»������
#pragma warning(push)
#pragma warning(disable:4800)
		m_bFullScreen					= (bool)bIsWindowed;
#pragma warning(pop)		

		if (bIsWindowed)//����ģʽ
		{
			if (m_dwStyle)
				SetWindowLong(m_d3dpp.hDeviceWindow, GWL_STYLE, m_dwStyle);
			if (m_dwExStyle)
				SetWindowLong(m_d3dpp.hDeviceWindow, GWL_EXSTYLE, m_dwExStyle);
			if (m_WndPlace.length == sizeof(WINDOWPLACEMENT))
				SetWindowPlacement(m_d3dpp.hDeviceWindow, &m_WndPlace) ;
			
			if (!m_bEnableVsync)
				m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			else
				m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
						
			m_d3dpp.FullScreen_RefreshRateInHz	= 0;							// ��ʾ��ˢ���ʣ�����ģʽ��ֵ����Ϊ0
			m_d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;		// ָ��ϵͳ��ν���̨�����������ݸ��Ƶ�ǰ̨������ D3DSWAPEFFECT_DISCARD:�����̨���������
			m_d3dpp.BackBufferHeight			= nVideoHeight;
			m_d3dpp.BackBufferWidth				= nVideoWidth;
			m_d3dpp.EnableAutoDepthStencil		= FALSE;							// �ر��Զ���Ȼ���
		}
		else
		{
			//ȫ��ģʽ
			m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
			m_d3dpp.FullScreen_RefreshRateInHz	= d3ddm.RefreshRate;	
			m_d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD; 
			m_d3dpp.EnableAutoDepthStencil		= FALSE;			
			m_d3dpp.BackBufferWidth				= GetSystemMetrics(SM_CXSCREEN);		// �����Ļ��
			m_d3dpp.BackBufferHeight			= GetSystemMetrics(SM_CYSCREEN);		// �����Ļ��

			GetWindowPlacement(m_d3dpp.hDeviceWindow, &m_WndPlace ) ;
			m_dwExStyle	 = GetWindowLong( m_d3dpp.hDeviceWindow, GWL_EXSTYLE ) ;
			m_dwStyle	 = GetWindowLong( m_d3dpp.hDeviceWindow, GWL_STYLE ) ;
			m_dwStyle	 &= ~WS_MAXIMIZE & ~WS_MINIMIZE; // remove minimize/maximize style
			m_hMenu		 = GetMenu( m_d3dpp.hDeviceWindow ) ;
		}
		/*
		CreateDevice��BehaviorFlags����ѡ�
		D3DCREATE_ADAPTERGROUP_DEVICEֻ�����Կ���Ч�����豸�������������ӵ�е�������ʾ���
		D3DCREATE_DISABLE_DRIVER_MANAGEMENT�����豸������������Դ�������ڷ�����Դ����ʱD3D���ò���ʧ��
		D3DCREATE_DISABLE_PRINTSCREEN:��ע�������ݼ���ֻ��Direct3D 9Ex
		D3DCREATE_DISABLE_PSGP_THREADING��ǿ�Ƽ��㹤�����������߳��ϣ�vista������Ч
		D3DCREATE_ENABLE_PRESENTSTATS������GetPresentStatistics�ռ�ͳ����Ϣֻ��Direct3D 9Ex
		D3DCREATE_FPU_PRESERVE��ǿ��D3D���߳�ʹ����ͬ�ĸ��㾫�ȣ��ή������
		D3DCREATE_HARDWARE_VERTEXPROCESSING��ָ��Ӳ�����ж��㴦���������D3DCREATE_PUREDEVICE
		D3DCREATE_MIXED_VERTEXPROCESSING��ָ����϶��㴦��
		D3DCREATE_SOFTWARE_VERTEXPROCESSING��ָ������Ķ��㴦��
		D3DCREATE_MULTITHREADED��Ҫ��D3D���̰߳�ȫ�ģ����߳�ʱ
		D3DCREATE_NOWINDOWCHANGES��ӵ�в��ı䴰�ڽ���
		D3DCREATE_PUREDEVICE��ֻ��ͼʹ�ô�Ӳ������Ⱦ
		D3DCREATE_SCREENSAVER�������������ֻ��Direct3D 9Ex
		D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DCREATE_MIXED_VERTEXPROCESSING, and D3DCREATE_SOFTWARE_VERTEXPROCESSING��������һ��һ��Ҫ����
		*/
		if (m_pDirect3DDeviceEx)
			SafeRelease(m_pDirect3DDeviceEx);

		if (FAILED(hr = m_pDirect3D9Ex->CreateDeviceEx(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			m_d3dpp.hDeviceWindow,
			vp ,
			&m_d3dpp,
			NULL,
			&m_pDirect3DDeviceEx)))
		{
			vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			if (FAILED(hr = m_pDirect3D9Ex->CreateDeviceEx(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				m_d3dpp.hDeviceWindow,
				vp,
				&m_d3dpp,
				NULL,
				&m_pDirect3DDeviceEx)))
			{
				DxTraceMsg("%s CreateDeviceEx Failed.\thr=%x.\n", __FUNCTION__, hr);
				goto _Failed;
			}
		}

		if (m_pDirect3DSurfaceRender)
			SafeRelease(m_pDirect3DSurfaceRender);
		if (FAILED(hr = m_pDirect3DDeviceEx->CreateOffscreenPlainSurface(nVideoWidth,
			nVideoHeight,
			nD3DFormat,
			D3DPOOL_DEFAULT,
			&m_pDirect3DSurfaceRender,
			NULL)))
		{
			DxTraceMsg("%s CreateOffscreenPlainSurface Failed.\thr=%x.\n", __FUNCTION__, hr);
			goto _Failed;
		}

		D3DSURFACE_DESC SrcSurfaceDesc;			
		m_pDirect3DSurfaceRender->GetDesc(&SrcSurfaceDesc);
		// �������
		m_nVideoWidth	 = nVideoWidth;
		m_nVideoHeight	 = nVideoHeight;
		m_nD3DFormat	 = nD3DFormat;		
		bSucceed		 = true;
		m_bInitialized	 = true;
_Failed:
		if (!bSucceed)
		{
			DxCleanup();
		}
		return bSucceed;
	}

	// D3dDirect9Ex��,ȡ��ResetDevice()����
	bool  ResetDevice()
	{
		HRESULT hr = S_OK;
		// ʹ��D3d9DeviceExʱ���������´���������Դ
		if (!m_d3dpp.Windowed)
		{
			D3DDISPLAYMODEEX	DispMode;
			DispMode.Size			= sizeof(D3DDISPLAYMODEEX);
			DispMode.Width			= GetSystemMetrics(SM_CXSCREEN);
			DispMode.Height			= GetSystemMetrics(SM_CYSCREEN);
			DispMode.RefreshRate	= m_d3dpp.FullScreen_RefreshRateInHz;
			DispMode.Format			= m_d3dpp.BackBufferFormat;
			DispMode.ScanLineOrdering= D3DSCANLINEORDERING_PROGRESSIVE;
			hr = m_pDirect3DDeviceEx->ResetEx(&m_d3dpp,&DispMode);
		}
		else
			hr = m_pDirect3DDeviceEx->ResetEx(&m_d3dpp,NULL);
		if (FAILED(hr))
		{
			DxTraceMsg("%s IDirect3DDevice9Ex::ResetEx Failed,hr = %08X.\n",__FUNCTION__,hr);
			return false;
		}
		return true;
	}

	// D3dDirect9Ex�£��ó�Ա������Ч
	virtual bool RestoreDevice()
	{
		return true;
	}
	bool HandelDevLost()
	{
		HRESULT hr = S_OK;
		if (!m_pDirect3DDeviceEx)
			return false;

		hr = m_pDirect3DDeviceEx->CheckDeviceState(m_d3dpp.hDeviceWindow);
		switch(hr)
		{
		case S_OK:
			{
				return true;
			}
			break;
		case S_PRESENT_MODE_CHANGED:
			{
				DxTraceMsg("%s The display mode has changed.\n",__FUNCTION__);
				return ResetDevice();
			}
			break;
		case S_PRESENT_OCCLUDED:	// ���ڱ���������ģʽ�Ļ����ڵ���ȫ�����汻��С��,��Ϊȫ�����ڣ�
			// ���ʱ��ֹͣ��Ⱦ��ֱ�ӵ��յ�WM_ACTIVATEAPPʱ�������¿�ʼ��Ⱦ
			{
				DxTraceMsg("%s The window is occluded.\n",__FUNCTION__);
			}
			break;
		case D3DERR_DEVICELOST:
			{
				DxTraceMsg("%s The device has been lost.\n",__FUNCTION__);
				return ResetDevice();
			}
			break;
		case D3DERR_DEVICENOTRESET:	// �豸��ʧ,��������Ҫ���´���������Դ
			{
				DxTraceMsg("%s The device is not Reset.\n",__FUNCTION__);
				return ResetDevice();
			}
			break;
		case D3DERR_DEVICEHUNG:		// ��Ҫ��λIDirect3DDeviceEx����,������Ҫ�ؽ�IDirect3DEx����
			{
				DxTraceMsg("%s The device is hung.\n",__FUNCTION__);
				DxCleanup();
				if (FAILED(hr))
					return false;
				return InitD3D(m_d3dpp.hDeviceWindow,m_nVideoWidth,m_nVideoHeight,m_bFullScreen,m_d3dpp.BackBufferFormat);	
			}
			break;
		case D3DERR_DEVICEREMOVED:	// ��Ҫ���´���IDirect3DEx����
			{
				DxTraceMsg("%s if the device has been removed.\n",__FUNCTION__);
				DxCleanup();
				SafeRelease(m_pDirect3D9Ex);
				hr = m_pDirect3DCreate9Ex(D3D_SDK_VERSION, &m_pDirect3D9Ex);
				if (FAILED(hr))
					return false;
				return InitD3D(m_d3dpp.hDeviceWindow,m_nVideoWidth,m_nVideoHeight,m_bFullScreen,m_d3dpp.BackBufferFormat);	
			}
			break;
		default:
			return false;
		}

		return true;
	}
	// ����ץͼ����Surface�е�ͼ�����ݱ��浽�ļ��У��˽�ͼ�õ���ͼ����ԭʼ��ͼ��
// 	virtual bool SaveSurfaceToFileA(CHAR *szFilePath, D3DXIMAGE_FILEFORMAT D3DImageFormat = D3DXIFF_JPG)
// 	{
// 		if (szFilePath && strlen(szFilePath) > 0)
// 		{
// 			WCHAR szFilePathW[1024] = { 0 };
// 			MultiByteToWideChar(CP_ACP, 0, szFilePath, -1, szFilePathW, 1024);
// 			return SaveSurfaceToFileW(szFilePathW, D3DImageFormat);
// 		}
// 		else
// 			return SaveSurfaceToFileW(nullptr, D3DImageFormat);
// 		
// 	}
	// ����ץͼ����Surface�е�ͼ�����ݱ��浽�ļ��У��˽�ͼ�õ���ͼ����ԭʼ��ͼ��
	virtual bool SaveSurfaceToFileW(WCHAR *szFilePath, D3DXIMAGE_FILEFORMAT D3DImageFormat = D3DXIFF_JPG)
	{
		if (!m_pDirect3DDeviceEx || 
			!m_pDirect3DSurfaceRender) 
			return false;
		if (!szFilePath || wcslen(szFilePath) <= 0)
			return false;
		Autolock(&m_csSnapShot);
		m_D3DXIFF = D3DImageFormat;
		HRESULT hr = S_OK;
		if (!m_pSnapshotSurface)
		{
			HRESULT hr = m_pDirect3DDeviceEx->CreateOffscreenPlainSurface(m_nVideoWidth,
				m_nVideoHeight,
				D3DFMT_A8R8G8B8,
				D3DPOOL_SYSTEMMEM,
				&m_pSnapshotSurface,
				NULL);
			if (FAILED(hr))
			{
				DxTraceMsg("%s IDirect3DSurface9::GetDesc failed,hr = %08X.\n", __FUNCTION__, hr);
				return false;
			}
		}
		wcscpy(m_szSnapShotPath, szFilePath);
		bool bSnapReady = false;
		// ��ͼ������δ����,�����Ž�ͼ�¼�
		if (WaitForSingleObject(m_hEventCopySurface, 0) == WAIT_TIMEOUT)
		{
			SetEvent(m_hEventSnapShot);
			// ���´���ͼ����
			if (WaitForSingleObject(m_hEventCopySurface, 1000) == WAIT_OBJECT_0)
				bSnapReady = true;
			else
				return false;
		}
		else // ��ͼ�����Ѿ���
			bSnapReady = true;
		if (!bSnapReady)
			return false;

		hr = D3DXSaveSurfaceToFileW(szFilePath, m_D3DXIFF, m_pSnapshotSurface, NULL, NULL);
		if (FAILED(hr))
		{
			DxTraceMsg("%s D3DXSaveSurfaceToFile Failed,hr = %08X.\n", __FUNCTION__, hr);
			return false;
		}
		return true;
	}
	
	// ��Ļץͼ������ʾ����Ļ�ϵ�ͼ�󣬱��浽�ļ���,�˽�ͼ�õ����п��ܲ���ԭʼ��ͼ�񣬿����Ǳ�����������ͼ�� 
	virtual void CaptureScreen(TCHAR *szFilePath, D3DXIMAGE_FILEFORMAT D3DImageFormat = D3DXIFF_JPG)
	{
		if (!m_pDirect3DDevice)
			return;

		D3DDISPLAYMODE mode;
		HRESULT hr = m_pDirect3DDevice->GetDisplayMode(0, &mode);
		if (FAILED(hr))
		{
			DxTraceMsg("%s IDirect3DSurface9::GetDesc GetDisplayMode,hr = %08X.\n", __FUNCTION__, hr);
			return;
		}

		IDirect3DSurface9 *pSnapshotSurface = NULL;
		hr = m_pDirect3DDevice->CreateOffscreenPlainSurface(mode.Width,
			mode.Height,
			D3DFMT_A8R8G8B8,
			D3DPOOL_SYSTEMMEM,
			&pSnapshotSurface,
			NULL);
		if (FAILED(hr))
		{
			DxTraceMsg("%s IDirect3DSurface9::GetDesc failed,hr = %08X.\n", __FUNCTION__, hr);
			return;
		}

		hr = m_pDirect3DDevice->GetFrontBufferData(0, pSnapshotSurface);
		if (FAILED(hr))
		{
			DxTraceMsg("%s IDirect3DDevice9::GetFrontBufferData failed,hr = %08X.\n", __FUNCTION__, hr);
			SafeRelease(pSnapshotSurface);
			return;
		}

		RECT *rtSaved = NULL;
		WINDOWINFO windowInfo;
		if (m_d3dpp.Windowed)
		{
			windowInfo.cbSize = sizeof(WINDOWINFO);
			GetWindowInfo(m_d3dpp.hDeviceWindow, &windowInfo);
			rtSaved = &windowInfo.rcWindow;
		}
		//_tcscpy_s(m_szSnapShotPath,MAX_PATH,szPath);
		// m_D3DXIFF = D3DXIFF_JPG;
		hr = D3DXSaveSurfaceToFile(szFilePath, D3DImageFormat, pSnapshotSurface, NULL, rtSaved);
		if (FAILED(hr))
			DxTraceMsg("%s D3DXSaveSurfaceToFile Failed,hr = %08X.\n", __FUNCTION__, hr);
		SafeRelease(pSnapshotSurface);
	}
#define RenderTimeout	100 //ms
	bool Render(AVFrame *pAvFrame,HWND hWnd = NULL,RECT *pRenderRt = NULL)
	{
		if (!pAvFrame)
			return false;
		DeclareRunTime();
		HWND hRenderWnd = m_d3dpp.hDeviceWindow;
		if (hWnd)
			hRenderWnd = hWnd;
		//IsNeedRender(hRenderWnd);
		if (!IsNeedRender(hRenderWnd))
			return true;
		SaveRunTime();
		HRESULT hr = -1;		
		D3DLOCKED_RECT SrcRect;
		DWORD dwTNow = timeGetTime();
		if (!HandelDevLost())
		{
			return false;
			SaveRunTime();
		}
		// HandelDevLost���޷�ʹ��m_pDirect3DDevice����ֱ�ӷ���false
		if (!m_pDirect3DDeviceEx)
			return false;
		
		switch(pAvFrame->format)
		{
		case  AV_PIX_FMT_DXVA2_VLD:
			{// Ӳ����֡������ֱ����ʾ
				IDirect3DSurface9* pRenderSurface = m_pDirect3DSurfaceRender;	
				IDirect3DSurface9* pSurface = (IDirect3DSurface9 *)pAvFrame->data[3];
				if (m_bD3DShared)
				{	
					pRenderSurface = pSurface;
					if (!pSurface)
						return false;
				}
				else
				{
					D3DSURFACE_DESC SrcSurfaceDesc, DstSurfaceDesc;
					pSurface->GetDesc(&SrcSurfaceDesc);
					m_pDirect3DSurfaceRender->GetDesc(&DstSurfaceDesc);
					hr = pSurface->LockRect(&SrcRect, nullptr, D3DLOCK_READONLY);
					D3DLOCKED_RECT DstRect;
					hr |= m_pDirect3DSurfaceRender->LockRect(&DstRect, NULL, D3DLOCK_DONOTWAIT);
					if (FAILED(hr))
					{
						DxTraceMsg("%s line(%d) IDirect3DSurface9::LockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
						return false;
					}
					gpu_memcpy(DstRect.pBits, SrcRect.pBits, SrcRect.Pitch*SrcSurfaceDesc.Height);
					/*
					// Y����ͼ��
					uint8_t *pY = (uint8_t*)DstRect.pBits;
					// UV����ͼ��
					uint8_t *pUV = (uint8_t*)DstRect.pBits + DstRect.Pitch * DstSurfaceDesc.Height;
					*/
	
					hr |= m_pDirect3DSurfaceRender->UnlockRect();
					hr |= pSurface->UnlockRect();
					//DxTraceMsg("hr = %08X.\n", hr);
					if (FAILED(hr))
					{
						DxTraceMsg("%s line(%d) IDirect3DSurface9::UnlockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
						return false;
					}
				}

				// �����ͼ����
				TransferSnapShotSurface(pAvFrame);

				// �����ⲿ�ֻ��ƽӿ�
				ExternDrawCall(hWnd,pRenderRt);

				IDirect3DSurface9 * pBackSurface = NULL;
				m_pDirect3DDeviceEx->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
				m_pDirect3DDeviceEx->BeginScene();
				hr = m_pDirect3DDeviceEx->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurface);
				D3DSURFACE_DESC desc;

				if (FAILED(hr))
				{
					m_pDirect3DDeviceEx->EndScene();
					DxTraceMsg("%s line(%d) IDirect3DDevice9Ex::GetBackBuffer failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return true;
				}
				pBackSurface->GetDesc(&desc);
				RECT dstrt = { 0, 0, desc.Width, desc.Height };
				RECT srcrt = { 0, 0, pAvFrame->width, pAvFrame->height };	
				hr = m_pDirect3DDeviceEx->StretchRect(pRenderSurface, &srcrt, pBackSurface, &dstrt, D3DTEXF_LINEAR);

				pBackSurface->Release();
				m_pDirect3DDeviceEx->EndScene();
				break;
			}
		default:		
			{// �����֡��ֻ֧��YUV420P��ʽ
				SaveRunTime();
				TransferSnapShotSurface(pAvFrame);
				D3DLOCKED_RECT d3d_rect;
				D3DSURFACE_DESC Desc;
				SaveRunTime();
				hr = m_pDirect3DSurfaceRender->GetDesc(&Desc);
				hr |= m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
				if (FAILED(hr))
				{
					//DxTraceMsg("%s line(%d) IDirect3DSurface9::LockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return false;
				}
				assert(Desc.Width == pAvFrame->width);
				assert(Desc.Height == pAvFrame->height);
				
				SaveRunTime();
				if ((pAvFrame->format == AV_PIX_FMT_YUV420P ||
					pAvFrame->format == AV_PIX_FMT_YUVJ420P) &&
					Desc.Format == (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'))
					CopyFrameYUV420P((byte *)d3d_rect.pBits,d3d_rect.Pitch,pAvFrame);
				else
				{
					if (!m_pPixelConvert)
#if _MSC_VER >= 1600
						m_pPixelConvert = make_shared<PixelConvert>(pAvFrame,Desc.Format);
#else
						m_pPixelConvert = shared_ptr<PixelConvert>(new PixelConvert(pAvFrame,Desc.Format));
#endif 
					m_pPixelConvert->ConvertPixel(pAvFrame);
					if (m_pPixelConvert->GetDestPixelFormat() == AV_PIX_FMT_YUV420P)
						CopyFrameYUV420P((byte *)d3d_rect.pBits,d3d_rect.Pitch,m_pPixelConvert->pFrameNew);
					else					
						memcpy((byte *)d3d_rect.pBits,m_pPixelConvert->pImage,m_pPixelConvert->nImageSize);
				}
				SaveRunTime();
				hr = m_pDirect3DSurfaceRender->UnlockRect();
				if (FAILED(hr))
				{
					//DxTraceMsg("%s line(%d) IDirect3DSurface9::UnlockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return false;
				}
				SaveRunTime();
				// �����ⲿ�ֻ��ƽӿ�
				ExternDrawCall(hWnd,pRenderRt);
				SaveRunTime();
				IDirect3DSurface9 * pBackSurface = NULL;	
				m_pDirect3DDeviceEx->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
				m_pDirect3DDeviceEx->BeginScene();
				hr = m_pDirect3DDeviceEx->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurface);
				SaveRunTime();
				if (FAILED(hr))
				{
					m_pDirect3DDeviceEx->EndScene();
					//DxTraceMsg("%s line(%d) IDirect3DDevice9Ex::GetBackBuffer failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return true;
				}
				pBackSurface->GetDesc(&Desc);
				RECT dstrt = { 0, 0, Desc.Width, Desc.Height };
				RECT srcrt = { 0, 0, pAvFrame->width, pAvFrame->height };	
				hr = m_pDirect3DDeviceEx->StretchRect(m_pDirect3DSurfaceRender, &srcrt, pBackSurface, &dstrt, D3DTEXF_LINEAR);
				SaveRunTime();
				SafeRelease(pBackSurface);
				m_pDirect3DDeviceEx->EndScene();
				SaveRunTime();
			}
			break;
		case AV_PIX_FMT_NONE:
			{
				DxTraceMsg("*************************************.\n");
				DxTraceMsg("*	Get a None picture Frame error	*.\n");
				DxTraceMsg("*************************************.\n");
				return true;
			}
			break;
		}

		// Present(RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
		if (hWnd)
			hr |= m_pDirect3DDeviceEx->PresentEx(NULL, pRenderRt, hWnd, NULL,0);
		else
			// (PresentEx)(RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags)
			hr |= m_pDirect3DDeviceEx->PresentEx(NULL, NULL,m_d3dpp.hDeviceWindow, NULL,0);
		SaveRunTime();
		if (SUCCEEDED(hr))
			return true;
		else
		{
			SaveRunTime();
			bool bRet =  HandelDevLost();
			SaveRunTime();
			return bRet;
		}
	}
};