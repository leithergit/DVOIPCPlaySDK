// Runlog.cpp: implementation of the CRunlog class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"

#include "Runlog.h"
#include <TCHAR.H>
#include <stdio.h>
#include <time.h>

#include <Shlwapi.h>
#include "Utility.h"

#include <memory>
using namespace std;
using namespace std::tr1;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

#pragma comment(lib,"Shlwapi.lib")
#if !defined(_countof)
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#define _DatetimeLen	8
#define _ExtNameLen		4
#define _LogBuffLength	1024*16
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996 4101) 
volatile bool g_bEnableRunlog = true;
void EnableRunlog(bool bEnable)
{
	g_bEnableRunlog = bEnable;
}

void CRunlog::CheckDateTime()
{
	TCHAR	szDataTime[32] = {0};
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	_stprintf_s(szDataTime,32,_T("%04d%02d%02d"),tm.wYear,tm.wMonth,tm.wDay);
	__try
	{
		if ((tm.wYear == m_systimeCreate.wYear) && (tm.wMonth == m_systimeCreate.wMonth) && (tm.wDay == m_systimeCreate.wDay))
			__leave;
		//重新创建文件
			int nLen = _tcslen(m_szFileName) - _DatetimeLen - _ExtNameLen;	// 取得原始输入的文件名
			_tcscpy_s(&m_szFileName[nLen],MAX_PATH - nLen,szDataTime);		
			_tcscat(m_szFileName,_T(".log"));
			::EnterCriticalSection(&m_RunlogSection);
			CloseHandle(m_hLogFile);
			m_hLogFile = NULL;
			m_hLogFile = CreateFile(m_szFileName,
				GENERIC_WRITE|GENERIC_READ,
				FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_ARCHIVE,
				NULL);
			::LeaveCriticalSection(&m_RunlogSection);
			if (m_hLogFile == NULL) 
				m_bCanlog = false;
			else
				GetLocalTime(&m_systimeCreate);
	}
	__finally
	{

	}
}
CRunlog::CRunlog()
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	m_hLogFile = NULL;
	//TCHAR szFileName[255] = {0};	
	TCHAR szApppath[MAX_PATH] = {0};
	m_bCanlog = true;	
	SYSTEMTIME tm;
	try
	{
		InitializeCriticalSection(&m_RunlogSection);
		GetLocalTime(&tm);		
		GetAppPath(szApppath,MAX_PATH);
		_stprintf_s(m_szFileName,MAX_PATH,_T("%s\\log\\%04d%02d%02d.log"),szApppath,tm.wYear,tm.wMonth,tm.wDay);
		_tcscat(szApppath,_T("\\log"));
		if (!PathFileExists(szApppath))
		{
			CreateDirectory(szApppath, NULL);
		}		
		
		m_hLogFile = CreateFile(m_szFileName,
								GENERIC_WRITE|GENERIC_READ,
								FILE_SHARE_READ|FILE_SHARE_WRITE,
								NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_ARCHIVE,
								NULL);
		if (m_hLogFile == NULL) 
			m_bCanlog = false;
		else					
			GetLocalTime(&m_systimeCreate);
		
	}
	catch (std::exception &e)
	{
		m_bCanlog = false;
		m_hLogFile = NULL;
	}
}

CRunlog::CRunlog(LPCTSTR lpszFileName)
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	m_hLogFile = NULL;
	m_bCanlog = true;
