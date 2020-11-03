/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCAsyncFileDataTransFilter.h"
#include "XCFileDataBuffer.h"

CXCAsyncFileDataTransFilter::CXCAsyncFileDataTransFilter(CXCCopyingEvent* pEvent)
	:CXCFileDataCacheTransFilter(pEvent),m_hWaitForFileBufNotFull(NULL),
	m_hThread(NULL),m_hWriteDataReadyEvent(NULL),m_hLinkEndForQueEmpty(NULL),m_uFirstCompleteFileID(0),m_bWriteThreadEnd(false)
{
}


CXCAsyncFileDataTransFilter::~CXCAsyncFileDataTransFilter(void)
{
	m_bWriteThreadEnd = true;

	if(m_hThread!=NULL)
	{
		::SetEvent(m_hWriteDataReadyEvent) ;
		::CloseHandle(m_hWriteDataReadyEvent) ;
		m_hWriteDataReadyEvent = NULL ;

		if(m_hWaitForFileBufNotFull!=NULL)
		{
			::SetEvent(m_hWaitForFileBufNotFull) ;
			::CloseHandle(m_hWaitForFileBufNotFull) ;
			m_hWaitForFileBufNotFull = NULL ;
		}

		if(m_hLinkEndForQueEmpty!=NULL)
		{
			::SetEvent(m_hLinkEndForQueEmpty) ;
			::CloseHandle(m_hLinkEndForQueEmpty) ;
			m_hLinkEndForQueEmpty = NULL ;
		}

		::WaitForSingleObject(m_hThread,3*1000) ;
		::CloseHandle(m_hThread) ;
		m_hThread = NULL ;
	}
}

bool CXCAsyncFileDataTransFilter::OnContinue() 
{
	return CXCFileDataCacheTransFilter::OnContinue() ;
}

bool CXCAsyncFileDataTransFilter::OnPause() 
{
	return CXCFileDataCacheTransFilter::OnPause() ;
}


void CXCAsyncFileDataTransFilter::OnStop() 
{
	m_bWriteThreadEnd = true;
	if(m_hThread!=NULL)
	{
		::SetEvent(m_hWriteDataReadyEvent) ;
		::CloseHandle(m_hWriteDataReadyEvent) ;
		m_hWriteDataReadyEvent = NULL ;

		if(m_hWaitForFileBufNotFull!=NULL)
		{
			::SetEvent(m_hWaitForFileBufNotFull) ;
			::CloseHandle(m_hWaitForFileBufNotFull) ;
			m_hWaitForFileBufNotFull = NULL ;
		}

		if(m_hLinkEndForQueEmpty!=NULL)
		{
			::SetEvent(m_hLinkEndForQueEmpty) ;
			::CloseHandle(m_hLinkEndForQueEmpty) ;
			m_hLinkEndForQueEmpty = NULL ;
		}

		::WaitForSingleObject(m_hThread,3*1000) ;
		::CloseHandle(m_hThread) ;
		m_hThread = NULL ;
	}

	CXCFileDataCacheTransFilter::OnStop() ;
}

bool CXCAsyncFileDataTransFilter::OnInitialize() 
{
	m_bLinkEnded = false ;
	if(m_hWriteDataReadyEvent==NULL)
	{
		m_hWriteDataReadyEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;
	}

	if(m_hThread==NULL)
	{
		m_bWriteThreadEnd = false;
		m_hThread = (HANDLE)::_beginthreadex(NULL,0,ThreadFunc,this,0,NULL) ;
	}

	return CXCFileDataCacheTransFilter::OnInitialize() ;
}

