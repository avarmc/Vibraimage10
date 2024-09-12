// CDlgLogin.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "CDlgLogin.h"
#include "afxdialogex.h"


// CDlgLogin dialog

IMPLEMENT_DYNAMIC(CDlgLogin, CDialogEx)

CDlgLogin::CDlgLogin(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOGIN, pParent)
{
	m_login = theApp.m_pEngine->engine.GetVar(VI_INFO_SITE_LOGIN, 0, VT_BSTR).bstrVal;
}

CDlgLogin::~CDlgLogin()
{
}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PWD, m_pwd);
	DDX_Text(pDX, IDC_EDIT_LOGIN, m_login);
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgLogin::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLogin::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgLogin message handlers

void CDlgLogin::OnBnClickedOk()
{
	UpdateData();

	theApp.m_pEngine->engine.PutVar(VI_INFO_SITE_LOGIN, 0, m_login);
	theApp.m_pEngine->engine.PutVar(VI_INFO_SITE_PWD, 0, m_pwd);

	CString answer = theApp.m_pEngine->engine.GetVar(VI_VAR_SEQ_ANSWER, 0, VT_BSTR).bstrVal;

	if(answer == L"OK")
		CDialogEx::OnOK();
	else {
		MessageBeep(MB_ICONERROR);
	}
}


void CDlgLogin::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


BOOL CDlgLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CString title;
	GetWindowText(title);
	title = CString( theApp.m_pEngine->engine.GetLang()->Get(L"STR_FRAME_CAPTION") ) + L" " + title;
	SetWindowText(title);
	ModifyStyleEx(0, WS_EX_APPWINDOW);
	if (!m_login.IsEmpty())
		GetDlgItem(IDC_EDIT_PWD)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
