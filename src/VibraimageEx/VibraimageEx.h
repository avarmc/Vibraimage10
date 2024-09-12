
// VibraimageEx.h : main header file for the VibraimageEx application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "ViEngineExSDK_ID.h"

#include "Cmdl.h"
#include "ViEngineExProc.h"
#include "ViEngineExProcDb.h"
#include "ViEngineExProcReg.h"
#include "NewVarEventVI.h"
#include "ViFTPStorage.h"
#include "CrushCheck.h"
#include "SplashScreenDll.h"
#include "LogReport.h"
#include "CVideoCheck.h"
#include "CWinAppExReg.h"
#include "CViLogStorage.h"

class CVibraimageExDoc;
class CVibraimageExView;
class CVibraimageExFrame;
class CVibraimageExGraph;
class CVibraimageExHist;
class CVibraimageExQT;
class CVibraimageExLD;
class CVibraimageExSV;
class CVibraimageExAI;
class CVibraimageExProp;
class CVibraimageExLDR;
class CVibraimageExVPos;
class CVibraimageExOverview;
class CVibraimageExAIT;
class CVibraimageExAIV;

// CVibraimageExApp:
// See VibraimageEx.cpp for the implementation of this class
//

class CVibraimageExApp : public CWinAppExReg, public CNewVarEventVI
{
public:
	CVibraimageExApp();
	~CVibraimageExApp() {
		m_splashScreen.Finish();
	}
public:
	CVibraimageExDoc *		m_pDoc;
	CVibraimageExView *		m_pView;
	CVibraimageExFrame *	m_pFrame;
	CVibraimageExGraph *	m_pGraph;
	CVibraimageExHist *		m_pHist;
	CVibraimageExQT	*		m_pQT;
	CVibraimageExAIT*		m_pAIT;
	CVibraimageExAIV*		m_pAIV;
	CVibraimageExSV	*		m_pSV;
	CVibraimageExLD *		m_pLD;
	CVibraimageExLDR *		m_pLDR;
	CVibraimageExAI*		m_pAI;
	CVibraimageExProp*		m_pDBList;
	CVibraimageExProp*		m_pMList;
	CVibraimageExVPos*		m_pVPos;
	CVibraimageExOverview*	m_pOverview;
public:
	typedef struct tagENGINE_PACK {
		int id,started;
		CVIENGINE_EX_CLASS		engine;
		CViEngineExProc			engineInterface;
		CViEngineExProcDb		engineDB;
		CViEngineExProcReg		engineReg;
		std::shared_ptr<CViFTPStorage>	pFtp;
		std::shared_ptr<CViLogStorage>	pLog;

		std::shared_ptr<CVideoCheck>	pVideoCheck;

		tagENGINE_PACK() {
			id = -1;
			started = false;
		}

	}ENGINE_PACK;

	ENGINE_PACK *				m_pEngine;
	std::vector<ENGINE_PACK*>	m_pEngines;

public:
	CCmdl		m_cmdl;
	CString		m_regString;
	CSplashScreen m_splashScreen;
	HWND		m_hWndStarter;
public:
	LONG		m_nLoadState;
	BOOL		m_bStartOK;
	BOOL		m_bDone, m_bInit, m_bAbort;
	BOOL		m_bNoMenu;
	BOOL		m_bCloseRqst;
	BOOL		m_bHidden;
	BOOL		m_cfgDisableRender;
	int			m_nStart;
	BOOL		m_bReady;
	BOOL		m_bRestart;
	BOOL		m_avxSupportted;
	BOOL		m_sseSupportted;
	CString		m_path;

	CLogReport			m_log;
	CCrushCheck			m_crush;


	BOOL		m_bRegLocked;
	ULONGLONG	m_tRegLocked = 0;
// Overrides
public:
	virtual void OnNewVar(int id, int subID);
	void PushVar(int id, int subID, int core);

	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	virtual BOOL StoreWindowPlacement(
		const CRect& rectNormalPosition,
		int nFlags,
		int nShowCmd);

	virtual BOOL LoadWindowPlacement(
		CRect& rectNormalPosition,
		int& nFlags,
		int& nShowCmd);


	virtual BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
	virtual BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	virtual BOOL WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);

	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
protected:
	DECLARE_MESSAGE_MAP()
public:
	PROCESS_INFORMATION Restart(bool bExit=true, std::vector< LPCWSTR >* cmdAdd=0);

	void OnExit();
	bool OnInit();
	void RequestStop();
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL SaveState(LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL);
	virtual BOOL LoadState(LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL);

	bool LoadSettings(LPCTSTR path,bool bCheckCores,bool bSave);

	bool CanLoadState();
	bool CanSaveState();

	void InitEngines(int nMax);
	bool SelectCore(int core, bool bUpdateAll);
	void AlignAll();

	void SkipAll(BOOL bSkip, DWORD dt=500);

	void OnError(int id,const WORD a_type, LPCWSTR msg);

	bool checkCPU();

	bool CheckExcelVersion();

	bool GetFileVersion(LPCTSTR path, VS_FIXEDFILEINFO& fv);

	void CleanSlateVI();

	BOOL RegDelnode(HKEY hKeyRoot, LPCTSTR lpSubKey);
	BOOL RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey);

	bool IsRegLocked();
	void SetRegLocked(bool nv);

	void RegSave(bool bEn = true);

	static PROCESS_INFORMATION launchProcess(std::wstring app, std::wstring arg);
	static bool checkIfProcessIsActive(PROCESS_INFORMATION pi);
	static bool stopProcess(PROCESS_INFORMATION& pi);
	static bool stopProcess(DWORD pid);

	static CString cmlParam(CString prm);

	void checkWaitForPID();
	BOOL IsProcessRunning(DWORD pid);
	BOOL FileExists(LPCTSTR szPath);

	bool GetProductAndVersion(CString& strProductName, CString& strProductVersion);

	int OnOpenDocument(int core,const CString& doc);
};



extern CVibraimageExApp theApp;
