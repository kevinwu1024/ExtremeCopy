/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCCore.h"
#include "XCWinStorageRelative.h"
#include "XCAsyncFileDataTransFilter.h"
#include "XCSyncFileDataTransFilter.h"
#include "XCDuplicateOutputTransFilter.h"
#include "XCLocalFileSyncSourceFilter.h"
#include "XCLocalFileSyncDestnationFilter.h"
#include "XCLocalFileAsynSourceFilter.h"
#include "XCLocalFileAsynDestnationFilter2.h"
#include "XCCopyingEvent.h"

#include <shlobj.h>

#pragma comment(lib,"Winmm.lib")

//============================================


CXCCore::CXCCore(void):m_bIni(false)
{
}


CXCCore::~CXCCore(void)
{
	SDataPack_SourceFileInfo::ReleaseBuffer() ; // 释放缓冲区内存
}

bool CXCCore::Run(const SGraphTaskDesc& gtd)
{
	bool bRet = false ;

	if(!m_bIni)
	{
		bRet = true ;
		m_bIni = true ;

		m_ImpactFileBehavior = gtd.ImpactFileBehavior ;
		m_RunningState = CFS_Stop ;
		CXCDestinationFilter::ResetCanCallbackMark() ;

		Debug_Printf(_T("CXCCore::Run() 1")) ;

		if(gtd.ExeType==XCTT_Move && gtd.DstFolderVer.size()==1)
		{// 在同一分区内移动文件
			SStorageInfoOfFile SrcSiof ;

			SStorageInfoOfFile DstSiof ;

			CXCWinStorageRelative::GetFileStoreInfo(gtd.DstFolderVer[0],DstSiof) ;
			size_t i = 0 ;

			SGraphTaskDesc NormalGtd = gtd;
			SGraphTaskDesc SameMovingGtd = gtd;

			NormalGtd.SrcFileVer.clear() ;
			SameMovingGtd.SrcFileVer.clear() ;

			// 把源文件按是否同一分区进行各文件拆分开来，以便在以不同的复制策略
			for( i=0;i<gtd.SrcFileVer.size();++i)
			{
				if(CXCWinStorageRelative::GetFileStoreInfo(gtd.SrcFileVer[i],SrcSiof))
				{
					if(SrcSiof.dwStorageID!=DstSiof.dwStorageID 
						|| SrcSiof.nPartitionIndex!=DstSiof.nPartitionIndex
						|| SrcSiof.nSectorSize!=DstSiof.nSectorSize
						|| SrcSiof.uDiskType!=DstSiof.uDiskType)
					{
						NormalGtd.SrcFileVer.push_back(gtd.SrcFileVer[i]) ;
					}
					else
					{
						SameMovingGtd.SrcFileVer.push_back(gtd.SrcFileVer[i]) ;
					}
				}
				else
				{
					_ASSERT(FALSE) ;
				}
			}

			// 以同一分区策略进行移动复制
			if(!SameMovingGtd.SrcFileVer.empty())
			{
				CXCSameDriveMoveFile same(&m_RunningState,&m_ImpactFileBehavior) ;

				bRet = same.Run(SameMovingGtd) ;
			}

			// 以普通复制模式进行复制
			if(bRet && !NormalGtd.SrcFileVer.empty())
			{
				bRet = this->NormalRun(NormalGtd) ;
			}
		}
		else
		{
			bRet = this->NormalRun(gtd) ;
		}

		this->Stop() ;
	}

	return bRet ;
}

bool CXCCore::NormalRun(const SGraphTaskDesc& gtd) 
{
	bool bRet = this->CreateLinkGraph(gtd,m_GraphSet) ;

	if(bRet)
	{
		bRet = m_GraphSet.Start() ;
	}

	return bRet ;
}


void CXCCore::Stop() 
{
	CptAutoLock lock(&m_Lock) ;

	if(m_bIni)
	{
		m_bIni = false ;
		m_GraphSet.Stop() ;

		CXCDestinationFilter::ResetCanCallbackMark() ;
	}

}

void CXCCore::Pause() 
{
	CptAutoLock lock(&m_Lock) ;

	if(m_bIni)
	{
		m_GraphSet.Pause() ;
	}
}

void CXCCore::Continue() 
{
	CptAutoLock lock(&m_Lock) ;

	if(m_bIni)
	{
		m_GraphSet.Continue() ;
	}
}

