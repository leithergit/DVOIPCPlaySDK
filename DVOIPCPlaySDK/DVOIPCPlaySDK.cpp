/////////////////////////////////////////////////////////////////////////
/// @file  DVOIPCPlaySDK.cpp
/// Copyright (c) 2015, 上海迪维欧科技
/// All rights reserved.  
/// @brief DVOIPC相机播放SDK实现
/// @version 1.0  
/// @author  xionggao.lee
/// @date  2015.12.17
///   
/// 修订说明：最初版本 
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

//DVO IPC 返回流包头
struct DVOIPC_StreamHeader
{
	UINT		    chn;	            //视频输入通道号，取值0~MAX-1.
	UINT		    stream;             //码流编号：0:主码流，1：子码流，2：第三码流。
	UINT		    frame_type;         //帧类型，范围参考APP_NET_TCP_STREAM_TYPE
	UINT		    frame_num;          //帧序号，范围0~0xFFFFFFFF.
	UINT		    sec;	            //时间戳,单位：秒，为1970年以来的秒数。
	UINT		    usec;               //时间戳,单位：微秒。
	UINT		    rev[2];
};

///	@brief			用于播放DVO私有格式的录像文件
///	@param [in]		szFileName		要播放的文件名
///	@param [in]		hWnd			显示图像的窗口
///	@return			若操作成功，返回一个DVO_PLAYHANDLE类型的播放句柄，所有后续播
///	放函数都要使用些接口，若操作失败则返回NULL,错误原因可参考
///	GetLastError的返回值
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

///	@brief			用于播放DVO私有格式的录像文件
///	@param [in]		szFileName		要播放的文件名
///	@param [in]		hWnd			显示图像的窗口
///	@return			若操作成功，返回一个DVO_PLAYHANDLE类型的播放句柄，所有后续播
///	放函数都要使用些接口，若操作失败则返回NULL,错误原因可参考
///	GetLastError的返回值
DVOIPCPLAYSDK_API DVO_PLAYHANDLE	dvoplay_OpenFileW(IN HWND hWnd, IN WCHAR *szFileName, FilePlayProc pPlayCallBack, void *pUserPtr)
{
	if (!hWnd || !IsWindow(hWnd) || !szFileName || !PathFileExistsW(szFileName))
		return nullptr;
	char szFilenameA[MAX_PATH] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, szFileName, -1, szFilenameA, MAX_PATH, NULL, NULL);
	return dvoplay_OpenFileA(hWnd, szFilenameA, pPlayCallBack,pUserPtr);
}

///	@brief			初始化流播放句柄,仅用于流播放
/// @param [in]		hUserHandle		网络连接句柄
///	@param [in]		hWnd			显示图像的窗口
/// @param [in]		nMaxFramesCache	流播放时允许最大视频帧数缓存数量
///	@return			若操作成功，返回一个DVO_PLAYHANDLE类型的播放句柄，所有后续播
///	放函数都要使用些接口，若操作失败则返回NULL,错误原因可参考GetLastError的返回值
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

/// @brief			关闭播放句柄
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		bRefresh		关闭播放器时，是否刷新画面
///	-# true			刷新画面以背景色填充
///	-# false		不刷新画面,保留最后一帧的图像
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			关闭播放句柄会导致播放进度完全终止，相关内存全部被释放,要再度播放必须重新打开文件或流数据
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

/// @brief			输入流数据
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			1	流缓冲区已满
/// @retval			-1	输入参数无效
/// @remark			播放流数据时，相应的帧数据其实并未立即播放，而是被放了播放队列中，应该根据dvoplay_PlayStream
///					的返回值来判断，是否继续播放，若说明队列已满，则应该暂停播放
DVOIPCPLAYSDK_API int dvoplay_InputStream(IN DVO_PLAYHANDLE hPlayHandle, unsigned char *szFrameData, int nFrameSize)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->InputStream(szFrameData, nFrameSize);
}

/// @brief			输入流相机实时码流
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			1	流缓冲区已满
/// @retval			-1	输入参数无效
/// @remark			播放流数据时，相应的帧数据其实并未立即播放，而是被放了播放队列中，应该根据dvoplay_PlayStream
///					的返回值来判断，是否继续播放，若说明队列已满，则应该暂停播放
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

/// @brief			开始播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
///	@param [in]		bEnableAudio	是否播放音频
///	-# true			播放声音
///	-# false		关闭声音
/// @param [in]		bEnableHaccel	是否开启硬解码
/// #- true			开启硬解码功能
/// #- false		关闭硬解码功能
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			当开启硬解码，而显卡不支持对应的视频编码的解码时，会自动切换到软件解码的状态,可通过
///					dvoplay_GetHaccelStatus判断是否已经开启硬解码
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

/// @brief			使视频适应窗口
/// @param [in]		bFitWindow		视频是否适应窗口
/// #- true			视频填满窗口,这样会把图像拉伸,可能会造成图像变形
/// #- false		只按图像原始比例在窗口中显示,超出比例部分,则以原始背景显示
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

/// @brief			停止播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
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

