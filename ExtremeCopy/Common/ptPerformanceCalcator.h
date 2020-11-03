#pragma once

#include <map>

class CptPerformanceCalcator
{
protected:
	CptPerformanceCalcator(void);

public:

	~CptPerformanceCalcator(void);

	static CptPerformanceCalcator* GetInstance() ;

	DWORD BeginCal() ;
	DWORD EndCal(const DWORD& dwBegin) ;
	DWORD EndCalAndSave(const DWORD& dwBegin,int nIndex) ;

	DWORD GetCal(int nIndex,bool bRemove) ;
	float GetCalInSecond(int nIndex,bool bRemove) ;


private:
	//DWORD	m_BeginTickCount ;
	std::map<int,DWORD> m_DiffByIndexMap ;
	//std::map<int,DWORD>	m_BeginTickByIDMap ;
	//CRITICAL_SECTION m_csLock ;
	int	m_uIDCount ;

	static CptPerformanceCalcator* s_pInstance ;

};

