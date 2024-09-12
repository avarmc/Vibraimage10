#include "stdafx.h"
#include "VibraimageEx.h"
#include "VibraimageExDoc.h"
#include "VibraimageExProp.h"
#include "VibraimageExFrame.h"
#include "VibraimageExGraph.h"
#include "VibraimageExHist.h"
#include "VibraimageExQT.h"
#include "VibraimageExAIT.h"
#include "VibraimageExAIV.h"
#include "VibraimageExLD.h"
#include "VibraimageExLDR.h"
#include "VibraimageExSV.h"
#include "VibraimageExAI.h"
#include "VibraimageExVPos.h"
#include "VibraimageExView.h"
#include "VibraimageExHistStat.h"
#include "PwdDlg.h"
#include "PwdDlg2.h"
#include "regdelete.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CVibraimageExProp::CVibraimageExProp(CPropertiesPack *pPack,int iid) : CPropertiesWndProp(pPack,iid)
{

}


CVibraimageExProp::~CVibraimageExProp()
{

}

BEGIN_MESSAGE_MAP(CVibraimageExProp, CPropertiesWndProp)
	ON_WM_TIMER()
END_MESSAGE_MAP()

CString CVibraimageExPropPack::GetConfigBase()
{
	return theApp.m_pEngine->engine.GetConfigBase();
}

CPropertiesWndBase * CVibraimageExPropPack::CreateWnd(const CStringA& type, int iid)
{
	if (type == "property")
		return new CVibraimageExProp( this,iid );
	if (type == "graph")
		return new CVibraimageExGraph(this,iid);
	if (type == "histogram")
		return new CVibraimageExHist(this,iid);
	if (type == "histstat")
		return new CVibraimageExHistStat(this, iid);
	if (type == "qt")
		return new CVibraimageExQT(this, iid);
	if (type == "ait")
		return new CVibraimageExAIT(this, iid);
	if (type == "aiv")
		return new CVibraimageExAIV(this, iid);
	if (type == "ld")
		return new CVibraimageExLD(this, iid);
	if (type == "sv")
		return new CVibraimageExSV(this, iid);
	if (type == "ldr")
		return new CVibraimageExLDR(this, iid);
	if (type == "ai")
		return new CVibraimageExAI(this, iid);
	if (type == "vpos")
		return new CVibraimageExVPos(this, iid);
	return 0;
}

int CVibraimageExPropPack::Tag2Id(const CString& sid)
{
	int iid = theApp.m_pEngine->engine.Tag2Id( CString(sid) );
	return iid;
}

int CVibraimageExPropPack::GetOwnerId(const CStringA& sid)
{
	if (sid == "engine" || sid == "app" || sid == "frame")
		return O_ENGINE;
	return 0;
}

bool CVibraimageExPropPack::SetValue(ITEM *pItem)
{
	size_t cnt = pItem->values.size();
	bool ok = false;
	for (size_t i = 0; i < cnt; ++i)
	{
		if (SetValue(&(pItem->values[i])))
			ok = true;
	}
	return ok;
}

bool CVibraimageExPropPack::SetValue(ITEM_VALUE *pItem)
{
	if (pItem->iid <= 0)
		return false;

	COleVariant v = ReScale( pItem, pItem->currentValue, true );

	if (pItem->ownerId == O_ENGINE )
	{
		CVIENGINE_EX_CLASS &engine = (theApp.m_pEngine->engine);

		switch (pItem->typeId)
		{
		case VI_VAR_SUB_F1:
			engine.PutVar(pItem->iid, pItem->iid2, v,1);
			return true;
		case VI_VAR_SUB_F2:
			engine.PutVar(pItem->iid, pItem->iid2, v,2);
			return true;
		case VI_VAR_SUB_BOOL1:
			engine.PutVar(pItem->iid, pItem->iid2,  (_variant_t)(bool)!!v.boolVal,1);
			return true;
		case VI_VAR_SUB_BOOL2:
			engine.PutVar(pItem->iid, pItem->iid2, (_variant_t)(bool)!!v.boolVal,2);
			return true;
		case VI_VAR_SUB_I1:
			engine.PutVar(pItem->iid, pItem->iid2, v,1);
			return true;
		case VI_VAR_SUB_I2:
			engine.PutVar(pItem->iid, pItem->iid2, v,2);
			return true;
		case VI_VAR_SUB_SELECT1:
			engine.PutVar(pItem->iid, pItem->iid2, (_variant_t)(long)GetValueSelect(pItem, (LPCWSTR)v.bstrVal),1);
			return true;
		case VI_VAR_SUB_SELECT2:
			engine.PutVar(pItem->iid, pItem->iid2, (_variant_t)(long)GetValueSelect(pItem, (LPCWSTR)v.bstrVal),2);
			return true;
		case VI_VAR_SUB_STRING:
			engine.PutVar(pItem->iid, pItem->iid2, v);
			return true;
		default: return false;
		}
	}

	return false;
}


