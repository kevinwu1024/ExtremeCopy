/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#include "stdafx.h"
#include "XCShellExt.h"
#include "ShellExtClassFactory.h"
#include "../ExtremeCopy/Common/ptRegistry.h"
#include <shellapi.h>
#include "../ExtremeCopy/Common/ptGlobal.h"
#include "../ExtremeCopy/App/XCCopyTask.h"
#include "../ExtremeCopy/Common/ptWinPath.h"
#include "../ExtremeCopy/App/XCConfiguration.h"

#pragma warning(disable:4996)

extern CSystemObject* g_pSysObj  ;

bool IsVISTAOrLaterVersion()
{
	bool bRet = false ;

	OSVERSIONINFO oi ;

	::memset(&oi,0,sizeof(OSVERSIONINFO)) ;
	oi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO) ;

	if(::GetVersionEx(&oi))
	{
		bRet = oi.dwMajorVersion>=6 ;
	}

	return bRet ;
}

bool GUID2CLSIDString(const GUID& guid,TCHAR* pString,int nSize)
{
	LPOLESTR oleStr = NULL ;

	::StringFromIID(guid, &oleStr);

	if(oleStr!=NULL)
	{
#ifdef UNICODE 
		int nLen = (int)::_tcslen(oleStr)*sizeof(TCHAR) ;

		::memcpy(pString,oleStr,min(nLen,nSize)) ;
#else
		::WideCharToMultiByte(CP_ACP, 0, oleStr, -1, pString, nSize, NULL, NULL);
#endif

		::CoTaskMemFree(oleStr) ;

		return true ;
	}
	return	false;
}

// 判断是否不为卸载的COM
bool IsValidCOM(const GUID& guid)
{
	return true ;
	bool bRet = false ;

	TCHAR szCLSIDBuf[256] = {0} ;
	if(::GUID2CLSIDString(guid,szCLSIDBuf,sizeof(szCLSIDBuf)))
	{
		CptRegistry reg ;

		TCHAR szCLSIDBuf2[256] = _T("CLSID\\") ;
		::_tcscat(szCLSIDBuf2,szCLSIDBuf) ; 
		
		bRet = reg.OpenKey(CptRegistry::CLASSES_ROOT,szCLSIDBuf2) ;

		reg.Close() ;
	}

	return bRet ;
}

