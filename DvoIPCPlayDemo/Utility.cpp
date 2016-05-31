//#include "stdafx.h"
//用于测试VSS的修改
#include "Utility.h"
#include <TCHAR.H>
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <Shlwapi.h>
//#include <memory>
//#include "AutoPtrArray.h"
#include <boost/smart_ptr.hpp>
using namespace std;
//using namespace std::tr1;
using namespace boost;

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib, "comsuppw.lib")


#define __countof(array) (sizeof(array)/sizeof(array[0]))
#pragma warning (disable:4996)

void TraceMsgA(LPCSTR pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);
	int nBuff;
	CHAR szBuffer[0x7fff];
	nBuff = _vsnprintf(szBuffer, __countof(szBuffer), pFormat, args);
	//::wvsprintf(szBuffer, pFormat, args);
	//assert(nBuff >=0);
	OutputDebugStringA(szBuffer);
	va_end(args);	
}

void TraceMsgW(LPCWSTR pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);
	int nBuff;
	WCHAR szBuffer[8192];
	nBuff = _vsnwprintf(szBuffer, __countof(szBuffer), pFormat, args);
	//::wvsprintf(szBuffer, pFormat, args);
	//assert(nBuff >=0);
	OutputDebugStringW(szBuffer);
	va_end(args);	
}
// 把路径szSrcPath中的字符szSeperatorSrc替换成字符szSeperatorDst
void TranslatePathA(LPSTR szSrcPath,CHAR szSeperatorSrc,CHAR szSeperatorDst)
{
	while(*szSrcPath ++)
	{
		if (*szSrcPath == szSeperatorSrc)
			*szSrcPath = szSeperatorDst;
	}
}

// 把路径szSrcPath中的字符szSeperatorSrc替换成字符szSeperatorDst
void TranslatePathW(LPWSTR szSrcPath,WCHAR szSeperatorSrc,WCHAR szSeperatorDst)
{
	while(*szSrcPath ++)
	{
		if (*szSrcPath == szSeperatorSrc)
			*szSrcPath = szSeperatorDst;
	}
}


// 判断字符是否是只包含空格或Tab符，是则返回true,否则返回false
bool IsEmptyStringA(IN char *ptr)
{
	if (!ptr)
		return true;
	int curpos=0;

	while(0 != ptr[curpos])
	{
		if((' ' != ptr[curpos]) && ('\t' != ptr[curpos]))
		{			
			return false;
		}
		curpos++;
	}
	return true;
}

// 判断字符是否是只包含空格或Tab符，是则返回true,否则返回false
bool IsEmptyStringW(IN WCHAR *ptr)
{
	if (!ptr)
		return true;
	int curpos=0;

	while(L'\0' != ptr[curpos])
	{
		if((L' ' != ptr[curpos]) && (L'\t' != ptr[curpos]))
		{			
			return false;
		}
		curpos++;
	}
	return true;
}


ULONG inet_addrW(LPCWSTR szIPW)
{
	char szIpA[64] = {0};
	WideCharToMultiByte(CP_ACP, 0, szIPW, -1, szIpA, 64, NULL, NULL);
	return inet_addr(szIpA);	
}

WCHAR* FAR inet_ntow(in_addr in)
{
	CHAR *szIPA = inet_ntoa(in);
	static WCHAR szIPW[64] = {0};
	MultiByteToWideChar(CP_ACP, 0, szIPA, -1, szIPW, 64);
	return (WCHAR *)szIPW;
}
// 取本机IP地址,存放于szIpAddress中
// 可能获得多个IP地址,每个IP地址占用16字符
// 返回值为IP地址数量
int GetHostIPAddressW(WCHAR *szIpAddress,int nBuffSize)
{
	WCHAR szText[255] = {0};
	char host_name[225];
	if(gethostname(host_name,sizeof(host_name))==SOCKET_ERROR)
	{
		GetLastErrorMessageW(L"获取主机名称",szText,WSAGetLastError());
		TraceMsg(_T("%s.\n"),szText);
		return 0;
	}

	struct hostent *phe=gethostbyname(host_name);
	if(phe==0)
	{
		GetLastErrorMessageW(L"获取主机名称",szText,WSAGetLastError());
		TraceMsg(_T("%s.\n"),szText);
		return 0;
	}
	struct in_addr addr;
	int i = 0;
	ZeroMemory(szIpAddress,nBuffSize*sizeof(WCHAR));

	for (;phe->h_addr_list[i] != 0; ++i) 
	{
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		char *szIPA = inet_ntoa(addr);
		WCHAR szIPW[32] = {0};
		MultiByteToWideChar(CP_ACP, 0, szIPA, -1, szIPW, 32);
		int nLen = wcslen(szIPW);
		wcscpy_s(&szIpAddress[i*16],nBuffSize,szIPW);
		nBuffSize -= 16;
	}	
	wcscpy_s(&szIpAddress[i*16],nBuffSize,L"127.0.0.1");
	return ++i;
}

// 取本机IP地址,存放于szIpAddress中
// 可能获得多个IP地址,每个IP地址占用16字符
// 返回值为IP地址数量
int GetHostIPAddressA(CHAR *szIpAddress,int nBuffSize)
{
	CHAR szText[255] = {0};
	char host_name[225];
	if(gethostname(host_name,sizeof(host_name))==SOCKET_ERROR)
	{
		GetLastErrorMessageA("获取主机名称",szText,WSAGetLastError());
		TraceMsg(_T("%s.\n"),szText);
		return 0;
	}

	struct hostent *phe=gethostbyname(host_name);
	if(phe==0)
	{
		GetLastErrorMessageA("获取主机名称",szText,WSAGetLastError());
		TraceMsg(_T("%s.\n"),szText);
		return 0;
	}
	struct in_addr addr;
	int i = 0;
	ZeroMemory(szIpAddress,nBuffSize*sizeof(CHAR));

	for (;phe->h_addr_list[i] != 0; ++i) 
	{
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		char *szIPA = inet_ntoa(addr);
		int nLen = strlen(szIPA);
		strcpy_s(&szIpAddress[i*16],nBuffSize,szIPA);
		nBuffSize -= 16;
	}	
	strcpy_s(&szIpAddress[i*16],nBuffSize,"127.0.0.1");
	return ++i;
}

//批量禁用/启用控件
//hWnd		控件所在窗口的句柄
//bEnable	为true启用控件,false则禁用控件
//nIDCount	要操作的控件的数量
//Sample:EnableDlgItems(hWnd,true,5,ID1,ID2,ID3,ID4,ID5);
//其中ID1,ID2...为控件ID
void EnableDlgItems(HWND hWnd,bool bEnable,int nIDCount,...)
{  						
	va_list   Args; 
	va_start(Args,nIDCount);
	int  nID;
	int nCount = 0;
	do
	{
		nID = va_arg(Args,int);
		::EnableWindow(::GetDlgItem(hWnd,nID),bEnable);
		nCount ++;

	}while (nCount < nIDCount);   
	va_end(Args);
}

// @li@163.com
// 1do@163.com
// abc@163..com
// Abc.com.@163.com
// abc.com@.163com.cn
// abc.com@163com.cn.
// abc.com@163com.cn
//	
// 必须符合string@domain的形式
// string 

bool IsValidMailAddressA(CHAR *szMail)
{
	// 第一个字符不合法
	if (!isalpha(szMail[0]))
		return false;
	int nLen = strlen(szMail);
	// 先查找@
	CHAR *pDest = strchr(szMail,'@');
	if (!pDest)
		return false;

	// @不是最后一个字符
	int nOffset = pDest - szMail;
	if ((nOffset + 1) == nLen)
		return false;

	// .不是最后一个字符
	if (szMail[nLen - 1] == '.')
		return false;

	// 不存.@也不在@.
	if (*(pDest-1) == '.' ||
		*(pDest+1) == '.')
		return false;

	// 是否存在连续的句点符(..)
	if (strstr(szMail,".."))
		return false;	

	// 检查@前半段是否有效
	for (int i = 0;i < nOffset;i ++)
	{
		if (szMail[i] == '.' ||
			isalnum(szMail[i]))
			continue;
		else
			return false;
	}
	nOffset += 2;
	for (int i = nOffset ;i < nLen;i ++)
	{
		if (szMail[i] == '.' ||
			isalnum(szMail[i]))
			continue;
		else
			return false;
	}
	return true;
}

bool IsValidMailAddressW(WCHAR *szMail)
{
	// 第一个字符不合法
	if (!iswalpha(szMail[0]))
		return false;
	int nLen = wcslen(szMail);
	// 先查找@
	WCHAR *pDest = wcschr(szMail,L'@');
	if (!pDest)
		return false;

	// @不是最后一个字符
	int nOffset = pDest - szMail;
	if ((nOffset + 1) == nLen)
		return false;

	// .不是最后一个字符
	if (szMail[nLen - 1] == L'.')
		return false;

	// 不存.@也不在@.
	if (*(pDest-1) == L'.' ||
		*(pDest+1) == L'.')
		return false;

	// 是否存在连续的句点符(..)
	if (wcsstr(szMail,L".."))
		return false;	

	// 检查@前半段是否有效
	for (int i = 0;i < nOffset;i ++)
	{
		if (szMail[i] == L'.' ||
			iswalnum(szMail[i]))
			continue;
		else
			return false;
	}
	nOffset += 2;
	for (int i = nOffset ;i < nLen;i ++)
	{
		if (szMail[i] == L'.' ||
			iswalnum(szMail[i]))
			continue;
		else
			return false;
	}
	return true;
}

void TraceLongStringA(LPCSTR pFormat,byte *szString ,int nStringLength,...)
{
	va_list args;
	va_start(args, nStringLength);
	int nBuff;
	CHAR szBuffer[1024] = {0};
	nBuff = _vsnprintf_s(szBuffer,1024, __countof(szBuffer), pFormat, args);
	//::wvsprintf(szBuffer, pFormat, args);
	assert(nBuff >=0);
	// OutputDebugString(szBuffer);
	va_end(args);	

	ZeroMemory(szBuffer,1024*sizeof(CHAR));
	int nLen = 0;
	for (int i = 0;i < nStringLength;i ++)
	{
		sprintf_s(&szBuffer[nLen],1024 - nLen,"%02X ",szString[i]);
		nLen += 3;
		if (nLen >= 1023)
		{
			OutputDebugStringA(szBuffer);
			ZeroMemory(szBuffer,1024*sizeof(CHAR));			
			nLen = 0;
		}
	}
	// OutputDebugString(szBuffer);
	// OutputDebugString(_T("\n"));
}

