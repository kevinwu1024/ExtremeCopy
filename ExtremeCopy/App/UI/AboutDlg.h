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
#include "ptSkinButton.h"
#include "..\XCGlobal.h"
#include "Hyperlink.h"

class CAboutDlg :public CptDialog
{
public:
	CAboutDlg(int nDlgID=IDD_DIALOG_ABOUT);
	~CAboutDlg(void);

protected:
	virtual void OnButtonClick(int nButtonID) ;
	virtual BOOL OnInitDialog() ;
	virtual void OnPaint() ;
	virtual int OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) ;

private:
	CptSkinButton		m_CloseSkinButton ;
	CptSkinButton		m_CancelSkinButton ;

	CHyperlink			m_Hyperlink;
	CHyperlink			m_EmailHyperlink;
	bool				m_bReg ;
};
