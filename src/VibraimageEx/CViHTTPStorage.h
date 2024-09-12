#pragma once

#include <map>
#include <list>

class CViEngineExProcDb;


class CViHTTPStorage
{
protected:
	static int	m_idVI_HTTP_STORAGE_URL;

	CViEngineExProcDb* m_pDB;
	CCriticalSection			m_templateLock;
	CStringA					m_template;
	CString						m_sessionName;
	typedef struct tagHTTP_DATA {
		CStringA strPreFileData, strPostFileData;
		std::vector<BYTE>  data;
	}HTTP_DATA;
	typedef struct tagHTTP_SEND
	{
		CViEngineExProcDb*	pDB;
		CStringA			strJSON;
		CCriticalSection	lock;
		CString				sessionName;
		HANDLE				hThread;
		CStringA			sTemplate;
		std::list<CString>	files;
		CStringA			sBoundary, sRequestHeaders;
		std::list < HTTP_DATA > data;
		size_t				data_size;
		bool				bSendImages;
	} HTTP_SEND;
protected:
	CViHTTPStorage();
	void onCreate(CStringA tmp);
	static CStringA	loadTemplate(int core);
	static CStringA base64(CString path);
	static CStringA base64str(CString str);
	static CStringA base64(unsigned char const* bytes_to_encode, unsigned int in_len);
	static DWORD WINAPI SendProc(LPVOID p);
	static CStringA GetReg(CStringA path, CStringA _key);
	static std::vector<BYTE> str2v(const CStringA& a);
	static std::vector<BYTE> fileData(const CString& path);
	static std::vector<BYTE> split(std::list< std::vector<BYTE> >& arr);

public:
	~CViHTTPStorage();

	static CViHTTPStorage* Create(CViEngineExProcDb*pDB);

	virtual void onSessionStarted(LPCWSTR sessName);
	virtual void onSessionEnd();
	
	virtual bool isSet(LPCSTR tag);
	virtual bool set(LPCSTR tag, LPCSTR v);
	virtual bool set(LPCSTR tag, float v);

	static	std::vector<BYTE>	getData(HTTP_SEND* ps);
	virtual CStringA	getCam();
	virtual CStringA	getSysName();
	virtual CStringA	getUserName();
	virtual CStringA	getUUID();
	virtual CStringA	getUUID(const WCHAR* sessName);

	static int getVer(HTTP_SEND* ps);
	static int sendJSON(HTTP_SEND* ps);
	static int sendJSON_v1(HTTP_SEND* ps);
	static int sendJSON_v2(HTTP_SEND* ps);
	virtual int sendJSON(CStringA data,CString	sessionName,std::list<CString>& files,bool bSendImages=true);

	virtual CStringA getImage(CStringA *pFiles,std::list<CString>* pfileList);
	virtual CStringA fileData(std::list<CString>& files);


	static void RegLoad(int core);
	static void RegSave(int core);
	static CString RegGroup(int core, LPCWSTR gr);

	static  void OnNewVar(int core,int id, int subID);
protected:
	virtual void MakePostFileData_v1(HTTP_SEND* ps);
	virtual CStringA MakeRequestHeaders_v1(HTTP_SEND* ps);
	static CStringA MakePostFileData_v1(CStringA& strBoundary);
	static CStringA MakePreFileData_v1(const CStringA& strBoundary, const  CStringA& strName, const  CStringA& strFileName, const  CStringA& ct);

	virtual void MakePostFileData_v2(HTTP_SEND* ps);
	virtual CStringA MakeRequestHeaders_v2(HTTP_SEND* ps);
	static CStringA MakePostFileData_v2(CStringA& strBoundary,bool bLast);
	static CStringA MakePreFileData_v2(const CStringA& strBoundary, const  CStringA& strName, const  CStringA& strFileName, const  CStringA& ct, bool bFirst);
};

