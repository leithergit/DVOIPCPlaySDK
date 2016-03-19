/////////////////////////////////////////////////////////////////////////
/// @file  DVOIPCPlaySDK.h
/// Copyright (c) 2015, 上海迪维欧科技
/// All rights reserved.  
/// @brief DVOIPC相机播放SDK声明和定义
/// @version 1.0  
/// @author  xionggao.lee
/// @date  2015.12.17
///   
/// 修订说明：最初版本 
/////////////////////////////////////////////////////////////////////////
#pragma once
#include "DVOMedia.h"

/*
MS VC++ 12.0 _MSC_VER = 1800 (Visual C++ 2013)
MS VC++ 11.0 _MSC_VER = 1700 (Visual C++ 2012)
MS VC++ 10.0 _MSC_VER = 1600(Visual C++ 2010)
MS VC++ 9.0 _MSC_VER = 1500
MS VC++ 8.0 _MSC_VER = 1400
MS VC++ 7.1 _MSC_VER = 1310
MS VC++ 7.0 _MSC_VER = 1300
MS VC++ 6.0 _MSC_VER = 1200
MS VC++ 5.0 _MSC_VER = 1100
*/

#if _MSC_VER <= 1500
#define nullptr	NULL
#endif

/// @brief	API导出宏
#ifdef DVOIPCPLAYSDK_EXPORTS
#define DVOIPCPLAYSDK_API __declspec(dllexport)
#else
#define DVOIPCPLAYSDK_API __declspec(dllimport)
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif


enum	DVO_ConnectMode
{
	DVO_TCP = 0,
	DVO_UDP
};

/// @enum SNAPSHOT_FORMAT
/// @brief 截图格式
typedef enum SNAPSHOT_FORMAT
{
	XIFF_BMP = 0,
	XIFF_JPG = 1,
	XIFF_TGA = 2,
	XIFF_PNG = 3,
	XIFF_DDS = 4,
	XIFF_PPM = 5,
	XIFF_DIB = 6,
	XIFF_HDR = 7,       //high dynamic range formats
	XIFF_PFM = 8,       //
	XIFF_FORCE_DWORD = 0x7fffffff
} ;

enum DVO_CALLBACK
{
	ExternDcDraw,	/// 外部DC绘制函数，回调定义详见@see ExternDraw
	ExternDcDrawEx,	/// 外部绘制函数，回调定义详见@see ExternDrawEx
	YUVCapture,		/// YUV数据回调，可得到完整的YUV数据,回调定义详见@see CaptureYUV
	YUVCaptureEx,	/// 扩展的YUV回调，可得到已分离的YUV数据和相应的参数数据,回调定义详见@see CaptureYUVEx
	YUVFilter,		/// YUV过滤器，可得到已分离的YUV数据和相应的参数数据，若对数据作出改变会显示在画面上,回调定义详见@see CaptureYUVEx
	FramePaser,		/// 文件帧解析回调，可得到完整正在播放的文件中的一帧未解码的帧,回调定义详见@see CaptureFrame
	FilePlayer		/// 文件播放回调，可得到关于播放器的一些完整信息,回调定义详见@see FilePlayProc
};

/// @enum PlayRate
/// @brief 播放倍率
// enum PlayRate
// {
// 	Rate_One32th = -32,
// 	Rate_One24th = -24,
// 	Rate_One20th = -20,
// 	Rate_One16th = -16,
// 	Rate_One10th = -10,
// 	Rate_One08th = -8,
// 	Rate_Quarter = -4,
// 	Rate_Half	 = -2,
// 	Rate_01X	 = 1,
// 	Rate_02X	 = 2,
// 	Rate_04X	 = 4,
// 	Rate_08X	 = 8,
// 	Rate_10X	 = 10,
// 	Rate_16X	 = 16,
// 	Rate_20X	 = 20,
// 	Rate_24X	 = 24,
// 	Rate_32X	 = 32
// };

