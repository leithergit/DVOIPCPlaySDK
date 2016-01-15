/////////////////////////////////////////////////////////////////////////
/// @file  DVOIPCPlaySDK.cpp
/// Copyright (c) 2015, �Ϻ���άŷ�Ƽ�
/// All rights reserved.  
/// @brief DVOIPC�������SDKʵ��
/// @version 1.0  
/// @author  xionggao.lee
/// @date  2015.12.17
///   
/// �޶�˵��������汾 
/////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <list>
#include <assert.h>
#include <memory>
#include <Shlwapi.h>
#include "DVOIPCPlaySDK.h"
#include "DvoPlayer.h"

using namespace std;
using namespace std::tr1;

//DVO IPC ��������ͷ
struct DVOIPC_StreamHeader
{
	UINT		    chn;	            //��Ƶ����ͨ���ţ�ȡֵ0~MAX-1.
	UINT		    stream;             //������ţ�0:��������1����������2������������
	UINT		    frame_type;         //֡���ͣ���Χ�ο�APP_NET_TCP_STREAM_TYPE
	UINT		    frame_num;          //֡��ţ���Χ0~0xFFFFFFFF.
	UINT		    sec;	            //ʱ���,��λ���룬Ϊ1970��������������
	UINT		    usec;               //ʱ���,��λ��΢�롣
	UINT		    rev[2];
};

///	@brief			���ڲ���DVO˽�и�ʽ��¼���ļ�
///	@param [in]		szFileName		Ҫ���ŵ��ļ���
///	@param [in]		hWnd			��ʾͼ��Ĵ���
///	@return			�������ɹ�������һ��DVO_PLAYHANDLE���͵Ĳ��ž�������к�����
///	�ź�����Ҫʹ��Щ�ӿڣ�������ʧ���򷵻�NULL,����ԭ��ɲο�
///	GetLastError�ķ���ֵ
DVOIPCPLAYSDK_API DVO_PLAYHANDLE	dvoplay_OpenFileA(IN HWND hWnd, IN char *szFileName, FilePlayProc pPlayCallBack, void *pUserPtr)
{
	if (!hWnd || !IsWindow(hWnd) || !szFileName)
	{
		SetLastError(DVO_Error_InvalidParameters);
		return nullptr;
	}
	if (!PathFileExistsA(szFileName))
	{
		SetLastError(ERROR_FILE_NOT_FOUND);
		return nullptr;
	}
	CDvoPlayer *pPlayer = _New CDvoPlayer(hWnd, szFileName);
	if (pPlayer)
	{
		pPlayer->SetPlayCallBack(pPlayCallBack,pUserPtr);
		return pPlayer;
	}
	else
		return nullptr;
}

///	@brief			���ڲ���DVO˽�и�ʽ��¼���ļ�
///	@param [in]		szFileName		Ҫ���ŵ��ļ���
///	@param [in]		hWnd			��ʾͼ��Ĵ���
///	@return			�������ɹ�������һ��DVO_PLAYHANDLE���͵Ĳ��ž�������к�����
///	�ź�����Ҫʹ��Щ�ӿڣ�������ʧ���򷵻�NULL,����ԭ��ɲο�
///	GetLastError�ķ���ֵ
DVOIPCPLAYSDK_API DVO_PLAYHANDLE	dvoplay_OpenFileW(IN HWND hWnd, IN WCHAR *szFileName, FilePlayProc pPlayCallBack, void *pUserPtr)
{
	if (!hWnd || !IsWindow(hWnd) || !szFileName || !PathFileExistsW(szFileName))
		return nullptr;
	char szFilenameA[MAX_PATH] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, szFileName, -1, szFilenameA, MAX_PATH, NULL, NULL);
	return dvoplay_OpenFileA(hWnd, szFilenameA, pPlayCallBack,pUserPtr);
}

