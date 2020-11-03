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
#include "../Common/ptThreadLock.h"

class CXCFileDataCacheTransFilter:
	public CXCTransformFilter
{
public:
	CXCFileDataCacheTransFilter(CXCCopyingEvent* pEvent);
	virtual ~CXCFileDataCacheTransFilter(void);

	virtual bool Connect(CXCFilter* pFilter,bool bUpstream) ;

protected:
	virtual bool OnInitialize() ;
	virtual bool OnContinue() ;
	virtual bool OnPause();
	virtual void OnStop();
	virtual void OnLinkEnded(pt_STL_list(SDataPack_SourceFileInfo*)& FileList) ;

	class CNullObject {};

	virtual int OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData) ;

	int WriteFileData(const EFilterCmd cmd,void* pFileData) ;

	void* AllocCmdPackMem(int nSize,CptString strSourceFile,int nFileID) ;
	void FreeCmdPackMem(void* pBuf) const;

	void FlushFileOperCompletedCommand(const pt_STL_list(SDataPack_SourceFileInfo*)& CompleteCmdList) ;
	void SendCacheBathCreateCommand() ;
	bool IsHaveCreateFileCache() const;

	/**
	template<typename T>
	inline int PushToCacheCmdQue(const EFilterCmd& cmd,void* pBuf,void* pFileData) 
	{
		_ASSERT(pBuf!=NULL) ;

		if(pBuf==NULL)
		{
			return ErrorHandlingFlag_Exit ; // ÍË³ö ExtremeCopy
		}

		T* pSfi = new (pBuf)T ;

		*pSfi = *((T*)pFileData) ;
		SCacheCommandInfo cci ;

		cci.cmd = cmd ;
		cci.pCmdData = pBuf ;

		if(m_bCacheCmdThreadSafe)
		{
			CptAutoLock lock(&m_CacheCmdLock) ;
			m_CacheCmdQue.push_back(cci) ;
		}
		else
		{
			m_CacheCmdQue.push_back(cci) ;
		}

		return 0 ;
	}

	template<>
	int PushToCacheCmdQue<CNullObject>(const EFilterCmd& cmd,void* pBuf,void* pFileData) 
	{
		SCacheCommandInfo cci ;
		cci.cmd = cmd ;
		cci.pCmdData = pFileData ;

		if(m_bCacheCmdThreadSafe)
		{
			CptAutoLock lock(&m_CacheCmdLock) ;
			m_CacheCmdQue.push_back(cci) ;
		}
		else
		{
			m_CacheCmdQue.push_back(cci) ;
		}

		return 0 ;
	}

	template<typename T>
	inline int PushToCacheCmdQueWithAlloc(const EFilterCmd& cmd,void* pFileData) 
	{
		void* pBuf = this->AllocCmdPackMem(sizeof(T),_T(""),((T*)pFileData)->uFileID) ;

		return PushToCacheCmdQue<T>(cmd,pBuf,pFileData) ;
	}
	

protected:
	struct SCacheCommandInfo
	{
		EFilterCmd	cmd ;
		void*	pCmdData ;
	};

private:
	
	struct SShareCacheAllocator
	{
		HANDLE		hHeap ;
		int			nRef ;
	};
/**/

protected:
	CXCFilterEventCB*						m_pUpstreamFilter ;
	CXCFilterEventCB*						m_pDownstreamFilter ;

	pt_STL_deque(SDataPack_FileData)		m_FileDataQue ;
	bool									m_bCacheCmdThreadSafe ;
	CXCFileDataBuffer*						m_pFileDataBuf ;

private:
	SDataPack_CreateFileInfo				m_CacheCreateFileInfo ;
	bool									m_bHaveCreateFileCmd ;

	CptCritiSecLock							m_CacheCmdLock ;
};

