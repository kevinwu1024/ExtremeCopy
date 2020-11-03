
// Hyperlink.h   -- Definition of CHyperlink class
// Ryan Lederman -- ryan@winprog.org

#ifndef _HYPERLINK_H
#define _HYPERLINK_H

#include "windows.h"

#define S_MOUSEOVER	   0
#define S_MOUSENOTOVER 1

typedef void (__cdecl* PHYPERLINKFN)( void* pVoid );	// Callback function

typedef struct {
	HWND hWndParent;		// Handle to parent window
	int  ctlID;				// Identifier used for control
	int  coordX;			// X axis coordinate of upper left corner
	int  coordY;			// Y axis coordinate of upper left corner
	int  width;				// Width of control, in pixels
	int  height;			// Height of control, in pixels

	TCHAR *szFontName;		// Face name of font used
	int  fontsize;			// Size of font
	bool bBold;				// Bold font?
	bool bUnderline;		// Underline font normally?

	COLORREF clrText;		// Foreground color (text color) of control
	COLORREF clrHilite;		// Highlighted color of text (mouse over, if enabled)
	COLORREF clrBack;		// Background color used (if applicable)

	
	bool bUseBg;			// Use background color, or just make control transparent?
	bool bUseCursor;		// Use custom curosr?
	HCURSOR hCursor;		// Handle to custom cursor, if used.
	PHYPERLINKFN pFn;		// Function pointer for function that is executed when user clicks
	void*			pCBAgr ;
	TCHAR *szCaption;		// Caption
} HYPERLINKSTRUCT, *PHYPERLINKSTRUCT;


struct HYPERLINKSTRUCT2
{
	HWND hControlWnd;		// Handle to parent window

	TCHAR *szFontName;		// Face name of font used
	int  fontsize;			// Size of font
	bool bBold;				// Bold font?
	bool bUnderline;		// Underline font normally?

	COLORREF clrText;		// Foreground color (text color) of control
	COLORREF clrHilite;		// Highlighted color of text (mouse over, if enabled)
	COLORREF clrBack;		// Background color used (if applicable)

	bool bUseBg;			// Use background color, or just make control transparent?
	bool bUseCursor;		// Use custom curosr?
	HCURSOR hCursor;		// Handle to custom cursor, if used.
	PHYPERLINKFN pFn;		// Function pointer for function that is executed when user clicks
	void*		 pCBAgr ;
} ;

class CHyperlink 
{
public:				// Public methods
	CHyperlink();
	~CHyperlink();
	bool Create( HYPERLINKSTRUCT2* phs ) ;
	bool Create( PHYPERLINKSTRUCT phs );
	void Destroy();
	void Update() ;
private:			// Private methods
	static LRESULT CALLBACK HyperlinkProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnDraw();
	void OnMouseMove();
public:				// Public members
	HWND m_hWnd;

private:			// Private members
	HFONT		 m_hFont;
	HBRUSH		 m_hBrushBackground;
	HCURSOR		 m_hCursor;
	HCURSOR		 m_hStockCursor;
	COLORREF	 m_rgbForeground;
	COLORREF	 m_rgbHighlight;
	COLORREF	 m_rgbBackground;
	PHYPERLINKFN m_pFn;
	bool         m_bUseBg;
	bool		 m_bMouseOver;
	bool		 m_bUseCursor;
	TCHAR		*m_szBuffer;
	int			 m_iLastState;
	void*		m_pCallBackArg ;

	bool			m_nDynamicCreate ;
};

#endif /* _HYPERLINK_H */