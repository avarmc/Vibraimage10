#pragma once

#include "VIEngine.h"


#ifdef VIENGINEEX_EXPORTS
#define VIENGINEEX_API __declspec(dllexport)
#else
#define VIENGINEEX_API __declspec(dllimport)
#endif

#if !defined SEQ_MD && !defined VIENGINEEX_GET_ENGINE && !defined SEQ_MEDIC_MED_SDK && !defined SEQ_LITE_MID_SDK && !defined SEQ_MEDIC_MOL && !defined SEQ_MEDIC_HTAI && !defined SEQ_MEDIC_C5S && !defined SEQ_MEDIC_STF_SDK
#	define	VIENGINEEX_GET_ENGINE
#endif 

class CAMCapWnd;

namespace CViEngineExNS
{

	class VIENGINEEX_API CVIENGINE_EX_CLASS;

	class VIENGINEEX_API CViEngineCore {
	private:
		void* m_pApp = 0;
	public:
		CViEngineCore(void* pApp);
		bool GetResultPtr(int id, void** ptr, int* pw, int* ph);
		int GetStatHist(int id, int* pHist256 = 0, float* pFPS = 0, int a = 0);
		void* LockResult(bool bLock, void* pLock);
	};

	class VIENGINEEX_API CViEngineExRegInterface
	{
	private:
		void *m_pApp = 0;
	public:
		CViEngineExRegInterface();
		CViEngineExRegInterface(CVIENGINE_EX_CLASS *pApp);
		~CViEngineExRegInterface();

		void Connect(CVIENGINE_EX_CLASS *pApp);
	public:
		virtual void RegCoreGroup(LPCWSTR gr, LPWSTR out,size_t max_cch);
		virtual LPCWSTR GetRegBase0(); // HKLU/SOFTWARE/+
		virtual LPCWSTR GetRegBase(bool bIncludeApp = false); // HKLU/SOFTWARE/+
		virtual int GetProfileIntEx(LPCWSTR gr, LPCWSTR key, int def = 0) { return def; }
		virtual void WriteProfileIntEx(LPCWSTR gr, LPCWSTR key, int v) {}

		virtual BOOL GetProfileStringEx(LPCWSTR gr, LPCWSTR key, LPWSTR res, int cch, LPCWSTR def = _T("")) { return FALSE; }
		virtual void WriteProfileStringEx(LPCWSTR gr, LPCWSTR key, LPCWSTR v) {}

		virtual int GetProfileBinEx(LPCWSTR gr, LPCWSTR key, void *ptr, int size_max) { return 0; }
		virtual void WriteProfileBinEx(LPCWSTR gr, LPCWSTR key, void *ptr, int size) {}
	public:
		virtual BOOL LoadSettings(LPCWSTR szXmlPathm,LPCWSTR groups = 0);
		virtual BOOL LoadSettings(LPCSTR szXmlStr, LPCWSTR group = 0, LPCWSTR groups = 0);
		virtual BOOL SaveSettings(LPSTR szXmlStr, LPCWSTR group = 0, bool bNew=true);
		virtual void OnSettingsLoad(LPCSTR szXmlStr) {}
	public:
		virtual void RegLoad(LPCWSTR gr, LPCWSTR key, LONGLONG& v);
		virtual void RegLoad(LPCWSTR gr, LPCWSTR key, int& v);
		virtual void RegLoad(LPCWSTR gr, LPCWSTR key, LPWSTR v, int cch = 256);
		virtual void RegLoad(LPCWSTR gr, LPCWSTR key, double& v);
		virtual void RegLoad(LPCWSTR gr, LPCWSTR key, float& v);
		virtual void RegLoad(LPCWSTR gr, LPCWSTR key, RECT& v);
		virtual void RegLoad(LPCWSTR gr, LPCWSTR key, POINT& v);
		virtual void RegLoad(LPCWSTR gr, LPCWSTR key, bool& v);

		virtual void RegSave(LPCWSTR gr, LPCWSTR key, LONGLONG v);
		virtual void RegSave(LPCWSTR gr, LPCWSTR key, int v);
		virtual void RegSave(LPCWSTR gr, LPCWSTR key, LPCWSTR v);
		virtual void RegSave(LPCWSTR gr, LPCWSTR key, double v);
		virtual void RegSave(LPCWSTR gr, LPCWSTR key, float v);
		virtual void RegSave(LPCWSTR gr, LPCWSTR key, const RECT& v);
		virtual void RegSave(LPCWSTR gr, LPCWSTR key, const POINT& v);
		virtual void RegSave(LPCWSTR gr, LPCWSTR key, bool v);

