/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


//#ifdef __PRACTISE_THINK_DEBUG_VIEW__
//#define __PRACTISE_THINK_DEBUG_VIEW__

#pragma once


#ifdef _MSC_VER
#ifdef _DEBUG
#define _PT_DEBUG
#endif
#elif 
#endif
//#define _PT_DEBUG // for relase 

#ifdef _PT_DEBUG

#include <stdio.h>
#include <stdarg.h>

//#pragma comment(lib,"ptDebugCmdDll.lib")

#ifdef __cplusplus
extern "C"
{
#endif
	void __stdcall ptDebug_Printf(const char*) ;
	void __stdcall ptDebug_SetNetInfo(const char* szIp,unsigned short wPort) ;
	void __stdcall ptDebug_SetInputCB(void (*lpfnDebug_Input)(unsigned long dwUser,unsigned char* argc[],int nCount,int nType),unsigned long dwUser) ;

#ifdef __cplusplus
}
#endif
#endif


////////////////////////////////////////////////////////////////////////////////////////////

inline void Debug_PrintfA(const char* pcFormat,...)
{
#ifdef _PT_DEBUG
	char szBuf[1024+1] = {0} ;
	va_list pArg;
    va_start(pArg, pcFormat);
    vsprintf(szBuf, pcFormat, pArg);
    va_end(pArg);
	//ptDebug_Printf(szBuf) ;
#endif
}

inline void Debug_PrintfW(const WCHAR* pcFormat,...)
{
#ifdef _PT_DEBUG
	WCHAR szBuf[16*1024+2] = {0} ;
	va_list pArg;
    va_start(pArg, pcFormat);
    vswprintf(szBuf, pcFormat, pArg);
    va_end(pArg);

	int nNeedSize = ::wcslen(szBuf)*sizeof(WCHAR) ; 

	if(nNeedSize>0 && nNeedSize<16*1024+1)
	{
		char szBufA[16*1024+1] = {0} ;

		::WideCharToMultiByte(CP_ACP,0,szBuf,nNeedSize,szBufA,sizeof(szBufA),NULL,NULL) ;

		//ptDebug_Printf(szBufA) ;
		OutputDebugStringA(szBufA);
		OutputDebugStringA("\r\n");
	}
	else
	{
		//ptDebug_Printf("") ;
		OutputDebugStringA("\r\n");
	}
#endif
}

#define Release_Printf Debug_PrintfW
#define Debug_Printf Debug_PrintfW

inline void Debug_SetNetInfo(const char* szIp,unsigned short wPort)
{
#ifdef _PT_DEBUG
	//ptDebug_SetNetInfo(szIp,wPort) ;
#endif
}

//#endif
