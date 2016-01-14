#pragma once
#include <dsound.h>
#include <memory>
// #include <mmstream.h>
// #include <MMReg.h>

//创建基于CWinThread类的线程
//pThreadClass	CWinThread类指针，用于指向创建后的线程对象
//ThreadFunc	线程的执行函数
//Param			线程的入口参数
//ThreadPririty	线程的优先级
#ifndef BeginWinThread
#define		BeginWinThread(pThreadClass,ThreadFunc,Param,ThreadPririty)		{pThreadClass = AfxBeginThread(ThreadFunc,Param,ThreadPririty,0,CREATE_SUSPENDED);pThreadClass->m_bAutoDelete = FALSE;pThreadClass->ResumeThread();}
#endif
using namespace std;
using namespace std::tr1;

#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxerr.lib")

#pragma warning (disable:4018 4996)

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


#ifndef IN
#define	IN
#endif

#ifndef OUT
#define	OUT
#endif

#ifndef INOUT
#define	INOUT
#endif

#ifdef _DEBUG
#define DsTrace	DxTrace
#else
#define DsTrace	
#endif

#define Audio_Play_Segments		20

class CDSoundPlayer
{
public:
#ifdef _DEBUG
	static void DxTrace(LPCSTR pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		int nBuff;
		CHAR szBuffer[4096];
		nBuff = _vsnprintf(szBuffer, 4096, pFormat, args);
		//::wvsprintf(szBuffer, pFormat, args);
		//assert(nBuff >=0);
		OutputDebugStringA(szBuffer);
		va_end(args);	
	}
#endif
	CDSoundPlayer(void)
	{
		m_nBufferPlayLength = 1;	// 默认1秒
		m_nNotificationsNum = 20;	// 把1秒分成25段
		m_pDirectSound = NULL;
		m_pDSBPrimary = NULL;
		m_pDSBSlavely = NULL;
		m_dwDSBufferSize = 0;
		m_dwNotifySize = 0;		
		m_pDSPosNotify = NULL;	
		m_hEventArray = NULL;	
		m_lVolume = 0;
		m_bPause = FALSE;
		m_bMute = FALSE;
		ZeroMemory(&m_wfx,sizeof(WAVEFORMATEX));
		m_wfx.cbSize = sizeof(WAVEFORMATEX);
		m_pWaveBuffer = NULL;
		m_nWaveBufferSize = 0;
		m_nWaveDataLength = 0;
		m_bPlayThreadRun = false;
		m_hPlayThread = NULL;

		InitializeCriticalSection(&m_csWaveBuffer);
		InitializeCriticalSection(&m_csBuffer);
		InitializeCriticalSection(&m_csPlay);
		m_dwLastTick = 0;
		m_dwLastTick1 = 0;
		m_bPlayed = false;
	}

	CDSoundPlayer(HWND hWnd)
	{
		m_nBufferPlayLength = 1;	// 默认1秒
		m_nNotificationsNum = 20;	// 把1秒分成25段
		m_pDirectSound = NULL;
		m_pDSBPrimary = NULL;
		m_pDSBSlavely = NULL;
		m_dwDSBufferSize = 0;
		m_dwNotifySize = 0;
		m_pDSPosNotify = NULL;
		m_hEventArray = NULL;
		m_lVolume = 0;
		m_bPause = FALSE;
		m_bMute = FALSE;
		ZeroMemory(&m_wfx, sizeof(WAVEFORMATEX));
		m_wfx.cbSize = sizeof(WAVEFORMATEX);
		m_pWaveBuffer = NULL;
		m_nWaveBufferSize = 0;
		m_nWaveDataLength = 0;
		m_bPlayThreadRun = false;
		m_hPlayThread = NULL;

		InitializeCriticalSection(&m_csWaveBuffer);
		InitializeCriticalSection(&m_csBuffer);
		InitializeCriticalSection(&m_csPlay);
		InitializeCriticalSection(&m_csDsound);
		
		m_dwLastTick = 0;
		m_dwLastTick1 = 0;
		m_bPlayed = false;
		//Initialize(hWnd, Audio_Play_Segments);
	}

