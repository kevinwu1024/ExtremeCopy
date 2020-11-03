/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "stdafx.h"
#include <Wininet.h>
#include "..\ExtremeCopy\App\Language\XCRes_ENU\resource.h"
#include "XCNewsChecker.h"
#include "..\ExtremeCopy\Common\TinyXML\tinyxml.h"
#include "..\ExtremeCopy\App\ptMultipleLanguage.h"
#include "..\ExtremeCopy\Common\ptWinPath.h"
#include "XCDownloadDlg.h"
#include "..\ExtremeCopy\App\UI\CptFileDialog.h"
#include "..\ExtremeCopy\App\XCGlobal.h"

#pragma comment(lib,"Wininet.lib")

#define XMLNEWS_NODE_NAME_ROOT						"ExtremeCopyNews" 
#define XMLNEWS_NODE_NAME_PRODUCT					"product" 
#define XMLNEWS_ATTRIBUTE_NAME_LATESTUPDATE			"latest-update"
#define XMLNEWS_NODE_NAME_FILE						"file" 
#define XMLNEWS_ATTRIBUTE_NAME_BITS					"bits" 
#define XMLNEWS_ATTRIBUTE_NAME_LASTESTVERSION		"latest-version" 
#define XMLNEWS_ATTRIBUTE_NAME_TESTVERSION			"test-ver" 
#define XMLNEWS_ATTRIBUTE_NAME_FILESIZE				"size" 
#define XMLNEWS_ATTRIBUTE_NAME_FILENAME				"name" 

#ifdef VERSION_PROFESSIONAL
// profession edition
#define XMLNEWS_NODE_NAME_PRODUCTNAME		"ExtremeCopyPro" 
#else

#ifdef VERSION_PORTABLE
// portable edition
#define XMLNEWS_NODE_NAME_PRODUCTNAME		"ExtremeCopyProtable" 
#else
// standard edition
#define XMLNEWS_NODE_NAME_PRODUCTNAME		"ExtremeCopyStd" 
#endif

#endif


CXCNewChecker::CXCNewChecker()
{
}

CXCNewChecker::~CXCNewChecker()
{
}

bool CXCNewChecker::IsChecked(const time_t& uLastUpdateTime) 
{
	time_t tNow = CptTime::Now() ;

	return (::abs(tNow-uLastUpdateTime)<2*24*60*60) ;
}

