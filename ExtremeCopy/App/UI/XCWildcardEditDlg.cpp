/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"

#include "../Language/XCRes_ENU/resource.h"
#include "XCWildcardEditDlg.h"
#include "..\..\Common\ptWinPath.h"
#include "..\XCConfiguration.h"
#include "ptMessageBox.h"


CXCWildcardEditDlg::CXCWildcardEditDlg(CptString strPath,HWND hParamWnd,int nDlgID):CptDialog(nDlgID,hParamWnd,CptMultipleLanguage::GetInstance()->GetResourceHandle())
{
	this->SetPathWithWildcard(strPath) ;
}

CXCWildcardEditDlg::~CXCWildcardEditDlg(void)
{
}

bool CXCWildcardEditDlg::OnOK()
{
	bool bRet = false ;

	int nWildcardTextLength = ::GetWindowTextLength(this->GetDlgItem(IDC_EDIT_WILDCARDINPUT)) ;

	if(nWildcardTextLength==0)
	{
		m_strWildcard = _T("") ;

		bRet = true ;
	}
	else if(nWildcardTextLength<128)
	{
		TCHAR szBuf[128] = {0} ;

		this->GetDlgItemText(IDC_EDIT_WILDCARDINPUT,szBuf,sizeof(szBuf)/sizeof(TCHAR)) ;

		if(::_tcslen(szBuf)==0 || ::IsContainWildcardChar(szBuf))
		{
			m_strWildcard = szBuf ;

			if(m_strWildcard.GetLength()>0)
			{
				m_strWildcard.Remove('\\') ;
				m_strWildcard.Remove('/') ;
			}
			bRet = true ;
		}
	}
	
	if(!bRet)
	{
		CptString strText = ::CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_INPUTLEGALWILDCARD) ;
		CptString strTitle = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;

		CptMessageBox::ShowMessage(this->GetSafeHwnd(),strText,strTitle,CptMessageBox::Button_OK) ;
	}

	return bRet ;
}

BOOL CXCWildcardEditDlg::OnInitDialog() 
{
	CptString strTilte = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WILDCARDDLG) ;
	::SetWindowText(this->GetSafeHwnd(),strTilte.c_str()) ;

	this->CenterParentWindow() ;

		this->SetDlgItemText(IDC_STATIC_DESTINATIONWILDCARD,m_strPath) ;
	this->SetDlgItemText(IDC_EDIT_WILDCARDINPUT,m_strWildcard) ;

	return CptDialog::OnInitDialog() ;
}


void CXCWildcardEditDlg::SetPathWithWildcard(CptString strPath) 
{
	EInterestFileType iftType = ::GetInterestFileType(strPath.c_str()) ;

	if(iftType==IFT_Folder || iftType==IFT_FolderWithWildcard)
	{
		if(iftType==IFT_FolderWithWildcard)
		{
			CptWinPath::SPathElementInfo pei ;

			pei.uFlag = CptWinPath::PET_Path|CptWinPath::PET_FileName ;

			if(!CptWinPath::GetPathElement(strPath.c_str(),pei))
			{
				return ;
			}

			m_strPath = pei.strPath ;
			m_strWildcard = pei.strFileName ;
		}
		else
		{
			m_strPath = strPath ;
			m_strWildcard = _T("") ;
		}
	}
}

CptString CXCWildcardEditDlg::GetPathWithWildcard() const
{
	CptString strRet = m_strPath  ;

	if(m_strWildcard.GetLength()>0)
	{
		strRet += '\\' ;
		strRet += m_strWildcard ;
	}

	return strRet ;
}

void CXCWildcardEditDlg::OnButtonClick(int nButtonID) 
{
	switch(nButtonID)
	{
	case IDC_BUTTON_HELP: // help
		::LaunchHelpFile(HFP_WildcardDlg) ;
		//::OpenLink(WEBLINK_HELP_WILDCARDDLG) ;
		break ;
	}
}

void CXCWildcardEditDlg::OnPaint() 
{
	CptDialog::OnPaint() ;
}

int CXCWildcardEditDlg::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	return CptDialog::OnProcessMessage(hWnd,uMsg,wParam,lParam) ;
}




