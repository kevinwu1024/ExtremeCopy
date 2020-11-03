/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptWinPath.h"


CptWinPath::CptWinPath(void)
{
}

CptWinPath::~CptWinPath(void)
{
}

// 结果不包含后面的斜杠
CptString CptWinPath::GetSpecialPath(int nFolder)
{
	// nFolder can be
	// CSIDL_APPDATA
	// CSIDL_COMMON_APPDATA
	// CSIDL_DESKTOP
	// CSIDL_STARTMENU
	// CSIDL_SYSTEM
	// CSIDL_WINDOWS

	CptString strRet ;
	TCHAR szPath[MAX_PATH ] = {0} ;

	if(::SHGetSpecialFolderPath(NULL,szPath,nFolder,FALSE))
	{
		strRet = szPath ;
	}

	return strRet ;
}

// 结果不包含后面的斜杠
CptString CptWinPath::GetStartupPath() 
{
	TCHAR szFileName[MAX_PATH] = {0} ;

	::GetModuleFileName(NULL,szFileName,sizeof(szFileName)/sizeof(TCHAR)) ;

	_ASSERT(szFileName[0]!=0) ;

	SPathElementInfo pei ;

	pei.uFlag = PET_Path ;

	bool bValidPath = GetPathElement(szFileName,pei) ;

	_ASSERT(bValidPath) ;

	return pei.strPath ;
}

CptString CptWinPath::GetCurPEFileName()
{
	TCHAR szFileName[MAX_PATH] = {0} ;

	::GetModuleFileName(NULL,szFileName,sizeof(szFileName)/sizeof(TCHAR)) ;

	_ASSERT(szFileName[0]!=0) ;

	SPathElementInfo pei ;

	pei.uFlag = PET_FileName ;

	bool bValidPath = GetPathElement(szFileName,pei) ;

	_ASSERT(bValidPath) ;

	return pei.strFileName ;
}

