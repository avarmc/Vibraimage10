#pragma once

#include "ViExFont.h"

#include <map>
// CColorProcress

class CColorProcress : public CStatic
{
//	DECLARE_DYNAMIC(CColorProcress)
	DECLARE_DYNCREATE(CColorProcress)
public:
	enum ALIGN {ALIGN_LEFT,ALIGN_CENTER,ALIGN_RIGHT};
	CColorProcress(int nVar=1);
	virtual ~CColorProcress();
protected:
	typedef struct tagVAR
	{
		float		m_vMin, m_vMax, m_v;
	} VAR;
	std::vector<VAR>	m_var;
public:
	CViExFont		m_font,m_font2, m_font3;

public:
	float		m_fontScale, m_font2Scale, m_font2ScaleSet;
public:
	CString		m_vFmt;
	bool		m_bInverse;
	bool		m_bSemaphore;
	float		m_SemaphoreLevel;
	bool		m_bAbsColor;
	bool		m_bResize;
public:
	BOOL	*	m_pLock;
	int			m_nAlign;
	bool		m_bRedraw,m_bDoRedraw;
	bool		m_bFontTmp = false;
	COLORREF	m_c0,m_c1,m_cf, m_cf1;
	bool		m_bStr;
	std::map<float,COLORREF>	m_cx;

	ULONGLONG	m_tDestroy = 0;

protected:
	CCriticalSection	m_lockStr;
	CString		m_text,m_str,m_strTop;

protected:
	DECLARE_MESSAGE_MAP()
protected:
	void CreateFont(int cx, int cy);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual void	Draw(CDC& dc, CRect &rView, bool bNewFont = false,int nVar=0);
	virtual void	Draw0(CDC& dc,CRect &rView,bool bNewFont=false);
	virtual void	Draw1(CDC& dc, CRect &rView, bool bNewFont = false);
	void OnPaintStd(CDC& dc,CRect &rView);
	void OnPaintSemaphore(CDC& dc,CRect &rView);
public:
	void CreateFont(int h);
	void GetRange(float* pMin, float* pMax,int nVar=0);
	void SetRange(float vMin, float vMax, int nVar = 0);
	void SetPos(float v, int nVar = 0,bool bRemake=true);
	float GetPos(int nVar = 0) { return m_var[nVar].m_v; }
	COLORREF GetColor(void);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	void SetStrTop(const CString& str) 	{ CSingleLock lock(&m_lockStr,TRUE); m_strTop = str; if(m_bRedraw) m_bDoRedraw = true; }
	void SetStrStr(const CString& str) 	{ CSingleLock lock(&m_lockStr,TRUE); m_str = str; if(m_bRedraw) m_bDoRedraw = true; }
	void SetStrText(const CString& str) 	{ CSingleLock lock(&m_lockStr,TRUE); m_text = str; if(m_bRedraw) m_bDoRedraw = true; }

	CString GetStrTop() { CSingleLock lock(&m_lockStr,TRUE); return m_strTop; }
	CString GetStrStr() { CSingleLock lock(&m_lockStr,TRUE); return m_str; }
	CString GetStrText() { CSingleLock lock(&m_lockStr,TRUE); return m_text; }
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

	const CColorProcress& operator = (const CColorProcress & c);
};


