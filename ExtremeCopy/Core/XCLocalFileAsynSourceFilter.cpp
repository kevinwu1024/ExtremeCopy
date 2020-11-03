/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
//#include "XCLocalFileAsynSourceFilter.h"
//#include "XCCopyingEvent.h"
//#include "XCFileChangingBuffer.h"
//
//
//CXCLocalFileAsynSourceFilter::CXCLocalFileAsynSourceFilter(CXCCopyingEvent* pEvent):CXCLocalFileSourceFilter(pEvent),m_APCEvent(NULL)
//{
//	::memset(&m_NextAllocBuf,0,sizeof(m_NextAllocBuf)) ;
//}
//
//
//CXCLocalFileAsynSourceFilter::~CXCLocalFileAsynSourceFilter(void)
//{
//	Debug_Printf(_T("CXCLocalFileAsynSourceFilter::~CXCLocalFileAsynSourceFilter()")) ;
//}
//
//bool CXCLocalFileAsynSourceFilter::OnInitialize() 
//{
//	m_nCreateFileFlag = FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_OVERLAPPED ;
//
//	if(m_APCEvent==NULL)
//	{
//		m_APCEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;
//	}
//
//	return CXCLocalFileSourceFilter::OnInitialize() ;
//}
//
//bool CXCLocalFileAsynSourceFilter::OnRun()
//{
//	return CXCLocalFileSourceFilter::OnRun() ;
//}
//
//
//bool CXCLocalFileAsynSourceFilter::OnPause() 
//{
//	//*m_pRunningState = CFS_Pause ;
//	//return true ;
//
//	return CXCLocalFileSourceFilter::OnPause() ;
//}
//
//void CXCLocalFileAsynSourceFilter::OnStop() 
//{
//	//*m_pRunningState = CFS_Stop ;
//	if(m_NextAllocBuf.pBuf!=NULL)
//	{
//		this->m_pFileDataBuf->Free(m_NextAllocBuf.pBuf,m_NextAllocBuf.dwBufSize) ;
//		::memset(&m_NextAllocBuf,0,sizeof(m_NextAllocBuf)) ;
//	}
//
//	if(m_APCEvent!=NULL)
//	{
//		::SetEvent(m_APCEvent) ;
//		::CloseHandle(m_APCEvent) ;
//		m_APCEvent = NULL ;
//	}
//
//	CXCLocalFileSourceFilter::OnStop() ;
//}
//
//// 返回 false 的话，则说明要退出整个复制
//bool CXCLocalFileAsynSourceFilter::FlushFileData() 
//{
//	if(!this->IsValideRunningState())
//	{
//		this->InsideStop() ;
//		return false ;
//	}
//
//	bool bRet = true ;
//
//	unsigned __int64 uRemainSize = 0 ;
//
//	SDataPack_FileData fd ;
//	SFileIDNamePairsInfo fnpi ;
//
//	pt_STL_list(SSourceFileInfo)::iterator& it = m_OpenedFileInfoList.begin() ;
//
//	BOOL bReadResult = FALSE ;
//	const int nFileDataBufPageSize = this->m_pFileDataBuf->GetPageSize() ;
//	//const int nPageSizeTimes = (8*1024*1024)/nFileDataBufPageSize ;
//
//	SDataPack_FileOperationCompleted FileCompletedPack ;
//	DWORD nReadAlignSize = 0 ;
//	bool bMultiple = false ;
//	bool bEOF = false ;
//
//	//pt_STL_list(HANDLE) CloseFileList ;
//	SAllocBufInfo CurAlloc ;
//	m_hCurFileHandle = INVALID_HANDLE_VALUE ;
//	SXCAsynOverlapped xcOverlap[2] ;
//	int nCurOverlapIndex = 0 ;
//	int nPreOverlapIndex = 0 ;
//
//	SFileEndedInfo fei ;
//
//	fei.bReadOrWrite = true ;
//	bool bStop = false ;
//
//	for(;bRet && it!=m_OpenedFileInfoList.end() && !(bStop=!this->IsValideRunningState());++it)
//	{
//		//if((*it).pFpi!=NULL && !(*it).pFpi->bDiscard)
//		{// 如果该文件在创建时期不被抛弃
//			//continue ;
//			if((*it).hFile==INVALID_HANDLE_VALUE)
//			{// 目录
//				if((*it).pFpi==NULL)
//				{// 作用于该文件夹的子文件和文件夹操作已完成, 让 destination filter 的 URI弹栈
//
//					// 该文件复制完成,则加入到完成队列处,并发送 EDC_FileOperationCompleted 命令
//
//					//Win_Debug_Printf(_T("CXCLocalFileAsynSourceFilter::FlushFileData() m_FilePairListLock 1")) ;
//
//					pt_STL_list(SDataPack_FileOperationCompleted) FOCList ;
//
//					{// 在这里牺牲空间，换取 m_FilePairListLock 尽快释放
//						CptAutoLock lock(&m_FilePairListLock) ;
//						pt_STL_list(SFileIDNamePairsInfo)::iterator FolderIt2 = m_ConfirmReleaseFilePairInfoList.begin() ;
//
//						for(;FolderIt2!=m_ConfirmReleaseFilePairInfoList.end();++FolderIt2)
//						{
//							if((*FolderIt2).pFpi->uFileID==(*it).nSectorSize)
//							{// nSectorSize 的值为该URI 文件夹的ID值
//								FileCompletedPack.CompletedFileInfoList.push_back((*FolderIt2).pFpi) ;
//
//								FOCList.push_back(FileCompletedPack) ;
//								//							m_OutputPin.PushData(EDC_FileOperationCompleted,&FileCompletedPack) ;
//								FileCompletedPack.CompletedFileInfoList.clear() ;
//								break ;
//							}
//						}
//					}
//
//					if(!FOCList.empty())
//					{
//						pt_STL_list(SDataPack_FileOperationCompleted)::iterator focit = FOCList.begin() ;
//
//						for(;focit!=FOCList.end();++focit)
//						{
//							m_pDownstreamFilter->OnDataTrans(this,EDC_FileOperationCompleted,&(*focit)) ;
//						}
//					}
//
//					//Win_Debug_Printf(_T("CXCLocalFileAsynSourceFilter::FlushFileData() m_FilePairListLock 2")) ;
//				}
//				else
//				{
//					if((*it).pFpi!=NULL && !(*it).pFpi->bDiscard)
//					{// 加入到"确认缓冲队列"
//						fnpi.nSectorSize = (*it).nSectorSize ;
//						fnpi.pFpi = (*it).pFpi ;
//
//						this->AddToDelayReleaseFileList(fnpi) ;
//					}
//				}
//			}
//			else if((*it).pFpi!=NULL && !(*it).pFpi->bDiscard)
//			{// 文件
//				if((*it).pFpi->nFileSize>0)
//				{
//					uRemainSize = (*it).pFpi->nFileSize ;
//					//fd.uReadBeginPos = 0 ;
//					nCurOverlapIndex = 0 ;
//					nPreOverlapIndex = 0 ;
//
//					nReadAlignSize = this->CalculateAlignSize(uRemainSize,(*it).nSectorSize) ;
//					/**
//					// file data buffer 的页面 是否为 sector 的倍数
//					nReadAlignSize = (DWORD)min((unsigned __int64)m_pFileDataBuf->GetChunkSize()/4,(*it).pFpi->nFileSize) ;
//
//					nReadAlignSize = ALIGN_SIZE_UP(nReadAlignSize,(*it).nSectorSize) ;
//
//					if(m_bSmallReadGran && nReadAlignSize>1024*1024)
//					{// 使用小粒度来读取数据
//						nReadAlignSize /= 4 ;
//					}
//
//					nReadAlignSize = ALIGN_SIZE_UP(nReadAlignSize,(DWORD)m_pFileDataBuf->GetPageSize()) ;
//					/**/
//
//					bEOF = false ;
//
//					::memset(&xcOverlap[nCurOverlapIndex],0,sizeof(xcOverlap[nCurOverlapIndex])) ;
//
//					m_hCurFileHandle = (*it).hFile ;
//					bool bFlush = false ;
//
//					SFileDataOccuredInfo fdoi ;
//
//					fdoi.bReadOrWrite = true ;
//					
//					while(bRet && !(*it).pFpi->bDiscard && !bEOF && uRemainSize>0 && !(bStop=!this->IsValideRunningState()))
//					{
//						//pBuf = pTemBuf ;
//						bFlush = false ;
//						CurAlloc = m_NextAllocBuf ;
//
//						while(CurAlloc.pBuf==NULL && !(bStop=!this->IsValideRunningState()))
//						{
//							CurAlloc.pBuf = this->AllocateFileDataBuf(CurAlloc.dwBufSize,nReadAlignSize,uRemainSize,(*it).nSectorSize) ;//(char*)m_pFileDataBuf->Allocate(m_nValidCachePointNum,dwBufSize) ;//(char*)m_OutputPin.Allocate(dwReadSize) ;
//
//							if(CurAlloc.pBuf==NULL)
//							{// 这说明缓冲区已填满了
//								//Debug_Printf(_T("CXCLocalFileAsynSourceFilter::FlushFileData() EDC_FlushData ")) ;
//								
//								m_pDownstreamFilter->OnDataTrans(this,EDC_FlushData,NULL) ;
//
//								bFlush = true ;
//								//Debug_Printf(_T("CXCLocalFileAsynSourceFilter::FlushFileData()remain: %I64u  IsRunning=%d"),
//								//	uRemainSize,this->IsValideRunningState()?1:0) ;
//
//								if(!this->CheckPauseAndWait())
//								{// 暂停
//									//return false ;
//									bStop = true ;
//									goto STOP_END ;
//								}
//							}
//							else
//							{
//								break ;
//							}
//						}
//
//						if(bStop || CurAlloc.pBuf==NULL)
//						{// 申请缓冲区失败，可能用户选择退出
//							Debug_Printf(_T("CXCLocalFileAsynSourceFilter::FlushFileData() 555")) ;
//							bStop = true ;
//							bRet = false ;
//							break ;
//						}
//
//EXCEPTION_RETRY_READFILE: // 重试（读文件）
//
//						CptGlobal::Int64ToDoubleWord(fd.uReadBeginPos,xcOverlap[nCurOverlapIndex].ov.OffsetHigh,xcOverlap[nCurOverlapIndex].ov.Offset) ;
//
//						if(!this->CheckPauseAndWait())
//						{// 暂停
//							bStop = true ;
//							goto STOP_END ;
//						}
//
//						xcOverlap[nCurOverlapIndex].ov.hEvent = m_APCEvent ;
//
//						//Debug_Printf(_T("CXCLocalFileAsynSourceFilter::FlushFileData() ReadFileEx")) ;
//
//						bReadResult = ::ReadFileEx((*it).hFile,CurAlloc.pBuf,CurAlloc.dwBufSize,(OVERLAPPED*)&xcOverlap[nCurOverlapIndex],FileIOCompletionRoutine) ;
//
//						const int nErrorCode = ::GetLastError() ;
//
//						::memset(&m_NextAllocBuf,0,sizeof(m_NextAllocBuf)) ;
//
//						if(!bReadResult && ERROR_HANDLE_EOF==nErrorCode)
//						{
//							Debug_Printf(_T("ERROR_HANDLE_EOF")) ;
//							bEOF = true ;
//							bReadResult = TRUE ;
//						}
//
//						if(bReadResult)
//						{
//							if(uRemainSize>CurAlloc.dwBufSize)
//							{
//								m_NextAllocBuf.pBuf = this->AllocateFileDataBuf(m_NextAllocBuf.dwBufSize,nReadAlignSize,uRemainSize-CurAlloc.dwBufSize,(*it).nSectorSize) ;
//							}
//							
//							// 交换缓冲区
//							nPreOverlapIndex = nCurOverlapIndex ;
//							nCurOverlapIndex = nCurOverlapIndex==0 ? 1 : 0 ;
//
//							::memset(&xcOverlap[nCurOverlapIndex],0,sizeof(xcOverlap[nCurOverlapIndex])) ;
//
//							const DWORD dwResult = ::WaitForSingleObjectEx(m_APCEvent,INFINITE,TRUE) ;
//							bReadResult = (dwResult==WAIT_IO_COMPLETION) ;
//							
//						}
//
//						if(bReadResult)
//						{// 读取文件数据成功
//
//							if(xcOverlap[nPreOverlapIndex].dwOperSize==0 && !(bStop=!this->IsValideRunningState()))
//							{// 如果返回的长度为0，则说明读取失败
//								//Debug_Printf(_T("如果返回的长度为0，则说明读取失败 %d"),nErrorCode) ;
//
//								if(this->m_pEvent!=NULL)
//								{
//									SXCExceptionInfo ei ;
//
//									ei.strSrcFile = (*it).pFpi->strSourceFile ;
//									ei.ErrorCode.nSystemError = 0 ;
//									ei.ErrorCode.AppError = CopyFileErrorCode_CanotReadFileData ;
//									ei.SupportType = ErrorHandlingFlag_Exit ;
//
//									ErrorHandlingResult result = m_pEvent->XCOperation_CopyExcetption(ei) ;
//
//									bRet = false ;
//
//									switch(result)
//									{
//									case ErrorHandlingFlag_Ignore: // 忽略
//										break ;
//
//									default:
//									case ErrorHandlingFlag_Exit: // 退出
//										bStop = true ;
//										*this->m_pRunningState = CFS_ReadyStop ;
//										break ;
//									}
//								}
//								
//								goto STOP_END ;
//								//return false ;
//							}
//
//							//Debug_Printf(_T("CXCLocalFileAsynSourceFilter::FlushFileData() 5")) ;
//
//							/**
//							//出于性能考虑暂不回调读数据
//							if(m_pEvent!=NULL)
//							{// 往上回调数据
//								fdoi.nDataSize = xcOverlap[nPreOverlapIndex].dwOperSize ;
//								fdoi.uFileID = (*it).pFpi->uFileID ;
//								fdoi.strFileName = (*it).pFpi->strSourceFile ;
//
//								m_pEvent->XCOperation_FileDataOccured(fdoi) ;
//							}
//							/**/
//
//							//Debug_Printf(_T("CXCLocalFileAsynSourceFilter::FlushFileData() 6")) ;
//
//							uRemainSize -= xcOverlap[nPreOverlapIndex].dwOperSize  ;
//							fd.uReadBeginPos = (*it).pFpi->nFileSize - uRemainSize  ;
//
//							
//							//fd.nDataSize = CurAlloc.dwBufSize ;//xcOverlap[nPreOverlapIndex].dwOperSize  ;
//							fd.nDataSize = xcOverlap[nPreOverlapIndex].dwOperSize  ;
//							fd.pData = CurAlloc.pBuf ;
//							fd.nBufSize = CurAlloc.dwBufSize ;
//							fd.uFileID = (*it).pFpi->uFileID ;
//
//							// 注意: CurAlloc.dwBufSize 有可能比 xcOverlap[nPreOverlapIndex].dwOperSize 大,
//							// 但若把 xcOverlap[nPreOverlapIndex].dwOperSize 赋作nDataSize,则在释放时就会
//							// 按 xcOverlap[nPreOverlapIndex].dwOperSize 大小释放,但申请则是按CurAlloc.dwBufSize 大小申请的,
//							// 因此会出现空间泄露的情况. 所以这里必须进行必要重新的对齐
//							if(nReadAlignSize>uRemainSize)
//							{// 若对齐粒度过大,则对其进行修正
//								//nReadAlignSize = this->CalculateAlignSize(uRemainSize,(*it).nSectorSize) ;
//								nReadAlignSize = ALIGN_SIZE_UP((DWORD)uRemainSize,(DWORD)m_pFileDataBuf->GetPageSize()) ;
//							}
//
//							//Win_Debug_Printf(_T("CXCLocalFileAsynSourceFilter::FlushFileData() EDC_FileData ")) ;
//
//							m_pDownstreamFilter->OnDataTrans(this,EDC_FileData,&fd) ;
//
//							//Debug_Printf(_T("CXCLocalFileAsynSourceFilter::FlushFileData() 7")) ;
//						}
//						else
//						{
//							SXCExceptionInfo ei ;
//
//							ei.strSrcFile = (*it).pFpi->strSourceFile ;
//							ei.uFileID = (*it).pFpi->uFileID ;
//							ei.ErrorCode.nSystemError = nErrorCode ;
//							ei.SupportType = ErrorHandlingFlag_RetryCancel ;
//
//							// 异常回调
//							ErrorHandlingResult result = this->m_pEvent->XCOperation_CopyExcetption(ei) ;
//
//							switch(result)
//							{
//							case ErrorHandlingFlag_Ignore: // 忽略
//								{// 内容抛弃
//									bRet = false ;
//
//									m_pEvent->XCOperation_FileDiscard((*it).pFpi,uRemainSize) ;
//
//									// 让 dstination filter 抛弃该文件
//									fd.pData = NULL ;
//									fd.nDataSize = 0 ;
//									m_pDownstreamFilter->OnDataTrans(this,EDC_FileData,&fd) ;
//								}
//								break ;
//
//							case ErrorHandlingFlag_Retry: // 重试（读文件）
//								goto EXCEPTION_RETRY_READFILE ;
//
//							default:
//							case ErrorHandlingFlag_Exit: // 退出
//								bRet = false ;
//								bStop = true ;
//								*this->m_pRunningState = CFS_ReadyStop ;
//								break ;
//							}
//						}
//					}// while(uRemainSize>0)
//
//					m_hCurFileHandle = INVALID_HANDLE_VALUE ;
//				}
//
//				// 这里的文件句柄在其后的 CXCLocalFileSourceFilter::FreeFileInfo() 处释放
//
//				if(bRet)
//				{// 不是退出
//					if(uRemainSize==0)
//					{// 这不是被"忽略"的文件
//						if((*it).pFpi!=NULL)
//						{// 加入到"确认缓冲队列"
//							fnpi.nSectorSize = (*it).nSectorSize ;
//							fnpi.pFpi = (*it).pFpi ;
//
//							this->AddToDelayReleaseFileList(fnpi) ;
//						}
//
//						if(m_pEvent!=NULL)
//						{// 把完成读取的文件往上回调
//							fei.strFileName = (*it).pFpi->strSourceFile ;
//							fei.uFileID = (*it).pFpi->uFileID ;
//
//							pt_STL_vector(SFileEndedInfo) FeiVer ;
//
//							FeiVer.push_back(fei) ;
//
//							m_pEvent->XCOperation_FileEnd(FeiVer) ;
//						}
//
//						// 该文件复制完成,则加入到完成队列处,并发送 EDC_FileOperationCompleted 命令
//						FileCompletedPack.CompletedFileInfoList.push_back((*it).pFpi) ;
////						m_OutputPin.PushData(EDC_FileOperationCompleted,&FileCompletedPack) ;
//						m_pDownstreamFilter->OnDataTrans(this,EDC_FileOperationCompleted,&FileCompletedPack) ;
//						FileCompletedPack.CompletedFileInfoList.clear() ;
//					}
//					else 
//					{// 这是中途被“抛弃”的
//						m_pEvent->XCOperation_FileDiscard((*it).pFpi,uRemainSize) ;
//						// 这里不用关闭文件句柄，这个会在 CXCLocalFileSourceFilter::FlushFileData() 处释放
//
//						SDataPack_SourceFileInfo::Free((*it).pFpi) ; // 在这里释放 source file info 信息体
//						(*it).pFpi = NULL ;
//					}
//				}
//			}
//		}
//
//	}// for 
//
//STOP_END:
//	if(bStop)
//	{
//		//this->FreeFileInfo() ;
//		this->InsideStop() ;
//		bRet = false ;
//	}
//
//	return bRet ;
//}
//
//
//void CXCLocalFileAsynSourceFilter::FileIOCompletionRoutine(DWORD dwErrorCode,DWORD dwNumberOfBytesTransfered,OVERLAPPED* lpOverlapped) 
//{
//	//Debug_Printf(_T("Read FileIOCompletionRoutine() begin")) ;
//
//	if(lpOverlapped!=NULL)
//	{
//		SXCAsynOverlapped* pXCOverlap = (SXCAsynOverlapped*)lpOverlapped ;
//
//		//Debug_Printf(_T("Read FileIOCompletionRoutine() %u"),dwNumberOfBytesTransfered) ;
//		pXCOverlap->dwOperSize = dwNumberOfBytesTransfered ;
//		//pXCOverlap->pThis->FileIOCompleteWork(dwNumberOfBytesTransfered) ;
//	}
//
//	//Debug_Printf(_T("Read FileIOCompletionRoutine() end")) ;
//}
//
////void CXCLocalFileAsynSourceFilter::FileIOCompleteWork(const DWORD& dwNumberOfBytesTransfered)
////{
////	ECopyFileState cfs = this->GetState() ;
////	if(this->GetState()!=CFS_Stop)
////	{
////		if(dwNumberOfBytesTransfered==0)
////		{// HANDLE CLOSED
////		}
////		else
////		{
////		}
////	}
////}
//
////bool CXCLocalFileAsynSourceFilter::FlushFile() 
////{
////	if(m_OpenedFileInfoList.empty())
////	{
////		return true ;
////	}
////
////	if(!m_OutputPin.IsConnected())
////	{
////		return false ;
////	}
////
////	if(!this->FlushCreateFile())
////	{
////		return false ;
////	}
////
////	if(this->FlushFileData())
////	{
////		return CXCLocalFileSourceFilter::FlushFile() ; // 调用上一级的，以便释放 m_OpenedFileInfoList 资源
////	}
////	
////	return false ;
////}
////
////bool CXCLocalFileAsynSourceFilter::AddFile(const CptString strFileName,const WIN32_FIND_DATA& wfd,int nSectorSize,unsigned uSpecifyFileID) 
////{
////	bool bRet = false ;
////	bool bAdd = true ;
////	
////EXCEPTION_RETRY_OPENFILE: // 重试（打开文件）
////	HANDLE hFile = ::CreateFile(strFileName.c_str(),GENERIC_READ,0,NULL,OPEN_EXISTING,
////		FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_OVERLAPPED,NULL) ;
////
////	if(hFile==INVALID_HANDLE_VALUE)
////	{// 打开文件失败
////		if(this->m_pEvent!=NULL)
////		{
////			SXCExceptionInfo ei ;
////			ei.ErrorCode.nSystemError = ::GetLastError() ;
////			ei.strSrcFile = strFileName ;
////			ei.SupportType = ErrorHandlingFlag_RetryIgnoreCancel ;
////
////			ErrorHandlingResult result = this->m_pEvent->XCOperation_CopyExcetption(ei) ;
////
////			switch(result)
////			{
////			case ErrorHandlingFlag_Ignore: // 忽略
////				bAdd = false ; // 不添加
////				m_pEvent->XCOperation_FileDiscard(NULL,0) ;
////				break ;
////
////			case ErrorHandlingFlag_Retry: // 重试（打开文件）
////				goto EXCEPTION_RETRY_OPENFILE ;
////				break ;
////
////			case ErrorHandlingFlag_Exit: // 退出
////			default:
////				return false ;
////			}
////		}
////		else
////		{
////			bAdd = false ;
////		}
////	}
////
////	if(bAdd)
////	{
////		return this->AddOpenedFileToList(hFile,strFileName,&wfd,nSectorSize,uSpecifyFileID) ;
////	}
////
////	return true ;
////}
////
////int CXCLocalFileAsynSourceFilter::OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData) 
////{
////	return CXCLocalFileSourceFilter::OnDataTrans(pSender,cmd,pFileData) ;
////}
//
////int CXCLocalFileAsynSourceFilter::OnPin_Data(CXCPin* pOwnerPin,EFilterCmd cmd,void* pFileData) 
////{
////	return CXCLocalFileSourceFilter::OnPin_Data(pOwnerPin,cmd,pFileData) ;
////}