bool CptWinPath::GetPathElement(const TCHAR* pFullPath,SPathElementInfo& pei) 
{
	_ASSERT(pFullPath!=NULL) ;

	int nLastSlashPos = -1 ;
	int nLastDotPos = -1 ;
	int nFirstColonPos = -1 ;
	
	if(pei.uFlag & PET_Drive)
	{// 分区
		if(nFirstColonPos==-1)
		{
			const TCHAR ch = ':' ;

			nFirstColonPos = ReverseFindFirstChrPos(pFullPath,false,&ch,1) ;

			_ASSERT(nFirstColonPos!=-1) ;
		}

		bool bValid = (nFirstColonPos==1) ;

		if(!bValid)
		{
			if(::memcmp(_T("\\\\?\\"),pFullPath,4*sizeof(TCHAR))==0
				&& ::memcmp(_T("UNC"),pFullPath+4,3*sizeof(TCHAR))!=0)
			{
				bValid = true ;
			}
		}

		if(bValid)
		{
			pei.strDrive.Format(_T("%c:"),pFullPath[nFirstColonPos-1]) ;
		}
		else
		{
			return false ;
		}
	}

	if(pei.uFlag & PET_Path)
	{// 路径
		TCHAR szBuf[MAX_PATH] = {0} ;
		if(nLastSlashPos==-1)
		{
			const TCHAR SlashArray[] = {'\\','/'} ;
			nLastSlashPos = ReverseFindFirstChrPos(pFullPath,true,SlashArray,sizeof(SlashArray)/sizeof(TCHAR)) ;
		}
		
		if(nLastSlashPos>=0)
		{
			::memcpy(szBuf,pFullPath,nLastSlashPos*sizeof(TCHAR)) ;

			if(szBuf[nLastSlashPos-1]==':')
			{// 若为根路径
				szBuf[nLastSlashPos] = '\\' ; 
			}
			else
			{
				szBuf[nLastSlashPos] = 0 ;// 该路径不包括最后的那个斜杠
			}

			pei.strPath = szBuf ;
		}
		else
		{
			return false ;
		}
	}

	if(pei.uFlag & PET_FileName)
	{// 文件名
		if(nLastSlashPos==-1)
		{
			const TCHAR SlashArray[] = {'\\','/'} ;
			nLastSlashPos = ReverseFindFirstChrPos(pFullPath,true,SlashArray,sizeof(SlashArray)/sizeof(TCHAR)) ;
		}

		if(nLastSlashPos>=0)
		{
			pei.strFileName = pFullPath+nLastSlashPos+1 ;
		}
		else
		{
			return false ;
		}
	}

	if(pei.uFlag & PET_ExtName)
	{// 文件扩展名
		if(nLastDotPos==-1)
		{
			const TCHAR ch = '.' ;

			nLastDotPos = ReverseFindFirstChrPos(pFullPath,true,&ch,1) ;
		}

		if(nLastDotPos>=0)
		{
			pei.strExtName = pFullPath+nLastDotPos+1 ;
		}
		else
		{
			return false ;
		}
		
	}

	//if(pei.uFlag & PET_FileNameNoExt)
	//{
	//	if(nLastSlashPos==-1)
	//	{
	//		const TCHAR SlashArray[] = {'\\','/'} ;
	//		nLastSlashPos = ReverseFindFirstChrPos(pFullPath,true,SlashArray,sizeof(SlashArray)/sizeof(TCHAR)) ;
	//	}

	//	if(nLastDotPos==-1)
	//	{
	//		const TCHAR ch = '.' ;

	//		nLastDotPos = ReverseFindFirstChrPos(pFullPath,true,&ch,1) ;
	//	}

	//	if(nLastDotPos!=-1 && nLastSlashPos!=-1)
	//	{
	//	}

	//	//TCHAR szBuf[MAX_PATH] = {0} ;
	//	//
	//	//::_tsplitpath(pFullPath,NULL,NULL,szBuf,NULL) ;
	//	//_ASSERT(szBuf[0]!=0) ;

	//	//if(szBuf[0]!=0)
	//	//{
	//	//	return false ;
	//	//}

	//	//pei.strFileNameNoExt = szBuf ;
	//}

	return true ;
}
	
void CptWinPath::SplitFileName(const TCHAR* pFileName,CptString& strFileNameNoExt,CptString& strExtName) 
{
	const TCHAR *p = ::_tcsrchr(pFileName,'.');   

	_ASSERT(p!=NULL) ;
	++p ;
	strExtName = p ;

	TCHAR szBuf[MAX_PATH] = {0} ;

	::memcpy(szBuf,pFileName,(p-pFileName-1)*sizeof(TCHAR)) ;

	strFileNameNoExt = szBuf ;
}

int CptWinPath::ReverseFindFirstChrPos(const TCHAR* pStr,bool bReverse,const TCHAR* pMachCharArray,const int nArraySize)
{
	_ASSERT(pStr!=NULL) ;

	int nRet = -1 ;

	if(bReverse)
	{
		const int nLen = (const int)::_tcslen(pStr) ;

		int nIndex = nLen -1 ;

		while(nIndex>=0)
		{
			if(IsMachChar(pStr[nIndex],pMachCharArray,nArraySize))
			{
				nRet = nIndex ;
				break ;
			}
			--nIndex ;
		}
	}
	else
	{
		int nIndex = 0 ;
		while(pStr[nIndex]!=NULL)
		{
			if(IsMachChar(pStr[nIndex],pMachCharArray,nArraySize))
			{
				nRet = nIndex ;
				break ;
			}
			++nIndex ;
		}
	}

	return nRet ;
}

bool CptWinPath::IsMachChar(const TCHAR ch,const TCHAR* pCharArray,const int nArraySize) 
{
	_ASSERT(pCharArray!=NULL) ;

	for(int i=0;i<nArraySize;++i)
	{
		if(pCharArray[i]==ch)
		{
			return true ;
		}
	}

	return false ;
}

