// ViHTTPLicenseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "ViHTTPLicenseDlg.h"
#include "afxdialogex.h"
#include "CDlgStarter.h"

// CViHTTPLicenseDlg dialog

IMPLEMENT_DYNAMIC(CViHTTPLicenseDlg, CDialogEx)

CViHTTPLicenseDlg::CViHTTPLicenseDlg(CWnd* pParent /*=NULL*/)
#ifdef SEQ_MEDIC_HT
	: CDialogEx(IDD_DIALOG_INETKEY_SIMPLE, pParent)
#else
	: CDialogEx(IDD_DIALOG_INETKEY, pParent)
#endif
	, m_strCode(_T(""))
	, m_strPwd(_T(""))
	, m_strAnswer(_T(""))
	, m_nRequest(0)
{
	m_strPwd = theApp.m_pEngine->engine.GetVar(VI_VAR_SEQ_PASSWORD,0,VT_BSTR).bstrVal;
	m_strAnswer = theApp.m_pEngine->engine.GetVar(VI_VAR_SEQ_ANSWER, 0, VT_BSTR).bstrVal;
	m_strCode = theApp.m_pEngine->engine.GetVar(VI_VAR_SEQ_CODE, 0, VT_BSTR).bstrVal;
}

CViHTTPLicenseDlg::~CViHTTPLicenseDlg()
{
	
}

void CViHTTPLicenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_REQUEST, m_strCode);
	DDX_Text(pDX, IDC_EDIT_CODE, m_strPwd);
	DDX_Text(pDX, IDC_EDIT_ANSWER, m_strAnswer);
}


BEGIN_MESSAGE_MAP(CViHTTPLicenseDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_GET_AUTHCODE_HTTP, &CViHTTPLicenseDlg::OnBnClickedBtnGetAuthcodeHttp)
	ON_BN_CLICKED(IDOK, &CViHTTPLicenseDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CViHTTPLicenseDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT_REQUEST, &CViHTTPLicenseDlg::OnEnChangeEditRequest)
	ON_EN_CHANGE(IDC_EDIT_ANSWER, &CViHTTPLicenseDlg::OnEnChangeEditRequest)
	ON_BN_CLICKED(IDC_BUTTON_WEB, &CViHTTPLicenseDlg::OnBnClickedButtonWeb)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CViHTTPLicenseDlg message handlers

void CViHTTPLicenseDlg::CheckHTTP(bool bOffline) {
	UpdateData();
	m_strCode.Trim(_T("\t \r\n*"));
	m_strAnswer.Trim(_T("\t \r\n*"));

	if (!bOffline)
		m_strAnswer.Empty();

	theApp.m_pEngine->engine.PutVar(VI_VAR_SEQ_ANSWER, 1, COleVariant((LPCTSTR)m_strAnswer));
	theApp.m_pEngine->engine.PutVar(VI_VAR_SEQ_CODE, 0, COleVariant((LPCTSTR)m_strCode));

	CString result = theApp.m_pEngine->engine.GetVar(VI_VAR_SEQ_REQUEST, 0, VT_BSTR).bstrVal;

	if (result != _T("0"))
	{
		if (m_nRequest != 0)
			m_strAnswer = theApp.m_pEngine->engine.GetLang()->Get(L"ERROR_LICENSE");
		else
			m_strAnswer = L"";
		m_strCode = L"";
		GetDlgItem(IDOK)->EnableWindow(false);
	}
	else
	{
		m_strPwd = theApp.m_pEngine->engine.GetVar(VI_VAR_SEQ_PASSWORD, 0, VT_BSTR).bstrVal;
		m_strAnswer = theApp.m_pEngine->engine.GetVar(VI_VAR_SEQ_ANSWER, 0, VT_BSTR).bstrVal;
		m_strCode = theApp.m_pEngine->engine.GetVar(VI_VAR_SEQ_CODE, 0, VT_BSTR).bstrVal;
		m_strCode.Trim(_T("\t \r\n*"));

		bool ok = theApp.m_pEngine->engine.GetSeq()->SeqIsOk();
		GetDlgItem(IDOK)->EnableWindow(ok && !m_strAnswer.IsEmpty());
	}
	UpdateData(FALSE);
	CheckRequest();
}

