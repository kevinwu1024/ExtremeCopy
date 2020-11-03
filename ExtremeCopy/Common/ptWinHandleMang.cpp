

#include "stdafx.h"
#include <process.h>
#include "ptWinHandleMang.h"

#ifdef _DEBUG
CptWinHandleMang::SHandleRecorder	CptWinHandleMang::m_HandleRecor ;

bool CptWinHandleMang::IsHandleEmpty()
{
	bool bRet = true ;

#ifdef _DEBUG
	CptAutoLock lock(&m_HandleRecor.lock) ;

	bRet = m_HandleRecor.HandleSet.empty() ;
#endif

	return bRet ;
}

void CptWinHandleMang::AddHandle(EHandleType ht,HANDLE h) 
{
	CptAutoLock lock(&m_HandleRecor.lock) ;

	SHandleInfo hi ;

	hi.uAllocTimes = ++m_HandleRecor.HandlerCounterArray[ht] ;
	hi.hHandle = h ;
	hi.ht = ht ;

	std::set<SHandleInfo>::const_iterator it = m_HandleRecor.HandleSet.find(hi) ;

	_ASSERT(it==m_HandleRecor.HandleSet.end()) ;

	m_HandleRecor.HandleSet.insert(hi) ;
}

void CptWinHandleMang::RemoveHandle(HANDLE h) 
{
	CptAutoLock lock(&m_HandleRecor.lock) ;

	SHandleInfo hi ;

	hi.hHandle = h ;

	std::set<SHandleInfo>::iterator it = m_HandleRecor.HandleSet.find(hi) ;
	
	_ASSERT(it!=m_HandleRecor.HandleSet.end()) ;

	m_HandleRecor.HandleSet.erase(it) ;
}
#endif

BOOL CptWinHandleMang::CloseHandle(HANDLE h) 
{
#ifdef _DEBUG
	CptWinHandleMang::RemoveHandle(h) ;
#endif
	return ::CloseHandle(h) ;
}

HANDLE CptWinHandleMang::CreateEvent(
	LPSECURITY_ATTRIBUTES lpEventAttributes,
	BOOL bManualReset,
	BOOL bInitialState,
	LPCTSTR lpName
	)
{
	HANDLE hRet = ::CreateEvent(
		lpEventAttributes,
		bManualReset,
		bInitialState,
		lpName
		);

#ifdef _DEBUG
	_ASSERT(hRet!=NULL) ;
	CptWinHandleMang::AddHandle(HT_Event,hRet) ;
#endif
	return hRet ;
}


HANDLE CptWinHandleMang::CreateFile(
	LPCTSTR lpFileName, 
	DWORD dwDesiredAccess, 
	DWORD dwShareMode, 
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
	DWORD dwCreationDisposition, 
	DWORD dwFlagsAndAttributes, 
	HANDLE hTemplateFile
	)
{
	HANDLE hRet =::CreateFile(
		lpFileName, 
		dwDesiredAccess, 
		dwShareMode, 
		lpSecurityAttributes, 
		dwCreationDisposition, 
		dwFlagsAndAttributes, 
		hTemplateFile
		) ;

#ifdef _DEBUG
	_ASSERT(hRet!=NULL) ;
	CptWinHandleMang::AddHandle(HT_File,hRet) ;
#endif
	return hRet ;
}

HANDLE CptWinHandleMang::_beginthreadex( 
	void *security,
	unsigned stack_size,
	unsigned (__stdcall *start_address )( void * ),
	void *arglist,
	unsigned initflag,
	unsigned *thrdaddr 
	)
{
	HANDLE hRet = (HANDLE)::_beginthreadex( 
		security,
		stack_size,
		start_address,
		arglist,
		initflag,
		thrdaddr 
		) ;

#ifdef _DEBUG
	_ASSERT(hRet!=NULL) ;
	CptWinHandleMang::AddHandle(HT_Thread,hRet) ;
#endif
	return hRet ;
}