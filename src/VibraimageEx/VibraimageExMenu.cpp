// VibraimageExMenu.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExMenu.h"
#include "VibraimageExFrame.h"

// CVibraimageExMenu

IMPLEMENT_DYNAMIC(CVibraimageExMenu, CMFCMenuBar)

CVibraimageExMenu::CVibraimageExMenu()
{
	m_itemFirst = 60000;
	m_tShow = GetTickCount64()-10000;
}

CVibraimageExMenu::~CVibraimageExMenu()
{
}


BEGIN_MESSAGE_MAP(CVibraimageExMenu, CMFCMenuBar)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CVibraimageExMenu message handlers




BOOL CVibraimageExMenu::Create(CWnd* pParentWnd, DWORD dwStyle , UINT nID )
{
	BOOL ok = CMFCMenuBar::Create( pParentWnd, dwStyle, nID);
	SetShowAllCommands();
	return ok;
}

void CVibraimageExMenu::CreateFromMenuStd(HMENU hMenu, BOOL bDefaultMenu, BOOL bForceUpdate)
{
	CMFCMenuBar::CreateFromMenu(hMenu, bDefaultMenu, bForceUpdate);
}

void CVibraimageExMenu::CreateFromMenu(HMENU hMenu, BOOL bDefaultMenu, BOOL bForceUpdate )
{
	auto cfg = theApp.m_pEngine->engine.GetConfigBase();
	if (!cfg)
		return;

	CMenu menu;
	menu.CreateMenu();

	CString path = GetDefaultPath() + cfg + _T("\\menu.xml");

	Load(menu, path);

	CreateFromMenuStd(menu.Detach(), bDefaultMenu, bForceUpdate);

	CMFCToolBar::SetBasicCommands(m_ids);
}

bool CVibraimageExMenu::Load(CMenu& menu,const CString& path)
{
	m_ids.RemoveAll();
	m_items.clear();

	CString sPath(path);

	xml_document doc;
	if (!doc.load_file(sPath))
		return false;
	return Load(menu,doc.first_child(),0);
}


bool CVibraimageExMenu::Load(CMenu& menu,xml_node& xml,int depth)
{
	Purge(menu);
	if (!depth) {
		CStringA bHide = xml.attribute_value("hide");

		if (bHide == "true") {
			theApp.m_pFrame->SetMenuBarVisibility(AFX_MBV_DISPLAYONFOCUS | AFX_MBV_DISPLAYONF10);
			SetTimer(1, 40, 0);
			Show(false);
		}

		m_flags = xml.attribute_value("flags");
	}

	int pos = 0,err = 0;
	xml_node_iterator i, ie;
	for (i = xml.begin(), ie = xml.end(); i != ie; ++i, ++pos)
	{
		ITEM itm;
		
		CStringA tag = i->name();
		CStringA sid = i->attribute_value("id");
		itm.iid = theApp.m_pEngine->engine.Tag2Id( CString(sid) );
		itm.lid = theApp.m_pEngine->engine.GetLang()->Find(i->attribute_wvalue("name").c_str());
		CString name = theApp.m_pEngine->engine.GetLang()->Get(itm.lid, i->attribute_wvalue("name").c_str());
		itm.cid = theApp.m_pEngine->engine.GetLang()->Find(i->attribute_wvalue("cname").c_str());


		int nid = itm.iid;

		if (itm.iid > 0 && m_items.find(itm.iid) != m_items.end())
			nid = (int)(m_items.size() + m_itemFirst);


		if (tag == "popup")
		{
			CMenu m;
			UINT style = MF_POPUP;

			m.CreatePopupMenu();
			Load(m, *i, depth + 1);
			HMENU hm = m.Detach();

			menu.AppendMenu(style, (UINT_PTR)hm, name);
			
			if (CheckIID(nid))
				m_ids.AddTail(nid);
		}
		else
		if (tag == "item")
		{
			
			if(!AppendMenuMulti(menu,*i,depth,pos))
				menu.AppendMenu(MF_STRING, nid, name);
			
			if (CheckIID(nid))
				m_ids.AddTail(nid);
		}
		else
		if (tag == "separator")
		{
			menu.AppendMenu(MF_SEPARATOR, 0, name);
		}

		if (itm.lid)
		{
			MENUITEMINFO mii;
			ZeroMemory(&mii, sizeof(mii));
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_DATA;
			mii.dwItemData = itm.lid;

			menu.SetMenuItemInfo(pos, &mii, TRUE);
		}

		if(nid > 0)
			m_items[nid] = itm;

	}

	return false;
}

