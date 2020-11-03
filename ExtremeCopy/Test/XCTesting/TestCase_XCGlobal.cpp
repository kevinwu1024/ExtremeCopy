#include "StdAfx.h"
#include "TestCase_XCGlobal.h"

#include "..\..\App\XCGlobal.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CTestCase_XCGlobal,TEST_SUITE_NAME_APP);

CTestCase_XCGlobal::CTestCase_XCGlobal(void)
{
}


CTestCase_XCGlobal::~CTestCase_XCGlobal(void)
{
}

void CTestCase_XCGlobal::AssertWithMessage(CptString strMessage,CptString strCorrect,CptString strResult)
{
	CptStringA strMessageA = strMessage ;
	CptStringA strCorrectA = strCorrect ;
	CptStringA strResultA = strResult ;

	std::string strStdCorrect = strCorrectA.c_str() ;
	std::string strStdResult = strResultA.c_str() ;

	//Debug_Printf(_T("correct = %s len=%d result = %s len=%d equal=%d"),strCorrect,strCorrect.GetLength(),strResult,strResult.GetLength(),strCorrectA.Compare(strResultA)) ;
	CPPUNIT_ASSERT_EQUAL_MESSAGE(strMessageA.c_str(),strStdCorrect,strStdResult) ;
}

void CTestCase_XCGlobal::Test_Function_MakeUnlimitFileName() 
{
	CptString strResult ;
	CptString strMessage ;
	CptString strTest ;
	CptString strCorrect ;

	CptStringA strResultA ;
	CptStringA strMessageA ;
	CptStringA strTestA ;
	CptStringA strCorrectA ;

	{
		strTest = _T("c:\\abc") ;

		strResult = CptGlobal::MakeUnlimitFileName(strTest,true) ;
		strMessage.Format(_T("1 不带'无限制符'的常规文件 [add] '无限制符'.\r\n  test: '%s' \r\n result:'%s'"),strTest,strResult) ;
		strCorrect = _T("\\\\?\\")+strTest ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;


		strResult = CptGlobal::MakeUnlimitFileName(strTest,false) ;
		strMessage.Format(_T("1 不带'无限制符'的常规文件 [remove] '无限制符'.\r\n  test: '%s' \r\n result:'%s'"),strTest,strResult) ;
		strCorrect = strTest ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;
	}

	{
		strTest = _T("file:\\\\\\c:\\abc") ;

		strResult = CptGlobal::MakeUnlimitFileName(strTest,true) ;
		strMessage.Format(_T("2 带'file:\\\\\\'前缀文件 [add] '无限制符'.\r\n  test: '%s' \r\n result:'%s'"),strTest,strResult) ;
		strCorrect = _T("\\\\?\\c:\\abc") ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;


		strResult = CptGlobal::MakeUnlimitFileName(strTest,false) ;
		strMessage.Format(_T("2 带'file:\\\\\\'前缀文件 [remove] '无限制符'.\r\n  test: '%s'\r\n  result:'%s'"),strTest,strResult) ;
		strCorrect = _T("c:\\abc") ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;
	}

	{
		strTest = _T("\\\\ilei\\pTest3") ;

		strResult = CptGlobal::MakeUnlimitFileName(strTest,true) ;
		strMessage.Format(_T("3 带'无限制符'前缀文件 [add] '无限制符'.\r\n  test: '%s'\r\n  result:'%s'"),strTest,strResult) ;
		strCorrect = _T("\\\\?\\UNC")+strTest ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;

		strResult = CptGlobal::MakeUnlimitFileName(strTest,false) ;
		strMessage.Format(_T("3 带'无限制符'前缀文件 [remove] '无限制符'.\r\n  test: '%s'\r\n  result:'%s'"),strTest,strResult) ;
		strCorrect = strTest ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;
	}

	{
		strTest = _T("\\\\?\\UNC\\\\ilei\\test4") ;

		strResult = CptGlobal::MakeUnlimitFileName(strTest,true) ;
		strMessage.Format(_T("4 带'无限制符'前缀远程文件 [add] '无限制符'.\r\n  test: '%s'\r\n  result:'%s'"),strTest,strResult) ;
		strCorrect = strTest ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;

		strResult = CptGlobal::MakeUnlimitFileName(strTest,false) ;
		strMessage.Format(_T("4 带'无限制符'前缀远程文件 [remove] '无限制符'.\r\n  test: '%s'\r\n  result:'%s'"),strTest,strResult) ;
		strCorrect = _T("\\\\ilei\\test4") ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;
	}

	{
		strTest = _T("\\\\?\\c:\\ilei\\test5") ;

		strResult = CptGlobal::MakeUnlimitFileName(strTest,true) ;
		strMessage.Format(_T("5 带'无限制符'前缀本地文件 [add] '无限制符'.\r\n  test: '%s'\r\n  result:'%s'"),strTest,strResult) ;
		strCorrect = strTest ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;

		strResult = CptGlobal::MakeUnlimitFileName(strTest,false) ;
		strMessage.Format(_T("5 带'无限制符'前缀本地文件 [remove] '无限制符'.\r\n  test: '%s'\r\n  result:'%s'"),strTest,strResult) ;
		strCorrect = _T("c:\\ilei\\test5") ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;
	}

	{
		strTest = _T("jioefj;lesafie") ;

		strResult = CptGlobal::MakeUnlimitFileName(strTest,true) ;
		strMessage.Format(_T("6 无效的测试文件字符串.\r\n  test: '%s'\r\n  result:'%s'"),strTest,strResult) ;
		strCorrect = _T("") ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;

		strResult = CptGlobal::MakeUnlimitFileName(strTest,false) ;
		strMessage.Format(_T("6 无效的测试文件字符串.\r\n  test: '%s'\r\n  result:'%s'"),strTest,strResult) ;
		strCorrect = _T("") ;
		this->AssertWithMessage(strMessage,strCorrect,strResult) ;
	}
}