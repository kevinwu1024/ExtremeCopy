/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "SameFileDialog.h"
#include "MainDialog.h"

#include <shellapi.h>
#include "..\..\Common\ptWinPath.h"
#include "..\Language\XCRes_ENU\resource.h"
#include "..\XCConfiguration.h"

CSameFileDialog::CSameFileDialog(CptString strSrcFile,CptString strDstFile,HWND hParentWnd)
:CptDialog(IDD_DIALOG_SAMENAME2,hParentWnd,CptMultipleLanguage::GetInstance()->GetResourceHandle()),m_ImpactFileResult(SFDB_StopCopy) 
{
	m_strSrcFile = strSrcFile ;
	m_strDstFile = strDstFile ;

	m_FileBehaviorSetting.Behavior = CopyBehavior_AskUser ;
	m_FileBehaviorSetting.SkipCondition = SameFileCondition_SameSize|SameFileCondition_SameCreateTime|SameFileCondition_SameModifyTime ;

	m_hCurFileFont = ::CreateFont(15,0,0,0,FW_BOLD,TRUE,FALSE,FALSE,DEFAULT_CHARSET,0,
					CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;

	m_hReplaceFont = ::CreateFont(13,0,0,0,FW_BOLD,TRUE,FALSE,FALSE,DEFAULT_CHARSET,0,
					CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;


	SHFILEINFO   sfi;

	::memset(&sfi,0,sizeof(sfi)) ;

	//::SHGetFileInfo(strSrcFile.c_str(),0,&sfi,sizeof(sfi),SHGFI_ICON|SHGFI_LARGEICON|SHGFI_USEFILEATTRIBUTES);
	::SHGetFileInfo(strSrcFile.c_str(),0,&sfi,sizeof(sfi),SHGFI_ICON);

	m_hFileIcon = sfi.hIcon ;
}

CSameFileDialog::~CSameFileDialog(void)
{
	if(m_hCurFileFont!=NULL)
	{
		::DeleteObject(m_hCurFileFont) ;
		m_hCurFileFont = NULL ;
	}
	
	if(m_hReplaceFont!=NULL)
	{
		::DeleteObject(m_hReplaceFont) ;
		m_hReplaceFont = NULL ;
	}
	
	if(m_hFileIcon!=NULL)
	{
		::DestroyIcon(m_hFileIcon) ;
		m_hFileIcon = NULL ;
	}
}

BOOL CSameFileDialog::OnInitDialog() 
{
	this->CenterScreen() ; 

	CptString strTilte = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_SAMEFILEDLG) ;
	::SetWindowText(this->GetSafeHwnd(),strTilte.c_str()) ;

	HWND hIconWnd = this->GetDlgItem(IDC_STATIC_ICON) ;

	::ShowWindow(hIconWnd,SW_HIDE) ;

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

	CptWinPath::SPathElementInfo pei ;

	pei.uFlag = CptWinPath::PET_FileName ;

	HWND hNewerStatic = this->GetDlgItem(IDC_STATIC_NEWER) ;

	if(hNewerStatic!=NULL)
	{
		::ShowWindow(hNewerStatic,SW_HIDE) ;
	}

	if(CptWinPath::GetPathElement(m_strSrcFile.c_str(),pei))
	{
		FILETIME   localtime; 

		this->SetDlgItemText(IDC_STATIC_SAMEFILENAME,pei.strFileName.c_str()) ;
		
		this->SetDlgItemText(IDC_STATIC_SOURCENAME,m_strSrcFile.c_str()) ;

		this->SetDlgItemText(IDC_STATIC_DESTINATIONNAME,m_strDstFile.c_str()) ;

		WIN32_FIND_DATA wfdSrc ;
		WIN32_FIND_DATA wfdDst ;

		HANDLE hFileFindSrc = ::FindFirstFile(m_strSrcFile.c_str(),&wfdSrc) ;

		if(hFileFindSrc!=INVALID_HANDLE_VALUE)
		{
			SYSTEMTIME SysTime ;
			CptString str ;

			::FileTimeToLocalFileTime(&wfdSrc.ftCreationTime,&localtime) ;
			// 创建时间
			if(::FileTimeToSystemTime(&localtime,&SysTime))
			{
				str.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
					SysTime.wYear,
					SysTime.wMonth,
					SysTime.wDay,
					SysTime.wHour,
					SysTime.wMinute,
					SysTime.wSecond) ;

				this->SetDlgItemText(IDC_STATIC_SOURCECREATETIME,str.c_str()) ;
			}

			::FileTimeToLocalFileTime(&wfdSrc.ftLastWriteTime,&localtime) ;

			// 修改时间
			if(::FileTimeToSystemTime(&localtime,&SysTime))
			{
				str.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
					SysTime.wYear,
					SysTime.wMonth,
					SysTime.wDay,
					SysTime.wHour,
					SysTime.wMinute,
					SysTime.wSecond) ;

				this->SetDlgItemText(IDC_STATIC_SOURCEMODIFYTIME,str.c_str()) ;
			}
			
			// 大小
			unsigned __int64 nSize = ::DoubleWordTo64(wfdSrc.nFileSizeLow,wfdSrc.nFileSizeHigh) ;
			str = ::GetSizeString(nSize) ;

			this->SetDlgItemText(IDC_STATIC_SOURCESIZE,str) ;

			HANDLE hFileFindDst = ::FindFirstFile(m_strDstFile,&wfdDst) ;

			if(hFileFindDst!=INVALID_HANDLE_VALUE)
			{
				// 创建时间
				::FileTimeToLocalFileTime(&wfdDst.ftCreationTime,&localtime) ;

				if(::FileTimeToSystemTime(&localtime,&SysTime))
				{
					str.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
						SysTime.wYear,
						SysTime.wMonth,
						SysTime.wDay,
						SysTime.wHour,
						SysTime.wMinute,
						SysTime.wSecond) ;

					this->SetDlgItemText(IDC_STATIC_DESTINATIONCREATETIME,str.c_str()) ;
				}

				// 修改时间
				::FileTimeToLocalFileTime(&wfdDst.ftLastWriteTime,&localtime) ;
				if(::FileTimeToSystemTime(&localtime,&SysTime))
				{
					str.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
						SysTime.wYear,
						SysTime.wMonth,
						SysTime.wDay,
						SysTime.wHour,
						SysTime.wMinute,
						SysTime.wSecond) ;

					this->SetDlgItemText(IDC_STATIC_DESTINATIONMODIFYTIME,str.c_str()) ;
				}

				// 显示文件是否更旧或更新的提示
				if(hNewerStatic!=NULL)
				{
					FILETIME   SrcLocaltime; 
					FILETIME   DstLocaltime; 

					::FileTimeToLocalFileTime(&wfdSrc.ftLastWriteTime,&SrcLocaltime) ;
					::FileTimeToLocalFileTime(&wfdDst.ftLastWriteTime,&DstLocaltime) ;

					if(SrcLocaltime.dwHighDateTime!=DstLocaltime.dwHighDateTime || 
						SrcLocaltime.dwLowDateTime!=DstLocaltime.dwLowDateTime)
					{
						bool bIsNewer = (SrcLocaltime.dwHighDateTime>DstLocaltime.dwHighDateTime || 
							(SrcLocaltime.dwHighDateTime==DstLocaltime.dwHighDateTime && 
							(SrcLocaltime.dwLowDateTime>DstLocaltime.dwLowDateTime))) ;

						CptString strNewer ;

						if(!bIsNewer)
						{// older
							strNewer = CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_OLDER) ;
						}
						else
						{// newer 
							strNewer = CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_NEWER) ;
						}

						::SetWindowText(hNewerStatic,strNewer.c_str()) ;
						::ShowWindow(hNewerStatic,SW_SHOW) ;
					}
				}
				

				// 大小
				nSize = ::DoubleWordTo64(wfdDst.nFileSizeLow,wfdSrc.nFileSizeHigh) ;
				str = ::GetSizeString(nSize) ;

				this->SetDlgItemText(IDC_STATIC_DESTINATIONSIZE,str) ;

				::FindClose(hFileFindDst) ;
			}
			::FindClose(hFileFindSrc) ;
		}

		this->UpdateDataToControl() ;
	}

	return CptDialog::OnInitDialog() ;
	//return TRUE ;
}

