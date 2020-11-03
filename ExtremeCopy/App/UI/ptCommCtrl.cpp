/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptCommCtrl.h"
#include <commctrl.h>

static const TCHAR* s_ptCommCtrlThisEntry = _T("ptCommCtrlThisEntry") ;

pt_STL_map(HWND,CptCommCtrl::SParentComCtrlInfo)	CptCommCtrl::m_ParentComCtrlMap ;
CptCritiSecLock	CptCommCtrl::m_MapLock ;

CptCommCtrl::CptCommCtrl(void):m_hWnd(NULL)//,m_pNextNode(NULL)
{
}

CptCommCtrl::~CptCommCtrl(void)
{
	if(m_hWnd!=NULL)
	{
#ifdef _WIN64
		SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_pOldCtrlDefWndProc);
#else
		::SetWindowLong(m_hWnd,GWLP_WNDPROC,(LONG)m_pOldCtrlDefWndProc) ; // 恢复旧的控件消息处理函数入口
#endif
		

		this->DisconnectParentWndProc(m_hWnd) ;
	}

	//this->ReleasePointerForParentWnd() ;
}

bool CptCommCtrl::AddStyle(const LONG dwNewStyle) 
{
	bool bRet = false ;

	if(NULL!=m_hWnd)
	{
		//::SetWindowLong(m_hWnd, GWL_STYLE, ::GetWindowLong(m_hWnd, GWL_STYLE));
		LONG OldStyle = ::GetWindowLong(m_hWnd, GWL_STYLE); 

		LONG NewStyle = OldStyle | dwNewStyle;

		bRet = (::SetWindowLong(m_hWnd, GWL_STYLE, NewStyle)==OldStyle);

		LONG ddd = ::GetWindowLong(m_hWnd, GWL_STYLE); 

		int aaa = 0 ;

	}
	
	return bRet ;
}

bool CptCommCtrl::RemoveStyle(const DWORD dwNewStyle) 
{
	bool bRet = false ;

	if(NULL!=m_hWnd)
	{
		LONG OldStyle = ::GetWindowLong(m_hWnd, GWL_STYLE); 

		LONG NewStyle = OldStyle & (~dwNewStyle);

		bRet = (::SetWindowLong(m_hWnd, GWL_STYLE, NewStyle)==OldStyle);

	}

	return bRet ;
}

DWORD CptCommCtrl::GetStyle() 
{
	DWORD dwRet = 0 ;

	if(NULL!=m_hWnd)
	{
		dwRet = ::GetWindowLong(m_hWnd, GWL_STYLE); 
	}

	return dwRet;
}

void CptCommCtrl::DisconnectParentWndProc(HWND hComCtrlWnd)
{
	CptAutoLock lock(&m_MapLock) ;

	HWND hParent = ::GetParent(hComCtrlWnd) ;

	pt_STL_map(HWND,SParentComCtrlInfo)::iterator it = m_ParentComCtrlMap.find(hParent) ;

	if(it!=m_ParentComCtrlMap.end())
	{// 该父窗口已存在

		//(*it).second.ComCtrlList.erase(this) ;

		pt_STL_list(CptCommCtrl*)::iterator it2 = (*it).second.ComCtrlList.begin() ;

		for(;it2!=(*it).second.ComCtrlList.end();++it2)
		{
			if((*it2)==this)
			{
				(*it).second.ComCtrlList.erase(it2) ;

				if((*it).second.ComCtrlList.empty())
				{
					m_ParentComCtrlMap.erase(hParent) ;
				}
				break ;
			}
		}
		//(*it).second.ComCtrlList.push_back(this) ;
	}
}

