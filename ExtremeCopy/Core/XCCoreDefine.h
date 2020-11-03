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
#include "XCFilterProtocol.h"
#include "..\Common\ptGlobal.h"
#include <deque>

// 错误代码位置
#define ExceptionCodePosition__XCDestnationFilter_CreateFile				1
#define ExceptionCodePosition__XCFileDataCacheTransFilter_OnPin_Data_AllocMem		2

// 错误处理类型
#define ErrorHandlingFlag_Null			0
#define ErrorHandlingFlag_Success		0
#define ErrorHandlingFlag_Retry			(1 << 1)
#define ErrorHandlingFlag_Ignore		(1 << 2)
#define ErrorHandlingFlag_Exit			(1 << 3)
#define ErrorHandlingFlag_List			(1 << 4)
//#define ErrorHandlingFlag_Skip			1 << 3

//#define ErrorHandlingFlag_Exit			1 << 8
#define ErrorHandlingFlag_RetryIgnoreListExit (ErrorHandlingFlag_List|ErrorHandlingFlag_Retry|ErrorHandlingFlag_Ignore|ErrorHandlingFlag_Exit)

#define ErrorHandlingFlag_RetryIgnoreCancel (ErrorHandlingFlag_Retry|ErrorHandlingFlag_Ignore|ErrorHandlingFlag_Exit)
#define ErrorHandlingFlag_IgnoreCancel (ErrorHandlingFlag_Ignore|ErrorHandlingFlag_Exit)
#define ErrorHandlingFlag_RetryCancel (ErrorHandlingFlag_Retry|ErrorHandlingFlag_Exit)

#define ERROR_HANDLE_PROCESS(nResult,RetryLabel) \
	{if((nResult)==ErrorHandlingFlag_Retry) goto RetryLabel ;\
	else return (nResult) ;}

typedef unsigned		ErrorHandlingResult;

// ExtremeCopy App 错误码
enum ECopyFileErrorCode
{
	CopyFileErrorCode_Unknown = -1,
	CopyFileErrorCode_Success = 0,
	CopyFileErrorCode_AppError,
	CopyFileErrorCode_InvaliableFileName,
	CopyFileErrorCode_CanotReadFileData,
	CopyFileErrorCode_WriteFileFailed,
};

// 错误码
struct SXCErrorCode
{
	ECopyFileErrorCode	AppError ;
	int					nSystemError ;

	SXCErrorCode():AppError(CopyFileErrorCode_Unknown),nSystemError(0)
	{
	}
};

// ExtremeCopy 发生异常时的信息体
struct SXCExceptionInfo
{
	SXCErrorCode	ErrorCode ;
	unsigned		uFileID ;
	CptString strSrcFile ;
	CptString strDstFile ;
	int				CodePosition ;
	int				SupportType;

	SXCExceptionInfo():uFileID(0),CodePosition(0),SupportType(0)
	{
	}
};

// ExtremeCopy 任务类型
enum EXCExecuteType
{
	XCTT_Copy = 1,
	XCTT_Move,
	XCTT_Delete
};

// ExtremeCopy 的运行状态
enum ECopyFileState
{
	CFS_Stop = 1,
	CFS_Running = 2,
	CFS_Pause = 3,
	CFS_Exit = 4,
	CFS_ReadyStop = 5, // 该状态为core内部退出时使用，不作外部使用
};

// 冲突文件名时的处理做法
enum EImpactFileBehaviorResult
{
	SFDB_Unkown = -1,
	SFDB_Default = -2, // 即是询问
	SFDB_Skip = 0,
	SFDB_Replace = 1,
	SFDB_Rename = 2,
	SFDB_StopCopy = 3
};

// filter 类型
enum EFilterType
{
	FilterType_Unknown,
	FilterType_Source,
	FilterType_Transform,
	FilterType_Destination
};

// "脚"的类型
enum EPinType
{
	PinType_Unknown,
	PinType_Input,
	PinType_Output
};

// 文件hash类型
enum EFileHashType
{
	FHT_UNKNOWN,
	FHT_MD5,
	FHT_SHA1
};

// 文件开始复制时回调给上层的信息结构体
struct SActiveFilesInfo
{
	unsigned uFileID ;
	unsigned __int64 uFileSize ;

