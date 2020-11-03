/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "XCTransformFilter.h"
#include <vector>
#include "..\Common\ptTypeDef.h"
#include "..\Common\ptThreadLock.h"
#include <map>

// 一路输入，多路输出
// 这是一个代理模式
class CXCDuplicateOutputTransFilter
	: public CXCTransformFilter
{
public:
	CXCDuplicateOutputTransFilter(CXCCopyingEvent* pEvent);
	virtual ~CXCDuplicateOutputTransFilter(void);

	virtual bool Connect(CXCFilter* pFilter,bool bUpstream) ;
protected:
	
	virtual int OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData) ;

	virtual bool OnInitialize() ;
	virtual bool OnContinue() ;
	virtual bool OnPause();
	virtual void OnStop();

private:
	void TransDataToDownStream(EFilterCmd cmd,void* pFileData) ;
private:
	CXCFilterEventCB*						m_pUpstreamFilter ;
	pt_STL_vector(CXCFilterEventCB*)		m_DownstreamFilterVer ;
	int										m_nBranchNum ;

	pt_STL_map(unsigned,int)	m_FileDoneConfirmHashMap ;
	CptCritiSecLock					m_Lock ;
};

