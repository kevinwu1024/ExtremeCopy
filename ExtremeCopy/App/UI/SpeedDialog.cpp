/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/
#include "StdAfx.h"
#include "SpeedDialog.h"
#include "..\Language\XCRes_ENU\resource.h"
#include "..\..\Common\ptTypeDef.h"
#include <CommCtrl.h>
#include "..\XCConfiguration.h"


CSpeedDialog::CSpeedDialog(bool bMove,const SRunTimeSettingDlgOption& optoin,CXCSettingChangingCB* pCB,HWND hParentWnd):CSkinDialog(IDD_DIALOG_SPEED,hParentWnd)//,m_pModelessDlg(NULL)
{
	_ASSERT(pCB!=NULL) ;

	m_bShow = false ;
	m_bCursorHover = false ;
	m_bControlHover = false ;
	m_bMove = bMove ;
	//m_SettingOption.bIsVerify = bMove ?  false : bVerifyData ;

	m_pSettingChangingCB = pCB ;

	//m_SettingOption.nSpeed = 0 ;

	m_SettingOption = optoin ;
}

CSpeedDialog::~CSpeedDialog(void)
{
}


void CSpeedDialog::UpdatePosition() 
{
	HWND hParentWnd = this->GetParentHwnd() ;

	SptRect rect ;
	::GetWindowRect(hParentWnd,rect.GetRECTPointer()) ; 
	
	SptRect rect2 ;
	::GetWindowRect(this->GetSafeHwnd(),rect2.GetRECTPointer()) ;

	int nX = rect.nLeft - rect2.GetWidth()  ;
	int nY = rect.nTop + 17 ;

	::SetWindowPos(this->GetSafeHwnd(),HWND_TOP,nX,nY,0,0,SWP_NOSIZE|SWP_NOZORDER) ;
}


void CSpeedDialog::OnTimer(int nTimerID)
{
	if(nTimerID==TIMER_DELAYHIDE && !m_bCursorHover)
	{
		this->HideAnimate() ;
	}

	this->KillTimer(nTimerID) ;
}

int CSpeedDialog::ShowAnimate() 
{
	//if(m_pModelessDlg!=NULL)
	//{
	//	m_pModelessDlg = this->GetModeless() ;
	//}
	HWND hWnd = this->GetSafeHwnd() ;

	if(hWnd!=NULL)
	{
		this->UpdatePosition() ;

		::AnimateWindow(hWnd,300,AW_BLEND) ;

		m_bShow = true ;
	}

	return 0 ;
}

void CSpeedDialog::HideAnimate() 
{
	HWND hWnd = this->GetSafeHwnd() ;

	if(hWnd!=NULL)
	{
		::AnimateWindow(hWnd,400,AW_BLEND|AW_HIDE) ;
		m_bShow = false ;
	}
}

void CSpeedDialog::DelayHide(int nDelayTime) 
{
	this->SetTimer(TIMER_DELAYHIDE,nDelayTime) ;
}

bool CSpeedDialog::IsShow() const
{
	return m_bShow ;
}

BOOL CSpeedDialog::OnInitDialog() 
{
	HWND hTrackWnd = this->GetDlgItem(IDC_SLIDER_SPEED) ;

	m_SkinTrackBar.Attach(hTrackWnd) ;

	m_SkinTrackBar.SetRange(10,100) ;
	m_SkinTrackBar.SetPos(100) ;

	m_SettingOption.nSpeed = this->CaculateSpeedValue() ;

	this->UpdateSpeedText() ;

	if(m_bMove)
	{
		::EnableWindow(this->GetDlgItem(IDC_CHECK_VERIFYDATA),FALSE) ;
	}
	else
	{
		HWND hVerifyWnd = this->GetDlgItem(IDC_CHECK_VERIFYDATA) ;
		m_VerifyCheckBox.Attach(hVerifyWnd) ;

		::SendMessage(this->GetDlgItem(IDC_CHECK_VERIFYDATA),BM_SETCHECK,m_SettingOption.bIsVerify?BST_CHECKED:BST_UNCHECKED,0) ;
	}

	::SendMessage(this->GetDlgItem(IDC_CHECK_CLOSEWINDOW),BM_SETCHECK,m_SettingOption.bIsCloseWindow?BST_CHECKED:BST_UNCHECKED,0) ;
	::SendMessage(this->GetDlgItem(IDC_CHECK_SHUTDOWN),BM_SETCHECK,m_SettingOption.bIsShutdown?BST_CHECKED:BST_UNCHECKED,0) ;

	this->SetDrawing(true,false) ;

	return CSkinDialog::OnInitDialog() ;
}

