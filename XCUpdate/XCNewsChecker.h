/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "../ExtremeCopy/Common/ptTime.h"
#include "..\ExtremeCopy\Common\ptString.h"
#include "..\ExtremeCopy\Common\ptTypeDef.h"

class CXCNewChecker
{
private:
	struct SNewsInfo
	{
		BYTE			byBetaVer ; // 0: 正式版， 1，2.。。n 为beta 版
		unsigned int	nFileSize ;
		CptString strDownloadFileURL ;
		SptVerionInfo	VerInfo ;
		CptString strName ; // 如： ExtremeCopy 

		SNewsInfo()
		{
			byBetaVer = 0 ;
			nFileSize = 0 ;
		}
	};

public:
	CXCNewChecker() ;
	virtual ~CXCNewChecker() ;

	bool CheckAndUpdateExtremeCopy(const time_t uLastUpdateTime) ;
	static bool IsChecked(const time_t& uLastUpdateTime) ;

private:
	bool ParseNewsXML(const char* p,SNewsInfo& ni) ;
	bool DownloadFile(CptString strURLFile,CptString strFolder) ;


};