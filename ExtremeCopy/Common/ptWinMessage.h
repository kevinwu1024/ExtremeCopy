/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "ptTypeDef.h"

struct SMouseMessage
{
	enum EMouseMsgType
	{
		MMT_LEFTBUTTONDOWN,
		MMT_LEFTBUTTONDOUBLEClICK,
		MMT_LEFTBUTTONUP,
		MMT_RIGHTBUTTONDOWN,
		MMT_RIGHTBUTTONUP,
		MMT_RIGHTBUTTONDOUBLEClICK,

		MMT_WHEEL,
		MMT_MOVE,
		MMT_UNKONWN
	};

	EMouseMsgType	MsgType ;
	SptPoint		ptPos ;
	int				nDisstance ;
	int				OtherKeys ;

	SMouseMessage():MsgType(MMT_UNKONWN)
	{
	}

	SMouseMessage(int nMsg,WPARAM wParam,LPARAM lParam)
	{
		this->SetMessage(nMsg,wParam,lParam) ;
	}

	bool IsMouseMessage(int nMsg) const
	{
		bool bRet = false ;
		//ptPos.Clear() ;

		switch(nMsg)
		{
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MOUSEWHEEL:
			bRet = true ;
			break ;
		}
		return bRet ;
	}

	bool SetMessage(int nMsg,WPARAM wParam,LPARAM lParam)
	{
		bool bRet = false ;

		MsgType = MMT_UNKONWN ;
		ptPos.Clear() ;
		nDisstance = 0 ;
		OtherKeys = 0 ;

		switch(nMsg)
		{
		case WM_MOUSEMOVE: MsgType = MMT_MOVE ;break ;
		case WM_LBUTTONDOWN: MsgType = MMT_LEFTBUTTONDOWN ;break ;
		case WM_LBUTTONUP:MsgType = MMT_LEFTBUTTONUP ;break ;
		case WM_LBUTTONDBLCLK:MsgType = MMT_LEFTBUTTONDOUBLEClICK ;break ;
		case WM_RBUTTONDOWN:MsgType = MMT_RIGHTBUTTONDOWN ;break ;
		case WM_RBUTTONUP:MsgType = MMT_RIGHTBUTTONUP ;break ;
		case WM_RBUTTONDBLCLK:MsgType = MMT_RIGHTBUTTONDOUBLEClICK ;break ;

		case WM_MOUSEWHEEL:
			MsgType = MMT_WHEEL ;
			nDisstance = HIWORD(wParam) ;
			
			break ;
		}

		if(MsgType!=MMT_UNKONWN)
		{
			ptPos.nX = LOWORD(lParam) ;
			ptPos.nY = HIWORD(lParam) ;

			OtherKeys = LOWORD(wParam) ;

			bRet = true ;
		}

		return bRet ;
		
	}
};