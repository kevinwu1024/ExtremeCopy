/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCFileChangingBuffer.h"


CXCFileChangingBuffer::CXCFileChangingBuffer():m_nMaxSkipCount(100),m_nMaxRenameCount(100),
	m_nCurSkipCount(0),m_nCurRenameCount(0)
{
}

bool CXCFileChangingBuffer::IsEmpty() const
{
	return m_FileChangingMap.empty() ;
}

int CXCFileChangingBuffer::GetCount() const 
{
	return (int)m_FileChangingMap.size() ;
}

bool CXCFileChangingBuffer::GetChangingStatus(CptString strSrcFile,SFileChangingStatusResult& fcsr,bool bRemove) 
{
	bool bRet = false ;

	if(!m_FileChangingMap.empty())
	{
		strSrcFile.MakeLower() ;
		//const TCHAR* pp = strFile.c_str() ;
		pt_STL_map(std::basic_string<TCHAR>,SFileChangingStatusResult)::iterator it = m_FileChangingMap.find(strSrcFile.c_str()) ;

		if(it!=m_FileChangingMap.end())
		{
			bRet = true ;
			fcsr = (*it).second ;

			if(bRemove)
			{
				m_FileChangingMap.erase(it) ;
			}
		}
	}


	return bRet ;
}

bool CXCFileChangingBuffer::AddChangingStatus(CptString strSrcFile,const SFileChangingStatusResult& fcsr) 
{
	switch(fcsr.fct)
	{
	case FileChangingType_Skip:
		if(m_nCurSkipCount>m_nMaxSkipCount)
		{
			return false ;
		}
		++m_nCurSkipCount ;
		break ;

	case FileChangingType_Rename:
		if(m_nCurRenameCount>m_nMaxRenameCount)
		{
			return false ;
		}
		++m_nCurRenameCount ;	
		break ;

	default: _ASSERT(FALSE) ; break ;
	}

	strSrcFile.MakeLower() ;

	m_FileChangingMap[strSrcFile.c_str()] = fcsr ;

	return true ;
}


int CXCFileChangingBuffer::GetMaxCount(const EFileChangingType fct) const 
{
	int nRet = 0 ;

	switch(fct)
	{
	case FileChangingType_Skip:
		nRet = m_nMaxSkipCount ;
		break ;

	case FileChangingType_Rename:
		nRet = m_nMaxRenameCount ;
		break ;

	default: _ASSERT(FALSE) ; break ;
	}

	return nRet ;
}

int CXCFileChangingBuffer::SetMaxCount(const EFileChangingType fct,int nNewMax) 
{
	int nRet = 0 ;

	if(nNewMax>0)
	{
		switch(fct)
		{
		case FileChangingType_Skip:
			nRet = m_nMaxSkipCount ;
			m_nMaxSkipCount = nNewMax ;
			break ;

		case FileChangingType_Rename:
			nRet = m_nMaxRenameCount ;
			m_nMaxRenameCount = nNewMax ;
			break ;

		default: _ASSERT(FALSE) ; break ;
		}

	}

	return nRet ;
}
