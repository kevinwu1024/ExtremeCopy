/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/
#include "StdAfx.h"

#include <process.h>
#include <shlobj.h>

#include "MainDialog.h"
#include "SameFileDialog.h"
#include "ptMessageBox.h"

#include "AboutDlg.h"

#include "..\CompileMacro.h"
#include "..\AppRegister.h"
#include "..\Language\XCRes_ENU\resource.h"
#include "..\XCConfiguration.h"
#include "..\ptMultipleLanguage.h"


#pragma data_seg("XCStartupPosition") // "XCShellNamePipeNumber"  为自定义
static unsigned int s_nStartupPostion = 0;
#pragma data_seg()
#pragma comment(linker,"/SECTION:XCStartupPosition,RWS")
	
const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));


CMainDialog::CMainDialog(bool bAutoRun,int nDlgID):CptDialog(nDlgID,NULL,CptMultipleLanguage::GetInstance()->GetResourceHandle())
{
	m_bIniShow = false ;
	m_pSpeedDlg = NULL ;
	m_DialogTemplateID = nDlgID ;

	m_pCurUIState = NULL ;
	m_bSimpleUI = false ;
	m_pTransApp = NULL ;

	m_bVerifyPhase = false ;
	m_bExit = false ;

	m_hCurFileFont = ::CreateFont(15,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;

	m_hToTextFont = ::CreateFont(13,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,
					CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;

	m_UISpeedInfo.bInMsgQueue = false ;
	m_UIFileNameInfo.bInMsgQueue = false ;
	m_bAutoRun = bAutoRun ;
}

CMainDialog::~CMainDialog(void)
{
	SAFE_DELETE_GDI(m_hCurFileFont) ;
	SAFE_DELETE_GDI(m_hToTextFont) ;

#ifdef VERSION_PROFESSIONAL
	m_UpArrowBitmapInfo.Release() ;
	m_DownArrowBitmapInfo.Release() ;
#endif
}

void CMainDialog::SetStartupPosition()
{
#ifdef _DEBUG
	this->CenterScreen() ;
	//::SetWindowPos(this->GetSafeHwnd(),HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE) ;
#else
	// 如果指定的窗口启动位置不是默认的，则再从默认的位置进行重新调整
	if(m_ConfigData.DlgStartupPos.nX<0 || m_ConfigData.DlgStartupPos.nX>(::GetSystemMetrics(SM_CXSCREEN)-50))
	{
		m_ConfigData.DlgStartupPos.nX = CONFIG_DEFAULT_MAINDIALOG_POS_X ;
	}

	if(m_ConfigData.DlgStartupPos.nY<0 || m_ConfigData.DlgStartupPos.nY>(::GetSystemMetrics(SM_CYSCREEN)))
	{
		m_ConfigData.DlgStartupPos.nY = CONFIG_DEFAULT_MAINDIALOG_POS_Y ;
	}

	const int nX = m_ConfigData.DlgStartupPos.nX + s_nStartupPostion*20  ;
	const int nY = m_ConfigData.DlgStartupPos.nY + s_nStartupPostion*20  ;
	::SetWindowPos(this->GetSafeHwnd(),HWND_TOPMOST,nX,nY,0,0,SWP_NOSIZE|SWP_NOZORDER) ;
	++s_nStartupPostion ;
#endif
}


void CMainDialog::OnFailedFile_Update(const SFailedFileInfo& OldFfi,const pt_STL_vector(SFailedFileInfo)& FailedFileVer) 
{
#ifdef VERSION_PROFESSIONAL
	_ASSERT(OldFfi.nIndex>=0) ;
	_ASSERT(OldFfi.nIndex<(int)FailedFileVer.size()) ;

	m_FailFileListView.UpdateFailFileStatusByID(FailedFileVer[OldFfi.nIndex]) ;
#endif
}

void CMainDialog::OnFailedFile_Added(const SFailedFileInfo& ffi)
{
#ifdef VERSION_PROFESSIONAL
	if(m_pFailedFile->GetFailedFileCount()==1 && !m_bMoreOrLess)
	{// 如果是第一个失败的文件，则展开窗口
		
		::ShowWindow(m_MoreLessSkinButton.GetSaftHwnd(),SW_SHOW) ;

		this->PerformButton(IDC_BUTTON_MORELESS) ;
	}

	m_FailFileListView.AddFailFile(ffi) ;
#endif
}

BOOL CMainDialog::OnInitDialog() 
{
	m_NormalWndUIState.SetParameter(&m_TrayUIState,this) ;
	m_TrayUIState.SetParameter(&m_NormalWndUIState,this) ;

	m_pCurUIState = &m_NormalWndUIState ;

	CptString strVersion = ::MakeXCVersionString() ;

	if(strVersion.GetLength()>0)
	{
		::SetWindowText(this->GetSafeHwnd(),strVersion.c_str()) ;
	}
	
	SptRect pt_rt ;
	SptPoint pt_pt ;

	// 进度条部分
	// 当前文件进度条
	::GetWindowRect(this->GetDlgItem(IDC_PROGRESS_CURRENT),(RECT*)&pt_rt) ;
	pt_pt = pt_rt.GetLocation() ;
	::ScreenToClient(this->GetSafeHwnd(),(POINT*)&pt_pt) ;
	pt_rt.SetLocation(pt_pt) ;
	
	::ShowWindow(this->GetDlgItem(IDC_PROGRESS_CURRENT),SW_HIDE) ;
	m_CurSkinProgressBar.SetRange(0,100) ;
	m_CurSkinProgressBar.SetParent(this->GetSafeHwnd()) ;
	m_CurSkinProgressBar.SetRectangle(pt_rt) ;

	// 总文件进度条
	::GetWindowRect(this->GetDlgItem(IDC_PROGRESS_TOTAL),(RECT*)&pt_rt) ;
	pt_pt = pt_rt.GetLocation() ;
	::ScreenToClient(this->GetSafeHwnd(),(POINT*)&pt_pt) ;
	pt_rt.SetLocation(pt_pt) ;
	
	::ShowWindow(this->GetDlgItem(IDC_PROGRESS_TOTAL),SW_HIDE) ;
	m_TotalSkinProgressBar.SetRange(0,100) ;
	m_TotalSkinProgressBar.SetParent(this->GetSafeHwnd()) ;
	m_TotalSkinProgressBar.SetRectangle(pt_rt) ;

	// 按钮部分
	m_RunSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_RUNANDPAUSE)) ;

	::EnableWindow(this->GetDlgItem(IDC_BUTTON_SKIP),FALSE) ;

	m_ExitSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_EXIT)) ;


