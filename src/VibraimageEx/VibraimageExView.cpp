
// VibraimageExView.cpp : implementation of the CVibraimageExView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "VibraimageEx.h"
#endif

#include "VibraimageExFrame.h"
#include "VibraimageExDoc.h"
#include "VibraimageExView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVibraimageExView

IMPLEMENT_DYNCREATE(CVibraimageExView, CView)

BEGIN_MESSAGE_MAP(CVibraimageExView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CVibraimageExView::OnFilePrintPreview)
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGING_ACTIVE_TAB, &CVibraimageExView::OnAfxWmChangingActiveTab)
	ON_REGISTERED_MESSAGE(AFX_WM_ON_RENAME_TAB, &CVibraimageExView::OnAfxWmRenameTab)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CVibraimageExView construction/destruction

CVibraimageExView::CVibraimageExView()
{
	theApp.m_pView = this;

}

CVibraimageExView::~CVibraimageExView()
{
	theApp.m_pView = 0;
}

BOOL CVibraimageExView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CVibraimageExView drawing

void CVibraimageExView::OnDraw(CDC* /*pDC*/)
{
	CVibraimageExDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CVibraimageExView printing


void CVibraimageExView::OnFilePrintPreview()
{
//#ifndef SHARED_HANDLERS
//	AFXPrintPreview(this);
//#endif
}

BOOL CVibraimageExView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVibraimageExView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CVibraimageExView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CVibraimageExView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
//	OnContextMenu(this, point);
}

void CVibraimageExView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{

	int id = theApp.m_pEngine->engineInterface.FindResult(point);

	CViEngineExLangInterface *pLang = theApp.m_pEngine->engine.GetLang();
	CMenu menu;
	menu.CreatePopupMenu();

	switch (id)
	{
	case VI_RESULT_VI0_A:
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI0A_SHOW, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI0A_AURA, pLang->Get(L"MENU_VIEW_MODE_AURA"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI0A_WBG, pLang->Get(L"MENU_VIEW_MODE_WHITEBG"));
		break;
	case VI_RESULT_VI1_A:
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI1A_SHOW, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI1A_AURA, pLang->Get(L"MENU_VIEW_MODE_AURA"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI1A_WBG, pLang->Get(L"MENU_VIEW_MODE_WHITEBG"));
		break;
	case VI_RESULT_VI2_A:
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI2A_SHOW, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI2A_AURA, pLang->Get(L"MENU_VIEW_MODE_AURA"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI2A_WBG, pLang->Get(L"MENU_VIEW_MODE_WHITEBG"));
		break;
	case VI_RESULT_DELTA_A:
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_A_SHOW, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_A_AURA, pLang->Get(L"MENU_VIEW_MODE_AURA"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_A_WBG, pLang->Get(L"MENU_VIEW_MODE_WHITEBG"));
		break;
	case VI_RESULT_DELTA_FA:
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_FA_SHOW, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_FA_AURA, pLang->Get(L"MENU_VIEW_MODE_AURA"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_FA_WBG, pLang->Get(L"MENU_VIEW_MODE_WHITEBG"));
		break;

	case VI_RESULT_VI0_B:
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI0B_SHOW, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI0B_AURA, pLang->Get(L"MENU_VIEW_MODE_AURA"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI0B_WBG, pLang->Get(L"MENU_VIEW_MODE_WHITEBG"));
		break;
	case VI_RESULT_VI1_B:
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI1B_SHOW, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI1B_AURA, pLang->Get(L"MENU_VIEW_MODE_AURA"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI1B_WBG, pLang->Get(L"MENU_VIEW_MODE_WHITEBG"));
		break;
	case VI_RESULT_VI2_B:
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI2B_SHOW, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI2B_AURA, pLang->Get(L"MENU_VIEW_MODE_AURA"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEVI2B_WBG, pLang->Get(L"MENU_VIEW_MODE_WHITEBG"));
		break;
	case VI_RESULT_DELTA_B:
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_B_SHOW, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_B_AURA, pLang->Get(L"MENU_VIEW_MODE_AURA"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_B_WBG, pLang->Get(L"MENU_VIEW_MODE_WHITEBG"));
		break;
	case VI_RESULT_DELTA_FB:
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_FB_SHOW, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_FB_AURA, pLang->Get(L"MENU_VIEW_MODE_AURA"));
		menu.AppendMenu(MF_STRING, ID_VIEWMODEDELTA_FB_WBG, pLang->Get(L"MENU_VIEW_MODE_WHITEBG"));
		break;

	case VI_RESULT_SRC_A:
		menu.AppendMenu(MF_STRING, ID_VIEWMODESRC_AURAA, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		break;
	case VI_RESULT_SRC_B:
		menu.AppendMenu(MF_STRING, ID_VIEWMODESRC_AURAB, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		break;
	case VI_RESULT_SRC_0:
		menu.AppendMenu(MF_STRING, ID_VIEWMODESRC_AURA0, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		break;
	case VI_RESULT_FFT_0:
		menu.AppendMenu(MF_STRING, ID_VIEWMODEFFT_AURA0, pLang->Get(L"MENU_VIEW_MODE_SHOW"));
		break;
	default: return;
	}

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, theApp.m_pFrame, 0);
/*
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
*/
}


// CVibraimageExView diagnostics

#ifdef _DEBUG
void CVibraimageExView::AssertValid() const
{
	CView::AssertValid();
}

void CVibraimageExView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVibraimageExDoc* CVibraimageExView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVibraimageExDoc)));
	return (CVibraimageExDoc*)m_pDocument;
}
#endif //_DEBUG


