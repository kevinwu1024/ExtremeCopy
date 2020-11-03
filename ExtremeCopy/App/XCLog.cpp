/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#include "StdAfx.h"
#include "XCLog.h"
#include "../Common/ptWinPath.h"
#include "../Common/ptTime.h"

CXCLog::CXCLog():m_hEnable(true)
{
}

CXCLog::~CXCLog()
{
}


bool CXCLog::SetEnable(bool bEnable)
{
	const bool bRet = m_hEnable ;

	m_hEnable = bEnable ;

	return bRet ;
}

int CXCLog::OnCopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1,void* pParam2) 
{
	switch(et)
	{
	case CXCCopyingEvent::ET_WriteLog: // 写日志
		break ;
	}

	return 0 ;
}

///////////////////--------------------------------------------


CXCFileLog::CXCFileLog(void):m_hLogFile(INVALID_HANDLE_VALUE)
{
}

CXCFileLog::~CXCFileLog(void)
{
	this->CloseHandle() ;
}

void CXCFileLog::WriteErrorOccured(const SXCExceptionInfo& ErrorInfo,ErrorHandlingResult process) 
{
	if(m_hEnable && m_hLogFile!=INVALID_HANDLE_VALUE)
	{
		CptString strText ;

		strText += _T("Error Occured .\r\n") ;

		strText += _T("source files:") ;

		if(ErrorInfo.strSrcFile.IsEmpty())
		{
			strText += _T(" -\r\n") ;
		}
		else
		{
			strText += ErrorInfo.strSrcFile + _T("\r\n") ;
		}

		// 目标目录
		strText += _T("destination files: ") ;
		if(ErrorInfo.strDstFile.GetLength()==0)
		{
			strText += _T("- \r\n") ;
		}
		else
		{
			strText += ErrorInfo.strDstFile + _T(" \r\n") ;
		}

		CptString strTem ;

		strTem.Format(_T("app error code: %d \r\n"),ErrorInfo.ErrorCode.AppError) ;
		strText +=  strTem;

		strTem.Format(_T("system error code: %d \r\n"),ErrorInfo.ErrorCode.nSystemError) ;
		strText +=  strTem;

		strTem.Format(_T("support type: %d  \r\n"),ErrorInfo.SupportType) ;
		strText +=  strTem;

		// 出错处理类型
		strText += _T("error process: ") ;
		switch(process)
		{
		case SRichCopySelection::EPT_Retry:
			strText += _T("Retry \r\n") ;
			break ;

		case SRichCopySelection::EPT_Exit:
			strText += _T("Exit \r\n") ;
			break ;

		case SRichCopySelection::EPT_Ignore:
			strText += _T("Ignore \r\n") ;
			break ;

		case SRichCopySelection::EPT_Ask:
			strText += _T("Ask \r\n") ;
			break ;
		default:
			strText += _T("- \r\n") ;
			break ;
		}

		this->WriteLog(strText) ;
	}
}



void CXCFileLog::StepIntoFailedFileProcess(int nFailedFilesCount) 
{
	if(m_hEnable && m_hLogFile!=INVALID_HANDLE_VALUE)
	{
		CptString strText ;

		strText.Format(_T("step into failed files recovery phase, failed files count : %d"),nFailedFilesCount) ;
		this->WriteLog(strText) ;
	}
}

void CXCFileLog::FailedFileStatusChanged(const SFailedFileInfo& ffi) 
{
	if(m_hEnable && m_hLogFile!=INVALID_HANDLE_VALUE)
	{
		CptString strText = _T("Failed file status changed.");

		strText.Format(_T("id=%d \r\n source : %s \r\n destination : %s \r\n app error code: %d \r\n system error code : %d \r\n"),
			ffi.nIndex,ffi.strSrcFile,ffi.strDstFile,ffi.ErrorCode.AppError,ffi.ErrorCode.nSystemError) ;

		strText += _T("status: ") ;

		switch(ffi.Status)
		{
		default:
		case EFST_Unknown :
			strText += _T("unknown \r\n") ;
			break ;
		case EFST_Failed:
			strText += _T("failed \r\n") ;
			break ;
		case EFST_Waitting:
			strText += _T("waitting \r\n") ;
			break ;
		case EFST_Running:
			strText += _T("running \r\n") ;
			break ;
		case EFST_Success:
			strText += _T("success \r\n") ;
			break ;
		}

		this->WriteLog(strText) ;
	}
}

