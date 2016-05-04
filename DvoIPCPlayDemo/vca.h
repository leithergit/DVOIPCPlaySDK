#pragma once
/*
#include "stdafx.h"
#include <atlfile.h>
#include <windows.h>
#include <initguid.h>
#include "vca5corelib.h"
#include "VCAMetaLib.h"

//DEFINE_GUID(IID_IVCA5, 0x48c2cd0, 0x4685, 0x43a8, 0xbf, 0x4d, 0xbd, 0xf3, 0xbc, 0xa1, 0xe9, 0x4f);
class CVca
{
	IVCA5*				g_VCAApi;
	VCA5_SYSTEM_INFO	g_VCA5SysInfo;
	DWORD				g_nVCAEngine;
	VCA5_ENGINE_PARAMS	g_EngineParam;
	VCA5_LICENSE_INFO	g_LicenseInfo;
	CVCAMetaLib*		m_pVCAMetaDateParser = nullptr;
public:
	BYTE* m_pResult = nullptr;
	DWORD m_nResult = VCA5_MAX_OUTPUT_BUF_SIZE;
	byte  *m_pYuvImage = nullptr;
	int	  m_nYuvImageSize = 0;
	CVca()
	{
		m_pResult = new BYTE[VCA5_MAX_OUTPUT_BUF_SIZE];
		ZeroMemory(m_pResult, VCA5_MAX_OUTPUT_BUF_SIZE);
	}
	~CVca()
	{
		if (m_pResult)
			delete []m_pResult;
		if (m_pYuvImage)
			delete[]m_pYuvImage;
		CloseVCA();
		EndupVCA();
	}
	void DbgMsg(LPCSTR szFmt, ...)
	{
#define DEBUG_STRING_LENGTH		255
		char szOutBuff[DEBUG_STRING_LENGTH];

		va_list  args;
		va_start(args, szFmt);


		_vsnprintf(szOutBuff, DEBUG_STRING_LENGTH - 1, szFmt, args);

		va_end(args);
		szOutBuff[DEBUG_STRING_LENGTH - 1] = NULL;
		fprintf(stderr, szOutBuff);
	}

#define	_V(x) \
	do { \
		if (x==0) { \
			printf("%s return 0\n", #x); \
			return FALSE; \
						} \
			} while (0)

public:
	static BOOL Uda5CreateInstance(HMODULE hLib, REFIID riid, void ** ppInterface)
	{
		if (hLib) {

			BOOL rs;
			IUnknown* pUnknown;
			BOOL(FAR WINAPI*_CreateInstance)(IUnknown ** ppInterface);
			FARPROC test_proc = GetProcAddress(hLib, "VCA5CreateInstance");
			if (test_proc) {
				*(FARPROC*)&_CreateInstance = test_proc;
				rs = (*_CreateInstance)(&pUnknown);
				if (rs) {
					HRESULT hr;
					hr = pUnknown->QueryInterface(riid, ppInterface);
					pUnknown->Release();
					if (SUCCEEDED(hr))
						return TRUE;
				}
			}
		}

		return FALSE;
	}


	char* ShowFunction(ULONG function)
	{
		static char strTemp[1024];
		memset(strTemp, 0, sizeof(strTemp));

		if (function&VCA5_FEATURE_PRESENCE) strcat(strTemp, "PRESENCE|");
		if (function&VCA5_FEATURE_ENTER) strcat(strTemp, "ENTER|");
		if (function&VCA5_FEATURE_EXIT) strcat(strTemp, "EXIT|");
		if (function&VCA5_FEATURE_APPEAR) strcat(strTemp, "APPEAR|");
		if (function&VCA5_FEATURE_DISAPPEAR) strcat(strTemp, "DISAPPEAR|");
		if (function&VCA5_FEATURE_STOPPED) strcat(strTemp, "STOPPED|");
		if (function&VCA5_FEATURE_DWELL) strcat(strTemp, "DWELL|");
		if (function&VCA5_FEATURE_DIRECTION) strcat(strTemp, "DIRECTION|");
		if (function&VCA5_FEATURE_SPEED) strcat(strTemp, "SPEED|");
		if (function&VCA5_FEATURE_ABOBJ) strcat(strTemp, "ABOBJ|");
		if (function&VCA5_FEATURE_TAILGATING) strcat(strTemp, "TAILGATING|");
		if (function&VCA5_FEATURE_LINECOUNTER) strcat(strTemp, "LINECOUNTER|");
		if (function&VCA5_FEATURE_PEOPLETRACKING) strcat(strTemp, "PEOPLETRACKING|");
		if (function&VCA5_FEATURE_COUNTING) strcat(strTemp, "COUNTING|");
		if (function&VCA5_FEATURE_CALIBRATION) strcat(strTemp, "CALIBRATION|");
		if (function&VCA5_FEATURE_METADATA) strcat(strTemp, "METADATA|");

		return strTemp;
	}
	
	BOOL InitVCAApi(TCHAR* szDllName, TCHAR* szDrvDllPath, char* szLicense, char* szUSN)
	{
		HMODULE hLib = NULL;
		USES_CONVERSION;

		if (szDllName) 
		{
			_V((hLib = LoadLibrary(szDllName)));
		}

		_V(Uda5CreateInstance(hLib, IID_IVCA5, (void**)&g_VCAApi));

		//Set Version
		BOOL(FAR WINAPI*_VCA5Init)(ULONG ulVersion);
		FARPROC test_proc = GetProcAddress(hLib, "VCA5Init");
		if (test_proc) 
		{
			*(FARPROC*)&_VCA5Init = test_proc;
			(*_VCA5Init)(VCA5_VERSION);
		}

		g_LicenseInfo.szUSN = szUSN;
		g_LicenseInfo.szLicense = szLicense;
		g_LicenseInfo.szDrvDllPath = T2A(szDrvDllPath);

		_V((g_VCAApi->VCA5Activate(1, (VCA5_LICENSE_INFO*)&g_LicenseInfo)));
		_V((g_VCAApi->VCA5GetSystemInfo(&g_VCA5SysInfo)));

		g_nVCAEngine = g_LicenseInfo.ulNumOfEngine;

		DbgMsg("License[%d] EngineCnt [%d] Function [0x%08X] Description [%s] \n",
			g_LicenseInfo.nLicenseID, g_LicenseInfo.ulNumOfEngine, g_LicenseInfo.ulFunction, g_LicenseInfo.szLicenseDesc);
		DbgMsg("Function Detail [%s] \n", ShowFunction(g_LicenseInfo.ulFunction));

		return TRUE;
	}


	BOOL	ReadLicense(LPCTSTR szFIleName, LPSTR szLincens, DWORD nBufferLen, PDWORD pdwBytesRead)
	{
		CAtlFile fLicense;
		ULONGLONG nFileSize = 0;
		DWORD dwBytesRead = 0;

		fLicense.Create(szFIleName, FILE_GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
		fLicense.GetSize(nFileSize);
		fLicense.Read(szLincens, nBufferLen, dwBytesRead);

		if (pdwBytesRead)
			*pdwBytesRead = dwBytesRead;

		if (nFileSize == dwBytesRead)
			return TRUE;

		DbgMsg("Can not Open file [%s]\n", szFIleName);

		return FALSE;
	}
#define VCA5_DLL_PATH			TEXT("VCA5Lib.dll")

	typedef BOOL(WINAPI* LPFCAP5QUERYINFO)(ULONG, ULONG uIn, void *);
	BOOL	SetupVCA(TCHAR* szDrvDll, TCHAR* szLicenseFile)
	{
		TCHAR	*szVCADll = VCA5_DLL_PATH;
		char	szLicense[1024];
		char	szUSN[16];
		TCHAR	szLicenseFilePath[255];

		HMODULE hCAP5Lib = NULL;
		LPFCAP5QUERYINFO	pCap5QueryInfo;

		memset(szLicense, 0, _countof(szLicense));
		memset(szUSN, 0, _countof(szUSN));

		//Get USN Number
		if (szDrvDll) {
			_V((hCAP5Lib = LoadLibrary(szDrvDll)));
		}
#define CAP5_QR_GET_USN 0x2004
		if (hCAP5Lib){
			pCap5QueryInfo = (LPFCAP5QUERYINFO)GetProcAddress(hCAP5Lib, "Cap5QueryInfo");
			_V(pCap5QueryInfo(CAP5_QR_GET_USN, 16, szUSN));
			FreeLibrary(hCAP5Lib);
		}

		//Default License 
#ifdef VCA_PRESENCE_TEST
		_V((InitVCAApi(szVCADll, szDrvDll, NULL, szUSN)));
#else
		if (!szLicenseFile){
			_stprintf(szLicenseFilePath, _T("license_%c%c%c%c%c%c%c%c%c.lic"),
				szUSN[0], szUSN[1], szUSN[2], szUSN[3], szUSN[4], szUSN[5], szUSN[6], szUSN[7], szUSN[8]);
		}
		else{
			_tcscpy(szLicenseFilePath, szLicenseFile);
		}

		DWORD dwLicenseLength = 0;
		_V((ReadLicense(szLicenseFilePath, szLicense, _countof(szLicense), &dwLicenseLength)));
		szLicense[dwLicenseLength] = 0;

		_V((InitVCAApi(szVCADll, szDrvDll, szLicense, szUSN)));
#endif
		return TRUE;
	}


	void	EndupVCA()
	{
		if (!g_VCAApi){
			return;
		}

		g_nVCAEngine = 0;
		g_VCAApi->Release();
		g_VCAApi = NULL;
	}


	BOOL	OpenVCA()
	{
		if (!g_VCAApi){
			DbgMsg("Open VCA Fail VCA5 instance does not create before\n");
			return FALSE;
		}
		DWORD i;
		g_EngineParam.ulImageSize = VCA5_MAKEIMGSIZE(1280,720);
		g_EngineParam.ulColorFormat = VCA5_COLOR_FORMAT_YV12;
		g_EngineParam.ulVideoFormat = VCA5_VIDEO_FORMAT_NTSC_M;
		g_EngineParam.ulFrameRate100 = 25;

		for (i = 0; i < g_nVCAEngine; i++) {

			g_EngineParam.ulLicenseCnt = 1;
			g_EngineParam.ucLicenseId[0] = g_LicenseInfo.nLicenseID;

			_V(g_VCAApi->VCA5Open(i, &g_EngineParam));
		}
		//m_pVCAMetaDateParser = CreateVCAMetaLib(CVCAMetaLib::MSXML_PARSER);
		return TRUE;
	}

	void	CloseVCA()
	{
		if (!g_nVCAEngine){
			return;
		}

		for (DWORD i = 0; i < g_nVCAEngine; i++) {
			g_VCAApi->VCA5Close(i);
		}
	}
	bool ProcessYuv(const unsigned char* pY,
					const unsigned char* pU,
					const unsigned char* pV,
					int nStrideY,
					int nStrideUV,
					int nWidth,
					int nHeight)
	{
		if (!m_pYuvImage)
			m_pYuvImage = new byte[nWidth*nHeight * 2];
		int nSize = nHeight * nStrideY;
		const size_t nHalfSize = (nSize) >> 1;
		byte *pDestY = m_pYuvImage;										// Y分量起始地址
		byte *pDestV = m_pYuvImage + nSize;								// U分量起始地址
		byte *pDestU = m_pYuvImage + (size_t)(nHalfSize >> 1);			// V分量起始地址

		// YUV420P的U和V分量对调，便成为YV12格式
		memcpy(pDestY, pY, nStrideY*nHeight);
		memcpy(pDestU, pV, nStrideUV*nHeight);
		memcpy(pDestV, pU, nStrideUV*nHeight);
		DWORD nEngine = 0;
		FILETIME timestamp;
		if (g_VCAApi->VCA5Process(nEngine, m_pYuvImage, (VCA5_TIMESTAMP*)&timestamp, &m_nResult, m_pResult) &&
			m_nResult != 0)
		{

		}
		return true;
	}
};

*/