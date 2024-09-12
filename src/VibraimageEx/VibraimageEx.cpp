
// VibraimageEx.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "VibraimageEx.h"
#include "VibraimageExFrame.h"
#include "AMCap.h"
#include "VibraimageExDoc.h"
#include "VibraimageExView.h"
#include "PropertiesPack.h"
#include <Mmsystem.h>

#include "VibraimageExGraph.h"
#include "VibraimageExAi.h"
#include "VibraimageExHist.h"
#include "VibraimageExLD.h"
#include "VibraimageExLDR.h"
#include "VibraimageExQT.h"
#include "VibraimageExAIV.h"
#include "VibraimageExAIT.h"
#include "VibraimageExSV.h"
#include "SplashScreenDll.h"
#include "VibraimageExOverview.h"
#include "CViHTTPStorage.h"
#include <strsafe.h>
#include "CDlgStarter.h"
#include "viPreLogin01_dll.h"
#include <clocale>
#include <afxregpath.h>

#define VI_VAR_FRAME_MAKE_TAGS
#include "viFrameVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CVibraimageExApp

BEGIN_MESSAGE_MAP(CVibraimageExApp, CWinAppEx)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CVibraimageExApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CVibraimageExApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_NEW, &CVibraimageExApp::OnFileNew)
END_MESSAGE_MAP()


// CVibraimageExApp construction



CVibraimageExApp::CVibraimageExApp() :
	 m_pFrame(0)
	, m_splashScreen(checkCPU())
	, m_pDoc(0)
	, m_pView(0)
	, m_bNoMenu(0)
	, m_bCloseRqst(0)
	, m_bHidden(0)
	, m_cfgDisableRender(0)
	, m_bInit(FALSE)
	, m_pGraph(0)
	, m_pHist(0)
	, m_pQT(0)
	, m_pLD(0)
	, m_pSV(0)
	, m_pLDR(0)
	, m_pAI(0)
	, m_pAIT(0)
	, m_pAIV(0)
	, m_pDBList(0)
	, m_pMList(0)
	, m_nStart(0)
	, m_pVPos(0)
	, m_bReady(0)
	, m_bStartOK(TRUE)
	, m_bAbort(FALSE)
	, m_bRestart(FALSE)
	, m_pOverview(0)
	, m_bRegLocked(-1)
	, m_hWndStarter(0)
	, m_pEngine(0)
	, m_nLoadState(0)
{
//	CSplashScreenLock gLock;
#ifdef _DEBUG
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("ELSYS.Vibraimage.10"));

	m_splashScreen.Start();

}

bool CVibraimageExApp::checkCPU() {
	bool sse3_Supportted = false;
	bool sse4_1Supportted = false;
	bool sse4_2Supportted = false;

	int cpuinfo[4];
	__cpuid(cpuinfo, 1);

	sse3_Supportted = cpuinfo[2] & (1 << 9) || false;
	sse4_1Supportted = cpuinfo[2] & (1 << 19) || false;
	sse4_2Supportted = cpuinfo[2] & (1 << 20) || false;

	m_sseSupportted = sse3_Supportted; // sse4_1Supportted && sse4_2Supportted;
	m_avxSupportted = cpuinfo[2] & (1 << 28) || false;

	bool osxsaveSupported = cpuinfo[2] & (1 << 27) || false;
	if (osxsaveSupported && m_avxSupportted)
	{
		// _XCR_XFEATURE_ENABLED_MASK = 0
		unsigned long long xcrFeatureMask = _xgetbv(0);
		m_avxSupportted = (xcrFeatureMask & 0x6) == 0x6;
	}

	return m_avxSupportted && m_sseSupportted;
}

#ifdef _DEBUG
class dbgMemTest {
public:
	~dbgMemTest() {
#ifdef _DEBUG
	//	_CrtDumpMemoryLeaks();
#endif
	}
};
std::shared_ptr< dbgMemTest > dbgMemTestVar = std::make_shared< dbgMemTest >();
#endif

CVibraimageExApp theApp;



// CVibraimageExApp initialization

BOOL CVibraimageExApp::InitInstance()
{
	CSplashScreenLock gLock;

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	std::setlocale(LC_ALL, "en_US.UTF-8");

	if (!m_sseSupportted)
	{
		AfxMessageBox(IDS_ERROR_SSE);
		return FALSE;
	}

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}



	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// Parse command line for standard shell commands, DDE, file open
	ParseCommandLine(m_cmdl);

	checkWaitForPID();

#if !defined _DEBUG  && (defined SEQ_VIBRAIMAGE_PRO ||  defined SEQ_LITE_MID || defined SEQ_DEFENDERX)
	   
	bool bGhost = false;

	if (m_cmdl.IsSet(_T("/ghost")))
		bGhost = true;

	if (m_cmdl.IsSet(_T("/noghost")))
		bGhost = false;