///	@brief			��ʼ�������ž��,������������
/// @param [in]		hUserHandle		�������Ӿ��
///	@param [in]		hWnd			��ʾͼ��Ĵ���
/// @param [in]		nMaxFramesCache	������ʱ���������Ƶ֡����������
///	@return			�������ɹ�������һ��DVO_PLAYHANDLE���͵Ĳ��ž�������к�����
///	�ź�����Ҫʹ��Щ�ӿڣ�������ʧ���򷵻�NULL,����ԭ��ɲο�GetLastError�ķ���ֵ
DVOIPCPLAYSDK_API DVO_PLAYHANDLE	dvoplay_OpenStream(IN HWND hWnd, CHAR *szStreamHeader, int nHeaderSize, IN int nMaxFramesCache)
{
	if (!hWnd || !IsWindow(hWnd) /*|| !szStreamHeader || !nHeaderSize*/)
		return nullptr;
	CDvoPlayer *pPlayer = _New CDvoPlayer(hWnd);
	if (!pPlayer)
		return nullptr;
	if (szStreamHeader && nHeaderSize)
	{
		int nDvoError = pPlayer->SetStreamHeader(szStreamHeader, nHeaderSize, nMaxFramesCache);
		if (nDvoError == DVO_Succeed)
			return pPlayer;
		else
		{
			SetLastError(nDvoError);
			delete pPlayer;
			return nullptr;
		}
	}
	else
		return pPlayer;
}

/// @brief			�رղ��ž��
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		bRefresh		�رղ�����ʱ���Ƿ�ˢ�»���
///	-# true			ˢ�»����Ա���ɫ���
///	-# false		��ˢ�»���,�������һ֡��ͼ��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
/// @remark			�رղ��ž���ᵼ�²��Ž�����ȫ��ֹ������ڴ�ȫ�����ͷ�,Ҫ�ٶȲ��ű������´��ļ���������
DVOIPCPLAYSDK_API int dvoplay_Close(IN DVO_PLAYHANDLE hPlayHandle/*bool bRefresh = true*/)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	//pPlayer->SetRefresh(bRefresh);
	delete pPlayer;
	return 0;
}

/// @brief			����������
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @retval			0	�����ɹ�
/// @retval			1	������������
/// @retval			-1	���������Ч
/// @remark			����������ʱ����Ӧ��֡������ʵ��δ�������ţ����Ǳ����˲��Ŷ����У�Ӧ�ø���dvoplay_PlayStream
///					�ķ���ֵ���жϣ��Ƿ�������ţ���˵��������������Ӧ����ͣ����
DVOIPCPLAYSDK_API int dvoplay_InputStream(IN DVO_PLAYHANDLE hPlayHandle, unsigned char *szFrameData, int nFrameSize)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->InputStream(szFrameData, nFrameSize);
}

/// @brief			���������ʵʱ����
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @retval			0	�����ɹ�
/// @retval			1	������������
/// @retval			-1	���������Ч
/// @remark			����������ʱ����Ӧ��֡������ʵ��δ�������ţ����Ǳ����˲��Ŷ����У�Ӧ�ø���dvoplay_PlayStream
///					�ķ���ֵ���жϣ��Ƿ�������ţ���˵��������������Ӧ����ͣ����
DVOIPCPLAYSDK_API int dvoplay_InputIPCStream(IN DVO_PLAYHANDLE hPlayHandle, IN byte *pFrameData, IN int nFrameType, IN int nFrameLength, int nFrameNum, time_t nFrameTime)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
	{
		assert(false);
		return DVO_Error_InvalidParameters;
	}
	
	return pPlayer->InputStream(pFrameData,nFrameType,nFrameLength,nFrameNum,nFrameTime);
}

