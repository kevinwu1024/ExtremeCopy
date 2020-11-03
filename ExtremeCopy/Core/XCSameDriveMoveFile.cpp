/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCSameDriveMoveFile.h"
#include "XCWinStorageRelative.h"
#include "XCCopyingEvent.h"
#include <shlobj.h>
#include "XCCoreDefine.h"
#include "../App/XCGlobal.h"

CXCSameDriveMoveFile::CXCSameDriveMoveFile(ECopyFileState* pRunState,EImpactFileBehaviorResult* pBehaviorResult)
	:m_pRunningState(pRunState),m_pImpactFileBehavior(pBehaviorResult)
{
	_ASSERT(NULL!=m_pRunningState) ;
	_ASSERT(NULL!=pBehaviorResult) ;
}


CXCSameDriveMoveFile::~CXCSameDriveMoveFile(void)
{
}

bool CXCSameDriveMoveFile::Run(const SGraphTaskDesc& gtd) 
{
	_ASSERT(gtd.SrcFileVer.size()>0) ;
	_ASSERT(gtd.DstFolderVer.size()==1) ;

	bool bRet = false ;
	bool bExe = true ;
	bool bFolder = false ;

	if(gtd.bIsRenameDst)
	{// 改名式的移动
ERROR_RETRY_RENAMEMOVE:
		_ASSERT(gtd.SrcFileVer.size()==1) ;

		CptString strDstFile = gtd.DstFolderVer[0] ;

		switch(this->CheckAndUpdateImpactFileName(gtd,gtd.SrcFileVer[0],strDstFile))
		{
		case ErrorHandlingFlag_Exit: return false ;
		case ErrorHandlingFlag_Ignore: return true ;
		default: break ;
		}

		BOOL bResult = ::MoveFileEx(gtd.SrcFileVer[0].c_str(),strDstFile.c_str(),MOVEFILE_REPLACE_EXISTING) ;

		if(!bResult && gtd.pCopyingEvent)
		{
			SXCExceptionInfo ei ;

			ei.ErrorCode.nSystemError = ::GetLastError() ;
			ei.strSrcFile = gtd.SrcFileVer[0] ;
			ei.strDstFile = gtd.DstFolderVer[0] ;
			ei.SupportType = ErrorHandlingFlag_RetryCancel ;

			switch(gtd.pCopyingEvent->XCOperation_CopyExcetption(ei))
			{
			default:
			case ErrorHandlingFlag_Exit:
				break ;

			case ErrorHandlingFlag_Retry:
				goto ERROR_RETRY_RENAMEMOVE ;
				break ;
			}
		}
		else
		{// 如果移动成功，则刷新源目录
			if(CptGlobal::IsFolder(strDstFile.c_str()))
			{
				::SHChangeNotify(SHCNE_ALLEVENTS,SHCNF_PATH|SHCNF_FLUSH,strDstFile.c_str(),NULL) ;
			}
			else
			{
				CptWinPath::SPathElementInfo pei ;

				pei.uFlag = CptWinPath::PET_Path ;

				if(CptWinPath::GetPathElement(strDstFile,pei))
				{
					::SHChangeNotify(SHCNE_ALLEVENTS,SHCNF_PATH|SHCNF_FLUSH,pei.strPath.c_str(),NULL) ;
				}
			}
		}
	}
	else
	{
		CptString strDstFile ;

		for(size_t i=0;i<gtd.SrcFileVer.size();++i)
		{
			bFolder = CptGlobal::IsFolder(gtd.SrcFileVer[i]) ;

			CptString strOriginFile ;

			if(::IsRecycleFile(gtd.SrcFileVer[i].c_str(),strOriginFile))
			{// 如果是从“回收站”进行移动的话，则需要进行改名
				_ASSERT(strOriginFile.GetLength()>0) ;

				strDstFile.Format(_T("%s\\%s"),gtd.DstFolderVer[0],strOriginFile) ;

				if(!bFolder)
				{
					switch(this->CheckAndUpdateImpactFileName(gtd,gtd.SrcFileVer[i],strDstFile))
					{
					case ErrorHandlingFlag_Exit: return false ;
					case ErrorHandlingFlag_Ignore: continue ;
					default: break ;
					}
				}
			}
			else
			{
				if(!bFolder)
				{// 如果要移动的是文件

					if(CptGlobal::IsFolder(gtd.DstFolderVer[0]))
					{
						CptWinPath::SPathElementInfo pei ;

						pei.uFlag = CptWinPath::PET_FileName ;

						bool bResult = CptWinPath::GetPathElement(gtd.SrcFileVer[i].c_str(),pei) ;

						_ASSERT(bResult?TRUE:FALSE) ;

						strDstFile.Format(_T("%s\\%s"),gtd.DstFolderVer[0],pei.strFileName) ;
					}
					else
					{
						strDstFile = gtd.DstFolderVer[0] ;// 改名
					}
					switch(this->CheckAndUpdateImpactFileName(gtd,gtd.SrcFileVer[i],strDstFile))
					{
					case ErrorHandlingFlag_Exit: return false ;
					case ErrorHandlingFlag_Ignore: continue ;
					default: break ;
					}
				}
				else
				{// 如果移动的是文件夹
					_ASSERT(CptGlobal::IsFolder(gtd.DstFolderVer[0])) ;

					CptWinPath::SPathElementInfo pei ;

					pei.uFlag = CptWinPath::PET_FileName ;

					bool bResult = CptWinPath::GetPathElement(gtd.SrcFileVer[i].c_str(),pei) ;

					_ASSERT(bResult?TRUE:FALSE) ;

					CptString strDst22 = gtd.DstFolderVer[0] ;
					strDst22.TrimRight('\\') ;
					strDst22.TrimRight('/') ;

					strDstFile.Format(_T("%s\\%s"),strDst22,pei.strFileName) ;

					// 如果是从子目录 move 到同名的父目录里，则只能把源目录里的逐个逐个文件或文件夹移出来
					if(CptGlobal::IsFolder(strDstFile.c_str()))
					{// 如果目标文件夹已存在
						bRet = true ;
						if (this->MoveItems(gtd,gtd.SrcFileVer[i],gtd.DstFolderVer[0])==ErrorHandlingFlag_Exit) 
						{
							return false ;
						}

						continue ;
					}
				}
			}
			

			if(this->MoveFile(gtd,gtd.SrcFileVer[i],strDstFile,bFolder)==ErrorHandlingFlag_Exit)
			{
				return false ;
			}

		}
	}

	return bRet ;
}

