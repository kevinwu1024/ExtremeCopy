/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

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
#include <tchar.h>

// shell extension 应是全功能的
#define VERSION_PROFESSIONAL

#include "../ExtremeCopy/Common/ptRegistry.h"
#include "../ExtremeCopy/Common/ptString.h"

#include "resource.h"

#include "ptDebugView.h"

#define MAX_PATH_EX		(MAX_PATH * 8)

#define SAFE_DELETE(p)	if((p)!=NULL) {delete (p);(p)=NULL;}

#define SAFE_RELEASE(p) if((p)!=NULL) {(p)->Release();(p)=NULL;}




// TODO: reference additional headers your program requires here
