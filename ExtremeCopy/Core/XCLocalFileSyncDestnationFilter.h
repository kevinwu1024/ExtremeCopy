/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once
#include "xcdestinationfilter.h"

class CXCLocalFileSyncDestnationFilter :
	public CXCLocalFileDestnationFilter
{
public:
	CXCLocalFileSyncDestnationFilter(CXCCopyingEvent* pEvent,const CptString strDestRoot,const SStorageInfoOfFile& siof,const bool bIsRenameCopy);
	virtual ~CXCLocalFileSyncDestnationFilter(void);

	virtual int WriteFileData(SDataPack_FileData& fd)  ;

	virtual bool OnInitialize() ;
	virtual bool OnContinue() ;
	virtual bool OnPause();
	virtual void OnStop();

protected:
	virtual void OnLinkEnded(pt_STL_list(SDataPack_SourceFileInfo*)& FileList) ;
};

