/*
* һ��������Ϳ��Գ������ʱ����dump�ļ�
* 
* ���������ʼ��ʱ����
* DeclareDumpFile();
* 
* ����ͷ�ļ�DumpFile.h, �����д���Ž��ļ���
* */

#pragma once
#include <windows.h>
#include <iostream>  
using namespace std; 

#ifndef _M_IX86  
#error "The following code only works for x86!"  
#endif

#define DBG_DUMP_DLL_DYM	0

#if DBG_DUMP_DLL_DYM
#else
#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
#endif

namespace NSDumpFile
{
		typedef  BOOL  (*MINIDUMPWRITEDUMP)(HANDLE hProcess,
	                                    DWORD ProcessId,
	                                    HANDLE hFile,
	                                    MINIDUMP_TYPE DumpType,
	                                    PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	                                    PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	                                    PMINIDUMP_CALLBACK_INFORMATION CallbackParam);
#if DBG_DUMP_DLL_DYM  	
		MINIDUMPWRITEDUMP  g_pDumpFunc = NULL;        // ��������ָ��
#endif
	
    inline BOOL IsDataSectionNeeded(const WCHAR* pModuleName)  
    {  
        if(pModuleName == 0)  
        {  
            return FALSE;  
        }  

        WCHAR szFileName[_MAX_FNAME] = L"";  
        _wsplitpath(pModuleName, NULL, NULL, szFileName, NULL);  

        if(wcsicmp(szFileName, L"ntdll") == 0)  
            return TRUE;  

        return FALSE;  
    }

    inline BOOL CALLBACK MiniDumpCallback(PVOID pParam,  
        const PMINIDUMP_CALLBACK_INPUT   pInput,  
        PMINIDUMP_CALLBACK_OUTPUT        pOutput)  
    {  
        if(pInput == 0 || pOutput == 0)  
            return FALSE;  

        switch(pInput->CallbackType)  
        {  
        case ModuleCallback:  
            if(pOutput->ModuleWriteFlags & ModuleWriteDataSeg)  
                if(!IsDataSectionNeeded(pInput->Module.FullPath))  
                    pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);  
        case IncludeModuleCallback:  
        case IncludeThreadCallback:  
        case ThreadCallback:  
        case ThreadExCallback:  
            return TRUE;  
        default:;  
        }  

        return FALSE;  
    } 

    void CreateDumpFile(LPCTSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)  
    {  
        // ����Dump�ļ�  
        //  
        HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, NULL, NULL);  
        if (hDumpFile == INVALID_HANDLE_VALUE)
        {
            //DWORD nErr = GetLastError();
            //CString strmsg;
            //strmsg.Format(_T("����������δ������쳣��MiniDump�ļ�����ʧ��,%d, %s"),nErr,lpstrDumpFilePathName);
            //MessageBox(NULL, strmsg, TEXT("��ʾ"), MB_OK);
            return;
        }

        // Dump��Ϣ  
        //  
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;  
        dumpInfo.ExceptionPointers = pException;  
        dumpInfo.ThreadId = ::GetCurrentThreadId();  
        dumpInfo.ClientPointers = TRUE;  
        DWORD dwProcess = ::GetCurrentProcessId();
        //HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcess);
        MINIDUMP_CALLBACK_INFORMATION mci;  
        mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;  
        mci.CallbackParam       = 0; 

        // д��Dump�ļ�����  
        //  
#if DBG_DUMP_DLL_DYM      
  			BOOL bDumpOK = g_pDumpFunc(GetCurrentProcess(), dwProcess, hDumpFile, MiniDumpNormal, pException ? &dumpInfo : NULL, NULL, NULL); 
#else        
        BOOL bDumpOK = ::MiniDumpWriteDump(GetCurrentProcess(), dwProcess, hDumpFile, MiniDumpNormal, pException ? &dumpInfo : NULL, NULL, NULL); 
        //BOOL bDumpOK = ::MiniDumpWriteDump(::GetCurrentProcess(), dwProcess, hDumpFile, MiniDumpNormal, pException ? &dumpInfo : NULL, NULL, &mci);
