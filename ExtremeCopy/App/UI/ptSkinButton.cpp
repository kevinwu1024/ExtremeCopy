/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptSkinButton.h"
#include <CommCtrl.h>

CptSkinButton::CptSkinButton(void)
{
	m_hNormal = NULL ;
	m_hHover = NULL ;
	m_hDown = NULL ;
	m_hFont = NULL ;
}

CptSkinButton::~CptSkinButton(void)
{
	SAFE_DELETE_GDI(m_hFont) ;
}

void CptSkinButton::Attach(HWND hwnd)
{
	CptCommCtrl::Attach(hwnd) ;

	m_strText.Empty() ;

	if(m_hWnd!=NULL)
	{
		TCHAR szBuf[128] = {0} ;
		::GetWindowText(m_hWnd,szBuf,sizeof(szBuf)/sizeof(TCHAR)) ;
		m_strText = szBuf ;

		//int nStyleEx = ::GetWindowLong(m_hWnd,GWL_STYLE) ;

		//nStyleEx |= BS_OWNERDRAW ;

		//::SetWindowLong(m_hWnd,GWL_STYLE,nStyleEx) ;
	}
}

void CptSkinButton::Update()
{
	if(m_hWnd!=NULL)
	{
		::InvalidateRect(m_hWnd,NULL,TRUE) ;
		::UpdateWindow(m_hWnd) ;
	}
}

void CptSkinButton::SetBkBitmap(const SSkinButtonBitmapInfo& sbbi)
{
	this->SetBkBitmap(sbbi.hNormal,sbbi.hHover,sbbi.hDown,sbbi.hDisable) ;
}

void CptSkinButton::SetBkBitmap(HBITMAP hNormal,HBITMAP hHover,HBITMAP hDown,HBITMAP hDisable)
{
	m_hNormal = hNormal ;
	m_hHover = hHover ;
	m_hDown = hDown ;
	m_hDisable = hDisable ;

	if(m_hWnd!=NULL)
	{
		::InvalidateRect(m_hWnd,NULL,TRUE) ;
	}

	int nStyleEx = ::GetWindowLong(m_hWnd,GWL_STYLE) ;

	nStyleEx |= BS_OWNERDRAW ;

	::SetWindowLong(m_hWnd,GWL_STYLE,nStyleEx) ;
}

void CptSkinButton::Paint()
{
	if(::IsWindowEnabled(this->GetSaftHwnd()))
	{
		switch(m_CurState.status.MouseStatus)
		{
		case ControlMouseStatus_Hover:

			this->Paint(m_hHover) ;
			break ;

		case ControlMouseStatus_Down:
			this->Paint(m_hDown) ;
			break ;

		case ControlMouseStatus_Leave:
		case ControlMouseStatus_Normal:
		default:
			this->Paint(m_hNormal) ;
			break ;
		}
	}
	else
	{
		this->Paint(m_hDisable) ;
	}
}

void CptSkinButton::Paint(HBITMAP hBitmap)
{
	if(hBitmap!=NULL && m_hWnd!=NULL)
	{
		RECT rtClient ;
		BITMAP bm ;

		::GetObject(hBitmap,sizeof(bm),&bm) ;
		::GetClientRect(m_hWnd,&rtClient) ;

		HDC hDC = ::GetDC(m_hWnd) ;
		HDC hCompatableDC = ::CreateCompatibleDC(hDC) ;

		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hCompatableDC,hBitmap) ;

		::StretchBlt(hDC,0,0,rtClient.right,rtClient.bottom,hCompatableDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY) ;

		if(!m_strText.IsEmpty())
		{
			if(m_hFont==NULL)
			{
				m_hFont = ::CreateFont(13,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,
					CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;
			}

			HFONT hOldFont = (HFONT)::SelectObject(hDC,m_hFont) ;
			int OldMode = ::SetBkMode(hDC,TRANSPARENT) ;
			//::TextOut(hDC,0,0,m_strText.c_str(),m_strText.GetLength()) ;

			::DrawText(hDC,m_strText.c_str(),m_strText.GetLength(),&rtClient,DT_CENTER|DT_VCENTER|DT_SINGLELINE ) ;

			::SetBkMode(hDC,OldMode) ;

			::SelectObject(hDC,hOldFont) ;
		}

		::DeleteDC(hCompatableDC) ;
		//::DeleteDC(hDC) ;
		::ReleaseDC(m_hWnd,hDC) ;
	}
}

void CptSkinButton::OnMouseEnter() 
{
	this->Paint() ;
}

void CptSkinButton::OnMuseLeave() 
{
	this->Paint() ;
}

void CptSkinButton::OnLButtonDown(int nFlag,const SptPoint& pt) 
{
	this->Paint() ;
}

void CptSkinButton::OnLButtonUp(int nFlag,const SptPoint& pt) 
{
	this->Paint() ;
}

void CptSkinButton::SetTextBold(bool bBold)
{
	if(m_hWnd!=NULL && m_strText.GetLength()>0)
	{
		SAFE_DELETE_GDI(m_hFont) ;

		if(bBold)
		{
			m_hFont = ::CreateFont(13,0,0,0,FW_SEMIBOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,
				CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;
		}
		else
		{
			m_hFont = ::CreateFont(13,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,
				CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;
		}

		this->Paint() ;
	}
}

int CptSkinButton::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam)
{
	int nRet = CptCommCtrl::PreProcCtrlMsg(hWnd,nMsg,wParam,lParam) ;

	switch(nMsg)
	{
	case WM_SETTEXT:
		{
			m_strText = (TCHAR*)lParam ;
		}
		break ;

	case WM_ENABLE:
		{
			//BOOL bEnable = (BOOL)wParam ;
			this->Paint() ;
		}
		break ;

		/**
	case WM_KILLFOCUS:
		if(m_strText.GetLength()>0)
		{
			SAFE_DELETE_GDI(m_hFont) ;

			m_hFont = ::CreateFont(13,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,
				CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;

			this->Paint() ;
		}
		break ;

	case WM_SETFOCUS:
		if(m_strText.GetLength()>0)
		{
			SAFE_DELETE_GDI(m_hFont) ;

			m_hFont = ::CreateFont(13,0,0,0,FW_SEMIBOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,
				CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;

			this->Paint() ;
		}
		break ;
		/**/
	}

	return nRet ;

}

int CptSkinButton::PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) 
{
	if(nMsg==WM_DRAWITEM && m_hNormal!=NULL)
	{
		DRAWITEMSTRUCT* pIS = (LPDRAWITEMSTRUCT) lParam ;

		if(pIS!=NULL && pIS->hwndItem==m_hWnd && pIS->CtlType==ODT_BUTTON)
		{
			if(pIS->itemAction&ODA_SELECT || pIS->itemState==ODS_SELECTED)
			{
				m_CurState.status.MouseStatus = ControlMouseStatus_Down ;
				this->Paint() ;
				m_CurState.status.MouseStatus = ControlMouseStatus_Hover ;
				this->Paint() ;
			}
			else
			{
				this->Paint() ;
			}
		}
	}

	return CptCommCtrl::PreProcParentMsg(hWnd,nMsg,wParam,lParam,bContinue) ;
}
