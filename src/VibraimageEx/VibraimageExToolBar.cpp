// VibraimageExToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExToolBar.h"
#include <afxregpath.h>

#define AFX_TEXT_MARGIN 3
#define AFX_STRETCH_DELTA 6
#define AFX_BUTTON_MIN_WIDTH 5
#define AFX_TOOLBAR_BUTTON_MARGIN 6

// CVibraimageExToolBar

IMPLEMENT_DYNAMIC(CVibraimageExToolBar, CMFCToolBar)

CVibraimageExToolBar::CVibraimageExToolBar() 
{
	m_pImg = 0;
}

CVibraimageExToolBar::~CVibraimageExToolBar()
{
	if (m_pImg)
		delete m_pImg;
}


BEGIN_MESSAGE_MAP(CVibraimageExToolBar, CMFCToolBar)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

bool CVibraimageExToolBar::Load(const CString& path)
{
	auto cfg = theApp.m_pEngine->engine.GetConfigBase();
	if (!cfg)
		return false;

	CString sPath;
	if (path.IsEmpty())
		sPath = GetDefaultPath() + cfg+_T("\\toolbar.xml");
	else
		sPath = path;

	RemoveAllButtons();

	xml_document xml_doc;

	if(!xml_doc.load_file(sPath))
		return false;

	xml_node xml = xml_doc.first_child();
	int w = 0, h = 0;
	const char * attr = 0;
	attr = xml.attribute_value("w");
	if (attr) w = atoi(attr);
	attr = xml.attribute_value("h");
	if (attr) h = atoi(attr);

	int iCX = GetSystemMetrics(SM_CXICON);
	int iCY = GetSystemMetrics(SM_CYICON);

	int iCXS = GetSystemMetrics(SM_CXSMICON);
	int iCYS = GetSystemMetrics(SM_CYSMICON);

	int iw = w*32/iCX;
	int ih = h*32/iCY;

	BOOL bLocked = TRUE;
	m_bLocked = bLocked;

	CSize sizeImage(iw, ih);
	CSize sizeButton(iw + AFX_TOOLBAR_BUTTON_MARGIN, ih + AFX_TOOLBAR_BUTTON_MARGIN);

	BOOL bDontScaleImages = bLocked ? m_bDontScaleLocked : m_bDontScaleImages;

	if (!bDontScaleImages && GetGlobalData()->GetRibbonImageScale() != 1.)
	{
		double dblImageScale = GetGlobalData()->GetRibbonImageScale();
		sizeButton = CSize((int)(.5 + sizeButton.cx * dblImageScale), (int)(.5 + sizeButton.cy * dblImageScale));
	}

	if (bLocked)
	{
		SetLockedSizes(sizeButton, sizeImage);
	}
	else if (!m_Images.IsScaled())
	{
		SetSizes(sizeButton, sizeImage);
	}
	
	

	int pos = 0;
	xml_node_iterator i, ie;
	for (i = xml.begin(), ie = xml.end(); i != ie; ++i, ++pos)
	{
		CStringA tag = i->name();
		CStringA sid = i->attribute_value("id");
		int iid = theApp.m_pEngine->engine.Tag2Id( CString(sid) );
		int lid = theApp.m_pEngine->engine.GetLang()->Find(i->attribute_wvalue("name").c_str());
		CString name = theApp.m_pEngine->engine.GetLang()->Get(lid,i->attribute_wvalue("name").c_str());

		if (tag == "button")
		{
			CMFCToolBarButton * pbtn = LoadButton(*i,w,h);
			if (pbtn)
				InsertButton(pbtn);
		} else
		if (tag == "separator")
		{
			InsertSeparator();
		}
	}



	CSize szImage, szButton;
	szImage = GetImageSize();
	szButton.cx = szImage.cx + 6;
	szButton.cy = szImage.cy + 6;
//	SetMenuSizes(szButton, szImage);

	DWORD dwStyle = CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
	SetPaneStyle(GetPaneStyle() | dwStyle);


	AdjustLocations();
	Invalidate();

	SetShowTooltips(TRUE);
	RecalcLayout();

	return true;
}

CMFCToolBarButton* CVibraimageExToolBar::LoadButton(xml_node& xml,int w,int h)
{
	CStringA tag = xml.name();
	CStringA sid = xml.attribute_value("id");
	CStringA simg = xml.attribute_value("img");

	int iid = theApp.m_pEngine->engine.Tag2Id(CString(sid));
	int lid = theApp.m_pEngine->engine.GetLang()->Find(xml.attribute_wvalue("name").c_str());
	CString name = theApp.m_pEngine->engine.GetLang()->Get(lid, xml.attribute_wvalue("name").c_str());

	simg.MakeLower();

	CMFCToolBarImages *pImg = &m_ImagesLocked;
	if (!m_pImg)
	{
		m_pImg = new CVIImages(pImg);
		m_pImg->Load(false, w, h);
	}


	int nimg = m_pImg->Get(simg);

	CMFCToolBarButton* pbtn = new BTN(iid, nimg, lid,name, FALSE, m_bLocked);

	
	return pbtn;
}



BOOL CVibraimageExToolBar::LoadStateVisible()
{
	LPCTSTR AFX_BASE_CONTROL_BAR_PROFILE = _T("Workspace");
	LPCTSTR AFX_REG_SECTION_FMT = _T("%TsBasePane-%d");
	LPCTSTR AFX_REG_SECTION_FMT_EX = _T("%TsBasePane-%d%x");

	CString strProfileName = ::AFXGetRegPath(AFX_BASE_CONTROL_BAR_PROFILE, 0);
	strProfileName.TrimRight(L"\\");

	UINT	nIndex = GetDlgCtrlID();
	
	CString strSection;

	strSection.Format(AFX_REG_SECTION_FMT, (LPCTSTR)strProfileName, nIndex);

	CSettingsStoreSP regSP;
	CSettingsStore& reg = regSP.Create(FALSE, TRUE);

	if (!reg.Open(strSection))
	{
		m_bRecentVisibleState = TRUE;
		return FALSE;
	}
	reg.Read(_T("IsVisible"), m_bRecentVisibleState);

	
	return TRUE;
}

BOOL CVibraimageExToolBar::BTN::OnGetCustomToolTipText(CString& strToolTip)
{
	BOOL ok = CMFCToolBarButton::OnGetCustomToolTipText(strToolTip);
	if(!ok)
		strToolTip = theApp.m_pEngine->engine.GetLang()->Get(m_lid);

	 return TRUE;
}


BOOL CVibraimageExToolBar::BTN::OnUpdateToolTip(CWnd* pWndParent, int iButtonIndex, CToolTipCtrl& wndToolTip, CString& strTipText)
{

	if (!CMFCToolBar::GetShowTooltips())
	{
		return FALSE;
	}

	CString strTips;

	if (OnGetCustomToolTipText(strTips))
	{
		strTipText = strTips;
	}


	wndToolTip.AddTool(pWndParent, strTipText, Rect(), iButtonIndex + 1);


	return TRUE;
}




BOOL CVibraimageExToolBar::OnEraseBkgnd(CDC* pDC)
{
	return CMFCToolBar::OnEraseBkgnd(pDC);
}