bool CXCCore::Skip()
{
	CptAutoLock lock(&m_Lock) ;

	return (m_bIni && m_GraphSet.Skip()) ;
}

CptString CXCCore::GetCurWillSkipFile() 
{
	CptAutoLock lock(&m_Lock) ;

	return m_GraphSet.GetCurWillSkipFile() ;
}

ECopyFileState CXCCore::GetState()  
{
	CptAutoLock lock(&m_Lock) ;
	return m_GraphSet.GetState() ;
}

// 处理异常
bool CXCCore::ProcessException(CXCCopyingEvent* pEvent)
{
	SXCExceptionInfo ei ;
	ei.ErrorCode.nSystemError = ::GetLastError() ;

	ei.SupportType = ErrorHandlingFlag_Exit|ErrorHandlingFlag_Retry ;

	ErrorHandlingResult result = pEvent->XCOperation_CopyExcetption(ei) ;

	return (result==ErrorHandlingFlag_Retry) ;
}

// 分配文件数据缓冲区
bool CXCCore::AllocFileDataBuffer(const SGraphTaskDesc& gtd)
{
	if(!m_FileDataBuffer.IsAllocateChunk()
		|| m_FileDataBuffer.GetChunkSize()!=gtd.nFileDataBufSize)
	{// 只申请一次，以第一次申请为准
		while(!m_FileDataBuffer.AllocateChunk(gtd.nFileDataBufSize,2*1024))
		{
			// 申请内存失败后,则告诉用户如何处理
			// 用户可以：重试 或 退出程序
			if(!this->ProcessException(gtd.pCopyingEvent))
			{
				return false ;
			}
		}
	}

	return true ;
}

