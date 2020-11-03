/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCCopyTask.h"
#include "..\Common/ptCharCode.h"
#include "..\Common\TinyXML\tinyxml.h"
#include "..\Common\ptGlobal.h"
#include <shellapi.h>
#include <mbstring.h>
#include "..\Common\ptWinPath.h"


#define XMLTASK_NODE_NAME_ROOT				"ExtremeCopyTasks"
#define XMLTASK_NODE_NAME_TASK				"XCTask"
#define XMLTASK_NODE_NAME_SOURCEFILES		"SourceFiles"
#define XMLTASK_NODE_NAME_FILE				"File"
#define XMLTASK_NODE_NAME_DESTFOLDER		"DestinationFolder"
#define XMLTASK_NODE_NAME_DIRECTORY			"Directory"
#define XMLTASK_NODE_NAME_EXECUTE			"Execute"
#define XMLTASK_NODE_NAME_SHOWMODE			"ShowMode"
#define XMLTASK_NODE_NAME_SAMEFILEREACT		"SameFileReact"
#define XMLTASK_NODE_NAME_SAMEFILECONDIFTION		"SameFileConditoin"

#define XMLTASK_NODE_NAME_SAMEFILECONDIFTION_IF		"SameFileConditoin_If"
#define XMLTASK_NODE_NAME_SAMEFILECONDIFTION_THEN		"SameFileConditoin_Then"
#define XMLTASK_NODE_NAME_SAMEFILECONDIFTION_OTHERWISE		"SameFileConditoin_Otherwise"

#define XMLTASK_NODE_NAME_ERRORREACT			"ErrorReact"
#define XMLTASK_NODE_NAME_ERRORRETRYTIMES	"ErrorRetryTimes"
#define XMLTASK_NODE_NAME_ERROROUTOFRETRYTIMES "ErrorOutOfRetryTimesReact"	
#define XMLTASK_NODE_NAME_STARTUPPOSITION	"StartupPosition"
#define XMLTASK_NODE_NAME_EVENT				"Events"
#define XMLTASK_NODE_NAME_FINISHEVENT		"FinishEvent"
#define XMLTASK_NODE_NAME_VERIFYDATA		"VerifyData"
#define XMLTASK_NODE_NAME_CREATEBY			"CreateBy"
#define XMLTASK_NODE_NAME_AUTOSHUTDOWN		"AutoShutdown"

#define XMLTASK_NODE_NAME_STARTUPPOSITION_X	"x"
#define XMLTASK_NODE_NAME_STARTUPPOSITION_Y	"y"

#define XMLTASK_ATTRIBUTE_NAME_VERSION			"version"
#define XMLTASK_FORMAT_VERSIONNUMBER		"1.0"

#define XMLTASK_VALUE_COPY					"copy"
#define XMLTASK_VALUE_MOVE					"move"
#define XMLTASK_VALUE_WINDOW				"window"
#define XMLTASK_VALUE_TRAY					"tray"
#define XMLTASK_VALUE_ASK					"ask"
#define XMLTASK_VALUE_REPLACE				"replace"
#define XMLTASK_VALUE_REPLACE_IF_NEWER		"replace_if_newer"
#define XMLTASK_VALUE_SKIP					"skip"
#define XMLTASK_VALUE_RENAME				"rename"
#define XMLTASK_VALUE_EXIT					"exit"
#define XMLTASK_VALUE_IGNORE				"ignore"
#define XMLTASK_VALUE_RETRY					"retry"

#define XMLTASK_VALUE_NEWER					"newer"
#define XMLTASK_VALUE_OLDER					"older"
#define XMLTASK_VALUE_BIGGER				"bigger"
#define XMLTASK_VALUE_SMALLER				"smaller"
#define XMLTASK_VALUE_SAMETIMEANDSIZE		"Same Time And Size"



CXCCopyTask::CXCCopyTask(void)
{
}

CXCCopyTask::~CXCCopyTask(void)
{
}
/**
bool CXCCopyTask::ParseTaskInfo(const WCHAR* pTaskString,SXCCopyTaskInfo& sti)
{
	bool bRet = false ;

	if(pTaskString!=NULL)
	{
		int nSize = ::wcslen(pTaskString)*sizeof(WCHAR) ;

		//WriteToFile(pTaskString,nSize,"target.txt") ;
		const WCHAR* pRead = pTaskString ;
		WCHAR StrBuf[MAX_PATH*8] = {0} ;
		int nSplitFlag = 0;

		bool bDidCmd = false ;
		bool bDidSrc = false ;
		bool bDidDst = false ;
		int nSrcCount = 0 ;
		int nDstCount = 0 ;

		const WCHAR* pNewLine = L"\r\n" ;

		while(pRead-pTaskString<nSize && nSplitFlag!=99)
		{
			//MessageBox(NULL,"pRead-pCmd<nSize","3",0) ;

			const WCHAR* pEnd =::wcsstr(pRead,pNewLine) ;
			//const WCHAR* pEnd =::MySubStrSearch(pRead,pNewLine) ;

			if(pEnd==NULL)
			{
				break ;
			}
			//break ;

			::memset(StrBuf,0,sizeof(StrBuf)) ;
			//int nS = pEnd-pRead ;
			::CopyMemory(StrBuf,pRead,sizeof(WCHAR)*(pEnd-pRead)) ;

			sti.cmd = SXCCopyTaskInfo::XCMD_XtremeRun ;

			//MessageBoxW(NULL,StrBuf,L"3",0) ;

			switch(nSplitFlag)
			{
			case 0:// 拆命令
				{
					//WCHAR StrBuf2[128] = {0} ;
					WCHAR* p1 =::wcsstr(StrBuf,L":") ;

					if(p1!=NULL)
					{
						if(!bDidCmd && ::memcmp(L"command",StrBuf,::wcslen(L"command"))==0)
						{
							if(::wcsstr(p1+1,L"xcmove"))
							{
								sti.CopyType = SXCCopyTaskInfo::RT_Move ;
							}
							else
							{
								sti.CopyType = SXCCopyTaskInfo::RT_Copy ;
							}

							bDidCmd = true ;
						}
						else if(!bDidSrc && ::memcmp(L"source size",StrBuf,::wcslen(L"source size"))==0)
						{
							nSplitFlag = 1; // 拆源文件
							nSrcCount = ::_wtoi(p1+2) ;
							bDidSrc = true ;
						}
						else if(!bDidDst && ::memcmp(L"destination size",StrBuf,::wcslen(L"destination size"))==0)
						{
							nSplitFlag = 2 ; // 拆目标文件
							nDstCount = ::_wtoi(p1+2) ;
							bDidDst = true ;
						}
					}
				}
				break ;

			case 1:// 拆源文件
				{
					sti.strSrcFileVer.push_back(StrBuf) ;
					if(--nSrcCount<=0)
					{
						nSplitFlag = 0 ; // 拆命令
					}
				}
				break ;

			case 2:// 拆目标文件
				sti.strDstFile = StrBuf ;
				nSplitFlag= 99 ;
				bRet = true ;// 拆解完成
				break ;
			}

			pEnd += ::wcslen(pNewLine) ;
			pRead = pEnd ;
		}
	}


	return bRet ;
}
/**/


