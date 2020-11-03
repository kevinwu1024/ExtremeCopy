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
#include <shlobj.h>
#include "COMObject.h"

// {F2D1B886-1A6D-482f-BD05-847ABB62C571}
DEFINE_GUID(CLSID_XCDragDropHandler, 0xf2d1b886, 0x1a6d, 0x482f, 0xbd, 0x5, 0x84, 0x7a, 0xbb, 0x62, 0xc5, 0x71);

enum EOperateType
{
	OT_Unknown ,
	OT_Copy,
	OT_Move,
	OT_Link,
	OT_CopyLink,
};

class CSystemObject ;

class CXCDragDropHandler: public CCOMObject,public IContextMenu,public IShellExtInit
{
public:
	CXCDragDropHandler(CSystemObject* pSysObj);
	~CXCDragDropHandler(void);

	STDMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

    // IContextMenu
    //STDMETHODIMP GetCommandString(UINT, UINT, UINT*, LPSTR, UINT);
	STDMETHODIMP GetCommandString(UINT_PTR, UINT, UINT*, LPSTR, UINT);
    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
    STDMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);

	// standard interface
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);

	inline CSystemObject* GetSysObj() const {return (CSystemObject*)m_pSysObj;}

private:
	

private:
	EOperateType	m_OperateType ;
	bool			m_bDefaultItemIsLink ;
};