#if defined SEQ_DEFENDERX
	else
	if (!m_cmdl.IsSet(_T("/ghost"))) {
		bGhost = false;
	}
#endif


	if (m_cmdl.IsSet(_T("/launcher"), 2))
		bGhost = false;

	if (m_cmdl.IsSet(_T("/r")))
		bGhost = false;
	if (m_cmdl.IsSet(_T("/load_config")))
		bGhost = false;

	if (bGhost) {

		gLock.Unlock();

		if (!m_cmdl.IsSet(_T("/norestart"), 1))
			m_crush.Start();

		CDlgStarter dlg;
		m_pMainWnd = &dlg;
		dlg.DoModal();
	
		return FALSE;
	}


	m_hWndStarter = (HWND)_tstoi64(m_cmdl.GetParam(_T("/launcher")));
	if (!IsWindow(m_hWndStarter))
		m_hWndStarter = 0;
#endif // #if defined SEQ_VIBRAIMAGE_PRO || defined SEQ_DEFENDERX || defined SEQ_LITE_MID

	if (m_hWndStarter)
		::SendMessage(m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_START, GetCurrentProcessId());



	if (m_cmdl.IsSet(_T("/u"), 1))
	{
		if (CAMCapWnd::RegisterDLL(true, false))
			m_log.Unregister((LPCTSTR)m_regString);
		return FALSE;
	}
#ifndef _DEBUG
	if (!m_hWndStarter && !m_cmdl.IsSet(_T("/norestart"), 1)) {
#	if defined SEQ_DEFENDERX
		m_crush.Start(1);
#	else
		m_crush.Start();
#	endif
	}
#endif
	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization


	if (m_cmdl.IsSet(_T("/delay"), 2))
	{
		CString str = m_cmdl.GetParam(_T("/delay"));
		long dt = str2int((LPCWSTR)str);
		if (str.Right(1).CompareNoCase(_T("s")) == 0)
			dt *= 1000;
		Sleep(dt);
	}


	m_pEngines.reserve(100);
	InitEngines(1);
	m_pEngine = m_pEngines[0];

	m_regString = m_pEngine->engine.GetReg()->GetRegBase0();

	if (!m_hWndStarter && m_cmdl.IsSet(_T("/excel")))
	{
		m_splashScreen.Finish();
		if (!CheckExcelVersion())
			return FALSE;
	}

	if (!m_hWndStarter && m_cmdl.IsSet(_T("/r"), 1))
	{
		if (CAMCapWnd::RegisterDLL(true, true))
			m_log.Register((LPCTSTR)m_regString);

		return FALSE;
	}

#ifdef SEQ_PRE_LOGIN01
	{
		m_splashScreen.Finish();

		int id = vi_prelogin_create(&m_pMainWnd);
		if (!m_pMainWnd)
			return FALSE;
		wchar_t strLogin[256] = { 0 };
		wchar_t strPwd[64] = { 0 };
		int nResult = vi_prelogin_call(id, strLogin, strPwd);
		m_pMainWnd = NULL;
		if(nResult != IDOK)
			return FALSE;
	}
#endif // SEQ_PRE_LOGIN01

	OnError(-1, EVENTLOG_INFORMATION_TYPE, _T("started"));

	SetRegistryKey(m_regString);

	InitEngines(-1);


	

	int nStartOK = GetProfileInt(_T("Settings"), _T("bStartOK"), 2);

	m_bStartOK = (nStartOK >= 0);

	WriteProfileInt(_T("Settings"), _T("bStartOK"), nStartOK-1);

	CVibraimageExPropPack::CheckConfigVer();

	m_nStart = GetProfileInt(_T("Start"), _T("nStart"), 1);

	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	SetCurrentDirectory(GetDefaultPath());

	
	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CVibraimageExDoc),
		RUNTIME_CLASS(CVibraimageExFrame),       // main SDI frame window
		RUNTIME_CLASS(CVibraimageExView)
		);
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(m_cmdl))
		return FALSE;


	if (m_pEngine->engine.GetSeq()->SeqIsOk() && IsWindow(m_pEngine->engine.GetWindow()) )
	{
		// The one and only window has been initialized, so show and update it
		OnInit();
	} else
		m_pMainWnd->ShowWindow(SW_HIDE);


	m_splashScreen.Finish();

	return TRUE;
}

int CVibraimageExApp::ExitInstance()
{
	AfxOleTerm(FALSE);
	m_splashScreen.Start();

	int ret = CWinAppEx::ExitInstance();

	m_pEngine = 0;
	for (int core = 0, coreMax = (int)m_pEngines.size(); core < coreMax; ++core) {
		if(m_pEngines[core]) delete m_pEngines[core];
	//	OnError(core, EVENTLOG_INFORMATION_TYPE, _T("finished"));
	}
	
	if (m_bRestart)
		Restart(false);


	OnError(-1, EVENTLOG_INFORMATION_TYPE, _T("finished"));
	

	if (m_hWndStarter)
		::SendMessage(m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_STOP, 0);

	return ret;
}

