// VibraimageExLD.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExLD.h"
#include <sstream>

// CVibraimageExLD

IMPLEMENT_DYNAMIC(CVibraimageExLD, CPropertiesWndBase)

CVibraimageExLD::CVibraimageExLD(CPropertiesPack *pPack,int iid) : CPropertiesWndBase(pPack,iid)
{
	theApp.m_pLD = this;
	m_wndLD = 0;
}

CVibraimageExLD::~CVibraimageExLD()
{
	theApp.m_pGraph = 0;
}


BEGIN_MESSAGE_MAP(CVibraimageExLD, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_WM_MOVE()
END_MESSAGE_MAP()



// CVibraimageExLD message handlers




int CVibraimageExLD::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertiesWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;



	return 0;
}


void CVibraimageExLD::OnSize(UINT nType, int cx, int cy)
{
	CPropertiesWndBase::OnSize(nType, cx, cy);
	if (m_hWnd)
		Align();
}


void CVibraimageExLD::OnPaint()
{
	CPaintDC dc(this);
	if (!m_wndLD)
	{
		CRect rc;
		GetClientRect(&rc);
		dc.FillSolidRect(&rc, 0xFFFFFF);
	}
}


void CVibraimageExLD::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		if (Align())
			KillTimer(nIDEvent);
		return;
	default: break;
	}
	__super::OnTimer(nIDEvent);
}

bool CVibraimageExLD::Align()
{
#if  !defined SEQ_MEDIC_MOL
	for (int core = 0, coreMax = (int)theApp.m_pEngines.size(); core < coreMax; ++core) {
		CViEngineExLDWInterface* pl = theApp.m_pEngines[core]->engine.GetLDW();
		if (!pl)
			continue;
		HWND wndLD = pl->GetWindow();
		if (!IsWindow(wndLD))
			continue;

		if (theApp.m_pEngines[core]->id == theApp.m_pEngine->id) {
			m_wndLD = wndLD;
			CRect rc;
			GetClientRect(&rc);
			if (::GetParent(m_wndLD) != m_hWnd)
				::SetParent(m_wndLD, m_hWnd);
			::MoveWindow(m_wndLD, rc.left, rc.top, rc.Width(), rc.Height(), TRUE);
			if (!::IsWindowVisible(m_wndLD))
				::ShowWindow(m_wndLD, SW_SHOW);
		} else
			::ShowWindow(wndLD, SW_HIDE);
	}
	return true;
#else
	return false;
#endif
}

bool CVibraimageExLD::Load(xml_node& xml)
{
	__super::Load(xml);
	CInc incLoad(&m_nLoad);

	if (m_xml.first_child())
		m_xml.remove_child(m_xml.first_child());
	m_xml.append_copy(xml);

	return LoadNode(m_xml.first_child());
}

bool CVibraimageExLD::LoadNode(xml_node& xml)
{
	xml_node_iterator i, ie;
	for (i = xml.begin(), ie = xml.end(); i != ie; ++i)
	{
		CStringA name = i->name();

		if (name == "item")
			AddItem(*i);
		else
		if (name == "template")
		{
			std::ostringstream oss;
			i->print(oss);
			m_xmlTmp = oss.str().c_str();
		}
		else
		if (name == "push")
		{
			const char * oid = i->attribute_value("id");
			CStringA opush = i->attribute_value("push");

			CStringA	tmp(m_xmlTmp);
			tmp.Replace("%", oid);
			tmp.Replace("push=\"*\"", opush);
			xml_document doc;
			if (doc.load(tmp))
			{
				xml_node xref = doc.first_child();

				xml_attribute_iterator ai;
				for (ai = i->attributes_begin(); ai != i->attributes_end(); ++ai)
				{
					CStringA aName = ai->name();
					if (aName == "id")
						continue;
					CStringA aVar = ai->value();
					xml_node_iterator xi, xie;

					for (xi = xref.begin(), xie = xref.end(); xi != xie; ++xi)
					{
						xml_attribute a = xi->attribute(aName);
						if( !a )
							a = xi->append_attribute(aName);
						a.set_value(aVar);
					}
				}
				LoadNode(xref);
			}
		}

	}


	return true;
}

bool CVibraimageExLD::AddItem(xml_node& xml)
{

	return true;
}



BOOL CVibraimageExLD::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

	return CPropertiesWndBase::OnMouseWheel(nFlags, zDelta, pt);
}


void CVibraimageExLD::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CPropertiesWndBase::OnActivate(nState, pWndOther, bMinimized);
}




void CVibraimageExLD::OnDestroy()
{
	CPropertiesWndBase::OnDestroy();
}


void CVibraimageExLD::OnMove(int x, int y)
{
	CPropertiesWndBase::OnMove(x, y);
	if (m_hWnd)
		Align();
}
