/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCShellInterface.h"
#include "ShellExtClassFactory.h"
#include <shellapi.h>
#include <tchar.h>

#pragma warning (disable:4996)


CXCShellInterface::CXCShellInterface(CSystemObject* pSysObj)
:m_nRefCount(0),m_pSysObj(pSysObj)//,m_hPipeNameEvent(NULL)//,m_pDstFolder(NULL)//,m_pDataObject(NULL)
{
	::CoInitialize(0);
}

CXCShellInterface::~CXCShellInterface(void)
{
	//Debug_Printf("CXCShellInterface::~CXCShellInterface()") ;

	//SAFE_RELEASE(m_pDataObject) ;
	//if(m_pDataObject!=NULL)
	//{
	//	m_pDataObject->Release() ;
	//	m_pDataObject = NULL ;
	//}

	//this->FreeFileBuf() ;

	//if(m_hPipeNameEvent!=NULL)
	//{
	//	::CloseHandle(m_hPipeNameEvent) ;
	//}

	::CoUninitialize();
}

STDMETHODIMP_(ULONG) CXCShellInterface::AddRef()
{
	if(m_pSysObj!=NULL)
	{
		m_pSysObj->IncrementRef() ;
	}

	return ++m_nRefCount ;
}

STDMETHODIMP_(ULONG) CXCShellInterface::Release()
{
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

STDMETHODIMP CXCShellInterface::QueryInterface(REFIID riid, void **ppv)
{
	HRESULT hr = E_NOINTERFACE ;

	*ppv = NULL ;

	if(::IsEqualGUID(IID_IUnknown,riid) || ::IsEqualGUID(IID_IShellExtInit,riid))
	{
		*ppv = static_cast<IShellExtInit *>(this) ;
		//this->AddRef() ;
		((IShellExtInit *)(*ppv))->AddRef() ;

		hr = S_OK ;
	}
	else if(::IsEqualGUID(IID_IContextMenu,riid))
	{
		*ppv = static_cast<IContextMenu *>(this) ;

		((IContextMenu *)(*ppv))->AddRef() ;

		hr = S_OK ;
	}

	return hr ;
}

void CXCShellInterface::FreeFileBuf()
{
	for(int i=0;i<(int)m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.size();++i)
	{
		if(m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer[i]!=NULL)
		{
			delete m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer[i] ;
			m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer[i] = NULL ;
		}
	}

	m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.clear() ;
}




HRESULT CXCShellInterface::StoreSrcFiles(IDataObject* pDataObj)
{
	HRESULT hr = S_OK;

	if(pDataObj!=NULL)
	{
		STGMEDIUM	medium;
		FORMATETC	fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

		if (FAILED(pDataObj->GetData(&fe, &medium)))
		{
			return E_INVALIDARG;
		}

		HDROP hDrop = (HDROP)::GlobalLock(medium.hGlobal);

		if(hDrop!=NULL)
		{
			this->FreeFileBuf() ;
			//medium.hGlobal
			//pDataObj->EnumDAdvise
			
			//hr = S_OK ; // 设置为成功
			int nFileCount = ::DragQueryFile(hDrop,0xffffffff, NULL, 0) ;

			for(int i=0;i<nFileCount;++i)
			{
				UINT uBufSize = ::DragQueryFile(hDrop,i,NULL,0) + 4 ;

				TCHAR* pFileNameBuf = new TCHAR[uBufSize] ;

				if(pFileNameBuf!=NULL)
				{
					::memset(pFileNameBuf,0,uBufSize) ;
					::DragQueryFile(hDrop,i,pFileNameBuf,uBufSize) ;

					m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.push_back(pFileNameBuf) ;
				}
				else
				{
					this->FreeFileBuf() ;
					hr = E_FAIL;
					break ;
				}
			}

			::GlobalUnlock(medium.hGlobal) ;
			::ReleaseStgMedium(&medium) ;

			if(m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.empty() && m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder==NULL)
			{
				hr = E_FAIL;
			}
		}
	}

	return hr ;
}

bool CXCShellInterface::IsCut(IDataObject* pDataObj)
{
	bool bRet = false ; // 默认为 copy

	if(pDataObj!=NULL)
	{
		// CFSTR_PERFORMEDDROPEFFECT
			// CFSTR_PREFERREDDROPEFFECT

		CLIPFORMAT nFormat = ::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT) ;
		//0x80070057 E_INVALIDARG
		STGMEDIUM	medium2;
		FORMATETC	fe2 = { nFormat, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 

		HRESULT hr9 = pDataObj->GetData(&fe2, &medium2) ;
		//Debug_Printf("CXCShellInterface::IsCut nFormat=%d result=%x",nFormat,hr9) ;

		if (SUCCEEDED( hr9)) 
		{
			int nSize = (int)::GlobalSize(medium2.hGlobal) ;
			DWORD dwCopyOrMove = 0 ;
			//int dwCopyOrMove = (int)::GlobalLock(medium2.hGlobal);
			void* pData = ::GlobalLock(medium2.hGlobal);

			::CopyMemory(&dwCopyOrMove,pData,nSize) ;

			//ULONG uRead = 0 ; 
			//medium2.pstm->Read(&dwCopyOrMove,sizeof(dwCopyOrMove),&uRead) ;

			switch(dwCopyOrMove)
			{
			case DROPEFFECT_MOVE: 
				bRet = true ;
				//Debug_Printf("CXCShellInterface::IsCut move") ;
				break ;

			case DROPEFFECT_COPY:
				//Debug_Printf("CXCShellInterface::IsCut copy") ;
				break ;

			default:
				break ;
			}

			::GlobalUnlock(medium2.hGlobal) ;
			::ReleaseStgMedium(&medium2) ;
		}
	}
	
	return bRet ;
}

STDMETHODIMP CXCShellInterface::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID)
{
	HRESULT hr = S_OK;

	SAFE_DELETE(m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder) ;

	//m_pSysObj->m_ContextMenuFileInfo.m_nPasteMenuCmd = -1 ;

	::memset(m_pSysObj->m_ContextMenuFileInfo.m_szLastFileOrFolder,0,
		sizeof(m_pSysObj->m_ContextMenuFileInfo.m_szLastFileOrFolder)) ;

	//if(pDataObj!=NULL)
	//{
	//	STGMEDIUM	medium;
	//	FORMATETC	fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	//	if (SUCCEEDED(pDataObj->GetData(&fe, &medium)))
	//	{
	//		HDROP hDrop = (HDROP)::GlobalLock(medium.hGlobal);

	//		int nFileCount = ::DragQueryFile(hDrop,0xffffffff, NULL, 0) ;
	//		Debug_Printf("CXCShellInterface::Initialize file_count=%d",nFileCount) ;

	//		char szBuf[MAX_PATH] = {0} ;

	//		for(int i=0; i<nFileCount;++i)
	//		{
	//			szBuf[0] = 0 ;
	//			::DragQueryFileA(hDrop,i,szBuf,sizeof(szBuf)) ;
	//			Debug_Printf(szBuf) ;
	//		}

	//		::GlobalUnlock(medium.hGlobal) ;
	//		::ReleaseStgMedium(&medium) ;
	//	}
	//	
	//}

	if(pDataObj!=NULL)
	{
		STGMEDIUM	medium;
		FORMATETC	fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

		if (SUCCEEDED(pDataObj->GetData(&fe, &medium)))
		{
			HDROP hDrop = (HDROP)::GlobalLock(medium.hGlobal);

			if(hDrop!=NULL)
			{
				int nFileCount = ::DragQueryFile(hDrop,0xffffffff, NULL, 0) ;

				if(nFileCount>0)
				{
					::DragQueryFile(hDrop,0,m_pSysObj->m_ContextMenuFileInfo.m_szLastFileOrFolder,
					sizeof(m_pSysObj->m_ContextMenuFileInfo.m_szLastFileOrFolder)) ;
				}
			}

			::GlobalUnlock(medium.hGlobal) ;
			::ReleaseStgMedium(&medium) ;
		}
	}

//	SAFE_RELEASE(m_pDataObject) ;
	
	/**
	hr = this->StoreSrcFiles(pDataObj) ;

	if(FAILED(hr))
	{
		return hr ;
	}
	/**/

	if(pDataObj!=NULL)
	{
		/*m_pDataObject = pDataObj ;
		m_pDataObject->AddRef() ;*/
		{
			//Func() ;
			/**
			

			int nFormat = ::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT) ;

			IEnumFORMATETC* pFormat = NULL ;
			FORMATETC etc;

			
			if(SUCCEEDED(pDataObj->EnumFormatEtc(DATADIR_SET,&pFormat)))
			{
				TCHAR szBuf[256] = {0} ;

				//int i = 0 ;
				//ULONG uCount = 0 ; 

				//HRESULT hree = pFormat->Next(0,NULL,&uCount) ;

				//Debug_Printf("CXCShellInterface::Initialize format_count=%d result=%s",uCount,SUCCEEDED(hree)?"true":"false") ;

				//FORMATETC* pEtc = (FORMATETC*)::CoTaskMemAlloc(sizeof(FORMATETC)) ;
				FORMATETC* pEtc = &etc ;

				pFormat->Reset() ;
				while(SUCCEEDED(pFormat->Next(1,pEtc,NULL))) 
				{ 
					
					//pDataObj->QueryGetData
					szBuf[0] = 0 ;
					::GetClipboardFormatName(pEtc->cfFormat,szBuf,sizeof(szBuf)) ;
					
					Debug_Printf("CXCShellInterface::Initialize format=%d format_name=%s",pEtc->cfFormat,pEtc->cfFormat==49159 ? "null": szBuf) ;

					if(pEtc->cfFormat==49159)
					{
						break ;
					}
				}

				pFormat->Release() ;
				//::CoTaskMemFree(pEtc) ;
			}

			/**

			if(::OpenClipboard(NULL))
			{
				Debug_Printf("CXCShellInterface::Initialize OpenClipboard()") ;

				UINT uNextFormat = 0 ;

				do
				{
					uNextFormat = ::EnumClipboardFormats(uNextFormat) ;
					Debug_Printf("CXCShellInterface::Initialize clip_format=%d",uNextFormat) ; 
				}
				while(uNextFormat) ;

				int nFormat = ::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT) ;

				HANDLE hClip = ::GetClipboardData(nFormat) ;

				if(hClip!=NULL)
				{
					int nSize = ::GlobalSize(hClip) ;
					::GlobalLock(hClip);
					DWORD dwCopyOrMove = (DWORD)::GlobalLock(hClip);
					::GlobalUnlock(hClip);

					switch(dwCopyOrMove) 
					{
					case DROPEFFECT_MOVE: 
						Debug_Printf("CXCShellInterface::Initialize move") ; 
						break ;

					case DROPEFFECT_COPY:
						Debug_Printf("CXCShellInterface::Initialize copy") ;
						break ;

					default:
						Debug_Printf("CXCShellInterface::Initialize unknow operate=%d size=%d",dwCopyOrMove,nSize) ;
						break ;
					}
				}
				else
				{
					Debug_Printf("CXCShellInterface::Initialize last_error=%d",::GetLastError()) ;
				}

				//::CloseClipboard() ;
			}
			
			/**
 
			CLIPFORMAT nFormat = ::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT) ;
			//0x80070057 E_INVALIDARG
			STGMEDIUM	medium2;
			FORMATETC	fe2 = { nFormat, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 

			HRESULT hr9 = pDataObj->GetData(&fe2, &medium2) ;
			Debug_Printf("CXCShellInterface::Initialize nFormat=%d result=%x",nFormat,hr9) ;

			if (SUCCEEDED( hr9)) 
			{
				DWORD dwCopyOrMove = (DWORD)::GlobalLock(medium2.hGlobal);

				ULONG uRead = 0 ; 

				//medium2.pstm->Read(&dwCopyOrMove,sizeof(dwCopyOrMove),&uRead) ;

				switch(dwCopyOrMove)
				{
				case DROPEFFECT_MOVE: 
					Debug_Printf("CXCShellInterface::Initialize move") ;
					break ;

				case DROPEFFECT_COPY:
					Debug_Printf("CXCShellInterface::Initialize copy") ;
					break ;

				default:
					Debug_Printf("CXCShellInterface::Initialize unknow operate=%d",dwCopyOrMove) ;
					break ;
				}

				//::GlobalUnlock(medium.hGlobal) ;
				::ReleaseStgMedium(&medium2) ;
			}
			/**/
		}
	}

	//if(hr==S_OK && pidlFolder!=NULL && !m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.empty())
	if(pidlFolder!=NULL)
	{
		m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder = new TCHAR[MAX_PATH_EX] ;
		if(m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder!=NULL)
		{
			::SHGetPathFromIDList(pidlFolder,m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder) ;
		}
		else
		{
			this->FreeFileBuf() ;
			hr = E_FAIL;
		}
	}

	Debug_Printf("CXCShellInterface::Initialize %s",hr==S_OK?"OK":"FAIL") ;

	return hr ;
}