void CSameFileDialog::OnPaint()
{
	CptDialog::OnPaint() ;

	if(m_hFileIcon!=NULL)
	{
		HWND hIconWnd = this->GetDlgItem(IDC_STATIC_ICON) ;

		::ShowWindow(hIconWnd,SW_HIDE) ;
		SptRect rt ;
	
		::GetWindowRect(hIconWnd,rt.GetRECTPointer()) ;

		SptPoint pt = rt.GetLocation() ;

		::ScreenToClient(this->GetSafeHwnd(),pt.GetPOINTPointer()) ;

		PAINTSTRUCT ps ;

		HDC hDC = ::BeginPaint(this->GetSafeHwnd(),&ps) ;

		::DrawIconEx(hDC,pt.nX,pt.nY,m_hFileIcon,rt.GetWidth(),rt.GetHeight(),0,NULL,DI_NORMAL);

		::EndPaint(this->GetSafeHwnd(),&ps) ;
	}
}

void CSameFileDialog::UpdateDataToControl() 
{
	HWND hControl = NULL ;
	BOOL bIsSkip = FALSE ;

	::SendMessage(hControl,BM_SETCHECK,BST_CHECKED,0) ;

	if(m_FileBehaviorSetting.SkipCondition & SameFileCondition_SameSize)
	{// same size
		::SendMessage(this->GetDlgItem(IDC_CHECK_SKIPSAMESIZE),BM_SETCHECK,BST_CHECKED,0) ;
	}

	if(m_FileBehaviorSetting.SkipCondition & SameFileCondition_SameCreateTime)
	{// create time
		//::SendMessage(this->GetDlgItem(IDC_CHECK_SKIPSAMECREATETIME),BM_SETCHECK,BST_CHECKED,0) ;
	}

	if(m_FileBehaviorSetting.SkipCondition & SameFileCondition_SameModifyTime)
	{// modify time
		::SendMessage(this->GetDlgItem(IDC_CHECK_SKIPSAMEMODIFYTIME),BM_SETCHECK,BST_CHECKED,0) ;
	}
}

