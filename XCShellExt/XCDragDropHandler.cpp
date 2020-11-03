/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCDragDropHandler.h"
#include "ShellExtClassFactory.h"
#include <shellapi.h>
#include <tchar.h>
#include "XCShellExt.h"
#include "../ExtremeCopy/Common/ptWinPath.h"
#pragma warning (disable:4996)

//#define DRAG_DROP_CMD_UNKNOWN		0
#define DRAG_DROP_CMD_COPY					0
#define DRAG_DROP_CMD_MOVE					1
#define DRAG_DROP_CMD_PASTE					2

CXCDragDropHandler::CXCDragDropHandler(CSystemObject* pSysObj):CCOMObject(pSysObj)
{
}

CXCDragDropHandler::~CXCDragDropHandler(void)
{
	Debug_Printf("CXCDragDropHandler::~CXCDragDropHandler() ") ;
}

STDMETHODIMP_(ULONG) CXCDragDropHandler::AddRef()
{
	return this->Increment() ;
}

STDMETHODIMP_(ULONG) CXCDragDropHandler::Release()
{
	return this->Decrement() ;
}

STDMETHODIMP CXCDragDropHandler::QueryInterface(REFIID riid, void **ppv)
{
	HRESULT hr = E_NOINTERFACE ;

	*ppv = NULL ;

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

STDMETHODIMP CXCDragDropHandler::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID)
{
	Debug_Printf("CXCDragDropHandler::Initialize 1") ;

	HRESULT hr = S_OK ;

	m_OperateType = OT_Unknown ; // 默认为未知

	this->GetSysObj()->m_ContextMenuFileInfo.m_strPreSrcFilesVer.clear() ;

	this->GetSysObj()->m_ContextMenuFileInfo.m_strPreSrcFilesVer.assign(this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.begin(),
		this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.end()) ;

	this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.clear() ;

	this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder = _T("") ;

	if(pDataObj!=NULL)
	{

		// 从剪切板获取要操作的源文件
		STGMEDIUM	medium;
		FORMATETC	fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

		if (SUCCEEDED(pDataObj->GetData(&fe, &medium)))
		{
			HDROP hDrop = (HDROP)::GlobalLock(medium.hGlobal);

			if(hDrop!=NULL)
			{
				int nFileCount = ::DragQueryFile(hDrop,0xffffffff, NULL, 0) ;

				TCHAR	szTem[MAX_PATH_EX] ;

				for(int i=0;i<nFileCount;++i)
				{
					szTem[0] = 0 ;

					::DragQueryFile(hDrop,i,szTem,sizeof(szTem)/sizeof(TCHAR)) ;

					this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.push_back(szTem) ;
				}
			}

			::GlobalUnlock(medium.hGlobal) ;
			::ReleaseStgMedium(&medium) ;

			// 从剪切板获取将要进行的操作
			m_OperateType = ::GetOperateType(pDataObj) ;

		} 
	}

	if(pidlFolder!=NULL)
	{
		TCHAR	szTem[MAX_PATH_EX] ;

		if(::SHGetPathFromIDList(pidlFolder,szTem))
		{
			this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder = szTem ;
		}
	}

	return hr ;
}


STDMETHODIMP CXCDragDropHandler::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT ccMax)
{
	return E_INVALIDARG;

	HRESULT hr = S_OK ;

	return hr ;
}

