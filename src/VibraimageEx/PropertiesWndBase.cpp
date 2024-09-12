// PropertiesWndBase.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "PropertiesWndBase.h"
#include "PropertiesPack.h"

namespace CDockablePaneVI
{

	// CPropertiesWndBase

	IMPLEMENT_DYNAMIC(CPropertiesWndBase, CDockablePane)

		CPropertiesWndBase::CPropertiesWndBase(CPropertiesPack *pPack, int iid) : m_pPack(pPack), m_iid(iid)
	{
		m_nLoad = m_nAdjust = 0;
		m_bSimple = false;
		m_bRecentVisibleStateRef = true;
	}

	CPropertiesWndBase::~CPropertiesWndBase()
	{
	}


	BEGIN_MESSAGE_MAP(CPropertiesWndBase, CDockablePane)
		ON_WM_SHOWWINDOW()
		ON_WM_TIMER()
		ON_WM_ERASEBKGND()
	END_MESSAGE_MAP()

	CBrush* CPropertiesWndBase::GetWndBrushColor(COLORREF c) {
		CBrush& br = m_brushes[c];
		if (!br.m_hObject)
			br.CreateSolidBrush(c);
		return &br;
	}

	CBrush* CPropertiesWndBase::GetWndBrush(int id) {
		return GetWndBrushColor(GetWndColor(id));
	}

	COLORREF CPropertiesWndBase::GetWndColor(int id) {

		static std::map<int, COLORREF> clr = {
			{ (int)CLR_GrayedText, GetGlobalData()->clrGrayedText },
			{ (int)CLR_BarShadow, GetGlobalData()->clrBarShadow },
			{ (int)CLR_BtnShadow, GetGlobalData()->clrBtnShadow },
			{ (int)CLR_ActiveCaption, (COLORREF)GetSysColor(COLOR_ACTIVECAPTION) },
			{ (int)CLR_Hilite, (COLORREF)GetSysColor(COLOR_HIGHLIGHT) },
			{ (int)CLR_Window,	 (COLORREF)GetSysColor(COLOR_WINDOW) },
			{ (int)CLR_FillName , (COLORREF)GetSysColor(COLOR_WINDOW)},
			{ (int)CLR_FillValue , (COLORREF)GetSysColor(COLOR_WINDOW)},
			{ (int)CLR_TextName , (COLORREF)GetSysColor(COLOR_WINDOWTEXT)},
			{ (int)CLR_TextValue, (COLORREF)GetSysColor(COLOR_WINDOWTEXT) }
		};
		return clr[id];
	}

	// CPropertiesWndBase message handlers


	void CPropertiesWndBase::AdjustLayout()
	{
		++m_nAdjust;
		if (m_hWnd && m_nAdjust == 1)
		{
			SetTabIconOnly(true);

			SetTimer(T_LAYOUT, 200, 0);
		}
		--m_nAdjust;
	}

