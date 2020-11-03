/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCVerifyResult.h"
#include "..\Common\ptGlobal.h"
#include "..\Core\XCWinStorageRelative.h"
#include "../Common/ptDebugView.h"

CXCVerifyResult::CXCVerifyResult(void):m_RunningState(CFS_Stop),m_hThread(NULL),
	m_hReadEvent1(NULL),m_hBeginReadEvent(NULL),m_hPauseWait(NULL),m_nValidAPCEventNum(0)
{
	::memset(m_hAPCEventArray,0,sizeof(m_hAPCEventArray)) ;
}


CXCVerifyResult::~CXCVerifyResult(void)
{
	this->Stop() ;
}

CXCVerifyResult::EFileDiffType CXCVerifyResult::CompareAttr(const WIN32_FIND_DATA& wfd1,const WIN32_FIND_DATA wfd2)
{
	if(wfd1.dwFileAttributes!=wfd2.dwFileAttributes)
	{// 属性不一致
		//if((wfd1.dwFileAttributes | FILE_ATTRIBUTE_COMPRESSED) !=
		//	(wfd2.dwFileAttributes | FILE_ATTRIBUTE_COMPRESSED))
		//{
		//	return FDT_AttrDiff;
		//}
	}

	if(!CptGlobal::IsFolder(wfd1.dwFileAttributes)
		&& ::memcmp(&wfd1.ftLastWriteTime,&wfd2.ftLastWriteTime,sizeof(FILETIME))!=0)
	{
		return FDT_TimeDiff ;
	}

	 return FDT_Same ;
}