void CptCommCtrl::ConnectParentWndProc(HWND hComCtrlWnd)
{
	CptAutoLock lock(&m_MapLock) ;

	HWND hParent = ::GetParent(hComCtrlWnd) ;

	pt_STL_map(HWND,SParentComCtrlInfo)::iterator it = m_ParentComCtrlMap.find(hParent) ;

	if(it!=m_ParentComCtrlMap.end())
	{// 该父窗口已存在
		//Debug_Printf(_T("CptCommCtrl::ConnectParentWndProc() 该父窗口 已 存在 %p"),hParent) ;

		(*it).second.ComCtrlList.push_back(this) ;
	}
	else
	{
		//Debug_Printf(_T("CptCommCtrl::ConnectParentWndProc() 该父窗口 不 存在 %p"),hParent) ;

		SParentComCtrlInfo pcc ;

#ifdef _WIN64
		pcc.OldWndProc = (WNDPROC)SetWindowLongPtr(hParent, GWLP_WNDPROC, (LONG_PTR)ProcParentMsg);
#else
		pcc.OldWndProc = (WNDPROC)::SetWindowLong(hParent,GWLP_WNDPROC,(LONG)ProcParentMsg) ;
#endif

		//
		pcc.ComCtrlList.push_back(this) ;

		m_ParentComCtrlMap[hParent] = pcc ;
	}
}

void CptCommCtrl::UpdateStatus(SControlStatus NewStatus) 
{
	if(m_hWnd!=NULL)
	{
		HWND hParentWnd = ::GetParent(m_hWnd) ;

		if(hParentWnd!=NULL)
		{
			int nID = ::GetDlgCtrlID(m_hWnd) ;
			::PostMessage(hParentWnd,WM_CONTROLSTATUSCHANGED,(WPARAM)nID,(LPARAM)NewStatus.uStatusBlock) ;
		}
	}
}

bool CptCommCtrl::Attach(HWND hWnd)
{
	if(hWnd!=NULL)
	{
		m_hWnd = hWnd ;

		// 指向当前新的控件消息处理函数入口
#ifdef _WIN64
		m_pOldCtrlDefWndProc = (WNDPROC)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)ProcCtrlMsg);
#else
		m_pOldCtrlDefWndProc = (WNDPROC)::SetWindowLong(m_hWnd,GWLP_WNDPROC,(LONG)ProcCtrlMsg) ;
#endif

		::SetProp(m_hWnd,s_ptCommCtrlThisEntry,(HANDLE)this) ;

		// 把父窗口消息处理串接起来
		this->ConnectParentWndProc(hWnd) ;

		/**
		HWND hParent = ::GetParent(m_hWnd) ;

		this->ReleasePointerForParentWnd() ;

		SThisNode* pOldNode = (SThisNode*)::GetProp(hParent,s_ptCommCtrlParentWndProc) ;

		SThisNode* pNode = new SThisNode() ;

		pNode->pThis = this ;
		pNode->pNext = pOldNode;//(SThisNode*)::GetProp(hParent,s_ptCommCtrlParentWndProc) ;

		::SetProp(hParent,s_ptCommCtrlParentWndProc,(HANDLE)pNode) ;

		if(pOldNode==NULL)
		{
			m_pParentDefWndProc = (WNDPROC)::SetWindowLong(hParent,GWLP_WNDPROC,(LONG)ProcParentMsg) ;
		}
		/**/
	}
	
	return true ;
}

void CptCommCtrl::OnLButtonDown(int nFlag,const SptPoint& pt) 
{
}

void CptCommCtrl::OnLButtonUp(int nFlag,const SptPoint& pt) 
{
}

void CptCommCtrl::OnRButtonDown(int nFlag,const SptPoint& pt) 
{
}

void CptCommCtrl::OnRButtonUp(int nFlag,const SptPoint& pt) 
{
}

void CptCommCtrl::OnMouseMove(int nFlag,const SptPoint& pt) 
{
}

void CptCommCtrl::OnMouseEnter() 
{
}

void CptCommCtrl::OnMuseLeave() 
{
}

int CptCommCtrl::SendMessage(int nMsg,WPARAM wParam,LPARAM lParam)
{
	return (int)::SendMessage(m_hWnd,nMsg,wParam,lParam) ;
}

