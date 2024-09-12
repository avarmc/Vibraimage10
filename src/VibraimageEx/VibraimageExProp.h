#pragma once
#include "PropertiesWndProp.h"
#include "PropertiesPack.h"
#include "NewVarEventVI.h"

using namespace CDockablePaneVI;

class CVibraimageExPropPack :	public CPropertiesPack
{
public:
	enum OWNER { O_ENGINE = 1 };
public:
	CVibraimageExPropPack(CFrameWndEx *pFrame):CPropertiesPack(pFrame) {}
	~CVibraimageExPropPack() {}

	virtual CString GetConfigBase();

	virtual bool CanLoadState();
	virtual bool CanSaveState();

	virtual CPropertiesWndBase * CreateWnd(const CStringA& type, int iid);
	virtual bool SetValue(ITEM *pItem);
	virtual bool GetValue(ITEM *pItem, VARTYPE vt);

	virtual bool SetValue(ITEM_VALUE *pItem);
	virtual bool GetValue(ITEM_VALUE *pItem, VARTYPE vt);

	virtual int GetOwnerId(const CStringA& sid);
	virtual int Tag2Id(const CString& sid);
	virtual bool DockPane(PANE& pane);

	virtual LPCWSTR	GetLang(int iid, LPCWSTR def = 0);
	virtual LPCWSTR	GetLang(LPCWSTR sid, LPCWSTR def = 0);
	virtual int		GetLangId(LPCWSTR sid);

	virtual int GetProfileInt(LPCWSTR szKey, LPCWSTR szEnt, int def = 0);
	virtual void WriteProfileInt(LPCWSTR szKey, LPCWSTR szEnt, int v);

	virtual CString GetProfileString(LPCWSTR szKey, LPCWSTR szEnt, CString def = _T(""));
	virtual void WriteProfileString(LPCWSTR szKey, LPCWSTR szEnt, CString v);
public:
	static bool CheckConfigVer(LPCWSTR path = 0);
	static void PurgeRegConfig(LPCTSTR key = 0);
};

class CVibraimageExProp :
	public CPropertiesWndProp, public CNewVarEventVI
{
public:
	CVibraimageExProp(CPropertiesPack *pPack,int iid);
	~CVibraimageExProp();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnNewVar(int id, int subID);

	virtual void UpdateColor(ITEM *pItem);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	virtual ITEM* AddItem(ITEM* pGroup, xml_node& xml);
	virtual void OnChangeSelection(ITEM *pItem);

	virtual bool CanEdit(CMFCPropertyGridProperty * pItm);
	virtual bool OnDblClk(ITEM *pItem);
	virtual bool OnClickValue(ITEM *pItem);

	virtual COLORREF GetWndColor(int id);
};

