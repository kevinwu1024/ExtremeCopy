/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "..\..\Common\ptString.h"

enum EFileDlgFlag
{
	EOPEN_MULTISELECTED = 1<<0,
	EOPEN_SELECTFOLDER = 1<<1
};

class CptFileDialog
{
public:
	CptFileDialog(void);
	~CptFileDialog(void);

	bool Open(bool bFileOrFolder,bool bSaveOrOpen,const TCHAR* pFilter,DWORD dwFlag,HWND hOwnWnd) ;
	const CptStringList& GetSelectedFiles() const;

private:
	UINT_PTR static __stdcall  MyFolderProc(  HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

private:
	CptStringList		m_StrList ;

};
