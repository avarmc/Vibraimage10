#pragma once
#include "ViEngineExSDK.h"

using namespace  CViEngineExNS;

class CViEngineExProcReg :	public CViEngineExRegInterface
{
protected:
	CString	m_regBase,m_regBaseApp;
public:
	int		m_id;
public:
	CViEngineExProcReg();
	virtual ~CViEngineExProcReg();
public:
	void Connect(int core, CVIENGINE_EX_CLASS *pApp) { m_id = core; CViEngineExRegInterface::Connect(pApp); }

	virtual LPCWSTR GetRegBase(bool bIncludeApp=false);
	virtual int GetProfileIntEx(LPCWSTR gr, LPCWSTR key, int def = 0);
	virtual void WriteProfileIntEx(LPCWSTR gr, LPCWSTR key, int v);

	virtual BOOL GetProfileStringEx(LPCWSTR gr, LPCWSTR key, LPWSTR res, int cch, LPCWSTR def = _T(""));
	virtual void WriteProfileStringEx(LPCWSTR gr, LPCWSTR key, LPCWSTR v);

	virtual int GetProfileBinEx(LPCWSTR gr, LPCWSTR key, void *ptr, int size_max);
	virtual void WriteProfileBinEx(LPCWSTR gr, LPCWSTR key, void *ptr, int size);

	virtual void OnSettingsLoad(LPCSTR szXmlStr);

	virtual bool IsLocked();

	virtual void MakeSave();
	virtual void MakeLoad();

	virtual CString RegGroup(LPCWSTR gr);

	virtual void Flush();
};

