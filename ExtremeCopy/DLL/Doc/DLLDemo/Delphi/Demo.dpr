program Demo;

{$APPTYPE CONSOLE}

uses
  SysUtils,
  ExtremeCopy in 'ExtremeCopy.pas';

function  ExtremeCopyRoutine(nCmd:integer;pRoutineParam:Pointer;nParam1:integer;nParam2:integer;strSrc:pchar;strDst:pchar) : integer; stdcall;
var
  nRet : integer ;
begin
  nRet := 0 ;

  case(nCmd) of

	ROUTINE_CMD_BEGINONEFILE: //begin to copy one file
		begin
			Writeln('begin to copy one file :') ;

		 	if(strSrc<>nil) then
			begin
				Writeln('beginning source file : %s ',strSrc) ;
			end;

			if(strDst<>nil) then
			begin
				Writeln('beginning destination file : ',strDst) ;
			end;
		end;


	ROUTINE_CMD_FINISHONEFILE: // copy one file ended
		begin
			Writeln('copy one file ended :') ;

		if(strSrc<>nil) then
			begin
				Writeln('ending source file : ',strSrc) ;
			end;

			if(strDst<>nil) then
			begin
				Writeln('ending destination file : ',strDst) ;
			end;
		end;


	ROUTINE_CMD_FILEFAILED: // file failed
		begin// ExtremeCopy will stop
			Writeln('file failed : error code = ',nParam1) ;

			if(strSrc<>nil) then
			begin
				Writeln('failed source file : ',strSrc) ;
			end;

			if(strDst<>nil) then
			begin
				Writeln('failed destination file : ',strDst) ;
			end;

		end;


	ROUTINE_CMD_SAMEFILENAME: // same file name
    begin
			Writeln('same file name : ') ;

			if(strSrc<>nil) then
			begin
				Writeln('same name source file : ',strSrc) ;
			end;

			if(strDst<>nil) then
			begin
				Writeln('same name destination file : ',strDst) ;
			end;

        nRet := SameFileProcess_Skip ; // skip
    end;

	ROUTINE_CMD_DATAWROTE: // data wrote into storage
    begin
      Writeln('data wrote: ',nParam1) ;
    end;

	ROUTINE_CMD_TASKFINISH: // finish current copy task
    begin
		  Writeln('task finished !') ;
		end;

	end;


  result := nRet ;
end;

var
  nTem : string ;
begin

	ExtremeCopy_AttachSrcA('c:\my.mp3') ;// specify source file
	ExtremeCopy_AttachSrcA('c:\Image') ;// specify source folder

	ExtremeCopy_SetDestinationFolderA('E:\') ;// specify destination folder

	ExtremeCopy_StartA(XCRunType_Copy,true,ExtremeCopyRoutine,nil) ;// start to run copy work

  readln( nTem) ;
  { TODO -oUser -cConsole Main : Insert code here }
end.


