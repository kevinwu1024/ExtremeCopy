/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCTransApp.h"
#include <process.h>
#include <set>
#include <vector>
#include <shlobj.h>
#include <Mmsystem.h>
#include "XCVerifyResult.h"
#include "..\Common\ptPerformanceCalcator.h"
#include <shellapi.h>
#include "..\Core\XCWinStorageRelative.h"
#include "ui\XCTaskQueueDlg.h"
#include "XCQueueLegacyOption.h"

#pragma comment(lib,"Winmm.lib")

#ifdef COMPILE_TEST_PERFORMANCE
DWORD	g_dwStartTick = 0 ;
#endif

CXCTransApp::CXCTransApp(const SConfigData& config,const SXCCopyTaskInfo& sti):
m_pUIWnd(NULL),
m_hCopyThread(NULL),
m_pCurActiveFile(NULL),
m_pXCSta(NULL),
m_nLastSpeedRate(100),
m_nSpeed(100),
m_CopyTask(sti),
m_nRemainRetryCount(m_CopyTask.ConfigShare.nRetryTimes),
m_pCurFailedFile(NULL),
m_CurStatus(XCST_Ready),
m_nCurFdoListIndex(0),
m_uCurUIFileID(0),
m_LastUIOnceSecondFileID(0),
m_hClearStopEvent(NULL),
m_hLaunchCompleteEvent(NULL)
{
	m_ConfigData = config ;

	// 传送队列共享设置
	// 如果前面的队列里的任务已设置 “关闭计算机”，则将把该设置应用到当前的任务
	m_CopyTask.ConfigShare.bShutdownAfterDone = CXCQueueLegacyOption::GetOption(QLO_MASK_ShutdownAfterTaskDone) ;

	m_LogFile.SetEnable(m_ConfigData.bWriteLog) ;

#ifdef VERSION_PROFESSIONAL
	m_pFailedFile = NULL ;
#endif
}

CXCTransApp::~CXCTransApp(void)
{
#ifdef VERSION_PROFESSIONAL
	m_TaskQueue.RemoveFromQueue() ; // 
#endif

	this->Release() ;
}


// 处理复制出错事件
ErrorHandlingResult CXCTransApp::ProcessCopyError(SRichCopySelection::EErrorProcessType ept, const SXCExceptionInfo& ErrorInfo) 
{
	ErrorHandlingResult nRet = ErrorHandlingFlag_Exit ;

	if(ept!=SRichCopySelection::EPT_Exit && ErrorInfo.ErrorCode.nSystemError==ERROR_DISK_FULL)
	{// 如果磁盘空间不够，那么必定以询问形式弹出
		SXCExceptionInfo ei = ErrorInfo ;

		ei.SupportType = ErrorHandlingFlag_Exit | ErrorHandlingFlag_Retry ;

		this->StopOneSecondTimer() ;

		nRet = m_pUIWnd->XCUI_OnError(ei) ;

		this->StartOneSecondTimer() ;

		return nRet ;
	}

	switch(ept)
	{
	case SRichCopySelection::EPT_Retry: // 重试
		if(m_nRemainRetryCount>0)
		{
			--m_nRemainRetryCount ;
			return ErrorHandlingFlag_Retry ;
		}
		else
		{
			if(m_CopyTask.ConfigShare.RetryFailThen!=SRichCopySelection::EPT_Retry)
			{// 重试超过指定次数的话,那么接着按另外的处理
				nRet = this->ProcessCopyError(m_CopyTask.ConfigShare.RetryFailThen,ErrorInfo) ;
			}

			m_nRemainRetryCount = m_CopyTask.ConfigShare.nRetryTimes ;
		}
		break ;

	case SRichCopySelection::EPT_Ask: // 询问
		{
			if(m_pUIWnd!=NULL)
			{
				this->StopOneSecondTimer() ;

				nRet = m_pUIWnd->XCUI_OnError(ErrorInfo) ;

				this->StartOneSecondTimer() ;
			}
			else
			{
			}
		}
		break ;

	case SRichCopySelection::EPT_Exit:// 退出
		{
			if(m_pUIWnd!=NULL)
			{
				this->StopOneSecondTimer() ;

				SXCExceptionInfo ErrorInfo2 = ErrorInfo ;

				ErrorInfo2.SupportType = ErrorHandlingFlag_Exit ;
				m_pUIWnd->XCUI_OnError(ErrorInfo2) ;

				this->StartOneSecondTimer() ;
			}
			
			nRet = ErrorHandlingFlag_Exit ;
		}
		break ;

	case SRichCopySelection::EPT_Ignore:// 忽略
		{
#ifdef VERSION_PROFESSIONAL
			if(m_pFailedFile->GetFailedFileCount()<m_ConfigData.nMaxFailedFiles)
			{
				// 添加到 FailedFileView 里去
				m_XCCopyEvent.XCOperation_RecordError(ErrorInfo) ;

				nRet = ErrorHandlingFlag_Ignore ;
			}
			else
			{// 超过一指定次数后,则询问
				m_CopyTask.ConfigShare.epc = SRichCopySelection::EPT_Ask ; // 修改失败时的处理方式为: 询问
				nRet = this->ProcessCopyError(SRichCopySelection::EPT_Ask,ErrorInfo) ;
			}
#else
			nRet = this->ProcessCopyError(SRichCopySelection::EPT_Ask,ErrorInfo) ;
#endif
		}
		
		break ;
	}
	
	return nRet ;
}



void CXCTransApp::Run()
{
	m_CurRunState = ARS_ReadyToRun ;

	if(m_pUIWnd!=NULL)
	{
		m_pUIWnd->XCUI_OnUIVisibleChanged(true) ;
	}
}

bool CXCTransApp::Ini(CXCUIWnd* pUIWnd,CXCStatistics* pSta,CXCFailedFile* pFailedFile)
{
	_ASSERT(pUIWnd!=NULL) ;
	_ASSERT(pSta!=NULL) ;
	_ASSERT(pFailedFile!=NULL) ;

	m_pXCSta = pSta ; 

	m_pUIWnd = pUIWnd ;
	
#ifdef _DEBUG
	m_LogFile.SetEnable(false) ;
#else
	m_LogFile.SetEnable(this->m_ConfigData.bWriteLog) ;
#endif

#ifdef VERSION_PROFESSIONAL
	m_pFailedFile = pFailedFile ;

	// 是否对多任务进行自动排队
	if(m_ConfigData.bAutoQueueMultipleTask)
	{
		if(!this->WaitForHDQueue(m_CopyTask))
		{// 队列等期间用户选择退出    
			return false ;
		}
	}
	
#endif
	
	if(m_pUIWnd!=NULL)
	{
		// 从合适的配置来源来指定界面的显示设置
		SXCUIDisplayConfig config ;

		config.bMinimumToTray = m_ConfigData.bMinimumToTray ;
		config.bTopMost = m_ConfigData.bTopMost ;
		config.ShowMode = m_CopyTask.ShowMode ;
		config.nMaxIgnoreFailedFiles = m_ConfigData.nMaxFailedFiles ;
		config.DlgStartupPos = m_CopyTask.ConfigShare.ptStartupPos ;
		config.bCloseWindow = m_ConfigData.bCloseWindowAfterDone ;
		config.bShutDown = m_CopyTask.ConfigShare.bShutdownAfterDone;

		m_pUIWnd->XCUI_OnIni(this,pFailedFile, config, m_CopyTask) ;

		m_strCopyOfText = m_pUIWnd->XCUI_GetCopyOfText() ;
	}

	return true ;
}

void CXCTransApp::Continue() 
{
	_ASSERT(m_CurRunState == ARS_ReadyToRun || m_CurRunState == ARS_Pause) ;

	if(m_CurRunState == ARS_ReadyToRun)
	{// 初次的启动
		EXCStatusType  OldStatus = m_CurStatus ;
		m_CurStatus = XCST_Copying ;
		m_pUIWnd->XCUI_OnStatusChanged(OldStatus,m_CurStatus) ;

		this->ExecuteTask() ;
	}
	else
	{// 暂停后的启动
		if(m_CurRunState==ARS_Pause)
		{
			m_CurRunState= ARS_StandardRunning ;

			EXCStatusType  OldStatus = m_CurStatus ;
			m_CurStatus = XCST_Copying ;
			m_pUIWnd->XCUI_OnStatusChanged(OldStatus,m_CurStatus) ;

			m_pUIWnd->XCUI_OnRunningStateChanged(CFS_Running) ;

			this->m_XCCore.Continue() ;
			this->StartOneSecondTimer() ;
		}
	}

	m_LogFile.TaskRunningStateChanged(1) ;
}

bool CXCTransApp::SkipCurrentFile() 
{
	_ASSERT(ARS_Pause==m_CurRunState) ;

	if(m_CurRunState==ARS_Pause)
	{// 因为在抛弃前，程序必然是先进入暂停状态
		return m_XCCore.Skip() ;
	}

	return false ;
}

void CXCTransApp::Pause() 
{
	if(m_CurRunState==ARS_StandardRunning)
	{
		EXCStatusType  OldStatus = m_CurStatus ;
		m_CurStatus = XCST_Pause ;
		m_pUIWnd->XCUI_OnStatusChanged(OldStatus,m_CurStatus) ;

		this->StopOneSecondTimer() ;
		m_CurRunState=ARS_Pause ;
		m_LogFile.TaskRunningStateChanged(0) ;
		m_pUIWnd->XCUI_OnRunningStateChanged(CFS_Pause) ;
		m_XCCore.Pause() ;
	}
}

void CXCTransApp::Stop() 
{
	if(m_CurRunState==ARS_Pause)
	{
		this->m_XCCore.Continue() ;
	}

	m_CurRunState = ARS_Exit ;

	if(NULL!=m_hLaunchCompleteEvent)
	{
		::WaitForSingleObject(m_hLaunchCompleteEvent,1000) ;
	}

	m_XCCore.Stop() ;
	m_pXCSta->Stop() ;

	if(m_pUIWnd!=NULL)
	{
		m_pUIWnd->XCUI_OnRunningStateChanged(CFS_Stop) ;
	}

	this->Release() ;
}

