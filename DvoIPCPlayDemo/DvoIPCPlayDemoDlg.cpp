
// DvoIPCPlayDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DvoIPCPlayDemo.h"
#include "DvoIPCPlayDemoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


enum _SubItem
{
	Item_VCodecType,
	Item_VideoWidth,
	Item_VideoHeight,
	Item_ACodecType,
	Item_StreamRate,
	Item_FrameRate,
	Item_FrameCache,
	Item_TotalStream,
	Item_ConnectTime,
	Item_RecFile,
	Item_RecTime,
	Item_FileLength,
};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDvoIPCPlayDemoDlg 对话框


CDvoIPCPlayDemoDlg::CDvoIPCPlayDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDvoIPCPlayDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDvoIPCPlayDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDvoIPCPlayDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CDvoIPCPlayDemoDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CDvoIPCPlayDemoDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_PLAYSTREAM, &CDvoIPCPlayDemoDlg::OnBnClickedButtonPlaystream)
	ON_BN_CLICKED(IDC_BUTTON_RECORD, &CDvoIPCPlayDemoDlg::OnBnClickedButtonRecord)
	ON_BN_CLICKED(IDC_BUTTON_PLAYFILE, &CDvoIPCPlayDemoDlg::OnBnClickedButtonPlayfile)	
	
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_STREAMINFO, &CDvoIPCPlayDemoDlg::OnNMCustomdrawListStreaminfo)
	ON_BN_CLICKED(IDC_CHECK_DISABLEAUDIO, &CDvoIPCPlayDemoDlg::OnBnClickedCheckEnableaudio)
	ON_BN_CLICKED(IDC_CHECK_FITWINDOW, &CDvoIPCPlayDemoDlg::OnBnClickedCheckFitwindow)	
	ON_WM_HSCROLL()
	ON_MESSAGE(WM_UPDATE_STREAMINFO, &CDvoIPCPlayDemoDlg::OnUpdateStreamInfo)
	ON_MESSAGE(WM_UPDATE_PLAYINFO, &CDvoIPCPlayDemoDlg::OnUpdatePlayInfo)
	ON_BN_CLICKED(IDC_BUTTON_SNAPSHOT, &CDvoIPCPlayDemoDlg::OnBnClickedButtonSnapshot)
	ON_CBN_SELCHANGE(IDC_COMBO_PLAYSPEED, &CDvoIPCPlayDemoDlg::OnCbnSelchangeComboPlayspeed)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CDvoIPCPlayDemoDlg::OnBnClickedButtonPause)
END_MESSAGE_MAP()


// CDvoIPCPlayDemoDlg 消息处理程序

BOOL CDvoIPCPlayDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	m_wndStatus.SubclassDlgItem(IDC_STATIC_STATUS, this);

	SendDlgItemMessage(IDC_COMBO_PICTYPE, CB_SETCURSEL, 1, 0);		// 默认使用JPG截图
	SendDlgItemMessage(IDC_COMBO_PLAYSPEED, CB_SETCURSEL, 8, 0);	// 默认播放速度为1X
	//m_wndBrowseCtrl.SubclassDlgItem(IDC_MFCEDITBROWSE, this);
	m_wndStreamInfo.SubclassDlgItem(IDC_LIST_STREAMINFO, this);
	m_wndStreamInfo.SetExtendedStyle(m_wndStreamInfo.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	int nCol = 0;

	m_wndStreamInfo.InsertColumn(nCol++, _T("项目"), LVCFMT_LEFT, 60);
	m_wndStreamInfo.InsertColumn(nCol++, _T("内容"), LVCFMT_LEFT, 120);
	int nItem = 0;
	m_wndStreamInfo.InsertItem(nItem++, _T("视频编码"));
	m_wndStreamInfo.InsertItem(nItem++, _T("视频宽度"));
	m_wndStreamInfo.InsertItem(nItem++, _T("视频高度"));
	m_wndStreamInfo.InsertItem(nItem++, _T("音频编码"));
	m_wndStreamInfo.InsertItem(nItem++, _T("码      率"));
	m_wndStreamInfo.InsertItem(nItem++, _T("帧      率"));
	m_wndStreamInfo.InsertItem(nItem++, _T("缓      存"));
	m_wndStreamInfo.InsertItem(nItem++, _T("码流总长"));
	m_wndStreamInfo.InsertItem(nItem++, _T("连接时长"));
	m_wndStreamInfo.InsertItem(nItem++, _T("录像文件"));
	m_wndStreamInfo.InsertItem(nItem++, _T("录像长度"));
	m_wndStreamInfo.InsertItem(nItem++, _T("录像时长"));

	SendDlgItemMessage(IDC_COMBO_STREAM, CB_SETCURSEL, 0, 0);
	SetDlgItemText(IDC_EDIT_ACCOUNT, _T("admin"));
	SetDlgItemText(IDC_EDIT_PASSWORD, _T("admin"));
	LoadSetting();
	CRect rtClient;
	GetDlgItemRect(IDC_STATIC_FRAME, rtClient);
	m_pVideoWndFrame = new CVideoFrame;
	m_pVideoWndFrame->Create(1024, rtClient, 1, 1, this);
	BOOL bRedraw = FALSE;

	UINT nSliderIDArray[] = {
		IDC_SLIDER_SATURATION,
		IDC_SLIDER_BRIGHTNESS,
		IDC_SLIDER_CONTRAST,
		IDC_SLIDER_CHROMA,
		IDC_SLIDER_PICSCALE,
		IDC_SLIDER_VOLUME,
		IDC_SLIDER_PLAYER };
	// 设置滑动块的取舍范围
	for (int i = 0; i < sizeof(nSliderIDArray) / sizeof(UINT); i++)
	{
		EnableDlgItem(nSliderIDArray[i], false);
		SendDlgItemMessage(nSliderIDArray[i],TBM_SETRANGEMIN, bRedraw, 0);
		SendDlgItemMessage(nSliderIDArray[i], TBM_SETRANGEMAX, bRedraw, 100);
	}

	SendDlgItemMessage(IDC_SLIDER_VOLUME, TBM_SETPOS, TRUE, 80);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

bool CDvoIPCPlayDemoDlg::LoadSetting()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	GetAppPath(szPath, MAX_PATH);
	_tcscat_s(szPath, MAX_PATH, _T("\\CameraSetting.xml"));
	if (!PathFileExists(szPath))
	{
		return false;
	}
	CMarkup xml;
	if (!xml.Load(szPath))
		return false;
	CString strCursel;
	CString strStreamType;
	CString strStream;
	HWND hCombox = GetDlgItem(IDC_IPADDRESS)->m_hWnd;

	if (xml.FindElem(_T("Configuration")))
	{
		if (xml.FindChildElem(_T("CurSel")))
		{
			xml.IntoElem();
			strCursel = xml.GetAttrib(_T("IP"));
			strStream = xml.GetAttrib(_T("Stream"));
			strStreamType = xml.GetAttrib(_T("StreamType"));
			xml.OutOfElem();
		}
		else
			return false;
		if (xml.FindChildElem(_T("CameraList")))
		{
			xml.IntoElem();

			while (xml.FindChildElem(_T("Camera")))
			{
				xml.IntoElem();
				ComboBox_AddString(hCombox, xml.GetAttrib(_T("IP")));
				xml.OutOfElem();
			}
			xml.OutOfElem();
		}
		else
			return false;
	}
	int nIndex = ComboBox_FindString(hCombox, 0, strCursel);
	if (nIndex == CB_ERR)
		ComboBox_SetCurSel(hCombox, 0);
	else
		ComboBox_SetCurSel(hCombox, nIndex);

	HWND hComboStrem = ::GetDlgItem(m_hWnd, IDC_COMBO_STREAM);
	nIndex = ComboBox_FindString(hComboStrem, 0, (LPCTSTR)strStream);
	ComboBox_SetCurSel(hComboStrem, nIndex);
	return true;
}

