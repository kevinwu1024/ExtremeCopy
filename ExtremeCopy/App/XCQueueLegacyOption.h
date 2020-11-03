/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#pragma data_seg("XCQueueLegacyOptionAA") // "XCQueueLegacyOptionAA"  为自定义
static unsigned	s_QueueLegacyOption = 0 ;
#pragma data_seg()
#pragma comment(linker,"/SECTION:XCQueueLegacyOptionAA,RWS")

#define QLO_MASK_ShutdownAfterTaskDone		(1<<0)

class CXCQueueLegacyOption
{
public:
	CXCQueueLegacyOption(void);
	virtual ~CXCQueueLegacyOption(void);

	static bool GetOption(const unsigned mask);
	static void SetOption(const unsigned mask);

private:
};