void CXCTransApp::Release()
{
	if(ARS_Exit!=m_CurRunState)
	{
		m_LogFile.TaskRunningStateChanged(2) ;

		m_CurRunState = ARS_Exit ;

		m_pXCSta->Stop() ;

		if(m_hCopyThread!=NULL)
		{
			::WaitForSingleObject(m_hCopyThread,3*1000) ;
			m_hCopyThread = NULL ;
		}
	}

	if(NULL!=m_hLaunchCompleteEvent)
	{
		::CloseHandle(m_hLaunchCompleteEvent) ;
		m_hLaunchCompleteEvent = NULL ;
	}
}

// UI 回调事件
void CXCTransApp::OnUITimer(int nTimerID) 
{
	if(nTimerID==TIMER_ID_ONE_SECOND)
	{
		this->m_pXCSta->MoveNextSecond() ;

		SXCUIOneSecondUpdateDisplay osud ;
		this->GetUIOneSecondData(osud) ;

		m_pUIWnd->XCUI_UpdateUI(UUIT_OneSecond,&osud,NULL) ;

#ifdef VERSION_PROFESSIONAL

		// 更新当前任务剩余时间，以便随后的等待任务能得知剩余时间
		const SStatisticalValue& sv = m_pXCSta->GetStaData() ;
		if(sv.uTotalSize>0 && sv.uTotalSize>sv.uTransSize && sv.fSpeed>0.1f)
		{
			m_TaskQueue.UpdateRunningTaskRemainTime((DWORD)((sv.uTotalSize-sv.uTransSize)/sv.fSpeed)) ;
		}
		else
		{
			m_TaskQueue.UpdateRunningTaskRemainTime(-1) ;
		}
#endif
	}
}


void CXCTransApp::ExecuteTask()
{
	m_hCopyThread = (HANDLE)::_beginthreadex(NULL,0,CopyThreadFunc,this,0,NULL) ;

	if(m_hCopyThread==NULL)
	{// 创建线程失败s
	}
}

const CXCStatistics* CXCTransApp::GetStatistics() const
{
	return m_pXCSta ;
}

CptString CXCTransApp::GetCurWillSkipFile() 
{
	return CptGlobal::MakeUnlimitFileName(m_XCCore.GetCurWillSkipFile(),false) ;
}


const SXCCopyTaskInfo* CXCTransApp::GetTaskInfo() const 
{
	return &m_CopyTask ;
}

bool CXCTransApp::ExecuteTask2(const SXCCopyTaskInfo& TaskInfo) 
{
	if(m_pUIWnd!=NULL)
	{
		m_pUIWnd->XCUI_OnRunningStateChanged(CFS_Running) ;
	}

	EXCStatusType  OldStatus = m_CurStatus ;
	m_CurStatus = XCST_Copying ;
	m_pUIWnd->XCUI_OnStatusChanged(OldStatus,m_CurStatus) ;

#ifdef COMPILE_TEST_PERFORMANCE
	g_dwStartTick = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif

	SGraphTaskDesc gtd ;

	gtd.SrcFileVer = TaskInfo.strSrcFileVer ;
	gtd.DstFolderVer = TaskInfo.strDstFolderVer ;
	
	gtd.ExeType = TaskInfo.CopyType==SXCCopyTaskInfo::RT_Move ? XCTT_Move : XCTT_Copy ;
	gtd.HashType = FHT_UNKNOWN ;
	gtd.pCopyingEvent = &m_XCCopyEvent ;
	gtd.nFileDataBufSize = m_ConfigData.nCopyBufSize*(1024*1024) ;

	{// 判断目标文件夹是否存在
		gtd.bIsRenameDst = ::IsDstRenameFileName(gtd) ;

		if(!gtd.bIsRenameDst)
		{
			DWORD dwAttr = 0 ;
			for(size_t i=0;i<gtd.DstFolderVer.size();++i)
			{
				dwAttr = ::GetFileAttributes(gtd.DstFolderVer[i].c_str()) ;
				if(dwAttr==INVALID_FILE_ATTRIBUTES || !CptGlobal::IsFolder(dwAttr))
				{
					if(m_pUIWnd!=NULL)
					{
						SXCExceptionInfo error ;

						error.ErrorCode.nSystemError = 2 ;// The system cannot find the file specified. 
						error.SupportType = ErrorHandlingFlag_List ;
						error.strDstFile = gtd.DstFolderVer[i] ;

						m_pUIWnd->XCUI_OnError(error) ;
					}

					return false ;
				}
			}
		}
	}

	this->StartOneSecondTimer() ;

	// 开始统计工作
	if(NULL!=m_pXCSta)
	{
		if(gtd.ExeType==XCTT_Move && gtd.DstFolderVer.size()==1)
		{// 在同一分区内移动文件

			SStorageInfoOfFile SrcSiof ;

			SStorageInfoOfFile DstSiof ;

			CXCWinStorageRelative::GetFileStoreInfo(gtd.DstFolderVer[0],DstSiof) ;
			size_t i = 0 ;

			SGraphTaskDesc NormalGtd = gtd;

			NormalGtd.SrcFileVer.clear() ;

			for( i=0;i<gtd.SrcFileVer.size();++i)
			{
				if(CXCWinStorageRelative::GetFileStoreInfo(gtd.SrcFileVer[i],SrcSiof))
				{
					if(SrcSiof.dwStorageID!=DstSiof.dwStorageID 
						|| SrcSiof.nPartitionIndex!=DstSiof.nPartitionIndex
						|| SrcSiof.nSectorSize!=DstSiof.nSectorSize
						|| SrcSiof.uDiskType!=DstSiof.uDiskType)
					{
						NormalGtd.SrcFileVer.push_back(gtd.SrcFileVer[i]) ;
					}
				}
			}

			if(!NormalGtd.SrcFileVer.empty())
			{
				m_pXCSta->Start(NormalGtd.SrcFileVer,this) ;
			}
			
		}
		else
		{
			m_pXCSta->Start(gtd.SrcFileVer,this) ;
		}
	}

	if(ARS_Exit==m_CurRunState)
	{// 如果用户此时已点击退出，则直接退出，而不再进行复制操作
		this->StopOneSecondTimer() ;
		::SetEvent(m_hLaunchCompleteEvent) ;
		return false ;
	}

	::SetEvent(m_hLaunchCompleteEvent) ;

	bool bRunResult = m_XCCore.Run(gtd) ;

	m_XCCore.Stop() ;

	this->StopOneSecondTimer() ;

	//for(int i=0;i<2;++i)
	{// 把最后一次的统计数据回调上去，使其补充完整
		SXCUIOneSecondUpdateDisplay osud ;
		this->GetUIOneSecondData(osud) ;

		m_pUIWnd->XCUI_UpdateUI(UUIT_OneSecond,&osud,(void*)TRUE) ;
	}

#ifdef COMPILE_TEST_PERFORMANCE
	Release_Printf(_T("start took time: %.2f seconds"),CptPerformanceCalcator::GetInstance()->GetCalInSecond(0,true)) ;
	Release_Printf(_T("read took time: %.2f seconds"),CptPerformanceCalcator::GetInstance()->GetCalInSecond(1,true)) ;
	Release_Printf(_T("write took time: %.2f seconds"),CptPerformanceCalcator::GetInstance()->GetCalInSecond(2,true)) ;
	//
	Release_Printf(_T("open read took time: %.2f seconds"),CptPerformanceCalcator::GetInstance()->GetCalInSecond(4,true)) ;
	Release_Printf(_T("create write file took time: %.2f seconds"),CptPerformanceCalcator::GetInstance()->GetCalInSecond(5,true)) ;
	//Release_Printf(_T("create write folder took time: %.2f seconds"),CptPerformanceCalcator::GetInstance()->GetCalInSecond(11,true)) ;
	Release_Printf(_T("round off took time: %.2f seconds"),CptPerformanceCalcator::GetInstance()->GetCalInSecond(6,true)) ;
	Release_Printf(_T("close read took time: %.2f seconds"),CptPerformanceCalcator::GetInstance()->GetCalInSecond(7,true)) ;
	//Release_Printf(_T("close read 2 took time: %.2f seconds"),CptPerformanceCalcator::GetInstance()->GetCalInSecond(70,true)) ;
	Release_Printf(_T("folder entry took time: %.2f seconds"),CptPerformanceCalcator::GetInstance()->GetCalInSecond(12,true)) ;

#endif
	

	// 在这里以task dialog创建的任务为优先级别去判断是否检查数据
	bool bVerify = m_CopyTask.ConfigShare.bVerifyData ; 

	if(bRunResult && gtd.ExeType==XCTT_Copy && bVerify && m_CurRunState!=ARS_Exit)
	{// 诊断复制文件是否完全正确
		CXCVerifyResult vr ;
		CXCVerifyResult::SVerifyPar vp ;

		CXCFileDataBuffer* pFDB = m_XCCore.GetFileDataBuf() ;

		vp.SrcVer = gtd.SrcFileVer ;
		vp.DstVer = gtd.DstFolderVer ;

		vp.pBuf = (BYTE*)pFDB->Allocate(1,pFDB->GetChunkSize()) ;
		vp.nBufSize = pFDB->GetChunkSize() ;
		vp.pExceptionCB = this ;
		vp.pChangingBuf = &this->m_FileChangingBuf ;

		 OldStatus = m_CurStatus ;
		m_CurStatus = XCST_Verifying ;
		m_pUIWnd->XCUI_OnStatusChanged(OldStatus,m_CurStatus) ;
		
		if(vr.Run(vp))
		{
			m_pUIWnd->XCUI_OnVerificationResult(XCST_Same) ;
		}
		else
		{
			m_pUIWnd->XCUI_OnVerificationResult(XCST_Different) ;
		}
	}

	OldStatus = m_CurStatus ;
	m_CurStatus = XCST_Finished ;
	m_pUIWnd->XCUI_OnStatusChanged(OldStatus,m_CurStatus) ;

	return bRunResult ;
}

