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
#include "ptShellTray.h"
#include "shellapi.h"
#include "windows.h"

CptShellTray::CptShellTray(void):m_hWnd(NULL),m_hContextMenu(NULL)
{
}

CptShellTray::~CptShellTray(void)
{
	this->Remove() ;
}

void CptShellTray::Remove()
{
	if(m_hWnd!=NULL)
	{
		NOTIFYICONDATA nd ;

		::memset(&nd,0,sizeof(nd)) ;

		nd.cbSize = sizeof(NOTIFYICONDATA) ;
		nd.hWnd = m_hWnd ;

		::Shell_NotifyIcon(NIM_DELETE, &nd);

		m_hWnd = NULL ;
	}
}


bool CptShellTray::Add(HWND hWnd,UINT uCallbackMsg,HICON hIcon,HMENU hContextMenu)
{
	bool bRet = false ;

	_ASSERT(hWnd!=NULL) ;

	if(hWnd!=NULL)
	{
		this->Remove() ;

		NOTIFYICONDATA nd ;

		::memset(&nd,0,sizeof(nd)) ;

		nd.cbSize = sizeof(NOTIFYICONDATA) ;
		nd.uFlags = NIF_ICON|NIF_MESSAGE ;
		nd.uCallbackMessage = uCallbackMsg ;

		nd.hWnd = hWnd ;
		nd.hIcon = hIcon ;

		::Shell_NotifyIcon(NIM_ADD, &nd);

		m_hWnd = hWnd ;
		m_hContextMenu = hContextMenu ;

		m_TrayPos.SetNotifyIconInfo(m_hWnd,1,uCallbackMsg) ;
	}
	
	return bRet ;
}

void CptShellTray::SetIcon(HICON hIcon)
{
	if(m_hWnd!=NULL)
	{
		NOTIFYICONDATA nd ;

		::memset(&nd,0,sizeof(nd)) ;

		nd.cbSize = sizeof(NOTIFYICONDATA) ;
		nd.uFlags = NIF_ICON | NIIF_NOSOUND;

		nd.hWnd = m_hWnd ;
		nd.hIcon = hIcon ;

		::Shell_NotifyIcon(NIM_MODIFY, &nd);
	}
}

void CptShellTray::SetInfo(const TCHAR* szInfo,const TCHAR* szTitle,DWORD dwIconType) 
{
	//return  ;

	
	if(m_hWnd!=NULL)
	{
		NOTIFYICONDATA nd ;

		::memset(&nd,0,sizeof(nd)) ;

		nd.cbSize = sizeof(NOTIFYICONDATA) ;
		nd.uFlags = NIF_INFO  ;
		
		//m_strBalloonTooltipTitle = _T("") ;

#pragma warning(push)
#pragma warning(disable:4996)
		if(szInfo!=NULL)
		{
			::_tcscpy(nd.szInfo,szInfo) ;

			//m_strBalloonTooltipTitle = szTitle ;
		}

		if(szTitle!=NULL)
		{
			::_tcscpy(nd.szInfoTitle,szTitle) ;
		}
#pragma warning(pop)

		nd.dwInfoFlags = dwIconType ;
		nd.uTimeout = 1*1000 ;

		nd.hWnd = m_hWnd ;
		//nd.hIcon = hIcon ;

		::Shell_NotifyIcon(NIM_MODIFY, &nd);
	}
}

void CptShellTray::ProcessMessage(WPARAM wParam,LPARAM lParam)
{
	if(m_hWnd!=NULL)
	{
		int nEvent = LOWORD(lParam) ;
		int nID = HIWORD(lParam) ;

		SptPoint pt ;
		pt.nX = lParam&0xffff ;
		pt.nY = (LONG)lParam >> 16 ;

		switch(nEvent)
		{
		case WM_RBUTTONUP:
			if(m_hContextMenu!=NULL)
			{
				::TrackPopupMenu(m_hContextMenu,TPM_LEFTALIGN | TPM_RIGHTBUTTON,pt.nX,pt.nY,0,m_hWnd,NULL) ;
			}
			break ;

		case WM_MOUSEMOVE:
			m_TrayPos.OnMouseMove() ;
			break ;

		case WM_MOUSELEAVE:
			this->HideBalloonTooltip() ;
			break ;
		}
	}
}


