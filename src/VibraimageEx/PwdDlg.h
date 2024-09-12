#pragma once


// CPwdDlg dialog

class CPwdDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPwdDlg)

public:
	int	m_pwdID = 0;

	CPwdDlg(int pwdID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CPwdDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PWD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	static bool Check(int pwdID = VI_INFO_START_PWD_OK);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CString m_pwd;
	virtual BOOL OnInitDialog();
};
