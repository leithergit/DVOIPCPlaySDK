
// DVOIPCSDKDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DVOIPCSDKDemo.h"
#include "DVOIPCSDKDemoDlg.h"
#include "SetRecordPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDVOIPCSDKDemoDlg 对话框

CDVOIPCSDKDemoDlg::CDVOIPCSDKDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDVOIPCSDKDemoDlg::IDD, pParent)
	,m_pSelectedPen(NULL)
	,m_pUnSelectedPen(NULL)
	,m_nLastSelected(0)
	,m_nHotkeyID(0)
	,m_pCurSeclectDecoder(NULL)
	,m_nCurSelect(-1)
	,m_nDeaultVideoWnd(0)
	,m_nManufacturer(DVO_HACCEL)
{
	ZeroMemory(m_pViewWndState,sizeof(m_pViewWndState));
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	_tcscpy_s(m_szRecordPath,MAX_PATH,_T("d:\\Record"));
}

void CDVOIPCSDKDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDVOIPCSDKDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CDVOIPCSDKDemoDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_PLAYSTREAM, &CDVOIPCSDKDemoDlg::OnBnClickedButtonPlaystream)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_UPDATE_DECODERINFO,&CDVOIPCSDKDemoDlg::OnUpdateDecoderInfo)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_HOTKEY()
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONNECTION, &CDVOIPCSDKDemoDlg::OnNMClickListConnection)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CDVOIPCSDKDemoDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_RECORD, &CDVOIPCSDKDemoDlg::OnBnClickedButtonRecord)
	ON_COMMAND(ID_Menu_Record, &CDVOIPCSDKDemoDlg::OnMenuRecord)
	ON_COMMAND(ID_FILE_STOPRECORD, &CDVOIPCSDKDemoDlg::OnFileStoprecord)
	ON_COMMAND(ID_FILE_OUTPUT, &CDVOIPCSDKDemoDlg::OnFileOutput)
	ON_COMMAND(ID_FILE_LOGIN_REC, &CDVOIPCSDKDemoDlg::OnFileLoginRec)
	ON_COMMAND_RANGE(ID_DECODER_HISILICON,ID_DECODER_HACCEL,&CDVOIPCSDKDemoDlg::OnDecoderSel)
	ON_COMMAND_RANGE(ID_IMAGEQUILTY_HIGHEST,ID_IMAGEQUILTY_HIGHESTSPEED,&CDVOIPCSDKDemoDlg::OnAdjustImageQulity)
// 	ON_COMMAND(ID_DECODER_FFMPEG, &CDVOIPCSDKDemoDlg::OnDecoderFfmpeg)
// 	ON_COMMAND(ID_DECODER_HISILICON, &CDVOIPCSDKDemoDlg::OnDecoderHisilicon)
	ON_COMMAND(ID_DECODER_COMPARE,&CDVOIPCSDKDemoDlg::OnDecoderCompare)
	ON_WM_MBUTTONDOWN()	
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_MESSAGE(WM_RENDERFRAME,&CDVOIPCSDKDemoDlg::OnRenderFrame)
	ON_MESSAGE(WM_INITDXSURFACE,&CDVOIPCSDKDemoDlg::OnInitDxSurface)
	ON_COMMAND(ID_FILE_SETRECORDPATH, &CDVOIPCSDKDemoDlg::OnFileSetrecordpath)
END_MESSAGE_MAP()


// CDVOIPCSDKDemoDlg 消息处理程序