// CVibraimageExApp message handlers



// CVibraimageExApp customization load/save methods

void CVibraimageExApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
//	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
//	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CVibraimageExApp::LoadCustomState()
{
	CWinAppEx::LoadCustomState();
}

void CVibraimageExApp::SaveCustomState()
{
	if (!CanSaveState())
		return;
	CWinAppEx::SaveCustomState();
}

BOOL CVibraimageExApp::StoreWindowPlacement(
	const CRect& rectNormalPosition,
	int nFlags,
	int nShowCmd)
{
	if (!CanSaveState())
		return FALSE;

	return CWinAppEx::StoreWindowPlacement(rectNormalPosition, nFlags, nShowCmd);
}

BOOL CVibraimageExApp::LoadWindowPlacement(
	CRect& rectNormalPosition,
	int& nFlags,
	int& nShowCmd)
{
	return CWinAppEx::LoadWindowPlacement(rectNormalPosition, nFlags, nShowCmd);
}

bool CVibraimageExApp::CanLoadState() {
	return m_nLoadState != 0;
}

bool CVibraimageExApp::CanSaveState() {
	if (!m_bCloseRqst || m_bAbort || IsRegLocked() )
		return false;
	if (!m_pFrame || m_pFrame->m_nInit)
		return false;
#if !defined SEQ_MEDIC_MED_SDK && !defined SEQ_LITE_MID_SDK && !defined SEQ_MEDIC_MOL
	if(m_pEngine->engine.GetMeasure() && m_pEngine->engine.GetMeasure()->IsStarted())
		return false;
#endif
		return true;
}

BOOL CVibraimageExApp::SaveState(LPCTSTR lpszSectionName, CFrameImpl* pFrameImpl)
{
	if (!CanSaveState())
		return FALSE;
	if (!lpszSectionName)
	//	return __super::SaveState(L"Docking", pFrameImpl);
		return FALSE;
	return __super::SaveState(lpszSectionName, pFrameImpl);
}


BOOL CVibraimageExApp::LoadState(LPCTSTR lpszSectionName, CFrameImpl* pFrameImpl)
{
	if(!lpszSectionName)
		return __super::LoadState(L"Docking", pFrameImpl);
	return __super::LoadState(lpszSectionName, pFrameImpl);
}


// CVibraimageExApp message handlers







void CVibraimageExApp::OnFileNew()
{

		__super::OnFileNew();
}

void  CVibraimageExApp::RequestStop() {
	for (int core = 0, coreMax = (int)m_pEngines.size(); core < coreMax; ++core) {
		m_pEngines[core]->engine.PutVar(VI_VAR_STOP_RQST,0,COleVariant((long)1));
	}
}
void CVibraimageExApp::OnExit()
{
//	CSplashScreenLock gLock;

	if (m_hWndStarter)
		::SendMessage(m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_STOP, 0);

	RequestStop();

	if (/* m_bReady &&*/ !m_bDone)
	{
		PlaySound(0, 0, SND_PURGE);

		if (m_pView)
			m_pView->OnCloseFrame();

		if (m_pOverview)
			m_pOverview->DestroyWindow();

		for (int core = 0, coreMax = (int)m_pEngines.size(); core < coreMax; ++core) {
			if (!m_pEngines[core]->started)
				continue;
			if(m_bReady)
				CViHTTPStorage::RegSave(core);
			m_pEngines[core]->engine.Stop();
			if (m_pEngines[core]->pFtp) {
				if (m_bReady)
					m_pEngines[core]->pFtp->RegSave();
				m_pEngines[core]->pFtp = std::shared_ptr<CViFTPStorage>();
			}
			m_pEngines[core]->pLog = std::shared_ptr<CViLogStorage>();
			m_pEngines[core]->pVideoCheck = std::shared_ptr<CVideoCheck>();
		}



		m_bDone = true;

		WriteProfileInt(_T("Settings"), _T("bStartOK"), 2);

		RegFlush();
	}
}


bool CVibraimageExApp::OnInit()
{
	CInc noMenu(&m_bNoMenu);


	if (!m_bDone && !m_bAbort) {


		m_pFrame->m_wndMenuBar.NewMenuBar();


		m_pDoc->MakeTitle();

		for (int core = 0, coreMax = (int)m_pEngines.size(); core < coreMax; ++core) {
			if (m_pEngines[core]->pFtp) {
				m_pEngines[core]->pFtp->RegLoad();
			}
			CViHTTPStorage::RegLoad(core);
		}

	}




	m_bInit = TRUE;
	return true;
}



void CVibraimageExApp::OnFileOpen()
{
	CWinAppEx::OnFileOpen();
}


