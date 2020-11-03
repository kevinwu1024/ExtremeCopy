/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptListView.h"
#include <shellapi.h>
#include "..\..\Common\ptGlobal.h"
#include "..\XCGlobal.h"

CptListView::CptListView(void)
{
	m_crBackgroud = RGB(255,0,0) ;
	m_hHScrollWnd = NULL ;
}

CptListView::~CptListView(void)
{
	if(m_hHScrollWnd!=NULL)
	{
		::DestroyWindow(m_hHScrollWnd) ;
		m_hHScrollWnd = NULL ;
	}

	m_HScrollbarImages.DeleteObject() ;
}


bool CptListView::SetBkColor(COLORREF crNew) 
{
	if(m_hWnd!=NULL)
	{
		return ::SendMessage(m_hWnd, LVM_SETBKCOLOR, 0, crNew) ? true : false;
	}

	return false ;
}

bool CptListView::SetTextBkColor(COLORREF cr) 
{
	bool bRet = false ;

	if(m_hWnd!=NULL)
	{
		bRet = ::SendMessage(m_hWnd, LVM_SETTEXTBKCOLOR, 0, (LPARAM)(COLORREF)(cr)) ? true : false ;
	}

	return bRet ;
}

COLORREF CptListView::GetTextBkColor() const
{
	COLORREF crRet = 0 ;

	if(m_hWnd!=NULL)
	{
		crRet = (COLORREF)::SendMessage(m_hWnd,LVM_GETTEXTBKCOLOR,0,0) ;
	}

	return crRet ;
}

HWND CptListView::GetHeaderHwnd() const
{
	if(m_hWnd!=NULL)
	{
		return (HWND) ::SendMessage(m_hWnd, LVM_GETHEADER, 0, 0); 
	}

	return NULL ;
}

//void CptListView::ReleasePointer()
//{
//	if(m_hWnd!=NULL)
//	{
//		HWND hParent = ::GetParent(m_hWnd) ;
//
//		SThisNode* pNode = (SThisNode*)::GetProp(hParent,s_ptListViewParentWndProc) ;
//
//		SThisNode* pPreNode = NULL ;
//		while(pNode!=NULL)
//		{
//			if(pNode->pThis==this)
//			{
//				if(pPreNode!=NULL)
//				{
//					pPreNode->pNext = pNode->pNext ;
//				}
//				else
//				{
//					::SetProp(hParent,s_ptListViewParentWndProc,NULL) ;
//				}
//
//				delete pNode ;
//				break ;
//			}
//			pPreNode = pNode ;
//			pNode = pNode->pNext ;
//		}
//	}
//}



DWORD CptListView::SetExtendedStyle(DWORD dwNewStyle)
{
	if(m_hWnd!=NULL)
	{
		return (DWORD) ::SendMessage(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM) dwNewStyle); 
	}
	
	return 0 ;
}

DWORD CptListView::GetExtendedStyle() const
{
	if(m_hWnd!=NULL)
	{
		return (DWORD) ::SendMessage(m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0); 
	}
	
	return 0 ;
}

bool CptListView::Attach(HWND hWnd) 
{
	CptCommCtrl::Attach(hWnd) ;
	//m_hWnd = hWnd ;

	//int nStyleEx = ::GetWindowLong(hWnd,GWL_STYLE) ;

	//nStyleEx |= LVS_OWNERDRAWFIXED ;

	//::SetWindowLong(hWnd,GWL_STYLE,nStyleEx) ;

	
	/**
	this->ReleasePointer() ;

	HWND hParent = ::GetParent(m_hWnd) ;

	SThisNode* pNode = new SThisNode() ;

	pNode->pThis = this ;
	pNode->pNext = (SThisNode*)::GetProp(hParent,s_ptListViewParentWndProc) ;

	::SetProp(hParent,s_ptListViewParentWndProc,(HANDLE)pNode) ;

	m_pParentDefWndProc = (WNDPROC)::SetWindowLong(hParent,GWLP_WNDPROC,(LONG)ParentMsgProc) ;
	m_pItemDefWndProc = (WNDPROC)::SetWindowLong(m_hWnd,GWLP_WNDPROC,(LONG)MsgProc) ;
	::SetProp(m_hWnd,s_ptListViewThisEntry,(HANDLE)this) ;
	/**/



	//this->CreateScrollbar() ;

	return true ;
}

