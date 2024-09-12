#pragma once


// CVibraimageExMenu

class CVibraimageExMenu : public CMFCMenuBar
{
	DECLARE_DYNAMIC(CVibraimageExMenu)

public:
	CVibraimageExMenu();
	virtual ~CVibraimageExMenu();

protected:
	DECLARE_MESSAGE_MAP()
public:
	CList<UINT, UINT>				m_ids;
	
	typedef struct tagITEM
	{
		int iid, lid, cid;
	} ITEM;

	int						m_itemFirst;
	std::map< int, ITEM >	m_items;

	LONGLONG				m_tShow;
	CStringA				m_flags;
public:
	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE, UINT nID = AFX_IDW_MENUBAR);
	virtual void CreateFromMenu(HMENU hMenu, BOOL bDefaultMenu = FALSE, BOOL bForceUpdate = FALSE);
	virtual void CreateFromMenuStd(HMENU hMenu, BOOL bDefaultMenu = FALSE, BOOL bForceUpdate = FALSE);
	bool Load(CMenu& menu,const CString& path);
	bool Load(CMenu& menu,xml_node& xml,int depth);
	void Purge(CMenu& menu);
	virtual BOOL LoadState(	LPCWSTR lpszProfileName = NULL,	int nIndex = -1,	UINT uiID = (UINT)-1	);

	void NewMenuBar();

	CMenu *GetMenu() { return CMenu::FromHandle( GetHMenu() );  }
	bool CheckIID(int iid);
	bool AppendMenuMulti(CMenu& menu, xml_node& xml, int depth, int & pos);

	virtual int CalcMaxButtonHeight();
	virtual CSize CalcSize(BOOL bVertDock);

	virtual int	MapID(int id);
	virtual int GetTextID(int id, bool bChecked);
	virtual void Show(bool bShow);
	virtual bool GetFlag(LPCSTR tag);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);


	afx_msg void OnTimer(UINT_PTR nIDEvent);

};


