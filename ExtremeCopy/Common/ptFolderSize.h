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
#include <vector>
#include "ptString.h"
#include "ptTypeDef.h"

struct SFileOrDirectoryInfo
{
	unsigned __int64 nTotalSize ;
	unsigned int nFileCount ;
	unsigned int nFolderCount ;
};

class CptFolderSize ;

class CFolderSizeEventCB
{
	friend CptFolderSize ;
protected:
	virtual void OnFolderSizeCompleted() {};
	virtual void OnFolderSizeReportInfo(const SFileOrDirectoryInfo& info) {}
	virtual void OnFolderSizeFileAttribute(LPCTSTR szFileFullName,const WIN32_FIND_DATA& wfd){}
};

class CptFolderSize
{
public:
	enum ERunState
	{
		State_Running,
		State_Pause,
		State_Stop
	};

public:
	CptFolderSize(void);
	virtual ~CptFolderSize(void);

	ERunState GetCurrnetState() const { return m_CurState;}

	bool Continue() ;
	bool Pause() ;
	void Stop() ;

	bool Start(const pt_STL_vector(CptString)& FileVer,CFolderSizeEventCB* pCB) ;

	ERunState GetState() const {return m_CurState;}

private:
	static unsigned int __stdcall AnalysisFolderThreadFunc(void*) ;

	void AnalysisFolderWork() ;
	void TravelDirectory(CptString strDir,SFileOrDirectoryInfo& fdi) ;
	void Wait() ;
	
private:
	CFolderSizeEventCB*			m_pEventCallBack ;
	ERunState					m_CurState ;
	HANDLE						m_hAnalysisThread ;

	pt_STL_vector(CptString)	m_strFolderVer ;
	int							m_nLastCheckFileCount ;

	HANDLE						m_hWaitEvent ;
};
