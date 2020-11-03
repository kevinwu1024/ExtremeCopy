/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#define		VERSION_PROFESSIONAL	// 专业版本

#ifndef VERSION_PROFESSIONAL
//#define		VERSION_PORTABLE		// portable version
#endif

//#define		VERSION_TEST_BETA
//#define		VERSION_TEST_ALPHA

#define		TEST_VERSION_NUMBER		5	// 测试版本号

// 这是用于公开的测试版的检测日期的，这日期是有效的测试版试用日期
#define		BETA_VERSION_EXPIRE_DATE	_T("2012-10-01 11:00:00")

//#define COMPILE_NOTDATAOCCUREDREAD		//当读数据发生时不回调

#define		VERSION_UNLIMIT_FILE_NAME_LENGTH	// 是否应用无限制长度的文件名

//#define		COMPILE_TEST_PERFORMANCE		// 启用检测特定功能模块的性能

#if defined(VERSION_TEST_BETA) || defined(VERSION_TEST_ALPHA)
#define VERSION_TEST // 是否测试版本
#endif


#if		defined(VERSION_PROFESSIONAL) && !defined(VERSION_TEST) && !defined(_DEBUG)
//#define		VERSION_CHECKREGSITER // 是否使用注册码
#endif

#if defined(VERSION_PROFESSIONAL)
#define		VERSION_HDD_QUEUE // 是否对HDD进行队列复制
#endif

/////---------------------------------------

// message 窗口完全弹出后的停留时间
#define		MESSAGE_WINDOW_STAY_LAST_TIME		8