STDMETHODIMP CXCShellInterface::GetCommandString(UINT idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT ccMax)
{
	Debug_Printf("CXCShellInterface::GetCommandString 1") ;

	/**
	if ((uFlags & GCS_HELPTEXT) && (uFlags & GCS_UNICODE)) 
	{
		//BOOL	is_dd = m_pDstFolder!=NULL;

		Debug_Printf("CXCShellInterface::GetCommandString 2") ;
		switch (idCmd) 
		{
		case 0:
			Debug_Printf("CXCShellInterface::GetCommandString 3") ;
			lstrcpynW ( (LPWSTR) pszName, L"Extreme Copy", ccMax );
			//strncpy(pszName, (m_pDstFolder!=NULL) ? GetLoadStr(IDS_DDCOPY) : GetLoadStr(IDS_RIGHTCOPY), cchMax);
			return	S_OK; 
		//case 1:
		//	//strncpy(pszName, (m_pDstFolder!=NULL) ? GetLoadStr(IDS_DDMOVE) : GetLoadStr(IDS_RIGHTDEL), cchMax);
		//	return	S_OK;
		case 2:
			Debug_Printf("CXCShellInterface::GetCommandString 4") ;
			lstrcpynW ( (LPWSTR) pszName, L"Extreme Paste", ccMax );
			//strncpy(pszName, GetLoadStr(IDS_RIGHTPASTE), cchMax);
			return	S_OK;
		}
	}

	Debug_Printf("CXCShellInterface::GetCommandString 5") ;
	/**/
	return E_INVALIDARG;
}

