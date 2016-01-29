#pragma once

#include <assert.h>
#include <vector>
#include <memory>
using namespace  std;
using namespace  std::tr1;

#pragma warning(disable:4244 4018)
#define		_GRID_LINE_WIDTH	2
// CVideoFrame

struct PanelInfo
{
	PanelInfo()
	{
		ZeroMemory(this, sizeof(PanelInfo));
	}
	PanelInfo(int nRowIn, int nColIn)
	{
		ZeroMemory(this, sizeof(PanelInfo));
		nRow = nRowIn;
		nCol = nColIn;
	}
	
	~PanelInfo()
	{
		DestroyWindow(hWnd);
	}
	void UpdateWindow()
	{
		MoveWindow(hWnd, rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top), TRUE);	
	}
	HWND hWnd;
	RECT rect;
	int nIndex;
	int nRow;
	int nCol;
	void *pCustumData;
};
typedef shared_ptr<PanelInfo> PanelInfoPtr;
class CVideoFrame : public CWnd
{
	DECLARE_DYNAMIC(CVideoFrame)

public:
	CVideoFrame();
	virtual ~CVideoFrame();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
private:
	UINT	m_nCols = 1, m_nRows = 1;
	int		m_nCurSelected = -1;
	vector <PanelInfoPtr>m_vecPanel;
	int		m_nPannelUsed = 0;		//  已用空格数量
private:
	void ResizePanel(int nWidth = 0, int nHeight = 0);

	// 绘制窗口
	void DrawGrid(CDC *pdc);

	// 创建创建面板窗口
	HWND CreatePanel(int nRow, int nCol)
	{
		WNDCLASSEX wcex;
		TCHAR *szWindowClass = _T("PanelWnd");
		TCHAR szWndName[256] = { 0 };
		_stprintf_s(szWndName, 256, _T("Panel(%d,%d)"), nRow, nCol);
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wcex.lpfnWndProc = PanelWndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = NULL;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = NULL;
		RegisterClassEx(&wcex);
		RECT* pRtWnd = GetPanelRect(nRow, nCol);
		_TraceMsgA("Rect(%d,%d) = (%d,%d,%d,%d).\n", nRow, nCol, pRtWnd->left, pRtWnd->right, pRtWnd->top, pRtWnd->bottom);
		
		return ::CreateWindow(szWindowClass,	// 窗口类
			szWndName,							// 窗口标题 
			WS_CHILD,							// 窗口风格
			pRtWnd->left, 						// 窗口左上角X坐标
			pRtWnd->top, 						// 窗口左上解Y坐标
			(pRtWnd->right - pRtWnd->left), 	// 窗口宽度
			(pRtWnd->bottom - pRtWnd->top), 	// 窗口高度
			m_hWnd, 							// 父窗口句柄
			NULL,								// 菜单句柄
			NULL,
			NULL);
	}
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	CPen *m_pSelectedPen = nullptr;
	CPen *m_pUnSelectedPen = nullptr;
	afx_msg void OnDestroy();
	virtual BOOL Create(UINT nID, const RECT& rect,int nRow,int nCol,CWnd* pParentWnd,  CCreateContext* pContext = NULL);
	LPRECT GetPanelRect(int nRow, int nCol)
	{
		if ((nRow*m_nCols + nCol) < m_vecPanel.size())		
			return &m_vecPanel[nRow*m_nCols + nCol]->rect;
		else
			return nullptr;
	}

	HWND GetPanelWnd(int nRow, int nCol)
	{
		if ((nRow*m_nCols + nCol) < m_vecPanel.size())
			return m_vecPanel[nRow*m_nCols + nCol]->hWnd;
		else
			return nullptr;
	}

	HWND GetPanelWnd(int nIndex)
	{
		if (nIndex < m_vecPanel.size())
			return m_vecPanel[nIndex]->hWnd;
		else
			return nullptr;
	}

	inline bool SetPanelParam(int nIndex, void *pParam)
	{
		if (nIndex < m_vecPanel.size())
		{
			m_vecPanel[nIndex]->pCustumData = pParam;
			return true;
		}
		else
			return false;
	}

	bool SetPanelParam(int nRow, int nCol, void *pParam)
	{
		return SetPanelParam(nRow*m_nCols + nCol, pParam);
	}
	
	inline void *GetPanelParam(int nIndex)
	{
		if (nIndex < m_vecPanel.size())
			return m_vecPanel[nIndex]->pCustumData;
		else
			return nullptr;
	}
	void *GetPanelParam(int nRow, int nCol)
	{
		return GetPanelParam(nRow*m_nCols + nCol);
	}

	int GetPanelCount()
	{
		return m_vecPanel.size();
	}
	int GetRows(){ return m_nRows; }
	int GetCols(){ return m_nCols; }
	bool AdjustPanels(int nRow, int nCols);
	bool AdjustPanels(int nCount);

#define __countof(array) (sizeof(array)/sizeof(array[0]))
#pragma warning (disable:4996)

	static void _TraceMsgA(LPCSTR pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		int nBuff;
		CHAR szBuffer[0x7fff];
		nBuff = _vsnprintf(szBuffer, __countof(szBuffer), pFormat, args);
		//::wvsprintf(szBuffer, pFormat, args);
		//assert(nBuff >=0);
		OutputDebugStringA(szBuffer);
		va_end(args);
	}
	
	static LRESULT CALLBACK PanelWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		//int wmId, wmEvent;
		PAINTSTRUCT ps;
		HDC hdc;
		switch (message)
		{
		case WM_SYSKEYDOWN:
		{
		}
		break;
		case WM_KEYDOWN:
		{
			switch (wParam)	// 忽略ESC和回车键
			{
			case VK_ESCAPE:
			case VK_RETURN:
			{
				//printf(_T("Capture Escape or Return Key.\n"));
				return 0;
			}
			break;
			}
		}
		break;
		case WM_PAINT:
		{
			hdc = ::BeginPaint(hWnd, &ps);
			::EndPaint(hWnd, &ps);
			break;
		}
		case WM_LBUTTONDBLCLK:	// 双击恢复监视
		{
			_TraceMsgA("%08X\tWM_LBUTTONDBLCLK.\n", hWnd);
			return 0;
		}
		default:
			return ::DefWindowProc(hWnd, message, wParam, lParam);		// 必须要有这一句，不然窗口可能无法创建成功
		}
		return 0l;
	}
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


