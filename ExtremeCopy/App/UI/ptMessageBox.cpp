/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptMessageBox.h"
#include "..\Language\XCRes_ENU\Resource.h"
#include "..\XCConfiguration.h"
#include "..\ptMultipleLanguage.h"

CptMessageBox::CptMessageBox(int nDlgID,HWND hParentWnd):CptDialog(nDlgID,hParentWnd,CptMultipleLanguage::GetInstance()->GetResourceHandle())
{
}

CptMessageBox::~CptMessageBox(void)
{
}

void CptMessageBox::OnPaint() 
{
	CptDialog::OnPaint() ;

	m_CloseSkinButton.Update() ;
}

BOOL CptMessageBox::OnInitDialog() 
{
	this->CenterParentWindow() ;
	
	this->SetDlgItemText(IDC_STATIC_TEXT,m_strText.c_str()) ;

	::SetWindowText(this->GetSafeHwnd(),m_strTitle.c_str()) ;

	m_CloseSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_TITLECLOSE2)) ;

	switch(m_ButtonType)
	{
	case Button_OK:
		{
			m_SkinButton1.Attach(this->GetDlgItem(IDC_BUTTON_MSGBOX_OK)) ;
			::ShowWindow(this->GetDlgItem(IDC_BUTTON_MSGBOX_YES),SW_HIDE) ;
			::ShowWindow(this->GetDlgItem(IDC_BUTTON_MSGBOX_NO),SW_HIDE) ;
		}
		break ;

	case Button_YesNo:
		{
			m_SkinButton1.Attach(this->GetDlgItem(IDC_BUTTON_MSGBOX_YES)) ;
			m_SkinButton2.Attach(this->GetDlgItem(IDC_BUTTON_MSGBOX_NO)) ;
			::ShowWindow(this->GetDlgItem(IDC_BUTTON_MSGBOX_OK),SW_HIDE) ;
		}
		break ;

	case Button_IgnoreCancel:
	case Button_RetryCancel:
	case Button_RetryIgnoreCancel:
		{
			m_SkinButton1.Attach(this->GetDlgItem(IDC_BUTTON_MSGBOX_RETRY)) ;
			m_SkinButton3.Attach(this->GetDlgItem(IDC_BUTTON_MSGBOX_CANCEL)) ;
			m_SkinButton2.Attach(this->GetDlgItem(IDC_BUTTON_MSGBOX_IGNORE)) ;

			if(m_ButtonType==Button_RetryCancel)
			{
				HWND hWnd = m_SkinButton2.GetSaftHwnd() ;

				::ShowWindow(hWnd,SW_HIDE) ;
			}

			if(m_ButtonType==Button_IgnoreCancel)
			{
				HWND hWnd = m_SkinButton1.GetSaftHwnd() ;

				::ShowWindow(hWnd,SW_HIDE) ;
			}
		}
		break ;
	}

	return CptDialog::OnInitDialog() ;
}

CptMessageBox::EMsgResult CptMessageBox::ShowMessage(HWND hParentWnd,CptString strText,CptString strTitle,EButton button)
{
	int nID = 0 ;

	switch(button)
	{
	case Button_OK:
	case Button_YesNo:
		nID = IDD_DIALOG_MESSAGEBOX ;
		break ;

	case Button_RetryIgnoreCancel:
		nID = IDD_DIALOG_MESSAGEBOX_COPYERROROPER ;
		break ;

	case Button_IgnoreCancel:
	case Button_RetryCancel:
		nID = IDD_DIALOG_MESSAGEBOX_COPYERROROPER ;
		break ;
	}

	CptMessageBox dlg(nID,hParentWnd) ;
//
	return dlg.ShowMessage(strText,strTitle,button) ;
}

CptMessageBox::EMsgResult CptMessageBox::ShowMessage(CptString strText,CptString strTitle,EButton button)
{
	m_ButtonType = button ;
	m_strText = strText ;
	m_strTitle = strTitle ;

	this->ShowDialog() ;

	return m_Result ;
}

void CptMessageBox::OnButtonClick(int nButtonID) 
{
	HWND hParent = this->GetParentHwnd() ;

	switch(nButtonID)
	{
	case IDC_BUTTON_MSGBOX_OK:
	case IDC_BUTTON_MSGBOX_YES:
		m_Result = MsgResult_Yes ;
		break ;

	case IDC_BUTTON_MSGBOX_NO:
		m_Result = MsgResult_No ;
		
		break ;

	case IDC_BUTTON_TITLECLOSE2:
	case IDC_BUTTON_MSGBOX_CANCEL:
		m_Result = MsgResult_Cancel ;
		break ;

	case IDC_BUTTON_MSGBOX_IGNORE:
		m_Result = MsgResult_Ignore ;
		break ;

	case IDC_BUTTON_MSGBOX_RETRY:
		m_Result = MsgResult_Retry ;
		break ;
	}

	this->PerformButton(IDCANCEL) ;
}

int CptMessageBox::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CptDialog::OnProcessMessage(hWnd,uMsg, wParam, lParam) ;
}
