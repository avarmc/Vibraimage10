#pragma once

#include "PropertiesWndBase.h"
// CVibraimageExGraph

using namespace CDockablePaneVI;

class CVibraimageExGraph : public CPropertiesWndBase
{
	DECLARE_DYNAMIC(CVibraimageExGraph)

protected:
	bool m_bTmpHideGraphs;
	HWND m_wndGraph;
	xml_document		m_xml;
	CStringA			m_xmlTmp;
public:
	CVibraimageExGraph(CPropertiesPack *pPack, int iid);
	virtual ~CVibraimageExGraph();

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

	virtual void CheckTmpHide();

	bool Align();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);

	virtual BOOL OnBeforeShowPaneMenu(CMenu& menu);
	virtual BOOL OnAfterShowPaneMenu(int nMenuResult);

	virtual void ShowPane(BOOL bShow, BOOL bDelay, BOOL bActivate/* = TRUE*/);
};


