#pragma once
#include "stdafx.h"
#include <winsock2.h>
#include <MSWSock.h>
//#include <memory>
#include <boost/smart_ptr.hpp>
#include <MSTcpIP.h>
#include <time.h>
#include <list>
#include "Utility.h"
#include "TimeUtility.h"
#include "AutoLock.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
//using namespace std::tr1;
using namespace boost;

#ifndef	_X64
#define _X64
#endif


struct _TCP_KEEPALIVE 
{ 
	ULONG	Onoff; 
	ULONG	nKeepAliveTime; 
	ULONG	nKeepAliveInterval; 
};

// 发送心跳间隔,单位秒,超过这处时间没有数据收发，则断开该连接，默认为30秒
#define _HeartBeatTime_Second		30
#define _Max_Recv_Error				3
#define MAX_BUFFER_LEN				2048  
#define _1M							(1024*1024)
#define _64M						(_1M*64)

/// @brief 标准异步回调函数
typedef long(__stdcall *STDCALLBACK)(WPARAM wp, LPARAM lp);
class CSocketClient
{
public:
	UINT			m_nSize;
	WSAEVENT		m_hSendEvent;
	WSAEVENT		m_hRecvEvent;
	WSAEVENT		m_hCloseEvent;
	SOCKET			s;
	bool			m_bPassive;								/**< 为true时，则为在服务端被动接入的连接，为false时，为客户端主动接入的连接*/
	bool			m_bBindIOCP;
															/**< 默认为false*/
	char*			m_pszRecvBuffer;						/**< 接收数据的缓冲区*/
	CRITICAL_SECTION	m_csRecvBuffer;
	UINT			m_nRecvBufferSize;						/**< 接收数据缓冲区长度*/
	UINT			m_nRecvBufferLength;					/**< 接收到的数据长度*/
	SOCKADDR_IN		m_RemoteAddr;	
	time_t			m_tLastActive;							/**< 客房端最后一次活动时间*/
	static LPFN_CONNECTEX	m_pfnConnectEx;
	LPFN_DISCONNECTEX	m_pfnDisConnectEx;					/**< m_pfnDisConnectEx使用m_pfnDisConnectEx断开的Socket,不用CloseSocket关闭，可以作新的Socket直接投递AcceptEx操作*/
	STDCALLBACK		m_pfnClientCallBack;					/**< 客户端专用回调函数*/

	UINT64			m_nClientID;							/**< 客户端ID*/
	UINT			m_nPacketFailed;						/**< 解析数据包出错的次数,如果出错次数超过_Max_Recv_Error,则断开连接*/
	UINT			m_nHeartBeatInterval_Second;			/**< 客户端跳间隔,心跳建议值为收发超时值的2倍*/
	
	UINT			m_nRecvContextBuffLength;				/**< 单次接收操作缓冲区的长度,默认为2048*/
#ifdef _X64
	UINT64			m_nFrameID;
	UINT64			m_nRecvBytes;
	UINT64			m_nSendBytes;
	UINT64			m_nContextBaseID;
#else
	UINT			m_nFrameID;
	UINT			m_nRecvBytes;
	UINT			m_nSendBytes;
	UINT			m_nContextBaseID;
#endif
	UINT			m_nRecvTimeoutms;						/**< 接收数据超时周期，单位为毫秒*/
	UINT			m_nSendTimeoutms;						/**< 发送数据超时周期，单位为毫秒*/
	WORD			m_nSendRetryCount;						/**< 发送已重试次数 */
	WORD			m_nRecvRetryCount;						/**< 接收已重试次数*/
	void*			m_pCustomData;
	
	int				m_nContextCount;
	
	CSocketClient(UINT nSendTimeoutms = 5000,UINT nRecvTimeOutms = 5000)
	{
		ZeroMemory(this, sizeof(CSocketClient)); 
		m_nSize = sizeof(CSocketClient);
		
		s = INVALID_SOCKET;	
		m_nRecvContextBuffLength = MAX_BUFFER_LEN;
		m_pszRecvBuffer = new char[MAX_BUFFER_LEN];
		ZeroMemory(m_pszRecvBuffer,MAX_BUFFER_LEN);
		m_nRecvBufferSize = MAX_BUFFER_LEN;
		m_nSendTimeoutms = nRecvTimeOutms;
		m_nRecvTimeoutms = nRecvTimeOutms;
		m_nHeartBeatInterval_Second = 30;		
		::InitializeCriticalSection(&m_csRecvBuffer);
	}

