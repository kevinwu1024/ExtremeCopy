/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "CptEditBox.h"
#include <shellapi.h>


CptEditBox::CptEditBox(void)
{
}

CptEditBox::~CptEditBox(void)
{
}

void CptEditBox::Attach(HWND hWnd) 
{
	CptCommCtrl::Attach(hWnd) ;
}

void CptEditBox::SetLimitText(unsigned int nMax) 
{
	_ASSERT(m_hWnd!=NULL) ;
	::SendMessage(m_hWnd, EM_SETLIMITTEXT, nMax, 0);
}

unsigned CptEditBox::GetLimitText() 
{
	_ASSERT(m_hWnd!=NULL) ;

	return (unsigned)::SendMessage(m_hWnd, EM_GETLIMITTEXT, 0, 0);
}
//
//int CptEditBox::PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam)
//{
//	int nRet = CptCommCtrl::PreProcCtrlMsg(hWnd,nMsg,wParam,lParam) ;
//
//	switch(nMsg)
//	{
//	//case WM_DROPFILES:
//	//	{
//	//		HDROP hDropInfo = (HDROP)wParam ;
//
//	//		const int fileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
//
//	//		TCHAR szFileName[MAX_PATH] = { 0 };
//	//		//TCHAR szListString[MAX_PATH] = { 0 };
//
//	//		//if(hWnd==this->GetDlgItem(IDC_LIST_SOURCEFILE))
//	//		{
//	//			for (int i = 0; i < fileCount; ++i)
//	//			{
//	//				::DragQueryFile(hDropInfo, i, szFileName, sizeof(szFileName));
//
//	//				if(CptGlobal::IsFolder(szFileName))
//	//				{
//	//					::SetWindowText(hWnd,szFileName) ;
//	//				}
//	//			}
//	//		}
//
//	//		::DragFinish(hDropInfo) ;
//	//	}
//
//	//	return 0 ;
//	//}
//
//	return nRet ;
//}
//