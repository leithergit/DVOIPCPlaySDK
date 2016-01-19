#pragma once
#include <dsound.h>
#include <memory>
#include <list>
using namespace std;
// #include <mmstream.h>
// #include <MMReg.h>

//��������CWinThread����߳�
//pThreadClass	CWinThread��ָ�룬����ָ�򴴽�����̶߳���
//ThreadFunc	�̵߳�ִ�к���
//Param			�̵߳���ڲ���
//ThreadPririty	�̵߳����ȼ�
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

#define Audio_Play_Segments		25

class CDSoundBuffer;
class CDsound
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
	CDsound()
	{
		m_nBufferPlayLength = 1;	// Ĭ��1��
		m_nNotificationsNum = 25;	// ��1��ֳ�25��
		m_pDirectSound = NULL;
		m_pDSBPrimary = NULL;
		m_dwDSBufferSize = 0;
		m_dwNotifySize = 0;
		ZeroMemory(&m_wfx, sizeof(WAVEFORMATEX));
		m_wfx.cbSize = sizeof(WAVEFORMATEX);
		InitializeCriticalSection(&m_csDsound);
		
	}
	CDsound(HWND hWnd)
	{
		m_nBufferPlayLength = 1;	// Ĭ��1��
		m_nNotificationsNum = 25;	// ��1��ֳ�25��
		m_pDirectSound = NULL;
		m_pDSBPrimary = NULL;
		m_dwDSBufferSize = 0;
		m_dwNotifySize = 0;
		ZeroMemory(&m_wfx, sizeof(WAVEFORMATEX));
		m_wfx.cbSize = sizeof(WAVEFORMATEX);
		InitializeCriticalSection(&m_csDsound);
		m_hWnd = hWnd;
	}
	~CDsound()
	{
		//�ͷŻ�����
		SAFE_RELEASE(m_pDSBPrimary);
		//�ͷ�directsound
		// Ϊ�λ����ͷ�m_pDirectSound����ʱ��ס,���Ҳ��ͷ�,�պ���ʱ���ٲ��������
		// xioggao.lee @2015.10.28
		SAFE_RELEASE(m_pDirectSound);

		if (m_hWnd)
			::PostMessage(m_hWnd, WM_DESTROY, 0, 0);
		
		DeleteCriticalSection(&m_csDsound);
	}
	//////////////////////////////////////////////////////////////////////////
	// ��ʼ������Ҫ�����ɲ��Ż�����������֪ͨ�¼�
	//bGlobalFocus���ΪTRUE����ʧȥ����ʱ��Ȼ����
	//////////////////////////////////////////////////////////////////////////	
	BOOL Initialize(HWND hWnd, int nNotifyCount = 50, int nPlayTime = 1/*Second*/)
	{
		if (!TryEnterCriticalSection(&m_csDsound))
			return FALSE;
		shared_ptr<CRITICAL_SECTION> autoLeaveSection(&m_csDsound, ::LeaveCriticalSection);
		if (m_pDirectSound)
			return TRUE;
	

		//����directsound
		if (FAILED(hr = DirectSoundCreate(NULL, &m_pDirectSound, NULL)))
		{
			DsTrace("CDSoundPlayer::Initialize() DirectSoundCreate Failed.\n");
			return FALSE;
		}

		if (!hWnd || !IsWindow(hWnd))
		{// ���޴��ڣ��򴴽��ڲ�С����
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

		//������������			
		// Get the primary buffer 				
		ZeroMemory(&m_wfx, sizeof(WAVEFORMATEX));
		m_wfx.cbSize = 0;
		m_wfx.wFormatTag = (WORD)WAVE_FORMAT_PCM;		// ��Ƶ��ʽ
		m_wfx.nChannels = 1;							// ��Ƶͨ������
		m_wfx.nSamplesPerSec = 8000;						// ������
		m_wfx.wBitsPerSample = 16;
		m_wfx.nBlockAlign = (WORD)((m_wfx.wBitsPerSample / 8) * m_wfx.nChannels);
		m_wfx.nAvgBytesPerSec = (DWORD)(m_wfx.nSamplesPerSec * m_wfx.nBlockAlign);

		m_nNotificationsNum = nNotifyCount;
		m_nBufferPlayLength = nPlayTime;

		//����һ��m_nBufferPlayLength��Ļ�����,��������������ֳ�m_nNotificationsNum��֪ͨ��;
		m_dwNotifySize = m_wfx.nSamplesPerSec*m_nBufferPlayLength*m_wfx.nBlockAlign / m_nNotificationsNum;
		//ȷ��m_dwNotifySize��nBlockAlign�ı���
		m_dwNotifySize = m_dwNotifySize - m_dwNotifySize%m_wfx.nBlockAlign;		
		m_dwDSBufferSize = m_dwNotifySize*m_nNotificationsNum;		

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
			DsTrace("CDSoundPlayer::Initialize() CreateSoundBuffer Failed.\n");
			SAFE_DELETE(m_pDirectSound);
			return FALSE;
		}
	}
	CDSoundBuffer *CreateDsoundBuffer()
	{
		CDSoundBuffer *pDsBuffer = new CDSoundBuffer();
		if (!TryEnterCriticalSection(&m_csDsound))
			return FALSE;
		shared_ptr<CRITICAL_SECTION> autoLeaveSection(&m_csDsound, ::LeaveCriticalSection);
		
		
		return TRUE;
	}