	// 释放所有Context，并重置定时器
	// 必须确定当前连接不是活动连接时才用重置此连接
	void Reset()
	{
		// TraceMsgA("@Reset SocketClient[%I64u] m_nContextCount = %d.\n",m_nClientID,m_nContextCount);
		if (m_pszRecvBuffer)
		{
			ZeroMemory(m_pszRecvBuffer,m_nRecvBufferSize);
			m_nRecvBufferLength = 0;
		}
		m_nRecvContextBuffLength = MAX_BUFFER_LEN;
		m_nRecvRetryCount = 0;
		m_nSendRetryCount = 0;
		m_pCustomData = NULL;
		m_nPacketFailed = 0;
		m_nFrameID = 0;
		m_nRecvBytes = 0;
		m_nSendBytes = 0;
		m_nContextBaseID = 0;		
		m_pCustomData = NULL;
	}
	// 释放掉Socket
	~CSocketClient()
	{
		if( s!=INVALID_SOCKET)
		{
			DisConnect(s);
		}		
		if (m_hRecvEvent)
		{
			WSACloseEvent(m_hRecvEvent);
			m_hRecvEvent = NULL;
		}
		if (m_hSendEvent)
		{
			WSACloseEvent(m_hSendEvent);
			m_hSendEvent = NULL;
		}
		if (m_hCloseEvent)
		{
			WSACloseEvent(m_hCloseEvent);
			m_hCloseEvent = NULL;
		}
		Reset();
		if (m_pszRecvBuffer)
		{
			delete m_pszRecvBuffer;
			m_pszRecvBuffer = NULL;
		}
		::DeleteCriticalSection(&m_csRecvBuffer);	
		TraceMsgA("SocketClient[%I64u] is Deleted.\n",m_nClientID);
	}
	// 重置缓冲区内容
	void ResetBuffer()
	{
		ZeroMemory(m_pszRecvBuffer,m_nRecvBufferSize);
	}

	inline void SetCustomData(void *pData)
	{
		m_pCustomData = pData;
	}

	inline void *GetCustomData()
	{
		return m_pCustomData;
	}

#define PostSend	Send
	int Send(char *szBuffer,int nBufferLen,DWORD &nSentBytes,char *pCustumAddr = NULL,int nTimeoutms = 0)
	{
		if (!szBuffer || !nBufferLen)
			return SOCKET_ERROR;
		DWORD dwBytes = 0;
		DWORD dwFlags = 0;
		WSABUF wsa;		
		wsa.buf = szBuffer;
		wsa.len = nBufferLen;
		DWORD dwErrorCode = 0;
		WSAOVERLAPPED ovl;
		ovl.hEvent = m_hSendEvent;
		WSAResetEvent(m_hSendEvent);
		if (WSASend(s, &wsa, 1, &nSentBytes, 0, &ovl, 0) == 0)
			return 0;
		else
		{
			dwErrorCode = WSAGetLastError();
			if (dwErrorCode != WSA_IO_PENDING)
			{
				HandleError(dwErrorCode);
				return dwErrorCode;
			}
			int nWaitTimeout = m_nSendTimeoutms;
			if (nTimeoutms != 0)
				nWaitTimeout = nTimeoutms;
			int nResult = WSAWaitForMultipleEvents(1, &m_hSendEvent, TRUE, nWaitTimeout, TRUE);
			if (nResult == WSA_WAIT_FAILED)
				return WSAGetLastError();
			else if (nResult == WSA_WAIT_TIMEOUT)
			{
				TraceMsgA("Client[%u ]@Send data timeout.\n", (UINT)m_nClientID);
				return WSA_WAIT_TIMEOUT;
			}
			DWORD dwFlags = 0;
			if (WSAGetOverlappedResult(s, &ovl, &nSentBytes, FALSE, &dwFlags))
			{
				return 0;
			}
			else
				return  ::WSAGetLastError();
		}
		
	}

