/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/


#include "StdAfx.h"
#include "ptCmdLineParser.h"

CptCmdLineParser::CptCmdLineParser(void)
{
}

CptCmdLineParser::~CptCmdLineParser(void)
{
}


int CptCmdLineParser::Parse(const TCHAR* pCmdLine) 
{
	_ASSERT(pCmdLine!=NULL) ;

	m_SwitchsMap.clear() ;
	m_ParamsVer.clear() ;

	pt_STL_vector(CptString) SwitchLineVer ;
	
	if(this->SplitSwitchGroup(pCmdLine,SwitchLineVer)>0)
	{
		for(unsigned i=0;i<SwitchLineVer.size();++i)
		{
			this->ParseSwitchLine(SwitchLineVer[i]) ;
		}
	}

	return (int)m_SwitchsMap.size() ;
}


bool CptCmdLineParser::IsContainSwitch(const TCHAR* pSwitch) const 
{
	_ASSERT(pSwitch!=NULL) ;

	const TCHAR* pSwitchName = pSwitch + this->GetSwitchNameOffset(pSwitch);

	return (m_SwitchsMap.find(pSwitchName)!=m_SwitchsMap.end()) ;
}

void CptCmdLineParser::RemoveSwitch(const TCHAR* pSwitch) 
{
	_ASSERT(pSwitch!=NULL) ;

	const TCHAR* pSwitchName = this->GetSwitchNameOffset(pSwitch) + pSwitch;
	SwitchContainer_t::const_iterator it = m_SwitchsMap.find(pSwitchName) ;

	m_SwitchsMap.erase(it) ;
}

int CptCmdLineParser::GetSwitchParamCount(const TCHAR* pSwitch) 
{
	_ASSERT(pSwitch!=NULL) ;

	int nRet = -1 ;

	const TCHAR* pSwitchName = this->GetSwitchNameOffset(pSwitch) + pSwitch;

	SwitchContainer_t::const_iterator it = m_SwitchsMap.find(pSwitchName) ;

	if(it!=m_SwitchsMap.end())
	{
		nRet = 0 ; // 至少说明该开关存在

		unsigned int nParamIndex = (*it).second ;
		
		if(nParamIndex>=0 && m_ParamsVer.size()>nParamIndex)
		{
			nRet = (int)m_ParamsVer[nParamIndex].size() ;
		}
	}

	return nRet ;
}

CptString CptCmdLineParser::GetParam(const TCHAR* pSwitch,int nIndex,const TCHAR* pDefaultValue) 
{
	_ASSERT(pSwitch!=NULL) ;
	_ASSERT(pDefaultValue!=NULL) ;
	_ASSERT(nIndex>=0) ;

	const TCHAR* pSwitchName = this->GetSwitchNameOffset(pSwitch) + pSwitch;

	SwitchContainer_t::const_iterator it = m_SwitchsMap.find(pSwitchName) ;

	if(it!=m_SwitchsMap.end())
	{
		unsigned int nParamIndex = (*it).second ;
		
		if(nParamIndex>=0 && m_ParamsVer.size()>nParamIndex)
		{
			if(m_ParamsVer[nParamIndex].size()>(unsigned int)nIndex)
			{
				return (m_ParamsVer[nParamIndex])[nIndex] ;
			}
		}
	}

	return pDefaultValue ;
}

bool CptCmdLineParser::GetParams(const TCHAR* pSwitch,pt_STL_vector(CptString)& strParamVer)
{
	_ASSERT(pSwitch!=NULL) ;

	bool bRet = false ;

	strParamVer.clear() ;

	const TCHAR* pSwitchName = this->GetSwitchNameOffset(pSwitch) + pSwitch;

	SwitchContainer_t::const_iterator it = m_SwitchsMap.find(pSwitchName) ;

	if(it!=m_SwitchsMap.end())
	{
		bRet = true ;

		unsigned int nParamIndex = (*it).second ;
		
		if(nParamIndex>=0 && m_ParamsVer.size()>nParamIndex)
		{
			strParamVer = m_ParamsVer[nParamIndex] ;
		}
	}

	return bRet ;
}

int CptCmdLineParser::GetSwitchCount() const 
{
	return (int)m_SwitchsMap.size() ;
}

