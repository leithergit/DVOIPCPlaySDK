// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "DvoPlayer.h"
HANDLE g_hThread_ClosePlayer = nullptr;
HANDLE g_hEventThreadExit = nullptr;
volatile bool g_bThread_ClosePlayer/* = false*/;
list<DVO_PLAYHANDLE > g_listPlayertoFree;
list<DVO_PLAYHANDLE> g_listPlayer;
CRITICAL_SECTION  g_csListPlayertoFree;
#ifdef _DEBUG
CRITICAL_SECTION g_csPlayerHandles;
UINT	g_nPlayerHandles = 0;
#endif
DWORD WINAPI Thread_ClosePlayer(void *);
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		TraceMsgA("%s DvoIPCPlaySDK is loaded.\r\n", __FUNCTION__);
		InitializeCriticalSection(&g_csListPlayertoFree);
#ifdef _DEBUG
		InitializeCriticalSection(&g_csPlayerHandles);
		g_nPlayerHandles = 0;
#endif
		g_bThread_ClosePlayer = true;
		g_hEventThreadExit = CreateEvent(nullptr, true, false, nullptr);
		g_hThread_ClosePlayer = CreateThread(nullptr, 0, Thread_ClosePlayer, nullptr, 0, 0);
	}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	{
		TraceMsgA("%s DLL_THREAD_DETACH .\r\n", __FUNCTION__);
	}
		break;
	case DLL_PROCESS_DETACH:
	{
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
		TraceMsgA("%s DvoIPCPlaySDK is Unloaded.\r\n", __FUNCTION__);
		CloseHandle(g_hEventThreadExit);
		g_hEventThreadExit = nullptr;
	}
		break;
	}
	return TRUE;
}


DWORD WINAPI Thread_ClosePlayer(void *)
{
	
#ifdef _DEBUG
	DWORD dwTime = timeGetTime();
#endif
	while (g_bThread_ClosePlayer)
	{
		EnterCriticalSection(&g_csListPlayertoFree);
		if (g_listPlayertoFree.size() > 0)
			g_listPlayer.swap(g_listPlayertoFree);
		LeaveCriticalSection(&g_csListPlayertoFree);
		if (g_listPlayer.size() > 0)
		{
			for (auto it = g_listPlayer.begin(); it != g_listPlayer.end();)
			{
				CDvoPlayer *pDvoPlayer = (CDvoPlayer *)(*it);
				delete pDvoPlayer;
				it = g_listPlayer.erase(it);
			}
		}
#if _DEBUG
		if ((timeGetTime() - dwTime) >= 5000)
		{
			CDvoPlayer::m_pCSGlobalCount->Lock();			
			//TraceMsgA("%s Count of CDvoPlayer Object = %d.\n", __FUNCTION__, CDvoPlayer::m_nGloabalCount);
			CDvoPlayer::m_pCSGlobalCount->Unlock();
			EnterCriticalSection(&g_csPlayerHandles);
			//TraceMsgA("%s Count of DVOPlayerHandle  = %d.\n", __FUNCTION__, g_nPlayerHandles);
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