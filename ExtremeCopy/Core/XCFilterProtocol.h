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
#include "../Common/ptString.h"
#include <list>
#include "../Common/ptTypeDef.h"
#include "../Common/ptThreadLock.h"
#include "../Common/ptWinPath.h"

class CXCFilter ;

enum EFileHashType ;


// 文件数据命令
enum EFilterCmd
{// S: source filter; D: destination filter; T: transform filter

	// 功能：新的FILTER LINK，用作清除目标文件的 URI 栈.
	// 方向：(S->D)。
	// 缓冲：否
	// 数据包： SDataPack_LinkIni
	// 返回值： 无
	EDC_LinkIni,	

	// 功能：把从暂停状态的 FILTER 重新运行起来
	// 方向：(S->D)。
	// 缓冲：否
	// 数据包： 无
	// 返回值： 无
	EDC_Continue,

	// 功能：把从暂停状态的 FILTER 重新运行起来
	// 方向：(S->D)。
	// 缓冲：否
	// 数据包： 无
	// 返回值： 无
	EDC_Pause,

	// 功能：停止 FILTER 所有工作，释放相关资源
	// 方向：(S->D)。
	// 缓冲：否
	// 数据包： 无
	// 返回值： 无
	EDC_Stop,

	// 功能：跳过当前正在复制的文件。该命令只是destinatin filter作处理，其它filter不处理
	// 方向：(S->D)。
	// 缓冲：否
	// 数据包： 无
	// 返回值： 无
	EDC_Skip,

	//// 功能：请求 destination filter 检查该文件名
	//// 方向：(S->D)
	//// 缓冲：否
	//// 数据包：SDataPack_CheckFileInfo
	//// 返回值：成功： ErrorHandlingFlag_Success; 忽略：ErrorHandlingFlag_Ignore; 退出：ErrorHandlingFlag_Exit
	//EDC_CheckFile,

	// 功能：请求 destination filter 创建文件,该命令对destination filter不是串行而来的，
	//			也就是说，当前有好几份文件还没完全写入磁盘，该命令就会过来要求创建新的文件。
	//			当 SDataPack_SourceFileInfo::
	// 方向：(S->D)
	// 缓冲：是
	// 数据包：SDataPack_SourceFileInfo
	// 返回值：成功： ErrorHandlingFlag_Success; 忽略：ErrorHandlingFlag_Ignore; 退出：ErrorHandlingFlag_Exit
	//EDC_CreateFile,	

	// 功能：请求 destination filter 创建文件,该命令对destination filter不是串行而来的，
	//			也就是说，当前有好几份文件还没完全写入磁盘，该命令就会过来要求创建新的文件。
	//			该命令不能被transfer filter 缓冲起来，而在返回时就应该知道创建结果，要么成功，要么失败
	// 方向：(S->D)
	// 缓冲：是
	// 数据包：SDataPack_CreateFileInfo
	// 返回值：成功： ErrorHandlingFlag_Success; 忽略：ErrorHandlingFlag_Ignore; 退出：ErrorHandlingFlag_Exit
	EDC_BatchCreateFile,	

	// 功能：(未使用，作保留) 文件信息
	// 方向：(S->D)
	// 数据包: SDataPack_SourceFileInfo
	// 返回值： 无
	//EDC_FileInfo,   

	// 功能：文件数据, 该命令在串行transfer filter里将被缓冲下来而不向destination filter传下去，直到收到 EDC_FlushData 命令才将以这个命令向下传
	//			若该命令在并行transfer filter里则直接向destination filter传下去
	//			当数据包参数为NULL时，说明上游处空闲，下游处可以做些空闲活
	//			当SDataPack_FileData成员变量pData==NULL，那么说明让下游知道该文件被抛弃
	// 方向：(S->T) （T->D）
	// 缓冲：是
	// 数据包：SDataPack_FileData
	// 返回值：成功： ErrorHandlingFlag_Success; 忽略：ErrorHandlingFlag_Ignore; 退出：ErrorHandlingFlag_Exit
	EDC_FileData, 

	// 功能： 要求各个具有文件数据缓冲功能的FILTER把缓冲区的数据写入存储器里
	// 方向：(S->T)
	// 缓冲：是
	// 数据包：无
	// 返回值：成功： ErrorHandlingFlag_Success; 忽略：ErrorHandlingFlag_Ignore; 退出：ErrorHandlingFlag_Exit
	EDC_FlushData, 

	// 功能： 文件的HASH值
	// 方向：（T->D）
	// 缓冲：是
	// 数据包: SDataPack_FileHash
	// 返回值：(待定)
	EDC_FileHash, 

	//EDC_InvalidBranch, //(未作用，作保留) (D->S)因为有多个写入分支，若其中一个分支写入失败且废弃，则写入的Filter返回这个命令回source filter

