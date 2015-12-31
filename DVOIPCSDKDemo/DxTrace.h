#pragma once

#ifdef _DEBUG
#define DxTraceMsg	DxTrace
#else
#define DxTraceMsg
#endif

void DxTrace(char *pFormat, ...);