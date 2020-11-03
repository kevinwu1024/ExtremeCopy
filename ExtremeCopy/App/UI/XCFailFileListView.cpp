/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCFailFileListView.h"

#include "..\Language\XCRes_ENU\resource.h"
#include "..\..\Common\ptTime.h"
#include <shellapi.h>
#include "MainDialog.h"

CXCFailFileListView::CXCFailFileListView(void):m_hTackMenu(NULL),
		m_pFailedFile(NULL)
{
}

CXCFailFileListView::~CXCFailFileListView(void)
{
	const int nCount = m_ImageList.GetImageCount() ;
	if(nCount>0)
	{
		HICON hIcon = NULL ;
		for(int i=0;i<nCount;++i)
		{
			hIcon = m_ImageList.GetIcon(i,0) ;
			::DestroyIcon(hIcon) ;
		}
		
		m_ImageList.RemoveAll() ;
	}

	this->DestroyTackMenu() ;
}

void CXCFailFileListView::SetUIEventCB(CXCFailedFile* pFailedFile)
{
	m_pFailedFile = pFailedFile ;
}

void CXCFailFileListView::DestroyTackMenu() 
{
	if(m_hTackMenu!=NULL)
	{
		::DestroyMenu(m_hTackMenu) ;
		m_hTackMenu = NULL ;
	}
}

void CXCFailFileListView::ShowContextMenu() 
{
	if(m_hWnd!=NULL)
	{
		std::vector<int> SelectIndexVer ;
		this->GetSelectIndexs(SelectIndexVer) ;

		if(!SelectIndexVer.empty())
		{
			this->DestroyTackMenu() ;

			m_hTackMenu = ::CptMultipleLanguage::GetInstance()->GetMenu(IDR_MENU1) ;

			HMENU hContextMenu = ::GetSubMenu(m_hTackMenu,2) ;

			SptPoint point ;
			::GetCursorPos(point.GetPOINTPointer()) ;

			bool bSourceEnable = !(SelectIndexVer.size()>1) ;
			bool bDestinationEnable = !(SelectIndexVer.size()>1) ;
			bool bShowRetryItem = true ;
			bool bShowCancelItem = true ;

			CptString strFail = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_STATUS_FAILED) ;
			CptString strWaitting = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_STATUS_WAITING) ;

			const int nBufSize = MAX_PATH*4 ;
			TCHAR* pBuf = new TCHAR[nBufSize] ;

			if(pBuf!=NULL)
			{
				for(size_t i=0;i<SelectIndexVer.size();++i)
				{
					// 判断源文件是否存在
					if(bSourceEnable)
					{
						this->GetItemText(SelectIndexVer[i],1,pBuf,nBufSize) ;

						if(pBuf[0]==NULL || !IsFileExist(pBuf))
						{
							bSourceEnable = false ;
						}
					}

					// 判断目标文件是否存在
					if(bDestinationEnable)
					{
						this->GetItemText(SelectIndexVer[i],2,pBuf,nBufSize) ;

						if(pBuf[0]==NULL || !IsFileExist(pBuf))
						{
							bDestinationEnable = false ;
						}
					}

					// 检查是否允许运行
					if(bShowRetryItem)
					{
						this->GetItemText(SelectIndexVer[i],4,pBuf,MAX_PATH*4) ;

						if(strFail.Compare(pBuf)!=0)
						{
							bShowRetryItem = false ;
						}
						else if(strWaitting.Compare(pBuf)!=0)
						{// 必须源文件及目标文件同时存在才可以 retry

							bShowRetryItem = (bSourceEnable && bDestinationEnable); // 默认是不显示 retry

							bShowCancelItem = false ;
						}
					}

				}

				if(!bShowRetryItem && !bShowCancelItem)
				{
					::EnableMenuItem(hContextMenu,ID_MENU_FAILEDFILELIST_RETRY,MF_GRAYED) ; // retry 变灰
				}

				if(bShowCancelItem)
				{// 显示 'cancel' 项
					::RemoveMenu(hContextMenu,ID_MENU_FAILEDFILELIST_RETRY,MF_BYCOMMAND) ;
				}
				else
				{
					::RemoveMenu(hContextMenu,ID_MENU_FAILEDFILELIST_CANCEL,MF_BYCOMMAND) ;
				}

				if(!bSourceEnable)
				{
					::EnableMenuItem(hContextMenu,ID_MENU_FAILEDFILELIST_LOCATION_SOURCE,MF_GRAYED) ;
					::EnableMenuItem(hContextMenu,ID_MENU_FAILEDFILELIST_PROPERTY_SOURCE,MF_GRAYED) ;
				}

				if(!bDestinationEnable)
				{
					::EnableMenuItem(hContextMenu,ID_MENU_FAILEDFILELIST_LOCATION_DESTINATION,MF_GRAYED) ;
					::EnableMenuItem(hContextMenu,ID_MENU_FAILEDFILELIST_PROPERTY_DESTINAMTION,MF_GRAYED) ;
				}

				SAFE_DELETE_MEMORY_ARRAY(pBuf) ;
			}
			
			::TrackPopupMenu(hContextMenu,TPM_LEFTALIGN,point.nX,point.nY,0,m_hWnd,NULL) ;
		}
	}
}

