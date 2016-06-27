#pragma once
#include <dsound.h>
#include <memory>
#include <list>
#include "autolock.h"
using namespace std;
using namespace std::tr1;
//#include <boost/smart_ptr.hpp>
//using namespace boost;
// #include <mmstream.h>
// #include <MMReg.h>

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

#define Audio_Play_Segments		50

/// @brief DirectSoundBuffer对象封装类，用于创建和管理DirectSoundBuffer对象 
class CDSoundBuffer
{
public:
	/// 类内引用成员的初始化时,在必须以引用的方式导入类构造参数,否则类成员的引用会指向无法预测的地址
	explicit CDSoundBuffer(int &nNotifyCount, DWORD &dwNotifySize)
		:m_nNotifyCount(nNotifyCount)
		, m_dwNotifySize(dwNotifySize)
	{
		m_pDSBuffer = NULL;
		m_pDSPosNotify = NULL;
		m_hEventArray = NULL;
		m_lVolume = 0;
		m_bPause = FALSE;
		m_bMute = FALSE;
		InitializeCriticalSection(&m_csBuffer);
		m_bPlayed = false;
	}

	~CDSoundBuffer(void)
	{
		StopPlay();
		if (m_hEventArray)
		{
			for (int i = 0; i < m_nNotifyCount; i++)
				CloseHandle(m_hEventArray[i]);
			delete[]m_hEventArray;
		}

		//释放缓冲区
		SAFE_RELEASE(m_pDSBuffer);
		SAFE_DELETE_ARRAY(m_pDSPosNotify);
		DeleteCriticalSection(&m_csBuffer);
	}

	BOOL IsPlaying()
	{
		BOOL bIsPlaying = FALSE;
		if (m_pDSBuffer == NULL)
			return FALSE;
		DWORD dwStatus = 0;
		HRESULT hr = m_pDSBuffer->GetStatus(&dwStatus);
		bIsPlaying |= ((dwStatus & DSBSTATUS_PLAYING) != 0);
		return bIsPlaying;
	}


	//////////////////////////////////////////////////////////////////////////
	// 初始化，主要是生成播放缓冲区和设置通知事件
	//bGlobalFocus如果为TRUE，在失去焦点时仍然播放
	//////////////////////////////////////////////////////////////////////////	
	bool Create(LPDIRECTSOUNDBUFFER pDSBPrimary)
	{
		if (!pDSBPrimary)
			return false;
		//创建副冲区
		if (FAILED(pDSBPrimary->QueryInterface(IID_IDirectSoundBuffer, (LPVOID*)&m_pDSBuffer)))
		{
			DsTrace("%s Create Slave Sound buffer Failed.\n", __FUNCTION__);
			return false;
		}

		IDirectSoundNotify *pDSNotify = NULL;
		if (FAILED(m_pDSBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&pDSNotify)))
		{
			DsTrace("%s m_pDSBSlavely->QueryInterface Failed.\n", __FUNCTION__);
			SAFE_RELEASE(m_pDSBuffer);
			return FALSE;
		}

		m_pDSPosNotify = new DSBPOSITIONNOTIFY[m_nNotifyCount];
		m_hEventArray = new HANDLE[m_nNotifyCount];
		for (int i = 0; i < m_nNotifyCount; i++)
		{
			m_pDSPosNotify[i].dwOffset = i*m_dwNotifySize;
			m_hEventArray[i] = ::CreateEvent(NULL, false, false, NULL);
			m_pDSPosNotify[i].hEventNotify = m_hEventArray[i];
		}
		pDSNotify->SetNotificationPositions(m_nNotifyCount, m_pDSPosNotify);
		pDSNotify->Release();
		pDSNotify = NULL;
		return TRUE;
	}


