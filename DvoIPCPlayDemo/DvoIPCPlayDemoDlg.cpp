
// DvoIPCPlayDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DvoIPCPlayDemo.h"
#include "DvoIPCPlayDemoDlg.h"
#include "afxdialogex.h"
#include "ipcMsgHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CVca* g_pVca = NULL;
enum _SubItem
{
	Item_VideoInfo,
	Item_ACodecType,	
	Item_StreamRate,
	Item_FrameRate,
	Item_VFrameCache,
	Item_AFrameCache,
	Item_TotalStream,
	Item_ConnectTime,
	Item_PlayedTime,
	Item_RecFile,
	Item_RecTime,
	Item_FileLength,
};


#define  ID_PLAYEVENT 1024
#define _PlayInterval 250
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

#define _Row	1
#define _Col	1


CDvoIPCPlayDemoDlg::CDvoIPCPlayDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDvoIPCPlayDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bPuased = false;
	m_bThreadStream = false;
	m_hThreadSendStream = NULL;
	m_hThreadPlayStream = NULL;
	m_nRow = 1;
	m_nCol = 1;
	m_nHotkeyID = 0;

	m_hFullScreen = NULL;
	m_nOriMonitorIndex = 0;	
	m_hIOCP = NULL;
	m_bClickPlayerSlide = false;
// 	m_pDDraw.reset();;
// 	m_pYUVImage.reset();

	m_pVideoWndFrame = NULL;
	m_bRefreshPlayer = true;
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
	ON_WM_TIMER()
	ON_WM_HOTKEY()
	ON_BN_CLICKED(IDC_BUTTON_TRACECACHE, &CDvoIPCPlayDemoDlg::OnBnClickedButtonTracecache)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_STREAMINFO, &CDvoIPCPlayDemoDlg::OnLvnGetdispinfoListStreaminfo)
	ON_MESSAGE(WM_TROGGLEFULLSCREEN,OnTroggleFullScreen)
	ON_BN_CLICKED(IDC_BUTTON_STOPBACKWORD, &CDvoIPCPlayDemoDlg::OnBnClickedButtonStopbackword)
	ON_BN_CLICKED(IDC_BUTTON_STOPFORWORD, &CDvoIPCPlayDemoDlg::OnBnClickedButtonStopforword)
	ON_BN_CLICKED(IDC_BUTTON_SEEKNEXTFRAME, &CDvoIPCPlayDemoDlg::OnBnClickedButtonSeeknextframe)
	ON_BN_CLICKED(IDC_CHECK_ENABLELOG, &CDvoIPCPlayDemoDlg::OnBnClickedCheckEnablelog)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_PLAYER, &CDvoIPCPlayDemoDlg::OnNMReleasedcaptureSliderPlayer)
	//ON_BN_CLICKED(IDC_CHECK_REFRESHPLAYER, &CDvoIPCPlayDemoDlg::OnBnClickedCheckRefreshplayer)
	ON_BN_CLICKED(IDC_CHECK_ENABLEHACCEL, &CDvoIPCPlayDemoDlg::OnBnClickedCheckEnablehaccel)
	ON_BN_CLICKED(IDC_CHECK_REFRESHPLAYER, &CDvoIPCPlayDemoDlg::OnBnClickedCheckRefreshplayer)
END_MESSAGE_MAP()


CFile *CDvoIPCPlayDemoDlg::m_pVldReport = NULL;

 int __cdecl VLD_REPORT(int reportType, wchar_t *message, int *returnValue)
{
	if (CDvoIPCPlayDemoDlg::m_pVldReport)
	{
		CDvoIPCPlayDemoDlg::m_pVldReport->Write(message, wcslen(message));
	}
	return 0;
}

// CDvoIPCPlayDemoDlg 消息处理程序
 
void CDvoIPCPlayDemoDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	if (nKey2 == VK_F12)
	{
		if (m_pVldReport)
		{
			delete m_pVldReport;
			m_pVldReport = NULL;
		}
		else
		{
			if (PathFileExists(_T("Vld_Report.txt")))
				DeleteFile(_T("Vld_Report.txt"));
			m_pVldReport = new CFile(_T("Vld_Report.txt"), CFile::modeCreate | CFile::modeWrite);
		}
		
// 		VLDSetReportOptions( VLD_OPT_REPORT_TO_DEBUGGER,NULL);
// 		VLDReportLeaks();
	}

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


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
	CenterWindow(this);

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_pPlayerInfo = boost::make_shared<PlayerInfo>();
	m_wndStatus.SubclassDlgItem(IDC_STATIC_STATUS, this);

	SendDlgItemMessage(IDC_COMBO_PICTYPE, CB_SETCURSEL, 1, 0);		// 默认使用JPG截图
	SendDlgItemMessage(IDC_COMBO_PLAYSPEED, CB_SETCURSEL, 8, 0);	// 默认播放速度为1X
	//m_wndBrowseCtrl.SubclassDlgItem(IDC_MFCEDITBROWSE, this);
	m_wndStreamInfo.SubclassDlgItem(IDC_LIST_STREAMINFO, this);
	m_wndStreamInfo.SetExtendedStyle(m_wndStreamInfo.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	int nCol = 0;

	m_wndStreamInfo.InsertColumn(nCol++, _T("项目"), LVCFMT_LEFT, 60);
	m_wndStreamInfo.InsertColumn(nCol++, _T("内容"), LVCFMT_LEFT, 130);
	CheckRadioButton(IDC_RADIO_DVONETSDK, IDC_RADIO_INSIDE_NETSDK, IDC_RADIO_DVONETSDK);
	int nItem = 0;
	ZeroMemory(m_szListText, sizeof(ListItem) * 16);
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("视频信息"));
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("音频编码"));
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("码      率"));
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("帧      率"));
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("视频缓存"));
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("音频缓存"));
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("码流总长"));
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("连接时长"));
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("播放时长"));
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("录像文件"));
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("录像长度"));
	_tcscpy_s(m_szListText[nItem++].szItemName, 32, _T("录像时长"));
	m_wndStreamInfo.SetItemCount(nItem);
	SendDlgItemMessage(IDC_COMBO_STREAM, CB_SETCURSEL, 0, 0);
	SetDlgItemText(IDC_EDIT_ACCOUNT, _T("admin"));
	SetDlgItemText(IDC_EDIT_PASSWORD, _T("admin"));
	LoadSetting();
	CRect rtClient;
	GetDlgItemRect(IDC_STATIC_FRAME, rtClient);
	m_pVideoWndFrame = new CVideoFrame;
	m_pVideoWndFrame->Create(1024, rtClient, _Row, _Col, this);
	BOOL bRedraw = FALSE;
	UINT nSliderIDArray[] = {
		IDC_SLIDER_SATURATION,
		IDC_SLIDER_BRIGHTNESS,
		IDC_SLIDER_CONTRAST,
		IDC_SLIDER_CHROMA,
		IDC_SLIDER_ZOOMSCALE,
		IDC_SLIDER_VOLUME,
		IDC_SLIDER_PLAYER};
	// 设置滑动块的取舍范围
	for (int i = 0; i < sizeof(nSliderIDArray) / sizeof(UINT); i++)
	{
		EnableDlgItem(nSliderIDArray[i], false);
		SendDlgItemMessage(nSliderIDArray[i],TBM_SETRANGEMIN, bRedraw, 0);
		SendDlgItemMessage(nSliderIDArray[i], TBM_SETRANGEMAX, bRedraw, 100);
	}
	SendDlgItemMessage(IDC_SLIDER_VOLUME, TBM_SETPOS, TRUE, 80);
	SendDlgItemMessage(IDC_SLIDER_ZOOMSCALE, TBM_SETPOS, TRUE, 100);
	SetDlgItemText(IDC_EDIT_PICSCALE, "100");
	EnableDlgItem(IDC_SLIDER_ZOOMSCALE, true);

	InitializeCriticalSection(&m_csListStream);
	m_nHotkeyID = GlobalAddAtom(_T("{A1C54F9F-A835-4fca-88DA-BF0C0DA0E6C5}"));
	
	if (m_nHotkeyID)
	{
		RegisterHotKey(m_hWnd, m_nHotkeyID, MOD_ALT | MOD_CONTROL, VK_F12);
	}
	//int nStatus = VLDSetReportHook(VLD_RPTHOOK_INSTALL, VLD_REPORT);
	SetDlgItemInt(IDC_EDIT_ROW, _Row);
	SetDlgItemInt(IDC_EDIT_COL, _Col);
	
	CheckDlgButton(IDC_CHECK_REFRESHPLAYER, BST_CHECKED);
	UINT nIDArreayTop[] = {
		IDC_STATIC_ACCOUNT,
		IDC_EDIT_ACCOUNT,
		IDC_STATIC_PWD,
		IDC_EDIT_PASSWORD,
		IDC_STATIC_CAMERA,
		IDC_IPADDRESS,
		IDC_BUTTON_CONNECT,
		IDC_BUTTON_DISCONNECT,
		IDC_STATIC_STREAM,
		IDC_COMBO_STREAM,
		IDC_BUTTON_PLAYSTREAM,
		IDC_BUTTON_RECORD,
		IDC_STATIC_ROW,
		IDC_EDIT_ROW,
		IDC_STATIC_COL,
		IDC_EDIT_COL,
		IDC_CHECK_ENABLEHACCEL 
	};
	

	UINT nIDArrayRight[] = {
		IDC_STATIC_STREAMINFO,
		IDC_CHECK_REFRESHPLAYER,
		IDC_LIST_STREAMINFO,
		IDC_STATIC_COLOR,
		IDC_STATIC_SATURATION,
		IDC_SLIDER_SATURATION,
		IDC_EDIT_SURATION,
		IDC_STATIC_BRIGHTNESS,
		IDC_SLIDER_BRIGHTNESS,
		IDC_EDIT_BRIGHTNESS,
		IDC_STATIC_CONTRAST,
		IDC_SLIDER_CONTRAST,
		IDC_EDIT_CONTRAST,
		IDC_STATIC_CHROMA,
		IDC_SLIDER_CHROMA,
		IDC_EDIT_CHROMA,
		IDC_STATIC_GRAPH,
		IDC_STATIC_PICSCALE,
		IDC_COMBO_PICSCALE,
		IDC_STATIC_ZOOMSCALE,
		IDC_SLIDER_ZOOMSCALE,
		IDC_EDIT_PICSCALE,
		IDC_CHECK_FITWINDOW,
		IDC_BUTTON_SNAPSHOT,
		IDC_COMBO_PICTYPE,
		IDC_BUTTON_TRACECACHE
	};
	UINT nIDArrayBottom[] = {
			IDC_STATIC_FILE,
			IDC_EDIT_FILEPATH,
			IDC_BUTTON_PLAYFILE,
			IDC_BUTTON_PAUSE,
			IDC_STATIC_SPEED,
			IDC_STATIC_VIDEOCACHE,
			IDC_EDIT_PLAYCACHE,
			IDC_CHECK_STREAMPLAY,
			IDC_CHECK_ENABLELOG,
			IDC_STATIC_TOTALTIME,
			IDC_BUTTON_STOPBACKWORD,
			IDC_SLIDER_PLAYER,
			IDC_BUTTON_SEEKNEXTFRAME,
			IDC_STATIC_CURTIME,
			IDC_EDIT_PLAYTIME,
			IDC_STATIC_CURFRAME,
			IDC_EDIT_PLAYFRAME,
			IDC_STATIC_CURFPS,
			IDC_EDIT_FPS,
			IDC_CHECK_DISABLEAUDIO,
			IDC_STATIC_VOLUME,
			IDC_SLIDER_VOLUME,
			IDC_STATIC_STATUS,
			IDC_BUTTON_STOPFORWORD,
			IDC_COMBO_PLAYSPEED
		};
	UINT nIDArrayCenter[] = { IDC_STATIC_FRAME };

	RECT rtDialog;
	GetClientRect(&rtDialog);
	CWnd *pItemWnd =  GetDlgItem(IDC_STATIC_ACCOUNT);
