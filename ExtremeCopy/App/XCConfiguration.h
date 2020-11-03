/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "XCGlobal.h"


class CXCConfiguration
{
public:
	CXCConfiguration(void);
	~CXCConfiguration(void);

	static CXCConfiguration* GetInstance()  ;
	static void Release() ;

	const SGlobalData* GetGlobalData() ;

	void LoadConfigDataFromFile(SConfigData& config,HMODULE hModule=::GetModuleHandle(NULL)) const ;
	void SaveConfigDataToFile(const SConfigData& config) const;

	void LoadConfigShareFromFile(SXCTaskAndConfigShare& ConfigShare,HMODULE hModule=::GetModuleHandle(NULL)) const ;
	void SaveConfigShareToFile(const SXCTaskAndConfigShare& ConfigShare) const ;

	bool GetIniFile(TCHAR* szIniFile,HMODULE hModule) const;

private:
	void LoadGlobalData() ;

private:
	static CXCConfiguration* m_pInstance ;

	SGlobalData		m_GlobalData ;
	bool			m_bIniGlobalData ;
};
