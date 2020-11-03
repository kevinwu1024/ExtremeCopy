/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptWinToolTip.h"

CptWinToolTip::CptWinToolTip(void):m_hToolTipWnd(NULL)
{
}

CptWinToolTip::~CptWinToolTip(void)
{
	this->Destroy() ;
}

void CptWinToolTip::Destroy() 
{
	if(m_hToolTipWnd!=NULL)
	{
		::CloseWindow(m_hToolTipWnd) ;
		::DestroyWindow(m_hToolTipWnd) ;
		m_hToolTipWnd = NULL ;
	}
}

BOOL CptWinToolTip::AddTip(HWND hWnd,HINSTANCE hInst,TCHAR *Tip,UINT id , BOOL Balloon)
{
	//	INITCOMMONCONTROLSEX icc;

	//	icc.dwSize =	sizeof(INITCOMMONCONTROLSEX);
	//icc.dwICC =ICC_BAR_CLASSES | ICC_TAB_CLASSES | ICC_WIN95_CLASSES ;

	//InitCommonControlsEx(&icc);

	hInst = NULL ;
	
	HWND hwndTip;
	TOOLINFO		ti;

	//INITCOMMONCONTROLSEX icc;
	//icc.dwSize =	sizeof(INITCOMMONCONTROLSEX);
	//icc.dwICC =ICC_BAR_CLASSES | ICC_TAB_CLASSES | ICC_WIN95_CLASSES ;

	//InitCommonControlsEx(&icc);

	if(Balloon)//If you have choosen the Boolen Toop Tip will set the Windows style according to that
	{
		hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
			WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP |TTS_BALLOON,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			hWnd, NULL, hInst,
			NULL);
	}
	else
	{

		hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
			WS_POPUP | TTS_NOPREFIX |TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			hWnd, NULL, hInst,
			NULL);
	}

	SendMessage(hwndTip,TTM_ACTIVATE,TRUE,0); //Will Active the Tool Tip Control

	ti.cbSize = sizeof(TOOLINFO);
//#ifdef _UNICODE 
//    ti.cbSize = sizeof ( TTTOOLINFOW_V2_SIZE ); // If use sizeof ( TOOLINFO ), the tooltip will never appear. 
//#else 
//    ti.cbSize = sizeof ( TTTOOLINFOA_V2_SIZE ); // If use sizeof ( TOOLINFO ), the tooltip will never appear. 
//#endif

	ti.uFlags =  TTF_IDISHWND | TTF_SUBCLASS;
	ti.hwnd   = hWnd;							//Handle of the window in which the Contol resides
	ti.uId    =(UINT)GetDlgItem(hWnd,id);       //ID of the Cotrol for which Tool Tip will be Displyed
	ti.hinst  = hInst;
	ti.lpszText  = Tip;							//Tip you want to Display;
	ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 

	if(!SendMessage(hwndTip,TTM_ADDTOOL,0,(LPARAM)&ti))
	{ //Will add the Tool Tip on Control
		//int aa = GetLastError() ;
		MessageBox(NULL,L"Couldn't create the ToolTip control.",L"Error",MB_OK);
	}

	return TRUE;
}

bool CptWinToolTip::Create(HWND hWnd,bool bBalloon)
{
	//INITCOMMONCONTROLSEX icc;

	this->Destroy() ;

	bool bRet = false ;
	//HWND hwndTip;
	
	//icc.dwSize =	sizeof(INITCOMMONCONTROLSEX);
	//icc.dwICC =ICC_BAR_CLASSES | ICC_TAB_CLASSES | ICC_WIN95_CLASSES ;

	//InitCommonControlsEx(&icc);

	UINT uStyle = WS_POPUP | TTS_NOPREFIX |TTS_ALWAYSTIP ;

	if(bBalloon)
	{
		uStyle |= TTS_BALLOON ;
	}

	m_hToolTipWnd = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
		uStyle,CW_USEDEFAULT, CW_USEDEFAULT,CW_USEDEFAULT, CW_USEDEFAULT,
		hWnd, NULL, NULL,NULL);

	//if(bBalloon)//If you have choosen the Boolen Toop Tip will set the Windows style according to that
	//{
	//	m_hToolTipWnd = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
	//		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP |TTS_BALLOON,
	//		CW_USEDEFAULT, CW_USEDEFAULT,
	//		CW_USEDEFAULT, CW_USEDEFAULT,
	//		hWnd, NULL, NULL,
	//		NULL);
	//}
	//else
	//{
	//	m_hToolTipWnd = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
	//		WS_POPUP | TTS_NOPREFIX |TTS_ALWAYSTIP,
	//		CW_USEDEFAULT, CW_USEDEFAULT,
	//		CW_USEDEFAULT, CW_USEDEFAULT,
	//		hWnd, NULL, NULL,
	//		NULL);
	//}

	bRet = (m_hToolTipWnd!=NULL) ;

	if(m_hToolTipWnd!=NULL)
	{
		::SendMessage(m_hToolTipWnd,TTM_ACTIVATE,TRUE,0); //Will Active the Tool Tip Control
	}

	//if(!SendMessage(hwndTip,TTM_ADDTOOL,0,(LPARAM)&ti)){ //Will add the Tool Tip on Control
	//	//MessageBox(NULL,L"Couldn't create the ToolTip control.",L"Error",MB_OK);

	//}

	return bRet;
}

bool CptWinToolTip::BindControl(HWND hControlWnd,TCHAR* pTxt)
{
	bool bRet = false ;

	if(m_hToolTipWnd!=NULL)
	{
		TOOLINFO		ti;

		::memset(&ti,0,sizeof(ti)) ;
		ti.cbSize = sizeof(TOOLINFO);

//#ifdef _UNICODE 
//    ti.cbSize = sizeof ( TTTOOLINFOW_V2_SIZE ); // If use sizeof ( TOOLINFO ), the tooltip will never appear. 
//#else 
//    ti.cbSize = sizeof ( TTTOOLINFOA_V2_SIZE ); // If use sizeof ( TOOLINFO ), the tooltip will never appear. 
//#endif

		//ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags =  TTF_IDISHWND | TTF_SUBCLASS;
		ti.hwnd   = ::GetParent(hControlWnd);		//Handle of the window in which the Contol resides
		ti.uId    =(UINT)hControlWnd;       //ID of the Cotrol for which Tool Tip will be Displyed
		//ti.hinst  = hInst;
		ti.lpszText  = pTxt;							//Tip you want to Display;
		//ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 

		bRet = ::SendMessage(m_hToolTipWnd,TTM_ADDTOOL,0,(LPARAM)&ti) ? true : false ;
	}

	return bRet ;
}

bool CptWinToolTip::UpdateDisplayText(LPTSTR lpszTxt)
{
	return false ;
	bool bRet = false ;

	if(m_hToolTipWnd!=NULL)
	{
		TOOLINFO		ti;

		::memset(&ti,0,sizeof(ti)) ;

		ti.cbSize = sizeof(TOOLINFO);
		//ti.uFlags =  TTF_IDISHWND | TTF_SUBCLASS;
		//ti.hwnd   = hWnd;							//Handle of the window in which the Contol resides
		//ti.uId    =(UINT)hControlWnd;       //ID of the Cotrol for which Tool Tip will be Displyed
		//ti.hinst  = hInst;
		ti.lpszText  = lpszTxt;							//Tip you want to Display;
		//ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 

		::SendMessage(m_hToolTipWnd,TTM_UPDATETIPTEXT,0,(LPARAM)&ti) ;
	}

	return bRet ;
}