#define		DVO_Succeed						(0)		///< 操作成功
#define		DVO_Error_InvalidParameters		(-1)		///< 无效的参数
#define		DVO_Error_NotDvoVideoFile		(-2)		///< 非DVO录像文件
#define		DVO_Error_NotInputStreamHeader	(-3)		///< 未输入DVO录像文件头
#define		DVO_Error_InvalidSDKVersion		(-4)		///< 录像文件头中的的SDK版本无效
#define		DVO_Error_PlayerNotStart		(-5)		///< 播放器尚未启动,无法取得播放过程的信息或属性
#define		DVO_Error_PlayerHasStart		(-6)		///< 播放器已经启动，不能执行初始化或其它设置操作
#define		DVO_Error_NotFilePlayer			(-7)		///< 这不是一个文件播放对象
#define		DVO_Error_InvalidFrame			(-8)		///< 无效的帧
#define		DVO_Error_InvalidFrameType		(-9)		///< 无效的帧类型
#define		DVO_Error_SummaryNotReady		(-10)	///< 文件摘要信息尚未准备好
#define		DVO_Error_FrameCacheIsFulled	(-11)	///< 视频帧缓冲区已经满
#define		DVO_Error_FileNotOpened			(-12)	///< 尚未打开视频文件
#define		DVO_Error_MaxFrameSizeNotEnough	(-13)	///< 最大帧尺寸不足，可能视频文件中存在超过256K的帧数据,应调用SetMaxFrameSize设置新的帧尺寸上限
#define		DVO_Error_InvalidPlayRate		(-14)	///< 无效的播放倍率
#define		DVO_Error_BufferSizeNotEnough	(-15)	///< 提供的缓冲区长度不足
#define		DVO_Error_VideoThreadNotRun		(-16)	///< 视频解码线程尚未启动或已经退出
#define		DVO_Error_AudioThreadNotRun		(-17)	///< 音频频解码线程尚未启动或已经退出
#define		DVO_Error_ReadFileFailed		(-18)	///< 读文件失败
#define		DVO_Error_FileNotExist			(-19)	///< 文件不存在
#define		DVO_Error_InvalidTimeOffset		(-20)	///< 无效的时间偏移或时间超出文件长度范围
#define		DVO_Error_DecodeFailed			(-21)	///< 解码失败
#define		DVO_Error_InvalidWindow			(-21)	///< 无效的窗口句柄
#define		DVO_Error_AudioFailed			(-22)	///< 音频播放初始化失败(播放设备未就绪)
#define		DVO_Error_DxError				(-23)	///< DirectX 错误
#define		DVO_Error_PlayerIsNotPaused		(-24)	///< 播放器尚未暂停
#define		DVO_Error_InsufficentMemory		(-255)	///< 内存不足

/// @brief 播放器即时信息
struct PlayerInfo
{
	DVO_CODEC	nVideoCodec;
	DVO_CODEC	nAudioCodec;
	USHORT		nVideoWidth;
	USHORT		nVideoHeight;
	BOOL		bAudioEnabled;	///< 是否已开启音频
	UINT		nTotalFrames;	///< 视频总帧数,只有文件播放时才有效
	time_t		tTotalTime;		///< 文件总时长(单位:毫秒),只有文件播放时才有效
	UINT		nCurFrameID;	///< 当前播放视频的帧ID,只有文件播放时才有效
	time_t		tCurFrameTime;	///< 返回当前播放视频的帧相对起点的时间(单位:毫秒)
	time_t		tTimeEplased;	///< 已播放时间(单位:毫秒)
	USHORT		nFPS;			///< 文件或码流中视频的原始帧率
	USHORT		nPlayFPS;		///< 当前播放的帧率
	WORD		nCacheSize;		///< 播放缓存
	WORD		nCacheSize2;	///< 音频缓存
	float		fPlayRate;		///< 播放速率,只有文件播放时才有效
	UINT		nReserver[4];
};
///	@def	DVO_PLAYHANDLE
///	@brief	DVO文件播放句柄

typedef void* 	DVO_PLAYHANDLE;

/// @brief		解码后YVU数据回调
typedef void (__stdcall *CaptureYUV)(DVO_PLAYHANDLE hPlayHandle,
									const unsigned char* pYUV,
									int nSize,
									int nWidth,
									int nHeight,
									INT64 nTime,
									void *pUserPtr);
