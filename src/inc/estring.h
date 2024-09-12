#pragma once

#include <list>
#include <vector>
#include <string>
#include <regex>
#include <Wincrypt.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include "pugixml.hpp"

#if defined _AFX
#include <afxinet.h>
#endif

template <class T>
inline std::list<T> vector2list(std::vector<T>& a) {
	std::list<T> r;
	for (std::vector<T>::iterator i = a.begin(), e = a.end(); i != e; ++i)
		r.push_back(*i);
	return r;
}

template <class T>
inline std::vector<T> list2vector(std::list<T>& a) {
	std::vector<T> r;
	r.reserve(a.size());

	for (std::list<T>::iterator i = a.begin(), e = a.end(); i != e; ++i)
		r.push_back(*i);
	return r;
}

template <class T>
inline std::vector<T> explode(T str,const T& sep)
{
	std::vector<T> arr;
	arr.reserve(str.GetLength()/16);
	while(!str.IsEmpty())
	{
		int pos = str.Find(sep);
		if(pos < 0)
			break;
		arr.push_back(str.Left(pos));
		str = str.Mid(pos+sep.GetLength());
		if (str.IsEmpty())
			arr.push_back(str);
	}
	if(!str.IsEmpty())
		arr.push_back(str);
	return arr;
}

template <class T>
inline std::list<T> explode_list(T str, const T& sep)
{
	std::list<T> arr;
	while (!str.IsEmpty())
	{
		int pos = str.Find(sep);
		if (pos < 0)
			break;
		arr.push_back(str.Left(pos));
		str = str.Mid(pos + sep.GetLength());
	}
	if (!str.IsEmpty())
		arr.push_back(str);
	return arr;
}

inline CStringA	int2strA(int v, LPCSTR fmt = "%i")
{
	CStringA str;
	str.Format(fmt, v);
	return str;
}

inline CStringW	int2str64(__int64 v, LPCWSTR fmt = L"%lld")
{
	CStringW str;
	str.Format(fmt, v);
	return str;
}

inline CStringW	int2str(int v, LPCWSTR fmt = L"%i")
{
	CStringW str;
	str.Format(fmt,v);
	return str;
}
inline CStringW	uint2str(int v)
{
	CStringW str;
	str.Format(L"%u", v);
	return str;
}

inline CStringW	flt2str(double v,LPCWSTR fmt=L"%lg")
{
	CStringW str;
	str.Format(fmt,v);
	return str;
}


inline CStringA	flt2strA(double v, LPCSTR fmt = "%lg")
{
	CStringA str;
	str.Format(fmt, v);
	return str;
}

inline int str2int(LPCTSTR str)
{
	if( _tcslen(str) >= 2 && str[0] == '0' && str[1] == 'x')
	{
		int v;
		_stscanf_s(str+2,_T("%X"),&v);
		return v;
	}
	return _tstoi(str);
}

inline double str2dbl(LPCTSTR str)
{
	return _tstof(str);
}

inline float str2flt(LPCTSTR str)
{
	return (float)_tstof(str);
}

#ifdef _UNICODE
inline CStringW LocaleNumber(LPCWSTR num)
{
	CStringW str = num;
	static TCHAR dot = 0;

	if(!dot)
	{
		CStringW tmp;
		
		if(! GetNumberFormat(LOCALE_USER_DEFAULT,0,	_T("0.0"),	0,	tmp.GetBufferSetLength(100),	100) )
		{
			return str;
		}

		tmp.Remove('0');

		if(tmp.GetLength() > 0)
			dot = tmp.GetAt(0);
		else
			dot = '.';
	}

	str.Replace('.',dot);

	return str;
}

inline CStringA LocaleNumber(LPCSTR num)
{
	return CStringA( LocaleNumber( CStringW(num) ) );
}
#else
inline CStringA LocaleNumber(LPCSTR num)
{
	CStringA str = num;
	static TCHAR dot = 0;

	if (!dot)
	{
		CStringA tmp;

		if (!GetNumberFormat(LOCALE_USER_DEFAULT, 0, _T("0.0"), 0, tmp.GetBufferSetLength(100), 100))
		{
			return str;
		}

		tmp.Remove('0');

		if (tmp.GetLength() > 0)
			dot = tmp.GetAt(0);
		else
			dot = '.';
	}

	str.Replace('.', dot);

	return str;
}