void CViHTTPLicenseDlg::OnBnClickedBtnGetAuthcodeHttp()
{
	m_nRequest++;
	UpdateData();
	m_strAnswer.Empty();
	UpdateData(FALSE);
	CheckHTTP(false);
}

void CViHTTPLicenseDlg::OnBnClickedButtonWeb()
{
	UpdateData();

	theApp.m_pEngine->engine.PutVar(VI_VAR_SEQ_ANSWER, 1, COleVariant((LPCTSTR)_T("*web")));
	theApp.m_pEngine->engine.PutVar(VI_VAR_SEQ_CODE, 0, COleVariant((LPCTSTR)m_strCode));

}

void CViHTTPLicenseDlg::OnBnClickedOk()
{
	UpdateData();

	BOOL ok = theApp.m_pEngine->engine.GetSeq()->SeqIsValidCode(m_strCode);

	if (ok && !GetDlgItem(IDC_BTN_GET_AUTHCODE_HTTP)->IsWindowVisible()) {
		CheckHTTP(false);

		if (m_strAnswer.IsEmpty() || m_strAnswer.Find(' ') >= 0)
			return;
	}
	else
	{
		theApp.m_pEngine->engine.PutVar(VI_VAR_SEQ_CODE, 1, COleVariant((LPCTSTR)m_strCode));
		theApp.m_pEngine->engine.PutVar(VI_VAR_SEQ_ANSWER, 0, COleVariant((LPCTSTR)m_strAnswer));
	}

	ok = theApp.m_pEngine->engine.GetSeq()->SeqIsOk();
	if (ok) {
		CDialogEx::OnOK();
		theApp.m_splashScreen.Start();
	}
	else
	{
		if (m_nRequest == 0)
			m_strCode = L"";
		else 
			m_strCode = theApp.m_pEngine->engine.GetLang()->Get(L"ERROR_LICENSE");

		m_strAnswer = L"";
		UpdateData(FALSE);
	}
	CheckRequest();
	m_nRequest++;
}


void CViHTTPLicenseDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
	theApp.m_splashScreen.Start();
}


void CViHTTPLicenseDlg::OnEnChangeEditRequest()
{
	UpdateData();
	CheckRequest();
}


BOOL CViHTTPLicenseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	theApp.m_splashScreen.Finish();

	CheckRequest();

	if (!GetDlgItem(IDC_BTN_GET_AUTHCODE_HTTP)->IsWindowVisible()) {
		if (m_strCode.Left(5) == _T("DEMO_")) {
			OnBnClickedOk();
		}
	}

	SetTimer(1, 500,0);
	return TRUE;  
}

void CViHTTPLicenseDlg::CheckRequest()
{
	BOOL ok = theApp.m_pEngine->engine.GetSeq()->SeqIsValidCode(m_strCode);
	
	if (GetDlgItem(IDC_BTN_GET_AUTHCODE_HTTP)->IsWindowVisible()) {
		GetDlgItem(IDOK)->EnableWindow(ok && !m_strAnswer.IsEmpty());

		GetDlgItem(IDC_BTN_GET_AUTHCODE_HTTP)->EnableWindow(ok && m_strAnswer.Find('/') < 0);
		GetDlgItem(IDC_BUTTON_WEB)->EnableWindow(ok && m_strCode.Left(5).CompareNoCase(_T("DEMO_")));
	}
	else {
		GetDlgItem(IDOK)->EnableWindow(ok);
	}
}



void CViHTTPLicenseDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		if (theApp.m_hWndStarter)
			::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_TEST, (LPARAM)m_hWnd);
	}

	CDialogEx::OnTimer(nIDEvent);
}
