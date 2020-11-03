/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCLocalFileSyncSourceFilter.h"
#include "XCCopyingEvent.h"



CXCLocalFileSyncSourceFilter::CXCLocalFileSyncSourceFilter(CXCCopyingEvent* pEvent):CXCLocalFileSourceFilter(pEvent)
{
}


CXCLocalFileSyncSourceFilter::~CXCLocalFileSyncSourceFilter(void)
{
}

bool CXCLocalFileSyncSourceFilter::OnRun()
{
	return CXCLocalFileSourceFilter::OnRun() ;
}

bool CXCLocalFileSyncSourceFilter::OnInitialize() 
{
	m_nCreateFileFlag = FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN ;

	return CXCLocalFileSourceFilter::OnInitialize() ;
}


bool CXCLocalFileSyncSourceFilter::OnPause() 
{
	return CXCLocalFileSourceFilter::OnPause() ;
}

void CXCLocalFileSyncSourceFilter::OnStop() 
{
	Debug_Printf(_T("CXCLocalFileSyncSourceFilter::OnStop()")) ;

	CXCLocalFileSourceFilter::OnStop() ;
}


bool CXCLocalFileSyncSourceFilter::FlushFileData() 
{
	Debug_Printf(_T("CXCLocalFileSyncSourceFilter::FlushFileData() begin")) ;

	if(!this->IsValideRunningState())
	{
		this->InsideStop() ;
		return false ;
	}

#ifdef COMPILE_TEST_PERFORMANCE
	DWORD dwkk = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif

	bool bRet = true ;

	DWORD dwRead = 0 ;
	unsigned __int64 uRemainSize = 0 ;
	char*	pBuf = NULL ;

	SDataPack_FileData fd ;
	SFileIDNamePairsInfo fnpi ;

	pt_STL_list(SFileIDNamePairsInfo) fnpiList ;
	pt_STL_list(SSourceFileInfo)::iterator& it = m_OpenedFileInfoList.begin() ;

	BOOL bReadResult = FALSE ;
	const int& nFileDataBufPageSize = this->m_pFileDataBuf->GetPageSize() ;
	DWORD nReadAlignSize = 0 ;

	bool bStop = false ;

	Debug_Printf(_T("CXCLocalFileSyncSourceFilter::FlushFileData() 1")) ;

	while(bRet && it!=m_OpenedFileInfoList.end() && !(bStop=!this->IsValideRunningState()))
	{
		//if((*it).pFpi!=NULL && !(*it).pFpi->bDiscard)
		{// 如果该文件在创建时期不被抛弃
			//continue ;
			if((*it).hFile==INVALID_HANDLE_VALUE)
			{// 目录

				// 加入到"确认缓冲队列"
				// 注意： (*it).pFpi==NULL 时，则这是 URI 弹栈
				if((*it).pFpi==NULL)
				{// 当要弹栈了，则说明最近一个文件夹的所有操作已完成，则把其加入完成队列

					_ASSERT(!m_FolderStack.empty()) ;

					fnpiList.push_back(m_FolderStack.back()) ;
					m_FolderStack.pop_back() ;
				}
				else
				{
					fnpi.nSectorSize = (*it).nSectorSize ;
					fnpi.pFpi = (*it).pFpi ;

					m_FolderStack.push_back(fnpi) ;
				}
			}
			else if((*it).hFile==NULL || (*it).pFpi->IsError())
			{// 如果该文件是在创建时已出错，则留给 destination filter 处理
			}
			else //if((*it).pFpi!=NULL && !(*it).pFpi->bDiscard)
			{// 文件
				_ASSERT((*it).pFpi!=NULL) ;

				uRemainSize = (*it).pFpi->nFileSize ;
				m_pCurSourceFileInfo = (*it).pFpi ;

				bool bFlush = false ;

				// file data buffer 的页面 是否为 sector 的倍数
				nReadAlignSize = this->CalculateAlignSize(uRemainSize,(*it).nSectorSize) ;

				SFileDataOccuredInfo fdoi ;
				fdoi.bReadOrWrite = true ;

				while(bRet && !(*it).pFpi->IsDiscard() && uRemainSize>0 && !(bStop=!this->IsValideRunningState()))
				{
					pBuf = NULL ;
					bFlush = false ;

					while(pBuf==NULL && !(bStop=(!this->IsValideRunningState())))
					{// 申请文件数据缓冲循环

						pBuf = (char*)m_pFileDataBuf->Allocate(m_nValidCachePointNum,nReadAlignSize) ;

						if(pBuf==NULL)
						{// 这说明缓冲区已填满了

							if(!fnpiList.empty())
							{// 先把为了性能而缓冲起来的用作扫尾的命令和数据向下发或保存起来
								//Debug_Printf(_T("CXCLocalFileSyncSourceFilter::FlushFileData() 2a4")) ;

								this->RoundOffReadFile(fnpiList) ;
								fnpiList.clear() ;
							}

#ifdef COMPILE_TEST_PERFORMANCE
							DWORD dw = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif
							m_pDownstreamFilter->OnDataTrans(this,EDC_FlushData,NULL) ;

#ifdef COMPILE_TEST_PERFORMANCE
							CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,3) ;
#endif

							bFlush = true ;
							if(!this->CheckPauseAndWait())
							{// 暂停
								bStop = true ;
								goto STOP_END ;
							}
						}
						else
						{
							break ;
						}
					}

					if(bStop)
					{// 申请缓冲区失败，可能用户选择退出
						bStop = true ;
						bRet = false ;
						break ;
					}

EXCEPTION_RETRY_READFILE: // 重试（读文件）

					dwRead = 0 ;

#ifdef COMPILE_TEST_PERFORMANCE
					DWORD dw = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif

					bReadResult = ::ReadFile((*it).hFile,pBuf,nReadAlignSize,&dwRead,NULL) ;

#ifdef _DEBUG
					// 用于测试读取数据慢的 case
					//::Sleep(10000) ;
#endif

#ifdef COMPILE_TEST_PERFORMANCE
					CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,1) ;
#endif
					if(!this->CheckPauseAndWait())
					{// 暂停
						bStop = true ;
						goto STOP_END ;
					}

					if(bReadResult && dwRead>0 )
					{// 读取文件数据成功
						uRemainSize -= dwRead ;

						fd.nDataSize = dwRead ;
						fd.nBufSize = nReadAlignSize ;
						fd.pData = pBuf ;
						fd.uFileID = (*it).pFpi->uFileID ;

						// 注意: fd.nBufSize 有可能比 dwRead 大,
						// 但若把 dwRead 赋作fd.nDataSize,则在释放时就会
						// 按 dwRead 大小释放,但申请则是按fd.nBufSize 大小申请的,
						// 因此会出现空间泄漏的情况. 所以这里必须进行必要重新的对齐
						if(nReadAlignSize>uRemainSize)
						{// 若对齐粒度过大,则对其进行修正
							nReadAlignSize = ALIGN_SIZE_UP((DWORD)uRemainSize,(DWORD)m_pFileDataBuf->GetPageSize()) ;
							nReadAlignSize = ALIGN_SIZE_UP(nReadAlignSize, (*it).nSectorSize);
						}

						//Debug_Printf(_T("CXCLocalFileSyncSourceFilter::FlushFileData() 2f")) ;

#ifdef COMPILE_TEST_PERFORMANCE
						DWORD dw = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif
						m_pDownstreamFilter->OnDataTrans(this,EDC_FileData,&fd) ;

#ifdef COMPILE_TEST_PERFORMANCE
						CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,13) ;
#endif
					}
					else
					{
						const int nErrorCode = ::GetLastError() ;

						SXCExceptionInfo ei ;

						ei.uFileID = (*it).pFpi->uFileID ;
						ei.strSrcFile = (*it).pFpi->strSourceFile ;
						ei.ErrorCode.nSystemError = nErrorCode ;
						ei.SupportType = ErrorHandlingFlag_RetryIgnoreCancel ;

						// 异常回调
						ErrorHandlingResult result = m_pEvent->XCOperation_CopyExcetption(ei) ;

						switch(result)
						{
						case ErrorHandlingFlag_Ignore: // 忽略
							{// 内容抛弃
								bRet = false ;

								// 让 dstination filter 抛弃该文件
								(*it).pFpi->SetDiscard(true) ;
								(*it).pFpi->SetError(true) ; // 设置为出错的文件，以便在 destination filter 把其回调上app层去

								m_pDownstreamFilter->OnDataTrans(this,EDC_FileData,&fd) ; // 数据依然往下发，让 destination filter 释放 fd.pData 空间
							}
							break ;

						case ErrorHandlingFlag_Retry: // 重试（读文件）
							goto EXCEPTION_RETRY_READFILE ;

						default:
						case ErrorHandlingFlag_Exit: // 退出
							bRet = false ;
							*this->m_pRunningState = CFS_ReadyStop ;
							break ;
						}
					}

				}// while(uRemainSize>0)

				//Debug_Printf(_T("CXCLocalFileSyncSourceFilter::FlushFileData() 3")) ;

				if(!this->CheckPauseAndWait())
				{// 因为当该文件为最后的文件时，倘若用户点击抛弃，则有可能 source filter 
					// 还没有来得及进入 pause 状态就从这个循环退出，这就会导致界面层 Skip 后再调用 Contiune()就会出现崩溃
					//这里也应该检测是否在暂停状态
					bStop = true ;
					goto STOP_END ;
				}

				//Debug_Printf(_T("CXCLocalFileSyncSourceFilter::FlushFileData() 4")) ;

				_ASSERT((*it).pFpi!=NULL) ; 

				// 无论文件是否被抛弃都应该加入到"确认缓冲队列"，
				// 以便 destination filter 返回 EDC_FileDoneConfirm 命令时释放 SDataPack_SourceFileInfo 空间
				fnpi.nSectorSize = (*it).nSectorSize ;
				fnpi.pFpi = (*it).pFpi ;

				fnpiList.push_back(fnpi) ;

				if((*it).pFpi->IsDiscard() && bRet && m_pEvent!=NULL)
				{// 不是退出，但中途被“抛弃”的
					//Release_Printf(_T("CXCLocalFileSyncSourceFilter::FlushFileData() discard 2")) ;
					m_pEvent->XCOperation_FileDiscard((*it).pFpi,uRemainSize) ;
				}

				
				if(XCTT_Move==m_TaskType && (*it).hFile!=INVALID_HANDLE_VALUE)
				{
					::CloseHandle((*it).hFile) ;
					(*it).hFile = INVALID_HANDLE_VALUE ;
				}
			}
		}

		++it ;
	}// while

	m_pCurSourceFileInfo = NULL ;

	if(!fnpiList.empty())
	{// 先把为了性能而缓冲起来的用作扫尾的命令和数据向下发或保存起来
		this->RoundOffReadFile(fnpiList) ;
		fnpiList.clear() ;
	}

