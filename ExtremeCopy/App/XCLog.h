/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "XCCopyTask.h"
#include "XCGlobal.h"
#include "../Core/XCCopyingEvent.h"
#include "XCStatistics.h"


class CXCLog : public CXCCopyingEventReceiver
{
public:
	CXCLog() ;
	virtual ~CXCLog() ;

	virtual bool WriteStart(const SXCCopyTaskInfo& task,const SConfigData& config) = 0;
	virtual void WriteErrorOccured(const SXCExceptionInfo& ErrorInfo,ErrorHandlingResult process) = 0;
	virtual void StepIntoFailedFileProcess(int nFailedFilesCount) = 0;
	virtual void FailedFileStatusChanged(const SFailedFileInfo& ffi) = 0;
	virtual void TaskRunningStateChanged(int NewState) = 0; // 0: pause; 1: run; 2: exit
	virtual void FinishTask(const SStatisticalValue& sta) = 0;

	bool SetEnable(bool bEnable) ;

	virtual int OnCopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1=NULL,void* pParam2=NULL) ;

protected:
	bool		m_hEnable ;
};

class CXCFileLog : public CXCLog
{
public:
	CXCFileLog(const CXCFileLog&);
	CXCFileLog(void);
	virtual ~CXCFileLog(void);

	CXCFileLog& operator=(const CXCFileLog&) ;

	virtual bool WriteStart(const SXCCopyTaskInfo& task,const SConfigData& config) ;
	virtual void WriteErrorOccured(const SXCExceptionInfo& ErrorInfo,ErrorHandlingResult process) ;
	virtual void StepIntoFailedFileProcess(int nFailedFilesCount) ;
	virtual void FailedFileStatusChanged(const SFailedFileInfo& ffi) ;
	virtual void TaskRunningStateChanged(int NewState) ; // 0: pause; 1: run; 2: exit
	virtual void FinishTask(const SStatisticalValue& sta) ;

	//bool SetEnable(bool bEnable) ;

	virtual int OnCopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1=NULL,void* pParam2=NULL) ;

private:
	inline void WriteLog(CptString strTxt) ;
	inline void CloseHandle() ;

private:
	HANDLE		m_hLogFile ;
	CptString	m_strFileFullName ;

};
