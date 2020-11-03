#pragma once

typedef bool (__stdcall *lpfnSendBugReportCallback_t)(void*) ;

class CptBugReport
{
public:
	struct SBugStartInfo
	{
		lpfnSendBugReportCallback_t	BugCB ;
		void*	pUserParam ;
		CptString		strSMTPLoginName ;
		CptString		strSMTPLoginPwd ;
		CptString		strDumpFileName ;
	};

protected:
	CptBugReport(void);
	~CptBugReport(void);

public:
	static CptBugReport* GetInstance() ;
	static void Release() ;
	void Begin(const SBugStartInfo& bsi) ;
	void End() ;

//private:
	static LONG __stdcall UnhandledExceptionFilter2(struct _EXCEPTION_POINTERS* ExceptionInfo) ;
	bool SendDumpViaEmail(CptString strDumpFile) ;
private:
	SBugStartInfo		m_bsi ;
	static CptBugReport*	m_pInstance ;
};

