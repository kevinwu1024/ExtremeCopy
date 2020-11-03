/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "shellapi.h"
#include "ptString.h"
#include "ptTypeDef.h"
#include <Windows.h>

class CTrayPos
{
private:
	POINT				m_ptMouse;
	HANDLE				m_hThread;
	HANDLE				m_hExitEvent;
	BOOL				m_bTrackMouse;
	CRITICAL_SECTION	m_cs;

	
public:
	CTrayPos();
	virtual ~CTrayPos();
	
	static UINT CALLBACK TrackMousePt(PVOID pvClass);
	VOID OnMouseMove();
	BOOL IsMouseHover() const;
	
protected:
	virtual VOID OnMouseHover() = 0;
	virtual VOID OnMouseLeave() = 0;
};

class CMsgTrayPos : public CTrayPos
{
private:
	HWND	m_hNotifyWnd;
	UINT	m_uID;
	UINT	m_uCallbackMsg;

public:
	CMsgTrayPos(HWND hwnd=NULL, UINT uID=0, UINT uCallbackMsg=0);
	~CMsgTrayPos();

	VOID SetNotifyIconInfo(HWND hwnd, UINT uID, UINT uCallbackMsg);

protected:
	VOID OnMouseHover();
	VOID OnMouseLeave();
};



class CptShellTray
{
public:
	CptShellTray(void);
	~CptShellTray(void);

	void SetIcon(HICON hIcon) ;
	void Remove() ;
	bool Add(HWND hWnd,UINT uCallbackMsg,HICON hIcon,HMENU hContextMenu) ;

	void SetInfo(const TCHAR* szInfo,const TCHAR* szTitle,DWORD dwIconType=NIIF_NONE) ;  // icon type: NIIF_ERROR,NIIF_INFO,NIIF_NONE,NIIF_USER,NIIF_WARNING
	void HideBalloonTooltip() ;
	void ProcessMessage(WPARAM wParam,LPARAM lParam) ;

	bool IsMouseHover() const
	{
		return m_TrayPos.IsMouseHover() ? true : false ;
	}

	HWND GetHwnd() const
	{
		return m_hWnd ;
	}

private:
	HWND		m_hWnd ;
	HMENU		m_hContextMenu ;
	CMsgTrayPos	m_TrayPos ;
};
