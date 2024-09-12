#include "stdafx.h"
#include "VibraimageEx.h"
#include "ViEngineExProcDb.h"
#include "UserIdDlg.h"
#include "CViHTTPStorage.h"
#include "CDlgStarter.h"

CViEngineExProcDb::SESSIONREF::~SESSIONREF() { SAFE_DELETE(pHttp); }

CViEngineExProcDb::CViEngineExProcDb() : m_pBase(0)
{
}


CViEngineExProcDb::~CViEngineExProcDb()
{
	PurgeSessions(0);
}

void CViEngineExProcDb::Connect(int core, CVIENGINE_EX_CLASS *pApp) {
	m_id = core; 
	CViEngineExDatabaseInterface::Connect(pApp); 	
}

bool CViEngineExProcDb::WriteFile(LPCWSTR tag, const void *lpBuf, UINT  nCount, bool bLock)
{
	/*
	if (wcscmp(tag, L"IMG_FACE_RECT") == 0)
	{
		return theApp.m_pEngines[m_id]->pFtp->onWriteFile(tag,lpBuf,nCount);
	} 
	if (wcscmp(tag, L"M_XML") == 0)
	{
		theApp.m_pEngines[m_id]->pFtp->onWriteFile(tag, lpBuf, nCount);
	}
	*/
	return __super::WriteFile(tag, lpBuf, nCount, bLock);
}


bool CViEngineExProcDb::CheckItemID(LPCWSTR id, bool bValidate)
{
	if (bValidate)
	{
		BOOL bResult = FALSE;
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_COMMAND_CHECKID, (LPARAM)&bResult);
		return !!bResult;
	}
	return true;
}


bool	CViEngineExProcDb::StartSession(LPCWSTR prefix, LPCWSTR postfix) {
	if (theApp.m_bDone || theApp.m_bAbort || theApp.m_bCloseRqst)
		return false;

	if (theApp.m_hWndStarter)
		::SendMessage(theApp.m_hWndStarter, WM_MSG_STARTER, ID_MSG_STARTER_DELAY, 1);

	bool ret = __super::StartSession(prefix, postfix);
	if (!ret)
		return false;
	TCHAR name[MAX_PATH];
	if (!GetSessionName(name, MAX_PATH))
		return false;

	CViHTTPStorage* pHttp = CViHTTPStorage::Create(this);
	{
		CSingleLock refLock(&m_sessionMapLock, TRUE);
		SESSIONREF& ref = m_sessionMap[name];
	}
	{
		CSingleLock refLock(&m_sessionMapLock, TRUE);
		m_sessionMap[name].pFtp = theApp.m_pEngines[m_id]->pFtp;

		if (pHttp) {
			{
				m_sessionMap[name].pHttp = pHttp;
			}
			pHttp->onSessionStarted(name);
		}
	}

	PurgeSessions(60000);

	return ret;
}

std::list<CString> CViEngineExProcDb::GetFiles(LPCWSTR name) {
	CSingleLock refLock(&m_sessionMapLock, TRUE);

	if (m_sessionMap.find(name) == m_sessionMap.end())
		return std::list<CString>();
	return m_sessionMap[name].files;
}

void CViEngineExProcDb::PurgeSessions(ULONGLONG dt) {
	CSingleLock refLock(&m_sessionMapLock, TRUE);
	ULONGLONG t = GetTickCount64();
	for (std::map<CString, SESSIONREF>::iterator i = m_sessionMap.begin(),ic; i != m_sessionMap.end();) {
		ic = i++;

		SESSIONREF& ref = ic->second;
		ULONGLONG dtRef = t - ref.t;
		if (dtRef >= dt) {
			m_sessionMap.erase(ic);
		}
	}
}

bool	CViEngineExProcDb::OnSessionStopped(LPCWSTR name) {

	__super::OnSessionStopped(name);
	if (theApp.m_bDone || theApp.m_bAbort || theApp.m_bCloseRqst)
		return false;

	CSingleLock refLock(&m_sessionMapLock, TRUE);
	SESSIONREF& ref = m_sessionMap[name];

	if (!ValidateSessionFace()) {
		SetStatus(name, "FAIL");
	}
	else
	{
		if (ref.pFtp)
			ref.pFtp->OnSessionStopped(ref.files);

		if (ref.pHttp) {
			ref.pHttp->onSessionEnd();
		}
		else
			SetStatus(name, "OK");
	}
	SAFE_DELETE(ref.pHttp);
	m_sessionMap.erase(name);

	return true;
}

bool	CViEngineExProcDb::StopSession(bool bLock ) {

	TCHAR name[MAX_PATH];
	GetSessionName(name, MAX_PATH);

	bool ret = __super::StopSession(bLock);

	CSingleLock refLock(&m_sessionMapLock, TRUE);
	SESSIONREF& ref = m_sessionMap[name];

	int cnt = GetLastSessionFiles(-1, 0, 0);
	for (int i = 0; i < cnt; ++i)
	{
		WCHAR fpath[MAX_PATH] = { 0 };
		GetLastSessionFiles(i, fpath, MAX_PATH);
		ref.files.push_back(fpath);
	}

	return ret;
}


void CViEngineExProcDb::onHttpResult(CString sessionName, CStringA msg) {
	if (theApp.m_bDone || theApp.m_bAbort || theApp.m_bCloseRqst)
		return;

#ifndef SEQ_DEFENDERX
	//simple decode JSON
	msg.Remove('{');
	msg.Remove('}');
	msg.Remove('\r');
	msg.Remove('\n');
	msg.Remove('\t');
	msg.Remove('\"');

	std::vector<CStringA> arr = explode(msg, CStringA(","));

	CStringA status = "";

	for (std::vector<CStringA>::iterator a = arr.begin(), ae = arr.end(); a != ae; ++a) {
		std::vector<CStringA> prm = explode(*a, CStringA(":"));
		if (prm.size() != 2)
			continue;
		prm[0].Trim(" ");
		prm[1].Trim(" ");
		if (prm[0] == "RESULT_CODE") {
			status = prm[1];
			break;
		}
	}
#else
	CStringA status = "OK";
#endif
	SetStatus(sessionName, status);
}