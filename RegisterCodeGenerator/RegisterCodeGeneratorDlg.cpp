/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "stdafx.h"
#include "RegisterCodeGenerator.h"
#include "RegisterCodeGeneratorDlg.h"

void GetRegisterCodeInString(CptStringList& sl,int prog,int nCount) ;
bool CheckRegisterCode2(const CptStringList& sl,int prog) ;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CRegisterCodeGeneratorDlg::CRegisterCodeGeneratorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterCodeGeneratorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRegisterCodeGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LICENSEOFPROGRAM, m_LicenOfProgComboBox);
}

BEGIN_MESSAGE_MAP(CRegisterCodeGeneratorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CREATE, &CRegisterCodeGeneratorDlg::OnBnClickedButtonCreate)
END_MESSAGE_MAP()


// CRegisterCodeGeneratorDlg message handlers

BOOL CRegisterCodeGeneratorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_LicenOfProgComboBox.AddString(this->GetLicenseOfProgramDisplayString(0)) ;
	m_LicenOfProgComboBox.AddString(this->GetLicenseOfProgramDisplayString(1)) ;
	m_LicenOfProgComboBox.AddString(this->GetLicenseOfProgramDisplayString(2)) ;

	m_LicenOfProgComboBox.SetCurSel(1) ;

	this->SetDlgItemInt(IDC_EDIT_COUNT,10) ;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRegisterCodeGeneratorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	//if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	//{
	//	CAboutDlg dlgAbout;
	//	dlgAbout.DoModal();
	//}
	//else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRegisterCodeGeneratorDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRegisterCodeGeneratorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CptString CRegisterCodeGeneratorDlg::GetLicenseOfProgramDisplayString(int prog) 
{
	switch(prog)
	{
	case 0: return _T("ExtremeCopy Pro 1.x") ;
	case 1: return _T("ExtremeCopy Pro 2.0") ;
	case 2: return _T("ExtremeCopy Library 2.0") ;
	default: return _T("") ;
	}
}

void CRegisterCodeGeneratorDlg::OnBnClickedButtonCreate()
{
	// TODO: Add your control notification handler code here

	int nCount = this->GetDlgItemInt(IDC_EDIT_COUNT) ;
	int prog = m_LicenOfProgComboBox.GetCurSel() ;

	CptStringList sl ;

	GetRegisterCodeInString(sl,prog,nCount) ;
	CptString strTxt ;

	_ASSERT(sl.GetCount()==nCount) ;

	for(int i=0;i<sl.GetCount();++i)
	{
		//
		strTxt += sl[i] + _T("\r\n") ;

		{
			CptStringList slSection ;

			slSection.Split(sl[i],'-') ;

			//_ASSERT(CheckRegisterCode2(slSection,prog)) ;
			if(!CheckRegisterCode2(slSection,prog))
			{
				AfxMessageBox(_T("注册码 创建和检验不一致错误!")) ;
				return ;
			}
		}
		
	}

	this->SetDlgItemText(IDC_EDIT_SERIESNUMBER,strTxt.c_str()) ;
}
