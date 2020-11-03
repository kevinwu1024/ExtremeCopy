/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCNormalWndUIState.h"
#include "UI/MainDialog.h"

CXCNormalWndUIState::CXCNormalWndUIState(void)
{
	m_bUpdateTotalFiles = false ;
	
}

CXCNormalWndUIState::~CXCNormalWndUIState(void)
{
}

void CXCNormalWndUIState::UpdateOneSecondUI(SXCUIOneSecondUpdateDisplay& osud,bool bCopyDone) 
{
	CMainDialog::SSateDisplayStrings sds ;

	const CXCStatistics* pStatistics = m_pMainDlg->m_pTransApp->GetStatistics() ;

	_ASSERT(pStatistics!=NULL) ;

	if(pStatistics==NULL)
	{
		return ;
	}

	if(osud.strCurSrcFileName.GetLength()>0)
	{
		this->UpdateBeginCopyOneFileUI(osud.strCurSrcFileName,osud.strCurDstFileName) ;
	}

	const bool bFolderSizeDone = pStatistics->DoesFolderSizeDone() ;

	sds.uFlag = CMainDialog::SSateDisplayStrings::nSppedMask 
		| CMainDialog::SSateDisplayStrings::nLapseTimeMask;

	if(bFolderSizeDone)
	{
		sds.uFlag |= CMainDialog::SSateDisplayStrings::nRemainTimeMask
			| CMainDialog::SSateDisplayStrings::nRemainFileMask;
	}

	m_pMainDlg->GetSateValueDisplayString(sds) ;

	if(!bCopyDone)
	{
		m_pMainDlg->SetDlgItemText(IDC_STATIC_SPEED,sds.strSpeed.c_str()) ;// 速度
		m_pMainDlg->SetDlgItemText(IDC_STATIC_LAPSETIME,sds.strLapseTime.c_str()) ; // 逝去时间
	}


	if(!m_pMainDlg->m_bVerifyPhase)
	{

		if(bFolderSizeDone)
		{
			m_pMainDlg->SetDlgItemText(IDC_STATIC_REMAINTIME,sds.strRemainTime.c_str()) ; // 剩余时间
			m_pMainDlg->SetDlgItemText(IDC_STATIC_REMAINFILES,sds.strRemainFiles.c_str()) ; // 剩余文件数量
		}

		if(!this->m_pMainDlg->m_bVerifyPhase)
		{// 若为复制数据阶段
			const SStatisticalValue& StaData = pStatistics->GetStaData() ;

			CptString strSize = ::GetSizeString(StaData.uTransSize) ;

			m_pMainDlg->SetDlgItemText(IDC_STATIC_TRANSFORMSIZE,strSize.c_str()) ; // 完成的文件大小字节

			if(pStatistics->DoesFolderSizeDone() && StaData.uTotalSize>0)
			{// 当全部的源大小确定后,才计算总的进度
				// 总的文件复制进度
				const int nProgress = (int)((StaData.uTransSize*100)/StaData.uTotalSize) ;

				m_pMainDlg->m_TotalSkinProgressBar.SetValue(nProgress) ;
				m_pMainDlg->m_Win7TaskBarProgress.SetProgressValue(m_pMainDlg->GetSafeHwnd(),nProgress) ;
			}

			if(StaData.uCurWholeSize>0)
			{// 当前文件进度
				m_pMainDlg->m_CurSkinProgressBar.SetValue((int)((StaData.uCurOperateSize*100)/(StaData.uCurWholeSize))) ;
			}

			if(!m_pMainDlg->IsSimpleUI())
			{
				CMainDialog::SSateDisplayStrings sds ;

				sds.uFlag = CMainDialog::SSateDisplayStrings::nDoneFilesMask;

				if(m_bUpdateTotalFiles)
				{
					sds.uFlag |= CMainDialog::SSateDisplayStrings::nRemainSizeMask;
				}

				m_pMainDlg->GetSateValueDisplayString(sds) ;

				m_pMainDlg->SetDlgItemText(IDC_STATIC_DONEFILES,sds.strDoneFiles.c_str()) ; // 完成文件数量

				if(m_bUpdateTotalFiles)
				{
					m_pMainDlg->SetDlgItemText(IDC_STATIC_REMAINSIZE,sds.strRemainSize.c_str()) ; // 剩余文件大小
				}
			}
		}

		if(!m_pMainDlg->IsSimpleUI())
		{
			const bool bIsShowTotal = (!m_bUpdateTotalFiles && bFolderSizeDone) ;

			sds.uFlag = 0 ;

			if(bIsShowTotal)
			{
				sds.uFlag |= CMainDialog::SSateDisplayStrings::nTotalFilesMask
					| CMainDialog::SSateDisplayStrings::nTotalSizeMask ;
			}

			m_pMainDlg->GetSateValueDisplayString(sds) ;

			if(bIsShowTotal && sds.strTotalFiles.Compare(_T("-"))!=0)
			{
				m_pMainDlg->SetDlgItemText(IDC_STATIC_TOTALFILES,sds.strTotalFiles.c_str()) ; // 总的文件数目

				if(m_bUpdateTotalFiles || sds.strTotalSize.Compare(_T("-"))!=0)
				{
					m_bUpdateTotalFiles = true ;
					m_pMainDlg->SetDlgItemText(IDC_STATIC_TOTALSIZE,sds.strTotalSize.c_str()) ; // 总的文件大小
				}
			}
		}
	}

}