void CXCShellInterface::OnNewClientConnect(HANDLE hPipe)
{
	/* 命令格式
	command: xcopy/xcmove
	source size: 3
	c:\abc.txt
	d:\my_folder
	e:\files.cpp
	destination size: 1
	f:\target_folder

	*/

	if(m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder!=NULL && !m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.empty())
	{
		DWORD dwWritten = 0 ;
		TCHAR DataBuf[MAX_PATH_EX] = {0} ;

		::_stprintf(DataBuf,_T("command: %s\r\n"),m_pSysObj->m_ContextMenuFileInfo.m_bMoveOrCopy?_T("xcmove"):_T("xcmove")) ;
		::WriteFile(hPipe,DataBuf,(DWORD)(::_tcslen(DataBuf)*sizeof(TCHAR)),&dwWritten,NULL);

		::_stprintf(DataBuf,_T("source size: %d\r\n"),m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.size()) ;
		::WriteFile(hPipe,DataBuf,(DWORD)(::_tcslen(DataBuf)*sizeof(TCHAR)),&dwWritten,NULL);

		for(int i=0;i<(int)m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.size();++i)
		{
			::_stprintf(DataBuf,_T("%s\r\n"),m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer[i]) ;
			::WriteFile(hPipe,DataBuf,(DWORD)(::_tcslen(DataBuf)*sizeof(TCHAR)),&dwWritten,NULL);
		}

		::_stprintf(DataBuf,_T("destination size: %d\r\n"),1) ;
		::WriteFile(hPipe,DataBuf,(DWORD)(::_tcslen(DataBuf)*sizeof(TCHAR)),&dwWritten,NULL);

		// 注意这里是2个 回车换行
		::_stprintf(DataBuf,_T("%s\r\n\r\n"),m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder) ;
		::WriteFile(hPipe,DataBuf,(DWORD)(::_tcslen(DataBuf)*sizeof(TCHAR)),&dwWritten,NULL);
	}
	

	::CloseHandle(hPipe) ;
}