bool CDvoIPCPlayDemoDlg::SaveSetting()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	GetAppPath(szPath, MAX_PATH);
	_tcscat_s(szPath, MAX_PATH, _T("\\CameraSetting.xml"));

	HWND hComboBox = GetDlgItem(IDC_IPADDRESS)->m_hWnd;
	TCHAR szStreamType[16] = { 0 };
	TCHAR szText[32] = { 0 };
	TCHAR szStream[16] = { 0 };
	GetDlgItemText(IDC_IPADDRESS, szText, 32);

	GetDlgItemText(IDC_COMBO_STREAM, szStream, 16);
	CMarkup xml;
	TCHAR *szDoc = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.SetDoc(szDoc);

	xml.AddElem(_T("Configuration"));
	xml.AddChildElem(_T("CurSel"));
	xml.IntoElem();
	xml.AddAttrib(_T("IP"), szText);
	xml.AddAttrib(_T("Stream"), szStream);
	xml.OutOfElem();

	xml.IntoElem();	  // Configuration
	xml.AddElem(_T("CameraList"));
	int nCount = ComboBox_GetCount(hComboBox);
	for (int i = 0; i < nCount; i++)
	{
		xml.AddChildElem(_T("Camera"));
		ComboBox_GetLBText(hComboBox, i, szText);
		xml.IntoElem();
		xml.AddAttrib(_T("IP"), szText);
		xml.OutOfElem();
	}
	xml.OutOfElem(); // Configuration

	xml.Save(szPath);
	return true;

}


void CDvoIPCPlayDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDvoIPCPlayDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDvoIPCPlayDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDvoIPCPlayDemoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (GetDlgItem(IDC_STATIC_FRAME)->GetSafeHwnd())
	{
// 		CRect rtList;
// 		GetDlgItemRect(IDC_LIST_CONNECTION, rtList);
// 
// 		rtList.left = 2;
// 		rtList.right = cx - 2;
// 		MoveDlgItem(IDC_LIST_CONNECTION, rtList);
// 
// 		CRect rtFrame;
// 		rtFrame.top = rtList.bottom + 3;
// 		rtFrame.left = 2;
// 		rtFrame.right = cx - 2;
// 		rtFrame.bottom = cy - 2;
// 		m_pVideoWndFrame->MoveWindow(rtFrame);
// 		Invalidate();
	}
}


void CDvoIPCPlayDemoDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	m_pPlayContext.reset();
	if (m_pVideoWndFrame)
	{
		m_pVideoWndFrame->DestroyWindow();
		delete m_pVideoWndFrame;
	}
}


void CDvoIPCPlayDemoDlg::OnBnClickedButtonConnect()
{
	CHAR szAccount[32] = { 0 };
	CHAR szPassowd[32] = { 0 };
	CHAR szIPAddress[32] = { 0 };
// 	TCHAR szAccountW[32] = { 0 };
// 	TCHAR szPassowdW[32] = { 0 };
// 	TCHAR szIPAddressW[32] = { 0 };

	GetDlgItemText(IDC_EDIT_ACCOUNT, szAccount, 32);
	GetDlgItemText(IDC_EDIT_ACCOUNT, szPassowd, 32);
	GetDlgItemText(IDC_IPADDRESS, szIPAddress, 32);
	int nStream = SendDlgItemMessage(IDC_COMBO_STREAM, CB_GETCURSEL);
	int nError = 0;
	app_net_tcp_sys_logo_info_t LoginInfo;
	USER_HANDLE hUser = DVO2_NET_Login(szIPAddress, 6001, szAccount, szPassowd, &LoginInfo, &nError, 5000);

	if (hUser != -1)
	{
		if (ComboBox_FindString(::GetDlgItem(m_hWnd, IDC_IPADDRESS), 0, szIPAddress) == CB_ERR)
			ComboBox_AddString(::GetDlgItem(m_hWnd, IDC_IPADDRESS), szIPAddress);

		m_pPlayContext = make_shared<PlayerContext>(hUser);		
		m_pPlayContext->pThis = this;
		m_pPlayContext->hUser = hUser;
		_tcscpy_s(m_pPlayContext->szIpAddress, 32, szIPAddress);
		SaveSetting();

		EnableDlgItem(IDC_BUTTON_DISCONNECT, true);
		EnableDlgItem(IDC_BUTTON_CONNECT, false);
		EnableDlgItem(IDC_EDIT_ACCOUNT, false);
		EnableDlgItem(IDC_EDIT_PASSWORD, false);
		EnableDlgItem(IDC_BUTTON_PLAYSTREAM, true);		
		EnableDlgItem(IDC_BUTTON_RECORD, false);
		EnableDlgItem(IDC_IPADDRESS, false);
		EnableDlgItem(IDC_BUTTON_PLAYFILE, false);
		EnableDlgItem(IDC_BUTTON_PAUSE, false);
		EnableDlgItem(IDC_COMBO_PLAYSPEED, false);
		EnableDlgItem(IDC_SLIDER_PLAYER, false);
	}
}

