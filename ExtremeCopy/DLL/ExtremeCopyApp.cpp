/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "stdafx.h"
#include "ExtremeCopyApp.h"


bool CheckRegisterCodeForExtremeCopyLibrary2(const CptStringList& sl) ;

// 判断是否改名的复制或移动文件
bool IsDstRenameFileName(const SGraphTaskDesc& gtd) 
{
	bool bRet = false ;

	if(gtd.SrcFileVer.size()==1 && gtd.DstFolderVer.size()==1)
	{
		DWORD dwDstAttr = ::GetFileAttributes(gtd.DstFolderVer[0].c_str()) ;

		if(dwDstAttr==INVALID_FILE_ATTRIBUTES || !CptGlobal::IsFolder(dwDstAttr))
		{
			DWORD dwSrcAttr = ::GetFileAttributes(gtd.SrcFileVer[0].c_str()) ;

			if(dwSrcAttr!=INVALID_FILE_ATTRIBUTES && !CptGlobal::IsFolder(dwSrcAttr))
			{
				CptWinPath::SPathElementInfo pei ;

				pei.uFlag = CptWinPath::PET_Path|CptWinPath::PET_FileName ;

				if(CptWinPath::GetPathElement(gtd.DstFolderVer[0].c_str(),pei)
					&& CptGlobal::IsFolder(pei.strPath))
				{// 此为改名的文件名
					bRet = true ;
				}
			}
		}

	}

	return bRet ;
}

CExtremeCopyApp::CExtremeCopyApp():m_lpfnRoutine(NULL),m_hThread(NULL),m_bIni(false),m_uCurFileID(0),m_nBufSize(32),m_uCurFileWrittenSize(0)
{
//	m_XCCore.SetEventCallEvent(this) ;
}

CExtremeCopyApp::~CExtremeCopyApp() 
{
	this->Stop() ;
}


void CExtremeCopyApp::Stop() 
{
	m_strSrcVer.clear() ;
	m_strDstFolderVer.clear() ;
	m_lpfnRoutine = NULL ;

	m_XCCore.Stop() ;

	if(m_hThread!=NULL)
	{
		::WaitForSingleObject(m_hThread,2*1000) ;
		::CloseHandle(m_hThread) ;
		m_hThread = NULL ;
	}

	m_ActiveFileInfoList.clear() ;
}

int CExtremeCopyApp::SetCopyBufferSize(int nBufSize) 
{
	int nRet = m_nBufSize ;

	switch(nBufSize)
	{
	case 32:
	case 16:
	case 8:
	case 4:
	case 2: m_nBufSize = nBufSize ;break ;

	default: nRet = -1 ; break ;
	}

	return nRet ;
}

void CExtremeCopyApp::SetLicenseKey(CptString strSN)
{
	m_strSeriesNumber = strSN ;
}

void CExtremeCopyApp::SetRoutine(lpfnCopyRoutine_t Routine)
{
	m_lpfnRoutine = Routine ;
}

bool CExtremeCopyApp::Pause() 
{
	if(m_XCCore.GetState()==CFS_Running)
	{
		m_XCCore.Pause() ;

		return true ;
	}

	return false ;
}

bool CExtremeCopyApp::Continue() 
{
	if(m_XCCore.GetState()==CFS_Pause)
	{
		m_XCCore.Continue() ;
		return true ;
	}

	return false ;
}

// 判断是否有父目录复制到其子目录里
bool CExtremeCopyApp::DoesIncludeRecuriseFolder(const pt_STL_vector(CptString)& SrcVer,const pt_STL_vector(CptString)& DstVer,int& nSrcIndex, int& nDstIndex)
{
	for(size_t i=0;i<SrcVer.size();++i)
	{
		for(size_t j=0;j<DstVer.size();++j)
		{
			if(CptGlobal::IsFolder(DstVer[i]) && 
				CptGlobal::IsFolder(SrcVer[j]) &&
				DstVer[i].Find(SrcVer[j])==0)
			{
				if(DstVer[i].GetLength()>SrcVer[j].GetLength())
				{
					const TCHAR c = DstVer[i].GetAt(SrcVer[j].GetLength()) ;

					if(c=='\\' || c=='/')
					{
						nSrcIndex = (int)j ;
						nDstIndex = (int)i ;
						return true ;
					}
				}
			}
		}
	}

	return false ;
}


