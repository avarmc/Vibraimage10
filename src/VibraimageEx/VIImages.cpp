#include "stdafx.h"
#include "VIImages.h"
#include "VibraimageEx.h"

CVIImages::CVIImages(CMFCToolBarImages * pImg):m_pImg(pImg)
{
}


CVIImages::~CVIImages()
{
	Clear();
}

void CVIImages::Clear( bool bLock )
{
	CSingleLock lock(&m_lock, !!bLock);

	while (!m_imgMap.empty())
	{
		ITEM& item = m_imgMap.begin()->second;
		if (item.hIcon && item.hIcon != INVALID_HANDLE_VALUE)
			DestroyIcon(item.hIcon);
		m_imgMap.erase(m_imgMap.begin());
	}
}

int CVIImages::Load( bool bClear, int w, int h)
{
	auto cfg = theApp.m_pEngine->engine.GetConfigBase();
	if (!cfg)
		return 0;

	CSingleLock lock(&m_lock, true);

	if (bClear)
		Clear(false);

	int cnt = 0;

	CString path = GetDefaultPath() + cfg+_T("\\images\\*.ico");
	CFileFind ff;
	BOOL ok = ff.FindFile(path);

	m_pImg->SetImageSize(CSize(w, h));
	while (ok)
	{
		ok = ff.FindNextFile();
		if (ff.IsDots() || ff.IsDirectory())
			continue;
		CString fp = ff.GetFilePath();
		CString fn = ff.GetFileName(), fe;
		fn.MakeLower();

		int dotPos = fn.ReverseFind('.');
		if (dotPos >= 0)
			fe = fn.Mid(dotPos + 1), fn = fn.Left(dotPos);

		
		if (fe.Right(4) == _T("ico"))
		{
			ITEM item;
			item.hIcon = 0;
			item.n = -1;
			item.path = fp;
			item.name = fn;
			item.w = w;
			item.h = h;
			m_imgMap[fn] = item;
		}

	}

	return (int)m_imgMap.size();
}

int CVIImages::Get(const CString& sid)
{
	CSingleLock lock(&m_lock, true);

	std::map< CString, ITEM >::iterator i = m_imgMap.find(sid);
	if (i == m_imgMap.end())
		return -1;

	ITEM& item = i->second;

	if (!item.hIcon || item.hIcon == INVALID_HANDLE_VALUE)
	{
		item.hIcon = (HICON)::LoadImage(NULL, item.path, IMAGE_ICON, item.w, item.h, LR_LOADFROMFILE | LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS);
		if (item.hIcon && item.hIcon != INVALID_HANDLE_VALUE)
		{
			item.n = m_pImg->AddIcon(item.hIcon, TRUE);
		}
	}

	return item.n;
}