	// 功能： 作用于该源文件的操作已完成， destination filter 应返回 EDC_FileDoneConfirm 命令，
	//			因为transfer filter具有缓冲作用，所以 EDC_FileDoneConfirm 命令将会是以异步返回
	// 方向：（S->T->D）
	// 缓冲：是
	// 数据包： SDataPack_FileOperationCompleted
	// 返回值： （待定）
	EDC_FileOperationCompleted, 

	// 功能： 若destination filter发现写入文件部分或全部失败，可以要求source filter重发部分或全部数据
	// 方向： （D->S）
	// 数据包： SDataPack_ResendFileData
	// 返回值：(待定)
	EDC_ResendFileData,

	// 功能： 当文件数据已成功写入磁盘，则发送该命令到 transfer filter
	// 方向： （D->S）
	// 数据包： SDataPack_FileDataIOComplete
	// 返回值：(待定)
	//EDC_DstFileDataIOComplete,

	// 功能： destination filter 确认已成功写入了。当destination filter写入介质成功后给source filter返回的确认，致使source filter关闭该文件资源
	// 方向： （D->S）
	// 数据包：SDataPack_FileDoneConfirm
	// 返回值： 无
	EDC_FileDoneConfirm, // (D->S)文件写入完成 . 
	
	// 功能： 作用于该文件夹的子文件和文件夹操作已完成
	// 方向： （S->D）
	// 数据包：无
	// 返回值：无
	//EDC_FolderChildrenOperationCompleted,

	// 功能： 该link的所有操作已完成
	// 方向： （S->D）
	// 数据包：SDataPack_FileOperationCompleted
	// 返回值：无
	EDC_LinkEnded
};

class CXCFileDataBuffer ;
class CXCFileChangingBuffer ;
enum ECopyFileState ;
enum EImpactFileBehaviorResult ;

// 初化LINK的数据包
struct SDataPack_LinkIni
{
	CXCFileDataBuffer*		pFileDataBuf ;
	ECopyFileState*			pCFState ;
	CXCFileChangingBuffer*	pFileChangingBuf ;
	EImpactFileBehaviorResult*	pDefaultImpactFileBehavior ; // 默认的冲突文件处理方法，用作 destination filter 以及在 XCCore处调用的MoveFileEx所用的
};

// filter 的 LINK 信息
struct SFilterLinkInfo
{
	unsigned	uID ;
	pt_STL_list(CXCFilter*)	FilterList ;
};

// 要复制的"源文件"信息, 对应 EFilterCmd::EDC_FileInfo
// 该信息体在source filter 打开文件时创建，当source filter向 destinatin filter发送 
// EDC_CreateFile 和 EDC_FileOperationCompleted 时将携带该信息体， 因为 EDC_FileOperationCompleted
// 有缓冲功能，所以该信息体即使在 source filter 已全部读取文件数据后也不会即时释放，而是加入在 “确认完成缓冲队列”
// 里，直到 source filter 收到来自 EDC_FileDoneConfirm 命令时才释放回缓冲区 。
// 但该信息体也会在一些情况中途释放，例如： 该文件在中途被“抛弃”
struct SDataPack_SourceFileInfo
{
#define DP_SFI_PROPERTY_MASK_LOCAL				1<<0 // 是否为本地文件
#define DP_SFI_PROPERTY_MASK_DISCARD			1<<1 // 是否抛弃文件
#define DP_SFI_PROPERTY_MASK_ERROR				1<<3 // 是否为出错文件


	unsigned			uFileID ;		// 文件ID
	CptString			strSourceFile ; // 源文件名（包含根路径）
	CptString			strNewFileName ; // 新的文件名，如果不为空，则优先使用该文件名作为destination的文件名
										// 该文件名为 raw 文件名
	DWORD				dwSourceAttr ; // 文件的属性
	SFileTimeInfo		SrcFileTime ; // 文件时间
	unsigned __int64	nFileSize ; // 文件大小
	int					nSysErrCode ; // 在 source filter 处发生的error code
	BYTE				byProperty ;

	static pt_STL_list(SDataPack_SourceFileInfo*) CacheList ; // 这个用作内存块重复使用
	static CptCritiSecLock			m_ThreadLock ;

	SDataPack_SourceFileInfo()
	{
		this->Ini() ;
	}

	bool IsDiscard() const
	{
		return (byProperty&DP_SFI_PROPERTY_MASK_DISCARD) ? true : false ;
	}

	bool IsError() const
	{
		return (byProperty&DP_SFI_PROPERTY_MASK_ERROR) ? true : false ;
	}

	bool IsLocal() const
	{
		return (byProperty&DP_SFI_PROPERTY_MASK_LOCAL) ? true : false ;
	}

