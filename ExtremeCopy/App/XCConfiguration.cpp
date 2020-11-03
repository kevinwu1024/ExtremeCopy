/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#include "StdAfx.h"
#include "XCConfiguration.h"
#include "Language\XCRes_ENU\resource.h"

#define INI_APP_SECTION_NAME	_T("App")

CXCConfiguration* CXCConfiguration::m_pInstance = NULL ;

CXCConfiguration::CXCConfiguration(void):m_bIniGlobalData(false)
{
}

CXCConfiguration::~CXCConfiguration(void)
{
	if(m_bIniGlobalData)
	{
		m_bIniGlobalData = false ;
		m_GlobalData.Release() ;
	}
}

CXCConfiguration* CXCConfiguration::GetInstance()
{
	if(m_pInstance==NULL)
	{
		m_pInstance = new CXCConfiguration() ;
	}

	return m_pInstance ;
}

void CXCConfiguration::Release() 
{
	if(m_pInstance!=NULL)
	{
		delete m_pInstance ;
		m_pInstance = NULL ;
	}
}

const SGlobalData* CXCConfiguration::GetGlobalData()
{
	if(!m_bIniGlobalData)
	{
		this->LoadGlobalData() ;

		m_bIniGlobalData = true ;
	}

	return &m_GlobalData ;
}

void CXCConfiguration::LoadGlobalData() 
{
	m_GlobalData.hSpeedBitmapNormal = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_SPEEDBUTTONNORMAL) ;//::LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP_MINBUTTONNORMAL)) ;
	m_GlobalData.hSpeedBitmapHover = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_SPEEDBUTTONHOVER) ;//::LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP_MINBUTTONHOVER)) ;
	m_GlobalData.hSpeedBitmapDown = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_SPEEDBUTTONDOWN) ;//::LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP_MINBUTTONDOWN)) ;

	m_GlobalData.hCursorHand = ::CptMultipleLanguage::GetInstance()->GetCursor(IDC_CURSOR_HAND) ;
}

bool CXCConfiguration::GetIniFile(TCHAR* szIniFile,HMODULE hModule) const
{
	_ASSERT(szIniFile!=NULL) ;

	if(szIniFile==NULL)
	{
		return false ;
	}
	TCHAR* pExeFileNamle = NULL ;

	TCHAR szFileName[8*MAX_PATH] = {0} ;

	::GetModuleFileName(hModule,szFileName,sizeof(szFileName)/sizeof(TCHAR)) ;
	::GetFullPathName(szFileName,sizeof(szFileName)/sizeof(TCHAR),szIniFile,&pExeFileNamle) ;

#pragma warning(push)
#pragma warning(disable:4996)
	::_tcscpy(pExeFileNamle,_T("Config.ini")) ;
#pragma warning(pop)

	return true ;
}

