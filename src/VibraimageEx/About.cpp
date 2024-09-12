#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "VibraimageEx.h"
#include "About.h"
#include "VibraimageExFrame.h"

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_UNREGISTER, &CAboutDlg::OnBnClickedButtonUnregister)
END_MESSAGE_MAP()



BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int bLimit = theApp.m_pEngine->engine.GetSeq()->SeqIsLimit();
	int bDemo = theApp.m_pEngine->engine.GetSeq()->SeqIsDemo();

	CString seqAnswer = theApp.m_pEngine->engine.GetVar(VI_VAR_SEQ_ANSWER,0,VT_BSTR).bstrVal;
	CString seqOwner = CString(theApp.m_pEngine->engine.GetLang()->Get(L"SEQ_OWNER")) + L"  " + theApp.m_pEngine->engine.GetSeq()->SeqOwner();
	CString seqSerial = CString(theApp.m_pEngine->engine.GetLang()->Get(L"SEQ_SERIAL")) + L"  " + theApp.m_pEngine->engine.GetSeq()->SeqSerial();

	CString seqType = CString(theApp.m_pEngine->engine.GetLang()->Get(L"SEQ_TYPE")) + L"  " +
		(bDemo ? theApp.m_pEngine->engine.GetLang()->Get(L"SEQ_DEMO") : theApp.m_pEngine->engine.GetLang()->Get(L"SEQ_PROF"));
		
	if (bLimit)
	{
		seqType += CString(_T(". ")) + theApp.m_pEngine->engine.GetLang()->Get(L"SEQ_LIMITED") + _T(": ") + theApp.m_pEngine->engine.GetSeq()->SeqLimit();
	}

	SetWindowText( theApp.m_pEngine->engine.GetLang()->Get(L"MENU_HELP_ABOUT"));
	CString name = CString( theApp.m_pEngine->engine.GetLang()->Get(L"STR_FRAME_CAPTION") );
	
	
	GetDlgItem(IDC_ABOUT0)->SetWindowText(name+ GetVersionInfo());
	GetDlgItem(IDC_ABOUT1)->SetWindowText(seqOwner);
	GetDlgItem(IDC_ABOUT2)->SetWindowText(seqSerial);
	GetDlgItem(IDC_ABOUT3)->SetWindowText(seqType);

	GetDlgItem(IDC_BUTTON_UNREGISTER)->ShowWindow((!theApp.m_pEngine->engine.GetSeq()->SeqIsUnreg() || seqAnswer.IsEmpty() || bLimit || seqAnswer.Find('/') < 0 || bDemo ) ? SW_HIDE : SW_SHOW);
	GetDlgItem(IDC_BUTTON_UNREGISTER)->SetWindowText(theApp.m_pEngine->engine.GetLang()->Get(L"BUTTON_UNREGISTER"));

	CString strCopyright;
	GetDlgItem(IDC_STATIC_COPYRIGHT)->GetWindowText(strCopyright);
	GetDlgItem(IDC_STATIC_COPYRIGHT)->SetWindowText(theApp.m_pEngine->engine.GetLang()->Rename(strCopyright));
	return TRUE;
}


void CAboutDlg::OnBnClickedButtonUnregister()
{
	if (theApp.m_pEngine->engine.StopAndUnregister())
	{
		OnCancel();
		theApp.m_pFrame->SetTimer(CVibraimageExFrame::T_CLOSE,2000,0);
	}
}

CString  CAboutDlg::GetVersionInfo() {
	CString str;
	TCHAR fileName[MAX_PATH + 100] = { 0 };
	GetModuleFileName(NULL, fileName, MAX_PATH + 1);

#if defined SEQ_VIBRAIMAGE_PRO_KZ
	CFileStatus fs;
	CString path = fileName;
	path = path.Left(path.ReverseFind('\\')) + L"\\ViEngineEx.dll";
	if (!CFile::GetStatus(path, fs))
		str = L" ver. 1.0";
	else
		str.Format(L" ver. 1.0/ (%s)", (LPCTSTR)fs.m_mtime.Format(L"%Y-%m-%d %H:%M"));

#else
	int major,minor,build,revision;
	if (!GetVersionInfo(fileName, major, minor, build, revision))
		return _T("");
	str.Format(_T(" %d.%d.%d.%d"), major, minor, build, revision);
#endif
	return str;
}

bool CAboutDlg::GetVersionInfo(
	LPCTSTR filename,
	int &major,
	int &minor,
	int &build,
	int &revision)
{
	DWORD   verBufferSize;
	char    verBuffer[2048];

	//  Get the size of the version info block in the file
	verBufferSize = GetFileVersionInfoSize(filename, NULL);
	if (verBufferSize > 0 && verBufferSize <= sizeof(verBuffer))
	{
		//  get the version block from the file
		if (TRUE == GetFileVersionInfo(filename, NULL, verBufferSize, verBuffer))
		{
			UINT length;
			VS_FIXEDFILEINFO *verInfo = NULL;

			//  Query the version information for neutral language
			if (TRUE == VerQueryValue(
				verBuffer,
				_T("\\"),
				reinterpret_cast<LPVOID*>(&verInfo),
				&length))
			{
				//  Pull the version values.
				major = HIWORD(verInfo->dwProductVersionMS);
				minor = LOWORD(verInfo->dwProductVersionMS);
				build = HIWORD(verInfo->dwProductVersionLS);
				revision = LOWORD(verInfo->dwProductVersionLS);
				return true;
			}
		}
	}

	return false;
}
