/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCContextMenu.h"
#include "ShellExtClassFactory.h"
#include <shellapi.h>
#include <tchar.h>
#include "XCShellExt.h"
#pragma warning (disable:4996)

#define		WINDOWS_PASTE_MENUITEM_ID		30979

#define		MENU_ITEM_ID_PASTE			0
#define		MENU_ITEM_ID_LAUNCH			1


CXCContextMenu::CXCContextMenu(CSystemObject* pSysObj):CCOMObject(pSysObj)
{
}

CXCContextMenu::~CXCContextMenu(void)
{
}


STDMETHODIMP_(ULONG) CXCContextMenu::AddRef()
{
	return this->Increment() ;
}

STDMETHODIMP_(ULONG) CXCContextMenu::Release()
{
	return this->Decrement() ;
}

bool GUID2CLSIDString(const GUID& guid,TCHAR* pString,int nSize) ;

STDMETHODIMP CXCContextMenu::QueryInterface(REFIID riid, void **ppv)
{
	HRESULT hr = E_NOINTERFACE ;

	*ppv = NULL ;

	TCHAR szCLIDBuf[260] = {0} ;
	GUID2CLSIDString(riid,szCLIDBuf,sizeof(szCLIDBuf)) ;

	if(::IsEqualGUID(IID_IUnknown,riid) || ::IsEqualGUID(IID_IShellExtInit,riid))
	{
		*ppv = static_cast<IShellExtInit *>(this) ;
		
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


STDMETHODIMP CXCContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
	if(pici!=NULL)
	{
		int cmd = LOWORD(pici->lpVerb) ;

		if(cmd==0)
		{
			if(!this->GetSysObj()->m_ContextMenuFileInfo.m_bExeTaskFile
				&& !this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder.IsEmpty())
			{// 执行 复制

				if(!::IsReplaceExplorerCopying())
				{// 当extremecopy 不是默认的COPY HANDLER时,点击MENU ITEM则说明是'extreme paste',所以执行 extremecopy
					::LaunchExtremeCopy(this->GetSysObj(),this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer,
						this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder,m_bIsMove,pici->hwnd) ;
				}
				else
				{// 当extremecopy 是默认的COPY HANDLER时,点击MENU ITEM则说明是'windows paste',所以执行 windows 复制

					this->GetSysObj()->m_bUseWindowsPaste = true ;

					int nBufSize = 4 ;

					for(size_t i=0;i<this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.size();++i)
					{
						nBufSize += this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer[i].GetLength()+1 ;
					}

					TCHAR* pSrcBuf = new TCHAR[nBufSize] ;
					::memset(pSrcBuf,0,nBufSize*sizeof(TCHAR)) ;
					int nOffset = 0 ;

					for(size_t i=0;i<this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.size();++i)
					{
						::_tcscpy(pSrcBuf+nOffset,this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer[i].c_str()) ;
						nOffset += this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer[i].GetLength()+1 ;
					}

					SHFILEOPSTRUCT   FileOp;
					::memset(&FileOp,0,sizeof(FileOp)) ;

					//FileOp.fFlags = FOF_NOCONFIRMATION ; 
					FileOp.hNameMappings = NULL;
					FileOp.hwnd = NULL;
					FileOp.lpszProgressTitle = NULL;
					FileOp.pFrom = pSrcBuf;
					FileOp.pTo = this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder.c_str();
					FileOp.wFunc = m_bIsMove ? FO_MOVE : FO_COPY;

					::SHFileOperation(&FileOp) ;

					delete [] pSrcBuf ;
					pSrcBuf = NULL ;

					this->GetSysObj()->m_bUseWindowsPaste = false ;
				}
			}
			else if(this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.size()==1 && this->GetSysObj()->m_ContextMenuFileInfo.m_bExeTaskFile)
			{// 执行 copy task 文件
				::ExecuteTaskFile(this->GetSysObj(),this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer[0]) ;
			}
			else if(this->GetSysObj()->m_ContextMenuFileInfo.m_bLaunchWithTaskEditor && !this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.empty())
			{// 以 Launch With ExtremeCopy 启动选择了的文件
				::LaunchTaskEditor(	this->GetSysObj(),this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer) ;
			}
		}
		else
		{
			// 这是来自和“Extreme Paste”/“Windows Paste” 混合在一起的 “Launch with ExtremeCopy Pro”
			if(!this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder.IsEmpty())
			{// 以 Launch With ExtremeCopy 启动选择了的文件
				pt_STL_vector(CptString) FileVer ;

				FileVer.push_back(this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder) ;

				::LaunchTaskEditor(	this->GetSysObj(),FileVer) ;
			}
		}
	}

	Debug_Printf("CXCContextMenu::InvokeCommand end") ;

	return E_INVALIDARG ;
}

HRESULT CXCContextMenu::StoreSrcFiles(IDataObject* pDataObj)
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

		this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.clear() ;

		HDROP hDrop = (HDROP)::GlobalLock(medium.hGlobal);

		if(hDrop!=NULL)
		{
			//hr = S_OK ; // 设置为成功
			int nFileCount = ::DragQueryFile(hDrop,0xffffffff, NULL, 0) ;

			TCHAR szFileName[8*MAX_PATH] = {0} ;

			for(int i=0;i<nFileCount;++i)
			{
				::DragQueryFile(hDrop,i,szFileName,sizeof(szFileName)/sizeof(TCHAR)) ;

				this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.push_back(szFileName) ;
			}

			::GlobalUnlock(medium.hGlobal) ;
			::ReleaseStgMedium(&medium) ;

			if(this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.empty())
			{
				hr = E_FAIL;
			}
		}
	}

	return hr ;
}

