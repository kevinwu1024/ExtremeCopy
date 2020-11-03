/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCFailedFile.h"

CXCFailedFile::CXCFailedFile():m_nMaxFailedFileCount(100),
	m_nCurFailedFileID(0)
{
}

CXCFailedFile::~CXCFailedFile() 
{
}

int CXCFailedFile::SetMaxCount(int nNewMax) 
{
	CptAutoLock lock(&m_FailedVerLock) ;

	if(nNewMax>0)
	{
		int nRet = m_nMaxFailedFileCount ;

		m_nMaxFailedFileCount = nNewMax ;

		return nRet ;
	}
	else
	{
		return nNewMax ;
	}
}

bool CXCFailedFile::IsFailedFileExist(const SFailedFileInfo& ffi) 
{
	_ASSERT(ffi.uFileID>0) ;

	CptAutoLock lock(&m_FailedVerLock) ;

	bool bRet = true ;

	pt_STL_vector(SFailedFileInfo)::const_iterator it = m_FailFileInfoVer.begin() ;

	while(it!=m_FailFileInfoVer.end() && (*it).uFileID!=ffi.uFileID)
	{
		++it ;
	}

	bRet = (it!=m_FailFileInfoVer.end()) ;

	return bRet ;
}

int CXCFailedFile::AddFailedFile(const SFailedFileInfo& ffi) 
{
	int nRet = -1 ;

	// ¸Ã failed file ÒÑ¼ÇÂ¼
	if(this->IsFailedFileExist(ffi))
	{
		return nRet ;
	}

	CptAutoLock lock(&m_FailedVerLock) ;

	const int nCount = (const int)m_FailFileInfoVer.size() ;

	_ASSERT(ffi.nIndex==nCount) ;

	if(nCount<m_nMaxFailedFileCount)
	{
		nRet = nCount ;

		m_FailFileInfoVer.push_back(ffi) ;

		this->Notify(NT_Added,ffi) ;
	}

	return nRet ;
}


//bool CXCFailedFile::ChangeFailedFileStatus(int nIndex,EFailFileStatusType NewStatus)
//{
//	CptAutoLock lock(&m_FailedVerLock) ;
//
//	if(nIndex>=0 && nIndex<(int)m_FailFileInfoVer.size() && !m_ObserverVer.empty())
//	{
//		if(m_FailFileInfoVer[nIndex].Status != NewStatus)
//		{
//			m_FailFileInfoVer[nIndex].Status = NewStatus ;
//
//			this->Notify() ;
//		}
//	}
//
//	return false ;
//}

void CXCFailedFile::Notify(const ENotifyType nt,const SFailedFileInfo& OldFfi) 
{
	for(size_t i=0;i<m_ObserverVer.size();++i)
	{
		switch(nt)
		{
		case NT_Added:
			m_ObserverVer[i]->OnFailedFile_Added(OldFfi) ;
			break ;

		case NT_Update:
			m_ObserverVer[i]->OnFailedFile_Update(OldFfi,m_FailFileInfoVer) ;
			break ;
		}
	}
}

bool CXCFailedFile::UpdateFailedFile(const SFailedFileInfo& ffi)
{
	bool bRet = false ;

	CptAutoLock lock(&m_FailedVerLock) ;

	if(ffi.nIndex>=0 && ffi.nIndex<(int)m_FailFileInfoVer.size())
	{
		bRet = true ;

		SFailedFileInfo ffi2 = m_FailFileInfoVer[ffi.nIndex] ;
		m_FailFileInfoVer[ffi.nIndex] = ffi ;

		this->Notify(NT_Update,ffi2) ;
	}

	return bRet ;
}

bool CXCFailedFile::GetFailedFileByFileID(unsigned uFileID,SFailedFileInfo& ffi)
{
	bool bRet = false ;

	CptAutoLock lock(&m_FailedVerLock) ;

	pt_STL_vector(SFailedFileInfo)::const_iterator it = m_FailFileInfoVer.begin() ;

	for(;it!=m_FailFileInfoVer.end() && !bRet;++it)
	{
		if((*it).uFileID==uFileID)
		{
			ffi = (*it) ;
			bRet = true ;
		}
	}

	return bRet ;
}

bool CXCFailedFile::GetFailedFileByIndex(int nIndex,SFailedFileInfo& ffi) 
{
	bool bRet = false ;

	CptAutoLock lock(&m_FailedVerLock) ;

	if(nIndex>=0 && nIndex<(int)m_FailFileInfoVer.size())
	{
		ffi = m_FailFileInfoVer[nIndex] ;
		bRet = true ;
	}

	return bRet ;
}

int CXCFailedFile::GetFailedFileCount()  
{
	CptAutoLock lock(&m_FailedVerLock) ;

	return (int)m_FailFileInfoVer.size() ;
}

int CXCFailedFile::AddObserver(CXCFailedFileObserver* pObserver)
{
	_ASSERT(pObserver!=NULL) ;

	CptAutoLock lock(&m_FailedVerLock) ;

	m_ObserverVer.push_back(pObserver) ;

	return (int)m_ObserverVer.size() ;
}