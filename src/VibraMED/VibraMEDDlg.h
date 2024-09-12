
// VibraMEDDlg.h : header file
//

#pragma once

#include "CVibraMED.h"

// CVibraMEDDlg dialog
class CVibraMEDDlg : public CDialogEx
{
// Construction
public:
	CVibraMEDDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIBRAMED_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
	std::shared_ptr< CVibraMED >		m_pVI;
	CImage								m_video;
	int									m_nFrame;
	ULONGLONG							m_t0;
// Implementation
protected:
	HICON m_hIcon;
	CComboBox m_dlgDevices;
	CListCtrl m_dlgVars;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeComboDevices();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void InitDevices();
	void InitVars();

	void SetValue(int pos, const CString& name, const CString& v, const CString& m);
	CString f2str(float v);
	CString i2str(int v);

	void AddRndFrame();
	DECLARE_MESSAGE_MAP()
protected:
	void	UpdateVars();
public:

	BOOL m_dlgCalculation;
	afx_msg void OnBnClickedButtonCalculation();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonResetAll();
};
