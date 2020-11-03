/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "../Core/XCCoreDefine.h"

#include <Winioctl.h>
#include "../Common/ptThread.h"
#include "../Common/ptGlobal.h"

#include "Language\XCRes_ENU\resource.h"

#include "ptMultipleLanguage.h"
#include <map>
#include <vector>
#include <list>


#include <string>

#include "../Common/sgi_stl_alloc.h"


#define LINK_HOME_WEBSITE		_T("www.easersoft.com")
#define LINK_EMAIL_SUPPORT		_T("support@easersoft.com")

#define WM_XC_COPYDATAOCCURED		WM_USER+1 // 复制数据发生操作
#define WM_XC_BEGINCOPYONEFILE		WM_USER+2 // 开始一份文件复制
#define WM_XC_SHELLTRAY				WM_USER+3 // 托盘消息


//#define UI_BG_COLOR		RGB(181,211,255)
//#define UI_TEXT_COLOR	RGB(10,10,230)

#define TIMER_ID_ONE_SECOND		1000 // 计时器ID值

//#define DEFAULT_UISTARTUP_POSITION	0xfffffff 

//#define XC_STL_queue_t(TKey,TVal) std::queue<TKey,TVal,std::less<Tkey>,sgi_std::allocator<std::pair<const TKey&,TVal> > >

// 网站页面链接类型
enum EWebLink
{
	WEBLINK_BUY_STANDARD,
	WEBLINK_BUY_PROFESSIONAL,
	WEBLINK_HELP_SAMEFILEDLG,
	WEBLINK_HELP_TASKDLG,
	WEBLINK_HELP_WILDCARDDLG,
	WEBLINK_SITE_HOME,
	WEBLINK_SITE_GETPROEDITION,
	WEBLINK_EMAIL_SUPPORT,
};


struct SGlobalData
{
	HBITMAP hSpeedBitmapNormal ;
	HBITMAP hSpeedBitmapHover ;
	HBITMAP hSpeedBitmapDown ;

	//HBRUSH	hDlgBkBrush ;

	HCURSOR hCursorHand ;

	int		nSwapBufSize ;

	SGlobalData()
	{
		::memset(this,0,sizeof(SGlobalData)) ;
	}

	void Release()
	{
		SAFE_DELETE_GDI(hSpeedBitmapNormal) ;
		SAFE_DELETE_GDI(hSpeedBitmapHover) ;
		SAFE_DELETE_GDI(hSpeedBitmapDown) ;

		SAFE_DELETE_GDI(hCursorHand) ;
	}
};

enum EUpdateUIType
{
	UUIT_OneSecond,
	UUIT_BeginCopyOneFile, // only for verification
	//UUIT_FinishCopyOneFile,
	UUIT_CopyDataOccured, // only for verification
	//UUIT_ShowIniDelayUI, // 显示初始化时的延迟显示界面
};

class CMainDialog ;

// GUI 的状态， 包括normal window,tray
class CXCGUIState
{
public:
	CXCGUIState():m_pNextState(NULL),m_pMainDlg(NULL) {}

	virtual void UpdateUI(const EUpdateUIType uuit,void* pParam1=NULL,void* pParam2=NULL) = 0;
	virtual CptString GetCurFileName() {return _T("");}

	virtual void ProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)  = 0;
	virtual void OnEnter(CptString strCurFileName) {}
	virtual void OnLeave() {}
	//void OnStatusChanged(EXCStatusType OldStatus,EXCStatusType NewStatus) {}

	void SetParameter(CXCGUIState* pNextState,CMainDialog* pDlg) 
	{
		m_pNextState = pNextState ;
		m_pMainDlg = pDlg ;
	}

protected:
	CXCGUIState*	m_pNextState ;
	CMainDialog*	m_pMainDlg ;
};

//// ------------------任务和程序的配置部分------------------- 开始----

struct SRichCopySelection
{
	enum EFileDifferenceType
	{
		FDT_Newer = 0,
		FDT_Older = 1,
		FDT_Bigger = 2,
		FDT_Smaller = 3,
		FDT_SameTimeAndSize = 4,
		FDT_Last,
	};

	enum ESameFileOperationType
	{
		SFOT_Skip = 0,
		SFOT_Replace = 1,
		SFOT_Ask = 2,
		SFOT_Rename = 3,
		SFOT_Last,
	};

