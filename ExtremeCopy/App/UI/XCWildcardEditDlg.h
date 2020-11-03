/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "SkinDialog.h"
#include "ptSkinButton.h"

class CXCWildcardEditDlg:public CptDialog
{
public:
	CXCWildcardEditDlg(CptString strPath,HWND hParamWnd,int nDlgID=IDD_DIALOG_EDITWILDCARD);
	virtual ~CXCWildcardEditDlg(void);

	
	CptString GetPathWithWildcard() const;

protected:
	void SetPathWithWildcard(CptString strPath) ;
	virtual void OnButtonClick(int nButtonID) ;
	virtual int OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) ;

	virtual void OnPaint() ;
	virtual BOOL OnInitDialog() ;
	virtual bool OnOK() ;

private:
	CptString		m_strPath ;
	CptString		m_strWildcard ;
};
