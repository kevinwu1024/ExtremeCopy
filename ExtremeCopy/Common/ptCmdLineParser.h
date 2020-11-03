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
#include <map>
#include <vector>
#include <string>
#include "ptTypeDef.h"
#include <vector>

class CptCmdLineParser
{
public:
	CptCmdLineParser(void);
	~CptCmdLineParser(void);

	// 返回的是 switch 的个数
	int Parse(const TCHAR* pCmdLine) ;

	int GetSwitchParamCount(const TCHAR* pSwitch) ;

	CptString GetParam(const TCHAR* pSwitch,int nIndex,const TCHAR* pDefaultValue) ;
	bool GetParams(const TCHAR* pSwitch,pt_STL_vector(CptString)& strParamVer) ;

	int GetSwitchCount() const ;
	CptString GetSwitchNameByIndex(int nIndex) const;

	bool IsContainSwitch(const TCHAR* pSwitch) const ;
	void RemoveSwitch(const TCHAR* pSwitch) ;

private:
	inline bool IsSwitchChar(const TCHAR ch) const ;

	int SplitSwitchGroup(const TCHAR* pCmdLine,pt_STL_vector(CptString)& SwitchLineVer) ;
	void ParseSwitchLine(CptString strSwitchLine) ;
	inline int GetSwitchNameOffset(const TCHAR* pSwitchName) const ;

private:

	typedef pt_STL_map(std::basic_string<TCHAR>,int)		SwitchContainer_t ;
	typedef pt_STL_vector(pt_STL_vector(CptString) ) ParamContainer_t ;
	
	SwitchContainer_t		m_SwitchsMap ;
	ParamContainer_t		m_ParamsVer ;
};
