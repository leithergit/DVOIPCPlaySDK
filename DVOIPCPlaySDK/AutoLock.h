#pragma once
#include <assert.h>
#include <Windows.h>
#include <stdio.h>
/// @brief �����Զ������ͽ����ٽ�������
///
/// �����ڽ�����������,�������ֶ������ٽ�������ĳ���
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
	DWORD	dwLockTime;
	int		nLine;
	CHAR	szFile[1024];
	char	szFunction[256];
};
void MyInitializeCriticalSection(MYCRITICAL_SECTION *);
BOOL MyTryEnterCriticalSection(MYCRITICAL_SECTION *, const CHAR *szFile = nullptr,char *szFunction =  nullptr, int nLine = 0);
void MyEnterCriticalSection(MYCRITICAL_SECTION *, const CHAR *szFile = nullptr, char *szFunction = nullptr, int nLine = 0);
void MyLeaveCriticalSection(MYCRITICAL_SECTION *);
void MyDeleteCriticalSection(MYCRITICAL_SECTION *);

#define _MyTryEnterCriticalSection(cs)	MyTryEnterCriticalSection(cs,__FILE__,__FUNCTION__,__LINE__)
#define _MyEnterCriticalSection(cs)		MyEnterCriticalSection(cs,__FILE__,__FUNCTION__,__LINE__)
#define _MyLeaveCriticalSection			MyLeaveCriticalSection

class CAutoLock
{
private:
	CRITICAL_SECTION *m_pCS;
	bool	m_bAutoDelete;
	bool	m_bMyCriticalSection;
#ifdef _LockOverTime
	DWORD	m_dwLockTime;
	DWORD	m_dwWaitTime;
	CHAR   *m_pszFile;
	int		m_nLockLine;
#endif
public:
	CAutoLock():m_pCS(NULL),m_bAutoDelete(false)
	{
	}
	CAutoLock(CRITICAL_SECTION *pCS, bool bAutoDelete = false, const CHAR *szFile = nullptr, int nLine = 0)
	{
		assert(pCS != NULL);
		m_bMyCriticalSection = false;
		if (pCS)
		{
#ifdef _LockOverTime
			m_dwLockTime = timeGetTime();
			if (szFile)
			{
				m_pszFile = new CHAR[strlen(szFile) + 1];
				strcpy(m_pszFile, szFile);
				m_nLockLine = nLine;
			}
#endif
			m_pCS = pCS;
			m_bAutoDelete = bAutoDelete;
			::EnterCriticalSection(m_pCS);
		}
	}
	CAutoLock(MYCRITICAL_SECTION *pCS, bool bAutoDelete = false, const CHAR *szFile = nullptr, char *szFunction = nullptr,  int nLine = 0)
	{
		assert(pCS != NULL);
		m_bMyCriticalSection = true;
		if (pCS)
		{
			m_pCS = pCS;
			m_bAutoDelete = bAutoDelete;
			::MyEnterCriticalSection(pCS, szFile,szFunction, nLine);
		}
	}
	~CAutoLock()
	{
		if (m_pCS)
		{
			if (m_bMyCriticalSection)
			{
				::MyLeaveCriticalSection((MYCRITICAL_SECTION *)m_pCS);
				if (m_bAutoDelete)
					::MyDeleteCriticalSection((MYCRITICAL_SECTION *)m_pCS);
			}
			else
			{
				::LeaveCriticalSection(m_pCS);			
#ifdef _OuputLockTime
				CHAR szOuput[1024] = { 0 };
				if ((timeGetTime() - m_dwLockTime) > _LockOverTime)
				{
					if (m_pszFile)
					{
						sprintf(szOuput, "Lock @File:%s:%d,locktime = %d.\n", m_pszFile, m_nLockLine, timeGetTime() - m_dwLockTime);
					}
					else
						sprintf(szOuput, "Lock locktime = %d.\n", timeGetTime() - m_dwLockTime);
					OutputDebugStringA(szOuput);
				}
				if (m_pszFile)
					delete[]m_pszFile;
#endif
				if (m_bAutoDelete)
					::DeleteCriticalSection((CRITICAL_SECTION *)m_pCS);
			}
		}
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