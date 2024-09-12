#pragma once

class CVIEngineCore : public CVIENGINE_CLASS
{
public:
	CVIEngineCore() {}

	virtual int GetProfileIntEx(LPCWSTR gr, LPCWSTR key, int def = 0) { return def; }
	virtual void WriteProfileIntEx(LPCWSTR gr, LPCWSTR key, int v) {};

	virtual BOOL GetProfileStringEx(LPCWSTR gr, LPCWSTR key, LPWSTR res, int cch, LPCWSTR def = _T("")) { return wcscpy_s(res,cch,def ? def :L""); }
	virtual void WriteProfileStringEx(LPCWSTR gr, LPCWSTR key, LPCWSTR v) {};
};