BOOL CDVOIPCSDKDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_wndConnectionList.SubclassDlgItem(IDC_LIST_CONNECTION,this);
	m_wndConnectionList.SetExtendedStyle(m_wndConnectionList.GetExtendedStyle()|LVS_EX_FULLROWSELECT |LVS_EX_DOUBLEBUFFER);	
	int nCol = 0;

	m_wndConnectionList.InsertColumn(nCol ++,_T("序号"),LVCFMT_LEFT,50);
	m_wndConnectionList.InsertColumn(nCol ++,_T("相机IP"),LVCFMT_LEFT,110);
	m_wndConnectionList.InsertColumn(nCol ++,_T("编码类型"),LVCFMT_LEFT,70);	
	m_wndConnectionList.InsertColumn(nCol ++,_T("码流"),LVCFMT_LEFT,50);	
	m_wndConnectionList.InsertColumn(nCol ++,_T("码率"),LVCFMT_LEFT,70);	
	m_wndConnectionList.InsertColumn(nCol ++,_T("帧率"),LVCFMT_LEFT,70);	
	m_wndConnectionList.InsertColumn(nCol ++,_T("缓存"),LVCFMT_LEFT,50);	
	m_wndConnectionList.InsertColumn(nCol ++,_T("码流总长"),LVCFMT_LEFT,80);	
	m_wndConnectionList.InsertColumn(nCol ++,_T("连接时长"),LVCFMT_LEFT,80);	
	m_wndConnectionList.InsertColumn(nCol ++,_T("录像时长"),LVCFMT_LEFT,80);	
	m_wndConnectionList.InsertColumn(nCol ++,_T("录像长度"),LVCFMT_LEFT,80);
	m_wndConnectionList.InsertColumn(nCol ++,_T("解码器"),LVCFMT_LEFT,100);
	//m_wndConnectionList.InsertColumn(nCol ++,_T("录像文"),LVCFMT_LEFT,385);	

	m_nVideoWndID[0] = IDC_STATIC_VIDEO;
	m_nVideoWndID[1] = IDC_STATIC_VIDEO2;
	m_nVideoWndID[2] = IDC_STATIC_VIDEO3;
	m_nVideoWndID[3] = IDC_STATIC_VIDEO4;
	for (int i = 0;i < 4;i ++)
	{
		m_wndVideo[i].SubclassDlgItem(m_nVideoWndID[i],this);
	}
	CRect rtList;
	GetDlgItemRect(IDC_LIST_CONNECTION,rtList);
	m_nListWidth = rtList.Width();
	m_nListTop = rtList.top;

	LoadSetting();
	m_pSelectedPen = new CPen(PS_SOLID, 2, RGB(0, 255, 0));  
	m_pUnSelectedPen = new CPen(PS_SOLID, 2, RGB(255, 0, 0));  
	for (int i = 0;i < 4;i ++)
		m_pFramePen[i] = m_pUnSelectedPen;
	
	InitHighPerformanceClock();
	SendDlgItemMessage(IDC_COMBO_STREAM,CB_SETCURSEL,0,0);
	SetDlgItemText(IDC_EDIT_ACCOUNT,_T("admin"));
	SetDlgItemText(IDC_EDIT_PASSWORD,_T("admin"));

	m_nHotkeyID = (WORD)GlobalAddAtom(_T("{A1C54F9F-A835-4fca-88DA-BF0C0DA0E6C5}"));
	printf(_T("m_nHotkeyID = %d\n"),m_nHotkeyID);
	if (m_nHotkeyID)
	{
		RegisterHotKey(m_hWnd,m_nHotkeyID,MOD_CONTROL,'1');
		RegisterHotKey(m_hWnd,m_nHotkeyID,MOD_CONTROL,'2');
		RegisterHotKey(m_hWnd,m_nHotkeyID,MOD_CONTROL,'3');
	}

	//OnDecoderSel(ID_DECODER_HACCEL);
	OnDecoderSel(ID_DECODER_FFMPEG);

	av_register_all();

	// 调整窗口尺寸，以便于显示
	CRect rtWnd;
	GetWindowRect(&rtWnd);
	CRect rtSlieder;
	GetDlgItemRect(IDC_SLIDER_GRAPHICQULITY,rtSlieder);
	rtWnd.right -= (rtSlieder.Width() + 3);

	int nVideoWndWidth = 480;
	int nVideoWndHeight = 270;
	CRect rtVideo;	

	rtVideo.left = 2;
	rtVideo.right = rtVideo.left + nVideoWndWidth;
	rtVideo.top = rtList.bottom + _FrameInterval;
	rtVideo.bottom = rtVideo.top + nVideoWndHeight;
	MoveDlgItem(IDC_STATIC_VIDEO,rtVideo);

	rtVideo.left = rtVideo.right + _FrameInterval;
	rtVideo.right = rtVideo.left + nVideoWndWidth;
	MoveDlgItem(IDC_STATIC_VIDEO2,rtVideo);

	rtVideo.top = rtVideo.bottom + _FrameInterval;
	rtVideo.bottom = rtVideo.top + nVideoWndHeight;
	MoveDlgItem(IDC_STATIC_VIDEO4,rtVideo);

	rtVideo.right = rtVideo.left - _FrameInterval;
	rtVideo.left = rtVideo.right - nVideoWndWidth;
	MoveDlgItem(IDC_STATIC_VIDEO3,rtVideo);

	int nWndWidth =  rtVideo.Width()*2 + _FrameInterval*2 + rtSlieder.Width() + 15;
	int nWndHeight = rtList.bottom + rtVideo.Height()*2 + _FrameInterval*2 + 2 + 18;
	rtWnd.right = rtWnd.left + nWndWidth;
	rtWnd.bottom = rtWnd.top + nWndHeight;
	MoveWindow(&rtWnd);
	m_wndSliderCtrl.SubclassDlgItem(IDC_SLIDER_GRAPHICQULITY,this);
	m_wndSliderCtrl.SetRange(0,11);
	m_wndSliderCtrl.SetPos(3);
		
	// 初始化全屏参数
	CVideoPlayer::InitFullScreenInfo();
	m_nMonitorCount = GetMonitorCount();
// 	int nMonitorIndex = 0;
// 	if (m_nMonitorCount > 0)
// 	{
// 		for (int i = 0;i < m_nMonitorCount;i ++)
// 		{
// 			boost::shared_ptr<CFullScreenVideoDlg>pDialog = boost::make_shared<CFullScreenVideoDlg>(/*IDD_DIALOG_VIDEO*/);
// 			if (pDialog->Create(IDD_DIALOG_VIDEO))
// 			{
// 				//::SetParent(pDialog->GetSafeHwnd(),::GetDesktopWindow());
// 				POINT pt;
// 				pt.x = 0;
// 				pt.y = 0;
// 				MoveWnd2Monitor(pDialog->m_hWnd,nMonitorIndex++,pt);
// 				m_vFullScreenWnd.push_back(pDialog);
// 			}
// 			else
// 			{
// 				AfxMessageBox(_T("创建全屏窗口失败."));
// 				return TRUE;
// 			}
// 		}
// 	}
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDVOIPCSDKDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDVOIPCSDKDemoDlg::OnPaint()
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
		//if (!m_bDecodeThreadRun)
		{
			CPaintDC dc(this);
			CRect rtVideo; 	
 			
 			CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)); 
// 			//把画刷加入设置描述表.  
			dc.SelectObject(m_pUnSelectedPen);
			for (int i = 0;i < 4;i ++)
			{
				if (i == m_nLastSelected)
					continue;
				GetDlgItemRect(m_nVideoWndID[i],rtVideo);	
				CRect rtOld = rtVideo;
				rtVideo.InflateRect(1,1,1,1);
				dc.Rectangle(rtVideo);				
				dc.FillSolidRect(rtOld,RGB(0,0,0));
			}

			GetDlgItemRect(m_nVideoWndID[m_nLastSelected],rtVideo);
			CRect rtOld = rtVideo;
			dc.SelectObject(m_pSelectedPen);			
			rtVideo.InflateRect(1,1,1,1);
			dc.Rectangle(rtVideo);
			dc.FillSolidRect(rtOld,RGB(0,0,0));
		}
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDVOIPCSDKDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CDVOIPCSDKDemoDlg::LoadSetting()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetAppPath(szPath,MAX_PATH);
	_tcscat_s(szPath,MAX_PATH,_T("\\CameraSetting.xml"));
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
			
			while(xml.FindChildElem(_T("Camera")))
			{
				xml.IntoElem();
				ComboBox_AddString(hCombox,xml.GetAttrib(_T("IP")));				
				xml.OutOfElem();
			}
			xml.OutOfElem();
		}
		else
			return false;
	}
	int nIndex =  ComboBox_FindString(hCombox,0,strCursel);
	if (nIndex == CB_ERR)
		ComboBox_SetCurSel(hCombox,0);
	else
		ComboBox_SetCurSel(hCombox,nIndex);
	HWND hComboStreamType = ::GetDlgItem(m_hWnd,IDC_COMBO_STREAMTYPE);
	nIndex = ComboBox_FindString(hComboStreamType,0,(LPCTSTR)strStreamType);
	ComboBox_SetCurSel(hComboStreamType,nIndex);

	HWND hComboStrem = ::GetDlgItem(m_hWnd,IDC_COMBO_STREAM);
	nIndex = ComboBox_FindString(hComboStrem,0,(LPCTSTR)strStream);
	ComboBox_SetCurSel(hComboStrem,nIndex);
	return true;
}