/// @brief		解码后YVU数据回调
typedef void (__stdcall *CaptureYUVEx)(DVO_PLAYHANDLE hPlayHandle,
									const unsigned char* pY,
									const unsigned char* pU,
									const unsigned char* pV,
									int nStrideY,
									int nStrideUV,
									int nWidth,
									int nHeight,
									INT64 nTime,
									void *pUserPtr);

typedef void(__stdcall *FilePlayProc)(DVO_PLAYHANDLE hPlayHandle,void *pUserPtr);

/// @brief DVO私有录像文件的帧数据解析回调函数
/// @param [in]		Framedata	一帧DVO私有录像的帧数据
/// @parqm [in]		nDataSize	数据的长度
/// @param [in]		pUserPtr	用户自定义指针
/// @remark 若要暂停数据解析，可调用dvoplay_pause函数

typedef void (__stdcall *CaptureFrame)(DVO_PLAYHANDLE hPlayHandle, 
									  const unsigned char *Framedata, 
									  const int nDataSize, 
									  void *pUserPtr);

typedef void(__stdcall *ExternDraw)(HWND hWnd, HDC hDc, RECT rt, void *pUserPtr);

typedef void (__stdcall *ExternDrawEx)(DVO_PLAYHANDLE hPlayHandle, RECT rt, void *pUserPtr);

///	@brief			用于播放DVO私有格式的录像文件
///	@param [in]		szFileName		要播放的文件名
///	@param [in]		hWnd			显示图像的窗口
/// @param [in]		pPlayCallBack	播放时的回调函数指针
/// @param [in]		pUserPtr		供pPlayCallBack返回的用户自定义指针
/// @param [in]		szLogFile		日志文件名,若为null，则不开启日志
///	@return			若操作成功，返回一个DVO_PLAYHANDLE类型的播放句柄，所有后续播
///	放函数都要使用些接口，若操作失败则返回NULL,错误原因可参考
///	GetLastError的返回值
DVOIPCPLAYSDK_API DVO_PLAYHANDLE	dvoplay_OpenFileA(IN HWND hWnd, IN char *szFileName, FilePlayProc pPlayCallBack = nullptr,void *pUserPtr = nullptr,char *szLogFile = nullptr);

///	@brief			用于播放DVO私有格式的录像文件
///	@param [in]		szFileName		要播放的文件名
///	@param [in]		hWnd			显示图像的窗口
/// @param [in]		pPlayCallBack	播放时的回调函数指针
/// @param [in]		pUserPtr		供pPlayCallBack返回的用户自定义指针
/// @param [in]		szLogFile		日志文件名,若为null，则不开启日志
///	@return			若操作成功，返回一个DVO_PLAYHANDLE类型的播放句柄，所有后续播
///	放函数都要使用些接口，若操作失败则返回NULL,错误原因可参考
///	GetLastError的返回值
DVOIPCPLAYSDK_API DVO_PLAYHANDLE	dvoplay_OpenFileW(IN HWND hWnd, IN WCHAR *szFileName, FilePlayProc pPlayCallBack = nullptr, void *pUserPtr = nullptr,char* szLogFile = nullptr);

///	@brief			初始化流播放句柄,仅用于流播放
///	@param [in]		hWnd			显示图像的窗口
/// @param [in]		szStreamHeader	DVO私有格式的录像文件头,播放相机实时码流时，应设置为null
/// @param [in]		nHeaderSize		DVO录像文件头的长度播放相机实时码流时，应设置为0
/// @param [in]		nMaxFramesCache	流播放时允许最大视频帧数缓存数量
/// @param [in]		szLogFile		日志文件名,若为null，则不开启日志
///	@return			若操作成功，返回一个DVO_PLAYHANDLE类型的播放句柄，所有后续播
///	放函数都要使用些接口，若操作失败则返回NULL,错误原因可参考GetLastError的返回值
DVOIPCPLAYSDK_API DVO_PLAYHANDLE	dvoplay_OpenStream(IN HWND hWnd, byte *szStreamHeader, int nHeaderSize, IN int nMaxFramesCache = 128, char *szLogFile = nullptr);

