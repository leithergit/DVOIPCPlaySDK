// VideoFrame.cpp : implementation file
//

#include "stdafx.h"
#include "VideoFrame.h"


// CVideoFrame

IMPLEMENT_DYNAMIC(CVideoFrame, CWnd)

CVideoFrame::CVideoFrame()
{
	
}

CVideoFrame::~CVideoFrame()
{
}


BEGIN_MESSAGE_MAP(CVideoFrame, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CVideoFrame message handlers

BOOL CVideoFrame::Create(UINT nID, const RECT& rect,int nRow,int nCol, CWnd* pParentWnd, CCreateContext* pContext)
{
	// can't use for desktop or pop-up windows (use CreateEx instead)
	ASSERT(pParentWnd != NULL);
	ASSERT(nID != 0);
	ASSERT(nRow != 0 && nCol != 0);
	m_nRows = nRow;
	m_nCols = nCol;

	LPCTSTR szWndClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), (HBRUSH) ::GetStockObject(BLACK_BRUSH), NULL);
	return CreateEx(0, szWndClass, _T("VideoFrame"),
		WS_CHILD | WS_VISIBLE,
		rect.left, rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		pParentWnd->GetSafeHwnd(),
		(HMENU)(UINT_PTR)nID,
		(LPVOID)pContext);
}

BOOL CVideoFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~WS_BORDER | ~WS_CAPTION | ~WS_SYSMENU | ~WS_THICKFRAME | ~WS_VSCROLL | ~WS_HSCROLL;
	cs.style |= WS_CHILD | WS_VISIBLE;
	
	return CWnd::PreCreateWindow(cs);
}


int CVideoFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pSelectedPen = new CPen(PS_SOLID, 1, RGB(255, 0, 0));
	m_pUnSelectedPen = new CPen(PS_SOLID, _GRID_LINE_WIDTH, RGB(0, 255, 0));
	
	AdjustPanels(m_nRows, m_nCols);

	return 0;
}

bool CVideoFrame::AdjustPanels(int nRow, int nCols)
{
	ASSERT(nRow != 0 && nCols != 0);
	if (!nRow || !nCols)
		return false;
	if (m_vecPanel.size())
	{
		m_vecPanel.clear();
	}
	m_nRows = nRow;
	m_nCols = nCols;
	for (int nRow = 0; nRow < m_nRows; nRow++)
	{
		for (int nCol = 0; nCol < m_nCols; nCol++)
		{
			m_vecPanel.push_back(PanelInfoPtr(new PanelInfo(nRow, nCol)));
		}
	}
	ResizePanel();
	int nIndex = 0;
	for (int nRow = 0; nRow < m_nRows; nRow++)
	{
		for (int nCol = 0; nCol < m_nCols; nCol++)
		{
			m_vecPanel[nIndex]->hWnd = CreatePanel(nRow, nCol);
			::ShowWindow(m_vecPanel[nIndex++]->hWnd, SW_SHOW);
		}
	}
	Invalidate();
	return true;
}

bool CVideoFrame::AdjustPanels(int nCount)
{
	ASSERT(nCount != 0);
	if (nCount == 0)
		return false;

	float fsqroot = sqrt((float)nCount);
	int nRowCount = floor(fsqroot);
	int nColCount = nRowCount;

	if (nRowCount*nColCount < nCount)
	{
		nColCount++;
		if (nRowCount*nColCount < nCount)
			nRowCount++;
	}

	// 必须保证列数大于行数
	if (nRowCount > nColCount)
	{
		int nTemp = nRowCount;
		nRowCount = nColCount;
		nColCount = nTemp;
	}
	m_nRows = nRowCount;
	m_nCols = nColCount;
	_TraceMsgA("%s Rows = %d\tCols = %d.\n", __FUNCTION__, nRowCount, nColCount);
	return AdjustPanels(m_nRows, m_nCols);
}
void CVideoFrame::DrawGrid(CDC *pDc)
{
	CRect rtClient;
	pDc->SelectObject(m_pUnSelectedPen);
	GetClientRect(&rtClient);
	int nWidth = rtClient.Width();
	int nHeight = rtClient.Height();
	int nAvgColWidth = nWidth / m_nCols;
	int nAvgRowHeight = nHeight / m_nRows;

	int nRemainedWidth = nWidth - nAvgColWidth*m_nCols;		// 平均分配宽度有盈余
	int nRemainedHeight = nHeight - nAvgRowHeight*m_nRows;	// 平均分配高度有盈余

	int nStartX = rtClient.left;
	int nStartY = rtClient.top;

	// 画竖线
	for (int nCol = 0; nCol < m_nCols; nCol++)
	{
		if (nCol > 0 && nRemainedWidth > 0)
		{
			nStartX++;
			nRemainedWidth--;
		}
		if (nCol > 0)
		{
			pDc->MoveTo(nStartX, nStartY);
			pDc->LineTo(nStartX, rtClient.bottom);
		}
		
		nStartX += nAvgColWidth;
	}
	
	// 画横线
	for (int nRow = 0; nRow < m_nRows; nRow++)
	{
		if (nRow > 0 && nRemainedHeight > 0)
		{
			nStartY++;
			nRemainedHeight--;
		}
		if (nRow > 0)
		{
			pDc->MoveTo(rtClient.left, nStartY);
			pDc->LineTo(rtClient.right, nStartY);
		}	
		nStartY += nAvgRowHeight;
	}
	
//#ifdef _DEBUG
//	_TraceMsgA("Index\tLeft\tRight\tTop\t\tBottom.\n");
//	for (int nRow = 0; nRow < m_nRows; nRow++)	
//	{
//		for (int nCol = 0; nCol < m_nCols; nCol++)
//		{
//			int nIndex = nRow*m_nCols + nCol;
//			RECT rtWnd = m_vecPanel[nIndex]->rect;
//			_TraceMsgA("(%d,%d)\t%d\t\t%d\t\t%d\t\t%d\n",nRow,nCol, pRtWnd->left, pRtWnd->right, pRtWnd->top, pRtWnd->bottom);
//		}
//	}
//#endif
}

void CVideoFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	DrawGrid(&dc);
}

void CVideoFrame::OnDestroy()
{
	CWnd::OnDestroy();
	if (m_pSelectedPen)
		delete m_pSelectedPen;
	if (m_pUnSelectedPen)
		delete m_pUnSelectedPen;
}

void CVideoFrame::ResizePanel(int nWidth,int nHeight)
{
	CRect rtClient;
	if (!nWidth || !nHeight )
	{
		GetClientRect(&rtClient);
		if (!nWidth)
			nWidth = rtClient.Width();
		if (!nHeight)
			nHeight = rtClient.Height();
	}
	int nAvgColWidth = nWidth / m_nCols;
	int nAvgRowHeight = nHeight / m_nRows;

	int nRemainedWidth = nWidth - nAvgColWidth*m_nCols;		// 平均分配宽度有盈余
	int nRemainedHeight = nHeight - nAvgRowHeight*m_nRows;	// 平均分配高度有盈余

	int nStartX = 0;
	
	// 画竖线
	for (int nCol = 0; nCol < m_nCols; nCol++)
	{
		if (nCol > 0 && nRemainedWidth > 0)
		{
			nStartX++;
			nRemainedWidth--;
		}
		// 计算每个窗格的左右边界
		for (int nRow = 0; nRow < m_nRows; nRow++)
		{
			m_vecPanel[nRow*m_nCols + nCol]->rect.left = nStartX + _GRID_LINE_WIDTH;
			if (nCol > 0)
				m_vecPanel[nRow*m_nCols + nCol - 1]->rect.right = nStartX - _GRID_LINE_WIDTH;
		}
		nStartX += nAvgColWidth;
	}
	// 修正最后一列的右坐标
	for (int nRow = 0; nRow < m_nRows; nRow++)
		m_vecPanel[nRow*m_nCols + m_nCols - 1]->rect.right = nWidth - _GRID_LINE_WIDTH;
	
	int nStartY = 0;
	// 画横线
	for (int nRow = 0; nRow < m_nRows; nRow++)
	{
		if (nRow > 0 && nRemainedHeight > 0)
		{
			nStartY++;
			nRemainedHeight--;
		}
		// 计算每个空格的上下边界		
		for (int nCol = 0; nCol < m_nCols; nCol++)
		{
			m_vecPanel[nRow*m_nCols + nCol]->rect.top = nStartY + _GRID_LINE_WIDTH;
			if (nRow > 0)
				m_vecPanel[(nRow - 1)*m_nCols + nCol]->rect.bottom = nStartY - _GRID_LINE_WIDTH;
		}
		
		nStartY += nAvgRowHeight;
	}
	// 修正最后一条的底坐标
	for (int nCol = 0; nCol < m_nCols; nCol++)
		m_vecPanel[(m_nRows - 1)*m_nCols + nCol]->rect.bottom = nHeight - _GRID_LINE_WIDTH;

//#ifdef _DEBUG
//	_TraceMsgA("Index\tLeft\tRight\tTop\t\tBottom.\n");
//	for (int nRow = 0; nRow < m_nRows; nRow++)	
//	{
//		for (int nCol = 0; nCol < m_nCols; nCol++)
//		{
//			int nIndex = nRow*m_nCols + nCol;
//			RECT rtWnd = m_vecPanel[nIndex]->rect;
//			_TraceMsgA("(%d,%d)\t%d\t\t%d\t\t%d\t\t%d\n",nRow,nCol, pRtWnd->left, pRtWnd->right, pRtWnd->top, pRtWnd->bottom);
//		}
//	}
//#endif
}

void CVideoFrame::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	ResizePanel(cx, cy);
	for (auto it = m_vecPanel.begin(); it != m_vecPanel.end(); it++)
		(*it)->UpdateWindow();
}
