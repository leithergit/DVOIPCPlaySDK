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
#include "./dump/DumpFile.h"
using namespace std;
using namespace std::tr1;

#define ID_F12 WM_USER + 1024
CDxSurfaceEx *g_pDxSurface = nullptr;
int g_nVideoWidth = 1280;
int g_nVideoHeight = 720;

bool g_bWindowShowed = false;
HINSTANCE hInst;	
HWND g_hEditText = nullptr;
bool g_bWaitForDebug = true;
void WaitForDebug()
{
	while (g_bWaitForDebug)
	{
		Sleep(10);
	}
}
LRESULT CALLBACK DialogProc(HWND , UINT , WPARAM , LPARAM );
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	//WaitForDebug();
	DeclareDumpFile();
	if (_tcscmp(lpCmdLine, _SnapShotCmdLine))
	{
		TraceMsgA("%s Invalid cmd line ,DVOSnapshot deny to run.\n", __FUNCTION__);
		return 0;
	}
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

void CloseShareMemory(HANDLE &hMemHandle, void *pMemory)
{
	if (!pMemory || !hMemHandle)
		return;
	if (pMemory)
	{
		UnmapViewOfFile(pMemory);
		pMemory = NULL;
	}
	if (hMemHandle)
	{
		CloseHandle(hMemHandle);
		hMemHandle = NULL;
	}
}


void *OpenShareMemory(HANDLE &hMemHandle,TCHAR *szUniqueName)
{
	void *pShareSection = NULL;
	bool bResult = false;
	__try
	{
		__try
		{
			if (!szUniqueName || _tcslen(szUniqueName) < 1)
				__leave;
			hMemHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, szUniqueName);
			if (hMemHandle == INVALID_HANDLE_VALUE ||
				hMemHandle == NULL)
			{
				_TraceMsg(_T("%s %d MapViewOfFile Failed,ErrorCode = %d.\r\n"), __FILEW__, __LINE__, GetLastError());
				__leave;
			}
			pShareSection = MapViewOfFile(hMemHandle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
			if (pShareSection == NULL)
			{
				_TraceMsg(_T("%s %d MapViewOfFile Failed,ErrorCode = %d.\r\n"), __FILEW__, __LINE__, GetLastError());
				__leave;
			}
			bResult = true;
			_TraceMsg(_T("Open Share memory map Succeed."));
		}
		__finally
		{
			if (!bResult)
			{
				CloseShareMemory(hMemHandle, pShareSection);
				pShareSection = NULL;
			}
		}
	}
	__except (1)
	{
		TraceMsg(_T("Exception in OpenShareMemory.\r\n"));
	}
	return pShareSection;
}

void * CreateShareMemory(HANDLE &hMemHandle, TCHAR *szUniqueName,UINT nSize)
{
	void *pMemAddr = nullptr;
// 	SECURITY_DESCRIPTOR sd;
// 	SECURITY_ATTRIBUTES sa;
	bool bResult = false;
	DWORD dwErrorCode = 0;
	__try
	{
		__try
		{
// 			if (!::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
// 			{
// 				_TraceMsg(_T("%s %d InitializeSecurityDescriptor failed,ErrorCode = %d."), __FILEW__, __LINE__, GetLastError());
// 				__leave;
// 			}
// 			if (!::SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE))
// 			{
// 				_TraceMsg(_T("%s %d SetSecurityDescriptorDacl failed,ErrorCode = %d."), __FILEW__, __LINE__, GetLastError());
// 				__leave;
// 			}
// 			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
// 			sa.bInheritHandle = FALSE;
// 			sa.lpSecurityDescriptor = &sd;
			hMemHandle = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr/*&sa*/,
				PAGE_READWRITE | SEC_COMMIT,
				0,
				nSize,
				szUniqueName);
			if (!hMemHandle)
			{
				_TraceMsg(_T("%s %d CreateFileMapping failed,ErrorCode = %d."), __FILEW__, __LINE__, GetLastError());
				__leave;
			}
			pMemAddr = MapViewOfFile(hMemHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (!pMemAddr)
			{
				_TraceMsg(_T("%s %d MapViewOfFile failed,ErrorCode = %d."), __FILEW__, __LINE__, GetLastError());
				__leave;
			}
// 			pMemoryMgr->hMutex = CreateMutex(&sa, FALSE, _MutexName);
// 			if (!pMemoryMgr->hMutex)
// 			{
// 				_TraceMsg(_T("%s %d CreateMutex failed,ErrorCode = %d."), __FILEW__, __LINE__, GetLastError());
// 				__leave;
// 			}
// 
// 			
// 			pMemoryMgr->hUpdateTipsEvent = CreateEvent(&sa, TRUE, FALSE, _UpdateTipsHandleName);
// 			if (!pMemoryMgr->hUpdateTipsEvent)
// 			{
// 				_TraceMsg(_T("%s %d CreateEvent failed,ErrorCode = %d."), __FILEW__, __LINE__, GetLastError());
// 				__leave;
// 			}			
			_TraceMsg(_T("Share memory map succeed."));
			bResult = true;
		}
		__finally
		{
			if (!bResult)
			{
				CloseShareMemory(hMemHandle, pMemAddr);
			}
		}
	}
	__except (1)
	{
		_TraceMsg(_T("%s %d Excpetion in CreateShareMemory."), __FILEW__, __LINE__);
	}
	return pMemAddr;
};

