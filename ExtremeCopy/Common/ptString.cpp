/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/



#include "stdafx.h"
#include "ptString.h"
#include <assert.h>
#include <stdio.h>

#define ADDRESS_NUMBER_TYPE		long long

//#define PT_DEBUG


//#ifdef PT_DEBUG
//	#include <ptDebugView.h>
//#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef _wcsinc
#define _wcsinc(_pc)    ((_pc)+1)
#endif

#ifndef _strinc
#define _strinc(_pc)    ((_pc)+1)
#endif

#pragma warning(push)
#pragma warning( disable : 4290 )
#pragma warning(disable:4996)

CptStringBase::CStrMemBuf*	CptStringBase::m_pStrMemBuf = NULL ; 
///----------------------------------------------------------------------------------------------------
// 字符串类内存池

CptStringBase::CStrMemBuf::CStrMemBuf()
{
#ifndef USE_STL_MEMORY_FOR_PTSTRING
	::memset(m_aMemList,0,sizeof(m_aMemList)) ;
	::memset(m_byUnitCount,0,sizeof(m_byUnitCount)) ;

	::InitializeCriticalSection(&m_csLock) ;
#endif

	::memset(m_EmptyBuf,0,sizeof(m_EmptyBuf)) ;

	SStringDesc* pEmptyDesc = (SStringDesc*)m_EmptyBuf ;

	pEmptyDesc->nRefs = 1 ;
	pEmptyDesc->nUsedSize = 0 ;
	pEmptyDesc->nMallocBufSize = 1 ;

#ifdef PT_DEBUG
	m_nAllocCounter = 0 ;
	m_uAllocBufSize = 0 ;
#endif
	
}

CptStringBase::CStrMemBuf::~CStrMemBuf() 
{
#ifdef PT_DEBUG
	Debug_PrintfW(_T("~CStrMemBuf() ")) ;
#endif

#ifndef USE_STL_MEMORY_FOR_PTSTRING

	::EnterCriticalSection(&m_csLock) ;

	SStringDesc* pResult = NULL ;
	
	for(int i=0; i<sizeof(m_aMemList)/sizeof(void*);++i)
	{
		while (m_aMemList[i]!=NULL)
		{
			pResult = reinterpret_cast<SStringDesc*>(m_aMemList[i]) ;
			
			m_aMemList[i] = reinterpret_cast<void*>(*reinterpret_cast<ADDRESS_NUMBER_TYPE*>(m_aMemList[i])) ;
			--m_byUnitCount[i] ;

			if(pResult!=NULL)
			{
#ifdef PT_DEBUG
				//if(pResult->nMallocBufSize==STRING_SIZE_128)
				{
					Debug_PrintfW(_T("free() 2 空间 0x%x size=%d ref=%d"),pResult,pResult->nMallocBufSize,pResult->nRefs ) ;
				}
				
				//Debug_Printf("free() 1 空间 0x%x",pResult) ;
#endif			
				::free(pResult) ;
				pResult = NULL ;
			}
		}
	}

	::LeaveCriticalSection(&m_csLock) ;
	
	::DeleteCriticalSection(&m_csLock) ;
#endif
}

void CptStringBase::CStrMemBuf::ReleaseAllRes() 
{
	unsigned uBufSize = 0 ;

	for(int i=0;i<sizeof(m_byUnitCount)/sizeof(BYTE);++i)
	{
		while(m_byUnitCount[i]>0)
		{
			SStringDesc* pDesc = (SStringDesc*)m_aMemList[i] ;

			m_aMemList[i] = reinterpret_cast<void*>(*reinterpret_cast<ADDRESS_NUMBER_TYPE*>(m_aMemList[i])) ;
			--m_byUnitCount[i] ;

#ifdef USE_STL_MEMORY_FOR_PTSTRING
			m_Alloc.deallocate(pResult,pDesc->nMallocBufSize) ;
#else
			::free(pDesc) ;
#endif
		}
	}
}

CptStringBase::SStringDesc* CptStringBase::CStrMemBuf::GetEmptyBuffer() const
{
	return (SStringDesc*)m_EmptyBuf ;
}

CptStringBase::SStringDesc* CptStringBase::CStrMemBuf::GetBuffer(const UINT uSize) 
{
	SStringDesc* pResult = NULL ;

#ifdef USE_STL_MEMORY_FOR_PTSTRING
	pResult = (SStringDesc*)m_Alloc.allocate(uSize) ;
	pResult->nMallocBufSize = uSize ;
#else
	
	int nIndex = -1 ;
	UINT uMaxSize = 0 ;

	if(uSize<STRING_SIZE_8)
	{
		nIndex = 0 ;
		uMaxSize = STRING_SIZE_8 ;
	}
	else if(uSize<STRING_SIZE_16)
	{
		nIndex = 1 ;
		uMaxSize = STRING_SIZE_16 ;
	}
	else if(uSize<STRING_SIZE_32)
	{
		nIndex = 2 ;
		uMaxSize = STRING_SIZE_32 ;
	}
	else if(uSize<STRING_SIZE_64)
	{
		nIndex = 3 ;
		uMaxSize = STRING_SIZE_64 ;
	}
	else if(uSize<STRING_SIZE_128)
	{
		nIndex = 4 ;
		uMaxSize = STRING_SIZE_128 ;
	}
	else if(uSize<STRING_SIZE_256)
	{
		nIndex = 5 ;
		uMaxSize = STRING_SIZE_256 ;
	}
	else if(uSize<STRING_SIZE_512)
	{
		nIndex = 6 ;
		uMaxSize = STRING_SIZE_512 ;
	}

	::EnterCriticalSection(&m_csLock) ;

	if(nIndex>-1 && nIndex<7)
	{
		
		if(m_aMemList[nIndex]!=NULL)
		{			
			pResult = (SStringDesc*)m_aMemList[nIndex] ;
			m_aMemList[nIndex] = reinterpret_cast<void*>(*reinterpret_cast<ADDRESS_NUMBER_TYPE*>(m_aMemList[nIndex])) ;
			--m_byUnitCount[nIndex] ;

#ifdef PT_DEBUG
			if(nIndex==4)
			{
				Debug_PrintfW(_T("摘取了空间 0x%x %d "),pResult,nIndex,pResult->nRefs) ;
			}
#endif
		}
		

		if(pResult==NULL)
		{
			switch(nIndex)
			{
			case 0: pResult = (SStringDesc*)::malloc(STRING_SIZE_8) ; break;

			case 1: pResult = (SStringDesc*)::malloc(STRING_SIZE_16) ; break;
				
			case 2: pResult = (SStringDesc*)::malloc(STRING_SIZE_32) ; break;
				
			case 3: pResult = (SStringDesc*)::malloc(STRING_SIZE_64) ; break;
				
			case 4: pResult = (SStringDesc*)::malloc(STRING_SIZE_128) ;break;
				
			case 5: pResult = (SStringDesc*)::malloc(STRING_SIZE_256) ; break;

			case 6: pResult = (SStringDesc*)::malloc(STRING_SIZE_512) ; break;
				
			default: break ;
			}
		}
		pResult->nMallocBufSize = uMaxSize ;

#ifdef PT_DEBUG
		if(nIndex==4 )
		{
			Debug_PrintfW(_T("malloc()了空间 0x%x"),pResult) ;
		}
#endif
	}
	
	if(pResult==NULL)
	{
		pResult = (SStringDesc*)::malloc(uSize) ;
		pResult->nMallocBufSize = uSize ;
	}

#ifdef PT_DEBUG
	++m_nAllocCounter ;
	m_uAllocBufSize += pResult->nMallocBufSize ;
#endif

	::LeaveCriticalSection(&m_csLock) ;
	
#endif
	return pResult ;
	
}

void CptStringBase::CStrMemBuf::ReleaseBuffer(CptStringBase::SStringDesc* pDesc) 
{
	assert(pDesc!=NULL) ;

#ifdef PT_DEBUG
	--m_nAllocCounter ;
	m_uAllocBufSize -= pDesc->nMallocBufSize ;
#endif

#ifdef USE_STL_MEMORY_FOR_PTSTRING
	m_Alloc.deallocate((char*)pDesc,pDesc->nMallocBufSize) ;
#else
	int nIndex = -1 ;

	if(pDesc->nMallocBufSize ==STRING_SIZE_16 && m_byUnitCount[0]<MAX_POOL_8)
	{
		nIndex = 0 ;
	}
	else if(pDesc->nMallocBufSize ==STRING_SIZE_16 && m_byUnitCount[1]<MAX_POOL_16)
	{
#ifdef PT_DEBUG
		//Debug_PrintfW(_T("回收空间 0x%x %d"),pDesc,STRING_SIZE_16) ;
#endif
		nIndex = 1 ;
	}
	else if(pDesc->nMallocBufSize ==STRING_SIZE_32 && m_byUnitCount[2]<MAX_POOL_32)
	{
#ifdef PT_DEBUG
		//Debug_PrintfW(_T("回收空间 0x%x %d"),pDesc,STRING_SIZE_32) ;
#endif
		nIndex = 2 ;
	}
	else if(pDesc->nMallocBufSize ==STRING_SIZE_64 && m_byUnitCount[3]<MAX_POOL_64)
	{
#ifdef PT_DEBUG
		//Debug_PrintfW(_T("回收空间 0x%x %d"),pDesc,STRING_SIZE_64) ;
#endif
		nIndex = 3 ;
	}
	else if(pDesc->nMallocBufSize ==STRING_SIZE_128 && m_byUnitCount[4]<MAX_POOL_128)
	{
#ifdef PT_DEBUG
		//Debug_PrintfW(_T("回收空间 0x%x %d ref=%d"),pDesc,STRING_SIZE_128,pDesc->nRefs) ;
#endif
		nIndex = 4 ;
	}
	else if(pDesc->nMallocBufSize ==STRING_SIZE_256 && m_byUnitCount[5]<MAX_POOL_256)
	{
#ifdef PT_DEBUG
		//Debug_PrintfW(_T("回收空间 0x%x %d"),pDesc,STRING_SIZE_256) ;
#endif
		nIndex = 5 ;
	}
	else if(pDesc->nMallocBufSize ==STRING_SIZE_512 && m_byUnitCount[6]<MAX_POOL_512)
	{
#ifdef PT_DEBUG
		//Debug_PrintfW(_T("回收空间 0x%x %d"),pDesc,STRING_SIZE_512) ;
#endif
		nIndex = 6 ;
	}

	::EnterCriticalSection(&m_csLock) ;

	if(nIndex>=0)
	{
		*(reinterpret_cast<ADDRESS_NUMBER_TYPE*>(pDesc)) = reinterpret_cast<ADDRESS_NUMBER_TYPE>(m_aMemList[nIndex]) ;
		m_aMemList[nIndex] = pDesc ;
		++m_byUnitCount[nIndex] ;
	}
	else
	{
#ifdef PT_DEBUG
		//if(pDesc->nMallocBufSize==STRING_SIZE_128)
		{
			Debug_PrintfW(_T("free() 1 空间 0x%x size=%d ref=%d"),pDesc,pDesc->nMallocBufSize,pDesc->nRefs) ;
		}
				
#endif	
		::free(pDesc) ;
	}

	::LeaveCriticalSection(&m_csLock) ;
#endif
}
//----------------------------------------------------------------------------------------
// CptStringBase

