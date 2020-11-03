
#include <Windows.h>


class CptCharCode
{
public:
	static inline bool Unicode2Utf8(const WCHAR* lpszUnicode,char* lpszUtf8,int nUft8Size)
	{
		_ASSERT(lpszUnicode!=NULL) ;
		_ASSERT(lpszUtf8!=NULL) ;
		_ASSERT(nUft8Size>0) ;

		bool bRet = false ;

		int	nLen = WideCharToMultiByte(CP_UTF8, 0, lpszUnicode, -1, NULL, 0, NULL, NULL); 

		if(nUft8Size>=nLen)
		{
			::WideCharToMultiByte (CP_UTF8, 0, lpszUnicode, -1, lpszUtf8, nLen, NULL,NULL);

			bRet = true ;
		}

		return bRet ;
	}

	static inline bool Utf82Unicode(const char* lpszUtf8,WCHAR* lpszUnicode,int nUnicodeSize)
	{
		_ASSERT(lpszUtf8!=NULL) ;
		_ASSERT(lpszUnicode!=NULL) ;
		_ASSERT(nUnicodeSize>0) ;

		bool bRet = false ;

		int nLen = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpszUtf8, -1, NULL,0);

		if(nUnicodeSize>nLen)
		{
			int aa = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpszUtf8, -1, lpszUnicode, nUnicodeSize);
			bRet = true ;
		}

		return bRet ;
	}

	static void CurrentSystemCode2Utf8(const char* lpszCSC,char* lpszUtf8,int nUft8Size) 
	{
		_ASSERT(lpszCSC!=NULL) ;
		_ASSERT(lpszUtf8!=NULL) ;
		_ASSERT(nUft8Size>0) ;

		// 先转换成unicode字符串
		int len=::MultiByteToWideChar(CP_ACP, 0, lpszCSC, -1, NULL,0);

		WCHAR * wszUtf8 = new WCHAR[len+1];

		::memset(wszUtf8, 0, (len+1)*sizeof(WCHAR));
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpszCSC, -1, wszUtf8, len);

		// 再由unicode 转换成 UTF8
		len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL); 

		if(nUft8Size>=len)
		{
			char *szUtf8=new char[len + 1];
			::memset(szUtf8, 0, len + 1);
			::WideCharToMultiByte (CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL,NULL);

			delete[] szUtf8;
		}

		delete[] wszUtf8;
	}

	static bool Utf82CurrentSystemCode(const char* lpszUtf8,char* lpszCSC,int nCSCSize) 
	{
		_ASSERT(lpszCSC!=NULL) ;
		_ASSERT(lpszUtf8!=NULL) ;
		_ASSERT(nCSCSize>0) ;

		bool bRet = false ;

		int nLen = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpszUtf8, -1, NULL,0);

		WCHAR * wszUnicode = new WCHAR[nLen+1];
		::memset(wszUnicode, 0, (nLen+1)*sizeof(WCHAR));

		::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpszUtf8, -1, wszUnicode, nLen);

		nLen = ::WideCharToMultiByte(CP_ACP, 0, wszUnicode, -1, NULL, 0, NULL, NULL); 

		if(nCSCSize>=nLen)
		{
			::WideCharToMultiByte (CP_ACP, 0, wszUnicode, -1, lpszCSC, nLen, NULL,NULL);
			bRet = true ;
		}
		
		delete[] wszUnicode;

		return bRet ;
	}

	static inline bool Utf82CurrentAppCode(const char* lpszUtf8,TCHAR* lpszCSC,int nBufSize)
	{
#ifdef _UNICODE
		return Utf82Unicode(lpszUtf8,lpszCSC,nBufSize) ;
#else
		return Utf82CurrentSystemCode(lpszUtf8,lpszCSC,nBufSize) ;
#endif
	}
};

