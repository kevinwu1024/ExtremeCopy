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

#include "ptCommCtrl.h"
#include "ptWinCtrlDef.h"

class CptComboBox : public CptCommCtrl
{
public:
	CptComboBox(void);
	~CptComboBox(void);

	bool AddString(const TCHAR* pStr) ;
	void Clear() ;

	int GetSelectIndex() ;
	bool SetSelectIndex(int nIndex) ;
	int GetItemCount() ;

protected:
	virtual int PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;
};
