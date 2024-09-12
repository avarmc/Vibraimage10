#include "stdafx.h"
#include "VibraimageEx.h"
#include "ViEngineExSDK.h"
#include "CViHTTPStorage.h"
#include "ViEngineExProcDb.h"

int	CViHTTPStorage::m_idVI_HTTP_STORAGE_URL = 35004;

CViHTTPStorage::CViHTTPStorage():m_pDB(0) {
	CStringA sys = getSysName();
	sys.IsEmpty();
}

CViHTTPStorage::~CViHTTPStorage() {
	
}

CViHTTPStorage* CViHTTPStorage::Create(CViEngineExProcDb* pDB) {
	int core = pDB->m_id,id;

	if ((id = theApp.m_pEngines[core]->engine.Tag2Id(L"VI_HTTP_STORAGE_URL")) > 0)
		m_idVI_HTTP_STORAGE_URL = id;

	theApp.m_pEngines[core]->engine.AddId(m_idVI_HTTP_STORAGE_URL, _T("VI_HTTP_STORAGE_URL"));

	CViHTTPStorage* p = NULL;
	int regID = theApp.m_pEngines[core]->engine.Tag2Id(L"VI_HTTP_STORAGE_URL");
	CString url = theApp.m_pEngines[core]->engine.GetVar(regID, 0, VT_BSTR).bstrVal;
	if (url.IsEmpty())
		return NULL;

	CStringA tmp = loadTemplate(pDB->m_id);
	if (tmp.IsEmpty())
		return NULL;

	p = new CViHTTPStorage();

	if (p) {
		p->m_pDB = pDB;
		p->onCreate(tmp);
	}

	return p;
}

void CViHTTPStorage::onCreate(CStringA tmp) {

	CSingleLock lock(&m_templateLock, TRUE);
	m_template = tmp;
}

