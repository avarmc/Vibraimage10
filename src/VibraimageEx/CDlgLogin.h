#pragma once


// CDlgLogin dialog

class CDlgLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgLogin();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGIN };
#endif
	CString m_pwd,m_login;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
