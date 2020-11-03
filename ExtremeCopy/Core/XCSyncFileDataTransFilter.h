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
#include "XCFileDataCacheTransFilter.h"

class CXCSyncFileDataTransFilter :
	public CXCFileDataCacheTransFilter
{
public:
	CXCSyncFileDataTransFilter(CXCCopyingEvent* pEvent);
	virtual ~CXCSyncFileDataTransFilter(void);

protected:
	virtual int OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData) ;

private:
	int FlushCmd() ;

	virtual bool OnInitialize() ;
	virtual bool OnContinue() ;
	virtual bool OnPause();
	virtual void OnStop();

private:
	
	pt_STL_list(SDataPack_SourceFileInfo*)						m_CacheFileOperCompList ; // 用来缓存从source filter 发来的 EDC_FileOperationCompleted 命令	
};

