/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "stdafx.h"
#include "ptTime.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CptTime::CptTime()
{

}

CptTime::~CptTime()
{

}

void CptTime::Now(STimeInfo& si) 
{
	time_t now = ::time(NULL) ;

	tm* pTm = ::localtime(&now) ;

	si.wYear	= pTm->tm_year+1900 ;
	si.wMonth	= pTm->tm_mon+1 ;
	si.wDay		= pTm->tm_mday ;
	si.wHour	= pTm->tm_hour ;
	si.wMinute	= pTm->tm_min ;
	si.wSecond	= pTm->tm_sec ;
}

void CptTime::Now(TCHAR* pcStrBuf) 
{
	assert(pcStrBuf!=NULL) ;

	time_t now = ::time(NULL) ;
	
	tm* pTm = ::localtime(&now) ;

#pragma warning(push)
#pragma warning(disable:4996)
	::_stprintf(pcStrBuf,_T("%04d-%02d-%02d %02d:%02d:%02d"),
				pTm->tm_year+1900,
				pTm->tm_mon+1,
				pTm->tm_mday,
				pTm->tm_hour,
				pTm->tm_min,
				pTm->tm_sec) ;
#pragma warning(pop)

}

time_t CptTime::Now() 
{
	return ::time(NULL) ;

	////return ::time(NULL) + GMT_OFFSET_HOUR*60*60 ;
}

void CptTime::Today(TCHAR* pcBuf) 
{
	assert(pcBuf!=NULL) ;

	time_t now = ::time(NULL) ;
	
	tm* pTm = ::localtime(&now) ;
	
#pragma warning(push)
#pragma warning(disable:4996)

	::_stprintf(pcBuf,_T("%04d-%02d-%02d"),
		pTm->tm_year+1900,
		pTm->tm_mon+1,
		pTm->tm_mday) ;

#pragma warning(pop)
}

void CptTime::Time(TCHAR* pcBuf) 
{
	assert(pcBuf!=NULL) ;

	time_t now = ::time(NULL) ;
	
	tm* pTm = ::localtime(&now) ;
	
	#pragma warning(push)
#pragma warning(disable:4996)

	::_stprintf(pcBuf,_T("%02d:%02d:%02d"),
		pTm->tm_hour,
		pTm->tm_min,
		pTm->tm_sec) ;
#pragma warning(pop)

}

time_t CptTime::Different(const TCHAR* pcDateTime1,const TCHAR* pcDateTime2) 
{
	assert(pcDateTime1!=NULL) ;
	assert(pcDateTime2!=NULL) ;

	return (CptTime::Str2Sec(pcDateTime1)-CptTime::Str2Sec(pcDateTime2)) ;
}

time_t CptTime::Different(const STimeInfo& si1,const STimeInfo& si2) 
{
	return (CptTime::Info2Sec(si1)-CptTime::Info2Sec(si2)) ;
}

bool CptTime::IsInRange(const TCHAR* pcTargetDateTime,const TCHAR* pcBeginDateTime,const TCHAR* pcEndDateTime) 
{
	assert(pcTargetDateTime!=NULL) ;
	assert(pcBeginDateTime!=NULL) ;
	assert(pcEndDateTime!=NULL) ;

	STimeInfo TarSi ;
	STimeInfo BeginSi ;
	STimeInfo EndSi ;

	CptTime::Str2Info(TarSi,pcTargetDateTime) ;
	CptTime::Str2Info(BeginSi,pcBeginDateTime) ;
	CptTime::Str2Info(EndSi,pcEndDateTime) ;

	return CptTime::IsInRange(TarSi,BeginSi,EndSi) ;
}

bool CptTime::IsInRange(const STimeInfo& ti,const STimeInfo& tiBegin,const STimeInfo& tiEnd) 
{
	time_t lTarget = CptTime::Info2Sec(ti) ;
	time_t lBegin = CptTime::Info2Sec(tiBegin) ;
	time_t lEnd = CptTime::Info2Sec(tiEnd) ;
	
	return (lTarget>lBegin && lTarget<lEnd) ;
}

