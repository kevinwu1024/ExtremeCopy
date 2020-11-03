/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "stdafx.h"
#include "..\Common\ptString.h"
#include "../Common/ptTypeDef.h"

/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/
#include <time.h>

// 本地的黑名单
const TCHAR* LicenseLocalBlackList[] = 
{
	_T("LGK91-GHGCZ-C34D2-DVVWQ")
} ;

// for ExtremeCopy 1.x
static const BYTE s_Chars[] = {'M','E','O','N','V','4','5','6','D','P','Q',
'R','W','A','B','2','3','S','T','C','L','X','J','K','7','8','Y','Z','0','1','U','F','G','H','I','9'} ;

 //for ExtremeCopy 2.0
static const BYTE s_Chars_2[] = {'M','E','O','6','D','P','3','S','B','2','J','8','Y','1','U','F','Q',
	'R','K','7','G','H','T','C','L','Z','0','X','W','A','N','V','4','5','I','9'} ;


bool GetStringSNToNum(const BYTE* Chars,int CharsSize,const CptStringList& sl,int* pIndexArry)
{
	int nCount = sl.GetCount() ;
	if(nCount==4 || nCount==5)
	{
		CptString strNum ;
		BYTE by = 0 ;

		for(int i=0;i<nCount;++i)
		{
			if(sl[i].GetLength()!=5)
			{
				return false ;
			}

			for(int j=0;j<5;++j)
			{
				int n = 0;
				by = (BYTE)(sl[i])[j] ;

				for(n=0;n<CharsSize;++n)
				{
					if(by==Chars[n])
					{
						pIndexArry[i*5+j] = n ;
						break ;
					}
				}

				if(n==CharsSize)
				{
					return false ;
				}
			}

			strNum += sl[i] ;
		}

		return true ;
	}

	return false ;
}

// 判断 license key 是否为本地的黑名单
bool IsBlackList(const CptStringList& sl)
{
	if(sl.GetCount()==4)
	{
		const int nBlackListNumber = sizeof(LicenseLocalBlackList)/sizeof(TCHAR*) ;

		for(int i=0;i<nBlackListNumber;++i)
		{
			const TCHAR* szLicense = LicenseLocalBlackList[i] ;

			CptStringList slBlack  ;

			if(slBlack.Split(szLicense,_T("-")) ==4)
			{
				if(slBlack[0].CompareNoCase(sl[0])==0 && 
					slBlack[1].CompareNoCase(sl[1])==0 && 
					slBlack[2].CompareNoCase(sl[2])==0 && 
					slBlack[3].CompareNoCase(sl[3])==0 )
				{
					return true ;
				}
			}
		}
	}
	

	return false ;
}

bool CheckRegisterCodeForExtremeCopy2(const CptStringList& sl) 
{
	// 如果为本地的黑名单，则检查失败
	if(IsBlackList(sl))
	{
		return false ;
	}

	bool bRet = false ;

	int nIndexArry[20] = {0} ;

	if(GetStringSNToNum(s_Chars_2,sizeof(s_Chars_2),sl,nIndexArry))
	{
		bRet = (nIndexArry[17] -nIndexArry[5]==11
			&& (nIndexArry[7] + nIndexArry[1])%sizeof(s_Chars_2)==nIndexArry[13]
		&& (nIndexArry[5] + nIndexArry[12])%sizeof(s_Chars_2)==nIndexArry[19]
		&& nIndexArry[16]- nIndexArry[0]==7) ;
	}

	return bRet ;
}

bool CheckRegisterCodeForExtremeCopyLibrary2(const CptStringList& sl) 
{
	bool bRet = false ;

	int nIndexArry[25] = {0} ;

	if(GetStringSNToNum(s_Chars_2,sizeof(s_Chars_2),sl,nIndexArry))
	{
		bRet = (nIndexArry[17] -nIndexArry[5]==11
			&& (nIndexArry[8] + nIndexArry[1])%sizeof(s_Chars_2)==nIndexArry[13]
		&& (nIndexArry[5] + nIndexArry[12])%sizeof(s_Chars_2)==nIndexArry[19]
		&& nIndexArry[16]- nIndexArry[0]==7
		&& (nIndexArry[9] + nIndexArry[24])%sizeof(s_Chars_2)==nIndexArry[21]) ;
	}

	return bRet ;
}
bool CheckRegisterCode(const CptStringList& sl) 
{
	bool bRet = false ;

	if(sl.GetCount()==4)
	{
		CptString strNum ;
		int nIndexArry[20] = {0} ;
		BYTE by = 0 ;

		for(int i=0;i<4;++i)
		{
			if(sl[i].GetLength()!=5)
			{
				return false ;
			}

			for(int j=0;j<5;++j)
			{
				int n = 0;
				by = (BYTE)(sl[i])[j] ;

				for(n=0;n<sizeof(s_Chars);++n)
				{
					if(by==s_Chars[n])
					{
						nIndexArry[i*5+j] = n ;
						break ;
					}
				}

				if(n==sizeof(s_Chars))
				{
					return false ;
				}
			}

			strNum += sl[i] ;
		}

		bRet = (nIndexArry[13] -nIndexArry[2]==8
			&& (nIndexArry[9] + nIndexArry[11])%sizeof(s_Chars)==nIndexArry[7]
		&& (nIndexArry[5] + nIndexArry[18])%sizeof(s_Chars)==nIndexArry[10]
		&& nIndexArry[16]- nIndexArry[0]==23) ;
	}

	return bRet ;
}