void CXCTransApp::OnStatEventFolderSizeCompleted()
{
	// 当源文件总大小计算完后，则判断目标分区空间是否足够大
	const SStatisticalValue& sta = m_pXCSta->GetStaData() ;

	SStorageInfoOfFile siof ;

	for(size_t i=0;i<m_CopyTask.strDstFolderVer.size();++i)
	{
		CXCWinStorageRelative::GetFileStoreInfo(m_CopyTask.strDstFolderVer[i],siof) ;

		switch(siof.uDiskType)
		{
		case DRIVE_FIXED:
		case DRIVE_RAMDISK:
		case DRIVE_REMOVABLE:
			{
				ULARGE_INTEGER FreeSize ;

				FreeSize.QuadPart = 0 ;
				if(::GetDiskFreeSpaceEx(m_CopyTask.strDstFolderVer[i].c_str(),&FreeSize,NULL,NULL))
				{
					// 如果剩下的文件总大小比目标分区还要大，那么则提示用户清理磁盘空间
					if(sta.uTotalSize-sta.uDoneSize>FreeSize.QuadPart)
					{// space not enough, error code is 112 
						
						// 强制设为运行状态，以便任务能够暂停下来
						
						if(m_CurRunState==ARS_StandardRunning )
						{// 这时候 core 还没有完全启动，所以应等其完全启动后再暂停
							int nWaitCounter = 10*1000 ;

							while(nWaitCounter>0 && m_XCCore.GetState()==CFS_Stop)
							{
								::Sleep(50) ;
								nWaitCounter -= 50 ;
							}
						}

						this->Pause() ;

						SXCExceptionInfo ei ;

						ei.ErrorCode.nSystemError = 112 ;
						ei.SupportType = ErrorHandlingFlag_Exit | ErrorHandlingFlag_Ignore ;
						ei.strDstFile = m_CopyTask.strDstFolderVer[i] ;

						if(m_pUIWnd->XCUI_OnError(ei)==ErrorHandlingFlag_Exit)
						{
							this->Stop() ;
						}
						else
						{
							this->Continue() ;
						}
					}
				}
			}
			break ;

		default: break ;
		}
	}
}

#ifdef VERSION_PROFESSIONAL
bool CXCTransApp::WaitForHDQueue(const SXCCopyTaskInfo& cti)
{
	bool bRet = true ;

	pt_STL_set(DWORD) StorageIDSet ;

	SStorageInfoOfFile Siof ;
	
	// 提取当前任务所涉及的所有HD 的ID
	for(int i=0;i<2;++i)
	{
		const pt_STL_vector(CptString)& FileVer = (i==0 ? cti.strSrcFileVer : cti.strDstFolderVer) ;

		pt_STL_vector(CptString)::const_iterator it = FileVer.begin() ;

		for(;it!=FileVer.end();++it)
		{
			if(CXCWinStorageRelative::GetFileStoreInfo((*it),Siof))
			{
				if(StorageIDSet.find(Siof.dwStorageID)==StorageIDSet.end())
				{
					StorageIDSet.insert(Siof.dwStorageID) ;
				}
			}
		}
	}

	if(!StorageIDSet.empty())
	{// 把这些 HD ID 加入到任务队列里
		pt_STL_vector(DWORD) StorageIDVer ;

		pt_STL_set(DWORD)::const_iterator it2 = StorageIDSet.begin() ;

		for(;it2!=StorageIDSet.end();++it2)
		{
			StorageIDVer.push_back(*it2) ;
		}

		int nQuePos = m_TaskQueue.AddToQueue(StorageIDVer) ;

		if(nQuePos>0)
		{// 则说明前面有任务在运行，此时应该进入等待状态，直到轮到用户或用户强制运行任务
			CXCTaskQueueDlg dlg(&m_TaskQueue,cti.strSrcFileVer,cti.strDstFolderVer) ;

			dlg.ShowDialog() ;

			bRet = (dlg.GetTaskQueueResult()!=CXCTaskQueueDlg::TQRT_ForceExit) ;
		}
	}
	
	return bRet ;
}

#endif

unsigned int CXCTransApp::CopyThreadFunc(void* pParam)
{
	CXCTransApp* pThis = (CXCTransApp*)pParam ;

	pThis->CopyWork() ;

	if(pThis->m_pUIWnd!=NULL)
	{
		pThis->m_pUIWnd->XCUI_OnRunningStateChanged(CFS_Stop) ;
	}

	pThis->m_hCopyThread = NULL ;

	return 0 ;
}

void CXCTransApp::OnFailedFile_Update(const SFailedFileInfo& OldFfi,const pt_STL_vector(SFailedFileInfo)& FailedFileVer) 
{
	_ASSERT(OldFfi.nIndex>=0) ;
	_ASSERT(OldFfi.nIndex<(int)FailedFileVer.size()) ;

	if(m_CurRunState==ARS_FailedRestore)
	{
		if(OldFfi.Status==EFST_Failed && FailedFileVer[OldFfi.nIndex].Status==EFST_Waitting)
		{// 用户点击了界面，并对该文件进行恢复操作
			if(OldFfi.strDstFile.GetLength()>0 && OldFfi.strSrcFile.GetLength()>0)
			{
				m_FailedFileMsgQue.PostMsg((void*)(OldFfi.nIndex+1)) ;
			}
		}
		else if(OldFfi.Status==EFST_Waitting && FailedFileVer[OldFfi.nIndex].Status==EFST_Failed)
		{// 用户点击了界面，并对该文件进行取消操作
			m_FailedFileMsgQue.RemoveMsg((void*)(OldFfi.nIndex+1)) ;
		}
	}
}

// 把文件名转换成不限长度的名
bool CXCTransApp::MakeUnlimitedFileName() 
{
	CptString strTem ;
	bool bRenew = false ;

	for(size_t i=0;i<m_CopyTask.strSrcFileVer.size();++i)
	{
		strTem = m_CopyTask.strSrcFileVer[i] ;
		m_CopyTask.strSrcFileVer[i] = CptGlobal::MakeUnlimitFileName(strTem,true) ;

		if(m_CopyTask.strSrcFileVer[i].GetLength()==0)
		{// 若文件名有误
			SXCExceptionInfo ei ;

			ei.strSrcFile = strTem ;
			//ei.strDstFile = m_CopyTask.strDstFolderVer[0] ;
			ei.SupportType = ErrorHandlingFlag_Exit | ErrorHandlingFlag_Ignore ;
			ei.uFileID = 0 ;
			ei.ErrorCode.nSystemError = 0 ;
			ei.ErrorCode.AppError = CopyFileErrorCode_InvaliableFileName ;

			switch(m_XCCopyEvent.XCOperation_CopyExcetption(ei))
			{
			case ErrorHandlingFlag_Ignore:
				bRenew = true ;
				m_XCCopyEvent.XCOperation_RecordError(ei) ;
				break ;

			default:
			case ErrorHandlingFlag_Exit: 
				//*this->m_pRunningState = CFS_Stop ;
				return false;

			}
		}
	}

	if(bRenew)
	{// 若有文件需要抛弃
		pt_STL_vector(CptString) strTemVer ;

		for(size_t i=0;i<m_CopyTask.strSrcFileVer.size();++i)
		{
			if(m_CopyTask.strSrcFileVer[i].GetLength()>0)
			{
				strTemVer.push_back(m_CopyTask.strSrcFileVer[i]) ;
			}
		}

		m_CopyTask.strSrcFileVer = strTemVer ;
	}

	pt_STL_vector(CptString)::iterator it = m_CopyTask.strDstFolderVer.begin() ;

	for(size_t i=0;i<m_CopyTask.strDstFolderVer.size();++i)
	{
		m_CopyTask.strDstFolderVer[i] = CptGlobal::MakeUnlimitFileName(m_CopyTask.strDstFolderVer[i],true) ;
	}

	return true ;
}

bool CXCTransApp::ExpandErrorCallback(CptString strFileName)
{
	SXCExceptionInfo ei ;
	ei.strSrcFile = strFileName ;
	ei.SupportType = ErrorHandlingFlag_Exit | ErrorHandlingFlag_Ignore ;
	ei.uFileID = 0 ;
	ei.ErrorCode.nSystemError = 0 ;
	ei.ErrorCode.AppError = CopyFileErrorCode_InvaliableFileName ;

	switch(m_XCCopyEvent.XCOperation_CopyExcetption(ei))
	{
	case ErrorHandlingFlag_Ignore:
		m_XCCopyEvent.XCOperation_RecordError(ei) ;
		break ;

	default:
	case ErrorHandlingFlag_Exit: 
		//*this->m_pRunningState = CFS_Stop ;
		return false;

	}

	return true ;
}