//void WriteToFile(const void* pBuf,int nSize,const char* pFileName=NULL)
//{
//
//	HANDLE hFile = ::CreateFile(_T("c:\\xc\\shell.txt"),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL) ;
//
//	if(hFile!=INVALID_HANDLE_VALUE)
//	{
//		DWORD dwWritten = 0 ;
//		::WriteFile(hFile,pBuf,nSize,&dwWritten,NULL) ;
//		::CloseHandle(hFile) ;
//	}
//}

void CXCShellInterface::GetCommandBuffer(void** ppBuf,int& nSize)
{
	*ppBuf = NULL ;
	nSize = 0 ;

	if(m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder!=NULL && !m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.empty())
	{
		/* 命令格式
	command: xcopy/xcmove
	source size: 3
	c:\abc.txt
	d:\my_folder
	e:\files.cpp
	destination size: 1
	f:\target_folder

	*/

		int nWholeBufSize = 4*1024*1024*sizeof(TCHAR) ;
		*ppBuf = ::malloc(nWholeBufSize);//new TCHAR[nWholeBufSize] ;

		if(*ppBuf!=NULL)
		{
			::memset(*ppBuf,0,nWholeBufSize) ;

			DWORD dwWritten = 0 ;
			TCHAR DataBuf[MAX_PATH_EX] = {0} ;
			TCHAR* pWrite = (TCHAR*)*ppBuf ;

			::_stprintf(pWrite,_T("command: %s\r\n"),m_pSysObj->m_ContextMenuFileInfo.m_bMoveOrCopy?_T("xcmove"):_T("xccopy")) ;
			pWrite += ::_tcslen(pWrite) ;
			//::WriteFile(hPipe,DataBuf,::_tcslen(DataBuf)*sizeof(TCHAR),&dwWritten,NULL);

			::_stprintf(pWrite,_T("source size: %d\r\n"),m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.size()) ;
			pWrite += ::_tcslen(pWrite) ;
			//::WriteFile(hPipe,DataBuf,::_tcslen(DataBuf)*sizeof(TCHAR),&dwWritten,NULL);

			for(int i=0;i<(int)m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.size();++i)
			{
				if(pWrite-(TCHAR*)(*ppBuf)>nWholeBufSize-256)
				{
					void* p = ::realloc(*ppBuf,2*nWholeBufSize) ;

					if(p!=NULL)
					{
						*ppBuf = p ;
						nWholeBufSize = 2*nWholeBufSize ;
					}
					else
					{
						::free(*ppBuf) ;
						*ppBuf = NULL ;

						return ;
					}
				}

				::_stprintf(pWrite,_T("%s\r\n"),m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer[i]) ;
				pWrite += ::_tcslen(pWrite) ;

				//WriteToFile(m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer[i],::_tcslen(m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer[i])*sizeof(TCHAR)) ;
				//::WriteFile(hPipe,DataBuf,::_tcslen(DataBuf)*sizeof(TCHAR),&dwWritten,NULL);
			}

			if(pWrite-(TCHAR*)(*ppBuf)>nWholeBufSize-256)
			{
				void* p = ::realloc(*ppBuf,2*nWholeBufSize) ;

				if(p!=NULL)
				{
					*ppBuf = p ;
					nWholeBufSize = 2*nWholeBufSize ;
				}
				else
				{
					::free(*ppBuf) ;
					*ppBuf = NULL ;

					return ;
				}
			}
			::_stprintf(pWrite,_T("destination size: %d\r\n"),1) ;
			pWrite += ::_tcslen(pWrite) ;
			//::WriteFile(hPipe,DataBuf,::_tcslen(DataBuf)*sizeof(TCHAR),&dwWritten,NULL);

			if(pWrite-(TCHAR*)(*ppBuf)>nWholeBufSize-256)
			{
				void* p = ::realloc(*ppBuf,nWholeBufSize+MAX_PATH) ;

				if(p!=NULL)
				{
					*ppBuf = p ;
					nWholeBufSize = nWholeBufSize+MAX_PATH ;
				}
				else
				{
					::free(*ppBuf) ;
					*ppBuf = NULL ;

					return ;
				}
			}
			// 注意这里是2个 回车换行
			::_stprintf(pWrite,_T("%s\r\n\r\n"),m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder) ;
			pWrite += ::_tcslen(pWrite) ;
			//::WriteFile(hPipe,DataBuf,::_tcslen(DataBuf)*sizeof(TCHAR),&dwWritten,NULL);
		}
		
	}
}