void CDvoIPCPlayDemoDlg::OnBnClickedButtonDisconnect()
{
	if (m_pPlayContext)
	{
		HWND hWnd = m_pPlayContext->hWndView;
		m_pPlayContext.reset();
		SetDlgItemText(IDC_BUTTON_PLAYSTREAM, _T("播放码流"));
		EnableDlgItem(IDC_BUTTON_DISCONNECT, false);
		EnableDlgItem(IDC_BUTTON_PLAYSTREAM, false);
		EnableDlgItem(IDC_BUTTON_RECORD, false);
		EnableDlgItem(IDC_BUTTON_CONNECT, true);
		EnableDlgItem(IDC_EDIT_ACCOUNT, true);
		EnableDlgItem(IDC_EDIT_PASSWORD, true);
		EnableDlgItem(IDC_IPADDRESS, true);
		EnableDlgItem(IDC_COMBO_STREAM, true);
		EnableDlgItem(IDC_BUTTON_PLAYFILE, true);
		EnableDlgItem(IDC_BUTTON_PLAYFILE, true);
		EnableDlgItem(IDC_BUTTON_PAUSE, true);
		EnableDlgItem(IDC_COMBO_PLAYSPEED, true);
		EnableDlgItem(IDC_SLIDER_PLAYER, true);
	}
}


void CDvoIPCPlayDemoDlg::OnBnClickedButtonPlaystream()
{
	if (m_pPlayContext)
	{
		bool bEnableWnd = false;
		CWaitCursor Wait;
		if (m_pPlayContext->hStream == -1)
		{
			int nStream = SendDlgItemMessage(IDC_COMBO_STREAM, CB_GETCURSEL);
			int nError = 0;
			REAL_HANDLE hStreamHandle = DVO2_NET_StartRealPlay(m_pPlayContext->hUser,
				0,
				nStream,
				DVO_TCP,
				0,
				NULL,
				(fnDVOCallback_RealAVData_T)StreamCallBack,
				(void *)m_pPlayContext.get(),
				&nError);
			if (hStreamHandle == -1)
			{
				m_wndStatus.SetWindowText(_T("连接码流失败"));
				m_wndStatus.SetAlarmGllitery();
				return;
			}
			m_pPlayContext->hStream = hStreamHandle;
			m_pPlayContext->pThis = this;
			if (!m_pPlayContext->hPlayer)
			{
				int nFreePanel = 0;
				m_pPlayContext->hWndView = m_pVideoWndFrame->GetPanelWnd(nFreePanel);
				m_pVideoWndFrame->SetPanelParam(nFreePanel, m_pPlayContext.get());
				m_pPlayContext->hPlayer = dvoplay_OpenStream(m_pPlayContext->hWndView, nullptr, 0);
				if (!m_pPlayContext->hPlayer)
				{
					m_wndStatus.SetWindowText(_T("打开流播放句柄失败"));
					m_wndStatus.SetAlarmGllitery();
					return;
				}
				bool bEnableAudio = (bool)IsDlgButtonChecked(IDC_CHECK_DISABLEAUDIO);
				bool bFitWindow = (bool)IsDlgButtonChecked(IDC_CHECK_FITWINDOW);
				dvoplay_Start(m_pPlayContext->hPlayer, !bEnableAudio, bFitWindow);
				int nVolume = SendDlgItemMessage(IDC_SLIDER_VOLUME, TBM_GETPOS);
				dvoplay_SetVolume(m_pPlayContext->hPlayer, nVolume);
				SetDlgItemText(IDC_BUTTON_PLAYSTREAM, _T("停止播放"));
			
				EnableDlgItems(m_hWnd, true, 6,
					IDC_SLIDER_SATURATION,
					IDC_SLIDER_BRIGHTNESS,
					IDC_SLIDER_CONTRAST,
					IDC_SLIDER_CHROMA,
					IDC_SLIDER_PICSCALE,
					IDC_SLIDER_VOLUME);
				EnableDlgItem(IDC_SLIDER_PLAYER, false);
				EnableDlgItem(IDC_COMBO_STREAM, false);
			}
			
		}
		else
		{
			DVO2_NET_StopRealPlay(m_pPlayContext->hStream);
			m_pPlayContext->hStream = -1;		
			
			if (m_pPlayContext->hPlayer)
			{
				dvoplay_Stop(m_pPlayContext->hPlayer);
				//dvoplay_Refresh(m_pPlayContext->hPlayer);
				dvoplay_Close(m_pPlayContext->hPlayer);

				m_pPlayContext->hPlayer = nullptr;
			}
			SetDlgItemText(IDC_BUTTON_PLAYSTREAM, _T("播放码流"));
			EnableDlgItems(m_hWnd, false, 6,
				IDC_SLIDER_SATURATION,
				IDC_SLIDER_BRIGHTNESS,
				IDC_SLIDER_CONTRAST,
				IDC_SLIDER_CHROMA,
				IDC_SLIDER_PICSCALE,
				IDC_SLIDER_VOLUME);
			EnableDlgItem(IDC_SLIDER_PLAYER, false);					
			EnableDlgItem(IDC_BUTTON_PLAYFILE, true);
			EnableDlgItem(IDC_COMBO_STREAM, true);
		}
	}
}

