/************************************************************************/
/* ��������SDK															*/
/************************************************************************/

#define MONITOR_CENTER    0x0001        // center rect to monitor
#define MONITOR_CLIP     0x0000        // clip rect to monitor
#define MONITOR_WORKAREA 0x0002        // use monitor work area
#define MONITOR_AREA     0x0000        // use monitor entire area

// ȡ�õ�ǰ�����Ѿ����ӵ���ʾ������
UINT GetMonitorCount();
// �ڶ�����ʾ������£���Ӧ�ó��򴰿��ƶ���ָ����ʾ�����ض�����
// hWnd				Ҫ�ƶ����ڵľ��
// nMonitorIndex	Ӧ�ó��򴰿ڽ����������µ���Ļ������ֵ,0Ϊ��һ����1Ϊ�ڶ������������ƣ�Windowsϵͳ���֧��10��
// ptLeftTop		Ӧ�ó���Ҫ�ƶ���Ŀ����Ļ�����꣬�����ƶ���ɺ󣬸����꼴��Ӧ���򴰿����Ͻ�����
// ע��:��ֻ��һ����Ļ������£��ú��������ƶ�����λ�ã�ͬʱ��Ŀ����Ļ�����ڵ�����£�Ҳ�����ƶ�����
// ����ǰ������Ļ��Ŀ����Ļ��ͬ�����ɲ����ƶ�����
bool MoveWnd2Monitor(HWND hWnd,UINT nMonitorIndex,POINT ptLeftTop);

// �ڶ�����ʾ������£���Ӧ�ó��򴰿��ƶ���ָ����ʾ�����ض�����
// hWnd				Ҫ�ƶ����ڵľ��
// ptLeftTop		Ӧ�ó���Ҫ�ƶ���Ŀ����Ļ�����꣬�����ƶ���ɺ󣬸����꼴��Ӧ���򴰿����Ͻ�����
// ע��:��ֻ��һ����Ļ������£��ú��������ƶ�����λ��
// ����ǰ������Ļ�Ѿ���������Ҳ�����ƶ�����
bool MoveWnd2PrimaryMonitor(HWND hWnd,POINT ptLeftTop);

// ʹ�����е������ļ��ڿգ����ƶ�ָ������
// hWnd				Ҫ�ƶ����ڵľ��
bool MoveWnd2MonitorWithConfig(HWND hWnd);

// ����������ý���
// �ڶ������ý����У�����������Ӧ�����������ڵ���Ļ��λ��
// ע��:��ֻ��һ����Ļ������£��ú��������ƶ�����λ�ã�ͬʱ��Ŀ����Ļ�����ڵ�����£�Ҳ�����ƶ�����
INT_PTR LanchMonitorConfigDlg(HWND hParent);

// �ɴ��ھ����ȡ������Ļ�ı��
// �����ɹ�ʱ������Ļ���,���򷵻�-1
int GetMonitorIndexFromWnd(HWND hWnd);

// ��ȡ����������ʾ���Ĺ�����
// hWnd			���ھ��
// lpAreaRect	����������
// �ɹ�ʱ����True,���򷵻�False;
bool GetMonitorWorkArea(HWND hWnd,LPRECT lpAreaRect);
