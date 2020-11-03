
#pragma once

#include <windows.h>

#pragma comment(lib,"ExtremeCopy.lib")

typedef int (__stdcall *lpfnCopyRoutineA_t)(int nCmd,void* pRoutineParam,int nParam1,int nParam2,const char* pSrcFile,const char* pDstFile) ;
typedef int (__stdcall *lpfnCopyRoutineW_t)(int nCmd,void* pRoutineParam,int nParam1,int nParam2,const WCHAR* pSrcFile,const WCHAR* pDstFile) ;

// ExtremeCopy running state
#define XC_STATE_STOP								1	// ExtremeCopy state: stop
#define XC_STATE_RUNNING							2	// ExtremeCopy state: running
#define XC_STATE_PAUSE								3	// ExtremeCopy state: pause

// command of routine
#define ROUTINE_CMD_BEGINONEFILE					1	// begin to copy one file
#define ROUTINE_CMD_FINISHONEFILE					2	// copy one file ended
#define ROUTINE_CMD_FILEFAILED						3	// file failed 
#define ROUTINE_CMD_SAMEFILENAME					4	// same file name
#define ROUTINE_CMD_DATAWROTE						5	// data wrote into storage
#define ROUTINE_CMD_TASKFINISH						6	// finish current copy task

// error after ExtremeCopy_Start return
#define START_ERROR_CODE_UNKNOWN					9999 // unknown error
#define START_ERROR_CODE_SUCCESS					0 // success
#define START_ERROR_CODE_INVALID_DSTINATION			1 // invalid destination file or folder to be specified
#define START_ERROR_CODE_INVALID_SOURCE				2 // invalid source file or folder to be specified
#define START_ERROR_CODE_INVALID_RECURISE			3 // there is recurise directory relationship between source and destination
#define START_ERROR_CODE_CANOT_LAUNCHTASK			4 // can't launch new copy task in current situation


// same file name process
#define SameFileProcess_Skip						0	// skip
#define SameFileProcess_Replace						1	// replace


// ExtremeCopy running type
#define XCRunType_Copy								1	// copy
#define XCRunType_Move								2	// move

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

	//void __stdcall ExtremeCopy_SetNotEvaluateA(const char* lpstrSN) ;
	//void __stdcall ExtremeCopy_SetNotEvaluateA(const WCHAR* lpstrSN) ;

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
	//			pRoutineParam	-	Be set to pRoutineParam of lpfnCopyRoutine*_t when routine be invoked
	// Return value:
	//			If invoke successful it will reutrn START_ERROR_CODE_SUCCESS , otherwise any error of start error list
	//
	int __stdcall ExtremeCopy_StartA(int RunType,bool bSyncOrAsyn,lpfnCopyRoutineA_t lpfnCopyRoutineA=NULL,void* pRoutineParam=NULL) ;
	int __stdcall ExtremeCopy_StartW(int RunType,bool bSyncOrAsyn,lpfnCopyRoutineW_t lpfnCopyRoutineW=NULL,void* pRoutineParam=NULL) ;


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

#ifdef __cplusplus
}
#endif


#ifdef _UNICODE
#define		ExtremeCopy_Start					ExtremeCopy_StartW
#define		ExtremeCopy_AttachSrc				ExtremeCopy_AttachSrcW 
#define 	ExtremeCopy_SetDestinationFolder	ExtremeCopy_SetDestinationFolderW
#define lpfnCopyRoutine_t			lpfnCopyRoutineW_t 
#else
#define		ExtremeCopy_Start					ExtremeCopy_StartA
#define 	ExtremeCopy_AttachSrc				ExtremeCopy_AttachSrcA 
#define 	ExtremeCopy_SetDestinationFolder	ExtremeCopy_SetDestinationFolderA
#define lpfnCopyRoutine_t			lpfnCopyRoutineA_t 
#endif