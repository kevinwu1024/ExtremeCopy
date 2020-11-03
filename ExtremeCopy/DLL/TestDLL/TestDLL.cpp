/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "stdafx.h"
#include "ExtremeCopyAPI.h"


int __stdcall ExtremeCopyRoutine(int nCmd,void* pRoutineParam,int nParam1,int nParam2,const TCHAR* pSrcFile,const TCHAR* pDstFile) ;

int _tmain(int argc, _TCHAR* argv[])
{
	//::ExtremeCopy_SetLicenseKey(_T("CRME7-MHAWH-U2A2S-PN8HA-XED7Q")) ;
	::ExtremeCopy_SetLicenseKey(_T("afdasfdsafefaafewfasasasssdsad")) ;

	::ExtremeCopy_AttachSrc(_T("\\\\?\\x:\\New folder")) ;
	//::ExtremeCopy_AttachSrc(_T("\\\\?\\x:\\italian_v2.zip")) ;

	::ExtremeCopy_SetDestinationFolder(_T("\\\\?\\x:\\New folder (2)")) ;

	int nResult = ::ExtremeCopy_Start(XCRunType_Copy,true,ExtremeCopyRoutine,NULL) ;


	{
		::ExtremeCopy_Stop() ;
		::ExtremeCopy_AttachSrc(_T("\\\\?\\x:\\dddd")) ;
		//::ExtremeCopy_AttachSrc(_T("\\\\?\\x:\\italian_v2.zip")) ;

		::ExtremeCopy_SetDestinationFolder(_T("\\\\?\\x:\\New folder (2)")) ;

		int nResult = ::ExtremeCopy_Start(XCRunType_Copy,true,ExtremeCopyRoutine,NULL) ;
	}

	::_tprintf(_T("result: %d \r\n"),nResult) ;

	system("pause") ;

	return 0;
}


int __stdcall ExtremeCopyRoutine(int nCmd,void* pRoutineParam,int nParam1,int nParam2,const TCHAR* pSrcStr,const TCHAR* pDstStr) 
{
	int nRet = 0 ;

//	_ASSERT(pRoutineParam!=NULL) ;

	//CECLComparionDemoDlg* pThis = (CECLComparionDemoDlg*)pRoutineParam ;

	switch(nCmd)
	{
	case ROUTINE_CMD_BEGINONEFILE: //begin to copy one file
		{
//			pThis->m_uFileCount++ ;

			//::_tprintf(_T("begin to copy one file :<<<<<<<<<<<<<<<<<<<<<<\r\n")) ;

			//if(pSrcStr!=NULL)
			//{
			//	::_tprintf(_T("beginning source file : %s \r\n"),pSrcStr) ;
			//}

			//if(pDstStr!=NULL)
			//{
			//	::_tprintf(_T("beginning destination file : %s \r\n"),pDstStr) ;
			//}
		}
		break ;

	case ROUTINE_CMD_FINISHONEFILE: // copy one file ended
		{
			//::_tprintf(_T("copy one file ended :>>>>>>>>>>>>>>>>>>>>>>>\r\n")) ;

			//if(pSrcStr!=NULL)
			//{
			//	::_tprintf(_T("ending source file : %s \r\n"),pSrcStr) ;
			//}

			//if(pDstStr!=NULL)
			//{
			//	::_tprintf(_T("ending destination file : %s \r\n"),pDstStr) ;
			//}
		}
		break ;

	case ROUTINE_CMD_FILEFAILED: // file failed 
		{// ExtremeCopy will stop
			//::_tprintf(_T("file failed : error code = %d \r\n"),nParam1) ;

			//if(pSrcStr!=NULL)
			//{
			//	::_tprintf(_T("failed source file : %s \r\n"),pSrcStr) ;
			//}

			//if(pDstStr!=NULL)
			//{
			//	::_tprintf(_T("failed destination file : %s \r\n"),pDstStr) ;
			//}
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

		nRet = SameFileProcess_Replace ; // replace
		break ;

	case ROUTINE_CMD_DATAWROTE: // data wrote into storage
		//::_tprintf(_T("data wrote: %d \r\n"),nParam1) ;
		//pThis->m_uDoneSize += nParam1 ;
		break ;

	case ROUTINE_CMD_TASKFINISH: // finish current copy task
		::_tprintf(_T("task finished !\r\n")) ;
		break ;

	default: break ;
	}

	return nRet ;
}