		virtual bool IsLocked() { return false; }

		virtual void MakeSave();
		virtual void MakeLoad();

		virtual void Flush() {}
	};

	class VIENGINEEX_API CViEngineExBaseInterface
	{
	protected:
		void *m_pApp = 0;
	public:
		CViEngineExBaseInterface();
		CViEngineExBaseInterface(CVIENGINE_EX_CLASS *pApp);
		~CViEngineExBaseInterface();

		void Connect(CVIENGINE_EX_CLASS *pApp);
	public:
		virtual void OnNewVar(int id, int subID) {}
		virtual void OnVideoInput(double dt, BYTE *pBits, int w, int h, int bpp) {}
		virtual void OnFaceDetected(VI_FACE_INFO_DATA *pInfo) {}
		virtual bool OnResultAlign(int idr, HWND hWnd, DWORD * pBits, int w, int h);
		virtual void OnPlaySound(int id, float v, float lev, LPCWSTR path, ULONGLONG t) {}

		virtual void OnMarker(int id);
		virtual bool IsMarker(int id);

		virtual bool IsOptionSet(LPCWSTR key, int shift = 1) { return false; }
		virtual bool GetOption(LPCWSTR key, int shift, LPWSTR param, int cch) { return false; }

		virtual bool IsExcelRequired();

		virtual bool CanStartFile(LPCWSTR path);

		virtual void OnLoginWindow(bool bShow) {}

		virtual void Sync();
		virtual void PutImg0();

		virtual void OnError(int id, const WORD a_type, LPCWSTR msg) {}

		virtual void OnCloseRequest();


		virtual void ProcessMessages();
		virtual void Log(LPCWSTR msg) {}
	public:
		virtual bool GetVideoChannel(LPWSTR name, long cch_max) { return false; }
	public:
		virtual int OnVideoFrame(double dblSampleTime, BYTE* pBuffer, long lBufferSize, void* pFormat);
		virtual int OnAudioFrame(double dblSampleTime, BYTE* pBuffer, long lBufferSize, void* pFormat);
	public:
		virtual COLORREF GetWndColor(int id); // 	VI_COLORS
		virtual HWND GetMainWnd();
	public:
		/// MOL Functions:
		virtual LPCSTR molMakePreFileData(LPCSTR strData, LPCSTR strBoundary, LPCSTR strFileName) { return strData; }
		virtual LPCWSTR molMakeUrl(LPCWSTR strUrl) { return  strUrl; }
		virtual void molResponse(LPCSTR strResponse) { }
	};

	class VIENGINEEX_API CViEngineExSeqInterface
	{
	protected:
		void *m_pApp = 0;
	public:
		CViEngineExSeqInterface(CVIENGINE_EX_CLASS *pApp);
		~CViEngineExSeqInterface() {}

		bool SeqIsValidCode(LPCWSTR code);
		bool SeqIsHttp(void);
		bool SeqIsOnline(void);
		bool SeqIsUnreg(void);
		bool SeqIsMeasure(void);
		bool SeqIsOk(void);
		bool SeqIsDemo(void);
		bool SeqIsLimit(void);
		bool SeqIsInit(void);
		LPCWSTR SeqSerial(void);
		LPCWSTR SeqOwner(void);
		LPCWSTR SeqLimit(void);
		int	SeqFlags();
	};

	class VIENGINEEX_API CViEngineExLangInterface
	{
	protected:
		void *m_pApp = 0;
	public:
		CViEngineExLangInterface();
		CViEngineExLangInterface(CVIENGINE_EX_CLASS *pApp);
		~CViEngineExLangInterface();

		void Connect(CVIENGINE_EX_CLASS *pApp);
	public:
		virtual LPCWSTR		Get(LPCWSTR sid, LPCWSTR def=0 ) { return L""; }
		virtual LPCWSTR		Get(int iid, LPCWSTR def = 0) { return L""; }
		virtual int			Find(LPCWSTR sid) { return -1; }
		virtual LPCWSTR		Name(int n) { return 0; }
		virtual LPCWSTR		Rename(LPCWSTR str) { return str; }
		virtual bool		IsSelected(int n) { return false; }
		virtual bool		Select(int n) { return false; }
		virtual void		Init(LPCWSTR tag) {}
	};

