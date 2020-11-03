/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "XCCoreDefine.h"
#include <map>

class CXCWinStorageRelative
{
public:
	CXCWinStorageRelative(void);
	~CXCWinStorageRelative(void);

	static bool MakeUnlimitFileName(CptString& strFileOrDirectory) ;
	static void RemoveUnlimitFileNameSymbol(CptString& strFileOrDirectory) ;
	static inline bool IsUnlimitFileName(const CptString& strFileOrDirectory) ;

	static bool GetFileStoreInfo(const CptString& strPath,SStorageInfoOfFile& siof) ;
	static DWORD GetDiskID(TCHAR cLetter) ;

private:
	static int GetPartitionIndex(TCHAR cLetter) ;
	static int GetDiskIndex(TCHAR cLetter) ;
	
private:
	static pt_STL_map(TCHAR,SStorageInfoOfFile)		m_PartitionLetter2SotrageInfoMap ; // ÅÌ·ûÎª´óÐ´
};

