#pragma once
//#include <windows.h>
#include "CViEngineExAPI.h"
#include <mutex>
#include <memory>
#include <filesystem>

class CViEngineExID;

class CViEngineExConsole :	public CViEngineExAPI
{
public:
	std::recursive_mutex				m_lockConsole;
	std::shared_ptr< CViEngineExID >	m_id;

	float								m_tVideo = -1;
	float								m_tStart = 20;

	int									m_nStart = 0;
	int									m_nStop = 0;

	std::filesystem::path				m_pathBase;
public:
	CViEngineExConsole();
	~CViEngineExConsole();
public:
	bool Start();

	void Configure();
	void OnNewVar(int id, int subID);
	void ShowLicense();

	bool IsReady();


	bool Check();

	void OutputTo(const wchar_t* path) {
		m_pathBase = path;
	}

	void StartFrom(float t) {
		m_tStart = t;
	}

	std::wstring flt2str(float v);

	virtual bool canWrite(const wchar_t* tag);

	virtual bool	GetBasePath(wchar_t* path, int cch_max);
	virtual bool	WriteFile(const wchar_t* tag, const void* lpBuf, int  nCount, bool bLock = true);
	virtual bool	GetItemBasePath(int n, wchar_t* base, int cch_max);
	virtual bool	GetSessionFilePath(const wchar_t* tag, wchar_t* path, int cch);

	bool	NewItemName(const wchar_t* def_name, wchar_t* new_name, int cch);

};

