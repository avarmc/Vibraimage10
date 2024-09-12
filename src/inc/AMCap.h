#pragma once

#ifdef VIAMCAP_EXPORTS
#define VIAMCAP_API __declspec(dllexport)
#else
#define VIAMCAP_API __declspec(dllimport)
#endif

// 	static void CallbackOnFrame(void *pUserData,double dblSampleTime, BYTE * pBuffer, long lBufferSize, void * pFormat);

#pragma once

#include <atlimage.h>

enum VIDEO_DEV_MODE { VIDEO_DEV_MODE_CAM, VIDEO_DEV_MODE_IP, VIDEO_DEV_MODE_FFMPEG, VIDEO_DEV_MODE_RTC, VIDEO_DEV_MODE_TEST };
// CPushImageWnd

class VIAMCAP_API CPushImageWnd : public CWnd
{
private:
	void *ptr;
protected:
	DECLARE_DYNAMIC(CPushImageWnd)
	DECLARE_MESSAGE_MAP()
public:
	COLORREF				m_color;
public:
	CPushImageWnd();
	virtual ~CPushImageWnd();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
public:
	bool PutImage(CImage& img);
	bool PutImage(void* pImg, int size);
	bool PutImage(void* pBits, int w, int h, int bpp);
	bool PutImage(LPCWSTR file);
};

class VIAMCAP_API CPushImageAVI
{
private:
	void *ptr;
public:
	CPushImageAVI();
	virtual ~CPushImageAVI();
public:
	bool SetFile(unsigned short const * file,float fps,bool bStream=false);
public:
	enum COMPR { COMPR_NONE, COMPR_MJPEG, COMPR_RGB8, COMPR_PNG };
	bool SetCompression(int mode);
	int GetCompression(void);
public:
	bool PutImage(CImage& img,double t=-1);
	bool PutImage(void* pImg, int size,double t=-1);
	bool PutImage(void* pBits, int w, int h, int bpp,double t=-1);
	bool PutImage(LPCWSTR file,double t=-1);
	bool PutSound(WAVEFORMATEX * pwf, void *pi, int size,double t);
	bool Stop(void);
	LPCWSTR GetType(void);
	bool IsStarted(void);
	void Sync(void);
	void SetCallbackOnGetVideo(void * pFn, void * pData);
	void SetCallbackOnGetAudio(void * pFn, void * pData);
	void SetPreAlloc(LONGLONG size);
	LONGLONG GetPreAlloc();
};


// CAMCapWnd
class VIAMCAP_API CAMCapWnd 
{
public:
	enum VIDEOTYPE  
	{
		VIDEOTYPE_RGB1,	// RGB, 1 bit per pixel (bpp), palettized 
		VIDEOTYPE_RGB4,	//  RGB, 4 bpp, palettized 
		VIDEOTYPE_RGB8,	// RGB, 8 bpp 
		VIDEOTYPE_RGB555,	// RGB 555, 16 bpp 
		VIDEOTYPE_RGB565,	//  RGB 565, 16 bpp 
		VIDEOTYPE_RGB24,	// RGB, 24 bpp 
		VIDEOTYPE_RGB32,	// RGB, 32 bpp 
		VIDEOTYPE_AYUV,	// 4:4:4 YUV format 
		VIDEOTYPE_UYVY,	//  UYVY (packed 4:2:2) 
		VIDEOTYPE_Y411,	//  Same as Y41P 
		VIDEOTYPE_Y41P,	//  Y41P (packed 4:1:1) 
		VIDEOTYPE_Y211,	//  Y211 
		VIDEOTYPE_YUY2,	//  YUY2 (packed 4:2:2) 
		VIDEOTYPE_YVYU,	//  YVYU (packed 4:2:2) 
		VIDEOTYPE_YUYV	// Same as YUY2. (Used by Canopus; FOURCC 'YUYV') 
	};

