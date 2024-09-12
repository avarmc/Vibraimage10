
#pragma once

#include "PropertiesWnd.h"

namespace CDockablePaneVI
{

	class CPropertiesWndProp;
	class CPropertiesPack;
	/*
	class CPropertiesToolBar : public CMFCToolBar
	{
	public:
		virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
		{
			CMFCToolBar::OnUpdateCmdUI((CFrameWnd*)GetOwner(), bDisableIfNoHndler);
		}

		virtual BOOL AllowShowOnList() const { return FALSE; }
	};
	*/


	class CPropertiesWndProp : public CPropertiesWnd
	{
		// Construction
	public:
		CPropertiesWndProp(CPropertiesPack *pPack, int iid);

		virtual void AdjustLayout();
		virtual void AdjustLayoutFn();

		// Attributes
	public:
		void SetVSDotNetLook(BOOL bSet)
		{
			m_wndPropList.SetVSDotNetLook(bSet);
			m_wndPropList.SetGroupNameFullWidth(bSet);
		}

		//	CComboBox m_wndObjectCombo;
		//	CPropertiesToolBar m_wndToolBar;
	public:
		enum TIMER { T_UPDATE = 100, T_LAYOUT };


		class ITEM_PROP : public CMFCPropertyGridProperty
		{
			ITEM *	m_pItem;
		public:
			// Group constructor
			ITEM_PROP(ITEM *pItem, const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE);

			// Simple property
			ITEM_PROP(ITEM *pItem, const CString& strName, const COleVariant& varValue, LPCWSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
				LPCWSTR lpszEditMask = NULL, LPCWSTR lpszEditTemplate = NULL, LPCWSTR lpszValidChars = NULL);
		public:
			CRect& InternalGetRect() { return m_Rect; }
			BOOL&  InternalGetValueList() { return m_bIsValueList; }
			CRect& InternalGetRectButton() { return m_rectButton; }
			CList<CMFCPropertyGridProperty*, CMFCPropertyGridProperty*>& InternalGetSubItems() { return m_lstSubItems; }
			virtual BOOL HasValueField() const { return CMFCPropertyGridProperty::HasValueField(); }

			virtual BOOL HasButton() const { return CMFCPropertyGridProperty::HasButton() || (m_pItem && !m_pItem->button.IsEmpty()); }

			virtual BOOL IsSelectedEx() const;
		public:

			virtual ~ITEM_PROP() {}

			virtual CString FormatProperty();

			virtual void OnDrawName(CDC* pDC, CRect rect);
			virtual void OnDrawValue(CDC* pDC, CRect rect);
			virtual void OnDrawButton(CDC* pDC, CRect rect);
			virtual void OnClickButton(CPoint point);
			virtual BOOL OnDblClk(CPoint point);
			virtual BOOL OnClickValue(UINT uiMsg,CPoint point);
			virtual void OnClickName(CPoint C);
		};
	protected:
		class CTRL : public CMFCPropertyGridCtrl
		{
			friend class ITEM_PROP;

			CPropertiesWndProp *pBase;
		public:
			ITEM *  m_currentEdit,*m_pPrevEdit = 0;
			LONG	m_nEdit;
		public:

			CTRL(CPropertiesWndProp *pb) :pBase(pb), m_currentEdit(0), m_pPrevEdit(0), m_nEdit(0) {
			}
			virtual ~CTRL() {}

			virtual BOOL EditNext(int pos);
			virtual BOOL EditItem(CMFCPropertyGridProperty* pProp, LPPOINT lptClick = NULL);
			virtual BOOL EndEditItem(BOOL bUpdateData = TRUE);
			virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
			virtual int CompareProps(const CMFCPropertyGridProperty* pProp1, const CMFCPropertyGridProperty* pProp2) const;
			virtual int OnDrawProperty(CDC* pDC, CMFCPropertyGridProperty* pProp) const;
			void ReposProperties() { CMFCPropertyGridCtrl::ReposProperties(); }

			virtual void OnChangeSelection(CMFCPropertyGridProperty* pNewSel, CMFCPropertyGridProperty* pOldSel)
			{
				if (pNewSel)
					pBase->OnChangeSelection((ITEM*)pNewSel->GetData());
				else
					pBase->OnChangeSelection(0);
			}

		};


		CStringA		m_sortSwitch;
		ITEM *			m_sortItem;
		ULONGLONG		m_sortTimer;
		bool			m_sortDESC;
		bool			m_sortABS;

		CStringA		m_idOnButton;
		CStringA		m_idSelected;

		CTRL			m_wndPropList;
		std::vector< std::shared_ptr< CButton > > m_wndButtons;

		// Implementation
	public:
		virtual ~CPropertiesWndProp();

		virtual bool Load(xml_node& xml);

		virtual  ITEM *CreateItem(CPropertiesWnd *pb, ITEM* pGroup, const CStringA& type, xml_node& xml);
		virtual  ITEM *CreateItem(CPropertiesWnd *pb, ITEM* pGroup, xml_node& xml);

		virtual ITEM* AddItem(ITEM* pGroup, xml_node& xml);

		virtual LRESULT OnPropertyChanged(__in WPARAM wparam, __in LPARAM lparam);

		virtual void OnChangeSelection(ITEM *pItem);
		virtual bool OnClickButton(ITEM *pItem);
		virtual BOOL IsSelectedEx(ITEM *pItem);

		virtual BOOL ValidateItemData(CMFCPropertyGridProperty* pProp);


		virtual bool UpdateValue(ITEM *pItem, bool bPush, bool bLock = true);
		virtual void UpdateColor(ITEM *pItem);

		virtual void RemoveAllItems();

		virtual bool Reload();

	protected:
		DECLARE_MESSAGE_MAP()
	protected:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnExpandAllProperties();
		afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
		afx_msg void OnSortProperties();
		afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg void OnSettingChange(UINT uFlags, LPCWSTR lpszSection);

		virtual void InitPropList();
		virtual void SetPropListFont();
		virtual void OnUpdateTimer();
	public:
		void RegSave();
		void RegSave(ITEM* pItem);
		afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnDestroy();
};




}