//	TCHAR szFileName[255] = {0};
	TCHAR szApppath[MAX_PATH] = {0};
	
	SYSTEMTIME tm;	
	try
	{
		InitializeCriticalSection(&m_RunlogSection);
		GetLocalTime(&tm);
		GetAppPath(szApppath,MAX_PATH);
		//_tcscpy(szApppath,_T("d:/"));
		if (_tcsstr(lpszFileName,_T(".log")))
		{
			TCHAR szTempFileName[MAX_PATH] = {0};
			memcpy(szTempFileName,lpszFileName,(_tcslen(lpszFileName) - 4)*sizeof(TCHAR));
			_stprintf_s(m_szFileName,MAX_PATH,_T("%s\\log\\%s%02d%02d%02d.log"),szApppath,szTempFileName,tm.wYear,tm.wMonth,tm.wDay);
		}
		else
			_stprintf_s(m_szFileName,MAX_PATH,_T("%s\\log\\%s%02d%02d%02d.log"),szApppath,lpszFileName,tm.wYear,tm.wMonth,tm.wDay);
		_tcscat(szApppath,_T("\\log"));
		if (!PathFileExists(szApppath))
		{
			CreateDirectory(szApppath, NULL);
		}
		m_hLogFile = CreateFile(m_szFileName,
								GENERIC_WRITE|GENERIC_READ,
								FILE_SHARE_READ|FILE_SHARE_WRITE,
								NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_ARCHIVE,
								NULL);
		if (m_hLogFile == NULL) 
			m_bCanlog = false;
		else
			GetLocalTime(&m_systimeCreate);
		
	}
	catch (std::exception &e)
	{
		m_bCanlog = false;
		m_hLogFile = NULL;
	}
}

void CRunlog::RunlogBinA(LPCSTR szTitle,byte *pBinBuff,int nLen,CHAR chSeperator)
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	if (NULL == szTitle || NULL == pBinBuff || 0 == nLen)
		return;
	if (m_bCanlog)
	{	
		try
		{
			//检查当前的文件是否当前日期产生
			CHAR	szDataTime[32] = {0};
			CHAR	szBuffer[_LogBuffLength] = {0};
			_strtime_s((char *)szBuffer,_LogBuffLength);			
			int nDatetimeLen = strlen((char *)szBuffer);
			szBuffer[nDatetimeLen++] = _T(' ');
			CHAR szTitleA[256] = {0};
			CHAR szSeperatorA[4] = {0};
			CHAR chSeperator1;
			strcat_s((char *)szBuffer,_LogBuffLength - nDatetimeLen,(char *)szTitle);
			chSeperator1 = chSeperator;

			int nBuffLen = strlen((char *)szBuffer);
			Hex2AscStringA(pBinBuff,nLen,(CHAR *)&szBuffer[nBuffLen],_LogBuffLength - nBuffLen,chSeperator1);
			
			DWORD dwWritten = 0;
			::EnterCriticalSection(&m_RunlogSection);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			WriteFile(m_hLogFile,szBuffer,strlen((char *)szBuffer),&dwWritten,NULL);

			szBuffer[0] = 0x0D;
			szBuffer[1] = 0x0A;
			szBuffer[2] = 00;
			WriteFile(m_hLogFile,szBuffer,2,&dwWritten,NULL);
			FlushFileBuffers(m_hLogFile);
			::LeaveCriticalSection(&m_RunlogSection);

		}
		catch (std::exception &e)
		{
			if (m_hLogFile != NULL)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;
			}		
			m_bCanlog = false;
		}
	}
}

void CRunlog::RunlogA(LPCSTR pFormat, ...)
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	if (m_bCanlog)
	{
		try
		{
			CheckDateTime();
			va_list args;
			va_start(args, pFormat);
			CHAR szBuffer[_LogBuffLength] = {0};	
			int nDatetimeLen = 0;			
			_strtime_s(szBuffer,_LogBuffLength);
			nDatetimeLen = strlen(szBuffer);
			szBuffer[nDatetimeLen++] = _T(' ');
			vsnprintf((CHAR *)&szBuffer[nDatetimeLen], _LogBuffLength - nDatetimeLen, pFormat, args);
			DWORD dwWritten = 0;	
			::EnterCriticalSection(&m_RunlogSection);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);			
			WriteFile(m_hLogFile,szBuffer,strlen(szBuffer),&dwWritten,NULL);
			FlushFileBuffers(m_hLogFile);
			::LeaveCriticalSection(&m_RunlogSection);
			va_end(args);
		}
		catch (std::exception &e)
		{
			if (m_hLogFile != NULL)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;
			}		
			m_bCanlog = false;
		}
	}
}