/// @brief			关闭播放句柄
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			关闭播放句柄会导致播放进度完全终止，相关内存全部被释放,要再度播放必须重新打开文件或流数据
DVOIPCPLAYSDK_API int dvoplay_Close(IN DVO_PLAYHANDLE hPlayHandle/*,bool bRefresh = true*/);

/// @brief			开启运行日志
/// @param			szLogFile		日志文件名
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			该函数为开关型函数,默认情况下，不会开启日志,调用此函数后会开启日志，再次调用时则会关闭日志
DVOIPCPLAYSDK_API int				EnableLog(IN DVO_PLAYHANDLE hPlayHandle, char *szLogFile);

/// @brief			输入流DVO私有帧格式码流
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			1	流缓冲区已满
/// @retval			-1	输入参数无效
/// @remark			播放流数据时，相应的帧数据其实并未立即播放，而是被放了播放队列中，应该根据dvoplay_PlayStream
///					的返回值来判断，是否继续播放，若说明队列已满，则应该暂停播放
DVOIPCPLAYSDK_API int dvoplay_InputStream(IN DVO_PLAYHANDLE hPlayHandle,unsigned char *szFrameData,int nFrameSize);

/// @brief			输入流相机实时码流
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			1	流缓冲区已满
/// @retval			-1	输入参数无效
/// @remark			播放流数据时，相应的帧数据其实并未立即播放，而是被放了播放队列中，应该根据dvoplay_PlayStream
///					的返回值来判断，是否继续播放，若说明队列已满，则应该暂停播放
DVOIPCPLAYSDK_API int dvoplay_InputIPCStream(IN DVO_PLAYHANDLE hPlayHandle, IN byte *pFrameData, IN int nFrameType, IN int nFrameLength, int nFrameNum, time_t nFrameTime = 0);

/// @brief			开始播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
///	@param [in]		bEnableAudio	是否播放音频
///	-# true			播放声音
///	-# false		关闭声音
/// @param [in]		bFitWindow		视频是否适应窗口
/// #- true			视频填满窗口,这样会把图像拉伸,可能会造成图像变形
/// #- false		只按图像原始比例在窗口中显示,超出比例部分,则以黑色背景显示
/// @param [in]		bEnableHaccel	是否开启硬解码
/// #- true			开启硬解码功能
/// #- false		关闭硬解码功能
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			当开启硬解码，而显卡不支持对应的视频编码的解码时，会自动切换到软件解码的状态,可通过
///					dvoplay_GetHaccelStatus判断是否已经开启硬解码
DVOIPCPLAYSDK_API int dvoplay_Start(IN DVO_PLAYHANDLE hPlayHandle,  IN bool bEnableAudio = false, bool bFitWindow = true,bool bEnableHaccel = false);

/// @brief 复位播放器,在窗口大小变化较大或要切换播放窗口时，建议复位播放器，否则画面质量可能会严重下降
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		hWnd			显示视频的窗口
/// @param [in]		nWidth			窗口宽度,该参数暂未使用,可设为0
/// @param [in]		nHeight			窗口高度,该参数暂未使用,可设为0
DVOIPCPLAYSDK_API int  dvoplay_Reset(IN DVO_PLAYHANDLE hPlayHandle, HWND hWnd = nullptr, int nWidth = 0, int nHeight = 0);

/// @brief			使视频适应窗口
/// @param [in]		bFitWindow		视频是否适应窗口
/// #- true			视频填满窗口,这样会把图像拉伸,可能会造成图像变形
/// #- false		只按图像原始比例在窗口中显示,超出比例部分,则以原始背景显示
DVOIPCPLAYSDK_API int dvoplay_FitWindow(IN DVO_PLAYHANDLE hPlayHandle, bool bFitWindow = true);

/// @brief			停止播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int dvoplay_Stop(IN DVO_PLAYHANDLE hPlayHandle);

/// @brief			暂停文件播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			这是一个开关函数，若当前句柄已经处于播放状态，首次调用dvoplay_Pause时，会播放进度则会暂停
///					再次调用时，则会再度播放
DVOIPCPLAYSDK_API int dvoplay_Pause(IN DVO_PLAYHANDLE hPlayHandle);

/// @brief			清空播放缓存
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int dvoplay_ClearCache(IN DVO_PLAYHANDLE hPlayHandle);