	void SetDiscard(const bool bDiscard=true)
	{
		if(bDiscard)
		{
			byProperty |= DP_SFI_PROPERTY_MASK_DISCARD ;
		}
		else
		{
			byProperty &= ~DP_SFI_PROPERTY_MASK_DISCARD ;
		}
		
	}

	void SetError(const bool bError=true)
	{
		if(bError)
		{
			byProperty |= DP_SFI_PROPERTY_MASK_ERROR ;
		}
		else
		{
			byProperty &= ~DP_SFI_PROPERTY_MASK_ERROR ;
		}
		
	}

	void Ini()
	{
		uFileID = 0 ;
		strSourceFile = _T("") ;
		dwSourceAttr = 0 ;
		nFileSize = 0 ;
		byProperty = 0 ;
		nSysErrCode = 0 ;

		byProperty |= DP_SFI_PROPERTY_MASK_LOCAL ;
	}

	static SDataPack_SourceFileInfo* Allocate()
	{
		SDataPack_SourceFileInfo* pRet = NULL ;

		CptAutoLock lock(&m_ThreadLock) ;
		if(!CacheList.empty())
		{
			pRet = CacheList.front();
			CacheList.pop_front() ;
		}
		else
		{
			pRet = new SDataPack_SourceFileInfo() ;
		}

		return pRet ;
	}

	static void Free(SDataPack_SourceFileInfo* pFpi)
	{
		if(pFpi!=NULL)
		{
			pFpi->Ini() ;

			CptAutoLock lock(&m_ThreadLock) ;
			CacheList.push_back(pFpi) ;
		}
	}

	static void ReleaseBuffer()
	{
		CptAutoLock lock(&m_ThreadLock) ;

		pt_STL_list(SDataPack_SourceFileInfo*)::iterator it = CacheList.begin() ;

		for(;it!=CacheList.end();++it)
		{
			if((*it)!=NULL)
			{
				delete (*it) ;
			}
		}

		CacheList.clear() ;
	}
};

// 用于 EDC_BatchCreateFile 命令的请求
struct SDataPack_CreateFileInfo
{
	pt_STL_list(SDataPack_SourceFileInfo*) SourceFileInfoList ; // 要创建的文件
	//bool bExistDiscard ; // 是否存在要被抛弃的文件或文件夹，这是输出参数，由destination filter填写的
	//pt_STL_vector(CptString)	DiscardFolderVer ; // 被抛弃的文件夹，这是输出参数，由destination filter填写的
};

//检测该文件名在 
struct SDataPack_CheckFileInfo
{
	CptString	strFileName ; // 不包含文件路径
	bool		bFileOrFolder ;
	bool		bDiscard ;
};

// 文件数据, 对应 EFilterCmd::EDC_FileData
struct SDataPack_FileData
{
	unsigned	uFileID ;
	//unsigned __int64	uReadBeginPos ; // 读的开始位置
	//unsigned __int64	uWriteBeginPos ; // 写的开始位置
	int			nDataSize ;	// 有效数据大小
	int			nBufSize ; // nBufSize 与 nDataSize 不一定一致，当文件尾段的长度不是sector的倍数时，就会出现不一致
							// 这里之所以再开一个变量记录，是因为防止 XCFileDataBuffer alloc时是BufSize,free时是DataSize
							// 这样就能够防止空间泄漏
	void*		pData ;		// 文件数据
	//bool		bDiscard ;

	SDataPack_FileData()
	{
		::memset(this,0,sizeof(*this)) ;
	}
};

// 作用于该文件操作已完成, 对应于 EFilterCmd::EDC_FileOperationCompleted
struct SDataPack_FileOperationCompleted
{
	pt_STL_list(SDataPack_SourceFileInfo*)	CompletedFileInfoList ;
};

// 文件HASH值，对应 EFilterCmd::EDC_FileHash
struct SDataPack_FileHash
{
	unsigned		uFileID ;			// 文件ID值
	EFileHashType	HashType ;		// HASH值类型
	int				nDataSize ; 	// HASH有效数据大小
	void*			pHashValue ; 	// HASH数据

	SDataPack_FileHash()
	{
		::memset(this,0,sizeof(*this)) ;
	}
};

// 重新发送指定文件的数据， 对应命令 EDC_ResendFileData
struct SDataPack_ResendFileData
{
	unsigned uFileID ;
	CptString		strFileNameWithURI ;
	CptString		strDestRoot ;
	unsigned __int64	uFileBeginPos ;
};

// 对应命令 EDC_FileDoneConfirm
struct SDataPack_FileDoneConfirm
{
	pt_STL_list(unsigned) FileDoneConfirmList ; // 批量的file ID
	//unsigned uFileID ;
};