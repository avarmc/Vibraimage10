// VibraimageExHistStat.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExHistStat.h"
#include <sstream>

// CVibraimageExHistStat

IMPLEMENT_DYNAMIC(CVibraimageExHistStat, CPropertiesWndBase)

CVibraimageExHistStat::CVibraimageExHistStat(CPropertiesPack *pPack,int iid) : CPropertiesWndBase(pPack,iid)
{
}

CVibraimageExHistStat::~CVibraimageExHistStat()
{
}


BEGIN_MESSAGE_MAP(CVibraimageExHistStat, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_WM_MOVE()
END_MESSAGE_MAP()



// CVibraimageExHistStat message handlers




int CVibraimageExHistStat::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertiesWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;



	return 0;
}


void CVibraimageExHistStat::OnSize(UINT nType, int cx, int cy)
{
	CPropertiesWndBase::OnSize(nType, cx, cy);
	if (m_hWnd)
		Align();
}


void CVibraimageExHistStat::OnPaint()
{
#if  !defined SEQ_MEDIC_MOL
	CPaintDC dc(this);
	HWND wndHist = theApp.m_pEngine->engine.GetStat()->GetWindow(m_sid);

	if (!wndHist)
	{
		CRect rc;
		GetClientRect(&rc);
		dc.FillSolidRect(&rc, 0x7FFFFF);
	}
#endif
}

bool CVibraimageExHistStat::Align()
{
#if  !defined SEQ_MEDIC_MOL
	for (int core = 0, coreMax = (int)theApp.m_pEngines.size(); core < coreMax; ++core) {
		if (m_sid.IsEmpty())
			continue;
		int nh = m_sid  == L"H" ? -1 : -2;
		HWND wndHist = theApp.m_pEngines[core]->engine.GetStat(nh)->GetWindow(m_sid);

		if (!wndHist)
			continue;
		if (theApp.m_pEngines[core]->id == theApp.m_pEngine->id) {
			CRect rc;
			GetClientRect(&rc);
			::SetParent(wndHist, m_hWnd);
			::MoveWindow(wndHist, rc.left, rc.top, rc.Width(), rc.Height(), TRUE);
			::ShowWindow(wndHist, SW_SHOW);
		} else
			::ShowWindow(wndHist, SW_HIDE);
	}
	return true;
#else
	return false;
#endif
}

bool CVibraimageExHistStat::Load(xml_node& xml)
{
	__super::Load(xml);
	CInc incLoad(&m_nLoad);

	return LoadNode(xml);
}

bool CVibraimageExHistStat::LoadNode(xml_node& xml)
{
	m_sid = CString( xml.attribute_value("tag"));
	Align();
	return true;
}

BOOL CVibraimageExHistStat::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CPropertiesWndBase::OnMouseWheel(nFlags, zDelta, pt);
}


void CVibraimageExHistStat::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CPropertiesWndBase::OnActivate(nState, pWndOther, bMinimized);
}


void CVibraimageExHistStat::OnDestroy()
{
	CPropertiesWndBase::OnDestroy();
}


void CVibraimageExHistStat::OnMove(int x, int y)
{
	CPropertiesWndBase::OnMove(x, y);
	if (m_hWnd)
		Align();
}
