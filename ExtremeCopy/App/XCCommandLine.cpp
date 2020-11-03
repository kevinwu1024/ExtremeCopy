/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCCommandLine.h"


#include ".\Language\XCRes_ENU\resource.h"

#include "../Common/ptGlobal.h"

// command line 1.0
#define XC_SWITCH_FROMSHELL				_T("-shell")
#define XC_SWITCH_REGISTERSN			_T("-register")
#define XC_SWITCH_CONFIG				_T("-config")
#define XC_SWITCH_ABOUT					_T("-about")
#define XC_SWITCH_TASKDIALOG			_T("-task_dlg")

#define XC_SWITCH_SOURCE				_T("-src")
#define XC_SWITCH_DESTINATION			_T("-dst")
#define XC_SWITCH_ACTION				_T("-exe")
#define XC_SWITCH_SAMEFILE				_T("-same_file")
//#define XC_SWITCH_SKIP					_T("-skip")
#define XC_SWITCH_ERROR					_T("-error")
#define XC_SWITCH_RETRY					_T("-retry")
#define XC_SWITCH_LOG					_T("-log")
#define XC_SWITCH_SHOW					_T("-show")
#define XC_SWITCH_TASKFILE				_T("-task_file")
#define XC_SWITCH_OUTOFRETRY			_T("-out_of_retry")

// command line 1.1
#define XC_SWITCH_CONDITION				_T("-condition")

// for 2.1
#define XC_SWITCH_CHECKUPDATE			_T("-check_update")



CXCCommandLine::CXCCommandLine(void)
{
}

CXCCommandLine::~CXCCommandLine(void)
{
}


bool CXCCommandLine::ParseCmdLine2TaskInfo(const TCHAR* pCmdLine,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	_ASSERT(pCmdLine!=NULL) ;

	if(pCmdLine[0]==0)
	{
		cmd.cmd = SXCCopyTaskInfo::XCMD_Null ;
		return true ;
	}

	bool bRet = true ;

	strError = _T("") ;

	cmd.SetDefaultValue() ; // 先设置为默认值

	CptCmdLineParser cl ;

	int nSwitchCount = cl.Parse(pCmdLine) ;

	pt_STL_vector(CptString) ParamVer ;

	if(cl.IsContainSwitch(XC_SWITCH_TASKFILE))
	{// 如果包含任务文件,则先解析它
		cl.GetParams(XC_SWITCH_TASKFILE,ParamVer) ;

		if(!SetCmdParam(XC_SWITCH_TASKFILE,ParamVer,cmd,strError))
		{
			bRet = false ;
		}
		else
		{
			cl.RemoveSwitch(XC_SWITCH_TASKFILE) ; // 完成后移除该开关
			--nSwitchCount ;
		}
	}

	if(bRet)
	{
		for(int i=0;i<nSwitchCount && bRet;++i)
		{
			ParamVer.clear() ;
			CptString strSwitch = cl.GetSwitchNameByIndex(i) ;

			cl.GetParams(strSwitch.c_str(),ParamVer) ;

			if(!SetCmdParam(strSwitch.c_str(),ParamVer,cmd,strError))
			{
				bRet = false ;
				break ;
			}
		}

		if(bRet && cmd.cmd==SXCCopyTaskInfo::XCMD_Unknown)
		{
			if(cmd.strDstFolderVer[0].GetLength()>0 && !cmd.strSrcFileVer.empty())
			{
				cmd.cmd = SXCCopyTaskInfo::XCMD_XtremeRun ;
			}
			else
			{
				strError = ::CptMultipleLanguage::GetInstance()->GetString(IDS_CMDLN_ERROR_SPECIFYPRIMARYSWITCH) ;
				bRet = false ;
			}
		}
	}

	return bRet ;

}

CptString CXCCommandLine::GetDoesntSupportError(const TCHAR* pSwitch)
{
	CptString strError ;

	CptString strFormat = ::CptMultipleLanguage::GetInstance()->GetString(IDS_CMDLN_ERROR_FORMAT_SWITCHDOESNTSUPPORTINCURRENTEDITION) ;

	strError.Format(strFormat,XC_SWITCH_SAMEFILE) ;

	return strError ;
}