inline CStringW LocaleNumber(LPCWSTR num)
{
	return CStringW(LocaleNumber(CStringA(num)));
}
#endif

inline CStringW LocaleNumber(double v,LPCWSTR fmt)
{
	CStringW str;
	str.Format(fmt,v);
	return str;
}
inline CStringW AddSpaces( const CStringW& str,int newLen,bool bRear=true,TCHAR ch=' ')
{
	int len = str.GetLength();
	if(len >= newLen)
		return str;
	if(bRear)
		return str + CStringW(ch,newLen-len);
	return CStringW(ch,newLen-len)+str;
}

inline double xatof(CStringW str)
{
	str.Replace(',','.');
	return _wtof(str);
}

#ifdef _UNICODE
inline double xatof(CStringA str)
{
	str.Replace(',','.');
	return atof(str);
}
#endif

inline CStringA estr_ToHex(BYTE* data, int len)
{
	std::vector<char> str;
	str.resize((size_t)len * 4 + 2);

	memset(&str[0], 0, str.size());

	char tmp[8];
	char* Temp = &str[0];

	for (size_t k = 0; k < (size_t)len; ++k)
	{
		BYTE v = data[k];
		UINT u = v;
		sprintf_s(tmp, 8, "%0.2X", u);
		int k2 = (int)(k * 2);
		str[k2] = tmp[0];
		str[k2 + 1] = tmp[1];

		//		strcat_s(Temp, str.size(), tmp);
	}
	return &str[0];
}

inline std::string estr_ToHexStr(BYTE* data, int len)
{
	std::vector<char> str;
	str.resize(((size_t)len) * 4 + 1);

	memset(&str[0], 0, str.size());

	char tmp[8];
	char* Temp = &str[0];

	for (int k = 0; k < len; ++k)
	{
		BYTE v = data[k];
		UINT u = v;
		sprintf_s(tmp, 8, "%0.2X", u);
		int k2 = k * 2;
		str[k2] = tmp[0];
		str[k2 + 1] = tmp[1];
		//	strcat_s(Temp, str.size(), tmp);
	}
	return &str[0];
}


#if defined _UNICODE && defined _AFX 
inline bool estr_CryptInit(HCRYPTPROV* pc)
{
	HCRYPTPROV hCryptProv = NULL;
	LPCWSTR UserName = L"Vibraimage10";  // name of the key container
	// to be used
	//--------------------------------------------------------------------
	// Attempt to acquire a context and a key
	// container. The context will use the default CSP
	// for the RSA_FULL provider type. DwFlags is set to zero
	// to attempt to open an existing key container.

	if(pc) *pc = 0;
	if(CryptAcquireContext(
		&hCryptProv,               // handle to the CSP
		UserName,                  // container name 
		NULL,                      // use the default provider
		PROV_RSA_FULL,             // provider type
		0))                        // flag values
	{
		// printf("A cryptographic context with the %s key container \n", UserName);
		// printf("has been acquired.\n\n");
	}
	else
	{ 
		//--------------------------------------------------------------------
		// An error occurred in acquiring the context. This could mean
		// that the key container requested does not exist. In this case,
		// the function can be called again to attempt to create a new key 
		// container. Error codes are defined in Winerror.h.
		if (GetLastError() == NTE_BAD_KEYSET)
		{
			if(CryptAcquireContext(
				&hCryptProv, 
				UserName, 
				NULL, 
				PROV_RSA_FULL, 
				CRYPT_NEWKEYSET)) 
			{
				 // printf("A new key container has been created.\n");
			}
			else
			{
				// printf("Could not create a new key container.\n");
				return false;
			}
		}
		else
		{
			//printf("A cryptographic service handle could not be acquired.\n");
			return false;
		}

	} // End of else.
		//--------------------------------------------------------------------
	// A cryptographic context and a key container are available. Perform
	// any functions that require a cryptographic provider handle.

	if(pc) *pc = hCryptProv;

	return true;
}

