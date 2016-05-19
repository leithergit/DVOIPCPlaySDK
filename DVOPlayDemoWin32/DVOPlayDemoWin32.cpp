// DVOPlayDemoWin32.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <windowsx.h>
#include "DVOPlayDemoWin32.h"
#include "SocketClient.h"
#include <MMSystem.h>
#include <memory>
#include <Shlwapi.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include "../DVOIPCPlaySDK/Utility.h"
#include "../DVOIPCPlaySDK/DVOIPCPlaySDK.h"
#include "../dvoipcnetsdk/dvoipcnetsdk.h"
#pragma  comment(lib,"dvoipcnetsdk.lib")
#pragma comment(lib, "DVOIPCPlaySDK.lib")
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"comctl32.lib")
using namespace std;
using namespace std::tr1;
#define MAX_LOADSTRING 100
#define ID_TIMER_CHECKSTREAM	1024
#define ID_TIMER_SNAPSHOT		1025

#ifdef Release_D
#undef assert
#define assert	((void)0)
#endif

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
int		g_nSnapIndex = 0;
int		g_nSnapShotCount = 0;
int		g_nSnapShotedCount = 0;
HWND	g_hListMessage = NULL;
HWND	g_hListCamera = NULL;
vector<string> g_vIPCamera;
#ifdef EnableDlgItem
#undef EnableDlgItem
#define EnableDlgItem(hWnd,ID,enable) {::EnableWindow(::GetDlgItem(hWnd,ID),enable);}
#endif
typedef void (CALLBACK* fnDVOCallback_RealAVData_T)( USER_HANDLE lUserID,  REAL_HANDLE lStreamHandle,  int nErrorType,  const char* pBuf,  int nDataSize,  void* pUser);

/// @brief		解码后YVU数据回调
void __stdcall _CaptureYUV(DVO_PLAYHANDLE hPlayHandle,
	const unsigned char* pYUV,
	int nSize,
	int nWidth,
	int nHeight,
	INT64 nTime,
	void *pUserPtr);
void  __stdcall StreamCallBack(IN USER_HANDLE  lUserID,
	IN REAL_HANDLE lStreamHandle,
	IN int         nErrorType,
	IN const char* pBuffer,
	IN int         nDataLen,
	IN void*       pUser);
#define		STREAM_COUNT		4
struct PlayerContext
{
public:
		USER_HANDLE hUser;
		REAL_HANDLE		hStream;
		DVO_PLAYHANDLE	hPlayer;
		DWORD			dwLastStreamTime;
		int				nReOpenStream;		// 重新打开码流的次数
		volatile bool bThreadRecvIPCStream ;
		DVO_PLAYHANDLE	hPlayerStream;		// 流播放句柄
		DWORD		nVideoFrames/* = 0*/;
		DWORD		nAudioFrames/* = 0*/;
		double		dfTimeRecv1 /*= 0.0f*/;
		double		dfTimeRecv2/* = 0.0f*/;
		HWND		hWndView;
		int			nItem;
		HANDLE		hYUVFile;
		bool		bRecvIFrame;
		time_t		tLastFrame;
		int			nFPS;
		UINT		nVideoFrameID;
		UINT		nAudioFrameID;
		int			nAudioCodec;
		bool		bWriteMediaHeader;
		time_t		tRecStartTime;
		UINT64		nRecFileLength;
		TCHAR		szIpAddress[32];
		TCHAR		szRecFilePath[MAX_PATH];
		__int64		nTimeStamp[100];
		int			nFirstID/* = 0*/;
		int			nTimeCount;

public:
	PlayerContext(USER_HANDLE hUserIn,
		REAL_HANDLE hStreamIn = -1,
		DVO_PLAYHANDLE hPlayerIn = NULL, int nCount = 1)
	{
		ZeroMemory(this, sizeof(PlayerContext));
		nTimeCount = 0;
		hUser = hUserIn;
		hStream = -1;
		hPlayer = hPlayerIn;
		bThreadRecvIPCStream = false;
	}
	bool GetStreamInfo(int nStream,DVO_MEDIAINFO *pMediaHeader,int &nError)
	{
		if (!pMediaHeader)
			return false;

		app_net_tcp_enc_info_t stResult = { 0 };
		app_net_tcp_com_schn_t req = { 0 };
		req.chn = 0;
		req.schn = nStream;

		for (int k = 0; k < 3; k++)
		{
			int nReBytes = 0;
			nError = DVO2_NET_GetDevConfig(hUser, DVO_DEV_CMD_STREAM_VIDEO_ENC_GET, &req, sizeof(app_net_tcp_com_schn_t), &stResult, sizeof(app_net_tcp_enc_info_t), &nReBytes);
			if (RET_SUCCESS == nError)
			{
				enum APP_NET_TCP_COM_VIDEO_MODE
				{
					APP_NET_TCP_COM_VIDEO_MODE_352_288 = 0,
					APP_NET_TCP_COM_VIDEO_MODE_704_576,
					APP_NET_TCP_COM_VIDEO_MODE_1280_720,
					APP_NET_TCP_COM_VIDEO_MODE_1920_1080,
					APP_NET_TCP_COM_VIDEO_MODE_1280_960,
					APP_NET_TCP_COM_VIDEO_MODE_1024_768,
					APP_NET_TCP_COM_VIDEO_MODE_176_144 = 0xFF,
					APP_NET_TCP_COM_VIDEO_MODE_MAX,
				}; //视频编码尺寸。
				pMediaHeader->nVideoCodec = (DVO_CODEC)stResult.enc_type;
				switch (stResult.fmt.enc_mode)
				{
				case APP_NET_TCP_COM_VIDEO_MODE_352_288:
				{
					pMediaHeader->nVideoWidth = 352;
					pMediaHeader->nVideoHeight = 288;
					break;
				}
				case	APP_NET_TCP_COM_VIDEO_MODE_704_576:
				{
					pMediaHeader->nVideoWidth = 704;
					pMediaHeader->nVideoHeight = 576;
					break;
				}
				case	APP_NET_TCP_COM_VIDEO_MODE_1280_720:
				{
					pMediaHeader->nVideoWidth = 1280;
					pMediaHeader->nVideoHeight = 720;
					break;
				}
				case	APP_NET_TCP_COM_VIDEO_MODE_1920_1080:
				{
					pMediaHeader->nVideoWidth = 1920;
					pMediaHeader->nVideoHeight = 1080;
					break;
				}
				case	APP_NET_TCP_COM_VIDEO_MODE_1280_960:
				{
					pMediaHeader->nVideoWidth = 1280;
					pMediaHeader->nVideoHeight = 960;
					break;
				}
				case	APP_NET_TCP_COM_VIDEO_MODE_1024_768:
				{
					pMediaHeader->nVideoWidth = 1024;
					pMediaHeader->nVideoHeight = 768;
					break;
				}
				case	APP_NET_TCP_COM_VIDEO_MODE_176_144:
				{
					pMediaHeader->nVideoWidth = 176;
					pMediaHeader->nVideoHeight = 144;
					break;
				}
				break;
				default:
				{
					return false;
					break;
				}
				}
				break;
			}
		}
		return true;
	}
	~PlayerContext()
	{
		
		if (hStream != -1)
		{
			DVO2_NET_StopRealPlay(hStream);
			hStream = -1;
		}
		if (hPlayer)
		{
			dvoplay_Close(hPlayer);
			hPlayer = NULL;
		}
		
		if (hPlayerStream)
		{
			dvoplay_Close(hPlayerStream);
			hPlayerStream = NULL;
		}
		
		if (hUser != -1)
		{
			DVO2_NET_Logout(hUser);
			hUser = -1;
		}
	}
};




