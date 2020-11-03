/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCFileDataBuffer.h"
#include "..\Common\ptGlobal.h"
#include "../Common/ptDebugView.h"
#include <set>


CXCFileDataBuffer::CXCFileDataBuffer(void):m_pBlockBuf(NULL),m_pRefCountBuf(NULL)
	,m_nCurAllocIndex(0),m_nChunkSize(32*1024*1024),m_nRefPageCount(0)
{
#ifdef _DEBUG
	m_nIDCount = 0 ;
	m_nLastFreeID = 0 ;
	m_pAllocIDRecordBuf = NULL ;
#endif
}


CXCFileDataBuffer::~CXCFileDataBuffer(void)
{
	this->Release() ;
}

void CXCFileDataBuffer::ResetRef() 
{
	m_nRefPageCount = 0 ;
	m_nCurAllocIndex = 0 ;

	::memset(m_pRefCountBuf,0,m_nRefNum) ;

}

void CXCFileDataBuffer::Release() 
{
	CptAutoLock lcok(&m_Lock) ;

	if(m_pBlockBuf!=NULL)
	{
		::VirtualUnlock(m_pBlockBuf,m_nChunkSize) ;
		::VirtualFree(m_pBlockBuf,m_nChunkSize,MEM_RELEASE) ;
		//::free(m_pBlockBuf) ;
		m_pBlockBuf = NULL ;
	}

	if(m_pRefCountBuf!=NULL)
	{
		::free(m_pRefCountBuf) ;
		m_pRefCountBuf = NULL ;
	}

	_ASSERTE( _CrtCheckMemory( ) );
}

bool CXCFileDataBuffer::IsAllocateChunk() const 
{
	return (m_pBlockBuf!=NULL) ;
}

bool CXCFileDataBuffer::AllocateChunk(int nChunkSize,int nAlignSize)
{
	bool bRet = false ;

	this->Release() ;

	CptAutoLock lcok(&m_Lock) ;

	if(nAlignSize<=0 || nAlignSize%512)
	{
		nAlignSize = ALIGN_SIZE_UP(nAlignSize,1024) ;
	}

	m_nPageSize = nAlignSize ;

	switch(nChunkSize)
	{
	case (2*1024*1024):
	case (4*1024*1024):
	case (8*1024*1024):
	case (32*1024*1024):
		break ;

	default:
	case (16*1024*1024):
		nChunkSize = 16*1024*1024 ;
		break ;

	}

	//if(nChunkSize<1024*1024)
	//{
	//	nChunkSize = 32*1024*1024 ;
	//}

	do
	{
		m_pBlockBuf = (BYTE*)::VirtualAlloc(NULL,nChunkSize,MEM_COMMIT,PAGE_READWRITE) ;

		if(m_pBlockBuf==NULL)
		{
			nChunkSize = (nChunkSize)>>1 ;
		}
	}
	while((m_pBlockBuf==NULL) && nChunkSize>4*1024);

	if(m_pBlockBuf!=NULL)
	{
		::VirtualLock(m_pBlockBuf,nChunkSize) ; // 让其在程序运行期间常驻内存，以提高性能

		m_nRefNum = ALIGN_SIZE_DOWN(nChunkSize,m_nPageSize)/m_nPageSize ;
		m_pRefCountBuf = (BYTE*)::malloc(m_nRefNum) ;

#ifdef _DEBUG
		m_pAllocIDRecordBuf = (unsigned*)::malloc(m_nRefNum*sizeof(unsigned)) ;
		::memset(m_pAllocIDRecordBuf,0,m_nRefNum*sizeof(unsigned)) ;
#endif
		m_nCurAllocIndex = 0 ;

		bRet = (m_pRefCountBuf!=NULL) ;

		if(bRet)
		{
			::memset(m_pBlockBuf,0,nChunkSize) ;
			::memset(m_pRefCountBuf,0,m_nRefNum) ;
		}
	}

	_ASSERTE( _CrtCheckMemory( ) );

	m_nChunkSize = nChunkSize ;

	return bRet ;
}

int CXCFileDataBuffer::GetChunkSize() const 
{
	return m_nChunkSize ;
}


bool CXCFileDataBuffer::IsEmpty()  
{
	CptAutoLock lock(&m_Lock) ;

	return m_nRefPageCount==0 ;
}

