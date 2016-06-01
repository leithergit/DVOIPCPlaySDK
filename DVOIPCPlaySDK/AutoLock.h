#pragma once

#include <assert.h>
#include <Windows.h>
#include <stdio.h>

#ifdef Release_D
#undef assert
#define assert	((void)0)
#endif
/// @brief 可以自动加锁和解锁临界区的类
///
/// 适用于解锁条件复杂,不方便手动解锁临界区代码的场合
///
/// @code
///	CRITICAL_SECTION cs;
/// CAutoLock  lock(&cs,true);
/// @endcode 
///
#define _OuputLockTime
#define _LockOverTime	100
struct MYCRITICAL_SECTION :public CRITICAL_SECTION
{
	MYCRITICAL_SECTION()
	{
		ZeroMemory(this, sizeof(MYCRITICAL_SECTION));
		InitializeCriticalSection(&cs);
	}
	~MYCRITICAL_SECTION()
	{
		DeleteCriticalSection(&cs);
	}
	DWORD	dwLockTime;
	int		nLine;
	CHAR	szFile[1024];
	char	szFunction[256];
	CRITICAL_SECTION cs;
};
void MyInitializeCriticalSection(MYCRITICAL_SECTION *);
BOOL MyTryEnterCriticalSection(MYCRITICAL_SECTION *, const CHAR *szFile = nullptr,char *szFunction =  nullptr, int nLine = 0);
void MyEnterCriticalSection(MYCRITICAL_SECTION *, const CHAR *szFile = nullptr, char *szFunction = nullptr, int nLine = 0);
void MyLeaveCriticalSection(MYCRITICAL_SECTION *);
void MyDeleteCriticalSection(MYCRITICAL_SECTION *);

// #define _MyTryEnterCriticalSection(cs)	MyTryEnterCriticalSection(cs,__FILE__,__FUNCTION__,__LINE__)
// #define _MyEnterCriticalSection(cs)		MyEnterCriticalSection(cs,__FILE__,__FUNCTION__,__LINE__)
// #define _MyLeaveCriticalSection			MyLeaveCriticalSection

#define _MyTryEnterCriticalSection		TryEnterCriticalSection
#define _MyEnterCriticalSection			EnterCriticalSection
#define _MyLeaveCriticalSection			LeaveCriticalSection

#define Autolock(cs)	CAutoLock lock(cs,false,__FILE__,__FUNCTION__,__LINE__);
class CAutoLock
{
private:
	CRITICAL_SECTION *m_pCS;
	bool	m_bAutoDelete;
	bool	m_bLocked;
#ifdef _LockOverTime
	DWORD	m_dwLockTime;
	CHAR   *m_pszFile;
	char   *m_pszFunction;
	int		m_nLockLine;
#endif

	explicit CAutoLock():m_pCS(NULL),m_bAutoDelete(false)
	{
	}
public:
	CAutoLock(CRITICAL_SECTION *pCS, bool bAutoDelete = false, const CHAR *szFile = nullptr, char *szFunction = nullptr, int nLine = 0)
	{
		ZeroMemory(this, sizeof(CAutoLock));
		assert(pCS != NULL);
		if (pCS)
		{
#ifdef _LockOverTime
			m_dwLockTime = timeGetTime();
			if (szFile)
			{
				m_pszFile = new CHAR[strlen(szFile) + 1];
				strcpy(m_pszFile, szFile);
				m_pszFunction = new char[strlen(szFunction) + 1];
				strcpy(m_pszFunction, szFunction);
				m_nLockLine = nLine;
			}
#endif
			m_pCS = pCS;
			m_bAutoDelete = bAutoDelete;
			::EnterCriticalSection(m_pCS);
			m_bLocked = true;
			if (timeGetTime() - m_dwLockTime >= _LockOverTime)
			{
				CHAR szOuput[1024] = { 0 };
				if (szFile)
				{
					sprintf(szOuput, "Wait Lock @File:%s:%d(%s),Waittime = %d.\n", szFile, nLine, szFunction,timeGetTime() - m_dwLockTime);
				}
				else
					sprintf(szOuput, "Wait Lock Waittime = %d.\n", timeGetTime() - m_dwLockTime);
				OutputDebugStringA(szOuput);
			}
		}
	}
	void Unlock()
	{
		if (!m_bLocked)
			return;
		if (m_pCS)
		{
			::LeaveCriticalSection(m_pCS);
			m_bLocked = false;
#ifdef _OuputLockTime
			CHAR szOuput[1024] = { 0 };
			if ((timeGetTime() - m_dwLockTime) > _LockOverTime)
			{
				if (m_pszFile)
				{
					sprintf(szOuput, "Lock @File:%s:%d(%s),locktime = %d.\n", m_pszFile, m_nLockLine, m_pszFunction, timeGetTime() - m_dwLockTime);
				}
				else
					sprintf(szOuput, "Lock locktime = %d.\n", timeGetTime() - m_dwLockTime);
				OutputDebugStringA(szOuput);
			}
			if (m_pszFile)
				delete[]m_pszFile;
			if (m_pszFunction)
				delete[]m_pszFunction;
#endif
			if (m_bAutoDelete)
				::DeleteCriticalSection((CRITICAL_SECTION *)m_pCS);
		}
	}
	~CAutoLock()
	{
		if (m_bLocked)
			Unlock();
	}
};

class CTryLock
{
private:
	CRITICAL_SECTION *m_pCS;
	bool	m_bAutoDelete;
	BOOL	m_bLocked;
public:
	CTryLock():m_pCS(NULL),m_bAutoDelete(false),m_bLocked(false)
	{
	}

	BOOL TryLock(CRITICAL_SECTION *pCS,bool bAutoDelete = false)
	{
		assert(pCS != NULL);
		if (pCS)
		{
			m_pCS = pCS;
			m_bAutoDelete = bAutoDelete;
			return (m_bLocked = ::TryEnterCriticalSection(m_pCS));
		}
		return false;
	}
	~CTryLock()
	{
		if (m_pCS)
		{
			if (m_bLocked)
				::LeaveCriticalSection(m_pCS);
			if (m_bAutoDelete)
				::DeleteCriticalSection(m_pCS);
		}
	}
};