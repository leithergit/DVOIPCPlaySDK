
#ifndef _SDK_TYPE_DEF_H_
#define _SDK_TYPE_DEF_H_

#include "dvrdvstypedef.h"

const DWORD			NET_DATA_BUFFER_SIZE = 32*1024*1024;	//������Ƶ�������ݻ�����ָ����С

enum NET_SDK_STREAM_ID
{
	NET_SDK_LIVE_STREAM_ID		= 0,
	NET_SDK_PLAYBACK_STREAM_ID	= 10,
	NET_SDK_BACKUP_STREAM_ID	= 100,
};

#endif