public:
	list<CDSoundBuffer>	m_listDsBuffer;
	CRITICAL_SECTION	m_csDsoundBuffer;	
	LPDIRECTSOUND m_pDirectSound	 = nullptr;
	CRITICAL_SECTION m_csDsound;		// ���ڿ���m_pDirectSound���󣬷�ֹͬ���̴������DIRECTSOUND����
	LPDIRECTSOUNDBUFFER m_pDSBPrimary = nullptr;
	CRITICAL_SECTION m_csDsound;
	int m_nNotificationsNum			 = 0;
	int m_nBufferPlayLength			 = 0;
	DWORD m_dwDSBufferSize			 = 0;
	DWORD m_dwNotifySize			 = 0;
	HWND	m_hWnd					 = nullptr;
};
class CDSoundBuffer
{
private:
	explicit CDSoundBuffer()
	{
	}
public:

	CDSoundBuffer(CDsound *pDsound)
	{
		m_pDsound = pDsound;
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
		m_bPlayThreadRun = false;
		if (m_hEventArray)
		{
			for (int i = 0; i < m_pDsound->m_nNotificationsNum; i++)
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
				
		//�ͷŻ�����
		SAFE_RELEASE(m_pDSBuffer);
		SAFE_DELETE_ARRAY(m_pDSPosNotify);				
		DeleteCriticalSection(&m_csBuffer);
	}

	BOOL IsPlaying()
	{
		BOOL bIsPlaying = FALSE;	
		if(m_pDSBuffer == NULL )
			return FALSE; 
		DWORD dwStatus = 0;
		m_pDSBuffer->GetStatus( &dwStatus );
		bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
		return bIsPlaying;
	}
	
		
	//////////////////////////////////////////////////////////////////////////
	// ��ʼ������Ҫ�����ɲ��Ż�����������֪ͨ�¼�
	//bGlobalFocus���ΪTRUE����ʧȥ����ʱ��Ȼ����
	//////////////////////////////////////////////////////////////////////////	
	bool Create(int nPlayTime = 1/*Second*/)
	{
		if (!m_pDsound)
			return false;
		//����������
		if (FAILED(m_pDsound->m_pDSBPrimary->QueryInterface(IID_IDirectSoundBuffer,(LPVOID*)&m_pDSBuffer)))
		{
			DsTrace("%s Create Slave Sound buffer Failed.\n",__FUNCTION__);		
			return false;
		}

		IDirectSoundNotify *pDSNotify = NULL;
		if(FAILED(m_pDSBuffer->QueryInterface(IID_IDirectSoundNotify,(LPVOID*)&m_pDSNotify)))
		{
			DsTrace("%s m_pDSBSlavely->QueryInterface Failed.\n",__FUNCTION__);
			SAFE_DELETE(m_pDSBuffer);
			return FALSE ;
		}

		m_pDSPosNotify = new DSBPOSITIONNOTIFY[m_pDsound->m_nNotificationsNum];
		m_hEventArray = new HANDLE[m_pDsound->m_nNotificationsNum];
		for (int i = 0; i < m_pDsound->m_nNotificationsNum; i++)
		{
			m_pDSPosNotify[i].dwOffset = i*m_pDsound->m_dwNotifySize;
			m_hEventArray[i] = ::CreateEvent(NULL,false,false,NULL); 
			m_pDSPosNotify[i].hEventNotify = m_hEventArray[i];
		}
		pDSNotify->SetNotificationPositions(m_pDsound->m_nNotificationsNum, m_pDSPosNotify);
		pDSNotify->Release();	
		pDSNotify = NULL;
		return TRUE;
	}

	inline DWORD WaitForPosNotify()
	{
		return (WaitForMultipleObjects(m_pDsound->m_nNotificationsNum, m_hEventArray, FALSE, 1000 / m_pDsound->m_nNotificationsNum) - WAIT_OBJECT_0);
	}

#define DSBPLAY_ONCE 0x00000000
	// ���Խ��벥�����̣�����ͨ��GetBuffer()ȡ�û����������������������Ƶ����
	// ����0ʱ�����벥������ʧ�ܣ����������߳��Ѿ����벥������
	// ���򷵻ص��ο������Ƶ�������ĳ���
	// �̻߳������ʱ���������StopPlay���˳��������̣���������������
	bool StartPlay(bool bLoopPlay = true)
	{		
		if (m_pDSBuffer)
		{
			if (!RestoreBuffer())
				return false;
			m_pDSBuffer->SetCurrentPosition(0);
			if (bLoopPlay)
				m_pDSBuffer->Play(0,0,DSBPLAY_LOOPING);
			else
				m_pDSBuffer->Play(0, 0, DSBPLAY_ONCE);
			m_bPlayed = true;
		}
		return 
	}
	// �˳���������
	BOOL StopPlay()
	{
		if (!m_bPlayed)
			return FALSE;
		
		m_bPlayed = false;
			
		if(m_pDSBuffer == NULL)
			return FALSE;
		m_bPause = FALSE;	
		HRESULT hr = m_pDSBuffer->Stop();
		if(FAILED(hr))
		{
			DsTrace("%s m_pDSBSlavely->Stop() Failed,hr = 0x%08X.\n",__FUNCTION__,hr);
			return FALSE;
		}
		hr = m_pDSBuffer->SetCurrentPosition(0L);
		if(FAILED(hr))
		{
			DsTrace("%s m_pDSBSlavely->SetCurrentPosition() Failed,hr = 0x%08X.\n",__FUNCTION__,hr);
			return FALSE;
		}
		return TRUE;
	}
		
	void Pause()
	{
		if(m_pDSBuffer == NULL)
		{
			return;
		}

		m_pDSBuffer->Stop();
		m_bPause = TRUE;
	}
	
	void SetMute(BOOL bMute)
	{
		if(m_pDSBuffer == NULL)
		{
			return;
		}

		m_bMute = bMute;
		if(bMute)
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
		if(m_pDSBuffer == NULL)
		{
			return;
		}

		m_lVolume = lVolume;
		if(m_bMute == FALSE)
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
	bool WritePCM(IN byte *pPCM,IN int nPCMLength)
	{
		LPVOID  pBuffer1;
		DWORD	nBuffer1Length;
		LPVOID  pBuffer2;
		DWORD	nBuffer2Length;

		DWORD nResult = WaitForMultipleObjects(m_pDsound->m_nNotificationsNum, m_hEventArray, FALSE, 1000 / m_pDsound->m_nNotificationsNum);
		if (nResult == WAIT_TIMEOUT)
			return false;
		
		if (!TryEnterCriticalSection(&m_csBuffer))
			return false;
			
		shared_ptr<CRITICAL_SECTION> autoLeaveSection(&m_csBuffer, ::LeaveCriticalSection);
		if (!m_pDSBuffer)
			return false;
		HRESULT hr = S_OK;
		
		if (!RestoreBuffer())
			return false;
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
	CRITICAL_SECTION m_csBuffer;// ������������
	bool	m_bPlayed;			// �Ƿ��ѽ��벥������
	LPDIRECTSOUNDBUFFER m_pDSBuffer;	// ��������
	DSBPOSITIONNOTIFY *m_pDSPosNotify;	// ����֪ͨ
	
	HANDLE* m_hEventArray;
	long m_lVolume;
	BOOL m_bPause;
	BOOL m_bMute;		
	int m_nWaveBufferSize;		// �������������ܳ���
	int m_nWaveDataLength;		// �������ݵĳ���
	bool m_bPlayThreadRun;
	HANDLE m_hPlayThread;
	CDsound*	m_pDsound;
};