#ifdef VERSION_PROFESSIONAL
	m_UpArrowBitmapInfo.hNormal = CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_UPARROW) ;
	m_UpArrowBitmapInfo.hDown = CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_UPARROWDOWN) ;
	m_UpArrowBitmapInfo.hHover = CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_UPARROWHOVER) ;

	m_DownArrowBitmapInfo.hNormal = CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_DOWNARROW) ;
	m_DownArrowBitmapInfo.hDown = CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_DOWNARROWDOWN) ;
	m_DownArrowBitmapInfo.hHover = CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_DOWNARROWHOVER) ;

	m_MoreLessSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_MORELESS)) ;
	m_MoreLessSkinButton.SetBkBitmap(m_DownArrowBitmapInfo) ;
	m_RetryAllFailedSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_RETRYALLFAILED)) ;

	m_VerifyDataSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_VERIFYDATA)) ;
	m_bMoreOrLess = false ; // less

	::ShowWindow(this->GetDlgItem(IDC_STATIC_GETPRO),SW_HIDE) ;
#else
	
	::DestroyWindow(this->GetDlgItem(IDC_BUTTON_MORELESS)) ;
	
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
		hls.szFontName = _T("Arial");			// Font face name

		m_Hyperlink.Create(&hls) ;
	}
#endif
	
	TCHAR szBuf[512] = {0} ;
	CptString strTxt ;

	m_SpeedSkinButton.Attach(this->GetDlgItem(IDC_BUTTON_SPEED)) ;
	
	m_SpeedSkinButton.SetBkBitmap(CXCConfiguration::GetInstance()->GetGlobalData()->hSpeedBitmapNormal,
		CXCConfiguration::GetInstance()->GetGlobalData()->hSpeedBitmapHover,
		CXCConfiguration::GetInstance()->GetGlobalData()->hSpeedBitmapHover,CXCConfiguration::GetInstance()->GetGlobalData()->hSpeedBitmapNormal) ;

	// 速度调整的 TOOLTIP
	strTxt = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TOOLTIP_CONTROLCOPYSPEED) ;

	// 设置图标
	HICON hIcon = CptMultipleLanguage::GetInstance()->GetIcon(IDI_SMALL) ;
	this->SetIcon(hIcon) ;

	m_pSpeedDlg = NULL ;

	bool bMove = (m_pTransApp->GetTaskInfo()->CopyType==SXCCopyTaskInfo::RT_Move)  ;

	SRunTimeSettingDlgOption RuntimeOption ;
	RuntimeOption.bIsCloseWindow = m_ConfigData.bCloseWindow ;
	RuntimeOption.bIsShutdown = m_ConfigData.bShutDown ;
	RuntimeOption.bIsVerify = m_ConfigData.bVerifyData ;

	m_pSpeedDlg = new CSpeedDialog(bMove,RuntimeOption,this,this->GetSafeHwnd()) ;
	m_pSpeedDlg->Create() ;
	
	// 计算出 运行 按钮底部再加上标题栏位置，以这个位置作为界面的分界线
	// 这样就能准备把界面折叠起来
	SptRect RunButtonRect ;

	::GetWindowRect(this->GetSafeHwnd(),m_OrginRect.GetRECTPointer()) ;

	HWND hRunButton = ::GetDlgItem(this->GetSafeHwnd(),IDC_BUTTON_MORELESS) ;
	::GetWindowRect(hRunButton,RunButtonRect.GetRECTPointer()) ;
	SptPoint ptBottom = SptPoint(RunButtonRect.nRight,RunButtonRect.nBottom) ;
	::ScreenToClient(this->GetSafeHwnd(),ptBottom.GetPOINTPointer()) ;

	m_nMoreHeightOffset = m_OrginRect.GetHeight() - (ptBottom.nY + ::GetSystemMetrics(SM_CYCAPTION) + RunButtonRect.GetHeight());

	::SetWindowPos(this->GetSafeHwnd(),HWND_TOP,0,0,m_OrginRect.GetWidth(),m_OrginRect.GetHeight()-m_nMoreHeightOffset,SWP_NOZORDER|SWP_NOMOVE) ;
	m_bMoreOrLess = false ; // less

	::SetWindowPos(this->GetSafeHwnd(),HWND_TOP,0,0,m_OrginRect.GetWidth(),m_OrginRect.GetHeight()-m_nMoreHeightOffset,SWP_NOZORDER|SWP_NOMOVE) ;

	m_nDelayShowUICounter = 0 ;

#ifdef _DEBUG
	const int nInterval = 1 ;
#else
	// 延迟 0.015秒显示。为的是在复制小量文件时不用老是闪一下程序界面就消失
	const int nInterval = 15 ; 
#endif

	if(m_ConfigData.bTopMost)
	{
		::SetWindowPos(this->GetSafeHwnd(),HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE) ;
	}

	this->SetStartupPosition() ;

	this->SetTimer(TIMER_ID_DELAYSHOWWINDOW,nInterval) ; // 延迟显示

#ifdef VERSION_PROFESSIONAL
	m_FailFileListView.Attach(this->GetDlgItem(IDC_LIST_CHANGE)) ;
#endif

	if(m_bAutoRun)
	{// 若界面启动后，自动运行
		this->PerformButton(IDC_BUTTON_RUNANDPAUSE) ;
	}

	return CptDialog::OnInitDialog() ;
}

void CMainDialog::OnPaint() 
{
	PAINTSTRUCT ps ;

	HDC hDC = ::BeginPaint(this->GetSafeHwnd(),&ps) ;

	m_CurSkinProgressBar.Draw(hDC) ;
	m_TotalSkinProgressBar.Draw(hDC) ;

	::EndPaint(this->GetSafeHwnd(),&ps) ;

#ifdef VERSION_PROFESSIONAL
	m_MoreLessSkinButton.Update() ;
#else
	m_Hyperlink.Update() ;
#endif
}


