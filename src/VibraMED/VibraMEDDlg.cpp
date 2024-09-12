
// VibraMEDDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "VibraMED.h"
#include "VibraMEDDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVibraMEDDlg dialog


CVibraMEDDlg::CVibraMEDDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIBRAMED_DIALOG, pParent)
	, m_dlgCalculation(FALSE)
	, m_nFrame(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_t0 = GetTickCount64();
}

void CVibraMEDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICES, m_dlgDevices);
	DDX_Control(pDX, IDC_LIST_VARS, m_dlgVars);
	DDX_Check(pDX, IDC_BUTTON_CALCULATION, m_dlgCalculation);
}

BEGIN_MESSAGE_MAP(CVibraMEDDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CVibraMEDDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CVibraMEDDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICES, &CVibraMEDDlg::OnCbnSelchangeComboDevices)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CALCULATION, &CVibraMEDDlg::OnBnClickedButtonCalculation)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CVibraMEDDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_RESET_ALL, &CVibraMEDDlg::OnBnClickedButtonResetAll)
END_MESSAGE_MAP()


// CVibraMEDDlg message handlers

BOOL CVibraMEDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_pVI = std::make_shared<CVibraMED>();

	InitDevices();
	InitVars();

	SetTimer(1, 40, 0);
//	SetTimer(2, 2000, 0);

	// select first device
	m_dlgDevices.SetCurSel(0);
	OnCbnSelchangeComboDevices();

	HWND hWndHist = m_pVI->getHistWnd();
	if (hWndHist) {
		::SetParent(hWndHist, m_hWnd);
		::MoveWindow(hWndHist, 10, 400, 500, 250,FALSE);
		::ShowWindow(hWndHist, SW_SHOW);
	}
		
	HWND hWndMain = m_pVI->m_pEngine->GetWindow();
	if (hWndHist) {
		::SetParent(hWndMain, m_hWnd);
		::MoveWindow(hWndMain, 520, 10, 320, 240, FALSE);
		::ShowWindow(hWndMain, SW_SHOW);
	}
	m_pVI->m_pEngine->PutVar(VI_MODE_B, 0, COleVariant((long)1));
	m_pVI->m_pEngine->PutVar(VI_MODE_AURA, 0, COleVariant((long)VI_RESULT_SRC_B));
	m_pVI->m_pEngine->PutVar(VI_MODE_RESULT, 0, COleVariant((long)VI_RESULT_SRC_B));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVibraMEDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVibraMEDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVibraMEDDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CVibraMEDDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}



