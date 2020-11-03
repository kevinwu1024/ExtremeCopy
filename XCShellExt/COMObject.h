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
#include <unknwn.h>

class CSystemObjectBase
{
public:
	CSystemObjectBase(HMODULE hModule)//:m_pDstFolder(NULL)
	{
		m_hModule = hModule ;
		m_nRefCount = 0 ;
	}

	int IncrementRef() {::InterlockedIncrement(&m_nRefCount) ; return m_nRefCount;}
	int DecrementRef() {::InterlockedDecrement(&m_nRefCount) ;return m_nRefCount;}
	int GetRefCount() const {return m_nRefCount;}

	bool GetDllFullName(TCHAR* pBuf,int nBufSize)
	{
		return (::GetModuleFileName(m_hModule, pBuf, nBufSize)>0) ;
	}

	HMODULE GetModuleHandle() const
	{
		return m_hModule ;
	}

private:
	LONG		m_nRefCount ;
	HMODULE		m_hModule ;
};

class CCOMObject
{
public:
	CCOMObject(CSystemObjectBase* pSysObj)
	{
		m_pSysObj = pSysObj ;
		m_nRefCount = 0 ;
	}

	virtual ~CCOMObject(void)
	{
	}

	ULONG Increment()
	{
		//Debug_Printf("CCOMObject::Increment () ref=%d",m_nRefCount+1) ;

		if(m_pSysObj!=NULL)
		{
			m_pSysObj->IncrementRef() ;
		}

		return ++m_nRefCount ;
	}

	ULONG Decrement()
	{
		//Debug_Printf("CCOMObject::Decrement () ref=%d",m_nRefCount) ;

		if(m_pSysObj!=NULL)
		{
			m_pSysObj->DecrementRef() ;
		}

		if(--m_nRefCount>0)
		{
			return m_nRefCount;
		}

		m_nRefCount = 0 ;
		delete this ;

		return 0 ;
	}

	ULONG GetRefCount() const {return m_nRefCount;}

private:
	ULONG			m_nRefCount ;

protected:
	CSystemObjectBase*	m_pSysObj ;
};

//class CCOMInterface : CCOMObject
//{
//public:
//	CCOMInterface(CSystemObjectBase* pSysObj):CCOMObject(pSysObj)
//	{
//	}
//
//	virtual ~CCOMInterface(void)
//	{
//	}
//
//	STDMETHODIMP_(ULONG) AddRef()
//	{
//		return this->Increment() ;
//	}
//
//	STDMETHODIMP_(ULONG) Release()
//	{
//		return this->Decrement() ;
//	}
//};


class CCOMFactory : public IClassFactory , public CCOMObject
{
public:
	CCOMFactory(CSystemObjectBase* pSysObj):CCOMObject(pSysObj)
	{
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		//Debug_Printf("CCOMFactory::AddRef() ref=%d",this->GetRefCount()+1) ;
		return this->Increment() ;
	}

	STDMETHODIMP_(ULONG) Release()
	{
		//Debug_Printf("CCOMFactory::Release() ref=%d",this->GetRefCount()) ;
		return this->Decrement() ;
	}

	STDMETHODIMP LockServer(BOOL fLock)
	{
		if(m_pSysObj!=NULL)
		{
			if(fLock)
			{
				m_pSysObj->IncrementRef() ;
			}
			else
			{
				m_pSysObj->DecrementRef() ;
			}
		}

		return S_OK ;
	}


	STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
	{
		HRESULT hr = E_NOINTERFACE ;

		*ppv = NULL ;

		//Debug_Printf("CCOMFactory::QueryInterface() 1") ;

		if(::IsEqualGUID(IID_IClassFactory,riid) || ::IsEqualGUID(IID_IUnknown,riid))
		{
			//Debug_Printf("CCOMFactory::QueryInterface() 2") ;

			*ppv = static_cast<IClassFactory*>(this) ;
			reinterpret_cast<IUnknown*>(*ppv)->AddRef();
			hr = S_OK ;
		}

		return hr ;
	}
};