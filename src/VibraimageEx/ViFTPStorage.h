#pragma once

#include "VIEngine.h"
#include "pugixml.hpp"
#include "NewVarEvent.h"

using namespace pugi;



class CViFTPStorage : public CNewVarEventVI
{
protected:
	CCriticalSection	m_lock;
	xml_document		m_xml;
	HANDLE				m_hThread;
protected:
	typedef struct tagSEND_DATA
	{
		CString	filename;
		std::vector<BYTE> data;
		tagSEND_DATA() {
		}
		~tagSEND_DATA() {
		}
	} SEND_DATA;

	std::list<SEND_DATA>	m_data;
public:
	int m_idVI_FTP_STORAGE_PATH;
	int m_idVI_FTP_STORAGE_ENABLE;
public:
	CVIENGINE_EX_CLASS *m_pEngine;
public:
	CViFTPStorage(CVIENGINE_EX_CLASS *pEngine);
	~CViFTPStorage();
public:
	void OnFaceDetected(VI_FACE_INFO_DATA *pInfo);
	
	bool Start();

	void OnNewVar(int id, int subID);

	bool onWriteFile(LPCWSTR tag, const void *lpBuf, UINT  nCount);
	void OnSessionStopped(const std::list<CString>& files);
	void RegLoad();
	void RegSave();
protected:
	static DWORD WINAPI ThreadProc(LPVOID p);
	void ThreadLocal();
	bool DoSend(CFtpConnection* pConnect, SEND_DATA& sd);
	bool LoadConfig();
};