// 返回true说明这是有效的检查，返回false说明这是无效的检查
bool CXCNewChecker::CheckAndUpdateExtremeCopy(const time_t uLastUpdateTime) 
{
#ifdef _DEBUG
	const TCHAR*  szUrl=_T("http://www.easersoft.com/Download/ExtremeCopyNews_ForDebug.xml");
#else
	const TCHAR*  szUrl=_T("http://www.easersoft.com/go.php?type=check&update=ExtremeCopy");
#endif
	
	bool bRet = false ;

	// 如果早前已检查过更新，则不再检查
	if(CXCNewChecker::IsChecked(uLastUpdateTime))
	{
		return true;
	}

	HINTERNET hInetSession = ::InternetOpen(_T("ExtremeCopy Checker"),INTERNET_OPEN_TYPE_PRECONFIG  ,NULL,NULL,0); 

	if(hInetSession!=NULL)   
	{ 
		HINTERNET hHttpFile=::InternetOpenUrl(hInetSession,szUrl,NULL,0,INTERNET_FLAG_RELOAD,0); 

		if(hHttpFile!=NULL) 
		{
			TCHAR szFileSize[32] = {0} ;
			DWORD dwFileSize = 0 ;
			DWORD  dwSizeBuffer = sizeof(szFileSize) ;

			if(::HttpQueryInfo(hHttpFile,HTTP_QUERY_CONTENT_LENGTH,(LPVOID)szFileSize,&dwSizeBuffer,NULL)) 
			{ 
				dwSizeBuffer = _tstoi(szFileSize) ;

				if(dwSizeBuffer>0 && dwSizeBuffer<1024*1024)
				{
					char* pBuf = new char[dwSizeBuffer] ;

					if(pBuf!=NULL)
					{
						DWORD nBufOffset = 0 ;
						DWORD  dwBytesRead = 0; 
						DWORD dwRemainSize = dwSizeBuffer ;

						while((int)dwRemainSize>0)
						{
							dwBytesRead = 0 ;
							if(::InternetReadFile(hHttpFile,  pBuf+nBufOffset,  8*1024,  &dwBytesRead))
							{
								nBufOffset += dwBytesRead ;
								dwRemainSize -= dwBytesRead ;
							}
							else
							{
								break ;
							}
						}

						SNewsInfo ni ;

						try
						{
							if(dwRemainSize==0 && this->ParseNewsXML(pBuf,ni))
							{
								CptString strExtremeCopyPE ;
								strExtremeCopyPE.Format(_T("%s/ExtremeCopy.exe"),CptWinPath::GetStartupPath()) ;

								SptVerionInfo VerInfo ;

								if(CptGlobal::GetPEFileVersion(strExtremeCopyPE.c_str(),VerInfo))
								{
									int nLaestVer = ni.VerInfo.nMajor* 10000 + ni.VerInfo.nMin*100 + ni.VerInfo.nFix ;
									int nCurVer = VerInfo.nMajor* 10000 + VerInfo.nMin*100 + VerInfo.nFix ;

#ifndef VERSION_TEST // 如果当前版本为正式版本
									if(nCurVer>=nLaestVer)
									{// 如果没有最新版本，则直接返回
										bRet = true;
									}

									if(!bRet && ni.byBetaVer!=0)
									{// 当 latest 为测试版的话，则不做提示
										bRet = true;
									}
#else
									// 如果当前版本为测试版，则再检查测试版本的号
									if(!bRet && nCurVer>=nLaestVer && TEST_VERSION_NUMBER>=ni.byBetaVer)
									{// 如果没有最新版本，则直接返回
										bRet = true;
									}
#endif

									if(!bRet)
									{
										CptString strNewVerFormat = CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_NEWVERSIONFORMAT) ;

										CptString strVerType ;

										if(ni.byBetaVer==0)
										{// 这是正式版
											strVerType = CptMultipleLanguage::GetInstance()->GetString(IDS_STRING_FINALVERSION) ;
										}
										else
										{// 这是测试版
											strVerType.Format(_T("beta %d"),ni.byBetaVer) ;
										}

										CptString strVer ;

										strVer.Format(_T("%d.%d.%d"),ni.VerInfo.nMajor,ni.VerInfo.nMin,ni.VerInfo.nFix) ;

										CptString strExtremeCopy = CptMultipleLanguage::GetInstance()->GetString(IDC_EXTREMECOPY) ;

										CptString strMsgBoxText ;

										strMsgBoxText.Format(strNewVerFormat.c_str(),ni.strName,strVer,strVerType) ;

										// 显示下载对话框
										if(::MessageBox(NULL,strMsgBoxText.c_str(),strExtremeCopy.c_str(),MB_YESNO)==IDYES)
										{
											CptFileDialog dlg ;

											DWORD dwFlag = 0 ;
											if(dlg.Open(false,false,NULL,dwFlag,NULL))
											{
												CptStringList sl = dlg.GetSelectedFiles() ;

												if(sl.GetCount()>0)
												{
													CptString strDesktop = sl[0];

													strDesktop.TrimLeft(_T("\\")) ;

													CXCDownloadDlg DlDlg ;

													DlDlg.SetDownloadFile(ni.strDownloadFileURL,strDesktop) ;

													DlDlg.ShowDialog() ;
												}
											}
											// http://www.easersoft.com/go.php?type=download&file=ExtremeCopy&edition=pro&bits=32
											//CptGlobal::DownloadFile(ni.strDownloadFileURL,strDesktop) ;
										}
									}

								}
							}
						}
						catch(...)
						{

						}

						SAFE_DELETE_MEMORY(pBuf) ;
					}
				}
			} 

			::InternetCloseHandle(hHttpFile) ;
		} 

		::InternetCloseHandle(hInetSession); 
	} 

	return bRet ;
}



