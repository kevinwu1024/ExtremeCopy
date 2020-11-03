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

class CptListBox : public CptCommCtrl
{
public:
	CptListBox(HWND hWnd=NULL);
	virtual ~CptListBox(void);

	void Attach(HWND hWnd) ;
	HWND GetHwnd() const {return m_hWnd;};

	bool AddString(const TCHAR* pStr) ;
	void Clear() ;

	int GetSelectIndex() ;
	bool SetSelectIndex(int nIndex) ;
	int GetItemCount() ;
	int GetSelectCount() ;
	bool GetMultipleSelIndex(std::vector<int>& IndexVer) ;

	bool DeleteString(const int nIndex) ;
	bool GetString(int nIndex,TCHAR* pStr) ;
	bool UpdateString(int nIndex,const TCHAR* lpStr) ;
	bool InsertString(int nIndex,const TCHAR* lpStr) ;

	void UpdateHorizontalExtent() ;

};
