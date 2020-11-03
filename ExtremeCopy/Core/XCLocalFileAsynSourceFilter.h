/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#pragma once
#include "xcsourcefilter.h"

#include "..\Common\ptTypeDef.h"
#include <list>
#include <deque>

class CXCLocalFileAsynSourceFilter :
	public CXCLocalFileSourceFilter
{
public:
	CXCLocalFileAsynSourceFilter(CXCCopyingEvent* pEvent);
	virtual ~CXCLocalFileAsynSourceFilter(void);

protected:
		virtual bool OnInitialize() ;

	virtual bool OnRun() ;
		
	virtual bool OnPause() ;
	virtual void OnStop() ;

private:
	bool FlushFileData() ;
	
	static void __stdcall FileIOCompletionRoutine(DWORD dwErrorCode,DWORD dwNumberOfBytesTransfered,OVERLAPPED* lpOverlapped) ;

	struct SAllocBufInfo
	{
		char*	pBuf ;
		DWORD	dwBufSize ;
	};

private:
	SAllocBufInfo		m_NextAllocBuf ;
	HANDLE				m_APCEvent ;
};