#if _MSC_VER >= 1600
 	SaveWndPosition(nIDArreayTop, sizeof(nIDArreayTop) / sizeof(UINT), DockTop, rtDialog);
 	SaveWndPosition(nIDArrayRight, sizeof(nIDArrayRight) / sizeof(UINT), DockRigth, rtDialog);
 	SaveWndPosition(nIDArrayBottom, sizeof(nIDArrayBottom) / sizeof(UINT), DockBottom, rtDialog);
 	SaveWndPosition(nIDArrayCenter, sizeof(nIDArrayCenter) / sizeof(UINT), DockCenter, rtDialog);
#endif

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDvoIPCPlayDemoDlg::SaveWndPosition(UINT *nDlgItemIDArray, UINT nItemCount, DockType nDock, RECT rtDialogClientRect)
{
	WndPostionInfo wndPos;
	wndPos.Dock = nDock;
	for (int i = 0; i < nItemCount; i++)
	{
		wndPos.hWnd = GetDlgItem(nDlgItemIDArray[i])->m_hWnd;
		wndPos.nID = nDlgItemIDArray[i];
		::GetWindowRect(wndPos.hWnd, &wndPos.rect);
		ScreenToClient(&wndPos.rect);
		switch (wndPos.Dock)
		{
		default:
		case DockTop:		// 无须变动
		{
			wndPos.DockDistance[0] = wndPos.rect.top - rtDialogClientRect.top;
			break;
		}
		case DockLeft:		// 无须变动
		{
			wndPos.DockDistance[0] = wndPos.rect.left - rtDialogClientRect.left;
			break;
		}
		case DockBottom:
		{
			wndPos.DockDistance[0] = rtDialogClientRect.bottom - wndPos.rect.bottom;
			break;
		}
		case DockRigth:
		{
			wndPos.DockDistance[0] = rtDialogClientRect.right - wndPos.rect.right;
			break;
		}
		case DockCenter:
		{
			wndPos.DockDistance[0] = wndPos.rect.left - rtDialogClientRect.left;
			wndPos.DockDistance[1] = wndPos.rect.top - rtDialogClientRect.top;
			wndPos.DockDistance[2] = rtDialogClientRect.right - wndPos.rect.right;
			wndPos.DockDistance[3] = rtDialogClientRect.bottom - wndPos.rect.bottom;
		}
			break;
		}
		
		m_vWndPostionInfo.push_back(wndPos);
	}
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

	if (GetDlgItem(IDC_EDIT_ACCOUNT)->GetSafeHwnd())
	{
		RECT rtDialog;
		GetClientRect(&rtDialog);
		for (vector<WndPostionInfo>::iterator it = m_vWndPostionInfo.begin(); it != m_vWndPostionInfo.end(); it++)
		{
			WndPostionInfo wndPos = (*it);
			RECT rt = wndPos.rect;
			switch (wndPos.Dock)
			{
			default:
			case DockTop:		// 无须变动
			case DockLeft:		// 无须变动
				break;
			case DockBottom:
			{
				if (nType == SIZE_MAXIMIZED)
				{
					rt.bottom = rtDialog.bottom - wndPos.DockDistance[0];
					rt.top = rt.bottom - (wndPos.rect.bottom - wndPos.rect.top);
					::MoveWindow(wndPos.hWnd, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, true);
				}
				else if (nType == SIZE_RESTORED)
				{
					::MoveWindow(wndPos.hWnd, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, true);
				}
			}
			break;
			case DockRigth:
			{
				if (nType == SIZE_MAXIMIZED)
				{
					rt.right = rtDialog.right - wndPos.DockDistance[0];
					rt.left = rt.right - (wndPos.rect.right - wndPos.rect.left);
					::MoveWindow(wndPos.hWnd, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, true);
				}
				else if (nType == SIZE_RESTORED)
				{
					::MoveWindow(wndPos.hWnd, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, true);
				}
				break;

			}
			case DockCenter:
			{
				if (nType == SIZE_MAXIMIZED)
				{
					rt.left = rtDialog.left + wndPos.DockDistance[0];
					rt.top = rtDialog.top + wndPos.DockDistance[1];
					rt.right = rtDialog.right - wndPos.DockDistance[2];
					rt.bottom = rtDialog.bottom - wndPos.DockDistance[3];					
					::MoveWindow(wndPos.hWnd, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, true);
					if (wndPos.nID == IDC_STATIC_FRAME)
						m_pVideoWndFrame->MoveWindow(&rt, true);
				}
				else if (nType == SIZE_RESTORED)
				{
					::MoveWindow(wndPos.hWnd, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, true);
					if (wndPos.nID == IDC_STATIC_FRAME)
						m_pVideoWndFrame->MoveWindow(&rt, true);
				}
				break;
			}
			}
		}
// 		if (m_pPlayContext)
// 			for (int i = 0; i < m_pPlayContext->nPlayerCount; i++)
// 			{
// 				dvoplay_Reset(m_pPlayContext->hPlayer[i]);
// 			}
	}
}

void CDvoIPCPlayDemoDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	m_bThreadStream = false;
	if (m_hThreadPlayStream && m_hThreadSendStream)
	{
		HANDLE hArray[] = { m_hThreadPlayStream, m_hThreadSendStream };
		WaitForMultipleObjects(2, hArray, TRUE, INFINITE);
		CloseHandle(m_hThreadPlayStream);
		CloseHandle(m_hThreadSendStream);
		m_hThreadPlayStream = NULL;
		m_hThreadSendStream = NULL;
	}

	m_pPlayContext.reset();
	if (m_pVideoWndFrame)
	{
		m_pVideoWndFrame->DestroyWindow();
		delete m_pVideoWndFrame;
	}
	DeleteCriticalSection(&m_csListStream);
	//VLDSetReportHook(VLD_RPTHOOK_REMOVE, VLD_REPORT);
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
	if (!IsValidIPAddress(szIPAddress))
	{
		AfxMessageBox(_T("请输入一个有效的相机IP"), MB_OK | MB_ICONSTOP);
		return;
	}
	int nNetType =  GetCheckedRadioButton(IDC_RADIO_DVONETSDK, IDC_RADIO_INSIDE_NETSDK);
	m_pPlayContext = boost::make_shared<PlayerContext>(-1, -1, (DVO_PLAYHANDLE *)NULL, _Row*_Col);
	
	m_pPlayContext->pThis = this;
	bool bSucceed = false;
	switch (nNetType)
	{
		default:
		case IDC_RADIO_DVONETSDK:
		{
			app_net_tcp_sys_logo_info_t LoginInfo;
			USER_HANDLE hUser = DVO2_NET_Login(szIPAddress, 6001, szAccount, szPassowd, &LoginInfo, &nError, 5000);
			if (hUser != -1)
			{
				if (ComboBox_FindString(::GetDlgItem(m_hWnd, IDC_IPADDRESS), 0, szIPAddress) == CB_ERR)
					ComboBox_AddString(::GetDlgItem(m_hWnd, IDC_IPADDRESS), szIPAddress);
				m_pPlayContext->hUser = hUser;
				bSucceed = true;
				
			}
		}
			break;
		case IDC_RADIO_TVTNETSDK:
		{
			m_pTVTPlay = new CTVTChan_1();
			bSucceed = m_pTVTPlay->Play(szIPAddress, 9008, m_pVideoWndFrame->GetPanelWnd(0));
		}
			break;
		case IDC_RADIO_INSIDE_NETSDK:
		{
			CSocketClient *pClient = new CSocketClient();
			if (pClient->Connect(szIPAddress, 6001) == 0)
			{
				m_pPlayContext->pClient = pClient;
				bSucceed = true;
			}
			else
				delete pClient;
		}
		break;

	}
	if (bSucceed)
	{
		_tcscpy_s(m_pPlayContext->szIpAddress, 32, szIPAddress);
		SaveSetting();
		EnableDlgItem(IDC_BUTTON_DISCONNECT, true);
		EnableDlgItem(IDC_BUTTON_CONNECT, false);
		EnableDlgItem(IDC_EDIT_ACCOUNT, false);
		EnableDlgItem(IDC_EDIT_PASSWORD, false);
		EnableDlgItem(IDC_BUTTON_PLAYSTREAM, true);
		EnableDlgItem(IDC_BUTTON_RECORD, true);
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
		DVO_MEDIAINFO MediaHeader;
		bool bEnableAudio = (bool)IsDlgButtonChecked(IDC_CHECK_DISABLEAUDIO);
		bool bFitWindow = (bool)IsDlgButtonChecked(IDC_CHECK_FITWINDOW);
		bool bEnableLog = (bool)IsDlgButtonChecked(IDC_CHECK_ENABLELOG);
		bool bEnableHaccel = (bool)IsDlgButtonChecked(IDC_CHECK_ENABLEHACCEL);
		bool bEnableProbeStream = (bool)IsDlgButtonChecked(IDC_CHECK_PROBESTREAM);
		int nVolume = SendDlgItemMessage(IDC_SLIDER_VOLUME, TBM_GETPOS);
		m_nRow = GetDlgItemInt(IDC_EDIT_ROW);
		m_nCol = GetDlgItemInt(IDC_EDIT_COL);
		if (m_pVideoWndFrame->GetRows() != m_nRow ||
			m_pVideoWndFrame->GetCols() != m_nCol)
		{
			if (m_nRow*m_nCol <= 36)
			{
				m_pVideoWndFrame->AdjustPanels(m_nRow, m_nCol);
				m_pPlayContext->nPlayerCount = m_nRow*m_nCol;
			}
			else
			{
				m_pVideoWndFrame->AdjustPanels(36);
				m_pPlayContext->nPlayerCount = 36;
			}
		}
		m_pPlayContext->nPlayerCount = m_nCol*m_nRow;
		bool bEnableWnd = false;
		int nStream = 0;
		CWaitCursor Wait;
		int nNetType = GetCheckedRadioButton(IDC_RADIO_DVONETSDK, IDC_RADIO_INSIDE_NETSDK);
		switch (nNetType)
		{
		case IDC_RADIO_DVONETSDK:
		default:
		{
			if (m_pPlayContext->hStream == -1)
			{
				nStream = SendDlgItemMessage(IDC_COMBO_STREAM, CB_GETCURSEL);
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
				EnableDlgItem(IDC_COMBO_STREAM, false);
			}
			if (IsDlgButtonChecked(IDC_CHECK_ONLYSTREAM) != BST_CHECKED)
			if (!m_pPlayContext->hPlayer[0])
			{
				int nFreePanel = 0;
				app_net_tcp_enc_info_t stResult = { 0 };
				app_net_tcp_com_schn_t req = { 0 };
				req.chn = 0;
				req.schn = nStream;

				for (int k = 0; k < 3; k++)
				{
					int nReBytes = 0;
					int  nRet = DVO2_NET_GetDevConfig(m_pPlayContext->hUser, DVO_DEV_CMD_STREAM_VIDEO_ENC_GET, &req, sizeof(app_net_tcp_com_schn_t), &stResult, sizeof(app_net_tcp_enc_info_t), &nReBytes);
					if (RET_SUCCESS == nRet)
					{
						enum APP_NET_TCP_COM_VIDEO_MODE
						{
							APP_NET_TCP_COM_VIDEO_MODE_352_288 = 0,
							APP_NET_TCP_COM_VIDEO_MODE_704_576,
							APP_NET_TCP_COM_VIDEO_MODE_1280_720,
							APP_NET_TCP_COM_VIDEO_MODE_1920_1080,
							APP_NET_TCP_COM_VIDEO_MODE_1280_960,
							APP_NET_TCP_COM_VIDEO_MODE_1024_768,
							APP_NET_TCP_COM_VIDEO_MODE_176_144 = 0xFF,
							APP_NET_TCP_COM_VIDEO_MODE_MAX,
						}; //视频编码尺寸。
						if (stResult.enc_type != 3)
							MediaHeader.nVideoCodec = (DVO_CODEC)stResult.enc_type;
						else
							MediaHeader.nVideoCodec = CODEC_H265;
						switch (stResult.fmt.enc_mode)
						{
						case APP_NET_TCP_COM_VIDEO_MODE_352_288:
						{
							MediaHeader.nVideoWidth = 352;
							MediaHeader.nVideoHeight = 288;
							break;
						}
						case	APP_NET_TCP_COM_VIDEO_MODE_704_576:
						{
							MediaHeader.nVideoWidth = 704;
							MediaHeader.nVideoHeight = 576;
							break;
						}
						case	APP_NET_TCP_COM_VIDEO_MODE_1280_720:
						{
							MediaHeader.nVideoWidth = 1280;
							MediaHeader.nVideoHeight = 720;
							break;
						}
						case	APP_NET_TCP_COM_VIDEO_MODE_1920_1080:
						{
							MediaHeader.nVideoWidth = 1920;
							MediaHeader.nVideoHeight = 1080;
							break;
						}
						case	APP_NET_TCP_COM_VIDEO_MODE_1280_960:
						{
							MediaHeader.nVideoWidth = 1280;
							MediaHeader.nVideoHeight = 960;
							break;
						}
						case	APP_NET_TCP_COM_VIDEO_MODE_1024_768:
						{
							MediaHeader.nVideoWidth = 1024;
							MediaHeader.nVideoHeight = 768;
							break;
						}
						case	APP_NET_TCP_COM_VIDEO_MODE_176_144:
						{
							MediaHeader.nVideoWidth = 176;
							MediaHeader.nVideoHeight = 144;
							break;
						}
						break;
						default:
						{
							AfxMessageBox("无效的视频尺寸信息", MB_OK | MB_ICONSTOP);
							return;
							break;
						}
						}
						break;
					}
				}
				for (int i = 0; i < m_pPlayContext->nPlayerCount; i++)
					//int i = 0;
				{
					m_pPlayContext->hWndView = m_pVideoWndFrame->GetPanelWnd(i);
					if (!bEnableProbeStream)
						m_pPlayContext->hPlayer[i] = dvoplay_OpenStream(m_pVideoWndFrame->GetPanelWnd(i), (byte *)&MediaHeader, sizeof(MediaHeader), 100, bEnableLog ? "dvoipcplaysdk" : NULL);
					else
						m_pPlayContext->hPlayer[i] = dvoplay_OpenStream(m_pVideoWndFrame->GetPanelWnd(i), NULL, 0, 100, bEnableLog ? "dvoipcplaysdk" : NULL);

					m_pVideoWndFrame->SetPanelParam(i, m_pPlayContext.get());
					if (!m_pPlayContext->hPlayer[i])
					{
						m_wndStatus.SetWindowText(_T("打开流播放句柄失败"));
						m_wndStatus.SetAlarmGllitery();
						return;
					}
					dvoplay_Refresh(m_pPlayContext->hPlayer[i]);
					if (i == 0)
						bEnableAudio = true;
					else
						bEnableAudio = false;
					dvoplay_Start(m_pPlayContext->hPlayer[i], bEnableAudio, bFitWindow,bEnableHaccel);
					dvoplay_SetVolume(m_pPlayContext->hPlayer[i], nVolume);
				}
				m_dfLastUpdate = GetExactTime();
				SetDlgItemText(IDC_BUTTON_PLAYSTREAM, _T("停止播放"));

				EnableDlgItems(m_hWnd, true, 6,
					IDC_SLIDER_SATURATION,
					IDC_SLIDER_BRIGHTNESS,
					IDC_SLIDER_CONTRAST,
					IDC_SLIDER_CHROMA,
					IDC_SLIDER_ZOOMSCALE,
					IDC_SLIDER_VOLUME);
				EnableDlgItem(IDC_SLIDER_PLAYER, false);
				EnableDlgItem(IDC_EDIT_ROW, false);
				EnableDlgItem(IDC_EDIT_COL, false);
			}
			else
			{
				if (!m_pPlayContext->pRecFile)		// 若正在录像，则不应断开码流
				{
					DVO2_NET_StopRealPlay(m_pPlayContext->hStream);
					m_pPlayContext->hStream = -1;
					EnableDlgItem(IDC_COMBO_STREAM, true);
				}
				for (int i = 0; i < m_pPlayContext->nPlayerCount; i++)
					if (m_pPlayContext->hPlayer[i])
					{
						dvoplay_Stop(m_pPlayContext->hPlayer[i]);
						//dvoplay_Refresh(m_pPlayContext->hPlayer);
						dvoplay_Close(m_pPlayContext->hPlayer[i]);
						m_pPlayContext->hPlayer[i] = NULL;
					}
				SetDlgItemText(IDC_BUTTON_PLAYSTREAM, _T("播放码流"));
				EnableDlgItems(m_hWnd, false, 6,
					IDC_SLIDER_SATURATION,
					IDC_SLIDER_BRIGHTNESS,
					IDC_SLIDER_CONTRAST,
					IDC_SLIDER_CHROMA,
					IDC_SLIDER_ZOOMSCALE,
					IDC_SLIDER_VOLUME);
				EnableDlgItem(IDC_SLIDER_PLAYER, false);
				EnableDlgItem(IDC_EDIT_ROW, true);
				EnableDlgItem(IDC_EDIT_COL, true);
				m_dfLastUpdate = 0.0f;
			}
		}
		break;
		case IDC_RADIO_TVTNETSDK:
		{

		}
			break;
		case IDC_RADIO_INSIDE_NETSDK:
		{
			if (m_pPlayContext->pClient)
			{
				MSG_HEAD MsgHead;
				ZeroMemory(&MsgHead, sizeof(MSG_HEAD));
				MsgHead.Magic1[0] = 0xF5;
				MsgHead.Magic1[1] = 0x5A;
				MsgHead.Magic1[2] = 0xA5;
				MsgHead.Magic1[3] = 0x5F;
				MsgHead.Version = htons(0x11);
				MsgHead.DataType = htons(0x01);
				MsgHead.CmdType = htons(0x02);
				MsgHead.CmdSubType = htons(0x03);
				MsgHead.Pktlen = htonl(sizeof(MsgHead) + sizeof(app_net_tcp_enc_ctl_t));
				app_net_tcp_enc_ctl_t Req;
				ZeroMemory(&Req, sizeof(Req));
				Req.chn = 0;
				Req.stream = htonl(nStream);
				Req.enable = htonl(1);		// Enable
				Req.nettype = 0;		//	TCP
				Req.port = 0;
				int nCommandLen = sizeof(MSG_HEAD) + sizeof(Req);
				char *pReqCommand = new char[sizeof(MSG_HEAD) + sizeof(Req) + 1];
				memcpy(pReqCommand, &MsgHead, sizeof(MSG_HEAD));
				memcpy(&pReqCommand[sizeof(MSG_HEAD)], &Req, sizeof(Req));
				DWORD nBytesSent = 0;
				bool bSucceed = false;

				if (m_pPlayContext->pClient->Send((char *)pReqCommand, nCommandLen, nBytesSent) == 0)
				{
					ZeroMemory(&MsgHead, sizeof(MSG_HEAD));
					DWORD nBytesRecv = 0;
					if (m_pPlayContext->pClient->Recv((char *)&MsgHead, sizeof(MSG_HEAD), nBytesRecv) == 0 &&
						nBytesRecv == sizeof(MSG_HEAD))
					{
						app_net_tcp_enc_ctl_ack_t Res;
						if (m_pPlayContext->pClient->Recv((char *)&Res, sizeof(Res), nBytesRecv) == 0 &&
							nBytesRecv == sizeof(Res))
						{
							if (Res.stream == nStream && Res.state == 0)
							{
								bSucceed = true;
							}
						}
					}
				}
				if (!bSucceed)
				{
					m_wndStatus.SetWindowText(_T("连接码流失败"));
					m_wndStatus.SetAlarmGllitery();
					return;
				}
				// 启动接收线程
				m_pPlayContext->StartRecv(StreamCallBack);
				if (IsDlgButtonChecked(IDC_CHECK_ONLYSTREAM) != BST_CHECKED)
				{
					m_pPlayContext->dfTimeRecv1 = GetExactTime();
					m_pPlayContext->hStream = -1;
					for (int i = 0; i < m_pPlayContext->nPlayerCount; i++)
						//int i = 0;
					{
						m_pPlayContext->hWndView = m_pVideoWndFrame->GetPanelWnd(i);
						bool bEnableRunlog = (bool)IsDlgButtonChecked(IDC_CHECK_ENABLELOG);
						m_pPlayContext->hPlayer[i] = dvoplay_OpenStream(m_pVideoWndFrame->GetPanelWnd(i), (byte *)&MediaHeader, sizeof(MediaHeader), 128, bEnableRunlog ? "dvoipcplaysdk" : nullptr);
						//m_pPlayContext->hPlayer[i] = dvoplay_OpenStream(m_pPlayContext->hWndView, nullptr, sizeof(MediaHeader), 0, bEnableRunlog ? "dvoipcplaysdk" : nullptr);

						m_pVideoWndFrame->SetPanelParam(i, m_pPlayContext.get());
						if (!m_pPlayContext->hPlayer[i])
						{
							m_wndStatus.SetWindowText(_T("打开流播放句柄失败"));
							m_wndStatus.SetAlarmGllitery();
							return;
						}
						dvoplay_Refresh(m_pPlayContext->hPlayer[i]);
						if (i == 0)
							bEnableAudio = true;
						else
							bEnableAudio = false;
						dvoplay_Start(m_pPlayContext->hPlayer[i], bEnableAudio, bFitWindow);
						dvoplay_SetVolume(m_pPlayContext->hPlayer[i], nVolume);
					}
				}

				m_dfLastUpdate = GetExactTime();
				SetDlgItemText(IDC_BUTTON_PLAYSTREAM, _T("停止播放"));

				EnableDlgItems(m_hWnd, true, 6,
					IDC_SLIDER_SATURATION,
					IDC_SLIDER_BRIGHTNESS,
					IDC_SLIDER_CONTRAST,
					IDC_SLIDER_CHROMA,
					IDC_SLIDER_ZOOMSCALE,
					IDC_SLIDER_VOLUME);
				EnableDlgItem(IDC_SLIDER_PLAYER, false);
				EnableDlgItem(IDC_EDIT_ROW, false);
				EnableDlgItem(IDC_EDIT_COL, false);
			}
			else
			{
				if (!m_pPlayContext->pRecFile)		// 若正在录像，则不应断开码流
				{
					m_pPlayContext->StopRecv();
					m_pPlayContext->hStream = -1;
					EnableDlgItem(IDC_COMBO_STREAM, true);
				}
				for (int i = 0; i < m_pPlayContext->nPlayerCount; i++)
					if (m_pPlayContext->hPlayer[i])
					{
						dvoplay_Stop(m_pPlayContext->hPlayer[i]);
						//dvoplay_Refresh(m_pPlayContext->hPlayer);
						dvoplay_Close(m_pPlayContext->hPlayer[i]);
						m_pPlayContext->hPlayer[i] = nullptr;
					}
				SetDlgItemText(IDC_BUTTON_PLAYSTREAM, _T("播放码流"));
				EnableDlgItems(m_hWnd, false, 6,
					IDC_SLIDER_SATURATION,
					IDC_SLIDER_BRIGHTNESS,
					IDC_SLIDER_CONTRAST,
					IDC_SLIDER_CHROMA,
					IDC_SLIDER_ZOOMSCALE,
					IDC_SLIDER_VOLUME);
				EnableDlgItem(IDC_SLIDER_PLAYER, false);
				EnableDlgItem(IDC_EDIT_ROW, true);
				EnableDlgItem(IDC_EDIT_COL, true);
				m_dfLastUpdate = 0.0f;
			}
		}
			break;
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
	if (m_pPlayContext)
	{
		CWaitCursor Wait;		
		if (!m_pPlayContext->pRecFile)
		{
			int nStream = SendDlgItemMessage(IDC_COMBO_STREAM, CB_GETCURSEL);
			if (m_pPlayContext->hStream == -1)
			{
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
					m_wndStatus.SetWindowText(_T("连接码流失败,无法录像"));
					m_wndStatus.SetAlarmGllitery();
					return;
				}
				m_pPlayContext->hStream = hStreamHandle;
				m_pPlayContext->pThis = this;
				EnableDlgItem(IDC_COMBO_STREAM, false);
			}

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
			if (!m_pPlayContext->hPlayer[0] && m_pPlayContext->hStream != -1)
			{// 未播放码流,并且码流有效,则要断开码流
				DVO2_NET_StopRealPlay(m_pPlayContext->hStream);
				m_pPlayContext->hStream = -1;
				EnableDlgItem(IDC_COMBO_STREAM, true);
			}
			TraceMsgA("%s LastVideoFrameID = %d\tLastAudioFrameID = %d.\n", __FUNCTION__, m_pPlayContext->nVideoFrameID, m_pPlayContext->nAudioFrameID);
			TraceMsgA("%s VideoFrameCount = %d\tAudioFrameCount = %d.\n", __FUNCTION__, m_pPlayContext->pStreamInfo->nVideoFrameCount, m_pPlayContext->pStreamInfo->nAudioFrameCount );
			SetDlgItemText(IDC_BUTTON_RECORD, _T("开始录像"));
		}
	}
}