// 创建工作链路
bool CXCCore::CreateLinkGraph(const SGraphTaskDesc& gtd,CXCGraphSet& GraphSet)
{
	SLinkDesc ld ;

	bool bRet = false ;

	do
	{
		// 解析任务信息从而建构 复制工作链
		PT_BREAK_IF(!this->ParseTaskToGraphDesc(gtd,ld)) ;

		// 分配文件数据内存
		PT_BREAK_IF(!this->AllocFileDataBuffer(gtd)) ;

		// 复位 文件数据内存
		m_FileDataBuffer.ResetRef() ;

		CXCSourceFilter* pSourceFilter = NULL ;
		CXCDestinationFilter* pDestFilter = NULL ;

		CXCTransformFilter* pTransFilter = NULL ;
		CXCDuplicateOutputTransFilter* pDupTransFilter = NULL ;

		int nIndex = 0 ;

		CXCGraph LinkGraph ;
		SXCSourceFilterTaskInfo sfti ;

		sfti.pFileDataBuf = &m_FileDataBuffer ;
		sfti.pFileChangingBuf = &m_FileChangingBuffer ;
		sfti.nValidCachePointNum = (int)ld.BranchLinkVer.size() ;
		sfti.ExeType = gtd.ExeType ;
		sfti.pDefaultImpactFileBehavior = &m_ImpactFileBehavior ;
		sfti.pRunningState = &m_RunningState ;

		int nSameDeviceCount = 0 ;
		int nDiffDeviceCount = 0 ;

		auto it = ld.SrcLinkVer.begin() ;

		// 默认为成功
		bRet = true ;

		for(;it!=ld.SrcLinkVer.end();++it)
		{
			const SFileStorageInfo& fsi = (*it) ;
			// 创建 source filter 
			while(bRet && (pSourceFilter = new CXCLocalFileSyncSourceFilter(gtd.pCopyingEvent)) ==NULL)
			{
				if(!this->ProcessException(gtd.pCopyingEvent))
				{
					LinkGraph.Release() ;
					GraphSet.Stop() ;
					bRet = false ;
				}
			}

			PT_BREAK_IF(!bRet) ; // 若创建 source filter 失败，则退出

			sfti.SrcFileVer = fsi.strFileOrFolderVer ;

			pSourceFilter->SetTask(sfti) ; // 指定任务参数

			_ASSERT(pSourceFilter!=NULL) ;

			LinkGraph.AddFilter(pSourceFilter) ; // 添加 source filter 到 graph link

			if(ld.BranchLinkVer.size()>1)
			{// 有多个分支, 则创建 duplicate transfer filter

				// 创建 duplicate transfer filter
				while(bRet && (pDupTransFilter = new CXCDuplicateOutputTransFilter(gtd.pCopyingEvent)) ==NULL)
				{
					if(!this->ProcessException(gtd.pCopyingEvent))
					{
						LinkGraph.Release() ;
						GraphSet.Stop() ;
						bRet = false ;
					}
				}

				PT_BREAK_IF(!bRet) ; // 若创建 dulplicate 分支失败，则退出

				LinkGraph.AddFilter(pDupTransFilter) ; // 添加 duplicate transfer filter 到 graph link
			}

			for(size_t j=0;bRet && j<ld.BranchLinkVer.size();++j)
			{// 连接各个分支
				if(ld.BranchLinkVer[j].siof.IsSameStorage(fsi.siof))
				{// 同一个物理存储器
					++nSameDeviceCount ;

					// 创建同步的 transfer filter
					while(bRet && (pTransFilter = new CXCSyncFileDataTransFilter(gtd.pCopyingEvent)) ==NULL)
					{
						if(!this->ProcessException(gtd.pCopyingEvent))
						{
							LinkGraph.Release() ;
							GraphSet.Stop() ;
							bRet = false ;
						}
					}

					PT_BREAK_IF(!bRet) ; // 若创建 filter 失败，则退出
				}
				else
				{// 不同的物理存储器
					++nDiffDeviceCount ;

					// 创建异步的 transfer filter
					while(bRet && (pTransFilter = new CXCAsyncFileDataTransFilter(gtd.pCopyingEvent)) ==NULL)
					{
						if(!this->ProcessException(gtd.pCopyingEvent))
						{
							LinkGraph.Release() ;
							GraphSet.Stop() ;
							bRet = false ;
						}
					}

					PT_BREAK_IF(!bRet) ; // 若创建 transfer filter 失败，则退出
				}

				//  创建 destination filter
				while(bRet && (pDestFilter = new CXCLocalFileSyncDestnationFilter(gtd.pCopyingEvent,
					ld.BranchLinkVer[j].strFileOrFolderVer[0],ld.BranchLinkVer[j].siof,
					gtd.bIsRenameDst)) ==NULL)
				{
					if(!this->ProcessException(gtd.pCopyingEvent))
					{
						LinkGraph.Release() ;
						GraphSet.Stop() ;
						bRet = false ;
					}
				}

				PT_BREAK_IF(!bRet) ; // 若创建 destination filter 失败，则退出

				_ASSERT(pTransFilter!=NULL) ;

				LinkGraph.AddFilter(pTransFilter) ; // 添加 transfer filter 到 graph link

				_ASSERT(pDestFilter!=NULL) ;

				pDestFilter->Connect(pTransFilter,true) ;

				LinkGraph.AddFilter(pDestFilter) ; // 添加 destination filter 到 graph link

				if(pDupTransFilter!=NULL)
				{// 各个分支与duplicate transfer filter各输出分支连接
					pDupTransFilter->Connect(pTransFilter,false) ;
				}
			}// for() 创建各分支

			PT_BREAK_IF(!bRet) ; // 创建分支失败，则退出
			
			if(nSameDeviceCount==0)
			{
				pSourceFilter->SetSmallReadGranularity(true) ;
			}

			if(pDupTransFilter!=NULL)
			{// 若存在duplicate,则 把 source filter 与 duplicate transfer filter 连接
				pSourceFilter->Connect(pDupTransFilter,false) ;
			}
			else
			{// 把source filter 与 单一的transfer 连接
				pSourceFilter->Connect(pTransFilter,false) ;
			}

			GraphSet.AddGraph(LinkGraph) ;
			LinkGraph.Clear() ;
		}
	}
	while(0);

	return bRet ;
}