static TCHAR *szCodecString[] =
{
	_T("未知"),
	_T("H.264"),
	_T("H.265"),
	_T("MJPEG"),
	_T("null"),
	_T("null"),
	_T("null"),
	_T("G711-A"),
	_T("G711-U"),
	_T("G726"),
	_T("AAC")
};

void CDvoIPCPlayDemoDlg::OnBnClickedButtonRecord()
{
	if (m_pPlayContext && m_pPlayContext->hStream)
	{
		int nStream = SendDlgItemMessage(IDC_COMBO_STREAM, CB_GETCURSEL);
		//DVO_CODEC nVideoCodec = CODEC_UNKNOWN;

		//dvoplay_GetCodec(m_pPlayContext->hPlayer, &nVideoCodec, nullptr);

		CWaitCursor Wait;
		if (!m_pPlayContext->pRecFile)
		{
			m_pPlayContext->bRecvIFrame = false;
			m_pPlayContext->nVideoFrameID = 0;
			m_pPlayContext->nAudioFrameID = 0;
			TCHAR szPath[MAX_PATH] = { 0 };
			TCHAR szFileDir[MAX_PATH] = { 0 };
			if (!PathFileExists(m_szRecordPath))
				GetAppPath(szFileDir, MAX_PATH);
			else
				_tcscpy_s(szFileDir, MAX_PATH, m_szRecordPath);

			CTime tNow = CTime::GetCurrentTime();
			_stprintf_s(m_pPlayContext->szRecFilePath,
				_T("%s\\DVO_%s_CH%d_%s.MP4"),
				szFileDir,
				m_pPlayContext->szIpAddress,
				nStream,
				tNow.Format(_T("%y%m%d%H%M%S")));
			m_pPlayContext->StartRecord();
			SetDlgItemText(IDC_BUTTON_RECORD, _T("停止录像"));
		}
		else
		{
			m_pPlayContext->StopRecord();
			TraceMsgA("%s LastVideoFrameID = %d\tLastAudioFrameID = %d.\n", __FUNCTION__, m_pPlayContext->nVideoFrameID, m_pPlayContext->nAudioFrameID);
			TraceMsgA("%s VideoFrameCount = %d\tAudioFrameCount = %d.\n", __FUNCTION__, m_pPlayContext->pStreamInfo->nVideoFrameCount, m_pPlayContext->pStreamInfo->nAudioFrameCount );
			SetDlgItemText(IDC_BUTTON_RECORD, _T("开始录像"));
		}
	}
}


void CDvoIPCPlayDemoDlg::OnBnClickedButtonPlayfile()
{
	bool bEnableWnd = true;
	if (!m_pPlayContext)
	{
		// sws_setColorspaceDetails()设置图像参数
		TCHAR szText[1024] = { 0 };		
		int nFreePanel = 0;		
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		TCHAR  szFilter[] = _T("录像视频文件 (*.mp4)|*.mp4|H.264录像文件(*.H264)|*.H264|H.265录像文件(*.H265)|*.H265|All Files (*.*)|*.*||");
		TCHAR szExportLog[MAX_PATH] = { 0 };
		CTime tNow = CTime::GetCurrentTime();		
		CFileDialog OpenDataBase(TRUE, _T("*.mp4"), _T(""), dwFlags, (LPCTSTR)szFilter);
		OpenDataBase.m_ofn.lpstrTitle = _T("请选择播放的文件");
		CString strFilePath;
		if (OpenDataBase.DoModal() == IDOK)
		{
			strFilePath = OpenDataBase.GetPathName();
			try
			{
				CFile fpMedia((LPCTSTR)strFilePath, CFile::modeRead);
				DVO_MEDIAINFO MediaHeader;
				if (fpMedia.Read(&MediaHeader, sizeof(DVO_MEDIAINFO)) < sizeof(DVO_MEDIAINFO) ||
					MediaHeader.nMediaTag != 0x44564F4D)	// 头标志 固定为   0x44564F4D 即字符串"MOVD"
				{
					m_wndStatus.SetWindowText(_T("指定的文件不是一个有效的DVO录像文件"));
					m_wndStatus.SetAlarmGllitery();					
					return;
				}
				SetDlgItemText(IDC_EDIT_FILEPATH, strFilePath);
				m_pPlayContext = make_shared<PlayerContext>(-1);
				m_pPlayContext->hWndView = m_pVideoWndFrame->GetPanelWnd(nFreePanel);
				m_pVideoWndFrame->SetPanelParam(nFreePanel, m_pPlayContext.get());
				m_pPlayContext->hPlayer = dvoplay_OpenFile(m_pPlayContext->hWndView, (CHAR *)(LPCTSTR)strFilePath,(FilePlayProc)PlayerCallBack,m_pPlayContext.get());
				
				if (!m_pPlayContext->hPlayer)
				{
					_stprintf_s(szText, 1024, _T("无法打开%s文件."), strFilePath);
					m_wndStatus.SetWindowText(szText);
					m_wndStatus.SetAlarmGllitery();
					m_pPlayContext.reset();
					return;
				}
				fpMedia.Close();
				bool bEnableAudio = (bool)IsDlgButtonChecked(IDC_CHECK_DISABLEAUDIO);
				bool bFitWindow = (bool)IsDlgButtonChecked(IDC_CHECK_FITWINDOW);
				
				if (dvoplay_Start(m_pPlayContext->hPlayer, !bEnableAudio, bFitWindow) != DVO_Succeed)
				{
					m_wndStatus.SetWindowText(_T("无法启动播放器"));
					m_wndStatus.SetAlarmGllitery();
					m_pPlayContext.reset();
					return;
				}
				PlayRate RateArray[] =
				{
					Rate_One32th,
					Rate_One24th,
					Rate_One20th,
					Rate_One16th,
					Rate_One10th,
					Rate_One08th,
					Rate_Quarter,
					Rate_Half,
					Rate_01X,
					Rate_02X,
					Rate_04X,
					Rate_08X,
					Rate_10X,
					Rate_16X,
					Rate_20X,
					Rate_24X,
					Rate_32X
				};
				int nCurSecl = SendDlgItemMessage(IDC_COMBO_PLAYSPEED, CB_GETCURSEL);
				if (nCurSecl <= 0 && nCurSecl > 16)
				{
					m_wndStatus.SetWindowText(_T("无效的播放速度,现以原始速度播放."));
					m_wndStatus.SetAlarmGllitery();
					nCurSecl = 8;
				}
				dvoplay_SetRate(m_pPlayContext->hPlayer, RateArray[nCurSecl]);
				m_pPlayContext->pThis = this;
				SetDlgItemText(IDC_BUTTON_PLAYFILE, _T("停止播放"));
				bEnableWnd = false;
			}
			catch (CFileException* e)
			{
				TCHAR szErrorMsg[255] = { 0 };
				e->GetErrorMessage(szErrorMsg, 255);
				m_wndStatus.SetWindowText(szErrorMsg);
				m_wndStatus.SetAlarmGllitery();
				return;
			}
		}
	}
	else if (m_pPlayContext->hPlayer)
	{
		m_pPlayContext.reset();
		SetDlgItemText(IDC_BUTTON_PLAYFILE, _T("播放文件"));
	}
	// 禁用所有其它码流播放相关按钮
	EnableDlgItem(IDC_BUTTON_DISCONNECT, bEnableWnd);
	EnableDlgItem(IDC_BUTTON_PLAYSTREAM, bEnableWnd);
	EnableDlgItem(IDC_BUTTON_RECORD, bEnableWnd);
	EnableDlgItem(IDC_BUTTON_CONNECT, bEnableWnd);
	EnableDlgItem(IDC_EDIT_ACCOUNT, bEnableWnd);
	EnableDlgItem(IDC_EDIT_PASSWORD, bEnableWnd);
	EnableDlgItem(IDC_IPADDRESS, bEnableWnd);
	EnableDlgItem(IDC_COMBO_STREAM, bEnableWnd);
	EnableDlgItems(m_hWnd, !bEnableWnd, 7,
					IDC_SLIDER_SATURATION,
					IDC_SLIDER_BRIGHTNESS,
					IDC_SLIDER_CONTRAST,
					IDC_SLIDER_CHROMA,
					IDC_SLIDER_PICSCALE,
					IDC_SLIDER_VOLUME, 
					IDC_SLIDER_PLAYER);
}

