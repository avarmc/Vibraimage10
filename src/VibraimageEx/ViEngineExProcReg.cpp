#include "stdafx.h"
#include "ViEngineExProcReg.h"
#include "VibraimageEx.h"
#include "VibraimageExFrame.h"
#include "CViHTTPStorage.h"


CViEngineExProcReg::CViEngineExProcReg():m_id(0)
{
}


CViEngineExProcReg::~CViEngineExProcReg()
{
}

int CViEngineExProcReg::GetProfileBinEx(LPCWSTR gr, LPCWSTR key, void *ptr, int size_max)
{
	LPBYTE pb = 0;
	UINT sb = 0;

	if (ptr && size_max)
		ZeroMemory(ptr, size_max);

	if (!theApp.GetProfileBinary(gr, key, &pb, &sb))
	{
		return 0;
	}

	if (pb)
	{
		if (ptr)
			memcpy(ptr, pb, min((int)sb, size_max));

		delete pb;
	}
	return sb;
}

void CViEngineExProcReg::WriteProfileBinEx(LPCWSTR gr, LPCWSTR key, void *ptr, int size)
{
	if (!ptr) ptr = &ptr;
	if(theApp.m_bReady && !theApp.IsRegLocked()) theApp.WriteProfileBinary(gr, key, (LPBYTE)ptr,size);
}

int CViEngineExProcReg::GetProfileIntEx(LPCWSTR gr, LPCWSTR key, int def)
{
	return theApp.GetProfileInt(gr, key, def);
}

void CViEngineExProcReg::WriteProfileIntEx(LPCWSTR gr, LPCWSTR key, int v)
{
	if (theApp.m_bReady && !theApp.IsRegLocked()) theApp.WriteProfileInt(gr, key, v);
}

BOOL CViEngineExProcReg::GetProfileStringEx(LPCWSTR gr, LPCWSTR key, LPWSTR res, int cch, LPCWSTR def)
{
	ASSERT(gr);
	ASSERT(key);
	ASSERT(res);
	CString str = theApp.GetProfileString(gr, key, def);
	_tcsncpy_s(res, cch, str, cch);
	return TRUE;
}

void CViEngineExProcReg::WriteProfileStringEx(LPCWSTR gr, LPCWSTR key, LPCWSTR v)
{
	ASSERT(gr);
	ASSERT(key);
	if ((theApp.m_bReady && !theApp.IsRegLocked()) || CString(key) == _T("prev_login") )
		theApp.WriteProfileString(gr, key, v);
}



LPCWSTR CViEngineExProcReg::GetRegBase(bool bIncludeApp)
{
	if (m_regBase.IsEmpty())
	{
		m_regBase.Format(_T("%s\\%d"), GetRegBase0(), m_id);
	}

	if(m_regBaseApp.IsEmpty())
	{
		m_regBaseApp.Format(_T("%s\\%s\\%d"), GetRegBase0(), theApp.m_pszAppName, m_id);
	}

	if (bIncludeApp)
		return m_regBaseApp;


	return m_regBase;
}

void CViEngineExProcReg::OnSettingsLoad(LPCSTR szXmlStr)
{
	theApp.m_pFrame->OnSettingsLoad(szXmlStr);
	Flush();

	// Open default video
	if(theApp.m_bReady)
	{
		TCHAR gr[64] = { 0 };
		TCHAR doc[1024] = { 0 };
		RegCoreGroup(L"Settings", gr, 64);

		theApp.m_pEngines[m_id]->engine.GetReg()->GetProfileStringEx(gr, L"Document", doc, 1024);
		if (doc[0])
			theApp.OnOpenDocument(m_id, doc);
	}
}

bool CViEngineExProcReg::IsLocked() {
	return theApp.IsRegLocked();
}

CString CViEngineExProcReg::RegGroup(LPCWSTR gr) {
	wchar_t out[1024];
	RegCoreGroup(gr, out, 1024);
	return out;
}

void CViEngineExProcReg::MakeSave() {
	__super::MakeSave();
	CViHTTPStorage::RegSave(m_id);
	Flush();
}

void CViEngineExProcReg::MakeLoad() {
	__super::MakeLoad();
	CViHTTPStorage::RegLoad(m_id);
	Flush();
}

void CViEngineExProcReg::Flush() {
	theApp.RegFlush();
}