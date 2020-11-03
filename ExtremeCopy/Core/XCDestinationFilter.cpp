/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCDestinationFilter.h"
#include "XCCopyingEvent.h"
#include "..\Common\ptWinPath.h"
#include "XCFileDataBuffer.h"
#include "..\Common\ptGlobal.h"
#include <deque>
#include "../Common/ptDebugView.h"


int CXCDestinationFilter::m_sDstFilterIDCounter = 0 ;

CXCDestinationFilter::CXCDestinationFilter(CXCCopyingEvent* pEvent,bool bIsRenameCopy):CXCFilter(pEvent),m_bIsRenameCopy(bIsRenameCopy)
{
	m_nDstFilterID = ++m_sDstFilterIDCounter ;
}


CXCDestinationFilter::~CXCDestinationFilter(void)
{
}


void CXCDestinationFilter::ResetCanCallbackMark() 
{
	m_sDstFilterIDCounter = 0 ;
}

// 因为可以有多路的 destination filter 同时写入，而只有第一个才具有回调数据的资格
bool CXCDestinationFilter::CanCallbackFileInfo() const 
{
	return (m_nDstFilterID==1) ;
}


bool CXCDestinationFilter::OnInitialize() 
{
	return CXCFilter::OnInitialize() ;
}

bool CXCDestinationFilter::OnContinue() 
{
	return CXCFilter::OnContinue() ;
}

bool CXCDestinationFilter::OnPause()
{
	return CXCFilter::OnPause() ;
}

void CXCDestinationFilter::OnStop()
{
	CXCFilter::OnStop() ;
}

//===============================


CXCLocalFileDestnationFilter::CXCLocalFileDestnationFilter(CXCCopyingEvent* pEvent,const CptString strDestRoot,const SStorageInfoOfFile& siof,
	const bool bIsRenameCopy)
	:CXCDestinationFilter(pEvent,bIsRenameCopy),m_pUpstreamFilter(NULL)
	,m_hCurFileHandle(INVALID_HANDLE_VALUE),
	m_strDestRoot(strDestRoot),m_pImpactFileBehavior(NULL),m_StorageInfo(siof)
{
	m_bIsDriverRoot = (strDestRoot.GetAt(strDestRoot.GetLength()-2) == ':') ;

	m_CurFileIterator = m_FileInfoList.end() ;
}

CXCLocalFileDestnationFilter::~CXCLocalFileDestnationFilter(void)
{
	pt_STL_list(SDstFileInfo)::iterator DstFileIt = m_FileInfoList.begin() ;

	for(;DstFileIt!=m_FileInfoList.end();++DstFileIt)
	{
		if((*DstFileIt).hFile!=INVALID_HANDLE_VALUE)
		{
			::CloseHandle((*DstFileIt).hFile) ;
		}

		CptGlobal::ForceDeleteFile((*DstFileIt).strFileName) ;
	}

	m_FileInfoList.clear() ;
}


ErrorHandlingResult CXCLocalFileDestnationFilter::GetErrorHandleResult(SDataPack_SourceFileInfo& sfi,CptString strDestFile)
{
	SXCExceptionInfo ei ;
	ei.ErrorCode.nSystemError = ::GetLastError() ;
	ei.SupportType = ErrorHandlingFlag_RetryIgnoreCancel ;
	ei.strDstFile = strDestFile ;
	ei.uFileID = sfi.uFileID ;
	ei.strSrcFile = sfi.strSourceFile ;

	ErrorHandlingResult result = m_pEvent->XCOperation_CopyExcetption(ei) ;

	if(result==ErrorHandlingFlag_Ignore)
	{
		m_pEvent->XCOperation_RecordError(ei) ;	
	}

	return result ;
}

bool CXCLocalFileDestnationFilter::OnInitialize() 
{
	return CXCDestinationFilter::OnInitialize() ;
}

bool CXCLocalFileDestnationFilter::OnContinue() 
{
	return CXCDestinationFilter::OnContinue() ;
}

bool CXCLocalFileDestnationFilter::OnPause()
{
	return CXCDestinationFilter::OnPause() ;
}