void TraceLongStringW(LPCWSTR pFormat,byte *szString ,int nStringLength,...)
{
	va_list args;
	va_start(args, nStringLength);
	int nBuff;
	WCHAR szBuffer[1024] = {0};
	nBuff = _vsnwprintf_s(szBuffer,1024, __countof(szBuffer), pFormat, args);
	//::wvsprintf(szBuffer, pFormat, args);
	assert(nBuff >=0);
	// OutputDebugString(szBuffer);
	va_end(args);	

	ZeroMemory(szBuffer,1024*sizeof(WCHAR));
	int nLen = 0;
	for (int i = 0;i < nStringLength;i ++)
	{
		swprintf_s(&szBuffer[nLen],1024 - nLen,L"%02X ",szString[i]);
		nLen += 3;
		if (nLen >= 1023)
		{
			OutputDebugStringW(szBuffer);
			ZeroMemory(szBuffer,1024*sizeof(WCHAR));			
			nLen = 0;
		}
	}
	// OutputDebugString(szBuffer);
	// OutputDebugString(_T("\n"));
}


//去掉字符串的空格符和制表符
void TrimSpaceA(char *ptr)
{
	if (ptr == NULL )
		return ;
	int begin=-1, end=0, curpos=0;

	while(0 != ptr[curpos])
	{
		if((' ' != ptr[curpos]) && ('\t' != ptr[curpos]))
		{			
			if(-1 == begin)
				begin = curpos;			
			end = curpos;
		}
		curpos++;
	}
	ptr[end+1] = 0;

	if(-1 == begin)
		ptr[0] = 0;
	else if(0 != begin)
	{
		memmove(ptr, &ptr[begin], end-begin+1);
		ZeroMemory(&ptr[end],begin);
	}
}
//去掉字符串的空格符和制表符
void TrimSpaceW(WCHAR *ptr)
{
	if (ptr == NULL )
		return ;
	int begin=-1, end=0, curpos=0;

	while(L'\0' != ptr[curpos])
	{
		if((L' ' != ptr[curpos]) && (L'\t' != ptr[curpos]))
		{			
			if(-1 == begin)
				begin = curpos;			
			end = curpos;
		}
		curpos++;
	}
	ptr[end+1] = L'\0';

	if(-1 == begin)
		ptr[0] = L'\0';
	else if(0 != begin)
	{
		memmove(ptr, &ptr[begin], (end-begin+1)*sizeof(WCHAR));
		ZeroMemory(&ptr[end],begin*sizeof(WCHAR));
	}
}

//输出错误信息
void ProcessErrorMessage(TCHAR *szErrorText,BOOL bTrace)
{
	TCHAR *szTemp = NULL;
	LPVOID lpMsgBuf = NULL;
	__try
	{
		if (szErrorText == NULL)
			__leave;
		szTemp = new TCHAR[256];
		if (szTemp == NULL)
			__leave;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
			);

		_stprintf_s(szTemp,256, _T("警告:  %s 因以下错误而失败: \r\n%s\r\n"), szErrorText, (LPTSTR)lpMsgBuf);
		if (bTrace)
			TraceMsg(szTemp);
		else 
			MessageBox(NULL, szTemp, _T("应用程序错误"), MB_ICONSTOP);		
	}
	__finally
	{
		if (szTemp != NULL)
			delete[] szTemp;
		if (lpMsgBuf != NULL)
			LocalFree(lpMsgBuf);
	}
}

void GetLastErrorMessageA(IN LPCSTR szErrorText,OUT LPSTR szText,IN DWORD dwErrorCode)
{
	LPVOID lpMsgBuf = NULL;
	__try
	{
		if (NULL == szErrorText)
			__leave;
		if (NULL == szText)
			__leave;
		if (0 == dwErrorCode )
			dwErrorCode = GetLastError();
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dwErrorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPSTR) &lpMsgBuf,
			0,
			NULL);
		if (!szErrorText)
			sprintf(szText, "%s 因以下错误而失败:\r\n\t%s\r\n", szErrorText, (CHAR *)lpMsgBuf);
		else
			sprintf(szText, "发生错误:\r\n\t%s\r\n",  (CHAR *)lpMsgBuf);
		//_tcscpy(szErrorText,szText);
	}
	__finally
	{
		if (lpMsgBuf != NULL)
			LocalFree(lpMsgBuf);
	}
}

void GetLastErrorMessageW(IN LPCWSTR szErrorText,OUT LPWSTR szText,IN DWORD dwErrorCode)
{
	LPVOID lpMsgBuf = NULL;
	__try
	{
		if (NULL == szErrorText)
			__leave;
		if (NULL == szText)
			__leave;
		if (0 == dwErrorCode )
			dwErrorCode = GetLastError();
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dwErrorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPWSTR) &lpMsgBuf,
			0,
			NULL);
		if (!szErrorText)
			swprintf(szText, L"%s 因以下错误而失败:\r\n\t%s\r\n", szErrorText, (WCHAR *)lpMsgBuf);
		else
			swprintf(szText, L"发生错误:\r\n\t%s\r\n", (WCHAR *)lpMsgBuf);
	}
	__finally
	{
		if (lpMsgBuf != NULL)
			LocalFree(lpMsgBuf);
	}
}

void OutputBinA(UCHAR *szBin,UINT nLength,CHAR *szFileName,CHAR *szTitle)
{
	CHAR szTime[16] = {0};
	_strtime_s(szTime,16);
	if (szFileName == NULL || strlen(szFileName) == 0)
	{
		TraceMsgA("%s ",szTime);
		for (UINT i = 0; i < nLength;i ++)
			TraceMsgA("%02X ",szBin[i]);
		TraceMsgA("\r\n");		
	}
	else
	{
		HANDLE hLogFile = NULL;
		DWORD dwWritten = 0;
		CHAR szBuffer[64] = {0};
		__try
		{
			hLogFile = CreateFileA(szFileName,
				GENERIC_WRITE|GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_ARCHIVE,
				NULL);
			if (hLogFile == NULL) 
				return ;
			SetFilePointer(hLogFile,0,NULL,FILE_END);
			if (szTitle != NULL)			
				sprintf_s(szBuffer,64,"%s %s\r\n",szTime,szTitle);			
			else

				sprintf_s(szBuffer,64,"%s ",szTime);			
			WriteFile(hLogFile,szBuffer,strlen(szBuffer),&dwWritten,NULL);
			for (UINT i = 0;i < nLength; i ++)
			{
				sprintf_s(szBuffer,64,"%02X ",szBin[i]);				
				WriteFile(hLogFile,szBuffer,3,&dwWritten,NULL);
			}
			szBuffer[0] = 0x0D;
			szBuffer[1] = 0x0A;
			szBuffer[2] = 00;
			WriteFile(hLogFile,szBuffer,2,&dwWritten,NULL);
			FlushFileBuffers(hLogFile);	
			CloseHandle(hLogFile);
		}
		__except(1)
		{
			if (hLogFile != NULL)
				CloseHandle(hLogFile);
		}
	}
}

void OutputBinW(UCHAR *szBin,UINT nLength,WCHAR *szFileName,WCHAR *szTitle)
{
	WCHAR szTime[16] = {0};
	_wstrtime_s(szTime,16);
	if (szFileName == NULL || wcslen(szFileName) == 0)
	{
		TraceMsgW(L"%s ",szTime);
		for (UINT i = 0; i < nLength;i ++)
			TraceMsgW(L"%02X ",szBin[i]);
		TraceMsgW(L"\r\n");		
	}
	else
	{
		HANDLE hLogFile = NULL;
		DWORD dwWritten = 0;
		WCHAR szBuffer[64] = {0};
		__try
		{
			hLogFile = CreateFileW(szFileName,
				GENERIC_WRITE|GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_ARCHIVE,
				NULL);
			if (hLogFile == NULL) 
				return ;
			SetFilePointer(hLogFile,0,NULL,FILE_END);
			if (szTitle != NULL)			
				swprintf_s(szBuffer,64,L"%s %s\r\n",szTime,szTitle);			
			else
				swprintf_s(szBuffer,64,L"%s ",szTime);			
			WriteFile(hLogFile,szBuffer,wcslen(szBuffer)*sizeof(WCHAR),&dwWritten,NULL);
			for (UINT i = 0;i < nLength; i ++)
			{
				swprintf_s(szBuffer,64,L"%02X ",szBin[i]);				
				WriteFile(hLogFile,szBuffer,3*sizeof(WCHAR),&dwWritten,NULL);
			}
			szBuffer[0] = 0x0D;
			szBuffer[1] = 0x0A;
			szBuffer[2] = 00;
			WriteFile(hLogFile,szBuffer,2*sizeof(WCHAR),&dwWritten,NULL);
			FlushFileBuffers(hLogFile);	
			CloseHandle(hLogFile);
		}
		__except(1)
		{
			if (hLogFile != NULL)
				CloseHandle(hLogFile);
		}
	}
}

byte Char2Hex(unsigned char nCh)
{
	toupper(nCh);
	if((nCh >= '0') && (nCh <= '9'))	
		return (nCh - '0');	
	else if	((nCh >= 'A') && (nCh <= 'F'))
		return (nCh - 'A' + 10);
	else
		return 0;
}

#ifdef _UNICODE
void SetDlgItemTextA2W(HWND hWnd,UINT nID,LPSTR szText)
{
	if (hWnd == NULL)
		return ;
	if (!IsWindow(hWnd))
		return;
	WCHAR	*wszText = new WCHAR [strlen(szText) + 1];
	MultiByteToWideChar( CP_ACP, 0, szText,strlen(szText) + 1, wszText,strlen(szText) + 1);
	::SetDlgItemText(hWnd,nID,wszText);	
	delete []wszText;
}
void GetDlgItemTextW2A(HWND hWnd,UINT nID,LPSTR szText,int nBuffLen)
{
	if (hWnd == NULL)
		return ;
	if (!IsWindow(hWnd))
		return;
	TCHAR szDlgText[255] = {0};
	GetDlgItemText(hWnd,nID,szDlgText,255);
	WideCharToMultiByte(CP_ACP,0,szDlgText,255,(LPSTR)szText,nBuffLen,NULL,NULL);

}
#endif