bool CXCVerifyResult::CompareFile(const SVerifyPar& pVP,const CptString& strSrc,const pt_STL_vector(CptString)& strDstVer,const WIN32_FIND_DATA& SrcWfd) 
{
	bool bRet = false ;

	int nAveBufSize = (int)(m_nBlockSize/(strDstVer.size()+1));

	nAveBufSize = ALIGN_SIZE_DOWN(nAveBufSize,4*1024) ;

	pt_STL_vector(SReadFileInfo) FileHandleVer ;

	int nBufPosOffset = 0 ;

	SReadFileInfo rfi ;

	//OutputDebugString(_T("CXCVerifyResult::CompareFile() 1 \r\n")) ;

	{
		CptAutoLock lock(&m_Lock) ;
		m_ReadFileHandleList.clear() ;
	}

	::memset(&rfi,0,sizeof(rfi)) ;

	rfi.hFile = ::CreateFile(strSrc.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_OVERLAPPED,NULL) ;

	//OutputDebugString(_T("CXCVerifyResult::CompareFile() 3 \r\n")) ;

	if(rfi.hFile!=INVALID_HANDLE_VALUE)
	{
		//OutputDebugString(_T("CXCVerifyResult::CompareFile() 4 \r\n")) ;

		{
			CptAutoLock lock(&m_Lock) ;
			m_ReadFileHandleList.push_back(rfi.hFile) ;
		}

		rfi.pBuf = m_pBlockBuf ;
		rfi.nBufSize = nAveBufSize ;

		FileHandleVer.push_back(rfi) ;

		// 如果APC 事件不够,则创建
		while(m_nValidAPCEventNum<(int)strDstVer.size()+1)
		{
			m_hAPCEventArray[m_nValidAPCEventNum] = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;
			
			//m_hAPCEventVer.push_back(hEvent) ;
			++m_nValidAPCEventNum ;
		}

		//OutputDebugString(_T("CXCVerifyResult::CompareFile() 5 \r\n")) ;

		for(size_t i=0;i<strDstVer.size();++i)
		{
			nBufPosOffset += nAveBufSize ;

			::memset(&rfi,0,sizeof(rfi)) ;

			rfi.nBufSize = nAveBufSize ;

			rfi.hFile = ::CreateFile(strDstVer[i].c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_OVERLAPPED,NULL) ;

			if(rfi.hFile!=INVALID_HANDLE_VALUE)
			{
				{
					CptAutoLock lock(&m_Lock) ;
					m_ReadFileHandleList.push_back(rfi.hFile) ;
				}

				rfi.pBuf = m_pBlockBuf + nBufPosOffset ;
				FileHandleVer.push_back(rfi) ;
			}
			else
			{
				bRet = false ;
				goto COMPARE_FILE_END ;
			}
		}

		pVP.pExceptionCB->OnVerifyProgressBeginOneFile(strSrc,strDstVer[0]) ;

		BOOL bReadResult ;

		unsigned __int64 uRemainFileSize = 0 ;
		unsigned __int64 uFileSize = 0 ;

		DWORD dwHi = 0 ;
		DWORD dwLow = 0 ;

		dwLow = ::GetFileSize(FileHandleVer[0].hFile,&dwHi) ;

		uFileSize = CptGlobal::DoubleWordTo64(dwLow,dwHi) ;
		uRemainFileSize = uFileSize ;

		bRet = true ;

		while(uRemainFileSize>0 && bRet && !this->IsStop())
		{
			for(size_t i=0;i<FileHandleVer.size();++i)
			{
				::memset(&FileHandleVer[i].ov,0,sizeof(FileHandleVer[i].ov)) ;

				CptGlobal::Int64ToDoubleWord(uFileSize-uRemainFileSize,FileHandleVer[i].ov.ov.OffsetHigh,FileHandleVer[i].ov.ov.Offset) ;

				//FileHandleVer[i].ov.pExceptionCB = pVP->pExceptionCB ;
				FileHandleVer[i].ov.uTotalFileSize = uFileSize ;
				FileHandleVer[i].ov.pBuf = FileHandleVer[i].pBuf ;
				FileHandleVer[i].ov.nBufSize = FileHandleVer[i].nBufSize ;
				FileHandleVer[i].ov.nFileHandleIndex = (int)i ;

				SReadFileInfo rfi = FileHandleVer[i] ;

				//FileHandleVer[i].ov.ov.hEvent = m_hAPCEventArray[i] ;

				bReadResult = ::ReadFileEx(FileHandleVer[i].hFile,FileHandleVer[i].pBuf,FileHandleVer[i].nBufSize,
					(OVERLAPPED*)(&(FileHandleVer[i].ov)),FileIOCompletionRoutine2) ;

				// ERROR_MORE_DATA
				//Win_Debug_Printf(_T("CXCVerifyResult::CompareFile()   ReadFileEx  error_code=%d"),GetLastError()) ;

				if(bReadResult || ERROR_HANDLE_EOF==::GetLastError())
				{
					bReadResult = (WAIT_IO_COMPLETION==::WaitForSingleObjectEx(m_hAPCEventArray[i],INFINITE,TRUE)) ;
				}
				
				if(!bReadResult)
				{
					bRet = false ;
					goto COMPARE_FILE_END ;
				}

				if(i==0)
				{// 因为这里是2个或以上的文件读取，所以只取其中一个就行，不然文件大小就会出错
					pVP.pExceptionCB->OnVerifyProgressDataOccured(FileHandleVer[i].ov.dwOperSize) ;
				}
				
				//::WaitForSingleObjectEx(m_hAPCEventArray[i],INFINITE,TRUE) ;
			}

			//bool bResult = true ;

			//Win_Debug_Printf(_T("CXCVerifyResult::CompareFile()   1")) ;

			//for(size_t i=0;i<FileHandleVer.size() && bResult;++i)
			//{
			//	bResult = (WAIT_IO_COMPLETION==::WaitForSingleObjectEx(m_hAPCEventArray[i],INFINITE,TRUE)) ;
			//}

			//Win_Debug_Printf(_T("CXCVerifyResult::CompareFile()   2")) ;

			//if(bResult)
			//if(WAIT_IO_COMPLETION==::WaitForMultipleObjectsEx(FileHandleVer.size(),m_hAPCEventArray,TRUE,INFINITE,TRUE))
			//if(WAIT_IO_COMPLETION==::SleepEx(INFINITE,TRUE))
			if(TRUE)
			{
				for(size_t i=1;i<FileHandleVer.size();++i)
				{
					if(FileHandleVer[0].ov.dwCRC32!=FileHandleVer[i].ov.dwCRC32)
					{
						bRet = false ;
						break ;
					}
				}

				uRemainFileSize -= FileHandleVer[0].ov.dwOperSize ;
			}
			else
			{
				bRet = false;
			}
		}

	}
	else
	{// 打开失败
		int aaa = 0 ;
	}

	if(bRet)
	{
		pVP.pExceptionCB->OnVerifyProgress(strSrc,strDstVer[0],&SrcWfd) ;
	}

COMPARE_FILE_END:
	this->CloseFileHandle() ;

	m_ReadFileHandleList.clear() ;

	//{
	//	for(int i=0;i<m_nValidAPCEventNum;++i)
	//	{
	//		::SetEvent(m_hAPCEventArray[i]) ;
	//		::CloseHandle(m_hAPCEventArray[i]) ;
	//		m_hAPCEventArray[i] = NULL ;
	//	}

	//	m_nValidAPCEventNum = 0 ;
	//}

	//OutputDebugString(_T("CXCVerifyResult::CompareFile() end \r\n")) ;


	//Win_Debug_Printf(_T("CXCVerifyResult::CompareFile()   end")) ;

	return bRet ;
}