bool CXCCore::ParseTaskToGraphDesc(const SGraphTaskDesc& gtd,SLinkDesc& ld)
{
	bool bRet = false ;

	if(!gtd.SrcFileVer.empty() && !gtd.DstFolderVer.empty())
	{
		SFileStorageInfo fsi ;

		for(size_t j=0;j<gtd.DstFolderVer.size();++j)
		{
			fsi.strFileOrFolderVer.clear() ;
			if(CXCWinStorageRelative::GetFileStoreInfo(gtd.DstFolderVer[j],fsi.siof))
			{// 创建destination filter分支数
				if(fsi.siof.uDiskType==DRIVE_CDROM)
				{// 若目标磁盘为光驱，那么就以失败退出

					if(gtd.pCopyingEvent!=NULL)
					{
						SXCExceptionInfo ei ;

						ei.ErrorCode.nSystemError = ERROR_BAD_COMMAND ;
						ei.strDstFile = gtd.DstFolderVer[j] ;
						ei.SupportType = ErrorHandlingFlag_Exit ;

						gtd.pCopyingEvent->XCOperation_CopyExcetption(ei) ;
					}

					return false ;
				}

				fsi.strFileOrFolderVer.push_back(gtd.DstFolderVer[j]) ;
				ld.BranchLinkVer.push_back(fsi) ;
			}
			else
			{
				_ASSERT(FALSE) ;
			}
		}

		bool bAdded = false ;

		for(size_t i=0;i<gtd.SrcFileVer.size();++i)
		{// 创建 link 数目
			bAdded = false ;

			const bool& bSuccess = CXCWinStorageRelative::GetFileStoreInfo(gtd.SrcFileVer[i],fsi.siof) ;

			_ASSERT(bSuccess) ;
			
			fsi.strFileOrFolderVer.clear() ;

			for(size_t j=0;j<ld.SrcLinkVer.size();++j)
			{
				if(ld.SrcLinkVer[j].siof.IsSameStorage(fsi.siof))
				{
					ld.SrcLinkVer[j].strFileOrFolderVer.push_back(gtd.SrcFileVer[i]) ;
					bAdded = true ;
					break ;
				}
			}

			if(!bAdded)
			{
				fsi.strFileOrFolderVer.push_back(gtd.SrcFileVer[i]) ;
				ld.SrcLinkVer.push_back(fsi) ;
			}
		}

		bRet = true ;
	}

	return bRet ;
}



//====================== CXCGraphSet ============

CXCCore::CXCGraphSet::CXCGraphSet():m_nCurIndex(0)
{
}

CXCCore::CXCGraphSet::~CXCGraphSet() 
{
	this->Stop() ;
}

int CXCCore::CXCGraphSet::GetCount()
{
	return (int)m_GraphList.size() ;
}

void CXCCore::CXCGraphSet::Release()
{
	pt_STL_list(CXCGraph)::iterator it = m_GraphList.begin() ;

	while(it!=m_GraphList.end())
	{
		(*it).Stop() ;
		(*it).Release() ;
		++it ;
	}

	m_GraphList.clear() ;
}

bool CXCCore::CXCGraphSet::Start() 
{
	if(this->GetState()!=CFS_Stop)
	{
		return false ;
	}

	if(this->GetState()==CFS_Stop)
	{
		CXCGraph* pGraph = NULL ;
		const size_t& nGraphCount = m_GraphList.size() ;

		// 对每一个工作链路进行初始化
		for(size_t i=0;i<nGraphCount;++i)
		{
			pGraph = this->GetCurGraph() ;

			_ASSERT(pGraph!=NULL) ;

			if(pGraph!=NULL && !pGraph->Initialize())
			{
				return false ;
			}

			++m_nCurIndex ; // 调整到下一个 Graph
		}

		m_nCurIndex = 0 ;

		// 运行逐个工作链路
		for(size_t i=0;i<nGraphCount;++i)
		{
			pGraph = this->GetCurGraph() ;

			_ASSERT(pGraph!=NULL) ;

			if(pGraph!=NULL && !pGraph->Run())
			{
				return false ;
			}

			// 不能在这里 release 和 m_GraphList.clear(), 因为CXCCore::CXCGraphSet::Start() 是被 CXCCore::Run() 调用的
			// 而当任务最后一个文件刚好完成时，用户点击了Skip()，尽管每个行为函数都用锁进行线程保护
			// 但恰好这个CXCCore::Run()没有，所以就会造成Skip()进来后并不保证source filter还存在
			// 所以不能在这里 Release()， 而应在 CXCCore::Stop() 处Release，因为Stop()是线程安全的
			//pGraph->Release() ;

			++m_nCurIndex ; // 调整到下一个 Graph
		}

		//m_GraphList.clear() ;
	}

	return true ;
}

bool CXCCore::CXCGraphSet::Pause() 
{
	CXCGraph* pGraph = this->GetCurGraph() ;

	return ((pGraph!=NULL) && pGraph->Pause()) ;
}

