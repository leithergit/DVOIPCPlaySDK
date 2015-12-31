#pragma once


// CSetRecordPath dialog

class CSetRecordPath : public CDialog
{
	DECLARE_DYNAMIC(CSetRecordPath)

public:
	CSetRecordPath(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetRecordPath();

// Dialog Data
	enum { IDD = IDD_SETRECORDPATH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowse();
	virtual BOOL OnInitDialog();
	TCHAR	m_szRecordPath[MAX_PATH];
	void SetRecordPath(TCHAR *szPath)
	{
		memcpy(m_szRecordPath,szPath,MAX_PATH);
	}

	void GetRecordPath(TCHAR *szPath,int nBuffLen)
	{
		memcpy(szPath,m_szRecordPath,nBuffLen);
	}
};