void CMainDialog::OnButtonClick(int nButtonID) 
{
	switch(nButtonID)
	{
	case IDC_BUTTON_RETRYALLFAILED: // 重试全部的失败文件
		m_FailFileListView.RetryAllFailedFile() ;
		break ;

	case IDC_BUTTON_SKIP: // 跳过当前文件
		{
			ECopyFileState cfs = m_pTransApp->GetXCState() ;

			_ASSERT(cfs==CFS_Running || cfs==CFS_Pause) ;

			// 只有在 运行 或 暂停 情况下该跳过才有效
			PT_BREAK_IF(!(cfs==CFS_Running || cfs==CFS_Pause)) ;

			bool bNeedContinue = (cfs==CFS_Pause) ;

			// 如果当前是 运行 状态，则先进入 暂停 状态下
			if(cfs==CFS_Running)
			{
				bNeedContinue = true ;
				m_pTransApp->Pause() ;
			}

			CptString strSkipFileName = m_pTransApp->GetCurWillSkipFile() ;

			if(strSkipFileName.GetLength()>0)
			{
				CptString strText ;
				CptString strTitle = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;

				strText.Format(_T("%s\r\n%s"),strSkipFileName,CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_AREYOUUSRESKIP) ) ;

				if(CptMessageBox::ShowMessage(this->GetSafeHwnd(),strText,strTitle,CptMessageBox::Button_YesNo)==CptMessageBox::MsgResult_Yes)
				{
					bool b = m_pTransApp->SkipCurrentFile() ;

					_ASSERT(b) ;
				}
			}

			if(bNeedContinue)
			{
				m_pTransApp->Continue() ;
				m_RunSkinButton.Update() ;
			}
			
		}
		
		break ;

#ifdef VERSION_PROFESSIONAL
	case IDC_BUTTON_VERIFYDATA: // verify data
		break ;

	case IDC_BUTTON_MORELESS: // more less 按钮
		if(m_bMoreOrLess)
		{// 转到 less
			m_MoreLessSkinButton.SetBkBitmap(m_DownArrowBitmapInfo) ;
			SptSize size(m_OrginRect.GetWidth(),m_OrginRect.GetHeight()-m_nMoreHeightOffset) ;
			this->SetWindowSize(size) ;
		}
		else
		{// 转到 more
			m_MoreLessSkinButton.SetBkBitmap(m_UpArrowBitmapInfo) ;
			this->SetWindowSize(m_OrginRect.GetSize()) ;
		}

		m_bMoreOrLess = !m_bMoreOrLess ;
		break ;
#endif

	case IDC_BUTTON_RUNANDPAUSE: // 运行按钮
		{
			if(m_pTransApp!=NULL)
			{
				switch(m_pTransApp->GetXCState())
				{
				case CFS_Stop:

				case CFS_Pause:
					m_pTransApp->Continue() ;
					break ;

				case CFS_Running:
					m_pTransApp->Pause() ;
					break ;
				}
			}
		}
		break ;

	case IDC_BUTTON_EXIT: // 退出
		this->PerformButton(IDCANCEL) ;
		
		break ;

	case IDC_BUTTON_HELP: // 帮助
		{
			::LaunchHelpFile(HFP_MainDlg) ;
		}
		break ;

	//case IDC_BUTTON_TITLEMIN: // 最小化
	//	//Release_Printf(_T("CMainDialog::OnButtonClick() IDC_BUTTON_TITLEMIN")) ;
	//	this->PostMessage(WM_SYSCOMMAND,SC_MINIMIZE,0) ;
	//	break ;

	}
}


void CMainDialog::OnControlStatusChanged(int nControlID,SControlStatus NewStatus) 
{
	if(nControlID==IDC_BUTTON_SPEED)
	{
		switch(NewStatus.status.MouseStatus)
		{
		case ControlMouseStatus_Leave:
		case ControlMouseStatus_Normal:
			{
				if(m_pSpeedDlg!=NULL && m_pSpeedDlg->IsShow())
				{
					m_pSpeedDlg->DelayHide() ;
					::SetWindowPos(m_pSpeedDlg->GetSafeHwnd(),m_ConfigData.bTopMost?HWND_TOPMOST:HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOMOVE) ;
					m_pSpeedDlg->UpdatePosition() ;
				}
			}
			break ;

		case ControlMouseStatus_Hover:
			{
				if(m_pSpeedDlg!=NULL && !m_pSpeedDlg->IsShow())
				{
					m_pSpeedDlg->ShowAnimate() ;
					::SetWindowPos(m_pSpeedDlg->GetSafeHwnd(),m_ConfigData.bTopMost?HWND_TOPMOST:HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOMOVE) ;
					m_pSpeedDlg->UpdatePosition() ;
				}
			}

			break ;
		}
	}
}

void CMainDialog::OnDestroy()
{
	if(m_pTransApp!=NULL)
	{
		m_pTransApp->Release() ;
	}
}

bool CMainDialog::OnCancel()
{
	bool bResult = true ;

	if(m_pTransApp->GetXCState()!=CFS_Stop || m_pTransApp->GetXCStatus()==XCST_Verifying)
	{
		bool bPauseByCancel = false ;

		if(m_pTransApp->GetXCState()==CFS_Running)
		{
			this->PerformButton(IDC_BUTTON_RUNANDPAUSE) ;
			bPauseByCancel = true ;
			::InvalidateRect(this->GetSafeHwnd(),NULL,TRUE) ;
		}

		bResult = ::ConfirmExitApp(this->GetSafeHwnd()) ; // 询问是否确实要退出

		if(bResult)
		{// 确认退出
			m_bExit = true ;

			this->Hide() ;// 先把界面隐藏，然后再慢慢退出
			m_pTransApp->Stop() ;
		}
		else
		{
			if(bPauseByCancel)
			{
				this->PerformButton(IDC_BUTTON_RUNANDPAUSE) ;
				::InvalidateRect(this->GetSafeHwnd(),NULL,TRUE) ;
			}
		}
	}

	if(bResult)
	{
		m_bExit = true ;

		if(m_pSpeedDlg!=NULL && m_pSpeedDlg->IsShow())
		{
			m_pSpeedDlg->Hide() ;
		}

		::AnimateWindow(this->GetSafeHwnd(),50,AW_BLEND|AW_HIDE) ;
	}

	return bResult ;
}