/// @brief			��ʼ����
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
///	@param [in]		bEnableAudio	�Ƿ񲥷���Ƶ
///	-# true			��������
///	-# false		�ر�����
/// @param [in]		bEnableHaccel	�Ƿ���Ӳ����
/// #- true			����Ӳ���빦��
/// #- false		�ر�Ӳ���빦��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
/// @remark			������Ӳ���룬���Կ���֧�ֶ�Ӧ����Ƶ����Ľ���ʱ�����Զ��л�����������״̬,��ͨ��
///					dvoplay_GetHaccelStatus�ж��Ƿ��Ѿ�����Ӳ����
DVOIPCPLAYSDK_API int dvoplay_Start(IN DVO_PLAYHANDLE hPlayHandle, 
									IN bool bEnableAudio/* = false*/, 
									IN bool bFitWindow /*= true*/, 
									IN bool bEnableHaccel/* = false*/)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->StartPlay(bEnableAudio, bEnableHaccel,bFitWindow);
}

/// @brief			ʹ��Ƶ��Ӧ����
/// @param [in]		bFitWindow		��Ƶ�Ƿ���Ӧ����
/// #- true			��Ƶ��������,�������ͼ������,���ܻ����ͼ�����
/// #- false		ֻ��ͼ��ԭʼ�����ڴ�������ʾ,������������,����ԭʼ������ʾ
DVOIPCPLAYSDK_API int dvoplay_FitWindow(IN DVO_PLAYHANDLE hPlayHandle, bool bFitWindow )
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	pPlayer->FitWindow(bFitWindow);
	return DVO_Succeed;
}

/// @brief			ֹͣ����
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int dvoplay_Stop(IN DVO_PLAYHANDLE hPlayHandle)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	pPlayer->StopPlay();
	return DVO_Succeed;
}

/// @brief			��ͣ�ļ�����
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
/// @remark			����һ�����غ���������ǰ����Ѿ����ڲ���״̬���״ε���dvoplay_Pauseʱ���Ქ�Ž��������ͣ
///					�ٴε���ʱ������ٶȲ���
DVOIPCPLAYSDK_API int dvoplay_Pause(IN DVO_PLAYHANDLE hPlayHandle)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	pPlayer->Pause();
	return DVO_Succeed;
}

/// @brief			��ȡ��������
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		pVideoCodec		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [out]	pAudioCodec	���ص�ǰhPlayHandle�Ƿ��ѿ���Ӳ���빦��
/// @remark �������Ͷ�����ο�:@see DVO_CODEC
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
/// @retval			DVO_Error_PlayerNotStart	��������δ����,�޷�ȡ�ò��Ź��̵���Ϣ������
DVOIPCPLAYSDK_API int dvoplay_GetCodec(IN DVO_PLAYHANDLE hPlayHandle, DVO_CODEC *pVideoCodec, DVO_CODEC *pAudioCodec)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->GetCodec(pVideoCodec, pAudioCodec);
}

/// @brief			����Ӳ���빦��
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		bEnableHaccel	�Ƿ���Ӳ���빦��
/// #- true			����Ӳ���빦��
/// #- false		�ر�Ӳ���빦��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
/// @remark			�����͹ر�Ӳ���빦�ܱ���Ҫdvoplay_Start֮ǰ���ò�����Ч
DVOIPCPLAYSDK_API int  dvoplay_EnableHaccel(IN DVO_PLAYHANDLE hPlayHandle, IN bool bEnableHaccel/* = false*/)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->EnableHaccel(bEnableHaccel);
}

/// @brief			��ȡӲ����״̬
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [out]	bEnableHaccel	���ص�ǰhPlayHandle�Ƿ��ѿ���Ӳ���빦��
/// #- true			�ѿ���Ӳ���빦��
/// #- false		δ����Ӳ���빦��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_GetHaccelStatus(IN DVO_PLAYHANDLE hPlayHandle, OUT bool &bEnableHaccel)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	bEnableHaccel = pPlayer->GetHaccelStatus();
	return DVO_Succeed;
}

