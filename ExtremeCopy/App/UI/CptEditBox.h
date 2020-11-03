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


class CptEditBox : public CptCommCtrl
{
public:
	CptEditBox(void);
	~CptEditBox(void);

	void Attach(HWND hWnd) ;
	HWND GetHandle() const {return m_hWnd;};

	void SetLimitText(unsigned int nMax) ;
	unsigned GetLimitText() ;
protected:
	//virtual int PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;

};
