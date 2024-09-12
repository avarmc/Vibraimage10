#pragma once

#include "colorprocress.h"
#include "PropertiesWndBase.h"
// 
// CVibraimageExQT
using namespace CDockablePaneVI;

class CVibraimageExQT : public CPropertiesWndBase, public CNewVarEventVI
{
	DECLARE_DYNAMIC(CVibraimageExQT)

	CColorProcress				m_wndProgress;
	std::vector<CMFCButton*>	m_btns;
	BOOL						m_bQT;
public:
	CVibraimageExQT(CPropertiesPack *pPack, int iid);
	virtual ~CVibraimageExQT();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);

	void Align();

	virtual bool Load(xml_node& xml);
	virtual int LoadBtns(xml_node& xml);
	virtual int LoadBtns(xml_node& xml,int n);
	virtual void OnNewVar(int id, int subID);

	void PutQT();
	void PutQT(float v,int type);

	bool FormatMenu(CCmdUI* pCmdUI, const CString& txt, BOOL en, BOOL ch);

	void UpdateUI(bool bRef = false);

	BOOL isQT();
	virtual void OnTimer(UINT_PTR nIDEvent);
};