inline int estr_md5(const char* str,BYTE *vHash)
{
	HCRYPTPROV hCryptProv;
	HCRYPTHASH hHash;

	int hl = 0;
	if(!estr_CryptInit(&hCryptProv))
		return 0;

	if(!CryptCreateHash(hCryptProv,CALG_MD5,0,0,&hHash))
		return 0;

	DWORD hLen = 256;
	ZeroMemory(vHash,hLen);

	if(CryptHashData(hHash,(BYTE*)str,(DWORD)strlen(str),CRYPT_USERDATA))
	{
		if(CryptGetHashParam(hHash,HP_HASHVAL,vHash,&hLen,0))
		{
			hl = hLen;
		}
	}

	if(hHash) 
		CryptDestroyHash(hHash);
	if(hCryptProv) 
		CryptReleaseContext(hCryptProv,0);
	return hl;
}
inline CStringA estr_md5(const char* str)
{
	BYTE vHash[256];
	memset(vHash, 0, sizeof(vHash));
	int size = estr_md5(str, vHash);
	return estr_ToHex(vHash, size);
}

#endif // #ifdef _UNICODE&& defined _AFX 


#if defined _AFX


inline COleDateTimeSpan str2dts(const CStringW& stra)
{
	int hh = 0, mm = 0, ss = 0, pos;

	if ((pos = stra.ReverseFind(':')) >= 0)
	{
		ss = _wtoi(stra.Mid(pos + 1));
		CStringW str( stra.Left(pos) );

		if ((pos = str.ReverseFind(':')) >= 0)
		{
			mm = _wtoi(str.Mid(pos + 1));
			hh = _wtoi(str.Left(pos));
		}
		else
			mm = _wtoi(str.Mid(pos + 1));
	}
	else
		ss = _wtoi(stra);

	COleDateTimeSpan r = COleDateTimeSpan(0, hh, mm, ss);
	if (stra.IsEmpty()) {
		r.SetStatus(COleDateTimeSpan::DateTimeSpanStatus::invalid);
	}
	return r;
}

#endif


inline CStringW v2str(const CStringW& v) { return v; }
inline CStringW v2str(double v) { CStringW str; str.Format(L"%g", v); return LocaleNumber(str); }
inline CStringW v2str(float v) { CStringW str; str.Format(L"%g", v); return LocaleNumber(str); }
inline CStringW v2str(__int64 v) { CStringW str; str.Format(L"%lli", v); return str; }
inline CStringW v2str(int v) { CStringW str; str.Format(L"%i", v); return str; }
inline CStringW v2str(long v) { CStringW str; str.Format(L"%i", v); return str; }
inline CStringW v2str(short v) { CStringW str; str.Format(L"%i", (int)v); return str; }
inline CStringW v2str(char v) { CStringW str; str.Format(L"%i", (int)v); return str; }
inline CStringW v2str(unsigned __int64 v) { CStringW str; str.Format(L"%llu", v); return str; }
inline CStringW v2str(unsigned int v) { CStringW str; str.Format(L"%u", (unsigned int)v); return str; }
inline CStringW v2str(unsigned long v) { CStringW str; str.Format(L"%u", (unsigned int)v); return str; }
inline CStringW v2str(unsigned short v) { CStringW str; str.Format(L"%u", (unsigned int)v); return str; }
inline CStringW v2str(unsigned char v) { CStringW str; str.Format(L"%u", (unsigned int)v); return str; }
inline CStringW v2str(const CPoint& v) { CStringW str; str.Format(L"%i;%i", v.x, v.y); return str; }
inline CStringW v2str(const CSize& v) { CStringW str; str.Format(L"%u;%u", v.cx, v.cy); return str; }
inline CStringW v2str(const CRect& v) { CStringW str; str.Format(L"%i;%i;%i;%i", v.left, v.top, v.right, v.bottom); return str; }

