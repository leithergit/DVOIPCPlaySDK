
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

// #include <vld.h>
// #include <vld_def.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS
#pragma warning(disable:4996)
#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#include <afxdisp.h>        // MFC 自动化类

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
#include "../DVOIPCNETSDK/dvoipcnetsdk.h"
#include "../DVOIPCPlaySDK/DVOIPCPlaySDK.h"

#ifdef _DEBUG
#pragma comment(lib, "../DVOIPCNETSDK/dvoipcnetsdk.lib")
#pragma comment(lib, "../Debug/DVOIPCPlaySDK.lib")
#else
#pragma comment(lib, "../DVOIPCNETSDK/dvoipcnetsdk.lib")
#pragma comment(lib, "../Release/DVOIPCPlaySDK.lib")
#endif

#include "../MulMonitor/MultiMonitorAPI.h"
#pragma comment(lib,"../MulMonitor/MultiMonitorAPI.lib")

#include "Utility.h"
#include "TimeUtility.h"
#include "Markup.h"
#include <list>
#include <vector>
#include <memory>
#include <WindowsX.h>
using namespace std;
using namespace std::tr1;

extern HANDLE g_hStdout;
extern HANDLE g_hStdin;

// #include "VCA5CoreLib.h"
// #include "Cap5BoardLibEx.h"
// #include "VCAMetaLib.h"
//#include "vca.h"
#define Var2Str(var) (#var)
//extern CVca *g_pVca;

// 把协议数据按协议格式明确解析出来，并输出
void _TraceProtocol(byte *szBuffer, int nBufferLength);


//#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//#endif