CptStringBase::CptStringBase():m_pchStrData(NULL)
{
}

void CptStringBase::ReleaseAllRes() 
{
	if(m_pStrMemBuf!=NULL)
	{
		m_pStrMemBuf->ReleaseAllRes()  ;

		delete m_pStrMemBuf ;
		m_pStrMemBuf = NULL ;
	}
}

void CptStringBase::NewMemBufInstance() 
{
	if(NULL==m_pStrMemBuf)
	{
		m_pStrMemBuf = new CStrMemBuf() ;
	}
}

void CptStringBase::SetEmptyStr() 
{
	if(m_pchStrData!=NULL && this->GetRefCount()>0)
	{
		this->DecreaseRef() ;
	}

	CptStringBase::NewMemBufInstance() ;

	m_pchStrData = (StringPointer_t*)(m_pStrMemBuf->GetEmptyBuffer())->GetStringPtr() ;
}

void CptStringBase::AllocBuf(const int nStringSize) 
{
	CptStringBase::NewMemBufInstance() ;

	if(nStringSize>0)
	{
		SStringDesc* pDesc = (SStringDesc*)(m_pStrMemBuf->GetBuffer(nStringSize+sizeof(SStringDesc)+2)) ;

		m_pchStrData = (StringPointer_t*)pDesc->GetStringPtr() ;
		::memset(m_pchStrData,0,pDesc->GetStrBufSize()) ;

		pDesc->nRefs = 1 ; // 引用 1 次
		pDesc->nUsedSize = 0 ; // 长度为0 
	}
	else
	{
		m_pchStrData = (StringPointer_t*)(m_pStrMemBuf->GetEmptyBuffer())->GetStringPtr() ;
	}


//#ifdef PT_DEBUG
//	Debug_Printf("申请了空间:0x%x,%d",this->GetStringDesc(),pDesc->nMallocBufSize) ;
//#endif

}

// 释放一个字符串内存空间
void CptStringBase::FreeBuf()
{
	if(m_pStrMemBuf!=NULL)
	{
		m_pStrMemBuf->ReleaseBuffer(this->GetStringDesc()) ;
		m_pchStrData = NULL ;
	}
}

// 减少引用
inline int CptStringBase::DecreaseRef() 
{
	_ASSERT(NULL!=m_pchStrData) ;

	if(NULL!=m_pStrMemBuf && this->GetStringDesc()!=m_pStrMemBuf->GetEmptyBuffer())
	{// 若为非空字符串
		int nResult = ::InterlockedDecrement(&(this->GetStringDesc()->nRefs)) ;

#ifdef PT_DEBUG
		//if(this->GetStringDesc()->nMallocBufSize==STRING_SIZE_128)
		//{
		//	Debug_PrintfW(_T("减少引用计数:0x%x %d"),this->GetStringDesc(),nResult) ;
		//}
		
#endif

		if(nResult==0)
		{
			this->FreeBuf() ;
		}

		return nResult;
	}

	return 1 ;
}

// 增加引用
inline int CptStringBase::IncreaseRef() const
{
	_ASSERT(NULL!=m_pchStrData) ;

	if(NULL!=m_pStrMemBuf && this->GetStringDesc()!=m_pStrMemBuf->GetEmptyBuffer())
	{// 若为非空字符串
		int nResult = ::InterlockedIncrement(&(this->GetStringDesc()->nRefs)) ;

#ifdef PT_DEBUG
		//if(this->GetStringDesc()->nMallocBufSize==STRING_SIZE_128)
		//{
		//	Debug_PrintfW(_T("增加引用计数:0x%x %d"),this->GetStringDesc(),nResult) ;
		//}
		
#endif

		return nResult ;
	}

	return 1 ;
}

inline void CptStringBase::BeforeWrite(const int nStringSize)
{
#ifdef PT_DEBUG
	//Debug_PrintfW(_T("BeforeWrite() 1:0x%x %d"),this->GetStringDesc(),nStringSize) ;
#endif

	bool bNew = true ;

	if(m_pchStrData!=NULL)
	{
		SStringDesc* pDesc = this->GetStringDesc() ;
		if(1==pDesc->nRefs && (pDesc->GetStrBufSize() > nStringSize+(int)sizeof(StringPointer_t)))
		{
			::memset(m_pchStrData,0,pDesc->GetStrBufSize()) ;
			pDesc->nUsedSize = 0 ;
			bNew = false ;
		}
	}

	if(bNew)
	{
		this->DecreaseRef() ;
		this->AllocBuf(nStringSize) ;
	}

//	Debug_Printf("BeforeWrite() 2:0x%x",this->GetStringDesc()) ;
}

inline void CptStringBase::CloneBeforeWrite()
{
	const int nOrgLen = this->GetStringDesc()->nUsedSize ;

	CptStringBase sb = *this ;
	sb.IncreaseRef() ;

	this->BeforeWrite(nOrgLen) ;
	
	::memcpy(m_pchStrData,sb.m_pchStrData,nOrgLen) ;
	this->GetStringDesc()->nUsedSize = nOrgLen ;
}

// 占用字节数
int CptStringBase::GetUsedSize() const 
{
	return this->GetStringDesc()->nUsedSize ;
}

bool CptStringBase::IsEmpty() const 
{
	return (this->GetStringDesc()->nUsedSize==0) ;
}

void CptStringBase::Empty() 
{
	this->SetEmptyStr() ;
}


//---------------------------------------------------------------------------------------------------
// CptStringA 类实现

CptStringA::CptStringA()
{
	this->SetEmptyStr() ;
}

CptStringA::CptStringA( const CptStringA& StrSrc )
{	
	StrSrc.IncreaseRef() ;
	m_pchStrData = (StringPointer_t*)StrSrc.GetStringDesc()->GetStringPtr() ;
}

CptStringA::CptStringA( char ch, int nRepeat)
{
	if (nRepeat >= 1)
	{
		this->AllocBuf(nRepeat*sizeof(char));
		this->GetStringDesc()->nUsedSize = nRepeat*sizeof(char) ;

		::memset(m_pchStrData, ch, nRepeat);
	}
	else
	{
		this->SetEmptyStr() ;
	}
}



CptStringA::CptStringA( LPCWSTR lpsz )
{
	bool bValid = false ;

	if (lpsz != NULL)
	{
		int nLenOfByte = (int)::wcslen(lpsz)*sizeof(wchar_t) ;

		if(nLenOfByte>0)
		{
			this->AllocBuf(nLenOfByte);

			::WideCharToMultiByte(CP_ACP, 0, lpsz, nLenOfByte , (char*)m_pchStrData,nLenOfByte+1, NULL, NULL);

			this->GetStringDesc()->nUsedSize = (int)::strlen((char*)m_pchStrData) ;

			bValid = true ;
		}

		_ASSERTE( _CrtCheckMemory( ) );
	}

	if(!bValid)
	{
		this->SetEmptyStr() ;
	}
}

CptStringA::CptStringA( LPCSTR lpsz )
{
	bool bValid = false ;

	if (lpsz != NULL)
	{
		int nLen = (int)::strlen(lpsz) ;

		if(nLen>0)
		{
			this->AllocBuf(nLen*sizeof(char));

			::memcpy(m_pchStrData,lpsz,nLen) ;

			this->GetStringDesc()->nUsedSize = nLen ;
			bValid = true ;
		}

		_ASSERTE( _CrtCheckMemory( ) );
	}
	
	if(!bValid)
	{
		this->SetEmptyStr() ;
	}
}

CptStringA::~CptStringA()
{
#ifdef PT_DEBUG
	Debug_PrintfW(_T("~CptStringA() 0x%x"),this->GetStringDesc()) ;
#endif
}

int CptStringA::InlineGetLength() const
{
	return (this->GetStringDesc()->nUsedSize / sizeof(char)) ;
}

int CptStringA::GetLength() const 
{
	return this->InlineGetLength() ;
}

CptStringA::operator LPCSTR() const
{
	return (const char*)m_pchStrData ;
}

const CptStringA& CptStringA::operator=(const CptStringA& StrSrc)
{
	if(StrSrc.m_pchStrData!=this->m_pchStrData)
	{
		this->DecreaseRef() ;

		SStringDesc* pDesc = StrSrc.GetStringDesc() ;
		m_pchStrData = (StringPointer_t*)pDesc->GetStringPtr() ;
		this->IncreaseRef() ;
	}

	return *this ;
}


const CptStringA& CptStringA::operator=(char ch)
{
	this->BeforeWrite(sizeof(ch)) ;
	this->GetStringDesc()->nUsedSize = sizeof(ch) ;
	((char*)m_pchStrData)[0] = ch ;

	return *this ;
}


