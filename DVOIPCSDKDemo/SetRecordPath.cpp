// SetRecordPath.cpp : implementation file
//

#include "stdafx.h"
#include "DVOIPCSDKDemo.h"
#include "SetRecordPath.h"


// CSetRecordPath dialog

IMPLEMENT_DYNAMIC(CSetRecordPath, CDialog)

CSetRecordPath::CSetRecordPath(CWnd* pParent /*=NULL*/)
	: CDialog(CSetRecordPath::IDD, pParent)
{

}

CSetRecordPath::~CSetRecordPath()
{
}

void CSetRecordPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetRecordPath, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CSetRecordPath::OnBnClickedButtonBrowse)
END_MESSAGE_MAP()


// CSetRecordPath message handlers



BOOL CSetRecordPath::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_EDIT_RECORDPATH,m_szRecordPath);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetRecordPath::OnBnClickedButtonBrowse()
{
	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = m_szRecordPath;   
	bi.lpszTitle = _T("��ѡ��¼���ļ��洢Ŀ¼:");   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	//����ѡ��Ŀ¼�Ի���
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);   
	TCHAR szRecPath[MAX_PATH] = {0};

	if(lp && SHGetPathFromIDList(lp, szRecPath))   
	{
		memcpy(m_szRecordPath,szRecPath,sizeof(szRecPath));
		SetDlgItemText(IDC_EDIT_RECORDPATH,m_szRecordPath);
	}
	else   
		AfxMessageBox(_T("��Ч��Ŀ¼��������ѡ��")); 
}