/// @brief			暂停文件播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			这是一个开关函数，若当前句柄已经处于播放状态，首次调用dvoplay_Pause时，会播放进度则会暂停
///					再次调用时，则会再度播放
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

/// @brief			获取码流类型
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		pVideoCodec		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [out]	pAudioCodec	返回当前hPlayHandle是否已开启硬解码功能
/// @remark 码流类型定义请参考:@see DVO_CODEC
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @retval			DVO_Error_PlayerNotStart	播放器尚未启动,无法取得播放过程的信息或属性
DVOIPCPLAYSDK_API int dvoplay_GetCodec(IN DVO_PLAYHANDLE hPlayHandle, DVO_CODEC *pVideoCodec, DVO_CODEC *pAudioCodec)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->GetCodec(pVideoCodec, pAudioCodec);
}

/// @brief			开启硬解码功能
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		bEnableHaccel	是否开启硬解码功能
/// #- true			开启硬解码功能
/// #- false		关闭硬解码功能
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			开启和关闭硬解码功能必须要dvoplay_Start之前调用才能生效
DVOIPCPLAYSDK_API int  dvoplay_EnableHaccel(IN DVO_PLAYHANDLE hPlayHandle, IN bool bEnableHaccel/* = false*/)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->EnableHaccel(bEnableHaccel);
}

/// @brief			获取硬解码状态
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [out]	bEnableHaccel	返回当前hPlayHandle是否已开启硬解码功能
/// #- true			已开启硬解码功能
/// #- false		未开启硬解码功能
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
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

/// @brief			检查是否支持特定视频编码的硬解码
/// @param [in]		nCodec		视频编码格式,@see DVO_CODEC
/// @retval			true		支持指定视频编码的硬解码
/// @retval			false		不支持指定视频编码的硬解码
DVOIPCPLAYSDK_API bool  dvoplay_IsSupportHaccel(IN DVO_CODEC nCodec)
{
	return CDvoPlayer::IsSupportHaccel(nCodec);
}

/// @brief			返回流播放时，当前播放队列中的视频帧的数量
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			播放流数据时，相应的帧数据其实并未立即播放，而是被放了播放队列中，应该根据dvoplay_PlayStream
///					的返回值来判断，是否继续播放，若说明队列已满，则应该暂停播放
DVOIPCPLAYSDK_API int dvoplay_GetCacheSize(IN DVO_PLAYHANDLE hPlayHandle, int &nCacheCount)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->GetCacheSize(nCacheCount);
}

/// @brief			取得文件播放的视频帧率
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [out]	nFPS			返回文件中视频的帧率
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_GetFps(IN DVO_PLAYHANDLE hPlayHandle, OUT int &nFPS)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->GetFps(nFPS);
}

/// @brief			取得文件中包括的有效视频帧总数量
/// @param [in]		hPlayHandle		由dvoplay_OpenFile返回的播放句柄
/// @param [out]	nFrames			返回文件中视频的总帧数
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
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

/// @brief			取得当前播放视频的帧信息
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [out]	pFilePlayInfo	文件播放的相关信息，参见@see FilePlayInfo
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_GetFilePlayInfo(IN DVO_PLAYHANDLE hPlayHandle,OUT FilePlayInfo *pFilePlayInfo)
{
	if (!hPlayHandle || !pFilePlayInfo)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->GetFilePlayInfo(pFilePlayInfo);
}

/// @brief			截取正放播放的视频图像
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		szFileName		要保存的文件名
/// @param [in]		nFileFormat		保存文件的编码格式,@see SNAPSHOT_FORMAT定义
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
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

/// @brief			截取正放播放的视频图像
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		szFileName		要保存的文件名
/// @param [in]		nFileFormat		保存文件的编码格式,@see SNAPSHOT_FORMAT定义
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_SnapShotW(IN DVO_PLAYHANDLE hPlayHandle, IN WCHAR *szFileName, IN SNAPSHOT_FORMAT nFileFormat/* = XIFF_JPG*/)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->SnapShot(szFileName, nFileFormat);
}

/// @brief			设置播放的音量
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		nVolume			要设置的音量值，取值范围0~100，为0时，则为静音
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
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

/// @brief			取得当前播放的音量
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [out]	nVolume			当前的音量值，取值范围0~100，为0时，则为静音
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
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

/// @brief			取得当前播放的速度的倍率
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [out]	nPlayRate		当前的播放的倍率,@see PlayRate
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
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

/// @brief			设置当前播放的速度的倍率
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		nPlayRate		当前的播放的倍率,@see PlayRate
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_SetRate(IN DVO_PLAYHANDLE hPlayHandle, IN PlayRate nPlayRate)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->SetRate(nPlayRate);
}

/// @brief			跳跃到指视频帧进行播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile返回的播放句柄
/// @param [in]		nFrameID		要播放帧的起始ID
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			若所指定帧为非关键帧，帧自动移动到就近的关键帧进行播放
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

/// @brief			跳跃到指定时间偏移进行播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		nTimeSet		要播放的起始时间,单位为秒
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			若所指定时间点对应帧为非关键帧，帧自动移动到就近的关键帧进行播放
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