int CSpeedDialog::CaculateSpeedValue() const
{
	//return 100 - m_SkinTrackBar.GetPos() ;
	return m_SkinTrackBar.GetPos() ;
}

/**
void CSpeedDialog::DrawEdge()
{
	HDC hDC = ::GetDC(this->GetSafeHwnd()) ;

	RECT rt ;

	::GetClientRect(this->GetSafeHwnd(),&rt) ;

	HPEN hPen = ::CreatePen(PS_SOLID,2,RGB(83,128,165)) ;

	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC,::GetStockObject(NULL_BRUSH)) ;
	HPEN hOldPen = (HPEN)::SelectObject(hDC,hPen) ;

	::RoundRect(hDC,rt.left,rt.top,rt.right,rt.bottom,8+5,8+5) ;

	::SelectObject(hDC,hOldPen) ; 
	::SelectObject(hDC,hOldBrush) ; 

	::DeleteObject(hPen) ;
	//::DrawEdge(hDC,&rt,BDR_SUNKENOUTER,BF_DIAGONAL) ;

	//::DeleteDC(hDC) ;
	::ReleaseDC(this->GetSafeHwnd(),hDC) ;
}

void CSpeedDialog::OnPaint()
{
	//Debug_Printf(_T("CSpeedDialog::OnPaint()")) ;
	this->DrawEdge() ;
}

BOOL CSpeedDialog::OnEraseBkgnd(HDC hDC)
{
	SptRect rt ;

	::GetClientRect(this->GetSafeHwnd(),rt.GetRECTPointer()) ;

	
	::FillRect(hDC,rt.GetRECTPointer(),(HBRUSH)::GetSysColorBrush(COLOR_BTNFACE)) ;

	this->OnPaint() ;

	//this->DrawEdge() ;
	return	TRUE ;
}
/**/

void CSpeedDialog::OnButtonClick(int nButtonID)
{
	switch(nButtonID)
	{
	case IDC_CHECK_VERIFYDATA: // verify data
		{
			_ASSERT(m_pSettingChangingCB!=NULL) ;

			if(m_pSettingChangingCB!=NULL)
			{
				m_SettingOption.bIsVerify = ::SendMessage(this->GetDlgItem(IDC_CHECK_VERIFYDATA),BM_GETCHECK,0,0)==BST_CHECKED ; ;
				m_SettingOption.uFlag = SRunTimeSettingDlgOption::RunTimeSettingOptionFlag_Verify ;

				m_pSettingChangingCB->OnRunTimeSettingChanged(m_SettingOption) ;
			}
		}
		break ;

	case IDC_CHECK_CLOSEWINDOW: // 关闭窗口
		{
			_ASSERT(m_pSettingChangingCB!=NULL) ;

			m_SettingOption.bIsCloseWindow = ::SendMessage(this->GetDlgItem(IDC_CHECK_CLOSEWINDOW),BM_GETCHECK,0,0)==BST_CHECKED  ;
			m_SettingOption.uFlag = SRunTimeSettingDlgOption::RunTimeSettingOptionFlag_CloseWindow ;

			m_pSettingChangingCB->OnRunTimeSettingChanged(m_SettingOption) ;
		}
		break ;

	case IDC_CHECK_SHUTDOWN: // 关机
		{
			_ASSERT(m_pSettingChangingCB!=NULL) ;

			m_SettingOption.bIsShutdown = ::SendMessage(this->GetDlgItem(IDC_CHECK_SHUTDOWN),BM_GETCHECK,0,0)==BST_CHECKED  ;
			m_SettingOption.uFlag = SRunTimeSettingDlgOption::RunTimeSettingOptionFlag_Shutdown ;

			m_pSettingChangingCB->OnRunTimeSettingChanged(m_SettingOption) ;
		}
		break ;
	}
}

int CSpeedDialog::GetSpeedValue() const
{
	return m_SettingOption.nSpeed ;
}

void CSpeedDialog::UpdateSpeedText() 
{
	TCHAR szBuf[32] = {0} ;

#pragma warning(push)
#pragma warning(disable:4996)
	::_stprintf (szBuf,_T("%d%%"),m_SettingOption.nSpeed) ;
#pragma warning(pop)

	this->SetDlgItemText(IDC_STATIC_SPEED,szBuf) ;
}

