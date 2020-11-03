/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

class CptThread;

class CptThreadTarget
{
	friend CptThread ;
public:
	CptThreadTarget():m_bStop(false){} 
protected:
	bool IsNeedStop() const {return m_bStop;}
	void PostEvent(bool bStop) {m_bStop=bStop;}

	virtual UINT ThreadRun(void* pArg) {return 0;}

private:
	bool m_bStop ;
};


class CptThread
{	
public:
	enum EState
	{
		State_Running,
		State_Stop,
		State_Waitting,
		State_Suspend,
	};

	CptThread(void);
	~CptThread(void);

	void ForceStop() ;
	void PostState(EState state) ;
	bool Start(CptThreadTarget* pTarget,void* pArg=NULL) ;

	EState GetState() const {return m_CurState;}
	void WaitForReleased(int nMilliSec) ;

private:
	static UINT __stdcall ThreadFunc(void* pArg) ;

	UINT ThreadFunc2() ;

private:
	CptThreadTarget*	m_pTarget ;
	void*				m_pTargetArg ;

	EState				m_CurState ;
	HANDLE				m_hThread ;
	HANDLE				m_hWaitEvent ;
};
