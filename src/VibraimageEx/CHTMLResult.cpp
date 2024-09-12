// CHTMLResult.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExFrame.h"
#include "CHTMLResult.h"

CString		CHTMLResult::m_path;
// CHTMLResult dialog

//IMPLEMENT_DYNCREATE(CHTMLResult, CDHtmlDialog)

CHTMLResult::CHTMLResult(const CString& path, CWnd* pParent)
	: CDHtmlDialog(IDD_HT_RESULT_HTML, 0, pParent)
{
	m_path = path;
}

CHTMLResult::~CHTMLResult()
{
}

void CHTMLResult::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CHTMLResult::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	Navigate(CString("file://") + m_path);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CHTMLResult, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CHTMLResult)
	DHTML_EVENT_ONCLICK(_T("close"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CHTMLResult message handlers

HRESULT CHTMLResult::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CHTMLResult::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}


void CHTMLResult::CheckTimer() {
	if (!theApp.m_pEngine)
		return;
	if(!theApp.m_pEngine->engine.GetDB())
		return;
	if(theApp.m_pEngine->engine.GetDB()->IsStarted())
		return;
	if (!theApp.m_pEngine->engine.GetSeq()->SeqIsInit())
		return;
	if (theApp.m_pEngine->engine.GetSeq()->SeqIsDemo())
		return;
	
	int n = theApp.m_pEngine->engine.GetDB()->GetLastSessionFiles(-1, 0, 0);
	if (!n)
		return;
	for(int i = 0;i < n; ++i) {
		WCHAR path[MAX_PATH * 2] = { 0 };
		theApp.m_pEngine->engine.GetDB()->GetLastSessionFiles(i,path, MAX_PATH * 2);
		CString sPath = path;
		if (sPath.Right(7) == _T("_M.html") && sPath != m_path) {
			ShowResult(sPath);
			return;
		}
	}
}

void CHTMLResult::ShowResult(const CString& path) {
	CHTMLResult dlg(path, theApp.m_pFrame);
	dlg.DoModal();
}

