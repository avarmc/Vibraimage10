// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Vibraimageex.h"
#include "ProgressDlg.h"


// CProgressDlg dialog

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	m_pUserData = 0;
	UserFn = 0;
	m_pThread = 0;
	m_result = 0;
	m_counter = 0;
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CProgressDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CProgressDlg::OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CProgressDlg message handlers

void CProgressDlg::OnBnClickedCancel()
{
	if(!m_pThread)
		OnCancel();
}

void CProgressDlg::OnBnClickedOk()
{
	if(!m_pThread)
		OnOK();
}

BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_title);

	m_progress.SetRange32(0,100);

	SetTimer(1,50,0);

	m_pThread = AfxBeginThread(ThreadProc,this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

UINT	CProgressDlg::ThreadProc(LPVOID p)
{

	CProgressDlg* pThis = (CProgressDlg*)p;
	
	while(!pThis->m_counter)
		Sleep(5);

	if(pThis->UserFn)
		pThis->m_result = pThis->UserFn(pThis->m_pUserData);

	pThis->m_pThread = 0;
	return 0;
}

void CProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		int v1=0,v2=0;
		m_progress.GetRange(v1,v2);
		m_progress.SetPos((m_progress.GetPos()+1)%v2);
		if(!m_pThread)
		{
			KillTimer(1);
			OnBnClickedOk();
		}
	} else
		CDialog::OnTimer(nIDEvent);
}


