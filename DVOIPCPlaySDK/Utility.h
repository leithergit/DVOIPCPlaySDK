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

//�ڲ�IO���ƺ���
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

// ��������		�ڴ�����ת��Ϊ16����ASCII�ַ���
// pHex			����������
// nHexLen		��������������
// szAscString	���16����ASCII�ַ���������
// nBuffLen		�����������󳤶�

// ����ֵ		<0ʱ ����������Ϸ�
//				>0 ����ת�����ASCII�����ĳ���
int Hex2AscStringW(IN unsigned char *pHex,IN int nHexLen,OUT WCHAR *szAscString,IN int nBuffLen,WCHAR chSeperator = L' ');
int Hex2AscStringA(IN unsigned char *pHex,IN int nHexLen,OUT CHAR *szAscString,IN int nBuffLen,CHAR chSeperator = ' ');


// ��������			16����ASCII�ַ���תΪ���ڴ�����
// szAscString		����16����ASCII�ַ���
// nAscStringLen	�������ݳ���
// pHex				���������
// nBuffLen			�����������󳤶�
// ����ֵ			<0ʱ ����������Ϸ�
//					>0 ����ת����pHex���ݵĳ���
int AscString2HexW(IN WCHAR *szAscString,IN int nAscStringLen, unsigned char *pHex,IN int nBuffLen,WCHAR chSeperator = L' ');
int AscString2HexA(IN CHAR *szAscString,IN int nAscStringLen, unsigned char *pHex,IN int nBuffLen,CHAR chSeperator = ' ');


// ȡָ��������һ���ֽ�
#define  GetByte(p,n)  ((byte*)&p)[n]
#define  GetInt16(p)	MAKEWORD(((byte *)&p)[1],((byte *)&p)[0])
#define  GetInt32(p)	MAKELONG(GetInt16(((byte *)&p)[2]),GetInt16(p))
#define  GetInt64(p)	MAKEUINT64(GetInt32(((byte *)&p)[4]),GetInt32(p))

ULONG inet_addrW(LPCWSTR IPW);
WCHAR* FAR inet_ntow(in_addr in);
bool IsValidMailAddressA(CHAR *szMail);
bool IsValidMailAddressW(WCHAR *szMail);
// ȡ����IP��ַ��,�����szIpAddress��
// ���ܻ�ö��IP��ַ,ÿ��IP��ַռ��16�ַ�
// ����ֵΪIP��ַ����
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

//���������Ϣ,��dwErrorCodeΪ0,��ErrorMessage����GetLastError()��ȡErrorCode
//szErrorText	��������ǰ�����������������
//szText		�Դ���ŵĽ���
//dwError		�����
void ProcessErrorMessage(TCHAR *szErrorText,BOOL bTrace = true);

void GetLastErrorMessageA(IN LPCSTR szErrorText,OUT LPSTR szText,IN DWORD dwErrorCode = 0);
void GetLastErrorMessageW(IN LPCWSTR szErrorText,OUT LPWSTR szText,IN DWORD dwErrorCode = 0);

void TraceMsgA(LPCSTR pFormat, ...);
void TraceMsgW(LPCWSTR pFormat, ...);
void TraceLongString(LPCTSTR pFormat,unsigned char *szString ,int nStringLength,...);

//��ʮ�������������,��szFileNameΪNULL,���ͬ��DebugOutString,
//���������ָ�����ļ�����
void OutputBin(UCHAR *szBin,UINT nLength,TCHAR *szFileName = NULL,TCHAR *szTitle = NULL);

// ��·��szSrcPath�е��ַ�szSeperatorSrc�滻���ַ�szSeperatorDst
void TranslatePathA(LPSTR szSrcPath,CHAR szSeperator1 = '\\',CHAR szSeperator2 = '/');
void TranslatePathW(LPWSTR szSrcPath,WCHAR szSeperator1 = L'\\',WCHAR szSeperator2 =L'/');


int FindStr(TCHAR *szString,TCHAR *str);
int FindChr(TCHAR *szString,TCHAR ch);
//�ַ���ת��������
//szNumber		������ַ���
//Trans			ת���Ľ��ƣ�ȡֵ��ΧΪ2��16,Ĭ��Ϊ10
//nNumLength	Ҫת�����ַ�����,Ϊ0ʱת�������ַ���
long strtolong(LPCSTR szNumber,int Trans = 10 ,int nNumLength = 0);
//�ַ���ת��������
//szNumber		������ַ���
//Trans			ת���Ľ��ƣ�ȡֵ��ΧΪ2��16,Ĭ��Ϊ10
//nNumLength	Ҫת�����ַ�����,Ϊ0ʱת�������ַ���
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

