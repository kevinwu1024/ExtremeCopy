/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "CptListBox.h"
#include <shellapi.h>

CptListBox::CptListBox(HWND hWnd)
{
}

CptListBox::~CptListBox(void)
{
}

void CptListBox::Attach(HWND hWnd) 
{
	CptCommCtrl::Attach(hWnd) ;

}

void CptListBox::UpdateHorizontalExtent() 
{
	if(m_hWnd!=NULL)
	{
		const int nItemCount = this->GetItemCount() ;

		if(nItemCount==0)
		{
			::SendMessage(m_hWnd,LB_SETHORIZONTALEXTENT,0,0) ;
		}
		else
		{
			HDC hDevDC = ::GetDC(m_hWnd) ;
			SIZE size ;
			::memset(&size,0,sizeof(size)) ;
			TCHAR* pStrBuf = new TCHAR[512] ;
			pStrBuf[0] = 0;
			int nWidth = 0 ;
			
			int nLen = 0 ;

			for(int i=0;i<nItemCount;++i)
			{
				if(this->GetString(i,pStrBuf)
					&& ::GetTextExtentPoint32(hDevDC,pStrBuf,(int)::_tcslen(pStrBuf),&size))
				{
					if(size.cx>nWidth)
					{
						nWidth = size.cx ;
					}
				}
			}


			if(nWidth>0)
			{
				::SendMessage(m_hWnd,LB_SETHORIZONTALEXTENT,nWidth,0) ;
				//SptRect rt ;
				//::GetWindowRect(m_hWnd,rt.GetRECTPointer()) ;
				//const int nOffset = nWidth-rt.GetWidth() ;

				//if(nOffset>0)
				//{
				//	::SendMessage(m_hWnd,LB_SETHORIZONTALEXTENT,nOffset+10,0) ;
				//}
				//else
				//{
				//	::SendMessage(m_hWnd,LB_SETHORIZONTALEXTENT,0,0) ;
				//}

			}

			delete [] pStrBuf ;
			pStrBuf = NULL ;

			::ReleaseDC(m_hWnd,hDevDC) ;
		}
	}
}

bool CptListBox::AddString(const TCHAR* pStr) 
{
	bool bRet = false ;

	if(m_hWnd!=NULL && pStr!=NULL)
	{
		bRet = (CB_ERR!=::SendMessage(m_hWnd,LB_ADDSTRING,NULL,(LPARAM)pStr)) ;

		if(bRet)
		{
			this->UpdateHorizontalExtent() ;
		}
	}

	return bRet ;
}

bool CptListBox::DeleteString(const int nIndex) 
{
	bool bRet = false ;

	if(m_hWnd!=NULL)
	{
		bRet = (CB_ERR!=::SendMessage(m_hWnd,LB_DELETESTRING,nIndex,NULL)) ;

		if(bRet)
		{
			this->UpdateHorizontalExtent() ;
		}
	}

	return bRet ;
}

bool CptListBox::GetString(int nIndex,TCHAR* pStr)
{
	bool bRet = false ;

	if(m_hWnd!=NULL && pStr!=NULL)
	{
		bRet = (CB_ERR!=::SendMessage(m_hWnd,LB_GETTEXT,nIndex,(LPARAM)pStr)) ;
	}

	return bRet ;
}

bool CptListBox::UpdateString(int nIndex,const TCHAR* lpStr)
{
	bool bRet = false ;

	if(nIndex>=0 && lpStr!=NULL)
	{
		const int nCount = this->GetItemCount() ;

		if(nCount>=0 && nIndex<nCount)
		{
			if(this->DeleteString(nIndex))
			{
				bRet = InsertString(nIndex,lpStr) ;
			}
		}
	}

	if(bRet)
	{
		this->UpdateHorizontalExtent() ;
	}

	return bRet ;
}

bool CptListBox::InsertString(int nIndex,const TCHAR* lpStr)
{
	bool bRet = false ;

	if(m_hWnd!=NULL && lpStr!=NULL)
	{
		bRet = (CB_ERR!=::SendMessage(m_hWnd,LB_INSERTSTRING,nIndex,(LPARAM)lpStr)) ;

		if(bRet)
		{
			this->UpdateHorizontalExtent() ;
		}
	}

	return bRet ;
}
//bool CptListBox::SetString(int nIndex,const TCHAR* pStr)
//{
//	bool bRet = false ;
//
//	if(m_hWnd!=NULL && pStr!=NULL)
//	{
//		//bRet = (CB_ERR!=::SendMessage(m_hWnd,LB_SETTEXT,nIndex,(LPARAM)pStr)) ;
//	}
//
//	return bRet ;
//}

void CptListBox::Clear() 
{
	if(m_hWnd!=NULL)
	{
		::SendMessage(m_hWnd,LB_RESETCONTENT,NULL,NULL) ;
	}
}

int CptListBox::GetSelectIndex() 
{
	int nRet = LB_ERR ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)::SendMessage(m_hWnd,LB_GETCURSEL,NULL,NULL) ;
	}
	
	return nRet ;
}

bool CptListBox::SetSelectIndex(int nIndex) 
{
	bool bRet = false ;

	if(m_hWnd!=NULL)
	{
		bRet = (CB_ERR!=::SendMessage(m_hWnd,LB_SETCURSEL,nIndex,NULL)) ;
	}
	
	return bRet ;
}

int CptListBox::GetSelectCount() 
{
	int nRet = LB_ERR ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)::SendMessage(m_hWnd,LB_GETSELCOUNT,NULL,NULL) ;
	}

	return nRet ;
}

bool CptListBox::GetMultipleSelIndex(std::vector<int>& IndexVer) 
{
	bool bRet = false ;

	int nSelCount = this->GetSelectCount() ;

	if(nSelCount>0)
	{
		IndexVer.resize(nSelCount) ;
		bRet = (::SendMessage(m_hWnd,LB_GETSELITEMS,nSelCount,(LPARAM)IndexVer.data())!=LB_ERR) ;
	}

	return bRet ;
}

int CptListBox::GetItemCount() 
{
	int nRet = -1 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)::SendMessage(m_hWnd,LB_GETCOUNT,NULL,NULL) ;
	}
	
	return nRet ;
}

/**
int CptListBox::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam)
{
	int nRet = CptCommCtrl::PreProcCtrlMsg(hWnd,nMsg,wParam,lParam) ;

	switch(nMsg)
	{
		//case WM_DROPFILES:
		//{
		//	HDROP hDropInfo = (HDROP)wParam ;

		//	const int fileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

		//	TCHAR szFileName[MAX_PATH] = { 0 };
		//	TCHAR szListString[MAX_PATH] = { 0 };

		//	//if(hWnd==this->GetDlgItem(IDC_LIST_SOURCEFILE))
		//	{
		//		for (int i = 0; i < fileCount; ++i)
		//		{
		//			::DragQueryFile(hDropInfo, i, szFileName, sizeof(szFileName));

		//			const int nListCount = this->GetItemCount() ;

		//			int j = 0 ;
		//			for(j=0;j<nListCount;++j)
		//			{
		//				szListString[0] = 0 ;
		//				this->GetString(j,szListString) ;

		//				CptString str1 = szListString ;
		//				CptString str2 = szFileName ;

		//				if(str1.CompareNoCase(str2)==0)
		//				{
		//					break ;
		//				}
		//			}

		//			if(j==nListCount)
		//			{
		//				this->AddString(szFileName) ;
		//			}
		//		}
		//	}
		//	
		//	::DragFinish(hDropInfo) ;
		//}

		//return 0 ;
	}

	

	return nRet ;
}
/**/

