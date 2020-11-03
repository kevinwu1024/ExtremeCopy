/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "..\Common\ptTypeDef.h"
#include <list>

#include "../Common/ptThreadLock.h"

class CXCFileDataBuffer
{
public:

	enum EWaitForEventType
	{
		WFET_WaitForBufEmpty = 0,
		WFET_WaitForBufFull = 1,
		WFET_WaitForBufNotFull = 2,
		WFET_WaitForBufNotEmpty = 3,
		WFET_END = 4
	};

	CXCFileDataBuffer(const CXCFileDataBuffer&) ;

	CXCFileDataBuffer(void);
	~CXCFileDataBuffer(void);

	CXCFileDataBuffer& operator=(const CXCFileDataBuffer&) ;

	void ResetRef() ;

	bool AllocateChunk(int nChunkSize,int nAlignSize) ;
	bool IsAllocateChunk() const ;

	void* Allocate(BYTE nRefCount,int nSize) ;
	void Free(void* pBuf,int nSize) ;

	int GetRemainSpace() ;
	int GetBottomRemainSpace() ;

	int GetPageSize() const ;
	int GetChunkSize() const ;

	bool IsEmpty()  ;
	bool IsFull()  ;

	void CheckBufAlloc() ;

	void* GetBlockAddress() const {return m_pBlockBuf;}

#ifdef _DEBUG
	void SaveAllocIDRecored() ;
#endif

private:
	void Release() ;

private:
	BYTE*	m_pBlockBuf ;	// 大内存
	int		m_nChunkSize ;	// 大内存块的大小

	BYTE*	m_pRefCountBuf ; // 相应的索引页面被引用的计数缓冲区
	int		m_nRefNum ;		// 大内存块内包含总的页面数目

	int		m_nPageSize ; // 一个页面的大小

	int		m_nCurAllocIndex ; // 当前分配到的页面索引,该指向的索引为还没分配出去的
	
	pt_STL_list(HANDLE)			m_BufEvent[(int)WFET_END] ;

	int		m_nRefPageCount ;		// 有多少个页面被当前引用着

#ifdef _DEBUG
	pt_STL_list(int)		m_AllocSizeList ;
	unsigned*				m_pAllocIDRecordBuf ;
	int						m_nIDCount ;
	unsigned						m_nLastFreeID ;
#endif
	CptCritiSecLock	m_Lock ;
};

