#include "DvoPlayer.h"

CAvRegister CDvoPlayer::avRegister;
//shared_ptr<CDSound> CDvoPlayer::m_pDsPlayer = make_shared<CDSound>(nullptr);
shared_ptr<CSimpleWnd> CDvoPlayer::m_pWndDxInit = make_shared<CSimpleWnd>();	///< ��Ƶ��ʾʱ�����Գ�ʼ��DirectX�����ش��ڶ���