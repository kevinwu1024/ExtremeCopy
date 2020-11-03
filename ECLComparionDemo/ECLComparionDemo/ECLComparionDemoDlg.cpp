
// ECLComparionDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ECLComparionDemo.h"
#include "ECLComparionDemoDlg.h"
#include "afxdialogex.h"

#include "ExtremeCopyAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CECLComparionDemoDlg dialog
CECLComparionDemoDlg::CECLComparionDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CECLComparionDemoDlg::IDD, pParent),m_uElapsedTimeCount(0),m_uDoneSize(0),m_uFileCount(0),m_uLastStreamSize(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	::InitializeCriticalSection(&m_Lock) ;
}

CECLComparionDemoDlg::~CECLComparionDemoDlg() 
{
	::DeleteCriticalSection(&m_Lock) ;
}

void CECLComparionDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CECLComparionDemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADDFOLDERSRC, &CECLComparionDemoDlg::OnBnClickedButtonAddfoldersrc)
	ON_BN_CLICKED(IDC_BUTTON_CLEARSOURCE, &CECLComparionDemoDlg::OnBnClickedButtonClearsource)
	ON_BN_CLICKED(IDC_BUTTON_ADDFOLDERDST, &CECLComparionDemoDlg::OnBnClickedButtonAddfolderdst)
	ON_BN_CLICKED(IDC_BUTTON_CLEARDST, &CECLComparionDemoDlg::OnBnClickedButtonCleardst)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_WINCOPY, &CECLComparionDemoDlg::OnBnClickedButtonWincopy)
	ON_BN_CLICKED(IDC_BUTTON_WINMOVE, &CECLComparionDemoDlg::OnBnClickedButtonWinmove)
	ON_BN_CLICKED(IDC_BUTTON_EXTREMECOPY, &CECLComparionDemoDlg::OnBnClickedButtonExtremecopy)
	ON_BN_CLICKED(IDC_BUTTON_EXTREMEMOVE, &CECLComparionDemoDlg::OnBnClickedButtonExtrememove)
	ON_BN_CLICKED(IDCANCEL, &CECLComparionDemoDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CECLComparionDemoDlg message handlers

BOOL CECLComparionDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	this->SetDlgItemText(IDC_EDIT_SOURCE,_T("m:\\New folder (2)")) ;
	this->SetDlgItemText(IDC_EDIT_DESTINATION,_T("x:\\New folder")) ; 

	m_hTaskThread = (HANDLE)::_beginthreadex(NULL,NULL,CECLComparionDemoDlg::TaskThreadFunc,this,NULL,(unsigned int*)&m_dwTaskThreadID) ;

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CECLComparionDemoDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here

	CDialogEx::OnCancel();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CECLComparionDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CECLComparionDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString CECLComparionDemoDlg::SelectFolder() 
{
	CString strRet ;

	TCHAR szFile[MAX_PATH] = {0} ;

	BROWSEINFO brInfo ;
	::memset(&brInfo,0,sizeof(brInfo)) ;

	brInfo.ulFlags = BIF_NEWDIALOGSTYLE ;

	LPITEMIDLIST pidlBrowse = ::SHBrowseForFolder(&brInfo) ;

	if(NULL!=pidlBrowse && ::SHGetPathFromIDList(pidlBrowse,szFile))
	{
		strRet = szFile ;
	}

	return strRet ;
}

// Add source folder to edit box
void CECLComparionDemoDlg::OnBnClickedButtonAddfoldersrc()
{
	// TODO: Add your control notification handler code here
	CString strFolder = this->SelectFolder() ;

	if(strFolder.GetLength()>0)
	{
		this->SetDlgItemText(IDC_EDIT_SOURCE,strFolder) ;
	}
}

// Clear source folder from edit box
void CECLComparionDemoDlg::OnBnClickedButtonClearsource()
{
	// TODO: Add your control notification handler code here
	this->SetDlgItemText(IDC_EDIT_SOURCE,_T("")) ;
}

// Add destination folder to edit box
void CECLComparionDemoDlg::OnBnClickedButtonAddfolderdst()
{
	// TODO: Add your control notification handler code here
	CString strFolder = this->SelectFolder() ;

	if(strFolder.GetLength()>0)
	{
		this->SetDlgItemText(IDC_EDIT_DESTINATION,strFolder) ;
	}
}

// Clear destination folder from edit box
void CECLComparionDemoDlg::OnBnClickedButtonCleardst()
{
	// TODO: Add your control notification handler code here
	this->SetDlgItemText(IDC_EDIT_DESTINATION,_T("")) ;
}

void CECLComparionDemoDlg::OnBnClickedButtonWincopy()
{
	// TODO: Add your control notification handler code here
	::PostThreadMessage(m_dwTaskThreadID,WM_DEMOTASKRUN,(WPARAM)TT_WinCopy,NULL) ;
}


void CECLComparionDemoDlg::OnBnClickedButtonWinmove()
{
	// TODO: Add your control notification handler code here
	::PostThreadMessage(m_dwTaskThreadID,WM_DEMOTASKRUN,(WPARAM)TT_WinMove,NULL) ;
}


void CECLComparionDemoDlg::OnBnClickedButtonExtremecopy()
{
	// TODO: Add your control notification handler code here
	::PostThreadMessage(m_dwTaskThreadID,WM_DEMOTASKRUN,(WPARAM)TT_ExtremeCopy,NULL) ;
}


void CECLComparionDemoDlg::OnBnClickedButtonExtrememove()
{
	// TODO: Add your control notification handler code here
	::PostThreadMessage(m_dwTaskThreadID,WM_DEMOTASKRUN,(WPARAM)TT_ExtremeMove,NULL) ;
}

unsigned CECLComparionDemoDlg::TaskThreadFunc(void* param) 
{
	CECLComparionDemoDlg* pThis = (CECLComparionDemoDlg*)param ;

	MSG msg ;

	while(::GetMessage(&msg,NULL,0,0))
	{
		if(WM_DEMOTASKRUN==msg.message)
		{
			switch(msg.wParam)
			{
			case TT_WinCopy:
			case TT_WinMove:
			case TT_ExtremeCopy:
			case TT_ExtremeMove:
				pThis->RunTask((ETaskType)msg.wParam) ;
				break ;

			default:_ASSERT(FALSE) ; break ;
			}
		}

	}

	return 0 ;
}

BOOL CECLComparionDemoDlg::RunTask(ETaskType tt) 
{
	BOOL bRet = FALSE ;

	CString strSrcFolder ;
	CString strDstFolder ;

	if(this->GetTaskFolders(strSrcFolder,strDstFolder))
	{
		m_uElapsedTimeCount = 0 ;
		m_uDoneSize = 0 ;
		m_uFileCount = 0 ;

		this->EnableAllButton(FALSE) ;

		this->SetTimer(TIMER_ONE_SECONDE,1000,NULL) ;

		switch(tt)
		{
		case TT_WinCopy:
			m_bIsWinCopy = true ;
			this->UpdateStaToUI() ;
			this->UpdateStatus(DS_Running) ;
			bRet=this->WinCopy(strSrcFolder,strDstFolder) ;
			break ;

		case TT_WinMove:
			m_bIsWinCopy = true ;
			this->UpdateStaToUI() ;
			this->UpdateStatus(DS_Running) ;
			bRet=this->WinMove(strSrcFolder,strDstFolder) ;
			break ;

		case TT_ExtremeCopy:
			m_bIsWinCopy = false ;
			this->UpdateStaToUI() ;
			this->UpdateStatus(DS_Running) ;
			bRet=this->ExtremeCopy(strSrcFolder,strDstFolder,TRUE) ;
			break ;

		case TT_ExtremeMove:
			m_bIsWinCopy = false ;
			this->UpdateStaToUI() ;
			this->UpdateStatus(DS_Running) ;
			bRet=this->ExtremeCopy(strSrcFolder,strDstFolder,FALSE) ;
			break ;

		default:
			_ASSERT(FALSE) ; 
			bRet=FALSE ; 
			break ;
		}

		this->KillTimer(TIMER_ONE_SECONDE) ;
		this->UpdateStaToUI() ;

		this->EnableAllButton(TRUE) ;

		this->UpdateStatus(bRet ? DS_Finished : DS_Failed) ;	
		
	}

	return bRet ;
}



BOOL CECLComparionDemoDlg::ExtremeCopy(CString strSrcDirectory,CString strDstDirectory,BOOL bCopyOrMove)
{
	BOOL bRet = FALSE ;

	::ExtremeCopy_Stop() ;

	if(::ExtremeCopy_AttachSrc((LPCTSTR)strSrcDirectory)
		&& ::ExtremeCopy_SetDestinationFolder((LPCTSTR)strDstDirectory))
	{
		//{
		//	::srand(::time(NULL)) ;

		//	int Array[] = {32,16,8,4,2} ;

		//	int BufSize = Array[rand()%5] ;

		//	BOOL b = (::ExtremeCopy_SetBufferSize(BufSize)!=-1) ;

		//	CString str ;
		//	str.Format(_T("buffer size is: %d  %s"),BufSize,b?_T("success"):_T("failed")) ;

		//	::OutputDebugString((LPCTSTR)str) ;
		//}

		bRet = ::ExtremeCopy_Start(bCopyOrMove ? XCRunType_Copy:XCRunType_Move,true,CECLComparionDemoDlg::ExtremeCopyRoutine,this)==START_ERROR_CODE_SUCCESS;
	}

	return bRet ;
}


int CECLComparionDemoDlg::ExtremeCopyRoutine(int nCmd,void* pRoutineParam,int nParam1,int nParam2,const TCHAR* pSrcStr,const TCHAR* pDstStr) 
{
	int nRet = 0 ;

	_ASSERT(pRoutineParam!=NULL) ;

	CECLComparionDemoDlg* pThis = (CECLComparionDemoDlg*)pRoutineParam ;

	switch(nCmd)
	{
	case ROUTINE_CMD_BEGINONEFILE: //begin to copy one file
		{
			pThis->m_uFileCount++ ;

			::_tprintf(_T("begin to copy one file :\r\n")) ;

			if(pSrcStr!=NULL)
			{
				::_tprintf(_T("beginning source file : %s \r\n"),pSrcStr) ;
			}

			if(pDstStr!=NULL)
			{
				::_tprintf(_T("beginning destination file : %s \r\n"),pDstStr) ;
			}
		}
		break ;

	case ROUTINE_CMD_FINISHONEFILE: // copy one file ended
		{
			::_tprintf(_T("copy one file ended :\r\n")) ;

			if(pSrcStr!=NULL)
			{
				::_tprintf(_T("ending source file : %s \r\n"),pSrcStr) ;
			}

			if(pDstStr!=NULL)
			{
				::_tprintf(_T("ending destination file : %s \r\n"),pDstStr) ;
			}
		}
		break ;

	case ROUTINE_CMD_FILEFAILED: // file failed 
		{// ExtremeCopy will stop
			::_tprintf(_T("file failed : error code = %d \r\n"),nParam1) ;

			if(pSrcStr!=NULL)
			{
				::_tprintf(_T("failed source file : %s \r\n"),pSrcStr) ;
			}

			if(pDstStr!=NULL)
			{
				::_tprintf(_T("failed destination file : %s \r\n"),pDstStr) ;
			}
		}
		break ;

	case ROUTINE_CMD_SAMEFILENAME: // same file name
		::_tprintf(_T("same file name : \r\n")) ;

		if(pSrcStr!=NULL)
		{
			::_tprintf(_T("same name source file : %s \r\n"),pSrcStr) ;
		}

		if(pDstStr!=NULL)
		{
			::_tprintf(_T("same name destination file : %s \r\n"),pDstStr) ;
		}

		nRet = SameFileProcess_Replace ; // replace
		break ;

	case ROUTINE_CMD_DATAWROTE: // data wrote into storage
		::_tprintf(_T("data wrote: %d \r\n"),nParam1) ;
		pThis->m_uDoneSize += nParam1 ;
		break ;

	case ROUTINE_CMD_TASKFINISH: // finish current copy task
		::_tprintf(_T("task finished !\r\n")) ;
		break ;

	default: break ;
	}

	return nRet ;
}

BOOL CECLComparionDemoDlg::WinCopy(CString strSrcDirectory,CString strDstDirectory) 
{
	BOOL bRet = FALSE ;

	WIN32_FIND_DATA wfd ;

	CString strSrcFiles = strSrcDirectory + _T("\\*.*") ;

	HANDLE hFind = ::FindFirstFile((LPCTSTR)strSrcFiles,&wfd) ;

	if(INVALID_HANDLE_VALUE!=hFind)
	{
		CString strSrcFile ;
		CString strDstFile ;

		CString strNewDirectory = strDstDirectory + _T("\\") + this->ExtractFileName(strSrcDirectory) ;

		::CreateDirectoryEx(strSrcDirectory,strNewDirectory,NULL) ;

		bRet = TRUE ;

		do
		{
			if(::_tcscmp(wfd.cFileName,_T("."))==0 || ::_tcscmp(wfd.cFileName,_T(".."))==0)
			{
				continue ;
			}
			else
			{
				strSrcFile = strSrcDirectory + _T("\\") + wfd.cFileName ;

				if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				{// directory
					if(!this->WinCopy(strSrcFile,strNewDirectory))
					{
						bRet = FALSE ;
						break ;
					}
				}
				else
				{// file
					strDstFile = strNewDirectory + _T("\\") + wfd.cFileName ;
					BOOL bCancel = FALSE ;

					if(!::CopyFileEx((LPCTSTR)(_T("\\\\?\\")+strSrcFile),(LPCTSTR)(_T("\\\\?\\")+strDstFile),(LPPROGRESS_ROUTINE)CECLComparionDemoDlg::CopyProgressRoutine,
								this,&bCancel,0))
					{
						bRet = FALSE ;
						break ;
					}
				}
			}
		}
		while(::FindNextFile(hFind,&wfd));

		::FindClose(hFind) ;
	}

	return bRet ;
}

BOOL CECLComparionDemoDlg::WinMove(CString strSrcDirectory,CString strDstDirectory) 
{
	BOOL bRet = FALSE ;

	WIN32_FIND_DATA wfd ;

	CString strSrcFiles = strSrcDirectory + _T("\\*.*") ;

	HANDLE hFind = ::FindFirstFile((LPCTSTR)strSrcFiles,&wfd) ;

	if(INVALID_HANDLE_VALUE!=hFind)
	{
		CString strSrcFile ;
		CString strDstFile ;

		CString strNewDirectory = strDstDirectory + _T("\\") + this->ExtractFileName(strSrcDirectory) ;

		::CreateDirectoryEx(strSrcDirectory,strNewDirectory,NULL) ;

		bRet = TRUE ;

		do
		{
			if(::_tcscmp(wfd.cFileName,_T("."))==0 || ::_tcscmp(wfd.cFileName,_T(".."))==0)
			{
				continue ;
			}
			else
			{
				strSrcFile = strSrcDirectory + _T("\\") + wfd.cFileName ;

				if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				{// directory
					if(!this->WinMove(strSrcFile,strNewDirectory))
					{
						bRet = FALSE ;
						break ;
					}
				}
				else
				{// file
					strDstFile = strNewDirectory + _T("\\") + wfd.cFileName ;
					BOOL bCancel = FALSE ;

					if(!::MoveFileWithProgress((LPCTSTR)(_T("\\\\?\\")+strSrcFile),(LPCTSTR)(_T("\\\\?\\")+strDstFile),(LPPROGRESS_ROUTINE)CECLComparionDemoDlg::CopyProgressRoutine,
						this,MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED))
					{
						bRet = FALSE ;
						break ;
					}
				}
			}
		}
		while(::FindNextFile(hFind,&wfd));

		::FindClose(hFind) ;

		::RemoveDirectory((LPCTSTR)strSrcDirectory) ;
	}

	return bRet ;

}

void CECLComparionDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	++m_uElapsedTimeCount ;

	this->UpdateStaToUI() ;

	CDialogEx::OnTimer(nIDEvent);
}

