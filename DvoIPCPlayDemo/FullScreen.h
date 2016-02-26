#ifndef _Full_Screen_2015_08_21
#define _Full_Screen_2015_08_21
#pragma once

#include "stdafx.h"
#include "Utility.h"
class CFullScreen
{
	CFullScreen ( const CFullScreen &refFull ) ;
	CFullScreen &operator= ( const CFullScreen &refFull ) ;

public:
	CFullScreen ()
		:m_hWnd(NULL),
		m_bInitialized(false)
	{
		/*Init () ;*/
	}

	~CFullScreen (){}

	bool IsFullScreen ()
	{
		return m_bFullScreen ;
	}
	
	void Reset()
	{
		ZeroMemory(this,sizeof(CFullScreen));
	}

	void ToggleFullScreen()
	{
		assert(m_hWnd != NULL);
		if (!m_hWnd)
			return ;
		if (!m_bInitialized)						
			Init () ;

		if ( !m_bFullScreen )
		{			
			GetWindowPlacement(m_hWnd, &m_windowedPWP ) ;
			m_dwExStyle = GetWindowLong( m_hWnd, GWL_EXSTYLE ) ;
			m_dwStyle = GetWindowLong( m_hWnd, GWL_STYLE ) ;
			m_dwStyle &= ~WS_MAXIMIZE & ~WS_MINIMIZE; // remove minimize/maximize style
			m_hMenu = GetMenu( m_hWnd ) ;
			// Hide the window to avoid animation of blank windows
			ShowWindow ( m_hWnd, SW_HIDE ) ;
			// Set FS window style
			SetWindowLong ( m_hWnd, GWL_STYLE, WS_POPUP | WS_SYSMENU | WS_CLIPCHILDREN ) ;
			SetMenu ( m_hWnd, NULL ) ;
			ShowWindow ( m_hWnd, SW_SHOWMAXIMIZED ) ;
		}
		else
		{
			if ( m_dwStyle != 0 )
				SetWindowLong( m_hWnd, GWL_STYLE, m_dwStyle );

			if ( m_dwExStyle != 0 )
				SetWindowLong( m_hWnd, GWL_EXSTYLE, m_dwExStyle );

			if ( m_hMenu != NULL )
				SetMenu( m_hWnd, m_hMenu );

 			if ( m_windowedPWP.length == sizeof( WINDOWPLACEMENT ) )
 			{
 				if ( m_windowedPWP.showCmd == SW_SHOWMAXIMIZED )
 				{
					ShowWindow ( m_hWnd, SW_HIDE );
					m_windowedPWP.showCmd = SW_HIDE ;
					SetWindowPlacement( m_hWnd, &m_windowedPWP) ;
					ShowWindow ( m_hWnd, SW_SHOWMAXIMIZED ) ;
					m_windowedPWP.showCmd = SW_SHOWMAXIMIZED ;
 				}
 				else
 					SetWindowPlacement( m_hWnd, &m_windowedPWP ) ;
			}
		}
		m_bFullScreen = !m_bFullScreen ;		
	}
	inline HWND GetWnd()
	{
		//assert(m_hWnd != NULL);
		return m_hWnd;
	}
	inline void SetWnd(HWND hWnd)
	{
		assert(hWnd != NULL);
		m_hWnd = hWnd;
	}
public:
	void	*pUserPtr;
private:
	void Init ()
	{
		m_bFullScreen = false ;
		m_dwExStyle = 0 ;
		m_dwStyle = 0 ;
		m_hMenu = NULL ;
		m_bInitialized = true;
		ZeroMemory( &m_windowedPWP, sizeof( WINDOWPLACEMENT ) ) ;
		m_windowedPWP.length = sizeof( WINDOWPLACEMENT ) ;
	}

private:
	HWND m_hWnd ;
	bool m_bInitialized;
	bool m_bFullScreen ;
	WINDOWPLACEMENT m_windowedPWP ;
	DWORD m_dwExStyle ;
	DWORD m_dwStyle ;
	HMENU m_hMenu ;

} ;
#endif	// _Full_Screen_2015_08_21

