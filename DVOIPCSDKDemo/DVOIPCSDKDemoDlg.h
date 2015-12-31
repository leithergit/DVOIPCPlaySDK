
// DVOIPCSDKDemoDlg.h : ͷ�ļ�
//

#pragma once


#include "stdafx.h"
#include "VideoDecode.h"
struct FullScreenCanvas
{
	CDialog *pDialog;
	
};

enum _SubItem
{
	SubItem_No = 0,
	SubItem_CameraIP,
	SubItem_CodecType,
	SubItem_StreamNo,
	SubItem_StreamRate,
	SubItem_FrameRate,
	SubItem_FrameCache,
	SubItem_TotalStream,
	SubItem_ConnectTime,
	SubItem_RecTime,
	SubItem_FileLength,
	SubItem_DecoderManufacturer
};

#define _MAX_VIDEOWND	4
#define _FrameInterval	3	// ��Ƶ���ڱ߿�ļ��

// CDVOIPCSDKDemoDlg �Ի���
class CDVOIPCSDKDemoDlg : public CDialog
{
// ����
public:
	CDVOIPCSDKDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DVOIPCSDKDEMO_DIALOG };

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
	Decoder_Manufacturer	m_nManufacturer;
	int			m_nMonitorCount;		//  ��ǰ�Ѿ�������ʾ��������
	//vector<boost::shared_ptr<CDialog>>m_vFullScreenWnd;		// ȫ������ָ��
	UINT		m_nVideoWndID[_MAX_VIDEOWND];
	TCHAR	m_szRecordPath[MAX_PATH];
	
	WORD m_nHotkeyID;

	// �����ػ����
	CPen	*m_pSelectedPen,*m_pUnSelectedPen;
	CPen	*m_pFramePen[_MAX_VIDEOWND];
	int		m_nLastSelected;		// ��һ��ѡ��Ĵ��ڣ��༴ʱ��ǰѡ�񴰿�
	int		m_nDeaultVideoWnd;		// Ĭ�ϵĴ���ID,�û�������ں���m_nLastSelected��ͬ��������Ű�ť�󣬵���

	int		m_nCurSelect;
	CVideoPlayer *m_pCurSeclectDecoder;
	void	*m_pViewWndState[_MAX_VIDEOWND];		// ���Ӵ���״̬,NULLΪδʹ��,�����Ѿ�ʹ��

	list<std::tr1::shared_ptr<CVideoPlayer>>m_listDecode;
	CListCtrl	m_wndConnectionList;
	int			m_nListWidth;	// List�ؼ��Ŀ��
	int			m_nListTop;		// List��ȵ�Top����
	
	static void VideoFitWindow(HWND hVideoWnd,int nVideoWidth,int nVideoHeight,int &nImageWidth,int &nImageHeight);
	// ����ָ����ŵĴ���Ϊѡ��״̬,������ǰ�ε�ѡ��Ĵ��ڵ�ѡ��״̬
	void SelectVideoWnd(int nWndNewSelected,int nWndOldSelected);

	afx_msg void OnBnClickedButtonPlaystream();
	afx_msg void OnDestroy();
//	afx_msg void OnSize(UINT nType, int cx, int cy);
	bool SaveSetting();
	bool LoadSetting();
	LRESULT OnUpdateDecoderInfo(WPARAM w,LPARAM l);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);	

	afx_msg void OnNMClickListConnection(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonRecord();
	afx_msg void OnMenuRecord();
	afx_msg void OnFileStoprecord();
	afx_msg void OnFileOutput();
	afx_msg void OnFileLoginRec();
	// �ؼ�̨�����¼��������
	static BOOL WINAPI ConsoleHandler(DWORD dwMsgType);
	afx_msg void OnDecoderSel(UINT nID);
	afx_msg void OnAdjustImageQulity(UINT nID)
	{
		if (!m_pCurSeclectDecoder)
			return ;
		switch(nID)
		{
		default:
		case ID_IMAGEQUILTY_BANLANCE:
			break;
		case ID_IMAGEQUILTY_HIGHEST:
			break;
		case ID_IMAGEQUILTY_HIGH:
			break;
		
			break;
		case ID_IMAGEQUILTY_HIGHSPEED:
			break;
		case ID_IMAGEQUILTY_HIGHESTSPEED:
				break;
		}
	}
	CStatic m_wndVideo[4];
	afx_msg void OnDecoderFfmpeg();
	afx_msg void OnDecoderHisilicon();
	void OnDecoderCompare();
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	
	CSliderCtrl		m_wndSliderCtrl;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	LRESULT OnInitDxSurface(WPARAM w,LPARAM l)	;
	LRESULT OnRenderFrame(WPARAM w,LPARAM l);
	afx_msg void OnFileSetrecordpath();
};