void CXCConfiguration::LoadConfigDataFromFile(SConfigData& config,HMODULE hModule) const
{
	const TCHAR* pSectionName = INI_APP_SECTION_NAME ;

	TCHAR szFileName[8*MAX_PATH] = {0} ;
	TCHAR szIniFile[8*MAX_PATH] = {0} ;

	this->GetIniFile(szIniFile,hModule) ;

//	TCHAR* pExeFileNamle = NULL ;
//
	
//	TCHAR szIniFile[8*MAX_PATH] = {0} ;
//
//	::GetModuleFileName(hModule,szFileName,sizeof(szFileName)/sizeof(TCHAR)) ;
//
//	::GetFullPathName(szFileName,sizeof(szIniFile)/sizeof(TCHAR),szIniFile,&pExeFileNamle) ;
//
//#pragma warning(push)
//#pragma warning(disable:4996)
//	::_tcscpy(pExeFileNamle,_T("Config.ini")) ;
//#pragma warning(pop)

	config.bDefaultCopying = ::GetPrivateProfileInt(pSectionName,_T("DefaultCopying"),1,szIniFile) ? true : false ;
	config.bPlayFinishedSound = ::GetPrivateProfileInt(pSectionName,_T("PlayFinishedSound"),0,szIniFile) ? true : false ;
	config.bTopMost = ::GetPrivateProfileInt(pSectionName,_T("TopMost"),1,szIniFile) ? true : false ;

	szFileName[0] = 0 ;
	::GetPrivateProfileString(pSectionName,_T("FinishedSoundFile"),_T("Done.wav"),szFileName,sizeof(szFileName)/sizeof(TCHAR),szIniFile) ;

	config.strSoundFile = szFileName ;

	// Ä¬ÈÏ±¾µØÓïÑÔ
	TCHAR szDefaultLangDLL[32] = { 0 };
	LCID lcid = GetUserDefaultUILanguage();
	if (lcid == 0x0804)
	{
		_tcscpy(szDefaultLangDLL, _T("XCRes_CHN.dll"));
	}
	else
	{
		_tcscpy(szDefaultLangDLL, _T("XCRes_ENU.dll"));
	}

	szFileName[0] = 0 ;
	::GetPrivateProfileString(pSectionName,_T("ResourceDLL"), szDefaultLangDLL,szFileName,sizeof(szFileName)/sizeof(TCHAR),szIniFile) ;

	config.strResourceDLL = szFileName ;

	config.UIType = ::GetPrivateProfileInt(pSectionName,_T("UI"),0,szIniFile) ? UI_Normal : UI_Simple ;

	config.bMinimumToTray = ::GetPrivateProfileInt(pSectionName,_T("MinimumToTray"),1,szIniFile) ? true : false ;

	config.nMaxFailedFiles = ::GetPrivateProfileInt(pSectionName,_T("MaxIgnoreFailedFiles"),1000,szIniFile) ;

	config.bWriteLog = ::GetPrivateProfileInt(pSectionName,_T("WriteLog"),0,szIniFile) ? true : false ;

	config.bCloseWindowAfterDone = ::GetPrivateProfileInt(pSectionName,_T("CloseWindowAfterDone"),1,szIniFile) ? true : false ;

	config.bAutoUpdate = ::GetPrivateProfileInt(pSectionName,_T("AutoUpdate"),1,szIniFile) ? true : false ;
	config.bAutoQueueMultipleTask = ::GetPrivateProfileInt(pSectionName,_T("AutoQueueMultipleTasks"),1,szIniFile) ? true : false ;
	config.uLastCheckUpdateTime = (time_t)::GetPrivateProfileInt(pSectionName,_T("LastCheckTime"),0,szIniFile) ;

	int nFileBufSize = ::GetPrivateProfileInt(pSectionName,_T("CopyBufSize"),32,szIniFile) ;

	switch(nFileBufSize)
	{
	case 16:
	case 8:
	case 4:
	case 2:
		config.nCopyBufSize = nFileBufSize ;
		break ;

	default:
	case 32:
		config.nCopyBufSize = 32 ;
		break ;
	}
	
}

void CXCConfiguration::SaveConfigDataToFile(const SConfigData& config) const
{
	const TCHAR* pSectionName = INI_APP_SECTION_NAME ;
	TCHAR szFileName[8*MAX_PATH] = {0} ;
	TCHAR szIniFile[8*MAX_PATH] = {0} ;

	this->GetIniFile(szIniFile,::GetModuleHandle(NULL)) ;

//	const TCHAR* pSectionName = _T("App") ;
//	//const TCHAR* pFileName = _T("Config.ini") ;
//	TCHAR* pExeFileNamle = NULL ;
//
//	TCHAR szFileName[8*MAX_PATH] = {0} ;
//	TCHAR szIniFile[8*MAX_PATH] = {0} ;
//
//	HMODULE hModule = ::GetModuleHandle(NULL) ;
//
//	::GetModuleFileName(hModule,szFileName,sizeof(szFileName)/sizeof(TCHAR)) ;
//
//	::GetFullPathName(szFileName,sizeof(szIniFile)/sizeof(TCHAR),szIniFile,&pExeFileNamle) ;
//
//#pragma warning(push)
//#pragma warning(disable:4996)
//	::_tcscpy(pExeFileNamle,_T("Config.ini")) ;
//#pragma warning(pop)

	::WritePrivateProfileString(pSectionName,_T("DefaultCopying"),config.bDefaultCopying?_T("1"):_T("0"),szIniFile) ;
	::WritePrivateProfileString(pSectionName,_T("PlayFinishedSound"),config.bPlayFinishedSound?_T("1"):_T("0"),szIniFile) ;
	::WritePrivateProfileString(pSectionName,_T("TopMost"),config.bTopMost?_T("1"):_T("0"),szIniFile) ;

	::WritePrivateProfileString(pSectionName,_T("FinishedSoundFile"),config.strSoundFile.c_str(),szIniFile) ;

	::WritePrivateProfileString(pSectionName,_T("ResourceDLL"),config.strResourceDLL.c_str(),szIniFile) ;

	::WritePrivateProfileString(pSectionName,_T("UI"),config.UIType==UI_Simple ? _T("0"):_T("1"),szIniFile) ;

	::WritePrivateProfileString(pSectionName,_T("MinimumToTray"),config.bMinimumToTray? _T("1"):_T("0"),szIniFile) ;

	::WritePrivateProfileString(pSectionName,_T("WriteLog"),config.bWriteLog? _T("1"):_T("0"),szIniFile) ;

	::WritePrivateProfileString(pSectionName,_T("CloseWindowAfterDone"),config.bCloseWindowAfterDone? _T("1"):_T("0"),szIniFile) ;

#pragma warning(push)
#pragma warning(disable:4996)
	::_stprintf (szFileName,_T("%d"),config.nMaxFailedFiles) ;
	::WritePrivateProfileString(pSectionName,_T("MaxIgnoreFailedFiles"),szFileName,szIniFile) ;

	::_stprintf (szFileName,_T("%d"),config.nCopyBufSize) ;
	::WritePrivateProfileString(pSectionName,_T("CopyBufSize"),szFileName,szIniFile) ;

	::_stprintf (szFileName,_T("%u"),config.uLastCheckUpdateTime) ;
	::WritePrivateProfileString(pSectionName,_T("LastCheckTime"),szFileName,szIniFile) ;
#pragma warning(pop)
	::WritePrivateProfileString(pSectionName,_T("AutoUpdate"),config.bAutoUpdate? _T("1"):_T("0"),szIniFile) ;

	::WritePrivateProfileString(pSectionName,_T("AutoQueueMultipleTasks"),config.bAutoQueueMultipleTask? _T("1"):_T("0"),szIniFile) ;

}

