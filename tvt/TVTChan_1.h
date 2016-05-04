#pragma once
#include "./sdk/include/dvrdvstypedef.h"
#include "./sdk/include/DVR_NET_SDK.h"
#include "../dvoipcplaysdk/dvoipcplaysdk.h"

#include <string>
using namespace  std;
class CTVTChan_1
{
public:
    CTVTChan_1(void);
    ~CTVTChan_1(void);

   BOOL Play(string ip,int nPort,HWND hwnd);
   BOOL StartPlayer();

   void DealLiveCallBack(NET_SDK_FRAME_INFO frameInfo, BYTE *pBuffer);
   static void CALLBACK LiveCallBack(LONG lLiveHandle, NET_SDK_FRAME_INFO frameInfo, BYTE *pBuffer, void *pUser);

   
   LONG m_hUserHandle;
   LONG m_lLiveHandle;
   void* m_hPlayHandle;   ///≤•∑≈ ”∆µ

   NET_SDK_DEVICEINFO			m_SystemParam;
   HWND m_hWnd;

};