/// @brief			开启硬解码功能
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		bEnableHaccel	是否开启硬解码功能
/// #- true			开启硬解码功能
/// #- false		关闭硬解码功能
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			开启和关闭硬解码功能必须要dvoplay_Start之前调用才能生效
DVOIPCPLAYSDK_API int  dvoplay_EnableHaccel(IN DVO_PLAYHANDLE hPlayHandle, IN bool bEnableHaccel = false);

/// @brief			获取硬解码状态
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [out]	bEnableHaccel	返回当前hPlayHandle是否已开启硬解码功能
/// #- true			已开启硬解码功能
/// #- false		未开启硬解码功能
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_GetHaccelStatus(IN DVO_PLAYHANDLE hPlayHandle, OUT bool &bEnableHaccel);

/// @brief			检查是否支持特定视频编码的硬解码
/// @param [in]		nCodec		视频编码格式,@see DVO_CODEC
/// @retval			true		支持指定视频编码的硬解码
/// @retval			false		不支持指定视频编码的硬解码
DVOIPCPLAYSDK_API bool  dvoplay_IsSupportHaccel(IN DVO_CODEC nCodec);

/// @brief			取得当前播放视频的帧信息
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [out]	nFrameID		返回当前播放视频的帧ID
/// @param [out]	tTimeStamp		返回当前播放视频的帧相对起点的时间(单位:毫秒)
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_GetPlayerInfo(IN DVO_PLAYHANDLE hPlayHandle, OUT PlayerInfo *pPlayInfo);

/// @brief			截取正放播放的视频图像
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		szFileName		要保存的文件名
/// @param [in]		nFileFormat		保存文件的编码格式,@see SNAPSHOT_FORMAT定义
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_SnapShotA(IN DVO_PLAYHANDLE hPlayHandle, IN char *szFileName, IN SNAPSHOT_FORMAT nFileFormat=XIFF_JPG);

/// @brief			截取正放播放的视频图像
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		szFileName		要保存的文件名
/// @param [in]		nFileFormat		保存文件的编码格式,@see SNAPSHOT_FORMAT定义
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_SnapShotW(IN DVO_PLAYHANDLE hPlayHandle, IN WCHAR *szFileName, IN SNAPSHOT_FORMAT nFileFormat = XIFF_JPG);

/// @brief			设置播放的音量
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		nVolume			要设置的音量值，取值范围0~100，为0时，则为静音
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_SetVolume(IN DVO_PLAYHANDLE hPlayHandle, IN int nVolume);

/// @brief			取得当前播放的音量
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [out]	nVolume			当前的音量值，取值范围0~100，为0时，则为静音
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_GetVolume(IN DVO_PLAYHANDLE hPlayHandle, OUT int &nVolume);

/// @brief			设置当前播放的速度的倍率
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		fPlayRate		当前的播放的倍率,大于1时为加速播放,小于1时为减速播放，不能为0或小于0
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_SetRate(IN DVO_PLAYHANDLE hPlayHandle, IN float fPlayRate);

/// @brief			播放下一帧
/// @retval			0	操作成功
/// @retval			-24	播放器未暂停
/// @remark			该函数仅适用于单帧播放
DVOIPCPLAYSDK_API int  dvoplay_SeekNextFrame(IN DVO_PLAYHANDLE hPlayHandle);

/// @brief			跳跃到指视频帧进行播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		bUpdate			是否更新画面,bUpdate为true则予以更新画面,画面则不更新
/// @param [in]		nFrameID		要播放帧的起始ID
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			1.若所指定时间点对应帧为非关键帧，帧自动移动到就近的关键帧进行播放
///					2.若所指定帧为非关键帧，帧自动移动到就近的关键帧进行播放
///					3.只有在播放暂时,bUpdate参数才有效				
DVOIPCPLAYSDK_API int  dvoplay_SeekFrame(IN DVO_PLAYHANDLE hPlayHandle, IN int nFrameID,bool bUpdate = false);

