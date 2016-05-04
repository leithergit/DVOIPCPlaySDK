/***********************************************************************
** Copyright (C) Tongwei Video Technology Co.,Ltd. All rights reserved.
** Author       : YSW
** Date         : 2010-11-19
** Name         : dvrdvsconfig.h
** Version      : 1.0
** Description  :	����DVR/DVS��ص�һЩ��������
** Modify Record:
1:����
***********************************************************************/
#ifndef __DVR_DVS_CONFIG_H__
#define __DVR_DVS_CONFIG_H__

#include "dvrdvstypedef.h"
//////////////////////////////////////////////////////////////////////////
const unsigned long DD_MAX_CAMERA_NUM	= 128;
const unsigned long DD_MAX_CAMERA_NUM_BYTE_LEN	= DD_MAX_CAMERA_NUM/8;
const unsigned long DD_MAX_SERIAL_NUMBER_LEN = 64;
const unsigned long DD_MAX_VERSION_BUF_LEN = 64;

const unsigned long	DD_MAX_NAME_LEN		= 64;	
const unsigned long	DD_MAX_NAME_BUF_LEN	= (DD_MAX_NAME_LEN * 2) + 4;

const unsigned long	DD_MAX_CAMERA_NAME_LEN		= 64;	
const unsigned long	DD_MAX_CAMERA_NAME_BUF_LEN	= (DD_MAX_CAMERA_NAME_LEN * 2) + 4;

const unsigned long DD_MAX_URL_LEN		= 256;
const unsigned long DD_MAX_URL_BUF_LEN	= DD_MAX_URL_LEN + 4;

const unsigned long DD_MAX_COLOR_CFG_NUM = 3;

const unsigned long DD_MAX_TEXT_LEN		= 64;
const unsigned long	DD_MAX_TEXT_BUF_LEN	= (DD_MAX_TEXT_LEN * 2) + 4;

const unsigned long DD_MAX_VIDEO_COVER_NUM = 3;

const unsigned long DD_MAX_USER_NAME_LEN	= 64;
const unsigned long DD_MAX_USER_NAME_BUF_LEN = (DD_MAX_USER_NAME_LEN * 2) + 4;
const unsigned long DD_MAX_PASSWORD_LEN		= 128;
const unsigned long DD_MAX_PASSWORD_BUF_LEN	= DD_MAX_PASSWORD_LEN + 4;

const unsigned long DD_MAX_PPPOE_ACCOUNT_LEN		= 128;
const unsigned long DD_MAX_PPPOE_ACCOUNT_BUF_LEN	= DD_MAX_PPPOE_ACCOUNT_LEN + 4;

const unsigned long DD_MAX_DDNS_ACCOUNT_LEN			= 128;
const unsigned long DD_MAX_DDNS_ACCOUNT_BUF_LEN		= DD_MAX_DDNS_ACCOUNT_LEN + 4;

const unsigned long DD_MAX_EMAIL_RECEIVE_ADDR_NUM = 3;

const unsigned long DD_MAX_MOTION_AREA_WIDTH_NUM = 1920/16;
const unsigned long DD_MAX_MOTION_AREA_HIGHT_NUM = (1080/16+3)&(~3);

const unsigned long DD_MAX_PRESET_NUM	= 128;
const unsigned long DD_MAX_CRUISE_NUM	= 32;
const unsigned long DD_MAX_TRACK_NUM	= 1;
const unsigned long DD_MAX_ACCOUNT_NUM	= 64;
const unsigned long DD_MAX_BUF_SIZE = 512*1024;

const unsigned long DD_MAX_SUPPORT_RESOLUTION = 7;
//////////////////////////////////////////////////////////////////////////

//�豸��Ϣ�����������ݣ������������ã�Ҳ�����ڲ�ѯ�豸��Ϣ
typedef struct _dd_device_info_
{
	unsigned long	iSize;		//���ṹ�峤��

	unsigned long	deviceID;	//�豸ID(0~255)
	char			deviceNo[DD_MAX_SERIAL_NUMBER_LEN];		//�豸���кţ�����ʹ����ĸ
	char			deviceName [DD_MAX_NAME_LEN];			//�豸���ƣ�ע�⿼��˫�ֽ��ַ���

	//����Ϊֻ����
	//version
	char			firmwareVersion [DD_MAX_VERSION_BUF_LEN];
	char			firmwareBuildDate [DD_MAX_VERSION_BUF_LEN];
	char			hardwareVersion [DD_MAX_VERSION_BUF_LEN];
	char			kernelVersion [DD_MAX_VERSION_BUF_LEN];
	char			mcuVersion [DD_MAX_VERSION_BUF_LEN];

	//�ӿڲ���
	unsigned char	audioNum;
	unsigned char	localVideoInNum;
	unsigned char	netVideoInNum;
	unsigned char	sensorInNum;
	unsigned char	relayOutNum;

	unsigned char	rs232Num;
	unsigned char	rs485Num;
	unsigned char	networkPortNum;
	unsigned char	diskCtrlNum;
	unsigned char	DiskNum;
	unsigned char	vgaNum;
	unsigned char	usbNum;
}DD_DEVICE_INFO;

