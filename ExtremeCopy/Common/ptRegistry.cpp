/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptRegistry.h"

CptRegistry::CptRegistry(void):m_hKey(NULL)//,m_CurRootName(UNKNOWN)
{
}

CptRegistry::~CptRegistry(void)
{
	this->Close() ;
}


bool CptRegistry::IsOpen() const 
{
	return (m_hKey!=NULL) ;
}

void CptRegistry::Close() 
{
	if(m_hKey!=NULL)
	{
		::RegCloseKey(m_hKey) ;
		m_hKey = NULL ;

//	m_CurRootName = UNKNOWN ;
	}
	
}


bool CptRegistry::OpenKey(ERegistryRoot root,LPCTSTR lpSubKey,REGSAM samDesired) 
{
	bool bRet = false ;

	HKEY hRootKey = NULL ;

	switch(root)
	{
	case CLASSES_ROOT:
		hRootKey = HKEY_CLASSES_ROOT ;
		break ;

	case CURRENT_USER:
		hRootKey = HKEY_CURRENT_USER ;
		break ;

	case LOCAL_MACHINE:
		hRootKey = HKEY_LOCAL_MACHINE ;
		break ;

	case USERS:
		hRootKey = HKEY_USERS ;
		break ;
	}

	if(hRootKey!=NULL)
	{
		this->Close() ;

		bRet = (::RegOpenKeyEx(hRootKey,lpSubKey,0,samDesired,&m_hKey)==ERROR_SUCCESS);
	}

	return bRet ;
}

bool CptRegistry::CreateKey(LPCTSTR lpSubKey,CptRegistry* pReg,DWORD dwOptions,REGSAM samDesired,LPSECURITY_ATTRIBUTES lpSA) 
{
	bool bRet = false ;

	if(m_hKey!=NULL)
	{
		HKEY hResult = NULL ;
		
		if(::RegCreateKeyEx(m_hKey,lpSubKey,0,NULL,dwOptions,samDesired,lpSA,&hResult,NULL)==ERROR_SUCCESS)
		{
			if(pReg!=NULL)
			{
				pReg->Close() ;
				pReg->m_hKey = hResult ;
			}
			else
			{
				::RegCloseKey(hResult) ;
				hResult = NULL ;
			}

			bRet = true ;
		}
	}

	return bRet ;
}

bool CptRegistry::DeleteKey(LPCTSTR lpSubKey) 
{
	bool bRet = false ;

	if(m_hKey!=NULL)
	{
		bRet = (::RegDeleteKey(m_hKey,lpSubKey) ==ERROR_SUCCESS);
	}

	return bRet ;
}

bool CptRegistry::DeleteValue(LPCTSTR lpValueName)
{
	bool bRet = false ;

	if(m_hKey!=NULL)
	{
		bRet = (::RegDeleteValue(m_hKey,lpValueName) ==ERROR_SUCCESS);
	}

	return bRet ;
}

bool CptRegistry::SetValue(LPCTSTR lpValueName,DWORD dwType,const BYTE* lpData,DWORD cbData)
{
	bool bRet = false ;

	if(m_hKey!=NULL)
	{
		bRet = (::RegSetValueEx(m_hKey,lpValueName,0,dwType,lpData,cbData) ==ERROR_SUCCESS);
	}

	return bRet ;
}

bool CptRegistry::SetValueString(LPCTSTR lpValueName,LPCTSTR lpString) 
{
	return this->SetValue(lpValueName,REG_SZ,(const BYTE*)lpString,(DWORD)::_tcslen(lpString)*sizeof(TCHAR)) ;
}

bool CptRegistry::SetValueInt(LPCTSTR lpValueName,DWORD nValue)
{
	return this->SetValue(lpValueName,REG_DWORD,(const BYTE*)&nValue,sizeof(nValue)) ;
}

bool CptRegistry::GetValue(LPCTSTR lpValueName,DWORD& dwType,BYTE* lpData,DWORD& cbData)
{
	bool bRet = false ;

	if(m_hKey!=NULL)
	{
		bRet = (::RegQueryValueEx(m_hKey,lpValueName,0,&dwType,lpData,&cbData) ==ERROR_SUCCESS);
	}

	return bRet ;
}

bool CptRegistry::GetValueInt(LPCTSTR lpValueName,DWORD& nValue) 
{
	DWORD dwSize = sizeof(nValue) ;
	DWORD dwType = 0  ;

	return (this->GetValue(lpValueName,dwType,(BYTE*)&nValue,dwSize) && (dwType==REG_DWORD)) ;
}