const CptStringA& CptStringA::operator=(LPCSTR lpsz)
{
	if(lpsz!=NULL)
	{
		int nLen = (int)::strlen(lpsz) ;

		if(nLen>0)
		{
			this->BeforeWrite(nLen) ;

			this->GetStringDesc()->nUsedSize = nLen ;

			::memcpy(m_pchStrData,lpsz,nLen) ;
		}
		else
		{
			this->SetEmptyStr() ;
		}
	}

	return *this ;
}

const CptStringA& CptStringA::operator=(LPCWSTR lpsz)
{
	if(lpsz!=NULL)
	{
		int nLenOfByte = (int)::wcslen(lpsz)*sizeof(wchar_t) ;
		this->BeforeWrite(nLenOfByte) ;
		this->GetStringDesc()->nUsedSize = nLenOfByte ;

		::WideCharToMultiByte(CP_ACP, 0, lpsz, nLenOfByte/sizeof(wchar_t) , 
			(char*)m_pchStrData,this->GetStringDesc()->nMallocBufSize, NULL, NULL);

		this->GetStringDesc()->nUsedSize = (int)::strlen((char*)m_pchStrData) ;
		
	}
	
	return *this ;
}



const CptStringA& CptStringA::operator+=(const CptStringA& str)
{
	const int nOldLen = this->InlineGetLength() ;
	const int nExtraLen = str.InlineGetLength() ;
	const int nNewLen = nOldLen + nExtraLen ;

	CptStringA strTem(*this) ;

	this->BeforeWrite(nNewLen) ;

	if(m_pchStrData!=strTem.m_pchStrData)
	{
		::memcpy(m_pchStrData,strTem.m_pchStrData,nOldLen) ;
	}

	::memcpy(((char*)m_pchStrData)+nOldLen,(char*)str.m_pchStrData,nExtraLen) ;

	this->GetStringDesc()->nUsedSize = nNewLen ;

	return *this ;
}


const CptStringA& CptStringA::operator+=(char ch)
{
	CptStringA str(*this) ;
	this->BeforeWrite(sizeof(ch)+this->InlineGetLength()) ;

	::memcpy(m_pchStrData,str.m_pchStrData,str.InlineGetLength()) ;
	char* pcEnd = ((char*)m_pchStrData)+str.InlineGetLength() ;
	*((CHAR*)(pcEnd)+1) = *((CHAR*)(pcEnd)) ;
	*((CHAR*)(pcEnd)) = ch ;

	this->GetStringDesc()->nUsedSize = str.InlineGetLength() + sizeof(ch) ;

	return *this ;
}



const CptStringA& CptStringA::operator+=(LPCSTR lpsz)
{
	if(lpsz!=NULL)
	{
		CptStringA str(lpsz) ;
		*this += str ;
	}
	return *this ;
}

const CptStringA& CptStringA::operator+=(LPCWSTR lpsz)
{
	if(lpsz!=NULL)
	{
		CptStringA str(lpsz) ;
		
		*this += str ;
	}
	return *this ;
}


CptStringA operator+(const CptStringA& str1,const CptStringA& str2) 
{
	CptStringA strResult(str1) ;
	strResult += str2 ;

	return strResult ;
}


CptStringA operator+(const CptStringA& str,const char ch) 
{
	CptStringA strResult(str) ;
	strResult += ch ;

	return strResult ;
}


CptStringA operator+(const char ch,const CptStringA& str) 
{
	CptStringA strResult(ch) ;
	strResult += str ;

	return strResult ;
}



CptStringA operator+(const CptStringA& str, LPCSTR lpsz)
{
	CptStringA strResult(str) ;
	strResult += lpsz ;
	
	return strResult ;
}

CptStringA operator+(LPCSTR lpsz, const CptStringA& str)
{
	CptStringA strResult(lpsz) ;

	strResult += str ;
	
	return strResult ;
}

bool CptStringA::operator==(const CptStringA str) const 
{
	return ::strcmp((char*)str.m_pchStrData,(char*)m_pchStrData) ? false : true ;
}

bool CptStringA::operator==(LPCSTR lpsz) const 
{
	assert(lpsz!=NULL) ;
	return ::strcmp((char*)lpsz,(char*)m_pchStrData) ? false : true ;
}

bool CptStringA::operator==(char ch) const 
{
	return (this->InlineGetLength()==1 && *((char*)m_pchStrData)==ch) ;
}

bool CptStringA::operator!=(const CptStringA str) const 
{
	return !((*this)==str) ;
}

bool CptStringA::operator!=(LPCSTR lpsz) const 
{
	return !((*this)==lpsz) ;
}

bool CptStringA::operator!=(char ch) const 
{
	return !((*this)==ch) ;
}

int CptStringA::Compare(LPCSTR lpsz) const
{
	assert(lpsz!=NULL) ;
	return ::strcmp(lpsz,(char*)m_pchStrData) ;
}

int CptStringA::CompareNoCase(LPCSTR lpsz) const
{
	assert(lpsz!=NULL) ;

	CptStringA str1(*this) ;
	CptStringA str2(lpsz) ;

	str1.MakeLower() ;
	str2.MakeLower() ;

	return ::strcmp((char*)str1.m_pchStrData,(char*)str2.m_pchStrData) ;
}

void CptStringA::MakeUpper()
{
	this->CloneBeforeWrite() ;

	::_strupr((char*)m_pchStrData) ;

}

void CptStringA::MakeLower()
{
	this->CloneBeforeWrite() ;

	::_strlwr((char*)m_pchStrData) ;

}

CptStringA CptStringA::Mid(int nFirst, int nCount) const
{
	if (nFirst < 0)
		nFirst = 0;
	
	if (nCount < 0)
		nCount = 0;

	const int nCharCount = this->InlineGetLength() ;

	if (nFirst + nCount > nCharCount)
		nCount = nCharCount - nFirst;
	if (nFirst > nCharCount)
		nCount = 0;

	if (nFirst == 0 && nFirst + nCount == nCharCount)
		return *this;

	CptStringA strResult ;

	if(nCount>0)
	{
		strResult.BeforeWrite(nCount+2) ;
		::memcpy(strResult.m_pchStrData,(char*)m_pchStrData+nFirst,nCount) ;
		strResult.GetStringDesc()->nUsedSize = nCount ;
	}

	_ASSERTE( _CrtCheckMemory( ) );

	return strResult ;
}

CptStringA CptStringA::Left(int nCount) const
{
	return this->Mid(0,nCount) ;
}

CptStringA CptStringA::Right(int nCount) const
{
	return this->Mid(this->InlineGetLength()-nCount,nCount);
}

int CptStringA::Replace(char chOld, char chNew)
{
	int nCount = 0;
	
	if (chOld != chNew)
	{
		this->CloneBeforeWrite() ;

		LPSTR psz = (char*)m_pchStrData;
		LPSTR pszEnd = psz + this->InlineGetLength();
		while (psz < pszEnd)
		{
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}
			psz = _strinc(psz);
		}
	}

	_ASSERTE( _CrtCheckMemory( ) );

	return nCount;
}

int CptStringA::Replace(LPCSTR lpszOld, LPCSTR lpszNew)
{
	assert(lpszOld!=NULL) ;
	assert(lpszNew!=NULL) ;

	int nSourceLen = (int)::strlen(lpszOld);
	if (nSourceLen == 0)
		return 0;
	int nReplacementLen = (int)::strlen(lpszNew);

	int nCount = 0;
	LPSTR lpszStart = (char*)m_pchStrData;
	LPSTR lpszEnd = (char*)m_pchStrData + this->InlineGetLength();
	LPSTR lpszTarget = NULL ;
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = ::strstr(lpszStart, lpszOld)) != NULL)
		{
			nCount++;
			lpszStart = lpszTarget + nSourceLen;
		}
		lpszStart += ::strlen(lpszStart) + 1;
	}


	if (nCount > 0)
	{
		this->CloneBeforeWrite() ;

		int nOldLength = this->InlineGetLength();
		int nNewLength =  nOldLength + (nReplacementLen-nSourceLen)*nCount;
		if (this->GetStringDesc()->GetStrBufSize() < nNewLength || this->GetStringDesc()->nRefs > 1)
		{

			CptStringA str(*this) ;
			LPSTR pstr = (char*)m_pchStrData;
			this->AllocBuf(nNewLength) ;

			::memcpy(m_pchStrData, pstr, this->InlineGetLength()*sizeof(char));

		}
		
		lpszStart = (char*)m_pchStrData;
		lpszEnd = (char*)m_pchStrData + this->InlineGetLength() ;

		while (lpszStart < lpszEnd)
		{
			while ( (lpszTarget = strstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = (int)(nOldLength - (lpszTarget - (char*)m_pchStrData + nSourceLen));
				::memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen,
					nBalance * sizeof(char));
				::memcpy(lpszTarget, lpszNew, nReplacementLen*sizeof(char));
				lpszStart = lpszTarget + nReplacementLen;
				lpszStart[nBalance] = '\0';
				nOldLength += (nReplacementLen - nSourceLen);
			}
			lpszStart += ::strlen(lpszStart) + 1;
		}
		assert(((char*)m_pchStrData)[nNewLength] == '\0');
		this->GetStringDesc()->nUsedSize = nNewLength;
	}

	_ASSERTE( _CrtCheckMemory( ) );

	return nCount;
}

int CptStringA::RemoveAt(int nIndex) 
{
	_ASSERT(nIndex>=0) ;

	return this->Remove(nIndex,1) ;
}

int CptStringA::Remove(int nBegin,int nLength) 
{
	_ASSERT(nBegin>=0) ;

	const int nStrLength = this->InlineGetLength() ;

	if(nBegin+nLength>nStrLength)
	{
		return 0 ;
	}
	else if(nBegin==0 && nLength==nStrLength)
	{
		this->SetEmptyStr() ;
		return nLength;
	}

	this->CloneBeforeWrite() ;

	if(nBegin+nLength-1==nStrLength)
	{
		((char*)m_pchStrData)[nBegin] = '\0' ;
	}
	else
	{
		::memmove((char*)m_pchStrData+nBegin,(char*)m_pchStrData+nBegin+nLength,nStrLength-nLength-nBegin+1) ;
	}

	GetStringDesc()->nUsedSize -= nLength ;
	
	return nLength ;
}