void CXCShellInterface::LaunchExtremeCopy() 
{
	TCHAR szDLLBuf[MAX_PATH_EX] = {0} ;

	//Debug_Printf("CXCShellInterface::LaunchExtremeCopy()") ;

	if(m_pSysObj->GetDllFullName(szDLLBuf,sizeof(szDLLBuf)))
	{

		TCHAR szEXEBuf[MAX_PATH_EX] = {0} ;
		TCHAR* pFileName =  NULL ;
		::GetFullPathName(szDLLBuf,sizeof(szEXEBuf)/sizeof(TCHAR),szEXEBuf,&pFileName) ;

		if(pFileName!=NULL)
		{/**/
			//Debug_Printf("CXCShellInterface::LaunchExtremeCopy() 2") ;

			::_tcscpy(pFileName,_T("ExtremeCopy.exe")) ;


			TCHAR* pCmd = NULL ;
			int nSize = 0 ;

			this->GetCommandBuffer((void**)&pCmd,nSize) ; // 获取命令文本
			
			nSize = (int)((::_tcslen(pCmd))*sizeof(TCHAR)) ;

			HANDLE hWrite = INVALID_HANDLE_VALUE ;
			HANDLE hRead = INVALID_HANDLE_VALUE ;
			SECURITY_ATTRIBUTES sa ;

			sa.nLength = sizeof(SECURITY_ATTRIBUTES) ;
			sa.bInheritHandle = TRUE ;
			sa.lpSecurityDescriptor = NULL ;
			 
			::CreatePipe(&hRead,&hWrite,&sa,nSize) ;
			//::DuplicateHandle(::GetCurrentProcess(), hWrite, ::GetCurrentProcess(), &hWrite, 0, FALSE, DUPLICATE_CLOSE_SOURCE|DUPLICATE_SAME_ACCESS);

			PROCESS_INFORMATION pi ;
			STARTUPINFO si ;

			::memset(&si,0,sizeof(si)) ;
			si.cb = sizeof(si) ;
			si.dwFlags = STARTF_USESTDHANDLES ;
			si.hStdInput = hRead ;
			si.hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE) ;
			si.hStdError = ::GetStdHandle(STD_ERROR_HANDLE) ;

			if(::CreateProcess(szEXEBuf,_T(" -shell"),NULL,NULL,TRUE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&si,&pi))
			{
				DWORD dwWritten = 0 ;

				BOOL b = ::WriteFile(hWrite,(void*)pCmd,nSize,&dwWritten,NULL) ;

				if(b && m_pSysObj->m_ContextMenuFileInfo.m_bMoveOrCopy && ::OpenClipboard(NULL))
				{
					::EmptyClipboard() ; 
					::CloseClipboard() ;
				}

				//Debug_Printf("CXCShellInterface::LaunchExtremeCopy() 3 nSize=%d Written=%d write=%s",nSize,dwWritten,b?"true":"false") ;
			}

			::CloseHandle(hWrite) ;


/**
			if(::OpenClipboard(NULL))
			{
				Debug_Printf("CXCShellInterface::LaunchExtremeCopy() 3") ;

				if(pCmd!=NULL)
				{
					Debug_Printf("CXCShellInterface::LaunchExtremeCopy() 4") ;

					HANDLE hMem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,(::_tcslen(pCmd))*sizeof(TCHAR)) ;

					if(hMem!=NULL)
					{
						Debug_Printf("CXCShellInterface::LaunchExtremeCopy() 5") ;

						int nFormat = ::RegisterClipboardFormat(_T("MaiHua XetremCopy Cmd")) ;

						TCHAR* p = (TCHAR*)::GlobalLock(hMem) ;

						::_tcscpy(p,pCmd) ;

						//::EmptyClipboard() ;
						//if(::SetClipboardData(CF_TEXT,hMem) !=NULL)
						if(::SetClipboardData(nFormat,hMem) !=NULL)
						{
							Debug_Printf("CXCShellInterface::LaunchExtremeCopy() OK") ; 

							PROCESS_INFORMATION pi ;
							STARTUPINFO si ;

							::memset(&si,0,sizeof(si)) ;
							si.cb = sizeof(si) ;

							::CreateProcess(szEXEBuf,_T(" -shell"),NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&si,&pi) ;

							if(m_pSysObj->m_ContextMenuFileInfo.m_bMoveOrCopy)
							{
								if(::OpenClipboard(NULL))
								{
									::EmptyClipboard() ;
									::CloseClipboard() ;
								}
							}
							
						}

						::GlobalUnlock(hMem) ;
					}

					::free(pCmd) ;
					pCmd = NULL ;
					
				}
				
				::CloseClipboard() ; 
			}
			/**
			if(m_hPipeNameEvent!=NULL)
			{
				::CloseHandle(m_hPipeNameEvent) ;
			}

			m_hPipeNameEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;

			
			CptNamePipeServer server ;

			if(server.Create(_T("MeiHua_XetremCopy"),this))
			{
				Debug_Printf("CXCShellInterface::LaunchExtremeCopy() 2") ;

				PROCESS_INFORMATION pi ;
				STARTUPINFO si ;

				::memset(&si,0,sizeof(si)) ;
				si.cb = sizeof(si) ;

				//::CreateProcess(szEXEBuf,_T("-shell"),NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&si,&pi) ;

				//::CreateProcess(szEXEBuf,NULL,NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&si,&pi) ;

				//Debug_Printf() ;
				//::WaitForSingleObject(m_hPipeNameEvent,3*1000) ;
			}

			Debug_Printf("CXCShellInterface::LaunchExtremeCopy() 3") ;
			server.Close() ;

			Debug_Printf("CXCShellInterface::LaunchExtremeCopy() 4") ;
			/**/
			//if(m_hPipeNameEvent!=NULL)
			//{
			//	::CloseHandle(m_hPipeNameEvent) ;
			//}
			//Debug_Printf("launch %s",szEXEBuf) ;
		} 
	}

	//Debug_Printf("CXCShellInterface::LaunchExtremeCopy() end") ;
}

