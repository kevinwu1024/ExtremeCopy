/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "stdafx.h"
#include "ptNamePipe.h"
#include <assert.h>
#include <stdio.h>
#include <process.h>

inline bool WaitReadData(HANDLE hFile,void* pDataBuf,int nBufSize,DWORD& dwRead,OVERLAPPED* pOverlap)
{
	BOOL bR = ::ReadFile(hFile,pDataBuf,nBufSize,&dwRead,pOverlap) ;

	return !(bR==FALSE && ERROR_IO_PENDING==GetLastError()) ;
}

CptNamePipeServer::CptNamePipeServer():m_bEndThread(true),m_hThread(NULL)
{
}

CptNamePipeServer::~CptNamePipeServer()
{
	this->Close() ;
}

UINT CptNamePipeServer::ProcessThreadFunc(LPVOID pParam)
{
	CptNamePipeServer* pThis = (CptNamePipeServer*)pParam ;

	struct SClientInfo
	{
		OVERLAPPED	overlap ;
		HANDLE		hPipe ;
		bool		bConned ;
		BYTE		byDataBuf[1024] ;

		SClientInfo()
		{
			hPipe = INVALID_HANDLE_VALUE ;
			bConned = false ;
			::memset(&overlap,0,sizeof(overlap)) ;
			overlap.hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;
		}

		~SClientInfo()
		{
			if(overlap.hEvent!=NULL)
			{
				::CloseHandle(overlap.hEvent) ;
				overlap.hEvent = NULL ;
			}
		}
	};

	DWORD dwNumOfBytesToRead ;
	DWORD dwWait = 0 ;
	int i = 0;
	int nSuccessCount = false ;
	

	const int nClientNum = min(pThis->m_nMaxClient,64) ;

	HANDLE* phEventArray = new HANDLE[nClientNum] ;

	SClientInfo* pCiArray = new SClientInfo[nClientNum] ;

	for (i=0; i<nClientNum; i++)
	{
		pCiArray[i].hPipe = ::CreateNamedPipe(pThis->m_szPipeName,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,nClientNum,PIPE_BUF_SIZE,PIPE_BUF_SIZE, 3*1000,NULL);

		if(pCiArray[i].hPipe!=INVALID_HANDLE_VALUE)
		{
			if(!ConnectNamedPipe(pCiArray[i].hPipe, &pCiArray[i].overlap) && ERROR_IO_PENDING==GetLastError())
			{
				phEventArray[i] = pCiArray[i].overlap.hEvent ;
				pCiArray[i].bConned = false ;
				++nSuccessCount ;
			}
		}
	}

	if(nSuccessCount==nClientNum)
	{
		while (!pThis->m_bEndThread)
		{
			dwWait = ::WaitForMultipleObjects(nClientNum,phEventArray, FALSE, 500);
			if(dwWait==WAIT_TIMEOUT)
			{
//				Debug_Printf("CptNamePipeServer::ProcessThreadFunc() 1") ;
				continue ;
			}
			if(dwWait==WAIT_ABANDONED)
			{
//				Debug_Printf("CptNamePipeServer::ProcessThreadFunc() 2") ;
				break ;
			}
			else
			{
				i = dwWait - WAIT_OBJECT_0 ;
				
				if(pCiArray[i].bConned)
				{// 已连接
					::GetOverlappedResult(pCiArray[i].hPipe,&pCiArray[i].overlap,&dwNumOfBytesToRead,FALSE) ;
					
					if(dwNumOfBytesToRead>0)
					{// 读取数据

//						cout << "服务端读取数据: " < dwNumOfBytesToRead << endl ;

//						Debug_Printf("CptNamePipeServer::ProcessThreadFunc() 3") ;

						if(pThis->m_pNpcb!=NULL)
						{
							do 
							{
								pThis->m_pNpcb->OnServerRecvData(pCiArray[i].hPipe,pCiArray[i].byDataBuf,
									dwNumOfBytesToRead) ;

							} while(::WaitReadData(pCiArray[i].hPipe,pCiArray[i].byDataBuf,sizeof(pCiArray[i].byDataBuf),
								dwNumOfBytesToRead,&pCiArray[i].overlap));

//							pCiArray[i].dwNumOfBytesToRead = 0 ;
							
//							::ReadFile(pCiArray[i].hPipe,pCiArray[i].byDataBuf,sizeof(pCiArray[i].byDataBuf),
//								&pCiArray[i].dwNumOfBytesToRead,&pCiArray[i].overlap) ;
						}
					}
					else
					{// 断开连接

//						Debug_Printf("CptNamePipeServer::ProcessThreadFunc() 4") ;

						if(pThis->m_pNpcb)
						{
							pThis->m_pNpcb->OnClose(pCiArray[i].hPipe) ;
						}

						::DisconnectNamedPipe(pCiArray[i].hPipe) ;
						::ConnectNamedPipe(pCiArray[i].hPipe, &pCiArray[i].overlap) ;
						pCiArray[i].bConned = false ;
					}
				}
				else
				{// 新连接
//					Debug_Printf("CptNamePipeServer::ProcessThreadFunc() 5") ;

					pCiArray[i].bConned = true ;

					if(pThis->m_pNpcb!=NULL)
					{
						pThis->m_pNpcb->OnNewClientConnect(pCiArray[i].hPipe) ;

						while(::WaitReadData(pCiArray[i].hPipe,pCiArray[i].byDataBuf,sizeof(pCiArray[i].byDataBuf),
														dwNumOfBytesToRead,&pCiArray[i].overlap))
						{
							pThis->m_pNpcb->OnServerRecvData(pCiArray[i].hPipe,pCiArray[i].byDataBuf,dwNumOfBytesToRead) ;
						}
					}
				}
			}
		}
	}

	if(pCiArray!=NULL)
	{
		delete [] pCiArray ;
	}

	if(phEventArray!=NULL)
	{
		delete [] phEventArray ;
	}

//	cout << "服务端退出线程" << endl ;

	return 0 ;
}