int CptStringA::Remove(char chRemove)
{
	if(this->InlineGetLength()==1 && this->GetAt(0)==chRemove)
	{
		this->SetEmptyStr() ;
		return 1 ;
	}

	this->CloneBeforeWrite() ;

	LPSTR pstrSource = (char*)m_pchStrData;
	LPSTR pstrDest = (char*)m_pchStrData;
	LPSTR pstrEnd = (char*)m_pchStrData + this->InlineGetLength() ;
	
	while (pstrSource < pstrEnd)
	{
		if (*pstrSource != chRemove)
		{
			*pstrDest = *pstrSource;
			pstrDest = _strinc(pstrDest);
		}
		pstrSource = _strinc(pstrSource);
	}
	*pstrDest = '\0';
	int nCount = (int)(pstrSource - pstrDest);
	this->GetStringDesc()->nUsedSize -= nCount;
	
	_ASSERTE( _CrtCheckMemory( ) );

	return nCount;
}

int CptStringA::Delete(int nIndex, int nCount)
{
	if (nIndex < 0)
		nIndex = 0;
	int nNewLength = this->InlineGetLength();
	if (nCount > 0 && nIndex < nNewLength)
	{
		this->CloneBeforeWrite() ;

		int nBytesToCopy = nNewLength - (nIndex + nCount) + 1;
		
		::memcpy(((char*)m_pchStrData) + nIndex,
			((char*)m_pchStrData) + nIndex + nCount, nBytesToCopy * sizeof(char));
		this->GetStringDesc()->nUsedSize = nNewLength - nCount;
	}

	_ASSERTE( _CrtCheckMemory( ) );
	
	return nNewLength;
}

int CptStringA::Find(LPCSTR lpszSub) const
{
	return this->Find(lpszSub,0) ;
}

int CptStringA::Find(LPCSTR lpszSub, int nStart) const
{
	assert(lpszSub!=NULL) ;
	
	int nLength = this->InlineGetLength();
	if (nStart > nLength)
		return -1;

	LPCSTR lpsz = ::strstr((char*)m_pchStrData + nStart, lpszSub);
	
	return (lpsz == NULL) ? -1 : (int)(lpsz - (char*)m_pchStrData);
}

int CptStringA::Find(char ch, int nStart) const
{
	int nLength = this->InlineGetLength() ;
	if (nStart >= nLength)
		return -1;
	
	LPCSTR lpsz = ::strchr((char*)m_pchStrData + nStart, ch);

	return (lpsz == NULL) ? -1 : (int)(lpsz - (char*)m_pchStrData);
}

int CptStringA::Find(char ch) const
{
	return this->Find(ch,0) ;
}

int CptStringA::ReverseFind(char ch) const
{
	LPCSTR lpsz = ::strrchr((char*)m_pchStrData , (unsigned char)ch);

	return (lpsz == NULL) ? -1 : (int)(lpsz - (char*)m_pchStrData);
}

int CptStringA::FindOneOf(LPCSTR lpszCharSet) const
{
	assert(lpszCharSet!=NULL) ;

	LPCSTR lpsz = ::strtok((char*)m_pchStrData , lpszCharSet);

	return (lpsz == NULL) ? -1 : (int)(lpsz - (char*)m_pchStrData);
}

void CptStringA::Format(LPCSTR lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	this->FormatV(lpszFormat, argList);
	va_end(argList);

	_ASSERTE( _CrtCheckMemory( ) );
}

