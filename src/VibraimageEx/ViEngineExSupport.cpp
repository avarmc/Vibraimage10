#include "stdafx.h"
#include "VibraimageEx.h"
#include "ViEngineExProc.h"
#include "VibraimageExFrame.h"
#include "VibraimageExView.h"
#include "AMCap.h"
#include "ViEngineExSupport.h"


CViEngineExSupport::CViEngineExSupport(int core_id):core(core_id) {

}

CViEngineExSupport::~CViEngineExSupport() {

}

void CViEngineExSupport::OnNewVar(int id, int subID) {
	switch (id) {
	case VI_VAR_NFRAME:
		OnFrameProcessed();
		break;
	default: break;
	}
}

void CViEngineExSupport::OnFrameProcessed() {
	GetEmotions();


#if defined VIENGINEEX_GET_ENGINE
	auto engine = theApp.m_pEngines[core]->engine.GetEngine();
	int rmode = engine->GetI1(VI_MODE_RESULT);
	double t = engine->GetF1(VI_VAR_TH);
	for (int i = 0; i < 26; ++i) {
		int mode = (1 << i);
		if ((mode & rmode) == 0)
			continue;

		void* pBits;
		int w, h, bpp = 32;
		if (engine->GetResultPtr(mode, &pBits, &w, &h) && pBits)
		{
			OnFrameImage(GetVar(VI_INFO_TIME_VIDEO).fltVal, GetModeName(mode), pBits, w * h * 4, w, h, bpp);
		}
	}
#endif // #if defined VIENGINEEX_GET_ENGINE
}

void CViEngineExSupport::OnFaceDetected(int id, const RECT& r) {
	ASSERT(TRUE);
}

void CViEngineExSupport::GetEmotions() {
/*
	if (GetVar(VI_INFO_TIME_VIDEO).fltVal < 10)
		return; //skip first 10s

	float T10[12];

	T10[0] = GetVar(VI_VAR_STAT_RES_P7).fltVal;
	T10[1] = GetVar(VI_VAR_STAT_RES_P6).fltVal;
	T10[2] = GetVar(VI_VAR_STAT_RES_F5X).fltVal;
	T10[3] = GetVar(VI_VAR_STAT_RES_P19).fltVal;
	T10[4] = GetVar(VI_VAR_STAT_RES_P16).fltVal;
	T10[5] = GetVar(VI_VAR_STAT_RES_P17).fltVal;
	T10[6] = GetVar(VI_VAR_STAT_RES_P8).fltVal;
	T10[7] = GetVar(VI_VAR_STAT_RES_P18).fltVal;
	T10[8] = GetVar(VI_VAR_STAT_RES_F6).fltVal;
	T10[9] = GetVar(VI_VAR_STAT_RES_F9).fltVal;
*/
}

COleVariant CViEngineExSupport::GetVar(int id, int subID, VARTYPE vt, int nVar) {
	return theApp.m_pEngines[core]->engine.GetVar(id, subID,vt,nVar);
}

int  CViEngineExSupport::OpenVideo(LPCWSTR path) {
	return theApp.OnOpenDocument(core, path);
}

int CViEngineExSupport::OpenCamera(int n) {
	CString cmd;
	cmd.Format(L"*VIDEO:*menu*%d", n);
	return theApp.OnOpenDocument(core, cmd);
}

CString CViEngineExSupport::GetCameraName(int n) {
	return theApp.m_pEngines[core]->engine.GetVideo()->GetDevNameVideoW(n);
}

void CViEngineExSupport::CloseVideo() {
	OpenCamera(-1);
}

void  CViEngineExSupport::OnFrameImage(double dblSampleTime, LPCWSTR source, void* pBits, long lBufferSize, int w, int h, int bpp) {
/*
	if (!pBits || !w || !h) //Empty image
		return;
	CImage img;
	img.Create(w, h, bpp);
	memcpy(img.GetBits(),pBits,min(lBufferSize, w * h * bpp / 8));

	CString path;
	path.Format(L"%s_%d_%llu.png", source, (long)(dblSampleTime * 1000),GetTickCount64());

	img.Save(path);
*/
	ASSERT(TRUE);
}


CString CViEngineExSupport::GetModeName(int mode) {
	switch (mode) {
	case VI_RESULT_SRC_A: return L"VI_RESULT_SRC_A";
	case VI_RESULT_VI0_A: return L"VI_RESULT_VI0_A";
	case VI_RESULT_VI1_A: return L"VI_RESULT_VI1_A";
	case VI_RESULT_VI2_A: return L"VI_RESULT_VI2_A";
	case VI_RESULT_DELTA_A: return L"VI_RESULT_DELTA_A";
	case VI_RESULT_DELTA_FA: return L"VI_RESULT_DELTA_FA";

	case VI_RESULT_SRC_B: return L"VI_RESULT_SRC_B";
	case VI_RESULT_VI0_B: return L"VI_RESULT_VI0_B";
	case VI_RESULT_VI1_B: return L"VI_RESULT_VI1_B";
	case VI_RESULT_VI2_B: return L"VI_RESULT_VI2_B";
	case VI_RESULT_DELTA_B: return L"VI_RESULT_DELTA_B";
	case VI_RESULT_DELTA_FB: return L"VI_RESULT_DELTA_FB";

	case VI_RESULT_SRC_0: return L"VI_RESULT_SRC_0";
	case VI_RESULT_FFT_0: return L"VI_RESULT_FFT_0";
	default: break;
	}
	return CString();
}