void CXCLocalFileDestnationFilter::OnStop()
{
	if(m_hCurFileHandle!=INVALID_HANDLE_VALUE)
	{
		::CancelIo(m_hCurFileHandle) ;
	}

	// 把还没有完全写入磁盘的文件删除掉
	bool bDelete = false ;
	bool bOverCurIt = false ;

	pt_STL_list(SDstFileInfo)::iterator it = m_FileInfoList.begin() ;

	for(;it!=m_FileInfoList.end();)
	{
		bDelete = true ;

		// 暂未知为什么要跳过对当前文件的删除
		// 故先把其以及下面的 bDelete = bOverCurIt ;一起注释
		//if(!bOverCurIt && it==m_CurFileIterator)
		//{
		//	bOverCurIt = true ;
		//}

		if((*it).hFile!=INVALID_HANDLE_VALUE)
		{
			if((*it).uRemainSize==0 && (*it).bNoBuf )
			{
				_ASSERT(!bOverCurIt) ;
				::SetFilePointer((*it).hFile, 0, NULL, FILE_BEGIN);
				::SetEndOfFile((*it).hFile) ;
				bDelete = false ;// 已完成的文件不删除
			}

			::CloseHandle((*it).hFile) ;
			(*it).hFile = INVALID_HANDLE_VALUE ;

			//bDelete = bOverCurIt ;
		}
		else
		{
			// 如果文件是在创建前就因为同名等原因而被抛弃，则不应作删除
			if((*it).pSfi->IsDiscard() && (*it).uRemainSize==0)
			{
				bDelete = false ;
			}
		}

		if(bDelete)
		{
			CptGlobal::ForceDeleteFile((*it).strFileName.c_str()) ;
		}

		++it ;
	}

	m_FileInfoList.clear() ;

	Debug_Printf(_T("CXCLocalFileDestnationFilter::OnStop() end")) ;

	CXCDestinationFilter::OnStop() ;

	Debug_Printf(_T("CXCLocalFileDestnationFilter::OnStop() end")) ;
}

int CXCLocalFileDestnationFilter::OnCreateXCFile(SDataPack_CreateFileInfo& cfi) 
{
	int nRet = 0 ;

	SActiveFilesInfo bfi ;
	CptString strDstFile ;

	pt_STL_list(SActiveFilesInfo) ActiveFileInfoQue ;
	bool bFolder = false ;
	bool bDiscardInSrcOpen = false ;

	if(!cfi.SourceFileInfoList.empty())
	{
		pt_STL_list(SDataPack_SourceFileInfo*)::iterator it = cfi.SourceFileInfoList.begin() ;

		//for(size_t i=0;i<cfi.SourceFileInfoVer.size() && this->IsValideRunningState();++i)
		for(;it!=cfi.SourceFileInfoList.end() && this->IsValideRunningState();++it)
		{
			if((*it)==NULL)
			{
				this->m_DestURI.Pop() ; // 弹出 URI 目录栈
			}
			else 
			{// 文件
				bDiscardInSrcOpen = (*it)->IsDiscard() ; //因为在 CreateXCFile() 返回也会置 bDiscard 为true，
														// 所以在这里就判断是否在source filter处已是抛弃

				nRet = this->CreateXCFile(*(*it),strDstFile) ;

				bFolder = CptGlobal::IsFolder((*it)->dwSourceAttr) ;

				/**
				// 暂不考虑文件夹抛弃
				
				if(bFolder && cfi.SourceFileInfoVer[i]->bDiscard)
				{// 若为被抛弃的文件夹,则在该文件夹内的所有文件及子文件夹均被抛弃。
					// 因为该文件夹被抛弃，所以在CreateXCFile()里没有被入目录栈，所以这里也不用出目录栈

					// 将其记录在被抛弃的文件夹缓冲区，以便 EDC_BatchCreateFile 命令返回时，
					// source filter 知道有哪些文件夹被抛弃
//					cfi.DiscardFolderVer.push_back(cfi.SourceFileInfoVer[i]->strSourceFile) ;

					int nDirStack = 1 ;

					for(size_t j=i+1;j<cfi.SourceFileInfoVer.size();++j)
					{
						if(cfi.SourceFileInfoVer[j]!=NULL)
						{
							if(cfi.SourceFileInfoVer[j]->strSourceFile.GetLength()>
								cfi.SourceFileInfoVer[i]->strSourceFile.GetLength())
							{// 因为若要包括被抛弃的目录，那么文件名长度必然大于该目录名长度（包括路径）
								if(cfi.SourceFileInfoVer[j]->strSourceFile.Left(cfi.SourceFileInfoVer[i]->strSourceFile.GetLength()).CompareNoCase(
									cfi.SourceFileInfoVer[i]->strSourceFile)!=0)
								{
									_ASSERT(FALSE) ;
								}
								else
								{
									if(CptGlobal::IsFolder(cfi.SourceFileInfoVer[j]->dwSourceAttr))
									{// 若要抛弃的目录包括子目录，则目录栈加1
										++nDirStack ;
									}

									cfi.SourceFileInfoVer[j]->bDiscard = true ;
								}
							}
							else
							{
								_ASSERT(FALSE) ;
							}
						}
						else
						{// 此为出目录栈,直到遇到和要抛弃的文件夹匹配的出目录栈才退出该循环
							if(--nDirStack<=0)
							{
								i = j -1;
								break ;
							}
						}
					}
				}
				else 
					/**/
				if(!bFolder && strDstFile.GetLength()>0 && !bDiscardInSrcOpen)
				{// 若为文件,且不是在source filter处已被抛弃
					bfi.strDstFile = strDstFile ;
					bfi.strSrcFile = (*it)->strSourceFile ;
					bfi.uFileID = (*it)->uFileID ;
					bfi.uFileSize = (*it)->nFileSize ;

					ActiveFileInfoQue.push_back(bfi) ;
				}
			}
		}
	}

	if(nRet==0 && !ActiveFileInfoQue.empty())
	{
		if(this->CanCallbackFileInfo())
		{
			this->m_pEvent->XCOperation_FileBegin(ActiveFileInfoQue) ;
		}
	}

	return nRet ;
}

