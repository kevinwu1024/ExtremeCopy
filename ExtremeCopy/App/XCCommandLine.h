/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "../Common/ptCmdLineParser.h"
#include "../Common/ptTypeDef.h"
#include "XCCopyTask.h"
#include "XCGlobal.h"


struct SFilterCondition
{
	CptString strFilter ;
	bool		bFilterApplyToSubFolder ;

};


class CXCCommandLine
{
public:
	CXCCommandLine(void);
	~CXCCommandLine(void);

	static bool ParseCmdLine2TaskInfo(const TCHAR* pCmdLine,SXCCopyTaskInfo& cmd,CptString& strError) ;
	

	
private:
	static bool SetCmdParam(CptString strSwitchName,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;

	static bool SetCmdParam_Shell(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_Register(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_Config(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_About(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_SourceFile(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_DestinationFolder(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_Condition(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;

	static bool SetCmdParam_Error(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_Retry(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_Log(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_Action(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_SameFile(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_Show(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_TaskFile(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_OutOfRetry(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_TaskDialog(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;
	static bool SetCmdParam_CheckUpdate(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) ;

	inline static bool FormatCmdParamError(const TCHAR* pSwitch,const CptString& strParam,CptString& strError) ;
	inline static CptString GetParameterErrorString(const TCHAR* pSwitch) ;

	inline static CptString GetDoesntSupportError(const TCHAR* pSwitch) ;
};
