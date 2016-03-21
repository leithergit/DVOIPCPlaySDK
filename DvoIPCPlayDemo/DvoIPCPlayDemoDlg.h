
// DvoIPCPlayDemoDlg.h : 头文件
//

#pragma once

#include "VideoFrame.h"
#include "AutoLock.h"
#include <memory>
#include "GlliteryStatic.h"
#include "fullscreen.h"
#include "SocketClient.h"
using namespace std;
using namespace std::tr1;

#define _Frame_PERIOD			30.0f		///< 一个帧率区间
#define WM_UPDATE_STREAMINFO	WM_USER + 1024
#define WM_UPDATE_PLAYINFO		WM_USER + 1025

struct FrameInfo
{
	time_t  tRecvTime;				///< 收到帧的时间 单位毫秒
	int		nFrameSize;
};

/// @brief 解析DVO私有帧结构体
struct FrameParser
{
	DVOFrameHeaderEx	*pHeaderEx;		///< DVO私有录像的帧数据
	UINT				nFrameSize;		///< pFrame的数据长度
	byte				*pLawFrame;		///< 原始码流数据
	UINT				nLawFrameSize;	///< 原始码流数据长度
};

struct StreamInfo
{
	UINT	nFrameID;
	bool	bRecvIFrame;		///< 是否收到第个I帧
	int		nVideoFrameCount;
	int		nAudioFrameCount;
	__int64 nVideoBytes;		///< 收到视频总字节数
	__int64 nAudioBytes;		///< 收到音频总字节数
	time_t  tFirstTime;			///< 接收数据的起始时间	单位毫秒
	time_t	tLastTime;			///< 最后一次接收数据时间 单位毫秒
	CRITICAL_SECTION	csFrameList;
	list<FrameInfo>FrameInfoList;
	StreamInfo()
	{
		ZeroMemory(this, offsetof(StreamInfo, FrameInfoList));
		::InitializeCriticalSection(&csFrameList);
	}
	~StreamInfo()
	{
		::DeleteCriticalSection(&csFrameList);
	}

	int GetFrameRate()
	{
		CAutoLock lock(&csFrameList);
		if (FrameInfoList.size() > 2)
		{
			time_t tSpan = FrameInfoList.back().tRecvTime - FrameInfoList.front().tRecvTime;
			if (!tSpan)
				tSpan = 1000;
			int nSize = FrameInfoList.size();
			return nSize * 1000 / (int)(tSpan);
		}
		else
			return 1;
	}

	int	GetVideoCodeRate(int nUnit = 1024)	///< 取得视频码率(Kbps)
	{
		time_t tSpan = 25;
		CAutoLock lock(&csFrameList);
		if (FrameInfoList.size() > 1)
			tSpan = FrameInfoList.back().tRecvTime - FrameInfoList.front().tRecvTime;
		__int64 nTotalBytes = 0;

		for (list<FrameInfo>::iterator it = FrameInfoList.begin();
			it != FrameInfoList.end();
			it++)
			nTotalBytes += (*it).nFrameSize;

		if (tSpan == 0)
			tSpan = 1;
		__int64 nRate = (nTotalBytes * 1000 * 8 / ((int)tSpan*nUnit));
		return (int)nRate;
	}
	int GetAudioCodeRate(int nUnit = 1024)
	{
		time_t tSpan = (tLastTime - tFirstTime);
		if (tSpan == 0)
			tSpan = 1;
		return (int)(nAudioBytes * 1000 / (tSpan*nUnit));
	}
	void inline Reset()
	{
		ZeroMemory(this, offsetof(StreamInfo, csFrameList));
		CAutoLock lock(&csFrameList);
		FrameInfoList.clear();
	}
	void PushFrameInfo(int nFrameSize)
	{
		time_t tNow = (time_t)(GetExactTime() * 1000);
		CAutoLock lock(&csFrameList);
		if (FrameInfoList.size() > 1)
		{
			for (list<FrameInfo>::iterator it = FrameInfoList.begin();
				it != FrameInfoList.end();)
			{
				if ((tNow - (*it).tRecvTime) > _Frame_PERIOD * 1000)
					it = FrameInfoList.erase(it);
				else
					it++;
			}
		}

		FrameInfo fi;
		fi.tRecvTime = tNow;
		fi.nFrameSize = nFrameSize;
		FrameInfoList.push_back(fi);
	}
};