bool CXCTransApp::ExpandWildcardName() 
{
	pt_STL_vector(size_t) WildcardFileIndexVer ;

	for(size_t i=0;i<m_CopyTask.strSrcFileVer.size();++i)
	{
		switch(::GetInterestFileType(m_CopyTask.strSrcFileVer[i].c_str()))
		{
		case IFT_Folder:
		case IFT_File:
			break ;

		case IFT_Invalid:
			{
				// 把这里先关闭，避免和后面发现问题后，重复提交到 FailedFile

			}
			break ;

		case IFT_FolderWithWildcard:
			WildcardFileIndexVer.push_back(i) ;
			break ;
		}
	}

	if(!WildcardFileIndexVer.empty())
	{
		pt_STL_vector(CptString) strFileVer ;

		int nWildcardIndex = 0;
		WIN32_FIND_DATA wfd ;
		CptWinPath::SPathElementInfo pei ;
		CptString strWildcardFolder ;

		for(size_t i=0;i<m_CopyTask.strSrcFileVer.size();++i)
		{
			if(WildcardFileIndexVer[nWildcardIndex]==i)
			{
				++nWildcardIndex ;

				HANDLE hFileFind = ::FindFirstFile(m_CopyTask.strSrcFileVer[i].c_str(),&wfd) ;

				if(hFileFind!=INVALID_HANDLE_VALUE)
				{
					pei.uFlag = CptWinPath::PET_Path ;

					if(CptWinPath::GetPathElement(m_CopyTask.strSrcFileVer[i].c_str(),pei))
					{
						strWildcardFolder = pei.strPath  ;
					}
					else
					{
						_ASSERT(FALSE) ;

						if(!this->ExpandErrorCallback(m_CopyTask.strSrcFileVer[i]))
						{
							return false ;
						}
					}

					bool bDotFound1 = false ;
					bool bDotFound2 = false ;

					do
					{

						if(!bDotFound1 && ::_tcscmp(wfd.cFileName,_T("."))==0)
						{
							bDotFound1 = true ;
							continue ;
						}
						else if(!bDotFound2 && ::_tcscmp(wfd.cFileName,_T(".."))==0)
						{
							bDotFound2 = true ;
							continue ;
						}
						else
						{
							CptString strTem ;

							TCHAR cLastChar = strWildcardFolder.GetAt(strWildcardFolder.GetLength()-1) ;

							if(cLastChar=='\\' || cLastChar=='/')
							{
								strTem.Format(_T("%s%s"),strWildcardFolder.c_str(),wfd.cFileName) ;
							}
							else
							{
								strTem.Format(_T("%s\\%s"),strWildcardFolder.c_str(),wfd.cFileName) ;
							}
							
							strFileVer.push_back(strTem) ;
						}
					}
					while(::FindNextFile(hFileFind,&wfd));

					::FindClose(hFileFind) ;
					hFileFind = INVALID_HANDLE_VALUE ;
				}
				else
				{
					if(!this->ExpandErrorCallback(m_CopyTask.strSrcFileVer[i]))
					{
						return false ;
					}
				}
			}
			else
			{
				strFileVer.push_back(m_CopyTask.strSrcFileVer[i]) ;
			}
		}

		m_CopyTask.strSrcFileVer.clear() ;
		m_CopyTask.strSrcFileVer = strFileVer ;
	}

	return true ;
}

void CXCTransApp::InsertWildcardToRootFolder() 
{
	pt_STL_vector(CptString)::iterator it = m_CopyTask.strSrcFileVer.begin() ;

	for(;it!=m_CopyTask.strSrcFileVer.end();++it)
	{
		TCHAR cLastChar = (*it).GetAt((*it).GetLength()-1) ;

		if(cLastChar=='\\' || cLastChar=='/')
		{
			if((*it).GetAt((*it).GetLength()-2)==':')
			{// 这是根目录，则添加 “所有文件”
				(*it) += _T("*.*") ;
			}
		}
	}
}

bool CXCTransApp::IsSpecialFolderMovement()
{
	// 源目录包含通配符或者是 根目录 则必须是展开后的
	bool bRet = false ;

	if(m_CopyTask.CopyType==SXCCopyTaskInfo::RT_Move)
	{
		int SpecialFolderIDArray[] = {CSIDL_WINDOWS,CSIDL_DESKTOP,CSIDL_APPDATA,CSIDL_COMMON_APPDATA,CSIDL_PROFILE} ;
		CptString strFolderArray[sizeof(SpecialFolderIDArray)/sizeof(int)] ;

		for(int i=0;i<sizeof(SpecialFolderIDArray)/sizeof(int);++i)
		{
			strFolderArray[i] = CptWinPath::GetSpecialPath(SpecialFolderIDArray[i]) ;
			strFolderArray[i].MakeLower() ;
		}

		pt_STL_vector(CptString)::iterator it = m_CopyTask.strSrcFileVer.begin() ;

		for(;it!=m_CopyTask.strSrcFileVer.end() && !bRet;++it)
		{
			// 这里判断 2 种情况
			// 1. 是否特殊目录本身
			// 2. 是否特殊目录的父目录

			CptString strTem = (*it) ;
			strTem.MakeLower() ;
			strTem = CptGlobal::MakeUnlimitFileName(strTem,false) ;

			for(int i=0;i<sizeof(SpecialFolderIDArray)/sizeof(int) && !bRet;++i)
			{
				if(strFolderArray[i].Find(strTem)>=0)
				{
					SXCExceptionInfo ei ;
					ei.strSrcFile = (*it) ;
					ei.SupportType = ErrorHandlingFlag_Exit  ;
					ei.uFileID = 0 ;
					ei.ErrorCode.nSystemError = 0 ;
					ei.ErrorCode.AppError = CopyFileErrorCode_InvaliableFileName ;

					m_XCCopyEvent.XCOperation_CopyExcetption(ei);
					bRet = true ;
				}
			}
		}
	}

	return bRet ;
}

// 处理copy工作
void CXCTransApp::CopyWork() 
{
	m_CurRunState = ARS_StandardRunning ;

	int nSrcProblemIndex = 0 ;
	int nDstProblemIndex = 0 ;

	CptString strRenamePrefix = ::CptMultipleLanguage::GetInstance()->GetString(IDS_FORMAT_COPYOF) ;

	// 判断是否存在源文件夹递归复制的情况
	if(::DoesIncludeRecuriseFolder(m_CopyTask,nSrcProblemIndex,nDstProblemIndex))
	{
		m_pUIWnd->XCUI_OnRecuriseFolder(m_CopyTask.strSrcFileVer[nSrcProblemIndex],m_CopyTask.strDstFolderVer[nDstProblemIndex]) ;
		goto APP_WORK_EXIT ;
	}

	// 回调事件设置
	m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_WriteLog,&m_LogFile) ;

	m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_ImpactFile,this) ;
	//m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_ReadOnly,this) ;
	m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_GetState,this) ;

	m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_CopyBatchFilesBegin,this) ;
	m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_CopyFileEnd,this) ;
	m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_CopyFileDataOccur,this) ;
	m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_CopyFileDiscard,this) ;
	m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_Exception,this) ;

#ifdef VERSION_PROFESSIONAL
	m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_RecordError,this) ;
#endif

#ifdef VERSION_PROFESSIONAL
	if(NULL!=m_pFailedFile)
	{
		m_pFailedFile->AddObserver(this) ;
	}
#endif

	m_LogFile.WriteStart(m_CopyTask,m_ConfigData) ;

	// OK

	// 如果源文件里有根目录，则先把 *.* 插入其中
	this->InsertWildcardToRootFolder() ;


#ifdef VERSION_UNLIMIT_FILE_NAME_LENGTH
	if(!this->MakeUnlimitedFileName()) 
	{// 把文件名转换成不限长度的名
		goto APP_WORK_EXIT ;
	}
#endif

	if(!this->ExpandWildcardName())
	{// 把带有通配符的源文件转为符合条件的文件
		goto APP_WORK_EXIT ;
	}

	// 这个调用必须在 ExpandWildcardName() 以后的，
	// 因为作为源码文件带有通配符的，就先展开后才能时行判断
	// 判断是否有特殊文件夹被 ExtremeCopy 进行移动，特殊文件夹 被移动是不允许的
	if(this->IsSpecialFolderMovement())
	{
		goto APP_WORK_EXIT ;
	}

	bool bResult = this->ExecuteTask2(m_CopyTask) ; // 执行复制任务

#ifdef VERSION_PROFESSIONAL
	m_TaskQueue.RemoveFromQueue() ;
#endif

	if(NULL!=m_pUIWnd)
	{
		m_pUIWnd->XCUI_OnRunningStateChanged(CFS_Stop) ;
	}

	if(bResult)
	{// 刷新 explorer 界面

		::SHChangeNotify(SHCNE_UPDATEDIR,SHCNF_PATH|SHCNF_FLUSH,m_CopyTask.strDstFolderVer[0].c_str(),NULL) ;

		if(m_CopyTask.CopyType==SXCCopyTaskInfo::RT_Move)
		{// 如果是移动，则也要更新源文件所在的文件夹
			CptWinPath::SPathElementInfo pei ;

			pei.uFlag = CptWinPath::PET_Path ;

			if(CptWinPath::GetPathElement(m_CopyTask.strSrcFileVer[0],pei))
			{
				::SHChangeNotify(SHCNE_ALLEVENTS,SHCNF_PATH|SHCNF_FLUSH,pei.strPath.c_str(),NULL) ;
			}
		}

		// 播放声音文件
		if(m_ConfigData.bPlayFinishedSound)
		{
			::sndPlaySound(m_ConfigData.strSoundFile.c_str(),SND_SYNC) ;
		}

		if(m_CopyTask.strFinishEvent.GetLength()>0)
		{// 执行 finish event
			::ShellExecute(NULL,_T("open"),m_CopyTask.strFinishEvent.c_str(),NULL,NULL,SW_SHOW) ;
		}
	}

	if(NULL!=m_pXCSta)
	{
		const SStatisticalValue& sta = m_pXCSta->GetStaData() ;
		m_LogFile.FinishTask(sta) ;
	}

#ifdef VERSION_PROFESSIONAL
	if(m_pFailedFile->GetFailedFileCount()==0 && !m_ConfigData.bCloseWindowAfterDone && m_CopyTask.CopyType==SXCCopyTaskInfo::RT_Copy)
#else
	if(!m_ConfigData.bCloseWindowAfterDone)
#endif
	{// 就算没有错误，但经过verify的话，则停留下来让用户知道结果（未确定这里在verify之后是否一定要停下来）
		m_CurRunState = ARS_WaitForExit ;
		while(m_CurRunState==ARS_WaitForExit)
		{
			::Sleep(100) ;
		}
	}

#ifdef VERSION_PROFESSIONAL
	this->ProcessFailedFiles() ; // 处理 failed file
#endif

