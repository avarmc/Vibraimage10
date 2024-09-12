#include "pch.h"
#include "CVICoreApp.h"

CVICoreApp::CVICoreApp(int argc, wchar_t* argv[]) {

}

CVICoreApp::~CVICoreApp() {
	StopEngine();
}



bool CVICoreApp::StartEngine() {
	if (m_pEngine)
		StopEngine();

	m_pEngine = std::make_shared< CVIEngineCore >();

	m_pEngine->Start();
	m_pEngine->AddImage(0, 0, 0, 0, 0);
	m_pEngine->Reset();
	m_pEngine->PutI1(VI_VAR_NFRAME_OUTF, 0);

	PrintLicense();

	if (!m_pEngine->SeqIsOk()) {
		StopEngine();
		return false;
	}
	
	
	m_pEngine->SetCallbackOnNewVar(CVICoreApp::CallbackOnNewVar, this);
	return true;
}

void CVICoreApp::StopEngine() {
	if (!m_pEngine)
		return;
	m_pEngine->Stop();
}

void CVICoreApp::CallbackOnNewVar(void* pUserData, int id, int subID)
{
	CVICoreApp* pThis = static_cast<CVICoreApp*>(pUserData);
	pThis->OnNewVar(id, subID);
}

void CVICoreApp::OnNewVar(int id, int subID) {
	switch (id) {
	case VI_VAR_NFRAME:
		OnNewFrameProcessed();
		break;
//	case VI_FACE_INFO:
	case VI_FACE_INFO_DETECT:
		OnFaceDetected((VI_FACE_INFO_DATA*)(m_pEngine->GetPtr(id)));
		break;
	default: break;
	}
}

void CVICoreApp::OnFaceDetected(VI_FACE_INFO_DATA *pFace) {
	if (!pFace)
		return;
	std::unique_lock<std::recursive_mutex> lock(m_consoleLock);
	int cx = (pFace->r.left + pFace->r.right);
	int cy = (pFace->r.top + pFace->r.bottom);
	std::wcout << L"FACE:\t" << cx << L"x" << cy << std::endl;
}

void CVICoreApp::OnNewFrameProcessed() {
	PrintInfo();
}

void CVICoreApp::PrintInfo() {
	std::unique_lock<std::recursive_mutex> lock(m_consoleLock);

	std::wcout << L"FRAME:\t" << std::endl;
	

	std::wcout << L"\tn:      \t" << m_pEngine->GetI1(VI_VAR_NFRAME) << std::endl;
	std::wcout << L"\tfpsIn:  \t" << m_pEngine->GetF1(VI_VAR_FPSIN) << std::endl;
	std::wcout << L"\tfpsOutF:\t" << m_pEngine->GetF1(VI_VAR_FPSOUTF) << std::endl;
	std::wcout << L"\tfpsOutR:\t" << m_pEngine->GetF1(VI_VAR_FPSOUTR) << std::endl;


	std::wcout << L"\tAnger:  \t" << m_pEngine->GetF1(VI_VAR_STAT_RES_P7) << std::endl;
	std::wcout << L"\tStress: \t" << m_pEngine->GetF1(VI_VAR_STAT_RES_P6) << std::endl;
	std::wcout << L"\tTension:\t" << m_pEngine->GetF1(VI_VAR_STAT_RES_F5X) << std::endl;
	std::wcout << L"\tSuspect:\t" << m_pEngine->GetF1(VI_VAR_STATE_VAR) << std::endl;
	std::wcout << std::endl;
}

void  CVICoreApp::PrintLicense() {
	std::unique_lock<std::recursive_mutex> lock(m_consoleLock);

	std::wcout << L"LICENSE:\t" << std::endl;
	std::wcout << L"\tOWNER:\t" << m_pEngine->SeqOwner() << std::endl;
	std::wcout << L"\tSERIAL:\t" << m_pEngine->SeqSerial() << std::endl;
	std::wcout << L"\tLIMIT:\t" << m_pEngine->SeqLimit() << std::endl;

}

void CVICoreApp::AddFrame(double t, void* p,int w,int h) {
	if (m_pEngine)
		m_pEngine->AddImage(p, w, h, 24, t);
}