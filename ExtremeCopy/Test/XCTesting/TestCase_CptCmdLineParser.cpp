#include "StdAfx.h"
#include "TestCase_CptCmdLineParser.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CTestCase_CptCmdLineParser,TEST_SUITE_NAME_COMMON);

CTestCase_CptCmdLineParser::CTestCase_CptCmdLineParser(void)
{
}


CTestCase_CptCmdLineParser::~CTestCase_CptCmdLineParser(void)
{
}


void CTestCase_CptCmdLineParser::TestFunction_CheckBarCharacter()
{
	const TCHAR* TestCmdLine = _T("-switch_a -switch_b \"the -switch_d string\" -switch_c") ;

	CptCmdLineParser parser ;

	int SwitchCount = parser.Parse(TestCmdLine) ;

	CPPUNIT_ASSERT_EQUAL(SwitchCount,3) ;

	pt_STL_vector(CptString) ParamVer ;

	bool bResult = parser.GetParams(_T("-switch_b"),ParamVer) ;

	CPPUNIT_ASSERT_EQUAL(bResult,true) ;
	CPPUNIT_ASSERT_EQUAL(ParamVer.size(),(size_t)1) ;
	CPPUNIT_ASSERT_EQUAL(ParamVer[0].Compare(_T("the -switch_d string")),0) ;
}

void CTestCase_CptCmdLineParser::TestFunction_CheckSwitchValidate()
{
	const TCHAR* TestCmdLine = _T("-switch_a not-switch_b \"isnot-switch too\" -switch_c --not") ;

	CptCmdLineParser parser ;

	int SwitchCount = parser.Parse(TestCmdLine) ;

	pt_STL_vector(CptString) ParamVer ;

	bool bResult = parser.GetParams(_T("-switch_c"),ParamVer) ;

	size_t kkk = ParamVer.size();

	CPPUNIT_ASSERT_EQUAL(bResult,true) ;
	CPPUNIT_ASSERT_EQUAL(ParamVer.size(),(size_t)1) ;
	CPPUNIT_ASSERT_EQUAL(ParamVer[0].Compare(_T("--not")),0) ;
}

