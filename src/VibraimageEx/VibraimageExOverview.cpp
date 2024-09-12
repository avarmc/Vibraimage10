// VibraimageExOverview.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExOverview.h"
#include "VibraimageExView.h"
#include "avxmem.h"

// CVibraimageExOverview

CString CVibraimageExOverview::m_regGroup = _T("Overview");

IMPLEMENT_DYNCREATE(CVibraimageExOverview, CFrameWndEx)

CVibraimageExOverview::CVibraimageExOverview()
{
	theApp.m_pOverview = this;
	m_viewsHash = -1;
}

CVibraimageExOverview::~CVibraimageExOverview()
{
	theApp.m_pOverview = NULL;
}


BEGIN_MESSAGE_MAP(CVibraimageExOverview, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CVibraimageExOverview message handlers


void  CVibraimageExOverview::Create(bool bInit) {
	if (bInit && !theApp.GetProfileInt(m_regGroup, _T("show"), 0))
		return;

	CWnd *pWnd = new CVibraimageExOverview();
	pWnd->CreateEx(WS_EX_APPWINDOW,0, 0,  WS_VISIBLE|WS_OVERLAPPEDWINDOW, CRect(0, 0, 10, 10), GetDesktopWindow(), 0, 0);
}


int CVibraimageExOverview::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon, FALSE);

	UpdateLang();

	theApp.WriteProfileInt(m_regGroup, _T("show"), 1);

	
	SetTimer(1, 10, 0);

	return 0;
}


void CVibraimageExOverview::OnNewVar(int id, int subID, int core) {
	__super::OnNewVar(id, subID, core);

	int mask = 0;
	if (id == VI_VAR_NFRAME || id == VI_VAR_NFRAME_OUTF) {
		for (size_t i = 0, n = m_views.size(); i < n; ++i)
			if (m_views[i]->core == core)
				m_views[i]->nRedraw++;
	} else
	if (id == VI_VAR_STATE_VAR) {
		for (size_t i = 0, n = m_views.size(); i < n; ++i)
			if (m_views[i]->core == core)
				m_views[i]->UpdateState();
	}
	
}

BOOL CVibraimageExOverview::PreCreateWindow(CREATESTRUCT& cs)
{
	
	
	return __super::PreCreateWindow(cs);
}

void CVibraimageExOverview::UpdateLang() {
	SetWindowText(theApp.m_pEngine->engine.GetLang()->Get(_T("STR_FRAME_CAPTION"),0));
}

void CVibraimageExOverview::OnDestroy()
{
	KillTimer(1);

	Clear();

	__super::OnDestroy();

	delete(this);
}


void CVibraimageExOverview::OnClose()
{
	theApp.WriteProfileInt(m_regGroup, _T("show"), 0);

	__super::OnClose();
}



void CVibraimageExOverview::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if (bShow && !IsWindowVisible())
	{
		WINDOWPLACEMENT *lwp;
		UINT nl;

		if (AfxGetApp()->GetProfileBinary(m_regGroup, _T("WP"), (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete[] lwp;
		}
	}
}


BOOL CVibraimageExOverview::DestroyWindow()
{
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileBinary(m_regGroup, _T("WP"), (LPBYTE)&wp, sizeof(wp));

	return __super::DestroyWindow();
}

void CVibraimageExOverview::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (m_hWnd)
		Align();
}

void CVibraimageExOverview::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
		Check();

	__super::OnTimer(nIDEvent);
}


void	CVibraimageExOverview::Check() {
	if (!m_hWnd)
		return;
	ULONGLONG hash = -1;
	int cnt = (int)GetViewCount(false,&hash);
	if (m_viewsHash != hash || cnt != m_views.size()) {
		m_viewsHash = hash;
		Remake();
	}

	for (size_t i = 0, n = m_views.size(); i < n; ++i)
		if (m_views[i]->nRedraw || m_views[i]->nFail) {
			m_views[i]->UpdateImg();
		}

	m_history.Check();
}

