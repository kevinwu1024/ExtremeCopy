/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "CptComboBox.h"
#include <shellapi.h>

CptComboBox::CptComboBox(void)
{
}

CptComboBox::~CptComboBox(void)
{
}

bool CptComboBox::AddString(const TCHAR* pStr) 
{
	bool bRet = false ;

	if(m_hWnd!=NULL && pStr!=NULL)
	{
		bRet = (CB_ERR!=::SendMessage(m_hWnd,CB_ADDSTRING,NULL,(LPARAM)pStr)) ;
	}

	return bRet ;
}

void CptComboBox::Clear() 
{
	if(m_hWnd!=NULL)
	{
		::SendMessage(m_hWnd,CB_RESETCONTENT,NULL,NULL) ;
	}
}

int CptComboBox::GetSelectIndex() 
{
	int nRet = -1 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)::SendMessage(m_hWnd,CB_GETCURSEL,NULL,NULL) ;
	}
	
	return nRet ;
}

bool CptComboBox::SetSelectIndex(int nIndex) 
{
	bool bRet = false ;

	if(m_hWnd!=NULL)
	{
		bRet = (CB_ERR!=::SendMessage(m_hWnd,CB_SETCURSEL,nIndex,NULL)) ;
	}
	
	return bRet ;
}

int CptComboBox::GetItemCount() 
{
	int nRet = -1 ;

	if(m_hWnd!=NULL)
	{
		nRet = (int)::SendMessage(m_hWnd,CB_GETCOUNT,NULL,NULL) ;
	}
	
	return nRet ;
}


int CptComboBox::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam)
{
	int nRet = CptCommCtrl::PreProcCtrlMsg(hWnd,nMsg,wParam,lParam) ;

	switch(nMsg)
	{
	case WM_DROPFILES:
		{
			HDROP hDropInfo = (HDROP)wParam ;

			const int fileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

			TCHAR szFileName[MAX_PATH] = { 0 };
			//TCHAR szListString[MAX_PATH] = { 0 };

			//if(hWnd==this->GetDlgItem(IDC_LIST_SOURCEFILE))
			{
				for (int i = 0; i < fileCount; ++i)
				{
					::DragQueryFile(hDropInfo, i, szFileName, sizeof(szFileName));

					if(CptGlobal::IsFolder(szFileName))
					{
						this->AddString(szFileName) ;
						//::SetWindowText(hWnd,szFileName) ;
					}

				}
			}

			::DragFinish(hDropInfo) ;
		}

		return 0 ;
	}

	return nRet ;
}