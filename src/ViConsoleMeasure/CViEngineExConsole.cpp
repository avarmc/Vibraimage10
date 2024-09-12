#include <iostream>
#include "CViEngineExConsole.h"


class CViEngineExID {
public:
	int idVI_VAR_SIZE;
	int idVI_VAR_N0;

	int idVI_MODE_RESULT;
	int idVI_MODE_AURA;

	int idVI_VAR_FPSIN;
	int idVI_VAR_FPSOUTF;
	int idVI_VAR_FPSOUTR;

	int idVI_VAR_FPSMAXF;
	int idVI_VAR_FPSMAXR;

	int idVI_VAR_NFRAME;
	int idVI_INFO_TIME_VIDEO;
	int idVI_INFO_M_PERIOD;
	int idVI_INFO_M_PROGRESS;


	int idVI_INFO_CHQ_TEST_VALUE; // Quality test value
	int idVI_INFO_CHQ_TEST_TYPE; // Quality test type

	int idVI_VAR_STAT_RES_P37; // Current avg AI value
	int idVI_VAR_STAT_RES_P38; // Final AI value
	int idVI_VAR_STAT_RES_P39; // AI timer value
	int idVI_VAR_STAT_RES_P41; // AI one time value

	int idID_ACTION_MEASUREMENT;
public:
	CViEngineExID(CViEngineExConsole *p) {
		
		idID_ACTION_MEASUREMENT = p->Tag2Id(L"ID_ACTION_MEASUREMENT");
		idVI_INFO_M_PERIOD = p->Tag2Id(L"VI_INFO_M_PERIOD");
		idVI_INFO_M_PROGRESS = p->Tag2Id(L"VI_INFO_M_PROGRESS");


		idVI_VAR_SIZE = p->Tag2Id(L"VI_VAR_SIZE");
		idVI_VAR_N0 = p->Tag2Id(L"VI_VAR_N0");
		idVI_MODE_RESULT = p->Tag2Id(L"VI_MODE_RESULT");
		idVI_MODE_AURA = p->Tag2Id(L"VI_MODE_AURA");

		idVI_VAR_FPSIN = p->Tag2Id(L"VI_VAR_FPSIN");
		idVI_VAR_FPSOUTF = p->Tag2Id(L"VI_VAR_FPSOUTF");
		idVI_VAR_FPSOUTR = p->Tag2Id(L"VI_VAR_FPSOUTR");

		idVI_VAR_FPSMAXF = p->Tag2Id(L"VI_VAR_FPSMAXF");
		idVI_VAR_FPSMAXR = p->Tag2Id(L"VI_VAR_FPSMAXR");

		idVI_VAR_NFRAME = p->Tag2Id(L"VI_VAR_NFRAME");

		idVI_INFO_TIME_VIDEO = p->Tag2Id(L"VI_INFO_TIME_VIDEO");

		idVI_INFO_CHQ_TEST_VALUE = p->Tag2Id(L"VI_INFO_CHQ_TEST_VALUE");
		idVI_INFO_CHQ_TEST_TYPE = p->Tag2Id(L"VI_INFO_CHQ_TEST_TYPE");

		idVI_VAR_STAT_RES_P37 = p->Tag2Id(L"VI_VAR_STAT_RES_P37");
		idVI_VAR_STAT_RES_P38 = p->Tag2Id(L"VI_VAR_STAT_RES_P38");
		idVI_VAR_STAT_RES_P39 = p->Tag2Id(L"VI_VAR_STAT_RES_P39");
		idVI_VAR_STAT_RES_P41 = p->Tag2Id(L"VI_VAR_STAT_RES_P41");
	}
};
CViEngineExConsole::CViEngineExConsole() {
}

CViEngineExConsole::~CViEngineExConsole() {

}


bool CViEngineExConsole::Start() {
	bool ok = __super::Start();
	m_nStart = 0;
	m_nStop = 0;
	m_id = std::make_shared< CViEngineExID >(this);

	Configure();

	ShowLicense();

	return ok;
}