STDMETHODIMP CXCContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	Debug_Printf("CXCContextMenu::QueryContextMenu 1") ;

	if(!::IsValidCOM(CLSID_XCContextMenu) || !::IsExtremeCopyExeExist())
	{
		Debug_Printf("CXCContextMenu::QueryContextMenu 1a") ;

		return	ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, 0));
	}

	bool bXCOrWindowPasteItem = !::IsReplaceExplorerCopying() ;

	if(this->CreateCopyMenuItem(hMenu,indexMenu,idCmdFirst,bXCOrWindowPasteItem))
	{
		Debug_Printf("CXCContextMenu::QueryContextMenu 2") ;

		this->GetSysObj()->m_ContextMenuFileInfo.m_bExeTaskFile = false ;

		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 2 );
	}

	{// 若不是复制,则判断是否为执行 copy task 文件

		bool bIsRightClickFolder = false ;

		//if(this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder.GetLength()>0
		//	&& this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.empty())
		//{
		//	this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.push_back(this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder) ;
		//	bIsRightClickFolder = true ;
		//}
		//else
		{
			// 如果用户只是右击 单个 文件夹，则也应该显示 “Launch With ExtremeCopy Pro” 这个菜单选项
			if(this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder.GetLength()>0
				&& this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.size()==1)
			{
				bIsRightClickFolder = (this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder.CompareNoCase(
					this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer[0])==0) ;
			}
		}

		if(bIsRightClickFolder || (this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder.IsEmpty() 
			&& this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.size()>0))
		{// 
			if(this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.size()==1)
			{
				const DWORD dwAttr = ::GetFileAttributes(this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer[0]) ;

				if(dwAttr!=INVALID_FILE_ATTRIBUTES)
				{// 这是有效的文件
					CptString strExtName = this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer[0].Right(4) ;

					if(strExtName.CompareNoCase(_T(".xct"))==0)
					{// 确定这是一个 copy task file
						TCHAR szBuf[256] = {0} ;

						::LoadString(CptMultipleLanguage::GetInstance()->GetResourceHandle(),IDS_RIGHTMENUITEM_EXECUTEBYXC,szBuf,sizeof(szBuf)) ;

						::InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, 0); // 分隔栏

						MENUITEMINFO mii ;
						::memset(&mii,0,sizeof(mii)) ;

						mii.cbSize = sizeof(mii) ;
						mii.fMask = MIIM_ID | MIIM_STRING |MIIM_CHECKMARKS | MIIM_BITMAP ;
						mii.dwTypeData = szBuf ;
						mii.wID = idCmdFirst ;
						mii.hbmpUnchecked = this->GetSysObj()->GetBitmap(MIBT_App);
						mii.hbmpChecked = this->GetSysObj()->GetBitmap(MIBT_App);

						::InsertMenuItem(hMenu,indexMenu++,TRUE,&mii) ;

						this->GetSysObj()->m_ContextMenuFileInfo.m_bLaunchWithTaskEditor = false ;
						this->GetSysObj()->m_ContextMenuFileInfo.m_bExeTaskFile = true ;

						return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 1 ); 
					}
				}
			}

			
			// 如果不是 *.xct 文件，则插入 “Launch With ExtremeCopy Pro” 菜单
			
			TCHAR szBuf[256] = {0} ;

			::LoadString(CptMultipleLanguage::GetInstance()->GetResourceHandle(),
				IDS_RIGHTMENUITEM_LAUNCH_WITH_EXTREMECOPY,szBuf,sizeof(szBuf)) ;

			::InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, 0); // 分隔栏

			MENUITEMINFO mii ;
			::memset(&mii,0,sizeof(mii)) ;

			mii.cbSize = sizeof(mii) ;
			mii.fMask = MIIM_ID | MIIM_STRING |MIIM_CHECKMARKS | MIIM_BITMAP ;
			mii.dwTypeData = szBuf ;
			mii.wID = idCmdFirst ;
			mii.hbmpUnchecked = this->GetSysObj()->GetBitmap(MIBT_App);
			mii.hbmpChecked = this->GetSysObj()->GetBitmap(MIBT_App);

			::InsertMenuItem(hMenu,indexMenu++,TRUE,&mii) ;

			this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder = _T("") ;
			this->GetSysObj()->m_ContextMenuFileInfo.m_bExeTaskFile = false ;
			this->GetSysObj()->m_ContextMenuFileInfo.m_bLaunchWithTaskEditor = true ;

			::InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, 0); // 分隔栏

			return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 1 ); 
		}
	}

	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 ); 
}

