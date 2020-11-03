/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#include "StdAfx.h"
#include "ShellExtClassFactory.h"


CShellContextMenuFactory::CShellContextMenuFactory(CSystemObject* pSysObj):CCOMFactory(pSysObj)//:m_nRefCount(0),m_pSysObj(pSysObj)
{
}

CShellContextMenuFactory::~CShellContextMenuFactory(void)
{
}


STDMETHODIMP CShellContextMenuFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj)
{
	*ppvObj = NULL;

	Debug_Printf("CShellContextMenuFactory::CreateInstance ") ;

	if (pUnkOuter)
	{// 不支持聚合
		return CLASS_E_NOAGGREGATION ;
	}

	CXCContextMenu* pShell = new CXCContextMenu(reinterpret_cast<CSystemObject*>(m_pSysObj)) ;

	if(pShell==NULL)
	{
		return E_OUTOFMEMORY ;
	}

	HRESULT hr = pShell->QueryInterface(riid,ppvObj) ;

	if(FAILED(hr))
	{
		delete pShell ;
		pShell = NULL ;
	}

	return hr ;
}

CShellDragDropHandlerFactory::CShellDragDropHandlerFactory(CSystemObject* pSysObj):CCOMFactory(pSysObj)
{
}

CShellDragDropHandlerFactory::~CShellDragDropHandlerFactory(void)
{
}

STDMETHODIMP CShellDragDropHandlerFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj)
{
	*ppvObj = NULL;

	Debug_Printf("CShellDragDropHandlerFactory::CreateInstance ") ;

	if (pUnkOuter)
	{// 不支持聚合
		return CLASS_E_NOAGGREGATION ;
	}

	CXCDragDropHandler* pShell = new CXCDragDropHandler(reinterpret_cast<CSystemObject*>(m_pSysObj)) ;

	if(pShell==NULL)
	{
		return E_OUTOFMEMORY ;
	}

	HRESULT hr = pShell->QueryInterface(riid,ppvObj) ;

	if(FAILED(hr))
	{
		delete pShell ;
		pShell = NULL ;
	}

	return hr ;
}

/**
STDMETHODIMP CShellContextMenuFactory::QueryInterface(REFIID riid, void **ppv)
{
	HRESULT hr = E_NOINTERFACE ;

	*ppv = NULL ;

	if(::IsEqualGUID(IID_IClassFactory,riid) || ::IsEqualGUID(IID_IUnknown,riid))
	{
		*ppv = static_cast<IClassFactory*>(this) ;
		reinterpret_cast<IUnknown*>(*ppv)->AddRef();
		hr = S_OK ;
	}

	return hr ;
}


STDMETHODIMP CShellContextMenuFactory::LockServer(BOOL fLock)
{
	HRESULT hr = S_OK ;

	Debug_Printf("CShellContextMenuFactory::LockServer() %s",fLock?"true":"false") ;

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

	return hr ;
}

STDMETHODIMP_(ULONG) CShellContextMenuFactory::AddRef()
{
	return (++m_nRefCount) ;
}

STDMETHODIMP_(ULONG) CShellContextMenuFactory::Release()
{
	//Debug_Printf("Release() count=%d",m_nRefCount) ;

	if(--m_nRefCount>0)
	{
		return m_nRefCount;
	}

	m_nRefCount = 0 ;
	delete this ;

	return 0 ;
	
}
/**/