
// Hyperlink.cpp -- Implementation of CHyperlink class
// Ryan Lederman -- ryan@winprog.org

#include "stdafx.h"
#include "Hyperlink.h"
#include <WinUser.h>

CHyperlink::CHyperlink():m_pCallBackArg(NULL) 
{
	m_nDynamicCreate = false ;
	m_hWnd = NULL ;
}

CHyperlink::~CHyperlink()
{

}

bool CHyperlink::Create( HYPERLINKSTRUCT2* phs )
{
	if(phs==NULL || phs->hControlWnd==NULL)
	{
		return false ;
	}

	m_hWnd = phs->hControlWnd ;

	if( SetProp( m_hWnd, _T("CLASSPOINTER"), (HANDLE)this ) == 0 )
		return false;

#ifdef _WIN64
	SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)HyperlinkProc);
#else
	::SetWindowLong(m_hWnd, GWLP_WNDPROC, (LONG)HyperlinkProc) ; // 恢复旧的控件消息处理函数入口
#endif

	int iFontBold = 0;
	int iFontUnderline = 0;
	int iHighlightFontUnderline = 0;

	phs->bBold ? iFontBold = FW_BOLD : iFontBold = FW_NORMAL;

	m_hFont = CreateFont( phs->fontsize ,0,0,0,iFontBold,0, phs->bUnderline,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH, phs->szFontName );

	if( m_hFont == NULL )
		return false;

	if( phs->bUseBg )
	{
		m_hBrushBackground = CreateSolidBrush( phs->clrBack );
		m_bUseBg = true;
	}
	else
	{	
		m_bUseBg = false;
	}
	
	m_bUseCursor = phs->bUseCursor; 
	
	if( m_bUseCursor )
		m_hCursor = phs->hCursor;

	m_rgbForeground = phs->clrText;
	m_rgbHighlight  = phs->clrHilite;
	m_rgbBackground = phs->clrBack;

	if( phs->pFn )
		m_pFn = phs->pFn;

	m_pCallBackArg = phs->pCBAgr ;

	int Len = ::GetWindowTextLength(m_hWnd) + 2 ;

	const int nAllSize = (Len + 1)*sizeof(TCHAR) ;
	m_szBuffer = (TCHAR*)malloc( nAllSize );

	if( m_szBuffer == NULL )
		return false;
	
	memset( m_szBuffer, 0, nAllSize );
	::GetWindowText(m_hWnd,m_szBuffer,nAllSize) ;
	//::_tcscpy( m_szBuffer, phs->szCaption );

	m_iLastState = -1;

	m_nDynamicCreate = false ;

	return true;
}

bool CHyperlink::Create( PHYPERLINKSTRUCT phs )
{
	if( !IsWindow( phs->hWndParent ) )
		return false;

	if( phs->width <= 0 || phs->height <= 0 )
		return false;

	if( !phs->szFontName )
		return false;

	m_hWnd = CreateWindow( _T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE, phs->coordX, phs->coordY, phs->width, phs->height,
						   phs->hWndParent, (HMENU)phs->ctlID, GetModuleHandle( NULL ), NULL );

	if( m_hWnd == NULL )
		return false;

	if( SetProp( m_hWnd, _T("CLASSPOINTER"), (HANDLE)this ) == 0 )
		return false;

		int iFontBold = 0;
	int iFontUnderline = 0;
	int iHighlightFontUnderline = 0;

#ifdef _WIN64
	SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)HyperlinkProc);
#else
	::SetWindowLong(m_hWnd, GWLP_WNDPROC, (LONG)HyperlinkProc) ; // 恢复旧的控件消息处理函数入口
#endif

	phs->bBold ? iFontBold = FW_BOLD : iFontBold = FW_NORMAL;

	m_hFont = CreateFont( phs->fontsize ,0,0,0,iFontBold,0, phs->bUnderline,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH, phs->szFontName );

	if( m_hFont == NULL )
		return false;

	if( phs->bUseBg )
	{
		m_hBrushBackground = CreateSolidBrush( phs->clrBack );
		m_bUseBg = true;
	}
	else
	{	
		m_bUseBg = false;
	}
	
	m_bUseCursor = phs->bUseCursor; 
	
	if( m_bUseCursor )
		m_hCursor = phs->hCursor;

	m_rgbForeground = phs->clrText;
	m_rgbHighlight  = phs->clrHilite;
	m_rgbBackground = phs->clrBack;

	if( phs->pFn )
		m_pFn = phs->pFn;

	m_pCallBackArg = phs->pCBAgr ;

	const int nAllSize = (const int)((::_tcslen( phs->szCaption ) + 1)*sizeof(TCHAR)) ;
	m_szBuffer = (TCHAR*)malloc( nAllSize );

	if( m_szBuffer == NULL )
		return false;
	
	memset( m_szBuffer, 0, nAllSize );
	::_tcscpy( m_szBuffer, phs->szCaption );

	m_iLastState = -1;

	m_nDynamicCreate = true ;

	return true;
}

