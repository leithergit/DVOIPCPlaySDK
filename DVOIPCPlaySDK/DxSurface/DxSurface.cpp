#include "DxSurface.h"
WndSurfaceMap	CDxSurface::m_WndSurfaceMap;
//WNDPROC	CDxSurface::m_pOldWndProc = NULL;
CriticalSectionPtr CDxSurface::m_WndSurfaceMapcs = shared_ptr<CriticalSectionWrap>(new CriticalSectionWrap());

int	CDxSurface::m_nObjectCount = 0;
CriticalSectionPtr CDxSurface::m_csObjectCount = shared_ptr<CriticalSectionWrap>(new CriticalSectionWrap());
