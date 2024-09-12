// VibraimageExGraph.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExHist.h"
#include <sstream>

// CVibraimageExHist

IMPLEMENT_DYNAMIC(CVibraimageExHist, CPropertiesWndBase)

CVibraimageExHist::CVibraimageExHist(CPropertiesPack *pPack, int iid) : CPropertiesWndBase(pPack,iid)
{
	theApp.m_pHist = this;
}

CVibraimageExHist::~CVibraimageExHist()
{
	theApp.m_pHist = 0;
}


BEGIN_MESSAGE_MAP(CVibraimageExHist, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
END_MESSAGE_MAP()



// CVibraimageExHist message handlers


int CVibraimageExHist::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertiesWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	Align();

	return 0;
}

void CVibraimageExHist::OnSize(UINT nType, int cx, int cy)
{
	CPropertiesWndBase::OnSize(nType, cx, cy);
	if (m_hWnd)
		//		SetTimer(1, 10, 0);
		Align();
}

bool CVibraimageExHist::Align()
{
	if (!m_hWnd || !theApp.m_pEngine->engine.GetHist())
		return false;
	CRect rc;
	GetClientRect(&rc);
	
	for (int core = 0, coreMax = (int)theApp.m_pEngines.size(); core < coreMax; ++core) {
		HWND hWndHist = theApp.m_pEngines[core]->engine.GetHist()->GetWindow();


		if (!hWndHist)
			continue;

		if (theApp.m_pEngines[core]->id == theApp.m_pEngine->id) {
		
			if (::GetParent(hWndHist) != m_hWnd)
				::SetParent(hWndHist, m_hWnd);
			::ShowWindow(hWndHist, SW_SHOWNOACTIVATE);
			::MoveWindow(hWndHist, rc.left, rc.top, rc.Width(), rc.Height(), TRUE);
		} else
			::ShowWindow(hWndHist, SW_HIDE);
	}
	return true;
}

bool CVibraimageExHist::Load(xml_node& xml)
{
	__super::Load(xml);
	if (m_xml.first_child())
		m_xml.remove_child(m_xml.first_child());
	m_xml.append_copy(xml);

	return LoadNode(m_xml.first_child());
}

bool CVibraimageExHist::LoadNode(xml_node& xml)
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
	return false;
}

bool CVibraimageExHist::AddItem(xml_node& xml)
{
	CStringA sid = xml.attribute_value("id");
	CStringA sidg = xml.attribute_value("group");
	if (sid.IsEmpty())
		return false;
	CStringA sw = xml.attribute_value("width");
	CStringA sDT = xml.attribute_value("dt");
	int iid = theApp.m_pEngine->engine.Tag2Id(CString(sid));
	int iidg = theApp.m_pEngine->engine.Tag2Id(CString(sidg));

	float dt = 0;
	int w = 1;
	if (!sw.IsEmpty())
		w = atoi(sw);
	if (!sDT.IsEmpty())
		dt = atof(sDT);
	theApp.m_pEngine->engine.GetHist()->AddHist(iidg, iid,w,(int)(dt*1000+0.5f));

	return true;
}




void CVibraimageExHist::OnMove(int x, int y)
{
	CPropertiesWndBase::OnMove(x, y);
	if (m_hWnd)
		Align();
}
