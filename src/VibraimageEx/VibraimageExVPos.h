#pragma once

#include <wmp.h>
#include "PropertiesWndBase.h"

using namespace CDockablePaneVI;

class CVibraimageExVPos : public CPropertiesWndBase, public CNewVarEventVI
{
	DECLARE_DYNAMIC(CVibraimageExVPos)

protected:
	CSliderCtrl	m_wndSlider;
	CButton		m_btnPause;

	int			m_vPos;
	enum ID  { ID_SLIDER = 1, ID_NUM,ID_GO };
public:
	CVibraimageExVPos(CPropertiesPack *pPack, int iid);
	~CVibraimageExVPos();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);

	void Align();

	virtual bool Load(xml_node& xml);

	virtual void OnNewVar(int id, int subID);


	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnActionPause();
	afx_msg void OnUpdateActionPause(CCmdUI *pCmdUI);
};