void GetRegisterCode(CptStringList& sl) 
{
	sl.Clear() ;

	int nIndexArry[20] = {0} ;

	for(int i=0;i<20;++i)
	{
		nIndexArry[i] = ::rand()%sizeof(s_Chars) ;
	}

	nIndexArry[13] = ::rand()%(sizeof(s_Chars)-8) + 8 ;
	nIndexArry[2] = nIndexArry[13] - 8 ;

	nIndexArry[7] = (nIndexArry[9] + nIndexArry[11])%sizeof(s_Chars) ;

	nIndexArry[10] = (nIndexArry[5] + nIndexArry[18])%sizeof(s_Chars) ;

	nIndexArry[16] = ::rand()%(sizeof(s_Chars)-23) + 23 ;
	nIndexArry[0] = nIndexArry[16] - 23 ;

	CptString str ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars[nIndexArry[0]],
		s_Chars[nIndexArry[1]],
		s_Chars[nIndexArry[2]],
		s_Chars[nIndexArry[3]],
		s_Chars[nIndexArry[4]]) ;

	//Debug_Printf(str.c_str()) ;
	sl.Add(str) ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars[nIndexArry[5]],
		s_Chars[nIndexArry[6]],
		s_Chars[nIndexArry[7]],
		s_Chars[nIndexArry[8]],
		s_Chars[nIndexArry[9]]) ;

	//Debug_Printf(str.c_str()) ;
	sl.Add(str) ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars[nIndexArry[10]],
		s_Chars[nIndexArry[11]],
		s_Chars[nIndexArry[12]],
		s_Chars[nIndexArry[13]],
		s_Chars[nIndexArry[14]]) ;

	//Debug_Printf(str.c_str()) ;
	sl.Add(str) ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars[nIndexArry[15]],
		s_Chars[nIndexArry[16]],
		s_Chars[nIndexArry[17]],
		s_Chars[nIndexArry[18]],
		s_Chars[nIndexArry[19]]) ;

	sl.Add(str) ;

}

void GetRegisterCodeForExtremeCopy2(CptStringList& sl) 
{
	sl.Clear() ;

	int nIndexArry[20] = {0} ;

	for(int i=0;i<20;++i)
	{
		nIndexArry[i] = ::rand()%sizeof(s_Chars_2) ;
	}
	
	nIndexArry[17] = ::rand()%(sizeof(s_Chars_2)-11) + 11 ;
	nIndexArry[5] = nIndexArry[17] - 11 ;

	nIndexArry[13] = (nIndexArry[7] + nIndexArry[1])%sizeof(s_Chars_2) ;

	nIndexArry[19] = (nIndexArry[5] + nIndexArry[12])%sizeof(s_Chars_2) ;

	nIndexArry[16] = ::rand()%(sizeof(s_Chars_2)-7) + 7 ;
	nIndexArry[0] = nIndexArry[16] - 7 ;

	CptString str ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars_2[nIndexArry[0]],
		s_Chars_2[nIndexArry[1]],
		s_Chars_2[nIndexArry[2]],
		s_Chars_2[nIndexArry[3]],
		s_Chars_2[nIndexArry[4]]) ;

	//Debug_Printf(str.c_str()) ;
	sl.Add(str) ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars_2[nIndexArry[5]],
		s_Chars_2[nIndexArry[6]],
		s_Chars_2[nIndexArry[7]],
		s_Chars_2[nIndexArry[8]],
		s_Chars_2[nIndexArry[9]]) ;

	//Debug_Printf(str.c_str()) ;
	sl.Add(str) ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars_2[nIndexArry[10]],
		s_Chars_2[nIndexArry[11]],
		s_Chars_2[nIndexArry[12]],
		s_Chars_2[nIndexArry[13]],
		s_Chars_2[nIndexArry[14]]) ;

	//Debug_Printf(str.c_str()) ;
	sl.Add(str) ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars_2[nIndexArry[15]],
		s_Chars_2[nIndexArry[16]],
		s_Chars_2[nIndexArry[17]],
		s_Chars_2[nIndexArry[18]],
		s_Chars_2[nIndexArry[19]]) ;

	sl.Add(str) ;

}

