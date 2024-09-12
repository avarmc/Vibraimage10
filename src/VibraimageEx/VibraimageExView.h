
// VibraimageExView.h : interface of the CVibraimageExView class
//

#pragma once

#include "colorprocress.h"
#include "NewVarEvent.h"
#include "VibraimageExToolBar.h"

class CVibraimageExView : public CView, public CNewVarEventVI
{
protected: // create from serialization only
	CVibraimageExView();
	DECLARE_DYNCREATE(CVibraimageExView)

// Attributes
public:
	CVibraimageExDoc* GetDocument() const;

	class TAB : public CMFCTabCtrl
	{
	public:
		TAB() {}
		virtual ~TAB() {}

	public:
		virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	};

	TAB		m_wndTabs;

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void OnInit();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CVibraimageExView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnAfxWmChangingActiveTab(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAfxWmRenameTab(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

	void OnCloseFrame();
	void Align();
	void AddTab(CWnd* pTabWnd, LPCWSTR lpszTabLabel, UINT uiImageId = (UINT)-1, BOOL bDetachable = TRUE);
	void RemoveTab(CWnd* pTabWnd);

	bool ShowTab(CBasePane* pBar, BOOL bShow);

	CString GetTabName(int core);
};

#ifndef _DEBUG  // debug version in VibraimageExView.cpp
inline CVibraimageExDoc* CVibraimageExView::GetDocument() const
   { return reinterpret_cast<CVibraimageExDoc*>(m_pDocument); }
#endif

