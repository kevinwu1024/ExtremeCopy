/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include <commctrl.h>
#include "ptCommCtrl.h"
#include "CptSkinHeaderCtrl.h"
#include "CptSkinHorizontalScrollbar.h"


class CptListView : public CptCommCtrl
{
public:
	CptListView(void);
	virtual ~CptListView(void);

	bool Attach(HWND hWnd) ;
	HWND GetHandle() const {return m_hWnd;}

	void Clear() ;

	int InsertColumn(int nIndex,const LVCOLUMN& Col) ;

	DWORD SetExtendedStyle(DWORD l) ;
	DWORD GetExtendedStyle() const;

	int InsertItem(const LVITEM& lvItem) ;
	int InsertItem(int nItemIndex,LPTSTR lpszItem ) ;
	int InsertItem(int nItemIndex,LPTSTR lpszItem,int nImage);

	bool SetItemText(int nItemIndex,int nSubItemIndex,LPTSTR lpszText) ;
	bool GetItemText(int nItemIndex,int nSubItemIndex,LPTSTR lpszText,int nBufSize) const;
	bool SetItem(const LVITEM& item) ;
	bool DeleteItem(int nItemIndex) ;

	HWND GetHeaderHwnd() const;

	int GetTopIndex() const ;
	//int GetSelectedColumn() ;
	int GetSelectedCount() const;
	int GetStringWidth(LPCTSTR lpszStr) const;

	bool SetBkColor(COLORREF crNew) ;
	bool SetTextBkColor(COLORREF cr) ;
	COLORREF GetTextBkColor() const;

	HIMAGELIST SetImageList(HIMAGELIST hImageList,int nType) ;
	HIMAGELIST GetImageList(int nType) const;

	//int GetSelectIndex() ;
	//bool SetSelectIndex(int nIndex) ;
	int GetItemCount() const ;

	int GetScrollPos(int nBar) const;
	bool GetScrollRange(int nBar,int& Max,int& Min) const;

	int GetHotItem() const ;
	void GetSelectIndexs(std::vector<int>& SelectIndexVer) ;
	void SetSelectIndexs(const std::vector<int>& SelectIndexVer) ;

protected:
	//virtual void OnPaint() ;

private:
	struct SThisNode
	{
		CptListView*	pThis ;
		SThisNode*		pNext ;

		SThisNode():pThis(NULL),pNext(NULL)
		{

		}
	};

protected:
	virtual int PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;
	virtual int PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) ;

private:
	void CreateScrollbar() ;

private:
	COLORREF						m_crBackgroud ;
	WNDPROC							m_pParentDefWndProc ;
	WNDPROC							m_pItemDefWndProc ;

	HWND							m_hHScrollWnd ;
	CptSkinHorizontalScrollbar		m_SkinHScrollbar ;
	SScrollbarImage					m_HScrollbarImages ;
};