/// @brief			����Ƿ�֧���ض���Ƶ�����Ӳ����
/// @param [in]		nCodec		��Ƶ�����ʽ,@see DVO_CODEC
/// @retval			true		֧��ָ����Ƶ�����Ӳ����
/// @retval			false		��֧��ָ����Ƶ�����Ӳ����
DVOIPCPLAYSDK_API bool  dvoplay_IsSupportHaccel(IN DVO_CODEC nCodec)
{
	return CDvoPlayer::IsSupportHaccel(nCodec);
}

/// @brief			����������ʱ����ǰ���Ŷ����е���Ƶ֡������
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
/// @remark			����������ʱ����Ӧ��֡������ʵ��δ�������ţ����Ǳ����˲��Ŷ����У�Ӧ�ø���dvoplay_PlayStream
///					�ķ���ֵ���жϣ��Ƿ�������ţ���˵��������������Ӧ����ͣ����
DVOIPCPLAYSDK_API int dvoplay_GetCacheSize(IN DVO_PLAYHANDLE hPlayHandle, int &nCacheCount)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->GetCacheSize(nCacheCount);
}

/// @brief			ȡ���ļ����ŵ���Ƶ֡��
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [out]	nFPS			�����ļ�����Ƶ��֡��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_GetFps(IN DVO_PLAYHANDLE hPlayHandle, OUT int &nFPS)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->GetFps(nFPS);
}

/// @brief			ȡ���ļ��а�������Ч��Ƶ֡������
/// @param [in]		hPlayHandle		��dvoplay_OpenFile���صĲ��ž��
/// @param [out]	nFrames			�����ļ�����Ƶ����֡��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_GetFrames(IN DVO_PLAYHANDLE hPlayHandle, OUT int &nFrames)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	if (pPlayer->IsFilePlayer())
		return pPlayer->GetFrames(nFrames);
	else
		return DVO_Error_NotFilePlayer;
}

/// @brief			ȡ�õ�ǰ������Ƶ��֡��Ϣ
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [out]	pFilePlayInfo	�ļ����ŵ������Ϣ���μ�@see FilePlayInfo
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_GetFilePlayInfo(IN DVO_PLAYHANDLE hPlayHandle,OUT FilePlayInfo *pFilePlayInfo)
{
	if (!hPlayHandle || !pFilePlayInfo)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->GetFilePlayInfo(pFilePlayInfo);
}

/// @brief			��ȡ���Ų��ŵ���Ƶͼ��
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		szFileName		Ҫ������ļ���
/// @param [in]		nFileFormat		�����ļ��ı����ʽ,@see SNAPSHOT_FORMAT����
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_SnapShotA(IN DVO_PLAYHANDLE hPlayHandle, IN char *szFileName, IN SNAPSHOT_FORMAT nFileFormat/* = XIFF_JPG*/)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	WCHAR szFileNameW[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, szFileName, -1, szFileNameW, MAX_PATH);
	return dvoplay_SnapShotW(hPlayHandle, szFileNameW, nFileFormat);
}

/// @brief			��ȡ���Ų��ŵ���Ƶͼ��
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		szFileName		Ҫ������ļ���
/// @param [in]		nFileFormat		�����ļ��ı����ʽ,@see SNAPSHOT_FORMAT����
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_SnapShotW(IN DVO_PLAYHANDLE hPlayHandle, IN WCHAR *szFileName, IN SNAPSHOT_FORMAT nFileFormat/* = XIFF_JPG*/)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->SnapShot(szFileName, nFileFormat);
}

/// @brief			���ò��ŵ�����
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		nVolume			Ҫ���õ�����ֵ��ȡֵ��Χ0~100��Ϊ0ʱ����Ϊ����
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_SetVolume(IN DVO_PLAYHANDLE hPlayHandle, IN int nVolume)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	pPlayer->SetVolume(nVolume);
	return DVO_Succeed;
}

/// @brief			ȡ�õ�ǰ���ŵ�����
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [out]	nVolume			��ǰ������ֵ��ȡֵ��Χ0~100��Ϊ0ʱ����Ϊ����
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_GetVolume(IN DVO_PLAYHANDLE hPlayHandle, OUT int &nVolume)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	pPlayer->SetVolume(nVolume);
	return DVO_Succeed;
}