void GetCommandStringXML(const pt_STL_vector(CptString)& strSrcFileVer,const CptString& strDstFolder,bool bMoveOrCopy,void** ppBuf,int& nBufSize)
{
	SXCCopyTaskInfo TaskInfo ;

	TaskInfo.SetDefaultValue() ;
	
	TaskInfo.strSrcFileVer = strSrcFileVer ;
	TaskInfo.strDstFolderVer.push_back(strDstFolder) ;
	TaskInfo.CopyType = bMoveOrCopy ? SXCCopyTaskInfo::RT_Move : SXCCopyTaskInfo::RT_Copy ;

	pt_STL_vector(SXCCopyTaskInfo) TaskVer ;

	TaskVer.push_back(TaskInfo) ;

	CXCCopyTask::ConvertTaskInfo2XMLString(TaskVer,(char**)ppBuf,nBufSize) ;
}
//
//void GetCommandString(const std::vector<CptString>& strSrcFileVer,const CptString& strDstFolder,bool bMoveOrCopy,void** ppBuf)
//{
//	*ppBuf = NULL ;
//	//nSize = 0 ;
//
//	//if(this->GetSysObj()->m_ContextMenuFileInfo.m_pDstFolder!=NULL && !this->GetSysObj()->m_ContextMenuFileInfo.m_SrcFilesVer.empty())
//	{
//		/* 命令格式
//	command: xcopy/xcmove
//	source size: 3
//	c:\abc.txt
//	d:\my_folder
//	e:\files.cpp
//	destination size: 1
//	f:\target_folder
//
//	*/
//
//		int nWholeBufSize = 0 ;
//		for(unsigned int i=0;i<strSrcFileVer.size();++i)
//		{
//			nWholeBufSize += strSrcFileVer[i].GetLength() ;
//		}
//
//		nWholeBufSize += strDstFolder.GetLength() + 1024 ;
//
//		nWholeBufSize *= sizeof(TCHAR) ;
//
//		//int nWholeBufSize = 4*1024*1024*sizeof(TCHAR) ;
//		*ppBuf = ::malloc(nWholeBufSize);//new TCHAR[nWholeBufSize] ;
//
//		if(*ppBuf!=NULL)
//		{
//			::memset(*ppBuf,0,nWholeBufSize) ;
//
//			DWORD dwWritten = 0 ;
//			TCHAR DataBuf[MAX_PATH_EX] = {0} ;
//			TCHAR* pWrite = (TCHAR*)*ppBuf ;
//
//			::_stprintf(pWrite,_T("command: %s\r\n"),bMoveOrCopy?_T("xcmove"):_T("xccopy")) ;
//			pWrite += ::_tcslen(pWrite) ;
//			//::WriteFile(hPipe,DataBuf,::_tcslen(DataBuf)*sizeof(TCHAR),&dwWritten,NULL);
//
//			::_stprintf(pWrite,_T("source size: %d\r\n"),strSrcFileVer.size()) ;
//			pWrite += ::_tcslen(pWrite) ;
//			//::WriteFile(hPipe,DataBuf,::_tcslen(DataBuf)*sizeof(TCHAR),&dwWritten,NULL);
//
//			for(int i=0;i<(int)strSrcFileVer.size();++i)
//			{
//				if(pWrite-(TCHAR*)(*ppBuf)>nWholeBufSize-256)
//				{
//					void* p = ::realloc(*ppBuf,2*nWholeBufSize) ;
//
//					if(p!=NULL)
//					{
//						*ppBuf = p ;
//						nWholeBufSize = 2*nWholeBufSize ;
//					}
//					else
//					{
//						::free(*ppBuf) ;
//						*ppBuf = NULL ;
//
//						return ;
//					}
//				}
//
//				::_stprintf(pWrite,_T("%s\r\n"),strSrcFileVer[i]) ;
//				pWrite += ::_tcslen(pWrite) ;
//
//				//WriteToFile(this->GetSysObj()->m_ContextMenuFileInfo.m_SrcFilesVer[i],::_tcslen(this->GetSysObj()->m_ContextMenuFileInfo.m_SrcFilesVer[i])*sizeof(TCHAR)) ;
//				//::WriteFile(hPipe,DataBuf,::_tcslen(DataBuf)*sizeof(TCHAR),&dwWritten,NULL);
//			}
//
//			if(pWrite-(TCHAR*)(*ppBuf)>nWholeBufSize-256)
//			{
//				void* p = ::realloc(*ppBuf,2*nWholeBufSize) ;
//
//				if(p!=NULL)
//				{
//					*ppBuf = p ;
//					nWholeBufSize = 2*nWholeBufSize ;
//				}
//				else
//				{
//					::free(*ppBuf) ;
//					*ppBuf = NULL ;
//
//					return ;
//				}
//			}
//			::_stprintf(pWrite,_T("destination size: %d\r\n"),1) ;
//			pWrite += ::_tcslen(pWrite) ;
//			//::WriteFile(hPipe,DataBuf,::_tcslen(DataBuf)*sizeof(TCHAR),&dwWritten,NULL);
//
//			if(pWrite-(TCHAR*)(*ppBuf)>nWholeBufSize-256)
//			{
//				void* p = ::realloc(*ppBuf,nWholeBufSize+MAX_PATH) ;
//
//				if(p!=NULL)
//				{
//					*ppBuf = p ;
//					nWholeBufSize = nWholeBufSize+MAX_PATH ;
//				}
//				else
//				{
//					::free(*ppBuf) ;
//					*ppBuf = NULL ;
//
//					return ;
//				}
//			}
//			// 注意这里是2个 回车换行
//			::_stprintf(pWrite,_T("%s\r\n\r\n"),strDstFolder) ;
//			pWrite += ::_tcslen(pWrite) ;
//			//::WriteFile(hPipe,DataBuf,::_tcslen(DataBuf)*sizeof(TCHAR),&dwWritten,NULL);
//		}
//		
//	}
//}
//

bool IsCut(IDataObject* pDataObj)
{
	return (::GetOperateType(pDataObj)==OT_Move) ;
}

