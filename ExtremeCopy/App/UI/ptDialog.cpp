/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include <process.h>
#include "ptDialog.h"


static const TCHAR* pHwndPropThisEntry = _T("ptDialogBoxThisPointer") ;

CptDialog::SDlgListInfo CptDialog::s_DlgListInfo ;

#define WF_TOOLTIPS         0x0001  // window is enabled for tooltips
#define WF_TEMPHIDE         0x0002  // window is temporarily hidden
#define WF_STAYDISABLED     0x0004  // window should stay disabled
#define WF_MODALLOOP        0x0008  // currently in modal loop
#define WF_CONTINUEMODAL    0x0010  // modal loop should continue running
#define WF_OLECTLCONTAINER  0x0100  // some descendant is an OLE control
#define WF_TRACKINGTOOLTIPS 0x0400  // window is enabled for tracking tooltips

CptDialog::CptDialog(UINT uIDTemplate,HWND hParent,HMODULE hModule):m_hParentWnd(hParent),m_hWnd(NULL),m_nFlags(0),m_hModule(hModule)
{
	m_uTemplateID = uIDTemplate ;
	m_Result = DialogResult_Cancel ;
}

CptDialog::~CptDialog(void)
{
}

bool CptDialog::Create()
{
	bool bRet = false ;

	m_hWnd = ::CreateDialogParam(m_hModule,MAKEINTRESOURCE(m_uTemplateID),m_hParentWnd,
		(DLGPROC)CptDialog::DialogProc,(LPARAM)this) ;

	bRet = m_hWnd!=NULL ;

	return bRet ;
}

void CptDialog::GetScreenSize(int& pWidth, int& pHeight)
{
	HDC hDC;
	hDC = GetDC(0);
	pWidth = GetDeviceCaps(hDC, HORZRES);
	pHeight = GetDeviceCaps(hDC, VERTRES);
	ReleaseDC(0, hDC);
}


bool CptDialog::IsVisible() const 
{
	bool bVisible = false ;

	if(NULL!=m_hWnd)
	{
		bVisible = ::IsWindowVisible(m_hWnd) ? true : false ;
	}

	return bVisible ;
}

void CptDialog::SetWindowSize(const SptSize& size)
{
	SptRect rt ;

	::GetClientRect(this->GetSafeHwnd(),rt.GetRECTPointer()) ;

	::SetWindowRgn(this->GetSafeHwnd(),NULL,FALSE) ;

	::SetWindowPos(this->GetSafeHwnd(),HWND_TOP,0,0,size.nWidth,size.nHeight,SWP_NOZORDER|SWP_NOMOVE) ;

	/**
	HRGN hRgn = ::CreateRoundRectRgn(0,0,size.nWidth,size.nHeight,ROUND_EDGE_SIZE,ROUND_EDGE_SIZE) ;

	::SetWindowRgn(this->GetSafeHwnd(),hRgn,TRUE) ;

	::DeleteObject(hRgn) ;
	/**/

	rt.nTop = rt.GetHeight()-3 ;
	rt.SetHeight(3) ;

	::InvalidateRect(this->GetSafeHwnd(),NULL,TRUE) ;

	this->OnPaint() ;
}
void CptDialog::CenterScreen() 
{
	if(m_hWnd!=NULL)
	{
		int nWidth, nHeight, nLeft, nTop;
		RECT rcWin;

		GetScreenSize(nWidth, nHeight);
		GetWindowRect(m_hWnd, &rcWin);

		nLeft = (nWidth - (rcWin.right - rcWin.left + 1)) / 2;
		nTop = (nHeight - (rcWin.bottom - rcWin.top + 1)) / 2;

		::MoveWindow(m_hWnd, nLeft, nTop, rcWin.right - rcWin.left + 1, rcWin.bottom - rcWin.top + 1, TRUE);
	}
}

void CptDialog::CenterParentWindow() 
{
	if(m_hWnd!=NULL)
	{
		if(m_hParentWnd==NULL)
		{
			this->CenterScreen() ;
		}
		else
		{
			RECT rcWin;
			RECT rtParent ;

			::GetWindowRect(m_hParentWnd,&rtParent) ;
			::GetWindowRect(m_hWnd, &rcWin);

			int nLeft = (rtParent.right - rtParent.left)/2+rtParent.left - (rcWin.right-rcWin.left)/2 ;
			int nTop = (rtParent.bottom - rtParent.top)/2+rtParent.top - (rcWin.bottom-rcWin.top)/2 ;

			::SetWindowPos(m_hWnd,HWND_TOP,nLeft,nTop,0,0,SWP_NOSIZE|SWP_NOZORDER) ;
		}
	}

}

void CptDialog::SetIcon(HICON hIcon)
{
	this->SendMessage(WM_SETICON,ICON_SMALL,(LPARAM)hIcon) ;
}

