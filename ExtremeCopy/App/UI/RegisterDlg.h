/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "skindialog.h"
#include "ptSkinButton.h"

#include "Hyperlink.h"

class CRegisterDlg :
	public CptDialog
{
public:
	CRegisterDlg(HWND hParentWnd);
	~CRegisterDlg(void);

	CptStringList GetSeriesNumber() const {return m_strSeriesNumList;}

protected:
	virtual BOOL OnInitDialog() ;
	virtual void OnPaint() ;
	//virtual bool OnCancel() ;
	virtual void OnButtonClick(int nButtonID) ;
	virtual int OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) ;

private:
	void Register() ;

private:
	CptStringList		m_strSeriesNumList ;

	CHyperlink			m_Hyperlink;
};