shared_ptr<PlayerContext> m_pPlayContext[STREAM_COUNT];/* = { NULL, NULL, NULL, NULL }*/;
BOOL CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK FilePlayDialogProc(HWND hDlg, UINT uMsg,WPARAM wParam, LPARAM lParam);
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;
	InitCommonControls();
	int nResult = DialogBox(hInst, MAKEINTRESOURCE(IDD_DVOIPCPLAY), NULL, (DLGPROC)DialogProc);
	//int nResult = DialogBox(hInst, MAKEINTRESOURCE(IDD_FILEPLAY), NULL, (DLGPROC)FilePlayDialogProc);
	nResult = GetLastError();
	
	return (int)0;
}

DWORD GetExStyle(HWND hWnd) 
{
	return (DWORD)GetWindowLong(hWnd, GWL_EXSTYLE);
}
DWORD GetStyle(HWND hWnd) 
{
	return (DWORD)GetWindowLong(hWnd, GWL_STYLE);
}
int RectWidth(RECT & rt)
{
	return (rt.right - rt.left);
}

int RectHeight(RECT &rt)
{
	return (rt.bottom - rt.top);
}

void CenterWindow(HWND hAlternateOwner)
{
	// determine owner window to center against
	DWORD dwStyle = GetStyle(hAlternateOwner);
	HWND hWndCenter = hAlternateOwner;
	// get coordinates of the window relative to its parent
	RECT rcDlg;
	GetWindowRect(hWndCenter,&rcDlg);
	RECT rcArea;
	RECT rcCenter;
	HWND hWndParent;
	if (!(dwStyle & WS_CHILD))
	{
		// don't center against invisible or minimized windows
		if (hWndCenter != NULL)
		{
			DWORD dwAlternateStyle = ::GetWindowLong(hWndCenter, GWL_STYLE);
			if (!(dwAlternateStyle & WS_VISIBLE) || (dwAlternateStyle & WS_MINIMIZE))
				hWndCenter = NULL;
		}

		MONITORINFO mi;
		mi.cbSize = sizeof(mi);

		
		{
			::GetWindowRect(hWndCenter, &rcCenter);
			GetMonitorInfo(	MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST), &mi);
			rcArea = mi.rcWork;
		}
	}
	else
	{
		// center within parent client coordinates
		hWndParent = ::GetParent(hAlternateOwner);
		if (!hWndParent)
			return;

		::GetClientRect(hWndParent, &rcArea);
		::GetClientRect(hWndCenter, &rcCenter);
		::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
	}

	// find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - RectWidth(rcDlg) / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - RectHeight(rcDlg) / 2;

	// if the dialog is outside the screen, move it inside
	if (xLeft + RectWidth(rcDlg) > rcArea.right)
		xLeft = rcArea.right - RectWidth(rcDlg);
	if (xLeft < rcArea.left)
		xLeft = rcArea.left;

	if (yTop + RectWidth(rcDlg) > rcArea.bottom)
		yTop = rcArea.bottom - RectHeight(rcDlg);
	if (yTop < rcArea.top)
		yTop = rcArea.top;

	// map screen coordinates to child coordinates
	::SetWindowPos(hWndCenter, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	
}
#define WSA_VERSION MAKEWORD(2,0)

