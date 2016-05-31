// Runlog.h: interface for the CRunlog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUNLOG_H__C589DB41_33FE_4903_A7D7_BCE9F5074877__INCLUDED_)
#define AFX_RUNLOG_H__C589DB41_33FE_4903_A7D7_BCE9F5074877__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>
#include <TCHAR.H>
//#include <memory>
#include <boost/smart_ptr.hpp>
using namespace std;
using namespace boost;
//using namespace std::tr1;

#ifndef byte
#define byte unsigned char
#endif

//class  CRunlog;

// #ifndef __ENABLE_RUNLOG
 #define __ENABLE_RUNLOG
// #else
// #undef  __ENABLE_RUNLOG
// #endif	


void EnableRunlog(bool bEnable=true);
class CRunlogA
{
public:
	CRunlogA();
	CRunlogA(LPCSTR lpszFileName);
	HANDLE m_hLogFile;
	SYSTEMTIME m_tTime;
	// ANSI版本
	void Runlog(LPCSTR pFormat, ...);
	// 删除RunlogEx函数，代之以Runlog宏
	//void RunlogEx(LPCSTR pFormat, ...);
	void RunlogBin(LPCSTR szTitle, byte *pBinBuff, int nLen, CHAR chSeperator = ' ');
	void RunlogHuge(byte *szHugeData, int nDateLen, LPCSTR pFormat, ...);
	void Runlogv(LPCSTR format, va_list ap);
	void RunlogHugev(LPCSTR szHugeText, const CHAR *format, va_list ap);	
	virtual ~CRunlogA();
private:
	SYSTEMTIME m_systimeCreate;		// 日志创建或更换时间
	bool m_bCanlog;
	CHAR	m_szFileName[MAX_PATH];
	boost::shared_ptr<char>m_pLogBuffer;
	CRITICAL_SECTION m_RunlogSection;
	void CheckDateTime();			// 检查文件的日期,若文件的创建日期与当前日期，则创建新文件
};
class  CRunlogW
{
public:
	CRunlogW();
	CRunlogW(LPCWSTR lpszFileName);
	HANDLE m_hLogFile;	
	SYSTEMTIME m_tTime;
	// UNICODE版本
	void Runlog(LPCWSTR pFormat, ...);
	void RunlogBin(LPCWSTR szTitle,byte *pBinBuff,int nLen,WCHAR chSeperator = L' ');
	void RunlogHuge(byte *szHugeData,int nDateLen,LPCWSTR pFormat,...);		
	void Runlogv(LPCWSTR format, va_list ap);
	void RunlogHugev(LPCWSTR szHugeText,const WCHAR *format, va_list ap);
	virtual ~CRunlogW();
private:
	SYSTEMTIME m_systimeCreate;		// 日志创建或更换时间
	bool m_bCanlog;
	WCHAR	m_szFileName[MAX_PATH];
	boost::shared_ptr<WCHAR>m_pLogBuffer;
	CRITICAL_SECTION m_RunlogSection;
	void CheckDateTime();			// 检查文件的日期,若文件的创建日期与当前日期，则创建新文件
};

#ifdef _UNICODE
typedef		CRunlogA CRunlog;
#else
typedef		CRunlogW CRunlog;
#endif

#endif // !defined(AFX_RUNLOG_H__C589DB41_33FE_4903_A7D7_BCE9F5074877__INCLUDED_)
