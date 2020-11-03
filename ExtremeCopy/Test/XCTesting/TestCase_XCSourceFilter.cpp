#include "StdAfx.h"
#include "TestCase_XCSourceFilter.h"
//
//CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CTestCase_XCSourceFilter,TEST_SUITE_NAME_CORE);
//
//
//CTestCase_XCSourceFilter::CTestCase_XCSourceFilter(void)
//{
//}
//
//
//CTestCase_XCSourceFilter::~CTestCase_XCSourceFilter(void)
//{
//}
//
//
//void CTestCase_XCSourceFilter::Test_LocalFile() 
//{
//	CXCLocalFileSyncSourceFilter* pSourFiler = new CXCLocalFileSyncSourceFilter(&m_CopyingEvent) ;
//
//	//m_CopyingEvent.SetReceiver(CXCCopyingEvent::ET_CopyFileBegin,this) ;
//	m_CopyingEvent.SetReceiver(CXCCopyingEvent::ET_CopyFileEnd,this) ;
//	m_CopyingEvent.SetReceiver(CXCCopyingEvent::ET_CopyFileDataOccur,this) ;
//	m_CopyingEvent.SetReceiver(CXCCopyingEvent::ET_CopyFileDiscard,this) ;
//	m_CopyingEvent.SetReceiver(CXCCopyingEvent::ET_Exception,this) ;
//	m_CopyingEvent.SetReceiver(CXCCopyingEvent::ET_ImpactFile,this) ;
//	m_CopyingEvent.SetReceiver(CXCCopyingEvent::ET_GetRenamedFileName,this) ;
//
////	m_InputPin.SetCallBack(this,&m_CopyingEvent) ;
//
//	CXCFileDataBuffer FileDataBuf ;
//	SXCSourceFilterTaskInfo sfti ;
//
//	FileDataBuf.AllocateChunk(32*1024*1024,1*1024) ;
//
//	pt_STL_vector(CXCPin*) PinVer ;
//
////	pSourFiler->GetPins(PinVer) ;
//
////	CXCPin::Connect(m_InputPin,*PinVer[0]) ;
//
//	sfti.ExeType = XCTT_Copy ;
//	sfti.pFileDataBuf = &FileDataBuf ;
//	sfti.nValidCachePointNum = 1 ;
//	sfti.SrcFileVer.push_back(::g_pValidDirWithFolder) ;
//
//	pSourFiler->SetTask(sfti) ;	
//	
//	pSourFiler->Run() ;
//
//	delete pSourFiler ;
//}
//
//int CTestCase_XCSourceFilter::OnPin_Data(CXCPin* pOwnerPin,EFilterCmd cmd,void* pFileData) 
//{
//	int nRet = 0 ;
//
//	return nRet ;
//}
//
//int CTestCase_XCSourceFilter::CopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1,void* pParam2) 
//{
//	int nRet = 0 ;
//
//	switch(et)
//	{
//	//case CXCCopyingEvent::ET_CopyFileBegin:
//	//	break ;
//
//	case CXCCopyingEvent::ET_CopyFileEnd:
//		break ;
//
//	case CXCCopyingEvent::ET_CopyFileDataOccur:
//		break ;
//
//	case CXCCopyingEvent::ET_CopyFileDiscard:
//		break ;
//
//	case CXCCopyingEvent::ET_Exception:
//		break ;
//
//	case CXCCopyingEvent::ET_ImpactFile:
//		break ;
//
//	case CXCCopyingEvent::ET_GetRenamedFileName:
//		break ;
//	}
//
//	return nRet ;
//}