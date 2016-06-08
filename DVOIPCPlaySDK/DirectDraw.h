#pragma once

//////////////////////////////////////////////////////////////////////////

#include <DDraw.h>

//////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "DDraw.lib")
#pragma comment(lib, "DxGuid.lib")

//////////////////////////////////////////////////////////////////////////

struct ImageSpace
{
	enum { SPACE_COUNT = 4, };

	PBYTE pBuffer[SPACE_COUNT];
	DWORD dwLineSize[SPACE_COUNT];
};

//////////////////////////////////////////////////////////////////////////

struct FormatYV12
{
	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));

		ddsd.dwSize = sizeof(ddsd);
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.dwWidth = dwWidth;
		ddsd.dwHeight = dwHeight;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_YUV;
		ddsd.ddpfPixelFormat.dwYUVBitCount = 8;
		ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y', 'V', '1', '2');
	}
	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	{
		LPBYTE lpSrcY = imageSpace.pBuffer[0];
		LPBYTE lpSrcU = imageSpace.pBuffer[1];
		LPBYTE lpSrcV = imageSpace.pBuffer[2];

		DWORD dwCopyLength = 0;

		if (lpSrcY != NULL) {
			dwCopyLength = dwWidth;
			for (LONG i = 0; i < dwHeight; ++i) {
				CopyMemory(lpSurface, lpSrcY, dwCopyLength);
				lpSrcY += imageSpace.dwLineSize[0];
				lpSurface += lPitch;
			}
		}
		if (lpSrcV != NULL) {
			dwCopyLength = dwWidth >> 1;
			for (LONG i = 0; i < dwHeight >> 1; ++i) {
				CopyMemory(lpSurface, lpSrcV, dwCopyLength);
				lpSrcV += imageSpace.dwLineSize[2];
				lpSurface += lPitch >> 1;
			}
		}
		if (lpSrcU != NULL) {
			dwCopyLength = dwWidth >> 1;
			for (LONG i = 0; i < dwHeight >> 1; ++i) {
				CopyMemory(lpSurface, lpSrcU, dwCopyLength);
				lpSrcU += imageSpace.dwLineSize[1];
				lpSurface += lPitch >> 1;
			}
		}
	}
};

struct FormatYUY2
{
	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));

		ddsd.dwSize = sizeof(ddsd);
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.dwWidth = dwWidth;
		ddsd.dwHeight = dwHeight;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_YUV;
		ddsd.ddpfPixelFormat.dwYUVBitCount = 8;
		ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y', 'U', 'Y', '2');
	}
	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	{
		LPBYTE lpSrcY = imageSpace.pBuffer[0];

		DWORD dwCopyLength = dwWidth << 1;

		for (LONG i = 0; i < dwHeight; ++i) {
			CopyMemory(lpSurface, lpSrcY, dwCopyLength);
			lpSrcY += imageSpace.dwLineSize[0];
			lpSurface += lPitch;
		}
	}
};

struct FormatUYVY
{
	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));

		ddsd.dwSize = sizeof(ddsd);
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.dwWidth = dwWidth;
		ddsd.dwHeight = dwHeight;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_YUV;
		ddsd.ddpfPixelFormat.dwYUVBitCount = 8;
		ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('U', 'Y', 'V', 'Y');
	}
	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	{
		FormatYUY2::Copy(lpSurface, imageSpace, dwWidth, dwHeight, lPitch);
	}
};

//////////////////////////////////////////////////////////////////////////

struct FormatPAL8
{
	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));

		ddsd.dwSize = sizeof(ddsd);
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.dwWidth = dwWidth;
		ddsd.dwHeight = dwHeight;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 8;
	}
	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	{
		LPBYTE lpSrcY = imageSpace.pBuffer[0];

		DWORD dwCopyLength = dwWidth << 2;

		for (LONG i = 0; i < dwHeight; ++i) {
			CopyMemory(lpSurface, lpSrcY, dwCopyLength);
			lpSrcY += imageSpace.dwLineSize[0];
			lpSurface += lPitch;
		}
	}
};

