/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "XCCoreDefine.h"
#include <map>
#include "..\Common\ptTypeDef.h"
#include "..\Common\ptThreadLock.h"
#include "../Common/ptDebugView.h"
#include <deque>


class CXCCopyingEventReceiver ;

class CXCCopyingEvent
{
public:
	struct SRenamedFileNameInfo
	{
		CptString strOldFileName ;
		bool	bSameFile ;
	};

	enum EEventType
	{
		ET_GetState,
		ET_Exception,
		ET_ImpactFile,
		ET_WriteLog,
		ET_GetRenamedFileName,

		ET_CopyBatchFilesBegin,
		ET_CreateDestinationFile,
		ET_CopyOneFileBegin,
		ET_CopyFileEnd,
		ET_CopyFileDataDone, // file data done 和 file end 是不同的，
							// done是写入到文件的数据已全部写入了，但还没有结束对文件的操作，
							// 而end 就已全部结束了该文件的操作
		ET_CopyFileDataOccur,
		ET_CopyFileDiscard,
		ET_RecordError,
	};

	CXCCopyingEvent(void);
	~CXCCopyingEvent(void);

	CXCCopyingEventReceiver* SetReceiver(EEventType et,CXCCopyingEventReceiver* pRecv) ;

	// 写日志
	void WriteLog(const TCHAR* pcFormat,...) ;

	// 获取当前ExtremeCopy 运行状态
	ECopyFileState GetState() ;

	// 若ExtremeCopy进入 Pause 状态时，该函数会直到解除Pause状态才返回
	bool WaitForXCContinue() ;
	
	void XCOperation_ImpactFile(const SImpactFileInfo& ImpactInfo,SImpactFileResult& ifr) ;

	void XCOperation_FileBegin(const pt_STL_list(SActiveFilesInfo)& FilePairInfoQue) ;
	void XCOperation_FileEnd(const pt_STL_vector(SFileEndedInfo)& FileEndedInfoVer) ;
	void XCOperation_FileDataOccured(const SFileDataOccuredInfo& fdoi) ;
	void XCOperation_FileDiscard(const SDataPack_SourceFileInfo* pSfi,const unsigned __int64& uDiscardFileSize) ;
	void XCOperation_RecordError(const SXCExceptionInfo& ec) ;

	ErrorHandlingResult XCOperation_CopyExcetption(const SXCExceptionInfo& ec) ;

private:
	inline CXCCopyingEventReceiver* GetRecer(const EEventType et) ;
private:
	pt_STL_map(EEventType,CXCCopyingEventReceiver*) 	m_RecerMap ;
	CptCritiSecLock			m_MapLock ;
	CXCCopyingEventReceiver*						m_pDataOccuredRecever ; // 基于性能考虑，把数据变化回调指针专门处理
};

class CXCCopyingEventReceiver
{
public:
	virtual int OnCopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1=NULL,void* pParam2=NULL) = 0 ;
};