struct PlayerContext
{
public:
	StreamInfo*	pStreamInfo;
	USER_HANDLE hUser;
	REAL_HANDLE hStream;
	CSocketClient *pClient;
	volatile bool bThreadRecvIPCStream = false;
	HANDLE hThreadRecvStream = nullptr;
	fnDVOCallback_RealAVData_T pStreamCallBack = nullptr;;
	int nPlayerCount;
	DVO_PLAYHANDLE	hPlayer[36];
	DVO_PLAYHANDLE	hPlayerStream;		// 流播放句柄
	HWND		hWndView;
	int			nItem;
	void*		pThis;
	CFile		*pRecFile;
	bool		bRecvIFrame;
	time_t		tLastFrame;
	int			nFPS;
	UINT		nVideoFrameID;
	UINT		nAudioFrameID;
	int			nAudioCodec;
	bool		bWriteMediaHeader;
	time_t		tRecStartTime;
	UINT64		nRecFileLength;
	TCHAR		szIpAddress[32];
	TCHAR		szRecFilePath[MAX_PATH];
	UINT		nTimeStamp[100];
	int			nTimeCount;

	CRITICAL_SECTION csRecFile;
public:
	PlayerContext(USER_HANDLE hUserIn,
		REAL_HANDLE hStreamIn = -1,
		DVO_PLAYHANDLE hPlayerIn = nullptr,int nCount = 1)
	{
		ZeroMemory(this, sizeof(PlayerContext));
		pStreamInfo = new StreamInfo();
		nTimeCount = 0;
		hUser = hUserIn;
		hStream = hStreamIn;
		nPlayerCount = nCount;
		hPlayer[0] = hPlayerIn;
		InitializeCriticalSection(&csRecFile);
	}
	~PlayerContext()
	{
		StopRecord();
		if (hStream != -1)
		{
			DVO2_NET_StopRealPlay(hStream);
			hStream = -1;
		}
		TraceMsgA("%s Now() = %.5f.\n", __FUNCTION__, GetExactTime());
		for (int i = 0; i < nPlayerCount;i ++)
		if (hPlayer[i])
			dvoplay_Close(hPlayer[i]);
		ZeroMemory(hPlayer, sizeof(DVO_PLAYHANDLE));
		if (hPlayerStream)
		{
			dvoplay_Close(hPlayerStream);
			hPlayerStream = nullptr;
		}
		if (pClient)
		{
			delete pClient;
			pClient = nullptr;
		}
		if (hUser != -1)
		{
			DVO2_NET_Logout(hUser);
			hUser = -1;
		}
		if (pStreamInfo)
			delete pStreamInfo;
		
		DeleteCriticalSection(&csRecFile);
	}
	void StartRecv(fnDVOCallback_RealAVData_T pCallBack)
	{
		bThreadRecvIPCStream = true;
		pStreamCallBack = pCallBack;
		hThreadRecvStream = CreateThread(nullptr, 0, ThreadRecvIPCStream, this, 0, nullptr);
	}
	void StopRecv()
	{
		if (hThreadRecvStream)
		{
			bThreadRecvIPCStream = false;
			WaitForSingleObject(hThreadRecvStream, INFINITE);
			CloseHandle(hThreadRecvStream);
			hThreadRecvStream = nullptr;
		}
	}
	

