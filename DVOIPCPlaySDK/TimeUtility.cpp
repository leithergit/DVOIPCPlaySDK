#include <stdio.h>
#include <assert.h>
#include <WinSock.h>
#include <memory>
#include "TimeUtility.h"
#pragma comment (lib,"Ws2_32")

using namespace std;
ETB g_etb;
CThreadSleep ThreadSleep;

//ȡ�þ�ȷʱ��,��λ��
double  GetExactTime()
{	
	assert(g_etb.dfFreq != 0);
	assert(g_etb.dfCounter != 0);
	LONGLONG dfCounter1;
	LARGE_INTEGER LarInt;
	QueryPerformanceCounter(&LarInt);
	dfCounter1 = LarInt.QuadPart;
	double dfReturn = (double)(g_etb.nBaseClock + g_etb.dfMilliseconds + (double)(dfCounter1 - g_etb.dfCounter) / g_etb.dfFreq);
	return dfReturn;
}

//ȡ�þ�ȷʱ��,��λ��
// double  GetExactTime()
// {
// 	assert(g_etb.dfFreq != 0);
// 	assert(g_etb.dfCounter != 0);
// 	LONGLONG dfCounter1;
// 	LARGE_INTEGER LarInt;
// 	QueryPerformanceCounter(&LarInt);
// 	dfCounter1 = LarInt.QuadPart;
// 	double dfReturn = (double)((double)(dfCounter1 - g_etb.dfCounter) / g_etb.dfFreq);
// 	return dfReturn;
// }

int GetDateTimeA(CHAR *szDateTime,int nSize)
{
	assert(nSize >= 19);
	ZeroMemory(szDateTime,sizeof(CHAR)*nSize);
	int nResult = _strdate_s(szDateTime,nSize);
	if (nResult)	
		return nResult;	
	int nLen = strlen(szDateTime);
	szDateTime[nLen] = ' ';
	nLen ++;
	nResult = _strtime_s((CHAR *)&szDateTime[nLen],nSize - nLen);
	if (nResult)	
		return nResult;
	return nResult;
}

int GetDateTimeW(WCHAR *szDateTime,int nSize)
{
	assert(nSize >= 19);
	ZeroMemory(szDateTime,sizeof(WCHAR)*nSize);
	int nResult = _wstrdate_s(szDateTime,nSize);
	if (nResult)	
		return nResult;	
	int nLen = wcslen(szDateTime);
	szDateTime[nLen] = L' ';
	nLen ++;
	nResult = _wstrtime_s((WCHAR *)&szDateTime[nLen],nSize - nLen);
	if (nResult)	
		return nResult;
	return nResult;
}


LPCWSTR DateTimeW()
{
	static WCHAR szDateTime[31] = {0};
	int nSize = 31;
	int nResult = _wstrdate_s(szDateTime,nSize);
	if (nResult)	
		return NULL;	
	int nLen = wcslen(szDateTime);
	szDateTime[nLen] = L' ';
	nLen ++;
	nResult = _wstrtime_s((WCHAR *)&szDateTime[nLen],nSize - nLen);
	if (nResult)	
		return NULL;

	return (LPCWSTR)szDateTime;
}

LPCSTR DateTimeA()
{
	static CHAR szDateTime[31] = {0};
	int nSize = 31;
	int nResult = _strdate_s(szDateTime,nSize);
	if (nResult)	
		return NULL;	
	int nLen = strlen(szDateTime);
	szDateTime[nLen] = ' ';
	nLen ++;
	nResult = _strtime_s((CHAR *)&szDateTime[nLen],nSize - nLen);
	if (nResult)	
		return NULL;
	return (LPCSTR)szDateTime;
}




