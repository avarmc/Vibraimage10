#pragma once

#ifdef VISTATICHIST_EXPORTS
#define VISTATICHIST_API __declspec(dllexport)
#else
#define VISTATICHIST_API __declspec(dllimport)
#endif


// CViStaticHist

class VISTATICHIST_API CViStaticHist : public CWnd
{
	DECLARE_DYNAMIC(CViStaticHist)

	void *m_ptr;
protected:
	CButton	m_wndBtn[2];
public:
	CViStaticHist();
	virtual ~CViStaticHist();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	bool StartUpdate(bool bStart, DWORD dt=40);
	bool AddHist(int gid, int id,int width, int dt=0);
	bool Draw(CDC* pDC, const CRect& rc);
	void SetPal(RGBQUAD* pal);

	virtual COLORREF GetWndColor(int id);
public:
	void			UpdateHists();
	virtual	void 	OnHistUpdate(int gid, int iid, UINT *pHist, float *pFps,int dt) {}
	virtual int	GetMinStep(int gid, int iid) { return 0; }
};