void CptStringA::FormatV(LPCSTR lpszFormat, va_list argList)
{// 以下代码来自MFC
	assert(lpszFormat!=NULL) ;

	if(lpszFormat==NULL)
	{
		return ;
	}

	va_list argListSave = argList;

	// make a guess at the maximum length of the resulting string
	int nMaxLen = 0;
	for (LPCSTR lpsz = lpszFormat; *lpsz != '\0'; lpsz = _strinc(lpsz))
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz = _strinc(lpsz)) == '%')
		{
			nMaxLen += 1;
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz = _strinc(lpsz))
		{
			// check for valid flags
			if (*lpsz == '#')
				nMaxLen += 2;   // for '0x'
			else if (*lpsz == '*')
				nWidth = va_arg(argList, int);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
				*lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = atoi(lpsz);
			for (; *lpsz != '\0' && isdigit(*lpsz); lpsz = _strinc(lpsz))
				;
		}
		assert(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz = _strinc(lpsz);

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz = _strinc(lpsz);
			}
			else
			{
				nPrecision = atoi(lpsz);
				for (; *lpsz != '\0' && isdigit(*lpsz); lpsz = _strinc(lpsz))
					;
			}
			assert(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (strncmp(lpsz, "I64", 3) == 0)
		{
			lpsz += 3;
		}
		else
		{
			switch (*lpsz)
			{
			// modifiers that affect size
			case 'h':
//				nModifier = FORCE_ANSI;
				lpsz = _strinc(lpsz);
				break;
			case 'l':
//				nModifier = FORCE_UNICODE;
				lpsz = _strinc(lpsz);
				break;

			// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz = _strinc(lpsz);
				break;
			}
		}

		// now should be on specifier
		switch (*lpsz | nModifier)
		{
		// single characters
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(argList, char);
			break;

		// strings
		case 's':
			{
				LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
					nItemLen = (int)::strlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 'S':
			{
#ifndef _UNICODE
				LPWSTR pstrNextArg = va_arg(argList, LPWSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
					nItemLen = (int)::wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
#else
				LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
					nItemLen = (int)::strlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
#endif
			}
			break;
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			if (nPrecision != 0)
				nItemLen = min(nItemLen, nPrecision);
			nItemLen = max(nItemLen, nWidth);
		}
		else
		{
			switch (*lpsz)
			{
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				
				va_arg(argList, int);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'e':
			case 'g':
			case 'G':
				va_arg(argList, double);
				nItemLen = 128;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'f':
				va_arg(argList, double);
				nItemLen = 128; // width isn't truncated
				nItemLen = max(nItemLen, 312+nPrecision);
				break;

			case 'p':
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			// no output
			case 'n':
				va_arg(argList, int*);
				break;

			default:
				assert(FALSE);  // unknown formatting option
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}

	this->BeforeWrite(nMaxLen) ;

	int nLen = ::vsprintf((char*)m_pchStrData, lpszFormat, argListSave) ;

	if(nLen>0)
	{
		this->GetStringDesc()->nUsedSize = nLen ;
	}
	else
	{
		this->SetEmptyStr() ;
	}

	va_end(argListSave);
}

void CptStringA::SetAt(int nIndex, char ch)
{
	assert(nIndex >= 0);
	assert(nIndex < this->InlineGetLength());

	this->CloneBeforeWrite() ;
	((char*)m_pchStrData)[nIndex] = ch;
}

char CptStringA::GetAt(int nIndex) const 
{
	assert(nIndex >= 0);
	assert(nIndex < this->InlineGetLength());

	return ((char*)m_pchStrData)[nIndex] ;
}

char CptStringA::operator []( int nIndex ) const
{
	assert(nIndex >= 0);
	assert(nIndex < this->InlineGetLength());

	return ((char*)m_pchStrData)[nIndex] ;
}

const char* CptStringA::c_str() const
{
	return (const char*)m_pchStrData ;
}

int CptStringA::AsInt() const throw(int) 
{
	char* It = NULL ;

	It = (char*)m_pchStrData ;

	while(*It!=0)
	{
		if((*It<'0' || *It>'9') && (*It!='.') && !((*It=='-') && (It!=(char*)m_pchStrData))) throw 0;
		It++ ;
	}

	return ::atoi((char*)m_pchStrData) ;
}

double CptStringA::AsFloat() const throw(int) 
{
	char * It = NULL ;

	It = (char*)m_pchStrData ;

	while(*It!=0)
	{
		if((*It<'0' || *It>'9') && (*It!='.') && !((*It=='-') && (It!=(char*)m_pchStrData))) throw 0;
		It++ ;
	}
	return ::atof((char*)m_pchStrData) ;
}

__int64 CptStringA::As64Int() const throw(int) 
{
	char* It = NULL ;

	It = (char*)m_pchStrData ;

	while(*It!=0)
	{
		if((*It<'0' || *It>'9') && (*It!='.') && !((*It=='-') && (It!=(char*)m_pchStrData))) throw 0;
		It++ ;
	}
	return ::_atoi64((char*)m_pchStrData) ;

}

int CptStringA::Insert(int nIndex, char ch)
{
	this->CloneBeforeWrite();

	if (nIndex < 0)
		nIndex = 0;

	int nNewLength = this->InlineGetLength();
	if (nIndex > nNewLength)
		nIndex = nNewLength;
	nNewLength++;

	if (this->GetStringDesc()->GetStrBufSize() < nNewLength)
	{
		CptStringA str(*this) ;
		const char* pstr = (char*)m_pchStrData;
		this->AllocBuf(nNewLength) ;
		::memcpy(m_pchStrData, pstr, (str.InlineGetLength()+1)*sizeof(char));
	}

	::memcpy((char*)m_pchStrData + nIndex + 1,
		(char*)m_pchStrData + nIndex, (nNewLength-nIndex)*sizeof(char));
	((char*)m_pchStrData)[nIndex] = ch;
	this->GetStringDesc()->nUsedSize = nNewLength;

	_ASSERTE( _CrtCheckMemory( ) );

	return nNewLength;
}

int CptStringA::Insert(int nIndex, LPCSTR pstr)
{
	if (nIndex < 0)
		nIndex = 0;

	int nInsertLength = (int)::strlen(pstr) ;
	int nNewLength = this->InlineGetLength();
	if (nInsertLength > 0)
	{
		this->CloneBeforeWrite() ;
		if (nIndex > nNewLength)
			nIndex = nNewLength;
		nNewLength += nInsertLength;

		if (this->GetStringDesc()->GetStrBufSize() < nNewLength)
		{
			CptStringA str(*this) ;
			const char* pstr = (char*)m_pchStrData;
			this->AllocBuf(nNewLength) ;
			memcpy(m_pchStrData, pstr, str.InlineGetLength()*sizeof(char));
		}

		::memcpy((char*)m_pchStrData + nIndex + nInsertLength,
			(char*)m_pchStrData + nIndex,
			(nNewLength-nIndex-nInsertLength+1)*sizeof(char));

		::memcpy((char*)m_pchStrData + nIndex,
			pstr, nInsertLength*sizeof(char));

		this->GetStringDesc()->nUsedSize = nNewLength;
	}

	_ASSERTE( _CrtCheckMemory( ) );
	return nNewLength;
}


int  CptStringA::TrimLeft(char c) 
{
	const int nLength = this->InlineGetLength();

	if(nLength>0 && ((char*)m_pchStrData)[0]==c)
	{
		int i=1;

		for(i=1;i<nLength;++i)
		{
			if(((char*)m_pchStrData)[i]!=c)
			{
				break ;
			}
		}

		this->Remove(0,i) ;

		return i ;
	}

	return 0 ;
}

int  CptStringA::TrimLeft(const char* szStr) 
{
	if(szStr!=NULL)
	{
		const int nLength = (const int)::strlen(szStr) ;
		const int nStrLen = this->InlineGetLength() ;

		if(nLength<nStrLen)
		{
			int nIndex = 0 ;

			while(nIndex+nLength<=nStrLen)
			{
				if(::memcmp((char*)m_pchStrData+nIndex,szStr,nLength)!=0)
				{
					break ;
				}

				nIndex += nLength ;
			}

			if(nIndex>0)
			{
				this->Remove(0,nIndex) ;

				return nIndex ;
			}
		}
	}

	return 0 ;
}

int  CptStringA::TrimRight(char c) 
{
	const int nLength = this->InlineGetLength();

	if(nLength>0 && ((char*)m_pchStrData)[nLength-1]==c)
	{
		int i=nLength-2;

		for(i=nLength-2;i>=0;--i)
		{
			if(((char*)m_pchStrData)[i]!=c)
			{
				++i;
				break ;
			}
		}

		this->Remove(i,nLength-i) ;

		return nLength-i ;
	}

	return 0 ;
}

int  CptStringA::TrimRight(const char* szStr) 
{
	if(szStr!=NULL)
	{
		const int nLength = (const int)::strlen(szStr) ;
		const int nStrLen = this->InlineGetLength() ;

		if(nLength<=nStrLen)
		{
			int nIndex = nStrLen-nLength ;

			while(nIndex>=0)
			{
				if(::memcmp((char*)m_pchStrData+nIndex,szStr,nLength)!=0)
				{
					nIndex += nLength ;
					break ;
				}

				nIndex -= nLength ;
			}

			if(nIndex<nStrLen-1)
			{
				this->Remove(nIndex,nStrLen-nIndex) ;

				return nStrLen-nIndex ;
			}
		}
	}

	return 0 ;
}

/////////////////////////////////////////////////////////////////////////////////////
/// CStringList 类

CptStringListA::CptStringListA() 
{
}

CptStringListA::CptStringListA(const CptStringListA& StrList) 
{
	m_StrVecr.assign(StrList.m_StrVecr.begin(),StrList.m_StrVecr.end()) ;
}

CptStringListA& CptStringListA::operator=(const CptStringListA& StrList) 
{
	if(this!=&StrList)
	{
		this->Clear() ;
		
		m_StrVecr.assign(StrList.m_StrVecr.begin(),StrList.m_StrVecr.end()) ;
	}

	return *this ;
}


int CptStringListA::Add(const CptStringA& str) 
{
	m_StrVecr.push_back(str) ;

	return (int)m_StrVecr.size() ;
}

int CptStringListA::GetCount() const 
{
	return (int)m_StrVecr.size() ;
}

int CptStringListA::Delete(int nIndex) 
{
	assert(nIndex>=0) ;
	assert(nIndex< static_cast<int>(m_StrVecr.size())) ;

	m_StrVecr.erase(m_StrVecr.begin()+nIndex) ;

	return (int)m_StrVecr.size() ;
}

int CptStringListA::Delete(const CptStringA& str) 
{
	int nIndex = this->Find(str) ;
	if(nIndex>=0)
	{
		return this->Delete(nIndex) ;
	}
	return this->GetCount() ;
}

int CptStringListA::Find(const CptStringA& str) const
{
	std::vector<CptStringA>::size_type i = 0 ;
	for(i=0;i<m_StrVecr.size() && m_StrVecr[i]!=str;++i) ;
	return (int)(i>=m_StrVecr.size() ? -1 : i );
}

int CptStringListA::Split(CptStringA strData,CptStringA strSep) 
{
	this->Clear() ;

	if(strData.GetLength()>0 && strSep.GetLength()>0)
	{
		CptStringA strTem ;
		int nStart = 0 ;
		int nPos = 0 ;

		while(1)
		{
			nPos = strData.Find(strSep.c_str(),nStart) ;
			if(nPos>=0)
			{
				CptStringA str = strData.Mid(nStart,nPos-nStart) ;
				this->Add(strData.Mid(nStart,nPos-nStart)) ;
				nStart = nPos + strSep.GetLength() ;
			}
			else if(nPos<0)
			{
				if(nStart!=strData.GetLength())
				{
					this->Add(strData.Mid(nStart,strData.GetLength()-nStart)) ;
				}
				
				break ;
			}
		}
	}

	return this->GetCount() ;
}

CptStringA& CptStringListA::operator[](int nIndex) 
{
	assert(nIndex>=0) ;
	assert(nIndex< static_cast<int>(m_StrVecr.size())) ;

	return m_StrVecr[nIndex] ;
}

CptStringA CptStringListA::operator[](int nIndex) const
{
	assert(nIndex>=0) ;
	assert(nIndex< static_cast<int>(m_StrVecr.size())) ;
	
	return m_StrVecr[nIndex] ;
}

void CptStringListA::Clear() 
{
	m_StrVecr.clear() ;
}

const CptStringListA CptStringListA::operator-(const CptStringListA& sl) 
{
	CptStringListA slRet = *this ;

	int nPos = 0 ;
	std::vector<CptStringA>::const_iterator it = sl.m_StrVecr.begin() ;

	for(; it!=sl.m_StrVecr.end();++it)
	{
		slRet.Delete(*it) ;
	}

	return slRet ;
}

const CptStringListA CptStringListA::operator+(const CptStringListA& sl) 
{
	CptStringListA slRet = *this ;
	
	int nPos = 0 ;
	
	for(int i=0; i<sl.GetCount();++i)
	{
		std::vector<CptStringA>::const_iterator it = sl.m_StrVecr.begin()+i ;
		if(slRet.Find(*it)==-1)
		{
			slRet.Add(*it) ;
		}
	}
	
	return slRet ;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//// CptStringW 类



CptStringW::CptStringW(void)
{
	this->SetEmptyStr() ;

	_ASSERTE( _CrtCheckMemory( ) );
}

CptStringW::CptStringW( const CptStringW& StrSrc )
{
	StrSrc.IncreaseRef() ;
	m_pchStrData = (StringPointer_t*)StrSrc.GetStringDesc()->GetStringPtr() ;

	_ASSERTE( _CrtCheckMemory( ) );
}
	
CptStringW::CptStringW( WCHAR ch, int nRepeat)
{
	if (nRepeat >= 1)
	{
		this->AllocBuf(nRepeat*sizeof(WCHAR));
		this->GetStringDesc()->nUsedSize = nRepeat*sizeof(WCHAR) ;

		for(int i=0;i<nRepeat;++i)
		{
			((WCHAR*)m_pchStrData)[i] = ch ;
		}
	}
	else
	{
		this->SetEmptyStr() ;
	}

	_ASSERTE( _CrtCheckMemory( ) );
}

CptStringW::CptStringW(LPCSTR lpsz)
{
	bool bValid = false ;
	if (lpsz != NULL)
	{
		int nNeedSize = 0 ;
		nNeedSize = ::MultiByteToWideChar(CP_ACP,0,lpsz,-1,NULL,0)*sizeof(WCHAR) ;

		if(nNeedSize>0)
		{
			this->AllocBuf(nNeedSize);

			::MultiByteToWideChar(CP_ACP,0,lpsz,-1,(WCHAR*)m_pchStrData,nNeedSize) ;

			this->GetStringDesc()->nUsedSize = nNeedSize - sizeof(WCHAR) ;

			bValid = true ;
		}
	}

	if(!bValid)
	{
		this->SetEmptyStr() ;
	}

	_ASSERTE( _CrtCheckMemory( ) );
}

CptStringW::CptStringW(LPCWSTR lpsz)
{
	bool bValid = false ;

	if (lpsz != NULL)
	{
		int nNeedSize = (int)::wcslen(lpsz)*sizeof(WCHAR) ;

		if(nNeedSize>0)
		{
			this->AllocBuf(nNeedSize) ;

			::memcpy(m_pchStrData,lpsz,nNeedSize) ;
			this->GetStringDesc()->nUsedSize = nNeedSize ;

			bValid = true ;
		}
	}

	if(!bValid)
	{
		this->SetEmptyStr() ;
	}

	_ASSERTE( _CrtCheckMemory( ) );
}

CptStringW::~CptStringW(void)
{
}

int CptStringW::InlineGetLength() const
{
	return (this->GetStringDesc()->nUsedSize / sizeof(WCHAR)) ;
}

int CptStringW::GetLength() const 
{
	return this->InlineGetLength() ;
}

CptStringW::operator LPCWSTR() const
{
	return (WCHAR*)m_pchStrData ;
}

// 赋值
const CptStringW& CptStringW::operator=(const CptStringW& StrSrc)
{
	if(StrSrc.m_pchStrData!=this->m_pchStrData)
	{
		this->DecreaseRef() ;

		SStringDesc* pDesc = StrSrc.GetStringDesc() ;
		m_pchStrData = (StringPointer_t*)pDesc->GetStringPtr() ;
		this->IncreaseRef() ;
	}

	_ASSERTE( _CrtCheckMemory( ) );
	return *this ;
}


const CptStringW& CptStringW::operator=(WCHAR ch)
{
	this->BeforeWrite(sizeof(ch)) ;
	this->GetStringDesc()->nUsedSize = sizeof(WCHAR) ;
	((WCHAR*)m_pchStrData)[0] = ch ;

	_ASSERTE( _CrtCheckMemory( ) );
	return *this ;
}

const CptStringW& CptStringW::operator=(LPCSTR lpsz)
{
	bool bValid = false ;

	if (lpsz != NULL)
	{
		int nNeedSize = 0 ;
		nNeedSize = ::MultiByteToWideChar(CP_ACP,0,lpsz,-1,NULL,0)*sizeof(WCHAR) ;

		if(nNeedSize>0)
		{
			this->BeforeWrite(nNeedSize) ;

			::MultiByteToWideChar(CP_ACP,0,lpsz,-1,(WCHAR*)m_pchStrData,nNeedSize) ;

			this->GetStringDesc()->nUsedSize = nNeedSize - sizeof(WCHAR) ;

			bValid = true ;
		}
	}

	if(!bValid)
	{
		this->SetEmptyStr() ;
	}

	_ASSERTE( _CrtCheckMemory( ) );
	return *this ;
}


const CptStringW& CptStringW::operator=(LPCWSTR lpsz)
{
	bool bValid = false ;

	if (lpsz != NULL)
	{
		int nNeedSize = (int)::wcslen(lpsz)*sizeof(WCHAR) ;

		if(nNeedSize>0)
		{
			this->BeforeWrite(nNeedSize) ;

			::memcpy(m_pchStrData,lpsz,nNeedSize) ;
			this->GetStringDesc()->nUsedSize = nNeedSize ;

			bValid = true ;
		}
	}

	if(!bValid)
	{
		this->SetEmptyStr() ;
	}

	_ASSERTE( _CrtCheckMemory( ) );

	return *this ;
}


// 串接
CptStringW operator+(const CptStringW& str1,const CptStringW& str2) 
{
	CptStringW strResult(str1) ;
	strResult += str2 ;

	return strResult ;
}


CptStringW operator+(const CptStringW& str,const WCHAR ch) 
{
		CptStringW strResult(str) ;
	strResult += ch ;

	return strResult ;
}

CptStringW operator+(const WCHAR ch,const CptStringW& str) 
{
	CptStringW strResult(ch) ;
	strResult += str ;

	return strResult ;
}


CptStringW operator+(const CptStringW& str, LPCWSTR lpsz)
{
	CptStringW strResult(str) ;

	strResult += lpsz ;
	
	return strResult ;
}

CptStringW operator+(LPCWSTR lpsz, const CptStringW& str)
{
	CptStringW strResult(lpsz) ;

	strResult += str ;
	
	return strResult ;
}


// 串接并赋值
const CptStringW& CptStringW::operator+=(const CptStringW& str)
{
	if(str.InlineGetLength()>0)
	{
		const int nOldLen = this->GetUsedSize() ;
		const int nExtraLen = str.GetUsedSize() ;
		const int nNewLen = nOldLen + nExtraLen ;

		CptStringW strTem(*this) ;

		this->BeforeWrite(nNewLen) ;

		//if(m_pchStrData!=strTem.m_pchStrData || this->GetStringDesc()->nRefs==1)
		{
			::memcpy(m_pchStrData,strTem.m_pchStrData,nOldLen) ;
		}

		::memcpy(((char*)m_pchStrData)+nOldLen,(char*)str.m_pchStrData,nExtraLen) ;

		this->GetStringDesc()->nUsedSize = nNewLen ;
	}

	_ASSERTE( _CrtCheckMemory( ) );
	return *this ;
}

const CptStringW& CptStringW::operator+=(WCHAR ch)
{
	const int nOldLen = this->GetUsedSize() ;

	CptStringW str(*this) ;

	this->BeforeWrite(sizeof(ch)+nOldLen) ;

	::memcpy(m_pchStrData,str.m_pchStrData,nOldLen) ;
	WCHAR* pcEnd = (WCHAR*)(((char*)m_pchStrData)+nOldLen) ;
	*((WCHAR*)(pcEnd)+1) = *((WCHAR*)(pcEnd)) ;
	*((WCHAR*)(pcEnd)) = ch ;

	this->GetStringDesc()->nUsedSize = nOldLen + sizeof(ch) ;

	_ASSERTE( _CrtCheckMemory( ) );

	return *this ;
}


const CptStringW& CptStringW::operator+=(LPCSTR lpsz)
{
	if(lpsz!=NULL)
	{
		CptStringW str(lpsz) ;
		*this += str ;
	}
	return *this ;
}

const CptStringW& CptStringW::operator+=(LPCWSTR lpsz)
{
	if(lpsz!=NULL)
	{
		CptStringW str(lpsz) ;
		
		*this += str ;
	}
	return *this ;
}

// 比较

bool CptStringW::operator==(const CptStringW str) const 
{
	return ::wcscmp((WCHAR*)str.m_pchStrData,(WCHAR*)m_pchStrData) ? false : true ;
}

bool CptStringW::operator==(LPCWSTR lpsz) const 
{
	assert(lpsz!=NULL) ;
	return ::wcscmp((WCHAR*)lpsz,(WCHAR*)m_pchStrData) ? false : true ;
}

bool CptStringW::operator==(WCHAR ch) const 
{
	return (this->GetUsedSize()==sizeof(WCHAR) && *((WCHAR*)m_pchStrData)==ch) ;
}

bool CptStringW::operator!=(const CptStringW str) const 
{
	return !((*this)==str) ;
}
bool CptStringW::operator!=(LPCWSTR lpsz) const 
{
	return !((*this)==lpsz) ;
}
bool CptStringW::operator!=(WCHAR ch) const 
{
	return !((*this)==ch) ;
}

int CptStringW::Compare(LPCWSTR lpsz) const
{
	assert(lpsz!=NULL) ;
	return ::wcscmp(lpsz,(WCHAR*)m_pchStrData) ;
}

int CptStringW::CompareNoCase(LPCWSTR lpsz) const
{
	assert(lpsz!=NULL) ;

	CptStringW str1(*this) ;
	CptStringW str2(lpsz) ;

	str1.MakeLower() ;
	str2.MakeLower() ;

	return ::wcscmp((WCHAR*)str1.m_pchStrData,(WCHAR*)str2.m_pchStrData) ;
}

// 
void CptStringW::MakeUpper()
{
	this->CloneBeforeWrite() ;

	::_wcsupr((WCHAR*)m_pchStrData) ;
}

void CptStringW::MakeLower()
{
	this->CloneBeforeWrite() ;

	::_wcslwr((WCHAR*)m_pchStrData) ;
}

// 提取
CptStringW CptStringW::Mid(int nFirst, int nCount) const
{
	if (nFirst < 0)
		nFirst = 0;

	if (nCount < 0)
		nCount = 0;

	const int nCharCount = this->InlineGetLength() ;

	if (nFirst + nCount > nCharCount)
		nCount = nCharCount - nFirst;
	if (nFirst > nCharCount)
		nCount = 0;

	if (nFirst == 0 && nFirst + nCount == nCharCount)
		return *this;

	CptStringW strResult ;

	if(nCount>0)
	{
		const int nStrSize = nCount*sizeof(WCHAR) ;
		strResult.BeforeWrite(nStrSize) ;
		::memcpy(strResult.m_pchStrData,(WCHAR*)m_pchStrData+nFirst,nStrSize) ;
		strResult.GetStringDesc()->nUsedSize = nStrSize ;
	}

	_ASSERTE( _CrtCheckMemory( ) );
	return strResult ;
}

CptStringW CptStringW::Left(int nCount) const
{
	return this->Mid(0,nCount) ;
}

CptStringW CptStringW::Right(int nCount) const
{
	return this->Mid(this->InlineGetLength()-nCount,nCount);
}

// 查找
int CptStringW::Replace(WCHAR chOld, WCHAR chNew)
{
	int nCount = 0;
	
	if (chOld != chNew)
	{
		this->CloneBeforeWrite() ;

		LPWSTR psz = (WCHAR*)m_pchStrData;
		LPWSTR pszEnd = psz + this->InlineGetLength();

		while (psz < pszEnd)
		{
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}

			psz = _wcsinc(psz);
		}
	}

	_ASSERTE( _CrtCheckMemory( ) );
	return nCount;
}

int CptStringW::Replace(LPCWSTR lpszOld, LPCWSTR lpszNew)
{
	assert(lpszOld!=NULL) ;
	assert(lpszNew!=NULL) ;

	int nSourceLen = (int)::wcslen(lpszOld);
	if (nSourceLen == 0)
		return 0;
	int nReplacementLen = (int)::wcslen(lpszNew);

	int nCount = 0;
	LPWSTR lpszStart = (WCHAR*)m_pchStrData;
	LPWSTR lpszEnd = (WCHAR*)m_pchStrData + this->InlineGetLength();
	LPWSTR lpszTarget = NULL ;
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = ::wcsstr(lpszStart, lpszOld)) != NULL)
		{
			nCount++;
			lpszStart = lpszTarget + nSourceLen;
		}
		lpszStart += ::wcslen(lpszStart) + 1;
	}

	if (nCount > 0)
	{
		this->CloneBeforeWrite() ;

		int nOldLength = this->InlineGetLength();
		int nNewLength =  nOldLength + (nReplacementLen-nSourceLen)*nCount;
		if (this->GetStringDesc()->GetStrBufSize() < nNewLength || this->GetStringDesc()->nRefs > 1)
		{

			CptStringW str(*this) ;
			LPWSTR pstr = (WCHAR*)m_pchStrData;
			this->AllocBuf(nNewLength) ;

			::memcpy(m_pchStrData, pstr, this->InlineGetLength()*sizeof(WCHAR));

		}
		
		lpszStart = (WCHAR*)m_pchStrData;
		lpszEnd = (WCHAR*)m_pchStrData + this->InlineGetLength() ;

		
		while (lpszStart < lpszEnd)
		{
			while ( (lpszTarget = ::wcsstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = (int)(nOldLength - ((lpszTarget - (WCHAR*)m_pchStrData) + nSourceLen));

				::memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen,
					nBalance * sizeof(WCHAR));
				::memcpy(lpszTarget, lpszNew, nReplacementLen*sizeof(WCHAR));
				lpszStart = lpszTarget + nReplacementLen;
				lpszStart[nBalance] = '\0';
				nOldLength += (nReplacementLen - nSourceLen);
			}
			lpszStart += wcslen(lpszStart) + 1;
		}
		assert(((WCHAR*)m_pchStrData)[nNewLength] == '\0');
		this->GetStringDesc()->nUsedSize = nNewLength*sizeof(WCHAR);
	}

	_ASSERTE( _CrtCheckMemory( ) );

	return nCount;
}

