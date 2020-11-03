/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCStatistics.h"


CXCStatistics::CXCStatistics() 
{
	m_FolderSizeDone = false ;
	m_bCalledUIIni = false ;
}

CXCStatistics::~CXCStatistics() 
{
	this->Stop() ;
}

bool CXCStatistics::Start(const pt_STL_vector(CptString)& FolderVer,CXCStatisticsCB* pCallback)
{
	if(FolderVer.empty() || m_FolderSize.GetState()!=CptFolderSize::State_Stop)
	{
		return false ;
	}

	m_pCallback = pCallback ;
	m_FolderSizeDone = false ;
	::memset(&m_StatValue,0,sizeof(m_StatValue)) ;

	bool bRet = m_FolderSize.Start(FolderVer,this) ;

	return bRet ;
}

void CXCStatistics::Stop() 
{
	m_FolderSize.Stop() ;
	::memset(&m_StatValue,0,sizeof(m_StatValue)) ;
}

const SStatisticalValue& CXCStatistics::GetStaData() const 
{
	return m_StatValue ;
}

bool CXCStatistics::DoesFolderSizeDone() const
{
	return m_FolderSizeDone ;
}


void CXCStatistics::SetCurWholeSize(const unsigned __int64& uCurWholeSize) 
{
	//Release_Printf(_T("CXCStatistics::SetCurWholeSize() uCurWholeSize=%u"),(unsigned)uCurWholeSize) ;

	_ASSERT(uCurWholeSize>0) ;

	m_StatValue.uCurWholeSize = uCurWholeSize ;
	m_StatValue.uCurOperateSize = 0 ;
}

void CXCStatistics::IncreseDoneFileCount(int nCount) 
{
	m_StatValue.nDoneFiles += nCount ;
}

void CXCStatistics::IncreaseSkipFileCount(int nCount) 
{
	m_StatValue.nSkipFiles += nCount ;
}

unsigned __int64 CXCStatistics::GetCurFileRemainSize() const 
{
	return m_StatValue.uCurWholeSize - m_StatValue.uCurOperateSize ;
}

void CXCStatistics::SetDataOccured(const unsigned& uSize,bool bReadOrWrite) 
{
	if(!bReadOrWrite)
	{
		m_SpeedData.SetCurSecondData(uSize) ;
	}

	if(!bReadOrWrite)
	{// 只有写入才记录
		m_StatValue.uTransSize += uSize ;
		m_StatValue.uCurOperateSize += uSize ;
	}
}

void CXCStatistics::SetVerifyDataOccured(const unsigned int& uSize) 
{
	m_StatValue.uVerifyDataSize += uSize ;
}

void CXCStatistics::MoveNextSecond() 
{
	++m_StatValue.nLapseTime ;

	if(m_StatValue.uTransSize>0)
	{
		m_StatValue.fSpeed = m_SpeedData.MoveNextSecond() ;
	}
}

// 文件夹大小回调事件
void CXCStatistics::OnFolderSizeCompleted() 
{
	m_FolderSizeDone = true ;

	if(m_pCallback!=NULL)
	{
		m_pCallback->OnStatEventFolderSizeCompleted() ;
	}
}

void CXCStatistics::OnFolderSizeReportInfo(const SFileOrDirectoryInfo& info) 
{
	m_StatValue.uTotalSize = info.nTotalSize ;
	m_StatValue.nTotalFiles = info.nFileCount ;

	if(!m_bCalledUIIni)
	{
		if(m_StatValue.uTransSize>0 && m_StatValue.fSpeed>0.0f)
		{
			if(m_StatValue.uTotalSize/m_StatValue.fSpeed>2.0f)
			{
				m_bCalledUIIni = true ;
			}
		}
		else if(m_StatValue.uTotalSize>10*1024*1024 || (m_StatValue.nTotalFiles>20 && m_StatValue.uTotalSize>3*1024*1024))
		{
			m_bCalledUIIni = true ;
		}
	}
}

//float CXCStatistics::GetSpeed() const 
//{
//	m_StatValue.fSpeed
//}