bool CVibraimageExPropPack::GetValue(ITEM *pItem, VARTYPE vt)
{
	size_t cnt = pItem->values.size();
	bool ok = false;
	for (size_t i = 0; i < cnt; ++i)
	{
		if (GetValue(&(pItem->values[i]), vt))
			ok = true;
	}
	return ok;
}

bool CVibraimageExPropPack::GetValue(ITEM_VALUE *pItem, VARTYPE vt)
{
	if(pItem->iid <= 0)
		return false;
	
	bool ok = false;
	COleVariant v = pItem->currentValue;

	if (pItem->ownerId == O_ENGINE )
	{
		CVIENGINE_EX_CLASS &engine = (theApp.m_pEngine->engine);

		switch (pItem->typeId)
		{
		case VI_VAR_SUB_I1:
		case VI_VAR_SUB_F1:
		case VI_VAR_SUB_BOOL1:
			v = engine.GetVar(pItem->iid, pItem->iid2,vt,1);
			ok = true;
			break;
		case VI_VAR_SUB_I2:
		case VI_VAR_SUB_F2:
		case VI_VAR_SUB_BOOL2:
			v = engine.GetVar(pItem->iid, pItem->iid2, vt,2);
			ok = true;
			break;
		case VI_VAR_SUB_STRING:
			v = engine.GetVar(pItem->iid, pItem->iid2, vt, 1);
			ok = true;
			break;
		case VI_VAR_SUB_SELECT1:
			v = GetValueSelect(pItem, engine.GetVar(pItem->iid, pItem->iid2,VT_INT,1).intVal );
			ok = true;
			break;
		case VI_VAR_SUB_SELECT2:
			v = GetValueSelect(pItem, engine.GetVar(pItem->iid, pItem->iid2, VT_INT, 1).intVal);
			ok = true;
			break;
		default: return false;
		}
	} 


	if (v.vt == VT_EMPTY)
		ok = false;

	if (ok)
		pItem->currentValue = ReScale(pItem, v, false);
	return ok;
}

void CVibraimageExProp::OnNewVar(int id, int subID)
{
	if (m_hWnd)
	{
		SetUpdate(id, subID);
	}
}

void CVibraimageExProp::OnTimer(UINT_PTR nIDEvent)
{
	CPropertiesWndProp::OnTimer(nIDEvent);
}

void CVibraimageExProp::UpdateColor(ITEM *pItem)
{
	if (!pItem || !theApp.m_pEngine->engine.UpdateColor(pItem->values[0].iid, pItem->values[0].iid2, 0, &pItem->clrFillValue))
		CPropertiesWndProp::UpdateColor(pItem);
}

bool CVibraimageExPropPack::DockPane(PANE& pane)
{
	std::vector<CStringA> tab = explode(pane.tab, CStringA("/"));
	if (tab[0] == "view")
	{
		if (theApp.m_pView)
		{
			pane.pWnd->m_bSimple = true;
			pane.pWnd->ModifyStyle(CBRS_FLOAT_MULTI | CBRS_ALIGN_ANY | WS_CAPTION, 0);
			pane.pWnd->EnableDocking(0);
			pane.pWnd->EnableGripper(FALSE);
			CString title = GetLang(CString(pane.title), CString(pane.title));
			theApp.m_pView->AddTab(pane.pWnd, title, -1, FALSE);
			return true;
		}
	} else
	if (theApp.m_pView)
		theApp.m_pView->AddTab(0, 0, -1, FALSE);
	return false;
}

