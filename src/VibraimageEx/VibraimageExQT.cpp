// VibraimageExQT.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExQT.h"
#include "PropertiesPack.h"

// CVibraimageExQT

IMPLEMENT_DYNAMIC(CVibraimageExQT, CPropertiesWndBase)

CVibraimageExQT::CVibraimageExQT(CPropertiesPack *pPack, int iid) 
	: CPropertiesWndBase(pPack, iid)
	, m_wndProgress(2)
{
	if(!theApp.m_pQT)
		theApp.m_pQT = this;

	m_wndProgress.m_font2ScaleSet = 1.7;
}

CVibraimageExQT::~CVibraimageExQT()
{
	for (size_t i = 0, s = m_btns.size(); i < s; ++i) {
		if (m_btns[i])
			delete m_btns[i];
	}
	if (theApp.m_pQT == this)
		theApp.m_pQT = 0;
}


BEGIN_MESSAGE_MAP(CVibraimageExQT, CPropertiesWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
//	ON_WM_TIMER()
END_MESSAGE_MAP()



// CVibraimageExQT message handlers




int CVibraimageExQT::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertiesWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	GetClientRect(&rc);
	m_wndProgress.Create(_T(""), WS_CHILD | WS_VISIBLE, rc, this, 0);
	m_wndProgress.SetRange(0, 100);
	m_wndProgress.SetRange(0, 100,1); 
	m_wndProgress.m_cf1 = RGB(64, 64, 224); 
	m_wndProgress.SetStrText(_T(""));
	m_wndProgress.m_bAbsColor = true;
	m_wndProgress.m_font2Scale = 2;

	UpdateUI();

	return 0;
}


void CVibraimageExQT::OnSize(UINT nType, int cx, int cy)
{
	CPropertiesWndBase::OnSize(nType, cx, cy);

	Align();
}

void CVibraimageExQT::PutQT() {
	float v = theApp.m_pEngine->engine.GetVar(VI_INFO_CHQ_TEST_VALUE, 0, VT_R4).fltVal;
	int type = theApp.m_pEngine->engine.GetVar(VI_INFO_CHQ_TEST_TYPE, 0, VT_INT).intVal;

	if (GetFlag("noqtw")) {
		PutQT(0, 0);
		return ;
	}

	PutQT(v, type);
}

void CVibraimageExQT::PutQT(float v,int type)
{
	
	TCHAR strText[64] = { 0 };

	CString hdr,hdr1,top;
	if (GetFlag("qthdr"))
		hdr = L"QT",hdr1 = L"\nError ";
	   	 

	m_wndProgress.SetStrText(strText);
	m_wndProgress.SetPos(v);
	switch (type)
	{
	case 1:
		top = (hdr + hdr1+ _T("1"));
		break;
	case 2:
		top = (hdr + hdr1 + _T("2"));
		break;
	case 3:
		top = (hdr + hdr1 + _T("3"));
		break;
	case 4:
		top = (hdr + hdr1 + _T("4"));
		break;
	case 5:
		top = (hdr + hdr1 + _T("5"));
		break;
	default:
		top = (hdr);
		break;
	}

	m_wndProgress.SetStrTop(top);
}

void CVibraimageExQT::OnNewVar(int id, int subID)
{
	if (id == VI_INFO_QTEST) {
		bool old = m_bQT;
		m_bQT = isQT()?3:0;
		PutQT();
	}

	if (m_bQT && (id == VI_INFO_CHQ_TEST_VALUE || id == VI_INFO_CHQ_TEST_QUALITY || id == VI_FILTER_PAUSE || id == VI_VAR_STAT_RES_P39))
	{
		PutQT();
		return;
	}
	
	if (id == VI_INFO_M_PROGRESS)
	{
		float v = 0;
		if (!GetFlag("noprogress")) {
			v = theApp.m_pEngine->engine.GetVar(id, 0, VT_R4).fltVal;
		}

		if (GetFlag("noqtw")) {
			CString strText;
			if (v > 0 && v < 1) {
				if (GetFlag("time")) {
					float period = theApp.m_pEngine->engine.GetVar(VI_INFO_M_PERIOD, 0, VT_R4).fltVal;
					strText.Format(L"%0.0f", v * period);
				}
				else 
				if (GetFlag("rtime")) {
					float period = theApp.m_pEngine->engine.GetVar(VI_INFO_M_PERIOD, 0, VT_R4).fltVal;
					strText.Format(L"%0.0f", (1 - v) * period);
				}
				else
					strText.Format(L"%0.0f%%", v * 100);
					
			}
			m_wndProgress.SetStrText(strText);
		}

		if (GetFlag("rev"))
			v = 1-v;

		m_wndProgress.SetPos(v < 1 ? v * 100 : 0, m_bQT ? 1 : 0);
	
		return;
	}

}

int CVibraimageExQT::LoadBtns(xml_node& xml) {
	
	for (int i = 0; i < 10; ++i)
		LoadBtns(xml, i);

	return (int)m_btns.size();
}