STOP_END:
	if(bStop)
	{
		this->InsideStop() ;
		bRet = false ;
	}

	Debug_Printf(_T("CXCLocalFileSyncSourceFilter::FlushFileData() end")) ;

	return bRet ;
}

void CXCLocalFileSyncSourceFilter::RoundOffReadFile(const pt_STL_list(SFileIDNamePairsInfo)& fnpiList) 
{
	Debug_Printf(_T("CXCLocalFileSyncSourceFilter::RoundOffReadFile() begin")) ;

#ifdef COMPILE_TEST_PERFORMANCE
	DWORD dw = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif

	// 加入到"确认缓冲队列"
	SDataPack_FileOperationCompleted FileCompletedPack ;

	{
		CptAutoLock lock(&m_FilePairListLock) ;

		pt_STL_list(SFileIDNamePairsInfo)::const_iterator it = fnpiList.begin() ;

		while(it!=fnpiList.end())
		{
			_ASSERT((*it).pFpi!=NULL) ;
			
			m_ConfirmReleaseFilePairInfoList.push_back(*it) ;
			
			FileCompletedPack.CompletedFileInfoList.push_back((*it).pFpi) ;
			++it ;
		}
	}

	// 该文件复制完成,则加入到完成队列处,并发送 EDC_FileOperationCompleted 命令
	
	m_pDownstreamFilter->OnDataTrans(this,EDC_FileOperationCompleted,&FileCompletedPack) ;

#ifdef COMPILE_TEST_PERFORMANCE
	CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,99) ;
#endif

	Debug_Printf(_T("CXCLocalFileSyncSourceFilter::RoundOffReadFile() end")) ;
}
