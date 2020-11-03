/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptThread.h"
#include <process.h>

CptThread::CptThread(void):m_CurState(State_Stop),m_pTarget(NULL),m_hThread(NULL),m_hWaitEvent(NULL)
{
}

CptThread::~CptThread(void)
{
	if(m_hThread!=NULL)
	{
		this->PostState(State_Stop) ;
		this->WaitForReleased(3*1000) ;
	}
}


bool CptThread::Start(CptThreadTarget* pTarget,void* pArg)
{
	bool bRet = false ;

	if((m_CurState==State_Stop || m_CurState==State_Waitting) && pTarget!=NULL)
	{
		m_pTarget = pTarget ;
		m_pTargetArg = pArg ;
		m_CurState = State_Running ;

		if(m_hThread==NULL)
		{
			m_hThread = (HANDLE)::_beginthreadex(NULL,0,ThreadFunc,this,0,NULL) ;

			bRet = m_hThread!=NULL ;
		}
		else
		{
			::ResumeThread(m_hThread) ;
		}
	}

	return bRet ;
}

UINT CptThread::ThreadFunc(void* pArg)
{
	CptThread* pThis = (CptThread*)pArg ;

	return pThis->ThreadFunc2() ;
}

UINT CptThread::ThreadFunc2()
{
	UINT uRet = 0 ;

	while(m_CurState!=State_Stop)
	{
		if(m_CurState==State_Running)
		{
			uRet = m_pTarget->ThreadRun(m_pTargetArg) ;
		}

		switch(m_CurState)
		{
		case State_Running:
		case State_Waitting:
			{
				if(m_hWaitEvent==NULL)
				{
					m_hWaitEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;
				}

				m_CurState = State_Waitting ;
				::WaitForSingleObject(m_hWaitEvent,INFINITE) ;
			}
			
			break ;

		case State_Suspend:
			::SuspendThread(m_hThread) ;
			break ;
		}
	}

	::CloseHandle(m_hThread) ;
	m_hThread = NULL ;

	return uRet ;
}

void CptThread::WaitForReleased(int nMilliSec)
{
	if(m_hThread!=NULL)
	{
		::WaitForSingleObject(m_hThread,nMilliSec) ;
		::CloseHandle(m_hThread) ;
		m_hThread = NULL ;
	}
}

void CptThread::PostState(EState state)
{
	if(m_CurState!=state)
	{
		switch(state)
		{
		case State_Stop:
			{
				m_CurState = State_Stop ;
				if(m_hWaitEvent!=NULL)
				{
					::SetEvent(m_hWaitEvent) ;
					::CloseHandle(m_hWaitEvent) ;
				}

				if(m_pTarget!=NULL)
				{
					m_pTarget->PostEvent(true) ;
				}
			}
			break ;

		case State_Waitting:
			m_CurState = State_Waitting ;
			if(m_pTarget!=NULL)
			{
				m_pTarget->PostEvent(true) ;
			}
			break ;

		case State_Suspend:
			m_CurState = State_Suspend ;
			if(m_pTarget!=NULL)
			{
				m_pTarget->PostEvent(true) ;
			}
			break ;
		}
	}
}