void	CVibraimageExOverview::Remake() {
	GetViewCount(true);
	Align();
}

void	CVibraimageExOverview::Clear() {
	for (size_t i = 0, n = m_views.size(); i < n; ++i)
		SAFE_DELETE(m_views[i]);

	m_views.clear();
}

int		CVibraimageExOverview::GetViewCount(bool bInit, ULONGLONG* pHash) {
	int cnt = 0;
	int nc = (int)theApp.m_pEngines.size();
	ULONGLONG hash = 0;

	if (bInit) {
		Clear();
		m_views.reserve(nc * 16);
	}

	for (int core = 0; core < nc; ++core) {
		if (!theApp.m_pEngines[core] || !theApp.m_pEngines[core]->started)
			continue;

		int mode = theApp.m_pEngines[core]->engine.GetVar(VI_MODE_RESULT, 0, VT_INT).intVal;
		hash += (((ULONGLONG)mode) << 32) + core;

		for (int i = 0; i < 32; ++i) {
			int idr = (1 << i);

			if ((mode & idr) != 0) {
				if (bInit)
					m_views.push_back(new View(core, idr));

				
				++cnt;
			}
		}
	}

	if (pHash)
		*pHash = hash;
	return cnt;
}

CSize CVibraimageExOverview::GetSplit(CRect rc) {
	int cnt = (int)m_views.size();
	CSize sBest(0, 0);
	int wBest = 0;


	for (int cx = cnt; cx >= 1; --cx) {
		int cy = (cnt + cx - 1) / cx;

		int w = rc.Width() / cx;
		int h = rc.Height() / cy;

		if (w * 6 > h * 7)
			w = h * 7 / 6;

		if (w > wBest) {
			wBest = w;
			sBest = CSize(cx, cy);
		}
	}
	
	return sBest;
}

void	CVibraimageExOverview::Align() {
	int cnt = (int)m_views.size();
	if (!cnt)
		return;
	CRect rc,rHist;
	GetClientRect(&rc);

	rHist = rc;
	rc.right -= rc.Width() / 5;
	rHist.left = rc.right + 5;

	CSize sw = GetSplit(rc);
	if (!sw.cx)
		return;

	CSize s1(rc.Width() / sw.cx, rc.Height() / sw.cy);

	for (int i = 0; i < cnt; ++i) {
		int x = i % sw.cx;
		int y = i / sw.cx;
		CRect r(CPoint(x*s1.cx, y*s1.cy), s1);

		if (!m_views[i]->m_hWnd) {
			m_views[i]->Create(0, 0, WS_CHILD | WS_VISIBLE, r, this, 0, 0);
		}
		else
			m_views[i]->MoveWindow(&r);

		m_views[i]->nFail++;

		m_views[i]->SetWindowText(theApp.m_pView->GetTabName(i));
	}

	if(!m_history.m_hWnd)
		m_history.Create(0, 0, WS_CHILD | WS_VISIBLE, rHist, this, 0, 0);
	else
		m_history.MoveWindow(&rHist);

	m_history.RedrawWindow();
	RedrawWindow();
}



CVibraimageExOverview::View::View(int _core, int _idr): core(_core),idr(_idr), nRedraw(0), nFail(1)
{
	memset(&pbi, 0, sizeof(pbi));
}

CVibraimageExOverview::View::~View()
{
}


