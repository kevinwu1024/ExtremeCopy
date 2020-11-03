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
#include "ptSkinButton.h"
#include "CptListBox.h"
#include "CptEditBox.h"
#include "MainDialog.h"
#include "Hyperlink.h"
#include "CptComboBox.h"

class CXCFileListBox : public CptListBox
{
public:
	CXCFileListBox(HWND hWnd=NULL) ;

	void RemoveSelItems() ;
protected:
	virtual int PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;
};

class CXCSrcFileListBox : public CXCFileListBox
{
public :
	CXCSrcFileListBox(HWND hWnd=NULL) ;
protected:
	virtual void OnDropFiles(HDROP hDropInfo) ;
};

class CXCDstFolderListBox : public CXCFileListBox
{
public :
	CXCDstFolderListBox(HWND hWnd=NULL) ;
protected:
	virtual void OnDropFiles(HDROP hDropInfo) ;
};

class CXCTaskEditDlg :public CptDialog
{
public:
	CXCTaskEditDlg(const SXCCopyTaskInfo& task, int nDlgID=IDD_DIALOG_SPECIFYSRCDST);
	virtual ~CXCTaskEditDlg(void);
	SXCCopyTaskInfo GetTaskInfo() const {return m_TaskInfo;}

protected:
	virtual void OnButtonClick(int nButtonID) ;

	virtual void OnListBoxItemDoubleClicked(int nID) ;
	virtual void OnListBoxItemSelectChanged(int nID,HWND hListBoxWnd) ;
		
	virtual BOOL OnInitDialog() ;

	virtual void OnPaint() ;
	virtual int OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) ;

private:
	void Run(bool bCopyOrMove) ;
	void UpdateDataToControls() ;
	void UpdateDataFromControls() ;

	inline void SetRadioCheck(pt_STL_vector(UINT) CtrlIDVer,UINT nCheckedID) ;
	
	void SetSameFileProcCheck(SRichCopySelection::ESameFileProcessType sfpt) ;
	void SetErrorProcCheck(SRichCopySelection::EErrorProcessType ept) ;
	void SetShowModeCheck(SXCCopyTaskInfo::EShowMode sm) ;
	void SetRetryFailThenCheck(SRichCopySelection::EErrorProcessType sfpt) ;
	void SaveTaskFile(SXCCopyTaskInfo::EExecuteType et) ;

	void ShowWildcardDlg() ;
	bool CheckListStringIsFolderOrWildcard() ;

	void ShowMenu(int nMenuIndex) ;

	void UpdateCopyOrMoveTaskView() ;


private:
	CptSkinButton				m_RunSkinButton ;
	CptSkinButton				m_MoveSkinButton ;

	CptSkinButton				m_MoreLessSkinButton ;

	CptWinToolTip				m_ExportButtonTooltip ;
	CptWinToolTip				m_ImportButtonTooltip ;

	CptSkinButton				m_ExportToFileSkinButton ;
	CptSkinButton				m_ImportFromFileSkinButton ;

	CXCSrcFileListBox			m_SourceListBox ;
	CXCDstFolderListBox			m_DestListBox ;

	SXCCopyTaskInfo				m_TaskInfo ;

	bool						m_bMoreOrLess ;
	SptRect						m_OrginRect ;
	
	int							m_nLessDlgHeight ;
	HMENU						m_hTaskMenu ;

	CptComboBox					m_SFIfConditonComboBox ;
	CptComboBox					m_SFThenOperationComboBox ;
	CptComboBox					m_SFOtherwiseOperationComboBox ;

#ifndef VERSION_PROFESSIONAL // 非专业版时
	CHyperlink					m_Hyperlink;
#endif
};