void CMainDialog::XCUI_SetTimer(int nTimerID,int nInterval) 
{
	this->SetTimer(nTimerID,nInterval) ;
}

void CMainDialog::XCUI_KillTimer(int nTimerID) 
{
	this->KillTimer(nTimerID) ;
}

bool CMainDialog::IsSimpleUI() const 
{
	return m_bSimpleUI ;
}

void CMainDialog::XCUI_OnIni(CXCTransApp* pTransApp,CXCFailedFile* pFailedFile,const SXCUIDisplayConfig& config,const SXCCopyTaskInfo& TaskInfo) 
{// 注意：当这个函数被调用时，CMainDialog还未被创建的。
	_ASSERT(pTransApp!=NULL) ;
	_ASSERT(pFailedFile!=NULL) ;

	m_pTransApp = pTransApp ;
	m_ConfigData = config ;

	m_ConfigData.bVerifyData = TaskInfo.ConfigShare.bVerifyData ; // 任务所指定的 verification 优先

	m_pFailedFile = pFailedFile ;
	m_pFailedFile->AddObserver(this) ;
	m_FailFileListView.SetUIEventCB(m_pFailedFile) ;

	m_bSimpleUI = (m_DialogTemplateID==IDD_DIALOG_MAIN_SIMPLY) ;

	if(!m_bSimpleUI) 
	{
		this->SetDlgItemText(IDC_STATIC_DESTINATIONFILE,TaskInfo.strDstFolderVer[0].c_str()) ;
	}
}

void CMainDialog::XCUI_OnRecuriseFolder(const CptString& strSrcFolder, const CptString& strDstFolder)
{
	if(m_bExit)
	{
		return ;
	}

	CptString strText ;

	CptString strTitle = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_ERROR) ;
	CptString strFormat = CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_RECURSIVEFOLDERERROR) ;
	strText.Format(strFormat.c_str(),strSrcFolder,strDstFolder) ;

	CptMessageBox::ShowMessage(this->GetSafeHwnd(),strText,strTitle,CptMessageBox::Button_OK) ;
}

void CMainDialog::XCUI_OnRunningStateChanged(ECopyFileState NewState) 
{
	if(m_bExit)
	{
		return ;
	}

	switch(NewState)
	{
	case CFS_Stop :
		{
			::EnableWindow(this->GetDlgItem(IDC_BUTTON_RUNANDPAUSE),FALSE) ;
			::EnableWindow(this->GetDlgItem(IDC_BUTTON_SKIP),FALSE) ;
		}
		break ;

	case CFS_Running :
		{
			::EnableWindow(this->GetDlgItem(IDC_BUTTON_RUNANDPAUSE),TRUE) ;

			::EnableWindow(this->GetDlgItem(IDC_BUTTON_SKIP),TRUE) ;
			this->SetDlgItemText(IDC_BUTTON_RUNANDPAUSE,CptMultipleLanguage::GetInstance()->GetString(IDS_BUTTON_PAUSE)) ;
		}
		break ;

	case CFS_Pause :
		{
			this->SetDlgItemText(IDC_STATIC_SPEED,_T("0")) ;
			this->SetDlgItemText(IDC_BUTTON_RUNANDPAUSE,CptMultipleLanguage::GetInstance()->GetString(IDS_BUTTON_CONTINUE)) ;
		}
		break ;

	case CFS_Exit :
		{
			this->PerformButton(IDCANCEL) ;
		}
		break ;
	}
}

void CMainDialog::XCUI_OnUIVisibleChanged(bool bVisible) 
{
	if(m_bExit)
	{
		return ;
	}

	if(bVisible)
	{
		if(this->GetSafeHwnd()==NULL)
		{
			this->ShowDialog(false) ;
		}
		else
		{
			m_bIniShow = true ;
			::ShowWindow(this->GetSafeHwnd(),SW_SHOW) ;
		}
	}
	else
	{
		_ASSERT(this->GetSafeHwnd()!=NULL) ;

		m_bIniShow = false ;
		::ShowWindow(this->GetSafeHwnd(),SW_HIDE) ;
	}
}


void CMainDialog::OnRunTimeSettingChanged(const SRunTimeSettingDlgOption& option)
{
	// 速度改变
	if(option.uFlag & SRunTimeSettingDlgOption::RunTimeSettingOptionFlag_Speed)
	{
		m_pTransApp->SetSpeed(option.nSpeed) ;
	}

	// verify 数据
	if(option.uFlag & SRunTimeSettingDlgOption::RunTimeSettingOptionFlag_Verify)
	{
		m_pTransApp->SetVerifyData(option.bIsVerify) ;
	}

	// 关闭窗口
	if(option.uFlag & SRunTimeSettingDlgOption::RunTimeSettingOptionFlag_CloseWindow)
	{
		m_ConfigData.bCloseWindow = option.bIsCloseWindow ;
		m_pTransApp->SetCloseWindow(option.bIsCloseWindow) ;
	}

	// 关机
	if(option.uFlag & SRunTimeSettingDlgOption::RunTimeSettingOptionFlag_Shutdown)
	{
		CXCQueueLegacyOption::SetOption(option.bIsShutdown ? QLO_MASK_ShutdownAfterTaskDone : (~QLO_MASK_ShutdownAfterTaskDone));

		m_pTransApp->SetShutdown(option.bIsShutdown) ;
	}
}