	enum ESameFileProcessType
	{
		SFPT_Skip, // 跳过
		SFPT_Replace, // 覆盖
		SFPT_Ask, // 询问
		SFPT_Rename, // 重命名
		//SFPT_ReplaceNewer, // 在 v2.1.1就不再用该类型了，其值变为 SFPT_IfCondition 类型
		SFPT_IfCondition, // 该类型为先前的SFPT_ReplaceNewer， 根据条件进行操作，即按照 EFileDifferenceType 则进行
							// 默认的情况是： Replace it if newer, otherwise ask me
	};

	// 遇到只读文件的处理类型
	enum EReadOnlyFileProcessType
	{
		ROFPT_Skip, // 跳过
		ROFPT_Ask, // 询问
		ROFPT_Replace // 覆盖，即继续移动或复制 
	};

	// 出错的处理方式
	enum EErrorProcessType
	{
		EPT_Retry, // 重试
		EPT_Exit, // 取消 (即退出程序)
		EPT_Ignore, // 忽略
		EPT_Ask, // 询问
	};
};

enum EUIType
{
	UI_Simple,
	UI_Normal,
	UI_Advance
};

// 相同文件根据条件的处理方式
struct SSameFileIfConditionInfo
{
	SRichCopySelection::EFileDifferenceType		IfCondition ;
	SRichCopySelection::ESameFileOperationType	ThenOperation ;
	SRichCopySelection::ESameFileOperationType	OtherwiseOperation ;
};

#define CONFIG_DEFAULT_SAME_FILE_NAME_PROCESS		(SRichCopySelection::SFPT_Ask)
#define CONFIG_DEFAULT_ERROR_PROCESS				(SRichCopySelection::EPT_Ignore)
#define CONFIG_DEFAULT_RETRY_FAILED_THEN_PROCESS	(SRichCopySelection::EPT_Ignore)
#define CONFIG_DEFAULT_RETRY_TIMES					1
#define CONFIG_DEFAULT_MAINDIALOG_POS_X				140  // 主窗口的默认启动位置 X
#define CONFIG_DEFAULT_MAINDIALOG_POS_Y				30 // 主窗口的默认启动位置 Y

struct SXCTaskAndConfigShare
{

	SptPoint				ptStartupPos ;
	SRichCopySelection::EErrorProcessType		epc ; // 出错时的处理方式
	SRichCopySelection::ESameFileProcessType	sfpt ; // 相同文件时的处理方式
	bool					bVerifyData ; // 是否检测数据
	bool					bShutdownAfterDone ; // 任务完成后自动关机
	SRichCopySelection::EErrorProcessType		RetryFailThen ; // 重试失败后的处理方式
	int						nRetryTimes ; // 重试次数. 当 epc 指定为重试时,该值有效
	SSameFileIfConditionInfo	sfic ; // 如果上面 sfpt 的值为 SFPT_IfCondition 时，该值才有效

	SXCTaskAndConfigShare()
	{
		this->SetDefaultValue() ;
	}

	void SetDefaultValue()
	{
		bVerifyData = false ;
		bShutdownAfterDone = false ;
		ptStartupPos.nX = CONFIG_DEFAULT_MAINDIALOG_POS_X ;
		ptStartupPos.nY = CONFIG_DEFAULT_MAINDIALOG_POS_Y ;
		epc = CONFIG_DEFAULT_ERROR_PROCESS ;
		sfpt = CONFIG_DEFAULT_SAME_FILE_NAME_PROCESS ;

		RetryFailThen = CONFIG_DEFAULT_RETRY_FAILED_THEN_PROCESS ;
		nRetryTimes = CONFIG_DEFAULT_RETRY_TIMES ;

		sfic.IfCondition = SRichCopySelection::FDT_Newer ;
		sfic.ThenOperation = SRichCopySelection::SFOT_Replace ;
		sfic.OtherwiseOperation = SRichCopySelection::SFOT_Ask ;
	}
};

// 任务描述结构
struct SXCCopyTaskInfo
{
	enum EXCCmd
	{
		XCMD_Unknown, // 未知
		XCMD_Null, // 空参数
		XCMD_Register, // 输入注册码
		XCMD_About, // about
		XCMD_XtremeRun, // 极速运行
		XCMD_Shell, // SHELL 启动的
		XCMD_TaskDlg, // 任务对话框
		XCMD_Config, // 配置
		XCMD_CheckUpdate, // 检查更新
	};

	// 执行操作的类型
	enum EExecuteType
	{
		RT_Copy = 20, // 复制
		RT_Move // 移动
	};

	enum EShowMode
	{
		ST_Window, // 窗口
		ST_Tray // 托盘
	};

