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
#include <commctrl.h>
#include "..\..\Common\ptTypeDef.h"

class CptSkinHeaderCtrl :
	public CptCommCtrl
{
public:
	CptSkinHeaderCtrl(void);
	virtual ~CptSkinHeaderCtrl(void);

	bool Attach(HWND hWnd) ;

	int GetItemCount() const ;
	int InsertItem(int nPos,HDITEM* phdi) ;
	bool DeleteItem(int nPos) ;
	bool GetItem(int nPos,HDITEM* pHeaderItem) const ;
	bool SetItem(int nPos, HDITEM* pHeaderItem) ;
	bool Layout(HDLAYOUT* pHeaderLayout) ;
	bool GetItemRect(int nIndex, SptRect& lpRect) const;

protected:
	virtual void OnPaint() ;
	virtual int PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;

private:
	void Paint() ;
};