void ScrollEditW(HWND hEdit,WCHAR *pFormat,...)
{
	va_list args;
	va_start(args, pFormat);
	int nBuff;
	WCHAR szBuffer[0xFFFF];
	nBuff = _vsnwprintf(szBuffer, __countof(szBuffer), pFormat, args);
	va_end(args);

	PDWORD_PTR dwResult = 0;
	int nLineCount = ::SendMessageW(hEdit,EM_GETLINECOUNT,0,0);
	if (nLineCount >= 4096)
	{
		::SendMessageW(hEdit,WM_SETTEXT,0,(LPARAM)szBuffer);
		return ;
	}
	if (SendMessageTimeoutW(hEdit, WM_GETTEXTLENGTH, 0, 0, SMTO_NORMAL, 1000L, dwResult) != 0)
	{
		int nLen = (int) dwResult;
		if (SendMessageTimeoutW(hEdit, EM_SETSEL, nLen, nLen, SMTO_NORMAL, 1000L, dwResult) != 0)		
			SendMessageTimeoutW(hEdit, EM_REPLACESEL, FALSE, (LPARAM)szBuffer, SMTO_NORMAL, 1000L, dwResult);		
	}
}

void ScrollEditA(HWND hEdit,char *pFormat,...)
{
	va_list args;
	va_start(args, pFormat);
	int nBuff;
	
	CHAR szBuffer[0xffff];
	nBuff = _vsnprintf(szBuffer, __countof(szBuffer), pFormat, args);	
	va_end(args);

	PDWORD_PTR dwResult = 0;
	int nLineCount = ::SendMessageA(hEdit,EM_GETLINECOUNT,0,0);
	if (nLineCount >= 4096)
	{
		::SendMessageA(hEdit,WM_SETTEXT,0,(LPARAM)szBuffer);
		return ;
	}
	if (SendMessageTimeoutA(hEdit, WM_GETTEXTLENGTH, 0, 0, SMTO_NORMAL, 1000L, dwResult) != 0)
	{
		int nLen = (int) dwResult;
		if (SendMessageTimeoutA(hEdit, EM_SETSEL, nLen, nLen, SMTO_NORMAL, 1000L, dwResult) != 0)		
			SendMessageTimeoutA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)szBuffer, SMTO_NORMAL, 1000L, dwResult);		
	}
}

bool IsHexDigitA(CHAR nCh)
{
	toupper(nCh);
	if(((nCh >= '0') && (nCh <= '9')) ||((nCh >= 'A') && (nCh <= 'F')))
		return true;
	else
		return false;
}

bool IsHexDigitW(WCHAR nCh)
{
	towupper(nCh);
	if(((nCh >= _T('0')) && (nCh <= _T('9'))) ||((nCh >= _T('A')) && (nCh <= _T('F'))))
		return true;
	else
		return false;
}

#define		IN
#define		OUT

//把a转换为Hex字符
#define TOHEXW(a, b) {*b++ = chHexTableW[a >> 4]; *b++ = chHexTableW[a & 0xf];}

// 功能描述		内存数据转换为16进制ASCII字符串
// pHex			输入数据流
// nHexLen		输入数据流长度
// szAscString	输出16进制ASCII字符串缓冲区
// nBuffLen		输出缓冲区最大长度

// 返回值		<0时 输入参数不合法
//				>0 返回转换后的ASCII符串的长度
int Hex2AscStringW(IN byte *pHex,IN int nHexLen,OUT WCHAR *szAscString,IN int nBuffLen,WCHAR chSeperator)
{	
	static const  char chHexTableW[] = {L'0',L'1',L'2',L'3',L'4',L'5',L'6',L'7',L'8',L'9',L'A',L'B',L'C',L'D',L'E',L'F'};
	if (!pHex ||
		!szAscString||
		!nBuffLen)
		return -1;
	byte nMult = 3;
	if (chSeperator == L'\0')
		nMult = 2;
	if (nHexLen*nMult > nBuffLen)
		return -1;
	WCHAR *p = &szAscString[0];

	int n;
	for (n = 0; n < nHexLen ; n++)
	{
		TOHEXW(pHex[n], p);
		if (nMult == 2)
			continue;
		*p++ = chSeperator;
	}
	return n*nMult;
}

//把a转换为Hex字符
#define TOHEXA(a, b) {*b++ = chHexTableA[a >> 4]; *b++ = chHexTableA[a & 0xf];}
// 功能描述		内存数据转换为16进制ASCII字符串
// pHex			输入数据流
// nHexLen		输入数据流长度
// szAscString	输出16进制ASCII字符串缓冲区
// nBuffLen		输出缓冲区最大长度

// 返回值		<0时 输入参数不合法
//				>0 返回转换后的ASCII符串的长度
int Hex2AscStringA(IN byte *pHex,IN int nHexLen,OUT CHAR *szAscString,IN int nBuffLen,CHAR chSeperator)
{	
	static const  char chHexTableA[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	if (!pHex ||
		!szAscString||
		!nBuffLen)
		return -1;
	byte nMult = 3;
	if (chSeperator == '\0')
		nMult = 2;
	if (nHexLen*nMult > nBuffLen)
		return -1;
	CHAR *p = &szAscString[0];

	int n;
	for (n = 0; n < nHexLen ; n++)
	{
		TOHEXA(pHex[n], p);
		if (nMult == 2)
			continue;
		*p++ = chSeperator;
	}
	return n*nMult;
}



#define		Char2DigitW(ch)	(ch > L'9'?ch - L'A' + 10:ch - L'0')
#define		UpcasecharW(ch)	(ch >= L'A'?ch:((ch - L'a') + L'A'))
// 功能描述			16进制ASCII字符串转为用内存数据
// szAscString		输入16进制ASCII字符串
// nAscStringLen	输入数据长度
// pHex				输出缓冲区
// nBuffLen			输出缓冲区最大长度
// 返回值			<0时 输入参数不合法
//					>0 返回转换后pHex数据的长度
int AscString2HexW(IN WCHAR *szAscString,IN int nAscStringLen,OUT byte *pHex,IN int nBuffLen,WCHAR chSeperator)
{//							
	static WCHAR *AscTable = L"0123456789ABCDEF";
	if (!szAscString ||
		!pHex||
		!nBuffLen)
		return -1;
	byte nMult = 3;
	if (chSeperator == L'\0')
		nMult = 2;

	if (nAscStringLen /nMult > nBuffLen)
		return -1;

	int nCount = 0;
	WCHAR ch;
	for (int i = 0;i < nAscStringLen;i += nMult)
	{		
		ch = UpcasecharW(szAscString[i]);
		byte nHi = Char2DigitW(ch);
		ch = UpcasecharW(szAscString[i + 1]);
		byte nLo = Char2DigitW(ch);
		pHex[nCount ++] = (nHi &0x0F)<<4|(nLo &0x0F);
	}
	return nCount;
}

#define		Char2DigitA(ch)	(ch > '9'?(ch - 'A' + 10):ch - '0')
#define		UpcasecharA(ch)	( ch >= 'A'?ch:(ch - 'a' + 'A'))
// 功能描述			16进制ASCII字符串转为用内存数据
// szAscString		输入16进制ASCII字符串
// nAscStringLen	输入数据长度
// pHex				输出缓冲区
// nBuffLen			输出缓冲区最大长度
// 返回值			<0时 输入参数不合法
//					>0 返回转换后pHex数据的长度
int AscString2HexA(IN CHAR *szAscString,IN int nAscStringLen,OUT byte *pHex,IN int nBuffLen,CHAR chSeperator)
{//							
	static CHAR *AscTable = "0123456789ABCDEF";
	if (!szAscString ||
		!pHex||
		!nBuffLen)
		return -1;
	byte nMult = 3;
	if (chSeperator == '\0')
		nMult = 2;

	if (nAscStringLen /nMult > nBuffLen)
		return -1;

	int nCount = 0;
	CHAR ch;
	for (int i = 0;i < nAscStringLen;i += nMult)
	{		
		ch = UpcasecharA(szAscString[i]);
		byte nHi = Char2DigitA(ch);
		ch = UpcasecharA(szAscString[i + 1]);
		byte nLo = Char2DigitA(ch);
		pHex[nCount ++] = (nHi &0x0F)<<4|(nLo &0x0F);
	}
	return nCount;
}

bool IsValidIPAddressA(LPCSTR szAddress)
{
	if (szAddress == NULL)
		return false;
	int nDot = 0;
	int nLen = strlen(szAddress);
	if (nLen <7 || nLen > 15)
		return false;
	for (int i = 0;i< nLen;i ++)
	{
		if (szAddress[i] == _T('.'))
			nDot ++;
	}

	if (nDot <3)
		return false;
	ULONG nIpValue = 0;
	nIpValue = inet_addr(szAddress);
	if (nIpValue == INADDR_NONE ||nIpValue == 0)
		return false;
	else
		return true;
}

bool IsValidIPAddressW(LPCWSTR szAddress)
{
	if (szAddress == NULL)
		return false;
	int nDot = 0;
	int nLen = wcslen(szAddress);
	if (nLen <7 || nLen > 15)
		return false;
	for (int i = 0;i< nLen;i ++)
	{
		if (szAddress[i] == _T('.'))
			nDot ++;
	}

	if (nDot <3)
		return false;
	ULONG nIpValue = 0;
	nIpValue = inet_addrW(szAddress);
	if (nIpValue == INADDR_NONE ||nIpValue == 0)
		return false;
	else
		return true;
}

bool IsDomainA(CHAR *szDomain)
{
	if (szDomain == NULL)
		return false;
	while(*szDomain != '\0')
	{
		if (isalpha(*szDomain))
		{
			return true;
		}
		else
			szDomain ++;
	}
	return false;
}
bool IsDomainW(WCHAR *szDomain)
{
	if (szDomain == NULL)
		return false;
	while(*szDomain != L'\0')
	{
		if (iswalpha(*szDomain))
		{
			return true;
		}
		else
			szDomain ++;
	}
	return false;
}

bool IsNumericA(char *szText,int nSize,int nTrans)
{
	if (nTrans  == 10)
	{
		if (nSize >0)
			for (int i = 0; i < nSize; i ++)
			{
				if ((unsigned char)szText[i] > '9')
					return false;
			}
		else
			return false;		
	}
	return true;
}

bool IsNumericW(WCHAR *szText,int nSize,int nTrans)
{
	if (nTrans  == 10)
	{
		if (nSize >0)
			for (int i = 0; i < nSize; i ++)
			{
				if ((WCHAR)szText[i] > L'9')
					return false;
			}
		else
			return false;		
	}
	return true;
}

//字符串转长整型数
//szNumber		输入的字符串
//Trans			转换的进制，取值范围为2到16,默认为10
//nNumLength	要转换的字符长度,为0时转换整个字符串
long strtolong(LPCSTR szNumber,int Trans ,int nNumLength)
{
	if (szNumber == NULL)
		return 0;
	if (strlen(szNumber) == 0)
		return 0;
	int nLength = strlen(szNumber);
	if ((nNumLength > 0) && (nNumLength <= nLength))
		nLength = nNumLength;
	long nRet = 0;
	int iStartPos = 0;
	if (szNumber[0] == '-')
		iStartPos = 1;

	for (int i = iStartPos; i < nLength ; i ++)
	{

		if (Trans <=10)
		{
			if (szNumber[i] >= '0' && szNumber[i] <= '9')
			{
				nRet *= Trans;
				nRet += (szNumber[i] - '0');

			}
			else			
				return (iStartPos == 0)?nRet:(0-nRet);
		}
		else
		{
			char UpperChar = (char)toupper(szNumber[i]);			
			if (UpperChar >= '0' && UpperChar <= '9') 
			{
				nRet *= Trans;
				nRet += (szNumber[i] - '0');

			}
			else if (UpperChar >= 'A' && UpperChar <= 'F')
			{
				nRet *= Trans;
				nRet += (UpperChar - 'A' + 10);

			}
			else
				return (iStartPos == 0)?nRet:(0-nRet);
		}
	}
	return (iStartPos == 0)?nRet:(0-nRet);
}
long wcstolong(LPCWSTR szNumber,int Trans,int nNumLength)
{
	if (szNumber == NULL)
		return 0;
	if (wcslen(szNumber) == 0)
		return 0;
	int nLength = wcslen(szNumber);
	if ((nNumLength > 0) && (nNumLength <= nLength))
		nLength = nNumLength;
	long nRet = 0;
	int iStartPos = 0;
	if (szNumber[0] == L'-')
		iStartPos = 1;

	for (int i = iStartPos; i < nLength ; i ++)
	{
		nRet *= Trans;
		if (Trans <=10)
		{
			if (szNumber[i] >= L'0' && szNumber[i] <= L'9')
				nRet += (szNumber[i] - L'0');
			else
			{
				return nRet;
			}
		}
		else
		{
			WCHAR UpperChar = towupper(szNumber[i]);			
			if (UpperChar >= L'0' && UpperChar <= L'9') 
				nRet += (szNumber[i] - L'0');
			else if (UpperChar >= L'A' && UpperChar <= L'F')
				nRet += (UpperChar - L'A' + 10);
			else
				return nRet;
		}
	}
	return (iStartPos == 0)?nRet:(0-nRet);
}

int FindChr(TCHAR *szString,TCHAR ch)
{
	TCHAR *pDest = _tcschr(szString,ch);
	if (!pDest )
		return -1;
	return (pDest - szString);
}

int FindStr(TCHAR *szString,TCHAR *str)
{
	TCHAR *pDest = _tcsstr(szString,str);
	if (!pDest )
		return -1;
	return (pDest - szString);
}
//非查表CRC16算法
unsigned short cal_crc(unsigned char *ptr, unsigned short len) 
{ 
	unsigned char i; 
	unsigned short crc=0; 
	while(len--!=0) 
	{  
		for(i=0x80; i!=0; i/=2)
		{ 
			if((crc&0x8000)!=0)
			{
				crc*=2; crc^=0x1021;
			} /* 余式CRC乘以2再求CRC */ 
			else
				crc*=2; 
			if((*ptr&i)!=0)
				crc^=0x1021; /* 再加上本位的CRC */ 
		} 
		ptr++; 
	} 
	return(crc); 
} 

static UINT const CRCTABLE [ 256 ] =
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
} ;

