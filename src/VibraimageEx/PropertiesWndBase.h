#pragma once


namespace CDockablePaneVI
{

	class CPropertiesPack;

	// CPropertiesWndBase

	class CPropertiesWndBase : public CDockablePane
	{
		DECLARE_DYNAMIC(CPropertiesWndBase)
	public:
		CPropertiesPack *	m_pPack;
		int					m_iid;
		int					m_nLoad;
		int					m_nAdjust;
		bool				m_bSimple;
		bool				m_bRecentVisibleStateRef;
		CStringA			m_flags;

		std::map<COLORREF, CBrush> m_brushes;

		enum TIMER { T_UPDATE = 100, T_TITLE, T_LAYOUT };
	public:
		CPropertiesWndBase(CPropertiesPack *pPack, int iid);
		virtual ~CPropertiesWndBase();

		virtual COLORREF GetWndColor(int id);
		virtual CBrush* GetWndBrushColor(COLORREF c);
		virtual CBrush* GetWndBrush(int id);

		virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT)-1);
		virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT)-1);

		virtual BOOL CanFloat() const { return !m_bSimple; }
		virtual BOOL CanBeAttached() const { return !m_bSimple; }
		virtual BOOL CanAutoHide() const { return !m_bSimple; }
		virtual BOOL CanAcceptPane(const CBasePane* p) const {	return m_bSimple ? false : CDockablePane::CanAcceptPane(p);	}
		virtual void AdjustLayout();
		virtual void AdjustLayoutFn();

		virtual bool Load(xml_node& xml);
		virtual bool Reload() {  return false; }


		virtual void SetTabIconOnly(BOOL bSet);

		virtual void UpdateAll() {}

		virtual LPCWSTR	GetLang(int iid, LPCWSTR def = 0);
		virtual LPCWSTR	GetLang(LPCWSTR sid, LPCWSTR def = 0);
		virtual int		GetLangId(LPCWSTR sid);

		virtual BOOL GetRecentVisibleState();

		virtual CDockablePane* AttachToTabWnd(
			CDockablePane* pTabControlBarAttachTo,
			AFX_DOCK_METHOD dockMethod,
			BOOL bSetActive = TRUE,
			CDockablePane** ppTabbedControlBar = NULL
			);

		virtual void OnShowWindow(BOOL bShow, UINT nStatus);
		virtual void OnTimer(UINT_PTR nIDEvent);

		virtual void OnUpdateTitle(bool bReal = false);

		virtual bool CanEdit(CMFCPropertyGridProperty * pItm) { return true;  }

		virtual bool GetFlag(LPCSTR tag);
	protected:
		DECLARE_MESSAGE_MAP()

	public:
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	};


}