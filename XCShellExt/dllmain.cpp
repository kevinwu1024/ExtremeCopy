/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <initguid.h>
#include <windows.h>
#include "ShellExtClassFactory.h"
#include "XCShellExt.h"

#include "../ExtremeCopy/Common/ptGlobal.h"
#include "../ExtremeCopy/App/XCConfiguration.h"

CSystemObject* g_pSysObj = NULL ; 

static const TCHAR* s_ShellContextMenuRegKeys[] = 
{
	_T("*\\shellex\\ContextMenuHandlers"),
	_T("Folder\\shellex\\ContextMenuHandlers"),
	_T("Directory\\shellex\\ContextMenuHandlers"),
	_T("Directory\\Background\\shellex\\ContextMenuHandlers"),
	_T("Drive\\shellex\\ContextMenuHandlers"),
	_T("InternetShortcut\\shellex\\ContextMenuHandlers"),
	_T("lnkfile\\shellex\\ContextMenuHandlers"),

};

static const TCHAR* s_ShellDragDropRegKeys[] = {
	_T("*\\shellex\\DragDropHandlers"),
	_T("Folder\\shellex\\DragDropHandlers"),
	_T("Directory\\shellex\\DragDropHandlers"), 
	_T("Drive\\shellex\\DragDropHandlers"),
	 
}; 

static const TCHAR* s_pShellRegKey = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved") ;
extern CSystemObject* g_pSysObj ;

static CShellContextMenuFactory*	s_pContextMenuFactory = NULL ;
static CShellDragDropHandlerFactory*	s_pDragDropFactory = NULL ;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			g_pSysObj = new CSystemObject(hModule) ;

			g_pSysObj->Initailize();
		}
		Debug_Printf("XCShell DLL_PROCESS_ATTACH") ;
		break ;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break ;
	case DLL_PROCESS_DETACH:
		{
			if(g_pSysObj!=NULL)
			{
				delete g_pSysObj ;
				g_pSysObj = NULL ;
			}

			CXCConfiguration::Release() ;
		}
		Debug_Printf("XCShell DLL_PROCESS_DETACH") ;
		break;
	}
	return TRUE;
}



void UnregisterShellExt(REFIID riid,const TCHAR* ShellExtRegArray[],int nArrayCount,const TCHAR* pAppName)
{
	TCHAR szCLSIDBuf[256] = {0} ;

	if(::GUID2CLSIDString(riid,szCLSIDBuf,sizeof(szCLSIDBuf)))
	{
		CptRegistry reg ;

		if(reg.OpenKey(CptRegistry::CLASSES_ROOT,NULL))
		{
			CptRegistry regTem ;
			for (int i=0; i<nArrayCount; i++) 
			{
				//if (reg.CreateKey(s_DllRegKeys[i]))
				if(regTem.OpenKey(CptRegistry::CLASSES_ROOT,ShellExtRegArray[i]))
				{
					regTem.DeleteTree(pAppName) ;
				}
			}

			reg.Close() ;
		}

		if(reg.OpenKey(CptRegistry::LOCAL_MACHINE,s_pShellRegKey))
		{
			reg.DeleteValue(szCLSIDBuf) ;
			reg.Close() ;
		}

		if(reg.OpenKey(CptRegistry::CLASSES_ROOT,_T("CLSID")))
		{// 反注册标准COM
			reg.DeleteTree(szCLSIDBuf) ;
			reg.Close() ;
		}
	}
}


STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut) 
{
	*ppvOut = NULL ;

	TCHAR szCLIDBuf[260] = {0} ;
	GUID2CLSIDString(rclsid,szCLIDBuf,sizeof(szCLIDBuf)) ;

	if(::IsEqualGUID(CLSID_XCContextMenu,rclsid))
	{// 'context menu' 的 shell extension
		 CShellContextMenuFactory* pFactory = new CShellContextMenuFactory(g_pSysObj) ;

		 if(pFactory==NULL)
		 {
			 return E_OUTOFMEMORY ;
		 }

		 return pFactory->QueryInterface(riid,ppvOut) ;
	}
	else if(::IsEqualGUID(CLSID_XCDragDropHandler,rclsid))
	{// 拖放 shell extension

		CShellDragDropHandlerFactory* pFactory = new CShellDragDropHandlerFactory(g_pSysObj) ;

		 if(pFactory==NULL)
		 {
			 return E_OUTOFMEMORY ;
		 }

		 return pFactory->QueryInterface(riid,ppvOut) ;
	}

	return CLASS_E_CLASSNOTAVAILABLE ;
}

STDAPI DllCanUnloadNow() 
{
	Debug_Printf("DllCanUnloadNow()") ;

	return (g_pSysObj==NULL || g_pSysObj->GetRefCount()<=0) ? S_OK : S_FALSE ;
}

