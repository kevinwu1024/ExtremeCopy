#include "StdAfx.h"
#include "XCSqlLog.h"
#include "XCGlobal.h"
#include "../Common/ptTime.h"


/*
#define LOG_DB_TABLE_FILE_WITH_PATH		_T("log_file_name")
#define LOG_DB_TABLE_TASK				_T("log_task")
#define LOG_DB_TABLE_RECORD				_T("log_record")


CXCSqlLog::CXCSqlLog(void):m_bOpened(false)
{
}


CXCSqlLog::~CXCSqlLog(void)
{
}

bool CXCSqlLog::Init() 
{
	if(m_bOpened)
	{
		return true ;
	}

	bool bRet = false ;

	try
	{
		const int nStepWait = 50 ;

		HANDLE hEvent = ::CreateEvent(NULL,FALSE,FALSE,_T("ExtremeCopy_Log_DB_Event")) ;

		DWORD dwResult = ::WaitForSingleObject(hEvent,nStepWait) ;

		m_SqliteDB.open(_T("x://my_log.db")) ;

		// 创建 "路径表"
		if(!m_SqliteDB.tableExists(LOG_DB_TABLE_FILE_WITH_PATH))
		{
			CptString strSQL ;
			strSQL.Format(_T("CREATE TABLE %s (ID INTEGER PRIMARY KEY,FileName NTEXT,FileSize BLOB,IsFolder BYTE,BelongToTaskID INTEGER,CreateTime DATETIME)"),LOG_DB_TABLE_FILE_WITH_PATH) ;
			m_SqliteDB.execDML(strSQL);
		}

		// 创建 "任务表"
		if(!m_SqliteDB.tableExists(LOG_DB_TABLE_TASK))
		{
			CptString strSQL ;
			strSQL.Format(_T("CREATE TABLE %s (ID INTEGER PRIMARY KEY,Source NTEXT,Destination NTEXT, Operation BYTE, Version CHAR(8), Configuration BLOB, LaunchType BYTE, IsNormalExit BYTE, BeginTime DATETIME, EndTime DATETIME)"),LOG_DB_TABLE_TASK) ;
			m_SqliteDB.execDML(strSQL);
		}

		// 创建 "记录表"
		if(!m_SqliteDB.tableExists(LOG_DB_TABLE_RECORD))
		{
			CptString strSQL ;
			strSQL.Format(_T("CREATE TABLE Customers(ID INTEGER PRIMARY KEY, TaskID INTEGER, Content NTEXT,CreateTime DATETIME)"),LOG_DB_TABLE_RECORD) ;
			m_SqliteDB.execDML(strSQL);
		}

		bRet = true ;
		
	}
	catch(...)
	{
		bRet = false ;
	}
	
	return bRet ;
}

bool CXCSqlLog::WriteStart(const SXCCopyTaskInfo& task,const SConfigData& config) 
{
	if(!this->Init())
	{
		return false ;
	}

	// 始于 v2.3.0 版本
	struct SLogConfigBlob
	{
		bool bDefaultCopying ; // 是否把ExtremeCopy 设置为默认的文件复制器
		bool bPlayFinishedSound ; // 文件复制完后播放声音
		bool bTopMost ;				// 窗口是否总是最前端显示
		EUIType	UIType ;			// 界面类型
		int		nCopyBufSize ;		// 文件数据交换缓冲区的大小
		bool	bMinimumToTray ;	// 最小化到托盘
		int		nMaxFailedFiles ;	// 最多失败的文件数
		bool	bWriteLog ;			// 是否写日志
		bool	bCloseWindowAfterDone ; //如果检测完数据则自动关闭窗口
		bool	bAutoUpdate ;		// 自动检查最新版本
		bool	bAutoQueueMultipleTask ; // 多任务时自动排队
		time_t uLastCheckUpdateTime ; // 上次检查最新版本的时间
	};

	CptString strSQL ;

	TCHAR szNow[64+1] = {0} ;

	CptString strVersion = ::MakeXCVersionString() ;
	CptTime::Now(szNow) ;

	// 插入一条任务到“任务表”，并取其任务ID
	strSQL.Format(_T("INSTERT INTO %s (Source,Destination,Operation,Version,LaunchType,IsNormalExit,BeginTime) VALUE ('','',%d,%s,%d,0,%s); SELECT ID FROM %s "),
		LOG_DB_TABLE_TASK,
		task.CopyType,
		strVersion,
		task.CreatedBy,
		szNow,
		LOG_DB_TABLE_TASK) ;

	m_SqliteDB.execDML(strSQL);

	return true ;
}

void CXCSqlLog::WriteErrorOccured(const SXCExceptionInfo& ErrorInfo,ErrorHandlingResult process) 
{
}

void CXCSqlLog::StepIntoFailedFileProcess(int nFailedFilesCount) 
{
}

void CXCSqlLog::FailedFileStatusChanged(const SFailedFileInfo& ffi) 
{
}

// 0: pause; 1: run; 2: exit
void CXCSqlLog::TaskRunningStateChanged(int NewState) 
{
}

void CXCSqlLog::FinishTask(const SStatisticalValue& sta) 
{
}
*/