bool CXCCommandLine::SetCmdParam(CptString strSwitchName,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError)
{
	if(strSwitchName.GetLength()>0)
	{
		if(strSwitchName.GetAt(0)!='-')
		{
			strSwitchName = "-" + strSwitchName ;
		}

		if(strSwitchName.Compare(XC_SWITCH_FROMSHELL)==0)
		{// -shell
			return SetCmdParam_Shell(XC_SWITCH_FROMSHELL,strParamVer,cmd,strError) ;
		}
		else if(strSwitchName.Compare(XC_SWITCH_REGISTERSN)==0)
		{// -register
			return SetCmdParam_Register(XC_SWITCH_REGISTERSN,strParamVer,cmd,strError) ;
		}
		else if(strSwitchName.Compare(XC_SWITCH_CONFIG)==0)
		{// -config
			return SetCmdParam_Config(XC_SWITCH_CONFIG,strParamVer,cmd,strError) ;
		}
		else if(strSwitchName.Compare(XC_SWITCH_ABOUT)==0)
		{// -about
			return SetCmdParam_About(XC_SWITCH_ABOUT,strParamVer,cmd,strError) ;
		}
		else if(strSwitchName.Compare(XC_SWITCH_SOURCE)==0)
		{// -src
			return SetCmdParam_SourceFile(XC_SWITCH_SOURCE,strParamVer,cmd,strError) ;
		}
		else if(strSwitchName.Compare(XC_SWITCH_DESTINATION)==0)
		{// -dst
			return SetCmdParam_DestinationFolder(XC_SWITCH_DESTINATION,strParamVer,cmd,strError) ;
		}
		else if(strSwitchName.Compare(XC_SWITCH_ACTION)==0)
		{// -act
			return SetCmdParam_Action(XC_SWITCH_ACTION,strParamVer,cmd,strError) ;
		}// 
		else if(strSwitchName.Compare(XC_SWITCH_CHECKUPDATE)==0)
		{// -check_update
			return SetCmdParam_CheckUpdate(XC_SWITCH_CHECKUPDATE,strParamVer,cmd,strError) ;
		}
		else if(strSwitchName.Compare(XC_SWITCH_SAMEFILE)==0)
		{// -samefile  same file

#ifdef VERSION_PROFESSIONAL // 只对专业版开放
			return SetCmdParam_SameFile(XC_SWITCH_SAMEFILE,strParamVer,cmd,strError) ;
#else
			strError = GetDoesntSupportError(XC_SWITCH_SAMEFILE) ;
			return false ;
#endif
		}
		else if(strSwitchName.Compare(XC_SWITCH_CONDITION)==0)
		{// -condition

#ifdef VERSION_PROFESSIONAL // 只对专业版开放
			return SetCmdParam_Condition(XC_SWITCH_CONDITION,strParamVer,cmd,strError) ;
#else
			strError = GetDoesntSupportError(XC_SWITCH_CONDITION) ;
			return false ;
#endif
		}
		else if(strSwitchName.Compare(XC_SWITCH_ERROR)==0)
		{// -error
#ifdef VERSION_PROFESSIONAL // 只对专业版开放
			return SetCmdParam_Error(XC_SWITCH_ERROR,strParamVer,cmd,strError) ;
#else
			strError = GetDoesntSupportError(XC_SWITCH_ERROR) ;
			return false ;
#endif
		}
		else if(strSwitchName.Compare(XC_SWITCH_RETRY)==0)
		{// -retry
#ifdef VERSION_PROFESSIONAL // 只对专业版开放
			return SetCmdParam_Retry(XC_SWITCH_RETRY,strParamVer,cmd,strError) ;
#else
			strError = GetDoesntSupportError(XC_SWITCH_RETRY) ;
			return false ;
#endif
		}
		else if(strSwitchName.Compare(XC_SWITCH_LOG)==0)
		{// -log
#ifdef VERSION_PROFESSIONAL // 只对专业版开放
			return SetCmdParam_Log(XC_SWITCH_LOG,strParamVer,cmd,strError) ;
#else
			strError = GetDoesntSupportError(XC_SWITCH_LOG) ;
			return false ;
#endif
		}
		else if(strSwitchName.Compare(XC_SWITCH_SHOW)==0)
		{// -show
#ifdef VERSION_PROFESSIONAL // 只对专业版开放
			return SetCmdParam_Show(XC_SWITCH_SHOW,strParamVer,cmd,strError) ;
#else
			strError = GetDoesntSupportError(XC_SWITCH_SHOW) ;
			return false ;
#endif
		}
		else if(strSwitchName.Compare(XC_SWITCH_TASKFILE)==0)
		{// -taskfile
#ifdef VERSION_PROFESSIONAL // 只对专业版开放
			return SetCmdParam_TaskFile(XC_SWITCH_TASKFILE,strParamVer,cmd,strError) ;
#else
			strError = GetDoesntSupportError(XC_SWITCH_TASKFILE) ;
			return false ;
#endif
		}
		else if(strSwitchName.Compare(XC_SWITCH_TASKDIALOG)==0)
		{// -task_dlg
#ifdef VERSION_PROFESSIONAL // 只对专业版开放
			return SetCmdParam_TaskDialog(XC_SWITCH_TASKDIALOG,strParamVer,cmd,strError) ;
#else
			strError = GetDoesntSupportError(XC_SWITCH_TASKDIALOG) ;
			return false ;
#endif
		}
		//
		else
		{
			CptString strFormat = ::CptMultipleLanguage::GetInstance()->GetString(IDS_CMDLN_ERROR_FORMAT_NOIDENTIFICATIONSWITCH) ;

			strError.Format(strFormat,strSwitchName) ;
		}
	}

	return false ;
}