LPCWSTR	CVibraimageExPropPack::GetLang(int iid, LPCWSTR def)
{
	return theApp.m_pEngine->engine.GetLang()->Get(iid, def);
}

LPCWSTR	CVibraimageExPropPack::GetLang(LPCWSTR sid, LPCWSTR def)
{
	return theApp.m_pEngine->engine.GetLang()->Get(sid, def);
}

int		CVibraimageExPropPack::GetLangId(LPCWSTR sid)
{
	return theApp.m_pEngine->engine.GetLang()->Find(sid);
}

int CVibraimageExPropPack::GetProfileInt(LPCWSTR szKey, LPCWSTR szEnt, int def )
{
	return theApp.m_pEngine->engine.GetReg()->GetProfileIntEx(szKey, szEnt, def);
}

void CVibraimageExPropPack::WriteProfileInt(LPCWSTR szKey, LPCWSTR szEnt, int v)
{
	return theApp.m_pEngine->engine.GetReg()->WriteProfileIntEx(szKey, szEnt, v);
}

CString CVibraimageExPropPack::GetProfileString(LPCWSTR szKey, LPCWSTR szEnt, CString def )
{
	TCHAR str[2048] = { 0 };
	theApp.m_pEngine->engine.GetReg()->GetProfileStringEx(szKey, szEnt, str, 2048,def);
	return str;
}

void CVibraimageExPropPack::WriteProfileString(LPCWSTR szKey, LPCWSTR szEnt, CString v)
{
	return theApp.m_pEngine->engine.GetReg()->WriteProfileStringEx(szKey, szEnt, v);
}

CVibraimageExProp::ITEM* CVibraimageExProp::AddItem(ITEM* pGroup, xml_node& xml)
{
	CStringA sid = xml.attribute_value("id");
	ITEM* pItem = 0;


#if  !defined SEQ_MEDIC_MOL
	if (sid == "VI_INFO_USER_DB")
	{
		theApp.m_pDBList = this;
		int cnt = theApp.m_pEngine->engine.GetDB()->GetItemCount();
		for (int i = 0; i < cnt; ++i)
		{
			char num[16];
			sprintf_s(num, 16, "%i", i);
			xml_attribute a = xml.attribute("id2");
			if (!a)
				a = xml.append_attribute("id2");
			a.set_value(num);

			pItem = CPropertiesWndProp::AddItem(pGroup, xml);
		}
	} else
#endif
		pItem = CPropertiesWndProp::AddItem(pGroup, xml);

	if (sid == "VI_INFO_M_PROGRESS")
	{
		theApp.m_pMList = this;
	}

	return pItem;
}

void CVibraimageExProp::OnChangeSelection(ITEM *pItem)
{
#if  !defined SEQ_MEDIC_MOL
	if (pItem && !pItem->values.empty() && pItem->values[0].iid == VI_INFO_USER_DB)
	{
		theApp.m_pEngine->engine.GetDB()->SelectItem(pItem->values[0].iid2);
		SetUpdateAll();
	}
#endif
}


bool CVibraimageExProp::CanEdit(CMFCPropertyGridProperty * pItm) {
	ITEM* pI = (ITEM*)pItm->GetData();

	if(!pI   || !pI->pBase )
		return false;

	for (auto& a : pI->values) {
		if (!theApp.m_pEngine->engine.CanEdit(a.iid))
			return false;
	}
	if (theApp.IsRegLocked())
		return false;
	return true;
}


bool CVibraimageExProp::OnClickValue(ITEM *pItem) {
	if (theApp.IsRegLocked())
		return false;

	if (pItem->values.size() == 1 && pItem->values[0].iid == VI_INFO_START_PWD1) {
		CPwdDlg2 dlg(VI_INFO_START_PWD_OK);
		dlg.DoModal();
		return true;
	}
	if (pItem->values.size() == 1 && pItem->values[0].iid == VI_INFO_LOCK_PWD1) {
		CPwdDlg2 dlg(VI_INFO_LOCK_PWD_OK);
		dlg.DoModal();
		return true;
	}

	return CPropertiesWndProp::OnClickValue(pItem);
}

