#include "stdafx.h"
#include "VibraimageEx.h"
#include "ViEngineExProc.h"
#include "VibraimageExFrame.h"
#include "VibraimageExView.h"
#include "VibraimageExGraph.h"
#include <Mmsystem.h>
#include "CDlgStarter.h"

CViEngineExProc::CViEngineExProc():m_id(0)
{
}


CViEngineExProc::~CViEngineExProc()
{
}

void CViEngineExProc::OnNewVar(int id, int subID)
{
	if (theApp.m_bDone || theApp.m_bAbort)
		return;

	theApp.PushVar(id, subID, m_id);
	if(m_support) m_support->OnNewVar(id, subID);
}

void  CViEngineExProc::Connect(int core, CVIENGINE_EX_CLASS* pApp) { 
	m_id = core;
	CViEngineExBaseInterface::Connect(pApp);
	m_support = std::make_shared< CViEngineExSupport >(core);
}

bool CViEngineExProc::IsOptionSet(LPCWSTR key, int shift)
{
//	if (!wcscmp(key, L"/video_compat"))
//		return true;
	return theApp.m_cmdl.IsSet(key, shift);
}

bool CViEngineExProc::GetOption(LPCWSTR key, int shift,LPWSTR param,int cch)
{
	if (param)
		* param = 0;

	if (!key) {
		CString prm = theApp.m_cmdl.GetParam(shift);
		if (prm.IsEmpty())
			return false;
		if (param)
			_tcsncpy_s(param, cch, prm, cch);
		return true;
	}

	if (!IsOptionSet(key, shift))
		return false;

	if(param)
		_tcsncpy_s(param, cch, theApp.m_cmdl.GetParam(key, shift), cch);

	return true;
}

bool CViEngineExProc::OnResultAlign(int idr, HWND hWnd, DWORD * pBits, int w, int h)
{
	if (m_id != theApp.m_pEngine->id)
		return false;

	bool ok =  __super::OnResultAlign(idr, hWnd, pBits, w, h);

	if (!ok || !hWnd)
		m_wndRects.erase(idr);
	else
		::GetWindowRect(hWnd, &m_wndRects[idr]);
	return ok;
}

void CViEngineExProc::OnCloseRequest() {
	__super::OnCloseRequest();

	if (theApp.m_hWndStarter)
		::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_DELAY, 1);

	if (!m_bCloseRqst) {
		theApp.m_pFrame->SetTimer(CVibraimageExFrame::T_CLOSE_RQST, 1000, 0);
		m_bCloseRqst = true;
	}
	//	theApp.m_pFrame->PostMessage(WM_CLOSE, 0, 0);
}

int	CViEngineExProc::FindResult(CPoint p, CRect* pr)
{
	std::map< int, CRect >::iterator i, ie;
	for (i = m_wndRects.begin(), ie = m_wndRects.end(); i != ie; ++i)
		if (i->second.PtInRect(p)) {
			if (pr)
				*pr = i->second;
			return i->first;
		}
	if (pr)
		pr->SetRectEmpty();

	return 0;
}

void CViEngineExProc::OnFaceDetected(VI_FACE_INFO_DATA *pInfo)
{
	if (theApp.m_bDone || theApp.m_bAbort || theApp.m_bCloseRqst)
		return;
	theApp.m_pEngines[m_id]->pFtp->OnFaceDetected(pInfo);
	theApp.m_pEngines[m_id]->pLog->OnFaceDetected(pInfo);
	if (m_support) m_support->OnFaceDetected(pInfo->n,pInfo->r);
}

void CViEngineExProc::OnPlaySound(int id, float v, float lev, LPCWSTR path, ULONGLONG t)
{
	if (m_id != theApp.m_pEngine->id)
		return;
	if (path && path[0] && (!id || (GetTickCount64()-t) < 500 ))
		PlaySound(path, 0, SND_FILENAME);
}