DWORD CECLComparionDemoDlg::CopyProgressRoutine(
	LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER StreamSize,
	LARGE_INTEGER StreamBytesTransferred,
	DWORD dwStreamNumber,
	DWORD dwCallbackReason,
	HANDLE hSourceFile,
	HANDLE hDestinationFile,
	LPVOID lpData
	)
{
	_ASSERT(NULL!=lpData) ;

	CECLComparionDemoDlg* pThis = (CECLComparionDemoDlg*)lpData ;

	::EnterCriticalSection(&pThis->m_Lock) ;

	unsigned __int64 uCurStreamSize = 0 ;

	
	{
		if(StreamBytesTransferred.HighPart>0)
		{
			uCurStreamSize = ((unsigned __int64)StreamBytesTransferred.HighPart)<<32 ;
		}

		uCurStreamSize = StreamBytesTransferred.LowPart ;
	}

	if(pThis->m_uLastStreamSize>uCurStreamSize)
	{
		pThis->m_uLastStreamSize = 0 ;
	}

	pThis->m_uDoneSize += (uCurStreamSize - pThis->m_uLastStreamSize) ;
	
	if(TotalBytesTransferred.HighPart==TotalFileSize.HighPart && 
		TotalBytesTransferred.LowPart==TotalFileSize.LowPart)
	{
		pThis->m_uFileCount++ ;
	}

	pThis->m_uLastStreamSize = uCurStreamSize ;
	::LeaveCriticalSection(&pThis->m_Lock) ;

	return PROGRESS_CONTINUE ;
}