struct FormatRGB555
{
	enum
	{
		RBitMask = 0x7C00,
		GBitMask = 0x03E0,
		BBitMask = 0x001F,
	};

	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));

		ddsd.dwSize = sizeof(ddsd);
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.dwWidth = dwWidth;
		ddsd.dwHeight = dwHeight;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;

		ddsd.ddpfPixelFormat.dwRBitMask = RBitMask;
		ddsd.ddpfPixelFormat.dwGBitMask = GBitMask;
		ddsd.ddpfPixelFormat.dwBBitMask = BBitMask;

		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	}
	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	{
		PSHORT lpSrcY = (PSHORT)imageSpace.pBuffer[0];
		PSHORT lpDstY = (PSHORT)lpSurface;

		for (LONG i = 0; i < dwHeight; ++i) {
			ZeroMemory(lpDstY, dwWidth * sizeof(SHORT));

			for (LONG j = 0; j < dwWidth; ++j) {
				lpDstY[j] |= (lpSrcY[j] & RBitMask) >> 10;
				lpDstY[j] |= (lpSrcY[j] & GBitMask);
				lpDstY[j] |= (lpSrcY[j] & BBitMask) << 10;
			}

			lpSrcY += imageSpace.dwLineSize[0] / sizeof(SHORT);
			lpDstY += lPitch / sizeof(SHORT);
		}
	}
};

struct FormatRGB565
{
	enum
	{
		RBitMask = 0xF800,
		GBitMask = 0x07E0,
		BBitMask = 0x001F,
	};

	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));

		ddsd.dwSize = sizeof(ddsd);
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.dwWidth = dwWidth;
		ddsd.dwHeight = dwHeight;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;

		ddsd.ddpfPixelFormat.dwRBitMask = RBitMask;
		ddsd.ddpfPixelFormat.dwGBitMask = GBitMask;
		ddsd.ddpfPixelFormat.dwBBitMask = BBitMask;

		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	}
	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	{
		PSHORT lpSrcY = (PSHORT)imageSpace.pBuffer[0];
		PSHORT lpDstY = (PSHORT)lpSurface;

		for (LONG i = 0; i < dwHeight; ++i) {
			ZeroMemory(lpDstY, dwWidth * sizeof(SHORT));

			for (LONG j = 0; j < dwWidth; ++j) {
				lpDstY[j] |= (lpSrcY[j] & RBitMask) >> 11;
				lpDstY[j] |= (lpSrcY[j] & GBitMask);
				lpDstY[j] |= (lpSrcY[j] & BBitMask) << 11;
			}

			lpSrcY += imageSpace.dwLineSize[0] / sizeof(SHORT);
			lpDstY += lPitch / sizeof(SHORT);
		}
	}
};

struct FormatBGR24
{
	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));

		ddsd.dwSize = sizeof(ddsd);
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.dwWidth = dwWidth;
		ddsd.dwHeight = dwHeight;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	}
	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	{
		LPBYTE lpSrcY = imageSpace.pBuffer[0];
		LPDWORD lpDstY = (LPDWORD)lpSurface;

		for (LONG i = 0; i < dwHeight; ++i) {
			ZeroMemory(lpDstY, dwWidth * sizeof(DWORD));

			for (LONG j = 0; j < dwWidth; ++j) {
				lpDstY[j] |= (lpSrcY[3 * j]);
				lpDstY[j] |= (lpSrcY[3 * j + 1] << 8);
				lpDstY[j] |= (lpSrcY[3 * j + 2] << 16);
			}

			lpSrcY += imageSpace.dwLineSize[0];
			lpDstY += lPitch / sizeof(DWORD);
		}
	}
};

