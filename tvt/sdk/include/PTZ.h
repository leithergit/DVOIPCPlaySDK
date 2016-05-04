#ifndef PTZ_H_H
#define PTZ_H_H

enum PROTOCOL_TYPE
{
	PROTOCOL_NULL = 0,
	PROTOCOL_PELCOP,
	PROTOCOL_PELCOD,
	PROTOCOL_LILIN,
	PROTOCOL_MINKING,
	PROTOCOL_NEON,
	PROTOCOL_STAR,
	PROTOCOL_VIDO,
	PROTOCOL_DSCP,
	PROTOCOL_VISCA,
	PROTOCOL_SAMSUNG,
	PROTOCOL_RM110,
	PROTOCOL_HY,
	PROTOCOL_END
};

typedef enum protocol_support_action
{
	PROTOCOL_SUPPORT_CURISE		= 0x0001,
	PROTOCOL_SUPPORT_TRACK		= 0x0002,
	PROTOCOL_SUPPORT_PRESET		= 0x0004,
	PROTOCOL_SUPPORT_AUTOSCAN	= 0x0008,
}PROTOCOL_SUPPORT_ACTION;

const char PTZ_PROTOCL_NAME [][64] = { "NULL","PELCOP", "PELCOD", "LILIN", "MINKING", "NEON", "STAR", "VIDO", "DSCP", "VISCA", "SAMSUNG", "RM110", "HY"};

inline unsigned long PTZSupportMask( unsigned long ProtocolID)
{
	unsigned long mask = 0;


	switch( ProtocolID )
	{
		//֧��Ѳ����Э��
	case PROTOCOL_PELCOP:
		{
			mask = PROTOCOL_SUPPORT_CURISE | PROTOCOL_SUPPORT_TRACK | PROTOCOL_SUPPORT_PRESET | PROTOCOL_SUPPORT_AUTOSCAN;
			break;
		}
	case PROTOCOL_PELCOD:
		{
			mask = PROTOCOL_SUPPORT_CURISE | PROTOCOL_SUPPORT_TRACK | PROTOCOL_SUPPORT_PRESET | PROTOCOL_SUPPORT_AUTOSCAN;
			break;
		}
	case PROTOCOL_LILIN:
		{
			mask = PROTOCOL_SUPPORT_PRESET;
			break;
		}
	case PROTOCOL_MINKING:
		{
			mask = PROTOCOL_SUPPORT_CURISE | PROTOCOL_SUPPORT_PRESET;
			break;
		}
	case PROTOCOL_NEON:
		{
			mask = PROTOCOL_SUPPORT_CURISE | PROTOCOL_SUPPORT_PRESET;
			break;
		}
	case PROTOCOL_STAR:
		{
			mask = PROTOCOL_SUPPORT_CURISE | PROTOCOL_SUPPORT_PRESET;
			break;
		}
	case PROTOCOL_VIDO:
		{
			mask = PROTOCOL_SUPPORT_CURISE | PROTOCOL_SUPPORT_PRESET;
			break;
		}
	case PROTOCOL_DSCP:
		{
			mask = PROTOCOL_SUPPORT_TRACK | PROTOCOL_SUPPORT_PRESET;
			break;
		}
	case PROTOCOL_VISCA:
		{
			mask = PROTOCOL_SUPPORT_PRESET;
			break;
		}
	case PROTOCOL_SAMSUNG:
		{
			mask = PROTOCOL_SUPPORT_CURISE | PROTOCOL_SUPPORT_TRACK | PROTOCOL_SUPPORT_PRESET | PROTOCOL_SUPPORT_AUTOSCAN;
			break;
		}
	case PROTOCOL_RM110:
		{
			mask = 0;
			break;
		}
	case PROTOCOL_HY:
		{
			mask = 0;
			break;
		}
	default:
		break;
	}

	return mask;
}

