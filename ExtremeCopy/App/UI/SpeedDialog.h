/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "skindialog.h"
#include "ptSkinTrackBar.h"
#include "ptWinToolTip.h"

struct SRunTimeSettingDlgOption
{
	enum
	{
		RunTimeSettingOptionFlag_Speed = 1<<0,
		RunTimeSettingOptionFlag_Verify = 1<<1,
		RunTimeSettingOptionFlag_CloseWindow = 1<<2,
		RunTimeSettingOptionFlag_Shutdown = 1<<3,
	};

	unsigned int	uFlag ;
	int		nSpeed ;
	bool	bIsVerify ;
	bool	bIsCloseWindow ;
	bool	bIsShutdown ;
};

class CXCSettingChangingCB
{
public:
	virtual void OnRunTimeSettingChanged(const SRunTimeSettingDlgOption& option) = 0;
};

class CSpeedDialog : public CSkinDialog
{
public:
	CSpeedDialog(bool bMove,const SRunTimeSettingDlgOption& optoin,CXCSettingChangingCB* pCB=NULL,HWND hParentWnd=NULL);
	virtual ~CSpeedDialog(void);

	int ShowAnimate() ;
	void HideAnimate() ;
	void DelayHide(int nDelayTime=700) ;

	void UpdatePosition() ;

	bool IsShow() const ;
	int GetSpeedValue() const ;

protected:
	virtual BOOL OnInitDialog() ;
	virtual void OnTimer(int nTimerID);
	virtual int OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) ;

	void UpdateSpeedText() ;
	inline int CaculateSpeedValue() const ;

	virtual void OnButtonClick(int nButtonID) ;

private:
	void DrawEdge() ;


#define TIMER_DELAYHIDE		1234

private:
	bool	m_bShow ;
	bool	m_bCursorHover ;
	bool	m_bControlHover ;

	bool	m_bMove ;

	CptSkinTrackBar	m_SkinTrackBar ;
	CptCommCtrl		m_VerifyCheckBox ;
	CXCSettingChangingCB*	m_pSettingChangingCB ;

	SRunTimeSettingDlgOption	m_SettingOption ;
};