BOOL CVibraimageExApp::PreTranslateMessage(MSG* pMsg)
{
	if (m_pEngine && m_pEngine->engine.PreTranslateMessage(pMsg))
		return TRUE;

	return __super::PreTranslateMessage(pMsg);
}

CString CVibraimageExApp::cmlParam(CString prm) {
	prm.Replace(L"\"", L"\\\"");
	if (prm.Find(' ') >= 0)
		return CString(L"\"") + prm +CString(L"\"");
	return prm;
}

PROCESS_INFORMATION CVibraimageExApp::Restart(bool bExit, std::vector< LPCWSTR >* cmdAdd)
{
	if (m_hWndStarter)
		::SendMessage(m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_STOP, 0);

	size_t cnt = m_cmdl.m_cmdl.size();
	TCHAR fileName[MAX_PATH + 1] = { 0 };

	GetModuleFileName(NULL, fileName, MAX_PATH + 1);
	GetFullPathName(CString(fileName), MAX_PATH + 1, fileName, 0);

	CString cmd = cmlParam(fileName);
	
	
	if (cmdAdd) {
		for (size_t i = 0; i < cmdAdd->size(); ++i) {
			cmd += L" ";
			cmd += cmlParam((*cmdAdd)[i]);
		}
	}

	if (!cmdAdd && !m_cmdl.IsSet(_T("/delay")))
	{
		cmd += L" /delay 3s";
	}

	if (bExit) {
		cmd += L" /waitforpid ";
		cmd += int2str(GetCurrentProcessId());
	}

	for (size_t i = 0; i < cnt; ++i)
	{
		if (m_cmdl.m_cmdl[i].IsEmpty())
			continue;

		if (m_cmdl.m_cmdl[i] == _T("/launcher") ) {
			i += 2;
			continue;
		}

		cmd += L" ";
		cmd += cmlParam(m_cmdl.m_cmdl[i]);
	}


	if(bExit)
		theApp.OnError(-1, EVENTLOG_INFORMATION_TYPE, CString(_T("restarting... "))+ fileName);



	PROCESS_INFORMATION pi = launchProcess((LPCWSTR)fileName, (LPCWSTR)cmd);

			
	if (bExit) {

		if (m_hWndStarter) {
			::SendMessage(theApp.m_hWndStarter, WM_CLOSE, 0, 0);
		} 
	//	ExitProcess(1);
	//	exit(1);
		m_splashScreen.Finish();
		stopProcess(GetCurrentProcessId());
	}

	return pi;
}


bool CVibraimageExApp::SelectCore(int core, bool bUpdateAll) {
	if (core < 0 || core >= (int)m_pEngines.size())
		return false;
	if (core == m_pEngine->id)
		return true;

	m_pEngine = m_pEngines[core];

	if (bUpdateAll)
	{
		AlignAll();
	}


	return true;
}

void CVibraimageExApp::AlignAll() {
	if (m_pGraph) m_pGraph->Align();
	if (m_pAI) m_pAI->Align();
	if (m_pAIT) m_pAIT->Align();
	if (m_pAIV) m_pAIV->Align();
	if (m_pHist) m_pHist->Align();
	if (m_pQT) m_pQT->Align();
	if (m_pSV) m_pSV->Align();
	if (m_pLD) m_pLD->Align();
	if (m_pLDR) m_pLDR->Align();
	if (m_pView) m_pView->Align();
	if (m_pFrame) m_pFrame->m_wndPane.UpdateAll();

}

void CVibraimageExApp::InitEngines(int nMax) {

	if (nMax < 0) {
		nMax = GetProfileInt(_T("Settings"), _T("cores"), 1);
		if (nMax < 1)
			nMax = 1;
	}

	for (int core = (int)m_pEngines.size(); core < nMax; ++core) {
		ENGINE_PACK *pp = new ENGINE_PACK;
		pp->started = 0;
		pp->id = core;
		pp->engine.PutVar(VI_INFO_CORE_ID, 0, COleVariant((long)core, VT_INT));
		pp->engine.PutVar(VI_INFO_CORE_USE, 0, COleVariant((long)m_pEngines.size()+1, VT_INT));

		CString ver;
		CString strProductName, strProductVersion;
		GetProductAndVersion(strProductName, strProductVersion);
		ver = CString(strProductVersion);

		
		pp->engineInterface.Connect(core, &pp->engine);
		pp->engineReg.Connect(core, &pp->engine);
		pp->engineDB.Connect(core, &pp->engine);
		pp->pFtp = std::make_shared<CViFTPStorage>(&pp->engine);
		pp->pVideoCheck = std::make_shared <CVideoCheck>(core);

		pp->pLog = std::make_shared<CViLogStorage>(core);

		pp->engine.PutVar(VI_VAR_APP_VER, 0, COleVariant((LPCTSTR)ver));

		m_pEngines.push_back( pp );



	//	OnError(core, EVENTLOG_INFORMATION_TYPE, _T("started"));
	}
}

