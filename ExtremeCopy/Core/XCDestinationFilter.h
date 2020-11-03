/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "xccoredefine.h"
#include "XCPin.h"
#include <deque>
#include <map>

class CXCDestinationFilter : public CXCFilter
{
public:
	CXCDestinationFilter(CXCCopyingEvent* pEvent,bool bIsRenameCopy);
	virtual ~CXCDestinationFilter(void);

	virtual EFilterType GetType() const
	{
		return FilterType_Destination ;
	}

	static void ResetCanCallbackMark() ;

protected:
	virtual bool OnInitialize() ;
	virtual bool OnContinue() ;
	virtual bool OnPause();
	virtual void OnStop();

	inline const bool IsRenameExe() const {return m_bIsRenameCopy;}

	inline bool CanCallbackFileInfo() const ;

	class CXCDirectoryURI
	{
	public:
		CXCDirectoryURI() ;
		void Clean() ;
		void Push(CptString strFolderName) ;
		void Pop() ;
		CptString GetURI() ;

	private:
		CptString				m_strLastURI ;
		pt_STL_deque(CptString)	m_URIDeque ;
		bool					m_bNewestURI ;
	};

	class CCreateDestFileCache
	{
	public: 
		bool CanAdd(CptString strPath,const SDataPack_SourceFileInfo& sfi) ;
		void Push(const SDataPack_SourceFileInfo& sfi) ;
		SDataPack_SourceFileInfo* Pop() ;
		bool IsEmpty() const ;

	private:
		CXCDirectoryURI	m_URI ;
	};

private:
	int				m_nDstFilterID ; // 该ID值是为了,当有多个dstination filter时,只有第一个filter可以向上回调数据
	static	int		m_sDstFilterIDCounter ;

	bool			m_bIsRenameCopy ; // 是否以 ‘改名’来复制，类似于CRT的 rename()
};

class CXCLocalFileDestnationFilter :
	public CXCDestinationFilter
{
public:
	CXCLocalFileDestnationFilter(CXCCopyingEvent* pEvent,const CptString strDestRoot,const SStorageInfoOfFile& siof,const bool bIsRenameCopy);
	virtual ~CXCLocalFileDestnationFilter(void);

	virtual bool Connect(CXCFilter* pFilter,bool bUpstream) ;

protected:
	struct SDstFileInfo
	{
		HANDLE				hFile ;
		bool				bNoBuf ;
		unsigned __int64	uRemainSize ; // 这个不一定是文件实际的大小，当不使用缓冲时则以sector大小的上对齐大小，
										// 当使用缓冲时则为文件的实际大小
		
		//unsigned __int64 uActFileSize ; // 文件的实际大小,以供完成的调整之用
		CptString strFileName ; // 全路径文件名
		SDataPack_SourceFileInfo*	pSfi ;

		SDstFileInfo()
		{
			hFile = INVALID_HANDLE_VALUE ;
			bNoBuf = false ;
			uRemainSize = 0 ;
			strFileName = _T("") ;			
			pSfi = NULL ;
		}
	};

	virtual int OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData) ;
	virtual int OnCreateXCFile(SDataPack_CreateFileInfo& cfi) ;
	int CreateXCFile(SDataPack_SourceFileInfo& sfi,CptString& strDstFile)  ;
	virtual int WriteFileData(SDataPack_FileData& fd)  = 0 ;

	virtual void OnLinkEnded(pt_STL_list(SDataPack_SourceFileInfo*)& FileList) {}

	// 当一个ROUND的文件复制完成后,就做这个ROUND的扫尾工作
	void RoundOffFile(pt_STL_list(SDataPack_SourceFileInfo*)& FileList ) ;

	SDstFileInfo* AllocDFI() ;
	void FreeDFI(SDstFileInfo* dfi) ;

protected:

	virtual bool OnInitialize() ;
	virtual bool OnContinue() ;
	virtual bool OnPause();
	virtual void OnStop();

	ErrorHandlingResult GetErrorHandleResult(SDataPack_SourceFileInfo& sfi,CptString strDestFile) ;

private:
	inline CptString MakeFileFullName(CptString strRawFileName) ;

	HANDLE m_hCurFileHandle ;

protected:
	
	CXCFilterEventCB*						m_pUpstreamFilter ;
	CXCDirectoryURI							m_DestURI ;

	pt_STL_list(SDstFileInfo)				m_FolderInfoList ; // 文件夹的删除是以创建的反顺序进行，即是栈的顺序
	pt_STL_list(SDstFileInfo)				m_FileInfoList ; // 写入的文件信息，这里是以顺序创建和写入
	pt_STL_list(SDstFileInfo)::iterator		m_CurFileIterator ; // 当前正在操作的文件指针，
																// 该指针是个分界点，指针前的 m_FileInfoList 元素为已写入到磁盘的文件等待 round off 的
																// 指针后的 m_FileInfoList 元素为创建了，但还没写入到磁盘的文件

	
	CptString		m_strDestRoot ;
	bool			m_bIsDriverRoot ; // m_strDestRoot 是否为根目录	
	int				m_nSwapChunkSize ;
	SStorageInfoOfFile m_StorageInfo ;
	DWORD							m_nCreateFileFlag ;

	CXCFileDataBuffer*			m_pFileDataBuf ;
	EImpactFileBehaviorResult*	m_pImpactFileBehavior ; // 冲突文件的处理方式
};