typedef struct _dd_basic_config_
{
	unsigned long iSize;				//���ṹ�峤��

	unsigned long videoFormat;			//��Ƶ��ʽ��ȡֵ�ο�DD_VIDEO_FORMAT

	unsigned long videoOut;				//��Ƶ����豸
	unsigned long videoOutResolution;	//����ֱ��ʣ�ȡֵ�ο�DD_VGA_RESOLUTION
	unsigned long VGARefresh;			//VGAˢ����
	unsigned long screensaver;			//����ʱ�䣨���ʾ��������

	unsigned long deviceLanguage;		//ϵͳ����
	unsigned long passwordCheck;		//�Ƿ���������

	unsigned long RecycleRecord;		//�Ƿ�������¼��

	//����ΪһЩֻ����
	unsigned long videoFormatMask;			//֧�ֵ���Ƶ��ʽ���루ֻ������ȡֵΪDD_VIDEO_FORMAT�������ݵ��������
	unsigned long videoOutMask;				//֧�ֵ���Ƶ����豸���루ֻ����
	unsigned long videoOutResolutionMask;	//֧�ֵ���Ƶ����豸�ֱ������루ֻ������ȡֵΪDD_VGA_RESOLUTION�������ݵ��������
	unsigned long languageMask;				//֧�ֵ��������루ֻ����
}DD_BASIC_CONFIG;

typedef struct _dd_date_time_config
{
	unsigned long iSize;		//���ṹ�峤��

	unsigned char dateFormat;	//���ڸ�ʽ��ȡֵΪDD_DATE_MODE
	unsigned char timeFormat;	//ʱ���ʽ��12����12Сʱ�ƣ�24����24Сʱ��
	unsigned char timeZone;		//ʱ����ȡֵΪDD_TIME_ZOME_NAME

	unsigned char enableNTP;	//�Ƿ���NTPͬ������
	unsigned short	ntpPort;	//NTP�˿�
	unsigned short	recv;
	char	ntpServerAddr[DD_MAX_URL_BUF_LEN];	//NTP��������ַ
}DD_DATE_TIME_CONFIG;

typedef struct _dd_daylight_info_
{
	unsigned char InMonth;		//�ڼ��½���DST
	unsigned char InMday;		//�ڼ��ս���DST�����ڷ�ʽ��Ч��
	unsigned char OutMonth;		//�ڼ��³�DST
	unsigned char OutMday;		//�ڼ��ճ�DST�����ڷ�ʽ��Ч��

	unsigned char InWeekIndex;	//�ڼ��ܽ���DST���ܷ�ʽ��Ч��
	unsigned char InWday;		//���ڼ�����DST���ܷ�ʽ��Ч��
	unsigned char OutWeekIndex;	//�ڼ��ܳ�DST���ܷ�ʽ��Ч��
	unsigned char OutWday;		//���ڼ���DST���ܷ�ʽ��Ч��

	unsigned short InYear;		//����DST����ݣ��������ã����Ƿ��ڴˣ��պÿ��Զ���ṹ�壬���Ա�����
	unsigned short OutYear;		//��DST����ݣ��������ã����Ƿ��ڴˣ��պÿ��Զ���ṹ�壬���Ա�����

	unsigned short enable;		//�Ƿ�������ʱ����
	unsigned short type;		//����ʱ���÷�ʽ���ܻ������ڷ�ʽ

	unsigned long InSecond;		//����DSTһ���ڵ���ƫ�ƣ�0��86399������ת����ʱ����
	unsigned long OutSecond;	//��DSTһ���ڵ���ƫ�ƣ�0��86399������ת����ʱ����

	unsigned long offSet;		//����ʱ��ƫ������0��86399���롣

}DD_DAYLIGHT_INFO;

typedef struct _dd_live_video_group_
{
	unsigned short	holdTime;	//ͣ��ʱ�䣨�룩,���ʾ��Ч
	unsigned short	channelNum;	//��Чͨ����Ŀ��ֻ����

	unsigned long	splitMode;	//�ָ�ģʽ��ȡֵ�ο�DD_VIEW_SPLIT_MODE

	//�������Ӧ��ͨ����ţ������±����ͨ����ţ�Ԫ��ֵ������������
	//0xff��ʾ��ͨ����Ч
	unsigned char	channel [DD_MAX_CAMERA_NUM]; //�������Ӧ��ͨ�����
}DD_LIVE_VIDEO_GROUP;

typedef struct _dd_live_audio_group_
{
	unsigned short holdTime;		//ͣ��ʱ�䣨�룩,���ʾ��Ч
	unsigned char volume;			//������0~100)
	unsigned char channel;			//ͨ�����
}DD_LIVE_AUDIO_GROUP;

typedef struct _dd_live_display_
{
	unsigned long iSize;			//���ṹ�峤��

	unsigned long showTime;			//�Ƿ���ʾϵͳʱ��
	unsigned long showNetwork;		//�Ƿ���ʾ����״̬
	unsigned long showHDD;			//�Ƿ���ʾӲ����Ϣ
	unsigned long showUSB;			//�Ƿ���ʾ�ƶ��洢����Ϣ

	unsigned short	alarmInNum;		//����������Ŀ��ֻ����
	unsigned short	alarmOutNum;	//���������Ŀ��ֻ����
	unsigned long	showAlarmIn;	//�Ƿ���ʾ����������Ϣ	
	unsigned long	showAlarmOut;	//�Ƿ���ʾ���������Ϣ

	unsigned long cameraNum;		//��Чͨ����Ŀ��ֻ����
	unsigned char showCameraName [DD_MAX_CAMERA_NUM];	//�Ƿ���ʾͨ������
	unsigned char showRecordStatus [DD_MAX_CAMERA_NUM];	//�Ƿ���ʾ¼��״̬
}DD_LIVE_DISPLAY;