// long __stdcall OnReConnect(WPARAM W,LPARAM L)
// {
// 	PlayerContext *pContext = (PlayerContext *)W;
// 	if (pContext->OpenVideoStream(pContext->nVideoStreams))
// 	{
// 		CSocketClient::TraceMsgA("%s 重连码流成功.", __FUNCTION__);
// 	}
// 	else
// 		CSocketClient::TraceMsgA("%s 重连码流失败.", __FUNCTION__);
// 	return 0;
// }

BOOL ReOpenStream(int nIndex , int nStream,HWND hWnd)
{
	char szText[128] = { 0 };
	if (m_pPlayContext[nIndex])
	{
		int nError = 0;
		DVO2_NET_StopRealPlay(m_pPlayContext[nIndex]->hStream);
		if (m_pPlayContext[nIndex]->hPlayer)
		{
			dvoplay_Close(m_pPlayContext[nIndex]->hPlayer);
			m_pPlayContext[nIndex]->hPlayer = NULL;
			m_pPlayContext[nIndex]->hStream = -1;
		}
		//Sleep(100);
		int nRetry = 0;
		REAL_HANDLE hStream = -1;
		while (hStream == -1)
		{
			hStream = DVO2_NET_StartRealPlay(m_pPlayContext[nIndex]->hUser,
				0,
				nStream,
				DVO_TCP,
				0,
				NULL,
				(fnDVOCallback_RealAVData_T)StreamCallBack,
				(void *)m_pPlayContext[nIndex].get(),
				&nError);
			nRetry++;
			if (nRetry >= 3)
				break;
			Sleep(500);
		}
		
		if (hStream == -1)
		{
			sprintf(szText, "连接码流失败[%d].", nIndex);
			ListBox_InsertString(g_hListMessage,0, szText);
			return FALSE;
		}
		m_pPlayContext[nIndex]->hStream = hStream;
		m_pPlayContext[nIndex]->dwLastStreamTime = timeGetTime();
		m_pPlayContext[nIndex]->nReOpenStream++;
				
		DVO_MEDIAINFO MediaHeader;
		MediaHeader.nVideoCodec = CODEC_H264;
		if (nStream == 0)
		{
			MediaHeader.nVideoWidth = 1280;
			MediaHeader.nVideoHeight = 720;
			TraceMsgA("%s 切换至主码流.\n", __FUNCTION__);
		}
		else
		{
			MediaHeader.nVideoWidth = 352;
			MediaHeader.nVideoHeight = 288;
			TraceMsgA("%s 切换至副码流.\n", __FUNCTION__);
		}
		HWND hVideo = GetDlgItem(hWnd, IDC_VIDEO_FRAME1 + nIndex);
		sprintf(szText, "dvoipcplaysdk_%s", g_vIPCamera[nIndex].c_str());
		//m_pPlayContext[nIndex]->hPlayer = dvoplay_OpenStream(NULL, (byte *)&MediaHeader, sizeof(MediaHeader), 128, "dvoipcplaysdk");
		m_pPlayContext[nIndex]->hPlayer = dvoplay_OpenStream(hVideo, (byte *)&MediaHeader, sizeof(MediaHeader), 128, szText);
		if (!m_pPlayContext[nIndex]->hPlayer)
		{
			sprintf(szText, "%s 打开流播放句柄失败[%d].",__FUNCTION__, nIndex);
			ListBox_InsertString(g_hListMessage,0, szText);
			return false;
		}
		m_pPlayContext[nIndex]->dwLastStreamTime = timeGetTime();
		dvoplay_Refresh(m_pPlayContext[nIndex]->hPlayer);
		dvoplay_Start(m_pPlayContext[nIndex]->hPlayer, false, true);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL SnapShot(int nIndex,HWND hDlg)
{
	char szText[1024] = { 0 };
	if (m_pPlayContext[nIndex] && m_pPlayContext[nIndex]->hPlayer)
	{
		CHAR szPath[1024];
		GetAppPathA(szPath, 1024);
		_tcscat(szPath,  _T("\\ScreenSave"));
		if (!PathFileExists(szPath))
		{
			if (!CreateDirectory(szPath, NULL))
			{
				sprintf(szText, "%s 无法创建保存截图文件的目录,请确认是否有足够的权限[%d].", __FUNCTION__, nIndex);
				ListBox_InsertString(g_hListMessage,0, szText);
				return FALSE;
			}
		}
		TCHAR szFileName[512] = { 0 };
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		// 生成文件名
		sprintf(szFileName,
			_T("\\SnapShot_%04d%02d%02d_%02d%02d%02d_%03d.jpg"),
			systime.wYear, systime.wMonth, systime.wDay,
			systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
		_tcscat_s(szPath, 1024, szFileName);
		if (dvoplay_SnapShot(m_pPlayContext[nIndex]->hPlayer, szPath, XIFF_JPG) == DVO_Succeed)
		{
			sprintf(szText, "%s 已经生成截图[%d]:%s.", __FUNCTION__, nIndex,szPath);
			ListBox_InsertString(g_hListMessage,0, szText);
			return TRUE;
		}
		else
		{
			sprintf(szText, "%s 播放器尚未启动,无法截图[%d].", __FUNCTION__, nIndex);
			ListBox_InsertString(g_hListMessage,0, szText);
			return FALSE;
		}
	}
	return FALSE;
}
BOOL OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
{
	CenterWindow(hDlg);
	SetDlgItemText(hDlg, IDC_EDIT_ACCOUNT, "admin");
	SetDlgItemText(hDlg, IDC_EDIT_PASSWORD, "admin");
	SetDlgItemText(hDlg, IDC_IPADDRESS, "192.168.3.127");
	SetDlgItemText(hDlg, IDC_EDIT_SNAPSHOT_COUNT, "-1");
	SetDlgItemText(hDlg, IDC_EDIT_SNAPSHOT_INTERVAL, "10");
	SetDlgItemText(hDlg, IDC_EDIT_SNAPSHOTED_COUNT, "0");
	HWND hStreamCombo = GetDlgItem(hDlg, IDC_COMBO_STREAM);
	ComboBox_AddString(hStreamCombo, "0");
	ComboBox_AddString(hStreamCombo, "1");
	ComboBox_AddString(hStreamCombo, "2");
	ComboBox_AddString(hStreamCombo, "3");
	ComboBox_SetCurSel(hStreamCombo, 0);
	g_hListMessage = GetDlgItem(hDlg, IDC_LIST_MESSAGE);
	g_hListCamera = GetDlgItem(hDlg, IDC_LIST_CAMERA);
	CheckDlgButton(hDlg, IDC_RADIO_PLAYSTREAM, BST_CHECKED);
	LV_COLUMN column;
	column.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
	column.pszText = "序号";
	column.fmt = LVCFMT_LEFT;
	column.cx = 50;
	ListView_InsertColumn(g_hListCamera, 0, &column);

	column.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
	column.pszText = "IP";
	column.fmt = LVCFMT_LEFT;
	column.cx = 100;
	ListView_InsertColumn(g_hListCamera, 1, &column);
	return TRUE;

}
void OnDestroy(HWND hDlg)
{
	ClearD3DCache();
}
void OnClose(HWND hDlg)
{
	for (int i = 0; i < g_vIPCamera.size();i ++)
		m_pPlayContext[i].reset();
	EndDialog(hDlg, IDOK);
}

void OnButtonAddList(HWND hDlg)
{
	char szIPCamera[32] = { 0 };
	char szText[64] = { 0 };
	GetDlgItemText(hDlg, IDC_IPADDRESS, szIPCamera, 32);
	g_vIPCamera.push_back(szIPCamera);
	sprintf(szText, "%d", g_vIPCamera.size());
	LVITEM item;
	item.mask = LVIF_TEXT;
	item.state = 0;
	item.stateMask = 0;
	item.iImage = 0;
	item.lParam = 0;
	item.pszText =  szText;
	item.iItem = g_vIPCamera.size() - 1;
	item.iSubItem = 0;
	ListView_InsertItem(g_hListCamera, &item);
	item.pszText = szIPCamera;
	item.iSubItem = 1;
	ListView_SetItem(g_hListCamera, &item);
}

void OnButtonConnect(HWND hDlg)
{

	CHAR szAccount[32] = { 0 };
	CHAR szPassowd[32] = { 0 };
	CHAR szIPAddress[32] = { 0 };
	GetDlgItemText(hDlg,IDC_EDIT_ACCOUNT, szAccount, 32);
	GetDlgItemText(hDlg, IDC_EDIT_ACCOUNT, szPassowd, 32);
	DVO2_NET_Init(true);
	int nStream = SendDlgItemMessage(hDlg, IDC_COMBO_STREAM, CB_GETCURSEL,0,0);
	for (int i = 0; i < g_vIPCamera.size();i ++)
		if (m_pPlayContext[i])
			m_pPlayContext[i].reset();

	app_net_tcp_sys_logo_info_t LoginInfo;
	int nError = 0;
	USER_HANDLE hUser = -1;
	for (int i = 0; i < g_vIPCamera.size(); i++)
	{
		hUser = DVO2_NET_Login(g_vIPCamera[i].c_str(), 6001, szAccount, szPassowd, &LoginInfo, &nError, 5000);
		if (hUser != -1)
		{
			m_pPlayContext[i] = shared_ptr<PlayerContext>(new PlayerContext (-1, -1, NULL, 1));
			m_pPlayContext[i]->hUser = hUser;
			m_pPlayContext[i]->hStream = -1;
			m_pPlayContext[i]->hPlayer = NULL;
		}
		else
		{
			m_pPlayContext[i].reset();
			ListBox_InsertString(g_hListMessage,0, "连接相机失败");
		}
	}
	EnableDlgItem(hDlg, IDC_BUTTON_DISCONNECT, true);
	EnableDlgItem(hDlg, IDC_BUTTON_CONNECT, false);
	EnableDlgItem(hDlg, IDC_EDIT_ACCOUNT, false);
	EnableDlgItem(hDlg, IDC_EDIT_PASSWORD, false);
	EnableDlgItem(hDlg, IDC_BUTTON_PLAYSTREAM, true);
	EnableDlgItem(hDlg, IDC_IPADDRESS, false);
}
void OnButtonDisConnect(HWND hDlg)
{
	for (int i = 0; i < g_vIPCamera.size(); i++)
		if (m_pPlayContext[i])
			m_pPlayContext[i].reset();
	EnableDlgItem(hDlg, IDC_BUTTON_DISCONNECT, false);
	EnableDlgItem(hDlg, IDC_BUTTON_PLAYSTREAM, false);
	EnableDlgItem(hDlg, IDC_BUTTON_CONNECT, true);
	EnableDlgItem(hDlg, IDC_EDIT_ACCOUNT, true);
	EnableDlgItem(hDlg, IDC_EDIT_PASSWORD, true);
	EnableDlgItem(hDlg, IDC_IPADDRESS, true);
	EnableDlgItem(hDlg, IDC_COMBO_STREAM, true);
	DVO2_NET_Release();
}

void OnButtonSwitchStream(HWND hDlg)
{
//KillTimer(hDlg, ID_TIMER_CHECKSTREAM);
	g_nSnapIndex = 0;
	if (m_pPlayContext[g_nSnapIndex] && m_pPlayContext[g_nSnapIndex]->hPlayer)
	{
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SNAPSHOT), FALSE);
		char szText[128] = { 0 };
		int nCount = ListBox_GetCount(g_hListMessage);
		if (nCount > 1024)
		{
			for (int i = 0; i < nCount; i++)
				ListBox_DeleteString(g_hListMessage, 0);
		}

		sprintf(szText, "正在进行第[%d]路截图……", g_nSnapIndex);
		ListBox_InsertString(g_hListMessage, 0, szText);

		int nError = 0;
		int nStream = SendDlgItemMessage(hDlg, IDC_COMBO_STREAM, CB_GETCURSEL, 0, 0);
		if (nStream == 1)	// 副码流切到主码流
			nStream = 0;
		else				// 主码流切换到副码流	
			nStream = 1;
		if (!ReOpenStream(g_nSnapIndex, nStream, hDlg))
		{
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SNAPSHOT), TRUE);
			return ;
		}
		//Sleep(2000);
		//EventDelay(NULL, 2000);
		if (SnapShot(g_nSnapIndex, hDlg))
		{
			g_nSnapShotedCount++;
			SetDlgItemInt(hDlg, IDC_EDIT_SNAPSHOTED_COUNT, g_nSnapShotedCount, FALSE);
		}
		if (nStream == 1)	// 副码流切到主码流
			nStream = 0;
		else				// 主码流切换到副码流	
			nStream = 1;
		//Sleep(100);
		ReOpenStream(g_nSnapIndex, nStream, hDlg);
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SNAPSHOT), TRUE);
		g_nSnapIndex++;
		g_nSnapIndex = g_nSnapIndex%g_vIPCamera.size();
		//SetTimer(hDlg, ID_TIMER_CHECKSTREAM, 100, NULL);
	}
}

