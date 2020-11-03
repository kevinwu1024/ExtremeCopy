/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "ptdialog.h"
#include "../XCTaskQueue.h"
#include "CptListBox.h"

#ifdef VERSION_HDD_QUEUE

class CXCTaskQueueDlg :
	public CptDialog
{
public:
	enum ETaskQueueResultType
	{
		TQRT_None,
		TQRT_ForceRun,
		TQRT_ForceExit,
		TQRT_MyTurn
	};

public:
	CXCTaskQueueDlg(CXCTaskQueue* pTaskQueue,const pt_STL_vector(CptString)& SrcVer,const pt_STL_vector(CptString)& DstVer);

	virtual ~CXCTaskQueueDlg(void);

	ETaskQueueResultType GetTaskQueueResult() const {return m_TaskQueResult ;}
private:
	virtual BOOL OnInitDialog() ;
	virtual void OnButtonClick(int nButtonID) ;
	virtual bool OnCancel() ;
	virtual int OnProcessMessage(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) ;
	virtual void OnTimer(int nTimerID) ;
	virtual void OnClose() ;

private:
	bool				m_bPause ;
	CXCTaskQueue*		m_pTaskQueue ;
	ETaskQueueResultType	m_TaskQueResult ;

	pt_STL_vector(CptString) m_SrcVer ;
	pt_STL_vector(CptString) m_DstVer ;

	CptListBox				m_SrcFileListBox ;
	CptListBox				m_DestFolderListBox ;
};

#endif