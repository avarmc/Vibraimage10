#include "stdafx.h"
#include "CWinAppExReg.h"


CWinAppExReg::~CWinAppExReg() {
	RegFlush();
}

void CWinAppExReg::RegFlush() {
	std::unique_lock<std::recursive_mutex> lock(m_regLock);
	for (auto& k : m_regKeys)
		RegCloseKey(k.second);
	m_regKeys.clear();
}

// returns key for:
//      HKEY_CURRENT_USER\"Software"\RegistryKey\AppName\lpszSection
// creating it if it doesn't exist.
// responsibility of the caller to call RegCloseKey() on the returned HKEY
HKEY CWinAppExReg::GetSectionKey(LPCTSTR lpszSection, CAtlTransactionManager* pTM)
{
	ASSERT(lpszSection != NULL);
	std::unique_lock<std::recursive_mutex> lock(m_regLock);

	auto tmpKey = m_regKeys.find(lpszSection);
	if (tmpKey != m_regKeys.end())
		return tmpKey->second;

	HKEY hSectionKey = NULL;
	HKEY hAppKey = GetAppRegistryKey(pTM);
	if (hAppKey == NULL)
		return NULL;

	DWORD dw;

	if (pTM != NULL)
	{
		pTM->RegCreateKeyEx(hAppKey, lpszSection, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &hSectionKey, &dw);
	}
	else
	{
		::RegCreateKeyEx(hAppKey, lpszSection, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &hSectionKey, &dw);
	}

	RegCloseKey(hAppKey);
	return m_regKeys[lpszSection] = hSectionKey;
}

UINT CWinAppExReg::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	int nDefault)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
	if (m_pszRegistryKey != NULL) // use registry
	{
		HKEY hSecKey = GetSectionKey(lpszSection);
		if (hSecKey == NULL)
			return nDefault;
		DWORD dwValue;
		DWORD dwType;
		DWORD dwCount = sizeof(DWORD);
		LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
			(LPBYTE)& dwValue, &dwCount);
	//	RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_DWORD);
			ASSERT(dwCount == sizeof(dwValue));
			return (UINT)dwValue;
		}
		return nDefault;
	}
	else
	{
		ASSERT(m_pszProfileName != NULL);
		return ::GetPrivateProfileInt(lpszSection, lpszEntry, nDefault,
			m_pszProfileName);
	}
}

CString CWinAppExReg::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	LPCTSTR lpszDefault)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
	if (m_pszRegistryKey != NULL)
	{
		HKEY hSecKey = GetSectionKey(lpszSection);
		if (hSecKey == NULL)
			return lpszDefault;
		CString strValue;
		DWORD dwType = REG_NONE;
		DWORD dwCount = 0;
		LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
			NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_SZ);
			lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
				(LPBYTE)strValue.GetBuffer(dwCount / sizeof(TCHAR)), &dwCount);
			strValue.ReleaseBuffer();
		}
	//	RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_SZ);
			return strValue;
		}
		return lpszDefault;
	}
	else
	{
		ASSERT(m_pszProfileName != NULL);

		if (lpszDefault == NULL)
			lpszDefault = _T("");	// don't pass in NULL
		TCHAR szT[4096];
		DWORD dw = ::GetPrivateProfileString(lpszSection, lpszEntry,
			lpszDefault, szT, _countof(szT), m_pszProfileName);
		ASSERT(dw < 4095);
		return szT;
	}
}

BOOL CWinAppExReg::GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	BYTE** ppData, UINT* pBytes)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
	ASSERT(ppData != NULL);
	ASSERT(pBytes != NULL);
	*ppData = NULL;
	*pBytes = 0;
	if (m_pszRegistryKey != NULL)
	{
		HKEY hSecKey = GetSectionKey(lpszSection);
		if (hSecKey == NULL)
		{
			return FALSE;
		}

		// ensure destruction
		// ::ATL::CRegKey rkSecKey(hSecKey);

		DWORD dwType = 0;
		DWORD dwCount = 0;
		LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType, NULL, &dwCount);
		*pBytes = dwCount;
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_BINARY);
			*ppData = new BYTE[*pBytes];
			lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
				*ppData, &dwCount);
		}
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_BINARY);
			return TRUE;
		}
		else
		{
			delete[] * ppData;
			*ppData = NULL;
		}
		return FALSE;
	}
	else
	{
		ASSERT(m_pszProfileName != NULL);

		CString str = GetProfileString(lpszSection, lpszEntry, NULL);
		if (str.IsEmpty())
			return FALSE;
		ASSERT(str.GetLength() % 2 == 0);
		INT_PTR nLen = str.GetLength();
		*pBytes = UINT(nLen) / 2;
		*ppData = new BYTE[*pBytes];
		for (int i = 0; i < nLen; i += 2)
		{
			(*ppData)[i / 2] = (BYTE)
				(((str[i + 1] - 'A') << 4) + (str[i] - 'A'));
		}
		return TRUE;
	}
}


