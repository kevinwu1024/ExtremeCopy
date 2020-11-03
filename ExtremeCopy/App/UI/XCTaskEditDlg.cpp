/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "..\CompileMacro.h"
#include "XCTaskEditDlg.h"
#include "..\Language\XCRes_ENU\resource.h"
#include "..\XCConfig.h"

#include "ptMessageBox.h"
#include "CptFileDialog.h"
#include <shellapi.h>
#include "..\XCGlobal.h"
#include "..\XCConfiguration.h"
#include "XCWildcardEditDlg.h"
#include "..\..\Common\ptWinPath.h"

void XCRunModeConfiguration() ;

void XCRunModeRegister() ;

void XCRunModeAbout() ;

/**
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32'name='Microsoft.Windows.Common-Controls' version='6.0.0.0'processorArchitecture='x86' publicKeyToken='6595b64144ccf1df'language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32'

name='Microsoft.Windows.Common-Controls' version='6.0.0.0'

processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df'

language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32'

name='Microsoft.Windows.Common-Controls' version='6.0.0.0'

processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df'

language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32'

name='Microsoft.Windows.Common-Controls' version='6.0.0.0'

processorArchitecture='*' publicKeyToken='6595b64144ccf1df'

language='*'\"")
#endif
#endif
/**/

CXCFileListBox::CXCFileListBox(HWND hWnd):CptListBox(hWnd)
{
}

int CXCFileListBox::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) 
{
	switch(nMsg)
	{
	case WM_KEYUP:
		{
			if(VK_DELETE==wParam)
			{
				this->RemoveSelItems() ;
			}
		}
		break ;
	}

	return CptListBox::PreProcCtrlMsg(hWnd,nMsg,wParam,lParam) ;
}

void CXCFileListBox::RemoveSelItems() 
{
	std::vector<int> IndexVer ;
	this->GetMultipleSelIndex(IndexVer) ;

	if(!IndexVer.empty())
	{
		std::sort(IndexVer.begin(),IndexVer.end()) ;

		int nIndexOffset = 0 ;

		for(size_t i=0;i<IndexVer.size();++i,++nIndexOffset)
		{
			this->DeleteString(IndexVer[i]-nIndexOffset) ;
		}
	}
}

CXCSrcFileListBox::CXCSrcFileListBox(HWND hWnd):CXCFileListBox(hWnd)
{
}


void CXCSrcFileListBox::OnDropFiles(HDROP hDropInfo)
{
	const int fileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	TCHAR szFileName[MAX_PATH] = { 0 };
	TCHAR szListString[MAX_PATH] = { 0 };

	for (int i = 0; i < fileCount; ++i)
	{
		::DragQueryFile(hDropInfo, i, szFileName, sizeof(szFileName));

		const int nListCount = this->GetItemCount() ;

		int j = 0 ;
		for(j=0;j<nListCount;++j)
		{
			szListString[0] = 0 ;
			this->GetString(j,szListString) ;

			CptString str1 = szListString ;
			CptString str2 = szFileName ;

			if(str1.CompareNoCase(str2)==0)
			{
				break ;
			}
		}

		if(j==nListCount)
		{
			this->AddString(szFileName) ;
		}
	}

	::DragFinish(hDropInfo) ;
}

CXCDstFolderListBox::CXCDstFolderListBox(HWND hWnd):CXCFileListBox(hWnd)
{
}

void CXCDstFolderListBox::OnDropFiles(HDROP hDropInfo)
{
#ifndef VERSION_PROFESSIONAL
	if(this->GetItemCount()>0)
	{// 若为 portable 版，则只支持一个 destination folder
		::DragFinish(hDropInfo) ;
		return ;
	}
#endif
	const int fileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	TCHAR szFileName[MAX_PATH] = { 0 };
	TCHAR szListString[MAX_PATH] = { 0 };

	for (int i = 0; i < fileCount; ++i)
	{
		::DragQueryFile(hDropInfo, i, szFileName, sizeof(szFileName));

		if(CptGlobal::IsFolder(szFileName))
		{
			const int nListCount = this->GetItemCount() ;

			int j = 0 ;
			for(j=0;j<nListCount;++j)
			{
				szListString[0] = 0 ;
				this->GetString(j,szListString) ;

				CptString str1 = szListString ;
				CptString str2 = szFileName ;

				if(str1.CompareNoCase(str2)==0)
				{
					break ;
				}
			}

			if(j==nListCount)
			{
				this->AddString(szFileName) ;
			}
		}
	}

	::DragFinish(hDropInfo) ;
}


CXCTaskEditDlg::CXCTaskEditDlg(const SXCCopyTaskInfo& task,int nDlgID):CptDialog(nDlgID,NULL,CptMultipleLanguage::GetInstance()->GetResourceHandle())
{
	m_TaskInfo = task ;
	m_hTaskMenu = NULL ;
}

