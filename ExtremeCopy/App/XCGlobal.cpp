/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "stdafx.h"
#include "XCGlobal.h"

#include <time.h>
#include "XCConfig.h"
#include "XCCopyTask.h"

#include "Winver.h"
#include <shellapi.h>

#pragma comment(lib,"Version.lib") 

// 这是使用XP风格的控件 （默认是早期的WINDOWS控件）

#if defined _WIN64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//#ifdef _UNICODE
//#if defined _M_IX86
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_X64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#else
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif
//#endif

/*
bool ChangeDesktopFilePosition(const std::list<CptString>& FileList,const SptPoint& CursorPos)
{
	if(FileList.empty())
	{
		return false ;
	}

	HWND  hwndParent = ::FindWindow( _T("Progman"), _T("Program Manager") ); 
	HWND  hwndSHELLDLL_DefView = ::FindWindowEx( hwndParent, NULL, _T("SHELLDLL_DefView"), NULL ); 
	HWND  hwndSysListView32 = ::FindWindowEx( hwndSHELLDLL_DefView, NULL, _T("SysListView32"), _T("FolderView") );

	int nNumber = ListView_GetItemCount( hwndSysListView32 );

	struct SFilePositionDesc
	{
		CptString FileName ;
		bool bFound ;
		int nIndex ;
		SptPoint Pos ;
	};

	std::list<SFilePositionDesc> FilePosList ;

	std::list<CptString>::iterator it = FileList.begin() ;

	for(;it!=FileList.end();++it)
	{
		SFilePositionDesc desc ;

		desc.FileName = (*it) ;
		desc.bFound = false ;

		FilePosList.push_back(desc) ;
	}

	TCHAR lpszText[MAX_PATH] = {0} ;

	// 在 list view 里查找对应文件名，并计算和记录下它的位置
	for(int i=0;i<nNumber;++i)
	{
		LVITEM item ;
		::memset(&item,0,sizeof(item)) ;

		item.iSubItem = nSubItemIndex ;
		item.pszText = lpszText ;
		item.cchTextMax = nBufSize ;

		if(::SendMessage(m_hWnd,LVM_GETITEMTEXT,nItemIndex,(LPARAM)(LVITEM *)&item) ? true : false)
		{
			std::list<SFilePositionDesc>::iterator it2 = FilePosList.begin() ;

			for(;it2!=FilePosList.end();++it2)
			{
				if(!(*it2).bFound)
				{
					if((*it2).FileName.CompareNoCase(lpszText)==0)
					{
						(*it2).bFound = true ;
						(*it2).nIndex = i ;
						break ;
					}
				}
			}
		}
	}

	// 更改文件位置
	std::list<SFilePositionDesc>::iterator it3 = FilePosList.begin() ;

	for(;it3!=FilePosList.end();++it3)
	{
		if((*it3).bFound)
		{
			::SendMessage( hwndSysListView32,   LVM_SETITEMPOSITION, (*it3).nIndex,   MAKELPARAM( (*it3).Pos.nX,(*it3).Pos.nY));
		}
	}
	//

	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	::UpdateWindow(hwndSysListView32);

	return false ;
}
*/
 
bool IsRecycleFile(const CptString& strFile,CptString& strOriginName)
{
	bool bRet = false ;

	CptWinPath::SPathElementInfo pei ;

	pei.uFlag = CptWinPath::PET_Path ;

	CptString strFile2 = CptGlobal::MakeUnlimitFileName(strFile,false) ;

	if(CptWinPath::GetPathElement(strFile2.c_str(),pei))
	{
		if(CptGlobal::IsRecycleFolder(pei.strPath.c_str()))
		{
			CptString strOriginFile ;

			if(CptGlobal::GetRecycleFileOriginName(strFile2.c_str(),strOriginFile))
			{
				pei.uFlag = CptWinPath::PET_FileName ;

				if(CptWinPath::GetPathElement(strOriginFile.c_str(),pei))
				{
					strOriginName = pei.strFileName ;

					bRet = true ;
				}
			}
		}
	}
	else
	{
		_ASSERT(FALSE) ;
	}

	return bRet ;
}

