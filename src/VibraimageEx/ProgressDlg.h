#pragma once
#include "afxcmn.h"


// CProgressDlg dialog

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProgressDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString	m_title;
	void *m_pUserData;
	int (*UserFn)(void *pUserData);
	int	m_result;
	int m_counter;
protected:
	CProgressCtrl	m_progress;
	CWinThread*		m_pThread;
protected:
	static UINT	ThreadProc(LPVOID p);

	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

};