bool CXCCopyTask::ExecuteTaskFileByExtremeCopy(CptString& strTaskFile,const TCHAR* pXCExeFileFolder)
{
	bool bRet = false ;

	if( pXCExeFileFolder!=NULL && IsFileExist(strTaskFile.c_str()))
	{
		CptString strXCExeFileFolder = pXCExeFileFolder ;

		const TCHAR cLastSlash = strXCExeFileFolder.GetAt(strXCExeFileFolder.GetLength()-1) ;
		if(cLastSlash!='\\' || cLastSlash!='/')
		{
			strXCExeFileFolder += '\\' ;
		}

		CptString strCmdLine  ;
		strCmdLine.Format(_T("-task_file \"%s\""),strTaskFile) ;

		CptString strXCExeFile = strXCExeFileFolder + _T("ExtremeCopy.exe") ;

		if(IsFileExist(strXCExeFile.c_str()))
		{
			if(CptGlobal::IsNeedElevatePrivilege())
			{// VISTA
				if((int)::ShellExecute(NULL,_T("runas"),_T("ExtremeCopy.exe"),strCmdLine.c_str(),strXCExeFileFolder.c_str(),SW_SHOW)>32)
				{ 
				}
			}
			else
			{// 直接启动 复制软件
				::ShellExecute(NULL,_T("open"),_T("ExtremeCopy.exe"),strCmdLine.c_str(),strXCExeFileFolder.c_str(),SW_SHOW) ;
			}
		}
	}

	return bRet ;

}


ETaskXMLErrorType CXCCopyTask::ExecuteTaskFile(CptString& strTaskFile,const TCHAR* pXCExeFileFolder) 
{
	pt_STL_vector(SXCCopyTaskInfo) TaskVer ;

	ETaskXMLErrorType result = ConvertXMLFile2TaskInfo(strTaskFile.c_str(),TaskVer) ;

	bool bError = false ;

	if(result==TXMLET_Success && TaskVer.size()>=1 )
	{
		if(TaskVer[0].strSrcFileVer.empty() || TaskVer[0].strDstFolderVer.empty() ||
			TaskVer[0].strDstFolderVer[0].GetLength()==0)
		{
			return TXMLET_ThereAreNotSrcOrDstFileOrFolder ;
		}
		else
		{
			bError = LaunchXCTaskInstanceViaNamePipe(TaskVer[0],pXCExeFileFolder,NULL,0) ;
		}
	}

	if(!bError)
	{
		result = TXMLET_UnknownError ;
	}
	
	return result ;
}

bool CXCCopyTask::LaunchXCTaskInstanceViaNamePipe(const SXCCopyTaskInfo& TaskInfo,const TCHAR* pXCExeFileFolder,HWND hWnd,DWORD dwWaitMilliseconds)
{
	bool bRet = false ;

	if( pXCExeFileFolder!=NULL)
	{
		CptString strXCExeFileFolder = pXCExeFileFolder ;

		const TCHAR cLastSlash = strXCExeFileFolder.GetAt(strXCExeFileFolder.GetLength()-1) ;
		if(cLastSlash!='\\' || cLastSlash!='/')
		{
			strXCExeFileFolder += '\\' ;
		}

		CptString strXCExeFile = strXCExeFileFolder + _T("ExtremeCopy.exe") ;

		if(IsFileExist(strXCExeFile.c_str()))
		{
			TCHAR* pUTF8XML = NULL ;
			int nContentSize = 0 ;

			pt_STL_vector(SXCCopyTaskInfo) TaskVer ;
			TaskVer.push_back(TaskInfo) ;
			CXCCopyTask::ConvertTaskInfo2XMLString(TaskVer,(char**)&pUTF8XML,nContentSize) ;

			if(pUTF8XML!=NULL)
			{
				HANDLE hPipe = INVALID_HANDLE_VALUE ;

				const TCHAR* pPipeName = _T("\\\\.\\pipe\\XCShell2AppPipeName") ;
				const int nMaxInstance = 16 ;

				if(CptGlobal::IsVISTAOrLaterVersion())
				{
					BYTE  sd[SECURITY_DESCRIPTOR_MIN_LENGTH];
					SECURITY_ATTRIBUTES  sa;

					sa.nLength = sizeof(sa);
					sa.bInheritHandle = TRUE;
					sa.lpSecurityDescriptor = &sd;

					::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
					::SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

					hPipe = ::CreateNamedPipe(pPipeName,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,
						PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,nMaxInstance,nContentSize,4*1024,3*1000,&sa) ;
				}
				else
				{
					hPipe = ::CreateNamedPipe(pPipeName,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,
						PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,nMaxInstance,nContentSize,4*1024,3*1000,NULL) ;
				}

				if(hPipe!=INVALID_HANDLE_VALUE)
				{
					bool bConn = false ;

					OVERLAPPED overlap ;

					::memset(&overlap,0,sizeof(overlap)) ;
					overlap.hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL) ;

					if(ConnectNamedPipe(hPipe, &overlap) || ERROR_IO_PENDING==GetLastError())
					{
						bool bNeedUACPopup = false ;

						if(CptGlobal::IsNeedElevatePrivilege())
						{// VISTA

							for(size_t i=0;i<TaskInfo.strDstFolderVer.size() && !bNeedUACPopup && !bNeedUACPopup;++i)
							{
								// 如果目标地是文件夹，则直接判断该文件夹，否则判断
								if(CptGlobal::IsFolder(TaskInfo.strDstFolderVer[i]))
								{
									bNeedUACPopup = !CptGlobal::CanFolderWrite(TaskInfo.strDstFolderVer[i]) ;
								}
								else
								{
									CptWinPath::SPathElementInfo pei ;
									pei.uFlag = CptWinPath::PET_Path ;

									if(CptWinPath::GetPathElement(TaskInfo.strDstFolderVer[i],pei))
									{
										bNeedUACPopup = !CptGlobal::CanFolderWrite(pei.strPath) ;
									}
								}
							}

							if(TaskInfo.CopyType==SXCCopyTaskInfo::RT_Move && !bNeedUACPopup)
							{// 如果这是移动的文件，则检测源文件是否可以删除

								pt_STL_vector(CptString) strVer ;

								CptWinPath::SPathElementInfo pei ;

								pei.uFlag = CptWinPath::PET_Path ;
								bool IsDid = false ;

								for(size_t i=0;i<TaskInfo.strSrcFileVer.size() && !bNeedUACPopup;++i)
								{
									if(CptWinPath::GetPathElement(TaskInfo.strSrcFileVer[i],pei))
									{
										IsDid = false ;
										for(size_t j=0;j<strVer.size() && !IsDid;++j)
										{// 查找之前是否已判断过该文件夹
											if(pei.strPath.CompareNoCase(strVer[j])==0)
											{
												IsDid = true ;
											}
										}

										if(!IsDid)
										{// 如果该文件夹未曾判断过，则对其进行判断
											bNeedUACPopup = !CptGlobal::CanFolderWrite(pei.strPath) ;

											if(!bNeedUACPopup)
											{
												strVer.push_back(pei.strPath) ;
											}
										}
									}
								}
							}

							//
							/**
							HANDLE hEvent = ::OpenEvent(EVENT_ALL_ACCESS,TRUE,_T("XCEventCopyForUAC")) ;

							if(hEvent==NULL)
							{
								CptString strUACExe = strXCExeFileFolder + _T("XCopyVISTA.exe") ;

								bool bRunXCopyVISTA = IsFileExist(strUACExe.c_str()) ;

								// 判断是否在固定的盘符里, 这是为 portable 版做的工作
								if(bRunXCopyVISTA)
								{
									if(strXCExeFileFolder[0]=='\\')
									{
										bRunXCopyVISTA = false ;
									}
									else
									{
										TCHAR root[16] = {0} ;
#pragma warning(push)
#pragma warning(disable:4996)
										::_stprintf(root,_T("%c:\\"),strXCExeFileFolder[0]) ;
#pragma warning(pop)

										UINT uType = ::GetDriveType(root) ;

										bRunXCopyVISTA = (uType==DRIVE_FIXED) ;

									}
								}

								if(bRunXCopyVISTA)
								{
									if((int)::ShellExecute(NULL,_T("runas"),_T("XCopyVISTA.exe"),_T("-forvista"),strXCExeFileFolder.c_str(),SW_SHOW)>32)
									{ 
									}
								}
								else
								{// 不经过 XCopyVISTA.exe 直接启动
									::ShellExecute(NULL,_T("runas"),_T("ExtremeCopy.exe"),_T("-shell"),strXCExeFileFolder.c_str(),SW_SHOW) ;
								}
							}
							else
							{
								::SetEvent(hEvent) ;
								::CloseHandle(hEvent) ;
							}
							/**/
						}
						
						if(bNeedUACPopup)
						{
							::ShellExecute(NULL,_T("runas"),_T("ExtremeCopy.exe"),_T("-shell"),strXCExeFileFolder.c_str(),SW_SHOW) ;
						}
						else
						{// 直接启动 复制软件
							::ShellExecute(NULL,_T("open"),_T("ExtremeCopy.exe"),_T("-shell"),strXCExeFileFolder.c_str(),SW_SHOW) ;
						}

						//if((int)::ShellExecute(NULL,bNeedAdmin ? _T("runas"):_T("open"),_T("ExtremeCopy.exe"),_T("-shell"),szEXEBuf,SW_SHOW)>32)
						{
							if(::WaitForSingleObject(overlap.hEvent,2*1000)==WAIT_OBJECT_0)
							{// 有连接成功连入

								DWORD dwWritten = 0 ;

								::WriteFile(hPipe,(void*)&nContentSize,4,&dwWritten,&overlap) ; // 先把长度发过去
								::WriteFile(hPipe,(void*)pUTF8XML,nContentSize,&dwWritten,&overlap) ; // 再把内容发过去

								bool bCheckNamePipeRun = true ;

								const DWORD dwBginTick = ::GetTickCount() ;

								while(bCheckNamePipeRun)
								{// 等待 ExtremeCopy 的复制工作完成
									DWORD dwWaitResult = ::WaitForSingleObject(overlap.hEvent,1) ;

									switch(dwWaitResult)
									{
									case WAIT_OBJECT_0:
										{
											DWORD dwNumOfBytesToRead = 0 ;
											BOOL bResult = ::GetOverlappedResult(hPipe,&overlap,&dwNumOfBytesToRead,FALSE) ;

											if(dwNumOfBytesToRead==0)
											{
												bCheckNamePipeRun = false ;
											}
											else
											{
												::ReadFile(hPipe,pUTF8XML,10,&dwWritten,&overlap) ;
											}
										}
										break ;

									case WAIT_TIMEOUT:
										{
											//if(bWait)
											if(dwWaitMilliseconds>0)
											{// 若等待
												MSG msg ;

												//if(::GetMessage(&msg,NULL,0,0))
												if(::PeekMessage(&msg,NULL,0,0,PM_REMOVE))
												{
													::TranslateMessage(&msg) ;
													::DispatchMessage(&msg) ;
												}

												bCheckNamePipeRun = (::GetTickCount()-dwBginTick<dwWaitMilliseconds) ;
												
											}
											else
											{
												bCheckNamePipeRun = false ;
											}

										}
										break ;

									default:
										bCheckNamePipeRun = false ;
										break ;
									}
								}
							}
						}
					}

					::CloseHandle(overlap.hEvent) ;
					overlap.hEvent = NULL ;

					bRet = true ;

					if(hPipe!=INVALID_HANDLE_VALUE)
					{
						::DisconnectNamedPipe(hPipe) ;
						::CloseHandle(hPipe) ;
						hPipe = INVALID_HANDLE_VALUE ;
					}

				}

				::free(pUTF8XML) ;
				pUTF8XML = NULL ;

			}
		}
	}

	return bRet ;
}

