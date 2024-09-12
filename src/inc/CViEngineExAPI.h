#pragma once



#ifdef VIENGINEEXAPI_EXPORTS
#define VIENGINEEXAPI_API __declspec(dllexport)
#else
#define VIENGINEEXAPI_API __declspec(dllimport)
#endif

class VIENGINEEXAPI_API CViEngineExAPI
{
	void* m_pRef;
public:
	CViEngineExAPI();
	~CViEngineExAPI();

	virtual bool Start();
	virtual void Stop();


	////////////////////////////////////////////////////////////////////////
	// Return configuration folder. By default returns "config". 
	// It means <Applications folder>\config
	////////////////////////////////////////////////////////////////////////
	const wchar_t* GetConfigBase() { return 0; }

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
	virtual int OpenDocument(const wchar_t* doc);

	virtual void OnNewVar(int id, int subID) {}
	
	virtual int OnVideoFrame(double dblSampleTime, unsigned char* pBuffer, long lBufferSize, void* pFormat);

	virtual void menuAction(int id);
	virtual void menuAction(const wchar_t* tag);
	virtual void menuStatus(int id,int& bEnabled, int& bChecked);
	virtual void menuStatus(const wchar_t* tag, int& bEnabled, int& bChecked);

	virtual int Tag2Id(const wchar_t* tag);
	virtual const wchar_t* Id2Tag(int id);

	virtual float GetVarF1(int id, int subID = 0, int nVar = 1);
	virtual float GetVarF1(const wchar_t* tag, int subID = 0, int nVar = 1);

	virtual int GetVarI1(int id, int subID = 0,int nVar=1);
	virtual int GetVarI1(const wchar_t* tag, int subID = 0, int nVar = 1);

	virtual const wchar_t* GetVarStr(int id, int subID = 0, int nVar = 1);
	virtual const wchar_t* GetVarStr(const wchar_t* tag, int subID = 0, int nVar = 1);

	virtual void PutVar(float v, int id, int subID = 0, int nVar = 1);
	virtual void PutVar(float v, const wchar_t* tag, int subID = 0, int nVar = 1);

	virtual void PutVar(int v, int id, int subID = 0, int nVar = 1);
	virtual void PutVar(int v, const wchar_t* tag, int subID = 0, int nVar = 1);

	virtual void PutVar(const wchar_t* v, int id, int subID = 0, int nVar = 1);
	virtual void PutVar(const wchar_t* v, const wchar_t* tag, int subID = 0, int nVar = 1);

	const wchar_t* GetLicense(const wchar_t* tag);

	virtual bool	canWrite(const wchar_t* tag) { return false; }
	virtual bool	WriteFile(const wchar_t* tag, const void* lpBuf, int  nCount, bool bLock = true) { return false; }
	virtual bool	GetBasePath(wchar_t* path, int cch_max) { return false; }
	virtual bool	GetItemBasePath(int n, wchar_t* base, int cch_max) { return false; }
	virtual bool	GetSessionFilePath(const wchar_t* tag, wchar_t* path, int cch) { return false; }

	virtual bool	NewItemName(const wchar_t* def_name, wchar_t* new_name, int cch) { return false; }
	virtual bool	OnSessionStart() { return false; }
	virtual void	OnSessionAbort() {}

	virtual void	OnSessionStopped(const wchar_t* name) {}
	virtual void	OnSessionClosed(const wchar_t* tag) {}

	virtual bool	IsSessionStarted();
};