bool CDVOIPCSDKDemoDlg::SaveSetting()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetAppPath(szPath,MAX_PATH);
	_tcscat_s(szPath,MAX_PATH,_T("\\CameraSetting.xml"));
	
	HWND hComboBox = GetDlgItem(IDC_IPADDRESS)->m_hWnd;
	TCHAR szStreamType[16] = {0};
	TCHAR szText[32] = {0};
	TCHAR szStream[16] = {0};
	GetDlgItemText(IDC_IPADDRESS,szText,32);
	GetDlgItemText(IDC_COMBO_STREAMTYPE,szStreamType,16);
	GetDlgItemText(IDC_COMBO_STREAM,szStream,16);
	CMarkup xml;
	TCHAR *szDoc = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.SetDoc(szDoc);

	xml.AddElem(_T("Configuration"));
	xml.AddChildElem(_T("CurSel"));
	xml.IntoElem();
	xml.AddAttrib(_T("IP"),szText);
	xml.AddAttrib(_T("Stream"),szStream);	
	xml.AddAttrib(_T("StreamType"),szStreamType);
	xml.OutOfElem();

	xml.IntoElem();	  // Configuration
	xml.AddElem(_T("CameraList"));
	int nCount = ComboBox_GetCount(hComboBox);
	for (int i = 0;i < nCount;i ++)
	{
		xml.AddChildElem(_T("Camera"));
		ComboBox_GetLBText(hComboBox,i,szText);
		xml.IntoElem();			
		xml.AddAttrib(_T("IP"),szText);		
		xml.OutOfElem();
	}
	xml.OutOfElem(); // Configuration

	xml.Save(szPath);
	return true;

}
void CDVOIPCSDKDemoDlg::OnNMClickListConnection(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_nCurSelect = pNMItemActivate->iItem;
	bool bEnable = false;
	m_pCurSeclectDecoder = NULL;
	if (m_nCurSelect != -1)
	{
		bEnable = true;
		m_pCurSeclectDecoder = (CVideoPlayer *)m_wndConnectionList.GetItemData(m_nCurSelect);
		if (m_pCurSeclectDecoder->m_pDxSurface)		
			SetDlgItemText(IDC_BUTTON_PLAYSTREAM,_T("停止播放"));		
		else		
			SetDlgItemText(IDC_BUTTON_PLAYSTREAM,_T("播放码流"));

		if (m_pCurSeclectDecoder->m_pRecFile)
			SetDlgItemText(IDC_BUTTON_RECORD,_T("停止录像"));
		else
			SetDlgItemText(IDC_BUTTON_RECORD,_T("开始录像"));
	}	
	EnableDlgItem(IDC_BUTTON_DISCONNECT,bEnable);
	EnableDlgItem(IDC_BUTTON_PLAYSTREAM,bEnable);	
	EnableDlgItem(IDC_BUTTON_RECORD,bEnable);
	*pResult = 0;
}
void CDVOIPCSDKDemoDlg::OnBnClickedButtonConnect()
{
	TCHAR szAccount[32] = {0};
	TCHAR szPassowd[32] = {0};
	TCHAR szIPAddress[32] = {0};
	
	GetDlgItemText(IDC_EDIT_ACCOUNT,szAccount,32);
	GetDlgItemText(IDC_EDIT_ACCOUNT,szPassowd,32);
	GetDlgItemText(IDC_IPADDRESS,szIPAddress,32);
	int nStream = SendDlgItemMessage(IDC_COMBO_STREAM,CB_GETCURSEL);
	int nStreamType = SendDlgItemMessage(IDC_COMBO_STREAMTYPE,CB_GETCURSEL);
	int nError = 0;
	app_net_tcp_sys_logo_info_t LoginInfo;
	USER_HANDLE hUser = DVO_NET_Login(szIPAddress,6001,szAccount,szPassowd,&LoginInfo,&nError,5000);
	if (hUser != -1)
	{		
		if (ComboBox_FindString(::GetDlgItem(m_hWnd,IDC_IPADDRESS),0,szIPAddress) == CB_ERR)
			ComboBox_AddString(::GetDlgItem(m_hWnd,IDC_IPADDRESS),szIPAddress);
		std::tr1::shared_ptr<CVideoPlayer> pDecoder = std::tr1::shared_ptr<CVideoPlayer>(new CVideoPlayer(hUser,(TCHAR *)szIPAddress));
		int nSize = m_listDecode.size();
		TCHAR szNo[16] = {0};
		_stprintf_s(szNo,16,_T("%d"),nSize + 1);
		pDecoder->m_nItem = nSize;
		m_wndConnectionList.InsertItem(nSize,szNo);
		TCHAR szItemText[32] = {0};
		
		m_wndConnectionList.SetItemText(nSize,SubItem_CameraIP,szIPAddress);

		_stprintf_s(szItemText,32,_T("%s"),(DVO_CODEC)nStreamType == DVO_CODEC_H264?_T("H.264"):_T("H.265"));
		m_wndConnectionList.SetItemText(nSize,SubItem_CodecType,szItemText);

		_stprintf_s(szItemText,32,_T("%d"),nStream);
		m_wndConnectionList.SetItemText(nSize,SubItem_StreamNo,szItemText);
		m_wndConnectionList.SetItemData(nSize,(DWORD_PTR)pDecoder.get());
		m_listDecode.push_back(pDecoder);
		pDecoder->StartReceive(m_hWnd,(DVO_CODEC)nStreamType,nStream);
		SaveSetting();
	}
}