void OnButtonPlayStream(HWND hDlg)
{
	char szText[128] = { 0 };
	int nStream = SendDlgItemMessage(hDlg,IDC_COMBO_STREAM, CB_GETCURSEL,0,0);
	for (int i = 0; i < g_vIPCamera.size();i ++)
		if (m_pPlayContext[i])
		{
			DVO_MEDIAINFO MediaHeader;
			bool bEnableWnd = false;
			if (m_pPlayContext[i]->hStream == -1)
			{
				int nError = 0;
				REAL_HANDLE hStreamHandle = DVO2_NET_StartRealPlay(m_pPlayContext[i]->hUser,
					0,
					nStream,
					DVO_TCP,
					0,
					NULL,
					(fnDVOCallback_RealAVData_T)StreamCallBack,
					(void *)m_pPlayContext[i].get(),
					&nError);
				if (hStreamHandle == -1)
				{
					sprintf(szText, "连接码流[%d]失败,Error = %d.", i,nError);
					ListBox_InsertString(g_hListMessage,0, szText);
					continue;
				}
				m_pPlayContext[i]->hStream = hStreamHandle;
				EnableDlgItem(hDlg,IDC_COMBO_STREAM, false);

				if (!m_pPlayContext[i]->hPlayer)
				{
					if (!m_pPlayContext[i]->GetStreamInfo(nStream, &MediaHeader,nError))
					{
						sprintf(szText, "获取码流信息失败[%d],Error = %d.", i,nError);
						ListBox_InsertString(g_hListMessage,0, szText);
						continue;
					}
					if (IsDlgButtonChecked(hDlg, IDC_RADIO_PLAYSTREAM) == BST_CHECKED)
					{// 播放并解码
						HWND hVideo = GetDlgItem(hDlg, (IDC_VIDEO_FRAME1 + i));
						sprintf(szText, "dvoipcplaysdk_%s", g_vIPCamera[i].c_str());
						m_pPlayContext[i]->hPlayer = dvoplay_OpenStream(hVideo, (byte *)&MediaHeader, sizeof(MediaHeader), 128, szText);
						if (!m_pPlayContext[i]->hPlayer)
						{
							sprintf(szText, "打开流播放句柄失败[%d].", i);
							ListBox_InsertString(g_hListMessage,0, szText);
							continue;
						}
					}
					if (IsDlgButtonChecked(hDlg, IDC_RADIO_DECODE) == BST_CHECKED)
					{// 仅解码
						m_pPlayContext[i]->hPlayer = dvoplay_OpenStream(NULL, (byte *)&MediaHeader, sizeof(MediaHeader), 128, szText);
						if (!m_pPlayContext[i]->hPlayer)
						{
							sprintf(szText, "打开流播放句柄失败[%d].", i);
							ListBox_InsertString(g_hListMessage,0, szText);
							continue;
						}
					}
					//SetTimer(hDlg, ID_TIMER_CHECKSTREAM, 100,NULL);
					dvoplay_Refresh(m_pPlayContext[i]->hPlayer);
					if (IsDlgButtonChecked(hDlg,IDC_CHECK_SAVEYUV) == BST_CHECKED)
					{
						SYSTEMTIME sysTime;
						GetLocalTime(&sysTime);
						char szYUVFile[512] = { 0 };
						sprintf(szYUVFile, "YVU_%s_%04d%02d%02d_%02d%02d%02d.YUV",
							g_vIPCamera[i].c_str(),
							sysTime.wYear,
							sysTime.wMonth,
							sysTime.wDay,
							sysTime.wHour,
							sysTime.wMinute,
							sysTime.wSecond);
						m_pPlayContext[i]->hYUVFile = CreateFileA(szYUVFile,
							GENERIC_WRITE,
							0,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_ARCHIVE,
							NULL);
						if (m_pPlayContext[i]->hYUVFile != INVALID_HANDLE_VALUE)
						{
							sprintf(szText, "YVU数据将被保存到文件%s中.", szYUVFile);
							ListBox_InsertString(g_hListMessage,0, szText);
						}
						else
						{
							sprintf(szText, "打开文件%s失败，YVU数据将无法保存.", szYUVFile);
							ListBox_InsertString(g_hListMessage,0, szText);
						}
						dvoplay_SetCallBack(m_pPlayContext[i]->hPlayer, YUVCapture, _CaptureYUV, m_pPlayContext[i].get());
					}
					dvoplay_Start(m_pPlayContext[i]->hPlayer, false, true);
				}
				m_pPlayContext[i]->dwLastStreamTime = timeGetTime();
			}
		}
		EnableDlgItem(hDlg, IDC_BUTTON_SNAPSHOT, TRUE);
		EnableDlgItem(hDlg, IDC_BUTTON_SWITCHSNAPSHOT, TRUE);
		EnableDlgItem(hDlg, IDC_BUTTON_PLAYSTREAM, FALSE);
		EnableDlgItem(hDlg, IDC_BUTTON_STOPPLAY, TRUE);
}

