#include "DxSurface.h"
WndSurfaceMap	CDxSurface::m_WndSurfaceMap;
//WNDPROC	CDxSurface::m_pOldWndProc = NULL;
CriticalSectionPtr CDxSurface::m_WndSurfaceMapcs = shared_ptr<CriticalSectionWrap>(new CriticalSectionWrap());