void CXCVerifyResult::FileIOCompletionRoutine2(DWORD dwErrorCode,DWORD dwNumberOfBytesTransfered,OVERLAPPED* lpOverlapped) 
{
	//::Win_Debug_Printf(_T("CXCVerifyResult::FileIOCompletionRoutine2() begin \r\n")) ;

	_ASSERT(lpOverlapped!=NULL) ;

	if(lpOverlapped!=NULL)
	{
		SXCAsynOverlapped* pXCOverlap = (SXCAsynOverlapped*)lpOverlapped ;

		//pXCOverlap->pExceptionCB->OnVerifyProgressDataOccured(dwNumberOfBytesTransfered) ;
		//CptString str ;

		//Win_Debug_Printf(_T("CXCVerifyResult::FileIOCompletionRoutine2() index = %d \r\n"),pXCOverlap->nFileHandleIndex) ;

		//::OutputDebugString(str.c_str()) ;
		
		pXCOverlap->dwOperSize = dwNumberOfBytesTransfered ;
		
		if(dwNumberOfBytesTransfered>0)
		{
			pXCOverlap->dwCRC32 = CptGlobal::CalculateCRC32((BYTE*)pXCOverlap->pBuf,pXCOverlap->dwOperSize) ;
		}
	}

	//::Win_Debug_Printf(_T("CXCVerifyResult::FileIOCompletionRoutine2() end \r\n")) ;
}

