/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "commctrl.h"
#include "ptCommCtrl.h"

class CptTabCtrl : public CptCommCtrl
{
public:
	CptTabCtrl(void);
	virtual ~CptTabCtrl(void);

	int GetCurFocus() const ;
	void SetCurFocus(int nIndex);

	int GetCurSel() const;
	int SetCurSel(int nIndex);


	DWORD GetExtendedStyle();
	DWORD SetExtendedStyle(DWORD dwNewStyle);

	HIMAGELIST GetImageList() const ;

	bool GetItem(int nIndex,TCITEM& TabCtrlItem) const;
	bool SetItem(int nIndex,TCITEM& TabCtrlItem) ;


	int GetItemCount( ) const;
	bool GetItemRect(int nIndex,SptRect& rect) const;

	int GetRowCount( ) const;

	int SetMinTabWidth(int cx);

	bool DeleteItem(int nItem) ;
	bool DeleteAllItems();

	int InsertItem(int nIndex,const TCITEM& item) ;
};