/// @brief			ȡ�õ�ǰ���ŵ��ٶȵı���
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [out]	nPlayRate		��ǰ�Ĳ��ŵı���,@see PlayRate
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_GetRate(IN DVO_PLAYHANDLE hPlayHandle, OUT PlayRate &nPlayRate)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	nPlayRate = (PlayRate)pPlayer->GetRate();
	return DVO_Succeed;
}

/// @brief			���õ�ǰ���ŵ��ٶȵı���
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		nPlayRate		��ǰ�Ĳ��ŵı���,@see PlayRate
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_SetRate(IN DVO_PLAYHANDLE hPlayHandle, IN PlayRate nPlayRate)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->SetRate(nPlayRate);
}

/// @brief			��Ծ��ָ��Ƶ֡���в���
/// @param [in]		hPlayHandle		��dvoplay_OpenFile���صĲ��ž��
/// @param [in]		nFrameID		Ҫ����֡����ʼID
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
/// @remark			����ָ��֡Ϊ�ǹؼ�֡��֡�Զ��ƶ����ͽ��Ĺؼ�֡���в���
DVOIPCPLAYSDK_API int  dvoplay_SeekFrame(IN DVO_PLAYHANDLE hPlayHandle, IN int nFrameID)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	if (pPlayer->IsFilePlayer())
	{
		return pPlayer->SeekFrame(nFrameID);
	}
	else
		return DVO_Error_NotFilePlayer;
}

/// @brief			��Ծ��ָ��ʱ��ƫ�ƽ��в���
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		nTimeSet		Ҫ���ŵ���ʼʱ��,��λΪ��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
/// @remark			����ָ��ʱ����Ӧ֡Ϊ�ǹؼ�֡��֡�Զ��ƶ����ͽ��Ĺؼ�֡���в���
DVOIPCPLAYSDK_API int  dvoplay_SeekTime(IN DVO_PLAYHANDLE hPlayHandle, IN double dfTimeSet)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	if (pPlayer->IsFilePlayer())	
		return pPlayer->SeekTime(dfTimeSet);	
	else
		return DVO_Error_NotFilePlayer;
}

/// @brief			������һ֡
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
/// @remark			�ú����������ڵ�֡����,�ú���������δʵ��
DVOIPCPLAYSDK_API int  dvoplay_SeekNextFrame(IN DVO_PLAYHANDLE hPlayHandle)
{
	return DVO_Succeed;
}

/// @brief			��/����Ƶ����
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		bEnable			�Ƿ񲥷���Ƶ
/// -#	true		������Ƶ����
/// -#	false		������Ƶ����
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_EnableAudio(IN DVO_PLAYHANDLE hPlayHandle, bool bEnable/* = true*/)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->EnableAudio(bEnable);
}

/// @brief			ˢ�²��Ŵ���
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
/// @remark			�ù���һ�����ڲ��Ž�����ˢ�´��ڣ��ѻ�����Ϊ��ɫ
DVOIPCPLAYSDK_API int  dvoplay_Refresh(IN DVO_PLAYHANDLE hPlayHandle)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	pPlayer->Refresh();
	return DVO_Succeed;
}

/// @brief			��ȡ�ѷ�ʱ��,��λ����
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_GetTimeEplased(IN DVO_PLAYHANDLE hPlayHandle, LONGLONG &nEplaseTime)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->GetTimeEplased(nEplaseTime);
}

/// @brief			�����ⲿ���ƻص��ӿ�
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		pUserPtr		�û��Զ���ָ�룬�ڵ��ûص�ʱ�����ᴫ�ش�ָ��
DVOIPCPLAYSDK_API int dvoplay_SetExternDrawCallBack(IN DVO_PLAYHANDLE hPlayHandle, IN void *pExternCallBack,IN void *pUserPtr)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	pPlayer->SetExternDraw(pExternCallBack, pUserPtr);
	return DVO_Succeed;
}