LRESULT CALLBACK CHyperlink::HyperlinkProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	long long pThisClass = (long long)GetProp( hWnd, _T("CLASSPOINTER") );

	switch( msg )
	{
		case WM_PAINT: 
			((CHyperlink*)pThisClass)->OnDraw(); 
			break;
		case WM_MOUSEMOVE: ((CHyperlink*)pThisClass)->OnMouseMove(); break;
		case WM_LBUTTONDOWN: 
			if( ((CHyperlink*)pThisClass)->m_pFn )
				((CHyperlink*)pThisClass)->m_pFn( ((CHyperlink*)pThisClass)->m_pCallBackArg );
			break;
		case WM_CAPTURECHANGED:
			((CHyperlink*)pThisClass)->OnMouseMove(); 
			break;
		case WM_SETCURSOR:
			if( ((CHyperlink*)pThisClass)->m_bUseCursor )
			{
				((CHyperlink*)pThisClass)->m_hStockCursor = SetCursor( ((CHyperlink*)pThisClass)->m_hCursor );
				return true;
			}
			return false;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

void CHyperlink::Update()
{
	//((CHyperlink*)pThisClass)->OnDraw(); 
	if(m_hWnd!=NULL)
	{
		this->OnDraw() ;
	}
}

void CHyperlink::OnDraw( void )
{
//	PAINTSTRUCT ps = {0};
	HDC			dc;
	RECT		winRect = {0};

//	dc = BeginPaint( m_hWnd, &ps );
	dc = ::GetDC(m_hWnd) ;
	GetClientRect( m_hWnd, &winRect );

	if( m_bUseBg )
	{
		FillRect( dc, &winRect, m_hBrushBackground );
		SetBkColor( dc, m_rgbBackground );
	}
	else
	{
		SetBkMode( dc, TRANSPARENT );
	
	}

	SelectObject( dc, m_hFont );

	if( m_bMouseOver )
	{
		SetTextColor( dc, m_rgbHighlight );
	}
	else
	{
		SetTextColor( dc, m_rgbForeground );
	}

	DrawTextEx( dc, m_szBuffer, (int)::_tcslen( m_szBuffer ), &winRect, DT_VCENTER | DT_CENTER, NULL );

	::ReleaseDC(m_hWnd,dc) ;
	//::DeleteDC(dc) ;
//	EndPaint( m_hWnd, &ps );
}

void CHyperlink::OnMouseMove()
{
	RECT  winRect = {0};
	POINT pt	  = {0};

	GetCursorPos( &pt );

	GetWindowRect( m_hWnd, &winRect );

	if( PtInRect( &winRect, pt ) )
	{
		SetCapture( m_hWnd );
		m_bMouseOver = true;

		if( m_iLastState != S_MOUSEOVER )
		{
			InvalidateRect( m_hWnd, NULL, TRUE );
			UpdateWindow( m_hWnd );
		}
		m_iLastState = S_MOUSEOVER;
	}
	else
	{
		ReleaseCapture();
		m_bMouseOver = false;

		if( m_iLastState != S_MOUSENOTOVER )
		{
			InvalidateRect( m_hWnd, NULL, TRUE );
			UpdateWindow( m_hWnd );
		}
		m_iLastState = S_MOUSENOTOVER;
	}
}

void CHyperlink::Destroy( void )
{
	if(m_hFont!=NULL)
	{
		DeleteObject( m_hFont );
		m_hFont = NULL ;
	}
	
	if(m_hBrushBackground!=NULL)
	{
		DeleteObject( m_hBrushBackground );
	}

	if(m_nDynamicCreate)
	{
		DestroyWindow( m_hWnd );
	}

	if( m_bUseCursor )
		DeleteObject( m_hCursor );

	if( m_szBuffer )
		free( m_szBuffer );
}