	static	DWORD WINAPI ThreadRecvIPCStream(void *p)
	{
		PlayerContext *pThis = (PlayerContext *)p;
		if (!pThis->pClient)
			return 0;
		CSocketClient *pClient = pThis->pClient;
		MSG_HEAD MsgHeader;
		DWORD nBytesRecved = 0;
		int nBufferSize = 64 * 1024;
		int nDataLength = 0;
		byte *pBuffer = new byte[nBufferSize];

		while (pThis->bThreadRecvIPCStream)
		{
			ZeroMemory(&MsgHeader, sizeof(MSG_HEAD));
			nBytesRecved = 0;
			if (pClient->Recv((char *)&MsgHeader, sizeof(MSG_HEAD), nBytesRecved) == 0 &&
				nBytesRecved == sizeof(MSG_HEAD))
			{
				int nPackLen = ntohl(MsgHeader.Pktlen) - sizeof(MSG_HEAD);
				if (nBufferSize < nPackLen)
				{
					delete[]pBuffer;
					while (nBufferSize < nPackLen)
						nBufferSize *= 2;
					pBuffer = new byte[nBufferSize];
				}
				nDataLength = 0;
				while (nDataLength < nPackLen)
				{
					if (!pClient->Recv((char *)&pBuffer[nDataLength], nPackLen - nDataLength, nBytesRecved) == 0)
						break;
					nDataLength += nBytesRecved;
				}
				app_net_tcp_enc_stream_head_t *pStreamHeader = (app_net_tcp_enc_stream_head_t *)pBuffer;
				pStreamHeader->chn = ntohl(pStreamHeader->chn);
				pStreamHeader->stream = ntohl(pStreamHeader->stream);
				pStreamHeader->frame_type = ntohl(pStreamHeader->frame_type);
				pStreamHeader->frame_num = ntohl(pStreamHeader->frame_num);
				pStreamHeader->sec = ntohl(pStreamHeader->sec);
				pStreamHeader->usec = ntohl(pStreamHeader->usec);

				pThis->pStreamCallBack(-1, -1, 0, (char *)pBuffer, nPackLen, pThis);
				ZeroMemory(pBuffer, nBufferSize);
			}
			Sleep(10);
		}
		if (pBuffer)
			delete[]pBuffer;
		return 0;
	}
	void StartRecord()
	{
		try
		{
			if (_tcslen(szRecFilePath) == 0)
				return;
			CAutoLock lock(&csRecFile);
			pRecFile = new CFile(szRecFilePath, CFile::modeCreate | CFile::modeWrite);
			tRecStartTime = time(nullptr);
		}
		catch (/*std::exception* e*/CException *e)
		{
			TCHAR szError[256] = { 0 };
			e->GetErrorMessage(szError, 256);
			_tprintf(_T("%s %d Exception:%s.\n"), __FILE__, __LINE__, szError/*e->what()*/);
			e->Delete();
		}
	}

	void StopRecord()
	{
		if (!pRecFile)
			return;
		try
		{
			CAutoLock lock(&csRecFile);
			delete pRecFile;
			pRecFile = NULL;
			ZeroMemory(szRecFilePath, sizeof(szRecFilePath));
			tRecStartTime = 0;
		}
		catch (/*std::exception* e*/CException *e)
		{
			TCHAR szError[256] = { 0 };
			e->GetErrorMessage(szError, 256);
			_tprintf(_T("%s %d Exception:%s.\n"), __FILE__, __LINE__, szError/*e->what()*/);
			e->Delete();
			pRecFile = NULL;
		}
	}
};

struct ListItem
{
	TCHAR szItemName[32];
	TCHAR szItemText[256];
};

struct FullScreenWnd
{
	WINDOWPLACEMENT windowedPWP;
	DWORD dwExStyle;
	DWORD dwStyle;
	HMENU hMenu;
	HWND  hWnd;
	FullScreenWnd()
	{
		ZeroMemory(this, sizeof(FullScreenWnd));
	}

	void SetWnd(HWND hWnd)
	{
		this->hWnd = hWnd;
		GetWindowPlacement(hWnd, &windowedPWP);
	}
	void SwitchFullScreen()
	{
		dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
		dwStyle = GetWindowLong(hWnd, GWL_STYLE);
		dwStyle &= ~WS_MAXIMIZE & ~WS_MINIMIZE; // remove minimize/maximize style
		hMenu = GetMenu(hWnd);
		// Hide the window to avoid animation of blank windows
		ShowWindow(hWnd, SW_HIDE);
		// Set FS window style
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_SYSMENU | WS_CLIPCHILDREN);
		SetMenu(hWnd, NULL);
		ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	}
	void Restore()
	{
		if (dwStyle != 0)
			SetWindowLong(hWnd, GWL_STYLE, dwStyle);

		if (dwExStyle != 0)
			SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle);

		if (hMenu != NULL)
			SetMenu(hWnd, hMenu);

		if (windowedPWP.length == sizeof(WINDOWPLACEMENT))
		{
			if (windowedPWP.showCmd == SW_SHOWMAXIMIZED)
			{
				ShowWindow(hWnd, SW_HIDE);
				windowedPWP.showCmd = SW_HIDE;
				SetWindowPlacement(hWnd, &windowedPWP);
				ShowWindow(hWnd, SW_SHOWMAXIMIZED);
				windowedPWP.showCmd = SW_SHOWMAXIMIZED;
			}
			else
				SetWindowPlacement(hWnd, &windowedPWP);
		}
	}
};
// CDvoIPCPlayDemoDlg 对话框
class CDvoIPCPlayDemoDlg : public CDialogEx
{
	// 构造
public:
	CDvoIPCPlayDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_DVOIPCPLAYDEMO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl	m_wndStreamInfo;
	CMFCEditBrowseCtrl	m_wndBrowseCtrl;
	int			m_nListWidth;			// List控件的宽度
	int			m_nListTop;				// List宽度的Top坐标
	TCHAR		m_szRecordPath[MAX_PATH];
	int			m_nMonitorCount;		//  当前已经连接显示器的数量
	CVideoFrame *m_pVideoWndFrame = nullptr;
	CGlliteryStatic m_wndStatus;
	double	m_dfLastUpdate;
	bool SaveSetting();
	bool LoadSetting();
	static CFile *m_pVldReport;
	ListItem m_szListText[16];
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonPlaystream();
	afx_msg void OnBnClickedButtonRecord();
	afx_msg void OnBnClickedButtonPlayfile();
	list<shared_ptr<PlayerContext>>m_listPlayer;
	// 相机实时码流捕捉回调函数
	static void  __stdcall StreamCallBack(IN USER_HANDLE  lUserID,
		IN REAL_HANDLE lStreamHandle,
		IN int         nErrorType,
		IN const char* pBuffer,
		IN int         nDataLen,
		IN void*       pUser);

