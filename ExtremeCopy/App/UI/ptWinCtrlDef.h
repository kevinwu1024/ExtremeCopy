
#pragma once

// 自定义消息
#define WM_CONTROLSTATUSCHANGED		WM_USER+800  // 控件状态改变 (wParam: 控件ID; lParam:新状态[EControlStatus 类型])

// 控件鼠标状态
enum EControlMouseStatus
{
	ControlMouseStatus_Normal = 0,
	ControlMouseStatus_Leave = 1,
	ControlMouseStatus_Hover = 2,
	ControlMouseStatus_Down = 3,
};

// 控件有效状态
enum EControlEnableStatus
{
	ControlEnableStatus_Enable = 0,
	ControlEnableStatus_Disable = 1,
};

struct SControlStatus
{//控件状态
	union
	{
		struct SStatus
		{
			EControlMouseStatus MouseStatus : 4 ;
			int Enable : 1 ;
		} status ;
		unsigned int uStatusBlock ;
	};

	SControlStatus()
	{
		this->status.Enable = ControlEnableStatus_Enable ;
		this->status.MouseStatus = ControlMouseStatus_Normal ;
	}
};

