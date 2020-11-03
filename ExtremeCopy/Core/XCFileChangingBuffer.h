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
#include <string>
#include <map>
#include "XCCoreDefine.h"

class CXCFileChangingBuffer
{
public:
	CXCFileChangingBuffer() ;

	enum EFileChangingType
	{
		FileChangingType_Normal,
		FileChangingType_Skip,
		FileChangingType_Rename 
	};

	struct SFileChangingStatusResult
	{
		EFileChangingType  fct ;
		CptString			strDstFileName ;
		CptString			strSrcFileName ;
	};

	bool IsEmpty() const ;
	bool GetChangingStatus(CptString strSrcFile,SFileChangingStatusResult& fcsr,bool bRemove) ;
	bool AddChangingStatus(CptString strSrcFile,const SFileChangingStatusResult& fcsr) ;

	int GetMaxCount(const EFileChangingType fct) const ;
	int GetCount() const ;
	int SetMaxCount(const EFileChangingType,int nNewMax) ;

private:
	//pt_STL_deque(SFileChangingStatusResult) m_FileChangingBuf ;
	pt_STL_map(std::basic_string<TCHAR>,SFileChangingStatusResult) m_FileChangingMap ;

	int		m_nMaxSkipCount ;
	int		m_nMaxRenameCount ;

	int		m_nCurSkipCount ;
	int		m_nCurRenameCount ;
};