bool IsDVOVideoFrame(app_net_tcp_enc_stream_head_t *pStreamHeader)
{
	switch (pStreamHeader->frame_type)
	{
	case 0:
	case APP_NET_TCP_COM_DST_IDR_FRAME:
	case APP_NET_TCP_COM_DST_I_FRAME:	
	case APP_NET_TCP_COM_DST_P_FRAME:
	case APP_NET_TCP_COM_DST_B_FRAME:
		return true;
	default:
		return false;
	}
}

bool IsDVOAudioFrame(app_net_tcp_enc_stream_head_t *pStreamHeader)
{
	switch (pStreamHeader->frame_type)
	{
	case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A律编码帧
	case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U律编码帧
	case APP_NET_TCP_COM_DST_726:           // 726编码帧
	case APP_NET_TCP_COM_DST_AAC:           // AAC编码帧
		return true;
	default:
		return false;
	}
}

void CDvoIPCPlayDemoDlg::PlayerCallBack(DVO_PLAYHANDLE hPlayHandle, void *pUserPtr)
{
	PlayerContext *pContext = (PlayerContext *)pUserPtr;
	if (pContext->pThis)
	{
		CDvoIPCPlayDemoDlg *pDlg = (CDvoIPCPlayDemoDlg *)pContext->pThis;
		pDlg->PostMessage(WM_UPDATE_PLAYINFO,(WPARAM)hPlayHandle);
	}

}
void CDvoIPCPlayDemoDlg::StreamCallBack(IN USER_HANDLE  lUserID,
	IN REAL_HANDLE lStreamHandle,
	IN int         nErrorType,
	IN const char* pBuffer,
	IN int         nDataLen,
	IN void*       pUser)
{
	if (!pUser)
		return;
	PlayerContext *pContext = (PlayerContext *)pUser;
	app_net_tcp_enc_stream_head_t *pStreamHeader = (app_net_tcp_enc_stream_head_t *)pBuffer;
	int nSizeofHeader = sizeof(app_net_tcp_enc_stream_head_t);
	byte *pFrameData = (byte *)(pStreamHeader)+nSizeofHeader;
	int nFrameLength = nDataLen - sizeof(app_net_tcp_enc_stream_head_t);	
	time_t tSec = pStreamHeader->sec;
	time_t tFrame = tSec * 1000 * 1000 + pStreamHeader->usec;
	
	switch (pStreamHeader->frame_type)
	{
	case 0:
	case APP_NET_TCP_COM_DST_IDR_FRAME:
	case APP_NET_TCP_COM_DST_I_FRAME:
	{
		if (!pContext->bRecvIFrame)
		{
			pContext->nVideoFrameID = 0;
			pContext->nAudioFrameID = 0;
			pContext->bRecvIFrame = true;
		}
		else
			pContext->nVideoFrameID++;
		pContext->pStreamInfo->PushFrameInfo(nFrameLength);
		pContext->pStreamInfo->nVideoFrameCount++;
		pContext->pStreamInfo->nVideoBytes += nFrameLength;
		pContext->pStreamInfo->nFrameID = pStreamHeader->frame_num;
		pContext->pStreamInfo->tLastTime = (time_t)(GetExactTime() * 1000);

		int nFrameTimeSpan = (tFrame - pContext->tLastFrame) / 1000;
		pContext->tLastFrame = tFrame;
		if (nFrameTimeSpan)
			pContext->nFPS = 1000 / nFrameTimeSpan;
		break;
	}
	case APP_NET_TCP_COM_DST_P_FRAME:
	case APP_NET_TCP_COM_DST_B_FRAME:
	{
		// 这里只对视频帧计算帧ID
		if (pContext->bRecvIFrame)
			pContext->nVideoFrameID ++;	

		pContext->pStreamInfo->PushFrameInfo(nFrameLength);
		pContext->pStreamInfo->nVideoFrameCount++;
		pContext->pStreamInfo->nVideoBytes += nFrameLength;
		pContext->pStreamInfo->nFrameID = pStreamHeader->frame_num;
		pContext->pStreamInfo->tLastTime = (time_t)(GetExactTime() * 1000);

		int nFrameTimeSpan = (tFrame - pContext->tLastFrame)/1000;
		pContext->tLastFrame = tFrame;
		if (nFrameTimeSpan)
			pContext->nFPS = 1000 / nFrameTimeSpan;
		break;
	}
	case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A律编码帧
	case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U律编码帧
	case APP_NET_TCP_COM_DST_726:           // 726编码帧
	case APP_NET_TCP_COM_DST_AAC:           // AAC编码帧
		if (pContext->bRecvIFrame)		
			pContext->nAudioFrameID++;
		pContext->nAudioCodec = pStreamHeader->frame_type;
		pContext->pStreamInfo->nAudioFrameCount++;
		break;
	default:
		//TraceMsgA("%s Audio Frame Length = %d.\n", __FUNCTION__, nFrameLength);
		break;
	}

	if (pContext->hPlayer)
		dvoplay_InputIPCStream(pContext->hPlayer, pFrameData, pStreamHeader->frame_type, nFrameLength, pStreamHeader->frame_num, tFrame);

	// 写入录像数据
	if (pContext->pRecFile && pContext->bRecvIFrame)
	{
		byte szHeader[64] = { 0 };
		int nHeadersize = 64;
		if (!pContext->bWriteMediaHeader)
		{
			if (dvoplay_BuildMediaHeader(szHeader, &nHeadersize, (DVO_CODEC)pContext->nAudioCodec) == DVO_Succeed)
			{
				pContext->pRecFile->SeekToBegin();
				pContext->pRecFile->Write(szHeader, nHeadersize);
				pContext->pRecFile->SeekToEnd();
				pContext->bWriteMediaHeader = true;
			}
		}
		if (pContext->bWriteMediaHeader)
		{
			int nSizetoSave = nDataLen;
			if (IsDVOVideoFrame(pStreamHeader))
				if (dvoplay_BuildFrameHeader(szHeader, &nHeadersize, pContext->nVideoFrameID, (byte *)pBuffer, nSizetoSave) == DVO_Succeed)
				{
					pContext->pRecFile->Write(szHeader, nHeadersize);
					pContext->pRecFile->Write(&pBuffer[nDataLen - nSizetoSave], nSizetoSave);
				}
			if (IsDVOAudioFrame(pStreamHeader))
				if (dvoplay_BuildFrameHeader(szHeader, &nHeadersize, pContext->nAudioFrameID, (byte *)pBuffer, nSizetoSave) == DVO_Succeed)
				{
					pContext->pRecFile->Write(szHeader, nHeadersize);
					pContext->pRecFile->Write(&pBuffer[nDataLen - nSizetoSave], nSizetoSave);
				}
		}
	}
	if (pContext->pThis)
	{
		CDvoIPCPlayDemoDlg *pDlg = (CDvoIPCPlayDemoDlg *)pContext->pThis;
		pDlg->PostMessage(WM_UPDATE_STREAMINFO);
	}
}

