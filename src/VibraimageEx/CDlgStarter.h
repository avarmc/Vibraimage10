#pragma once

#define WM_MSG_STARTER (WM_USER+6)
#define ID_MSG_STARTER_TEST 1
#define ID_MSG_STARTER_START 2
#define ID_MSG_STARTER_STOP 3
#define ID_MSG_STARTER_KILL 5
#define ID_MSG_STARTER_DELAY 4

// CDlgStarter dialog

class CDlgStarter : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStarter)
	CCriticalSection	m_lock;
	ULONGLONG			m_tStart;
public:
	CDlgStarter(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgStarter();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_STARTER };
#endif
protected:
	PROCESS_INFORMATION	m_piChild;
	DWORD			m_idChild;
	HWND			m_hWndChild;
	ULONGLONG		m_tCheck;
	BOOL			m_bDelay;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	LRESULT ProcessMsg(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	virtual void PreInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void StartVI();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	bool CheckMem();
public:



};
