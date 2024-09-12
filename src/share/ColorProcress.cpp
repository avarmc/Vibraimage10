// ColorProcress.cpp : implementation file
//

#include "stdafx.h"
#include "colorprocress.h"
#include "float.h"
#include "math.h"
#include "round.h"

// CColorProcress

IMPLEMENT_DYNCREATE(CColorProcress, CStatic)

CColorProcress::CColorProcress(int nVar):
	m_c0(RGB(255,255,255))
	,m_c1(RGB(0,224,0))
	,m_cf(RGB(0,0,0))
	,m_cf1(RGB(0, 224, 0))
	,m_bRedraw(true)
	,m_nAlign(ALIGN_CENTER)
	,m_bStr(true)
	,m_bSemaphore(false)
	,m_bDoRedraw(false)
	,m_pLock(0)
	,m_bInverse(false)
	,m_bAbsColor(false)
	, m_bResize(false)
{
	m_var.resize(nVar);
	for (int i = 0; i < nVar; ++i)
	{
		m_var[i].m_vMax = 1;
		m_var[i].m_vMin = 0;
		m_var[i].m_v = 0;
	}
	m_fontScale = 1;
	m_font2Scale = m_font2ScaleSet = 0.7;
}

CColorProcress::~CColorProcress()
{
}


BEGIN_MESSAGE_MAP(CColorProcress, CStatic)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_MOVING()
	ON_WM_SIZING()
END_MESSAGE_MAP()



// CColorProcress message handlers


void CColorProcress::CreateFont(int h)
{
	m_font.CreateFont((int)(h*m_fontScale));
	m_font2.CreateFont( (int)(h*fabs(m_fontScale*m_font2ScaleSet)));
	m_font3.CreateFont((int)(h * fabs(m_fontScale*m_font2ScaleSet)*2/3));
}


void CColorProcress::GetRange(float* pMin, float* pMax, int nVar)
{
	if(pMin)
		*pMin = m_var[nVar].m_vMin;
	if(pMax)
		*pMax = m_var[nVar].m_vMax;
}

void CColorProcress::SetRange(float vMin, float vMax, int nVar)
{
	if (nVar < 0 || nVar >= (int)m_var.size())
		return;
	if(m_var[nVar].m_vMin != vMin || m_var[nVar].m_vMax != vMax)
	{
		m_var[nVar].m_vMin = vMin, m_var[nVar].m_vMax = vMax;
		if(m_hWnd && m_bRedraw)
		{
			m_bDoRedraw = true;
		//	RedrawWindow(0,0,RDW_INVALIDATE|RDW_UPDATENOW);
		}
	}
}


void CColorProcress::SetPos(float v, int nVar, bool bRemake)
{
	if(m_var[nVar].m_v != v || bRemake)
	{
		m_var[nVar].m_v = v;
		if(m_hWnd && m_bRedraw)
		{
			m_bDoRedraw = true;
//			RedrawWindow(0,0,RDW_INVALIDATE|RDW_UPDATENOW);
		}
	}
}

void CColorProcress::CreateFont(int cx, int cy)
{
	if (cx < cy)
	{
		int nSize = cx / 4;

		if (nSize > cy / 6)
			nSize = cy / 6;

		CreateFont(nSize);
	}
	else
	{
		int nSize = cy;
		
		if (nSize > cx / 8)
			nSize = cx / 8;

		CreateFont(nSize);
	}
}

void CColorProcress::OnSize(UINT nType, int cx, int cy)
{
	if(m_pLock && *m_pLock)
		return;

	CStatic::OnSize(nType, cx, cy);

	CreateFont(cx,cy);

	if(m_hWnd )
	{
#ifdef _DEBUG_CHECK_REDRAW
		_DEBUG_CHECK_REDRAW;
#endif
		if(m_bSemaphore)
			RedrawWindow();
		else
		if(m_bRedraw)
			RedrawWindow(0,0,RDW_INVALIDATE|RDW_UPDATENOW);
	}
}