APP_WORK_EXIT:

	if(NULL!=m_pUIWnd)
	{
		m_pUIWnd->XCUI_OnRunningStateChanged(CFS_Exit) ;
	}

	m_CurRunState = ARS_Exit ;

	// 程序是在正常完成任务的情况下
	// 用户选择了任务结束后关机，但必须是任务队列后面没有任务才能关机
	if(bResult && m_CopyTask.ConfigShare.bShutdownAfterDone && m_TaskQueue.GetWaitingTaskCount()==0)
	{
		//CptGlobal::ShutdownWindows() ;
		MessageBox(0,_T("shutdown"),_T(""),0);
	}

	return ;
}


bool CXCTransApp::SetSpeed(const int nSpeed) 
{
	if(nSpeed>0 && nSpeed<=100)
	{
		m_nSpeed = nSpeed ;
		return true ;
	}

	return false ;
}

void CXCTransApp::SetShutdown(const bool bIsShutdown) 
{
	m_CopyTask.ConfigShare.bShutdownAfterDone = bIsShutdown ;
}

void CXCTransApp::SetVerifyData(const bool bVerify) 
{
	m_CopyTask.ConfigShare.bVerifyData = bVerify ;
}

void CXCTransApp::SetCloseWindow(const bool bIsCloseWindow) 
{
	m_ConfigData.bCloseWindowAfterDone = bIsCloseWindow ;
}

ECopyFileState CXCTransApp::GetXCState() 
{
	return m_XCCore.GetState() ;
}

EXCStatusType CXCTransApp::GetXCStatus() const 
{
	return m_CurStatus ;
}

void CXCTransApp::StartOneSecondTimer() 
{
	if(m_pUIWnd!=NULL)
	{
		m_pUIWnd->XCUI_SetTimer(TIMER_ID_ONE_SECOND,1000) ;
	}
}

void CXCTransApp::StopOneSecondTimer() 
{
	if(m_pUIWnd!=NULL)
	{
		m_pUIWnd->XCUI_KillTimer(TIMER_ID_ONE_SECOND) ;
	}
}


int CXCTransApp::OnCopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1,void* pParam2) 
{// 这里的格式参考 CXCCopyingEvent.cpp 的实现, CopyingEvent_Execute() 被调用是线程安全的
	int nRet = 0 ;

	switch(et)
	{
	case CXCCopyingEvent::ET_CopyBatchFilesBegin: // 一批文件开始复制
		{
			_ASSERT(pParam1!=NULL) ;

			pt_STL_list(SActiveFilesInfo)& pSrcFileQue = *(pt_STL_list(SActiveFilesInfo)*)pParam1 ;

			this->ProcessEvent_CopyFileBegin(pSrcFileQue) ;
		}
		break ;

	case CXCCopyingEvent::ET_CopyFileEnd: // 一批文件所有操作结束
		{
			pt_STL_vector(SFileEndedInfo)* feiVer = (pt_STL_vector(SFileEndedInfo) *)pParam1 ;

			this->m_pXCSta->IncreseDoneFileCount((int)feiVer->size()) ;
		}

		break ;

	case CXCCopyingEvent::ET_CopyFileDataOccur: // 文件数据发生操作
		{
			_ASSERT(pParam1!=NULL) ;
			SFileDataOccuredInfo& pFileDataInfo = *(SFileDataOccuredInfo*)pParam1 ;

			{
				CptAutoLock lock(&m_FdoListLock) ;

				// 把数据保存到缓冲区，然后再由 1秒 UI 刷新操作来计算出统计数据
				m_fdoiList[m_nCurFdoListIndex].push_back(pFileDataInfo) ;
			}

			this->SpeedDelay() ;

		}
		break ;

	case CXCCopyingEvent::ET_CopyFileDiscard: // 文件抛弃
		{
			_ASSERT(pParam1!=NULL) ;
			const SDataPack_SourceFileInfo& pFD = *(SDataPack_SourceFileInfo*)pParam1 ;
			const unsigned __int64& uDiscardSize = *(unsigned __int64*)pParam2 ;

			this->ProcessEvent_CopyFileDiscard(pFD,uDiscardSize) ;
		}
		break ;

	case CXCCopyingEvent::ET_Exception: // 发生异常
		{
			_ASSERT(pParam1!=NULL) ;

			SXCExceptionInfo& ExceptInfo = *(SXCExceptionInfo*)pParam1 ;

			nRet = (int)this->ProcessEvent_Exception(ExceptInfo) ;
		}
		break ;

	case CXCCopyingEvent::ET_ImpactFile: // 冲突文件
		{
			_ASSERT(pParam1!=NULL) ;
			_ASSERT(pParam2!=NULL) ;
			
			SImpactFileInfo& ImpactInfo = *(SImpactFileInfo*)pParam1 ;
			SImpactFileResult& Result = *(SImpactFileResult*)pParam2 ;

			this->ProcessEvent_ImpactFile(ImpactInfo,Result) ;
		}
		break ;

	case CXCCopyingEvent::ET_GetState: // 状态
		{
			_ASSERT(pParam1!=NULL) ;

			ECopyFileState& pState = *(ECopyFileState*)pParam1 ;

			pState = this->GetXCState() ;
		}
		break ;

#ifdef VERSION_PROFESSIONAL
	case CXCCopyingEvent::ET_RecordError:
		{
			_ASSERT(pParam1!=NULL) ;
			SXCExceptionInfo* pECI = (SXCExceptionInfo*)pParam1 ;

			this->AddToFailedFile(*pECI) ;
		}
		break ;
#endif
	}

	return nRet ;
}

// 开始复制一批文件
void CXCTransApp::ProcessEvent_CopyFileBegin(const pt_STL_list(SActiveFilesInfo)& SrcFileList) 
{
	_ASSERT(!SrcFileList.empty()) ;

	SActiveFilesInfo bfi ;

	pt_STL_list(SActiveFilesInfo)::const_iterator it = SrcFileList.begin() ;

	CptAutoLock lock(&m_ActiveMapLock) ;

	while(it!=SrcFileList.end())
	{	
		// 当m_ActiveFilesInfoMap已存在相应的ID时,则说明可能有多个destination,因为该回调函数是从destination filter 处回调上来的
		if(m_ActiveFilesInfoMap.find((*it).uFileID)==m_ActiveFilesInfoMap.end())
		{
			if((*it).uFileSize==0)
			{// 因为当文件长度为0时，其对应文件ID就没有数据上调，所以如果加入m_ActiveFilesInfoMap的话，就不会被释放了，但文件数要增1
				//this->m_pXCSta->IncreseDoneFileCount() ;
			}
			else
			{
				m_ActiveFilesInfoMap[(*it).uFileID] = (*it) ;
			}
		}

		++it ;
	}

}

void CXCTransApp::GetUIOneSecondData(SXCUIOneSecondUpdateDisplay& osud)
{
	pt_STL_list(SFileDataOccuredInfo)* pFdoiList = NULL ;

	{
		CptAutoLock lock(&m_FdoListLock) ;
		pFdoiList = &m_fdoiList[m_nCurFdoListIndex] ;

		m_nCurFdoListIndex = (m_nCurFdoListIndex==0 ? 1 : 0 ) ; // 交换缓冲区
	}

	if(pFdoiList->empty())
	{// 如果目标文件已创建，但还没有数据发生回调上来，则先把文件名先显示出来
		CptAutoLock lock(&m_ActiveMapLock) ;
		
		if(!m_ActiveFilesInfoMap.empty())
		{
			const SActiveFilesInfo& FileInfo = (*m_ActiveFilesInfoMap.begin()).second ;

			osud.strCurSrcFileName = FileInfo.strSrcFile ;
			osud.strCurDstFileName = FileInfo.strDstFile ;
		}
	}
	else
	{
		m_CreatedDestinationFileList.clear() ;
		m_FriendlyUIList.clear() ;

		pt_STL_list(SFileDataOccuredInfo)::const_iterator it = pFdoiList->begin() ;
		pt_STL_map(unsigned,SActiveFilesInfo)::iterator it2 ;

		unsigned __int64 uPreFileSize = 0 ;

		{
			CptAutoLock lock(&m_ActiveMapLock) ;

			for(;it!=pFdoiList->end();++it)
			{
				if(m_LastUIOnceSecondFileID!=(*it).uFileID)
				{// 记录下该从 m_ActiveFilesInfoMap 移除的文件 ID
					it2 = m_ActiveFilesInfoMap.find((*it).uFileID) ;

					// 这里有可能在active 文件里不存在对应文件ID，因为可能有文件在抛弃时就把该ID从m_ActiveFilesInfoMap，
					// 但 m_fdoiList 仍残留其数据
					if(it2!=m_ActiveFilesInfoMap.end())
					{
						// 设置当前文件大小
						m_pXCSta->SetCurWholeSize((*it2).second.uFileSize) ;

						osud.strCurSrcFileName = CptGlobal::MakeUnlimitFileName((*it2).second.strSrcFile,false) ;
						osud.strCurDstFileName = CptGlobal::MakeUnlimitFileName((*it2).second.strDstFile,false) ;

						if(m_LastUIOnceSecondFileID>0)
						{// 从 m_ActiveFilesInfoMap 里删除已全部计算完文件数据的文件
							it2 = m_ActiveFilesInfoMap.find(m_LastUIOnceSecondFileID) ;

							//_ASSERT(it2!=m_ActiveFilesInfoMap.end()) ;
							if(it2!=m_ActiveFilesInfoMap.end())
							{
								//if(uPreFileSize<20*1024*1024)
								{// 如果前一个不大于 20MB 才加入友好队列
									m_FriendlyUIList.push_back(osud) ;
									uPreFileSize = (*it2).second.uFileSize ;
								}
								
								m_ActiveFilesInfoMap.erase(it2) ;
							}
						}

						m_LastUIOnceSecondFileID = (*it).uFileID ;
					}

				}// if(m_LastUIOnceSecondFileID!=(*it).uFileID)

				m_pXCSta->SetDataOccured((*it).nDataSize,(*it).bReadOrWrite) ;
			}

			if(!m_FriendlyUIList.empty())
			{
				m_FriendlyUIList.pop_back() ;// 把最后一个弹出
			}
			
			m_uCurUIFileID = m_LastUIOnceSecondFileID ; // 这里更新当前 UI 文件ID值
		}
		
		pFdoiList->clear() ;// 把这个已计算完的 fdoi list 清空
	}
	
	if(!m_FriendlyUIList.empty())
	{
		osud = m_FriendlyUIList.back() ;
		m_FriendlyUIList.pop_back() ;
	}

	if(osud.strCurSrcFileName.GetLength()>0)
	{
		osud.strCurSrcFileName = CptGlobal::MakeUnlimitFileName(osud.strCurSrcFileName,false) ;
	}

	if(osud.strCurDstFileName.GetLength()>0)
	{
		osud.strCurDstFileName = CptGlobal::MakeUnlimitFileName(osud.strCurDstFileName,false) ;
	}
}

