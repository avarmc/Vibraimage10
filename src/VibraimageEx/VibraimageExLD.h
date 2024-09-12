#pragma once

#include "PropertiesWndBase.h"
// CVibraimageExLD

using namespace CDockablePaneVI;

class CVibraimageExLD : public CPropertiesWndBase
{
	DECLARE_DYNAMIC(CVibraimageExLD)

protected:
	HWND m_wndLD;
	xml_document		m_xml;
	CStringA			m_xmlTmp;
public:
	CVibraimageExLD(CPropertiesPack *pPack, int iid);
	virtual ~CVibraimageExLD();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	virtual bool Load(xml_node& xml);
	virtual bool LoadNode(xml_node& xml);
	virtual bool AddItem(xml_node& xml);
	bool Align();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
};


