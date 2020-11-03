#pragma once

#include <cppunit/TestCase.h>
#include <cppunit/ui/text/TextTestRunner.h>

#include <cppunit/extensions/HelperMacros.h>
#include "..\..\Core\XCCore.h"
#include "..\..\Core\XCCopyingEvent.h"
#include "..\..\Common\ptTypeDef.h"
#include "..\..\App\XCVerifyResult.h"

/**/
class CTestCase_XCCoreRunning : public CppUnit::TestCase,public CXCCopyingEventReceiver,public CXCVerifyExceptionCB
{
	CPPUNIT_TEST_SUITE(CTestCase_XCCoreRunning);
	
	CPPUNIT_TEST(Test_SameHD_CopySingalFile_SmallFile);
	CPPUNIT_TEST(Test_SameHD_CopySingalFile_BigFile);
	CPPUNIT_TEST(Test_SameHD_CopyMultipleFiles_SmallFile);
	CPPUNIT_TEST(Test_SameHD_CopyMultipleFiles_BigFile);
	CPPUNIT_TEST(Test_SameHD_CopyMultipleFiles_SmallFile_WithFolder);
	CPPUNIT_TEST(Test_SameHD_CopyMultipleFiles_BigFile_WithFolder);

	CPPUNIT_TEST(Test_DiffHD_CopySingalFile_SmallFile);
	CPPUNIT_TEST(Test_DiffHD_CopySingalFile_BigFile);
	CPPUNIT_TEST(Test_DiffHD_CopyMultipleFiles_SmallFile);
	CPPUNIT_TEST(Test_DiffHD_CopyMultipleFiles_BigFile);
	CPPUNIT_TEST(Test_DiffHD_CopyMultipleFiles_SmallFile_WithFolder);
	CPPUNIT_TEST(Test_DiffHD_CopyMultipleFiles_BigFile_WithFolder);
	
	CPPUNIT_TEST_SUITE_END();

public:
	CTestCase_XCCoreRunning(void);
	~CTestCase_XCCoreRunning(void);

	void Test_SameHD_CopySingalFile_SmallFile() ;
	void Test_SameHD_CopySingalFile_BigFile() ;
	void Test_SameHD_CopyMultipleFiles_SmallFile() ;
	void Test_SameHD_CopyMultipleFiles_BigFile() ;
	void Test_SameHD_CopyMultipleFiles_SmallFile_WithFolder() ;
	void Test_SameHD_CopyMultipleFiles_BigFile_WithFolder() ;

	void Test_DiffHD_CopySingalFile_SmallFile() ;
	void Test_DiffHD_CopySingalFile_BigFile() ;
	void Test_DiffHD_CopyMultipleFiles_SmallFile() ;
	void Test_DiffHD_CopyMultipleFiles_BigFile() ;
	void Test_DiffHD_CopyMultipleFiles_SmallFile_WithFolder() ;
	void Test_DiffHD_CopyMultipleFiles_BigFile_WithFolder() ;

	void Test_CopyFile(pt_STL_vector(CptString) strVer,CptString strDstFolder) ;

	virtual int CopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1=NULL,void* pParam2=NULL) ;

	virtual void setUp() ;
	virtual void tearDown() ;

	virtual ECopyFileState OnFileDiff(CptString strSrc,CptString strDst,CXCVerifyResult::EFileDiffType fdt) ;

	virtual ECopyFileState OnVerifyFileDiff(CptString strSrc,CptString strDst,CXCVerifyResult::EFileDiffType fdt) {return CFS_Stop;}
	virtual int OnCopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1=NULL,void* pParam2=NULL) {return 0;}

private:
	void CleanFolder(CptString strFolder) ;
private:
	CXCCopyingEvent		m_CopyingEvent ;
	CXCCore				m_XCCore ;

	CptString			m_SrcSmallFile ;
	CptString			m_SrcBigFile ;

	pt_STL_vector(CptString)	m_SrcSmallFilesVer ;
	pt_STL_vector(CptString)	m_SrcBigFilesVer ;

	CptString			m_SrcSmallFileWithFolder ;
	CptString			m_SrcBigFileWithFolder ;

	CptString			m_SameHDFolder ;
	CptString			m_DiffHDFolder ;

};

/**/