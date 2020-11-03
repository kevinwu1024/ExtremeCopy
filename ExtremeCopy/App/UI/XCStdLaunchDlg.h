/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "ptdialog.h"
#include "Hyperlink.h"

class CXCStdLaunchDlg :public CptDialog
{
public:
	CXCStdLaunchDlg(int nDlgID=IDD_DIALOG_STDLAUNCH);
	virtual ~CXCStdLaunchDlg(void);

protected:
	virtual void OnButtonClick(int nButtonID) ;

	virtual BOOL OnInitDialog() ;
	virtual void OnPaint() ;

private:
	CHyperlink			m_Hyperlink;
};
