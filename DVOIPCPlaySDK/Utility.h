#pragma  once
//#include <WinSock2.h>
#include <windows.h>
#include <WinDef.h>
#include <assert.h>
#include <TCHAR.h>
#include <WinSock.h>
#include <winioctl.h>
#include <comutil.h>
#include <memory>

#ifdef Release_D
#undef assert
#define assert	((void)0)
#endif

using namespace std;
using namespace std::tr1;

#define IN
#define OUT
#define INOUT

#define MAKEUINT64(low,high)	((UINT64)(((UINT)(low)) | ((UINT64)((UINT)(high))) << 32))
#define HILONG(l64)				((UINT)(((UINT64)(l64) >> 32) & 0xFFFFFFFF))	
#define LOLONG(l64)				((UINT)(l64))

#if !defined (HIWORD)
#define HIWORD(l)				((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#endif

//内部IO控制函数
typedef BOOL (*KernelIoControlProc)(DWORD , LPVOID , DWORD , LPVOID , DWORD , LPDWORD ); 
#define IOCTL_HAL_GET_DEVICE_INFO CTL_CODE(FILE_DEVICE_HAL, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_REBOOT          CTL_CODE(FILE_DEVICE_HAL, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_COLDBOOT        CTL_CODE(FILE_DEVICE_HAL, 30, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define MAKEUINT64(low,high)	((UINT64)(((UINT)(low)) | ((UINT64)((UINT)(high))) << 32))

#define  _TraceMsg TraceMsg

#ifdef _UNICODE
#ifdef _DEBUG
#define TraceMsg TraceMsgW
#else
#define TraceMsg 
#endif

#define _tcsAnalysisData		AnalysisDataW
#define _trimSpace				TrimSpaceW
#define	_tcstolong				wcstolong
#define _tcsReserverFind		WcsReserverFind
#define  TranslatePath			TranslatePathW
#define	IsEmptyString			IsEmptyStringW
#define	IsValidIPAddress		IsValidIPAddressW
//#define	DateTime				DateTimeW
#define GetAppPath				GetAppPathW
#define	GetHostIPAddress		GetHostIPAddressW
#define	GetLastErrorMessage		GetLastErrorMessageW
#define	Hex2AscString			Hex2AscStringW
#define	AscString2Hex			AscString2HexW
#define ScrollEdit				ScrollEditW
#define	IsValidMailAddress		IsValidMailAddressW
#define	IsValidSmsAddress		IsValidSmsAddressW
#define SetAutoRun				SetAutoRunW
#define DeleteAutoRun			DeleteAutoRunW
#define	IsAutoRun				IsAutoRunW

#else
#ifdef _DEBUG
#define TraceMsg TraceMsgA
#else
#define TraceMsg 
#endif
#define	AnalysisData			AnalysisDataA
#define _tcsAnalysisData		AnalysisDataA
#define _tcstolong				strtolong
#define _tcsReserverFind		StrReserverFind
#define _trimSpace				TrimSpaceA
#define  TranslatePath			TranslatePathA
#define	IsEmptyString			IsEmptyStringA
#define	IsValidIPAddress		IsValidIPAddressA
#define GetAppPath				GetAppPathA
#define	GetHostIPAddress		GetHostIPAddressA
#define	GetLastErrorMessage		GetLastErrorMessageA
#define	Hex2AscString			Hex2AscStringA
#define	AscString2Hex			AscString2HexA
#define ScrollEdit				ScrollEditA
#define	IsValidMailAddress		IsValidMailAddressA
#define	IsValidSmsAddress		IsValidSmsAddressA
#define SetAutoRun				SetAutoRunA
#define DeleteAutoRun			DeleteAutoRunA
#define	IsAutoRun				IsAutoRunA
#endif

// 功能描述		内存数据转换为16进制ASCII字符串
// pHex			输入数据流
// nHexLen		输入数据流长度
// szAscString	输出16进制ASCII字符串缓冲区
// nBuffLen		输出缓冲区最大长度

