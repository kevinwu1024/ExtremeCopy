#pragma once

#include <cppunit/TestCase.h>
#include <cppunit/ui/text/TextTestRunner.h>

#include <cppunit/extensions/HelperMacros.h>
#include "..\..\Core\XCLocalFileSyncSourceFilter.h"
#include "..\..\Core\XCCopyingEvent.h"
//
//class CTestCase_XCSourceFilter : public CppUnit::TestCase,public CXCCopyingEventReceiver//,public CXCPinEventCB
//{
//	CPPUNIT_TEST_SUITE(CTestCase_XCSourceFilter);
//	CPPUNIT_TEST_SUITE_END();
//	
//public:
//	CTestCase_XCSourceFilter(void);
//	virtual ~CTestCase_XCSourceFilter(void);
//
//	void Test_LocalFile() ;
//
//	virtual int CopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1=NULL,void* pParam2=NULL) ;
//	virtual int OnPin_Data(CXCPin* pOwnerPin,EFilterCmd cmd,void* pFileData) ;
//	virtual int OnCopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1=NULL,void* pParam2=NULL) {return 0;}
//	virtual ECopyFileState OnVerifyFileDiff(CptString strSrc,CptString strDst,CXCVerifyResult::EFileDiffType fdt) {return CFS_Stop;}
//
//private:
//	CXCCopyingEvent		m_CopyingEvent ;
//	CXCPin				m_InputPin ;
//};