inline void str2v(const CStringW& str, CStringW& v) { v = str; }
inline void str2v(const CStringW& str, double& v) { v = xatof(str); }
inline void str2v(const CStringW& str, float& v) { v = (float)xatof(str); }
inline void str2v(const CStringW& str, __int64& v) { v = _wtoi64(str); }
inline void str2v(const CStringW& str, int& v) { v = _wtoi(str); }
inline void str2v(const CStringW& str, long& v) { v = (long)_wtoi(str); }
inline void str2v(const CStringW& str, short& v) { v = (short)_wtoi(str); }
inline void str2v(const CStringW& str, char& v) { v = (char)_wtoi(str); }
inline void str2v(const CStringW& str, unsigned __int64& v) { v = (unsigned __int64)_wtoi64(str); }
inline void str2v(const CStringW& str, unsigned int& v) { v = (unsigned int)_wtoi(str); }
inline void str2v(const CStringW& str, unsigned long& v) { v = (unsigned long)_wtoi(str); }
inline void str2v(const CStringW& str, unsigned short& v) { v = (unsigned short)_wtoi(str); }
inline void str2v(const CStringW& str, unsigned char& v) { v = (unsigned char)_wtoi(str); }
inline void str2v(const CStringW& str, CPoint& v)
{
	std::vector<CStringW> arr = explode(str, CStringW(_T(";")));
	if (arr.size() == 2)
	{
		str2v(arr[0], v.x);
		str2v(arr[1], v.y);
	}
}
inline void str2v(const CStringW& str, CSize& v)
{
	std::vector<CStringW> arr = explode(str, CStringW(_T(";")));
	if (arr.size() == 2)
	{
		str2v(arr[0], v.cx);
		str2v(arr[1], v.cy);
	}
}
inline void str2v(const CStringW& str, CRect& v)
{
	std::vector<CStringW> arr = explode(str, CStringW(_T(";")));
	if (arr.size() == 4)
	{
		str2v(arr[0], v.left);
		str2v(arr[1], v.top);
		str2v(arr[2], v.right);
		str2v(arr[3], v.bottom);
	}
}

inline CStringW StrRemoveMulti(CStringW str, LPCTSTR rem)
{
	while (*rem)
	{
		str.Remove(*rem);
		++rem;
	}
	return str;
}

inline CStringW StrFileName(const CStringW& str)
{
	return StrRemoveMulti(str,_T("\\,/\r\n\a\t%?&*<>'`\"|"));
}
#if  defined _AFX 
inline CStringW ReadURL(LPCTSTR url, LPTSTR pErrMsg = 0, int nMaxErr = 1024, int nMaxData= 65536)
{
	if (pErrMsg && nMaxErr > 0)
		*pErrMsg = 0;

	CStringW str;
	try
	{
		CInternetSession session;
		CStdioFile *pFile = session.OpenURL(url, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_DONT_CACHE);
		if (pFile)
		{
			std::vector<char> buf(nMaxData);
			
			UINT n = pFile->Read(&buf[0], (UINT)(buf.size() - 1));
			str = pugi::as_utf16(&buf[0]).c_str();
			pFile->Close();
		}
	}
	catch (CInternetException *e)
	{
		if (pErrMsg && nMaxErr > 0)
			e->GetErrorMessage(pErrMsg, nMaxErr);
	}

	return str;

}

#endif // && defined _AFX 