/// @brief			跳跃到指定时间偏移进行播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		nTimeOffset		要播放的起始时间(单位:毫秒)
/// @param [in]		bUpdate			是否更新画面,bUpdate为true则予以更新画面,画面则不更新
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			1.若所指定时间点对应帧为非关键帧，帧自动移动到就近的关键帧进行播放
///					2.若所指定帧为非关键帧，帧自动移动到就近的关键帧进行播放
///					3.只有在播放暂时,bUpdate参数才有效
///					4.用于单帧播放时只能向前移动
DVOIPCPLAYSDK_API int  dvoplay_SeekTime(IN DVO_PLAYHANDLE hPlayHandle, IN time_t nTimeOffset,bool bUpdate = false);

/// @brief 从文件中读取一帧，读取的起点默认值为0,SeekFrame或SeekTime可设定其起点位置
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [out]	pFrameBuffer	帧数据缓冲区
/// @param [out]	nBufferSize		帧缓冲区的大小
DVOIPCPLAYSDK_API int  dvoplay_GetFrame(IN DVO_PLAYHANDLE hPlayHandle, OUT byte **pFrameBuffer, OUT UINT &nBufferSize);

/// @brief			设置文件播放时,支持的最大视频帧的尺寸,默认最大的视频的尺寸为256K,当视频帧
/// 大于256K时,可能会造文件读取文件错误,因此需要设置视频帧的大小,在dvoplay_Start前调用才有效
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		nMaxFrameSize	最大视频帧的尺寸
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			若所指定时间点对应帧为非关键帧，帧自动移动到就近的关键帧进行播放
DVOIPCPLAYSDK_API int  dvoplay_SetMaxFrameSize(IN DVO_PLAYHANDLE hPlayHandle, IN UINT nMaxFrameSize = 256*1024);

/// @brief			取得文件播放时,支持的最大视频帧的尺寸,默认最大的视频的尺寸为256K,当视频帧
/// 大于256K时,可能会造文件读取文件错误,因此需要设置视频帧的大小,在dvoplay_Start前调用才有效
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		nMaxFrameSize	最大视频帧的尺寸
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			若所指定时间点对应帧为非关键帧，帧自动移动到就近的关键帧进行播放
DVOIPCPLAYSDK_API int  dvoplay_GetMaxFrameSize(IN DVO_PLAYHANDLE hPlayHandle, INOUT UINT &nMaxFrameSize);

/// @brief			开/关音频播放
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		bEnable			是否播放音频
/// -#	true		开启音频播放
/// -#	false		禁用音频播放
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_EnableAudio(IN DVO_PLAYHANDLE hPlayHandle,bool bEnable = true);

/// @brief			刷新播放窗口
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
/// @remark			该功能一般用于播放结束后，刷新窗口，把画面置为黑色
DVOIPCPLAYSDK_API int  dvoplay_Refresh(IN DVO_PLAYHANDLE hPlayHandle);

/// @brief			获取已放时间,单位毫秒
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @retval			0	操作成功
/// @retval			-1	输入参数无效
DVOIPCPLAYSDK_API int  dvoplay_GetTimeEplased(IN DVO_PLAYHANDLE hPlayHandle, LONGLONG &nEplaseTime);

/// @brief			设置获取用户回调接口,通过此回调，用户可通过回调得到一些数据,或对得到的数据作一些处理
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		回调函数的类型 @see DVO_CALLBACK
/// @param [in]		pUserCallBack	回调函数指针
/// @param [in]		pUserPtr		用户自定义指针，在调用回调时，将会传回此指针
DVOIPCPLAYSDK_API int dvoplay_SetCallBack(IN DVO_PLAYHANDLE hPlayHandle, DVO_CALLBACK nCallBackType, IN void *pUserCallBack, IN void *pUserPtr);


/// @brief			设置外部绘制回调接口
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		pUserPtr		用户自定义指针，在调用回调时，将会传回此指针
DVOIPCPLAYSDK_API int dvoplay_SetExternDrawCallBack(IN DVO_PLAYHANDLE hPlayHandle, IN void *pExternCallBack, IN void *pUserPtr);

/// @brief			设置获取YUV数据回调接口,通过此回调，用户可直接获取解码后的YUV数据
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		pUserPtr		用户自定义指针，在调用回调时，将会传回此指针
DVOIPCPLAYSDK_API int dvoplay_SetYUVCapture(IN DVO_PLAYHANDLE hPlayHandle, IN void *pYuvCallBack, IN void *pUserPtr);

