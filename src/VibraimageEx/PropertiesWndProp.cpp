
#include "stdafx.h"
#include "VibraimageEx.h"
#include "PropertiesWnd.h"
#include "PropertiesWndProp.h"
#include "Resource.h"
#include "VibraimageExFrame.h"
#include "VibraimageEx.h"
#include <sstream>
#include "estring.h"
#include "vi_colors.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define visualManager CMFCVisualManager::GetInstance()

namespace CDockablePaneVI
{

	/////////////////////////////////////////////////////////////////////////////
	// CResourceViewBar

	CPropertiesWndProp::CPropertiesWndProp(CPropertiesPack *pPack, int iid) : CPropertiesWnd(pPack, iid), m_wndPropList(this)
	{
		m_nComboHeight = 0;
		m_nUpdate = 0;
		m_nDescription = 0;
		m_sortItem = 0;
		m_sortTimer = 0;
		m_sortDESC = false;
	}

	CPropertiesWndProp::~CPropertiesWndProp()
	{

	}


	BEGIN_MESSAGE_MAP(CPropertiesWndProp, CPropertiesWnd)
		ON_WM_CREATE()
		ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
		ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
		ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
		ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
		ON_WM_SETFOCUS()
		ON_WM_SETTINGCHANGE()
		ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)

		ON_COMMAND(ID_COMMAND_REG_SAVE, RegSave)
		ON_WM_HSCROLL()
		ON_WM_DESTROY()
	END_MESSAGE_MAP()



	// Group constructor
	CPropertiesWndProp::ITEM_PROP::ITEM_PROP(ITEM *pItem, const CString& strGroupName, DWORD_PTR dwData, BOOL bIsValueList)
		: CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList), m_pItem(pItem)
	{
	}

	// Simple property
	CPropertiesWndProp::ITEM_PROP::ITEM_PROP(ITEM *pItem, const CString& strName, const COleVariant& varValue, LPCWSTR lpszDescr, DWORD_PTR dwData,
		LPCWSTR lpszEditMask, LPCWSTR lpszEditTemplate, LPCWSTR lpszValidChars)
		: CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars), m_pItem(pItem)
	{
	}

	BOOL CPropertiesWndProp::ITEM_PROP::IsSelectedEx() const
	{ 
		BOOL sel = FALSE;
		if (m_pItem)
			sel = m_pItem->pBase->IsSelectedEx(m_pItem);
		return sel;
	}


	CString CPropertiesWndProp::ITEM_PROP::FormatProperty()
	{
		ITEM *pItem = (CPropertiesWnd::ITEM *)GetData();

		if (m_bIsValueList)
		{
			CString strVal = _T("");
			std::list<ITEM *>::iterator i, ie;
			int n = 0, nMax = pItem->nList ? pItem->nList : 100;
			for (i = m_pItem->pChild.begin(), ie = m_pItem->pChild.end(); i != ie && n < nMax; ++i, ++n)
			{
				ITEM_PROP *pc = (ITEM_PROP*)((*i)->pItem);
				if (pc)
				{
					if (!n)
					{
						pItem->clrFillValue = (*i)->clrFillValue;
					}
					if (!strVal.IsEmpty())
						strVal += _T("  ");
					strVal += pc->FormatProperty();
				}

			}
			return strVal;
		}

		if (!pItem || pItem->values[0].format.IsEmpty() || pItem->values[0].type == "bool" || pItem->values[0].type == "select")
			return CMFCPropertyGridProperty::FormatProperty();

		return pItem->pBase->FormatProperty(GetValue(), pItem->values[0].format);
	}



	void CPropertiesWndProp::ITEM_PROP::OnDrawName(CDC* pDC, CRect rect)
	{

		ASSERT_VALID(this);
		ASSERT_VALID(pDC);
		ASSERT_VALID(m_pWndList);

		CPropertiesWndProp::CTRL *pWndList = static_cast<CPropertiesWndProp::CTRL*>(m_pWndList);

		if (pWndList->pBase->m_nLoad)
			return;
		COLORREF clrTextOld = pDC->SetTextColor(m_pItem->clrTextName);

		CRect rectFill = rect;
		rectFill.top++;

		if (IsSelectedEx())
		{
			pDC->FillRect(rectFill, m_pItem->pBase->GetWndBrush(CLR_ActiveCaption) );
		}
		else
		if (IsSelected() && pWndList->m_bFocused)
			pDC->FillRect(rectFill, m_pItem->pBase->GetWndBrush(CLR_Hilite) );
		else
			pDC->FillSolidRect(rectFill, /* m_pItem->pBase->GetWndColor(CLR_FillName) */ m_pItem->clrFillName);

		if (m_pParent != NULL && static_cast<CPropertiesWndProp::ITEM_PROP*>(m_pParent)->m_bIsValueList)
		{
			rect.left += rect.Height();
		}

		rect.DeflateRect(AFX_TEXT_MARGIN, 0);

		int nTextHeight = pDC->DrawText(m_strName, rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);

		m_bNameIsTruncated = pDC->GetTextExtent(m_strName).cx > rect.Width();

		if (IsSelected() && pWndList->m_bVSDotNetLook && IsGroup() && !m_bIsValueList)
		{
			CRect rectFocus = rect;
			rectFocus.top = rectFocus.CenterPoint().y - nTextHeight / 2;
			rectFocus.bottom = rectFocus.top + nTextHeight;
			rectFocus.right = min(rect.right, rectFocus.left + pDC->GetTextExtent(m_strName).cx);
			rectFocus.InflateRect(2, 0);

			COLORREF clrShadow = pWndList->m_bControlBarColors ? m_pItem->pBase->GetWndColor(CLR_BarShadow) : m_pItem->pBase->GetWndColor(CLR_BtnShadow);

			pDC->Draw3dRect(rectFocus, clrShadow, clrShadow);
		}

		if (clrTextOld != (COLORREF)-1)
		{
			pDC->SetTextColor(clrTextOld);
		}
	}

	void CPropertiesWndProp::ITEM_PROP::OnDrawValue(CDC* pDC, CRect rect)
	{
		ASSERT_VALID(this);
		ASSERT_VALID(pDC);
		ASSERT_VALID(m_pWndList);

		CPropertiesWndProp::CTRL *pWndList = static_cast<CPropertiesWndProp::CTRL*>(m_pWndList);
		if (pWndList->pBase->m_nLoad)
			return;

		if ((IsGroup() && !m_bIsValueList) || !HasValueField())
		{
			return;
		}

		CFont* pOldFont = NULL;
		if (IsModified() && pWndList->m_bMarkModifiedProperties)
		{
			pOldFont = pDC->SelectObject(&pWndList->m_fontBold);
		}


		CString strVal = FormatProperty();

		rect.DeflateRect(AFX_TEXT_MARGIN, 0);

		if (m_pItem->progressMin == m_pItem->progressMax)
			pDC->FillSolidRect(&rect, m_pItem->clrFillValue);
		else
		{
			CRect r1(rect), r2(rect);
			float v = 0;
			if (m_pItem->values[0].currentValue.vt == VT_R4)
				v = m_pItem->values[0].currentValue.fltVal;
			else
				if (m_pItem->values[0].currentValue.vt == VT_R8)
					v = (float)m_pItem->values[0].currentValue.dblVal;

			float dv = (v - m_pItem->progressMin) / (m_pItem->progressMax - m_pItem->progressMin);
			if (dv < 0) dv = 0;
			if (dv > 1) dv = 1;
			r1.right = r2.left = r1.left + (int)(r1.Width()*dv);
			pDC->FillSolidRect(&r1, m_pItem->progressClr);
			pDC->FillSolidRect(&r2, m_pItem->clrFillValue);
		}

		COLORREF clrTextOld = pDC->SetTextColor(m_pItem->clrTextValue);

		pDC->DrawText(strVal, rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);

		m_bValueIsTruncated = pDC->GetTextExtent(strVal).cx > rect.Width();

		if (clrTextOld != (COLORREF)-1)
		{
			pDC->SetTextColor(clrTextOld);
		}

		if (pOldFont != NULL)
		{
			pDC->SelectObject(pOldFont);
		}
	}


	void CPropertiesWndProp::ITEM_PROP::OnDrawButton(CDC* pDC, CRect rect)
	{
		CMFCPropertyGridProperty::OnDrawButton(pDC, rect);
	}

	void CPropertiesWndProp::ITEM_PROP::OnClickButton(CPoint point)
	{
		if (!m_pItem)
			return;
		if (!m_pItem->pBase->OnClickButton(m_pItem))
			CMFCPropertyGridProperty::OnClickButton(point);
	}

	BOOL CPropertiesWndProp::ITEM_PROP::OnDblClk(CPoint point) {
		if (m_pItem) {
			if (m_pItem->pBase->OnDblClk(m_pItem))
				return TRUE;
		}
		return CMFCPropertyGridProperty::OnDblClk(point);
	}

	BOOL CPropertiesWndProp::ITEM_PROP::OnClickValue(UINT uiMsg, CPoint point) {
		if (m_pItem) {
			if (m_pItem->pBase->OnClickValue(m_pItem))
				return TRUE;
		}
		return CMFCPropertyGridProperty::OnClickValue(uiMsg,point);
	}

	void CPropertiesWndProp::ITEM_PROP::OnClickName(CPoint C) {
		if (m_pItem) {
			if (m_pItem->pBase->OnClickName(m_pItem))
				return;
		}
		return CMFCPropertyGridProperty::OnClickName(C);
	}

	/////////////////////////////////////////////////////////////////////////////
	// CResourceViewBar message handlers
	void CPropertiesWndProp::AdjustLayout()
	{
		if (m_hWnd)
		{
			CPropertiesWnd::AdjustLayout();
		}
	}

	void CPropertiesWndProp::AdjustLayoutFn()
	{
		CPropertiesWnd::AdjustLayoutFn();

		if (GetSafeHwnd() == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
		{
			return;
		}
		int cyTlb = 0, hButtons = 0;
		for (std::vector< std::shared_ptr< CButton > >::iterator i = m_wndButtons.begin(); i != m_wndButtons.end(); ++i) {
			if (*i && (*i)->m_hWnd) {
				CRect rb;
				(*i)->GetWindowRect(&rb);
				hButtons += rb.Height();
			}
		}

		if (IsWindow(m_wndPropList))
		{
			CRect rectClient, rScroll;
			GetClientRect(rectClient);
			rScroll = rectClient;



			int xScroll = 0;
			CScrollBar *hScroll = 0;
			CPropertiesPack::PANE *pInfo = m_pPack->GetInfo(m_iid);
			if (pInfo)
			{
				if (pInfo->xscroll > 1)
				{
					// FixIt
			//		EnableScrollBarCtrl(SB_HORZ, ESB_ENABLE_BOTH);
			//		hScroll = m_wndPropList.GetScrollBarCtrl(SB_VERT);
				}

				if (pInfo->xscroll > 1)
				{
					int rw = rectClient.Width();
					int mx = (int)(rw*(pInfo->xscroll - 1));
					if (xScroll > mx)
						xScroll = mx;
					SetScrollRange(SB_HORZ, 0, mx);
					xScroll = GetScrollPos(SB_HORZ);
					rectClient.right = rectClient.left + (int)(rw*pInfo->xscroll);
				}
			}


			CRect rProp(CPoint(rectClient.left - xScroll, rectClient.top + m_nComboHeight+hButtons + cyTlb), CSize(rectClient.Width(), rectClient.Height() - (m_nComboHeight + hButtons + cyTlb)));
			m_wndPropList.SetWindowPos(NULL, rProp.left, rProp.top, rProp.Width(), rProp.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

			int h0 = rectClient.top;
			for (std::vector< std::shared_ptr< CButton > >::iterator i = m_wndButtons.begin(); i != m_wndButtons.end(); ++i) {
				if (*i && (*i)->m_hWnd) {
					CRect rb;
					(*i)->GetWindowRect(&rb);
					(*i)->SetParent(this);
					(*i)->MoveWindow(rProp.left, h0, rProp.Width(), rb.Height());
					h0 += rb.Height();
					(*i)->SetFont(&m_fntPropList);
				}
			}

			if (pInfo && hScroll && pInfo->xscroll > 1)
			{

				//	m_wndPropList.GetClientRect(&rScroll);
				rScroll.right = rectClient.right/* + xScroll */;
				rScroll.left = rScroll.right - GetSystemMetrics(SM_CXVSCROLL);
				hScroll->SetWindowPos(0, rScroll.left, rScroll.top, rScroll.Width(), rScroll.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			}
		}
	}



	int CPropertiesWndProp::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDockablePane::OnCreate(lpCreateStruct) == -1)
			return -1;

		CRect rectDummy;
		rectDummy.SetRectEmpty();

		if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
		{
			TRACE0("Failed to create Properties Grid \n");
			return -1;      // fail to create
		}


		InitPropList();

		AdjustLayoutFn();
		AdjustLayout();

		SetTimer(T_UPDATE, 40, 0);

		return 0;
	}


	void CPropertiesWndProp::OnExpandAllProperties()
	{
		m_wndPropList.ExpandAll();
	}

	void CPropertiesWndProp::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
	{
	}

	void CPropertiesWndProp::OnSortProperties()
	{
		m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
	}

	void CPropertiesWndProp::OnUpdateSortProperties(CCmdUI* pCmdUI)
	{
		pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
	}


	bool CPropertiesWndProp::Load(xml_node& xml)
	{
	//	__super::Load(xml);
		CInc incLoad(&m_nLoad);

		m_sortSwitch = xml.attribute_value("sort_switch");

		m_sortDESC = (CStringA(xml.attribute_value("sort_mode")).CompareNoCase("DESC") == 0);
		m_sortABS = (CStringA(xml.attribute_value("sort_abs")).CompareNoCase("true") == 0);
		m_idOnButton = xml.attribute_value("onbutton");
		m_idSelected = xml.attribute_value("selected");

		if (!CPropertiesWnd::Load(xml))
			return false;

		m_wndPropList.EnableDescriptionArea(!!m_nDescription);

		// create buttons
		if (m_wndButtons.empty()) {
			for (xml_node_iterator i = m_xml.first_child().begin(); i != m_xml.first_child().end(); ++i) {
				const char* name = i->name();
				if (strcmp(name, "button") == 0) {
					m_wndButtons.push_back(std::make_shared<CButton>());
					m_wndButtons.back()->Create(GetLang(i->attribute_wvalue("title").c_str()), WS_CHILD | WS_VISIBLE, CRect(0, 0, 10, atoi(i->attribute_value("height"))), this, m_pPack->Tag2Id(CString(i->attribute_value("id"))));
				}
			}
		}
		return true;
	}


	CPropertiesWnd::ITEM* CPropertiesWndProp::AddItem(ITEM* pGroup, xml_node& xml)
	{
		CStringA type = xml.attribute_value("type");

		ITEM *pItem = CPropertiesWnd::AddItem(pGroup, xml);
		ITEM *pItemGr = pGroup;

		CMFCPropertyGridProperty * pGr = pItemGr ? (CMFCPropertyGridProperty *)pItemGr->pItem : 0;

		if (pItem)
		{
			if (!m_sortSwitch.IsEmpty() && pItem->values[0].sid == m_sortSwitch)
				m_sortItem = pItem;

			CMFCPropertyGridProperty *pItemItem = (CMFCPropertyGridProperty *)pItem->pItem;

			if (pItemItem->GetValue().vt != VT_BOOL)
				pItemItem->AllowEdit(!!pItem->edit);
			if (!pGroup)
				pItem->pos = m_wndPropList.AddProperty(pItemItem);
			else
				pItem->pos = pGr->AddSubItem(pItemItem);

			if (pItemItem->IsGroup())
			{
				CString tag = CString(_T("Property\\") + CString(m_baseId));
				CString sid = pItem->values[0].sid.IsEmpty() ? pItem->name : CString(pItem->values[0].sid);
				const char *strExp = xml.attribute_value("expanded");
				BOOL bExp = pItemItem->IsExpanded();
				if (strExp && strcmp(strExp, "false") == 0)
					bExp = FALSE;
				pItemItem->Expand(m_pPack->GetProfileInt(tag, sid, bExp));
			}

			if (pItem->values[0].type == "select")
			{
				for (size_t i = 0; i < pItem->values[0].swName.size(); ++i)
					pItemItem->AddOption(pItem->values[0].swName[i]);
				pItemItem->AllowEdit(FALSE);
			}

			pItemItem->Show(!pItem->hidden);
		}

		UpdateColor(pItem);

		return pItem;
	}

	void CPropertiesWndProp::InitPropList()
	{
		SetPropListFont();

		m_wndPropList.EnableHeaderCtrl(FALSE);
		m_wndPropList.SetVSDotNetLook();
		m_wndPropList.MarkModifiedProperties();
		m_wndPropList.SetBoolLabels(GetLang(_T("BUTTON_YES")), GetLang(_T("BUTTON_NO")));

	}

	void CPropertiesWndProp::OnSetFocus(CWnd* pOldWnd)
	{
		CDockablePane::OnSetFocus(pOldWnd);
		m_wndPropList.SetFocus();
	}

	void CPropertiesWndProp::OnSettingChange(UINT uFlags, LPCWSTR lpszSection)
	{
		CDockablePane::OnSettingChange(uFlags, lpszSection);
		SetPropListFont();
	}


	LRESULT CPropertiesWndProp::OnPropertyChanged(__in WPARAM wparam, __in LPARAM lparam)
	{
		// Parameters:
		// [in] wparam: the control ID of the CMFCPropertyGridCtrl that changed.
		// [in] lparam: pointer to the CMFCPropertyGridProperty that changed.

		// Cast the lparam to a property.
		CMFCPropertyGridProperty * pp = (CMFCPropertyGridProperty *)lparam;
		ITEM *pItem = (ITEM *)pp->GetData();

		UpdateValue(pItem, true);


		return 0;
	}

	BOOL CPropertiesWndProp::ValidateItemData(CMFCPropertyGridProperty* pProp)
	{
		return TRUE;
	}

	void CPropertiesWndProp::OnChangeSelection(ITEM *pItem)
	{

	}

	BOOL CPropertiesWndProp::IsSelectedEx(ITEM *pItem)
	{
		if (m_idSelected.IsEmpty() || pItem->values.empty())
			return FALSE;

		std::vector<CStringA> arr = explode(m_idOnButton, CStringA(":"));
		if (arr.size() != 3)
			return FALSE;

		ITEM_VALUE v;
		v.pItem = pItem;

		xml_document doc;
		xml_node xml = doc.append_child();
		xml.set_name("item");
		xml.append_attribute("id").set_value(arr[2]);
		xml.append_attribute("type").set_value("int");
		xml.append_attribute("src").set_value(arr[0]);

		LoadAttributes(v, xml);
		v.currentValue = COleVariant((long)-1, VT_INT);

		if(!m_pPack->GetValue(&v, VT_INT))
			return FALSE;

		if(arr[1] == "2")
			return v.currentValue.lVal == pItem->values[0].iid2;
		return v.currentValue.lVal == pItem->values[0].iid;
	}

	bool CPropertiesWndProp::OnClickButton(ITEM *pItem)
	{
		if (m_idOnButton.IsEmpty() || pItem->type=="bool")
			return false;
		std::vector<CStringA> arr = explode(m_idOnButton, CStringA(":"));
		if (arr.size() != 3)
			return false;

		ITEM_VALUE v;
		v.pItem = pItem;

		xml_document doc;
		xml_node xml = doc.append_child();
		xml.set_name("item");
		xml.append_attribute("id").set_value(arr[2]);
		xml.append_attribute("type").set_value("int");
		xml.append_attribute("src").set_value(arr[0]);

		LoadAttributes(v, xml);
		if(arr[1] == "2")
			v.currentValue = COleVariant((long)pItem->values[0].iid2, VT_INT);
		else
			v.currentValue = COleVariant((long)pItem->values[0].iid, VT_INT);

		m_pPack->SetValue(&v);

		SetUpdateAll();

		return true;
	}

	int CPropertiesWndProp::CTRL::OnDrawProperty(CDC* pDC, CMFCPropertyGridProperty* pProp) const
	{
		ASSERT_VALID(this);
		ASSERT_VALID(pDC);
		ASSERT_VALID(pProp);
		ITEM_PROP * pPropX = static_cast<ITEM_PROP*>(pProp);

		CRect& mRect = pPropX->InternalGetRect();
		CRect& mrectButton = pPropX->InternalGetRectButton();
		BOOL& bIsValueList = pPropX->InternalGetValueList();


		if (!mRect.IsRectEmpty())
		{
			if (mRect.top >= m_rectList.bottom)
			{
				return FALSE;
			}

			if (mRect.bottom >= m_rectList.top)
			{
				const int nXCenter = m_rectList.left + m_nLeftColumnWidth;
				COLORREF clrTextOld = (COLORREF)-1;

				if (m_bVSDotNetLook)
				{
					CRect rectLeft = mRect;

					if (!pProp->IsGroup())
					{
						rectLeft.right = min(nXCenter, rectLeft.left);
					}

					if (bIsValueList)
					{
						rectLeft.right = rectLeft.left + rectLeft.Height();
					}

					rectLeft.left = m_rectList.left;
					rectLeft.bottom = min(rectLeft.bottom, m_rectList.bottom);

					if (rectLeft.left < rectLeft.right)
					{
						CBrush br(m_clrGroupBackground != (COLORREF)-1 ? m_clrGroupBackground : m_clrGray);
					}
				}

				if (!pProp->IsEnabled())
				{
					clrTextOld = pDC->SetTextColor(pBase->GetWndColor(CLR_GrayedText) );
				}

				CRect rectName = mRect;

				if ((!pProp->IsGroup() || bIsValueList || !m_bGroupNameFullWidth) && pPropX->HasValueField())
				{
					rectName.right = nXCenter;
				}

				if (pProp->IsGroup())
				{
					if (m_bGroupNameFullWidth && !m_bVSDotNetLook && !bIsValueList)
					{
						CRect rectFill = rectName;
						rectFill.top++;

						if (m_brBackground.GetSafeHandle() != NULL)
						{
							const CBrush& br = m_brBackground;
							pDC->FillRect(rectFill, (CBrush *)&br);
						}
						else
						{
							pDC->FillRect(rectFill, pBase->GetWndBrush(CLR_Window) );
						}
					}

					CRect rectExpand = rectName;
					rectName.left += m_nRowHeight;
					rectExpand.right = rectName.left;

					CRgn rgnClipExpand;
					CRect rectExpandClip = rectExpand;
					rectExpandClip.bottom = min(rectExpandClip.bottom, m_rectList.bottom);

					rgnClipExpand.CreateRectRgnIndirect(&rectExpandClip);
					pDC->SelectClipRgn(&rgnClipExpand);

					pProp->OnDrawExpandBox(pDC, rectExpand);
				}
				else if (!pPropX->HasValueField())
				{
					CRect rectFill = rectName;
					rectFill.top++;

					if (m_brBackground.GetSafeHandle() != NULL)
					{
						const CBrush& br = m_brBackground;
						pDC->FillRect(rectFill, (CBrush *)&br);
					}
					else
					{
						pDC->FillRect(rectFill, pBase->GetWndBrush(CLR_Window) );
					}
				}

				if (rectName.right > rectName.left)
				{
					CRgn rgnClipName;
					CRect rectNameClip = rectName;
					rectNameClip.bottom = min(rectNameClip.bottom, m_rectList.bottom);

					rgnClipName.CreateRectRgnIndirect(&rectNameClip);
					pDC->SelectClipRgn(&rgnClipName);

					HFONT hOldFont = NULL;
					if (pProp->IsGroup() /* AVA && !bIsValueList */)
					{
						hOldFont = (HFONT) ::SelectObject(pDC->GetSafeHdc(), m_fontBold.GetSafeHandle());
					}

					pProp->OnDrawName(pDC, rectName);

					if (hOldFont != NULL)
					{
						::SelectObject(pDC->GetSafeHdc(), hOldFont);
					}
				}

				CRect rectValue = mRect;
				rectValue.left = nXCenter + 1;

				CRgn rgnClipVal;
				CRect rectValClip = rectValue;
				rectValClip.bottom = min(rectValClip.bottom, m_rectList.bottom);

				rgnClipVal.CreateRectRgnIndirect(&rectValClip);
				pDC->SelectClipRgn(&rgnClipVal);

				pProp->OnDrawValue(pDC, rectValue);

				if (!mrectButton.IsRectEmpty())
				{
					pProp->OnDrawButton(pDC, mrectButton);
				}

				pDC->SelectClipRgn(NULL);

				pDC->MoveTo(m_rectList.left, mRect.bottom);
				pDC->LineTo(m_rectList.right, mRect.bottom);

				if (clrTextOld != (COLORREF)-1)
				{
					pDC->SetTextColor(clrTextOld);
				}
			}
		}

		if (pProp->IsExpanded() || m_bAlphabeticMode)
		{
			CList<CMFCPropertyGridProperty*, CMFCPropertyGridProperty*>& lstSubItems = pPropX->InternalGetSubItems();
			for (POSITION pos = lstSubItems.GetHeadPosition(); pos != NULL;)
			{
				if (!OnDrawProperty(pDC, lstSubItems.GetNext(pos)))
				{
					return FALSE;
				}
			}
		}

		return TRUE;
	}

	void CPropertiesWndProp::RegSave()
	{
		if (!IsWindow(*this))
			return;

		int cnt = m_wndPropList.GetPropertyCount();
		for (int i = 0; i < cnt; ++i)
		{
			ITEM *pItem = (ITEM *)m_wndPropList.GetProperty(i)->GetData();
			RegSave(pItem);
		}
	}

	void  CPropertiesWndProp::RegSave(ITEM* pItem)
	{
		CMFCPropertyGridProperty *pItemItem = (CMFCPropertyGridProperty *)pItem->pItem;

		if (pItemItem && pItemItem->IsGroup())
		{
			CString tag = CString(_T("Property\\") + CString(m_baseId));
			CString sid = pItem->values[0].sid.IsEmpty() ? pItem->name : CString(pItem->values[0].sid);
			m_pPack->WriteProfileInt(tag, sid, pItemItem->IsExpanded());

			int cnt = pItemItem->GetSubItemsCount();
			for (int i = 0; i < cnt; ++i)
			{
				ITEM * ps = (ITEM *)pItemItem->GetSubItem(i)->GetData();
				RegSave(ps);
			}
		}
	}

	int CPropertiesWndProp::CTRL::CompareProps(const CMFCPropertyGridProperty* pProp1, const CMFCPropertyGridProperty* pProp2) const
	{
		ITEM * ps1 = (ITEM *)pProp1->GetData();
		ITEM * ps2 = (ITEM *)pProp2->GetData();

		if (ps1 == ps2)
			return 0;

		if (ps1->sorted != ps2->sorted)
		{
			if (!ps1->sorted)
				return -1;
			return 1;
		}

		if (!ps1->sorted)
			return ps1->npos < ps2->npos ? -1 : 1;

		double v1 = 0, v2 = 0;
		if (ps1->values[0].currentValue.vt == VT_R4)
			v1 = ps1->values[0].currentValue.fltVal;
		else
			if (ps1->values[0].currentValue.vt == VT_R8)
				v1 = ps1->values[0].currentValue.dblVal;

		if (ps2->values[0].currentValue.vt == VT_R4)
			v2 = ps2->values[0].currentValue.fltVal;
		else
			if (ps2->values[0].currentValue.vt == VT_R8)
				v2 = ps2->values[0].currentValue.dblVal;

		if (static_cast<CPropertiesWndProp*>(ps1->pBase)->m_sortDESC)
		{
			if (static_cast<CPropertiesWndProp*>(ps1->pBase)->m_sortABS)
				return fabs(v1) > fabs(v2) ? -1 : 1;
			return (v1) > (v2) ? -1 : 1;
		}

		if (static_cast<CPropertiesWndProp*>(ps1->pBase)->m_sortABS)
			return fabs(v1) < fabs(v2) ? -1 : 1;
		return (v1) < (v2) ? -1 : 1;
	}

	BOOL CPropertiesWndProp::CTRL::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
	{
		BOOL ok = CMFCPropertyGridCtrl::Create(dwStyle, rect, pParentWnd, nID);

		COLORREF clrBackground;
		COLORREF clrText;
		COLORREF clrGroupBackground;
		COLORREF clrGroupText;
		COLORREF clrDescriptionBackground;
		COLORREF clrDescriptionText;
		COLORREF clrLine;

		GetCustomColors(clrBackground, clrText, clrGroupBackground, clrGroupText, clrDescriptionBackground, clrDescriptionText, clrLine);
		clrBackground = pBase->GetWndColor(CLR_Window);

		clrText = pBase->GetWndColor(CLR_TextValue); 
		clrGroupText = pBase->GetWndColor(CLR_TextValue);
		clrDescriptionText = pBase->GetWndColor(CLR_TextValue);

		SetCustomColors(clrBackground, clrText, clrGroupBackground, clrGroupText, clrDescriptionBackground, clrDescriptionText, clrLine);
		return ok;
	}


	CPropertiesWnd::ITEM * CPropertiesWndProp::CreateItem(CPropertiesWnd *pb, ITEM* pGroup, const CStringA& type, xml_node& xml)
	{
		ITEM *pItem = CPropertiesWnd::CreateItem(pb, pGroup, type, xml);

#ifdef _DEBUG
		if (pItem->values[0].iid == VI_VAR_STAT_CFG_SIN)
		{
			int kkkid = 0;
		}
#endif

		if (pItem->type == "" || pItem->type == "group" || pItem->type.Left(5) == "vlist")
		{
			if (pItem->type.Left(5) == "vlist")
			{
				pItem->nList = atoi(pItem->type.Mid(5));

				pItem->pItem = (DWORD_PTR)new ITEM_PROP(pItem, _T(""), 0, TRUE);
			}
			else
				pItem->pItem = (DWORD_PTR)new ITEM_PROP(pItem, _T(""), 0, FALSE);
		}
		else
		if (pItem->type.Left(5) == "float")
		{
			pItem->pItem = (DWORD_PTR)new ITEM_PROP(pItem, CString(), (_variant_t)(double)0, _T(""));
		}
		else
		if (pItem->type.Left(3) == "int")
		{
			int a = 0;
			pItem->pItem = (DWORD_PTR)new ITEM_PROP(pItem, CString(), (_variant_t)a, _T(""));
		}
		else
		if (pItem->type.Left(4) == "bool")
		{
			pItem->pItem = (DWORD_PTR)new ITEM_PROP(pItem, CString(), (_variant_t)false, _T(""));
		}
		else
		if (pItem->type.Left(6) == "select")
		{
			pItem->pItem = (DWORD_PTR)new ITEM_PROP(pItem, CString(), _T(""), _T(""));
		//	pItem->pItem = (DWORD_PTR)new  CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
		}
		else
		if (pItem->type.Left(6) == "file")
		{
			CString ext = xml.attribute_wvalue("ext").c_str();
			CString imask = xml.attribute_wvalue("mask").c_str();
			CString mask = GetLang(imask);
			pItem->pItem = (DWORD_PTR)new  CMFCPropertyGridFileProperty(_T(""), TRUE, _T(""), ext, 0, mask, _T(""));
		}
		else
		if (pItem->type.Left(6) == "path")
		{
			pItem->pItem = (DWORD_PTR)new  CMFCPropertyGridFileProperty(_T(""), _T(""));
		}
		else
		{
			pItem->pItem = (DWORD_PTR)new ITEM_PROP(pItem, CString(), (_variant_t)_T(""), _T(""));
		}

		if (pItem->pItem)
		{
			ITEM_PROP *pItemItem = (ITEM_PROP *)pItem->pItem;

			pItemItem->SetData((DWORD_PTR)pItem);
			if (pItem->edit) {
				pItem->edit = CanEdit(pItemItem);
			}
		}
		return pItem;
	}

	CPropertiesWnd::ITEM * CPropertiesWndProp::CreateItem(CPropertiesWnd *pb, ITEM* pGroup, xml_node& xml)
	{
		ITEM *pItem = CPropertiesWnd::CreateItem(pb, pGroup, xml);

		if (!pItem)
			return 0;

		ITEM_PROP *pItemItem = (ITEM_PROP *)pItem->pItem;

		pItemItem->SetName(GetLang(pItem->nameId, pItem->name));
		pItemItem->SetDescription(GetLang(pItem->descriptionId, pItem->description));

		pItem->bGroup = !!pItemItem->IsGroup();

		return pItem;
	}

	void CPropertiesWndProp::UpdateColor(ITEM *pItem)
	{
		if (pItem)
		{
			ITEM_PROP* pItemItem = (ITEM_PROP*)pItem->pItem;
			pItem->edit = CanEdit(pItemItem);
			
			if (pItem->color_child)
			{
				std::list<ITEM *>::iterator i, ie;
				for (i = pItem->pChild.begin(), ie = pItem->pChild.end(); i != ie; ++i)
				{
					ITEM *pChild = *i;
					if (pChild->values[0].iid == pItem->color_child)
					{
						UpdateColor(pChild);
						pItem->clrFillName = pChild->clrFillName;
						return;
					}
				}
			}

			pItem->clrFillName = (pItem->edit /* || pItem->type == "bool" */ ) ? RGB(224, 255, 224) : GetWndColor(CLR_Window);
			pItem->clrFillValue = GetWndColor(CLR_Window);
		}
	}

	bool CPropertiesWndProp::UpdateValue(ITEM *pItem, bool bPush, bool bLock)
	{
		if (m_wndPropList.m_currentEdit)
			return false;

		CMFCPropertyGridProperty * pp = (CMFCPropertyGridProperty *)pItem->pItem;


		if (bPush)
		{

			CSingleLock lock(&pItem->currentValueLock, !!bLock);
			pItem->values[0].currentValue = pp->GetValue();

			m_pPack->SetValue(pItem);

			size_t i, cnt = pItem->values.size();
			for (i = 0; i < cnt; ++i)
				SetUpdate(pItem->values[i].iid, pItem->values[i].iid2);

			InterlockedIncrement(&m_nUpdate);
		}
		else
		{
			CSingleLock lock(&pItem->currentValueLock, !!bLock);

			COleVariant vOriginal = pp->GetValue(); //pp->GetOriginalValue();

			pItem->values[0].currentValue = pp->GetValue();
			bool ok = m_pPack->GetValue(pItem, vOriginal.vt);

			if (vOriginal.vt == VT_R8 && pItem->values[0].currentValue.vt == VT_BSTR)
			{
				m_pPack->GetValue(pItem, vOriginal.vt);
			}

			if (ok)
			{
#ifdef _DEBUG
				if (pItem->values[0].currentValue.vt != vOriginal.vt)
				{
					ok = m_pPack->GetValue(pItem, vOriginal.vt);
				}
#endif
				pp->SetValue( Variant2Variant(pItem->values[0].currentValue, vOriginal) );
			}

			if (pItem->hidden && pItem->nVer > 0)
			{
				pp->Show(TRUE);
				pItem->hidden = false;
			}

			if (m_sortItem && m_sortItem == pItem && pItem->values[0].currentValue.vt == VT_BOOL)
			{
				BOOL bSortNew = pItem->values[0].currentValue.boolVal;
				BOOL bSortOld = m_wndPropList.IsAlphabeticMode();
				if (bSortNew != bSortOld)
					m_wndPropList.SetAlphabeticMode(bSortNew);
			}
		}

		UpdateColor(pItem);
		return true;
	}



	bool CPropertiesWndProp::Reload()
	{
		bool ok = CPropertiesWnd::Reload();
	
		m_wndPropList.SetBoolLabels(GetLang(_T("BUTTON_YES")), GetLang(_T("BUTTON_NO")));

		OnUpdateTitle();

		m_wndPropList.RedrawWindow();
		return ok;
	}

	void CPropertiesWndProp::RemoveAllItems()
	{
		m_sortItem = 0;
		if (IsWindow(m_hWnd) && IsWindow(m_wndPropList.m_hWnd))
			m_wndPropList.RemoveAll();
		CPropertiesWnd::RemoveAllItems();
	}

	void CPropertiesWndProp::SetPropListFont()
	{
		CPropertiesWnd::SetPropListFont();
		m_wndPropList.SetFont(&m_fntPropList);
	}


	void CPropertiesWndProp::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		CPropertiesWnd::OnHScroll(nSBCode, nPos, pScrollBar);

		int pos = GetScrollPos(SB_HORZ);
		int vMin, vMax;
		GetScrollRange(SB_HORZ, &vMin, &vMax);

		switch (nSBCode)
		{
		case SB_LINELEFT:	//   Scroll left.
		case SB_LEFT:		//   Scroll to far left.
			pos = pos > vMin + 15 ? pos - 15 : vMin;
			break;
		case SB_ENDSCROLL:	//   End scroll.
			pos = vMax;
			break;
		case SB_PAGELEFT:	//   Scroll one page left.
			pos = pos > vMin + 50 ? pos - 50 : vMin;
			break;
		case SB_PAGERIGHT:	//   Scroll one page right.
			pos = pos < vMax - 50 ? pos + 50 : vMax;
			break;
		case SB_LINERIGHT:	//   Scroll right.
		case SB_RIGHT:	//   Scroll to far right.
			pos = pos < vMax - 15 ? pos + 15 : vMax;
			break;
		case SB_THUMBPOSITION:
			pos = nPos;
			break;
			//   Scroll to absolute position.The current position is specified by the nPos parameter.
		case SB_THUMBTRACK:	//
			pos = nPos;
			break;
		}

		SetScrollPos(SB_HORZ, pos);
		AdjustLayoutFn();

	}


	void CPropertiesWndProp::OnDestroy()
	{
		m_wndPropList.DestroyWindow();

		CPropertiesWnd::OnDestroy();
	}

	void CPropertiesWndProp::OnUpdateTimer()
	{
		CPropertiesWnd::OnUpdateTimer();

		if (!m_wndPropList.IsAlphabeticMode())
			return;
		ULONGLONG t = GetTickCount64();
		ULONGLONG dt = t - m_sortTimer;
		if (dt > 500)
		{
			m_wndPropList.ReposProperties(); // AdjustLayout();
			m_sortTimer = t;
		}
	}

	BOOL CPropertiesWndProp::CTRL::EditItem(CMFCPropertyGridProperty* pProp, LPPOINT lptClick)
	{
		CInc nEdit(&m_nEdit);

		ITEM * pItm = (ITEM*)pProp->GetData();
		if (!pItm || !pItm->pBase || !pItm->pBase->CanEdit(pProp))
			return false;

		ASSERT(!m_currentEdit);
		m_currentEdit = m_pPrevEdit = pItm;
		BOOL ok = CMFCPropertyGridCtrl::EditItem(pProp, lptClick);
		
		return ok;
	}

	BOOL CPropertiesWndProp::CTRL::EndEditItem(BOOL bUpdateData)
	{
		BOOL ok = CMFCPropertyGridCtrl::EndEditItem(bUpdateData);
		
		if (bUpdateData && ok )
		{
			int pos = -1;

			if (!m_nEdit && m_pPrevEdit) {
				pos = (m_pPrevEdit->pos + 1) % pBase->m_items.size();
				m_pPrevEdit = 0;

				if (pos >= 0 && GetKeyState(VK_RETURN) & 0x8000) {
					EditNext(pos);
				//	if (!pos && pBase->m_wndButtons.size() > 0 && pBase->m_wndButtons[0] && pBase->m_wndButtons[0]->m_hWnd)
				//		pBase->m_wndButtons[0]->SetFocus();
				}
			}

			m_currentEdit = 0;
		}

		return ok;
	}

	BOOL CPropertiesWndProp::CTRL::EditNext(int pos) {
		ITEM* pi = pBase->GetItemByPos(pos);
		if (!pi)
			return FALSE;
		CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)pi->pItem;
		
		if (!pProp) 
			return FALSE;
		SetCurSel(pProp);
		return TRUE;
	}


}

