/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include <map>
#include <list>
#include "..\..\Common\ptThreadLock.h"
#include "..\..\Common\ptTypeDef.h"
#include "..\..\Common\ptString.h"
#include "..\..\Common\ptGlobal.h"
#include <shellapi.h>
#include "ptWinCtrlDef.h"

struct SSkinCtrlImage
{
	HBITMAP hNormal ;
	HBITMAP hHover ;
	HBITMAP hPress ;
	HBITMAP hDisble ;

	SSkinCtrlImage()
	{
		hNormal = NULL ;
		hHover = NULL ;
		hPress = NULL ;
		hDisble = NULL ;
	}

	void DeleteObject()
	{
		if(hNormal!=NULL)
		{
			::DeleteObject(hNormal) ;
			hNormal = NULL ;
		}

		if(hHover!=NULL)
		{
			::DeleteObject(hHover) ;
			hHover = NULL ;
		}

		if(hPress!=NULL)
		{
			::DeleteObject(hPress) ;
			hPress = NULL ;
		}

		if(hDisble!=NULL)
		{
			::DeleteObject(hDisble) ;
			hDisble = NULL ;
		}
	}
};

class CptCommCtrl
{
public:
	CptCommCtrl(void);
	virtual ~CptCommCtrl(void);

	bool Attach(HWND hWnd) ;
	HWND GetSaftHwnd() const {return m_hWnd;}
	bool AddStyle(const LONG dwNewStyle) ;
	bool RemoveStyle(const DWORD dwNewStyle) ;
	DWORD GetStyle() ;

	bool SetWindowText(CptString strTxt) ;
	CptString GetWindowText() ;
	int SendMessage(int nMsg,WPARAM wParam,LPARAM lParam) ;

protected:
	virtual void OnLButtonDown(int nFlag,const SptPoint& pt) ;
	virtual void OnLButtonUp(int nFlag,const SptPoint& pt) ;
	virtual void OnRButtonDown(int nFlag,const SptPoint& pt) ;
	virtual void OnRButtonUp(int nFlag,const SptPoint& pt) ;
	virtual void OnMouseMove(int nFlag,const SptPoint& pt) ;
	virtual void OnDropFiles(HDROP hDropInfo) {}

	virtual void OnMouseEnter() ;
	virtual void OnMuseLeave() ;

	virtual void OnPaint() ;
	virtual BOOL OnEraseBkgnd(HDC hDC){return FALSE;} 
	virtual int OnNMCustomdraw(NMHDR* pNMHDR,bool& bContinue) {bContinue = true;return 0;} 

	virtual int PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;
	virtual int PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) ;

	void UpdateStatus(SControlStatus NewStatus) ;

private:
	static int __stdcall ProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;
	static int __stdcall ProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;

	void ConnectParentWndProc(HWND hComCtrlWnd) ;
	void DisconnectParentWndProc(HWND hComCtrlWnd) ;

private:
	
	struct SParentComCtrlInfo
	{
		WNDPROC						OldWndProc ;
		pt_STL_list(CptCommCtrl*)		ComCtrlList ;

		SParentComCtrlInfo():OldWndProc(NULL)
		{
		}
	};

protected:
	HWND		m_hWnd ;
	SControlStatus	m_CurState ;

private:
	static pt_STL_map(HWND,SParentComCtrlInfo)	m_ParentComCtrlMap ;
	static CptCritiSecLock						m_MapLock ;
	WNDPROC										m_pOldCtrlDefWndProc ;
};