inline bool PTZSupportAction( unsigned long ProtocolID, PROTOCOL_SUPPORT_ACTION PTZAction )
{
	bool SupportAction = false;
	
	if( PROTOCOL_SUPPORT_CURISE == PTZAction )
	{
		switch( ProtocolID )
		{
		//֧��Ѳ����Э��
		case PROTOCOL_PELCOP:
		case PROTOCOL_PELCOD:
		case PROTOCOL_MINKING:
		case PROTOCOL_NEON:
		case PROTOCOL_STAR:
		case PROTOCOL_VIDO:
		case PROTOCOL_SAMSUNG:
				SupportAction = true;
				break;
		default:
				break;
		}
	}
	else if( PROTOCOL_SUPPORT_TRACK == PTZAction )
	{
		switch( ProtocolID )
		{
			//֧�ֹ켣��Э��
		case PROTOCOL_PELCOP:
		case PROTOCOL_PELCOD:
		case PROTOCOL_DSCP:
		case PROTOCOL_SAMSUNG:
			SupportAction = true;
			break;
		default:
			break;
		}
	}
	else if( PROTOCOL_SUPPORT_PRESET == PTZAction )
	{
		switch( ProtocolID )
		{
			//��֧��Ԥ�õ��Э��
		case PROTOCOL_NULL:
		case PROTOCOL_RM110:
		case PROTOCOL_HY:
			SupportAction = false;
			break;
		default:
			SupportAction = true;
			break;
		}
	}
	else if( PROTOCOL_SUPPORT_AUTOSCAN == PTZAction )
	{
		switch( ProtocolID )
		{
			//֧���Զ�ɨ���Э��
		case PROTOCOL_PELCOP:
		case PROTOCOL_PELCOD:
		case PROTOCOL_SAMSUNG:
			SupportAction = true;
			break;
		default:
			break;
		}
	}
	else
	{
		//assert(false);
	}

	return SupportAction;
}

typedef enum _ptz_com_type
{
	PTZ_CMD_STOP			=0,	//ֹͣ

	PTZ_CMD_LEFT,				//��
	PTZ_CMD_RIGHT,				//��
	PTZ_CMD_UP,					//��
	PTZ_CMD_DOWN,				//��

	PTZ_CMD_LEFT_UP,			//����
	PTZ_CMD_LEFT_DOWN,			//����
	PTZ_CMD_RIGHT_UP,			//����
	PTZ_CMD_RIGHT_DOWN,			//����

	PTZ_CMD_NEAR,				//����
	PTZ_CMD_FAR,				//��Զ

	PTZ_CMD_ZOOM_OUT,			//��С
	PTZ_CMD_ZOOM_IN,			//�Ŵ�

	PTZ_CMD_IRIS_OPEN,			//������Ȧ
	PTZ_CMD_IRIS_CLOSE,			//�رչ�Ȧ

	PTZ_CMD_PRESET_SET,			//����Ԥ�õ�
	PTZ_CMD_PRESET_GO,			//���ڼ���Ԥ�õ�
	PTZ_CMD_PRESET_DEL,			//ɾ��Ԥ�õ�

	PTZ_CMD_CRUISE_CFG,			//����Ѳ����,�൱��ִ��Enter��Set��Leave��������
	PTZ_CMD_ENTER_CURISE_MODE,	//����Ѳ��ģʽ,�˺��������Ѳ����Ԥ�õ�
	PTZ_CMD_CRUISE_SET,			//����Ѳ���ߵ�Ԥ�õ�
	PTZ_CMD_LEAVE_CURISE_MODE,	//�˳�����Ѳ��
	PTZ_CMD_CRUISE_RUN,			//ѡ��һ��Ѳ���߽���Ѳ��
	PTZ_CMD_CRUISE_STOP,		//ֹͣѲ��
	PTZ_CMD_CRUISE_DEL,			//ɾ��Ѳ����

	PTZ_CMD_TRACK_START,		//��ʼ�켣
	PTZ_CMD_TRACK_STOP,			//ֹͣ�켣
	PTZ_CMD_TRACK_START_RECORD,	//��ʼ����켣
	PTZ_CMD_TRACK_STOP_RECORD,	//ֹͣ����켣

	PTZ_CMD_AUTO_SCAN_START,	//��ʼ�Զ�ɨ��
	PTZ_CMD_AUTO_SCAN_STOP,		//ֹͣ�Զ�ɨ��

	PTZ_CMD_RESET = 0xF0,		//��λ��̨״̬
}PTZ_CMD_TYPE;

//��������ֵ����
enum PORT_BAUDRATE
{
	SBR_110 = 0,
	SBR_300,
	SBR_600,
	SBR_1200,
	SBR_2400,
	SBR_4800,
	SBR_9600,
	SBR_19200,
	SBR_38400,
	SBR_57600,
	SBR_115200,
	SBR_230400,
	SBR_460800,
	SBR_921600
};

enum PARITYTYPE
{
	PARITYEVEN = 'E',		//żУ��
	PARITYODD = 'O',		//��У��
	PARITYMARK = 'M',		//
	PARITYSPACE = 'S',		//
	PARITYNONE = 'N'		//��У��
};

