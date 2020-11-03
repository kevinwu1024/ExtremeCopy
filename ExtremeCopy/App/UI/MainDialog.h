
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
#include "..\Language\XCRes_ENU\Resource.h"
#include "CptProgressBar.h"

#include "SkinDialog.h"
#include "ptSkinButton.h"

#include "ptSkinProgress.h"

#include <vector>
#include "Hyperlink.h"

#include "XCFailFileListView.h"
#include "CptSkinTabCtrl.h"
#include "CptSkinTabBar.h"
#include "SpeedDialog.h"

#include "ptWinToolTip.h"

#include "..\..\Common\ptShellTray.h"
#include "..\XCTaryUIState.h"
#include "..\XCNormalWndUIState.h"
#include "..\XCCommandLine.h"

#include "..\..\Common\ptFolderSize.h"

#include "..\XCTransApp.h"
#include "..\XCFailedFile.h"
#include "Win7TaskbarProgress.h"
#include "..\XCQueueLegacyOption.h"

class CMainDialog :
	public CptDialog ,public CXCUIWnd,public CXCFailedFileObserver,public CXCSettingChangingCB
{
public:
	friend class CXCTaryUIState ;
	friend class CXCNormalWndUIState ;

	CMainDialog(bool bAutoRun,int nDlgID);
	virtual ~CMainDialog(void);

	static CptString GetAppErrorString(unsigned uAppErrorCode) ;

protected:

	// speed dialog 回调事件
	virtual void OnRunTimeSettingChanged(const SRunTimeSettingDlgOption& option) ;

	// Failed File 的事件回调
	virtual void OnFailedFile_Update(const SFailedFileInfo& OldFfi,const pt_STL_vector(SFailedFileInfo)& FailedFileVer) ;
	virtual void OnFailedFile_Added(const SFailedFileInfo& ffi) ;

	// APP 事件回调
	virtual void XCUI_SetTimer(int nTimerID,int nInterval) ;
	virtual void XCUI_KillTimer(int nTimerID) ;
	virtual void XCUI_OnIni(CXCTransApp* pTransApp,CXCFailedFile* pFailedFile,const SXCUIDisplayConfig& config,const SXCCopyTaskInfo& TaskInfo) ;

	virtual void XCUI_OnRunningStateChanged(ECopyFileState NewState) ;
	virtual void XCUI_OnUIVisibleChanged(bool bVisible) ;

	virtual void XCUI_OnStatusChanged(EXCStatusType OldStatus,EXCStatusType NewStatus)  ;
	virtual void XCUI_OnVerificationResult(EXCVerificationResult result) ;
	virtual void XCUI_OnRecuriseFolder(const CptString& strSrcFolder, const CptString& strDstFolder) ;

	virtual ErrorHandlingResult XCUI_OnError(const SXCExceptionInfo& ErrorInfo) ;
	
	virtual EImpactFileBehaviorResult XCUI_OnImpactFile(const SImpactFileInfo& ImpactInfo,SImpactFileBehaviorSetting& Setting) ;

	virtual void XCUI_UpdateUI(EUpdateUIType uuit,void* pParam1,void* pParam2) ;
	virtual CptString XCUI_GetCopyOfText() const;
private:

	struct SUpdaeUISpeedInfo
	{
		CptCritiSecLock		ThreadLock ;
		bool bInMsgQueue ;
		bool bReadOrWrite ;
		unsigned int nSize ;
	};

	struct SUpdateUIFileNameInfo
	{
		CptCritiSecLock		ThreadLock ;
		bool bInMsgQueue ;
		CptString strSrcFileName ;
		CptString strDstFileName ;
	};

	struct SSateDisplayStrings
	{
		enum EMask
		{
			nSppedMask = 1<<0 ,
			nTotalFilesMask = 1<<1 ,
			nDoneFilesMask =  1<<2 ,
			nTotalSizeMask = 1<<3 ,
			nRemainSizeMask = 1<<4 ,
			nTotalTimeMask = 1<<5 ,
			nRemainTimeMask = 1<<6 ,
			nLapseTimeMask = 1<<7 ,
			nRemainFileMask = 1<<8 ,
		};

		UINT uFlag ;

		CptString strSpeed ;

		CptString strTotalFiles ;
		CptString strDoneFiles ;
		CptString strRemainFiles ;

		CptString strTotalSize ;
		CptString strRemainSize ;

		CptString strTotalTime ;
		CptString strRemainTime ;
		CptString strLapseTime ;
	};

	CptString FormatErrorString(const SXCErrorCode& ec) ;

protected:

	virtual BOOL OnInitDialog() ;
	virtual void OnTimer(int nTimerID) ;
	virtual void OnDestroy();
	virtual void OnButtonClick(int nButtonID) ;
	virtual int OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) ;

	virtual void OnPaint() ;
	virtual void OnControlStatusChanged(int nControlID,SControlStatus NewStatus) ;

	virtual bool OnCancel() ;
	virtual void OnSize(UINT uType,int nNewWidth,int nNewHeigh) ;

	void SetStartupPosition() ;