void CColorProcress::OnPaint()
{
	CSingleLock lock(&m_lockStr,TRUE);
	CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);

	int w = rc.Width();
	int h = rc.Height();
	if (!w || !h)
		return;

	CRect r(0, 0, w, h);

	CImage img;
	img.Create(w, h, 32);

	CImageDC idc(img);
	CDC *pDC = CDC::FromHandle(idc);

	Draw(*pDC,r,false);
	if( m_var.size() > 1)
		Draw1(*pDC, r, false);

	img.Draw(dc, rc.left, rc.top);
}

void	CColorProcress::Draw(CDC& dc, CRect &rView, bool bNewFont, int nVar)
{
	if (!nVar)
		Draw0(dc, rView, bNewFont);
	else
		Draw1(dc, rView, bNewFont);
}

void	CColorProcress::Draw0(CDC& dc,CRect &rView,bool bNewFont)
{

	if(m_bSemaphore)
		OnPaintSemaphore(dc,rView);
	else
	{
		if(bNewFont)
			CreateFont(rView.Width(),rView.Height());
		
		OnPaintStd(dc,rView);

		if(bNewFont)
		{
			m_font.DeleteObject();
			m_font2.DeleteObject();
			m_font3.DeleteObject();
		}

	}

}

void	CColorProcress::Draw1(CDC& dc, CRect &rView, bool bNewFont)
{
	CRect rc = rView;

	rc.top = rc.bottom - round_i(rc.Height()*(m_var[1].m_v - m_var[1].m_vMin) / (m_var[1].m_vMax - m_var[1].m_vMin));

	if (rc.top < rc.bottom)
	{
		rc.left += 2;
		rc.right -= 2;

		CBrush br, *oldBr;
		br.CreateHatchBrush(HS_DIAGCROSS, m_cf1);

		oldBr = dc.SelectObject(&br);

		dc.SetBkMode( TRANSPARENT);
		dc.Rectangle(&rc);

		dc.SelectObject(oldBr);
	}
}

void CColorProcress::OnPaintSemaphore(CDC& dc,CRect &rView)
{
	CRect rc = rView;
	CBrush br1,br2,*pbr;
	CPen pen1,pen2,*ppen;
	CRect rc1,rc2;

	int rh = rc.Height()*3/4;

	rc1.top = rc2.top = rc.top + (rc.Height()-rh)/2;
	rc1.bottom = rc2.bottom = rc.bottom - (rc.Height()-rh)/2;
	
	rc2.right = rc.CenterPoint().x - rh*2/3;
	rc2.left = rc2.right - rh;

	rc1.left = rc.CenterPoint().x + rh*2/3;
	rc1.right = rc1.left + rh;



	if(m_var[0].m_v >= m_SemaphoreLevel)
	{
		br1.CreateSolidBrush(RGB(224,0,0));
		br2.CreateSolidBrush(RGB(255,255,255));
	} else
	{
		br1.CreateSolidBrush(RGB(255,255,255));
		br2.CreateSolidBrush(RGB(0,224,0));
	} 

	pen1.CreatePen(PS_SOLID,max(rh/20,1),RGB(128,0,0));
	pen2.CreatePen(PS_SOLID,max(rh/20,1),RGB(0,128,0));

	ppen = dc.SelectObject(&pen1);
	pbr = dc.SelectObject(&br1);
	dc.Ellipse(&rc1);

	dc.SelectObject(&pen2);
	dc.SelectObject(&br2);
	dc.Ellipse(&rc2);

	dc.SelectObject(pbr);
	dc.SelectObject(ppen);

	br1.DeleteObject();
	br2.DeleteObject();

}