int CXCLocalFileDestnationFilter::CreateXCFile(SDataPack_SourceFileInfo& sfi,CptString& strOutDstFile) 
{
	int nRet = 0 ;

	strOutDstFile = _T("") ;
	//Debug_Printf(_T("CXCLocalFileDestnationFilter::CreateXCFile() 1 id=%u name=%s"),sfi.uFileID,sfi.strSourceFile) ;

	if(!this->IsValideRunningState())
	{
		return ErrorHandlingFlag_Exit ;
	}

	SDstFileInfo dfi ;

#ifdef COMPILE_TEST_PERFORMANCE
	DWORD dw53 = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif
	CptString strDestFile ;

	if(this->IsRenameExe())
	{// 如果是‘改名’式复制或者移动
		strDestFile = m_strDestRoot ;//MAKE_FILE_FULL_NAME(m_strDstFileName) ;
	}
	else
	{
		CptString strFileName ;

		if(sfi.strNewFileName.GetLength()>0)
		{
			strFileName = sfi.strNewFileName ;
		}
		else
		{
			strFileName = ::GetRawFileName(sfi.strSourceFile) ;
		}
		
		strDestFile = this->MakeFileFullName(strFileName) ;//MAKE_FILE_FULL_NAME(::GetRawFileName(sfi.strSourceFile)) ;
	}

#ifdef COMPILE_TEST_PERFORMANCE
	CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw53,23) ;