typedef struct _dd_channel_config_
{
	unsigned long iSize;						//���ṹ�峤��

	unsigned long hide;							//�Ƿ�����ͨ��
	char	name [DD_MAX_CAMERA_NAME_BUF_LEN];	//ͨ������
}DD_CHANNEL_CONFIG;

typedef struct _dd_video_color_
{
	unsigned long	startTime;		//����ɫ�ʶ�Ӧ�Ŀ�ʼʱ�䣨һ�������ʱ�䣩

	unsigned char	brightness;		//����
	unsigned char	hue;			//ɫ��
	unsigned char	saturation;		//���Ͷ�
	unsigned char	contrast;		//�Աȶ�
}DD_VIDEO_COLOR;

typedef struct _dd_video_color_config_
{
	unsigned long	iSize;			//���ṹ�峤��

	unsigned long	usedNum;		//ʹ�õķ�����Ŀ��Ĭ��Ϊһ
	DD_VIDEO_COLOR	videoColor[DD_MAX_COLOR_CFG_NUM];	//ɫ�ʷ���
}DD_VIDEO_COLOR_CONFIG;

typedef struct _dd_position_
{
	unsigned short x;		//������
	unsigned short y;		//������
}DD_POSITION;

typedef struct _dd_area_
{
	unsigned short x;		//������
	unsigned short y;		//������

	unsigned short cx;		//���
	unsigned short cy;		//�߶�
}DD_AREA;

typedef struct _dd_video_osd_config_
{
	unsigned long iSize;					//���ṹ�峤��

	unsigned char enableCameraName;			//����ͨ������
	unsigned char enableTimeStamp;			//����ʱ���
	unsigned char enableTimeStampWithWeek;	//ʱ����ϼ�����
	unsigned char enableDefineText;			//�����Զ����ı�

	DD_POSITION cameraName;					//ͨ�����Ƶ�λ��
	DD_POSITION timeStamp;					//ʱ�����λ��
	DD_POSITION defineText;					//�Զ����ı���λ��
	char text [DD_MAX_TEXT_BUF_LEN];		//�Զ����ı�

	struct  
	{
		unsigned long	enable;				//���򸲸��Ƿ���Ч
		DD_AREA			area;				//�������
	}cover[DD_MAX_VIDEO_COVER_NUM];			//���ǵ��������
}DD_VIDEO_OSD_CONFIG;

typedef struct _dd_encode_config_
{
	unsigned long iSize;		//���ṹ�峤��

	unsigned short resolution;	//�ֱ��ʣ�ȡֵ�ο�DD_VIDEO_SIZE
	unsigned short rate;		//֡��
	unsigned short encodeType;	//�������ͣ�ȡֵ�ο�DD_VIDEO_ENCODE_MODE
	unsigned short quality;		//���ʣ�ȡֵ�ο�DD_IMAGE_QUALITY

	unsigned short minBitrate;	//��������
	unsigned short maxBitrate;	//��������
}DD_ENCODE_CONFIG;

typedef struct _dd_encode_config_support_
{
	DD_ENCODE_CONFIG encodeConfig[DD_MAX_SUPPORT_RESOLUTION];
	unsigned long num;
}DD_ENCODE_CONFIG_SUPPORT;

typedef struct _dd_record_config_
{
	unsigned long iSize;			//���ṹ�峤��

	unsigned char bOnlyVideo;		//¼����Ƶ������Ƶ��
	unsigned char bWithAudio;		//¼����Ƶ������¼����Ƶ�Ļ����ϣ�
	unsigned char bindAudioChannel;	//��Ӧ����Ƶͨ������һ������Ƶͨ�����һ�£�
	unsigned char bRedundancy;		//�Ƿ�����¼��

	unsigned short preAlarmTime;	//��ǰ¼��ʱ��
	unsigned short postAlarmTime;	//����¼��ʱ��

	unsigned short expired;			//¼�����ʱ��
	unsigned short recv;			//�����ֽ�
}DD_RECORD_CONFIG;

//�˽ṹ��Ϊ��ֻ�������ݣ����ڲ�ѯ�豸֧�ֲ�����
typedef struct _dd_record_config_mask_
{
	unsigned long iSize;

	unsigned char bindAudioChannel;	//�Ƿ�֧������Ƶͨ����
	unsigned char bRedundancy;		//�Ƿ�֧������¼��
	unsigned char recv1;
	unsigned char recv2;

	unsigned short minPreAlarmTime;	//��С��ǰ¼��ʱ��
	unsigned short maxPreAlarmTime;	//���ǰ¼��ʱ��

	unsigned short minPostAlarmTime;	//��С����¼��ʱ��
	unsigned short maxPostAlarmTime;	//��󾯺�¼��ʱ��

	unsigned short minExpired;		//��С¼�����ݹ���ʱ��
	unsigned short maxExpired;		//���¼�����ݹ���ʱ��
}DD_RECORD_CONFIG_MASK;