BOOL CECLComparionDemoDlg::GetTaskFolders(CString& strSrcFolder,CString& strDstFolder) 
{
	BOOL bRet = TRUE ;

	this->GetDlgItemText(IDC_EDIT_SOURCE,strSrcFolder) ;
	this->GetDlgItemText(IDC_EDIT_DESTINATION,strDstFolder) ;

	if(strSrcFolder.GetLength()==0) 
	{
		AfxMessageBox(_T("please specify source folder")) ;
		bRet = FALSE ;
	}
	else if (strDstFolder.GetLength()==0)
	{
		AfxMessageBox(_T("please specify destination folder")) ;
		bRet = FALSE ;
	}

	return bRet ;
}

CString CECLComparionDemoDlg::ExtractFileName(CString strFile) 
{
	CString strRet;

	for(int i=strFile.GetLength()-1;i>0;--i)
	{
		TCHAR ch = strFile.GetAt(i) ;

		if(ch=='\\' || ch=='/')
		{
			strRet = strFile.Right(strFile.GetLength()-i-1) ;
			break ;
		}
	}

	return strRet ;
}

void CECLComparionDemoDlg::EnableAllButton(BOOL bEnable) 
{
	this->GetDlgItem(IDC_BUTTON_ADDFOLDERSRC)->EnableWindow(bEnable) ;
	this->GetDlgItem(IDC_BUTTON_CLEARSOURCE)->EnableWindow(bEnable) ;
	this->GetDlgItem(IDC_BUTTON_ADDFOLDERDST)->EnableWindow(bEnable) ;
	this->GetDlgItem(IDC_BUTTON_CLEARDST)->EnableWindow(bEnable) ;
	this->GetDlgItem(IDC_BUTTON_WINCOPY)->EnableWindow(bEnable) ;
	this->GetDlgItem(IDC_BUTTON_WINMOVE)->EnableWindow(bEnable) ;
	this->GetDlgItem(IDC_BUTTON_EXTREMECOPY)->EnableWindow(bEnable) ;
	this->GetDlgItem(IDC_BUTTON_EXTREMEMOVE)->EnableWindow(bEnable) ;
}