bool CXCContextMenu::CreateCopyMenuItem(HMENU hMenu,int indexMenu,int idCmdFirst,bool bXCOrWindows)
{
	bool bRet = false ;

	bool bAddMenuItem = false ;

	if(this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder.IsEmpty() 
		&& this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.size()==1
		&& !this->GetSysObj()->m_ContextMenuFileInfo.m_strPreSrcFilesVer.empty())
	{
		const DWORD dwAttr = ::GetFileAttributes(this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer[0]) ;

		if(dwAttr!=INVALID_FILE_ATTRIBUTES && (dwAttr &FILE_ATTRIBUTE_DIRECTORY) )
		{// 当右击的目标为"文件夹"时，把该文件夹记录下来
			this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder = this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer[0] ;
		}
	}

	if(!this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder.IsEmpty())
	{
		IDataObject* pClipDataObj = NULL ;
		if(SUCCEEDED(::OleGetClipboard(&pClipDataObj)))
		{// 从剪切板得到要复制的文件
			if(this->StoreSrcFiles(pClipDataObj)==S_OK)
			{
				m_bIsMove = ::IsCut(pClipDataObj) ;

				pClipDataObj->Release() ;
				bAddMenuItem = true ;
			}
		}
	}

	if(bAddMenuItem)
	{
		// 添加 'extreme paste' 或 'Windows paste' 菜单项
		::InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, 0); // 分隔栏

		TCHAR szBuf[128] = {0} ;

		const int nStringID = bXCOrWindows ? IDS_RIGHTMENUITEM_PASTE : IDS_RIGHTMENUITEM_WINDOWPASTE ;
		HBITMAP hItemBmp = this->GetSysObj()->GetBitmap(bXCOrWindows?MIBT_App:MIBT_Windows) ;

		::LoadString(CptMultipleLanguage::GetInstance()->GetResourceHandle(),nStringID,szBuf,sizeof(szBuf)) ;

		MENUITEMINFO mii ;
		::memset(&mii,0,sizeof(mii)) ;

		mii.cbSize = sizeof(mii) ;
		mii.fMask = MIIM_ID | MIIM_STRING |MIIM_CHECKMARKS | MIIM_BITMAP ;
		mii.dwTypeData = szBuf ;
		mii.wID = idCmdFirst ;
		mii.hbmpUnchecked = hItemBmp;
		mii.hbmpChecked = hItemBmp;

		if(!bXCOrWindows)
		{// windows paste
			mii.fState = ::GetMenuState(hMenu,WINDOWS_PASTE_MENUITEM_ID,MF_BYCOMMAND) ;
		}

		::InsertMenuItem(hMenu,indexMenu++,TRUE,&mii) ;

		// 添加 “Launch With ExtremeCopy Pro”
		::LoadString(CptMultipleLanguage::GetInstance()->GetResourceHandle(),
			IDS_RIGHTMENUITEM_LAUNCH_WITH_EXTREMECOPY,szBuf,sizeof(szBuf)) ;

		::memset(&mii,0,sizeof(mii)) ;

		mii.cbSize = sizeof(mii) ;
		mii.fMask = MIIM_ID | MIIM_STRING |MIIM_CHECKMARKS | MIIM_BITMAP ;
		mii.dwTypeData = szBuf ;
		mii.wID = idCmdFirst+1 ;
		mii.hbmpUnchecked = this->GetSysObj()->GetBitmap(MIBT_App);
		mii.hbmpChecked = this->GetSysObj()->GetBitmap(MIBT_App);

		::InsertMenuItem(hMenu,indexMenu++,TRUE,&mii) ;

		bRet = true ;
	}

	return bRet ;
}

