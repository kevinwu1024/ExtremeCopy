/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCFileDataCacheTransFilter.h"
#include "XCCopyingEvent.h"
#include "XCFileDataBuffer.h"

CXCFileDataCacheTransFilter::CXCFileDataCacheTransFilter(CXCCopyingEvent* pEvent)
	:CXCTransformFilter(pEvent),m_pUpstreamFilter(NULL),m_pDownstreamFilter(NULL)
{
	m_pFileDataBuf = NULL ;

	m_bCacheCmdThreadSafe = true ; // 默认是线程安全的
	m_bHaveCreateFileCmd = false ;
}


CXCFileDataCacheTransFilter::~CXCFileDataCacheTransFilter(void)
{
}

int CXCFileDataCacheTransFilter::WriteFileData(const EFilterCmd cmd,void* pFileData) 
{
	int nRet = ErrorHandlingFlag_Success ;

	pt_STL_deque(SDataPack_FileData)::iterator it = m_FileDataQue.begin() ;

	nRet = this->m_pDownstreamFilter->OnDataTrans(this,EDC_FileData,&(*it)) ;

	this->m_pFileDataBuf->Free((*it).pData,(*it).nDataSize) ;

	return nRet ;
}

bool CXCFileDataCacheTransFilter::OnInitialize() 
{
	return CXCTransformFilter::OnInitialize() ;
}

bool CXCFileDataCacheTransFilter::OnContinue() 
{
	return CXCTransformFilter::OnContinue() ;
}

bool CXCFileDataCacheTransFilter::OnPause() 
{
	return CXCTransformFilter::OnPause() ;
}


void CXCFileDataCacheTransFilter::OnStop() 
{
	CXCTransformFilter::OnStop() ;
}

bool CXCFileDataCacheTransFilter::Connect(CXCFilter* pFilter,bool bUpstream)
{
	bool bRet = false ;

	if(pFilter!=NULL)
	{
		if(bUpstream)
		{
			if(m_pUpstreamFilter==NULL)
			{
				m_pUpstreamFilter = pFilter ;
				pFilter->Connect(this,false) ;
			}
			
		}
		else
		{
			if(m_pDownstreamFilter==NULL)
			{
				m_pDownstreamFilter = pFilter ;
				pFilter->Connect(this,true) ;
			}
		}

		bRet = true ;
	}

	return bRet ;
}

bool CXCFileDataCacheTransFilter::IsHaveCreateFileCache() const
{
	return m_bHaveCreateFileCmd ;
}

void CXCFileDataCacheTransFilter::SendCacheBathCreateCommand() 
{
	if(!m_CacheCreateFileInfo.SourceFileInfoList.empty())
	{
		_ASSERT(m_bHaveCreateFileCmd) ;

		m_pDownstreamFilter->OnDataTrans(this,EDC_BatchCreateFile,&m_CacheCreateFileInfo) ;
		m_CacheCreateFileInfo.SourceFileInfoList.clear() ;

		m_bHaveCreateFileCmd = false ;
	}
}

void CXCFileDataCacheTransFilter::OnLinkEnded(pt_STL_list(SDataPack_SourceFileInfo*)& FileList) 
{
	this->FlushFileOperCompletedCommand(FileList) ;
}

