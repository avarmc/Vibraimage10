#pragma once
#include "ViEngineExSDK.h"
#include "ViEngineExSupport.h"

using namespace  CViEngineExNS;

class CViEngineExProc :	public CViEngineExBaseInterface
{
public:
	int									m_id;
	std::shared_ptr<CViEngineExSupport>	m_support;
	bool								m_bCloseRqst = false;
public:
	CViEngineExProc();
	virtual ~CViEngineExProc();


	std::map< int, CRect >	m_wndRects;
public:
	void Connect(int core, CVIENGINE_EX_CLASS* pApp);

	
	virtual void OnNewVar(int id, int subID);

	virtual bool IsOptionSet(LPCWSTR key, int shift);
	virtual bool GetOption(LPCWSTR key, int shift, LPWSTR param, int cch);

	virtual bool OnResultAlign(int idr, HWND hWnd, DWORD * pBits, int w, int h);
	virtual void OnFaceDetected(VI_FACE_INFO_DATA *pInfo);

	virtual void OnPlaySound(int id, float v, float lev, LPCWSTR path, ULONGLONG t);

	virtual void OnLoginWindow(bool bShow);

	virtual void OnError(int id, const WORD a_type, LPCWSTR msg);
	int	FindResult(CPoint p,CRect *pr = 0);

	virtual void OnCloseRequest();

	virtual COLORREF GetWndColor(int id);

	virtual HWND GetMainWnd();

	virtual void Log(LPCWSTR msg);
public:
	virtual bool GetVideoChannel(LPWSTR name, long cch_max);
public:
	virtual int OnVideoFrame(double dblSampleTime, BYTE* pBuffer, long lBufferSize, void* pFormat);
	virtual int OnAudioFrame(double dblSampleTime, BYTE* pBuffer, long lBufferSize, void* pFormat);
public:
	/// MOL Functions:
	virtual LPCSTR molMakePreFileData(LPCSTR strData, LPCSTR strBoundary, LPCSTR strFileName);
	virtual LPCWSTR molMakeUrl(LPCWSTR strUrl);
	virtual void molResponse(LPCSTR strResponse);

protected:
	/// MOL Data:
	CStringA m_molData;
	CString m_molUrl;
};

