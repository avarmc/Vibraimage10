
// VibraimageExDoc.cpp : implementation of the CVibraimageExDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "VibraimageEx.h"
#endif

#include "VibraimageExDoc.h"
#include "VibraimageExFrame.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVibraimageExDoc

IMPLEMENT_DYNCREATE(CVibraimageExDoc, CDocument)

BEGIN_MESSAGE_MAP(CVibraimageExDoc, CDocument)
END_MESSAGE_MAP()


// CVibraimageExDoc construction/destruction

CVibraimageExDoc::CVibraimageExDoc()
{
	theApp.m_pDoc = this;
}

CVibraimageExDoc::~CVibraimageExDoc()
{
	theApp.m_pDoc = 0;
}

BOOL CVibraimageExDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	
	theApp.m_pFrame->SetTimer(CVibraimageExFrame::T_NEW_DOC, 1000, 0);
	

	return TRUE;
}




// CVibraimageExDoc serialization

void CVibraimageExDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CVibraimageExDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CVibraimageExDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CVibraimageExDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CVibraimageExDoc diagnostics

#ifdef _DEBUG
void CVibraimageExDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVibraimageExDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVibraimageExDoc commands


void CVibraimageExDoc::MakeTitle(BOOL bTmp)
{
	if (theApp.m_bDone)
		return;
	if (bTmp)
		theApp.m_pFrame->SetTimer(CVibraimageExFrame::T_TITLE, bTmp, 0);
	else
	{
		TCHAR path[MAX_PATH * 4] = { 0 };
		theApp.m_pEngine->engine.GetCurrentDocument(path, MAX_PATH * 4);
		SetTitle(path);
	}
}


BOOL CVibraimageExDoc::OnOpenDocument(LPCWSTR lpszPathName)
{
	theApp.OnOpenDocument(theApp.m_pEngine->id,lpszPathName);
//	theApp.m_pEngine->engine.OpenDocument(lpszPathName);
//	theApp.m_pEngine->engine.OnMenu(ID_ACTION_MEASUREMENT); //start measurement
	MakeTitle();

	theApp.RegFlush();

	return TRUE;
}


void CVibraimageExDoc::DeleteContents()
{
	theApp.m_pEngine->engine.OnMenu(ID_VIDEOSETTINGS0_STOP);

	MakeTitle();

	__super::DeleteContents();

	theApp.RegFlush();
}

void CVibraimageExDoc::OnNewVar(int id, int subID)
{
	if (id == VI_INFO_MENU)
	{
		if(subID == (theApp.m_pEngine->engine.Tag2Id(_T("ID_FILE_OPEN")) | 0x10000))
			MakeTitle(TRUE);
		else
		if (subID == (ID_VIDEOSETTINGS0_STOP | 0x10000)) {
			MakeTitle(TRUE);
		}
	}
}

void CVibraimageExDoc::SetTitle(LPCTSTR lpszTitle)
{
	// return;
	if (!theApp.m_pFrame)
		return;

	theApp.m_pFrame->UpdateTitle();

	CString empty;
	empty.LoadString(AFX_IDS_UNTITLED);


	if (!lpszTitle || empty == lpszTitle)
	{
		theApp.m_pFrame->ModifyStyle(FWS_ADDTOTITLE, 0);
		__super::SetTitle(_T(""));
	}
	else
	{
		theApp.m_pFrame->ModifyStyle(0, FWS_ADDTOTITLE);
		__super::SetTitle(lpszTitle);
	}

}
