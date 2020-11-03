/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

//#define _UNICODE

#ifdef _DEBUG
//#define PT_DEBUG
#endif

#include <windows.h>
#include <tchar.h>
#include <vector>

//#define USE_SGI_STL_MEMORY_FOR_PTSTRING

#ifdef USE_SGI_STL_MEMORY_FOR_PTSTRING
//#define USE_STL_MEMORY_FOR_PTSTRING
#endif

#ifdef USE_SGI_STL_MEMORY_FOR_PTSTRING
#include "sgi_stl_alloc.h"
#define __STD_SPACE		sgi_std
#else
#define __STD_SPACE		std
#endif

#ifdef _UNICODE

#define CptString		CptStringW
#define CptStringList	CptStringListW

#else

#define CptString		CptStringA
#define CptStringList	CptStringListA

#endif //#ifdef _UNICODE

class CptStringA ;

CptStringA operator+(const CptStringA& str1,const CptStringA& str2) ;

CptStringA operator+(const CptStringA& str,const char ch) ;

CptStringA operator+(const char ch,const CptStringA& str) ;


CptStringA operator+(const CptStringA& str, LPCSTR lpsz);
CptStringA operator+(LPCSTR lpsz, const CptStringA& str);


#pragma warning(push)
#pragma warning( disable : 4290 )

#pragma pack(push,1)

class CptStringBase
{
public:
	static void ReleaseAllRes() ;
	int GetRefCount() const {return this->GetStringDesc()->nRefs;}

protected:
	CptStringBase() ;// 不被外部访问
	~CptStringBase() {this->DecreaseRef();}

	struct SStringDesc
	{
		long nRefs;		// 内存块引用计数
		long nUsedSize;			// 字符串长度 
		long nMallocBufSize;		// 内存分配长度

		SStringDesc():nRefs(0),nUsedSize(0),nMallocBufSize(0) {} ;

		void* GetStringPtr() const { return (void*)(this+1); }

		int GetStrBufSize() const { return nMallocBufSize-sizeof(SStringDesc) ;}
	};

		// 字符串内存池
	class CStrMemBuf 
	{

#ifdef PT_DEBUG

#define		MAX_POOL_8		0
#define		MAX_POOL_16		0
#define		MAX_POOL_32		0
#define		MAX_POOL_64		0
#define		MAX_POOL_128	0
#define		MAX_POOL_256	0
#define		MAX_POOL_512	0

#else

#define		MAX_POOL_8		50
#define		MAX_POOL_16		20
#define		MAX_POOL_32		30
#define		MAX_POOL_64		50
#define		MAX_POOL_128	10
#define		MAX_POOL_256	10
#define		MAX_POOL_512	10

#endif

	public:
		CStrMemBuf() ;
		~CStrMemBuf() ;

		// 连同SStringDesc空间一起分配,返回SStringDesc的入口地址
		SStringDesc* GetBuffer(const UINT uSize) ;
		void ReleaseBuffer(SStringDesc* pDesc) ;

		SStringDesc* GetEmptyBuffer() const;	
		void ReleaseAllRes() ;

	private:
		//	inline SStringDesc* GetSpace(const int& nIndex) ;
		//	inline void ReleaseSpace(const int& nIndex) ;
	private:
		BYTE		m_EmptyBuf[sizeof(SStringDesc)+sizeof(WCHAR)] ;

#ifdef USE_STL_MEMORY_FOR_PTSTRING
		__STD_SPACE::allocator<char>	m_Alloc ;
#else
				//WCHAR		m_chEmpty ;
		void*		m_aMemList[7] ;
		BYTE		m_byUnitCount[7] ;

		CRITICAL_SECTION	m_csLock ;
#endif

#ifdef PT_DEBUG
	public:
		int		m_nAllocCounter ;
		unsigned m_uAllocBufSize ;
#endif

	};

#define STRING_SIZE_8		8
#define STRING_SIZE_16		16
#define STRING_SIZE_32		32
#define STRING_SIZE_64		64
#define STRING_SIZE_128		128
#define STRING_SIZE_256		256
#define STRING_SIZE_512		512

	inline SStringDesc* GetStringDesc() const {return (((SStringDesc*)m_pchStrData)-1) ; } ;

	void AllocBuf(const int nStringSize) ;

	void FreeBuf() ;

	inline int DecreaseRef() ;

	inline int IncreaseRef() const;

	inline void BeforeWrite(const int nStringSize) ;

	inline void SetEmptyStr() ;