void CXCConfiguration::LoadConfigShareFromFile(SXCTaskAndConfigShare& ConfigShare,HMODULE hModule) const 
{
	const TCHAR* pSectionName = INI_APP_SECTION_NAME ;
	TCHAR szIniFile[8*MAX_PATH] = {0} ;

	this->GetIniFile(szIniFile,hModule) ;

	ConfigShare.bVerifyData = ::GetPrivateProfileInt(pSectionName,_T("VerifyData"),0,szIniFile) ? true : false ;

	ConfigShare.ptStartupPos.nX = ::GetPrivateProfileInt(pSectionName,_T("StartupPosX"),CONFIG_DEFAULT_MAINDIALOG_POS_X,szIniFile) ;
	ConfigShare.ptStartupPos.nY = ::GetPrivateProfileInt(pSectionName,_T("StartupPosY"),CONFIG_DEFAULT_MAINDIALOG_POS_Y,szIniFile) ;

	ConfigShare.nRetryTimes = ::GetPrivateProfileInt(pSectionName,_T("RetryTimes"),1,szIniFile) ;

	//ConfigShare.bShutdownAfterDone = ::GetPrivateProfileInt(pSectionName,_T("ShutdownAfterDone"),1,szIniFile) ? true : false ;
#ifdef VERSION_PROFESSIONAL

	SRichCopySelection::EErrorProcessType epc = (SRichCopySelection::EErrorProcessType)::GetPrivateProfileInt(pSectionName,_T("ErrorProcess"),SRichCopySelection::EPT_Ignore,szIniFile) ;
	
	switch(epc)
	{
	case SRichCopySelection::EPT_Ignore:
	case SRichCopySelection::EPT_Ask:
	case SRichCopySelection::EPT_Exit:
	case SRichCopySelection::EPT_Retry:
		ConfigShare.epc = epc ;
		break ;

	default:
		_ASSERT(FALSE) ;
		ConfigShare.epc = SRichCopySelection::EPT_Ignore ;
		break ;
	}

	SRichCopySelection::EErrorProcessType RetryFailThen = (SRichCopySelection::EErrorProcessType)::GetPrivateProfileInt(pSectionName,_T("RetryFailedThenProcess"),SRichCopySelection::SFPT_Ask,szIniFile) ;

	switch(RetryFailThen)
	{
	case SRichCopySelection::EPT_Ignore:
	case SRichCopySelection::EPT_Ask:
	case SRichCopySelection::EPT_Exit:
		ConfigShare.RetryFailThen = RetryFailThen ;
		break ;

	default:
		_ASSERT(FALSE) ;
		ConfigShare.RetryFailThen = SRichCopySelection::EPT_Ignore ;
		break ;
	}

	SRichCopySelection::ESameFileProcessType sfpt = (SRichCopySelection::ESameFileProcessType)::GetPrivateProfileInt(pSectionName,_T("SameFileProcess"),SRichCopySelection::SFPT_Ask,szIniFile) ;

	switch(sfpt)
	{
	case SRichCopySelection::SFPT_Ask:
	case SRichCopySelection::SFPT_Rename:
	case SRichCopySelection::SFPT_Replace:
	case SRichCopySelection::SFPT_Skip:
	case SRichCopySelection::SFPT_IfCondition:
		ConfigShare.sfpt = sfpt ;
		break ;

	default:
		_ASSERT(FALSE) ;
		ConfigShare.sfpt = SRichCopySelection::SFPT_Ask ;
		break ;
	}

	// same file condition
	TCHAR DefaultText[32] = {0} ;
	TCHAR buffer[32] = {0} ;
#pragma warning(push)
#pragma warning(disable:4996)
	::_stprintf (DefaultText,_T("%d|%d|%d"),SRichCopySelection::FDT_Newer,SRichCopySelection::SFOT_Replace,SRichCopySelection::SFOT_Ask) ;
#pragma warning(pop)

	::GetPrivateProfileString(pSectionName,_T("SameFileOperationCondition"),DefaultText,buffer,sizeof(buffer)/sizeof(TCHAR),szIniFile) ;

	bool bIsParseConditionOK = false ;
	CptStringList sl ;
	if(sl.Split(buffer,_T("|"))==3)
	{
		try
		{
			int IfConfition = sl[0].AsInt() ;
			int ThenOperation = sl[1].AsInt() ;
			int OtherwiseOperation = sl[2].AsInt() ;

			if((IfConfition>=0 && IfConfition<SRichCopySelection::FDT_Last)
				&& (ThenOperation>=0 && ThenOperation<SRichCopySelection::SFOT_Last)
				&& (OtherwiseOperation>=0 && OtherwiseOperation<SRichCopySelection::SFOT_Last))
			{
				ConfigShare.sfic.IfCondition = (SRichCopySelection::EFileDifferenceType)IfConfition ;
				ConfigShare.sfic.ThenOperation = (SRichCopySelection::ESameFileOperationType)ThenOperation ;
				ConfigShare.sfic.OtherwiseOperation = (SRichCopySelection::ESameFileOperationType)OtherwiseOperation ;

				bIsParseConditionOK = true ;
			}
		}
		catch(...)
		{
			bIsParseConditionOK = false ;
		}
	}

	if(!bIsParseConditionOK)
	{
		ConfigShare.sfic.IfCondition = SRichCopySelection::FDT_Newer ;
		ConfigShare.sfic.ThenOperation = SRichCopySelection::SFOT_Replace ;
		ConfigShare.sfic.OtherwiseOperation = SRichCopySelection::SFOT_Ask ;
	}

#else
	ConfigShare.epc = SRichCopySelection::EPT_Ignore ;
	ConfigShare.RetryFailThen = SRichCopySelection::EPT_Ignore ;
	ConfigShare.sfpt = SRichCopySelection::SFPT_Ask ;

	ConfigShare.sfic.IfCondition = SRichCopySelection::FDT_Newer ;
	ConfigShare.sfic.ThenOperation = SRichCopySelection::SFOT_Replace ;
	ConfigShare.sfic.OtherwiseOperation = SRichCopySelection::SFOT_Ask ;
#endif
}

