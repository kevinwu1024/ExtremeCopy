#pragma once

#include <commctrl.h>

class CptWinToolTip
{
public:
	CptWinToolTip(void);
	~CptWinToolTip(void);

	BOOL AddTip(HWND hWnd,HINSTANCE hInst,TCHAR *Tip,UINT id , BOOL Balloon) ;

	bool Create(HWND hParentWnd,bool bBalloon) ;

	bool BindControl(HWND hControlWnd,TCHAR* pTxt) ;
	bool UpdateDisplayText(LPTSTR lpszTxt) ;
	void Destroy() ;

private:
	

private:
	HWND	m_hToolTipWnd ;
};