BEGIN_MESSAGE_MAP(CVibraimageExOverview::View, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()


void CVibraimageExOverview::View::OnPaint()
{
	CPaintDC dc(this); 

	CRect rc;
	GetClientRect(&rc);

	if (Check()) {
		dc.FillSolidRect(&rc, 0xFFFFFF);
		++nFail;
	}

	int d = min(rc.Width() / 30, rc.Height() / 30);
	CRect rg(rc.left + d, rc.top + d, rc.right - d, rc.bottom - d);

	Draw(&dc, rg);

	nRedraw = 0;
}


void CVibraimageExOverview::View::Draw(CDC *pDC, CRect& rg) {
	CRect ri(rg),rs(rg);

	ri.bottom = rg.bottom - rg.Height() / 8;
	rs.top = ri.bottom + 5;

	rImg = ri;

	DrawImg(pDC,ri);

	DrawState(pDC,rs,ri.Width());
}

void CVibraimageExOverview::View::DrawImg(CDC *pDC, CRect& ri) {
	pDC->SetStretchBltMode(COLORONCOLOR);

	bool ok = false;

#if defined VIENGINEEX_GET_ENGINE
	if (theApp.m_pEngines[core] && theApp.m_pEngines[core]->started) {
		CVIENGINE_CLASS * pe = theApp.m_pEngines[core]->engine.GetEngine();
		if (pe) {
			void *pi = 0;
			int iw = 0, ih = 0;
			if (pe->GetResultPtr(idr, &pi, &iw, &ih) && pi && iw && ih) {
				int nw, nh;

				if (nFail)
					pDC->FillSolidRect(&ri, 0xFFFFFF);

				if (ri.Width() * ih > ri.Height() * iw)
					nh = ri.Height(), nw = ri.Height()*iw / ih;
				else
					nw = ri.Width(), nh = ri.Width()*ih / iw;

				ri.left += (ri.Width() - nw) / 2;
				ri.top += (ri.Height() - nh) / 2;

				ri.right = ri.left + nw;
				ri.bottom = ri.top + nw;

				pbi.bmiHeader.biBitCount = 32;
				pbi.bmiHeader.biPlanes = 1;
				pbi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				pbi.bmiHeader.biWidth = iw;
				pbi.bmiHeader.biHeight = ih;



				void *lock = pe->LockResult(true, 0);
				::StretchDIBits(*pDC, ri.left, ri.top, nw, nh, 0, 0, iw, ih, pi, &pbi, DIB_RGB_COLORS, SRCCOPY);
				pe->LockResult(false, lock);
				ok = true;

				
				nFail = 0;
			}
		}
	}
#endif
	if (!ok) {
		pDC->FillSolidRect(&ri, RGB(0, 0, 224));
		++nFail;
	}
}

void CVibraimageExOverview::View::DrawState(CDC *pDC, CRect& rs,  int rw) {
	CRect r,ra(rs);
	wndProgress.GetWindowRect(&r);
	ScreenToClient(&r);
	
	int dx = (ra.Width() - rw) / 2;
	ra.left += dx;
	ra.right = ra.left + rw;
	if (r != ra) {
		wndProgress.MoveWindow(&ra);
		++nFail;
	}
}

bool CVibraimageExOverview::View::Check() {
	if (theApp.m_pEngines[core] && theApp.m_pEngines[core]->started)
	{
		int w = theApp.m_pEngines[core]->engine.GetVar(VI_VAR_SIZE, 0, VT_I4, 1).intVal;
		int h = theApp.m_pEngines[core]->engine.GetVar(VI_VAR_SIZE, 0, VT_I4, 2).intVal;
		if (w == pbi.bmiHeader.biWidth && h == pbi.bmiHeader.biHeight)
			return false;
	}

	if (!pbi.bmiHeader.biWidth)
		return false;

	memset(&pbi, 0, sizeof(pbi));
	return true;
}


void CVibraimageExOverview::View::SetWindowText(LPCTSTR lpszString) {
	__super::SetWindowText(lpszString);
	wndProgress.SetStrTop(lpszString);
}

int CVibraimageExOverview::View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc(0,0,1,1);

	wndProgress.Create(_T(""), WS_CHILD | WS_VISIBLE, rc, this, 0);
	wndProgress.SetRange(0, 100);
	wndProgress.m_vFmt = _T("%0.0f %%");
	wndProgress.m_c1 = RGB(224, 0, 0);
	wndProgress.SetStrText(_T(""));
	wndProgress.m_bAbsColor = true;
	wndProgress.m_font2Scale = 4;

	return 0;
}

