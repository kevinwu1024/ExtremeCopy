/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "RegisterCountdownDlg.h"
#include "..\Language\XCRes_ENU\Resource.h"
#include "..\XCConfiguration.h"
/*
CRegisterCountdownDlg::CRegisterCountdownDlg(void):CptDialog(IDD_DIALOG_REGISTERCOUNTDOWN,NULL,CptMultipleLanguage::GetInstance()->GetResourceHandle())
{
}

CRegisterCountdownDlg::~CRegisterCountdownDlg(void)
{
}

BOOL CRegisterCountdownDlg::OnInitDialog() 
{
	//HBITMAP hBitmapNormal = CXCConfiguration::GetInstance()->GetGlobalData()->hBitmapNormal ;
	//HBITMAP hBitmapHover = CXCConfiguration::GetInstance()->GetGlobalData()->hBitmapHover ;
	//HBITMAP hBitmapDown = CXCConfiguration::GetInstance()->GetGlobalData()->hBitmapDown ;
	//HBITMAP hBitmapDisable = CXCConfiguration::GetInstance()->GetGlobalData()->hBitmapDisable ;

	m_CancelSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_CANCEL)) ;
	//m_CancelSkinButton.SetBkBitmap(hBitmapNormal,hBitmapHover,hBitmapDown,hBitmapDisable) ;

	m_InputRegisterSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_INPUTREGISTER)) ;
	//m_InputRegisterSkinButton.SetBkBitmap(hBitmapNormal,hBitmapHover,hBitmapDown,hBitmapDisable) ;

	m_GoToRegisterSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_GOTOREGISTER)) ;
	//m_GoToRegisterSkinButton.SetBkBitmap(hBitmapNormal,hBitmapHover,hBitmapDown,hBitmapDisable) ;

	//m_CloseSkinButton.SetHandle(this->GetDlgItem(IDC_BUTTON_TITLECLOSE)) ;
	//m_CloseSkinButton.SetBkBitmap(CXCConfiguration::GetInstance()->GetGlobalData()->hCloseBitmapNormal,CXCConfiguration::GetInstance()->GetGlobalData()->hCloseBitmapHover,CXCConfiguration::GetInstance()->GetGlobalData()->hCloseBitmapDown) ;

	m_nRemainTime =  4 ;

	this->SetTimer(COUNTDOWN_TIMER,1*1000) ;

	this->OnTimer(COUNTDOWN_TIMER) ;

	this->CenterScreen() ;

	return CptDialog::OnInitDialog() ;
}

void CRegisterCountdownDlg::OnPaint() 
{
	CptDialog::OnPaint() ;

	//m_CloseSkinButton.Update() ;
}

//bool CRegisterCountdownDlg::OnCancel() 
//{
//}

void CRegisterCountdownDlg::OnButtonClick(int nButtonID) 
{
	switch(nButtonID)
	{
	case IDC_BUTTON_REGISTER:
		break ;

//	case IDC_BUTTON_TITLECLOSE:
	case IDC_BUTTON_CANCEL:
		this->Close() ;
		break ;
	}

	if(nButtonID==IDC_BUTTON_REGISTER)
	{// 注册
	}
}

void CRegisterCountdownDlg::OnTimer(int nTimerID)
{
	if(nTimerID==COUNTDOWN_TIMER)
	{
		CptString strText ;
		strText.Format(_T("%d seconds"),m_nRemainTime) ;

		this->SetDlgItemText(IDC_STATIC_COUNTDOWN,strText.c_str()) ;

		if(--m_nRemainTime<0)
		{
			this->KillTimer(COUNTDOWN_TIMER) ;
			this->m_Result = DialogResult_OK ;
			this->Close() ;
		}
	}
}

int CRegisterCountdownDlg::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	//switch(uMsg)
	//{

	//case WM_CTLCOLORBTN :
	//case WM_CTLCOLOREDIT :
	//case WM_CTLCOLORDLG :
	//case WM_CTLCOLORSTATIC :
	//case WM_CTLCOLORSCROLLBAR :
	//case WM_CTLCOLORLISTBOX :
	//	{
	//		HDC hdc = (HDC)wParam ;

	//		::SetBkMode(hdc,TRANSPARENT) ;
	//		
	//		//HBRUSH hBrush = ::CreateSolidBrush(RGB(113,204,255)) ;
	//		//HBRUSH hBrush = ::CreateSolidBrush(RGB(181,211,255)) ;

	//		//if((HWND)lParam==this->GetDlgItem(IDC_STATIC_CURRENTFILE))
	//		//{// 当前复制的文件
	//		//	//HFONT hFont = ::CreateFont(10,0,0,0,FW_BOLD,TRUE,FALSE,FALSE,DEFAULT_CHARSET,0,
	//		//	//	CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;
	//		//	::SelectObject(hdc,m_hCurFileFont) ;
	//		//}
	//		//else if((HWND)lParam==this->GetDlgItem(IDC_STATIC_TO))
	//		//{
	//		//	::SelectObject(hdc,m_hToTextFont) ;
	//		//}
	//		//else 
	//		if(uMsg==WM_CTLCOLORSTATIC)
	//		{
	//			::SetTextColor(hdc,RGB(10,10,230)) ;
	//		}

	//		return (int)CXCConfiguration::GetInstance()->GetGlobalData()->hDlgBkBrush ;
	//	}
	//	break ;
	//}

	return CptDialog::OnProcessMessage(hWnd,uMsg,wParam,lParam) ;
}
*/