// 返回值		<0时 输入参数不合法
//				>0 返回转换后的ASCII符串的长度
int Hex2AscStringW(IN unsigned char *pHex,IN int nHexLen,OUT WCHAR *szAscString,IN int nBuffLen,WCHAR chSeperator = L' ');
int Hex2AscStringA(IN unsigned char *pHex,IN int nHexLen,OUT CHAR *szAscString,IN int nBuffLen,CHAR chSeperator = ' ');


// 功能描述			16进制ASCII字符串转为用内存数据
// szAscString		输入16进制ASCII字符串
// nAscStringLen	输入数据长度
// pHex				输出缓冲区
// nBuffLen			输出缓冲区最大长度
// 返回值			<0时 输入参数不合法
//					>0 返回转换后pHex数据的长度
int AscString2HexW(IN WCHAR *szAscString,IN int nAscStringLen, unsigned char *pHex,IN int nBuffLen,WCHAR chSeperator = L' ');
int AscString2HexA(IN CHAR *szAscString,IN int nAscStringLen, unsigned char *pHex,IN int nBuffLen,CHAR chSeperator = ' ');


// 取指定参数的一个字节
#define  GetByte(p,n)  ((byte*)&p)[n]
#define  GetInt16(p)	MAKEWORD(((byte *)&p)[1],((byte *)&p)[0])
#define  GetInt32(p)	MAKELONG(GetInt16(((byte *)&p)[2]),GetInt16(p))
#define  GetInt64(p)	MAKEUINT64(GetInt32(((byte *)&p)[4]),GetInt32(p))

ULONG inet_addrW(LPCWSTR IPW);
WCHAR* FAR inet_ntow(in_addr in);
bool IsValidMailAddressA(CHAR *szMail);
bool IsValidMailAddressW(WCHAR *szMail);
// 取本机IP地址表,存放于szIpAddress中
// 可能获得多个IP地址,每个IP地址占用16字符
// 返回值为IP地址数量
int GetHostIPAddressA(CHAR *szIpAddress,int nBuffSize);
int GetHostIPAddressW(WCHAR *szIpAddress,int nBuffSize);

#ifdef _UNICODE
#define		IsNumeric	IsNumericW
#define		IsHexDigit	IsHexDigitW
#else
#define		IsNumeric	IsNumericA
#define		IsHexDigit	IsHexDigitA
#endif

bool IsNumericA(char *szText,int nSize,int nTrans = 10);
bool IsNumericW(WCHAR *szText,int nSize,int nTrans = 10);
bool IsHexDigitA(CHAR ch);
bool IsHexDigitW(WCHAR ch);

//输出错误信息,若dwErrorCode为0,由ErrorMessage调用GetLastError()获取ErrorCode
//szErrorText	发生错误前进行事务的文字描述
//szText		对错误号的解释
//dwError		错误号
void ProcessErrorMessage(TCHAR *szErrorText,BOOL bTrace = true);

void GetLastErrorMessageA(IN LPCSTR szErrorText,OUT LPSTR szText,IN DWORD dwErrorCode = 0);
void GetLastErrorMessageW(IN LPCWSTR szErrorText,OUT LPWSTR szText,IN DWORD dwErrorCode = 0);

void TraceMsgA(LPCSTR pFormat, ...);
void TraceMsgW(LPCWSTR pFormat, ...);
void TraceLongString(LPCTSTR pFormat,unsigned char *szString ,int nStringLength,...);

//以十六进制输出数据,若szFileName为NULL,则等同于DebugOutString,
//否则输出到指定的文件当中
void OutputBin(UCHAR *szBin,UINT nLength,TCHAR *szFileName = NULL,TCHAR *szTitle = NULL);

// 把路径szSrcPath中的字符szSeperatorSrc替换成字符szSeperatorDst
void TranslatePathA(LPSTR szSrcPath,CHAR szSeperator1 = '\\',CHAR szSeperator2 = '/');
void TranslatePathW(LPWSTR szSrcPath,WCHAR szSeperator1 = L'\\',WCHAR szSeperator2 =L'/');


