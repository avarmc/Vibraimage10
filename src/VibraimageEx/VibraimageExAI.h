#pragma once

#include "PropertiesWndBase.h"
// 
// CVibraimageExAI
using namespace CDockablePaneVI;

class CVibraimageExAI : public CPropertiesWndBase
{
	DECLARE_DYNAMIC(CVibraimageExAI)


public:
	CVibraimageExAI(CPropertiesPack *pPack, int iid);
	virtual ~CVibraimageExAI();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);

	void Align();

	virtual bool Load(xml_node& xml);
};