void CDVOIPCSDKDemoDlg::OnBnClickedButtonDisconnect()
{
	if (m_pCurSeclectDecoder)
	{
		int nSize = m_listDecode.size();
		if (nSize <= 0 )
		{
			AfxMessageBox(_T("没有可以注销的连接."));
			return;
		}

		list<shared_ptr<CVideoPlayer>>::iterator it;
		int nIndex = 0;
		for (it = m_listDecode.begin();it != m_listDecode.end();it ++)
		{
			if((*it).get() == m_pCurSeclectDecoder)
			{
				// 删除连接，将引发CVideoDecoder析构
				int nID = ::GetDlgCtrlID(m_pCurSeclectDecoder->m_hWndView);				
				it = m_listDecode.erase(it);
				// 断开后画面黑屏
				for (int i = 0;i < _MAX_VIDEOWND;i ++)
				{
					if (nID == m_nVideoWndID[i])
					{
						m_pViewWndState[i] = NULL;
						CDC *pDC = GetDC();
						CRect rt;
						GetDlgItemRect(nID,rt);
						pDC->FillSolidRect(rt,RGB(0,0,0));
						break;
					}
				}
				EnableDlgItem(IDC_BUTTON_DISCONNECT,false);
				EnableDlgItem(IDC_BUTTON_PLAYSTREAM,false);	
				m_wndConnectionList.DeleteItem(m_nCurSelect);
				m_nCurSelect = -1;		
				m_pCurSeclectDecoder = NULL;
				break;
			}
		}
	}
}


void CDVOIPCSDKDemoDlg::OnBnClickedButtonPlaystream()
{	
	if (m_pCurSeclectDecoder)
	{
		if (m_pCurSeclectDecoder->m_nCodecType == DVO_CODEC_H264 && 
			m_nManufacturer == HISILICON)
		{
			AfxMessageBox(_T("暂未集成海思H.264解码器,请选择其它解码器."),MB_ICONSTOP);
			return;
		}
		CWaitCursor Wait;
		if (!m_pCurSeclectDecoder->m_pDxSurface)
		{
			if (m_nLastSelected >=0 && m_nLastSelected <= (_MAX_VIDEOWND -1))
			{
				// 查找空闲的窗格	
				int nWndUnused = -1;			
				if (m_pViewWndState[m_nLastSelected])
				{					
					for (int i = 0;i < _MAX_VIDEOWND;i ++)
					{
						if (!m_pViewWndState[i])
						{
							nWndUnused = i;
							break;
						}
					}
					if (nWndUnused == -1)
					{
						AfxMessageBox(_T("所有监视窗口已经在播放码流."));
						return;
					}
				}
				else
					nWndUnused = m_nLastSelected;

				UINT nWndID = m_nVideoWndID[nWndUnused] ;
				HWND hViewWnd = GetDlgItem(nWndID)->GetSafeHwnd();
				if (hViewWnd)
				{
					SelectVideoWnd(nWndUnused,m_nLastSelected);
					m_pViewWndState[nWndUnused] = m_pCurSeclectDecoder;	
					m_nLastSelected = nWndUnused;
					// 默认使用第一显示器显示全屏图像
					PlayContext *pc = new PlayContext;
					pc->hVideoWnd		 = hViewWnd;
					pc->nMaxVideoWidth	 = _Max_Image_Width;
					pc->nMaxVideoHeight	 = _Max_Image_Height;
					pc->nCodecType		 = (DVO_CODEC)ComboBox_GetCurSel(::GetDlgItem(m_hWnd,IDC_COMBO_STREAMTYPE));
					pc->nWndWidth		 = _ViewVideoWidth;
					pc->nWndHeight		 = _ViewVideoHeight;
					m_pCurSeclectDecoder->m_nMonitorIndex = 0;	// 默认使用第一个显示器
					m_pCurSeclectDecoder->StartPlayStream(pc,m_nManufacturer);
				}
			}
		}
		else
		{
			m_pCurSeclectDecoder->StopPlayStream();
			CRect rtVideo;
			::GetWindowRect(m_pCurSeclectDecoder->m_hWndView,&rtVideo);
			ScreenToClient(&rtVideo);
			CDC *pDC = GetDC();
			pDC->FillSolidRect(rtVideo,RGB(0,0,0));
			m_pViewWndState[m_nLastSelected] = NULL;
		}
	}	
}

void CDVOIPCSDKDemoDlg::OnBnClickedButtonRecord()
{
	int nStream = SendDlgItemMessage(IDC_COMBO_STREAM,CB_GETCURSEL);
	int nStreamType = SendDlgItemMessage(IDC_COMBO_STREAMTYPE,CB_GETCURSEL);
	if (m_pCurSeclectDecoder)
	{
		CWaitCursor Wait;
		if (!m_pCurSeclectDecoder->m_pRecFile)
		{
			TCHAR szFileName[MAX_PATH] = {0};
			TCHAR szPath[MAX_PATH] = {0};

			TCHAR szFileDir[MAX_PATH] = {0};
			if (!PathFileExists(m_szRecordPath))
				GetAppPath(szFileDir,MAX_PATH);
			else
				_tcscpy_s(szFileDir,MAX_PATH,m_szRecordPath);

			CTime tNow = CTime::GetCurrentTime();
			_stprintf_s(szFileName,
				_T("%s\\DVO_%s_CH%d_%s.%s"),
						szFileDir,
						m_pCurSeclectDecoder->m_szIpAddress,
						nStream,
						tNow.Format(_T("%y%m%d%H%M%S")),
						nStreamType==0?_T("H264"):_T("h265"));
			m_pCurSeclectDecoder->StartRecord(szFileName);
		}
		else
		{
			m_pCurSeclectDecoder->StopRecord();	
		}
	}	

}

void CDVOIPCSDKDemoDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_listDecode.clear();

	if (m_pSelectedPen )
		delete m_pSelectedPen;
	if (m_pUnSelectedPen)
		delete m_pUnSelectedPen;
}

