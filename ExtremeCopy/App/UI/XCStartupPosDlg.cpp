/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "stdafx.h"
#include "XCStartupPosDlg.h"
#include "../Language/XCRes_ENU/resource.h"
#include "../../Common/ptGlobal.h"
#include "../ptMultipleLanguage.h"
#include "../XCConfiguration.h"


CXCStartupPosDlg::CXCStartupPosDlg(const SptPoint& pt)
	:CptDialog(IDD_DIALOG_STARTUPPOS,NULL,CptMultipleLanguage::GetInstance()->GetResourceHandle()),
	m_ptStartupPos(pt),m_hExtremeCopyBitmap(NULL),m_bHold(false)
{
#ifndef VERSION_PROFESSIONAL
	// 非专业版的，则每次都以默认的位置作为初始位置
	SptPoint pt2 ;

	pt2.nX = (int)(m_ScaleX * CONFIG_DEFAULT_MAINDIALOG_POS_X) ;
	pt2.nY = (int)(m_ScaleY * CONFIG_DEFAULT_MAINDIALOG_POS_Y) ;

	m_XCUIRect.SetLocation(pt2) ;
#endif
}


CXCStartupPosDlg::~CXCStartupPosDlg(void)
{
	if(m_hXCUIMemDC!=NULL)
	{
		::SelectObject(m_hXCUIMemDC,m_hOldXCMemBitmap) ;
		::DeleteObject(m_hXCUIMemDC) ;
		::DeleteObject(m_hExtremeCopyBitmap) ;
	}

	if(m_hDesktopMemDC!=NULL)
	{
		::SelectObject(m_hDesktopMemDC,m_hOldDesktopMemBitmap) ;
		::DeleteObject(m_hDesktopMemDC) ;
		::DeleteObject(m_hWinDesktopBitmap) ;
	}

	SAFE_DELETE_GDI(m_hExtremeCopyBitmap) ;
}


BOOL CXCStartupPosDlg::OnInitDialog() 
{
	this->CenterParentWindow() ;

	m_hPicWnd = ::GetDlgItem(this->GetSafeHwnd(),IDC_STATIC_SCREEN) ;
	m_hExtremeCopyBitmap = CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_EXTREMECOPYUI) ;
	m_hWinDesktopBitmap = CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_DESKTOP) ;

	SptSize ScreenSize ;
	ScreenSize.nWidth = ::GetSystemMetrics(SM_CXSCREEN) ;
	ScreenSize.nHeight = ::GetSystemMetrics(SM_CYSCREEN) ;

	// 创建 ExtremeCopy UI 图片的内存兼容DC
	HDC hWndDC = ::GetDC(m_hPicWnd) ;

	m_hXCUIMemDC = ::CreateCompatibleDC(hWndDC) ;
	m_hOldXCMemBitmap = (HBITMAP)::SelectObject(m_hXCUIMemDC,m_hExtremeCopyBitmap) ;

	m_hDesktopMemDC = ::CreateCompatibleDC(hWndDC) ;
	m_hOldDesktopMemBitmap = (HBITMAP)::SelectObject(m_hDesktopMemDC,m_hWinDesktopBitmap) ;

	::ReleaseDC(m_hPicWnd,hWndDC) ;

	::GetClientRect(m_hPicWnd,m_PicRect.GetRECTPointer()) ;
	
	::ScreenToClient(this->GetSafeHwnd(),m_PicRect.GetLocation().GetPOINTPointer()) ;

	// 指定 ExtremeCopy 图片在 PIC 窗口所在的区域
	m_ScaleX = (float)m_PicRect.GetWidth()/ScreenSize.nWidth ;
	m_ScaleY = (float)m_PicRect.GetHeight()/ScreenSize.nHeight ;

	SptPoint pt ;
	pt.nX = (int)(m_ScaleX * m_ptStartupPos.nX) ;
	pt.nY = (int)(m_ScaleY * m_ptStartupPos.nY) ;

	m_XCUIRect.SetLocation(pt) ;
	m_XCUIRect.SetWidth((m_PicRect.GetWidth()*379)/ScreenSize.nWidth) ;
	m_XCUIRect.SetHeight((m_PicRect.GetHeight()*201)/ScreenSize.nHeight) ;

	BITMAP bm ;
	::memset(&bm,0,sizeof(bm)) ;
	::GetObject(m_hExtremeCopyBitmap,sizeof(bm),&bm) ;
	m_XCUIImageSize.nWidth = bm.bmWidth ;
	m_XCUIImageSize.nHeight = bm.bmHeight ;

	::memset(&bm,0,sizeof(bm)) ;
	::GetObject(m_hWinDesktopBitmap,sizeof(bm),&bm) ;
	m_DesktopImageSize.nWidth = bm.bmWidth ;
	m_DesktopImageSize.nHeight = bm.bmHeight ;

	m_bCrossCursor = false ;

	this->DrawXCPos() ;

#ifndef VERSION_PROFESSIONAL
	// 非专业版，禁用OK按钮
	::EnableWindow(this->GetDlgItem(IDOK),FALSE) ;
#endif

	return CptDialog::OnInitDialog() ;
}

void CXCStartupPosDlg::OnPaint() 
{
	this->DrawXCPos() ;
	//CptDialog::OnPaint() ;
}

