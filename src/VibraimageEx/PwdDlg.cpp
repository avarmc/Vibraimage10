// PwdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "PwdDlg.h"
#include "afxdialogex.h"


// CPwdDlg dialog

IMPLEMENT_DYNAMIC(CPwdDlg, CDialogEx)

CPwdDlg::CPwdDlg(int pwdID, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_PWD, pParent)
	, m_pwd(_T(""))
	, m_pwdID(pwdID)
{
	theApp.m_tRegLocked = -1;
}

CPwdDlg::~CPwdDlg()
{
	theApp.m_tRegLocked = GetTickCount64();
}

void CPwdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PWD, m_pwd);
}


BEGIN_MESSAGE_MAP(CPwdDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPwdDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPwdDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CPwdDlg message handlers


void CPwdDlg::OnBnClickedOk()
{
	UpdateData();

	theApp.m_pEngine->engine.PutVar(m_pwdID-1, 0, COleVariant(m_pwd));

	if( theApp.m_pEngine->engine.GetVar(m_pwdID,0, VT_INT).intVal )
		CDialogEx::OnOK();
}


void CPwdDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


BOOL CPwdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


bool CPwdDlg::Check(int pwdID)
{
	if (theApp.m_pEngine->engine.GetVar(pwdID, 0, VT_INT).intVal)
		return true;

	CPwdDlg dlg(pwdID);
	return (dlg.DoModal() == IDOK);
}