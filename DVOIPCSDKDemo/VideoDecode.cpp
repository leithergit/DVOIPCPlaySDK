
#include "VideoDecode.h"
CFullScreen CVideoPlayer::m_ArrayFullScreen[_Max_Monitors];
int		CVideoPlayer::	m_nFullScreenInUsing = 0;// ����ʹ�õ�ȫ����ʾ������
CRITICAL_SECTION	CVideoPlayer::m_csArrayFullScreen;