// -update
bool CXCCommandLine::SetCmdParam_CheckUpdate(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	cmd.cmd = SXCCopyTaskInfo::XCMD_CheckUpdate ;

	bool bRet = true ;
	if(!strParamVer.empty())
	{
		bRet = FormatCmdParamError(pSwitch,strParamVer[0],strError) ;
	}

	return bRet ;
}

// -shell
bool CXCCommandLine::SetCmdParam_Shell(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	cmd.cmd = SXCCopyTaskInfo::XCMD_Shell ;

	bool bRet = true ;
	if(!strParamVer.empty())
	{
		bRet = FormatCmdParamError(pSwitch,strParamVer[0],strError) ;
	}
	return bRet ;

}

// -register
bool CXCCommandLine::SetCmdParam_Register(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	cmd.cmd = SXCCopyTaskInfo::XCMD_Register ;

	bool bRet = true ;
	if(!strParamVer.empty())
	{
		bRet = FormatCmdParamError(pSwitch,strParamVer[0],strError) ;
	}
	return bRet ;
}

// -config
bool CXCCommandLine::SetCmdParam_Config(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	cmd.cmd = SXCCopyTaskInfo::XCMD_Config ;

	bool bRet = true ;
	if(!strParamVer.empty())
	{
		bRet = FormatCmdParamError(pSwitch,strParamVer[0],strError) ;
	}
	return bRet ;
}

// -about
bool CXCCommandLine::SetCmdParam_About(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	cmd.cmd = SXCCopyTaskInfo::XCMD_About ;

	bool bRet = true ;
	if(!strParamVer.empty())
	{
		bRet = FormatCmdParamError(pSwitch,strParamVer[0],strError) ;
	}
	return bRet ;
}

// -src
bool CXCCommandLine::SetCmdParam_SourceFile(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	cmd.strSrcFileVer.clear() ;

	for(unsigned int i=0;i<strParamVer.size();++i)
	{
		if(strParamVer[i].Find('|')>0)
		{// 这是带有条件过滤的文件
		}
		else
		{
			cmd.strSrcFileVer.push_back(strParamVer[i]) ;
		}
	}
	//cmd.strSrcFileVer = strParamVer ;

	return true ;
}

// -dst
bool CXCCommandLine::SetCmdParam_DestinationFolder(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	bool bRet = false ;
	const int nSize = (int)strParamVer.size() ;

	if(nSize==1)
	{
		cmd.strDstFolderVer.push_back(strParamVer[0]) ;

		bRet = true ;
	}
	else if(nSize>1)
	{
		bRet = FormatCmdParamError(pSwitch,strParamVer[1],strError) ;
	}
	else
	{
		strError = CptMultipleLanguage::GetInstance()->GetString(IDS_CMDLN_ERROR_SPECIFYDESTINATIONFOLDER) ;
	}

	return bRet ;
}