	int Recv(INOUT char *szBuffer,IN int nBuffersize,OUT DWORD &nDataRecved)
	{
		if (s == INVALID_SOCKET)
			return SOCKET_ERROR;

		DWORD dwErrorCode = 0;
		WSAOVERLAPPED ovl;
		ZeroMemory(&ovl, sizeof(ovl));
		ovl.hEvent = m_hRecvEvent;
		WSAResetEvent(m_hRecvEvent);

		WSABUF wsa;
		wsa.buf = szBuffer;
		wsa.len = nBuffersize;	

		DWORD dwFlags = 0;	

		int nResult = WSARecv(s, &wsa, 1, &nDataRecved, &dwFlags, &ovl,0);
		if (nResult ==0)		// 数据接收成功
			return 0;
		if (nResult == SOCKET_ERROR )
		{	
			DWORD dwErrorCode = WSAGetLastError();
			if (dwErrorCode != WSA_IO_PENDING)
			{
				HandleError(dwErrorCode);
				return dwErrorCode;
			}
			int nResult = WSAWaitForMultipleEvents(1, &m_hRecvEvent, TRUE, m_nRecvTimeoutms, TRUE);
			if (nResult == WSA_WAIT_FAILED)
				return WSAGetLastError();
			else if (nResult == WSA_WAIT_TIMEOUT)
				return WSAGetLastError();

			DWORD dwFlags = 0;
			if (WSAGetOverlappedResult(s, &ovl, &nDataRecved, FALSE, &dwFlags))
				return 0;
			else
				return  ::WSAGetLastError();
		}	
		return 0;
	}