bool CptCommCtrl::SetWindowText(CptString strTxt) 
{
	if(m_hWnd!=NULL)
	{
		return ::SetWindowText(m_hWnd,strTxt.c_str()) ? true : false ;
	}

	return false ;
}

CptString CptCommCtrl::GetWindowText() 
{
	CptString strRet ;

	if(m_hWnd!=NULL)
	{
		int nLen = ::GetWindowTextLength(m_hWnd) ;

		if(nLen>0)
		{
			TCHAR* pBuf = new TCHAR[nLen+1] ;

			if(pBuf!=NULL)
			{
				::GetWindowText(m_hWnd,pBuf,nLen+1) ;
				strRet = pBuf ;

				delete [] pBuf ;
			}
		}
	}

	return strRet ;
}

int CptCommCtrl::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) 
{
	//MessageBox(NULL,_T("CptCommCtrl PreProcCtrlMsg"),_T(""),0) ;

	int nRet = (int)::CallWindowProc(m_pOldCtrlDefWndProc,hWnd,nMsg,wParam,lParam) ;

	switch(nMsg)
	{
	case WM_DROPFILES:
		{
			HDROP hDropInfo = (HDROP)wParam ;

			this->OnDropFiles(hDropInfo) ;
		}
		return 0 ;

	case WM_LBUTTONDOWN:
		{
			//MessageBox(NULL,_T("CptCommCtrl WM_LBUTTONDOWN"),_T(""),0) ;

			m_CurState.status.MouseStatus = ControlMouseStatus_Down ;
			this->UpdateStatus(m_CurState) ;

			SptPoint pt ;

			pt.nX = lParam&0xffff ;
			pt.nY = (LONG)(lParam >> 16) ;

			this->OnLButtonDown((int)wParam,pt) ;
		}
		break ;

	case WM_LBUTTONUP:
		{
			//Debug_Printf(_T("CptCommCtrl::PreProcCtrlMsg() hwnd=%p old_wnd=%p"),::GetParent(hWnd),m_pOldCtrlDefWndProc) ;
			m_CurState.status.MouseStatus = ControlMouseStatus_Hover ;
			this->UpdateStatus(m_CurState) ;

			SptPoint pt ;

			pt.nX = lParam&0xffff ;
			pt.nY = (LONG)(lParam >> 16) ;

			this->OnLButtonUp((int)wParam,pt) ;
		}
		break ;

	case WM_MOUSEMOVE:
		{
			SptPoint pt ;

			pt.nX = lParam&0xffff ;
			pt.nY = (LONG)(lParam >> 16) ;

			this->OnMouseMove((int)wParam,pt) ;

			if(m_CurState.status.MouseStatus==ControlMouseStatus_Normal)
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = m_hWnd;
				tme.dwFlags = TME_LEAVE | TME_HOVER;
				tme.dwHoverTime = 1;

				::TrackMouseEvent(&tme);
			}
		}
		break ;

	case WM_RBUTTONDOWN:
		{
			SptPoint pt ;

			pt.nX = lParam&0xffff ;
			pt.nY = (LONG)(lParam >> 16) ;

			this->OnRButtonDown((int)wParam,pt) ;
		}
		break ;

	case WM_RBUTTONUP:
		{
			SptPoint pt ;

			pt.nX = lParam&0xffff ;
			pt.nY = (LONG)(lParam >> 16) ;

			this->OnRButtonUp((int)wParam,pt) ;
		}
		break ;

	case WM_MOUSEHOVER:
		m_CurState.status.MouseStatus = ControlMouseStatus_Hover ;
		this->UpdateStatus(m_CurState) ;
		this->OnMouseEnter() ;
		break ;

	case WM_MOUSELEAVE:
		m_CurState.status.MouseStatus = ControlMouseStatus_Leave ;
		this->UpdateStatus(m_CurState) ;
		this->OnMuseLeave() ;
		m_CurState.status.MouseStatus = ControlMouseStatus_Normal ;
		//this->UpdateStatus(m_CurState) ;
		break ;

	case WM_PAINT:
		this->OnPaint() ;
		break ;

	case WM_ERASEBKGND:
		{
			HDC hdc = (HDC)wParam ;
			BOOL b = this->OnEraseBkgnd(hdc) ;
			return b ;
		}
		break ;
	}

	return nRet ;
}

