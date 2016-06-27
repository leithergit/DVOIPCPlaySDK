#pragma once
#include <d3d9.h>
//#include <d3dx9tex.h>
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
#include "../Utility.h"
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

using namespace  std;
//#include <boost/smart_ptr.hpp>
//using namespace boost;
using namespace  std::tr1;
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


#define WM_RENDERFRAME		WM_USER + 1024		// 帧渲染消息	WPARAM为CDxSurface指针,LPARAM为一个指向DxSurfaceRenderInfo结构的指针,
#define	WM_INITDXSURFACE	WM_USER + 1025		// DXSurface初始化消息	WPARAM为CDxSurface指针，LPARAM为DxSurfaceInitInfo结构的指针
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
	GQ_SINC = -3,		//30		相对于上一算法，细节要清晰一些。
	GQ_SPLINE = -2,		//47		和上一个算法，我看不出区别。
	GQ_LANCZOS = -1,	//70		相对于上一算法，要平滑(也可以说是模糊)一点点，几乎无区别。
	GQ_BICUBIC = 0,		//80		感觉差不多，比上上算法边缘要平滑，比上一算法要锐利。	
	GQ_GAUSS,			//80		相对于上一算法，要平滑(也可以说是模糊)一些。
	GQ_BICUBLIN,		//87		同上。
	GQ_X,				//91		与上一图像，我看不出区别。
	GQ_BILINEAR,		//95		感觉也很不错，比上一个算法边缘平滑一些。
	GQ_AREA,			//116		与上上算法，我看不出区别。
	GQ_FAST_BILINEAR,	//228		图像无明显失真，感觉效果很不错
	GQ_POINT			//427		细节比较锐利，图像效果比上图略差一点点。
};

#define _TraceMemory

#if defined(_DEBUG) && defined(_TraceMemory)
#define TraceTimeout		150
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
/// @brief 跟踪函数执行过程的一些信息的类,主要用于调试
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
			if (TimeSpanEx(m_dfTimeIn) > TraceTimeout/1000.0f)
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
#define SaveRunTime()		LineSave.SaveLineTime(__FILE__,__LINE__);
#define DeclareRunTime(nTimeout)	CLineRunTime LineSave(nTimeout);
#else
#define SaveRunTime()
#define DeclareRunTime(nTimeout)			
#endif

