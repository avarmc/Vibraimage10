#pragma once

#include "PropertiesWndBase.h"
// CVibraimageExHistStat

using namespace CDockablePaneVI;

class CVibraimageExHistStat : public CPropertiesWndBase
{
	DECLARE_DYNAMIC(CVibraimageExHistStat)

protected:
	CString				m_sid;
public:
	CVibraimageExHistStat(CPropertiesPack *pPack, int iid);
	virtual ~CVibraimageExHistStat();

protected:
	DECLARE_MESSAGE_MAP()
public:
	bool Align();


	virtual bool Load(xml_node& xml);
	virtual bool LoadNode(xml_node& xml);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
};