// 获取‘拖放’的操作类型
EOperateType GetOperateType(IDataObject* pDataObj)
{
	EOperateType nRet = OT_Unknown ;

	if(pDataObj!=NULL)
	{
		CLIPFORMAT nFormat = ::RegisterClipboardFormat(	CFSTR_PREFERREDDROPEFFECT) ;
		//CLIPFORMAT nFormat = ::RegisterClipboardFormat(	CFSTR_LOGICALPERFORMEDDROPEFFECT) ;

		STGMEDIUM	medium2;
		FORMATETC	fe2 = { nFormat, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 

		HRESULT hr9 = pDataObj->GetData(&fe2, &medium2) ;

		if (SUCCEEDED( hr9)) 
		{
			//int nSize = (int)::GlobalSize(medium2.hGlobal) ;
			
			void* pData = ::GlobalLock(medium2.hGlobal);

			if(pData!=NULL)
			{
				DWORD dwCopyOrMove = *(DWORD*)pData ;

				::GlobalUnlock(medium2.hGlobal) ;
				::ReleaseStgMedium(&medium2) ;

				if(dwCopyOrMove!=DROPEFFECT_NONE )
				{
					
					
					switch(dwCopyOrMove)
					{
					case DROPEFFECT_MOVE: nRet = OT_Move ; break ;
					case DROPEFFECT_COPY: nRet = OT_Copy ; break ;
					case DROPEFFECT_LINK: nRet = OT_Link ; break ;
					case 5: nRet = OT_CopyLink ; break ;
					}
					
					//nRet = (dwCopyOrMove & DROPEFFECT_MOVE) ? OT_Move : ((dwCopyOrMove & DROPEFFECT_LINK) ? OT_Link : OT_Copy) ;
				}
			}


			/**
			switch(dwCopyOrMove)
			{
			case DROPEFFECT_MOVE: 
				bRet = true ;
				Debug_Printf("::IsCut() move") ;
				break ;

			case DROPEFFECT_COPY:
				Debug_Printf("::IsCut() copy") ;
				break ;

			default:
				Debug_Printf("::IsCut() unknow operate=%d size=%d",dwCopyOrMove,nSize) ;
				break ;
			}
			/**/

			
		}
	}

	return nRet ;
}

bool CreateShortcut(CSystemObject* pSysObj,const pt_STL_vector(CptString)& strSrcFileVer,const CptString& strDstFolder,HWND hWnd)
{// 
	CptWinPath::SPathElementInfo pei ;

	pei.uFlag = CptWinPath::PET_FileName ;
	CptString strShortcut ;

	bool bAddSlash = (strDstFolder.GetAt(strDstFolder.GetLength()-1)!='\\'
		|| strDstFolder.GetAt(strDstFolder.GetLength()-1)!='/') ;

	for(size_t i=0;i<strSrcFileVer.size();++i)
	{
		if(CptWinPath::GetPathElement(strSrcFileVer[i].c_str(),pei))
		{
			if(bAddSlash)
			{
				strShortcut.Format(_T("%s\\%s.lnk"),strDstFolder,pei.strFileName) ;
			}
			else
			{
				strShortcut.Format(_T("%s%s.lnk"),strDstFolder,pei.strFileName) ;
			}
			
			if(!CptGlobal::CreateShortcutLink(strSrcFileVer[i].c_str(),strShortcut.c_str()))
			{
				//return false ;
			}
		}
		else
		{
			_ASSERT(FALSE) ;
		}		
	}

	return true ;
}

bool LaunchTaskEditor( CSystemObject* pSysObj,const pt_STL_vector(CptString)& strSrcFileVer2)
{
	bool bRet = false ;

	TCHAR* szDLLBuf = new TCHAR[MAX_PATH_EX] ;

	if(szDLLBuf!=NULL)
	{
		::memset(szDLLBuf,0,MAX_PATH_EX*sizeof(TCHAR)) ;

		if(pSysObj!=NULL && pSysObj->GetDllFullName(szDLLBuf,MAX_PATH_EX*sizeof(TCHAR)))
		{
			TCHAR* pSlash = ::_tcsrchr(szDLLBuf,'\\') ;

			if(pSlash==NULL)
			{
				pSlash = ::_tcsrchr(szDLLBuf,'/') ; 
			}

			if(pSlash!=NULL)
			{
				pSlash[0] = 0 ;

				SXCCopyTaskInfo TaskInfo ;

				TaskInfo.SetDefaultValue() ; // 设置默认值
				TaskInfo.strSrcFileVer.assign(strSrcFileVer2.begin(),strSrcFileVer2.end()) ;
				TaskInfo.CopyType = SXCCopyTaskInfo::RT_Copy ;
				TaskInfo.CreatedBy = SXCCopyTaskInfo::TCT_ShellExtension ; // 由shell 创建的任务

				// 如果是shell 的话，rich copy selection 应从ini文件里读取
				CXCConfiguration::GetInstance()->LoadConfigShareFromFile(TaskInfo.ConfigShare,g_pSysObj->GetModuleHandle()) ;

				// 先创建一份 临时的 .xct 任务文件
				pt_STL_vector(SXCCopyTaskInfo) TaskVer ;
				TaskVer.push_back(TaskInfo) ;

				//CXCCopyTask::ConvertTaskInfo2XMLString(TaskVer,(char**)&pUTF8XML,nContentSize) ;

				//if(pUTF8XML!=NULL)
				{
					TCHAR szTempPath[MAX_PATH] = {0} ;

					::GetTempPath(MAX_PATH,szTempPath) ;

					CptString strPrefix ;

					GUID guid ;
					if(CptGlobal::CreateGUID(guid))
					{
						strPrefix.Format(_T("%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X")
							, guid.Data1
							, guid.Data2
							, guid.Data3
							, guid.Data4[0], guid.Data4[1]
						, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
						, guid.Data4[6], guid.Data4[7]);
					}
					else
					{
						strPrefix = _T("AAFD9C84-1507-45BB-B613-3907A54AE873") ;
					}

					CptString strTemXCT ;

					int nLen = _tcslen(szTempPath) ;

					if(szTempPath[nLen-1]!='\\' || szTempPath[nLen-1]!='/')
					{
						strTemXCT.Format(_T("%s%s.xct.tmp"),szTempPath,strPrefix.c_str()) ;
					}
					else
					{
						strTemXCT.Format(_T("%s\\%s.xct.tmp"),szTempPath,strPrefix.c_str()) ;
					}

					if(CXCCopyTask::ConvertTaskInfo2XMLFile(TaskVer,strTemXCT))
					{
						CptString strXCExeFileFolder = szDLLBuf ;

						const TCHAR cLastSlash = strXCExeFileFolder.GetAt(strXCExeFileFolder.GetLength()-1) ;

						if(cLastSlash!='\\' || cLastSlash!='/')
						{
							strXCExeFileFolder += '\\' ;
						}

						CptString strParam ;

						strParam.Format(_T("-task_dlg \"%s\""),strTemXCT) ;

						::ShellExecute(NULL,_T("open"),_T("ExtremeCopy.exe"),strParam.c_str(),strXCExeFileFolder.c_str(),SW_SHOW) ;

						::Sleep(500) ;
						::DeleteFile(strTemXCT.c_str()) ;
					}
				}
			}
		}

		delete [] szDLLBuf ;
		szDLLBuf = NULL ;
	}

	return bRet ;
}


bool LaunchExtremeCopy(CSystemObject* pSysObj,
						const pt_STL_vector(CptString)& strSrcFileVer,
						const CptString& strDstFolder,
					   bool bMoveOrCopy,HWND hWnd)
{
	bool bRet = false ;

	TCHAR* szDLLBuf = new TCHAR[MAX_PATH_EX] ;

	if(szDLLBuf!=NULL)
	{
		::memset(szDLLBuf,0,MAX_PATH_EX*sizeof(TCHAR)) ;

		if(pSysObj!=NULL && pSysObj->GetDllFullName(szDLLBuf,MAX_PATH_EX*sizeof(TCHAR)))
		{
			TCHAR* pSlash = ::_tcsrchr(szDLLBuf,'\\') ;

			if(pSlash==NULL)
			{
				pSlash = ::_tcsrchr(szDLLBuf,'/') ; 
			}

			if(pSlash!=NULL)
			{
				pSlash[0] = 0 ;

				SXCCopyTaskInfo TaskInfo ;

				TaskInfo.SetDefaultValue() ; // 设置默认值

				TaskInfo.strSrcFileVer.assign(strSrcFileVer.begin(),strSrcFileVer.end()) ;
				TaskInfo.strDstFolderVer.push_back(strDstFolder) ;
				//TaskInfo.strDstFile = strDstFolder ;
				TaskInfo.CopyType = bMoveOrCopy ? SXCCopyTaskInfo::RT_Move : SXCCopyTaskInfo::RT_Copy ;
				TaskInfo.CreatedBy = SXCCopyTaskInfo::TCT_ShellExtension ; // 由shell 创建的任务

				// 如果是shell 的话，rich copy selection 应从ini文件里读取
				CXCConfiguration::GetInstance()->LoadConfigShareFromFile(TaskInfo.ConfigShare,g_pSysObj->GetModuleHandle()) ;

				// 启动 ExtremeCopy 后不等待，但sleep()一会儿
				bRet = CXCCopyTask::LaunchXCTaskInstanceViaNamePipe( TaskInfo,szDLLBuf,hWnd,2*1000) ;

			}
		}

		delete [] szDLLBuf ;
		szDLLBuf = NULL ;
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

	case TXMLET_UnknownError:
		strRet = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TASKXMLERROR_INVALIDFORMAT) ;
		break ;

	case TXMLET_ThereAreNotSrcOrDstFileOrFolder:
		strRet = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TASKXMLERROR_NOSRCORDSTFILEORFOLDER) ;
		break ;
	}

	return strRet ;
}

