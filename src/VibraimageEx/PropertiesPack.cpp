#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExFrame.h"
#include "PropertiesPack.h"
#include "PropertiesWndBase.h"
#include "PropertiesWnd.h"
#include "safe_delete.h"
#include "Resource.h"

namespace CDockablePaneVI
{

	CPropertiesPack::CPropertiesPack(CFrameWndEx *pFrame) :m_pFrame(pFrame)
	{
		
	}



	CPropertiesPack::~CPropertiesPack()
	{
		RemoveAll();
	}

	void CPropertiesPack::RemoveAll() {
		for (auto& wnd : m_wnd) {
			if (wnd.second.pWnd)
				delete wnd.second.pWnd;
		}
		m_wnd.clear();
		m_wndList.clear();
	}

	bool CPropertiesPack::LoadXml(LPCWSTR path, int depth)
	{
		if (depth > 10)
			return false;

		xml_document doc;
		if (!doc.load_file(path))
			return false;
		xml_node xml = doc.first_child();
		for (xml_node_iterator i = xml.begin(), ie = xml.end(); i != ie; ++i)
		{
			const char* name = i->name();
			if (strcmp(name, "include") == 0)
			{
				
				CString base = GetFilePath(path);
				LoadXml(base + i->wchild_value().c_str(), depth + 1);
			}
			else
				CreatePane(*i);
		}
		
		return true;
	}

	bool CPropertiesPack::Reload()
	{
		for (std::list<PANE*>::iterator i = m_wndList.begin(), ie = m_wndList.end(); i != ie; ++i)
		{
			PANE & pane = *(*i);
			pane.pWnd->Reload();
		}
		return true;
	}

	bool CPropertiesPack::Load(LPCWSTR path)
	{
		auto cfg = GetConfigBase();
		if (!cfg)
			return false;

		m_path = path ? path : GetDefaultPath() + cfg + _T("\\frame.xml");

		if (path)
			WriteProfileString(_T("PropertiesPack"), _T("path"), m_path);

		if (!LoadXml(m_path))
			return false;

		for (std::list<PANE*>::iterator i = m_wndList.begin(), ie = m_wndList.end(); i != ie; ++i)
		{
			PANE & pane = *(*i);
			pane.pWnd->EnableDocking(CBRS_ALIGN_ANY);
		}

		return true;
	}

	void CPropertiesPack::DockPanes()
	{
		std::map<CStringA, CDockablePane*> pTabbedBar;
		std::map<CStringA, CDockablePane* > &pWnd = m_wndTabs;

		CDockingManager *pMgr = m_pFrame->GetDockingManager();

		for (std::list<PANE*>::iterator i = m_wndList.begin(), ie = m_wndList.end(); i != ie; ++i)
		{
			PANE & pane = *(*i);
			std::vector<CStringA> tab = explode(pane.tab, CStringA("/"));


			if (DockPane(pane))
			{
				// do nothing
			} 
			else
			if (pWnd.find(tab[0]) == pWnd.end())
			{
				pWnd[tab[0]] = pane.pWnd;

				UINT wnd_flags = GetWindowLong(pane.pWnd->m_hWnd, GWL_STYLE);
				UINT align_flags = wnd_flags & (CBRS_BOTTOM | CBRS_TOP | CBRS_RIGHT | CBRS_LEFT);

				if(align_flags)
					m_pFrame->DockPane(pane.pWnd);
				else {
					CRect r(100, 100, 500, 500);
					if (pane.sizeDefault.cx && pane.sizeDefault.cy) {
						CRect rw;
						
						m_pFrame->GetWindowRect(&rw);
						r.left = rw.CenterPoint().x - pane.sizeDefault.cx / 2;
						r.top = rw.CenterPoint().y - pane.sizeDefault.cy / 2;
						r.right = r.left + pane.sizeDefault.cx;
						r.bottom = r.top + pane.sizeDefault.cy;
					}
					pane.pWnd->FloatPane(r);
				}
			}
			else
			{
				if (tab.size() > 1)
				{
					int flags = CBRS_ALIGN_ANY;
					if (tab[1] == "left")
						flags = CBRS_ALIGN_LEFT;
					if (tab[1] == "right")
						flags = CBRS_ALIGN_RIGHT;
					if (tab[1] == "top")
						flags = CBRS_ALIGN_TOP;
					if (tab[1] == "bottom")
						flags = CBRS_ALIGN_BOTTOM;

					pane.pWnd->DockToWindow(pWnd[tab[0]], flags);
				}
				else
					pane.pWnd->AttachToTabWnd(pWnd[tab[0]], DM_SHOW, TRUE, &pTabbedBar[tab[0]]);
			}

			if (pane.iconHandle)
				pane.pWnd->SetIcon(pane.iconHandle, FALSE);

			if (pane.pWnd->GetFlag("hidden"))
				pane.pWnd->ShowPane(FALSE, FALSE, FALSE);
		}

		for (std::map<CStringA, CDockablePane*>::iterator i = pTabbedBar.begin(), ie = pTabbedBar.end(); i != ie; ++i)
		{
			if (i->second)
			{
				i->second->AdjustLayout();
				i->second->RecalcLayout();
			}
		}
	}