void CViHTTPStorage::onSessionStarted(LPCWSTR sessName) {
	int core = m_pDB->m_id;
	float state = theApp.m_pEngines[core]->engine.GetVar(VI_VAR_STATE_VAR, 0, VT_R4).fltVal;
	float pressure = theApp.m_pEngines[core]->engine.GetVar(VI_VAR_STAT_RES_P6, 0, VT_R4).fltVal; // stress
	float aggressive = theApp.m_pEngines[core]->engine.GetVar(VI_VAR_STAT_RES_P7, 0, VT_R4).fltVal; // anger
	float nervous = theApp.m_pEngines[core]->engine.GetVar(VI_VAR_STAT_RES_F5X, 0, VT_R4).fltVal; // tension

	m_sessionName = sessName;

	SYSTEMTIME stUTC{};
	GetSystemTime(&stUTC);
	COleDateTime t = stUTC;

	CStringA dt;
	dt.Format("%0.4u-%0.2u-%0.2u %0.2u:%0.2u:%0.2u",
		t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	CStringA dtx;
	dtx.Format("%0.4u-%0.2u-%0.2uT%0.2u:%0.2u:%0.2u.000000Z",
		t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	set("{UUID}", getUUID(sessName));
	set("{SEND_DATE}", dt);
	set("{ALARM_DATE}", dt);
	set("{CAMERA_ID}", getCam());
	set("{SYSTEM_ID}", getSysName());
	set("{USER_ID}", getUserName());
	set("{PRG_ID}", CStringA(theApp.m_pEngine->engine.GetLang()->Get(L"STR_FRAME_CAPTION")) );
	set("{FRAME_TIME}", dtx);

	set("{SCORE}", state);
	set("{PRESSURE}", pressure);
	set("{AGGRESSIVE}", aggressive);
	set("{NERVOUS}", nervous);
}

void CViHTTPStorage::onSessionEnd() {
	CStringA files;
	std::list<CString> fileList;

	set("{IMG}", getImage(&files, &fileList) );
	set("{MSG_CODE}", "1001");
	set("{FILES}", files);

	bool bSendImages = !fileList.empty();
	if (isSet("{IMG_DATA}")) {
		set("{IMG_DATA}", fileData(fileList));
		bSendImages = false;
	}

	CSingleLock lock(&m_templateLock, TRUE);
	CStringA data = m_template;
	lock.Unlock();

	sendJSON(data,m_sessionName, fileList, bSendImages);

	m_sessionName.Empty();
}

CStringA	CViHTTPStorage::loadTemplate(int core) {
	CString path = GetDefaultPath() + L"conf\\http.json";
	CStringA r;
	try {
		CFile f;
		if (!f.Open(path, CFile::modeRead))
			return r;
		int len = (int)f.GetLength();
		f.Read(r.GetBufferSetLength(len), len);
		f.Close();
	}
	catch (CFileException*) {

	}

	r.Remove('\r');
	r.Remove('\n');
	r.Replace('\t',' ');

	return r;
}

bool CViHTTPStorage::isSet(LPCSTR tag) {
	CSingleLock lock(&m_templateLock, TRUE);
	return m_template.Find(tag) >= 0;
}

bool CViHTTPStorage::set(LPCSTR tag, LPCSTR v) {
	CSingleLock lock(&m_templateLock, TRUE);
	return m_template.Replace(tag, v) > 0;
}

bool CViHTTPStorage::set(LPCSTR tag, float v) {
	CStringA str;
	str.Format("%f", v);
	return set(tag, str);
}

CStringA	CViHTTPStorage::getUserName() {
	if (!m_pDB)
		return "";
	WCHAR name[256];
	if(!m_pDB->GetItemData(L"name", name, 256))
		return "";
	return pugi::as_utf8(name).c_str();
}

CStringA CViHTTPStorage::getSysName() {
	CStringA tmp, str;

	CStringA strSOFTWARE_Microsoft = "SOFTWARE\\Microsoft\\";
	CStringA strWindows = "Windows";

	CStringA strCurrentVersion = "\\CurrentVersion";

	tmp = GetReg(strSOFTWARE_Microsoft + strWindows + " NT" + strCurrentVersion, "ProductName");

	if (!tmp.IsEmpty())
	{
		if (!str.IsEmpty()) str += "/";
		str += tmp;
	}


	tmp = GetReg(strSOFTWARE_Microsoft + strWindows + " NT" + strCurrentVersion, "ProductId");

	if (!tmp.IsEmpty())
	{
		if (!str.IsEmpty()) str += "/";
		str += tmp;
	}


	return str;
}

CStringA	CViHTTPStorage::getCam() {
	WCHAR docName[1024];
	theApp.m_pEngines[m_pDB->m_id]->engine.GetCurrentDocument(docName, 1024);
	CStringA r = pugi::as_utf8(docName).c_str();
	if (r.Left(7) == "*VIDEO:")
		r = r.Mid(7);
	r.Trim("\r\n \t");
	return r;
}

CStringA	CViHTTPStorage::getUUID(const WCHAR* sessName) {
	CStringA uuid = pugi::as_utf8(sessName).c_str();
	uuid.Remove(' ');
	uuid.Remove('-');
	uuid.Remove('_');

	return uuid;
}

CStringA	CViHTTPStorage::getUUID() {
	return getUUID(m_sessionName);
}

int CViHTTPStorage::sendJSON(CStringA data, CString	sessionName, std::list<CString>& files, bool bSendImages) {
	HTTP_SEND* ps = new HTTP_SEND;
	ps->pDB = m_pDB;
	ps->sessionName = sessionName;
	ps->strJSON = data;
	ps->sTemplate = m_template;
	ps->files = files;
	ps->sBoundary = "FFF3F395A90B452BB8BEDC878DDBD152$";
	ps->data_size = 0;
	ps->bSendImages = bSendImages;

	if (getVer(ps) == 1) {
		ps->sRequestHeaders = MakeRequestHeaders_v1(ps);
		MakePostFileData_v1(ps);
	}
	else {
		ps->sRequestHeaders = MakeRequestHeaders_v2(ps);
		MakePostFileData_v2(ps);
	}
	CSingleLock lock(&ps->lock, TRUE);
	ps->hThread = CreateThread(0, 0, SendProc, ps, 0, 0);

	return 0;
}

DWORD WINAPI CViHTTPStorage::SendProc(LPVOID p) {
	HTTP_SEND* ps = (HTTP_SEND*)p;
	{
		CSingleLock lock(&ps->lock, TRUE);

		sendJSON(ps);

		CloseHandle(ps->hThread);
	}
	delete ps;
	return 0;
}

std::vector<BYTE> CViHTTPStorage::str2v(const CStringA& a) {
	int len = a.GetLength();
	std::vector<BYTE> s(len);
	memcpy(&s[0], (LPCSTR)a, len);
	return s;
}

std::vector<BYTE> CViHTTPStorage::fileData(const CString& path) {
	std::vector<BYTE> s;
	try {
		CFile f;
		CFileException error;
		if (!f.Open(path, CFile::modeRead,&error)) {
			ASSERT(FALSE);
			return s;
		}
		ASSERT(f.GetLength() > 0);
		s.resize(f.GetLength());
		f.Read(&s[0], (UINT)s.size());
		f.Close();
	}
	catch (CFileException* ) {
		s.clear();
		ASSERT(FALSE);
	}
	return s;
}

std::vector<BYTE> CViHTTPStorage::split(std::list< std::vector<BYTE> >& arr) {
	size_t s = 0, pos = 0;
	for (std::list< std::vector<BYTE> >::iterator i = arr.begin(), e = arr.end(); i != e; ++i)
		s += i->size();

	std::vector<BYTE> r(s);
	for (std::list< std::vector<BYTE> >::iterator i = arr.begin(), e = arr.end(); i != e; ++i) {
		memcpy(&r[pos],&((*i)[0]),i->size());
		pos += i->size();
	}
	return r;
}

std::vector<BYTE>	CViHTTPStorage::getData(HTTP_SEND* ps) {
	return str2v(ps->sTemplate);
}

int CViHTTPStorage::getVer(HTTP_SEND* ps) {
#ifndef SEQ_DEFENDERX
	if (ps->sTemplate.Find("\"DATA\":") >= 0)
		return 1;
#endif
	return 2;
}

int CViHTTPStorage::sendJSON(HTTP_SEND* ps) {
	if (getVer(ps) == 1)
		return sendJSON_v1(ps);
	return sendJSON_v2(ps);
}

int CViHTTPStorage::sendJSON_v1(HTTP_SEND* ps) {

	int core = ps->pDB->m_id;

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	int regID = theApp.m_pEngines[core]->engine.Tag2Id(L"VI_HTTP_STORAGE_URL");
	CString url = theApp.m_pEngines[core]->engine.GetVar(regID, 0, VT_BSTR).bstrVal;

	CString pstrServer;
	CString pstrURL;
	INTERNET_PORT nPort = 80;
	DWORD dwServiceType = AFX_INET_SERVICE_HTTP;
	
	if (!AfxParseURL(url, dwServiceType, pstrServer, pstrURL, nPort))
		return 0;

	CInternetSession session(_T("MySession"));
	std::vector<BYTE> sFormData = getData(ps);

	bool bResult = false;

	CHttpConnection* pConnection = session.GetHttpConnection(pstrServer, nPort, NULL, NULL);
	if (!pConnection) {
		ps->pDB->onHttpResult(ps->sessionName, "");
		return 0;
	}
	CHttpFile* pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, pstrURL);
	if (!pFile) {
		pConnection->Close();
		session.Close();
		ps->pDB->onHttpResult(ps->sessionName, "");
		return 0;
	}

	try {

		if (!pFile->AddRequestHeaders(strHeaders)) {
			pFile->Close();
			pConnection->Close();
			session.Close();
			ps->pDB->onHttpResult(ps->sessionName, "");
			return 0;
		}

		BOOL result;
		result = pFile->SendRequest(strHeaders, &sFormData[0], (DWORD)sFormData.size());

		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);
		CString out;
		if (dwRet == HTTP_STATUS_OK)
		{
			char szBuff[1024*8] = { 0 };
			ZeroMemory(szBuff, sizeof(szBuff));
			UINT nRead = pFile->Read(szBuff, sizeof(szBuff)-1);
			if (nRead > 0)
			{
				ps->pDB->onHttpResult(ps->sessionName, szBuff);
				bResult = true;
			}
		}
	}
	catch (CInternetException* ) {
	}

	pFile->Close();
	pConnection->Close();
	session.Close();

	if(!bResult)
		ps->pDB->onHttpResult(ps->sessionName,"");

	return 0;
}