LRESULT CDvoIPCPlayDemoDlg::OnUpdatePlayInfo(WPARAM w, LPARAM l)
{ 
	DVO_PLAYHANDLE  *hPlayer = (DVO_PLAYHANDLE *)w;
	if (hPlayer)
	{
		FilePlayInfo fpi;
		if (dvoplay_GetFilePlayInfo(hPlayer, &fpi) != DVO_Succeed)
			return 0;
		int nSlidePos = 0;
		if (fpi.nTotalFrames > 0)
			nSlidePos = (int)(100 * (double)fpi.nCurFrameID / fpi.nTotalFrames);
		
		SendDlgItemMessage(IDC_SLIDER_PLAYER, TBM_SETPOS, TRUE, nSlidePos);
		time_t T1 = fpi.tCurFrameTime / 1000;
		int nFloat = fpi.tCurFrameTime - T1 * 1000;
		int nHour = T1 / 3600;
		int nMinute = (T1 - nHour * 3600) / 60;
		int nSecond = T1 % 60;
		TCHAR szPlayText[64] = { 0 };
		_stprintf_s(szPlayText, 64, _T("%02d:%02d:%02d.%03d"), nHour, nMinute, nSecond, nFloat);
		SetDlgItemText(IDC_EDIT_PLAYTIME, szPlayText);
		_stprintf_s(szPlayText, 64, _T("%d"), fpi.nCurFrameID);
		SetDlgItemText(IDC_EDIT_PLAYFRAME, szPlayText);
			
		_stprintf_s(szPlayText, 64, _T("%d"), fpi.nCacheSize);
		SetDlgItemText(IDC_EDIT_PLAYCACHE, szPlayText);
		
		_stprintf_s(szPlayText, 64, _T("%d"), fpi.nPlayFPS);
		SetDlgItemText(IDC_EDIT_FPS, szPlayText);
	}
	return 0;
}

