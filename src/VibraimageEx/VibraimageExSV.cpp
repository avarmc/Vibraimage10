// VibraimageExQT.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExSV.h"


// CVibraimageExSV

IMPLEMENT_DYNAMIC(CVibraimageExSV, CPropertiesWndBase)

CVibraimageExSV::CVibraimageExSV(CPropertiesPack *pPack, int iid) :CPropertiesWndBase(pPack, iid)
{
	theApp.m_pSV = this;
}

CVibraimageExSV::~CVibraimageExSV()
{
}


BEGIN_MESSAGE_MAP(CVibraimageExSV, CPropertiesWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
END_MESSAGE_MAP()



// CVibraimageExSV message handlers




int CVibraimageExSV::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertiesWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	GetClientRect(&rc);
	m_wndProgress.Create(_T(""), WS_CHILD | WS_VISIBLE, rc, this, 0);
	m_wndProgress.SetRange(0, 100);
	m_wndProgress.m_vFmt = _T("%0.0f %%");
	m_wndProgress.m_c1 = RGB(224, 0, 0);
	m_wndProgress.SetStrText(_T(""));
	m_wndProgress.m_bAbsColor = true;
	m_wndProgress.m_font2Scale = 2;

	return 0;
}


void CVibraimageExSV::OnSize(UINT nType, int cx, int cy)
{
	CPropertiesWndBase::OnSize(nType, cx, cy);

	Align();
}


void CVibraimageExSV::OnNewVar(int id, int subID)
{
	if (id == VI_VAR_STATE_VAR)
	{

		float v = theApp.m_pEngine->engine.GetVar(VI_VAR_STATE_VAR, 0, VT_R4).fltVal*100;
		float l = theApp.m_pEngine->engine.GetVar(VI_VAR_STATE_CRITICAL, 0, VT_R4).fltVal*100;

		if(v >= l)
			m_wndProgress.m_c1 = RGB(224, 0, 0);
		else
		if (v >= l/2)
			m_wndProgress.m_c1 = RGB(224, 224, 0);
		else
			m_wndProgress.m_c1 = RGB(0, 224, 0);

		TCHAR strText[32];
		_stprintf_s(strText, 32, _T("%i%%"), (int)v);
		m_wndProgress.SetStrText(strText);
		m_wndProgress.SetPos(v);
	}
}

bool CVibraimageExSV::Load(xml_node& xml)
{
	return CPropertiesWndBase::Load(xml);
}


void CVibraimageExSV::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if (m_hWnd)
		Align();
}

void CVibraimageExSV::Align()
{
	CRect rc;
	GetClientRect(&rc);
	m_wndProgress.m_c0 = theApp.m_pEngine ? theApp.m_pEngine->engineInterface.GetWndColor(CLR_Window) : RGB(255, 255, 255);
	m_wndProgress.m_cf = theApp.m_pEngine ? theApp.m_pEngine->engineInterface.GetWndColor(CLR_TextValue) : 0;

	if (m_hWnd && m_wndProgress.m_hWnd)
		m_wndProgress.MoveWindow(&rc);
}