void CVibraimageExOverview::View::UpdateState()
{
	float v = theApp.m_pEngines[core]->engine.GetVar(VI_VAR_STATE_VAR, 0, VT_R4).fltVal * 100;
	float l = theApp.m_pEngines[core]->engine.GetVar(VI_VAR_STATE_CRITICAL, 0, VT_R4).fltVal * 100;

	if (v >= l)
		wndProgress.m_c1 = RGB(224, 0, 0);
	else
	if (v >= l / 2)
		wndProgress.m_c1 = RGB(224, 224, 0);
	else
		wndProgress.m_c1 = RGB(0, 224, 0);

	TCHAR strText[32];
	_stprintf_s(strText, 32, _T("%i%%"), (int)v);
	wndProgress.SetStrText(strText);
	wndProgress.SetPos(v);
}


void CVibraimageExOverview::View::UpdateImg() {
	if (!m_hWnd)
		return;

	if (nFail || rImg.IsRectEmpty())
		RedrawWindow();
	else
		RedrawWindow(&rImg,0,RDW_INVALIDATE|RDW_UPDATENOW);
}




CVibraimageExOverview::History::History()
{
}

CVibraimageExOverview::History::~History()
{
}


BEGIN_MESSAGE_MAP(CVibraimageExOverview::History, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CVibraimageExOverview::History::OnPaint()
{
	CPaintDC dc(this); 

	CRect rc;
	GetClientRect(&rc);

	Draw(&dc, rc);
}


void CVibraimageExOverview::History::Draw(CDC *pDC, CRect& rg) {
	std::map<ULONGLONG, HBITMAP> ref;
	
	pDC->SetStretchBltMode(COLORONCOLOR);

	GetHash(&ref);

	int y = 0, rh = rg.Height(), rw = rg.Width();

	for (std::map<ULONGLONG, HBITMAP>::iterator i = ref.begin(), ie = ref.end(); i != ie && y < rh; ++i) {
		CImage img;
		img.Attach(i->second);
		if (img.IsNull())
			continue;

		int iw = img.GetWidth();
		int ih = img.GetHeight();

		int nh = ih * rw / iw;
		img.Draw(*pDC, rg.left, rg.top + y, rw, nh);

		img.Detach();

		y += nh + 5;
	}
}

bool  CVibraimageExOverview::History::Check() {
	if (!m_hWnd)
		return false;
	ULONGLONG hash = GetHash();
	if (hash == lHash)
		return false;
	lHash = hash;
	RedrawWindow();
	return true;
}

ULONGLONG  CVibraimageExOverview::History::GetHash( std::map<ULONGLONG, HBITMAP> * pBM) {
	ULONGLONG hash = 0;
	HBITMAP  bm[1024];
	ULONGLONG dt[1024];
	DWORD	  id[1024];

	ULONGLONG t0 = GetTickCount64();
	int nc = (int)theApp.m_pEngines.size();
	for (int core = 0; core < nc; ++core) {
		if (!theApp.m_pEngines[core] || !theApp.m_pEngines[core]->started)
			continue;
		CViEngineExNS::CViEngineExAutoCaptureInterface *pc = theApp.m_pEngines[core]->engine.GetCapture();
		if (!pc)
			continue;

		int n = pc->GetCaptured(bm, dt, id, 1024);

		hash += n;

		for (int i = 0; i < n; ++i) {
			ULONGLONG hid = (((ULONGLONG)dt[i]) << 32) + (id[i] << 16) + ((ULONGLONG)(bm[i]));
			hash += hid;
			if (pBM) {
				ULONGLONG hid0 = (((ULONGLONG)(t0 - dt[i])) << 32) + (id[i]);

				(*pBM)[hid0] = bm[i];
			}
		}
	}
	return hash;
}