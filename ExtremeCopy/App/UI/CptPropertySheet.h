/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include <prsht.h>
#include "ptCommCtrl.h"
#include <vector>


class CptPropertySheet : public CptCommCtrl
{
public:
	CptPropertySheet(void);
	virtual ~CptPropertySheet(void);

	int AddPage(const PROPSHEETPAGE& page) ;
	void ClearPage() ;

	void ShowDialog() ;
	void ShowDialogAsWizard() ;
	void Show(HWND hParent) ;

private:

	std::vector<PROPSHEETPAGE> m_PageVer ;
};
