/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "..\..\Common\ptTypeDef.h"

class CptSkinProgress
{
public:
	CptSkinProgress(void);
	~CptSkinProgress(void);

	void SetRectangle(const SptRect& rt) ;
	void SetParent(HWND hWnd) ;

	bool SetRange(int nMax,int nMin) ;//{m_nMaxValue=nMax;m_nMinValue=nMin;}
	int GetMax() const {return m_nMaxValue;}
	int GetMin() const {return m_nMinValue;}

	int GetValue() const {return m_nCurValue;}
	void SetValue(int nValue) ;


	void Draw() ;
	void Draw(HDC hDC) ;
private:
	
private:
	int			m_nMaxValue ;
	int			m_nMinValue ;
	int			m_nCurValue ;

	SptRect		m_Rect ;
	HWND		m_hParentWnd ;
	HBITMAP		m_hBarBitmap ;
	HBITMAP		m_hResultBufBitmap ;
	HFONT		m_hPercentFont ;
};