// 结束复制一文件
void CXCTransApp::ProcessEvent_CopyFileDataDone(unsigned int uFileID) 
{
}

/**
// 文件数据发生操作
void CXCTransApp::ProcessEvent_CopyFileDataOccur(const SFileDataOccuredInfo& FileDataInfo) 
{
	_ASSERT(!m_ActiveFilesInfoMap.empty()) ;

	if(m_pCurActiveFile==NULL && !FileDataInfo.bReadOrWrite)
	{// 新开始的一文件
		pt_STL_map(unsigned,SActiveFilesInfo)::iterator it = m_ActiveFilesInfoMap.find(FileDataInfo.uFileID) ;

		_ASSERT(it!=m_ActiveFilesInfoMap.end()) ;

		m_pCurActiveFile = &((*it).second) ;

		this->UpdateUICurrentFileName() ;
	}
	else
	{
#ifdef _DEBUG
		if(m_pCurActiveFile!=NULL && !FileDataInfo.bReadOrWrite)
		{
			unsigned __int64 nRemainSize = m_pXCSta->GetCurFileRemainSize() ;
			_ASSERT(m_pCurActiveFile->uFileID==FileDataInfo.uFileID) ;
		}
#endif
	}

	m_pXCSta->SetDataOccured((unsigned __int64)FileDataInfo.nDataSize,FileDataInfo.bReadOrWrite) ;

	if(!FileDataInfo.bReadOrWrite)
	{
		m_pUIWnd->XCUI_UpdateUI(UUIT_CopyDataOccured,(void*)&FileDataInfo.bReadOrWrite,(void*)&FileDataInfo.nDataSize) ;

		this->SpeedDelay() ; // 控制速度

		if(m_pXCSta->GetCurFileRemainSize()==0)
		{// 完成一文件写入
			m_ActiveFilesInfoMap.erase(m_pCurActiveFile->uFileID) ;

			m_pCurActiveFile = NULL ;
		}
	}

	//m_pUIWnd->XCUI_UpdateUI(UUIT_CopyDataOccured,(void*)FileDataInfo.bReadOrWrite,(void*)FileDataInfo.nDataSize) ;
}
/**/

void CXCTransApp::AddSkipFileToFileChangedBuf(const CptString& strSrcFile,const CptString& strDstFile) 
{
	if(m_CopyTask.CopyType==SXCCopyTaskInfo::RT_Copy)
	{// 因为用户可能随时设置verify，所以这里不检测m_CopyTask.bVerifyData

		{// 如果当前是复制任务，则加入到 文件改变缓冲区
			CXCFileChangingBuffer::SFileChangingStatusResult fcsr ;

			fcsr.fct = CXCFileChangingBuffer::FileChangingType_Skip ;
			fcsr.strSrcFileName = strSrcFile ;
			fcsr.strDstFileName = strDstFile ;

			if(!m_FileChangingBuf.AddChangingStatus(strSrcFile,fcsr))
			{// 已满
			}
		}
	}
}

// 文件抛弃
void CXCTransApp::ProcessEvent_CopyFileDiscard(const SDataPack_SourceFileInfo& sfi,const unsigned __int64& uDiscardSize) 
{
	unsigned __int64 uFixOccuredSize = 0 ;

	{
		CptAutoLock lock(&m_ActiveMapLock) ;

		pt_STL_map(unsigned,SActiveFilesInfo)::iterator it2 = m_ActiveFilesInfoMap.find(sfi.uFileID) ;

		if(it2==m_ActiveFilesInfoMap.end())
		{
			this->AddSkipFileToFileChangedBuf(sfi.strSourceFile,_T("")) ;
			return ;
		}

		if(m_uCurUIFileID!=0 && sfi.uFileID==m_uCurUIFileID)
		{// 抛弃正在显示的文件
			uFixOccuredSize = m_pXCSta->GetCurFileRemainSize() ;
			m_uCurUIFileID = 0 ;
		}
		else
		{// 抛弃还没有显示出来的文件
			uFixOccuredSize = (*it2).second.uFileSize ;
		}
		
		this->AddSkipFileToFileChangedBuf((*it2).second.strSrcFile,(*it2).second.strDstFile) ;

		/**
		{// 从1秒缓冲区处删除这份跳过的文件

			CptAutoLock lock(&m_FdoListLock) ;
			pt_STL_list(SFileDataOccuredInfo)* pFdoiList = &m_fdoiList[m_nCurFdoListIndex] ;

			CCompareDOI doi((*it2).second.uFileID) ;

			m_fdoiList[m_nCurFdoListIndex].remove_if(doi) ;
			m_fdoiList[m_nCurFdoListIndex==0 ? 1 : 0].remove_if(doi) ;

		}
		/**/
		m_ActiveFilesInfoMap.erase(it2) ;
	}


	// 文件跳过后则把剩余数据调整过来
	{
		DWORD dwHi = 0;
		DWORD dwLow = 0;

		CptGlobal::Int64ToDoubleWord(uFixOccuredSize,dwHi,dwLow) ;

		m_pXCSta->SetDataOccured(dwLow,true) ;
		m_pXCSta->SetDataOccured(dwLow,false) ;

		for(DWORD i=0;i<dwHi;++i)
		{
			m_pXCSta->SetDataOccured(0xffffffff,true) ;
			m_pXCSta->SetDataOccured(0xffffffff,false) ;
		}
	}

	m_pXCSta->IncreaseSkipFileCount(1) ;
}

// 出现异常
ErrorHandlingResult CXCTransApp::ProcessEvent_Exception(const SXCExceptionInfo& ExceptionInfo) 
{
	ErrorHandlingResult ret = ErrorHandlingFlag_Exit ;

	if(m_CurRunState==ARS_FailedRestore)
	{// 在恢复阶段

		_ASSERT(m_pCurFailedFile!=NULL) ;

		if(m_pCurFailedFile==NULL)
		{
			return ErrorHandlingFlag_Exit ;
		}

		m_pCurFailedFile->ErrorCode = ExceptionInfo.ErrorCode ;
	}
	else
	{// 标准复制阶段
		SRichCopySelection::EErrorProcessType ept = this->CalErrorProcType(m_CopyTask.ConfigShare.epc,ExceptionInfo.SupportType) ;

		ret = this->ProcessCopyError(ept,ExceptionInfo) ;
	}

	m_LogFile.WriteErrorOccured(ExceptionInfo,ret) ;	

	return ret ;
}


SRichCopySelection::EErrorProcessType CXCTransApp::CalErrorProcType(const SRichCopySelection::EErrorProcessType& DefaultType, const int& SupportProcessType) const
{
	SRichCopySelection::EErrorProcessType ret = DefaultType ;

	switch(DefaultType)
	{
	case SRichCopySelection::EPT_Retry:
		if(!(SupportProcessType&ErrorHandlingFlag_Retry))
		{
			ret = SRichCopySelection::EPT_Ask ;
		}
		break ;

	case SRichCopySelection::EPT_Ignore:
		if(!(SupportProcessType&ErrorHandlingFlag_Ignore))
		{
			ret = SRichCopySelection::EPT_Ask ;
		}
		break ;
	}

	return ret ;
}

// 冲突文件
void CXCTransApp::ProcessEvent_ImpactFile(const SImpactFileInfo& ImpactInfo,SImpactFileResult& ifr) 
{
	ifr.result = SFDB_Unkown ;

	switch(ImpactInfo.ImpactType)
	{
	case IT_SameFileName:
		{
			ifr.result = this->ImpactFile_SameFile(ImpactInfo) ;

			// 把变化的文件放入缓冲区，等verify时用
			switch(ifr.result)
			{
				case SFDB_Skip: // 跳过
					{
						//CXCFileChangingBuffer::SFileChangingStatusResult fcsr ;

						//fcsr.fct = CXCFileChangingBuffer::FileChangingType_Skip ;
						//fcsr.strDstFileName = ImpactInfo.strDestFile ;

						//if(!m_FileChangingBuf.AddChangingStatus(ImpactInfo.strSrcFile,fcsr))
						//{// 已满
						//}
					}
					break ;

				case SFDB_Rename: // 改名
					{
						ifr.strNewDstFileName = this->ProcessEvent_GetRenamedFileName(ImpactInfo.strDestFile,ImpactInfo.strSrcFile.CompareNoCase(ImpactInfo.strDestFile)==0,CptGlobal::IsFolder(ImpactInfo.strSrcFile)) ;

						if(m_CopyTask.CopyType==SXCCopyTaskInfo::RT_Copy)
						{// 因为用户可能随时设置verify，所以这里不检测verify
							CXCFileChangingBuffer::SFileChangingStatusResult fcsr ;

							fcsr.fct = CXCFileChangingBuffer::FileChangingType_Rename ;
							// 暂不添加目标文件名
							fcsr.strDstFileName = ifr.strNewDstFileName ;
							fcsr.strSrcFileName = ImpactInfo.strSrcFile ;

							if(!m_FileChangingBuf.AddChangingStatus(ImpactInfo.strSrcFile,fcsr))
							{// 已满
							}
						}
					}
					break ;
			}
		}
		break ;

	case IT_ReadOnly:
		break ;

	case IT_ExecutableFile:
		break ;
	}
}

