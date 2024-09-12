
// VibraimageExFrame.h : interface of the CVibraimageExFrame class
//

#pragma once
#include "VibraimageExProp.h"
#include "NewVarEventVI.h"
#include "VibraimageExMenu.h"
#include "VibraimageExToolBar.h"

class CVibraimageExApp;
class CVibraimageExView;
class CColorProcress;


class CVibraimageExFrame : public CFrameWndEx, public CNewVarEventVI
{
protected:
	CVibraimageExApp *pApp;
protected: // create from serialization only
	CVibraimageExFrame();
	DECLARE_DYNCREATE(CVibraimageExFrame)

// Attributes
public:
	enum TIMER { T_INIT=1, T_TITLE, T_CHECK, T_NEW_DOC, T_CLOSE, T_MSGBOX, T_REG_FLUSH,T_SHOWLD,T_SHOW_MAX,T_SHOW_NORM,T_CLOSE_RQST,T_MEASURE_TEST_START, T_MEASURE_SHOW};
// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CVibraimageExFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CVibraimageExMenu		m_wndMenuBar;
	CVibraimageExToolBar	m_wndToolBar;
	CVibraimageExToolBar	m_wndToolBarVi;
	
	CVibraimageExPropPack	m_wndPane;

	std::shared_ptr< CColorProcress >	m_wndMsg;
public:
	CPropertiesWndBase	*	m_pContextMenuHolder;
public:
	int						m_nInit;
	int						m_bNewMenu;
	int						m_bNewDB;
	std::map<int, int>		m_timerData;

//	CWndDlgbarLDLite		m_wndLDLite;

#ifndef SEQ_DISABLE_HIST
	
#endif // #ifndef SEQ_DISABLE_HIST

protected:
// License check
	bool ShowHTTPLicenseDialog();
	bool TestLicense();
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);


	afx_msg void OnViewCustomize();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	LRESULT OnResetMenu(WPARAM, LPARAM);
public:
	afx_msg void OnMENUID(UINT id);
	afx_msg void OnUpdateMENUID(CCmdUI* pCmdUI);
	bool		 OnPreMENUID(UINT id);
	bool		 OnPreUpdateMENUID(int id, BOOL& bEnabled, BOOL& bChecked, LPWSTR pText);
	void OnCommandCheckid(BOOL *pResult);

	bool OnMenuMeasurement(UINT id);
public:
	int Message(int idCap, int idMsg, int buttons = MB_OK);
	int Message(int idCap, int idMsg, const CString &sAdd, int buttons = MB_OK);

	virtual void OnUpdateFrameMenu(HMENU hMenuAlt);
	virtual BOOL OnShowPopupMenu(CMFCPopupMenu* pMenuPopup);
	virtual void UpdatePopupMenu(CMFCPopupMenu* pMenuPopup);

	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);

	BOOL DoModalLoop();

	void OnAVIStart();
	void OnAVIStop();
	void PutMarker(int id);
	void OnTimerAVIRestartNow();
	void OnInitView(CVibraimageExView* pView, int step);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	bool OnInit();

	void UpdateTitle();

	void MakeLoadState();
	void OnSettingsLoad(LPCSTR szXmlStr);

	virtual void OnNewVar(int id, int subID);

	virtual void OnMenuRef(int id, int step);

	CString GetToolbarPath();

	virtual void ShowPane(CBasePane* pBar, BOOL bShow, BOOL bDelay, BOOL bActivate);

	void UpdateSysMenu();

	void MakeLoadStateInit();
	void MakeSaveSettings(CString path,std::vector<char> *pXML=0,bool bEn=true);
	void MakeLoadPos();
	void MakeSavePos(bool bSet = false);
public:
	void FormatMenu(CCmdUI * pCmdUI, const CString& txt, BOOL en = true, BOOL ch = false);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	afx_msg void OnHelpAbout();
	afx_msg void OnUpdateHelpAbout(CCmdUI *pCmdUI);
	afx_msg void OnHelpHelp();
	afx_msg void OnUpdateHelpHelp(CCmdUI* pCmdUI);

	afx_msg void OnViewSimple();
	afx_msg void OnUpdateViewSimple(CCmdUI *pCmdUI);
	afx_msg void OnViewResetView();
	afx_msg void OnUpdateResetView(CCmdUI* pCmdUI);
	afx_msg void OnSettingsLock();
	afx_msg void OnUpdateSettingsLock(CCmdUI* pCmdUI);

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewOverview();
	afx_msg void OnUpdateViewOverview(CCmdUI *pCmdUI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnMove(int x, int y);
};