bool ExecuteTaskFile(CSystemObject* pSysObj,CptString strTaskFile)
{
	bool bRet = false ;

	TCHAR* szDLLBuf = new TCHAR[MAX_PATH_EX] ;
	::memset(szDLLBuf,0,MAX_PATH_EX*sizeof(TCHAR)) ;

	if(pSysObj!=NULL && pSysObj->GetDllFullName(szDLLBuf,MAX_PATH_EX*sizeof(TCHAR)))
	{
		TCHAR* pSlash = ::_tcsrchr(szDLLBuf,'\\') ;

		if(pSlash==NULL)
		{
			pSlash = ::_tcsrchr(szDLLBuf,'/') ; 
		}

		if(pSlash!=NULL)
		{
			pSlash[0] = 0 ;

			ETaskXMLErrorType result = CXCCopyTask::ExecuteTaskFile(strTaskFile,szDLLBuf) ;

			if(result!=TXMLET_Success)
			{
				CptString strText = GetTaskXMLErrorString(result) ;
				CptString strTitle = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_APP_PRO) ;

				::MessageBox(NULL,strText.c_str(),strTitle.c_str(),MB_OK) ;
			}
		}
	}

	if(szDLLBuf!=NULL)
	{
		delete [] szDLLBuf ;
		szDLLBuf = NULL ;
	}

	return bRet ;
}

