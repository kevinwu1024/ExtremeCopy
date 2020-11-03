/****************************************************************************

Copyright (c) 2008-2020 Kevin Wu (Wu Feng)

github: https://github.com/kevinwu1024/ExtremeCopy
site: http://www.easersoft.com

Licensed under the Apache License, Version 2.0 License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

https://opensource.org/licenses/Apache-2.0

****************************************************************************/

#pragma once

#ifdef VERSION_HDD_QUEUE

#include "../Common/ptTypeDef.h"

#include <vector>
#include <list>


const int s_ShareMemTaskQueSize = 1024 ;

#pragma data_seg("XCTaskQueueAA") // "XCTaskQueue"  为自定义
static BYTE s_pShareQueue[s_ShareMemTaskQueSize] = {0} ;
static BYTE	s_byTaskIDCounter = 0 ;
#pragma data_seg()
#pragma comment(linker,"/SECTION:XCTaskQueueAA,RWS")


class CXCTaskQueue
{
public:

	CXCTaskQueue() ;
	CXCTaskQueue(const CXCTaskQueue&) ;

	virtual ~CXCTaskQueue() ;

	// 把当前任务所涉及的HDD ID加入队列中，返回的是在队列的顺序
	int AddToQueue(pt_STL_vector(DWORD) StorageIDVer) ;

	// 等待在队列中是否轮到自己，返回在队列中的位置
	int Wait(DWORD dwInterval) ;

	// 从队列中移除当前任务
	void RemoveFromQueue() ;

	// 在队列最前面的正在运行的任务剩余时间，返回剩余时间，以秒为单位
	DWORD GetTimeLeftOfRunningTask() ;

	// 把当前任务在队列里移动，返回的是在队列的位置
	int MovePriority(bool bForwardOrBackward) ;

	int GetWaitingTaskCount();

	//
	void UpdateRunningTaskRemainTime(DWORD dwRemainTime) ;

private:
	struct SStorageQueInfo
	{
		DWORD dwStorID ;
		DWORD dwRemainTime ;
		pt_STL_list(BYTE)	TaskIDQue ;
	};

public:
	bool LoadData(pt_STL_list(SStorageQueInfo)& sqiList) ;
	int PutData(const pt_STL_list(SStorageQueInfo)& sqiList) ;

	bool Lock() ;
	void Unlock() ;

private:
	HANDLE		m_hMutex;
	BYTE		m_byCurTaskID ;
};

#endif