int CptStringW::RemoveAt(int nIndex) 
{
	_ASSERT(nIndex>=0) ;

	return this->Remove(nIndex,1) ;
}

int CptStringW::Remove(int nBegin,int nLength) 
{
	_ASSERT(nBegin>=0) ;

	const int nStrLength = this->InlineGetLength() ;

	//_ASSERT(nBegin+nLength<=nStrLength) ;

	if(nBegin+nLength>nStrLength)
	{
		return 0 ;
	}
	else if(nBegin==0 && nLength==nStrLength)
	{
		this->SetEmptyStr() ;
		return nLength;
	}

	this->CloneBeforeWrite() ;

	if(nBegin+nLength-1==nStrLength)
	{
		((WCHAR*)m_pchStrData)[nBegin] = '\0' ;
	}
	else
	{
		::memmove((WCHAR*)m_pchStrData+nBegin,(WCHAR*)m_pchStrData+nBegin+nLength,sizeof(WCHAR)*(nStrLength-nLength-nBegin+1)) ;
	}

	GetStringDesc()->nUsedSize -= nLength * sizeof(TCHAR) ;
	
	return nLength ;
}

int CptStringW::Remove(WCHAR chRemove)
{
	if(this->InlineGetLength()==1 && this->GetAt(0)==chRemove)
	{
		this->SetEmptyStr() ;
		return 1 ;
	}

	this->CloneBeforeWrite() ;

	LPWSTR pstrSource = (WCHAR*)m_pchStrData;
	LPWSTR pstrDest = (WCHAR*)m_pchStrData;
	LPWSTR pstrEnd = (WCHAR*)((char*)m_pchStrData + this->GetUsedSize()) ;
	
	while (pstrSource < pstrEnd)
	{
		if (*pstrSource != chRemove)
		{
			*pstrDest = *pstrSource;
			pstrDest = _wcsinc(pstrDest);
		}
		pstrSource = _wcsinc(pstrSource);
	}
	*pstrDest = L'\0';
	int nCount = (int)(pstrSource - pstrDest);
	this->GetStringDesc()->nUsedSize -= nCount*sizeof(WCHAR);
	
	_ASSERTE( _CrtCheckMemory( ) );

	return nCount;
}