	inline void CloneBeforeWrite() ;
	static inline void NewMemBufInstance() ;

	
public:
	bool IsEmpty() const ;
	int GetUsedSize() const ;
	//int GetLength() const ;
	void Empty() ;

protected:
	//inline int InlineGetLength() const ;

#ifdef _UNICODE
#define 	StringPointer_t		WCHAR
#else
#define 	StringPointer_t		char
#endif

	StringPointer_t*		m_pchStrData ;		// 字符串开始入口地址. 存储格式:[SStringDesc][String]

#ifdef PT_DEBUG
	public:
#else
	protected:
#endif
	static CStrMemBuf*	m_pStrMemBuf ;
};

#pragma pack(pop)

//  字符串类 ANSI
class CptStringA  : public CptStringBase
{
public:
	CptStringA();
	
	CptStringA( const CptStringA& StrSrc );
	
	CptStringA( char ch, int nRepeat = 1 );

	CptStringA(LPCSTR lpsz);

	CptStringA(LPCWSTR lpsz);
	
	~CptStringA() ;

//	CString(const unsigned char* psz);

	int GetLength() const ;

	//int GetCharCount() const ;

	operator LPCSTR() const;
	
	// 赋值
	const CptStringA& operator=(const CptStringA& stringSrc);

	const CptStringA& operator=(char ch);

	const CptStringA& operator=(LPCSTR lpsz);

	const CptStringA& operator=(LPCWSTR lpsz);

	// 串接
	friend CptStringA operator+(const CptStringA& str1,const CptStringA& str2) ;

	friend CptStringA operator+(const CptStringA& str,const char ch) ;

	friend CptStringA operator+(const char ch,const CptStringA& str) ;
	
	friend CptStringA operator+(const CptStringA& str, LPCSTR lpsz);
	friend CptStringA operator+(LPCSTR lpsz, const CptStringA& str);


	// 串接并赋值
	const CptStringA& operator+=(const CptStringA& string);
	
	const CptStringA& operator+=(char ch);

	const CptStringA& operator+=(LPCWSTR lpsz);
	const CptStringA& operator+=(LPCSTR lpsz);

	// 比较
	
	bool operator==(const CptStringA str) const ;
	bool operator==(LPCSTR lpsz) const ;
	bool operator==(char ch) const ;

	bool operator!=(const CptStringA str) const ;
	bool operator!=(LPCSTR lpsz) const ;
	bool operator!=(char ch) const ;
	
	int Compare(LPCSTR lpsz) const;

	int CompareNoCase(LPCSTR lpsz) const;

	// 
	void MakeUpper();

	void MakeLower();

	// 提取
	CptStringA Mid(int nFirst, int nCount) const;

	CptStringA Left(int nCount) const;

	CptStringA Right(int nCount) const;

	// 查找
	int Replace(char chOld, char chNew);

	int Replace(LPCSTR lpszOld, LPCSTR lpszNew);

	int Remove(char chRemove);

	int Remove(int nBegin,int nLength) ;

	int RemoveAt(int nIndex) ;

	int Insert(int nIndex, char ch);

	int Insert(int nIndex, LPCSTR pstr);

	int Delete(int nIndex, int nCount = 1);

	int Find(LPCSTR lpszSub) const;

	int Find(LPCSTR lpszSub, int nStart) const;

	int Find(char ch) const;

	int Find(char ch, int nStart) const ;

	int ReverseFind(char ch) const ;

	int FindOneOf(LPCSTR lpszCharSet) const ;

	void Format(LPCSTR lpszFormat, ...);

	void FormatV(LPCSTR lpszFormat, va_list argList);

	void SetAt(int nIndex, char ch) ;

	char GetAt(int nIndex) const ;

	char operator []( int nIndex ) const;

	const char* c_str() const ;

	int AsInt() const throw(int) ;

	__int64 As64Int() const throw(int) ;

	double AsFloat() const throw(int) ;

	int TrimLeft(char c) ;
	int  TrimLeft(const char* szStr) ;

	int  TrimRight(char c) ;
	int  TrimRight(const char* szStr) ;

private:
	inline int InlineGetLength() const ;
};

class CptStringListA
{
public:
	CptStringListA() ;
	CptStringListA(const CptStringListA& StrList) ;

	CptStringListA& operator=(const CptStringListA& StrList) ;

	int Add(const CptStringA& str) ;
	int GetCount() const ;
	int Delete(int nIndex) ;
	int Delete(const CptStringA& str) ;
	int Find(const CptStringA& str) const;
	CptStringA& operator[](int nIndex) ;
	CptStringA operator[](int nIndex) const ;

	const CptStringListA operator-(const CptStringListA& sl) ;
	const CptStringListA operator+(const CptStringListA& sl) ;