void CVibraimageExApp::OnNewVar(int id, int subID) {
	if (m_bCloseRqst)
		return;

	if (id == VI_INFO_CORE_USE) {
		if (m_pDoc) {
			int n = m_pEngine->engine.GetVar(id, 0, VT_INT).intVal;
			WriteProfileInt(_T("Settings"), _T("cores"),n);
			if(n != (int)m_pEngines.size())
				m_bRestart = 1;
		}
	} else
	if (id == VI_INFO_GRAPH_HIDE_ALL) {
		if (m_pFrame) m_pFrame->m_wndPane.UpdateAll();
	} else
	if (id == VI_INFO_LOCK_PWD_TIMEOUT || id == VI_INFO_LOCK_PWD || id == VI_INFO_LOCK_PWD1 ) {
		if(m_tRegLocked != -1)
			m_tRegLocked = GetTickCount64();
	}

	CViHTTPStorage::OnNewVar(m_pEngine->id, id, subID);
}

void CVibraimageExApp::PushVar(int id, int subID, int core) {
	if(!m_bCloseRqst)
		CNewVarEvent::PushVar(id, subID, core);
}

void CVibraimageExApp::SkipAll(BOOL bSkip,DWORD dt) {
	for (int core = 0, coreMax = (int)m_pEngines.size(); core < coreMax; ++core) {
		ENGINE_PACK*p = m_pEngines[core];
		if (!p || !p->started)
			continue;
		
		p->engine.PutVar(VI_FILTER_SKIP, 0, COleVariant((long)bSkip, VT_INT), 1);
		if(bSkip)
			p->engineInterface.PutImg0();
	}

	Sleep(dt);

	for (int core = 0, coreMax = (int)m_pEngines.size(); core < coreMax; ++core) {
		ENGINE_PACK*p = m_pEngines[core];
		if (!p || !p->started)
			continue;
		p->engineInterface.Sync();
	}
}


void CVibraimageExApp::OnError(int id, const WORD a_type, LPCWSTR msg) {
	CString msg2,title;
	if (m_pDoc) {
		title = m_pDoc->GetTitle();
	}
	if(!title.IsEmpty())
		msg2.Format(_T("(core %d) {%s} [%s]"), id+1, (LPCTSTR)title, msg);
	else
		msg2.Format(_T("(core %d) [%s]"), id + 1, msg);

	if (m_pEngine && m_pEngine->pLog)
		m_pEngine->pLog->Log(msg2);

	m_log.LogMessage(msg2, a_type, m_regString);
}


bool  CVibraimageExApp::CheckExcelVersion() {

	if (!m_pEngine->engineInterface.IsExcelRequired())
		return true;

// Get excel path
	const CString szPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\excel.exe");

	HKEY hKey;

	TCHAR excelPath[2048];

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return false;

	DWORD size = 2048;
	DWORD type = REG_SZ;
	LONG ret = RegQueryValueEx(hKey, _T(""), 0, &type, (LPBYTE)excelPath, &size);

	if (ret != ERROR_SUCCESS)
		return false;

	if (RegCloseKey(hKey) != ERROR_SUCCESS)
		return false;


	VS_FIXEDFILEINFO fv;
	if (!GetFileVersion(excelPath, fv))
		return false;

	WORD v1 = (fv.dwFileVersionMS >> 16 ) & 0xffff;
//	WORD v2 = (fv.dwFileVersionMS >> 0) & 0xffff;
//	WORD v3 = (fv.dwFileVersionLS >> 16) & 0xffff;
//	WORD v4 = (fv.dwFileVersionLS >> 0) & 0xffff;

	if (v1 >= 14)
		return true;

	if( AfxMessageBox(IDS_ERROR_EXCEL, MB_OKCANCEL | MB_ICONEXCLAMATION) != IDOK )
		return false;
	return true;
}


bool  CVibraimageExApp::GetFileVersion(LPCTSTR path, VS_FIXEDFILEINFO& fv) {
	ZeroMemory(&fv, sizeof(fv));

	DWORD  verHandle = 0;
	UINT   size = 0;
	LPBYTE lpBuffer = NULL;
	DWORD  verSize = GetFileVersionInfoSize(path, &verHandle);

	if (!verSize)
		return false;

	char* verData = new char[verSize];
	ZeroMemory(verData, verSize);

	if (GetFileVersionInfo(path, verHandle, verSize, verData))
	{
		if (VerQueryValue(verData, _T("\\"), (VOID FAR * FAR*) & lpBuffer, &size))
		{
			if (size)
			{
				VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
				if (verInfo->dwSignature == 0xfeef04bd)
				{
					fv = *verInfo;
				}

			}
		}
	}
	delete[] verData;

	return fv.dwSignature == 0xfeef04bd;
}


