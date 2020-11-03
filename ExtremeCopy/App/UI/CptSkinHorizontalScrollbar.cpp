/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "CptSkinHorizontalScrollbar.h"
#include "ptListView.h"
#include "..\..\Common\ptGlobal.h"

CptSkinHorizontalScrollbar::CptSkinHorizontalScrollbar(void)
{
	m_nThumbLeft = 25;
	m_duThumbRemainder = 0.00f;

	m_bMouseDown = false;
	m_bMouseDownArrowLeft = false;
	m_bMouseDownArrowRight = false;
	m_bDragging = false;
}

CptSkinHorizontalScrollbar::~CptSkinHorizontalScrollbar(void)
{
}

bool CptSkinHorizontalScrollbar::Attach(HWND hWnd)
{
	return CptCommCtrl::Attach(hWnd) ;

	//::InvalidateRect(m_hWnd,NULL,TRUE) ;
	//::UpdateWindow(m_hWnd) ;

	return true ;
}

void CptSkinHorizontalScrollbar::OnLButtonDown(int nFlag,const SptPoint& pt) 
{
	::SetCapture(m_hWnd);
	SptRect clientRect;
	::GetClientRect(m_hWnd,clientRect.GetRECTPointer());
	
	int nWidth = clientRect.GetWidth()-m_ArrowSize.nWidth;

	SptRect rectLeftArrow(0,0,m_ArrowSize.nWidth,m_ArrowSize.nHeight);
	SptRect rectRightArrow(nWidth,0,nWidth+m_ArrowSize.nWidth,m_ArrowSize.nHeight);
	SptRect rectThumb(m_nThumbLeft,0,m_nThumbLeft+m_ThumbSize.nWidth,m_ThumbSize.nHeight);
	
	//Debug_Printf(_T("OnLButtonDown() (%d %d)"),pt.nX,pt.nY) ;

	if(rectThumb.IsContainPoint(pt))
	{
		//Debug_Printf(_T("OnLButtonDown() thumb (%d %d %d %d)"),rectThumb.nLeft,rectThumb.nTop,rectThumb.nRight,rectThumb.nBottom) ;

		m_bMouseDown = true;
	}

	if(rectRightArrow.IsContainPoint(pt))
	{
		//Debug_Printf(_T("OnLButtonDown() right (%d %d %d %d)"),rectRightArrow.nLeft,rectRightArrow.nTop,rectRightArrow.nRight,rectRightArrow.nBottom) ;

		m_bMouseDownArrowRight = true;
		SetTimer(m_hWnd,2,250,NULL);
	}

	if(rectLeftArrow.IsContainPoint(pt))
	{
		//Debug_Printf(_T("OnLButtonDown() left (%d %d %d %d)"),rectLeftArrow.nLeft,rectLeftArrow.nTop,rectLeftArrow.nRight,rectLeftArrow.nBottom) ;

		m_bMouseDownArrowLeft = true;
		::SetTimer(m_hWnd,2,250,NULL);
	}
}

void CptSkinHorizontalScrollbar::OnLButtonUp(int nFlag,const SptPoint& pt) 
{
	UpdateThumbPosition();
	KillTimer(m_hWnd,1);
	ReleaseCapture();
	
	bool bInChannel = true;
	
	SptRect clientRect;
	GetClientRect(m_hWnd,clientRect.GetRECTPointer());
	
	int nWidth = clientRect.GetWidth()-m_ArrowSize.nWidth;

	SptRect rectLeftArrow(0,0,m_ArrowSize.nWidth,m_ArrowSize.nHeight);
	SptRect rectThumb(m_nThumbLeft,0,m_nThumbLeft+m_ThumbSize.nWidth,m_ThumbSize.nHeight);

	if(rectLeftArrow.IsContainPoint(pt))
	{
		this->ScrollLeft();	
		bInChannel = false;
	}

	SptRect rectRightArrow(nWidth,0,nWidth+m_ArrowSize.nWidth,m_ArrowSize.nHeight);
	
	if(rectRightArrow.IsContainPoint(pt))
	{
		ScrollRight();	
		bInChannel = false;
	}

	if(rectThumb.IsContainPoint(pt))
	{
		bInChannel = false;
	}

	if(bInChannel == true && !m_bMouseDown)
	{
		if(pt.nX > m_nThumbLeft)
		{
			this->PageRight();
		}
		else
		{
			this->PageLeft();
		}
	}

	//reset all variables
	m_bMouseDown = false;
	m_bDragging = false;
	m_bMouseDownArrowLeft = false;
	m_bMouseDownArrowRight = false;
}

