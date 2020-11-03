
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


#include <algorithm>
#include "sgi_stl_alloc.h"

//#define USE_SGI_MEMORY_POOL


#ifdef USE_SGI_MEMORY_POOL

#define pt_STL_vector(T) std::vector<T,sgi_std::allocator<T> > 
#define pt_STL_list(T) std::list<T,sgi_std::allocator<T> > 
#define pt_STL_deque(T) std::deque<T,sgi_std::allocator<T> >
#define pt_STL_map(TKey,TVal) std::map<TKey,TVal,std::less<TKey>,sgi_std::allocator<std::pair<const TKey&,TVal> > >
#define pt_STL_set(T) std::set<T,std::less<T>,sgi_std::allocator<T> >
#define pt_STL_hash_map(TKey,TVal) std::hash_map<TKey,TVal,std::hash_compare<TKey>,sgi_std::allocator<std::pair<const TKey&,TVal> > >
#define pt_STL_stack(T) std::stack<T,sgi_std::allocator<T> > 

#else

#define pt_STL_vector(T) std::vector<T> 
#define pt_STL_list(T) std::list<T> 
#define pt_STL_deque(T) std::deque<T>
#define pt_STL_map(TKey,TVal) std::map<TKey,TVal>
#define pt_STL_set(T) std::set<T,std::less<T>>
#define pt_STL_hash_map(TKey,TVal) std::hash_map<TKey,TVal>
#define pt_STL_stack(T) std::vector<T> 

#endif

#define PT_BREAK_IF(_condition_) \
			if((_condition_)) break 

// 版本信息
struct SptVerionInfo
{
	int nMajor ;	// 主版本号
	int nMin ;		// 次版本号
	int nFix ;		// 修复版本号
	int nBuild ;	// 构建版本号

	SptVerionInfo()
	{
		::memset(this,0,sizeof(SptVerionInfo)) ;
	}

	// 把版本号转为字符串
	void ToString(TCHAR* szVer)
	{
#pragma warning(push)
#pragma warning(disable:4996)
		::_stprintf(szVer,_T("%d.%d.%d.%d"),nMajor,nMin,nFix,nBuild) ;
#pragma warning(pop)
	}

	// 从有效的字符串提取各版本号为数字
	bool FromString(const TCHAR* lpVer)
	{
		_ASSERT(lpVer!=NULL) ;

		if(lpVer==NULL)
		{
			return false ;
		}

		const int nLen = (const int)::_tclen(lpVer) ;

		if(nLen>30)
		{
			return false ;
		}
		TCHAR szVer[30+1] = {0} ;

#pragma warning(push)
#pragma warning(disable:4996)
		::_tcscpy(szVer,lpVer) ;


		TCHAR* pCur = NULL ;
		int nIndex = 0 ;
		int nOffset = 0 ;

		pCur = ::_tcstok(szVer,_T(".")) ;

		while(pCur!=NULL)
		{
			if(!IsDigital(pCur))
			{
				nIndex = -1 ;
				break ;
			}

			switch(nIndex)
			{
			case 0: nMajor = ::_tstoi(pCur) ; break ;
			case 1: nMin = ::_tstoi(pCur) ; break ;
			case 2: nFix = ::_tstoi(pCur) ; break ;
			case 3: nBuild = ::_tstoi(pCur) ; break ;
			default: 
				++nIndex ;
				break ;
			}

			pCur = ::_tcstok(NULL,_T(".")) ;
			++nIndex ;

#pragma warning(pop)
		}
		
		const bool bRet = (nIndex>=0 && nIndex<5) ;

		if(!bRet)
		{
			::memset(this,0,sizeof(SptVerionInfo)) ;
		}

		return bRet ;
	}

	// 比较版本号大小
	// 返回值 0: 相等	1:大于	-1：小于
	int Compare(const SptVerionInfo& ver) const
	{
		int nRet  ;

		do
		{
			if((nRet = this->Compare(nMajor,ver.nMajor))!=0)
			{// 若主版本号不相同
				break ;
			}

			if((nRet = this->Compare(nMin,ver.nMin))!=0)
			{// 若次版本号不相同
				break ;
			}

			if((nRet =  this->Compare(nFix,ver.nFix))!=0)
			{// 若 fix 版本号不相同
				break ;
			}

			nRet = this->Compare(nBuild,ver.nBuild) ;
		}
		while(0);

		return nRet ;
	}

private:
	inline bool IsDigital(const TCHAR* p) const
	{
		_ASSERT(p!=NULL) ;

		int nCurIndex = 0 ;
		while(p[nCurIndex]!=NULL)
		{
			if(!(p[nCurIndex]>='0' && p[nCurIndex]<='9'))
			{
				return false ;
			}
			++nCurIndex ;
		}
		return true ;
	}

