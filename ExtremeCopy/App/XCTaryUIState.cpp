/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCTaryUIState.h"
#include "UI\MainDialog.h"
#include "Language\XCRes_ENU\resource.h"
#include "..\Common\ptWinPath.h"

CXCTaryUIState::CXCTaryUIState(void)
{
	m_strTotalProcess = _T("- %") ;
	m_strRemainTime = _T("-:-:-") ;
	m_strSpeed = _T("-") ;
}

CXCTaryUIState::~CXCTaryUIState(void)
{
}

void CXCTaryUIState::UpdateUI(const EUpdateUIType uuit,void* pParam1,void* pParam2)
{
	//if(m_ShellTray.IsMouseHover())
	{// 只有鼠标停留在TRAY才有界面刷新

		switch(uuit)
		{
		case UUIT_OneSecond:
			if(m_ShellTray.IsMouseHover())
			{
				SXCUIOneSecondUpdateDisplay* p = (SXCUIOneSecondUpdateDisplay*)pParam1 ;

				this->UpdateOneSecondUI(*p) ;
				//this->UpdateOneSecondUI() ;
			}
			
			break ;

		//case UUIT_BeginCopyOneFile:
		//	{
		//		if(m_ShellTray.IsMouseHover())
		//		{
		//			if(pParam1!=NULL && pParam2 !=NULL)
		//			{
		//				CptString* pstrSrc = (CptString*)pParam1 ;
		//				CptString* pstrDst = (CptString*)pParam2 ;

		//				this->UpdateBeginCopyOneFileUI(*pstrSrc,*pstrDst) ;
		//			}
		//		}

		//	}

		//	break ;

		//case UUIT_FinishCopyOneFile:
		//	if(m_ShellTray.IsMouseHover())
		//	{
		//		this->UpdateFinishCopyOneFileUI() ;
		//	}

		//	break ;

		//case UUIT_CopyDataOccured:
		//	{
		//		if(m_ShellTray.IsMouseHover())
		//		{
		//			if(pParam1!=NULL && pParam2 !=NULL)
		//			{
		//				bool bReadOrWrite = *(bool*)pParam1 ;
		//				UINT uSize = *((UINT*)pParam2) ;

		//				this->UpdateCopyDataOccuredUI(bReadOrWrite,uSize) ;
		//			}
		//		}
		//	}

		//	break ;
		}
	}
	
}

void CXCTaryUIState::UpdateOneSecondUI(SXCUIOneSecondUpdateDisplay& osud) 
{
	CMainDialog::SSateDisplayStrings sds ;

	sds.uFlag = CMainDialog::SSateDisplayStrings::nSppedMask 
		| CMainDialog::SSateDisplayStrings::nLapseTimeMask
		| CMainDialog::SSateDisplayStrings::nRemainTimeMask;

	m_pMainDlg->GetSateValueDisplayString(sds) ;

	m_strSpeed = sds.strSpeed ;
	m_strLapseTime = sds.strLapseTime ;
	m_strRemainTime = sds.strRemainTime ;

	if(osud.strCurSrcFileName.GetLength()>0)
	{
		CptWinPath::SPathElementInfo pei ;

		pei.uFlag = CptWinPath::PET_FileName ;

		if(CptWinPath::GetPathElement(osud.strCurSrcFileName.c_str(),pei))
		{
			m_strCurFileName = pei.strFileName ;
		}
	}

	this->SetFileProgressString(false) ;

	this->UpdateUIString() ;
}

void CXCTaryUIState::SetFileProgressString(bool bReadOrWrite) 
{
	if(!bReadOrWrite)
	{
		const CXCStatistics* pStatistics = m_pMainDlg->m_pTransApp->GetStatistics() ;

		_ASSERT(pStatistics!=NULL) ;

		if(pStatistics==NULL)
		{
			return ;
		}

		const SStatisticalValue& StaData = pStatistics->GetStaData() ;

		CptString strSize = ::GetSizeString(StaData.uTransSize) ;

		if(pStatistics->DoesFolderSizeDone() && StaData.uTotalSize>0)
		{// 当全部的源大小确定后,才计算总的进度
			// 总的文件复制进度
			const int nProcess = (int)((StaData.uTransSize*100)/StaData.uTotalSize) ;
			m_strTotalProcess.Format(_T("%d%%"),nProcess) ;
		}

		if(StaData.uCurWholeSize>0)
		{// 当前文件进度
			const int nProcess  = (int)((StaData.uCurOperateSize*100)/(StaData.uCurWholeSize)) ;
			m_strCurProcess.Format(_T("%d%%"),nProcess) ;

			//if(nProcess<10)
			//{
			//	m_strCurProcess = _T("  ") + m_strTotalProcess ;
			//}
			//else if(nProcess<100)
			//{
			//	m_strCurProcess = _T(" ") + m_strTotalProcess ;
			//}
			//m_pMainDlg->m_CurSkinProgressBar.SetValue((int)((m_pMainDlg->m_pUIEventCB->GetStaData().uCurOperateSize*100)/(m_pMainDlg->m_pUIEventCB->GetStaData().uCurWholeSize))) ;
		}

		//this->UpdateUIString() ;
	}
}

