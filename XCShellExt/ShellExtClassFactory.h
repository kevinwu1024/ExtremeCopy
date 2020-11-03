/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#pragma once
#include "unknwn.h"
#include "XCShellInterface.h"
#include "Resource.h"
#include "COMObject.h"
#include "XCContextMenu.h"
#include "XCDragDropHandler.h"
#include "XCShellExt.h"
#include "../ExtremeCopy/App/ptMultipleLanguage.h"
#include "../ExtremeCopy/App/Language/XCRes_ENU/resource.h"

class CSystemObject ;

// {7AC6FD70-2E2C-4d82-9C47-02F8208FDE54}
//DEFINE_GUID(CLSID_XCContextMenuFactory, 0x7ac6fd70, 0x2e2c, 0x4d82, 0x9c, 0x47, 0x2, 0xf8, 0x20, 0x8f, 0xde, 0x54);


class CShellContextMenuFactory : public CCOMFactory
{
public:
	CShellContextMenuFactory(CSystemObject* pSysObj);
	virtual ~CShellContextMenuFactory(void);

	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);
};

class CShellDragDropHandlerFactory : public CCOMFactory
{
public:
	CShellDragDropHandlerFactory(CSystemObject* pSysObj);
	virtual ~CShellDragDropHandlerFactory(void);

	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);
};

enum EMenuItemBitmapType
{
	MIBT_App,
	MIBT_Windows
};


class CSystemObject : public CSystemObjectBase
{
public:
	CSystemObject(HMODULE hModule): CSystemObjectBase(hModule)
	{

	}

	~CSystemObject()
	{
		if(m_hAppBitmap!=NULL)
		{
			::DeleteObject(m_hAppBitmap) ;
			m_hAppBitmap = NULL ;
		}
	}

	void Initailize()
	{
		HMODULE hModule = this->GetModuleHandle();

		CptString strRes = ::GetResourceDLL(hModule) ;

		::CptMultipleLanguage::GetInstance(strRes.c_str())->LoadResourceLibary(strRes.c_str(),hModule) ;

		m_hAppBitmap = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_MENUITEM) ; //::LoadBitmap(this->GetModuleHandle(),MAKEINTRESOURCE(IDB_BITMAP_APP)) ;

		m_hWindowMenuItemBitmap = ::CptMultipleLanguage::GetInstance()->GetBitmap(IDB_BITMAP_WINDOWS) ;
		m_bVISTAVersion = ::IsVISTAOrLaterVersion() ;
		m_bUseWindowsPaste = false ;
	}

	HBITMAP GetBitmap(EMenuItemBitmapType mibt) const
	{
		switch(mibt)
		{
		case MIBT_App: return m_hAppBitmap ;
		case MIBT_Windows: return m_hWindowMenuItemBitmap ;
		}
		
		return NULL ;
	}

	bool IsVISTAOrLasterVersion() const
	{
		return m_bVISTAVersion ;
	}

public:
	SXCContextMenuExtFileInfo		m_ContextMenuFileInfo ;
	bool							m_bUseWindowsPaste ;
	//LPDATAOBJECT	m_pDataObject ;

private:
	HBITMAP		m_hAppBitmap ;
	HBITMAP		m_hWindowMenuItemBitmap ;
	bool		m_bVISTAVersion ;
};