/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#pragma once
#include "xcdestinationfilter.h"
#include "..\Common\ptThreadLock.h"

class CXCLocalFileAsynDestnationFilter2 :
	public CXCLocalFileDestnationFilter
{
public:
	CXCLocalFileAsynDestnationFilter2(CXCCopyingEvent* pEvent,const CptString strDestRoot,const SStorageInfoOfFile& siof,const bool bIsRenameCopy,const EImpactFileBehaviorResult ifbr);
	virtual ~CXCLocalFileAsynDestnationFilter2(void);

	virtual int WriteFileData(SDataPack_FileData& fd) ;

protected:
	virtual bool OnInitialize() ;
	virtual bool OnContinue() ;
	virtual bool OnPause();
	virtual void OnStop();

	virtual void OnLinkEnded(pt_STL_list(SDataPack_SourceFileInfo*)& FileList) ;

	virtual int OnCreateXCFile(SDataPack_CreateFileInfo& cfi) ;
	virtual int OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData) ;

private:
	inline bool SetCreateFileEvent() ;
private:
	static void __stdcall FileIOCompletionRoutine(DWORD dwErrorCode,DWORD dwNumberOfBytesTransfered,OVERLAPPED* lpOverlapped) ;

	inline bool WaitForPreDataDone() ;

	struct SFileSizeInfo
	{
		unsigned __int64 uWriteBeginPos ;
		unsigned __int64 uRemainSize ; // 这个不一定是文件实际的大小，当不使用缓冲时则以sector大小的上对齐大小，
										// 当使用缓冲时则为文件的实际大小
	};

private:
	SXCAsynOverlapped	m_xcOverlap[2] ;
	int					m_nCurOverlapIndex ;
	int					m_nPreOverlapIndex ;
	SDstFileInfo*		m_pPreDstFileInfo ;
	bool				m_bWaitForCreateFile ;

	HANDLE				m_hBeginWaitCreateFile ;
	HANDLE				m_hEndWaitCreateFile ;
	HANDLE				m_hAPCEvent ;

#ifdef _DEBUG
	int		m_nPreWaitCount ;
#endif
};