bool CptRegistry::GetValueString(LPCTSTR lpValueName,LPTSTR lpString,DWORD dwBufSize) 
{
	DWORD dwType = 0  ;

	return (this->GetValue(lpValueName,dwType,(BYTE*)lpString,dwBufSize) && (dwType==REG_SZ)) ;
}

bool CptRegistry::GetInfo(SRegInfo& ri)
{
	bool bRet = false ;

	if(m_hKey!=NULL)
	{
		DWORD dwClassSize = sizeof(ri.lpClass) ;
		FILETIME ft ;

		bRet = (::RegQueryInfoKey(m_hKey,ri.lpClass,&dwClassSize,NULL,&ri.dwSubKeyCount,&ri.dwMaxNameLen,NULL,&ri.dwValue,NULL,&ri.dwMaxValueLen,NULL,&ft) ==ERROR_SUCCESS);

		if(bRet)
		{
			::FileTimeToSystemTime(&ft,&ri.LastWriteTime) ;
		}
	}

	return bRet ;
}

bool CptRegistry::SaveToFile(LPCTSTR lpFileName)
{
		bool bRet = false ;

	if(m_hKey!=NULL && lpFileName!=NULL)
	{
		//int aa = ::RegSaveKey(m_hKey,lpFileName,NULL) ;

		bRet = (::RegSaveKey(m_hKey,lpFileName,NULL)==ERROR_SUCCESS);
	}

	return bRet ;
}

bool CptRegistry::GetSubKeyName(int nSubKeyIndex,TCHAR* lpNameBuf,DWORD dwBufSize)
{
	bool bRet = false ;

	if(m_hKey!=NULL)
	{
		bRet = (::RegEnumKeyEx(m_hKey,nSubKeyIndex,lpNameBuf,&dwBufSize,NULL,NULL,NULL,NULL)==ERROR_SUCCESS);
	}

	return bRet ;
}

bool CptRegistry::OpenKey(HKEY hRootKey,LPCTSTR lpSubKey)
{
	bool bRet = false ;

//	if(m_hKey==NULL)
	{
		this->Close() ;

		bRet = (::RegOpenKeyEx(hRootKey,lpSubKey,0,KEY_ALL_ACCESS,&m_hKey)==ERROR_SUCCESS);
	}

	return bRet ;
}

bool CptRegistry::DeleteTree(LPCTSTR lpSubKey)
{
	bool bRet = false ;

	if(m_hKey!=NULL)
	{
		DWORD dwSubKeyCount = 0 ;

		if(::RegQueryInfoKey(m_hKey,NULL,NULL,NULL,&dwSubKeyCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
		{
			bRet = true ;
			HKEY hKey = NULL ;
			TCHAR szKeyName[256] = {0} ;
			DWORD dwKeySize = 0;

			for(DWORD i=0;i<dwSubKeyCount;++i)
			{// 遍历所有子键
				dwKeySize = sizeof(szKeyName)/sizeof(TCHAR) ;

				if(::RegEnumKeyEx(m_hKey,i,szKeyName,&dwKeySize,NULL,NULL,NULL,NULL) ==ERROR_SUCCESS)
				{
					CptRegistry reg ;

					if(reg.OpenKey(m_hKey,lpSubKey))
					{// 打开子键
						SRegInfo ri ;

						if(reg.GetInfo(ri))
						{
							for(DWORD j=0;j<ri.dwSubKeyCount;++j)
							{// 遍历子键的子键
								dwKeySize = sizeof(szKeyName)/sizeof(TCHAR) ;

								if(reg.GetSubKeyName(j,szKeyName,dwKeySize))
								{
									reg.DeleteTree(szKeyName) ;
								}

							}
						}
					}

					reg.Close() ;

					this->DeleteKey(lpSubKey) ; // 删除子键
				}
				
			}
		}
	}
/**
	if(m_hKey!=NULL)
	{
		char	buf[100];
		BOOL	ret = TRUE;

		if (lpSubKey != NULL && !OpenKey(subKey))
			return	false;

		while (::EnumKey(0, buf, sizeof(buf)))
		{
			if ((ret = DeleteTree(buf)) == false)
				break;
		}
		if (lpSubKey != NULL)
		{
			//CloseKey();
			this->Close() ;
			ret = DeleteKey(subKey) ? ret : FALSE;
		}
		else 
		{
			while (EnumValue(0, buf, sizeof(buf)))
			{
				if (DeleteValue(buf) == FALSE)
				{
					ret = FALSE;
					break;
				}
			}
		}
		return	ret;
		//bRet = (::RegDeleteTree (m_hKey,lpSubKey) ==ERROR_SUCCESS);
	}
/**/
	return bRet ;
}