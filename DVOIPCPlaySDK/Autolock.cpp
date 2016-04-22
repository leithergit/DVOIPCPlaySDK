#include "AutoLock.h"

void MyInitializeCriticalSection(MYCRITICAL_SECTION *pCS)
{
	::InitializeCriticalSection((CRITICAL_SECTION *)pCS);	
	
}
BOOL MyTryEnterCriticalSection(MYCRITICAL_SECTION *pCS, const CHAR *szFile/* = nullptr*/, char *szFunction/* =  nullptr*/, int nLine/* = 0*/)
{
	if (!pCS)
		return FALSE;
	BOOL bRet = ::TryEnterCriticalSection((CRITICAL_SECTION *)pCS);
	if (!bRet)
		return bRet;
	EnterCriticalSection(&pCS->cs);
	pCS->dwLockTime = timeGetTime();
	if (szFile)
	{
		strcpy(pCS->szFile, szFile);
		pCS->nLine = nLine;
	}
	if (szFunction)
		strcpy(pCS->szFunction, szFunction);
	LeaveCriticalSection(&pCS->cs);
	return bRet;
}

void MyEnterCriticalSection(MYCRITICAL_SECTION *pCS, const CHAR *szFile/* = nullptr*/,char *szFunction/* =  nullptr*/, int nLine/* = 0*/)
{
	if (!pCS)
		return ;
#ifdef _OuputLockTime
	DWORD dwWaitTime = timeGetTime();
#endif
	
	EnterCriticalSection((CRITICAL_SECTION *)pCS);
#ifdef _OuputLockTime
	pCS->dwLockTime = timeGetTime();
	dwWaitTime = timeGetTime() - dwWaitTime;
	if (dwWaitTime > 100)
	{
		CHAR szOuput[1024] = { 0 };
		if (szFile && 
			szFunction && 
			strlen(pCS->szFile))
		{
			sprintf(szOuput, "Wait Lock @File:%s:%d(%s),Waittime = %d,Last Lock @File:%s:%d(%s).\n", szFile,nLine,szFunction,dwWaitTime,pCS->szFile, pCS->nLine, pCS->szFunction);
		}
		else
			sprintf(szOuput, "Lock Waittime = %d.\n", timeGetTime() - pCS->dwLockTime);
		OutputDebugStringA(szOuput);
	}
	EnterCriticalSection(&pCS->cs);
	if (szFile)
	{
		strcpy(pCS->szFile, szFile);
		pCS->nLine = nLine;
	}
	if (szFunction)
		strcpy(pCS->szFunction, szFunction);
	LeaveCriticalSection(&pCS->cs);
#endif
}

void MyLeaveCriticalSection(MYCRITICAL_SECTION *pCS)
{
	if (!pCS)
		return;
	::LeaveCriticalSection((CRITICAL_SECTION *)pCS);
#ifdef _OuputLockTime
	if ((timeGetTime() - pCS->dwLockTime) > _LockOverTime)
	{
		CHAR szOuput[1024] = { 0 };
		if (strlen(pCS->szFile))
		{
			sprintf(szOuput, "Lock @File:%s:%d(%s),locktime = %d.\n", pCS->szFile, pCS->nLine,pCS->szFunction, timeGetTime() - pCS->dwLockTime);
		}
		else
			sprintf(szOuput, "Lock locktime = %d.\n", timeGetTime() - pCS->dwLockTime);
		OutputDebugStringA(szOuput);
	}
#endif
	
}

void MyDeleteCriticalSection(MYCRITICAL_SECTION *pCS)
{
	if (!pCS)
		return ;
	DeleteCriticalSection((CRITICAL_SECTION *)pCS);
}