#endif

	if(!sfi.IsDiscard())
	{// 因为创建文件命令可能为异步，
		// 即该文件在source filter打开是正常，只是在读取到一定时候就抛弃，
		// 然而过一段时间后destination filter才收到该文件的创建命令，所以这里得作检测
		
		if(*m_pImpactFileBehavior!=SFDB_Replace && !CptGlobal::IsFolder(sfi.dwSourceAttr))
		{// 如果遇到冲突文件总是覆盖，那么就不检测是否存在冲突文件
			bool bIsDestExist = IsFileExist(strDestFile.c_str()) ;

			if(bIsDestExist)
			{// 目标文件名已存在，则回调到上一层询问

				SImpactFileResult result ;
				
				if(this->m_pEvent!=NULL)
				{
					if(*m_pImpactFileBehavior==SFDB_Default || *m_pImpactFileBehavior==SFDB_Rename)
					{// 等于默认（改名）才向上回调
						SImpactFileInfo ImpactInfo ;
						ImpactInfo.ImpactType = IT_SameFileName ;

						ImpactInfo.strSrcFile = sfi.strSourceFile ;
						ImpactInfo.strDestFile = strDestFile ;

						m_pEvent->XCOperation_ImpactFile(ImpactInfo,result) ;
					}
					else
					{
						result.result = *m_pImpactFileBehavior ;
					}
				}

				if(result.bAlways)
				{
					*m_pImpactFileBehavior = result.result ;
				}

				switch(result.result)
				{
				case SFDB_Skip: // 跳过
					{// 就算是跳过，除了把discard设置为true外，也应该加入到 m_FileInfoList 缓冲区里
						//sfi.bDiscard = true ;
						sfi.SetDiscard(true) ;

						dfi.hFile = INVALID_HANDLE_VALUE ;
						dfi.bNoBuf = false ;
						dfi.strFileName = strDestFile ;
						dfi.pSfi = &sfi ; // 把其指针保存起来

						m_FileInfoList.push_back(dfi) ;
					}
					
					return nRet ;

				case SFDB_Default:
				case SFDB_Rename: // 改名
					strDestFile = result.strNewDstFileName ;
					break ;

				case SFDB_Replace: // 覆盖
					break ;

				default:
				case SFDB_StopCopy: // 退出
					*m_pRunningState = CFS_ReadyStop ;
					return ErrorHandlingFlag_Exit ;
				}
			}
		}
		
		if(CptGlobal::IsFolder(sfi.dwSourceAttr))
		{// 创建文件夹

//EXCEPTION_RETRY_CREATEFOLDER:
			
			BOOL bDirectResult = FALSE ;

			if(sfi.strSourceFile.CompareNoCase(strDestFile)==0)
			{// 如果是做文件夹复件的话
				SImpactFileInfo ImpactInfo ;
				SImpactFileResult result ;
				ImpactInfo.ImpactType = IT_SameFileName ;

				ImpactInfo.strSrcFile = sfi.strSourceFile ;
				ImpactInfo.strDestFile = strDestFile ;

				m_pEvent->XCOperation_ImpactFile(ImpactInfo,result) ;

				strDestFile = result.strNewDstFileName ;
			}

			if(sfi.IsLocal())
			{
				bDirectResult = ::CreateDirectoryEx(sfi.strSourceFile.c_str(),strDestFile.c_str(),NULL) ;
			}
			else
			{
				bDirectResult = ::CreateDirectory(strDestFile.c_str(),NULL) ;
			}
			
			if(!bDirectResult && ::GetLastError()!=ERROR_ALREADY_EXISTS)
			{// 创建文件夹失败, 目前遇到创建文件夹失败，则直接退出

				SXCExceptionInfo ei ;
				ei.ErrorCode.nSystemError = ::GetLastError() ;
				ei.SupportType = ErrorHandlingFlag_Exit ;
				ei.strDstFile = strDestFile ;
				ei.uFileID = sfi.uFileID ;
				ei.strSrcFile = sfi.strSourceFile ;

				this->m_pEvent->XCOperation_CopyExcetption(ei) ;

				return ErrorHandlingFlag_Exit ;
			}

			if(!sfi.IsDiscard())
			{// 若该文件夹创建成功，且不被抛弃,那么就增加URI的栈
				m_DestURI.Push(GetRawFileName(strDestFile)) ;
			}

			dfi.strFileName = strDestFile ;
			//dfi.pSfi->uFileID = sfi.uFileID ;
			dfi.uRemainSize = 0 ;
			dfi.hFile = INVALID_HANDLE_VALUE ;
			dfi.pSfi = &sfi ; // 把其指针保存起来

		}
		else
		{// 创建文件

			//Debug_Printf(_T("CXCLocalFileDestnationFilter::CreateXCFile() 4 create file")) ;

#ifdef COMPILE_TEST_PERFORMANCE
		DWORD dw52 = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif

			dfi.bNoBuf = (m_StorageInfo.uDiskType!=DRIVE_REMOTE && (sfi.nFileSize >= 64*1024 || (sfi.nFileSize%m_StorageInfo.nSectorSize)==0)) ;

			dfi.strFileName = strDestFile ;
			dfi.pSfi = &sfi ; // 把其指针保存起来

			DWORD dwFlag = (dfi.bNoBuf ? FILE_FLAG_NO_BUFFERING : 0) | m_nCreateFileFlag;// FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED;

			strOutDstFile = strDestFile ;

#ifdef COMPILE_TEST_PERFORMANCE
		CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw52,22) ;
#endif

EXCEPTION_RETRY_CREATEDESTFILE:// 重试（创建目标文件）

			
#ifdef COMPILE_TEST_PERFORMANCE
		DWORD dw5 = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif
			dfi.hFile = ::CreateFile(strDestFile.c_str(),GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,dwFlag,NULL) ;

			if(dfi.hFile==INVALID_HANDLE_VALUE && IsFileExist(strDestFile.c_str()))
			{
				::SetFileAttributes(strDestFile.c_str(), FILE_ATTRIBUTE_NORMAL);
				dfi.hFile = ::CreateFile(strDestFile.c_str(),GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,dwFlag,NULL) ;
			}

			if(dfi.hFile!=INVALID_HANDLE_VALUE)
			{
				if(dfi.bNoBuf)
				{
					int nOver = sfi.nFileSize%m_StorageInfo.nSectorSize ;

					dfi.uRemainSize = (nOver==0 ? sfi.nFileSize : sfi.nFileSize+m_StorageInfo.nSectorSize - nOver);
				}
				else
				{
					dfi.uRemainSize = sfi.nFileSize ;
				}

				if(dfi.uRemainSize>m_nSwapChunkSize)
				{
					unsigned __int64 dwLow = dfi.bNoBuf ? ALIGN_SIZE_UP(dfi.uRemainSize,m_StorageInfo.nSectorSize) : dfi.uRemainSize ;
					DWORD dwHi = (DWORD)(dwLow>>32) ;

EXCEPTION_RETRY_ALLOCATEFILESIZE:// 重试（分配文件大小）
					if(::SetFilePointer(dfi.hFile, (DWORD)dwLow, (PLONG)&dwHi, FILE_BEGIN)!=INVALID_SET_FILE_POINTER
						&& ::SetEndOfFile(dfi.hFile))
					{
						::SetFilePointer(dfi.hFile, 0, NULL, FILE_BEGIN);
					}
					else
					{
						int nError = ::GetLastError() ;

						if(this->m_pEvent!=NULL)
						{
							switch(this->GetErrorHandleResult(sfi,strDestFile))
							{
							case ErrorHandlingFlag_Ignore: // 忽略
								strOutDstFile = _T("") ;
								sfi.SetDiscard(true) ;
								nRet = ErrorHandlingFlag_Ignore ;
								break ;

							case ErrorHandlingFlag_Retry: // 重试（分配文件大小）
								goto EXCEPTION_RETRY_ALLOCATEFILESIZE ;

							default:
							case ErrorHandlingFlag_Exit: // 退出
								strOutDstFile = _T("") ;
								sfi.SetDiscard(true) ;
								*this->m_pRunningState = CFS_ReadyStop ;

								// 因错误而退出的话，则应把刚刚创建的文件删除掉
								if(dfi.hFile!=INVALID_HANDLE_VALUE)
								{
									::CloseHandle(dfi.hFile) ;
									dfi.hFile = INVALID_HANDLE_VALUE ;
									::DeleteFile(strDestFile.c_str()) ;
								}
								return ErrorHandlingFlag_Exit ;
							}
						}
					}
				}
			}
			else
			{
				int nError = ::GetLastError() ;

				if(this->m_pEvent!=NULL)
				{
					switch(this->GetErrorHandleResult(sfi,strDestFile))
					{
					case ErrorHandlingFlag_Ignore: // 忽略
						//sfi.bDiscard = true ;
						sfi.SetDiscard(true) ;
						nRet = ErrorHandlingFlag_Ignore ;
						break ;

					case ErrorHandlingFlag_Retry: // 重试（创建目标文件）
						goto EXCEPTION_RETRY_CREATEDESTFILE ;

					default:
					case ErrorHandlingFlag_Exit: // 退出
						//sfi.bDiscard = true ;
						sfi.SetDiscard(true) ;
						*this->m_pRunningState = CFS_ReadyStop ;
						return ErrorHandlingFlag_Exit ;
					}
				}
			}

#ifdef COMPILE_TEST_PERFORMANCE
		CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw5,5) ;
