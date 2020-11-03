using System;
using System.Runtime.InteropServices;

namespace System
{
    class ExtremeCopy
    {
        public delegate int CopyRoutine(int nCmd,int nParam1,int nParam2,string strSrcFile,string strDstFile);

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

        // error process
        public const int ErrorHandlingFlag_Retry = 1 << 1;		// retry 
        public const int ErrorHandlingFlag_Ignore = 1 << 2;	// ignore
        public const int ErrorHandlingFlag_Exit = 1 << 3;		// exit

        // same file name process
        public const int SameFileProcess_Replace = 1;	// replace
        public const int SameFileProcess_Skip = 0;	// skip
        public const int SameFileProcess_Rename = 3;	// rename


        // ExtremeCopy running type
        public const int XCRunType_Copy = 1;	// copy
        public const int XCRunType_Move = 2;	// move


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
        // Return value:
        //			If invoke successful it will reutrn true, otherwise false
        //
        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_StartA", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern bool ExtremeCopy_StartA(int RunType,bool bSyncOrAsyn,CopyRoutine routine);

        [DllImport("ExtremeCopy.dll", EntryPoint = "ExtremeCopy_StartW", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        public static extern bool ExtremeCopy_StartW(int RunType, bool bSyncOrAsyn, CopyRoutine routine);


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
