/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "ptlistview.h"
#include "..\XCTransApp.h"
#include "CptImageList.h"
#include <map>
#include "..\XCFailedFile.h"


class CXCFailFileListView :
	public CptListView
{
public:
	CXCFailFileListView(void);
	virtual ~CXCFailFileListView(void);

	bool Attach(HWND hWnd) ;

	int AddFailFile(const SFailedFileInfo& FailFileInfo) ;
	int RetryAllFailedFile() ;

	bool UpdateFailFileStatusByID(const SFailedFileInfo& ffi) ;

	void SetUIEventCB(CXCFailedFile* pFailedFile) ;

protected:
	virtual int PreProcCtrlMsg(HWND hWnd,UINT nMsg,WPARAM wParam, LPARAM lParam) ;

	virtual void OnRButtonUp(int nFlag,const SptPoint& pt) ;
	virtual void OnRButtonDown(int nFlag,const SptPoint& pt) ;

private:
	inline int GetIconIndexByStatus(EFailFileStatusType ffst) const;
	void SetFileIcon(int nItemIndex,int nSubItemIndex) ;
	void ShowContextMenu() ;

	CptString GetStatusString(EFailFileStatusType status) ;

	inline void DestroyTackMenu() ;

	inline void LocateFile(bool bSrcOrDst) ;
	inline void PropertyFile(bool bSrcOrDst) ;
	inline void SetSelectedFileState(EFailFileStatusType NewState) ;

#define		STATUSICONINDEX_FAIL			0
#define		STATUSICONINDEX_SUCCESS			1
#define		STATUSICONINDEX_RUNNING			2
#define		STATUSICONINDEX_WAITING			3

	CptImageList	m_ImageList ;

	pt_STL_map(int,int)	m_SystemIndex2IconIndexMap ;
	HMENU			m_hTackMenu ;

	CXCFailedFile*		m_pFailedFile ;
};