void CRunlog::RunlogHugeA(byte *szHugeData,int nDataLen,LPCSTR pFormat,...)
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	if (m_bCanlog)
	{
		try
		{
			//检查当前的文件是否当前日期产生
			CheckDateTime();	
			va_list args;
			va_start(args, pFormat);
			CHAR szBuffer[_LogBuffLength] = {0};	
			int nDatetimeLen = 0;
			_strtime_s(szBuffer,_LogBuffLength);
			nDatetimeLen = strlen(szBuffer);
			szBuffer[nDatetimeLen ++] = _T(' ');
			vsnprintf((CHAR *)&szBuffer[nDatetimeLen], _LogBuffLength - nDatetimeLen , pFormat, args);
			DWORD dwWritten = 0;			
			::EnterCriticalSection(&m_RunlogSection);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			BOOL bWrite = WriteFile(m_hLogFile,szBuffer,strlen(szBuffer),&dwWritten,NULL);
			//SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			bWrite = WriteFile(m_hLogFile,szHugeData,nDataLen,&dwWritten,NULL);
			FlushFileBuffers(m_hLogFile);
			::LeaveCriticalSection(&m_RunlogSection);			
			va_end(args);
		}
		catch (std::exception &e)
		{
			if (m_hLogFile != NULL)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;
			}		
			m_bCanlog = false;
		}
	}
}

void CRunlog::RunlogvA(LPCSTR pFormat, va_list args)
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	if (m_bCanlog)
	{
		try
		{
			//检查当前的文件是否当前日期产生
			CheckDateTime();
			CHAR szBuffer[_LogBuffLength] = {0};		
			int nDatetimeLen = 0;
			_strtime_s(szBuffer,_LogBuffLength);
			nDatetimeLen = strlen(szBuffer);
			szBuffer[nDatetimeLen++] = ' ';
			vsnprintf((CHAR *)&szBuffer[nDatetimeLen], _LogBuffLength - nDatetimeLen, pFormat, args);
			DWORD dwWritten = 0;	
			::EnterCriticalSection(&m_RunlogSection);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);			
			WriteFile(m_hLogFile,szBuffer,strlen(szBuffer),&dwWritten,NULL);
			FlushFileBuffers(m_hLogFile);
			::LeaveCriticalSection(&m_RunlogSection);			
		}
		catch (std::exception &e)
		{
			if (m_hLogFile != NULL)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;
			}		
			m_bCanlog = false;
		}
	}
}
void CRunlog::RunlogHugevA(LPCSTR szHugeText,const CHAR *pFormat, va_list args)
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	if (m_bCanlog)
	{
		try
		{
			CheckDateTime();			
			CHAR szBuffer[_LogBuffLength] = {0};	
			int nDatetimeLen = 0;
			int nHugeTextLen = 0;
			byte *pHugeTextA = NULL;

			_strtime_s(szBuffer,_LogBuffLength);
			nDatetimeLen = strlen(szBuffer);
			szBuffer[nDatetimeLen++] = ' ';
			vsnprintf((CHAR *)&szBuffer[nDatetimeLen], _LogBuffLength - nDatetimeLen, pFormat, args);
			DWORD dwWritten = 0;			
			::EnterCriticalSection(&m_RunlogSection);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			WriteFile(m_hLogFile,szBuffer,strlen((LPSTR)szBuffer),&dwWritten,NULL);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			WriteFile(m_hLogFile,szHugeText,nHugeTextLen,&dwWritten,NULL);			
			FlushFileBuffers(m_hLogFile);
			::LeaveCriticalSection(&m_RunlogSection);
		}
		catch (std::exception &e)
		{
			if (m_hLogFile != NULL)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;
			}		
			m_bCanlog = false;
		}
	}
}