void CXCConfiguration::SaveConfigShareToFile(const SXCTaskAndConfigShare& ConfigShare) const 
{
	const TCHAR* pSectionName = INI_APP_SECTION_NAME ;
	TCHAR szIniFile[8*MAX_PATH] = {0} ;
	TCHAR szVal[64] = {0} ;

	this->GetIniFile(szIniFile,::GetModuleHandle(NULL)) ;

	::WritePrivateProfileString(pSectionName,_T("VerifyData"),ConfigShare.bVerifyData? _T("1"):_T("0"),szIniFile) ;

#pragma warning(push)
#pragma warning(disable:4996)
	::_stprintf (szVal,_T("%d"),ConfigShare.ptStartupPos.nX) ;
	::WritePrivateProfileString(pSectionName,_T("StartupPosX"),szVal,szIniFile) ;

	::_stprintf (szVal,_T("%d"),ConfigShare.ptStartupPos.nY) ;
	::WritePrivateProfileString(pSectionName,_T("StartupPosY"),szVal,szIniFile) ;

	::_stprintf (szVal,_T("%d"),ConfigShare.nRetryTimes) ;
	::WritePrivateProfileString(pSectionName,_T("RetryTimes"),szVal,szIniFile) ;

	::_stprintf (szVal,_T("%d"),ConfigShare.epc) ;
	::WritePrivateProfileString(pSectionName,_T("ErrorProcess"),szVal,szIniFile) ;

	::_stprintf (szVal,_T("%d"),ConfigShare.RetryFailThen) ;
	::WritePrivateProfileString(pSectionName,_T("RetryFailedThenProcess"),szVal,szIniFile) ;

	::_stprintf (szVal,_T("%d"),ConfigShare.sfpt) ;
	::WritePrivateProfileString(pSectionName,_T("SameFileProcess"),szVal,szIniFile) ;

	::_stprintf(szVal,_T("%d|%d|%d"),ConfigShare.sfic.IfCondition,ConfigShare.sfic.ThenOperation,ConfigShare.sfic.OtherwiseOperation) ;
	::WritePrivateProfileString(pSectionName,_T("SameFileOperationCondition"),szVal,szIniFile) ;
#pragma warning(pop)
}