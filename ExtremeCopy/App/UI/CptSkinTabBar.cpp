/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "CptSkinTabBar.h"
#include "..\..\Common\ptGlobal.h"

CptSkinTabBar::CptSkinTabBar(void)
{
}

CptSkinTabBar::~CptSkinTabBar(void)
{
}

void CptSkinTabBar::SetImage(const SImageInfo& images)
{
	m_Images = images ;
}

int CptSkinTabBar::AddButton(LPCTSTR lpText)
{
	return 0 ;
}

void CptSkinTabBar::OnLButtonDown(int nFlag,const SptPoint& pt) 
{
}

void CptSkinTabBar::OnLButtonUp(int nFlag,const SptPoint& pt) 
{
}

void CptSkinTabBar::OnMouseMove(int nFlag,const SptPoint& pt) 
{
}

void CptSkinTabBar::OnPaint() 
{
	SptRect rect ;
	SptSize size ;

	::GetClientRect(m_hWnd,rect.GetRECTPointer()) ;

	HDC hDevDC = ::GetDC(m_hWnd) ;
	HDC hMemDC = ::CreateCompatibleDC(hDevDC) ;
	HDC hMemTemDC = ::CreateCompatibleDC(hDevDC) ;
	HBITMAP hMemBitmap = ::CreateCompatibleBitmap(hDevDC,rect.GetWidth(),rect.GetHeight()) ;

	HBITMAP hOldBitmapTem = (HBITMAP)::SelectObject(hMemTemDC,m_Images.hBackground) ;
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC,hMemBitmap) ;

	CptGlobal::GetBitmapSize(m_Images.hBackground,size) ;

	::StretchBlt(hMemDC,0,0,rect.GetWidth(),rect.GetHeight(),hMemTemDC,0,0,size.nWidth,size.nHeight,SRCCOPY) ;
	::BitBlt(hDevDC,0,0,rect.GetWidth(),rect.GetHeight(),hMemDC,0,0,SRCCOPY) ;

	::SelectObject(hMemTemDC,hOldBitmapTem) ;
	::SelectObject(hMemDC,hOldBitmap) ;

	::DeleteObject(hMemBitmap) ;
	::DeleteDC(hMemTemDC) ;
	::DeleteDC(hMemDC) ;
	::ReleaseDC(m_hWnd,hDevDC) ;
}


int CptSkinTabBar::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) 
{
	switch(nMsg)
	{
	case WM_ERASEBKGND:
		return FALSE ;
	}

	return CptCommCtrl::PreProcCtrlMsg(hWnd,nMsg,wParam,lParam) ;
}

int CptSkinTabBar::PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) 
{
	return CptCommCtrl::PreProcParentMsg(hWnd,nMsg,wParam,lParam,bContinue) ;
}