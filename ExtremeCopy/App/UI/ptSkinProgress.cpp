/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptSkinProgress.h"
#include "..\XCGlobal.h"

CptSkinProgress::CptSkinProgress(void):m_nMaxValue(100),m_nMinValue(0),m_nCurValue(0),m_hResultBufBitmap(NULL)
{
	m_hBarBitmap = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_PROGRESSBAR) ;

	m_hPercentFont = ::CreateFont(11,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;
}

CptSkinProgress::~CptSkinProgress(void)
{
	if(m_hBarBitmap!=NULL)
	{
		::DeleteObject(m_hBarBitmap) ;
		m_hBarBitmap =NULL ;
	}

	if(m_hResultBufBitmap!=NULL)
	{
		::DeleteObject(m_hResultBufBitmap) ;
		m_hResultBufBitmap = NULL ;
	}

	SAFE_DELETE_GDI(m_hPercentFont) ;
}

void CptSkinProgress::Draw(HDC hDC) 
{
	_ASSERT(hDC!=NULL) ;

	const int nUnitLen = 10 ;

	HDC hMemDC = ::CreateCompatibleDC(hDC) ;
	HDC hMemDC2 = ::CreateCompatibleDC(hDC) ;

	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC,m_hBarBitmap) ;

	if(m_hResultBufBitmap==NULL)
	{
		m_hResultBufBitmap = ::CreateCompatibleBitmap(hDC,m_Rect.GetWidth(),m_Rect.GetHeight()) ;
	}

	HBITMAP hOldBitmap2 = (HBITMAP)::SelectObject(hMemDC2,m_hResultBufBitmap) ;

	::StretchBlt(hMemDC2,0,0,m_Rect.GetWidth(),m_Rect.GetHeight(),hMemDC,0,0,nUnitLen,16,SRCCOPY) ;

	int nValidLen = (int)(((float)m_nCurValue/m_nMaxValue)*m_Rect.GetWidth()) ;

	int nSpanCount = (nValidLen-20) ;

	if(m_nCurValue>0)
	{
		// head
		::StretchBlt(hMemDC2,0,0,nUnitLen,m_Rect.GetHeight(),hMemDC,2*nUnitLen,0,10,16,SRCCOPY) ;

		if(nSpanCount>0)
		{
			::StretchBlt(hMemDC2,nUnitLen,0,nSpanCount,m_Rect.GetHeight(),hMemDC,3*nUnitLen,0,10,16,SRCCOPY) ;
		}

		nSpanCount = max(nSpanCount,0) ;
		// tail 
		::StretchBlt(hMemDC2,nSpanCount+nUnitLen,0,nUnitLen,m_Rect.GetHeight(),hMemDC,5*nUnitLen,0,10,16,SRCCOPY) ;

	}

	::BitBlt(hDC,m_Rect.nLeft,m_Rect.nTop,m_Rect.GetWidth(),m_Rect.GetHeight(),hMemDC2,0,0,SRCCOPY) ;

	::SelectObject(hMemDC2,hOldBitmap2) ;
	::SelectObject(hMemDC,hOldBitmap) ;

	::DeleteDC(hMemDC2) ;
	::DeleteDC(hMemDC) ;

	{
		HFONT hOldFont = (HFONT)::SelectObject(hDC,m_hPercentFont) ;
		::SetBkMode(hDC,TRANSPARENT) ;
		CptString strPercent ;
		strPercent.Format(_T("%d%%"),m_nCurValue) ;

		::DrawText(hDC,strPercent.c_str(),strPercent.GetLength(),m_Rect.GetRECTPointer(),DT_CENTER) ;

		::SelectObject(hDC,hOldFont) ;
	}
}

void CptSkinProgress::Draw()
{
	if(m_hBarBitmap!=NULL && m_hParentWnd!=NULL && ::IsWindowVisible(m_hParentWnd))
	{
		HDC hDC = ::GetDC(m_hParentWnd) ;

		if(NULL!=hDC)
		{
			this->Draw(hDC) ;

			::ReleaseDC(m_hParentWnd,hDC) ;
		}
	}
}

void CptSkinProgress::SetParent(HWND hWnd)
{
	m_hParentWnd = hWnd ;
}

bool CptSkinProgress::SetRange(int nMax,int nMin)
{
	bool bRet = false ;

	if(nMax>nMin)
	{
		m_nMaxValue=nMax;
		m_nMinValue=nMin;

		this->Draw() ;

		bRet = true ;
	}

	return bRet ;
}

void CptSkinProgress::SetValue(int nValue)
{
	if(nValue<m_nMinValue)
	{
		nValue = m_nMinValue ;
	}
	else if(nValue>m_nMaxValue)
	{
		nValue = m_nMaxValue ;
	}
	else
	{
		m_nCurValue = nValue ;
	}

	this->Draw() ;
}

void CptSkinProgress::SetRectangle(const SptRect& rt)
{
	m_Rect = rt ;

	if(m_hResultBufBitmap!=NULL)
	{
		::DeleteObject(m_hResultBufBitmap) ;
		m_hResultBufBitmap = NULL ;
	}

	this->Draw() ;
}
