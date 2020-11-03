/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "AppRegister.h"
#include <shlobj.h>
#include "XCGlobal.h"
#include <time.h>

static const TCHAR* s_szRegisterFileName = _T("genflpui.dat") ;

bool CheckRegisterCode2(const CptStringList& sl,int prog) ;

CAppRegister::CAppRegister(void):m_hFile(INVALID_HANDLE_VALUE)
{
	::memset(&m_RegInfo,0,sizeof(m_RegInfo)) ;

	GetBeginDate() ;
}

CAppRegister::~CAppRegister(void)
{
	if(m_hFile!=INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hFile) ;
		m_hFile = INVALID_HANDLE_VALUE ;
	}
}

bool CAppRegister::IsExpired()  
{
	const int nTrialDays = 30 ;

	if(m_RegInfo.bRegister || (::time(NULL)>=m_RegInfo.nTime && ::time(NULL) - m_RegInfo.nTime< nTrialDays*24*60*60 && !m_RegInfo.bPrompt))
	{
		struct tm*  p = ::gmtime((time_t*)&m_RegInfo.nTime) ;

		if((p->tm_year + 1900 == m_RegInfo.nFirstYear
			&& p->tm_mon+1 == m_RegInfo.nFirstMonth
			&& p->tm_mday == m_RegInfo.nFirstDay
			&& p->tm_hour == m_RegInfo.nFirstHour))
		{
			return false ;
		}
	}

	m_RegInfo.bPrompt = 1 ;

	this->WriteRegInfoToFile() ;

	return true ;
}

void CAppRegister::WriteRegInfoToFile() 
{
	if(m_hFile!=INVALID_HANDLE_VALUE)
	{
		DWORD dwWrite = 0; 

		::SetFilePointer(m_hFile,0,0,FILE_BEGIN) ;
		::SetEndOfFile(m_hFile) ;
		::WriteFile(m_hFile,&m_RegInfo,sizeof(m_RegInfo),&dwWrite,NULL) ;
		::FlushFileBuffers(m_hFile) ;
	}
}

bool CAppRegister::CheckRegister(HWND hParentWnd)
{
	this->GetBeginDate(true) ;

	CptStringList sl ;
	GetRegisterCode(sl) ;

	bool bRet = true ;

	if(this->IsExpired())
	{
		this->ShowRegisterDlg(hParentWnd) ;
	}
	else
	{
	}

	return bRet ;
}

bool CAppRegister::ShowRegisterDlg(HWND hParentWnd)
{
	CRegisterDlg dlg(hParentWnd) ;

	dlg.ShowDialog() ;

	bool bRet = ::CheckRegisterCode2(dlg.GetSeriesNumber(),1) ;

	if(bRet)
	{
		m_RegInfo.bRegister = 1 ;
		this->WriteRegInfoToFile() ;
	}

	return bRet ;
}

bool CAppRegister::IsAppRegistedReadOnly() 
{
	this->GetBeginDate(false) ;

	return this->IsAppRegisted() ;
}

bool CAppRegister::IsAppRegisted() const
{
	return m_RegInfo.bRegister ? true : false ;
}

SYSTEMTIME CAppRegister::GetBeginDate(bool bRewrite) 
{
	SYSTEMTIME timeRet ;

	::memset(&timeRet,0,sizeof(timeRet)) ;

	TCHAR szBuf[MAX_PATH] = {0} ;

	//if(::SHGetSpecialFolderPath(NULL,szBuf,CSIDL_SYSTEM,FALSE))
	if(::SHGetSpecialFolderPath(NULL,szBuf,CSIDL_APPDATA,FALSE))
	{
		const int nLen = (const int)::_tcslen(szBuf) ;

		if(szBuf[nLen-1]!='\\')
		{
			::_tcscat(szBuf,_T("\\ExtremeCopy")) ;
		}
		else
		{
			::_tcscat(szBuf,_T("ExtremeCopy")) ;
		}

		if(!CptGlobal::IsFolder(szBuf))
		{
			if(!::CreateDirectory(szBuf,NULL))
			{
				return timeRet ;
			}
		}

		::_tcscat(szBuf,_T("\\")) ;
		::_tcscat(szBuf,s_szRegisterFileName) ;

		DWORD dwAccess = GENERIC_READ ;

		if(bRewrite)
		{
			dwAccess = GENERIC_READ | GENERIC_WRITE ;
		}

		HANDLE hFile = ::CreateFile(szBuf,dwAccess,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL)  ;

		m_hFile = hFile ;
		if(hFile!=INVALID_HANDLE_VALUE)
		{
			DWORD dwRead = 0 ;
			::memset(szBuf,0,sizeof(szBuf)) ;

			::ReadFile(hFile,szBuf,sizeof(szBuf),&dwRead,NULL) ;

			if(dwRead==sizeof(SRegisterInfo))
			{
				::memcpy(&m_RegInfo,szBuf,dwRead) ;
			}
			else
			{
				SYSTEMTIME LocalTime ;
				::GetSystemTime(&LocalTime) ;

				m_RegInfo.nFirstDay = LocalTime.wDay;
				m_RegInfo.nFirstMinute = LocalTime.wMinute;
				m_RegInfo.nFirstMonth = LocalTime.wMonth;
				m_RegInfo.nFirstYear = LocalTime.wYear;
				m_RegInfo.nFirstTimeSeconds = LocalTime.wSecond;
				m_RegInfo.nFirstHour = LocalTime.wHour ;
				m_RegInfo.nTime = ::time(NULL) ;

				::srand((unsigned int)::time(0)) ;
				int i = 0 ;

				m_RegInfo.nRandom1 = ::rand() ;
				m_RegInfo.nRandom2 = ::rand() ;
				m_RegInfo.nRandom3 = ::rand() ;
				m_RegInfo.nRandom4 = ::rand() ;
				m_RegInfo.nRandom5 = ::rand() ;
				m_RegInfo.nRandom6 = ::rand() ;

				for(i=0;i<sizeof(m_RegInfo.RandomArray1);++i)
				{
					m_RegInfo.RandomArray1[i] = ::rand() ;
				}

				for(i=0;i<sizeof(m_RegInfo.RandomArray2);++i)
				{
					m_RegInfo.RandomArray2[i] = ::rand() ;
				}

				for(i=0;i<sizeof(m_RegInfo.RandomArray3);++i)
				{
					m_RegInfo.RandomArray3[i] = ::rand() ;
				}

				for(i=0;i<sizeof(m_RegInfo.RandomArray4);++i)
				{
					m_RegInfo.RandomArray4[i] = ::rand() ;
				}

				if(bRewrite)
				{
					this->WriteRegInfoToFile() ;
				}

			}
		}
	}

	return timeRet ;
}