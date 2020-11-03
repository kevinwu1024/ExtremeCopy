/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#include "stdafx.h"
#include "XCopyVISTA.h"

#include "shellapi.h"

#include "../ExtremeCopy/Common/ptString.h"
#include "../XCShellExt/XCShellExt.h"

#include "../ExtremeCopy/Common/ptGlobal.h"

#pragma warning(disable:4996)

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
//TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
//TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

//const TCHAR*	s_pTitle = _T("XCopyVISTATransWnd") ;
//const TCHAR*	s_pWndClass = _T("XCopyVISTAWndCls") ;

#define		WM_XCVISTA		WM_USER+100

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);



//bool IsVISTAOrLaterVersion()
//{
//	bool bRet = false ;
//
//	OSVERSIONINFO oi ;
//
//	::memset(&oi,0,sizeof(OSVERSIONINFO)) ;
//
//	oi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO) ;
//
//	if(::GetVersionEx(&oi))
//	{
//		bRet = oi.dwMajorVersion>=6 ;
//	}
//
//	return bRet ;
//}

void LaunchExtremeExe(const TCHAR* pArg)
{
	TCHAR szFileName[MAX_PATH_EX] = {0} ;

	TCHAR szXCopyFileBuf[MAX_PATH_EX] = {0} ;
	TCHAR* pExeFileNamle = NULL ;

	HMODULE hModule = ::GetModuleHandle(NULL) ;

	::GetModuleFileName(hModule,szFileName,sizeof(szFileName)/sizeof(TCHAR)) ;

	::GetFullPathName(szFileName,sizeof(szXCopyFileBuf)/sizeof(TCHAR),szXCopyFileBuf,&pExeFileNamle) ;
	::_tcscpy(pExeFileNamle,_T("")) ;

	//::_tcscpy(pExeFileNamle,_T("")) ;

	if(CptGlobal::IsVISTAOrLaterVersion())
	{
		::ShellExecute(NULL,_T("runas"),_T("ExtremeCopy.exe"),pArg,szXCopyFileBuf,SW_SHOW) ;
	}
	else
	{
		::ShellExecute(NULL,_T("open"),_T("ExtremeCopy.exe"),pArg,szXCopyFileBuf,SW_SHOW) ;
	}
}

void LaunchExtremeCopy()
{
	//MessageBox(NULL,_T("LaunchExtremeCopy() 1"),_T(""),0) ;

	TCHAR szFileName[MAX_PATH_EX] = {0} ;

	TCHAR szXCopyFileBuf[MAX_PATH_EX] = {0} ;
	TCHAR* pExeFileNamle = NULL ;

	HMODULE hModule = ::GetModuleHandle(NULL) ;

	::GetModuleFileName(hModule,szFileName,sizeof(szFileName)/sizeof(TCHAR)) ;

	::GetFullPathName(szFileName,sizeof(szXCopyFileBuf)/sizeof(TCHAR),szXCopyFileBuf,&pExeFileNamle) ;
	::_tcscpy(pExeFileNamle,_T("")) ;

	//if(IsFileExist(szXCopyFileBuf))
	{
		//MessageBox(NULL,szXCopyFileBuf,_T(""),0) ;

		::_tcscpy(pExeFileNamle,_T("")) ;
		::ShellExecute(NULL,_T("open"),_T("ExtremeCopy.exe"),_T("-shell"),szXCopyFileBuf,SW_SHOW) ;
	}
}

