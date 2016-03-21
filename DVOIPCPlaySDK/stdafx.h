// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
// #include <vld_def.h>
// #include <vld.h>
#include <assert.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#ifdef Release_D
#undef assert
#define assert	((void)0)
#endif
#include <windows.h>
#define _HI_MULTITHREAD
#ifndef __HisiLicon
#define __HisiLicon
#endif


// TODO: reference additional headers your program requires here
