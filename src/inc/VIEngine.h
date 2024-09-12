#pragma once

#ifdef VIENGINE_EXPORTS
#	if defined PROCESS_AVX
#		define VIENGINE_API __declspec(dllexport)
#	else
#		define VIENGINE_API
#endif
#else
#define VIENGINE_API __declspec(dllimport)
#endif

#include "resource_cnd.h"
#include "VIVar.h"

enum VI_MODE_RESULT_VARS
{
	VI_RESULT_SRC_A		= 0x0001,
	VI_RESULT_VI0_A		= 0x0002,
	VI_RESULT_VI1_A		= 0x0004,
	VI_RESULT_VI2_A		= 0x0008,
	VI_RESULT_DELTA_A	= 0x0010,
	VI_RESULT_DELTA_FA	= 0x0020,

	VI_RESULT_SRC_B		= 0x0100,
	VI_RESULT_VI0_B		= 0x0200,
	VI_RESULT_VI1_B		= 0x0400,
	VI_RESULT_VI2_B		= 0x0800,
	VI_RESULT_DELTA_B	= 0x1000,
	VI_RESULT_DELTA_FB	= 0x2000,
	VI_RESULT_EXT		= 0x4000,
	VI_RESULT_THERMAL   = 0x8000,

	VI_RESULT_SRC_0		= 0x100000,
	VI_RESULT_FFT_0		= 0x200000,

	VI_RESULT_SRC_MASK = (VI_RESULT_SRC_A|VI_RESULT_SRC_B|VI_RESULT_SRC_0),
	VI_RESULT_DELTA_MASK = (VI_RESULT_DELTA_A|VI_RESULT_DELTA_B),

	VI_RESULT_A_MASK = 0x00FF,
	VI_RESULT_B_MASK = 0xFF00,

	VI_RESULT_EXT_MASK = (VI_RESULT_EXT| VI_RESULT_THERMAL)

};

enum VI_MODE_LD
{
	 VI_MODE_LD_AUDIO	
	,VI_MODE_LD_MANUAL
	,VI_MODE_LD_AUTO
};

enum VI_MODE_LD_CMP
{
	 VI_MODE_LD_CMP_DISABLED = 0
	,VI_MODE_LD_CMP = 1
	,VI_MODE_LD_CMP_CAP = 2
};

enum VI_MODE_FACE_DRAW
{
	VI_MODE_FACE_DRAW_DISABLED = 0
	, VI_MODE_FACE_DRAW_YES = 1
	, VI_MODE_FACE_DRAW_ALL = 2
};

#pragma pack(push, 1)
typedef struct tagVI_FACE_INFO_DATA
{
	BYTE *		pImg;
	RGBTRIPLE *	pImg24;

	int w,h;
	RECT	r;
	float	s;

	int n, cnt;
	ULONGLONG dt; // ((ULONG)GetTickCount64())
	
	// Agression, Stress, Tension, Suspect
	float A, S, T, SV;
	int faceID;
}VI_FACE_INFO_DATA;
#pragma pack(pop)

class CVIEngineBaseRef;
// This class is exported from the VIEngine.dll
class VIENGINE_API CVIENGINE_CLASS 
{
private:
	CVIEngineBaseRef*	m_pBase;
public:

	CVIENGINE_CLASS(int nThread=-1);
	virtual ~CVIENGINE_CLASS(void);

#ifdef _DEBUG
	virtual int GetProfileIntEx(LPCWSTR gr, LPCWSTR key, int def = 0) = 0;
	virtual void WriteProfileIntEx(LPCWSTR gr, LPCWSTR key, int v) = 0;

	virtual BOOL GetProfileStringEx(LPCWSTR gr, LPCWSTR key, LPWSTR res, int cch, LPCWSTR def = _T("")) = 0;
	virtual void WriteProfileStringEx(LPCWSTR gr, LPCWSTR key, LPCWSTR v) = 0;
#else
	virtual int GetProfileIntEx(LPCWSTR gr, LPCWSTR key, int def = 0) { return def; }
	virtual void WriteProfileIntEx(LPCWSTR gr, LPCWSTR key, int v) {}

	virtual BOOL GetProfileStringEx(LPCWSTR gr, LPCWSTR key, LPWSTR res, int cch, LPCWSTR def = _T("")) { return FALSE; }
	virtual void WriteProfileStringEx(LPCWSTR gr, LPCWSTR key, LPCWSTR v) {}
#endif
	virtual int GetProfileBinEx(LPCWSTR gr, LPCWSTR key, void *ptr, int size_max) { return 0; }
	virtual void WriteProfileBinEx(LPCWSTR gr, LPCWSTR key, void *ptr, int size) {}

public:
	bool AddImage(void* p, int w, int h, int bpp,double t);
	bool AddSound(WAVEFORMATEX* pfmt, double dblT, void* pBuffer, int lSize);
	bool AddThermal(DWORD* p, int w, int h, int bpp, double tL, double tS);

