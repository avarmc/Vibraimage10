// VibraimageExQT.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExLDR.h"


// CVibraimageExLDR

IMPLEMENT_DYNAMIC(CVibraimageExLDR, CPropertiesWndBase)

CVibraimageExLDR::CVibraimageExLDR(CPropertiesPack *pPack, int iid) : CPropertiesWndBase(pPack, iid)
{
	theApp.m_pLDR = this;
}

CVibraimageExLDR::~CVibraimageExLDR()
{
}


BEGIN_MESSAGE_MAP(CVibraimageExLDR, CPropertiesWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
END_MESSAGE_MAP()



// CVibraimageExLDR message handlers




int CVibraimageExLDR::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertiesWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	GetClientRect(&rc);
	m_wndProgress.Create(_T(""), WS_CHILD | WS_VISIBLE, rc, this, 0);
	m_wndProgress.SetRange(0, 100);
	m_wndProgress.m_vFmt = _T("%0.0f %%");
	m_wndProgress.m_c1 = RGB(224, 0, 0);
	m_wndProgress.SetStrText(_T(""));
	m_wndProgress.m_bAbsColor = true;
	m_wndProgress.m_font2Scale = 2;

	return 0;
}


void CVibraimageExLDR::OnSize(UINT nType, int cx, int cy)
{
	CPropertiesWndBase::OnSize(nType, cx, cy);

	Align();
}


void CVibraimageExLDR::OnNewVar(int id, int subID)
{
#if defined VIENGINEEX_GET_ENGINE
	if (id == VI_VAR_LD_RESULT)
	{
		CVIENGINE_CLASS* pEngine = theApp.m_pEngine->engine.GetEngine();
		float v = pEngine ? pEngine->GetF1(id)*100 : 0;
		CString result = pEngine ? pEngine->GetStr(id) : _T("");

		if (result.IsEmpty())
		{
			TCHAR strText[32];
			_stprintf_s(strText, 32, _T("%i%%"), (int)v);
			m_wndProgress.m_c1 = RGB(224, 0, 0);
			m_wndProgress.m_c0 = RGB(255, 255, 255);

			m_wndProgress.SetStrText(strText);
			m_wndProgress.SetRange(0, 100);
			m_wndProgress.SetPos(v);
		}
		else
		{
			if (result == _T("NDI"))
			{
				m_wndProgress.m_c1 = m_wndProgress.m_c0 = RGB(0, 224, 0);
			}
			else
			if (result == _T("DI"))
			{
				m_wndProgress.m_c1 = m_wndProgress.m_c0 = RGB(224, 0, 0);
			}
			else
			{
				m_wndProgress.m_c1 = m_wndProgress.m_c0 = RGB(224, 224, 0);
			}
			m_wndProgress.SetStrText(result);
			m_wndProgress.SetRange(0, 0);
			m_wndProgress.SetPos(0);
		}
	}
#endif // #if !defined SEQ_MEDIC_MED_SDK && !defined SEQ_LITE_MID_SDK SEQ_MEDIC_MOL
}

bool CVibraimageExLDR::Load(xml_node& xml)
{
	return CPropertiesWndBase::Load(xml);
}


void CVibraimageExLDR::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if (m_hWnd)
		Align();
}

void CVibraimageExLDR::Align()
{
	CRect rc;
	GetClientRect(&rc);
	if (m_hWnd && m_wndProgress.m_hWnd)
		m_wndProgress.MoveWindow(&rc);
}