void CptListView::CreateScrollbar() 
{
	// 隐藏 ListView 自带的滚动条
	::InitializeFlatSB(m_hWnd);
	::FlatSB_EnableScrollBar(m_hWnd, SB_BOTH, ESB_DISABLE_BOTH);

	m_HScrollbarImages.FirstArrow.hNormal = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_LEFT_NORMAL) ;
	m_HScrollbarImages.FirstArrow.hHover = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_LEFT_HOVER) ;
	m_HScrollbarImages.FirstArrow.hPress = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_LEFT_PRESS) ;

	m_HScrollbarImages.SecondArrow.hNormal = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_RIGHT_NORMAL) ;
	m_HScrollbarImages.SecondArrow.hHover = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_RIGHT_HOVER) ;
	m_HScrollbarImages.SecondArrow.hPress = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_RIGHT_PRESS) ;

	m_HScrollbarImages.Thumb.hNormal = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_THUMB_NORMAL) ;
	m_HScrollbarImages.Thumb.hHover = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_THUMB_HOVER) ;
	m_HScrollbarImages.Thumb.hPress = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_THUMB_PRESS) ;

	m_HScrollbarImages.Background.hNormal = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_BACKGROUND_NORMAL) ;
	//m_HScrollbarImages.Background.hHover = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_BACKGROUND_HOVER) ;
	//m_HScrollbarImages.Background.hPress = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_HOR_BACKGROUND_PRESS) ;

	SptSize ScrollbarSize ;

	CptGlobal::GetBitmapSize(m_HScrollbarImages.FirstArrow.hNormal,ScrollbarSize) ;
	const int nScrollbarHeight = ScrollbarSize.nHeight+2 ;

	// 创建滚动条窗口
	m_hHScrollWnd = ::CreateWindowEx(0,_T("STATIC"),_T(""),WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP,0,0,50,50,m_hWnd,NULL,NULL,NULL) ;

	SptRect windowRect;
	::GetWindowRect(m_hWnd,windowRect.GetRECTPointer());
	windowRect.SetLocation(0,0) ;

	int nTitleBarHeight = 0 ;
	int nDialogFrameHeight = 0;
	int nDialogFrameWidth = 0;

	const int nParentStyle = ::GetWindowLong(m_hWnd,GWL_STYLE) ;

	if(nParentStyle&WS_CAPTION)
	{
		nTitleBarHeight = ::GetSystemMetrics(SM_CYSIZE);
	}

	if((nParentStyle& WS_BORDER))
	{
		nDialogFrameHeight = GetSystemMetrics(SM_CYDLGFRAME);
		nDialogFrameWidth = GetSystemMetrics(SM_CYDLGFRAME);
	}

	if(nParentStyle & WS_THICKFRAME)
	{
		nDialogFrameHeight+=1;
		nDialogFrameWidth+=1;
	}

	//SptPoint pt = windowRect.GetLocation() ;
	//::ScreenToClient(m_hWnd,pt.GetPOINTPointer()) ;
	//windowRect.SetLocation(pt) ;
	//::GetClientRect(m_hWnd,

	windowRect.nTop+=nTitleBarHeight+nDialogFrameHeight;
	windowRect.nBottom+=nTitleBarHeight+nDialogFrameHeight;
	windowRect.nLeft +=nDialogFrameWidth;
	windowRect.nRight+=nDialogFrameWidth;

	SptRect vBar(windowRect.nRight-nDialogFrameWidth,
		windowRect.nTop-nTitleBarHeight-nDialogFrameHeight,
		windowRect.nRight+nScrollbarHeight-nDialogFrameWidth,
		windowRect.nBottom+nScrollbarHeight-nTitleBarHeight-nDialogFrameHeight);

	SptRect hBar(windowRect.nLeft-nDialogFrameWidth,
		windowRect.nBottom-nTitleBarHeight-nDialogFrameHeight,
		windowRect.nRight-3-nDialogFrameWidth,
		windowRect.nBottom+nScrollbarHeight+2-nTitleBarHeight-nDialogFrameHeight);

	int nWidth = hBar.GetWidth() ;
	int nHeight = hBar.GetHeight() ;

	::SetWindowPos(m_hHScrollWnd,HWND_TOPMOST,hBar.nLeft,hBar.nTop-hBar.GetHeight(),hBar.GetWidth(),hBar.GetHeight(),SWP_NOZORDER) ;
	m_SkinHScrollbar.Attach(m_hHScrollWnd) ;

	m_SkinHScrollbar.SetSkinImage(m_HScrollbarImages) ;
	