void CVibraMEDDlg::InitVars() {
	CRect rc;
	m_dlgVars.GetClientRect(&rc);

	m_dlgVars.InsertColumn(0, _T("Tag"));
	m_dlgVars.InsertColumn(1, _T("Value"));
	m_dlgVars.InsertColumn(2, _T("Average"));

	m_dlgVars.SetColumnWidth(0, rc.Width() / 3);
	m_dlgVars.SetColumnWidth(1, rc.Width() / 3);
	m_dlgVars.SetColumnWidth(2, rc.Width() / 3);

	m_dlgVars.ModifyStyleEx(0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CVibraMEDDlg::InitDevices() {
	std::vector<CString> devices = m_pVI->getCameras();

	// Add video sources
	for (size_t i = 0; i < devices.size(); ++i)
		m_dlgDevices.InsertString((int)i,devices[i]);

	// Add File source
	m_dlgDevices.InsertString(m_dlgDevices.GetCount(), _T("*file"));
	
	// Add Custom source
	m_dlgDevices.InsertString(m_dlgDevices.GetCount(), _T("*random"));
}

void CVibraMEDDlg::OnCbnSelchangeComboDevices()
{
	int nSelected = m_dlgDevices.GetCurSel();

	if (nSelected < 0) { // Empty device
		KillTimer(3);
		m_pVI->selectVideoSource(_T(""), false);
	} else if( nSelected == m_dlgDevices.GetCount()-2) // file source
	{
		CFileDialog dlg(TRUE);
		if (dlg.DoModal() == IDOK) {
			KillTimer(3);
			m_pVI->selectVideoSource(dlg.GetPathName(), true);
		}

	}
	else if (nSelected == m_dlgDevices.GetCount() - 1) // rnd source
	{
		m_pVI->selectVideoSource(_T(""), false);
		m_t0 = GetTickCount64();
		SetTimer(3, 100, 0);
	}
	else { // device name
		KillTimer(3);

		CString name;
		m_dlgDevices.GetLBText(nSelected, name);
		m_pVI->selectVideoSource(name, false);
	}

	m_nFrame = -1;
}


void CVibraMEDDlg::OnTimer(UINT_PTR nIDEvent)
{
	static int flag = 0;

	if (nIDEvent == 1) {
		UpdateVars();
	} else
	if (nIDEvent == 2) {
		if (flag % 2 == 0) {
			m_pVI->startCalculation();
		} else
			m_pVI->stopCalculation();

		flag++;
	} else
	if (nIDEvent == 3) {
		AddRndFrame();
	} else
		CDialogEx::OnTimer(nIDEvent);
}



void	CVibraMEDDlg::UpdateVars() {
	int nFrame = m_pVI->getI(VI_VAR_NFRAME_IN); 
	if (nFrame == m_nFrame) // do nothing
		return;
	m_nFrame = nFrame;

	int pos = 0;
	m_dlgVars.LockWindowUpdate();
	m_dlgCalculation = m_pVI->isCalculationStarted() ? TRUE : FALSE;

	SetValue(pos++, _T("size"), i2str(m_pVI->getI(VI_VAR_SIZE,0,1)) + _T("x") + i2str(m_pVI->getI(VI_VAR_SIZE, 0, 2)),_T(""));
	SetValue(pos++, _T("fps(in/out)"), f2str(m_pVI->getF(VI_VAR_FPSIN)) + _T("/") + f2str(m_pVI->getF(VI_VAR_FPSOUTR)), _T(""));
	SetValue(pos++, _T("N"), i2str(m_pVI->getI(VI_VAR_N0_RQST)), _T(""));

	bool bCalculationReady = m_pVI->isCalculationReady();

	for (int n = 0; n < 12; ++n) {
		CString current = f2str(m_pVI->getT12_value(n));
		CString measure = (m_dlgCalculation || !bCalculationReady) ? _T("") : f2str(m_pVI->getT12_measure(n));
		SetValue(pos++, m_pVI->getT12_name(n), current, measure);
	}
	m_dlgVars.UnlockWindowUpdate();

	
	UpdateData(FALSE);
}

CString CVibraMEDDlg::i2str(int v) {
	CString str;
	str.Format(_T("%0d"), v);
	return str;
}

CString CVibraMEDDlg::f2str(float v) {
	CString str;
	str.Format(_T("%0.2f"), v);
	return str;
}

void CVibraMEDDlg::SetValue(int pos, const CString& name, const CString& v, const CString& m) {
	bool bNew = false;
	while (m_dlgVars.GetItemCount() <= pos) {
		bNew = true;
		m_dlgVars.InsertItem(m_dlgVars.GetItemCount(), _T(""));
	}
	if(bNew)
		m_dlgVars.SetItemText(pos, 0, name);

	m_dlgVars.SetItemText(pos, 1, v);
	m_dlgVars.SetItemText(pos, 2, m);
}

void CVibraMEDDlg::OnBnClickedButtonCalculation()
{
	if (m_pVI->isCalculationStarted()) {
		m_pVI->stopCalculation();
		GetDlgItem(IDC_BUTTON_CALCULATION)->SetWindowText(_T("Start calculation"));
	}
	else {
		m_pVI->startCalculation();
		GetDlgItem(IDC_BUTTON_CALCULATION)->SetWindowText(_T("Stop calculation"));
	}
}


void CVibraMEDDlg::OnBnClickedButtonReset()
{
	m_pVI->clear();

	m_pVI->m_pEngine->GetReg()->LoadSettings(L"C:\\ELSYS\\Vibraimage10\\defaults\\mix.xml");
}


void CVibraMEDDlg::OnBnClickedButtonResetAll()
{
	m_pVI->stopCalculation();

	m_pVI = nullptr;
	m_pVI = std::make_shared<CVibraMED>();
	OnCbnSelchangeComboDevices();


	HWND hWndHist = m_pVI->getHistWnd();
	if (hWndHist) {
		::SetParent(hWndHist, m_hWnd);
		::MoveWindow(hWndHist, 10, 400, 500, 250, FALSE);
		::ShowWindow(hWndHist, SW_SHOW);
	}

}

void CVibraMEDDlg::AddRndFrame() {
	int w = 640, h = 480;
	std::vector<RGBTRIPLE> img((size_t)(w * h) );
	// fill frame by custom data
	for (auto& i : img) {
		i.rgbtBlue = rand();
		i.rgbtGreen = rand();
		i.rgbtRed = rand();
	}
	// send frame to engine
#if defined VIENGINEEX_GET_ENGINE
	m_pVI->m_pEngine->GetEngine()->AddImage( &img[0], w,h,24, (GetTickCount64() - m_t0) / 1000.0f );
#endif // #if defined VIENGINEEX_GET_ENGINE
}