#endif
		}

		// 无论是创建文件成功与否，这里都保存进缓存里
	}
	else
	{// 就算该文件是被抛弃的也应该放入到 m_FileInfoList 里去
		
		if(sfi.IsError())
		{// 如果该文件在创建时已出错,则把其记录下来

			SXCExceptionInfo ei ;

			ei.ErrorCode.nSystemError = sfi.nSysErrCode ;
			ei.strSrcFile = sfi.strSourceFile ;
			ei.strDstFile = strDestFile ;
			ei.uFileID = sfi.uFileID ;

			this->m_pEvent->XCOperation_RecordError(ei) ;
		}

		dfi.uRemainSize = 0 ;
		dfi.hFile = INVALID_HANDLE_VALUE ;
		dfi.pSfi = &sfi ; // 把其指针保存起来
	}

	if(CptGlobal::IsFolder(dfi.pSfi->dwSourceAttr))
	{
		m_FolderInfoList.push_back(dfi) ;
	}
	else
	{
		m_FileInfoList.push_back(dfi) ;
	}
	
	return nRet ;
}



void CXCLocalFileDestnationFilter::RoundOffFile(pt_STL_list(SDataPack_SourceFileInfo*)& FileList )
{
	pt_STL_list(SDataPack_SourceFileInfo*)::const_iterator it = FileList.begin() ;

	SDataPack_FileDoneConfirm fdc ;
	
	bool bIsRunning = true ;

	SFileEndedInfo fei ;

	bool bFound = false ;

	pt_STL_vector(SFileEndedInfo) FeiVer ;
	pt_STL_list(HANDLE) ReadyCloseHandleList ;

	pt_STL_list(SDstFileInfo)::iterator DstFileIt ;

	for(;it!=FileList.end() && bIsRunning;++it )
	{
		bIsRunning = this->IsValideRunningState() ;

		fdc.FileDoneConfirmList.push_back((*it)->uFileID) ; // 收集可以让source filter释放相应文件资源的文件ID

		bFound = false ;

		{// 查找对应的 file ID
			if(CptGlobal::IsFolder((*it)->dwSourceAttr))
			{// 文件夹
				_ASSERT(!m_FolderInfoList.empty()) ;

				//DstFileIt = m_FolderInfoList.rbegin() ;
				DstFileIt = m_FolderInfoList.begin() ;

				if((*DstFileIt).pSfi->uFileID!=(*it)->uFileID)
				{
					bFound = false ;

					for(;DstFileIt!=m_FolderInfoList.end();++DstFileIt)
					{
						if((*DstFileIt).pSfi->uFileID==(*it)->uFileID)
						{
							bFound = true ;
							break ;
						}
					}

					_ASSERT(bFound) ;
				}
				else
				{
					bFound = true ;
				}
			}
			else
			{// 文件

				_ASSERT(!m_FileInfoList.empty()) ;

				DstFileIt = m_FileInfoList.begin() ;

				if((*DstFileIt).pSfi->uFileID!=(*it)->uFileID)
				{
					bFound = false ;

					for(;DstFileIt!=m_CurFileIterator;++DstFileIt)
					{
						if((*DstFileIt).pSfi->uFileID==(*it)->uFileID)
						{
							bFound = true ;
							break ;
						}
					}

					_ASSERT(bFound) ;
				}
				else
				{
					bFound = true ;
				}
			}
		}
		

		if(!(*it)->IsDiscard())
		{
			//Win_Debug_Printf(_T("CXCLocalFileDestnationFilter::RoundOffFile() id=%u"),(*it)->uFileID) ;

			//if(it2!=m_FileInfoMap.end())
			if(bFound)
			{
				if(CptGlobal::IsFolder((*it)->dwSourceAttr))
				{// 文件夹
					//if(!(*it)->bLocal)
					if(!(*it)->IsLocal())
					{
						(*DstFileIt).hFile = ::CreateFile((*DstFileIt).strFileName,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL) ;

						_ASSERT((*DstFileIt).hFile!=NULL) ;

						if((*DstFileIt).hFile!=INVALID_HANDLE_VALUE)
						{   
							BOOL b = ::SetFileTime((*DstFileIt).hFile,&(*it)->SrcFileTime.CreateTime,&(*it)->SrcFileTime.LastAccessTime,&(*it)->SrcFileTime.LastWriteTime) ;

							BOOL b2 = ::CloseHandle((*DstFileIt).hFile) ;

							//it2->second.hFile = INVALID_HANDLE_VALUE ;
							//if(!(*it)->bLocal)
							if(!(*it)->IsLocal())
							{// 源文件夹非本地文件
								BOOL b3 = ::SetFileAttributes((*DstFileIt).strFileName.c_str(),(*it)->dwSourceAttr) ;

								//DWORD dwNew = ::GetFileAttributes(it2->second.strFileName.c_str()) & ~FILE_ATTRIBUTE_COMPRESSED ;

							}
							//::SHChangeNotify(SHCNE_ALLEVENTS,SHCNF_PATH|SHCNF_FLUSH,it2->second.strFileName.c_str(),NULL) ;

						}
					}

					//if(m_FolderInfoList.back()==DstFileIt)
					//{
					//	m_FolderInfoList.pop_back() ;
					//}
					//else
					{
						m_FolderInfoList.erase(DstFileIt) ;
					}
					
				}
				else
				{
					if(INVALID_HANDLE_VALUE!=(*DstFileIt).hFile && 0==(*DstFileIt).uRemainSize && bIsRunning)
					{// 该文件已完成写入到磁盘并且为有效的句柄。因为如果源文件大小为0的话，其 uRemainSize 也会为0的

						{// 把完成写的文件加入缓冲区，待下面处回调
//							fei.strFileName = (*DstFileIt).strFileName ;
							fei.uFileID = (*DstFileIt).pSfi->uFileID;

							FeiVer.push_back(fei) ;
						}

#ifdef COMPILE_TEST_PERFORMANCE
							DWORD dw = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif

							/**
						if(it2->second.bNoBuf && (*it)->nFileSize%m_StorageInfo.nSectorSize)
						{// 这里需要重新打开文件句柄
							::CloseHandle(it2->second.hFile) ;

							it2->second.hFile = ::CreateFile(it2->second.strFileName.c_str(),GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,
								FILE_FLAG_SEQUENTIAL_SCAN,NULL) ;
						}

						if(m_StorageInfo.nSectorSize>0 && it2->second.uRemainSize!=0)
						{// 若该文件需要调整长度

							_ASSERT(it2->second.hFile!=INVALID_HANDLE_VALUE) ;

							DWORD dwLow = (DWORD)(*it)->nFileSize;
							DWORD dwHi = (DWORD)((*it)->nFileSize>>32) ;

							BOOL b2 = (::SetFilePointer(it2->second.hFile, dwLow, (PLONG)&dwHi, FILE_BEGIN)==INVALID_SET_FILE_POINTER);
							BOOL b = ::SetEndOfFile(it2->second.hFile) ;
						}
						/**/

						/**/
						if((*DstFileIt).bNoBuf && (*it)->nFileSize>0 && m_StorageInfo.nSectorSize>0 
							&& (*it)->nFileSize%m_StorageInfo.nSectorSize )
						{// 若该文件需要调整长度
							//HANDLE hFile = it2->second.hFile ;
							//::FlushFileBuffers(it2->second.hFile) ;

							if((*DstFileIt).bNoBuf && (*it)->nFileSize%m_StorageInfo.nSectorSize)
							{
								::CloseHandle((*DstFileIt).hFile) ;

								(*DstFileIt).hFile = ::CreateFile((*DstFileIt).strFileName.c_str(),GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,
									FILE_FLAG_SEQUENTIAL_SCAN,NULL) ;
							}

							if((*DstFileIt).hFile!=INVALID_HANDLE_VALUE)
							{
								DWORD dwLow = (DWORD)(*it)->nFileSize;
								DWORD dwHi = (DWORD)((*it)->nFileSize>>32) ;

								BOOL b2 = (::SetFilePointer((*DstFileIt).hFile, dwLow, (PLONG)&dwHi, FILE_BEGIN)==INVALID_SET_FILE_POINTER);
								BOOL b = ::SetEndOfFile((*DstFileIt).hFile) ;

								//Release_Printf(_T("SetFilePointer()=%d SetEndOfFile()=%d"),b2,b) ;
							}
						}
/**/
						::SetFileTime((*DstFileIt).hFile,&(*it)->SrcFileTime.CreateTime,&(*it)->SrcFileTime.LastAccessTime,&(*it)->SrcFileTime.LastWriteTime) ;
						//
						//ReadyCloseHandleList.push_back(it2->second.hFile) ;
						::CloseHandle((*DstFileIt).hFile) ;

						if ((*it)->dwSourceAttr & (FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM)) 
						{
							::SetFileAttributes((*DstFileIt).strFileName.c_str(),(*it)->dwSourceAttr) ;
						}
						
#ifdef COMPILE_TEST_PERFORMANCE
							CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,6) ;
#endif
					}
					else
					{// 文件尚未完全写入磁盘

						if((*DstFileIt).hFile!=INVALID_HANDLE_VALUE)
						{
							::CloseHandle((*DstFileIt).hFile) ;
						}

						//ReadyCloseHandleList.push_back(it2->second.hFile) ;
						CptGlobal::ForceDeleteFile((*DstFileIt).strFileName) ;
					}

					if(DstFileIt==m_CurFileIterator) 
					{
						++m_CurFileIterator ;
					}

					m_FileInfoList.erase(DstFileIt) ;
				}
			}
			else
			{// 这是不应该找不到的，就算是抛弃的文件，在m_FileInfoLis里也该找到。请参见 'ExtremeCopy 原理.docx' 里的算法描述
				_ASSERT(FALSE) ;
				//Release_Printf(_T("RoundOffFile() not found")) ;
				//fdc.FileDoneConfirmList.pop_back() ;
			}
		}
		else
		{
			//Release_Printf(_T("CXCLocalFileDestnationFilter::RoundOffFile() discard  111")) ;

			if(bFound)
			{
				//Release_Printf(_T("CXCLocalFileDestnationFilter::RoundOffFile() discard  222")) ;

				if(!CptGlobal::IsFolder((*it)->dwSourceAttr))
				{
					//Release_Printf(_T("CXCLocalFileDestnationFilter::RoundOffFile() discard  333")) ;

					if((*DstFileIt).hFile!=INVALID_HANDLE_VALUE)
					{
						::CloseHandle((*DstFileIt).hFile) ;

						// 只有是已打开的文件都会删除，因为可能当复制时遇到相同文件，而此时用户点击'skip'的话，在目标文件夹的该文件是有效的，所以是不应该删除的
						CptGlobal::ForceDeleteFile((*DstFileIt).strFileName) ; 
					}

				}

				if(DstFileIt==m_CurFileIterator) 
				{
					++m_CurFileIterator ;
				}

				m_FileInfoList.erase(DstFileIt) ;

			}
			else
			{
				_ASSERT(FALSE) ;

			}
			//Release_Printf(_T("RoundOffFile() meet dicards file")) ;
		}

		
	}// end for

	if(m_pEvent!=NULL && !FeiVer.empty())
	{// 把完成写的文件往上回调
		m_pEvent->XCOperation_FileEnd(FeiVer) ;
	}

	if(!fdc.FileDoneConfirmList.empty())
	{// 发送文件完成确认命令
		m_pUpstreamFilter->OnDataTrans(this,EDC_FileDoneConfirm,&fdc) ;
	}

	//Win_Debug_Printf(_T("CXCLocalFileDestnationFilter::RoundOffFile() end")) ;
}