ECopyFileState CXCVerifyResult::Compare(const SVerifyPar* pVP,int nSrcIndex,const pt_STL_vector(SStorageInfoOfFile)& DstSiofVer)
{
	ECopyFileState cfsRet = CFS_Running ;

	WIN32_FIND_DATA SrcWfd ;
	HANDLE hFindHandle = INVALID_HANDLE_VALUE;
	pt_STL_vector(SDstInfo) DstInfoVer ;
	CXCVerifyResult::EFileDiffType edf = FDT_ContentDiff;
	SDstInfo di ;
	pt_STL_vector(CptString) strDstVer ;

	hFindHandle = ::FindFirstFile(pVP->SrcVer[nSrcIndex].c_str(),&SrcWfd) ;

	if(hFindHandle==INVALID_HANDLE_VALUE)
	{
		edf = FDT_SrcFileInexist;
		goto COMPARE_DIFF_END ;
	}

	::FindClose(hFindHandle) ;
	hFindHandle = INVALID_HANDLE_VALUE ;

	
	//CptString strRenamedFile ;
	//CptString strRenamedPath ;

	{// 找出当前源文件有变化过的目标文件
		CXCFileChangingBuffer::SFileChangingStatusResult fcsr ;

		if(pVP->pChangingBuf!=NULL)
		{
			pVP->pChangingBuf->GetChangingStatus(pVP->SrcVer[nSrcIndex],fcsr,true) ;
		}

		switch(fcsr.fct)
		{
		case CXCFileChangingBuffer::FileChangingType_Rename:
			{
				return CFS_Running ; // 暂时直接返回

				/**
				strRenamedFile = fcsr.strDstFileName ;
				CptWinPath::SPathElementInfo pei ;

				pei.uFlag = CptWinPath::PET_Path ;

				if(CptWinPath::GetPathElement(strRenamedFile.c_str(),pei))
				{
					strRenamedPath = pei.strPath ;
				}
				else
				{
					_ASSERT(FALSE) ;
				}
				/**/
			}
			//
			break ;

		case CXCFileChangingBuffer::FileChangingType_Skip: // 该文件已跳过
			return CFS_Running ;
			break ;

		default:
		case CXCFileChangingBuffer::FileChangingType_Normal:
			break ;
		}
	}
	

	for(size_t i=0;i<pVP->DstVer.size();++i)
	{
		if(CptGlobal::IsFolder(pVP->DstVer[i]))
		{
			di.strFile.Format(_T("%s\\%s"),pVP->DstVer[i],SrcWfd.cFileName) ;
		}
		else
		{
			di.strFile = pVP->DstVer[i] ;
		}

		/**
		if(strRenamedFile.GetLength()>0 && strRenamedPath.GetLength()>0)
		{// 如果改名

			CptWinPath::SPathElementInfo pei ;

			pei.uFlag = CptWinPath::PET_Path ;

			if(CptWinPath::GetPathElement(di.strFile.c_str(),pei))
			{
				if(pei.strPath.CompareNoCase(strRenamedPath)==0)
				{// 如果与改名的文件为相同路径则
					di.strFile = strRenamedFile ;
					strRenamedPath = _T("") ;
					strRenamedFile = _T("") ;
				}
			}
		}
		/**/

		hFindHandle = ::FindFirstFile(di.strFile.c_str(),&di.wfd) ;

		_ASSERT(hFindHandle!=INVALID_HANDLE_VALUE) ;

		if(hFindHandle!=INVALID_HANDLE_VALUE)
		{
			::FindClose(hFindHandle) ;
			hFindHandle = INVALID_HANDLE_VALUE ;

			di.fdt = edf = this->CompareAttr(SrcWfd,di.wfd) ;

			if(di.fdt==FDT_Same)
			{
				di.fdt = FDT_Unknown ;

				if(!CptGlobal::IsFolder(SrcWfd.dwFileAttributes))
				{// 若为文件

					if(SrcWfd.nFileSizeLow==di.wfd.nFileSizeLow && SrcWfd.nFileSizeHigh==di.wfd.nFileSizeHigh)
					{// 长度不一致
						if(di.wfd.nFileSizeLow==0 && di.wfd.nFileSizeHigh==0)
						{// 长度为0，那么内容肯定是一样的
							di.fdt = FDT_Same ;
						}
					}
					else
					{
						di.fdt = FDT_ContentDiff ;
					}
				}
				
			}
		}
		else
		{
			di.fdt = FDT_DstFileInexist ;
		}

		DstInfoVer.push_back(di) ;
	}

	for(size_t i=0;i<DstInfoVer.size();++i)
	{
		if(DstInfoVer[i].fdt==FDT_Unknown)
		{
			strDstVer.push_back(DstInfoVer[i].strFile) ;
		}
		else
		{
			m_bSame = false ;
			cfsRet = pVP->pExceptionCB->OnVerifyFileDiff(pVP->SrcVer[nSrcIndex],_T(""),edf) ;

			if(cfsRet==CFS_Stop)
			{
				return cfsRet ;
			}
		}
	}

	//OutputDebugString(_T("CXCVerifyResult::Compare() 1 \r\n")) ;

	if(!strDstVer.empty())
	{
		if(CptGlobal::IsFolder(pVP->SrcVer[nSrcIndex]))
		{
			this->CompareFolder(*pVP,pVP->SrcVer[nSrcIndex],strDstVer) ;
		}
		else
		{
			m_bSame = this->CompareFile(*pVP,pVP->SrcVer[nSrcIndex],strDstVer,SrcWfd) ;
		}
	}

	//OutputDebugString(_T("CXCVerifyResult::Compare() 2 \r\n")) ;

	return cfsRet ;

COMPARE_DIFF_END:
	m_bSame = false ;
	if(pVP->pExceptionCB!=NULL)
	{
		cfsRet = pVP->pExceptionCB->OnVerifyFileDiff(pVP->SrcVer[nSrcIndex],_T(""),edf) ;
	}

	return cfsRet ;
}