//	m_SkinHScrollbar.SetWindowPos(NULL,vBar.left,vBar.top,vBar.Width(),vBar.Height(),SWP_NOZORDER);
//	m_SkinHorizontalScrollbar.SetWindowPos(NULL,hBar.left,hBar.top,hBar.Width(),hBar.Height(),SWP_NOZORDER);
}

int CptListView::InsertColumn(int nIndex,const LVCOLUMN& Col)
{
	if(m_hWnd!=NULL)
	{//ListView_InsertColumn
		return (int)::SendMessage(m_hWnd, LVM_INSERTCOLUMN, (WPARAM)(int)(nIndex), (LPARAM)(const LV_COLUMN *)(&Col)) ;
	}
	

	return -1 ;
}

void CptListView::Clear() 
{
}

int CptListView::InsertItem(const LVITEM& lvItem) 
{
	if(m_hWnd!=NULL)
	{
		return (int)::SendMessage(m_hWnd,LVM_INSERTITEM, 0, (LPARAM)(const LVITEM *)(&lvItem));
	}

	return -1 ;
}

int CptListView::InsertItem(int nItemIndex,LPTSTR lpszItem ) 
{
	int nRet = -1 ;

	if(m_hWnd!=NULL)
	{
		LVITEM item ;
		::memset(&item,0,sizeof(item)) ;

		item.mask = LVIF_TEXT | LVIF_IMAGE ;
		item.iItem = nItemIndex ;
		item.iSubItem = 0 ;
		item.cchTextMax = MAX_PATH; 
		item.pszText = lpszItem ;
		item.iImage = -1 ;

		//nRet = ListView_InsertItem(m_hWnd,&item) ;
		nRet = (int)::SendMessage(m_hWnd,LVM_INSERTITEM, 0, (LPARAM)(const LVITEM *)(&item));
	}

	return nRet ;
}

int CptListView::InsertItem(int nItemIndex,LPTSTR lpszItem,int nImage)
{
	return -1 ;
}

//void CptListView::OnPaint() 
//{
//	HDC hDevDC = ::GetDC(m_hWnd) ;
//
//
//	::ReleaseDC(m_hWnd,hDevDC) ;
//}

HIMAGELIST CptListView::SetImageList(HIMAGELIST hImageList,int nType)
{
	// nType: LVSIL_SMALL, LVSIL_NORMAL ,LVSIL_STATE 
	// 必须设置 LVS_EX_SUBITEMIMAGES 属性

	HIMAGELIST nRet = NULL ;

	//ListView_SetImageList
	if(m_hWnd!=NULL)
	{
		nRet = (HIMAGELIST)::SendMessage(m_hWnd,LVM_SETIMAGELIST,nType,(LPARAM)(HIMAGELIST)(hImageList)) ;
	}

	return nRet ;
}

HIMAGELIST CptListView::GetImageList(int nType) const
{
	// nType: LVSIL_SMALL, LVSIL_NORMAL ,LVSIL_STATE 
	// 必须设置 LVS_EX_SUBITEMIMAGES 属性

	HIMAGELIST nRet = NULL ;

	//ListView_SetImageList
	if(m_hWnd!=NULL)
	{
		nRet = ListView_GetImageList(m_hWnd,nType) ;
	}

	return nRet ;
}

//int CptListView::GetSelectedColumn() 
//{
//	int nRet = -1 ;
//
//	if(m_hWnd!=NULL)
//	{
//		nRet = ::SendMessage(m_hWnd,LVM_GETSELECTEDCOLUMN,0,0) ;
//		//nRet = (int)ListView_GetSelectedColumn(m_hWnd) ;
//	}
//
//	return nRet ;
//} ListView_GetTopIndex

int CptListView::GetTopIndex() const
{
	int nRet = -1 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)ListView_GetTopIndex(m_hWnd) ;
	}

	return nRet ;
}