	inline int Compare(int nVal1,int nVal2) const
	{
		return (nVal1==nVal2) ? 0 : ((nVal1>nVal2) ? 1 : -1) ;
	}
};




// 文件时间
struct SFileTimeInfo
{
	FILETIME	CreateTime ;
	FILETIME	LastAccessTime ;
	FILETIME	LastWriteTime ;
};


struct SptPoint
{
	LONG  nX;
    LONG  nY;

	SptPoint():nX(0),nY(0) {}

	SptPoint(int x,int y):nX(x),nY(y) {}

	SptPoint(const POINT& pt)
	{
		::memcpy(this,&pt,sizeof(POINT)) ;
	}

	inline POINT* GetPOINTPointer() const
	{
		return (POINT*)this ;
	}

	void SetValue(int x,int y) {nX=x;nY=y;}
	void Clear() {nX=0;nY=0;}
};

struct SptSize
{
	LONG nWidth ;
	LONG nHeight ;

	SptSize():nWidth(0),nHeight(0)
	{
	}

	SptSize(int width,int height)
	{
		nWidth = width ;
		nHeight = height ;
	}
};

struct SptRect
{
	LONG    nLeft;
    LONG    nTop;
    LONG    nRight;
    LONG    nBottom;

	SptRect()
	{
		::memset(this,0,sizeof(SptRect)) ;
	}

	SptRect(const RECT& rt)
	{
		::memcpy(this,&rt,sizeof(RECT)) ;
	}

	SptRect(const SptPoint& pt,const SptSize& size)
	{
		this->nLeft = pt.nX ;
		this->nTop = pt.nY ;

		this->nRight = nLeft + size.nWidth ;
		this->nBottom = nTop + size.nHeight ;
	}

	SptRect(int left,int top,int right,int bottom)
	{
		this->nLeft = left ;
		this->nTop = top ;
		this->nRight = right ;
		this->nBottom = bottom ;
	}

	SptRect(const SptRect& rt)
	{
		::memcpy(this,&rt,sizeof(SptRect)) ;
	}

	inline RECT* GetRECTPointer() const
	{
		return (RECT*)this ;
	}

	inline bool IsContainPoint(const SptPoint& pt) const
	{
		return (pt.nX>=nLeft && pt.nX<=nRight && pt.nY>=nTop && pt.nY<=nBottom) ;
	}

	void SetValue(int left,int top,int right,int bottom)
	{
		this->nLeft = left ;
		this->nTop = top ;
		this->nRight = right ;
		this->nBottom = bottom ;
	}

	inline int GetWidth() const	{return (nRight - nLeft) ;}
	inline int GetHeight() const {return (nBottom-nTop) ;}

	inline SptSize GetSize() const {return SptSize(this->GetWidth(),this->GetHeight());}

	inline SptPoint GetLocation() const{return SptPoint(nLeft,nTop) ;}

	inline RECT GetRECTValue() const
	{
		return *((RECT*)this) ;
	}

	void SetWidth(int nNewWidth)
	{
		if(nNewWidth<0)
		{
			nNewWidth = 0 ;
		}

		nRight = nLeft + nNewWidth ;
	}

	void SetHeight(int nNewHeight)
	{
		if(nNewHeight<0)
		{
			nNewHeight = 0 ;
		}

		nBottom = nTop + nNewHeight ;
	}

	void SetLocation(int x,int y) 
	{
		const int nWidth = this->GetWidth() ;
		const int nHeight = this->GetHeight() ;

		nLeft = x ;
		nTop = y ;

		nRight = nWidth + nLeft ;
		nBottom = nTop + nHeight ;
	}

	void SetLocation(const SptPoint pt) 
	{
		this->SetLocation(pt.nX,pt.nY) ;
	}
};