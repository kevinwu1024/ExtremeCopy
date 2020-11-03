/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ConfigurationDialog.h"
#include "..\Language\XCRes_ENU\resource.h"
#include <Commdlg.h>
#include "..\XCConfiguration.h"
#include "..\..\Common\ptWinPath.h"
#include "ptMessageBox.h"
#include "../../App/ptMultipleLanguage.h"
#include "XCStartupPosDlg.h"

CConfigurationDialog::CConfigurationDialog(void):CptDialog(IDD_DIALOG_CONFIGURATION,NULL,CptMultipleLanguage::GetInstance()->GetResourceHandle()),m_nPreLanguageIndex(0),m_nCurLanguageIndex(0)
{
}

CConfigurationDialog::~CConfigurationDialog(void)
{
}

BOOL CConfigurationDialog::OnInitDialog() 
{
	CptString strTxt = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_CONFIGURATION) ;

	::SetWindowText(this->GetSafeHwnd(),strTxt.c_str()) ;

	CptString strVersion = ::MakeXCVersionString() ;

	if(strVersion.GetLength()>0)
	{
		this->SetDlgItemText(IDC_STATIC_VERSIONNAME,strVersion.c_str()) ;
	}

	this->CenterScreen() ;

	this->IniUI() ;

	// 从配置文件加载设置数据
	CXCConfiguration::GetInstance()->LoadConfigDataFromFile(m_ConfigData) ;
	CXCConfiguration::GetInstance()->LoadConfigShareFromFile(m_ConfigShare) ;

	this->UpdateDataToControl() ;

#ifdef VERSION_PORTABLE
	::ShowWindow(this->GetDlgItem(IDC_CHECK_ASDEFAULTCOPYING),SW_HIDE) ;
#endif

	return CptDialog::OnInitDialog() ;
}

void CConfigurationDialog::IniUI() 
{
	m_FileDataBufComboBox.Attach(this->GetDlgItem(IDC_COMBO_FILEDATABUF)) ;

	int BufSizeArray[] = {32,16,8,4,2} ;
	CptString strBufSize ;

	for(int i=0;i<sizeof(BufSizeArray)/sizeof(int);++i)
	{
		strBufSize.Format(_T("%d"),BufSizeArray[i]) ;
		m_FileDataBufComboBox.AddString(strBufSize.c_str()) ;
	}

	m_LanguageComboBox.Attach(this->GetDlgItem(IDC_COMBO_LANGUAGE)) ;

	m_UITypeComboBox.Attach(this->GetDlgItem(IDC_COMBO_UITYPE)) ;

	CptString str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_UITYPE_SIMPLE) ;
	m_UITypeComboBox.AddString(str.c_str()) ;

	str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_UITYPE_NORMAL) ;
	m_UITypeComboBox.AddString(str.c_str()) ;

	CptString strStartupPath = CptWinPath::GetStartupPath() + _T("\\Language") ;

	::SearchResourceDLL(m_LanguageName2DLLNameMap,strStartupPath) ;

	Str2StrMap_t::const_iterator it = m_LanguageName2DLLNameMap.begin() ;
	
	for(;it!=m_LanguageName2DLLNameMap.end();++it)
	{
		m_LanguageComboBox.AddString((*it).first.c_str()) ;
	}

	m_SFIfConditonComboBox.Attach(this->GetDlgItem(IDC_COMBO_IFCONDITION)) ;
	m_SFThenOperationComboBox.Attach(this->GetDlgItem(IDC_COMBO_THENOPERATION)) ;
	m_SFOtherwiseOperationComboBox.Attach(this->GetDlgItem(IDC_COMBO_OTHERWISEOPERATOIN)) ;

	// 添加 “相同文件”处理操作的字符串到界面
	for(int i=0;i<SRichCopySelection::SFOT_Last;++i)
	{
		switch(i)
		{
		case SRichCopySelection::SFOT_Skip:
			str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_SKIP) ;
			break ;

		case SRichCopySelection::SFOT_Replace:
			str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_REPLACE) ;
			break ;

		default: _ASSERT(FALSE) ;
		case SRichCopySelection::SFOT_Ask:
			str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_ASKME) ;
			break ;

		case SRichCopySelection::SFOT_Rename:
			str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_RENAME) ;
			break ;
		}

		m_SFThenOperationComboBox.AddString(str.c_str()) ;
		m_SFOtherwiseOperationComboBox.AddString(str.c_str()) ;
	}

	// 添加“相同文件”条件的字符串到界面
	for(int i=0;i<SRichCopySelection::FDT_Last;++i)
	{
		switch(i)
		{
		default: _ASSERT(FALSE) ;
		case SRichCopySelection::FDT_Newer:
			str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_NEWER) ;
			break ;

		case SRichCopySelection::FDT_Older:
			str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_OLDER) ;
			break ;

		case SRichCopySelection::FDT_Bigger:
			str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_BIGGER) ;
			break ;

		case SRichCopySelection::FDT_Smaller:
			str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_SMALLER) ;
			break ;

		case SRichCopySelection::FDT_SameTimeAndSize:
			str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_SAMETIMEANDSIZE) ;
			break ;
		}

		m_SFIfConditonComboBox.AddString(str.c_str()) ;
	}