#define _DistanceVideoBorder	66
void CDVOIPCSDKDemoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd())
	{
		CRect rtList;
		GetDlgItemRect(IDC_LIST_CONNECTION,rtList);
		CRect rtSlider;
		GetDlgItemRect(IDC_SLIDER_GRAPHICQULITY,rtSlider);
		rtList.left = 2;
		rtList.right = cx - 2 - (rtSlider.Width() + 6);
		MoveDlgItem(IDC_LIST_CONNECTION,rtList);

		CRect rtClient;
		GetClientRect(&rtClient);
		
		rtClient.right -= (rtSlider.Width() + 3);

		CRect rtVideo;
		int nFrameWidth = (cx- 6 /*- (rtSlider.Width() + 6)*/)/2;
		int nFrameHeight = (rtClient.bottom - rtList.bottom - _FrameInterval*2 - 2)/2;

		rtVideo.left = 2;
		rtVideo.right = rtVideo.left + nFrameWidth;
		rtVideo.top = rtList.bottom + _FrameInterval;
		rtVideo.bottom = rtVideo.top + nFrameHeight;
		GetDlgItem(IDC_STATIC_VIDEO)->MoveWindow(&rtVideo);

		rtVideo.left = rtVideo.right + _FrameInterval ;
		rtVideo.right = rtVideo.left + 	nFrameWidth;	
		GetDlgItem(IDC_STATIC_VIDEO2)->MoveWindow(&rtVideo);
	
		rtVideo.bottom = cy - 2;
		rtVideo.top = rtVideo.bottom - nFrameHeight;
		GetDlgItem(IDC_STATIC_VIDEO4)->MoveWindow(&rtVideo);

		rtVideo.left = 2;
		rtVideo.right = rtVideo.left + nFrameWidth;			
		GetDlgItem(IDC_STATIC_VIDEO3)->MoveWindow(&rtVideo);

		Invalidate();
	}
}


LRESULT CDVOIPCSDKDemoDlg::OnUpdateDecoderInfo(WPARAM w,LPARAM l)
{
	TCHAR szText[64] = {0};
	CVideoPlayer *pDecoder = (CVideoPlayer *)w;
	int nItem = -1;
	int nItemCount = m_wndConnectionList.GetItemCount();
	for (int i = 0;i < nItemCount;i ++)
	{
		if (m_wndConnectionList.GetItemData(i) == (DWORD_PTR)pDecoder)
		{
			nItem = i;
			break;
		}
	}
	if (nItem != -1)
	{
		int nStreamRate = pDecoder->m_pStreamInfo->GetVideoCodeRate();
		_stprintf_s(szText,64,_T("%d Kbps"),nStreamRate);		
		m_wndConnectionList.SetItemText(nItem,SubItem_StreamRate,szText);// 码率

		_stprintf_s(szText,64,_T("%d fps"),pDecoder->m_pStreamInfo->GetFrameRate());		
		m_wndConnectionList.SetItemText(nItem,SubItem_FrameRate ,szText);// 帧率

		_stprintf_s(szText,64,_T("%d"),pDecoder->m_nFraceCache);
		m_wndConnectionList.SetItemText(nItem,SubItem_FrameCache,szText);	// 帧缓存

		_stprintf_s(szText,64,_T("%d KB"),(pDecoder->m_pStreamInfo->nVideoBytes + pDecoder->m_pStreamInfo->nAudioBytes)/1024);
		m_wndConnectionList.SetItemText(nItem,SubItem_TotalStream,szText);	// 收到总码流长度

		if (pDecoder->m_pStreamInfo->tFirstTime > 0)
		{
			time_t nRecTime = time(0)*1000 - pDecoder->m_pStreamInfo->tFirstTime;
			CTimeSpan tSpan(nRecTime/1000);
			_stprintf_s(szText,64,_T("%02d:%02d:%02d"),tSpan.GetHours(),tSpan.GetMinutes(),tSpan.GetSeconds());
		}
		else
			_tcscpy_s(szText,64,_T("00:00:00"));	
		m_wndConnectionList.SetItemText(nItem,SubItem_ConnectTime,szText);		// 连接时长

		if (pDecoder->m_tRecStartTime > 0)
		{
			time_t nRecTime = time(0) - pDecoder->m_tRecStartTime;
			CTimeSpan tSpan(nRecTime);
			_stprintf_s(szText,64,_T("%02d:%02d:%02d"),tSpan.GetHours(),tSpan.GetMinutes(),tSpan.GetSeconds());
		}
		else
			_tcscpy_s(szText,64,_T("00:00:00"));	
		m_wndConnectionList.SetItemText(nItem,SubItem_RecTime,szText);		// 录像时长

		_stprintf_s(szText,_T("%d KB"),pDecoder->m_nRecFileLength/1024);
		m_wndConnectionList.SetItemText(nItem,SubItem_FileLength,szText);
		
		TCHAR *szDecoder[] = {_T("UnKnown"),_T("HISILICON"),_T("FFMPEG"),_T("DVO_HACCEL")};
		
		m_wndConnectionList.SetItemText(nItem,SubItem_DecoderManufacturer,szDecoder[pDecoder->m_nDecodecManufacturer]);
	}

	return 0;
}

void CDVOIPCSDKDemoDlg::VideoFitWindow(HWND hVideoWnd,int nVideoWidth,int nVideoHeight,int &nImageWidth,int &nImageHeight)
{
	CRect rtWnd;
	::GetWindowRect(hVideoWnd,&rtWnd);
	int nWndWidth = rtWnd.Width();
	int nWndHeight = rtWnd.Height();

	float dfScaleWnd = (float)nWndWidth/nWndHeight;			// 视频窗口宽高比
	float dfScaleVideo = (float)nVideoWidth/nVideoHeight;		// 视频宽高比
	if (dfScaleVideo > dfScaleWnd)
	{
		nImageWidth = nWndWidth;
		nImageHeight = nWndWidth*nVideoHeight/nVideoWidth;
	}
	else if (dfScaleVideo < dfScaleWnd)
	{
		nImageHeight = nWndHeight;
		nImageWidth = nWndHeight*nVideoWidth/nVideoHeight;
	}
	else
	{
		nImageWidth = nWndWidth;
		nImageHeight = nWndHeight;
	}
	// 调整高度比例，以保护图像高度的不失调
	while ((float)nImageHeight /nImageWidth < (float)nVideoHeight/nVideoWidth)
		nImageHeight +=1;

}

void CDVOIPCSDKDemoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rt;  
	CDC *pDC = GetDC();
	int nCurSel = 0;

	CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));  
	//把画刷加入设置描述表.  
	pDC->SelectObject(pBrush);  
	for (int i = 0;i < _MAX_VIDEOWND;i ++)
	{
		GetDlgItemRect(m_nVideoWndID[i],rt);		
		if (rt.PtInRect(point))
		{
			nCurSel = i;
			
			break;
		}
	}
	
