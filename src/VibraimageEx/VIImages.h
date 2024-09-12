#pragma once
class CVIImages
{
public:
	typedef struct tagITEM
	{
		int n,w,h;
		CString path,name;
		HICON hIcon;
	} ITEM;
	CCriticalSection			m_lock;
	CMFCToolBarImages *			m_pImg;
	std::map< CString, ITEM >	m_imgMap;
public:
	CVIImages(CMFCToolBarImages * pImg);
	virtual ~CVIImages();

	
	void Clear(bool bLock = true);
	int Load(bool bClear,int w,int h);

	int Get(const CString& sid);
	int Get(const char *sid) { return Get(CString(sid)); }
};

