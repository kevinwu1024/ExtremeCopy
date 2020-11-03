/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "..\ExtremeCopy\App\UI\ptDialog.h"
#include "..\ExtremeCopy\App\Language/XCRes_ENU/resource.h"
#include "..\ExtremeCopy\Common\ptString.h"


class CXCDownloadDlg : public CptDialog
{
public:
	CXCDownloadDlg(int nDlgID=IDD_DIALOG_DOWNLOAD,HWND hParentWnd=NULL) ;
	virtual ~CXCDownloadDlg() ;

	void SetDownloadFile(CptString strURLFile,CptString strFolder) ;

private:
	bool DownloadFile(CptString strURLFile,CptString strFolder) ;

	virtual BOOL OnInitDialog() ;
	virtual bool OnCancel() ;

	static UINT __stdcall DownloadThreadFunc(void* p) ;


private:
	CptString m_strURLFile ;
	CptString m_strFolder ;
	HANDLE m_hThread ;
	bool		m_bCancel ;
	bool		m_bPause ;
};