	~CDSoundPlayer(void)
	{
		StopPlay();
		m_bPlayThreadRun = false;
		if (m_hEventArray)
		{
			for (int i = 0;i < m_nNotificationsNum;i ++)
				CloseHandle(m_hEventArray[i]);
			delete []m_hEventArray;
		}
		if (m_hPlayThread)
		{
			if (WaitForSingleObject(m_hPlayThread,1000) == WAIT_TIMEOUT)
				TerminateThread(m_hPlayThread,0xFF);
			CloseHandle(m_hPlayThread);
			m_hPlayThread = NULL;
		}
				
		
		//释放缓冲区
		SAFE_RELEASE(m_pDSBPrimary);
		SAFE_RELEASE(m_pDSBSlavely);
		//释放directsound
		// 为何会在释放m_pDirectSound对象时锁住,暂且不释放,日后有时间再查这个问题
		// xioggao.lee @2015.10.28
		SAFE_RELEASE(m_pDirectSound);
		SAFE_DELETE_ARRAY(m_pDSPosNotify);
		
		if (m_hWnd)
			::PostMessage(m_hWnd,WM_DESTROY,0,0);
		DeleteCriticalSection(&m_csWaveBuffer);
		DeleteCriticalSection(&m_csBuffer);
		DeleteCriticalSection(&m_csPlay);
	}

	BOOL IsPlaying()
	{
		BOOL bIsPlaying = FALSE;	
		if(m_pDSBSlavely == NULL )
			return FALSE; 
		DWORD dwStatus = 0;
		m_pDSBSlavely->GetStatus( &dwStatus );
		bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
		return bIsPlaying;
	}
	
	//设置播放缓冲区的长度，以秒为单位,如果不设置，默认的是3秒
	inline void SetBufferLength(int nSecond = 1)
	{
		m_nBufferPlayLength = nSecond;
	}
	inline DWORD GetBufferLength()
	{
		return m_nBufferPlayLength;
	}
	//设置该长度的播放缓冲区的通知事件数量,如果不设置，默认的是25个
	inline void SetNotificationsNum(int nCount = 25)
	{
		m_nNotificationsNum = nCount;
	}

	inline DWORD GetNotificationsNum()
	{
		return m_nNotificationsNum;
	}

	// 创建一个简单的不可见的窗口
	// 用于在用户未提供窗口句柄时，为DsSound提供窗口

	HWND CreateSimpleWindow(IN HINSTANCE hInstance = NULL)
	{
		WNDCLASSEX wcex;
		TCHAR *szWindowClass = _T("DirectX Sound Window");
		wcex.cbSize			= sizeof(WNDCLASSEX);
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= DefWindowProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= LoadIcon(hInstance, IDC_ICON);
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= szWindowClass;
		wcex.hIconSm		= LoadIcon(hInstance, IDC_ICON);

		RegisterClassEx(&wcex);

		return ::CreateWindow(szWindowClass, 	// 窗口类
							szWindowClass,		// 窗口标题 
							WS_OVERLAPPEDWINDOW,// 窗口风格
							CW_USEDEFAULT, 		// 窗口左上角X坐标
							0, 					// 窗口左上解Y坐标
							32, 				// 窗口宽度
							32, 					// 窗口高度
							NULL, 				// 父窗口句柄
							NULL,				// 菜单句柄
							hInstance, 			
							NULL);
	}
	
