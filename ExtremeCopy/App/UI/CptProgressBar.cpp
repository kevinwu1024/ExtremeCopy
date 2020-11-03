/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "CptProgressBar.h"
#include <commctrl.h>

#pragma comment(lib,"comctl32.lib")

CptProgressBar::CptProgressBar(HWND hBarWnd):m_hBarWnd(hBarWnd)
{
}

CptProgressBar::CptProgressBar(HWND hParent,int nBarID)
{
	m_hBarWnd = ::GetDlgItem(hParent,nBarID) ;
}

CptProgressBar::CptProgressBar():m_hBarWnd(NULL)
{
}

CptProgressBar::~CptProgressBar(void)
{
}

void CptProgressBar::SetControl(HWND hBarWnd) 
{
	m_hBarWnd = hBarWnd ;
}

void CptProgressBar::SetControl(HWND hParent,int nBarID) 
{
	m_hBarWnd = ::GetDlgItem(hParent,nBarID) ;
}


void CptProgressBar::SetRange(int nMin,int nMax) 
{
	::SendMessage(m_hBarWnd,PBM_SETRANGE,0,MAKELPARAM(nMin,nMax)) ;
}

int CptProgressBar::GetMinValue() 
{
	return (int)::SendMessage(m_hBarWnd,PBM_GETRANGE,TRUE,NULL) ;
}

int CptProgressBar::GetMaxValue() 
{
	return (int)::SendMessage(m_hBarWnd,PBM_GETRANGE,FALSE,NULL) ;
}

void CptProgressBar::SetValue(int nValue) 
{
	::SendMessage(m_hBarWnd,PBM_SETPOS,nValue,0) ;
}

int CptProgressBar::GetValue() 
{
	return (int)::SendMessage(m_hBarWnd,PBM_GETPOS,NULL,NULL) ;
}