
// VibraimageExFrame.cpp : implementation of the CVibraimageExFrame class
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExDoc.h"
#include "VibraimageExView.h"
#include "VibraimageExFrame.h"
#include "VibraimageExGraph.h"
#include "VibraimageExHist.h"
#include "VibraimageExLD.h"
#include "VibraimageExOverview.h"
#include "VibraimageExQT.h"
#include "CVideoCheck.h"
#include "CDlgLogin.h"
#include "About.h"
#include "ViHTTPLicenseDlg.h"
#include "UserIdDlg.h"
#include "PwdDlg.h"
#include "CHTMLResult.h"
#include <afxregpath.h>
#include "CDlgStarter.h"
#include <sstream>
#include "AMCap.h"
#include "regdelete.h"
#include "CViLogStorage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WM_ABORT_CLOSE (WM_USER+4)

// CVibraimageExFrame

IMPLEMENT_DYNCREATE(CVibraimageExFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;


BEGIN_MESSAGE_MAP(CVibraimageExFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE(AFX_WM_RESETMENU, &CVibraimageExFrame::OnResetMenu)
	ON_WM_INITMENUPOPUP()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_SIZE()

	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CVibraimageExFrame::OnViewCustomize)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CVibraimageExFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CVibraimageExFrame::OnUpdateApplicationLook)


	ON_COMMAND_RANGE(ID_FILE_PAGE_SETUP, ID_FILE_PRINT_PREVIEW, &CVibraimageExFrame::OnMENUID)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_PAGE_SETUP, ID_FILE_PRINT_PREVIEW, &CVibraimageExFrame::OnUpdateMENUID)


	ON_COMMAND(ID_HELP_ABOUT, &CVibraimageExFrame::OnHelpAbout)
	ON_UPDATE_COMMAND_UI(ID_HELP_ABOUT, &CVibraimageExFrame::OnUpdateHelpAbout)

	ON_COMMAND(ID_HELP_HELP, &CVibraimageExFrame::OnHelpHelp)
	ON_UPDATE_COMMAND_UI(ID_HELP_HELP, &CVibraimageExFrame::OnUpdateHelpHelp)

	ON_COMMAND(ID_VIEW_SIMPLE, &CVibraimageExFrame::OnViewSimple)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SIMPLE, &CVibraimageExFrame::OnUpdateViewSimple)

	ON_COMMAND(ID_VIEW_RESETVIEW, &CVibraimageExFrame::OnViewResetView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESETVIEW, &CVibraimageExFrame::OnUpdateResetView)

	ON_COMMAND(ID_SETTINGS_LOCK, &CVibraimageExFrame::OnSettingsLock)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_LOCK, &CVibraimageExFrame::OnUpdateSettingsLock)

	ON_COMMAND(ID_VIEW_OVERVIEW, &CVibraimageExFrame::OnViewOverview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OVERVIEW, &CVibraimageExFrame::OnUpdateViewOverview)

	ON_COMMAND_RANGE(ID_VI_MENUID_START, ID_VI_MENUID_END, &CVibraimageExFrame::OnMENUID)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VI_MENUID_START, ID_VI_MENUID_END, &CVibraimageExFrame::OnUpdateMENUID)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOMMAND()
	ON_WM_MOVE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CVibraimageExFrame construction/destruction

CVibraimageExFrame::CVibraimageExFrame() :
	pApp( (CVibraimageExApp*) AfxGetApp()), 
	m_wndPane(this) 
{
	theApp.m_pFrame = this;
	m_nInit = 1;
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
	m_pContextMenuHolder = 0;

	m_bNewDB = m_bNewMenu = 0;

	for (int core = 0, coreMax = (int)theApp.m_pEngines.size(); core < coreMax; ++core) {
		pApp->m_pEngines[core]->engine.AddId(ID_VIEW_SIMPLE, _T("ID_VIEW_SIMPLE"));
	}
}

CVibraimageExFrame::~CVibraimageExFrame()
{
	theApp.m_pFrame = 0;
}

bool CVibraimageExFrame::ShowHTTPLicenseDialog()
{
	CViHTTPLicenseDlg dlg;
	if (dlg.DoModal() != IDOK)
		return false;
	return true;
}

bool CVibraimageExFrame::TestLicense()
{
	if (theApp.m_pEngine->engine.GetSeq()->SeqIsOnline())
		return true;

//	if (theApp.m_pEngine->engine.GetSeq()->SeqIsMeasure())
//		return true;
	bool tested = false;
	bool bDlg = false;


	for (int test = 5; test >= 0; --test)
	{
		if (theApp.m_pEngine->engine.GetSeq()->SeqIsOk())
			break;


		if (test && theApp.m_pEngine->engine.GetSeq()->SeqIsHttp())
		{
			if (!ShowHTTPLicenseDialog())
				test = 0;
			tested = !theApp.m_pEngine->engine.GetSeq()->SeqIsDemo() && test;
			bDlg = true;
		}


		if (!test)
		{
			theApp.m_splashScreen.Finish();

			CString msg;
			CCrushCheck::disable();
			if (theApp.m_hWndStarter)
				::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_STOP, 1);

			if (!tested) {
				msg = theApp.m_pEngine->engine.GetLang()->Get(L"ERROR_LICENSE");
				theApp.OnError(theApp.m_pEngine->id, EVENTLOG_ERROR_TYPE, msg);
			}

			if (!bDlg && !msg.IsEmpty()) {
				SetTimer(T_MSGBOX, 5000, 0);
				MessageBox(msg, m_strTitle, MB_OK | MB_ICONERROR);
				KillTimer(T_MSGBOX);
			}
			theApp.m_bAbort++;
			ModifyStyle(WS_VISIBLE, 0, 0);

			PostMessage(WM_ABORT_CLOSE);
			return false;
		}

	}

	bool ok = theApp.m_pEngine->engine.GetSeq()->SeqIsOk();
	return ok;
}

int CVibraimageExFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (lpCreateStruct)
		lpCreateStruct->style &= (~WS_VISIBLE);

	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	LockWindowUpdate();
	
	/*
	int aElements[4] = { COLOR_WINDOW, COLOR_ACTIVECAPTION, COLOR_3DFACE,COLOR_MENUBAR };
	DWORD aNewColors[4];
	aNewColors[0] = RGB(0x80, 0x80, 0x80);  // light gray 
	aNewColors[1] = RGB(0x80, 0x00, 0x80);  // dark purple 
	aNewColors[2] = RGB(0x80, 0xF0, 0x80);  
	aNewColors[3] = RGB(0x00, 0xF0, 0x80);
	SetSysColors(4, aElements, aNewColors);
	*/

	BOOL bNameValid;

	for (int core = 0, coreMax = (int)theApp.m_pEngines.size(); core < coreMax; ++core) {

		bool ok = theApp.m_pEngines[core]->engine.Start(m_hWnd) ? true : false;
		theApp.m_pEngines[core]->engine.PutVar(VI_FILTER_PAUSE, 0, COleVariant((long)1,VT_I4));
		theApp.m_pEngines[core]->pLog->Init();

		if (!core && ok) {
			if (theApp.m_pEngines[core]->engine.GetVar(VI_INFO_NEED_LOGIN, 0, VT_INT).intVal) {
				theApp.m_splashScreen.Finish();
				CDlgLogin dlg(this);
				if (theApp.m_hWndStarter)
					::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_STOP, 1);

				if (dlg.DoModal() != IDOK) {
					CCrushCheck::disable();
					ModifyStyle(WS_VISIBLE, 0, 0);
					theApp.m_bAbort++;
					PostMessage(WM_ABORT_CLOSE);
					return 0;
				}
				theApp.m_splashScreen.Start();
			}
		}

		if(core != coreMax-1) Sleep(2000);
		if (!ok || !TestLicense())
		{
			CCrushCheck::disable();
			ModifyStyle(WS_VISIBLE, 0, 0);
			theApp.m_bAbort++;
			PostMessage(WM_ABORT_CLOSE);
			return 0;
		}
		theApp.m_pEngines[core]->started = 1;
	}
	theApp.WriteProfileInt(_T("Start"), _T("nStart"), theApp.m_nStart + 1);



	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY );
	

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	CSize menuBSize = m_wndMenuBar.GetMenuButtonSize();
	CSize menuISize = m_wndMenuBar.GetMenuImageSize();

	CString toolBarPath = GetToolbarPath();
	if (PathFileExists(toolBarPath))
	{
		if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1, 1, 1, 1), ID_VIEW_MODE_TOOLBAR) )
		{
			TRACE0("Failed to create toolbar\n");
			return -1;      // fail to create
		}
		m_wndToolBar.Load(toolBarPath);

		
		CString strToolBarName = theApp.m_pEngine->engine.GetLang()->Get(_T("TOOLBAR_STANDARD"));
		
		m_wndToolBar.SetWindowText(strToolBarName);
	}

	toolBarPath = GetDefaultPath() + theApp.m_pEngine->engine.GetConfigBase() + _T("\\toolbarVI.xml");
	if (PathFileExists(toolBarPath))
	{
		if (!m_wndToolBarVi.CreateEx(theApp.m_pFrame, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1, 1, 1, 1), ID_VIEW_MODE_TOOLBAR_VI))
		{
			TRACE0("Failed to create toolbar\n");
			return -1;      // fail to create
		}
		m_wndToolBarVi.Load(toolBarPath);

		
		CString strToolBarName = theApp.m_pEngine->engine.GetLang()->Get(_T("TOOLBAR_VI"));

		m_wndToolBarVi.SetWindowText(strToolBarName);
	}


	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
//	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);


	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(  CBRS_ALIGN_TOP );

	if(IsWindow(m_wndToolBar))
		m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	if (IsWindow(m_wndToolBarVi))
		m_wndToolBarVi.EnableDocking(CBRS_ALIGN_LEFT);

	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	if (IsWindow(m_wndToolBar))
		DockPane(&m_wndToolBar); 

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	m_wndPane.Load();

	m_wndPane.DockPanes();

	
	if (IsWindow(m_wndToolBarVi))
		DockPane(&m_wndToolBarVi);

	// Load menu item image (not placed on any standard toolbars):
//	CMFCToolBar::AddToolBarForImageCollection(IDR_MAINFRAME_256,  0,0, 0);

		
	// set the visual manager and style based on persisted value
	 OnApplicationLook(theApp.m_nAppLook);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, 0, 0, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
//	CMFCToolBar::EnableQuickCustomization();


	int iCX = GetSystemMetrics(SM_CXSMICON);
	int iCY = GetSystemMetrics(SM_CYSMICON);

	m_wndMenuBar.SetMenuSizes(CSize(iCX+6, iCY+6), CSize(iCX, iCY));
	
//	if (!theApp.m_cmdl.IsSet(_T("/load_config")))
//		MakeLoadState();

//	CMFCToolBar::SetBasicCommands(m_wndMenuBar.m_ids);
	SetTimer(T_INIT, 200,0);

	UpdateSysMenu();


	if (theApp.m_hWndStarter)
		::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_TEST, (LPARAM)m_hWnd);

	if (theApp.m_cmdl.IsSet("/measure_test")) {
		SetTimer(T_MEASURE_TEST_START, 20000, 0);
	}

	theApp.RegFlush();
	return 0;
}

BOOL CVibraimageExFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	CRect rw;
	int x = theApp.GetProfileInt(L"Frame", L"frame_x", cs.x);
	int y = theApp.GetProfileInt(L"Frame", L"frame_y", cs.y);
	int cx = theApp.GetProfileInt(L"Frame", L"frame_cx", 0);
	int cy = theApp.GetProfileInt(L"Frame", L"frame_cy", 0);

	rw.left = rw.top = 0;
	rw.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	rw.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);


	if (cx && cy && x < rw.right && y < rw.bottom && x + cx >= rw.left && y + cy >= rw.top) {
		cs.cx = cx;
		cs.cy = cy;
		cs.x = x;
		cs.y = y;
		if (theApp.GetProfileInt(L"Frame", L"frame_max", 0))
			cs.style |= WS_MAXIMIZE;
	}

	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}


// CVibraimageExFrame diagnostics

#ifdef _DEBUG
void CVibraimageExFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CVibraimageExFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CVibraimageExFrame message handlers





BOOL CVibraimageExFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work
	

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	if (!theApp.m_bAbort) {
		// enable customization button for all user toolbars
		BOOL bNameValid;
		CString strCustomize;
		bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
		ASSERT(bNameValid);

		for (int i = 0; i < iMaxUserToolbars; i++)
		{
			CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
			if (pUserToolbar != NULL)
			{
				//		pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
			}
		}
	}

	return TRUE;
}


void CVibraimageExFrame::OnDestroy()
{
	if(!theApp.m_bCloseRqst)
		theApp.OnExit();

	
	CFrameWndEx::OnDestroy();

	m_wndPane.RemoveAll();
}


LRESULT CVibraimageExFrame::OnResetMenu(WPARAM, LPARAM)
{
	m_wndMenuBar.NewMenuBar();
	return 0;
}

void CVibraimageExFrame::OnUpdateFrameMenu(HMENU hMenuAlt)
{
	CFrameWndEx::OnUpdateFrameMenu(hMenuAlt);
}

