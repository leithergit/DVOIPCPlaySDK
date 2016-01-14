// GlliteryStatic.cpp : implementation file
//

#include "stdafx.h"
#include "GlliteryStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGlliteryStatic

CGlliteryStatic::CGlliteryStatic()
{
	m_iGlitteryCount = 0;
	m_clrText = m_clrOriText = RGB(0,0,0);	
	m_clrBack = m_clrOriBack = ::GetSysColor(COLOR_3DFACE);	
	m_brBkgnd.CreateSolidBrush(m_clrBack);
	::InitializeCriticalSection(&m_CS);
}

CGlliteryStatic::~CGlliteryStatic()
{
	m_brBkgnd.DeleteObject();
	::DeleteCriticalSection(&m_CS);
}

BEGIN_MESSAGE_MAP(CGlliteryStatic, CStatic)
	//{{AFX_MSG_MAP(CGlliteryStatic)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGlliteryStatic message handlers

HBRUSH CGlliteryStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor(m_clrText);
	pDC->SetBkColor(m_clrBack);
	return (HBRUSH) m_brBkgnd;	
}

void CGlliteryStatic::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
	this->Invalidate();
}

void CGlliteryStatic::SetBkColor(COLORREF clrBack)
{
	m_clrBack = clrBack;
	m_brBkgnd.DeleteObject();
	m_brBkgnd.CreateSolidBrush(clrBack);
	this->Invalidate();
}
//使文本闪烁
DWORD CGlliteryStatic::Glittery(LPVOID lpParam)
{
	CGlliteryStatic *pCtrl	= (CGlliteryStatic *)lpParam;
	while (pCtrl->m_iGlitteryCount <8)
	{
		::EnterCriticalSection(&pCtrl->m_CS);
		if (pCtrl->m_iGlitteryCount % 2 ==0)
		{
			pCtrl->m_clrBack = pCtrl->m_clrCurBack;
			pCtrl->m_clrText = pCtrl->m_clrCurText;
		}
		else
		{
			//pCtrl->m_clrBack = pCtrl->m_clrOriBack;
			pCtrl->m_clrText = pCtrl->m_clrOriText;
		}
		
		pCtrl->m_brBkgnd.DeleteObject();
		pCtrl->m_brBkgnd.CreateSolidBrush(pCtrl->m_clrBack);
		pCtrl->Invalidate();
		pCtrl->m_iGlitteryCount ++;	
		::LeaveCriticalSection(&pCtrl->m_CS);
		Sleep(pCtrl->m_nSleeptime);
	}
	pCtrl->m_clrBack = pCtrl->m_clrOriBack;
	pCtrl->m_clrText = pCtrl->m_clrOriText;
	pCtrl->m_brBkgnd.DeleteObject();
	pCtrl->m_brBkgnd.CreateSolidBrush(pCtrl->m_clrBack);
	pCtrl->Invalidate();
	return 0;
}

void CGlliteryStatic::SetAlarmGllitery(/*COLORREF clrText, COLORREF clrBack*/)
{
	m_clrCurBack = RGB(255,0,0);;
	m_clrCurText = RGB(255,255,0);
	m_iGlitteryCount = 0;						//闪烁计数
	m_nSleeptime = 300;
	::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) Glittery,(LPVOID)this,0,0);
}
void CGlliteryStatic::SetOkGllitery(/*COLORREF clrText, COLORREF clrBack*/)
{
	m_clrCurBack = RGB(0,255,0);
	m_clrCurText = RGB(255,255,0);
	m_iGlitteryCount = 0;					//闪烁计数
	m_nSleeptime = 150;
	::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) Glittery,(LPVOID)this,0,0);
}