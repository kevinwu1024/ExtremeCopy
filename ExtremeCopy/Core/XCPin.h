/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#include "XCCoreDefine.h"

class CXCOutputPin ;
class CXCInputPin ;


class CXCPin 
{
public:
	friend CXCOutputPin ;
	friend CXCInputPin ;

	CXCPin();
	virtual ~CXCPin() ;

	bool IsConnected() const ;

	virtual EPinType GetType() {return PinType_Unknown;}
	
	void SetCallBack(CXCFilterEventCB* pPinCB,CXCCopyingEvent* pEvent) ;

	 static bool Connect(CXCPin& InputPin,CXCPin& OutputPin) ;

protected:
	inline int SetData(CXCPin* pOwnerPin,EFilterCmd cmd,void* pFileData) ;
	inline int GetData(CXCPin* pOwnerPin,EFilterCmd cmd,void* pFileData) ;

protected:
	CXCCopyingEvent*	m_pEvent ;
	CXCPin*				m_pAssociatePin;
	CXCFilterEventCB*		m_pPinCB ;
};

class CXCOutputPin : public CXCPin
{
public:
	CXCOutputPin() {}
	virtual EPinType GetType() {return PinType_Output;}
	int PushData(EFilterCmd cmd,void* pFileData) ;
};

class CXCInputPin : public CXCPin
{
public:
	CXCInputPin() {}
	virtual EPinType GetType() {return PinType_Input;}

	int PullData(EFilterCmd cmd,void* pFileData) ;
	int PushData(EFilterCmd cmd,void* pFileData) ;
};