void GetRegisterCodeForExtremeCopyLibrary2(CptStringList& sl) 
{
	sl.Clear() ;

	int nIndexArry[25] = {0} ;

	for(int i=0;i<25;++i)
	{
		nIndexArry[i] = ::rand()%sizeof(s_Chars_2) ;
	}

	nIndexArry[17] = ::rand()%(sizeof(s_Chars_2)-11) + 11 ;
	nIndexArry[5] = nIndexArry[17] - 11 ;

	nIndexArry[13] = (nIndexArry[8] + nIndexArry[1])%sizeof(s_Chars_2) ;

	nIndexArry[19] = (nIndexArry[5] + nIndexArry[12])%sizeof(s_Chars_2) ;

	nIndexArry[16] = ::rand()%(sizeof(s_Chars_2)-7) + 7 ;
	nIndexArry[0] = nIndexArry[16] - 7 ;

	nIndexArry[21] = (nIndexArry[9] + nIndexArry[24])%sizeof(s_Chars_2) ;

	CptString str ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars_2[nIndexArry[0]],
		s_Chars_2[nIndexArry[1]],
		s_Chars_2[nIndexArry[2]],
		s_Chars_2[nIndexArry[3]],
		s_Chars_2[nIndexArry[4]]) ;

	//Debug_Printf(str.c_str()) ;
	sl.Add(str) ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars_2[nIndexArry[5]],
		s_Chars_2[nIndexArry[6]],
		s_Chars_2[nIndexArry[7]],
		s_Chars_2[nIndexArry[8]],
		s_Chars_2[nIndexArry[9]]) ;

	//Debug_Printf(str.c_str()) ;
	sl.Add(str) ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars_2[nIndexArry[10]],
		s_Chars_2[nIndexArry[11]],
		s_Chars_2[nIndexArry[12]],
		s_Chars_2[nIndexArry[13]],
		s_Chars_2[nIndexArry[14]]) ;

	//Debug_Printf(str.c_str()) ;
	sl.Add(str) ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars_2[nIndexArry[15]],
		s_Chars_2[nIndexArry[16]],
		s_Chars_2[nIndexArry[17]],
		s_Chars_2[nIndexArry[18]],
		s_Chars_2[nIndexArry[19]]) ;

	sl.Add(str) ;

	str.Format(_T("%C%C%C%C%C"),
		s_Chars_2[nIndexArry[20]],
		s_Chars_2[nIndexArry[21]],
		s_Chars_2[nIndexArry[22]],
		s_Chars_2[nIndexArry[23]],
		s_Chars_2[nIndexArry[24]]) ;

	sl.Add(str) ;

}

bool CheckRegisterCode2(const CptStringList& sl,int prog)
{
	switch(prog)
	{
	case 0: // ExtremeCopy 1.x
		//GetRegisterCode(sl2) ;
		return CheckRegisterCode(sl) ;
		break ;

	case 1: // ExtremeCopy 2.0
		//GetRegisterCodeForExtremeCopy2(sl2) ;
		return CheckRegisterCodeForExtremeCopy2(sl) ;
		break ;

	case 2: // ExtremeCopy Library 2.0
		return CheckRegisterCodeForExtremeCopyLibrary2(sl) ;
		break ;

	default:
		return false ;
	}
}

void GetRegisterCodeInString(CptStringList& sl,int prog,int nCount)
{
	::srand((unsigned int)::time(NULL)) ;
	CptStringList sl2 ;

	CptString str ;

	for(int j=0;j<nCount;++j)
	{
		switch(prog)
		{
		case 0: // ExtremeCopy 1.x
			GetRegisterCode(sl2) ;
			break ;

		case 1: // ExtremeCopy 2.0
			GetRegisterCodeForExtremeCopy2(sl2) ;
			break ;

		case 2: // ExtremeCopy Library
			GetRegisterCodeForExtremeCopyLibrary2(sl2) ;
			break ;
		}
		
		str = _T("") ;
		for(int i=0;i<sl2.GetCount(); ++i)
		{
			str += sl2[i] ;
			if(i<sl2.GetCount()-1)
			{
				str += _T("-") ;
			}
		}

		sl.Add(str) ;
	}


}