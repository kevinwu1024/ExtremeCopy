#include "stdafx.h"
#include "ptBugReport.h"
#include "XZip.h"
#include <Dbghelp.h>
#include "CSmtp.h"

CptBugReport*	CptBugReport::m_pInstance = NULL ;

CptBugReport::CptBugReport(void)
{
}


CptBugReport::~CptBugReport(void)
{
}

CptBugReport* CptBugReport::GetInstance() 
{
	if(m_pInstance==NULL)
	{
		m_pInstance = new CptBugReport() ;
	}

	return m_pInstance;
}

void CptBugReport::Release() 
{
	if(m_pInstance==NULL)
	{
		delete m_pInstance ;
		m_pInstance = NULL ;
	}
}


void CptBugReport::Begin(const SBugStartInfo& bsi) 
{
	m_bsi = bsi ;
	::SetUnhandledExceptionFilter(CptBugReport::UnhandledExceptionFilter2) ;
}

void CptBugReport::End() 
{
	::SetUnhandledExceptionFilter(NULL) ;
}

bool CptBugReport::SendDumpViaEmail(CptString strDumpFile)
{
	bool bRet = false ;

	TCHAR szTemPath[MAX_PATH] = {0} ;

	::GetTempPath(sizeof(szTemPath)/sizeof(TCHAR),szTemPath) ;

	int nLen = _tcslen(szTemPath) ;

	if(szTemPath[nLen-1]!='\\' && szTemPath[nLen-1]!='/')
	{
		::_tcscat(szTemPath,_T("\\")) ;
	}

	::_tcscat(szTemPath,_T("ExtremeCopyDump.zip")) ;

	HZIP hzip = ::CreateZip(szTemPath,0,ZIP_FILENAME) ;

	if(hzip!=NULL)
	{
		//ZRESULT result = ::ZipAdd(hzip,(LPCTSTR)m_bsi.strDumpFileName,(void*)(LPCTSTR)strDumpFile,0,ZIP_FILENAME) ;
		ZRESULT result = ::ZipAdd(hzip,(LPCTSTR)_T("ExtremeCopy.dmp"),(void*)(LPCTSTR)strDumpFile,0,ZIP_FILENAME) ;
		::CloseZip(hzip) ;
		hzip = NULL ;

		if(result==ZR_OK)
		{
			CSmtp mail;

			mail.SetSMTPServer("typhoon.lunarbreeze.com",465);
			mail.SetSecurityType(CSmtp::USE_SSL);

			mail.SetLogin("report_bug@easersoft.com");
			mail.SetSenderMail("report_bug@easersoft.com");
			mail.SetPassword("5,,?og+(DI8t");

			/**
			mail.SetLogin("support@easersoft.com");
			mail.SetSenderMail("support@easersoft.com");
			mail.SetPassword(",y^vT}5BJ%.*");
			/**/

			mail.AddRecipient("support@easersoft.com") ;

			mail.SetSenderName("product reporter");
			mail.SetXPriority(XPRIORITY_NORMAL);
			
			mail.SetSubject("bug report") ;
			mail.AddMsgLine("hi, this report generate by ExtremeCopy bug reportor !") ;

#ifdef _UNICODE
			char szTemDumpPath[MAX_PATH] = {0} ;
			int nLenOfByte = (int)::wcslen(szTemPath)*sizeof(wchar_t) ;

			::WideCharToMultiByte(CP_ACP, 0, szTemPath, nLenOfByte/sizeof(wchar_t) , 
				(char*)szTemDumpPath,sizeof(szTemDumpPath), NULL, NULL);

			mail.AddAttachment(szTemDumpPath) ;
#else
			mail.AddAttachment(szTemPath) ;
#endif
			mail.Send() ;
			
		}
	}

	return bRet ;
}

LONG CptBugReport::UnhandledExceptionFilter2(
	__in          struct _EXCEPTION_POINTERS* pExceptionInfo
	)
{
	if(GetInstance()->m_bsi.BugCB!=NULL && GetInstance()->m_bsi.BugCB(GetInstance()->m_bsi.pUserParam))
	{
		TCHAR szTemPath[MAX_PATH] = {0} ;

		::GetTempPath(sizeof(szTemPath)/sizeof(TCHAR),szTemPath) ;

		int nLen = _tcslen(szTemPath) ;

		if(szTemPath[nLen-1]!='\\' && szTemPath[nLen-1]!='/')
		{
			::_tcscat(szTemPath,_T("\\")) ;
		}

		::_tcscat(szTemPath,(LPCTSTR)(GetInstance()->m_bsi.strDumpFileName)) ;

		// Create the file
		HANDLE hMiniDumpFile = CreateFile(
			szTemPath,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
			NULL);

		if(hMiniDumpFile!=INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION eInfo;

			eInfo.ThreadId = GetCurrentThreadId();
			eInfo.ExceptionPointers = pExceptionInfo;
			eInfo.ClientPointers = FALSE;

			::MiniDumpWriteDump(
				GetCurrentProcess(),
				GetCurrentProcessId(),
				hMiniDumpFile,
				MiniDumpNormal,
				&eInfo,
				NULL,
				NULL);

			::CloseHandle(hMiniDumpFile) ;

			GetInstance()->SendDumpViaEmail(szTemPath) ;
		}
	}
	


	//AfxMessageBox(_T("UnhandledExceptionFilter2() called")) ;
	//RecordExceptionInfo(ExceptionInfo, _T("ExceptionAttacher.cpp - AfxWinMain")) ;

	return EXCEPTION_EXECUTE_HANDLER ;
}