void CXCVerifyResult::CompareFolder(const SVerifyPar& vp,CptString strSrc, const pt_STL_vector(CptString)& DstFolderVer)
{
	struct SDstInfo
	{
		CptString strFile ;
		WIN32_FIND_DATA wfd ;
	};

	bool bDotFound1 = false ;
	bool bDotFound2 = false ;

	WIN32_FIND_DATA SrcWfd ;
	WIN32_FIND_DATA DstWfd ;

	CptString strDstFile ;

	CptString strSrcFiles = strSrc + _T("\\*.*") ;
	CptString strSrc2 ;

	HANDLE hFindHandle2 = INVALID_HANDLE_VALUE ;

	HANDLE hFindHandle = ::FindFirstFile(strSrcFiles,&SrcWfd) ;

	if(hFindHandle!=INVALID_HANDLE_VALUE)
	{
		pt_STL_vector(CptString) DstFileVer ;
		bool bIsSkip = false ;

		do
		{
			if(!bDotFound1 && ::_tcscmp(SrcWfd.cFileName,_T("."))==0)
			{
				bDotFound1 = true ;
				continue ;
			}
			else if(!bDotFound2 && ::_tcscmp(SrcWfd.cFileName,_T(".."))==0)
			{
				bDotFound2 = true ;
				continue ;
			}
			else
			{
				bIsSkip = false ;

				strSrc2.Format(_T("%s\\%s"),strSrc,SrcWfd.cFileName) ;

				bool bIsSrcFolder = CptGlobal::IsFolder(SrcWfd.dwFileAttributes) ;
				bool bFileExist = false ;

				// 遍历读取文件夹里的文件，并且判断其相关属性是否一致
				for(size_t i=0;i<DstFolderVer.size() && !bIsSkip;++i)
				{
					strDstFile.Format(_T("%s\\%s"),DstFolderVer[i],SrcWfd.cFileName) ;

					bFileExist = false ;

					if(IsFileExist(strDstFile))
					{
						bFileExist = (CptGlobal::IsFolder(strDstFile)==bIsSrcFolder) ;
					}
					else
					{// 若该源文件不存在,则检查是否是被忽略的文件. 即把跳过的文件,路过verify
						if(!vp.pChangingBuf->IsEmpty())
						{
							CptString strInexistSrcFile ;
							strInexistSrcFile.Format(_T("%s\\%s"),strSrc,SrcWfd.cFileName) ;

							CXCFileChangingBuffer::SFileChangingStatusResult fcsr ;

							if(vp.pChangingBuf->GetChangingStatus(strInexistSrcFile,fcsr,true))
							{
								if(fcsr.fct==CXCFileChangingBuffer::FileChangingType_Skip
									|| fcsr.fct==CXCFileChangingBuffer::FileChangingType_Skip)
								{
									bIsSkip = true ;
									continue ;
									
								}
							}
						}
						
					}

					if(bFileExist)
					{
						hFindHandle2 = ::FindFirstFile(strDstFile,&DstWfd) ;

						if(hFindHandle2!=INVALID_HANDLE_VALUE)
						{
							::FindClose(hFindHandle2) ;
							hFindHandle2 = INVALID_HANDLE_VALUE ;

							EFileDiffType edt = this->CompareAttr(SrcWfd,DstWfd) ;

							if(edt==FDT_Same)
							{
								DstFileVer.push_back(strDstFile) ;
							}
							else
							{
								m_bSame = false ;
								if(vp.pExceptionCB!=NULL)
								{
									m_RunningState = vp.pExceptionCB->OnVerifyFileDiff(strSrc2,strDstFile,edt) ;
								}
								return  ;
							}
						}
					}
					else
					{
						m_bSame = false ;
						if(vp.pExceptionCB!=NULL)
						{
							m_RunningState = vp.pExceptionCB->OnVerifyFileDiff(strSrc2,strDstFile,FDT_DstFileInexist) ;
						}

						::FindClose(hFindHandle) ;

						return ;
					}
				}

				if(!bIsSkip)
				{
					if(bIsSrcFolder)
					{// folder

						//OutputDebugString(_T("CXCVerifyResult::CompareFolder() 5 \r\n")) ;

						this->CompareFolder(vp,strSrc2,DstFileVer) ;

						//OutputDebugString(_T("CXCVerifyResult::CompareFolder() 6 \r\n")) ;
					}
					else
					{// file
						//OutputDebugString(_T("CXCVerifyResult::CompareFolder() 7 \r\n")) ;

						if(!this->CompareFile(vp,strSrc2,DstFileVer,SrcWfd))
						{
							m_bSame = false ;
							if(vp.pExceptionCB!=NULL)
							{
								m_RunningState = vp.pExceptionCB->OnVerifyFileDiff(strSrc2,strDstFile,FDT_ContentDiff) ;
							}
						}

						//OutputDebugString(_T("CXCVerifyResult::CompareFolder() 8 \r\n")) ;
					}
				}

				DstFileVer.clear() ;

			}
		}
		while(::FindNextFile(hFindHandle,&SrcWfd) && !this->IsStop());

		::FindClose(hFindHandle) ;
		hFindHandle = INVALID_HANDLE_VALUE ;
	}

}