bool CXCNewChecker::ParseNewsXML(const char* pBuf,SNewsInfo& ni) 
{
	bool bRet = false ;

#ifndef _DEBUG
	try
#endif
	{
		TiXmlBase::SetCondenseWhiteSpace(false) ;

		TiXmlDocument xmlDoc ;

		xmlDoc.Parse(pBuf,0) ;

		TiXmlElement* pRootEle = xmlDoc.RootElement() ;

		if(pRootEle!=NULL && ::strcmp(pRootEle->Value(),XMLNEWS_NODE_NAME_ROOT)==0 && !pRootEle->NoChildren())
		{
			TiXmlElement* pProductEle = pRootEle->FirstChildElement(XMLNEWS_NODE_NAME_PRODUCT) ;

			if(pProductEle!=NULL && !pProductEle->NoChildren())
			{
				TiXmlElement* pProductNameEle = pProductEle->FirstChildElement(XMLNEWS_NODE_NAME_PRODUCTNAME) ;

				_ASSERT(pProductNameEle!=NULL) ;

				if(pProductNameEle!=NULL)
				{
					// 检测最近的版本
					const char* pLatestVersion = pProductNameEle->Attribute(XMLNEWS_ATTRIBUTE_NAME_LASTESTVERSION) ;
					_ASSERT(pLatestVersion!=NULL) ;

					if(pLatestVersion!=NULL)
					{
						CptStringListA sl ;

						if(sl.Split(pLatestVersion,'.')>0)
						try
						{
							ni.VerInfo.nMajor = sl[0].AsInt() ;
							ni.VerInfo.nMin = sl[1].AsInt() ;
							ni.VerInfo.nFix = sl[2].AsInt() ;
						}
						catch(...)
						{
							return false ;
						}
					}

					// 检测最近的测试版本号
					const char* pVerType = pProductNameEle->Attribute(XMLNEWS_ATTRIBUTE_NAME_TESTVERSION) ;
					_ASSERT(pVerType!=NULL) ;

					if(pVerType!=NULL)
					{
						ni.byBetaVer = ::atoi(pVerType) ;
					}

					// 读取名字
					const char* pFileName = pProductNameEle->Attribute(XMLNEWS_ATTRIBUTE_NAME_FILENAME) ;
					_ASSERT(pFileName!=NULL) ;

					if(pFileName!=NULL)
					{
						ni.strName = pFileName ;
					}

					TiXmlElement* pFileEle = pProductNameEle->FirstChildElement(XMLNEWS_NODE_NAME_FILE) ;

					_ASSERT(pFileEle!=NULL) ;

					if(pFileEle!=NULL)
					{
						// 文件大小
						const char* pFileSize = pFileEle->Attribute(XMLNEWS_ATTRIBUTE_NAME_FILESIZE) ;

						_ASSERT(pFileSize) ;

						if(pFileSize!=NULL)
						{
							ni.nFileSize = ::atoi(pFileSize) ;
						}


						// 所属 bit
						const char* pBits = pFileEle->Attribute(XMLNEWS_ATTRIBUTE_NAME_BITS) ;

						_ASSERT(pBits!=NULL) ;

						int nBits = 32 ;

						if(pBits!=NULL) 
						{
							int nBits = ::atoi(pBits) ;

							_ASSERT(nBits==32 || nBits==64) ;

							if(nBits!=64)
							{
								nBits = 32 ;
							}
						}

						bool nNeedFindNext = false ;

#ifdef _WIN64
						if(nBits==32)
						{
							nNeedFindNext = true ;
						}
						
#else
						if(nBits==64)
						{
							nNeedFindNext = true ;
						}
#endif
						if(nNeedFindNext)
						{
							pFileEle = pProductNameEle->NextSiblingElement(XMLNEWS_NODE_NAME_FILE) ;
						}
						
						_ASSERT(pFileEle!=NULL) ;

						if(pFileEle!=NULL)
						{
							ni.strDownloadFileURL = pFileEle->FirstChild()->Value() ;

							bRet = true ;
						}
					}

				}
			}

		}
	}
#ifndef _DEBUG
	catch(...)
	{
		//bRet = TXMLET_UnknownError ;
		bRet = false ;
	}
#endif

	return bRet ;
}