void CViHTTPStorage::MakePostFileData_v1(HTTP_SEND* ps) {
}

CStringA CViHTTPStorage::MakeRequestHeaders_v1(HTTP_SEND* ps) {
	return "";
}



CStringA CViHTTPStorage::MakePostFileData_v2(CStringA& strBoundary, bool bLast)
{
	CStringA strFormat;
	CStringA strData;

	strFormat += ("\r\n\r\n");
	strFormat += ("--%s");
	if (bLast) {
		strFormat += ("--");
	}
	strFormat += ("\r\n");

	strData.Format(strFormat, strBoundary);
	return strData;
}

CStringA CViHTTPStorage::MakePreFileData_v2(const CStringA& strBoundary, const  CStringA& strName, const  CStringA& strFileName, const  CStringA& ct,bool bFirst)
{
	CStringA strFormat;
	CStringA strData;
	if (bFirst) {
		strFormat = ("--%s");
		strFormat += ("\r\n");
		strFormat += ("Content-Disposition: form-data; name=\"user\"");
		strFormat += ("\r\n\r\n");
		strFormat += ("%s");
		strFormat += ("\r\n");

		strFormat += ("--%s");
		strFormat += ("\r\n");
		strFormat += ("Content-Disposition: form-data; name=\"email\"");
		strFormat += ("\r\n\r\n");
		strFormat += ("%s");
		strFormat += ("\r\n");
	}

	strFormat += ("--%s");
	strFormat += ("\r\n");
	strFormat += ("Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"");
	strFormat += ("\r\n");
	strFormat += ("Content-Type: %s");
	strFormat += ("\r\n");
	strFormat += ("Content-Transfer-Encoding: binary");
	strFormat += ("\r\n\r\n");

	if(bFirst)
		strData.Format(strFormat, strBoundary, "DX", strBoundary, "DX@DX", strBoundary,strName, strFileName,ct);
	else
		strData.Format(strFormat, strBoundary, strName, strFileName, ct);

	return strData;
}


void CViHTTPStorage::MakePostFileData_v2(HTTP_SEND* ps) {
	HTTP_DATA d;

// json
	d.strPreFileData = MakePreFileData_v2(ps->sBoundary, "eventData", "eventData.json", "application/json",true);
	d.data = getData(ps);
	bool bLast = !ps->bSendImages || ps->files.size() == 0;

	d.strPostFileData = MakePostFileData_v2(ps->sBoundary, bLast);
	ps->data.push_back(d);
	ps->data_size += d.strPreFileData.GetLength() + d.strPostFileData.GetLength() + d.data.size();

// send files
	if (ps->bSendImages) {
		int pos = 0, cnt = ps->files.size();

		for (std::list<CString>::iterator i = ps->files.begin(), e = ps->files.end(); i != e; ++i,++pos) {
			CString path = *i;
			CStringA name, filename,ext;

			bLast = (pos == cnt - 1);

			if (i == ps->files.begin())
				name = "eventImage";
			else
				name.Format("eventImage%d", (int)(ps->data.size()));

			filename = CStringA(path.Mid(path.ReverseFind('\\') + 1));

			ext = filename.Right(3);
			if (ext.CompareNoCase("jpg") == 0)
				ext = "jpeg";
			d.strPreFileData = MakePreFileData_v2(ps->sBoundary, name, filename, CStringA("image/") + ext,false);
			d.strPostFileData = MakePostFileData_v2(ps->sBoundary, bLast);
			d.data = fileData(path);
			ps->data.push_back(d);
			ps->data_size += d.strPreFileData.GetLength() + d.strPostFileData.GetLength() + d.data.size();
		}
	}
}

CStringA CViHTTPStorage::MakeRequestHeaders_v2(HTTP_SEND* ps) {
	CStringA strFormat;
	CStringA strData;
	strFormat = ("Content-Type: multipart/form-data; boundary=%s\r\n");
	strData.Format(strFormat, ps->sBoundary);
	return strData;
}

int CViHTTPStorage::sendJSON_v2(HTTP_SEND* ps) {
	int core = ps->pDB->m_id;


	DWORD dwTotalRequestLength;

	
	int regID = theApp.m_pEngines[core]->engine.Tag2Id(L"VI_HTTP_STORAGE_URL");
	CString url = theApp.m_pEngines[core]->engine.GetVar(regID, 0, VT_BSTR).bstrVal;

	CString pstrServer;
	CString pstrURL;
	INTERNET_PORT nPort = 80;
	DWORD dwServiceType = AFX_INET_SERVICE_HTTP;

	if (!AfxParseURL(url, dwServiceType, pstrServer, pstrURL, nPort))
		return 0;

	CInternetSession session(_T("MySession"));
	

	bool bResult = false;

	CHttpConnection* pConnection = session.GetHttpConnection(pstrServer, nPort, NULL, NULL);
	if (!pConnection)
		return 0;
	CHttpFile* pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, pstrURL);
	if (!pFile) {
		pConnection->Close();
		session.Close();
		return 0;
	}

	try {

		if (!pFile->AddRequestHeaders(CString(ps->sRequestHeaders))) {
			pFile->Close();
			pConnection->Close();
			session.Close();
			return 0;
		}
		pFile->SendRequestEx(ps->data_size, HSR_SYNC | HSR_INITIATE);

		CStringA prevPost;

		for (std::list < HTTP_DATA >::iterator i = ps->data.begin(), e = ps->data.end(); i != e; ++i) {
			HTTP_DATA& d = *i;
			
			pFile->Write((LPSTR)(LPCSTR)d.strPreFileData, d.strPreFileData.GetLength());

			ASSERT(d.data.size() > 0);

			if(d.data.size() > 0)
				pFile->Write(&d.data[0], d.data.size());

			prevPost = d.strPostFileData;

			//Finish the upload.
			pFile->Write((LPSTR)(LPCSTR)d.strPostFileData, d.strPostFileData.GetLength());
		}

		pFile->EndRequest(HSR_SYNC);

		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);
		CString out;
		if (dwRet == HTTP_STATUS_OK)
		{
			int szSize = 1024 * 8;
#if defined _DEBUG // || true
			szSize *= 1024;
#endif
			std::vector<char> szBuff(szSize+1);
			ZeroMemory(&szBuff[0], szSize+1);
			int nRead = 0, nReadTotal = 0;
			while (szSize > 0 && (nRead = pFile->Read(&szBuff[nReadTotal], szSize)) > 0) {
				nReadTotal += nRead;
				szSize -= nRead;
			}
			if (nReadTotal > 0)
			{
				ps->pDB->onHttpResult(ps->sessionName, &szBuff[0]);
#if defined _DEBUG // || true
				CFile f;
				f.Open(L"E:\\r0001.txt", CFile::modeCreate | CFile::modeWrite);
				f.Write(&szBuff[0], nReadTotal);
				f.Close();
#endif
				bResult = true;
			}
		}
	}
	catch (CInternetException* ) {
	}

	pFile->Close();
	pConnection->Close();
	session.Close();

	if (!bResult)
		ps->pDB->onHttpResult(ps->sessionName, "");

	return 0;
}


