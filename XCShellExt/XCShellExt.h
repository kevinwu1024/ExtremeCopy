/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#include <windows.h>
#include <vector>
#include "unknwn.h"
#include "../ExtremeCopy/Common/ptString.h"
#include "XCDragDropHandler.h"
#include "../ExtremeCopy/Common/ptTypeDef.h"
//bool GUID2CLSIDString(const GUID& guid,TCHAR* pString,int nSize) ;
/**
extern "C"
{
	HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut) ;
	HRESULT __stdcall DllCanUnloadNow() ;
	HRESULT __stdcall DllRegisterServer(void) ;
	HRESULT __stdcall DllUnregisterServer(void) ;
}
/**/

class CSystemObject ;

#define IsFileExist(lpFileFuleName) (::GetFileAttributes(lpFileFuleName)!=INVALID_FILE_ATTRIBUTES) 

bool IsExtremeCopyExeExist() ;
bool IsReplaceExplorerCopying() ;
bool GUID2CLSIDString(const GUID& guid,TCHAR* pString,int nSize) ;
bool IsValidCOM(const GUID& guid) ;
bool IsCut(IDataObject* pDataObj) ;
EOperateType GetOperateType(IDataObject* pDataObj) ;
bool LaunchExtremeCopy( CSystemObject* pSysObj,const pt_STL_vector(CptString)& strSrcFileVer,const CptString& strDstFolder,
					   bool bMoveOrCopy,HWND hWnd) ;
bool LaunchTaskEditor( CSystemObject* pSysObj,const pt_STL_vector(CptString)& strSrcFileVer) ;
bool CreateShortcut(CSystemObject* pSysObj,const pt_STL_vector(CptString)& strSrcFileVer,const CptString& strDstFolder,HWND hWnd) ;

bool ExecuteTaskFile(CSystemObject* pSysObj,CptString strTaskFile) ;

CptString GetResourceDLL(HMODULE hModule) ;
bool IsVISTAOrLaterVersion() ;
bool IsEnableUAC() ;
bool IsNeedElevatePrivilege(const pt_STL_vector(CptString)& strSrcFileVer,const CptString& strDstFolder,bool bMoveOrCopy);
//bool LaunchExtremeCopy(const std::vector<CptString>& strSrcFileVer,const CptString& strDstFolder ) ;