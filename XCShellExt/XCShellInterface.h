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
#include "XCContextMenu.h"

struct SXCShellExtFileInfo
{
	std::vector<TCHAR*>	m_SrcFilesVer ;
	TCHAR*				m_pDstFolder ;
	bool				m_bMoveOrCopy ;
	int					m_nPasteMenuCmd ;
	TCHAR				m_szLastFileOrFolder[MAX_PATH_EX]  ;

	SXCShellExtFileInfo():m_pDstFolder(NULL)
	{
		::memset(m_szLastFileOrFolder,0,sizeof(m_szLastFileOrFolder)) ;
	}
};

class CSystemObject ;

class CXCShellInterface: public IContextMenu,public IShellExtInit//,public CNamePipeCB
{
public:
	CXCShellInterface(CSystemObject* pSysObj);
	virtual ~CXCShellInterface(void);

		// IShellExtInit
    STDMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

    // IContextMenu
    STDMETHODIMP GetCommandString(UINT, UINT, UINT*, LPSTR, UINT);
    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
    STDMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);

	// standard interface
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);

	//virtual void OnServerRecvData(HANDLE hPipe,void* pData,int nNumOfByte) {};
	virtual void OnNewClientConnect(HANDLE hPipe)  ;

	virtual void OnClose(HANDLE hPipe) {};

private:
	void FreeFileBuf() ;
	HRESULT StoreSrcFiles(IDataObject* pDataObj) ;
	bool IsCut(IDataObject* pDataObj) ;

	void LaunchExtremeCopy() ;
	void GetCommandBuffer(void** ppBuf,int& nSize) ;

	//void* AjustBuf(void* pBuf,int nToSize) ;

private:
	ULONG m_nRefCount ;
	CSystemObject*		m_pSysObj ;
	//HANDLE				m_hPipeNameEvent ;
	//std::vector<TCHAR*>	m_SrcFilesVer ;
	//TCHAR*				m_pDstFolder ;

	//LPDATAOBJECT		m_pDataObject ;
};
