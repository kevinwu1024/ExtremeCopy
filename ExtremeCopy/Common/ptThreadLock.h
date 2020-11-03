/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include <windows.h>

class CptCritiSecLock 
{
public:
	CptCritiSecLock()
	{
		::InitializeCriticalSection(&m_csLock) ;
	}

	~CptCritiSecLock()
	{
		::DeleteCriticalSection(&m_csLock) ;
	}

	bool Lock(const int nTimeOut=0)  
	{
		::EnterCriticalSection(&m_csLock) ;
		return true ;
	}

	void Unlock()
	{
		::LeaveCriticalSection(&m_csLock) ;
	}

private:
	CRITICAL_SECTION	m_csLock ;
};

#define CptThreadLock CptCritiSecLock

class CptAutoLock  
{
public:
	CptAutoLock(const CptAutoLock& ) ;
	CptAutoLock& operator=(const CptAutoLock& ) ;

	explicit CptAutoLock(CptThreadLock* pLock):m_pLock(pLock)
	{
		if(m_pLock!=NULL)
		{
			m_pLock->Lock(0) ;
		}
	}

	~CptAutoLock()
	{
		if(m_pLock!=NULL)
		{
			m_pLock->Unlock() ;
		}
	}

private:
	CptThreadLock*		m_pLock ;
};

