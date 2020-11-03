/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCWinStorageRelative.h"
#include "..\Common\ptString.h"
#include <Winioctl.h>

pt_STL_map(TCHAR,SStorageInfoOfFile)		CXCWinStorageRelative::m_PartitionLetter2SotrageInfoMap ;

CXCWinStorageRelative::CXCWinStorageRelative(void)
{
}


CXCWinStorageRelative::~CXCWinStorageRelative(void)
{
}

bool CXCWinStorageRelative::MakeUnlimitFileName(CptString& strFileOrDirectory)
{
	bool bRet = false ;

	if(strFileOrDirectory.GetLength()>3)
	{
		if(strFileOrDirectory.GetAt(0) == '\\')
		{// 网络名
			strFileOrDirectory = _T("\\\\?\\UNC") + strFileOrDirectory ;
		}
		else
		{// 逻辑磁盘名
			strFileOrDirectory = _T("\\\\?\\") + strFileOrDirectory ;
		}
	}

	return bRet ;
}

void CXCWinStorageRelative::RemoveUnlimitFileNameSymbol(CptString& strFileOrDirectory)
{
	if(IsUnlimitFileName(strFileOrDirectory))
	{
		strFileOrDirectory = strFileOrDirectory.Delete(0,4) ;
	}
}

bool CXCWinStorageRelative::IsUnlimitFileName(const CptString& strFileOrDirectory) 
{
	return (strFileOrDirectory.GetLength()>=7 && strFileOrDirectory.Left(4).Compare(_T("\\\\?\\"))==0) ;
}

bool CXCWinStorageRelative::GetFileStoreInfo(const CptString& strPath,SStorageInfoOfFile& siof)
{
	_ASSERT(strPath.GetLength()>2) ;

	bool bRet = false ;

	TCHAR cPartitionLetter = 0 ;
	TCHAR cPartitionLetter2 = 0 ;

	if(CXCWinStorageRelative::IsUnlimitFileName(strPath))
	{
		cPartitionLetter = strPath[4] ;
		cPartitionLetter2 = strPath[5] ;
	}
	else
	{
		cPartitionLetter = strPath[0] ;
		cPartitionLetter2 = strPath[1] ;
	}

	if((cPartitionLetter>='a' && cPartitionLetter<='z') || (cPartitionLetter>='A' && cPartitionLetter<='Z'))
	{
		if(cPartitionLetter>='a' && cPartitionLetter<='z')
		{// 把这个字符转为大写
			cPartitionLetter = cPartitionLetter - 'a' + 'A' ;
		}

		pt_STL_map(TCHAR,SStorageInfoOfFile)::iterator it = m_PartitionLetter2SotrageInfoMap.find(cPartitionLetter) ;

		if(it!=m_PartitionLetter2SotrageInfoMap.end())
		{
			siof = (*it).second ;
		}
		else
		{
			TCHAR root[16] = {0} ;

			{// sector 大小
				siof.nSectorSize = 512 ;

				DWORD	spc, bps, fc, cl;

#pragma warning(push)
#pragma warning(disable:4996)
				::_stprintf(root,_T("%c:\\"),cPartitionLetter) ;
#pragma warning(pop)

				if(::GetDiskFreeSpace(root, &spc, &bps, &fc, &cl))
				{
					siof.nSectorSize = bps ;
				}
			}

			//siof.nSotrageIndex = CXCWinStorageRelative::GetDiskIndex(cPartitionLetter) ;
			siof.dwStorageID = CXCWinStorageRelative::GetDiskID(cPartitionLetter) ;
			siof.nPartitionIndex = cPartitionLetter - 'A' ;
			siof.uDiskType = ::GetDriveType(root) ;

			m_PartitionLetter2SotrageInfoMap[cPartitionLetter] = siof ;
		}

		bRet = true ;
	}
	else if(cPartitionLetter=='\\' && cPartitionLetter2=='\\')
	{// 网络文件（夹）
		siof.nPartitionIndex = -1 ;
		siof.dwStorageID = 0 ;
//		siof.nSotrageIndex = -1 ;
		siof.nSectorSize = 512 ;
		siof.uDiskType = DRIVE_UNKNOWN ;
	}
	else
	{
		_ASSERT(FALSE) ;
	}

	return bRet ;
}

DWORD CXCWinStorageRelative::GetDiskID(TCHAR cLetter) 
{
	CptRegistry reg ;
	DWORD dwRet = 0 ;

	if(reg.OpenKey(CptRegistry::LOCAL_MACHINE,_T("SYSTEM\\MountedDevices"),KEY_READ))
	{
		TCHAR szDosName[64+1] = {0} ;

#pragma warning(push)
#pragma warning(disable:4996)
		::_stprintf(szDosName,_T("\\DosDevices\\%c:"),cLetter) ;
#pragma warning(pop)

		BYTE		byDataBuf[1024];
		DWORD dwType = 0 ;
		DWORD nBufSize = sizeof(byDataBuf) ;

		if(reg.GetValue(szDosName,dwType,byDataBuf,nBufSize))
		{
			TCHAR* wbuf = (TCHAR *)byDataBuf;

			if (::_tcsncmp(wbuf, L"\\??\\", 4) == 0 && (wbuf = ::_tcschr(wbuf, '#')) && (wbuf = ::_tcschr(wbuf+1, '#')) && (wbuf = ::_tcschr(wbuf, '&'))) 
			{
				dwRet = _tcstoul(wbuf+1, 0, 16);
			}
			else
			{
				dwRet = *(DWORD *)byDataBuf;
			}
		}

		reg.Close() ;
	}

	return dwRet ;
}

int CXCWinStorageRelative::GetDiskIndex(TCHAR cLetter)
{
	int nRet = -1 ;

	HANDLE   hDevice;  
	DWORD   bytesReturned;  

	TCHAR   rawDiskName[16] = {0} ;

#pragma warning(push)
#pragma warning(disable:4996)
	::_stprintf(rawDiskName,_T("\\\\.\\%c:"),cLetter) ;
#pragma warning(pop)

	hDevice = ::CreateFile(rawDiskName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);  

	if (hDevice != INVALID_HANDLE_VALUE) 
	{
		STORAGE_DEVICE_NUMBER   deviceInfo;  

		if(::DeviceIoControl(hDevice,    
			IOCTL_STORAGE_GET_DEVICE_NUMBER,
			//IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,  
			NULL,  
			0,    
			&deviceInfo,    
			sizeof(deviceInfo),  
			&bytesReturned,  
			NULL))
		{
			nRet = deviceInfo.DeviceNumber ;
		}

		CloseHandle(hDevice); 
	}
	
	return nRet ;
}