#pragma once


// CUserIdDlg dialog

class CUserIdDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserIdDlg)

public:
	CUserIdDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUserIdDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ID };
#endif
public:
	CString m_id;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	bool Check();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEditId();
	virtual BOOL OnInitDialog();
};