void CVibraimageExApp::CleanSlateVI() {
	++m_bAbort;

	CleanState();
	

	BOOL bSuccess = RegDelnode(HKEY_CURRENT_USER,CString(_T("SOFTWARE\\"))+ m_regString + _T("\\VibraimageEx\\Workspace"));
	bSuccess = RegDelnode(HKEY_CURRENT_USER, CString(_T("SOFTWARE\\")) + m_regString + _T("\\VibraimageEx\\Property"));

	Restart();
}

//*************************************************************
//
//  RegDelnodeRecurse()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL CVibraimageExApp::RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey)
{
	LPTSTR lpEnd;
	LONG lResult;
	DWORD dwSize;
	TCHAR szName[MAX_PATH];
	HKEY hKey;
	FILETIME ftWrite;

	// First, see if we can delete the key without having
	// to recurse.

	lResult = RegDeleteKey(hKeyRoot, lpSubKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;

	lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

	if (lResult != ERROR_SUCCESS)
	{
		if (lResult == ERROR_FILE_NOT_FOUND) {
			printf("Key not found.\n");
			return TRUE;
		}
		else {
			printf("Error opening key.\n");
			return FALSE;
		}
	}

	// Check for an ending slash and add one if it is missing.

	lpEnd = lpSubKey + lstrlen(lpSubKey);

	if (*(lpEnd - 1) != TEXT('\\'))
	{
		*lpEnd = TEXT('\\');
		lpEnd++;
		*lpEnd = TEXT('\0');
	}

	// Enumerate the keys

	dwSize = MAX_PATH;
	lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
		NULL, NULL, &ftWrite);

	if (lResult == ERROR_SUCCESS)
	{
		do {

			*lpEnd = TEXT('\0');
			StringCchCat(lpSubKey, MAX_PATH * 2, szName);

			if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
				break;
			}

			dwSize = MAX_PATH;

			lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
				NULL, NULL, &ftWrite);

		} while (lResult == ERROR_SUCCESS);
	}

	lpEnd--;
	*lpEnd = TEXT('\0');

	RegCloseKey(hKey);

	// Try again to delete the key.

	lResult = RegDeleteKey(hKeyRoot, lpSubKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

//*************************************************************
//
//  RegDelnode()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL CVibraimageExApp::RegDelnode(HKEY hKeyRoot, LPCTSTR lpSubKey)
{
	TCHAR szDelKey[MAX_PATH * 2];

	StringCchCopy(szDelKey, MAX_PATH * 2, lpSubKey);
	return RegDelnodeRecurse(hKeyRoot, szDelKey);

}

bool CVibraimageExApp::IsRegLocked() {
	if(m_bCloseRqst && m_cmdl.IsSet(L"/measure"))
		return true;
	if (m_bCloseRqst && m_pEngine && !m_pEngine->engine.GetSeq()->SeqIsOk() )
		return true;
	if(m_bRegLocked != -1)
		return !!m_bRegLocked;

	if (m_tRegLocked != -1) 
		m_tRegLocked = GetTickCount64();
	return !!(m_bRegLocked = GetProfileInt(L"Settings", L"RegLocked",0));
}

void CVibraimageExApp::RegSave(bool bEn) {
	if (theApp.m_cmdl.IsSet(L"/measure") || IsRegLocked() )
		return;

	if (theApp.m_hWndStarter)
		::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_DELAY, 1);

	SaveCustomState();
	SaveState();

	CString base = _T("Docking");
	// CString base = _T("Workspace");
	CString reg = AFXGetRegPath(base);
//	reg.TrimRight(L"\\");

	if(m_pFrame)
		m_pFrame->GetDockingManager()->SaveState(reg);
	
	if (bEn) {
		for (size_t i = 0; i < m_pEngines.size(); ++i) {
			if (m_pEngines[i]) {
				m_pEngines[i]->engineReg.MakeSave();
			}
		}
	}

	if (theApp.m_hWndStarter)
		::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_DELAY, 0);

	RegFlush();
}

void CVibraimageExApp::SetRegLocked(bool nv) {
	if (nv && !IsRegLocked()) {
		RegSave();
		__super::WriteProfileInt(_T("Settings"), _T("bStartOK"), 2);
	}

	__super::WriteProfileInt(L"Settings", L"RegLocked", !!nv);
	m_bRegLocked =  !!nv;
	if (m_tRegLocked != -1)
		m_tRegLocked = GetTickCount64();
	if (m_pFrame) {
		m_pFrame->m_wndPane.UpdateAll();
		m_pFrame->RedrawWindow();
	}
}

