#include "StdAfx.h"
//#include "TestCase_XCStatistics.h"
//#include "..\..\Common\ptGlobal.h"
//
//
//CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CTestCase_XCStatistics,TEST_SUITE_NAME_APP);
//
//static const unsigned int g_uTotalFileCount = 19389;
//static const unsigned __int64 g_uTotalFileSize = 254390756;
//
//CTestCase_XCStatistics::CTestCase_XCStatistics(void)
//{
//	//Debug_Printf(_T("CTestCase_XCStatistics::CTestCase_XCStatistics()")) ;
//}
//
//
//CTestCase_XCStatistics::~CTestCase_XCStatistics(void)
//{
//	//Debug_Printf(_T("CTestCase_XCStatistics::~CTestCase_XCStatistics()")) ;
//}
//
//
//void CTestCase_XCStatistics::setUp() 
//{
//	//Debug_Printf(_T("CTestCase_XCStatistics::setUp()")) ;
//}
//
//void CTestCase_XCStatistics::tearDown() 
//{
//	//Debug_Printf(_T("CTestCase_XCStatistics::tearDown()")) ;
//}
//
//void CTestCase_XCStatistics::Test_RunStop_EmptyParameter()
//{
//	//CXCStatistics sta ;
//
//	pt_STL_vector(CptString) EmptyVer ;
//
//	m_Sta.Stop() ;
//
//	bool bResult = m_Sta.Start(EmptyVer) ;
//
//	CPPUNIT_ASSERT_EQUAL(false,bResult) ;
//}
//
//void CTestCase_XCStatistics::Test_RunStop_InvalidParameter() 
//{
//	//CXCStatistics sta ;
//	m_Sta.Stop() ;
//
//	pt_STL_vector(CptString) InvalidVer ;
//
//	InvalidVer.push_back(g_pInvalidDir) ;
//
//	bool bResult = m_Sta.Start(InvalidVer) ;
//
//	CPPUNIT_ASSERT_EQUAL(false,bResult) ;
//}
//
//void CTestCase_XCStatistics::Test_RunStop_ValidParameter() 
//{
//	for(int i=0;i<2;++i)
//	{
//		this->TestValidParameter() ;
//	}
//}
//
//void CTestCase_XCStatistics::Test_RunStop_ValidParameter_Interrupt()
//{
//	for(int i=0;i<2;++i)
//	{
//		this->TestValidParameterWithInterrupt() ;
//	}
//}
//
//void CTestCase_XCStatistics::Test_RunStop_ValidParameterCrossWithInterrupt()
//{
//	for(int i=0;i<2;++i)
//	{
//		this->TestValidParameter() ;
//		this->TestValidParameterWithInterrupt() ;
//	}
//}
//
//
//void CTestCase_XCStatistics::TestValidParameter() 
//{
//	m_Sta.Stop() ;
//
//	pt_STL_vector(CptString) ValidVer ;
//
//	ValidVer.push_back(g_pValidDirWithFolder) ;
//
//	bool bResult = m_Sta.Start(ValidVer) ;
//
//	while(!m_Sta.DoesFolderSizeDone())
//	{
//		::Sleep(100) ;
//	}
//
//	const SStatisticalValue& ss = m_Sta.GetStaData() ;
//
//	CPPUNIT_ASSERT_EQUAL(true,bResult) ;
//
//	CptStringA str ;
//	str.Format("%u",ss.nTotalFiles) ;
//
//	CPPUNIT_ASSERT_EQUAL_MESSAGE(str.c_str(),(unsigned)g_uTotalFileCount,ss.nTotalFiles) ;
//
//	CPPUNIT_ASSERT_EQUAL((unsigned __int64)g_uTotalFileSize,ss.uTotalSize) ;
//
//	m_Sta.Stop() ;
//}
//
//void CTestCase_XCStatistics::TestValidParameterWithInterrupt() 
//{
//	m_Sta.Stop() ;
//
//	pt_STL_vector(CptString) ValidVer ;
//
//	ValidVer.push_back(g_pValidDirWithFolder) ;
//
//	bool bResult = m_Sta.Start(ValidVer) ;
//
//	m_Sta.Stop() ;
//
//	CPPUNIT_ASSERT_EQUAL(true,bResult) ;
//
//	CPPUNIT_ASSERT_EQUAL(false,m_Sta.DoesFolderSizeDone()) ;
//}
//
//void CTestCase_XCStatistics::Test_SizeCal_SpecialValue() 
//{
//	this->StartLegally() ;
//
//	unsigned int nDoneFileCount = m_Sta.GetStaData().nDoneFiles ;
//	unsigned __int64 nSize = 0 ;
//	unsigned __int64 nSize4G = 1024*1024*1024 ;
//	nSize4G = 4*nSize4G ;
//
//	// 0
//	nSize = 0 ;
//	nDoneFileCount = m_Sta.GetStaData().nDoneFiles ;
//	m_Sta.SetCurWholeSize(nSize) ;
//	m_Sta.SetDataOccured(0,false) ;
//
//	CPPUNIT_ASSERT_EQUAL(nDoneFileCount+1,m_Sta.GetStaData().nDoneFiles) ;
//
//
//	// 1
//	nSize =1 ;
//	nDoneFileCount = m_Sta.GetStaData().nDoneFiles ;
//	m_Sta.SetCurWholeSize(nSize) ;
//	m_Sta.SetDataOccured(nSize,false) ;
//
//	CPPUNIT_ASSERT_EQUAL(nDoneFileCount+1,m_Sta.GetStaData().nDoneFiles) ;
//
//	// 4G
//	nSize = nSize4G ;
//	nDoneFileCount = m_Sta.GetStaData().nDoneFiles ;
//	m_Sta.SetCurWholeSize(nSize) ;
//	unsigned int uSize1 = nSize - nSize/2 ;
//	m_Sta.SetDataOccured(uSize1,false) ;
//	m_Sta.SetDataOccured(nSize-uSize1,false) ;
//
//	CPPUNIT_ASSERT_EQUAL(nDoneFileCount+1,m_Sta.GetStaData().nDoneFiles) ;
//
//	// 4G-1
//	nSize = nSize4G-1 ;
//	nDoneFileCount = m_Sta.GetStaData().nDoneFiles ;
//	m_Sta.SetCurWholeSize(nSize) ;
//	uSize1 = nSize - nSize/2 ;
//	m_Sta.SetDataOccured(uSize1,false) ;
//	m_Sta.SetDataOccured(nSize-uSize1,false) ;
//
//	CPPUNIT_ASSERT_EQUAL(nDoneFileCount+1,m_Sta.GetStaData().nDoneFiles) ;
//
//	// 4G+1
//	nSize = nSize4G+1 ;
//	nDoneFileCount = m_Sta.GetStaData().nDoneFiles ;
//	m_Sta.SetCurWholeSize(nSize) ;
//	uSize1 = nSize - nSize/2 ;
//	m_Sta.SetDataOccured(uSize1,false) ;
//	m_Sta.SetDataOccured(nSize-uSize1,false) ;
//
//	CPPUNIT_ASSERT_EQUAL(nDoneFileCount+1,m_Sta.GetStaData().nDoneFiles) ;
//
//	m_Sta.Stop() ;
//}
//
////void CTestCase_XCStatistics::Test_SizeCal_ValidParameter() 
////{
////	this->StartLegally() ;
////
////	m_Sta.Stop() ;
////}
////
////// 小于4G
////void CTestCase_XCStatistics::TestSizeLessThan4G() 
////{
////
////}
////
////// 大于4G
////void CTestCase_XCStatistics::TestSizeOverThan4G() 
////{
////
////}
//
//void CTestCase_XCStatistics::Test_SpeedCal()
//{
//	this->StartLegally() ;
//
//	while(!m_Sta.DoesFolderSizeDone())
//	{
//		::Sleep(50) ;
//	}
//
//	::srand(::time(NULL)) ;
//
//	m_Sta.SetCurWholeSize(g_uTotalFileSize) ;
//
//	unsigned int nPerSize = 0 ;
//	unsigned uTotalSize = 0 ;
//	int nTimes = 5 ;
//
//	for(int i=0;i<nTimes;i++)
//	{
//		nPerSize = ::rand()*1024 ;
//		m_Sta.SetDataOccured(nPerSize,false) ;
//
//		uTotalSize += nPerSize ;
//
//		m_Sta.MoveNextSecond() ;
//	}
//
//	float fSpeed = m_Sta.GetStaData().fSpeed ;
//	float fSpeed2 = (float)uTotalSize/nTimes ;
//
//	CptStringA str ;
//	str.Format("expect=%0.2f act=%0.2f",fSpeed2,fSpeed) ;
//
//	CPPUNIT_ASSERT_EQUAL_MESSAGE(str.c_str(),fSpeed2,fSpeed) ;
//
//}
//
//void CTestCase_XCStatistics::StartLegally() 
//{
//	m_Sta.Stop() ;
//
//	pt_STL_vector(CptString) ValidVer ;
//
//	ValidVer.push_back(g_pValidDirWithFolder) ;
//
//	bool bResult = m_Sta.Start(ValidVer) ;
//
//	CPPUNIT_ASSERT_EQUAL(true,bResult) ;
//}