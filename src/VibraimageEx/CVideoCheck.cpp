#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExFrame.h"
#include "CVideoCheck.h"
#include "AMCap.h"

CVideoCheck::CVideoCheck(int core) :m_id(core) {
	m_tVideo = -1;
	m_nFrame = -1;
	m_devRestartTimer = 0;
	m_tFrame = GetTickCount64();
}

void CVideoCheck::Check() {
	ULONGLONG t = GetTickCount64(), dt = t - m_tFrame;
	if (dt < 10000)
		return;

	if (!theApp.m_bInit || theApp.m_bCloseRqst || theApp.m_bDone || theApp.m_bAbort
		|| !theApp.m_pEngines[m_id] || !theApp.m_pEngines[m_id]->started)
		return;
	CVIENGINE_EX_CLASS& engine = theApp.m_pEngines[m_id]->engine;
	if (!engine.GetVideo())
		return;

	if (!m_docRestart.IsEmpty()) {
		Restart();
		return;
	}

	int nVideo = engine.GetVideo()->GetDeviceVideo();


	double tVideo = engine.GetVar(VI_INFO_TIME_VIDEO, 0, VT_R8).dblVal;
	int nFrame = engine.GetVar(VI_VAR_NFRAME_IN, 0, VT_INT).intVal;

	if (nFrame == m_nFrame && tVideo == m_tVideo) {
		if (dt > 20000)
			OnTimeout();
		return;
	}
	m_tVideo = tVideo;
	m_nFrame = nFrame;
	m_tFrame = t;
}

void CVideoCheck::CheckAll() {
	for (auto i : theApp.m_pEngines) {
		i->pVideoCheck->Check();
	}
}

void CVideoCheck::OnTimeout() {
	CString msg;
	CVIENGINE_EX_CLASS& engine = theApp.m_pEngines[m_id]->engine;

	WCHAR doc[1024] = { 0 };
	engine.GetCurrentDocument(doc, 1024);
	if(!doc[0])
		return;

	msg.Format(_T("camera timeout [%s]"), doc);
	theApp.OnError(m_id, EVENTLOG_ERROR_TYPE, msg);

	if (!engine.GetVar(VI_INFO_CAMERA_LOCK, 0, VT_INT, 1).intVal)
		return;

	if (engine.GetVideo()->CanCmdCaptureStop())
		engine.GetVideo()->CmdCaptureStop();

	if (engine.GetVideo()->CanCmdPreviewStop())
		engine.GetVideo()->CmdPreviewStop();

	if (engine.GetVideo()->CanCmdAviStop())
		engine.GetVideo()->CmdAviStop();

	engine.GetVideo()->Stop();

	m_devRestartTimer = GetTickCount64();
	m_docRestart = doc;
}

void CVideoCheck::Restart() {
	ULONGLONG t = GetTickCount64(), dt = t - m_devRestartTimer;
	if (dt < 10000)
		return;

	CVIENGINE_EX_CLASS& engine = theApp.m_pEngines[m_id]->engine;

	engine.GetVideo()->UpdateDevices();

	if (!m_docRestart.IsEmpty()) {
		theApp.OnOpenDocument(m_id, m_docRestart);
		//	engine.OpenDocument(m_docRestart);
	}

	m_devRestartTimer = 0;
	m_docRestart.Empty();

	m_tFrame = t;
}