// 转换文件大小的"数字"到"字符串"
CptString GetSizeString(unsigned __int64 nValue) 
{
	CptString str ;

	if(nValue>1024*1024*1024)
	{// 大
		unsigned __int64 nTem = (unsigned __int64)1024*1024*1024 ;

		nTem = nTem * 1024 ;

		if(nValue>nTem)
		{// T
			const float fSpeed = nValue/(nTem*1.0f) ;
			str.Format(_T("%.1f T"),fSpeed) ;
			//::sprintf(szSize,"%.2f T",m_nTransSize/(nTem*1.0f)) ;
		}
		else
		{// G
			const float fSpeed = nValue/(1024.0f*1024*1024) ;
			str.Format(_T("%.1f G"),fSpeed) ;
			//::sprintf(szSize,"%.2f G",m_nTransSize/(1024.0f*1024*1024)) ;
		}
	}
	else
	{// 小

		if(nValue>1024*1024)
		{// M
			const float f = nValue/ (1024.0f*1024.0f) ;
			//::sprintf(szSize,"%.2f M",f) ;
			str.Format(_T("%.1f M"),f) ;
		}
		else
		{// K
			const float f = nValue/ 1024.0f ;
			//::sprintf(szSize,"%.2f K",f) ;
			str.Format(_T("%.1f K"),f) ;
		}
	}

	return str ;
}

unsigned __int64 DoubleWordTo64( const DWORD dwLow, const DWORD dwHight)
{
	unsigned __int64 dwRet = 0;

	if(dwHight>0)
	{
		dwRet = ((unsigned __int64)dwHight)<<32 ;
	}

	dwRet = dwRet + (unsigned __int64)dwLow ;

	return dwRet ;
}



void TransparenWnd(HWND hWnd,int nPercent)
{
	if(nPercent==0)
	{
		if(GetWindowLong(hWnd, GWL_EXSTYLE)&0x80000L)
		{
			SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) & ~0x80000L);
		}
	}
	else
	{
		typedef BOOL(__stdcall *TransFunc1_t)(HWND hWnd,COLORREF crKey,BYTE bAlpha,DWORD dwFlags)  ;

		HMODULE hModule = ::GetModuleHandle(_T("user32.dll"));
		TransFunc1_t SetLayeredWindowAttributes = NULL ;
		SetLayeredWindowAttributes =  (TransFunc1_t) ::GetProcAddress (hModule, "SetLayeredWindowAttributes" );
		
		if(SetLayeredWindowAttributes)
		{
			SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | 0x80000L);
			
			//const float fAlpha = 255*(1.0-((float)nPercent/255.0)) ;
			const float fAlpha = 255*(nPercent/100.0f) ;
			
		//	Debug_Printf("Alpha=%.2f",fAlpha) ;
			
			SetLayeredWindowAttributes(hWnd, 0,(BYTE)fAlpha , 0x2);
		}
	}
}

bool ConfirmExitApp(HWND hParentWnd)
{
#ifndef _DEBUG

	// 若该窗口隐藏,则先把它恢复
	::ShowWindow(hParentWnd,SW_RESTORE) ;
	//::ShowWindow(hParentWnd,SW_SHOW) ;
	//::PostMessage(hParentWnd,WM_SYSCOMMAND,SIZE_RESTORED,0) ;

	CptString strText = ::CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_SUREEXIT) ;
	CptString strWarning = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;

	return (::MessageBox(hParentWnd,strText.c_str(),strWarning.c_str(),MB_YESNO)==IDYES) ;
	//return true ;
	//return (CptMessageBox::ShowMessage(hParentWnd,strText,strWarning,CptMessageBox::Button_YesNo)==CptMessageBox::MsgResult_Yes) ;

#else
	return true ;
#endif
}