// 	HWND hVideoWnd1 = ::WindowFromPoint((POINT)point);
// 	long nWndProc1 = GetWindowLong(hVideoWnd1,GWL_WNDPROC);
// 	TraceMsgA("WndProc = %08X.\n",nWndProc1);
	HWND hVideoWnd2 = ::GetDlgItem(m_hWnd,IDC_STATIC_VIDEO);
	WNDPROC nWndProc2 = (WNDPROC)GetWindowLong(hVideoWnd2,GWL_WNDPROC);	
	TraceMsgA("WndProc = %08X.\n",nWndProc2);
	
	SelectVideoWnd(nCurSel,m_nLastSelected);
	m_nLastSelected =  nCurSel;	
	if (nCurSel >=0 && nCurSel < _MAX_VIDEOWND)
	{
		if (m_pViewWndState[nCurSel])
		{
			EnableDlgItem(IDC_BUTTON_PLAYSTREAM,TRUE);
			EnableDlgItem(IDC_BUTTON_RECORD,TRUE);
			EnableDlgItem(IDC_BUTTON_DISCONNECT,TRUE);
			m_pCurSeclectDecoder = (CVideoPlayer *)m_pViewWndState[nCurSel];
			if (m_pCurSeclectDecoder->m_pDxSurface)
				SetDlgItemText(IDC_BUTTON_PLAYSTREAM,_T("停止播放"));
			else
				SetDlgItemText(IDC_BUTTON_PLAYSTREAM,_T("播放码流"));

			if (m_pCurSeclectDecoder->m_pRecFile)
				SetDlgItemText(IDC_BUTTON_RECORD,_T("停止录像"));
			else
				SetDlgItemText(IDC_BUTTON_RECORD,_T("开始录像"));
		}
		else
		{
			EnableDlgItem(IDC_BUTTON_PLAYSTREAM,FALSE);
			EnableDlgItem(IDC_BUTTON_RECORD,FALSE);
			EnableDlgItem(IDC_BUTTON_DISCONNECT,FALSE);
		}
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void MyRectangle(CDC *pDC,CRect rt)
{
	pDC->MoveTo(rt.TopLeft());
	pDC->LineTo(rt.right,rt.top);
	pDC->LineTo(rt.right,rt.bottom);
	pDC->LineTo(rt.left,rt.bottom);
	pDC->LineTo(rt.left,rt.top);
	
};
void CDVOIPCSDKDemoDlg::SelectVideoWnd(int nWndNewSelected,int nWndOldSelected)
{
	if ((nWndNewSelected >= 0 && nWndNewSelected < _MAX_VIDEOWND) &&
		(nWndOldSelected >= 0 && nWndOldSelected < _MAX_VIDEOWND))
	{
		CRect rt;  
		CDC *pDC = GetDC();
		// 恢复上次选择的矩形为未选择状态
		GetDlgItemRect(m_nVideoWndID[nWndOldSelected],rt);
		CRect rtOld = rt;
		rt.InflateRect(1,1,1,1);
		pDC->SelectObject(m_pUnSelectedPen);
		MyRectangle(pDC,rt);
		//pDC->Rectangle(rt);	
		if (!m_pViewWndState[nWndOldSelected])
			pDC->FillSolidRect(rtOld,RGB(0,0,0));
		// 设置当前选的的矩形为选择状态
		GetDlgItemRect(m_nVideoWndID[nWndNewSelected],rt);
		rtOld = rt;
		rt.InflateRect(1,1,1,1);
		pDC->SelectObject(m_pSelectedPen);
		//pDC->Rectangle(rt);	
		MyRectangle(pDC,rt);
		if (!m_pViewWndState[nWndNewSelected])
			pDC->FillSolidRect(rtOld,RGB(0,0,0));
	}
}

void CDVOIPCSDKDemoDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect rt;  
	int nCurSel = 0;
	
	for (int i = 0;i <_MAX_VIDEOWND;i ++)
	{
		GetDlgItemRect(m_nVideoWndID[i],rt);
		if (rt.PtInRect(point))
		{
			nCurSel = i;
			break;
		}
	}
	SelectVideoWnd(nCurSel,m_nLastSelected);
	m_nLastSelected =  nCurSel;	
	if (nCurSel >=0 && nCurSel < _MAX_VIDEOWND)
	{
		if (m_pViewWndState[nCurSel])
		{
			EnableDlgItem(IDC_BUTTON_PLAYSTREAM,TRUE);
			EnableDlgItem(IDC_BUTTON_RECORD,TRUE);
			EnableDlgItem(IDC_BUTTON_DISCONNECT,TRUE);
			m_pCurSeclectDecoder = (CVideoPlayer *)m_pViewWndState[nCurSel];
			if (m_pCurSeclectDecoder->m_pDxSurface)
			{
				SetDlgItemText(IDC_BUTTON_PLAYSTREAM,_T("停止播放"));
				m_wndVideo[nCurSel].SendMessage(WM_LBUTTONDBLCLK,100,100);
				if (!m_pCurSeclectDecoder->m_hFullScreen)
					CVideoPlayer::SetFullScreenInfo(m_pCurSeclectDecoder);	
			}
			else
				SetDlgItemText(IDC_BUTTON_PLAYSTREAM,_T("播放码流"));

			if (m_pCurSeclectDecoder->m_pRecFile)
				SetDlgItemText(IDC_BUTTON_RECORD,_T("停止录像"));
			else
				SetDlgItemText(IDC_BUTTON_RECORD,_T("开始录像"));	
		}
		else
		{
			EnableDlgItem(IDC_BUTTON_PLAYSTREAM,FALSE);
			EnableDlgItem(IDC_BUTTON_RECORD,FALSE);
			EnableDlgItem(IDC_BUTTON_DISCONNECT,FALSE);
		}
	}
	CDialog::OnLButtonDblClk(nFlags, point);
}


void CDVOIPCSDKDemoDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	printf(_T("nHotkeyID = %d\r\n"),nHotKeyId);
	switch(nKey2)
	{
	case  '1':
		{
			if (m_pCurSeclectDecoder)
			{
				m_pCurSeclectDecoder->m_nMonitorIndex = 0;
				//m_pCurSeclectDecoder->m_hFullScreen = m_vFullScreenWnd[0]->GetSafeHwnd();
			}
		}
		break;
	case  '2':
		{
			if (m_pCurSeclectDecoder)
			{
				if (m_nMonitorCount > 1)
				{
					//m_pCurSeclectDecoder->m_hFullScreen = m_vFullScreenWnd[1]->GetSafeHwnd();
					m_pCurSeclectDecoder->m_nMonitorIndex = 1;
				}
			}
		}
		break;
	case  '3':
		{
			if (m_pCurSeclectDecoder)
			{
				if (m_nMonitorCount > 2)
					m_pCurSeclectDecoder->m_nMonitorIndex = 2;
					//m_pCurSeclectDecoder->m_hFullScreen = m_vFullScreenWnd[2]->GetSafeHwnd();
			}
		}
		break;
	default:
		break;
	}
	
	CDialog::OnHotKey(nHotKeyId, nKey1, nKey2);
}


void CDVOIPCSDKDemoDlg::OnMenuRecord()
{
	if (m_listDecode.size() > 0)
	{
		TCHAR szFileName[MAX_PATH] = {0};
		TCHAR szPath[MAX_PATH] = {0};
		list<shared_ptr<CVideoPlayer>>::iterator it;
		int nSquence = 0;
		for (it = m_listDecode.begin();it != m_listDecode.end(); it ++)
		{
			if (!(*it)->m_pRecFile)
			{
				TCHAR szFileDir[MAX_PATH] = {0};
				if (!PathFileExists(m_szRecordPath))
					GetAppPath(szFileDir,MAX_PATH);
				else
					_tcscpy_s(szFileDir,MAX_PATH,m_szRecordPath);
				CTime tNow = CTime::GetCurrentTime();
				_stprintf_s(szFileName,
					_T("D:\\%s\\DVO_%s_CH%d%d_%s.%s"),
					szFileDir,
					(*it)->m_szIpAddress,
					(*it)->m_nStreamNo,
					nSquence,
					tNow.Format(_T("%y%m%d%H%M%S")),
					(*it)->m_nCodecType==DVO_CODEC_H264?_T("H264"):_T("h265"));
				(*it)->StartRecord(szFileName);
				nSquence ++;
			}
		}
	}
}

void CDVOIPCSDKDemoDlg::OnFileStoprecord()
{
	if (m_listDecode.size() > 0)
	{
		TCHAR szFileName[MAX_PATH] = {0};
		TCHAR szPath[MAX_PATH] = {0};
		list<std::tr1::shared_ptr<CVideoPlayer>>::iterator it;
		for (it = m_listDecode.begin();it != m_listDecode.end(); it ++)
		{
			if ((*it)->m_pRecFile)
			{
				(*it)->StopRecord();
			}
		}
	}
}

BOOL CDVOIPCSDKDemoDlg::ConsoleHandler(DWORD dwMsgType)
{
	static TCHAR *szEvent[] = { "CTRL_C_EVENT",
		"CTRL_BREAK_EVENT",
		"CTRL_CLOSE_EVENT",
		"NULL",
		"NULL",
		"CTRL_LOGOFF_EVENT",
		"CTRL_SHUTDOWN_EVENT"};
	if (dwMsgType >= CTRL_C_EVENT && dwMsgType <= CTRL_SHUTDOWN_EVENT)
		TraceMsgA("EventID = %s.\n",szEvent[dwMsgType]);
	else
		TraceMsgA("EventID = Unknown Event.\n");
	switch(dwMsgType)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
		{
			FreeConsole();
			g_hStdin = NULL;
			g_hStdout = NULL;
			return TRUE;
		}
		break;
		// 3 is reserved!
		// 4 is reserved!
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		{
			FreeConsole();
			g_hStdin = NULL;
			g_hStdout = NULL;
		}
		break;
	} 
	
	return TRUE;
}

void CDVOIPCSDKDemoDlg::OnFileOutput()
{
	CMenu *pMenu = GetMenu()->GetSubMenu(0);
	if (g_hStdin || g_hStdout)
	{		
		fclose(stdin);  
		fclose(stdout); 
		FreeConsole();  
		g_hStdin = NULL;
		g_hStdout = NULL;
		pMenu->CheckMenuItem(ID_FILE_OUTPUT,MF_BYCOMMAND|MF_UNCHECKED);
		return;
	}
	AllocConsole();  
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);	// 设置控制台窗口消息接入口，以安全关闭控制台
	freopen("conin$","r+t",stdin);  
	freopen("conout$","w+t",stdout);  
	g_hStdin  = GetStdHandle(STD_INPUT_HANDLE);  
	g_hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	pMenu->CheckMenuItem(ID_FILE_OUTPUT,MF_BYCOMMAND|MF_CHECKED);
}