	//////////////////////////////////////////////////////////////////////////
	// 初始化，主要是生成播放缓冲区和设置通知事件
	//bGlobalFocus如果为TRUE，在失去焦点时仍然播放
	//////////////////////////////////////////////////////////////////////////	
	BOOL Initialize(HWND hWnd,int nNotifyCount = 16,int nPlayTime = 1/*Second*/)
	{
		if (!TryEnterCriticalSection(&m_csDsound))
			return FALSE;
		shared_ptr<CRITICAL_SECTION> autoLeaveSection(&m_csDsound, ::LeaveCriticalSection);
		if (m_pDirectSound)
			return TRUE;

		LRESULT hr;	
		if(m_pDSBSlavely)
		{
			m_pDSBSlavely->Stop();
			SAFE_RELEASE(m_pDSBSlavely);
		}

		if(m_pDirectSound)
			SAFE_RELEASE(m_pDirectSound);
		
		//创建directsound
		if(FAILED(hr = DirectSoundCreate(NULL, &m_pDirectSound, NULL)))
		{
			DsTrace("CDSoundPlayer::Initialize() DirectSoundCreate Failed.\n");
			return FALSE;
		}
		
		if (!hWnd || !IsWindow(hWnd))
		{// 若无窗口，则创建内部小窗口
			m_hWnd = CreateSimpleWindow(NULL);
			if (m_hWnd)
			{
				if (FAILED(m_pDirectSound->SetCooperativeLevel(m_hWnd, DSSCL_PRIORITY)))
				{
					DsTrace("CDSoundPlayer::Initialize() SetCooperativeLevel Failed.\n");
					return FALSE;
				}				
			}
			else
			{
				DsTrace("CDSoundPlayer::Initialize() CreateSimpleWindow Failed.\n");
				return FALSE;
			}
		}
		else if (FAILED(m_pDirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
		{
			DsTrace("CDSoundPlayer::Initialize() SetCooperativeLevel Failed.\n");
			return FALSE;
		}

		//创建主缓冲区			
		// Get the primary buffer 				
		ZeroMemory( &m_wfx, sizeof(WAVEFORMATEX) ); 
		m_wfx.cbSize			= 0;
		m_wfx.wFormatTag		= (WORD) WAVE_FORMAT_PCM;		// 音频格式
		m_wfx.nChannels			= 1;							// 音频通道数量
		m_wfx.nSamplesPerSec	= 8000;						// 采样率
		m_wfx.wBitsPerSample	= 16;							
		m_wfx.nBlockAlign		= (WORD) ((m_wfx.wBitsPerSample / 8) * m_wfx.nChannels);
		m_wfx.nAvgBytesPerSec	= (DWORD) (m_wfx.nSamplesPerSec * m_wfx.nBlockAlign);
		
		m_nNotificationsNum		= nNotifyCount;
		m_nBufferPlayLength		= nPlayTime;

		//定义一个m_nBufferPlayLength秒的缓冲区,并将这个缓冲区分成m_nNotificationsNum个通知块;
		m_dwNotifySize = m_wfx.nSamplesPerSec*m_nBufferPlayLength*m_wfx.nBlockAlign/m_nNotificationsNum;
		//确保m_dwNotifySize是nBlockAlign的倍数
		m_dwNotifySize = m_dwNotifySize - m_dwNotifySize%m_wfx.nBlockAlign;
		//m_dwNotifySize = 192000;
		//m_nNotificationsNum = 4;
		m_dwDSBufferSize = m_dwNotifySize*m_nNotificationsNum;
		
		DSBUFFERDESC dsbd;
		ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
		dsbd.dwSize        = sizeof(DSBUFFERDESC);
		dsbd.dwFlags       = DSBCAPS_GLOBALFOCUS|
							 DSBCAPS_CTRLVOLUME|
							 DSBCAPS_CTRLPOSITIONNOTIFY |
							 DSBCAPS_GETCURRENTPOSITION2;
							 //DSBCAPS_PRIMARYBUFFER;
		dsbd.dwBufferBytes = m_dwDSBufferSize;
		dsbd.lpwfxFormat   = &m_wfx;

		if( FAILED( hr = m_pDirectSound->CreateSoundBuffer( &dsbd, &m_pDSBPrimary, NULL ) ))
		{
			DsTrace("CDSoundPlayer::Initialize() CreateSoundBuffer Failed.\n");
			SAFE_DELETE(m_pDirectSound);
			return FALSE;
		}

		//创建副冲区
		if (FAILED(m_pDSBPrimary->QueryInterface(IID_IDirectSoundBuffer,(LPVOID*)&m_pDSBSlavely)))
		{
			DsTrace("CDSoundPlayer::Initialize() Create Slave Sound buffer Failed.\n");
			SAFE_DELETE(m_pDSBPrimary);
			SAFE_DELETE(m_pDirectSound);
			return FALSE;
		}

		IDirectSoundNotify *m_pDSNotify = NULL;
		if(FAILED(m_pDSBSlavely->QueryInterface(IID_IDirectSoundNotify,(LPVOID*)&m_pDSNotify)))
		{
			DsTrace("CDSoundPlayer::Initialize() m_pDSBSlavely->QueryInterface Failed.\n");
			
			SAFE_DELETE(m_pDSBSlavely);
			SAFE_DELETE(m_pDSBPrimary);
			SAFE_DELETE(m_pDirectSound);
			return FALSE ;
		}

		m_pDSPosNotify = new DSBPOSITIONNOTIFY[m_nNotificationsNum];
		m_hEventArray = new HANDLE[m_nNotificationsNum];
		for(int i = 0;i < m_nNotificationsNum;i ++)
		{
			m_pDSPosNotify[i].dwOffset =i*m_dwNotifySize;
			m_hEventArray[i] = ::CreateEvent(NULL,false,false,NULL); 
			m_pDSPosNotify[i].hEventNotify = m_hEventArray[i];
		}
		m_pDSNotify->SetNotificationPositions(m_nNotificationsNum,m_pDSPosNotify);
		m_pDSNotify->Release();	
		m_pDSNotify = NULL;
		// 启动播放线程
		UINT nThreadAddr = 0;
		m_bPlayThreadRun = true;
		
		m_hPlayThread = (HANDLE )_beginthreadex(NULL,0,PlayThread,this,0,NULL);
		//m_hPlayThread = (HANDLE)_beginthreadex(NULL,0,PlayThread,this,NULL,&nThreadAddr);
		return TRUE;
	}

	inline DWORD WaitForPosNotify()
	{
		return WaitForMultipleObjects (m_nNotificationsNum, m_hEventArray, FALSE, INFINITE);
	}

#define DSBPLAY_ONCE 0x00000000
	// 尝试进入播放流程，可以通用GetBuffer()取得缓冲区，往缓冲区中添加音频数据
	// 返回0时，进入播放流程失败，可能其它线程已经进入播放流程
	// 否则返回单次可填充音频缓冲区的长度
	// 线程或进程退时，必须调用StopPlay以退出播放流程，否则可能造成死锁
	DWORD StartPlay()
	{
		// 每次重新播放时音频缓冲区都应清空
		EnterCriticalSection(&m_csWaveBuffer);
		if (m_nWaveBufferSize < m_dwNotifySize*2 )	// 需要扩大缓冲区
		{
			m_pWaveBuffer.reset();
			m_nWaveBufferSize = m_dwNotifySize*2;
			m_pWaveBuffer = shared_ptr<byte>(new byte [m_nWaveBufferSize]);
		}		
		m_nWaveDataLength = 0;
		LeaveCriticalSection(&m_csWaveBuffer);

		if (!TryEnterCriticalSection(&m_csPlay))
			return 0;
		
		if (m_pDSBSlavely)
		{
			m_pDSBSlavely->SetCurrentPosition(0);
			m_pDSBSlavely->Play(0,0,DSBPLAY_LOOPING);
			m_bPlayed = true;
		}
		return m_dwNotifySize;
	}
	// 退出播放流程
	BOOL StopPlay()
	{
		if (!m_bPlayed)
			return FALSE;
		
		m_bPlayed = false;
		LeaveCriticalSection(&m_csPlay);
		
		if(m_pDSBSlavely == NULL)
			return FALSE;
		m_bPause = FALSE;	
		HRESULT hr = m_pDSBSlavely->Stop();
		if(FAILED(hr))
		{
			DsTrace("CDSoundPlayer::Stop() m_pDSBSlavely->Stop() Failed,hr = 0x%08X.\n",hr);
			return FALSE;
		}
		hr = m_pDSBSlavely->SetCurrentPosition(0L);
		if(FAILED(hr))
		{
			DsTrace("CDSoundPlayer::Stop() m_pDSBSlavely->SetCurrentPosition() Failed,hr = 0x%08X.\n",hr);
			return FALSE;
		}
		return TRUE;
	}

	inline BOOL GetBuffer(DWORD nOffset,void **pBuffer,DWORD &nBufferLength)
	{
		if (TryEnterCriticalSection(&m_csBuffer))
		{
			shared_ptr<CRITICAL_SECTION> autoLeaveSection(&m_csBuffer,::LeaveCriticalSection);
			if (m_pDSBSlavely)
			{
				HRESULT hr = m_pDSBSlavely->Lock(nOffset,m_dwNotifySize,pBuffer,&nBufferLength,NULL,NULL,0);
				
				if (FAILED (hr))
				{
					DsTrace("CDSoundPlayer::GetBuffer() m_pDSBSlavely->Lock() Failed.\n");
					return FALSE;
				}
				else
					return TRUE;
			}
			else
				return FALSE;
		}
		else
			return FALSE;
	}

	inline void ReleaseBuffer(void *pBuffer,int nBufferLength)
	{
		if (TryEnterCriticalSection(&m_csBuffer))
		{
			if (m_pDSBSlavely && pBuffer && nBufferLength) 
			{
				HRESULT hr = m_pDSBSlavely->Unlock(pBuffer,nBufferLength,NULL,0);
				if (FAILED(hr))
					DsTrace("CDSoundPlayer::ReleaseBuffer() m_pDSBSlavely->Unlock() Failed.\n");
			}
			::LeaveCriticalSection(&m_csBuffer);
		}
	}

	void Pause()
	{
		if(m_pDSBSlavely == NULL)
		{
			return;
		}

		m_pDSBSlavely->Stop();
		m_bPause = TRUE;

	}
	
	void SetMute(BOOL bMute)
	{
		if(m_pDSBSlavely == NULL)
		{
			return;
		}

		m_bMute = bMute;
		if(bMute)
		{
			m_pDSBSlavely->SetVolume(-10000);
		}
		else
		{
			m_pDSBSlavely->SetVolume(m_lVolume);
		}

	}

	// 音量范围-10000,0
	// -10000	静音,0	最大音量

	void SetVolume(long lVolume)
	{
		if(m_pDSBSlavely == NULL)
		{
			return;
		}

		m_lVolume = lVolume;
		if(m_bMute == FALSE)
		{
			m_pDSBSlavely->SetVolume(m_lVolume);	
		}
	}
	bool IsInitialized()
	{
		EnterCriticalSection(&m_csDsound);
		shared_ptr<CRITICAL_SECTION> autoLeaveSection(&m_csDsound, ::LeaveCriticalSection);
		return (m_pDirectSound != nullptr);
	}
	int  GetVolume()
	{
		return m_lVolume;
	}
	BOOL GetMute()
	{
		return m_bMute;
	}

	BOOL GetPause()
	{
		return m_bPause;
	}
	
	int GetWaveDataLength()
	{
		EnterCriticalSection(&m_csWaveBuffer);
		int nRetval = m_nWaveDataLength;
		LeaveCriticalSection(&m_csWaveBuffer);
		return nRetval;
	}
	// 从音频缓冲区取出完整的音频数据
	int GetWaveData(INOUT byte *pWaveBuffer,IN int nBufferSize)
	{
// 		DWORD dwSpan = GetTickCount() - m_dwLastTick;
// 		DsTrace("%s TimeSpan = %d.\n",__FUNCTION__,dwSpan);
// 		m_dwLastTick = GetTickCount();
		EnterCriticalSection(&m_csWaveBuffer);
		
		shared_ptr<CRITICAL_SECTION> autoLeaveSection(&m_csWaveBuffer,::LeaveCriticalSection);
		if (m_nWaveDataLength >= m_dwNotifySize)
		{
			byte *pWaveBufferPtr = m_pWaveBuffer.get();
			memcpy_s(pWaveBuffer,nBufferSize,pWaveBufferPtr,m_dwNotifySize);
			// 把后部未使用数据前移
			memmove(pWaveBufferPtr,&pWaveBufferPtr[m_dwNotifySize],m_nWaveDataLength - m_dwNotifySize);
			m_nWaveDataLength -= m_dwNotifySize;
			return m_dwNotifySize;
		}
		else
			return 0;
	}

	void PutWaveData(IN byte *pWaveData,IN int nDataLength)
	{
#ifdef _DEBUG
		if (m_dwLastTick1 == 0)
		{
			m_nAudioFrames = 1;
			m_dwLastTick1 = GetTickCount();
			m_dwLastTick = m_dwLastTick1;
		}
		else
		{
			m_nAudioFrames ++;
			DWORD dwSpan = GetTickCount() - m_dwLastTick;
			m_dwLastTick = GetTickCount();
			DWORD dwTotalSpan = m_dwLastTick - m_dwLastTick1;
			//DsTrace("%s TimeSpan = \t%d\tAvg(FrameInterval) = %d.\n",__FUNCTION__,dwSpan,dwTotalSpan/m_nAudioFrames);
		}
#endif
		EnterCriticalSection(&m_csWaveBuffer);
		
		byte *pWaveBufferPtr = m_pWaveBuffer.get();
		if ((m_nWaveDataLength + nDataLength) > m_nWaveBufferSize)
		{// 添加新数据后，如果会挤爆原缓冲区,则要废弃原缓冲区最前的一段内容
			int nDiscardLen = m_nWaveDataLength + nDataLength - m_nWaveBufferSize;		// 要废弃的数据长度
			int nMoveLen = m_nWaveDataLength - nDiscardLen;
			memmove(pWaveBufferPtr,&pWaveBufferPtr[nDiscardLen],nMoveLen);
			memcpy(&pWaveBufferPtr[nMoveLen],pWaveData,nDataLength);
			m_nWaveDataLength = m_nWaveBufferSize;
		}
		else
		{
			memcpy(&pWaveBufferPtr[m_nWaveDataLength],pWaveData,nDataLength);
			m_nWaveDataLength += nDataLength;
		}
		LeaveCriticalSection(&m_csWaveBuffer);
	}

private:
// 	BOOL PlayBuffer()
// 	{
// 		HRESULT hr;
// 		VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
// 		DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
// 		DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 
// 
// 		if(m_pDSBuffer == NULL)
// 		{
// 			return FALSE;
// 		}
// 		if(m_bPause == TRUE) 
// 		{
// 			if(RestoreBuffer() == FALSE)
// 			{
// 				DsTrace("CDSoundPlayer::PlayBuffer() RestoreBuffer Failed.\n");
// 				return FALSE;
// 			}
// 			m_bPause = FALSE;
// 		}
// 		else
// 		{
// 			m_bFillNextNotificationWithSilence = FALSE;
// 			m_dwPlayProgress = 0;
// 			m_dwLastPlayPos = 0;
// 			m_dwNextWriteOffset = 0;
// 
// 			//如果缓冲区丢失需要恢复
// 			DWORD dwStatus;
// 			if( FAILED( hr = m_pDSBuffer->GetStatus( &dwStatus ) ) )		
// 			{
// 				DsTrace("CDSoundPlayer::PlayBuffer() m_pDSBuffer->GetStatus() Failed.\n");
// 				return FALSE;
// 			}
// 			if( dwStatus & DSBSTATUS_BUFFERLOST )
// 			{
// 				// Since the app could have just been activated, then
// 				// DirectSound may not be giving us control yet, so 
// 				// the restoring the buffer may fail.  
// 				// If it does, sleep until DirectSound gives us control.
// 				do 
// 				{
// 					hr = m_pDSBuffer->Restore();
// 					if( hr == DSERR_BUFFERLOST )
// 						Sleep( 10 );
// 				}
// 				while( ( hr = m_pDSBuffer->Restore() ) == DSERR_BUFFERLOST );
// 
// 			}
// 
// 			//向缓冲区写数据
// 			m_pDSBuffer->Stop();
// 			m_pDSBuffer->SetCurrentPosition(0L);	
// 			if(FAILED(hr = m_pDSBuffer->Lock(0, m_dwDSBufferSize, &pDSLockedBuffer,	&dwDSLockedBufferSize,NULL,NULL,0L)))
// 			{
// 				DsTrace("CDSoundPlayer::PlayBuffer() m_pDSBuffer->Lock() Failed.\n");				
// 				return FALSE;
// 			}
// 
// 			if(FAILED(hr = m_pDSBuffer->Unlock(pDSLockedBuffer,	dwDSLockedBufferSize,NULL,NULL)))
// 			{
// 				DsTrace("CDSoundPlayer::PlayBuffer() m_pDSBuffer->Unlock() Failed.\n");	
// 				return FALSE;
// 			}
// 
// 		}
// 
// 		//不加这两行再开发播放时不能设置音量，不知道原因
// 		m_pDSBuffer->SetVolume(-10000);
// 		m_pDSBuffer->SetVolume(m_lVolume);
// 
// 		SetMute(m_bMute);
// 		SetVolume(m_lVolume);	
// 		m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING );
// 
// 	}
// 	BOOL RestoreBuffer()
// 	{
// 		HRESULT hr; 
// 		VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
// 		DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
// 		DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 
// 		//如果缓冲区丢失需要恢复
// 		DWORD dwStatus;
// 		if( FAILED( hr = m_pDSBuffer->GetStatus( &dwStatus ) ) )		
// 		{
// 			e_log.WriteLog("m_pDSBuffer->GetStatus()失败\n");
// 			return FALSE;
// 		}
// 		if( dwStatus & DSBSTATUS_BUFFERLOST )
// 		{
// 			// Since the app could have just been activated, then
// 			// DirectSound may not be giving us control yet, so 
// 			// the restoring the buffer may fail.  
// 			// If it does, sleep until DirectSound gives us control.
// 			do 
// 			{
// 				hr = m_pDSBuffer->Restore();
// 				if( hr == DSERR_BUFFERLOST )
// 					Sleep( 10 );
// 			}
// 			while( ( hr = m_pDSBuffer->Restore() ) == DSERR_BUFFERLOST );
// 
// 
// 			//向缓冲区写数据
// 			if(FAILED(hr = m_pDSBuffer->Lock(0, m_dwDSBufferSize, &pDSLockedBuffer,
// 				&dwDSLockedBufferSize,NULL,NULL,0L)))
// 			{
// 				e_log.WriteLog("RestoreBuffer(),Lock失败\n");
// 				return FALSE;
// 			}
// 
// 			//首先读出缓冲区长度的数据
// 			m_pWaveFile->Read((BYTE *)pDSLockedBuffer,dwDSLockedBufferSize,&dwWavDataRead);
// 			if( dwWavDataRead == 0 )
// 			{
// 				// Wav is blank, so just fill with silence
// 				FillMemory( (BYTE*) pDSLockedBuffer, 
// 					dwDSLockedBufferSize, 
// 					(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
// 			}		
// 			else if(dwWavDataRead < dwDSLockedBufferSize)//如果文件小于缓冲区,用静音来填充剩余的空间
// 			{
// 				FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, 
// 					dwDSLockedBufferSize - dwWavDataRead, 
// 					(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );			
// 			}
// 
// 			if(FAILED(hr = m_pDSBuffer->Unlock(pDSLockedBuffer,
// 				dwDSLockedBufferSize,NULL,NULL)))
// 			{
// 				e_log.WriteLog("RestoreBuffer(),Unlock失败\n");
// 				return FALSE;
// 			}
// 		}
// 		return TRUE;
// 	}
	//定义成友元线程函数，以便访问该类私有数据
	//friend DWORD WINAPI HandleNotificationThread(void* pParam);
private:
	
	static UINT __stdcall PlayThread(void *p)
	{
		CDSoundPlayer *pThis = (CDSoundPlayer *)p;
		DWORD dwOffset = 0;
		byte *pBuffer = NULL;
		DWORD nBufferLength = 0;
		DWORD dwResult = WAIT_OBJECT_0;
		while(pThis->m_bPlayThreadRun)
		{
			if (!pThis->m_dwNotifySize ||
				!pThis->m_nWaveBufferSize)
			{
				Sleep(25);
				continue;
			}
			
			if (pThis->GetWaveDataLength() < pThis->m_dwNotifySize)
			{
				Sleep(1000/pThis->m_nNotificationsNum);
				continue;
			}
			if((dwResult >=WAIT_OBJECT_0) && (dwResult <=WAIT_OBJECT_0 + pThis->m_nNotificationsNum))
			{
				if (!pThis->GetBuffer(dwOffset,(void **)&pBuffer,nBufferLength))
					continue;
				pThis->GetWaveData(pBuffer,nBufferLength);				

				dwOffset += pThis->m_dwNotifySize;
				dwOffset %= (pThis->m_dwNotifySize * pThis->m_nNotificationsNum);
				pThis->ReleaseBuffer(pBuffer,nBufferLength);
				//DsTrace("this is %d of buffer @%08X.\n",dwOffset,pBuffer);
			}
			//DsTrace("Wait for Notifications.\n");
			
			dwResult = WaitForMultipleObjects (pThis->m_nNotificationsNum, pThis->m_hEventArray, FALSE, INFINITE);
			DWORD dwSpan = GetTickCount() - pThis->m_dwLastTick;
			//DsTrace("%s TimeSpan = %d.\n",__FUNCTION__,dwSpan);
			pThis->m_dwLastTick = GetTickCount();
		}
		return 0;
	}
	DWORD	m_dwLastTick;
	DWORD	m_dwLastTick1;

	int		m_nAudioFrames;		// 投放音频帧的数量
	CRITICAL_SECTION m_csBuffer;// 缓冲区互斥量
	CRITICAL_SECTION m_csPlay;	// 用于控制播放的互斥量，防止同一对象被多个线程播放
	CRITICAL_SECTION m_csDsound;// 用于控制m_pDirectSound多象，防止同一个对象被多次创建
	bool	m_bPlayed;			// 是否已进入播放流程
	int		m_nBufferPlayLength;// 缓冲区的长度(单位为秒)
	int		m_nNotificationsNum;// 该缓冲区的通知个数
	HWND	m_hWnd;
	LPDIRECTSOUND m_pDirectSound;	
	LPDIRECTSOUNDBUFFER m_pDSBPrimary;	// 主缓冲区
	LPDIRECTSOUNDBUFFER m_pDSBSlavely;	// 副缓冲区
	DSBPOSITIONNOTIFY *m_pDSPosNotify;	// 播放通知

	DWORD m_dwDSBufferSize;
	DWORD m_dwNotifySize;
	HANDLE* m_hEventArray;
	//CWinThread *m_pPlayThread;
	long m_lVolume;
	BOOL m_bPause;
	BOOL m_bMute;	
	WAVEFORMATEX m_wfx;        // Pointer to WAVEFORMATEX structure
	shared_ptr<byte> m_pWaveBuffer;
	int m_nWaveBufferSize;		// 声音缓冲区的总长度
	int m_nWaveDataLength;		// 声音数据的长度
	CRITICAL_SECTION m_csWaveBuffer; //
	bool m_bPlayThreadRun;
	HANDLE m_hPlayThread;
};