#ifndef VERSION_PROFESSIONAL

	// 如果非专业版，则相关的设置不可用
	::EnableWindow(this->GetDlgItem(IDC_EDIT_MAXFAILEDFILE),FALSE) ;

	::EnableWindow(this->GetDlgItem(IDC_RADIO_SAMEFILE_ASKME),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_RADIO_SAMEFILE_RENAMEALL),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_RADIO_SAMEFILE_REPLACEALL),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_RADIO_SAMEFILE_SKIPALL),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_RADIO_SAMEFILE_REPLACENEWER),FALSE) ;

	::EnableWindow(this->GetDlgItem(IDC_RADIO_ERROR_ASKME),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_RADIO_ERROR_RETRY),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_RADIO_ERROR_IGNORE),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_RADIO1_ERROR_EXIT),FALSE) ;

	::EnableWindow(this->GetDlgItem(IDC_EDIT_RETRYMAXTIMES),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_RADIO1_RETRYTHENASK),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_RADIO1_RETRYTHENIGNORE),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_RADIO1_RETRYTHENEXIT),FALSE) ;

	::EnableWindow(this->GetDlgItem(IDC_COMBO_FILEDATABUF),FALSE) ;

	::EnableWindow(this->GetDlgItem(IDC_COMBO_THENOPERATION),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_COMBO_THENOPERATION),FALSE) ;
	::EnableWindow(this->GetDlgItem(IDC_COMBO_OTHERWISEOPERATOIN),FALSE) ;
#endif
}

