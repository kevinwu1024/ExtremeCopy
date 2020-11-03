//#pragma once
//
//#include <cppunit/TestCase.h>
//#include <cppunit/ui/text/TextTestRunner.h>
//
//#include <cppunit/extensions/HelperMacros.h>
//#include "..\..\App\XCStatistics.h"
//
//
//class CTestCase_XCStatistics : public CppUnit::TestCase
//{
//	CPPUNIT_TEST_SUITE(CTestCase_XCStatistics);
//	CPPUNIT_TEST(Test_RunStop_EmptyParameter);
//	CPPUNIT_TEST(Test_RunStop_InvalidParameter);
//	CPPUNIT_TEST(Test_RunStop_ValidParameter);
//	CPPUNIT_TEST(Test_RunStop_ValidParameter_Interrupt);
//	CPPUNIT_TEST(Test_SizeCal_SpecialValue);
//	CPPUNIT_TEST(Test_RunStop_ValidParameterCrossWithInterrupt);
//	//CPPUNIT_TEST(Test_SpeedCal);
//	CPPUNIT_TEST_SUITE_END();
//
//public:
//	CTestCase_XCStatistics(void);
//	virtual ~CTestCase_XCStatistics(void);
//
//	virtual void setUp() ;
//	virtual void tearDown() ;
//
//	void Test_RunStop_EmptyParameter() ;
//	void Test_RunStop_InvalidParameter() ;
//
//	void Test_RunStop_ValidParameter() ;
//	void Test_RunStop_ValidParameter_Interrupt() ;
//
//	// 正常完成和中断完成来交叉运行
//	void Test_RunStop_ValidParameterCrossWithInterrupt() ;
//
//	//void Test_SizeCal_ValidParameter() ;
//
//	void Test_SizeCal_SpecialValue() ;
//	void Test_SpeedCal() ;
//
//private:
//	void TestValidParameter() ;
//	void TestValidParameterWithInterrupt() ;
//	
//	void TestSizeLessThan4G() ;
//	void TestSizeOverThan4G() ;
//
//	void StartLegally() ;
//
//private:
//	CXCStatistics m_Sta ;
//	
////const TCHAR* CaseName =  _T("CTestCase_XCStatistics") ;
//};
//