void CXCFileLog::TaskRunningStateChanged(int NewState)  // 0: pause; 1: run; 2: exit
{
	if(m_hEnable && m_hLogFile!=INVALID_HANDLE_VALUE)
	{
		CptString strText ;

		strText += _T("Running state changed: ") ;

		switch(NewState)
		{
		case 0: // pause
			strText += _T("pause \r\n") ;
			break;

		case 1: // run
			strText += _T("run \r\n") ;
			break ;

		case 2: // exit
			strText += _T("exit \r\n") ;
			break ;
		}

		this->WriteLog(strText) ;
	}
}

void CXCFileLog::FinishTask(const SStatisticalValue& sta) 
{
	if(m_hEnable)
	{
		this->WriteLog(_T("Task Finished ! \r\n")) ;

		CptString strSta ;
		CptString strSpeed ;
		CptString strLapseTime ;

		const float f = (float)((double)sta.uTotalSize/(sta.nLapseTime==0?1:sta.nLapseTime)) ;

		if(f>=1024*1024.f)
		{
			if(f>1024*1024*1024)
			{// G
				const float f2 = f/(1024*1024*1024) ;
				strSpeed.Format(_T("%.2f G/s"),f2) ;
			}
			else
			{// M
				const float f2 = f/(1024*1024) ;
				strSpeed.Format(_T("%.1f M/s"),f2) ;
			}
		}
		else
		{
			const float f2 = f/1024 ;
			strSpeed.Format(_T("%.1f K/s"),f2) ;
		}

		strLapseTime.Format(_T("%d:%02d:%02d"),sta.nLapseTime/(60*60),(sta.nLapseTime%(60*60))/60,sta.nLapseTime%60) ;

		strSta.Format(_T("Statistics : \r\nAverage Speed:%s \r\nTook Time: %s \r\nTotal Files: %u \r\n\
Done Files: %u\r\nTotal Size: %s \r\nVerify Data Size: %s \r\n"),
			strSpeed,strLapseTime,sta.nTotalFiles,sta.nDoneFiles,::GetSizeString(sta.uTotalSize),
			::GetSizeString(sta.uVerifyDataSize)) ;

		this->WriteLog(strSta) ;

		this->CloseHandle() ;
	}
}

void CXCFileLog::CloseHandle() 
{
	if(m_hLogFile!=INVALID_HANDLE_VALUE)
	{
		::FlushFileBuffers(m_hLogFile) ;
		::CloseHandle(m_hLogFile) ;
		m_hLogFile = INVALID_HANDLE_VALUE ;

		CptString strStandardLog = CptWinPath::GetStartupPath() + _T("\\log\\LastTask.log") ;

		::DeleteFile(strStandardLog.c_str()) ;

		::_trename(m_strFileFullName.c_str(),strStandardLog.c_str()) ;
	}
}

void CXCFileLog::WriteLog(CptString strTxt)
{
	if(m_hEnable && m_hLogFile!=INVALID_HANDLE_VALUE)
	{
		TCHAR szTime[32+1] = {0} ;
		CptTime::Now(szTime) ;

		CptString strLog ;

		strLog.Format(_T("[%s] %s \r\n"),szTime,strTxt) ;

		DWORD dwWrote = 0;
		::WriteFile(m_hLogFile,strLog.c_str(),strLog.GetLength()*sizeof(TCHAR),&dwWrote,NULL) ;
	}
}

