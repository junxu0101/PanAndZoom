
// PanAndZoomDoc.h : interface of the CPanAndZoomDoc class
//


#pragma once
#include <memory>

#pragma comment(lib, "gdiplus.lib")
#include "IImageRenderer.h"

class CPanAndZoomDoc : public CDocument
{
protected: // create from serialization only
	CPanAndZoomDoc() noexcept;
	DECLARE_DYNCREATE(CPanAndZoomDoc)

// Attributes
public:

// Operations
public:
	// New interface
	IImageRenderer* GetRenderer() const { return m_pRenderer.get(); }
	void SetRenderer(std::unique_ptr<IImageRenderer> renderer);
	CString GetImagePath() const { return m_strImagePath; }

	// Legacy interface for compatibility
	bool HasImage() const { return m_pRenderer && m_pRenderer->HasImage(); }
	CSize GetImageSize() const { return m_pRenderer ? m_pRenderer->GetImageSize() : CSize(0, 0); }

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
	virtual ~CPanAndZoomDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	std::unique_ptr<IImageRenderer> m_pRenderer;
	CString m_strImagePath;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
};
