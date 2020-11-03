/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include <Windows.h>
#include "ptTypeDef.h"
#include <process.h>
#include <Tlhelp32.h>
#include "ptRegistry.h"
#include "ptString.h"
#include <bitset>
#include <shlobj.h> 
#include <shobjidl.h>
#include <Wininet.h>
#include <ShlGuid.h>


#pragma once

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

class CptGlobal
{
public:

#define VAR_NAME_TO_STRING(var) #var

#define ALIGN_SIZE_UP(v,s) (((v)%(s))?((v)+(s)-(v)%(s)):(v)) // 数值向上(大值)对齐 v: 要对齐的数值， s:要对齐的粒值
#define ALIGN_SIZE_DOWN(v,s) ((v)-(v)%(s)) // 数值向下(小值)对齐

#define SAFE_DELETE_GDI(object) {if((object)!=NULL) {::DeleteObject((object));(object)=NULL;}}
#define SAFE_DELETE_MEMORY(p)	{if((p)!=NULL){delete (p);(p)=NULL;}}
#define SAFE_DELETE_MEMORY_ARRAY(pArray) {if((pArray)!=NULL){delete [] (pArray); (pArray)=NULL;}}

#define IsFileExist(lpFileFuleName) (::GetFileAttributes(lpFileFuleName)!=INVALID_FILE_ATTRIBUTES) 

	//static void Int64ToDoubleWord(const unsigned __int64& nInt64,DWORD& dwHi,DWORD& dwLow)
	//{
	//	dwHi = (DWORD)(nInt64>>32) ;
	//	dwLow = (DWORD)nInt64 ;
	//}

	static inline void SetButtonCheck(HWND hParentWnd,UINT id,bool bCheck)
	{
		::SendMessage(::GetDlgItem(hParentWnd,id),BM_SETCHECK,bCheck?BST_CHECKED:BST_UNCHECKED,0) ;
	}

	static inline void SetButtonCheck(HWND hButtonWnd,bool bCheck)
	{
		::SendMessage(hButtonWnd,BM_SETCHECK,bCheck?BST_CHECKED:BST_UNCHECKED,0) ;
	}

	static inline bool GetButtonCheck(HWND hParentWnd,UINT id)
	{
		return (::SendMessage(::GetDlgItem(hParentWnd,id),BM_GETCHECK,0,0)==BST_CHECKED) ;
	}

	static inline bool GetButtonCheck(HWND hButtonWnd)
	{
		return (::SendMessage(hButtonWnd,BM_GETCHECK,0,0)==BST_CHECKED) ;
	}

	static bool IsRecycleFolder(const TCHAR* lpszFolder)
	{
		bool ret = false ;

		HRESULT hr;
		SHDESCRIPTIONID  pdid  ;
		LPITEMIDLIST pidl = NULL;

		IShellFolder *pisfRecBin = NULL;

		// 获取“根”目录，桌面
		SHGetDesktopFolder(&pisfRecBin);

		hr = pisfRecBin->ParseDisplayName(NULL,NULL,(TCHAR*)lpszFolder,NULL,&pidl, NULL) ;

		if (SUCCEEDED(hr))
		{
			IShellFolder *psf = NULL;
			LPCITEMIDLIST pidlChild = NULL;

			if (SUCCEEDED(hr = SHBindToParent(pidl, IID_IShellFolder, (void**)&psf, &pidlChild)))
			{
				hr = ::SHGetDataFromIDList(psf, pidlChild, SHGDFIL_DESCRIPTIONID, &pdid, sizeof(pdid));

				ret = (SUCCEEDED(hr) && (pdid.clsid == CLSID_RecycleBin));

				psf->Release();
			}

			::CoTaskMemFree(pidl);
		}

		return ret ;
	}

	static bool GetRecycleFileOriginName(const TCHAR* strRecycleFile,CptString& strOriginFile)
{
	bool bRet = false ;

	HRESULT hr;
	LPITEMIDLIST pidl = NULL;

	IShellFolder *pisfRecBin = NULL;

	// 获取“根”目录，桌面
	SHGetDesktopFolder(&pisfRecBin);

	hr = pisfRecBin->ParseDisplayName(NULL,NULL,(TCHAR*)strRecycleFile,NULL,&pidl, NULL) ;

	if (SUCCEEDED(hr)) 
	{
		IShellFolder *psf = NULL;
		LPCITEMIDLIST pidlChild = NULL;

		if (SUCCEEDED(hr = SHBindToParent(pidl, IID_IShellFolder,(void**)&psf, &pidlChild))) 
		{
				STRRET strret;
				if(SUCCEEDED(psf->GetDisplayNameOf(pidlChild,SHGDN_NORMAL,&strret)))
				{
					if(::_tcsstr(strret.pOleStr,_T(":\\"))!=NULL)
					{
						strOriginFile = strret.pOleStr ;
						//::_tcscpy(strOriginFile,strret.pOleStr) ;
						bRet = true ;
					}

					::CoTaskMemFree(strret.pOleStr) ;
				}

			psf->Release();
		}

		::CoTaskMemFree(pidl);
	}

	return bRet ;
}

