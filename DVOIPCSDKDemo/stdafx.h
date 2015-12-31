
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once
// #include <vld_def.h>
// #include <vld.h>
#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS


#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

#include "../DVOIPCNETSDK/dvoipcnetsdk.h"
#ifdef _DEBUG
#pragma comment(lib, "../DVOIPCNETSDK/dvoipcnetsdk_d.lib")
#else
#pragma comment(lib, "../DVOIPCNETSDK/dvoipcnetsdk.lib")
#endif
#include "FullScreen.h"

#include "../MulMonitor/MultiMonitorAPI.h"
#pragma comment(lib,"../MulMonitor/MultiMonitorAPI")

//#include <Windows.h>
#include <WindowsX.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavcodec/dxva2.h"
#ifdef __cplusplus
}
#endif

//#define 
//#define _ENABLE_FFMPEG_LAV

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
#elif defined( _ENABLE_FFMPEG_LAV)
#pragma comment(lib,"avcodec-lav.lib")
#pragma comment(lib,"avformat-lav.lib")
#pragma comment(lib,"avutil-lav.lib")
#pragma comment(lib,"swscale-lav.lib")	
#else
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swscale.lib")	
#endif	




#include <list>
#include <vector>
#include <memory>
using namespace std;
using namespace std::tr1;

// #include <boost/smart_ptr.hpp>
// #include <boost/make_shared.hpp>
//using namespace boost;
#include "Utility.h"
#include "TimeUtility.h"
#include "AutoLock.h"
#include "Markup.h"


enum	DVO_CODEC
{
	DVO_CODEC_H264		= 0,
	DVO_CODEC_H265		= 1
};
enum	DVO_ConnectMode
{
	DVO_TCP = 0,
	DVO_UDP
};


#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


extern HANDLE g_hStdout; 
extern HANDLE g_hStdin;

#define Var2Str(var) (#var)

// ��Э�����ݰ�Э���ʽ��ȷ���������������
void _TraceProtocol(byte *szBuffer,int nBufferLength);

