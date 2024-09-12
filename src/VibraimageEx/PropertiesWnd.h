
#pragma once

#include "PropertiesWndBase.h"
#include "vi_colors.h"

namespace CDockablePaneVI
{

	class CPropertiesWnd;
	class CPropertiesPack;



	class CPropertiesWnd : public CPropertiesWndBase
	{
		// Construction
	public:
		CPropertiesWnd(CPropertiesPack *pPack, int iid);


		// Attributes
	public:
		virtual void SetVSDotNetLook(BOOL bSet)
		{
		}

	protected:

		CFont				m_fntPropList;
		xml_document		m_xml;

		//	CComboBox m_wndObjectCombo;
		//	CPropertiesToolBar m_wndToolBar;
	public:

		class ITEM;
		typedef struct tagITEM_VALUE
		{
			ITEM *pItem;

			CStringA					sid;
			int							iid;

			CStringA					sid2;
			int							iid2;

			CString						value;

			CStringA					type;
			int							typeId;
			float						scale;
			CStringA					owner;
			int							ownerId;
			COleVariant					currentValue;

			CString						format;

			std::vector<CString>		swTag;
			std::vector<CString>		swName;
			std::vector<int>			swValue;
		}ITEM_VALUE;

		class ITEM
		{
		public:
#ifdef _DEBUG
			static std::list<ITEM*> gItems;
#endif
			CPropertiesWnd *			pBase;
			DWORD_PTR					pItem;
			ITEM *						pParent;
			std::list<ITEM *>			pChild;

			CStringA					type;

			std::vector<ITEM_VALUE>		values;

			int							nVer;
			int							nList;
			BOOL						hidden;

			bool						bGroup;

			bool						bNeedUpdate;
			int							pos, npos;

			float						progressMin;
			float						progressMax;
			COLORREF					progressClr;

			CStringA					tag;
			CString						name;
			int							nameId;

			CString						button;

			CString						description;
			int							descriptionId;

			bool						edit;

			int							version;

			CCriticalSection			currentValueLock;

			HWND						hEdit;

			int							color_child;

			COLORREF					clrFillName;
			COLORREF					clrFillValue;
			COLORREF					clrTextName;
			COLORREF					clrTextValue;

			bool						sorted;

		public:
			ITEM(CPropertiesWnd * pb);
			virtual void Reset();
		public:

			virtual ~ITEM() {
#ifdef _DEBUG
				gItems.remove(this);
#endif			
			}
		};


		LONG						m_nUpdate;
		CStringA					m_baseId;

		std::multimap< int, ITEM* >	m_items;

		CStringA					m_xmlTmp, m_xmlTmpTag;
		int							m_nDescription;
		int							m_nComboHeight;
		// Implementation
	public:
		virtual ~CPropertiesWnd();

		virtual bool Load(xml_node& xml);
		virtual bool Reload();

		virtual CString FormatProperty(const COleVariant& v, const CString& format);

	protected:

		virtual void LoadAttributes(ITEM_VALUE& v, xml_node& xml);
		virtual void LoadValues(ITEM* pItem, xml_node& xml);

		virtual int GetTypeID(const CStringA& type);

		virtual  ITEM * CreateItem(CPropertiesWnd *pb, ITEM* pGroup, const CStringA& type, xml_node& xml);
		virtual  ITEM  *CreateItem(CPropertiesWnd *pb, ITEM* pGroup, xml_node& xml);

		virtual ITEM* AddItem(ITEM* pGroup, xml_node& xml);

		virtual void UpdateAll() { SetUpdateAll(); }

		virtual ITEM* GetItemByPos(int pos);
		virtual ITEM* GetItem(int id, int subID);
		virtual int SetUpdate(int id, int subID);
		virtual int SetUpdateAll();
		virtual CTabbedPane* CreateTabbedPane();

		virtual void RemoveAllItems();

		virtual bool UpdateValue(ITEM *pItem, bool bPush, bool bLock = true) { return true; }

	protected:
		DECLARE_MESSAGE_MAP()
	protected:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg void OnSettingChange(UINT uFlags, LPCWSTR lpszSection);
		afx_msg void OnClose();
		afx_msg void OnDestroy();

		virtual void InitPropList();
		virtual void SetPropListFont();

		virtual void OnAfterDock(CBasePane* pBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod);
	public:
		virtual bool OnClickButton(ITEM *pItem) { return false; }
		virtual bool OnDblClk(ITEM *pItem) { return false; }
		virtual bool OnClickValue(ITEM *pItem) { return false; }
		virtual bool OnClickName(ITEM *pItem) { return false; }

		virtual BOOL IsSelectedEx(ITEM *pItem) { return FALSE; }
		virtual void OnUpdateTimer();
		afx_msg void OnTimer(UINT_PTR nIDEvent);
		void RegSave();
		void RegSave(ITEM* pItem);
	};


}