bool CptTime::IsValidFormat(const TCHAR* pcDateTime) 
{
	assert(pcDateTime!=NULL) ;

	if(::_tcslen(pcDateTime)<21)
	{
		int nTem = 0 ;
		TCHAR szCopy[20+1] = {0} ;
		::_tcscpy(szCopy,pcDateTime) ;
		
		TCHAR* pcTem = ::_tcstok(szCopy,_T("-")) ;
		
		pcTem = ::_tcstok(NULL,_T("-")) ;
		nTem = ::_tstoi(pcTem) ;

		if(nTem>0 && nTem<13)
		{
			pcTem = ::_tcstok(NULL,_T(" ")) ;
			nTem = ::_tstoi(pcTem) ;

			if(nTem>0 && nTem<32)
			{
				pcTem = ::_tcstok(NULL,_T(":")) ;
				nTem = ::_tstoi(pcTem) ;

				if(nTem>=0 && nTem<24)
				{
					pcTem = ::_tcstok(NULL,_T(":")) ;
					nTem = ::_tstoi(pcTem) ;
					if(nTem>=0 && nTem<60)
					{
						pcTem = ::_tcstok(NULL,_T(":")) ;
						nTem = ::_tstoi(pcTem) ;
						if(nTem>=0 && nTem<60)
						{
							return true ;
						}
					}
				}
			}
		}
		
	}
	
	return false ;
}

void CptTime::Info2Str(TCHAR* pcDateTime,const STimeInfo& si) 
{
	assert(pcDateTime!=NULL) ;

	#pragma warning(push)
#pragma warning(disable:4996)
	::_stprintf(pcDateTime,_T("%04d-%02d-%02d %02d:%02d:%02d"),
				si.wYear,
				si.wMonth,
				si.wDay,
				si.wHour,
				si.wMinute,
				si.wSecond) ;
#pragma warning(pop)
}

bool CptTime::Str2Info(STimeInfo& si,const TCHAR* pcDateTime) 
{
	assert(pcDateTime!=NULL) ;

	if(!CptTime::IsValidFormat(pcDateTime))
	{
		return false ;
	}

	TCHAR szCopy[20+1] = {0} ;

	::_tcscpy(szCopy,pcDateTime) ;

	TCHAR* pcTem = ::_tcstok(szCopy,_T("-")) ;
	si.wYear = ::_tstoi(pcTem) ;

	pcTem = ::_tcstok(NULL,_T("-")) ;
	si.wMonth = ::_tstoi(pcTem) ;

	pcTem = ::_tcstok(NULL,_T(" ")) ;
	si.wDay = ::_tstoi(pcTem) ;

	pcTem = ::_tcstok(NULL,_T(":")) ;
	si.wHour = ::_tstoi(pcTem) ;

	pcTem = ::_tcstok(NULL,_T(":")) ;
	si.wMinute = ::_tstoi(pcTem) ;

	pcTem = ::_tcstok(NULL,_T(":")) ;
	si.wSecond = ::_tstoi(pcTem) ;	

	return true ;
}

void CptTime::CalulateDiff(STimeInfo& si,const long nDiffVal) 
{
	si = CptTime::Sec2Info(CptTime::Info2Sec(si) + nDiffVal) ;
}

time_t CptTime::Info2Sec(const STimeInfo& si) 
{
	tm Tm ;
	
	Tm.tm_year  =si.wYear-1900 ;
	Tm.tm_mon   =si.wMonth-1;
	Tm.tm_mday  =si.wDay ;		
	Tm.tm_hour  =si.wHour + GMT_OFFSET_HOUR ;	
	Tm.tm_min   =si.wMinute ;
	Tm.tm_sec   =si.wSecond ;
	
	return ::mktime(&Tm) ;
}

time_t CptTime::Str2Sec(const TCHAR* pcDateTime) 
{
	assert(pcDateTime!=NULL) ;

	if(!CptTime::IsValidFormat(pcDateTime))
	{
		return -1 ;
	}

	STimeInfo si ;

	CptTime::Str2Info(si,pcDateTime) ;

	return CptTime::Info2Sec(si) ;
}

STimeInfo CptTime::Sec2Info(time_t lTime) 
{
	lTime -= GMT_OFFSET_HOUR*60*60 ;
	
	tm* pTm = localtime(&lTime);
	
	STimeInfo si ;

	if(pTm!=NULL)
	{
		si.wYear	= pTm->tm_year+1900 ;
		si.wMonth	= pTm->tm_mon+1 ;
		si.wDay		= pTm->tm_mday ;
		si.wHour	= pTm->tm_hour ;
		si.wMinute	= pTm->tm_min ;
		si.wSecond	= pTm->tm_sec ;
	}
	else
	{
		::memset(&si,0,sizeof(si)) ;
	}

	return si ;
}

void CptTime::Sec2Str(const long& lTime,TCHAR* pcDateTime) 
{
	const STimeInfo ti = Sec2Info(lTime) ;

#pragma warning(push)
#pragma warning(disable:4996)
	::_stprintf(pcDateTime,_T("%04d-%02d-%02d %02d:%02d:%02d"),
		ti.wYear,
		ti.wMonth,
		ti.wDay,
		ti.wHour,
		ti.wMinute,
		ti.wSecond) ;
#pragma warning(pop)
}
