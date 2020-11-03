/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "ptDialog.h"
#include "..\..\Common\ptTypeDef.h"
#include "..\..\Common\WndShadow.h"

class CSkinDialog : public CptDialog
{
public:
	CSkinDialog(int nDlgID,HWND hParentWnd);
	virtual ~CSkinDialog(void);

protected:
	virtual int OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) ;
	virtual BOOL OnInitDialog() ;
	virtual void OnPaint() ;
	virtual BOOL OnEraseBkgnd(HDC hDC) ;
	
	void SetWindowSize(const SptSize& size) ;

	void SetDrawing(bool bDrawEdge,bool bDrawTitelBar) ;
	void SetDragMove(bool bDragMove) ;

private:
	void DrawTitleBar() ;
	void DrawEdge() ;

private:
	bool m_bDrawEdge ;
	bool m_bDrawTitleBar ;
	bool m_bDragMove ;

	CWndShadow m_Shadow;
};