enum DATABITS
{
	DATABITS7 = 7,
	DATABITS8
};

typedef enum _ptz_speed
{
	PTZ_SPEED_1 = 1,
	PTZ_SPEED_2	= 2,
	PTZ_SPEED_3	= 3,
	PTZ_SPEED_4	= 4,
	PTZ_SPEED_5	= 5,
	PTZ_SPEED_6	= 6,
	PTZ_SPEED_7	= 7,
	PTZ_SPEED_8	= 8
}PTZ_SPEED;

enum STOPBITS
{
	STOPBITS1 = 2,
	STOPBITSONEHALF,
	STOPBITS2
};

inline unsigned long GetBaudRate(PORT_BAUDRATE rate)
{
	unsigned long ret = 0011;

	switch(rate)
	{
	case SBR_110:
		ret = 003;
		break;
	case SBR_300:
		ret = 007;
		break;
	case SBR_600:
		ret = 0010;
		break;
	case SBR_1200:
		ret = 0011;
		break;
	case SBR_2400:
		ret = 0013;
		break;
	case SBR_4800:
		ret = 0014;
		break;
	case SBR_9600:
		ret = 0015;
		break;
	case SBR_19200:
		ret = 0016;
		break;
	case SBR_38400:
		ret = 0017;
		break;
	case SBR_57600:
		ret = 0010001;
		break;
	case SBR_115200:
		ret = 0010002;
		break;
	case SBR_230400:
		ret = 0010003;
		break;
	case SBR_460800:
		ret = 0010004;
		break;
	case SBR_921600:
		ret = 0010007;
		break;
	default :
		ret = 0011;
		break;
	}

	return ret;
}

inline void  GetBaudRate(PORT_BAUDRATE rate, unsigned long *pBandRate )
{

	switch(rate)
	{
	case SBR_110:
		*pBandRate = 110;
		break;
	case SBR_300:
		*pBandRate = 300;
		break;
	case SBR_600:
		*pBandRate = 600; 
		break;
	case SBR_1200:
		*pBandRate = 1200;
		break;
	case SBR_2400:
		*pBandRate = 2400;
		break;
	case SBR_4800:
		*pBandRate = 4800;
		break;
	case SBR_9600:
		*pBandRate = 9600;
		break;
	case SBR_19200:
		*pBandRate = 19200;
		break;
	case SBR_38400:
		*pBandRate = 38400;
		break;
	case SBR_57600:
		*pBandRate = 57600;
		break;
	case SBR_115200:
		*pBandRate = 115200;
		break;
	case SBR_230400:
		*pBandRate = 230400;
		break;
	case SBR_460800:
		*pBandRate = 460800;
		break;
	case SBR_921600:
		*pBandRate = 921600;
		break;
	default :
		*pBandRate = 110;
		break;
	}

}

//////////////////////////////////////////////////////////////////////////

enum PTZ_STATE
{
	PTZ_ST_NULL = 0x0000,
	PTZ_ST_NORMAL = 0x0001, //��ͨ״̬,[Pan/Zoom]��
	PTZ_ST_TRACK = 0x0002, //�켣
	PTZ_ST_AUTOSCAN = 0x0004, //�Զ�ɨ��
	PTZ_ST_CRUISE = 0x0008, //Ѳ��

	PTZ_ST_LOCK = 0x0100, //����������;���ô˱�ǽ���ֹ��������
	PTZ_ST_CFG_PRESET = 0x0200, //����Ԥ�õ�[Ŀǰû��ʹ��]
	PTZ_ST_CFG_TRACK = 0x0400, // ���ù켣
	PTZ_ST_CFG_CRUISE = 0x0800, //����Ѳ����
};

enum PTZ_ERR
{
	PTZ_ERR_OK = 0,
	PTZ_ERR_WRITE_COM,			//д����ʧ��
	PTZ_ERR_UF_PROTOCOL,		//û���ҵ�Э�鼯

	PTZ_ERR_CHNN_LOCK,			//ͨ��������
	PTZ_ERR_CHNN_UN_INTERRUPT,	//���ܴ��ͨ������
	PTZ_ERR_CHNN_UN_SET,		//û���ҵ����ͨ������
	PTZ_ERR_CHNN_UN_ENABLE,		//δ����ͨ��
	PTZ_ERR_CHNN_WAITING,		//ͨ�����ڵȴ���
	PTZ_ERR_PRESET_DISABLE,		//Ԥ�õ�δ����
	PTZ_ERR_OPERATE_FAIL,		//������̨ʧ��,
};


#endif

