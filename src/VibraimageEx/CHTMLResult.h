#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif

// CHTMLResult dialog

class CHTMLResult : public CDHtmlDialog
{
//	DECLARE_DYNCREATE(CHTMLResult)

	static CString		m_path;
public:
	CHTMLResult(const CString& path,CWnd* pParent = nullptr);   // standard constructor
	virtual ~CHTMLResult();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

	static void CheckTimer();
	static void ShowResult(const CString& path);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HT_RESULT_HTML, IDH = 101 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