/// @brief			播放下一帧
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			该函数仅适用于单帧播放,该函数功能暂未实现
DVOIPCPLAYSDK_API int  dvoplay_SeekNextFrame(IN DVO_PLAYHANDLE hPlayHandle)
{
	return DVO_Succeed;
}

/// @brief			开/关音频播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		bEnable			是否播放音频
/// -#	true		开启音频播放
/// -#	false		禁用音频播放
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_EnableAudio(IN DVO_PLAYHANDLE hPlayHandle, bool bEnable/* = true*/)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->EnableAudio(bEnable);
}

/// @brief			刷新播放窗口
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			该功能一般用于播放结束后，刷新窗口，把画面置为黑色
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

/// @brief			获取已放时间,单位毫秒
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_GetTimeEplased(IN DVO_PLAYHANDLE hPlayHandle, LONGLONG &nEplaseTime)
{
	if (!hPlayHandle)
		return DVO_Error_InvalidParameters;
	CDvoPlayer *pPlayer = (CDvoPlayer *)hPlayHandle;
	if (pPlayer->nSize != sizeof(CDvoPlayer))
		return DVO_Error_InvalidParameters;
	return pPlayer->GetTimeEplased(nEplaseTime);
}

/// @brief			设置外部绘制回调接口
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		pUserPtr		用户自定义指针，在调用回调时，将会传回此指针
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

/// @brief			设置获取YUV数据回调接口,通过此回调，用户可直接获取解码后的YUV数据
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		pUserPtr		用户自定义指针，在调用回调时，将会传回此指针
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

/// @brief			设置获取YUV数据回调接口,通过此回调，用户可直接获取解码后的YUV数据
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		pUserPtr		用户自定义指针，在调用回调时，将会传回此指针
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

/// @brief			设置DVO私用格式录像，帧解析回调,通过此回，用户可直接获取原始的帧数据
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		pUserPtr		用户自定义指针，在调用回调时，将会传回此指针
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

/// @brief			生成一个DVO录像文件头
/// @param [in,out]	pMediaHeader	由用户提供的用以接收DVO录像文件头的缓冲区
/// @param [in,out]	pHeaderSize		指定用户提供的用缓冲区的长度，若操作成功，则返回已生成的DVO录像文件头长度
/// @param [in]		nFPS			视频的帧率
/// @param [in]		nAudioCodec		音频的编码类型
/// @param [in]		nVideoCodec		视频的编译类型
/// @remark		    若pMediaHeader为NULL,则pHeaderSize只返回所需缓冲区的长度
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
	
	// 音频和视频编码的类型必须限定下以下范围内
	if (nAudioCodec <= CODEC_G711A ||
		nAudioCodec > CODEC_AAC||
		nVideoCodec < CODEC_H264||
		nVideoCodec > CODEC_H265)
		return DVO_Error_InvalidParameters;

	DVO_MEDIAINFO *pMedia = new DVO_MEDIAINFO();
	pMedia->nCameraType	 = 0;	// 0为迪维欧相机产生的文件
	pMedia->nFps		 = 0;	// FPS可以文件中推算，因此FPS字段已无意义
	pMedia->nVideoCodec  = nVideoCodec;
	pMedia->nAudioCodec  = nAudioCodec;	
	memcpy(pMediaHeader, pMedia, sizeof(DVO_MEDIAINFO));
	*pHeaderSize = sizeof(DVO_MEDIAINFO);
	delete pMedia;
	return DVO_Succeed;
}

/// @brief			生成一个DVO录像帧
/// @param [in,out]	pMediaFrame		由用户提供的用以接收DVO录像帧的缓冲区
/// @param [in,out]	pFrameSize		指定用户提供的用缓冲区的长度，若操作成功，则返回已生成的DVO录像帧长度
/// @param [in,out]	nFrameID		DVO录像帧的ID，第一帧必须为0，后续帧依次递增，音频帧和视频帧必须分开计数
/// @param [in]		pDVOIpcStream	从DVO IPC得到的码流数据
/// @param [in,out]	nStreamLength	输入时为从DVO IPC得到的码流数据长度，输出时为码流数据去头后的长度,即裸码流的长度
/// @remark		    若pMediaFrame为NULL,则pFrameSize只返回DVO录像帧长度
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
	case 0:									// 海思I帧号为0，这是固件的一个BUG，有待修正
	case APP_NET_TCP_COM_DST_IDR_FRAME: 	// IDR帧
	case APP_NET_TCP_COM_DST_I_FRAME:		// I帧
		FrameHeader.nType = FRAME_I;
		break;
	case APP_NET_TCP_COM_DST_P_FRAME:       // P帧
	case APP_NET_TCP_COM_DST_B_FRAME:       // B帧
	case APP_NET_TCP_COM_DST_711_ALAW:      // 711 A律编码帧
	case APP_NET_TCP_COM_DST_711_ULAW:      // 711 U律编码帧
	case APP_NET_TCP_COM_DST_726:           // 726编码帧
	case APP_NET_TCP_COM_DST_AAC:           // AAC编码帧
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