int CptStringW::Insert(int nIndex, WCHAR ch)
{
	this->CloneBeforeWrite();

	if (nIndex < 0)
		nIndex = 0;

	int nNewLength = this->InlineGetLength();
	if (nIndex > nNewLength)
		nIndex = nNewLength;
	nNewLength++;

	if (this->GetStringDesc()->GetStrBufSize() < nNewLength)
	{
		CptStringW str(*this) ;
		const WCHAR* pstr = (WCHAR*)m_pchStrData;
		this->AllocBuf(nNewLength) ;
		::memcpy(m_pchStrData, pstr, (str.InlineGetLength()+1)*sizeof(WCHAR));
	}

	::memcpy((WCHAR*)m_pchStrData + nIndex + 1,
		(WCHAR*)m_pchStrData + nIndex, (nNewLength-nIndex)*sizeof(WCHAR));
	((WCHAR*)m_pchStrData)[nIndex] = ch;

	this->GetStringDesc()->nUsedSize = nNewLength*sizeof(WCHAR);

	_ASSERTE( _CrtCheckMemory( ) );
	return nNewLength;
}

int CptStringW::Insert(int nIndex, LPCWSTR pstr)
{
	if (nIndex < 0)
		nIndex = 0;

	int nInsertLength = (int)::wcslen(pstr) ;
	int nNewLength = this->InlineGetLength();

	if (nInsertLength > 0)
	{
		if (nIndex > nNewLength)
		{
			nIndex = nNewLength;
		}
		nNewLength += nInsertLength;

		CptStringW str(*this) ;
		this->AllocBuf(nNewLength*sizeof(TCHAR)) ;

		//if (this->GetStringDesc()->GetStrBufSize() < (int)(nNewLength*sizeof(TCHAR)))
		{
			::memcpy(m_pchStrData, str.m_pchStrData, str.InlineGetLength()*sizeof(WCHAR));
		}

		::memcpy((WCHAR*)m_pchStrData + nIndex + nInsertLength,
			(WCHAR*)m_pchStrData + nIndex,
			(nNewLength-nIndex-nInsertLength+1)*sizeof(WCHAR));

		::memcpy((WCHAR*)m_pchStrData + nIndex,
			pstr, nInsertLength*sizeof(WCHAR));

		this->GetStringDesc()->nUsedSize = nNewLength * sizeof(WCHAR);
	}

	_ASSERTE( _CrtCheckMemory( ) );

	return nNewLength;
}

int CptStringW::Delete(int nIndex, int nCount)
{
	if (nIndex < 0)
		nIndex = 0;

	int nNewLength = this->InlineGetLength();

	if (nCount > 0 && nIndex < nNewLength)
	{
		this->CloneBeforeWrite() ;

		int nBytesToCopy = nNewLength - (nIndex + nCount) + 1;

		::memcpy(((WCHAR*)m_pchStrData) + nIndex,
			((WCHAR*)m_pchStrData) + nIndex + nCount, nBytesToCopy * sizeof(WCHAR));

		this->GetStringDesc()->nUsedSize = (nNewLength - nCount) * sizeof(WCHAR);
	}

	_ASSERTE( _CrtCheckMemory( ) );

	return nNewLength;
}

int CptStringW::Find(LPCWSTR lpszSub) const
{
	return this->Find(lpszSub,0) ;
}

int CptStringW::Find(LPCWSTR lpszSub, int nStart) const
{
	assert(lpszSub!=NULL) ;
	
	int nLength = this->InlineGetLength();
	if (nStart > nLength)
		return -1;

	LPWSTR lpsz = ::wcsstr((WCHAR*)m_pchStrData + nStart, lpszSub);

	return (lpsz == NULL) ? -1 : (int)(lpsz - (WCHAR*)m_pchStrData);
}

int CptStringW::Find(WCHAR ch, int nStart) const 
{
	int nLength = this->InlineGetLength() ;
	if (nStart >= nLength)
		return -1;
	
	LPWSTR lpsz = ::wcschr((WCHAR*)m_pchStrData + nStart, (WCHAR)ch);

	return (lpsz == NULL) ? -1 : (int)(lpsz - (WCHAR*)m_pchStrData);
}

int CptStringW::Find(WCHAR ch) const
{
	return this->Find(ch,0) ;
}

int CptStringW::ReverseFind(WCHAR ch) const 
{
	LPWSTR lpsz = ::wcsrchr((WCHAR*)m_pchStrData , ch);

	return (lpsz == NULL) ? -1 : (int)(lpsz - (WCHAR*)m_pchStrData);
}

int CptStringW::FindOneOf(LPCWSTR lpszWCharSet) const 
{
	assert(lpszWCharSet!=NULL) ;
	
	LPWSTR lpsz = ::wcstok((WCHAR*)m_pchStrData , lpszWCharSet);

	return (lpsz == NULL) ? -1 : (int)(lpsz - (WCHAR*)m_pchStrData);
}


// 格式化
void CptStringW::Format(LPCWSTR lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	this->FormatV(lpszFormat, argList);
	va_end(argList);

	_ASSERTE( _CrtCheckMemory( ) );
}

