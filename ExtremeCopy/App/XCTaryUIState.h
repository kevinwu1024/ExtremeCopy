/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "XCGlobal.h"
#include "..\Common\ptShellTray.h"

class CXCTaryUIState :
	public CXCGUIState
{
public:
	CXCTaryUIState(void);
	virtual ~CXCTaryUIState(void);

	virtual void UpdateUI(const EUpdateUIType uuit,void* pParam1,void* pParam2) ;
	virtual void ProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)  ;

	virtual CptString GetCurFileName() {return m_strCurFileName;}

	virtual void OnEnter(CptString strCurFileName) ;
	virtual void OnLeave() ;

private:
	void UpdateOneSecondUI(SXCUIOneSecondUpdateDisplay& osud) ;
	void SetFileProgressString(bool bReadOrWrite) ;
	void UpdateUIString() ;

private:
	CptShellTray			m_ShellTray ;

	CptString				m_strCurFileName ;
	CptString				m_strSpeed ;
	CptString				m_strLapseTime ;
	CptString				m_strRemainTime ;
	CptString				m_strCurProcess ;
	CptString				m_strTotalProcess ;

	CptString				m_strTitle ;
	CptString				m_strUIFormatText ;
};
