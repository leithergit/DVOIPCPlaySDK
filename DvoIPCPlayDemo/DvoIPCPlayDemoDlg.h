
// DvoIPCPlayDemoDlg.h : ͷ�ļ�
//

#pragma once

#include "VideoFrame.h"
#include "AutoLock.h"
#include <memory>
#include "GlliteryStatic.h"

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
	int nPlayerCount;
	DVO_PLAYHANDLE	hPlayer[16];
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

	CRITICAL_SECTION csRecFile;
public:
	PlayerContext(USER_HANDLE hUserIn,
		REAL_HANDLE hStreamIn = -1,
		DVO_PLAYHANDLE hPlayerIn = nullptr,int nCount = 1)
	{
		ZeroMemory(this, sizeof(PlayerContext));
		pStreamInfo = new StreamInfo();
		hUserIn = hUserIn;
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
		if (hUser != -1)
		{
			DVO2_NET_Logout(hUser);
			hUser = -1;
		}
		if (pStreamInfo)
			delete pStreamInfo;
		
		DeleteCriticalSection(&csRecFile);
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
	bool SaveSetting();
	bool LoadSetting();
	static CFile *m_pVldReport;
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
				if (nDvoError = dvoplay_GetFrame(pPlayCtx->hPlayer, &pFrameBuffer, nFrameSize) != DVO_Succeed)
				{
					TraceMsgA("%s dvoplay_GetFrame Failed:%d.\n", __FUNCTION__, nDvoError);
					Sleep(10);
					continue;
				}
				nFrames++;
				TraceMsgA("%s nFrames = %d FrameSize = %d.\n", __FUNCTION__, nFrames,nFrameSize);
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
					Sleep(10);
					continue;
				}
			}
			
			nLoopCount++;
			if (nDvoError = dvoplay_InputStream(pPlayCtx->hPlayerStream, pStream->pStream, pStream->nStreamSize) != DVO_Succeed)
			{
				TraceMsgA("%s dvoplay_InputStream Failed:%d.\n", __FUNCTION__, nDvoError);
				bInputList = false;
				Sleep(10);
				continue;
			}	
			bInputList = true;
			Sleep(10);
		}
		return 0;
	}
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonTracecache();
};