ETaskXMLErrorType CXCCopyTask::ConvertXMLFile2TaskInfo(const TCHAR* lpszXMLFile,pt_STL_vector(SXCCopyTaskInfo)& TaskVer) 
{
	_ASSERT(lpszXMLFile!=NULL) ;

	ETaskXMLErrorType bRet = TXMLET_UnknownError ;

	HANDLE hFile = ::CreateFile(lpszXMLFile,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL) ;

	if(hFile!=INVALID_HANDLE_VALUE)
	{
#ifndef _DEBUG
		try
#endif
		{
			DWORD dwHi = 0;
			DWORD dwLow = ::GetFileSize(hFile,&dwHi) ;

			if(dwLow<8*1024*1024 && dwLow>2)
			{// 文件必须小于8M
				const int nBufSize = dwLow + 10 ;

				DWORD dwRead = 0 ;
				char* pBuf = new char[nBufSize] ;
				::memset(pBuf,0,nBufSize) ;

				_ASSERT(pBuf!=NULL) ;

				::ReadFile(hFile,pBuf,nBufSize,&dwRead,NULL) ;

				CptString strXML ;
				char* pUTF8XML = NULL ;
				bool bNewXMLBuf = false ;

				if((BYTE)pBuf[0]==0xFF && (BYTE)pBuf[1]==0xFE)
				{// unicode

					WCHAR* pUnicodeStr = (WCHAR*)(pBuf+2) ;
					int	nLen = ::WideCharToMultiByte(CP_UTF8, 0, pUnicodeStr, -1, NULL, 0, NULL, NULL); 

					pUTF8XML = new char[nLen+2] ;
					bNewXMLBuf = true ;
					::memset(pUTF8XML,0,nLen+2) ;

					::WideCharToMultiByte (CP_UTF8, 0, pUnicodeStr, -1, pUTF8XML, nLen, NULL,NULL);
				}
				else if((BYTE)pBuf[0]==0xFE && (BYTE)pBuf[1]==0xFF)
				{// Unicode big endian
					char cTem = 0 ;
					for(unsigned int i=2;i<dwRead;i+=2)
					{
						cTem = pBuf[i] ;
						pBuf[i] = pBuf[i+1] ;
						pBuf[i+1] = cTem ;
					}

					WCHAR* pUnicodeStr = (WCHAR*)(pBuf+2) ;

					int	nLen = ::WideCharToMultiByte(CP_UTF8, 0, pUnicodeStr, -1, NULL, 0, NULL, NULL); 

					pUTF8XML = new char[nLen+2] ;
					bNewXMLBuf = true ;
					::memset(pUTF8XML,0,nLen+2) ;

					::WideCharToMultiByte (CP_UTF8, 0, pUnicodeStr, -1, pUTF8XML, nLen, NULL,NULL);
				}
				else if((BYTE)pBuf[0]==0xEF && (BYTE)pBuf[1]==0xBB && (BYTE)pBuf[2]==0xBF)
				{// utf-8
					pUTF8XML = pBuf+3 ;
				}
				else
				{
					if(CptGlobal::IsTextUTF8(pBuf,dwRead))
					{// 再判断是否 为不包含BOM 的 UTF8 编码 
						pUTF8XML = pBuf ;
					}
					else
					{// 系统内码
						// 先转换成unicode字符串
						int nLen = ::MultiByteToWideChar(CP_ACP, 0, pBuf, -1, NULL,0);

						WCHAR * wszUnicode = new WCHAR[nLen+1];

						_ASSERT(wszUnicode!=NULL) ;

						::memset(wszUnicode, 0, (nLen+1)*sizeof(WCHAR));
						::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pBuf, -1, wszUnicode, nLen);

						// 再由unicode 转换成 UTF8
						nLen = ::WideCharToMultiByte(CP_UTF8, 0, wszUnicode, -1, NULL, 0, NULL, NULL); 

						pUTF8XML = new char[nLen+2] ;
						bNewXMLBuf = true ;
						::memset(pUTF8XML,0,nLen+2) ;

						::WideCharToMultiByte (CP_UTF8, 0, wszUnicode, -1, pUTF8XML, nLen, NULL,NULL);

						delete [] wszUnicode ;
						wszUnicode = NULL ;
					}
				}

				if(pUTF8XML!=NULL)
				{
					bRet = ConvertXML2TaskInfo(pUTF8XML,TaskVer) ;
				}

				if(pUTF8XML!=NULL && bNewXMLBuf)
				{
					delete [] pUTF8XML ;
				}

				SAFE_DELETE_MEMORY_ARRAY(pBuf) ;
			}
		}
#ifndef _DEBUG
		catch(...)
		{
			//Release_Printf(_T("CXCCopyTask::ConvertXML2TaskInfo() 12")) ;
			bRet = TXMLET_UnknownError ;
		}
#endif

		::CloseHandle(hFile) ;
	}

	return bRet ;
}