	enum VIDEO_STANDATRD
	{
    VIDEO_STANDATRD_None       = 0x00000000,
    VIDEO_STANDATRD_NTSC_M     = 0x00000001, 
    VIDEO_STANDATRD_NTSC_M_J   = 0x00000002,  
    VIDEO_STANDATRD_NTSC_433   = 0x00000004,
    VIDEO_STANDATRD_PAL_B      = 0x00000010,
    VIDEO_STANDATRD_PAL_D      = 0x00000020,
    VIDEO_STANDATRD_PAL_H      = 0x00000080,
    VIDEO_STANDATRD_PAL_I      = 0x00000100,
    VIDEO_STANDATRD_PAL_M      = 0x00000200,
    VIDEO_STANDATRD_PAL_N      = 0x00000400,
    VIDEO_STANDATRD_PAL_60     = 0x00000800,
    VIDEO_STANDATRD_SECAM_B    = 0x00001000,
    VIDEO_STANDATRD_SECAM_D    = 0x00002000,
    VIDEO_STANDATRD_SECAM_G    = 0x00004000,
    VIDEO_STANDATRD_SECAM_H    = 0x00008000,
    VIDEO_STANDATRD_SECAM_K    = 0x00010000,
    VIDEO_STANDATRD_SECAM_K1   = 0x00020000,
    VIDEO_STANDATRD_SECAM_L    = 0x00040000,
    VIDEO_STANDATRD_SECAM_L1   = 0x00080000,
    VIDEO_STANDATRD_PAL_N_COMBO = 0x00100000
	};

private:
	void * m_pCapture;
public:
	CAMCapWnd();
	virtual ~CAMCapWnd();

public:
	static bool CheckRegisterDLL();
	static bool RegisterDLL(bool bForse=false,bool bRegister=true);
public:
	BOOL ShowWindow(int nCmdShow);
	BOOL Create(LPCWSTR lpszClassName,LPCWSTR lpszWindowName,DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID,CCreateContext* pContext = NULL);
	void MoveWindow(int x,int y,int nWidth,int nHeight,BOOL bRepaint = TRUE);
	void MoveWindow(LPCRECT lpRect,BOOL bRepaint = TRUE);

	HWND GetWindow(void);

	BOOL DoModalLoop();
public:
	virtual void OnError(int id, const WORD a_type, LPCWSTR msg) {}

	void SetRegistryKey(LPCWSTR key);
	void SetRegistryKeyGroup(LPCWSTR key);
	void SetTimeLimit(bool bUse, DWORD limit);
	void GetTimeLimit(bool &bUse, DWORD &limit);
	void SetFrameRate(bool bUse,double rate); 
	void GetFrameRate(bool &bUse,double &rate); 

	BOOL SetCaptureFileAllocSize(WORD size); 
	WORD GetCaptureFileAllocSize(); 
	BOOL SetCaptureFile(LPCWSTR file);
	LPCWSTR GetCaptureFile();

	BOOL SaveCaptureFile(LPCWSTR tachDstFile);

	bool CanCaptureAudio(int nVideoDev);
	void SetCaptureAudio(bool cap,bool bRestart=true);
	bool GetCaptureAudio();

	void SetCaptureCC(bool cap);
	bool GetCaptureCC();

	void CmdCapture(bool bPaused=false);
	bool CanCmdCapture(void);


	void CmdPreview(void);
	void CmdPreviewStop(void);
	bool CanCmdPreview(void);
	bool CanCmdPreviewStop(void);

	void CmdCaptureStop(int nMode = 0);
	bool CanCmdCaptureStop(void);

	bool CanCmdAviStop(void);
	void CmdAviStop(void);

	void CmdMasterNo(void);
	void CmdMasterVideo(void);
	void CmdMasterAudio(void);
	void ChooseAudioFormat(void);
	bool CanChooseAudioFormat(void);
	bool CanSetCaptureAudio(void);
	bool CanSetCaptureCC(void);
	bool CanSetFrameRate(void);
	bool CanSetTimeLimit(void);
	bool CanSetCaptureFileAllocSize(void);
	bool CanSetCaptureFile(void);
	bool CanSaveCaptureFile(void);
	bool CanCmdMasterNo(void);
	bool CanCmdMasterAudio(void);

	bool CanCmdMasterVideo(void);

	bool CanRTSP(void);

	bool IsCapturing(void);
	bool IsCmdMasterNo(void);
	bool IsCmdMasterAudio(void);
	bool IsCmdMasterVideo(void);
	bool IsPreviewing(void);
	
	void CmdDeviceVideo(int n);
	int GetDeviceVideo(void);
	int GetDeviceVideoCount(void);
	bool IsDeviceVideo(int n);
	bool CanDeviceVideo(void);

	void CmdDeviceAudio(int n);
	int GetDeviceAudio(void);
	int GetDeviceAudioCount(void);
	bool IsDeviceAudio(int n);
	bool CanDeviceAudio(void);

	BOOL IsDeviceVI(int n);
	BOOL PutImageVI(void* pImg, int w, int h, int bpp);

	bool CanCmdDialog(int n);
	void CmdDialog(int n);

	HWND GetRTC(void);

	bool IsThermal(void);
	BOOL GetThermalImage(DWORD* pImg, int* pw, int* ph,double *ptL, double* ptS );

	bool IsAVI(void);
	void Stop(void);
	UINT64 GetFrameCount(void);

	LPCWSTR GetDialogNameVideoW(int n);
	LPCWSTR GetDialogNameAudioW(int n);
	LPCWSTR GetDevNameVideoW(int n);
	LPCWSTR GetDevNameAudioW(int n);
	int		GetDevTypeVideo(int n);

	void	UpdateDevices();

	LPCWSTR ConfigureRTC(int n);
	bool CallRTSJS(LPCWSTR cmd);