void CConfigurationDialog::UpdateDataToControl() 
{
	m_UITypeComboBox.SetSelectIndex(m_ConfigData.UIType==UI_Simple?0:1) ;

	::SendMessage(this->GetDlgItem(IDC_CHECK_ASDEFAULTCOPYING),BM_SETCHECK,m_ConfigData.bDefaultCopying?BST_CHECKED:BST_UNCHECKED,0) ;
	::SendMessage(this->GetDlgItem(IDC_CHECK_TOPMOST),BM_SETCHECK,m_ConfigData.bTopMost?BST_CHECKED:BST_UNCHECKED,0) ;

	::SendMessage(this->GetDlgItem(IDC_CHECK_PLAYFINISHEDSOUND),BM_SETCHECK,m_ConfigData.bPlayFinishedSound?BST_CHECKED:BST_UNCHECKED,0) ;

	::SendMessage(this->GetDlgItem(IDC_CHECK_MINIMUMTOTRAY),BM_SETCHECK,m_ConfigData.bMinimumToTray?BST_CHECKED:BST_UNCHECKED,0) ;

	::SendMessage(this->GetDlgItem(IDC_CHECK_WRITELOG),BM_SETCHECK,m_ConfigData.bWriteLog?BST_CHECKED:BST_UNCHECKED,0) ;

	::SendMessage(this->GetDlgItem(IDC_CHECK_CLOSEAFTERVERIFY),BM_SETCHECK,m_ConfigData.bCloseWindowAfterDone?BST_CHECKED:BST_UNCHECKED,0) ;

	::SendMessage(this->GetDlgItem(IDC_CHECK_VERIFYDATA),BM_SETCHECK,m_ConfigShare.bVerifyData?BST_CHECKED:BST_UNCHECKED,0) ;

	::SendMessage(this->GetDlgItem(IDC_CHECK_CHECKNEWVERSION),BM_SETCHECK,m_ConfigData.bAutoUpdate?BST_CHECKED:BST_UNCHECKED,0) ;

	::SendMessage(this->GetDlgItem(IDC_CHECK_QUEUETASK),BM_SETCHECK,m_ConfigData.bAutoQueueMultipleTask?BST_CHECKED:BST_UNCHECKED,0) ;

	this->SetDlgItemInt(IDC_EDIT_MAXFAILEDFILE,m_ConfigData.nMaxFailedFiles) ;

	this->SetDlgItemText(IDC_EDIT_SOUNDFILE,m_ConfigData.strSoundFile.c_str()) ;

	int nLanguageIndex = 0 ;

	Str2StrMap_t::const_iterator it = m_LanguageName2DLLNameMap.begin() ;
	
	for(;it!=m_LanguageName2DLLNameMap.end();++it)
	{
		if(m_ConfigData.strResourceDLL.CompareNoCase((*it).second.c_str())==0)
		{
			m_LanguageComboBox.SetSelectIndex(nLanguageIndex) ;
			m_nPreLanguageIndex = nLanguageIndex ;
			break ;
		}
		++nLanguageIndex ;
	}

	int nSelIndex = 0 ;

	switch(m_ConfigData.nCopyBufSize)
	{
	case 32: nSelIndex = 0 ; break ;
	default:
	case 16: nSelIndex = 1 ; break ;
	case 8: nSelIndex = 2 ; break ;
	case 4: nSelIndex = 3 ; break ;
	case 2: nSelIndex = 4 ; break ;
	}

	m_FileDataBufComboBox.SetSelectIndex(nSelIndex) ;

	m_SFIfConditonComboBox.SetSelectIndex(m_ConfigShare.sfic.IfCondition) ;
	m_SFThenOperationComboBox.SetSelectIndex(m_ConfigShare.sfic.ThenOperation) ;
	m_SFOtherwiseOperationComboBox.SetSelectIndex(m_ConfigShare.sfic.OtherwiseOperation) ;
	

#ifdef VERSION_PROFESSIONAL
	// 共享的设置
	this->SetSameFileProcCheck(m_ConfigShare.sfpt) ;
	this->SetErrorProcCheck(m_ConfigShare.epc) ;
	this->SetRetryFailThenCheck(m_ConfigShare.RetryFailThen) ;

	this->SetDlgItemInt(IDC_EDIT_RETRYMAXTIMES,m_ConfigShare.nRetryTimes) ;
	
#endif
}

