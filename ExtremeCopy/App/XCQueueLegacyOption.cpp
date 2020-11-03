/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#include "StdAfx.h"
#include "XCQueueLegacyOption.h"


CXCQueueLegacyOption::CXCQueueLegacyOption(void)
{
}


CXCQueueLegacyOption::~CXCQueueLegacyOption(void)
{
}

bool CXCQueueLegacyOption::GetOption(const unsigned mask)
{
	return (s_QueueLegacyOption & QLO_MASK_ShutdownAfterTaskDone);
}

void CXCQueueLegacyOption::SetOption(const unsigned mask)
{
	s_QueueLegacyOption |= mask;
}