	int	CreateSocket()
	{
		DWORD dwResult = SOCKET_ERROR;
		__try
		{
			s = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (s == INVALID_SOCKET)
				__leave;

			//设置socket 选项
			//1.允许优雅关闭socket
			linger sLinger;
			sLinger.l_onoff = 1;	//(在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
			// 如果m_sLinger.l_onoff = 0;则功能和2.)作用相同;
			sLinger.l_linger = 2;//(容许逗留的时间为秒)
			if (setsockopt(s,SOL_SOCKET,SO_LINGER,(const char*)&sLinger,sizeof(linger)) == SOCKET_ERROR)
				__leave;
			// 对于非阻塞socket,SO_SNDTIMEO和SO_RCVTIMEO选项无效
// 			if (m_nSendTimeout > 0)
// 			{
// 				//2.设置收发超时时限
// 				if (setsockopt(s,SOL_SOCKET,SO_SNDTIMEO,(char *)&m_nSendTimeout,sizeof(int)) == SOCKET_ERROR)
// 					__leave;
// 			}
// 			if (m_nRecvTimeoutMs > 0)
// 			{
// 				//接收时限
// 				if (setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char *)&m_nRecvTimeoutMs,sizeof(int)) == SOCKET_ERROR)
// 					__leave;
// 			}

			// 设置TCP_KEEPALIVE属性
			_TCP_KEEPALIVE KeepAliveIn = {0},KeepALiveOut = {0}; 
			KeepAliveIn.nKeepAliveTime		= 5000;		// 探测频率,5000ms一次
			KeepAliveIn.nKeepAliveInterval	= 2000;		// 探测失败后连重试频率,2000ms一次
			KeepAliveIn.Onoff				= TRUE;  
			BOOL bKeepAliveEnable			= TRUE;
			ULONG nInLen					= sizeof(_TCP_KEEPALIVE);
			ULONG nOutLen					= sizeof(_TCP_KEEPALIVE);
			ULONG nBytesReturn				= 0;
			// 设置此选项后,如果对方关闭了socket连接,再进行收发操作时，返立即返回WSAENETRESET错误,
			// 并且之后所有的的收发操作都会返回WSAENOTCONN错误
			if (setsockopt(s,SOL_SOCKET,SO_KEEPALIVE,(char *)&bKeepAliveEnable,sizeof(BOOL)) != 0 )
				__leave;

			if (WSAIoctl(s, SIO_KEEPALIVE_VALS, (LPVOID)&KeepAliveIn, nInLen,(LPVOID)&KeepALiveOut, nOutLen, &nBytesReturn, NULL, NULL) == SOCKET_ERROR)
				__leave;
			dwResult =  0;
		}
		__finally
		{
			if (dwResult)
			{
				dwResult = WSAGetLastError();
				if (INVALID_SOCKET != s)
					closesocket(s);
				s = INVALID_SOCKET;
			}
		}
		return dwResult;
	}
	static void DisConnect(SOCKET &socket)
	{
		linger sLinger;
		sLinger.l_onoff = 1;	//(在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
		// 如果m_sLinger.l_onoff = 0;则功能和2.)作用相同;
		sLinger.l_linger = 2;//优雅关闭
		__try
		{
			if (socket == INVALID_SOCKET)
				__leave;
			if (setsockopt(socket,SOL_SOCKET,SO_LINGER,(const char*)&sLinger,sizeof(linger)) != SOCKET_ERROR)
			{//优雅关闭
				if (shutdown(socket,SD_BOTH) != SOCKET_ERROR)
				{
					char szBuff[1023] = {0};
					recv(socket,szBuff,1023,0);
					closesocket(socket);
					__leave;
				}
				else
				{
					shutdown(socket,SD_BOTH);
					closesocket(socket);
					__leave;
				}
			}
			else
			{//强制关闭
				shutdown(socket,SD_BOTH);
				closesocket(socket);
				__leave;
			}
		}
		__finally
		{
			socket = INVALID_SOCKET;
		}
	}
	// 重叠I/O模型进行同步连接
	int Connect(const CHAR *szIp,int nPort,UINT nConnectTimeout = 5000)
	{	
		DWORD dwErrorCode = 0;
		int nResult = SOCKET_ERROR;
		__try
		{			
			if (s != INVALID_SOCKET)
				__leave;

			if (CreateSocket() != 0)
				__leave;

			int nNonBlock = 1;
			dwErrorCode		= ioctlsocket(s, FIONBIO, (unsigned long*)&nNonBlock);
			if(dwErrorCode != 0)
				__leave;

			SOCKADDR_IN ServerAddr;
			ServerAddr.sin_port=htons((u_short)nPort);
			ServerAddr.sin_family=AF_INET;	
			ServerAddr.sin_addr.s_addr =  inet_addr((LPCSTR)szIp);

			if (::connect(s, (LPSOCKADDR)&ServerAddr, sizeof(SOCKADDR_IN)))
			{
				dwErrorCode = WSAGetLastError();
				if (dwErrorCode != WSAEWOULDBLOCK)
					__leave;

				struct timeval timeout ;
				fd_set FDSET;
				FD_ZERO(&FDSET);
				FD_SET(s, &FDSET);
				timeout.tv_sec = nConnectTimeout/1000;
				timeout.tv_usec = nConnectTimeout%1000;	
				nResult = select(0, 0, &FDSET, 0, &timeout);
				switch(nResult)
				{
				case SOCKET_ERROR:
				case 0:
					{
						__leave;
					}
					break;
				default:
					{
						if (!FD_ISSET(s,&FDSET))
						{
							__leave;
						}
					}
				}
			}
			m_hRecvEvent = WSACreateEvent();
			if (!m_hRecvEvent)
				__leave;
			m_hSendEvent = WSACreateEvent();
			if (!m_hSendEvent)
				__leave;
			m_hCloseEvent = WSACreateEvent();
			if (!m_hCloseEvent)
				__leave;
			WSAResetEvent(m_hSendEvent);
			WSAResetEvent(m_hRecvEvent);
			WSAResetEvent(m_hCloseEvent);
			if (SOCKET_ERROR == WSAEventSelect(s, m_hCloseEvent, FD_CLOSE))
				__leave;
			nResult = 0;
			dwErrorCode = 0;
		}
		__finally
		{
			if (nResult != 0)
			{
				dwErrorCode = WSAGetLastError();
				if (dwErrorCode == 0)
				{
					dwErrorCode = WSAECONNREFUSED;
				}
				if (m_hRecvEvent)
				{
					WSACloseEvent(m_hRecvEvent);
					m_hRecvEvent = NULL;
				}
				if (m_hSendEvent)
				{
					WSACloseEvent(m_hSendEvent);
					m_hSendEvent = NULL;
				}
				if (m_hCloseEvent)
				{
					WSACloseEvent(m_hCloseEvent);
					m_hCloseEvent = NULL;
				}
			}
		}
		return dwErrorCode;
	}