	CPropertiesWndBase* CPropertiesPack::Find(CStringA sid) {
		std::list<PANE*>::iterator i, e;
		for (i = m_wndList.begin(), e = m_wndList.end(); i != e; ++i)
			if (*i && (*i)->sid == sid)
				return (*i)->pWnd;
		return 0;
	}

	CPropertiesWndBase * CPropertiesPack::CreateWnd(const CStringA& type, int iid)
	{
		if (type == "property")
			return new CPropertiesWnd(this, iid);
		return 0;
	}

	CSize CPropertiesPack::GetDefaultSize()
	{
		HWND hWnd = ::GetDesktopWindow();
		CWnd *pWnd = CWnd::FromHandle(hWnd);

		CDC *pDC = pWnd->GetDC();
		CSize s = pDC->GetTextExtent(_T("0123456789"));

		pWnd->ReleaseDC(pDC);

		return CSize(s.cx * 4, s.cy * 20);
	}

	CDockablePane * CPropertiesPack::CreatePane(xml_node& xml)
	{
		CStringA type = xml.name();
		CString imgPath = GetFilePath(m_path) + _T("images\\");

		PANE pane;
		pane.iid = 0;
		pane.iconHandle = 0;
		pane.type = type;
		pane.sizeDefault = GetDefaultSize();

		pane.iconPath = attrib(xml, "icon");
		pane.sid = attrib(xml, "id");
		pane.title = attrib(xml, "title");
		pane.align = attrib(xml, "align");
		pane.tab = attrib(xml, "tab");
		pane.xscroll = (float)_tstof(attrib(xml, "xscroll"));
		pane.show = attrib(xml, "show");

		if (pane.tab.IsEmpty())
			pane.tab = "*";
		pane.pWnd = 0;


		CStringA strSize(attrib(xml, "size"));

		if (!strSize.IsEmpty())
		{
			std::vector<CStringA> arr = explode(strSize, CStringA(","));
			if (arr.size() == 2)
			{
				if (arr[0].Right(1) == "%")
					pane.sizeDefault.cx = pane.sizeDefault.cx*atoi(arr[0]) / 100;
				else
					pane.sizeDefault.cx = atoi(arr[0]);

				if (arr[1].Right(1) == "%")
					pane.sizeDefault.cy = pane.sizeDefault.cy*atoi(arr[1]) / 100;
				else
					pane.sizeDefault.cy = atoi(arr[1]);
			}
			else
				if (arr.size() == 1)
				{
					if (arr[0].Right(1) == "%")
						pane.sizeDefault.cy = pane.sizeDefault.cy*atoi(arr[0]) / 100;
					else
						pane.sizeDefault.cy = atoi(arr[0]);
				}
		}

		if (pane.sid.IsEmpty())
			return 0;

		pane.iid = Tag2Id(CString(pane.sid));

		int iCXS = GetSystemMetrics(SM_CXSMICON);
		int iCYS = GetSystemMetrics(SM_CYSMICON);

		if (!pane.iconPath.IsEmpty())
			pane.iconHandle = (HICON)LoadImage(0, imgPath + pane.iconPath, IMAGE_ICON, iCXS, iCYS, LR_DEFAULTCOLOR | LR_LOADFROMFILE);

		CreatePaneProperty(xml, pane);

		if (pane.iid && pane.pWnd)
		{
			m_wnd[pane.iid] = pane;
			m_wndList.push_back(&m_wnd[pane.iid]);
		}
		else
			return 0;

		return pane.pWnd;
	}

