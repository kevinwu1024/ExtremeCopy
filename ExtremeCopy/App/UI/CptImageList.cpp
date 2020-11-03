/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "CptImageList.h"


CptImageList::CptImageList(void):m_pImageList(NULL)//m_hHandle(NULL)
{
}

CptImageList::CptImageList(const CptImageList& ImageList)
{
	ImageList.AddRef() ;
}

CptImageList::~CptImageList(void)
{
	this->Destroy() ;
}

int CptImageList::AddRef() const
{
	if(m_pImageList!=NULL)
	{
		++m_pImageList->nRefCount ;
	}

	return m_pImageList->nRefCount ;
}

//CptImageList& CptImageList::operator=(const CptImageList& ImageList)
//{
//}

bool CptImageList::Create(SptSize size,int nFlag,int nInitial,int nGrow) 
{
	this->Destroy() ;

	m_pImageList = new SImageListInfo() ;

	if(m_pImageList!=NULL)
	{
		m_pImageList->nRefCount = 1 ;
		m_pImageList->hHandle = ::ImageList_Create(size.nWidth,size.nHeight,nFlag,nInitial,nGrow) ;
	}
	
	return this->IsValid() ;
}

void CptImageList::Destroy() 
{
	if(m_pImageList!=NULL)
	{
		if(--m_pImageList->nRefCount==0)
		{
			::ImageList_Destroy(m_pImageList->hHandle) ;

			delete m_pImageList ;
		}

		m_pImageList = NULL ;
	}
}

bool CptImageList::LoadImage(HINSTANCE hi, LPCTSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags)
{
	this->Destroy() ;

	m_pImageList = new SImageListInfo() ;

	if(m_pImageList!=NULL)
	{
		m_pImageList->nRefCount = 1 ;
		m_pImageList->hHandle = ::ImageList_LoadImage(hi,lpbmp,cx,cGrow,crMask,uType,uFlags) ;
	}

	return this->IsValid() ;
}

void CptImageList::CopyOnWrite()
{
	if(m_pImageList!=NULL && m_pImageList->nRefCount>1)
	{
		--m_pImageList->nRefCount ;

		m_pImageList = new SImageListInfo() ;

		if(m_pImageList!=NULL)
		{
			m_pImageList->nRefCount = 1 ;
			m_pImageList->hHandle = ::ImageList_Duplicate(m_pImageList->hHandle) ;
		}
	
	}
}

bool CptImageList::IsValid() const
{
	return (m_pImageList!=NULL && m_pImageList->hHandle!=NULL) ;
}

//HIMAGELIST CptImageList::GetSafeHandle() const
//{
//	return this->IsValid() ? m_pImageList->hHandle : NULL ;
//}

int CptImageList::GetImageCount() 
{
	int nRet = 0 ;

	if(this->IsValid())
	{
		nRet = ::ImageList_GetImageCount(m_pImageList->hHandle) ;
	}

	return nRet ;
}

bool CptImageList::SetImageCount(int nNewCount) 
{
	bool bRet = false ;

	if(this->IsValid())
	{
		this->CopyOnWrite() ;
		bRet = ::ImageList_SetImageCount(m_pImageList->hHandle,nNewCount) ? true : false ;
	}

	return bRet ;
}

COLORREF CptImageList::SetBkColor(COLORREF crNew) 
{
	COLORREF nRet = 0 ;

	if(this->IsValid())
	{
		this->CopyOnWrite() ;
		nRet = ::ImageList_SetBkColor(m_pImageList->hHandle,crNew) ;
	}

	return nRet ;
}

COLORREF CptImageList::GetBkColor() 
{
	COLORREF nRet = 0 ;

	if(this->IsValid())
	{
		nRet = ::ImageList_GetBkColor(m_pImageList->hHandle) ;
	}

	return nRet ;
}

int CptImageList::ReplaceIcon(int nIndex,HICON hNewIcon) 
{
	int nRet = 0 ;

	if(this->IsValid())
	{
		this->CopyOnWrite() ;
		nRet = ::ImageList_ReplaceIcon(m_pImageList->hHandle,nIndex,hNewIcon) ;
	}

	return nRet ;

}

int CptImageList::AddIcon(HICON hNewIcon) 
{
	return this->ReplaceIcon(-1,hNewIcon) ;
}

//int CptImageList::AddIcon(LPCTSTR lpFile,const SptSize& size)
//{
//	int nRet = 0 ;
//
//	HICON hIcon = (HICON)::LoadImage(NULL,lpFile,IMAGE_ICON,size.nWidth,size.nHeight,LR_LOADFROMFILE) ;
//
//	if(hIcon!=NULL)
//	{
//		nRet = this->AddIcon(hIcon) ;
//	}
//
//	return nRet ;
//}

int CptImageList::AddIcon(LPCTSTR lpFile)
{
	int nRet = 0 ;

	HICON hIcon = (HICON)::LoadImage(NULL,lpFile,IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE) ;

	if(hIcon!=NULL)
	{
		nRet = this->AddIcon(hIcon) ;
	}

	return nRet ;
}

HICON CptImageList::GetIcon(int nIndex,UINT uFlags) 
{
	HICON nRet = NULL ;

	if(this->IsValid())
	{
		nRet = ::ImageList_GetIcon(m_pImageList->hHandle,nIndex,uFlags) ;
	}

	return nRet ;
}

bool CptImageList::Remove(int nIndex) 
{
	bool bRet = false ;

	if(this->IsValid())
	{
		this->CopyOnWrite() ;
		bRet = ::ImageList_Remove(m_pImageList->hHandle,nIndex) ? true : false ;
	}

	return bRet ;
}

bool CptImageList::RemoveAll() 
{
	return this->Remove(-1) ;
}

bool CptImageList::Draw(HDC hDC,int nIndex,SptPoint pt,UINT fStyle) 
{
	bool bRet = false ;

	if(this->IsValid())
	{
		bRet = ::ImageList_Draw(m_pImageList->hHandle,nIndex,hDC,pt.nX,pt.nY,fStyle) ? true : false ;
	}

	return bRet ;
}

bool CptImageList::Copy(CptImageList& ImageList,int nIndex,UINT uFlags)
{
	bool bRet = false ;

	if(this->IsValid() && ImageList.IsValid())
	{
		bRet = ::ImageList_Copy(ImageList.m_pImageList->hHandle,nIndex,m_pImageList->hHandle,nIndex,uFlags) ? true : false ;
	}

	return bRet ;
}

HIMAGELIST CptImageList::GetHandle() 
{
	HIMAGELIST hRet = NULL ;

	if(this->IsValid())
	{
		hRet = ::ImageList_Duplicate(m_pImageList->hHandle) ;
	}

	return hRet ;
}