int FindStr(TCHAR *szString,TCHAR *str);
int FindChr(TCHAR *szString,TCHAR ch);
//字符串转长整型数
//szNumber		输入的字符串
//Trans			转换的进制，取值范围为2到16,默认为10
//nNumLength	要转换的字符长度,为0时转换整个字符串
long strtolong(LPCSTR szNumber,int Trans = 10 ,int nNumLength = 0);
//字符串转长整型数
//szNumber		输入的字符串
//Trans			转换的进制，取值范围为2到16,默认为10
//nNumLength	要转换的字符长度,为0时转换整个字符串
long wcstolong(LPCWSTR szNumber,int Trans = 10,int nNumLength = 0);
int StrReserverFind(LPCSTR srcstr,CHAR ch);
int WcsReserverFind(LPCWSTR srcstr,WCHAR ch);
void TrimSpaceA(char *ptr);
void TrimSpaceW(WCHAR *ptr);
WORD CALC_CRC16 (unsigned char *lpszBuff, WORD Len,WORD nInitValue = 0);

BYTE XOR_Sum(unsigned char *ptr,int nLen);

bool IsDomainA(CHAR *szDomain);
bool IsDomainW(WCHAR *szDomain);
#ifdef _UNICODE
#define IsDomain	IsDomainW
#else
#define IsDomain	IsDomainA
#endif

int  A2WHelper(IN LPCSTR lpa, OUT LPWSTR lpw, IN int nWcharBuffLen);
int  W2AHelper(IN LPCWSTR lpw,OUT LPSTR lpa,  IN int nCharBuffLen);

int  ANSI2UTF8(IN const CHAR *szAnsi,OUT char *szUTF8,IN int nBuffSize);
int  UTF82ANSI(IN const CHAR *szUTF8 ,OUT char *szAnsi,IN int nBuffSize);

int  UTF82WHelper(IN LPCSTR lpu,OUT LPWSTR lpw,IN int nWcharBuffLen);
int  W2UTF8Helper(IN LPCWSTR lpw,OUT LPSTR lpu,IN int nUTFBuffLen);

unsigned char Char2Hex(unsigned char nCh);

//条件延时函数,当hEvent重置或等待时间超过dwTime ms时，函数返回
void EventDelay(HANDLE hEvent,DWORD dwTime);

//获取当前进程执行文件的路径
void GetAppPathA(CHAR *szPath,int nBuffLen = MAX_PATH);
void GetAppPathW(WCHAR *szPath,int nBuffLen = MAX_PATH);

long SafeMemcpy(void *pSrcAddr, const void *pDestAddr, size_t nSize);
int  AnalysisDataA(char *szSrc,int nSrcLen,char *szText,int &nTextLen,char Flag = ';');
int  AnalysisDataW(WCHAR *szSrc,int nSrcLen,WCHAR *szText,int &nTextLen,WCHAR Flag = ';');

#define		Mask64(w)			((int)((unsigned __int64 )(-1) >> (64 - (w))))
#define		Mask(w)			((int)((unsigned __int64 )(-1) >> (64 - (w))))
#define		bax1(b,w)		((b)>=(w)?((b) + 1 -(w)):0)
#define		bax2(b,w)		((((b)+1) >= (w))?(w):((b)+1))

// 获取 x 的第b位的位宽为w的值
#define		BitW64(x,b,w)		((((INT64)(x))>>bax1((b),(w))) & Mask64(bax2((b),(w))))
#define		BitW(x,b,w)		((((INT32)(x))>>bax1((b),(w))) & Mask(bax2((b),(w))))

// 取x的第bit位
#define		Bit64(x,bit)		((((INT64)x)&(((INT64)1) << bit))>>bit)
#define		Bit(x,bit)		((((INT)x)&(((INT)1) << bit))>>bit)

