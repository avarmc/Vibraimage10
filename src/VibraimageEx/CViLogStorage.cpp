#include "stdafx.h"
#include "VibraimageEx.h"
#include "ViEngineExSDK.h"
#include "CViLogStorage.h"
#include <filesystem>
#include <ctime>
#include "pugixml.hpp"

#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"

namespace fs = std::filesystem;

CViLogStorage::CViLogStorage(int coreID):m_id(coreID) {
#ifdef SEQ_VIBRAIMAGE_PRO_KZ
	m_bFace = 1;
#endif

}
CViLogStorage::~CViLogStorage() {
	OnFaceOut();
	Log(L"STOP");
	m_bStop = true;
	if (m_pThread && m_pThread->joinable())
		m_pThread->join();

	Write();
	
}

void CViLogStorage::Start() {
	auto pe = theApp.m_pEngines[m_id];
	CString regBase = pe->engineReg.GetRegBase();
	CString ver1, ver2;
	theApp.GetProductAndVersion(ver1, ver2);
	m_appInfo = CStringA(ver1) + "/" + CStringA(ver2);

	TCHAR szPath[MAX_PATH * 4];
	// Get path for each computer, non-user specific and non-roaming data.
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
	{

		// Append product-specific path
		PathAppend(szPath, regBase);
		m_path = szPath;

		if (!fs::exists(szPath))
			fs::create_directories(szPath);
	}

	if (!m_path.IsEmpty() && !m_pThread)
		m_pThread = std::make_shared< std::thread >(ThreadProc, this);
}

void CViLogStorage::Init() {
	m_bInit = true;
	Log(L"START");
}

bool CViLogStorage::OnFaceOut() {
	if (!m_bFace || !bLog())
		return false;

	bool ok = false;
	if (m_suspect >= 0) {
		auto pe = theApp.m_pEngines[m_id];
		float level = pe->engine.GetVar(VI_VAR_STATE_CRITICAL_LEV, 0, VT_R4).fltVal*100;
		CString msg;
		msg.Format(L"FACE\t%f\t%f", m_suspect, level);
		Log(msg);
		ok = true;
	}
	m_suspect = -1;
	return ok;
}

void CViLogStorage::OnFaceDetected(VI_FACE_INFO_DATA* pInfo)
{
	if (m_bStop || !m_bFace || !bLog())
		return;

	CSingleLock lock(&m_lock, TRUE);
	
	if (pInfo->r.left != pInfo->r.right ) {
		auto pe = theApp.m_pEngines[m_id];
		m_tFace = std::chrono::system_clock::now();
		m_suspect = max(m_suspect, pInfo->SV);	
	}
	else {
		OnFaceOut();
	}
}

void CViLogStorage::Check() {
	if (m_bStop || !m_bFace || !bLog())
		return;

	CSingleLock lock(&m_lock, TRUE);
	auto t = std::chrono::system_clock::now();
	unsigned long long int tc = std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count();
	unsigned long long int tf = std::chrono::duration_cast<std::chrono::milliseconds>(m_tFace.time_since_epoch()).count();
	
	if (tc - tf > 2000)
		OnFaceOut();

	if (tc - tf > 60000 ) {
		auto pe = theApp.m_pEngines[m_id];
		CString msg;
		float fpsIN = pe->engine.GetVar(VI_VAR_FPSIN, 0, VT_R4).fltVal;
		float fpsOUT = pe->engine.GetVar(VI_VAR_FPSOUTR, 0, VT_R4).fltVal;
		msg.Format(L"FACE\t%f\t%f", fpsIN, fpsOUT);
		Log(L"CHECK");
		m_tFace = t;
		
	}
}

bool CViLogStorage::bLog() {
	auto pe = theApp.m_pEngines[m_id];
	return !!pe->engine.GetVar(VI_INFO_CAPTURE_LOG, 0, VT_INT).intVal;
}

void  CViLogStorage::Log(const CString& msg) {
	CSingleLock lock(&m_lock, TRUE);
	if (m_bStop || !bLog())
		return;

	LOG l;
	l.t = std::chrono::system_clock::now();
	l.msg = msg;
	m_log.push_back(l);
	m_ver++;

	if (m_bInit && !m_pThread)
		Start();
}

void  CViLogStorage::Write() {
	std::list< LOG > log;
	{
		CSingleLock lock(&m_lock, TRUE);
		if (m_log.empty())
			return;

		log.splice(log.begin(),m_log);
	}

	CString lf,cf;
	FILE* f = 0;

	for (auto& l : log) {
		std::time_t now = std::chrono::system_clock::to_time_t(l.t);
		unsigned long long int tf = std::chrono::duration_cast<std::chrono::milliseconds>(l.t.time_since_epoch()).count();
		std::tm* ptm = std::localtime(&now);
		char dtm[32];
		char dts[48];
		// Format: Mo, 15.06.2009 20:20:00
		std::strftime(dtm, 32, "%Y-%m-%d-%H", ptm);
		std::strftime(dts, 32, "%Y-%m-%d %H:%M:%S", ptm);
			   
		cf.Format(L"%s\\%s\\%s.log", (LPCTSTR)m_path, (LPCTSTR)CString(dtm).Left(10), (LPCTSTR)CString(dtm));

		if (!fs::exists(fs::path((LPCTSTR)cf).parent_path()))
			fs::create_directories(fs::path((LPCTSTR)cf).parent_path());

		if (f && cf != lf) {
			fclose(f);
			f = 0;
		}

		if (!f)
			_wfopen_s(&f, cf, L"a");
		if (!f)
			break;
		
		std::string str = pugi::as_utf8(l.msg);

		fprintf_s(f, "%s\t%llu\t%s\t%s\n", dts,tf, m_appInfo.c_str(), str.c_str());
	}

	if (f) fclose(f);

}

void CViLogStorage::CheckAll() {
	for (auto& l : theApp.m_pEngines)
		if (l && l->pLog)
			l->pLog->Check();
}


void CViLogStorage::ThreadLocal() {
	size_t ver = -1;
	
	while (!theApp.m_bDone && !m_bStop) {
		if(m_ver != ver)
		{
			Write();
			ver = m_ver;
		}

		Sleep(m_bFace ? 100 : 250);
	}
}
