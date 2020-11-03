/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "skindialog.h"
#include "ptSkinButton.h"
#include "CptComboBox.h"
#include "..\XCGlobal.h"

class CConfigurationDialog :
	public CptDialog
{
public:
	CConfigurationDialog(void);
	~CConfigurationDialog(void);

	virtual BOOL OnInitDialog() ;
	virtual void OnButtonClick(int nButtonID) ;

private:
	void UpdateDataToControl() ; 
	void UpdateDataFromControl() ;

	void IniUI() ;

	void SetSameFileProcCheck(SRichCopySelection::ESameFileProcessType sfpt) ;
	void SetErrorProcCheck(SRichCopySelection::EErrorProcessType ept) ;
	void SetRetryFailThenCheck(SRichCopySelection::EErrorProcessType sfpt) ;

	void SetRadioCheck(pt_STL_vector(UINT) CtrlIDVer,UINT nCheckedID) ;

private:

	SConfigData			m_ConfigData ;
	SXCTaskAndConfigShare	m_ConfigShare ;
	CptComboBox			m_UITypeComboBox ;
	CptComboBox			m_LanguageComboBox ;
	CptComboBox			m_FileDataBufComboBox ;

	CptComboBox			m_SFIfConditonComboBox ;
	CptComboBox			m_SFThenOperationComboBox ;
	CptComboBox			m_SFOtherwiseOperationComboBox ;

	typedef pt_STL_map(std::basic_string<TCHAR>,std::basic_string<TCHAR>)	Str2StrMap_t ;

	Str2StrMap_t		m_LanguageName2DLLNameMap ;
	int					m_nPreLanguageIndex ;
	int					m_nCurLanguageIndex ;
};