STDMETHODIMP CXCContextMenu::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID)
{
	Debug_Printf("CXCContextMenu::Initialize 1") ;

	HRESULT hr = S_OK;

	if(pidlFolder==NULL)
	{
		Debug_Printf("CXCContextMenu::Initialize 2") ;

		this->GetSysObj()->m_ContextMenuFileInfo.m_strPreSrcFilesVer.clear() ;

		// 暂存到 "先前" 的源文件缓冲区
		this->GetSysObj()->m_ContextMenuFileInfo.m_strPreSrcFilesVer.assign(this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.begin(),
			this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.end()) ;

		this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.clear() ;

		this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder = _T("") ;

		if(pDataObj!=NULL)
		{// 源文件
			Debug_Printf("CXCContextMenu::Initialize 3") ;

			STGMEDIUM	medium;
			FORMATETC	fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

			if (SUCCEEDED(pDataObj->GetData(&fe, &medium)))
			{
				HDROP hDrop = (HDROP)::GlobalLock(medium.hGlobal);

				if(hDrop!=NULL)
				{
					int nFileCount = ::DragQueryFile(hDrop,0xffffffff, NULL, 0) ;

					TCHAR	szTem[MAX_PATH_EX] ;
					//TCHAR*	szTem = new TCHAR[MAX_PATH_EX] ;

					for(int i=0;i<nFileCount;++i)
					{
						szTem[0] = 0 ;

						::DragQueryFile(hDrop,i,szTem,MAX_PATH_EX) ;

						this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.push_back(szTem) ;
					}
				}

				::GlobalUnlock(medium.hGlobal) ;
				::ReleaseStgMedium(&medium) ;
			} 
		}
	}

	if(pidlFolder!=NULL)
	{// 目标文件夹
		TCHAR*	szTem = new TCHAR[MAX_PATH_EX] ;

		if(::SHGetPathFromIDList(pidlFolder,szTem))
		{
			this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder = szTem ;
		}

		delete [] szTem ;
		szTem = NULL ;
	}

	return S_OK ;
}

//STDMETHODIMP CXCContextMenu::GetCommandString(UINT idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT ccMax)
STDMETHODIMP CXCContextMenu::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT ccMax)
{
	//Debug_Printf("CXCContextMenu::GetCommandString 1") ;

	/**
	if ((uFlags & GCS_HELPTEXT) && (uFlags & GCS_UNICODE)) 
	{
		//BOOL	is_dd = m_pDstFolder!=NULL;

		Debug_Printf("CXCContextMenu::GetCommandString 2") ;
		switch (idCmd) 
		{
		case 0:
			Debug_Printf("CXCContextMenu::GetCommandString 3") ;
			lstrcpynW ( (LPWSTR) pszName, L"Extreme Copy", ccMax );
			//strncpy(pszName, (m_pDstFolder!=NULL) ? GetLoadStr(IDS_DDCOPY) : GetLoadStr(IDS_RIGHTCOPY), cchMax);
			return	S_OK; 
		//case 1:
		//	//strncpy(pszName, (m_pDstFolder!=NULL) ? GetLoadStr(IDS_DDMOVE) : GetLoadStr(IDS_RIGHTDEL), cchMax);
		//	return	S_OK;
		case 2:
			Debug_Printf("CXCContextMenu::GetCommandString 4") ;
			lstrcpynW ( (LPWSTR) pszName, L"Extreme Paste", ccMax );
			//strncpy(pszName, GetLoadStr(IDS_RIGHTPASTE), cchMax);
			return	S_OK;
		}
	}

	Debug_Printf("CXCContextMenu::GetCommandString 5") ;
	/**/
	return E_INVALIDARG;
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