void CXCCopyTask::ConverUtf8ToCurrentAppCode(const char* pUtf8,TCHAR* pOutStr,int nBufSize)
{
	::memset(pOutStr,0,nBufSize) ;
	CptCharCode::Utf82CurrentAppCode(pUtf8,pOutStr,nBufSize) ;
}

ETaskXMLErrorType CXCCopyTask::ConvertXML2TaskInfo(const char* pUTF8XML,pt_STL_vector(SXCCopyTaskInfo)& TaskVer) 
{
	_ASSERT(pUTF8XML!=NULL) ;

	ETaskXMLErrorType bRet = TXMLET_UnknownError ;

#ifndef _DEBUG
	try
#endif
	{
		TiXmlBase::SetCondenseWhiteSpace(false) ;

		TiXmlDocument xmlDoc ;

		if(xmlDoc.Parse(pUTF8XML,0)==0)
		{
			return TXMLET_InvalidTaskXMLData ;
		}

		TiXmlElement* pRootEle = xmlDoc.RootElement() ;

		if(pRootEle!=NULL && ::strcmp(pRootEle->Value(),XMLTASK_NODE_NAME_ROOT)==0 && !pRootEle->NoChildren())
		{// 有效的 合法头部

			const char* lpcszVerion = pRootEle->Attribute(XMLTASK_ATTRIBUTE_NAME_VERSION) ;

			if(lpcszVerion!=NULL)
			{// 检测版本

				SptVerionInfo CurVer ;
				SptVerionInfo XMLVer ;

				CurVer.FromString(_T(XMLTASK_FORMAT_VERSIONNUMBER)) ;

				TCHAR szUnicode[30] = {0} ;

				if(!CptCharCode::Utf82Unicode(lpcszVerion,szUnicode,sizeof(szUnicode)) ||
					!XMLVer.FromString(szUnicode) || CurVer.Compare(XMLVer)<0)
				{// 版本不正确
					return TXMLET_CantSupportFormatVersion ;
				}

				//if(::strcmp(lpcszVerion,XMLTASK_FORMAT_VERSIONNUMBER)!=0)
				//{// 版本不正确
				//	return TXMLET_CantSupportFormatVersion ;
				//}
			}

			//Release_Printf(_T("CXCCopyTask::ConvertXML2TaskInfo() 4")) ;
			SXCCopyTaskInfo task ;

			TiXmlElement* pTaskEle = NULL ;
			pTaskEle = pRootEle->FirstChildElement(XMLTASK_NODE_NAME_TASK) ;

			TCHAR szBuf[2*MAX_PATH] = {0} ;

			while(pTaskEle!=NULL)
			{
				task.SetDefaultValue() ; // 设置默认值

				TiXmlElement* pEle = pTaskEle->FirstChildElement() ;

				while(pEle!=NULL)
				{
					if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_SOURCEFILES)==0)
					{// source files
						TiXmlElement* pFileEle = pEle->FirstChildElement(XMLTASK_NODE_NAME_FILE) ;

						while(pFileEle!=NULL)
						{
							const char* pUtf8 = pFileEle->FirstChild()->Value() ;
														
							ConverUtf8ToCurrentAppCode(pUtf8,szBuf,sizeof(szBuf)) ;

							task.strSrcFileVer.push_back(szBuf) ;

							pFileEle = pFileEle->NextSiblingElement(XMLTASK_NODE_NAME_FILE) ;
						}

					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_DESTFOLDER)==0)
					{// destination folder

						TiXmlElement* pDirectoryEle = pEle->FirstChildElement(XMLTASK_NODE_NAME_DIRECTORY) ;

						while(pDirectoryEle!=NULL)
						{
							const char* pUtf8 = pDirectoryEle->FirstChild()->Value() ;

							ConverUtf8ToCurrentAppCode(pUtf8,szBuf,sizeof(szBuf)) ;

							task.strDstFolderVer.push_back(szBuf) ;

							pDirectoryEle = pDirectoryEle->NextSiblingElement() ;
						}

					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_EXECUTE)==0)
					{// execute
						const char* pUtf8 = pEle->FirstChild()->Value() ;

						if(::strcmp(pUtf8,XMLTASK_VALUE_COPY)==0)
						{
							task.CopyType = SXCCopyTaskInfo::RT_Copy ;
						}
						else if(::strcmp(pUtf8,XMLTASK_VALUE_MOVE)==0)
						{
							task.CopyType = SXCCopyTaskInfo::RT_Move ;
						}
						else
						{// 出错
							return TXMLET_ExecuteError ;
						}
					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_SHOWMODE)==0)
					{// show mode
						const char* pUtf8 = pEle->FirstChild()->Value() ;

						if(::strcmp(pUtf8,XMLTASK_VALUE_WINDOW)==0)
						{
							task.ShowMode = SXCCopyTaskInfo::ST_Window ;
						}
						else if(::strcmp(pUtf8,XMLTASK_VALUE_TRAY)==0)
						{
							task.ShowMode = SXCCopyTaskInfo::ST_Tray ;
						}
						else
						{// 出错
							return TXMLET_ShowModeError ;
						}
					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_SAMEFILEREACT)==0)
					{// 相同文件的处理方式
						const char* pUtf8 = pEle->FirstChild()->Value() ;

						if(::strcmp(pUtf8,XMLTASK_VALUE_ASK)==0)
						{
							task.ConfigShare.sfpt = SRichCopySelection::SFPT_Ask ;
						}
						else if(::strcmp(pUtf8,XMLTASK_VALUE_REPLACE)==0)
						{
							task.ConfigShare.sfpt = SRichCopySelection::SFPT_Replace ;
						}
						else if(::strcmp(pUtf8,XMLTASK_VALUE_SKIP)==0)
						{
							task.ConfigShare.sfpt = SRichCopySelection::SFPT_Skip ;
						}
						else if(::strcmp(pUtf8,XMLTASK_VALUE_RENAME)==0)
						{
							task.ConfigShare.sfpt = SRichCopySelection::SFPT_Rename ;
						}
						else if(::strcmp(pUtf8,XMLTASK_VALUE_REPLACE_IF_NEWER)==0)
						{
							task.ConfigShare.sfpt = SRichCopySelection::SFPT_IfCondition ;
						}
						else
						{// 出错
							_ASSERT(FALSE) ;
							return TXMLET_SameFileReactionError ;
						}
					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_SAMEFILECONDIFTION)==0)
					{// 相同文件的条件判断

						{// 'if' condition
							TiXmlElement* pEleIf = pEle->FirstChildElement(XMLTASK_NODE_NAME_SAMEFILECONDIFTION_IF) ;
							const char* pUtf8If = pEleIf->FirstChild()->Value() ;

							if(::strcmp(pUtf8If,XMLTASK_VALUE_NEWER)==0)
							{// newer
								task.ConfigShare.sfic.IfCondition = SRichCopySelection::FDT_Newer ;
							}
							else if(::strcmp(pUtf8If,XMLTASK_VALUE_OLDER)==0)
							{// older
								task.ConfigShare.sfic.IfCondition = SRichCopySelection::FDT_Older ;
							}
							else if(::strcmp(pUtf8If,XMLTASK_VALUE_BIGGER)==0)
							{// bigger
								task.ConfigShare.sfic.IfCondition = SRichCopySelection::FDT_Bigger ;
							}
							else if(::strcmp(pUtf8If,XMLTASK_VALUE_SMALLER)==0)
							{// smaller
								task.ConfigShare.sfic.IfCondition = SRichCopySelection::FDT_Smaller ;
							}
							else if(::strcmp(pUtf8If,XMLTASK_VALUE_SAMETIMEANDSIZE)==0)
							{// same time and size
								task.ConfigShare.sfic.IfCondition = SRichCopySelection::FDT_SameTimeAndSize ;
							}
							else
							{
								_ASSERT(FALSE) ;
								return TXMLET_SameFileConditionError ;
							}
						}

						{// 'then' condition
							TiXmlElement* pEleThen = pEle->FirstChildElement(XMLTASK_NODE_NAME_SAMEFILECONDIFTION_THEN) ;
							const char* pUtf8Then = pEleThen->FirstChild()->Value() ;

							if(::strcmp(pUtf8Then,XMLTASK_VALUE_ASK)==0)
							{
								task.ConfigShare.sfic.ThenOperation = SRichCopySelection::SFOT_Ask ;
							}
							else if(::strcmp(pUtf8Then,XMLTASK_VALUE_REPLACE)==0)
							{
								task.ConfigShare.sfic.ThenOperation = SRichCopySelection::SFOT_Replace ;
							}
							else if(::strcmp(pUtf8Then,XMLTASK_VALUE_SKIP)==0)
							{
								task.ConfigShare.sfic.ThenOperation = SRichCopySelection::SFOT_Skip ;
							}
							else if(::strcmp(pUtf8Then,XMLTASK_VALUE_RENAME)==0)
							{
								task.ConfigShare.sfic.ThenOperation = SRichCopySelection::SFOT_Rename ;
							}
							else 
							{
								_ASSERT(FALSE) ;
								return TXMLET_SameFileConditionError ;
							}
						}

						{// 'otherwise' condition
							TiXmlElement* pEleOtherwise = pEle->FirstChildElement(XMLTASK_NODE_NAME_SAMEFILECONDIFTION_OTHERWISE) ;
							const char* pUtf8Otherwise = pEleOtherwise->FirstChild()->Value() ;
							_ASSERT(pEleOtherwise!=NULL) ;

							if(::strcmp(pUtf8Otherwise,XMLTASK_VALUE_ASK)==0)
							{
								task.ConfigShare.sfic.OtherwiseOperation = SRichCopySelection::SFOT_Ask ;
							}
							else if(::strcmp(pUtf8Otherwise,XMLTASK_VALUE_REPLACE)==0)
							{
								task.ConfigShare.sfic.OtherwiseOperation = SRichCopySelection::SFOT_Replace ;
							}
							else if(::strcmp(pUtf8Otherwise,XMLTASK_VALUE_SKIP)==0)
							{
								task.ConfigShare.sfic.OtherwiseOperation = SRichCopySelection::SFOT_Skip ;
							}
							else if(::strcmp(pUtf8Otherwise,XMLTASK_VALUE_RENAME)==0)
							{
								task.ConfigShare.sfic.OtherwiseOperation = SRichCopySelection::SFOT_Rename ;
							}
							else 
							{
								_ASSERT(FALSE) ;
								return TXMLET_SameFileConditionError ;
							}

						}
					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_ERRORREACT)==0)
					{// error reaction
						const char* pUtf8 = pEle->FirstChild()->Value() ;

						if(::strcmp(pUtf8,XMLTASK_VALUE_ASK)==0)
						{
							task.ConfigShare.epc = SRichCopySelection::EPT_Ask ;
						}
						else if(::strcmp(pUtf8,XMLTASK_VALUE_EXIT)==0)
						{
							task.ConfigShare.epc = SRichCopySelection::EPT_Exit ;
						}
						else if(::strcmp(pUtf8,XMLTASK_VALUE_IGNORE)==0)
						{
							task.ConfigShare.epc = SRichCopySelection::EPT_Ignore ;
						}
						else if(::strcmp(pUtf8,XMLTASK_VALUE_RETRY)==0)
						{
							task.ConfigShare.epc = SRichCopySelection::EPT_Retry ;
						}
						else
						{// 出错
							return TXMLET_ErrorReactionError ;
						}
					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_ERRORRETRYTIMES)==0)
					{// error retry times
						const char* pUtf8 = pEle->FirstChild()->Value() ;

						task.ConfigShare.nRetryTimes = ::atoi(pUtf8) ;
					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_ERROROUTOFRETRYTIMES)==0)
					{// error out of retry times react
						const char* pUtf8 = pEle->FirstChild()->Value() ;

						if(::strcmp(pUtf8,XMLTASK_VALUE_EXIT)==0)
						{
							task.ConfigShare.RetryFailThen = SRichCopySelection::EPT_Exit ;
						}
						else if(::strcmp(pUtf8,XMLTASK_VALUE_IGNORE)==0)
						{
							task.ConfigShare.RetryFailThen = SRichCopySelection::EPT_Ignore ;
						}
						else if(::strcmp(pUtf8,XMLTASK_VALUE_ASK)==0)
						{
							task.ConfigShare.RetryFailThen = SRichCopySelection::EPT_Ask ;
						}
						else
						{// 出错
							//Release_Printf(_T("CXCCopyTask::ConvertXML2TaskInfo() 10")) ;
							return TXMLET_OutOfRetryError ;
						}
					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_STARTUPPOSITION)==0)
					{// startup position
						const TiXmlElement* pPosXEle = pEle->FirstChildElement(XMLTASK_NODE_NAME_STARTUPPOSITION_X) ;

						if(pPosXEle!=NULL)
						{
							const char* pUtf8 = pPosXEle->FirstChild()->Value() ;
							task.ConfigShare.ptStartupPos.nX = ::atoi(pUtf8) ;
						}

						const TiXmlElement* pPosYEle = pEle->FirstChildElement(XMLTASK_NODE_NAME_STARTUPPOSITION_Y) ;

						if(pPosXEle!=NULL)
						{
							const char* pUtf8 = pPosYEle->FirstChild()->Value() ;
							task.ConfigShare.ptStartupPos.nY = ::atoi(pUtf8) ;
						}
						
					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_VERIFYDATA)==0)
					{// verify data
						const char* pUtf8 = pEle->FirstChild()->Value() ;

						task.ConfigShare.bVerifyData = !(pUtf8==NULL || ::strcmp(pUtf8,"0")==0) ;
					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_EVENT)==0)
					{// event

						// finish event
						TiXmlElement* pFinishEle = pEle->FirstChildElement(XMLTASK_NODE_NAME_FINISHEVENT) ;

						if(pFinishEle!=NULL)
						{
							const char* pUtf8 = pFinishEle->FirstChild()->Value() ;

							ConverUtf8ToCurrentAppCode(pUtf8,szBuf,sizeof(szBuf)) ;
							task.strFinishEvent = szBuf ;
						}
					}
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_CREATEBY)==0)
					{// created by
						const char* pUtf8 = pEle->FirstChild()->Value() ;

						int nCreatedBy  = ::atoi(pUtf8) ;

						switch(nCreatedBy)
						{
						default: task.CreatedBy = SXCCopyTaskInfo::TCT_TaskDialog ; break ;

						case SXCCopyTaskInfo::TCT_TaskDialog:
						case SXCCopyTaskInfo::TCT_ShellExtension:
							task.CreatedBy = (SXCCopyTaskInfo::ETaskCreatedByType)nCreatedBy ;
							break ;
						}

					}// 
					/*
					else if(::strcmp(pEle->Value(),XMLTASK_NODE_NAME_AUTOSHUTDOWN)==0)
					{// 自动关机
						const char* pUtf8 = pEle->FirstChild()->Value() ;
						int nAutoShutdown  = ::atoi(pUtf8) ;
						task.ConfigShare.bShutdownAfterDone = (nAutoShutdown ? true : false) ;
					}
					/**/

					pEle = pEle->NextSiblingElement() ;
				}

				TaskVer.push_back(task) ;
				pTaskEle = pTaskEle->NextSiblingElement(XMLTASK_NODE_NAME_TASK) ;
			}

			//Release_Printf(_T("CXCCopyTask::ConvertXML2TaskInfo() 11")) ;

			bRet = TXMLET_Success ;
		}
	}