	class VIENGINEEX_API CViEngineExDatabaseInterface
	{
	private:
		void *m_pApp = 0;
	protected:
		CViEngineExDatabaseInterface *m_pBase = 0;
	public:
		CViEngineExDatabaseInterface();
		CViEngineExDatabaseInterface(CVIENGINE_EX_CLASS *pApp);
		~CViEngineExDatabaseInterface();

		virtual void Connect(CVIENGINE_EX_CLASS *pApp);
	public:
		virtual bool	GetBasePath(LPWSTR path, int cch_max) { return m_pBase ? m_pBase->GetBasePath(path, cch_max) :  false; }
		virtual bool	SetBasePath(LPWSTR path) { return m_pBase ? m_pBase->SetBasePath(path) : false; }

		virtual int		Reload() { return m_pBase ? m_pBase->Reload() : 0; }
		virtual int		GetItemCount() { return  m_pBase ? m_pBase->GetItemCount() : 0; }
		virtual bool	GetItemBasePath(int n, LPWSTR base, int cch_max) { return  m_pBase ? m_pBase->GetItemBasePath(n,base,cch_max) : false; }
		virtual int		GetCurrentItem() { return  m_pBase ? m_pBase->GetCurrentItem() : -1; }
		virtual bool	SelectItem(int id, BOOL bLock = TRUE) { return  m_pBase ? m_pBase->SelectItem(id,bLock) : false; }

		virtual int		NewItem(LPCWSTR name) { return m_pBase ? m_pBase->NewItem(name) : -1; }

		virtual bool	NewID(LPWSTR id, int cch);

		virtual bool	SetItemData(LPCWSTR tag, LPCWSTR value) { return m_pBase ? m_pBase->SetItemData(tag, value) : false; }
		virtual bool	GetItemData(LPCWSTR tag, LPWSTR value, int cch) { return m_pBase ? m_pBase->GetItemData(tag, value,cch) : false; }

		virtual bool   CheckItemID(LPCWSTR id, bool bValidate) { return  m_pBase ? m_pBase->CheckItemID(id, bValidate) : 0; }
	public:
		virtual bool	IsStarted() { return  m_pBase ? m_pBase->IsStarted() : false; }
		virtual bool	StartSession(LPCWSTR prefix=0, LPCWSTR postfix=0) { return  m_pBase ? m_pBase->StartSession(prefix,postfix) : false; }
		virtual bool	IsAborted() { return  m_pBase ? m_pBase->IsAborted() : false; }
		virtual bool	AbortSession(bool bLock = true) { return  m_pBase ? m_pBase->AbortSession(bLock) : false; }
		virtual bool	StopSession(bool bLock = true) { return  m_pBase ? m_pBase->StopSession(bLock) : false; }
		virtual bool	GetSessionName(LPWSTR name, int cch) { return  m_pBase ? m_pBase->GetSessionName(name, cch) : false; }
		virtual bool	OnSessionStopped(LPCWSTR name) { return  m_pBase ? m_pBase->OnSessionStopped(name) : false; }
		virtual bool	GetSessionFilePath(LPCWSTR tag, LPWSTR path, int cch);
		virtual bool	AddReference(LPCWSTR path) { return  m_pBase ? m_pBase->AddReference(path) : false; }

		virtual LPCWSTR GetLastFile(LPCWSTR tag) { return  m_pBase ? m_pBase->GetLastFile(tag) : 0; }
		virtual bool	WriteFile(LPCWSTR tag, const void *lpBuf, UINT  nCount, bool bLock=true) { return  m_pBase ? m_pBase->WriteFile(tag, lpBuf, nCount, bLock) : false; }
		virtual UINT 	ReadFile(LPCWSTR tag, void *lpBuf, UINT  nCount, bool bLock = true) { return  m_pBase ? m_pBase->ReadFile(tag, lpBuf, nCount, bLock) : false; }
		virtual bool	CloseFile(LPCWSTR tag, bool bLock=true,bool bWait=false) { return  m_pBase ? m_pBase->CloseFile(tag, bLock,bWait) : false; }
		virtual bool	OnSessionClosed(LPCWSTR tag) { return  m_pBase ? m_pBase->OnSessionClosed(tag) : false; }