#endif          
        //if (bDumpOK)
        //{
        //    MessageBox(NULL, TEXT("����������δ������쳣��MiniDump�ļ��Ѿ������ڳ��������Ŀ¼��"), TEXT("��ʾ"), MB_OK);
        //}
        //else
        //{
        //    MessageBox(NULL, TEXT("����������δ������쳣������MiniDump�ļ�ʧ�ܡ�"), TEXT("��ʾ"), MB_OK);
        //}

        CloseHandle(hDumpFile);  
    }  


    LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
    {
        return NULL;
    }


    BOOL PreventSetUnhandledExceptionFilter()
    {
        // �˺���һ���ɹ����ã�֮��� SetUnhandledExceptionFilter �ĵ��ý���Ч
        HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
        if (hKernel32 ==   NULL)
            return FALSE;


        void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
        if(pOrgEntry == NULL)
            return FALSE;


        unsigned char newJump[ 100 ];
        DWORD dwOrgEntryAddr = (DWORD)(ULONGLONG)pOrgEntry;
        dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far


        void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
        DWORD dwNewEntryAddr = (DWORD)(ULONGLONG)pNewFunc;
        DWORD dwRelativeAddr = dwNewEntryAddr -  dwOrgEntryAddr;


        newJump[ 0 ] = 0xE9;  // JMP absolute
        memcpy(&newJump[ 1 ], &dwRelativeAddr, sizeof(pNewFunc));
        SIZE_T bytesWritten;
        BOOL bRet = WriteProcessMemory(GetCurrentProcess(),    pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
        return bRet;
    }

	int StrReserverFind(LPCTSTR srcstr, TCHAR ch)
	{
		LPTSTR lpsz = (LPTSTR)_tcsrchr(srcstr, ch);
		return (lpsz == NULL) ? -1 : (int)(lpsz - srcstr);
	}

	void GetAppPath(TCHAR *szPath, int nBuffLen)
	{
		int nPos;
		TCHAR	szTempPath[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szTempPath, MAX_PATH);
		nPos = StrReserverFind(szTempPath, _T('\\'));
#ifdef _DEBUG
		if (nBuffLen <= 0)
			assert(true);
#endif
		_tcsncpy(szPath, szTempPath, nPos);
	}

	void GetAppName(TCHAR *szPath, int nBuffLen)
	{
		int nPos;
		TCHAR	szTempPath[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szTempPath, MAX_PATH);
		nPos = StrReserverFind(szTempPath, _T('.'));
#ifdef _DEBUG
		if (nBuffLen <= 0)
			assert(true);
#endif
		_tcsncpy(szPath, szTempPath, nPos);
	}


    LONG WINAPI UnhandledExceptionFilterEx(struct _EXCEPTION_POINTERS *pException)
    {
        // ��ȡ��ǰ����·��
		TCHAR szAppPath[1024] = { 0 };
		GetAppName(szAppPath, 1024);
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		TCHAR szFileName[1024] = { 0 };
		_stprintf(szFileName,_T("%s_%04d-%02d-%02dT%02d-%02d-%02d.dmp"),szAppPath,sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
		CreateDumpFile(szFileName, pException);
#if 0
        // ���ﵯ��һ������Ի����˳�����  
        //  
        FatalAppExit(-1,  _T("*** App Unhandled Exception! ***")); 
#endif      
        /*
        EXCEPTION_CONTINUE_EXECUTION (�C1)  �쳣�����ԣ������������쳣���ֵĵ�֮�󣬼����ָ����С�
        EXCEPTION_CONTINUE_SEARCH (0)  �쳣����ʶ��Ҳ����ǰ�����__exceptģ�鲻������쳣��������Ӧ����ȷ���쳣����ģ�顣ϵͳ����������һ���try-except���м�������һ��ǡ����__exceptģ�顣
        EXCEPTION_EXECUTE_HANDLER (1)  �쳣�Ѿ���ʶ��Ҳ����ǰ������쳣����ϵͳ�Ѿ��ҵ��˲��ܹ�ȷ�ϣ����__exceptģ�������ȷ���쳣����ģ�顣�����������뵽__exceptģ���С�
        */
#if _DEBUG
        return EXCEPTION_CONTINUE_SEARCH;
#endif
        return EXCEPTION_EXECUTE_HANDLER;
    }


    void RunCrashHandler()
    {
#if DBG_DUMP_DLL_DYM    	
    		// load dgbhelp.dll
		    HMODULE hDll = NULL;
		    hDll = LoadLibrary("dbghelp.dll") ;
		    if (hDll == NULL)
		    {
		        AfxMessageBox("load dbghelp.dll fail");
		        return ;
		    }
		    // load MiniDumpWriteDump function
		    g_pDumpFunc = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
		    if (g_pDumpFunc == NULL)
		    {
		        AfxMessageBox("load MiniDumpWriteDump function fail");
		        return ;
		    }
#endif		    
        //ע���쳣������
        SetUnhandledExceptionFilter(UnhandledExceptionFilterEx);
        //ʹSetUnhandledExceptionFilter��Ч 
        PreventSetUnhandledExceptionFilter();
    }
};


#define DeclareDumpFile() NSDumpFile::RunCrashHandler();