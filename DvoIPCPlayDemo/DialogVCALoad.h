#pragma once
#include "afxeditbrowsectrl.h"


// CDialogVCALoad dialog

class CDialogVCALoad : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogVCALoad)

public:
	CDialogVCALoad(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogVCALoad();

// Dialog Data
	enum { IDD = IDD_DIALOG_VCALOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadvca();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBrowse();
};