#ifndef _DEBUG
	catch(...)
	{
		//Release_Printf(_T("CXCCopyTask::ConvertXML2TaskInfo() 12")) ;
		bRet = TXMLET_InvalidTaskXMLData ;
	}
#endif
	//Release_Printf(_T("CXCCopyTask::ConvertXML2TaskInfo() 13")) ;

	return bRet ;
}


ETaskXMLErrorType CXCCopyTask::ConvertXML2TaskInfo(const CptString& strXML,pt_STL_vector(SXCCopyTaskInfo)& TaskVer) 
{
	char* pUTF8XML = NULL ;

#ifdef _UNICODE
	int	nLen = ::WideCharToMultiByte(CP_UTF8, 0, strXML.c_str(), -1, NULL, 0, NULL, NULL); 
	pUTF8XML = new char[nLen+1] ;
	::WideCharToMultiByte (CP_UTF8, 0, strXML.c_str(), -1, pUTF8XML, nLen, NULL,NULL);
#else
	// 先转换成unicode字符串
	int nLen = ::MultiByteToWideChar(CP_ACP, 0, strXML.c_str(), -1, NULL,0);

	WCHAR * wszUnicode = new WCHAR[nLen+1];

	_ASSERT(wszUnicode!=NULL) ;

	::memset(wszUnicode, 0, (nLen+1)*sizeof(WCHAR));
	::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strXML.c_str(), -1, wszUnicode, nLen);

	// 再由unicode 转换成 UTF8
	nLen = ::WideCharToMultiByte(CP_UTF8, 0, wszUnicode, -1, NULL, 0, NULL, NULL); 

	pUTF8XML = new char[nLen+2] ;
	bNewXMLBuf = true ;
	::memset(pUTF8XML,0,nLen+2) ;

	::WideCharToMultiByte (CP_UTF8, 0, wszUnicode, -1, pUTF8XML, nLen, NULL,NULL);

	delete wszUnicode ;
	wszUnicode = NULL ;
