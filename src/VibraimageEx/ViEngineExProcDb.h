#pragma once
#include "ViEngineExSDK.h"

class CViHTTPStorage;
class CViFTPStorage;

using namespace  CViEngineExNS;

class CViEngineExProcDb : public CViEngineExDatabaseInterface
{
protected:
	CViEngineExDatabaseInterface *m_pBase;
public:
	int		m_id;
	class SESSIONREF {
	public:
		SESSIONREF():pHttp(0),t(GetTickCount64()){}
		~SESSIONREF();
		ULONGLONG		t;
		CViHTTPStorage* pHttp = 0;
		std::shared_ptr<CViFTPStorage> pFtp;
		std::list<CString> files;
	};
	std::map<CString, SESSIONREF> m_sessionMap;
	CCriticalSection			  m_sessionMapLock;
public:
	CViEngineExProcDb();
	~CViEngineExProcDb();

public:
	void Connect(int core, CVIENGINE_EX_CLASS *pApp);

	virtual bool	WriteFile(LPCWSTR tag, const void *lpBuf, UINT  nCount, bool bLock = true);
	virtual bool	CheckItemID(LPCWSTR id, bool bValidate);

	virtual bool	StartSession(LPCWSTR prefix = 0, LPCWSTR postfix = 0);
	virtual bool	StopSession(bool bLock = true);

	virtual void onHttpResult(CString sessionName,CStringA msg);
	virtual bool	OnSessionStopped(LPCWSTR name);

	virtual std::list<CString> GetFiles(LPCWSTR name);

	void PurgeSessions(ULONGLONG dt);
};

