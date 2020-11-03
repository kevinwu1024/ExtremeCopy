/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "ptdialog.h"
#include "../../Common/ptTypeDef.h"

class CXCStartupPosDlg : public CptDialog
{
public:
	CXCStartupPosDlg(const SptPoint& pt) ;
	virtual ~CXCStartupPosDlg() ;

	SptPoint GetStartupPos() const;

private:
	virtual void OnPaint() ;
	virtual void OnLButtonDown(int nFlag,const SptPoint& pt) ;
	virtual void OnLButtonUp(int nFlag,const SptPoint& pt) ;
	virtual void OnMouseMove(int nFlag,const SptPoint& pt) ;
	virtual void OnButtonClick(int nButtonID) ;
	//virtual bool OnCancel() ;
	virtual bool OnOK() ;
	virtual BOOL OnInitDialog() ;
	virtual BOOL OnEraseBkgnd(HDC hDC) ;

	void DrawXCPos() ;


private:
	SptPoint	m_ptStartupPos ;

	HWND		m_hPicWnd ;

	HBITMAP		m_hExtremeCopyBitmap ;
	HDC			m_hXCUIMemDC ;
	HBITMAP		m_hOldXCMemBitmap ;
	SptSize		m_XCUIImageSize ;

	HBITMAP		m_hWinDesktopBitmap ;
	HDC			m_hDesktopMemDC ;
	HBITMAP		m_hOldDesktopMemBitmap ;
	SptSize		m_DesktopImageSize ;

	bool		m_bHold ;
	SptRect		m_XCUIRect ;
	SptRect		m_PicRect ;

	float		m_ScaleX ;
	float		m_ScaleY ;

	SptPoint	m_ptBeginHold ;
	bool		m_bCrossCursor ;
};