void CConfigurationDialog::UpdateDataFromControl() 
{
	m_ConfigData.UIType = m_UITypeComboBox.GetSelectIndex()==0 ? UI_Simple : UI_Normal ;

	m_ConfigData.bDefaultCopying = ::SendMessage(this->GetDlgItem(IDC_CHECK_ASDEFAULTCOPYING),BM_GETCHECK,0,0)==BST_CHECKED ;
	m_ConfigData.bPlayFinishedSound = ::SendMessage(this->GetDlgItem(IDC_CHECK_PLAYFINISHEDSOUND),BM_GETCHECK,0,0)==BST_CHECKED ;
	m_ConfigData.bTopMost = ::SendMessage(this->GetDlgItem(IDC_CHECK_TOPMOST),BM_GETCHECK,0,0)==BST_CHECKED ;
	m_ConfigData.bMinimumToTray = ::SendMessage(this->GetDlgItem(IDC_CHECK_MINIMUMTOTRAY),BM_GETCHECK,0,0)==BST_CHECKED ;
	m_ConfigData.bWriteLog = ::SendMessage(this->GetDlgItem(IDC_CHECK_WRITELOG),BM_GETCHECK,0,0)==BST_CHECKED ;
	m_ConfigData.bCloseWindowAfterDone = ::SendMessage(this->GetDlgItem(IDC_CHECK_CLOSEAFTERVERIFY),BM_GETCHECK,0,0)==BST_CHECKED ;
	m_ConfigData.bAutoUpdate = ::SendMessage(this->GetDlgItem(IDC_CHECK_CHECKNEWVERSION),BM_GETCHECK,0,0)==BST_CHECKED ;
	m_ConfigData.bAutoQueueMultipleTask = (::SendMessage(this->GetDlgItem(IDC_CHECK_QUEUETASK),BM_GETCHECK,0,0)==BST_CHECKED) ;

	m_ConfigShare.bVerifyData = (::SendMessage(this->GetDlgItem(IDC_CHECK_VERIFYDATA),BM_GETCHECK,0,0)==BST_CHECKED) ;

	m_ConfigData.nMaxFailedFiles = this->GetDlgItemInt(IDC_EDIT_MAXFAILEDFILE) ;
	
	TCHAR szBuf[8*MAX_PATH] = {0} ;
	this->GetDlgItemText(IDC_EDIT_SOUNDFILE,szBuf,sizeof(szBuf)/sizeof(TCHAR)) ;

	m_ConfigData.strSoundFile = szBuf ;

	const int nSelectedLanguageIndex= m_LanguageComboBox.GetSelectIndex() ;
	int nSelectIndex = 0 ;

	Str2StrMap_t::const_iterator it = m_LanguageName2DLLNameMap.begin() ;

	for(;it!=m_LanguageName2DLLNameMap.end();++it)
	{
		if(nSelectIndex==nSelectedLanguageIndex)
		{
			m_ConfigData.strResourceDLL = (*it).second.c_str() ;
			m_nCurLanguageIndex = nSelectIndex ;
			break ;
		}

		++nSelectIndex ;
	}

	switch(m_FileDataBufComboBox.GetSelectIndex())
	{
	case 0: m_ConfigData.nCopyBufSize = 32; break ;
	default:
	case 1: m_ConfigData.nCopyBufSize = 16; break ;
	case 2: m_ConfigData.nCopyBufSize = 8; break ;
	case 3: m_ConfigData.nCopyBufSize = 4; break ;
	case 4: m_ConfigData.nCopyBufSize = 2; break ;
	}

#ifdef VERSION_PROFESSIONAL
	// 共享的设置

	// 遇到相同文件时的处理方式
	if(::SendMessage(this->GetDlgItem(IDC_RADIO_SAMEFILE_ASKME),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.sfpt = SRichCopySelection::SFPT_Ask ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_SAMEFILE_RENAMEALL),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.sfpt = SRichCopySelection::SFPT_Rename ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_SAMEFILE_REPLACEALL),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.sfpt = SRichCopySelection::SFPT_Replace ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_SAMEFILE_SKIPALL),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.sfpt = SRichCopySelection::SFPT_Skip ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_SAMEFILE_REPLACENEWER),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.sfpt = SRichCopySelection::SFPT_IfCondition ;
	}

	switch(m_SFIfConditonComboBox.GetSelectIndex())
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		m_ConfigShare.sfic.IfCondition = (SRichCopySelection::EFileDifferenceType)m_SFIfConditonComboBox.GetSelectIndex() ;
		break ;

	default:
		_ASSERT(FALSE) ;
		m_ConfigShare.sfic.IfCondition = SRichCopySelection::FDT_Newer ;
		break ;
	}
	
	switch(m_SFThenOperationComboBox.GetSelectIndex())
	{
	case 0:
	case 1:
	case 2:
	case 3:
		m_ConfigShare.sfic.ThenOperation = (SRichCopySelection::ESameFileOperationType)m_SFThenOperationComboBox.GetSelectIndex() ;
		break ;

	default:
		_ASSERT(FALSE) ;
		m_ConfigShare.sfic.ThenOperation = SRichCopySelection::SFOT_Replace ;
		break ;
	}

	switch(m_SFOtherwiseOperationComboBox.GetSelectIndex())
	{
	case 0:
	case 1:
	case 2:
	case 3:
		m_ConfigShare.sfic.OtherwiseOperation = (SRichCopySelection::ESameFileOperationType)m_SFOtherwiseOperationComboBox.GetSelectIndex() ;
		break ;
	default:
		_ASSERT(FALSE) ;
		m_ConfigShare.sfic.OtherwiseOperation = SRichCopySelection::SFOT_Ask ;
		break ;
	}

	// 遇到错误时的处理方式
	if(::SendMessage(this->GetDlgItem(IDC_RADIO_ERROR_ASKME),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.epc = SRichCopySelection::EPT_Ask ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO1_ERROR_EXIT),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.epc = SRichCopySelection::EPT_Exit ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_ERROR_IGNORE),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.epc = SRichCopySelection::EPT_Ignore ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_ERROR_RETRY),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.epc = SRichCopySelection::EPT_Retry ;
	}

	if(::SendMessage(this->GetDlgItem(IDC_RADIO1_RETRYTHENASK),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.RetryFailThen = SRichCopySelection::EPT_Ask ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO1_RETRYTHENEXIT),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.RetryFailThen = SRichCopySelection::EPT_Exit ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO1_RETRYTHENIGNORE),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_ConfigShare.RetryFailThen = SRichCopySelection::EPT_Ignore ;
	}

	m_ConfigShare.nRetryTimes = this->GetDlgItemInt(IDC_EDIT_RETRYMAXTIMES) ;
	
