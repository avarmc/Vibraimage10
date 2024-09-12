// VibraimageExQT.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExAi.h"


// CVibraimageExAI

IMPLEMENT_DYNAMIC(CVibraimageExAI, CPropertiesWndBase)

CVibraimageExAI::CVibraimageExAI(CPropertiesPack *pPack, int iid) : CPropertiesWndBase(pPack, iid)
{
	theApp.m_pAI = this;
}

CVibraimageExAI::~CVibraimageExAI()
{
}


BEGIN_MESSAGE_MAP(CVibraimageExAI, CPropertiesWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
END_MESSAGE_MAP()



// CVibraimageExAI message handlers




int CVibraimageExAI::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertiesWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;


	return 0;
}


void CVibraimageExAI::OnSize(UINT nType, int cx, int cy)
{
	CPropertiesWndBase::OnSize(nType, cx, cy);

	Align();
}


bool CVibraimageExAI::Load(xml_node& xml)
{
	return CPropertiesWndBase::Load(xml);
}


void CVibraimageExAI::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if (m_hWnd)
		Align();
}

void CVibraimageExAI::Align()
{
	CRect rc;
	GetClientRect(&rc);
	for (int core = 0, coreMax = (int)theApp.m_pEngines.size(); core < coreMax; ++core) {
		HWND hWndImg = theApp.m_pEngines[core]->engine.GetCapture()->GetWindow();
		if (hWndImg)
		{
			if (theApp.m_pEngines[core]->id == theApp.m_pEngine->id) {
				if (::GetParent(hWndImg) != m_hWnd)
					::SetParent(hWndImg, m_hWnd);
				::MoveWindow(hWndImg, rc.left, rc.top, rc.Width(), rc.Height(), TRUE);
				::ShowWindow(hWndImg, SW_SHOW);
			} else
				::ShowWindow(hWndImg, SW_HIDE);
		}
	}
}