// 判断是否改名的复制或移动文件
bool CExtremeCopyApp::IsDstRenameFileName(const pt_STL_vector(CptString)& SrcVer,const pt_STL_vector(CptString)& DstVer) 
{
	bool bRet = false ;

	if(SrcVer.size()==1 && DstVer.size()==1)
	{
		DWORD dwDstAttr = ::GetFileAttributes(DstVer[0].c_str()) ;

		if(dwDstAttr==INVALID_FILE_ATTRIBUTES || !CptGlobal::IsFolder(dwDstAttr))
		{
			DWORD dwSrcAttr = ::GetFileAttributes(SrcVer[0].c_str()) ;

			if(dwSrcAttr!=INVALID_FILE_ATTRIBUTES && !CptGlobal::IsFolder(dwSrcAttr))
			{
				CptWinPath::SPathElementInfo pei ;

				pei.uFlag = CptWinPath::PET_Path|CptWinPath::PET_FileName ;

				if(CptWinPath::GetPathElement(DstVer[0].c_str(),pei)
					&& CptGlobal::IsFolder(pei.strPath))
				{// 此为改名的文件名
					bRet = true ;
				}
			}
		}
	}

	return bRet ;
}

int CExtremeCopyApp::CheckTaskParameter() 
{
	//int nRet = START_ERROR_CODE_UNKNOWN ;

	if(m_strSrcVer.empty())
	{
		return START_ERROR_CODE_INVALID_SOURCE ; // 无效的源文件
	}

	if(m_strDstFolderVer.empty())
	{
		return START_ERROR_CODE_INVALID_DSTINATION ; // 无效的目标文件夹
	}

	if(m_XCCore.GetState()!=CFS_Stop)
	{
		return START_ERROR_CODE_CANOT_LAUNCHTASK ;
	}

	for(size_t i=0;i<m_strSrcVer.size();++i)
	{
		if(!IsFileExist(m_strSrcVer[i].c_str()))
		{
			return START_ERROR_CODE_INVALID_SOURCE ; // 无效的源文件
		}
	}

	if(!IsDstRenameFileName(m_strSrcVer,m_strDstFolderVer))
	{// 如果为非‘改名’文件
		DWORD dwAttr = 0 ;
		for(size_t i=0;i<m_strDstFolderVer.size();++i)
		{
			dwAttr = ::GetFileAttributes(m_strDstFolderVer[i].c_str()) ;
			if(dwAttr==INVALID_FILE_ATTRIBUTES || !CptGlobal::IsFolder(dwAttr))
			{
				return START_ERROR_CODE_INVALID_DSTINATION ; // 无效的目标文件夹
			}
		}
	}

	return START_ERROR_CODE_UNKNOWN ;
}

int CExtremeCopyApp::Start(int nRunType,bool bSyncOrAsync) 
{
	int nRet = this->CheckTaskParameter() ;

	if(nRet!=START_ERROR_CODE_UNKNOWN)
	{
		return nRet ;
	}

	if(!m_bIni)
	{
		m_bIni = true ;

		// 回调事件设置
		m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_ImpactFile,this) ;
		m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_GetState,this) ;

		m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_CopyBatchFilesBegin,this) ;
		m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_CopyFileEnd,this) ;
		m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_CopyFileDataOccur,this) ;
		m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_CopyFileDiscard,this) ;
		m_XCCopyEvent.SetReceiver(CXCCopyingEvent::ET_Exception,this) ;
	}

	m_ActiveFileInfoList.clear() ;
	const bool bCopyOrMove = (nRunType==XCRunType_Copy) ;

	if(bSyncOrAsync)
	{
		this->CopyWork(bCopyOrMove) ;
	}
	else
	{
		SThreadParamData* pParamData = new SThreadParamData() ;

		pParamData->pThis = this ;
		pParamData->bCopyOrMove = bCopyOrMove ;

		m_hThread = (HANDLE)::_beginthreadex(NULL,0,CopyThreadFunc,pParamData,0,NULL) ;
	}

	nRet = START_ERROR_CODE_SUCCESS ;

	return nRet ;
}

