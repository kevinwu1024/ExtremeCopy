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
#include "..\Common\ptTypeDef.h"
#include <vector>
#include "XCCoreDefine.h"

class CXCSameDriveMoveFile
{
public:
	CXCSameDriveMoveFile(ECopyFileState* pRunState,EImpactFileBehaviorResult* pBehaviorResult);
	virtual ~CXCSameDriveMoveFile(void);

	bool Run(const SGraphTaskDesc& gtd) ;

private:
	int CheckAndUpdateImpactFileName(const SGraphTaskDesc& gtd,CptString strSrcFile,CptString& strDstFile) ;
	ErrorHandlingResult MoveItems(const SGraphTaskDesc& gtd,CptString strSubDir,CptString strParentDir) ; // 从子目录移动其所有的文件和文件夹到其同名的父目录里
	ErrorHandlingResult MoveFile(const SGraphTaskDesc& gtd,CptString strSrcFile,CptString strDstFolder,bool bFolder) ;

	// 判断目录关系：是否源目录的父目录是目标目录的子目录，且源目录与该父目录同名
	inline bool IsSameNameAndSubdir(CptString strSrcDir,CptString strDstDir) ;

private:
	EImpactFileBehaviorResult	*m_pImpactFileBehavior ; // 冲突文件的处理方式
	ECopyFileState			*m_pRunningState ;	// 运行状态
};

