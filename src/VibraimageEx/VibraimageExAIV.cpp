// VibraimageExQT.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExAIV.h"
#include "PropertiesPack.h"
#include "VibraimageExFrame.h"

// CVibraimageExAIV

IMPLEMENT_DYNAMIC(CVibraimageExAIV, CPropertiesWndBase)

CVibraimageExAIV::CVibraimageExAIV(CPropertiesPack *pPack, int iid) 
	: CPropertiesWndBase(pPack, iid)
//	, m_wndProgress(2)
{
	m_nUpdate = m_nAlign = m_nShow =  0;
	if (!theApp.m_pAIV)
		theApp.m_pAIV = this;
}

CVibraimageExAIV::~CVibraimageExAIV()
{
	if (theApp.m_pAIV == this)
		theApp.m_pAIV = 0;
}


BEGIN_MESSAGE_MAP(CVibraimageExAIV, CPropertiesWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
//	ON_WM_TIMER()
ON_WM_PAINT()
END_MESSAGE_MAP()



// CVibraimageExAIV message handlers




int CVibraimageExAIV::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertiesWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(1, 50, 0);

	m_nAlign++;
	return 0;
}


void CVibraimageExAIV::OnSize(UINT nType, int cx, int cy)
{
	CPropertiesWndBase::OnSize(nType, cx, cy);
	m_nAlign++;
}

void  CVibraimageExAIV::Update() {
	std::unique_lock<std::recursive_mutex> lock(m_wndLock);

	float t = theApp.m_pEngine->engine.GetVar(VI_VAR_STAT_RES_P39, 0, VT_R4).fltVal;
	size_t cnt = GetWndCount();
	if (cnt != m_wndProgress.size()) {
		m_nAlign++;
		return;
	}

	int mode = (int)(theApp.m_pEngine->engine.GetVar(VI_INFO_AI_MODE, -1, VT_INT).intVal);

	int id37 = VI_INFO_AI_P37;
	int id38 = VI_INFO_AI_P38;

	if (mode & AI_MODE_SINGLE) {
		id37 = VI_VAR_STAT_RES_P37;
		id38 = VI_VAR_STAT_RES_P38;
	}

	float rangeMin = theApp.m_pEngine->engine.GetVar(VI_INFO_AI_CFG_RANGE, 0, VT_R4).fltVal;
	float rangeMax = theApp.m_pEngine->engine.GetVar(VI_INFO_AI_CFG_RANGE, 1, VT_R4).fltVal;
	float rangeX = rangeMax - rangeMin;


	for (auto& wnd : m_wndProgress) {
		float v = theApp.m_pEngine->engine.GetVar(id37, wnd.first, VT_R4).fltVal;
		if (v == -1000)
			break;
		if (!wnd.second)
			continue;
		wnd.second->SetRange(rangeMin, rangeMax);
		wnd.second->SetRange(rangeMin, rangeMax, 1);

		wnd.second->m_font2ScaleSet = 1.7;
		float v2 = theApp.m_pEngine->engine.GetVar(id38, wnd.first, VT_R4).fltVal;

		if (v2 >= 0)
			v = v2;

		CString name = theApp.m_pEngine->engine.GetVar(VI_INFO_AI_CFG_NAME, wnd.first, VT_BSTR).bstrVal;
		if (name.IsEmpty()) name = L"AI";
		float th = theApp.m_pEngine->engine.GetVar(VI_VAR_STAT_RES_P42, 0, VT_R4).fltVal;


		if (v >= 0 && t > 0) {
			if (v >= th) {
				wnd.second->m_c1 = RGB(224, 64, 64);
				wnd.second->m_c0 = RGB(255, 255, 255);
			}
			else
			{
				wnd.second->m_c1 = RGB(255, 255, 255);
				wnd.second->m_c0 = RGB(64, 255, 64);
			}
			wnd.second->m_vFmt = name + _T("=%0.2f");
			wnd.second->SetPos(v * rangeX + rangeMin, 0, true);
		}
		else {
			wnd.second->m_c1 = RGB(224, 64, 64);
			wnd.second->m_c0 = RGB(255, 255, 224);
			wnd.second->m_vFmt = name + _T("=?");
			wnd.second->SetPos(0);
		}
	}
	m_nUpdate = 0;
}
void CVibraimageExAIV::OnNewVar(int id, int subID)
{
	if (id == VI_INFO_AI_LIVE) {
		++m_nShow;
	} else
	if(id == VI_INFO_AI_P38 || id == VI_INFO_AI_P37 || id == VI_VAR_STAT_RES_P37 || id == VI_VAR_STAT_RES_P38)
	{
		++m_nUpdate;
	}
}



