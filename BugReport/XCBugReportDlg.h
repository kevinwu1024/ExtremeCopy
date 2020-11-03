/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "..\extremecopy\app\ui\ptdialog.h"
#include "..\extremecopy\app\Language\XCRes_ENU\resource.h"

struct SUserSituationInfo
{
	CptString	m_strEmail ;
	CptString	m_strProblemContent ;
};

class CXCBugReportDlg :
	public CptDialog
{
public:
	CXCBugReportDlg(int nDlgID=IDD_DIALOG_BUGREPORT);
	virtual ~CXCBugReportDlg(void);

	virtual bool OnOK() ;
	virtual bool OnCancel() ;
	virtual BOOL OnInitDialog() ;

	SUserSituationInfo GetUserContent() const;

private:
	SUserSituationInfo	m_UserContent ;
	//const CptString		m_strMiniDumpFile ;
};

