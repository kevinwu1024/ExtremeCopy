/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "..\Language\XCRes_ENU\resource.h"
#include "XCStdLaunchDlg.h"
#include "..\ptMultipleLanguage.h"
#include "..\XCGlobal.h"
#include "..\XCConfiguration.h"

void XCRunModeConfiguration() ;


CXCStdLaunchDlg::CXCStdLaunchDlg(int nDlgID):CptDialog(nDlgID,NULL,CptMultipleLanguage::GetInstance()->GetResourceHandle())
{
}

CXCStdLaunchDlg::~CXCStdLaunchDlg(void)
{
}


BOOL CXCStdLaunchDlg::OnInitDialog() 
{
	this->CenterScreen() ;

	{
		HYPERLINKSTRUCT2 hls = {0};

		hls.fontsize  = 14;					// Size of font used
		hls.bBold     = false;				// Bold font?
		hls.bUnderline = true;				// Underlined font?
		hls.bUseBg    = false;				// Use opaque background?
		hls.bUseCursor = true;				// Use custom cursor?
		hls.hCursor    = CXCConfiguration::GetInstance()->GetGlobalData()->hCursorHand ;//::LoadCursor( GetModuleHandle( NULL ), (LPCTSTR)IDC_CURSOR_HAND );
		hls.clrBack   = RGB(0,0,0);			// Fill this out if bUseBg is true
		hls.clrHilite = RGB( 0, 0, 255 );	// Color of mouseover font
		hls.clrText   = RGB( 0, 0, 255 );	// Color of regular font
		hls.hControlWnd = this->GetDlgItem(IDC_STATIC_GETPRO) ;

		hls.pFn       = OnHyperLinkHomePageCallBack2;		// Function pointer to function that is called when link is clicked
		hls.pCBAgr = (void*)WEBLINK_SITE_GETPROEDITION ;
		hls.szFontName = _T("Arial");			// Font face name

		m_Hyperlink.Create(&hls) ;
	}

	CptString strText = ::CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_STANDARDEDTIONNOMAINDIALOG) ;
	CptString strTitle = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;

	::SetWindowText(this->GetSafeHwnd(),strTitle.c_str()) ;

	this->SetDlgItemText(IDC_STATIC_TEXT,strText) ;

	return CptDialog::OnInitDialog() ;
}

void CXCStdLaunchDlg::OnPaint() 
{
	CptDialog::OnPaint() ;

#ifndef VERSION_PROFESSIONAL // 非专业版
	m_Hyperlink.Update() ;
#endif
}


void CXCStdLaunchDlg::OnButtonClick(int nButtonID) 
{
	switch(nButtonID)
	{
	case IDC_BUTTON_CONFIGURATION: // 配置
		::ShowWindow(this->GetSafeHwnd(),SW_HIDE) ;
		::XCRunModeConfiguration() ;
		//break ;

	case IDC_BUTTON_EXIT:// 退出
		this->PerformButton(IDCANCEL) ;
		break ;
	}
}