	CDockablePane * CPropertiesPack::CreatePaneProperty(xml_node& xml, PANE& pane)
	{
		CStringA type = xml.name();
		CPropertiesWndBase *pWnd = CreateWnd(type, pane.iid);
		if (!pWnd)
			return 0;
		pane.pWnd = pWnd;

		CString strTitle = GetLang(CString(pane.title), CString(pane.title));
		UINT flags_align = CBRS_LEFT;

		if (pane.align == "bottom")
			flags_align = CBRS_BOTTOM;
		else
		if (pane.align == "top")
			flags_align = CBRS_TOP;
		else
		if (pane.align == "right")
			flags_align = CBRS_RIGHT;
		else
		if (pane.align == "float")
			flags_align = CBRS_NOALIGN;

		if (!pane.pWnd->CreateEx(0, strTitle, m_pFrame, CRect(0, 0, pane.sizeDefault.cx, pane.sizeDefault.cy),
			TRUE, pane.iid,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | flags_align | CBRS_FLOAT_MULTI,
			AFX_CBRS_REGULAR_TABS,
			AFX_DEFAULT_DOCKING_PANE_STYLE | AFX_SCBS_EDGETOP
			))
		{
			TRACE0("Failed to create window\n");
			SAFE_DELETE(pane.pWnd);

			return 0; // failed to create
		}

		pWnd->Load(xml);

		return pane.pWnd;
	}


	void CPropertiesPack::RegSave()
	{
		for (std::map<int, PANE>::iterator i = m_wnd.begin(), ie = m_wnd.end(); i != ie; ++i)
		{
			if (IsWindow(i->second.pWnd->m_hWnd))
				i->second.pWnd->SendMessage(WM_COMMAND, ID_COMMAND_REG_SAVE, 0);
			else
			{
				DEBUG_ONLY(((ULONG)GetTickCount64()));
			}
		}
	}

	void CPropertiesPack::ShowAll(BOOL bShow)
	{
		for (std::map<int, PANE>::iterator i = m_wnd.begin(), ie = m_wnd.end(); i != ie; ++i)
		{
			BOOL bs = i->second.show != "always" ? bShow : TRUE;
			if (IsWindow(i->second.pWnd->m_hWnd))
				i->second.pWnd->ShowPane(bs, TRUE, FALSE);
		}
	}

	void CPropertiesPack::OnInit()
	{
		//	RecalcLayout();
		AdjustLayout();
	}


	void CPropertiesPack::RecalcLayout()
	{
		for (std::list<PANE*>::iterator i = m_wndList.begin(), ie = m_wndList.end(); i != ie; ++i)
		{
			PANE & pane = *(*i);
			pane.pWnd->RecalcLayout();
		}
	}

	void CPropertiesPack::AdjustVisible()
	{
		for (std::list<PANE*>::iterator i = m_wndList.begin(), ie = m_wndList.end(); i != ie; ++i)
		{
			PANE & pane = *(*i);
			if (pane.pWnd->GetRecentVisibleState())
			{
				pane.pWnd->ShowPane(TRUE, FALSE, FALSE);
			}
			else
			{
				pane.pWnd->ShowPane(FALSE, FALSE, FALSE);
			}
		}
	}

	void CPropertiesPack::AdjustLayout()
	{
		for (std::list<PANE*>::iterator i = m_wndList.begin(), ie = m_wndList.end(); i != ie; ++i)
		{
			PANE & pane = *(*i);

			pane.pWnd->AdjustLayout();
		}
	}

	COleVariant CPropertiesPack::ReScale(ITEM_VALUE *pItem, const COleVariant& a, bool bDiv)
	{
		COleVariant v = a;

		if (pItem->scale == 1 || pItem->scale == 0)
			return v;

		switch (v.vt)
		{
		case VT_R4:
			if (bDiv)
				v.fltVal /= pItem->scale;
			else
				v.fltVal *= pItem->scale;
			break;
		case VT_R8:
			if (bDiv)
				v.dblVal /= pItem->scale;
			else
				v.dblVal *= pItem->scale;
			break;
		case VT_I8:
			if (bDiv)
				v.llVal /= (int)pItem->scale;
			else
				v.llVal *= (int)pItem->scale;
			break;
		case VT_I4:
			if (bDiv)
				v.lVal /= (int)pItem->scale;
			else
				v.lVal *= (int)pItem->scale;
			break;
		case VT_INT:
			if (bDiv)
				v.intVal /= (int)pItem->scale;
			else
				v.intVal *= (int)pItem->scale;
			break;
		default: return v;
		}

		return v;
	}