unsigned int CExtremeCopyApp::CopyThreadFunc(void* pParam)
{
	SThreadParamData* pParamData = (SThreadParamData*)pParam ;

	pParamData->pThis->CopyWork(pParamData->bCopyOrMove) ;

	::CloseHandle(pParamData->pThis->m_hThread) ;
	pParamData->pThis->m_hThread = NULL ;

	delete pParamData ;

	return  0 ;
}

void CExtremeCopyApp::CopyWork(bool bCopyOrMove)
{
	bool bRet = false ;

	SGraphTaskDesc gtd ;

	gtd.SrcFileVer = m_strSrcVer ;
	gtd.DstFolderVer = m_strDstFolderVer ;

	gtd.ExeType = bCopyOrMove ? XCTT_Copy : XCTT_Move;
	gtd.HashType = FHT_UNKNOWN ;
	gtd.pCopyingEvent = &m_XCCopyEvent ;
	gtd.nFileDataBufSize = m_nBufSize*(1024*1024) ;

	{// 判断目标文件夹是否存在
		gtd.bIsRenameDst = ::IsDstRenameFileName(gtd) ;

		if(!gtd.bIsRenameDst)
		{
			DWORD dwAttr = 0 ;
			for(size_t i=0;i<gtd.DstFolderVer.size();++i)
			{
				dwAttr = ::GetFileAttributes(gtd.DstFolderVer[i].c_str()) ;
				if(dwAttr==INVALID_FILE_ATTRIBUTES || !CptGlobal::IsFolder(dwAttr))
				{// 目标文件夹不存在
					if(m_lpfnRoutine!=NULL)
					{
						m_lpfnRoutine(ROUTINE_CMD_FILEFAILED,2,0,NULL,NULL) ;
					}

					return  ;
				}
			}
		}
	}

	bRet = m_XCCore.Run(gtd) ;

	m_XCCore.Stop() ;

	if(m_lpfnRoutine!=NULL)
	{
		m_lpfnRoutine(ROUTINE_CMD_TASKFINISH,bRet?1:0,0,NULL,NULL) ;
	}

	_ASSERT(m_ActiveFileInfoList.empty()) ;
}

ECopyFileState CExtremeCopyApp::GetState() 
{
	return m_XCCore.GetState() ;
}

int CExtremeCopyApp::AttachSrc(const CptString strSrcFile) 
{
	int nRet = -1 ;

	if(strSrcFile.GetLength()>0)
	{
		if(m_strSrcVer.size()==0)
		{
			CptString str = CptGlobal::MakeUnlimitFileName(strSrcFile,true) ;

			m_strSrcVer.push_back(str) ;

			nRet = (int)m_strSrcVer.size() ;
		}
		else
		{
			if(m_strSeriesNumber.GetLength()==29)
			{
				CptStringList sl ;

				if(sl.Split(m_strSeriesNumber,"-")==5
					&& CheckRegisterCodeForExtremeCopyLibrary2(sl))
				{// 如果 license key 没问题，则允许加入
					CptString str = CptGlobal::MakeUnlimitFileName(strSrcFile,true) ;

					m_strSrcVer.push_back(str) ;

					nRet = (int)m_strSrcVer.size() ;
				}
			}
		}
	}
	else
	{
		m_strSrcVer.clear() ;
		nRet = 0 ;
	}

	return nRet ;
}

int CExtremeCopyApp::AttachDst(const CptString strDstFile) 
{
	int nRet = -1 ;

	if(strDstFile.GetLength()>0)
	{
		CptString str = CptGlobal::MakeUnlimitFileName(strDstFile,true) ;

		m_strDstFolderVer.push_back(str) ;

		nRet = (int)m_strDstFolderVer.size() ;

	}
	else
	{
		m_strDstFolderVer.clear() ;
		nRet = 0 ;
	}

	return nRet ;
}

bool CExtremeCopyApp::SetDestinationFolder(const CptString strDstFolder) 
{
	if(strDstFolder.GetLength()>0)
	{
		m_strDstFolderVer.clear() ;

		CptString str = CptGlobal::MakeUnlimitFileName(strDstFolder,true) ;

		m_strDstFolderVer.push_back(str) ;

		return true ;
	}

	return false ;
}

