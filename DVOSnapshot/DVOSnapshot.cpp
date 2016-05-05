// DVOSnapshot.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <TCHAR.h>
#include <WindowsX.h>
#include <process.h>

#pragma comment ( lib, "d3d9.lib" )
#pragma comment ( lib, "d3dx9.lib" )
#include <stdio.h>
#include <wtypes.h>
#include "../DVOIPCPlaySDK/DxSurface/DxSurface.h"
#include "../DVOIPCPlaySDK/Utility.h"
#include "../DVOIPCPlaySDK/DVOIPCPlaySDK.h"
#include "DVOSnapshot.h"
using namespace std;
using namespace std::tr1;

#define ID_F12 WM_USER + 1024
CDxSurfaceEx *g_pDxSurface = nullptr;
int g_nVideoWidth = 1280;
int g_nVideoHeight = 720;

bool g_bWindowShowed = false;
HINSTANCE hInst;	
LRESULT CALLBACK DialogProc(HWND , UINT , WPARAM , LPARAM );
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
// 	hInst = hInstance;
// 	int nResult = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_SNAPSHOT), NULL, (DLGPROC)DialogProc);
	HWND   hDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG_SNAPSHOT), NULL, (DLGPROC)DialogProc);

	ShowWindow(hDialog, SW_HIDE);
	UpdateWindow(hDialog);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

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


BOOL OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
{
	// ×¢²áÈÈ¼ü
	::RegisterHotKey(hDlg, ID_F12, MOD_ALT | MOD_CONTROL, VK_F12);
	g_bWindowShowed = false;
	if (!g_pDxSurface)
	{
		g_pDxSurface = new CDxSurfaceEx();
		g_pDxSurface->InitD3D(GetDlgItem(hDlg, IDC_STATIC_FRAME), g_nVideoWidth, g_nVideoHeight, TRUE);
	}
	//ShowWindow(hDlg, SW_HIDE);
	return FALSE;
}

void OnCommand(HWND hDlg, UINT nID, HWND hWndCtrl, UINT nCodeNotify)
{
	switch (nID)
	{
	case IDOK:
	case IDCANCEL:
		break;
	default:
		break;
	}
}

void OnClose(HWND hWnd)
{
	EndDialog(hWnd, 0);
}

void OnDestroy(HWND hWnd)
{
	PostQuitMessage(0);
}

BOOL OnCopyData(HWND hWnd, HWND hwndFrom, PCOPYDATASTRUCT pCDS)
{
	YUVFrame *pYUVFrame = (YUVFrame *)pCDS->lpData;
	TraceMsgA("%s.\n", __FUNCTION__);
	if (g_pDxSurface)
	{
		if (g_pDxSurface->GetVideoSize() != MAKELONG(pYUVFrame->nWidth, pYUVFrame->nHeight))
		{
			g_pDxSurface->DxCleanup();
			g_pDxSurface->InitD3D(GetDlgItem(hWnd, IDC_STATIC_FRAME), pYUVFrame->nWidth, pYUVFrame->nHeight);
		}
		uint8_t *pYUVBuffer = (byte *)(((byte *)pYUVFrame) + sizeof(YUVFrame));
		AVFrame *pFrame = av_frame_alloc();
		pFrame->width = pYUVFrame->nWidth;
		pFrame->height = pYUVFrame->nHeight;
		pFrame->data[0] = pYUVBuffer;
		pFrame->data[1] = &pYUVBuffer[pYUVFrame->nWidth*pYUVFrame->nHeight];
		pFrame->data[2] = &pYUVBuffer[pYUVFrame->nWidth*pYUVFrame->nHeight * 5 / 4];
		pFrame->linesize[0] = pYUVFrame->nLineSize[0];
		pFrame->linesize[1] = pYUVFrame->nLineSize[1];
		pFrame->linesize[2] = pYUVFrame->nLineSize[2];
		pFrame->format = AV_PIX_FMT_YUV420P;
		g_pDxSurface->RequireSnapshot();
		g_pDxSurface->Render(pFrame);
		if (g_pDxSurface->SaveSurfaceToFileA(pYUVFrame->szFileName))
		{
			TraceMsgA("%s ½ØÍ¼³É¹¦ %S.\n", __FUNCTION__, pYUVFrame->szFileName);
		}
		else
			TraceMsgA("%s ½ØÍ¼Ê§°Ü %s.\n", __FUNCTION__);
		av_free(pFrame);
	}
	return TRUE;
}
void OnHotKey(HWND hwnd, int idHotKey, UINT fuModifiers, UINT vk)
{
	if (idHotKey == ID_F12)
	{
		if (g_bWindowShowed)
			::ShowWindow(hwnd, SW_SHOW);
		else
			::ShowWindow(hwnd, SW_HIDE);
		g_bWindowShowed = !g_bWindowShowed;
	}
}

void OnSysCommand(HWND hwnd, UINT nID, int x, int y)
{
	if (nID == SC_MINIMIZE)
	{
		ShowWindow(hwnd,SW_HIDE);
		g_bWindowShowed = true;
	}
}

void OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
{
// 	ShowWindow(hwnd, SW_HIDE);
// 	g_bWindowShowed = true;
}


LRESULT CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hwnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hwnd, WM_COPYDATA, OnCopyData);
		HANDLE_MSG(hwnd, WM_HOTKEY, OnHotKey);
		HANDLE_MSG(hwnd, WM_SYSCOMMAND, OnSysCommand);
		HANDLE_MSG(hwnd, WM_ACTIVATE, OnActivate);

	}
	return 0;
}