// 获取改名后的文件名
CptString CXCTransApp::ProcessEvent_GetRenamedFileName(CptString strOldFileName,bool bSameFile,bool bFolder) 
{
	_ASSERT(strOldFileName.GetLength()>2) ;

	CptString strNewFileFormat = strOldFileName ; 

	CptString strNewFile = strOldFileName ;

	int nIndex = -1 ;
	
	if(bFolder)
	{// 文件夹
		nIndex = strNewFileFormat.GetLength() ;
	}
	else
	{// 文件

		// 因为文件可以不带'.' ,而文件夹可能会带 '.',所以找到的文件
		const TCHAR* t = strNewFileFormat.c_str() ;
		const int nLen = strNewFileFormat.GetLength() ;

		for(int i=nLen-1;i>0;--i)
		{
			if(t[i]=='.')
			{
				nIndex = i ;
				break ;
			}
			else if(t[i]=='\\' || t[i]=='/')
			{
				break ;
			}
		}
	}

	if(bSameFile)
	{// 复件的改名
		CptString strCopyOf(_T(" - ")) ;

		strCopyOf += m_strCopyOfText ;

		if(nIndex==-1)
		{
			strNewFile += strCopyOf ;
		}
		else
		{
			strNewFile.Insert(nIndex,strCopyOf) ;
		}

		if(IsFileExist(strNewFile.c_str()))
		{
			strNewFile = this->ProcessEvent_GetRenamedFileName(strNewFile,false,bFolder) ;
		}
	}
	else
	{// 非复件的文件名改名
		const TCHAR* szRenameFormat = _T("(%d)") ;

		if(nIndex==-1)
		{
			strNewFileFormat += szRenameFormat ;
		}
		else
		{
			strNewFileFormat.Insert(nIndex,szRenameFormat) ;
		}

		int i = 1 ;

		do
		{
			strNewFile.Format(strNewFileFormat.c_str(),i) ;

			++i ;
		}
		while(IsFileExist(strNewFile.c_str())) ;
	}


	return strNewFile ;
}

EImpactFileBehaviorResult CXCTransApp::AskUserForSameFile(SImpactFileInfo ifi) 
{
	EImpactFileBehaviorResult sfbRet = SFDB_Skip ;

	if(m_pUIWnd!=NULL)
	{
		this->StopOneSecondTimer() ;

		ifi.strSrcFile = CptGlobal::MakeUnlimitFileName(ifi.strSrcFile,false) ;
		ifi.strDestFile = CptGlobal::MakeUnlimitFileName(ifi.strDestFile,false) ;

		sfbRet = m_pUIWnd->XCUI_OnImpactFile(ifi,m_BehaviorSetting) ;
		m_BehaviorSetting.bActived = true ;

		this->StartOneSecondTimer() ;
	}

	return sfbRet ;
}

// 当 ‘same file dialog’第一次弹出并作出选择后，app 会记住这个选择，并在该函数处理下次出现 same file 的情况
// 这函数就是做这个带有记忆性的处理工作
EImpactFileBehaviorResult CXCTransApp::ImpactFile_SameFileByUserChoose(const SImpactFileInfo& ImpactInfo) 
{
	EImpactFileBehaviorResult bRet = SFDB_Replace ; // 

	switch(m_BehaviorSetting.Behavior)
	{
	case CopyBehavior_ReplaceUncondition: // 覆盖旧文件
		bRet = SFDB_Replace ; 
		break ; 

	case CopyBehavior_SkipCondition: // 跳过
		{
			if(m_BehaviorSetting.SkipCondition!=0
				&& (SameFileCondition_SameSize&m_BehaviorSetting.SkipCondition
				|| SameFileCondition_SameCreateTime&m_BehaviorSetting.SkipCondition
				|| SameFileCondition_SameModifyTime&m_BehaviorSetting.SkipCondition))
			{
				WIN32_FIND_DATA wfdSrc ;
				WIN32_FIND_DATA wfdDst ;

				HANDLE hFileFindSrc = ::FindFirstFile(ImpactInfo.strSrcFile.c_str(),&wfdSrc) ;

				if(hFileFindSrc!=INVALID_HANDLE_VALUE)
				{
					HANDLE hFileFindDst = ::FindFirstFile(ImpactInfo.strDestFile.c_str(),&wfdDst) ;

					if(hFileFindDst!=INVALID_HANDLE_VALUE)
					{
						bool bFinish = false ;

						if(!bFinish && (SameFileCondition_SameSize&m_BehaviorSetting.SkipCondition))
						{// 是否相同"大小"
							bFinish = !(wfdSrc.nFileSizeLow==wfdDst.nFileSizeLow && wfdSrc.nFileSizeHigh == wfdDst.nFileSizeHigh) ;
						}

						if(!bFinish && (SameFileCondition_SameCreateTime&m_BehaviorSetting.SkipCondition))
						{// 是否相同"CREATE TIME"
							bFinish = !(wfdSrc.ftCreationTime.dwHighDateTime == wfdDst.ftCreationTime.dwHighDateTime
								&& wfdSrc.ftCreationTime.dwLowDateTime == wfdDst.ftCreationTime.dwLowDateTime);
						}

						if(!bFinish && (SameFileCondition_SameModifyTime&m_BehaviorSetting.SkipCondition))
						{// 是否相同"MODIFY TIME"
							bFinish = !(wfdSrc.ftLastWriteTime.dwHighDateTime == wfdDst.ftLastWriteTime.dwHighDateTime
								&& wfdSrc.ftLastWriteTime.dwLowDateTime == wfdDst.ftLastWriteTime.dwLowDateTime);
						}

						bRet = bFinish ? SFDB_Replace:SFDB_Skip ;

						::FindClose(hFileFindDst) ;
					}

					::FindClose(hFileFindSrc) ;
				}
			}
		}
		break ;

	case CopyBehavior_AskUser: // 询问用户
		bRet = this->AskUserForSameFile(ImpactInfo) ;
		break ;

	case CopyBehavior_Rename: // 改名
		bRet = SFDB_Rename ;
		break ;
	}

	return bRet;
}

bool CXCTransApp::IsMatchSameFileCondition(const CptString& strSrcFile,const CptString& strDestFile,SRichCopySelection::EFileDifferenceType fdt) 
{
	bool bRet = false ;
	bool bIsValid = false ;

	WIN32_FIND_DATA wfdSrc ;
	WIN32_FIND_DATA wfdDst ;
	HANDLE hFileFindSrc = ::FindFirstFile(strSrcFile,&wfdSrc) ;

	// 判断是否正在复制的文件是更新的
	if(hFileFindSrc!=INVALID_HANDLE_VALUE)
	{
		::FindClose(hFileFindSrc) ;
		HANDLE hFileFindDst = ::FindFirstFile(strDestFile,&wfdDst) ;

		if(hFileFindDst!=INVALID_HANDLE_VALUE)
		{
			::FindClose(hFileFindDst) ;
			bIsValid = true ;
		}
	}
	
	if(bIsValid)
	{
		switch(fdt)
		{
		case SRichCopySelection::FDT_Newer:
		case SRichCopySelection::FDT_Older:
			{
				FILETIME   SrcLocaltime; 
				FILETIME   DstLocaltime; 

				::FileTimeToLocalFileTime(&wfdSrc.ftLastWriteTime,&SrcLocaltime) ;
				::FileTimeToLocalFileTime(&wfdDst.ftLastWriteTime,&DstLocaltime) ;

				if(SrcLocaltime.dwHighDateTime==DstLocaltime.dwHighDateTime && SrcLocaltime.dwLowDateTime==DstLocaltime.dwLowDateTime)
				{
					bRet = false;
				}
				else
				{

					bool bIsNewer = (SrcLocaltime.dwHighDateTime>DstLocaltime.dwHighDateTime || 
						(SrcLocaltime.dwHighDateTime==DstLocaltime.dwHighDateTime && 
						(SrcLocaltime.dwLowDateTime>DstLocaltime.dwLowDateTime))) ;

					bRet = (((fdt==SRichCopySelection::FDT_Newer) && bIsNewer) || ((fdt==SRichCopySelection::FDT_Older) && !bIsNewer))  ;
				}
			}
			break ;

		case SRichCopySelection::FDT_Bigger:
			{
				bRet = (wfdSrc.nFileSizeHigh>wfdDst.nFileSizeHigh) || 
					((wfdSrc.nFileSizeHigh==wfdDst.nFileSizeHigh) && (wfdSrc.nFileSizeLow>wfdDst.nFileSizeLow)) ;
			}
			break ;

		case SRichCopySelection::FDT_Smaller:
			{
				bRet = (wfdSrc.nFileSizeHigh<wfdDst.nFileSizeHigh) || 
					((wfdSrc.nFileSizeHigh==wfdDst.nFileSizeHigh) && (wfdSrc.nFileSizeLow<wfdDst.nFileSizeLow)) ;
			}
			break ;

		case SRichCopySelection::FDT_SameTimeAndSize:
			{
				bRet = false ;
				if((wfdSrc.nFileSizeHigh==wfdDst.nFileSizeHigh) && (wfdSrc.nFileSizeLow==wfdDst.nFileSizeLow))
				{
					FILETIME   SrcLocaltime; 
					FILETIME   DstLocaltime; 

					::FileTimeToLocalFileTime(&wfdSrc.ftLastWriteTime,&SrcLocaltime) ;
					::FileTimeToLocalFileTime(&wfdDst.ftLastWriteTime,&DstLocaltime) ;

					bRet = ((SrcLocaltime.dwHighDateTime==DstLocaltime.dwHighDateTime) && 
						(SrcLocaltime.dwLowDateTime==DstLocaltime.dwLowDateTime)) ;
				}
			}
			break ;

		default:
			_ASSERT(FALSE) ;
			break ;
		}
	}

	return bRet ;
}

