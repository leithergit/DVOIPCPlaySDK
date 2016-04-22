#pragma once
#include <TCHAR.H>
#include <windows.h>
#include <time.h>
#include <assert.h>
#include <mmsystem.h>
#include <thread>
#include <chrono>
#include "Runlog.h"
using namespace std::this_thread;
using namespace std::chrono;
#ifdef _UNICODE
#define GetDateTime			GetDateTimeW
#define	_DateTime			DateTimeW
#define UTC2DateTimeString	UTC2DateTimeStringW
#else
#define GetDateTime			GetDateTimeA
#define	_DateTime			DateTimeA
#define UTC2DateTimeString	UTC2DateTimeStringA
#endif


int		GetDateTimeA(CHAR *szDateTime,int nSize);
int		GetDateTimeW(WCHAR *szDateTime,int nSize);

bool	IsLeapYear(UINT nYear);
UINT64	DateTimeString2UTC(TCHAR *szTime,UINT64 &nTime);
void	UTC2DateTimeStringA(UINT64 nTime,CHAR *szTime,int nSize);
void	UTC2DateTimeStringW(UINT64 nTime,WCHAR *szTime,int nSize);
BOOL	SystemTime2UTC(SYSTEMTIME *pSystemTime,UINT64 *pTime);
BOOL	UTC2SystemTime(UINT64 *pTime,SYSTEMTIME *pSystemTime);

// NTPУʱ��
struct   NTP_Packet
{
	int			Control_Word;   
	int			root_delay;   
	int			root_dispersion;   
	int			reference_identifier;   
	__int64		reference_timestamp;   
	__int64		originate_timestamp;   
	__int64		receive_timestamp;   
	int			transmit_timestamp_seconds;   
	int			transmit_timestamp_fractions;   
};

/************************************************************************/
/* ����˵��:�Զ���ʱ�������ͬ������
/* ����˵��:��
/* �� �� ֵ:�ɹ�����TRUE��ʧ�ܷ���FALSE
/************************************************************************/
BOOL NTPTiming(const char* szTimeServer);


#define TimeSpan(t)		(time(NULL) - (time_t)t)
#define TimeSpanEx(t)	(GetExactTime() - t)
typedef struct __ExactTimeBase
{
	LONGLONG	dfFreq;
	LONGLONG	dfCounter;
	time_t		nBaseClock;
	double		dfMilliseconds;
	__ExactTimeBase()
	{
		ZeroMemory(this,sizeof(ETB));
		SYSTEMTIME systime1;
		SYSTEMTIME systime2;
		ZeroMemory(&systime1, sizeof(SYSTEMTIME));
		ZeroMemory(&systime2, sizeof(SYSTEMTIME));

		HANDLE hProcess			= GetCurrentProcess();
		HANDLE hThread			= GetCurrentThread();

		DWORD dwPriorityClass	= GetPriorityClass(hProcess);		
		DWORD dwThreadPriority	= GetThreadPriority(hThread);

		DWORD dwError			= 0;
		// �ѽ������ȼ�������ʵʱ��
		if (!SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS))
		{
			dwError = GetLastError();
			if (!SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
			{
				dwError = GetLastError();
				if (!SetPriorityClass(hProcess, ABOVE_NORMAL_PRIORITY_CLASS))
				{
					dwError = GetLastError();
				}
			}
		}
		// ���߳����ȼ�������ʵʱ��
		if (!SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL))
		{
			dwError = GetLastError();
			if (!SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST))
			{
				dwError = GetLastError();
				if (!SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL))
				{
					dwError = GetLastError();
				}
			}
		}
		GetSystemTime(&systime1);
		// У׼��׼ʱ��
		while (true)
		{
			GetSystemTime(&systime2);
			if (memcmp(&systime1, &systime2, sizeof(SYSTEMTIME)) != 0)
				break;
		}
		// �ָ��̺߳ͽ��̵����ȼ�
		SetThreadPriority(hThread, dwThreadPriority);
		SetPriorityClass(hProcess, dwPriorityClass);

		SystemTime2UTC(&systime2,(UINT64 *)&nBaseClock);
		dfMilliseconds = (double)(systime2.wMilliseconds /1000);

#ifdef _DEBUG
		TCHAR szText[64] = {0};
		_stprintf_s(szText,_T("BaseClock of ETB = %I64d.\n"),nBaseClock);
		OutputDebugString(szText);
#endif
		LARGE_INTEGER LarInt;		
		QueryPerformanceFrequency(&LarInt);	
		dfFreq = LarInt.QuadPart;
		QueryPerformanceCounter(&LarInt);
		dfCounter = LarInt.QuadPart;
	}

	~__ExactTimeBase()
	{
	}

}ETB;
extern ETB g_etb;
#define	InitPerformanceClock	

/// @brief ȡϵͳ��ȷʱ��,��λ��,����Ϊ25΢������
double  GetExactTime();