CStringA CViHTTPStorage::getImage(CStringA* pFiles, std::list<CString>* pfileList) {

	std::list<CString> all_files = m_pDB->GetFiles(m_sessionName);
	std::list<CString> files;
	CString fp;

	for (std::list<CString>::iterator i = all_files.begin(),e = all_files.end(); i != e; ++i)
	{
		CString fpath = *i;
		CString ext =  PathFindExtensionW(fpath);
		
		if (!ext.CompareNoCase(L".png") || !ext.CompareNoCase(L".jpg") || !ext.CompareNoCase(L".bmp"))
			files.push_back(fpath);

		if (!fp.IsEmpty())
			fp += L"\n";
		fp += fpath;

	}

	if (pfileList)
		* pfileList = files;

	if (pFiles) {
		*pFiles = base64str(fp);
	}

	if(! files.empty() ) // get image from file
		return base64(files.front());

	return "";

}

CStringA CViHTTPStorage::base64str(CString str) {
	std::string a = pugi::as_utf8(str);
	return base64((unsigned char const*)a.c_str(), (unsigned int)a.length());
}

CStringA CViHTTPStorage::base64(CString path) {
	try {
		CFile f;
		if(! f.Open(path,CFile::modeRead) )
			return "";

		int len = (int)f.GetLength();
		std::vector<unsigned char> data(len);
		f.Read(&data[0], len);
		f.Close();

		return base64(&data[0], len);
	}
	catch (CFileException* ) {
	}

	return "";
}