	bool IsActive(void);
	bool CanPlayAVI(void);
	bool PlayAVI(LPCWSTR file,LPCWSTR fileOut=0,double tStart=-1);
	bool RestartAVI(double tStart=0);
	double	VideoDuration();
	double	VideoPosition();
	LPCWSTR GetAVIFile();
	bool GoTo(double t, bool bPause);
	bool SetState(int state);
	int GetState();

	bool CanRenderAudio(void);
	bool GetRenderAudio(void);
	void SetRenderAudio(bool render);

	void SetCallbackData(void* data);
	void SetCallbackRTC(void* pFn);
	void SetCallbackVideo(void * pFn);
	void SetCallbackPaint(void * pFn);
	void SetCallbackAudio(void * pFn);
	void SetCompressorName(LPCWSTR name);
	void SetCompressor(int set);
	bool GetCompressor(void);
	bool CanSetCompressor(void);
	void SetCaptureMessage(bool bShow, LPCWSTR title, LPCWSTR msg);
	void SetNotifyWindow(HWND hWndNotify);
	void SetAutoLoad(bool bSet);
	double GetCurrentTimeMS(void);
	LPCWSTR GetCurrentFile(void);
	int GetInplaceVideo(void);
	void SetInplaceVideo(int v);
	void SetFormatRqst(int w, int h);
	bool SetBPP(int bpp);
	int GetBPP(void);
	bool SetAVICM(int mode);
	int GetAVICM(void);
	void SetLoadDevices(int bLoad);
	void CmdFilter(LPCWSTR caption,LPCWSTR config,LPCWSTR ok,LPCWSTR cancel);
	void SetDefFrameRate(double fr);
	double GetDefFrameRate(void);
	void SetAlertNoDev(bool bSet);
	bool GetLastTimes(double* pv, double* pa);

	int GetVideoType(void);
	bool PlayIP(LPCWSTR host, int port, LPCWSTR user, LPCWSTR pwd, LPCWSTR url,float fps,bool bStop=false,LPCWSTR ptz=0,bool bRTSP=false);
	float GetFpsIP(void);
	float GetFpsIPBase(void);
	bool IsIP();

	void SetGEOPath(LPCWSTR path);

	void SetPreferredVideoStandard(int type);
	int  GetPreferredVideoStandard();

	int GetRotate();
	void SetRotate(int nRotate);

	bool GetCaptureAborted(bool bReset = true);

	void CmdCaptureIS(void);
	void CmdCaptureISStop(void);
	bool CanCmdCaptureIS(void);
	bool IsCaptureIS(void);
	bool GetIPCamError(bool bReset=false);

	CString GetCamDefaults();
	void SetCamDefaults(int nSet); // 0-disable; 1-set it once; 2-always set
	void SaveCamDefaults();
	void LoadCamDefaults(bool bReset = false);

	bool IsAudioStream();
	bool IsVideoStream();

	void EnableFlags(int flag,CString tag);
	void RestoreFlags(CString state, CString tag);

	void ProcessMessages();
};


class VIAMCAP_API CAVIFileFrameSequence
{
private:
	void * m_ptr;
public:
	 CAVIFileFrameSequence();
	~CAVIFileFrameSequence();
public:
	bool SetFile(LPCWSTR file,bool bRenderAudio=false);
	int GetImageSize(int* pW=0, int* pH=0);
	bool Stop(void);
	bool GetImage(void * pBuffer,DWORD t);
	bool GetImageCur(void * pBuffer);
public:
	DWORD GetTotalTime(void);
	DWORD GetCurrentPos(double *pt=0);
public:
	bool Play(double rate=0);
	bool Pause(void);
	int GetPlay(void);
	bool SetBPP(int bpp);
	int GetBPP(void);
	bool Jump(DWORD t);

	void SetCallbackVideo(void * pFn,void *pData);
	bool PutRate(double rate);
	double GetRate(void);
};

class VIAMCAP_API CAVIVideoEdit
{
private:
	void *ptr;
public:
	CAVIVideoEdit();
	~CAVIVideoEdit();
public:
	BITMAPINFOHEADER	m_fmt;
public:
	void SetCallbackOnVideo(void * pFn, void * pData);
	void SetNotify(HWND hwnd,UINT msg);	

	bool Run(LPCWSTR src,LPCWSTR dst,int compress=0);
	static void OnNotify(DWORD_PTR lParam);
};

class VIAMCAP_API CAMCapRecompressor 
{
private:
	void *ptr;
public:
	BOOL					bProcessing;
	BITMAPINFOHEADER		m_fmtVideo;
	WAVEFORMATEX			m_fmtAudio;
public:
	CAMCapRecompressor();
	~CAMCapRecompressor();
public:
	virtual bool	ProcessFile(unsigned short const * src,unsigned short const * dst,bool bWait=true,double* pTotalTime=0);
	virtual bool	Release();
	void SetCallback(void* pFnVideo, void* pFnAudio, void* pUserData);
	double Progress(void);
	bool CheckReady(void);
};