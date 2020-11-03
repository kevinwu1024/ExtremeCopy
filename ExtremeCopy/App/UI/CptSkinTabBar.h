/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "ptCommCtrl.h"
#include "..\XCGlobal.h"

class CptSkinTabBar : public CptCommCtrl
{
public:
	struct SImageInfo
	{
		HBITMAP hLeft ;
		HBITMAP hRight ;
		HBITMAP hSpan ;
		HBITMAP hBackground ;

		SImageInfo()
		{
			hLeft = NULL ;
			hRight = NULL ;
			hSpan = NULL ;
			hBackground = NULL ;
		}
	};

public:
	CptSkinTabBar(void);
	virtual ~CptSkinTabBar(void);

	int AddButton(LPCTSTR lpText) ;
	void SetImage(const SImageInfo& images) ;

protected:
	virtual void OnLButtonDown(int nFlag,const SptPoint& pt) ;
	virtual void OnLButtonUp(int nFlag,const SptPoint& pt) ;
	virtual void OnMouseMove(int nFlag,const SptPoint& pt) ;
	virtual void OnPaint() ;

	virtual int PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;
	virtual int PreProcParentMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam,bool& bContinue) ;

private:
	struct STabButton
	{
		TCHAR		szText[64+1] ;
		SptRect		rect ;
	};

private:
	pt_STL_vector(STabButton)		m_TabButtonVctor ;
	int								m_nCurIndex ;
	SImageInfo						m_Images ;

};
