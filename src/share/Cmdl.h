#pragma once
#include "afxwin.h"
#include <vector>

class CCmdl :
	public CCommandLineInfo
{
public:
	std::vector<CString> m_cmdl;
public:
	CCmdl(void);
	~CCmdl(void);

public:
	virtual void ParseParam(const TCHAR* pszParam,BOOL bFlag,BOOL bLast);
	bool IsSet(LPCSTR param, int count = 1) { return IsSet(CString(param), count); }
	bool IsSet(LPCWSTR param,int count=1);
	CString GetParam(int pos);
	CString GetParam(LPCSTR param, int shift = 1) { return GetParam(CString(param), shift); }
	CString GetParam(LPCWSTR param, int shift=1);
	CString GetString(LPCWSTR separator=_T(" "));
	void Replace(LPCWSTR param,LPCWSTR var,int shift=1);
	bool Remove(LPCWSTR param, int cnt);
};