int		Mystrlen(LPCSTR str);
int		Mystrcmp(LPCSTR srcstr,LPCSTR dststr);
int		Mystrcpy(LPSTR  dststr,LPCSTR srcstr);
int		Mystrcat(LPSTR  dststr,LPCSTR srcstr);

BYTE	*UINT642Byte(UINT64 nValue64);
UINT64  Byte2UINT64(BYTE *pByte);
UINT	Byte2UINT(BYTE *pByte);
BYTE	*UINT2Byte(UINT nValue32);
//创建基于CWinThread类的线程
//pThreadClass	CWinThread类指针，用于指向创建后的线程对象
//ThreadFunc	线程的执行函数
//Param			线程的入口参数
//ThreadPririty	线程的优先级
#define		BeginWinThread(pThreadClass,ThreadFunc,Param,ThreadPririty)	pThreadClass = AfxBeginThread(ThreadFunc,Param,ThreadPririty,0,CREATE_SUSPENDED);pThreadClass->m_bAutoDelete = FALSE;pThreadClass->ResumeThread();

#define		DECLEAR_FROMVIEW_CREATE()		BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL) ;
#define		IMPLEMENT_FROMVIEW_CREATE(FormView)		BOOL FormView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);}

#define		GetDlgItemRect(nID,Rt)	{	GetDlgItem(nID)->GetWindowRect(&Rt);	ScreenToClient(&Rt);}
#define		SetDlgItemFocus(nID)	{	GetDlgItem(nID)->SetFocus();}

#define		MoveDlgItem(nID,Rt)		{	GetDlgItem(nID)->MoveWindow(&Rt);}
#define		EnableDlgItem(ID,enable)	{::EnableWindow(::GetDlgItem(m_hWnd,ID),enable);}


void SetDlgItemTextA2W(HWND hWnd,UINT nID,LPSTR szText);
void GetDlgItemTextW2A(HWND hWnd,UINT nID,LPSTR szText,int nBuffLen =  256);

void EnableDlgItems(HWND hWnd,bool bEnable,int nIDCount,...);


void ScrollEditA(HWND hEdit,char *pFormat,...);
void ScrollEditW(HWND hEdit,WCHAR *pFormat,...);
// void InitPerformanceClock();
// double  GetExactTime();

//KMP字符串匹配算法 返回值大于0,匹配成功,返回值小于0,匹配失败
int KMP_StrFind(BYTE *szSource,int nSourceLength,BYTE *szKey,int nKeyLength);

//生成[range_min ,range_max]区间的无符号整型数,调用前需至少调用一次srand()
UINT RangedRand( UINT range_min, UINT range_max);
//生成[range_min ,range_max]区间的浮点数,调用前需至少调用一次srand()
double RangedRand( double range_min, double range_max);

bool MemMerge(INOUT char **pDestBuff,INOUT int &nDestBuffLength,INOUT int &nDestBuffSize,IN char *szTempBuff,IN int nTempLen);

bool SetAutoRunA(LPCSTR strAutoRunItem,LPCTSTR szAppPath);
bool SetAutoRunW(LPCWSTR strAutoRunItem,LPCTSTR szAppPath);
bool DeleteAutoRunA(LPCSTR strAutoRunItem);
bool DeleteAutoRunW(LPCWSTR strAutoRunItem);
bool IsAutoRunA(LPCSTR strAutoRunItem,bool &bAutoRun);
bool IsAutoRunW(LPCWSTR strAutoRunItem,bool &bAutoRun);
DWORD GetCRC32(byte *pBuffer,DWORD  Length,DWORD dwInitCRC = 0);