	void Sync(int dt = 0);
	void SyncGlobal(int dt = 0);

public:
	bool SetResultPtr(int id,void* ptr, int w, int h, bool bLock);
	bool GetResultPtr(int id,void ** ptr, int* pw, int* ph);
	DWORD GetResultVer(void);
	void* LockResult(bool bLock, void* pLock);
	bool GetSrcLine(int x, int y, float* px=0, float* py=0);
	bool GetSumHist(int id, float* px=0, float* py=0,int a=0);

public:
	void	setFace(int x, int y, int size, DWORD dt = 1000, int faceID=0);
public:
	void Start(void);
	void Stop(bool bFinal=false);
	void Pause(bool bSet);
	void Reset(void);
public:
	bool CanEdit(int id);
	unsigned int GetVersion(int id);
	void GetI(int id,LONG &v1, LONG& v2);
	void GetI(int id,int &v1, int& v2);
	void GetF(int id,float &v1, float& v2);

	LPVOID GetPtr(int id);
	int GetI1(int id);
	int GetI1(int id,int nFace);
	int GetI2(int id);
	float GetF1(int id);
	float GetF1(int id, int nFace);
	float GetF2(int id);

	VI_VAR GetVar(int id);


	void PutVar(int id,const VI_VAR& v);

	void PutI1(int id,int v);
	void PutI2(int id,int v);
	void PutF1(int id,float v);
	void PutF2(int id,float v);
	void PutI(int id,int v1,int v2);
	void PutF(int id,float v1,float v2);

	LPCWSTR GetStr(int id);
	void PutStr(int id, LPCWSTR str, int subID=0);

public:
	void NewSource();
public:
	void SetCallbackOnNewVar(void * pFn, void * pData);
	void RemoveCallbackOnNewVar(void * pFn, void * pData);
	void SetCallbackOnImg8(void * pFn, void * pData);


public:
	bool SeqIsValidCode(LPCWSTR code);
	bool SeqIsHttp(void);
	bool SeqIsOnline(void);
	bool SeqIsOk(void);
	bool SeqIsDemo(void);
	bool  SeqIsLimit(void);
	bool SeqIsInit(void);
	LPCWSTR SeqSerial(void);
	LPCWSTR SeqOwner(void);
	LPCWSTR SeqLimit(void);
	int		SeqGetCounter();
	int		SeqFlags();
public:
	static LPCWSTR Tag(int id);
	int GetID(LPCWSTR tag);

	float GetAVG(int id,int *pCnt=0,int a=0);
	void PutAVG(int id,int cnt);
	void PutAVG(int id,int cnt,float v);

	bool GetStat(int id,float *cMin, float *cMax, float *bMin, float *bMax,float *rate,int *rate_base);
	bool GetStatCalc(int id);
	float GetStatRate1(int id);
	float GetStatRate2(int id);
	float GetStatLevel(int id);
	float GetStatRate1v(int id);
	float GetStatRate2v(int id);

	float GetStatRate1def(int id);
	float GetStatRate2def(int id);
	float GetStatLevelDef(int id);

	int GetStatHist(int id, int* pHist256=0,float * pFPS=0, int a = 0);
	int GetStatHistFFT(int id, float* pHist256=0,float * pFPS=0, int a=0);

	void PutStatCalc(int id, bool bCalc);
	void PutStatRate1(int id, float v);
	void PutStatRate2(int id, float v);
	void PutStatRate1v(int id, float v);
	void PutStatRate2v(int id, float v);
	void PutStatLevel(int id, float v);

	void GetStatPulseHR(float* pR, bool bA);

	void Lock(int bLock);
	bool IsLocked();

	bool IsStarted();

	void RegSave(void);
	void RegLoad(void);

	RGBQUAD* GetPal(void);
	void PutPal(RGBQUAD* pal);
	int Tag2Id(LPCWSTR tag);

	int	GetImgSrc(int *pw=0,int *ph=0,void *ptr=0);
	float* GetImgVI(int n);

	bool SrcMaskLoad(LPCWSTR file);
	bool SrcMaskSave(LPCWSTR file);
	bool SrcMaskIsSet();
	bool SrcMaskErase(int x,int y);

	bool GetAura(int * pCWL, int * pCWR, int * pCR, int * pCL, int nProc, bool bB);

public:
	float	GetCorrelation( int idX, int idY, float *pXY, int *pXYsize, int maxSize);
public:
	void *	StoreCapture();
	void	StoreFree(void *pStore);

	void	StoreStart(void *pStore);
	void	StoreStop(void *pStore);

	void	StoreSetLen(double dt,void *pStore);
	double	StoreGetLen(void *pStore);

	VI_VAR	StoreGetVar(int id,double t,void *pStore);
	int		StoreGetData(int id,double tStart=0,double *pt=0, VI_VAR* pv=0,void *pStore=0);
public:
	void	SetFilter(int id,int n,float sf,float ff,BOOL bHP,BOOL bEN);
	void	GetFilter(int id,int n,float *psf,float *pff,BOOL *pbHP,BOOL *pbEN);

public:
	bool ValidateFace(void* pImg, int w, int h, int bpp, const RECT& r0);

public:
	virtual BOOL ReadURL(LPCTSTR url, LPTSTR pErrMsg, void* pTo, int nMax) { return FALSE; }
	virtual int md5(const char* str, BYTE* vHash) { return 0; }
};

