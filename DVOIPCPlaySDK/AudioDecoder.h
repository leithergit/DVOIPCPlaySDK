#pragma once
#include <assert.h>
#include <windows.h>
#include <stdio.h>
#include <wtypes.h>
#include "DVOMedia.h"
#include "./AudioCodec/g711.h"
#include "./AudioCodec/g726.h"
#include "neaacdec.h"
#include "Utility.h"
#ifdef _DEBUG
#pragma comment(lib,"libfaadD")
#else
#pragma comment(lib,"libfaad")
#endif


#define AudioTrace	TraceMsgA

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

enum AudioSampleBit
{
	SampleBit8 = 1,
	SampleBit16,
	SampleBit24,
	SampleBit32,
	SampleBit40
};
#define FRAME_MAX_LEN 1024*8   
#define BUFFER_MAX_LEN 1024*1024  
struct  AccCodec
{
	NeAACDecHandle	hAacHandle;
	byte			*pAacBuffer;
	NeAACDecFrameInfo* pAacFrameInfo;
	unsigned long	nSamplerate;	
	int				nSamplebit;
	unsigned char	nChannels;
	bool			bInitialized;
	AccCodec()
	{
		ZeroMemory(this, sizeof(AccCodec));
		hAacHandle = NeAACDecOpen();
		if (!hAacHandle)		
			assert(false);
		
		pAacBuffer = new byte[FRAME_MAX_LEN];
		if (!pAacBuffer)
		{
			if (hAacHandle)
				NeAACDecClose(hAacHandle);
		}

		NeAACDecConfigurationPtr conf = NeAACDecGetCurrentConfiguration(hAacHandle);
		NeAACDecSetConfiguration(hAacHandle, conf);
	}
	~AccCodec()
	{
		if (hAacHandle)
		{
			NeAACDecClose(hAacHandle);
			hAacHandle = nullptr;
		}
	}
	/// @brief 音频解码
	/// pInputFrame一个完整的ADTS(audio data transfer stream)帧,Decode不负责校验pInputFrame是否正确
	/// @retval			0	操作成功
	/// @retval			-1	解码失败
	int Decode(OUT byte *pPCMdata, IN int &nPCMdataSize, IN unsigned char *pInputFrame, int nFrameSize)
	{
		if (!bInitialized)
		{			
			NeAACDecInit(hAacHandle, pInputFrame, nFrameSize, &nSamplerate, &nChannels);
			bInitialized = true;
		}
		pPCMdata = (unsigned char*)NeAACDecDecode(hAacHandle, pAacFrameInfo, pInputFrame, nFrameSize);
		if (pAacFrameInfo->error > 0)
		{
			AudioTrace("%s AccDecode Error:%s.\n",NeAACDecGetErrorMessage(pAacFrameInfo->error));
			return -1;
		}
		else if (pPCMdata && pAacFrameInfo->samples > 0)
		{
			AudioTrace("AacFrame info: bytesconsumed %d\tchannels %d\theader_type %d\tobject_type %d\tsamples %d\t samplerate %d\n",
				pAacFrameInfo->bytesconsumed,
				pAacFrameInfo->channels,
				pAacFrameInfo->header_type,
				pAacFrameInfo->object_type,
				pAacFrameInfo->samples,
				pAacFrameInfo->samplerate);
			nPCMdataSize = pAacFrameInfo->samples*pAacFrameInfo->channels;
			return 0;
		}
		else
			return -1;
	}
	
	/// @brief 从输入数据流中取得一个ADTS帧
	int Get_ADTS_Frame(unsigned char* pAacstream, size_t nStreamLenggth, unsigned char* pAacFrame, size_t* nFrameSize)
	{
		size_t size = 0;

		if (!pAacstream || nStreamLenggth < 7 || !pAacFrame || !nFrameSize)
		{
			return -1;
		}

		while (1)
		{
			if (nStreamLenggth  < 7)
			{
				return -1;
			}

			if ((pAacstream[0] == 0xff) && ((pAacstream[1] & 0xf0) == 0xf0))
			{
				size |= ((pAacstream[3] & 0x03) << 11);		//high 2 bit  
				size |= pAacstream[4] << 3;					//middle 8 bit  
				size |= ((pAacstream[5] & 0xe0) >> 5);      //low 3bit  
				break;
			}
			--nStreamLenggth;
			++pAacstream;
		}

		if (nStreamLenggth < size)
		{
			return -1;
		}

		memcpy(pAacFrame, pAacstream, size);
		*nFrameSize = size;
		return 0;
	}
};
class CAudioDecoder
{
private:
	DVO_CODEC	m_nAudioCodec;
	g726_state_t *m_pG726State;
	AccCodec	*m_pAacDecoder;	
public:
	CAudioDecoder()
	{
		ZeroMemory(this, sizeof(CAudioDecoder));
		m_nAudioCodec = CODEC_UNKNOWN;
	}
	bool SetACodecType(DVO_CODEC nAudioCodec, /*int nSampleRate,int nChannels,*/AudioSampleBit nSamplebit)
	{
		switch (nAudioCodec)
		{
		case CODEC_G711U:
		case CODEC_G711A:
			m_nAudioCodec = nAudioCodec;
			break;
		case CODEC_G726:
		{
			m_nAudioCodec = nAudioCodec;
			m_pG726State = new g726_state_t;
			switch (nSamplebit)
			{
			case SampleBit8:				
			case SampleBit16:				
			case SampleBit24:				
			case SampleBit32:				
			case SampleBit40:
				break;
			default:
			{
				assert(false); 
				return false;
			}
			}
			m_pG726State = g726_init(m_pG726State, 8000 * nSamplebit);
		}			
		case CODEC_AAC:
		{
			m_nAudioCodec = nAudioCodec;
			m_pAacDecoder = new AccCodec();
			if (!m_pAacDecoder)
				return false;
			break;
		}
		default:
		{
			assert(false);
			return false;
		}
		}
	}
	int Decode(OUT byte *pPCMData, INOUT int &nPCMdataSize, IN byte *pAudioFrame,IN int nFrameSize)
	{
		switch (m_nAudioCodec)
		{
		case CODEC_G711U:
			return g711u_decode((short*)pPCMData, pAudioFrame, nFrameSize);
			break;
		case CODEC_G711A:
			return g711a_decode((short*)pPCMData, pAudioFrame, nFrameSize);
			break;
		case CODEC_G726:
			return g726_decode(m_pG726State, (short*)pPCMData, pAudioFrame, nFrameSize);
			break;
		case CODEC_AAC:
			return m_pAacDecoder->Decode(pPCMData, nPCMdataSize, pAudioFrame, nFrameSize);
			break;
		default:
		{
			assert(false);
			return -1;
		}
			break;
		}
	}
	~CAudioDecoder()
	{
		if (m_pG726State)
			delete m_pG726State;
		if (m_pAacDecoder)
			delete m_pAacDecoder;
	}
};

