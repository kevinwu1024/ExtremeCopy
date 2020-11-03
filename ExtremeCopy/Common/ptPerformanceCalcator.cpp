#include "StdAfx.h"
#include "ptPerformanceCalcator.h"


CptPerformanceCalcator* CptPerformanceCalcator::s_pInstance = NULL ;

CptPerformanceCalcator::CptPerformanceCalcator(void)
{

	m_uIDCount = 0 ;
}


CptPerformanceCalcator::~CptPerformanceCalcator(void)
{
}


CptPerformanceCalcator* CptPerformanceCalcator::GetInstance() 
{
	if(s_pInstance==NULL)
	{
		s_pInstance = new CptPerformanceCalcator() ;
	}

	return s_pInstance ;
}

DWORD CptPerformanceCalcator::BeginCal() 
{
	return ::GetTickCount() ;
}

DWORD CptPerformanceCalcator::EndCal(const DWORD& dwBegin) 
{
	return ::GetTickCount() - dwBegin ;
}

DWORD CptPerformanceCalcator::EndCalAndSave(const DWORD& dwBegin,int nIndex) 
{
	DWORD dwDiff = ::GetTickCount() - dwBegin ;

	std::map<int,DWORD>::iterator it = m_DiffByIndexMap.find(nIndex) ;
	
	if(it!=m_DiffByIndexMap.end())
	{
		m_DiffByIndexMap[nIndex] = m_DiffByIndexMap[nIndex] + dwDiff ;

	}
	else
	{
		m_DiffByIndexMap[nIndex] = dwDiff ;
	}

	return dwDiff ;
}

DWORD CptPerformanceCalcator::GetCal(int nIndex,bool bRemove) 
{
	DWORD dwRet = 0 ;

	std::map<int,DWORD>::iterator it = m_DiffByIndexMap.find(nIndex) ;

	if(it!=m_DiffByIndexMap.end())
	{
		dwRet = m_DiffByIndexMap[nIndex] ;

		if(bRemove)
		{
			m_DiffByIndexMap.erase(it) ;
		}
		
	}

	return dwRet ;
}

float CptPerformanceCalcator::GetCalInSecond(int nIndex,bool bRemove) 
{
	float dwRet = 0.0f ;

	std::map<int,DWORD>::iterator it = m_DiffByIndexMap.find(nIndex) ;

	if(it!=m_DiffByIndexMap.end())
	{
		dwRet = ((float)m_DiffByIndexMap[nIndex])/1000 ;

		if(bRemove)
		{
			m_DiffByIndexMap.erase(it) ;
		}
	}

	return dwRet ;
}