bool CVibraimageExProp::OnDblClk(ITEM *pItem) {
	if (theApp.IsRegLocked())
		return false;
	if (pItem->values.size() == 1 && pItem->values[0].iid == VI_INFO_START_PWD1) {
		CPwdDlg2 dlg(VI_INFO_START_PWD_OK);
		dlg.DoModal();
		return true;
	}
	if (pItem->values.size() == 1 && pItem->values[0].iid == VI_INFO_LOCK_PWD1) {
		CPwdDlg2 dlg(VI_INFO_LOCK_PWD_OK);
		dlg.DoModal();
		return true;
	}
	return CPropertiesWndProp::OnDblClk(pItem);
}

COLORREF CVibraimageExProp::GetWndColor(int id) {
	if (theApp.m_pEngine)
		return theApp.m_pEngine->engineInterface.GetWndColor(id);
	return __super::GetWndColor(id);
}

bool CVibraimageExPropPack::CheckConfigVer(LPCWSTR path)
{
	auto cfg = theApp.m_pEngine->engine.GetConfigBase();
	if (!cfg)
		return true;

	CString mpath = path ? path : GetDefaultPath() + cfg + _T("\\frame.xml");
	std::map<CString, CString> files;
	// list all config files and make md5
	CString spath = mpath.Left(mpath.ReverseFind('\\') + 1) + _T("*.*");
	CFileFind ff;
	BOOL ok = ff.FindFile(spath);
	while (ok)
	{
		ok = ff.FindNextFile();
		if (ff.IsDots() || ff.IsDirectory())
			continue;
		CString name = ff.GetFileName();
		name.MakeUpper();

		files[name] = ff.GetFilePath();
	}

	// prepare md5
	HCRYPTPROV hCryptProv;
	HCRYPTHASH hHash;

	if (!CryptInit(&hCryptProv))
		return false;

	if (!CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash))
		return false;

	BYTE vHash[256];
	DWORD hLen = sizeof(vHash);
	ZeroMemory(vHash, hLen);

	CStringA md5;

	// read sorted files
	std::map<CString, CString>::iterator i, ie;

	for (i = files.begin(), ie = files.end(); i != ie; ++i)
	{
		std::vector<BYTE> data;

		CFile file;
		try {
			if (file.Open(i->second, CFile::modeRead))
			{
				UINT fs = (UINT)file.GetLength();
				data.resize(fs);
				file.Read(&data[0], fs);
				file.Close();
			}
		}
		catch (CFileException *)
		{

		}
		if (data.size() > 0)
			CryptHashData(hHash, &data[0], (DWORD)data.size(), CRYPT_USERDATA);
	}

	if (CryptGetHashParam(hHash, HP_HASHVAL, vHash, &hLen, 0))
	{
		md5 = ToHex(vHash, hLen);
	}

	if (hHash)
		CryptDestroyHash(hHash);
	if (hCryptProv)
		CryptReleaseContext(hCryptProv, 0);


	CString md5cur(md5);
	CString md5prev = AfxGetApp()->GetProfileString(_T("Settings"), _T("configVer"));

	if (md5cur == md5prev)
		return false;

	AfxGetApp()->WriteProfileString(_T("Settings"), _T("configVer"), md5cur);
	PurgeRegConfig();

	return true;
}

void CVibraimageExPropPack::PurgeRegConfig(LPCTSTR key)
{
	if (!key)
	{
		PurgeRegConfig(_T("Workspace"));
		PurgeRegConfig(_T("Property"));
		PurgeRegConfig(_T("Docking"));
		return;
	}

	RegDelnode(HKEY_CURRENT_USER,CString(_T("SOFTWARE\\"))+ theApp.m_regString + _T("\\VibraimageEx\\" + key));
}

bool  CVibraimageExPropPack::CanLoadState()
{
	return theApp.CanLoadState() && theApp.m_pEngine->engine.GetSeq()->SeqIsOk();
}
bool CVibraimageExPropPack::CanSaveState()
{
	return theApp.m_pEngine->engine.GetSeq()->SeqIsOk();
}