// -condition
bool CXCCommandLine::SetCmdParam_Condition(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	bool bRet = false ;

	if(strParamVer.size()==2)
	{
		if(strParamVer[0].Compare(_T("skip")))
		{
			try
			{
				//cmd.Condition.Skip.uCondition = (UINT)strParamVer[1].AsInt() ;

				bRet = true ;
			}
			catch(...)
			{
			}
		}
	}

	return bRet ;
}

// -error
bool CXCCommandLine::SetCmdParam_Error(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	bool bRet = true ;

	if(strParamVer.size()==1)
	{
		if(strParamVer[0].Compare(_T("retry"))==0)
		{
			cmd.ConfigShare.epc = SRichCopySelection::EPT_Retry ;
		}
		else if(strParamVer[0].Compare(_T("exit"))==0)
		{
			cmd.ConfigShare.epc = SRichCopySelection::EPT_Exit ;
		}
		else if(strParamVer[0].Compare(_T("ignore"))==0)
		{
			cmd.ConfigShare.epc = SRichCopySelection::EPT_Ignore ;
		}
		else if(strParamVer[0].Compare(_T("ask"))==0)
		{
			cmd.ConfigShare.epc = SRichCopySelection::EPT_Ask ;
		}
		else
		{// 出错
			bRet = FormatCmdParamError(pSwitch,strParamVer[0],strError) ;
		}
	}
	else
	{
		strError = GetParameterErrorString(pSwitch) ;
		bRet = false ;
	}

	return bRet ;
}

// -retry
bool CXCCommandLine::SetCmdParam_Retry(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	bool bRet = false ;

	if(strParamVer.size()==1)
	{
		try
		{
			cmd.ConfigShare.nRetryTimes = strParamVer[0].AsInt() ;
			bRet = true ;
		}
		catch(...)
		{// 出错
			cmd.ConfigShare.nRetryTimes = 1 ;
		}
	}

	if(!bRet)
	{
		strError = GetParameterErrorString(pSwitch) ;
	}

	return bRet ;
}

// -log
bool CXCCommandLine::SetCmdParam_Log(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	bool bRet = false ;

	if(strParamVer.size()==1)
	{
		cmd.strLogFile = strParamVer[0] ;
		bRet = true ;
	}

	if(!bRet)
	{
		strError = GetParameterErrorString(pSwitch) ;
	}

	return bRet ;
}

// -exe
bool CXCCommandLine::SetCmdParam_Action(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	bool bRet = false ;

	if(strParamVer.size()==1)
	{
		if(strParamVer[0].Compare(_T("copy"))==0)
		{
			cmd.CopyType = SXCCopyTaskInfo::RT_Copy;
			bRet = true ;
		}
		else if(strParamVer[0].Compare(_T("move"))==0)
		{
			cmd.CopyType = SXCCopyTaskInfo::RT_Move ;
			bRet = true ;
		}
		else 
		{// 出错
			return FormatCmdParamError(pSwitch,strParamVer[0],strError) ;
		}
	}

	if(!bRet)
	{
		strError = GetParameterErrorString(pSwitch) ;
	}

	return bRet ;
}

// -same_file
bool CXCCommandLine::SetCmdParam_SameFile(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	bool bRet = false ;

	if(!strParamVer.empty())
	{
		if(strParamVer[0].Compare(_T("skip"))==0)
		{
			cmd.ConfigShare.sfpt = SRichCopySelection::SFPT_Skip ;
			bRet = true ;
		}
		else if(strParamVer[0].Compare(_T("replace"))==0)
		{
			cmd.ConfigShare.sfpt = SRichCopySelection::SFPT_Replace ;
			bRet = true ;
		}
		else if(strParamVer[0].Compare(_T("ask"))==0)
		{
			cmd.ConfigShare.sfpt = SRichCopySelection::SFPT_Ask ;
			bRet = true ;
		}
		else if(strParamVer[0].Compare(_T("rename"))==0)
		{
			cmd.ConfigShare.sfpt = SRichCopySelection::SFPT_Rename ;
			bRet = true ;
		}
		else
		{// 出错
			return FormatCmdParamError(pSwitch,strParamVer[0],strError) ;
		}
	}

	if(!bRet)
	{
		strError = GetParameterErrorString(pSwitch) ;
	}

	return bRet ;
}