//���ų̽ṹ��24����24Сʱ��unsigned long long��ÿһλ����ÿһ���ӵ�״̬
typedef struct _dd_date_schedule_
{
	unsigned long long hour [24];
}DD_DATE_SCHEDULE;

//���ų̽ṹ��7��ʾÿ��7����ų�
typedef struct _dd_week_schedule_
{
	DD_DATE_SCHEDULE week[7];
}DD_WEEK_SCHEDULE;

typedef struct _dd_network_ip_config_
{
	unsigned long iSize;			//���ṹ�峤��

	unsigned long	useDHCP;		//�Ƿ�ʹ�ö�̬�����ַ

	unsigned long	IP;				//�����ַ
	unsigned long	subnetMask;		//��������
	unsigned long	gateway;		//����

	unsigned long	preferredDNS;	//������������������ַ
	unsigned long	alternateDNS;	//������������������ַ

	unsigned long	usePPPoE;		//�Ƿ�ʹ��PPPoE
	char account[DD_MAX_PPPOE_ACCOUNT_BUF_LEN];	//PPPoE���ʺ�
	char password[DD_MAX_PASSWORD_BUF_LEN];		//PPPoE������
}DD_NETWORK_IP_CONFIG;

typedef struct _dd_network_advance_config_
{
	unsigned long iSize;			//���ṹ�峤��

	unsigned char bMessagePort;		//�Ƿ�֧����Ϣ�˿ڣ�ֻ����
	unsigned char bAlarmPort;		//�Ƿ�֧�ֱ����˿ڣ�ֻ����
	unsigned char bMultiCastIP;		//�Ƿ�֧�ֶಥ��ַ��ֻ����
	unsigned char bMTUByteNum;		//�Ƿ�֧��MTU�ֽ�����ֻ����

	unsigned short httpPort;		//HTTP�˿�
	unsigned short datePort;		//���ݶ˿�
	unsigned short messagePort;		//��Ϣ����˿�
	unsigned short alarmPort;		//�����˿�

	unsigned short maxOnlineUserNum;	//֧�ֵ���������û���Ŀ��ֻ����
	unsigned short OnlineUserNum;		//�����û���Ŀ

	unsigned long multiCastIP;		//�ಥ��ַ
	unsigned long mtuByteNum;		//MTU�ֽ���
}DD_NETWORK_ADVANCE_CONFIG;

typedef struct _dd_ddns_config_
{
	unsigned long iSize;			//���ṹ�峤��

	unsigned short enable;			//�Ƿ�����DDNS
	unsigned short interval;		//�ϱ���������

	unsigned long useDDNSServer;	//ʹ�õ�DDNS���������ͻ��ߵ�ַ

	unsigned long userHostDomain;	//�Ƿ��������������

	char userName [DD_MAX_DDNS_ACCOUNT_BUF_LEN];	//DDNS�ʺ�
	char password [DD_MAX_PASSWORD_BUF_LEN];		//DDNS����

	char hostDomain [DD_MAX_URL_BUF_LEN];			//������������������Ӧĳ��Э�飬����ָ����������
}DD_DDNS_CONFIG;

typedef struct _dd_ddns_server_info 
{
	unsigned char  DDNSID; /////��Ӧ�ķ�����ID   ID����0��Ӧ�ķ��������Ʋ���Ч
	unsigned char  supportproperty;//֧�ֵĳ��û�����������������Ե�MASK,NCFG_ENUM_DDNS_SUPPORT_PROPERTY
	unsigned char  noused[2];
	char           DDNSServerName[64];  ////DDNS�������ĵ�ַ
}DD_DDNS_SERVER_INFO;

typedef struct _dd_smtp_config_
{
	unsigned long iSize;				//���ṹ�峤��

	unsigned short port;				//�ʼ�����˿�
	unsigned short enableSSL;			//�Ƿ�����SSL���

	char server [DD_MAX_URL_BUF_LEN];	//���ͷ�������ַ

	char sendAddress [DD_MAX_URL_BUF_LEN];	//�����ʼ���ַ
	char password [DD_MAX_PASSWORD_BUF_LEN];//�ʺ�����

	unsigned long enableRecvAddrNum;		//���õĽ��յ�ַ��Ŀ��ֻ����
	char receiveAddress [DD_MAX_EMAIL_RECEIVE_ADDR_NUM][DD_MAX_URL_BUF_LEN];//���յ�ַ�б�
}DD_SMTP_CONFIG;

