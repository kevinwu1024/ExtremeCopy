/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCCopyingEvent.h"
#include <stdarg.h>

CXCCopyingEvent::CXCCopyingEvent(void):m_pDataOccuredRecever(NULL)
{
}


CXCCopyingEvent::~CXCCopyingEvent(void)
{
}


CXCCopyingEventReceiver* CXCCopyingEvent::SetReceiver(CXCCopyingEvent::EEventType et,CXCCopyingEventReceiver* pRecv)
{
	CXCCopyingEventReceiver* pRet = NULL ;

	if(et==ET_CopyFileDataOccur)
	{
		m_pDataOccuredRecever = pRecv ;
	}
	else
	{
		CptAutoLock lock(&m_MapLock) ;

		pt_STL_map(EEventType,CXCCopyingEventReceiver*)::iterator it = m_RecerMap.find(et) ;

		if(it!=m_RecerMap.end())
		{
			pRet = (*it).second ;
			(*it).second = pRecv ;
		}
		else
		{
			m_RecerMap[et] = pRecv ;
		}
	}

	return pRet ;
}

void CXCCopyingEvent::WriteLog(const TCHAR* pcFormat,...)
{
	CXCCopyingEventReceiver* pRecer = this->GetRecer(ET_WriteLog) ;

	if(pRecer!=NULL)
	{
		TCHAR* szBuf = (TCHAR*)::malloc((1024+1)*sizeof(TCHAR)) ;

		_ASSERT(szBuf!=NULL) ;

		if(szBuf!=NULL)
		{
			va_list pArg;
			va_start(pArg, pcFormat);

#pragma warning(push)
#pragma warning(disable:4996)
			vswprintf(szBuf, pcFormat, pArg);
#pragma warning(pop)

			va_end(pArg);

			pRecer->OnCopyingEvent_Execute(ET_WriteLog,szBuf) ;

			::free(szBuf) ;
			szBuf = NULL ;
		}
	}
}

ErrorHandlingResult CXCCopyingEvent::XCOperation_CopyExcetption(const SXCExceptionInfo& ec)
{
	ErrorHandlingResult ret = ErrorHandlingFlag_Exit ;	

	const EEventType et = ET_Exception ;

	CptAutoLock lock(&m_MapLock) ;

	CXCCopyingEventReceiver* pRecer = this->GetRecer(et) ;

	if(pRecer!=NULL)
	{
		ret = (ErrorHandlingResult)pRecer->OnCopyingEvent_Execute(et,(SXCExceptionInfo*)&ec) ;
	}

	return ret ;
}

void CXCCopyingEvent::XCOperation_RecordError(const SXCExceptionInfo& ec)
{
	const EEventType et = ET_RecordError ;

	CptAutoLock lock(&m_MapLock) ;

	CXCCopyingEventReceiver* pRecer = this->GetRecer(et) ;

	if(pRecer!=NULL)
	{
		pRecer->OnCopyingEvent_Execute(et,(SXCExceptionInfo*)&ec) ;
	}
}

void CXCCopyingEvent::XCOperation_FileDiscard(const SDataPack_SourceFileInfo* pSfi,const unsigned __int64& uDiscardFileSize) 
{
	const EEventType et = ET_CopyFileDiscard ;

	CptAutoLock lock(&m_MapLock) ;

	CXCCopyingEventReceiver* pRecer = this->GetRecer(et) ;

	if(pRecer!=NULL)
	{
		pRecer->OnCopyingEvent_Execute(et,(SDataPack_SourceFileInfo*)pSfi,(unsigned __int64*)&uDiscardFileSize) ;
	}
}

void CXCCopyingEvent::XCOperation_FileBegin(const pt_STL_list(SActiveFilesInfo)& FilePairInfoQue) 
{
#ifdef COMPILE_TEST_PERFORMANCE

	DWORD dw = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif

	const EEventType et = ET_CopyBatchFilesBegin ;

	CptAutoLock lock(&m_MapLock) ;

	CXCCopyingEventReceiver* pRecer = this->GetRecer(et) ;

	if(pRecer!=NULL)
	{
		pRecer->OnCopyingEvent_Execute(et,(pt_STL_deque(SActiveFilesInfo)*)&FilePairInfoQue,NULL) ;
	}

#ifdef COMPILE_TEST_PERFORMANCE
		CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,8) ;
#endif
}

void CXCCopyingEvent::XCOperation_FileEnd(const pt_STL_vector(SFileEndedInfo)& FileEndedInfoVer) 
{
#ifdef COMPILE_TEST_PERFORMANCE

	DWORD dw = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif

	const EEventType et = ET_CopyFileEnd ;

	CptAutoLock lock(&m_MapLock) ;

	CXCCopyingEventReceiver* pRecer = this->GetRecer(et) ;

	if(pRecer!=NULL)
	{
		pRecer->OnCopyingEvent_Execute(et,(pt_STL_vector(SFileEndedInfo) *)&FileEndedInfoVer,NULL) ;
	}

#ifdef COMPILE_TEST_PERFORMANCE
		CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,8) ;
#endif
}


void CXCCopyingEvent::XCOperation_FileDataOccured(const SFileDataOccuredInfo& fdoi) 
{	
#ifdef COMPILE_TEST_PERFORMANCE

	DWORD dw = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif

	if(m_pDataOccuredRecever!=NULL)
	{
		m_pDataOccuredRecever->OnCopyingEvent_Execute(ET_CopyFileDataOccur,(SFileDataOccuredInfo*)&fdoi,NULL) ;
	}

#ifdef COMPILE_TEST_PERFORMANCE
		CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,8) ;
#endif

}

void CXCCopyingEvent::XCOperation_ImpactFile(const SImpactFileInfo& ImpactInfo,SImpactFileResult& ifr)
{
	const EEventType et = ET_ImpactFile ;

	CptAutoLock lock(&m_MapLock) ;

	ifr.result = SFDB_StopCopy ;

	CXCCopyingEventReceiver* pRecer = this->GetRecer(et) ;

	if(pRecer!=NULL)
	{
		pRecer->OnCopyingEvent_Execute(et,(void*)&ImpactInfo,&ifr) ;
	}
}


ECopyFileState CXCCopyingEvent::GetState() 
{	
	const EEventType et = ET_GetState ;

	ECopyFileState nRet = CFS_Stop ;

	CptAutoLock lock(&m_MapLock) ;

	CXCCopyingEventReceiver* pRecer = this->GetRecer(et) ;

	if(pRecer!=NULL)
	{
		
		pRecer->OnCopyingEvent_Execute(et,&nRet) ;
	}

	return nRet ;
}

bool CXCCopyingEvent::WaitForXCContinue()
{
	bool bRet = true ;

	ECopyFileState state = this->GetState() ;

	while(state==CFS_Pause)
	{
		::Sleep(100) ;
	}

	bRet = (state==CFS_Running) ;

	return bRet ;
}

CXCCopyingEventReceiver* CXCCopyingEvent::GetRecer(const CXCCopyingEvent::EEventType et)
{// 注意该函数并非线程安全的,但各个调用它的函数是线程安全的
	CXCCopyingEventReceiver* pRet = NULL ;	

	pt_STL_map(EEventType,CXCCopyingEventReceiver*)::const_iterator it = m_RecerMap.find(et) ;

	if(it!=m_RecerMap.end())
	{
		pRet = (*it).second ;
	}

	return pRet ;
}