void CDvoIPCPlayDemoDlg::OnBnClickedButtonPlayfile()
{
	bool bEnableWnd = true;
	UINT bIsStreamPlay = IsDlgButtonChecked(IDC_CHECK_STREAMPLAY);
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
					(MediaHeader.nMediaTag != DVO_TAG &&	// 头标志 固定为   0x44564F4D 即字符串"MOVD"
					MediaHeader.nMediaTag != GSJ_TAG))
				{
					m_wndStatus.SetWindowText(_T("指定的文件不是一个有效的DVO录像文件"));
					m_wndStatus.SetAlarmGllitery();	
					return;
				}
				fpMedia.Close();
				SetDlgItemText(IDC_EDIT_FILEPATH, strFilePath);
				m_pPlayContext = boost::make_shared<PlayerContext>(-1);
				m_pPlayContext->hWndView = m_pVideoWndFrame->GetPanelWnd(nFreePanel);
				m_pVideoWndFrame->SetPanelParam(nFreePanel, m_pPlayContext.get());
								
				bool bEnableAudio = (bool)IsDlgButtonChecked(IDC_CHECK_DISABLEAUDIO);
				bool bFitWindow = (bool)IsDlgButtonChecked(IDC_CHECK_FITWINDOW);
				bool bEnableLog = (bool)IsDlgButtonChecked(IDC_CHECK_ENABLELOG);
				bool bEnableHaccel = (bool)IsDlgButtonChecked(IDC_CHECK_ENABLEHACCEL);
				if (bIsStreamPlay != BST_CHECKED)
				{
					//m_pPlayContext->hPlayer[0] = dvoplay_OpenFile(m_pPlayContext->hWndView, (CHAR *)(LPCTSTR)strFilePath,(FilePlayProc)PlayerCallBack,m_pPlayContext.get(),bEnableLog?"dvoipcplaysdk":nullptr);
					m_pPlayContext->hPlayer[0] = dvoplay_OpenFile(m_pPlayContext->hWndView, (CHAR *)(LPCTSTR)strFilePath, NULL, m_pPlayContext.get(), bEnableLog ? "dvoipcplaysdk" : NULL);
					if (!m_pPlayContext->hPlayer[0])
					{
						_stprintf_s(szText, 1024, _T("无法打开%s文件."), strFilePath);
						m_wndStatus.SetWindowText(szText);
						m_wndStatus.SetAlarmGllitery();
						m_pPlayContext.reset();
						return;
					}
				
					PlayerInfo pi;
					if (dvoplay_GetPlayerInfo(m_pPlayContext->hPlayer[0], &pi) != DVO_Succeed)
					{
						m_wndStatus.SetWindowText(_T("获取文件信息失败."));
						m_wndStatus.SetAlarmGllitery();
					}

					time_t T1 = pi.tTotalTime / 1000;
					int nFloat = pi.tTotalTime - T1 * 1000;
					int nHour = T1 / 3600;
					int nMinute = (T1 - nHour * 3600) / 60;
					int nSecond = T1 % 60;
					TCHAR szPlayText[64] = { 0 };
					_stprintf_s(szPlayText, 64, _T("%02d:%02d:%02d"), nHour, nMinute, nSecond);
					SetDlgItemText(IDC_STATIC_TOTALTIME, szPlayText);

					if (dvoplay_Start(m_pPlayContext->hPlayer[0], !bEnableAudio, bFitWindow, bEnableHaccel) != DVO_Succeed)
					{
						m_wndStatus.SetWindowText(_T("无法启动播放器"));
						m_wndStatus.SetAlarmGllitery();
						m_pPlayContext.reset();
						return;
					}
					SendDlgItemMessage(IDC_SLIDER_PLAYER, TBM_SETPOS, TRUE, 0);
					
					SetTimer(ID_PLAYEVENT,_PlayInterval, NULL);
					//this->OnBnClickedButtonPause();
				}
				else
				{
					// 创建文件解析句柄
					// 一般在流媒体服务端创建,用于向客户端提供媒体流数据
					m_pPlayContext->hPlayer[0] = dvoplay_OpenFile(NULL, (CHAR *)(LPCTSTR)strFilePath);					
					if (!m_pPlayContext->hPlayer[0])
					{
						_stprintf_s(szText, 1024, _T("无法打开%s文件."), strFilePath);
						m_wndStatus.SetWindowText(szText);
						m_wndStatus.SetAlarmGllitery();
						m_pPlayContext.reset();
						return;
					}
					PlayerInfo pi;
					if (dvoplay_GetPlayerInfo(m_pPlayContext->hPlayer[0], &pi) != DVO_Succeed)
					{
						m_wndStatus.SetWindowText(_T("获取文件信息失败."));
						m_wndStatus.SetAlarmGllitery();
					}

					time_t T1 = pi.tTotalTime / 1000;
					int nFloat = pi.tTotalTime - T1 * 1000;
					int nHour = T1 / 3600;
					int nMinute = (T1 - nHour * 3600) / 60;
					int nSecond = T1 % 60;
					TCHAR szPlayText[64] = { 0 };
					_stprintf_s(szPlayText, 64, _T("%02d:%02d:%02d"), nHour, nMinute, nSecond);
					SetDlgItemText(IDC_STATIC_TOTALTIME, szPlayText);

					// 创建文件流播放句柄
					// 一般在客户端创建,用于播放服务端发送的媒体流数据
					// 设置较小的播放缓存,移动进度条时,可以及时更新
					m_pPlayContext->hPlayerStream = dvoplay_OpenStream(m_pPlayContext->hWndView, (byte*)&MediaHeader, sizeof(DVO_MEDIAINFO),4);
					if (!m_pPlayContext->hPlayerStream)
					{
						m_wndStatus.SetWindowText(_T("无法打开流播放器."));
						m_wndStatus.SetAlarmGllitery();
						m_pPlayContext.reset();
						return;
					}
					
					if (dvoplay_Start(m_pPlayContext->hPlayerStream, !bEnableAudio, bFitWindow) != DVO_Succeed)
					{
						m_wndStatus.SetWindowText(_T("无法启动流媒体播放器"));
						m_wndStatus.SetAlarmGllitery();
						m_pPlayContext.reset();
						return;
					}
					// 创建读文件帧线程
					m_bThreadStream = true;
					m_hThreadSendStream = (HANDLE)CreateThread(NULL, 0, ThreadSendStream, this, 0, NULL);
					m_hThreadPlayStream = (HANDLE)CreateThread(NULL, 0, ThreadPlayStream, this, 0, NULL);
				}
				int nCurSpeedIndex = 8;
				nCurSpeedIndex = SendDlgItemMessage(IDC_COMBO_PLAYSPEED, CB_GETCURSEL);
				if (nCurSpeedIndex <= 0 && nCurSpeedIndex > 16)
				{
					m_wndStatus.SetWindowText(_T("无效的播放速度,现以原始速度播放."));
					m_wndStatus.SetAlarmGllitery();
					nCurSpeedIndex = 8;
				}
				float fPlayRate = 1.0f;				
				switch (nCurSpeedIndex)
				{
				default:
				case 8:
					break;
				case 0:
					fPlayRate = 1.0 / 32;
					break;
				case 1:
					fPlayRate = 1.0 / 24;
					break;
				case 2:
					fPlayRate = 1.0 / 20;
					break;
				case 3:
					fPlayRate = 1.0 / 16;
					break;
				case 4:
					fPlayRate = 1.0 / 10;
					break;
				case 5:
					fPlayRate = 1.0 / 8;
					break;
				case 6:
					fPlayRate = 1.0 / 4;
					break;
				case 7:
					fPlayRate = 1.0 / 2;
					break;
				case 9:
					fPlayRate = 2.0f;
					break;
				case 10:
					fPlayRate = 4.0f;
					break;
				case 11:
					fPlayRate = 8.0f;
					break;
				case 12:
					fPlayRate = 10.0f;
					break;
				case 13:
					fPlayRate = 16.0f;
					break;
				case 14:
					fPlayRate = 20.0f;
					break;
				case 15:
					fPlayRate = 24.0f;
					break;
				case 16:
					fPlayRate = 32.0f;
					break;
				}
				dvoplay_SetRate(m_pPlayContext->hPlayer[0], fPlayRate);
				m_pPlayContext->pThis = this;
				SetDlgItemText(IDC_BUTTON_PLAYFILE, _T("停止播放"));
				bEnableWnd = false;
				m_dfLastUpdate = GetExactTime();
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
	else if (m_pPlayContext->hPlayer[0])
	{
		KillTimer(ID_PLAYEVENT);
		if (bIsStreamPlay == BST_CHECKED)
		{
			m_bThreadStream = false;
			HANDLE hArray[] = { m_hThreadPlayStream, m_hThreadSendStream };
			WaitForMultipleObjects(2,hArray, TRUE, INFINITE);
			CloseHandle(m_hThreadPlayStream);
			CloseHandle(m_hThreadSendStream);
			m_hThreadPlayStream = NULL;
			m_hThreadSendStream = NULL;
		}
		m_pPlayContext.reset();
		SetDlgItemText(IDC_BUTTON_PLAYFILE, _T("播放文件"));
		m_dfLastUpdate = 0.0f;
	}
	// 禁用所有其它码流播放相关按钮
	EnableDlgItem(IDC_BUTTON_DISCONNECT, bEnableWnd);
	EnableDlgItem(IDC_CHECK_STREAMPLAY, bEnableWnd);
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
					IDC_SLIDER_ZOOMSCALE,
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
		if (TimeSpanEx(pDlg->m_dfLastUpdate) < 0.200f)
			return;
		pDlg->m_dfLastUpdate = GetExactTime();
		int nDvoError = dvoplay_GetPlayerInfo(hPlayHandle, pDlg->m_pPlayerInfo.get());
 		if (nDvoError == DVO_Succeed ||
 			nDvoError == DVO_Error_FileNotExist)
 			pDlg->PostMessage(WM_UPDATE_PLAYINFO, (WPARAM)pDlg->m_pPlayerInfo.get(), (LPARAM)nDvoError);
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
	SYSTEMTIME sysFrame;
	UTC2SystemTime((UINT64 *)&tSec, &sysFrame);
		
	switch (pStreamHeader->frame_type)
	{
	case 0:
	case APP_NET_TCP_COM_DST_IDR_FRAME:
	case APP_NET_TCP_COM_DST_I_FRAME:
	{
		if (!pContext->bRecvIFrame)
		{
			pContext->nVideoFrameID = 1;
			pContext->nAudioFrameID = 1;
			pContext->bRecvIFrame = true;
			pContext->dfTimeRecv1 = GetExactTime();
			pContext->dfTimeRecv2 = GetExactTime();
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
	switch (pStreamHeader->frame_type)
	{
	case 0:
	case APP_NET_TCP_COM_DST_IDR_FRAME:
	case APP_NET_TCP_COM_DST_I_FRAME:
	case APP_NET_TCP_COM_DST_P_FRAME:
	case APP_NET_TCP_COM_DST_B_FRAME:
	{
		if (pContext->m_dfLastInputstream != 0.0f)
		{
			pContext->m_pInputStreamTimeTrace->AddTime(TimeSpanEx(pContext->m_dfLastInputstream));
			if (pContext->m_pInputStreamTimeTrace->nTimeCount >= 100)
				pContext->m_pInputStreamTimeTrace->OutputTime(0.04f);
			
		}
		pContext->m_dfLastInputstream = GetExactTime();
		pContext->nVideoFrames++;
		if (pContext->bRecvIFrame)
		{
// 			SYSTEMTIME sysTime,sysTimeRef;
// 			GetSystemTime(&sysTime);
// 
// 			unsigned long long tNow,tNow2;
// 			unsigned long long tRef;
// 			SystemTime2UTC(&sysTime, &tNow);
// 			if (pContext->nTimeCount == 0)
// 				pContext->nFirstID = pStreamHeader->frame_num;
// 			tNow2 = time(0);
// 			tNow = (tNow + 8*3600) * 1000 * 1000 + (double)sysTime.wMilliseconds * 1000;
// 			
// 			pContext->nTimeStamp[pContext->nTimeCount++] = (tNow -tFrame )/ 1000;
// 			if (pContext->nTimeCount >= 100)
// 			{
// 				UINT64 nSum = 0;
// 				TraceMsgA("%s Play Delay(%d):\n", __FUNCTION__, pContext->nFirstID);
// 				for (int i = 0; i < pContext->nTimeCount; i++)
// 				{
// 					TraceMsgA("%d\t", (pContext->nTimeStamp[i]) );
// 					nSum += ((pContext->nTimeStamp[i]) );
// 
// 					if ((i + 1) % 20 == 0)
// 						TraceMsgA("\n");
// 				}
// 				TraceMsgA("%s Avg Delay = %d.\n", __FUNCTION__, nSum / 100);
// 				pContext->nTimeCount = 0;
// 			}
		}
	}
	break;
	default:
		break;
	}
	
	if (TimeSpanEx(pContext->dfTimeRecv1) >20.0f)
	{
		TraceMsgA("%s Time = %.3f\tVideoFrames = %d\tAutioFrames = %d.\n", __FUNCTION__, TimeSpanEx(pContext->dfTimeRecv2), pContext->nVideoFrameID, pContext->nAudioFrameID);
		pContext->dfTimeRecv1 = GetExactTime();
	}
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
	unsigned long long tNow;
	SystemTime2UTC(&sysTime, &tNow);
	tNow = tNow * 1000 * 1000 + (double)sysTime.wMilliseconds * 1000;
	//for (int i = 0; i < pContext->nPlayerCount;i ++)
 	//if (pContext->hPlayer[i])
	//	dvoplay_InputIPCStream(pContext->hPlayer[i], pFrameData, pStreamHeader->frame_type, nFrameLength, pStreamHeader->frame_num, tNow);

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
	if (pContext->pThis )
	{
		CDvoIPCPlayDemoDlg *pDlg = (CDvoIPCPlayDemoDlg *)pContext->pThis;
		if (!pDlg->m_bRefreshPlayer)
			return;
		if (TimeSpanEx(pDlg->m_dfLastUpdate) < 0.200f)
			return;
		pDlg->m_dfLastUpdate = GetExactTime();
		pDlg->PostMessage(WM_UPDATE_STREAMINFO);
	}
}

LRESULT CDvoIPCPlayDemoDlg::OnUpdatePlayInfo(WPARAM w, LPARAM l)
{ 
	PlayerInfo  *fpi = (PlayerInfo *)w;
	if (fpi )
	{
		if (l != DVO_Error_FileNotExist)
		{
			int nSlidePos = 0;
			if (fpi->nTotalFrames > 0)
				nSlidePos = (int)(100 * (double)fpi->nCurFrameID / fpi->nTotalFrames);
			SendDlgItemMessage(IDC_SLIDER_PLAYER, TBM_SETPOS, TRUE, nSlidePos);
		}

		time_t T1 = fpi->tCurFrameTime / 1000;
		int nFloat = fpi->tCurFrameTime - T1 * 1000;
		int nHour = T1 / 3600;
		int nMinute = (T1 - nHour * 3600) / 60;
		int nSecond = T1 % 60;
		TCHAR szPlayText[64] = { 0 };
		_stprintf_s(szPlayText, 64, _T("%02d:%02d:%02d.%03d"), nHour, nMinute, nSecond, nFloat);
		SetDlgItemText(IDC_EDIT_PLAYTIME, szPlayText);
		_stprintf_s(szPlayText, 64, _T("%d"), fpi->nCurFrameID);
 		SetDlgItemText(IDC_EDIT_PLAYFRAME, szPlayText);

		_stprintf_s(szPlayText, 64, _T("%d"), fpi->nCacheSize);
		SetDlgItemText(IDC_EDIT_PLAYCACHE, szPlayText);

		_stprintf_s(szPlayText, 64, _T("%d"), fpi->nPlayFPS);
		SetDlgItemText(IDC_EDIT_FPS, szPlayText);
		m_dfLastUpdate = GetExactTime();

		if (fpi->nVideoCodec >= CODEC_UNKNOWN && fpi->nVideoCodec <= CODEC_AAC)
			_stprintf_s(m_szListText[Item_VideoInfo].szItemText, 256, _T("%s(%dx%d)"), szCodecString[fpi->nVideoCodec + 1], fpi->nVideoWidth, fpi->nVideoHeight);
		else
			_stprintf_s(m_szListText[Item_VideoInfo].szItemText, 256, _T("N/A"), szCodecString[fpi->nVideoCodec + 1]);

		if (fpi->nAudioCodec >= CODEC_UNKNOWN && fpi->nAudioCodec <= CODEC_AAC)
			_tcscpy(m_szListText[Item_ACodecType].szItemText, szCodecString[fpi->nAudioCodec + 1]);
		else
			_tcscpy(m_szListText[Item_ACodecType].szItemText, _T("N/A"));
		_stprintf_s(m_szListText[Item_VFrameCache].szItemText, 64, _T("%d"), fpi->nCacheSize);
		_stprintf_s(m_szListText[Item_AFrameCache].szItemText, 64, _T("%d"), fpi->nCacheSize2);

		if (fpi->tTimeEplased > 0)
		{
			CTimeSpan tSpan(fpi->tTimeEplased / 1000);
			_stprintf_s(m_szListText[Item_PlayedTime].szItemText, 64, _T("%02d:%02d:%02d"), tSpan.GetHours(), tSpan.GetMinutes(), tSpan.GetSeconds());
		}
		else
			_tcscpy_s(m_szListText[Item_PlayedTime].szItemText, 64, _T("00:00:00"));
		m_wndStreamInfo.Invalidate();
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
	PlayerInfo fpi;
	int nResult = dvoplay_GetPlayerInfo(m_pPlayContext->hPlayer[0], &fpi);
	if (nResult == DVO_Succeed ||
		nResult == DVO_Error_SummaryNotReady ||	// 文件播放概要信息尚未做准备好
		nResult == DVO_Error_FileNotExist)
	{
		if (fpi.nVideoCodec >= CODEC_UNKNOWN && fpi.nVideoCodec <= CODEC_AAC)
			_stprintf_s(m_szListText[Item_VideoInfo].szItemText, 256, _T("%s(%dx%d)"), szCodecString[fpi.nVideoCodec + 1], fpi.nVideoWidth, fpi.nVideoHeight);
		else
			_stprintf_s(m_szListText[Item_VideoInfo].szItemText, 256, _T("M/A"), szCodecString[fpi.nVideoCodec + 1]);
		
		if (fpi.nAudioCodec >= CODEC_UNKNOWN && fpi.nAudioCodec <= CODEC_AAC)
			_tcscpy(m_szListText[Item_ACodecType].szItemText, szCodecString[fpi.nAudioCodec + 1]);
		else
			_tcscpy(m_szListText[Item_ACodecType].szItemText, _T("N/A"));
		_stprintf_s(m_szListText[Item_VFrameCache].szItemText, 64, _T("%d"), fpi.nCacheSize);
		_stprintf_s(m_szListText[Item_AFrameCache].szItemText, 64, _T("%d"), fpi.nCacheSize2);
		
		if (fpi.tTimeEplased > 0)
		{
			CTimeSpan tSpan(fpi.tTimeEplased / 1000);
			_stprintf_s(m_szListText[Item_PlayedTime].szItemText, 64, _T("%02d:%02d:%02d"), tSpan.GetHours(), tSpan.GetMinutes(), tSpan.GetSeconds());
		}
		else
			_tcscpy_s(m_szListText[Item_PlayedTime].szItemText, 64, _T("00:00:00"));
	}
	
	int nStreamRate = m_pPlayContext->pStreamInfo->GetVideoCodeRate();
	_stprintf_s(m_szListText[Item_StreamRate].szItemText, 64, _T("%d Kbps"), nStreamRate);
	_stprintf_s(m_szListText[Item_FrameRate].szItemText, 64, _T("%d fps"), m_pPlayContext->nFPS);
	_stprintf_s(m_szListText[Item_TotalStream].szItemText, 64, _T("%d KB"), (m_pPlayContext->pStreamInfo->nVideoBytes + m_pPlayContext->pStreamInfo->nAudioBytes) / 1024);

	if (m_pPlayContext->pStreamInfo->tFirstTime > 0)
	{
		time_t nRecTime = time(0) * 1000 - m_pPlayContext->pStreamInfo->tFirstTime;
		CTimeSpan tSpan(nRecTime / 1000);
		_stprintf_s(m_szListText[Item_ConnectTime].szItemText, 64, _T("%02d:%02d:%02d"), tSpan.GetHours(), tSpan.GetMinutes(), tSpan.GetSeconds());
	}
	else
		_tcscpy_s(m_szListText[Item_ConnectTime].szItemText, 64, _T("00:00:00"));

	if (m_pPlayContext->pRecFile)
	{
		_tcscpy_s(m_szListText[Item_RecFile].szItemText, 256,m_pPlayContext->szRecFilePath);
		
		time_t nRecTime = time(0) - m_pPlayContext->tRecStartTime;
		CTimeSpan tSpan(nRecTime);
		_stprintf_s(m_szListText[Item_RecTime].szItemText, 64, _T("%02d:%02d:%02d"), tSpan.GetHours(), tSpan.GetMinutes(), tSpan.GetSeconds());
		_stprintf_s(m_szListText[Item_FileLength].szItemText, _T("%d KB"), m_pPlayContext->pRecFile->GetLength() / 1024);
	}
	else
	{
		_tcscpy_s(m_szListText[Item_RecFile].szItemText, 256, _T("暂未录像"));
		_tcscpy_s(m_szListText[Item_RecTime].szItemText, 256, _T("00:00:00"));
		_tcscpy_s(m_szListText[Item_FileLength].szItemText, 256, _T("暂未录像"));
	}
	m_dfLastUpdate = GetExactTime();
	m_wndStreamInfo.Invalidate();

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
	if (m_pPlayContext && m_pPlayContext->hPlayer[0])
	{
		bool bEnable = (bool)IsDlgButtonChecked(IDC_CHECK_DISABLEAUDIO);
		if (dvoplay_EnableAudio(m_pPlayContext->hPlayer[0], !bEnable) != DVO_Succeed)
		{
			m_wndStatus.SetWindowText(_T("开关音频失败."));
			m_wndStatus.SetAlarmGllitery();
		}
	}
}

void CDvoIPCPlayDemoDlg::OnBnClickedCheckFitwindow()
{
	if (m_pPlayContext )
	{
		for (int i = 0; i < m_pPlayContext->nPlayerCount; i++)
		{
			bool bFitWindow = (bool)IsDlgButtonChecked(IDC_CHECK_FITWINDOW);
			if (dvoplay_FitWindow(m_pPlayContext->hPlayer[i], bFitWindow) != DVO_Succeed)
			{
				m_wndStatus.SetWindowText(_T("调整视频显示方式失败."));
				m_wndStatus.SetAlarmGllitery();
				return;
			}
			::InvalidateRect(m_pVideoWndFrame->GetPanelWnd(i), NULL, true);
		}
	}
}

#define _Var(P)	#P

void CDvoIPCPlayDemoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
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
	case IDC_SLIDER_ZOOMSCALE:
	{
		CRect  rtVideoFrame;
		GetDlgItemRect(IDC_STATIC_FRAME, rtVideoFrame);
		int nWidth = rtVideoFrame.Width();
		int nHeight = rtVideoFrame.Height();
		int nCenterX = rtVideoFrame.left +  nWidth/ 2;
		int nCenterY = rtVideoFrame.top + nHeight / 2;
		int nNewWidth = nPos*nWidth / 100;
		int nNewHeight = nPos*nHeight / 100;
		rtVideoFrame.left = nCenterX - nNewWidth / 2;
		rtVideoFrame.right = rtVideoFrame.left + nNewWidth;
		rtVideoFrame.top = nCenterY - nNewHeight / 2;
		rtVideoFrame.bottom = rtVideoFrame.top + nNewHeight;
		m_pVideoWndFrame->MoveWindow(&rtVideoFrame);
		SetDlgItemInt(IDC_EDIT_PICSCALE, nPos);

		TraceMsgA("SliderID:%s\tPos = %d.\n", _Var(IDC_SLIDER_ZOOMSCALE), nPos);
		break;
	}
	case IDC_SLIDER_VOLUME:
	{
		if (!m_pPlayContext || !m_pPlayContext->hPlayer[0])
		{
			CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
			return;
		}
		dvoplay_SetVolume(m_pPlayContext->hPlayer[0], nPos);
	}
		break;
	case IDC_SLIDER_PLAYER:
	{
		if (!m_pPlayContext || !m_pPlayContext->hPlayer[0])
		{
			CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
			return;
		}
		DeclareRunTime();
		switch (nSBCode)
		{
		case SB_PAGEDOWN:
		case SB_PAGEUP:
		case SB_THUMBTRACK:
			KillTimer(ID_PLAYEVENT);
			break;
		case SB_THUMBPOSITION:
		case SB_ENDSCROLL:
		{
			SaveWaitTime();
			SaveRunTime();
			int nPos = SendDlgItemMessage(IDC_SLIDER_PLAYER, TBM_GETPOS);
			SaveRunTime();
			//TraceMsgA("%s Player Slide Pos = %d.\n", __FUNCTION__, nPos);
			int nTotalFrames = 0;
			PlayerInfo pi;
			UINT bIsStreamPlay = IsDlgButtonChecked(IDC_CHECK_STREAMPLAY);
			SaveRunTime();
			if (dvoplay_GetPlayerInfo(m_pPlayContext->hPlayer[0], &pi) == DVO_Succeed)
			{
				SaveRunTime();
				int nSeekFrame = pi.nTotalFrames*nPos / 100;
				bool bUpdate = true;
				if (bIsStreamPlay)
				{
					dvoplay_ClearCache(m_pPlayContext->hPlayerStream);
					SaveRunTime();
					CAutoLock lock(&m_csListStream);
					m_listStream.clear();
					bUpdate = false;		// 流播放无法通过这种方式刷新画面
				}
				SaveRunTime();
				dvoplay_SeekFrame(m_pPlayContext->hPlayer[0], nSeekFrame, bUpdate);
				SaveRunTime();
			}
			m_bClickPlayerSlide = true;
			SetTimer(ID_PLAYEVENT, _PlayInterval, NULL);
		}
			break;
		default:
			break;
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
	if (m_pPlayContext && m_pPlayContext->hPlayer[0])
	{
		CWaitCursor Wait;
		TCHAR szPath[1024];
		GetAppPath(szPath, 1024);
		_tcscat_s(szPath, 1024, _T("\\ScreenSave"));
		if (!PathFileExists(szPath))
		{
			if (!CreateDirectory(szPath,NULL))
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
		if (dvoplay_SnapShot(m_pPlayContext->hPlayer[0], szPath, (SNAPSHOT_FORMAT)nPicType) == DVO_Succeed)
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
	UINT bIsStreamPlay = IsDlgButtonChecked(IDC_CHECK_STREAMPLAY);
	if (m_pPlayContext && m_pPlayContext->hPlayer[0])
	{
		int nCurSecl = SendDlgItemMessage(IDC_COMBO_PLAYSPEED, CB_GETCURSEL);
		if (nCurSecl <= 0 && nCurSecl > 16)
		{
			m_wndStatus.SetWindowText(_T("无效的播放速度."));
			m_wndStatus.SetAlarmGllitery();
			return;
		}
		float fPlayRate = 1.0f;		
		switch (nCurSecl)
		{
		default:
		case 8:
			break;
		case 0:
			fPlayRate = 1.0 / 32;
			break;
		case 1:
			fPlayRate = 1.0 / 24;
			break;
		case 2:
			fPlayRate = 1.0 / 20;
			break;
		case 3:
			fPlayRate = 1.0 / 16;
			break;
		case 4:
			fPlayRate = 1.0 / 10;
			break;
		case 5:
			fPlayRate = 1.0 / 8;
			break;
		case 6:
			fPlayRate = 1.0 / 4;
			break;
		case 7:
			fPlayRate = 1.0 / 2;
			break;		
		case 9:
			fPlayRate = 2.0f;
			break;
		case 10:
			fPlayRate = 4.0f;
			break;
		case 11:
			fPlayRate = 8.0f;
			break;
		case 12:
			fPlayRate = 10.0f;
			break;
		case 13:
			fPlayRate = 16.0f;
			break;
		case 14:
			fPlayRate = 20.0f;
			break;
		case 15:
			fPlayRate = 24.0f;
			break;
		case 16:
			fPlayRate = 32.0f;
			break;
		}
		if (bIsStreamPlay)
			dvoplay_SetRate(m_pPlayContext->hPlayerStream, fPlayRate);
		else
			dvoplay_SetRate(m_pPlayContext->hPlayer[0], fPlayRate);
	}
	else
	{
		m_wndStatus.SetWindowText(_T("播放器尚未启动."));
		m_wndStatus.SetAlarmGllitery();
	}
}


void CDvoIPCPlayDemoDlg::OnBnClickedButtonPause()
{
	UINT bIsStreamPlay = IsDlgButtonChecked(IDC_CHECK_STREAMPLAY);
	if (m_pPlayContext && m_pPlayContext->hPlayer[0])
	{
		if (bIsStreamPlay)
		{
			dvoplay_Pause(m_pPlayContext->hPlayerStream);
		}
		else
			dvoplay_Pause(m_pPlayContext->hPlayer[0]);
		m_bPuased = !m_bPuased;
		if (m_bPuased)
			SetDlgItemText(IDC_BUTTON_PAUSE, _T("继续播放"));
		else
			SetDlgItemText(IDC_BUTTON_PAUSE, _T("暂停播放"));
	}
	else
	{
		m_wndStatus.SetWindowText(_T("播放器尚未启动."));
		m_wndStatus.SetAlarmGllitery();
	}
}


void CDvoIPCPlayDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_PLAYEVENT &&
		m_pPlayContext->hPlayer[0] && m_bRefreshPlayer)
	{
		PlayerInfo pi;
		PlayerInfo *fpi = &pi;
		int nError = dvoplay_GetPlayerInfo(m_pPlayContext->hPlayer[0] ,& pi);
		if (nError != DVO_Error_FileNotExist)
		{
			int nSlidePos = 0;
			if (fpi->nTotalFrames > 0)
				nSlidePos = (int)(100 * (double)fpi->nCurFrameID / fpi->nTotalFrames);
			if (m_bClickPlayerSlide)
			{
				//TraceMsgA("%s New Player Slider Pos = %d.\n", __FUNCTION__, nSlidePos);
				m_bClickPlayerSlide = false;
			}
			SendDlgItemMessage(IDC_SLIDER_PLAYER, TBM_SETPOS, TRUE, nSlidePos);
			//TraceMsgA("%s nSlidePos = %d.\n", __FUNCTION__,nSlidePos);
		}

		time_t T1 = fpi->tCurFrameTime / 1000;
		int nFloat = fpi->tCurFrameTime - T1 * 1000;
		int nHour = T1 / 3600;
		int nMinute = (T1 - nHour * 3600) / 60;
		int nSecond = T1 % 60;
		TCHAR szPlayText[64] = { 0 };
		_stprintf_s(szPlayText, 64, _T("%02d:%02d:%02d.%03d"), nHour, nMinute, nSecond, nFloat);
		SetDlgItemText(IDC_EDIT_PLAYTIME, szPlayText);
		_stprintf_s(szPlayText, 64, _T("%d"), fpi->nCurFrameID);
		SetDlgItemText(IDC_EDIT_PLAYFRAME, szPlayText);

		_stprintf_s(szPlayText, 64, _T("%d"), fpi->nCacheSize);
		SetDlgItemText(IDC_EDIT_PLAYCACHE, szPlayText);

		_stprintf_s(szPlayText, 64, _T("%d"), fpi->nPlayFPS);
		SetDlgItemText(IDC_EDIT_FPS, szPlayText);
		m_dfLastUpdate = GetExactTime();

		if (fpi->nVideoCodec >= CODEC_UNKNOWN && fpi->nVideoCodec <= CODEC_AAC)
			_stprintf_s(m_szListText[Item_VideoInfo].szItemText, 256, _T("%s(%dx%d)"), szCodecString[fpi->nVideoCodec + 1], fpi->nVideoWidth, fpi->nVideoHeight);
		else
			_stprintf_s(m_szListText[Item_VideoInfo].szItemText, 256, _T("M/A"), szCodecString[fpi->nVideoCodec + 1]);

		if (fpi->nAudioCodec >= CODEC_UNKNOWN && fpi->nAudioCodec <= CODEC_AAC)
			_tcscpy(m_szListText[Item_ACodecType].szItemText, szCodecString[fpi->nAudioCodec + 1]);
		else
			_tcscpy(m_szListText[Item_ACodecType].szItemText, _T("N/A"));
		_stprintf_s(m_szListText[Item_VFrameCache].szItemText, 64, _T("%d"), fpi->nCacheSize);
		_stprintf_s(m_szListText[Item_AFrameCache].szItemText, 64, _T("%d"), fpi->nCacheSize2);

		if (fpi->tTimeEplased > 0)
		{
			CTimeSpan tSpan(fpi->tTimeEplased / 1000);
			_stprintf_s(m_szListText[Item_PlayedTime].szItemText, 64, _T("%02d:%02d:%02d"), tSpan.GetHours(), tSpan.GetMinutes(), tSpan.GetSeconds());
		}
		else
			_tcscpy_s(m_szListText[Item_PlayedTime].szItemText, 64, _T("00:00:00"));
		m_wndStreamInfo.Invalidate();

	}
	CDialogEx::OnTimer(nIDEvent);
}


void CDvoIPCPlayDemoDlg::OnBnClickedButtonTracecache()
{
	if (m_pPlayContext && m_pPlayContext->nPlayerCount)
	{
		PlayerInfo pi;
		for (int i = 0; i < m_pPlayContext->nPlayerCount;i ++)
			if (m_pPlayContext->hPlayer[i])
			{
				ZeroMemory(&pi, sizeof(PlayerInfo));
				dvoplay_GetPlayerInfo(m_pPlayContext->hPlayer[i], &pi);
				TraceMsgA("%s Play[%d]:Size = %dx%d\tVideo cache = %d\tAudio Cache = %d.\n", __FUNCTION__, i, pi.nVideoWidth,pi.nVideoHeight,pi.nCacheSize, pi.nCacheSize2);
			}
	}
}


void CDvoIPCPlayDemoDlg::OnLvnGetdispinfoListStreaminfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if (pDispInfo->item.iItem >= 0)
	{
		int nItem = pDispInfo->item.iItem;
		int nSubItem = pDispInfo->item.iSubItem;
		switch (nSubItem)
		{
		case 0:
			pDispInfo->item.pszText = m_szListText[nItem].szItemName;
			break;
		case 1:
			pDispInfo->item.pszText = m_szListText[nItem].szItemText;
			break;
		default:
			break;
		}
	}
	*pResult = 0;
}


void CDvoIPCPlayDemoDlg::OnBnClickedButtonStopbackword()
{
	// 步进5秒
	int nTotalFrames = 0;
	PlayerInfo pi;
	UINT bIsStreamPlay = IsDlgButtonChecked(IDC_CHECK_STREAMPLAY);
	if (dvoplay_GetPlayerInfo(m_pPlayContext->hPlayer[0], &pi) == DVO_Succeed)
	{
		if (pi.tCurFrameTime < 5000)
			return;
		int nSeekTime = pi.tCurFrameTime - 5000;		

		TraceMsgA("%s\nnTotalFrames = %d\ttTotalTime = %I64d\tnCurFrameID = %d\ttCurFrameTime = %I64d\ttTimeEplased = %I64d.\n",
			__FUNCTION__,
			pi.nTotalFrames,
			pi.tTotalTime,
			pi.nCurFrameID,
			pi.tCurFrameTime,
			pi.tTimeEplased);
		bool bUpdate = true;
		if (bIsStreamPlay)
		{
			dvoplay_ClearCache(m_pPlayContext->hPlayerStream);
			CAutoLock lock(&m_csListStream);
			m_listStream.clear();
			bUpdate = false;		// 流播放无法通过这种方式刷新画面
		}

		dvoplay_SeekTime(m_pPlayContext->hPlayer[0], nSeekTime, bUpdate);
	}
}


void CDvoIPCPlayDemoDlg::OnBnClickedButtonStopforword()
{
	// 步进5秒
	int nTotalFrames = 0;
	PlayerInfo pi;
	UINT bIsStreamPlay = IsDlgButtonChecked(IDC_CHECK_STREAMPLAY);
	if (dvoplay_GetPlayerInfo(m_pPlayContext->hPlayer[0], &pi) == DVO_Succeed)
	{
		int nSeekTime = pi.tCurFrameTime + 5000;
		if (nSeekTime > pi.tTotalTime)
			return;
		
		TraceMsgA("%s\nnTotalFrames = %d\ttTotalTime = %I64d\tnCurFrameID = %d\ttCurFrameTime = %I64d\ttTimeEplased = %I64d.\n",
			__FUNCTION__,
			pi.nTotalFrames,
			pi.tTotalTime,
			pi.nCurFrameID,
			pi.tCurFrameTime,
			pi.tTimeEplased);
		bool bUpdate = true;
		if (bIsStreamPlay)
		{
			dvoplay_ClearCache(m_pPlayContext->hPlayerStream);
			CAutoLock lock(&m_csListStream);
			m_listStream.clear();
			bUpdate = false;		// 流播放无法通过这种方式刷新画面
		}

		dvoplay_SeekTime(m_pPlayContext->hPlayer[0], nSeekTime, bUpdate);
	}
}


void CDvoIPCPlayDemoDlg::OnBnClickedButtonSeeknextframe()
{
	// 步进5秒
	int nTotalFrames = 0;
	PlayerInfo pi;
	UINT bIsStreamPlay = IsDlgButtonChecked(IDC_CHECK_STREAMPLAY);
	if (dvoplay_GetPlayerInfo(m_pPlayContext->hPlayer[0], &pi) == DVO_Succeed)
	{
		
		dvoplay_SeekNextFrame(m_pPlayContext->hPlayer[0]);
	}
}


void CDvoIPCPlayDemoDlg::OnBnClickedCheckEnablelog()
{
	if (m_pPlayContext && m_pPlayContext->hPlayer[0])
	{
		if (IsDlgButtonChecked(IDC_CHECK_ENABLELOG) == BST_CHECKED)
			EnableLog(m_pPlayContext->hPlayer[0],"DVOIPCPlaySdk");
		else
			EnableLog(m_pPlayContext->hPlayer[0], NULL);
	}
}



void CDvoIPCPlayDemoDlg::OnNMReleasedcaptureSliderPlayer(NMHDR *pNMHDR, LRESULT *pResult)
{
	//TraceMsgA(_T("Code = %d\thWndFrom = %08X\tidFrom = %d.\n"), pNMHDR->code, pNMHDR->hwndFrom, pNMHDR->idFrom);
	*pResult = 0;
}


// void CDvoIPCPlayDemoDlg::OnBnClickedCheckRefreshplayer()
// {
// 	m_bRefreshPlayer = IsDlgButtonChecked(IDC_CHECK_REFRESHPLAYER);
// }

void CDvoIPCPlayDemoDlg::OnBnClickedCheckEnablehaccel()
{
	 
}

void CDvoIPCPlayDemoDlg::OnBnClickedCheckRefreshplayer()
{
	m_bRefreshPlayer = IsDlgButtonChecked(IDC_CHECK_REFRESHPLAYER);
}
