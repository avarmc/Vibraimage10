#include "stdafx.h"
#include "ViExFont.h"
#include <atlbase.h>
#include "strsafe.h"

LOGFONT	CViExFont::m_lf = { 0 };
CString	CViExFont::m_fnt_path;
LONG	CViExFont::m_nRef = 0;
CCriticalSection CViExFont::m_lock;

#define SafeRelease(p) { if((p) && *(p)) { (*(p))->Release(); (*(p))=0; } }

CViExFont::CViExFont():m_h(-1)
{
	CSingleLock lock(&m_lock, true);
	if (InterlockedIncrement(&m_nRef) == 1)
	{
		ZeroMemory(&m_lf, sizeof(m_lf));
		m_fnt_path = GetDefaultPath() + _T("ARIALUNI.TTF");
		AddFontResourceEx(m_fnt_path, FR_PRIVATE, 0);
		if (GetLogFontFromFileName(m_fnt_path, &m_lf) != S_OK)
		{
			m_lf.lfFaceName[0] = 0;
		}
	}
}


CViExFont::~CViExFont()
{
	DeleteObject();
	CSingleLock lock(&m_lock, true);

	if (InterlockedDecrement(&m_nRef) == 0)
	{
		RemoveFontResource(m_fnt_path);
	}
}

void CViExFont::CreateFont(int h,int ft)
{
	CSingleLock lock(&m_lock, true);

	if (m_font.m_hObject)
	{
		if (m_h == h)
			return;
		m_font.DeleteObject();
	}

	m_h = h;

	if (m_lf.lfFaceName[0])
	{
		LOGFONT fs = m_lf;
		fs.lfHeight = h;
		fs.lfWeight = ft;
		if (m_font.CreateFontIndirect(&fs))
			return;
	}

	LOGFONT fs =
	{
		h,
		0,
		0,
		0,
		ft,
		FALSE,FALSE,FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		_T("")
	};


	_tcscpy_s(fs.lfFaceName,32, _T("Arial Unicode MS"));
	if (m_font.CreateFontIndirect(&fs))
		return;

	_tcscpy_s(fs.lfFaceName,32, _T("Arial"));
	m_font.CreateFontIndirect(&fs);
}

HRESULT  CViExFont::GetLogFontFromFileName(LPCWSTR fontFileName, _Out_ LOGFONT* logFont)
{
	IDWriteFontFace* pFontFace = NULL;
	IDWriteGdiInterop *pgdiInterop = NULL;
	HRESULT hr = CreateFontFaceFromFontFile(fontFileName, &pFontFace,&pgdiInterop);
	if (hr != S_OK || !pFontFace || !pgdiInterop)
	{
		SafeRelease(&pFontFace);
		SafeRelease(&pgdiInterop);
		return hr;
	}

	// Get the necessary logical font information.
	pgdiInterop->ConvertFontFaceToLOGFONT(pFontFace, OUT logFont);
	SafeRelease(&pFontFace);
	SafeRelease(&pgdiInterop);

	return hr;
}


HRESULT CViExFont::CreateFontFaceFromFontFile(LPCWSTR filename, IDWriteFontFace **ppFontFace, IDWriteGdiInterop **ppgdiInterop)
{
	HRESULT hr = S_OK;

	IDWriteFactory* pDWriteFactory = NULL;
	IDWriteFontFace* pFontFace = NULL;
	IDWriteFontFile* pFontFiles = NULL;
	IDWriteGdiInterop *pgdiInterop = NULL;

	// Create the DirectWrite factory.
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory)
		);

	pDWriteFactory->GetGdiInterop(&pgdiInterop);
	if (ppgdiInterop)
		*ppgdiInterop = pgdiInterop;

	//Gets a filename from a application directory
	

	if (SUCCEEDED(hr))
	{
		hr = pDWriteFactory->CreateFontFileReference(
			filename,
			NULL,
			&pFontFiles);
	}

	IDWriteFontFile* fontFileArray[] = { pFontFiles };

	if (SUCCEEDED(hr))
	{
		hr = pDWriteFactory->CreateFontFace(
			DWRITE_FONT_FACE_TYPE_TRUETYPE,
			1, // file count
			fontFileArray,
			0,
			DWRITE_FONT_SIMULATIONS_NONE,
			&pFontFace
			);
	}

	//Return the pointer to the caller
	*ppFontFace = pFontFace;

	SafeRelease(&pDWriteFactory);
	SafeRelease(&pFontFiles);

	return hr;
}


void CViExFont::DeleteObject()
{
	CSingleLock lock(&m_lock, true);
	if (m_font.m_hObject)
		m_font.DeleteObject();
}