void CExtremeCopyApp::RemoveCityFromActiveList(unsigned uCityID) 
{
	_ASSERT(!m_ActiveFileInfoList.empty()) ;

	bool bFound = false ;

	pt_STL_list(SActiveFilesInfo)::iterator it = m_ActiveFileInfoList.begin() ;

	for(;it!=m_ActiveFileInfoList.end();++it)
	{
		if((*it).uFileID==uCityID)
		{
			m_ActiveFileInfoList.erase(it) ;
			bFound = true ;
			break ;
		}
	}

	_ASSERT(bFound) ;
}

void CExtremeCopyApp::PostBeginOneFileCommand(const SActiveFilesInfo& afi) 
{
	CptString strSrc ;
	CptString strDst ;

	strSrc = CptGlobal::MakeUnlimitFileName(afi.strSrcFile,false) ;
	strDst = CptGlobal::MakeUnlimitFileName(afi.strDstFile,false) ;

	DWORD dwLow ;
	DWORD dwHi ;
	CptGlobal::Int64ToDoubleWord(afi.uFileSize,dwHi,dwLow) ;

	if(m_lpfnRoutine!=NULL)
	{
		m_lpfnRoutine(ROUTINE_CMD_BEGINONEFILE,dwLow,dwHi,strSrc.c_str(),strDst.c_str()) ;// 另一文件又开始
	}
}