inline CptString CXCLocalFileDestnationFilter::MakeFileFullName(CptString strRawFileName)
{
	CptString strURI = m_DestURI.GetURI() ;
	CptString strRet ;

	if(m_bIsDriverRoot && strURI.GetLength()==0)
	{
		strRet = m_strDestRoot + strRawFileName  ;
	}
	else
	{
		CptString strRootWithoutSlash = m_strDestRoot ;

		strRootWithoutSlash.TrimRight('\\') ;
		strRootWithoutSlash.TrimRight('/') ;

		strRet = strRootWithoutSlash + strURI + _T("\\") + strRawFileName ;
	}

	return strRet ;
}

bool CXCLocalFileDestnationFilter::Connect(CXCFilter* pFilter,bool bUpstream) 
{
	bool bRet = false ;

	if(bUpstream && pFilter!=NULL && m_pUpstreamFilter==NULL)
	{
		m_pUpstreamFilter = pFilter ;
		pFilter->Connect(this,false) ;
		bRet = false ;
	}

	return bRet ;
}

int CXCLocalFileDestnationFilter::OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData)
{
	int nRet = 0 ;

	switch(cmd)
	{

	case EDC_Continue:
		{
			return this->OnContinue() ? 0 : 1 ;
		}
		break ;

	case EDC_Pause:
		{
			return this->OnPause() ? 0 : 1 ;
		}
		break ;

	case EDC_Stop:
		this->OnStop() ;
		break ;

	case EDC_LinkIni: // 新的FILTER LINK
		{
			SDataPack_LinkIni* pLinkIni = (SDataPack_LinkIni*)pFileData ;
			m_DestURI.Clean() ;

			_ASSERT(pLinkIni->pDefaultImpactFileBehavior!=NULL) ;

			//m_nSwapChunkSize = pLinkIni->pFileDataBuf->GetPageSize() ;
			m_nSwapChunkSize = pLinkIni->pFileDataBuf->GetChunkSize() ;

			m_pRunningState = pLinkIni->pCFState ;
			m_pFileDataBuf = pLinkIni->pFileDataBuf ;
			m_pFileChangingBuffer = pLinkIni->pFileChangingBuf ;
			m_pImpactFileBehavior = pLinkIni->pDefaultImpactFileBehavior ;

			this->OnInitialize() ;
		}
		break ;

	//case EDC_CreateFile:// 创建文件
	//	{
	//		_ASSERT(pFileData!=	NULL) ;
	//		SDataPack_SourceFileInfo* pSfi = (SDataPack_SourceFileInfo*)pFileData ;

	//		nRet = this->CreateXCFile(*pSfi) ;
	//	}
	//	break ;

	case EDC_BatchCreateFile: // 批量创建文件
		{
			_ASSERT(pFileData!=NULL) ;

			//Release_Printf(_T("CXCLocalFileDestnationFilter::OnDataTrans() EDC_BatchCreateFile")) ;

			SDataPack_CreateFileInfo* pCfi = (SDataPack_CreateFileInfo*)pFileData ;

#ifdef COMPILE_TEST_PERFORMANCE
			DWORD dw = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif
			this->OnCreateXCFile(*pCfi) ;

#ifdef COMPILE_TEST_PERFORMANCE
			CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,16) ;
#endif
		}
		break ;

	case EDC_FileData: // 文件数据
		{
			if(pFileData!=NULL)
			{
				SDataPack_FileData* pFD = (SDataPack_FileData*)pFileData ;
				//_ASSERT(pFD!=NULL) ;

				int nRet = this->WriteFileData(*pFD) ;
				
				//if(nRet==0 && m_pCurDstFileInfo!=NULL && !pFD->bDiscard)
				//{
				//	if((*m_CurFileIterator).uFileID==pFD->uFileID && (*m_CurFileIterator).uRemainSize<=pFD->nDataSize)
				//	{
				//		m_pEvent->XCOperation_FileDataDone(pFD->uFileID) ;
				//	}
				//}

				return nRet ;
			}
		}
		break ;

	case EDC_FileHash: // 文件的HASH 值
		break ;


	case EDC_FileOperationCompleted: // 作用于该文件的操作已完成
		{// 做文件的扫尾工作

			Debug_Printf(_T("CXCLocalFileDestnationFilter::OnDataTrans() EDC_FileOperationCompleted 1")) ;

			SDataPack_FileOperationCompleted* pFOC = (SDataPack_FileOperationCompleted*)pFileData ;

			_ASSERT(pFOC!=NULL) ;
			this->RoundOffFile(pFOC->CompletedFileInfoList) ;

			Debug_Printf(_T("CXCLocalFileDestnationFilter::OnDataTrans() EDC_FileOperationCompleted 2")) ;
			return 0 ;
		}
		break ;

	//case EDC_FolderChildrenOperationCompleted: // 作用于该文件夹的子文件和文件夹操作已完成
	//	{
	//		this->m_DestURI.Pop() ; // 弹出 URI 栈
	//	}
	//	break ;

	case EDC_LinkEnded: // 所有的复制工作准备完成，做最后的扫尾工作
		{
			SDataPack_FileOperationCompleted* pFOC = (SDataPack_FileOperationCompleted*)pFileData ;

			_ASSERT(pFOC!=NULL) ;

			this->OnLinkEnded(pFOC->CompletedFileInfoList) ;
		}
		
		break ;
	}

	return nRet ;
}


