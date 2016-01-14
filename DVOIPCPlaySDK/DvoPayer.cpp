#include "DvoPlayer.h"

CAvRegister CDvoPlayer::avRegister;
shared_ptr<CDSoundPlayer> CDvoPlayer::m_pDsPlayer = make_shared<CDSoundPlayer>(nullptr);
shared_ptr<CSimpleWnd> CDvoPlayer::m_pWndDxInit = make_shared<CSimpleWnd>();	///< 视频显示时，用以初始化DirectX的隐藏窗口对象