CptString CMainDialog::FormatErrorString(const SXCErrorCode& ec)
{
	CptString strRet ;

	if((ec.AppError==CopyFileErrorCode_Success || ec.AppError==CopyFileErrorCode_Unknown) && ec.nSystemError!=0)
	{
		if(ec.nSystemError>0)
		{
			TCHAR* lpBuf = NULL ;

			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,NULL,ec.nSystemError,0,(LPTSTR)&lpBuf,0,NULL) ;

			if(lpBuf!=NULL)
			{
				strRet = lpBuf ;

				::LocalFree(lpBuf) ;
			}
		}
		else
		{
			strRet = CptMultipleLanguage::GetInstance()->GetString(IDS_ERROR_APPERROR) ;
		}
	}
	else
	{
		switch(ec.AppError)
		{
		case CopyFileErrorCode_Success:// 
			strRet = CptMultipleLanguage::GetInstance()->GetString(IDS_ERROR_SUCCESS) ;
			break ;

		case CopyFileErrorCode_AppError:
			strRet = CptMultipleLanguage::GetInstance()->GetString(IDS_ERROR_APPERROR) ;
			break ;

		case CopyFileErrorCode_InvaliableFileName:
			strRet = CptMultipleLanguage::GetInstance()->GetString(IDS_ERROR_INVALIDFILENAME) ;
			break ;

		default:
		case CopyFileErrorCode_Unknown:
			strRet = CptMultipleLanguage::GetInstance()->GetString(IDS_ERROR_UNKNOWN) ;
			break ;
		}
	}

	return strRet ;
}

CptString CMainDialog::GetAppErrorString(unsigned uAppErrorCode) 
{
	CptString strTxt ;

	switch(uAppErrorCode)
	{
	case CopyFileErrorCode_CanotReadFileData: 
		strTxt = ::CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_CANOTREADFILEDATA) ;
		break ;

	case CopyFileErrorCode_InvaliableFileName:
		strTxt = ::CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_INVALIDFILENAME) ;
		break ;

	case CopyFileErrorCode_WriteFileFailed:
		strTxt = ::CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_WRITEFILEFAILED) ;
		break ;

	case CopyFileErrorCode_AppError:
	default:
		strTxt = ::CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_ERROREXITAPP) ; 
	}

	return strTxt ;
}

ErrorHandlingResult CMainDialog::XCUI_OnError(const SXCExceptionInfo& ErrorInfo) 
{
	if(m_bExit)
	{
		return ErrorHandlingFlag_Exit;
	}

	// 如果在托盘模式的，则先把其转成窗口模式，否则在恢复窗口模式时出错的对话框会让界面出现挂死的情况
	if(m_pCurUIState==&m_TrayUIState)
	{
		// 模拟一个双击事件
		m_TrayUIState.ProcessMessage(this->GetSafeHwnd(),WM_XC_SHELLTRAY,0,WM_LBUTTONDBLCLK) ;
	}

	if(ErrorInfo.SupportType ==ErrorHandlingFlag_Exit)
	{// 程序出错只能退出的话, 则显示出错信息，
		CptString strTxt ;

		if(ErrorInfo.ErrorCode.nSystemError==0)
		{
			strTxt = CMainDialog::GetAppErrorString(ErrorInfo.ErrorCode.AppError) ;
		}
		else
		{
			TCHAR* lpBuf = NULL ;

			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,NULL,ErrorInfo.ErrorCode.nSystemError,0,(LPTSTR)&lpBuf,0,NULL) ;

			if(lpBuf!=NULL)
			{
				strTxt = lpBuf ;

				::LocalFree(lpBuf) ;
			}
		}
		
		CptString strTitle = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_ERROR) ;

		CptMessageBox::ShowMessage(this->GetSafeHwnd(),strTxt,strTitle,CptMessageBox::Button_OK) ;

		return ErrorHandlingFlag_Exit ;
	}

	CptString strTxt ;

	if(ErrorInfo.strSrcFile.GetLength()>0)
	{
		CptString str ;

		str.Format(_T("%s : %s \r\n"),
			::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_SOURCEFILE),
			CptGlobal::MakeUnlimitFileName(ErrorInfo.strSrcFile,false)) ;

		strTxt += str ;
	}

	if(ErrorInfo.strDstFile.GetLength()>0)
	{
		CptString str ;

		str.Format(_T("%s : %s \r\n"),
			::CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_DESTINATIONFILE),
			CptGlobal::MakeUnlimitFileName(ErrorInfo.strDstFile,false)) ;

		strTxt += str ;
	}

	CptString strError = this->FormatErrorString(ErrorInfo.ErrorCode) ;

	strTxt += strError ;

	if(m_pFailedFile->GetFailedFileCount() >= this->m_ConfigData.nMaxIgnoreFailedFiles)
	{
		CptString str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_OVERMAXFAILEDFILESNUMBER) ;

		strTxt.Format(_T("%s %s"),str,strError) ;
	}

	int nRet = ErrorHandlingFlag_Exit ;

	CptString strTitle = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_ERROR) ;

	CptMessageBox::EButton button  = CptMessageBox::Button_OK;

	switch(ErrorInfo.SupportType)
	{
	case ErrorHandlingFlag_RetryIgnoreCancel:
		button = CptMessageBox::Button_RetryIgnoreCancel ;
		break ;

	case ErrorHandlingFlag_Ignore|ErrorHandlingFlag_Exit:
		button = CptMessageBox::Button_IgnoreCancel ;
		break ;

	case ErrorHandlingFlag_Retry|ErrorHandlingFlag_Exit:
		button = CptMessageBox::Button_RetryCancel ;
		break ;
	}

	switch(CptMessageBox::ShowMessage(this->GetSafeHwnd(),strTxt.c_str(),strTitle.c_str(),button))
	{
	case CptMessageBox::MsgResult_Ignore:
		nRet = ErrorHandlingFlag_Ignore;
		break ;

	case CptMessageBox::MsgResult_Retry:
		nRet = ErrorHandlingFlag_Retry;
		break ;

	default:
	case CptMessageBox::MsgResult_Cancel:
		nRet = ErrorHandlingFlag_Exit;
		break ;
	}

	return nRet ;
}

CptString CMainDialog::XCUI_GetCopyOfText() const
{
	CptString strRet = CptMultipleLanguage::GetInstance()->GetString(IDS_FORMAT_COPYOF) ;

	if(strRet.GetLength()==0)
	{
		strRet = CXCUIWnd::XCUI_GetCopyOfText() ;
	}

	return strRet ;
}

