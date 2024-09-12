#include "stdafx.h"
#include "VibraimageEx.h"
#include "ViFTPStorage.h"
#include "ViEngineExSDK.h"

using namespace CViEngineExNS;


CViFTPStorage::CViFTPStorage(CVIENGINE_EX_CLASS *pEngine) 
{
	m_pEngine = pEngine;
	m_hThread = 0;
	m_idVI_FTP_STORAGE_PATH = 35001;
	m_idVI_FTP_STORAGE_ENABLE = 35002;
	int id;
	if ((id = m_pEngine->Tag2Id(L"VI_FTP_STORAGE_PATH")) > 0)
		m_idVI_FTP_STORAGE_PATH = id;

	if ((id = m_pEngine->Tag2Id(L"VI_FTP_STORAGE_ENABLE")) > 0)
		m_idVI_FTP_STORAGE_ENABLE = id;

	m_pEngine->AddId(m_idVI_FTP_STORAGE_PATH, L"VI_FTP_STORAGE_PATH");
	m_pEngine->AddId(m_idVI_FTP_STORAGE_ENABLE, L"VI_FTP_STORAGE_ENABLE");
}


CViFTPStorage::~CViFTPStorage()
{
}


void CViFTPStorage::OnFaceDetected(VI_FACE_INFO_DATA *pInfo)
{
	// Example capturing all faces
/*
	if (!m_pEngine->engine.GetVar(VI_FTP_STORAGE_ENABLE, 0, VT_BOOL).boolVal)
		return;
	
	CSingleLock lock(&m_lock, true);

	if (!LoadConfig())
		return;

	if (pInfo->pImg24 && m_data.size() < 100)
	{
		m_data.push_back(SEND_DATA());
		SEND_DATA& sd = m_data.back();

		int w = pInfo->w;
		int h = pInfo->h;
		int bpp = 24;
		void *pImgSrc = pInfo->pImg24;
		int size = w*h*bpp / 8;
		void *pImg = new char[size];
		if (pImg) memcpy(pImg, pImgSrc, size);

		CViEngineExTools::rectangle(pImg, w, h, bpp, pInfo->r, 0xFF);

		sd.pImg = CViEngineExTools::pack_image(pImg, w,h, bpp, "PNG", &sd.size);
		
		SAFE_DELETE(pImg);

		sd.filename = COleDateTime::GetCurrentTime().Format(_T("%Y%m%d%H%M%S.png"));
	}
	lock.Unlock();
	Start();
*/
}

bool CViFTPStorage::LoadConfig()
{
	if (!m_pEngine)
		return false;
	auto cfg = m_pEngine->GetConfigBase();
	if (!cfg)
		return 0;

	bool ok = true;
	if (m_xml.empty() || !m_xml.first_child())
	{
		COleVariant base_path = m_pEngine->GetVar(m_idVI_FTP_STORAGE_PATH, 0, VT_BSTR);
		CString path;
		if (base_path.vt != VT_BSTR || !base_path.bstrVal || !wcslen(base_path.bstrVal))
		{
			path = GetDefaultPath() + cfg + _T("\\ftp.xml");
		}
		else
			path = base_path.bstrVal;

		if (m_xml.first_child())
			m_xml.remove_child(m_xml.first_child());

		if (path.IsEmpty())
			return false;
		ok = m_xml.load_file(path);
	}
	
	return ok;
}

bool CViFTPStorage::Start()
{
	if (!m_pEngine)
		return false;

	CSingleLock lock(&m_lock, true);

	if (m_hThread)
		return false;

	m_hThread = CreateThread(0, 0, ThreadProc, this, 0, 0);
	return true;
}

DWORD CViFTPStorage::ThreadProc(LPVOID p)
{
	CViFTPStorage *pThis = static_cast<CViFTPStorage *>(p);
	pThis->ThreadLocal();
	return 0;
}

void CViFTPStorage::ThreadLocal()
{
	CSingleLock lock(&m_lock, false);

	CInternetSession sess(_T("VI FTP Session"));

	CFtpConnection* pConnect = NULL;

	CString host, user, pwd, path, port;

	xml_node xml = m_xml.first_child();
	if (xml)
	{
		host = xml["host"].wchild_value().c_str();
		user = xml["user"].wchild_value().c_str();
		pwd = xml["pwd"].wchild_value().c_str();
		path = xml["path"].wchild_value().c_str();
		port = xml["port"].wchild_value().c_str();
	}

	if (port.IsEmpty())
		port = _T("21");

	try {
		pConnect = sess.GetFtpConnection(host, user, pwd,_tstoi(port));
		if (!pConnect->SetCurrentDirectory(path))
			SAFE_DELETE(pConnect);

	}
	catch (CInternetException*  pEx  )
	{
#ifdef _DEBUG
		TCHAR msg[1024];
		pEx->GetErrorMessage(msg,1024);
#endif
		SAFE_DELETE(pConnect);
	}

	while (m_pEngine && pConnect && m_pEngine->GetVar(m_idVI_FTP_STORAGE_ENABLE, 0, VT_BOOL).boolVal && !theApp.m_bDone && !theApp.m_bCloseRqst)
	{
		std::list<SEND_DATA> data;

		lock.Lock();
		if (m_data.empty())
			break;
		data.splice(data.end(), m_data, m_data.begin());
		lock.Unlock();

		if (!DoSend(pConnect, data.front()))
		{
			// do not care; do nothing
		}

	}

	if (pConnect)
	{
		pConnect->Close();
		delete pConnect;
	}

	CloseHandle(m_hThread);
	m_hThread = 0;

	lock.Unlock();
}