//һ�������
#define		__Day_Sec                      86400				
//���µ�����
#define		__Long_Month_Sec               2678400
//С�µ�����
#define		__Short_Month_Sec              2592000
//����µ�����
#define		__Long_Feb_Sec                 2505600
//���µ�����
#define		__Short_Feb_Sec                2419200
//���������
#define		__Leap_Year_Sec                31622400
//һ�������
#define		__Year_Sec                     31536000
//���������
#define		__T4_Year_Sec				126230400	//(__Year_Sec*3 + __Leap_Year_Sec)
//100�������
#define		__T100_Year_Sec				(__int64)(3155673600+86400)
//1\400�������
#define		__T400_Year_Sec				(__int64)(12622780800)


bool IsLeapYear(UINT nYear)
{
	return ((nYear % 4 == 0 && nYear % 100 != 0) || (nYear%400 == 0));
}

//Index						   0123456789012345678
//convet the time string like "2014-04-10 12:11:10" to a UTC time
UINT64 DateTimeString2UTC(TCHAR *szTime,UINT64 &nTime)

{
	if (!szTime || _tcslen(szTime)!= 18)
		return 0;
	TCHAR szDigit[8];
	SYSTEMTIME systime;
	_tcsncpy_s(szDigit,8,szTime,4);
	systime.wYear = _ttoi(szDigit);

	_tcsncpy_s(szDigit,8,&szTime[5],2);
	systime.wMonth = _ttoi(szDigit);

	_tcsncpy_s(szDigit,8,&szTime[8],2);
	systime.wDay = _ttoi(szDigit);

	_tcsncpy_s(szDigit,8,&szTime[11],2);
	systime.wHour = _ttoi(szDigit);

	_tcsncpy_s(szDigit,8,&szTime[14],2);
	systime.wMinute = _ttoi(szDigit);

	_tcsncpy_s(szDigit,8,&szTime[17],2);
	systime.wSecond = _ttoi(szDigit);
	SystemTime2UTC(&systime,&nTime);
	return nTime;
}

void UTC2DateTimeStringA(UINT64 nTime,CHAR *szTime,int nSize)
{
	tm TM;
	_localtime64_s(&TM,(time_t *)&nTime);	
	sprintf_s(szTime,nSize,"%04d-%02d-%-02d %02d:%02d:%02d",
		TM.tm_year + 1900,
		TM.tm_mon + 1,
		TM.tm_mday,
		TM.tm_hour,
		TM.tm_min,
		TM.tm_sec);
}

void UTC2DateTimeStringW(UINT64 nTime,WCHAR *szTime,int nSize)
{
	tm TM;
	_localtime64_s(&TM,(time_t *)&nTime);	
	swprintf_s(szTime,nSize,L"%04d-%02d-%-02d %02d:%02d:%02d",
		TM.tm_year + 1900,
		TM.tm_mon + 1,
		TM.tm_mday,
		TM.tm_hour,
		TM.tm_min,
		TM.tm_sec);
}
//ϵͳʱ��ת��ΪUnixʱ��
BOOL SystemTime2UTC(SYSTEMTIME *pSystemTime,UINT64 *pTime)
{
	if (pSystemTime == NULL || pTime == NULL)
		return FALSE;
	if (pSystemTime->wYear < 1970)
		return FALSE;
	if (pSystemTime->wMonth < 1 || pSystemTime->wMonth > 31)
		return FALSE;
	if (pSystemTime->wDay< 1 || pSystemTime->wDay > 31)
		return FALSE;
	if (pSystemTime->wHour < 0 || pSystemTime->wHour > 24)
		return FALSE;
	if (pSystemTime->wMonth < 0 || pSystemTime->wMinute > 60)
		return FALSE;
	if (pSystemTime->wSecond < 0 || pSystemTime->wSecond > 60)
		return FALSE;
	if (pSystemTime->wMilliseconds < 0)
		return FALSE;

	BOOL bLeapYear = IsLeapYear(pSystemTime->wYear);

	int nYears = pSystemTime->wYear - 1970;
	int nBaseLeapYears = (1970/400)*97 + (1970%400)/4 - (1970%400)/100;
	int nLeapYeas = (pSystemTime->wYear/400)*97 + (pSystemTime->wYear%400)/4 - (pSystemTime->wYear%400)/100 - nBaseLeapYears ;
	if (bLeapYear)
		nLeapYeas --;	
	UINT64 nYearTime = (UINT64)nYears*__Year_Sec + (UINT64)nLeapYeas*__Day_Sec;
	UINT64 nMonthTime = 0;

	for (int nLoop = 1;nLoop < pSystemTime->wMonth;nLoop ++)
	{
		switch(nLoop)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:				
			nMonthTime += __Long_Month_Sec	;
			break;
		case 2:
			if (bLeapYear)
			{
				if (pSystemTime->wDay >29)
					return FALSE;
				nMonthTime += __Long_Feb_Sec;
			}
			else
			{
				if (pSystemTime->wDay >28)
					return FALSE;
				nMonthTime += __Short_Feb_Sec;
			}
			break;		
		case 4:			
		case 6:
		case 9:		
		case 11:
			{
				if (pSystemTime->wDay >30)
					return FALSE;
				nMonthTime += __Short_Month_Sec;
			}

			break;
		default:
			break;
		}
	}
	UINT64 nDayTime = (pSystemTime->wDay - 1)*__Day_Sec;
	UINT64 nHourTime = pSystemTime->wHour*3600;
	UINT64 nMinuteTime = pSystemTime->wMinute*60;

	UINT64 nTotalTime = nYearTime + nMonthTime + nDayTime + nHourTime + nMinuteTime + pSystemTime->wSecond;
	*pTime = nTotalTime;
	return TRUE;

}