struct FormatRGB32
{
	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));

		ddsd.dwSize = sizeof(ddsd);
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.dwWidth = dwWidth;
		ddsd.dwHeight = dwHeight;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	}
	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	{
		LPBYTE lpSrcY = imageSpace.pBuffer[0];
		LPDWORD lpDstY = (LPDWORD)lpSurface;

		for (LONG i = 0; i < dwHeight; ++i) {
			ZeroMemory(lpDstY, dwWidth * sizeof(DWORD));

			for (LONG j = 0; j < dwWidth; ++j) {
				lpDstY[j] |= (lpSrcY[4 * j]);
				lpDstY[j] |= (lpSrcY[4 * j + 1] << 8);
				lpDstY[j] |= (lpSrcY[4 * j + 2] << 16);
				lpDstY[j] |= (lpSrcY[4 * j + 3] << 24);
			}

			lpSrcY += imageSpace.dwLineSize[0];
			lpDstY += lPitch / sizeof(DWORD);
		}
	}
};

struct FormatBGR32
{
	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));

		ddsd.dwSize = sizeof(ddsd);
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.dwWidth = dwWidth;
		ddsd.dwHeight = dwHeight;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	}
	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	{
		LPBYTE lpSrcY = imageSpace.pBuffer[0];
		LPDWORD lpDstY = (LPDWORD)lpSurface;

		for (LONG i = 0; i < dwHeight; ++i) {
			ZeroMemory(lpDstY, dwWidth * sizeof(DWORD));

			for (LONG j = 0; j < dwWidth; ++j) {
				lpDstY[j] |= (lpSrcY[4 * j] << 16);
				lpDstY[j] |= (lpSrcY[4 * j + 1] << 8);
				lpDstY[j] |= (lpSrcY[4 * j + 2]);
				lpDstY[j] |= (lpSrcY[4 * j + 3] << 24);
			}

			lpSrcY += imageSpace.dwLineSize[0];
			lpDstY += lPitch / sizeof(DWORD);
		}
	}
};

//////////////////////////////////////////////////////////////////////////

/// @brief DirectDraw 功能封装类，配合Formatxxx结构，可显示多种像素格式的图象
class CDirectDraw
{
	typedef VOID(*CopyCallback)(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch);

public:
	CDirectDraw()
		: m_hWnd(NULL),
		m_dwWidth(0),
		m_dwHeight(0),
		m_pClipper(NULL),
		m_pDirectDraw(NULL),
		m_pSurfacePrimary(NULL),
		m_pSurfaceOffScreen(NULL),
		m_copyCallback(NULL)
	{
	}
	~CDirectDraw()
	{
		Release();
	}

