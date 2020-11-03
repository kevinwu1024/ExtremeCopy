/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "..\Common\ptFolderSize.h"

class CXCStatisticsCB
{
public:
	virtual void OnStatEventFolderSizeCompleted()  = 0;
};

struct SStatisticalValue
{
	float				fSpeed ; // 单位: byte/second
	unsigned int		nLapseTime ; // 单位: 秒

	unsigned int		nTotalFiles ;
	unsigned int		nDoneFiles ;
	unsigned int		nSkipFiles ;

	unsigned __int64	uVerifyDataSize ;
	unsigned __int64	uTotalSize ;
	unsigned __int64	uTransSize ; 
	unsigned __int64	uDoneSize ;

	unsigned __int64	uCurWholeSize ;
	unsigned __int64	uCurOperateSize ; // 包括读和写的总操作大小
	unsigned __int64	uCurDoubleWholeSize ;

	SStatisticalValue()
	{
		::memset(this,0,sizeof(this)) ;
	}
};

class CXCStatistics : public CFolderSizeEventCB
{
public:
	CXCStatistics() ;
	virtual ~CXCStatistics() ;

	bool Start(const pt_STL_vector(CptString)& FolderVer,CXCStatisticsCB* pCallback) ;
	void Stop() ;

	const SStatisticalValue& GetStaData() const ;
	bool DoesFolderSizeDone() const ;
	bool IsShowDelayUI() const {return m_bCalledUIIni;}

	void SetCurWholeSize(const unsigned __int64& uCurWholeSize) ;

	void SetDataOccured(const unsigned& uSize,bool bReadOrWrite) ;
	void SetVerifyDataOccured(const unsigned int& uSize) ;

	void IncreseDoneFileCount(int nCount) ;
	void IncreaseSkipFileCount(int nCount) ;
	void MoveNextSecond() ;
	unsigned __int64 GetCurFileRemainSize() const ;

protected:
	// 文件夹大小回调事件
	virtual void OnFolderSizeCompleted() ;
	virtual void OnFolderSizeReportInfo(const SFileOrDirectoryInfo& info) ;

private:

	/**/
	template<int nSeconds>
	struct SSpeedData
	{
		SSpeedData()
		{
			m_nTotalSize = 0 ;
			m_nCurIndex = 0 ;
			m_nTimeCount = 1 ;

			for(int i=0;i<nSeconds;++i)
			{
				m_nOperateSizeArray[i] = 0 ;
			}
		}

		// 返回当前 nSeconds 内的平均速度
		float MoveNextSecond()
		{
			//CptAutoLock lock(&m_lock) ;

			m_nOperateSizeArray[m_nCurIndex] = m_nTotalSize ;

			++m_nCurIndex ;
			if(m_nCurIndex>=nSeconds)
			{
				m_nCurIndex = 0 ;
			}

			float nRet = 0.0f ;

			if(m_nTimeCount<nSeconds)
			{
				++m_nTimeCount ;
				nRet = (m_nTotalSize/(float)(m_nTimeCount-1)) ;
			}
			else
			{
				nRet = (float)(m_nTotalSize-m_nOperateSizeArray[m_nCurIndex])/(float)(nSeconds-1) ;
			}

			return nRet ;
		}

		void SetCurSecondData(const unsigned int& nSize)
		{
			m_nTotalSize += nSize ;
		}

	private:
		//CptCritiSecLock		m_lock ;
		unsigned __int64 m_nTotalSize ;
		unsigned __int64 m_nOperateSizeArray[nSeconds] ; // 存放N秒的数据
		int m_nCurIndex ;
		unsigned m_nTimeCount ; // 过往的时间的计数器 (<= nSeconds),该值稳定时是等于nSeconds的值,并且初始化为1,所以不会等于0
	};
	/**/

private:
	
	SStatisticalValue		m_StatValue ;
	SSpeedData<10>			m_SpeedData ;
	CptFolderSize			m_FolderSize ;
	bool					m_FolderSizeDone ;
	bool					m_bCalledUIIni ; // 用作统计界面是否要显示

	CXCStatisticsCB*	m_pCallback ;
};