	CptString	strSrcFile ;
	CptString	strDstFile ;
};

class CXCCopyingEvent ;

// Graph 任务的描述信息
struct SGraphTaskDesc
{
	 pt_STL_vector(CptString) SrcFileVer ; // 源文件（带全路径）
	 pt_STL_vector(CptString) DstFolderVer; // 目标文件夹，如果是单

	 EXCExecuteType ExeType ;
	 EFileHashType	HashType ;
	 CXCCopyingEvent*	pCopyingEvent ;
	 int				nFileDataBufSize ;
	 EImpactFileBehaviorResult		ImpactFileBehavior ;
	 bool			bIsRenameDst ;

	 SGraphTaskDesc()
	 {
		 ExeType = XCTT_Copy ;
		 HashType = FHT_UNKNOWN ;
		 pCopyingEvent = NULL ;
		 nFileDataBufSize = 32*1024*1024 ;
		 ImpactFileBehavior = SFDB_Default ;
		 bIsRenameDst = false ;
	 }
};

//// 文件操作完成，对应 EFilterCmd::EDC_FileOperationCompleted
//struct SDataPack_FileOpCompletedInfo
//{
//	unsigned	uID ; 		// 文件ID
//	bool		bDiscard ; 	// 是否抛弃
//
//	SDataPack_FileOpCompletedInfo()
//	{
//		::memset(this,0,sizeof(*this)) ;
//	}
//};

// 复制数据事件
struct SFileDataOccuredInfo
{
	unsigned	uFileID ;
	bool		bReadOrWrite ;
	int			nDataSize ;
};

// 文件复制完成时的回调
struct SFileEndedInfo
{
	unsigned	uFileID ;
	//bool		bReadOrWrite ;
	//CptString	strFileName ;
};

// 冲突的文件类型
enum EImpactFileType
{
	IT_SameFileName, // 相同名字文件
	IT_ReadOnly, // 只读文件
	IT_ExecutableFile, // 可执行文件
};

struct SImpactFileInfo
{
	EImpactFileType ImpactType ;

	CptString strSrcFile ;
	CptString strDestFile ;
};

struct SImpactFileResult
{
	EImpactFileBehaviorResult	result ; // 返回值
	CptString strNewDstFileName ;  // 当用户选择改名时，应提供新名字
	bool bAlways ; // 总是这个结果,如果这值为true，说明用户遇到冲突文件时总是按这个设置处理

	SImpactFileResult():result(SFDB_Default),bAlways(false)
	{
	}
};

// 文件所对应的所在磁盘相关信息
struct SStorageInfoOfFile
{
	int			nSectorSize ;
	int			nPartitionIndex ; // 若为 -1 则非DOS盘
	DWORD		dwStorageID ;	// 存储器的ID值
	unsigned	uDiskType ; // DRIVE_FIXED   参考 ::GetDriveType()
							// DRIVE_REMOTE
							// DRIVE_CDROM 
							// DRIVE_RAMDISK
							// DRIVE_UNKNOWN
							// DRIVE_NO_ROOT_DIR
							// DRIVE_REMOVABLE

	bool IsSameStorage(const SStorageInfoOfFile& siof) const
	{
		//return (siof.nSectorSize==this->nSectorSize && siof.nSotrageIndex>=0 && siof.nSotrageIndex==this->nSotrageIndex && siof.uDiskType==this->uDiskType) ;
	return (siof.uDiskType==this->uDiskType && siof.nSectorSize==this->nSectorSize && siof.dwStorageID>0 && siof.dwStorageID==this->dwStorageID ) ;
	}

	SStorageInfoOfFile()
	{
		nSectorSize = 512 ;
		nPartitionIndex = -1 ;
		//nSotrageIndex = -1 ;
		dwStorageID = 0 ;
		uDiskType = DRIVE_UNKNOWN ;
	}
};

class CXCFileChangingBuffer ;

// source filter 任务信息
struct SXCSourceFilterTaskInfo
{
	pt_STL_vector(CptString) SrcFileVer ;
	EXCExecuteType					ExeType ;
	CXCFileDataBuffer*		pFileDataBuf ;
	CXCFileChangingBuffer*	pFileChangingBuf ;
	int						nValidCachePointNum ; // 文件数据缓冲块被引用次数，也就是后段分支的数目
	EImpactFileBehaviorResult*	pDefaultImpactFileBehavior ; // 默认的冲突文件处理方法，用作 destination filter 以及在 XCCore处调用的MoveFileEx所用的
	ECopyFileState*				pRunningState ;
};

