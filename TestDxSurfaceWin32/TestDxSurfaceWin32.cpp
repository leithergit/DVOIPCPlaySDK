// TestDxSurfaceWin32.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TestDxSurfaceWin32.h"
#include <d3d9.h>
#include <assert.h>
#include <memory>
#include <COMMCTRL.H>
//#include <d3dx9tex.h>
#pragma comment ( lib, "d3d9.lib" )
#pragma comment ( lib, "d3dx9.lib" )
#include <stdio.h>
#include <wtypes.h>
#include "../DVOIPCPlaySDK/DxSurface/DxSurface.h"
#include "../DVOIPCPlaySDK/Utility.h"
#include "../DVOIPCPlaySDK/DVOIPCPlaySDK.h"
using namespace std;
using namespace std::tr1;
#define SafeRelease(p) { if(p) { (p)->Release(); (p)=NULL; } }

typedef IDirect3D9* WINAPI pDirect3DCreate9(UINT);
typedef HRESULT WINAPI pDirect3DCreate9Ex(UINT, IDirect3D9Ex**);
CDxSurfaceEx *g_pDxSurface = nullptr;

// #define __countof(array) (sizeof(array)/sizeof(array[0]))
// #pragma warning (disable:4996)
// 
// void DxTraceMsg(char *pFormat, ...)
// {
// 	va_list args;
// 	va_start(args, pFormat);
// 	int nBuff;
// 	CHAR szBuffer[0x7fff];
// 	nBuff = _vsnprintf(szBuffer, __countof(szBuffer), pFormat, args);
// 	//::wvsprintf(szBuffer, pFormat, args);
// 	//assert(nBuff >=0);
// 	OutputDebugStringA(szBuffer);
// 	va_end(args);
// }