#endif
}



void CConfigurationDialog::SetSameFileProcCheck(SRichCopySelection::ESameFileProcessType sfpt)
{
	pt_STL_vector(UINT) CtrlIDVer ;
	int ControlID = 0 ;

	CtrlIDVer.push_back(IDC_RADIO_SAMEFILE_ASKME) ;
	CtrlIDVer.push_back(IDC_RADIO_SAMEFILE_RENAMEALL) ;
	CtrlIDVer.push_back(IDC_RADIO_SAMEFILE_REPLACEALL) ;
	CtrlIDVer.push_back(IDC_RADIO_SAMEFILE_SKIPALL) ;
	CtrlIDVer.push_back(IDC_RADIO_SAMEFILE_REPLACENEWER) ;

	switch(sfpt)
	{
	case SRichCopySelection::SFPT_Ask: ControlID = IDC_RADIO_SAMEFILE_ASKME ; break ;
	case SRichCopySelection::SFPT_Rename: ControlID = IDC_RADIO_SAMEFILE_RENAMEALL ; break ;
	case SRichCopySelection::SFPT_Replace: ControlID = IDC_RADIO_SAMEFILE_REPLACEALL ; break ;
	case SRichCopySelection::SFPT_Skip: ControlID = IDC_RADIO_SAMEFILE_SKIPALL ; break ;
	case SRichCopySelection::SFPT_IfCondition: ControlID = IDC_RADIO_SAMEFILE_REPLACENEWER ; break ;
	default: _ASSERT(FALSE) ; break ;
	}

	this->SetRadioCheck(CtrlIDVer,ControlID) ;
}

void CConfigurationDialog::SetErrorProcCheck(SRichCopySelection::EErrorProcessType ept)
{
	pt_STL_vector(UINT) CtrlIDVer ;
	int ControlID = 0 ;

	CtrlIDVer.push_back(IDC_RADIO_ERROR_ASKME) ;
	CtrlIDVer.push_back(IDC_RADIO1_ERROR_EXIT) ;
	CtrlIDVer.push_back(IDC_RADIO_ERROR_IGNORE) ;
	CtrlIDVer.push_back(IDC_RADIO_ERROR_RETRY) ;

	switch(ept)
	{
	case SRichCopySelection::EPT_Ask: ControlID = IDC_RADIO_ERROR_ASKME; break ;
	case SRichCopySelection::EPT_Exit: ControlID = IDC_RADIO1_ERROR_EXIT; break ;
	case SRichCopySelection::EPT_Ignore: ControlID = IDC_RADIO_ERROR_IGNORE; break ;
	case SRichCopySelection::EPT_Retry: ControlID = IDC_RADIO_ERROR_RETRY; break ;
	default: _ASSERT(FALSE) ; break ;
	}

	this->SetRadioCheck(CtrlIDVer,ControlID) ;
}

void CConfigurationDialog::SetRetryFailThenCheck(SRichCopySelection::EErrorProcessType sfpt)
{
	pt_STL_vector(UINT) CtrlIDVer ;
	int ControlID = 0 ;

	CtrlIDVer.push_back(IDC_RADIO1_RETRYTHENASK) ;
	CtrlIDVer.push_back(IDC_RADIO1_RETRYTHENEXIT) ;
	CtrlIDVer.push_back(IDC_RADIO1_RETRYTHENIGNORE) ;

	switch(sfpt)
	{
	case SRichCopySelection::EPT_Ask: ControlID = IDC_RADIO1_RETRYTHENASK; break ;
	case SRichCopySelection::EPT_Exit: ControlID = IDC_RADIO1_RETRYTHENEXIT; break ;
	case SRichCopySelection::EPT_Ignore: ControlID = IDC_RADIO1_RETRYTHENIGNORE; break ;
	default:_ASSERT(FALSE) ; break ;
	}

	this->SetRadioCheck(CtrlIDVer,ControlID) ;
}

void CConfigurationDialog::SetRadioCheck(pt_STL_vector(UINT) CtrlIDVer,UINT nCheckedID)
{
	for(unsigned int i=0;i<CtrlIDVer.size();++i)
	{
		UINT uChecked = (CtrlIDVer[i]==nCheckedID) ? BST_CHECKED : BST_UNCHECKED ;

		::SendMessage(this->GetDlgItem(CtrlIDVer[i]),BM_SETCHECK,uChecked,0) ;
	}
}