bool CXCFailFileListView::Attach(HWND hWnd) 
{
	bool bRet = CptListView::Attach(hWnd) ;

	if(bRet)
	{
		CptString strTem ;

		this->SetExtendedStyle(this->GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT |LVS_EX_SUBITEMIMAGES) ;
		LVCOLUMN col ;
		::memset(&col,0,sizeof(col)) ;

		int nIndex = 0 ;

		// 0
		// No
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT  ;
		strTem = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_NO) ;
		col.pszText = (LPTSTR)strTem.c_str() ;
		col.cchTextMax = 0 ;
		col.cx = 31 ;
		this->InsertColumn(nIndex,col) ;
		++nIndex ;

		// 1
		// source file
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT  ;
		strTem = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_SOURCEFILE) ;
		col.pszText = (LPTSTR)strTem.c_str() ;
		col.cchTextMax = 6 ;
		col.cx = 110 ;
		this->InsertColumn(nIndex,col) ;
		++nIndex ;

		// 2
		// destination 
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT ;
		strTem = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_DESTINATION) ;
		col.pszText = (LPTSTR)strTem.c_str() ;
		col.cchTextMax = 6 ;
		col.cx = 110 ;
		this->InsertColumn(nIndex,col) ;
		++nIndex ;

		// 3
		// error string
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT ;
		strTem = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_ERRORCODESTRING) ;
		col.pszText = (LPTSTR)strTem.c_str() ;
		col.cchTextMax = 6 ;
		col.cx = 140 ;
		this->InsertColumn(nIndex,col) ;
		++nIndex ;

		// 4
		// status
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH ;
		col.fmt = LVCFMT_LEFT ;
		strTem = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_STATUS) ;
		col.pszText = (LPTSTR)strTem.c_str() ;
		col.cchTextMax = 6 ;
		col.cx = 60 ;
		this->InsertColumn(nIndex,col) ;
		++nIndex ;

		// 5
		// fail time
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT ;
		strTem = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_CREATETIME) ;
		col.pszText = (LPTSTR)strTem.c_str() ;
		col.cchTextMax = 6 ;
		col.cx = 60 ;
		this->InsertColumn(nIndex,col) ;
		++nIndex ;

		// 6
		// code
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT ;
		strTem = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_ERRORCODE) ;
		col.pszText = (LPTSTR)strTem.c_str() ;
		col.cchTextMax = 6 ;
		col.cx = 20 ;
		this->InsertColumn(nIndex,col) ;
		++nIndex ;

		// LVS_EX_FULLROWSELECT
		//this->SetBkColor(UI_BG_COLOR) ;
		//this->SetTextBkColor(UI_BG_COLOR) ;

		//HWND hHeadWnd = this->GetHeaderHwnd() ;

		//m_SkinHeaderCtrl.Attach(hHeadWnd) ;
	}

	return bRet ;
}