CXCTaskEditDlg::~CXCTaskEditDlg(void)
{
	if(m_hTaskMenu!=NULL)
	{
		::DestroyMenu(m_hTaskMenu) ;
		m_hTaskMenu = NULL ;
	}
}


BOOL CXCTaskEditDlg::OnInitDialog() 
{
	this->CenterScreen() ;

	TCHAR szBuf[512] = {0} ;
	CptString strTxt ;

	// 计算出 运行 按钮底部再加上标题栏位置，以这个位置作为界面的分界线
	// 这样就能准备把界面折叠起来
	SptRect RunButtonRect ;

	::GetWindowRect(this->GetSafeHwnd(),m_OrginRect.GetRECTPointer()) ;

	HWND hRunButton = ::GetDlgItem(this->GetSafeHwnd(),IDC_BUTTON_MORELESS) ;
	::GetWindowRect(hRunButton,RunButtonRect.GetRECTPointer()) ;
	SptPoint ptBottom = SptPoint(RunButtonRect.nRight,RunButtonRect.nBottom) ;
	::ScreenToClient(this->GetSafeHwnd(),ptBottom.GetPOINTPointer()) ;

	m_nLessDlgHeight = m_OrginRect.GetHeight() - (ptBottom.nY + ::GetSystemMetrics(SM_CYCAPTION) +  ::GetSystemMetrics(SM_CYMENU) + RunButtonRect.GetHeight());

	::SetWindowPos(this->GetSafeHwnd(),HWND_TOP,0,0,m_OrginRect.GetWidth(),m_OrginRect.GetHeight()-m_nLessDlgHeight,SWP_NOZORDER|SWP_NOMOVE) ;
	m_bMoreOrLess = false ; // less

	m_RunSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_RUNCOPY)) ;
	m_MoveSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_RUNMOVE)) ;

#ifdef VERSION_PROFESSIONAL // 只有专业版才有这个功能
	::ShowWindow(this->GetDlgItem(IDC_STATIC_GETPRO),SW_HIDE) ;
	m_MoreLessSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_MORELESS)) ;

	m_ExportToFileSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_EXPORTTOFILE)) ;

	strTxt = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TOOLTIP_EXPORTFILEBUTTON) ;
	::_tcscpy(szBuf,strTxt.c_str()) ;
	m_ExportButtonTooltip.Create(this->GetSafeHwnd(),true) ;
	m_ExportButtonTooltip.BindControl(m_ExportToFileSkinButton.GetSaftHwnd(),szBuf) ;

	m_ImportFromFileSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_IMPORTFROMFILE)) ;

	strTxt = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TOOLTIP_IMPORTFILE) ;
	::_tcscpy(szBuf,strTxt.c_str()) ;
	m_ImportButtonTooltip.Create(this->GetSafeHwnd(),true) ;
	m_ImportButtonTooltip.BindControl(m_ImportFromFileSkinButton.GetSaftHwnd(),szBuf) ;

#else
	{
		HYPERLINKSTRUCT2 hls = {0};

		hls.fontsize  = 14;					// Size of font used
		hls.bBold     = false;				// Bold font?
		hls.bUnderline = true;				// Underlined font?
		hls.bUseBg    = false;				// Use opaque background?
		hls.bUseCursor = true;				// Use custom cursor?
		hls.hCursor    = CXCConfiguration::GetInstance()->GetGlobalData()->hCursorHand ;//::LoadCursor( GetModuleHandle( NULL ), (LPCTSTR)IDC_CURSOR_HAND );
		hls.clrBack   = RGB(0,0,0);			// Fill this out if bUseBg is true
		hls.clrHilite = RGB( 0, 0, 255 );	// Color of mouseover font
		hls.clrText   = RGB( 0, 0, 255 );	// Color of regular font
		hls.hControlWnd = this->GetDlgItem(IDC_STATIC_GETPRO) ;

		hls.pFn       = OnHyperLinkHomePageCallBack2;		// Function pointer to function that is called when link is clicked
		hls.pCBAgr = (void*)WEBLINK_SITE_GETPROEDITION ;
		hls.szFontName = FONT_NAME_DEFAULT;			// Font face name

		m_Hyperlink.Create(&hls) ;
	}

	::ShowWindow(this->GetDlgItem(IDC_BUTTON_MORELESS),SW_HIDE) ;
	::ShowWindow(this->GetDlgItem(IDC_BUTTON_EXPORTTOFILE),SW_HIDE) ;
	::ShowWindow(this->GetDlgItem(IDC_BUTTON_IMPORTFROMFILE),SW_HIDE) ;