void CptSkinHorizontalScrollbar::OnMouseMove(int nFlag,const SptPoint& pt) 
{
}

void CptSkinHorizontalScrollbar::OnTimer(int nTimerID) 
{
}

void CptSkinHorizontalScrollbar::OnPaint() 
{
	this->Draw() ;
}

void CptSkinHorizontalScrollbar::PageRight()
{
	//m_ListView->SendMessage(WM_HSCROLL, MAKELONG(SB_PAGEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CptSkinHorizontalScrollbar::PageLeft()
{
	//m_ListView->SendMessage(WM_HSCROLL, MAKELONG(SB_PAGEUP,0),NULL);
	UpdateThumbPosition();
}

void CptSkinHorizontalScrollbar::ScrollLeft()
{
	//m_ListView->SendMessage(WM_HSCROLL, MAKELONG(SB_LINELEFT,0),NULL);
	UpdateThumbPosition();
}

void CptSkinHorizontalScrollbar::ScrollRight()
{
	//m_ListView->SendMessage(WM_HSCROLL, MAKELONG(SB_LINERIGHT,0),NULL);
	UpdateThumbPosition();
}

void CptSkinHorizontalScrollbar::UpdateThumbPosition()
{
	//return ;
	/**
	SptRect clientRect;
	::GetClientRect(m_hWnd,clientRect.GetRECTPointer());

	double nPos = 0;//m_ListView->GetScrollPos(SB_HORZ);
	int nMax2,nMin2 ;
	//m_ListView->GetScrollRange(SB_HORZ,nMax2,nMin2) ;
	double nMax  = nMax2 ;
	//double nMax = m_ListView->GetScrollLimit(SB_HORZ);
	double nWidth = clientRect.GetWidth()-75; 
	double nVar = nMax;

	m_duThumbInterval = nWidth/nVar;

	double nNewdbValue = m_duThumbInterval * (nPos);
	int nNewValue = (int)nNewdbValue;
	double nExtra = nNewdbValue - nNewValue;
	m_duThumbRemainder = nExtra;
	
	m_nThumbLeft = 25+nNewValue;

	this->LimitThumbPosition();
	/**/
	
	this->Draw();
}

void CptSkinHorizontalScrollbar::LimitThumbPosition()
{
	SptRect clientRect;
	::GetClientRect(m_hWnd,clientRect.GetRECTPointer());

	if(m_nThumbLeft+m_ArrowSize.nWidth > (clientRect.GetWidth()-m_ArrowSize.nWidth))
	{
		m_nThumbLeft = clientRect.GetWidth()-2*m_ArrowSize.nWidth;
	}

	if(m_nThumbLeft < (clientRect.nLeft+m_ArrowSize.nWidth))
	{
		m_nThumbLeft = clientRect.nLeft+m_ArrowSize.nWidth;
	}
}

void CptSkinHorizontalScrollbar::SetSkinImage(const SScrollbarImage& image)
{
	m_Images = image ;

	CptGlobal::GetBitmapSize(m_Images.FirstArrow.hNormal,m_ArrowSize) ;
	CptGlobal::GetBitmapSize(m_Images.Thumb.hNormal,m_ThumbSize) ;

	m_nThumbLeft = m_ArrowSize.nWidth ;
}

//bool CptSkinHorizontalScrollbar::GetBitmapSize(HBITMAP hBitmap,SptSize& size)
//{
//	bool bRet = false ;
//
//	if(hBitmap!=NULL)
//	{
//		BITMAP bm ;
//
//		::memset(&bm,0,sizeof(bm)) ;
//		if(::GetObject(hBitmap,sizeof(bm),&bm))
//		{
//			size.nWidth = bm.bmWidth ;
//			size.nHeight = bm.bmHeight ;
//
//			bRet = true ;
//		}
//	}
//
//	return bRet ;
//	
//}

void CptSkinHorizontalScrollbar::Draw() 
{
	SptRect clientRect ;

	::GetClientRect(m_hWnd,clientRect.GetRECTPointer()) ;

	const int nArrowWidth = m_ArrowSize.nWidth ;
	const int nArrowHeight = m_ArrowSize.nHeight ;

	HDC hDevDC = ::GetDC(m_hWnd) ;
	HDC hMemDC = ::CreateCompatibleDC(hDevDC) ;
	HDC hMemTemDC = ::CreateCompatibleDC(hDevDC) ;

	HBITMAP hMemBitmap = ::CreateCompatibleBitmap(hDevDC,clientRect.GetWidth(),clientRect.GetHeight()) ;

	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC,hMemBitmap) ;

	HBRUSH hBrush = ::CreateSolidBrush(RGB(255,0,0)) ;

	::FillRect(hMemDC,clientRect.GetRECTPointer(),hBrush) ;

	SptSize size ;

	HBITMAP hOldBitmap1 = (HBITMAP)::SelectObject(hMemTemDC,m_Images.Background.hNormal) ;
	
	if(CptGlobal::GetBitmapSize(m_Images.Background.hNormal,size))
	{
		::StretchBlt(hMemDC,0,0,clientRect.GetWidth(),clientRect.GetHeight(),hMemTemDC,0,0,size.nWidth,size.nHeight,SRCCOPY) ;
	}
	
	if(CptGlobal::GetBitmapSize(m_Images.FirstArrow.hNormal,size))
	{
		::SelectObject(hMemTemDC,m_Images.FirstArrow.hNormal) ;
		::StretchBlt(hMemDC,0,0,nArrowWidth,nArrowHeight,hMemTemDC,0,0,size.nWidth,size.nHeight,SRCCOPY) ;
	}
	
	if(CptGlobal::GetBitmapSize(m_Images.SecondArrow.hNormal,size) )
	{
		::SelectObject(hMemTemDC,m_Images.SecondArrow.hNormal) ;
		::StretchBlt(hMemDC,clientRect.GetWidth()-nArrowWidth,0,nArrowWidth,nArrowHeight,hMemTemDC,0,0,size.nWidth,size.nHeight,SRCCOPY) ;
	}
	
	if(CptGlobal::GetBitmapSize(m_Images.Thumb.hNormal,size))
	{
		::SelectObject(hMemTemDC,m_Images.Thumb.hNormal) ;
		::StretchBlt(hMemDC,m_nThumbLeft,0,nArrowWidth,nArrowHeight,hMemTemDC,0,0,size.nWidth,size.nHeight,SRCCOPY) ;
	}

	::BitBlt(hDevDC,0,0,clientRect.GetWidth(),clientRect.GetHeight(),hMemDC,0,0,SRCCOPY) ;

	::SelectObject(hMemDC,hOldBitmap) ;
	::SelectObject(hMemTemDC,hOldBitmap1) ;

	::DeleteObject(hBrush) ;
	::DeleteObject(hMemBitmap) ;

	::DeleteObject(hMemTemDC) ;
	::DeleteDC(hMemDC) ;
	::ReleaseDC(m_hWnd,hDevDC) ;
}

