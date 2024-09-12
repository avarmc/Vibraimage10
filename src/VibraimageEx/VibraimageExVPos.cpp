#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExVPos.h"
#include "round.h"


IMPLEMENT_DYNAMIC(CVibraimageExVPos, CPropertiesWndBase)

CVibraimageExVPos::CVibraimageExVPos(CPropertiesPack *pPack, int iid) : CPropertiesWndBase(pPack, iid)
{
	theApp.m_pVPos = this;
	m_vPos = -1;
}


CVibraimageExVPos::~CVibraimageExVPos()
{
}

BEGIN_MESSAGE_MAP(CVibraimageExVPos, CPropertiesWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(IDRETRY, &CVibraimageExVPos::OnActionPause)
	ON_UPDATE_COMMAND_UI(IDRETRY, &CVibraimageExVPos::OnUpdateActionPause)
END_MESSAGE_MAP()




int CVibraimageExVPos::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertiesWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	GetClientRect(&rc);

	int flags = WS_CHILD | WS_VISIBLE |  TBS_TOOLTIPS | TBS_AUTOTICKS;

	m_wndSlider.Create(flags,rc,this, ID_SLIDER);
	m_wndSlider.SetTicFreq(1000);
	m_wndSlider.EnableWindow(FALSE);
	
	m_btnPause.Create(_T("[]"), flags | BS_PUSHBUTTON| BS_CENTER|BS_VCENTER, rc, this, IDRETRY);


	SetTimer(1, 100, 0);
	Align();

	
	return 0;
}


void CVibraimageExVPos::OnSize(UINT nType, int cx, int cy)
{
	CPropertiesWndBase::OnSize(nType, cx, cy);

	Align();
}


void CVibraimageExVPos::OnNewVar(int id, int subID)
{
}

bool CVibraimageExVPos::Load(xml_node& xml)
{
	return CPropertiesWndBase::Load(xml);
}

void CVibraimageExVPos::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	Align();
}

void CVibraimageExVPos::Align()
{
	if (!m_hWnd)
		return;

	CRect rc,rS,rB;
	GetClientRect(&rc);

	rB = rS = rc;

	if (rc.Width() > rc.Height())
	{
		m_wndSlider.ModifyStyle(TBS_VERT, 0);
		rB.left = rB.right - max(rc.Height(),100);
		m_btnPause.MoveWindow(&rB);

		rS.right = rB.left;
		m_wndSlider.MoveWindow(&rS);
	}
	else
	{
		m_wndSlider.ModifyStyle(0, TBS_VERT);

		rB.bottom = rB.top + max(rc.Width(),30);
		m_btnPause.MoveWindow(&rB);

		rS.top = rB.bottom;
		m_wndSlider.MoveWindow(&rS);
	}
}


BOOL CVibraimageExVPos::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	
	pDC->FillSolidRect(&rc, GetSysColor(COLOR_3DFACE));
	return TRUE;
}


void CVibraimageExVPos::OnTimer(UINT_PTR nIDEvent)
{

	if (nIDEvent == 1)
	{
		if (m_vPos >= 0)
		{
			theApp.m_pEngine->engine.PutVar(VI_INFO_TIME_VIDEO_POS, 0, COleVariant(m_vPos / 1000.0f));
			m_vPos = -1;
		}

		int duration = round_i( theApp.m_pEngine->engine.GetVar(VI_INFO_TIME_VIDEO_LEN,0,VT_R4).fltVal*1000 ); // millisec
		int pos = round_i( theApp.m_pEngine->engine.GetVar(VI_INFO_TIME_VIDEO_POS, 0, VT_R4).fltVal*1000 ); // millisec

		if (m_wndSlider.m_hWnd)
		{
			m_wndSlider.EnableWindow(duration > 0);

			if (m_wndSlider.GetRangeMax() != duration)
			{
				m_wndSlider.SetRange(0, duration);
				m_wndSlider.SetPos(0);
				m_wndSlider.RedrawWindow();
			}

			if (m_wndSlider.GetPos() != pos && m_vPos == -1 && pos <= duration)
				m_wndSlider.SetPos(pos);
		}


		return;
	}

	__super::OnTimer(nIDEvent);
}


void CVibraimageExVPos::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	OnScroll(nSBCode, nPos, pScrollBar);
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CVibraimageExVPos::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	OnScroll(nSBCode, nPos, pScrollBar);
	__super::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CVibraimageExVPos::OnScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar && pScrollBar->m_hWnd == m_wndSlider.m_hWnd)
	{
		switch (nSBCode)
		{
		case SB_ENDSCROLL:	//   End scroll.
			m_vPos = m_wndSlider.GetPos();
			return;
		default: break;
		}
		m_vPos = -2;
		theApp.m_pEngine->engine.PutVar(VI_INFO_TIME_VIDEO_STATE, 0, COleVariant((long)2));
		
	}
	__super::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CVibraimageExVPos::OnActionPause()
{
	int state = theApp.m_pEngine->engine.GetVar(VI_INFO_TIME_VIDEO_STATE, 0, VT_INT).intVal;
	theApp.m_pEngine->engine.PutVar(VI_INFO_TIME_VIDEO_STATE, 0, COleVariant(state == 2 ? (long)1 : (long) 2));
}


void CVibraimageExVPos::OnUpdateActionPause(CCmdUI *pCmdUI)
{
	int duration = round_i(theApp.m_pEngine->engine.GetVar(VI_INFO_TIME_VIDEO_LEN, 0, VT_R4).fltVal * 1000); // millisec
	int state = theApp.m_pEngine->engine.GetVar(VI_INFO_TIME_VIDEO_STATE, 0, VT_INT).intVal;
	int pos = round_i(theApp.m_pEngine->engine.GetVar(VI_INFO_TIME_VIDEO_POS, 0, VT_R4).fltVal * 1000); // millisec

	CString str;
	str.Format(_T("%0.3f"), pos / 1000.0);
	pCmdUI->SetText(str);
	pCmdUI->Enable(duration > 0);
	pCmdUI->SetCheck(state == 2 ? BST_CHECKED: BST_UNCHECKED);
}