//������ʱ����,��hEvent���û�ȴ�ʱ�䳬��dwTime msʱ����������
void EventDelay(HANDLE hEvent,DWORD dwTime);

//��ȡ��ǰ����ִ���ļ���·��
void GetAppPathA(CHAR *szPath,int nBuffLen = MAX_PATH);
void GetAppPathW(WCHAR *szPath,int nBuffLen = MAX_PATH);

long SafeMemcpy(void *pSrcAddr, const void *pDestAddr, size_t nSize);
int  AnalysisDataA(char *szSrc,int nSrcLen,char *szText,int &nTextLen,char Flag = ';');
int  AnalysisDataW(WCHAR *szSrc,int nSrcLen,WCHAR *szText,int &nTextLen,WCHAR Flag = ';');

#define		Mask64(w)			((int)((unsigned __int64 )(-1) >> (64 - (w))))
#define		Mask(w)			((int)((unsigned __int64 )(-1) >> (64 - (w))))
#define		bax1(b,w)		((b)>=(w)?((b) + 1 -(w)):0)
#define		bax2(b,w)		((((b)+1) >= (w))?(w):((b)+1))

// ��ȡ x �ĵ�bλ��λ��Ϊw��ֵ
#define		BitW64(x,b,w)		((((INT64)(x))>>bax1((b),(w))) & Mask64(bax2((b),(w))))
#define		BitW(x,b,w)		((((INT32)(x))>>bax1((b),(w))) & Mask(bax2((b),(w))))

// ȡx�ĵ�bitλ
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
//��������CWinThread����߳�
//pThreadClass	CWinThread��ָ�룬����ָ�򴴽�����̶߳���
//ThreadFunc	�̵߳�ִ�к���
//Param			�̵߳���ڲ���
//ThreadPririty	�̵߳����ȼ�
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

//KMP�ַ���ƥ���㷨 ����ֵ����0,ƥ��ɹ�,����ֵС��0,ƥ��ʧ��
int KMP_StrFind(BYTE *szSource,int nSourceLength,BYTE *szKey,int nKeyLength);

//����[range_min ,range_max]������޷���������,����ǰ�����ٵ���һ��srand()
UINT RangedRand( UINT range_min, UINT range_max);
//����[range_min ,range_max]����ĸ�����,����ǰ�����ٵ���һ��srand()
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
// ���������ļ�·��ȡ��������Ŀ¼
// strPath		Դ���ļ�·������:"C:\\MainDir\\SubDir\\File1.txt"
// strDestDir	���Ŀ¼�����ܵ������:"C:\\MainDir\\SubDir"
// nDestDirSize	���Ŀ¼������strDestDir�ĳ���
// chSeperator	·���ָ���,������'/'��'\\',Ĭ��Ϊ'\\'
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

// ��ȡӲ���������б������
// ����Ӳ������������
// �ɹ�ʱszDrvListΪ�ĸ��ַ�һ��Ķ�ά����
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

// ��ȡ���ʣ��ռ�Ĵ���������
#ifdef _UNICODE
#define GetMaxFreeSpaceHDDrviers	GetMaxFreeSpaceHDDrviersW
#else
#define GetMaxFreeSpaceHDDrviers	GetMaxFreeSpaceHDDrviersA
#endif
bool GetMaxFreeSpaceHDDrviersA(CHAR	*szDrv,int nBuffSize);
bool GetMaxFreeSpaceHDDrviersW(WCHAR	*szDrv,int nBuffSize);

// �ֽ���ת��Ϊ_variant_t����
bool BytePtr2Variant(IN byte *pBuff,IN int nLength,OUT _variant_t &var);

// _variant_t���ݻ�Ϊ�ֽ���ת
bool Variant2BytePtr(IN _variant_t var,INOUT byte *pBuff,IN int nBuffSize,OUT int &nDatalen);

// LPCSTR DateTimeA();
// 
// LPCWSTR DateTimeW();


// �ж��ַ��Ƿ���ֻ�����ո��Tab�������򷵻�true,���򷵻�false
bool IsEmptyStringA(IN char *ptr);
// �ж��ַ��Ƿ���ֻ�����ո��Tab�������򷵻�true,���򷵻�false
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