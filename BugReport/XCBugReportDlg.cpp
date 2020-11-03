/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#include "StdAfx.h"
#include "XCBugReportDlg.h"
#include "..\ExtremeCopy\App\ptMultipleLanguage.h"
#include "ptBugReport.h"


CXCBugReportDlg::CXCBugReportDlg(int nDlgID)
	:CptDialog(nDlgID,NULL,CptMultipleLanguage::GetInstance()->GetResourceHandle())
{
}


CXCBugReportDlg::~CXCBugReportDlg(void)
{
}


SUserSituationInfo CXCBugReportDlg::GetUserContent() const
{
	return m_UserContent ;
}

bool CXCBugReportDlg::OnCancel()
{
	return true ;
}

BOOL CXCBugReportDlg::OnInitDialog()
{
	this->CenterScreen() ;

	return CptDialog::OnInitDialog() ;
}

bool CXCBugReportDlg::OnOK()
{
	TCHAR* pBuf = new TCHAR[1024];

	if(pBuf==NULL)
	{
		return false;
	}

	::memset(pBuf,0,1024);

	bool bValidEmail = false ;

	m_UserContent.m_strEmail = _T("") ;
	m_UserContent.m_strProblemContent = _T("") ;
	
	if(this->GetDlgItemText(IDC_EDIT_EMAIL,pBuf,1024)>0)
	{
		m_UserContent.m_strEmail = pBuf ;
	}

	bValidEmail = (m_UserContent.m_strEmail.GetLength()==0);

	do
	{
		PT_BREAK_IF(bValidEmail);

		int nAtPos = m_UserContent.m_strEmail.Find('@',1) ;

		PT_BREAK_IF(nAtPos<=0);

		nAtPos = m_UserContent.m_strEmail.Right(m_UserContent.m_strEmail.GetLength()-nAtPos).Find('.');

		PT_BREAK_IF(nAtPos<=0);

		bValidEmail = true ;
	}
	while(0);

	if(this->GetDlgItemText(IDC_EDIT_PROBLEMCONTENT,pBuf,1024)>0)
	{
		m_UserContent.m_strProblemContent = pBuf ;
	}

	if(!bValidEmail)
	{// 有效的 email 地址
		CptString strText = CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_PLEASESPECIFYVALIDEMAIL) ;
		CptString strTitle = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_ERROR) ;

		::MessageBox(NULL,strText.c_str(),strTitle.c_str(),MB_OK) ;
	}

	delete [] pBuf;
	pBuf = NULL;

	return bValidEmail ;
}