int CptSkinHorizontalScrollbar::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam)
{
	switch(nMsg)
	{
	//case WM_LBUTTONDOWN:
	//	{
	//		SptPoint pt ;

	//		pt.nX = lParam&0xffff ;
	//		pt.nY = lParam >> 16 ;

	//		this->OnLButtonDown((int)wParam,pt) ;
	//	}
	//	break ;

	//case WM_LBUTTONUP:
	//	{
	//		SptPoint pt ;

	//		pt.nX = lParam&0xffff ;
	//		pt.nY = lParam >> 16 ;

	//		this->OnLButtonUp((int)wParam,pt) ;
	//	}
	//	break ;

	//case WM_MOUSEMOVE:
	//	{
	//		SptPoint pt ;

	//		pt.nX = lParam&0xffff ;
	//		pt.nY = lParam >> 16 ;

	//		this->OnMouseMove((int)wParam,pt) ;
	//	}
	//	break ;

	//case WM_ERASEBKGND:
	//	this->OnPaint() ;
	//	return FALSE ;

	//case WM_PAINT:
	//	this->OnPaint() ;
	//	return 0 ;
	//	//break ;

	case WM_TIMER:
		this->OnTimer((int)wParam) ;
		break ;
	}

	return CptCommCtrl::PreProcCtrlMsg(hWnd,nMsg,wParam, lParam) ;
}

int CptSkinHorizontalScrollbar::PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) 
{

	int nRet = CptCommCtrl::PreProcParentMsg(hWnd,nMsg,wParam,lParam,bContinue) ;

	return nRet ;
	
}