void CopyFrameYV12(byte *pDest, int nStride, char *pYUV420P,int nWidth,int nHeight)
{
	int nSize = nHeight * nStride;
	const size_t nHalfSize = (nSize) >> 1;
	byte *pDestY = pDest;										// Y分量起始地址
	byte *pDestV = pDest + nSize;								// U分量起始地址
	byte *pDestU = pDestV + (size_t)(nHalfSize >> 1);			// V分量起始地址
	int linesize[3] = { nWidth, nWidth / 2, nWidth / 2 };
	char *data[3] = { pYUV420P, pYUV420P + nWidth*nHeight, pYUV420P + nWidth*nHeight *5/ 4 };
	
	// YUV420P的U和V分量对调，便成为YV12格式
	// 复制Y分量
	for (int i = 0; i < nHeight; i++)
		memcpy(pDestY + i * nStride, data[0] + i * linesize[0], linesize[0]);

	// 复制YUV420P的U分量到目村的YV12的U分量
	for (int i = 0; i < nHeight / 2; i++)
		memcpy(pDestU + i * nStride / 2, data[1] + i * linesize[1], linesize[1]);

	// 复制YUV420P的V分量到目村的YV12的V分量
	for (int i = 0; i < nHeight / 2; i++)
		memcpy(pDestV + i * nStride / 2, data[2] + i * linesize[2], linesize[2]);
}

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
HMODULE m_hD3D9 = nullptr;
IDirect3D9Ex			*m_pDirect3D9Ex		 = NULL;
IDirect3DDevice9Ex		*m_pDirect3DDeviceEx	= NULL;
pDirect3DCreate9Ex		*m_pDirect3DCreate9Ex	= NULL;
IDirect3DSurface9 *m_pDirect3DSurfaceRender = nullptr;
D3DPRESENT_PARAMETERS m_d3dpp;
int nVideoWidth = 1280;
int nVideoHeight = 720;
char *szYUVFile = "YVU_192.168.3.127_20160505_093408.YUV";
HANDLE g_hYUVFile = nullptr;
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	char szText[32] = { 0 };
	memcpy_s(szText, 32, szYUVFile, strlen(szYUVFile));
	//InitCommonControls();
	hInst = hInstance;
	int nResult = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)WndProc);
	nResult = GetLastError();

	return (int) 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HRESULT hr;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		CheckDlgButton(hWnd, IDC_RADIO_D3DAPI, BST_CHECKED); 
		CheckDlgButton(hWnd, IDC_RADIO_ZOOMOUT, BST_CHECKED);
		return (INT_PTR)TRUE;
	}
		
	case WM_COMMAND:
	{ 
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDOK:
		case IDCANCEL:
		{
			EndDialog(hWnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
			break;
		
		case IDC_BUTTON_INITDX:
		{
			if (IsDlgButtonChecked(hWnd, IDC_RADIO_D3DAPI) == BST_CHECKED)
			{
				if (!m_hD3D9)
					m_hD3D9 = LoadLibraryA("d3d9.dll");
				if (!m_hD3D9)
				{
					DxTraceMsg("%s Failed load D3d9.dll.\n", __FUNCTION__);
					MessageBox(hWnd, "LoadLibraryA Failed.", "提示", MB_OK | MB_ICONSTOP);
					return 0;
				}
			
				m_pDirect3DCreate9Ex = (pDirect3DCreate9Ex*)GetProcAddress(m_hD3D9, "Direct3DCreate9Ex");
				if (!m_pDirect3DCreate9Ex)
				{
					DxTraceMsg("%s Can't locate the Procedure \"Direct3DCreate9Ex\".\n", __FUNCTION__);
					MessageBox(hWnd, "GetProcAddress Failed.", "提示", MB_OK | MB_ICONSTOP);
					return 0;
				}
				hr = m_pDirect3DCreate9Ex(D3D_SDK_VERSION, &m_pDirect3D9Ex);
				if (FAILED(hr))
				{
					DxTraceMsg("%s Direct3DCreate9Ex failed.\n", __FUNCTION__);
					MessageBox(hWnd, "Direct3DCreate9Ex Failed.", "提示", MB_OK | MB_ICONSTOP);
					return 0;
				}
				D3DCAPS9 caps;
				m_pDirect3D9Ex->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
				int vp = 0;
				if (caps.DevCaps& D3DDEVCAPS_HWTRANSFORMANDLIGHT)
					vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
				else
					vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				/*
				caps.DevCaps的取值及含义
				D3DDEVCAPS_CANBLTSYSTONONLOCAL		Device supports blits from system-memory textures to nonlocal video-memory textures.
				D3DDEVCAPS_CANRENDERAFTERFLIP		Device can queue rendering commands after a page flip. Applications do not change their behavior if this flag is set; this capability means that the device is relatively fast.
				D3DDEVCAPS_DRAWPRIMITIVES2			Device can support at least a DirectX 5-compliant driver.
				D3DDEVCAPS_DRAWPRIMITIVES2EX		Device can support at least a DirectX 7-compliant driver.
				D3DDEVCAPS_DRAWPRIMTLVERTEX			Device exports an IDirect3DDevice9::DrawPrimitive-aware hal.
				D3DDEVCAPS_EXECUTESYSTEMMEMORY		Device can use execute buffers from system memory.
				D3DDEVCAPS_EXECUTEVIDEOMEMORY		Device can use execute buffers from video memory.
				D3DDEVCAPS_HWRASTERIZATION			Device has hardware acceleration for scene rasterization.
				D3DDEVCAPS_HWTRANSFORMANDLIGHT		Device can support transformation and lighting in hardware.
				D3DDEVCAPS_NPATCHES					Device supports N patches.
				D3DDEVCAPS_PUREDEVICE				Device can support rasterization, transform, lighting, and shading in hardware.
				D3DDEVCAPS_QUINTICRTPATCHES			Device supports quintic Bézier curves and B-splines.
				D3DDEVCAPS_RTPATCHES				Device supports rectangular and triangular patches.
				D3DDEVCAPS_RTPATCHHANDLEZERO		When this device capability is set, the hardware architecture does not require caching of any information, and uncached patches (handle zero) will be drawn as efficiently as cached ones. Note that setting D3DDEVCAPS_RTPATCHHANDLEZERO does not mean that a patch with handle zero can be drawn. A handle-zero patch can always be drawn whether this cap is set or not.
				D3DDEVCAPS_SEPARATETEXTUREMEMORIES	Device is texturing from separate memory pools.
				D3DDEVCAPS_TEXTURENONLOCALVIDMEM	Device can retrieve textures from non-local video memory.
				D3DDEVCAPS_TEXTURESYSTEMMEMORY		Device can retrieve textures from system memory.
				D3DDEVCAPS_TEXTUREVIDEOMEMORY		Device can retrieve textures from device memory.
				D3DDEVCAPS_TLVERTEXSYSTEMMEMORY		Device can use buffers from system memory for transformed and lit vertices.
				D3DDEVCAPS_TLVERTEXVIDEOMEMORY		Device can use buffers from video memory for transformed and lit vertices.
				*/
				hr = S_OK;
				D3DDISPLAYMODE d3ddm;
				if (FAILED(hr = m_pDirect3D9Ex->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
				{
					DxTraceMsg("%s GetAdapterDisplayMode Failed.\nhr=%x", __FUNCTION__, hr);
					MessageBox(hWnd, "GetAdapterDisplayMode Failed.", "提示", MB_OK | MB_ICONSTOP);
					return 0;
				}

				HWND hDestPresent = nullptr;
				if (IsDlgButtonChecked(hWnd, IDC_RADIO_ZOOMOUT) == BST_CHECKED)
				{
					hDestPresent = GetDlgItem(hWnd, IDC_STATIC_FRAME);
					ShowWindow(hDestPresent, SW_SHOW);
					ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FRAME2), SW_HIDE);
				}
				else
				{
					hDestPresent = GetDlgItem(hWnd, IDC_STATIC_FRAME2);
					ShowWindow(hDestPresent, SW_SHOW);
					ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FRAME), SW_HIDE);
				}

				ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
				m_d3dpp.BackBufferFormat = d3ddm.Format;
				m_d3dpp.BackBufferCount = 1;
				m_d3dpp.Flags = 0;
				m_d3dpp.Windowed = TRUE;
				m_d3dpp.hDeviceWindow = hDestPresent;
				m_d3dpp.MultiSampleQuality = 0;
				m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;					// 显示视频时，不宜使用多重采样，否则将导致画面错乱
	#pragma warning(push)
	#pragma warning(disable:4800)
	#pragma warning(pop)		
				m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
				m_d3dpp.FullScreen_RefreshRateInHz = 0;							// 显示器刷新率，窗口模式该值必须为0
				m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;		// 指定系统如何将后台缓冲区的内容复制到前台缓冲区 D3DSWAPEFFECT_DISCARD:清除后台缓存的内容
			
				m_d3dpp.BackBufferHeight = 0;
				m_d3dpp.BackBufferWidth = 0;
			
				m_d3dpp.EnableAutoDepthStencil = FALSE;							// 关闭自动深度缓存
				/*
				CreateDevice的BehaviorFlags参数选项：
				D3DCREATE_ADAPTERGROUP_DEVICE只对主显卡有效，让设备驱动输出给它所拥有的所有显示输出
				D3DCREATE_DISABLE_DRIVER_MANAGEMENT代替设备驱动来管理资源，这样在发生资源不足时D3D调用不会失败
				D3DCREATE_DISABLE_PRINTSCREEN:不注册截屏快捷键，只对Direct3D 9Ex
				D3DCREATE_DISABLE_PSGP_THREADING：强制计算工作必须在主线程上，vista以上有效
				D3DCREATE_ENABLE_PRESENTSTATS：允许GetPresentStatistics收集统计信息只对Direct3D 9Ex
				D3DCREATE_FPU_PRESERVE；强制D3D与线程使用相同的浮点精度，会降低性能
				D3DCREATE_HARDWARE_VERTEXPROCESSING：指定硬件进行顶点处理，必须跟随D3DCREATE_PUREDEVICE
				D3DCREATE_MIXED_VERTEXPROCESSING：指定混合顶点处理
				D3DCREATE_SOFTWARE_VERTEXPROCESSING：指定纯软的顶点处理
				D3DCREATE_MULTITHREADED：要求D3D是线程安全的，多线程时
				D3DCREATE_NOWINDOWCHANGES：拥有不改变窗口焦点
				D3DCREATE_PUREDEVICE：只试图使用纯硬件的渲染
				D3DCREATE_SCREENSAVER：允许被屏保打断只对Direct3D 9Ex
				D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DCREATE_MIXED_VERTEXPROCESSING, and D3DCREATE_SOFTWARE_VERTEXPROCESSING中至少有一个一定要设置
				*/
			
				if (FAILED(hr = m_pDirect3D9Ex->CreateDeviceEx(D3DADAPTER_DEFAULT,
					D3DDEVTYPE_HAL,
					m_d3dpp.hDeviceWindow,
					vp,
					&m_d3dpp,
					NULL,
					&m_pDirect3DDeviceEx)))
				{
					vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
					if (FAILED(hr = m_pDirect3D9Ex->CreateDeviceEx(D3DADAPTER_DEFAULT,
						D3DDEVTYPE_HAL,
						m_d3dpp.hDeviceWindow,
						vp,
						&m_d3dpp,
						NULL,
						&m_pDirect3DDeviceEx)))
					{
						DxTraceMsg("%s CreateDeviceEx Failed.\thr=%x.\n", __FUNCTION__, hr);
						MessageBox(hWnd, "CreateDeviceEx Failed.", "提示", MB_OK | MB_ICONSTOP);
						return 0;
					}
				}

			}
			else
			{
				if (!g_pDxSurface)
				{
					g_pDxSurface = new CDxSurfaceEx();
					g_pDxSurface->InitD3D(GetDlgItem(hWnd, IDC_STATIC_FRAME), nVideoWidth, nVideoHeight, TRUE);
				}
			}

			break;
		}
		case IDC_BUTTON_CREATEDXSURFACE:
		{
			if (IsDlgButtonChecked(hWnd, IDC_RADIO_D3DAPI) == BST_CHECKED)
			{
				if (!m_pDirect3DDeviceEx)
				{
					EnableWindow(hWnd, FALSE);
					MessageBox(hWnd, "Direct3DDeviceEx Not Created.", "提示", /*MB_OK |*/ MB_ICONSTOP);
					EnableWindow(hWnd, TRUE);
					return 0;
				}
				if (FAILED(hr = m_pDirect3DDeviceEx->CreateOffscreenPlainSurface(1280,
					nVideoHeight,
					(D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
					D3DPOOL_DEFAULT,
					&m_pDirect3DSurfaceRender,
					NULL)))
				{
					DxTraceMsg("%s CreateOffscreenPlainSurface Failed.\thr=%x.\n", __FUNCTION__, hr);
					MessageBox(hWnd, "CreateOffscreenPlainSurface Failed.", "提示", MB_OK | MB_ICONSTOP);
					return 0;
				}
			}
			else
			{
				if (g_pDxSurface)
					MessageBox(hWnd, "DxSurface已创建.", "提示", MB_OK | MB_ICONSTOP);
			}
			
			break;
		}
		case IDC_BUTTON_RENDERYUV:
		{
			if (IsDlgButtonChecked(hWnd, IDC_RADIO_D3DAPI) == BST_CHECKED)
			{
				if (!m_pDirect3DSurfaceRender)
				{
					MessageBox(hWnd, "Direct3DSurfaceRender Failed.", "提示", MB_OK | MB_ICONSTOP);
					return 0;
				}
			}
			else
			{
				if (!g_pDxSurface)
				{
					MessageBox(hWnd, "g_pDxSurface Is Not Create.", "提示", MB_OK | MB_ICONSTOP);
					return 0;
				}
			}
			char szPath[1024] = { 0 };
			GetAppPathA(szPath, 1024);
			strcat(szPath, "\\");
			strcat(szPath, szYUVFile);
			g_hYUVFile = CreateFileA(szPath,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_ARCHIVE,
				NULL);
			if (g_hYUVFile == INVALID_HANDLE_VALUE)
			{
				MessageBox(hWnd, "CreateFileA  Failed.", "提示", MB_OK | MB_ICONSTOP);
				return 0;
			}
			shared_ptr<void> FileClosePtr(g_hYUVFile, CloseHandle);
			DWORD nFileLength = 0;
			if (!(nFileLength = GetFileSize(g_hYUVFile, nullptr)))
			{
				MessageBox(hWnd, "GetFileSize Failed.", "提示", MB_OK | MB_ICONSTOP);
				return 0;
			}
			char *pYUVBuffer = (char *)_aligned_malloc(nFileLength, 16);
			shared_ptr<char> pYUVBufferPtr(pYUVBuffer, _aligned_free);
			DWORD dwBytesreads = 0;
			HWND hDestPresent = nullptr;
			if (IsDlgButtonChecked(hWnd, IDC_RADIO_ZOOMOUT) == BST_CHECKED)
			{
				hDestPresent = GetDlgItem(hWnd, IDC_STATIC_FRAME);
				ShowWindow(hDestPresent, SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FRAME2), SW_HIDE);
			}
			else
			{
				hDestPresent = GetDlgItem(hWnd, IDC_STATIC_FRAME2);
				ShowWindow(hDestPresent, SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FRAME), SW_HIDE);
			}

			RECT rtDest;
			GetWindowRect(hDestPresent, &rtDest);
			ScreenToClient(hDestPresent, (LPPOINT)&rtDest);
			ScreenToClient(hDestPresent, ((LPPOINT)&rtDest) + 1);

			if (ReadFile(g_hYUVFile, pYUVBuffer, nFileLength, &dwBytesreads, nullptr) && dwBytesreads)
			{
				if (IsDlgButtonChecked(hWnd, IDC_RADIO_D3DAPI) == BST_CHECKED)
				{
					D3DLOCKED_RECT d3d_rect;
					D3DSURFACE_DESC Desc;
				
					HRESULT hr = m_pDirect3DSurfaceRender->GetDesc(&Desc);
					hr |= m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
					if (FAILED(hr))
					{
						MessageBox(hWnd, "LockRect Failed.", "提示", MB_OK | MB_ICONSTOP);
						return false;
					}
								
					CopyFrameYV12((byte *)d3d_rect.pBits, d3d_rect.Pitch, pYUVBuffer,nVideoWidth,nVideoHeight);

					hr = m_pDirect3DSurfaceRender->UnlockRect();
					if (FAILED(hr))
					{
						//DxTraceMsg("%s line(%d) IDirect3DSurface9::UnlockRect failed:hr = %08X.\n",__FUNCTION__,__LINE__,hr);
						return false;
					}
				
					IDirect3DSurface9 * pBackSurface = NULL;
					m_pDirect3DDeviceEx->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
					m_pDirect3DDeviceEx->BeginScene();
					hr = m_pDirect3DDeviceEx->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurface);
					if (FAILED(hr))
					{
						m_pDirect3DDeviceEx->EndScene();
						return true;
					}
					pBackSurface->GetDesc(&Desc);
					RECT dstrt = { 0, 0, Desc.Width, Desc.Height };
					RECT srcrt = { 0, 0, nVideoWidth, nVideoHeight };
					hr = m_pDirect3DDeviceEx->StretchRect(m_pDirect3DSurfaceRender, &srcrt, pBackSurface, &dstrt, D3DTEXF_LINEAR);
					SafeRelease(pBackSurface);
					m_pDirect3DDeviceEx->EndScene();
					
					hr |= m_pDirect3DDeviceEx->PresentEx(NULL, &rtDest, hDestPresent, NULL, 0);
				}
				else
				{
					if (g_pDxSurface)
					{
						AVFrame *pFrame = av_frame_alloc();
						pFrame->width = nVideoWidth;
						pFrame->height = nVideoHeight;
						pFrame->data[0] = (uint8_t *)pYUVBuffer;
						pFrame->data[1] = (uint8_t *)&pYUVBuffer[nVideoWidth*nVideoHeight];
						pFrame->data[2] = (uint8_t *)&pYUVBuffer[nVideoWidth*nVideoHeight * 5 / 4];
						pFrame->linesize[0] = nVideoWidth;
						pFrame->linesize[1] = nVideoWidth / 2;
						pFrame->linesize[2] = nVideoWidth / 2;
						pFrame->format = AV_PIX_FMT_YUV420P;
						g_pDxSurface->Render(pFrame,hDestPresent,&rtDest);
						av_free(pFrame);
					}
				}
			}
			
			break;
		}
		case IDC_BUTTON_SENDYUV:
		{
			ShowWindow(hWnd, SW_HIDE);
			HWND hDest = FindWindow(nullptr, _T("DVO SnapShot"));
			if (!hDest)
			{
				MessageBox(nullptr, "找不到指定的窗口", "提示", MB_ICONSTOP);
				return 0;
			}
			char szPath[1024] = { 0 };
			GetAppPathA(szPath, 1024);
			strcat(szPath, "\\");
			strcat(szPath, szYUVFile);
			g_hYUVFile = CreateFileA(szPath,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_ARCHIVE,
				NULL);
			if (g_hYUVFile == INVALID_HANDLE_VALUE)
			{
				MessageBox(hWnd, "CreateFileA  Failed.", "提示", MB_OK | MB_ICONSTOP);
				return 0;
			}
			shared_ptr<void> FileClosePtr(g_hYUVFile, CloseHandle);
			DWORD nFileLength = 0;
			if (!(nFileLength = GetFileSize(g_hYUVFile, nullptr)))
			{
				MessageBox(hWnd, "GetFileSize Failed.", "提示", MB_OK | MB_ICONSTOP);
				return 0;
			}
			char *pYUVBuffer = (char *)_aligned_malloc(nFileLength, 16);
			shared_ptr<char> pYUVBufferPtr(pYUVBuffer, _aligned_free);
			DWORD dwBytesreads = 0;

			if (ReadFile(g_hYUVFile, pYUVBuffer, nFileLength, &dwBytesreads, nullptr) && dwBytesreads)
			{
				int nSize = sizeof(YUVFrame) + nVideoWidth * nVideoHeight * 3 / 2;
				nSize = FFALIGN(nSize, 16);
				YUVFrame *pYUVFrame = (YUVFrame *)malloc(nSize);
				pYUVFrame->nFormat = AV_PIX_FMT_YUV420P;
				pYUVFrame->nFrameLength = nVideoWidth * nVideoHeight * 3 / 2;
				pYUVFrame->nHeight = nVideoHeight;
				pYUVFrame->nWidth = nVideoWidth;
				pYUVFrame->nLineSize[0] = 1280;
				pYUVFrame->nLineSize[1] = 640;
				pYUVFrame->nLineSize[2] = 640;
				SYSTEMTIME sysTime;
				GetLocalTime(&sysTime);
				char szYUVFile[512] = { 0 };
				swprintf(pYUVFrame->szFileName, L"YVU_192.168.3.29_%04d%02d%02d_%02d%02d%02d.jpg",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond);
				byte *pYUV = (byte *)(((byte *)pYUVFrame) + sizeof(YUVFrame));
				memcpy(pYUV, pYUVBuffer, nSize - sizeof(YUVFrame));
				COPYDATASTRUCT cds;
				cds.cbData = FFALIGN(nSize, 16);
				cds.lpData = pYUVFrame;
				LRESULT nResult = SendMessage(hDest, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
				free(pYUVFrame);
				DxTraceMsg("WM_COPYDATA Result = %d.\n", nResult);
			}
			break;
		}
		case IDC_BUTTON_CLOSEDXSURFACE:
		{
			if (IsDlgButtonChecked(hWnd, IDC_RADIO_D3DAPI) == BST_CHECKED)
			{
				if (!m_pDirect3DSurfaceRender)
				{
					MessageBox(hWnd, "Direct3DSurfaceRender Failed.", "提示", MB_OK | MB_ICONSTOP);
					return 0;
				}
				SafeRelease(m_pDirect3DSurfaceRender);
			}
			else
			{
				g_pDxSurface->ReleaseOffSurface();
			}
			break;
		}
		case IDC_BUTTON_FREED3D9:
		{
			if (IsDlgButtonChecked(hWnd, IDC_RADIO_D3DAPI) == BST_CHECKED)
			{
				if (!m_pDirect3DDeviceEx)
				{
					MessageBox(hWnd, "Direct3DDeviceEx Not Created.", "提示", MB_OK | MB_ICONSTOP);
					return 0;
				}
				SafeRelease(m_pDirect3DDeviceEx);
				if (!m_pDirect3D9Ex)
				{
					MessageBox(hWnd, "Direct3D9Ex Not Created.", "提示", MB_OK | MB_ICONSTOP);
					return 0;
				}
				SafeRelease(m_pDirect3D9Ex);
				if (m_hD3D9)
				{
					FreeLibrary(m_hD3D9);
					m_hD3D9 = NULL;
				}
			}
			else
			{
				if (g_pDxSurface)
				{
					delete g_pDxSurface;
					g_pDxSurface = nullptr;
				}
			}

			break;
		}
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
		
	case WM_PAINT:
 		hdc = BeginPaint(hWnd, &ps);
 		// TODO: Add any drawing code here...
 		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
// 	default:
// 		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