void CXCNormalWndUIState::ProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)  
{
}


void CXCNormalWndUIState::UpdateBeginCopyOneFileUI(CptString strSrcFile,CptString strDstFile) 
{
	if(strSrcFile.GetLength()>0)
	{
		CptStringList sl ;

		m_pMainDlg->SetDlgItemText(IDC_STATIC_SOURCEFILE,strSrcFile.c_str()) ;
		m_pMainDlg->SetDlgItemText(IDC_STATIC_DESTINATIONFILE,strDstFile.c_str()) ;

		if(!m_pMainDlg->m_bVerifyPhase)
		{
			if(sl.Split(strSrcFile,_T("\\"))>0 || sl.Split(strSrcFile,_T("/"))>0)
			{
				m_pMainDlg->SetDlgItemText(IDC_STATIC_CURRENTFILE,sl[sl.GetCount()-1].c_str()) ;// 当前复制的文件
			}
		}
	}
}

void CXCNormalWndUIState::UpdateCopyDataOccuredUI(bool bReadOrWrite,UINT uSize) 
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

		_ASSERT(this->m_pMainDlg->m_bVerifyPhase) ;

		if(this->m_pMainDlg->m_bVerifyPhase)
		{// 若为检查数据阶段
			const int nProgress = (int)((StaData.uVerifyDataSize*100)/StaData.uTotalSize) ;

			m_pMainDlg->m_TotalSkinProgressBar.SetValue(nProgress) ;
			m_pMainDlg->m_Win7TaskBarProgress.SetProgressValue(m_pMainDlg->GetSafeHwnd(),nProgress) ;
		}
	}
}

void CXCNormalWndUIState::UpdateUI(const EUpdateUIType uuit,void* pParam1,void* pParam2)  
{
	switch(uuit)
	{
	case UUIT_OneSecond:
		{
			_ASSERT(pParam1) ;

			if(!m_pMainDlg->m_bExit)
			{
				SXCUIOneSecondUpdateDisplay* p = (SXCUIOneSecondUpdateDisplay*)pParam1 ;
				BOOL bCopyDone = (BOOL)pParam2 ;

				this->UpdateOneSecondUI(*p,bCopyDone?true:false) ;
			}

		}
		
		break ;

	case UUIT_BeginCopyOneFile:
		{
			if(pParam1!=NULL && pParam2 !=NULL && !m_pMainDlg->m_bExit)
			{
				CptString* pstrSrc = (CptString*)pParam1 ;
				CptString* pstrDst = (CptString*)pParam2 ;

				if(pstrSrc->GetLength()>0)
				{
					this->UpdateBeginCopyOneFileUI(*pstrSrc,*pstrDst) ;
				}

			}
		}
		
		break ;

	//case UUIT_FinishCopyOneFile:
	//	this->UpdateFinishCopyOneFileUI() ;
	//	break ;

	case UUIT_CopyDataOccured:
		{
			if(pParam1!=NULL && pParam2 !=NULL && !m_pMainDlg->m_bExit)
			{
				bool bReadOrWrite = *(bool*)pParam1 ;
				UINT uSize = *((UINT*)pParam2) ;

				this->UpdateCopyDataOccuredUI(bReadOrWrite,uSize) ;
			}
		}
		
		break ;
	}
}

void CXCNormalWndUIState::SetParameter(CXCGUIState* pNextState,CMainDialog* pDlg) 
{
	CXCGUIState::SetParameter(pNextState,pDlg) ;

	_ASSERT(pDlg!=NULL) ;
}

void CXCNormalWndUIState::OnEnter(CptString strCurFileName) 
{
	_ASSERT(m_pMainDlg!=NULL) ;

	m_pMainDlg->SetDlgItemText(IDC_STATIC_CURRENTFILE,strCurFileName.c_str()) ;
}

CptString CXCNormalWndUIState::GetCurFileName()  
{
	_ASSERT(m_pMainDlg!=NULL) ;

	TCHAR szBuf[MAX_PATH] = {0} ;

	m_pMainDlg->GetDlgItemText(IDC_STATIC_CURRENTFILE,szBuf,sizeof(szBuf)/sizeof(TCHAR)) ;

	return szBuf ;
}
