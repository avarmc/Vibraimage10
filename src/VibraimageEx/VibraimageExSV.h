#pragma once

#include "colorprocress.h"
#include "PropertiesWndBase.h"
// 
// CVibraimageExSV
using namespace CDockablePaneVI;

class CVibraimageExSV : public CPropertiesWndBase, public CNewVarEventVI
{
	DECLARE_DYNAMIC(CVibraimageExSV)

	CColorProcress	m_wndProgress;

public:
	CVibraimageExSV(CPropertiesPack *pPack, int iid);
	virtual ~CVibraimageExSV();

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


