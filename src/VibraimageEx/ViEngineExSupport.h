#pragma once

class CViEngineExSupport
{
	int core;

public:
	CViEngineExSupport(int core_id);
	~CViEngineExSupport();
//////////////////////////////////////////////////////////////
// The OnNewVar function is called automatically for each parameter change.
	void OnNewVar(int id, int subID);

//////////////////////////////////////////////////////////////
// The OnFrameProcessed function is called for each frame processed.
	void OnFrameProcessed();

//////////////////////////////////////////////////////////////
// The OnFaceDetected function is called for each face detected.
	void OnFaceDetected(int id, const RECT& r);

//////////////////////////////////////////////////////////////
// The GetEmotions function is example of Emotion parameters data.
	void GetEmotions();

//////////////////////////////////////////////////////////////
// The GetVar function return parameter value
	COleVariant GetVar(int id, int subID = 0, VARTYPE vt = VT_R4, int nVar = 1);

//////////////////////////////////////////////////////////////
// The OpenVideo used to open Video file
	int OpenVideo(LPCWSTR path);

//////////////////////////////////////////////////////////////
// The OpenCamera used to open camera interface
	int OpenCamera(int n);

//////////////////////////////////////////////////////////////
// The GetCameraName return name of camera interface
	CString GetCameraName(int n);

//////////////////////////////////////////////////////////////
// The CloseVideo disconnect from source video
	void CloseVideo();

//////////////////////////////////////////////////////////////
// The OnFrameImage function is called for each frame video frame.
	void OnFrameImage(double dblSampleTime, LPCWSTR source, void* pBits, long lBufferSize, int w, int h, int bpp);

//////////////////////////////////////////////////////////////
// The GetModeName function return string ID of image mode
	CString GetModeName(int mode);
};

