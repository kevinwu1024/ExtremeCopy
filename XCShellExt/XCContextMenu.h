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
#include <vector>
#include "COMObject.h"
#include "../ExtremeCopy/Common/ptTypeDef.h"

// {2D4E0551-33FE-4c58-B1BF-1277B9C511F2}
DEFINE_GUID(CLSID_XCContextMenu, 0x2d4e0551, 0x33fe, 0x4c58, 0xb1, 0xbf, 0x12, 0x77, 0xb9, 0xc5, 0x11, 0xf2);

//DEFINE_GUID(CLSID_XCContextMenu, 0x3b04f31b, 0xa13a, 0x4746, 0x90, 0xad, 0x69, 0xc1, 0xeb, 0x7b, 0x11, 0x21);

class CSystemObject ;

struct SXCContextMenuExtFileInfo
{
	
	pt_STL_vector(CptString)	m_strPreSrcFilesVer ;
	pt_STL_vector(CptString)	m_strCurSrcFilesVer ;
	CptString				m_strDstFolder ;

	
	pt_STL_vector(TCHAR*)	m_SrcFilesVer ;
	TCHAR*				m_pDstFolder ;
	bool				m_bMoveOrCopy ;
	bool				m_bExeTaskFile ;
	bool				m_bLaunchWithTaskEditor ;
	//int					m_nPasteMenuCmd ;
	TCHAR				m_szLastFileOrFolder[MAX_PATH_EX]  ;

	SXCContextMenuExtFileInfo():m_pDstFolder(NULL)
	{
		::memset(m_szLastFileOrFolder,0,sizeof(m_szLastFileOrFolder)) ;
		m_bExeTaskFile = false ;
		m_bLaunchWithTaskEditor = false ;
	}
};

class CXCContextMenu: public CCOMObject,public IShellExtInit,public IContextMenu
{
public:
	CXCContextMenu(CSystemObject* pSysObj);
	virtual ~CXCContextMenu(void);

	// IShellExtInit
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

private:
	HRESULT StoreSrcFiles(IDataObject* pDataObj) ;
	//void FreeFileBuf() ;
	//HRESULT StoreSrcFiles(IDataObject* pDataObj) ;
	//bool IsCut(IDataObject* pDataObj) ;

	//void LaunchExtremeCopy() ;
	//void GetCommandBuffer(void** ppBuf,int& nSize) ;

	bool CreateCopyMenuItem(HMENU hMenu,int indexMenu,int idCmdFirst,bool bXCOrWindows) ;
	inline CSystemObject* GetSysObj() const {return (CSystemObject*)m_pSysObj;}

	UINT	m_nWindowsPasteMenuItemID ; 
	bool	m_bIsMove ;
//private:
//	ULONG m_nRefCount ;
//	CSystemObject*		m_pSysObj ;
};