		virtual HBITMAP	GetCapturedImage(int pos, int id = -1) { return  m_pBase ? m_pBase->GetCapturedImage(pos, id) : NULL; }

		virtual UINT	GetLastSessionFiles(int n, LPWSTR path, int cch_max) { return  m_pBase ? m_pBase->GetLastSessionFiles(n,path,cch_max) : 0; }

		virtual int		GetDBMode() { return m_pBase ? m_pBase->GetDBMode() : 0; }
		virtual void	SetDBMode(int mode) {	if (m_pBase) m_pBase->SetDBMode(mode);	}

		virtual void	SetStatus(LPCWSTR sessionName, LPCSTR status) { if (m_pBase) m_pBase->SetStatus(sessionName, status); }

		virtual int		GetBackupCount() { return 10000; }
		virtual void	MakeBackup(LPCWSTR sessionName) { if (m_pBase) m_pBase->MakeBackup(sessionName); }

		virtual bool	ValidateSessionFace() { return m_pBase ? m_pBase->ValidateSessionFace() : true; }
			
		virtual bool	ProcessMeasureJSON(LPCWSTR xmlPath, LPCWSTR jsonTemplate, LPCWSTR jsonOutPath) { return m_pBase ? m_pBase->ProcessMeasureJSON(xmlPath, jsonTemplate, jsonOutPath) : false; }
		virtual bool    onResultJSON(LPCSTR strJSON,LPCWSTR path) {	return m_pBase ? m_pBase->onResultJSON(strJSON,path) : false; }

		virtual bool	IsBusy() { return false; }
	
		virtual bool GetSavePhoto() { return m_pBase ? m_pBase->GetSavePhoto() : false; }
		virtual bool SetSavePhoto(bool bSave) { return m_pBase ? m_pBase->SetSavePhoto(bSave) : false; }
	};



	class VIENGINEEX_API CViEngineExLDWInterface
	{
	private:
		void *m_pApp = 0;
	public:
		CViEngineExLDWInterface(void *pApp);
		~CViEngineExLDWInterface();
	public:
		virtual HWND GetWindow();

	};

	class VIENGINEEX_API CViEngineExGraphInterface
	{
	private:
		void *m_pg = 0;
	public:
		CViEngineExGraphInterface(void *pg);
		~CViEngineExGraphInterface();
	public:
		HWND GetWindow();

		int	 GetCount();
		void RemoveAll();
		void AddGraph(int id, int order = 0);

		int GetID(int pos);

		void SetPeriod(double dt);
		double GetPeriod();

		void SetName(int id, LPCWSTR name);
		LPCWSTR GetName(int id);

		void SetTag(int id, LPCWSTR name);
		LPCWSTR GetTag(int id);

		bool	GetDiffMode(int id);
		double	GetDiffPeriod(int id);

		bool	SetDiffMode(int id, bool bSet);
		bool	SetDiffPeriod(int id, double v);

		bool SetMinMax(int id, float vMin, float vMax, bool bAuto);
		bool GetMinMax(int id, float *pMin, float *pMax);

		bool PushVar(int id, double t, float v);
		bool PushVar(int id, ULONGLONG t, float v);

		bool IsEnabled(int id);
		bool Enable(int id, bool bEnable = true);

		void SetMarker(int id, DWORD c, LPCWSTR tag = 0);

		void RegLoad(int id);
		void RegSave(int id);

		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	};

	class VIENGINEEX_API CViEngineExHistInterface
	{
	private:
		void *m_ph = 0;
	public:
		CViEngineExHistInterface(void *ph);
		~CViEngineExHistInterface();
	public:
		HWND GetWindow();
		bool AddHist(int idg, int id,int width,int dt=0);
	};

	class VIENGINEEX_API CViEngineExStatInterface
	{
	private:
		void *m_ph = 0;
	public:
		CViEngineExStatInterface(void *ph);
		~CViEngineExStatInterface();
	public:
		HWND GetWindow(LPCWSTR sid);
		int GetBioTesterXML(LPSTR strXML, int cch);
		int MsgID();
	};

	class VIENGINEEX_API CViEngineExMeasureInterface
	{
	private:
		void *m_pApp = 0;
	public:
		CViEngineExMeasureInterface(void *pApp);
		~CViEngineExMeasureInterface();

