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

class CXCNormalWndUIState :
	public CXCGUIState
{
public:
	CXCNormalWndUIState(void);
	virtual ~CXCNormalWndUIState(void);

	virtual void UpdateUI(const EUpdateUIType uuit,void* pParam1,void* pParam2)  ;

	virtual void ProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)  ;

	void SetParameter(CXCGUIState* pNextState,CMainDialog* pDlg)  ;
	virtual CptString GetCurFileName()  ;

	virtual void OnEnter(CptString strCurFileName) ;

private:
	void UpdateOneSecondUI(SXCUIOneSecondUpdateDisplay& osud,bool bCopyDone) ;
	void UpdateBeginCopyOneFileUI(CptString strSrcFile,CptString strDstFile) ;
	void UpdateCopyDataOccuredUI(bool bReadOrWrite,UINT uSize) ;

private:
	bool	m_bUpdateTotalFiles ;
};
