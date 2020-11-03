
// ECLComparionDemoDlg.h : header file
//

#pragma once


// CECLComparionDemoDlg dialog
class CECLComparionDemoDlg : public CDialogEx
{
// Construction
public:
	CECLComparionDemoDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CECLComparionDemoDlg() ;
// Dialog Data
	enum { IDD = IDD_ECLCOMPARIONDEMO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	public:
		afx_msg void OnBnClickedButtonAddfoldersrc();
		afx_msg void OnBnClickedButtonClearsource();
		afx_msg void OnBnClickedButtonAddfolderdst();
		afx_msg void OnBnClickedButtonCleardst();
		afx_msg void OnTimer(UINT_PTR nIDEvent);
		afx_msg void OnBnClickedButtonWincopy();
		afx_msg void OnBnClickedButtonWinmove();
		afx_msg void OnBnClickedButtonExtremecopy();
		afx_msg void OnBnClickedButtonExtrememove();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	
private:
#define	WM_DEMOTASKRUN (WM_USER+1)		

	typedef enum _tagEDemoStatus
	{
		DS_Running,
		DS_Finished,
		DS_Failed
	}EDemoStatus;

	typedef enum _tagETaskType
	{
		TT_WinCopy,
		TT_WinMove,
		TT_ExtremeCopy,
		TT_ExtremeMove
	}ETaskType ;

	static DWORD __stdcall CopyProgressRoutine(
		LARGE_INTEGER TotalFileSize,
		LARGE_INTEGER TotalBytesTransferred,
		LARGE_INTEGER StreamSize,
		LARGE_INTEGER StreamBytesTransferred,
		DWORD dwStreamNumber,
		DWORD dwCallbackReason,
		HANDLE hSourceFile,
		HANDLE hDestinationFile,
		LPVOID lpData
		);

	static int __stdcall ExtremeCopyRoutine(int nCmd,void* pRoutineParam,int nParam1,int nParam2,const TCHAR* pSrcFile,const TCHAR* pDstFile) ;

	static unsigned __stdcall TaskThreadFunc(void* param) ;

	CString SelectFolder() ;

	BOOL WinCopy(CString strSrcDirectory,CString strDstDirectory) ;
	BOOL WinMove(CString strSrcDirectory,CString strDstDirectory) ;
	BOOL ExtremeCopy(CString strSrcDirectory,CString strDstDirectory,BOOL bCopyOrMove);
	//BOOL ExtremeMove(CString strSrcDirectory,CString strDstDirectory);

	CString ExtractFileName(CString strDirectory) ;
	void EnableAllButton(BOOL bEnable) ;
	void UpdateStatus(EDemoStatus NewStatus) ;
	void UpdateStaToUI() ;
	BOOL RunTask(ETaskType tt) ;

	BOOL GetTaskFolders(CString& strSrcFolder,CString& strDstFolder) ;

#define TIMER_ONE_SECONDE	1000

private:
	CRITICAL_SECTION	m_Lock ;
	unsigned			m_uElapsedTimeCount ;

	unsigned __int64	m_uLastStreamSize ;
	unsigned __int64	m_uDoneSize ;
	unsigned			m_uFileCount ;

	bool		m_bIsWinCopy ;
	
	DWORD				m_dwTaskThreadID ;
	HANDLE				m_hTaskThread ;

public:
	afx_msg void OnBnClickedCancel();
};
