
// DvoIPCPlayDemoDlg.h : ͷ�ļ�
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

#define _Frame_PERIOD			30.0f		///< һ��֡������
#define WM_UPDATE_STREAMINFO	WM_USER + 1024
#define WM_UPDATE_PLAYINFO		WM_USER + 1025

struct FrameInfo
{
	time_t  tRecvTime;				///< �յ�֡��ʱ�� ��λ����
	int		nFrameSize;
};

/// @brief ����DVO˽��֡�ṹ��
struct FrameParser
{
	DVOFrameHeaderEx	*pHeaderEx;		///< DVO˽��¼���֡����
	UINT				nFrameSize;		///< pFrame�����ݳ���
	byte				*pLawFrame;		///< ԭʼ��������
	UINT				nLawFrameSize;	///< ԭʼ�������ݳ���
};

struct StreamInfo
{
	UINT	nFrameID;
	bool	bRecvIFrame;		///< �Ƿ��յ��ڸ�I֡
	int		nVideoFrameCount;
	int		nAudioFrameCount;
	__int64 nVideoBytes;		///< �յ���Ƶ���ֽ���
	__int64 nAudioBytes;		///< �յ���Ƶ���ֽ���
	time_t  tFirstTime;			///< �������ݵ���ʼʱ��	��λ����
	time_t	tLastTime;			///< ���һ�ν�������ʱ�� ��λ����
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

	int	GetVideoCodeRate(int nUnit = 1024)	///< ȡ����Ƶ����(Kbps)
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
	DVO_PLAYHANDLE	hPlayerStream;		// �����ž��
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
// CDvoIPCPlayDemoDlg �Ի���
class CDvoIPCPlayDemoDlg : public CDialogEx
{
	// ����
public:
	CDvoIPCPlayDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

	// �Ի�������
	enum { IDD = IDD_DVOIPCPLAYDEMO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


	// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl	m_wndStreamInfo;
	CMFCEditBrowseCtrl	m_wndBrowseCtrl;
	int			m_nListWidth;			// List�ؼ��Ŀ��
	int			m_nListTop;				// List��ȵ�Top����
	TCHAR		m_szRecordPath[MAX_PATH];
	int			m_nMonitorCount;		//  ��ǰ�Ѿ�������ʾ��������
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
	// ���ʵʱ������׽�ص�����
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
		{// �е�ȫ��
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
		{// �лش���
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
	list<DvoStreamPtr> m_listStream;	// �����Ŷ���
	// ����������
	// ����ֻ�ǰ����ݷ��������Ŷ�����ģ�ⷢ�Ͳ���
	int SendStream(byte *pFrameBuffer, int nFrameSize)
	{
		CAutoLock lock(&m_csListStream);
		DvoStreamPtr pStream = make_shared<DvoStream>(pFrameBuffer, nFrameSize);
		m_listStream.push_back(pStream);
		return m_listStream.size();
	}
	// ����������
	// ����ֻ�Ǵ���������ȡ��������ģ����ղ���
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

	// ����¼���ļ��߳�,һ���ڷ����,��һ֡���Ľ���������ŵ���������
	// ��ʵ�ʵ���ý��������У�Ӧ���ǰ�����ֱ�ӷ��͵����ſͻ���ȥ
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
			// ��ȡ��һ֡
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
	// �������߳�,����������ȡ�����ݽ��в���
	// ʵ�ʿ����У�Ӧ���Ǵӷ���˽����ݽ��в���
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
