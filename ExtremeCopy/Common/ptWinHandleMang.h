
#pragma once
#include <set>
#include <Windows.h>
#include "ptThreadLock.h"


class CptWinHandleMang
{
public:
	static HANDLE CptWinHandleMang::CreateEvent(
		LPSECURITY_ATTRIBUTES lpEventAttributes,
		BOOL bManualReset,
		BOOL bInitialState,
		LPCTSTR lpName
		);

	static HANDLE CptWinHandleMang::CreateFile(
		LPCTSTR lpFileName, 
		DWORD dwDesiredAccess, 
		DWORD dwShareMode, 
		LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
		DWORD dwCreationDisposition, 
		DWORD dwFlagsAndAttributes, 
		HANDLE hTemplateFile
		);

	static HANDLE CptWinHandleMang::_beginthreadex( 
		void *security,
		unsigned stack_size,
		unsigned (__stdcall *start_address )( void * ),
		void *arglist,
		unsigned initflag,
		unsigned *thrdaddr 
		);

	static BOOL CptWinHandleMang::CloseHandle(HANDLE h) ;

	static bool IsHandleEmpty() ;
	
private:
	
	enum EHandleType
	{
		HT_File = 0,
		HT_Thread = 1,
		HT_Event = 2,
		HT_Mute = 3,
		HT_End = 4
	};

	struct SHandleInfo
	{
		HANDLE	hHandle ;
		unsigned int uAllocTimes ;
		EHandleType	ht ;

		bool operator ==(const SHandleInfo& hi) const
		{
			return (this->hHandle==hi.hHandle) ;
		}

		bool operator >(const SHandleInfo& hi) const
		{
			return ((uintptr_t)this->hHandle>(uintptr_t)hi.hHandle) ;
		}

		bool operator <(const SHandleInfo& hi) const
		{
			return ((uintptr_t)this->hHandle<(uintptr_t)hi.hHandle) ;
		}
	};

	struct SHandleRecorder
	{
		CptCritiSecLock	lock ;
		std::set<SHandleInfo>	HandleSet ;
		unsigned int	HandlerCounterArray[(int)HT_End] ;

		SHandleRecorder()
		{
			::memset(HandlerCounterArray,0,sizeof(HandlerCounterArray)) ;
		}
	};

#ifdef _DEBUG
private:

	static void AddHandle(EHandleType ht,HANDLE h) ;
	static void RemoveHandle(HANDLE h) ;

private:
	static SHandleRecorder	m_HandleRecor ;
#endif
};