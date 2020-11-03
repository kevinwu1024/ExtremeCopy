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
#include "XCCoreDefine.h"
#include "XCSourceFilter.h"
#include "XCFileDataBuffer.h"
#include "XCFileChangingBuffer.h"
#include "..\Common\ptThreadLock.h"
#include "XCSameDriveMoveFile.h"

// ExtremeCopy 内核封装类
class CXCCore
{
public:
	CXCCore(void);
	~CXCCore(void);

	bool Run(const SGraphTaskDesc& gtd) ;

	void Stop() ;
	void Pause() ;
	void Continue() ;
	bool Skip() ;

	ECopyFileState GetState()  ;

	CXCFileDataBuffer*	GetFileDataBuf() {return &m_FileDataBuffer;}
	CptString GetCurWillSkipFile() ;

private:
	inline bool NormalRun(const SGraphTaskDesc& gtd) ;
	inline bool ProcessException(CXCCopyingEvent* pEvent) ; // 返回值，true: 重试， false:退出

protected:
	// 
	class CXCGraph
	{
	public:
		CXCGraph();
		~CXCGraph() ;

		void AddFilter(CXCFilter* pFilter) ;
		void Clear() ;

		bool Run() ;
		bool Pause() ;
		void Stop() ;
		bool Skip() ;
		bool Continue() ;
		bool Initialize() ;

		ECopyFileState GetState() const ;
		void Release() ;

		CptString GetCurWillSkipFile() ;

	private:
		pt_STL_list(CXCFilter*) FilterList ;
		CXCSourceFilter*		pSourceFilter ;
	};

	class CXCGraphSet
	{
	public:
		CXCGraphSet() ;
		~CXCGraphSet() ;

		bool Start() ;
		bool Pause() ;
		void Stop() ;
		bool Skip() ;
		bool Continue() ;

		ECopyFileState GetState()  ;
		void AddGraph(CXCGraph& graph) ;
		int GetCount() ;

		CptString GetCurWillSkipFile() ;

	private:
		void Release() ;
		CXCGraph* GetCurGraph() ;

	private:
		pt_STL_list(CXCGraph)	m_GraphList ;
		int		m_nCurIndex ;
	};

private:

	struct SFileStorageInfo
	{
		pt_STL_vector(CptString) strFileOrFolderVer ; // 对于 source 来说，有多个，但对于destination 来说，则只有一个
		SStorageInfoOfFile		siof ;
	};

	struct SLinkDesc
	{
		pt_STL_vector(SFileStorageInfo) SrcLinkVer ;
		pt_STL_vector(SFileStorageInfo) BranchLinkVer ;
	};

private:
	bool ParseTaskToGraphDesc(const SGraphTaskDesc& gtd,SLinkDesc& ld) ;
	bool CreateLinkGraph(const SGraphTaskDesc& gtd,CXCGraphSet& GraphSet) ;
	bool AllocFileDataBuffer(const SGraphTaskDesc& gtd) ;

private:
	CXCGraphSet					m_GraphSet ;
	bool						m_bIni ;
	CXCFileDataBuffer			m_FileDataBuffer ;
	CXCFileChangingBuffer		m_FileChangingBuffer ;
	CptCritiSecLock				m_Lock ; 
	EImpactFileBehaviorResult	m_ImpactFileBehavior ; // 冲突文件的处理方式
	ECopyFileState				m_RunningState ;	// 运行状态
};