LRESULT CDvoIPCPlayDemoDlg::OnUpdateStreamInfo(WPARAM w, LPARAM l)
{
	if (!m_pPlayContext)
		return 0;
	TCHAR szText[128] = { 0 };

	DVO_CODEC nVideoCodec = CODEC_UNKNOWN;
	DVO_CODEC nAudioCodec = CODEC_UNKNOWN;
	FilePlayInfo fpi;
	dvoplay_GetFilePlayInfo(m_pPlayContext->hPlayer)
	if (dvoplay_GetCodec(m_pPlayContext->hPlayer, &nVideoCodec, &nAudioCodec) == DVO_Succeed)
	{
		if (nVideoCodec >= CODEC_UNKNOWN && nVideoCodec <= CODEC_AAC)
			m_wndStreamInfo.SetItemText(Item_VCodecType, 1, szCodecString[nVideoCodec + 1]);// 视频编码类型
		else
			m_wndStreamInfo.SetItemText(Item_VCodecType, 1, _T("N/A"));

		if (nAudioCodec >= CODEC_UNKNOWN && nAudioCodec <= CODEC_AAC)
			m_wndStreamInfo.SetItemText(Item_ACodecType, 1, szCodecString[nAudioCodec + 1]);// 音频编码类型
		else
			m_wndStreamInfo.SetItemText(Item_ACodecType, 1, _T("N/A"));
	}
	int nStreamRate = m_pPlayContext->pStreamInfo->GetVideoCodeRate();
	_stprintf_s(szText, 64, _T("%d Kbps"), nStreamRate);
	m_wndStreamInfo.SetItemText(Item_StreamRate, 1, szText);// 码率

	_stprintf_s(szText, 64, _T("%d fps"), m_pPlayContext->nFPS);
	m_wndStreamInfo.SetItemText(Item_FrameRate, 1, szText);// 帧率
	int nCache = 0;
	if (m_pPlayContext->hPlayer)
		dvoplay_GetCacheSize(m_pPlayContext->hPlayer, nCache);

	_stprintf_s(szText, 64, _T("%d"), nCache);
	m_wndStreamInfo.SetItemText(Item_FrameCache, 1, szText);	// 帧缓存

	_stprintf_s(szText, 64, _T("%d KB"), (m_pPlayContext->pStreamInfo->nVideoBytes + m_pPlayContext->pStreamInfo->nAudioBytes) / 1024);
	m_wndStreamInfo.SetItemText(Item_TotalStream, 1, szText);	// 收到总码流长度

	if (m_pPlayContext->pStreamInfo->tFirstTime > 0)
	{
		time_t nRecTime = time(0) * 1000 - m_pPlayContext->pStreamInfo->tFirstTime;
		CTimeSpan tSpan(nRecTime / 1000);
		_stprintf_s(szText, 64, _T("%02d:%02d:%02d"), tSpan.GetHours(), tSpan.GetMinutes(), tSpan.GetSeconds());
	}
	else
		_tcscpy_s(szText, 64, _T("00:00:00"));
	m_wndStreamInfo.SetItemText(Item_ConnectTime, 1, szText);		// 连接时长

	if (m_pPlayContext->pRecFile)
	{
		m_wndStreamInfo.SetItemText(Item_RecFile, 1, m_pPlayContext->szRecFilePath);// 文件名
		time_t nRecTime = time(0) - m_pPlayContext->tRecStartTime;
		CTimeSpan tSpan(nRecTime);
		_stprintf_s(szText, 64, _T("%02d:%02d:%02d"), tSpan.GetHours(), tSpan.GetMinutes(), tSpan.GetSeconds());
		m_wndStreamInfo.SetItemText(Item_RecTime, 1, szText);				// 录像时长

		_stprintf_s(szText, _T("%d KB"), m_pPlayContext->pRecFile->GetLength() / 1024);
		m_wndStreamInfo.SetItemText(Item_FileLength, 1, szText);
	}
	else
	{
		_tcscpy_s(szText, 128, _T("暂未录像"));
		m_wndStreamInfo.SetItemText(Item_RecFile, 1, szText);				// 文件名
		m_wndStreamInfo.SetItemText(Item_RecTime, 1, _T("00:00:00"));		// 录像时长
		m_wndStreamInfo.SetItemText(Item_FileLength, 1, szText);
	}

	return 0;
}

void CDvoIPCPlayDemoDlg::OnNMClickListConnection(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
}


void CDvoIPCPlayDemoDlg::OnNMCustomdrawListStreaminfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW *pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	*pResult = CDRF_DODEFAULT;
	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
	{
		int    nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		if (0 == pLVCD->iSubItem)
		{
			pLVCD->clrTextBk = RGB(224, 224, 224);
		}
		else
			pLVCD->clrTextBk = RGB(250, 250, 250);

		*pResult = CDRF_DODEFAULT;
	}
}


void CDvoIPCPlayDemoDlg::OnBnClickedCheckEnableaudio()
{
	if (m_pPlayContext && m_pPlayContext->hPlayer)
	{
		bool bEnable = (bool)IsDlgButtonChecked(IDC_CHECK_DISABLEAUDIO);
		if (dvoplay_EnableAudio(m_pPlayContext->hPlayer, !bEnable) != DVO_Succeed)
		{
			m_wndStatus.SetWindowText(_T("开关音频失败."));
			m_wndStatus.SetAlarmGllitery();
		}
	}
}


void CDvoIPCPlayDemoDlg::OnBnClickedCheckFitwindow()
{
	if (m_pPlayContext && m_pPlayContext->hPlayer)
	{
		bool bFitWindow = (bool)IsDlgButtonChecked(IDC_CHECK_FITWINDOW);
		if (dvoplay_FitWindow(m_pPlayContext->hPlayer, bFitWindow) != DVO_Succeed)
		{
			m_wndStatus.SetWindowText(_T("调整视频显示方式失败."));
			m_wndStatus.SetAlarmGllitery();
			return;
		}
		::InvalidateRect(m_pPlayContext->hWndView, nullptr, true);
	}
}

