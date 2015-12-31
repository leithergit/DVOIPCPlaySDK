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
#include "dvoipcnetsdk.h"
#include "DVOIPCPlaySDK.h"
#include "DvoPlayer.h"

///	@brief			���ڲ���DVO˽�и�ʽ��¼���ļ�
///	@param [in]		szFileName		Ҫ���ŵ��ļ���
///	@param [in]		hWnd			��ʾͼ��Ĵ���
///	@return			�������ɹ�������һ��DVO_PLAYHANDLE���͵Ĳ��ž�������к�����
///	�ź�����Ҫʹ��Щ�ӿڣ�������ʧ���򷵻�NULL,����ԭ��ɲο�
///	GetLastError�ķ���ֵ
DVOIPCPLAYSDK_API DVO_PLAYHANDLE	dvoplay_OpenFileA(IN HWND hWnd, IN char *szFileName)
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
	return  _New CDvoPlayer(hWnd, szFileName);
}

///	@brief			���ڲ���DVO˽�и�ʽ��¼���ļ�
///	@param [in]		szFileName		Ҫ���ŵ��ļ���
///	@param [in]		hWnd			��ʾͼ��Ĵ���
///	@return			�������ɹ�������һ��DVO_PLAYHANDLE���͵Ĳ��ž�������к�����
///	�ź�����Ҫʹ��Щ�ӿڣ�������ʧ���򷵻�NULL,����ԭ��ɲο�
///	GetLastError�ķ���ֵ
DVOIPCPLAYSDK_API DVO_PLAYHANDLE	dvoplay_OpenFileW(IN HWND hWnd, IN WCHAR *szFileName)
{
	if (!hWnd || !IsWindow(hWnd) || !szFileName || !PathFileExistsW(szFileName))
		return nullptr;
	char szFilenameA[MAX_PATH] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, szFileName, -1, szFilenameA, MAX_PATH, NULL, NULL);
	return dvoplay_OpenFileA(hWnd, szFilenameA);
}

///	@brief			��ʼ�������ž��,������������
/// @param [in]		szStreamHeader	DVO˽�и�ʽ��¼���ļ�ͷ
/// @param [in]		nHeaderSize		DVO¼���ļ�ͷ�ĳ���
///	@param [in]		hWnd			��ʾͼ��Ĵ���
/// @param [in]		nMaxFramesCache	������ʱ���������Ƶ֡����������
///	@return			�������ɹ�������һ��DVO_PLAYHANDLE���͵Ĳ��ž�������к�����
///	�ź�����Ҫʹ��Щ�ӿڣ�������ʧ���򷵻�NULL,����ԭ��ɲο�GetLastError�ķ���ֵ
DVOIPCPLAYSDK_API DVO_PLAYHANDLE	dvoplay_OpenStream(IN HWND hWnd, CHAR *szStreamHeader, int nHeaderSize, IN int nMaxFramesCache)
{
	if (!hWnd || !IsWindow(hWnd) || !szStreamHeader || !nHeaderSize)
		return nullptr;
	CDvoPlayer *pPlayer = _New CDvoPlayer(hWnd);
	if (!pPlayer)
		return nullptr;
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

/// @brief			�رղ��ž��
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
/// @remark			�رղ��ž���ᵼ�²��Ž�����ȫ��ֹ������ڴ�ȫ�����ͷ�,Ҫ�ٶȲ��ű������´��ļ���������
DVOIPCPLAYSDK_API int dvoplay_Close(IN DVO_PLAYHANDLE hPlayHandle)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
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
DVOIPCPLAYSDK_API int dvoplay_Start(IN DVO_PLAYHANDLE hPlayHandle, IN bool bEnableAudio/* = false*/, bool bEnableHaccel/* = false*/)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->StartPlay(bEnableAudio, bEnableHaccel);
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
	nFPS = pPlayer->GetFps();
	return DVO_Succeed;
}

/// @brief			ȡ���ļ��а�������Ч��Ƶ֡������
/// @param [in]		hPlayHandle		��dvoplay_OpenFile���صĲ��ž��
/// @param [out]	nFrames			�����ļ�����Ƶ����֡��
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_GetFrames(IN DVO_PLAYHANDLE hPlayHandle, OUT int nFrames)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	if (pPlayer->IsFilePlayer())
	{
		nFrames = pPlayer->GetFrames();
		return DVO_Succeed;
	}
	else
		return DVO_Error_NotFilePlayer;
}

/// @brief			ȡ�õ�ǰ������Ƶ��֡ID
/// @param [in]		hPlayHandle		��dvoplay_OpenFile��dvoplay_OpenStream���صĲ��ž��
/// @param [out]	nFrameID		���ص�ǰ������Ƶ��֡ID
/// @retval			0	�����ɹ�
/// @retval			-1	���������Ч
DVOIPCPLAYSDK_API int  dvoplay_GetCurrentFrameID(IN DVO_PLAYHANDLE hPlayHandle, OUT int &nFramesID)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	nFramesID = pPlayer->GetCurrentFrameID();
	return DVO_Succeed;
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
DVOIPCPLAYSDK_API int  dvoplay_GetEplaseTime(IN DVO_PLAYHANDLE hPlayHandle, LONGLONG &nEplaseTime)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	nEplaseTime = pPlayer->GetEplaseTime();
	return DVO_Succeed;
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