	static void __stdcall PlayerCallBack(DVO_PLAYHANDLE hPlayHandle, void *pUserPtr);
	shared_ptr<PlayerInfo>	m_pPlayerInfo;
	afx_msg void OnNMClickListConnection(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnUpdateStreamInfo(WPARAM, LPARAM);
	afx_msg LRESULT OnUpdatePlayInfo(WPARAM, LPARAM);
	afx_msg void OnNMCustomdrawListStreaminfo(NMHDR *pNMHDR, LRESULT *pResult);
	shared_ptr<PlayerContext>m_pPlayContext;
	afx_msg void OnBnClickedCheckEnableaudio();
	afx_msg void OnBnClickedCheckFitwindow();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonSnapshot();
	afx_msg void OnCbnSelchangeComboPlayspeed();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	HWND m_hFullScreen = nullptr;
	UINT m_nOriMonitorIndex = 0;
	FullScreenWnd m_FullScreen;
	void *m_hIOCP = nullptr;

	afx_msg LRESULT OnTroggleFullScreen(WPARAM W, LPARAM L)
	{
		if (!m_pPlayContext || !m_pPlayContext->hPlayer[0])
			return 0l;
		if (!m_hFullScreen)
		{// 切到全屏
			m_hFullScreen = (HWND)W;
			HWND hDesktop = ::GetDesktopWindow();	
			m_nOriMonitorIndex = GetMonitorIndexFromWnd(m_hWnd);
			m_FullScreen.SetWnd(m_hFullScreen);
			::SetParent(m_hFullScreen, ::GetDesktopWindow());
			POINT pt;
			pt.x = 0;
			pt.y = 0;
			MoveWnd2Monitor(m_hFullScreen, m_nOriMonitorIndex, pt);				
			m_FullScreen.SwitchFullScreen();
			dvoplay_Reset(m_pPlayContext->hPlayer[0], m_hFullScreen);
			ShowWindow(SW_HIDE);
		}
		else
		{// 切回窗口
			m_FullScreen.Restore();			
			::SetParent(m_hFullScreen, m_pVideoWndFrame->GetSafeHwnd());
			dvoplay_Reset(m_pPlayContext->hPlayer[0], m_hFullScreen);
			m_hFullScreen = nullptr;
			ShowWindow(SW_SHOW);
		}
		return 0;
	}
	bool m_bPuased = false;
	volatile bool m_bThreadStream = false;
	HANDLE  m_hThreadSendStream = nullptr;
	HANDLE  m_hThreadPlayStream = nullptr;
	int		m_nRow = 1;
	int		m_nCol = 1;
	ATOM m_nHotkeyID = 0;
	struct DvoStream
	{
		DvoStream(byte *pBuffer,int nBufferSize)
		{
			pStream = new byte[nBufferSize];
			nStreamSize = nBufferSize;
			if (pStream)
				memcpy(pStream, pBuffer, nBufferSize);
		}
		~DvoStream()
		{
			if (pStream)
				delete[]pStream;
		}
		byte *pStream;
		int  nStreamSize;
	};
	typedef shared_ptr<DvoStream> DvoStreamPtr;
	CRITICAL_SECTION m_csListStream;
	list<DvoStreamPtr> m_listStream;	// 流播放队列
	// 发送数据流
	// 这里只是把数据放入流播放队列来模拟发送操作
	int SendStream(byte *pFrameBuffer, int nFrameSize)
	{
		CAutoLock lock(&m_csListStream);
		DvoStreamPtr pStream = make_shared<DvoStream>(pFrameBuffer, nFrameSize);
		m_listStream.push_back(pStream);
		return m_listStream.size();
	}
	// 接收数据流
	// 这里只是从流队列中取出数据来模拟接收操作
	int RecvStream(DvoStreamPtr &pStream,bool &bRecved)
	{
		CAutoLock lock(&m_csListStream);
		if (m_listStream.size() > 0)
		{
			pStream = m_listStream.front();
			m_listStream.pop_front();
			bRecved = true;
		}
		else
			bRecved = false;
		return m_listStream.size();
	}

	// 解码录像文件线程,一般在服务端,把一帧数的解析出来后放到流队列中
	// 在实际的流媒体服务器中，应该是把数据直接发送到播放客户端去
	static  DWORD WINAPI ThreadSendStream(void *p)
	{
		CDvoIPCPlayDemoDlg *pThis = (CDvoIPCPlayDemoDlg *)p;
		if (!pThis->m_pPlayContext || 
			!pThis->m_pPlayContext->hPlayer )
			return 0;
		shared_ptr<PlayerContext>pPlayCtx = pThis->m_pPlayContext;
		
		byte *pFrameBuffer = nullptr;
		UINT nFrameSize = 0;
				
		int nDvoError = 0;
		int nStreamListSize = 0;
		int nFrames = 0;
		while (pThis->m_bThreadStream)
		{
			// 读取下一帧
			if (nStreamListSize < 8)
			{
				if (nDvoError = dvoplay_GetFrame(pPlayCtx->hPlayer[0], &pFrameBuffer, nFrameSize) != DVO_Succeed)
				{
					TraceMsgA("%s dvoplay_GetFrame Failed:%d.\n", __FUNCTION__, nDvoError);
					Sleep(5);
					continue;
				}
				nFrames++;
				//TraceMsgA("%s nFrames = %d FrameSize = %d.\n", __FUNCTION__, nFrames,nFrameSize);
				nStreamListSize = pThis->SendStream(pFrameBuffer, nFrameSize);
			}
			else
			{
				CAutoLock lock(&pThis->m_csListStream);
				nStreamListSize = pThis->m_listStream.size();
			}
			
			Sleep(5);
		}
		return 0;
	}
	// 流播放线程,从流队列中取出数据进行播放
	// 实际开发中，应该是从服务端接数据进行播放
	static  DWORD WINAPI ThreadPlayStream(void *p)
	{
		CDvoIPCPlayDemoDlg *pThis = (CDvoIPCPlayDemoDlg *)p;
		if (!pThis->m_pPlayContext ||
			!pThis->m_pPlayContext->hPlayerStream)
			return 0;
		shared_ptr<PlayerContext>pPlayCtx = pThis->m_pPlayContext;
		int nDvoError = 0;
		int nStreamListSize = 0;
		DvoStreamPtr pStream;
		bool bRecved = false;
		int nLoopCount = 0;
		bool bInputList = true;
		while (pThis->m_bThreadStream)
		{
			if (bInputList)
			{
				nStreamListSize = pThis->RecvStream(pStream, bRecved);
				if (!bRecved)
				{
					Sleep(5);
					continue;
				}
			}
			
			nLoopCount++;
			if (nDvoError = dvoplay_InputStream(pPlayCtx->hPlayerStream, pStream->pStream, pStream->nStreamSize) != DVO_Succeed)
			{
				
				int nDvoError = dvoplay_GetPlayerInfo(pPlayCtx->hPlayerStream, pThis->m_pPlayerInfo.get());
				TraceMsgA("%s dvoplay_InputStream Failed:%d\tVideocache size = %d.\n", __FUNCTION__, nDvoError, pThis->m_pPlayerInfo->nCacheSize);
				bInputList = false;
				Sleep(5);
				continue;
			}
			
			int nDvoError = dvoplay_GetPlayerInfo(pPlayCtx->hPlayerStream, pThis->m_pPlayerInfo.get());
			if (nDvoError == DVO_Succeed ||
				nDvoError == DVO_Error_FileNotExist)
				pThis->PostMessage(WM_UPDATE_PLAYINFO, (WPARAM)pThis->m_pPlayerInfo.get(), (LPARAM)nDvoError);
			bInputList = true;
			Sleep(5);
		}
		return 0;
	}
	
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonTracecache();
	afx_msg void OnLvnGetdispinfoListStreaminfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonStopbackword();
	afx_msg void OnBnClickedButtonStopforword();
	afx_msg void OnBnClickedButtonSeeknextframe();
	afx_msg void OnBnClickedCheckEnablelog();
};
