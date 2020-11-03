/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCLocalFileAsynDestnationFilter2.h"
#include "XCCopyingEvent.h"
#include "..\Common\ptGlobal.h"
#include "XCFileDataBuffer.h"

//
//CXCLocalFileAsynDestnationFilter2::CXCLocalFileAsynDestnationFilter2(CXCCopyingEvent* pEvent,const CptString strDestRoot,const SStorageInfoOfFile& siof,const bool bIsRenameCopy,const EImpactFileBehaviorResult ifbr)
//	:CXCLocalFileDestnationFilter(pEvent,strDestRoot,siof,bIsRenameCopy,ifbr),m_pPreDstFileInfo(NULL),m_nPreOverlapIndex(0)
//	,m_bWaitForCreateFile(false),m_hBeginWaitCreateFile(NULL),m_hEndWaitCreateFile(NULL),m_hAPCEvent(NULL)
//{
//#ifdef _DEBUG
//	m_nPreWaitCount = 0 ;
//#endif
//}
//
//CXCLocalFileAsynDestnationFilter2::~CXCLocalFileAsynDestnationFilter2(void)
//{
//	if(m_hBeginWaitCreateFile!=NULL)
//	{
//		::SetEvent(m_hBeginWaitCreateFile) ;
//		::CloseHandle(m_hBeginWaitCreateFile) ;
//		m_hBeginWaitCreateFile = NULL ;
//	}
//
//	if(m_hEndWaitCreateFile!=NULL)
//	{
//		::SetEvent(m_hEndWaitCreateFile) ;
//		::CloseHandle(m_hEndWaitCreateFile) ;
//		m_hEndWaitCreateFile = NULL ;
//	}
//}
//
//bool CXCLocalFileAsynDestnationFilter2::OnInitialize() 
//{
//	m_nCreateFileFlag = FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED;
//
//	::memset(m_xcOverlap,0,sizeof(m_xcOverlap)) ;
//
//	m_nCurOverlapIndex = 0 ;
//
//	if(m_hAPCEvent==NULL)
//	{
//		m_hAPCEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;
//	}
//
//	return CXCLocalFileDestnationFilter::OnInitialize() ;
//}
//
//
//bool CXCLocalFileAsynDestnationFilter2::OnContinue() 
//{
//	return CXCLocalFileDestnationFilter::OnContinue() ;
//}
//
//bool CXCLocalFileAsynDestnationFilter2::OnPause() 
//{
//	return CXCLocalFileDestnationFilter::OnPause() ;
//}
//
//
//void CXCLocalFileAsynDestnationFilter2::OnStop() 
//{
//	if(m_hBeginWaitCreateFile!=NULL)
//	{
//		::SetEvent(m_hBeginWaitCreateFile) ;
//		m_hBeginWaitCreateFile = NULL ;
//	}
//
//	if(m_hEndWaitCreateFile!=NULL)
//	{
//		::SetEvent(m_hEndWaitCreateFile) ;
//		m_hEndWaitCreateFile = NULL ;
//	}
//
//	if(m_hAPCEvent!=NULL)
//	{
//		::SetEvent(m_hAPCEvent) ;
//		::CloseHandle(m_hAPCEvent) ;
//		m_hAPCEvent = NULL ;
//	}
//
//	CXCLocalFileDestnationFilter::OnStop() ;
//}
//
//bool CXCLocalFileAsynDestnationFilter2::WaitForPreDataDone() 
//{
//	bool bRet = true ;
//
//	//Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::WaitForPreDataDone() begin")) ;
//	//::OutputDebugString(_T("CXCLocalFileAsynDestnationFilter2::WaitForPreDataDone() begin\r\n")) ;
//
//	if(m_pPreDstFileInfo!=NULL)
//	{
//		bRet = false ;
//EXCEPTION_RETRY_WRITEDATA:
//
//		_ASSERT(m_hAPCEvent!=NULL) ;
//
//		if(::WaitForSingleObjectEx(m_hAPCEvent,INFINITE,TRUE)==WAIT_IO_COMPLETION) 
//		{
//			_ASSERT(--m_nPreWaitCount==0) ;
//
//			if(m_xcOverlap[m_nPreOverlapIndex].dwOperSize==0 && this->IsValideRunningState())
//			{// 如果返回的长度为0，则说明写入失败
//				if(this->m_pEvent!=NULL)
//				{
//					SXCExceptionInfo ei ;
//
//					ei.strSrcFile = m_pPreDstFileInfo->strFileName ;
//					ei.uFileID = m_pPreDstFileInfo->uFileID ;
//
//					ei.ErrorCode.nSystemError = 0 ;
//					ei.ErrorCode.AppError = CopyFileErrorCode_WriteFileFailed ;
//					ei.SupportType = ErrorHandlingFlag_Exit ;
//
//					ErrorHandlingResult result = m_pEvent->XCOperation_CopyExcetption(ei) ;
//					
//					switch(result)
//					{
//					case ErrorHandlingFlag_Ignore: // 忽略
//						break ;
//
//					default:
//					case ErrorHandlingFlag_Exit: // 退出
//						*this->m_pRunningState = CFS_ReadyStop ;
//						break ;
//					}
//				}
//
//				return false ;
//			}
//
//			if(m_pEvent!=NULL && this->CanCallbackFileInfo())
//			{// 往上回调数据
//				SFileDataOccuredInfo fdoi ;
//
//				fdoi.bReadOrWrite = false ;
//				fdoi.nDataSize = m_xcOverlap[m_nPreOverlapIndex].dwOperSize ;
//				fdoi.uFileID = m_pPreDstFileInfo->uFileID ;
////				fdoi.strFileName = m_pPreDstFileInfo->strFileName ;
//
//				//Win_Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::WaitForPreDataDone() id=%u"),fdoi.uFileID) ;
//
//				m_pEvent->XCOperation_FileDataOccured(fdoi) ;
//			}
//
//			// 释放数据
//			m_pFileDataBuf->Free(m_xcOverlap[m_nPreOverlapIndex].pBuf,m_xcOverlap[m_nPreOverlapIndex].nBufSize) ;
//			//m_pFileDataBuf->Free(m_xcOverlap[m_nPreOverlapIndex].pBuf,m_xcOverlap[m_nPreOverlapIndex].dwOperSize) ;
//			m_pPreDstFileInfo->uRemainSize -= m_xcOverlap[m_nPreOverlapIndex].dwOperSize ;
//			m_pPreDstFileInfo->uWriteBeginPos += m_xcOverlap[m_nPreOverlapIndex].dwOperSize ;
//
//			m_pPreDstFileInfo = NULL ;
//
//			// 因为写入数据与创建文件在同一时刻只能有一个发生，
//			// 所以当数据写入后，可能有创建文件的请求在等待
//			bRet = this->SetCreateFileEvent() ; 
//		}
//		else
//		{
//			//Debug_Printf(_T("CXCLocalFileDestnationFilter::WaitForPreDataDone() ")) ;
//			//::OutputDebugString(_T("CXCLocalFileAsynDestnationFilter2::WaitForPreDataDone() \r\n")) ;
//
//			if(this->m_pEvent!=NULL)
//			{
//				SXCExceptionInfo ei ;
//
//				ei.strSrcFile = m_pPreDstFileInfo->strFileName ;
//				ei.uFileID = m_pPreDstFileInfo->uFileID ;
//				ei.uFileID = m_pPreDstFileInfo->uFileID ;
//				ei.ErrorCode.nSystemError = ::GetLastError() ;
//				ei.ErrorCode.AppError = CopyFileErrorCode_WriteFileFailed ;
//				ei.SupportType = ErrorHandlingFlag_IgnoreCancel ;
//
//				ErrorHandlingResult result = m_pEvent->XCOperation_CopyExcetption(ei) ;
//
//				switch(result)
//				{
//				case ErrorHandlingFlag_Ignore: // 忽略
//					break ;
//
//				case ErrorHandlingFlag_Retry: // 重试（写文件）
//					goto EXCEPTION_RETRY_WRITEDATA ;
//
//				default:
//				case ErrorHandlingFlag_Exit: // 退出
//					*this->m_pRunningState = CFS_ReadyStop ;
//					break ;
//				}
//			}
//			bRet = false ;
//			//_ASSERT(FALSE) ;
//		}
//	}
//	else
//	{
//		bRet = this->SetCreateFileEvent() ;
//	}
//
//	//::OutputDebugString(_T("CXCLocalFileAsynDestnationFilter2::WaitForPreDataDone() end \r\n")) ;
//	//Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::WaitForPreDataDone() end")) ;
//
//	return bRet ;
//}
//
//int CXCLocalFileAsynDestnationFilter2::OnCreateXCFile(SDataPack_CreateFileInfo& cfi)
//{
//	int nRet = 0 ;
//
//	// 这里是为了保证创建文件和写文件数据在同一时刻只能有一个在发生，所以才用两个事件通知来实现
//	if(m_hBeginWaitCreateFile==NULL)
//	{
//		m_hBeginWaitCreateFile = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;
//		m_hEndWaitCreateFile = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;
//
//		nRet = CXCLocalFileDestnationFilter::OnCreateXCFile(cfi)  ;
//	}
//	else
//	{
//		nRet = CXCLocalFileDestnationFilter::OnCreateXCFile(cfi)  ;
//		/**
//		m_bWaitForCreateFile = true ;
//
//		Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::OnCreateXCFile() 1")) ;
//
//		bool bLoop = true ;
//
//		while(bLoop)
//		{
//			switch(::WaitForSingleObject(m_hBeginWaitCreateFile,1*1000))
//			{
//			case WAIT_OBJECT_0:
//				{
//					if(this->IsValideRunningState())
//					{
//						nRet = CXCLocalFileDestnationFilter::OnCreateXCFile(cfi)  ;
//					}
//					::SetEvent(m_hEndWaitCreateFile) ;
//				}
//				bLoop = false ;
//				break ;
//
//			case WAIT_TIMEOUT:
//				bLoop = this->IsValideRunningState() ;
//				break ;
//
//			default:
//				bLoop = false ;
//				break ;
//			}
//		}
//		/**/
//	}
//
//	Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::OnCreateXCFile() end")) ;
//
//	return nRet;
//}
//
//bool CXCLocalFileAsynDestnationFilter2::SetCreateFileEvent() 
//{
//	if(m_bWaitForCreateFile)
//	{
//		m_bWaitForCreateFile = false ;
//
//		//Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::SetCreateFileEvent() 1")) ;
//
//		::SetEvent(m_hBeginWaitCreateFile) ;
//
//		Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::SetCreateFileEvent() ::WaitFor(m_hEndWaitCreateFile)")) ;
//
//		while(::WaitForSingleObject(m_hEndWaitCreateFile,1000)==WAIT_TIMEOUT && this->IsValideRunningState()) ;
//
//		//::WaitForSingleObject(m_hEndWaitCreateFile,INFINITE) ;
//
//		return this->IsValideRunningState() ;
//	}
//
//	return true ;
//}
//
//int CXCLocalFileAsynDestnationFilter2::WriteFileData(SDataPack_FileData& fd) 
//{
//	int nRet = 0 ;
//
//	/**
//	//::OutputDebugString(_T("CXCLocalFileAsynDestnationFilter2::WriteFileData() begin \r\n")) ;
//	//Debug_Printf(_T("CXCLocalFileDestnationFilter::WriteFileData() begin")) ;
//
//	if(!this->IsValideRunningState())
//	{
//		return ErrorHandlingFlag_Exit ;
//	}
//
//	_ASSERT(!m_FileInfoList.empty()) ;
//
//	if(m_CurFileIterator==m_FileInfoList.end()|| m_pCurDstFileInfo->uFileID!=fd.uFileID)
//	{
//		pt_STL_map(unsigned,SDstFileInfo)::iterator& it = m_FileInfoMap.find(fd.uFileID) ;
//
//		if(it!=m_FileInfoMap.end())
//		{
//			m_pCurDstFileInfo = &(*it).second ;
//
//			//::OutputDebugString(_T("CXCLocalFileAsynDestnationFilter2::WriteFileData() new id \r\n")) ;
//			//Debug_Printf(_T("CXCLocalFileDestnationFilter::WriteFileData() new_id=%u"),m_pCurDstFileInfo->uFileID) ;
//		}
//		else
//		{
//			_ASSERT(FALSE) ;
//			//m_pCurDstFileInfo = NULL ;
//
//			//::OutputDebugString(_T("CXCLocalFileAsynDestnationFilter2::WriteFileData() 3 \r\n")) ;
//			//Debug_Printf(_T("CXCLocalFileDestnationFilter::WriteFileData() 3")) ;
//		}
//	}
//	else if(fd.pData==NULL && m_pCurDstFileInfo!=NULL && m_pCurDstFileInfo->uFileID==fd.uFileID)
//	{// 上游已抛弃该文件,所以文件在这里也该抛弃
//		::CloseHandle(m_pCurDstFileInfo->hFile) ;
//		::DeleteFile(m_pCurDstFileInfo->strFileName) ;
//		m_FileInfoMap.erase(fd.uFileID) ;
//
//		m_pCurDstFileInfo = NULL ;
//
//		return 0 ;
//	}
//
//	_ASSERT(m_pCurDstFileInfo!=NULL) ;
//
//	//if(m_FileInfoMap.find(fd.uFileID)!=m_FileInfoMap.end())
//	if(m_pCurDstFileInfo!=NULL)
//	{
//		//SDstFileInfo& dfi = m_FileInfoMap[fd.uFileID] ;
//
//		DWORD dwWrite = 0 ;
//
//		int nSize = fd.nDataSize ;
//
//		if(m_pCurDstFileInfo->bNoBuf)
//		{//不使用系统缓冲,则进行扇区大小对齐
//			nSize = ALIGN_SIZE_UP(fd.nDataSize,m_StorageInfo.nSectorSize) ;
//		}
//
//EXCEPTION_RETRY_WRITEDATA:// 重试（写文件）
//		//Debug_Printf(_T("CXCLocalFileDestnationFilter::WriteFileData() size=%d"),nSize) ;
//
//		if(!this->WaitForPreDataDone())
//		{// 写入失败则抛弃该文件
//
////			_ASSERT(FALSE) ;
//			//Debug_Printf(_T("CXCLocalFileDestnationFilter::WriteFileData() failed")) ;
//			//::OutputDebugString(_T("CXCLocalFileAsynDestnationFilter2::WriteFileData() failed \r\n")) ;
//
//			if(fd.uFileID==m_pPreDstFileInfo->uFileID)
//			{
//				fd.bDiscard = true ;
//
//				::CloseHandle(m_pCurDstFileInfo->hFile) ;
//				::DeleteFile(m_pCurDstFileInfo->strFileName) ;
//				m_FileInfoMap.erase(fd.uFileID) ;
//
//				m_pCurDstFileInfo = NULL ;
//				return 0 ;
//			}
////			_ASSERT(FALSE) ;
//		}
//
//		::memset(&m_xcOverlap[m_nCurOverlapIndex].ov,0,sizeof(m_xcOverlap[m_nCurOverlapIndex].ov)) ;
//
//		CptGlobal::Int64ToDoubleWord(m_pCurDstFileInfo->uWriteBeginPos,m_xcOverlap[m_nCurOverlapIndex].ov.OffsetHigh,m_xcOverlap[m_nCurOverlapIndex].ov.Offset) ;
//
//		m_xcOverlap[m_nCurOverlapIndex].pBuf = fd.pData ;
//		m_xcOverlap[m_nCurOverlapIndex].nBufSize = fd.nBufSize ; // 注意这里是用 nBufSize,而不是nDataSize
//		
//		m_xcOverlap[m_nCurOverlapIndex].ov.hEvent = m_hAPCEvent ;
//
//		if( ::WriteFileEx(m_pCurDstFileInfo->hFile,(BYTE*)fd.pData,nSize,(OVERLAPPED*)&m_xcOverlap[m_nCurOverlapIndex],FileIOCompletionRoutine))
//		{
//			_ASSERT(++m_nPreWaitCount==1) ;
//
//			//::OutputDebugString(_T("CXCLocalFileAsynDestnationFilter2::WriteFileData() after WriteFileEx() error_code=%d\r\n")) ;
//			//Win_Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::WriteFileData() after WriteFileEx() error_code=%d"),GetLastError()) ;
//			//Debug_Printf(_T("CXCLocalFileDestnationFilter::WriteFileData() after WriteFileEx()")) ;
//
//			m_pPreDstFileInfo = m_pCurDstFileInfo ;
//			
//			m_nPreOverlapIndex = m_nCurOverlapIndex ;
//			m_nCurOverlapIndex = m_nCurOverlapIndex==0 ? 1 : 0 ;
//			::memset(&m_xcOverlap[m_nCurOverlapIndex],0,sizeof(SXCAsynOverlapped)) ;
//		}
//		else
//		{
//			//_ASSERT(FALSE) ;
//			//::OutputDebugString(_T("CXCLocalFileAsynDestnationFilter2::WriteFileData() failed \r\n")) ;
//			//Debug_Printf(_T("CXCLocalFileDestnationFilter::WriteFileData() failed")) ;
//			if(this->m_pEvent!=NULL)
//			{
//				SXCExceptionInfo ei ;
//
//				ei.uFileID = m_pCurDstFileInfo->uFileID ;
//				ei.ErrorCode.nSystemError = ::GetLastError() ;
//				ei.SupportType = ErrorHandlingFlag_RetryIgnoreCancel ;
//
//				ErrorHandlingResult result = m_pEvent->XCOperation_CopyExcetption(ei) ;
//
//				switch(result)
//				{
//				case ErrorHandlingFlag_Ignore: // 忽略
//					nRet = ErrorHandlingFlag_Ignore ;
//					break ;
//
//				case ErrorHandlingFlag_Retry: // 重试（写文件）
//					goto EXCEPTION_RETRY_WRITEDATA ;
//
//				default:
//				case ErrorHandlingFlag_Exit: // 退出
//					*this->m_pRunningState = CFS_ReadyStop ;
//					return ErrorHandlingFlag_Exit ;
//				}
//			}
//		}
//
//		//Debug_Printf(_T("file id: %u  remain size: %u "),fd.uFileID,dfi.uRemainSize) ;
//	}
//	else
//	{
//		_ASSERT(FALSE) ;
//	}
//
//	//::OutputDebugString(_T("CXCLocalFileAsynDestnationFilter2::WriteFileData() end \r\n")) ;
//	//Debug_Printf(_T("CXCLocalFileDestnationFilter::WriteFileData() end")) ;
//	/**/
//	return nRet ;
//}
//
//void CXCLocalFileAsynDestnationFilter2::FileIOCompletionRoutine(DWORD dwErrorCode,DWORD dwNumberOfBytesTransfered,OVERLAPPED* lpOverlapped) 
//{
//	_ASSERT(lpOverlapped!=NULL) ;
//
//	//Debug_Printf(_T("Write FileIOCompletionRoutine() %u"),dwNumberOfBytesTransfered) ;
//
//	SXCAsynOverlapped* ov = (SXCAsynOverlapped*)lpOverlapped ;
//
//	ov->dwOperSize = dwNumberOfBytesTransfered ;
//
//	if(dwNumberOfBytesTransfered>0)
//	{
//		//_ASSERT((ov->nBufSize-dwNumberOfBytesTransfered)<512) ;
//	}	
//}
//
//
//void CXCLocalFileAsynDestnationFilter2::OnLinkEnded(pt_STL_list(SDataPack_SourceFileInfo*)& FileList) 
//{
//	Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::OnLinkEnded() 1")) ;
//	if(!FileList.empty())
//	{
//		Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::OnLinkEnded() 2")) ;
//
//		// 因为是异步处理，当LinkEnded命令发出时，可能有文件未被确认释放（事实上，命令发出后一瞬间已确认释放了），
//		// 所以就会出现 FileList 有文件ID，但却已释放了的情况
//		if(this->WaitForPreDataDone() && !m_FileInfoMap.empty())
//		{
//			this->RoundOffFile(FileList) ;
//		}
//	}
//
//	Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::OnLinkEnded() end")) ;
//}
//
//
//int CXCLocalFileAsynDestnationFilter2::OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData)
//{
//	Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::OnDataTrans() begin")) ;
//
//	switch(cmd)
//	{
//	case EDC_FileOperationCompleted:
//		{
//			this->WaitForPreDataDone() ;
//		}
//		break ;
//
//	case EDC_FileData:
//		{
//			if(pFileData==NULL && this->IsValideRunningState())
//			{
//				this->WaitForPreDataDone() ;
//				return 0 ;
//			}
//		}
//		break ;
//	}
//
//	int nRet = 0 ;
//	
//
//	nRet = CXCLocalFileDestnationFilter::OnDataTrans(pSender,cmd,pFileData) ;
//
//	Debug_Printf(_T("CXCLocalFileAsynDestnationFilter2::OnDataTrans() end")) ;
//
//	return nRet;
//}
