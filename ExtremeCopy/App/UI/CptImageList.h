/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "..\..\Common\ptTypeDef.h"
#include "commctrl.h"


class CptImageList
{
public:
	CptImageList(void);
	CptImageList(const CptImageList&);

	~CptImageList(void);

	CptImageList& operator=(const CptImageList&) ;

	bool Create(SptSize size,int nFlag=ILC_COLOR32|ILC_MASK,int nInitial=0,int nGrow=1) ;
	void Destroy() ;

	bool Copy(CptImageList& ImageList,int nIndex,UINT uFlags) ;

	inline bool IsValid() const ;

	int GetImageCount() ;
	bool SetImageCount(int nNewCount) ;

	COLORREF SetBkColor(COLORREF crNew) ;
	COLORREF GetBkColor() ;

	int ReplaceIcon(int nIndex,HICON hNewIcon) ;
	//int AddIcon(HICON hNewIcon,const SptSize& size) ;
	int AddIcon(HICON hNewIcon) ;
	int AddIcon(LPCTSTR lpFile) ;
	HICON GetIcon(int nIndex,UINT uFlags) ;
	
	bool Remove(int nIndex) ;
	bool RemoveAll() ;

	bool Draw(HDC hDC,int nIndex,SptPoint pt,UINT fStyle) ;

	bool LoadImage(HINSTANCE hi, LPCTSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags);

	HIMAGELIST GetHandle() ;

private:
	int AddRef() const ;
	void CopyOnWrite() ;

private:
	struct SImageListInfo
	{
		int				nRefCount ;
		HIMAGELIST		hHandle ;

		SImageListInfo()
		{
			nRefCount = 0 ;
			hHandle = NULL ;
		}
	};

private:
	//HIMAGELIST		m_hHandle ;
	SImageListInfo*		m_pImageList ;
};