STDMETHODIMP CXCDragDropHandler::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{ 
	HRESULT hr = S_OK ;

	if(pici!=NULL) 
	{
		int cmd = LOWORD(pici->lpVerb) ;

		if(cmd==DRAG_DROP_CMD_PASTE || cmd==DRAG_DROP_CMD_COPY || cmd==DRAG_DROP_CMD_MOVE)
		{
			CSystemObject* pSysObj = this->GetSysObj() ;

			if(!pSysObj->m_ContextMenuFileInfo.m_strCurSrcFilesVer.empty()
				&& !pSysObj->m_ContextMenuFileInfo.m_strDstFolder.IsEmpty())
			{
				const int ActType_Nothing = 0 ;
				const int ActType_CreateShortcut = 1 ;
				const int ActType_Move = 2 ;
				const int ActType_Copy = 3 ;

				int ActType = ActType_Nothing;

				if(cmd==DRAG_DROP_CMD_PASTE)
				{
					if(m_OperateType==OT_Unknown)
					{// 为拖放操作
						bool bShiftPressed = (pici->fMask & CMIC_MASK_SHIFT_DOWN) ? true : false ;
						bool bCtrlPressed = (pici->fMask & CMIC_MASK_CONTROL_DOWN) ? true : false ;

						bool bAltPressed = (::GetKeyState(VK_MENU)&0x80) ? true : false ;

						if((bCtrlPressed && bShiftPressed && !bAltPressed) 
							|| (bAltPressed && !bCtrlPressed && !bShiftPressed))
						{// 创建快捷方式
							ActType = ActType_CreateShortcut ;
						}
						else if((bShiftPressed && !bCtrlPressed)) 
						{// 按着shift键来拖放

							ActType = ActType_Move ; // 默认是拖放

							CptWinPath::SPathElementInfo pei ;

							pei.uFlag = CptWinPath::PET_Path ;

							if(CptWinPath::GetPathElement(pSysObj->m_ContextMenuFileInfo.m_strCurSrcFilesVer[0].c_str(),pei)
								&& pSysObj->m_ContextMenuFileInfo.m_strDstFolder.CompareNoCase(pei.strPath)==0)
							{
								ActType = ActType_Copy ; // 如果是在同一文件夹里按 shift 键来拖放的话，那么这是做复制
							}
						}
						else
						{
							// 下面这行代码暂时从下面的判断移走，因为当文件被拖放时，m_SrcFilesVer 总为空的
							// !pSysObj->m_ContextMenuFileInfo.m_SrcFilesVer.empty()

							if(!bCtrlPressed && pSysObj->m_ContextMenuFileInfo.m_strDstFolder.GetLength()>0 
								&& pSysObj->m_ContextMenuFileInfo.m_strCurSrcFilesVer[0].GetAt(0)==pSysObj->m_ContextMenuFileInfo.m_strDstFolder.GetAt(0)
								&& pSysObj->m_ContextMenuFileInfo.m_strCurSrcFilesVer[0].GetAt(0)!='\\')
							{// 若 control 键没被按下, 并且是同一分区,则是'移动' 文件处理
								ActType = ActType_Move ;
							}
							else
							{
								ActType = ActType_Copy ;
							}
						}
					}
					else
					{// 快捷键或菜单操作
						switch(m_OperateType)
						{
						case OT_Move: ActType=ActType_Move ; break ;
						case OT_Link: ActType=ActType_CreateShortcut ; break ;

						case OT_Copy: ActType=ActType_Copy ; break ;

						case OT_CopyLink: ActType = (m_bDefaultItemIsLink?ActType_CreateShortcut:ActType_Copy); break ;

						default: ActType = ActType_Nothing ; break ;
						}
					}
				}
				else// if(cmd==DRAG_DROP_CMD_UNKNOWN)
				{// 右键拖放					
					if(cmd==DRAG_DROP_CMD_COPY)
					{
						ActType=ActType_Copy ;
					}
					else if(cmd==DRAG_DROP_CMD_MOVE)
					{
						ActType=ActType_Move ;
					}
				}

				switch(ActType)
				{
				default:
					_ASSERT(FALSE) ;
				case ActType_Nothing:
					break ;

				case ActType_CreateShortcut:
					::CreateShortcut(this->GetSysObj(),this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer,
						this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder,pici->hwnd) ;
					break ;

				case ActType_Move:
				case ActType_Copy:
					::LaunchExtremeCopy(this->GetSysObj(),this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer,
						this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder,(ActType==ActType_Move),pici->hwnd) ;
					break ;
				}

				this->GetSysObj()->m_ContextMenuFileInfo.m_strPreSrcFilesVer.clear() ;
				this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.clear() ;
				this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder.Empty() ;
			}
		}
		
	}

	return hr ;
}