//
//bool GetPEFileVersion(LPCTSTR lpFileName,int& nMajorVer,int& nMinVer,int& FixVer,int& BuildVer)
//{
//	bool bRet = false ;
//
//	DWORD dwBlockSize = 0;
//	DWORD dwBlockHandle = 0;
//
//	dwBlockSize = ::GetFileVersionInfoSize( lpFileName, &dwBlockHandle );
//
//	if( dwBlockSize != 0 )
//	{//  Get the version information
//		char* szBlock = (char*) ::malloc( dwBlockSize );
//
//		if(szBlock!=NULL)
//		{
//			BOOL bSuccess = ::GetFileVersionInfo( lpFileName, dwBlockHandle, dwBlockSize, szBlock );
//
//			if( bSuccess )
//			{
//				//  Get the fixed file information from that file
//				VS_FIXEDFILEINFO* pVsfi;
//				unsigned int cVsfi;
//
//				if( ::VerQueryValue( szBlock, _T("\\"), (void**) &pVsfi, &cVsfi ))
//				{
//					if(cVsfi == sizeof(VS_FIXEDFILEINFO))
//					{
//						nMajorVer = HIWORD( pVsfi -> dwFileVersionMS ) ;
//						nMinVer = LOWORD( pVsfi -> dwFileVersionMS ) ;
//						FixVer = HIWORD( pVsfi -> dwFileVersionLS ) ;
//						BuildVer = LOWORD( pVsfi -> dwFileVersionLS ) ;
//
//						bRet = true ;
//					}
//				}
//			}
//
//			::free( szBlock );
//			szBlock = NULL ;
//		}
//	}
//
//
//	return bRet ;
//}
//
//void LoadConfigDataFromFile(SConfigData& config) 
//{
//	const TCHAR* pSectionName = _T("App") ;
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
//	::_tcscpy(pExeFileNamle,_T("Config.ini")) ;
//
//	config.bDefaultCopying = ::GetPrivateProfileInt(pSectionName,_T("DefaultCopying"),1,szIniFile) ? true : false ;
//	config.bPlayFinishedSound = ::GetPrivateProfileInt(pSectionName,_T("PlayFinishedSound"),1,szIniFile) ? true : false ;
//	config.bTopMost = ::GetPrivateProfileInt(pSectionName,_T("TopMost"),1,szIniFile) ? true : false ;
//
//	szFileName[0] = 0 ;
//	::GetPrivateProfileString(pSectionName,_T("FinishedSoundFile"),_T("Done.wav"),szFileName,sizeof(szFileName)/sizeof(TCHAR),szIniFile) ;
//
//	config.strSoundFile = szFileName ;
//
//	config.UIType = ::GetPrivateProfileInt(pSectionName,_T("UI"),0,szIniFile) ? UI_Normal : UI_Simple ;
//
//	config.bMinimumToTray = ::GetPrivateProfileInt(pSectionName,_T("MinimumToTray"),1,szIniFile) ? true : false ;
//
//	config.nCopyBufSize = ::GetPrivateProfileInt(pSectionName,_T("XCCopyBufSize"),32,szIniFile) ;
//
//	if(config.nCopyBufSize<=0)
//	{
//		config.nCopyBufSize = 32 ;
//	}
//	
//}
//
//void SaveConfigDataToFile(const SConfigData& config) 
//{
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
//	::_tcscpy(pExeFileNamle,_T("Config.ini")) ;
//
//	::WritePrivateProfileString(pSectionName,_T("DefaultCopying"),config.bDefaultCopying?_T("1"):_T("0"),szIniFile) ;
//	::WritePrivateProfileString(pSectionName,_T("PlayFinishedSound"),config.bPlayFinishedSound?_T("1"):_T("0"),szIniFile) ;
//	::WritePrivateProfileString(pSectionName,_T("TopMost"),config.bTopMost?_T("1"):_T("0"),szIniFile) ;
//
//	::WritePrivateProfileString(pSectionName,_T("FinishedSoundFile"),config.strSoundFile.c_str(),szIniFile) ;
//
//	::WritePrivateProfileString(pSectionName,_T("UI"),config.UIType==UI_Simple ? _T("0"):_T("1"),szIniFile) ;
//
//	::WritePrivateProfileString(pSectionName,_T("MinimumToTray"),config.bMinimumToTray? _T("1"):_T("0"),szIniFile) ;
//}