bool CXCFileDataBuffer::IsFull()  
{
	CptAutoLock lock(&m_Lock) ;

	return m_nRefPageCount==m_nRefNum ;
}
//
//void CXCFileDataBuffer::WaitForEvent(EWaitForEventType wfet,HANDLE hEvent,DWORD dwInterval)
//{
//	if(hEvent!=NULL)
//	{
//		{
//			CptAutoLock lock(&m_Lock) ;
//
//			switch(wfet)
//			{
//			case WFET_WaitForBufNotFull:
//			case WFET_WaitForBufEmpty: 
//				if(m_nRefPageCount==0)
//				{
//					return ;
//				}
//				break ;
//
//			case WFET_WaitForBufNotEmpty:
//			case WFET_WaitForBufFull:
//				if(m_nRefPageCount==m_nRefNum)
//				{
//					return ;
//				}
//				break ;
//			}
//
//			m_BufEvent[(int)wfet].push_back(hEvent) ;
//		}
//
//		::WaitForSingleObject(hEvent,dwInterval) ;
//	}
//}


int CXCFileDataBuffer::GetRemainSpace() 
{
	CptAutoLock lcok(&m_Lock) ;

	return (m_nRefNum - m_nRefPageCount)*m_nPageSize ;
}

int CXCFileDataBuffer::GetBottomRemainSpace() 
{
	int nRet = 0 ;

	CptAutoLock lcok(&m_Lock) ;

	int nIndex = m_nCurAllocIndex ;
	int nDistance = 0 ;

	while(nIndex<m_nRefNum && m_pRefCountBuf[nIndex]>0)
	{// 找出下一个干净的页面的起始位置
		++nIndex ;
	}

	if(nIndex>=m_nRefNum)
	{
		nRet = 0 ;
	}
	else
	{
		int nIndex2 = nIndex+1 ;

		while(nIndex2<m_nRefNum && m_pRefCountBuf[nIndex2]==0)
		{// 找出下一个'不'干净的页面的起始位置
			++nIndex2 ;
		}

		nRet = (nIndex2 - nIndex)*m_nPageSize ;
	}

	return nRet ;
}
#ifdef _DEBUG
void CXCFileDataBuffer::CheckBufAlloc() 
{
	int nNonSwitchZero = 0 ; // 从 '有' 到 '空' 切换次数
	int nZeroSwitchNon = 0 ; // 从 '空' 到 '有' 切换次数

	bool bHaveZero = false ;
	bool bHaveNonZero = false ;

	for(int i=1;i<m_nRefNum;++i)
	{
		if(m_pRefCountBuf[i-1]!=m_pRefCountBuf[i])
		{
			if(m_pRefCountBuf[i])
			{
				++nZeroSwitchNon ;
			}
			else
			{
				++nNonSwitchZero ;
			}

			if(nZeroSwitchNon>=2)
			{
				_ASSERT(FALSE) ;
			}
		}

	}

}


void CXCFileDataBuffer::SaveAllocIDRecored() 
{
	HANDLE hFile = ::CreateFile(_T("x:\\AllocIDRecordBuf.txt"),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL) ;

	if(hFile!=INVALID_HANDLE_VALUE)
	{
		DWORD dwWritten ;
		
		for(int i=0;i<m_nRefNum;++i)
		{
			CptString str ;

			str.Format(_T("%04d  "),m_pAllocIDRecordBuf[i]) ;
			::WriteFile(hFile,str.c_str(),str.GetLength()*sizeof(TCHAR),&dwWritten,NULL) ;
		}
		//::WriteFile(hFile,m_pAllocIDRecordBuf,m_nRefNum*sizeof(unsigned),&dwWritten,NULL) ;
		::FlushFileBuffers(hFile) ;
		::CloseHandle(hFile) ;
	}
}
#endif

