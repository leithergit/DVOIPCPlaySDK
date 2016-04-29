// #pragma once
// #include "stdafx.h"
// #include <winsock2.h>
// #include <MSWSock.h>
// #include <memory>
// #include <MSTcpIP.h>
// #include <time.h>
// #include <list>
// 
// #pragma comment(lib, "ws2_32.lib")
// 
// #ifndef INOUT
// #define INOUT
// #endif
// 
// #ifndef IN
// #define IN
// #endif
// 
// #ifndef OUT
// #define OUT
// #endif
// 
// 
// struct MSG_HEAD
// {
// 	MSG_HEAD()
// 	{
// 		ZeroMemory(this, sizeof(MSG_HEAD));
// 		Magic1[0] = 0xF5;
// 		Magic1[1] = 0x5A;
// 		Magic1[2] = 0xA5;
// 		Magic1[3] = 0x5F;
// 		Version = 0x0101;
// 	}
// 	char	Magic1[4];  // 	���ֽ� �̶����ݣ�0xF5��0x5A��0xA5��0x5F��
// 	UINT	Pktlen;		//	����U32 ���ݰ��ܳ��ȡ�
// 	USHORT	Version;	//  ����U16 Э��汾�ţ���8λΪ���汾�ţ���8λΪ�Ӱ汾�š�
// 	USHORT	Hwtype;		//	����U16 Ӳ������(����)
// 	UINT	Sn;		    //  ����U32 ֡���к�(����)
// 	USHORT	CmdType;	//	����U16 ��ʾ��������
// 	USHORT	CmdSubType;	//	����U16 ��ʾ����������
// 	USHORT	DataType;	//  ����U16 ���ݰ�����1���������2����Ƶ����3����Ƶ��
// 	USHORT	Rev1;		//	����U16 �����ֶ�
// 	UINT	Rev2;		//	����U32 �����ֶ�
// 	UINT	Rev3;		//	����U32 �����ֶ�
// };
// 
// //IPC ������Ƶ����ͷ
// typedef struct{
// 	UINT	    chn;	            //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
// 	UINT	    stream;             //������ţ�0:��������1����������2������������
// 	UINT	    frame_type;         //֡���ͣ���Χ�ο�APP_NET_TCP_STREAM_TYPE
// 	UINT	    frame_num;          //֡��ţ���Χ0~0xFFFFFFFF.
// 	UINT		sec;	            //ʱ���,��λ���룬Ϊ1970��������������
// 	UINT		usec;               //ʱ���,��λ��΢�롣
// 	UINT		 rev[2];
// }app_net_tcp_enc_stream_head_t;
// 
// #define U32		UINT
// 
// //������ VideoStream
// typedef struct{
// 	U32			chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
// 	U32			stream;             //������ţ�0:��������1����������2������������
// 	U32			enable;             //0:ֹͣ���ͣ�1����ʼ���͡�
// 	U32			nettype;            //0:tcp,1:udp.
// 	U32			port;               //udp port.
// 	U32			rev[3];
// }app_net_tcp_enc_ctl_t;
// 
// typedef struct{
// 	U32			chn;                //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
// 	U32			stream;             //������ţ�0:��������1����������2������������
// 	U32			state;              //0:�ɹ���1��ʧ�ܡ�
// 	U32			rev;
// }app_net_tcp_enc_ctl_ack_t;
// 
// typedef enum {
// 	APP_NET_TCP_COM_DST_IDR_FRAME = 1,  ///< IDR֡��
// 	APP_NET_TCP_COM_DST_I_FRAME,        ///< I֡��
// 	APP_NET_TCP_COM_DST_P_FRAME,        ///< P֡��
// 	APP_NET_TCP_COM_DST_B_FRAME,        ///< B֡��
// 	APP_NET_TCP_COM_DST_JPEG_FRAME,     ///< JPEG֡
// 	APP_NET_TCP_COM_DST_711_ALAW,       ///< 711 A�ɱ���֡
// 	APP_NET_TCP_COM_DST_711_ULAW,       ///< 711 U�ɱ���֡
// 	APP_NET_TCP_COM_DST_726,            ///< 726����֡
// 	APP_NET_TCP_COM_DST_AAC,            ///< AAC����֡��
// 	APP_NET_TCP_COM_DST_MAX,
// } APP_NET_TCP_STREAM_TYPE;
// // #ifdef _DEBUG
// // #define new DEBUG_NEW
// // #endif
// 
// using namespace std;
// using namespace std::tr1;
// 
// #ifndef	_X64
// #define _X64
// #endif
// 
// struct _TCP_KEEPALIVE 
// { 
// 	ULONG	Onoff; 
// 	ULONG	nKeepAliveTime; 
// 	ULONG	nKeepAliveInterval; 
// };
// 
// // �����������,��λ��,�����⴦ʱ��û�������շ�����Ͽ������ӣ�Ĭ��Ϊ30��
// #define _HeartBeatTime_Second		30
// #define _Max_Recv_Error				3
// #define MAX_BUFFER_LEN				2048  
// #define _1M							(1024*1024)
// #define _64M						(_1M*64)
// 
// /// @brief ��׼�첽�ص�����
// typedef long(__stdcall *STDCALLBACK)(WPARAM wp, LPARAM lp);
// 
// class CSocketClient
// {
// public:
// 	UINT			m_nSize;
// 	WSAEVENT		m_hSendEvent;
// 	WSAEVENT		m_hRecvEvent;
// 	WSAEVENT		m_hCloseEvent;
// 	SOCKET			s;
// 	bool			m_bPassive;								/**< Ϊtrueʱ����Ϊ�ڷ���˱�����������ӣ�Ϊfalseʱ��Ϊ�ͻ����������������*/
// 	bool			m_bBindIOCP;
// 															/**< Ĭ��Ϊfalse*/
// 	char*			m_pszRecvBuffer;						/**< �������ݵĻ�����*/
// 	CRITICAL_SECTION	m_csRecvBuffer;
// 	UINT			m_nRecvBufferSize;						/**< �������ݻ���������*/
// 	UINT			m_nRecvBufferLength;					/**< ���յ������ݳ���*/
// 	SOCKADDR_IN		m_RemoteAddr;	
// 	timeval			m_tConnectTimeout;
// 	time_t			m_tLastActive;							/**< �ͷ������һ�λʱ��*/
// 	static LPFN_CONNECTEX	m_pfnConnectEx;
// 	LPFN_DISCONNECTEX	m_pfnDisConnectEx;					/**< m_pfnDisConnectExʹ��m_pfnDisConnectEx�Ͽ���Socket,����CloseSocket�رգ��������µ�Socketֱ��Ͷ��AcceptEx����*/
// 	STDCALLBACK		m_pfnClientCallBack;					/**< �ͻ���ר�ûص�����*/
// 
// 	UINT64			m_nClientID;							/**< �ͻ���ID*/
// 	UINT			m_nPacketFailed;						/**< �������ݰ�����Ĵ���,��������������_Max_Recv_Error,��Ͽ�����*/
// 	UINT			m_nHeartBeatInterval_Second;			/**< �ͻ��������,��������ֵΪ�շ���ʱֵ��2��*/
// 	
// 	UINT			m_nRecvContextBuffLength;				/**< ���ν��ղ����������ĳ���,Ĭ��Ϊ2048*/
// #ifdef _X64
// 	UINT64			m_nFrameID;
// 	UINT64			m_nRecvBytes;
// 	UINT64			m_nSendBytes;
// 	UINT64			m_nContextBaseID;
// #else
// 	UINT			m_nFrameID;
// 	UINT			m_nRecvBytes;
// 	UINT			m_nSendBytes;
// 	UINT			m_nContextBaseID;
// #endif
// 	UINT			m_nRecvTimeoutms;						/**< �������ݳ�ʱ���ڣ���λΪ����*/
// 	UINT			m_nSendTimeoutms;						/**< �������ݳ�ʱ���ڣ���λΪ����*/
// 	WORD			m_nSendRetryCount;						/**< ���������Դ��� */
// 	WORD			m_nRecvRetryCount;						/**< ���������Դ���*/
// 	void*			m_pCustomData;
// 		
// 	int				m_nContextCount;
// 	
// 	CSocketClient(UINT nSendTimeoutms = 5000,UINT nRecvTimeOutms = 5000)
// 	{
// 		ZeroMemory(this, sizeof(CSocketClient)); 
// 		m_nSize = sizeof(CSocketClient);
// 		
// 		s = INVALID_SOCKET;	
// 		m_nRecvContextBuffLength = MAX_BUFFER_LEN;
// 		m_pszRecvBuffer = new char[MAX_BUFFER_LEN];
// 		ZeroMemory(m_pszRecvBuffer,MAX_BUFFER_LEN);
// 		m_nRecvBufferSize = MAX_BUFFER_LEN;
// 		m_nSendTimeoutms = nRecvTimeOutms;
// 		m_nRecvTimeoutms = nRecvTimeOutms;
// 		m_nHeartBeatInterval_Second = 30;		
// 		::InitializeCriticalSection(&m_csRecvBuffer);
// 	}
// #define __countof(array) (sizeof(array)/sizeof(array[0]))
// 	static void TraceMsgA(LPCSTR pFormat, ...)
// 	{
// 		va_list args;
// 		va_start(args, pFormat);
// 		int nBuff;
// 		CHAR szBuffer[0x7fff];
// 		nBuff = _vsnprintf(szBuffer, __countof(szBuffer), pFormat, args);
// 		//::wvsprintf(szBuffer, pFormat, args);
// 		//assert(nBuff >=0);
// 		OutputDebugStringA(szBuffer);
// 		va_end(args);
// 	}
// 
// 	// �ͷ�����Context�������ö�ʱ��
// 	// ����ȷ����ǰ���Ӳ��ǻ����ʱ�������ô�����
// 	void Reset()
// 	{
// 		// TraceMsgA("@Reset SocketClient[%I64u] m_nContextCount = %d.\n",m_nClientID,m_nContextCount);
// 		if (m_pszRecvBuffer)
// 		{
// 			ZeroMemory(m_pszRecvBuffer,m_nRecvBufferSize);
// 			m_nRecvBufferLength = 0;
// 		}
// 		m_nRecvContextBuffLength = MAX_BUFFER_LEN;
// 		m_nRecvRetryCount = 0;
// 		m_nSendRetryCount = 0;
// 		m_pCustomData = NULL;
// 		m_nPacketFailed = 0;
// 		m_nFrameID = 0;
// 		m_nRecvBytes = 0;
// 		m_nSendBytes = 0;
// 		m_nContextBaseID = 0;		
// 		m_pCustomData = NULL;
// 	}
// 	// �ͷŵ�Socket
// 	~CSocketClient()
// 	{
// 		if( s!=INVALID_SOCKET)
// 		{
// 			DisConnect(s);
// 		}		
// 		if (m_hRecvEvent)
// 		{
// 			WSACloseEvent(m_hRecvEvent);
// 			m_hRecvEvent = NULL;
// 		}
// 		if (m_hSendEvent)
// 		{
// 			WSACloseEvent(m_hSendEvent);
// 			m_hSendEvent = NULL;
// 		}
// 		if (m_hCloseEvent)
// 		{
// 			WSACloseEvent(m_hCloseEvent);
// 			m_hCloseEvent = NULL;
// 		}
// 		Reset();
// 		if (m_pszRecvBuffer)
// 		{
// 			delete m_pszRecvBuffer;
// 			m_pszRecvBuffer = NULL;
// 		}
// 		::DeleteCriticalSection(&m_csRecvBuffer);	
// 	}
// 	// ���û���������
// 	void ResetBuffer()
// 	{
// 		ZeroMemory(m_pszRecvBuffer,m_nRecvBufferSize);
// 	}
// 
// 	inline void SetCustomData(void *pData)
// 	{
// 		m_pCustomData = pData;
// 	}
// 
// 	inline void *GetCustomData()
// 	{
// 		return m_pCustomData;
// 	}
// 
// #define PostSend	Send
// 	int Send(char *szBuffer,int nBufferLen,DWORD &nSentBytes,char *pCustumAddr = NULL,int nTimeoutms = 0)
// 	{
// 		if (!szBuffer || !nBufferLen)
// 			return SOCKET_ERROR;
// 		DWORD dwBytes = 0;
// 		DWORD dwFlags = 0;
// 		WSABUF wsa;		
// 		wsa.buf = szBuffer;
// 		wsa.len = nBufferLen;
// 		DWORD dwErrorCode = 0;
// 		WSAOVERLAPPED ovl;
// 		ovl.hEvent = m_hSendEvent;
// 		WSAResetEvent(m_hSendEvent);
// 		if (WSASend(s, &wsa, 1, &nSentBytes, 0, &ovl, 0) == 0)
// 			return 0;
// 		else
// 		{
// 			dwErrorCode = WSAGetLastError();
// 			if (dwErrorCode != WSA_IO_PENDING)
// 			{
// 				//HandleError(dwErrorCode);
// 				return dwErrorCode;
// 			}
// 			int nWaitTimeout = m_nSendTimeoutms;
// 			if (nTimeoutms != 0)
// 				nWaitTimeout = nTimeoutms;
// 			int nResult = WSAWaitForMultipleEvents(1, &m_hSendEvent, TRUE, nWaitTimeout, TRUE);
// 			if (nResult == WSA_WAIT_FAILED)
// 				return WSAGetLastError();
// 			else if (nResult == WSA_WAIT_TIMEOUT)
// 			{
// 				return WSA_WAIT_TIMEOUT;
// 			}
// 			DWORD dwFlags = 0;
// 			if (WSAGetOverlappedResult(s, &ovl, &nSentBytes, FALSE, &dwFlags))
// 			{
// 				return 0;
// 			}
// 			else
// 				return  ::WSAGetLastError();
// 		}
// 		
// 	}
// 
// 	int Recv(INOUT char *szBuffer,IN int nBuffersize,OUT DWORD &nDataRecved)
// 	{
// 		if (s == INVALID_SOCKET)
// 			return SOCKET_ERROR;
// 
// 		DWORD dwErrorCode = 0;
// 		WSAOVERLAPPED ovl;
// 		ZeroMemory(&ovl, sizeof(ovl));
// 		ovl.hEvent = m_hRecvEvent;
// 		WSAResetEvent(m_hRecvEvent);
// 
// 		WSABUF wsa;
// 		wsa.buf = szBuffer;
// 		wsa.len = nBuffersize;	
// 
// 		DWORD dwFlags = 0;	
// 
// 		int nResult = WSARecv(s, &wsa, 1, &nDataRecved, &dwFlags, &ovl,0);
// 		if (nResult ==0)		// ���ݽ��ճɹ�
// 			return 0;
// 		if (nResult == SOCKET_ERROR )
// 		{	
// 			DWORD dwErrorCode = WSAGetLastError();
// 			if (dwErrorCode != WSA_IO_PENDING)
// 			{
// 				//HandleError(dwErrorCode);
// 				return dwErrorCode;
// 			}
// 			int nResult = WSAWaitForMultipleEvents(1, &m_hRecvEvent, TRUE, m_nRecvTimeoutms, TRUE);
// 			if (nResult == WSA_WAIT_FAILED)
// 				return WSAGetLastError();
// 			else if (nResult == WSA_WAIT_TIMEOUT)
// 				return WSAGetLastError();
// 
// 			DWORD dwFlags = 0;
// 			if (WSAGetOverlappedResult(s, &ovl, &nDataRecved, FALSE, &dwFlags))
// 				return 0;
// 			else
// 				return  ::WSAGetLastError();
// 		}	
// 		return 0;
// 	}
// 
// 	int	CreateSocket()
// 	{
// 		DWORD dwResult = SOCKET_ERROR;
// 		__try
// 		{
// 			s = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
// 			if (s == INVALID_SOCKET)
// 				__leave;
// 
// 			//����socket ѡ��
// 			//1.�������Źر�socket
// 			linger sLinger;
// 			sLinger.l_onoff = 1;	//(��closesocket()����,���ǻ�������û������ϵ�ʱ��������)
// 			// ���m_sLinger.l_onoff = 0;���ܺ�2.)������ͬ;
// 			sLinger.l_linger = 2;//(��������ʱ��Ϊ��)
// 			if (setsockopt(s,SOL_SOCKET,SO_LINGER,(const char*)&sLinger,sizeof(linger)) == SOCKET_ERROR)
// 				__leave;
// 			// ���ڷ�����socket,SO_SNDTIMEO��SO_RCVTIMEOѡ����Ч
// // 			if (m_nSendTimeout > 0)
// // 			{
// // 				//2.�����շ���ʱʱ��
// // 				if (setsockopt(s,SOL_SOCKET,SO_SNDTIMEO,(char *)&m_nSendTimeout,sizeof(int)) == SOCKET_ERROR)
// // 					__leave;
// // 			}
// // 			if (m_nRecvTimeoutMs > 0)
// // 			{
// // 				//����ʱ��
// // 				if (setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char *)&m_nRecvTimeoutMs,sizeof(int)) == SOCKET_ERROR)
// // 					__leave;
// // 			}
// 
// 			// ����TCP_KEEPALIVE����
// 			_TCP_KEEPALIVE KeepAliveIn = {0},KeepALiveOut = {0}; 
// 			KeepAliveIn.nKeepAliveTime		= 5000;		// ̽��Ƶ��,5000msһ��
// 			KeepAliveIn.nKeepAliveInterval	= 2000;		// ̽��ʧ�ܺ�������Ƶ��,2000msһ��
// 			KeepAliveIn.Onoff				= TRUE;  
// 			BOOL bKeepAliveEnable			= TRUE;
// 			ULONG nInLen					= sizeof(_TCP_KEEPALIVE);
// 			ULONG nOutLen					= sizeof(_TCP_KEEPALIVE);
// 			ULONG nBytesReturn				= 0;
// 			// ���ô�ѡ���,����Է��ر���socket����,�ٽ����շ�����ʱ������������WSAENETRESET����,
// 			// ����֮�����еĵ��շ��������᷵��WSAENOTCONN����
// 			if (setsockopt(s,SOL_SOCKET,SO_KEEPALIVE,(char *)&bKeepAliveEnable,sizeof(BOOL)) != 0 )
// 				__leave;
// 
// 			if (WSAIoctl(s, SIO_KEEPALIVE_VALS, (LPVOID)&KeepAliveIn, nInLen,(LPVOID)&KeepALiveOut, nOutLen, &nBytesReturn, NULL, NULL) == SOCKET_ERROR)
// 				__leave;
// 			dwResult =  0;
// 		}
// 		__finally
// 		{
// 			if (dwResult)
// 			{
// 				dwResult = WSAGetLastError();
// 				if (INVALID_SOCKET != s)
// 					closesocket(s);
// 				s = INVALID_SOCKET;
// 			}
// 		}
// 		return dwResult;
// 	}
// 	static void DisConnect(SOCKET &socket)
// 	{
// 		linger sLinger;
// 		sLinger.l_onoff = 1;	//(��closesocket()����,���ǻ�������û������ϵ�ʱ��������)
// 		// ���m_sLinger.l_onoff = 0;���ܺ�2.)������ͬ;
// 		sLinger.l_linger = 2;//���Źر�
// 		__try
// 		{
// 			if (socket == INVALID_SOCKET)
// 				__leave;
// 			if (setsockopt(socket,SOL_SOCKET,SO_LINGER,(const char*)&sLinger,sizeof(linger)) != SOCKET_ERROR)
// 			{//���Źر�
// 				if (shutdown(socket,SD_BOTH) != SOCKET_ERROR)
// 				{
// 					char szBuff[1023] = {0};
// 					recv(socket,szBuff,1023,0);
// 					closesocket(socket);
// 					__leave;
// 				}
// 				else
// 				{
// 					shutdown(socket,SD_BOTH);
// 					closesocket(socket);
// 					__leave;
// 				}
// 			}
// 			else
// 			{//ǿ�ƹر�
// 				shutdown(socket,SD_BOTH);
// 				closesocket(socket);
// 				__leave;
// 			}
// 		}
// 		__finally
// 		{
// 			socket = INVALID_SOCKET;
// 		}
// 	}
// 	
// 	// �ص�I/Oģ�ͽ���ͬ������
// 	int Connect(const CHAR *szIp,int nPort,UINT nConnectTimeout = 5000)
// 	{	
// 		DWORD dwErrorCode = 0;
// 		int nResult = SOCKET_ERROR;
// 		__try
// 		{			
// 			if (s != INVALID_SOCKET)
// 				__leave;
// 
// 			if (CreateSocket() != 0)
// 				__leave;
// 
// 			int nNonBlock = 1;
// 			dwErrorCode		= ioctlsocket(s, FIONBIO, (unsigned long*)&nNonBlock);
// 			if(dwErrorCode != 0)
// 				__leave;
// 
// 			SOCKADDR_IN ServerAddr;
// 			ServerAddr.sin_port=htons((u_short)nPort);
// 			ServerAddr.sin_family=AF_INET;	
// 			ServerAddr.sin_addr.s_addr =  inet_addr((LPCSTR)szIp);
// 
// 			if (::connect(s, (LPSOCKADDR)&ServerAddr, sizeof(SOCKADDR_IN)))
// 			{
// 				dwErrorCode = WSAGetLastError();
// 				if (dwErrorCode != WSAEWOULDBLOCK)
// 					__leave;
// 
// 				fd_set FDSET;
// 				FD_ZERO(&FDSET);
// 				FD_SET(s, &FDSET);
// 				m_tConnectTimeout.tv_sec = nConnectTimeout / 1000;
// 				m_tConnectTimeout.tv_usec = nConnectTimeout % 1000;
// 				nResult = select(0, 0, &FDSET, 0, &m_tConnectTimeout);
// 				switch(nResult)
// 				{
// 				case SOCKET_ERROR:
// 				case 0:
// 					{
// 						__leave;
// 					}
// 					break;
// 				default:
// 					{
// 						if (!FD_ISSET(s,&FDSET))
// 						{
// 							__leave;
// 						}
// 					}
// 				}
// 			}
// 			
// 			m_hRecvEvent = WSACreateEvent();
// 			if (!m_hRecvEvent)
// 				__leave;
// 			m_hSendEvent = WSACreateEvent();
// 			if (!m_hSendEvent)
// 				__leave;
// 			m_hCloseEvent = WSACreateEvent();
// 			if (!m_hCloseEvent)
// 				__leave;
// 			WSAResetEvent(m_hSendEvent);
// 			WSAResetEvent(m_hRecvEvent);
// 			WSAResetEvent(m_hCloseEvent);
// 			
// 			if (SOCKET_ERROR == WSAEventSelect(s, m_hCloseEvent, FD_CLOSE))
// 				__leave;
// 			memcpy(&m_RemoteAddr, &ServerAddr, sizeof(SOCKADDR_IN));
// 			nResult = 0;
// 			dwErrorCode = 0;
// 		}
// 		__finally
// 		{
// 			if (nResult != 0)
// 			{
// 				dwErrorCode = WSAGetLastError();
// 				if (dwErrorCode == 0)
// 				{
// 					dwErrorCode = WSAECONNREFUSED;
// 				}
// 				if (m_hRecvEvent)
// 				{
// 					WSACloseEvent(m_hRecvEvent);
// 					m_hRecvEvent = NULL;
// 				}
// 				if (m_hSendEvent)
// 				{
// 					WSACloseEvent(m_hSendEvent);
// 					m_hSendEvent = NULL;
// 				}
// 				if (m_hCloseEvent)
// 				{
// 					WSACloseEvent(m_hCloseEvent);
// 					m_hCloseEvent = NULL;
// 				}
// 			}
// 		}
// 		return dwErrorCode;
// 	}
// 	int ReConnect()
// 	{
// 		TraceMsgA("%s.\n", __FUNCTION__);
// 		DWORD dwErrorCode = 0;
// 		int nResult = SOCKET_ERROR;
// 		__try
// 		{
// 			if (s != INVALID_SOCKET)
// 				__leave;
// 
// 			if (CreateSocket() != 0)
// 				__leave;
// 
// 			int nNonBlock = 1;
// 			dwErrorCode = ioctlsocket(s, FIONBIO, (unsigned long*)&nNonBlock);
// 			if (dwErrorCode != 0)
// 				__leave;
// 			
// 			if (::connect(s, (LPSOCKADDR)&m_RemoteAddr, sizeof(SOCKADDR_IN)))
// 			{
// 				dwErrorCode = WSAGetLastError();
// 				if (dwErrorCode != WSAEWOULDBLOCK)
// 					__leave;
// 				
// 				fd_set FDSET;
// 				FD_ZERO(&FDSET);
// 				FD_SET(s, &FDSET);
// 				
// 				nResult = select(0, 0, &FDSET, 0, &m_tConnectTimeout);
// 				switch (nResult)
// 				{
// 				case SOCKET_ERROR:
// 				case 0:
// 				{
// 					__leave;
// 				}
// 				break;
// 				default:
// 				{
// 					if (!FD_ISSET(s, &FDSET))
// 					{
// 						__leave;
// 					}
// 				}
// 				}
// 			}
// 
// 			if (m_hRecvEvent)
// 				WSACloseEvent(m_hRecvEvent);
// 			m_hRecvEvent = WSACreateEvent();
// 			if (!m_hRecvEvent)
// 				__leave;
// 			if (m_hSendEvent)
// 				WSACloseEvent(m_hSendEvent);
// 			m_hSendEvent = WSACreateEvent();
// 			if (!m_hSendEvent)
// 				__leave;
// 			if (m_hCloseEvent)
// 				WSACloseEvent(m_hCloseEvent);
// 			m_hCloseEvent = WSACreateEvent();
// 			if (!m_hCloseEvent)
// 				__leave;
// 			WSAResetEvent(m_hSendEvent);
// 			WSAResetEvent(m_hRecvEvent);
// 			WSAResetEvent(m_hCloseEvent);
// 
// 			if (SOCKET_ERROR == WSAEventSelect(s, m_hCloseEvent, FD_CLOSE))
// 				__leave;
// 			
// 			nResult = 0;
// 			dwErrorCode = 0;
// 		}
// 		__finally
// 		{
// 			if (nResult != 0)
// 			{
// 				dwErrorCode = WSAGetLastError();
// 				if (dwErrorCode == 0)
// 				{
// 					dwErrorCode = WSAECONNREFUSED;
// 				}
// 				if (m_hRecvEvent)
// 				{
// 					WSACloseEvent(m_hRecvEvent);
// 					m_hRecvEvent = NULL;
// 				}
// 				if (m_hSendEvent)
// 				{
// 					WSACloseEvent(m_hSendEvent);
// 					m_hSendEvent = NULL;
// 				}
// 				if (m_hCloseEvent)
// 				{
// 					WSACloseEvent(m_hCloseEvent);
// 					m_hCloseEvent = NULL;
// 				}
// 			}
// 		}
// 		return dwErrorCode;
// 	}
// 	///////////////////////////////////////////////////////////////////
// 	// ��ʾ��������ɶ˿��ϵĴ���
// 	void HandleError(const DWORD dwErrorCode )
// 	{
// 		TraceMsgA("%s.\n", __FUNCTION__);
// 		switch(dwErrorCode)
// 		{
// 		case WSA_WAIT_TIMEOUT:		
// 			
// 			break;
// 			// ����������һ������
// 		case ERROR_OPERATION_ABORTED:
// 			//case WSA_OPERATION_ABORTED:
// 			// The overlapped operation has been canceled due to the closure of the socket, the execution of the "SIO_FLUSH" command in WSAIoctl, 
// 			// or the thread that initiated the overlapped request exited before the operation completed. 
// 			//  �ص�������Ϊsocket�ѹرձ�ȡ�����ڵ���WSAIoctlִ��SIO_FLUSH����ʱ���ʼ���ص�������߳��ڲ������ǰ�˳�
// 		case WSAENETRESET:
// 			// For a stream socket, the connection has been broken due to keep-alive activity detecting a failure while the operation was in progress. 
// 			// For a datagram socket, this error indicates that the time to live has expired.
// 			// ���ò����ڽ����У����ڱ��ֻ�Ĳ�����⵽һ�����ϣ��������жϡ� 
// 			// ������socket,�������ڽ���ʱ,Keep-alive����һ��ʧ�ܶ��Ͽ������ӡ�
// 			// ���ڱ���socket,�ô����ͷ����ʱ���Ѿ�ʧЧ��
// 		case WSAECONNABORTED:
// 			// The virtual circuit was terminated due to a time-out or other failure.
// 			// ��ʱ������ʧ�ܵ��������·����ֹ
// 			// ���������е������ֹ��һ���ѽ��������ӡ�
// 		case WSAECONNRESET:
// 			// For a stream socket, the virtual circuit was reset by the remote side. The application should close the socket as it is no longer usable. 
// 			// For a UDP datagram socket, this error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message.
// 			// ������socket,���������·��Զ�̶˸�λ.Ӧ�ó����ʱӦ�رյ�ǰ��socket,������Ѳ�����.
// 			// ����UDP����socket,�ô��������һ��send����������һ�� ICMP "�˿��޷�����"��Ϣ
// 			// Զ������ǿ�ȹر���һ�����е����ӡ�
// 		case WSAESHUTDOWN:
// 			// The socket has been shut down; it is not possible to WSASend on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH.
// 			// ������ǰ�Ĺرյ��ã��׽������Ǹ������Ѿ��رգ����ͻ�������ݵ�����û�б����ܡ�
// 		{
// 			TraceMsgA("%s Error on SOCKET��%d�� = %d,Now Try to Reconnect.\n", __FUNCTION__,s,dwErrorCode);
// 			DisConnect(s);
// 			if (ReConnect())
// 			{
// 
// 			}
// 			break;
// 		}
// 		case WSAEFAULT:
// 			// The lpBuffers, lpNumberOfBytesSent, lpOverlapped, lpCompletionRoutine parameter is not totally contained in a valid part of the user address space.
// 			// ����lpBuffers,lpNumberOfBytesSent,lpOverlapped,lpCompletionRoutineδ��ȫ�������û�����Ч��ַ�ռ���,����Щ���������˷��û��ռ�ĵ�ַ
// 			// ϵͳ��⵽��һ�������г���ʹ��ָ�����ʱ����Чָ���ַ�� 
// 		case WSAENETDOWN:
// 			// The network subsystem has failed.
// 			// �׽��ֲ���������һ�����������硣 
// 		case WSAENOTCONN:
// 			// The socket is not connected.
// 			// �����׽���û�����Ӳ���(��ʹ��һ�� sendto ���÷������ݱ��׽���ʱ)û���ṩ��ַ�����ͻ�������ݵ�����û�б����ܡ� 
// 		case WSAENOTSOCK:
// 			// The descriptor is not a socket.
// 			// ��һ�����׽����ϳ�����һ�������� 
// 		
// 		case ERROR_NETNAME_DELETED:
// 		case WSAENOBUFS:	
// 			// The Windows Sockets provider reports a buffer deadlock.
// 			// ����ϵͳ�������ռ䲻����������������ִ���׽����ϵĲ���	
// 			{	
// 				DisConnect(s);
// 				s = INVALID_SOCKET;
// 			}
// 			break;		
// 		default:
// 			break;
// 		}
// 	}
// };
