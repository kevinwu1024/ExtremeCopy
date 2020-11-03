/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "CptTabCtrl.h"

CptTabCtrl::CptTabCtrl(void)
{
}

CptTabCtrl::~CptTabCtrl(void)
{
}

int CptTabCtrl::GetCurFocus() const 
{
	int nRet = -1 ;

	if(m_hWnd!=NULL)
	{
		nRet = TabCtrl_GetCurFocus(m_hWnd) ;
	}

	return nRet ;
}

void CptTabCtrl::SetCurFocus(int nIndex)
{
	TabCtrl_SetCurFocus(m_hWnd,nIndex) ;
}

int CptTabCtrl::GetCurSel() const
{
	return TabCtrl_GetCurSel(m_hWnd) ;
}

int CptTabCtrl::SetCurSel(int nIndex)
{
	return TabCtrl_SetCurSel(m_hWnd,nIndex) ;
}

DWORD CptTabCtrl::GetExtendedStyle()
{
	return TabCtrl_GetExtendedStyle(m_hWnd) ;
}

DWORD CptTabCtrl::SetExtendedStyle(DWORD dwNewStyle)
{
	return TabCtrl_SetExtendedStyle(m_hWnd,dwNewStyle) ;
}

HIMAGELIST CptTabCtrl::GetImageList() const 
{
	return TabCtrl_GetImageList(m_hWnd) ;
}

bool CptTabCtrl::GetItem(int nIndex,TCITEM& Item) const
{
	return TabCtrl_GetItem(m_hWnd,nIndex,&Item) ? true : false ;
}

bool CptTabCtrl::SetItem(int nIndex,TCITEM& Item) 
{
	return TabCtrl_SetItem(m_hWnd,nIndex,&Item) ? true : false ;
}


int CptTabCtrl::GetItemCount() const
{
	return TabCtrl_GetItemCount(m_hWnd) ;
}

bool CptTabCtrl::GetItemRect(int nIndex,SptRect& rect) const
{
	return TabCtrl_GetItemRect(m_hWnd,nIndex,rect.GetRECTPointer()) ? true : false ;
}
//
//DWORD CptTabCtrl::GetItemState( int nIndex, DWORD dwMask) const
//{
//	return TabCtrl_GetItemState(m_hWnd,nIndex,dwMask) ;
//}
//
//bool CptTabCtrl::SetItemState(int nIndex, DWORD dwMask,DWORD dwState) 
//{
//}

int CptTabCtrl::GetRowCount( ) const
{
	return TabCtrl_GetRowCount(m_hWnd) ;
}

int CptTabCtrl::SetMinTabWidth(int cx)
{
	return TabCtrl_SetMinTabWidth(m_hWnd,cx) ;
}

bool CptTabCtrl::DeleteItem(int nIndex) 
{
	return TabCtrl_DeleteItem(m_hWnd,nIndex) ? true : false ;
}

bool CptTabCtrl::DeleteAllItems()
{
	return TabCtrl_DeleteAllItems(m_hWnd) ? true : false ;
}

int CptTabCtrl::InsertItem(int nIndex,const TCITEM& item) 
{
	return TabCtrl_InsertItem(m_hWnd,nIndex,&item) ;
}