	int CPropertiesPack::GetValueSelect(ITEM_VALUE *pItem, LPCWSTR str)
	{
		for (size_t i = 0, cnt = pItem->swName.size(); i < cnt; ++i)
		{
			if (pItem->swName[i] == str)
				return pItem->swValue[i];
		}
		return -1;
	}


	CString CPropertiesPack::GetValueSelect(ITEM_VALUE *pItem, int id)
	{
		for (size_t i = 0, cnt = pItem->swName.size(); i < cnt; ++i)
		{
			if (pItem->swValue[i] == id)
				return pItem->swName[i];
		}
		return CString();
	}

	CString CPropertiesPack::GetDefaultFormat(int typeID)
	{
		switch (typeID)
		{
		case VI_VAR_SUB_F1:
		case VI_VAR_SUB_F2:
			return _T("%f");
		case VI_VAR_SUB_BOOL1:
		case VI_VAR_SUB_BOOL2:
			return _T("%d");
		case VI_VAR_SUB_I1:
		case VI_VAR_SUB_I2:
			return _T("%d");
		case VI_VAR_SUB_STRING:
		case VI_VAR_SUB_SELECT1:
		case VI_VAR_SUB_SELECT2:
			return _T("%s");
		default: break;
		}

		return _T("");
	}

	CPropertiesPack::PANE* CPropertiesPack::GetInfo(int iid)
	{
		std::map<int, PANE>::iterator i = m_wnd.find(iid);
		if (i != m_wnd.end())
			return &(i->second);
		return 0;
	}

	CDockablePane* CPropertiesPack::GetTab(LPCSTR tab)
	{
		if (!tab || !*tab)
			return NULL;
		std::map<CStringA, CDockablePane* >::iterator i = m_wndTabs.find(tab);
		if (i == m_wndTabs.end())
			return NULL;
		return i->second;
	}

	void CPropertiesPack::UpdateAll()
	{
		for (std::list<PANE*>::iterator i = m_wndList.begin(), ie = m_wndList.end(); i != ie; ++i)
		{
			PANE & pane = *(*i);
			if (pane.pWnd)
				pane.pWnd->UpdateAll();
		}

	}

	bool CPropertiesPack::CryptInit(HCRYPTPROV* pc)
	{
		HCRYPTPROV hCryptProv = NULL;
		LPCWSTR UserName = _T("Vibraimage");  // name of the key container
											  // to be used
											  //--------------------------------------------------------------------
											  // Attempt to acquire a context and a key
											  // container. The context will use the default CSP
											  // for the RSA_FULL provider type. DwFlags is set to zero
											  // to attempt to open an existing key container.

		if (pc) *pc = 0;
		if (CryptAcquireContext(
			&hCryptProv,               // handle to the CSP
			UserName,                  // container name 
			NULL,                      // use the default provider
			PROV_RSA_FULL,             // provider type
			0))                        // flag values
		{
			// printf("A cryptographic context with the %s key container \n", UserName);
			// printf("has been acquired.\n\n");
		}
		else
		{
			//--------------------------------------------------------------------
			// An error occurred in acquiring the context. This could mean
			// that the key container requested does not exist. In this case,
			// the function can be called again to attempt to create a new key 
			// container. Error codes are defined in Winerror.h.
			if (GetLastError() == NTE_BAD_KEYSET)
			{
				if (CryptAcquireContext(
					&hCryptProv,
					UserName,
					NULL,
					PROV_RSA_FULL,
					CRYPT_NEWKEYSET))
				{
					// printf("A new key container has been created.\n");
				}
				else
				{
					// printf("Could not create a new key container.\n");
					return false;
				}
			}
			else
			{
				//printf("A cryptographic service handle could not be acquired.\n");
				return false;
			}

		} // End of else.
		  //--------------------------------------------------------------------
		  // A cryptographic context and a key container are available. Perform
		  // any functions that require a cryptographic provider handle.

		if (pc) *pc = hCryptProv;

		return true;
	}

	CStringA CPropertiesPack::ToHex(BYTE* data, int len)
	{
		std::vector<char> str;
		str.resize(len * 4);

		char tmp[8];
		char *Temp = &str[0];

		for (int k = 0; k < len; ++k)
		{
			BYTE v = data[k];
			UINT u = v;
			sprintf_s(tmp, 8, "%0.2X", u);
			strcat_s(Temp, str.size(), tmp);
		}
		return &str[0];
	}

}