int CXCFailFileListView::AddFailFile(const SFailedFileInfo& FailFileInfo)
{
	bool bIsNew = false ;

	if(m_ImageList.GetImageCount()==0)
	{
		SptSize size(16,16) ;

		m_ImageList.Create(size) ;

		HICON hIconTem = NULL ;

		hIconTem = ::CptMultipleLanguage::GetInstance()->GetIcon(IDI_ICON_STATUS_FAIL) ;
		m_ImageList.AddIcon(hIconTem) ;

		hIconTem = ::CptMultipleLanguage::GetInstance()->GetIcon(IDI_ICON_STATUS_SUCCESS) ;
		m_ImageList.AddIcon(hIconTem) ;

		hIconTem = ::CptMultipleLanguage::GetInstance()->GetIcon(IDI_ICON_STATUS_RUNNING) ;
		m_ImageList.AddIcon(hIconTem) ;

		hIconTem = ::CptMultipleLanguage::GetInstance()->GetIcon(IDI_ICON_STATUS_WAITING) ;
		m_ImageList.AddIcon(hIconTem) ;

		this->SetImageList(m_ImageList.GetHandle(),LVSIL_SMALL) ;

		bIsNew = true ;
	}

	SFailedFileInfo ffi ;
	if(m_pFailedFile->GetFailedFileByFileID(FailFileInfo.uFileID,ffi))
	{
		int nIndex = this->GetItemCount() ;

		CptString str ;

		str.Format(_T("%d"),nIndex+1) ;
		this->InsertItem(nIndex,(LPTSTR)str.c_str()) ;

		this->UpdateFailFileStatusByID(ffi) ;
	}
	//else
	//{
	//	_ASSERT(m_pFailedFile!=NULL) ;

	//	if(FailFileInfo.uFileID==2)
	//	{
	//		int aa = 0 ;
	//	}

	//	bIsNew = (!m_pFailedFile->IsFailedFileExist(FailFileInfo)) ;
	//}

	//if(bIsNew)
	//{
	//	int nIndex = this->GetItemCount() ;

	//	CptString str ;

	//	str.Format(_T("%d"),nIndex+1) ;
	//	this->InsertItem(nIndex,(LPTSTR)str.c_str()) ;

	//	this->UpdateFailFileStatusByID(FailFileInfo) ;
	//}

//	this->SetItemText(nIndex,1,(LPTSTR)FailFileInfo.strSrcFile.c_str()) ; // source file
//	this->SetFileIcon(nIndex,1) ;
//
//	//{
//	//	LVITEM item ;
//	//	::memset(&item,0,sizeof(item)) ;
//
//	//	item.mask = LVIF_IMAGE  ;
//	//	item.iItem = nIndex ;
//	//	item.iSubItem = 0 ;
//	//	item.iImage = this->GetIconIndexByStatus(FailFileInfo.Status) ;
//
//	//	bRet = this->SetItem(item) ;
//	//}
////	this->ChangeFailFileStatusByID(nIndex,FailFileInfo.Status) ;
//
//	this->SetItemText(nIndex,2,(LPTSTR)FailFileInfo.strDstFile.c_str()) ; // destination file
//	this->SetFileIcon(nIndex,2) ;

	//TCHAR* pBuf = new TCHAR[512+1] ;

	//if(pBuf)
	//{
	//	if(FailFileInfo.nSystemErrorCode!=0)
	//	{// error code
	//		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,FailFileInfo.nSystemErrorCode,0,pBuf,512,NULL) ;

	//		this->SetItemText(nIndex,3,pBuf) ;
	//	}

	//	// status
	//	//CptString str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_STATUS_FAILED) ;
	//	//this->SetItemText(nIndex,4,(LPTSTR)str.c_str()) ;
	//	this->SetItemText(nIndex,4,(LPTSTR)this->GetStatusString(ffi.Status)) ;

	//	// failed time
	//	CptTime::Time(pBuf) ; 

	//	this->SetItemText(nIndex,5,pBuf) ;
	//}

	//SAFE_DELETE_MEMORY_ARRAY(pBuf) ;

	return this->GetItemCount() ;
}