// 从子目录移动其所有的文件和文件夹到其同名的父目录里
ErrorHandlingResult CXCSameDriveMoveFile::MoveItems(const SGraphTaskDesc& gtd,CptString strSubDir,CptString strParentDir)
{
	ErrorHandlingResult bRet = ErrorHandlingFlag_Success ;

	{
		CptString strDstFile ;
		CptWinPath::SPathElementInfo pei ;

		pei.uFlag = CptWinPath::PET_FileName ;

		bool bResult = CptWinPath::GetPathElement(strSubDir.c_str(),pei) ;

		_ASSERT(bResult?TRUE:FALSE) ;

		CptString strDst22 = strParentDir ;
		strDst22.TrimRight('\\') ;
		strDst22.TrimRight('/') ;

		strDstFile.Format(_T("%s\\%s"),strDst22,pei.strFileName) ;

		::CreateDirectoryEx(strSubDir.c_str(),strDstFile.c_str(),NULL) ;

		if(pei.strFileName.CompareNoCase(_T("qt4"))==0)
		{
			int aa = 0 ;
		}
	}
	
	SGraphTaskDesc gtd2 ;
	WIN32_FIND_DATA wfd2 ;

	CptString strSrcFile2 = strSubDir + _T("\\*.*") ;

	HANDLE hFind = ::FindFirstFile(strSrcFile2.c_str(),&wfd2) ;

	_ASSERT(INVALID_HANDLE_VALUE!=hFind) ;

	if(INVALID_HANDLE_VALUE!=hFind)
	{
		//bRet = true ;
		bool bDotFound1 = false ;
		bool bDotFound2 = false ;
		CptString strTemSrcFile ;
		CptString strTemDstFile ;
		CptString strDstFile ;
		bool bSameNameFolder = false ;
		CptWinPath::SPathElementInfo pei ;

		pei.uFlag = CptWinPath::PET_FileName ;

		bool bResult = CptWinPath::GetPathElement(strSubDir.c_str(),pei) ;
		DWORD dwAttr = 0 ;

		_ASSERT(bResult?TRUE:FALSE) ;

		CptString strParent = strParentDir ;
		strParent.TrimRight('\\') ;
		strParent.TrimRight('/') ;

		strDstFile.Format(_T("%s\\%s"),strParent,pei.strFileName) ;

		do
		{
			if(!bDotFound1 && ::_tcscmp(wfd2.cFileName,_T("."))==0)
			{
				bDotFound1 = true ;
				continue ;
			}
			else if(!bDotFound2 && ::_tcscmp(wfd2.cFileName,_T(".."))==0)
			{
				bDotFound2 = true ;
				continue ;
			}
			else
			{
				bSameNameFolder = false ;

				strTemSrcFile.Format(_T("%s\\%s"),strSubDir,wfd2.cFileName) ;
				strTemDstFile.Format(_T("%s\\%s"),strDstFile,wfd2.cFileName) ;

				dwAttr = ::GetFileAttributes(strTemDstFile.c_str()) ;

				if(dwAttr!=INVALID_FILE_ATTRIBUTES)
				{
					if(CptGlobal::IsFolder(dwAttr))
					{// 若为目录
						bSameNameFolder = true ; // 该目录不用下面的 MoveFile来处理
					}
					else
					{// 如果是文件
						switch(this->CheckAndUpdateImpactFileName(gtd,strTemSrcFile,strTemDstFile))
						{
						case ErrorHandlingFlag_Exit: 
							{
								bRet = ErrorHandlingFlag_Exit ;
								goto MOVE_ITEMS_END ;
							}

						case ErrorHandlingFlag_Ignore: continue ;//goto ERROR_IGNORE_CONTINUE ; break ;
						default: break ;
						}
					}
				}
				else
				{// 文件或文件夹不存在
				}

				if(bSameNameFolder || CptGlobal::IsFolder(wfd2.dwFileAttributes))
				{
					bRet = this->MoveItems(gtd,strTemSrcFile,strDstFile) ;
				}
				else
				{
					bRet = this->MoveFile(gtd,strTemSrcFile,strTemDstFile,false) ;
				}

			}
		}
		while( ErrorHandlingFlag_Exit!=bRet && ::FindNextFile(hFind,&wfd2)) ;

MOVE_ITEMS_END:
		::FindClose(hFind) ;
		hFind = INVALID_HANDLE_VALUE ;

		if(ErrorHandlingFlag_Success==bRet)
		{
			::RemoveDirectory(strSubDir.c_str());
		}
	}

	return bRet ;
}