bool CVibraimageExAIV::Load(xml_node& xml)
{
	bool ret = CPropertiesWndBase::Load(xml);

	return ret;
}


void CVibraimageExAIV::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if (m_hWnd)
		Align();
}

int CVibraimageExAIV::GetWndCount() {
	
	int mode = (int)(theApp.m_pEngine->engine.GetVar(VI_INFO_AI_MODE, -1, VT_INT).intVal);
	if (mode & AI_MODE_SINGLE)
		return 1;
	int cnt = (int)(theApp.m_pEngine->engine.GetVar(VI_INFO_AI_P37, -1, VT_R4).fltVal+0.5f);
	if (!cnt)
		return 1;
	return cnt;
}

int CVibraimageExAIV::CheckAndCreate() {
	std::unique_lock<std::recursive_mutex> lock(m_wndLock);

	int cnt = GetWndCount();
	while (m_wndProgress.size() > (size_t)cnt)
		m_wndProgress.erase(m_wndProgress.rbegin()->first);

	CRect rc;
	GetClientRect(&rc);

	for (int i = 0; i < cnt; ++i) {
		auto& wnd = m_wndProgress[i];
		if (!wnd)
			wnd = std::make_shared< CColorProcress >(2);

		if (!wnd->m_hWnd) {
			wnd->Create(_T(""), WS_CHILD | WS_VISIBLE, rc, this, 0);
			wnd->SetRange(0, 100);
			wnd->SetRange(0, 100, 1);
			wnd->m_cf1 = RGB(225, 64, 0);
			wnd->SetStrText(_T(""));
			wnd->m_bAbsColor = true;
			wnd->m_font2Scale = 2;
			wnd->m_vFmt = _T("AI=%0.2f");
		}
	}
	return cnt;
}

void CVibraimageExAIV::Align()
{
	if (!m_hWnd)
		return;

	CRect rc;
	GetClientRect(&rc);

	int cnt = CheckAndCreate();

	CPoint rp = rc.TopLeft();
	CSize rs = rc.Size(),rd(0,0);
	if (cnt > 1 && rc.Width() > rc.Height()) {
		rs.cy /= cnt;
		rd.cy = rs.cy;
	}
	else {
		rs.cx /= cnt;
		rd.cx = rs.cx;
	}

	std::unique_lock<std::recursive_mutex> lock(m_wndLock);

	for (auto& wnd : m_wndProgress) {
		CRect rw = CRect(rp, rs);
		if (cnt > 1)
			rw.bottom -= 2;
		if (wnd.second && wnd.second->m_hWnd)
			wnd.second->MoveWindow(&rw);

		rp.x += rd.cx;
		rp.y += rd.cy;
	}
	m_nAlign = 0;

}


bool CVibraimageExAIV::FormatMenu(CCmdUI* pCmdUI, const CString& txt, BOOL en, BOOL ch) {
	return false;
}


void CVibraimageExAIV::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		if (m_nAlign)
			Align();
		if (m_nUpdate)
			Update();

		if (m_nShow) {
#ifndef _DEBUG
			if (m_hWnd && theApp.m_pFrame) {
				if (theApp.m_pEngine->engine.GetVar(VI_INFO_AI_LIVE, 0, VT_INT).intVal)
					ShowPane(TRUE, TRUE, FALSE);
				else
					ShowPane(FALSE, TRUE, FALSE);

				//	theApp.m_pFrame->RecalcLayout();
			}
#endif
			m_nShow = 0;
		}
	}
	__super::OnTimer(nIDEvent);
}


void CVibraimageExAIV::OnPaint()
{
	CPaintDC dc(this); 
	CRect rc;
	GetClientRect(&rc);
	dc.FillSolidRect(&rc, 0xFFFFFF);
}
