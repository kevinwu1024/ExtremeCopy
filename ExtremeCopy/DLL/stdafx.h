// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <crtdbg.h>

#include "../Common/ptGlobal.h"
#include <assert.h>

#include "../App/CompileMacro.h"

#include <ptDebugView.h>

#ifdef COMPILE_TEST_PERFORMANCE
#include "..\Common\CptPerformanceCalcator.h"
#endif