int CXCAsyncFileDataTransFilter::OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData) 
{
	int nRet = 0 ;

	switch(cmd)
	{
	case EDC_FlushData:
		::Sleep(1) ;
		break ;

	case EDC_BatchCreateFile:
		{
			CptAutoLock lock(&m_CreateBatchFileLock) ;

			nRet = CXCFileDataCacheTransFilter::OnDataTrans(pSender,cmd,pFileData) ;

			{
				CptAutoLock lock(&m_FileDataQueLock) ;

				bool bNeedWaitUp = m_FileDataQue.empty() ;

				SDataPack_FileData fd ;

				fd.pData = NULL ;
				fd.uFileID = 0 ;
				fd.nBufSize = 1 ; // create file cmd

				m_FileDataQue.push_back(fd) ;

				if(bNeedWaitUp)
				{// 唤醒等待的写线程
					::SetEvent(m_hWriteDataReadyEvent) ;
				}
			}

			return nRet ;
		}
		break ;

	case EDC_FileData: // 文件数据
		{
			SDataPack_FileData* pFD = (SDataPack_FileData*)pFileData ;

			_ASSERT(pFD!=NULL) ;

			bool bNeedWaitUp = false ;

			{
				CptAutoLock lock(&m_FileDataQueLock) ;

				bNeedWaitUp = m_FileDataQue.empty() ;

				CXCFileDataCacheTransFilter::OnDataTrans(pSender,cmd,pFileData) ;
			}
			
			if(bNeedWaitUp)
			{// 唤醒等待的写线程
				::SetEvent(m_hWriteDataReadyEvent) ;
			}
		}

		return 0 ;

	case EDC_FileOperationCompleted: // 作用于该文件的操作已完成
		{
			SDataPack_FileOperationCompleted* pFD = (SDataPack_FileOperationCompleted*)pFileData ;

			SCompleteCmdCacheInfo ccci ;

			CptAutoLock lock(&m_FileOperCompleteCacheLock) ;

			ccci.CacheCompleteCmdList = pFD->CompletedFileInfoList ;

			m_CacheCompleteCmdList.push_back(ccci) ; // 只把命令缓冲起来

			{
				CptAutoLock lock(&m_FileDataQueLock) ;

				bool bNeedWaitUp = m_FileDataQue.empty() ;

				SDataPack_FileData fd ;

				fd.pData = NULL ;
				fd.uFileID = 0 ;
				fd.nBufSize = 2 ; // complete cmd

				m_FileDataQue.push_back(fd) ;

				if(bNeedWaitUp)
				{// 唤醒等待的写线程
					::SetEvent(m_hWriteDataReadyEvent) ;
				}
			}

			return 0 ;
		}

		return nRet ;


	case EDC_LinkEnded:
		{
			SMsgPack mp ;

			mp.cmd = EDC_LinkEnded ;
			mp.pFileData = pFileData ;

			HANDLE hMsg = NULL ;

			// 放入队列，等待由OutputWorkThread完成

			{
				CptAutoLock lock(&m_FileDataQueLock) ;

				hMsg = m_IdelMsgQue.AsynSendMsg(&mp) ;

				if(m_FileDataQue.empty())
				{// 如果 OutputWorkdThread 已静止，那么将其唤醒
					::SetEvent(m_hWriteDataReadyEvent) ;
				}
			}

			m_IdelMsgQue.WaitForAsynResult(hMsg,&nRet) ;

		}
		return nRet ;

	}

	return CXCFileDataCacheTransFilter::OnDataTrans(pSender,cmd,pFileData) ;
}

int CXCAsyncFileDataTransFilter::ProcessLinkedEnd(SDataPack_FileOperationCompleted* pFoc) 
{// 移除因为source filter延迟收到 EDC_FileDoneConfirm 命令，
	//所引起在 EDC_LinkEnded 携带了需要向下再次触发 EDC_FileOperationCompleted 所带来的问题

	CptAutoLock lock(&m_FileOperCompleteCacheLock) ;
	CptAutoLock lock2(&m_FileDataQueLock) ;

	SDataPack_FileOperationCompleted fdc ;

	if(!m_CacheCompleteCmdList.empty())
	{
		pt_STL_list(SDataPack_SourceFileInfo*)& CompleteList = (*m_CacheCompleteCmdList.begin()).CacheCompleteCmdList ;

		pt_STL_list(SDataPack_SourceFileInfo*)::iterator it = CompleteList.begin() ;//m_CacheFileOperCompList.begin() ;
		pt_STL_list(SDataPack_SourceFileInfo*)::iterator it2 = pFoc->CompletedFileInfoList.begin() ;

		for(;it!=CompleteList.end() && !pFoc->CompletedFileInfoList.empty();++it)
		{
			it2 = pFoc->CompletedFileInfoList.begin() ;

			for(;it2!=pFoc->CompletedFileInfoList.end();++it2)
			{
				if((*it2)->uFileID==(*it)->uFileID)
				{
					pFoc->CompletedFileInfoList.erase(it2) ;
					break ;
				}
			}
		}

		it2 = pFoc->CompletedFileInfoList.begin() ;

		for(;it2!=pFoc->CompletedFileInfoList.end();++it2)
		{
			CompleteList.push_back(*it2) ;
		}

		if(!CompleteList.empty())
		{
			fdc.CompletedFileInfoList = CompleteList ;
			m_CacheCompleteCmdList.clear() ;
		}
	}

	return m_pDownstreamFilter->OnDataTrans(this,EDC_LinkEnded,&fdc) ;
}

