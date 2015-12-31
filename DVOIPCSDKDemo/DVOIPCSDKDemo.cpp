
// DVOIPCSDKDemo.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "DVOIPCSDKDemo.h"
#include "DVOIPCSDKDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDVOIPCSDKDemoApp

BEGIN_MESSAGE_MAP(CDVOIPCSDKDemoApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDVOIPCSDKDemoApp ����

CDVOIPCSDKDemoApp::CDVOIPCSDKDemoApp()
{
}


// Ψһ��һ�� CDVOIPCSDKDemoApp ����

CDVOIPCSDKDemoApp theApp;


// CDVOIPCSDKDemoApp ��ʼ��

BOOL CDVOIPCSDKDemoApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();
	DVO_NET_Init();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CDVOIPCSDKDemoDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int CDVOIPCSDKDemoApp::ExitInstance()
{
	DVO_NET_Release();
	return CWinAppEx::ExitInstance();
}