int CExtremeCopyApp::OnCopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1,void* pParam2) 
{
	int nRet = 0 ;

	//CptAutoLock lock(&m_CoreCallbackLock) ;

	switch(et)
	{
	case CXCCopyingEvent::ET_CopyBatchFilesBegin: // 一批文件开始复制
		{
			_ASSERT(pParam1!=NULL) ;

			pt_STL_list(SActiveFilesInfo)& pSrcFileQue = *(pt_STL_list(SActiveFilesInfo)*)pParam1 ;

			{
				CptAutoLock lock(&m_FdoListLock) ;

				//if(m_lpfnRoutine!=NULL)
				{
					pt_STL_list(SActiveFilesInfo)::const_iterator it = pSrcFileQue.begin() ;

					for(;it!=pSrcFileQue.end();++it)
					{
						if((*it).uFileSize==0)
						{// 长度为0的文件不加入缓冲区，直接处理掉
							if(m_lpfnRoutine!=NULL)
							{
								m_lpfnRoutine(ROUTINE_CMD_BEGINONEFILE,0,0,(*it).strSrcFile.c_str(),(*it).strDstFile.c_str()) ;// 另一文件又开始
								m_lpfnRoutine(ROUTINE_CMD_FINISHONEFILE,0,0,NULL,NULL) ;
							}
						}
						else
						{
							m_ActiveFileInfoList.push_back(*it) ;
						}
						
					}
				}
			}
		}
		break ;

	case CXCCopyingEvent::ET_CopyFileEnd: // 一批文件所有操作结束
		{
			pt_STL_vector(SFileEndedInfo)* feiVer = (pt_STL_vector(SFileEndedInfo) *)pParam1 ;

			{
				CptAutoLock lock(&m_FdoListLock) ;

				if(m_uCurFileID>0)
				{
					
					pt_STL_vector(SFileEndedInfo)::const_iterator it = feiVer->begin() ;

					for(;it!=feiVer->end();++it)
					{
						if((*it).uFileID==m_uCurFileID)
						{
							this->RemoveCityFromActiveList(m_uCurFileID) ;

							if(m_lpfnRoutine!=NULL)
							{
								m_lpfnRoutine(ROUTINE_CMD_FINISHONEFILE,0,0,NULL,NULL) ;
							}
							
							m_uCurFileID = 0 ;
							break ;
						}
					}
				}


				//_ASSERT(m_ActiveFileInfoList.empty()) ;
				
			}
		}

		break ;

	case CXCCopyingEvent::ET_CopyFileDataOccur: // 文件数据发生操作
		{
			_ASSERT(pParam1!=NULL) ;
			SFileDataOccuredInfo& pFileDataInfo = *(SFileDataOccuredInfo*)pParam1 ;

			{
				CptAutoLock lock(&m_FdoListLock) ;

				pt_STL_list(SActiveFilesInfo)::const_iterator it = m_ActiveFileInfoList.begin() ;

				for(;it!=m_ActiveFileInfoList.end();++it)
				{
					if((*it).uFileID==pFileDataInfo.uFileID)
					{
						break ;
					}
				}

				_ASSERT(it!=m_ActiveFileInfoList.end()) ;

				if(!pFileDataInfo.bReadOrWrite)
				{
					if(m_uCurFileID==0)
					{
						this->PostBeginOneFileCommand(*it) ; // 开始一文件

						m_uCurFileID = pFileDataInfo.uFileID ;
					}

					m_uCurFileWrittenSize += pFileDataInfo.nDataSize ;

					if(m_lpfnRoutine!=NULL)
					{
						m_lpfnRoutine(ROUTINE_CMD_DATAWROTE,(int)pFileDataInfo.nDataSize,0,NULL,NULL) ;
					}
				}

				//if(m_uCurFileID!=pFileDataInfo.uFileID)

				if(m_uCurFileWrittenSize>=(*it).uFileSize)
				{// 在切换到下一 个 file stream 之前先把当前这个stream告诉应用软件让它结束了

					if(m_uCurFileID>0)
					{
						m_uCurFileWrittenSize = 0 ;
						//int aa = m_ActiveFileInfoList.size() ;
						this->RemoveCityFromActiveList(m_uCurFileID) ;
						//int ddd = m_ActiveFileInfoList.size() ;
						if(m_lpfnRoutine!=NULL)
						{
							m_lpfnRoutine(ROUTINE_CMD_FINISHONEFILE,0,0,NULL,NULL) ;// 一文件结束
						}

						m_uCurFileID = 0 ;
					}

				}
			}

		}
		break ;

	case CXCCopyingEvent::ET_CopyFileDiscard: // 文件抛弃
		{
			_ASSERT(pParam1!=NULL) ;
			const SDataPack_SourceFileInfo& pFD = *(SDataPack_SourceFileInfo*)pParam1 ;
			const unsigned __int64& uDiscardSize = *(unsigned __int64*)pParam2 ;

			//this->ProcessEvent_CopyFileDiscard(pFD,uDiscardSize) ;
		}
		break ;

	case CXCCopyingEvent::ET_Exception: // 发生异常
		{
			_ASSERT(pParam1!=NULL) ;

			nRet = ErrorHandlingFlag_Exit ;

			SXCExceptionInfo& ExceptInfo = *(SXCExceptionInfo*)pParam1 ;

			if(m_lpfnRoutine!=NULL)
			{	
				CptString strSrc ;
				CptString strDst ;

				strSrc = CptGlobal::MakeUnlimitFileName(ExceptInfo.strSrcFile,false) ;
				strDst = CptGlobal::MakeUnlimitFileName(ExceptInfo.strDstFile,false) ;

				if(m_lpfnRoutine!=NULL)
				{
					nRet = m_lpfnRoutine(ROUTINE_CMD_FILEFAILED,ExceptInfo.ErrorCode.nSystemError,0,strSrc.c_str(),strDst.c_str()) ;
				}
				else
				{
					nRet = ErrorHandlingFlag_Exit ;
				}

				if(nRet!=ErrorHandlingFlag_Retry && nRet!=ErrorHandlingFlag_Ignore)
				{
					nRet = ErrorHandlingFlag_Exit ;
				}
			}
			//nRet = (int)this->ProcessEvent_Exception(ExceptInfo) ;
		}
		break ;

	case CXCCopyingEvent::ET_ImpactFile: // 冲突文件
		{
			_ASSERT(pParam1!=NULL) ;
			_ASSERT(pParam2!=NULL) ;

			SImpactFileInfo& ImpactInfo = *(SImpactFileInfo*)pParam1 ;
			SImpactFileResult& Result = *(SImpactFileResult*)pParam2 ;

			Result.result = SFDB_Skip ; // 默认是跳过

			if(m_lpfnRoutine!=NULL)
			{
								CptString strSrc ;
				CptString strDst ;

				strSrc = CptGlobal::MakeUnlimitFileName(ImpactInfo.strSrcFile,false) ;
				strDst = CptGlobal::MakeUnlimitFileName(ImpactInfo.strDestFile,false) ;

				EImpactFileBehaviorResult result = (EImpactFileBehaviorResult)m_lpfnRoutine(ROUTINE_CMD_SAMEFILENAME,0,0,
					strSrc.c_str(),strDst.c_str()) ;

				switch(result)
				{
					//case SFDB_Unkown:
				case SFDB_Replace :
					//case SFDB_Skip:
					//case SFDB_Rename :
					//case SFDB_StopCopy:
					Result.result = result ;
					break ;
				}
			}

			//this->ProcessEvent_ImpactFile(ImpactInfo,Result) ;
		}
		break ;

	case CXCCopyingEvent::ET_GetState: // 状态
		{
			_ASSERT(pParam1!=NULL) ;

			ECopyFileState& pState = *(ECopyFileState*)pParam1 ;

			//pState = this->GetXCState() ;
		}
		break ;

	}

	return nRet ;
}

