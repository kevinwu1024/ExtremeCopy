using System;

namespace demo
{
    class Program
    {
        static void Main(string[] args)
        {

            ExtremeCopy.ExtremeCopy_AttachSrcW("c:\\my.mp3"); // specify source file
            ExtremeCopy.ExtremeCopy_AttachSrcW("c:\\Image"); // specify source folder

            ExtremeCopy.ExtremeCopy_SetDestinationFolderW("E:\\destination-folder"); // specify destination folder

            ExtremeCopy.ExtremeCopy_StartW(ExtremeCopy.XCRunType_Copy, true, ExtremeCopyRoutine); // start to run copy work
            
        }

        static int ExtremeCopyRoutine(int nCmd, int nParam1, int nParam2, string strSrcFile, string strDstFile)
        {
            int nRet = 0;

            switch (nCmd)
            {
                case ExtremeCopy.ROUTINE_CMD_BEGINONEFILE: //begin to copy one file
                    {
                        System.Console.WriteLine("begin to copy one file :\r\n");

                        if (strSrcFile != null)
                        {
                            System.Console.WriteLine("beginning source file : {0} \r\n", strSrcFile);
                        }

                        if (strDstFile != null)
                        {
                            System.Console.WriteLine("beginning destination file : {0} \r\n", strDstFile);
                        }
                    }
                    break;

                case ExtremeCopy.ROUTINE_CMD_FINISHONEFILE: // copy one file ended
                    {
                        System.Console.WriteLine("copy one file ended  :\r\n");

                        if (strSrcFile != null)
                        {
                            System.Console.WriteLine("ending source file : {0} \r\n", strSrcFile);
                        }

                        if (strDstFile != null)
                        {
                            System.Console.WriteLine("ending destination file : {0} \r\n", strDstFile);
                        }
                    }
                    break;

                case ExtremeCopy.ROUTINE_CMD_FILEFAILED: // file failed 
                    {
                        System.Console.WriteLine("file failed : error code = {0} \r\n", nParam1);

                        if (strSrcFile != null)
                        {
                            System.Console.WriteLine("failed source file : {0} \r\n", strSrcFile);
                        }

                        if (strDstFile != null)
                        {
                            System.Console.WriteLine("failed destination file : {0} \r\n", strDstFile);
                        }

                        nRet = ExtremeCopy.ErrorHandlingFlag_Ignore; // ignore
                    }
                    break;

                case ExtremeCopy.ROUTINE_CMD_SAMEFILENAME: // same file name
                    System.Console.WriteLine("same file name : \r\n");

                    if (strSrcFile != null)
                    {
                        System.Console.WriteLine("same name source file : {0} \r\n", strSrcFile);
                    }

                    if (strDstFile != null)
                    {
                        System.Console.WriteLine("same name destination file : {0} \r\n", strDstFile);
                    }

                    nRet = ExtremeCopy.SameFileProcess_Rename; // rename
                    break;

                case ExtremeCopy.ROUTINE_CMD_DATAWROTE: // data wrote into storage
                    System.Console.WriteLine("data wrote: {0} \r\n", nParam1);
                    break;

                case ExtremeCopy.ROUTINE_CMD_TASKFINISH: // finish current copy task
                    System.Console.WriteLine("task finished !\r\n");
                    break;

                default: break;
            }

            return nRet;
        }
    }
}
