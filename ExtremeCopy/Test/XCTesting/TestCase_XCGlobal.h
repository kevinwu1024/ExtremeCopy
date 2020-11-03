#pragma once
#include "..\..\Common\ptGlobal.h"

#include <cppunit/TestCase.h>
#include <cppunit/ui/text/TextTestRunner.h>

#include <cppunit/extensions/HelperMacros.h>

class CTestCase_XCGlobal : public CppUnit::TestCase
{
	CPPUNIT_TEST_SUITE(CTestCase_XCGlobal);
	CPPUNIT_TEST(Test_Function_MakeUnlimitFileName);
	CPPUNIT_TEST_SUITE_END();

private:
	void AssertWithMessage(CptString strMessage,CptString strCorrect,CptString strResult) ;

public:
	CTestCase_XCGlobal(void);
	~CTestCase_XCGlobal(void);

	void Test_Function_MakeUnlimitFileName() ;
};