STDMETHODIMP CXCShellInterface::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
	Debug_Printf("CXCShellInterface::InvokeCommand 1") ;
 
	//if(m_pSysObj->m_ContextMenuFileInfo.m_nPasteMenuCmd==LOWORD(pici->lpVerb))
	if(LOWORD(pici->lpVerb)==0)
	{
		if(m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder!=NULL && !m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.empty())
		{
			//Debug_Printf("operate: %s",m_pSysObj->m_ContextMenuFileInfo.m_bMoveOrCopy?"move":"copy") ;

			this->LaunchExtremeCopy() ;

			return S_OK ;
			//for(int i=0;i<m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.size();++i)
			//{
			//	//Debug_Printf("scr file: %s",m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer[i]) ;
			//}

			//Debug_Printf("dst folder: %s",m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder) ;
		}
	} 

	return E_INVALIDARG ;
}

bool GUID2CLSIDString(const GUID& guid,TCHAR* pString,int nSize) ;

STDMETHODIMP CXCShellInterface::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	//Debug_Printf("CXCShellInterface::QueryContextMenu 1 first=%d last=%d index=%d flag=%d",idCmdFirst,idCmdLast,indexMenu,uFlags) ;

	//if ( uFlags & CMF_DEFAULTONLY )
	//{
	//	Debug_Printf("CXCShellInterface::QueryContextMenu 2") ;
	//	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 ); 
	//}

	TCHAR szCLSIDBuf[256] = {0} ;
	if(::GUID2CLSIDString(CLSID_XCContextMenu,szCLSIDBuf,sizeof(szCLSIDBuf)))
	{// 判断是否不为卸载的COM
		CptRegistry reg ;

		TCHAR szCLSIDBuf2[256] = _T("CLSID\\") ;
		::_tcscat(szCLSIDBuf2,szCLSIDBuf) ;
		
		bool bResult = reg.OpenKey(CptRegistry::CLASSES_ROOT,szCLSIDBuf2) ;

		reg.Close() ;

		if(!bResult)
		{// 该COM被卸载了
			return	ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, 0));
		}
	}

	bool bRightClieckFolderMenu = false ;

	if(m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder==NULL )
	{
		DWORD dwAttr = ::GetFileAttributes(m_pSysObj->m_ContextMenuFileInfo.m_szLastFileOrFolder) ;

		if(dwAttr!=INVALID_FILE_ATTRIBUTES && (dwAttr &FILE_ATTRIBUTE_DIRECTORY) )
		{// 当右击的目标为"文件夹"时
			m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder = new TCHAR[sizeof(m_pSysObj->m_ContextMenuFileInfo.m_szLastFileOrFolder)] ;

			if(m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder!=NULL)
			{
				::_tcscpy(m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder,m_pSysObj->m_ContextMenuFileInfo.m_szLastFileOrFolder) ;

				bRightClieckFolderMenu = true ;
			}
		}
	}

	if(m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder!=NULL)
	{// 右击 文件夹空白区
		this->FreeFileBuf() ;

		IDataObject* pClipDataObj = NULL ;
		if(SUCCEEDED(::OleGetClipboard(&pClipDataObj)))
		{// 从剪切板得到要复制的文件
			this->StoreSrcFiles(pClipDataObj) ;

			m_pSysObj->m_ContextMenuFileInfo.m_bMoveOrCopy = this->IsCut(pClipDataObj) ;
			pClipDataObj->Release() ;
		}

		if (!m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.empty() && (uFlags == CMF_NORMAL) || (uFlags & (CMF_VERBSONLY|CMF_DEFAULTONLY))) 
		{
			return	ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, 0));
		}
		/**
		if (!bAdd && (uFlags == CMF_NORMAL) || (uFlags & (CMF_VERBSONLY|CMF_DEFAULTONLY))) 
		{
		Debug_Printf("CXCShellInterface::QueryContextMenu 2a") ;
		return	ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, 0));
		} 
		/**/
		//if(bAdd && (uFlags == CMF_NORMAL))
		//if(bAdd)
		if(!m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.empty())
		{
			//Debug_Printf("CXCShellInterface::QueryContextMenu 3 paste_menu_id=%d",m_pSysObj->m_ContextMenuFileInfo.m_nPasteMenuCmd) ;

			//m_pSysObj->m_ContextMenuFileInfo.m_nPasteMenuCmd = idCmdFirst ;

			TCHAR szBuf[256] = {0} ;

			::LoadString(m_pSysObj->GetModuleHandle(),IDS_RIGHTMENUITEMPASTE,szBuf,sizeof(szBuf)) ;

			//HMENU hTargetMenu = hMenu ;

			//if(bCreatePopMenu)
			//{
			//	hTargetMenu = ::CreatePopupMenu();
			//	::InsertMenu(hMenu, indexMenu, MF_POPUP|MF_BYPOSITION, (LONG_PTR)hTargetMenu, szBuf);
			//	indexMenu = 0;
			//}

			::InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, 0); // 分隔栏
			
			MENUITEMINFO mii ;
			::memset(&mii,0,sizeof(mii)) ;

			mii.cbSize = sizeof(mii) ;
			mii.fMask = MIIM_ID | MIIM_STRING |MIIM_CHECKMARKS | MIIM_BITMAP ;
			mii.dwTypeData = szBuf ;
			mii.wID = idCmdFirst ;
			mii.hbmpUnchecked = this->m_pSysObj->GetBitmap(MIBT_App);
			mii.hbmpChecked = this->m_pSysObj->GetBitmap(MIBT_App);
			//mii.hbmpItem = this->m_pSysObj->GetAppBitmap();

			//Debug_Printf("hbmpItem %s",mii.hbmpItem==NULL?"false":"true") ;

			::InsertMenuItem(hMenu,indexMenu++,TRUE,&mii) ;

			//::SetMenuItemBitmaps(hMenu,

			//::InsertMenu ( hMenu, indexMenu++, MF_STRING|MF_BYPOSITION,m_pSysObj->m_ContextMenuFileInfo.m_nPasteMenuCmd, szBuf );
			//::InsertMenu ( hTargetMenu, indexMenu++, MF_STRING|MF_BYPOSITION,idCmdFirst, szBuf );
			

			//If successful, returns an HRESULT value that has its severity value set to SEVERITY_SUCCESS and its code value set to the offset 
			//of the largest command identifier that was assigned, plus one. For example, assume that idCmdFirst is set to 5 and you add three 
			//items to the menu with command identifiers of 5, 7, and 8. The return value should be MAKE_HRESULT(SEVERITY_SUCCESS, 0, 8 - 5 + 1).
			//Otherwise, it returns an OLE error value. 

			//return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, idCmdFirst+1 ); 
			return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 1 ); 
		}
	}

	//Debug_Printf("CXCShellInterface::QueryContextMenu src_count=%d dst=%s",m_pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.size(),m_pSysObj->m_ContextMenuFileInfo.m_pDstFolder!=NULL?"true":"false") ;
	

	//Debug_Printf("CXCShellInterface::QueryContextMenu 4") ;
  return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

}


