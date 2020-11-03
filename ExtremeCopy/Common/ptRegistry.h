/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#pragma once

#include <windows.h>

class CptRegistry
{
public:
	enum ERegistryRoot
	{
		CLASSES_ROOT = 1,
		CURRENT_USER,
		LOCAL_MACHINE,
		USERS,
		UNKNOWN
	};

	struct SRegInfo
	{
		TCHAR lpClass[MAX_PATH] ;
		DWORD dwSubKeyCount ;
		DWORD dwValue ;
		DWORD dwMaxNameLen ;
		DWORD dwMaxValueLen ;
		SYSTEMTIME LastWriteTime ;
	};

public:
	CptRegistry(const CptRegistry&) ;
	CptRegistry(void);
	~CptRegistry(void);

	CptRegistry& operator=(const CptRegistry&) ;
	bool IsOpen() const ;
	void Close() ;

	bool OpenKey(ERegistryRoot root,LPCTSTR lpSubKey,REGSAM samDesired=KEY_ALL_ACCESS) ;

	bool CreateKey(LPCTSTR lpSubKey,CptRegistry* pReg=NULL,DWORD dwOptions=REG_OPTION_NON_VOLATILE,
			REGSAM samDesired=KEY_ALL_ACCESS,LPSECURITY_ATTRIBUTES lpSA=NULL) ;

	bool GetSubKeyName(int nSubKeyIndex,TCHAR* lpNameBuf,DWORD dwBufSize) ;

	bool DeleteKey(LPCTSTR lpSubKey) ;

	bool DeleteTree(LPCTSTR lpSubKey) ;

	bool GetInfo(SRegInfo& ri) ;

	bool SaveToFile(LPCTSTR lpFileName) ;

	// value
	bool SetValueInt(LPCTSTR lpValueName,DWORD nValue) ;
	bool GetValueInt(LPCTSTR lpValueName,DWORD& nValue) ;

	bool SetValueString(LPCTSTR lpValueName,LPCTSTR lpString) ;
	bool GetValueString(LPCTSTR lpValueName,LPTSTR lpString,DWORD dwBufSize) ;

	bool SetValue(LPCTSTR lpValueName,DWORD dwType,const BYTE* lpData,DWORD cbData) ;
	bool GetValue(LPCTSTR lpValueName,DWORD& dwType,BYTE* lpData,DWORD& cbData) ;

	bool DeleteValue(LPCTSTR lpValueName) ;

private:
	bool OpenKey(HKEY hRootKey,LPCTSTR lpSubKey) ;

private:
	HKEY			m_hKey ;
};