bool RegisterShellExt(REFIID riid,const TCHAR* ShellExtRegArray[],int nArrayCount,const TCHAR* pDllFileName,const TCHAR* pAppName)
{
	CptRegistry reg ;

	TCHAR szCLSIDBuf[256] = {0} ;

	Debug_Printf("RegisterShellExt() 1") ;

	if(::GUID2CLSIDString(riid,szCLSIDBuf,sizeof(szCLSIDBuf)))
	{
		CptRegistry regTem1 ;
		CptRegistry regTem2 ;
		
		bool bRet = false ;

		Debug_Printf("RegisterShellExt() 2") ;

		if(reg.OpenKey(CptRegistry::CLASSES_ROOT,_T("CLSID")))
		{// 注册标准COM
			if(reg.CreateKey(szCLSIDBuf,&regTem1))
			{
				regTem1.SetValueString(NULL,pAppName) ;

				if(regTem1.CreateKey(_T("InProcServer32"),&regTem2))
				{
					regTem2.SetValueString(NULL, pDllFileName);
					regTem2.SetValueString(_T("ThreadingModel"), _T("Apartment"));

					Debug_Printf("RegisterShellExt() 2") ;

					bRet = true ;
				}

				reg.Close() ;
			}
		}

		Debug_Printf("RegisterShellExt() 3") ;

		if(bRet && reg.OpenKey(CptRegistry::CLASSES_ROOT,NULL))
		{// 注册SHELL

			Debug_Printf("RegisterShellExt() 4") ;

			for (int i=0; i<nArrayCount; i++) 
			{
				if (reg.CreateKey(ShellExtRegArray[i],&regTem1))
				{
					if (regTem1.CreateKey(pAppName,&regTem2)) 
					{
						//Debug_Printf(ShellExtRegArray[i]) ;
						regTem2.SetValueString(NULL,szCLSIDBuf) ;
					}
				}
			}

			reg.Close() ;
		}

		Debug_Printf("RegisterShellExt() 5") ;

		if(bRet && reg.OpenKey(CptRegistry::LOCAL_MACHINE,s_pShellRegKey))
		{
			reg.SetValueString(szCLSIDBuf,pAppName) ;
			reg.Close() ;
		}

		return bRet ;
	}

	Debug_Printf("RegisterShellExt() end") ;

	return false ;
}


STDAPI DllUnregisterServer(void) 
{
#ifdef _M_AMD64
// 当前是在 64位 PE文件
	if(!CptGlobal::IsWin64())
	{// 若要注册在32位系统,则不做任何工作
		return S_OK ;
	}
#else
// 当前是在32位 PE文件
	if(CptGlobal::IsWin64())
	{// 若要注册在64位系统,则不做任何工作
		return S_OK ;
	}
#endif

	TCHAR szAppNameBuf[256] = {0} ;
	TCHAR szCLSIDBuf[256] = {0} ;

	//if(::GUID2CLSIDString(CLSID_XCContextMenu,szCLSIDBuf,sizeof(szCLSIDBuf)) &&
		if(::LoadString(::CptMultipleLanguage::GetInstance()->GetResourceHandle(),IDS_TITLE_APP,szAppNameBuf,sizeof(szAppNameBuf)) )
	{
		// 反注册 context menu
		::UnregisterShellExt(CLSID_XCContextMenu,s_ShellContextMenuRegKeys,sizeof(s_ShellContextMenuRegKeys)/sizeof(TCHAR*),szAppNameBuf) ;

		// 反注册 drag drop
		::UnregisterShellExt(CLSID_XCDragDropHandler,s_ShellDragDropRegKeys,sizeof(s_ShellDragDropRegKeys)/sizeof(TCHAR*),szAppNameBuf) ;
		
		return S_OK ;
	}

	return E_FAIL ;
}

STDAPI DllRegisterServer(void) 
{
	//MessageBox(NULL,_T("DllRegisterServer() 1"),_T(""),0) ;

#ifdef _M_AMD64
//	MessageBox(NULL,_T("当前是在64位 PE文件"),_T(""),0) ;
// 当前是在 64位 PE文件
	if(!CptGlobal::IsWin64())
	{// 若要注册在32位系统,则不做任何工作
//		MessageBox(NULL,_T("若要注册在32位系统,则不做任何工作"),_T(""),0) ;
		return S_OK ;
	}
#else
//	MessageBox(NULL,_T("当前是在32位 PE文件"),_T(""),0) ;
// 当前是在32位 PE文件
	if(CptGlobal::IsWin64())
	{// 若要注册在64位系统,则不做任何工作
//		MessageBox(NULL,_T("若要注册在64位系统,则不做任何工作"),_T(""),0) ;
		return S_OK ;
	}
#endif

	CptRegistry reg ;

	TCHAR szAppNameBuf[256] = {0} ;
	TCHAR szDllName[MAX_PATH] = {0} ;

	BOOL bLoadString = ::LoadString(::CptMultipleLanguage::GetInstance()->GetResourceHandle(),IDS_TITLE_APP,szAppNameBuf,sizeof(szAppNameBuf)) ;

	if(  bLoadString &&
		g_pSysObj->GetDllFullName(szDllName,sizeof(szDllName)))
	{
		// 注册 drag drop
		bool bResutl1 = ::RegisterShellExt(CLSID_XCDragDropHandler,s_ShellDragDropRegKeys,sizeof(s_ShellDragDropRegKeys)/sizeof(TCHAR*),szDllName,szAppNameBuf) ;

		// 注册 context menu
		bool bResutl2 = ::RegisterShellExt(CLSID_XCContextMenu,s_ShellContextMenuRegKeys,sizeof(s_ShellContextMenuRegKeys)/sizeof(TCHAR*),szDllName,szAppNameBuf) ;

		return bResutl1&&bResutl2 ? S_OK : E_FAIL ;
	}

	return E_FAIL ;
}

