#include "AutoLock.h"
// 
// void MyInitializeCriticalSection(MYCRITICAL_SECTION *pCS)
// {
// 	::InitializeCriticalSection((CRITICAL_SECTION *)pCS);	
// 	ZeroMemory(pCS->szFile, sizeof(pCS->szFile));
// 	ZeroMemory(pCS->szFunction, sizeof(pCS->szFunction));
// 	pCS->nLine = 0;
// 	pCS->dwLockTime = 0;
// 	pCS->dwWaitTime = 0;
// }
// BOOL MyTryEnterCriticalSection(MYCRITICAL_SECTION *pCS, const CHAR *szFile/* = NULL*/, char *szFunction/* =  NULL*/, int nLine/* = 0*/)
// {
// 	if (!pCS)
// 		return FALSE;
// 	BOOL bRet = ::TryEnterCriticalSection((CRITICAL_SECTION *)pCS);
// 	if (!bRet)
// 		return bRet;
// 	pCS->dwLockTime = timeGetTime();
// 	if (szFile)
// 	{
// 		strcpy(pCS->szFile, szFile);
// 		pCS->nLine = nLine;
// 	}
// 	if (szFunction)
// 		strcpy(pCS->szFunction, szFunction);
// 	return bRet;
// }
// 
// void MyEnterCriticalSection(MYCRITICAL_SECTION *pCS, const CHAR *szFile/* = NULL*/,char *szFunction/* =  NULL*/, int nLine/* = 0*/)
// {
// 	if (!pCS)
// 		return ;
// #ifdef _OuputLockTime
// 	pCS->dwWaitTime = timeGetTime();
// #endif
// 	
// 	EnterCriticalSection((CRITICAL_SECTION *)pCS);
// #ifdef _OuputLockTime
// 	pCS->dwLockTime = timeGetTime();
// 	pCS->dwWaitTime = timeGetTime() - pCS->dwWaitTime;
// 	if (pCS->dwWaitTime > 100)
// 	{
// 		CHAR szOuput[1024] = { 0 };
// 		if (strlen(pCS->szFile))
// 		{
// 			sprintf(szOuput, "Wait Lock @File:%s:%d,Waittime = %d,Last Lock @File:%s:%d(%s).\n", pCS->szFile, pCS->nLine, pCS->dwWaitTime,pCS->szFunction);
// 		}
// 		else
// 			sprintf(szOuput, "Lock Waittime = %d.\n", timeGetTime() - pCS->dwLockTime);
// 		OutputDebugStringA(szOuput);
// 	}
// 	if (szFile)
// 	{
// 		strcpy(pCS->szFile, szFile);
// 		pCS->nLine = nLine;
// 	}
// 	if (szFunction)
// 		strcpy(pCS->szFunction, szFunction);
// #endif
// }
// 
// void MyLeaveCriticalSection(MYCRITICAL_SECTION *pCS, const CHAR *szFile/* = NULL*/, char *szFunction/* =  NULL*/, int nLine/* = 0*/)
// {
// 	if (!pCS)
// 		return;
// 	::LeaveCriticalSection((CRITICAL_SECTION *)pCS);
// #ifdef _OuputLockTime
// 	if ((timeGetTime() - pCS->dwLockTime) > _LockOverTime)
// 	{
// 		CHAR szOuput[1024] = { 0 };
// 		if (strlen(pCS->szFile))
// 		{
// 			sprintf(szOuput, "Lock @File:%s:%d,locktime = %d.\n", pCS->szFile, pCS->nLine, timeGetTime() - pCS->dwLockTime);
// 		}
// 		else
// 			sprintf(szOuput, "Lock locktime = %d.\n", timeGetTime() - pCS->dwLockTime);
// 		OutputDebugStringA(szOuput);
// 	}
// #endif
// 	
// }
// 
// void MyDeleteCriticalSection(MYCRITICAL_SECTION *pCS)
// {
// 	if (!pCS)
// 		return ;
// 	DeleteCriticalSection((CRITICAL_SECTION *)pCS);
// }