void OnHyperLinkHomePageCallBack2(void* pVoid)
{
	EWebLink LinkType = (EWebLink)(int)pVoid ;
	OpenLink(LinkType) ;
}

bool OpenLink(const EWebLink LinkType) 
{
	if(LinkType==WEBLINK_EMAIL_SUPPORT)
	{
		CptString strEmail ;

		strEmail.Format(_T("mailto:%s"),LINK_EMAIL_SUPPORT) ;
		return OpenLink(strEmail) ;
	}
	else if(LinkType==WEBLINK_SITE_HOME)
	{
		return OpenLink(LINK_HOME_WEBSITE) ;
	}
	else
	{
		CptString strURI ;
		CptString strURL = LINK_ROOT_APP_NAVIGATE;

		switch(LinkType)
		{
		case WEBLINK_BUY_STANDARD:
			strURI = LINK_URI_BUY_STANDARD ;
			break ;
		case WEBLINK_BUY_PROFESSIONAL:
			strURI = LINK_URI_BUY_PROFESSION ;
			break ;
		case WEBLINK_HELP_SAMEFILEDLG:
			strURI = LINK_URI_HELP_SAME_FILE_DLG ;
			break ;
		case WEBLINK_HELP_TASKDLG:
			strURI = LINK_URI_HELP_TASK_DLG ;
			break ;

		case WEBLINK_HELP_WILDCARDDLG:
			strURI = LINK_URI_HELP_WILDCARD_DLG ;
			break ;

		case WEBLINK_SITE_GETPROEDITION:
			strURI = LINK_URI_GET_PRO ;
			break ;

		default:
			return false ;
		}

		TCHAR szBuf[32] = {0} ;

		SptVerionInfo VerInfo ;
		CptGlobal::GetPEFileVersion(NULL,VerInfo) ;

		VerInfo.ToString(szBuf) ;

		strURI += _T("&ver=") ;
		strURI += szBuf ;

#ifdef VERSION_PROFESSIONAL 
		strURI += _T("&edition=pro") ;
#else
		strURI += _T("&edition=std") ;
#endif
		strURL += strURI ;

		return OpenLink(strURL.c_str()) ;
	}
	//return strURL ;
}

bool OpenLink(const TCHAR* pLink)
{
	return ((int)::ShellExecute(NULL,_T("open"),pLink,NULL,NULL,SW_SHOWNORMAL)>32) ;
}

// 判断是否改名的复制或移动文件
bool IsDstRenameFileName(const SGraphTaskDesc& gtd) 
{
	bool bRet = false ;

	if(gtd.SrcFileVer.size()==1 && gtd.DstFolderVer.size()==1)
	{
		DWORD dwDstAttr = ::GetFileAttributes(gtd.DstFolderVer[0].c_str()) ;

		if(dwDstAttr==INVALID_FILE_ATTRIBUTES || !CptGlobal::IsFolder(dwDstAttr))
		{
			DWORD dwSrcAttr = ::GetFileAttributes(gtd.SrcFileVer[0].c_str()) ;

			if(dwSrcAttr!=INVALID_FILE_ATTRIBUTES && !CptGlobal::IsFolder(dwSrcAttr))
			{
				CptWinPath::SPathElementInfo pei ;

				pei.uFlag = CptWinPath::PET_Path|CptWinPath::PET_FileName ;

				if(CptWinPath::GetPathElement(gtd.DstFolderVer[0].c_str(),pei)
					&& CptGlobal::IsFolder(pei.strPath))
				{// 此为改名的文件名
					bRet = true ;
				}
			}
		}

	}

	return bRet ;
}

