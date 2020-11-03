/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "SkinDialog.h"
#include "..\XCConfiguration.h"

#define ROUND_EDGE_SIZE		8

CSkinDialog::CSkinDialog(int nDlgID,HWND hParentWnd):CptDialog(nDlgID,hParentWnd,CptMultipleLanguage::GetInstance()->GetResourceHandle()) 
{
	m_bDrawEdge = true ;
	m_bDrawTitleBar = true ;
	m_bDragMove = true ;
}

CSkinDialog::~CSkinDialog(void)
{
}

BOOL CSkinDialog::OnInitDialog()
{//
	/**
	LONG lStyle = WS_POPUP ;

	lStyle = ::GetWindowLong(this->GetSafeHwnd(),GWL_STYLE) ;

	lStyle &= (~WS_OVERLAPPED) ;
	lStyle |= WS_POPUP ;
	//lStyle = lStyle&(~WS_BORDER ) ;

	::SetWindowLong(this->GetSafeHwnd(),GWL_STYLE,lStyle) ;

	lStyle = ::GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE) ;

	lStyle = lStyle&(~WS_EX_DLGMODALFRAME) ;
	lStyle = lStyle&(~WS_EX_CLIENTEDGE) ;
	lStyle = lStyle&(~WS_EX_WINDOWEDGE) ;
	lStyle = lStyle&(~WS_EX_STATICEDGE) ;

	::SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,lStyle) ;
/**/

	
	//RECT rtWin ;


	//this->DrawEdge() ;
    //CRgn   rg;  
    //rg.CreateRoundRectRgn(rt.left,   rt.top,   rt.right,   rt.bottom,   55,   55);  
    //this->SetWindowRgn(rg,   FALSE); 

	//HDC hDC = ::GetDC(this->GetSafeHwnd()) ;

	//::DrawEllipse
	////RECT rtClient ;

	////::GetClientRect(this->GetSafeHwnd(),&rtClient) ;

	////RECT rtTitle = {0,0,rtClient.right,22} ;

	////::DrawCaption(this->GetSafeHwnd(),hDC,&rtTitle,
	//::DeleteDC(hDC) ;

	RECT   rt;

	//GetWindowRect(this->GetSafeHwnd(),&rtWin);  
	GetClientRect(this->GetSafeHwnd(),&rt);  


	HRGN hRgn = ::CreateRoundRectRgn(rt.left,rt.top,rt.right,rt.bottom,ROUND_EDGE_SIZE,ROUND_EDGE_SIZE) ;
	//::ValidateRgn(this->GetSafeHwnd(),hRgn) ;
	//::InvalidateRgn(this->GetSafeHwnd(),hRgn,TRUE) ;
	::SetWindowRgn(this->GetSafeHwnd(),hRgn,FALSE) ;
	::DeleteObject(hRgn) ;

	//
	//LONG lStyle = ::GetWindowLong(this->GetSafeHwnd(),GWL_STYLE)|DWL_MSGRESULT ;
	//::SetWindowLong(this->GetSafeHwnd(),GWL_STYLE,lStyle) ;
/**
	m_Shadow.Initialize(NULL) ;
	
	m_Shadow.Create(this->GetSafeHwnd(),CWndShadow::ES_ON) ;

	m_Shadow.SetSize(2) ;
	m_Shadow.SetSharpness(8) ;
	/**/

	//m_Shadow.SetPosition(0,0) ;
	//m_Shadow.SetColor(RGB(0,100,20)) ;
	//m_Shadow.SetDarkness() ;

	return TRUE ;
}

void CSkinDialog::SetDragMove(bool bDragMove)
{
	m_bDragMove = bDragMove ;
}

void CSkinDialog::SetWindowSize(const SptSize& size)
{
	SptRect rt ;

	::GetClientRect(this->GetSafeHwnd(),rt.GetRECTPointer()) ;

	::SetWindowRgn(this->GetSafeHwnd(),NULL,FALSE) ;

	::SetWindowPos(this->GetSafeHwnd(),HWND_TOP,0,0,size.nWidth,size.nHeight,SWP_NOZORDER|SWP_NOMOVE) ;

	HRGN hRgn = ::CreateRoundRectRgn(0,0,size.nWidth,size.nHeight,ROUND_EDGE_SIZE,ROUND_EDGE_SIZE) ;

	::SetWindowRgn(this->GetSafeHwnd(),hRgn,TRUE) ;

	::DeleteObject(hRgn) ;

	rt.nTop = rt.GetHeight()-3 ;
	rt.SetHeight(3) ;

	::InvalidateRect(this->GetSafeHwnd(),NULL,TRUE) ;

	this->OnPaint() ;
}

