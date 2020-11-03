/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCCoreDefine.h"


pt_STL_list(SDataPack_SourceFileInfo*) SDataPack_SourceFileInfo::CacheList ;
CptCritiSecLock			SDataPack_SourceFileInfo::m_ThreadLock ;

CptString GetRawFileName(CptString strFullFileName) 
{
	CptWinPath::SPathElementInfo pei ;

	pei.uFlag = CptWinPath::PET_FileName ;

	if(CptWinPath::GetPathElement(strFullFileName.c_str(),pei))
	{
		return pei.strFileName ;
	}

	return _T("") ;
}

bool IsContainWildcardChar(const TCHAR* pStr)
{
	_ASSERT(pStr!=NULL) ;

	bool bRet = false ;

	int nCurIndex = 0 ;

	while(pStr[nCurIndex]!=NULL)
	{
		if(pStr[nCurIndex]=='*' || pStr[nCurIndex]=='?')
		{
			bRet = true ;
			break ;
		}

		++nCurIndex ;
	}

	return bRet ;
}

void RenameNewFileOrFolder(CptString& strFileOrFolder)
{
	CptString strNewFileFormat = strFileOrFolder ; 

	CptString strNewFile = strFileOrFolder ;

	CptString strCopyOf = _T("Copy Of ") ;//::CptMultipleLanguage::GetInstance()->GetString(IDS_FORMAT_COPYOF) ;

	int nIndex = strNewFileFormat.ReverseFind(_T('\\'))+1 ;
	strNewFileFormat.Insert(nIndex,strCopyOf+_T("(%d)")) ;
	strNewFile.Insert(nIndex,strCopyOf.c_str()) ;

	bool bFound = true ;

	int i = 0 ;

	do
	{
		if(i>0)
		{
			strNewFile.Format(strNewFileFormat.c_str(),i) ;
		}

		++i ;
	}
	while(IsFileExist(strNewFile.c_str())) ;

	strFileOrFolder = strNewFile ;
}

//bool IsFolder(const TCHAR* lpDir)
//{
//	_ASSERT(lpDir!=NULL) ;
//	DWORD dwAttr = ::GetFileAttributes(lpDir) ;
//
//	return (dwAttr!=INVALID_FILE_ATTRIBUTES && dwAttr&FILE_ATTRIBUTE_DIRECTORY) ;
//}


EInterestFileType GetInterestFileType(const TCHAR* pFileName) 
{
	EInterestFileType iftRet = IFT_Invalid ;

	const DWORD dwAttr = ::GetFileAttributes(pFileName) ;

	//CptString str = pFileName ;
	//Debug_Printf(_T("%d  %s %d"),dwAttr,pFileName,str.GetLength()) ;
	//int aa = ::GetLastError() ;

	if(dwAttr!=INVALID_FILE_ATTRIBUTES)
	{
		iftRet = (dwAttr&FILE_ATTRIBUTE_DIRECTORY) ? IFT_Folder : IFT_File ;
	}
	else
	{
		int nLastSlashIndex = -1 ;
		int nCurIndex = 0 ;

		while(pFileName[nCurIndex]!=NULL)
		{
			if(pFileName[nCurIndex]=='\\' || pFileName[nCurIndex]=='/')
			{
				nLastSlashIndex = nCurIndex ;
			}
			++nCurIndex ;
		}

		if(nLastSlashIndex>0)
		{
			TCHAR szBuf[MAX_PATH] = {0} ;
			::memcpy(szBuf,pFileName,(nLastSlashIndex+1)*sizeof(TCHAR)) ;

			if(::GetFileAttributes(szBuf)&FILE_ATTRIBUTE_DIRECTORY)
			{
				if(IsContainWildcardChar(pFileName+nLastSlashIndex+1))
				{
					iftRet = IFT_FolderWithWildcard ;
				}
			}
		}
	}

	return iftRet ;
}