// VibraimageExGraph.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExGraph.h"
#include "VibraimageExFrame.h"
#include "VibraimageExView.h"
#include <sstream>

// CVibraimageExGraph

IMPLEMENT_DYNAMIC(CVibraimageExGraph, CPropertiesWndBase)

CVibraimageExGraph::CVibraimageExGraph(CPropertiesPack *pPack,int iid) : CPropertiesWndBase(pPack,iid)
{
	theApp.m_pGraph = this;
	m_wndGraph = 0;
	m_bTmpHideGraphs = false;
}

CVibraimageExGraph::~CVibraimageExGraph()
{
	theApp.m_pGraph = 0;
}


BEGIN_MESSAGE_MAP(CVibraimageExGraph, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_WM_MOVE()

END_MESSAGE_MAP()



// CVibraimageExGraph message handlers




int CVibraimageExGraph::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertiesWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(2, 100, 0);

	return 0;
}


void CVibraimageExGraph::OnSize(UINT nType, int cx, int cy)
{
	CPropertiesWndBase::OnSize(nType, cx, cy);
	if (m_hWnd)
		Align();
}


void CVibraimageExGraph::OnPaint()
{
	CPaintDC dc(this);
	if (!m_wndGraph)
	{
		CRect rc;
		GetClientRect(&rc);
		dc.FillSolidRect(&rc, 0xFFFFFF);
	}
}


void CVibraimageExGraph::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 2:
		CheckTmpHide();
		return;
	default: break;
	}
	__super::OnTimer(nIDEvent);
}

void CVibraimageExGraph::CheckTmpHide()
{
	bool bImgRepl = theApp.m_pEngine->engine.GetVar(VI_INFO_IMG_REPL, 0, VT_BOOL, 1).boolVal ? true:false;
	if (bImgRepl)
	{
		if (!m_bTmpHideGraphs && IsWindowVisible())
		{
			m_bTmpHideGraphs = true;
			ShowPane(FALSE, TRUE, FALSE);
			theApp.m_pFrame->AdjustDockingLayout();
			theApp.m_pFrame->RecalcLayout();
		}
	}
	else
	if (m_bTmpHideGraphs)
	{
		m_bTmpHideGraphs = false;
		ShowPane( TRUE, TRUE, FALSE);
		theApp.m_pFrame->AdjustDockingLayout();
		theApp.m_pFrame->RecalcLayout();
	}
}

bool CVibraimageExGraph::Align()
{
	BOOL bVisible = IsWindowVisible();

	for (int core = 0, coreMax = (int)theApp.m_pEngines.size(); core < coreMax; ++core) {
		CViEngineExGraphInterface* pg = theApp.m_pEngines[core]->engine.GetGraph();
		if (!pg)
			continue;
		HWND wndGraph = pg->GetWindow();
		if (!wndGraph)
			continue;
		if (theApp.m_pEngines[core]->id == theApp.m_pEngine->id && bVisible) {
			m_wndGraph = wndGraph;
			CRect rc;
			GetClientRect(&rc);
			if (::GetParent(m_wndGraph) != m_hWnd)
				::SetParent(m_wndGraph, m_hWnd);
			::MoveWindow(m_wndGraph, rc.left, rc.top, rc.Width(), rc.Height(), TRUE);
			if (!::IsWindowVisible(m_wndGraph))
				::ShowWindow(m_wndGraph, SW_SHOW);
		} else
			::ShowWindow(wndGraph, SW_HIDE);
	}
	return true;
}
bool CVibraimageExGraph::Load(xml_node& xml)
{
	__super::Load(xml);
	CInc incLoad(&m_nLoad);

	if (m_xml.first_child())
		m_xml.remove_child(m_xml.first_child());
	m_xml.append_copy(xml);

	return LoadNode(m_xml.first_child());
}

bool CVibraimageExGraph::LoadNode(xml_node& xml)
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

bool CVibraimageExGraph::AddItem(xml_node& xml)
{
	CViEngineExGraphInterface* pg = theApp.m_pEngine->engine.GetGraph();
	if (CString("item") != xml.name() || !pg)
		return false;
	CString title = xml.attribute_wvalue("title").c_str();
	CStringA sid = xml.attribute_value("id");


	CStringA sEn = xml.attribute_value("enabled");
	bool bEn = (sEn == "true" || sEn == "1");

	CStringA sAuto = xml.attribute_value("auto");
	bool bAuto = (sAuto == "true" || sAuto == "1");

	CStringA sMin = xml.attribute_value("min");
	float vMin = (float)atof(sMin);

	CStringA sMax = xml.attribute_value("max");
	float vMax = (float)atof(sMax);

	int order = pg->GetCount();
	int iid = theApp.m_pEngine->engine.Tag2Id(CString(sid));
	if (!iid)
		return false;
	CString name = xml.attribute_wvalue("name").c_str();
	int nameId = GetLangId(name);

	pg->AddGraph(iid, order);
	pg->SetMinMax(iid, vMin, vMax, bAuto);
	pg->SetName(iid,GetLang(nameId, name));
	pg->SetTag(iid, CString(sid));

	pg->Enable(iid, bEn);

	pg->RegLoad(iid);
	return true;
}



BOOL CVibraimageExGraph::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CViEngineExGraphInterface* pg = theApp.m_pEngine->engine.GetGraph();
	if (pg)
		return pg->OnMouseWheel(nFlags, zDelta, pt);

	return CPropertiesWndBase::OnMouseWheel(nFlags, zDelta, pt);
}


void CVibraimageExGraph::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CPropertiesWndBase::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here
}




void CVibraimageExGraph::OnDestroy()
{
	CPropertiesWndBase::OnDestroy();
}


void CVibraimageExGraph::OnMove(int x, int y)
{
	CPropertiesWndBase::OnMove(x, y);
	if (m_hWnd)
		Align();
}

BOOL  CVibraimageExGraph::OnBeforeShowPaneMenu(CMenu& menu)
{
//	menu.AppendMenu(MF_STRING, (UINT)-202, _T("simple"));
//	theApp.m_pFrame->m_pContextMenuHolder = this;
	return true;
}

BOOL CVibraimageExGraph::OnAfterShowPaneMenu(int nMenuResult)
{
	if (nMenuResult != (UINT)-202)
		return TRUE;

	if (m_bSimple)
	{
		theApp.m_pView->RemoveTab(this);
		theApp.m_pFrame->ShowPane(this, TRUE, TRUE, FALSE);
		
		m_bSimple = false;

	}
	else
	{
		CString title;
		GetWindowText(title);
		
		theApp.m_pFrame->ShowPane(this, FALSE, TRUE, FALSE);
		theApp.m_pView->AddTab(this, title, -1, FALSE);
		m_bSimple = true;
	}
	return TRUE;
}

void CVibraimageExGraph::ShowPane(BOOL bShow, BOOL bDelay, BOOL bActivate) {
	__super::ShowPane(bShow, bDelay, bActivate);

	Align();
}