BOOL CVibraimageExApp::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue) {
	if (IsRegLocked()) 
		return FALSE;
	BOOL r = __super::WriteProfileInt(lpszSection, lpszEntry, nValue);
	if (m_pFrame && m_pFrame->m_hWnd && !m_bCloseRqst)
		m_pFrame->SetTimer(CVibraimageExFrame::T_REG_FLUSH, 2000, 0);
	return r;
}

BOOL CVibraimageExApp::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue) {
	if (IsRegLocked())
		return FALSE;
	
	BOOL r = __super::WriteProfileString(lpszSection, lpszEntry, lpszValue);
	if (m_pFrame && m_pFrame->m_hWnd && !m_bCloseRqst)
		m_pFrame->SetTimer(CVibraimageExFrame::T_REG_FLUSH, 2000, 0);
	return r;
}

BOOL CVibraimageExApp::WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes) {
	if (IsRegLocked())
		return FALSE;
	BOOL r = __super::WriteProfileBinary(lpszSection, lpszEntry, pData, nBytes);
	if (m_pFrame && m_pFrame->m_hWnd && !m_bCloseRqst)
		m_pFrame->SetTimer(CVibraimageExFrame::T_REG_FLUSH, 2000, 0);
	return r;
}


PROCESS_INFORMATION CVibraimageExApp::launchProcess(std::wstring app, std::wstring arg)
{

	// Prepare handles.
	STARTUPINFO si;
	PROCESS_INFORMATION pi; // The function returns this
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	//Prepare CreateProcess args
	std::wstring app_w(app.length(), L' '); // Make room for characters
	std::copy(app.begin(), app.end(), app_w.begin()); // Copy string to wstring.

	std::wstring arg_w(arg.length(), L' '); // Make room for characters
	std::copy(arg.begin(), arg.end(), arg_w.begin()); // Copy string to wstring.

	std::wstring input = app_w + L" " + arg_w;
	wchar_t* arg_concat = const_cast<wchar_t*>(input.c_str());
	const wchar_t* app_const = app_w.c_str();

	// Start the child process.
	if (!CreateProcessW(
		app_const,      // app path
		arg_concat,     // Command line (needs to include app path as first argument. args seperated by whitepace)
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		//printf("CreateProcess failed (%d).\n", GetLastError());
		throw std::exception("Could not create child process");
	}
	else
	{
		// std::cout << "[          ] Successfully launched child process" << std::endl;
	}

	// Return process handle
	return pi;
}

bool CVibraimageExApp::checkIfProcessIsActive(PROCESS_INFORMATION pi)
{
	// Check if handle is closed
	if (pi.hProcess == NULL)
	{
		//	printf("Process handle is closed or invalid (%d).\n", GetLastError());
		return FALSE;
	}

	// If handle open, check if process is active
	DWORD lpExitCode = 0;
	if (GetExitCodeProcess(pi.hProcess, &lpExitCode) == 0)
	{
		//	printf("Cannot return exit code (%d).\n", GetLastError());
		throw std::exception("Cannot return exit code");
	}
	else
	{
		if (lpExitCode == STILL_ACTIVE)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

bool CVibraimageExApp::stopProcess(DWORD pid) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	TerminateProcess(hProcess, 1);
	CloseHandle(hProcess);
	return true;
}
bool CVibraimageExApp::stopProcess(PROCESS_INFORMATION& pi)
{
	// Check if handle is invalid or has allready been closed
	if (pi.hProcess == NULL)
	{
		//	printf("Process handle invalid. Possibly allready been closed (%d).\n");
		return 0;
	}

	// Terminate Process
	if (!TerminateProcess(pi.hProcess, 1))
	{
		//	printf("ExitProcess failed (%d).\n", GetLastError());
		return 0;
	}

	// Wait until child process exits.
	if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED)
	{
		//	printf("Wait for exit process failed(%d).\n", GetLastError());
		return 0;
	}

	// Close process and thread handles.
	if (!CloseHandle(pi.hProcess))
	{
		//	printf("Cannot close process handle(%d).\n", GetLastError());
		return 0;
	}
	else
	{
		pi.hProcess = NULL;
	}

	if (!CloseHandle(pi.hThread))
	{
		//	printf("Cannot close thread handle (%d).\n", GetLastError());
		return 0;
	}
	else
	{
		pi.hProcess = NULL;
	}
	return 1;
}


bool CVibraimageExApp::LoadSettings(LPCTSTR path, bool bCheckCores, bool bSave) {
	BeginWaitCursor();

	theApp.RegFlush();

	int core0 = m_pEngine->id;
	bool bRestart = false;
	m_splashScreen.Start();

	std::map<int, std::map<CStringA,int> > xLoaded;
	SkipAll(TRUE);
	if(bSave)
		RegSave();

	xml_document doc;
	if (!doc.load_file(path))
		return false;

	if (theApp.m_hWndStarter)
		::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_DELAY, 1);

	auto avCores = doc.first_child().attribute_value("cores");
	if (avCores && *avCores) {
		int nCores = atoi(avCores);

		bRestart = (nCores != m_pEngines.size());
		WriteProfileInt(_T("Settings"), _T("cores"), nCores);
		
		if (nCores != m_pEngines.size() && bCheckCores) {
			std::vector<LPCWSTR> opt;
			opt.push_back(L"/load_config");
			opt.push_back(path);

			Restart(true, &opt);
			return true;
		}
		
		for (xml_node_iterator i = doc.first_child().begin(); i != doc.first_child().end(); ++i) {
			const char* sid = i->attribute_value("id");
			int id = sid ? atoi(sid) : 0;
			if (id < 0 || id >= m_pEngines.size())
				continue;
			xml_node xcore = *i;
			CStringA base = xcore.attribute_value("base");
			if (xLoaded[id][base])
				continue;
			xml_document cdoc;
			cdoc.append_copy(xcore);
			if (cdoc.first_child()) {
				cdoc.first_child().set_name("vibraimage");
				std::string str = cdoc.save_str(0);
				m_pEngines[id]->engine.GetReg()->LoadSettings(str.c_str());
				xLoaded[id][base]++;

			}
			if (theApp.m_hWndStarter)
				::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_DELAY, 1);
		}
	}
	else
	if (m_pEngine->engine.GetReg()->LoadSettings(path))
	{
		if (theApp.m_hWndStarter)
			::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_DELAY, 1);
	}


	if(m_pFrame)
		m_pFrame->m_wndPane.UpdateAll();

	
	m_splashScreen.Finish();
	EndWaitCursor();

	if (!bRestart) {
		SkipAll(FALSE);
	}
	else {
		if (!IsRegLocked())
			m_bRegLocked = 1;

		Restart(true);
	}
	return true;
}

