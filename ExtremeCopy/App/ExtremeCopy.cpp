/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#include "stdafx.h"

#include <commctrl.h>
#include "ExtremeCopy.h"
#include "XCRunMode.h"
#include "XCVerifyResult.h"
#include "../Common/sqlite/CppSQLite3U.h"

#pragma comment(lib,"comctl32.lib")

/**
void WriteToFile(const SXCCopyTaskInfo& sti)
{
HANDLE hFile = ::CreateFile("c:\\xc\\copy_sti.txt",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL) ;

if(hFile!=INVALID_HANDLE_VALUE)
{
DWORD dwWritten = 0 ;

//MessageBox(NULL,"5","3",0) ;
if(sti.CopyType == CXCTransObject::TransType_Move)
{
::WriteFile(hFile,"move \r\n",strlen("move \r\n"),&dwWritten,NULL) ;
}
else
{
::WriteFile(hFile,"copy \r\n",strlen("copy \r\n"),&dwWritten,NULL) ;
}




for(int i=0;i<sti.strSrcFileVer.size();++i)
{
::WriteFile(hFile,sti.strSrcFileVer[i].c_str(),sti.strSrcFileVer[i].GetLength(),&dwWritten,NULL) ;
::WriteFile(hFile,"\r\n",2,&dwWritten,NULL) ;
}

if(sti.strDstFile.GetLength()>0)
{
::WriteFile(hFile,sti.strDstFile.c_str(),sti.strDstFile.GetLength(),&dwWritten,NULL) ;
::WriteFile(hFile,"\r\n",2,&dwWritten,NULL) ;
}

//::WriteFile(hFile,pCmd,nSize,&dwWritten,NULL) ;
::CloseHandle(hFile) ;
}
}

void WriteToFile(const void* pBuf,int nSize,const char* pFileName=NULL)
{
CptString strFileName("c:\\xc\\copy.txt") ;

if(pFileName!=NULL)
{
strFileName.Format("c:\\xc\\%s",pFileName) ;
}


HANDLE hFile = ::CreateFile(strFileName.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL) ;

if(hFile!=INVALID_HANDLE_VALUE)
{
DWORD dwWritten = 0 ;
::WriteFile(hFile,pBuf,nSize,&dwWritten,NULL) ;
::CloseHandle(hFile) ;
}
}
/**

void SetClip()
{
if(::OpenClipboard(NULL))
{
const WCHAR* pCmd = L"command: xcmove\r\nsource size: 3\r\nc:\\abc.txt\r\nd:\\my_folder\r\ne:\\files.cpp\r\ndestination size: 1\r\nf:\\target_folder\r\n\r\n" ;

//int kk = ::wcslen(pCmd) ;
//HANDLE hMem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,::wcslen(pCmd)*sizeof(WCHAR)+2) ;
HANDLE hMem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,2) ;

if(hMem!=NULL)
{
int nFormat = ::RegisterClipboardFormat(_T("MaiHua XetremCopy Cmd")) ;

int nSize = ::GlobalSize(hMem) ;
WCHAR* p = (WCHAR*)::GlobalLock(hMem) ;

//WCHAR* dd = ::wcscpy(p,pCmd) ;

HANDLE h = ::SetClipboardData(CF_TEXT,hMem) ;

int aa = ::GetLastError() ;

::GlobalUnlock(hMem) ;
}
::CloseClipboard() ;
}

}
/**/

//#include "XCSqlLog.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	::SetUnhandledExceptionFilter(XCReportBug) ;

	{
		SConfigData*	pConfigData = new SConfigData();

		//读取配置
		CXCConfiguration::GetInstance()->LoadConfigDataFromFile(*pConfigData) ;

		if(::LoadXCResource(pConfigData))
		{// 加载语言资源
			
			SXCCopyTaskInfo task ;
			CptString strError ;

			//// 默认是使用本地ini文件的 rich copy selection 设置
			CXCConfiguration::GetInstance()->LoadConfigShareFromFile(task.ConfigShare) ;

			if(CXCCommandLine::ParseCmdLine2TaskInfo(lpCmdLine,task,strError) )
			{
				INITCOMMONCONTROLSEX icc;
				icc.dwSize =	sizeof(INITCOMMONCONTROLSEX);
				icc.dwICC =ICC_BAR_CLASSES | ICC_TAB_CLASSES  ;

				InitCommonControlsEx(&icc);

				switch(task.cmd)
				{
				case SXCCopyTaskInfo::XCMD_Null:
					::XCRunModeNull(task,*pConfigData) ;
					break ;

				case SXCCopyTaskInfo::XCMD_About:
					::XCRunModeAbout() ;
					break ;

				case SXCCopyTaskInfo::XCMD_Config:
					::XCRunModeConfiguration() ;
					break ;
					 
#ifdef VERSION_PROFESSIONAL
				case SXCCopyTaskInfo::XCMD_Register:
					::XCRunModeRegister() ;
					break ;
#endif

#ifndef VERSION_PORTABLE
				case SXCCopyTaskInfo::XCMD_Shell:
					::XCRunModeShell(task,*pConfigData) ;
					break ;
#endif

				case SXCCopyTaskInfo::XCMD_XtremeRun:
					::XCRunModeXtremeRun(task,*pConfigData,true) ;
					break ;

				case SXCCopyTaskInfo::XCMD_TaskDlg:
					::XCRunModeXtremeTaskDlg(task,*pConfigData) ;
					break ;

				case SXCCopyTaskInfo::XCMD_CheckUpdate:
					::XCRunCheckAndUpdate(task,*pConfigData) ;
					break ;

				default:
					break ;

				}
			}
			else
			{// 命令行解析失败
				CptString strErrorTitle = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_ERROR) ;
				CptString strCmdLineError = ::CptMultipleLanguage::GetInstance()->GetString(IDS_CMDLN_ERROR_CMDLNERROR) ;

				CptString strTxt = strCmdLineError + strError + _T("\r\n");

				CptMessageBox::ShowMessage(NULL,strTxt,strErrorTitle,CptMessageBox::Button_OK) ;
			}
		}

		CptMultipleLanguage::Release() ;
		CXCConfiguration::Release() ;

		SAFE_DELETE_MEMORY(pConfigData) ;
	}

	//Release_Printf(_T("")) ;
	//		_CrtMemState cms2 ;
	//_CrtMemCheckpoint(&cms2) ;

	//_CrtMemState cms3 ;

	//if(_CrtMemDifference(&cms3,&cms1,&cms2))
	//{
	//	Debug_Printf(_T("There is memory leak")) ;
	//}
	//else
	//{
	//	Debug_Printf(_T("There is no memory leak")) ;
	//}


	return 0 ;
}