BOOL CXCStartupPosDlg::OnEraseBkgnd(HDC hDC)
{
	SptRect rect ;
	::GetClientRect(this->GetSafeHwnd(),rect.GetRECTPointer()) ;

	HBRUSH hDlgBrush = ::CreateSolidBrush(::GetSysColor( COLOR_3DFACE )) ;
	
	::FillRect(hDC,rect.GetRECTPointer(),hDlgBrush) ;
	::DeleteObject(hDlgBrush) ;

	this->DrawXCPos() ;
	return TRUE ;
}

void CXCStartupPosDlg::DrawXCPos() 
{
	HDC hWndDC = ::GetDC(m_hPicWnd) ;
	HDC hCompatibleDC = ::CreateCompatibleDC(hWndDC) ;

	HBITMAP hMemBitmap = ::CreateCompatibleBitmap(hWndDC,m_PicRect.GetWidth(),m_PicRect.GetHeight()) ;
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hCompatibleDC,hMemBitmap) ;
	
	SptRect rect ;
	::GetClientRect(m_hPicWnd,rect.GetRECTPointer()) ;
	HBRUSH hWhiteBrush = (HBRUSH)::GetStockObject(WHITE_BRUSH) ;
	//::FillRect(hCompatibleDC,rect.GetRECTPointer(),hWhiteBrush) ;
	// 
	::StretchBlt(hCompatibleDC,0,0,rect.GetWidth(),rect.GetHeight(),m_hDesktopMemDC,0,0,m_DesktopImageSize.nWidth,m_DesktopImageSize.nHeight,SRCCOPY) ;

	::MoveToEx(hCompatibleDC,0,0,NULL) ;
	::LineTo(hCompatibleDC,rect.GetWidth()-1,0) ;
	::LineTo(hCompatibleDC,rect.GetWidth()-1,rect.GetHeight()-1) ;
	::LineTo(hCompatibleDC,0,rect.GetHeight()-1) ;
	::LineTo(hCompatibleDC,0,0) ;
	::StretchBlt(hCompatibleDC,m_XCUIRect.GetLocation().nX,m_XCUIRect.GetLocation().nY,m_XCUIRect.GetWidth(),m_XCUIRect.GetHeight(),m_hXCUIMemDC,0,0,m_XCUIImageSize.nWidth,m_XCUIImageSize.nHeight,SRCCOPY) ;
	::BitBlt(hWndDC,0,0,m_PicRect.GetWidth(),m_PicRect.GetHeight(),hCompatibleDC,0,0,SRCCOPY) ;

	::SelectObject(hCompatibleDC,hOldBitmap) ;
	::DeleteObject(hMemBitmap) ;
	::DeleteDC(hCompatibleDC) ;
	::ReleaseDC(m_hPicWnd,hWndDC) ;
}

void CXCStartupPosDlg::OnLButtonDown(int nFlag,const SptPoint& pt) 
{
	m_bHold = false ;

	if(m_XCUIRect.IsContainPoint(pt))
	{
		m_bHold = true ;

		m_ptBeginHold = pt ;
	}
}

void CXCStartupPosDlg::OnLButtonUp(int nFlag,const SptPoint& pt) 
{
	if(m_bHold)
	{
	}

	m_bHold = false ;
}

void CXCStartupPosDlg::OnMouseMove(int nFlag,const SptPoint& pt) 
{
	if(m_bHold)
	{
		//if(m_XCUIRect.GetWidth()+m_XCUIRect.
		SptPoint ptNew ;
		ptNew.nX = pt.nX - m_ptBeginHold.nX  + m_XCUIRect.GetLocation().nX ;
		ptNew.nY = pt.nY - m_ptBeginHold.nY + m_XCUIRect.GetLocation().nY;

		if(ptNew.nX>=0 && ptNew.nY>=0 && ptNew.nX + m_XCUIRect.GetWidth()<=m_PicRect.GetWidth()
			&& ptNew.nY + m_XCUIRect.GetHeight()<=m_PicRect.GetHeight())
		{
					m_ptBeginHold = pt ;

		m_XCUIRect.SetLocation(ptNew) ;

		this->DrawXCPos() ;
		}
	}
	else
	{
		if(m_XCUIRect.IsContainPoint(pt) && !m_bCrossCursor)
		{// 修改 cursor 为 十字可移动的
			
		}
		else
		{// 把 cursor 恢复为默认的
		}
	}
}

bool CXCStartupPosDlg::OnOK()
{
	m_ptStartupPos.nX = (LONG)(m_XCUIRect.GetLocation().nX / m_ScaleX) ;
	m_ptStartupPos.nY = (LONG)(m_XCUIRect.GetLocation().nY / m_ScaleY) ;

	return true ;
}

//bool CXCStartupPosDlg::OnCancel() 
//{
//	return true ;
//	CptString strText = CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_SUREEXIT) ;
//	CptString strWarning = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;
//
//	return (::MessageBox(this->GetSafeHwnd(),strText.c_str(),strWarning.c_str(),MB_YESNO)==IDYES)  ;
//}


void CXCStartupPosDlg::OnButtonClick(int nButtonID) 
{
	switch(nButtonID)
	{
	case IDC_BUTTON_RESTOREDEFAULT: // 恢复默认位置
		{
			SptPoint pt ;

			pt.nX = (int)(m_ScaleX * CONFIG_DEFAULT_MAINDIALOG_POS_X) ;
			pt.nY = (int)(m_ScaleY * CONFIG_DEFAULT_MAINDIALOG_POS_Y) ;

			m_XCUIRect.SetLocation(pt) ;

			this->DrawXCPos() ;
		}
		//this->PerformButton(IDCANCEL) ;
		break ;
	}
}

SptPoint CXCStartupPosDlg::GetStartupPos() const
{
	return m_ptStartupPos ;
}