//////////////////////////////////////////////////////////////////////////
// 从完整的文件路径取出其所在目录
// strPath		源如文件路径，如:"C:\\MainDir\\SubDir\\File1.txt"
// strDestDir	输出目录，可能到结果如:"C:\\MainDir\\SubDir"
// nDestDirSize	输出目录缓冲区strDestDir的长度
// chSeperator	路径分隔符,可以是'/'或'\\',默认为'\\'
bool GetDirectoryFromFullPathA(LPCSTR strPath,LPSTR strDestDir,int nDestDirSize,CHAR chSeperator = '\\');
bool GetDirectoryFromFullPathW(LPCWSTR strPath,LPWSTR strDestDir,int nDestDirSize,WCHAR chSeperator = '\\');

BOOL CreateDirectoryTreeA(LPSTR strDir,CHAR szSeperator = '\\') ;
BOOL CreateDirectoryTreeW(LPWSTR strDir,WCHAR szSeperator = L'\\') ;

#ifdef _UNICODE
#define GetDirectoryFromFullPath	GetDirectoryFromFullPathW
#define CreateDirectoryTree			CreateDirectoryTreeW
#else
#define GetDirectoryFromFullPath	GetDirectoryFromFullPathA
#define CreateDirectoryTree			CreateDirectoryTreeA
#endif

// 获取硬盘驱动器列表和数量
// 返回硬盘驱动器数量
// 成功时szDrvList为四个字符一组的二维数组
#define __DrvNameLen	4

struct DrvSpaceW
{
	WCHAR   szDriver[8];
	int		nTotalSpace;
	int		nFreeSpace;
};

struct DrvSpaceA
{
	CHAR   szDriver[8];
	int		nTotalSpace;
	int		nFreeSpace;
};

#ifdef _UNICODE
#define DrvSpace		DrvSpaceW
#define GetFixedHardDrivers		GetFixedHardDriversW
#define	GetDriversSpaces		GetDriversSpacesW
#else
#define DrvSpace		DrvSpaceA
#define GetFixedHardDrivers		GetFixedHardDriversA
#define	GetDriversSpaces		GetDriversSpacesA
#endif

int GetFixedHardDriversA(CHAR *szDrvList,int nBuffSize);
int GetFixedHardDriversW(WCHAR *szDrvList,int nBuffSize);

// void GetDriversSpacesA(DrvSpaceA *DrvSpaceArray,int &nAvailableDrvCount);
// void GetDriversSpacesW(DrvSpaceW *DrvSpaceArray,int &nAvailableDrvCount);

// 获取最大剩余空间的磁盘驱动器
#ifdef _UNICODE
#define GetMaxFreeSpaceHDDrviers	GetMaxFreeSpaceHDDrviersW
#else
#define GetMaxFreeSpaceHDDrviers	GetMaxFreeSpaceHDDrviersA
#endif
bool GetMaxFreeSpaceHDDrviersA(CHAR	*szDrv,int nBuffSize);
bool GetMaxFreeSpaceHDDrviersW(WCHAR	*szDrv,int nBuffSize);

// 字节流转换为_variant_t数据
bool BytePtr2Variant(IN byte *pBuff,IN int nLength,OUT _variant_t &var);

// _variant_t数据换为字节流转
bool Variant2BytePtr(IN _variant_t var,INOUT byte *pBuff,IN int nBuffSize,OUT int &nDatalen);

// LPCSTR DateTimeA();
// 
// LPCWSTR DateTimeW();


// 判断字符是否是只包含空格或Tab符，是则返回true,否则返回false
bool IsEmptyStringA(IN char *ptr);
// 判断字符是否是只包含空格或Tab符，是则返回true,否则返回false
bool IsEmptyStringW(IN WCHAR *ptr);

bool IsValidIPAddressA(LPCSTR szAddress);
bool IsValidIPAddressW(LPCWSTR szAddress);

bool IsValidSmsAddressA(CHAR *szSms);
bool IsValidSmsAddressW(WCHAR *szSms);



/************************************************************************/
/* add by Brezze.Wang                                                   */
/************************************************************************/

/*************************************************************************
	macro
*************************************************************************/
#define UNICODE_TO_ANSI(lpUnicode, lpAnsi, nRet)\
{\
	UnicodeStrToMultiByteStr(CP_ACP, lpUnicode, lpAnsi, nRet);\
}

