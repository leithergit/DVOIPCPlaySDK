/************************************************************************/
/* 多屏调度SDK															*/
/************************************************************************/

#define MONITOR_CENTER    0x0001        // center rect to monitor
#define MONITOR_CLIP     0x0000        // clip rect to monitor
#define MONITOR_WORKAREA 0x0002        // use monitor work area
#define MONITOR_AREA     0x0000        // use monitor entire area

// 取得当前主机已经连接的显示器数量
UINT GetMonitorCount();
// 在多屏显示的情况下，把应用程序窗口移动到指定显示器的特定坐标
// hWnd				要移动窗口的句柄
// nMonitorIndex	应用程序窗口将被动到的新的屏幕的索引值,0为第一屏，1为第二屏，依此类推，Windows系统最多支持10屏
// ptLeftTop		应用程序要移动到目标屏幕的坐标，窗口移动完成后，该坐标即对应程序窗口左上角坐标
// 注意:在只有一个屏幕的情况下，该函数不会移动窗口位置，同时在目标屏幕不存在的情况下，也不会移动窗口
// 若当前所在屏幕与目标屏幕相同，依旧不会移动窗口
bool MoveWnd2Monitor(HWND hWnd,UINT nMonitorIndex,POINT ptLeftTop);

// 在多屏显示的情况下，把应用程序窗口移动到指定显示器的特定坐标
// hWnd				要移动窗口的句柄
// ptLeftTop		应用程序要移动到目标屏幕的坐标，窗口移动完成后，该坐标即对应程序窗口左上角坐标
// 注意:在只有一个屏幕的情况下，该函数不会移动窗口位置
// 若当前所在屏幕已经是主屏，也不会移动窗口
bool MoveWnd2PrimaryMonitor(HWND hWnd,POINT ptLeftTop);

// 使用现有的配置文件内空，来移动指定窗口
// hWnd				要移动窗口的句柄
bool MoveWnd2MonitorWithConfig(HWND hWnd);

// 进入多屏配置界面
// 在多屏配置界面中，您可以配置应程序启动所在的屏幕和位置
// 注意:在只有一个屏幕的情况下，该函数不会移动窗口位置，同时在目标屏幕不存在的情况下，也不会移动窗口
INT_PTR LanchMonitorConfigDlg(HWND hParent);

// 由窗口句柄获取所以屏幕的编号
// 操作成功时返回屏幕编号,否则返回-1
int GetMonitorIndexFromWnd(HWND hWnd);

// 获取窗口所以显示器的工作区
// hWnd			窗口句柄
// lpAreaRect	工作区矩形
// 成功时返回True,否则返回False;
bool GetMonitorWorkArea(HWND hWnd,LPRECT lpAreaRect);
