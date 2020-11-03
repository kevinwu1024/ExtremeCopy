/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptSkinTrackBar.h"
#include "commctrl.h"
#include "..\..\Common\ptWinGraphic.h"
#include "..\XCGlobal.h"


CptSkinTrackBar::CptSkinTrackBar(void)
{
	m_hChannelImage = NULL ;
	m_hThumbImage = NULL ;
}//TBM_SETRANGE

CptSkinTrackBar::~CptSkinTrackBar(void)
{
	if(m_hChannelImage!=NULL)
	{
		SAFE_DELETE_GDI(m_hChannelImage) ;
	}

	if(m_hThumbImage!=NULL)
	{
		SAFE_DELETE_GDI(m_hThumbImage) ;
	}
}

void CptSkinTrackBar::Attach(HWND hwnd)
{
	CptCommCtrl::Attach(hwnd) ;

	if(m_hWnd!=NULL)
	{
		m_hChannelImage = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_TRACKBAR2) ;

		int nWidth = 0 ;
		int nHeight = 0 ;

		if(CptWinGraphic::GetBitmapSize(m_hChannelImage,nWidth,nHeight))
		{
			m_ChannelImageSize.nWidth = nWidth ;
			m_ChannelImageSize.nHeight = nHeight ;
		}

		m_hThumbImage = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_TRACKTHUMB2) ;

		if(CptWinGraphic::GetBitmapSize(m_hThumbImage,nWidth,nHeight))
		{
			m_ThumbImageSize.nWidth = nWidth ;
			m_ThumbImageSize.nHeight = nHeight ;
		}
	}
}
int CptSkinTrackBar::GetRangeMin() const
{
	int nRet = 0 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)::SendMessage(m_hWnd,TBM_GETRANGEMIN,NULL,NULL) ;
	}

	return nRet ;
}

int CptSkinTrackBar::GetRangeMax() const
{
	int nRet = 0 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)::SendMessage(m_hWnd,TBM_GETRANGEMAX,NULL,NULL) ;
	}

	return nRet ;
}

void CptSkinTrackBar::GetRange(int& nMin,int& nMax) const
{
	nMin = this->GetRangeMin() ;
	nMax = this->GetRangeMax() ;
}

void CptSkinTrackBar::SetRange(int nMin,int nMax,BOOL bRedraw)
{
	if(m_hWnd!=NULL)
	{
		::SendMessage(m_hWnd,TBM_SETRANGE,(WPARAM)bRedraw,(LPARAM) MAKELONG (nMin, nMax)) ;
	}
}

UINT CptSkinTrackBar::GetNumTics() const
{
	UINT nRet = 0 ;

	if(m_hWnd!=NULL)
	{
		nRet = (UINT)::SendMessage(m_hWnd,TBM_GETNUMTICS,NULL,NULL) ;
	}

	return nRet ;
}

void CptSkinTrackBar::SetTicFreq(int nFreq )
{
	if(m_hWnd!=NULL)
	{
		::SendMessage(m_hWnd,TBM_SETTICFREQ,(WPARAM) (WORD) nFreq,NULL) ;
	}
}

int CptSkinTrackBar::GetTicPos(int nTic ) const
{
	UINT nRet = 0 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)::SendMessage(m_hWnd,TBM_GETTICPOS,(WPARAM) (WORD) nTic,NULL) ;
	}

	return nRet ;
}

int CptSkinTrackBar::GetPos() const
{
	int nRet = 0 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)::SendMessage(m_hWnd,TBM_GETPOS,NULL,NULL) ;
	}

	return nRet ;
}

void CptSkinTrackBar::SetPos(int nPos)
{
	if(m_hWnd!=NULL)
	{
		::SendMessage(m_hWnd,TBM_SETPOS,(WPARAM)TRUE,(LPARAM) (LONG)nPos) ;
	}
}

