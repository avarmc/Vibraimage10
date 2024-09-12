// CDlgStarter.cpp : implementation file
//

#include "stdafx.h"
#include "VibraimageEx.h"
#include "CDlgStarter.h"
#include "afxdialogex.h"
#include <psapi.h>

// CDlgStarter dialog

IMPLEMENT_DYNAMIC(CDlgStarter, CDialogEx)

CDlgStarter::CDlgStarter(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_STARTER, pParent)
{
	m_tStart = m_tCheck = GetTickCount64();
	m_bDelay = 0;
	m_hWndChild = 0;
	m_idChild = 0;
	ZeroMemory(&m_piChild, sizeof(m_piChild));
}

CDlgStarter::~CDlgStarter()
{
}

void CDlgStarter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgStarter, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgStarter::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgStarter message handlers


void CDlgStarter::OnBnClickedCancel()
{
	if (m_hWndChild)
		::PostMessage(m_hWndChild, WM_CLOSE, 0, 0);
	CDialogEx::OnCancel();
}


BOOL CDlgStarter::OnInitDialog()
{
	CSingleLock lock(&m_lock, true);

	CDialogEx::OnInitDialog();

	MoveWindow(-1, -1, 1, 1, FALSE);
	
	StartVI();

	SetTimer(1, 2000, 0);
	return FALSE;
}


void CDlgStarter::PreInitDialog()
{
	CDialogEx::PreInitDialog();
}


LRESULT CDlgStarter::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_MSG_STARTER) {
		return ProcessMsg(wParam, lParam);
	} 
	return CDialogEx::WindowProc(message, wParam, lParam);
}

LRESULT CDlgStarter::ProcessMsg(WPARAM wParam, LPARAM lParam) {
	CSingleLock lock(&m_lock, true);

	switch (wParam) {
	case ID_MSG_STARTER_TEST:
		m_tCheck = GetTickCount64();
		m_hWndChild = (HWND)lParam;
		break;
	case ID_MSG_STARTER_DELAY:
		m_bDelay = lParam;
		break;
	case ID_MSG_STARTER_START:
		m_tStart = m_tCheck = GetTickCount64();
		m_idChild = (DWORD)lParam;
		ShowWindow(SW_HIDE);
		break;
	case ID_MSG_STARTER_STOP:
		m_tCheck = GetTickCount64();
		KillTimer(1);
		OnCancel();
		break;
	case ID_MSG_STARTER_KILL:
		m_tCheck = GetTickCount64();
		KillTimer(1);
		Sleep(5000);
		theApp.stopProcess(m_piChild);
		OnCancel();
		break;
	}
	return 0;
}

void CDlgStarter::StartVI() {
	CSingleLock lock(&m_lock, true);

	CString strWnd,strCmd = L"/launcher";
	strWnd.Format(L"%llu", (ULONGLONG)m_hWnd);

	std::vector< LPCWSTR > cmd;
	cmd.push_back(strCmd);
	cmd.push_back(strWnd);

	m_piChild = theApp.Restart(false, &cmd);
}

bool CDlgStarter::CheckMem() {
	if (!(m_idChild && m_idChild == m_piChild.dwProcessId))
		return false;

	size_t nRam = 0,mb = 1024 * 1024;
	PROCESS_MEMORY_COUNTERS pmc;
	if (m_piChild.hProcess && GetProcessMemoryInfo(m_piChild.hProcess, &pmc, sizeof(pmc)))
	{
		nRam = max(nRam, pmc.PeakWorkingSetSize);
		nRam = max(nRam, pmc.WorkingSetSize);
		nRam = max(nRam, pmc.QuotaPeakPagedPoolUsage);
		nRam = max(nRam, pmc.QuotaPagedPoolUsage);
		nRam = max(nRam, pmc.QuotaPeakNonPagedPoolUsage);
		nRam = max(nRam, pmc.QuotaNonPagedPoolUsage);

		if (nRam > mb * 1024 * 40)
			return true;
	}
	return false;
}

void CDlgStarter::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		CSingleLock lock(&m_lock, true);

		if (!m_idChild || m_bDelay)
			m_tCheck = GetTickCount64();

		ULONGLONG dt = GetTickCount64() - m_tCheck;
		ULONGLONG dtStart = GetTickCount64() - m_tStart;
		ULONGLONG dtMax = 20000;
		if (!m_hWndChild || dtStart < 120000)
			dtMax = 120000;


		if(CheckMem())
			dtMax = 0;
		else
		if (m_idChild && m_idChild != m_piChild.dwProcessId)
			dtMax = 0;

		if (dt <= dtMax )
			return;

		if (m_idChild) {
				theApp.stopProcess(m_piChild);
				m_idChild = 0;
				m_hWndChild = 0;
				StartVI();
		}
	} else
		CDialogEx::OnTimer(nIDEvent);
}