void CXCFailFileListView::SetFileIcon(int nItemIndex,int nSubItemIndex)
{
	TCHAR* pBuf = new TCHAR[MAX_PATH*4] ;

	if(pBuf!=NULL && this->GetItemText(nItemIndex,nSubItemIndex,pBuf,MAX_PATH*4) && IsFileExist(pBuf))
	{// 加载图标
		int nImageIndex = -1 ;

		SHFILEINFO   sfi;
		::memset(&sfi,0,sizeof(sfi)) ;
		HIMAGELIST hSystemImageList = (HIMAGELIST)::SHGetFileInfo(pBuf,0,&sfi,sizeof(sfi),SHGFI_ICON);

		if(sfi.hIcon!=NULL)
		{
			pt_STL_map(int,int)::iterator it = m_SystemIndex2IconIndexMap.find(sfi.iIcon) ;

			if(it!=m_SystemIndex2IconIndexMap.end())
			{// 存在,则直接使用
				nImageIndex = (*it).second ;

				::DestroyIcon(sfi.hIcon) ;
				sfi.hIcon = NULL ;
			}
			else
			{// 不存在,则入档
				m_ImageList.AddIcon(sfi.hIcon) ;

				nImageIndex = m_ImageList.GetImageCount()-1  ;

				m_SystemIndex2IconIndexMap[sfi.iIcon] = nImageIndex ;

				HIMAGELIST hImageList = this->SetImageList(m_ImageList.GetHandle(),LVSIL_SMALL) ;

				if(hImageList!=NULL)
				{
					::ImageList_Destroy(hImageList) ;
					hImageList = NULL ;
				}
			}
		}

		if(nImageIndex!=-1)
		{
			LVITEM item ;
			::memset(&item,0,sizeof(item)) ;

			item.mask = LVIF_IMAGE  ;
			item.iItem = nItemIndex ;
			item.iSubItem = nSubItemIndex ;
			item.iImage = nImageIndex ;

			this->SetItem(item) ;
		}

	}

	SAFE_DELETE_MEMORY_ARRAY(pBuf) ;
}

bool CXCFailFileListView::UpdateFailFileStatusByID(const SFailedFileInfo& ffi) 
{
	bool bRet = false ;

	if(ffi.nIndex>=0 && ffi.nIndex<this->GetItemCount())
	{
		if(ffi.strSrcFile.GetLength()>0)
		{
			CptString strFile = CptGlobal::MakeUnlimitFileName(ffi.strSrcFile,false) ;
			this->SetItemText(ffi.nIndex,1,(LPTSTR)strFile.c_str()) ; // source file

			this->SetFileIcon(ffi.nIndex,1) ;
		}

		LVITEM item ;
		::memset(&item,0,sizeof(item)) ;

		item.mask = LVIF_IMAGE  ;
		item.iItem = ffi.nIndex ;
		item.iSubItem = 0 ;
		item.iImage = this->GetIconIndexByStatus(ffi.Status) ;

		bRet = this->SetItem(item) ;

		if(ffi.strDstFile.GetLength()>0)
		{
			CptString strFile = CptGlobal::MakeUnlimitFileName(ffi.strDstFile,false) ;
			this->SetItemText(ffi.nIndex,2,(LPTSTR)strFile.c_str()) ; // destination file
			this->SetFileIcon(ffi.nIndex,2) ;
		}

		const int nBufSize = 512 ;

		TCHAR* pBuf = new TCHAR[nBufSize] ;

		if(pBuf!=NULL)
		{
			::memset(pBuf,0,nBufSize*sizeof(TCHAR)) ;

			// 错误字符串
			if(ffi.ErrorCode.nSystemError==0)
			{
				if(ffi.ErrorCode.AppError>0)
				{
					CptString strTxt = CMainDialog::GetAppErrorString(ffi.ErrorCode.AppError) ;
					::_tcscpy(pBuf,strTxt.c_str()) ;
				}
				//::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,ffi.ErrorCode.nSystemError,0,pBuf,512,NULL) ;
			}
			else
			{
				::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,ffi.ErrorCode.nSystemError,0,pBuf,nBufSize,NULL) ;
			}
			
			this->SetItemText(ffi.nIndex,3,pBuf) ;

			// 状态
			//CptString str = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_STATUS_FAILED) ;
			this->SetItemText(ffi.nIndex,4,(LPTSTR)this->GetStatusString(ffi.Status).c_str()) ;

			// 失败时间
			CptTime::Time(pBuf) ; 

			this->SetItemText(ffi.nIndex,5,pBuf) ;

			// 错误码
#pragma warning(push)
#pragma warning(disable:4996)
			if(ffi.ErrorCode.nSystemError==0)
			{
				::_itot(ffi.ErrorCode.AppError,pBuf,10) ;
			}
			else
			{
				::_itot(ffi.ErrorCode.nSystemError,pBuf,10) ;
			}
			
#pragma warning(pop)
			this->SetItemText(ffi.nIndex,6,pBuf) ;

			SAFE_DELETE_MEMORY_ARRAY(pBuf) ;
		}

	}

	return bRet ;
}