private:
//
//#define TIMER_ID_ONE_SECOND		1000
#define TIMER_ID_SHOWWINDOW				1001
#define TIMER_ID_DELAYSHOWWINDOW		1002

	bool GetSateValueDisplayString(SSateDisplayStrings& sds) ;
	bool IsSimpleUI() const ;

	void UpdateUI(EUpdateUIType uuit,void* pParam1,void* pParam2) ;

	void MinimumToTray() ;

	void ShowIniWindows() ;

private:

	HFONT m_hCurFileFont ;
	HFONT m_hToTextFont ;

	CptSkinButton		m_RunSkinButton ;
	CptSkinButton		m_ExitSkinButton ;

	CptSkinButton		m_SpeedSkinButton ;

#ifdef VERSION_PROFESSIONAL
	SSkinButtonBitmapInfo	m_UpArrowBitmapInfo ;
	SSkinButtonBitmapInfo	m_DownArrowBitmapInfo ;

	CptSkinButton			m_RetryAllFailedSkinButton ;
	CptSkinButton			m_VerifyDataSkinButton ;

	CptSkinButton			m_MoreLessSkinButton ;

	bool					m_bMoreOrLess ;
	
#endif

	CptSkinProgress		m_CurSkinProgressBar ;
	CptSkinProgress		m_TotalSkinProgressBar ;

	int					m_DialogTemplateID ;

	CSpeedDialog*		m_pSpeedDlg ;
	CXCFailedFile*		m_pFailedFile ;

	SUpdaeUISpeedInfo		m_UISpeedInfo ;
	SUpdateUIFileNameInfo	m_UIFileNameInfo ;

	CptShellTray			m_ShellTray ;

	HICON					m_hAppSmallIcon ;

	CXCGUIState*			m_pCurUIState ;

	CXCNormalWndUIState		m_NormalWndUIState ;
	CXCTaryUIState			m_TrayUIState ;

	SXCUIDisplayConfig		m_ConfigData ;
	bool					m_bSimpleUI ;
	bool					m_bIniShow ; // 是否已初始化窗口,令其能显示
	bool					m_bAutoRun ;
	bool					m_bVerifyPhase ;

	bool					m_bExit ; // 当用户点击按钮 Exit ，并确认退出后，马上做的退出标识，以便防止app 再向界面提交更新造成错误
	int						m_nMoreHeightOffset ;
	int						m_nDelayShowUICounter ;
	SptRect					m_OrginRect ;
	CXCTransApp*			m_pTransApp ;

	//bool					m_bMinimumToTray ;
	/////////////////// 2.0 的东西
	CXCFailFileListView			m_FailFileListView ;

	CWin7TaskbarProgress	m_Win7TaskBarProgress ;
	
#ifndef VERSION_PROFESSIONAL // 非专业版时
	CHyperlink			m_Hyperlink;
#endif
};

