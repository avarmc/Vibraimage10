
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "VibraimageExFrame.h"
#include "VibraimageEx.h"
#include <sstream>
#include "vi_colors.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace CDockablePaneVI
{

	/////////////////////////////////////////////////////////////////////////////
	// CResourceViewBar

	CPropertiesWnd::CPropertiesWnd(CPropertiesPack *pPack, int iid) : CPropertiesWndBase(pPack, iid)
	{
		m_nComboHeight = 0;
		m_nUpdate = 0;
		m_nDescription = 0;
	}

	CPropertiesWnd::~CPropertiesWnd()
	{
		RemoveAllItems();
	}


	BEGIN_MESSAGE_MAP(CPropertiesWnd, CPropertiesWndBase)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_SETFOCUS()
		ON_WM_SETTINGCHANGE()
		ON_WM_CLOSE()
		ON_WM_DESTROY()
		ON_WM_TIMER()
		ON_COMMAND(ID_COMMAND_REG_SAVE, RegSave)
	END_MESSAGE_MAP()

#ifdef _DEBUG
	std::list<CPropertiesWnd::ITEM*> CPropertiesWnd::ITEM::gItems;
#endif

	// Group constructor
	CPropertiesWnd::ITEM::ITEM(CPropertiesWnd * pb) : pBase(pb)
	{
		pItem = 0;

		Reset();
#ifdef _DEBUG
		gItems.push_back(this);
#endif}
	}



	CString CPropertiesWnd::FormatProperty(const COleVariant& v, const CString& format)
	{
		CString str;
		switch (v.vt)
		{
		case VT_R4:
			str.Format(format, v.fltVal);
			break;
		case VT_R8:
			str.Format(format, v.dblVal);
			break;
		case VT_I8:
			str.Format(format, v.llVal);
			break;
		case VT_INT:
			str.Format(format, v.intVal);
			break;
		case VT_I4:
			str.Format(format, v.lVal);
			break;
		case VT_BSTR:
			if (format.IsEmpty())
				str = v.bstrVal;
			else
				str.Format(format, v.bstrVal);
			break;
		default: return CString();
		}

		return str;
	}

	void CPropertiesWnd::ITEM::Reset()
	{
		pItem = 0;

		description.Empty();
		name.Empty();

		pos = -1;
		descriptionId = -1;
		nameId = -1;
		edit = false;

		version = 0;

		bNeedUpdate = false;
		bGroup = false;
		hEdit = 0;

		values.clear();

		progressMin = 0;
		progressMax = 0;
		progressClr = RGB(0, 224, 0);

		clrFillName = pBase->GetWndColor(CLR_FillName);
		clrFillValue = pBase->GetWndColor(CLR_FillValue);
		clrTextName = pBase->GetWndColor(CLR_TextName);
		clrTextValue = pBase->GetWndColor(CLR_TextValue);
	}

	/////////////////////////////////////////////////////////////////////////////
	// CResourceViewBar message handlers

	int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDockablePane::OnCreate(lpCreateStruct) == -1)
			return -1;

		AdjustLayoutFn();
		AdjustLayout();

		SetTimer(T_UPDATE, 40, 0);

		return 0;
	}

	void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
	{
		CDockablePane::OnSize(nType, cx, cy);

		AdjustLayout();
		if (m_pPack) m_pPack->AdjustLayout();
	}

	void CPropertiesWnd::RemoveAllItems()
	{
		for (auto& i : m_items) {
			ITEM* p = i.second;
			delete (p);
		}

		m_items.clear();
	}

	bool CPropertiesWnd::Reload()
	{
		return Load(m_xml.first_child());
	}

	bool CPropertiesWnd::Load(xml_node& xml)
	{
#ifdef _DEBUG
		if (m_xml.first_child()) {
			GetTickCount64();
		}
#endif

		__super::Load(xml);
		CInc incLoad(&m_nLoad);

		if (xml != m_xml.first_child())
		{
			if (m_xml.first_child())
				m_xml.remove_child(m_xml.first_child());
			m_xml.append_copy(xml);
		}

		m_baseId = xml.attribute_value("id");

		RemoveAllItems();

		m_nDescription = 0;

		AddItem(0, xml);

		SetPropListFont();

		return true;
	}

	void CPropertiesWnd::LoadAttributes(ITEM_VALUE& v, xml_node& xml)
	{
		v.sid = xml.attribute_wvalue("id").c_str();
		v.iid = m_pPack->Tag2Id(CString(v.sid));

#ifdef _DEBUG
		if (v.iid == VI_VAR_STAT_CFG_SIN)
		{
			int kkkid = 0;
		}
#endif

		v.sid2 = xml.attribute_wvalue("id2").c_str();
		v.iid2 = m_pPack->Tag2Id(CString(v.sid2));
		v.type = xml.attribute_value("type");
		v.typeId = GetTypeID(v.type);

		v.value = xml.attribute("value") ? xml.attribute_wvalue("description").c_str() : xml.wchild_value().c_str();
		v.format = xml.attribute_wvalue("format").c_str();
		v.scale = xml.attribute("scale") ? (float)atof(xml.attribute_value("scale")) : 1;
		v.owner = xml.attribute_value("src");
		ASSERT(v.pItem && v.pItem->pBase && v.pItem->pBase->m_pPack);
		v.ownerId = v.pItem->pBase->m_pPack->GetOwnerId(v.owner);
		if (v.format.IsEmpty())
			v.format = v.pItem->pBase->m_pPack->GetDefaultFormat(v.typeId);
	}

	void CPropertiesWnd::LoadValues(ITEM* pItem, xml_node& xml)
	{
		size_t pos = 0;
		for (xml_node_iterator i = xml.begin(), ie = xml.end(); i != ie; ++i)
		{
			if (CStringA("value") != i->name())
				continue;
			while (pItem->values.size() <= pos)
			{
				pItem->values.push_back(ITEM_VALUE());
				pItem->values.back().pItem = pItem;
			}

			LoadAttributes(pItem->values[pos], *i);

			++pos;
		}
	}

	int CPropertiesWnd::GetTypeID(const CStringA& type)
	{
		if (type == "" || type == "group" || type.Left(5) == "vlist" || type == "string")
		{
			return VI_VAR_SUB_STRING;
		}
		if (type.Left(5) == "float")
		{
			return   (type.Right(1) == "2") ? VI_VAR_SUB_F2 : VI_VAR_SUB_F1;
		}

		if (type.Left(3) == "int")
		{
			int a = 0;
			return   (type.Right(1) == "2") ? VI_VAR_SUB_I2 : VI_VAR_SUB_I1;
		}

		if (type.Left(4) == "bool")
		{
			bool a = false;
			return   (type.Right(1) == "2") ? VI_VAR_SUB_BOOL2 : VI_VAR_SUB_BOOL1;
		}
		if (type.Left(6) == "select")
		{
			bool a = false;
			return   (type.Right(1) == "2") ? VI_VAR_SUB_SELECT2 : VI_VAR_SUB_SELECT1;
		}

		return  VI_VAR_SUB_STRING;

	}
	CPropertiesWnd::ITEM * CPropertiesWnd::CreateItem(CPropertiesWnd *pb, ITEM* pGroup, const CStringA& type, xml_node& xml)
	{
		ITEM *pItem = new ITEM(pb);
		if (pGroup)
			pGroup->pChild.push_back(pItem);

		ITEM& item = *pItem;

		item.bNeedUpdate = true;
		item.npos = (int)m_items.size();
		item.nVer = 0;
		item.nList = 0;
		item.pParent = pGroup;
		item.pBase = pb;
		item.tag = xml.name();
		item.color_child = m_pPack->Tag2Id(CString(xml.attribute_wvalue("color_child").c_str()));
		item.values.resize(1);

		item.values[0].pItem = pItem;

		item.hidden = (CStringA(xml.attribute_value("hidden")).CompareNoCase("true") == 0);
		item.name = xml.attribute_wvalue("name").c_str();
		item.nameId = GetLangId(item.name);
		item.description = xml.attribute_wvalue("description").c_str();
		item.descriptionId = GetLangId(item.description);
		item.edit = (CStringA(xml.attribute_value("edit")).CompareNoCase("true") == 0);
		if (type == "bool")
			item.edit = true;

		item.sorted = (CStringA(xml.attribute_value("sorted")).CompareNoCase("true") == 0);
		item.button = xml.attribute_wvalue("button").c_str();


		if (type.Left(5) == "table")
			item.type = item.values[0].type;
		else
			item.type = type;

		LoadAttributes(item.values[0], xml);
		LoadValues(&item, xml);

		item.progressMin = xml.attribute("progress_min") ? (float)atof(xml.attribute_value("progress_min")) : 0;
		item.progressMax = xml.attribute("progress_max") ? (float)atof(xml.attribute_value("progress_max")) : 0;

		if (!item.description.IsEmpty())
			pb->m_nDescription++;


		return pItem;
	}

	CPropertiesWnd::ITEM * CPropertiesWnd::CreateItem(CPropertiesWnd *pb, ITEM* pGroup, xml_node& xml)
	{
		CStringA tag = xml.name();
		if (tag != "item")
			return 0;


		CStringA type = xml.attribute_value("type");
		ITEM* pItem = CreateItem(pb, pGroup, type, xml);
		ITEM& item = *pItem;

		bool bNeedUpdate = false;
		for (size_t i = 0; i < item.values.size(); ++i)
			if (item.values[i].iid > 0)
			{
				bNeedUpdate = true;
				break;
			}

		if (bNeedUpdate)
		{
			UpdateValue(&item, false);

			item.bNeedUpdate = true;
			InterlockedIncrement(&pb->m_nUpdate);
		}

		return pItem;
	}



	CPropertiesWnd::ITEM* CPropertiesWnd::AddItem(ITEM* pGroup, xml_node& xml)
	{
		CStringA type = xml.attribute_value("type");
		ITEM *pItem = CreateItem(this, pGroup, xml);

		if (type == "" || type == "group" || type.Left(5) == "vlist")
		{
			for (xml_node_iterator i = xml.begin(), ie = xml.end(); i != ie; ++i)
			{
				CStringA name = i->name();

				if (name == "item")
				{
					AddItem(pItem, *i);
				}
				else
					if (name == "template")
					{
						std::ostringstream oss;
						i->print(oss);
						m_xmlTmp = oss.str().c_str();
						m_xmlTmpTag = i->attribute("tag") ? i->attribute_value("tag") : "%";
					}
					else
						if (name == "push")
						{
							const char * oid = i->attribute_value("id");
							CStringA	tmp(m_xmlTmp);
							tmp.Replace(m_xmlTmpTag, oid);
							xml_document doc;
							if (doc.load(tmp))
							{
								xml_node xa = doc.first_child();
								for (xml_node_iterator a = xa.begin(), ae = xa.end(); a != ae; ++a)
									AddItem(pItem, *a);
							}
						}
			}
		}
		else
			if (type == "select")
			{
				for (xml_node_iterator i = xml.begin(), ie = xml.end(); i != ie; ++i)
				{
					if (CStringA("option") != i->name())
						continue;
					CString title = i->attribute_wvalue("title").c_str();
					CString oid = i->attribute_wvalue("id").c_str();
					CString value = i->wchild_value().c_str();
					CString st = GetLang(title);
					if (st.IsEmpty())
						st = title;

					pItem->values[0].swName.push_back(st);
					int sValue = -1;

#ifdef _DEBUG
					if (oid.GetLength() == 1)
					{
						int dummy = 0;
					}
#endif
					if (!oid.IsEmpty())
						sValue = m_pPack->Tag2Id(oid);
					if (sValue == -1 && !value.IsEmpty())
						sValue = m_pPack->Tag2Id(value);

					pItem->values[0].swValue.push_back(sValue);
					pItem->values[0].swTag.push_back(title);
				}
			}

		if (pItem /* && pItem->values[0].iid */)
			m_items.insert(std::pair< int, ITEM*>(pItem->values[0].iid, pItem));

		return pItem;
	}

	void CPropertiesWnd::InitPropList()
	{
		SetPropListFont();
	}

	void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
	{
		CDockablePane::OnSetFocus(pOldWnd);
	}

	void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCWSTR lpszSection)
	{
		CDockablePane::OnSettingChange(uFlags, lpszSection);
		SetPropListFont();
	}

	void CPropertiesWnd::SetPropListFont()
	{
		::DeleteObject(m_fntPropList.Detach());

		LOGFONT lf;
		afxGlobalData.fontRegular.GetLogFont(&lf);

		NONCLIENTMETRICS info;
		info.cbSize = sizeof(info);

		afxGlobalData.GetNonClientMetrics(info);

		lf.lfHeight = info.lfMenuFont.lfHeight;
		lf.lfWeight = info.lfMenuFont.lfWeight;
		lf.lfItalic = info.lfMenuFont.lfItalic;

		if (m_xml.first_child().attribute("font")) {
			CString a = m_xml.first_child().attribute("font").wvalue().c_str();
			if (a.Left(1) == _T("x"))
				lf.lfHeight = (int)(lf.lfHeight * _tstof(a.Mid(1)) + 0.5);
			else
			if (a.Left(1) == _T("-") || a.Left(1) == _T("+"))
				lf.lfHeight += _tstoi(a);
			else
				lf.lfHeight = _tstoi(a);
		}

		m_fntPropList.CreateFontIndirect(&lf);
	}




	void CPropertiesWnd::OnClose()
	{
		CDockablePane::OnClose();
	}


	void CPropertiesWnd::OnDestroy()
	{
		RemoveAllItems();

		CDockablePane::OnDestroy();
	}

	CPropertiesWnd::ITEM* CPropertiesWnd::GetItemByPos(int pos) {

		for (std::multimap< int, ITEM* >::iterator i = m_items.begin(); i != m_items.end(); ++i)
		{
			if (i->second->pos == pos)
				return i->second;
		}
		return 0;
	}

	CPropertiesWnd::ITEM* CPropertiesWnd::GetItem(int id, int subID)
	{
		std::pair< std::multimap< int, ITEM* >::iterator, std::multimap< int, ITEM* >::iterator> ppp;
		ppp = m_items.equal_range(id);
		for (std::multimap<int, ITEM*>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
		{
			if ((*it2).second->values[0].iid2 == subID)
				return (*it2).second;
		}
		return 0;
	}

	int CPropertiesWnd::SetUpdateAll()
	{
		if (m_nLoad)
			return 0;
		for (std::multimap<int, ITEM*>::iterator i = m_items.begin(), ie = m_items.end(); i != ie; ++i)
		{
			ITEM *pItem = i->second;
			pItem->bNeedUpdate = true;
		}
		return ++m_nUpdate;
	}

	int CPropertiesWnd::SetUpdate(int id, int subID)
	{
		if (m_nLoad)
			return 0;

		std::pair< std::multimap< int, ITEM* >::iterator, std::multimap< int, ITEM* >::iterator> ppp;
		ppp = m_items.equal_range(id);
		int cnt_ref = 0;

		for (std::multimap<int, ITEM*>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
		{
			ITEM *pItem = it2->second;
			int iid2 = pItem->values[0].iid2;
			if (subID && iid2 != subID)
				continue;
			if (!pItem->bNeedUpdate || !m_nUpdate)
			{
				if (pItem->pParent)
					pItem->pParent->bNeedUpdate = true;
				pItem->bNeedUpdate = true;
				++m_nUpdate;
			}
			pItem->nVer++;
			++cnt_ref;
		}
		if (cnt_ref)
			return cnt_ref;

		for (std::multimap<int, ITEM*>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
		{
			ITEM *pItem = it2->second;

			if (!pItem->bNeedUpdate || !m_nUpdate)
			{
				if (pItem->pParent)
					pItem->pParent->bNeedUpdate = true;
				pItem->bNeedUpdate = true;
				++m_nUpdate;
			}

			++cnt_ref;
			pItem->nVer++;
		}

		return cnt_ref;
	}

	void CPropertiesWnd::OnUpdateTimer()
	{
		if (m_nUpdate)
		{
			std::map< int, std::map< int, ITEM* > >::iterator i1, i1e;
			std::map< int, ITEM* >::iterator i, ie;

			for (std::multimap<int, ITEM*>::iterator i = m_items.begin(), ie = m_items.end(); i != ie; ++i)
			{
				if (i->second->bNeedUpdate)
				{
					CMFCPropertyGridProperty * pp = (CMFCPropertyGridProperty *)(i->second->pItem);

					if (pp && !pp->IsInPlaceEditing())
					{
						if (UpdateValue(i->second, false))
							i->second->bNeedUpdate = false;
					}
				}
			}
			m_nUpdate = 0;
		}
	}

	void CPropertiesWnd::OnTimer(UINT_PTR nIDEvent)
	{
		if (m_nLoad)
			return;
		switch (nIDEvent)
		{
		case T_UPDATE:
			OnUpdateTimer();
			return;
		case T_LAYOUT:
			if (theApp.m_bInit)
			{
				KillTimer(nIDEvent);
				AdjustLayoutFn();
			}
			return;
		default: break;
		}

		CPropertiesWndBase::OnTimer(nIDEvent);
	}

	void CPropertiesWnd::RegSave()
	{
	}

	void  CPropertiesWnd::RegSave(ITEM* pItem)
	{
	}

	void CPropertiesWnd::OnAfterDock(CBasePane* pBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod)
	{
		CDockablePane::OnAfterDock(pBar, lpRect, dockMethod);
		if (m_hWnd)
		{
			TCHAR cln[256] = { 0 };
			int nn = GetClassName(m_hWnd, cln, 256);
			cln[0] = 0;
		}
	}



	CTabbedPane* CPropertiesWnd::CreateTabbedPane()
	{
		ASSERT_VALID(this);
		CRect rectTabBar;
		GetWindowRect(&rectTabBar);
		ASSERT_VALID(GetParent());
		GetParent()->ScreenToClient(&rectTabBar);

		CTabbedPane* pTabbedBar = (CTabbedPane*)m_pTabbedControlBarRTC->CreateObject();
		ASSERT_VALID(pTabbedBar);
		pTabbedBar->m_bTabsAlwaysTop = TRUE;

		pTabbedBar->SetAutoDestroy(TRUE);

		if (!pTabbedBar->Create(_T(""), GetParent(), rectTabBar, TRUE, (UINT)-1, GetStyle() | CBRS_FLOAT_MULTI))
		{
			TRACE0("Failed to create tabbed control bar\n");
			return NULL;
		}

		pTabbedBar->m_recentDockInfo.m_recentMiniFrameInfo.m_rectDockedRect = m_recentDockInfo.m_recentMiniFrameInfo.m_rectDockedRect;
		pTabbedBar->m_recentDockInfo.m_recentSliderInfo.m_rectDockedRect = m_recentDockInfo.m_recentSliderInfo.m_rectDockedRect;
		pTabbedBar->m_recentDockInfo.m_rectRecentFloatingRect = m_recentDockInfo.m_rectRecentFloatingRect;

		return pTabbedBar;
	}



}