int CXCSameDriveMoveFile::CheckAndUpdateImpactFileName(const SGraphTaskDesc& gtd,CptString strSrcFile,CptString& strDstFile) 
{
	int nRet = 0 ;

	if(IsFileExist(strDstFile.c_str()))
	{
		if(*m_pImpactFileBehavior==SFDB_Replace)
		{
			//CptGlobal::ForceDeleteFile(strDstFile.c_str()) ;
			return nRet ;
		}

		SImpactFileResult result ;

		if(gtd.pCopyingEvent!=NULL)
		{
			if(*m_pImpactFileBehavior==SFDB_Default || *m_pImpactFileBehavior==SFDB_Rename)
			{// 等于默认（改名）才向上回调
				SImpactFileInfo ImpactInfo ;
				ImpactInfo.ImpactType = IT_SameFileName ;

				ImpactInfo.strSrcFile = strSrcFile ;
				ImpactInfo.strDestFile = strDstFile ;

				gtd.pCopyingEvent->XCOperation_ImpactFile(ImpactInfo,result) ;
			}
			else
			{
				result.result = *m_pImpactFileBehavior ;
			}
		}

		if(result.bAlways)
		{
			*m_pImpactFileBehavior = result.result ;
		}

		switch(result.result)
		{
		case SFDB_Skip: // 跳过
			return ErrorHandlingFlag_Ignore ;

		case SFDB_Default:
		case SFDB_Rename: // 改名
			strDstFile = result.strNewDstFileName ;//this->RenameNewFileOrFolder(strDestFile) ;
			break ;

		case SFDB_Replace: // 覆盖
			//CptGlobal::ForceDeleteFile(strDstFile.c_str()) ;
			break ;

		default:
		case SFDB_StopCopy: // 退出
			*m_pRunningState = CFS_ReadyStop ;
			return ErrorHandlingFlag_Exit ;
		}
	}

	return nRet ;
}