CStringA CViHTTPStorage::base64(unsigned char const* bytes_to_encode, unsigned int in_len)
{
	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	CStringA ret;

	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;
}

void CViHTTPStorage::OnNewVar(int core, int id, int subID) {
	if(35004 == m_idVI_HTTP_STORAGE_URL)
		m_idVI_HTTP_STORAGE_URL  = theApp.m_pEngines[core]->engine.Tag2Id(L"VI_HTTP_STORAGE_URL");

	if (theApp.m_bInit && id == m_idVI_HTTP_STORAGE_URL)
		RegSave(core);
}

CString CViHTTPStorage::RegGroup(int core, LPCWSTR gr) {
	return theApp.m_pEngines[core]->engineReg.RegGroup(gr);
}

void CViHTTPStorage::RegLoad(int core) {
	int regID = theApp.m_pEngines[core]->engine.Tag2Id(L"VI_HTTP_STORAGE_URL");
	m_idVI_HTTP_STORAGE_URL = regID;

	theApp.m_pEngines[core]->engine.AddId(regID, L"VI_HTTP_STORAGE_URL");
	TCHAR url[1024];
	theApp.m_pEngines[core]->engineReg.GetProfileStringEx(RegGroup(core,L"Settings"), L"httpURL", url, 1023);
	theApp.m_pEngines[core]->engine.PutVar(regID, 0, COleVariant(url));
}

