
// DvoIPCPlayDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDvoIPCPlayDemoApp: 
// �йش����ʵ�֣������ DvoIPCPlayDemo.cpp
//

class CDvoIPCPlayDemoApp : public CWinApp
{
public:
	CDvoIPCPlayDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CDvoIPCPlayDemoApp theApp;