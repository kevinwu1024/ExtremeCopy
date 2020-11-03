/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include <windows.h>
#include "..\..\Common\ptString.h"

#include "ptCommCtrl.h"

struct SSkinButtonBitmapInfo
{
	HBITMAP		hNormal ;
	HBITMAP		hDown ;
	HBITMAP		hHover ;
	HBITMAP		hDisable ;

	SSkinButtonBitmapInfo():hNormal(NULL),hDown(NULL),hHover(NULL),hDisable(NULL)
	{
	}

	~SSkinButtonBitmapInfo()
	{
		this->Release() ;
	}

	void Release()
	{
		SAFE_DELETE_GDI(hNormal) ;
		SAFE_DELETE_GDI(hDown) ;
		SAFE_DELETE_GDI(hHover) ;
		SAFE_DELETE_GDI(hDisable) ;
	}
};

class CptSkinButton : public CptCommCtrl
{
public:
	CptSkinButton(void);
	virtual ~CptSkinButton(void);

	void Attach(HWND hwnd) ;

	void SetBkBitmap(const SSkinButtonBitmapInfo& sbbi) ;
	void SetBkBitmap(HBITMAP hNormal,HBITMAP hHover,HBITMAP hDown,HBITMAP hDisable) ;

	void Update() ;
	void SetTextBold(bool bBold) ;

protected:
	virtual int PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;
	virtual int PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) ;

	virtual void OnMouseEnter() ;
	virtual void OnMuseLeave() ;
	virtual void OnLButtonDown(int nFlag,const SptPoint& pt) ;
	virtual void OnLButtonUp(int nFlag,const SptPoint& pt) ;

private:
	void Paint() ;
	void Paint(HBITMAP hBitmap) ;

private:
	HBITMAP			m_hNormal ;
	HBITMAP			m_hHover ;
	HBITMAP			m_hDown ;
	HBITMAP			m_hDisable ;

	WNDPROC			m_pParentDefWndProc ;
	WNDPROC			m_pItemDefWndProc ;
	CptString		m_strText ;
	HFONT			m_hFont ;
};
