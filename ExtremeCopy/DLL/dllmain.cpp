/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "stdafx.h"
#include "ExtremeCopyApp.h"
#include "ExportExtremeCopyAPI.h"

CExtremeCopyApp* g_pExtremeCopyApp = NULL ;
lpfnCopyRoutineA_t g_lpfnCopyRoutineA = NULL ;
lpfnCopyRoutineW_t g_lpfnCopyRoutineW = NULL ;
void*				g_pRoutineParam = NULL ;

int __stdcall CopyRoutine(int nCmd,int nParam1,int nParam2,const TCHAR* pSrcStr,const TCHAR* pDstStr) ;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{ 
			::DisableThreadLibraryCalls(hModule);
			g_pExtremeCopyApp = new CExtremeCopyApp() ;
		} 
		break ;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break ;
	case DLL_PROCESS_DETACH:
		{
			if(g_pExtremeCopyApp!=NULL)
			{
				delete g_pExtremeCopyApp ;
				g_pExtremeCopyApp = NULL ;
			}

			CptStringBase::ReleaseAllRes() ;
		}
		break;
	}
	return TRUE;
}

int __stdcall ExtremeCopy_SetBufferSize(int nBufSize) 
{
	int nRet = -1 ;

	if(g_pExtremeCopyApp!=NULL)
	{
		nRet = g_pExtremeCopyApp->SetCopyBufferSize(nBufSize) ;
	}

	return nRet ;
}

void __stdcall ExtremeCopy_SetLicenseKeyA(const char* lpstrSN) 
{
	if(g_pExtremeCopyApp!=NULL && lpstrSN!=NULL)
	{
		g_pExtremeCopyApp->SetLicenseKey(lpstrSN) ;
	}
}

void __stdcall ExtremeCopy_SetLicenseKeyW(const WCHAR* lpstrSN) 
{
	if(g_pExtremeCopyApp!=NULL && lpstrSN!=NULL)
	{
		g_pExtremeCopyApp->SetLicenseKey(lpstrSN) ;
	}
}

void __stdcall ExtremeCopy_Stop() 
{
	if(g_pExtremeCopyApp!=NULL)
	{
		g_pExtremeCopyApp->Stop() ;
	}
}

bool __stdcall ExtremeCopy_Pause() 
{
	bool bRet = false ;

	if(g_pExtremeCopyApp!=NULL)
	{
		bRet = g_pExtremeCopyApp->Pause() ;
	}

	return bRet ;
}

bool __stdcall ExtremeCopy_Continue() 
{
	bool bRet = false ;

	if(g_pExtremeCopyApp!=NULL)
	{
		bRet = g_pExtremeCopyApp->Continue() ;
	}

	return bRet ;
}

int __stdcall ExtremeCopy_StartW(int nRunType,bool bSyncOrAsyn,lpfnCopyRoutineW_t lpfnCopyRoutineW,void* pRoutineParam) 
{
	int bRet = 0 ;

	if(g_pExtremeCopyApp!=NULL && (XCRunType_Copy==nRunType || nRunType==XCRunType_Move))
	{
		g_lpfnCopyRoutineW = lpfnCopyRoutineW ;
		g_pRoutineParam = pRoutineParam ;

		g_pExtremeCopyApp->SetRoutine(CopyRoutine) ;

		bRet = g_pExtremeCopyApp->Start(nRunType,bSyncOrAsyn) ;
	}

	return bRet ;
}

int __stdcall ExtremeCopy_StartA(int nRunType,bool bSyncOrAsyn,lpfnCopyRoutineA_t lpfnCopyRoutineA,void* pRoutineParam) 
{
	int bRet = 0 ;

	if(g_pExtremeCopyApp!=NULL && (XCRunType_Copy==nRunType || nRunType==XCRunType_Move))
	{
		g_lpfnCopyRoutineA = lpfnCopyRoutineA ;
		g_pRoutineParam = pRoutineParam ;

		g_pExtremeCopyApp->SetRoutine(CopyRoutine) ;

		bRet = g_pExtremeCopyApp->Start(nRunType,bSyncOrAsyn) ;
	}

	return bRet ;
}