void CXCAsyncFileDataTransFilter::OutputWorkThread() 
{
	DWORD dwWaitResult = WAIT_OBJECT_0 ;
	SDataPack_FileData fd ;
	bool bWait = false ;
	unsigned uCurFileID = 0 ;

	::memset(&fd,0,sizeof(fd)) ;

	bool bCreateOrComplete = true ;
	void* pLastDataPointer = NULL ;

	do
	{
		PT_BREAK_IF(m_bWriteThreadEnd);
		PT_BREAK_IF(!this->IsValideRunningState());

		{// 这个中括号不能去掉， 因为这段代码是线程安全的
			CptAutoLock lock(&m_FileDataQueLock) ;

			bWait = m_FileDataQue.empty() ;

			if(!bWait)
			{
				_ASSERTE( _CrtCheckMemory( ) );

				fd = m_FileDataQue.front() ;
				m_FileDataQue.pop_front() ;
			}
		}

		// 因为 处理complete command命令时有通知 source filter做释放处理，且经过多个公共加锁，所以会出现死锁情况
		if(bWait && m_uFirstCompleteFileID+1<fd.uFileID && m_uFirstCompleteFileID>0)
		{// 如果遇到source file 文件完成命令与新的文件ID一致的话，则发送文件完成命令先
			m_uFirstCompleteFileID = 0 ;
		}

		PT_BREAK_IF(m_bWriteThreadEnd || !this->IsValideRunningState()); // 随时可能退出线程

		if(bWait)
		{
			SMsgPack* pMp = (SMsgPack*)m_IdelMsgQue.BeginMsg() ;

			if(pMp!=NULL)
			{
				if(pMp->cmd==EDC_LinkEnded)
				{
					SDataPack_FileOperationCompleted* pFoc = (SDataPack_FileOperationCompleted*)pMp->pFileData ;

					int nResult = this->ProcessLinkedEnd(pFoc) ;

					m_IdelMsgQue.EndMsg(&nResult) ;

					PT_BREAK_IF(true); // 因为最后的命令已发出，所以就可以直接退出线程
				}
				else
				{
					_ASSERT(FALSE) ;
					m_IdelMsgQue.EndMsg(NULL) ;
				}
			}

			PT_BREAK_IF(m_bWriteThreadEnd || !this->IsValideRunningState()); // 随时可能退出线程

			if(fd.nDataSize>0)
			{// 下面的Filter做一些空闲的工作，例如等待异步IO完成
				m_pDownstreamFilter->OnDataTrans(this,EDC_FileData,NULL) ;

				PT_BREAK_IF(m_bWriteThreadEnd || !this->IsValideRunningState()); // 随时可能退出线程

				CptAutoLock lock(&m_FileDataQueLock) ;

				if(!m_FileDataQue.empty())
				{
					continue ;
				}
			}
			
			dwWaitResult = ::WaitForSingleObject(m_hWriteDataReadyEvent,2000) ;

			PT_BREAK_IF(m_bWriteThreadEnd || !this->IsValideRunningState()); // 随时可能退出线程
		}
		else
		{
			if(fd.uFileID==0 && fd.pData==NULL)
			{
				if(fd.nBufSize==1)
				{// create file
					CptAutoLock lock(&m_CreateBatchFileLock) ;

					this->SendCacheBathCreateCommand() ;
				}
				else if(fd.nBufSize==2)
				{// complete cmd
					if(!m_CacheCompleteCmdList.empty())
					{
						this->FlushFileOperCompletedCommand(m_CacheCompleteCmdList.front().CacheCompleteCmdList) ;

						m_CacheCompleteCmdList.pop_front() ;
					}
				}

				continue ;
			}
			_ASSERTE( _CrtCheckMemory( ) );

			//CptAutoLock lock(&m_CreateFileLock) ; // 因为创建文件和写文件数据是两个不同的线程，所以这里为了HD有最大效率，故只能通行其中一个

EXCEPTION_RETRY_LOCALASYFILEDATA: // 重试

			PT_BREAK_IF(m_bWriteThreadEnd || !this->IsValideRunningState()); // 随时可能退出线程

			pLastDataPointer = fd.pData ;
			// 向下游传数据
			switch(m_pDownstreamFilter->OnDataTrans(this,EDC_FileData,&fd))
			{
			case 0:
				break ;

			case ErrorHandlingFlag_Exit: // 退出
				dwWaitResult= WAIT_FAILED ;
				break ;

			case ErrorHandlingFlag_Ignore: // 忽略
				break ;

			case ErrorHandlingFlag_Retry: // 重试
				goto EXCEPTION_RETRY_LOCALASYFILEDATA ;
				break ;
			}
		}

		_ASSERTE( _CrtCheckMemory( ) );

	}
	while(!m_bWriteThreadEnd || this->IsValideRunningState());

}

UINT CXCAsyncFileDataTransFilter::ThreadFunc(void* pParam)
{
	CXCAsyncFileDataTransFilter* pThis = (CXCAsyncFileDataTransFilter*)pParam ;
	pThis->OutputWorkThread() ;

	return 0 ;
}