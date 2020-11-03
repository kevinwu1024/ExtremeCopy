#include "StdAfx.h"
#include "TestCase_CptString.h"


CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CTestCase_CptString,TEST_SUITE_NAME_COMMON);

CTestCase_CptString::CTestCase_CptString(void)
{
}


CTestCase_CptString::~CTestCase_CptString(void)
{
}

void CTestCase_CptString::TestEmptyStatus() 
{
	/*
	int nLength = 0 ;
	int nRemoveCount = 0 ;
	CptString str1 ;

	CPPUNIT_ASSERT_EQUAL(CptString::m_pStrMemBuf->m_nAllocCounter,0) ;
	CPPUNIT_ASSERT_EQUAL(CptString::m_pStrMemBuf->m_uAllocBufSize,(unsigned)0) ;

	str1 = _T("aaaa") ;

	CPPUNIT_ASSERT_EQUAL(CptString::m_pStrMemBuf->m_nAllocCounter,1) ;

	str1 = _T("") ;

	CPPUNIT_ASSERT_EQUAL(CptString::m_pStrMemBuf->m_nAllocCounter,0) ;
	CPPUNIT_ASSERT_EQUAL(CptString::m_pStrMemBuf->m_uAllocBufSize,(unsigned)0) ;

	str1 = _T("abcd") ;

	CPPUNIT_ASSERT_EQUAL(CptString::m_pStrMemBuf->m_nAllocCounter,1) ;

	nLength = str1.GetLength() ;

	nRemoveCount = str1.Remove(0,str1.GetLength()) ;
	
	CPPUNIT_ASSERT_EQUAL(nRemoveCount,nLength) ;
	CPPUNIT_ASSERT_EQUAL(CptString::m_pStrMemBuf->m_nAllocCounter,0) ;
	CPPUNIT_ASSERT_EQUAL(CptString::m_pStrMemBuf->m_uAllocBufSize,(unsigned)0) ;

	str1 = 'a' ;
	nLength = str1.GetLength() ;
	nRemoveCount = str1.Remove('a') ;

	CPPUNIT_ASSERT_EQUAL(nRemoveCount,nLength) ;
	CPPUNIT_ASSERT_EQUAL(CptString::m_pStrMemBuf->m_nAllocCounter,0) ;
	CPPUNIT_ASSERT_EQUAL(CptString::m_pStrMemBuf->m_uAllocBufSize,(unsigned)0) ;
	*/
}

void CTestCase_CptString::TestFunction_CheckMemoryMechanism() 
{
	this->TestEmptyStatus() ;
}