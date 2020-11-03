/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCPin.h"


CXCPin::CXCPin():m_pAssociatePin(NULL)
{
	this->SetCallBack(NULL,NULL) ;
}

CXCPin::~CXCPin(void)
{
}

bool CXCPin::IsConnected() const 
{
	return (m_pAssociatePin!=NULL) ;
}

void CXCPin::SetCallBack(CXCFilterEventCB* pPinCB,CXCCopyingEvent* pEvent) 
{
	m_pPinCB = pPinCB ; 
	m_pEvent = pEvent ;
}
/**
void* CXCPin::Allocate(int nSize) 
{
if(m_pPinCB!=NULL)
{
if(m_pPinCB->CanAllocate(this))
{
return m_pPinCB->Allocate(this,nSize) ;
}
else if(m_pAssociatePin!=NULL && m_pAssociatePin->CanAllocate())
{
return m_pAssociatePin->Allocate(nSize) ;
}
}

return NULL ;
}

bool CXCPin::CanAllocate()  
{
if(m_pPinCB==NULL)
{
return false;
}
else
{
return m_pPinCB->CanAllocate(this) ;
}
}
/**/

int CXCPin::SetData(CXCPin* pOwnerPin,EFilterCmd cmd,void* pFileData) 
{
	int nRet = 0 ;

	if(m_pPinCB!=NULL)
	{
//		nRet = m_pPinCB->OnPin_Data(pOwnerPin,cmd,pFileData) ;
	}

	return nRet ;
}

int CXCPin::GetData(CXCPin* pOwnerPin,EFilterCmd cmd,void* pFileData) 
{
	int nRet = 0 ;

	if(m_pPinCB!=NULL)
	{
		//nRet = m_pPinCB->OnPin_Data(pOwnerPin,cmd,pFileData) ;
	}

	return nRet ;
}

bool CXCPin::Connect(CXCPin& InputPin,CXCPin& OutputPin) 
{
	if(InputPin.IsConnected() || OutputPin.IsConnected()
		|| OutputPin.GetType()==InputPin.GetType())
	{
		return false ;
	}

	OutputPin.m_pAssociatePin = &InputPin ;
	InputPin.m_pAssociatePin = &OutputPin ;

	return true ;
}

int CXCOutputPin::PushData(EFilterCmd cmd,void* pFileData) 
{
	int nRet = 0 ;

	if(m_pAssociatePin!=NULL)
	{
		m_pAssociatePin->SetData(this,cmd,pFileData) ;
	}

	return nRet ;
}

int CXCInputPin::PullData(EFilterCmd cmd,void* pFileData) 
{
	int nRet = 0 ;

	if(m_pAssociatePin!=NULL)
	{
		m_pAssociatePin->GetData(this,cmd,pFileData) ;
	}

	return nRet ;
}

int CXCInputPin::PushData(EFilterCmd cmd,void* pFileData) 
{
	int nRet = 0 ;

	if(m_pAssociatePin!=NULL)
	{
		m_pAssociatePin->SetData(this,cmd,pFileData) ;
	}

	return nRet ;
}