#endif

	// 显示版本号到标题栏
	CptString strVersion = ::MakeXCVersionString() ;

	if(strVersion.GetLength()>0)
	{
		::SetWindowText(this->GetSafeHwnd(),strVersion.c_str()) ;
	}

	m_SourceListBox.Attach(this->GetDlgItem(IDC_LIST_SOURCEFILE)) ;
	m_DestListBox.Attach(this->GetDlgItem(IDC_LIST_DESTINATION)) ;

	::DragAcceptFiles(this->GetDlgItem(IDC_LIST_SOURCEFILE),TRUE) ;
	::DragAcceptFiles(this->GetDlgItem(IDC_LIST_DESTINATION),TRUE) ;

#if !defined(VERSION_PROFESSIONAL) || !defined(VERSION_CHECKREGSITER)
	HMENU hMainMenu = ::GetMenu(this->GetSafeHwnd()) ;

	HMENU hSubMenu = ::GetSubMenu(hMainMenu,0) ;

	::RemoveMenu(hSubMenu,ID_MENU_REGISTER,MF_BYCOMMAND) ;
#endif

	m_SFIfConditonComboBox.Attach(this->GetDlgItem(IDC_COMBO_IFCONDITION)) ;
	m_SFThenOperationComboBox.Attach(this->GetDlgItem(IDC_COMBO_THENOPERATION)) ;
	m_SFOtherwiseOperationComboBox.Attach(this->GetDlgItem(IDC_COMBO_OTHERWISEOPERATOIN)) ;

	CptString str ;

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

	this->UpdateDataToControls() ;

	return CptDialog::OnInitDialog() ;
}

void CXCTaskEditDlg::OnPaint() 
{
	CptDialog::OnPaint() ;

#ifndef VERSION_PROFESSIONAL // 非专业版
	m_Hyperlink.Update() ;
#endif
}


void CXCTaskEditDlg::Run(bool bCopyOrMove) 
{
	this->UpdateDataFromControls() ;

	bool bExist = true ;

	for(size_t i=0;i<m_TaskInfo.strDstFolderVer.size();++i)
	{
		if(!IsFileExist(m_TaskInfo.strDstFolderVer[i].c_str()))
		{
			bExist = false ;
			break ;
		}
	}

	if(bExist)
	{
		if(m_SourceListBox.GetItemCount()>0)
		{
			m_TaskInfo.cmd = SXCCopyTaskInfo::XCMD_XtremeRun ;
			m_TaskInfo.CopyType = bCopyOrMove ? SXCCopyTaskInfo::RT_Copy : SXCCopyTaskInfo::RT_Move ;
			m_TaskInfo.CreatedBy = SXCCopyTaskInfo::TCT_TaskDialog ;

			int nSrcProblemIndex = 0 ;
			int nDstProblemIndex = 0 ;

			// 存在文件夹递归复制的情况
			if(::DoesIncludeRecuriseFolder(m_TaskInfo,nSrcProblemIndex,nDstProblemIndex))
			{
				CptString strSrcFolder = m_TaskInfo.strSrcFileVer[nSrcProblemIndex] ;
				CptString strDstFolder = m_TaskInfo.strDstFolderVer[nDstProblemIndex] ;

				CptString strText ;

				CptString strTitle = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_ERROR) ;
				CptString strFormat = CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_RECURSIVEFOLDERERROR) ;
				strText.Format(strFormat.c_str(),strSrcFolder,strDstFolder) ;

				CptMessageBox::ShowMessage(this->GetSafeHwnd(),strText,strTitle,CptMessageBox::Button_OK) ;

				return ;
			}
			else
			{
				this->PerformButton(IDOK) ;
			}
		}
		else
		{
			CptString strText = ::CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_INPUTSOURCEFILE) ;
			CptString strTitle = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;
			CptMessageBox::ShowMessage(this->GetSafeHwnd(),strText,strTitle,CptMessageBox::Button_OK) ;
		}
	}
	else
	{
		CptString strText = ::CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_INPUTDESTINATIONFOLDER) ;

		CptString strTitle = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;
		CptMessageBox::ShowMessage(this->GetSafeHwnd(),strText,strTitle,CptMessageBox::Button_OK) ;
	}
}

void CXCTaskEditDlg::OnListBoxItemDoubleClicked(int nID) 
{
	if(nID==IDC_LIST_SOURCEFILE && this->CheckListStringIsFolderOrWildcard())
	{
		this->ShowWildcardDlg() ;
	}
}

bool CXCTaskEditDlg::CheckListStringIsFolderOrWildcard()
{
	bool bRet = false ;
	const int nCurIndex = m_SourceListBox.GetSelectIndex() ;

	if(nCurIndex>=0)
	{
		TCHAR szBuf[MAX_PATH] = {0} ;

		if(m_SourceListBox.GetString(nCurIndex,szBuf))
		{
			const EInterestFileType iftType = ::GetInterestFileType(szBuf) ;

			BOOL bEnable = (iftType==IFT_Folder || iftType==IFT_FolderWithWildcard) ;

			HWND hWnd = this->GetDlgItem(IDC_BUTTON_WILDCARD2) ;
			::EnableWindow(hWnd,bEnable) ;

			bRet = bEnable ? true : false ;
		}
	}

	return bRet ;
}