#define DSBPLAY_ONCE 0x00000000
	// 尝试进入播放流程，可以通用GetBuffer()取得缓冲区，往缓冲区中添加音频数据
	// 返回0时，进入播放流程失败，可能其它线程已经进入播放流程
	// 否则返回单次可填充音频缓冲区的长度
	// 线程或进程退时，必须调用StopPlay以退出播放流程，否则可能造成死锁
	bool StartPlay(bool bLoopPlay = true, bool bContinue = false)
	{
		if (m_pDSBuffer)
		{
			if (!RestoreBuffer())
				return false;
			if (bContinue)
				m_pDSBuffer->SetCurrentPosition(m_nPlayOffset);
			else
				m_pDSBuffer->SetCurrentPosition(0);

			if (bLoopPlay)
				m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
			else
				m_pDSBuffer->Play(0, 0, DSBPLAY_ONCE);
			m_bPlayed = true;
		}
		return true;
	}
	// 退出播放流程
	BOOL StopPlay()
	{
		if (!m_bPlayed)
			return FALSE;

		m_bPlayed = false;

		if (m_pDSBuffer == NULL)
			return FALSE;
		m_bPause = FALSE;
		HRESULT hr = m_pDSBuffer->SetCurrentPosition(0L);

		if (FAILED(hr))
		{
			DsTrace("%s m_pDSBSlavely->SetCurrentPosition() Failed,hr = 0x%08X.\n", __FUNCTION__, hr);
			return FALSE;
		}

		hr = m_pDSBuffer->Stop();
		if (FAILED(hr))
		{
			DsTrace("%s m_pDSBSlavely->Stop() Failed,hr = 0x%08X.\n", __FUNCTION__, hr);
			return FALSE;
		}
		return TRUE;
	}

	void Pause()
	{
		if (m_pDSBuffer == NULL)
		{
			return;
		}

		m_pDSBuffer->Stop();
		m_bPause = TRUE;
	}

	void SetMute(BOOL bMute)
	{
		if (m_pDSBuffer == NULL)
		{
			return;
		}

		m_bMute = bMute;
		if (bMute)
		{
			m_pDSBuffer->SetVolume(-10000);
		}
		else
		{
			m_pDSBuffer->SetVolume(m_lVolume);
		}
	}

	// 音量范围-10000,0
	// -10000	静音,0	最大音量
	void SetVolume(long lVolume)
	{
		if (m_pDSBuffer == NULL)
		{
			return;
		}

		m_lVolume = lVolume;
// 		if (m_bMute == FALSE)
// 		{
// 			m_pDSBuffer->SetVolume(m_lVolume);
// 		}

		double decibels;
		DWORD dsVol;

		if (lVolume == 0)
			dsVol = DSBVOLUME_MIN;
		else if (lVolume > 10000)
			dsVol = DSBVOLUME_MAX;
		else
		{
			decibels = 20.0 * log10((double)lVolume / 100.0);
			dsVol = (DWORD)(decibels * 100.0);
		}
		m_pDSBuffer->SetVolume(dsVol);
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

	bool RestoreBuffer()
	{
		DWORD dwStatus;
		if (FAILED(m_pDSBuffer->GetStatus(&dwStatus)))
		{
			DsTrace("%s m_pDSBuffer->GetStatus() Failed.\n", __FUNCTION__);
			return false;
		}
		if (dwStatus & DSBSTATUS_BUFFERLOST)
		{
			DsTrace("%s DsBuffer lost ,Now try to restore.\n", __FUNCTION__);
			int nRestoreCount = 0;
			bool bRestored = false;
			while (nRestoreCount < 5)
			{
				if (SUCCEEDED(m_pDSBuffer->Restore()))
				{
					bRestored = true;
					break;
				}
				else
				{
					nRestoreCount++;
					Sleep(10);
				}
			}
			if (!bRestored)
			{
				return false;
			}
		}
		return true;
	}

	inline bool WaitForPosNotify()
	{
		return (WaitForMultipleObjects(m_nNotifyCount, m_hEventArray, FALSE, 1000 / m_nNotifyCount) == WAIT_TIMEOUT);
	}


	bool WritePCM(IN byte *pPCM, IN int nPCMLength,bool bWait = true)
	{
		LPVOID  pBuffer1;
		DWORD	nBuffer1Length;
		LPVOID  pBuffer2;
		DWORD	nBuffer2Length;

		if (bWait)
		{
			DWORD nResult = WaitForMultipleObjects(m_nNotifyCount, m_hEventArray, FALSE, 200);
			if (nResult == WAIT_TIMEOUT)
			{
				DsTrace("%s Wait for Dsound Play evnet timeout.\n", __FUNCTION__);
				return false;
			}
		}


		if (!TryEnterCriticalSection(&m_csBuffer))
		{
			DsTrace("%s Failed to lock dsound buffer.\n", __FUNCTION__);
			return false;
		}
		if (!m_pDSBuffer)
		{
			DsTrace("%s Dsound Buffer is invalid.\n", __FUNCTION__);
			return false;
		}
		shared_ptr<CRITICAL_SECTION> autoLeaveSection(&m_csBuffer, ::LeaveCriticalSection);

		HRESULT hr = S_OK;

		if (!RestoreBuffer())
		{
			DsTrace("%s Restore dsound Buffer is invalid.\n", __FUNCTION__);
			return false;
		}
		hr = m_pDSBuffer->Lock(m_nPlayOffset, nPCMLength, &pBuffer1, &nBuffer1Length, &pBuffer2, &nBuffer2Length, 0);

		if (FAILED(hr))
		{
			DsTrace("%s m_pDSBSlavely->Lock() Failed.\n", __FUNCTION__);
			return false;
		}
		// 写入声音数据
		CopyMemory(pBuffer1, pPCM, nBuffer1Length);
		if (nullptr != pBuffer2)
		{
			CopyMemory(pBuffer2, pPCM + nBuffer1Length, nBuffer2Length);
		}
		m_nPlayOffset += nPCMLength;
		m_nPlayOffset %= (m_dwNotifySize * m_nNotifyCount);
		hr = m_pDSBuffer->Unlock(pBuffer1, nBuffer1Length, pBuffer2, nBuffer2Length);
		if (FAILED(hr))
		{
			DsTrace("%s m_pDSBSlavely->Unlock() Failed.\n", __FUNCTION__);
			return false;
		}
		return true;
	}
private:
	DWORD m_nPlayOffset = 0;
	DWORD m_nPositoin = 0;
	CRITICAL_SECTION m_csBuffer;// 缓冲区互斥量
	bool	m_bPlayed = false;	// 是否已进入播放流程
	LPDIRECTSOUNDBUFFER m_pDSBuffer;	// 副缓冲区
	DSBPOSITIONNOTIFY *m_pDSPosNotify;	// 播放通知
	HANDLE* m_hEventArray = nullptr;
	long m_lVolume = 0;
	BOOL m_bPause = false;
	BOOL m_bMute;
	const int &m_nNotifyCount;
	const DWORD &m_dwNotifySize;
};
typedef shared_ptr<CDSoundBuffer> CDSoundBufferPtr;
/// @brief DirectSound对象封装类，用于创建和管理DirectSound对象 
class CDSound
{
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
	// 创建一个简单的不可见的窗口
	// 用于在用户未提供窗口句柄时，为DsSound提供窗口
	HWND CreateSimpleWindow(IN HINSTANCE hInstance = NULL)
	{
		WNDCLASSEX wcex;
		TCHAR *szWindowClass = _T("DirectX Sound Window");
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DefWindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, IDC_ICON);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = LoadIcon(hInstance, IDC_ICON);

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
public:
	CDSound()
	{
		m_nBufferPlayLength = 1;	// 默认1秒
		m_nNotifyCount = 25;	// 把1秒分成25段
		m_pDirectSound = NULL;
		m_pDSBPrimary = NULL;
		m_dwDSBufferSize = 0;
		m_dwNotifySize = 0;
		ZeroMemory(&m_wfx, sizeof(WAVEFORMATEX));
		m_wfx.cbSize = sizeof(WAVEFORMATEX);
		InitializeCriticalSection(&m_csDsound);
		InitializeCriticalSection(&m_csListBuffer);
	}
	CDSound(HWND hWnd)
	{
		m_nBufferPlayLength = 1;	// 默认1秒
		m_nNotifyCount = 25;	// 把1秒分成25段
		m_pDirectSound = NULL;
		m_pDSBPrimary = NULL;
		m_dwDSBufferSize = 0;
		m_dwNotifySize = 0;
		ZeroMemory(&m_wfx, sizeof(WAVEFORMATEX));
		m_wfx.cbSize = sizeof(WAVEFORMATEX);
		InitializeCriticalSection(&m_csDsound);
		InitializeCriticalSection(&m_csListBuffer);
		m_hWnd = hWnd;
	}
	~CDSound()
	{
		UnInitialize();
		// 		if (m_hWnd)
		// 			::PostMessage(m_hWnd, WM_DESTROY, 0, 0);
		DeleteCriticalSection(&m_csDsound);
		DeleteCriticalSection(&m_csListBuffer);
	}