	static void CleanFolder(CptString strFolder) 
	{
		CptString strAllFile = strFolder + _T("\\*.*") ;
		WIN32_FIND_DATA wdf ;

		bool bDotFound1 = false ;
		bool bDotFound2 = false ;

		HANDLE hFind = ::FindFirstFile(strAllFile.c_str(),&wdf) ;

		if(hFind!=INVALID_HANDLE_VALUE)
		{
			CptString strFile ;

			do
			{
				if(!bDotFound1 && ::_tcscmp(wdf.cFileName,_T("."))==0)
				{
					bDotFound1 = true ;
					continue ;
				}
				else if(!bDotFound2 && ::_tcscmp(wdf.cFileName,_T(".."))==0)
				{
					bDotFound2 = true ;
					continue ;
				}
				else
				{
					strFile.Format(_T("%s\\%s"),strFolder.c_str(),wdf.cFileName) ;

					if(CptGlobal::IsFolder(wdf.dwFileAttributes))
					{// folder
						//this->RemoveAllFiles(strFile) ;
						CptGlobal::DeleteFolder(strFile) ;
					}
					else
					{
						CptGlobal::ForceDeleteFile(strFile) ;
					}

				}
			}
			while(::FindNextFile(hFind,&wdf)) ;
			::FindClose(hFind) ;
		}
	}

	static void DeleteFolder(CptString strFolder) 
	{
		CptString strAllFile = strFolder + _T("\\*.*") ;
		WIN32_FIND_DATA wdf ;

		bool bDotFound1 = false ;
		bool bDotFound2 = false ;

		HANDLE hFind = ::FindFirstFile(strAllFile.c_str(),&wdf) ;

		if(hFind!=INVALID_HANDLE_VALUE)
		{
			CptString strFile ;

			do
			{
				if(!bDotFound1 && ::_tcscmp(wdf.cFileName,_T("."))==0)
				{
					bDotFound1 = true ;
					continue ;
				}
				else if(!bDotFound2 && ::_tcscmp(wdf.cFileName,_T(".."))==0)
				{
					bDotFound2 = true ;
					continue ;
				}
				else
				{
					strFile.Format(_T("%s\\%s"),strFolder.c_str(),wdf.cFileName) ;

					if(CptGlobal::IsFolder(wdf.dwFileAttributes))
					{// folder
						DeleteFolder(strFile) ;
					}

					CptGlobal::ForceDeleteFile(strFile) ;
				}
			}
			while(::FindNextFile(hFind,&wdf)) ;

			::FindClose(hFind) ;

			::RemoveDirectory(strFolder.c_str()) ;
		//ForceDeleteFile(strFolder.c_str()) ;
		}
	}


	static bool ForceDeleteFile(LPCTSTR lpFile)
	{
		bool bRet = false ;

		if(!::DeleteFile(lpFile))
		{
			if(::SetFileAttributes(lpFile,FILE_ATTRIBUTE_NORMAL))
			{
				bRet = ::DeleteFile(lpFile) ? true : false ;
			}
		}
		else
		{
			bRet = true ;
		}

		return bRet ;
	}

	static bool IsFolder(const TCHAR* lpDir)
	{
		_ASSERT(lpDir!=NULL) ;

		bool bRet = false ;

		if(lpDir!=NULL)
		{
			const DWORD dwAttr = ::GetFileAttributes(lpDir) ;

			bRet = (dwAttr!=INVALID_FILE_ATTRIBUTES && dwAttr&FILE_ATTRIBUTE_DIRECTORY) ;
		}

		return bRet ;
	}

	static inline bool IsFolder(const DWORD dwAttr)
	{
		return (dwAttr!=INVALID_FILE_ATTRIBUTES && dwAttr&FILE_ATTRIBUTE_DIRECTORY) ;
	}

	//static bool IsFolderWithWildcard(const TCHAR* lpDir)
	//{
	//	_ASSERT(lpDir!=NULL) ;

	//	bool bRet = false ;

	//	if(lpDir!=NULL)
	//	{
	//		const DWORD dwAttr = ::GetFileAttributes(lpDir) ;

	//		if(dwAttr==INVALID_FILE_ATTRIBUTES)
	//		{
	//		}
	//	}

	//	return bRet ;
	//}

	static bool CreateGUID(GUID& guid)
	{
		::CoInitialize(NULL);

		bool bRet = (S_OK == ::CoCreateGuid(&guid)) ;
		::CoUninitialize();

		return bRet ;
	}
	static unsigned __int64 DoubleWordTo64( const DWORD dwLow, const DWORD dwHight)
	{
		unsigned __int64 dwRet = 0;

		if(dwHight>0)
		{
			dwRet = ((unsigned __int64)dwHight)<<32 ;
		}

		dwRet = dwRet + (unsigned __int64)dwLow ;

		return dwRet ;
	}

	static bool GetBitmapSize(HBITMAP hBitmap,SptSize& size)
	{
		bool bRet = false ;

		if(hBitmap!=NULL)
		{
			BITMAP bm ;

			::memset(&bm,0,sizeof(bm)) ;
			if(::GetObject(hBitmap,sizeof(bm),&bm))
			{
				size.nWidth = bm.bmWidth ;
				size.nHeight = bm.bmHeight ;

				bRet = true ;
			}
		}

		return bRet ;

	}

	static CptString FormatCommaNumber(int nValue)
	{
		CptString strRet ;

		if(nValue>=1000)
		{
			strRet.Format(_T("%d"),nValue) ;

			int nCommaCount = strRet.GetLength()/3 ;

			int nOffset = 3 ;
			while(nCommaCount>0)
			{
				strRet.Insert(strRet.GetLength()-nOffset,',') ;
				--nCommaCount ;
				nOffset += 4 ;
			}
		}
		else
		{
			strRet.Format(_T("%d"),nValue) ;
		}

		return strRet ;
	}