void OnButtonStopPlay(HWND hDlg)
{
	KillTimer(hDlg, ID_TIMER_CHECKSTREAM);
	KillTimer(hDlg, ID_TIMER_SNAPSHOT);
	for (int i = 0; i < g_vIPCamera.size(); i++)
		if (m_pPlayContext[i])
		{
			DVO2_NET_StopRealPlay(m_pPlayContext[i]->hStream);
			m_pPlayContext[i]->hStream = -1;
			EnableDlgItem(hDlg, IDC_COMBO_STREAM, TRUE);
			if (m_pPlayContext[i]->hPlayer)
			{
				dvoplay_Stop(m_pPlayContext[i]->hPlayer);
				//dvoplay_Refresh(m_pPlayContext[i]->hPlayer);
				dvoplay_Close(m_pPlayContext[i]->hPlayer);
				if (m_pPlayContext[i]->hYUVFile)
				{
					CloseHandle(m_pPlayContext[i]->hYUVFile);
					m_pPlayContext[i]->hYUVFile = NULL;
				}
				m_pPlayContext[i]->hPlayer = NULL;
			}
		}
		EnableDlgItem(hDlg, IDC_BUTTON_SNAPSHOT, FALSE);
		EnableDlgItem(hDlg, IDC_BUTTON_SWITCHSNAPSHOT, FALSE);
		EnableDlgItem(hDlg, IDC_BUTTON_PLAYSTREAM, TRUE);
		EnableDlgItem(hDlg, IDC_BUTTON_STOPPLAY, FALSE);
}
void OnButtonSwitchSnapshot(HWND hDlg)
{
	if (m_pPlayContext[0] && m_pPlayContext[0]->hPlayer)
	{
		BOOL bTranslated;
		g_nSnapIndex = 0;
		g_nSnapShotedCount = 0;
		g_nSnapShotCount = GetDlgItemInt(hDlg, IDC_EDIT_SNAPSHOT_COUNT, &bTranslated, TRUE);
		int nInterval = GetDlgItemInt(hDlg, IDC_EDIT_SNAPSHOT_INTERVAL, &bTranslated, FALSE);
		SetTimer(hDlg, ID_TIMER_SNAPSHOT, nInterval * 1000,NULL);
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SWITCHSNAPSHOT), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_STOPSNAPSHOT), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SWITCSTREAM), FALSE);

	}
}

