/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/
#include "stdafx.h"
#include "XCDownloadDlg.h"
#include <process.h>
#include <Wininet.h>
#include "..\ExtremeCopy\App\UI\CptProgressBar.h"
#include "..\ExtremeCopy\App\ptMultipleLanguage.h"

CXCDownloadDlg::CXCDownloadDlg(int nDlgID,HWND hParentWnd):CptDialog(nDlgID,hParentWnd,CptMultipleLanguage::GetInstance()->GetResourceHandle())
{
	m_bCancel = false ;
	m_hThread = NULL ;
	m_bPause = false ;
}

CXCDownloadDlg::~CXCDownloadDlg()
{
	if(m_hThread!=NULL)
	{
		::WaitForSingleObject(m_hThread,3*1000) ;
		::CloseHandle(m_hThread) ;
	}
}

BOOL CXCDownloadDlg::OnInitDialog() 
{
	::SetWindowPos(this->GetSafeHwnd(),HWND_TOPMOST,20,20,0,0,SWP_NOSIZE|SWP_NOZORDER) ;

	m_bCancel = false ;
	m_hThread = (HANDLE)::_beginthreadex(NULL,0,DownloadThreadFunc,this,0,NULL) ;

	return CptDialog::OnInitDialog() ;
}

void CXCDownloadDlg::SetDownloadFile(CptString strURLFile,CptString strFolder) 
{
	m_strURLFile = strURLFile ;
	m_strFolder = strFolder ;
}


UINT CXCDownloadDlg::DownloadThreadFunc(void* p) 
{
	CXCDownloadDlg* pThis = (CXCDownloadDlg*)p ;
	_ASSERT(pThis!=NULL) ;

	pThis->DownloadFile(pThis->m_strURLFile,pThis->m_strFolder) ;
	::CloseHandle(pThis->m_hThread) ;
	pThis->m_hThread = NULL ;

	pThis->PerformButton(IDCANCEL) ;

	return 0 ;
}

bool CXCDownloadDlg::OnCancel()
{
	bool bResult = true ;
	
	if(m_hThread!=NULL)
	{
		bResult = false ;
		m_bPause = true ;

		CptString strWarningTitle = CptMultipleLanguage::GetInstance()->GetString(IDS_TITLE_WARNING) ;
		CptString strMsgText = CptMultipleLanguage::GetInstance()->GetString(IDS_MESSAGEBOX_AREYOUSURECANCELDOWNLOAD) ;

		if(::MessageBox(NULL,strMsgText.c_str(),strWarningTitle.c_str(),MB_YESNO)==IDYES)
		{
			m_bCancel = true ;
			bResult = true ;
			
			::ShowWindow(this->GetSafeHwnd(),SW_HIDE) ;
		}

		m_bPause = false ;
	}


	return bResult ;
}

bool CXCDownloadDlg::DownloadFile(CptString strURLFile,CptString strFolder)
{
	bool bRet = false ;

	HINTERNET hInetSession = ::InternetOpen(_T("practise-think Checker"),INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0); 

	if(hInetSession!=NULL)   
	{ 
		strURLFile.TrimLeft(' ') ;
		strURLFile.TrimRight(' ') ;
		HINTERNET hHttpFile=::InternetOpenUrl(hInetSession,strURLFile.c_str(),NULL,0,INTERNET_FLAG_RELOAD,0); 

		if(hHttpFile!=NULL) 
		{
			BYTE Buf[1024] = {0} ;
			DWORD dwSize = sizeof(Buf) ;

			if(::InternetQueryOption(hHttpFile,INTERNET_OPTION_URL ,(LPVOID)Buf,&dwSize))
			{
				TCHAR szFileSize[32] = {0} ;
				DWORD dwFileSize = 0 ;
				DWORD  dwSizeBuffer = sizeof(szFileSize) ;
				CptStringList sl ;

				if(sl.Split((TCHAR*)Buf,_T("/"))>0 && ::HttpQueryInfo(hHttpFile,HTTP_QUERY_CONTENT_LENGTH,(LPVOID)szFileSize,&dwSizeBuffer,NULL)) 
				{ 
					CptString strFile ;

					strFile.Format(_T("%s\\%s"),strFolder,sl[sl.GetCount()-1]) ;

					HANDLE hFile = ::CreateFile(strFile.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL) ;

					if(hFile!=INVALID_HANDLE_VALUE)
					{
						this->SetDlgItemText(IDC_STATIC_FILENAME,sl[sl.GetCount()-1]) ;

						CptProgressBar ProBar ;

						ProBar.SetControl(this->GetDlgItem(IDC_PROGRESS_DOWNLOAD)) ;
						ProBar.SetRange(0,100) ;

						int nPercent = 0 ;
						CptString strPercent ;

						dwSizeBuffer = ::_tstoi(szFileSize) ;

						dwFileSize = dwSizeBuffer ;
						DWORD dwRemainSize = dwSizeBuffer ;
						
						DWORD  dwBytesRead = 0; 
						dwSizeBuffer = 0 ;

						bRet = true ;
						while(dwRemainSize>0 && !m_bCancel)
						{
							::InternetReadFile(hHttpFile, Buf,  sizeof(Buf), &dwBytesRead); 

							dwRemainSize -= dwBytesRead ;
							if(!::WriteFile(hFile,Buf,dwBytesRead,&dwSizeBuffer,NULL))
							{
								bRet = false ;
								break ;
							}

							nPercent = (int)((((float)(dwFileSize-dwRemainSize))/dwFileSize)*100) ;

							strPercent.Format(_T("%d%%"),nPercent) ;
							
							this->SetDlgItemText(IDC_STATIC_PERCENT,strPercent) ;

							ProBar.SetValue(nPercent) ;

							while(m_bPause)
							{
								::Sleep(50) ;
							}
						}

						::CloseHandle(hFile) ;

						if(!bRet || m_bCancel)
						{
							::DeleteFile(strFile.c_str()) ;
						}
					}

				}
			}
		}
	}

	return bRet ;
}