WORD CALC_CRC16 (unsigned char *lpszBuff,  WORD Len,WORD nInitValue)
{
	byte da;
	WORD ii;
	WORD crchl;

	crchl = nInitValue ;
	for ( ii = 0x00 ; ii < Len ; ii ++ )
	{		
		da = (byte)( crchl / 256 ) ;                   
		crchl <<= 0x08 ;                        
		crchl ^= CRCTABLE [ da ^ lpszBuff[ii]] ;  
	}
	return crchl;
}

BYTE XOR_Sum(unsigned char *ptr,int nLen)
{
	int nSum = *ptr;
	for (int i = 1;i < nLen;i ++)	
		nSum ^= ptr[i];
	//TraceMsg(_T("XorSum = %d\r\n"),(BYTE)nSum);
	return (BYTE)nSum;
}
//重启WINCE系统
//nBootCode为0时热启动,其它值时为冷启动
// void WINAPI Reboot (DWORD  nBootMode)
// {
// 	int   inVal;
// 	BYTE  outBuf[1024];
// 	DWORD bytesReturned;
// 	HINSTANCE  hCoreDll = LoadLibrary(_T("Coredll.dll"));
// 	if (hCoreDll != NULL)
// 	{
// 		KernelIoControlProc KernelIoControl =  (KernelIoControlProc)GetProcAddress(hCoreDll,_T("KernelIoControl"));	
// 		if (KernelIoControl != NULL)
// 		{
// 			if(nBootMode == 0)
// 				KernelIoControl(IOCTL_HAL_REBOOT, &inVal, 4, outBuf, 1024, &bytesReturned);   //热启动
// 			else
// 				KernelIoControl(IOCTL_HAL_COLDBOOT, &inVal, 4, outBuf, 1024, &bytesReturned); //冷启动
// 		}
// 	}
// }

//条件延时函数,当hEvent重置或等待时间超过dwTime ms时，函数返回
void EventDelay(HANDLE hEvent,DWORD dwTime)
{
	DWORD dwStart = GetTickCount();
	if (hEvent != NULL)
	{
		while(WaitForSingleObject(hEvent,0) == WAIT_TIMEOUT)
		{
			MSG msg;
			//响应其它消息的处理
			while(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			if(GetTickCount() - dwStart > dwTime)
				return;
			::Sleep(1);
		}
	}
	else while(1)		
	{
		MSG msg;
		//响应其它消息的处理
		while(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if(GetTickCount() - dwStart > dwTime)
			return;
		::Sleep(1);
	}
}

int Mystrlen(LPCTSTR str)
{
	if (str == NULL)
		return 0;
	int nLen = 0;
	while (*str!= NULL)
	{
		str ++;
		nLen  ++;
	}
	return nLen;
}
int Mystrcmp(LPCTSTR srcstr,LPCTSTR dststr)
{
	if (srcstr == NULL || dststr == NULL)
		return -1;
	int nsrclen = _tcslen(srcstr);
	int ndstlen = _tcslen(dststr);
	if (ndstlen == nsrclen)
	{
		for (int i = 0;i < nsrclen; i ++)
		{
			if (*srcstr ++ != *dststr ++)
				return -1;
		}
		return 0;
	}
	else 
		return -1;
}
int Mystrcpy(LPTSTR dststr,LPCTSTR srcstr)
{
	if (srcstr == NULL || dststr == NULL)
		return -1;
	int nsrclen = _tcslen(srcstr);
	memcpy(dststr,srcstr,nsrclen*sizeof(TCHAR));
	return nsrclen;

}
int Mystrcat(LPTSTR  dststr,LPCTSTR srcstr)
{
	if (srcstr == NULL || dststr == NULL)
		return -1;
	int nsrclen = _tcslen(srcstr);
	int ndstlen = _tcslen(dststr);
	memcpy(dststr+ndstlen,srcstr,nsrclen*sizeof(TCHAR));
	return (nsrclen + ndstlen);
}

int StrReserverFind(LPCSTR srcstr,CHAR ch)
{
	LPSTR lpsz = (LPSTR)strrchr(srcstr, ch);
	return (lpsz == NULL) ? -1 : (int)(lpsz - srcstr);
}

int WcsReserverFind(LPCWSTR srcstr,WCHAR ch)
{
	LPWSTR lpsz = (LPWSTR)wcsrchr(srcstr, ch);
	return (lpsz == NULL) ? -1 : (int)(lpsz - srcstr);

}

void GetAppPathA(CHAR *szPath,int nBuffLen)
{
	int nPos;
	CHAR	szTempPath[MAX_PATH] = {0};
	::GetModuleFileNameA(NULL,szTempPath,MAX_PATH);
	nPos = StrReserverFind(szTempPath,_T('\\'));
#ifdef _DEBUG
	if (nBuffLen <=0 )
		assert(true);
#endif
	strncpy_s(szPath,nBuffLen,szTempPath,nPos);
}

void GetAppPathW(WCHAR *szPath,int nBuffLen)
{
	int nPos;
	WCHAR	szTempPath[MAX_PATH] = {0};
	::GetModuleFileNameW(NULL,szTempPath,MAX_PATH);
	nPos = WcsReserverFind(szTempPath,_T('\\'));
#ifdef _DEBUG
	if (nBuffLen <=0 )
		assert(true);
#endif
	wcsncpy_s(szPath,nBuffLen,szTempPath,nPos);
}

int  A2WHelper(IN LPCSTR lpa,OUT LPWSTR lpw,IN int nWcharBuffLen)
{
	assert(lpa != NULL);
	if (lpw == NULL)
		return 0;

	int nNeedBuffSize = ::MultiByteToWideChar(CP_ACP, NULL, lpa, -1, NULL, 0);
	if (!lpw || !nWcharBuffLen)
		return nNeedBuffSize;
	assert(nWcharBuffLen >= (nNeedBuffSize + 1));
	if (nWcharBuffLen < nNeedBuffSize + 1)
		return 0;
	return MultiByteToWideChar(CP_ACP, 0, lpa, -1, lpw, nWcharBuffLen);
}

int  W2AHelper(IN LPCWSTR lpw,OUT LPSTR lpa, IN int nCharBuffLen)
{
	assert(lpw != NULL);
	if (lpa == NULL)
		return 0;
	
	int nNeedBuffLen = ::WideCharToMultiByte(CP_ACP, NULL, lpw,-1 , NULL, 0, NULL, NULL);
	if (!lpa || !nCharBuffLen)
		return nNeedBuffLen;
	assert(nCharBuffLen >= (nNeedBuffLen + 1));
	if (nCharBuffLen < nNeedBuffLen + 1)
		return 0;
	return WideCharToMultiByte(CP_ACP, 0, lpw, -1, lpa, nCharBuffLen, NULL, NULL);
}

int W2UTF8Helper(IN LPCWSTR lpw,OUT LPSTR lpu,IN int nUTFBuffLen)
{	
	assert(lpw != NULL);
	if (lpw == NULL)
		return 0;
	int nNeedBuffSize = ::WideCharToMultiByte(CP_UTF8, NULL, lpw, -1, NULL, 0, NULL, NULL);
	if  (!lpu || !nUTFBuffLen)
		return nNeedBuffSize;
	
	assert(nUTFBuffLen >= (nNeedBuffSize + 1));
	if (nUTFBuffLen < nNeedBuffSize + 1)
		return 0;
	return  ::WideCharToMultiByte(CP_UTF8, NULL, lpw, -1, lpu, nUTFBuffLen, NULL, NULL);
}

int UTF82WHelper(IN LPCSTR lpu, OUT LPWSTR lpw, IN int nWcharBuffLen)
{	
	assert(lpu != NULL);
	if (lpu == NULL)
		return 0;

	int nNeedBuffSize = ::MultiByteToWideChar(CP_UTF8, NULL, lpu, -1, NULL, 0);
	if (!lpw || !nWcharBuffLen)
		return nNeedBuffSize;
	assert(nWcharBuffLen >= (nNeedBuffSize + 1));
	if (nWcharBuffLen < (nNeedBuffSize + 1))
		return 0;
	return ::MultiByteToWideChar(CP_UTF8, 0, lpu, -1, lpw, nWcharBuffLen);
}

int ANSI2UTF8(IN const CHAR *szAnsi,OUT char *szUTF8,IN int nBuffSize)
{	
	if (!szAnsi)
	{
		assert(false);
		return 0;
	}
	
	int nLenW = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, -1, NULL, 0);
	stdshared_ptr<WCHAR >szUnicode( new WCHAR[nLenW + 1]);
	MultiByteToWideChar(CP_ACP, 0, szAnsi, -1, szUnicode.get(), nLenW + 1);
	if (!szUTF8 )
		return ::WideCharToMultiByte(CP_UTF8, NULL, szUnicode.get(),nLenW , NULL, 0, NULL, NULL);
	else 
		return ::WideCharToMultiByte(CP_UTF8, NULL, szUnicode.get(), nLenW, szUTF8, nBuffSize, NULL, NULL);
}

int UTF82ANSI(IN const CHAR *szUTF8 ,OUT char *szAnsi,IN int nBuffSize)
{	
	if (!szUTF8)
	{
		assert(false);
		return 0;
	}
	int nLenW = ::MultiByteToWideChar(CP_UTF8, NULL, szUTF8, -1, NULL, 0);
	stdshared_ptr<WCHAR >szUnicode( new WCHAR[nLenW + 1]);
	::MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, szUnicode.get(), nLenW + 1);

	if (!szAnsi || !nBuffSize)
		return ::WideCharToMultiByte(CP_ACP, NULL, szUnicode.get(),nLenW , NULL, 0, NULL, NULL);
	else
		return ::WideCharToMultiByte(CP_ACP, 0, szUnicode.get(), nLenW, szAnsi, nBuffSize, NULL, NULL);
}