CXCLocalFileDestnationFilter::CXCDirectoryURI::CXCDirectoryURI() 
{
	m_bNewestURI = true ;
}

void CXCLocalFileDestnationFilter::CXCDirectoryURI::Clean() 
{
	m_URIDeque.clear() ;
	m_bNewestURI = false ;
}

void CXCLocalFileDestnationFilter::CXCDirectoryURI::Push(CptString strFolderName) 
{
	m_URIDeque.push_back(strFolderName) ;
	m_bNewestURI = false ;
}

void CXCLocalFileDestnationFilter::CXCDirectoryURI::Pop() 
{
	if(!m_URIDeque.empty())
	{
		m_URIDeque.pop_back() ;
		m_bNewestURI = false ;
	}
}

CptString CXCLocalFileDestnationFilter::CXCDirectoryURI::GetURI() 
{
	if(!m_bNewestURI)
	{
		m_strLastURI = _T("") ;

		if(!m_URIDeque.empty())
		{
			pt_STL_deque(CptString)::const_iterator it = m_URIDeque.begin() ;

			while(it!=m_URIDeque.end())
			{
				m_strLastURI += _T("\\") ;
				m_strLastURI += (*it) ;

				++it ;
			}
		}

		m_bNewestURI = true ;
	}

	return m_strLastURI ;
}

//CXCLocalFileDestnationFilter::CDstFileInfo::CDstFileInfo(const bool bSequence):m_bSequence(bSequence)
//{
//}
//
//SDstFileInfo* CXCLocalFileDestnationFilter::CDstFileInfo::GetFileInfoByID(const unsigned& uFileID) 
//{
//	
//}
//
//SDstFileInfo* CXCLocalFileDestnationFilter::CDstFileInfo::GetCurFileInfo() 
//{
//}
//
//void CXCLocalFileDestnationFilter::CDstFileInfo::AddDstFileInfo(const SDstFileInfo& dfi) 
//{
//}
//
//void CXCLocalFileDestnationFilter::CDstFileInfo::RemoveDstFileInfoByID(const unsigned& uFileID) 
//{
//}