void OnButtonStopSnapshot(HWND hDlg)
{
	KillTimer(hDlg, ID_TIMER_SNAPSHOT);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SNAPSHOT), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SWITCHSNAPSHOT), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_STOPSNAPSHOT), FALSE);
}

void OnButtonPlayFile(HWND hDlg)
{
	ShowWindow(hDlg,SW_HIDE);
	int nResult = DialogBox(hInst, MAKEINTRESOURCE(IDD_FILEPLAY), NULL, (DLGPROC)FilePlayDialogProc);
	ShowWindow(hDlg,SW_SHOW);
}
LRESULT OnCommand(HWND hDlg, UINT nID, HWND hWndCtrl, UINT nCodeNotify)
{
	switch (nID)
	{
	case IDOK:
		EndDialog(hDlg, IDOK);
		break;
	case IDC_BUTTON_ADDLIST:
		{
			OnButtonAddList(hDlg);
			break;
		}
	case IDC_BUTTON_CONNECT:
		{
			OnButtonConnect(hDlg);
			break;
		}
	case IDC_BUTTON_DISCONNECT:
		{
			OnButtonDisConnect(hDlg);
			break;
		}
	case IDC_BUTTON_SWITCSTREAM:
		{
			OnButtonSwitchStream(hDlg);
			break;
		}
	case IDC_BUTTON_PLAYSTREAM:
		{
			OnButtonPlayStream(hDlg);
			break;
		}
	case IDC_BUTTON_STOPPLAY:
		{
			OnButtonStopPlay(hDlg);
			break;
		}
	case IDC_BUTTON_SNAPSHOT:
		{
			SnapShot(0,hDlg);
			break;
		}
	case IDC_BUTTON_SWITCHSNAPSHOT:
		{
			OnButtonSwitchSnapshot(hDlg);
			break;
		}
	case IDC_BUTTON_STOPSNAPSHOT:
		{
			OnButtonStopSnapshot(hDlg);
			break;
		}
	case IDC_BUTTON_FILEPLAY:
		{
			OnButtonPlayFile(hDlg);
			break;
		}
	}
	return 0;
}

