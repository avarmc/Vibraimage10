// UserIdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "UserIdDlg.h"
#include "afxdialogex.h"


// CUserIdDlg dialog

IMPLEMENT_DYNAMIC(CUserIdDlg, CDialogEx)

CUserIdDlg::CUserIdDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ID, pParent)
	, m_id(_T(""))
{

}

CUserIdDlg::~CUserIdDlg()
{
}

void CUserIdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ID, m_id);
}


BEGIN_MESSAGE_MAP(CUserIdDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CUserIdDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CUserIdDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT_ID, &CUserIdDlg::OnEnChangeEditId)
END_MESSAGE_MAP()


// CUserIdDlg message handlers


void CUserIdDlg::OnBnClickedOk()
{
	if( Check() )
		CDialogEx::OnOK();
}


void CUserIdDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CUserIdDlg::OnEnChangeEditId()
{
	Check();
}


BOOL CUserIdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	Check();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

bool CUserIdDlg::Check()
{
	UpdateData();
	bool ok = !m_id.IsEmpty();
	GetDlgItem(IDOK)->EnableWindow(!!ok);

	return ok;
}