void CXCTaskEditDlg::OnListBoxItemSelectChanged(int nID,HWND hListBoxWnd) 
{
	if(nID==IDC_LIST_SOURCEFILE)
	{
		this->CheckListStringIsFolderOrWildcard() ;
	}
}

void CXCTaskEditDlg::ShowMenu(int nMenuIndex)
{
	{
		UINT ButtonID = 0 ;

		switch(nMenuIndex)
		{
		case 0: ButtonID = IDC_BUTTON_EXPORTTOFILE ; break ;
		case 1: ButtonID = IDC_BUTTON_BROWSESRCFILE ; break ;
		case 2: ButtonID = IDC_BUTTON_MAINMENU ; break ;
		}

		if(m_hTaskMenu==NULL)
		{
			m_hTaskMenu = ::CptMultipleLanguage::GetInstance()->GetMenu(IDR_MENU1) ;
		}

		if(m_hTaskMenu!=NULL)
		{
			SptRect rt ;
			::GetWindowRect(this->GetDlgItem(ButtonID),rt.GetRECTPointer()) ;

			HMENU hSubMenu = ::GetSubMenu(m_hTaskMenu,nMenuIndex) ;

#ifdef VERSION_PORTABLE
			if(ButtonID==IDC_BUTTON_MAINMENU)
			{
				::RemoveMenu(hSubMenu,ID_MENU_REGISTER,MF_BYCOMMAND) ;
			}
#endif

			::TrackPopupMenu(hSubMenu,TPM_LEFTALIGN,rt.nLeft,rt.nTop+rt.GetHeight(),0,this->GetSafeHwnd(),NULL) ;
		}
	}
}