	// 任务由哪个功能创建的
	enum ETaskCreatedByType
	{
		TCT_TaskDialog, // 由任务对话框处创建的任务
		TCT_ShellExtension, // SHELL 处创建的任务
	};

	EExecuteType CopyType ; // 运行类型

	pt_STL_vector(CptString) strSrcFileVer ;
	pt_STL_vector(CptString) strDstFolderVer ;
	EXCCmd cmd ;

	CptString				strLogFile ;
	EShowMode				ShowMode ;
	CptString				strFinishEvent ; // 任务完成时可以触发的事件
	ETaskCreatedByType		CreatedBy ; // 该参数目前用作 verification 的设置以哪个为准，而进行判断该任务是由哪个创建的

	SXCTaskAndConfigShare		ConfigShare ; // 和 SConfigData 有着相同类型的设置数据，这是便于 shell extension 直接赋值

	SXCCopyTaskInfo()
	{
		this->SetDefaultValue() ;
	}

	void SetDefaultValue()
	{
		cmd = XCMD_Unknown ;

		strSrcFileVer.clear() ;
		strDstFolderVer.clear() ;
		strLogFile = _T("") ;

		CopyType = RT_Copy ;
		CreatedBy = TCT_TaskDialog ;

		ShowMode = SXCCopyTaskInfo::ST_Window ;

		strFinishEvent = _T("") ;
	}
};

struct SConfigData
{
	bool bDefaultCopying ; // 是否把ExtremeCopy 设置为默认的文件复制器
	bool bPlayFinishedSound ; // 文件复制完后播放声音
	bool bTopMost ;				// 窗口是否总是最前端显示
	CptString strSoundFile ;	// 结束后要播放的声音文件
	EUIType	UIType ;			// 界面类型
	int		nCopyBufSize ;		// 文件数据交换缓冲区的大小
	bool	bMinimumToTray ;	// 最小化到托盘
	int		nMaxFailedFiles ;	// 最多失败的文件数
	bool	bWriteLog ;			// 是否写日志
	bool	bCloseWindowAfterDone ; //如果检测完数据则自动关闭窗口
	CptString	strResourceDLL ; // 使用的语言包
	bool	bAutoUpdate ;		// 自动检查最新版本
	bool	bAutoQueueMultipleTask ; // 多任务时自动排队
	time_t uLastCheckUpdateTime ; // 上次检查最新版本的时间

	SConfigData()
	{
		bDefaultCopying = true ;
		bPlayFinishedSound = false ;
		bTopMost = true ;
		UIType = UI_Simple ;
		nCopyBufSize = 32 ;
		bMinimumToTray = true ;
		nMaxFailedFiles = 1000 ;
		bWriteLog = false ;
		bCloseWindowAfterDone = true ;

		bAutoUpdate = true ;
		bAutoQueueMultipleTask = true ;
		uLastCheckUpdateTime = 0 ;
	}
};

// log日志中配置的字段
//struct SLogConfigData
//{
//	unsigned int totalBoolConfig ;
//	EUIType		UIType ;			// 界面类型
//	BYTE		nCopyBufSize ;		// 文件数据交换缓冲区的大小
//	int		nMaxFailedFiles ;	// 最多失败的文件数
//
//	SXCTaskAndConfigShare ShareConfig ;
//
//	
//};

//// ------------------任务和程序的配置部分------------------- 结束

// 失败文件状态类型
enum EFailFileStatusType
{
	EFST_Unknown ,
	EFST_Failed,
	EFST_Waitting,
	EFST_Running,
	EFST_Success,
};

// 遇到相同冲突类型的文件时做法
struct SImpactFileBehaviorSetting
{
	bool	bActived ; // 该结构体信息是否活动。 
						//当'same file name dialog' 弹出来后，
						// 如果用户选择了诸如： replace all, skip all 之类的
						// app 应该记住这个选择，以便下次遇到 same file name 时不用再弹出这个对话框
						// 从而知道应如何操作。 该域就是用作是否用户做了选择的设置

	bool bApplyForReadOnly ;
	ECopyBehavior Behavior ;
	int SkipCondition ; // SameFileCondition_SameSize 
						// SameFileCondition_SameCreateTime
						// SameFileCondition_SameModifyTime

	SImpactFileBehaviorSetting():bActived(false)
	{
	}
};

// 失败文件信息描述结构
struct SFailedFileInfo
{
	int			nIndex ;
	unsigned	uFileID ;
	EFailFileStatusType		Status ;
	CptString	strSrcFile ;
	CptString	strDstFile ;
	SXCErrorCode	ErrorCode ;

