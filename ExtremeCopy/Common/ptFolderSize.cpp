/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptFolderSize.h"
#include <process.h>
#include "ptGlobal.h"

CptFolderSize::CptFolderSize(void)
:m_CurState(State_Stop),m_hAnalysisThread(NULL),m_nLastCheckFileCount(0),m_pEventCallBack(NULL),
m_hWaitEvent(NULL)
{
}

CptFolderSize::~CptFolderSize(void)
{
	this->Stop() ;
}


bool CptFolderSize::Start(const pt_STL_vector(CptString)& FileVer,CFolderSizeEventCB* pCB)
{
	_ASSERT(!FileVer.empty()) ;

	if(m_CurState==State_Stop && !FileVer.empty())
	{
		//for(size_t i=0;i<FolderVer.size();++i)
		//{
		//	if(!IsFileExist(FolderVer[i]))
		//	{
		//		return false ;
		//	}
		//}

		m_CurState = State_Running ;

		m_strFolderVer = FileVer ;
		m_pEventCallBack = pCB ;

		m_hAnalysisThread = (HANDLE)::_beginthreadex(NULL,0,AnalysisFolderThreadFunc,this,0,NULL) ;

		if(m_hAnalysisThread==NULL)
		{
			return false ;
		}
	}

	return true ;
}

bool CptFolderSize::Continue() 
{
	if(m_CurState==State_Pause)
	{
		m_CurState = State_Running ;
		return true ;
	}
	
	return false ;
}

bool CptFolderSize::Pause() 
{
	if(m_CurState==State_Running)
	{
		m_CurState = State_Pause ;
		return true ;
	}
	return false ;
}

void CptFolderSize::Stop() 
{
	m_CurState = State_Stop ;

	if(m_hWaitEvent!=NULL)
	{
		::SetEvent(m_hWaitEvent) ;
		::CloseHandle(m_hWaitEvent) ;
		m_hWaitEvent = NULL ;
	}

	if(m_hAnalysisThread!=NULL)
	{
		::WaitForSingleObject(m_hAnalysisThread,3*1000) ;
		::CloseHandle(m_hAnalysisThread) ;
		m_hAnalysisThread = NULL ;
	}

	m_strFolderVer.clear() ;
	m_pEventCallBack = NULL ;
}

unsigned int CptFolderSize::AnalysisFolderThreadFunc(void* pParam)
{
	CptFolderSize* pThis = (CptFolderSize*)pParam ;

	pThis->AnalysisFolderWork() ;

	return 0 ;
}

void CptFolderSize::AnalysisFolderWork()
{
	SFileOrDirectoryInfo fdi ;

	fdi.nFileCount = 0 ;
	fdi.nTotalSize = 0 ;
	fdi.nFolderCount = 0 ;

	m_hWaitEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;

	for(unsigned int i=0;i<m_strFolderVer.size();++i)
	{
		if(m_CurState!=State_Stop)
		{
			this->TravelDirectory(m_strFolderVer[i],fdi) ;
		}
	}

	if(m_pEventCallBack!=NULL && m_CurState!=State_Stop)
	{
		m_pEventCallBack->OnFolderSizeCompleted() ;
	}

	m_CurState = State_Stop ;
}

void CptFolderSize::Wait()
{
	if(m_hWaitEvent!=NULL)
	{
		::WaitForSingleObject(m_hWaitEvent,INFINITE) ;
	}
}

void CptFolderSize::TravelDirectory(CptString strDir,SFileOrDirectoryInfo& fdi)
{
	DWORD dwAttr = ::GetFileAttributes(strDir.c_str()) ;

	if(dwAttr==INVALID_FILE_ATTRIBUTES)
	{
		return ;
	}

	//if(strDir.GetLength()==3)
	//{// 若为根目录

	//	if(::GetVolumeInformation(strDir.c_str(),NULL,0,NULL,NULL,NULL
	//	return ;
	//}

	CptString strTem ;
	bool bIsDir = false ;
	
	if(dwAttr & FILE_ATTRIBUTE_DIRECTORY)
	{// 目录
		//Debug_Printf("CptFolderSize::TravelDirectory() %s len=%d",strDir.c_str(),strDir.GetLength()) ;
		strTem = strDir + _T("\\*.*") ;

		//Debug_Printf("CptFolderSize::TravelDirectory() %s",strTem.c_str()) ;
		bIsDir = true ;
	}
	else
	{// 文件
		strTem = strDir ;
	}
	
	WIN32_FIND_DATA wfd ;

	HANDLE hFileFind = ::FindFirstFile(strTem.c_str(),&wfd) ;

	if(hFileFind!=INVALID_HANDLE_VALUE )
	{
		if(wfd.dwFileAttributes!=INVALID_FILE_ATTRIBUTES)
		{
			bool bDotFound1 = false ;
			bool bDotFound2 = false ;

			do
			{
				if(!bDotFound1 && ::_tcscmp(wfd.cFileName,_T("."))==0)
				{
					bDotFound1 = true ;
					continue ;
				}
				else if(!bDotFound2 && ::_tcscmp(wfd.cFileName,_T(".."))==0)
				{
					bDotFound2 = true ;
					continue ;
				}
				else
				{
					if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{// 若为目录
						++fdi.nFolderCount ;

						CptString strNext ;
						strNext = strDir ;
						strNext += _T("\\") ; 
						strNext = strNext + wfd.cFileName ;

						this->TravelDirectory(strNext,fdi) ; 
					}
					else
					{// 否则为文件
						++fdi.nFileCount ;

						unsigned __int64 dwSize = 0;

						if(wfd.nFileSizeHigh>0)
						{
							dwSize = ((unsigned __int64)wfd.nFileSizeHigh)<<32 ;
						}

						dwSize = dwSize + (unsigned __int64)wfd.nFileSizeLow ;

						fdi.nTotalSize += dwSize ;

						if(m_pEventCallBack!=NULL)
						{
							m_pEventCallBack->OnFolderSizeReportInfo(fdi) ;
						}
					}
				}

				if(m_CurState==State_Pause)
				{// 如果暂停则,停下来等待
					this->Wait() ;
				}

				if(m_CurState==State_Stop)
				{
					break ;
				}
			}
			while(::FindNextFile(hFileFind,&wfd));
		}
		
		::FindClose(hFileFind) ;
		hFileFind = INVALID_HANDLE_VALUE ;
	}
}