/// @brief �߳����߷�ʽ
struct CThreadSleep
{
	enum SleepWay
	{
		Sys_Sleep = 0,		///< ֱ�ӵ���ϵͳ����Sleep
		Wmm_Sleep = 1,		///< ʹ�ö�ý��ʱ����߾���
		Std_Sleep = 2		///< ʹC++11�ṩ���߳����ߺ���
	};
	CThreadSleep()
	{
		double dfSumSpan1 = 0.0f, dfSumSpan2 = 0.0f, dfSumSpan3 = 0.0f;
		double dfT1 = 0.0f;
		for (int i = 0; i < 32; i++)
		{
			dfT1 = GetExactTime();
			Sleep(1);
			dfSumSpan1 += TimeSpanEx(dfT1);

			dfT1 = GetExactTime();
			timeBeginPeriod(1); //���þ���Ϊ1����
			::Sleep(1); //��ǰ�̹߳���һ����
			timeEndPeriod(1); //������������
			dfSumSpan2 += TimeSpanEx(dfT1);

			dfT1 = GetExactTime();
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
			dfSumSpan3 += TimeSpanEx(dfT1);
		}
		double dfSpanSum = dfSumSpan1;
		
		if (dfSumSpan1 <= dfSumSpan2)
		{
			nSleepWay = Sys_Sleep;
		}
		else
		{
			dfSpanSum = dfSumSpan2;
			nSleepWay = Wmm_Sleep;
		}
		if (dfSumSpan3 < dfSpanSum)
		{
			nSleepWay = Std_Sleep;
			dfSpanSum = dfSumSpan3;
		}
		nSleepPrecision = (DWORD)(1000 * dfSpanSum/32);
	}
	void operator ()(DWORD nTimems)
	{
		switch(nSleepWay)
		{
		case Sys_Sleep:
			::Sleep(nTimems);
			break;		
		case Wmm_Sleep:
		{
			timeBeginPeriod(1); //���þ���Ϊ1����
			::Sleep(nTimems);	//��ǰ�̹߳���һ����
			timeEndPeriod(1);	//������������
		}
			break;
		case Std_Sleep:
			std::this_thread::sleep_for(std::chrono::nanoseconds(nTimems*1000));
			break;
		default:
			assert(false);
			break;
		}
	}
	inline DWORD GetPrecision()
	{
		return nSleepPrecision;
	}

private:
	SleepWay	nSleepWay;
	DWORD		nSleepPrecision;
};

extern CThreadSleep ThreadSleep;
#define GetSleepPricision()	ThreadSleep.GetPrecision();

#define  _LockOverTime 100
#define  SaveWaitTime()	CWaitTime WaitTime(__FILE__,__LINE__,__FUNCTION__);
class CWaitTime
{
	DWORD dwTimeEnter;
	char szFile[512];
	int nLine;
	char szFunction[256];
public:
	CWaitTime(char *szInFile,int nInLine,char *szInFunction)
	{
		dwTimeEnter = timeGetTime();
		strcpy(szFile, szInFile);
		nLine = nInLine;
		strcpy(szFunction, szInFunction);
	}
	~CWaitTime()
	{
		if ((timeGetTime() - dwTimeEnter) > 200)
		{
			char szText[1024] = { 0 };
			_stprintf(szText, "Wait Timeout @File:%s %d(%s) WaitTime = %d(ms).\n", szFile, nLine, szFunction, (timeGetTime() - dwTimeEnter));
			OutputDebugString(szText);
		}
	}
};



struct LineTime
{
	CHAR szFile[256];
	int nLine;
	DWORD nTime;
	LineTime(char *pszFile, int nFileLine)
	{
		nTime = timeGetTime();
		strcpy(szFile, pszFile);
		nLine = nFileLine;
	}
};
#ifdef _DEBUG
#define SaveRunTime()		LineSave.SaveLineTime(__FILE__,__LINE__);
#define DeclareRunTime()	CLineRunTime LineSave;
#else
#define SaveRunTime()
#define DeclareRunTime()			
#endif

#include <vector>
using namespace  std;
class CLineRunTime
{
public:
	CLineRunTime(CRunlogA *plog = nullptr)
	{
		pRunlog = plog;
	}
	~CLineRunTime()
	{
		DWORD dwTotalSpan = 0;
		int nSize = pTimeArray.size();
		if (nSize < 1)
			return;
		else if (nSize < 2)
			dwTotalSpan = timeGetTime() - pTimeArray[0]->nTime;
		if (dwTotalSpan >= _LockOverTime * 2)
		{
			OutputMsg("@File:%s %s line %d Total Runtime span = %d.\n", __FUNCTION__, pTimeArray[0]->szFile, pTimeArray[0]->nLine, dwTotalSpan);
		}
		for (int i = 1; i < nSize; i++)
		{
			DWORD dwSpan = pTimeArray[i]->nTime - pTimeArray[i - 1]->nTime;
			if (dwSpan >= _LockOverTime)
				OutputMsg("@File:%s %s line %d Runtime span = %d.\n", __FUNCTION__, pTimeArray[i]->szFile, pTimeArray[i]->nLine, dwSpan);
		}
	}
	void SaveLineTime(char *szFile, int nLine)
	{
		shared_ptr<LineTime> pLineTime = make_shared<LineTime>(szFile, nLine);
		pTimeArray.push_back(pLineTime);
	}
#define __countof(array) (sizeof(array)/sizeof(array[0]))
#pragma warning (disable:4996)
	void OutputMsg(char *pFormat, ...)
	{
		int nBuff;
		CHAR szBuffer[4096];
		va_list args;
		va_start(args, pFormat);
		nBuff = _vsnprintf(szBuffer, __countof(szBuffer), pFormat, args);
		//::wvsprintf(szBuffer, pFormat, args);
		//assert(nBuff >=0);
#ifdef _DEBUG
		OutputDebugStringA(szBuffer);
#endif
		if (pRunlog)
			pRunlog->Runlog(szBuffer);
		va_end(args);
	}
public:
	vector<shared_ptr<LineTime>> pTimeArray;
	CRunlogA *pRunlog = nullptr;
};