void CDVOIPCSDKDemoDlg::OnFileLoginRec()
{
	TCHAR szAccount[32] = {0};
	TCHAR szPassowd[32] = {0};
	TCHAR szIPAddress[32] = {0};

	GetDlgItemText(IDC_EDIT_ACCOUNT,szAccount,32);
	GetDlgItemText(IDC_EDIT_ACCOUNT,szPassowd,32);
	GetDlgItemText(IDC_IPADDRESS,szIPAddress,32);
	int nStream = SendDlgItemMessage(IDC_COMBO_STREAM,CB_GETCURSEL);
	int nStreamType = SendDlgItemMessage(IDC_COMBO_STREAMTYPE,CB_GETCURSEL);
	int nError = 0;
	app_net_tcp_sys_logo_info_t LoginInfo;
	USER_HANDLE hUser = DVO_NET_Login(szIPAddress,6001,szAccount,szPassowd,&LoginInfo,&nError,5000);
	if (hUser != -1)
	{		
		if (ComboBox_FindString(::GetDlgItem(m_hWnd,IDC_IPADDRESS),0,szIPAddress) == CB_ERR)
			ComboBox_AddString(::GetDlgItem(m_hWnd,IDC_IPADDRESS),szIPAddress);
		std::tr1::shared_ptr<CVideoPlayer> pDecoder = std::tr1::shared_ptr<CVideoPlayer>(new CVideoPlayer(hUser,(TCHAR *)szIPAddress));
		int nSize = m_listDecode.size();
		TCHAR szNo[16] = {0};
		_stprintf_s(szNo,16,_T("%d"),nSize + 1);
		pDecoder->m_nItem = nSize;
		m_wndConnectionList.InsertItem(nSize,szNo);
		TCHAR szItemText[32] = {0};

		m_wndConnectionList.SetItemText(nSize,SubItem_CameraIP,szIPAddress);

		_stprintf_s(szItemText,32,_T("%s"),(DVO_CODEC)nStreamType == DVO_CODEC_H264?_T("H.264"):_T("H.265"));
		m_wndConnectionList.SetItemText(nSize,SubItem_CodecType,szItemText);

		_stprintf_s(szItemText,32,_T("%d"),nStream);
		m_wndConnectionList.SetItemText(nSize,SubItem_StreamNo,szItemText);
		m_wndConnectionList.SetItemData(nSize,(DWORD_PTR)pDecoder.get());
		m_listDecode.push_back(pDecoder);
		pDecoder->StartReceive(m_hWnd,(DVO_CODEC)nStreamType,nStream);

		TCHAR szFileName[MAX_PATH] = {0};
		TCHAR szPath[MAX_PATH] = {0};
		TCHAR szFileDir[MAX_PATH] = {0};
		if (!PathFileExists(m_szRecordPath))
			GetAppPath(szFileDir,MAX_PATH);
		else
			_tcscpy_s(szFileDir,MAX_PATH,m_szRecordPath);

		CTime tNow = CTime::GetCurrentTime();
		_stprintf_s(szFileName,
			_T("D:\\%s\\DVO_%s_CH%d_%s.%s"),
			szFileDir,
			pDecoder->m_szIpAddress,
			nStream,
			tNow.Format(_T("%y%m%d%H%M%S")),
			nStreamType==0?_T("H264"):_T("h265"));
		pDecoder->StartRecord(szFileName);
		SaveSetting();
	}
}

void CDVOIPCSDKDemoDlg::OnDecoderSel(UINT nID)
{
	m_nManufacturer = (Decoder_Manufacturer)(nID - ID_DECODER_HISILICON + 1);
	CMenu *pMenu = GetMenu()->GetSubMenu(1);
	for (UINT nMenuID = ID_DECODER_HISILICON;nMenuID <= ID_DECODER_HACCEL;nMenuID ++)
	{
		if (nMenuID == nID)
			pMenu->CheckMenuItem(nMenuID,MF_BYCOMMAND|MF_CHECKED);
		else
			pMenu->CheckMenuItem(nMenuID,MF_BYCOMMAND|MF_UNCHECKED);
	}
}

void CDVOIPCSDKDemoDlg::OnDecoderFfmpeg()
{
	

}

void CDVOIPCSDKDemoDlg::OnDecoderHisilicon()
{
// 	m_nManufacturer = HISILICON;
// 	CMenu *pMenu = GetMenu()->GetSubMenu(1);
// 	pMenu->CheckMenuItem(ID_DECODER_FFMPEG,MF_BYCOMMAND|MF_UNCHECKED);
// 	pMenu->CheckMenuItem(ID_DECODER_HISILICON,MF_BYCOMMAND|MF_CHECKED);
}

void CDVOIPCSDKDemoDlg::OnDecoderCompare()
{
	m_listDecode.clear();
	m_wndConnectionList.DeleteAllItems();
	// 连续创建两个连接
	OnBnClickedButtonConnect();
	OnBnClickedButtonConnect();
	m_nManufacturer = FFMPEG;
	m_pCurSeclectDecoder = m_listDecode.front().get();
	m_nCurSelect = 0;
	m_nLastSelected = 0;
	OnBnClickedButtonPlaystream();

	m_nManufacturer = HISILICON;
	m_pCurSeclectDecoder = m_listDecode.back().get();
	m_nCurSelect = 1;
	m_nLastSelected = 1;
	OnBnClickedButtonPlaystream();
	m_listDecode.front().get()->ClearFrameCacke();
	m_listDecode.back().get()->ClearFrameCacke();
}
void CDVOIPCSDKDemoDlg::OnMButtonDown(UINT nFlags, CPoint point)
{
	
	CDialog::OnMButtonDown(nFlags, point);
}


void CDVOIPCSDKDemoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDVOIPCSDKDemoDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_pCurSeclectDecoder && 
		pScrollBar->GetDlgCtrlID() == IDC_SLIDER_GRAPHICQULITY)
	{
		m_pCurSeclectDecoder->m_nGQP = (GraphicQulityParameter)(((CSliderCtrl *)pScrollBar)->GetPos() - 3);
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


LRESULT CDVOIPCSDKDemoDlg::OnInitDxSurface(WPARAM w,LPARAM l)
{
	if (!w || !l)
		return -1;
	CDxSurface *pDxSurface = (CDxSurface *)w;
	DxSurfaceInitInfo *pInitInfo = (DxSurfaceInitInfo *)l;
	if (!pDxSurface->InitD3D(pInitInfo->hPresentWnd,
							pInitInfo->nFrameWidth,
							pInitInfo->nFrameHeight,
							pInitInfo->bWindowed,
							pInitInfo->nD3DFormat))
	{
		assert(false);
		return 0;
	}
	return 0;
}

LRESULT CDVOIPCSDKDemoDlg::OnRenderFrame(WPARAM w,LPARAM l)
{
	if (!w || !l)
		return -1;

	CDxSurface *pDxSurface = (CDxSurface *)w;
	DxSurfaceRenderInfo *pRenderInfo = (DxSurfaceRenderInfo *)l;

	if (!pDxSurface->IsInited())		// D3D设备尚未创建,说明未初始化
	{
		assert(false);
		return 0;
	}
	
	pDxSurface->Render(pRenderInfo->pAvFrame,pRenderInfo->hPresentWnd);	
	return 0;
}
void CDVOIPCSDKDemoDlg::OnFileSetrecordpath()
{
	CSetRecordPath dlg;
	dlg.SetRecordPath(m_szRecordPath);
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetRecordPath(m_szRecordPath,MAX_PATH);
	}
}