int CXCFileDataCacheTransFilter::OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData) 
{
	int nRet = 0 ;

	switch(cmd)
	{

	case EDC_LinkIni: // 新的FILTER LINK
		{
			SDataPack_LinkIni* pIni = (SDataPack_LinkIni*)pFileData ;

			_ASSERT(pIni!=NULL) ;
			_ASSERT(pIni->pFileDataBuf!=NULL) ;

			m_pFileDataBuf = pIni->pFileDataBuf ;
			m_pRunningState = pIni->pCFState ;
			m_pFileChangingBuffer = pIni->pFileChangingBuf ;

			this->OnInitialize() ;
		}
		break ;

	case EDC_Continue:
		{
			nRet = this->OnContinue() ? 0 : 1 ;
		}
		break ;

	case EDC_Pause:
		{
			nRet = this->OnPause() ? 0 : 1 ;
		}
		break ;

	case EDC_Stop:
		this->OnStop() ;
		break ;

	case EDC_BatchCreateFile: // 创建文件
		{
			_ASSERT(pFileData!=NULL) ;

			SDataPack_CreateFileInfo* cfi = (SDataPack_CreateFileInfo*)pFileData ;

			_ASSERT(!cfi->SourceFileInfoList.empty()) ;

			pt_STL_list(SDataPack_SourceFileInfo*)::const_iterator it = cfi->SourceFileInfoList.begin() ;

			for(;it!=cfi->SourceFileInfoList.end();++it)
			{
				m_CacheCreateFileInfo.SourceFileInfoList.push_back((*it)) ;
			}

			m_bHaveCreateFileCmd = true ;
		}
		return 0 ;

	case EDC_FileData: // 文件数据
		{// 该命令在
			SDataPack_FileData* pFD = (SDataPack_FileData*)pFileData ;

			bool bAddToQue = m_FileDataQue.empty() ;

			if(!bAddToQue)
			{
				const SDataPack_FileData& fd = m_FileDataQue.back() ;

				bAddToQue = (fd.uFileID==0 || fd.pData==NULL || fd.uFileID!=pFD->uFileID || (BYTE*)fd.pData + fd.nDataSize!=(BYTE*)pFD->pData) ;
			}

			if(bAddToQue)
			{
				m_FileDataQue.push_back(*pFD) ;
			}
			else
			{
				SDataPack_FileData& fd = m_FileDataQue.back() ;

				_ASSERT(pFD->uFileID==fd.uFileID) ;
				_ASSERT(pFD->pData==(char*)fd.pData+fd.nDataSize) ;
//				_ASSERT(pFD->uReadBeginPos!=fd.uReadBeginPos) ;

				fd.nDataSize += pFD->nDataSize ;
				fd.nBufSize += pFD->nBufSize ;
			}
		}
		return 0 ;


	case EDC_LinkEnded:
		{
			_ASSERT(pFileData!=NULL) ;

			SDataPack_FileOperationCompleted* pFoc = (SDataPack_FileOperationCompleted*)pFileData ;

			this->OnLinkEnded(pFoc->CompletedFileInfoList) ;
		}
		break ;



	//case EDC_FileOperationCompleted:
	//	{
	//		//SDataPack_FileOperationCompleted* pFileData
	//		//PushToCacheCmdQue<CNullObject>(EDC_FileOperationCompleted,NULL,

	//		//m_CacheCmdQue
	//		//if(m_bCacheCmdThreadSafe)
	//		//{
	//		//	CptAutoLock lock(&m_CacheCmdLock) ;
	//		//	m_CacheCmdQue.push_back(cci) ;
	//		//}
	//		//else
	//		//{
	//		//	m_CacheCmdQue.push_back(cci) ;
	//		//}

	//		return PushToCacheCmdQueWithAlloc<SDataPack_SourceFileInfo>(cmd,pFileData) ;
	//	}

	//case EDC_ResendFileData:
	//	return PushToCacheCmdQueWithAlloc<SDataPack_ResendFileData>(cmd,pFileData) ;

	//case EDC_FileDoneConfirm:
	//	{
	//		return PushToCacheCmdQueWithAlloc<SDataPack_FileDoneConfirm>(cmd,pFileData) ;
	//	}

	//case EDC_FolderChildrenOperationCompleted:
	//	return PushToCacheCmdQue<CNullObject>(cmd,NULL,pFileData) ;
	}

	// 把数据往下一个 filter 推
	if(pSender==this->m_pDownstreamFilter)
	{
		nRet = m_pUpstreamFilter->OnDataTrans(this,cmd,pFileData) ;
		
	}
	else
	{
		nRet = m_pDownstreamFilter->OnDataTrans(this,cmd,pFileData) ;
	}

	return nRet ;
}

void CXCFileDataCacheTransFilter::FlushFileOperCompletedCommand(const pt_STL_list(SDataPack_SourceFileInfo*)& CompleteCmdList) 
{
	SDataPack_FileOperationCompleted fdc ;

	fdc.CompletedFileInfoList = CompleteCmdList ;

	m_pDownstreamFilter->OnDataTrans(this,EDC_FileOperationCompleted,&fdc) ;

}