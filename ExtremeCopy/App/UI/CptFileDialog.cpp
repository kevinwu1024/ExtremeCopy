/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "CptFileDialog.h"
#include <Commdlg.h>
#include <shlobj.h>
#include <WinUser.h>

CptFileDialog::CptFileDialog(void)
{
}

CptFileDialog::~CptFileDialog(void)
{
}


bool CptFileDialog::Open(bool bFileOrFolder,bool bSaveOrOpen,const TCHAR* pFilter,DWORD dwFlag,HWND hOwnWnd) 
{
	bool bRet = false ;

	m_StrList.Clear() ;

	TCHAR szFile[8*MAX_PATH] = {0} ;

	if(bFileOrFolder)
	{// file

		OPENFILENAME ofn ;
		::memset(&ofn,0,sizeof(ofn)) ;

		ofn.hInstance = (HMODULE)::GetCurrentProcess() ;
		ofn.lStructSize = sizeof(ofn) ;
		ofn.hwndOwner = hOwnWnd;
		ofn.lpstrFile = szFile;

		ofn.lpstrFile[0] = _T('\0');
		ofn.nMaxFile = sizeof(szFile)/sizeof(TCHAR);
		//ofn.lpstrFilter = _T("All File\0*.*\0");
		ofn.lpstrFilter = pFilter ;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.lpfnHook = (LPOFNHOOKPROC)MyFolderProc; 

		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST  | OFN_EXPLORER | OFN_PATHMUSTEXIST ;

		const bool bMultiSelected = dwFlag &EOPEN_MULTISELECTED ? true : false ;// 多选
		const bool bSelectFolder = dwFlag &EOPEN_SELECTFOLDER ? true : false ;// 选文件夹

		ofn.Flags |= bMultiSelected  ? OFN_ALLOWMULTISELECT : 0 ; 
		ofn.Flags |= bSelectFolder  ? OFN_ENABLEHOOK : 0 ; 

		if(bSaveOrOpen)
		{// save
			if(::GetSaveFileName(&ofn))
			{
				m_StrList.Add(ofn.lpstrFile) ;
				bRet = true ;
			}
		}
		else
		{// open
			if(::GetOpenFileName(&ofn))
			{
				if(bMultiSelected && ::_tcslen(ofn.lpstrFile)<ofn.nFileOffset)
				{// 多文件
					CptString strFolder = ofn.lpstrFile ;

					CptString strFile ;

					int nIndex = ofn.nFileOffset ;

					while(ofn.lpstrFile[nIndex]!=NULL)
					{
						strFile = strFolder + _T("\\") + (ofn.lpstrFile + nIndex) ;

						m_StrList.Add(strFile) ;

						nIndex += (int)(::_tcslen(ofn.lpstrFile + nIndex) + 1) ;
					}

					bRet = true ;
				}
				else
				{// 单文件
					m_StrList.Add(ofn.lpstrFile) ;
					bRet = true ;
				}
			}
		}

	}
	else
	{// folder

		BROWSEINFO brInfo ;
		::memset(&brInfo,0,sizeof(brInfo)) ;

		brInfo.ulFlags = BIF_NEWDIALOGSTYLE ;

		LPITEMIDLIST pidlBrowse = ::SHBrowseForFolder(&brInfo) ;

		if(pidlBrowse!=NULL)
		{
			if(::SHGetPathFromIDList(pidlBrowse,szFile))
			{
				m_StrList.Add(szFile) ;
				bRet = true ;
			}
		}
		//if(::GetSaveFileName(&ofn))
		//{
		//	m_StrList.Add(ofn.lpstrFile) ;
		//	bRet = true ;
		//}
	}

	return bRet ;
}

const CptStringList& CptFileDialog::GetSelectedFiles() const
{
	return m_StrList ;
}

LONG g_lOriWndProc = NULL;
BOOL g_bReplaced = FALSE;

LRESULT static __stdcall  _WndProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
    if( uMsg == WM_COMMAND )
    {
        if( wParam == IDOK )
        {
            //Your codes
        }
    }

	return ((WNDPROC)g_lOriWndProc)( hwnd, uMsg, wParam ,lParam );
    //return CallWndProc( (WNDPROC) g_lOriWndProc , hwnd, uMsg, wParam ,lParam );
}


UINT_PTR CptFileDialog::MyFolderProc(  HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( hdlg != NULL &&  g_bReplaced  == FALSE )
	{
		TCHAR wcsClassName1[MAX_PATH];
		TCHAR wcsClassName2[MAX_PATH];
		HWND hParent = ::GetParent( hdlg );
		::GetClassName( hParent, wcsClassName1 , sizeof(wcsClassName1)/sizeof(TCHAR) );
		::GetClassName( hdlg     , wcsClassName2 , sizeof(wcsClassName2)/sizeof(TCHAR) );

		if( 0== ::_tcscmp( wcsClassName1, wcsClassName2 ) )
		{
			g_bReplaced  = TRUE;
			g_lOriWndProc  = ::SetWindowLong( ::GetParent( hdlg ), GWLP_WNDPROC , (LONG)_WndProc );
		}
	}
	if( uiMsg == WM_NOTIFY )
	{
		LPOFNOTIFY lpOfNotify = (LPOFNOTIFY)lParam;
		if( lpOfNotify->hdr.code == CDN_FOLDERCHANGE )
		{
			// your codes here
			//MessageBox(NULL,_T("folder"),_T(""),0) ;
		}
	}


	return FALSE;
}