int CptListView::GetSelectedCount() const
{
	int nRet = -1 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)ListView_GetSelectedCount(m_hWnd) ;
	}

	return nRet ;
}

int CptListView::GetStringWidth(LPCTSTR lpszStr) const
{
	int nRet = -1 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)ListView_GetStringWidth(m_hWnd,lpszStr) ;
	}

	return nRet ;
}

int CptListView::GetHotItem() const 
{
	int nRet = -1 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)ListView_GetHotItem(m_hWnd) ;
	}

	return nRet ;
}

void CptListView::GetSelectIndexs(std::vector<int>& SelectIndexVer) 
{
	if(m_hWnd!=NULL)
	{
		SelectIndexVer.clear() ;
		int nItemCount = this->GetItemCount() ;

		const bool bIsSingleSel = (::GetWindowLong(m_hWnd,GWL_STYLE)& LVS_SINGLESEL) ? true : false ;

		for(int i=0;i<nItemCount;++i)
		{
			if(ListView_GetItemState(m_hWnd,i,LVIS_SELECTED) == LVIS_SELECTED)  
			{
				SelectIndexVer.push_back(i) ;

				if(bIsSingleSel)
				{
					break ;
				}
			}
		}
	}
}

void CptListView::SetSelectIndexs(const std::vector<int>& SelectIndexVer)
{
	if(m_hWnd!=NULL)
	{
		int nItemCount = this->GetItemCount() ;

		const bool bIsSingleSel = (::GetWindowLong(m_hWnd,GWL_STYLE)& LVS_SINGLESEL) ? true : false ;

		for(size_t i=0;i<SelectIndexVer.size();++i)
		{
			if(SelectIndexVer[i]<nItemCount) 
			{
				ListView_SetItemState(m_hWnd,SelectIndexVer[i],LVIS_SELECTED,LVIS_SELECTED) ;

				if(bIsSingleSel)
				{
					break ;
				}
			}
		}
	}
}

//int CptListView::GetSelectIndex() 
//{
//	return 0 ;
//}
//
//bool CptListView::SetSelectIndex(int nIndex) 
//{
//	return 0 ;
//}

int CptListView::GetItemCount() const
{
	int nRet = 0 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)::SendMessage(m_hWnd,LVM_GETITEMCOUNT,0,0) ;
	}

	return nRet ;
}

int CptListView::GetScrollPos(int nBar) const
{
	int nRet = 0 ;

	if(m_hWnd!=NULL)
	{
		nRet = ::GetScrollPos(m_hWnd,nBar) ;
	}

	return nRet ;
}

bool CptListView::GetScrollRange(int nBar,int& Max,int& Min) const
{
	bool bRet = false ;

	if(m_hWnd!=NULL)
	{
		bRet = ::GetScrollRange(m_hWnd,nBar,&Max,&Min) ? true : false ;
	}

	return bRet ;
}

bool CptListView::SetItem(const LVITEM& item) 
{
	bool nRet = false ;

	if(m_hWnd!=NULL)
	{
		nRet = ::SendMessage(m_hWnd,LVM_SETITEM,0,(LPARAM)(LVITEM *)&item) ? true : false ;
	}

	return nRet ;
}

bool CptListView::GetItemText(int nItemIndex,int nSubItemIndex,LPTSTR lpszText,int nBufSize) const
{
	bool nRet = false ;

	if(m_hWnd!=NULL)
	{
		LVITEM item ;
		::memset(&item,0,sizeof(item)) ;

		item.iSubItem = nSubItemIndex ;
		item.pszText = lpszText ;
		item.cchTextMax = nBufSize ;

		nRet = ::SendMessage(m_hWnd,LVM_GETITEMTEXT,nItemIndex,(LPARAM)(LVITEM *)&item) ? true : false ;
	}

	return nRet ;
}

bool CptListView::SetItemText(int nItemIndex,int nSubItemIndex,LPTSTR lpszText)
{
	
	bool nRet = false ;

	if(m_hWnd!=NULL)
	{
		LVITEM item ;
		::memset(&item,0,sizeof(item)) ;

		item.iSubItem = nSubItemIndex ;
		item.pszText = lpszText ;

		nRet = ::SendMessage(m_hWnd,LVM_SETITEMTEXT,nItemIndex,(LPARAM)(LVITEM *)&item) ? true : false ;
	}

	return nRet ;
}

