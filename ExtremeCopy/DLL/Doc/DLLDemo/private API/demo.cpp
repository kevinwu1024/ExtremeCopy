// demo.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include "ExtremeCopy.h"

int __stdcall ExtremeCopyRoutine(int nCmd,int nParam1,int nParam2,const TCHAR* pSrcStr,const TCHAR* pDstStr) ;

int _tmain(int argc, _TCHAR* argv[])
{
	ExtremeCopy_AttachSrc(_T("c:\\my.mp3")) ;// specify source file
	ExtremeCopy_AttachSrc(_T("c:\\Image")) ;// specify source folder

	ExtremeCopy_SetDestinationFolder(_T("E:\\destination-folder")) ;// specify destination folder

	ExtremeCopy_Start(XCRunType_Copy,true) ;// start to run copy work

	return 0; 
}

int __stdcall ExtremeCopyRoutine(int nCmd,int nParam1,int nParam2,const TCHAR* pSrcStr,const TCHAR* pDstStr)
{
	int nRet = 0 ;

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
		}
		break ;

	case ROUTINE_CMD_FILEFAILED: // file failed 
		{
			::_tprintf(_T("file failed : error code = %d \r\n"),nParam1) ;

			if(pSrcStr!=NULL)
			{
				::_tprintf(_T("failed source file : %s \r\n"),pSrcStr) ;
			}

			if(pDstStr!=NULL)
			{
				::_tprintf(_T("failed destination file : %s \r\n"),pDstStr) ;
			}

			nRet = ErrorHandlingFlag_Ignore ; // ignore
		}
		break ;

	case ROUTINE_CMD_SAMEFILENAME: // same file name
			::_tprintf(_T("same file name : \r\n")) ;

			if(pSrcStr!=NULL)
			{
				::_tprintf(_T("same name source file : %s \r\n"),pSrcStr) ;
			}

			if(pDstStr!=NULL)
			{
				::_tprintf(_T("same name destination file : %s \r\n"),pDstStr) ;
			}

			nRet = SameFileProcess_Rename ; // rename
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

