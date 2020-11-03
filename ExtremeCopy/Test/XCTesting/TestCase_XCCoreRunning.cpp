#include "StdAfx.h"
#include "TestCase_XCCoreRunning.h"


CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CTestCase_XCCoreRunning,TEST_SUITE_NAME_CORE);

CTestCase_XCCoreRunning::CTestCase_XCCoreRunning(void)
{
}


CTestCase_XCCoreRunning::~CTestCase_XCCoreRunning(void)
{
}



void CTestCase_XCCoreRunning::CleanFolder(CptString strFolder) 
{
	CptString strAllFile = strFolder + _T("\\*.*") ;
	WIN32_FIND_DATA wdf ;

	bool bDotFound1 = false ;
	bool bDotFound2 = false ;

	HANDLE hFind = ::FindFirstFile(strAllFile.c_str(),&wdf) ;

	if(hFind!=INVALID_HANDLE_VALUE)
	{
		CptString strFile ;

		do
		{
			if(!bDotFound1 && ::_tcscmp(wdf.cFileName,_T("."))==0)
			{
				bDotFound1 = true ;
				continue ;
			}
			else if(!bDotFound2 && ::_tcscmp(wdf.cFileName,_T(".."))==0)
			{
				bDotFound2 = true ;
				continue ;
			}
			else
			{
				strFile.Format(_T("%s\\%s"),strFolder.c_str(),wdf.cFileName) ;

				if(CptGlobal::IsFolder(wdf.dwFileAttributes))
				{// folder
					//this->RemoveAllFiles(strFile) ;
					CptGlobal::DeleteFolder(strFile) ;
				}
				else
				{
					CptGlobal::ForceDeleteFile(strFile) ;
				}
				
			}
		}
		while(::FindNextFile(hFind,&wdf)) ;

		::FindClose(hFind) ;
	}
}

void CTestCase_XCCoreRunning::Test_CopyFile(pt_STL_vector(CptString) strVer,CptString strDstFolder)
{
	CXCCore core ;
	CXCVerifyResult verify ;
	CXCFileChangingBuffer ChaBuf ;

	SGraphTaskDesc gtd ;

	gtd.ExeType = XCTT_Copy ;
	gtd.pCopyingEvent = &m_CopyingEvent ;
	gtd.SrcFileVer = strVer ;
	gtd.DstFolderVer.push_back(strDstFolder) ;

	core.Run(gtd) ;

	CXCVerifyResult::SVerifyPar param ;

	param.pChangingBuf = &ChaBuf ;
	param.pExceptionCB = this ;
	param.SrcVer = gtd.SrcFileVer ;
//	param.strDstFolder = gtd.DstFolderVer[0] ;

	bool b = verify.Run(param) ;
	
	if(b)
	{
		CptGlobal::CleanFolder(strDstFolder) ;
	}

	CPPUNIT_ASSERT(b) ;
}

void CTestCase_XCCoreRunning::Test_SameHD_CopySingalFile_SmallFile() 
{
	pt_STL_vector(CptString) strVer ;

	strVer.push_back(m_SrcSmallFile) ;

	this->Test_CopyFile(strVer,m_SameHDFolder) ;
	/*CXCCore core ;
	CXCVerifyResult verify ;
	CXCFileChangingBuffer ChaBuf ;

	SGraphTaskDesc gtd ;

	gtd.ExeType = XCTT_Copy ;
	gtd.pCopyingEvent = &m_CopyingEvent ;
	gtd.SrcFileVer.push_back(m_SrcSmallFile) ;
	gtd.DstFolderVer.push_back(m_SameHDFolder) ;

	core.Run(gtd) ;

	CXCVerifyResult::SVerifyPar param ;

	param.pChangingBuf = &ChaBuf ;
	param.pExceptionCB = this ;
	param.SrcVer = gtd.SrcFileVer ;
	param.strDstFolder = gtd.DstFolderVer[0] ;

	bool b = verify.Run(param) ;
	
	if(b)
	{
		CptGlobal::CleanFolder(gtd.DstFolderVer[0]) ;
	}

	CPPUNIT_ASSERT(b) ;*/
}

void CTestCase_XCCoreRunning::Test_SameHD_CopySingalFile_BigFile() 
{
	pt_STL_vector(CptString) strVer ;

	strVer.push_back(m_SrcBigFile) ;

	this->Test_CopyFile(strVer,m_SameHDFolder) ;
}

void CTestCase_XCCoreRunning::Test_SameHD_CopyMultipleFiles_SmallFile() 
{

	this->Test_CopyFile(m_SrcSmallFilesVer,m_SameHDFolder) ;
}

void CTestCase_XCCoreRunning::Test_SameHD_CopyMultipleFiles_BigFile() 
{
	this->Test_CopyFile(m_SrcBigFilesVer,m_SameHDFolder) ;
}