typedef struct _dd_auto_report_
{
	unsigned long   bUse;			//�Ƿ������Զ��ϱ�ע�Ṧ��
	char			host[256];		//ע���ƽ̨��������ַ
	unsigned long	dwPort;			//ע���ƽ̨�������˿�
	unsigned long	ID;				//�����ע��ID
}DD_AUTO_REPORT;
typedef struct _dd_account_config_
{
	unsigned long iSize;			//���ṹ�峤��

	unsigned long enable;			//�Ƿ����ø��ʺ�
	unsigned long bindMAC;			//�Ƿ�������ַ
	unsigned long group;			//����Ⱥ�飬ȡֵ�ο�DD_USER_GROUP

	char MAC [8];					//���󶨵������ַ
	char name [DD_MAX_USER_NAME_BUF_LEN];	//�û�����
	char password [DD_MAX_PASSWORD_BUF_LEN];//�ʺ�����

	//������Ȩ�޵��������
	unsigned char logSearch;		//��־����Ȩ��
	unsigned char systemSetup;		//ϵͳ����
	unsigned char fileManagement;	//�ļ�����
	unsigned char diskManagement;	//���̹���
	unsigned char remoteLogin;		//Զ�̵���
	unsigned char twoWayAudio;		//�����Խ�
	unsigned char systemMaintain;	//ϵͳά��
	unsigned char OnlineUserManagement;	//�����û�����
	unsigned char shutdown;			//�ػ���������
	unsigned char alarmOutCtrl;		//�����������
	unsigned char netAlarm;			//���籨��
	unsigned char netSerialCtrl;	//���紮�ڿ���
	
	unsigned char authLive;
	unsigned char authRecord;
	unsigned char authPlayback;
	unsigned char authBackup;
	unsigned char authPTZ;
	unsigned char netAuthView;
	unsigned char netauthRecord;
	unsigned char netauthPlayback;
	unsigned char netauthBackup;
	unsigned char netauthPTZ;
	unsigned char recv[2];

	unsigned char authLiveCH [DD_MAX_CAMERA_NUM_BYTE_LEN];          //�ֳ�Ԥ��ͨ��
	unsigned char authRecordCH [DD_MAX_CAMERA_NUM_BYTE_LEN];		//�����ֶ�¼��
	unsigned char authPlaybackCH [DD_MAX_CAMERA_NUM_BYTE_LEN];		//���ؼ������ط�
	unsigned char authBackupCH [DD_MAX_CAMERA_NUM_BYTE_LEN];		//���ر���
	unsigned char authPTZCH [DD_MAX_CAMERA_NUM_BYTE_LEN];			//������̨����

	unsigned char netAuthViewCH [DD_MAX_CAMERA_NUM_BYTE_LEN];		//Զ���ֳ�Ԥ��
	unsigned char netAuthRecordCH [DD_MAX_CAMERA_NUM_BYTE_LEN];		//Զ���ֶ�¼��
	unsigned char netAuthPlaybackCH [DD_MAX_CAMERA_NUM_BYTE_LEN];	//Զ�̻ط�
	unsigned char netAuthBackupCH [DD_MAX_CAMERA_NUM_BYTE_LEN];		//Զ�̱���
	unsigned char netAuthPTZCH [DD_MAX_CAMERA_NUM_BYTE_LEN];		//Զ����̨����
}DD_ACCOUNT_CONFIG;

typedef struct _dd_trigger_alarm_out_
{
	unsigned char toBuzzer;					//��������������
	unsigned char ShowFullScreen;			//�������汨����ͨ���ţ�0xffΪ��������
	unsigned char sendEmail;				//�����ʼ�
	unsigned char toUploadToAlarmCentre;	//�ϴ�����������

	unsigned long toAlarmOut;				//�������������λ��Ӧ����豸��
}DD_TRIGGER_ALARM_OUT;

typedef struct _dd_trigger_record_
{
	unsigned char snapCH [DD_MAX_CAMERA_NUM_BYTE_LEN];		//����ץͼ
	unsigned char recordCH [DD_MAX_CAMERA_NUM_BYTE_LEN];	//����¼��
}DD_TRIGGER_RECORD;

typedef struct _dd_trigger_ptz_
{
	unsigned char toPTZType;	//�������ͣ�Ԥ�õ㡢Ѳ���ߡ��켣��DD_PTZ_TYPE

	unsigned char toIndex;		//�����ģ�Ԥ�õ㡢Ѳ���ߡ��켣�����
	unsigned char backIndex;	//�����ķ��أ�Ԥ�õ㡢Ѳ���ߡ��켣�����
	unsigned char recv;			//�����ֽ�
}DD_TRIGGER_PTZ;

typedef struct _dd_sensor_config_
{
	unsigned long iSize;		//���ṹ�峤��

	unsigned char	enable;		//�Ƿ��������
	unsigned char	bNO;		//�豸���ͣ��������߳���
	unsigned short	holdTime;	//�ӳ�ʱ��

	char name [DD_MAX_NAME_BUF_LEN];	//�豸����
}DD_SENSOR_CONFIG;

typedef struct _dd_motion_area_
{
	unsigned long sensitivity;		//�����ȣ�0��7��������Խ��������Խ��

	unsigned short widthNum;		//����ĺ���դ����Ŀ
	unsigned short heightNum;		//���������դ����Ŀ

	//�����դ���������ݣ�����1920x1080��ÿ��8X8��С��
	unsigned char area [DD_MAX_MOTION_AREA_HIGHT_NUM][DD_MAX_MOTION_AREA_WIDTH_NUM];
}DD_MOTION_AREA;

typedef struct _dd_motion_config_
{
	unsigned long iSize;		//���ṹ�峤��

	unsigned char	enable;		//�Ƿ������ƶ����
	unsigned char	recv;		//�����ֽ�

	unsigned short	holdTime;	//�ӳ�ʱ��

	DD_MOTION_AREA	area;		//��������
}DD_MOTION_CONFIG;

typedef struct _dd_buzzer_config_
{
	unsigned char enable;		//���������ÿ���
	unsigned char recv;			//�����ֽ�

	unsigned short holdTime;	//�ӳ�ʱ��
}DD_BUZZER_CONFIG;