void CXCVerifyResult::CheckPauseThenWait() 
{
	if(m_RunningState==CFS_Pause)
	{// 暂停
		if(m_hPauseWait!=NULL)
		{
			::WaitForSingleObject(m_hPauseWait,INFINITE) ;
		}
	}
}

bool CXCVerifyResult::Run(SVerifyPar vp) 
{
	Debug_Printf(_T("CXCVerifyResult::Run() begin")) ;

	m_bSame = true ;

	pt_STL_vector(SStorageInfoOfFile)	DstSiofVer ;
	SStorageInfoOfFile DstSiof ;

	if(!vp.SrcVer.empty() && !vp.DstVer.empty() && vp.DstVer.size()<64)
	{
		for(size_t i=0;i<vp.DstVer.size();++i)
		{
			if(IsFileExist(vp.DstVer[i].c_str())
				&& CXCWinStorageRelative::GetFileStoreInfo(vp.DstVer[i],DstSiof))
			{
				DstSiofVer.push_back(DstSiof) ;

				if(vp.DstVer[i].GetLength()>2 && 
					vp.DstVer[i].GetAt(vp.DstVer[i].GetLength()-2)!=':')
				{
					vp.DstVer[i].TrimRight('\\') ;
					vp.DstVer[i].TrimRight('/') ;
				}

				vp.DstVer[i] = CptGlobal::MakeUnlimitFileName(vp.DstVer[i],true) ;
			}
			else
			{
				m_bSame = false ;
				return false ;
			}
		}

		m_nReadCount = 0 ;
		BYTE* pVirtualBuf = NULL ;
		BYTE* pBuf = vp.pBuf ;
		int nBufSize = vp.nBufSize ;

		if(vp.pBuf==NULL)
		{
			nBufSize = 8*1024*1024 ;
			pVirtualBuf = (BYTE*)::VirtualAlloc(NULL,nBufSize,MEM_COMMIT,PAGE_READWRITE) ;

			if(pVirtualBuf==NULL)
			{
				return false ;
			}

			::VirtualLock(vp.pBuf,nBufSize) ; 
			pBuf = pVirtualBuf ;
		}

		for(size_t i=0;i<vp.SrcVer.size();++i)
		{
			if(vp.SrcVer[i].GetLength()>2 && 
			vp.SrcVer[i].GetAt(vp.SrcVer[i].GetLength()-2)!=':')
			{
				vp.SrcVer[i].TrimRight('\\') ;
				vp.SrcVer[i].TrimRight('/') ;
			}

			vp.SrcVer[i] = CptGlobal::MakeUnlimitFileName(vp.SrcVer[i],true) ;
		}

		m_RunningState = CFS_Running ;

		DWORD dwNewSize = ALIGN_SIZE_DOWN(nBufSize,4*1024) ;

		m_nBufSize = dwNewSize/2 ;

		m_pSrcBuf = pBuf ;
		m_pDstBuf = pBuf + m_nBufSize ;

		m_pBlockBuf = pBuf ;
		m_nBlockSize = dwNewSize ;

		SStorageInfoOfFile SrcSiof ;
		bool bLoop = true ;

		//OutputDebugString(_T("CXCVerifyResult::Run() 1 \r\n")) ;

		for(size_t i=0;bLoop && i<vp.SrcVer.size();++i)
		{
			//if(CXCWinStorageRelative::GetFileStoreInfo(vp.SrcVer[i],SrcSiof))
			{
				//OutputDebugString(_T("CXCVerifyResult::Run() 2 \r\n")) ;

				switch(this->Compare(&vp,(int)i,DstSiofVer))
				//switch(this->Compare(&vp,i,false))
				{
				default:
				case CFS_Running:break ;

				case CFS_Exit:
				case CFS_Stop: bLoop = false; break ;

				case CFS_Pause:
					this->CheckPauseThenWait() ;
					break ;
				}

				//OutputDebugString(_T("CXCVerifyResult::Run() 3 \r\n")) ;
			}
		}

		//OutputDebugString(_T("CXCVerifyResult::Run() 4 \r\n")) ;

		//m_pSrcBuf = NULL ;
		//m_pDstBuf = NULL ;

		if(pVirtualBuf!=NULL)
		{
			::VirtualUnlock(pVirtualBuf,vp.nBufSize) ;
			::VirtualFree(pVirtualBuf,vp.nBufSize,MEM_RELEASE) ;
			pVirtualBuf = NULL ;
		}
	}
	else
	{
		m_bSame = false ;
	}

	Debug_Printf(_T("CXCVerifyResult::Run() 99")) ;

	this->Stop() ;

	Debug_Printf(_T("CXCVerifyResult::Run() end")) ;

	return m_bSame ;
}