void CXCTaskEditDlg::OnButtonClick(int nButtonID) 
{
	switch(nButtonID)
	{
	case IDC_BUTTON_DELETEDST: // 移除目标文件夹
		{
			m_DestListBox.RemoveSelItems() ;
		}
		break ;

	case IDC_BUTTON_DELETE: // 移除源码文件
		{
			m_SourceListBox.RemoveSelItems() ;
		}

		break ;

	case ID_MENU_OPENSRCFILE:// 源文件
		{
			CptFileDialog dlg ;

			DWORD dwFlag = EOPEN_MULTISELECTED ;
			CptString strAllFiles = ::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_ALLFILE) ;
			strAllFiles = strAllFiles + _T("\0*.*\0") ;

			//if(dlg.Open(true,_T("All File\0*.*\0"),dwFlag,this->GetSafeHwnd()))
			if(dlg.Open(true,false,strAllFiles.c_str(),dwFlag,this->GetSafeHwnd()))
			{
				CptStringList sl = dlg.GetSelectedFiles() ;

				for(int i=0;i<sl.GetCount();++i)
				{
					m_SourceListBox.AddString(sl[i].c_str()) ;
				}
			}
		}
		break ;

	case ID_MENU_OPENSRCFOLDER: // 源文件夹
		{
			CptFileDialog dlg ;

			DWORD dwFlag = 0 ;
			if(dlg.Open(false,false,NULL,dwFlag,this->GetSafeHwnd()))
			{
				CptStringList sl = dlg.GetSelectedFiles() ;

				if(sl.GetCount()>0)
				{
					m_SourceListBox.AddString(sl[0].c_str()) ;
				}
			}
		}
		break ;

	case IDC_BUTTON_BROWSESRCFILE: // 
		{
			this->ShowMenu(1) ; // 打开源文件或源文件夹菜单
		}
		break ;

	case IDC_BUTTON_BROWSEDSTFOLDER: // 目标文件夹
		{
#ifdef VERSION_PORTABLE
			if(m_DestListBox.GetItemCount()>0)
			{
				break ;
			}
#endif
			CptFileDialog dlg ;

			DWORD dwFlag = 0 ;
			if(dlg.Open(false,false,NULL,dwFlag,this->GetSafeHwnd()))
			{
				CptStringList sl = dlg.GetSelectedFiles() ;

				if(sl.GetCount()>0)
				{
					//this->SetDlgItemText(IDC_EDIT_DESTINATIONFOLDER,sl[0].c_str()) ;
					m_DestListBox.AddString(sl[0].c_str()) ;
				}
			}
		}
		break ;

	//case IDC_BUTTON_MAINMENU: // 主菜单
	//	{
	//		this->ShowMenu(2) ; // 打开源文件或源文件夹菜单
	//	}
	//	break ;

	case ID_MENU_CONFIGURATION: // configuration
		::XCRunModeConfiguration() ;
		break ;

	case ID_MENU_REGISTER: // 注册
		if(CptGlobal::IsNeedElevatePrivilege())
		{
			CptString str = CptWinPath::GetStartupPath() ;
			::ShellExecute(NULL,_T("runas"),_T("XCopyVISTA.exe"),SWITCH_XCopyVISTA_REGISTER,str.c_str(),SW_SHOW) ;
		}
		else
		{
			::XCRunModeRegister() ;
		}
		//
		break ;

	case ID_MENU_ABOUT: // about
		::XCRunModeAbout() ;
		break ;

	case IDC_BUTTON_WILDCARD2: // 通配符按钮
		{
			this->ShowWildcardDlg() ;
		}
		break ;

	case IDC_BUTTON_RUNMOVE: // move
		this->Run(false) ;
		break ;

	case IDC_BUTTON_RUNCOPY: // copy
		this->Run(true) ;
		break ;

	case IDC_BUTTON_CLEAR: // clear source
		m_SourceListBox.Clear() ;
		break ;

	case IDC_BUTTON_CLEARDST: // clear destination
		m_DestListBox.Clear() ;
		break ;

#ifdef VERSION_PROFESSIONAL // 专业版

	case IDC_BUTTON_MORELESS: // more or less
		if(m_bMoreOrLess)
		{// 转到 less
			this->SetDlgItemText(IDC_BUTTON_MORELESS,CptMultipleLanguage::GetInstance()->GetString(IDS_BUTTON_MORE)) ;
			SptSize size(m_OrginRect.GetWidth(),m_OrginRect.GetHeight()-m_nLessDlgHeight) ;
			this->SetWindowSize(size) ;
		}
		else
		{// 转到 more
			this->SetDlgItemText(IDC_BUTTON_MORELESS,CptMultipleLanguage::GetInstance()->GetString(IDS_BUTTON_LESS)) ;
			this->SetWindowSize(m_OrginRect.GetSize()) ;
		}

		m_bMoreOrLess = !m_bMoreOrLess ;

		break ;

	case ID_MENU_ASCOPYTASK: // 保存为复制任务文件
		this->SaveTaskFile(SXCCopyTaskInfo::RT_Copy) ;
		break ; 

	case ID_MENU_ASMOVETASK: // 保存为移动任务文件
		this->SaveTaskFile(SXCCopyTaskInfo::RT_Move) ;
		break ;

	case IDC_BUTTON_EXPORTTOFILE: // 导出到文件
		{
			this->ShowMenu(0) ;
		}
		break ;

	case IDC_BUTTON_IMPORTFROMFILE: // 从文件导入
		{
			CptFileDialog dlg ;

			DWORD dwFlag = 0 ;
			if(dlg.Open(true,false,_T(".xct\0*.xct\0"),dwFlag,this->GetSafeHwnd()))
			{
				const CptStringList& sl = dlg.GetSelectedFiles() ;
				CptString strFileName = sl[0] ;

				//const CptString& strFileName = dlg.GetSelectedFiles()[0] ;
				pt_STL_vector(SXCCopyTaskInfo) TaskVer ;

				ETaskXMLErrorType result = CXCCopyTask::ConvertXMLFile2TaskInfo(strFileName.c_str(),TaskVer) ;

				if(result==TXMLET_Success)
				{
					m_TaskInfo = TaskVer[0] ;
					this->UpdateDataToControls() ;

					this->UpdateCopyOrMoveTaskView() ;
				}
				else
				{
					CptString strError = ::GetTaskXMLErrorString(result) ;
					CptString strTitle = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_ERROR) ;

					CptMessageBox::ShowMessage(this->GetSafeHwnd(),strError,strTitle,CptMessageBox::Button_OK) ;
				}
			}
		}
		break ;

#endif

	case ID_MENU_HELP:
	case IDC_BUTTON_HELP: // 帮助
		{
			::LaunchHelpFile(HFP_MainMenu) ;
		}
		break ;

	//case IDC_BUTTON_TITLEMIN: // 最小化
	//	this->PostMessage(WM_SYSCOMMAND,SC_MINIMIZE,0) ;
	//	break ;

	case IDC_BUTTON_EXIT:// 退出
		this->PerformButton(IDCANCEL) ;
		break ;
	}
}

void CXCTaskEditDlg::UpdateCopyOrMoveTaskView()
{
	if(m_TaskInfo.CopyType==SXCCopyTaskInfo::RT_Copy)
	{
		m_RunSkinButton.SetTextBold(true) ;
		m_MoveSkinButton.SetTextBold(false) ;
	}
	else if(m_TaskInfo.CopyType==SXCCopyTaskInfo::RT_Move)
	{
		m_RunSkinButton.SetTextBold(false) ;
		m_MoveSkinButton.SetTextBold(true) ;
	}
}