typedef struct _dd_relay_config_
{
	unsigned char enable;		//��������豸���ÿ���
	unsigned char recv;			//�����ֽ�

	unsigned short holdTime;	//�ӳ�ʱ��

	char name [DD_MAX_NAME_BUF_LEN];	//�豸����
}DD_RELAY_CONFIG;

typedef struct _dd_serial_config_
{
	unsigned long baudRate;			//������
	unsigned long dataBit;			//����λ
	unsigned long stopBit;			//ֹͣλ
	unsigned long parity;			//��żУ��λ
	unsigned long dataFlowControl;	//����������
}DD_SERIAL_CONFIG;

typedef struct _dd_ptz_config_
{
	unsigned long iSize;		//���ṹ�峤��

	unsigned char enable;		//�Ƿ�������̨����
	unsigned char address;		//��ַ
	unsigned char recv1;		//�����ֽ�
	unsigned char recv2;		//�����ֽ�

	unsigned long protocol;		//Э��

	DD_SERIAL_CONFIG serial;	//����
}DD_PTZ_CONFIG;

typedef struct _dd_ptz_protocol_info 
{
	unsigned long  protocolID;		//��Ӧ��Э������ID
	unsigned long  protocolProperty;//֧�ֵĳ�ID��������֮����������Ե�MASK,�����Ƿ�֧�ֹ켣����������
	char           ProtocolName[64];//Э������
}DD_PTZ_PROTOCOL_INFO;

typedef struct _dd_ptz_preset_config_
{
	unsigned long iSize;		//���ṹ�峤��

	//Ԥ�õ�����
	unsigned char enablePreset [DD_MAX_PRESET_NUM];
}DD_PTZ_PRESET_CONFIG;

//////////////////////////////////////////////////////////////////////////
typedef struct _dd_config_item_head_
{
	unsigned short	itemID;		//ID
	unsigned short	num;		//Ԫ����Ŀ��������ٸ�ͨ����ÿ��ͨ����Ӧһ��Ԫ��
	unsigned short	subLen;		//ÿ��Ԫ�صĳ���
	unsigned short recv;
	unsigned long	len;		//�����ܳ���
}DD_CONFIG_ITEM_HEAD;


typedef struct _dd_config_block_head_
{
	unsigned long	biSize;		//���ṹ�峤��
	unsigned long	ItemNum;	//Ԫ����Ŀ
	unsigned long	netcfgver;	//��ǰ����İ汾�ţ��豸�˹̶�����DD_CONFIG_VERSION���ͻ��˸�����Ҫ��̬�ж�
}DD_CONFIG_BLOCK_HEAD;

typedef struct _dd_config_item_head_ex_
{
	DD_CONFIG_ITEM_HEAD		item_head;
	unsigned long			start_pos;		//�ļ���ƫ��
}DD_CONFIG_ITEM_HEAD_EX;		

typedef struct _dd_config_block_info_
{
	DD_CONFIG_ITEM_HEAD_EX	itemHead;
	unsigned long			off;			//������ƫ��
}DD_CONFIG_BLOCK_INFO;

//////////////////////////////////////////////////////////////////////////
const unsigned long		DD_CONFIG_VERSION	= 20101123;

