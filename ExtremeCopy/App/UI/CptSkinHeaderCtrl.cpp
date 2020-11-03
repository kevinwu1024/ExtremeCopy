/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "CptSkinHeaderCtrl.h"
#include "..\XCGlobal.h"


CptSkinHeaderCtrl::CptSkinHeaderCtrl(void)
{
}

CptSkinHeaderCtrl::~CptSkinHeaderCtrl(void)
{
}

bool CptSkinHeaderCtrl::Attach(HWND hWnd) 
{
	CptCommCtrl::Attach(hWnd) ;

	return true ;
}

void CptSkinHeaderCtrl::OnPaint() 
{
	this->Paint() ;
}

void CptSkinHeaderCtrl::Paint() 
{
	int nItems = GetItemCount();

	SptRect ItemRect ;
	SptRect ClientRect ;

	SptSize SpanSize ;
	SptSize LeftSize ;
	SptSize RightSize ;

	::GetClientRect(m_hWnd,ClientRect.GetRECTPointer());

	HDC hDevDC = ::GetDC(m_hWnd) ;
	HDC hMemDC = ::CreateCompatibleDC(hDevDC) ;
	HDC hBitmapDC = ::CreateCompatibleDC(hDevDC) ;
	
	HBITMAP hTitleSpan = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_COLUMNTITLESPAN) ;
	HBITMAP hTitleLeft = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_COLUMNTITLELEFT) ;
	HBITMAP hTitleRight = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_COLUMNTITLERIGHT) ;
	
	HBITMAP hMemBitmap = ::CreateCompatibleBitmap(hDevDC,ClientRect.GetWidth(),ClientRect.GetHeight()) ;

	HBITMAP hOldMap = (HBITMAP)::SelectObject(hBitmapDC,hTitleSpan) ;

	HBITMAP hOldMap2 = (HBITMAP)::SelectObject(hMemDC,hMemBitmap) ;
	HBRUSH hBrush = ::CreatePatternBrush(hTitleSpan) ;
	//HBRUSH hBrush = ::CreateSolidBrush(RGB(76,85,118)) ;
	::FillRect(hMemDC,ClientRect.GetRECTPointer(),hBrush) ;

	BITMAP bm ;

	::memset(&bm,0,sizeof(bm)) ;
	::GetObject(hTitleSpan,sizeof(bm),(void*)&bm) ;
	SpanSize.nWidth = bm.bmWidth ;
	SpanSize.nHeight = bm.bmHeight ;

	::memset(&bm,0,sizeof(bm)) ;
	::GetObject(hTitleLeft,sizeof(bm),&bm) ;
	LeftSize.nWidth = bm.bmWidth ;
	LeftSize.nHeight = bm.bmHeight ;

	::memset(&bm,0,sizeof(bm)) ;
	::GetObject(hTitleRight,sizeof(bm),&bm) ;
	RightSize.nWidth = bm.bmWidth ;
	RightSize.nHeight = bm.bmHeight ;

	::SetBkMode(hMemDC,TRANSPARENT) ;
	//::SetTextColor(hMemDC,UI_TEXT_COLOR) ;

	const int nImageHeight = 26 ;

	const UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_TOP |DT_CENTER | DT_END_ELLIPSIS ;

	TCHAR szTitleName[256] = {0};

	int nLeftOffset = 0 ;

	HFONT hNewFont = ::CreateFont(15,0,0,0,FW_THIN,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;

	HFONT hOldFont = (HFONT)::SelectObject(hMemDC,hNewFont) ;

	for(int i = 0; i <nItems; i++)
	{
		HD_ITEM hditem1;
		
		hditem1.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
		hditem1.pszText = szTitleName;
		hditem1.cchTextMax = 255;
		this->GetItem( i, &hditem1 );
		
		this->GetItemRect(i, ItemRect);
		
		// Õ³Ìù×ó±ßÍ¼Æ¬
		::SelectObject(hBitmapDC,hTitleLeft) ;
		::StretchBlt(hMemDC,nLeftOffset,0,2,nImageHeight,hBitmapDC,0,0,LeftSize.nWidth,LeftSize.nHeight,SRCCOPY) ;
		nLeftOffset += 2 ;

		// Õ³ÌùÖÐ±ßÍ¼Æ¬
		::SelectObject(hBitmapDC,hTitleSpan) ;
		::StretchBlt(hMemDC,nLeftOffset,0,ItemRect.GetWidth()-4,nImageHeight,hBitmapDC,0,0,SpanSize.nWidth,SpanSize.nHeight,SRCCOPY) ;
		nLeftOffset += ItemRect.GetWidth()-4 ;

		// Õ³ÌùÓÒ±ßÍ¼Æ¬
		::SelectObject(hBitmapDC,hTitleRight) ;
		::StretchBlt(hMemDC,0,0,2,nImageHeight,hBitmapDC,0,0,RightSize.nWidth,RightSize.nHeight,SRCCOPY) ;
		nLeftOffset += 2 ;

		::DrawText(hMemDC,szTitleName,(int)::_tcslen(szTitleName),ItemRect.GetRECTPointer(),uFormat) ;

		//::BitBlt(hDevDC,0,0,10,nImageHeight,hMemDC,0,0,SRCCOPY) ;

		//DRAWITEMSTRUCT	DrawItemStruct;

		//DrawItemStruct.CtlType		= ODT_HEADER; 
		//DrawItemStruct.hDC			= hMemDC;
		//DrawItemStruct.itemAction	= ODA_DRAWENTIRE; 
		//DrawItemStruct.hwndItem 	= m_hWnd; 
		//DrawItemStruct.rcItem	= ItemRect.GetRECTValue();
		//DrawItemStruct.itemID	= i;

		//::SendMessage(m_hWnd,WM_DRAWITEM,NULL,(LPARAM)&DrawItemStruct) ;

		
		//this->DrawItem(&DrawItemStruct);
	}

	::BitBlt(hDevDC,0,0,ClientRect.GetWidth(),nImageHeight,hMemDC,0,0,SRCCOPY) ;

	::SelectObject(hMemDC,hOldMap2) ;
	::SelectObject(hMemDC,hOldFont) ;
	::SelectObject(hBitmapDC,hOldMap) ;

	::DeleteObject(hTitleLeft) ;
	::DeleteObject(hTitleRight) ;
	::DeleteObject(hTitleSpan) ;
	::ReleaseDC(m_hWnd,hDevDC) ;
	::DeleteDC(hMemDC) ;
	::DeleteDC(hBitmapDC) ;
	::DeleteObject(hMemBitmap) ;
	::DeleteObject(hBrush) ;
	::DeleteObject(hNewFont) ;
}

 int CptSkinHeaderCtrl::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) 
 {
	 switch(nMsg)
	 {
	 //case WM_PAINT:
		// this->Paint() ;
		// //break ;
		// return 0;

	 case WM_ERASEBKGND:
		 this->Paint() ;
		 return FALSE ;
	 }
	 return CptCommCtrl::PreProcCtrlMsg(hWnd,nMsg,wParam, lParam) ;
 }

 bool CptSkinHeaderCtrl::GetItemRect(int nIndex, SptRect& lpRect) const
 {
	 bool bRet = false ;

	 if(m_hWnd!=NULL)
	 {
		 bRet = ((BOOL)::SendMessage(m_hWnd, HDM_GETITEMRECT, nIndex, (LPARAM)lpRect.GetRECTPointer())) ? true : false;
	 }

	 return bRet ;
 }

 int CptSkinHeaderCtrl::GetItemCount() const
 {
	 int nRet = 0 ;

	 if(m_hWnd!=NULL)
	 {
		 nRet = (int) ::SendMessage(m_hWnd, HDM_GETITEMCOUNT, 0, 0L); 
	 }
	 
	 return nRet ;
 }

 int CptSkinHeaderCtrl::InsertItem(int nPos,HDITEM* phdi) 
 {
	 int nRet = 0 ;

	 if(m_hWnd!=NULL)
	 {
		 nRet = (int) ::SendMessage(m_hWnd, HDM_INSERTITEM, nPos, (LPARAM)phdi); 
	 }

	 return nRet ;
 }

 bool CptSkinHeaderCtrl::DeleteItem(int nPos) 
 {
	 bool bRet = false ;

	 if(m_hWnd!=NULL)
	 {
		 bRet = ((BOOL)::SendMessage(m_hWnd, HDM_DELETEITEM, nPos, 0L)) ? true : false;
	 }

	 return bRet ;
 }

 bool CptSkinHeaderCtrl::GetItem(int nPos,HDITEM* pHeaderItem) const 
 {
	 bool bRet = false ;

	 if(m_hWnd!=NULL)
	 {
		 bRet = ((BOOL)::SendMessage(m_hWnd, HDM_GETITEM, nPos, (LPARAM)pHeaderItem)) ? true : false;
	 }

	 return bRet ;
 }

 bool CptSkinHeaderCtrl::SetItem(int nPos, HDITEM* pHeaderItem) 
 {
	 bool bRet = false ;

	 if(m_hWnd!=NULL)
	 {
		 bRet = ((BOOL)::SendMessage(m_hWnd, HDM_SETITEM, nPos, (LPARAM)pHeaderItem)) ? true : false;
	 }

	 return bRet ;
 }

 bool CptSkinHeaderCtrl::Layout(HDLAYOUT* pHeaderLayout) 
 {
	 bool bRet = false ;

	 if(m_hWnd!=NULL)
	 {
		 bRet = ((BOOL)::SendMessage(m_hWnd, HDM_LAYOUT, 0, (LPARAM)pHeaderLayout)) ? true : false;
	 }

	 return bRet ;
 }