	void CPropertiesWndBase::AdjustLayoutFn()
	{
		if (GetSafeHwnd() == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
		{
			return;
		}

		CBaseTabbedPane *pBar = GetParentTabbedPane();
		if (pBar)
		{
			pBar->AdjustLayout();
			pBar->RecalcLayout();
		}

	}

	BOOL CPropertiesWndBase::GetRecentVisibleState()
	{
		return  __super::GetRecentVisibleState(); // &&  m_bRecentVisibleStateRef;
	}

	bool CPropertiesWndBase::Load(xml_node& xml)
	{
		if (!xml)
			return false;
		xml_attribute a = xml.attribute("show");
		m_bRecentVisibleState = (a && a.value() && !strcmp(a.value(), "true"));
		m_bRecentVisibleStateRef = !!m_bRecentVisibleState;
		
		m_flags = xml.attribute_value("flags");
		m_bSimple = GetFlag("simple");
		return true;
	}

	void  CPropertiesWndBase::SetTabIconOnly(BOOL bSet)
	{
		CBaseTabbedPane *pBar = GetParentTabbedPane();
		if (pBar)
		{
			CTabbedPane* pTabControlBar = DYNAMIC_DOWNCAST(CTabbedPane, pBar);
			CMFCTabCtrl *pTab;
			pBar->EnableSetCaptionTextToTabName(FALSE);

			if (pTabControlBar && (pTab = pTabControlBar->GetTabWnd()))
			{
				int cnt = pTab->GetTabsNum();
				for (int i = 0; i < cnt; ++i)
				{
					pTab->SetTabIconOnly(i, bSet);
				}

				pTabControlBar->AdjustLayout();
			}
		}
	}

	LPCWSTR	CPropertiesWndBase::GetLang(int iid, LPCWSTR def)
	{
		return m_pPack->GetLang(iid, def);
	}

	LPCWSTR	CPropertiesWndBase::GetLang(LPCWSTR sid, LPCWSTR def)
	{
		return m_pPack->GetLang(sid, def);
	}

	int		CPropertiesWndBase::GetLangId(LPCWSTR sid)
	{
		return m_pPack->GetLangId(sid);
	}

	CDockablePane* CPropertiesWndBase::AttachToTabWnd(
		CDockablePane* pTabControlBarAttachTo,
		AFX_DOCK_METHOD dockMethod,
		BOOL bSetActive /* = TRUE */,
		CDockablePane** ppTabbedControlBar /* = NULL */
		)
	{
		CDockablePane* pWnd =  CDockablePane::AttachToTabWnd(pTabControlBarAttachTo, dockMethod, bSetActive, ppTabbedControlBar);
		
		OnUpdateTitle();

		return pWnd;
	}



	void CPropertiesWndBase::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		CDockablePane::OnShowWindow(bShow, nStatus);
		OnUpdateTitle();
	}

	void  CPropertiesWndBase::OnUpdateTitle(bool bReal)
	{
		if (!bReal)
		{
			KillTimer(T_TITLE);
			SetTimer(T_TITLE, 250, 0);
			return;
		}

		SetTabIconOnly(TRUE);

		CBaseTabbedPane *pBar = GetParentTabbedPane();
		if (pBar)
		{
			CTabbedPane* pTabControlBar = DYNAMIC_DOWNCAST(CTabbedPane, pBar);
			CMFCTabCtrl *pTab;
			CString strTitle = _T("");

			pBar->EnableSetCaptionTextToTabName(FALSE);

			if (pTabControlBar && (pTab = pTabControlBar->GetTabWnd()))
			{

				int cnt = pTab->GetTabsNum();

				int i = pTab->GetActiveTab();

				if (i >= 0)
				{
					CWnd *pWnd = pTab->GetTabWnd(i);
					if (pWnd)
					{
						int iid = ::GetWindowLong(*pWnd, GWL_ID);

						CPropertiesPack::PANE* pp = 0;
						if (m_pPack)
							pp = m_pPack->GetInfo(iid);
						if (pp)
							strTitle = GetLang(CString(pp->title), CString(pp->title));

					}
				}
			}
			pBar->SetWindowText(strTitle);
		}
		else {
			EnableGripper(!m_bSimple);
		}

		KillTimer(T_TITLE);
	}

	void CPropertiesWndBase::OnTimer(UINT_PTR nIDEvent)
	{
		if (m_nLoad)
			return;
		if (nIDEvent == T_TITLE)
		{
			OnUpdateTitle(true);
		}
		else
			CDockablePane::OnTimer(nIDEvent);
	}

	BOOL CPropertiesWndBase::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
	{
		if (m_pPack->CanLoadState())
			return __super::LoadState(lpszProfileName, nIndex, uiID);
		return FALSE;
	}

	BOOL CPropertiesWndBase::SaveState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
	{
		if (m_pPack->CanSaveState())
			return __super::SaveState(lpszProfileName, nIndex, uiID);
		return FALSE;
	}


	bool CPropertiesWndBase::GetFlag(LPCSTR tag) {
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
}



BOOL CDockablePaneVI::CPropertiesWndBase::OnEraseBkgnd(CDC* pDC)
{
	return CDockablePane::OnEraseBkgnd(pDC);
}