		////////////////////////////////////////////////////////////////////////
		// GetResultXML:
		//	Get BioTester results (XML format)
		// Parameters:
		//	strXML: 
		//		string storing results 
		//	cch: 
		//		max string lenght
		// Return value: actual text lenght 
		////////////////////////////////////////////////////////////////////////
		int GetResultXML(LPSTR strXML, int cch);

		bool IsStarted();
		bool IsReady();

		void Start(double period=-1);
		void Stop(bool bClear);
		void Clear();
	};

	class VIENGINEEX_API CViEngineExAutoCaptureInterface
	{
	private:
		void *m_pApp = 0;
	public:
		CViEngineExAutoCaptureInterface();
		CViEngineExAutoCaptureInterface(CVIENGINE_EX_CLASS *pApp);
		~CViEngineExAutoCaptureInterface();

		void Connect(CVIENGINE_EX_CLASS *pApp);
	public:
		virtual bool	OnImageSave(bool bAuto, float state = -1) { return false; }
		virtual bool	OnCriticalState(int mode=0,float state=-1) { return false; }
		virtual float	GetDT() { return 0; }
		virtual void	SetDT(float dt) {}
		virtual int		GetHideT() { return 0; }
		virtual void	SetHideT(float dt) {}
		virtual int		GetImageCount() { return 0; }
		virtual int		GetImageCountMax() { return 0; }
		virtual void	SetImageCountMax(int n) { }
		virtual HBITMAP	GetImage(int pos, int id) { return NULL; }
		virtual bool	IsAutoCapture() { return false; }
		virtual HWND	GetWindow() { return 0; }
		virtual bool	GetFileCapture() { return false; }
		virtual void	SetFileCapture(bool bCapture) { }
		virtual bool	GetFileCaptureXML() { return false; }
		virtual void	SetFileCaptureXML(bool bCapture) { }
		virtual bool	GetFileCaptureFace() { return false; }
		virtual void	SetFileCaptureFace(bool bCapture) { }
		virtual bool	GetFileCaptureOriginal() { return false; }
		virtual void	SetFileCaptureOriginal(bool bCapture) { }
		virtual bool	GetFilePrintSuspect() { return false; }
		virtual void	SetFilePrintSuspect(bool bPrint) { }
		virtual int		GetFileCaptureGroupBy() { return 0; }
		virtual void	SetFileCaptureGroupBy(int v) { }

		virtual void	CheckAndPurge() {}
		virtual void	OnFaceDetected(VI_FACE_INFO_DATA *pInfo) {}
		virtual void	RegLoad(void) {}
		virtual void	RegSave(void) {}
		virtual LPCWSTR GetImageExt() { return L"png"; }

		virtual int	GetCaptured(HBITMAP *pBM, ULONGLONG*pT, DWORD *pID, int nMax) { return 0; }

		virtual void	SetStatus(LPCWSTR sessionName, LPCSTR status) {}

		virtual bool	CheckCritical(CImage& imgSrc, float level) { return true; }
	};

	class VIENGINEEX_API CVIENGINE_EX_CLASS
	{
	public:
		void *m_pApp = 0;	// Reference to internal core storage
	public:
		CVIENGINE_EX_CLASS();	// default constructor
		~CVIENGINE_EX_CLASS(); // default destructor
	public:
		////////////////////////////////////////////////////////////////////////
		// OpenDocument:
		//	Open specified source file
		// Parameters:
		//	path: 
		//		1) AVI file path like "D:\test.avi"
		//		2) Video source like "*VIDEO:*menu*1", "*VIDEO:My Camera 1"
		//		3) LDQ file like "D:\Child1.ldq"
		// Return value: retruns 0
		////////////////////////////////////////////////////////////////////////
		int		OpenDocument(LPCWSTR path);

		////////////////////////////////////////////////////////////////////////
		// GetCurrentDocument:
		// Receive last opened video source
		// Parameters:
		//	path: pointer to string 
		//	cch: the size of string
		// Returns "true" if succeded
		////////////////////////////////////////////////////////////////////////
		bool	GetCurrentDocument(LPWSTR path, int cch);

		////////////////////////////////////////////////////////////////////////
		// Start
		// Prform initialization and start engine
		// Parameters:
		//	hWndParent: handle to parent window
		// Returns: handle to engine window
		////////////////////////////////////////////////////////////////////////
		HWND	Start(HWND hWndParent = 0);