int __stdcall ExtremeCopy_GetState() 
{
	int bRet = -1 ;

	if(g_pExtremeCopyApp!=NULL)
	{
		bRet = (int)g_pExtremeCopyApp->GetState() ;
	}

	return bRet ;
}

int __stdcall ExtremeCopy_AttachSrcA(const char* lpstr) 
{
	if(lpstr==NULL || g_pExtremeCopyApp==NULL)
	{
		return -1 ;
	}

	CptString str = lpstr ;
	return g_pExtremeCopyApp->AttachSrc(str) ;

#ifdef _UNICODE
#else
#endif
}

int __stdcall ExtremeCopy_AttachSrcW(const WCHAR* lpstr) 
{
	if(lpstr==NULL || g_pExtremeCopyApp==NULL)
	{
		return -1 ;
	}

	CptString str = lpstr ;

	return g_pExtremeCopyApp->AttachSrc(str) ;

#ifdef _UNICODE
#else
#endif
}

bool __stdcall ExtremeCopy_SetDestinationFolderA(const char* lpstr) 
{
	if(lpstr==NULL || g_pExtremeCopyApp==NULL)
	{
		return false ;
	}

	CptString str = lpstr ;

	return g_pExtremeCopyApp->SetDestinationFolder(str) ;

#ifdef _UNICODE
#else
#endif
}

bool __stdcall ExtremeCopy_SetDestinationFolderW(const WCHAR* lpstr) 
{
	if(lpstr==NULL || g_pExtremeCopyApp==NULL)
	{
		return false ;
	}

	CptString str = lpstr ;

	return g_pExtremeCopyApp->SetDestinationFolder(str) ;

#ifdef _UNICODE
#else
#endif
}

//void __stdcall ExtremeCopy_SetCopyRoutineA(lpfnCopyRoutineA_t lpfnCopyRoutineA) 
//{
//	g_lpfnCopyRoutineA = lpfnCopyRoutineA ;
//
//	if(g_pExtremeCopyApp!=NULL)
//	{
//		g_pExtremeCopyApp->SetRoutine(CopyRoutine) ;
//	}
//
//#ifdef _UNICODE
//#else
//#endif
//}

//void __stdcall ExtremeCopy_SetCopyRoutineW(lpfnCopyRoutineW_t lpfnCopyRoutineW) 
//{
//	g_lpfnCopyRoutineW = lpfnCopyRoutineW ;
//
//	if(g_pExtremeCopyApp!=NULL)
//	{
//		g_pExtremeCopyApp->SetRoutine(CopyRoutine) ;
//	}
//
//#ifdef _UNICODE
//#else
//#endif
//}


int __stdcall CopyRoutine(int nCmd,int nParam1,int nParam2,const TCHAR* pSrcStr,const TCHAR* pDstStr)
{
	if(g_lpfnCopyRoutineW!=NULL)
	{
#ifdef _UNICODE
		CptStringW strSrc = pSrcStr==NULL ? L"" : pSrcStr ;
		CptStringW strDst = pDstStr==NULL ? L"" : pDstStr ;
#else
		CptStringW strSrc = pSrcStr==NULL ? "" : pSrcStr ;
		CptStringW strDst = pDstStr==NULL ? "" : pDstStr ;
#endif
		return g_lpfnCopyRoutineW(nCmd,g_pRoutineParam,nParam1,nParam2,pSrcStr==NULL?NULL:strSrc.c_str(),pDstStr==NULL?NULL:strDst.c_str()) ;
	}

	if(g_lpfnCopyRoutineA!=NULL)
	{
#ifdef _UNICODE
		CptStringA strSrc = pSrcStr==NULL ? L"" : pSrcStr ;
		CptStringA strDst = pDstStr==NULL ? L"" : pDstStr ;
#else
		CptStringA strSrc = pSrcStr==NULL ? "" : pSrcStr ;
		CptStringA strDst = pDstStr==NULL ? "" : pDstStr ;
#endif
		

		return g_lpfnCopyRoutineA(nCmd,g_pRoutineParam,nParam1,nParam2,pSrcStr==NULL?NULL:strSrc.c_str(),pDstStr==NULL?NULL:strDst.c_str()) ;
	}

	return 0 ;
}