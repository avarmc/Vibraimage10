// VibraimageExQT.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExAIT.h"
#include "PropertiesPack.h"
#include "VibraimageExFrame.h"

// CVibraimageExAIT

IMPLEMENT_DYNAMIC(CVibraimageExAIT, CPropertiesWndBase)

CVibraimageExAIT::CVibraimageExAIT(CPropertiesPack *pPack, int iid) 
	: CPropertiesWndBase(pPack, iid)
	, m_wndProgress(1)
{
	if (!theApp.m_pAIT)
		theApp.m_pAIT = this;
	m_wndProgress.m_font2ScaleSet = 0.7;
}

CVibraimageExAIT::~CVibraimageExAIT()
{
	if (theApp.m_pAIT == this)
		theApp.m_pAIT = 0;
}


BEGIN_MESSAGE_MAP(CVibraimageExAIT, CPropertiesWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
//	ON_WM_TIMER()
END_MESSAGE_MAP()



// CVibraimageExAIT message handlers




int CVibraimageExAIT::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertiesWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	GetClientRect(&rc);
	m_wndProgress.Create(_T(""), WS_CHILD | WS_VISIBLE, rc, this, 0);
	m_wndProgress.SetRange(0, 60);
	m_wndProgress.SetRange(0, 60,1); 
	m_wndProgress.m_cf1 = RGB(64, 64, 224); 
	m_wndProgress.SetStrText(_T(""));
	m_wndProgress.m_bAbsColor = true;
	m_wndProgress.m_font2Scale = 2;


	return 0;
}


void CVibraimageExAIT::OnSize(UINT nType, int cx, int cy)
{
	CPropertiesWndBase::OnSize(nType, cx, cy);

	Align();
}


void CVibraimageExAIT::OnNewVar(int id, int subID)
{
	if(id == VI_VAR_STAT_RES_P40 || id == VI_VAR_STAT_RES_P39 || id == VI_VAR_STAT_RES_P38)
	{

		float t = theApp.m_pEngine->engine.GetVar(VI_VAR_STAT_RES_P39, 0, VT_R4).fltVal;
		float period = theApp.m_pEngine->engine.GetVar(VI_VAR_STAT_RES_P40, 0, VT_R4).fltVal;
		if (period == 0)
			period = 5;

		m_wndProgress.SetRange(0, period);
		m_wndProgress.SetRange(0, period, 1);

		if (t < period) {
			m_wndProgress.m_cf1 = RGB(64, 224, 224); 
			m_wndProgress.SetPos(period - t);
		}
		else {
			m_wndProgress.SetPos(0);
		}

	}
}



bool CVibraimageExAIT::Load(xml_node& xml)
{
	bool ret = CPropertiesWndBase::Load(xml);

	return ret;
}


void CVibraimageExAIT::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if (m_hWnd)
		Align();
}

void CVibraimageExAIT::Align()
{
	m_wndProgress.m_c0 = theApp.m_pEngine ? theApp.m_pEngine->engineInterface.GetWndColor(CLR_Window) : RGB(255, 255, 255);
	m_wndProgress.m_cf = theApp.m_pEngine ? theApp.m_pEngine->engineInterface.GetWndColor(CLR_TextValue) : 0;

	CRect rc,rp;
	GetClientRect(&rc);
	rp = rc;
		
	if (m_hWnd && m_wndProgress.m_hWnd)
		m_wndProgress.MoveWindow(&rp);
}


bool CVibraimageExAIT::FormatMenu(CCmdUI* pCmdUI, const CString& txt, BOOL en, BOOL ch) {
	return false;
}



void CVibraimageExAIT::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);
}