// 显示通配符对话框
void CXCTaskEditDlg::ShowWildcardDlg() 
{
	const int nCurIndex = m_SourceListBox.GetSelectIndex() ;

	if(nCurIndex>=0)
	{
		TCHAR szBuf[MAX_PATH] = {0} ;

		if(m_SourceListBox.GetString(nCurIndex,szBuf))
		{
			CXCWildcardEditDlg dlg(szBuf,this->GetSafeHwnd()) ;

			if(dlg.ShowDialog()==CptDialog::DialogResult_OK)
			{
				CptString strWildcard = dlg.GetPathWithWildcard() ;

				m_SourceListBox.UpdateString(nCurIndex,strWildcard.c_str()) ;
			}
		}
	}
}

// 保存为任务文件
void CXCTaskEditDlg::SaveTaskFile(SXCCopyTaskInfo::EExecuteType et)
{
	CptFileDialog dlg ;

	DWORD dwFlag = 0 ;
	if(dlg.Open(true,true,_T(".xct\0*.xct\0"),dwFlag,this->GetSafeHwnd()))
	{
		this->UpdateDataFromControls() ;

		pt_STL_vector(SXCCopyTaskInfo) TaskVer ;

		SXCCopyTaskInfo TaskTem = m_TaskInfo ;
		TaskTem.CopyType = et ;
		TaskVer.push_back(TaskTem) ;
		const CptStringList& sl = dlg.GetSelectedFiles() ;
		CptString strFileName = sl[0] ;

		const CptString strTem = SUFFIX_XC_TASK_FILE ;

		if(strFileName.GetLength()<4 || strTem.CompareNoCase(strFileName.c_str()+strFileName.GetLength()-4)!=0)
		{
			strFileName = strFileName + SUFFIX_XC_TASK_FILE ;
		}

		const TCHAR* pp = strFileName.c_str() ;
		CXCCopyTask::ConvertTaskInfo2XMLFile(TaskVer,strFileName) ;
	}
}

void CXCTaskEditDlg::SetRadioCheck(pt_STL_vector(UINT) CtrlIDVer,UINT nCheckedID)
{
	for(unsigned int i=0;i<CtrlIDVer.size();++i)
	{
		UINT uChecked = (CtrlIDVer[i]==nCheckedID) ? BST_CHECKED : BST_UNCHECKED ;

		::SendMessage(this->GetDlgItem(CtrlIDVer[i]),BM_SETCHECK,uChecked,0) ;
	}
}

void CXCTaskEditDlg::SetSameFileProcCheck(SRichCopySelection::ESameFileProcessType sfpt)
{
	pt_STL_vector(UINT) CtrlIDVer ;
	int ControlID = 0 ;

	CtrlIDVer.push_back(IDC_RADIO_SAMEFILEASK) ;
	CtrlIDVer.push_back(IDC_RADIO_SAMEFILERENAME) ;
	CtrlIDVer.push_back(IDC_RADIO_SAMEFILEREPLACE) ;
	CtrlIDVer.push_back(IDC_RADIO_SAMEFILESKIP) ;
	CtrlIDVer.push_back(IDC_RADIO_SAMEFILEREPLACENEWER) ;

	switch(sfpt)
	{
	case SRichCopySelection::SFPT_IfCondition: ControlID = IDC_RADIO_SAMEFILEREPLACENEWER ; break ;
	case SRichCopySelection::SFPT_Ask: ControlID = IDC_RADIO_SAMEFILEASK ; break ;
	case SRichCopySelection::SFPT_Rename: ControlID = IDC_RADIO_SAMEFILERENAME ; break ;
	case SRichCopySelection::SFPT_Replace: ControlID = IDC_RADIO_SAMEFILEREPLACE ; break ;
	case SRichCopySelection::SFPT_Skip: ControlID = IDC_RADIO_SAMEFILESKIP ; break ;
	default: _ASSERT(FALSE) ; break ;
	}

	this->SetRadioCheck(CtrlIDVer,ControlID) ;
}

void CXCTaskEditDlg::SetErrorProcCheck(SRichCopySelection::EErrorProcessType ept)
{
	pt_STL_vector(UINT) CtrlIDVer ;
	int ControlID = 0 ;

	CtrlIDVer.push_back(IDC_RADIO_ERRORASK) ;
	CtrlIDVer.push_back(IDC_RADIO1_ERROREXIT) ;
	CtrlIDVer.push_back(IDC_RADIO_ERRORIGNORE) ;
	CtrlIDVer.push_back(IDC_RADIO_ERRORRETRY) ;

	switch(ept)
	{
	case SRichCopySelection::EPT_Ask: ControlID = IDC_RADIO_ERRORASK; break ;
	case SRichCopySelection::EPT_Exit: ControlID = IDC_RADIO1_ERROREXIT; break ;
	case SRichCopySelection::EPT_Ignore: ControlID = IDC_RADIO_ERRORIGNORE; break ;
	case SRichCopySelection::EPT_Retry: ControlID = IDC_RADIO_ERRORRETRY; break ;
	default: _ASSERT(FALSE) ; break ;
	}

	this->SetRadioCheck(CtrlIDVer,ControlID) ;
}

