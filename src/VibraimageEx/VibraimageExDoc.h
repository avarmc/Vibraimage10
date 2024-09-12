
// VibraimageExDoc.h : interface of the CVibraimageExDoc class
//


#pragma once


class CVibraimageExDoc : public CDocument, public CNewVarEventVI
{
protected: // create from serialization only
	CVibraimageExDoc();
	DECLARE_DYNCREATE(CVibraimageExDoc)

// Attributes
public:
// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CVibraimageExDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	void MakeTitle(BOOL bTmp=0);

	virtual BOOL OnOpenDocument(LPCWSTR lpszPathName);
	virtual void DeleteContents();

	void OnNewVar(int id, int subID = 0);

	virtual void SetTitle(LPCTSTR lpszTitle);
};
