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

/// @brief DirectSoundBuffer�����װ�࣬���ڴ����͹���DirectSoundBuffer���� 
class CDSoundBuffer
{
public:
	/// �������ó�Ա�ĳ�ʼ��ʱ,�ڱ��������õķ�ʽ�����๹�����,�������Ա�����û�ָ���޷�Ԥ��ĵ�ַ
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

		//�ͷŻ�����
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
	// ��ʼ������Ҫ�����ɲ��Ż�����������֪ͨ�¼�
	//bGlobalFocus���ΪTRUE����ʧȥ����ʱ��Ȼ����
	//////////////////////////////////////////////////////////////////////////	
	bool Create(LPDIRECTSOUNDBUFFER pDSBPrimary)
	{
		if (!pDSBPrimary)
			return false;
		//����������
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
	// ���Խ��벥�����̣�����ͨ��GetBuffer()ȡ�û����������������������Ƶ����
	// ����0ʱ�����벥������ʧ�ܣ����������߳��Ѿ����벥������
	// ���򷵻ص��ο������Ƶ�������ĳ���
	// �̻߳������ʱ���������StopPlay���˳��������̣���������������
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
	// �˳���������
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

	// ������Χ-10000,0
	// -10000	����,0	�������
	void SetVolume(long lVolume)
	{
		if (m_pDSBuffer == NULL)
		{
			return;
		}

		m_lVolume = lVolume;
		if (m_bMute == FALSE)
		{
			m_pDSBuffer->SetVolume(m_lVolume);
		}
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


	bool WritePCM(IN byte *pPCM, IN int nPCMLength)
	{
		LPVOID  pBuffer1;
		DWORD	nBuffer1Length;
		LPVOID  pBuffer2;
		DWORD	nBuffer2Length;

// 		DWORD nResult = WaitForMultipleObjects(m_nNotifyCount, m_hEventArray, FALSE, 200);
// 		if (nResult == WAIT_TIMEOUT)
// 		{
// 			DsTrace("%s Wait for Dsound Play evnet timeout.\n", __FUNCTION__);
// 			return false;
// 		}

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
		// д����������
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
	CRITICAL_SECTION m_csBuffer;// ������������
	bool	m_bPlayed = false;	// �Ƿ��ѽ��벥������
	LPDIRECTSOUNDBUFFER m_pDSBuffer;	// ��������
	DSBPOSITIONNOTIFY *m_pDSPosNotify;	// ����֪ͨ
	HANDLE* m_hEventArray = nullptr;
	long m_lVolume = 0;
	BOOL m_bPause = false;
	BOOL m_bMute;
	const int &m_nNotifyCount;
	const DWORD &m_dwNotifySize;
};
typedef shared_ptr<CDSoundBuffer> CDSoundBufferPtr;
/// @brief DirectSound�����װ�࣬���ڴ����͹���DirectSound���� 
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
	// ����һ���򵥵Ĳ��ɼ��Ĵ���
	// �������û�δ�ṩ���ھ��ʱ��ΪDsSound�ṩ����
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

		return ::CreateWindow(szWindowClass, 	// ������
			szWindowClass,		// ���ڱ��� 
			WS_OVERLAPPEDWINDOW,// ���ڷ��
			CW_USEDEFAULT, 		// �������Ͻ�X����
			0, 					// �������Ͻ�Y����
			32, 				// ���ڿ��
			32, 					// ���ڸ߶�
			NULL, 				// �����ھ��
			NULL,				// �˵����
			hInstance,
			NULL);
	}
public:
	CDSound()
	{
		m_nBufferPlayLength = 1;	// Ĭ��1��
		m_nNotifyCount = 25;	// ��1��ֳ�25��
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
		m_nBufferPlayLength = 1;	// Ĭ��1��
		m_nNotifyCount = 25;	// ��1��ֳ�25��
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
	// ��ʼ������Ҫ�����ɲ��Ż�����������֪ͨ�¼�
	//bGlobalFocus���ΪTRUE����ʧȥ����ʱ��Ȼ����
	//////////////////////////////////////////////////////////////////////////	
	BOOL Initialize(HWND hWnd, int nNotifyCount = 50, int nPlayTime = 1/*Second*/, DWORD nSampleFreq = 8000, WORD nBitsPerSample = 16)
	{
		if (!TryEnterCriticalSection(&m_csDsound))
			return FALSE;
		shared_ptr<CRITICAL_SECTION> autoLeaveSection(&m_csDsound, ::LeaveCriticalSection);
		if (m_pDirectSound)
			return TRUE;

		HRESULT hr = S_OK;
		//����directsound
		if (FAILED(hr = DirectSoundCreate(NULL, &m_pDirectSound, NULL)))
		{
			DsTrace("%s DirectSoundCreate Failed.\n", __FUNCTION__);
			return FALSE;
		}

		if (!hWnd || !IsWindow(hWnd))
		{// ���޴��ڣ��򴴽��ڲ�С����
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

		//������������			
		// Get the primary buffer 				
		ZeroMemory(&m_wfx, sizeof(WAVEFORMATEX));
		m_wfx.cbSize = 0;
		m_wfx.wFormatTag = (WORD)WAVE_FORMAT_PCM;		// ��Ƶ��ʽ
		m_wfx.nChannels = 1;							// ��Ƶͨ������
		m_wfx.nSamplesPerSec = nSampleFreq;				// ������
		m_wfx.wBitsPerSample = nBitsPerSample;			// ����λ��
		m_wfx.nBlockAlign = (WORD)((m_wfx.wBitsPerSample / 8) * m_wfx.nChannels);
		m_wfx.nAvgBytesPerSec = (DWORD)(m_wfx.nSamplesPerSec * m_wfx.nBlockAlign);
		m_nNotifyCount = nNotifyCount;
		m_nBufferPlayLength = nPlayTime;
		if (!m_nNotifyCount || !m_wfx.nBlockAlign)
			return FALSE;
		//����һ��m_nBufferPlayLength��Ļ�����,��������������ֳ�m_nNotificationsNum��֪ͨ��;
		m_dwNotifySize = m_wfx.nSamplesPerSec*m_nBufferPlayLength*m_wfx.nBlockAlign / m_nNotifyCount;
		//ȷ��m_dwNotifySize��nBlockAlign�ı���
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
		//�ͷŻ�����
		SAFE_RELEASE(m_pDSBPrimary);
		//�ͷ�directsound		
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