//void CSkinDialog::OnControlStateChanged(int nControlID,EControlState NewState) 
//{
//}

void CSkinDialog::OnPaint()
{
	if(m_bDrawTitleBar)
	{
		this->DrawTitleBar() ;
	}

	if(m_bDrawEdge)
	{
		this->DrawEdge() ;
	}
	
}

void CSkinDialog::SetDrawing(bool bDrawEdge,bool bDrawTitelBar)
{
	m_bDrawTitleBar = bDrawTitelBar ;
	m_bDrawEdge = bDrawEdge ;

	::InvalidateRect(this->GetSafeHwnd(),NULL,TRUE) ;
	::UpdateWindow(this->GetSafeHwnd()) ;
}

BOOL CSkinDialog::OnEraseBkgnd(HDC hDC)
{
	SptRect rt ;

	::GetClientRect(this->GetSafeHwnd(),rt.GetRECTPointer()) ;

	::FillRect(hDC,rt.GetRECTPointer(),(HBRUSH)::GetSysColorBrush(COLOR_BTNFACE)) ;
	//::FillRect(hDC,rt.GetRECTPointer(),CXCConfiguration::GetInstance()->GetGlobalData()->hDlgBkBrush) ;

	this->OnPaint() ;

	return TRUE ;
}

void CSkinDialog::DrawEdge()
{
	HDC hDC = ::GetDC(this->GetSafeHwnd()) ;

	RECT rt ;

	::GetClientRect(this->GetSafeHwnd(),&rt) ;

	HPEN hPen = ::CreatePen(PS_SOLID,2,RGB(83,128,165)) ;
	
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC,::GetStockObject(NULL_BRUSH)) ;
	HPEN hOldPen = (HPEN)::SelectObject(hDC,hPen) ;

	::RoundRect(hDC,rt.left,rt.top,rt.right,rt.bottom,ROUND_EDGE_SIZE+5,ROUND_EDGE_SIZE+5) ;

	::SelectObject(hDC,hOldPen) ; 
	::SelectObject(hDC,hOldBrush) ; 

	::DeleteObject(hPen) ;
	//::DrawEdge(hDC,&rt,BDR_SUNKENOUTER,BF_DIAGONAL) ;

	//::DeleteDC(hDC) ;
	::ReleaseDC(this->GetSafeHwnd(),hDC) ;
}

void CSkinDialog::DrawTitleBar()
{
	HDC hDC = ::GetDC(this->GetSafeHwnd()) ;

	RECT rtClient ;

	::GetClientRect(this->GetSafeHwnd(),&rtClient) ;

	RECT rtTitle = {0,0,rtClient.right-rtClient.left,22} ;

	//HINSTANCE hInstance = ::GetModuleHandle(NULL) ;

	HBITMAP hBitmap = CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_TITLE) ;
	//HBITMAP hBitmap = ::LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP_TITLE)) ;

	HBRUSH hBrush = ::CreatePatternBrush(hBitmap) ;

	::FillRect(hDC,&rtTitle,hBrush) ;

	HICON hIcon = CptMultipleLanguage::GetInstance()->GetIcon(IDI_SMALL) ;//::LoadIcon(hInstance,MAKEINTRESOURCE(IDI_SMALL)) ;

	::DrawIconEx(hDC,8,2,hIcon,20,20,0,NULL,DI_NORMAL);

	//::DestroyIcon(hIcon) ;
	//::DrawIcon(hDC,8,0,hIcon) ;

	TCHAR szBuf[128] = {0} ;