// 有冲突文件
// 
EImpactFileBehaviorResult CMainDialog::XCUI_OnImpactFile(const SImpactFileInfo& ImpactInfo,SImpactFileBehaviorSetting& Setting) 
{
	if(m_bExit)
	{
		return SFDB_StopCopy;
	}

	EImpactFileBehaviorResult ret = SFDB_Default ;

	switch(ImpactInfo.ImpactType)
	{
	case IT_SameFileName: // 相同文件名
		{
			CSameFileDialog dlg(ImpactInfo.strSrcFile,ImpactInfo.strDestFile,this->GetSafeHwnd()) ;

			dlg.ShowDialog() ;

			Setting = dlg.GetFileBehaviorSetting() ;

			ret = dlg.GetResult();
		}
		break ;

	case IT_ReadOnly:// 只读文件
		break ;

	case IT_ExecutableFile: // 可执行文件
		break ;
	}

	return ret ;
}


//////=================================================================================

bool CMainDialog::GetSateValueDisplayString(SSateDisplayStrings& sds)
{
	const CXCStatistics* pStatistics = m_pTransApp->GetStatistics() ;

	_ASSERT(pStatistics!=NULL) ;

	if(pStatistics==NULL)
	{
		return false;
	}

	const SStatisticalValue& StaValue = pStatistics->GetStaData() ;
	//const SStatisticalValue& StaValue = m_XCSta.GetStaData() ;

	if(SSateDisplayStrings::nLapseTimeMask & sds.uFlag)
	{// 用去的时间
		sds.strLapseTime.Format(_T("%02d:%02d:%02d"),StaValue.nLapseTime/(60*60),(StaValue.nLapseTime%(60*60))/60,StaValue.nLapseTime%60) ;
	}
	
	if(SSateDisplayStrings::nSppedMask & sds.uFlag && m_pTransApp!=NULL)
	{
		if(StaValue.uTransSize>0)
		{// 计算复制速度
			const float f = StaValue.fSpeed ;

			if(f>=1024*1024.f)
			{
				if(f>1024*1024*1024)
				{// G
					const float f2 = f/(1024*1024*1024) ;
					sds.strSpeed.Format(_T("%.2f G/s"),f2) ;
				}
				else
				{// M
					const float f2 = f/(1024*1024) ;
					sds.strSpeed.Format(_T("%.1f M/s"),f2) ;
				}
			}
			else
			{
				const float f2 = f/1024 ;
				sds.strSpeed.Format(_T("%.1f K/s"),f2) ;
			}
		}
		else
		{
			sds.strSpeed = _T("-") ;
		}
	}

	const bool bTranSizeWorks = (StaValue.uTransSize>0 && StaValue.uTotalSize>=StaValue.uTransSize) ;

	if(SSateDisplayStrings::nRemainSizeMask & sds.uFlag)
	{// 剩余大小
		if(bTranSizeWorks )
		{// 剩余文件大小
			sds.strRemainSize = ::GetSizeString(StaValue.uTotalSize-StaValue.uTransSize) ;
		}
		else
		{
			sds.strRemainSize = _T("-") ;
		}
	}

	if(SSateDisplayStrings::nTotalSizeMask & sds.uFlag)
	{// 总的文件大小
		if(bTranSizeWorks)
		{// 总的文件大小
			sds.strTotalSize = ::GetSizeString(StaValue.uTotalSize) ;
		}
		else
		{
			sds.strTotalSize = _T("-") ;
		}
	}

	if(SSateDisplayStrings::nTotalFilesMask & sds.uFlag)
	{// 总的文件数目
		if(StaValue.nTotalFiles>0)
		{
			sds.strTotalFiles = CptGlobal::FormatCommaNumber(StaValue.nTotalFiles) ;
		}
		else
		{
			sds.strTotalFiles = _T("-") ;
		}
	}

	if(SSateDisplayStrings::nRemainFileMask & sds.uFlag)
	{// 剩余文件数目
		if(StaValue.nTotalFiles>0)
		{
			sds.strRemainFiles = CptGlobal::FormatCommaNumber(StaValue.nTotalFiles-StaValue.nDoneFiles-StaValue.nSkipFiles) ;
		}
		else
		{
			sds.strRemainFiles = _T("-") ;
		}
	}

	if(SSateDisplayStrings::nDoneFilesMask & sds.uFlag)
	{// 完成的文件数目
		sds.strDoneFiles = CptGlobal::FormatCommaNumber(StaValue.nDoneFiles) ;
	}

	if(SSateDisplayStrings::nRemainTimeMask & sds.uFlag)
	{// 剩余时间
		if(bTranSizeWorks)
		{
			int nRemainTime = (int)(StaValue.uTotalSize>StaValue.uTransSize ? (StaValue.nLapseTime/(float)StaValue.uTransSize)*(StaValue.uTotalSize-StaValue.uTransSize) : 0) ;

			sds.strRemainTime.Format(_T("%02d:%02d:%02d"),nRemainTime/(60*60),(nRemainTime%(60*60))/60,nRemainTime%60) ;
		}
		else
		{
			sds.strRemainTime = _T("-") ;
		}
	}

	return true ;
}


void CMainDialog::ShowIniWindows() 
{
	if(m_pTransApp==NULL)
	{
		return;
	}

	if(m_ConfigData.ShowMode==SXCCopyTaskInfo::ST_Window)
	{
		m_bIniShow = true ;
		::ShowWindow(this->GetSafeHwnd(),SW_SHOW) ;
	}
	else
	{
		this->PostMessage(WM_SYSCOMMAND,SC_MINIMIZE,0) ; // 最小化
	}
}

// 时间到
void CMainDialog::OnTimer(int nTimerID)
{
	if(m_pTransApp!=NULL)
	{
		m_pTransApp->OnUITimer(nTimerID) ;
	}

	if(nTimerID==TIMER_ID_DELAYSHOWWINDOW)
	{
		if((m_nDelayShowUICounter+=15)>700)
		{
			this->KillTimer(TIMER_ID_DELAYSHOWWINDOW) ;
			this->ShowIniWindows() ;
		}
		else
		{
			const CXCStatistics* pStatistics = m_pTransApp->GetStatistics() ;

			_ASSERT(NULL!=pStatistics) ;

			if(pStatistics->IsShowDelayUI())
			{
				this->KillTimer(TIMER_ID_DELAYSHOWWINDOW) ;
				this->ShowIniWindows() ;
			}
		}
	}

}