void CViEngineExConsole::ShowLicense() {
	std::unique_lock<std::recursive_mutex> lock(m_lockConsole);
	std::wcout << L"LICENSE:";
	std::wcout << L"\tOWNER=" << GetLicense(L"OWNER");
	std::wcout << L"\tSERIAL=" << GetLicense(L"SERIAL");
	std::wcout << L"\tLIMIT=" << GetLicense(L"LIMIT");
	std::wcout << std::endl;
}

void CViEngineExConsole::Configure() {
	PutVar((int)1, L"VI_FILTER_FPSDIV_RQST");
	PutVar(5.0f, L"VI_VAR_FPSMAXR");
	PutVar(30.0f, L"VI_VAR_FPSMAXF");
	PutVar((int)0, L"VI_MODE_RESULT");
	PutVar((int)0, L"VI_MODE_AURA");
	PutVar((int)1, L"VI_INFO_AI_LIVE");
	PutVar((int)1, L"VI_INFO_QTEST");
	PutVar((int)1, L"VI_FACE_ENABLE");
	PutVar((int)0, L"VI_INFO_AI_LIVE");
}

void CViEngineExConsole::OnNewVar(int id, int subID) {

}



bool  CViEngineExConsole::Check() {
	float tVideo = GetVarF1(m_id->idVI_INFO_TIME_VIDEO);
	float progress = GetVarF1(m_id->idVI_INFO_M_PROGRESS);
	int bMeasureStarted = 0, bMeasureEnabled = 0;
	menuStatus(m_id->idID_ACTION_MEASUREMENT, bMeasureEnabled, bMeasureStarted);

	if ( bMeasureEnabled && !bMeasureStarted && tVideo > m_tStart)
	{
		if (!m_nStart) {
			{		
				std::unique_lock<std::recursive_mutex> lock(m_lockConsole);
				std::wcout << L"MEASURE: Starting " << tVideo << std::endl;;
			}
			m_nStart++;
			menuAction(m_id->idID_ACTION_MEASUREMENT);
			return true;
		}
		if (!IsSessionStarted()) {
			std::unique_lock<std::recursive_mutex> lock(m_lockConsole);
			std::wcout << L"MEASURE: End " << tVideo << std::endl;;
			return false;
		}
	} 
	return true;
}

bool  CViEngineExConsole::IsReady() {
	float tVideo = GetVarF1(m_id->idVI_INFO_TIME_VIDEO);
	if (tVideo < m_tVideo )
		return false;
	if( _wtoi(GetLicense(L"STATUS")) != 1 )
		return false;

	m_tVideo = tVideo;
	return true;
}

std::wstring CViEngineExConsole::flt2str(float v) {
	wchar_t str[64];
	swprintf_s(str, L"%0.4f", v);
	return str;
}

bool CViEngineExConsole::canWrite(const wchar_t* tag) {
	if (!tag)
		return true;
	if (!wcscmp(tag, L"M_XML"))
		return true;
	return false;
}

bool	CViEngineExConsole::GetBasePath(wchar_t* path, int cch_max) {
	if (!canWrite(0))
		return false;
	if (!path)
		return false;
	wcscpy_s(path, cch_max, m_pathBase.parent_path().c_str());
	return true;
}

bool	CViEngineExConsole::WriteFile(const wchar_t* tag, const void* lpBuf, int  nCount, bool bLock) {
	if (!wcscmp(tag, L"M_XML")) {
		FILE* f = 0;
		if (!_wfopen_s(&f, m_pathBase.c_str(), L"w")) {
			fwrite(lpBuf, 1, nCount, f);
			fclose(f);
		}
	}
	return true;
}
bool	CViEngineExConsole::GetItemBasePath(int n, wchar_t* base, int cch_max) {
	return false;
}
bool	CViEngineExConsole::GetSessionFilePath(const wchar_t* tag, wchar_t* path, int cch) {
	return false;
}

bool	CViEngineExConsole::NewItemName(const wchar_t* def_name, wchar_t* new_name, int cch) {
	if (cch)
		new_name[0] = 0;
	return true;
}