void CXCTaskEditDlg::SetShowModeCheck(SXCCopyTaskInfo::EShowMode sm)
{
	pt_STL_vector(UINT) CtrlIDVer ;
	int ControlID = 0 ;

	CtrlIDVer.push_back(IDC_RADIO_SHOWMODETRAY) ;
	CtrlIDVer.push_back(IDC_RADIO_SHOWMODEWINDOW) ;

	switch(sm)
	{
	case SXCCopyTaskInfo::ST_Tray: ControlID = IDC_RADIO_SHOWMODETRAY ;break ;

	case SXCCopyTaskInfo::ST_Window: ControlID = IDC_RADIO_SHOWMODEWINDOW ;break ;
	default: _ASSERT(FALSE) ; break ;
	}

	this->SetRadioCheck(CtrlIDVer,ControlID) ;
}

void CXCTaskEditDlg::SetRetryFailThenCheck(SRichCopySelection::EErrorProcessType sfpt)
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

void CXCTaskEditDlg::UpdateDataToControls() 
{
	// 源文件
	m_SourceListBox.Clear() ;

	if(!m_TaskInfo.strSrcFileVer.empty())
	{
		for(unsigned int i=0;i<m_TaskInfo.strSrcFileVer.size();++i)
		{
			m_SourceListBox.AddString(m_TaskInfo.strSrcFileVer[i].c_str()) ;
		}
	}

	// 目标文件夹
	m_DestListBox.Clear() ;

	if(!m_TaskInfo.strSrcFileVer.empty())
	{
		for(unsigned int i=0;i<m_TaskInfo.strDstFolderVer.size();++i)
		{
			m_DestListBox.AddString(m_TaskInfo.strDstFolderVer[i].c_str()) ;
		}
	}

	pt_STL_vector(UINT) CtrlIDVer ;

	int ControlID = 0 ;

	this->SetSameFileProcCheck(m_TaskInfo.ConfigShare.sfpt) ;
	this->SetErrorProcCheck(m_TaskInfo.ConfigShare.epc) ;
	this->SetRetryFailThenCheck(m_TaskInfo.ConfigShare.RetryFailThen) ;

	this->SetShowModeCheck(m_TaskInfo.ShowMode) ;

	this->SetDlgItemInt(IDC_EDIT_RETRYMAXTIMES,m_TaskInfo.ConfigShare.nRetryTimes) ;

	::SendMessage(this->GetDlgItem(IDC_CHECK_VERIFYDATA),BM_SETCHECK,m_TaskInfo.ConfigShare.bVerifyData?BST_CHECKED:BST_UNCHECKED,0) ;

	this->SetDlgItemText(IDC_EDIT_FINISHEVENT,m_TaskInfo.strFinishEvent.c_str()) ;


	m_SFIfConditonComboBox.SetSelectIndex(m_TaskInfo.ConfigShare.sfic.IfCondition) ;
	m_SFThenOperationComboBox.SetSelectIndex(m_TaskInfo.ConfigShare.sfic.ThenOperation) ;
	m_SFOtherwiseOperationComboBox.SetSelectIndex(m_TaskInfo.ConfigShare.sfic.OtherwiseOperation) ;
}