/// @brief			设置获取YUV数据回调接口,通过此回调，用户可直接获取解码后的YUV数据
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		pUserPtr		用户自定义指针，在调用回调时，将会传回此指针
DVOIPCPLAYSDK_API int dvoplay_SetYUVCaptureEx(IN DVO_PLAYHANDLE hPlayHandle, IN void *pYuvCallBack, IN void *pUserPtr);

/// @brief			设置DVO私用格式录像，帧解析回调,通过此回，用户可直接获取原始的帧数据
/// @param [in]		hPlayHandle		由dvoplay_OpenFile或dvoplay_OpenStream返回的播放句柄
/// @param [in]		pUserPtr		用户自定义指针，在调用回调时，将会传回此指针
DVOIPCPLAYSDK_API int dvoplay_SetFrameParserCallback(IN DVO_PLAYHANDLE hPlayHandle, IN void *pYuvCallBack, IN void *pUserPtr);

/// @brief			生成一个DVO录像文件头
/// @param [in,out]	pMediaHeader	由用户提供的用以接收DVO录像文件头的缓冲区
/// @param [in,out]	pHeaderSize		指定用户提供的用缓冲区的长度，若操作成功，则返回已生成的DVO录像文件头长度
/// @param [in]		nAudioCodec		音频的编码类型
/// @param [in]		nVideoCodec		视频的编译类型
/// @param [in]		nFPS			视频的帧率
/// @remark		    若pMediaHeader为NULL,则pHeaderSize只返回所需缓冲区的长度
DVOIPCPLAYSDK_API int dvoplay_BuildMediaHeader(INOUT byte *pMediaHeader, INOUT int  *pHeaderSize, IN DVO_CODEC nAudioCodec, IN DVO_CODEC nVideoCodec = CODEC_H264,USHORT nFPS = 25);

/// @brief			生成一个DVO录像帧
/// @param [in,out]	pFrameHeader	由用户提供的用以接收DVO录像帧的缓冲区
/// @param [in,out]	pHeaderSize		指定用户提供的用缓冲区的长度，若操作成功，则返回已生成的DVO录像帧长度
/// @param [in,out]	nFrameID		DVO录像帧的ID，第一帧必须为0，后续帧依次递增，音频帧和视频帧必须分开计数
/// @param [in]		pDVOIpcStream	从DVO IPC得到的码流数据
/// @param [in,out]	nStreamLength	输入时为从DVO IPC得到的码流数据长度，输出时为码流数据去头后的长度,即裸码流的长度
/// @remark		    若pMediaFrame为NULL,则pFrameSize只返回DVO录像帧长度
DVOIPCPLAYSDK_API int dvoplay_BuildFrameHeader(OUT byte *pFrameHeader, INOUT int *pHeaderSize, IN int nFrameID, IN byte *pDVOIpcStream, INOUT int &nStreamLength);

#ifdef _UNICODE
#define dvoplay_OpenFile	dvoplay_OpenFileW
#define dvoplay_SnapShot	dvoplay_SnapShotW
#else
#define dvoplay_OpenFile	dvoplay_OpenFileA
#define dvoplay_SnapShot	dvoplay_SnapShotA
#endif

/// @brief			用于播放DVO私有格式的录像文件
/// @param [in]		szFileName		要播放的文件名
/// @param [in]		hWnd			显示图像的窗口
/// @param [in]		bEnableAudio	是否播放音频，为true时，则播放声音，为false则关闭声音,默认为false
/// @return			若操作成功，返回一个DVO_PLAYHANDLE类型的播放句柄，所有后续播放函数都要使用些接口，若操作失败则返回NULL,错误原因可参考GetLastError的返回值
/// @exception		对抛出的异常进行注释
/// @pre			使用函数前需注意的地方和函数调用的条件
/// @post			使用函数后需注意的地方
/// @par			示例:
/// @code
/// 示例代码及其注释
/// @endcode
/// @since			通常用来说明从什么版本,时间写此部分代码
/// @see			参见其他内容
/// @note			需要注意的问题
/// @deprecated		该函数若有被取消或被替代的可能性,在此说明