#endif

	ETaskXMLErrorType bRet = TXMLET_UnknownError ;

	if(pUTF8XML!=NULL)
	{
		bRet = ConvertXML2TaskInfo(pUTF8XML,TaskVer) ;
	}

	return bRet ;
}

bool CXCCopyTask::ConvertTaskInfo2XML(const pt_STL_vector(SXCCopyTaskInfo)& TaskVer,TiXmlDocument* pXMLDoc)
{
	if(pXMLDoc==NULL)
	{
		return false ;
	}

	char szUtf8[2*MAX_PATH] = {0} ;

//#ifdef _UNICODE
//	CptCharCode::Unicode2Utf8(strFileName.c_str(),szUtf8,sizeof(szUtf8)) ;
//#else
//	CptCharCode::CurrentSystemCode2Utf8(strFileName.c_str(),szUtf8,sizeof(szUtf8)) ;
//#endif

	TiXmlDocument& xmlDoc = *pXMLDoc ;

	TiXmlDeclaration Declaration( "1.0","utf-8","yes"); // 声明XML的属性

	xmlDoc.InsertEndChild( Declaration ); // 写入基本的XML头结构

	TiXmlElement* xmlRootEle = new TiXmlElement(XMLTASK_NODE_NAME_ROOT) ;

	xmlRootEle->SetAttribute(XMLTASK_ATTRIBUTE_NAME_VERSION,XMLTASK_FORMAT_VERSIONNUMBER) ;

	for(unsigned int i=0;i<TaskVer.size();++i)
	{
		const SXCCopyTaskInfo& task = TaskVer[i] ;
		TiXmlElement* xmlXCTaskEle = new TiXmlElement(XMLTASK_NODE_NAME_TASK) ;

		if(!task.strSrcFileVer.empty())
		{// source files
			TiXmlElement* xmlSourceFilesEle = new TiXmlElement(XMLTASK_NODE_NAME_SOURCEFILES) ;

			for(unsigned int j=0;j<task.strSrcFileVer.size();++j)
			{
				TiXmlElement* xmlFilesEle = new TiXmlElement(XMLTASK_NODE_NAME_FILE) ;

				::memset(szUtf8,0,sizeof(szUtf8)) ;

#ifdef _UNICODE
				CptCharCode::Unicode2Utf8(task.strSrcFileVer[j].c_str(),szUtf8,sizeof(szUtf8)) ;
#else
				CptCharCode::CurrentSystemCode2Utf8(task.strSrcFileVer[j].c_str(),szUtf8,sizeof(szUtf8)) ;
#endif

				TiXmlText* Text = new TiXmlText(szUtf8) ;
				xmlFilesEle->LinkEndChild(Text) ;

				xmlSourceFilesEle->LinkEndChild(xmlFilesEle) ;
			}

			xmlXCTaskEle->LinkEndChild(xmlSourceFilesEle) ;
		}

		if(!task.strDstFolderVer.empty() && task.strDstFolderVer[0].GetLength()>0)
		{// 目标文件夹
			TiXmlElement* xmlDestFolderEle = new TiXmlElement(XMLTASK_NODE_NAME_DESTFOLDER) ;

			for(size_t i=0;i<task.strDstFolderVer.size();++i)
			{
				TiXmlElement* xmlDirectoryEle = new TiXmlElement(XMLTASK_NODE_NAME_DIRECTORY) ;

				xmlDestFolderEle->LinkEndChild(xmlDirectoryEle) ;

				::memset(szUtf8,0,sizeof(szUtf8)) ;

#ifdef _UNICODE
				CptCharCode::Unicode2Utf8(task.strDstFolderVer[i].c_str(),szUtf8,sizeof(szUtf8)) ;
#else
				CptCharCode::CurrentSystemCode2Utf8(task.strDstFolderVer[i].c_str(),szUtf8,sizeof(szUtf8)) ;
#endif

				TiXmlText* Text = new TiXmlText(szUtf8) ;

				xmlDirectoryEle->LinkEndChild(Text) ;

				xmlXCTaskEle->LinkEndChild(xmlDestFolderEle) ;
			}
		}

		{// 执行类型
			TiXmlText* pText = NULL ;

			switch(task.CopyType)
			{
			case SXCCopyTaskInfo::RT_Copy:
				pText = new TiXmlText(XMLTASK_VALUE_COPY) ;
				break ;

			case SXCCopyTaskInfo::RT_Move:
				pText = new TiXmlText(XMLTASK_VALUE_MOVE) ;
				break ;
			}
			
			if(pText!=NULL)
			{
				TiXmlElement* pxmlExecuteEle = new TiXmlElement(XMLTASK_NODE_NAME_EXECUTE) ;
				pxmlExecuteEle->LinkEndChild(pText) ;
				xmlXCTaskEle->LinkEndChild(pxmlExecuteEle) ;
			}
		}

		{// 显示模式
			TiXmlText* pText = NULL ;

			switch(task.ShowMode)
			{
			case SXCCopyTaskInfo::ST_Tray:
				pText = new TiXmlText(XMLTASK_VALUE_TRAY) ;
				break ;

			case SXCCopyTaskInfo::ST_Window:
				pText = new TiXmlText(XMLTASK_VALUE_WINDOW) ;
				break ;
			}

			if(pText!=NULL)
			{
				TiXmlElement* pxmlEle = new TiXmlElement(XMLTASK_NODE_NAME_SHOWMODE) ;
				pxmlEle->LinkEndChild(pText) ;
				xmlXCTaskEle->LinkEndChild(pxmlEle) ;
			}
		}

		{// 相同文件时处理的方式
			TiXmlText* pText = NULL ;

			switch(task.ConfigShare.sfpt)
			{
			case SRichCopySelection::SFPT_Ask:
				pText = new TiXmlText(XMLTASK_VALUE_ASK) ;
				break ;

			case SRichCopySelection::SFPT_Rename:
				pText = new TiXmlText(XMLTASK_VALUE_RENAME) ;
				break ;

			case SRichCopySelection::SFPT_Skip:
				pText = new TiXmlText(XMLTASK_VALUE_SKIP) ;
				break ;

			case SRichCopySelection::SFPT_Replace:
				pText = new TiXmlText(XMLTASK_VALUE_REPLACE) ;
				break ;

			case SRichCopySelection::SFPT_IfCondition:
				pText = new TiXmlText(XMLTASK_VALUE_REPLACE_IF_NEWER) ;
				break ;

			default:
				_ASSERT(FALSE) ;
				break ;
			}

			if(pText!=NULL)
			{
				TiXmlElement* pxmlEle = new TiXmlElement(XMLTASK_NODE_NAME_SAMEFILEREACT) ;
				pxmlEle->LinkEndChild(pText) ;
				xmlXCTaskEle->LinkEndChild(pxmlEle) ;
			}
		}

		{// 相同文件条件处理方式
			TiXmlText* pTextIf = NULL ;

			switch(task.ConfigShare.sfic.IfCondition)
			{
			case SRichCopySelection::FDT_Newer:
				pTextIf = new TiXmlText(XMLTASK_VALUE_NEWER) ; 
				break ;

			case SRichCopySelection::FDT_Older:
				pTextIf = new TiXmlText(XMLTASK_VALUE_OLDER) ; 
				break ;

			case SRichCopySelection::FDT_Bigger:
				pTextIf = new TiXmlText(XMLTASK_VALUE_BIGGER) ; 
				break ;

			case SRichCopySelection::FDT_Smaller:
				pTextIf = new TiXmlText(XMLTASK_VALUE_SMALLER) ; 
				break ;

			case SRichCopySelection::FDT_SameTimeAndSize:
				pTextIf = new TiXmlText(XMLTASK_VALUE_SAMETIMEANDSIZE) ; 
				break ;

			default: 
				_ASSERT(FALSE); 
				pTextIf = new TiXmlText(XMLTASK_VALUE_NEWER) ; 
				break ;
			}

			TiXmlText* pTextThen = NULL ;
			switch(task.ConfigShare.sfic.ThenOperation)
			{
			case SRichCopySelection::SFOT_Ask:
				pTextThen = new TiXmlText(XMLTASK_VALUE_ASK) ; 
				break ;
			case SRichCopySelection::SFOT_Replace:
				pTextThen = new TiXmlText(XMLTASK_VALUE_REPLACE) ; 
				break ;
			case SRichCopySelection::SFOT_Skip:
				pTextThen = new TiXmlText(XMLTASK_VALUE_SKIP) ; 
				break ;
			case SRichCopySelection::SFOT_Rename:
				pTextThen = new TiXmlText(XMLTASK_VALUE_RENAME) ; 
				break ;

			default: 
				_ASSERT(FALSE); 
				pTextThen = new TiXmlText(XMLTASK_VALUE_ASK) ; 
				break ;
			}

			TiXmlText* pTextOtherwise = NULL ;
			switch(task.ConfigShare.sfic.OtherwiseOperation)
			{
			case SRichCopySelection::SFOT_Ask:
				pTextOtherwise = new TiXmlText(XMLTASK_VALUE_ASK) ; 
				break ;
			case SRichCopySelection::SFOT_Replace:
				pTextOtherwise = new TiXmlText(XMLTASK_VALUE_REPLACE) ; 
				break ;
			case SRichCopySelection::SFOT_Skip:
				pTextOtherwise = new TiXmlText(XMLTASK_VALUE_SKIP) ; 
				break ;
			case SRichCopySelection::SFOT_Rename:
				pTextOtherwise = new TiXmlText(XMLTASK_VALUE_RENAME) ; 
				break ;

			default: 
				_ASSERT(FALSE); 
				pTextOtherwise = new TiXmlText(XMLTASK_VALUE_SKIP) ; 
				break ;
			}

			TiXmlElement* pxmlEle = new TiXmlElement(XMLTASK_NODE_NAME_SAMEFILECONDIFTION) ;

			TiXmlElement* pxmlEleIf = new TiXmlElement(XMLTASK_NODE_NAME_SAMEFILECONDIFTION_IF) ;
			pxmlEleIf->LinkEndChild(pTextIf) ;
			pxmlEle->LinkEndChild(pxmlEleIf) ;

			TiXmlElement* pxmlEleThen = new TiXmlElement(XMLTASK_NODE_NAME_SAMEFILECONDIFTION_THEN) ;
			pxmlEleThen->LinkEndChild(pTextThen) ;
			pxmlEle->LinkEndChild(pxmlEleThen) ;

			TiXmlElement* pxmlEleOtherwise = new TiXmlElement(XMLTASK_NODE_NAME_SAMEFILECONDIFTION_OTHERWISE) ;
			pxmlEleOtherwise->LinkEndChild(pTextOtherwise) ;
			pxmlEle->LinkEndChild(pxmlEleOtherwise) ;

			xmlXCTaskEle->LinkEndChild(pxmlEle) ;
		}

		{// 错误时的处理的方式
			TiXmlText* pText = NULL ;

			switch(task.ConfigShare.epc)
			{
			case SRichCopySelection::EPT_Ask:
				pText = new TiXmlText(XMLTASK_VALUE_ASK) ;
				break ;

			case SRichCopySelection::EPT_Exit:
				pText = new TiXmlText(XMLTASK_VALUE_EXIT) ;
				break ;

			case SRichCopySelection::EPT_Ignore:
				pText = new TiXmlText(XMLTASK_VALUE_IGNORE) ;
				break ;

			case SRichCopySelection::EPT_Retry:
				pText = new TiXmlText(XMLTASK_VALUE_RETRY) ;
				break ;
			}

			if(pText!=NULL)
			{
				TiXmlElement* pxmlEle = new TiXmlElement(XMLTASK_NODE_NAME_ERRORREACT) ;
				pxmlEle->LinkEndChild(pText) ;
				xmlXCTaskEle->LinkEndChild(pxmlEle) ;
			}
		}

		{// 错误时重试次数
			char szBuf2[10] = {0} ;
			::_itoa(task.ConfigShare.nRetryTimes,szBuf2,10) ;
			TiXmlText* pText = new TiXmlText(szBuf2) ;

			TiXmlElement* pxmlEle = new TiXmlElement(XMLTASK_NODE_NAME_ERRORRETRYTIMES) ;
			pxmlEle->LinkEndChild(pText) ;
			xmlXCTaskEle->LinkEndChild(pxmlEle) ;
		}

		{// 超过错误重试次数后的错误处理方式
			TiXmlText* pText = NULL ;

			switch(task.ConfigShare.RetryFailThen)
			{
			case SRichCopySelection::EPT_Ask:
				pText = new TiXmlText(XMLTASK_VALUE_ASK) ;
				break ;

			case SRichCopySelection::EPT_Exit:
				pText = new TiXmlText(XMLTASK_VALUE_EXIT) ;
				break ;

			case SRichCopySelection::EPT_Ignore:
				pText = new TiXmlText(XMLTASK_VALUE_IGNORE) ;
				break ;
			}

			if(pText!=NULL)
			{
				TiXmlElement* pxmlEle = new TiXmlElement(XMLTASK_NODE_NAME_ERROROUTOFRETRYTIMES) ;
				pxmlEle->LinkEndChild(pText) ;
				xmlXCTaskEle->LinkEndChild(pxmlEle) ;
			}
		}

		if(task.ConfigShare.ptStartupPos.nX!=CONFIG_DEFAULT_MAINDIALOG_POS_X || task.ConfigShare.ptStartupPos.nY!=CONFIG_DEFAULT_MAINDIALOG_POS_Y)
		{// 启动时窗口的位置
			TiXmlElement* pxmlEle = new TiXmlElement(XMLTASK_NODE_NAME_STARTUPPOSITION) ;

			TiXmlElement* pxmlEleX = new TiXmlElement(XMLTASK_NODE_NAME_STARTUPPOSITION_X) ;
			TiXmlElement* pxmlEleY = new TiXmlElement(XMLTASK_NODE_NAME_STARTUPPOSITION_Y) ;

			char szBuf2[10] = {0} ;

			::_itoa(task.ConfigShare.ptStartupPos.nX,szBuf2,10) ;
			pxmlEleX->LinkEndChild(new TiXmlText(szBuf2)) ;

			::_itoa(task.ConfigShare.ptStartupPos.nY,szBuf2,10) ;
			pxmlEleY->LinkEndChild(new TiXmlText(szBuf2)) ;
			
			pxmlEle->LinkEndChild(pxmlEleX) ;
			pxmlEle->LinkEndChild(pxmlEleY) ;
			xmlXCTaskEle->LinkEndChild(pxmlEle) ;
		}

		{// 检查数据是否正确
			TiXmlElement* pxmlEle = new TiXmlElement(XMLTASK_NODE_NAME_VERIFYDATA) ;

			pxmlEle->LinkEndChild(new TiXmlText(task.ConfigShare.bVerifyData?"1":"0")) ;

			xmlXCTaskEle->LinkEndChild(pxmlEle) ;
		}		

		{// 事件
			TiXmlElement* pxmlEle = new TiXmlElement(XMLTASK_NODE_NAME_EVENT) ;

			if(task.strFinishEvent.GetLength()>0)
			{// 结束事件
				TiXmlElement* pxmlEleX = new TiXmlElement(XMLTASK_NODE_NAME_FINISHEVENT) ;

				CptStringA str(task.strFinishEvent) ;

				pxmlEleX->LinkEndChild(new TiXmlText(str.c_str())) ;

				pxmlEle->LinkEndChild(pxmlEleX) ;
			}

			xmlXCTaskEle->LinkEndChild(pxmlEle) ;
		}

		{// 由哪个创建的
			// 
			TiXmlElement* pxmlEle = new TiXmlElement(XMLTASK_NODE_NAME_CREATEBY) ;

			char szBuf2[10] = {0} ;

			::_itoa(task.CreatedBy,szBuf2,10) ;
			pxmlEle->LinkEndChild(new TiXmlText(szBuf2)) ;

			xmlXCTaskEle->LinkEndChild(pxmlEle) ;
		}

		/*
		{// 自动关机
			// 
			TiXmlElement* pxmlEle = new TiXmlElement(XMLTASK_NODE_NAME_AUTOSHUTDOWN) ;

			char szBuf2[10] = {0} ;

			::_itoa(task.ConfigShare.bShutdownAfterDone?1:0,szBuf2,10) ;
			pxmlEle->LinkEndChild(new TiXmlText(szBuf2)) ;

			xmlXCTaskEle->LinkEndChild(pxmlEle) ;
		}
		/**/

		xmlRootEle->LinkEndChild(xmlXCTaskEle) ;
	}

	xmlDoc.LinkEndChild(xmlRootEle) ;

	return true ;
}