typedef enum dd_config_item_id
{
	DD_CONFIG_ITEM_SYSTEM_BASE=0x0100,
	DD_CONFIG_ITEM_DEVICE_INFO,				//DD_DEVICE_INFO
	DD_CONFIG_ITEM_SYSTEM_BASIC,			//DD_BASIC_CONFIG
	DD_CONFIG_ITEM_DATE_TIME,				//DD_DATE_TIME_CONFIG
	DD_CONFIG_ITEM_DAYLIGHT_INFO,			//DD_DAYLIGHT_INFO
	DD_CONFIG_ITEM_SYSTEM_END,

	DD_CONFIG_ITEM_LIVE_BASE=0x0200,
	DD_CONFIG_ITEM_LIVE_MAIN_CAMERA,		//DD_LIVE_VIDEO_GROUP * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_LIVE_SPOT_CAMERA,		//DD_LIVE_VIDEO_GROUP * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_LIVE_AUDIO,				//DD_LIVE_AUDIO_GROUP * AUDIO_INPUT_NUM
	DD_CONFIG_ITEM_LIVE_DISPLAY,			//DD_LIVE_DISPLAY
	DD_CONFIG_ITEM_LIVE_END,

	DD_CONFIG_ITEM_CHNN_BASE=0x0300,
	DD_CONFIG_ITEM_CHNN_CONFIG,				//DD_CHANNEL_CONFIG * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_VIDEO_COLOR,				//DD_VIDEO_COLOR_CONFIG * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_VIDEO_OSD,				//DD_VIDEO_OSD_CONFIG * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_CHNN_END,

	DD_CONFIG_ITEM_ENCODE_BASE=0x0400,
	DD_CONFIG_ITEM_ENCODE_MASK_MAJOR,		//DD_ENCODE_CONFIG_SUPPORT
	DD_CONFIG_ITEM_ENCODE_MASK_MINOR,		//DD_ENCODE_CONFIG_SUPPORT
	DD_CONFIG_ITEM_ENCODE_SCHEDULE,			//DD_ENCODE_CONFIG * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_ENCODE_ALARM,			//DD_ENCODE_CONFIG * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_ENCODE_NETWORK,			//DD_ENCODE_CONFIG * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_ENCODE_END,

	DD_CONFIG_ITEM_RECORD_BASE=0x0500,
	DD_CONFIG_ITEM_RECORD_MASK,				//DD_RECORD_CONFIG_MASK
	DD_CONFIG_ITEM_RECORD_SETUP,			//DD_RECORD_CONFIG * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_RECORD_SCHEDULE_SCHEDULE,//DD_WEEK_SCHEDULE * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_RECORD_SCHEDULE_MOTION,	//DD_WEEK_SCHEDULE * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_RECORD_SCHEDULE_SENSOR,	//DD_WEEK_SCHEDULE * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_RECORD_END,

	DD_CONFIG_ITEM_NETWORK_BASE=0x0600,
	DD_CONFIG_ITEM_NETWORK_IP,				//DD_NETWORK_IP_CONFIG
	DD_CONFIG_ITEM_NETWORK_ADVANCE,			//DD_NETWORK_ADVANCE_CONFIG
	DD_CONFIG_ITEM_NETWORK_DDNS,			//DD_DDNS_CONFIG
	DD_CONFIG_ITEM_DDNS_SERVER_INFO,		//DD_DDNS_SERVER_INFO
	DD_CONFIG_ITEM_NETWORK_SMTP,			//DD_SMTP_CONFIG
//2012-3-15����DVR����ע��Ĺ���
	DD_CONFIG_ITEM_AUTO_REPORT,				//DD_AUTO_REPORT
	DD_CONFIG_ITEM_NETWORK_END,

	DD_CONFIG_ITEM_ACCOUNT_BASE=0x0700,
	DD_CONFIG_ITEM_ACCOUNT,					//DD_ACCOUNT_CONFIG * MAX_USER_NUM
	DD_CONFIG_ITEM_ACCOUNT_END,

	DD_CONFIG_ITEM_SENSOR_BASE=0x0800,
	DD_CONFIG_ITEM_SENSOR_SETUP,			//DD_SENSOR_CONFIG * SENSOR_INPUT_NUM
	DD_CONFIG_ITEM_SENSOR_SCHEDULE,			//DD_WEEK_SCHEDULE * SENSOR_INPUT_NUM
	DD_CONFIG_ITEM_SENSOR_ALARM_OUT,		//DD_TRIGGER_ALARM_OUT * SENSOR_INPUT_NUM
	DD_CONFIG_ITEM_SENSOR_TO_RECORD,		//DD_TRIGGER_RECORD * SENSOR_INPUT_NUM
	DD_CONFIG_ITEM_SENSOR_TO_PTZ,			//(DD_TRIGGER_PTZ * VIDEO_INPUT_NUM) * SENSOR_INPUT_NUM
	DD_CONFIG_ITEM_SENSOR_END,

	DD_CONFIG_ITEM_MOTION_BASE=0x0900,
	DD_CONFIG_ITEM_MOTION_SETUP,			//DD_MOTION_CONFIG * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_MOTION_SCHEDULE,			//DD_WEEK_SCHEDULE * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_MOTION_ALARM_OUT,		//DD_TRIGGER_ALARM_OUT * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_MOTION_TO_RECORD,		//DD_TRIGGER_RECORD * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_MOTION_TO_PTZ,			//(DD_TRIGGER_PTZ * VIDEO_INPUT_NUM) * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_MOTION_END,

	DD_CONFIG_ITEM_SHELTER_BASE=0x0a00,
	DD_CONFIG_ITEM_SHELTER_SETUP,			//DD_MOTION_CONFIG * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_SHELTER_SCHEDULE,		//DD_WEEK_SCHEDULE * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_SHELTER_ALARM_OUT,		//DD_TRIGGER_ALARM_OUT * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_SHELTER_TO_RECORD,		//DD_TRIGGER_RECORD * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_SHELTER_TO_PTZ,			//(DD_TRIGGER_PTZ * VIDEO_INPUT_NUM) * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_SHELTER_END,

	DD_CONFIG_ITEM_VLOSS_BASE=0x0b00,
	DD_CONFIG_ITEM_VLOSS_SCHEDULE,			//DD_WEEK_SCHEDULE * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_VLOSS_ALARM_OUT,			//DD_TRIGGER_ALARM_OUT * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_VLOSS_TO_RECORD,			//DD_TRIGGER_RECORD * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_VLOSS_TO_PTZ,			//(DD_TRIGGER_PTZ * VIDEO_INPUT_NUM) * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_VLOSS_END,

	DD_CONFIG_ITEM_ALARM_OUT_BASE=0x0c00,
	DD_CONFIG_ITEM_RELAY_SETUP,				//DD_RELAY_CONFIG * RELAY_NUM
	DD_CONFIG_ITEM_RELAY_SCHEDULE,			//DD_WEEK_SCHEDULE * RELAY_NUM
	DD_CONFIG_ITEM_BUZZER_SETUP,			//DD_BUZZER_CONFIG
	DD_CONFIG_ITEM_BUZZER_SCHEDULE,			//DD_WEEK_SCHEDULE
	DD_CONFIG_ITEM_ALARM_OUT_END,

	DD_CONFIG_ITEM_PTZ_BASE=0x0d00,
	DD_CONFIG_ITEM_PTZ_SETUP,				//DD_PTZ_CONFIG * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_PTZ_PRESET,				//DD_PTZ_PRESET_CONFIG * VIDEO_INPUT_NUM
	DD_CONFIG_ITEM_PTZ_PROTOCOL_INFO,       //DD_PTZ_PROTOCOL_INFO
	DD_CONFIG_ITEM_PTZ_END,

	DD_DECODER_CONFIG_ITEM_BASE=0x0e00,
	DD_DECODER_CONFIG_DEVICE_INFO,			//DEC_DEVICE_CONFIG
	DD_DECODER_CONFIG_NETWORK_INFO,			//DEC_NETWORK_CONFIG
	DD_DECODER_CONFIG_NTP_INFO,				//DEC_ADVANCE_NETWORK
	DD_DECODER_CONFIG_DDNS_INFO,			//DEC_DDNS_INFO
	DD_DECODER_CONFIG_SENSOR_SETUP,			//DEC_SENSOR_SETUP* VIDEO_INPUT_NUM
	DD_DECODER_CONFIG_SENSOR_SCHEDULE,		//DEC_WEEK_SCHEDULE * VIDEO_INPUT_NUM
	DD_DECODER_CONFIG_OTHER_ALARMOUT,		//DEC_OTHER_ALARM* VIDEO_INPUT_NUM

	DD_CONFIG_ITEM_END
}DD_CONFIG_ITEM_ID;