#include <vector>
using namespace  std;
/// @brief 追踪每一行代码运行的时间，用于追查代码中哪一段代码执行时间超出预期
class CLineRunTime
{
	DWORD m_nTimeout;
public:
	CLineRunTime(int nTimeout,CRunlogA *plog = nullptr)
	{
		m_nTimeout = nTimeout;
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
		if (dwTotalSpan >= m_nTimeout)
		{
			OutputMsg("%s @File:%s line %d Total Runtime span = %d.\n", __FUNCTION__, pTimeArray[0]->szFile, pTimeArray[0]->nLine, dwTotalSpan);
		}
		for (int i = 1; i < nSize;i ++)
		{
			DWORD dwSpan = pTimeArray[i]->nTime - pTimeArray[i - 1]->nTime;
			if (dwSpan >= m_nTimeout)
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

/// @biref 把FFMPEG像素转换为D3DFomrat像素
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
			nDstAvFormat = AV_PIX_FMT_RGB565;	// 亦有可能是、AV_PIX_FMT_BGR565LE、AV_PIX_FMT_BGR565BE
			break;
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
			nDstAvFormat = AV_PIX_FMT_RGB555;	// 亦有可能是AV_PIX_FMT_BGR555BE
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
		nImageSize	 = av_image_get_buffer_size(nDstAvFormat, pSrcFrame->width,pSrcFrame->height,16); 
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
		// 把显示图像与YUV帧关联
		av_image_fill_arrays(pFrameNew->data,pFrameNew->linesize, pImage, nDstAvFormat, pSrcFrame->width,pSrcFrame->height,16);
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
		sws_scale转换函数各种算法转换(缩小)效率和画质对比情况(详情请参考:http://blog.csdn.net/leixiaohua1020/article/details/12029505)
		算法				帧率	图像主观感受
		SWS_FAST_BILINEAR	228		图像无明显失真，感觉效果很不错。
		SWS_BILINEAR		95		感觉也很不错，比上一个算法边缘平滑一些。
		SWS_BICUBIC			80		感觉差不多，比上上算法边缘要平滑，比上一算法要锐利。
		SWS_X				91		与上一图像，我看不出区别。
		SWS_POINT			427		细节比较锐利，图像效果比上图略差一点点。
		SWS_AREA			116		与上上算法，我看不出区别。
		SWS_BICUBLIN		87		同上。
		SWS_GAUSS			80		相对于上一算法，要平滑(也可以说是模糊)一些。
		SWS_SINC			30		相对于上一算法，细节要清晰一些。
		SWS_LANCZOS			70		相对于上一算法，要平滑(也可以说是模糊)一点点，几乎无区别。
		SWS_SPLINE			47		和上一个算法，我看不出区别。

		sws_scale转换函数各种算法转换(放大)效率和画质对比情况
		算法				帧率	图像主观感受
		SWS_FAST_BILINEAR	103		图像无明显失真，感觉效果很不错。
		SWS_BILINEAR		100		和上图看不出区别。
		SWS_BICUBIC			78		相对上图，感觉细节清晰一点点。
		SWS_X				106		与上上图无区别。
		SWS_POINT			112		边缘有明显锯齿。
		SWS_AREA			114		边缘有不明显锯齿。
		SWS_BICUBLIN		95		与上上上图几乎无区别。
		SWS_GAUSS			86		比上图边缘略微清楚一点。
		SWS_SINC			20		与上上图无区别。
		SWS_LANCZOS			64		与上图无区别。
		SWS_SPLINE			40		与上图无区别。
		结论和建议:
			如果对图像的缩放，要追求高效，比如说是视频图像的处理，在不明确是放大还是缩小时，
		直接使用SWS_FAST_BILINEAR算法即可。如果明确是要缩小并显示，建议使用Point算法，如果
		是明确要放大并显示，其实使用CImage的Strech更高效。
			当然，如果不计速度追求画面质量。在上面的算法中，选择帧率最低的那个即可，画面效
			果一般是最好的。
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
	// 进行像素转换
	int inline ConvertPixel(AVFrame *pSrcFrame,GraphicQulityParameter nGQ = GQ_BICUBIC)
	{
		if (!pImage)
			return -1;
		//DxTrace("@File:%s %s pSrcFrame(%d,%d)->LineSize(%d,%d,%d).\n", __FILE__, __FUNCTION__, pSrcFrame->width, pSrcFrame->height, pSrcFrame->linesize[0], pSrcFrame->linesize[1], pSrcFrame->linesize[2]);
		if (nGQP != nGQ)
		{
			// 转换算法调整
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
										nDstAvFormat,						// dst format
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
// 注意:
/// @brief IDirect3DSurface9对象封类，用于创建和管理IDirect3DSurface9对象，可以显示多种象素格式的图像
/// @remark 使用CDxSurface对象显示图象时，必须在创建线程内显示图，否则当发生DirectX设备丢失时，无法重置DirectX的资源
class CDxSurface
{
protected:	
	long					m_nVtableAddr;		// 虚函数表地址，该变量地址位置虚函数表之后，仅用于类初始化，请匆移动该变量的位置
	
	D3DPRESENT_PARAMETERS	m_d3dpp;
	CRITICAL_SECTION		m_csRender;			// 渲染临界区
	CRITICAL_SECTION		m_csSnapShot;		// 截图临界区
	bool					m_bD3DShared;		// IDirect3D9接口是否为共享 	
	/*HWND					m_hWnd;*/
	DWORD					m_dwExStyle;
	DWORD					m_dwStyle;
	bool					m_bFullScreen;		// 是否全屏,为true时，则进行全屏/窗口的切换,切换完成后,m_bSwitchScreen将被置为false
	WINDOWPLACEMENT			m_WndPlace;
	HMENU					m_hMenu;
	IDirect3DSurface9		*m_pDirect3DSurfaceRender/* = NULL*/;
	IDirect3DSurface9		*m_pSnapshotSurface;	/* = NULL*/;	//截图专用表面
	D3DFORMAT				m_nD3DFormat;
	UINT					m_nVideoWidth;
	UINT					m_nVideoHeight;
	bool					m_bInitialized;	
	HMODULE					m_hD3D9;
	shared_ptr<PixelConvert>m_pPixelConvert;
	IDirect3D9				*m_pDirect3D9		/* = NULL*/;
	IDirect3DDevice9		*m_pDirect3DDevice	/*= NULL*/;	
	HANDLE					m_hEventSnapShot;	// 截图请求事件
	bool					m_bSnapFlag;		// 截图事件标志,若该标志为TRUE，即使窗口被隐藏也会进行画面渲染
	HANDLE					m_hEventCopySurface;// 解码数据复制事件
	// 外部绘制接口，提供外部接口，允许调用方自行绘制图像
	ExternDrawProc			m_pExternDraw;
	void*					m_pUserPtr;			// 外部调用者自定义指针
	
	bool					m_bFullWnd;			// 是否填满窗口,为True时，则填充整个窗口客户区，否则只按视频比例显示	
	HWND					m_hFullScreenWindow;// 伪全屏窗口
	// 截图相关变量

	//D3DXIMAGE_FILEFORMAT	m_D3DXIFF;			// 截图类型,默认为bmp格式
 	WCHAR					m_szSnapShotPath[MAX_PATH];
	bool					m_bEnableVsync;		// 启用垂直同步,默认为true,在需要显示的帧率大于显示器实际帧率时,需要禁用垂直同步

	bool					m_bVideoScaleFixed;	// 当m_bVideoScaleFixed为true,并且m_fWHScale = 0时,则使用图像原始比例,比例值为Width/Height
												// 当m_bVideoScaleFixed为true,并且m_fWHScale 大于0时,则使用dfWHScale提供的比例显示，图像可能会被拉伸变形
												// 当m_bVideoScaleFixed为false,m_fWHScale参数将被忽略,此时像将填满窗口客户区
	float					m_fWHScale;	
	WNDPROC					m_pOldWndProc;
	static WndSurfaceMap	m_WndSurfaceMap;
	static CriticalSectionPtr m_WndSurfaceMapcs;
	static	int				m_nObjectCount;
	static CriticalSectionPtr m_csObjectCount;
	bool					m_bWndSubclass;		// 是否子类化显示窗口,为ture时，则将显示窗口子类化,此时窗口消息会先被CDxSurface::WndProc优先处理,再由窗口函数处理
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
		// 借助于m_nVtableAddr变量，避开对虚函数表的初始化
		// 仅适用于微软的Visual C++编译器
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

	// 禁用垂直同步,只有在初始化之前调用,才会有效
	// 默认情况下，垂直同步被启用, 在需要显示的帧率大于显示器实际帧率时,需要禁用垂直同步
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
	/// @设置外部DC回调函数
	/// @remark IDirect3DSurface9可以产生一个DC指针，通过回调函数把此DC指针传给用户，用户通过此DC可以在IDirect3DSurface9上作图
	/// 但这种方式效率极低，会明显拖慢视频显示的速度
	void SetExternDraw(void *pExternDrawProc,void *pUserPtr)
	{
		m_pExternDraw = (ExternDrawProc)pExternDrawProc;
		m_pUserPtr = pUserPtr;
	}

	// 调用外部绘制函数
	virtual void ExternDrawCall(HWND hWnd, RECT *pRect)
	{
		if (!m_pDirect3DSurfaceRender)
			return;

		if (m_pExternDraw)
		{
			D3DSURFACE_DESC Desc;
			if (FAILED(m_pDirect3DSurfaceRender->GetDesc(&Desc)))
				return;
			switch (Desc.Format)
			{//Direct3DSurface::GetDC() only supports D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5, D3DFMT_R8G8B8, D3DFMT_X8R8G8B8, and D3DFMT_A8R8G8B8.
				// 外部图你绘制仅支持以下像素格式
			case D3DFMT_R5G6B5:
			case D3DFMT_X1R5G5B5:
			case D3DFMT_A1R5G5B5:
			case D3DFMT_R8G8B8:
			case D3DFMT_X8R8G8B8:
			case D3DFMT_A8R8G8B8:
				break;
			default:
			{
				DxTraceMsg("%s Unsupported Pixel format.\r\n", __FUNCTION__);
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
					memcpy(&MotionRect, pRect, sizeof(RECT));
				else
				{
					MotionRect.left = 0;
					MotionRect.right = Desc.Width;
					MotionRect.top = 0;
					MotionRect.bottom = Desc.Height;
				}
				m_pExternDraw(hMotionWnd, hDc, MotionRect, m_pUserPtr);
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
	/// @brief 取图像尺寸和象素格式 低32位的低16 bit为宽度，高16bit为高度，高32位为象素格式
	UINT64 GetVideoSizeAndFormat()
	{
		return MAKEUINT64(MAKELONG(m_nVideoWidth, m_nVideoHeight), m_nD3DFormat);
	}
	// @brief D3dDirect9Ex下，该成员不再有效
	virtual bool RestoreDevice()
	{// 恢复设备，即用原始参数重建资源
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
		SafeRelease(m_pSnapshotSurface);
		SafeRelease(m_pDirect3DDevice);
	}
// 以下代码失效，因为不再使用DirectX进行截图操作	
	// 发送截图请求，即置信截图事件
// 	void RequireSnapshot()
// 	{
// 		SetEvent(m_hEventSnapShot);
// 		m_bSnapFlag = true;
// 	}
// 
// 	// 把解码帧pAvFrame中的图像传送到截图表面
// 	void TransferSnapShotSurface(AVFrame *pAvFrame)
// 	{
// 		if (m_pSnapshotSurface && WaitForSingleObject(m_hEventSnapShot, 0) == WAIT_OBJECT_0)						// 收到截图请求
// 		{
// 			//ResetEvent(m_hEventCreateSurface);
// 			// 不能使用StretchRect把显存表面复制到系统内存表面
// 			// hr = m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, &srcrt, m_pSnapshotSurface, &srcrt, D3DTEXF_LINEAR);
// 			D3DLOCKED_RECT D3dRect;
// 			D3DSURFACE_DESC desc;
// 			m_pSnapshotSurface->GetDesc(&desc);
// 			m_pSnapshotSurface->LockRect(&D3dRect, NULL, D3DLOCK_DONOTWAIT);
// 			// FFMPEG像素编码字节序与DirectX像素的字节序相反,因此
// 			// D3DFMT_A8R8G8B8(A8:R8:G8:B8)==>AV_PIX_FMT_BGRA(B8:G8:R8:A8)
// #if _MSC_VER >= 1600
// 			shared_ptr<PixelConvert> pVideoScale = make_shared<PixelConvert>(pAvFrame,D3DFMT_A8R8G8B8,GQ_BICUBIC);
// #else
// 			shared_ptr<PixelConvert> pVideoScale (new PixelConvert(pAvFrame,D3DFMT_A8R8G8B8,GQ_BICUBIC));
// #endif
// 			pVideoScale->ConvertPixel(pAvFrame);
// 			memcpy_s(D3dRect.pBits, D3dRect.Pitch*desc.Height, pVideoScale->pImage, pVideoScale->nImageSize);
// 			m_pSnapshotSurface->UnlockRect();
// 			SetEvent(m_hEventCopySurface);
// 			DxTraceMsg("%s Surface is Transfered.\n", __FUNCTION__);
// 		}
// 	}
// 	
// 	// 解码抓图，把Surface中的图像数据保存到文件中，此截图得到的图像是原始的图像
// 	virtual bool SaveSurfaceToFileA(CHAR *szFilePath,D3DXIMAGE_FILEFORMAT D3DImageFormat = D3DXIFF_JPG)
// 	{
// 		if (!szFilePath || strlen(szFilePath) <= 0)
// 			return false;
// 		
// 		WCHAR szFilePathW[1024] = { 0 };
// 		MultiByteToWideChar(CP_ACP, 0, szFilePath, -1, szFilePathW, 1024);
// 		return SaveSurfaceToFileW(szFilePathW, D3DImageFormat);
// 	}
// 	
// 	// 解码抓图，把Surface中的图像数据保存到文件中，此截图得到的图像是原始的图像
// 	virtual bool SaveSurfaceToFileW(WCHAR *szFilePath, D3DXIMAGE_FILEFORMAT D3DImageFormat = D3DXIFF_JPG)
// 	{
// 		if (!m_pDirect3DDevice ||
// 			!m_pDirect3DSurfaceRender)
// 			return false;
// 		if (!szFilePath || wcslen(szFilePath) <= 0)
// 			return false;
// 		CAutoLock lock(&m_csSnapShot);
// 		
// 		m_D3DXIFF = D3DImageFormat;
// 		HRESULT hr = S_OK;
// 		if (!m_pSnapshotSurface)
// 		{
// 			HRESULT hr = m_pDirect3DDevice->CreateOffscreenPlainSurface(m_nVideoWidth,
// 				m_nVideoHeight,
// 				D3DFMT_A8R8G8B8,
// 				D3DPOOL_SYSTEMMEM,
// 				&m_pSnapshotSurface,
// 				NULL);
// 			if (FAILED(hr))
// 			{
// 				DxTraceMsg("%s IDirect3DSurface9::GetDesc failed,hr = %08X.\n", __FUNCTION__, hr);
// 				return false;
// 			}
// 		}
// 
// 		wcscpy(m_szSnapShotPath, szFilePath);
// 		bool bSnapReady = false;
// 		// 截图数据尚未就绪,则置信截图事件
// 		if (WaitForSingleObject(m_hEventCopySurface, 0) == WAIT_TIMEOUT)
// 		{
// 			SetEvent(m_hEventSnapShot);
// 			// 重新待截图数据
// 			if (WaitForSingleObject(m_hEventCopySurface, 1000) == WAIT_OBJECT_0)
// 				bSnapReady = true;
// 			else
// 				return false;
// 		}
// 		else // 截图数据已就绪
// 			bSnapReady = true;
// 		if (!bSnapReady)
// 			return false;
// 		m_bSnapFlag = false;
// 		hr = D3DXSaveSurfaceToFileW(szFilePath, m_D3DXIFF, m_pSnapshotSurface, NULL, NULL);
// 		if (FAILED(hr))
// 		{
// 			DxTraceMsg("%s D3DXSaveSurfaceToFile Failed,hr = %08X.\n", __FUNCTION__, hr);
// 			return false;
// 		}
// 		return true;
// 	}
// 	
// 	// 屏幕抓图，把显示到屏幕上的图象，保存到文件中,此截图得到的有可能不是原始的图像，可能是被拉伸或处理过的图像 
// 	virtual void CaptureScreen(TCHAR *szFilePath,D3DXIMAGE_FILEFORMAT D3DImageFormat = D3DXIFF_JPG)		
// 	{
// 		if (!m_pDirect3DDevice)
// 			return ;
// 
// 		D3DDISPLAYMODE mode;
// 		HRESULT hr = m_pDirect3DDevice->GetDisplayMode(0,&mode);
// 		if (FAILED(hr))
// 		{
// 			DxTraceMsg("%s IDirect3DSurface9::GetDesc GetDisplayMode,hr = %08X.\n",__FUNCTION__,hr);
// 			return ;
// 		}
// 
// 		IDirect3DSurface9 *pSnapshotSurface = NULL;	
// 		hr = m_pDirect3DDevice->CreateOffscreenPlainSurface(mode.Width,
// 															mode.Height, 
// 															D3DFMT_A8R8G8B8, 
// 															D3DPOOL_SYSTEMMEM, 
// 															&pSnapshotSurface, 
// 															NULL);
// 		if (FAILED(hr))
// 		{
// 			DxTraceMsg("%s IDirect3DSurface9::GetDesc failed,hr = %08X.\n",__FUNCTION__,hr);
// 			return ;
// 		}
// 		
// 		hr = m_pDirect3DDevice->GetFrontBufferData(0,pSnapshotSurface);
// 		if (FAILED(hr))
// 		{
// 			DxTraceMsg("%s IDirect3DDevice9::GetFrontBufferData failed,hr = %08X.\n",__FUNCTION__,hr);
// 			SafeRelease(pSnapshotSurface);
// 			return ;
// 		}
// 
// 		RECT *rtSaved = NULL;
// 		WINDOWINFO windowInfo ;
// 		if (m_d3dpp.Windowed)
// 		{
// 			windowInfo.cbSize = sizeof(WINDOWINFO);
// 			GetWindowInfo(m_d3dpp.hDeviceWindow, &windowInfo);
// 			rtSaved = &windowInfo.rcWindow;
// 		}
// 		//_tcscpy_s(m_szSnapShotPath,MAX_PATH,szPath);
// 		// m_D3DXIFF = D3DXIFF_JPG;
// 		hr = D3DXSaveSurfaceToFile(szFilePath,D3DImageFormat,pSnapshotSurface,NULL,rtSaved);
// 		if (FAILED(hr))		
// 			DxTraceMsg("%s D3DXSaveSurfaceToFile Failed,hr = %08X.\n",__FUNCTION__,hr);
// 		SafeRelease(pSnapshotSurface);
// 	}
// 	/*
// 	D3DXIFF_BMP          = 0,
// 	D3DXIFF_JPG          = 1,
// 	D3DXIFF_TGA          = 2,
// 	D3DXIFF_PNG          = 3,
// 	D3DXIFF_DDS          = 4,
// 	D3DXIFF_PPM          = 5,
// 	D3DXIFF_DIB          = 6,
// 	D3DXIFF_HDR          = 7,
// 	D3DXIFF_PFM          = 8,
// 	*/
// 	void SavetoFile(TCHAR *szPath,D3DXIMAGE_FILEFORMAT nD3DXIFF)
// 	{
// 		//_tcscpy_s(m_szSnapShotPath,MAX_PATH,szPath);
// #ifdef _UNICODE
// 		SaveSurfaceToFileW(szPath,nD3DXIFF);
// #else
// 		SaveSurfaceToFileA(szPath, nD3DXIFF);
// #endif
// 	}
// 
// 	void SavetoBmp(TCHAR *szPath)
// 	{
// 		//_tcscpy_s(m_szSnapShotPath,MAX_PATH,szPath);
// #ifdef _UNICODE
// 		SaveSurfaceToFileW(szPath,D3DXIFF_BMP);
// #else
// 		SaveSurfaceToFileA(szPath, D3DXIFF_BMP);
// #endif
// 	}
// 	
	// 调用InitD3D之前必须先调用AttachWnd函数关联视频显示窗口
	// nD3DFormat 必须为以下格式之一
	// MAKEFOURCC('Y', 'V', '1', '2')	默认格式,可以很方便地由YUV420P转换得到,而YUV420P是FFMPEG解码后得到的默认像素格式
	// MAKEFOURCC('N', 'V', '1', '2')	仅DXVA硬解码使用该格式
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
		m_d3dpp.MultiSampleType			= D3DMULTISAMPLE_NONE;					// 显示视频时，不宜使用多重采样，否则将导致画面错乱
		m_d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;				// 指定系统如何将后台缓冲区的内容复制到前台缓冲区 D3DSWAPEFFECT_DISCARD:清除后台缓存的内容
#pragma warning(push)
#pragma warning(disable:4800)
		m_bFullScreen					= (bool)bIsWindowed;
#pragma warning(pop)		

		if (bIsWindowed)//窗口模式
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
			m_d3dpp.FullScreen_RefreshRateInHz	= 0;							// 显示器刷新率，窗口模式该值必须为0
			
			m_d3dpp.BackBufferHeight			= nVideoHeight;
			m_d3dpp.BackBufferWidth				= nVideoWidth;
			m_d3dpp.EnableAutoDepthStencil		= FALSE;							// 关闭自动深度缓存
		}
		else
		{
			//全屏模式
			m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
			m_d3dpp.FullScreen_RefreshRateInHz	= d3ddm.RefreshRate;	
			
			m_d3dpp.EnableAutoDepthStencil		= FALSE;			
			m_d3dpp.BackBufferWidth				= GetSystemMetrics(SM_CXSCREEN);		// 获得屏幕宽
			m_d3dpp.BackBufferHeight			= GetSystemMetrics(SM_CYSCREEN);		// 获得屏幕高

			GetWindowPlacement(m_d3dpp.hDeviceWindow, &m_WndPlace ) ;
			m_dwExStyle	 = GetWindowLong( m_d3dpp.hDeviceWindow, GWL_EXSTYLE ) ;
			m_dwStyle	 = GetWindowLong( m_d3dpp.hDeviceWindow, GWL_STYLE ) ;
			m_dwStyle	 &= ~WS_MAXIMIZE & ~WS_MINIMIZE; // remove minimize/maximize style
			m_hMenu		 = GetMenu( m_d3dpp.hDeviceWindow ) ;
		}
		/*
		CreateDevice的BehaviorFlags参数选项：
		D3DCREATE_ADAPTERGROUP_DEVICE只对主显卡有效，让设备驱动输出给它所拥有的所有显示输出
		D3DCREATE_DISABLE_DRIVER_MANAGEMENT代替设备驱动来管理资源，这样在发生资源不足时D3D调用不会失败
		D3DCREATE_DISABLE_PRINTSCREEN:不注册截屏快捷键，只对Direct3D 9Ex
		D3DCREATE_DISABLE_PSGP_THREADING：强制计算工作必须在主线程上，vista以上有效
		D3DCREATE_ENABLE_PRESENTSTATS：允许GetPresentStatistics收集统计信息只对Direct3D 9Ex
		D3DCREATE_FPU_PRESERVE；强制D3D与线程使用相同的浮点精度，会降低性能
		D3DCREATE_HARDWARE_VERTEXPROCESSING：指定硬件进行顶点处理，必须跟随D3DCREATE_PUREDEVICE
		D3DCREATE_MIXED_VERTEXPROCESSING：指定混合顶点处理
		D3DCREATE_SOFTWARE_VERTEXPROCESSING：指定纯软的顶点处理
		D3DCREATE_MULTITHREADED：要求D3D是线程安全的，多线程时
		D3DCREATE_NOWINDOWCHANGES：拥有不改变窗口焦点
		D3DCREATE_PUREDEVICE：只试图使用纯硬件的渲染
		D3DCREATE_SCREENSAVER：允许被屏保打断只对Direct3D 9Ex
		D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DCREATE_MIXED_VERTEXPROCESSING, and D3DCREATE_SOFTWARE_VERTEXPROCESSING中至少有一个一定要设置
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
		// 保存参数
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
	// 判断是否需要在目标窗口上显示图像
	// 在被隐藏或最小化的窗口上显示图像速度非常慢,会严重影响整个渲染进程，因此
	// 当窗口或其根窗口处于隐藏或最小化状态时，则不应在该窗口上绘制图像
	bool IsNeedRender(HWND hRenderWnd)
	{
		if (IsWindow(hRenderWnd) && m_bSnapFlag)
			return true;
		// 若窗口被隐藏或最小化则不再显示图像
		if (IsIconic(hRenderWnd))		// 窗口最小化	
		{
			//DxTraceMsg("%s hRenderWnd is Iconic.\n", __FUNCTION__);
			return false;
		}
		if (!IsWindowVisible(hRenderWnd))// 窗口隐藏
		{
			//DxTraceMsg("%s hRenderWnd is Unvisible Window.\n", __FUNCTION__);
			return false;
		}
		// 若当前窗口的根窗口被隐藏或最小化亦不显示图像
		HWND hRoot = GetAncestor(hRenderWnd,GA_ROOT);
		if (hRoot)
		{
			if (IsIconic(hRoot))			// 窗口最小化
			{
				//DxTraceMsg("%s hRoot is Iconic.\n", __FUNCTION__);
				return false;
			}
			if (!IsWindowVisible(hRoot))	// 窗口隐藏
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
	// 暂时不要使用这个功能，因为目前尚未找到全屏换回窗口模式的方法
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
	
	// 处理设备丢失
	// m_bManualReset	默认为false,只处理被动的设备丢失,为true时，则是主动制造设备丢失,以便适应窗口尺寸变化，如全屏放大等
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
					DxTrace("%s ResetDevice Failed,Not Try to Reinit D3D.\n",__FUNCTION__);
					DxCleanup();
					return InitD3D(m_d3dpp.hDeviceWindow, m_nVideoWidth, m_nVideoHeight, m_bFullScreen);
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
	void CopyFrameYUV420P(D3DLOCKED_RECT *pD3DRect,int nDescHeight,AVFrame *pFrame420P)
	//void CopyFrameYUV420P(byte *pDest,int nDestSize,int nStride,AVFrame *pFrame420P)
	{
		byte *pDest = (byte *)pD3DRect->pBits;
		int nStride = pD3DRect->Pitch;
		int nSize = nDescHeight * nStride;
		int nHalfSize = (nSize) >> 1;	
		byte *pDestY = pDest;										// Y分量起始地址
		byte *pDestV = pDest + nSize;								// U分量起始地址
		int nSizeofV = nHalfSize>>1;
		byte *pDestU = pDestV + (size_t)(nHalfSize >> 1);			// V分量起始地址
		int nSizoefU = nHalfSize>>1;

		// YUV420P的U和V分量对调，便成为YV12格式
		// 复制Y分量
 		for (int i = 0; i < pFrame420P->height; i++)
 			memcpy_s(pDestY + i * nStride, nSize*3/2 - i*nStride, pFrame420P->data[0] + i * pFrame420P->linesize[0], pFrame420P->width);

		// 复制YUV420P的U分量到目村的YV12的U分量
 		for (int i = 0; i < pFrame420P->height / 2; i++)
 			memcpy_s(pDestU + i * nStride / 2,nSizoefU - i*nStride/2, pFrame420P->data[1] + i * pFrame420P->linesize[1], pFrame420P->width/2);

		// 复制YUV420P的V分量到目村的YV12的V分量
 		for (int i = 0; i < pFrame420P->height / 2; i++)
 			memcpy_s(pDestV + i * nStride / 2,nSizeofV - i*nStride/2, pFrame420P->data[2] + i * pFrame420P->linesize[2], pFrame420P->width/2);
	}

	void CopyFrameARGB(byte *pDest,int nDestSize,int nStride,AVFrame *pFrameARGB)
	{	
		for (int i = 0; i < pFrameARGB->height; i++)
			memcpy_s(pDest + i * nStride, nDestSize - i*nStride,pFrameARGB->data[0] + i * pFrameARGB->linesize[0], pFrameARGB->width);
	}
	
	virtual bool Render(AVFrame *pAvFrame, HWND hWnd = NULL, RECT *pClippedRT = nullptr, RECT *pRenderRt = nullptr)
	{
		//TraceMemory();
		DeclareRunTime(5);
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
		SaveRunTime();
		if (!HandelDevLost())
			return false;
		SaveRunTime();
		// HandelDevLost仍无法使用m_pDirect3DDevice，则直接返回false
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
			{// 硬解码帧，可以直接显示
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
					// 复制表面时不需针对YUV各分量分别复制，直接复制表面即可，前提是表面的参数必须完全一致
					gpu_memcpy(DstRect.pBits, SrcRect.pBits, SrcRect.Pitch*SrcSurfaceDesc.Height*3/2);
					/*
					// Y分量图像
					uint8_t *pY = (uint8_t*)DstRect.pBits;
					// UV分量图像
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
				// 处理截图请求
				//TransferSnapShotSurface(pAvFrame);
				// 处理外部分绘制接口
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
				if (pClippedRT)
				{
					CopyRect(&srcrt, pClippedRT);
				}
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
			{// 软解码帧，只支持YUV420P格式		
				SaveRunTime();
				//TransferSnapShotSurface(pAvFrame);
				D3DLOCKED_RECT d3d_rect;
				D3DSURFACE_DESC Desc;
				hr = m_pDirect3DSurfaceRender->GetDesc(&Desc);
				hr |= m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
				SaveRunTime();
				//DxTraceMsg("hr = %08X.\n",hr);
				if (FAILED(hr))
				{
					DxTraceMsg("%s line(%d) IDirect3DSurface9::LockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return false;
				}
 				if ((pAvFrame->format == AV_PIX_FMT_YUV420P ||
					pAvFrame->format == AV_PIX_FMT_YUVJ420P) &&
					Desc.Format == (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'))
					CopyFrameYUV420P(&d3d_rect,Desc.Height, pAvFrame);
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
						CopyFrameYUV420P(&d3d_rect,Desc.Height,m_pPixelConvert->pFrameNew);
					else					
						memcpy((byte *)d3d_rect.pBits,m_pPixelConvert->pImage,m_pPixelConvert->nImageSize);
				}
				hr = m_pDirect3DSurfaceRender->UnlockRect();
				SaveRunTime();
				if (FAILED(hr))
				{
					DxTraceMsg("%s line(%d) IDirect3DSurface9::UnlockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return false;
				}

				// 处理外部分绘制接口
				ExternDrawCall(hWnd,pRenderRt);

				IDirect3DSurface9 * pBackSurface = NULL;	
				m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
				SaveRunTime();
				m_pDirect3DDevice->BeginScene();
				hr = m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurface);
				SaveRunTime();
				if (FAILED(hr))
				{
					m_pDirect3DDevice->EndScene();
					DxTraceMsg("%s line(%d) IDirect3DDevice9::GetBackBuffer failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return false;
				}	
				pBackSurface->GetDesc(&Desc);
				RECT dstrt = { 0, 0, Desc.Width, Desc.Height };
				RECT srcrt = { 0, 0, pAvFrame->width, pAvFrame->height };	
				if (pClippedRT)
				{
					CopyRect(&srcrt, pClippedRT);
				}
				hr = m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, &srcrt, pBackSurface, &dstrt, D3DTEXF_LINEAR);
				SaveRunTime();
				
				SafeRelease(pBackSurface);
				m_pDirect3DDevice->EndScene();
				SaveRunTime();
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
		SaveRunTime();
		hr = m_pDirect3DDevice->Present(NULL, pRenderRt, hRenderWnd, NULL);
		SaveRunTime();
		
#ifdef _DEBUG
		double dfT4 = GetExactTime();
		//DxTraceMsg("%s TimeSpan(T3-T4)\t%.6f\n",__FUNCTION__,dfT4 - dfT3);
#endif
		if (SUCCEEDED(hr))
			return true;
		else
			return HandelDevLost();	
	}

	// 设置是以固定比便显示视频
	// 当bScaleFixed为true,并且dfWHScale = 0时,则使用图像原始比例,比例值为Width/Height
	// 当bScaleFixed为true,并且dfWHScale 大于0时,则使用dfWHScale提供的比例显示，图像可能会被拉伸变形
	// 当bScaleFixed为false,dfWHScale参数将被忽略,此时像自动填满窗口客户区
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
	// 设置显示窗口是否要被子类化
	// bWndSubclass为ture，则AttachWnd时会显示窗口进行子类化，否则不会执行子类化操作
	// 此函数必须在AttachWnd前执行才有效
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
// 				if (!pSurface->IsInited())		// D3D设备尚未创建,说明未初始化
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
			{// 切换窗口模式,要从m_WndSurfaceMap删除窗口句柄
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
	// 1.检查指定的表面像素格式，是否在指定的适配器类型、适配器像素格式下可用。
	// GetAdapterDisplayMode,CheckDeviceType的应用
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

	// 2.根据适配器类型，获取顶点运算(变换和光照运算)的格式
	// D3DCAPS9结构体，GetDeviceCaps的应用
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

	// 3.输出显卡信息,Description描述，厂商型号，Dircet3D的驱动Driver版本号，显卡的唯一标识号：DeviceIdentifier
	// GetAdapterCount()，GetAdapterIdentifier的使用。
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

	// 4.输出指定Adapter，显卡像素模式(不会与缓存表面格式做兼容考虑)的显卡适配器模式信息
	// GetAdapterModeCount,EnumAdapterModes的使用
	void PrintDisplayModeInfo(D3DFORMAT fmt)
	{
		if(m_pDirect3D9 == NULL)
		{
			DxTraceMsg("%s Direct3D9 not initialized.\n",__FUNCTION__);
			return;
		}
		// 显卡适配器模式的个数，主要是分辨率的差异
		DWORD nAdapterModeCount=m_pDirect3D9->GetAdapterModeCount(D3DADAPTER_DEFAULT, fmt);
		if(nAdapterModeCount == 0)
		{
			DxTraceMsg("%s D3DFMT_格式：%x不支持", __FUNCTION__,fmt);
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

	// 5.对于指定的资源类型，检查资源的使用方式，资源像素格式，在默认的显卡适配器下是否支持
	// GetAdapterDisplayMode，CheckDeviceFormat的使用
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

	// 6.对指定的表面像素格式，窗口模式，和显卡像素模式；检查对指定的多重采样类型支持不，且返回质量水平等级
	// CheckDeviceMultiSampleType的应用
	bool CheckMultiSampleType(D3DFORMAT surfaceFmt,BOOL bWindow, D3DMULTISAMPLE_TYPE &eSampleType, DWORD *pQualityLevel)
	{
		//变量MultiSampleType的值设为D3DMULTISAMPLE_NONMASKABLE，就必须设定成员变量MultiSampleQuality的质量等级值
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

	// 7.根据显卡适配器和目标缓存类型，检查指定深度缓存的格式是否支持
	// CheckDepthStencilMatch的应用
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
		// 1.CheckDeviceFormat,显卡模式和表面会做兼容处理
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
		// 3.显示显卡的信息
		PrintDisplayInfo();

		D3DDISPLAYMODE d3ddm;
		if(FAILED(m_pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm)))
			return false;
		// 4.输出显卡适配器模式信息,不会与缓存表面格式做兼容考虑
		PrintDisplayModeInfo(d3ddm.Format);

		// 5.显卡模式和资源表面会做兼容处理
		int nUsageTexture = D3DUSAGE_WRITEONLY;
		D3DFORMAT fmtTexture = D3DFMT_A8R8G8B8;

		if(!CheckResourceFormat(nUsageTexture/*D3DUSAGE_DEPTHSTENCIL*/,
			D3DRTYPE_TEXTURE/*D3DRTYPE_SURFACE*/,d3ddm.Format/*D3DFMT_D15S1*/))
		{
			DxTraceMsg("%s CheckResourceFormat Texture Resource FMT failed.\n"__FUNCTION__);
			return false;
		}

		// 6.采样纹理
		D3DMULTISAMPLE_TYPE eSampleType = D3DMULTISAMPLE_16_SAMPLES;// 测试结果本机只是支持4,2类型的采样纹理
		DWORD dwQualityLevel = 0;
		if(!CheckMultiSampleType(d3ddm.Format, bIsWindowed,  eSampleType,&dwQualityLevel))
		{
			eSampleType = D3DMULTISAMPLE_NONE;
		}

		// 7.深度缓存检测
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
		d3dpp.MultiSampleQuality        = dwQualityLevel;				// 不能用dwQualityLevel
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

// CDxSurfaceEx类，仅限于Windows Vista及以上操作系统下使用
// 其性能与稳定性比CDxSurface要强，维护也更方便

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
		// 释放由基类创建的Direct3D9对象
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
		SafeRelease(m_pSnapshotSurface);
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

	// 调用InitD3D之前必须先调用AttachWnd函数关联视频显示窗口
	// nD3DFormat 必须为以下格式之一
	// MAKEFOURCC('Y', 'V', '1', '2')	默认格式,可以很方便地由YUV420P转换得到,而YUV420P是FFMPEG解码后得到的默认像素格式
	// MAKEFOURCC('N', 'V', '1', '2')	仅DXVA硬解码使用该格式
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
		bool bZoomWnd = false;			// 是否需要扩大窗口,DirectX不能在窗口像素面积为0的窗口上工作
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
		caps.DevCaps的取值及含义
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
		D3DDEVCAPS_QUINTICRTPATCHES			Device supports quintic Bézier curves and B-splines.
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
		m_d3dpp.MultiSampleType			= D3DMULTISAMPLE_NONE;					// 显示视频时，不宜使用多重采样，否则将导致画面错乱
#pragma warning(push)
#pragma warning(disable:4800)
		m_bFullScreen					= (bool)bIsWindowed;
#pragma warning(pop)		

		if (bIsWindowed)//窗口模式
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
						
			m_d3dpp.FullScreen_RefreshRateInHz	= 0;							// 显示器刷新率，窗口模式该值必须为0
			m_d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;		// 指定系统如何将后台缓冲区的内容复制到前台缓冲区 D3DSWAPEFFECT_DISCARD:清除后台缓存的内容
			m_d3dpp.BackBufferHeight			= nVideoHeight;
			m_d3dpp.BackBufferWidth				= nVideoWidth;
			m_d3dpp.EnableAutoDepthStencil		= FALSE;							// 关闭自动深度缓存
			m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		}
		else
		{
			//全屏模式
			m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
			m_d3dpp.FullScreen_RefreshRateInHz	= d3ddm.RefreshRate;	
			m_d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD; 
			m_d3dpp.EnableAutoDepthStencil		= FALSE;			
			m_d3dpp.BackBufferWidth				= GetSystemMetrics(SM_CXSCREEN);		// 获得屏幕宽
			m_d3dpp.BackBufferHeight			= GetSystemMetrics(SM_CYSCREEN);		// 获得屏幕高

			GetWindowPlacement(m_d3dpp.hDeviceWindow, &m_WndPlace ) ;
			m_dwExStyle	 = GetWindowLong( m_d3dpp.hDeviceWindow, GWL_EXSTYLE ) ;
			m_dwStyle	 = GetWindowLong( m_d3dpp.hDeviceWindow, GWL_STYLE ) ;
			m_dwStyle	 &= ~WS_MAXIMIZE & ~WS_MINIMIZE; // remove minimize/maximize style
			m_hMenu		 = GetMenu( m_d3dpp.hDeviceWindow ) ;
		}
		/*
		CreateDevice的BehaviorFlags参数选项：
		D3DCREATE_ADAPTERGROUP_DEVICE只对主显卡有效，让设备驱动输出给它所拥有的所有显示输出
		D3DCREATE_DISABLE_DRIVER_MANAGEMENT代替设备驱动来管理资源，这样在发生资源不足时D3D调用不会失败
		D3DCREATE_DISABLE_PRINTSCREEN:不注册截屏快捷键，只对Direct3D 9Ex
		D3DCREATE_DISABLE_PSGP_THREADING：强制计算工作必须在主线程上，vista以上有效
		D3DCREATE_ENABLE_PRESENTSTATS：允许GetPresentStatistics收集统计信息只对Direct3D 9Ex
		D3DCREATE_FPU_PRESERVE；强制D3D与线程使用相同的浮点精度，会降低性能
		D3DCREATE_HARDWARE_VERTEXPROCESSING：指定硬件进行顶点处理，必须跟随D3DCREATE_PUREDEVICE
		D3DCREATE_MIXED_VERTEXPROCESSING：指定混合顶点处理
		D3DCREATE_SOFTWARE_VERTEXPROCESSING：指定纯软的顶点处理
		D3DCREATE_MULTITHREADED：要求D3D是线程安全的，多线程时
		D3DCREATE_NOWINDOWCHANGES：拥有不改变窗口焦点
		D3DCREATE_PUREDEVICE：只试图使用纯硬件的渲染
		D3DCREATE_SCREENSAVER：允许被屏保打断只对Direct3D 9Ex
		D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DCREATE_MIXED_VERTEXPROCESSING, and D3DCREATE_SOFTWARE_VERTEXPROCESSING中至少有一个一定要设置
		*/
		if (m_pDirect3DDeviceEx)
			SafeRelease(m_pDirect3DDeviceEx);

		// 检查是否能够启用多重采样
		hr = m_pDirect3D9Ex->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, FALSE, D3DMULTISAMPLE_4_SAMPLES, NULL);

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
		// 保存参数
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

	// D3dDirect9Ex下,取代ResetDevice()函数
	bool  ResetDevice()
	{
		HRESULT hr = S_OK;
		// 使用D3d9DeviceEx时，无须重新创建表面资源
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

	// D3dDirect9Ex下，该成员不再有效
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
				if (!ResetDevice())
				{
					DxCleanup();
					return InitD3D(m_d3dpp.hDeviceWindow, m_nVideoWidth, m_nVideoHeight, m_bFullScreen);
				}
			}
			break;
		case S_PRESENT_OCCLUDED:	// 窗口被其它窗口模式的画面遮挡或全屏画面被最小化,若为全屏窗口，
			// 则此时可停止渲染，直接到收到WM_ACTIVATEAPP时，可重新开始演染
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
		case D3DERR_DEVICENOTRESET:	// 设备丢失,但不再需要重新创建所有资源
			{
				DxTraceMsg("%s The device is not Reset.\n",__FUNCTION__);
				return ResetDevice();
			}
			break;
		case D3DERR_DEVICEHUNG:		// 需要复位IDirect3DDeviceEx对象,但不需要重建IDirect3DEx对象
			{
				DxTraceMsg("%s The device is hung.\n",__FUNCTION__);
				DxCleanup();
				return InitD3D(m_d3dpp.hDeviceWindow,m_nVideoWidth,m_nVideoHeight,m_bFullScreen);	
			}
			break;
		case D3DERR_DEVICEREMOVED:	// 需要重新创建IDirect3DEx对象
			{
				DxTraceMsg("%s if the device has been removed.\n",__FUNCTION__);
				DxCleanup();
				SafeRelease(m_pDirect3D9Ex);
				hr = m_pDirect3DCreate9Ex(D3D_SDK_VERSION, &m_pDirect3D9Ex);
				if (FAILED(hr))
					return false;
				return InitD3D(m_d3dpp.hDeviceWindow,m_nVideoWidth,m_nVideoHeight,m_bFullScreen);	
			}
			break;
		default:
			return false;
		}

		return true;
	}

#define RenderTimeout	100 //ms
bool Render(AVFrame *pAvFrame, HWND hWnd = NULL, RECT *pClippedRT = nullptr, RECT *pRenderRt = nullptr)
	{
		if (!pAvFrame)
			return false;
		DeclareRunTime(5);
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
		// HandelDevLost仍无法使用m_pDirect3DDevice，则直接返回false
		if (!m_pDirect3DDeviceEx)
			return false;
		
		switch(pAvFrame->format)
		{
		case  AV_PIX_FMT_DXVA2_VLD:
			{// 硬解码帧，可以直接显示
				IDirect3DSurface9* pRenderSurface = m_pDirect3DSurfaceRender;	
				IDirect3DSurface9* pDXVASurface = (IDirect3DSurface9 *)pAvFrame->data[3];
				if (m_bD3DShared)
				{	
					pRenderSurface = pDXVASurface;
					if (!pDXVASurface)
						return false;
				}
				else
				{
					D3DSURFACE_DESC DXVASurfaceDesc, DstSurfaceDesc;
					pDXVASurface->GetDesc(&DXVASurfaceDesc);
					m_pDirect3DSurfaceRender->GetDesc(&DstSurfaceDesc);
					hr = pDXVASurface->LockRect(&SrcRect, nullptr, D3DLOCK_READONLY);
					D3DLOCKED_RECT DstRect;
					hr |= m_pDirect3DSurfaceRender->LockRect(&DstRect, NULL, D3DLOCK_DONOTWAIT);
					if (FAILED(hr))
					{
						DxTraceMsg("%s line(%d) IDirect3DSurface9::LockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
						return false;
					}
					gpu_memcpy(DstRect.pBits, SrcRect.pBits, SrcRect.Pitch*DstSurfaceDesc.Height * 3 / 2);
					/*
					// Y分量图像
					uint8_t *pY = (uint8_t*)DstRect.pBits;
					// UV分量图像
					uint8_t *pUV = (uint8_t*)DstRect.pBits + DstRect.Pitch * DstSurfaceDesc.Height;
					*/
	
					hr |= m_pDirect3DSurfaceRender->UnlockRect();
					hr |= pDXVASurface->UnlockRect();
					//DxTraceMsg("hr = %08X.\n", hr);
					if (FAILED(hr))
					{
						DxTraceMsg("%s line(%d) IDirect3DSurface9::UnlockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
						return false;
					}
				}

				// 处理截图请求
				//TransferSnapShotSurface(pAvFrame);

				// 处理外部分绘制接口
				ExternDrawCall(hWnd,pRenderRt);

				IDirect3DSurface9 * pBackSurface = NULL;
				//m_pDirect3DDeviceEx->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
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
				if (pClippedRT)
				{
					CopyRect(&srcrt, pClippedRT);
				}
				hr = m_pDirect3DDeviceEx->StretchRect(pRenderSurface, &srcrt, pBackSurface, &dstrt, D3DTEXF_NONE/*D3DTEXF_LINEAR*/);
				pBackSurface->Release();
				m_pDirect3DDeviceEx->EndScene();
				break;
			}
		default:		
			{// 软解码帧，只支持YUV420P格式
				SaveRunTime();
				//TransferSnapShotSurface(pAvFrame);
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
					CopyFrameYUV420P(&d3d_rect,Desc.Height, pAvFrame);
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
						CopyFrameYUV420P(&d3d_rect,Desc.Height, m_pPixelConvert->pFrameNew);
					else					
						memcpy_s((byte *)d3d_rect.pBits, d3d_rect.Pitch*Desc.Height, m_pPixelConvert->pImage, m_pPixelConvert->nImageSize);
				}
				SaveRunTime();
				hr = m_pDirect3DSurfaceRender->UnlockRect();
				if (FAILED(hr))
				{
					//DxTraceMsg("%s line(%d) IDirect3DSurface9::UnlockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
					return false;
				}
				SaveRunTime();
				// 处理外部分绘制接口
				ExternDrawCall(hWnd,pRenderRt);
				SaveRunTime();
				IDirect3DSurface9 * pBackSurface = NULL;	
				//m_pDirect3DDeviceEx->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
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
				if (pClippedRT)
				{
					CopyRect(&srcrt, pClippedRT);
				}
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