CptString GetTaskXMLErrorString(const ETaskXMLErrorType& ErrorCode)
{
	CptString strRet ;

	switch(ErrorCode)
	{
	case TXMLET_CantSupportFormatVersion:
		strRet = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TASKXMLERROR_CANTSUPPORTVERSION) ;
		break ;

	case TXMLET_ExecuteError:
		strRet = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TASKXMLERROR_EXECUTECOMMAND) ;
		break ;

	case TXMLET_ShowModeError:
		strRet = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TASKXMLERROR_SHOWMODE) ;
		break ;

	case TXMLET_SameFileReactionError:
		strRet = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TASKXMLERROR_SAMEFILEREACTION) ;
		break ;

	case TXMLET_ErrorReactionError:
		strRet = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TASKXMLERROR_ERRORREACTION) ;
		break ;

	case TXMLET_OutOfRetryError:
		strRet = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TASKXMLERROR_OUTOFRETRY) ;
		break ;

	default:
	case TXMLET_InvalidTaskXMLData:
	case TXMLET_UnknownError:
		strRet = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TASKXMLERROR_INVALIDFORMAT) ;
		break ;

	case TXMLET_ThereAreNotSrcOrDstFileOrFolder:
		strRet = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TASKXMLERROR_NOSRCORDSTFILEORFOLDER) ;
		break ;
	}

	return strRet ;
}

// 判断是否有父目录复制到其子目录里
bool DoesIncludeRecuriseFolder(const SXCCopyTaskInfo& sti,int& nSrcIndex, int& nDstIndex)
{
	for(size_t i=0;i<sti.strDstFolderVer.size();++i)
	{
		for(size_t j=0;j<sti.strSrcFileVer.size();++j)
		{
			if(CptGlobal::IsFolder(sti.strDstFolderVer[i]) && 
				CptGlobal::IsFolder(sti.strSrcFileVer[j]) &&
				sti.strDstFolderVer[i].Find(sti.strSrcFileVer[j])==0)
			{
				if(sti.strDstFolderVer[i].GetLength()>sti.strSrcFileVer[j].GetLength())
				{
					const TCHAR c = sti.strDstFolderVer[i].GetAt(sti.strSrcFileVer[j].GetLength()) ;

					if(c=='\\' || c=='/')
					{
						nSrcIndex = (int)j ;
						nDstIndex = (int)i ;
						return true ;
					}
				}
			}
		}
	}

	return false ;
}

CptString MakeXCVersionString()
{
	CptString strRet ;

	TCHAR* pBuf = new TCHAR[4*MAX_PATH] ;

	if(pBuf!=NULL)
	{
		SptVerionInfo VerInfo ;
		if(CptGlobal::GetPEFileVersion(NULL,VerInfo))
		{

#ifdef VERSION_PROFESSIONAL 
			CptString strAppName = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_APP_PRO) ;
#else
			CptString strAppName = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_APP) ;
#endif

#ifdef VERSION_TEST_BETA
			strRet.Format(_T("%s %d.%d.%d  Beta %d"),strAppName.c_str(),VerInfo.nMajor,VerInfo.nMin,VerInfo.nFix,TEST_VERSION_NUMBER) ;
#else
#ifdef VERSION_TEST_ALPHA
			strRet.Format(_T("%s %d.%d.%d  Alpha %d"),strAppName.c_str(),VerInfo.nMajor,VerInfo.nMin,VerInfo.nFix,TEST_VERSION_NUMBER) ;
#else
			strRet.Format(_T("%s %d.%d.%d"),strAppName.c_str(),VerInfo.nMajor,VerInfo.nMin,VerInfo.nFix) ;
#endif
#endif
		}

		SAFE_DELETE_MEMORY_ARRAY(pBuf) ;
	}

	return strRet ;
}

