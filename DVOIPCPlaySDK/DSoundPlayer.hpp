#pragma once
#include <dsound.h>
#include <memory>
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
		m_nBufferPlayLength = 1;	// Ĭ��1��
		m_nNotificationsNum = 20;	// ��1��ֳ�25��
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
		m_nBufferPlayLength = 1;	// Ĭ��1��
		m_nNotificationsNum = 20;	// ��1��ֳ�25��
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
				
		
		//�ͷŻ�����
		SAFE_RELEASE(m_pDSBPrimary);
		SAFE_RELEASE(m_pDSBSlavely);
		//�ͷ�directsound
		// Ϊ�λ����ͷ�m_pDirectSound����ʱ��ס,���Ҳ��ͷ�,�պ���ʱ���ٲ��������
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
	
	//���ò��Ż������ĳ��ȣ�����Ϊ��λ,��������ã�Ĭ�ϵ���3��
	inline void SetBufferLength(int nSecond = 1)
	{
		m_nBufferPlayLength = nSecond;
	}
	inline DWORD GetBufferLength()
	{
		return m_nBufferPlayLength;
	}
	//���øó��ȵĲ��Ż�������֪ͨ�¼�����,��������ã�Ĭ�ϵ���25��
	inline void SetNotificationsNum(int nCount = 25)
	{
		m_nNotificationsNum = nCount;
	}

	inline DWORD GetNotificationsNum()
	{
		return m_nNotificationsNum;
	}

	// ����һ���򵥵Ĳ��ɼ��Ĵ���
	// �������û�δ�ṩ���ھ��ʱ��ΪDsSound�ṩ����

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
	
	//////////////////////////////////////////////////////////////////////////
	// ��ʼ������Ҫ�����ɲ��Ż�����������֪ͨ�¼�
	//bGlobalFocus���ΪTRUE����ʧȥ����ʱ��Ȼ����
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
		
		//����directsound
		if(FAILED(hr = DirectSoundCreate(NULL, &m_pDirectSound, NULL)))
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
		ZeroMemory( &m_wfx, sizeof(WAVEFORMATEX) ); 
		m_wfx.cbSize			= 0;
		m_wfx.wFormatTag		= (WORD) WAVE_FORMAT_PCM;		// ��Ƶ��ʽ
		m_wfx.nChannels			= 1;							// ��Ƶͨ������
		m_wfx.nSamplesPerSec	= 8000;						// ������
		m_wfx.wBitsPerSample	= 16;							
		m_wfx.nBlockAlign		= (WORD) ((m_wfx.wBitsPerSample / 8) * m_wfx.nChannels);
		m_wfx.nAvgBytesPerSec	= (DWORD) (m_wfx.nSamplesPerSec * m_wfx.nBlockAlign);
		
		m_nNotificationsNum		= nNotifyCount;
		m_nBufferPlayLength		= nPlayTime;

		//����һ��m_nBufferPlayLength��Ļ�����,��������������ֳ�m_nNotificationsNum��֪ͨ��;
		m_dwNotifySize = m_wfx.nSamplesPerSec*m_nBufferPlayLength*m_wfx.nBlockAlign/m_nNotificationsNum;
		//ȷ��m_dwNotifySize��nBlockAlign�ı���
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

		//����������
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
		// ���������߳�
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
	// ���Խ��벥�����̣�����ͨ��GetBuffer()ȡ�û����������������������Ƶ����
	// ����0ʱ�����벥������ʧ�ܣ����������߳��Ѿ����벥������
	// ���򷵻ص��ο������Ƶ�������ĳ���
	// �̻߳������ʱ���������StopPlay���˳��������̣���������������
	DWORD StartPlay()
	{
		// ÿ�����²���ʱ��Ƶ��������Ӧ���
		EnterCriticalSection(&m_csWaveBuffer);
		if (m_nWaveBufferSize < m_dwNotifySize*2 )	// ��Ҫ���󻺳���
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
	// �˳���������
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

	// ������Χ-10000,0
	// -10000	����,0	�������

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
	// ����Ƶ������ȡ����������Ƶ����
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
			// �Ѻ�δʹ������ǰ��
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
		{// ��������ݺ�����ἷ��ԭ������,��Ҫ����ԭ��������ǰ��һ������
			int nDiscardLen = m_nWaveDataLength + nDataLength - m_nWaveBufferSize;		// Ҫ���������ݳ���
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
// 			//�����������ʧ��Ҫ�ָ�
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
// 			//�򻺳���д����
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
// 		//�����������ٿ�������ʱ����������������֪��ԭ��
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
// 		//�����������ʧ��Ҫ�ָ�
// 		DWORD dwStatus;
// 		if( FAILED( hr = m_pDSBuffer->GetStatus( &dwStatus ) ) )		
// 		{
// 			e_log.WriteLog("m_pDSBuffer->GetStatus()ʧ��\n");
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
// 			//�򻺳���д����
// 			if(FAILED(hr = m_pDSBuffer->Lock(0, m_dwDSBufferSize, &pDSLockedBuffer,
// 				&dwDSLockedBufferSize,NULL,NULL,0L)))
// 			{
// 				e_log.WriteLog("RestoreBuffer(),Lockʧ��\n");
// 				return FALSE;
// 			}
// 
// 			//���ȶ������������ȵ�����
// 			m_pWaveFile->Read((BYTE *)pDSLockedBuffer,dwDSLockedBufferSize,&dwWavDataRead);
// 			if( dwWavDataRead == 0 )
// 			{
// 				// Wav is blank, so just fill with silence
// 				FillMemory( (BYTE*) pDSLockedBuffer, 
// 					dwDSLockedBufferSize, 
// 					(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
// 			}		
// 			else if(dwWavDataRead < dwDSLockedBufferSize)//����ļ�С�ڻ�����,�þ��������ʣ��Ŀռ�
// 			{
// 				FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, 
// 					dwDSLockedBufferSize - dwWavDataRead, 
// 					(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );			
// 			}
// 
// 			if(FAILED(hr = m_pDSBuffer->Unlock(pDSLockedBuffer,
// 				dwDSLockedBufferSize,NULL,NULL)))
// 			{
// 				e_log.WriteLog("RestoreBuffer(),Unlockʧ��\n");
// 				return FALSE;
// 			}
// 		}
// 		return TRUE;
// 	}
	//�������Ԫ�̺߳������Ա���ʸ���˽������
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

	int		m_nAudioFrames;		// Ͷ����Ƶ֡������
	CRITICAL_SECTION m_csBuffer;// ������������
	CRITICAL_SECTION m_csPlay;	// ���ڿ��Ʋ��ŵĻ���������ֹͬһ���󱻶���̲߳���
	CRITICAL_SECTION m_csDsound;// ���ڿ���m_pDirectSound���󣬷�ֹͬһ�����󱻶�δ���
	bool	m_bPlayed;			// �Ƿ��ѽ��벥������
	int		m_nBufferPlayLength;// �������ĳ���(��λΪ��)
	int		m_nNotificationsNum;// �û�������֪ͨ����
	HWND	m_hWnd;
	LPDIRECTSOUND m_pDirectSound;	
	LPDIRECTSOUNDBUFFER m_pDSBPrimary;	// ��������
	LPDIRECTSOUNDBUFFER m_pDSBSlavely;	// ��������
	DSBPOSITIONNOTIFY *m_pDSPosNotify;	// ����֪ͨ

	DWORD m_dwDSBufferSize;
	DWORD m_dwNotifySize;
	HANDLE* m_hEventArray;
	//CWinThread *m_pPlayThread;
	long m_lVolume;
	BOOL m_bPause;
	BOOL m_bMute;	
	WAVEFORMATEX m_wfx;        // Pointer to WAVEFORMATEX structure
	shared_ptr<byte> m_pWaveBuffer;
	int m_nWaveBufferSize;		// �������������ܳ���
	int m_nWaveDataLength;		// �������ݵĳ���
	CRITICAL_SECTION m_csWaveBuffer; //
	bool m_bPlayThreadRun;
	HANDLE m_hPlayThread;
};