bool CptNamePipeServer::Create(TCHAR* szPipeName, CNamePipeCB* pNpcb,int nMaxClient,TCHAR* szHostName) 
{
	if(m_bEndThread)
	{
//		this->Close() ;
		m_nMaxClient = nMaxClient ;
		m_bEndThread = false ;
		m_pNpcb = pNpcb ;
		#pragma warning(push)
#pragma warning(disable:4996)
		::_stprintf(m_szPipeName,_T("\\\\%s\\pipe\\%s"),szHostName,szPipeName) ;
#pragma warning(pop)
		m_hThread = (HANDLE)::_beginthreadex(NULL,0,ProcessThreadFunc,this,0,NULL) ;
	}

	return true ;
}

int CptNamePipeServer::SendData(HANDLE hPipe,void* pData,DWORD dwNumOfBytes) 
{
	assert(pData!=NULL) ;
	DWORD dwWritten = 0 ;
	
	if(hPipe!=INVALID_HANDLE_VALUE && pData!=NULL)
	{
		WriteFile(hPipe,pData,dwNumOfBytes,&dwWritten,NULL);
	}

	return dwWritten ;
}

void CptNamePipeServer::GetPipeName(TCHAR* szPipeName,int nBufSize) 
{
	assert(szPipeName!=NULL) ;
	
	if(nBufSize>(int)::_tcslen(m_szPipeName))
	{
		::_tcscpy(szPipeName,m_szPipeName) ;
		//::strcpy(szPipeName,m_szPipeName) ;
	}
	else
	{
		szPipeName[0] = 0 ;
	}
}

void CptNamePipeServer::Close() 
{
	m_bEndThread = true ;
	
	if(m_hThread!=NULL)
	{
		::WaitForSingleObject(m_hThread,3*1000) ;
		::CloseHandle(m_hThread) ;
		m_hThread = NULL ;
	}
	
	if(m_pNpcb!=NULL)
	{
		m_pNpcb->OnClose(NULL) ;
	}
	
	m_pNpcb = NULL ;
}


/////////////////////////////////////////////////////////////////////////////////////
///////////////

CptNamePipeClient::CptNamePipeClient():m_hFile(INVALID_HANDLE_VALUE),m_bEndThread(true),m_pNpcb(NULL),m_hThread(NULL)
{
	::memset(m_szPipeName,0,sizeof(m_szPipeName)) ;
}

CptNamePipeClient::~CptNamePipeClient()
{
	this->Close() ;
}

