using System;
using System.Runtime.InteropServices;

namespace System
{
    class ExtremeCopy
    {
        public delegate int CopyRoutine(int nCmd,IntPtr pRoutineParam,int nParam1,int nParam2,string strSrcFile,string strDstFile);

        public const int XC_STATE_STOP = 1;	// ExtremeCopy state: stop
        public const int XC_STATE_RUNNING = 2;// ExtremeCopy state: running
        public const int XC_STATE_PAUSE = 3;	// ExtremeCopy state: pause

        // command of routine
        public const int ROUTINE_CMD_BEGINONEFILE = 1;	// begin to copy one file
        public const int ROUTINE_CMD_FINISHONEFILE = 2;	// copy one file ended
        public const int ROUTINE_CMD_FILEFAILED = 3;	// file failed 
        public const int ROUTINE_CMD_SAMEFILENAME = 4;	// same file name
        public const int ROUTINE_CMD_DATAWROTE = 5;	// data wrote into storage
        public const int ROUTINE_CMD_TASKFINISH = 6;	// finish current copy task

        // same file name process
        public const int SameFileProcess_Replace = 1;	// replace
        public const int SameFileProcess_Skip = 0;	// skip

		// error after ExtremeCopy_Start return
		public const int START_ERROR_CODE_UNKNOWN = 9999 ;// unknown error
		public const int START_ERROR_CODE_SUCCESS = 0 ;// success
		public const int START_ERROR_CODE_INVALID_DSTINATION = 1; // invalid destination file or folder to be specified
		public const int START_ERROR_CODE_INVALID_SOURCE = 2; // invalid source file or folder to be specified
		public const int START_ERROR_CODE_INVALID_RECURISE = 3; // there is recurise directory relationship between source and destination
		public const int START_ERROR_CODE_CANOT_LAUNCHTASK = 4; // can't launch new copy task in current situation

		// size value of copy buffer
		public const int EXTREMECOPY_BUFFER_SIZE_32MB =	32 ;
		public const int EXTREMECOPY_BUFFER_SIZE_16MB =	16 ;
		public const int EXTREMECOPY_BUFFER_SIZE_8MB = 8 ;
		public const int EXTREMECOPY_BUFFER_SIZE_4MB = 4 ;
		public const int EXTREMECOPY_BUFFER_SIZE_2MB = 2 ;

        // ExtremeCopy running type
        public const int XCRunType_Copy = 1;	// copy
        public const int XCRunType_Move = 2;	// move

		// Function: SetLicenseKey
	//			Set the license key which purchased from easersoft.com, so that no limit to copy task
	// Parameters:  
	//			string of license key which purchased from easersoft.com
	// Return value:
	//			None.
	//
		[DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_SetLicenseKeyA", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern void ExtremeCopy_SetLicenseKeyA(string strFile);

        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_SetLicenseKeyW", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        public static extern void ExtremeCopy_SetLicenseKeyW(string strFile);

		// Function: SetBufferSize
		//			Specify size of buffer for copying. By default, this size is 32 MB
		// Parameters:  
		//			new size of buffer for copying
		// Return value:
		//			Last buffer size will be returned if success, otherwise is -1
		//
		[DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_SetBufferSize", CallingConvention = CallingConvention.StdCall)]
		public static extern int ExtremeCopy_SetBufferSize(int nBufSize) ;
	
        // Function: ExtremeCopy_Stop
        //			Stop copy work.
        // Parameters:  
        //			None.
        // Return value:
        //			None.
        //
        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_Stop", CallingConvention = CallingConvention.StdCall)]
        public static extern void ExtremeCopy_Stop();


        // Function: ExtremeCopy_Pause
        //			Pause copy work if copy work is running.
        // Parameters:  
        //			None.
        // Return value:
        //			If invoke successful it will reutrn true, otherwise false
        //
        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_Pause", CallingConvention = CallingConvention.StdCall)]
        public static extern bool ExtremeCopy_Pause();


        // Function: ExtremeCopy_Continue
        //			Continue to run if copy work pause.
        // Parameters:  
        //			None.
        // Return value:
        //			If invoke successful it will reutrn true, otherwise false
        //
        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_Continue", CallingConvention = CallingConvention.StdCall)]
        public static extern bool ExtremeCopy_Continue();


        // Function: ExtremeCopy_Start
        //			Start to run copy work. Must invoke ExtremeCopy_AttachSrc*() to specify source files and invoke ExtremeCopy_SetDestinationFolder*() to specify destination folder before call this function
        // Parameters:  
        //			RunType			-	ExtremeCopy DLL running type. XCRunType_Copy is copy; XCRunType_Move is move
        //			bSyncOrAsyn		-	synchronous or asynchronous . If set true, ExtremeCopy_Start will not return until copy work finished. If set false, ExtremeCopy_Start will return immediately and
        //								check ROUTINE_CMD_TASKFINISH command for copy work finished in copy routine which set by ExtremeCopy_SetCopyRoutine*()
        //          routine         -	copy routine which will be called back if special event occured 
        //			pRoutineParam	-	Be set to pRoutineParam of lpfnCopyRoutine*_t when routine be invoked
        //
        // Return value:
        //			If invoke successful it will reutrn START_ERROR_CODE_SUCCESS , otherwise any error of start error list
        //
        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_StartA", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern int ExtremeCopy_StartA(int RunType,bool bSyncOrAsyn,CopyRoutine routine,IntPtr pRoutineParam);

        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_StartW", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        public static extern int ExtremeCopy_StartW(int RunType, bool bSyncOrAsyn, CopyRoutine routine, IntPtr pRoutineParam);


        // Function: ExtremeCopy_GetState
        //			Get ExtremeCopy DLL running state
        // Parameters:  
        //			None.
        // Return value:
        //			XC_STATE_STOP	:	stop
        //			XC_STATE_RUNNING:	running
        //			XC_STATE_PAUSE	:	pause
        //
        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_GetState", CallingConvention = CallingConvention.StdCall)]
        public static extern int ExtremeCopy_GetState();

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
        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_AttachSrcA", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern int ExtremeCopy_AttachSrcA(string strFile);

        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_AttachSrcW", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        public static extern int ExtremeCopy_AttachSrcW(string strFile);

        // Function: ExtremeCopy_SetDestinationFolder
        //			Specify destination folder where need to copy/move to
        //
        //			ExtremeCopy_SetDestinationFolderA is Ascii verion for string
        //			ExtremeCopy_SetDestinationFolderW is Unicode verion for string
        // Parameters:  
        //			strFolder	-	Target folder where ExtremeCopy copy or move files to
        // Return value:
        //			Count of source files/folders be specified in the queue
        //
        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_SetDestinationFolderA", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern int ExtremeCopy_SetDestinationFolderA(string strFolder);

        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_SetDestinationFolderW", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        public static extern int ExtremeCopy_SetDestinationFolderW(string strFolder);

    }
    
}