int CptSkinTrackBar::OnNMCustomdraw(NMHDR* pNMHDR,bool& bContinue)
{
	bContinue = true ;
	return 0 ;

	int nRet = 0 ;

	LPNMCUSTOMDRAW lpNMCustomDraw = (LPNMCUSTOMDRAW)(pNMHDR);

	bContinue = true ;

	switch( lpNMCustomDraw->dwDrawStage ) 
	{
	case CDDS_PREPAINT:
		nRet = CDRF_NOTIFYITEMDRAW;
		bContinue = false ;
		break;

	case CDDS_ITEMPREPAINT:  // Before an item is drawn. This is where we perform our item-specific custom drawing
		if( lpNMCustomDraw->dwItemSpec == TBCD_THUMB )
		{
			this->DrawThumb(lpNMCustomDraw->hdc,lpNMCustomDraw->rc,lpNMCustomDraw->uItemState) ;
			nRet = CDRF_SKIPDEFAULT ;
			bContinue = false ;
			//if( OnSliderDrawThumb( dcPaint, rcPaint,lpNMCustomDraw->uItemState) )
			//	*pResult = CDRF_SKIPDEFAULT;
		} 
		else if( lpNMCustomDraw->dwItemSpec == TBCD_CHANNEL )
		{
			this->DrawChannel(lpNMCustomDraw->hdc,lpNMCustomDraw->rc,lpNMCustomDraw->uItemState) ;
			nRet = CDRF_SKIPDEFAULT ;
			bContinue = false ;
			//CRect rcClient;
			//GetClientRect( &rcClient);

			//if( OnSliderDrawChannel( dcPaint,rcClient,lpNMCustomDraw->uItemState))
			//	*pResult = CDRF_SKIPDEFAULT;
		}
		else if( lpNMCustomDraw->dwItemSpec == TBCD_TICS )
		{
			nRet = CDRF_SKIPDEFAULT ;
			//bContinue = false ;
			//CRect rcClient;
		 //   GetClientRect( &rcClient);
			//if( OnSliderDrawTics( dcPaint,rcClient, lpNMCustomDraw->uItemState))
			//	*pResult = CDRF_SKIPDEFAULT;
		}
		else
		{
			_ASSERT( FALSE );
		}

		break;
	} 

	return nRet ; // 返回 1 即妨止继续由默认消息处理函数处理
}

BOOL CptSkinTrackBar::OnEraseBkgnd(HDC hDC)
{
	return TRUE;
}

void CptSkinTrackBar::DrawChannel(HDC hdc,SptRect rt,UINT uState) 
{
	HDC hMemDC = ::CreateCompatibleDC(hdc) ;
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC,m_hChannelImage) ;
	
	const int nWidthOffset = 15 ;

	CptWinGraphic::TransparentBlt2(hdc,rt.nLeft,rt.nTop-nWidthOffset/2,rt.GetWidth(),rt.GetHeight()+nWidthOffset,
		hMemDC,0,0,m_ChannelImageSize.nWidth,m_ChannelImageSize.nHeight,RGB(181,211,255)) ;

	//CptWinGraphic::TransparentBlt2(hdc,rt.nLeft-nWidthOffset/2,rt.nTop,rt.GetWidth()+nWidthOffset,rt.GetHeight(),
	//	hMemDC,0,0,m_ChannelImageSize.nWidth,m_ChannelImageSize.nHeight,RGB(255,0,255)) ;

	::SelectObject(hMemDC,hOldBitmap) ;
	::DeleteDC(hMemDC) ;
	
}

void CptSkinTrackBar::DrawThumb(HDC hdc,SptRect rt,UINT uState) 
{
	HDC hMemDC = ::CreateCompatibleDC(hdc) ;
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC,m_hThumbImage) ;

	if ( uState == CDIS_SELECTED)
	{
	}
	else if ( uState == CDIS_HOT)
	{
	}
	else 
	{
	}

	//::BitBlt(hdc,rt.nLeft,rt.nTop,rt.GetWidth(),rt.GetHeight(),hMemDC,0,0,SRCCOPY) ;
	
	CptWinGraphic::TransparentBlt2(hdc,rt.nLeft,rt.nTop,rt.GetWidth()-2,rt.GetHeight(),
		hMemDC,0,0,m_ThumbImageSize.nWidth,m_ThumbImageSize.nHeight,RGB(15,153,233)) ;

	::SelectObject(hMemDC,hOldBitmap) ;
	::DeleteDC(hMemDC) ;
}

void CptSkinTrackBar::DrawTick(HDC hdc,SptRect rt,UINT uState)
{
}

//int CptSkinTrackBar::PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) 
//{
//
//
//	if(nMsg==WM_DRAWITEM)
//	{
//		int aa = 0 ;
//		////Debug_Printf(_T("CptSkinButton::PreProcParentMsg() 1")) ;
//		//DRAWITEMSTRUCT* pIS = (LPDRAWITEMSTRUCT) lParam ;
//
//		//if(pIS!=NULL && pIS->hwndItem==m_hWnd && pIS->CtlType==ODT_BUTTON)
//		//{
//		//	if(pIS->itemAction&ODA_SELECT || pIS->itemState==ODS_SELECTED)
//		//	{
//		//		m_CurState = ControlStatus_Down ;
//		//		this->Paint() ;
//		//		m_CurState = ControlStatus_Hover ;
//		//		this->Paint() ;
//		//	}
//		//	//else if(pIS->itemAction & ODA_FOCUS)
//		//	//{
//		//	//	m_CurState = BS_Hover ;
//		//	//	this->Paint() ;
//		//	//}
//		//	else
//		//	{
//		//		//m_CurState = BS_Normal ;
//		//		this->Paint() ;
//		//	}
//		//	//bContinue = false ;
//		//	//return TRUE ;
//		//}
//
//		return TRUE ;
//	}
//
//	return CptCommCtrl::PreProcParentMsg(hWnd,nMsg,wParam,lParam,bContinue) ;
//}