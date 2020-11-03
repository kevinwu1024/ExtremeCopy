/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "../Common/ptString.h"

class CptMultipleLanguage
{
public:

	static CptMultipleLanguage* GetInstance(const TCHAR* pDLL=NULL)  ;
	static void Release() ;

	 CptString GetString(int nResourceID) ;
	 HBITMAP GetBitmap(int nResourceID) ;
	 HICON GetIcon(int nResourceID) ;
	 HCURSOR GetCursor(int nResourceID) ;
	 HMENU GetMenu(int nResourceID) ;

	 HINSTANCE GetResourceHandle() const ;

	 bool LoadResourceLibary(const TCHAR* pDLL,HMODULE hModule=NULL) ;

protected:
	void FreeCurrentLibary() ;

protected:
	CptMultipleLanguage(void);
	~CptMultipleLanguage(void);

private:
	HINSTANCE	m_hInst ;
	static CptMultipleLanguage*		m_pInstance ;
};

