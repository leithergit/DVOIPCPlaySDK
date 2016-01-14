#if !defined(AFX_GLLITERYSTATIC_H__1F4BE9DF_719E_4F93_B78B_9AA0EAC8AB88__INCLUDED_)
#define AFX_GLLITERYSTATIC_H__1F4BE9DF_719E_4F93_B78B_9AA0EAC8AB88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GlliteryStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGlliteryStatic window

class CGlliteryStatic : public CStatic
{
// Construction
public:
	CGlliteryStatic();
	virtual ~CGlliteryStatic();
	void SetOkGllitery(/*COLORREF clrText,COLORREF clrBack*/);
	void SetAlarmGllitery(/*COLORREF clrText,COLORREF clrBack*/);
	void SetBkColor(COLORREF clrBack);
	void SetTextColor(COLORREF clrText);
	UINT m_nSleeptime;
	

// Attributes
public:

// Operations
public:
	
	CRITICAL_SECTION m_CS;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlliteryStatic)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CGlliteryStatic)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	COLORREF m_clrText,m_clrBack;
	COLORREF m_clrOriText,m_clrOriBack;
	COLORREF m_clrCurText,m_clrCurBack;
	CBrush m_brBkgnd;
	INT m_iGlitteryCount;	
	static DWORD Glittery(LPVOID lpParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLLITERYSTATIC_H__1F4BE9DF_719E_4F93_B78B_9AA0EAC8AB88__INCLUDED_)
