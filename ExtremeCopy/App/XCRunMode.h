/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "UI/MainDialog.h"

#include "UI\ConfigurationDialog.h"
#include "UI\ptMessageBox.h"
#include "AppRegister.h"
#include "UI\AboutDlg.h"
#include <Tlhelp32.h>

#include "UI\XCTaskEditDlg.h"
#include ".\Language\XCRes_ENU\resource.h"

#include "XCCommandLine.h"
#include "..\Common\ptWinPath.h"

#include "XCConfiguration.h"
#include <io.h> 
#include "UI\XCStdLaunchDlg.h"


void XCRunModeNull(const SXCCopyTaskInfo& task,SConfigData& config) ;
void XCRunModeShell(const SXCCopyTaskInfo& task,SConfigData& config) ;
void XCRunModeConfiguration() ;
void XCRunModeRegister() ;
void XCRunModeAbout() ;
void XCRunModeXtremeTaskDlg(const SXCCopyTaskInfo& task,SConfigData& config) ;
void XCRunModeXtremeRun(const SXCCopyTaskInfo& task,SConfigData& config,bool bAutoRun) ;
void XCRunCheckAndUpdate(const SXCCopyTaskInfo& task,SConfigData& config) ;

bool LoadXCResource(SConfigData* pConfigData) ;

LONG WINAPI XCReportBug(struct _EXCEPTION_POINTERS* ExceptionInfo) ;

#ifdef _DEBUG
void WriteTaskInfo2File(const SXCCopyTaskInfo& task,CptString strTargetFile=_T("c:\\XCTask.txt"));
#endif