// -show
bool CXCCommandLine::SetCmdParam_Show(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError)
{
	bool bRet = false ;

	if(strParamVer.size()==1)
	{
		if(strParamVer[0].Compare(_T("window"))==0)
		{
			cmd.ShowMode = SXCCopyTaskInfo::ST_Window ;
			bRet = true ;
		}
		else if(strParamVer[0].Compare(_T("tray"))==0)
		{
			cmd.ShowMode = SXCCopyTaskInfo::ST_Tray ;
			bRet = true ;
		}
		else
		{// 出错
			return FormatCmdParamError(pSwitch,strParamVer[0],strError) ;
		}
	}

	if(!bRet)
	{
		strError = GetParameterErrorString(pSwitch) ;
	}

	return bRet ;
}

// -task_dlg
bool CXCCommandLine::SetCmdParam_TaskDialog(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError)
{
	bool bRet = CXCCommandLine::SetCmdParam_TaskFile(pSwitch,strParamVer,cmd,strError) ;

	if(bRet)
	{
		cmd.cmd = SXCCopyTaskInfo::XCMD_TaskDlg ;
	}

	return bRet ;
}

// -task_file
bool CXCCommandLine::SetCmdParam_TaskFile(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError)
{
	bool bRet = true ;

	if(strParamVer.size()==1)
	{
		pt_STL_vector(SXCCopyTaskInfo) TaskVer ;
		ETaskXMLErrorType result = CXCCopyTask::ConvertXMLFile2TaskInfo(strParamVer[0].c_str(),TaskVer) ;

		if(result==TXMLET_Success)
		{
			cmd = TaskVer[0] ;
			bRet = true ;
		}
		else
		{
			strError = ::GetTaskXMLErrorString(result) ;

			if(strError.GetLength()==0)
			{
				return FormatCmdParamError(pSwitch,strParamVer[0],strError) ;
			}
			else
			{
				return false ;
			}


		}
	}
	else
	{
		if(strParamVer.empty())
		{//
			strError = GetParameterErrorString(pSwitch) ;
		}
		else
		{
			return FormatCmdParamError(pSwitch,strParamVer[1],strError) ;
		}
	}

	if(!bRet)
	{
		strError = GetParameterErrorString(pSwitch) ;
	}

	return bRet ;
}

bool CXCCommandLine::SetCmdParam_OutOfRetry(const TCHAR* pSwitch,const pt_STL_vector(CptString)& strParamVer,SXCCopyTaskInfo& cmd,CptString& strError) 
{
	bool bRet = true ;

	if(strParamVer.size()==1)
	{
		if(strParamVer[0].Compare(_T("exit"))==0)
		{
			cmd.ConfigShare.RetryFailThen = SRichCopySelection::EPT_Exit ;
		}
		else if(strParamVer[0].Compare(_T("ignore"))==0)
		{
			cmd.ConfigShare.RetryFailThen = SRichCopySelection::EPT_Ignore ;
		}
		else if(strParamVer[0].Compare(_T("ask"))==0)
		{
			cmd.ConfigShare.RetryFailThen = SRichCopySelection::EPT_Ask ;
		}
		else
		{// 出错
			bRet = FormatCmdParamError(pSwitch,strParamVer[0],strError) ;
		}
	}
	else
	{
		strError = GetParameterErrorString(pSwitch) ;
		bRet = false ;
	}

	return bRet ;
}

bool CXCCommandLine::FormatCmdParamError(const TCHAR* pSwitch,const CptString& strParam,CptString& strError)
{
	CptString strFormat = CptMultipleLanguage::GetInstance()->GetString(IDS_CMDLN_ERROR_FORMAT_NOIDENTIFICAIONPARAMETER) ;

	strError.Format(strFormat.c_str(),strParam,pSwitch) ;

	return true ;
}

CptString CXCCommandLine::GetParameterErrorString(const TCHAR* pSwitch)
{
	CptString strError ;

	CptString strFormat = ::CptMultipleLanguage::GetInstance()->GetString(IDS_CMDLN_ERROR_FORMAT_SPECIFYPARAMETER) ;

	strError.Format(strFormat,pSwitch) ;

	return strError ;
}