#define _Var(P)	#P

void CDvoIPCPlayDemoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (!m_pPlayContext || !m_pPlayContext->hPlayer)
	{
		CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}
	
	UINT nSliderID = pScrollBar->GetDlgCtrlID();	
	nPos = SendDlgItemMessage(nSliderID, TBM_GETPOS, 0, 0l);
	switch (nSliderID)
	{
	case IDC_SLIDER_SATURATION:
		TraceMsgA("SliderID:%s\tPos = %d.\n", _Var(IDC_SLIDER_SATURATION),nPos);
		break;
	case IDC_SLIDER_BRIGHTNESS:
		TraceMsgA("SliderID:%s\tPos = %d.\n", _Var(IDC_SLIDER_BRIGHTNESS), nPos);
		break;
	case IDC_SLIDER_CONTRAST:
		TraceMsgA("SliderID:%s\tPos = %d.\n", _Var(IDC_SLIDER_CONTRAST), nPos);
		break;
	case IDC_SLIDER_CHROMA:
		TraceMsgA("SliderID:%s\tPos = %d.\n", _Var(IDC_SLIDER_CHROMA), nPos);
		break;
	case IDC_SLIDER_PICSCALE:
		TraceMsgA("SliderID:%s\tPos = %d.\n", _Var(IDC_SLIDER_PICSCALE), nPos);
		break;
	case IDC_SLIDER_VOLUME:
	{
		dvoplay_SetVolume(m_pPlayContext->hPlayer, nPos);
	}
		break;
	case IDC_SLIDER_PLAYER:
	{
		int nTotalFrames = 0;
		if (dvoplay_GetFrames(m_pPlayContext->hPlayer, nTotalFrames) == DVO_Succeed)
		{
			int nSeekFrame = nTotalFrames*nPos / 100;
			dvoplay_SeekFrame(m_pPlayContext->hPlayer, nSeekFrame);
		}
	}
		break;
	default:
		break;
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDvoIPCPlayDemoDlg::OnBnClickedButtonSnapshot()
{
	if (m_pPlayContext && m_pPlayContext->hPlayer)
	{
		CWaitCursor Wait;
		TCHAR szPath[1024];
		GetAppPath(szPath, 1024);
		_tcscat_s(szPath, 1024, _T("\\ScreenSave"));
		if (!PathFileExists(szPath))
		{
			if (!CreateDirectory(szPath,nullptr))
			{
				m_wndStatus.SetWindowText(_T("无法创建保存截图文件的目录,请确主是否有足够的权限."));
				m_wndStatus.SetAlarmGllitery();
				return;
			}
		}
		TCHAR szFileName[64] = { 0 };
		SYSTEMTIME systime;
		GetLocalTime(&systime);

		TCHAR *szFileExt[] = { _T("bmp"), _T("jpg"), _T("tga"), _T("png") };
		int nPicType = SendDlgItemMessage(IDC_COMBO_PICTYPE, CB_GETCURSEL);
		if (nPicType > 3 || nPicType < 0)
		{
			m_wndStatus.SetWindowText(_T("不支持的图片格式,请选择正确的图片格式."));
			m_wndStatus.SetAlarmGllitery();
			return;
		}
		// 生成文件名
		_stprintf_s(szFileName, 64,
			_T("\\SnapShot_%04d%02d%02d_%02d%02d%02d_%03d.%s"),
			systime.wYear, systime.wMonth, systime.wDay,
			systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds,
			szFileExt[nPicType]);
		_tcscat_s(szPath, 1024, szFileName);
		if (dvoplay_SnapShot(m_pPlayContext->hPlayer, szPath, (SNAPSHOT_FORMAT)nPicType) == DVO_Succeed)
		{
			TCHAR szText[1024];
			_stprintf_s(szText, 1024, _T("已经生成截图:%s."), szPath);
			m_wndStatus.SetWindowText(szText);
			m_wndStatus.SetOkGllitery();
		}
		else
		{
			m_wndStatus.SetWindowText(_T("播放器尚未启动,无法截图."));
			m_wndStatus.SetAlarmGllitery();
		}
	}
	else
	{
		m_wndStatus.SetWindowText(_T("播放器尚未启动."));
		m_wndStatus.SetAlarmGllitery();
	}
}


void CDvoIPCPlayDemoDlg::OnCbnSelchangeComboPlayspeed()
{
	if (m_pPlayContext && m_pPlayContext->hPlayer)
	{
		PlayRate RateArray[] = 
		{
			Rate_One32th,
			Rate_One24th,
			Rate_One20th,
			Rate_One16th,
			Rate_One10th,
			Rate_One08th,
			Rate_Quarter,
			Rate_Half,
			Rate_01X ,
			Rate_02X ,
			Rate_04X ,
			Rate_08X ,
			Rate_10X ,
			Rate_16X ,
			Rate_20X ,
			Rate_24X ,
			Rate_32X 
		};
		int nCurSecl = SendDlgItemMessage(IDC_COMBO_PLAYSPEED, CB_GETCURSEL);
		if (nCurSecl <= 0 && nCurSecl > 16)
		{
			m_wndStatus.SetWindowText(_T("无效的播放速度."));
			m_wndStatus.SetAlarmGllitery();
			return;
		}
		dvoplay_SetRate(m_pPlayContext->hPlayer, RateArray[nCurSecl]);
	}
	else
	{
		m_wndStatus.SetWindowText(_T("播放器尚未启动."));
		m_wndStatus.SetAlarmGllitery();
	}
}


void CDvoIPCPlayDemoDlg::OnBnClickedButtonPause()
{
	if (m_pPlayContext && m_pPlayContext->hPlayer)
	{
		dvoplay_Pause(m_pPlayContext->hPlayer);
	}
	else
	{
		m_wndStatus.SetWindowText(_T("播放器尚未启动."));
		m_wndStatus.SetAlarmGllitery();
	}
}