void CVibraimageExMenu::Purge(CMenu& menu)
{
	while (menu.RemoveMenu(0, MF_BYPOSITION))
		;
}

BOOL CVibraimageExMenu::LoadState(LPCWSTR lpszProfileName, int nIndex, UINT uiID)
{
	return FALSE;
}


void CVibraimageExMenu::NewMenuBar()
{
	CreateFromMenu(0,TRUE);
	theApp.m_pFrame->UpdateTitle();
}


bool CVibraimageExMenu::CheckIID(int iid)
{
	if(!iid)
		return false;
	if (iid >= ID_VIDEOSETTINGS0_0 && iid <= ID_VIDEOSETTINGS0_15)
		return false;
	if (iid >= ID_VIDEOSETTINGS0_DEVICE0 && iid <= ID_VIDEOSETTINGS0_DEVICE99)
		return false;
	return true;
}


bool CVibraimageExMenu::AppendMenuMulti(CMenu& menu, xml_node& xml, int depth,int & pos)
{
	CStringA tag = xml.name();
	CStringA sid = xml.attribute_value("id");

	if (sid == "ID_VIDEOSETTINGS0*")
	{
		for (int id = ID_VIDEOSETTINGS0_0, n = 0; id <= ID_VIDEOSETTINGS0_15; ++id, ++n)
		{
			BOOL bChecked = FALSE, bEnabled = FALSE;
			TCHAR strName[256] = { 0 };

			theApp.m_pEngine->engine.GetMenuState(id, bEnabled, bChecked, strName);
			if (*strName && bEnabled != -1)
			{
				menu.AppendMenu(MF_STRING, id, strName);
				++pos;
			}
		}
		return true;
	} 
	if (sid == "ID_VIDEOSETTINGS0_DEVICE*")
	{
		for (int id = ID_VIDEOSETTINGS0_DEVICE0, n = 0; id <= ID_VIDEOSETTINGS0_DEVICE99; ++id, ++n)
		{
			BOOL bChecked = FALSE, bEnabled = FALSE;
			TCHAR strName[256] = { 0 };

			theApp.m_pEngine->engine.GetMenuState(id, bEnabled, bChecked, strName);

			if (*strName && bEnabled != -1)
			{
				menu.AppendMenu(MF_STRING, id, strName);
				++pos;
			}
		}
		return true;
	}
	if (sid == "ID_AUDIOSETTINGS0*")
	{
		for (int id = ID_AUDIOSETTINGS0_0, n = 0; id <= ID_AUDIOSETTINGS0_15; ++id, ++n)
		{
			BOOL bChecked = FALSE, bEnabled = FALSE;
			TCHAR strName[256] = { 0 };

			theApp.m_pEngine->engine.GetMenuState(id, bEnabled, bChecked, strName);
			if (*strName && bEnabled != -1)
			{
				menu.AppendMenu(MF_STRING, id, strName);
				++pos;
			}
		}
		return true;
	}
	if (sid == "ID_AUDIO_TH*")
	{
		for (int id = ID_AUDIO_TH00, n = 0; id <= ID_AUDIO_TH15; ++id, ++n)
		{
			BOOL bChecked = FALSE, bEnabled = FALSE;
			TCHAR strName[256] = { 0 };

			theApp.m_pEngine->engine.GetMenuState(id, bEnabled, bChecked, strName);
			if (*strName && bEnabled != -1)
			{
				menu.AppendMenu(MF_STRING, id, strName);
				++pos;
			}
		}
		return true;
	}
	if (sid == "ID_AUDIOSETTINGS0_DEVICE*")
	{
		for (int id = ID_AUDIOSETTINGS0_DEVICE0, n = 0; id <= ID_AUDIOSETTINGS0_DEVICE9; ++id, ++n)
		{
			BOOL bChecked = FALSE, bEnabled = FALSE;
			TCHAR strName[256] = { 0 };

			theApp.m_pEngine->engine.GetMenuState(id, bEnabled, bChecked, strName);

			if (*strName && bEnabled != -1)
			{
				menu.AppendMenu(MF_STRING, id, strName);
				++pos;
			}
		}
		return true;
	}
	if (sid == "ID_AICFG*")
	{
#if defined SEQ_MEDIC_C5S || defined SEQ_VIBRAIMAGE_PRO
		for (int id = ID_AICFG_0, n = 0; id <= ID_AICFG_19; ++id, ++n)
		{
			CString name = theApp.m_pEngine->engine.GetVar(VI_INFO_AI_CONFIG,id-ID_AICFG_0,VT_BSTR);
			if (!name.IsEmpty())
			{
				menu.AppendMenu(MF_STRING, id, name);
				++pos;
			}
		}
#endif
		return true;
	}
	if (sid == "ID_LANGUAGE*")
	{
		for (int id = ID_LANGUAGE_0, n = 0; id <= ID_LANGUAGE_19; ++id,  ++n)
		{
			CString name = theApp.m_pEngine->engine.GetLang()->Name(n);
			if (!name.IsEmpty())
			{
				menu.AppendMenu(MF_STRING, id, name);
				++pos;
			}
		}
		return true;
	}
	if (sid == "ID_SETTINGS_DEFAULTS*")
	{
		for (int id = ID_SETTINGS_DEFAULTS_0, n = 0; id < ID_SETTINGS_DEFAULTS_0+100; ++id, ++n)
		{
			menu.AppendMenu(MF_STRING, id, L"*");
			++pos;
		}
		return true;
	}
	if (sid == "ID_LOAD_LD*")
	{
		for (int id = ID_LOAD_LD_0, n = 0; id < ID_LOAD_LD_0 + 100; ++id, ++n)
		{
			menu.AppendMenu(MF_STRING, id, L"*");
			++pos;
		}
		return true;
	}
	if (sid == "ID_SETTINGS_CORES*")
	{
		for (int id = ID_SETTINGS_CORES_0, n = 0; id < ID_SETTINGS_CORES_0 + (int)theApp.m_pEngines.size(); ++id, ++n)
		{
			CString name;
			name.Format(_T("%d"), n + 1);
			menu.AppendMenu(MF_STRING, id, name);
			++pos;
		}
		return true;
	}
	return false;
}