ErrorHandlingResult CXCSameDriveMoveFile::MoveFile(const SGraphTaskDesc& gtd,CptString strSrcFile,CptString strDstFolder,bool bFolder)
{
	ErrorHandlingResult ret = ErrorHandlingFlag_Success ;

ERROR_RETRY_RENAMEMOVE99:
	DWORD dwFlag = (bFolder ? 0 :MOVEFILE_REPLACE_EXISTING) ;

	bool bRet = (::MoveFileEx(strSrcFile.c_str(),strDstFolder.c_str(),dwFlag)==TRUE) ;
	//bool bRet = (::MoveFile(strSrcFile.c_str(),strDstFolder.c_str())==TRUE) ;

	if(!bRet && gtd.pCopyingEvent)
	{
		if(bFolder && ::GetLastError()==5)
		{
			CptWinPath::SPathElementInfo pei ;

			pei.uFlag = CptWinPath::PET_Path ;

			bool bResult = CptWinPath::GetPathElement(strDstFolder.c_str(),pei) ;

			ret = this->MoveItems(gtd,strSrcFile,pei.strPath) ;
		}
		else
		{
			SXCExceptionInfo ei ;

			ei.ErrorCode.nSystemError = ::GetLastError() ;
			ei.strSrcFile = strSrcFile ;
			ei.strDstFile = strDstFolder ;
			ei.SupportType = ErrorHandlingFlag_RetryIgnoreCancel ;

			switch(gtd.pCopyingEvent->XCOperation_CopyExcetption(ei))
			{
			case ErrorHandlingFlag_Ignore:
				break ;

			default:
			case ErrorHandlingFlag_Exit:
				ret = ErrorHandlingFlag_Exit ;
				break ;

			case ErrorHandlingFlag_Retry:
				goto ERROR_RETRY_RENAMEMOVE99 ;
				break ;
			}
		}
		
	}

	return ret ;
}

// 判断目录关系：是否源目录的父目录是目标目录的子目录，且源目录与该父目录同名
bool CXCSameDriveMoveFile::IsSameNameAndSubdir(CptString strSrcDir,CptString strDstDir) 
{
	bool bRet = false ;

	if(CptGlobal::IsFolder(strSrcDir.c_str()) && CptGlobal::IsFolder(strDstDir.c_str()))
	{
		bRet = true ;
	}
	//else if(strDstDir.GetLength()<strSrcDir.GetLength() && CptGlobal::IsFolder(strDstDir.c_str()) && CptGlobal::IsFolder(strSrcDir.c_str()))
	//{
	//	//CptString sss = strSrcDir.Left(strDstDir.GetLength()-1) ;
	//	if(strSrcDir.Left(strDstDir.GetLength()).CompareNoCase(strDstDir)==0)
	//	{
	//		bRet = true ;
	//	}
	//}

	return bRet ;
}