#pragma once
#include <windows.h>
#include <assert.h>
/// @brief 可以自动加锁和解锁临界区的类
///
/// 适用于解锁条件复杂,不方便手动解锁临界区代码的场合
///
/// @code
///	CRITICAL_SECTION cs;
/// CAutoLock  lock(&cs,true);
/// @endcode 
///
class CAutoLock
{
private:
	CRITICAL_SECTION *m_pCS;
	bool	m_bAutoDelete;
public:
	CAutoLock():m_pCS(NULL),m_bAutoDelete(false)
	{
	}
	CAutoLock(CRITICAL_SECTION *pCS,bool bAutoDelete = false)
	{
		assert(pCS != NULL);
		if (pCS)
		{
			m_pCS = pCS;
			m_bAutoDelete = bAutoDelete;
			::EnterCriticalSection(m_pCS);	
		}			
	}
	~CAutoLock()
	{
		if (m_pCS)
		{
			::LeaveCriticalSection(m_pCS);
			if (m_bAutoDelete)
				::DeleteCriticalSection(m_pCS);
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