/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "ptcommctrl.h"
#include "..\..\Common\ptTypeDef.h"

class CptListView ;

struct SScrollbarImage
{
	SSkinCtrlImage	FirstArrow ;
	SSkinCtrlImage	SecondArrow ;
	SSkinCtrlImage	Thumb ;
	SSkinCtrlImage	Background ;

	void DeleteObject()
	{
		FirstArrow.DeleteObject() ;
		SecondArrow.DeleteObject() ;
		Thumb.DeleteObject() ;
		Background.DeleteObject() ;
	}
};

class CptSkinHorizontalScrollbar :
	public CptCommCtrl
{
public:
	CptSkinHorizontalScrollbar(void);
	virtual ~CptSkinHorizontalScrollbar(void);

	bool Attach(HWND hWnd) ;

	void ScrollLeft() ;
	void ScrollRight() ;
	void UpdateThumbPosition() ;
	void LimitThumbPosition() ;

	void SetSkinImage(const SScrollbarImage& image) ;
//	void SetSkinImage(HBITMAP hLeftArrow,HBITMAP hRightArrow,HBITMAP hThumb) ;

protected:
	virtual int PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;
	virtual int PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) ;

private:
	virtual void OnLButtonDown(int nFlag,const SptPoint& pt) ;
	virtual void OnLButtonUp(int nFlag,const SptPoint& pt) ;
	virtual void OnMouseMove(int nFlag,const SptPoint& pt) ;

	virtual void OnTimer(int nTimerID) ;
	virtual void OnPaint() ;

	void PageRight() ;
	void PageLeft() ;

	void Draw() ;

private:
	bool		m_bMouseDown ;
	bool		m_bDragging ;
	bool		m_bMouseDownArrowRight ;
	bool		m_bMouseDownArrowLeft ;
	int			m_nThumbLeft ;
	double		m_duThumbRemainder ;
	double		m_duThumbInterval ;

	SScrollbarImage		m_Images ;

	SptSize			m_ArrowSize ;
	SptSize			m_ThumbSize ;
};
