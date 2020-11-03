unit ExtremeCopy;

interface
const XC_STATE_STOP     =	  1;	// ExtremeCopy state: stop
const XC_STATE_RUNNING  =	  2;	// ExtremeCopy state: running
const  XC_STATE_PAUSE   =	  3;	// ExtremeCopy state: pause

// command of routine
const ROUTINE_CMD_BEGINONEFILE 	=	  1;	// begin to copy one file
const ROUTINE_CMD_FINISHONEFILE	=	  2;	// copy one file ended
const ROUTINE_CMD_FILEFAILED	  =		3;	// file failed
const ROUTINE_CMD_SAMEFILENAME  =		4;	// same file name
const ROUTINE_CMD_DATAWROTE	    =		5;	// data wrote into storage
const ROUTINE_CMD_TASKFINISH 	  =		6;	// finish current copy task

// size value of copy buffer
const EXTREMECOPY_BUFFER_SIZE_32MB	= 32 ;
const EXTREMECOPY_BUFFER_SIZE_16MB	= 16 ;
const EXTREMECOPY_BUFFER_SIZE_8MB = 8 ;
const EXTREMECOPY_BUFFER_SIZE_4MB = 4 ;
const EXTREMECOPY_BUFFER_SIZE_2MB = 2 ;

// same file name process
const SameFileProcess_Skip	  =		0	;// skip
const SameFileProcess_Replace =		1;	// replace

const XCRunType_Copy  =		1;	// copy
const XCRunType_Move  =		2;	// move

// Function: SetLicenseKey
	//			Set the license key which purchased from easersoft.com, so that no limit to copy task
	// Parameters:  
	//			string of license key which purchased from easersoft.com
	// Return value:
	//			None.
	//
	procedure ExtremeCopy_SetLicenseKeyA(const lpstrSN:pchar) ;stdcall;
	procedure ExtremeCopy_SetLicenseKeyW(const lpstrSN:pwidechar) ;stdcall;
	
	// Function: SetBufferSize
	//			Specify size of buffer for copying. By default, this size is 32 MB
	// Parameters:  
	//			new size of buffer for copying,
	// Return value:
	//			Last buffer size will be returned if success, otherwise is -1
	//
	function ExtremeCopy_SetBufferSize(nBufSize:integer):integer ; stdcall ;
	
// Function: ExtremeCopy_Stop
	//			Stop copy work.
	// Parameters:
	//			None.
	// Return value:
	//			None.
	//
	procedure  ExtremeCopy_Stop() ; stdcall;

  // Function: ExtremeCopy_Pause
	//			Pause copy work if copy work is running.
	// Parameters:
	//			None.
	// Return value:
	//			If invoke successful it will reutrn true, otherwise false
	//
	function ExtremeCopy_Pause():boolean ; stdcall;

  // Function: ExtremeCopy_Continue
	//			Pause continue to work if copy work pause.
	// Parameters:  
	//			None.
	// Return value:
	//			If invoke successful it will reutrn true, otherwise false
	//
	function ExtremeCopy_Continue():boolean ; stdcall;

// Function: ExtremeCopy_Start
	//			Start to run copy work. Must invoke ExtremeCopy_AttachSrc*() to specify source files and invoke ExtremeCopy_SetDestinationFolder*() to specify destination folder before call this function
	// Parameters:  
	//			RunType			-	ExtremeCopy DLL running type. XCRunType_Copy is copy; XCRunType_Move is move
	//			bSyncOrAsyn		-	synchronous or asynchronous . If set true, ExtremeCopy_Start will not return until copy work finished. If set false, ExtremeCopy_Start will return immediately and
	//								check ROUTINE_CMD_TASKFINISH command for copy work finished in copy routine which set by ExtremeCopy_SetCopyRoutine*()
	//			routine			-	copy routine which will be called back if special event occured 
	// Return value:
	//			If invoke successful it will reutrn true, otherwise false
	//
  type
  CopyRoutineA_t= function(nCmd:integer;pRoutineParam:Pointer;nParam1:integer;nParam2:integer;strSrc:pchar;strDst:pchar):integer; stdcall;
  CopyRoutineW_t= function(nCmd:integer;pRoutineParam:Pointer;nParam1:integer;nParam2:integer;strSrc:pwidechar;strDst:pwidechar):integer; stdcall;

  function ExtremeCopy_StartA(RunType:integer; bSyncOrAsyn:boolean;routine:CopyRoutineA_t;pRoutineParam:Pointer):integer ; stdcall;
  function ExtremeCopy_StartW(RunType:integer; bSyncOrAsyn:boolean;routine:CopyRoutineW_t;pRoutineParam:Pointer):integer ; stdcall;

  // Function: ExtremeCopy_GetState
	//			Get ExtremeCopy DLL running state
	// Parameters:
	//			None.
	// Return value:
	//			XC_STATE_STOP	:	stop
	//			XC_STATE_RUNNING:	running
	//			XC_STATE_PAUSE	:	pause
	//
  function ExtremeCopy_GetState() : integer ; stdcall;

// Function: ExtremeCopy_AttachSrc
	//			Specify source file or folder for ExtremeCopy DLL . If more than one files or folders need to copy/move, please attach all of them before invoke ExtremeCopy_Start(), 
	//			so that ExtremeCopy DLL to know what about the files and optimize the resource to reach hight speed
	//
	//			ExtremeCopy_AttachSrcA is Ascii verion for string
	//			ExtremeCopy_AttachSrcW is Unicode verion for string
	// Parameters:  
	//			lpstrFile	-	source file/foler which need to copy or move
	// Return value:
	//			If invoke successful it will reutrn true, otherwise false
	//
 function ExtremeCopy_AttachSrcA(const lpstrFile:AnsiString) : integer ; stdcall;
 function ExtremeCopy_AttachSrcW(const lpstrFile:WideString) : integer ; stdcall;

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
function ExtremeCopy_SetDestinationFolderA(const lpstrFolder:AnsiString) : integer ; stdcall;
function ExtremeCopy_SetDestinationFolderW(const lpstrFolder:WideString) : integer ;  stdcall;


implementation
  	procedure  ExtremeCopy_Stop ;external 'ExtremeCopy.dll';
    function   ExtremeCopy_Pause; external 'ExtremeCopy.dll' ;
    function ExtremeCopy_Continue; external 'ExtremeCopy.dll' ;
    function ExtremeCopy_GetState; external 'ExtremeCopy.dll' ;
	function   ExtremeCopy_SetBufferSize; external 'ExtremeCopy.dll' ;

    function ExtremeCopy_StartA; external 'ExtremeCopy.dll' ;
    function ExtremeCopy_StartW; external 'ExtremeCopy.dll' ;

    function ExtremeCopy_AttachSrcA; external 'ExtremeCopy.dll' ;
    function ExtremeCopy_AttachSrcW; external 'ExtremeCopy.dll' ;

    function ExtremeCopy_SetDestinationFolderA; external 'ExtremeCopy.dll' ;
    function ExtremeCopy_SetDestinationFolderW; external 'ExtremeCopy.dll' ;

    procedure ExtremeCopy_SetCopyRoutineA;external 'ExtremeCopy.dll' ;
    procedure ExtremeCopy_SetCopyRoutinew;external 'ExtremeCopy.dll' ;
	
	procedure ExtremeCopy_SetLicenseKeyA;external 'ExtremeCopy.dll' ;
    procedure ExtremeCopy_SetLicenseKeyW;external 'ExtremeCopy.dll' ;
end.