BOOL CWinAppExReg::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	int nValue)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
	if (m_pszRegistryKey != NULL)
	{
		HKEY hSecKey = GetSectionKey(lpszSection);
		if (hSecKey == NULL)
			return FALSE;
		LONG lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_DWORD,
			(LPBYTE)& nValue, sizeof(nValue));
	//	RegCloseKey(hSecKey);
		return lResult == ERROR_SUCCESS;
	}
	else
	{
		ASSERT(m_pszProfileName != NULL);

		TCHAR szT[16];
		_stprintf_s(szT, _countof(szT), _T("%d"), nValue);
		return ::WritePrivateProfileString(lpszSection, lpszEntry, szT,
			m_pszProfileName);
	}
}

BOOL CWinAppExReg::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	LPCTSTR lpszValue)
{
	ASSERT(lpszSection != NULL);
	if (m_pszRegistryKey != NULL)
	{
		LONG lResult;
		if (lpszEntry == NULL) //delete whole section
		{
			HKEY hAppKey = GetAppRegistryKey();
			if (hAppKey == NULL)
				return FALSE;
			lResult = ::RegDeleteKey(hAppKey, lpszSection);
			RegCloseKey(hAppKey);
		}
		else if (lpszValue == NULL)
		{
			HKEY hSecKey = GetSectionKey(lpszSection);
			if (hSecKey == NULL)
				return FALSE;
			// necessary to cast away const below
			lResult = ::RegDeleteValue(hSecKey, (LPTSTR)lpszEntry);
		//	RegCloseKey(hSecKey);
		}
		else
		{
			HKEY hSecKey = GetSectionKey(lpszSection);
			if (hSecKey == NULL)
				return FALSE;
			lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_SZ,
				(LPBYTE)lpszValue, (static_cast<DWORD>(_tcslen(lpszValue)) + 1) * sizeof(TCHAR));
		//	RegCloseKey(hSecKey);
		}
		return lResult == ERROR_SUCCESS;
	}
	else
	{
		ASSERT(m_pszProfileName != NULL);
		ASSERT(AtlStrLen(m_pszProfileName) < 4095); // can't read in bigger
		return ::WritePrivateProfileString(lpszSection, lpszEntry, lpszValue,
			m_pszProfileName);
	}
}

BOOL CWinAppExReg::WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	LPBYTE pData, UINT nBytes)
{
	ASSERT(lpszSection != NULL);
	if (m_pszRegistryKey != NULL)
	{
		LONG lResult;
		HKEY hSecKey = GetSectionKey(lpszSection);
		if (hSecKey == NULL)
			return FALSE;
		lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_BINARY,
			pData, nBytes);
	//	RegCloseKey(hSecKey);
		return lResult == ERROR_SUCCESS;
	}

	// convert to string and write out
	LPTSTR lpsz = new TCHAR[nBytes * 2 + 1];
	UINT i;
	for (i = 0; i < nBytes; i++)
	{
		lpsz[i * 2] = (TCHAR)((pData[i] & 0x0F) + 'A'); //low nibble
		lpsz[i * 2 + 1] = (TCHAR)(((pData[i] >> 4) & 0x0F) + 'A'); //high nibble
	}
	lpsz[i * 2] = 0;

	ASSERT(m_pszProfileName != NULL);

	BOOL bResult = WriteProfileString(lpszSection, lpszEntry, lpsz);
	delete[] lpsz;
	return bResult;
}
