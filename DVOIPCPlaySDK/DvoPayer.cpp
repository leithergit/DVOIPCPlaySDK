#include "DvoPlayer.h"

CAvRegister CDvoPlayer::avRegister;
//shared_ptr<CDSound> CDvoPlayer::m_pDsPlayer = make_shared<CDSound>(nullptr);
shared_ptr<CSimpleWnd> CDvoPlayer::m_pWndDxInit = make_shared<CSimpleWnd>();	///< 视频显示时，用以初始化DirectX的隐藏窗口对象