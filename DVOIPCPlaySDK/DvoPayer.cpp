#include "DvoPlayer.h"

CAvRegister CDvoPlayer::avRegister;
shared_ptr<CDSoundPlayer> CDvoPlayer::m_pDsPlayer = make_shared<CDSoundPlayer>(nullptr);
shared_ptr<CSimpleWnd> CDvoPlayer::m_pWndDxInit = make_shared<CSimpleWnd>();	///< ��Ƶ��ʾʱ�����Գ�ʼ��DirectX�����ش��ڶ���