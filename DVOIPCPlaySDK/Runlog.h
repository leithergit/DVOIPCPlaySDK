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

#ifndef byte
#define byte unsigned char
#endif

//class  CRunlog;

// #ifndef __ENABLE_RUNLOG
 #define __ENABLE_RUNLOG
// #else
// #undef  __ENABLE_RUNLOG
// #endif	


#ifdef _UNICODE
#define Runlog			RunlogW
#define Runlogv			RunlogvW
#define RunlogBin		RunlogBinW
#define RunlogHuge		RunlogHugeW
#define RunlogEx		RunlogW
#else
#define Runlog			RunlogA
#define Runlogv			RunlogvA
#define RunlogBin		RunlogBinA
#define RunlogHuge		RunlogHugeA
#define RunlogEx		RunlogA
#endif

void EnableRunlog(bool bEnable=true);
class  CRunlog  
{
public:
	CRunlog();
	CRunlog(LPCTSTR lpszFileName);
	HANDLE m_hLogFile;	
	SYSTEMTIME m_tTime;
	// ANSI�汾
	void RunlogA(LPCSTR pFormat, ...);
	// ɾ��RunlogEx��������֮��Runlog��
	//void RunlogEx(LPCSTR pFormat, ...);
	void RunlogBinA(LPCSTR szTitle,byte *pBinBuff,int nLen,CHAR chSeperator = ' ');
	void RunlogHugeA(byte *szHugeData,int nDateLen,LPCSTR pFormat,...);	
	void RunlogvA(LPCSTR format, va_list ap);
	void RunlogHugevA(LPCSTR szHugeText,const CHAR *format, va_list ap);
	// UNICODE�汾
	void RunlogW(LPCWSTR pFormat, ...);
	void RunlogBinW(LPCWSTR szTitle,byte *pBinBuff,int nLen,WCHAR chSeperator = L' ');
	void RunlogHugeW(byte *szHugeData,int nDateLen,LPCWSTR pFormat,...);		
	void RunlogvW(LPCWSTR format, va_list ap);
	void RunlogHugevW(LPCWSTR szHugeText,const WCHAR *format, va_list ap);
	virtual ~CRunlog();
private:
	SYSTEMTIME m_systimeCreate;		// ��־���������ʱ��
	bool m_bCanlog;
	TCHAR	m_szFileName[MAX_PATH];
	CRITICAL_SECTION m_RunlogSection;
	void CheckDateTime();			// ����ļ�������,���ļ��Ĵ��������뵱ǰ���ڣ��򴴽����ļ�
};

#endif // !defined(AFX_RUNLOG_H__C589DB41_33FE_4903_A7D7_BCE9F5074877__INCLUDED_)