//	if(!m_strTitle.IsEmpty())
	{
		if(::GetWindowText(this->GetSafeHwnd(),szBuf,sizeof(szBuf)/sizeof(TCHAR))>0)
		{
			int nOldMode = ::SetBkMode(hDC,TRANSPARENT) ;
			//int nOldMode = ::GetBkMode(hDC) ;
			::TextOut(hDC,32,2,szBuf,(int)::_tcslen(szBuf)) ;
			::SetBkMode(hDC,nOldMode) ;
		}
	}

	::DestroyIcon(hIcon) ;
	::DeleteObject(hBrush) ;
	::DeleteObject(hBitmap) ;

	//::DeleteDC(hDC) ;
	::ReleaseDC(this->GetSafeHwnd(),hDC) ;
}

int CSkinDialog::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	//Debug_Printf(_T("CSkinDialog::OnProcessMessage() hwnd=%p msg=%x wParam=%x lParam=%x"),hWnd,uMsg,wParam,lParam) ;

	switch(uMsg)
	{
	case 0x00AE: //:WM_NCUAHDRAWCAPTION //彻底解决最大最小关闭按钮依然显示的问题:
	case 0x00AF://:WM_NCUAHDRAWFRAME
		return WM_NCPAINT;

	case WM_SHOWWINDOW : 
		::AnimateWindow(hWnd,1,AW_BLEND) ;// 为了窗体的阴影效果而存在的代码
		::InvalidateRect(hWnd,NULL,TRUE) ;
		::UpdateWindow(hWnd) ;
		return 0 ;

	case WM_LBUTTONDOWN:

		if(m_bDragMove)
		{
			this->SendMessage(WM_SYSCOMMAND, SC_MOVE|0x0002,NULL) ;
		}

		return 1 ;

	//case WM_WINDOWPOSCHANGING:
	//	return 1 ;

	//case WM_SYSCOMMAND:
	//	return 0 ;

	case WM_SIZE:
		{
			
		}
		//Debug_Printf(_T("width=%d height=%d"),LOWORD(lParam),HIWORD(lParam)) ;
		break ;

	//case WM_NCACTIVATE:
	////case WM_SETTEXT:
	//case WM_NCLBUTTONUP:
	//case WM_NCMOUSEMOVE:
	//	break;
	case WM_NCPAINT:
		{
			/**
			HWND hWnd = this->GetSafeHwnd() ;
			HDC hdc = GetWindowDC(hWnd); // Paint into this DC 
			RECT rcWin; 
			GetWindowRect(hWnd, &rcWin); 
			OffsetRect( &rcWin, -rcWin.left, -rcWin.top ); 

			HBRUSH hBrushFrame = ::CreateSolidBrush(RGB(181,211,255)) ;
			HBITMAP hBitmap = ::LoadBitmap(::GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP_TITLE)) ;
			HBRUSH hBrush = ::CreatePatternBrush(hBitmap) ;

			//for(int i=0; i<4; i++) 
			{ 
				::FrameRect(hdc, &rcWin, hBrushFrame);
				::InflateRect(&rcWin, -1, -1); 

				
				//::InflateRect(&rcWin, -1, -1); 
			} 

			RECT rtTitle = {rcWin.left,rcWin.top,rcWin.right,rcWin.top+30} ;
				::FillRect(hdc,&rtTitle,hBrush) ;

			::DeleteObject(hBrush) ;
			::DeleteObject(hBrushFrame) ;
			::DeleteObject(hBitmap) ;
			ReleaseDC(hWnd, hdc);

			//return 0 ;
			
			/**
			//HDC hdc = ::GetDCEx(this->GetSafeHwnd(), (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);
			HDC hdc = ::GetDCEx(this->GetSafeHwnd(), (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);
			// Paint into this DC

			RECT rt  ;
			rt.top = -300 ;
			rt.left = -300 ;
			rt.bottom = 300 ;
			rt.right = 300 ;
			HBRUSH hBrush = ::CreateSolidBrush(RGB(255,0,0)) ;
			::FillRect(hdc,&rt,hBrush) ;
			::DeleteObject(hBrush) ;

			::ReleaseDC(this->GetSafeHwnd(), hdc);
			/**/
			//this->DrawTitleBar() ;
		}
		//return FALSE ;
		break ;
	}

	return CptDialog::OnProcessMessage(hWnd,uMsg,wParam,lParam) ;
}