int CSpeedDialog::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_HSCROLL:
	case WM_VSCROLL:
		{// 滑块的值发生变化
			if(IDC_SLIDER_SPEED==::GetDlgCtrlID((HWND)lParam))
			{
				m_SettingOption.nSpeed = this->CaculateSpeedValue() ;
				this->UpdateSpeedText() ;

				if(m_pSettingChangingCB!=NULL)
				{
					m_SettingOption.uFlag = SRunTimeSettingDlgOption::RunTimeSettingOptionFlag_Speed ;
					m_pSettingChangingCB->OnRunTimeSettingChanged(m_SettingOption) ;
				}
			}
		}
		break ;

	//case WM_NOTIFY:
	//	{
	//		//NMTRBTHUMBPOSCHANGING*  pNMHDR = (NMTRBTHUMBPOSCHANGING*)lParam; 
	//		NMCUSTOMDRAW *pNMHDR = (NMCUSTOMDRAW*)lParam;
	//		
	//		if(pNMHDR!=NULL && pNMHDR->hdr.code==NM_CUSTOMDRAW && wParam==IDC_SLIDER_SPEED)
	//		{
	//			Debug_Printf(_T("position: %d"),m_SkinTrackBar.GetPos()) ;
	//			this->UpdateSpeedText() ;
	//		}
	//		//int aa = 0 ;
	//	}
	//	break ;

	case WM_MOUSEMOVE:
		if(!m_bCursorHover || !m_bControlHover)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = this->GetSafeHwnd();
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 1;

			::TrackMouseEvent(&tme);
		}
		break ;

	case WM_CONTROLSTATUSCHANGED:
		//if(m_bControlHover)
		{
			SControlStatus cs ;
			cs.uStatusBlock = (unsigned int)lParam ;

			switch(cs.status.MouseStatus)
			{
			case ControlMouseStatus_Hover:
				//Debug_Printf(_T("control WM_MOUSEHOVER")) ;
				m_bControlHover = true ;
				m_bCursorHover = true ;
				break ;

			case ControlMouseStatus_Leave:
				m_bCursorHover = false ;
				m_bControlHover = false ;
				//Debug_Printf(_T("control WM_MOUSELEAVE")) ;
				this->DelayHide() ;
				break ;
			}
		}
		break ;

	//case WM_NCMOUSEHOVER:
	case WM_MOUSEHOVER:
		//Debug_Printf(_T("WM_MOUSEHOVER")) ;
		//Debug_Printf(_T("dialog WM_MOUSEHOVER")) ;
		m_bCursorHover = true ;
		m_bControlHover = false ;
		this->Show() ;
		//this->ShowAnimate() ;
		break ;

	//case WM_NCHITTEST:
	//	{
	//		int nX = 0 ;
	//		int nY = 0 ;

	//		nX = LOWORD(lParam) ;
	//		nY = HIWORD(lParam) ;

	//		Debug_Printf(_T("x:%d y:%d"),nX,nY) ;
	//	}
	//	break ;

	case WM_MOUSELEAVE:
	//case WM_NCMOUSELEAVE:
		//Debug_Printf(_T("dialog WM_MOUSELEAVE 1")) ;
		if(!m_bControlHover)
		{
			//Debug_Printf(_T("dialog WM_MOUSELEAVE 2")) ;
			m_bCursorHover = false ;
			this->DelayHide() ;
		}

		break ;

	//case WM_CTLCOLORBTN :
	//case WM_CTLCOLOREDIT :
	//case WM_CTLCOLORDLG :
	//case WM_CTLCOLORSTATIC :
	//case WM_CTLCOLORSCROLLBAR :
	//case WM_CTLCOLORLISTBOX :
	//case WM_CTLCOLORMSGBOX:
	//	{
	//		HDC hdc = (HDC)wParam ;

	//		::SetBkMode(hdc,TRANSPARENT) ;

	//		if(uMsg==WM_CTLCOLORSTATIC)
	//		{
	//			::SetTextColor(hdc,UI_TEXT_COLOR) ;
	//		}

	//		return (int)CXCConfiguration::GetInstance()->GetGlobalData()->hDlgBkBrush ;
	//	}
	//	break ;
	}

	return CSkinDialog::OnProcessMessage(hWnd,uMsg,wParam,lParam) ;
}


// IDB_BITMAP_TRACKBAR