long SafeMemcpy(void *pSrcAddr, const void *pDestAddr, size_t nSize)
{
	//检查输入内存的正确性
	//if (IsBadWritePtr((void *)pSrcAddr,nSize)) 
	//	 __leave;
	//if (IsBadReadPtr((void *)pDestAddr,nSize))
	//	 __leave;
	__try
	{
		memcpy(pSrcAddr,pDestAddr,nSize);
		return S_OK;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		TraceMsg(_T("访问内存时发生异常\r\n"));
		return S_FALSE;
	}
}

int AnalysisDataA(char *szSrc,int nSrcLen,char *szText,int &nTextLen,char Flag)
{
	if (nSrcLen >0)
	{
		int i = 0;
		for (i = 0; i < nSrcLen;i ++)
		{
			if (szSrc[i] == Flag)
				break;
		}
		nTextLen = i;
		SafeMemcpy(szText,szSrc,nTextLen);
		return (nSrcLen - nTextLen);
	}
	else
		return 0;
}

int AnalysisDataW(WCHAR *szSrc,int nSrcLen,WCHAR *szText,int &nTextLen,WCHAR Flag)
{
	if (nSrcLen >0)
	{
		int i = 0;
		for (i = 0; i < nSrcLen;i ++)
		{
			if (szSrc[i] == Flag)
				break;
		}
		nTextLen = i;
		SafeMemcpy(szText,szSrc,nTextLen*sizeof(WCHAR));
		return (nSrcLen - nTextLen);
	}
	else
		return 0;
}


byte	*UINT642Byte(UINT64 nValue64)
{
	static byte szValue[8] = {0};
	byte *pByte = (byte *)&nValue64;
	for (int i = 0;i < 8;i ++)
		szValue[i] = pByte[7 - i];
	return (byte *)szValue;
}

UINT64  Byte2UINT64(byte *pByte)
{
	DWORD nLo = MAKELONG(MAKEWORD(pByte[7],pByte[6]),MAKEWORD(pByte[5],pByte[4]));
	DWORD nHi = MAKELONG(MAKEWORD(pByte[3],pByte[2]),MAKEWORD(pByte[1],pByte[0]));
	return MAKEUINT64(nLo,nHi);
}

byte *UINT2Byte(UINT nValue32)
{
	static byte szValue[4] = {0};
	byte *pByte = (byte *)&nValue32;
	for (int i = 0;i < 4;i ++)
		szValue[i] = pByte[3 - i];
	return (byte *)szValue;

}
UINT Byte2UINT(byte *pByte)
{
	WORD nLo = MAKEWORD(pByte[3],pByte[2]);
	WORD nHi = MAKEWORD(pByte[1],pByte[0]);
	return MAKELONG(nLo,nHi);
}

//KMP匹配算法模式串的预处理
//返回true,执行成功
//返回false,则为pNext空间不足
bool GetNext(byte *szKey,int nKeyLength,int *pNext,int nNextLength)
{  
	int i = 0,j = -1;  
	pNext[0] = -1; 
	if (nKeyLength <=0 )
		nKeyLength = strlen((CHAR *)szKey);
	while( i< nKeyLength)
	{  
		if ( j== -1 || szKey[i] == szKey[j])
		{  
			i ++;  
			j ++;  
			if (szKey[i] != szKey[j])
			{	
				if (i < nNextLength)
					pNext[i] = j;
				else
					return false;
			}  
			else 
			{  
				if (i < nNextLength)
					pNext[i] = pNext[j]; 
				else
					return false;
			}  
		}  
		else
		{  
			j = pNext[j];  
		}  
	}  
	return true;
}
//KMP字符串匹配算法
//返回值大于0,匹配成功
//返回值小于0,匹配失败
int KMP_StrFind(BYTE *szSource,int nSourceLength,BYTE *szKey,int nKeyLength)
{   
	int Next[1024] = {0};
	int i = 0,j = 0; 
	if (nSourceLength <= 0)
		nSourceLength = strlen((CHAR *)szSource); 
	if (nKeyLength <= 0)
		nKeyLength = strlen((CHAR *)szKey); 
	if (!GetNext(szSource,nKeyLength,Next,1024))
		return -1;
	while(i < nSourceLength && j < nKeyLength)
	{  
		if(j == -1 || szSource[i] == szKey[j])
		{  
			i ++;  
			j ++;  
		}  
		else 
		{ //若发生不匹配,模式串向右滑动,继续与父串匹配,父串指针无须回溯
			j = Next[j]; 
		}  
	}  
	if(j == nKeyLength)
	{ //匹配成功,返回位置
		return i-nKeyLength;  
	}  
	else 
		return -1;  
}  

//二分查找算法，时间复杂度为Log2(n)
//Array		已经排序好数据
//Array		中元素的数量
//nKey		要查找的数据
int BinarySearch(int *Array,int nCount,int nKey)
{
	int nLeft = 0;				//left var
	int nRight = nCount-1;		//right var
	int nMiddle;
	int nValue = -1;
	while(nLeft <= nRight)
	{
		nMiddle = (nLeft + nRight)/2;
		if(nKey < Array[nMiddle])
			nRight = nMiddle - 1;
		else if (nKey > Array[nMiddle])
			nLeft = nMiddle + 1;
		else if (nKey == Array[nMiddle])
		{
			nValue = nMiddle;
			break;
		}
	}
	return nValue;
}