void CXCFileDataBuffer::Free(void* pBuf,int nSize) 
{
#ifdef COMPILE_TEST_PERFORMANCE
	DWORD dw = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif

	_ASSERT(m_pBlockBuf!=NULL) ;
	_ASSERT(m_pRefCountBuf!=NULL) ;
	_ASSERT(pBuf!=NULL) ;

	const int nOffset = (int)((BYTE*)pBuf - m_pBlockBuf)  ;

	_ASSERT(nOffset%m_nPageSize ? FALSE : TRUE) ; // 若偏移不为页面的倍数，那么就出问题了

	const int nIndex = nOffset/m_nPageSize ;//+ (nOffset%m_nPageSize ? 1 : 0);

	_ASSERT(m_nRefNum>nIndex) ;

	const int nPageCount = nSize/m_nPageSize + (nSize%m_nPageSize ? 1 : 0) ;

	Debug_Printf(_T("CXCFileDataBuffer::Free() free_page_count=%d remain=%d pBuf=%p nSize=%d"),nPageCount,m_nRefNum-m_nRefPageCount, pBuf, nSize) ;

	{
		CptAutoLock lcok(&m_Lock) ;

#ifdef _DEBUG
		//_ASSERT(!m_AllocSizeList.empty()) ;

		//int nTotal = 0 ;
		//int nPopCount = 0 ;

		//while(nTotal<nPageCount)
		//{
		//	_ASSERT(!m_AllocSizeList.empty()) ;
		//	nTotal += m_AllocSizeList.front() ;
		//	m_AllocSizeList.pop_front() ;
		//	++nPopCount ;
		//}
		//

		//{
		//	_ASSERT(nTotal==nPageCount) ;
		//}

#endif
		//if(m_nRefPageCount==m_nRefNum)
		//{// 当页面引用数和所有页面数目相当时，则说明缓冲区而全部被使用
//			this->CheckAndTriggerEvent(WFET_WaitForBufNotFull) ;
		//}

#ifdef _DEBUG
		// 检测分配ID是否连续，不连续则说明有问题
		{
			std::set<int> tem_set ;

			for(int i=0;i<m_nRefNum;++i)
			{
				if(m_pRefCountBuf[i]>0)
				{
					if(tem_set.find(m_pRefCountBuf[i])!=tem_set.end())
					{
						tem_set.insert(m_pRefCountBuf[i]) ;
					}
				}
			}

			if(tem_set.size()>1)
			{
				int nPre = 0 ;
				std::set<int>::iterator it = tem_set.begin() ;

				for(;it!=tem_set.end();++it)
				{
					if(nPre>0)
					{
						_ASSERT(nPre==(*it)-1) ;
					}
					nPre = (*it) ;
				}
			}
		}
#endif

		for(int i=0;i<nPageCount;++i)
		{
			_ASSERT(m_pRefCountBuf[nIndex+i]>0) ;

			if(--m_pRefCountBuf[nIndex+i]==0)
			{
				Debug_Printf(_T("free page index=%d "), nIndex + i);
				--m_nRefPageCount ;

#ifdef _DEBUG
				_ASSERT(m_pAllocIDRecordBuf[nIndex+i]>0) ;
				m_nLastFreeID = m_pAllocIDRecordBuf[nIndex+i] ;
				m_pAllocIDRecordBuf[nIndex+i]=0 ;
#endif
			}
		}

#if _DEBUG
		/**
		for(int i=0;i<m_nRefNum;++i)
		{
			if(!(m_pAllocIDRecordBuf[i]==0 || m_pAllocIDRecordBuf[i]>m_nLastFreeID)) 
			{
				this->SaveAllocIDRecored() ;
				_ASSERT(FALSE) ;
			}
		}
		/**/
#endif

#ifdef _DEBUG
		/**
		_ASSERT(m_nLastFreePageIndex==nIndex) ;

		m_nLastFreePageIndex += nPageCount ;

		if(m_nLastFreePageIndex==m_nRefNum)
		{
			m_nLastFreePageIndex = 0 ;
		}
		/**/

		//this->CheckBufAlloc() ;
#endif

		_ASSERT(!(m_nRefPageCount<0 || m_nRefPageCount>m_nRefNum)) ;

		//if(m_nRefPageCount==0)
		{// 当所有的页面都不被引用着，那么这个时候缓冲区为空
//			this->CheckAndTriggerEvent(WFET_WaitForBufEmpty) ;
		}

		_ASSERTE( _CrtCheckMemory() );
	}

#ifdef COMPILE_TEST_PERFORMANCE
	CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,10) ;
#endif
	
}


