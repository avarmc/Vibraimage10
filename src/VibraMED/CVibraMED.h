#pragma once

#include <mmreg.h>
#include <memory>
#include <vector>
#include <atlimage.h>

#include "resource_cnd.h"
#include "ViEngineExSDK.h"
#include "ViEngineExSDK_ID.h"

using namespace CViEngineExNS;

#include <map>
#include <mutex>


class CVibraMED
{
private:
	class MyBase : public CViEngineExBaseInterface
	{
		CVibraMED* pMed;
		typedef struct tagDATA {
			HWND hWnd = 0;
			RGBQUAD* pBits = 0;
			int w = 0; 
			int h = 0;
		}DATA;

		std::recursive_mutex	m_dataLock;
		std::map<int, DATA>		m_data;
	public:
		MyBase(CVibraMED* _pMed) :pMed(_pMed) {
		}

		virtual void OnNewVar(int id, int subID) {
			__super::OnNewVar(id, subID);

			switch (id) {
			case VI_VAR_NFRAME:
			case VI_VAR_NFRAME_OUTF:
				OnCaptureFrame();
				break;
			default: 
				break;
			}
		}
		virtual void OnVideoInput(double dt, BYTE* pBits, int w, int h, int bpp) {
			// Input frame received
			__super::OnVideoInput(dt, pBits, w, h, bpp);
		}
		virtual void OnFaceDetected(VI_FACE_INFO_DATA* pInfo) {
			// Face detected
			 __super::OnFaceDetected(pInfo);
		}
		virtual bool OnResultAlign(int idr, HWND hWnd, DWORD* pBits, int w, int h) {
			
			// memory allocation for result
			std::unique_lock<std::recursive_mutex> lock(m_dataLock);
			auto& data = m_data[idr];
			data.hWnd = hWnd;
			data.pBits = (RGBQUAD*)pBits;
			data.w = w;
			data.h = h;

			return __super::OnResultAlign(idr, hWnd, pBits, w, h);
		}
	public:
		void OnCaptureFrame() {
			std::unique_lock<std::recursive_mutex> lock(m_dataLock);
			for (auto& d : m_data) {
			//	save image data
			}
		}
	};

	class EmptyDB : public CViEngineExDatabaseInterface
	{
		CVibraMED* pMed;
	public:
		EmptyDB(CVibraMED* _pMed):pMed(_pMed) {
		}
		virtual bool	StartSession(LPCWSTR prefix = 0, LPCWSTR postfix = 0) { return false; }
		virtual bool	StopSession(bool bLock = true) { return false; }
		virtual bool	WriteFile(LPCWSTR tag, const void* lpBuf, UINT  nCount, bool bLock = true) { 
			if (CString(tag) == L"M_XML") {
				FILE* f = 0;
				if (!fopen_s(&f, "measure.xml", "wb") && f) {
					fwrite(lpBuf, 1, nCount, f);
					fclose(f);
				}

				float v[12];

				for (int i = 0; i < 12; ++i) {
					v[i] = pMed->getT12_measure(i);
				}
				return true;
			}
			return false; 
		}
	};
public:
	std::shared_ptr< CVIENGINE_EX_CLASS >		m_pEngine;
	MyBase										m_base;
	EmptyDB										m_db;
	HWND										m_hWnd;
	static int									m_T12_id[12];
	static CString								m_T12_name[12];
public:
	CVibraMED();
	~CVibraMED();

	// get Hist Window handle
	HWND										getHistWnd();

	//get list of video devices
	std::vector<CString>						getCameras();

	//select video source
	bool										selectVideoSource(const CString& name, bool isFile);

	// get int value
	int											getI(int id,int subID=0,int nVar=1);
	// get float value
	float										getF(int id, int subID = 0, int nVar = 1);
	// get string value
	CString										getStr(int id, int subID = 0);

	bool										isCalculationStarted();
	bool										isCalculationReady();

	void										startCalculation();
	void										stopCalculation();

	CString										getT12_name(int n);
	float										getT12_value(int n);
	float										getT12_measure(int n);

	void										reset();
	void										clear();

	void										reInit();
};