		////////////////////////////////////////////////////////////////////////
		// Stop:
		//	Stop engine and clear resources.
		////////////////////////////////////////////////////////////////////////
		void	Stop();
			
		////////////////////////////////////////////////////////////////////////
		// IsStarted:
		//	return true if started
		////////////////////////////////////////////////////////////////////////
		bool IsStarted();

		////////////////////////////////////////////////////////////////////////
		// StopAndUnregister:
		//	Stop engine, clear resources and remove license.
		////////////////////////////////////////////////////////////////////////
		bool	StopAndUnregister();
	public:

		////////////////////////////////////////////////////////////////////////
		// GetVersion:
		// Returns engine version. Currently 1
		////////////////////////////////////////////////////////////////////////
		unsigned int GetVersion(int id);

		////////////////////////////////////////////////////////////////////////
		// Return configuration folder. By default returns "config". 
		// It means <Applications folder>\config
		////////////////////////////////////////////////////////////////////////
		virtual LPCWSTR GetConfigBase();

		////////////////////////////////////////////////////////////////////////
		// GetVar:
		// Returns parameter with specified ID, subID ant vt
		////////////////////////////////////////////////////////////////////////
		COleVariant GetVar(int id, int subID, VARTYPE vt, int nVar = 1);

		////////////////////////////////////////////////////////////////////////
		// PutVar:
		// Set parameter value with specified ID, subID ant vt
		////////////////////////////////////////////////////////////////////////
		bool	PutVar(int id, int subID, const COleVariant& v, int nVar = 1);

		////////////////////////////////////////////////////////////////////////
		// UpdateColor:
		// Get default color for parameter's menu with specified ID, subID 
		// Returns "true" if succeded
		////////////////////////////////////////////////////////////////////////
		bool	UpdateColor(int id, int subID, int ref, COLORREF *pColor);

		////////////////////////////////////////////////////////////////////////
		// CanEdit:
		// Returns true if value is writable
		////////////////////////////////////////////////////////////////////////
		bool CanEdit(int id);
		bool CanEdit(const char * tag);

		////////////////////////////////////////////////////////////////////////
		// OnMenu:
		//	Perform menu action with specified ID
		// Returns "true" if succeded
		////////////////////////////////////////////////////////////////////////
		bool	OnMenu(int id);

		////////////////////////////////////////////////////////////////////////
		// GetMenuState:
		//	Get menu item state and text with specified ID
		// Returns "true" if succeded
		////////////////////////////////////////////////////////////////////////
		bool	GetMenuState(int id, BOOL& bEnabled, BOOL& bChecked, LPWSTR pText = 0);


		////////////////////////////////////////////////////////////////////////
		// GetWindow:
		//	Returns: handle to main window
		////////////////////////////////////////////////////////////////////////
		HWND	GetWindow();

		////////////////////////////////////////////////////////////////////////
		// PreTranslateMessage:
		// pMsg :A pointer to a MSG structure that contains the message to process.
		// Returns: Nonzero if the message was fully processed in PreTranslateMessage and should not be processed further. Zero if the message should be processed in the normal way.
		////////////////////////////////////////////////////////////////////////
		BOOL PreTranslateMessage(MSG* pMsg);
	public:

		////////////////////////////////////////////////////////////////////////
		// Tag2Id:
		// Convert string sID to integer ID
		// Returns: ID of specified parameter
		// Example Tag2Id("VI_VAR_SIZE") returns 3
		////////////////////////////////////////////////////////////////////////
		int Tag2Id(LPCWSTR sid,bool bNew = true);

		////////////////////////////////////////////////////////////////////////
		// AddId:
		// Register string sID with integer ID
		// Returns: ID of specified parameter
		// Example Tag2Id("VI_VAR_SIZE") returns 3
		////////////////////////////////////////////////////////////////////////
		int AddId(int id, LPCWSTR sid);

		////////////////////////////////////////////////////////////////////////
		// Id2Tag:
		// Convert  integer ID to string sID 
		// Returns: ID of specified parameter or NULL
		////////////////////////////////////////////////////////////////////////
		LPCWSTR Id2Tag(int id);
	public:

		////////////////////////////////////////////////////////////////////////
		// CAMCapWnd:
		// Returns video capture interface
		////////////////////////////////////////////////////////////////////////
		CAMCapWnd *	GetVideo();

#if defined VIENGINEEX_GET_ENGINE
		////////////////////////////////////////////////////////////////////////
		// GetEngine:
		// Returns VIEngine core interface
		////////////////////////////////////////////////////////////////////////
		CVIENGINE_CLASS *	GetEngine();
#else
		////////////////////////////////////////////////////////////////////////
		// GetEngine:
		// Returns VIEngine core interface
		////////////////////////////////////////////////////////////////////////
		CViEngineCore* GetEngine() { return GetEngineCore(); }
#endif
		CViEngineCore* GetEngineCore();

		////////////////////////////////////////////////////////////////////////
		// CViEngineExGraphInterface:
		// Returns graph window interface
		////////////////////////////////////////////////////////////////////////
		CViEngineExGraphInterface*			GetGraph();

		////////////////////////////////////////////////////////////////////////
		// CViEngineExHistInterface:
		// Returns histogram window interface
		////////////////////////////////////////////////////////////////////////
		CViEngineExHistInterface*			GetHist();

		////////////////////////////////////////////////////////////////////////
		// CViEngineExMeasureInterface:
		// Returns measurement (M mode) interface
		////////////////////////////////////////////////////////////////////////

#if  !defined SEQ_MEDIC_MOL
		CViEngineExMeasureInterface*		GetMeasure();
#endif
		////////////////////////////////////////////////////////////////////////
		// CViEngineExLDWInterface:
		// Returns LD window (LD mode) interface
		////////////////////////////////////////////////////////////////////////
#if  !defined SEQ_MEDIC_MOL
		CViEngineExLDWInterface*			GetLDW();
#endif


		////////////////////////////////////////////////////////////////////////
		// CViEngineExStatInterface:
		// Returns stat window interface
		// n = [0,1,2],-1(HT),-2(live)
		////////////////////////////////////////////////////////////////////////
#if  !defined SEQ_MEDIC_MOL
		CViEngineExStatInterface*			GetStat(int n=-1);
#endif
		////////////////////////////////////////////////////////////////////////
		// CViEngineExLangInterface:
		// Returns language translation interface
		////////////////////////////////////////////////////////////////////////
		CViEngineExLangInterface*			GetLang();

		////////////////////////////////////////////////////////////////////////
		// CViEngineExRegInterface:
		// Returns registry storage interface
		////////////////////////////////////////////////////////////////////////
		CViEngineExRegInterface*			GetReg();

		////////////////////////////////////////////////////////////////////////
		// CViEngineExAutoCaptureInterface:
		// Returns image acpture window interface
		////////////////////////////////////////////////////////////////////////
		CViEngineExAutoCaptureInterface*	GetCapture();

		////////////////////////////////////////////////////////////////////////
		// CViEngineExDatabaseInterface:
		// Returns database storage interface
		////////////////////////////////////////////////////////////////////////
#if  !defined SEQ_MEDIC_MOL
		CViEngineExDatabaseInterface*		GetDB();
#endif

		////////////////////////////////////////////////////////////////////////
		// CViEngineExSeqInterface:
		// Returns security state interface
		////////////////////////////////////////////////////////////////////////
		CViEngineExSeqInterface * GetSeq();

	};


	class VIENGINEEX_API CViEngineExTools
	{
	public:
		////////////////////////////////////////////////////////////////////////
		// free:
		// release memory block
		////////////////////////////////////////////////////////////////////////
		static void free_mem(void *ptr);

		////////////////////////////////////////////////////////////////////////
		// pack_image:
		// pack image data to specified format
		// pData - bitmap data
		// w,h - image size
		// bpp - bitcount
		// format - one of "BMP", "PNG", "JPG"
		// pSize - pointer to image data size (out)
		// Returns - pointer to image data. You should release it by "free(ptr)"
		////////////////////////////////////////////////////////////////////////
		static void * pack_image(void *pData, int w, int h, int bpp, char *format, int *pSize);

		////////////////////////////////////////////////////////////////////////
		// rectangle:
		// draw rectangle
		// pData - bitmap data
		// w,h - image size
		// bpp - bitcount
		// r - rectangle
		// c - color
		
		////////////////////////////////////////////////////////////////////////
		static void  rectangle(void *pData, int w, int h, int bpp, const RECT& r, COLORREF c);

	};
}