#ifndef _DVO_MEDIA_2015_09_07
#define _DVO_MEDIA_2015_09_07
#include <Windows.h>
#include <WTypes.h>
#include <time.h>
/// DVO¼���ļ���ʽ
#define DVO_TAG                 0x44564F4D //"MOVD"
#define GSJ_TAG					0x48574D49 //"IMWH"
#define DVO_IPC_SDK_VERSION_2015_09_07     0x62
#define DVO_IPC_SDK_VERSION_2015_10_20     0x63		///< �汾����,2015-10-20 ��ΪDVO_MEDIAINFO����nFps�ֶ�,���ֶ�֮ռ����֮ǰ��nCameraType
													///< �ֶε�λ��,Ϊ�������ֲ��,�԰汾������֮,���汾��Ϊ0x62ʱ,������fpsΪ0��1����ǿ��
													///< nFps=25,����ʹ��DVO_MEDIAINFO�е�nFps�ֶ�
#define DVO_IPC_SDK_VERSION_2015_12_16     0x64		///< �汾����,2015-11-09 ��ΪDVOFrameHeader��û��֡���,�ڲ���ʱ�޷�֪���ļ���֡��,��
													///< ����ļ����б���,���ܻ����֡��,�������DVOFrameHeader,����nFrameTime�ֶ�,���ڱ�ʶ
													///< ��Ƶ����Ƶ��ID,����ʱ,ֻ��Ҫȡ����һ֡��֡ID,���֪����֡��,�����Ч�ʡ�
#define DVO_IPC_SDK_VERSION     DVO_IPC_SDK_VERSION_2015_12_16		

/// @brief ֡����
enum FrameType
{
	FRAME_UNKNOWN	 = -1,
	FRAME_IDR		 = 1,
	FRAME_I			 = 2,
	FRAME_P			 = 3,
	FRAME_B			 = 4,
	FRAME_JPEG		 = 5,
	FRAME_G711A		 = 6,
	FRAME_G711U		 = 7,
	FRAME_G726		 = 8,
	FRAME_AAC		 = 9,
	Frame_DATA		 = 10
};

/// @brief ��������
enum DVO_CODEC
{
	CODEC_UNKNOWN = -1,
	CODEC_H264		 = 0x00,
	CODEC_MJPEG		 = 0x01,
	CODEC_H265		 = 0x02,
	CODEC_G711A		 = FRAME_G711A,
	CODEC_G711U		 = FRAME_G711U,
	CODEC_G726		 = FRAME_G726,
	CODEC_AAC		 = FRAME_AAC
};

#pragma pack(push)
#pragma pack(1)
///@brief DVO˽��¼���ļ�ͷ
struct 	DVO_MEDIAINFO
{
	DVO_MEDIAINFO()
	{
		ZeroMemory(this, sizeof(DVO_MEDIAINFO));
		nMediaTag	 = DVO_TAG;
		nSDKversion	 = DVO_IPC_SDK_VERSION;
	}
	~DVO_MEDIAINFO()
	{
	}
	unsigned int 	nMediaTag;		///< ͷ��־ �̶�Ϊ   0x44564F4D ���ַ���"MOVD"
	long			nSDKversion;	///< �̶�ֵΪ	DVO_IPC_SDK_VERSION
									///< �汾����,2015-12-26
									///< ��ΪDVO_MEDIAINFO����nFps�ֶ�,���ֶ�֮ռ����֮ǰ��nCameraType
									///< �ֶε�λ��,Ϊ�������ֲ��,�԰汾������֮,���汾��Ϊ0x62ʱ,������fpsΪ0��1����ǿ��
									///< nFps=25,����ʹ��DVO_MEDIAINFO�е�nFps�ֶ�
	DVO_CODEC		nVideoCodec;	///< ��Ƶ��������
	DVO_CODEC		nAudioCodec;	///< ��Ƶ��������
	int				nVideoWidth;	///< ��Ƶͼ���ȣ�������Ϊ0
	int				nVideoHeight;	///< ��Ƶͼ��߶ȣ�������Ϊ0
	unsigned char	nFps;			///< ֡��
	unsigned char	nCameraType;	///< �������DVO���Ϊ0����Ѷʿ���Ϊ1 fps
	unsigned char	reserved1[2];	///< ����,������0
	unsigned int	reserved2[3];	///< ����,������0
};

/// @brief DVO˽��¼��֡ͷ
struct DVOFrameHeader
{
	DVOFrameHeader()
	{
		ZeroMemory(this,sizeof(DVOFrameHeader));
		nFrameTag		 = DVO_TAG;
		nFrameUTCTime	 = (long)time(NULL);
	}
	~DVOFrameHeader()
	{
	}
	long	nLength;				///< �������ݳ���,���ֽ�Ϊ��λ
	long	nType;					///< ȡֵ��ö������FrameType
	__int64	nTimestamp;				///< ʱ���,��λ΢��
	long	nFrameTag;				///< DVO_TAG
	long	nFrameUTCTime;			///< �յ�֡��utcʱ��
};
/// @brief DVO˽��¼����չ֡ͷ
struct DVOFrameHeaderEx:public DVOFrameHeader
{
	DVOFrameHeaderEx()
	{
		ZeroMemory(this,sizeof(DVOFrameHeaderEx));
		nFrameTag		 = DVO_TAG;
		nFrameUTCTime	 = (long)time(NULL);
	}
	unsigned int	nFrameID;		///< ֡���,��Ƶ֡����Ƶ֡�ֱ��������,��0��ʼ����,����һ֡nFrameIDΪ0
#ifndef _DEBUG
	unsigned int	nReserver[3];	///< �����ֶ�,������0
#else
	unsigned int	nReserver;		///< �����ֶ�,������0
	double          dfRecvTime;
#endif
};
#pragma pack(pop)

/*
ÿ��¼���ļ��Ŀ�ʼλ�ã�д��̶���СΪ40���ֽڵ��ļ�ͷ,����ΪDVO_MEDIAINFO�ṹ�����ݡ�
ÿһ֡���ݣ�ǰ�����һ���̶���СΪ24���ֽڵ�֡ͷ����ÿһ֡��������Ϣ,����Ϊstream_head�ṹ�����ݣ�������д��֡���ݡ�

��ʽ������
�ļ�ͷ
֡����+֡����
֡����+֡����
...
֡����+֡����
*/

#endif	//_DVO_MEDIA_2015_09_07