/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include <ShObjIdl.h>


class CWin7TaskbarProgress
{
public:
	CWin7TaskbarProgress() ;
	~CWin7TaskbarProgress() ;

	void SetProgressValue(HWND hWnd,int nValue) ; // nValue值的范围0~ 100， 以百分比输入， 若70%，则value为 70
private:
	ITaskbarList3*			m_pTaskbarList ;
};