int CptListView::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) 
{
//	switch(nMsg)
//	{
//		case WM_DROPFILES:
//		{
//			HDROP hDropInfo = (HDROP)wParam ;
//
//			const int fileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
//
//			TCHAR szFileName[MAX_PATH] = { 0 };
//			TCHAR szListString[MAX_PATH] = { 0 };
//
//			//if(hWnd==this->GetDlgItem(IDC_LIST_SOURCEFILE))
//			{
//				for (int i = 0; i < fileCount; ++i)
//				{
//					::DragQueryFile(hDropInfo, i, szFileName, sizeof(szFileName));
//
//					const int nListCount = this->GetItemCount() ;
//
//					int j = 0 ;
//					for(j=0;j<nListCount;++j)
//					{
//						szListString[0] = 0 ;
////					this->GetString(j,szListString) ;
//
//						CptString str1 = szListString ;
//						CptString str2 = szFileName ;
//
//						if(str1.CompareNoCase(str2)==0)
//						{
//							break ;
//						}
//					}
//
//					if(j==nListCount)
//					{
//						//this->AddString(szFileName) ;
//					}
//				}
//			}
//			
//			::DragFinish(hDropInfo) ;
//		}
//
//		break ;
//	}

	//switch(nMsg)
	//{
	//case WM_PAINT:
	//	//::SendMessage(m_SkinHScrollbar.m_hWnd,WM_PAINT,0,0) ;
	//	break ;
	//}

	return CptCommCtrl::PreProcCtrlMsg(hWnd,nMsg,wParam, lParam) ;
}

int CptListView::PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) 
{
	//if(nMsg==WM_NOTIFY)
	//{
	//	LPNMHDR lpnmhdr = (LPNMHDR) lParam;

	//	if(lpnmhdr->hwndFrom==m_hWnd && lpnmhdr->code==NM_CUSTOMDRAW)
	//	{
	//		NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( lpnmhdr );

	//		pLVCD->clrText = RGB(255,0,0) ;
	//		int fff = 0 ;

	//		bContinue = false ;
	//		return CDRF_DODEFAULT ;
	//	}
	//	//int aa = 0 ;
	//}

	if(nMsg==WM_DRAWITEM)
	{
		DRAWITEMSTRUCT* pIS = (LPDRAWITEMSTRUCT) lParam ;

		if(pIS!=NULL && pIS->hwndItem==m_hWnd && pIS->CtlType==ODT_LISTVIEW)
		{
			TCHAR* p = (TCHAR*)pIS->itemData ;
			TCHAR szBuf[200] = {0} ;

			::GetWindowText(pIS->hwndItem,szBuf,100) ;

			if(pIS->itemAction&ODA_SELECT)
			{
			}
			else if(pIS->itemAction&ODA_DRAWENTIRE)
			{
				//pIS->itemData
			}

			return TRUE ;
			//if(pIS->itemAction&ODA_SELECT || pIS->itemState==ODS_SELECTED || pIS->itemState==ODS_DEFAULT )
			//{
			//	//HBITMAP hBitmap = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_COLUMNTITLE) ;
			//	//HBRUSH brNew = ::CreatePatternBrush(hBitmap) ;
			//	////BRUSH brOld = (BRUSH)::SelectObject(pIS->hDC,brNew) ;

			//	//::FillRect(pIS->hDC,&pIS->rcItem,brNew) ;

			//	//::DeleteObject(brNew) ;
			//	//::DeleteObject(hBitmap) ;
			//	//pIS->
			//	return TRUE ;
			//}
		}
	}

	return CptCommCtrl::PreProcParentMsg(hWnd,nMsg,wParam, lParam,bContinue) ;
}