UINT RangedRand( UINT range_min, UINT range_max)
{
	// Generate random numbers in the half-closed interval
	// [range_min, range_max). In other words,
	// range_min <= random number < range_max
	return  (UINT)((double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min);
}
double RangedRand( double range_min, double range_max)
{
	// Generate random numbers in the half-closed interval
	// [range_min, range_max). In other words,
	// range_min <= random number < range_max
	return  ((double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min);
}

bool MemMerge(INOUT char **pDestBuff,INOUT int &nDestBuffLength,INOUT int &nDestBuffSize,IN char *szTempBuff,IN int nTempLen)
{
	if (!pDestBuff || !szTempBuff || !nDestBuffSize|| !nTempLen)
		return false;
	int nNewBufferLength = nTempLen + nDestBuffLength;
	if (nNewBufferLength <= nDestBuffSize)
	{
		memcpy(&(*pDestBuff)[nDestBuffLength],szTempBuff,nTempLen);
		nDestBuffLength = nNewBufferLength;		
	}
	else
	{
		int nNewBufferSize = nDestBuffSize;
		while(nNewBufferSize < nNewBufferLength)
			nNewBufferSize*=2;

		char *pTemp = new char[nNewBufferSize];
		if (pTemp == NULL)
			return false;
		memcpy(pTemp,*pDestBuff,nDestBuffLength);
		memcpy(&pTemp[nDestBuffLength],szTempBuff,nTempLen);
		delete [](*pDestBuff);
		*pDestBuff = pTemp;		
		nDestBuffLength = nNewBufferLength;
		nDestBuffSize = nNewBufferSize;
	}
	return true;
}


bool SetAutoRunA(LPCSTR strAutoRunItem,LPCSTR szAppPath)
{
	HKEY  hKey = NULL;
	DWORD retCode = 0;	

	bool bResult = false;
	__try
	{
		HKEY  hKey = NULL;
		DWORD retCode = 0;
		LPCSTR szMainSubKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
		retCode = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szMainSubKey,0,KEY_ALL_ACCESS,&hKey);
		switch(retCode)
		{
		case ERROR_SUCCESS:
			{
				retCode = RegSetValueExA(hKey,strAutoRunItem,NULL,REG_SZ,(LPBYTE)&szAppPath[0],sizeof(TCHAR)*strlen(szAppPath));	
				if (retCode == ERROR_SUCCESS)
					bResult = true;
				__leave;
			}			
			break;
		case ERROR_ACCESS_DENIED:
			{
				//MessageBox(NULL,_T("访问注册表被拒绝,请以管理身份重新运行一次."),_T("提示"),MB_OK|MB_ICONSTOP);
				return false;
			}
			break;
		default:
			{
				CHAR szErrorText[255] = {0};			 
				GetLastErrorMessageA("Call RegCreateKeyEx:",szErrorText,retCode);
				TraceMsgA(szErrorText);
				bResult = false;
			}
			break;
		}
	}
	__finally
	{
		if (hKey != NULL)
			RegCloseKey(hKey);
	}
	return bResult;
}

bool SetAutoRunW(LPCWSTR strAutoRunItem,LPCWSTR szAppPath)
{
	HKEY  hKey = NULL;
	DWORD retCode = 0;	

	bool bResult = false;
	__try
	{
		HKEY  hKey = NULL;
		DWORD retCode = 0;
		LPCWSTR szMainSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
		retCode = RegOpenKeyExW(HKEY_LOCAL_MACHINE,szMainSubKey,0,KEY_ALL_ACCESS,&hKey);
		switch(retCode)
		{
		case ERROR_SUCCESS:
			{
				retCode = RegSetValueExW(hKey,strAutoRunItem,NULL,REG_SZ,(LPBYTE)&szAppPath[0],sizeof(TCHAR)*wcslen(szAppPath));	
				if (retCode == ERROR_SUCCESS)
					bResult = true;
				__leave;
			}			
			break;
		case ERROR_ACCESS_DENIED:
			{
				//MessageBox(NULL,_T("访问注册表被拒绝,请以管理身份重新运行一次."),_T("提示"),MB_OK|MB_ICONSTOP);
				return false;
			}
			break;
		default:
			{
				WCHAR szErrorText[255] = {0};			 
				GetLastErrorMessageW(L"Call RegCreateKeyEx:",szErrorText,retCode);
				TraceMsgW(szErrorText);
				bResult = false;
			}
			break;
		}
	}
	__finally
	{
		if (hKey != NULL)
			RegCloseKey(hKey);
	}
	return bResult;
}

bool DeleteAutoRunA(LPCSTR strAutoRunItem)
{
	HKEY  hKey = NULL;
	DWORD retCode = 0;	

	bool bResult = false;
	__try
	{
		HKEY  hKey = NULL;
		DWORD retCode = 0;
		LPCSTR szMainSubKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
		retCode = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szMainSubKey,0,KEY_ALL_ACCESS,&hKey);
		switch(retCode)
		{
		case ERROR_SUCCESS:
			{
				retCode = RegDeleteValueA(hKey,strAutoRunItem);				
				if (retCode == ERROR_SUCCESS)
					bResult = true;
				__leave;
			}			
			break;
		case ERROR_ACCESS_DENIED:
			{
				//MessageBox(NULL,L"访问注册表被拒绝,请以管理身份重新运行一次.",L"提示",MB_OK|MB_ICONSTOP);
				return false;
			}
			break;
		default:
			{
				CHAR szErrorText[255] = {0};			 
				GetLastErrorMessageA("Call RegCreateKeyEx:",szErrorText,retCode);
				TraceMsgA(szErrorText);
				bResult = false;
			}
			break;
		}
	}
	__finally
	{
		if (hKey != NULL)
			RegCloseKey(hKey);
	}
	return bResult;
}


bool DeleteAutoRunW(LPCWSTR strAutoRunItem)
{
	HKEY  hKey = NULL;
	DWORD retCode = 0;	

	bool bResult = false;
	__try
	{
		HKEY  hKey = NULL;
		DWORD retCode = 0;
		LPCWSTR szMainSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
		retCode = RegOpenKeyExW(HKEY_LOCAL_MACHINE,szMainSubKey,0,KEY_ALL_ACCESS,&hKey);
		switch(retCode)
		{
		case ERROR_SUCCESS:
			{
				retCode = RegDeleteValueW(hKey,strAutoRunItem);				
				if (retCode == ERROR_SUCCESS)
					bResult = true;
				__leave;
			}			
			break;
		case ERROR_ACCESS_DENIED:
			{
				//MessageBox(NULL,L"访问注册表被拒绝,请以管理身份重新运行一次.",L"提示",MB_OK|MB_ICONSTOP);
				return false;
			}
			break;
		default:
			{
				WCHAR szErrorText[255] = {0};			 
				GetLastErrorMessageW(L"Call RegCreateKeyEx:",szErrorText,retCode);
				TraceMsgW(szErrorText);
				bResult = false;
			}
			break;
		}
	}
	__finally
	{
		if (hKey != NULL)
			RegCloseKey(hKey);
	}
	return bResult;
}


bool IsAutoRunA(LPCSTR strAutoRunItem,bool &bAutoRun)
{
	HKEY  hKey = NULL;
	DWORD retCode = 0;	
	bool bResult = false;
	bAutoRun = false;
	__try
	{
		HKEY  hKey = NULL;
		DWORD retCode = 0;
		LPCSTR szMainSubKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
		CHAR szAppValue[MAX_PATH];
		CHAR szAppPath[MAX_PATH];
		GetModuleFileNameA(NULL,szAppPath,MAX_PATH);
		DWORD dwValueType,dwValueLen;

		retCode = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szMainSubKey,0,KEY_ALL_ACCESS,&hKey);
		switch(retCode)
		{
		case ERROR_SUCCESS:
			{				
				retCode = RegQueryValueExA(hKey,strAutoRunItem,NULL,&dwValueType,(LPBYTE)szAppValue,&dwValueLen);
				if (retCode == ERROR_SUCCESS)
				{
					strupr(szAppPath);
					strupr(szAppValue);
					if (strstr(szAppValue,szAppPath))
					{
						bAutoRun = true;
					}
					bResult = true;
				}
				else if (retCode == ERROR_FILE_NOT_FOUND)
				{
					bResult = true;
					bAutoRun = false;
				}
				__leave;
			}			
			break;
		case ERROR_ACCESS_DENIED:
			{
				//MessageBox(NULL,_T("访问注册表被拒绝,请以管理身份重新运行一次."),_T("提示"),MB_OK|MB_ICONSTOP);
				return false;
			}
			break;
		default:
			{
				CHAR szErrorText[255] = {0};			 
				GetLastErrorMessageA("Call RegCreateKeyEx:",szErrorText,retCode);
				TraceMsgA(szErrorText);
				bResult = false;
			}
			break;
		}
	}
	__finally
	{
		if (hKey != NULL)
			RegCloseKey(hKey);
	}
	return bResult;
}

bool IsAutoRunW(LPCWSTR strAutoRunItem,bool &bAutoRun)
{
	HKEY  hKey = NULL;
	DWORD retCode = 0;	
	bool bResult = false;
	bAutoRun = false;
	__try
	{
		HKEY  hKey = NULL;
		DWORD retCode = 0;
		LPCWSTR szMainSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
		WCHAR szAppValue[MAX_PATH];
		WCHAR szAppPath[MAX_PATH];
		GetModuleFileNameW(NULL,szAppPath,MAX_PATH);
		DWORD dwValueType,dwValueLen;

		retCode = RegOpenKeyExW(HKEY_LOCAL_MACHINE,szMainSubKey,0,KEY_ALL_ACCESS,&hKey);
		switch(retCode)
		{
		case ERROR_SUCCESS:
			{				
				retCode = RegQueryValueExW(hKey,strAutoRunItem,NULL,&dwValueType,(LPBYTE)szAppValue,&dwValueLen);
				if (retCode == ERROR_SUCCESS)
				{
					wcsupr(szAppPath);
					wcsupr(szAppValue);
					if (wcsstr(szAppValue,szAppPath))
					{
						bAutoRun = true;
					}
					bResult = true;
				}
				else if (retCode == ERROR_FILE_NOT_FOUND)
				{
					bResult = true;
					bAutoRun = false;
				}
				__leave;
			}			
			break;
		case ERROR_ACCESS_DENIED:
			{
				//MessageBox(NULL,_T("访问注册表被拒绝,请以管理身份重新运行一次."),_T("提示"),MB_OK|MB_ICONSTOP);
				return false;
			}
			break;
		default:
			{
				WCHAR szErrorText[255] = {0};			 
				GetLastErrorMessageW(L"Call RegCreateKeyEx:",szErrorText,retCode);
				TraceMsgW(szErrorText);
				bResult = false;
			}
			break;
		}
	}
	__finally
	{
		if (hKey != NULL)
			RegCloseKey(hKey);
	}
	return bResult;
}



const DWORD Crc32Table[] =
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

DWORD GetCRC32(byte *pBuffer,DWORD  Length,DWORD dwInitCRC)
{
	DWORD crc;
	DWORD i;

	// Generate the CRC32 checksum.
	crc = dwInitCRC ^ 0xffffffffL;

	for (i = 0; i < Length; i++)
	{
		crc = Crc32Table[(crc ^ pBuffer[i]) & 0xff] ^ (crc >> 8);
	}

	return (crc ^ 0xffffffffL);
}


bool GetDirectoryFromFullPathA(LPCSTR strPath,LPSTR strDestDir,int nDirSize,CHAR chSeperator)
{
	LPCSTR pDest = strrchr(strPath,chSeperator);
	if (!pDest)
		return false;
	int nPos = pDest - strPath;	
	if (nPos <=2 )
		return false;
	strncpy_s(strDestDir,nDirSize,strPath,nPos);	
	return true;
}

bool GetDirectoryFromFullPathW(LPCWSTR strPath,LPWSTR strDestDir,int nDirSize,WCHAR chSeperator)
{
	LPCWSTR pDest = wcsrchr(strPath,chSeperator);
	if (!pDest)
		return false;
	int nPos = pDest - strPath;	
	if (nPos <=2 )
		return false;
	wcsncpy_s(strDestDir,nDirSize,strPath,nPos);	
	return true;
}
typedef WCHAR	DirectoryW[MAX_PATH];
typedef CHAR	DirectoryA[MAX_PATH];

#ifdef _UNICODE
#define  Directory	DirectoryW
#else
#define  Directory	DirectoryA
#endif

