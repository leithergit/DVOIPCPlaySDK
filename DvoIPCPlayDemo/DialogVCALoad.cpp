// DialogVCALoad.cpp : implementation file
//

#include "stdafx.h"
#include "DvoIPCPlayDemo.h"
#include "DialogVCALoad.h"
#include "afxdialogex.h"
#include "Utility.h"
#include "DvoIPCPlayDemoDlg.h"

// CDialogVCALoad dialog

IMPLEMENT_DYNAMIC(CDialogVCALoad, CDialogEx)

CDialogVCALoad::CDialogVCALoad(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogVCALoad::IDD, pParent)
{

}

CDialogVCALoad::~CDialogVCALoad()
{
}

void CDialogVCALoad::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogVCALoad, CDialogEx)
	ON_BN_CLICKED(IDC_LOADVCA, &CDialogVCALoad::OnBnClickedLoadvca)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BROWSE, &CDialogVCALoad::OnBnClickedBrowse)
END_MESSAGE_MAP()


// CDialogVCALoad message handlers

void CDialogVCALoad::OnBnClickedLoadvca()
{
// 	if (!g_pVca)
// 	{
// 		bool bSucceed = false;
// 		CWaitCursor Wait;
// 		g_pVca = new CVca;
// 		TCHAR szLicense[512] = { 0 };
// 		GetDlgItemText(IDC_EDIT_LICENSE, szLicense, 512);
// 		if (!g_pVca->SetupVCA(nullptr, szLicense) )
// 			goto Failed;
// 		if (!g_pVca->OpenVCA())
// 			goto Failed;
// 		bSucceed = true;
// 		goto Succeed;
// Failed:
// 		if (g_pVca)
// 		{
// 			delete g_pVca;
// 			g_pVca = nullptr;
// 		}
// 		
// 	Succeed:
// 		if (bSucceed)
// 		{
// 			CDvoIPCPlayDemoDlg *pDlg = (CDvoIPCPlayDemoDlg *)AfxGetApp()->m_pMainWnd;
// 			
// 			AfxMessageBox(_T("加载视频分析引擎成功."));
// 		}
// 		else
// 			AfxMessageBox(_T("加载视频分析引擎失败."),MB_ICONERROR|MB_OK);
// 	}
}


BOOL CDialogVCALoad::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	TCHAR szPath[512] = { 0 };
	GetAppPath(szPath, 512);
	_tcscat(szPath, _T("\\License.lic"));
	SetDlgItemText(IDC_EDIT_LICENSE, szPath);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogVCALoad::OnDestroy()
{
	CDialogEx::OnDestroy();
}


void CDialogVCALoad::OnBnClickedBrowse()
{
	TCHAR szText[1024] = { 0 };

	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	TCHAR  szFilter[] = _T("License Flies (*.lic)|*.lic|All Files (*.*)|*.*||");
	TCHAR szExportLog[512] = { 0 };
	CTime tNow = CTime::GetCurrentTime();
	CFileDialog OpenDataBase(TRUE, _T("*.lic"), _T(""), dwFlags, (LPCTSTR)szFilter);
	OpenDataBase.m_ofn.lpstrTitle = _T("Plz select a license file");
	if (OpenDataBase.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LICENSE, OpenDataBase.GetPathName());
	}
}
