
#pragma once 



typedef int (__stdcall *lpfnCopyRoutineA_t)(int nCmd,void* pRutineParam, int nParam1,int nParam2,const char* pSrcStr,const char* pDstStr) ;
typedef int (__stdcall *lpfnCopyRoutineW_t)(int nCmd,void* pRutineParam, int nParam1,int nParam2,const WCHAR* pSrcStr,const WCHAR* pDstStr) ;


#ifdef __cplusplus
extern "C"
{
#endif
	// Function: SetLicenseKey
	//			Set the license key which purchased from easersoft.com, so that no limit to copy task
	// Parameters:  
	//			string of license key which purchased from easersoft.com
	// Return value:
	//			None.
	//
	void __stdcall ExtremeCopy_SetLicenseKeyA(const char* lpstrSN) ;
	void __stdcall ExtremeCopy_SetLicenseKeyW(const WCHAR* lpstrSN) ;
		
	// Function: SetBufferSize
	//			Specify size of buffer for copying. By default, this size is 32 MB
	// Parameters:  
	//			new size of buffer for copying
	// Return value:
	//			Last buffer size will be returned if success, otherwise is -1
	//
	int __stdcall ExtremeCopy_SetBufferSize(int nBufSize) ;


	// Function: ExtremeCopy_Stop
	//			Stop copy work.
	// Parameters:  
	//			None.
	// Return value:
	//			None.
	//
	void __stdcall ExtremeCopy_Stop() ;


	// Function: ExtremeCopy_Pause
	//			Pause copy work if copy work is running.
	// Parameters:  
	//			None.
	// Return value:
	//			If invoke successful it will reutrn true, otherwise false
	//
	bool __stdcall ExtremeCopy_Pause() ;


	// Function: ExtremeCopy_Continue
	//			Pause continue to work if copy work pause.
	// Parameters:  
	//			None.
	// Return value:
	//			If invoke successful it will reutrn true, otherwise false
	//
	bool __stdcall ExtremeCopy_Continue() ;


		// Function: ExtremeCopy_GetState
	//			Get ExtremeCopy DLL running state
	// Parameters:  
	//			None.
	// Return value:
	//			XC_STATE_STOP	:	stop
	//			XC_STATE_RUNNING:	running
	//			XC_STATE_PAUSE	:	pause
	//
	int __stdcall ExtremeCopy_GetState() ; 

	
	// Function: ExtremeCopy_Start
	//			Start to run copy work. Must invoke ExtremeCopy_AttachSrc*() to specify source files and invoke ExtremeCopy_SetDestinationFolder*() to specify destination folder before call this function
	// Parameters:  
	//			RunType			-	ExtremeCopy DLL running type. XCRunType_Copy is copy; XCRunType_Move is move
	//			bSyncOrAsyn		-	synchronous or asynchronous . If set true, ExtremeCopy_Start will not return until copy work finished. If set false, ExtremeCopy_Start will return immediately and
	//								check ROUTINE_CMD_TASKFINISH command for copy work finished in copy routine which set by ExtremeCopy_SetCopyRoutine*()
	//			routine			-	copy routine which will be called back if special event occured 
	// Return value:
	//			If invoke successful it will return START_ERROR_CODE_SUCCESS , otherwise any error of start error list
	//
	int __stdcall ExtremeCopy_StartA(int RunType,bool bSyncOrAsyn,lpfnCopyRoutineA_t lpfnCopyRoutineA,void* pRoutineParam=NULL) ;
	int __stdcall ExtremeCopy_StartW(int RunType,bool bSyncOrAsyn,lpfnCopyRoutineW_t lpfnCopyRoutineW,void* pRoutineParam=NULL) ;


	// Function: ExtremeCopy_AttachSrc
	//			Specify source file or folder for ExtremeCopy DLL . If more than one files or folders need to copy/move, please attach all of them before invoke ExtremeCopy_Start(), 
	//			so that ExtremeCopy DLL to know what about the files and optimize the resource to reach hight speed
	//
	//			ExtremeCopy_AttachSrcA is Ascii verion for string
	//			ExtremeCopy_AttachSrcW is Unicode verion for string
	// Parameters:  
	//			lpstrFile	-	source file/foler which need to copy or move
	// Return value:
	//			If invoke successful it will reutrn count of files and folders in buffer, otherwise buffer will be cleared and return 0
	//
	int __stdcall ExtremeCopy_AttachSrcA(const char* lpstrFile) ;
	int __stdcall ExtremeCopy_AttachSrcW(const WCHAR* lpstrFile) ;

	// Function: ExtremeCopy_SetDestinationFolder
	//			Specify destination folder where need to copy/move to
	//
	//			ExtremeCopy_SetDestinationFolderA is Ascii verion for string
	//			ExtremeCopy_SetDestinationFolderW is Unicode verion for string
	// Parameters:  
	//			lpstrFolder	-	Target folder where ExtremeCopy copy or move files to
	// Return value:
	//			Count of source files/folders be specified in the queue
	//
	bool __stdcall ExtremeCopy_SetDestinationFolderA(const char* lpstrFolder) ;
	bool __stdcall ExtremeCopy_SetDestinationFolderW(const WCHAR* lpstrFolder) ;
	

	// Function: ExtremeCopy_SetCopyRoutine
	//			Specify copy routine which will be called back if special event occured .
	//
	//			ExtremeCopy_SetCopyRoutineA is Ascii verion for string
	//			ExtremeCopy_SetCopyRoutineW is Unicode verion for string
	// Parameters:  
	//			None.
	// Return value:
	//			Count of source files/folders be specified in the queue
	//
	//void __stdcall ExtremeCopy_SetCopyRoutineA(lpfnCopyRoutineA_t) ;
	//void __stdcall ExtremeCopy_SetCopyRoutineW(lpfnCopyRoutineW_t) ;

#ifdef __cplusplus
}
#endif