//void Func()
//{
//	
//			IDataObject* pClipDataObj = NULL ;
//			if(SUCCEEDED(::OleGetClipboard(&pClipDataObj)))
//			{
//				Debug_Printf("CXCShellInterface::Initialize data object") ;
//
//				IEnumFORMATETC* pFormat = NULL ;
//				FORMATETC etc;
//
//				if(SUCCEEDED(pClipDataObj->EnumFormatEtc(DATADIR_GET,&pFormat)))
//				{
//					TCHAR szBuf[256] = {0} ;
//
//					FORMATETC* pEtc = &etc ;
//
//					pFormat->Reset() ;
//					ULONG uCount = 0 ;
//					while(SUCCEEDED(pFormat->Next(1,pEtc,&uCount)) && uCount>0) 
//					{ 
//
//						//pDataObj->QueryGetData
//						szBuf[0] = 0 ;
//						::GetClipboardFormatName(pEtc->cfFormat,szBuf,sizeof(szBuf)) ;
//
//						Debug_Printf("CXCShellInterface::Initialize format=%d format_name=%s",pEtc->cfFormat,szBuf) ;
//
//						uCount = 0 ;
//					}
//
//					pFormat->Release() ;
//					//::CoTaskMemFree(pEtc) ;
//				}
//				/**
//				CLIPFORMAT nFormat = ::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT) ;
//				//0x80070057 E_INVALIDARG
//				STGMEDIUM	medium2;
//				FORMATETC	fe2 = { nFormat, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
//
//				HRESULT hr9 = pClipDataObj->GetData(&fe2, &medium2) ;
//				Debug_Printf("CXCShellInterface::Initialize nFormat=%d result=%x",nFormat,hr9) ;
//
//				if (SUCCEEDED( hr9)) 
//				{
//					DWORD dwCopyOrMove = (DWORD)::GlobalLock(medium2.hGlobal);
//
//					ULONG uRead = 0 ; 
//
//					//medium2.pstm->Read(&dwCopyOrMove,sizeof(dwCopyOrMove),&uRead) ;
//
//					switch(dwCopyOrMove)
//					{
//					case DROPEFFECT_MOVE: 
//						Debug_Printf("CXCShellInterface::Initialize move") ;
//						break ;
//
//					case DROPEFFECT_COPY:
//						Debug_Printf("CXCShellInterface::Initialize copy") ;
//						break ;
//
//					default:
//						Debug_Printf("CXCShellInterface::Initialize unknow operate=%d",dwCopyOrMove) ;
//						break ;
//					}
//
//					//::GlobalUnlock(medium.hGlobal) ;
//					::ReleaseStgMedium(&medium2) ;
//				}
///**/
//				pClipDataObj->Release() ;
//			}
//}