void CptStringW::FormatV(LPCWSTR lpszFormat, va_list argList)
{
	assert(lpszFormat!=NULL) ;

	if(lpszFormat==NULL)
	{
		return ;
	}

	va_list argListSave = argList;

	// make a guess at the maximum length of the resulting string
	int nMaxLen = 0;
	for (LPCWSTR lpsz = lpszFormat; *lpsz != '\0'; lpsz = _wcsinc(lpsz))
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz = _wcsinc(lpsz)) == '%')
		{
			//nMaxLen += _tclen(lpsz);
			nMaxLen += 1;
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz = _wcsinc(lpsz))
		{
			// check for valid flags
			if (*lpsz == '#')
				nMaxLen += 2;   // for '0x'
			else if (*lpsz == '*')
				nWidth = va_arg(argList, int);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
				*lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = _wtoi(lpsz);
			for (; *lpsz != '\0' && iswdigit(*lpsz); lpsz = _wcsinc(lpsz))
				;
		}
		assert(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz = _wcsinc(lpsz);

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz = _wcsinc(lpsz);
			}
			else
			{
				nPrecision = _wtoi(lpsz);
				for (; *lpsz != '\0' && iswdigit(*lpsz); lpsz = _wcsinc(lpsz))
					;
			}
			assert(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (wcsncmp(lpsz, L"I64", 3) == 0)
		{
			lpsz += 3;
		}
		else
		{
			switch (*lpsz)
			{
			// modifiers that affect size
			case 'h':
//				nModifier = FORCE_ANSI;
				lpsz = _wcsinc(lpsz);
				break;
			case 'l':
//				nModifier = FORCE_UNICODE;
				lpsz = _wcsinc(lpsz);
				break;

			// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz = _wcsinc(lpsz);
				break;
			}
		}

		// now should be on specifier
		switch (*lpsz | nModifier)
		{
		// single characters
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(argList, WCHAR);
			break;

		// strings
		case 's':
			{
				LPWSTR pstrNextArg = va_arg(argList, LPWSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
					nItemLen = (int)::wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 'S':
			{
#ifndef _UNICODE
				LPWSTR pstrNextArg = va_arg(argList, LPWSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
					nItemLen = (int)::wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
#else
				LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = lstrlenA(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
#endif
			}
			break;

		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			if (nPrecision != 0)
				nItemLen = min(nItemLen, nPrecision);
			nItemLen = max(nItemLen, nWidth);
		}
		else
		{
			switch (*lpsz)
			{
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				//if (nModifier & FORCE_INT64)
				//	va_arg(argList, __int64);
				//else
					va_arg(argList, int);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'e':
			case 'g':
			case 'G':
				va_arg(argList, double);
				nItemLen = 128;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'f':
				va_arg(argList, double);
				nItemLen = 128; // width isn't truncated
				// 312 == strlen("-1+(309 zeroes).")
				// 309 zeroes == max precision of a double
				nItemLen = max(nItemLen, 312+nPrecision);
				break;

			case 'p':
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			// no output
			case 'n':
				va_arg(argList, int*);
				break;

			default:
				assert(FALSE);  // unknown formatting option
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}

	this->BeforeWrite(nMaxLen*sizeof(WCHAR)) ;
	
	int nLen = ::vswprintf((WCHAR*)m_pchStrData, lpszFormat, argListSave) ;

	if(nLen>0)
	{
		this->GetStringDesc()->nUsedSize = nLen * sizeof(WCHAR) ;
	}
	else
	{
		this->SetEmptyStr() ;
	}

	va_end(argListSave);
}

void CptStringW::SetAt(int nIndex, WCHAR ch) 
{
	assert(nIndex >= 0);
	assert(nIndex < this->InlineGetLength());

	this->CloneBeforeWrite() ;
	((WCHAR*)m_pchStrData)[nIndex] = ch;
}

WCHAR CptStringW::GetAt(int nIndex) const 
{
	assert(nIndex >= 0);
	assert(nIndex < this->InlineGetLength());

	return ((WCHAR*)m_pchStrData)[nIndex] ;
}

WCHAR CptStringW::operator []( int nIndex ) const
{
	assert(nIndex >= 0);
	assert(nIndex < this->InlineGetLength());

	return ((WCHAR*)m_pchStrData)[nIndex] ;
}

const WCHAR* CptStringW::c_str() const 
{
	return (const WCHAR*)m_pchStrData ;
}

int CptStringW::AsInt() const throw(int) 
{
#ifdef _UNICODE
	WCHAR* It = NULL ;

	It = (WCHAR*)m_pchStrData ;
#else
	char* It = NULL ;

	It = (char*)m_pchStrData ;
#endif

	while(*It!=0)
	{
		if((*It<'0' || *It>'9') && (*It!='.') && !((*It=='-') && (It!=m_pchStrData))) throw 0;
		It++ ;
	}

	return ::_wtoi((WCHAR*)m_pchStrData) ;
}

__int64 CptStringW::As64Int() const throw(int) 
{
#ifdef _UNICODE
	WCHAR* It = NULL ;

	It = (WCHAR*)m_pchStrData ;
#else
	char* It = NULL ;

	It = (char*)m_pchStrData ;
#endif

	while(*It!=0)
	{
		if((*It<'0' || *It>'9') && (*It!='.') && !((*It=='-') && (It!=m_pchStrData))) throw 0;
		It++ ;
	}
	return ::_wtoi64((WCHAR*)m_pchStrData) ;
}

double CptStringW::AsFloat() const throw(int) 
{
#ifdef _UNICODE
	WCHAR* It = NULL ;

	It = (WCHAR*)m_pchStrData ;
#else
	char* It = NULL ;

	It = (char*)m_pchStrData ;
#endif

	while(*It!=0)
	{
		if((*It<'0' || *It>'9') && (*It!='.') && !((*It=='-') && (It!=m_pchStrData))) throw 0;
		It++ ;
	}
	return ::_wtof((WCHAR*)m_pchStrData) ;
}

int CptStringW::TrimLeft(WCHAR c) 
{
	const int nLength = this->InlineGetLength();

	if(nLength>0 && ((WCHAR*)m_pchStrData)[0]==c)
	{
		int i=1;

		for(i=1;i<nLength;++i)
		{
			if(((WCHAR*)m_pchStrData)[i]!=c)
			{
				break ;
			}
		}

		this->Remove(0,i) ;

		return i ;
	}

	return 0 ;
}

int CptStringW::TrimLeft(const WCHAR* szStr) 
{
	if(szStr!=NULL)
	{
		const int nLength = (const int)::wcslen(szStr) ;
		const int nStrLen = this->InlineGetLength() ;

		if(nLength<nStrLen)
		{
			int nIndex = 0 ;

			while(nIndex+nLength<=nStrLen)
			{
				if(::memcmp((WCHAR*)m_pchStrData+nIndex,szStr,nLength*sizeof(WCHAR))!=0)
				{
					break ;
				}

				nIndex += nLength ;
			}

			if(nIndex>0)
			{
				this->Remove(0,nIndex) ;
			}

			return nIndex ;
		}
	}

	return 0 ;
}

int CptStringW::TrimRight(WCHAR c) 
{
	const int nLength = this->InlineGetLength();

	if(nLength>0 && ((WCHAR*)m_pchStrData)[nLength-1]==c)
	{
		int i=nLength-2;

		for(i=nLength-2;i>=0;--i)
		{
			if(((WCHAR*)m_pchStrData)[i]!=c)
			{
				++i ;
				break ;
			}
		}

		this->Remove(i,nLength-i) ;

		return nLength-i ;
	}

	return 0 ;
}

int CptStringW::TrimRight(const WCHAR* szStr) 
{
	if(szStr!=NULL)
	{
		const int nLength = (const int)::wcslen(szStr) ;
		const int nStrLen = this->InlineGetLength() ;

		if(nLength<=nStrLen)
		{
			int nIndex = nStrLen-nLength ;

			while(nIndex>=0)
			{
				if(::memcmp((WCHAR*)m_pchStrData+nIndex,szStr,nLength*sizeof(WCHAR))!=0)
				{
					nIndex += nLength ;
					break ;
				}

				nIndex -= nLength ;
			}

			if(nIndex<=nStrLen-1)
			{
				this->Remove(nIndex,nStrLen-nIndex) ;

				return nStrLen-nIndex ;
			}
		}
	}

	return 0 ;
}


CptStringListW::CptStringListW() 
{
}

CptStringListW::CptStringListW(const CptStringListW& StrList) 
{
	m_StrVecr.assign(StrList.m_StrVecr.begin(),StrList.m_StrVecr.end()) ;
}

CptStringListW& CptStringListW::operator=(const CptStringListW& StrList) 
{
	if(this!=&StrList)
	{
		this->Clear() ;
		
		m_StrVecr.assign(StrList.m_StrVecr.begin(),StrList.m_StrVecr.end()) ;
	}

	return *this ;
}

int CptStringListW::Add(const CptStringW& str) 
{
	m_StrVecr.push_back(str) ;

	return (int)m_StrVecr.size() ;
}

int CptStringListW::GetCount() const 
{
	return (int)m_StrVecr.size() ;
}

int CptStringListW::Delete(int nIndex) 
{
	assert(nIndex>=0) ;
	assert(nIndex< static_cast<int>(m_StrVecr.size())) ;

	m_StrVecr.erase(m_StrVecr.begin()+nIndex) ;

	return (int)m_StrVecr.size() ;
}

int CptStringListW::Delete(const CptStringW& str) 
{
	int nIndex = this->Find(str) ;
	if(nIndex>=0)
	{
		return this->Delete(nIndex) ;
	}
	return this->GetCount() ;
}

int CptStringListW::Find(const CptStringW& str) const
{
	std::vector<CptStringA>::size_type i = 0 ;
	for(i=0;i<m_StrVecr.size() && m_StrVecr[i]!=str;++i) ;
	return (int)( i>=m_StrVecr.size() ? -1 : i) ;
}

CptStringW& CptStringListW::operator[](int nIndex) 
{
	assert(nIndex>=0) ;
	assert(nIndex< static_cast<int>(m_StrVecr.size())) ;

	return m_StrVecr[nIndex] ;
}

CptStringW CptStringListW::operator[](int nIndex) const 
{
	assert(nIndex>=0) ;
	assert(nIndex< static_cast<int>(m_StrVecr.size())) ;
	
	return m_StrVecr[nIndex] ;
}

int CptStringListW::Split(CptStringW strData,CptStringW strSep) 
{
	this->Clear() ;

	if(strData.GetLength()>0 && strSep.GetLength()>0)
	{
		CptStringW strTem ;
		int nStart = 0 ;
		int nPos = 0 ;

		while(1)
		{

			nPos = strData.Find(strSep.c_str(),nStart) ;
			if(nPos>=0)
			{
				CptStringW str = strData.Mid(nStart,nPos-nStart) ;
				this->Add(strData.Mid(nStart,nPos-nStart)) ;
				nStart = nPos + strSep.GetLength() ;
			}
			else if(nPos<0)
			{
				if(nStart!=strData.GetLength())
				{
					this->Add(strData.Mid(nStart,strData.GetLength()-nStart)) ;
				}
				
				break ;
			}
		}
	}

	return this->GetCount() ;
}

void CptStringListW::Clear() 
{
	m_StrVecr.clear() ;
}

const CptStringListW CptStringListW::operator-(const CptStringListW& sl) 
{
		CptStringListW slRet = *this ;

	int nPos = 0 ;
	std::vector<CptStringW>::const_iterator it = sl.m_StrVecr.begin() ;

	for(; it!=sl.m_StrVecr.end();++it)
	{
		slRet.Delete(*it) ;
	}

	return slRet ;
}

const CptStringListW CptStringListW::operator+(const CptStringListW& sl) 
{
	CptStringListW slRet = *this ;

	int nPos = 0 ;

	for(int i=0; i<sl.GetCount();++i)
	{
		std::vector<CptStringW>::const_iterator it = sl.m_StrVecr.begin()+i ;
		if(slRet.Find(*it)==-1)
		{
			slRet.Add(*it) ;
		}
	}

	return slRet ;
}

#pragma warning(pop)