void CConfigurationDialog::OnButtonClick(int nButtonID) 
{
	switch(nButtonID)
	{
	case IDC_BUTTON_POSITIONDLG: // 启动位置设置对话框
		{
			CXCStartupPosDlg dlg(m_ConfigShare.ptStartupPos) ;

			if(dlg.ShowDialog()==CptDialog::DialogResult_OK)
			{
				m_ConfigShare.ptStartupPos = dlg.GetStartupPos() ;
			}
		}
		break ;

	case IDC_BUTTON_SELECTSOUNDFILE: // 指定音频文件
		{
			TCHAR szFile[8*MAX_PATH] = {0} ;

			OPENFILENAME ofn ;
			::memset(&ofn,0,sizeof(ofn)) ;

			ofn.lStructSize = sizeof(ofn) ;
			ofn.hwndOwner = this->GetSafeHwnd();
			ofn.lpstrFile = szFile;

			ofn.lpstrFile[0] = _T('\0');
			ofn.nMaxFile = sizeof(szFile)/sizeof(TCHAR);
			ofn.lpstrFilter = _T("WAV FILE\0*.wav\0MP3 FILE\0*.mp3\0All File\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if(::GetOpenFileName(&ofn))
			{
				this->SetDlgItemText(IDC_EDIT_SOUNDFILE,szFile) ;
			}
		}
		break ;

	case ID_BUTTON_OK:// 修改确认
		this->UpdateDataFromControl() ;
		if(m_nCurLanguageIndex!=m_nPreLanguageIndex)
		{
			CptString strTxt = CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_RESTARTFORUPDATELANGUAGE) ;
			CptString strTitle = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_APP) ;

			CptMessageBox::ShowMessage(this->GetSafeHwnd(),strTxt,strTitle,CptMessageBox::Button_OK) ;
		}

		CXCConfiguration::GetInstance()->SaveConfigDataToFile(m_ConfigData) ;
		CXCConfiguration::GetInstance()->SaveConfigShareToFile(m_ConfigShare) ;
		this->OnOK() ;
		break ;

//	case IDC_BUTTON_TITLECLOSE:
	case IDC_BUTTON_CANCEL:
		this->OnCancel() ;
		break ;

	case IDC_BUTTON_HELP: // 帮助
		{
			::LaunchHelpFile(HFP_ConfigurationDlg) ;
		}
		break ;
	}

}
//
//int CConfigurationDialog::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) 
//{
//	//switch(uMsg)
//	//{
//	//case WM_CTLCOLORBTN :
//	//case WM_CTLCOLOREDIT :
//	//case WM_CTLCOLORDLG :
//	//case WM_CTLCOLORSTATIC :
//	//case WM_CTLCOLORSCROLLBAR :
//	//case WM_CTLCOLORLISTBOX :
//	//	{
//	//		HDC hdc = (HDC)wParam ;
//
//	//		::SetBkMode(hdc,TRANSPARENT) ;
//	//		
//	//		//HBRUSH hBrush = ::CreateSolidBrush(RGB(113,204,255)) ;
//	//		//HBRUSH hBrush = ::CreateSolidBrush(RGB(181,211,255)) ;
//
//	//		//if((HWND)lParam==this->GetDlgItem(IDC_STATIC_CURRENTFILE))
//	//		//{// 当前复制的文件
//	//		//	//HFONT hFont = ::CreateFont(10,0,0,0,FW_BOLD,TRUE,FALSE,FALSE,DEFAULT_CHARSET,0,
//	//		//	//	CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;
//	//		//	::SelectObject(hdc,m_hCurFileFont) ;
//	//		//}
//	//		//else if((HWND)lParam==this->GetDlgItem(IDC_STATIC_TO))
//	//		//{
//	//		//	::SelectObject(hdc,m_hToTextFont) ;
//	//		//}
//	//		//else 
//	//		if(uMsg==WM_CTLCOLORSTATIC)
//	//		{
//	//			::SetTextColor(hdc,RGB(10,10,230)) ;
//	//		}
//
//	//		return (int)CXCConfiguration::GetInstance()->GetGlobalData()->hDlgBkBrush ;
//	//	}
//	//	break ;
//	//}
//
//	return CptDialog::OnProcessMessage(hWnd,uMsg,wParam,lParam) ;
//}