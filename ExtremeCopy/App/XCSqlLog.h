#pragma once
#include "XCCopyTask.h"
#include "XCGlobal.h"
#include "../Core/XCCopyingEvent.h"
#include "XCStatistics.h"
//#include "../Common/sqlite/CppSQLite3U.h"
#include "XCLog.h"

/*
class CXCSqlLog : public CXCLog
{
public:
	CXCSqlLog(void);
	~CXCSqlLog(void);

	virtual bool WriteStart(const SXCCopyTaskInfo& task,const SConfigData& config) ;
	virtual void WriteErrorOccured(const SXCExceptionInfo& ErrorInfo,ErrorHandlingResult process) ;
	virtual void StepIntoFailedFileProcess(int nFailedFilesCount) ;
	virtual void FailedFileStatusChanged(const SFailedFileInfo& ffi) ;
	virtual void TaskRunningStateChanged(int NewState) ; // 0: pause; 1: run; 2: exit
	virtual void FinishTask(const SStatisticalValue& sta) ;

	bool Init() ;

private:
	

private:
	bool			m_bOpened ;
	CppSQLite3DB	m_SqliteDB ;

	unsigned int	m_nCurTaskID ;
};

*/