SnapShotMem *g_pSnapShotMem = nullptr;
HANDLE		 g_hSnapShotMem = nullptr;
HANDLE		 g_hSnapShotEvent = nullptr;
BOOL OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
{
	// 注册热键
	::RegisterHotKey(hDlg, ID_F12, MOD_ALT | MOD_CONTROL, VK_F12);
	g_bWindowShowed = false;
	if (!g_pDxSurface)
	{
		g_pDxSurface = new CDxSurfaceEx();
		g_pDxSurface->InitD3D(GetDlgItem(hDlg, IDC_STATIC_FRAME), g_nVideoWidth, g_nVideoHeight, TRUE);
	}
	g_hEditText = GetDlgItem(hDlg, IDC_EDIT_TEXT);
	SendDlgItemMessage(hDlg,IDC_EDIT_TEXT, EM_LIMITTEXT, 0, 0xFFFF);
	//SendDlgItemMessage(hDlg, EM_SETBKGNDCOLOR, 0, GetSysColor(COLOR_3DFACE));
	// 创建共享内存
// 	g_pSnapShotMem = (SnapShotMem *)CreateShareMemory(g_hSnapShotMem, _SnapShotMemName, sizeof(SnapShotMem));
// 	if (!g_hSnapShotMem || !g_hSnapShotMem)
// 	{
// 		TraceMsgA("%s CreateShareMemory failed.\n", __FUNCTION__);
// 		ExitProcess(0);
// 	}
// 	g_pSnapShotMem->hSnapShot = hDlg;
// 	g_hSnapShotEvent = CreateEvent(nullptr, FALSE, FALSE, _SnapShotEventName);
// 	if (!g_hSnapShotEvent)
// 	{
// 		TraceMsgA("%s Create SnapshotEvent failed.\n", __FUNCTION__);
// 		CloseShareMemory(g_hSnapShotMem, g_pSnapShotMem);
// 		ExitProcess(0);
// 	}
	
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
	DestroyWindow(hWnd);
}

void OnDestroy(HWND hWnd)
{
	PostQuitMessage(0);
}

BOOL OnCopyData(HWND hWnd, HWND hwndFrom, PCOPYDATASTRUCT pCDS)
{
	YUVFrame *pYUVFrame = (YUVFrame *)pCDS->lpData;
	TraceMsgA("%s.\n", __FUNCTION__);
	TCHAR szTime[32] = { 0 };
	TCHAR szDate[32] = { 0 };
	_tstrtime(szTime);
	_tstrdate(szDate);
	ScrollEdit(g_hEditText, _T("%s %s Recv a YUV data.\r\n"), szDate, szTime);
	
	if (g_pDxSurface)
	{
		if (g_pDxSurface->GetVideoSize() != MAKELONG(pYUVFrame->nWidth, pYUVFrame->nHeight))
		{
			ScrollEdit(g_hEditText, _T("%s %s YUV data resized to (%d,%d).\r\n"), szDate, szTime, pYUVFrame->nWidth, pYUVFrame->nHeight);
			g_pDxSurface->DxCleanup();
			g_pDxSurface->InitD3D(GetDlgItem(hWnd, IDC_STATIC_FRAME), pYUVFrame->nWidth, pYUVFrame->nHeight);
		}
		uint8_t *pYUVBuffer = (byte *)(((byte *)pYUVFrame) + sizeof(YUVFrame));
		int nPicSize = pYUVFrame->nLineSize[0] * pYUVFrame->nHeight;
		AVFrame *pFrame = av_frame_alloc();
		pFrame->width = pYUVFrame->nWidth;
		pFrame->height = pYUVFrame->nHeight;
		pFrame->data[0] = pYUVBuffer;
		pFrame->data[1] = &pYUVBuffer[nPicSize];
		pFrame->data[2] = &pYUVBuffer[nPicSize * 5 / 4];
		pFrame->linesize[0] = pYUVFrame->nLineSize[0];
		pFrame->linesize[1] = pYUVFrame->nLineSize[1];
		pFrame->linesize[2] = pYUVFrame->nLineSize[2];
		pFrame->format = AV_PIX_FMT_YUV420P;
		g_pDxSurface->RequireSnapshot();
		g_pDxSurface->Render(pFrame);
		_tstrtime(szTime);
		_tstrdate(szDate);
		if (g_pDxSurface->SaveSurfaceToFileW(pYUVFrame->szFileName, (D3DXIMAGE_FILEFORMAT)pYUVFrame->nD3DImageFormat))
			ScrollEdit(g_hEditText, _T("%s %s Snapshot succeed,File:%s.\r\n"), szDate, szTime, pYUVFrame->szFileName);
		else
			ScrollEdit(g_hEditText, _T("%s %s Snapshot Failed,File:%s.\r\n"), szDate, szTime, pYUVFrame->szFileName);
		av_free(pFrame);
	}
	return TRUE;
}
void OnHotKey(HWND hwnd, int idHotKey, UINT fuModifiers, UINT vk)
{
	if (idHotKey == ID_F12)
	{
		if (!g_bWindowShowed)
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