void CMainDialog::XCUI_OnVerificationResult(EXCVerificationResult result) 
{
	if(m_bExit)
	{
		return ;
	}

	LONG nStrID = IDS_STRING_VERIFICATION_SAME ;
	switch(result)
	{
	case XCST_Same:
		nStrID = IDS_STRING_VERIFICATION_SAME ;
		break ;

	default:
	case XCST_Different:
		nStrID = IDS_STRING_VERIFICATION_DIFF ;
		break ;
	}

	CptString str = ::CptMultipleLanguage::GetInstance()->GetString(nStrID) ;

	this->SetDlgItemText(IDC_STATIC_VERIFICATION,str.c_str()) ;
}


void CMainDialog::XCUI_OnStatusChanged(EXCStatusType OldStatus,EXCStatusType NewStatus)
{
	if(m_bExit)
	{
		return ;
	}

	if(OldStatus==XCST_Copying && NewStatus==XCST_Verifying)
	{// 转到 verify 状态
		m_bVerifyPhase = true ;

		if(!this->IsSimpleUI())
		{
			this->SetDlgItemText(IDC_STATIC_REMAINTIME,_T("-")) ; // 剩余时间
			this->SetDlgItemText(IDC_STATIC_REMAINFILES,_T("-")) ; // 剩余文件数量
			this->SetDlgItemText(IDC_STATIC_SPEED,_T("-")) ;// 速度
		}

		this->m_CurSkinProgressBar.SetValue(0) ;
		this->SetDlgItemText(IDC_STATIC_SOURCEFILE,_T("-")) ;
		this->SetDlgItemText(IDC_STATIC_DESTINATIONFILE,_T("-")) ;

		this->SetDlgItemText(IDC_STATIC_CURRENTFILE,_T("")) ;

		::EnableWindow(this->GetDlgItem(IDC_BUTTON_RUNANDPAUSE),FALSE) ;
		
		::EnableWindow(this->GetDlgItem(IDC_BUTTON_SKIP),FALSE) ;
	}
	
	if(NewStatus==XCST_Finished)
	{
		this->m_CurSkinProgressBar.SetValue(0) ;
		this->SetDlgItemText(IDC_STATIC_SOURCEFILE,_T("-")) ;
		this->SetDlgItemText(IDC_STATIC_DESTINATIONFILE,_T("-")) ;

		this->SetDlgItemText(IDC_STATIC_REMAINTIME,_T("-")) ; // 剩余时间
		this->SetDlgItemText(IDC_STATIC_REMAINFILES,_T("-")) ; // 剩余文件数量
		this->SetDlgItemText(IDC_STATIC_SPEED,_T("-")) ;// 速度

		this->SetDlgItemText(IDC_STATIC_CURRENTFILE,_T("")) ;
		this->m_TotalSkinProgressBar.SetValue(0) ;
		m_Win7TaskBarProgress.SetProgressValue(this->GetSafeHwnd(),0) ;
	}

	LONG nStrID = 0 ;
	switch(NewStatus)
	{
	default:
	case XCST_Unkown: nStrID = IDS_STRING_STATUS_UNKOWN ; break ;
		break ;
	case XCST_Ready: nStrID = IDS_STRING_STATUS_READY ; break ;
		break ;
	case XCST_Copying: 
		nStrID = (SXCCopyTaskInfo::RT_Move==m_pTransApp->GetTaskInfo()->CopyType) ? IDS_STRING_STATUS_MOVING : IDS_STRING_STATUS_COPYING ; break ;
		break ;
	case XCST_Pause: nStrID = IDS_STRING_STATUS_PAUSE ; break ;
		break ;
	case XCST_Verifying: nStrID = IDS_STRING_STATUS_VERFYING ; break ;
		break ;
	case XCST_Finished: nStrID = IDS_STRING_STATUS_FINISHED ; break ;
	}

	CptString str = ::CptMultipleLanguage::GetInstance()->GetString(nStrID) ;

	this->SetDlgItemText(IDC_STATIC_STATUS,str.c_str()) ;
}


void CMainDialog::XCUI_UpdateUI(EUpdateUIType uuit,void* pParam1,void* pParam2)
{// 因为界面更新也占用一定的时间，所以对一些占用时间敏感的操作通过消息队列来完成界面的更新

	if(m_bExit)
	{
		return ;
	}

	switch(uuit)
	{
	case UUIT_CopyDataOccured: // IO数据发生了操作 (for verification)
		{
			CptAutoLock lock(&m_UISpeedInfo.ThreadLock) ;

			m_UISpeedInfo.bReadOrWrite = *(bool*)pParam1 ;
			m_UISpeedInfo.nSize = *((UINT*)pParam2) ;

			if(!m_UISpeedInfo.bInMsgQueue)
			{// 若不在消息队列,则向队列发送消息
				m_UISpeedInfo.bInMsgQueue = true ;
				this->PostMessage(WM_XC_COPYDATAOCCURED,NULL,NULL) ;
			}
		}
		
		break ;

	case UUIT_BeginCopyOneFile: // 开始复制一文件 (for verification)
		if(pParam1!=NULL && pParam2!=NULL)
		{
			CptAutoLock lock(&m_UIFileNameInfo.ThreadLock) ;

			m_UIFileNameInfo.strSrcFileName = *(CptString*)pParam1 ;
			m_UIFileNameInfo.strDstFileName = *(CptString*)pParam2 ;

			if(!m_UIFileNameInfo.bInMsgQueue)
			{
				m_UIFileNameInfo.bInMsgQueue = true ;
				this->PostMessage(WM_XC_BEGINCOPYONEFILE,NULL,NULL) ;
			}

		}
		break ;

	default:
		if(this->m_pCurUIState!=NULL)
		{
			m_pCurUIState->UpdateUI(uuit,pParam1,pParam2) ;
		}
		break ;
	}

}