bool CXCCore::CXCGraphSet::Skip() 
{
	CXCGraph* pGraph = this->GetCurGraph() ;

	return ((pGraph!=NULL) && pGraph->Skip()) ;
}

bool CXCCore::CXCGraphSet::Continue()
{
	CXCGraph* pGraph = this->GetCurGraph() ;

	return ((pGraph!=NULL) && pGraph->Continue()) ;
}

void CXCCore::CXCGraphSet::Stop() 
{
	CXCGraph* pGraph = this->GetCurGraph() ;

	if(pGraph!=NULL)
	{
		pGraph->Stop() ;
	}

	m_nCurIndex = 0 ;

	this->Release() ;
}

CptString CXCCore::CXCGraphSet::GetCurWillSkipFile() 
{
	CptString strRet ;

	CXCGraph* pGraph = this->GetCurGraph() ;

	if(pGraph!=NULL)
	{
		strRet = pGraph->GetCurWillSkipFile() ;
	}

	return strRet ;
}

ECopyFileState CXCCore::CXCGraphSet::GetState()  
{
	CXCGraph* pGraph = this->GetCurGraph() ;

	ECopyFileState ret = CFS_Stop ;

	if(pGraph!=NULL)
	{
		ret = pGraph->GetState() ;
	}
	//_ASSERT(pGraph!=NULL) ;

	return ret ;
}

void CXCCore::CXCGraphSet::AddGraph(CXCGraph& graph) 
{
	m_GraphList.push_back(graph) ;
}

CXCCore::CXCGraph* CXCCore::CXCGraphSet::GetCurGraph() 
{
	int nCount = 0 ;

	pt_STL_list(CXCGraph)::iterator it = m_GraphList.begin() ;

	while(it!=m_GraphList.end())
	{
		if(m_nCurIndex==nCount++)
		{
			return &(*it) ;
		}
		++it ;
	}

	return NULL ;
}

//======================= CXCGraph ================

CXCCore::CXCGraph::CXCGraph():pSourceFilter(NULL)
{
}

CXCCore::CXCGraph::~CXCGraph()
{
}

void CXCCore::CXCGraph::Release()
{
	auto it = FilterList.begin() ;

	while(it!=FilterList.end())
	{
		CXCFilter* pp = *it ;

		_ASSERT(pp!=NULL) ;

		delete pp ;
		++it ;
	}

	FilterList.clear() ;
}

void CXCCore::CXCGraph::AddFilter(CXCFilter* pFilter)
{
	FilterList.push_back(pFilter) ;

	if(pFilter->GetType()==FilterType_Source)
	{// 若为 source filter
		pSourceFilter = (CXCSourceFilter*)pFilter ;
	}
}

void CXCCore::CXCGraph::Clear() 
{
	FilterList.clear() ;
}

bool CXCCore::CXCGraph::Initialize() 
{
	return true ;
}

bool CXCCore::CXCGraph::Run() 
{
	if(!FilterList.empty())
	{
		if(pSourceFilter!=NULL)
		{
			return pSourceFilter->Run() ;
		}
	}

	return false ;
}

bool CXCCore::CXCGraph::Pause()
{
	if(pSourceFilter!=NULL)
	{
		return pSourceFilter->Pause() ;
	}

	return false ;
}

void CXCCore::CXCGraph::Stop()
{
	if(pSourceFilter!=NULL)
	{
		pSourceFilter->Stop() ;
	}
}

bool CXCCore::CXCGraph::Skip() 
{
	if(pSourceFilter!=NULL)
	{
		return pSourceFilter->Skip() ;
	}
	return false ;
}

bool CXCCore::CXCGraph::Continue() 
{
	if(pSourceFilter!=NULL)
	{
		return pSourceFilter->Continue() ;
	}
	return false ;
}

CptString CXCCore::CXCGraph::GetCurWillSkipFile() 
{
	CptString strRet ;

	if(pSourceFilter!=NULL)
	{
		SDataPack_SourceFileInfo* pSfi = pSourceFilter->GetCurrentSFI() ;

		if(pSfi!=NULL)
		{
			strRet = pSfi->strSourceFile ;
		}
	}

	return strRet ;
}

ECopyFileState CXCCore::CXCGraph::GetState() const
{
	if(pSourceFilter!=NULL)
	{
		return pSourceFilter->GetState() ;
	}

	return CFS_Stop ;
}