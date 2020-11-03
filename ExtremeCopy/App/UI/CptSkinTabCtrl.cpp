/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "CptSkinTabCtrl.h"
#include "..\XCConfiguration.h"

CptSkinTabCtrl::CptSkinTabCtrl(void)
{
}

CptSkinTabCtrl::~CptSkinTabCtrl(void)
{
}

void CptSkinTabCtrl::Attach(HWND hwnd)
{
	CptCommCtrl::Attach(hwnd) ;

	int nStyleEx = ::GetWindowLong(m_hWnd,GWL_STYLE) ;

	nStyleEx |= TCS_OWNERDRAWFIXED ;

	int kk = ::SetWindowLong(m_hWnd,GWL_STYLE,nStyleEx) ;
}

int CptSkinTabCtrl::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) 
{
	//switch(nMsg)
	//{
	////	case WM_CTLCOLORBTN :
	////case WM_CTLCOLOREDIT :
	////case WM_CTLCOLORDLG :
	////case WM_CTLCOLORSTATIC :
	////case WM_CTLCOLORSCROLLBAR :
	////case WM_CTLCOLORLISTBOX :
	////case WM_CTLCOLORMSGBOX:
	////	{
	////		/**/
	////		HDC hdc = (HDC)wParam ;

	////		::SetBkMode(hdc,TRANSPARENT) ;
	////		
	////		//HBRUSH hBrush = ::CreateSolidBrush(RGB(113,204,255)) ;
	////		//HBRUSH hBrush = ::CreateSolidBrush(RGB(181,211,255)) ;

	////		//if((HWND)lParam==this->GetDlgItem(IDC_STATIC_CURRENTFILE))
	////		//{// 当前复制的文件
	////		//	//HFONT hFont = ::CreateFont(10,0,0,0,FW_BOLD,TRUE,FALSE,FALSE,DEFAULT_CHARSET,0,
	////		//	//	CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL) ;
	////		//	::SelectObject(hdc,m_hCurFileFont) ;
	////		//}
	////		//else if((HWND)lParam==this->GetDlgItem(IDC_STATIC_TO) || (HWND)lParam==this->GetDlgItem(IDC_STATIC_FROM))
	////		//{
	////		//	::SelectObject(hdc,m_hToTextFont) ;
	////		//}
	////		//else if(uMsg==WM_CTLCOLORSTATIC)
	////		//{
	////		//	::SetTextColor(hdc,UI_TEXT_COLOR) ;
	////		//}

	////		return (int)CXCConfiguration::GetInstance()->GetGlobalData()->hDlgBkBrush ;
	////		/**/
	////	}
	////	break ;

	////case WM_ERASEBKGND :
	////	{
	////		HDC hDC = (HDC)wParam ;
	////		SptRect rect ;

	////		::GetClientRect(m_hWnd,rect.GetRECTPointer()) ;

	////		::FillRect(hDC,rect.GetRECTPointer(),::CXCConfiguration::GetInstance()->GetGlobalData()->hDlgBkBrush) ;
	////		
	////	}
	////	
	////	return TRUE ;
	//}
	return CptCommCtrl::PreProcCtrlMsg(hWnd,nMsg,wParam,lParam) ;
}

int CptSkinTabCtrl::PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) 
{
	if(nMsg==WM_DRAWITEM)
	{
		DRAWITEMSTRUCT* pIS = (LPDRAWITEMSTRUCT) lParam ;

		if(pIS!=NULL && pIS->hwndItem==m_hWnd && pIS->CtlType==ODT_TAB)
		{
			TCHAR szText[100] = {0} ;
			TCITEM item ;
			::memset(&item,0,sizeof(item)) ;

			item.mask =  TCIF_TEXT;
			item.pszText = szText ;
			item.cchTextMax = sizeof(szText)/sizeof(TCHAR) ;

			this->GetItem(pIS->itemID,item) ;

//			::FillRect(pIS->hDC,&pIS->rcItem,::CXCConfiguration::GetInstance()->GetGlobalData()->hDlgBkBrush) ;

			int OldMode = ::SetBkMode(pIS->hDC,TRANSPARENT) ;
			::TextOut(pIS->hDC,pIS->rcItem.left+5,pIS->rcItem.top+5,szText,(int)::_tcslen(szText)) ;

			::SetBkMode(pIS->hDC,OldMode) ;
			bContinue = false ;

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

	return CptCommCtrl::PreProcParentMsg(hWnd,nMsg,wParam,lParam,bContinue) ;
}