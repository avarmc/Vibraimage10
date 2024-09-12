#pragma once


// CPwdDlg2 dialog

class CPwdDlg2 : public CDialogEx
{
	DECLARE_DYNAMIC(CPwdDlg2)

public:

	CPwdDlg2(int pwdID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPwdDlg2();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PWD2 };
#endif
	int	m_pwdID = 0;

	CString m_pwdCur;
	CString m_pwdNew1;
	CString m_pwdNew2;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void Check();
	afx_msg void OnBnClickedOk();
	
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditPwd();
	afx_msg void OnEnChangeEditPwd2();
	afx_msg void OnEnChangeEditPwd3();
};