BOOL CreateDirectoryTreeA(LPSTR strDir,CHAR szSeperator) 
{ 
	BOOL bRet = FALSE; 
	CHAR szDir[1024] = {0};
	strcpy_s(szDir,1024,strDir);
	//确保以"\"结尾，以创建最后一个目录
	int nLen = strlen(szDir);
	if (szDir[nLen - 1] != szSeperator)
	{
		szDir[nLen] = szSeperator;
		szDir[++nLen] = '\0';
	}
	DirectoryA	DirArray[256];	//存放每一层目录
	ZeroMemory(DirArray,sizeof(DirectoryA)*256);
	DirectoryA	TempDir = {0};           //临时变量，存放目录字符串 

	int nDirLen = strlen(szDir);
	int nDirCount = 0;
	for(int i=0; i != nDirLen; ++i) 
	{ 
		if(szDir[i] != szSeperator) 
		{ 
			nLen = strlen(TempDir);
			TempDir[nLen] = szDir[i];
			TempDir[nLen + 1] = '\0';
		} 
		else 
		{ 
			if (nDirCount >= 256)		
				return FALSE;			
			strcpy_s(DirArray[nDirCount ++],256,TempDir);
			CHAR szSeperator2[8] = {0};
			szSeperator2[0] = szSeperator;
			strcat_s(TempDir,256, szSeperator2);
		} 
	} 
	//遍历vector,创建目录 
	for(int i = 0;i < nDirCount;i ++) 
	{ 
		LPCSTR szTempDir = DirArray[i];
		if (!PathFileExistsA(szTempDir))
		{
			bRet = CreateDirectoryA(szTempDir, NULL); 
			TraceMsg(_T("%s Create Succeed.\r\n"),szTempDir);
		}
		else
		{
			TraceMsg(_T("%s already exist.\r\n"),szTempDir);
		}
	} 
	return bRet; 
} 

BOOL CreateDirectoryTreeW(LPWSTR strDir,WCHAR szSeperator) 
{ 
	BOOL bRet = FALSE; 
	WCHAR szDir[1024] = {0};
	wcscpy_s(szDir,1024,strDir);
	//确保以"\"结尾，以创建最后一个目录
	int nLen = wcslen(szDir);
	if (szDir[nLen - 1] != szSeperator)
	{
		szDir[nLen] = szSeperator;
		szDir[++nLen] = L'\0';
	}
	DirectoryW	DirArray[256];	//存放每一层目录
	ZeroMemory(DirArray,sizeof(DirectoryW)*256);
	DirectoryW	TempDir = {0};           //临时变量，存放目录字符串 

	int nDirLen = wcslen(szDir);
	int nDirCount = 0;
	for(int i=0; i != nDirLen; ++i) 
	{ 
		if(szDir[i] != szSeperator) 
		{ 
			nLen = wcslen(TempDir);
			TempDir[nLen] = szDir[i];
			TempDir[nLen + 1] = L'\0';
		} 
		else 
		{ 
			if (nDirCount >= 256)		
				return FALSE;			
			wcscpy_s((WCHAR *)DirArray[nDirCount ++],256,TempDir);
			WCHAR szSeperator2[8] = {0};
			szSeperator2[0] = szSeperator;
			wcscat_s(TempDir,256, szSeperator2);
		} 
	} 
	//遍历vector,创建目录 
	for(int i = 0;i < nDirCount;i ++) 
	{ 
		LPCWSTR szTempDir = DirArray[i];
		if (!PathFileExistsW(szTempDir))
		{
			bRet = CreateDirectoryW(szTempDir, NULL); 
			TraceMsg(_T("%s Create Succeed.\r\n"),szTempDir);
		}
		else
		{
			TraceMsg(_T("%s already exist.\r\n"),szTempDir);
		}
	} 
	return bRet; 
} 

BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{
	STORAGE_PROPERTY_QUERY Query;	// input param for query 
	DWORD dwOutBytes;				// IOCTL output length 
	BOOL bResult;					// IOCTL return val // specify the query type 
	Query.PropertyId = StorageDeviceProperty;
	Query.QueryType = PropertyStandardQuery;	// Query using IOCTL_STORAGE_QUERY_PROPERTY 
	bResult = ::DeviceIoControl(hDevice,		// device handle
		IOCTL_STORAGE_QUERY_PROPERTY,			// info of device property
		&Query, sizeof(STORAGE_PROPERTY_QUERY),	//input data buffer
		pDevDesc, pDevDesc->Size,				// output data buffer 
		&dwOutBytes,							//out length 
		NULL);
	return bResult;
}

int GetFixedHardDriversA(CHAR *szDrvList,int nBuffSize)
{
	CHAR szTemp[512] = {0};
	const int BUFSIZE = 512;
	const int DrvNameLen = 4;
	bool bResult = false;
	HANDLE hDevice = NULL;
	int nDrvCount = 0;
	CHAR *szDriveType[] = 
	{
		"DRIVE_UNKNOWN",
		"DRIVE_NO_ROOT_DIR",
		"DRIVE_REMOVABLE",
		"DRIVE_FIXED",
		"DRIVE_REMOTE",
		"DRIVE_CDROM",
		"DRIVE_RAMDISK",
		NULL
	};

	CHAR *szBusType[] = 
	{
		"BusTypeUnknown",
		"BusTypeScsi",
		"BusTypeAtapi",
		"BusTypeAta",
		"BusType1394",
		"BusTypeSsa",
		"BusTypeFibre",
		"BusTypeUsb",
		"BusTypeRAID",
		"BusTypeiScsi",
		"BusTypeSas",
		"BusTypeSata",
		"BusTypeSd",
		"BusTypeMmc",
		"BusTypeMax",
		"BusTypeMaxReserved",
		NULL
	};
		PSTORAGE_DEVICE_DESCRIPTOR pDevDesc = NULL;
		CHAR *pDrvList = szDrvList;
		__try
		{
			if (!szDrvList)
				__leave;		
			ZeroMemory(szDrvList,nBuffSize);
			if (!GetLogicalDriveStringsA(BUFSIZE-1, szTemp)) 
				__leave;
			CHAR szName[MAX_PATH];
			CHAR szDrive[3] = " :";
			BOOL bFound = FALSE;
			CHAR* p = szTemp;
			do 
			{
				// Copy the drive letter to the template string
				*szDrive = *p;
				if (QueryDosDeviceA(szDrive, szName, BUFSIZE))
				{
					TraceMsgA("%s=%s ",szDrive,szName);
				}
				// Look up each device name
				UINT nDrvType = GetDriveTypeA(szDrive);
#ifdef _DEBUG
				if (nDrvType <= DRIVE_RAMDISK )
					TraceMsgA("DriveType = %s\t",szDriveType[nDrvType]);	
#endif
				if (DRIVE_FIXED == nDrvType)
				{				
					//打开设备，磁盘分区的路径
					CHAR szPhysicalDrv[256] = {0};
					strcpy_s(szPhysicalDrv,256,"\\\\.\\");
					strcat_s(szPhysicalDrv,256,szDrive);
					hDevice = CreateFileA(szPhysicalDrv, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL); 
					if (hDevice == INVALID_HANDLE_VALUE)
						__leave;
					if (!pDevDesc)
					{
						pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR) new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
						pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;
					}
					//ZeroMemory(pDevDesc,pDevDesc->Size);					
					if(!GetDisksProperty(hDevice, pDevDesc))					
						__leave;
					if (pDevDesc->BusType <= BusTypeMax)
					{
						TraceMsgA("Bustype = %s\n",szBusType[pDevDesc->BusType]);
					}
					switch(pDevDesc->BusType)
					{			
					case BusTypeScsi:			
					case BusTypeAtapi:
					case BusTypeAta:
					case BusType1394:
					case BusTypeRAID:
					case BusTypeiScsi:
					case BusTypeSata:
					case BusTypeSas:
						{
							strcpy_s(pDrvList,(nBuffSize - nDrvCount*DrvNameLen),szDrive);
							pDrvList += DrvNameLen;
							nDrvCount ++;						
						}
					default:
						break;
					}
					CloseHandle(hDevice);
					hDevice = NULL;
				}
				TraceMsgA("\t");
				// Go to the next NULL character.
				while (*p++);
			} while (!bFound && *p); // end of string
		}
		__finally
		{
			if (hDevice)
				CloseHandle(hDevice);
			if (pDevDesc)
			{
				delete [](byte *)pDevDesc;
			}
		}
		return nDrvCount;
}

int GetFixedHardDriversW(WCHAR *szDrvList,int nBuffSize)
{
	WCHAR szTemp[512] = {0};
	const int BUFSIZE = 512;
	const int DrvNameLen = 4;
	bool bResult = false;
	HANDLE hDevice = NULL;
	int nDrvCount = 0;
	WCHAR *szDriveType[] = 
	{
		L"DRIVE_UNKNOWN",
		L"DRIVE_NO_ROOT_DIR",
		L"DRIVE_REMOVABLE",
		L"DRIVE_FIXED",
		L"DRIVE_REMOTE",
		L"DRIVE_CDROM",
		L"DRIVE_RAMDISK",
		NULL
	};

	WCHAR *szBusType[] = 
	{
		L"BusTypeUnknown",
		L"BusTypeScsi",
		L"BusTypeAtapi",
		L"BusTypeAta",
		L"BusType1394",
		L"BusTypeSsa",
		L"BusTypeFibre",
		L"BusTypeUsb",
		L"BusTypeRAID",
		L"BusTypeiScsi",
		L"BusTypeSas",
		L"BusTypeSata",
		L"BusTypeSd",
		L"BusTypeMmc",
		L"BusTypeMax",
		L"BusTypeMaxReserved",
		NULL};
	PSTORAGE_DEVICE_DESCRIPTOR pDevDesc = NULL;
	WCHAR *pDrvList = szDrvList;
	__try
	{
		if (!szDrvList)
			__leave;		
		ZeroMemory(szDrvList,nBuffSize);
		if (!GetLogicalDriveStringsW(BUFSIZE-1, szTemp)) 
			__leave;
		WCHAR szName[MAX_PATH];
		WCHAR szDrive[3] = L" :";
		BOOL bFound = FALSE;
		WCHAR* p = szTemp;
		do 
		{
			// Copy the drive letter to the template string
			*szDrive = *p;
			if (QueryDosDeviceW(szDrive, szName, BUFSIZE))
			{
				TraceMsgW(L"%s=%s ",szDrive,szName);
			}
			// Look up each device name
			UINT nDrvType = GetDriveTypeW(szDrive);
#ifdef _DEBUG
			if (nDrvType <= DRIVE_RAMDISK )
				TraceMsgW(L"DriveType = %s\t",szDriveType[nDrvType]);	
#endif
			if (DRIVE_FIXED == nDrvType)
			{				
				//打开设备，磁盘分区的路径
				WCHAR szPhysicalDrv[256] = {0};
				wcscpy_s(szPhysicalDrv,256,L"\\\\.\\");
				wcscat_s(szPhysicalDrv,256,szDrive);
				hDevice = CreateFileW(szPhysicalDrv, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL); 
				if (hDevice == INVALID_HANDLE_VALUE)
					__leave;
				if (!pDevDesc)
				{
					pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR) new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
					pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;
				}
				//ZeroMemory(pDevDesc,pDevDesc->Size);					
				if(!GetDisksProperty(hDevice, pDevDesc))					
					__leave;
				if (pDevDesc->BusType <= BusTypeMax)
				{
					TraceMsgW(L"Bustype = %s\n",szBusType[pDevDesc->BusType]);
				}
				switch(pDevDesc->BusType)
				{			
				case BusTypeScsi:			
				case BusTypeAtapi:
				case BusTypeAta:
				case BusType1394:
				case BusTypeRAID:
				case BusTypeiScsi:
				case BusTypeSata:
				case BusTypeSas:
					{
						wcscpy_s(pDrvList,(nBuffSize - nDrvCount*DrvNameLen),szDrive);
						pDrvList += DrvNameLen;
						nDrvCount ++;						
					}
				default:
					break;
				}
				CloseHandle(hDevice);
				hDevice = NULL;
			}
			TraceMsgW(L"\t");
			// Go to the next NULL character.
			while (*p++);
		} while (!bFound && *p); // end of string
	}
	__finally
	{
		if (hDevice)
			CloseHandle(hDevice);
		if (pDevDesc)
		{
			delete [](byte *)pDevDesc;
		}
	}
	return nDrvCount;
}

