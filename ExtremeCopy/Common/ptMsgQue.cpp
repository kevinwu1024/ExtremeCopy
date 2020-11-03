/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptMsgQue.h"

CptMsgQue::CptMsgQue():m_pCurMsgPack(NULL)
{
}

CptMsgQue::~CptMsgQue() 
{
	this->Release() ;
}

void CptMsgQue::Release() 
{
	CptAutoLock lock(&m_Lock) ;

	if(m_pCurMsgPack!=NULL)
	{
		m_pCurMsgPack->bSuccess = false ;
		m_pCurMsgPack->pResult = NULL ;
		::SetEvent(m_pCurMsgPack->hEvent) ;
		::CloseHandle(m_pCurMsgPack->hEvent) ;
		//m_hEventList.push_back(m_pCurMsgPack->hEvent) ;
		m_pCurMsgPack = NULL ;
	}

	pt_STL_deque(SMsgPack*)::iterator itMsg = m_MsgQue.begin() ;

	for(;itMsg!=m_MsgQue.end();++itMsg)
	{
		(*itMsg)->bSuccess = false ;
		(*itMsg)->pResult = NULL ;
		::SetEvent((*itMsg)->hEvent) ;
		::CloseHandle((*itMsg)->hEvent) ;
	}

	pt_STL_list(HANDLE)::iterator itEvent = m_hEventList.begin() ;

	for(;itEvent!=m_hEventList.end();++itEvent)
	{
		::CloseHandle((*itEvent)) ;
	}

	m_hEventList.clear() ;
}

HANDLE CptMsgQue::GetEventHandle()
{
	HANDLE hRet = NULL ;
	if(m_hEventList.empty())
	{
		hRet = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;
	}
	else
	{
		hRet = m_hEventList.back() ;
		m_hEventList.pop_back() ;
	}

	return hRet ;
}

HANDLE CptMsgQue::AsynSendMsg(void* pData) 
{
	_ASSERT(pData!=NULL) ;

	CptAutoLock lock(&m_Lock) ;

	SMsgPack* pMp = this->AllocMsgBuf() ;//new SMsgPack();
	pMp->EventType = ET_Asyn ;
	pMp->pData = pData ;

	pMp->hEvent = this->GetEventHandle() ;

	m_MsgQue.push_back(pMp) ;

	return (HANDLE)pMp ;
}

bool CptMsgQue::WaitForAsynResult(HANDLE hAsy,void* pResult)
{
	_ASSERT(hAsy!=NULL) ;

	SMsgPack* pMp = (SMsgPack*)hAsy ;

	::WaitForSingleObject(pMp->hEvent,INFINITE) ;

	CptAutoLock lock(&m_Lock) ;

	m_hEventList.push_back(pMp->hEvent) ;

	pResult = pMp->pResult ;
	bool bRet = pMp->bSuccess ;
	//delete pMp ;
	this->FreeMsgBuf(pMp) ;

	return bRet ;
}

void CptMsgQue::PostMsg(void* pData) 
{
	//_ASSERT(pData!=NULL) ;

	CptAutoLock lock(&m_Lock) ;

	SMsgPack* pMp = this->AllocMsgBuf() ;//new SMsgPack();
	pMp->EventType = ET_Post ;
	pMp->pData = pData ;
	pMp->hEvent = NULL ;

	m_MsgQue.push_back(pMp) ;
}

bool CptMsgQue::RemoveMsg(void* pData)
{
	bool bRet = false ;

	CptAutoLock lock(&m_Lock) ;

	bool bLoop = true ;

	while(!m_MsgQue.empty() && bLoop)
	{
		bLoop = false ;

		pt_STL_deque(SMsgPack*)::iterator it = m_MsgQue.begin() ;

		for(;it!=m_MsgQue.end();++it)
		{
			if((*it)->pData==pData)
			{
				SMsgPack* pMp = (*it) ;

				m_MsgQue.erase(it) ;
				this->FreeMsgBuf(pMp) ;

				bLoop = true ;
				bRet = true ;
			}
		}
	}

	return bRet ;
}

bool CptMsgQue::SyncSendMsg(void* pData,void* pResult) 
{
	_ASSERT(pData!=NULL) ;

	SMsgPack mp ;
	mp.EventType = ET_Sync ;
	mp.pData = pData ;

	{
		CptAutoLock lock(&m_Lock) ;
		
		mp.hEvent = this->GetEventHandle() ;

		m_MsgQue.push_back(&mp) ;
	}

	::WaitForSingleObject(mp.hEvent,INFINITE) ;

	pResult = mp.pResult ;

	return mp.bSuccess ;
}

void* CptMsgQue::BeginMsg() 
{
	CptAutoLock lock(&m_Lock) ;

	if(!m_MsgQue.empty() && m_pCurMsgPack==NULL)
	{
		m_pCurMsgPack = m_MsgQue.front() ;
		m_MsgQue.pop_front() ;

		_ASSERT(m_pCurMsgPack!=NULL) ;

		return (m_pCurMsgPack==NULL) ? NULL : m_pCurMsgPack->pData ;
	}
	else
	{
		return NULL ;
	}
}

void CptMsgQue::EndMsg(void* pResult) 
{
	CptAutoLock lock(&m_Lock) ;

	if(m_pCurMsgPack!=NULL)
	{
		m_pCurMsgPack->bSuccess = true ;
		m_pCurMsgPack->pResult = pResult ;

		if(m_pCurMsgPack->hEvent!=NULL)
		{
			::SetEvent(m_pCurMsgPack->hEvent) ;

			//m_hEventList.push_back(m_pCurMsgPack->hEvent) ;
		}
	}

	m_pCurMsgPack = NULL ;
}

CptMsgQue::SMsgPack* CptMsgQue::AllocMsgBuf() 
{
	SMsgPack* pRet = NULL ;

	if(!m_MsgBufPool.empty())
	{
		pRet = m_MsgBufPool.front() ;
		m_MsgBufPool.pop_front() ;
	}
	else
	{
		pRet = new SMsgPack() ;
	}

	return pRet ;
}

void CptMsgQue::FreeMsgBuf(SMsgPack* pMp) 
{
	_ASSERT(pMp!=NULL) ;
	m_MsgBufPool.push_back(pMp) ;
}