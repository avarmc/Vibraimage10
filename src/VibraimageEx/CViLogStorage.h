#pragma once
#include <map>
#include <list>
#include <chrono>

class CViLogStorage
{
public:
	CCriticalSection	m_lock;
	int					m_id;
	CString				m_path;


	bool									m_bStop = false;
	int										m_bLog = -1;
	bool									m_bFace = false;
	bool									m_bInit = false;

	std::chrono::system_clock::time_point	m_tFace;
	float									m_suspect = -1;

	typedef struct tagLOG {
		std::chrono::system_clock::time_point t;
		CString msg;
	}LOG;

	std::list< LOG > m_log;
	std::shared_ptr< std::thread >	m_pThread;
	size_t	m_ver = 0;

	std::string	m_appInfo;
public:
	CViLogStorage(int coreID);
	~CViLogStorage();

	void OnFaceDetected(VI_FACE_INFO_DATA* pInfo);
	bool OnFaceOut();
	void Check();
	void Init();
	void Start();
	void Log(const CString& msg);
	void Write();
	bool bLog();

	static void CheckAll();
	static void ThreadProc(CViLogStorage* pThis) { pThis->ThreadLocal(); }
	void ThreadLocal();
};

