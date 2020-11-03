/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCDuplicateOutputTransFilter.h"


CXCDuplicateOutputTransFilter::CXCDuplicateOutputTransFilter(CXCCopyingEvent* pEvent)
	:CXCTransformFilter(pEvent),m_pUpstreamFilter(NULL),m_nBranchNum(0)
{
}


CXCDuplicateOutputTransFilter::~CXCDuplicateOutputTransFilter(void)
{
}

bool CXCDuplicateOutputTransFilter::OnContinue() 
{
	return CXCTransformFilter::OnContinue() ;
}

bool CXCDuplicateOutputTransFilter::OnPause() 
{
	return CXCTransformFilter::OnPause() ;
}


void CXCDuplicateOutputTransFilter::OnStop() 
{
	CXCTransformFilter::OnStop() ;
}

bool CXCDuplicateOutputTransFilter::OnInitialize() 
{
	m_nBranchNum = (int)m_DownstreamFilterVer.size() ;

	_ASSERT(m_nBranchNum>1) ;

	return CXCTransformFilter::OnInitialize() ;
}

int CXCDuplicateOutputTransFilter::OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData) 
{
	int nRet = 0 ;

	switch(cmd)
	{
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

	case EDC_LinkIni: // 新的FILTER LINK
		{
			SDataPack_LinkIni* pIni = (SDataPack_LinkIni*)pFileData ;
			this->OnInitialize() ;
		}
		break ;

	case EDC_FileDoneConfirm:
		{
			SDataPack_FileDoneConfirm* pFdc = (SDataPack_FileDoneConfirm*)pFileData ;

			SDataPack_FileDoneConfirm fdc ;

			_ASSERT(pFdc!=NULL) ;

			pt_STL_list(unsigned)::iterator it = pFdc->FileDoneConfirmList.begin() ;
			pt_STL_map(unsigned,int)::iterator it2 ;

			{
				CptAutoLock lock(&m_Lock) ;

				for(;it!=pFdc->FileDoneConfirmList.end();++it)
				{
					it2 = m_FileDoneConfirmHashMap.find((*it)) ;

					if(it2!=m_FileDoneConfirmHashMap.end())
					{
						if(m_FileDoneConfirmHashMap[(*it)]+1==m_nBranchNum)
						{
							fdc.FileDoneConfirmList.push_back(*it) ;

							m_FileDoneConfirmHashMap.erase(it2) ;
						}
						else
						{
							++m_FileDoneConfirmHashMap[(*it)] ;
						}
					}
					else
					{
						m_FileDoneConfirmHashMap[(*it)] = 1 ;
					}
				}
			}

			if(!fdc.FileDoneConfirmList.empty())
			{
				m_pUpstreamFilter->OnDataTrans(this,EDC_FileDoneConfirm,&fdc) ;
			}
		}
		return 0 ;

	}

	if(pSender==this->m_pUpstreamFilter)
	{
		nRet = 0 ;

		pt_STL_vector(CXCFilterEventCB*)::iterator it= m_DownstreamFilterVer.begin() ;

		for(;it!=m_DownstreamFilterVer.end();++it)
		{
			(*it)->OnDataTrans(this,cmd,pFileData) ;
		}
	}
	else
	{
		nRet = m_pUpstreamFilter->OnDataTrans(this,cmd,pFileData) ;
	}

	return nRet ;
}

void CXCDuplicateOutputTransFilter::TransDataToDownStream(EFilterCmd cmd,void* pFileData) 
{
	pt_STL_vector(CXCFilterEventCB*)::iterator it= m_DownstreamFilterVer.begin() ;

	for(;it!=m_DownstreamFilterVer.end();++it)
	{
		(*it)->OnDataTrans(this,cmd,pFileData) ;
	}
}

bool CXCDuplicateOutputTransFilter::Connect(CXCFilter* pFilter,bool bUpstream) 
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
			bool bFound = false ;

			pt_STL_vector(CXCFilterEventCB*)::iterator it= m_DownstreamFilterVer.begin() ;

			for(;it!=m_DownstreamFilterVer.end();++it)
			{
				if((*it)==pFilter)
				{
					bFound = true ;
					break ;
				}
			}

			if(!bFound)
			{
				m_DownstreamFilterVer.push_back(pFilter) ;
				pFilter->Connect(this,true) ;
			}
		}

		bRet = true ;
	}

	return bRet ;
}


//int CXCDuplicateOutputTransFilter::OnPin_Data(CXCPin* pOwnerPin,EFilterCmd cmd,void* pFileData) 
//{
//	if(pOwnerPin==&m_InputPin)
//	{// 来自 input 的 pin
//
//		switch(cmd)
//		{
////		case EDC_FolderChildrenOperationCompleted: // 这是没返回值的命令
//		case EDC_FileDoneConfirm: 
//		case EDC_LinkIni:
//			for(size_t i=0;i<m_OutputPinVer.size();++i)
//			{
//				m_OutputPinVer[i].PushData(cmd,pFileData) ;
//			}
//			break ;
//
//		case EDC_FileHash: // 未确定部分
//			break ;
//
//		case EDC_FlushData:
//		case EDC_FileData:
//		case EDC_CreateFile:
//			{
//				size_t nIgnoreCount = 0 ;
//				size_t nSuccessCount = 0 ;
//
//				const size_t nPinNum = m_OutputPinVer.size() ;
//
//				for(size_t i=0;i<nPinNum;++i)
//				{
//					switch(m_OutputPinVer[i].PushData(cmd,pFileData))
//					{
//					case ErrorHandlingFlag_Exit:
//						return ErrorHandlingFlag_Exit ;
//
//					case ErrorHandlingFlag_Ignore:
//						++nIgnoreCount ;
//						break ;
//
//					case ErrorHandlingFlag_Success:
//						++nSuccessCount ;
//						break ;
//					}
//				}
//
//				return (nIgnoreCount==nPinNum) ? ErrorHandlingFlag_Ignore : ErrorHandlingFlag_Success ;
//			}
//			break ;
//		}
//	}
//	else
//	{// 来自 output 的 pin
//
//		switch(cmd)
//		{
//		case EDC_ResendFileData:
//			break ;
//
//		case EDC_FileDoneConfirm:
//			{
//				_ASSERT(pFileData!=NULL) ;
//
//				SDataPack_FileDoneConfirm* pConfirm = (SDataPack_FileDoneConfirm*)pFileData ;
//
//				pt_STL_list(unsigned)::const_iterator FileIt = pConfirm->FileDoneConfirmList.begin() ;
//
//				for(;FileIt!=pConfirm->FileDoneConfirmList.end();++FileIt)
//				{
//					pt_STL_hash_map(unsigned,int)::iterator& it = m_FileDoneConfirmHashMap.find(*FileIt) ;
//
//					bool bForward = false ;
//
//					if(it==m_FileDoneConfirmHashMap.end())
//					{// 若该ID是第一次记录
//						if(m_OutputPinVer.size()==1)
//						{
//							bForward = true ;
//						}
//						else
//						{
//							m_FileDoneConfirmHashMap[*FileIt] = 1 ;
//						}
//					}
//					else
//					{
//						(*it).second++ ;
//
//						if((*it).second==m_OutputPinVer.size())
//						{
//							bForward = true ;
//							m_FileDoneConfirmHashMap.erase(it) ;
//						}
//					}
//
//					if(bForward)
//					{
//						m_InputPin.PushData(cmd,pFileData) ;
//					}
//				}
//				
//			}
//			break ;
//		}
//		m_InputPin.PushData(cmd,pFileData) ;
//	}
//
//	return 0 ;
//}