void CptDialog::SetDlgItemText(int nItemID,LPCTSTR lpText)
{
	if(m_hWnd!=NULL)
	{
		::SetDlgItemText(m_hWnd,nItemID,lpText) ;
	}
}

UINT CptDialog::GetDlgItemText(int nItemID,LPTSTR lpText,int nMaxSize)
{
	if(m_hWnd!=NULL)
	{
		return ::GetDlgItemText(m_hWnd,nItemID,lpText,nMaxSize) ;
	}

	return 0 ;
}

BOOL CptDialog::SetDlgItemInt(int nItemID,UINT nValue) 
{
	if(m_hWnd!=NULL)
	{
		return ::SetDlgItemInt(m_hWnd,nItemID,nValue,FALSE) ;
	}

	return FALSE ;
}


int CptDialog::GetDlgItemInt(int nItemID)
{
	if(m_hWnd!=NULL)
	{
		return ::GetDlgItemInt(m_hWnd,nItemID,NULL,FALSE) ;
	}

	return 0 ;
}

HWND CptDialog::GetDlgItem(int nItemID) const
{
	if(m_hWnd!=NULL)
	{
		return ::GetDlgItem(m_hWnd,nItemID) ;
	}
	
	return NULL ;
}

long CptDialog::SendMessage(UINT uMsg,WPARAM wParam,LPARAM lParam) 
{
	if(m_hWnd!=NULL)
	{
		return (long)::SendMessage(m_hWnd,uMsg,wParam,lParam) ;
	}

	return 0 ;
}

BOOL CptDialog::PostMessage(UINT uMsg,WPARAM wParam,LPARAM lParam) 
{
	if(m_hWnd!=NULL)
	{
		return ::PostMessage(m_hWnd,uMsg,wParam,lParam) ;
	}

	return FALSE ;
}

void CptDialog::PerformButton(int nButtonID)
{
	this->PostMessage(WM_COMMAND,nButtonID,0) ;
}

bool CptDialog::SetTimer(int nTimerID,int nInterval) 
{
	if(m_hWnd!=NULL)
	{
		return ::SetTimer(m_hWnd,nTimerID,nInterval,NULL) ? true : false ;
	}

	return false ;
}

void CptDialog::KillTimer(int nTimerID)
{
	if(m_hWnd!=NULL)
	{
		::KillTimer(m_hWnd,nTimerID) ;
	}
}

void CptDialog::OnLButtonDown(int nFlag,const SptPoint& pt) 
{
}

void CptDialog::OnLButtonUp(int nFlag,const SptPoint& pt) 
{
}

void CptDialog::OnMouseMove(int nFlag,const SptPoint& pt) 
{
}

void CptDialog::SetExitFlag() 
{
	if (m_nFlags & WF_CONTINUEMODAL)
	{
		m_nFlags &= ~WF_CONTINUEMODAL;
		this->PostMessage(WM_NULL,NULL,NULL);
	}
}

CptDialog::EDialogResult CptDialog::ShowDialog(bool bShow) 
{
	//DWORD dw = (DWORD)(void*)this ;

	//::DialogBoxParam(m_hModule, MAKEINTRESOURCE(m_uTemplateID), m_hParentWnd, (DLGPROC)CptDialog::DialogProc,(LPARAM)this) ;

	/**/

	bool bParentEnable = false ;

	if(NULL==m_hParentWnd)
	{
		if(!s_DlgListInfo.IsEmpty())
		{
			CptDialog* pOwner = s_DlgListInfo.GetTop() ;

			m_hParentWnd = pOwner->GetSafeHwnd() ;
		}
	}

	if (m_hParentWnd && m_hParentWnd != ::GetDesktopWindow() && ::IsWindowEnabled(m_hParentWnd))
	{
		bParentEnable = true ;
		::EnableWindow(m_hParentWnd, FALSE) ;
	}

	m_nFlags |= WF_CONTINUEMODAL  ;

	LPCDLGTEMPLATE lpDialogTemplate ;
	HRSRC hResource = ::FindResource(m_hModule, MAKEINTRESOURCE(m_uTemplateID), RT_DIALOG);

	_ASSERT(NULL!=hResource) ;

	HGLOBAL lpTemplate = ::LoadResource(m_hModule, hResource) ;

	lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(lpTemplate);

	_ASSERT(NULL!=lpTemplate) ;

	HWND hWnd = ::CreateDialogIndirectParam(m_hModule,lpDialogTemplate, m_hParentWnd,(DLGPROC)CptDialog::DialogProc,(LPARAM)this) ;

	if(NULL!=hWnd)
	{
		s_DlgListInfo.Push(this) ;

		MSG msg ;

		if(bShow)
		{
			::ShowWindow(hWnd,SW_SHOWNORMAL) ;
		}
		
		//::UpdateWindow(hWnd) ;

		while((m_nFlags & WF_CONTINUEMODAL))
		{
			if(::PeekMessage(&msg,NULL,NULL,NULL,PM_NOREMOVE))
			{
				if(::GetMessage(&msg,NULL,NULL,NULL))
				{
					if(!this->PreTranslateMessage(hWnd,msg.message,msg.wParam,msg.lParam))
					{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);
					}
				}
				else
				{
					this->SetExitFlag() ;// 收到 WM_QUIT 退出
				}
			}
			else
			{
				::Sleep(1) ;
			}
		}

		::DestroyWindow(m_hWnd) ;

		s_DlgListInfo.Pop() ;
	}

	if(bParentEnable)
	{
		::EnableWindow(m_hParentWnd, TRUE);

		::SetActiveWindow(m_hParentWnd) ;
		::BringWindowToTop(m_hParentWnd) ;
		//::SetForegroundWindow(m_hParentWnd) ;
	}
	
	m_hParentWnd = NULL ;
	m_hWnd = NULL ;
	/**/
	return m_Result ;
}