void CVibraimageExFrame::UpdatePopupMenu(CMFCPopupMenu* pMenuPopup)
{
	if (theApp.m_bCloseRqst)
		return;

	if (!pMenuPopup)
		return;
	
	HMENU hMenu = pMenuPopup->GetHMenu();
	if (!hMenu)
		return;
	
	CMenu *pMenu = CMenu::FromHandle(hMenu);

	int cntm = pMenu->GetMenuItemCount();
	int cnt = pMenuPopup->GetMenuItemCount();


	for (int i = 0; i < cnt && i < cntm; ++i)
	{
		CMenu *pSub = pMenu->GetSubMenu(i);
		if (!pSub)
			continue;
		CMFCToolBarMenuButton* pb = pMenuPopup->GetMenuItem(i);

		if (!pb->m_bText || pb->m_nID != -1)
			continue;

		if (!i && pb->m_strText == _T("Toolbars"))
			pb->m_strText = theApp.m_pEngine->engine.GetLang()->Get(_T("MENU_VIEW_TOOLBARS"));

		BOOL bChecked = FALSE;

		int nSub = pSub->GetMenuItemCount();
		for (int si = 0; si < nSub; ++si)
		{
			int iid = pSub->GetMenuItemID(si);
			if (iid == -1)
				continue;

			TCHAR txt[256] = _T("");
			BOOL en = FALSE, ch = FALSE;
			pApp->m_pEngine->engine.GetMenuState(m_wndMenuBar.MapID(iid), en, ch, txt);
			if ((ch & 3) == 3)
				bChecked = TRUE;
		}

		if (bChecked)
			pb->SetStyle(pb->m_nStyle | TBBS_CHECKED);
		else
			if (pb->m_nStyle & TBBS_CHECKED)
				pb->SetStyle(pb->m_nStyle & (~TBBS_CHECKED));

	}
}

BOOL CVibraimageExFrame::OnShowPopupMenu(CMFCPopupMenu* pMenuPopup)
{
	CMFCMenuBar::SetShowAllCommands();
	
	BOOL ret =  CFrameWndEx::OnShowPopupMenu(pMenuPopup);
	
	UpdatePopupMenu(pMenuPopup);
	
	return ret;
}


void CVibraimageExFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFrameWndEx::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

BOOL CVibraimageExFrame::DoModalLoop()
{
	int cnt = 0;
	MSG msg;
	HWND hWnd = m_hWnd;

	while (m_hWnd && ::PeekMessage(&msg, hWnd, 0, -1, PM_REMOVE))
	{
		if (msg.message == WM_CLOSE)
		{
			return FALSE;
		}
		if (msg.message == WM_COMMAND && (msg.wParam == IDOK || msg.wParam == IDCANCEL))
		{
			return FALSE;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (cnt++ > 500) break;
	}

	return TRUE;
}


void CVibraimageExFrame::OnClose()
{
	KillTimer(T_CHECK);

	if (theApp.m_bCloseRqst++)
		return;


	if (theApp.m_hWndStarter) {
		if(pApp->m_cmdl.IsSet(L"/measure"))
			::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_KILL, 0);
		else
			::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_STOP, 0);

		theApp.m_hWndStarter = 0;
	}
	if (!theApp.m_bAbort)
		theApp.m_splashScreen.Start();

	if (theApp.CanSaveState()) {
		MakeSavePos(true);

		m_wndPane.RegSave();
		theApp.RegSave(true);
		theApp.RegFlush();

		std::vector<char> xmlState;
		MakeSaveSettings(CString(), &xmlState,true);
		pApp->WriteProfileBinary(_T("Settings0"), _T("engineState"), (LPBYTE)& xmlState[0], xmlState.size());
		theApp.RegFlush();

	}


	CFrameWndEx::OnClose();



	theApp.OnExit();

}


void CVibraimageExFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (theApp.m_bCloseRqst)
		return;

	if (theApp.m_bAbort) {
		KillTimer(nIDEvent);
		return;
	}
	switch (nIDEvent)
	{
	case T_INIT:
		if (!OnInit())
			KillTimer(nIDEvent);

		return;
	case T_CLOSE_RQST: {
		bool busy = false;
		for (auto& e : pApp->m_pEngines) {
			if (e && e->engine.IsStarted() && e->engineDB.IsBusy())
				busy = true;
		}
		if (!busy)
			KillTimer(nIDEvent);
			PostMessage(WM_CLOSE, 0, 0);
		}
		return;
	case T_TITLE:
		KillTimer(nIDEvent);
		theApp.m_pDoc->MakeTitle(FALSE);
		return;
	case T_REG_FLUSH:
		KillTimer(nIDEvent);
		theApp.RegFlush();
		return;
	case T_CHECK:
		KillTimer(nIDEvent);

		if (theApp.m_bCloseRqst) {
			theApp.m_bCloseRqst = 0;
			theApp.m_bAbort++;
			ModifyStyle(WS_VISIBLE, 0, 0);
			PostMessage(WM_ABORT_CLOSE);
			return;
		}

		if (!m_nInit)
		{
			if (m_bNewMenu)
			{
				m_wndMenuBar.NewMenuBar();
				m_bNewMenu = 0;
				theApp.m_pDoc->MakeTitle(FALSE);
			}
#if  !defined SEQ_MEDIC_MOL
			if (m_bNewDB)
			{
				int nRet = theApp.m_pEngine->engine.GetDB()->Reload();
				if (theApp.m_pDBList)
					theApp.m_pDBList->Reload();
			//	if (nRet >= 0) // 210831
					m_bNewDB = 0;
			}
#endif
			if (!theApp.m_pEngine->engine.GetSeq()->SeqIsOk())
			{
				CCrushCheck::disable();
				KillTimer(nIDEvent);
				theApp.m_pEngine->engine.GetSeq()->SeqIsOk();
				theApp.m_pEngine->engine.GetSeq()->SeqIsOk();

				CString msg = theApp.m_pEngine->engine.GetLang()->Get(L"ERROR_LICENSE");
				theApp.OnError(theApp.m_pEngine->id, EVENTLOG_ERROR_TYPE, msg);

				SetTimer(T_MSGBOX, 5000,0);
				MessageBox(msg, m_strTitle, MB_OK | MB_ICONERROR);
				KillTimer(T_MSGBOX);

				ModifyStyle(WS_VISIBLE, 0, 0);
				PostMessage(WM_CLOSE);
			}

			if (theApp.m_bRestart)
			{
				if (theApp.m_bRestart > 1 || MessageBox(theApp.m_pEngine->engine.GetLang()->Get(L"MESSAGE_NEED_RESTART"), 0, MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK) {
					theApp.m_bRestart++;
					PostMessage(WM_CLOSE);
				}
				else
					theApp.m_bRestart = false;
			}

			if (!theApp.IsRegLocked() && theApp.m_tRegLocked != -1) {
				int timeout = theApp.m_pEngine->engine.GetVar(VI_INFO_LOCK_PWD_TIMEOUT, 0, VT_INT).intVal;
				if (timeout > 0 && timeout < 15)
					timeout = 15;
				if (timeout > 0 && GetTickCount64() - theApp.m_tRegLocked > timeout * 1000) {
				//	AfxMessageBox(L"ddd");
					theApp.SetRegLocked(true);
				}
			}

			CHTMLResult::CheckTimer();

			CVideoCheck::CheckAll();
			CViLogStorage::CheckAll();
		}
		if (theApp.m_hWndStarter)
			::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_TEST, (LPARAM)m_hWnd);

		if (m_wndMsg) {
			if (!m_wndMsg->m_hWnd)
			{
				CRect rc;
				GetClientRect(&rc);
				CRect r(CPoint(rc.CenterPoint().x -300, rc.CenterPoint().y - 20), CSize(600, 40));
				ClientToScreen(&r);

				m_wndMsg->CreateEx(WS_EX_TOOLWINDOW,0,0, WS_CHILD | WS_VISIBLE, r, GetDesktopWindow(),0);
				m_wndMsg->m_tDestroy = GetTickCount64() + 5000;
				::SetWindowPos(m_wndMsg->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			else 
			if(!m_wndMsg->m_tDestroy || m_wndMsg->GetStrText().IsEmpty() )
			{
				m_wndMsg = 0;
			}
		}
		SetTimer(T_CHECK, 100, 0);
		return;
	case T_CLOSE: {
		KillTimer(nIDEvent);
		PostMessage(WM_CLOSE);
		}
		return;
	case T_MEASURE_SHOW: 
		{
			KillTimer(nIDEvent);
			if (theApp.m_pMList)
			{
				BOOL bSimple = theApp.GetProfileInt(_T("Settings"), _T("bSimple"), FALSE);

				if (!theApp.m_pMList->IsPaneVisible() && !bSimple) {
					theApp.m_pMList->ShowPane(TRUE, TRUE, TRUE);
					//	RecalcLayout();
				}
			}
		}
		return;
	case T_NEW_DOC:
		if (!m_nInit && theApp.m_bReady)
		{
			KillTimer(nIDEvent); 
			for (int core = 0, coreMax = (int)theApp.m_pEngines.size(); core < coreMax; ++core) {
				BOOL bStartOK = theApp.m_bStartOK;
				if (!bStartOK && theApp.m_pEngines[core]->engine.GetVar(VI_INFO_CAMERA_LOCK, 0, VT_INT, 1).intVal)
					bStartOK = TRUE;

				if (theApp.m_cmdl.IsSet(L"/open"))
					theApp.OnOpenDocument(core,theApp.m_cmdl.GetParam(L"/open"));
				else
				if (theApp.m_cmdl.IsSet(L"/measure"))
					theApp.OnOpenDocument(core, theApp.m_cmdl.GetParam(L"/measure"));
				else
					theApp.OnOpenDocument(core, bStartOK ? _T("*") : _T(""));
			}
			theApp.m_pDoc->MakeTitle();

			theApp.RegFlush();
		}
		break;
	case T_MSGBOX: {
			HWND hWnd = ::FindWindow(NULL, m_strTitle);
			if (hWnd) {
				::PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			KillTimer(T_MSGBOX);
		}
		break;
	case T_SHOWLD:
		KillTimer(nIDEvent);
		if (theApp.m_pLD && IsWindow(theApp.m_pLD->m_hWnd))
		{
			BOOL bShow = !!theApp.m_pEngine->engine.GetVar(m_timerData[T_SHOWLD], 0, VT_BOOL);
			ShowPane(theApp.m_pLD, bShow, TRUE, FALSE);
			RecalcLayout();
		}
		break;
	case T_SHOW_MAX:
		KillTimer(nIDEvent);
		ShowWindow(SW_SHOWMAXIMIZED);
		break;
	case T_SHOW_NORM:
		KillTimer(nIDEvent);
		ShowWindow(SW_SHOWNORMAL);
		break;
	case T_MEASURE_TEST_START:
		{
			KillTimer(nIDEvent);
			OnMENUID(ID_ACTION_MEASUREMENT);
			long dt = (long)(theApp.m_pEngine->engine.GetVar( VI_INFO_M_PERIOD, 0, VT_R4).fltVal * 1000);
			SetTimer(T_MEASURE_TEST_START, dt + min(dt,20000), 0);
	}
		break;

	default: break;

	}

	CFrameWndEx::OnTimer(nIDEvent);
}


int CVibraimageExFrame::Message(int idCap, int idMsg, int buttons)
{
	return MessageBox(theApp.m_pEngine->engine.GetLang()->Get(idMsg), theApp.m_pEngine->engine.GetLang()->Get(idCap), buttons);
}

int CVibraimageExFrame::Message(int idCap, int idMsg, const CString &sAdd, int buttons)
{
	CString msg;
	msg.Format(theApp.m_pEngine->engine.GetLang()->Get(idMsg), sAdd);
	return MessageBox(msg, theApp.m_pEngine->engine.GetLang()->Get(idCap), buttons);
}

void CVibraimageExFrame::OnAVIStart()
{
}


void CVibraimageExFrame::OnAVIStop()
{
}


void CVibraimageExFrame::PutMarker(int id)
{
}


void CVibraimageExFrame::OnTimerAVIRestartNow()
{
}



void CVibraimageExFrame::OnInitView(CVibraimageExView* pView, int step)
{

}

void  CVibraimageExFrame::MakeLoadPos() {
	++m_nInit;

	CRect rw, rc;
	GetWindowRect(&rc);
	int x = theApp.GetProfileInt(L"Frame", L"frame_x", rc.left);
	int y = theApp.GetProfileInt(L"Frame", L"frame_y", rc.top);
	int cx = theApp.GetProfileInt(L"Frame", L"frame_cx", 0);
	int cy = theApp.GetProfileInt(L"Frame", L"frame_cy", 0);
	int bMax = theApp.GetProfileInt(L"Frame", L"frame_max", -1);

	if (bMax == -1) {
		--m_nInit;
		return;
	}

	rw.left = rw.right = 0;
	rw.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	rw.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	if (bMax) {
		if (cx && cy && x < rw.right && y < rw.bottom && x + cx >= rw.left && y + cy >= rw.top) {
			MoveWindow(x, y, cx, cy);
		}

		SetTimer(T_SHOW_MAX, 1000,0);
	}
	else {
		if (cx && cy && x < rw.right && y < rw.bottom && x + cx >= rw.left && y + cy >= rw.top) {
			MoveWindow(x, y, cx, cy);
		} 
		SetTimer(T_SHOW_NORM, 1000, 0);
	}


	--m_nInit;
}

void  CVibraimageExFrame::MakeSavePos(bool bSet) {
	if (bSet || (!m_nInit && m_hWnd && !theApp.IsRegLocked() && !theApp.m_bCloseRqst && !theApp.m_bAbort) ) {
		int bMax = GetWindowLong(m_hWnd, GWL_STYLE) & WS_MAXIMIZE;
		theApp.WriteProfileInt(L"Frame", L"frame_max", bMax);
		if (!bMax) {
			CRect rw;
			GetWindowRect(&rw);

			theApp.WriteProfileInt(L"Frame", L"frame_x", rw.left);
			theApp.WriteProfileInt(L"Frame", L"frame_y", rw.top);
			theApp.WriteProfileInt(L"Frame", L"frame_cx", rw.Width());
			theApp.WriteProfileInt(L"Frame", L"frame_cy", rw.Height());
		}
		theApp.RegFlush();
	}
}

void CVibraimageExFrame::OnMove(int x, int y)
{
	__super::OnMove(x, y);

	MakeSavePos();
}


void CVibraimageExFrame::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	MakeSavePos();
}


BOOL CVibraimageExFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	return  __super::OnCreateClient(lpcs, pContext);
}

