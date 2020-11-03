/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "SkinDialog.h"
#include "ptSkinButton.h"

class CptMessageBox : public CptDialog
{
public:
	enum EMsgResult
	{
		MsgResult_Yes,
		MsgResult_No,
		MsgResult_Retry,
		MsgResult_Ignore,
		MsgResult_Cancel
	};

	enum EButton
	{
		Button_OK,
		Button_YesNo,
		Button_RetryIgnoreCancel,
		Button_RetryCancel,
		Button_IgnoreCancel,
	};

	CptMessageBox(int nDlgID,HWND hParentWnd);
	~CptMessageBox(void);

	static EMsgResult ShowMessage(HWND hParentWnd,CptString strText,CptString strTitle,EButton button) ;

private:
	EMsgResult ShowMessage(CptString strText,CptString strTitle,EButton button) ;

protected:
	virtual void OnPaint() ;
	virtual BOOL OnInitDialog() ;
	virtual void OnButtonClick(int nButtonID) ;
	virtual int OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) ;

private:
	CptString m_strText ;
	CptString m_strTitle ;
	EMsgResult	m_Result ;

	CptSkinButton m_CloseSkinButton ;

	CptSkinButton m_SkinButton1 ;
	CptSkinButton m_SkinButton2 ;
	CptSkinButton m_SkinButton3 ;

	EButton		m_ButtonType ;
	
};
