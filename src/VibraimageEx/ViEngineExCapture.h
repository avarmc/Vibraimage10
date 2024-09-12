#pragma once
#include "ViEngineExSDK.h"

using namespace  CViEngineExNS;

class CViEngineExCaptureMy :
	public CViEngineExNS::CViEngineExAutoCaptureInterface
{
protected:
	int	m_id;
	CViEngineExNS::CViEngineExAutoCaptureInterface* m_pBase;
public:
	CViEngineExCaptureMy();
	~CViEngineExCaptureMy();
	void Connect(int core, CVIENGINE_EX_CLASS* pApp);

public:
	virtual bool	OnImageSave(bool bAuto, float state = -1);
	virtual bool	OnCriticalState(int mode = 0, float state = -1) { return  m_pBase->OnCriticalState(mode,state); }
	virtual float	GetDT() { return  m_pBase->GetDT(); }
	virtual void	SetDT(float dt) { m_pBase->SetDT(dt); }
	virtual int		GetHideT() { return  m_pBase->GetHideT(); }
	virtual void	SetHideT(float dt) { m_pBase->SetHideT(dt); }
	virtual int		GetImageCount() { return  m_pBase->GetImageCount(); }
	virtual int		GetImageCountMax() { return  m_pBase->GetImageCountMax(); }
	virtual void	SetImageCountMax(int n) { m_pBase->SetImageCountMax(n); }
	virtual HBITMAP	GetImage(int pos, int id) { return  m_pBase->GetImage(pos,id); }
	virtual bool	IsAutoCapture() { return  m_pBase->IsAutoCapture(); }
	virtual HWND	GetWindow() { return  m_pBase->GetWindow(); }
	virtual bool	GetFileCapture() { return  m_pBase->GetFileCapture(); }
	virtual void	SetFileCapture(bool bCapture) { m_pBase->SetFileCapture(bCapture); }
	virtual bool	GetFileCaptureXML() { return  m_pBase->GetFileCaptureXML(); }
	virtual void	SetFileCaptureXML(bool bCapture) { m_pBase->SetFileCaptureXML(bCapture); }
	virtual bool	GetFileCaptureFace() { return  m_pBase->GetFileCaptureFace(); }
	virtual void	SetFileCaptureFace(bool bCapture) { m_pBase->SetFileCaptureFace(bCapture); }
	virtual bool	GetFileCaptureOriginal() { return  m_pBase->GetFileCaptureOriginal(); }
	virtual void	SetFileCaptureOriginal(bool bCapture) { m_pBase->SetFileCaptureOriginal(bCapture);  }
	virtual bool	GetFilePrintSuspect() { return  m_pBase->GetFilePrintSuspect(); }
	virtual void	SetFilePrintSuspect(bool bPrint) { m_pBase->SetFilePrintSuspect(bPrint); }
	virtual int		GetFileCaptureGroupBy() { return  m_pBase->GetFileCaptureGroupBy(); }
	virtual void	SetFileCaptureGroupBy(int v) { m_pBase->SetFileCaptureGroupBy(v); }

	virtual void	CheckAndPurge() { m_pBase->CheckAndPurge(); }
	virtual void	OnFaceDetected(VI_FACE_INFO_DATA* pInfo) { m_pBase->OnFaceDetected(pInfo);  }
	virtual void	RegLoad(void) { m_pBase->RegLoad(); }
	virtual void	RegSave(void) { m_pBase->RegSave(); }
	virtual LPCWSTR GetImageExt() { return  m_pBase->GetImageExt(); }

	virtual int	GetCaptured(HBITMAP* pBM, DWORD* pT, DWORD* pID, int nMax) { return  m_pBase->GetCaptured(pBM,pT,pID,nMax); }

	virtual void	SetStatus(LPCWSTR sessionName, LPCSTR status) { m_pBase->SetStatus(sessionName,status); }

	virtual bool	CheckCritical(CImage& imgSrc, float level) { return  m_pBase->CheckCritical(imgSrc,level); }
};


