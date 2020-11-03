#pragma once
#include "ptDialog.h"
#include "..\XCGlobal.h"

class CXCMessageDlg : public CptDialog
{
public:
	CXCMessageDlg();
	virtual ~CXCMessageDlg(void);

	void SetMessage(const CptString& strMsg) ;

private:
	enum EMovmentState
	{
		MS_Unknown,
		MS_MoveUp,
		MS_MoveDown,
		MS_Stay
	};

protected:
	virtual BOOL OnInitDialog() ;
	virtual void OnTimer(int nTimerID) ;
	virtual void OnButtonClick(int nButtonID) ;
	virtual bool OnCancel() ;

	virtual void OnClose() ;
	void ChangeState(const EMovmentState NewState) ;



#define TIMER_ID_MOVEUP		200
#define TIMER_ID_MOVEDOWN	201
#define TIMER_ID_STAY		202

	CptString		m_strMessage ;
	EMovmentState	m_MovmentState ; 
	int				m_nScreenHeight ;
	int				m_nStayTimeCounter ;
	//int				m_nWndHeight ;
};

