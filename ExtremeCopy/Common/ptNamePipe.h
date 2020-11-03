/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#if !defined(AFX_NAMEPIPE_H__81E6BAE6_504A_44F1_8CF3_665375312E87__INCLUDED_)
#define AFX_NAMEPIPE_H__81E6BAE6_504A_44F1_8CF3_665375312E87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#define PIPE_BUF_SIZE 8*1024
#define PIPE_TIMEOUT  (120*1000) /*120 seconds*/

class CNamePipeCB
{
public:
	virtual void OnServerRecvData(HANDLE hPipe,void* pData,int nNumOfByte) {};
	virtual void OnNewClientConnect(HANDLE hPipe) {} ;
	virtual void OnClientRecvData(void* pData,int nNumOfByte) {};

	virtual void OnClose(HANDLE hPipe) {};
};

class CptNamePipeServer 
{
protected:
	static UINT __stdcall ProcessThreadFunc(LPVOID pParam) ;
	
public:
	CptNamePipeServer();
	virtual ~CptNamePipeServer();

	bool Create(TCHAR* szPipeName,CNamePipeCB* pNpcb,int nMaxClient=1,TCHAR* szHostName=_T(".")) ;

	int SendData(HANDLE hPipe,void* pData,DWORD dwNumOfBytes) ;

	void GetPipeName(TCHAR* szPipeName,int nBufSize) ;

	void Close() ;

	bool IsCreate() const {return m_bEndThread;}


private:
	bool         m_bEndThread ;
	TCHAR        m_szPipeName[MAX_PATH+1] ;
	OVERLAPPED   m_Overlapped ;

	CNamePipeCB* m_pNpcb ;
	int			m_nMaxClient ;

	HANDLE		m_hThread ;
};


class CptNamePipeClient
{
protected:
	
	static UINT __stdcall RecvThreadFunc(LPVOID pParam) ;
	
public:
	CptNamePipeClient();
	virtual ~CptNamePipeClient();
	
	bool Create(TCHAR* szPipeName,CNamePipeCB* pNpcb,TCHAR* szHostName=_T(".")) ;
	
	int SendData(void* pData,DWORD dwNumOfBytes) ;
	
	void GetPipeName(TCHAR* szPipeName,int nBufSize) ;
	
	void Close() ;
	
	bool IsCreate() const {return m_bEndThread;}
	
private:
	bool         m_bEndThread ;
	TCHAR        m_szPipeName[MAX_PATH+1] ;
	
	CNamePipeCB* m_pNpcb ;
	HANDLE      m_hFile ;
	HANDLE		m_hThread ;
};

#endif // !defined(AFX_NAMEPIPE_H__81E6BAE6_504A_44F1_8CF3_665375312E87__INCLUDED_)
