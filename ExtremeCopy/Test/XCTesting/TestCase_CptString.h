#pragma once

#include "../../Common/ptString.h"
#include <cppunit/TestCase.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>


class CTestCase_CptString :
	public CppUnit::TestCase
{
	CPPUNIT_TEST_SUITE(CTestCase_CptString);
	CPPUNIT_TEST(TestFunction_CheckMemoryMechanism);
	CPPUNIT_TEST_SUITE_END();

public:
	CTestCase_CptString(void);
	~CTestCase_CptString(void);

	void TestFunction_CheckMemoryMechanism() ;

private:
	void TestEmptyStatus() ;
};