int SearchResourceDLL(pt_STL_map(std::basic_string<TCHAR>,std::basic_string<TCHAR>)& LanguageName2DLLNameMap,CptString strFolder)
{
	int nRet = -1 ;

	LanguageName2DLLNameMap.clear() ;
	//strDLLVer.clear() ;

	WIN32_FIND_DATA wfd ;
	CptString strDLLFiles = strFolder + _T("\\*.dll") ;

	HANDLE hFileFind = ::FindFirstFile(strDLLFiles.c_str(),&wfd) ;

	if(hFileFind!=INVALID_HANDLE_VALUE)
	{
		CptString strFileName ;

		TCHAR szBuf[128+1] = {0} ;
		HMODULE hInst = NULL ;

		do
		{
			strFileName.Format(_T("%s\\%s"),strFolder,wfd.cFileName) ;

			hInst = ::LoadLibraryEx(strFileName.c_str(),NULL,LOAD_LIBRARY_AS_DATAFILE) ;

			if(hInst!=NULL)
			{
				szBuf[0] =0 ;
				::LoadString(hInst,IDS_LANGUAGE_NAME,szBuf,sizeof(szBuf)/sizeof(TCHAR)) ;

				if(szBuf[0]!=0)
				{
					if(LanguageName2DLLNameMap.find(szBuf)==LanguageName2DLLNameMap.end())
					{
						LanguageName2DLLNameMap[szBuf] = wfd.cFileName ;
					}
				}
				
				//strDLLVer.push_back(szBuf) ;
				::FreeLibrary(hInst) ;

				hInst = NULL ;
			}
		}
		while(::FindNextFile(hFileFind,&wfd)) ;

		::FindClose(hFileFind) ;
		hFileFind = INVALID_HANDLE_VALUE ;
	}

	return nRet ;
}

void LaunchHelpFile(const EHelpFilePage hfp=HFP_MainMenu) 
{
	CptString strHelpFile = CptWinPath::GetStartupPath() + _T("\\") + FILE_NAME_APP_HELP;

	if(CptGlobal::IsFileExist2(strHelpFile))
	{
		CptString strWinPath = CptWinPath::GetSpecialPath(CSIDL_WINDOWS) ;

		if(CptGlobal::IsFileExist2(strWinPath+_T("\\hh.exe")))
		{
			CptString strPageName ;

			switch(hfp)
			{
			default:
			case HFP_MainMenu:
				strPageName = HELP_FIILE_PAGE_HOME ;
				break ;

			case HFP_SameFileNameDlg:
				strPageName = HELP_FILE_PAGE_SAME_FILE ;
				break ;

			case HFP_ConfigurationDlg:
				strPageName = HELP_FILE_PAGE_CONFIGURATION ;
				break ;

			case HFP_TaskEditDlg:
				strPageName = HELP_FILE_PAGE_TASK_EDIT ;
				break ;

			case HFP_WildcardDlg:
				strPageName = HELP_FILE_PAGE_WILDCARD ;
				break ;

			case HFP_TaskQueue:
				strPageName = HELP_FILE_PAGE_TASK_QUEUE ;
				break ;
			}

			CptString strParam ;
			strParam.Format(_T("mk:@MSITStore:%s::%s"),strHelpFile,strPageName) ;

			::ShellExecute(NULL,_T("open"),_T("hh"),strParam.c_str(),strWinPath.c_str(),SW_MAXIMIZE);
		}
		else
		{
			::ShellExecute(NULL,_T("open"),strHelpFile.c_str(),NULL,NULL,SW_MAXIMIZE);
		}
	}
	else
	{
		_ASSERT(FALSE) ;
	}
	
	
}

//BOOL __stdcall EnumExplorerWindowsProc(HWND hwnd,LPARAM lParam)
//{
//	TCHAR szWndClassName[128+1] = {0} ;
//
//	::GetClassName(hwnd,szWndClassName,sizeof(szWndClassName)/sizeof(TCHAR)) ;
//	// CabinetWClass
//
//	if(::_tcscmp(szWndClassName,_T("CabinetWClass"))==0)
//	{
//		::SendMessage(hwnd,WM_KEYDOWN,VK_F5,0) ;
//		::SendMessage(hwnd,WM_KEYUP,VK_F5,0) ;
//		//::keybd_event(VK_F5,0,0,0) ;
//	}
//
//	return TRUE ;
//}
//
//void RefreshExplorer()
//{
//	//::EnumWindows(EnumExplorerWindowsProc,0) ;
//	
//}

//bool IsFileExist(LPCTSTR lpFileFuleName)
//{
//	return (::GetFileAttributes(lpFileFuleName)!=INVALID_FILE_ATTRIBUTES) ;
//}