
// DVOIPCSDKDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDVOIPCSDKDemoApp:
// �йش����ʵ�֣������ DVOIPCSDKDemo.cpp
//

class CDVOIPCSDKDemoApp : public CWinAppEx
{
public:
	CDVOIPCSDKDemoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CDVOIPCSDKDemoApp theApp;