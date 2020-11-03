/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "..\Common\ptTypeDef.h"
#include <vector>
#include "..\Common\ptThreadLock.h"
#include "XCGlobal.h"


class CXCFailedFileObserver ;

class CXCFailedFile
{
public:
	CXCFailedFile() ;
	~CXCFailedFile() ;

	int SetMaxCount(int nNewMax) ;
	int AddFailedFile(const SFailedFileInfo& ffi) ;
	bool UpdateFailedFile(const SFailedFileInfo& ffi) ;
	//bool ChangeFailedFileStatus(int nIndex,EFailFileStatusType NewStatus);
	bool GetFailedFileByIndex(int nIndex,SFailedFileInfo& ffi) ;
	bool GetFailedFileByFileID(unsigned uFileID,SFailedFileInfo& ffi) ;
	int GetFailedFileCount()  ;

	int AddObserver(CXCFailedFileObserver* pObserver) ;

	bool IsFailedFileExist(const SFailedFileInfo& ffi) ;
private:
	enum ENotifyType
	{
		NT_Added,
		NT_Update
	};

private:
	inline void Notify(const ENotifyType nt,const SFailedFileInfo& OldFfi) ;
	

private:
	int						m_nCurFailedFileID ; // 当前failed文件的索引ID
	bool					m_bIsCurFailedFolder ; // 当前的failed 是否为文件夹
	CptCritiSecLock					m_FailedVerLock ;
	pt_STL_vector(SFailedFileInfo)	m_FailFileInfoVer ;
	int								m_nMaxFailedFileCount ;

	pt_STL_vector(CXCFailedFileObserver*)			m_ObserverVer ;
};

class CXCFailedFileObserver
{
public:
	virtual void OnFailedFile_Update(const SFailedFileInfo& OldFfi,const pt_STL_vector(SFailedFileInfo)& FailedFileVer) = 0 ;
	virtual void OnFailedFile_Added(const SFailedFileInfo& ffi) {};
};