bool IsReplaceExplorerCopying()
{
	SConfigData config ;
	CXCConfiguration::GetInstance()->LoadConfigDataFromFile(config,g_pSysObj->GetModuleHandle()) ;

	return config.bDefaultCopying ;
}

CptString GetResourceDLL(HMODULE hModule) 
{
	SConfigData config ;
	CXCConfiguration::GetInstance()->LoadConfigDataFromFile(config,g_pSysObj->GetModuleHandle()) ;

	return config.strResourceDLL ;
}

bool IsExtremeCopyExeExist()
{
	TCHAR szDLLBuf[MAX_PATH_EX] = {0} ;
	
	if(::g_pSysObj->GetDllFullName(szDLLBuf,sizeof(szDLLBuf)))
	{
		TCHAR szEXEBuf[MAX_PATH_EX] = {0} ;
		TCHAR* pFileName =  NULL ;
		::GetFullPathName(szDLLBuf,sizeof(szEXEBuf)/sizeof(TCHAR),szEXEBuf,&pFileName) ;

		if(pFileName!=NULL)
		{
			::_tcscpy(pFileName,_T("ExtremeCopy.exe")) ;
		}

		return IsFileExist(szEXEBuf) ;
	}

	return false ;
}

// 判断是否需要提升 特权 级别
bool IsNeedElevatePrivilege(const pt_STL_vector(CptString)& strSrcFileVer,const CptString& strDstFolder,bool bMoveOrCopy)
{
	bool bRet = false ;

	if(CptGlobal::IsVISTAOrLaterVersion() && CptGlobal::IsEnableUAC())
	{

		//if(strDstFolder.GetAt(1)==':')
		//{
		//}
		
		//  TCHAR     lpVolumeNameBuffer[1024];     //硬盘卷标名称  
  //DWORD   dwVolumeSerialNumber;             //序列号  
  //DWORD   dwMaximumComponentLength;     //文件名最大长度  
  //DWORD   FileSystemFlags;                       //文件系统标志  
  //TCHAR     lpFileSystemNameBuffer[1024];//文件系统名称  
  // 
  //CString   strDisk;  
  //strDisk.Format("%c:\\\\",diskChar);  
  // 
  //if(!GetVolumeInformation(strDisk,                
  //                lpVolumeNameBuffer,1024,    
  //&dwVolumeSerialNumber,  
  //&dwMaximumComponentLength,  
  //&FileSystemFlags,  
  //lpFileSystemNameBuffer,1024))   

		bRet = true ;
	}

	return bRet ;
}

//bool IsEnableUAC()
//{
//	static bool once = FALSE;
//	static bool ret = FALSE;
//
//	if (!once) 
//	{
//		if (g_pSysObj->IsVISTAOrLasterVersion()) 
//		{
//			CptRegistry reg ;
//			ret = true;
//			if(reg.OpenKey(CptRegistry::LOCAL_MACHINE,_T("Microsoft\\Windows\\CurrentVersion\\Policies\\System")))
//			{
//				DWORD val = 1;
//				ret = !(reg.GetValueInt(_T("EnableLUA"), val) && val == 0) ;
//			}
//		}
//		once = true;
//	}
//
//	return	ret;
//}