void CVibraimageExFrame::MakeLoadState() {

	theApp.m_nLoadState++;
	LPBYTE tmp = 0;
	UINT nSize = 0;

	
	pApp->GetProfileBinary(_T("Settings0"), _T("engineState"), &tmp, &nSize);

	if (nSize && tmp) {
		RegDelnode(HKEY_CURRENT_USER, CString(_T("SOFTWARE\\")) + theApp.m_regString + _T("\\VibraimageEx\\Docking" ));
		RegDelnode(HKEY_CURRENT_USER, CString(_T("SOFTWARE\\")) + theApp.m_regString + _T("\\VibraimageEx\\Workspace"));

		theApp.m_pEngine->engineReg.LoadSettings((LPCSTR)& tmp[0],0,L"engine");
		theApp.RegFlush();
	}
	if (tmp)
		delete[] tmp;
	MakeLoadPos();
	theApp.m_nLoadState--;
}

void CVibraimageExFrame::MakeLoadStateInit() {
	if (theApp.m_cmdl.IsSet(L"/load_config")) {
		CString path = theApp.m_cmdl.GetParam(L"/load_config");
		theApp.m_cmdl.Remove(L"/load_config", 2);

		if (theApp.FileExists(path)) {
			BYTE tmp = 0;
			theApp.RegFlush();
			pApp->WriteProfileBinary(_T("Settings0"), _T("engineState"), &tmp, 0);
			theApp.RegFlush();

			theApp.LoadSettings(path, false, false);
		}
	}
	else
		MakeLoadState();
}

bool CVibraimageExFrame::OnInit()
{
	if (!m_nInit)
	{
		KillTimer(T_INIT);
	
		theApp.m_splashScreen.Start();

		MakeLoadStateInit();

		if (theApp.m_pView)
			theApp.m_pView->Align();
		m_wndMenuBar.NewMenuBar();

		SetTimer(T_CHECK, 100, 0);

		if (theApp.m_nStart == 1) {
			pApp->m_pEngine->engine.OnMenu(ID_SETTINGS_DEFAULTS_0 + 99);
			
			CDockablePane* pVI = m_wndPane.Find("ID_INFO_WND_V4");
			if (pVI)
				pVI->ShowPane(TRUE, TRUE, TRUE);
		}

		RecalcLayout();
		
		theApp.m_bReady++;

	
		UnlockWindowUpdate();

		MakeLoadPos();

		theApp.m_pMainWnd->UpdateWindow();

		RedrawWindow();

		CVibraimageExOverview::Create(true);



		theApp.m_pView->OnInit();

		theApp.m_splashScreen.Finish();

		theApp.RegFlush();

		return false;
	}
	m_wndPane.OnInit();

	if (m_nInit < -5) {
		m_nInit = 1;
	}

	if (m_nInit == 1)
	{
		if (!pApp->m_pEngine->engine.GetSeq()->SeqIsOk()) {
		}
		else {
			KillTimer(T_INIT);

			if (!CPwdDlg::Check()) {
				theApp.m_bAbort++;
				PostMessage(WM_ABORT_CLOSE);
			}

			SetTimer(T_INIT, 200, 0);
		}
	}

	--m_nInit;

	return true;
}



void CVibraimageExFrame::FormatMenu(CCmdUI * pCmdUI, const CString& txt, BOOL en, BOOL ch)
{
	if (theApp.m_pQT && theApp.m_pQT->FormatMenu(pCmdUI, txt, en, ch))
		return;
	if (!txt.IsEmpty())
		pCmdUI->SetText(txt);
	pCmdUI->Enable(en);
	pCmdUI->SetCheck(ch);
}

bool CVibraimageExFrame::OnMenuMeasurement(UINT id) {

	if (!pApp->GetProfileInt(L"Settings", L"userInfo", 0))
		return false;

	CPropertiesWndBase* pUserInfo = m_wndPane.Find("ID_INFO_WND_USERINFO");
	if(!pUserInfo)
		return false;
	BOOL ok = pUserInfo->IsPaneVisible();
	if (ok) {
		pUserInfo->ShowPane(FALSE, FALSE, FALSE);
	//	RecalcLayout();
		return false;
	}
	pUserInfo->ShowPane(TRUE, FALSE, TRUE);
//	RecalcLayout();

	return false;
}