void CVibraimageExApp::checkWaitForPID() {
	if (!m_cmdl.IsSet(L"/waitforpid", 2))
		return;
	DWORD pid = _tstoi(m_cmdl.GetParam(L"/waitforpid"));
	if (!pid)
		return;
	
	m_cmdl.Remove(L"/waitforpid", 2);

	while (IsProcessRunning(pid))
		Sleep(100);
}

BOOL CVibraimageExApp::IsProcessRunning(DWORD pid)
{
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
	DWORD ret = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return ret == WAIT_TIMEOUT;
}


BOOL CVibraimageExApp::FileExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int CVibraimageExApp::OnOpenDocument(int core, const CString& doc) {
	if (m_pEngines[core] && m_pEngine->pLog)
		m_pEngines[core]->pLog->Log(CString(L"OPEN\t") + doc);
	if(m_pEngines[core])
		return m_pEngines[core]->engine.OpenDocument(doc);

	return 0;
}

bool CVibraimageExApp::GetProductAndVersion(CString& strProductName, CString& strProductVersion)
{
	// get the filename of the executable containing the version resource
	TCHAR szFilename[MAX_PATH + 1] = { 0 };
	if (GetModuleFileName(NULL, szFilename, MAX_PATH) == 0)
	{
		TRACE("GetModuleFileName failed with error %d\n", GetLastError());
		return false;
	}

	// allocate a block of memory for the version info
	DWORD dummy;
	DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
	if (dwSize == 0)
	{
		TRACE("GetFileVersionInfoSize failed with error %d\n", GetLastError());
		return false;
	}
	std::vector<BYTE> data(dwSize);
	memset(&data[0], 0, dwSize);

	// load the version info
	if (!GetFileVersionInfo(szFilename, NULL, dwSize, &data[0]))
	{
		TRACE("GetFileVersionInfo failed with error %d\n", GetLastError());
		return false;
	}

	// get the name and version strings
	LPVOID pvProductName = NULL;
	unsigned int iProductNameLen = 0;
	LPVOID pvProductVersion = NULL;
	unsigned int iProductVersionLen = 0;
	unsigned int iTranslate = 0;

	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	VerQueryValue(&data[0],
		TEXT("\\VarFileInfo\\Translation"),
		(LPVOID*)& lpTranslate,
		&iTranslate);
	TCHAR lang[64];
	StringCchPrintf(lang, 64, _T("%04x%04x"), lpTranslate[0].wLanguage,	lpTranslate[0].wCodePage);

	if (!VerQueryValue(&data[0], CString("\\StringFileInfo\\")+ lang +"\\ProductName", &pvProductName, &iProductNameLen) ||
		!VerQueryValue(&data[0], CString("\\StringFileInfo\\")+ lang +"\\ProductVersion", &pvProductVersion, &iProductVersionLen))
	{
		TRACE("Can't obtain ProductName and ProductVersion from resources\n");
		return false;
	}

	strProductName.SetString((LPCTSTR)pvProductName, iProductNameLen);
	strProductVersion.SetString((LPCTSTR)pvProductVersion, iProductVersionLen);

	return true;
}