void CColorProcress::OnPaintStd(CDC& dc,CRect &rView)
{
	CRect rc = rView;
	CRect rc0,rc1;

	if (m_font.m_font.m_hObject == NULL || m_bFontTmp) {
		CreateFont(rc.Width(), rc.Height());
		m_bFontTmp = false;
	}

	CFont *oldFont = dc.SelectObject(&m_font.m_font);

	dc.SetTextColor(m_cf);
	dc.SetBkMode(TRANSPARENT);

	rc0 = rc1 = rc;
	float vMax = m_var[0].m_vMax;
	float vMin = m_var[0].m_vMin;
	float v = m_bInverse ? vMax- m_var[0].m_v+vMin : m_var[0].m_v;

	if(m_var[0].m_vMax < m_var[0].m_vMin)
		vMax =1, vMin = v = 0;
	if(v < vMin) v = vMin;
	if(v > vMax) v = vMax;

	CString str;

	if(m_bStr && m_var[0].m_v < 10* m_var[0].m_vMax )
	{
		if(!m_vFmt.IsEmpty())
			str.Format(m_vFmt, m_var[0].m_v);
		else
		if(m_var[0].m_vMax <= 1)
		{
			str.Format(_T("%0.3f"), m_var[0].m_v);
			str.TrimRight('0');
			str.TrimRight('.');
		}
		else
		if(m_var[0].m_vMax <= 100)
			str.Format(_T("%0.1f"), m_var[0].m_v);
		else
			str.Format(_T("%0.0f"), m_var[0].m_v);

	}

	m_str = str;
	CSize strs = dc.GetTextExtent(str);
	CSize strt = dc.GetTextExtent(m_text);

	for (int div = 10; div < 100 && strt.cx; div+=2) {
		if (strt.cx <= rc.Width())
			break;
		
		CreateFont(rc.Width()*10/div, rc.Height()*10/ div);
		m_bFontTmp = true;

		dc.SelectObject(&m_font.m_font);
		strt = dc.GetTextExtent(m_text);	
	}

	if( strs.cx == 0 )
		strs = strt;

	if(strt.cy > strs.cy)
		strs.cy = strt.cy;
	if (strt.cx > strs.cx)
		strs.cx = strt.cx;

	if(rc.Width() > rc.Height())
	{
		if(vMax == vMin)
			rc1.right = rc.left;
		else
			rc1.right	= (LONG)(rc.left + rc.Width()*(v-vMin)/(vMax-vMin));
		rc0.left = rc1.right;

		if(rc1.Width() > 0)
			dc.FillSolidRect(&rc1,GetColor());

		if(rc0.Width() > 0)
			dc.FillSolidRect(&rc0,m_c0);
	} else
	{
		if(vMax == vMin)
			rc1.top	= rc1.bottom;
		else
			rc1.top	= (LONG)(rc.bottom - rc.Height()*(v-vMin)/(vMax-vMin));
		rc0.bottom = rc1.top;

		if(rc1.Height() > 0)
			dc.FillSolidRect(&rc1,GetColor());

		if(rc0.Height() > 0)
			dc.FillSolidRect(&rc0,m_c0);
	}


	CPoint pos;
	if(m_nAlign == ALIGN_CENTER)
		pos = CPoint(rc.left + (rc.Width()-strs.cx)/2,rc.top + (rc.Height()-strs.cy)/2);
	else
	if(m_nAlign == ALIGN_LEFT)
		pos = CPoint(rc.left,rc.top + (rc.Height()-strs.cy)/2);
	else
	if(m_nAlign == ALIGN_RIGHT)
		pos = CPoint(rc.right-strs.cx,rc.top + (rc.Height()-strs.cy)/2);

	if(!m_text.IsEmpty())
	{
		dc.TextOut(pos.x,pos.y,m_text);
	} else
		dc.TextOut(pos.x,pos.y,str);

	if(!m_strTop.IsEmpty())
	{
		std::vector<CString> arr = explode(m_strTop, CString(L"\n"));
		int dy = 0;


		dc.SelectObject(&m_font2.m_font);

		for(size_t i = 0, is = arr.size(); i < is; ++i) {

			CSize s = dc.GetTextExtent(arr[i]);
			int x1, y1;
			if (rc.Height() > rc.Width())
			{
				x1 = rc.left + (rc.Width() - s.cx) / 2;
				y1 = rc.top + s.cy / 2;
			}
			else
			{
				x1 = rc.left + s.cy / 2;
				y1 = rc.top + (rc.Height() - s.cy) / 2;
			}
			dc.TextOut(x1, y1+dy, arr[i]);

			dy += s.cy*5/4;
			dc.SelectObject(&m_font3.m_font);
		}
	}

	dc.SelectObject(oldFont);
}