///////////////////////////////////////


const unsigned long DEC_MAX_NAME_LEN = 64;
const unsigned long DEC_MAX_VERSION_BUF_LEN = 64;
const unsigned long DEC_MAX_BUF_LEN = (DEC_MAX_NAME_LEN * 2) + 4;

typedef struct _dec_device_config
{
	unsigned long	iSize;		//���ṹ�峤��

	char			deviceName [DEC_MAX_NAME_LEN];			//�豸���ƣ�ע�⿼��˫�ֽ��ַ���
	unsigned long	channelNum;								//������ͨ������
	unsigned long	productID;								//��ƷID��
	unsigned long	productSubID;							//��Ʒ��ID��
	//����Ϊֻ����
	//version
	unsigned long	softVersion;							//����汾��
	char			mcuVersion [DEC_MAX_VERSION_BUF_LEN];			//MCU�汾
	char			kernelVersion [DEC_MAX_VERSION_BUF_LEN];	//�ں˰汾
	char			hardwareVersion [DEC_MAX_VERSION_BUF_LEN];	//Ӳ���汾

}DEC_DEVICE_CONFIG;

typedef struct _dec_network_config
{	
	unsigned long	iSize;		//���ṹ�峤��

	unsigned long	IP;				//�����ַ
	unsigned long	subnetMask;		//��������
	unsigned long	gateway;		//����
	
	unsigned short httpPort;		//HTTP�˿�
	unsigned short decoderPort;			//�������˿�
	char MAC [8];					//���󶨵������ַ
	unsigned long multiCastIP;		//�ಥ��ַ
	unsigned long bDHCP;

	unsigned long dns1;				//DNS1
	unsigned long dns2;				//DNS2
}DEC_NETWORK_CONFIG;

typedef struct _dec_advance_network
{
	unsigned long	iSize;		//���ṹ�峤��

	unsigned char TimeZone; //ʱ��
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
	unsigned char mday;
	unsigned char month;
	unsigned short year;//2008~2025
	
	bool	enableFlag;		//NTPʹ�ܱ�ʶ
	char	name [132];			//NTP����������ַ��
	int		NTP_Port;			//NTP�������Ķ˿�
	int		syncInterval;		//ͬ����ʱ��������СʱΪ��λ
}DEC_ADVANCE_NETWORK;

typedef struct _dec_sensor_setup_
{
	unsigned long iSize;		//���ṹ�峤��

	unsigned char	enable;		//�Ƿ��������
	unsigned char	bNO;		//�豸���ͣ��������߳���
	unsigned short	holdTime;	//�ӳ�ʱ��

	char name [DEC_MAX_BUF_LEN];	//�豸����
	unsigned long  toBuzzer;			//��������������
	unsigned long  toAlarmOut;			//�������������λ��Ӧ����豸��
}DEC_SENSOR_SETUP;

typedef struct _dec_other_alarm_
{
	unsigned long iSize;					//���ṹ�峤��

	unsigned long toBuzzerForIPConflict;	//IP��ͻ����������
	unsigned long toAlarmOutForDisconnect;	//IP��ͻ�������������λ��Ӧ����豸��
	unsigned long toBuzzerForDisconnect;	//����Ͽ�����������
	unsigned long toAlarmOutForIPConflict;	//����Ͽ��������������λ��Ӧ����豸��
}DEC_OTHER_ALARM;

//���ų̽ṹ��24����24Сʱ��unsigned long long��ÿһλ����ÿһ���ӵ�״̬
typedef struct _dec_date_schedule_
{
	unsigned long long hour [24];
}DEC_DATE_SCHEDULE;

//���ų̽ṹ��7��ʾÿ��7����ų�
typedef struct _dec_week_schedule_
{
	DEC_DATE_SCHEDULE week[7];
}DEC_WEEK_SCHEDULE;

#endif //__DVR_DVS_CONFIG_H__