void CMainDialog::MinimumToTray() 
{
	HWND hWnd = this->GetSafeHwnd() ;
	HICON hIcon = (HICON)this->SendMessage(WM_GETICON,ICON_SMALL,NULL) ;

	m_ShellTray.Add(hWnd,WM_XC_SHELLTRAY,hIcon,NULL) ;
	::ShowWindow(hWnd,SW_HIDE) ;

	if(m_pCurUIState!=NULL)
	{
		CptString strCurFileName = m_pCurUIState->GetCurFileName();
		m_pCurUIState->OnLeave() ;
		m_pCurUIState = &m_TrayUIState ; // 切换到托盘状态
		m_pCurUIState->OnEnter(strCurFileName) ;
	}
}


void CMainDialog::OnSize(UINT uType,int nNewWidth,int nNewHeigh) 
{
	switch(uType)
	{
	case SIZE_MINIMIZED: // 最小化
		{
			if(this->m_ConfigData.bMinimumToTray || !m_bIniShow)
			{// 若最少化到托盘
				//m_bMinimumToTray = false ;
				this->MinimumToTray() ;
			}
		}
		break ;

	case SIZE_MAXSHOW: // 最大化
		break ;

	case SIZE_RESTORED: // 恢复
		{
			m_ShellTray.Remove() ;

			if(m_pCurUIState!=NULL)
			{
				CptString strCurFileName = m_pCurUIState->GetCurFileName();
				m_pCurUIState->OnLeave() ;
				m_pCurUIState = &m_NormalWndUIState ; // 切换到窗口状态
				m_pCurUIState->OnEnter(strCurFileName) ;
			}
		}
		break ;
	}

}

int CMainDialog::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_pCurUIState!=NULL)
	{
		m_pCurUIState->ProcessMessage(hWnd,uMsg,wParam,lParam) ;
	}

	if(uMsg==WM_TASKBARCREATED && m_pCurUIState== &m_TrayUIState)
	{// 任务栏重新生成之后
		this->MinimumToTray() ;
	}

	switch(uMsg)
	{
	case WM_SHOWWINDOW :
		::AnimateWindow(hWnd,100,AW_BLEND) ;
		::InvalidateRect(hWnd,NULL,TRUE) ;
		::UpdateWindow(hWnd) ;
		m_CurSkinProgressBar.Draw() ;
		m_TotalSkinProgressBar.Draw() ;
		return 0 ;

	case WM_WINDOWPOSCHANGING:
		if(!m_bIniShow)
		{
			WINDOWPOS* pWnd = (WINDOWPOS*)lParam ;

			_ASSERT(pWnd!=NULL) ;

			pWnd->flags &= ~SWP_SHOWWINDOW;
		}
		break ;

	case WM_KEYUP:
	case WM_KEYDOWN:
		{
			if(wParam==VK_ESCAPE)
			{// 拦截 ESCAPE 键退出
				return TRUE ;
			}
		}
		break ;

	case WM_QUERYENDSESSION: // 拦截关机消息
		{
			this->Show() ;
			CptString strTitle = ::CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;
			CptString strText = ::CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_QUITCONFIRM) ;

			if(CptMessageBox::ShowMessage(this->GetSafeHwnd(),strText,strTitle,CptMessageBox::Button_YesNo)
				==CptMessageBox::MsgResult_No)
			{
				return FALSE ;
			}
			else
			{
				m_pTransApp->Stop() ;
				this->Close() ;
			}
		}
		break ;

	case WM_XC_COPYDATAOCCURED: // 复制数据发生操作
		{
			bool bReadOrWrite = false ;
			unsigned int nSize = 0 ;

			{
				CptAutoLock lock(&m_UISpeedInfo.ThreadLock) ;

				m_UISpeedInfo.bInMsgQueue = false ;// 标识为出队列

				bReadOrWrite = m_UISpeedInfo.bReadOrWrite ;
				nSize = m_UISpeedInfo.nSize ;
			}

			m_pCurUIState->UpdateUI(UUIT_CopyDataOccured,&bReadOrWrite,&nSize) ;
		}
		break ;

	case WM_XC_BEGINCOPYONEFILE: // 开始复制1文件
		{
			CptString strSrcFileName ;
			CptString strDstFileName ;

			{
				CptAutoLock lock(&m_UIFileNameInfo.ThreadLock) ;

				m_UIFileNameInfo.bInMsgQueue = false ; // 标识为出队列

				strSrcFileName = m_UIFileNameInfo.strSrcFileName ;
				strDstFileName = m_UIFileNameInfo.strDstFileName ;
			}

			m_pCurUIState->UpdateUI(UUIT_BeginCopyOneFile,&strSrcFileName,&strDstFileName) ;
		}
		break ;

		/**/
	case WM_CTLCOLORBTN :
	case WM_CTLCOLOREDIT :
	//case WM_CTLCOLORDLG :
	case WM_CTLCOLORSTATIC :
	case WM_CTLCOLORSCROLLBAR :
	case WM_CTLCOLORLISTBOX :
	case WM_CTLCOLORMSGBOX:
		{
			HDC hdc = (HDC)wParam ;

			::SetBkMode(hdc,TRANSPARENT) ;
			
			if((HWND)lParam==this->GetDlgItem(IDC_STATIC_CURRENTFILE))
			{// 当前复制的文件
				::SelectObject(hdc,m_hCurFileFont) ;
			}
			else if((HWND)lParam==this->GetDlgItem(IDC_STATIC_TO) || (HWND)lParam==this->GetDlgItem(IDC_STATIC_FROM))
			{
				::SelectObject(hdc,m_hToTextFont) ;
			}
			
			return (int)::GetSysColorBrush(COLOR_BTNFACE) ;
		}
		break ;
		/**/

	case WM_MOVE:
		if(m_pSpeedDlg!=NULL && m_pSpeedDlg->IsShow())
		{
			m_pSpeedDlg->UpdatePosition() ;
		}
		break ;
	}

	return CptDialog::OnProcessMessage(hWnd,uMsg,wParam,lParam) ;
}
