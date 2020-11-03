/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "ptDiskManager.h"
#include "Winioctl.h"

#include <stdio.h>
#include <tchar.h>
#include "ptRegistry.h"

//#pragma warning(disable:4996)
//#include "shlwapi.h"
//#pragma comment(lib,"shlwapi.lib")

CptDiskManager::CptDiskManager(void)
{
}

CptDiskManager::~CptDiskManager(void)
{
}


int CptDiskManager::GetPartitionIndex(const TCHAR* pStr) 
{
	if(pStr[1]==':')
	{
		int nIndex = _totupper(pStr[0])-'A' ;

		if(nIndex>=0 && nIndex<26)
		{
			return nIndex ;
		}
	}
	return -1 ;
}

/**
ULONG CptDiskManager::GetMountPointDriveType(LPCTSTR lpRootPathName)
{
    TCHAR lpszVolumeName[MAX_PATH];

    //SetLastError(ERROR_SUCCESS);
    if (lpRootPathName == NULL)
    {
        return GetDriveType(NULL);
    } 
	else if (!::GetVolumeNameForVolumeMountPoint(lpRootPathName, 
        lpszVolumeName,MAX_PATH))
    {
        return DRIVE_NO_ROOT_DIR;
    }

	return ::GetDriveType(lpszVolumeName);
}
/**/

int CptDiskManager::GetDiskIndex(const TCHAR* pStr) 
{
	int nRet = -1 ;

	int nLetter = pStr[0] ;

	if((nLetter>='a' && nLetter<='z') || (nLetter>='A' && nLetter<='Z'))
	{//GetValue("HKEY_Local_MachineHardWareDevicemapScsiScsiport0ScsiBus0TargetId1LogicalUnitId0","Identifier","cuowu")
		{
			HANDLE   hDevice;  
			DWORD   bytesReturned;  

			TCHAR   rawDiskName[16] = {0} ;

			#pragma warning(push)
#pragma warning(disable:4996)
			::_stprintf(rawDiskName,_T("\\\\.\\%c:"),nLetter) ;
#pragma warning(pop)

			//hDevice = ::CreateFile(rawDiskName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);  
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
		}
	}
	else
	{
		nRet = 65533 ;
	}
	
	return nRet ;
}

int CptDiskManager::GetDiskSector(const TCHAR* pStr)
{
	int nRet = 512 ;

	DWORD	spc, bps, fc, cl;

	if(::GetDiskFreeSpace(pStr, &spc, &bps, &fc, &cl))
	{
		nRet = bps ;
	}

	return nRet ;
}

DWORD CptDiskManager::GetDiskID(TCHAR cLetter) 
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

int CptDiskManager::GetDiskSector(int nPartitionIndex) 
{
	int nRet = 512 ;

	if(nPartitionIndex<=0 && nPartitionIndex>=26)
	{
		DWORD	spc, bps, fc, cl;

		TCHAR root[16] = {0} ;
#pragma warning(push)
#pragma warning(disable:4996)
		::_stprintf(root,_T("%c:\\"),nPartitionIndex+L'A') ;
#pragma warning(pop)
		//::_stprintf(root,_T("\\\\"),nPartitionIndex+L'A') ;

		if(GetDiskFreeSpace(root, &spc, &bps, &fc, &cl))
		{
			nRet = bps ;
		}
	}

	return nRet ;
}



//bool CptDiskManager::IsSameDisk(const char* pStr1,const char* pStr2) 
//{
//	if(pStr1[1]==pStr2[1] && pStr1[1]==':')
//	{
//		int nLetter1 = toupper(pStr1[0]) ;
//		int nLetter2 = toupper(pStr2[0]) ;
//
//		int	idx1 = nLetter1-'A';
//		int	idx2 = nLetter2-'A';
//	}
//	else if(pStr1[1]!=pStr2[1] || ::GetDriveType(pStr1) != ::GetDriveType(pStr2))
//	{
//		return false ;
//	}
//
//	return true ;
//}