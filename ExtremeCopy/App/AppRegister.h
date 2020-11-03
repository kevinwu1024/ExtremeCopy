/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "UI\RegisterDlg.h"
#include "UI\RegisterCountdownDlg.h"

class CAppRegister
{
private:
	struct SRegisterInfo
	{
		BYTE	RandomArray1[5] ;

		int nRandom1			:	3 ;
		int nFirstTimeSeconds	:	5 ;
		int nRandom2			:	7 ;
		int nFirstDay			:	7 ;
		int nRandom3			:	7 ;
		int nVersionMin			:	3 ;

		BYTE	RandomArray2[27] ;

		time_t	nTime ;
		int nRandom4			:	5 ;
		int nFirstMonth			:	5 ;
		int nFirstMinute		:	7 ;
		int bRegister			:	1 ;
		int bPrompt				:	1 ;
		int nRandom5			:	13 ;

		BYTE	RandomArray3[33] ;

		int nFirstYear			:	12 ;
		int nMajorVersion		:	3 ;
		int nFirstHour			:	7 ;
		int nRandom6			:	10 ;

		BYTE	RandomArray4[17] ;
	};

public:
	CAppRegister(void);
	~CAppRegister(void);

	bool CheckRegister(HWND hParentWnd) ;
	bool ShowCountDownDlg() ;
	bool ShowRegisterDlg(HWND hParentWnd) ;

	bool IsExpired()  ;
	bool IsAppRegisted() const;
	bool IsAppRegistedReadOnly() ;

private:
	bool Register(CptString strSN) ;
	SYSTEMTIME GetBeginDate(bool bRewrite=true) ;
	void WriteRegInfoToFile() ;

private:
	SRegisterInfo		m_RegInfo ;
	int					m_nTryTimes ;
	HANDLE				m_hFile ;

};