/**
// 复制回调事件
//virtual void OnBeginCopyOneFile2(void* pSender,const SFileOrDirectoryDiskInfo& Src,const SFileOrDirectoryDiskInfo& Dst) ;
void CExtremeCopyApp::OnBeginCopyOneFile(void* pSender,CReadFileProcessor* pReader,CWriteFileProcessor* pWriter) 
{
	if(m_lpfnRoutine!=NULL)
	{
		m_lpfnRoutine(ROUTINE_CMD_BEGINONEFILE,0,0,pReader->GetFileName().c_str(),pWriter->GetFileName().c_str()) ;
	}
}

void CExtremeCopyApp::OnCopyOneFileCompleted(void* pSender) 
{
	if(m_lpfnRoutine!=NULL)
	{
		m_lpfnRoutine(ROUTINE_CMD_FINISHONEFILE,0,0,NULL,NULL) ;
	}
}

ErrorHandlingResult CExtremeCopyApp::OnCopyError(void* pSender,const SCBErrorInfo& ErrorInfo) 
{
	ErrorHandlingResult ret = ErrorHandlingFlag_Exit ;

	if(m_lpfnRoutine!=NULL)
	{
		ret = m_lpfnRoutine(ROUTINE_CMD_FILEFAILED,ErrorInfo.nSystemErrorCode,0,ErrorInfo.strSrcFile.c_str(),ErrorInfo.strDstFile.c_str()) ;

		if(ret!=ErrorHandlingFlag_Retry && ret!=ErrorHandlingFlag_Ignore)
		{
			//Debug_Printf(_T("CExtremeCopyApp::OnCopyError()")) ;
			ret = ErrorHandlingFlag_Exit ;
		}
	}

	return ret ;
}

void CExtremeCopyApp::OnCopyDataSizeOccured(void* pSender,bool bReadOrWrite,unsigned int nSize)
{
	if(m_lpfnRoutine!=NULL)
	{
		m_lpfnRoutine(ROUTINE_CMD_DATAWROTE,(int)nSize,0,NULL,NULL) ;
	}
}

void CExtremeCopyApp::OnCopyStateChanged(void* pSender,ECopyFileState OldState,ECopyFileState NewState) 
{
}

void CExtremeCopyApp::OnCopyIdle(void* pSender) 
{
}

int CExtremeCopyApp::OnPhaseDone() 
{
	return 0 ;
}

ESameFileDialogBehaviorResult CExtremeCopyApp::OnSameFileName(void* pSend,LPCTSTR lpSrcFile,LPCTSTR lpDstFile) 
{
	ESameFileDialogBehaviorResult sfbRet = SFDB_Skip ;

	if(m_lpfnRoutine!=NULL)
	{
		ESameFileDialogBehaviorResult result = (ESameFileDialogBehaviorResult)m_lpfnRoutine(ROUTINE_CMD_SAMEFILENAME,0,0,lpSrcFile,lpDstFile) ;

		switch(result)
		{
		//case SFDB_Unkown:
		case SFDB_Replace :
		//case SFDB_Skip:
		//case SFDB_Rename :
		//case SFDB_StopCopy:
			sfbRet = result ;
			break ;
		}
	}

	return sfbRet ;
}
/**/