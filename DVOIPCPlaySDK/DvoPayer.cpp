#include "DvoPlayer.h"

CAvRegister CDvoPlayer::avRegister;
shared_ptr<CDSoundPlayer> CDvoPlayer::m_pDsPlayer = make_shared<CDSoundPlayer>(nullptr);