void CptShellTray::HideBalloonTooltip()
{
	if(m_hWnd!=NULL)
	{
		//const TCHAR* pp = m_strBalloonTooltipTitle.c_str() ;
		//HWND hShellTrayWnd = ::FindWindowEx(NULL,NULL,_T("Shell_TrayWnd"),NULL) ;
		HWND hBalloonToolTip = ::FindWindowEx(NULL,NULL,_T("tooltips_class32"),NULL) ;

		//TCHAR szBuf[256] = {0} ; 
		//::GetWindowText(hBalloonToolTip,szBuf,sizeof(szBuf)/sizeof(TCHAR)) ;

		if(hBalloonToolTip!=NULL)
		{
			//HWND hParentWnd = ::GetParent(hBalloonToolTip) ;
			//::GetClassName(hParentWnd,szBuf,100) ;

			::PostMessage(hBalloonToolTip,WM_LBUTTONDOWN,0,MAKELONG(22,10)) ;
		}
	}
}

//=======================================

CTrayPos::CTrayPos()
{
	UINT	uThreadId;

	m_bTrackMouse = FALSE;
	m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread = (HANDLE) _beginthreadex(NULL, 0, CTrayPos::TrackMousePt, this, 0, &uThreadId);
	InitializeCriticalSection(&m_cs);
}

CTrayPos::~CTrayPos()
{
	if(m_hThread != NULL)
	{
		SetEvent(m_hExitEvent);
		if(WaitForSingleObject(m_hThread, 5000) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hThread, 0);
		}

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if(m_hExitEvent != NULL)
	{
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
	}

	DeleteCriticalSection(&m_cs);
}

UINT CALLBACK CTrayPos::TrackMousePt(PVOID pvClass)
{
	POINT		ptMouse;
	CTrayPos	*pTrayPos = (CTrayPos *) pvClass;

	while(WaitForSingleObject(pTrayPos->m_hExitEvent, 2000) == WAIT_TIMEOUT)
	{

		if(pTrayPos->m_bTrackMouse == TRUE)
		{
			GetCursorPos(&ptMouse);
			
			if(ptMouse.x != pTrayPos->m_ptMouse.x || ptMouse.y != pTrayPos->m_ptMouse.y)
			{
				pTrayPos->m_bTrackMouse = FALSE;
				pTrayPos->OnMouseLeave();
			}
		}
	}

	return 0;
}

VOID CTrayPos::OnMouseMove()
{
	EnterCriticalSection(&m_cs);

	GetCursorPos(&m_ptMouse);
	if(m_bTrackMouse == FALSE)
	{
		OnMouseHover();
		m_bTrackMouse = TRUE;
	}

	LeaveCriticalSection(&m_cs);
}

BOOL CTrayPos::IsMouseHover() const
{
	return m_bTrackMouse;
}

//////////////////////////////////////////////////////////////////////////

CMsgTrayPos::CMsgTrayPos(HWND hwnd, UINT uID, UINT uCallbackMsg)
	: CTrayPos()
{
	SetNotifyIconInfo(hwnd, uID, uCallbackMsg);
}

CMsgTrayPos::~CMsgTrayPos()
{
}

VOID CMsgTrayPos::SetNotifyIconInfo(HWND hwnd, UINT uID, UINT uCallbackMsg)
{
	m_hNotifyWnd = hwnd;
	m_uID = uID;
	m_uCallbackMsg = uCallbackMsg;
}

VOID CMsgTrayPos::OnMouseHover()
{
	if(m_hNotifyWnd != NULL && IsWindow(m_hNotifyWnd))
		PostMessage(m_hNotifyWnd, m_uCallbackMsg, m_uID, WM_MOUSEHOVER);
}

VOID CMsgTrayPos::OnMouseLeave()
{
	if(m_hNotifyWnd != NULL && IsWindow(m_hNotifyWnd))
		PostMessage(m_hNotifyWnd, m_uCallbackMsg, m_uID, WM_MOUSELEAVE);
}