BOOL UTC2SystemTime(UINT64 *pTime,SYSTEMTIME *pSystemTime)
{
	if (pSystemTime == NULL || pTime == NULL)
		return FALSE;
	UINT64 T64 = *pTime;
	int nYOffset = 0;
	if (T64 >= (__Year_Sec + __Year_Sec))
	{
		T64 = (T64 - __Year_Sec -__Year_Sec);
		nYOffset = 2;
	}
	__int64 nY1 = (T64 / __T400_Year_Sec)*400;
	__int64 nR1 = T64 % __T400_Year_Sec;

	__int64 nY2 = (nR1 / __T100_Year_Sec)*100;
	__int64 nR2 = nR1 % __T100_Year_Sec;

	__int64 nY3 = (nR2 / __T4_Year_Sec)*4;
	__int64 nR3 = nR2 % __T4_Year_Sec;

	__int64 nY = nY1 + nY2 + nY3 + 1970 + nYOffset;
	__int64 nY4 = 1;
	__int64 nD = 1;
	if (IsLeapYear((UINT)nY))
	{
		if (nR3 >= __Leap_Year_Sec)
		{
			nY4 = (nR3 - __Leap_Year_Sec)/__Year_Sec + 1;
			int nTD = (nR3 - __Leap_Year_Sec - (nY4-1)*__Year_Sec) %__Day_Sec;
			nD = (nR3 - __Leap_Year_Sec - (nY4-1)*__Year_Sec) /__Day_Sec;	
			// 			if (nTD)
			// 				nD ++;
		}
		else
		{
			nY4 = 0;
			nD = nR3/__Day_Sec;
		}
	}
	else
	{
		nY4 = nR3 / __Year_Sec;			//��������
		nD = (nR3%__Year_Sec)/__Day_Sec;
	}

	//����
	__int64 nR4 = nR3 % __Day_Sec;
	if (nR4 >=0)
		nD ++;

	__int64 nH = nR4 / 3600;				//Сʱ
	__int64 nR5 = nR4 % 3600;

	__int64 nM = nR5 / 60;					//��
	__int64 nS = nR5 % 60;					//��	

	bool bLeapYear = false;
	int nMonthArray[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
	for (int y = 1;y < nY4; y++)
	{
		if (IsLeapYear(nY + y))
		{
			nD --;
			break;
		}
	}
	nY = nY + nY4;
	if (IsLeapYear((UINT)nY))
	{//����������
		nMonthArray[2] = 29; 		
	}	
	pSystemTime->wYear = (WORD)nY;

	int nMonth = 1;
	int nDay = (int)nD;
	for (nMonth = 1;nMonth <= 12;nMonth ++)
	{
		nDay = nDay - nMonthArray[nMonth];
		if (nDay >= 1)
			continue;
		else
		{
			if (nDay <=0)
				nDay += nMonthArray[nMonth];
			break;
		}
	}
	pSystemTime->wMonth = nMonth;
	pSystemTime->wDay = nDay;
	pSystemTime->wHour = (WORD)nH;
	pSystemTime->wMinute = (WORD)nM;
	pSystemTime->wSecond = (WORD)nS;
	return TRUE;	
}

/************************************************************************/
/* ����˵��:�Զ���ʱ�������ͬ������
/* ����˵��:��
/* �� �� ֵ:�ɹ�����TRUE��ʧ�ܷ���FALSE
/************************************************************************/
BOOL NTPTiming(const char  *szTimeServer)
{	
	// ���IP���й���½ʱ��ͬ����������ַ���������޸�
	SOCKET soc=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	struct sockaddr_in addrSrv;
	addrSrv.sin_addr.S_un.S_addr=inet_addr((char *)szTimeServer);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(123);

	NTP_Packet NTP_Send,NTP_Recv; 
	NTP_Send.Control_Word   =   htonl(0x0B000000);   
	NTP_Send.root_delay		=   0;   
	NTP_Send.root_dispersion   =   0;   
	NTP_Send.reference_identifier	=   0;   
	NTP_Send.reference_timestamp	=   0;   
	NTP_Send.originate_timestamp	=   0;   
	NTP_Send.receive_timestamp		=   0;   
	NTP_Send.transmit_timestamp_seconds		=   0;   
	NTP_Send.transmit_timestamp_fractions   =   0; 

	struct timeval tv_out;
	tv_out.tv_sec = 5;//�ȴ�5��
	tv_out.tv_usec = 0;

	setsockopt(soc,SOL_SOCKET,SO_RCVTIMEO,(char *)&tv_out, sizeof(tv_out));

	if(SOCKET_ERROR==sendto(soc,(const char*)&NTP_Send,sizeof(NTP_Send),
		0,(struct sockaddr*)&addrSrv,sizeof(addrSrv)))
	{
		closesocket(soc);
		return FALSE;
	}
	int sockaddr_Size =sizeof(addrSrv);
	if(SOCKET_ERROR==recvfrom(soc,(char*)&NTP_Recv,sizeof(NTP_Recv),
		0,(struct sockaddr*)&addrSrv,&sockaddr_Size))
	{
		closesocket(soc);
		return FALSE;
	}
	closesocket(soc);
	//WSACleanup();

	SYSTEMTIME	newtime;
	float		Splitseconds;
	struct		tm	LocalTime;
	time_t		ntp_time;

	// ��ȡʱ���������ʱ��
	ntp_time	= ntohl(NTP_Recv.transmit_timestamp_seconds)-2208988800;
	if (localtime_s(&LocalTime,&ntp_time))
	{
		return FALSE;
	}

	// ��ȡ�µ�ʱ��
	newtime.wYear      =LocalTime.tm_year+1900;
	newtime.wMonth     =LocalTime.tm_mon+1;
	newtime.wDayOfWeek =LocalTime.tm_wday;
	newtime.wDay       =LocalTime.tm_mday;
	newtime.wHour      =LocalTime.tm_hour;
	newtime.wMinute    =LocalTime.tm_min;
	newtime.wSecond    =LocalTime.tm_sec;

	// ����ʱ�侫��
	Splitseconds=(float)ntohl(NTP_Recv.transmit_timestamp_fractions);
	Splitseconds=(float)0.000000000200 * Splitseconds;
	Splitseconds=(float)1000.0 * Splitseconds;
	newtime.wMilliseconds   =   (unsigned   short)Splitseconds;

	// �޸ı���ϵͳʱ��
	SetLocalTime(&newtime);
	return TRUE;
}