#pragma once

#include "VIImages.h"

// CVibraimageExToolBar

class CVibraimageExToolBar : public CMFCToolBar
{
	DECLARE_DYNAMIC(CVibraimageExToolBar)

	class BTN :public CMFCToolBarButton
	{
	public:
		int	m_iid, m_lid;
	public:
		BTN( UINT uiID, int iImage, int iText, LPCTSTR lpszText, BOOL bUserButton = FALSE, BOOL bLocked = FALSE)
			:CMFCToolBarButton(uiID, iImage, lpszText, bUserButton, bLocked)
		{
			m_iid = uiID;
			m_lid = iText;
		}
		virtual ~BTN() {}

		virtual BOOL OnUpdateToolTip(CWnd* /*pWndParent*/, int /*iButtonIndex*/, CToolTipCtrl& /*wndToolTip*/, CString& /*str*/);
		virtual BOOL OnGetCustomToolTipText(CString& strToolTip);
	};
public:
	CVIImages	*	m_pImg;
public:
	CVibraimageExToolBar();
	virtual ~CVibraimageExToolBar();

	bool Load(const CString& path);
	CMFCToolBarButton* LoadButton(xml_node& xml, int w, int h);


	BOOL LoadStateVisible();


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