CptString CXCFailFileListView::GetStatusString(EFailFileStatusType status)
{
	switch(status)
	{
	default:
	case EFST_Unknown: return _T("") ;

	case EFST_Failed: return ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_STATUS_FAILED) ;

	case EFST_Waitting: return ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_STATUS_WAITING) ;
		break ;

	case EFST_Running: return ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_STATUS_RUNNING) ;
		break ;

	case EFST_Success: return ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_STATUS_SUCCESS) ;
		break ;
	}
}




void CXCFailFileListView::OnRButtonUp(int nFlag,const SptPoint& pt) 
{
	//Debug_Printf(_T("WM_RBUTTONUP")) ;
}

void CXCFailFileListView::OnRButtonDown(int nFlag,const SptPoint& pt) 
{
	std::vector<int> SelectIndexVer ;
	this->GetSelectIndexs(SelectIndexVer) ;

	this->ShowContextMenu() ;
	//Debug_Printf(_T("WM_RBUTTONDOWN index=%d"),SelectIndexVer[0]) ;
}

int CXCFailFileListView::GetIconIndexByStatus(EFailFileStatusType ffst) const
{
	switch(ffst)
	{
	default:
	case EFST_Failed: return STATUSICONINDEX_FAIL ;
	case EFST_Waitting: return STATUSICONINDEX_WAITING ;
	case EFST_Running: return STATUSICONINDEX_RUNNING ;
	case EFST_Success: return STATUSICONINDEX_SUCCESS ;
	}
}


void CXCFailFileListView::LocateFile(bool bSrcOrDst) 
{
	std::vector<int> SelectIndexVer ;
	this->GetSelectIndexs(SelectIndexVer) ;

	if(!SelectIndexVer.empty())
	{
		TCHAR* pBuf = new TCHAR[MAX_PATH*4] ;

		CptString str ;

		this->GetItemText(SelectIndexVer[0],bSrcOrDst?1:2,pBuf,MAX_PATH*4) ;

		if(pBuf[0]!=NULL && IsFileExist(pBuf))
		{
			str.Format(_T("/select, \"%s\""),pBuf) ;
			::ShellExecute(m_hWnd,_T("open"),_T("explorer.exe"),str.c_str(),NULL,SW_SHOW) ;
		}

		SAFE_DELETE_MEMORY_ARRAY(pBuf) ;
	}
}

void CXCFailFileListView::PropertyFile(bool bSrcOrDst) 
{
	std::vector<int> SelectIndexVer ;
	this->GetSelectIndexs(SelectIndexVer) ;

	if(!SelectIndexVer.empty())
	{
		TCHAR* pBuf = new TCHAR[MAX_PATH*4] ;
		this->GetItemText(SelectIndexVer[0],bSrcOrDst?1:2,pBuf,MAX_PATH*4) ;

		if(pBuf[0]!=NULL && IsFileExist(pBuf))
		{
			static SHELLEXECUTEINFO se;

			se.hwnd=m_hWnd;
			se.lpVerb=_T("properties");
			se.lpFile=pBuf;
			se.fMask= SEE_MASK_NOCLOSEPROCESS|SEE_MASK_INVOKEIDLIST |SEE_MASK_FLAG_NO_UI ;

			se.lpParameters=NULL;
			se.lpDirectory=NULL;
			se.cbSize=sizeof(se);

			::ShellExecuteEx(&se);
		}

		SAFE_DELETE_MEMORY_ARRAY(pBuf) ;
	}
}

