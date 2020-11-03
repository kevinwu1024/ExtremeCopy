
// XCTesting.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XCTesting.h"
#include "XCTestingDlg.h"

#include "../../Common/ptGlobal.h"

#include "TestCase_XCStatistics.h"

#include <cppunit/ui/mfc/TestRunner.h>
#include <cppunit\extensions\TestFactoryRegistry.h>

#ifdef _DEBUG
#pragma comment(lib,"cppunitd.lib")
#pragma comment(lib,"testrunnerd.lib")
#else
#pragma comment(lib,"cppunit.lib")
#pragma comment(lib,"testrunner.lib")
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CXCTestingApp

BEGIN_MESSAGE_MAP(CXCTestingApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CXCTestingApp construction

CXCTestingApp::CXCTestingApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CXCTestingApp object

CXCTestingApp theApp;


// CXCTestingApp initialization

BOOL CXCTestingApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.

	//INITCOMMONCONTROLSEX InitCtrls;
	//InitCtrls.dwSize = sizeof(InitCtrls);
	//// Set this to include all the common control classes you want to use
	//// in your application.
	////InitCtrls.dwICC = ICC_WIN95_CLASSES;
	//InitCommonControlsEx(&InitCtrls);

	//CWinApp::InitInstance();

	AfxEnableControlContainer();

	CppUnit::MfcUi::TestRunner runner ;

	runner.addTest(CppUnit::TestFactoryRegistry::getRegistry(TEST_SUITE_NAME_COMMON).makeTest()) ;
	//runner.addTest(CppUnit::TestFactoryRegistry::getRegistry(TEST_SUITE_NAME_APP).makeTest()) ;
	//runner.addTest(CppUnit::TestFactoryRegistry::getRegistry(TEST_SUITE_NAME_CORE).makeTest()) ;
	//runner.addTest(CppUnit::TestFactoryRegistry::getRegistry(TEST_SUITE_NAME_TEM).makeTest()) ;

	runner.run() ;

	return FALSE;
}