int CVibraimageExMenu::CalcMaxButtonHeight()
{
	return 16;
}

CSize  CVibraimageExMenu::CalcSize(	BOOL bVertDock	)
{
//	return CSize(16, 16);
	return CMFCMenuBar::CalcSize(bVertDock);
}

int	CVibraimageExMenu::MapID(int id)
{
	std::map< int, ITEM >::iterator i = m_items.find(id);
	if (i == m_items.end())
		return id;

	return i->second.iid;
}

int 	CVibraimageExMenu::GetTextID(int id, bool bChecked)
{
	std::map< int, ITEM >::iterator i = m_items.find(id);
	if (i == m_items.end())
		return 0;

	return (bChecked && i->second.cid > 0) ? i->second.cid : i->second.lid;
}

void CVibraimageExMenu::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMFCMenuBar::OnShowWindow(bShow, nStatus);

	if(bShow)
		m_tShow = GetTickCount64();

}


void CVibraimageExMenu::Show(bool bShow) {
	
	if (bShow) {
		ShowWindow(SW_SHOW);
		SetFocus();
		m_tShow = GetTickCount64();
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}

void CVibraimageExMenu::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent != 1) {
		CMFCMenuBar::OnTimer(nIDEvent);
		return;
	}
	bool bFocus = GetFocus() && GetFocus()->m_hWnd == m_hWnd;
	if (IsWindowVisible()) {
		if (!bFocus && GetTickCount64() - m_tShow > 6000) {
			Show(false);
		}
	}
	else {
		CPoint p;
		CRect r;
		GetWindowRect(&r);
		GetCursorPos(&p);

		if (bFocus || r.PtInRect(p) || (GetKeyState(VK_F10) & 0x8000))
			Show(true);

	}

}


bool CVibraimageExMenu::GetFlag(LPCSTR tag) {
	CStringA str = m_flags;
	while (!str.IsEmpty())
	{
		int pos = str.Find("|");
		if (pos < 0)
			break;

		if (str.Left(pos).CompareNoCase(tag) == 0)
			return true;

		str = str.Mid(pos + 1);
	}
	return str.CompareNoCase(tag) == 0;
}