// 相同文件名
EImpactFileBehaviorResult CXCTransApp::ImpactFile_SameFile(const SImpactFileInfo& pImpactInfo)
{
	EImpactFileBehaviorResult sfbRet = SFDB_Skip ;
	CptString strSrc = pImpactInfo.strSrcFile ;

	if(m_BehaviorSetting.bActived)
	{// 若该设置在上一次 same file name dialog 时用户作了选择，
		// 那么该调用上次用户选择来处理这一次 same file name
		sfbRet = this->ImpactFile_SameFileByUserChoose(pImpactInfo) ;
	}
	else
	{
		if(strSrc.CompareNoCase(pImpactInfo.strDestFile)==0)
		{// 若在同一目录下的文件,则将其改名
			return SFDB_Rename ;
		}
		else
		{
			switch(m_CopyTask.ConfigShare.sfpt)
			{
			case SRichCopySelection::SFPT_Skip: // 跳过
				sfbRet = SFDB_Skip ;
				break ;

			case SRichCopySelection::SFPT_Replace: // 覆盖
				sfbRet = SFDB_Replace ;
				break ;

			default: _ASSERT(FALSE) ;
			case SRichCopySelection::SFPT_Ask: // 询问
				sfbRet = this->AskUserForSameFile(pImpactInfo) ;
				break ;

			case SRichCopySelection::SFPT_Rename: // 重命名
				sfbRet = SFDB_Rename ;
				break ;

			case SRichCopySelection::SFPT_IfCondition: // 根据条件进行做相关的处理
				{
					bool bIsMatchCondition = false ;

					SRichCopySelection::ESameFileOperationType operation ;

					if(this->IsMatchSameFileCondition(pImpactInfo.strSrcFile,pImpactInfo.strDestFile,m_CopyTask.ConfigShare.sfic.IfCondition))
					{
						operation = m_CopyTask.ConfigShare.sfic.ThenOperation ;
					}
					else
					{
						operation = m_CopyTask.ConfigShare.sfic.OtherwiseOperation ;
					}

					switch(operation)
					{
					case SRichCopySelection::SFOT_Skip: sfbRet = SFDB_Skip ; break ;

					case SRichCopySelection::SFOT_Replace: sfbRet = SFDB_Replace ; break ;

					case SRichCopySelection::SFOT_Rename: sfbRet = SFDB_Rename ; break ;

					default: _ASSERT(FALSE) ;
					case SRichCopySelection::SFOT_Ask:
						sfbRet = this->AskUserForSameFile(pImpactInfo) ;
						break ;
					}
				}
				break ;

			}
		}
	}
	

	return sfbRet ;
}

void CXCTransApp::SpeedDelay()
{
	if(m_pUIWnd!=NULL)
	{
#ifdef _DEBUG
		//::Sleep(100) ; // for debug
#endif
		if(m_nSpeed>100 || m_nSpeed<=0)
		{
			m_nSpeed = 100 ;
		}

		int nSpeed = m_nSpeed ;//m_pUIWnd->XCUI_GetSpeedValue() ;

		if(nSpeed<100)
		{// 非全速复制
			
			if(nSpeed<9)
			{
				nSpeed = 9 ;
			}

			const SStatisticalValue& StaData = m_pXCSta->GetStaData() ;

			if(m_nLastSpeedRate!=nSpeed && StaData.fSpeed>1.0f)
			{
				m_nLastSpeedRate = nSpeed ;

				int nSleepTime = (int)((100/(float)nSpeed-1)*1000)+16;

				if(nSleepTime>3000)
				{
					nSleepTime = 3000 ;
				}

				if(nSleepTime<=0)
				{
					nSleepTime = 1 ;
				}

				m_nLastSpeedDelayTime = nSleepTime ;
				//Debug_Printf(_T("sleep time: %d %d"),nSleepTime,nSpeed) ;
			}

			{// 延迟时间
				int nSleepTime2 = m_nLastSpeedDelayTime ;

				while(nSleepTime2>0)
				{
					::Sleep(15) ;

					nSleepTime2 -= 15 ;
				}
			}
		}
	}
}

ECopyFileState CXCTransApp::OnVerifyFileDiff(CptString strSrc,CptString strDst,CXCVerifyResult::EFileDiffType fdt) 
{
	return CFS_Running ;
}

void CXCTransApp::OnVerifyProgressBeginOneFile(const CptString& strSrc,const CptString& strDst) 
{
	CptString strSrc1 = CptGlobal::MakeUnlimitFileName(strSrc,false) ;
	CptString strDst1 = CptGlobal::MakeUnlimitFileName(strDst,false) ;

	m_pUIWnd->XCUI_UpdateUI(UUIT_BeginCopyOneFile,(void*)&strSrc1,(void*)&strDst1) ;
}

void CXCTransApp::OnVerifyProgressDataOccured(const DWORD& uFileSize) 
{
	bool bReadOrWrite = false ;
	this->m_pXCSta->SetVerifyDataOccured(uFileSize) ;

	m_pUIWnd->XCUI_UpdateUI(UUIT_CopyDataOccured,(void*)&bReadOrWrite,(void*)&uFileSize) ;
}

void CXCTransApp::OnVerifyProgress(const CptString& strSrc,const CptString& strDst,const WIN32_FIND_DATA* pWfd) 
{
	_ASSERT(pWfd!=NULL) ;
	/**
	// UUIT_BeginCopyOneFile 命令是为使进度条回0
	CptString strSrc1 = CptGlobal::MakeUnlimitFileName(strSrc,false) ;
	CptString strDst1 = CptGlobal::MakeUnlimitFileName(strDst,false) ;

	m_pUIWnd->XCUI_UpdateUI(UUIT_BeginCopyOneFile,(void*)&strSrc1,(void*)&strDst1) ;

	bool bReadOrWrite = false ;
	this->m_pXCSta->SetVerifyDataOccured(pWfd->nFileSizeLow) ;

	m_pUIWnd->XCUI_UpdateUI(UUIT_CopyDataOccured,(void*)&bReadOrWrite,(void*)&(pWfd->nFileSizeLow)) ;

	if(pWfd->nFileSizeHigh>0)
	{
		const DWORD dwMax = 0xFFFFFFFF ;

		for(unsigned i=0;i<pWfd->nFileSizeHigh;++i)
		{
			this->m_pXCSta->SetVerifyDataOccured(dwMax) ;
			m_pUIWnd->XCUI_UpdateUI(UUIT_CopyDataOccured,(void*)&bReadOrWrite,(void*)&dwMax) ;
		}

		//this->m_pXCSta->SetVerifyDataOccured(pWfd->nFileSizeHigh) ;
		//m_pUIWnd->XCUI_UpdateUI(UUIT_CopyDataOccured,(void*)&bReadOrWrite,(void*)&(pWfd->nFileSizeHigh)) ;
	}
	/**/
}



#ifdef VERSION_PROFESSIONAL // 专业版才有的功能函数 

void CXCTransApp::ProcessFailedFiles() 
{
	if(m_CurRunState==ARS_StandardRunning)
	{
		m_CurRunState=ARS_FailedRestore ;
		//bool bFailedEmpty = true ;
		int nFailedFileCount = this->m_pFailedFile->GetFailedFileCount() ;

		if(nFailedFileCount>0)
		{// 如果存在需要恢复的文件
			m_LogFile.StepIntoFailedFileProcess(nFailedFileCount) ;

			std::vector<SXCCopyTaskInfo> WaitTaskVer ;
			std::vector<int> WaitIndexVer ;
			SXCCopyTaskInfo ti = m_CopyTask ;

			if(m_pUIWnd!=NULL)
			{
				m_pUIWnd->XCUI_OnUIVisibleChanged(true) ;
			}

			SFailedFileInfo ffi ;

			bool bGetFailedFile = false ;

			while(m_CurRunState==ARS_FailedRestore)
			{// 直到用户点击退出按钮才退出
				void * p = m_FailedFileMsgQue.BeginMsg() ;

				if(p!=NULL)
				{
					int nIndex = (int)p - 1 ;

					bGetFailedFile = m_pFailedFile->GetFailedFileByIndex(nIndex,ffi) ;

					_ASSERT(bGetFailedFile==true) ;
					_ASSERT(ffi.Status==EFST_Waitting) ;

					if(bGetFailedFile)
					{
						ti.strSrcFileVer.clear() ;
						ti.strDstFolderVer.clear() ;

						ti.strSrcFileVer.push_back(ffi.strSrcFile) ;
						ti.strDstFolderVer.push_back(ffi.strDstFile) ;

						m_pCurFailedFile = &ffi ;

						if(this->ExecuteTask2(ti))
						{
							ffi.Status = EFST_Success ;
							m_pFailedFile->UpdateFailedFile(ffi) ;
						}
						else
						{
							ffi.Status = EFST_Failed ;
							m_pFailedFile->UpdateFailedFile(ffi) ;
						}
						m_pCurFailedFile = NULL ;
					}

					m_FailedFileMsgQue.EndMsg(NULL) ;
				}
				else
				{
					::Sleep(100) ;
				}
			}
		}
	}
}

void CXCTransApp::AddToFailedFile(const SXCExceptionInfo& ErrorInfo)
{
	SFailedFileInfo ffi ;

	ffi.nIndex = m_pFailedFile->GetFailedFileCount() ;
	ffi.Status = EFST_Failed ;
	ffi.strSrcFile = ErrorInfo.strSrcFile ;
	ffi.strDstFile = ErrorInfo.strDstFile ;
	ffi.ErrorCode = ErrorInfo.ErrorCode ;
	ffi.uFileID = ErrorInfo.uFileID ;

	m_pFailedFile->AddFailedFile(ffi) ;
}

#endif

