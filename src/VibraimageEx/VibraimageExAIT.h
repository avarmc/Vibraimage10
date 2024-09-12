#pragma once

#include "colorprocress.h"
#include "PropertiesWndBase.h"
// 
// CVibraimageExAIT
using namespace CDockablePaneVI;

class CVibraimageExAIT : public CPropertiesWndBase, public CNewVarEventVI
{
	DECLARE_DYNAMIC(CVibraimageExAIT)

	CColorProcress				m_wndProgress;
public:
	CVibraimageExAIT(CPropertiesPack *pPack, int iid);
	virtual ~CVibraimageExAIT();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);

	void Align();

	virtual bool Load(xml_node& xml);
	virtual void OnNewVar(int id, int subID);

	bool FormatMenu(CCmdUI* pCmdUI, const CString& txt, BOOL en, BOOL ch);

	virtual void OnTimer(UINT_PTR nIDEvent);
};


