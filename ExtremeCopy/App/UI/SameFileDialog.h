/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "ptDialog.h"
#include "..\..\Common\ptString.h"
#include "ptSkinButton.h"
#include "..\..\Common\ptTypeDef.h"
#include "..\XCGlobal.h"


class CSameFileDialog : public CptDialog
{
public:
	CSameFileDialog(CptString strSrcFile,CptString strDstFile,HWND hParentWnd=NULL);
	virtual ~CSameFileDialog(void);

	SImpactFileBehaviorSetting GetFileBehaviorSetting() const {return m_FileBehaviorSetting;}

	EImpactFileBehaviorResult GetResult() const {return m_ImpactFileResult;}

protected:
	virtual void OnButtonClick(int nButtonID) ;
	virtual BOOL OnInitDialog() ;
	virtual void OnClose() ;
	virtual bool OnCancel() ;
	virtual void OnPaint() ;
	virtual int OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) ;

private:
	void UpdateDataToControl() ;
	void UpdateDataFromControl() ;

private:
	SImpactFileBehaviorSetting m_FileBehaviorSetting ;
	CptString m_strSrcFile ;
	CptString m_strDstFile ;

	EImpactFileBehaviorResult m_ImpactFileResult ;

	HFONT m_hCurFileFont ;
	HFONT m_hReplaceFont ;

	HICON	m_hFileIcon ;

	SptRect				m_OrginRect ;
	bool				m_bMoreOrLess ;
	int				m_nMoreHeightOffset ;

	CptSkinButton		m_MoreLessSkinButton ;
};