void CTestCase_XCCoreRunning::Test_SameHD_CopyMultipleFiles_SmallFile_WithFolder() 
{
	pt_STL_vector(CptString) strVer ;

	strVer.push_back(m_SrcSmallFileWithFolder) ;

	this->Test_CopyFile(strVer,m_SameHDFolder) ;
}

void CTestCase_XCCoreRunning::Test_SameHD_CopyMultipleFiles_BigFile_WithFolder() 
{
	pt_STL_vector(CptString) strVer ;

	strVer.push_back(m_SrcBigFileWithFolder) ;

	this->Test_CopyFile(strVer,m_SameHDFolder) ;
}

void CTestCase_XCCoreRunning::Test_DiffHD_CopySingalFile_SmallFile() 
{
}

void CTestCase_XCCoreRunning::Test_DiffHD_CopySingalFile_BigFile() 
{
}

void CTestCase_XCCoreRunning::Test_DiffHD_CopyMultipleFiles_SmallFile() 
{
}

void CTestCase_XCCoreRunning::Test_DiffHD_CopyMultipleFiles_BigFile() 
{
}

void CTestCase_XCCoreRunning::Test_DiffHD_CopyMultipleFiles_SmallFile_WithFolder() 
{
}

void CTestCase_XCCoreRunning::Test_DiffHD_CopyMultipleFiles_BigFile_WithFolder() 
{
}

void CTestCase_XCCoreRunning::setUp() 
{
	m_CopyingEvent.SetReceiver(CXCCopyingEvent::ET_Exception,this) ;
	m_CopyingEvent.SetReceiver(CXCCopyingEvent::ET_ImpactFile,this) ;
	
	m_SrcBigFile = _T("E:\\test_copy\\SourceFiles\\1.big") ;

	m_SrcSmallFile = _T("E:\\test_copy\\SourceFiles\\Small\\Folder1\\dog.jpg") ;
	//m_SrcSmallFile = _T("E:\\test_copy\\SmallFile\\Folder1\\zero.txt") ;
	//
	m_SrcSmallFilesVer.push_back(m_SrcSmallFile) ;
	m_SrcSmallFilesVer.push_back(_T("E:\\test_copy\\SourceFiles\\Small\\Folder1\\zero.txt")) ;
	m_SrcSmallFilesVer.push_back(_T("E:\\test_copy\\SourceFiles\\Small\\Folder1\\这是个中文名的文件.txt")) ;

	m_SrcSmallFileWithFolder = _T("E:\\test_copy\\SourceFiles\\Small") ;

	m_SrcBigFileWithFolder = _T("E:\\test_copy\\SourceFiles") ;

	m_DiffHDFolder = _T("G:\\test_copy") ;
	m_SameHDFolder = _T("F:\\test_copy") ;
}

void CTestCase_XCCoreRunning::tearDown() 
{
}

int CTestCase_XCCoreRunning::CopyingEvent_Execute(CXCCopyingEvent::EEventType et,void* pParam1,void* pParam2) 
{
	switch(et)
	{
	case CXCCopyingEvent::ET_Exception:
		break ;

	case CXCCopyingEvent::ET_ImpactFile:
		{
			EImpactFileBehaviorResult& Result = *(EImpactFileBehaviorResult*)pParam2 ;

			Result = SFDB_Replace ;
		}
		break ;
	}

	return 0 ;
}


ECopyFileState CTestCase_XCCoreRunning::OnFileDiff(CptString strSrc,CptString strDst,CXCVerifyResult::EFileDiffType fdt)
{
	switch(fdt)
	{
	case CXCVerifyResult::FDT_TimeDiff: 
		CPPUNIT_ASSERT_EQUAL_MESSAGE("FDT_TimeDiff",0,1) ;break ;
	case CXCVerifyResult::FDT_AttrDiff: CPPUNIT_ASSERT_EQUAL_MESSAGE("FDT_AttrDiff",0,1) ; break ;
	case CXCVerifyResult::FDT_ContentDiff: CPPUNIT_ASSERT_EQUAL_MESSAGE("FDT_ContentDiff",0,1) ; break ;
	case CXCVerifyResult::FDT_DstFileInexist: CPPUNIT_ASSERT_EQUAL_MESSAGE("FDT_DstFileInexist",0,1) ; break ;
	case CXCVerifyResult::FDT_SrcFileInexist: CPPUNIT_ASSERT_EQUAL_MESSAGE("FDT_SrcFileInexist",0,1) ; break ;
	default:
	case CXCVerifyResult::FDT_Same: CPPUNIT_ASSERT_EQUAL_MESSAGE("FDT_Same",0,1) ; break ;

	}

	return CFS_Running ;
}