	HRESULT Draw(const ImageSpace &imageSpace,RECT *pRectClip,RECT *pRectRender,bool bPresent = true)
 	{
		HRESULT hr = E_INVALIDARG;
		RECT rectSrc = { 0 };
		RECT rectDst = { 0 };

		DDSURFACEDESC2 ddOffScreenSurfaceDesc = { 0 };
		if (m_copyCallback == NULL || m_pSurfaceOffScreen == NULL || m_dwWidth == 0 || m_dwHeight == 0) {
			goto Exit;
		}

		ddOffScreenSurfaceDesc.dwSize = sizeof(ddOffScreenSurfaceDesc);
		hr = m_pSurfaceOffScreen->Lock(NULL, &ddOffScreenSurfaceDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
		if (hr != S_OK) {
			goto Exit;
		}

		m_copyCallback((LPBYTE)ddOffScreenSurfaceDesc.lpSurface, imageSpace, ddOffScreenSurfaceDesc.dwWidth, ddOffScreenSurfaceDesc.dwHeight, ddOffScreenSurfaceDesc.lPitch);
		m_pSurfaceOffScreen->Unlock(NULL);
		if (pRectClip)
		{
			CopyRect(&rectSrc, pRectClip);
		}
		else
		{
			rectSrc.right = ddOffScreenSurfaceDesc.dwWidth;
			rectSrc.bottom = ddOffScreenSurfaceDesc.dwHeight;
		}
		
 		if (!pRectRender)
 		{
			::GetClientRect(m_hWnd, &rectDst);
			::ClientToScreen(m_hWnd, (LPPOINT)&(rectDst.left));
			::ClientToScreen(m_hWnd, (LPPOINT)&(rectDst.right));
 		}
 		else
 			CopyRect(&rectDst, pRectRender);

		while (bPresent)
		{
			hr = m_pSurfacePrimary->Blt(&rectDst, m_pSurfaceOffScreen, &rectSrc, DDBLT_WAIT, NULL);
			if (hr == DDERR_SURFACELOST) 
			{
				m_pSurfacePrimary->Restore();
				m_pSurfaceOffScreen->Restore();
			}

			if (hr != DDERR_WASSTILLDRAWING) 
			{
				break;
			}
		}

		hr = S_OK;
	Exit:
		return hr;
	}

	template <typename PixelFormatT>
	HRESULT Create(HWND hWnd, DDSURFACEDESC2 &ddOffScreenSurfaceDesc)
	{
		Release();

		HRESULT hr = DirectDrawCreateEx(NULL, (LPVOID *)&m_pDirectDraw, IID_IDirectDraw7, NULL);
		if (hr != S_OK) {
			goto Exit;
		}

		hr = m_pDirectDraw->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
		if (hr != S_OK) {
			goto Exit;
		}

		{
			DDSURFACEDESC2 ddPrimarySurfaceDesc = { 0 };
			ZeroMemory(&ddPrimarySurfaceDesc, sizeof(ddPrimarySurfaceDesc));
			ddPrimarySurfaceDesc.dwSize = sizeof(ddPrimarySurfaceDesc);
			ddPrimarySurfaceDesc.dwFlags = DDSD_CAPS;
			ddPrimarySurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
			hr = m_pDirectDraw->CreateSurface(&ddPrimarySurfaceDesc, &m_pSurfacePrimary, NULL);
			if (hr != S_OK) {
				goto Exit;
			}
			hr = m_pDirectDraw->CreateClipper(0, &m_pClipper, NULL);
			if (hr != S_OK) {
				goto Exit;
			}
			hr = m_pClipper->SetHWnd(0, hWnd);
			if (hr != S_OK) {
				goto Exit;
			}
			m_pSurfacePrimary->SetClipper(m_pClipper);
		}

		hr = m_pDirectDraw->CreateSurface(&ddOffScreenSurfaceDesc, &m_pSurfaceOffScreen, NULL);
		if (hr != S_OK) {
			goto Exit;
		}

		m_hWnd = hWnd;
		m_dwWidth = ddOffScreenSurfaceDesc.dwWidth;
		m_dwHeight = ddOffScreenSurfaceDesc.dwHeight;
		m_copyCallback = PixelFormatT::Copy;

	Exit:
		if (hr != S_OK) {
			Release();
		}
		return hr;
	}

	VOID Release()
	{
		m_hWnd = NULL;
		m_dwWidth = 0;
		m_dwHeight = 0;
		m_copyCallback = NULL;

		if (m_pClipper != NULL) {
			m_pClipper->Release();
			m_pClipper = NULL;
		}
		if (m_pSurfacePrimary != NULL) {
			m_pSurfacePrimary->SetClipper(NULL);
			m_pSurfacePrimary->Release();
			m_pSurfacePrimary = NULL;
		}
		if (m_pSurfaceOffScreen != NULL) {
			m_pSurfaceOffScreen->SetClipper(NULL);
			m_pSurfaceOffScreen->Release();
			m_pSurfaceOffScreen = NULL;
		}
		if (m_pDirectDraw != NULL) {
			m_pDirectDraw->Release();
			m_pDirectDraw = NULL;
		}
	}

private:
	LPDIRECTDRAW7		 m_pDirectDraw;
	LPDIRECTDRAWCLIPPER  m_pClipper;
	LPDIRECTDRAWSURFACE7 m_pSurfacePrimary;
	LPDIRECTDRAWSURFACE7 m_pSurfaceOffScreen;

	HWND m_hWnd;
	DWORD m_dwWidth;
	DWORD m_dwHeight;

	CopyCallback m_copyCallback;
};
