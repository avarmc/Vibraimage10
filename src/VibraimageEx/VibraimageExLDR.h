#pragma once

#include "colorprocress.h"
#include "PropertiesWndBase.h"
// 
// CVibraimageExLDR
using namespace CDockablePaneVI;

class CVibraimageExLDR : public CPropertiesWndBase, public CNewVarEventVI
{
	DECLARE_DYNAMIC(CVibraimageExLDR)

	CColorProcress	m_wndProgress;

public:
	CVibraimageExLDR(CPropertiesPack *pPack, int iid);
	virtual ~CVibraimageExLDR();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);

	void Align();

	virtual bool Load(xml_node& xml);

	virtual void OnNewVar(int id, int subID);

};