bool CptNamePipeClient::Create(TCHAR* szPipeName, CNamePipeCB* pNpcb,TCHAR* szHostName) 
{
	assert(szHostName!=NULL) ;
	assert(szPipeName!=NULL) ;
	
	this->Close() ;
	
	bool bResult = false ;
	int	 nWaitCount = 3 ;

	if(m_hFile==INVALID_HANDLE_VALUE)
	{
#pragma warning(push)
#pragma warning(disable:4996)
		::_stprintf(m_szPipeName,_T("\\\\%s\\pipe\\%s"),szHostName,szPipeName) ;
#pragma warning(pop)
		
		while (!bResult && --nWaitCount>0)
		{
			m_hFile = ::CreateFile(m_szPipeName,GENERIC_WRITE|GENERIC_READ,0,NULL,
				OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);
			
			bResult = (m_hFile!=INVALID_HANDLE_VALUE)  ;

			if(bResult)
			{
				DWORD dwMode = PIPE_READMODE_MESSAGE;
				::SetNamedPipeHandleState(m_hFile,&dwMode,NULL,NULL) ;
			}

			if(!bResult && !::WaitNamedPipe(m_szPipeName,3000))
			{
				break ;
			}
		}
		

		if(bResult && pNpcb!=NULL)
		{// 打开一个接收线程
			m_bEndThread = false ;
			m_pNpcb = pNpcb ;
			m_hThread = (HANDLE)::_beginthreadex(NULL,0,RecvThreadFunc,this,0,NULL) ;
		}
	}

	return bResult ;
}

void CptNamePipeClient::Close()
{
	if(m_hFile!=INVALID_HANDLE_VALUE)
	{
		m_bEndThread = true ;

		if(m_hThread!=NULL)
		{
			::WaitForSingleObject(m_hThread,3*1000) ;
			::CloseHandle(m_hThread) ;
			m_hThread = NULL ;
		}

		if(m_pNpcb!=NULL)
		{
			m_pNpcb->OnClose(NULL) ;
		}
		
		::CloseHandle(m_hFile) ;
		m_hFile = INVALID_HANDLE_VALUE ;
		m_pNpcb = NULL ;
	}
}

int CptNamePipeClient::SendData(void* pData,DWORD dwNumOfBytes) 
{
	assert(pData!=NULL) ;
	DWORD dwWritten = 0 ;

	if(m_hFile!=INVALID_HANDLE_VALUE && pData!=NULL)
	{
		WriteFile(m_hFile,pData,dwNumOfBytes,&dwWritten,NULL);
	}

	return dwWritten ;
}

void CptNamePipeClient::GetPipeName(TCHAR* szPipeName,int nBufSize) 
{
	assert(szPipeName!=NULL) ;

	if(nBufSize>(int)::_tcslen(m_szPipeName))
	{
		::_tcscpy(szPipeName,m_szPipeName) ;
	}
	else
	{
		szPipeName[0] = 0 ;
	}
}


UINT CptNamePipeClient::RecvThreadFunc(LPVOID pParam)
{
	CptNamePipeClient* pThis = (CptNamePipeClient*)pParam ;

	BOOL bR ;
	DWORD dwResutl = 0 ;
	DWORD dwNumOfBytesToRead =0 ;
	BYTE Data[PIPE_BUF_SIZE] = {0} ;

	bool bConnect = false ;

	OVERLAPPED   overlap ;

	::memset(&overlap,0,sizeof(overlap)) ;

	overlap.hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;

	while(!pThis->m_bEndThread)
	{
		dwNumOfBytesToRead = 0 ;
		bR = ::ReadFile(pThis->m_hFile,Data,sizeof(Data),&dwNumOfBytesToRead,&overlap) ;

		if(bR==FALSE)
		{
			if(::GetLastError()==ERROR_IO_PENDING)
			{
				dwResutl = ::WaitForSingleObject(overlap.hEvent,500) ;

				if(dwResutl == WAIT_OBJECT_0)
				{
					::GetOverlappedResult(pThis->m_hFile,&overlap,&dwNumOfBytesToRead,FALSE) ;
					bR = TRUE ;
				}
				else if(dwResutl==WAIT_TIMEOUT)
				{
					continue ;
				}
				else
				{
					break ;
				}
			}
		}
		
		if(bR && dwNumOfBytesToRead>0)
		{
			pThis->m_pNpcb->OnClientRecvData(Data,dwNumOfBytesToRead)  ;
		}
		else
		{// 服务端主动关闭
			break ;
		}
	}
	
	::CloseHandle(pThis->m_hFile) ;
	pThis->m_hFile = INVALID_HANDLE_VALUE ;
//	pThis->Close() ;

//	cout << "客户端退出线程" << endl ;

	if(overlap.hEvent!=NULL)
	{
		::CloseHandle(overlap.hEvent) ;
		overlap.hEvent = NULL ;
	}
	
//	::CloseHandle(pThis->m_hFile) ;
//	pThis->m_hFile = NULL ;
	
	return 1 ;
}

