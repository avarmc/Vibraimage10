// PwdDlg2.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "PwdDlg2.h"
#include "afxdialogex.h"


// CPwdDlg2 dialog

IMPLEMENT_DYNAMIC(CPwdDlg2, CDialogEx)

CPwdDlg2::CPwdDlg2(int pwdID, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_PWD2, pParent)
	, m_pwdCur(_T(""))
	, m_pwdNew1(_T(""))
	, m_pwdNew2(_T(""))
	, m_pwdID(pwdID)
{
	theApp.m_tRegLocked = -1;
}

CPwdDlg2::~CPwdDlg2()
{
	theApp.m_tRegLocked = GetTickCount64();
}

void CPwdDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PWD, m_pwdCur);
	DDX_Text(pDX, IDC_EDIT_PWD2, m_pwdNew1);
	DDX_Text(pDX, IDC_EDIT_PWD3, m_pwdNew2);
}


BEGIN_MESSAGE_MAP(CPwdDlg2, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPwdDlg2::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_PWD, &CPwdDlg2::OnEnChangeEditPwd)
	ON_EN_CHANGE(IDC_EDIT_PWD2, &CPwdDlg2::OnEnChangeEditPwd2)
	ON_EN_CHANGE(IDC_EDIT_PWD3, &CPwdDlg2::OnEnChangeEditPwd3)
END_MESSAGE_MAP()


// CPwdDlg2 message handlers


void CPwdDlg2::OnBnClickedOk()
{
	theApp.m_pEngine->engine.PutVar(m_pwdID-2, 0, COleVariant((LPCTSTR)m_pwdNew1));
	CDialogEx::OnOK();
}


BOOL CPwdDlg2::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	GetDlgItem(IDOK)->SetWindowText(theApp.m_pEngine->engine.GetLang()->Get(_T("BUTTON_OK")));
	GetDlgItem(IDC_STATIC_PWD1)->SetWindowText(theApp.m_pEngine->engine.GetLang()->Get(_T("VI_INFO_PWD0")));
	GetDlgItem(IDC_STATIC_PWD2)->SetWindowText(theApp.m_pEngine->engine.GetLang()->Get(_T("VI_INFO_PWD1")));
	GetDlgItem(IDC_STATIC_PWD3)->SetWindowText(theApp.m_pEngine->engine.GetLang()->Get(_T("VI_INFO_PWD2")));
	SetWindowText(theApp.m_pEngine->engine.GetLang()->Get(_T("VI_INFO_PWD_NEW")));

	theApp.m_pEngine->engine.PutVar(m_pwdID-1, 0, COleVariant((LPCTSTR)m_pwdCur));

	Check();

	return TRUE;  
}

void CPwdDlg2::Check() {
	UpdateData();

	GetDlgItem(IDOK)->EnableWindow(m_pwdNew1 == m_pwdNew2 && theApp.m_pEngine->engine.GetVar(m_pwdID, 0, VT_INT).intVal);
}

void CPwdDlg2::OnEnChangeEditPwd()
{
	UpdateData();
	theApp.m_pEngine->engine.PutVar(m_pwdID-2, 0, COleVariant((LPCTSTR)m_pwdCur));
	Check();
}


void CPwdDlg2::OnEnChangeEditPwd2()
{
	Check();
}


void CPwdDlg2::OnEnChangeEditPwd3()
{
	Check();
}
