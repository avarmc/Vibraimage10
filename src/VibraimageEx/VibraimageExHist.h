#pragma once

#include "PropertiesWndBase.h"
// CVibraimageExHist

using namespace CDockablePaneVI;

class CVibraimageExHist : public CPropertiesWndBase
{
	DECLARE_DYNAMIC(CVibraimageExHist)

protected:
	xml_document		m_xml;
	CStringA			m_xmlTmp;

public:
	CVibraimageExHist(CPropertiesPack *pPack, int iid);
	virtual ~CVibraimageExHist();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	

	virtual bool Load(xml_node& xml);
	virtual bool LoadNode(xml_node& xml);
	virtual bool AddItem(xml_node& xml);
	bool Align();
	
	afx_msg void OnMove(int x, int y);
};