STDMETHODIMP CXCDragDropHandler::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	Debug_Printf("CXCDragDropHandler::QueryContextMenu 1") ;

	if(!this->GetSysObj()->m_ContextMenuFileInfo.m_strCurSrcFilesVer.empty()
		&& !this->GetSysObj()->m_ContextMenuFileInfo.m_strDstFolder.IsEmpty())
	{// 插入 “右键拖放” 菜单
		TCHAR szBuf[256] = {0} ;

		MENUITEMINFO mii ;

		// 添加 “Extreme Copy”
		::LoadString(CptMultipleLanguage::GetInstance()->GetResourceHandle(),IDS_RIGHTMENUITEM_EXTREME_COPY,szBuf,sizeof(szBuf)) ;

		::memset(&mii,0,sizeof(mii)) ;

		mii.cbSize = sizeof(mii) ;
		mii.fMask = MIIM_ID | MIIM_STRING |MIIM_CHECKMARKS | MIIM_BITMAP ;
		mii.dwTypeData = szBuf ;
		mii.wID = idCmdFirst + DRAG_DROP_CMD_COPY ;
		mii.hbmpUnchecked = this->GetSysObj()->GetBitmap(MIBT_App);
		mii.hbmpChecked = this->GetSysObj()->GetBitmap(MIBT_App);

		::InsertMenuItem(hMenu,indexMenu++,TRUE,&mii) ;

		// 添加 “Extreme Move”
		::LoadString(CptMultipleLanguage::GetInstance()->GetResourceHandle(),IDS_RIGHTMENUITEM_EXTREME_MOVE,szBuf,sizeof(szBuf)) ;

		::memset(&mii,0,sizeof(mii)) ;

		mii.cbSize = sizeof(mii) ;
		mii.fMask = MIIM_ID | MIIM_STRING |MIIM_CHECKMARKS | MIIM_BITMAP ;
		mii.dwTypeData = szBuf ;
		mii.wID = idCmdFirst + DRAG_DROP_CMD_MOVE; // 
		mii.hbmpUnchecked = this->GetSysObj()->GetBitmap(MIBT_App);
		mii.hbmpChecked = this->GetSysObj()->GetBitmap(MIBT_App);

		::InsertMenuItem(hMenu,indexMenu++,TRUE,&mii) ;

		//添加 “Extreme Paste”
		::LoadString(CptMultipleLanguage::GetInstance()->GetResourceHandle(),IDS_RIGHTMENUITEM_PASTE,szBuf,sizeof(szBuf)) ;

		::memset(&mii,0,sizeof(mii)) ;

		mii.cbSize = sizeof(mii) ;
		mii.fMask = MIIM_ID | MIIM_STRING |MIIM_CHECKMARKS | MIIM_BITMAP ;
		mii.dwTypeData = szBuf ;
		mii.wID = idCmdFirst + DRAG_DROP_CMD_PASTE;
		mii.hbmpUnchecked = this->GetSysObj()->GetBitmap(MIBT_App);
		mii.hbmpChecked = this->GetSysObj()->GetBitmap(MIBT_App);

		::InsertMenuItem(hMenu,indexMenu++,TRUE,&mii) ;

		if(::IsReplaceExplorerCopying() && ::IsExtremeCopyExeExist())
		{// 如果设置为"取代Explorer的复制功能", 并且ExtremeCopy 程序又存在的话

			if(!this->GetSysObj()->m_bUseWindowsPaste)
			{// 虽然

				MENUITEMINFO mii;
				mii.cbSize = sizeof(MENUITEMINFO);
				mii.fMask = MIIM_STATE;

				//if(::GetMenuItemInfo(hMenu, 1, FALSE, &mii) && mii.fState & MFS_DEFAULT)
				//	m_eDefaultMenuItem = eAction_Copy;
				//if(::GetMenuItemInfo(hMenu, 2, FALSE, &mii) && mii.fState & MFS_DEFAULT)
				//	m_eDefaultMenuItem = eAction_Move;

				m_bDefaultItemIsLink = (::GetMenuItemInfo(hMenu, 3, FALSE, &mii) && (mii.fState & MFS_DEFAULT)) ;

				// 指定 “Extreme Paste” 为默认的选项
				::SetMenuDefaultItem(hMenu, idCmdFirst+DRAG_DROP_CMD_PASTE, FALSE);	
			}
		}
		
		::InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, 0); // 分隔栏

		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 3 ); 
	}

	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );
}