void CRunlog::RunlogW(LPCWSTR pFormat, ...)
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	if (m_bCanlog)
	{
		try
		{
			CheckDateTime();
			va_list args;
			va_start(args, pFormat);			
			int nDatetimeLen = 0;
			WCHAR szBufferW[_LogBuffLength] = {0};	
			_wstrtime_s(szBufferW,_LogBuffLength);
			nDatetimeLen = wcslen(szBufferW);
			szBufferW[nDatetimeLen++] = _T(' ');
			_vsnwprintf((WCHAR *)&szBufferW[nDatetimeLen], _LogBuffLength - nDatetimeLen, pFormat, args);
			DWORD dwWritten = 0;
			int nBufferLen = 0;
			stdshared_ptr<char> pBufferA = W2AString(szBufferW,nBufferLen);	

			::EnterCriticalSection(&m_RunlogSection);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);			
			WriteFile(m_hLogFile,pBufferA.get(),nBufferLen,&dwWritten,NULL);
			FlushFileBuffers(m_hLogFile);
			::LeaveCriticalSection(&m_RunlogSection);
			va_end(args);
		}
		catch (std::exception &e)
		{
			if (m_hLogFile != NULL)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;
			}		
			m_bCanlog = false;
		}
	}
}

void CRunlog::RunlogBinW(LPCWSTR szTitle,byte *pBinBuff,int nLen,WCHAR chSeperator)
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	if (NULL == szTitle || NULL == pBinBuff || 0 == nLen)
		return;
	if (m_bCanlog)
	{	
		try
		{
			//检查当前的文件是否当前日期产生
			WCHAR	szDataTime[32] = {0};
			WCHAR	szBuffer[_LogBuffLength] = {0};
			_wstrtime_s((WCHAR*)szBuffer,_LogBuffLength);			
			int nDatetimeLen = wcslen((WCHAR*)szBuffer);
			szBuffer[nDatetimeLen++] = L' ';
			WCHAR szTitle[256] = {0};
			WCHAR szSeperator[4] = {0};			
			wcscat_s((WCHAR *)szBuffer,_LogBuffLength - nDatetimeLen,(WCHAR*)szTitle);		
			int nBuffLen = wcslen((WCHAR*)szBuffer);
			Hex2AscStringW(pBinBuff,nLen,(WCHAR *)&szBuffer[nBuffLen],_LogBuffLength - nBuffLen,chSeperator);
						
			stdshared_ptr<char> pBufferA = W2AString(szBuffer,nBuffLen);
			
			DWORD dwWritten = 0;
			::EnterCriticalSection(&m_RunlogSection);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			WriteFile(m_hLogFile,pBufferA.get(),nBuffLen,&dwWritten,NULL);
			char *szLine = "\r\n";		// 回车换行			
 			WriteFile(m_hLogFile,szLine,2,&dwWritten,NULL);
 			FlushFileBuffers(m_hLogFile);
 			::LeaveCriticalSection(&m_RunlogSection);
		}
		catch (std::exception &e)
		{
			if (m_hLogFile != NULL)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;
			}		
			m_bCanlog = false;
		}
	}
}

void CRunlog::RunlogHugeW(byte *szHugeData,int nDataLen,LPCWSTR pFormat,...)
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	if (m_bCanlog)
	{
		try
		{
			//检查当前的文件是否当前日期产生
			CheckDateTime();	
			va_list args;
			va_start(args, pFormat);			

			WCHAR szBuffer[_LogBuffLength] = {0};
			_wstrtime_s(szBuffer,_LogBuffLength);
			int nDatetimeLen = wcslen(szBuffer);
			szBuffer[nDatetimeLen ++] = L' ';
			szBuffer[nDatetimeLen] = L'\0';
			_vsnwprintf((WCHAR *)&szBuffer[nDatetimeLen], _LogBuffLength - nDatetimeLen, pFormat, args);

			int nBufferLen = 0;
			stdshared_ptr<char> pBufferA = W2AString(szBuffer,nBufferLen);

			DWORD dwWritten = 0;			
			::EnterCriticalSection(&m_RunlogSection);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			BOOL bWrite = WriteFile(m_hLogFile,pBufferA.get(),nBufferLen,&dwWritten,NULL);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			bWrite = WriteFile(m_hLogFile,szHugeData,nDataLen,&dwWritten,NULL);
			FlushFileBuffers(m_hLogFile);
			::LeaveCriticalSection(&m_RunlogSection);			
			va_end(args);
		}
		catch (std::exception &e)
		{
			if (m_hLogFile != NULL)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;
			}		
			m_bCanlog = false;
		}
	}
}