class CXCCopyingEvent ;
class CXCPin ;


class CXCFilterEventCB
{
public:
	virtual int OnDataTrans(CXCFilterEventCB* pSender,EFilterCmd cmd,void* pFileData) = 0 ;
};


class CXCFilter : public CXCFilterEventCB
{
public:
	CXCFilter(CXCCopyingEvent* pEvent=NULL):m_pEvent(pEvent),m_pRunningState(NULL),m_pFileChangingBuffer(NULL) {}

	virtual ~CXCFilter() {}

	virtual EFilterType GetType() const {return FilterType_Unknown ;}
	virtual bool Initialize() {return true ;}
	inline bool IsValideRunningState() const {return (m_pRunningState!=NULL && *m_pRunningState!=CFS_Stop && *m_pRunningState!=CFS_Exit && *m_pRunningState!=CFS_ReadyStop);}

	virtual bool Connect(CXCFilter* pFilter,bool bUpstream) =0 ;


protected:
	struct SXCAsynOverlapped
	{
		OVERLAPPED ov ;
		DWORD		dwOperSize ;
		void*		pBuf ;
		int			nBufSize ;
	};
	virtual bool OnInitialize() {return true;}
	virtual bool OnPause() {return true ;}
	virtual bool OnContinue() {return true ;}
	virtual void OnStop() {}
	
protected:
	CXCCopyingEvent*	m_pEvent ;
	ECopyFileState*		m_pRunningState ;
	CXCFileChangingBuffer*	m_pFileChangingBuffer ;
	//ECopyFileState		m_RunningState ;
};


//======================================================= 华丽的分界线 =================


enum EOperationResult
{
	OperationResult_Success = 0 ,
	OperationResult_Cancel,
	OperationResult_Ignore,
	OperationResult_TryAgain,
};

enum ECopyBehavior
{
	CopyBehavior_ReplaceUncondition = 7, // 当相同文件名时,覆盖它
	CopyBehavior_SkipCondition, // 当相同文件名条件符合时,覆盖它
	CopyBehavior_Rename, // 自动改名,
	CopyBehavior_AskUser, // 询问用户,该如何操作
};

enum EInterestFileType
{
	IFT_Invalid,
	//IFT_Unknown,
	IFT_File,
	IFT_Folder,
	IFT_FolderWithWildcard
};

#define SameFileCondition_SameSize				(1<<1) 
#define SameFileCondition_SameCreateTime		(1<<2)
#define SameFileCondition_SameModifyTime		(1<<3)


class CXCTransObject ;

struct STransformDataDesc
{
	void*	pAllocBuf ;
	int		nAllocSize ;
	void*	pDataBuf ;
	int		nBufSize ;
	DWORD	nOperatorSize ;
	DWORD	nOffset ;
	void*	pExtra ;

	STransformDataDesc()
	{
		::memset(this,0,sizeof(STransformDataDesc)) ;
	}
};

struct SFileOrDirectoryDiskInfo
{
	unsigned __int64 nFileSize ;
	int nPartitionIndex ;
	int nHardDriverIndex ;
	int nSector ;
	int nDriverType ;
	DWORD dwFileAttr ;
	CptString strFileOrDirectory ;

	SFileOrDirectoryDiskInfo()
	{
		dwFileAttr = INVALID_FILE_ATTRIBUTES ; // GetFileAttributes
		nDriverType = DRIVE_UNKNOWN ; // GetDriveType
		nSector = 512 ;
		nHardDriverIndex = 0 ;
		nPartitionIndex = 0 ;
		nFileSize = 0 ;
	}

};


bool IsContainWildcardChar(const TCHAR* pStr) ;
void RenameNewFileOrFolder(CptString& strFileOrFolder) ;
//bool IsFolder(const TCHAR* lpDir) ;
EInterestFileType GetInterestFileType(const TCHAR* pFileName) ;
CptString GetRawFileName(CptString strFullFileName) ;