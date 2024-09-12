#pragma once

#include "NewVarEvent.h"
#include "colorprocress.h"

// CVibraimageExOverview

class CVibraimageExOverview : public CFrameWndEx, public CNewVarEventVI
{
	DECLARE_DYNCREATE(CVibraimageExOverview)

protected:
	class View : public CWnd
	{
	public:
		int core;
		int idr;
		int nRedraw;
		BITMAPINFO pbi;
		CColorProcress	wndProgress;
		int			nFail;
		CRect		rImg;
	public:
		View(int _core,int _idr);
		virtual ~View();

		void Draw(CDC *pDC, CRect& rg);
		void DrawImg(CDC *pDC, CRect& ri);
		void DrawState(CDC *pDC, CRect& rs,int rw);


		bool Check();
		void UpdateImg();
		void UpdateState();
		afx_msg void OnPaint();
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

		void SetWindowText(LPCTSTR lpszString);
	protected:
		DECLARE_MESSAGE_MAP()

	};

	class History : public CWnd
	{
		ULONGLONG	lHash;
	public:
		History();
		virtual ~History();

		void Draw(CDC *pDC, CRect& rg);

		bool Check();
		ULONGLONG GetHash(std::map<ULONGLONG, HBITMAP> * pBM=0);

		afx_msg void OnPaint();
		

	protected:
		DECLARE_MESSAGE_MAP()

	};
public:
	CVibraimageExOverview();
	virtual ~CVibraimageExOverview();

	virtual void OnNewVar(int id, int subID, int core);

	static void Create(bool bInit=false);

	virtual BOOL DestroyWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	static CString	m_regGroup;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	void UpdateLang();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void	Align();
protected:

	History				m_history;
	std::vector<View *> m_views;
	ULONGLONG			m_viewsHash;

	void	Check();
	void	Remake();
	int		GetViewCount(bool bInit=false, ULONGLONG* pHash=0);
	CSize	GetSplit(CRect rc);
	void	Clear();
};