bool CptDialog::ShowModeless() 
{
	bool bRet = false ;

	if(m_hWnd!=NULL)
	{
		HANDLE hThread = (HANDLE)::_beginthreadex(NULL,0,ModelessThread,this,0,NULL) ;

		bRet = (hThread!=NULL) ;
	}

	return bRet ;
}

CptDialog* CptDialog::GetModeless(UINT uIDTemplate,HWND hParentWnd)
{
	bool bRet = false ;

	CptDialog* pDlg = new CptDialog(uIDTemplate,hParentWnd,NULL) ;

	if(pDlg!=NULL)
	{
		HANDLE hThread = (HANDLE)::_beginthreadex(NULL,0,ModelessThread,pDlg,0,NULL) ;

		bRet = (hThread!=NULL) ;
	}

	if(!bRet)
	{
		delete pDlg ;
		pDlg = NULL ;
	}

	return pDlg ;
}

CptDialog* CptDialog::GetModeless()
{
	return CptDialog::GetModeless(m_uTemplateID,this->GetParentHwnd()) ;
}

bool CptDialog::ShowInTaskbar(bool bShow)
{
	bool bRet = false ;

	if(m_hWnd!=NULL)
	{
		LONG style = ::GetWindowLong(m_hWnd, GWL_EXSTYLE); //获得窗体扩展风格

		if(bShow)
		{
			style &= ~WS_EX_APPWINDOW; // 取消WS_EX_APPWINDOW
		}
		else
		{
			style |= WS_EX_APPWINDOW; // 增加WS_EX_APPWINDOW
		}
		
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, style); //设置窗体扩展风格

	}

	return bRet;
}


unsigned CptDialog::ModelessThread(void* pParam)
{
	CptDialog* pThis = (CptDialog*)pParam ;

	if(pThis!=NULL)
	{
		if(pThis->Create())
		{
			HWND hWnd = pThis->GetSafeHwnd() ;

			BOOL bRet = FALSE ;
			MSG msg ;

			while ((bRet = ::GetMessage(&msg, hWnd, 0, 0)) != 0) 
			{ 
				if (bRet == -1)
				{
					break ;
					
				}
				else if (!IsWindow(hWnd) || !IsDialogMessage(hWnd, &msg)) 
				{ 
					TranslateMessage(&msg); 
					DispatchMessage(&msg); 
				} 
			}

			delete pThis ;
		}
	}

	return 0 ;
}

int CptDialog::Show() 
{
	int nRet = 0 ;

	if(m_hWnd!=NULL)
	{
		nRet = ::ShowWindow(m_hWnd,SW_SHOW) ;
	}

	return 0 ;
}

void CptDialog::Hide() 
{
	int nRet = 0 ;

	if(m_hWnd!=NULL)
	{
		nRet = ::ShowWindow(m_hWnd,SW_HIDE) ;
	}
}


void CptDialog::Close()
{
	if(m_hWnd!=NULL)
	{
		this->OnClose() ;
		this->OnDestroy() ;

		//::EndDialog(m_hWnd,0) ;
		//m_hWnd = NULL ;
	}
}

void CptDialog::OnClose()
{
	this->SetExitFlag() ;
}

void CptDialog::OnDestroy() 
{
	this->SetExitFlag() ;

	//m_hWnd = NULL ;
}

bool CptDialog::OnOK()
{
	this->m_Result = CptDialog::DialogResult_OK ;

	this->SetExitFlag() ;

	return true ;
}

bool CptDialog::OnCancel()
{
	this->m_Result = CptDialog::DialogResult_Cancel ;

	this->SetExitFlag() ;

	return true ;
}

