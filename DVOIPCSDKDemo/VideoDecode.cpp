
#include "VideoDecode.h"
CFullScreen CVideoPlayer::m_ArrayFullScreen[_Max_Monitors];
int		CVideoPlayer::	m_nFullScreenInUsing = 0;// 正在使用的全屏显示器数量
CRITICAL_SECTION	CVideoPlayer::m_csArrayFullScreen;