int CVibraimageExQT::LoadBtns(xml_node& xml, int n) {
	auto cfg = theApp.m_pEngine->engine.GetConfigBase();
	if (!cfg)
		return 0;

	CStringA tag;
	if (n) tag.Format("btn%d", n); else tag = "btn";

	xml_attribute a = xml.attribute(tag);

	if (a) {
		std::vector<CString> arr = explode(CString(a.wvalue().c_str()), CString(_T("/")));
		while (m_btns.size() <= n)
			m_btns.push_back(0);

		m_btns[n] = new CMFCButton();
		m_btns[n]->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_ICON, CRect(0, 0, 0, 0), this, theApp.m_pEngines[0]->engine.Tag2Id(arr[0]));

		m_btns[n]->m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;

		CString path = GetDefaultPath() + cfg + _T("\\images\\");
		HICON img1 = 0, img2 = 0;
		if (arr.size() >= 2 && !arr[1].IsEmpty()) {
			img1 = (HICON)LoadImage( // returns a HANDLE so we have to cast to HICON
				NULL,             // hInstance must be NULL when loading from a file
				path + arr[1],   // the icon file name
				IMAGE_ICON,       // specifies that the file is an icon
				64,                // width of the image (we'll specify default later on)
				64,                // height of the image
				LR_LOADFROMFILE |  // we want to load a file (as opposed to a resource)
				LR_SHARED         // let the system release the handle when it's no longer used
			);
		}
		if (arr.size() >= 3 && !arr[2].IsEmpty()) {
			img2 = (HICON)LoadImage( // returns a HANDLE so we have to cast to HICON
				NULL,             // hInstance must be NULL when loading from a file
				path + arr[2],   // the icon file name
				IMAGE_ICON,       // specifies that the file is an icon
				64,                // width of the image (we'll specify default later on)
				64,                // height of the image
				LR_LOADFROMFILE |  // we want to load a file (as opposed to a resource)
				LR_SHARED         // let the system release the handle when it's no longer used
			);
		}


		if (img1)
			m_btns[n]->SetIcon(img1);

		if (img2)
			m_btns[n]->SetCheckedImage(img2);
	}


	return (int)m_btns.size();
}

bool CVibraimageExQT::Load(xml_node& xml)
{
	bool ret;

	LoadBtns(xml);

	ret = CPropertiesWndBase::Load(xml);

	if (GetFlag("noqt") && theApp.m_nStart == 1) {
		theApp.m_pEngine->engine.PutVar(VI_INFO_QTEST, 0, COleVariant((long)0));
	}

	UpdateUI();

	return ret;
}

void CVibraimageExQT::UpdateUI( bool bRef) {

	m_bQT = isQT();

	if (m_bQT) {
		m_wndProgress.m_c1 = RGB(224, 0, 0);
		m_wndProgress.m_cx[0] = RGB(224, 0, 0);
		m_wndProgress.m_cx[50] = RGB(224, 224, 0);
		m_wndProgress.m_cx[80] = RGB(0, 224, 0);
		m_wndProgress.m_vFmt = _T("%0.0f%%");
		m_wndProgress.m_bStr = true;
	}
	else {
		m_wndProgress.m_c1 = RGB(0,224, 0);
		m_wndProgress.m_cx.clear();
		m_wndProgress.m_vFmt = _T("");
		m_wndProgress.m_bStr = false;
	}

	if (GetFlag("yellow"))
		m_wndProgress.m_c1 = RGB(224, 224, 0);
	else
	if (GetFlag("red"))
		m_wndProgress.m_c1 = RGB(224, 0, 0);
	else
	if (GetFlag("blue"))
		m_wndProgress.m_c1 = RGB(0, 0, 224);
	else
		m_wndProgress.m_c1 = RGB(224, 0, 0);

	if(!bRef)
		PutQT();

	if (GetFlag("noqtw"))
		m_wndProgress.SetStrTop(L"T,s");
}

void CVibraimageExQT::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if (m_hWnd)
		Align();
}

void CVibraimageExQT::Align()
{
	m_wndProgress.m_c0 = theApp.m_pEngine ? theApp.m_pEngine->engineInterface.GetWndColor(CLR_Window) : RGB(255, 255, 255);
	m_wndProgress.m_cf = theApp.m_pEngine ? theApp.m_pEngine->engineInterface.GetWndColor(CLR_TextValue) : 0;


	CRect rc,rp,rb;
	GetClientRect(&rc);
	rp = rb = rc;

	for (size_t i = 0, s = m_btns.size(); i < s; ++i) {
		if (m_hWnd && m_btns[i] && m_btns[i]->m_hWnd) {
			if (rc.Height() > rc.Width()) {
				rb.top = rp.top;
				rp.top += rc.Width();
				rb.bottom = rp.top;
			}
			else
			{
				rb.left = rp.left;
				rp.left += rc.Height();
				rb.right = rp.left;
			}
			m_btns[i]->MoveWindow(&rb);
		}
	}
	if (m_hWnd && m_wndProgress.m_hWnd)
		m_wndProgress.MoveWindow(&rp);
	UpdateUI();
}


bool CVibraimageExQT::FormatMenu(CCmdUI* pCmdUI, const CString& txt, BOOL en, BOOL ch) {
	for (size_t i = 0, s = m_btns.size(); i < s; ++i) {
		if (!(m_hWnd && m_btns[i] && m_btns[i]->m_hWnd))
			continue;
		if (pCmdUI->m_nID == GetWindowLong(m_btns[i]->m_hWnd, GWL_ID)) {
			pCmdUI->Enable(en);
			pCmdUI->SetCheck(ch);
			pCmdUI->SetText(_T(""));
			return true;
		}
	}
	return false;
}

BOOL CVibraimageExQT::isQT() {
	if (GetFlag("noqtw"))
		return 0;
	return theApp.m_pEngine->engine.GetVar(VI_INFO_QTEST, 0, VT_INT).intVal;
}

void CVibraimageExQT::OnTimer(UINT_PTR nIDEvent)
{
	if (isQT() != m_bQT)
		UpdateUI();

	__super::OnTimer(nIDEvent);
}