int CptDialog::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED)			
		{
			switch(LOWORD(wParam))
			{
			case IDOK:
				if(this->OnOK())
				{
					this->m_Result = DialogResult_OK ;
					this->Close() ;
					//return 0 ;
				}
				break ;

			case IDCANCEL:
				if(this->OnCancel())
				{
					this->m_Result = DialogResult_Cancel ;
					this->Close() ;
					//return 0 ;
				}
				break ;

			default:
				this->OnButtonClick(LOWORD(wParam)) ; break ;

			}
		}
		//else if(HIWORD(wParam) ==BN_DBLCLK)
		//{
		//	this->OnButtonDoubleClick(LOWORD(wParam)) ;
		//}
		else if(HIWORD(wParam) == LBN_DBLCLK)
		{
			this->OnListBoxItemDoubleClicked(LOWORD(wParam)) ;
		}
		else if(HIWORD(wParam)==LBN_SELCHANGE)
		{
			this->OnListBoxItemSelectChanged(LOWORD(wParam),(HWND)lParam) ;
		}
		return 1;

	case WM_LBUTTONDOWN:
		{
			SptPoint pt ;

			pt.nX = lParam&0xffff ;
			pt.nY = (LONG)(lParam >> 16) ;

			this->OnLButtonDown((int)wParam,pt) ;
		}
		return 1 ;

	case WM_LBUTTONUP:
		{
			SptPoint pt ;

			pt.nX = lParam&0xffff ;
			pt.nY = (LONG)(lParam >> 16) ;

			this->OnLButtonUp((int)wParam,pt) ;
		}
		return 1 ;

	case WM_MOUSEMOVE:
		{
			SptPoint pt ;

			pt.nX = lParam&0xffff ;
			pt.nY = (LONG)(lParam >> 16) ;

			this->OnMouseMove((int)wParam,pt) ;
		}
		return 1 ;

	case WM_PAINT:
		{
			this->OnPaint() ;

			return 0 ;
		}
		break ;

	case WM_ERASEBKGND:
		{
			HDC hdc = (HDC)wParam ;
			BOOL b = this->OnEraseBkgnd(hdc) ;

			return b ;
		}
		break ;

	case WM_SIZE:
		this->OnSize((UINT)wParam,LOWORD(lParam),HIWORD(lParam)) ;
		return 0 ;
		//break ;

	case WM_TIMER:
		this->OnTimer((int)wParam) ;
		return 1 ;

	case WM_CONTROLSTATUSCHANGED:
		{
			SControlStatus cs ;
			cs.uStatusBlock = (unsigned int)lParam ;
			this->OnControlStatusChanged((int)wParam,cs) ;
		}
		
		return 0 ;
		//break ;

	case WM_DESTROY:
	case WM_QUIT:
		{
			this->OnClose() ;
			this->OnDestroy() ;

			return 0 ;
		}
		break ;
	}

	return 0 ;
}

bool CptDialog::PreTranslateMessage(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	if(WM_KEYDOWN==uMsg && (VK_ESCAPE==wParam || VK_RETURN==wParam))
	{
		return true ;
	}

	return false ;
}

BOOL __stdcall CptDialog::DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CptDialog* pThis = (CptDialog*)::GetProp(hDlg,pHwndPropThisEntry) ;

	if(pThis!=NULL)
	{
		return pThis->OnProcessMessage(hDlg,uMsg, wParam, lParam) ;
	}

	switch(uMsg)
	{

	case WM_CREATE:
		break ;

	case WM_INITDIALOG: 
		{
			if(lParam!=0)
			{
				//HWND hParent = ::GetParent(hDlg) ;

				//if(hParent!=NULL)
				//{
				//	DWORD dwProcessID = 0 ;

				//	DWORD dwCurThreadID = ::GetCurrentThreadId() ;

				//	DWORD dwParentThreadID = ::GetWindowThreadProcessId(hParent,&dwProcessID) ;

				//	if(dwCurThreadID!=dwParentThreadID)
				//	{
				//		BOOL bResult = ::AttachThreadInput(dwParentThreadID,dwCurThreadID,TRUE) ;

				//		int aa = 0 ;
				//	}
				//	//
				//}

				::SetProp(hDlg,pHwndPropThisEntry,(HANDLE)lParam) ;
				//::SetWindowLong(hDlg,GWL_USERDATA,lParam) ;

				CptDialog* pThis = (CptDialog*)lParam ;

				pThis->m_hWnd = hDlg ;

				//::SetWindowsHookEx(WH_GETMESSAGE, &CptDialog::GetHookMsgProc,  NULL, ::GetCurrentThreadId()) ;

				return pThis->OnInitDialog() ;
			}
		}
		break;

	}

	return FALSE ;
}
