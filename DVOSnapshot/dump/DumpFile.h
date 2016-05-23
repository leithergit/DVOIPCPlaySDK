/*
* 一个宏命令，就可以程序崩溃时生成dump文件
* 
* 在主程序初始化时加入
* DeclareDumpFile();
* 
* 创建头文件DumpFile.h, 将下列代码放进文件中
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
		MINIDUMPWRITEDUMP  g_pDumpFunc = NULL;        // 声明函数指针
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
        // 创建Dump文件  
        //  
        HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, NULL, NULL);  
        if (hDumpFile == INVALID_HANDLE_VALUE)
        {
            //DWORD nErr = GetLastError();
            //CString strmsg;
            //strmsg.Format(_T("本程序遇到未处理的异常，MiniDump文件创建失败,%d, %s"),nErr,lpstrDumpFilePathName);
            //MessageBox(NULL, strmsg, TEXT("提示"), MB_OK);
            return;
        }

        // Dump信息  
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

        // 写入Dump文件内容  
        //  
#if DBG_DUMP_DLL_DYM      
  			BOOL bDumpOK = g_pDumpFunc(GetCurrentProcess(), dwProcess, hDumpFile, MiniDumpNormal, pException ? &dumpInfo : NULL, NULL, NULL); 
#else        
        BOOL bDumpOK = ::MiniDumpWriteDump(GetCurrentProcess(), dwProcess, hDumpFile, MiniDumpNormal, pException ? &dumpInfo : NULL, NULL, NULL); 
        //BOOL bDumpOK = ::MiniDumpWriteDump(::GetCurrentProcess(), dwProcess, hDumpFile, MiniDumpNormal, pException ? &dumpInfo : NULL, NULL, &mci);
#endif          
        //if (bDumpOK)
        //{
        //    MessageBox(NULL, TEXT("本程序遇到未处理的异常，MiniDump文件已经生成在程序的运行目录。"), TEXT("提示"), MB_OK);
        //}
        //else
        //{
        //    MessageBox(NULL, TEXT("本程序遇到未处理的异常，生成MiniDump文件失败。"), TEXT("提示"), MB_OK);
        //}

        CloseHandle(hDumpFile);  
    }  


    LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
    {
        return NULL;
    }


    BOOL PreventSetUnhandledExceptionFilter()
    {
        // 此函数一旦成功调用，之后对 SetUnhandledExceptionFilter 的调用将无效
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
        // 获取当前工作路径
		TCHAR szAppPath[1024] = { 0 };
		GetAppName(szAppPath, 1024);
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		TCHAR szFileName[1024] = { 0 };
		_stprintf(szFileName,_T("%s_%04d-%02d-%02dT%02d-%02d-%02d.dmp"),szAppPath,sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
		CreateDumpFile(szFileName, pException);
#if 0
        // 这里弹出一个错误对话框并退出程序  
        //  
        FatalAppExit(-1,  _T("*** App Unhandled Exception! ***")); 
#endif      
        /*
        EXCEPTION_CONTINUE_EXECUTION (–1)  异常被忽略，控制流将在异常出现的点之后，继续恢复运行。
        EXCEPTION_CONTINUE_SEARCH (0)  异常不被识别，也即当前的这个__except模块不是这个异常错误所对应的正确的异常处理模块。系统将继续到上一层的try-except域中继续查找一个恰当的__except模块。
        EXCEPTION_EXECUTE_HANDLER (1)  异常已经被识别，也即当前的这个异常错误，系统已经找到了并能够确认，这个__except模块就是正确的异常处理模块。控制流将进入到__except模块中。
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
        //注册异常处理函数
        SetUnhandledExceptionFilter(UnhandledExceptionFilterEx);
        //使SetUnhandledExceptionFilter无效 
        PreventSetUnhandledExceptionFilter();
    }
};


#define DeclareDumpFile() NSDumpFile::RunCrashHandler();