void CECLComparionDemoDlg::UpdateStatus(EDemoStatus NewStatus) 
{
	CString strStatus ;

	switch(NewStatus)
	{
	case DS_Running: strStatus = _T("Running") ; break ;
	case DS_Finished : strStatus = _T("Finished") ; break ;
	case DS_Failed: strStatus = _T("Failed") ; break ;

	default: _ASSERT(FALSE) ; return ;
	}

	this->SetDlgItemText(m_bIsWinCopy ? IDC_STATIC_WINSTATUS : IDC_STATIC_EXTREMECOPYSTATUS ,strStatus) ;
}

void CECLComparionDemoDlg::UpdateStaToUI() 
{
	CString strTime ;
	CString strFiles ;
	CString strSize ;

	::EnterCriticalSection(&m_Lock) ;

	unsigned __int64 uDoneSize = m_uDoneSize ;
	strTime.Format(_T("%d:%02d:%02d"),m_uElapsedTimeCount/(60*60),m_uElapsedTimeCount%(60*60)/60,m_uElapsedTimeCount%60) ;
	strFiles.Format(_T("%u"),m_uFileCount) ;

	::LeaveCriticalSection(&m_Lock) ;

	if(uDoneSize>1024*1024*1024)
	{// 
		unsigned __int64 nTem = (unsigned __int64)1024*1024*1024 ;

		nTem = nTem * 1024 ;

		if(uDoneSize>nTem)
		{// T
			const float f = uDoneSize/(nTem*1.0f) ;
			strSize.Format(_T("%.1f T"),f) ;
		}
		else
		{// G
			const float f = uDoneSize/(1024.0f*1024*1024) ;
			strSize.Format(_T("%.1f G"),f) ;
		}
	}
	else
	{// 
		if(uDoneSize>1024*1024)
		{// M
			const float f = uDoneSize/ (1024.0f*1024.0f) ;
			strSize.Format(_T("%.1f M"),f) ;
		}
		else
		{// K
			const float f = uDoneSize/ 1024.0f ;
			strSize.Format(_T("%.1f K"),f) ;
		}
	}

	this->SetDlgItemText(m_bIsWinCopy ? IDC_STATIC_WINTIME : IDC_STATIC_EXTREMECOPYTIME ,strTime) ;
	this->SetDlgItemText(m_bIsWinCopy ? IDC_STATIC_WINSIZE : IDC_STATIC_EXTREMECOPYSIZE ,strSize) ;
	this->SetDlgItemText(m_bIsWinCopy ? IDC_STATIC_WINFILE : IDC_STATIC_EXTREMECOPYFILE ,strFiles) ;
}