void CXCTaskEditDlg::UpdateDataFromControls() 
{
	TCHAR szDstFolder[MAX_PATH] = {0} ;

	// 源文件
	int nItemCount = m_SourceListBox.GetItemCount() ;

	m_TaskInfo.strSrcFileVer.clear() ;
	for(int i=0;i<nItemCount; ++i)
	{
		m_SourceListBox.GetString(i,szDstFolder) ;
		m_TaskInfo.strSrcFileVer.push_back(szDstFolder) ;
	}

	// 目标文件夹
	nItemCount = m_DestListBox.GetItemCount() ;

	m_TaskInfo.strDstFolderVer.clear() ;
	for(int i=0;i<nItemCount; ++i)
	{
		m_DestListBox.GetString(i,szDstFolder) ;
		m_TaskInfo.strDstFolderVer.push_back(szDstFolder) ;
	}

	//m_TaskInfo.ConfigShare.sfpt = SRichCopySelection::SFPT_Ask ;
	// 遇到相同文件时的处理方式
	if(::SendMessage(this->GetDlgItem(IDC_RADIO_SAMEFILEASK),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.sfpt = SRichCopySelection::SFPT_Ask ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_SAMEFILERENAME),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.sfpt = SRichCopySelection::SFPT_Rename ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_SAMEFILEREPLACE),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.sfpt = SRichCopySelection::SFPT_Replace ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_SAMEFILESKIP),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.sfpt = SRichCopySelection::SFPT_Skip ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_SAMEFILEREPLACENEWER),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.sfpt = SRichCopySelection::SFPT_IfCondition ;
	}


	switch(m_SFIfConditonComboBox.GetSelectIndex())
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		m_TaskInfo.ConfigShare.sfic.IfCondition = (SRichCopySelection::EFileDifferenceType)m_SFIfConditonComboBox.GetSelectIndex() ;
		break ;

	default:
		_ASSERT(FALSE) ;
		m_TaskInfo.ConfigShare.sfic.IfCondition = SRichCopySelection::FDT_Newer ;
		break ;
	}

	switch(m_SFThenOperationComboBox.GetSelectIndex())
	{
	case 0:
	case 1:
	case 2:
	case 3:
		m_TaskInfo.ConfigShare.sfic.ThenOperation = (SRichCopySelection::ESameFileOperationType)m_SFThenOperationComboBox.GetSelectIndex() ;
		break ;

	default:
		_ASSERT(FALSE) ;
		m_TaskInfo.ConfigShare.sfic.ThenOperation = SRichCopySelection::SFOT_Replace ;
		break ;
	}

	switch(m_SFOtherwiseOperationComboBox.GetSelectIndex())
	{
	case 0:
	case 1:
	case 2:
	case 3:
		m_TaskInfo.ConfigShare.sfic.OtherwiseOperation = (SRichCopySelection::ESameFileOperationType)m_SFOtherwiseOperationComboBox.GetSelectIndex() ;
		break ;
	default:
		_ASSERT(FALSE) ;
		m_TaskInfo.ConfigShare.sfic.OtherwiseOperation = SRichCopySelection::SFOT_Ask ;
		break ;
	}


	// 遇到出错时的处理方式
	if(::SendMessage(this->GetDlgItem(IDC_RADIO_ERRORASK),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.epc = SRichCopySelection::EPT_Ask ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO1_ERROREXIT),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.epc = SRichCopySelection::EPT_Exit ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_ERRORIGNORE),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.epc = SRichCopySelection::EPT_Ignore ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_ERRORRETRY),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.epc = SRichCopySelection::EPT_Retry ;
	}

	if(::SendMessage(this->GetDlgItem(IDC_RADIO1_RETRYTHENASK),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.RetryFailThen = SRichCopySelection::EPT_Ask ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO1_RETRYTHENEXIT),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.RetryFailThen = SRichCopySelection::EPT_Exit ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO1_RETRYTHENIGNORE),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ConfigShare.RetryFailThen = SRichCopySelection::EPT_Ignore ;
	}

	m_TaskInfo.ConfigShare.nRetryTimes = this->GetDlgItemInt(IDC_EDIT_RETRYMAXTIMES) ;

	if(::SendMessage(this->GetDlgItem(IDC_RADIO_SHOWMODETRAY),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ShowMode = SXCCopyTaskInfo::ST_Tray ;
	}
	else if(::SendMessage(this->GetDlgItem(IDC_RADIO_SHOWMODEWINDOW),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{
		m_TaskInfo.ShowMode = SXCCopyTaskInfo::ST_Window ;
	}

	m_TaskInfo.ConfigShare.bVerifyData = (::SendMessage(this->GetDlgItem(IDC_CHECK_VERIFYDATA),BM_GETCHECK,0,0)==BST_CHECKED) ;

	int nEventTxtLen = ::GetWindowTextLength(this->GetDlgItem(IDC_EDIT_FINISHEVENT)) ;

	if(nEventTxtLen>0)
	{
		TCHAR* pEvent = new TCHAR[nEventTxtLen+sizeof(TCHAR)] ;

		this->GetDlgItemText(IDC_EDIT_FINISHEVENT,pEvent,nEventTxtLen+sizeof(TCHAR)) ;

		m_TaskInfo.strFinishEvent = pEvent ;

		delete [] pEvent ;
	}

}

int CXCTaskEditDlg::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	//Debug_Printf(_T("CXCTaskEditDlg::OnProcessMessage() msg=0x%x"),uMsg) ;

	//switch(uMsg)
	//{

	//case WM_CTLCOLORBTN :
	//case WM_CTLCOLOREDIT :
	//case WM_CTLCOLORDLG :
	//case WM_CTLCOLORSTATIC :
	//case WM_CTLCOLORSCROLLBAR :
	//case WM_CTLCOLORLISTBOX :
	//	{
	//		HDC hdc = (HDC)wParam ;

	//		::SetBkMode(hdc,TRANSPARENT) ;
	//		if(uMsg==WM_CTLCOLORSTATIC)
	//		{
	//			::SetTextColor(hdc,RGB(10,10,230)) ;
	//		}

	//		return (int)CXCConfiguration::GetInstance()->GetGlobalData()->hDlgBkBrush ;
	//	}
	//	break ;
	//}

	return CptDialog::OnProcessMessage(hWnd,uMsg,wParam,lParam) ;
}