bool CXCFileLog::WriteStart(const SXCCopyTaskInfo& task,const SConfigData& config) 
{
	if(!m_hEnable)
	{
		return false ;
	}

	this->CloseHandle() ;

	CptString strFolder = CptWinPath::GetStartupPath() + _T("\\log") ;

	if(!CptGlobal::IsFolder(strFolder.c_str()))
	{
		if(!::CreateDirectory(strFolder.c_str(),NULL))
		{
			return false ;
		}
	}

	TCHAR szTime[32+1] = {0} ;
	CptTime::Now(szTime) ;
	CptString strTem = szTime ;

	const TCHAR* pp2 = strTem.c_str() ;

	strTem.Remove(_T(':')) ;

	m_strFileFullName.Format(_T("%s\\XCTask_%s.log"),strFolder,strTem) ;

	m_hLogFile = ::CreateFile(m_strFileFullName.c_str(),GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL) ;

	if(m_hLogFile!=INVALID_HANDLE_VALUE)
	{
#ifdef _UNICODE
		DWORD dwWrite = 0 ;
		DWORD dwData = 0xFEFF ;
		::WriteFile(m_hLogFile,&dwData,4,&dwWrite,NULL) ;
#endif
		CptString strText = _T("ExtremeCopy start task. \r\n") ;

		//CptString strTaskType = task.CopyType==RT_Copy ? _T("copy") : _T("move") ;
		//CptString strSource ;

		//for(size_t i=0;i<task.strSrcFileVer.size();++i)
		//{
		//	strSource += task.strSrcFileVer[i] + _T("\r\n") ;
		//}

		//strText.Format(_T("ExtremeCopy start '%s' task : \r\n source = %s destination=%s"), 
		//	strTaskType,strSource,task.strDstFile) ;

		// 复制类型
		strText += _T("copy type: ") ;

		switch(task.CopyType)
		{
		case SXCCopyTaskInfo::RT_Copy:
			strText += _T("Copy \r\n") ;
			break ;

		case SXCCopyTaskInfo::RT_Move:
			strText += _T("Move \r\n") ;
			break ;

		default:
			strText += _T("- \r\n") ;
			break ;
		}

		// 源文件
		strText += _T("source files:") ;

		if(task.strSrcFileVer.empty())
		{
			strText += _T(" -\r\n") ;
		}
		else
		{
			for(unsigned int i=0;i<task.strSrcFileVer.size();++i)
			{
				strText += task.strSrcFileVer[i] + _T("\r\n") ;
			}
		}

		// 目标目录
		strText += _T("destination files: ") ;
		if(task.strDstFolderVer.empty())
		{
			strText += _T(" -\r\n") ;
		}
		else
		{
			for(unsigned int i=0;i<task.strDstFolderVer.size();++i)
			{
				strText += task.strDstFolderVer[i] + _T("\r\n") ;
			}
		}

		// 相同文件处理方式
		strText += _T("same file process: ") ;
		switch(task.ConfigShare.sfpt)
		{
		case SRichCopySelection::SFPT_Skip:
			strText += _T("Skip \r\n") ;
			break ;

		case SRichCopySelection::SFPT_Replace:
			strText += _T("Replace \r\n") ;
			break ;

		case SRichCopySelection::SFPT_Ask:
			strText += _T("Ask \r\n") ;
			break ;

		case SRichCopySelection::SFPT_Rename:
			strText += _T("Rename \r\n") ;
			break ;

		default:
			strText += _T("- \r\n") ;
			break ;
		}

		// 出错处理类型
		strText += _T("error process: ") ;
		switch(task.ConfigShare.epc)
		{
		case SRichCopySelection::EPT_Retry:
			strText += _T("Retry \r\n") ;
			break ;

		case SRichCopySelection::EPT_Exit:
			strText += _T("Exit \r\n") ;
			break ;

		case SRichCopySelection::EPT_Ignore:
			strText += _T("Ignore \r\n") ;
			break ;

		case SRichCopySelection::EPT_Ask:
			strText += _T("Ask \r\n") ;
			break ;
		default:
			strText += _T("- \r\n") ;
			break ;
		}

		// 重试次数
		CptString strTem ;
		strTem.Format(_T("retry times: %d \r\n"),task.ConfigShare.nRetryTimes) ;
		strText += strTem ;

		// 显示类型
		strText += _T("show mode: ") ;
		switch(task.ShowMode)
		{
		case SXCCopyTaskInfo::ST_Window:
			strText += _T("Window \r\n") ;
			break ;

		case SXCCopyTaskInfo::ST_Tray:
			strText += _T("Tary \r\n") ;
			break ;

		default:
			strText += _T("- \r\n") ;
			break ;
		}

		this->WriteLog(strText) ;

		return true ;
	}
	
	return false ;
}

int CXCFileLog::OnCopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1,void* pParam2) 
{
	switch(et)
	{
	case CXCCopyingEvent::ET_WriteLog: // 写日志
		break ;
	}

	return 0 ;
}