CptString CptCmdLineParser::GetSwitchNameByIndex(int nIndex) const
{
	if(nIndex>=0 && m_SwitchsMap.size()>(unsigned int)nIndex)
	{
		SwitchContainer_t::const_iterator it = m_SwitchsMap.begin() ;

		for(int i=0;i<nIndex;++i,++it)
		{
		}

		return (*it).first.c_str() ;
	}

	return _T("") ;
}

int CptCmdLineParser::GetSwitchNameOffset(const TCHAR* pSwitchName) const
{
	int nOffset = 0 ;

	while(this->IsSwitchChar(pSwitchName[nOffset]))
	{
		++nOffset ;
	}

	return nOffset ;
}

void CptCmdLineParser::ParseSwitchLine(CptString strSwitchLine)
{
	CptString strSwitch ;
	CptString strParam ;
	pt_STL_vector(CptString) strParamVer ;
	bool bSwitch = true ;
	bool bInsideQuot = false ;
	int nCurIndex = 0 ;

	const TCHAR* pSwitchLine = strSwitchLine.c_str() ;

	while(pSwitchLine[nCurIndex]!=NULL)
	{
		if(bSwitch)
		{// 若为开关的字符串
			if(pSwitchLine[nCurIndex]==' ')
			{
				bSwitch = false ;
			}
			else// if(this->IsSwitchChar(pSwitchLine[nCurIndex]))
			{
				strSwitch += pSwitchLine[nCurIndex] ;
			}
		}
		else
		{// 若为参数的字符串
			switch(pSwitchLine[nCurIndex])
			{
			case ' ':
				if(!bInsideQuot)
				{
					if(strParam.GetLength()>0)
					{
						strParamVer.push_back(strParam) ;
						strParam = _T("") ;
					}
				}
				else
				{
					strParam += pSwitchLine[nCurIndex] ; // 要收集的参数
				}
				break ;

			case '"':
				bInsideQuot = !bInsideQuot ;
				break ;

			default:
				strParam += pSwitchLine[nCurIndex] ; // 要收集的参数
				break ;
			}
		}

		++nCurIndex ;
	}

	if(strSwitch.GetLength()>0)
	{
		int nParamIndex = -1 ;

		if(strParam.GetLength()>0)
		{
			strParamVer.push_back(strParam) ;
			strParam = _T("") ;
		}

		if(!strParamVer.empty())
		{
			m_ParamsVer.push_back(strParamVer) ;
			nParamIndex = (int)(m_ParamsVer.size() -1) ;
		}

		m_SwitchsMap[strSwitch.c_str()] = nParamIndex ;
	}
}

int CptCmdLineParser::SplitSwitchGroup(const TCHAR* pCmdLine,pt_STL_vector(CptString)& SwitchLineVer)
{
	_ASSERT(pCmdLine!=NULL) ;

	CptString strTem ;

	SwitchLineVer.clear() ;
	int nCurIndex = 0 ;
	bool bInsideQuot = false ;

	while(pCmdLine[nCurIndex]!=NULL)
	{
		if(!bInsideQuot && (nCurIndex==0 || (nCurIndex>0 && pCmdLine[nCurIndex-1]==' '))
			&& this->IsSwitchChar(pCmdLine[nCurIndex]) && !this->IsSwitchChar(pCmdLine[nCurIndex+1]))
		{// 在非双引号内的才有可能是 switch
			// 只有当前面的是空格，那么后面才有可能是 switch 
			// 并且连续2个或以上的 switch 标识也不算是 switch
			if(strTem.GetLength()>0)
			{
				SwitchLineVer.push_back(strTem) ;
				strTem = _T("") ;
			}

			bInsideQuot = false ;
		}
		else
		{
			if(pCmdLine[nCurIndex]=='"')
			{
				bInsideQuot = !bInsideQuot ;
			}
			
			strTem += pCmdLine[nCurIndex] ;
			
		}
		
		++nCurIndex ;
	}

	if(strTem.GetLength()>0)
	{
		SwitchLineVer.push_back(strTem) ;
		strTem = _T("") ;
	}

	return (int)SwitchLineVer.size() ;
}

bool CptCmdLineParser::IsSwitchChar(const TCHAR ch) const
{
	return (ch=='-' || ch=='/') ;
}