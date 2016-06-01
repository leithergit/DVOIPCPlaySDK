// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "DvoPlayer.h"
#ifdef Release_D
#undef assert
#define assert	((void)0)
#endif

HANDLE g_hThread_ClosePlayer = nullptr;
HANDLE g_hEventThreadExit = nullptr;
volatile bool g_bThread_ClosePlayer/* = false*/;
list<DVO_PLAYHANDLE > g_listPlayertoFree;
list<DVO_PLAYHANDLE> g_listPlayer;
CRITICAL_SECTION  g_csListPlayertoFree;
list<DxSurfaceWrap *>g_listDxCache;
CRITICAL_SECTION  g_csListDxCache;

UINT	g_nMaxCacheTime = 30;		//CDxSurface对象在g_listDxCache中留存的最大时间,默认为30秒,超过这个时间时,对应的CDxSurface对象将被删除
#ifdef _DEBUG
CRITICAL_SECTION g_csPlayerHandles;
UINT	g_nPlayerHandles = 0;
#endif

double	g_dfProcessLoadTime = 0.0f;
HWND	g_hSnapShotWnd = nullptr;
CDxSurface* GetDxInCache(int nWidth, int nHeight, D3DFORMAT nD3DFormat /*= (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2')*/)
{
	CAutoLock lock(&g_csListDxCache);
	for (auto it = g_listDxCache.begin(); it != g_listDxCache.end();)
	{
		if ((*it)->CompareSizeAndFormat(nWidth, nHeight, nD3DFormat))
		{
			CDxSurface *pDxSurface = (*it)->Strip();		// 剥离DxSurface对象
#ifdef _DEBUG
			TraceMsgA("%s DxSurface Obj %p Striped.\n",__FUNCTION__, pDxSurface);
			pDxSurface->OutputDxPtr();
#endif
			delete (*it);
			it = g_listDxCache.erase(it);
			return pDxSurface;
		}
		else
			it++;
	}
	return nullptr;
}
void PutDxCache(CDxSurface *pDxSurface)
{
	TraceMsgA("%s DxSurface Obj %p input.\n",__FUNCTION__, pDxSurface);
	EnterCriticalSection(&g_csListDxCache);
	DxSurfaceWrap* pDxSurfaceWrap =  new DxSurfaceWrap(pDxSurface);
	g_listDxCache.push_back(pDxSurfaceWrap);
	LeaveCriticalSection(&g_csListDxCache);
}

void RemoveTimeoutDxSurface()
{
	CAutoLock lock(&g_csListDxCache);
	for (auto it = g_listDxCache.begin(); it != g_listDxCache.end();)
	{
		if (TimeSpanEx((*it)->dfInput) > g_nMaxCacheTime)
		{
#ifdef _DEBUG
			TraceMsgA("%s DxSurface Obj %p is Timeout.\n", __FUNCTION__, (*it)->pDxSurface);
			(*it)->pDxSurface->OutputDxPtr();
#endif
			delete (*it);
			it = g_listDxCache.erase(it);
			return;
		}
		else
			it++;
	}
}


void RemoveAllDxSurface()
{
	CAutoLock lock(&g_csListDxCache);
	for (auto it = g_listDxCache.begin(); it != g_listDxCache.end();)
	{
#ifdef _DEBUG
		TraceMsgA("%s DxSurface Obj %p is Timeout.\n", __FUNCTION__, (*it)->pDxSurface);
		(*it)->pDxSurface->OutputDxPtr();
#endif
		it = g_listDxCache.erase(it);
	}
}

DWORD WINAPI Thread_Helper(void *);
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		g_dfProcessLoadTime = GetExactTime();
		//TraceMsgA("%s DvoIPCPlaySDK is loaded.\r\n", __FUNCTION__);
		InitializeCriticalSection(&g_csListDxCache);
		InitializeCriticalSection(&g_csListPlayertoFree);
#ifdef _DEBUG
		InitializeCriticalSection(&g_csPlayerHandles);
		g_nPlayerHandles = 0;