bool  CVibraimageExFrame::OnPreMENUID(UINT id) {
	switch (id) {
	case ID_SETTINGS_USER_INFO: {
			BOOL en = pApp->GetProfileInt(L"Settings", L"userInfo", 0);
			pApp->WriteProfileInt(L"Settings", L"userInfo", !en);
			CPropertiesWndBase* pUserInfo = m_wndPane.Find("ID_INFO_WND_USERINFO");
			if(pUserInfo)
				pUserInfo->ShowPane(FALSE, TRUE, FALSE);
			pApp->m_pEngine->engine.PutVar(VI_INFO_USERINFO_ID, 0, COleVariant(_T("")));
			pApp->m_pEngine->engine.PutVar(VI_INFO_USERINFO_NAME, 0, COleVariant(_T("")));
			pApp->m_pEngine->engine.PutVar(VI_INFO_USERINFO_BD, 0, COleVariant(_T("")));
			pApp->m_pEngine->engine.PutVar(VI_INFO_USERINFO_GENDER, 0, COleVariant(_T("")));
			pApp->m_pEngine->engine.PutVar(VI_INFO_USERINFO_AGE, 0, COleVariant(_T("")));
			pApp->m_pEngine->engine.PutVar(VI_INFO_USERINFO_COMMENTS, 0, COleVariant(_T("")));
			RecalcLayout();
		}
		break;
	case ID_ACTION_MEASUREMENT:
	case ID_ACTION_MEASUREMENT1:
	case ID_ACTION_MEASUREMENT2:
		return OnMenuMeasurement(id);
	default: break;
	}
	return false;
}
bool CVibraimageExFrame::OnPreUpdateMENUID(int id, BOOL& bEnabled, BOOL& bChecked, LPWSTR pText) {
	switch (id) {
	case ID_SETTINGS_USER_INFO:
		bChecked = !!pApp->GetProfileInt(L"Settings", L"userInfo", 0);
		bEnabled = TRUE;
		return true;
	default: break;
	}
	return false;
}

void  CVibraimageExFrame::OnMENUID(UINT id)
{
	if(!OnPreMENUID(id))
		pApp->m_pEngine->engine.OnMenu( m_wndMenuBar.MapID( id ) );
}

void  CVibraimageExFrame::OnUpdateMENUID(CCmdUI* pCmdUI)
{
	if (theApp.m_bCloseRqst || !pApp->m_pEngine->engine.IsStarted())
		return;

	int id = m_wndMenuBar.MapID(pCmdUI->m_nID);

	TCHAR txt[256] = _T("");
	BOOL en = FALSE, ch = FALSE;

	if(!OnPreUpdateMENUID(id, en, ch, txt))
		pApp->m_pEngine->engine.GetMenuState(id, en, ch, txt);

	if (en == -1 && pCmdUI->m_pMenu)
		pCmdUI->m_pMenu->RemoveMenu(pCmdUI->m_nIndex, MF_BYPOSITION);
	else
	if( txt[0])
		FormatMenu(pCmdUI, txt, en && theApp.m_bReady, ch & 1);
	else
	{ 
		int lid = m_wndMenuBar.GetTextID(pCmdUI->m_nID, !!(ch & 1));
		if (lid > 0)
		{
			FormatMenu(pCmdUI, theApp.m_pEngine->engine.GetLang()->Get(lid), en && theApp.m_bReady, ch & 1);
		} else
			FormatMenu(pCmdUI, txt, en && theApp.m_bReady, ch & 1);
	}
	
}
// CVibraimageExFrame message handlers


void CVibraimageExFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CVibraimageExFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CVibraimageExFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	//	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}



void CVibraimageExFrame::MakeSaveSettings(CString path, std::vector<char>* pXML, bool bEn) {
	CString base = _T("Docking");
	// CString base = _T("Workspace");
	CString reg = AFXGetRegPath(base);
//	reg.TrimRight(L"\\");

	if (theApp.m_hWndStarter)
		::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_DELAY, 1);

	BOOL ok = GetDockingManager()->SaveState(reg);



	if (IsWindow(m_wndToolBar))
	{
		m_wndToolBar.SaveState(reg);
	}
	if (IsWindow(m_wndToolBarVi))
	{
		m_wndToolBarVi.SaveState(reg);
		m_wndToolBar.SaveParameters(reg);
	}

	theApp.SaveState(base);

	theApp.RegSave(bEn);
	theApp.RegFlush();

	int core0 = theApp.m_pEngine->id;

	std::vector<char> xml(8 * 1024 * 1024);
	ZeroMemory(&xml[0], xml.size());

	if (theApp.m_pEngines.size() == 1) {
		theApp.m_pEngine->engine.GetReg()->SaveSettings(&xml[0], 0);
		//	theApp.m_pEngine->engine.GetReg()->LoadSettings(dlg.GetPathName());
	}
	else {
		pugi::xml_document doc;
		pugi::xml_node root = doc.append_child();
		root.set_name("vibraimage");
		root.append_attribute("cores").set_value((int)theApp.m_pEngines.size());

		for (size_t c = 0; c < theApp.m_pEngines.size(); ++c) {
			if (theApp.m_hWndStarter)
				::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_DELAY, 1);

			std::vector<char> cxml(8 * 1024 * 1024);
			ZeroMemory(&cxml[0], cxml.size());

			theApp.m_pEngines[c]->engine.GetReg()->SaveSettings(&cxml[0], 0);

			pugi::xml_document cdoc;
			if (cdoc.load(&cxml[0])) {
				pugi::xml_node core = root.append_copy(cdoc.first_child());
				core.set_name("core");
				core.append_attribute("id").set_value((int)c);
			}
		}

		std::ostringstream oss;
		doc.print(oss);
		std::string str = xml_document::xml_header(true) + "\r\n" + oss.str();
		xml.resize(str.size() + 1);
		strcpy(&xml[0], str.c_str());
	}

	if (pXML) {
		* pXML = xml;
		pXML->push_back(0);
	}

	if (!path.IsEmpty()) {
		try {
			CFile f;
			if (f.Open(path, CFile::modeCreate | CFile::modeWrite)) {
				f.Write(&xml[0], (UINT)strlen(&xml[0]));
				f.Close();
			}
		}
		catch (CFileException* e) {
			e->ReportError();
		}
	}

	MakeSavePos();

	theApp.RegFlush();

}