void CXCTaryUIState::UpdateUIString()
{
	CptString strUIContent ;

	strUIContent.Format(m_strUIFormatText.c_str(),
		m_strCurFileName,m_strSpeed,m_strCurProcess,m_strTotalProcess,m_strLapseTime,m_strRemainTime) ;

	m_ShellTray.SetInfo(strUIContent.c_str(),m_strTitle.c_str()) ;
}

void CXCTaryUIState::OnEnter(CptString strCurFileName) 
{
	_ASSERT(m_pMainDlg!=NULL) ;

	m_strCurFileName = strCurFileName ;

	HWND hWnd = m_pMainDlg->GetSafeHwnd() ;
	//HICON hIcon = CptMultipleLanguage::GetInstance()->GetIcon(IDI_SMALL) ;
	//HICON hIcon = (HICON)::GetClassLong(hWnd,GCL_HICON) ;
	HICON hIcon = (HICON)m_pMainDlg->SendMessage(WM_GETICON,ICON_SMALL,NULL) ;

	m_ShellTray.Add(hWnd,WM_XC_SHELLTRAY,hIcon,NULL) ;

	if(m_strTitle.GetLength()==0)
	{
#ifdef VERSION_PROFESSIONAL 
		m_strTitle = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_APP_PRO) ;
#else
		m_strTitle = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_APP) ;
#endif

		m_strTitle += _T("                  ") ;
	}

	if(m_strUIFormatText.GetLength()==0)
	{
		m_strUIFormatText = CptMultipleLanguage::GetInstance()->GetString(IDS_FORMAT_TRAYUITEXT) ;
	}
}

void CXCTaryUIState::OnLeave() 
{
	m_ShellTray.Remove() ;
}

void CXCTaryUIState::ProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)  
{
	switch(uMsg)
	{
		case WM_XC_SHELLTRAY: // 托盘消息
		{
			m_ShellTray.ProcessMessage(wParam,lParam) ;

			int nEvent = LOWORD(lParam) ;

			switch(nEvent)
			{
			case WM_LBUTTONUP:
			case WM_RBUTTONUP: // 右击
			case WM_LBUTTONDBLCLK:// 双击

				this->m_pMainDlg->m_bIniShow = true ;
				{
					CptAutoLock lock(&m_pMainDlg->m_UISpeedInfo.ThreadLock) ;
					::ShowWindow(hWnd,SW_RESTORE) ;
				}
				
				break ;

			//case WM_MOUSEMOVE:
			//	
			//	break ;

			//case WM_MOUSEMOVE: // 移动
			//	{
			//		TRACKMOUSEEVENT    tme;
			//		tme.cbSize =sizeof(TRACKMOUSEEVENT);
			//		tme.dwFlags =TME_LEAVE|TME_HOVER;
			//		tme.dwHoverTime=0;
			//		tme.hwndTrack =hWnd;

			//		::TrackMouseEvent(&tme);
			//	}
			//	//Debug_Printf(_T("WM_MOUSEMOVE")) ;
			//	break ;

			case WM_MOUSEHOVER :

				break ;

			//case WM_MOUSELEAVE :
			//	//m_ShellTray.SetInfo(_T("values...."),_T("MOUSE MOVE")) ;
			//	//Debug_Printf(_T("WM_MOUSELEAVE")) ;
			//	break ;
			}

			//if(nEvent==)
			//{
			//	
			//	//this->SendMessage(WM_SIZE,SIZE_RESTORED,NULL) ;
			//}
		}
		
		break ;
	}
}
