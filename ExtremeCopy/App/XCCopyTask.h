/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "../Common/ptTypeDef.h"
#include <vector>
#include "..\Common\ptString.h"
#include "XCGlobal.h"

class TiXmlDocument ;


enum ETaskXMLErrorType
{
	TXMLET_Success,
	TXMLET_UnknownError,
	TXMLET_ThereAreNotSrcOrDstFileOrFolder,
	TXMLET_CantSupportFormatVersion,
	TXMLET_ExecuteError,
	TXMLET_ShowModeError,
	TXMLET_SameFileReactionError,
	TXMLET_SameFileConditionError,
	TXMLET_ErrorReactionError,
	TXMLET_OutOfRetryError,
	TXMLET_InvalidTaskXMLData,
};

class CXCCopyTask
{
public:
	CXCCopyTask(void);
	~CXCCopyTask(void);

	static bool ExecuteTaskFileByExtremeCopy(CptString& strTaskFile,const TCHAR* pXCExeFileFolder) ;
	
	static ETaskXMLErrorType ExecuteTaskFile(CptString& strTaskFile,const TCHAR* pXCExeFileFolder) ;
	static bool LaunchXCTaskInstanceViaNamePipe(const SXCCopyTaskInfo& TaskInfo,const TCHAR* pXCExeFileFolder,HWND hWnd,DWORD dwWaitMilliseconds) ;

	static ETaskXMLErrorType ConvertXMLFile2TaskInfo(const TCHAR* lpszXMLFile,pt_STL_vector(SXCCopyTaskInfo)& TaskVer) ;
	static ETaskXMLErrorType ConvertXML2TaskInfo(const CptString& strXML,pt_STL_vector(SXCCopyTaskInfo)& TaskVer) ;
	static ETaskXMLErrorType ConvertXML2TaskInfo(const char* pUTF8XML,pt_STL_vector(SXCCopyTaskInfo)& TaskVer) ;

	static bool ConvertTaskInfo2XMLFile(const pt_STL_vector(SXCCopyTaskInfo)& TaskVer,const CptString& strFileName) ;
	static bool ConvertTaskInfo2XMLString(const pt_STL_vector(SXCCopyTaskInfo)& TaskVer,char** ppUtf8XML,int& nStrLen) ;

private:

	inline static void ConverUtf8ToCurrentAppCode(const char* pUtf8,TCHAR* pOutStr,int nBufSize) ;

	static bool ConvertTaskInfo2XML(const pt_STL_vector(SXCCopyTaskInfo)& TaskVer,TiXmlDocument* pXMLDoc) ;
};