void CViHTTPStorage::RegSave(int core) {
	int regID = theApp.m_pEngines[core]->engine.Tag2Id(L"VI_HTTP_STORAGE_URL");

	CString url = theApp.m_pEngines[core]->engine.GetVar(regID, 0,VT_BSTR).bstrVal;
	theApp.m_pEngines[core]->engineReg.WriteProfileStringEx(RegGroup(core, L"Settings"), L"httpURL", url);
}


CStringA CViHTTPStorage::GetReg(CStringA path, CStringA _key)
{
	HKEY key = NULL;
	LONG result = 0;
	char sysnomber[512 * 8] = "";
	CStringA nomber;
	DWORD size = sizeof(sysnomber);
	DWORD type = REG_MULTI_SZ;

	// sprintf_s(sysnomber, "%s\t%s", (LPCSTR)_key, (LPCSTR)_key);

	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		CString(path),
		0, KEY_READ | KEY_WOW64_64KEY, &key);
	if (result != ERROR_SUCCESS)
	{
		return sysnomber;
	}
	result = RegQueryValueExA(key, _key, NULL, &type,
		(LPBYTE)sysnomber, &size);
	if (result != ERROR_SUCCESS)
	{
		return sysnomber;
	}
	if (size > 0 && size < sizeof(sysnomber))
	{
		for (int i = 0; (i + 1) < (int)size; i++)
			if (sysnomber[i] == 0)
				sysnomber[i] = '_';
		sysnomber[size] = 0;
	}

	for (int i = 0; (i + 1) < (int)size; i++)
		if (sysnomber[i] == 0)
			sysnomber[i] = '\n';
	sysnomber[size] = 0;
	return sysnomber;
}


CStringA CViHTTPStorage::fileData(std::list<CString>& files) {
	CStringA r,str;
	if (files.empty())
		return "";

	int cnt = 0;
	r = "{";
	for (std::list<CString>::iterator i = files.begin(), e = files.end(); i != e; ++i,++cnt) {
		if (cnt) r += ",";
		CString name = *i;
		if (name.ReverseFind('\\') >= 0)
			name = name.Mid(name.ReverseFind('\\') + 1);
		str.Format("\"%s\":\"%s\"",(LPCSTR)base64str(name), (LPCSTR)base64(*i));
		r += str;
	}

	r += "}";

	return r;
}