UINT OnTimer(HWND hDlg,UINT nEvent)
{
	switch (nEvent)
	{
	case ID_TIMER_CHECKSTREAM:
		{
			if (m_pPlayContext[0] && m_pPlayContext[0]->hPlayer)
			{
				int nError = 0;
				int nStream = SendDlgItemMessage(hDlg, IDC_COMBO_STREAM, CB_GETCURSEL, 0, 0);
				DVO2_NET_StopRealPlay(m_pPlayContext[0]->hStream);
				// 重新打开码流
				m_pPlayContext[0]->hStream = DVO2_NET_StartRealPlay(m_pPlayContext[0]->hUser,
					0,
					nStream,
					DVO_TCP,
					0,
					NULL,
					(fnDVOCallback_RealAVData_T)StreamCallBack,
					(void *)m_pPlayContext[0].get(),
					&nError);
				if (m_pPlayContext[0]->hStream == -1)
				{
					ListBox_InsertString(g_hListMessage,0, _T("连接码流失败"));
					return 0;
				}
				m_pPlayContext[0]->dwLastStreamTime = timeGetTime();
				m_pPlayContext[0]->nReOpenStream++;
				TraceMsgA("Try go Reopen IPC Video Stream [%d].\n", m_pPlayContext[0]->nReOpenStream);
			}
		}
		break;
	case ID_TIMER_SNAPSHOT:
		{
			if (m_pPlayContext[g_nSnapIndex] && m_pPlayContext[g_nSnapIndex]->hPlayer)
			{
				if (g_nSnapShotCount != -1 &&
					g_nSnapShotedCount >= g_nSnapShotCount)
				{
					EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SWITCHSNAPSHOT),TRUE);
					return 0;
				}
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SNAPSHOT), FALSE);
				char szText[128] = { 0 };
				int nCount = ListBox_GetCount(g_hListMessage);
				if (nCount > 1024)
				{
					for (int i = 0; i < nCount;i ++)
						ListBox_DeleteString(g_hListMessage,0);
				}

				sprintf(szText, "正在进行第[%d]路截图……", g_nSnapIndex);
				ListBox_InsertString(g_hListMessage,0, szText);

				int nError = 0;
				int nStream = SendDlgItemMessage(hDlg, IDC_COMBO_STREAM, CB_GETCURSEL, 0, 0);
				if (nStream == 1)	// 副码流切到主码流
					nStream = 0;
				else				// 主码流切换到副码流	
					nStream = 1;
				if (!ReOpenStream(g_nSnapIndex,nStream, hDlg))
				{
					EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SNAPSHOT), TRUE);
					return 0;
				}
				//Sleep(2000);
				//EventDelay(NULL, 2000);
				if (SnapShot(g_nSnapIndex,hDlg))
				{
					g_nSnapShotedCount++;
					SetDlgItemInt(hDlg, IDC_EDIT_SNAPSHOTED_COUNT, g_nSnapShotedCount, FALSE);
				}
				if (nStream == 1)	// 副码流切到主码流
					nStream = 0;
				else				// 主码流切换到副码流	
					nStream = 1;
				//Sleep(100);
				ReOpenStream(g_nSnapIndex,nStream, hDlg);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SNAPSHOT), TRUE);
				g_nSnapIndex++;
				g_nSnapIndex = g_nSnapIndex%g_vIPCamera.size();
			}
		}
	}
	return 0;
}
BOOL CALLBACK DialogProc(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{
//	TraceMsgA("%s message = %04X.\n",__FUNCTION__,message);
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hwnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
	}
	
	return 0;
}
BOOL OnInitDialogFilePlay(HWND hDlg, HWND hWndFocus, LPARAM lParam)
{
	return 0;
}

