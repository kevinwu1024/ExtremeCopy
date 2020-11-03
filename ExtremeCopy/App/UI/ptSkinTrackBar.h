/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "ptcommctrl.h"

class CptSkinTrackBar : public CptCommCtrl
{
public:
	CptSkinTrackBar(void);
	virtual ~CptSkinTrackBar(void);

	void Attach(HWND hwnd) ;
	int GetRangeMin() const;
	int GetRangeMax() const;

	void GetRange(int& nMin,int& nMax) const;
	void SetRange(int nMin,int nMax,BOOL bRedraw = FALSE);

	UINT GetNumTics() const;
	void SetTicFreq(int nFreq );
	int GetTicPos(int nTic ) const;

	int GetPos() const;
	void SetPos(int nPos);

protected:
	virtual int OnNMCustomdraw(NMHDR* pNMHDR,bool& bContinue) ;
	virtual BOOL OnEraseBkgnd(HDC hDC) ;

	void DrawChannel(HDC hdc,SptRect rt,UINT uState) ;
	void DrawThumb(HDC hdc,SptRect rt,UINT uState) ;
	void DrawTick(HDC hdc,SptRect rt,UINT uState) ;

private:
	HBITMAP	m_hChannelImage ;
	SptSize m_ChannelImageSize ;

	HBITMAP m_hThumbImage ;
	SptSize m_ThumbImageSize ;
};