void CptCommCtrl::OnPaint()
{
}

int CptCommCtrl::ProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam)
{
	CptCommCtrl* pThis = (CptCommCtrl*)::GetProp(hWnd,s_ptCommCtrlThisEntry) ;

	if(pThis!=NULL)
	{
		return pThis->PreProcCtrlMsg(hWnd,nMsg,wParam,lParam) ;
	}

	return FALSE ;
}

int CptCommCtrl::PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) 
{
	int nRet = 0 ;

	bContinue = true ;

	if(nMsg==WM_NOTIFY)
	{
		if(wParam==::GetDlgCtrlID(m_hWnd))
		{
			int nResult = this->OnNMCustomdraw((NMHDR*) lParam,bContinue) ;

			if(!bContinue)
			{
				return nResult ;
			}
		}
	}
	
	/**
	if(m_pNextNode!=NULL)
	{
		if(m_pNextNode->pThis!=NULL)
		{
			m_pNextNode->pThis->m_pNextNode = m_pNextNode->pNext ;
		}

		nRet = m_pNextNode->pThis->PreProcParentMsg( hWnd, nMsg, wParam,  lParam) ;
	}
	else
	{
		
		nRet = ::CallWindowProc(m_pParentDefWndProc,hWnd,nMsg,wParam,lParam) ;
	}
	
	m_pNextNode = NULL ;
	/**/

	//if(!(m_pParentDefWndProc==(WNDPROC)ProcParentMsg))
	//{
	//	//nRet = ::CallWindowProc(m_pParentDefWndProc,hWnd,nMsg,wParam,lParam) ;
	//}

	return nRet ;
}

int CptCommCtrl::ProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam)
{
	/**
	int nRet = FALSE ;

	WNDPROC wndPro = NULL ;

	{
		CptAutoLock lock(&m_MapLock) ;

		std::map<HWND,SParentComCtrlInfo>::iterator it = m_ParentComCtrlMap.find(hWnd) ;
		
		if(it!=m_ParentComCtrlMap.end())
		{
			//Debug_Printf(_T("CptCommCtrl::ProcParentMsg() %p"),hWnd) ;
			std::list<CptCommCtrl*>::iterator it2 = (*it).second.ComCtrlList.begin() ;

			bool bContinue = true ;

			for(;it2!=(*it).second.ComCtrlList.end();++it2)
			{
				nRet = (*it2)->PreProcParentMsg(hWnd,nMsg,wParam,lParam,bContinue) ;
			}

			wndPro = (*it).second.OldWndProc ;
		}
	}

	if(wndPro!=NULL)
	{
		nRet = ::CallWindowProc(wndPro,hWnd,nMsg,wParam,lParam) ;
	}

	return nRet ;
	/**/

	int nRet = FALSE ;

	WNDPROC wndPro = NULL ;

	{
		CptAutoLock lock(&m_MapLock) ;

		pt_STL_map(HWND,SParentComCtrlInfo)::iterator it = m_ParentComCtrlMap.find(hWnd) ;
		
		if(it!=m_ParentComCtrlMap.end())
		{
			
			pt_STL_list(CptCommCtrl*)::iterator it2 = (*it).second.ComCtrlList.begin() ;

			bool bContinue = true ;

			for(;it2!=(*it).second.ComCtrlList.end();++it2)
			{
				nRet = (*it2)->PreProcParentMsg(hWnd,nMsg,wParam,lParam,bContinue) ;

				if(!bContinue)
				{
					return nRet ;
				}
			}

			wndPro = (*it).second.OldWndProc ;
		}
	}

	if(wndPro!=NULL)
	{
		nRet = (int)::CallWindowProc(wndPro,hWnd,nMsg,wParam,lParam) ;
	}

	return nRet ;
}