bool GetMaxFreeSpaceHDDrviersA(CHAR *szDrv,int nBuffSize)
{
	stdshared_ptr<WCHAR> szDrvW(new WCHAR[nBuffSize]);
	if (GetMaxFreeSpaceHDDrviersW(szDrvW.get(),nBuffSize))
	{
		W2AHelper(szDrvW.get(),szDrv,nBuffSize);
		return true;
	}
	else
		return false;
}

bool GetMaxFreeSpaceHDDrviersW(WCHAR *szDrv,int nBuffSize)
{
	if (!szDrv || !nBuffSize)
		return false;
	DrvSpaceW DrvSpaceArray[128] = {0};	
	ULARGE_INTEGER FreeBytesAvailable;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;	
	// 初始化磁盘列表
	WCHAR szDrvList[512] = {0};
	int nDrvCount = GetFixedHardDriversW(szDrvList,512);
	if (nDrvCount > 0)
	{
		for (int i = 0;i < nDrvCount;i ++)		
			wcscpy_s(DrvSpaceArray[i].szDriver, 8,&szDrvList[__DrvNameLen*i]);
	}
	else
		return false;
	int nMaxFreeSpace = 0;
	for (int i = 0;i < nDrvCount;i ++)
	{			
		if (GetDiskFreeSpaceExW(DrvSpaceArray[i].szDriver,&FreeBytesAvailable,&TotalNumberOfBytes,&TotalNumberOfFreeBytes))
		{
			DrvSpaceArray[i].nTotalSpace = (int)((__int64)TotalNumberOfBytes.QuadPart/(1024*1024));
			DrvSpaceArray[i].nFreeSpace = (int)((__int64)FreeBytesAvailable.QuadPart/(1024*1024));
			if (nMaxFreeSpace < DrvSpaceArray[i].nFreeSpace)
			{
				nMaxFreeSpace = DrvSpaceArray[i].nFreeSpace;
				wcscpy_s(szDrv,nBuffSize,DrvSpaceArray[i].szDriver);
			}
		}
	}
	return true;
}


// 字节流转换为_variant_t数据
bool BytePtr2Variant(IN byte *pBuff,IN int nLength,OUT _variant_t &var)
{
	if (!pBuff || !nLength)
		return false;
	BSTR bstrTemp = SysAllocStringByteLen(NULL,nLength);
	memcpy(bstrTemp,pBuff,nLength);
	var.vt = VT_BSTR;
	var.bstrVal = bstrTemp;
	return true;
}

// _variant_t数据换为字节流转
bool Variant2BytePtr(IN _variant_t var,INOUT byte *pBuff,IN int nBuffSize,OUT int &nDatalen)
{
	assert(var.vt == VT_BSTR);
	if (var.vt == VT_BSTR)
	{
		int nSrcDatalen = SysStringByteLen(var.bstrVal);
		assert(nSrcDatalen >= nBuffSize);
		if (nSrcDatalen < nBuffSize)
			return false;		// 返回输入缓冲区不足错误
		memcpy(pBuff,var.bstrVal,nSrcDatalen);
		nDatalen = nSrcDatalen;
		return true;
	}
	else
		return false;
}


/************************************************************************/
/* add by Brezze.Wang                                                   */
/************************************************************************/

INT_PTR UnicodeStrToMultiByteStr(UINT_PTR nCodePage, LPCWSTR lpUniStr, LPSTR* lppMultiByteStr)
{
	INT_PTR						nMultiByteStrLen;
	INT_PTR						nRet;

	if((*lppMultiByteStr))
	{
		return 0;
	}

	nMultiByteStrLen			= WideCharToMultiByte(nCodePage, 0, lpUniStr, -1, NULL, 0, NULL, NULL);
	if(nMultiByteStrLen > 0)
	{
		(*lppMultiByteStr)		= new CHAR[nMultiByteStrLen /*+ 1*/];
		ZeroMemory((*lppMultiByteStr), (nMultiByteStrLen /*+ 1*/) * sizeof(CHAR));
		nRet					= WideCharToMultiByte(nCodePage, 0, lpUniStr, -1, (*lppMultiByteStr), nMultiByteStrLen, NULL, NULL);
		if(nRet == nMultiByteStrLen)
		{
			//(*lppMultiByteStr)[nMultiByteStrLen]	= _T('\0');
			return nMultiByteStrLen;
		}

		delete[] (*lppMultiByteStr);
		(*lppMultiByteStr)		= NULL;
	}

	return 0;
}

INT_PTR UnicodeStrToMultiByteStr(UINT_PTR nCodePage, LPCWSTR lpUniStr, LPSTR lpMultiByteStr, UINT_PTR nMultiByteLen)
{
	INT_PTR						nRet;

	if(!lpMultiByteStr)
	{
		return 0;
	}

	nRet					= WideCharToMultiByte(nCodePage, 0, lpUniStr, -1, lpMultiByteStr, nMultiByteLen, NULL, NULL);
	if(nRet == nMultiByteLen)
	{
		return nMultiByteLen;
	}

	return 0;
}

INT_PTR MultiByteStrToUnicodeStr(UINT_PTR nCodePage, LPCSTR lpMultiByteStr, LPWSTR* lppUniStr)
{
	INT_PTR						nUniStrLen;
	INT_PTR						nRet;

	if((*lppUniStr))
	{
		return 0;
	}

	nUniStrLen					= MultiByteToWideChar(nCodePage, 0, lpMultiByteStr, -1, NULL, 0);
	if(nUniStrLen > 0)
	{
		(*lppUniStr)			= new WCHAR[nUniStrLen + 1];
		ZeroMemory((*lppUniStr), (nUniStrLen + 1) * sizeof(WCHAR));
		nRet					= MultiByteToWideChar(nCodePage, 0, lpMultiByteStr, -1, (*lppUniStr), nUniStrLen);
		if(nRet == nUniStrLen)
		{
			(*lppUniStr)[nUniStrLen]	= _T('\0');
			return nUniStrLen;
		}

		delete[] (*lppUniStr);
		(*lppUniStr)			= NULL;
	}

	return 0;
}

INT_PTR MultiByteStrToUnicodeStr(UINT_PTR nCodePage, LPCSTR lpMultiByteStr, LPWSTR lpUniStr, UINT_PTR nUniStrLen)
{
	INT_PTR						nRet;

	if(!lpUniStr)
	{
		return 0;
	}

	nRet					= MultiByteToWideChar(nCodePage, 0, lpMultiByteStr, -1, lpUniStr, nUniStrLen - 1);
	if(nRet == nUniStrLen - 1)
	{
		lpUniStr[nUniStrLen - 1]	= _T('\0');
		return nRet;
	}

	return 0;
}
// 必须符合11位数字
// +86（+区号）数字   
bool IsValidSmsAddressA(CHAR *szSms)
{
	int nLen = strlen(szSms);
	
	if(szSms[0] != '+' && !isdigit(szSms[0]))
	{
		return false;
	}
	for (int i = 1 ;i < nLen;i ++)
	{
		if(isdigit(szSms[i]))
			continue;
		else
			return false;
	}
	return true;
}

bool IsValidSmsAddressW(WCHAR *szSms)
{
	int nLen = wcslen(szSms);

	if(szSms[0] != L'+' && !isdigit(szSms[0]))
	{
		return false;
	}
	for (int i = 1 ;i < nLen;i ++)
	{
		if(isdigit(szSms[i]))
			continue;
		else
			return false;
	}
	return true;
}

stdshared_ptr<char> UTF8String(IN LPCWSTR pText,int& OUT nReturnLength)
{
	int nLenW = wcslen(pText);
	int nNeedBuffSize = ::WideCharToMultiByte(CP_UTF8, NULL, pText, nLenW, NULL, 0, NULL, NULL);
	char *pUTF8 = new char[nNeedBuffSize + 1];
	ZeroMemory(pUTF8,nNeedBuffSize + 1);

	nReturnLength = WideCharToMultiByte(CP_UTF8, NULL, pText, nLenW, pUTF8, nNeedBuffSize + 1, NULL, NULL);
	return stdshared_ptr<char>(pUTF8);
}

stdshared_ptr<char> W2AString(IN LPCWSTR str,int& OUT nReturnLength)
{
	int nLenW = wcslen(str);
	int nNeedBuffSize = ::WideCharToMultiByte(CP_ACP, NULL, str, nLenW, NULL, 0, NULL, NULL);
	char* pAnsi = new char[nNeedBuffSize + 1];
	ZeroMemory(pAnsi,nNeedBuffSize + 1);
	nReturnLength = WideCharToMultiByte(CP_ACP, NULL, str, nLenW, pAnsi, nNeedBuffSize + 1, NULL, NULL);	
	return stdshared_ptr<char>(pAnsi);
}

stdshared_ptr<WCHAR> A2WString(IN LPCSTR str,int& OUT nReturnLength)
{
	int nLenA = strlen(str);
	int nNeedBuffSize = ::MultiByteToWideChar(CP_ACP, NULL, str, nLenA, NULL, 0);

	WCHAR* pUnicode = new WCHAR[nNeedBuffSize + 1];
	ZeroMemory(pUnicode,(nNeedBuffSize + 1)*sizeof(WCHAR));

	nReturnLength = MultiByteToWideChar(CP_ACP, 0, str, nLenA, pUnicode, nNeedBuffSize + 1);
	return stdshared_ptr<WCHAR>(pUnicode);
}