void CVibraimageExFrame::OnMenuRef(int id, int step)
{
	if (id >= ID_SETTINGS_DEFAULTS_0 && id <= ID_SETTINGS_DEFAULTS_0 + 99)
	{
		if (step == 1) {
			m_wndPane.UpdateAll();
			theApp.SkipAll(FALSE);
			theApp.m_splashScreen.Finish();
		}
		else {
			theApp.m_splashScreen.Start();
			theApp.SkipAll(TRUE);
		}
	}
#if defined SEQ_MEDIC_C5S // || defined SEQ_MEDIC_MED
	if (id == ID_FILE_AVICAPTURE && step == 0) {
		if(! pApp->m_pEngine->engine.GetVideo()->IsCapturing())
			OnCommandCheckid(0);
	}
#endif

	if (id >= ID_LANGUAGE_0 && id <= ID_LANGUAGE_19)
	{
		if (step >= 1)
		{
			m_wndMenuBar.NewMenuBar();

			m_wndPane.Reload();	

			if (theApp.m_pOverview)
				theApp.m_pOverview->UpdateLang();

			UpdateSysMenu();

			for (size_t i = 0; i < theApp.m_pEngines.size(); ++i) {

				if(pApp->m_pEngine != theApp.m_pEngines[i])
					theApp.m_pEngines[i]->engine.OnMenu(m_wndMenuBar.MapID(id));
			}
		}
		return;
	}

	if (id >= ID_SETTINGS_CORES_0 && id < ID_SETTINGS_CORES_0+100)
	{
		if (step == 1 && theApp.m_pView)
		{
			int n = id - ID_SETTINGS_CORES_0;
			if(n >= 0 && theApp.m_pView->m_wndTabs.m_hWnd && n < theApp.m_pView->m_wndTabs.GetTabsNum())
				theApp.m_pView->m_wndTabs.SetActiveTab(n);
		}
		return;
	}

	if (id >= ID_VIDEOSETTINGS0_DEVICE0 && id <= ID_VIDEOSETTINGS0_DEVICE99 || id == ID_VIDEOSETTINGS0_STOP || id == ID_VIDEOSETTINGS0_AVI || id == ID_VIDEOSETTINGS0_ANYCAM)
	{
		theApp.m_pDoc->MakeTitle(500);
		return;
	}

	if (id == ID_VIEW_RESETVIEW) {
		OnViewResetView();
		return;
	}

	if (id == ID_FILE_SAVESETTINGS)
	{
		if (step == 1)
		{
			BeginWaitCursor();
			CString path = GetDefaultPath() + _T("defaults\\");
			CFileDialog dlg(FALSE, _T("xml"), path);
			if (dlg.DoModal() == IDOK)
			{
				if (theApp.m_hWndStarter)
					::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_DELAY, 1);

				std::vector<char> xmlState;
				theApp.RegFlush();
				MakeSaveSettings(dlg.GetPathName(), &xmlState);
				pApp->WriteProfileBinary(_T("Settings0"), _T("engineState"), (LPBYTE)& xmlState[0], xmlState.size());
				theApp.RegFlush();
			}
			EndWaitCursor();
		}
		return;
	}

	if (id == ID_FILE_LOADSETTINGS)
	{
		if (step == 1)
		{
			CString path = GetDefaultPath() + _T("defaults\\");
			CFileDialog dlg(TRUE, _T("xml"), path);
			if (dlg.DoModal() == IDOK)
			{
				theApp.LoadSettings(dlg.GetPathName(),true,true);
				theApp.RegFlush();
			}
		}
		return;
	}

	if (id == ID_ACTION_MEASUREMENT)
	{
		SetTimer(T_MEASURE_SHOW, 250, 0);

		return;
	}

}

void CVibraimageExFrame::OnSettingsLoad(LPCSTR /* szXmlStr */)
{
	theApp.m_nLoadState++;

	CString base = _T("Docking");
//	CString base = _T("Workspace");
	CString reg = AFXGetRegPath(base);
//	reg.TrimRight(L"\\");

	if (theApp.m_hWndStarter)
		::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_DELAY, 1);

	BOOL ok = GetDockingManager()->LoadState( reg );

	if (IsWindow(m_wndToolBar))
		m_wndToolBar.LoadState(reg);
	if (IsWindow(m_wndToolBarVi))
		m_wndToolBarVi.LoadState(reg);
	theApp.LoadState(base);

	GetDockingManager()->SetDockState();

	
	m_wndMenuBar.NewMenuBar();
	m_wndPane.AdjustVisible();
	RecalcLayout();

	theApp.m_pView->Align();

	if (theApp.m_pOverview)
		theApp.m_pOverview->Align();

	theApp.RegFlush();
	theApp.m_nLoadState--;
}

void  CVibraimageExFrame::OnNewVar(int id, int subID)
{
	switch (id)
	{
	case VI_INFO_WND_HIST:
		if (theApp.m_pHist && IsWindow(theApp.m_pHist->m_hWnd))
		{
			BOOL bShow = !!theApp.m_pEngine->engine.GetVar(id, 0, VT_BOOL);
			ShowPane(theApp.m_pHist, bShow, TRUE, FALSE);
			RecalcLayout();
		}
		break;
	case VI_INFO_WND_GRAPHS:
		if (theApp.m_pGraph && IsWindow(theApp.m_pGraph->m_hWnd))
		{
			BOOL bShow = !!theApp.m_pEngine->engine.GetVar(id, 0, VT_BOOL);
			ShowPane(theApp.m_pGraph, bShow, TRUE, FALSE);
			RecalcLayout();
		}
		break;
	case VI_INFO_WND_LDW:
		KillTimer(T_SHOWLD);
		m_timerData[T_SHOWLD] = id;

		SetTimer(T_SHOWLD, 500, 0);
		break;
	case VI_INFO_MENU:
		OnMenuRef(subID&0xFFFF, (subID >> 16) & 0xFFFF);
		break;
	case ID_VI_EVENT_UPDATE_MENU_VIDEO:
	case ID_VI_EVENT_UPDATE_MENU_AUDIO:
	case ID_VI_EVENT_UPDATE_MENU:
		m_bNewMenu++;
		break;
	case ID_VI_EVENT_UPDATE_DB:
		m_bNewDB++;
		break;

	case VI_INFO_CLOSE_RQST:
		// theApp.m_bCloseRqst++;
		pApp->m_pEngine->engineInterface.OnCloseRequest();
		break;
#ifdef SEQ_MD
	case VI_VAR_SEQ_HTTP_STATUS:
		{
			CString str = pApp->m_pEngine->engine.GetVar(id, subID, VT_BSTR).bstrVal;
			if (!m_wndMsg && !str.IsEmpty()) {
				m_wndMsg = std::make_shared<CColorProcress>();
				int pos = str.Find(L"data=");
				if (pos > 0)
					str = str.Left(pos);
				str.Trim(L" \r\n\t");
				str.Replace(L"\t", L" ");
				m_wndMsg->SetStrText(str);
			}
		}
#endif
		break;
	default: break;
	}

	
}


void CVibraimageExFrame::ShowPane(CBasePane* pBar, BOOL bShow, BOOL bDelay, BOOL bActivate)
{
	if (!theApp.m_pView && theApp.m_pView->ShowTab(pBar, bShow))
		CFrameWndEx::ShowPane(pBar, bShow, bDelay, bActivate);
}

void CVibraimageExFrame::OnSysCommand(UINT nID, LPARAM lParam) {
	if (nID == ID_VIEW_RESETVIEW) {
		OnViewResetView();
		return;
	}
	__super::OnSysCommand(nID, lParam);
}