// CVibraimageExView message handlers


void CVibraimageExView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	if (!m_wndTabs.m_hWnd)
	{
		AddTab(0, L"VI", -1, FALSE);
	}

	m_wndTabs.SetActiveTab(0);

	Align();
}

void CVibraimageExView::OnInit() {
	for(int i = 2; i >= 0; --i)
		m_wndTabs.SetActiveTab(i);
}

CString CVibraimageExView::GetTabName(int core) {
	if (!m_wndTabs.m_hWnd || core < 0 || core >= (int)theApp.m_pEngines.size())
		return _T("");
	CString label;
	m_wndTabs.GetTabLabel(core, label);
	return label;
}

void CVibraimageExView::Align()
{
	CRect rc;
	GetClientRect(&rc);
	if (rc.Width() < 16) rc.right = rc.left + 16;
	if (rc.Height() < 16) rc.bottom = rc.top + 16;

	if (m_wndTabs.m_hWnd )
	{
		m_wndTabs.MoveWindow(&rc);

		for (int core = 0, coreMax = (int)theApp.m_pEngines.size(); core < coreMax; ++core) {
			CString name;
			if (theApp.m_pEngines.size() == 1)
				name = _T("VI");
			else
				name.Format(_T("VI %d"), core + 1);

			CString tabName;
			tabName.Format(_T("tabName%d"), core);

			name = theApp.GetProfileString(_T("Settings"), tabName, name);

			m_wndTabs.SetTabLabel(core, name);
		}
	}

	
}


void CVibraimageExView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	Align();
}


void CVibraimageExView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	CRect r;
	int id = theApp.m_pEngine->engineInterface.FindResult(point,&r);

	if (id == VI_RESULT_THERMAL) {
		int rw = r.Width();
		int rh = r.Height();
		int x = point.x - r.left;
		int y = point.y - r.top;

		theApp.m_pEngine->engine.PutVar(VI_VAR_THERMAL_POS, 0, COleVariant((float)x/(float)rw),1);
		theApp.m_pEngine->engine.PutVar(VI_VAR_THERMAL_POS, 0, COleVariant((float)(rh-1-y) / (float)rh), 2);
	}

	__super::OnLButtonDblClk(nFlags, point);
}


void CVibraimageExView::OnLButtonDown(UINT nFlags, CPoint point)
{
	theApp.m_pFrame->SendMessage(WM_COMMAND, ID_ACTION_RESETVI);

	__super::OnLButtonDown(nFlags, point);
}


void CVibraimageExView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	__super::OnLButtonUp(nFlags, point);
}