void CSameFileDialog::UpdateDataFromControl()
{
	m_FileBehaviorSetting.SkipCondition = 0 ;

	if(::SendMessage(this->GetDlgItem(IDC_CHECK_SKIPSAMESIZE),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{// same size
		m_FileBehaviorSetting.SkipCondition |= SameFileCondition_SameSize ;
	}

	if(::SendMessage(this->GetDlgItem(IDC_CHECK_SKIPSAMEMODIFYTIME),BM_GETCHECK,BST_CHECKED,0)==BST_CHECKED)
	{// modify time
		m_FileBehaviorSetting.SkipCondition |= SameFileCondition_SameModifyTime ;
	}
}


void CSameFileDialog::OnClose()
{
	this->UpdateDataFromControl() ;

	CptDialog::OnClose() ;
}

bool CSameFileDialog::OnCancel()
{
	if(::ConfirmExitApp(this->GetSafeHwnd()))
	{
		m_ImpactFileResult = SFDB_StopCopy ;
		//m_ImpactFileResult = SFDB_Rename ;
		return true ;
	}

	return false ;
}

void CSameFileDialog::OnButtonClick(int nButtonID) 
{
	switch(nButtonID)
	{
	case IDC_BUTTON_EXIT: // exit

		this->PerformButton(IDCANCEL) ;
		break ;

	case IDC_BUTTON_HELP: // help
		{
			::LaunchHelpFile(HFP_SameFileNameDlg) ;
		}
		break ;

	case IDC_BUTTON_REPLACE: // replace
		m_ImpactFileResult = SFDB_Replace ;
		m_FileBehaviorSetting.Behavior = CopyBehavior_AskUser ;
		this->Close() ;
		break ;

	case IDC_BUTTON_REPLACEALL: // replace all
		m_ImpactFileResult = SFDB_Replace ;
		m_FileBehaviorSetting.Behavior = CopyBehavior_ReplaceUncondition ;
		this->Close() ;
		break ;

	case IDC_BUTTON_SKIP: // skip
		m_FileBehaviorSetting.Behavior = CopyBehavior_AskUser ;
		m_ImpactFileResult = SFDB_Skip ;
		this->Close() ;
		break ;

	case IDC_BUTTON_SKIPALL: // skip all
		m_ImpactFileResult = SFDB_Skip ;
		m_FileBehaviorSetting.Behavior = CopyBehavior_SkipCondition ;
		this->Close() ;
		break ;

//	case IDC_BUTTON_TITLECLOSE2:
////	case IDC_BUTTON_STOPCOPY: // stop copy
//		if(::ConfirmExitApp(this->GetSafeHwnd()))
//		{
//			this->PerformButton(IDCANCEL) ;
//		}
//		
//		//this->OnCancelButtonClick() ;
//		break ;

	case IDC_BUTTON_RENAME: // rename
		m_FileBehaviorSetting.Behavior = CopyBehavior_AskUser ;
		m_ImpactFileResult = SFDB_Rename ;
		this->Close() ;
		break ;

	case IDC_BUTTON_RNAMEALL: // rename all
		m_ImpactFileResult = SFDB_Rename ;
		m_FileBehaviorSetting.Behavior = CopyBehavior_Rename ;
		this->Close() ;
		break ;

	case IDC_BUTTON_MORELESS: // more or less
		if(m_bMoreOrLess)
		{// 转到 less
			SptSize size(m_OrginRect.GetWidth(),m_OrginRect.GetHeight()-m_nMoreHeightOffset) ;
			this->SetWindowSize(size) ;
			this->SetDlgItemText(IDC_BUTTON_MORELESS,CptMultipleLanguage::GetInstance()->GetString(IDS_BUTTON_MORE)) ;
			//::SetDlgItemText(this->GetSafeHwnd(),IDC_BUTTON_MORELESS,CptMultipleLanguage::GetInstance()->GetString(IDS_BUTTON_MORE)) ;
			//::SetWindowPos(this->GetSafeHwnd(),HWND_TOP,0,0,m_OrginRect.GetWidth(),m_OrginRect.GetHeight()-m_nMoreHeightOffset,SWP_NOZORDER|SWP_NOMOVE) ;
		}
		else
		{// 转到 more
			this->SetWindowSize(m_OrginRect.GetSize()) ;
			//::SetDlgItemText(this->GetSafeHwnd(),IDC_BUTTON_MORELESS,CptMultipleLanguage::GetInstance()->GetString(IDS_BUTTON_LESS)) ;
			this->SetDlgItemText(IDC_BUTTON_MORELESS,CptMultipleLanguage::GetInstance()->GetString(IDS_BUTTON_LESS)) ;
			//::SetWindowPos(this->GetSafeHwnd(),HWND_TOP,0,0,m_OrginRect.GetWidth(),m_OrginRect.GetHeight(),SWP_NOZORDER|SWP_NOMOVE) ;
		}

		m_bMoreOrLess = !m_bMoreOrLess ;
		
		break ;

	//case IDCANCEL:
	//	this->Close() ;
	//	break;

		/**
	case IDC_RADIO_SAMEFILEREPLACE:
	case IDC_RADIO_SAMEFILEASKME:
		::EnableWindow(this->GetDlgItem(IDC_CHECK_SKIPSAMESIZE),FALSE) ;
		::EnableWindow(this->GetDlgItem(IDC_CHECK_SKIPSAMEMODIFYTIME),FALSE) ;
		::EnableWindow(this->GetDlgItem(IDC_CHECK_SKIPSAMECREATETIME),FALSE) ;
		break ;

	case IDC_RADIO_SAMEFILESKIP:
		::EnableWindow(this->GetDlgItem(IDC_CHECK_SKIPSAMESIZE),TRUE) ;
		::EnableWindow(this->GetDlgItem(IDC_CHECK_SKIPSAMEMODIFYTIME),TRUE) ;
		::EnableWindow(this->GetDlgItem(IDC_CHECK_SKIPSAMECREATETIME),TRUE) ;
		break ;
		/**/
	}

	
}

int CSameFileDialog::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CTLCOLORBTN :
	case WM_CTLCOLOREDIT :
	//case WM_CTLCOLORDLG :
	case WM_CTLCOLORSTATIC :
	case WM_CTLCOLORSCROLLBAR :
	case WM_CTLCOLORLISTBOX :
		{
			HDC hdc = (HDC)wParam ;

			::SetBkMode(hdc,TRANSPARENT) ;
			//HBRUSH hBrush = ::CreateSolidBrush(RGB(113,204,255)) ;
			//HBRUSH hBrush = ::CreateSolidBrush(RGB(181,211,255)) ;

			if((HWND)lParam==this->GetDlgItem(IDC_STATIC_SAMEFILENAME))
			{// 当前复制的文件
				::SelectObject(hdc,m_hCurFileFont) ;
			}
			else if((HWND)lParam==this->GetDlgItem(IDC_STATIC_REPLACE))
			{
				::SetTextColor(hdc,RGB(255,0,0)) ;
				::SelectObject(hdc,m_hReplaceFont) ;
			}
			else if(
				(HWND)lParam==this->GetDlgItem(IDC_STATIC_SOURCENAME)||
				(HWND)lParam==this->GetDlgItem(IDC_STATIC_SOURCECREATETIME)||
				(HWND)lParam==this->GetDlgItem(IDC_STATIC_SOURCEMODIFYTIME)||
				(HWND)lParam==this->GetDlgItem(IDC_STATIC_SOURCESIZE)
				)
			{
				::SetTextColor(hdc,RGB(0,128,0)) ;
				//
				//::SetTextColor(hdc,RGB(128,255,128)) ;
			}
			else if(
				(HWND)lParam==this->GetDlgItem(IDC_STATIC_DESTINATIONNAME)||
				(HWND)lParam==this->GetDlgItem(IDC_STATIC_DESTINATIONCREATETIME)||
				(HWND)lParam==this->GetDlgItem(IDC_STATIC_DESTINATIONMODIFYTIME)||
				(HWND)lParam==this->GetDlgItem(IDC_STATIC_DESTINATIONSIZE)
				)
			{
				
				::SetTextColor(hdc,RGB(128,128,128)) ;
				//::SetTextColor(hdc,RGB(0,0,0)) ;
			}
			//else if(uMsg==WM_CTLCOLORSTATIC)
			//{
			//	::SetTextColor(hdc,RGB(10,10,230)) ;
			//}

			return (int)::GetSysColorBrush(COLOR_BTNFACE) ;
			//return (int)::GetStockObject(NULL_BRUSH) ;

			//return (int)CXCConfiguration::GetInstance()->GetGlobalData()->hDlgBkBrush ;
		}
		break ;
	}

	return CptDialog::OnProcessMessage(hWnd,uMsg,wParam,lParam) ;
}