	int Split(CptStringA strData,CptStringA strSep) ;
	void Clear() ;


private:
	std::vector<CptStringA>		m_StrVecr ;
};

/////////////////////////////////////////////
//字符串类, UNICODE
class CptStringW : public CptStringBase
{
public:
	CptStringW(void);
	~CptStringW(void);
	
	CptStringW( const CptStringW& StrSrc );
	
	CptStringW( WCHAR ch, int nRepeat = 1 );

	CptStringW(LPCSTR lpsz);

	CptStringW(LPCWSTR lpsz);
	
	

//	CString(const unsigned WCHAR* psz);

	int GetLength() const ;
//LPCWSTR
//	int GetCharCount() const ;

	operator LPCWSTR() const;
	
	// 赋值
	const CptStringW& operator=(const CptStringW& StrSrc);

	const CptStringW& operator=(WCHAR ch);

	const CptStringW& operator=(LPCSTR lpsz);

	const CptStringW& operator=(LPCWSTR lpsz);

	// 串接
	friend CptStringW operator+(const CptStringW& str1,const CptStringW& str2) ;

	friend CptStringW operator+(const CptStringW& str,const WCHAR ch) ;

	friend CptStringW operator+(const WCHAR ch,const CptStringW& str) ;
	

	friend CptStringW operator+(const CptStringW& str, LPCWSTR lpsz);
	friend CptStringW operator+(LPCWSTR lpsz, const CptStringW& str);


	// 串接并赋值
	const CptStringW& operator+=(const CptStringW& string);
	
	const CptStringW& operator+=(WCHAR ch);


	const CptStringW& operator+=(LPCSTR lpsz);
	const CptStringW& operator+=(LPCWSTR lpsz);

	// 比较
	
	bool operator==(const CptStringW str) const ;
	bool operator==(LPCWSTR lpsz) const ;
	bool operator==(WCHAR ch) const ;

	bool operator!=(const CptStringW str) const ;
	bool operator!=(LPCWSTR lpsz) const ;
	bool operator!=(WCHAR ch) const ;
	
	int Compare(LPCWSTR lpsz) const;

	int CompareNoCase(LPCWSTR lpsz) const;

	// 
	void MakeUpper();

	void MakeLower();

	// 提取
	CptStringW Mid(int nFirst, int nCount) const;

	CptStringW Left(int nCount) const;

	CptStringW Right(int nCount) const;

	// 查找
	int Replace(WCHAR chOld, WCHAR chNew);

	int Replace(LPCWSTR lpszOld, LPCWSTR lpszNew);

	int Remove(int nBegin,int nLength) ;

	int RemoveAt(int nIndex) ;

	int Remove(WCHAR chRemove);

	int Insert(int nIndex, WCHAR ch);

	int Insert(int nIndex, LPCWSTR pstr);

	int Delete(int nIndex, int nCount = 1);

	int Find(LPCWSTR lpszSub) const;

	int Find(LPCWSTR lpszSub, int nStart) const;

	int Find(WCHAR ch) const;

	int Find(WCHAR ch, int nStart) const ;

	int ReverseFind(WCHAR ch) const ;

	int FindOneOf(LPCWSTR lpszWCHARSet) const ;

	void Format(LPCWSTR lpszFormat, ...);

	void FormatV(LPCWSTR lpszFormat, va_list argList);

	void SetAt(int nIndex, WCHAR ch) ;

	WCHAR GetAt(int nIndex) const ;

	WCHAR operator []( int nIndex ) const;

	const WCHAR* c_str() const ;

	int AsInt() const throw(int) ;

	__int64 As64Int() const throw(int) ;

	double AsFloat() const throw(int) ;

	int TrimLeft(WCHAR c) ;
	int TrimLeft(const WCHAR* szStr) ;

	int TrimRight(WCHAR c) ;
	int TrimRight(const WCHAR* szStr) ;

private:
		inline int InlineGetLength() const ;
};


class CptStringListW
{
public:
	CptStringListW() ;
	CptStringListW(const CptStringListW& StrList) ;

	CptStringListW& operator=(const CptStringListW& StrList) ;

	int Add(const CptStringW& str) ;
	int GetCount() const ;
	int Delete(int nIndex) ;
	int Delete(const CptStringW& str) ;
	int Find(const CptStringW& str) const;
	CptStringW& operator[](int nIndex) ;
	CptStringW operator[](int nIndex) const ;

	const CptStringListW operator-(const CptStringListW& sl) ;
	const CptStringListW operator+(const CptStringListW& sl) ;

	int Split(CptStringW strData,CptStringW strSep) ;
	void Clear() ;

private:
	std::vector<CptStringW>		m_StrVecr ;
};

#pragma warning(pop)

