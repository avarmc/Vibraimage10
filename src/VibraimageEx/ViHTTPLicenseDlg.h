#pragma once


// CViHTTPLicenseDlg dialog

class CViHTTPLicenseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CViHTTPLicenseDlg)
	DECLARE_MESSAGE_MAP()
public:
	CString m_strPwd;
	CString m_strCode;
	CString m_strAnswer;
	int		m_nRequest;
public:
	CViHTTPLicenseDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CViHTTPLicenseDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INETKEY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void CheckRequest();
	void CheckHTTP(bool bOffline);
public:
	afx_msg void OnBnClickedBtnGetAuthcodeHttp();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEditRequest();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonWeb();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