bool CXCCopyTask::ConvertTaskInfo2XMLString(const pt_STL_vector(SXCCopyTaskInfo)& TaskVer,char** ppUtf8XML,int& nStrLen)
{
	if(ppUtf8XML==NULL)
	{
		return false;
	}

	bool bRet = false ;

	try
	{
		TiXmlDocument xmlDoc ;

		TiXmlPrinter printer;

		if(ConvertTaskInfo2XML(TaskVer,&xmlDoc) && xmlDoc.Accept(&printer))
		{
			const char* pXMLString = printer.CStr();
			nStrLen = (int)printer.Size() ;
			*ppUtf8XML = (char*)::malloc(nStrLen+3) ;

			::memcpy(*ppUtf8XML,pXMLString,nStrLen) ;

			bRet = true ;
		}
	}
	catch(...)
	{
	} 

	return bRet ;
}

bool CXCCopyTask::ConvertTaskInfo2XMLFile(const pt_STL_vector(SXCCopyTaskInfo)& TaskVer,const CptString& strFileName) 
{
	TiXmlDocument xmlDoc ;

	bool bRet = false  ;

	try
	{
		if(ConvertTaskInfo2XML(TaskVer,&xmlDoc))
		{
			CptStringA strkkk = strFileName.c_str() ;

			bRet = xmlDoc.SaveFile(strkkk.c_str()) ;
			bRet = true;
		}
	}
	catch(...)
	{
	}

	return bRet ;
}