bool CViFTPStorage::DoSend(CFtpConnection* pConnect, SEND_DATA& sd)
{
	BOOL ok = FALSE;
	if (sd.data.empty())
		return TRUE;

	try {
		CInternetFile* pFile = pConnect->OpenFile(sd.filename, GENERIC_WRITE);
		if (!pFile)
			return false;
		pFile->Write(&sd.data[0], (UINT)sd.data.size());
		pFile->Close();
		delete pFile;
		ok = true;
	}
	catch (CInternetException*  pEx )
	{
#ifdef _DEBUG
		pEx->ReportError();
#endif

	}
	return !!ok;
}

void CViFTPStorage::OnNewVar(int id, int subID)
{
	if (!m_pEngine)
		return;
	if(id == m_idVI_FTP_STORAGE_PATH )
	{
		CSingleLock lock(&m_lock, true);
		if (m_xml.first_child())
			m_xml.remove_child(m_xml.first_child());

		LoadConfig();
		return;
	}
}

void CViFTPStorage::OnSessionStopped(const std::list<CString>& files) {
	if (!m_pEngine || !m_pEngine->GetVar(m_idVI_FTP_STORAGE_ENABLE, 0, VT_BOOL).boolVal)
		return;
	CSingleLock lock(&m_lock, true);

	if (!LoadConfig())
		return;

	for (auto& f : files) {
		SEND_DATA sd;
		sd.filename = f;
		try {
			CFile file;
			if (file.Open(f, CFile::modeRead)) {
				size_t len = file.GetLength();
				sd.data.resize(len);
				file.Read(&sd.data[0], sd.data.size());
				file.Close();
			}
		}
		catch (CFileException*) {
			sd.data.clear();
		}

		if (!sd.data.empty()) {
			{
				CSingleLock lock(&m_lock, true);
				int pos = sd.filename.ReverseFind('\\');
				if (pos >= 0)
					sd.filename = sd.filename.Mid(pos + 1);

				m_data.push_back(sd);
			}
			Start();
		}
	}
}

bool  CViFTPStorage::onWriteFile(LPCWSTR tag, const void *lpBuf, UINT  nCount)
{
	if (!m_pEngine || !m_pEngine->GetVar(m_idVI_FTP_STORAGE_ENABLE, 0, VT_BOOL).boolVal)
		return false;
	CSingleLock lock(&m_lock, true);

	if (!LoadConfig())
		return false;

	if (!lpBuf || !nCount)
		return true;


	m_data.push_back(SEND_DATA());
	SEND_DATA& sd = m_data.back();

	sd.data.resize(nCount);

	if (wcscmp(tag, L"IMG_FACE_RECT") == 0)
		sd.filename = COleDateTime::GetCurrentTime().Format(_T("%Y%m%d%H%M%S.png"));
#if  !defined SEQ_MEDIC_MOL
	else
	if (wcscmp(tag, L"M_XML") == 0)
	{
		WCHAR path[1024] = { 0 };
		m_pEngine->GetDB()->GetSessionFilePath(tag, path, 1024);
		CString sPath = path;
		int pos = sPath.ReverseFind('\\');
		if (pos >= 0)
			sd.filename = sPath.Mid(pos + 1);
		else
			sd.filename = sPath;
	}
#endif
	memcpy(&sd.data[0], lpBuf, nCount);
	lock.Unlock();

	Start();

	return true;
}

void  CViFTPStorage::RegLoad()
{
	if (!m_pEngine)
		return;

	CString gr;
	gr.Format(_T("%d\\FTP"), m_pEngine->GetVar(VI_INFO_CORE_ID, 0, VT_INT, 1).intVal);

	int bEnable = FALSE;
	m_pEngine->GetReg()->RegLoad(gr, L"VI_FTP_STORAGE_ENABLE",bEnable);
	m_pEngine->PutVar(m_idVI_FTP_STORAGE_ENABLE, 0, COleVariant((long) bEnable, VT_BOOL) );

	WCHAR path[1024] = L".\\ftp\\ftp.xml";
	m_pEngine->GetReg()->RegLoad(gr, L"VI_FTP_STORAGE_PATH",path,1024 );
	m_pEngine->PutVar(m_idVI_FTP_STORAGE_PATH,0, COleVariant(path) );
}

void  CViFTPStorage::RegSave()
{
	if (!m_pEngine)
		return;

	CString gr;
	gr.Format(_T("%d\\FTP"), m_pEngine->GetVar(VI_INFO_CORE_ID, 0, VT_INT, 1).intVal);

	m_pEngine->GetReg()->RegSave(gr, L"VI_FTP_STORAGE_ENABLE", m_pEngine->GetVar(m_idVI_FTP_STORAGE_ENABLE, 0, VT_BOOL).boolVal);
	m_pEngine->GetReg()->RegSave(gr, L"VI_FTP_STORAGE_PATH", m_pEngine->GetVar(m_idVI_FTP_STORAGE_PATH, 0, VT_BSTR).bstrVal);
}