void* CXCFileDataBuffer::Allocate(BYTE nRefCount,int nSize)
{
	//Debug_Printf(_T("CXCFileDataBuffer::Allocate() begin")) ;

	_ASSERT(nSize<=m_nChunkSize) ;

	// 计算出需求的缓存空间占用多少个 "页面"
	const int nPageCount = nSize/m_nPageSize + (nSize%m_nPageSize ? 1 : 0) ;

	CptAutoLock lcok(&m_Lock) ;	
	
	//Debug_Printf(_T("CXCFileDataBuffer::Allocate() used=%d remain=%d request=%d"),m_nRefPageCount,m_nRefNum-m_nRefPageCount,nPageCount) ;

	if(m_nRefNum-m_nRefPageCount<nPageCount)
	{// 如果剩余页面数量不够分配
		return NULL ;
	}

#ifdef COMPILE_TEST_PERFORMANCE
	DWORD dw = CptPerformanceCalcator::GetInstance()->BeginCal() ;
#endif

	while(m_nCurAllocIndex<m_nRefNum && m_pRefCountBuf[m_nCurAllocIndex]>0)
	{// 找出下一个干净的页面的起始位置
		//_ASSERT(false) ;
		++m_nCurAllocIndex ;
	}

	{// 判断是否有足够的干净页面可供使用,若没有,则返回NULL
		if(m_nCurAllocIndex+nPageCount>m_nRefNum)
		{
			m_nCurAllocIndex = 0 ;
			if(m_pRefCountBuf[m_nCurAllocIndex]>0)
			{

#ifdef COMPILE_TEST_PERFORMANCE
				CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,9) ;
#endif
				return NULL ;
			}
		}

		for(int i=1;i<nPageCount;++i)
		{
			if(m_pRefCountBuf[m_nCurAllocIndex+i])
			{
#ifdef _DEBUG
				//Debug_Printf(_T("CXCFileDataBuffer::Allocate() used=%d remain=%d request=%d m_nCurAllocIndex=%d"), m_nRefPageCount, m_nRefNum - m_nRefPageCount, nPageCount, m_nCurAllocIndex);
				Debug_Printf(_T("CXCFileDataBuffer::Allocate() 5 i=%d nPageCount=%d m_pRefCountBuf[m_nCurAllocIndex+i]=%d m_pRefCountBuf[m_nCurAllocIndex+i+1]=%d m_nCurAllocIndex=%d m_nRefNum=%d m_nRefPageCount=%d m_nIDCount=%d"),
					i,nPageCount,m_pRefCountBuf[m_nCurAllocIndex+i], m_pRefCountBuf[m_nCurAllocIndex + i + 1],m_nCurAllocIndex,m_nRefNum,m_nRefPageCount,m_nIDCount) ;
#endif
				
#ifdef COMPILE_TEST_PERFORMANCE
				CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,9) ;
#endif

				return NULL ;
			}
		}
	}

	void* pRet = m_pBlockBuf + (m_nCurAllocIndex * m_nPageSize);

	//if(m_nRefPageCount==0)
	//{
//		this->CheckAndTriggerEvent(WFET_WaitForBufNotEmpty) ;
	//}
	
	// 把将要分配出去的页面相应的引用计数赋值
	//for(int i=m_nCurAllocIndex;i<nPageCount+m_nCurAllocIndex;++i)
	//{
	//	m_pRefCountBuf[i] = nRefCount ;
	//}
#ifdef _DEBUG
	++m_nIDCount ;
	for(int i=0;i<nPageCount;++i)
	{
		m_pAllocIDRecordBuf[i+m_nCurAllocIndex] = m_nIDCount ;
	}
#endif

	::memset(m_pRefCountBuf+m_nCurAllocIndex,nRefCount,nPageCount) ;

	m_nRefPageCount += nPageCount ;

	Debug_Printf(_T("alloc page. cur_alloc=%d  total_alloc=%d pBuf=%p nSize=%d"),nPageCount,m_nRefPageCount, pRet, nSize) ;

	_ASSERT(!(m_nRefPageCount<0 || m_nRefPageCount>m_nRefNum)) ;

	//if(m_nRefPageCount==m_nRefNum)
	//{
		//this->CheckAndTriggerEvent(WFET_WaitForBufFull) ;
	//}

	m_nCurAllocIndex += nPageCount ;

#ifdef _DEBUG
	//this->CheckBufAlloc() ;
#endif

	_ASSERTE( _CrtCheckMemory( ) );

	//_ASSERT(nPageCount<=4096 || nPageCount==m_nRefNum) ;

#ifdef _DEBUG	
	//m_AllocSizeList.push_back(nPageCount) ;
#endif

#ifdef COMPILE_TEST_PERFORMANCE
	CptPerformanceCalcator::GetInstance()->EndCalAndSave(dw,9) ;
#endif

	return pRet ;
}


//bool CXCFileDataBuffer::CheckAndTriggerEvent(EWaitForEventType wfet) 
//{
//	if(!m_BufEvent[(int)wfet].empty())
//	{// 若有注册了缓冲区为空的事件
//
//		pt_STL_list(HANDLE)::iterator it = m_BufEvent[(int)wfet].begin() ;
//
//		for(;it!=m_BufEvent[(int)wfet].end();++it)
//		{
//			::SetEvent((*it)) ;
//		}
//
//		m_BufEvent[(int)wfet].clear() ;
//
//		return true ;
//	}
//
//	return false ;
//}

int CXCFileDataBuffer::GetPageSize() const 
{
	return m_nPageSize ;
}