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

#include "..\Core\XCCoreDefine.h"
#include "..\Core\XCFileChangingBuffer.h"

class CXCVerifyExceptionCB ;


class CXCVerifyResult
{
public:
	CXCVerifyResult(void);
	~CXCVerifyResult(void);

	struct SVerifyPar
	{
		pt_STL_vector(CptString) SrcVer ;
		pt_STL_vector(CptString) DstVer ;
		CXCFileChangingBuffer*	pChangingBuf ;
		CXCVerifyExceptionCB*	pExceptionCB ;
		BYTE* pBuf ;
		int nBufSize ;

		SVerifyPar():pChangingBuf(NULL),pExceptionCB(NULL),pBuf(NULL),nBufSize(0)
		{
		}
	};

	enum EFileDiffType
	{
		FDT_Unknown,
		FDT_Same,
		FDT_TimeDiff,
		FDT_AttrDiff,
		FDT_ContentDiff,
		FDT_DstFileInexist,
		FDT_SrcFileInexist,
	};

	bool Run(SVerifyPar vp) ;
	void Pause() ;
	void Stop() ;
	void Continue() ;

private:
	ECopyFileState Compare(const SVerifyPar* pVP,int nSrcIndex,const pt_STL_vector(SStorageInfoOfFile)& DstSiofVer) ;
	//bool CompareFile_SameHD(CptString strSrc, CptString strDst) ;
	bool CompareFile(const SVerifyPar& pVP,const CptString& strSrc,const pt_STL_vector(CptString)& strDstVer,const WIN32_FIND_DATA& SrcWfd) ;
	void CompareFolder(const SVerifyPar& vp,CptString strSrc, const pt_STL_vector(CptString)& strDstVer) ;
	
	static void __stdcall FileIOCompletionRoutine2(DWORD dwErrorCode,DWORD dwNumberOfBytesTransfered,OVERLAPPED* lpOverlapped) ;

	inline bool IsStop() const {return (m_RunningState==CFS_Stop || m_RunningState==CFS_Exit);}

	inline void CloseFileHandle() ;
	inline void CheckPauseThenWait() ;

	inline EFileDiffType CompareAttr(const WIN32_FIND_DATA& wfd1,const WIN32_FIND_DATA wfd2) ;
	
private:
	struct SXCAsynOverlapped
	{
		OVERLAPPED ov ;
		CXCVerifyExceptionCB*	pExceptionCB ;
		int			nFileHandleIndex ;
		unsigned __int64	uTotalFileSize ;
		DWORD		dwOperSize ;
		void*		pBuf ;
		int			nBufSize ;
		DWORD		dwCRC32 ;
		//CXCVerifyResult*	pThis ;

#ifdef _DEBUG
		TCHAR*		pSrcFile ;
		TCHAR*		pDstFile ;
#endif
	};

	struct SDstInfo
	{
		CptString strFile ;
		WIN32_FIND_DATA wfd ;
		EFileDiffType fdt ;
	};

	
	struct SReadFileInfo
	{
		HANDLE	hFile ;
		BYTE*	pBuf ;
		int		nBufSize ;

		SXCAsynOverlapped ov ;
	};

private:
	HANDLE					m_hThread ;

	pt_STL_list(HANDLE)		m_ReadFileHandleList ;

	HANDLE					m_hReadEvent1 ;
	HANDLE					m_hPauseWait ;
	HANDLE					m_hBeginReadEvent ;

	int						m_nReadCount ;

	DWORD					m_nBufSize ;

	unsigned __int64		m_uFileSize ;
	DWORD					m_dwSrcDataSize ;
	DWORD					m_dwDstDataSize ;

	BYTE*					m_pSrcBuf ;
	BYTE*					m_pDstBuf ;

	BYTE*					m_pBlockBuf ;
	int						m_nBlockSize ;

	ECopyFileState			m_RunningState ;
	CptCritiSecLock			m_Lock ;
	bool					m_bSame ;

	HANDLE					m_hAPCEventArray[MAXIMUM_WAIT_OBJECTS] ;
	int						m_nValidAPCEventNum ;
};

class CXCVerifyExceptionCB
{
public:
	virtual ECopyFileState OnVerifyFileDiff(CptString strSrc,CptString strDst,CXCVerifyResult::EFileDiffType fdt) = 0;
	virtual void OnVerifyProgress(const CptString& strSrc,const CptString& strDst,const WIN32_FIND_DATA* pWfd) {}
	virtual void OnVerifyProgressBeginOneFile(const CptString& strSrc,const CptString& strDst) {}
	virtual void OnVerifyProgressDataOccured(const DWORD& uFileSize) {}
};