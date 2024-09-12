#pragma once

#include "default_path.h"
#include <afxmt.h>

class CViExFont
{
public:
	static CCriticalSection m_lock;
	static LOGFONT	m_lf;
	static CString	m_fnt_path;
	static LONG		m_nRef;
public:
	int				m_h;
	CFont			m_font;
public:
	CViExFont();
	~CViExFont();
	
	void CreateFont(int h, int ft= FW_NORMAL);
	void DeleteObject();

protected:
	HRESULT CreateFontFaceFromFontFile(LPCWSTR filename, IDWriteFontFace **ppFontFace, IDWriteGdiInterop **ppgdiInterop);
	HRESULT GetLogFontFromFileName(LPCWSTR fontFileName, _Out_ LOGFONT* logFont);


};