COLORREF CColorProcress::GetColor()
{
	if(m_var[0].m_v <= m_var[0].m_vMin )
		return m_c0;
	COLORREF c = m_c1;
	if(m_cx.empty())
		return c;


	float vm = FLT_MAX;
	std::map<float,COLORREF>::iterator i;
	for( i = m_cx.begin(); i != m_cx.end(); ++i)
	{
		if(!m_bAbsColor)
		{
			float dv = fabs(m_var[0].m_v - i->first);
			if(dv <= vm)
				c = i->second, vm = dv;
		} else
		{
			if(m_var[0].m_v >= i->first )
				c = i->second;
		}
	}


	return c;
}

void CColorProcress::OnLButtonDown(UINT nFlags, CPoint point)
{
	GetParent()->SendMessage(WM_LBUTTONDOWN,nFlags,(point.x&0xFFFF)|(point.y<<16));

	CStatic::OnLButtonDown(nFlags, point);
}

void CColorProcress::OnLButtonUp(UINT nFlags, CPoint point)
{
	GetParent()->SendMessage(WM_LBUTTONUP,nFlags,(point.x&0xFFFF)|(point.y<<16));

	CStatic::OnLButtonUp(nFlags, point);
}

BOOL CColorProcress::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	pDC->FillSolidRect(&r,0xFFFFFF);
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CColorProcress::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		if (m_tDestroy && m_tDestroy < GetTickCount64())
		{	
			KillTimer(nIDEvent);
			m_tDestroy = 0;
		}

		if(m_bDoRedraw)
		{
#ifdef _DEBUG_CHECK_REDRAW
		_DEBUG_CHECK_REDRAW;
#endif
			RedrawWindow(0,0,RDW_INVALIDATE|RDW_UPDATENOW);
			m_bDoRedraw = false;
		}
	} else
		CStatic::OnTimer(nIDEvent);
}

int CColorProcress::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(1,40,0);

	return 0;
}

void CColorProcress::OnMove(int x, int y)
{
	if(m_pLock && *m_pLock)
		return;
	CStatic::OnMove(x, y);
}

void CColorProcress::OnMoving(UINT fwSide, LPRECT pRect)
{
	if(m_pLock && *m_pLock)
		return;
	CStatic::OnMoving(fwSide, pRect);
}

void CColorProcress::OnSizing(UINT fwSide, LPRECT pRect)
{
	if(m_pLock && *m_pLock)
		return;
	CStatic::OnSizing(fwSide, pRect);
}


const CColorProcress& CColorProcress::operator = (const CColorProcress & c)
{
	m_var= c.m_var;

	m_fontScale = c.m_fontScale;
	m_font2Scale = c.m_font2Scale;
	m_font2ScaleSet = c.m_font2ScaleSet;

	m_vFmt = c.m_vFmt;
	m_bInverse = c.m_bInverse;
	m_bSemaphore = c.m_bSemaphore;
	m_SemaphoreLevel = c.m_SemaphoreLevel;
	m_bAbsColor = c.m_bAbsColor;

	m_nAlign = c.m_nAlign;
	m_bRedraw = TRUE;
	m_bDoRedraw = TRUE;
	m_c0 = c.m_c0;
	m_c1 = c.m_c1;
	m_cf = c.m_cf;
	m_bStr = c.m_bStr;
	m_cx = c.m_cx;

	m_text = c.m_text;
	m_str = c.m_str;
	m_strTop = c.m_strTop;

	return *this;
}