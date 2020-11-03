/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "stdafx.h"
#include "XCUpdate.h"
#include "../ExtremeCopy/Common/ptTypeDef.h"
#include "../ExtremeCopy/App/XCGlobal.h"
#include "../ExtremeCopy/App/ptMultipleLanguage.h"
#include "../ExtremeCopy/App/XCConfiguration.h"
#include "XCNewsChecker.h"

// 加载资源
bool LoadXCResource(SConfigData* pConfigData) 
{
	_ASSERT(pConfigData) ;

	// 查找资源 DLL
	CptString strStartUpPath = CptWinPath::GetStartupPath() ;

	CptString strLanguagePath = strStartUpPath+ _T("\\Language") ;

	/**
	// 这是锁定 polish 语言版的
	CptString strDll = strLanguagePath + _T("\\XCRes_POL.dll") ;

	bool bLoad = false ;
	if(IsFileExist(strDll.c_str()))
	{
	HMODULE hInst = ::LoadLibraryEx(strDll.c_str(),NULL,LOAD_LIBRARY_AS_DATAFILE) ;

	if(hInst!=NULL)
	{ 
	TCHAR szBuf[128+1] = {0} ;
	::LoadString(hInst,IDS_LANGUAGE_NAME,szBuf,sizeof(szBuf)/sizeof(TCHAR)) ;

	if(szBuf[0]!=0 && ::_tcscmp(_T("Polski"),szBuf)==0)
	{// 如果语言真的为波兰语，则加载语言
	CptMultipleLanguage::GetInstance(_T("XCRes_POL.dll")) ;
	bLoad = true ;
	}

	//strDLLVer.push_back(szBuf) ;
	::FreeLibrary(hInst) ;

	hInst = NULL ;
	}


	}

	if(!bLoad)
	{
	::MessageBox(NULL,_T("Can't load language resource file !"),NULL,MB_OK) ;
	}

	/**/

	CptString strDll = strLanguagePath + _T("\\") + pConfigData->strResourceDLL ;

	if(!IsFileExist(strDll.c_str()))
	{
		typedef pt_STL_map(std::basic_string<TCHAR>,std::basic_string<TCHAR>)	Str2StrMap_t ; 

		Str2StrMap_t LanguageName2DLLNameMap ;

		::SearchResourceDLL(LanguageName2DLLNameMap,strLanguagePath) ;

		if(!LanguageName2DLLNameMap.empty())
		{
			pConfigData->strResourceDLL = (*LanguageName2DLLNameMap.begin()).second.c_str() ;

			CXCConfiguration::GetInstance()->SaveConfigDataToFile(*pConfigData) ;
		}
		else
		{
			::MessageBox(NULL,_T("Can't load resource file !"),NULL,MB_OK) ;
			return false ;
		}
	}

	// 加载语言
	CptMultipleLanguage::GetInstance(pConfigData->strResourceDLL.c_str()) ;
	return true ;
}

void XCRunCheckAndUpdate(const SConfigData& config)
{
	CXCNewChecker nc ;

	nc.CheckAndUpdateExtremeCopy(config.uLastCheckUpdateTime) ; 

	// 更新在设置里的最新检查更新日期
	SConfigData cd ;
	CXCConfiguration::GetInstance()->LoadConfigDataFromFile(cd) ;
	cd.uLastCheckUpdateTime = CptTime::Now() ;

	CXCConfiguration::GetInstance()->SaveConfigDataToFile(cd) ;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	{
		SConfigData*	pConfigData = new SConfigData();

		//读取配置
		CXCConfiguration::GetInstance()->LoadConfigDataFromFile(*pConfigData) ;

		if(::LoadXCResource(pConfigData))
		{// 加载语言资源

			XCRunCheckAndUpdate(*pConfigData) ;
			//SXCCopyTaskInfo task ;
			//CptString strError ;

			////// 默认是使用本地ini文件的 rich copy selection 设置
			//CXCConfiguration::GetInstance()->LoadConfigShareFromFile(task.ConfigShare) ;

			//if(CXCCommandLine::ParseCmdLine2TaskInfo(lpCmdLine,task,strError) )
			//{
			//}
		}

		CptMultipleLanguage::Release() ;
		CXCConfiguration::Release() ;

		SAFE_DELETE_MEMORY(pConfigData) ;
	}

	return 0 ;
	
}