int CVibraimageExView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CVibraimageExView::AddTab(CWnd* pTabWnd, LPCWSTR lpszTabLabel, UINT uiImageId, BOOL bDetachable)
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndTabs.m_hWnd)
	{
		if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this,1))
		{
			TRACE0("Failed to create output tab window\n");
			return;      // fail to create
		}
		m_wndTabs.HideSingleTab(TRUE);
		m_wndTabs.EnableInPlaceEdit(TRUE);

		for (int core = 0, coreMax = (int)theApp.m_pEngines.size(); core < coreMax; ++core) {
			HWND hWndEn = theApp.m_pEngines[core]->engine.GetWindow();
			if (hWndEn)
			{
				CWnd *wndVI;

	
				wndVI = CWnd::FromHandlePermanent(hWndEn);
				wndVI->ModifyStyle(0, LBS_NOINTEGRALHEIGHT);
				wndVI->SetParent(&m_wndTabs);
				wndVI->ShowWindow(SW_SHOW);
				CString name;
				if (theApp.m_pEngines.size() == 1)
					name = _T("VI");
				else
					name.Format( _T("VI %d"),core+1);

				CString tabName;
				tabName.Format(_T("tabName%d"), core);

				name = theApp.GetProfileString(_T("Settings"), tabName, name);

				m_wndTabs.AddTab(wndVI, name, -1, FALSE);
			}
		}
	}

	if (pTabWnd)
	{
		pTabWnd->ModifyStyle(0, LBS_NOINTEGRALHEIGHT);
		pTabWnd->SetParent(&m_wndTabs);

		if(lpszTabLabel)
			m_wndTabs.AddTab(pTabWnd, lpszTabLabel, -1, bDetachable);
	}

	

	Align();
}

void CVibraimageExView::RemoveTab(CWnd* pTabWnd)
{
	int id = m_wndTabs.GetTabFromHwnd(*pTabWnd);
	if (id >= 0 && id < m_wndTabs.GetTabsNum())
		m_wndTabs.RemoveTab(id);
}

LRESULT CVibraimageExView::OnAfxWmRenameTab(WPARAM wParam, LPARAM lParam) {
	CMFCTabCtrl* pTab = (CMFCTabCtrl*)lParam;

	int id = (int)wParam;
	if (id >= 0 && id < (int)theApp.m_pEngines.size()) {
		CString tabName,name = (LPCTSTR)lParam;
		tabName.Format(_T("tabName%d"), id);
		theApp.WriteProfileString(_T("Settings"), tabName, name);
	}
	return 0;
}

LRESULT CVibraimageExView::OnAfxWmChangingActiveTab(WPARAM wParam, LPARAM lParam)
{
	CMFCTabCtrl* pTab = (CMFCTabCtrl*)lParam;

	int id = (int)wParam;
	if (id >= 0 && id < (int)theApp.m_pEngines.size())
		theApp.SelectCore(id,true);

	Align();

	theApp.m_pDoc->MakeTitle();
	return 0;
}

bool CVibraimageExView::ShowTab(CBasePane* pBar, BOOL bShow)
{
	int id = m_wndTabs.GetTabFromHwnd(*pBar);
	if (id >= 0 && id < m_wndTabs.GetTabsNum())
	{
		if (bShow)
		{
			m_wndTabs.SetActiveTab(id);
		}
		else
		{
			int nCur = m_wndTabs.GetActiveTab();
			if(nCur == id)
				m_wndTabs.SetActiveTab(0);
		}
		Align();
		return true;
	} 
	Align();
	return false;
}

BOOL CVibraimageExView::TAB::OnCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL ret = CMFCTabCtrl::OnCommand(wParam, lParam);

	if (lParam == 0) 
		return ret;

	int scroll = 0;
	if ((HWND)lParam == m_btnScrollLeft.GetSafeHwnd())
	{
		scroll = -1;
	}
	else if ((HWND)lParam == m_btnScrollRight.GetSafeHwnd())
	{
		scroll = 1;
	}
	else if ((HWND)lParam == m_btnScrollFirst.GetSafeHwnd())
	{
		scroll = -GetTabsNum();
	}
	else if ((HWND)lParam == m_btnScrollLast.GetSafeHwnd())
	{
		scroll = GetTabsNum();
	}

	if (scroll)
	{
		int nTabs = GetTabsNum();
		int nCur = GetActiveTab();
		int nSel = nCur + scroll;
		if (nSel < 0) nSel = 0;
		if (nSel >= nTabs) nSel = nTabs - 1;

		if (nCur != nSel)
			SetActiveTab(nSel);
	}

	return ret;
}

void CVibraimageExView::OnCloseFrame()
{
	if (!theApp.m_bDone && m_wndTabs.m_hWnd && m_wndTabs.GetVisibleTabsNum() > 0)
	{
		m_wndTabs.RemoveAllTabs();
	}
}