void CXCVerifyResult::Pause() 
{
	if(m_RunningState==CFS_Running)
	{
		if(m_hPauseWait==NULL)
		{
			m_hPauseWait = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;
		}

		m_RunningState = CFS_Pause ;
	}
	
}

void CXCVerifyResult::Continue()
{
	if(m_RunningState == CFS_Pause)
	{
		m_RunningState = CFS_Running ;

		::SetEvent(m_hPauseWait) ;
	}
}

void CXCVerifyResult::Stop() 
{
	m_RunningState = CFS_Stop ;

	this->CloseFileHandle() ;

	if(m_hReadEvent1!=NULL)
	{
		::SetEvent(m_hReadEvent1) ;
		::CloseHandle(m_hReadEvent1) ;
		m_hReadEvent1 = NULL ;
	}

	if(m_hPauseWait!=NULL)
	{
		::SetEvent(m_hPauseWait) ;
		::CloseHandle(m_hPauseWait) ;
		m_hPauseWait = NULL ;
	}

	for(int i=0;i<m_nValidAPCEventNum;++i)
	{
		::SetEvent(m_hAPCEventArray[i]) ;
		::CloseHandle(m_hAPCEventArray[i]) ;
		m_hAPCEventArray[i] = NULL ;
	}

	m_nValidAPCEventNum = 0 ;


	if(m_hThread!=NULL)
	{
		::WaitForSingleObject(m_hThread,2*1000) ;

		::CloseHandle(m_hThread) ;
		m_hThread = NULL ;
	}
}

void CXCVerifyResult::CloseFileHandle()
{
	CptAutoLock lock(&m_Lock) ;

	pt_STL_list(HANDLE)::iterator it = m_ReadFileHandleList.begin() ;

	for(;it!=m_ReadFileHandleList.end();++it)
	{
		if(*it!=INVALID_HANDLE_VALUE)
		{
			::CancelIo(*it) ;
			::CloseHandle(*it) ;
			(*it) = INVALID_HANDLE_VALUE ;
		}
	}
}