/// @brief			���û�ȡYUV���ݻص��ӿ�,ͨ���˻ص����û���ֱ�ӻ�ȡ������YUV����
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		pUserPtr		�û��Զ���ָ�룬�ڵ��ûص�ʱ�����ᴫ�ش�ָ��
DVOIPCPLAYSDK_API int dvoplay_SetYUVCapture(IN DVO_PLAYHANDLE hPlayHandle, IN void *pCaptureYUV, IN void *pUserPtr)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	pPlayer->SetYUVCapture((CaptureYUV)pCaptureYUV, pUserPtr);
	return DVO_Succeed;
}

/// @brief			���û�ȡYUV���ݻص��ӿ�,ͨ���˻ص����û���ֱ�ӻ�ȡ������YUV����
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		pUserPtr		�û��Զ���ָ�룬�ڵ��ûص�ʱ�����ᴫ�ش�ָ��
DVOIPCPLAYSDK_API int dvoplay_SetYUVCaptureEx(IN DVO_PLAYHANDLE hPlayHandle, IN void *pCaptureYUVEx, IN void *pUserPtr)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	pPlayer->SetYUVCaptureEx((CaptureYUVEx)pCaptureYUVEx, pUserPtr);
	return DVO_Succeed;
}

/// @brief			����DVO˽�ø�ʽ¼��֡�����ص�,ͨ���˻أ��û���ֱ�ӻ�ȡԭʼ��֡����
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [in]		pUserPtr		�û��Զ���ָ�룬�ڵ��ûص�ʱ�����ᴫ�ش�ָ��
DVOIPCPLAYSDK_API int dvoplay_SetFrameParserCallback(IN DVO_PLAYHANDLE hPlayHandle, IN void *pCaptureFrame, IN void *pUserPtr)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;

	pPlayer->SetFrameParserCapture((CaptureFrame)pCaptureFrame, pUserPtr);
	return DVO_Succeed;
}

/// @brief			����һ��DVO¼���ļ�ͷ
/// @param [in,out]	pMediaHeader	���û��ṩ�����Խ���DVO¼���ļ�ͷ�Ļ�����
/// @param [in,out]	pHeaderSize		ָ���û��ṩ���û������ĳ��ȣ��������ɹ����򷵻������ɵ�DVO¼���ļ�ͷ����
/// @param [in]		nFPS			��Ƶ��֡��
/// @param [in]		nAudioCodec		��Ƶ�ı�������
/// @param [in]		nVideoCodec		��Ƶ�ı�������
/// @remark		    ��pMediaHeaderΪNULL,��pHeaderSizeֻ�������軺�����ĳ���
DVOIPCPLAYSDK_API int dvoplay_BuildMediaHeader(INOUT byte *pMediaHeader, INOUT int  *pHeaderSize,IN DVO_CODEC nAudioCodec,IN DVO_CODEC nVideoCodec)
{
	if (!pHeaderSize)
		return DVO_Error_InvalidParameters;
	if (!pMediaHeader)
	{
		*pHeaderSize = sizeof(DVO_MEDIAINFO);
		return DVO_Succeed;
	}
	else if (*pHeaderSize < sizeof(DVO_MEDIAINFO))
		return DVO_Error_BufferSizeNotEnough;
	
	// ��Ƶ����Ƶ��������ͱ����޶������·�Χ��
	if (nAudioCodec <= CODEC_G711A ||
		nAudioCodec > CODEC_AAC||
		nVideoCodec < CODEC_H264||
		nVideoCodec > CODEC_H265)
		return DVO_Error_InvalidParameters;

	DVO_MEDIAINFO *pMedia = new DVO_MEDIAINFO();
	pMedia->nCameraType	 = 0;	// 0Ϊ��άŷ����������ļ�
	pMedia->nFps		 = 0;	// FPS�����ļ������㣬���FPS�ֶ���������
	pMedia->nVideoCodec  = nVideoCodec;
	pMedia->nAudioCodec  = nAudioCodec;	
	memcpy(pMediaHeader, pMedia, sizeof(DVO_MEDIAINFO));
	*pHeaderSize = sizeof(DVO_MEDIAINFO);
	delete pMedia;
	return DVO_Succeed;
}