	bool IsInitialized()
	{
		CAutoLock lock(&m_csDsound);
		return (m_pDirectSound != nullptr);
	}

	//////////////////////////////////////////////////////////////////////////
	// 初始化，主要是生成播放缓冲区和设置通知事件
	//bGlobalFocus如果为TRUE，在失去焦点时仍然播放
	//////////////////////////////////////////////////////////////////////////	
	BOOL Initialize(HWND hWnd, int nNotifyCount = 50, int nPlayTime = 1/*Second*/, DWORD nSampleFreq = 8000, WORD nBitsPerSample = 16)
	{
		if (!TryEnterCriticalSection(&m_csDsound))
			return FALSE;
		shared_ptr<CRITICAL_SECTION> autoLeaveSection(&m_csDsound, ::LeaveCriticalSection);
		if (m_pDirectSound)
			return TRUE;

		HRESULT hr = S_OK;
		//创建directsound
		if (FAILED(hr = DirectSoundCreate(NULL, &m_pDirectSound, NULL)))
		{
			DsTrace("%s DirectSoundCreate Failed.\n", __FUNCTION__);
			return FALSE;
		}

		if (!hWnd || !IsWindow(hWnd))
		{// 若无窗口，则创建内部小窗口
			m_hWnd = CreateSimpleWindow(NULL);
			if (m_hWnd)
			{
				if (FAILED(m_pDirectSound->SetCooperativeLevel(m_hWnd, DSSCL_PRIORITY)))
				{
					DsTrace("%s SetCooperativeLevel Failed.\n", __FUNCTION__);
					return FALSE;
				}
			}
			else
			{
				DsTrace("%s CreateSimpleWindow Failed.\n", __FUNCTION__);
				return FALSE;
			}
		}
		else if (FAILED(m_pDirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
		{
			DsTrace("%s SetCooperativeLevel Failed.\n", __FUNCTION__);
			return FALSE;
		}

		//创建主缓冲区			
		// Get the primary buffer 				
		ZeroMemory(&m_wfx, sizeof(WAVEFORMATEX));
		m_wfx.cbSize		 = 0;
		m_wfx.wFormatTag	 = (WORD)WAVE_FORMAT_PCM;		// 音频格式
		m_wfx.nChannels		 = 1;							// 音频通道数量
		m_wfx.nSamplesPerSec = nSampleFreq;				// 采样率
		m_wfx.wBitsPerSample = nBitsPerSample;			// 采样位宽
		m_wfx.nBlockAlign	 = (WORD)((m_wfx.wBitsPerSample / 8) * m_wfx.nChannels);
		m_wfx.nAvgBytesPerSec= (DWORD)(m_wfx.nSamplesPerSec * m_wfx.nBlockAlign);
		m_nNotifyCount		 = nNotifyCount;
		m_nBufferPlayLength	 = nPlayTime;
		if (!m_nNotifyCount || !m_wfx.nBlockAlign)
			return FALSE;
		//定义一个m_nBufferPlayLength秒的缓冲区,并将这个缓冲区分成m_nNotificationsNum个通知块;
		m_dwNotifySize = m_wfx.nSamplesPerSec*m_nBufferPlayLength*m_wfx.nBlockAlign / m_nNotifyCount;
		//确保m_dwNotifySize是nBlockAlign的倍数
		m_dwNotifySize = m_dwNotifySize - m_dwNotifySize%m_wfx.nBlockAlign;
		m_dwDSBufferSize = m_dwNotifySize*m_nNotifyCount;

		DSBUFFERDESC dsbd;
		ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
		dsbd.dwSize = sizeof(DSBUFFERDESC);
		dsbd.dwFlags = DSBCAPS_GLOBALFOCUS |
			DSBCAPS_CTRLVOLUME |
			DSBCAPS_CTRLPOSITIONNOTIFY |
			DSBCAPS_GETCURRENTPOSITION2;
		//DSBCAPS_PRIMARYBUFFER;
		dsbd.dwBufferBytes = m_dwDSBufferSize;
		dsbd.lpwfxFormat = &m_wfx;

		if (FAILED(hr = m_pDirectSound->CreateSoundBuffer(&dsbd, &m_pDSBPrimary, NULL)))
		{
			DsTrace("%s CreateSoundBuffer Failed.\n", __FUNCTION__);
			SAFE_RELEASE(m_pDirectSound);
			return false;
		}
		return true;
	}
	void UnInitialize()
	{
		shared_ptr<CRITICAL_SECTION> autoLeaveSection(&m_csDsound, ::LeaveCriticalSection);
		EnterCriticalSection(&m_csListBuffer);
		m_listDsBuffer.clear();
		LeaveCriticalSection(&m_csListBuffer);
		//释放缓冲区
		SAFE_RELEASE(m_pDSBPrimary);
		//释放directsound		
		SAFE_RELEASE(m_pDirectSound);
	}
	CDSoundBuffer *CreateDsoundBuffer()
	{
		CDSoundBufferPtr pDsBuffer = make_shared<CDSoundBuffer>(m_nNotifyCount, m_dwNotifySize);
		if (pDsBuffer->Create(m_pDSBPrimary))
		{
			::EnterCriticalSection(&m_csListBuffer);
			m_listDsBuffer.push_back(pDsBuffer);
			::LeaveCriticalSection(&m_csListBuffer);
			return pDsBuffer.get();
		}
		else
		{
			return nullptr;
		}
	}

	void DestroyDsoundBuffer(CDSoundBuffer *pDsBuffer)
	{
		if (!pDsBuffer)
			return;
		::EnterCriticalSection(&m_csListBuffer);
		for (list<CDSoundBufferPtr>::iterator it = m_listDsBuffer.begin(); it != m_listDsBuffer.end();)
			if ((*it).get() == pDsBuffer)
			{
				m_listDsBuffer.erase(it);
				break;
			}
			else
				it++;
		::LeaveCriticalSection(&m_csListBuffer);
	}

public:
	list<CDSoundBufferPtr>m_listDsBuffer;
	LPDIRECTSOUND m_pDirectSound = nullptr;
	LPDIRECTSOUNDBUFFER m_pDSBPrimary = nullptr;
	CRITICAL_SECTION m_csListBuffer;
	CRITICAL_SECTION m_csDsound;
	int m_nNotifyCount = 0;
	int m_nBufferPlayLength = 0;
	DWORD m_dwDSBufferSize = 0;
	DWORD m_dwNotifySize = 0;
	HWND	m_hWnd = nullptr;
	WAVEFORMATEX m_wfx;
};


