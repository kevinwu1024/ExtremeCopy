/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include <time.h>
#include <memory.h>

struct STimeInfo
{
	unsigned short	wYear ; 
	unsigned short	wMonth; 
	unsigned short	wDay; 
	unsigned short	wHour; 
	unsigned short	wMinute; 
	unsigned short	wSecond; 
	
	bool operator>(const STimeInfo& ti) const
	{
		return (wYear>ti.wYear || wMonth>ti.wMonth || wDay>ti.wDay || wHour>ti.wHour || wMinute>ti.wMinute || wSecond>ti.wSecond) ;
	}
	
	bool operator<(const STimeInfo& ti) const
	{
		return ti>(*this) ;
	}
	
	bool operator>=(const STimeInfo& ti) const
	{
		return (*this>ti || *this==ti);
	}
	
	bool operator<=(const STimeInfo& ti) const
	{
		return (*this<ti || *this==ti);
	}
	bool operator==(const STimeInfo& ti) const
	{
		return (::memcmp(this,&ti,sizeof(STimeInfo))==0) ;
	}
};

class CptTime  
{
public:

	CptTime();
	/* virtual */ ~CptTime();  // 当重载该类时请把virtual注释去掉

	static void Now(STimeInfo& si) ;
	static void Now(TCHAR* pcBuf);
	static void Today(TCHAR* pcBuf) ;
	static void Time(TCHAR* pcBuf) ;
	static time_t Now() ;
	static time_t Different(const TCHAR* pcDateTime1,const TCHAR* pcDateTime2) ;
	static time_t Different(const STimeInfo& si1,const STimeInfo& si2) ;
	static void Info2Str(TCHAR* pcDateTime,const STimeInfo& si) ;
	static bool Str2Info(STimeInfo& si,const TCHAR* pcDateTime) ;
	static bool IsInRange(const STimeInfo& ti,const STimeInfo& tiBegin,const STimeInfo& tiEnd) ;
	static bool IsInRange(const TCHAR* pcTargetDateTime,const TCHAR* pcBeginDateTime,const TCHAR* pcEndDateTime) ;
	static void CalulateDiff(STimeInfo& si,const long nDiffVal) ;
	static time_t Info2Sec(const STimeInfo& si) ;
	static time_t Str2Sec(const TCHAR* pcDateTime)  ;
	static bool IsValidFormat(const TCHAR* pcDateTime) ;
	static void Sec2Str(const long& lTime,TCHAR* pcDateTime) ;
	static STimeInfo Sec2Info(time_t lTime) ;

private:

#define GMT_OFFSET_HOUR			8
};

