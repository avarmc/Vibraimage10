#pragma once

#include "CVIEngineCore.h"

class CVICoreApp
{
protected:
	std::shared_ptr<CVIEngineCore>	m_pEngine;
	std::recursive_mutex				m_consoleLock;
public:
	CVICoreApp(int argc, wchar_t* argv[]);
	~CVICoreApp();

public:
	bool StartEngine();
	void StopEngine();
	bool IsStarted() { return m_pEngine ? true : false; }

	void AddFrame(double t, void* p, int w, int h);
protected:


	static void CallbackOnNewVar(void* pUserData, int id, int subID);

	void OnNewVar(int id, int subID);
	void OnNewFrameProcessed();
	void OnFaceDetected(VI_FACE_INFO_DATA* pFace);

	void PrintInfo();
	void PrintLicense();
};