int CXCFailFileListView::RetryAllFailedFile() 
{
	int nRet = -1 ;

//	if(m_pUIEventCB!=NULL)
	{
		nRet = 0 ;

		CptString strFail = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FAILFILETITLE_STATUS_FAILED) ;
		const int nItemCount = this->GetItemCount() ;

		const int nBufSize = MAX_PATH*4 ;
		TCHAR* pBuf = new TCHAR[nBufSize] ;

		SFailedFileInfo ffi ;
		bool bEnableRetry = false ;

		for(int i=0;i<nItemCount;++i)
		{
			this->GetItemText(i,4,pBuf,nBufSize) ;

			if(strFail.Compare(pBuf)==0)
			{
				++nRet ;

				bEnableRetry = true ;

				// list view 里读取里面的 source 和 destinatin 文件路径
				// 并且确认它们在硬盘里都存在
				for(int j=1;j<=2 && bEnableRetry;++j)
				{
					if(this->GetItemText(i,j,pBuf,nBufSize))
					{
						if(pBuf[0]==NULL || !IsFileExist(pBuf))
						{
							bEnableRetry = false ;
						}
					}
					else
					{
						_ASSERT(FALSE) ;
						bEnableRetry = false ;
					}
				}
				
				if(bEnableRetry && m_pFailedFile->GetFailedFileByIndex(i,ffi) && ffi.Status!=EFST_Waitting)
				{
					ffi.Status = EFST_Waitting ;
					m_pFailedFile->UpdateFailedFile(ffi) ;
				}
			}
		}

		SAFE_DELETE_MEMORY_ARRAY(pBuf) ;
	}

	return nRet ;
}

void CXCFailFileListView::SetSelectedFileState(EFailFileStatusType NewState)
{
	_ASSERT(m_pFailedFile!=NULL) ;

	std::vector<int> SelectIndexVer ;

	this->GetSelectIndexs(SelectIndexVer) ;

	if(!SelectIndexVer.empty())
	{
		size_t i = 0 ;
		SFailedFileInfo ffi ;

		for(i=0;i<SelectIndexVer.size();++i)
		{
			if(m_pFailedFile->GetFailedFileByIndex(SelectIndexVer[i],ffi) && ffi.Status!=NewState)
			{
				ffi.Status = NewState ;
				m_pFailedFile->UpdateFailedFile(ffi) ;
			}
		}
	}
}

int CXCFailFileListView::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) 
{
	//switch(nMsg)
	//{
	////case WM_MOUSEMOVE:
	////	Debug_Printf(_T("WM_MOUSEMOVE")) ;
	////	break;
	////case WM_LBUTTONUP:
	////	Debug_Printf(_T("WM_LBUTTONUP")) ;
	////	break;
	//case WM_RBUTTONDOWN:
	//	Debug_Printf(_T("WM_RBUTTONUP")) ;
	//	break ;
	//}

	switch(nMsg)
	{
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED)			
		{
			switch(LOWORD(wParam))
			{
			case ID_MENU_FAILEDFILELIST_CANCEL: // 取消
				{
					this->SetSelectedFileState(EFST_Failed) ;
				}
				break ;

			case ID_MENU_FAILEDFILELIST_RETRY: // 重试
				{
					this->SetSelectedFileState(EFST_Waitting) ;
				}
				
				break ;

			case ID_MENU_FAILEDFILELIST_LOCATION_SOURCE: // 定位源文件
				LocateFile(true) ;
				break ;

			case ID_MENU_FAILEDFILELIST_LOCATION_DESTINATION: // 定位目标文件
				LocateFile(false) ;
				break ;

			case ID_MENU_FAILEDFILELIST_PROPERTY_SOURCE: // 源文件属性
				{
					this->PropertyFile(true) ;
				}
				break ;

			case ID_MENU_FAILEDFILELIST_PROPERTY_DESTINAMTION: // 目标文件属性
				this->PropertyFile(false) ;
				break ;
			}
		}
		break ;
	}

	return CptListView::PreProcCtrlMsg(hWnd,nMsg,wParam, lParam) ;
}
//
//int CXCFailFileListView::PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) 
//{
//		switch(nMsg)
//	{
//	//case WM_MOUSEMOVE:
//	//	Debug_Printf(_T("WM_MOUSEMOVE")) ;
//	//	break;
//	//case WM_LBUTTONUP:
//	//	Debug_Printf(_T("WM_LBUTTONUP")) ;
//	//	break;
//	case WM_RBUTTONUP:
//		Debug_Printf(_T("WM_RBUTTONUP 2")) ;
//		break ;
//	}
//
//	return CptListView::PreProcParentMsg(hWnd,nMsg,wParam, lParam,bContinue) ;
//}
