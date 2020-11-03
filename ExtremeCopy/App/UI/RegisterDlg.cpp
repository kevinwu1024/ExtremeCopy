/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "RegisterDlg.h"
#include "..\Language\XCRes_ENU\Resource.h"
#include "..\XCConfiguration.h"

CRegisterDlg::CRegisterDlg(HWND hParentWnd):CptDialog(IDD_DIALOG_REGISTER,hParentWnd,CptMultipleLanguage::GetInstance()->GetResourceHandle())
{
}

CRegisterDlg::~CRegisterDlg(void)
{
}

BOOL CRegisterDlg::OnInitDialog() 
{
	CptString strTitle = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_REGISTER) ;
	::SetWindowText(this->GetSafeHwnd(),strTitle.c_str()) ;

	HYPERLINKSTRUCT hls = {0};

	hls.bBold     = false;				// Bold font?
	hls.bUnderline = true;				// Underlined font?
	hls.bUseBg    = false;				// Use opaque background?
	hls.bUseCursor = true;				// Use custom cursor?
	hls.hCursor    = CXCConfiguration::GetInstance()->GetGlobalData()->hCursorHand ;//::LoadCursor( GetModuleHandle( NULL ), (LPCTSTR)IDC_CURSOR_HAND );
	hls.clrBack   = RGB(0,0,0);			// Fill this out if bUseBg is true
	hls.clrHilite = RGB( 255, 0, 0 );	// Color of mouseover font
	hls.clrText   = RGB( 0, 0, 255 );	// Color of regular font
	hls.coordX    = 130;					// X-axis coordinate of upper left corner
	hls.coordY    = 3;					// Y-axis coordinate of upper left corner
	hls.ctlID     = 0;					// Set this to use GetDlgItem on this control
	hls.fontsize  = 15;					// Size of font used
	hls.height    = 20;					// Height of control
	hls.hWndParent = this->GetSafeHwnd();              // Handle to parent window
	hls.pFn       = OnHyperLinkHomePageCallBack2;		// Function pointer to function that is called when link is clicked
	hls.pCBAgr = (void*)WEBLINK_SITE_HOME ;
	hls.szCaption = LINK_HOME_WEBSITE; // Caption
	hls.szFontName = _T("Arial");			// Font face name
	hls.width      = 130;				// Width of control

	m_Hyperlink.Create(&hls) ;

	// 指定每个一个编辑框最多 5 个字符，避免用户多输入不必要的字符
	const int nMaxLen = 5 ;
	::SendMessage(this->GetDlgItem(IDC_EDIT_NUMBER1), EM_SETLIMITTEXT, nMaxLen, 0);
	::SendMessage(this->GetDlgItem(IDC_EDIT_NUMBER2), EM_SETLIMITTEXT, nMaxLen, 0);
	::SendMessage(this->GetDlgItem(IDC_EDIT_NUMBER3), EM_SETLIMITTEXT, nMaxLen, 0);
	::SendMessage(this->GetDlgItem(IDC_EDIT_NUMBER4), EM_SETLIMITTEXT, nMaxLen, 0);
	/**/
#ifdef VERSION_PROFESSIONAL
	this->SetDlgItemText(IDC_STATIC_LICENCE,_T("")) ;
	
#else
	//this->SetDlgItemText(IDC_STATIC_LICENCE,_T("Free for home user")) ;
	//this->SetDlgItemText(IDC_STATIC_LICENCE,_T("")) ;
#endif
	/**/

	this->CenterScreen() ;

	return CptDialog::OnInitDialog() ;
}

void CRegisterDlg::OnPaint() 
{
	CptDialog::OnPaint() ;

//	m_CloseSkinButton.Update() ;
}

//bool CRegisterDlg::OnCancel() 
//{
//}

void CRegisterDlg::OnButtonClick(int nButtonID) 
{
	switch(nButtonID)
	{
	case IDC_BUTTON_REGISTER:// 注册
		this->Register() ;

//	case IDC_BUTTON_TITLECLOSE:
	case IDC_BUTTON_CANCEL:
		this->Close() ;
		break ;

	case IDC_BUTTON_BUY: // 购买

#ifdef VERSION_PROFESSIONAL
		::OpenLink(WEBLINK_BUY_PROFESSIONAL) ;
#else
		::OpenLink(WEBLINK_BUY_STANDARD) ;
#endif
		break ;
	}

	//if(nButtonID==IDC_BUTTON_REGISTER)
	//{// 注册
	//}
}

void CRegisterDlg::Register() 
{
	//CptString strNum1,
	//CptStringList strNumList ;
	TCHAR szBuf[512] = {0} ;
	m_strSeriesNumList.Clear() ;

	this->GetDlgItemText(IDC_EDIT_NUMBER1,szBuf,sizeof(szBuf)) ;
	m_strSeriesNumList.Add(szBuf) ;

	szBuf[0] = 0 ;
	this->GetDlgItemText(IDC_EDIT_NUMBER2,szBuf,sizeof(szBuf)) ;
	m_strSeriesNumList.Add(szBuf) ;

	szBuf[0] = 0 ;
	this->GetDlgItemText(IDC_EDIT_NUMBER3,szBuf,sizeof(szBuf)) ;
	m_strSeriesNumList.Add(szBuf) ;

	szBuf[0] = 0 ;
	this->GetDlgItemText(IDC_EDIT_NUMBER4,szBuf,sizeof(szBuf)) ;
	m_strSeriesNumList.Add(szBuf) ;

}

int CRegisterDlg::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch(uMsg)
	{
	case WM_CTLCOLORBTN :
	case WM_CTLCOLOREDIT :
	case WM_CTLCOLORDLG :
	case WM_CTLCOLORSTATIC :
	case WM_CTLCOLORSCROLLBAR :
	case WM_CTLCOLORLISTBOX :
		{
			HDC hdc = (HDC)wParam ;

			::SetBkMode(hdc,TRANSPARENT) ;
			
			//HBRUSH hBrush = ::CreateSolidBrush(RGB(113,204,255)) ;
			//HBRUSH hBrush = ::CreateSolidBrush(RGB(181,211,255)) ;

			//if((HWND)lParam==this->GetDlgItem(IDC_STATIC_CURRENTFILE))
			//{// 当前复制的文件
			//	//HFONT hFont = ::CreateFont(10,0,0,0,FW_BOLD,TRUE,FALSE,FALSE,DEFAULT_CHARSET,0,
			//	//	CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;
			//	::SelectObject(hdc,m_hCurFileFont) ;
			//}
			//else if((HWND)lParam==this->GetDlgItem(IDC_STATIC_TO))
			//{
			//	::SelectObject(hdc,m_hToTextFont) ;
			//}
			//else 
			if(uMsg==WM_CTLCOLORSTATIC)
			{
				if((HWND)lParam==this->GetDlgItem(IDC_STATIC_LICENCE))
				{
					::SetTextColor(hdc,RGB(230,10,10)) ;
				}
				//else
				//{
				//	::SetTextColor(hdc,RGB(10,10,230)) ;
				//}
				
			}
			return (int)::GetSysColorBrush(COLOR_BTNFACE) ;
			//return (int)::GetStockObject(NULL_BRUSH) ;
			//return (int)CXCConfiguration::GetInstance()->GetGlobalData()->hDlgBkBrush ;
		}
		break ;
	}

	return CptDialog::OnProcessMessage(hWnd,uMsg,wParam,lParam) ;
}