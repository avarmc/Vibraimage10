#pragma once

#include "colorprocress.h"
#include "PropertiesWndBase.h"
// 
// CVibraimageExAIV
using namespace CDockablePaneVI;

class CVibraimageExAIV : public CPropertiesWndBase, public CNewVarEventVI
{
	DECLARE_DYNAMIC(CVibraimageExAIV)

	std::recursive_mutex					m_wndLock;
	std::map< int, std::shared_ptr<CColorProcress> >			m_wndProgress;

	int										m_nAlign,m_nUpdate,m_nShow;

	enum AI_MODE { AI_MODE_AVG = 2, AI_MODE_SINGLE = 4, AI_MODE_MEDIAN = 8};
public:
	CVibraimageExAIV(CPropertiesPack *pPack, int iid);
	virtual ~CVibraimageExAIV();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();

	void Align();

	virtual bool Load(xml_node& xml);
	virtual void OnNewVar(int id, int subID);

	bool FormatMenu(CCmdUI* pCmdUI, const CString& txt, BOOL en, BOOL ch);

	virtual void OnTimer(UINT_PTR nIDEvent);
	int CheckAndCreate();
	int GetWndCount();

	void Update();
};