	///////////////////////////////////////////////////////////////////
	// 显示并处理完成端口上的错误
	void HandleError(const DWORD dwErrorCode )
	{
		switch(dwErrorCode)
		{
		case ERROR_OPERATION_ABORTED:
		//case WSA_OPERATION_ABORTED:
			// The overlapped operation has been canceled due to the closure of the socket, the execution of the "SIO_FLUSH" command in WSAIoctl, 
			// or the thread that initiated the overlapped request exited before the operation completed. 
			//  重叠操作因为socket已关闭被取消，在调用WSAIoctl执行SIO_FLUSH命令时或初始化重叠请求的线程在操作完成前退出
			// 
			break;
		case WSA_WAIT_TIMEOUT:		
			// 本机放弃了一个操作
		case WSAECONNABORTED:
			// The virtual circuit was terminated due to a time-out or other failure.
			// 因超时或其它失败导致虚拟回路被中止
			// 您的主机中的软件中止了一个已建立的连接。
		case WSAECONNRESET:
			// For a stream socket, the virtual circuit was reset by the remote side. The application should close the socket as it is no longer usable. 
			// For a UDP datagram socket, this error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message.
			// 对于流socket,表明虚拟回路被远程端复位.应用程序此时应关闭当前的socket,因此其已不可用.
			// 对于UDP报文socket,该错误表明上一个send操作导致了一个 ICMP "端口无法到达"消息
			// 远程主机强迫关闭了一个现有的连接。
		case WSAEFAULT:
			// The lpBuffers, lpNumberOfBytesSent, lpOverlapped, lpCompletionRoutine parameter is not totally contained in a valid part of the user address space.
			// 参数lpBuffers,lpNumberOfBytesSent,lpOverlapped,lpCompletionRoutine未完全包含在用户的有效地址空间中,即这些参数引用了非用户空间的地址
			// 系统检测到在一个调用中尝试使用指针参数时的无效指针地址。 
		case WSAENETDOWN:
			// The network subsystem has failed.
			// 套接字操作遇到了一个已死的网络。 
		case WSAENETRESET:
			// For a stream socket, the connection has been broken due to keep-alive activity detecting a failure while the operation was in progress. 
			// For a datagram socket, this error indicates that the time to live has expired.
			// 当该操作在进行中，由于保持活动的操作检测到一个故障，该连接中断。 
			// 对于流socket,当接正在进行时,Keep-alive活动检测一个失败而断开了连接。
			// 对于报文socket,该错误表头生存时间已经失效。
		case WSAENOTCONN:
			// The socket is not connected.
			// 由于套接字没有连接并且(当使用一个 sendto 调用发送数据报套接字时)没有提供地址，发送或接收数据的请求没有被接受。 
		case WSAENOTSOCK:
			// The descriptor is not a socket.
			// 在一个非套接字上尝试了一个操作。 
		case WSAESHUTDOWN:
			// The socket has been shut down; it is not possible to WSASend on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH.
			// 由于以前的关闭调用，套接字在那个方向已经关闭，发送或接收数据的请求没有被接受。
		case ERROR_NETNAME_DELETED:
		case WSAENOBUFS:	
			// The Windows Sockets provider reports a buffer deadlock.
			// 由于系统缓冲区空间不足或队列已满，不能执行套接字上的操作	
			{				
				DisConnect(s);
#ifdef _X64
				TraceMsgA("SocketClient[%I64u] 被关闭.\n", m_nClientID);
#else
				TraceMsgA("SocketClient[%u] 被关闭.\n",m_nClientID);
#endif
				s = INVALID_SOCKET;
				
			}
			break;		
			
		default:
			break;
		}
	}
};
