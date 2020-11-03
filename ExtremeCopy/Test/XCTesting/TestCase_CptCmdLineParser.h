#pragma once


#include "../../Common/ptCmdLineParser.h"
#include <cppunit/TestCase.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>


class CTestCase_CptCmdLineParser : public CppUnit::TestCase
{
	CPPUNIT_TEST_SUITE(CTestCase_CptCmdLineParser);
	CPPUNIT_TEST(TestFunction_CheckBarCharacter);
	CPPUNIT_TEST(TestFunction_CheckSwitchValidate);
	CPPUNIT_TEST_SUITE_END();

public:
	CTestCase_CptCmdLineParser(void);
	~CTestCase_CptCmdLineParser(void);

	void TestFunction_CheckBarCharacter() ;
	void TestFunction_CheckSwitchValidate();
};