void LaunchHelper(LPCTSTR  lpCmdLine)
{
	bool bLegal = (::_tcscmp(lpCmdLine,_T("-register"))==0 || ::_tcscmp(lpCmdLine,_T("-config"))==0) ;

	if(bLegal)
	{
		::LaunchExtremeExe(lpCmdLine) ;
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//MessageBox(NULL,lpCmdLine,_T("main() 1"),0) ;

	::LaunchHelper(lpCmdLine) ;

	HANDLE hMutex= ::CreateMutex(NULL,TRUE, _T("XCopyVISTASingle")); 

	if(hMutex!=NULL && ::GetLastError()!=ERROR_ALREADY_EXISTS && ::_tcscmp(lpCmdLine,_T("-forvista"))==0)
	//if(::_tcscmp(lpCmdLine,_T("-forvista"))==0)
	{
		//MessageBox(NULL,_T("main() 2"),_T(""),0) ;

		BYTE  sd[SECURITY_DESCRIPTOR_MIN_LENGTH];
		SECURITY_ATTRIBUTES  sa;

		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = &sd;

		::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
		::SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

		HANDLE hEvent = ::CreateEvent(&sa,FALSE,FALSE,_T("XCEventCopyForUAC")) ;

		do
		{
			::LaunchExtremeCopy() ;
		}
		while(::WaitForSingleObject(hEvent,INFINITE)==WAIT_OBJECT_0);
	}

	//MessageBox(NULL,_T("main() 3"),_T(""),0) ;
	//if(::_tcscmp(lpCmdLine,_T("-forvista")))
	//{
	//	return 0 ;
	//}
/**

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	//LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//::_tcscpy(szTitle,_T("XCopyVISTATransWnd")) ;
	//::_tcscpy(szWindowClass,_T("XCopyVISTAWndCls")) ;

	//LoadString(hInstance, IDC_XCOPYVISTA, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

//	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_XCOPYVISTA));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
	/**/
}

/**

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_XCOPYVISTA));
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_XCOPYVISTA);
	wcex.lpszClassName	= s_pWndClass;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   //hWnd = ::CreateWindowEx(WS_EX_TOOLWINDOW,s_pWndClass, s_pTitle, WS_MINIMIZE|WS_POPUPWINDOW,
	  // CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL,NULL, hInstance, NULL) ;
   hWnd = CreateWindow(s_pWndClass, s_pTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//



bool LaunchExtremeCopy(TCHAR* pCmd)
{
	bool bRet = false ;

	//::MessageBox(NULL,_T("LaunchExtremeCopy() begin"),_T(""),0) ;

	if(pCmd==NULL)
	{
		return false ;
	}

	TCHAR szFileName[MAX_PATH_EX] = {0} ;

	TCHAR szXCopyFileBuf[MAX_PATH_EX] = {0} ;
	TCHAR* pExeFileNamle = NULL ;

	HMODULE hModule = ::GetModuleHandle(NULL) ;

	::GetModuleFileName(hModule,szFileName,sizeof(szFileName)/sizeof(TCHAR)) ;

	::GetFullPathName(szFileName,sizeof(szXCopyFileBuf)/sizeof(TCHAR),szXCopyFileBuf,&pExeFileNamle) ;
	::_tcscpy(pExeFileNamle,_T("ExtremeCopy.exe")) ;

	//::MessageBox(NULL,_T("LaunchExtremeCopy() 1"),_T(""),0) ;

	if(IsFileExist(szXCopyFileBuf))
	{
		//::MessageBox(NULL,_T("LaunchExtremeCopy() 2"),_T(""),0) ;

		int nSize = (::_tcslen(pCmd))*sizeof(TCHAR) ;

		HANDLE hWrite = INVALID_HANDLE_VALUE ;
		HANDLE hRead = INVALID_HANDLE_VALUE ;
		SECURITY_ATTRIBUTES sa ;

		sa.nLength = sizeof(SECURITY_ATTRIBUTES) ;
		sa.bInheritHandle = TRUE ;
		sa.lpSecurityDescriptor = NULL ;

		::CreatePipe(&hRead,&hWrite,&sa,nSize) ;

		PROCESS_INFORMATION pi ;
		STARTUPINFO si ;

		::memset(&si,0,sizeof(si)) ;
		si.cb = sizeof(si) ;
		si.dwFlags = STARTF_USESTDHANDLES ;
		si.hStdInput = hRead ;
		si.hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE) ;
		si.hStdError = ::GetStdHandle(STD_ERROR_HANDLE) ;

		if(::CreateProcess(szXCopyFileBuf,_T(" -shell"),NULL,NULL,TRUE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&si,&pi))
		{
			DWORD dwWritten = 0 ;

			bRet = ::WriteFile(hWrite,(void*)pCmd,nSize,&dwWritten,NULL) ? true : false ;

			::MessageBox(NULL,_T("LaunchExtremeCopy() 3"),_T(""),0) ;

			//if(bRet && bMoveOrCopy && ::OpenClipboard(NULL))
			//{// ÈôÎª¼ôÇÐ,ÔòÇå¿Õ¼ôÌù°å
			//	::EmptyClipboard() ; 
			//	::CloseClipboard() ;
			//}
		}
		else
		{
			::CloseHandle(hRead) ;
		}

		::CloseHandle(hWrite) ;
	}

	//::MessageBox(NULL,_T("LaunchExtremeCopy() end"),_T(""),0) ;

	return bRet ;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		//case IDM_ABOUT:
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		//case IDM_EXIT:
		//	DestroyWindow(hWnd);
		//	break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_COPYDATA:
		{
			//::MessageBox(NULL,_T("recive copy_data message"),_T(""),0) ;

			if(lParam)
			{
				//::MessageBox(NULL,_T("recive copy_data message 2"),_T(""),0) ;

				COPYDATASTRUCT* pCS = (COPYDATASTRUCT*)lParam ;

				return LaunchExtremeCopy((TCHAR*)pCS->lpData) ? TRUE : FALSE ;
			}
			
			//::MessageBox(NULL,_T("aaaaaa"),_T(""),0) ;
			//int aa = 0 ;
		}
		break ;

	case WM_XCVISTA:
		::MessageBox(NULL,_T("aaaaaa"),_T(""),0) ;
		break ;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/**
// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
/**/