void CVibraimageExFrame::UpdateSysMenu() {
	
	if (theApp.m_bCloseRqst)
		return;

	CMenu* pSys = GetSystemMenu(false);
	if (!pSys)
		return;

	int id = ID_VIEW_RESETVIEW;
	MENUITEMINFO info;
	ZeroMemory(&info, sizeof(info));
	info.cbSize = sizeof(info);

	CString txt = theApp.m_pEngine->engine.GetLang()->Get(L"BUTTON_RESET");

	if (!pSys->GetMenuItemInfo(id, &info, false))
	{
		pSys->AppendMenu(MF_SEPARATOR);
		pSys->AppendMenu(MF_STRING, id, txt);
	}
	else
		pSys->ModifyMenu(id, MF_BYCOMMAND, id, txt);

}

void CVibraimageExFrame::OnHelpAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


void CVibraimageExFrame::OnUpdateHelpAbout(CCmdUI *pCmdUI)
{
}

void CVibraimageExFrame::OnHelpHelp()
{
	CString path = GetDefaultPath() + L"manual.pdf";

	ShellExecute(m_hWnd,	_T("open"),	path,	NULL,	NULL, SW_MAXIMIZE);
}


void CVibraimageExFrame::OnUpdateHelpHelp(CCmdUI* pCmdUI)
{
	CString path = GetDefaultPath() + L"manual.pdf";
	CFileStatus fs;
	if(!CFile::GetStatus(path,fs))
		pCmdUI->m_pMenu->RemoveMenu(pCmdUI->m_nIndex, MF_BYPOSITION);

}

void CVibraimageExFrame::UpdateTitle()
{
	m_strTitle = theApp.m_pEngine->engine.GetLang()->Get(L"STR_FRAME_CAPTION");
	OnUpdateFrameTitle(TRUE);
}


void CVibraimageExFrame::OnViewResetView() {
	theApp.CleanSlateVI();
}

void CVibraimageExFrame::OnUpdateResetView(CCmdUI* pCmdUI) {
	pCmdUI->SetText(theApp.m_pEngine->engine.GetLang()->Get(L"BUTTON_RESET"));
}

void CVibraimageExFrame::OnSettingsLock() {
	if (theApp.IsRegLocked()) {

		if (CPwdDlg::Check(VI_INFO_LOCK_PWD_OK))
			theApp.SetRegLocked(false);
	}
	else
	{
		theApp.SetRegLocked(true);
		theApp.m_pEngine->engine.PutVar(VI_INFO_LOCK_PWD1, 0, COleVariant(L""));
	}
}

void CVibraimageExFrame::OnUpdateSettingsLock(CCmdUI* pCmdUI) {
	pCmdUI->SetText(theApp.m_pEngine->engine.GetLang()->Get(L"MENU_SETTINGS_LOCK"));
	pCmdUI->SetCheck(theApp.IsRegLocked() ? 1:0);
}


void CVibraimageExFrame::OnViewSimple()
{
	BOOL bSimple = !theApp.GetProfileInt(_T("Settings"), _T("bSimple"), FALSE);
	theApp.WriteProfileInt(_T("Settings"),_T("bSimple"), bSimple);

	m_wndPane.ShowAll(!bSimple);

	if(IsWindow(m_wndToolBar))
		m_wndToolBar.Load(GetToolbarPath());
	
	RecalcLayout();
}


void CVibraimageExFrame::OnUpdateViewSimple(CCmdUI *pCmdUI)
{
	BOOL bSimple = theApp.GetProfileInt(_T("Settings"), _T("bSimple"), FALSE);
	pCmdUI->SetCheck(bSimple);
}

CString CVibraimageExFrame::GetToolbarPath()
{
	auto cfg = theApp.m_pEngine->engine.GetConfigBase();
	if (!cfg)
		return L"";

	BOOL bSimple = theApp.GetProfileInt(_T("Settings"), _T("bSimple"), FALSE);
	if(!bSimple)
		return GetDefaultPath() + cfg + _T("\\toolbar.xml");

	return GetDefaultPath() + cfg + _T("\\toolbar_simple.xml");
}



void CVibraimageExFrame::OnCommandCheckid(BOOL *pResult)
{
	int iid = theApp.m_pEngine->engineDB.GetCurrentItem();

	CUserIdDlg dlg;
	dlg.m_id = (iid >= 0) ? pApp->m_pEngine->engine.GetVar(VI_INFO_USER_DB, iid, VT_BSTR, 1).bstrVal : L"";
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_pEngine->engineDB.NewItem(dlg.m_id);
		if (pResult) *pResult = TRUE;
	} else
		if (pResult) *pResult = FALSE;

}


BOOL CVibraimageExFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{

	if (wParam == ID_COMMAND_CHECKID)
	{
		OnCommandCheckid((BOOL *)lParam);
		return TRUE;
	}
	return __super::OnCommand(wParam, lParam);
}


LRESULT CVibraimageExFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

	if (message == WM_ABORT_CLOSE) {
		if (theApp.m_hWndStarter)
			::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_STOP, 1);

		if (!theApp.m_bCloseRqst) {
			theApp.m_bAbort++;
			return SendMessage(WM_CLOSE);
		}
	} else
	if (message == WM_ENDSESSION) {
		if(lParam & (ENDSESSION_CLOSEAPP| ENDSESSION_CRITICAL)) {
			MessageBeep(MB_ICONSTOP);
			theApp.WriteProfileInt(_T("Settings"), _T("bStartOK"), 2);
			SendMessage(WM_ABORT_CLOSE);
			return 0;
		}
	}
	else
	if (message == WM_QUERYENDSESSION) {
		if (lParam & (ENDSESSION_CLOSEAPP | ENDSESSION_CRITICAL)) {
			MessageBeep(MB_ICONSTOP);
			theApp.WriteProfileInt(_T("Settings"), _T("bStartOK"), 2);
			SendMessage(WM_ABORT_CLOSE);
			return theApp.m_bDone ? 1:0;
		}
	}
	
	return __super::WindowProc(message, wParam, lParam);
}


void CVibraimageExFrame::OnViewOverview()
{
	if (theApp.m_pOverview)
		theApp.m_pOverview->DestroyWindow();
	else {
		CVibraimageExOverview::Create();
	}

}


void CVibraimageExFrame::OnUpdateViewOverview(CCmdUI *pCmdUI)
{
	if (theApp.m_pEngines.size() <= 1)
		pCmdUI->m_pMenu->RemoveMenu(pCmdUI->m_nIndex, MF_BYPOSITION);
}


void CVibraimageExFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
	
	MakeSavePos();
}


BOOL CVibraimageExFrame::OnEraseBkgnd(CDC* pDC)
{

	return __super::OnEraseBkgnd(pDC);
}