void CViEngineExProc::OnLoginWindow(bool bShow) {
	if (bShow)
		theApp.m_splashScreen.Finish();
	else
		theApp.m_splashScreen.Start();
}

LPCSTR CViEngineExProc::molMakePreFileData(LPCSTR strData, LPCSTR strBoundary, LPCSTR strFileName) {
	bool bNeedNewData = false; // fix it

	if (!bNeedNewData)
		return strData;

	m_molData = strData;
	// ... update 'm_molData'

	return m_molData;
}

LPCWSTR CViEngineExProc::molMakeUrl(LPCWSTR strUrl) {
	bool bNeedNewData = false; // fix it

	if (!bNeedNewData)
		return strUrl;

	m_molUrl = strUrl;
	// ... update 'm_molUrl'

	return m_molUrl;
}

void CViEngineExProc::molResponse(LPCSTR strResponse) {
	// AfxMessageBox(CString(strResponse));
}

void CViEngineExProc::OnError(int id, const WORD a_type, LPCWSTR msg) {
	theApp.OnError(id,a_type, msg);
}


bool CViEngineExProc::GetVideoChannel(LPWSTR name, long cch_max) {
	return false;
}

int  CViEngineExProc::OnVideoFrame(double dblSampleTime, BYTE* pBuffer, long lBufferSize, void* pFormat) {
	if (theApp.m_bAbort || !theApp.m_bInit || theApp.m_bCloseRqst)
		return 0;
	if (m_support) {
		BITMAPINFOHEADER* pih = (BITMAPINFOHEADER*)pFormat;
		int w = pih->biWidth;
		int h = pih->biHeight;
		int bpp = pih->biBitCount;
		m_support->OnFrameImage(dblSampleTime, L"INPUT_VIDEO", pBuffer, lBufferSize, w, h, bpp);
	}
	return  __super::OnVideoFrame(dblSampleTime, pBuffer, lBufferSize, pFormat);
}

int  CViEngineExProc::OnAudioFrame(double dblSampleTime, BYTE* pBuffer, long lBufferSize, void* pFormat) {
	if (theApp.m_bAbort || !theApp.m_bInit || theApp.m_bCloseRqst)
		return 0;
	return  __super::OnAudioFrame(dblSampleTime, pBuffer, lBufferSize, pFormat);
}

COLORREF CViEngineExProc::GetWndColor(int id) {
	/*
	static std::map<int, COLORREF> clr = {

	{ (int)CLR_GrayedText, GetGlobalData()->clrGrayedText },
	{ (int)CLR_BarShadow, GetGlobalData()->clrBarShadow },
	{ (int)CLR_BtnShadow, GetGlobalData()->clrBtnShadow },
	{ (int)CLR_ActiveCaption, (COLORREF)GetSysColor(COLOR_ACTIVECAPTION) },
	{ (int)CLR_Hilite, (COLORREF)GetSysColor(COLOR_HIGHLIGHT) },
	{ (int)CLR_Window,	(COLORREF)GetSysColor(COLOR_WINDOW) },
	{ (int)CLR_FillName , (COLORREF)GetSysColor(COLOR_WINDOW)},
	{ (int)CLR_FillValue , (COLORREF)GetSysColor(COLOR_WINDOW)},
	{ (int)CLR_TextName , (COLORREF)GetSysColor(COLOR_WINDOWTEXT)},
	{ (int)CLR_TextValue, (COLORREF)GetSysColor(COLOR_WINDOWTEXT) }

	};
	auto i = clr.find(id);
	if(i != clr.end())
		return i->second;
*/

	return __super::GetWndColor(id);
}

HWND CViEngineExProc::GetMainWnd() {
	return theApp.m_pFrame->m_hWnd;
}


void CViEngineExProc::Log(LPCWSTR msg) {
	if (theApp.m_bDone || theApp.m_bAbort || theApp.m_bCloseRqst)
		return;
	auto pl = theApp.m_pEngines[m_id]->pLog;
	if (pl)
		pl->Log(msg);
}