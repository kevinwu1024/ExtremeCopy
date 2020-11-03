// DebugDLL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../ExtremeCopyApp.h"
#include "ExtremeCopy.h"

static int s_BeginEndLogic = 0 ;

int __stdcall ExtremeCopyRoutine(int nCmd,int nParam1,int nParam2,const TCHAR* pSrcStr,const TCHAR* pDstStr)
{
	int nRet = 0 ;

	//::_tprintf(_T("routine param %d :\r\n"),*(int*)pRoutineParam) ;

	switch(nCmd)
	{
	case ROUTINE_CMD_BEGINONEFILE: //begin to copy one file
		{
			::_tprintf(_T("begin to copy one file :\r\n")) ;

			if(pSrcStr!=NULL)
			{
				::_tprintf(_T("beginning source file : %s \r\n"),pSrcStr) ;
			}

			if(pDstStr!=NULL)
			{
				::_tprintf(_T("beginning destination file : %s \r\n"),pDstStr) ;
			}

			_ASSERT(s_BeginEndLogic==0) ;
			++s_BeginEndLogic ;
		}
		break ;

	case ROUTINE_CMD_FINISHONEFILE: // copy one file ended
		{
			::_tprintf(_T("copy one file ended :\r\n")) ;

			if(pSrcStr!=NULL)
			{
				::_tprintf(_T("ending source file : %s \r\n"),pSrcStr) ;
			}

			if(pDstStr!=NULL)
			{
				::_tprintf(_T("ending destination file : %s \r\n"),pDstStr) ;
			}

			_ASSERT(s_BeginEndLogic==1) ;
			--s_BeginEndLogic ;
		}
		break ;

	case ROUTINE_CMD_FILEFAILED: // file failed 
		{// ExtremeCopy will stop
			::_tprintf(_T("file failed : error code = %d \r\n"),nParam1) ;

			if(pSrcStr!=NULL)
			{
				::_tprintf(_T("failed source file : %s \r\n"),pSrcStr) ;
			}

			if(pDstStr!=NULL)
			{
				::_tprintf(_T("failed destination file : %s \r\n"),pDstStr) ;
			}
		}
		break ;

	case ROUTINE_CMD_SAMEFILENAME: // same file name
		//::_tprintf(_T("same file name : \r\n")) ;

		//if(pSrcStr!=NULL)
		//{
		//	::_tprintf(_T("same name source file : %s \r\n"),pSrcStr) ;
		//}

		//if(pDstStr!=NULL)
		//{
		//	::_tprintf(_T("same name destination file : %s \r\n"),pDstStr) ;
		//}

		//nRet = SameFileProcess_Skip ; // skip
		nRet = SameFileProcess_Replace ; // replace
		break ;

	case ROUTINE_CMD_DATAWROTE: // data wrote into storage
		::_tprintf(_T("data wrote: %d \r\n"),nParam1) ;
		break ;

	case ROUTINE_CMD_TASKFINISH: // finish current copy task
		::_tprintf(_T("task finished !\r\n")) ;
		break ;

	default: break ;
	}

	return nRet ;
}


int _tmain(int argc, _TCHAR* argv[])
{
	//std::list<int> MyList ;

	//MyList.push_back(1) ;
	//MyList.push_back(2) ;
	//MyList.push_back(3) ;
	//MyList.push_back(2) ;
	//MyList.push_back(4) ;
	//MyList.push_back(2) ;

	//std::list<int>::iterator it = MyList.begin() ;
	//std::list<int>::iterator NextIt ;

	////for(;it!=MyList.end();++it)
	//while(it!=MyList.end())
	//{
	//	if((*it)==2)
	//	{
	//		NextIt = it ;
	//		++NextIt ;
	//		MyList.erase(it) ;
	//		it = NextIt ;
	//		continue ;
	//	}
	//	++it ;
	//}

	//int aa = 0 ;
	//return 1 ;

	CExtremeCopyApp app ;

	app.SetLicenseKey(_T("CRME7-MHAWH-U2A2S-PN8HA-XED7Q")) ;
	
	app.SetRoutine(ExtremeCopyRoutine) ;

	{
		//app.AttachSrc(_T("\\\\?\\x:\\New folder")) ;
		////app.AttachSrc(_T("\\\\?\\x:\\test.zip")) ;

		//app.SetDestinationFolder(_T("\\\\?\\m:\\New folder (2)")) ;

		app.AttachSrc(_T("n:\\test2")) ;
		//app.AttachSrc(_T("E:\\Practise_Think\\Code\\OpenSrc\\boost_1_43_0")) ;

		app.SetDestinationFolder(_T("x:\\test")) ;
		//app.SetDestinationFolder(_T("x:\\a")) ;

		app.Start(XCRunType_Move,true) ;
	}

	//{
	//	app.Stop() ;

	//	app.SetRoutine(ExtremeCopyRoutine) ;

	//	app.AttachSrc(_T("\\\\?\\x:\\New folder")) ;

	//	//app.AttachDst(_T("\\\\?\\m:\\New folder (2)")) ;
	//	app.SetDestinationFolder(_T("\\\\?\\m:\\New folder (2)")) ;

	//	//app.AttachSrc(_T("x:\\c")) ;

	//	//app.AttachDst(_T("x:\\b")) ;

	//	app.Start(XCRunType_Copy,false) ;
	//}

	system("pause") ;

	return 0;
}