/// @brief			����һ��DVO¼��֡
/// @param [in,out]	pMediaFrame		���û��ṩ�����Խ���DVO¼��֡�Ļ�����
/// @param [in,out]	pFrameSize		ָ���û��ṩ���û������ĳ��ȣ��������ɹ����򷵻������ɵ�DVO¼��֡����
/// @param [in,out]	nFrameID		DVO¼��֡��ID����һ֡����Ϊ0������֡���ε�������Ƶ֡����Ƶ֡����ֿ�����
/// @param [in]		pDVOIpcStream	��DVO IPC�õ�����������
/// @param [in,out]	nStreamLength	����ʱΪ��DVO IPC�õ����������ݳ��ȣ����ʱΪ��������ȥͷ��ĳ���,���������ĳ���
/// @remark		    ��pMediaFrameΪNULL,��pFrameSizeֻ����DVO¼��֡����
DVOIPCPLAYSDK_API int dvoplay_BuildFrameHeader(OUT byte *pFrameHeader, INOUT int *HeaderSize, IN int nFrameID, IN byte *pDVOIpcStream, IN int &nStreamLength)
{
	if (!pDVOIpcStream || !nStreamLength)
		return sizeof(DVOFrameHeaderEx);
	if (!pFrameHeader)
	{
		*HeaderSize = sizeof(DVOFrameHeaderEx);
		return DVO_Succeed;
	}
	else if (*HeaderSize < sizeof(DVOFrameHeaderEx))
		return DVO_Error_BufferSizeNotEnough;

	DVOIPC_StreamHeader *pStreamHeader = (DVOIPC_StreamHeader *)pDVOIpcStream;
	int nSizeofHeader = sizeof(DVOIPC_StreamHeader);
	byte *pFrameData = (byte *)(pStreamHeader)+nSizeofHeader;
	int nFrameLength = nStreamLength - sizeof(DVOIPC_StreamHeader);
	*HeaderSize = sizeof(DVOFrameHeaderEx);
	
	DVOFrameHeaderEx FrameHeader;	
	switch (pStreamHeader->frame_type)
	{
	case 0:									// ��˼I֡��Ϊ0�����ǹ̼���һ��BUG���д�����
	case APP_NET_TCP_COM_DST_IDR_FRAME: 	// IDR֡
	case APP_NET_TCP_COM_DST_I_FRAME:		// I֡
		FrameHeader.nType = FRAME_I;
		break;
	case APP_NET_TCP_COM_DST_P_FRAME:       // P֡
	case APP_NET_TCP_COM_DST_B_FRAME:       // B֡
	case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A�ɱ���֡
	case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U�ɱ���֡
	case APP_NET_TCP_COM_DST_726:           // 726����֡
	case APP_NET_TCP_COM_DST_AAC:           // AAC����֡
		FrameHeader.nType = pStreamHeader->frame_type;
		break;
	default:
	{
		assert(false);
		break;
	}
	}
	FrameHeader.nTimestamp	 = pStreamHeader->sec * 1000 * 1000 + pStreamHeader->usec;
	FrameHeader.nLength		 = nFrameLength;
	FrameHeader.nFrameTag	 = DVO_TAG;		///< DVO_TAG
	FrameHeader.nFrameUTCTime= time(NULL);
	FrameHeader.nFrameID	 = nFrameID;
	memcpy(pFrameHeader, &FrameHeader, sizeof(DVOFrameHeaderEx));
	nStreamLength			 = nFrameLength;
	
	return DVO_Succeed;
}