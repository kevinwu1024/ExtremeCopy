
#pragma once

#include "..\Core\XCCore.h"
#include "..\Core\XCCopyingEvent.h"
#include <vector>
#include <list>
#include "..\Core\XCCoreDefine.h"
#include "..\Common\ptTypeDef.h"

#define ROUTINE_CMD_BEGINONEFILE		1	// 开始复制一文件
#define ROUTINE_CMD_FINISHONEFILE		2	// 结束复制一文件
#define ROUTINE_CMD_FILEFAILED			3	// 失败的文件
#define ROUTINE_CMD_SAMEFILENAME		4	// 文件名相同
#define ROUTINE_CMD_DATAWROTE			5	// 数据写入成功
#define ROUTINE_CMD_TASKFINISH			6	// 复制完成


#define START_ERROR_CODE_SUCCESS					0 // 成功
#define START_ERROR_CODE_INVALID_DSTINATION			1 // 无效的目标文件夹
#define START_ERROR_CODE_INVALID_SOURCE				2 // 无效的源文件
#define START_ERROR_CODE_INVALID_RECURISE			3 // 递归包含文件夹
#define START_ERROR_CODE_CANOT_LAUNCHTASK			4 // 当前状态不允许启动新的复制任务
#define START_ERROR_CODE_UNKNOWN					999 // 未知错误

//#define ROUTINE_CMD_STATECHANGED		6	// 状态发生变化

#define XCRunType_Copy		1
#define XCRunType_Move		2

typedef int (__stdcall *lpfnCopyRoutine_t)(int nCmd,int nParam1,int nParam2,const TCHAR* pSrcStr,const TCHAR* pDstStr) ;

class CExtremeCopyApp : public CXCCopyingEventReceiver
{
public:

	CExtremeCopyApp() ;
	~CExtremeCopyApp() ;

	void SetLicenseKey(CptString strSN) ;

	void Stop() ;
	bool Pause() ;
	bool Continue() ;
	int Start(int nRunType,bool bSyncOrAsync) ;
	ECopyFileState GetState() ;
	int AttachSrc(const CptString strSrcFile) ;
	int AttachDst(const CptString strDstFile) ;

	bool SetDestinationFolder(const CptString strDstFolder) ;
	void SetRoutine(lpfnCopyRoutine_t) ;
	int SetCopyBufferSize(int nBufSize) ;
protected:

	int CheckTaskParameter() ;
		// 复制回调事件
	virtual int OnCopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1=NULL,void* pParam2=NULL) ;

	bool DoesIncludeRecuriseFolder(const pt_STL_vector(CptString)& SrcVer,const pt_STL_vector(CptString)& DstVer,int& nSrcIndex, int& nDstIndex) ;

	bool IsDstRenameFileName(const pt_STL_vector(CptString)& SrcVer,const pt_STL_vector(CptString)& DstVer)  ;

	inline void RemoveCityFromActiveList(unsigned uCityID) ;
	void PostBeginOneFileCommand(const SActiveFilesInfo& afi) ;

private:
	static unsigned int __stdcall CopyThreadFunc(void*) ;
	void CopyWork(bool bCopyOrMove) ;

	struct SThreadParamData
	{
		CExtremeCopyApp*	pThis ;
		bool				bCopyOrMove ;
	};

	//class CActiveFileComparetor
	//{
	//public:
	//	bool operator ()(const SActiveFilesInfo& afi)
	//	{
	//		return (afi.uFileID==m_uFileID) ;
	//	}
	//	
	//	void SetCompareFileID(unsigned uFileID)
	//	{
	//		m_uFileID = uFileID ;
	//	}
	//private:
	//	unsigned int	m_uFileID ;
	//}  ;

private:
	CXCCore					m_XCCore ;
	CXCCopyingEvent			m_XCCopyEvent ;

	pt_STL_vector(CptString)		m_strSrcVer ;
	pt_STL_vector(CptString)	m_strDstFolderVer ;

	lpfnCopyRoutine_t			m_lpfnRoutine ;

	HANDLE						m_hThread ;
	CptString					m_strSeriesNumber ;

	bool					m_bIni ;

	pt_STL_list(SActiveFilesInfo)		m_ActiveFileInfoList ;
	CptCritiSecLock			m_FdoListLock ;
	unsigned int				m_uCurFileID ;

	//CActiveFileComparetor			m_ActiveFileComparetor ;
	int							m_nBufSize ;
	unsigned __int64			m_uCurFileWrittenSize ;
};