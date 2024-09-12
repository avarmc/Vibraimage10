#pragma once

#include <Wincrypt.h>
#include "PropertiesWnd.h"

namespace CDockablePaneVI
{

	class CPropertiesWndBase;

	class CPropertiesPack
	{
	public:
		typedef CPropertiesWnd::ITEM_VALUE ITEM_VALUE;
		typedef CPropertiesWnd::ITEM ITEM;

		typedef struct tagPANE
		{
			CPropertiesWndBase *		pWnd;
			int					iid;
			CStringA			sid;
			CStringA			title;
			CStringA			type;
			CStringA			align;
			CStringA			tab;
			HICON				iconHandle;
			CString				iconPath;
			CSize				sizeDefault;
			float				xscroll;
			CStringA			show;
		} PANE;

	protected:
		CString					m_path;
		std::map<int, PANE>		m_wnd;
		std::list<PANE *>		m_wndList;
		CFrameWndEx *			m_pFrame;

		std::map<CStringA, CDockablePane* > m_wndTabs;
		
		

	public:
		CPropertiesPack(CFrameWndEx *pFrame);
		virtual ~CPropertiesPack();

		virtual void RemoveAll();

		CString getPath() { return m_path; }
		virtual bool CanLoadState() { return true; }
		virtual bool CanSaveState() { return true; }

		virtual bool Reload();

		virtual CString GetConfigBase() { return _T("config"); }
		

		virtual bool Load(LPCWSTR path = 0);
		virtual bool LoadXml(LPCWSTR path, int depth = 0);

		virtual CDockablePane * CreatePane(xml_node& xml);
		virtual CDockablePane * CreatePaneProperty(xml_node& xml, PANE& pane);

		virtual void DockPanes();
		virtual bool DockPane(PANE& pane) { return false; }

		virtual CPropertiesWndBase* Find(CStringA sid);
		virtual CPropertiesWndBase * CreateWnd(const CStringA& type, int iid);
		virtual int GetOwnerId(const CStringA& sid) { return 0; }

		virtual bool SetValue(ITEM *pItem) { return false; }
		virtual bool GetValue(ITEM *pItem, VARTYPE vt) { return false; }

		virtual int GetValueSelect(ITEM_VALUE *pItem, LPCWSTR str);
		virtual CString GetValueSelect(ITEM_VALUE *pItem, int id);


		virtual bool SetValue(ITEM_VALUE *pItem) { return false; }
		virtual bool GetValue(ITEM_VALUE *pItem, VARTYPE vt) { return false; }
		virtual COleVariant ReScale(ITEM_VALUE *pItem, const COleVariant& a, bool bDiv);

		virtual CString GetDefaultFormat(int typeID);

		virtual int Tag2Id(const CString& sid) { return 0; }

		virtual CSize GetDefaultSize();

		virtual LPCWSTR	GetLang(int iid, LPCWSTR def = 0) { return def; }
		virtual LPCWSTR	GetLang(LPCWSTR sid, LPCWSTR def = 0) { return def; }
		virtual int		GetLangId(LPCWSTR sid) { return -1; }

		virtual int GetProfileInt(LPCWSTR szKey, LPCWSTR szEnt, int def = 0) { return def; }
		virtual void WriteProfileInt(LPCWSTR szKey, LPCWSTR szEnt, int v) {}

		virtual CString GetProfileString(LPCWSTR szKey, LPCWSTR szEnt, CString def = _T("")) { return def; }
		virtual void WriteProfileString(LPCWSTR szKey, LPCWSTR szEnt, CString v) {}

		virtual CDockablePane* GetTab(LPCSTR tab);

		virtual void UpdateAll();
	protected:
		CString attrib(xml_node& xml, LPCSTR name);
		CString GetFilePath(const CString& path);
	protected:
		static bool CryptInit(HCRYPTPROV* pc);
		static CStringA ToHex(BYTE* data, int len);
	public:
		void ShowAll(BOOL bShow);
		void RegSave();
		void OnInit();
		void RecalcLayout();
		void AdjustLayout();

		void AdjustVisible();

		PANE* GetInfo(int iid);
	};

	inline CString CPropertiesPack::attrib(xml_node& xml, LPCSTR name)
	{
		xml_attribute a = xml.attribute(name);
		if (a == 0)
			return CString();
		return a.wvalue().c_str();
	}

	inline CString CPropertiesPack::GetFilePath(const CString& path)
	{
		int pos;
		if ((pos = path.ReverseFind('\\')) < 0) return _T("");
		return path.Left(pos + 1);
	}
}