void CRunlog::RunlogvW(LPCWSTR pFormat, va_list args)
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	if (m_bCanlog)
	{
		try
		{
			//检查当前的文件是否当前日期产生
			CheckDateTime();			
			int nDatetimeLen = 0;			
			WCHAR szBuffer[_LogBuffLength] = {0};	
			_wstrtime_s(szBuffer,_LogBuffLength);
			nDatetimeLen = wcslen(szBuffer);
			szBuffer[nDatetimeLen++] = _T(' ');
			_vsnwprintf((WCHAR *)&szBuffer[nDatetimeLen], _LogBuffLength - nDatetimeLen, pFormat, args);

			int nBufferLen = 0;
			stdshared_ptr<char> pBufferA = W2AString(szBuffer,nBufferLen);
	
			::EnterCriticalSection(&m_RunlogSection);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			DWORD dwWritten = 0;	
			WriteFile(m_hLogFile,pBufferA.get(),nBufferLen,&dwWritten,NULL);
			FlushFileBuffers(m_hLogFile);
			::LeaveCriticalSection(&m_RunlogSection);			
		}
		catch (std::exception &e)
		{
			if (m_hLogFile != NULL)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;
			}		
			m_bCanlog = false;
		}
	}
}
void CRunlog::RunlogHugevW(LPCWSTR szHugeText,const WCHAR *pFormat, va_list args)
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	if (m_bCanlog)
	{
		try
		{
			CheckDateTime();						
			int nDatetimeLen = 0;
			
			WCHAR szBuffer[_LogBuffLength] = {0};	
			_wstrtime_s(szBuffer,_LogBuffLength);
			nDatetimeLen = wcslen(szBuffer);
			szBuffer[nDatetimeLen++] = _T(' ');
			_vsnwprintf((WCHAR *)&szBuffer[nDatetimeLen], _LogBuffLength - nDatetimeLen, pFormat, args);

			int nBufferLen = 0;
			stdshared_ptr<char> pBufferA = W2AString(szBuffer,nBufferLen);
			int nHugeTextLen = 0;
			stdshared_ptr<char> pHugeTextA = W2AString(szHugeText,nHugeTextLen);			
			
			DWORD dwWritten = 0;			
			::EnterCriticalSection(&m_RunlogSection);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			WriteFile(m_hLogFile,pBufferA.get(),strlen((LPSTR)szBuffer),&dwWritten,NULL);
			SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			WriteFile(m_hLogFile,pHugeTextA.get(),strlen((LPCSTR)pHugeTextA.get()),&dwWritten,NULL);			
			FlushFileBuffers(m_hLogFile);
			::LeaveCriticalSection(&m_RunlogSection);

		}
		catch (std::exception &e)
		{
			if (m_hLogFile != NULL)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;
			}		
		}
	}
}

CRunlog::~CRunlog()
{
	if (!g_bEnableRunlog)
	{
		return ;
	}
	if (m_bCanlog)
	{
		__try
		{
			::EnterCriticalSection(&m_RunlogSection);
			FlushFileBuffers(m_hLogFile);
			if (CloseHandle(m_hLogFile))
			{	
				m_hLogFile = NULL;
				m_bCanlog = false;
			}
			::LeaveCriticalSection(&m_RunlogSection);
			::DeleteCriticalSection(&m_RunlogSection);
		}		
		__except(1)
		{
			m_hLogFile = NULL;
			m_bCanlog = false;
		}
	}
}