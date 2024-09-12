#include "pch.h"
#include "CVibraMED.h"
#include "AMCap.h"
#include "viFrameVar.h"

int	CVibraMED::m_T12_id[12] = {
		VI_VAR_STAT_RES_P7,
		VI_VAR_STAT_RES_P6,
		VI_VAR_STAT_RES_F5X,
		VI_VAR_STAT_RES_P19,
		VI_VAR_STAT_RES_P16,
		VI_VAR_STAT_RES_P17,
		VI_VAR_STAT_RES_P8,
		VI_VAR_STAT_RES_P18,
		VI_VAR_STAT_RES_F6,
		VI_VAR_STAT_RES_F9,
		VI_VAR_STAT_RES_P29,
		VI_VAR_STAT_RES_P30
};

CString	CVibraMED::m_T12_name[12] = {
	_T("Aggression (P7)"),
	_T("Stress (P6)"),
	_T("Tension/Anxiety (F5X)"),
	_T("Suspect (P19)"),
	_T("Balance (P16)"),
	_T("Charm (P17)"),
	_T("Energy (P8)"),
	_T("Self regulation (P18)"),
	_T("Inhibition (F6)"),
	_T("Neuroticism (F9)"),
	_T("Depression (P29)"),
	_T("Happiness (P30)")
};

CVibraMED::CVibraMED():m_db(this),m_base(this) {
	m_pEngine = std::make_shared<CVIENGINE_EX_CLASS>();
	m_db.Connect(&(*m_pEngine));
	m_base.Connect(&(*m_pEngine));
	m_pEngine->Start( ::GetDesktopWindow() );
	m_hWnd = m_pEngine->GetWindow();
	::ShowWindow(m_hWnd, SW_HIDE);
	m_pEngine->PutVar(VI_INFO_M_OPEN, 0, COleVariant((long)0));
	auto hist = m_pEngine->GetHist();
	if (hist) {
		hist->AddHist(1, m_pEngine->Tag2Id(L"histC1"),1);
		hist->AddHist(1, m_pEngine->Tag2Id(L"histC2"),1);
		hist->AddHist(2, m_pEngine->Tag2Id(L"histN1"), 1);
		hist->AddHist(2, m_pEngine->Tag2Id(L"histN2"), 2);
		hist->AddHist(3, m_pEngine->Tag2Id(L"histF1"), 1);
		hist->AddHist(3, m_pEngine->Tag2Id(L"histF2"), 1);
	}
	m_pEngine->GetVideo()->SetCaptureAudio(false, false);
}

CVibraMED::~CVibraMED() {
	m_pEngine->Stop();
}

std::vector<CString>	CVibraMED::getCameras() {
	std::vector<CString> devices;
	int cnt = m_pEngine->GetVideo()->GetDeviceVideoCount();
	devices.resize(cnt);

	for (int i = 0; i < cnt; ++i) {
		devices[i] = m_pEngine->GetVideo()->GetDevNameVideoW(i);
	}
	return devices;
}


bool	CVibraMED::selectVideoSource(const CString& name, bool isFile) {
	if (name.IsEmpty()) { // close
		m_pEngine->OnMenu(ID_VIDEOSETTINGS0_STOP);
		return true;
	}

	if (isFile) { // Play AVI
		return !!m_pEngine->OpenDocument(name);
	}

	return !!m_pEngine->OpenDocument(CString(_T("*VIDEO:"))+name);
}




int	CVibraMED::getI(int id, int subID, int nVar) {
	return m_pEngine->GetVar(id, subID, VT_INT, nVar).intVal;
}

float CVibraMED::getF(int id, int subID, int nVar) {
	return m_pEngine->GetVar(id, subID, VT_R4, nVar).fltVal;
}

CString	CVibraMED::getStr(int id, int subID) {
	return m_pEngine->GetVar(id, subID, VT_BSTR).bstrVal;
}

bool	CVibraMED::isCalculationReady() {
	return m_pEngine->GetMeasure()->IsReady();
}

bool	CVibraMED::isCalculationStarted() {
	return m_pEngine->GetMeasure()->IsStarted();
}

void	CVibraMED::startCalculation() {
	m_pEngine->GetMeasure()->Start(10000);
	
}

void	CVibraMED::stopCalculation() {
	m_pEngine->GetMeasure()->Stop(false);
}

CString	CVibraMED::getT12_name(int n) {
	return m_T12_name[n];
}

float	CVibraMED::getT12_value(int n) {
	return getF(m_T12_id[n])*100;
}

float	CVibraMED::getT12_measure(int n) {
	return getF(VI_INFO_M_M,m_T12_id[n])*100;
}

void	CVibraMED::reset() {
	m_pEngine->PutVar(VI_VAR_RESET, 0, COleVariant((long)0));
}

void	CVibraMED::clear() {
	m_pEngine->GetMeasure()->Clear();
}


HWND	CVibraMED::getHistWnd() {
	auto hist = m_pEngine->GetHist();
	if (hist)
		return hist->GetWindow();
	return 0;
}

void	CVibraMED::reInit() {

}