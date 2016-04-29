
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

// #include <vld.h>
// #include <vld_def.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS
#pragma warning(disable:4996)
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

// ��Э�����ݰ�Э���ʽ��ȷ���������������
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


