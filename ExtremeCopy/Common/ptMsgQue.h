/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "ptThreadLock.h"
#include "ptTypeDef.h"
#include <deque>
#include <list>

class CptMsgQue
{
public:
	CptMsgQue() ;
	~CptMsgQue() ;

	bool SyncSendMsg(void* pData,void* pResult) ;
	void PostMsg(void* pData) ;
	HANDLE AsynSendMsg(void* pData) ;
	bool WaitForAsynResult(HANDLE hAsy,void* pResult) ;

	void* BeginMsg() ;
	void EndMsg(void* pResult) ;
	bool RemoveMsg(void* pData) ;

	void Release() ;

private:

	enum EEventType
	{
		ET_Sync,
		ET_Asyn,
		ET_Post
	};

	struct SMsgPack
	{
		HANDLE	hEvent ;
		void*	pData ;
		void*	pResult ;
		bool	bSuccess ;
		//bool	bSync ;
		EEventType	EventType ;

		SMsgPack():hEvent(NULL),pData(NULL),pResult(NULL),bSuccess(false),EventType(ET_Sync)
		{
		}
	};

private:
	inline HANDLE GetEventHandle() ;
	inline SMsgPack* AllocMsgBuf() ;
	inline void FreeMsgBuf(SMsgPack* pMp) ;

private:
	CptCritiSecLock					m_Lock ;
	pt_STL_deque(SMsgPack*)			m_MsgQue ;
	pt_STL_list(HANDLE)				m_hEventList ; // send message µÄ event pool

	pt_STL_list(SMsgPack*)			m_MsgBufPool ;
	SMsgPack*						m_pCurMsgPack ;
};