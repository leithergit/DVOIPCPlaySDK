#include <windows.h>
#include <stdio.h>
#include <wtypes.h>
#include "DxTrace.h"
#define __countof(array) (sizeof(array)/sizeof(array[0]))
#pragma warning (disable:4996)

void DxTrace(char *pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);
	int nBuff;
	CHAR szBuffer[0x7fff];
	nBuff = _vsnprintf(szBuffer, __countof(szBuffer), pFormat, args);
	//::wvsprintf(szBuffer, pFormat, args);
	//assert(nBuff >=0);
	OutputDebugStringA(szBuffer);
	va_end(args);	
}