	SFailedFileInfo()
	{
		nIndex = 0 ;
		uFileID = 0 ;
		Status = EFST_Unknown ;
		strSrcFile = _T("") ;
		strDstFile = _T("") ;
	}
};

// 这是用作更新XCTransApp 以 UUIT_OneSecond 命令回调到界面的参数的结构体
struct SXCUIOneSecondUpdateDisplay
{
	CptString	strCurSrcFileName ; // 当前源文件名
	CptString	strCurDstFileName ;	// 当前目标文件名
	//int			nPercentOfCurFile ; // 当前文件的进度百分比，所以该值最大是100，最小是0
};

struct SXCUIDisplayConfig
{
	bool	bTopMost ;
	bool	bMinimumToTray ;
	bool	bVerifyData ;
	bool	bShutDown ;
	bool	bCloseWindow ;
	SXCCopyTaskInfo::EShowMode	ShowMode ;
	int		nMaxIgnoreFailedFiles ;
	SptPoint	DlgStartupPos ;

	SXCUIDisplayConfig()
	{
	}
};

enum EXCStatusType
{
	XCST_Unkown,
	XCST_Ready,
	XCST_Copying,
	XCST_Pause,
	XCST_Verifying,
	XCST_Finished,
};

enum EHelpFilePage
{
	HFP_MainMenu,
	HFP_SameFileNameDlg,
	HFP_ConfigurationDlg,
	HFP_TaskEditDlg,
	HFP_WildcardDlg,
	HFP_TaskQueue,
	HFP_MainDlg,
};

enum EXCVerificationResult
{
	XCST_Same,
	XCST_Different,
};

class CXCTransApp ;
class CXCFailedFile ;

class CXCUIWnd
{
public:
	CXCUIWnd(){}

	virtual void XCUI_SetTimer(int nTimerID,int nInterval) {}
	virtual void XCUI_KillTimer(int nTimerID) {}

	virtual void XCUI_OnRunningStateChanged(ECopyFileState NewState) {}
	virtual void XCUI_OnUIVisibleChanged(bool bVisible) {}
	virtual ErrorHandlingResult XCUI_OnError(const SXCExceptionInfo& ErrorInfo) = 0 ;

	virtual EImpactFileBehaviorResult XCUI_OnImpactFile(const SImpactFileInfo& ImpactInfo,SImpactFileBehaviorSetting& Setting) = 0 ;

	virtual void XCUI_OnIni(CXCTransApp* pTransApp,CXCFailedFile* pFailedFile,const SXCUIDisplayConfig& config,const SXCCopyTaskInfo& TaskInfo) {}

	virtual void XCUI_UpdateUI(EUpdateUIType uuit,void* pParam1,void* pParam2) {}
	
	virtual CptString XCUI_GetCopyOfText() const {return _T("Copy Of");}

	virtual void XCUI_OnStatusChanged(EXCStatusType OldStatus,EXCStatusType NewStatus)  {}
	virtual void XCUI_OnVerificationResult(EXCVerificationResult result) {}
	virtual void XCUI_OnRecuriseFolder(const CptString& strSrcFolder, const CptString& strDstFolder) {}
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

CptString GetSizeString(unsigned __int64 nValue) ;

bool ConfirmExitApp(HWND hParentWnd=NULL) ;
unsigned __int64 DoubleWordTo64(const DWORD dwLow,const DWORD dwHight) ;

void TransparenWnd(HWND hWnd,int nPercent) ;

bool CheckRegisterCode(const CptStringList& sl) ;
void GetRegisterCode(CptStringList& sl) ;

void OnHyperLinkHomePageCallBack2(void* pVoid);

bool DoesIncludeRecuriseFolder(const SXCCopyTaskInfo& sti,int& nSrcIndex, int& nDstIndex) ;
CptString MakeXCVersionString() ;
int SearchResourceDLL(pt_STL_map(std::basic_string<TCHAR>,std::basic_string<TCHAR>)& LanguageName2DLLNameMap,CptString strFolder) ;

enum ETaskXMLErrorType ;
CptString GetTaskXMLErrorString(const ETaskXMLErrorType& ErrorCode) ;

bool OpenLink(const TCHAR* pLink) ;
bool OpenLink(const EWebLink LinkType) ;
bool IsDstRenameFileName(const SGraphTaskDesc& gtd) ;

bool IsRecycleFile(const CptString& strFile,CptString& strOriginName) ;

void LaunchHelpFile(const EHelpFilePage hfp) ;