/**
BOOL CptListView::ProcMessage(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam)
{
	switch(nMsg)
	{
		case WM_DROPFILES:
		{
			HDROP hDropInfo = (HDROP)wParam ;

			const int fileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

			TCHAR szFileName[MAX_PATH] = { 0 };
			TCHAR szListString[MAX_PATH] = { 0 };

			//if(hWnd==this->GetDlgItem(IDC_LIST_SOURCEFILE))
			{
				for (int i = 0; i < fileCount; ++i)
				{
					::DragQueryFile(hDropInfo, i, szFileName, sizeof(szFileName));

					const int nListCount = this->GetItemCount() ;

					int j = 0 ;
					for(j=0;j<nListCount;++j)
					{
						szListString[0] = 0 ;
//					this->GetString(j,szListString) ;

						CptString str1 = szListString ;
						CptString str2 = szFileName ;

						if(str1.CompareNoCase(str2)==0)
						{
							break ;
						}
					}

					if(j==nListCount)
					{
						//this->AddString(szFileName) ;
					}
				}
			}
			
			::DragFinish(hDropInfo) ;
		}

		break ;
	}
	return ::CallWindowProc(m_pItemDefWndProc,hWnd,nMsg,wParam,lParam) ;
}

BOOL CptListView::MsgProc(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam)
{
	CptListView* pThis = (CptListView*)::GetProp(hWnd,s_ptListViewThisEntry) ;

	if(pThis!=NULL)
	{
		return pThis->ProcMessage(hWnd,nMsg,wParam,lParam) ;
	}

	return FALSE ;

//	return ::DefWindowProc(hWnd,nMsg,wParam,lParam) ;
}

BOOL CptListView::ParentProcMessage(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) 
{
	//if(nMsg==WM_NOTIFY)
	//{
	//	LPNMHDR lpnmhdr = (LPNMHDR) lParam;

	//	if(lpnmhdr->hwndFrom==m_hWnd && lpnmhdr->code==NM_CUSTOMDRAW)
	//	{
	//		NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( lpnmhdr );

	//		pLVCD->clrText = RGB(255,0,0) ;
	//		int fff = 0 ;

	//		return CDRF_DODEFAULT ;
	//	}
	//	int aa = 0 ;
	//}


	
	//if(nMsg==WM_DRAWITEM)
	//{
	//	DRAWITEMSTRUCT* pIS = (LPDRAWITEMSTRUCT) lParam ;

	//	if(pIS!=NULL && pIS->hwndItem==m_hWnd && pIS->CtlType==ODT_LISTVIEW)
	//	{

	//		if(pIS->itemAction&ODA_SELECT || pIS->itemState==ODS_SELECTED || pIS->itemState==ODS_DEFAULT )
	//		{
	//			HBITMAP hBitmap = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_COLUMNTITLE) ;
	//			HBRUSH brNew = ::CreatePatternBrush(hBitmap) ;
	//			//BRUSH brOld = (BRUSH)::SelectObject(pIS->hDC,brNew) ;

	//			::FillRect(pIS->hDC,&pIS->rcItem,brNew) ;

	//			::DeleteObject(brNew) ;
	//			::DeleteObject(hBitmap) ;
	//			//pIS->
	//			return TRUE ;
	//		}
	//		//{
	//		//	m_CurState = BS_Down ;
	//		//	this->Paint() ;
	//		//	m_CurState = BS_Hover ;
	//		//	this->Paint() ;
	//		//}
	//		////else if(pIS->itemAction & ODA_FOCUS)
	//		////{
	//		////	m_CurState = BS_Hover ;
	//		////	this->Paint() ;
	//		////}
	//		//else
	//		//{
	//		//	//m_CurState = BS_Normal ;
	//		//	this->Paint() ;
	//		//}
	//		
	//	}
	//}


	int nRet = 0 ;

	if(!(m_pParentDefWndProc==(WNDPROC)ParentMsgProc))
	{
		nRet = ::CallWindowProc(m_pParentDefWndProc,hWnd,nMsg,wParam,lParam) ;
	}

	return nRet ;
}



BOOL CptListView::ParentMsgProc(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE ;
	SThisNode* pNode = (SThisNode*)::GetProp(hWnd,s_ptListViewParentWndProc) ;
//	CptListView* pThis = (CptListView*)::GetProp(hWnd,s_ptListBoxParentWndProc) ;

	while(pNode!=NULL && pNode->pThis!=NULL)
	{
		//SThisNode* pNext = NULL ;
		bRet = pNode->pThis->ParentProcMessage(hWnd,nMsg,wParam,lParam) ;
		pNode = pNode->pNext ;
	}

	return bRet ;
	//return ::DefWindowProc(hWnd,nMsg,wParam,lParam) ;
}
/**/