#pragma once

#ifdef VISTATHIST_EXPORTS
#define VISTATHIST_API __declspec(dllexport)
#else
#define VISTATHIST_API __declspec(dllimport)
#endif

#include <list>

#include "VIEngine.h"

class VISTATHIST_API CViStatHist
{
	void *pb;
public:
	CViStatHist();
	virtual ~CViStatHist();
public:
	typedef struct tagPE
	{
		float E, I, HP, DP,aE,aI, Imax, Imin, Emax, Emin;
		double t;
		double tSys;
		int nqRef;
		int pos;
		int type;
	} PE;

	typedef struct tagPE_LIST
	{
		std::list<PE>	l;
		COLORREF		c;
	} PE_LIST;

	typedef struct tagHIST_RESULT
	{
		double t0;
		float	h[256];
		float	ha[256];

		float				M, D, S;
		float				vMin, vMax;
		int					xMax, xMaxA;

		float				fps, fpsMax;
	} HIST_RESULT;

	typedef struct tagLHIST {
		int posL, posV;
		double t0;
		double		ts, te;

		float				M, D, S;
		float				vMin, vMax;
		int					xMax, xMaxA;

		float				fps, fpsMax;

		int					nqRef;
		double				tSys;
		int					id;
		WCHAR				tag[64];

		bool bSelected;

		int nGroup;

		COLORREF					cMarker;

		float h[256];

		tagLHIST() {
			memset(this, 0, sizeof(*this));
		}
	}LHIST;
public:
	void setQuestion(int nq);

	bool Create(HWND hWndParent);
	HWND GetWindow(LPCWSTR tag);

	bool Start(bool bReset, bool bMarkerMode,double delay,double tLen);
	bool Stop(int nStat, bool bAbort = false);
	bool IsStarted();

	void SetTBest(double tT,double tH = -1);

	void SetGroup(int ngr);
	int GetPE(std::list< PE_LIST >&	arr, bool bAll = false);
	bool RemoveHist(UINT n);
	int GetLHist(std::list<LHIST>& lh);

	void OnNewVar(int id, int subID);

	int		GetHistR(int n, int nr, HIST_RESULT* pRef);
	PE *	GetBioTester(int *pc,bool bPE);

	COLORREF GetMarkerColor(int n);

	double GetMaxT();
	void SetMaxT(double t);

public:
	virtual double		GetTimer() { return 0; }
	virtual CVIENGINE_CLASS *	GetEngine() = 0;
	virtual void		CloseFile(LPCWSTR tag) {}
	virtual bool		WriteFile(LPCWSTR tag, const void *lpBuf, UINT  nCount, bool bLock = true, bool bClose = false) { return false; }
	virtual UINT 		ReadFile(LPCWSTR tag, void *lpBuf, UINT  nCount, bool bLock = true) { return false; }
	virtual	void		OnRClick(int nHist, POINT *p) {}
	virtual bool		GetLang(LPCWSTR id, LPWSTR name, int cch) { return false; }
	virtual bool		SaveAsImage(UINT n);
	virtual int			MsgID() { return WM_USER + 101; }
	virtual int			Clone(CViStatHist& a, int mode,double xt1=0,double xt2=0);

	virtual int GetI1(int id,int subID) { return 0; }
	virtual float GetF1(int id, int subID) { return 0; }


	virtual void Reset();
};