inline std::string UriDecode(const std::string & sSrc)
{
	// Note from RFC1630:  "Sequences which start with a percent sign
	// but are not followed by two hexadecimal characters (0-9, A-F) are reserved
	// for future extension"
	const char HEX2DEC[256] =
	{
		/*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
		/* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,

		/* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,

		/* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,

		/* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		/* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
	};

	const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
	const int SRC_LEN = (int)sSrc.length();
	const unsigned char * const SRC_END = pSrc + SRC_LEN;
	const unsigned char * const SRC_LAST_DEC = SRC_END - 2;   // last decodable '%' 

	char * const pStart = new char[SRC_LEN];
	char * pEnd = pStart;

	while (pSrc < SRC_LAST_DEC)
	{
		if (*pSrc == '%')
		{
			char dec1, dec2;
			if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
				&& -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
			{
				*pEnd++ = (dec1 << 4) + dec2;
				pSrc += 3;
				continue;
			}
		}

		*pEnd++ = *pSrc++;
	}

	// the last 2- chars
	while (pSrc < SRC_END)
		*pEnd++ = *pSrc++;

	std::string sResult(pStart, pEnd);
	delete[] pStart;
	return sResult;
}


inline std::string UriEncode(const std::string & sSrc)
{
	// Only alphanum is safe.
	const char SAFE[256] =
	{
		/*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
		/* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,

		/* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
		/* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
		/* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
		/* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,

		/* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

		/* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
	};

	const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
	const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
	const int SRC_LEN = (int)sSrc.length();
	unsigned char * const pStart = new unsigned char[(size_t)SRC_LEN * (size_t)3];
	unsigned char * pEnd = pStart;
	const unsigned char * const SRC_END = pSrc + SRC_LEN;

	for (; pSrc < SRC_END; ++pSrc)
	{
		if (SAFE[*pSrc])
			*pEnd++ = *pSrc;
		else
		{
			// escape this char
			*pEnd++ = '%';
			*pEnd++ = DEC2HEX[*pSrc >> 4];
			*pEnd++ = DEC2HEX[*pSrc & 0x0F];
		}
	}

	std::string sResult((char *)pStart, (char *)pEnd);
	delete[] pStart;
	return sResult;
}


inline std::string UriEncode(BYTE *pData, int size)
{
	// Only alphanum is safe.
	const char SAFE[256] =
	{
		/*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
		/* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,

		/* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
		/* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
		/* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
		/* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,

		/* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

		/* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		/* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
	};

	const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
	const unsigned char * pSrc = (const unsigned char *)pData;
	const int SRC_LEN = size;
	unsigned char * const pStart = new unsigned char[(size_t)SRC_LEN * (size_t)3];
	unsigned char * pEnd = pStart;
	const unsigned char * const SRC_END = pSrc + SRC_LEN;

	memset(pStart, 0, (size_t)SRC_LEN * (size_t)3);

	for (; pSrc < SRC_END; ++pSrc)
	{
		if (SAFE[*pSrc])
			*pEnd++ = *pSrc;
		else
		{
			// escape this char
			*pEnd++ = '%';
			*pEnd++ = DEC2HEX[*pSrc >> 4];
			*pEnd++ = DEC2HEX[*pSrc & 0x0F];
		}
	}

	std::string sResult((char *)pStart, (char *)pEnd);
	delete[] pStart;
	return sResult;
}



#if defined _AFX

inline COleVariant Var2Variant(LPCWSTR v, VARTYPE vt)
{
	COleVariant r;
	r.vt = vt;
	switch (vt)
	{
	case VT_I4:
	case VT_INT:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_I2:
	case VT_UI2:
	case VT_I1:
	case VT_UI1:
		r.llVal = _wtoi64(v);
		break;
	case VT_R4:
		r.fltVal = (float)_wtof(v);
		break;
	case VT_R8:
		r.dblVal = (double)_wtof(v);
		break;
	case VT_BOOL:
		r.boolVal = (_wtoi64(v)) ? -1 : 0;
		break;
	case VT_BSTR:
#ifdef _UNICODE
		r = COleVariant(v);
#else
		r = COleVariant(CStringA(v));
#endif
		break;
	default: break;
	}
	return r;
}


inline COleVariant Var2Variant(LONGLONG v, VARTYPE vt)
{
	COleVariant r;
	r.vt = vt;
	switch (vt)
	{
	case VT_I4:
	case VT_INT:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_I2:
	case VT_UI2:
	case VT_I1:
	case VT_UI1:
		r.llVal = v;
		break;
	case VT_R4:
		r.fltVal = (float)v;
		break;
	case VT_R8:
		r.dblVal = (double)v;
		break;
	case VT_BOOL:
		r.boolVal = (v) ? -1 : 0;
		break;
	case VT_BSTR:
	{
		TCHAR tmp[64];
		_stprintf_s(tmp, 64, _T("%lld"), v);
		r = COleVariant(tmp);
		break;
	}
	default: break;
	}
	return r;
}



inline COleVariant Var2Variant(int v, VARTYPE vt)
{
	COleVariant r;
	r.vt = vt;
	switch (vt)
	{
	case VT_I4:
	case VT_INT:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_I2:
	case VT_UI2:
	case VT_I1:
	case VT_UI1:
		r.llVal = v;
		break;
	case VT_R4:
		r.fltVal = (float)v;
		break;
	case VT_R8:
		r.dblVal = (double)v;
		break;
	case VT_BOOL:
		r.boolVal = (v) ? -1 : 0;
		break;
	case VT_BSTR:
	{
		TCHAR tmp[64];
		_stprintf_s(tmp, 64, _T("%i"), v);
		r = COleVariant(tmp);
		break;
	}
	default: break;
	}
	return r;
}

inline COleVariant Var2Variant(float v, VARTYPE vt)
{
	COleVariant r;
	r.vt = vt;
	switch (vt)
	{
	case VT_I4:
	case VT_INT:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_I2:
	case VT_UI2:
	case VT_I1:
	case VT_UI1:
		r.llVal = (LONGLONG)v;
		break;
	case VT_R4:
		r.fltVal = (float)v;
		break;
	case VT_R8:
		r.dblVal = (double)v;
		break;
	case VT_BOOL:
		r.boolVal = (v == 0) ? 0 : -1;
		break;
	case VT_BSTR:
	{
		TCHAR tmp[64];
		_stprintf_s(tmp, 64, _T("%g"), v);
		r = COleVariant(tmp);
		break;
	}
	default: break;
	}
	return r;
}

inline COleVariant Var2Variant(double v, VARTYPE vt)
{
	COleVariant r;
	r.vt = vt;
	switch (vt)
	{
	case VT_I4:
	case VT_INT:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_I2:
	case VT_UI2:
	case VT_I1:
	case VT_UI1:
		r.llVal = (LONGLONG)v;
		break;
	case VT_R4:
		r.fltVal = (float)v;
		break;
	case VT_R8:
		r.dblVal = (double)v;
		break;
	case VT_BOOL:
		r.boolVal = (v == 0) ? 0 : -1;
		break;
	case VT_BSTR:
	{
		TCHAR tmp[64];
		_stprintf_s(tmp, 64, _T("%g"), v);
		r = COleVariant(tmp);
		break;
	}
	default: break;
	}
	return r;
}
inline float Variant2float(const COleVariant& v)
{
	switch (v.vt)
	{
	case VT_R4: return (float)v.fltVal;
	case VT_R8: return (float)v.dblVal;

	case VT_I4: return (float)v.lVal;
	case VT_UI4: return (float)v.ulVal;
	case VT_UINT: return (float)v.uintVal;
	case VT_INT: return (float)v.intVal;
	case VT_I8: return (float)v.llVal;
	case VT_UI8: return (float)v.ullVal;
	case VT_I2: return (float)v.iVal;
	case VT_UI2: return (float)v.uiVal;
	case VT_I1: return (float)v.cVal;
	case VT_UI1: return (float)v.bVal;
	case VT_BOOL: return v.boolVal ? 1.0f : 0.0f;
	case VT_BSTR: return (float)_wtof(v.bstrVal);
	default: break;
	}

	return 0;
}

inline int Variant2int(const COleVariant& v)
{
	switch (v.vt)
	{
	case VT_I4: return (int)v.lVal;
	case VT_UI4: return (int)v.ulVal;
	case VT_UINT: return (int)v.uintVal;
	case VT_INT: return (int)v.intVal;
	case VT_I8: return (int)v.llVal;
	case VT_UI8: return (int)v.ullVal;
	case VT_I2: return (int)v.iVal;
	case VT_UI2: return (int)v.uiVal;
	case VT_I1: return (int)v.cVal;
	case VT_UI1: return (int)v.bVal;
	case VT_BOOL: return v.boolVal ? 1 : 0;
	case VT_BSTR: return _wtoi(v.bstrVal);
	case VT_R4: return (int)v.fltVal;
	case VT_R8: return (int)v.dblVal;

	default: break;
	}

	return 0;
}

inline CStringW Variant2str(const COleVariant& v)
{
	if (v.vt == VT_BSTR)
		return CStringW(v.bstrVal);

	CStringW str;

	switch (v.vt)
	{
	case VT_I4: str.Format(L"%i", v.lVal); break;
	case VT_UI4: str.Format(L"%u", v.ulVal); break;
	case VT_UINT: str.Format(L"%i", v.uintVal); break;
	case VT_INT: str.Format(L"%i", v.intVal); break;
	case VT_I8: str.Format(L"%I64i", v.llVal); break;
	case VT_UI8: str.Format(L"%I64u", v.ullVal); break;
	case VT_I2: str.Format(L"%i", (INT)v.iVal); break;
	case VT_UI2: str.Format(L"%u", (UINT)v.uiVal); break;
	case VT_I1: str.Format(L"%i", (INT)v.cVal); break;
	case VT_UI1: str.Format(L"%u", (UINT)v.bVal); break;
	case VT_BOOL: str.Format(L"%i", v.boolVal ? 1 : 0); break;
	case VT_R4: str.Format(L"%g", v.fltVal); break;
	case VT_R8: str.Format(L"%g", v.dblVal); break;

	default: break;
	}

	return str;
}


inline COleVariant Variant2Variant(const COleVariant &v, const COleVariant& to) {
	if (v.vt == to.vt)
		return v;

	switch (v.vt)
	{
	case VT_R4: return Var2Variant(v.fltVal,to.vt);
	case VT_R8:  return Var2Variant(v.dblVal,to.vt);

	case VT_I4: return Var2Variant((int)v.lVal, to.vt);
	case VT_UI4: return Var2Variant((LONGLONG)v.ulVal, to.vt);
	case VT_UINT: return Var2Variant((LONGLONG)v.uintVal, to.vt);
	case VT_INT: return Var2Variant(v.intVal, to.vt);
	case VT_I8: return Var2Variant(v.llVal, to.vt);
	case VT_UI8: return Var2Variant((LONGLONG)v.ullVal , to.vt);
	case VT_I2: return Var2Variant(v.iVal, to.vt);
	case VT_UI2: return Var2Variant(v.uiVal, to.vt);
	case VT_I1: return Var2Variant(v.cVal, to.vt);
	case VT_UI1: return Var2Variant(v.bVal, to.vt);
	case VT_BOOL: return Var2Variant(v.boolVal ? 1 : 0, to.vt);
	case VT_BSTR: return Var2Variant(v.bstrVal, to.vt);
	default: break;
	}
	return to;
}

#endif 

inline std::string base64_encode(const std::string& in) {

	std::string out;

	int val = 0, valb = -6;
	for (int jj = 0; jj < in.size(); jj++) {
		char c = in[jj];
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4) out.push_back('=');
	return out;
}

inline std::string base64_decode(const std::string& in) {

	std::string out;

	std::vector<int> T(256, -1);
	for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

	int val = 0, valb = -8;
	for (int jj = 0; jj < in.size(); jj++) {
		char c = in[jj];
		if (T[c] == -1) break;
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0) {
			out.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return out;
}

inline std::string AVAILABLE_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";

inline int index(char c) {
	for (int ii = 0; ii < AVAILABLE_CHARS.size(); ii++) {
		if (AVAILABLE_CHARS[ii] == c) {
			// std::cout << ii << " " << c << std::endl;
			return ii;
		}
	}
	return -1;
}


inline std::string extend_key(std::string& msg, std::string& key) {
	//generating new key
	int msgLen = msg.size();
	std::string newKey(msgLen, 'x');
	int keyLen = key.size(), i, j;
	for (i = 0, j = 0; i < msgLen; ++i, ++j) {
		if (j == keyLen)
			j = 0;

		newKey[i] = key[j];
	}
	newKey[i] = '\0';
	return newKey;
}


inline std::string encrypt_vigenere(std::string& msg, std::string& key) {
	int msgLen = msg.size(), keyLen = key.size(), i, j;
	std::string encryptedMsg(msgLen, 'x');
	// char newKey[msgLen], encryptedMsg[msgLen], decryptedMsg[msgLen];

	std::string newKey = extend_key(msg, key);

	//encryption
	for (i = 0; i < msgLen; ++i) {
		// std::cout << msg[i] << " " << isalnum(msg[i]) << std::endl;
		if (isalnum(msg[i]) || msg[i] == ' ') {
			encryptedMsg[i] = AVAILABLE_CHARS[((index(msg[i]) + index(newKey[i])) % AVAILABLE_CHARS.size())];
		}
		else {
			encryptedMsg[i] = msg[i];
		}
	}

	encryptedMsg[i] = '\0';
	return encryptedMsg;
}

inline std::string decrypt_vigenere(std::string& encryptedMsg, std::string& newKey) {
	// decryption
	int msgLen = encryptedMsg.size();
	std::string decryptedMsg(msgLen, 'x');
	int i;
	for (i = 0; i < msgLen; ++i) {
		if (isalnum(encryptedMsg[i]) || encryptedMsg[i] == ' ') {
			decryptedMsg[i] = AVAILABLE_CHARS[(((index(encryptedMsg[i]) - index(newKey[i])) + AVAILABLE_CHARS.size()) % AVAILABLE_CHARS.size())];
		}
		else {
			decryptedMsg[i] = encryptedMsg[i];
		}
	}
	decryptedMsg[i] = '\0';
	return decryptedMsg;
}

inline std::string str_encrypt(std::string& msg, std::string& key) {
	std::string b64_str = base64_encode(msg);
	std::string vigenere_msg = encrypt_vigenere(b64_str, key);
	// std::cout << vigenere_msg << std::endl;
	return vigenere_msg;
}


inline std::string str_decrypt(std::string& encrypted_msg, std::string& key) {
	std::string newKey = extend_key(encrypted_msg, key);
	std::string b64_encoded_str = decrypt_vigenere(encrypted_msg, newKey);
	std::string b64_decode_str = base64_decode(b64_encoded_str);
	return b64_decode_str;
}

inline bool str_is_integer(const std::string& s) {
	return std::regex_match(s, std::regex("[(-|+)|][0-9]+"));
}


template <typename T1, typename T2>
inline T1 findClosestKey(const std::map<T1, T2>& data, T1 key)
{
	if (data.size() == 0) {
		return T1();
	//	throw std::out_of_range("Received empty map.");
	}

	auto lower = data.lower_bound(key);

	if (lower == data.end()) // If none found, return the last one.
		return std::prev(lower)->first;

	if (lower == data.begin())
		return lower->first;

	// Check which one is closest.
	auto previous = std::prev(lower);
	if ((key - previous->first) < (lower->first - key))
		return previous->first;

	return lower->first;
}


template <typename T1, typename T2>
inline T2 findClosestValue(const std::map<T1, T2>& data, T1 key)
{
	if (data.size() == 0) {
		return T2();
		//	throw std::out_of_range("Received empty map.");
	}

	auto lower = data.lower_bound(key);

	if (lower == data.end()) // If none found, return the last one.
		return std::prev(lower)->second;

	if (lower == data.begin())
		return lower->second;

	// Check which one is closest.
	auto previous = std::prev(lower);
	if ((key - previous->first) < (lower->first - key))
		return previous->second;

	return lower->second;
}