#endif
		//CDvoPlayer::CheckSnapshotWnd(g_hSnapShotWnd);
		g_bThread_ClosePlayer = true;
		g_hEventThreadExit = CreateEvent(nullptr, true, false, nullptr);
		g_hThread_ClosePlayer = CreateThread(nullptr, 0, Thread_Helper, nullptr, 0, 0);
	}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
	{
// 		HWND hSnapWnd = FindWindow(nullptr, _T("DVO SnapShot"));
// 		if (hSnapWnd)
// 			PostMessage(hSnapWnd, WM_QUIT,0,0);
		
		g_bThread_ClosePlayer = false;
		DWORD dwExitCode = 0;
		GetExitCodeThread(g_hThread_ClosePlayer, &dwExitCode);
		if (dwExitCode == STILL_ACTIVE)
			if (WaitForSingleObject(g_hEventThreadExit, 2000) == WAIT_TIMEOUT)
			{
				TraceMsgA("%s %d(%s) WaitForSingleObject Timeout.\n)", __FILE__, __LINE__, __FUNCTION__);
			}
		Sleep(10);
		GetExitCodeThread(g_hThread_ClosePlayer, &dwExitCode);
		if (dwExitCode != STILL_ACTIVE)
			TerminateThread(g_hThread_ClosePlayer, 0);
		CloseHandle(g_hThread_ClosePlayer);
		DeleteCriticalSection(&g_csListPlayertoFree);
#if _DEBUG
		DeleteCriticalSection(&g_csPlayerHandles);
#endif
		DeleteCriticalSection(&g_csListDxCache);
		//TraceMsgA("%s DvoIPCPlaySDK is Unloaded.\r\n", __FUNCTION__);
		CloseHandle(g_hEventThreadExit);
		g_hEventThreadExit = nullptr;
	}
		break;
	}
	return TRUE;
}

void PreventScreenSave()
{
	INPUT input;
	memset(&input, 0, sizeof(INPUT));
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(INPUT));
}
DWORD WINAPI Thread_Helper(void *)
{
//#ifdef _DEBUG
	DWORD dwTime = timeGetTime();
	DWORD dwLastPreventTime = timeGetTime();
//#endif
	while (g_bThread_ClosePlayer)
	{
		if ((timeGetTime() - dwLastPreventTime) > 30*1000)
		{
			PreventScreenSave();
			dwLastPreventTime = timeGetTime();
		}
		RemoveTimeoutDxSurface();
		EnterCriticalSection(&g_csListPlayertoFree);
		if (g_listPlayertoFree.size() > 0)
			g_listPlayer.swap(g_listPlayertoFree);
		LeaveCriticalSection(&g_csListPlayertoFree);
		if (g_listPlayer.size() > 0)
		{
			for (auto it = g_listPlayer.begin(); it != g_listPlayer.end();)
			{
				CDvoPlayer *pDvoPlayer = (CDvoPlayer *)(*it);
				if (pDvoPlayer->StopPlay(pDvoPlayer->GetD3DCache()))
				{
					delete pDvoPlayer;
					it = g_listPlayer.erase(it);
				}
				else
					it++;
			}
		}
#if _DEBUG
		if ((timeGetTime() - dwTime) >= 30000)
		{
			CDvoPlayer::m_pCSGlobalCount->Lock();			
			TraceMsgA("%s Count of CDvoPlayer Object = %d.\n", __FUNCTION__, CDvoPlayer::m_nGloabalCount);
			CDvoPlayer::m_pCSGlobalCount->Unlock();
			EnterCriticalSection(&g_csPlayerHandles);
			TraceMsgA("%s Count of DVOPlayerHandle  = %d.\n", __FUNCTION__, g_nPlayerHandles);
			LeaveCriticalSection(&g_csPlayerHandles);
			dwTime = timeGetTime();
		}
#endif
 		Sleep(10);
	}
	TraceMsgA("%s Exit.\n", __FUNCTION__);
	SetEvent(g_hEventThreadExit);
	return 0;
}