BOOL CALLBACK FilePlayDialogProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, OnInitDialogFilePlay);
		//HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hDlg, WM_CLOSE, OnClose);
		HANDLE_MSG(hDlg, WM_DESTROY, OnDestroy);
		//HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
	}	
	return 0;
}

void  __stdcall StreamCallBack(IN USER_HANDLE  lUserID,
	IN REAL_HANDLE lStreamHandle,
	IN int         nErrorType,
	IN const char* pBuffer,
	IN int         nDataLen,
	IN void*       pUser)
{
	if (!pUser)
		return;
	PlayerContext *pContext = (PlayerContext *)pUser;
	pContext->dwLastStreamTime = timeGetTime();
	app_net_tcp_enc_stream_head_t *pStreamHeader = (app_net_tcp_enc_stream_head_t *)pBuffer;
	int nSizeofHeader = sizeof(app_net_tcp_enc_stream_head_t);
	byte *pFrameData = (byte *)(pStreamHeader)+nSizeofHeader;
	int nFrameLength = nDataLen - sizeof(app_net_tcp_enc_stream_head_t);
	time_t tSec = pStreamHeader->sec;
	time_t tFrame = tSec * 1000 * 1000 + pStreamHeader->usec;

	switch (pStreamHeader->frame_type)
	{
	case 0:
	case APP_NET_TCP_COM_DST_IDR_FRAME:
	case APP_NET_TCP_COM_DST_I_FRAME:
	{
		if (!pContext->bRecvIFrame)
		{
			pContext->nVideoFrameID = 1;
			pContext->nAudioFrameID = 1;
			pContext->bRecvIFrame = true;
		}
		else
			pContext->nVideoFrameID++;
		int nFrameTimeSpan = (tFrame - pContext->tLastFrame) / 1000;
		pContext->tLastFrame = tFrame;
		if (nFrameTimeSpan)
			pContext->nFPS = 1000 / nFrameTimeSpan;
		break;
	}
	case APP_NET_TCP_COM_DST_P_FRAME:
	case APP_NET_TCP_COM_DST_B_FRAME:
	{
		// 这里只对视频帧计算帧ID
		if (pContext->bRecvIFrame)
			pContext->nVideoFrameID++;
		int nFrameTimeSpan = (tFrame - pContext->tLastFrame) / 1000;
		pContext->tLastFrame = tFrame;
		if (nFrameTimeSpan)
			pContext->nFPS = 1000 / nFrameTimeSpan;
		break;
	}
	case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A律编码帧
	case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U律编码帧
	case APP_NET_TCP_COM_DST_726:           // 726编码帧
	case APP_NET_TCP_COM_DST_AAC:           // AAC编码帧
		if (pContext->bRecvIFrame)
			pContext->nAudioFrameID++;
		pContext->nAudioCodec = pStreamHeader->frame_type;
		break;
	default:
		//TraceMsgA("%s Audio Frame Length = %d.\n", __FUNCTION__, nFrameLength);
		break;
	}
	
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
	int i = 0;
	
	//for (int i = 0; i < pContext->nPlayerCount; i++)
	if (pContext->hPlayer)
		dvoplay_InputIPCStream(pContext->hPlayer, pFrameData, pStreamHeader->frame_type, nFrameLength, pStreamHeader->frame_num, 0);
}

/// @brief		解码后YVU数据回调
void __stdcall _CaptureYUV(DVO_PLAYHANDLE hPlayHandle,
	const unsigned char* pYUV,
	int nSize,
	int nWidth,
	int nHeight,
	INT64 nTime,
	void *pUserPtr)
{
	PlayerContext *pContext = (PlayerContext *)pUserPtr;
	if (pContext->hYUVFile)
	{
		DWORD dwBytesWrite = 0;
		if (!WriteFile(pContext->hYUVFile, pYUV, nSize, &dwBytesWrite, NULL))
		{
			ListBox_InsertString(g_hListMessage,0, "写入YVU数据失败");
		}
		CloseHandle(pContext->hYUVFile);
		pContext->hYUVFile = NULL;
	}
}