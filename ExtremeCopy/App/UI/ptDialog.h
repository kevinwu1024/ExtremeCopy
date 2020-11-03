/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include <windows.h>
#include "../../Common/ptTypeDef.h"
#include "../../Common/ptThreadLock.h"

#include <list>
#include "ptWinCtrlDef.h"

class CptDialog
{
public:
	enum EDialogResult
	{
		DialogResult_OK ,
		DialogResult_Cancel ,
	};

	CptDialog(UINT uIDTemplate,HWND hParent=NULL,HMODULE hModule=NULL) ;

	virtual ~CptDialog(void);

	bool Create() ;
	EDialogResult ShowDialog(bool bShow=true) ;
	bool ShowModeless() ;

	static CptDialog* GetModeless(UINT uIDTemplate,HWND hParentWnd) ;
	CptDialog* GetModeless() ;

	int Show() ;
	void Hide() ;

	void CenterScreen() ;
	void CenterParentWindow() ;

	bool SetTimer(int nTimerID,int nInterval) ;
	void KillTimer(int nTimerID) ;
	void SetWindowSize(const SptSize& size) ;

	void Close() ;

	void SetDlgItemText(int nItemID,LPCTSTR lpText) ;
	UINT GetDlgItemText(int nItemID,LPTSTR lpText,int nMaxSize) ;

	BOOL SetDlgItemInt(int nItemID,UINT nValue) ;
	int GetDlgItemInt(int nItemID) ;

	void SetIcon(HICON hIcon) ;

	HWND GetDlgItem(int nItemID) const ;
	long SendMessage(UINT uMsg,WPARAM wParam,LPARAM lParam) ;
	BOOL PostMessage(UINT uMsg,WPARAM wParam,LPARAM lParam) ;

	HWND GetSafeHwnd() const {return m_hWnd ;}
	HWND GetParentHwnd() const {return m_hParentWnd;}

	bool ShowInTaskbar(bool bShow) ;
	bool IsVisible() const ;

protected:
	typedef bool (__thiscall CptDialog::*PreTranslateMessage_t)(HWND, UINT, WPARAM, LPARAM) ;

	virtual BOOL OnInitDialog() {return FALSE;}
	virtual void OnPaint() {}
	virtual BOOL OnEraseBkgnd(HDC hDC){return FALSE;} 
	virtual void OnClose() ;
	virtual void OnDestroy() ;
	virtual void OnButtonClick(int nButtonID) {}

	virtual void OnListBoxItemDoubleClicked(int nID) {}
	virtual void OnListBoxItemSelectChanged(int nID,HWND hListBoxWnd) {}

	//virtual void OnButtonDoubleClick(int nButtonID) {}
	virtual void OnControlStatusChanged(int nControlID,SControlStatus NewStatus) {}
	virtual bool OnOK() ;
	virtual bool OnCancel() ;
	virtual void OnTimer(int nTimerID){} 
	virtual int OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) ;
	virtual void OnSize(UINT uType,int nNewWidth,int nNewHeigh) {}

	//  Û±Íœ˚œ¢
	virtual void OnLButtonDown(int nFlag,const SptPoint& pt) ;
	virtual void OnLButtonUp(int nFlag,const SptPoint& pt) ;
	virtual void OnMouseMove(int nFlag,const SptPoint& pt) ;

	virtual bool PreTranslateMessage(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) ;

	void PerformButton(int nButtonID) ;

private:
	void GetScreenSize(int& pWidth, int& pHeight) ;
	static BOOL __stdcall DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) ;
	static unsigned __stdcall ModelessThread(void* pParam) ;

	inline void SetExitFlag() ;
	//static LRESULT __stdcall GetHookMsgProc(int  code,  WPARAM  wParam,  LPARAM  lParam) ;

protected:
	EDialogResult	m_Result ;

private:
	HWND		m_hWnd ;
	HWND		m_hParentWnd ;
	UINT		m_uTemplateID ;
	DWORD		m_nFlags ;

private:
	struct SDlgListInfo
	{
	public:
		void Push(CptDialog* pDlg)
		{
			_ASSERT(NULL!=pDlg) ;
			CptAutoLock lock(&ListLock) ;

			DlgList.push_back(pDlg) ;
		}

		void Pop()
		{
			_ASSERT(!DlgList.empty()) ;

			CptAutoLock lock(&ListLock) ;

			DlgList.pop_back() ;
		}

		bool IsEmpty()
		{
			CptAutoLock lock(&ListLock) ;

			return DlgList.empty() ;
		}

		CptDialog* GetTop()
		{
			CptAutoLock lock(&ListLock) ;

			return DlgList.back() ;
		}

	public:
		pt_STL_list(CptDialog*) DlgList ;
		CptCritiSecLock ListLock ;
	};

	static SDlgListInfo		s_DlgListInfo ;

	HMODULE		m_hModule ;
};