#define UNICODE_TO_UTF8(lpUnicode, lpUTF8, nRet)\
{\
	UnicodeStrToMultiByteStr(CP_UTF8, lpUnicode, lpUTF8, nRet);\
}

#define ANSI_TO_UNICODE(lpAnsi, lpUnicode, nRet)\
{\
	MultiByteStrToUnicodeStr(CP_ACP, lpAnsi, lpUnicode, nRet);\
}

#define UTF8_TO_UNICODE(lpUTF8, lpUnicode, nRet)\
{\
	MultiByteStrToUnicodeStr(CP_UTF8, lpUTF8, lpUnicode, nRet);\
}

#ifdef MYSQL_CHARACTER_UTF8
#	define MYSQL_CHARACTER_TO_CPLUSPLUS UTF8_TO_UNICODE
#	define CPLUSPLUS_CHARACTER_TO_MYSQL UNICODE_TO_UTF8
#else
#	define MYSQL_CHARACTER_TO_CPLUSPLUS ANSI_TO_UNICODE
#	define CPLUSPLUS_CHARACTER_TO_MYSQL UNICODE_TO_ANSI
#endif

/*************************************************************************
	function name
		UnicodeStrToMultiByteStr

	description
		change unicode string to multi-byte string

	parameters
		nCodePage
			[in] CP_ACP		ANSI
				 CP_UTF8	UTF8
		lpUniStr
			[in] Pointer to the wide character string to convert.

		lppMultiByteStr
			[out] Pointer pointer to a buffer that receives the converted 
				  string.

	return value
		Returns the number of bytes written to the buffer pointed to by 
		lpMultiByteStr if successful. The number includes the byte for 
		the terminating null character.
*************************************************************************/
INT_PTR UnicodeStrToMultiByteStr(UINT_PTR nCodePage, LPCWSTR lpUniStr, LPSTR* lppMultiByteStr);
INT_PTR UnicodeStrToMultiByteStr(UINT_PTR nCodePage, LPCWSTR lpUniStr, LPSTR lpMultiByteStr, UINT_PTR nMultiByteLen);

/*************************************************************************
	function name
		MultiByteStrToUnicodeStr

	description
		change multi-byte string to unicode string

	parameters
		nCodePage
			[in] CP_ACP		ANSI
				 CP_UTF8	UTF8
		lpMultiByteStr
			[in] Pointer to the character string to convert. 

		lppMultiByteStr
			[out] Pointer pointer to a buffer that receives the converted 
				  string.

	return value
		Returns the number of WCHAR values written to the buffer 
		indicated by lpWideCharStr if successful.
*************************************************************************/
INT_PTR MultiByteStrToUnicodeStr(UINT_PTR nCodePage, LPCSTR lpMultiByteStr, LPWSTR* lppUniStr);
INT_PTR MultiByteStrToUnicodeStr(UINT_PTR nCodePage, LPCSTR lpMultiByteStr, LPWSTR lpUniStr, UINT_PTR nUniStrLen);

#define _UTF8(UnicodeText,nLength)		(UTF8String(UnicodeText,nLength).get())
#define _AnsiString(UnicodeText,nLength)	(W2AString(UnicodeText,nLength).get())
#define _UnicodeString(AnsiText,nLength)		(A2WString(AnsiText,nLength).get())

#define stdshared_ptr		std::tr1::shared_ptr
stdshared_ptr<char> UTF8String(IN LPCWSTR pText,int& OUT nReturnLength);
stdshared_ptr<char> W2AString(IN LPCWSTR pText,int& OUT nReturnLength);
stdshared_ptr<WCHAR> A2WString(IN LPCSTR pText,int& OUT nReturnLength);

TCHAR *GetOsVersion();
DWORD GetOsMajorVersion();
bool  GetFilePosition(HANDLE hFile, LONGLONG &nFileOffset);