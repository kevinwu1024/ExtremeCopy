/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"

#include "Win7TaskbarProgress.h"

CWin7TaskbarProgress::CWin7TaskbarProgress() 
{
	 m_pTaskbarList = NULL ;
//	 WM_TAASKBARBUTTONCREATEED = 0 ;

	DWORD dwVersion = ::GetVersion() ;
	DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	if((dwMajorVersion>6 || (dwMajorVersion==6 && dwMinorVersion>=1)))
	{
		//WM_TAASKBARBUTTONCREATEED = ::RegisterWindowMessage(_T("TaskbarButtonCreated")); 

		if(SUCCEEDED(CoInitialize(NULL)))
		{
			HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pTaskbarList));

			if(FAILED(m_pTaskbarList->HrInit()))
			{
				m_pTaskbarList->Release() ;
				m_pTaskbarList = NULL ;
			}
		}
	}
}

CWin7TaskbarProgress::~CWin7TaskbarProgress() 
{
	if(m_pTaskbarList!=NULL)
	{
		m_pTaskbarList->Release() ;
		m_pTaskbarList = NULL ;
	}
}

//void CWin7TaskbarProgress::ProcessMsg(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) 
//{
//	if(uMsg==WM_TAASKBARBUTTONCREATEED && m_pTaskbarList!=NULL)
//	{
//		if(m_pTaskbarList!=NULL)
//		{
//			m_hWnd = hWnd ;
//
//		}
//	}
//}

void CWin7TaskbarProgress::SetProgressValue(HWND hWnd,int nValue) 
{
	if(m_pTaskbarList!=NULL)
	{
		nValue = max(nValue,0) ;
		nValue = min(nValue,100) ;

		m_pTaskbarList->SetProgressValue(hWnd,nValue,100);
	}
}