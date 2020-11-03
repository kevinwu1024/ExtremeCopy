#pragma once

#include <Windows.h>

class CptDiskManager
{
public:
	CptDiskManager(void);
	~CptDiskManager(void);

	static int GetPartitionIndex(const TCHAR* pStr) ;
	static int GetDiskIndex(const TCHAR* pStr) ;
	static int GetDiskSector(const TCHAR* pStr) ;
	static int GetDiskSector(int nPartitionIndex) ;
	static DWORD GetDiskID(TCHAR cLetter)  ;

	//bool IsSameDisk(const char* pStr1,const char* pStr2) ;
};
