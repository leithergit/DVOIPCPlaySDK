
// DVOIPCSDKDemoDlg.h : 头文件
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
#define _FrameInterval	3	// 视频窗口边框的间隔

// CDVOIPCSDKDemoDlg 对话框
class CDVOIPCSDKDemoDlg : public CDialog
{
// 构造
public:
	CDVOIPCSDKDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DVOIPCSDKDEMO_DIALOG };

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
	Decoder_Manufacturer	m_nManufacturer;
	int			m_nMonitorCount;		//  当前已经连接显示器的数量
	//vector<boost::shared_ptr<CDialog>>m_vFullScreenWnd;		// 全屏窗口指针
	UINT		m_nVideoWndID[_MAX_VIDEOWND];
	TCHAR	m_szRecordPath[MAX_PATH];
	
	WORD m_nHotkeyID;

	// 矩形重绘变量
	CPen	*m_pSelectedPen,*m_pUnSelectedPen;
	CPen	*m_pFramePen[_MAX_VIDEOWND];
	int		m_nLastSelected;		// 上一次选择的窗口，亦即时当前选择窗口
	int		m_nDeaultVideoWnd;		// 默认的窗口ID,用户点击窗口后，与m_nLastSelected相同，点击播放按钮后，递增

	int		m_nCurSelect;
	CVideoPlayer *m_pCurSeclectDecoder;
	void	*m_pViewWndState[_MAX_VIDEOWND];		// 监视窗口状态,NULL为未使用,否则已经使用

	list<std::tr1::shared_ptr<CVideoPlayer>>m_listDecode;
	CListCtrl	m_wndConnectionList;
	int			m_nListWidth;	// List控件的宽度
	int			m_nListTop;		// List宽度的Top坐标
	
	static void VideoFitWindow(HWND hVideoWnd,int nVideoWidth,int nVideoHeight,int &nImageWidth,int &nImageHeight);
	// 设置指定序号的窗口为选定状态,并撤销前次的选择的窗口的选择状态
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
	// 控件台窗口事件处理入口
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
