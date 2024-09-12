#include "stdafx.h"
#include "ViEngineExSDK.h"
#include "ViEngineExCapture.h"

CViEngineExCaptureMy::CViEngineExCaptureMy(): m_pBase(0),m_id(-1) {

}

CViEngineExCaptureMy::~CViEngineExCaptureMy() {

}

void CViEngineExCaptureMy::Connect(int core, CVIENGINE_EX_CLASS* pApp) {
	m_id = core;
	m_pBase = pApp->GetCapture();
	CViEngineExNS::CViEngineExAutoCaptureInterface::Connect(pApp);
}

bool	CViEngineExCaptureMy::OnImageSave(bool bAuto, float state) {
//	HBITMAP hbit = this->GetImage(0, VI_RESULT_SRC_0);
	return m_pBase->OnImageSave(bAuto, state); 
}