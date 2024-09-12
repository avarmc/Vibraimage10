#pragma once

#include <map>
class CWinAppExReg :
	public CWinAppEx
{
	std::recursive_mutex	m_regLock;
	std::map< CString, HKEY > m_regKeys;
public:
	CWinAppExReg() {}
	~CWinAppExReg();

	void RegFlush();

	HKEY GetSectionKey(LPCTSTR lpszSection, CAtlTransactionManager* pTM=0);

	UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
	CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault);
	BOOL GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, BYTE** ppData, UINT* pBytes);
	BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
	BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	BOOL WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);
};

