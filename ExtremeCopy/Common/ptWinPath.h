/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#pragma once
#include "ptString.h"
#include <shlobj.h> 

class CptWinPath
{
public:
	enum EPathElementType
	{
		PET_Drive = 1<<0,
		PET_Path = 1<<1,
		PET_FileName = 1<<2 ,
		PET_ExtName = 1<<3 ,
		//PET_FileNameNoExt = 1<<4 ,
	};

	struct SPathElementInfo
	{
		UINT uFlag ;
		CptString strDrive ;
		CptString strPath ; // 该路径不包括最后的那个斜杠
		CptString strFileName ;
		CptString strExtName ;
		//CptString strFileNameNoExt ;
	};

	CptWinPath(void);
	~CptWinPath(void);

	// 结果不包含后面的斜杠
	static CptString GetSpecialPath(int nFolder) ;
	static CptString GetStartupPath() ;
	static CptString GetCurPEFileName() ;
	static bool GetPathElement(const TCHAR* pFullPath,SPathElementInfo& pei) ;
	static void SplitFileName(const TCHAR* pFileName,CptString& strFileNameNoExt,CptString& strExtName) ;

private:
	static int ReverseFindFirstChrPos(const TCHAR* pStr,bool bReverse, const TCHAR* pMachCharArray,const int nArraySize) ;
	inline static bool IsMachChar(const TCHAR ch,const TCHAR* pCharArray,const int nArraySize) ;

};
