/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCTaskQueueDlg.h"
#include "../Language/XCRes_ENU/resource.h"
#include "../../Common/ptGlobal.h"
#include "../ptMultipleLanguage.h"
#include "../XCGlobal.h"
#include "../../Common/ptDebugView.h"

#ifdef VERSION_HDD_QUEUE

CXCTaskQueueDlg::CXCTaskQueueDlg(CXCTaskQueue* pTaskQueue,const pt_STL_vector(CptString)& SrcVer,const pt_STL_vector(CptString)& DstVer)
	:CptDialog(IDD_DIALOG_TASKQUEUE,NULL,CptMultipleLanguage::GetInstance()->GetResourceHandle()),
	m_bPause(false),m_pTaskQueue(pTaskQueue),m_TaskQueResult(TQRT_None),m_SrcVer(SrcVer),m_DstVer(DstVer)
{
	_ASSERT(m_pTaskQueue!=NULL) ;
}


CXCTaskQueueDlg::~CXCTaskQueueDlg(void)
{
	
}


BOOL CXCTaskQueueDlg::OnInitDialog() 
{
	this->CenterScreen() ;

	// 设置图标
	HICON hIcon = CptMultipleLanguage::GetInstance()->GetIcon(IDI_SMALL) ;
	this->SetIcon(hIcon) ;

	CptGlobal::SetButtonCheck(this->GetSafeHwnd(),IDC_CHECK_PAUSE,true) ;

	this->SetTimer(TIMER_ID_ONE_SECOND,1000) ;

	this->OnTimer(TIMER_ID_ONE_SECOND) ;

	m_SrcFileListBox.Attach(this->GetDlgItem(IDC_LIST_SOURCEFILE)) ;
	m_DestFolderListBox.Attach(this->GetDlgItem(IDC_LIST_DESTINATIONFOLDER)) ;

	// 添加 source file 到 list box
	for(size_t i=0;i<m_SrcVer.size();++i)
	{
		m_SrcFileListBox.AddString(m_SrcVer[i].c_str()) ;
	}

	// 添加 destination folder 到 list box
	for(size_t i=0;i<m_DstVer.size();++i)
	{
		m_DestFolderListBox.AddString(m_DstVer[i].c_str()) ;
	}

	m_SrcVer.clear() ;
	m_DstVer.clear() ;

	return CptDialog::OnInitDialog() ;
}

void CXCTaskQueueDlg::OnClose() 
{
	this->KillTimer(TIMER_ID_ONE_SECOND) ;

	CptDialog::OnClose() ;
}

bool CXCTaskQueueDlg::OnCancel() 
{
	bool bRet = false ;

#ifdef _DEBUG
	bRet = true ;
#else
	CptString strText = CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_SUREEXIT) ;
	CptString strWarning = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;

	bRet = (::MessageBox(this->GetSafeHwnd(),strText.c_str(),strWarning.c_str(),MB_YESNO)==IDYES)  ;
#endif

	if(bRet)
	{
		m_TaskQueResult = TQRT_ForceExit ;
	}
	
	return bRet ;
}

void CXCTaskQueueDlg::OnButtonClick(int nButtonID) 
{
	switch(nButtonID)
	{
	case IDC_BUTTON_RUN: // 运行
		{
			CptString strText = CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_ALLOFFOLLOWTASKSWILLPAUSE) ;
			CptString strWarning = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;

			if(::MessageBox(this->GetSafeHwnd(),strText.c_str(),strWarning.c_str(),MB_YESNO)==IDYES) 
			{
				m_TaskQueResult = TQRT_ForceRun ;
				this->OnOK() ;
			}
		}
		break ;

	case IDC_BUTTON_EXIT: // 退出
		{
			this->Close() ;
		}
		break ;

	case IDC_CHECK_PAUSE: // 暂停
		{
			m_bPause = !CptGlobal::GetButtonCheck(this->GetSafeHwnd(),IDC_CHECK_PAUSE) ;

			if(!m_bPause)
			{
				CptString strText = CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_ALLOFFOLLOWTASKSWILLPAUSE) ;
				CptString strWarning = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;

				bool b = (::MessageBox(this->GetSafeHwnd(),strText.c_str(),strWarning.c_str(),MB_YESNO)==IDYES) ;

				CptGlobal::SetButtonCheck(this->GetSafeHwnd(),IDC_CHECK_PAUSE,b) ;
			}
		}
		break ;

	case IDC_BUTTON_HELP: // 帮助
		{
			::LaunchHelpFile(HFP_TaskQueue) ;
		}
		break ;
	}
}

void CXCTaskQueueDlg::OnTimer(int nTimerID)
{
	int QuePos = m_pTaskQueue->Wait(0) ;

	if(QuePos>0)
	{// 继续等待
		DWORD dwTimeLeft = m_pTaskQueue->GetTimeLeftOfRunningTask() ;
		if(dwTimeLeft!=-1)
		{
			this->SetDlgItemInt(IDC_STATIC_LEFTTIME,dwTimeLeft) ;
		}
		else
		{
			this->SetDlgItemText(IDC_STATIC_LEFTTIME,_T("-")) ;
		}
		
		this->SetDlgItemInt(IDC_STATIC_POSITION,QuePos) ;
	}
	else
	{// 轮到自己了
		
		if(m_bPause)
		{// 如果用户选择了不自动启动任务，则把位置让给后面正在的排队任务
			m_pTaskQueue->MovePriority(false) ;
		}
		else
		{
			this->KillTimer(TIMER_ID_ONE_SECOND) ;

			m_TaskQueResult = TQRT_MyTurn ;

			this->Close() ;
		}
	}
}

int CXCTaskQueueDlg::OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_NOTIFY && wParam==IDC_SPIN_PRIORITY)
	{
		//Debug_Printf(_T("CXCTaskQueueDlg::OnProcessMessage() 1")) ;

		LPNMHDR lpnmh = (LPNMHDR)lParam;

		switch(lpnmh->code)
		{
		case UDN_DELTAPOS: 
			{
				//Debug_Printf(_T("CXCTaskQueueDlg::OnProcessMessage() 2")) ;

				LPNMUPDOWN  lpnmud = (LPNMUPDOWN)lParam ;

				if(lpnmud->iDelta == 1) //如果点击的是Spin中的往下按钮
				{
					Debug_Printf(_T("CXCTaskQueueDlg::OnProcessMessage() down")) ;
					int QuePos = m_pTaskQueue->MovePriority(false) ;

					this->SetDlgItemInt(IDC_STATIC_POSITION,QuePos) ;
				}
				else if(lpnmud->iDelta == - 1)    //如果点击的是Spin中往上按钮
				{
					Debug_Printf(_T("CXCTaskQueueDlg::OnProcessMessage() up")) ;
					int QuePos = m_pTaskQueue->MovePriority(true) ;

					this->SetDlgItemInt(IDC_STATIC_POSITION,QuePos) ;
				}
			}
			return 0 ;

		default: break ;
		}
	}

	return CptDialog::OnProcessMessage(hWnd,uMsg, wParam, lParam) ;
}

#endif