	static void Int64ToDoubleWord(const unsigned __int64 nInt64,DWORD& dwHi,DWORD& dwLow)
	{
		dwHi = (DWORD)(nInt64>>32) ;
		dwLow = (DWORD)nInt64 ;
	}

	// 用法： CreateLink("c:\\a.txt",  "d:\\a.txt.lnk");
	static bool CreateShortcutLink(LPCWSTR szPath,LPCWSTR szLink)
	{
		::CoInitialize(NULL);

		HRESULT hres;

		IShellLink* psl;
		IPersistFile* ppf;
		//WORD wsz[MAX_PATH];

		hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,IID_IShellLinkW, (void**)&psl);

		if(FAILED(hres))
		{
			return false;
		}
		
		psl->SetPath(szPath);

		hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);

		if(FAILED(hres))
		{
			return false;
		}

		//::MultiByteToWideChar(CP_ACP, 0, szLink, -1, wsz, MAX_PATH);

		hres = ppf->Save(szLink, STGM_READWRITE);

		ppf->Release();

		psl->Release();

		::CoUninitialize();

		return TRUE;
	}

	static bool DownloadFile(CptString strURLFile,CptString strFolder)
	{
		bool bRet = false ;

		HINTERNET hInetSession = ::InternetOpen(_T("practise-think Checker"),INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0); 

		if(hInetSession!=NULL)   
		{ 
			strURLFile.TrimLeft(' ') ;
			strURLFile.TrimRight(' ') ;
			HINTERNET hHttpFile=::InternetOpenUrl(hInetSession,strURLFile.c_str(),NULL,0,INTERNET_FLAG_RELOAD,0); 

			if(hHttpFile!=NULL) 
			{
				BYTE Buf[1024] = {0} ;
				DWORD dwSize = sizeof(Buf) ;

				if(::InternetQueryOption(hHttpFile,INTERNET_OPTION_URL ,(LPVOID)Buf,&dwSize))
				{
					TCHAR szFileSize[32] = {0} ;
					DWORD dwFileSize = 0 ;
					DWORD  dwSizeBuffer = sizeof(szFileSize) ;
					CptStringList sl ;

					if(sl.Split((TCHAR*)Buf,_T("/"))>0 && ::HttpQueryInfo(hHttpFile,HTTP_QUERY_CONTENT_LENGTH,(LPVOID)szFileSize,&dwSizeBuffer,NULL)) 
					{ 
						CptString strFile ;

						strFile.Format(_T("%s\\%s"),strFolder,sl[sl.GetCount()-1]) ;

						HANDLE hFile = ::CreateFile(strFile.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL) ;

						if(hFile!=INVALID_HANDLE_VALUE)
						{
							dwSizeBuffer = ::_tstoi(szFileSize) ;

							DWORD dwRemainSize = dwSizeBuffer ;
							DWORD  dwBytesRead = 0; 
							dwSizeBuffer = 0 ;

							bRet = true ;
							while(dwRemainSize>0)
							{
								::InternetReadFile(hHttpFile, Buf,  sizeof(Buf), &dwBytesRead); 

								dwRemainSize -= dwBytesRead ;
								if(!::WriteFile(hFile,Buf,dwBytesRead,&dwSizeBuffer,NULL))
								{
									bRet = false ;
									break ;
								}
							}

							::CloseHandle(hFile) ;

							if(!bRet)
							{
								::DeleteFile(strFile.c_str()) ;
							}
						}

					}
				}
			}

			
		}
		
		return bRet ;
	}

	//static bool GetPEFileVersion(LPCTSTR lpFileName,int& nMajorVer,int& nMinVer,int& FixVer,int& BuildVer)
	//{
	//	bool bRet = false ;

	//	DWORD dwBlockSize = 0;
	//	DWORD dwBlockHandle = 0;

	//	dwBlockSize = ::GetFileVersionInfoSize( lpFileName, &dwBlockHandle );

	//	if( dwBlockSize != 0 )
	//	{//  Get the version information
	//		char* szBlock = (char*) ::malloc( dwBlockSize );

	//		if(szBlock!=NULL)
	//		{
	//			BOOL bSuccess = ::GetFileVersionInfo( lpFileName, dwBlockHandle, dwBlockSize, szBlock );

	//			if( bSuccess )
	//			{
	//				//  Get the fixed file information from that file
	//				VS_FIXEDFILEINFO* pVsfi;
	//				unsigned int cVsfi;

	//				if( ::VerQueryValue( szBlock, _T("\\"), (void**) &pVsfi, &cVsfi ))
	//				{
	//					if(cVsfi == sizeof(VS_FIXEDFILEINFO))
	//					{
	//						nMajorVer = HIWORD( pVsfi -> dwFileVersionMS ) ;
	//						nMinVer = LOWORD( pVsfi -> dwFileVersionMS ) ;
	//						FixVer = HIWORD( pVsfi -> dwFileVersionLS ) ;
	//						BuildVer = LOWORD( pVsfi -> dwFileVersionLS ) ;

	//						bRet = true ;
	//					}
	//				}
	//			}

	//			::free( szBlock );
	//			szBlock = NULL ;
	//		}
	//	}

	//	return bRet ;
	//}

	static bool IsFileExist2(CptString strFilePath)
	{
		bool bRet = false ;

		if(strFilePath.GetLength()>0)
		{
			WIN32_FIND_DATA wfd ;
			HANDLE hFind = ::FindFirstFile((LPCTSTR)strFilePath,&wfd) ;

			if(hFind!=INVALID_HANDLE_VALUE)
			{
				bRet = true ;
				::FindClose(hFind) ;
			}
		}

		return bRet ;
	}

	static void GetScreenSize(int *pWidth, int *pHeight)
	{
		HDC hDC;
		hDC = GetDC(0);
		if (pWidth != NULL) *pWidth = GetDeviceCaps(hDC, HORZRES);
		if (pHeight != NULL ) *pHeight = GetDeviceCaps(hDC, VERTRES);
		ReleaseDC(0, hDC);
	}

	// 中心显示窗体
	static void CenterWindow(HWND hwndDlg)
	{
		int nWidth, nHeight, nLeft, nTop;
		RECT rcWin;

		GetScreenSize(&nWidth, &nHeight);
		GetWindowRect(hwndDlg, &rcWin);

		nLeft = (nWidth - (rcWin.right - rcWin.left + 1)) / 2;
		nTop = (nHeight - (rcWin.bottom - rcWin.top + 1)) / 2;

		MoveWindow(hwndDlg, nLeft, nTop, rcWin.right - rcWin.left + 1, rcWin.bottom - rcWin.top + 1, TRUE);
	}

	// 透明窗体
	// nPercent: 0~255
	static void TransparenWnd(HWND hWnd,int nPercent,BOOL bTransparency)
	{
		typedef BOOL (__stdcall *TransFunc1_t)(HWND,COLORREF,BYTE,DWORD) ;

		if(!bTransparency || nPercent==0)
		{
			if(GetWindowLong(hWnd, GWL_EXSTYLE)&0x80000L)
			{
				SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) & ~0x80000L);
			}
		}
		else
		{
			HMODULE hModule = ::GetModuleHandle(_T("user32.dll"));
			TransFunc1_t SetLayeredWindowAttributes = NULL ;
			SetLayeredWindowAttributes =  (TransFunc1_t)::GetProcAddress(hModule, "SetLayeredWindowAttributes" );

			if(SetLayeredWindowAttributes)
			{
				SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | 0x80000L);

				const float fAlpha = (float)(255*(1.0-((float)nPercent/200.0))) ;

				//	Debug_Printf("Alpha=%.2f",fAlpha) ;
				SetLayeredWindowAttributes(hWnd, 0,(BYTE)fAlpha , 0x2);
			}
		}
	}

	// 进程是否存在
	static bool IsProcessExist(const TCHAR* pProceName)
	{
		_ASSERT(pProceName!=NULL) ;

		bool bRet = false ;

		HANDLE hSnapShot= ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

		PROCESSENTRY32 ProcessInfo;//声明进程信息变量
		ProcessInfo.dwSize=sizeof(ProcessInfo);//设置ProcessInfo的大小

		//返回系统中第一个进程的信息
		BOOL Status=Process32First(hSnapShot,&ProcessInfo);

		while(Status)
		{
			if(::_tcscmp(ProcessInfo.szExeFile,pProceName)==0)
			{
				bRet = true ;
				break ;
				/**
				HANDLE hProcess = NULL ;
				hProcess= OpenProcess(PROCESS_ALL_ACCESS, FALSE,ProcessInfo.th32ProcessID);
				if(hProcess!=NULL)
				{
				::CloseHandle(hProcess) ;
				bRet = true ;
				break ;
				}
				/**/
			}

			Status=Process32Next(hSnapShot,&ProcessInfo);
		}

		::CloseHandle(hSnapShot) ;

		return bRet ;
	}

	static bool IsWin64()
	{
		SYSTEM_INFO SysInfo ;
		::memset(&SysInfo,0,sizeof(SysInfo)) ;

		//typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
		void (WINAPI *pGetNativeSystemInfo)(LPSYSTEM_INFO) = (void (WINAPI *)(LPSYSTEM_INFO)) GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");

		if(pGetNativeSystemInfo!=NULL)
		{
			pGetNativeSystemInfo(&SysInfo) ;

			return (SysInfo.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 || SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ) ;
		}

		return false ;
	}

	static bool IsWow64()
	{
		static char cRet = -1 ;

		if (cRet==-1) 
		{
			BOOL bWow64Process = FALSE ;
			BOOL (WINAPI *pIsWow64Process)(HANDLE, BOOL *) = (BOOL (WINAPI *)(HANDLE, BOOL *))GetProcAddress(::GetModuleHandle(_T("kernel32")), "IsWow64Process");
			if (pIsWow64Process!=NULL) 
			{
				pIsWow64Process(::GetCurrentProcess(), &bWow64Process);
			}

			cRet = (bWow64Process==TRUE) ? 1 : 0 ;
		}

		return cRet==0?false:true;
	}

	static bool GetPEFileVersion(LPCTSTR lpFileName,SptVerionInfo& VerInfo)
	{
		bool bRet = false ;

		DWORD dwBlockSize = 0;
		DWORD dwBlockHandle = 0;
		TCHAR* pFileNameBuf = NULL ;

		if(lpFileName==NULL)
		{// 若文件名为NULL, 则为本PE文件的版本号
			pFileNameBuf = new TCHAR[2*MAX_PATH] ;

			_ASSERT(pFileNameBuf!=NULL) ;

			::GetModuleFileName(::GetModuleHandle(NULL),pFileNameBuf,4*MAX_PATH/sizeof(TCHAR)) ;
		}

		do
		{
			bRet = false;

			const TCHAR* pFileName = pFileNameBuf==NULL ? lpFileName : pFileNameBuf ;

			dwBlockSize = ::GetFileVersionInfoSize( pFileName, &dwBlockHandle );

			PT_BREAK_IF(dwBlockSize==NULL); // 获取版本信息失败就退出

			char* szBlock = (char*) ::malloc( dwBlockSize );

			PT_BREAK_IF(szBlock==NULL); // 分配内存失败就退出

			if(::GetFileVersionInfo( pFileName, dwBlockHandle, dwBlockSize, szBlock ))
			{
				//  Get the fixed file information from that file
				VS_FIXEDFILEINFO* pVsfi;
				unsigned int cVsfi;

				if( ::VerQueryValue( szBlock, _T("\\"), (void**) &pVsfi, &cVsfi ) && cVsfi == sizeof(VS_FIXEDFILEINFO))
				{
					VerInfo.nMajor = HIWORD( pVsfi -> dwFileVersionMS ) ;
					VerInfo.nMin = LOWORD( pVsfi -> dwFileVersionMS ) ;
					VerInfo.nFix = HIWORD( pVsfi -> dwFileVersionLS ) ;
					VerInfo.nBuild = LOWORD( pVsfi -> dwFileVersionLS ) ;

					bRet = true ;
				}
			}

			::free( szBlock );
			szBlock = NULL ;
		}
		while(0);
		
		SAFE_DELETE_MEMORY_ARRAY(pFileNameBuf) ;

		return bRet ;
	}

	//SptVerionInfo GetCurPEVersion() 
	//{
	//	static bool bOnce = false;
	//	static SptVerionInfo verRet ;

	//	if(!bOnce)
	//	{
	//		TCHAR* pBuf = new TCHAR[4*MAX_PATH] ;

	//		if(pBuf!=NULL)
	//		{
	//			int nMajVer = 0 ;
	//			int nMinVer = 0 ;
	//			int nFixVer = 0 ;
	//			int nBuildVer = 0 ;

	//			::GetModuleFileName(::GetModuleHandle(NULL),pBuf,4*MAX_PATH/sizeof(TCHAR)) ;

	//			if(::GetPEFileVersion(pBuf,nMajVer,nMinVer,nFixVer,nBuildVer) )
	//			{
	//			}

	//			SAFE_DELETE_MEMORY_ARRAY(pBuf) ;
	//		}

	//		
	//	}

	//}

	static bool IsVISTAOrLaterVersion()
	{
		static char cRet = -1 ;

		if(cRet==-1)
		{
			OSVERSIONINFO oi ;

			::memset(&oi,0,sizeof(OSVERSIONINFO)) ;

			oi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO) ;

			if(::GetVersionEx(&oi))
			{
				cRet = oi.dwMajorVersion>=6 ? 1 : 0;
			}
		}

		return cRet ? true : false ;
	}

	static bool ShutdownWindows()
	{
		bool bRet = false ;

		if(CptGlobal::IsVISTAOrLaterVersion())
		{// window vsita or 7
			::system("shutdown /s") ;
			bRet = true ;
		}
		else
		{// windows xp
			HANDLE hToken;   
			TOKEN_PRIVILEGES tkp;   

			if(::OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
			{
				::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
				tkp.PrivilegeCount=1;
				tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;

				::AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);

				if(::GetLastError()==ERROR_SUCCESS)   
				{
					::ExitWindowsEx(EWX_POWEROFF|EWX_FORCE,0) ;  
					bRet = true ;
				}   
			}   
		}

		return bRet ;
	}
	

	static bool IsEnableUAC()
	{
		static char cRet = -1 ;

		if (cRet==-1) 
		{
			if (IsVISTAOrLaterVersion()) 
			{
				CptRegistry reg ;
				cRet = 1;
				if(reg.OpenKey(CptRegistry::LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System")))
				{
					DWORD val = 1;
					cRet = !(reg.GetValueInt(_T("EnableLUA"), val) && val == 0) ? 1 : 0 ;
				}
			}
		}

		return	cRet?true:false;
	}

	static bool IsUserAnAdmin()
	{
		static BOOL	(WINAPI *pIsUserAnAdmin)(void);

		static char cRet = -1 ;

		if (cRet==-1) 
		{
			pIsUserAnAdmin = (BOOL (WINAPI *)(void))GetProcAddress(::GetModuleHandle(_T("shell32")), "IsUserAnAdmin");
			if (pIsUserAnAdmin!=NULL) 
			{
				cRet = (pIsUserAnAdmin()==TRUE) ? 1:0;
			}
		}

		return	cRet?true:false;
	}

	static bool SetPrivilege(LPTSTR pszPrivilege, bool bEnable)
	{
		HANDLE hToken = NULL;
		TOKEN_PRIVILEGES tp;
		bool bRet = false ;

		if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		{
			if(hToken!=NULL)
			{
				if (::LookupPrivilegeValue(NULL, pszPrivilege, &tp.Privileges[0].Luid))
				{
					tp.PrivilegeCount = 1;
					tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0 ;

					bRet = ::AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0) == TRUE ;
				}

				::CloseHandle(hToken) ;
			}
		}

		return true;
	}

	/* IsTextUTF8
	*
	* UTF-8 is the encoding of Unicode based on Internet Society RFC2279
	* ( See http://www.cis.ohio-state.edu/htbin/rfc/rfc2279.html )
	*
	* Basicly:
	* 0000 0000-0000 007F - 0xxxxxxx  (ascii converts to 1 octet!)
	* 0000 0080-0000 07FF - 110xxxxx 10xxxxxx    ( 2 octet format)
	* 0000 0800-0000 FFFF - 1110xxxx 10xxxxxx 10xxxxxx (3 octet format)
	* (this keeps going for 32 bit unicode)
	*
	*
	* Return value:  TRUE, if the text is in UTF-8 format.
	*                FALSE, if the text is not in UTF-8 format.
	*                We will also return FALSE is it is only 7-bit ascii, so the right code page
	*                will be used.
	*
	*                Actually for 7 bit ascii, it doesn't matter which code page we use, but
	*                notepad will remember that it is utf-8 and "save" or "save as" will store
	*                the file with a UTF-8 BOM.  Not cool.
	*/

	static BOOL IsTextUTF8( LPSTR lpstrInputStream, INT iLen )
	{
		INT   i;
		DWORD cOctets;  // octets to go in this UTF-8 encoded character
		UCHAR chr;
		BOOL  bAllAscii= TRUE;

		cOctets= 0;
		for( i=0; i < iLen; i++ ) {
			chr= *(lpstrInputStream+i);

			if( (chr&0x80) != 0 ) bAllAscii= FALSE;

			if( cOctets == 0 )  {
				//
				// 7 bit ascii after 7 bit ascii is just fine.  Handle start of encoding case.
				//
				if( chr >= 0x80 ) { 
					//
					// count of the leading 1 bits is the number of characters encoded
					//
					do {
						chr <<= 1;
						cOctets++;
					}
					while( (chr&0x80) != 0 );

					cOctets--;                        // count includes this character
					if( cOctets == 0 ) return FALSE;  // must start with 11xxxxxx
				}
			}
			else {
				// non-leading bytes must start as 10xxxxxx
				if( (chr&0xC0) != 0x80 ) {
					return FALSE;
				}
				cOctets--;                           // processed another octet in encoding
			}
		}

		//
		// End of text.  Check for consistency.
		//

		if( cOctets > 0 ) {   // anything left over at the end is an error
			return FALSE;
		}

		if( bAllAscii ) {     // Not utf-8 if all ascii.  Forces caller to use code pages for conversion
			return FALSE;
		}

		return TRUE;
	}

	static bool IsInstallSoftware(LPCTSTR lpDisplayName)
	{
#pragma warning(push)
#pragma warning(disable:4996)
		bool bRet = false ;

		HKEY   key,key1;  
		DWORD   lpcbname=100,ret=0;  
		TCHAR   szName[100];  
		TCHAR	szDisplayNameUpper[100] ;
		DWORD	lSize = 0 ;

		::_tcscpy(szDisplayNameUpper,lpDisplayName) ;
		::_tcsupr(szDisplayNameUpper) ;
		
		int   i=0;  

		if(::RegOpenKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"),&key)==ERROR_SUCCESS)  
		{  
			for(i=0;ret==ERROR_SUCCESS && !bRet;i++)  
			{  
				ZeroMemory(szName,sizeof(szName));  
				lpcbname=sizeof(szName)/sizeof(TCHAR);  

				ret=::RegEnumKey(key,i,szName,sizeof(szName)/sizeof(TCHAR));  

				if (ret==ERROR_SUCCESS)  
				{  
					if(::RegOpenKey(key,szName,&key1)==ERROR_SUCCESS)
					{  
						ZeroMemory(szName,sizeof(szName));  
						lpcbname=sizeof(szName)/sizeof(TCHAR);  
						lSize = sizeof(szName)/sizeof(TCHAR) ;
						DWORD dwType = REG_SZ ;

						if(::RegQueryValueEx(key1,_T("DisplayName"),NULL,&dwType,(BYTE*)szName,(DWORD*)&lSize)==ERROR_SUCCESS)  
						{  
							::_tcsupr(szName) ;
							if(::_tcscmp(szName,szDisplayNameUpper)==0)
							{
								bRet = true ;
							}
						}  

						::RegCloseKey(key1) ;
					}  
				}//if  
			}//for  

			::RegCloseKey(key) ;
		}  
		return bRet ;

#pragma warning(pop)
	}

	static bool CanFolderWrite(const CptString strFolder)
	{
		bool bRet = false ;

		if(CptGlobal::IsFolder(strFolder.c_str()))
		{
			CptString strTemFile ;

			if(strFolder[strFolder.GetLength()-1]!='\\' && strFolder[strFolder.GetLength()-1]!='/')
			{
				strTemFile.Format(_T("%s\\%s"),strFolder,_T("A1BEEFE4-28A7-457C-9D80-2AA99362639F")) ;
			}
			else
			{
				strTemFile.Format(_T("%s%s"),strFolder,_T("A1BEEFE4-28A7-457C-9D80-2AA99362639F")) ;
			}

			if(IsFileExist(strTemFile.c_str()))
			{
				if(strFolder[strFolder.GetLength()-1]!='\\' && strFolder[strFolder.GetLength()-1]!='/')
				{
					strTemFile.Format(_T("%s\\%s"),strFolder,_T("D9470AD6-3731-46A0-800E-42CC081573C0")) ;
				}
				else
				{
					strTemFile.Format(_T("%s%s"),strFolder,_T("D9470AD6-3731-46A0-800E-42CC081573C0")) ;
				}

				if(IsFileExist(strTemFile.c_str()))
				{// 若依然存在，则返回false
					return false ;
				}
			}

			HANDLE hFile = ::CreateFile(strTemFile.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
				FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE|FILE_FLAG_SEQUENTIAL_SCAN,NULL) ;

			if(hFile!=INVALID_HANDLE_VALUE)
			{
				::CloseHandle(hFile) ;
				bRet = true ;
			}
		}

		return bRet ;
	}


	// 判断是否需要提升 特权 级别
	static bool IsNeedElevatePrivilege()
	{
		bool bRet = false ;

		if(CptGlobal::IsVISTAOrLaterVersion() && CptGlobal::IsEnableUAC())
		{

			//if(strDstFolder.GetAt(1)==':')
			//{
			//}

			//  TCHAR     lpVolumeNameBuffer[1024];     //硬盘卷标名称  
			//DWORD   dwVolumeSerialNumber;             //序列号  
			//DWORD   dwMaximumComponentLength;     //文件名最大长度  
			//DWORD   FileSystemFlags;                       //文件系统标志  
			//TCHAR     lpFileSystemNameBuffer[1024];//文件系统名称  
			// 
			//CString   strDisk;  
			//strDisk.Format("%c:\\\\",diskChar);  
			// 
			//if(!GetVolumeInformation(strDisk,                
			//                lpVolumeNameBuffer,1024,    
			//&dwVolumeSerialNumber,  
			//&dwMaximumComponentLength,  
			//&FileSystemFlags,  
			//lpFileSystemNameBuffer,1024))   

			bRet = true ;
		}

		return bRet ;
	}

			/**
	static BOOL GetCurrentUserAndDomain(CptString& strUserAccount,CptString& strDomain) 
	{
		BOOL fSuccess   =   FALSE;
		HANDLE hToken   =   NULL;
		PTOKEN_USER     ptiUser     =   NULL;
		DWORD                 cbti           =   0;
		SID_NAME_USE   snu;

		__try   
		{

			//   Get   the   calling   thread 's   access   token.
			if   (!OpenThreadToken(GetCurrentThread(),TOKEN_QUERY,TRUE,&hToken))   
			{

				if   (GetLastError()   !=   ERROR_NO_TOKEN)
					__leave;

				//   Retry   against   process   token   if   no   thread   token   exists.
				if   (!OpenProcessToken(GetCurrentProcess(),   TOKEN_QUERY,
					&hToken))
					__leave;
			}

			//   Obtain   the   size   of   the   user   information   in   the   token.
			if   (GetTokenInformation(hToken,   TokenUser,   NULL,   0,   &cbti))   
			{

				//   Call   should   have   failed   due   to   zero-length   buffer.
				__leave;

			}   else   
			{

				//   Call   should   have   failed   due   to   zero-length   buffer.
				if   (GetLastError()   !=   ERROR_INSUFFICIENT_BUFFER)
					__leave;
			}

			//   Allocate   buffer   for   user   information   in   the   token.
			ptiUser   =   (PTOKEN_USER)   HeapAlloc(GetProcessHeap(),   0,   cbti);
			if   (!ptiUser)
				__leave;

			//   Retrieve   the   user   information   from   the   token.
			if(!GetTokenInformation(hToken,TokenUser,ptiUser,cbti,&cbti))
				__leave;

			TCHAR szUser[256+1] = {0} ;
			DWORD dwUserSize = sizeof(szUser) ;

			TCHAR szDomain[256+1] = {0} ;
			DWORD dwDomainSize = sizeof(szDomain) ;

			//   Retrieve   user   name   and   domain   name   based   on   user 's   SID.
			if   (!LookupAccountSid(NULL,   ptiUser-> User.Sid,   szUser,   &dwUserSize,
				szDomain,   &dwDomainSize,   &snu))
				__leave;

			strUserAccount = szUser ;
			strDomain = szDomain ;
			fSuccess   =   TRUE;

		}   
__finally   
		{

			//   Free   resources.
			if   (hToken)
				CloseHandle(hToken);

			if   (ptiUser)
				HeapFree(GetProcessHeap(),   0,   ptiUser);
		}

		return   fSuccess;
	}
	/**/

	static DWORD CalculateCRC32(BYTE* pBuf,int nBufSize) 
	{
		// Static CRC table; we have a table lookup algorithm
		static const DWORD arrdwCrc32Table[256] =
		{
			0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
			0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
			0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
			0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
			0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
			0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
			0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
			0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
			0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
			0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
			0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
			0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
			0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
			0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
			0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
			0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,

			0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
			0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
			0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
			0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
			0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
			0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
			0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
			0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
			0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
			0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
			0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
			0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
			0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
			0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
			0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
			0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,

			0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
			0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
			0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
			0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
			0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
			0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
			0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
			0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
			0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
			0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
			0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
			0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
			0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
			0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
			0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
			0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,

			0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
			0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
			0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
			0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
			0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
			0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
			0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
			0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
			0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
			0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
			0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
			0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
			0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
			0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
			0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
			0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
		};

		ULONG  crc(0xffffffff);

		while(nBufSize--)
			crc = (crc >> 8) ^ arrdwCrc32Table[(crc & 0xFF) ^ *pBuf++];

		return crc^0xffffffff;

		/**

		DWORD dwRet = 0 ;

		DWORD * CONST pdwCrc32 = &dwRet;
		// There is a bug in the Microsoft compilers where inline assembly
		// code cannot access static member variables.  This is a work around
		// for that bug.  For more info see Knowledgebase article Q88092
		CONST VOID * CONST ptrCrc32Table = &arrdwCrc32Table;


		// 下面这段ASM代码在MS 的 x64编译器下不能通过
		// Register use:
		//		eax - CRC32 value
		//		ebx - a lot of things
		//		ecx - CRC32 value
		//		edx - address of end of buffer
		//		esi - address of start of buffer
		//		edi - CRC32 table
		//
		// assembly part by Brian Friesen
		__asm
		{
			// Save the esi and edi registers
			//push esi
			//push edi

			mov eax, pdwCrc32			// Load the pointer to dwCrc32
				mov ecx, [eax]				// Dereference the pointer to load dwCrc32

			mov edi, ptrCrc32Table		// Load the CRC32 table

				mov esi, pBuf			// Load buffer
				mov ebx, nBufSize		// Load dwBytesRead
				lea edx, [esi + ebx]		// Calculate the end of the buffer

crc32loop:
			xor eax, eax				// Clear the eax register
				mov bl, byte ptr [esi]		// Load the current source byte

			mov al, cl					// Copy crc value into eax
				inc esi						// Advance the source pointer

				xor al, bl					// Create the index into the CRC32 table
				shr ecx, 8

				mov ebx, [edi + eax * 4]	// Get the value out of the table
			xor ecx, ebx				// xor with the current byte

				cmp edx, esi				// Have we reached the end of the buffer?
				jne crc32loop

				// Restore the edi and esi registers
				//pop edi
				//pop esi

				mov eax, pdwCrc32			// Load the pointer to dwCrc32
				mov [eax], ecx				// Write the result
		}

		return dwRet ;
		/**/

		
	}
	/**/

	//static bool PaintImageToDC(HDC hDC,HBITMAP hBitmap)
	//{
	//	//::GetObject(hBitmap,sizeof(bm),&bm) ;
	//	////::GetClientRect(m_hWnd,&rtClient) ;

	//	//HDC hDC = ::GetDC(m_hWnd) ;
	//	//HDC hCompatableDC = ::CreateCompatibleDC(hDC) ;

	//	//HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hCompatableDC,hBitmap) ;

	//	//::StretchBlt(hDC,0,0,rtClient.right,rtClient.bottom,hCompatableDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY) ;
	//	return false ;
	//}

	static CptString MakeUnlimitFileName(CptString strFileName,bool bAddOrRemove)
	{
		CptString strRet ;

		int nLength = strFileName.GetLength() ;

		if(nLength>11 && strFileName.Left(8).CompareNoCase(_T("file:\\\\\\"))==0)
		{// 带 file: 头的本地文件,先去掉这个头
			strFileName = strFileName.Right(strFileName.GetLength()-8) ;
			nLength = strFileName.GetLength() ;
		}

		if(nLength<3)
		{
			return strRet ;
		}

		int nResult = -99 ;
		const TCHAR* szLetters = strFileName.c_str() ;

		if(nLength==3)
		{
			const TCHAR& c1 = strFileName.GetAt(0) ;

			if((c1<='Z' && c1>='A') || (c1<='z' && c1>='a'))
			{
				if(strFileName.GetAt(1)==':' && strFileName.GetAt(2)=='\\')
				{
					nResult = bAddOrRemove ? 1 : 0 ;
				}
			}
		}
		else if(nLength>3)
		{
			int Change = 0 ; // 0: 不改变, 1: 追加 unlimit 的头, -1: 去掉 unlimit 的头, -99: 无效的文件名

			//TCHAR szLetters[4] = {0} ;
			//::memcpy(szLetters,strFileName.c_str(),sizeof(szLetters)) ;

			CptString strTemTest ;

			if(szLetters[0]=='\\' && szLetters[1]=='\\' && szLetters[2]=='?' && szLetters[3]=='\\')
			{// 带有 unlimit 的头
				Change = bAddOrRemove ? 0 : -1  ;

				strTemTest = strFileName.Right(strFileName.GetLength()-4) ;
			}
			else
			{//
				strTemTest = strFileName ;
				Change = bAddOrRemove ? 1 : 0 ;
			}

			CptStringList sl ;

			sl.Split(strTemTest,'\\') ;
			// 检查字符串是否为合法的路径格式

			if(sl.GetCount()>1)
			{
				if( (sl[0].GetLength()==0 && sl[1].GetLength()==0) ||
					(sl[0].GetLength()==3 && sl[0].CompareNoCase(_T("UNC"))==0) || 
					(sl[0].GetLength()==2 && sl[0].GetAt(1)==':' && ((sl[0].GetAt(0)<='z' && sl[0].GetAt(0)>='a') || (sl[0].GetAt(0)<='Z' && sl[0].GetAt(0)>='A'))))
				{
					nResult = Change ;
				}
			}
			else if(sl.GetCount()==1)
			{
				if((sl[0].GetLength()==2 && sl[0].GetAt(1)==':' && ((sl[0].GetAt(0)<='z' && sl[0].GetAt(0)>='a') || (sl[0].GetAt(0)<='Z' && sl[0].GetAt(0)>='A'))))
				{
					nResult = Change ;
				}
			}
		}

		switch(nResult)
		{
		case -1:
			{// 去除
				if(strFileName.Left(7).CompareNoCase(_T("\\\\?\\UNC"))==0)
				{// 网络的
					strRet = _T("\\") + strFileName.Right(strFileName.GetLength()-7) ;
				}
				else
				{// 本地的
					strRet = strFileName.Right(strFileName.GetLength()-4) ;
				}
			}
			break ;

		case 0:
			strRet = strFileName ;
			break ;
		case 1:
			{// 添加
				if(szLetters[0]=='\\')
				{// 网络的
					strFileName.TrimLeft('\\') ;
					strRet = _T("\\\\?\\UNC\\") + strFileName ;
				}
				else
				{// 本地的
					strRet = _T("\\\\?\\") + strFileName ;
				}
			}
			break ;

		default:
			strRet = _T("") ;
			break ;
		}

		return strRet ;
	}

};
