/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "xctransformfilter.h"
#include "XCPin.h"
#include "XCTransformFilter.h"
#include <deque>
#include <set>
#include "..\Common\ptThreadLock.h"
#include "XCFileDataCacheTransFilter.h"
#include "..\Common\ptMsgQue.h"

class CXCAsyncFileDataTransFilter :
	public CXCFileDataCacheTransFilter
{
public:
	CXCAsyncFileDataTransFilter(CXCCopyingEvent* pEvent);
	virtual ~CXCAsyncFileDataTransFilter(void);
	
protected:
	virtual int OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData) ;
	virtual bool OnInitialize() ;
	virtual bool OnContinue() ;
	virtual bool OnPause();
	virtual void OnStop();

private:
	struct SMsgPack
	{
		EFilterCmd cmd ;
		void* pFileData ;
	};

private:
	static UINT __stdcall ThreadFunc(void* pParam) ;
	void OutputWorkThread() ;
	bool WaitForFileDataQueEmpty() ;
	int ProcessLinkedEnd(SDataPack_FileOperationCompleted* pFoc) ;

	struct SCompleteCmdCacheInfo
	{
		pt_STL_list(SDataPack_SourceFileInfo*)	CacheCompleteCmdList ;
	};


private:
	HANDLE						m_hThread ;
	HANDLE						m_hWriteDataReadyEvent ;
	HANDLE						m_hWaitForFileBufNotFull ;
	HANDLE						m_hLinkEndForQueEmpty ;

	CptMsgQue					m_IdelMsgQue ;

	bool						m_bLinkEnded ;
	bool						m_bWriteThreadEnd;

	CptCritiSecLock				m_FileDataQueLock ;
	CptCritiSecLock				m_FileOperCompleteCacheLock ;
	CptCritiSecLock				m_CreateBatchFileLock ;
	bool						m_bIsHaveFileOperComplCmd ;
	unsigned					m_uFirstCompleteFileID ;
	pt_STL_list(SCompleteCmdCacheInfo)	m_CacheCompleteCmdList ;
};

