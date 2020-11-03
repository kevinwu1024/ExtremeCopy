/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptMultipleLanguage.h"
#include <map>

#pragma warning(disable:4996)

CptMultipleLanguage* CptMultipleLanguage::m_pInstance = NULL ;


CptMultipleLanguage::CptMultipleLanguage(void)
{
	m_hInst = NULL ;
}

CptMultipleLanguage::~CptMultipleLanguage(void)
{
	this->FreeCurrentLibary() ;
}

CptMultipleLanguage* CptMultipleLanguage::GetInstance(const TCHAR* pDLL) 
{
	if(m_pInstance==NULL)
	{
		m_pInstance = new CptMultipleLanguage() ;
		m_pInstance->LoadResourceLibary(pDLL) ;

		_ASSERT(m_pInstance!=NULL) ;
	}

	return m_pInstance ;
}

void CptMultipleLanguage::Release() 
{
	if(m_pInstance!=NULL)
	{
		m_pInstance->FreeCurrentLibary() ;
		delete m_pInstance ;
		m_pInstance = NULL ;
	}
}

HINSTANCE CptMultipleLanguage::GetResourceHandle() const
{
	return m_hInst ;
}

void CptMultipleLanguage::FreeCurrentLibary()
{
	if(m_hInst!=NULL)
	{
		::FreeLibrary(m_hInst) ;
		m_hInst = NULL ;
	}
}

bool CptMultipleLanguage::LoadResourceLibary(const TCHAR* pDLL,HMODULE hModule) 
{
	this->FreeCurrentLibary() ;

	TCHAR szFileName[MAX_PATH] = {0} ;
	::GetModuleFileName(hModule,szFileName,sizeof(szFileName)/sizeof(WCHAR)) ;

	size_t nStrLen =::_tcslen(szFileName) ;
	
	for(size_t i=nStrLen-1;i>=0;--i)
	{
		if(szFileName[i]=='\\')
		{
			szFileName[i+1] = 0 ;

			CptString strPath = szFileName ;
			strPath += _T("Language") ;

			bool bSearch = true ;

			if(pDLL!=NULL)
			{
				::_stprintf(szFileName,_T("%s\\%s"),strPath.c_str(),pDLL) ;
			}

			m_hInst = ::LoadLibraryEx(szFileName,NULL,LOAD_LIBRARY_AS_DATAFILE) ;

			break ;
		}
	}

	return true ;
}

CptString CptMultipleLanguage::GetString(int nResourceID)
{
	TCHAR szBuf[512+1] = {0} ;
	::LoadString(m_hInst,nResourceID,szBuf,sizeof(szBuf)/sizeof(TCHAR)) ;

	return CptString(szBuf) ;
}

HMENU CptMultipleLanguage::GetMenu(int nResourceID)
{
	return ::LoadMenu(m_hInst,MAKEINTRESOURCE(nResourceID)) ;
}

HBITMAP CptMultipleLanguage::GetBitmap(int nResourceID)
{
	return ::LoadBitmap(m_hInst,MAKEINTRESOURCE(nResourceID)) ;
}

HICON CptMultipleLanguage::GetIcon(int nResourceID)
{
	return ::LoadIcon(m_hInst,MAKEINTRESOURCE(nResourceID)) ;
}

HCURSOR CptMultipleLanguage::GetCursor(int nResourceID)
{
	return ::LoadCursor(m_hInst,MAKEINTRESOURCE(nResourceID)) ;
}
