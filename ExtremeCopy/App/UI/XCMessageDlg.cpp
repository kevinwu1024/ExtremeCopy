#include "StdAfx.h"
#include "XCMessageDlg.h"


CXCMessageDlg::CXCMessageDlg()
	:CptDialog(IDD_DIALOG_MESSAGE,NULL,CptMultipleLanguage::GetInstance()->GetResourceHandle())
{
}


CXCMessageDlg::~CXCMessageDlg(void)
{
	Debug_Printf(_T("CXCMessageDlg::~CXCMessageDlg()")) ;
}


BOOL CXCMessageDlg::OnInitDialog()
{
	int nScreenWidth = 0 ;
	m_nStayTimeCounter = 0 ;
	m_nScreenHeight = 0 ;

	CptGlobal::GetScreenSize(&nScreenWidth,&m_nScreenHeight) ;

	SptRect rect;
	::GetWindowRect(this->GetSafeHwnd(),rect.GetRECTPointer()) ;

	::SetWindowPos(this->GetSafeHwnd(),HWND_TOPMOST,nScreenWidth-rect.GetWidth(),m_nScreenHeight-rect.GetHeight(),0,0,SWP_NOSIZE) ;

	m_MovmentState = MS_Unknown ;

	this->ChangeState(MS_MoveUp) ;

	return FALSE ;
}

void CXCMessageDlg::OnClose()
{
	delete this ;// 这是一个非模态对话框，所以在关闭时就释放自己
}

void CXCMessageDlg::SetMessage(const CptString& strMsg) 
{
	m_strMessage = strMsg ;
}

bool CXCMessageDlg::OnCancel()
{
	this->ChangeState(MS_MoveDown) ;

	return false ;
}


void CXCMessageDlg::OnButtonClick(int nButtonID)
{
	//if(nButtonID==IDC_BUTTON_CLOSE)
	//{
	//}
}

void CXCMessageDlg::OnTimer(int nTimerID) 
{
	switch(nTimerID)
	{
	case TIMER_ID_MOVEUP:// 向上移动
		{
			const int nStep = 2 ;
			SptRect rect;
			::GetWindowRect(this->GetSafeHwnd(),rect.GetRECTPointer()) ;

			if(rect.nBottom>m_nScreenHeight)
			{
				::SetWindowPos(this->GetSafeHwnd(),HWND_TOPMOST,rect.nLeft,rect.nTop-nStep,0,0,SWP_NOSIZE) ;
			}
			else
			{
				this->ChangeState(MS_Stay) ;
			}
		}
		break ;

	case TIMER_ID_MOVEDOWN: // 向下移动
		break ;

	case TIMER_ID_STAY: // 停留
		{
			// 如果超过停留时间，则自行退出
			if(m_nStayTimeCounter++ >MESSAGE_WINDOW_STAY_LAST_TIME)
			{
				this->ChangeState(MS_MoveDown) ;
			}
		}
		break ;
	}
}

void CXCMessageDlg::ChangeState(const EMovmentState NewState)
{
	if(m_MovmentState==NewState)
	{
		return ;
	}

	switch(NewState)
	{
	case MS_MoveDown:
		{
			_ASSERT(m_MovmentState==MS_MoveUp || m_MovmentState==MS_Stay) ;

			m_MovmentState = MS_MoveDown ;

			// 不进行 move down 直接关闭窗口
			this->Close() ; 
		}
		break ;

	case MS_MoveUp:
		{
			_ASSERT(m_MovmentState==MS_Unknown) ;

			m_MovmentState = MS_MoveUp ;

			this->SetTimer(TIMER_ID_MOVEUP,50) ;

		}
		break ;

	case MS_Stay:
		{
			_ASSERT(m_MovmentState==MS_MoveUp) ;

			m_MovmentState = MS_Stay ;

			this->SetTimer(TIMER_ID_STAY,1000) ;
		}
		break ;

	default:
		_ASSERT(FALSE) ;
		break ;
	}


	//if(bMoveUp)
	//{
	//	_ASSERT(MS_Unknown==m_MovmentState) ;

	//	if(m_MovmentState!=MS_MoveUp)
	//	{
	//		m_MovmentState =  MS_MoveDown ;

	//		this->SetTimer(TIMER_ID_MOVEUP) ;
	//	}
	//}
	//else
	//{
	//	if(m_MovmentState!=MS_MoveDown)
	//	{
	//		m_MovmentState =  MS_MoveDown ;

	//		// 关闭所有可能正在进行中的 timer
	//		this->KillTimer(TIMER_ID_MOVEUP) ;
	//		this->KillTimer(TIMER_ID_STAY) ;

	//		this->SetTimer(TIMER_ID_MOVEDOWN) ;
	//	}
	//}
	
}