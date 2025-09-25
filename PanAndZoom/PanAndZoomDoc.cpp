
// PanAndZoomDoc.cpp : implementation of the CPanAndZoomDoc class
//
#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "PanAndZoom.h"
#endif

#include "PanAndZoomDoc.h"
#include "IImageRenderer.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPanAndZoomDoc

IMPLEMENT_DYNCREATE(CPanAndZoomDoc, CDocument)

BEGIN_MESSAGE_MAP(CPanAndZoomDoc, CDocument)
END_MESSAGE_MAP()


// CPanAndZoomDoc construction/destruction

CPanAndZoomDoc::CPanAndZoomDoc() noexcept
{
	// TODO: add one-time construction code here
}

CPanAndZoomDoc::~CPanAndZoomDoc()
{
	if (m_pRenderer)
	{
		m_pRenderer->Shutdown();
		// m_pRenderer will be automatically destroyed by unique_ptr
	}
}

BOOL CPanAndZoomDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CPanAndZoomDoc serialization

void CPanAndZoomDoc::Serialize(CArchive& ar)
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
void CPanAndZoomDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CPanAndZoomDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CPanAndZoomDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CPanAndZoomDoc diagnostics

#ifdef _DEBUG
void CPanAndZoomDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPanAndZoomDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPanAndZoomDoc commands

BOOL CPanAndZoomDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// Create default renderer if none exists
	// Get current renderer type from application
	CPanAndZoomApp* pApp = (CPanAndZoomApp*)AfxGetApp();
	RendererType rendererType = pApp->GetCurrentRendererType();

	// Create renderer of the selected type
	if (!m_pRenderer)
	{
		m_pRenderer = RendererManager::CreateRenderer(rendererType);

		// Get the view window handle
		POSITION pos = GetFirstViewPosition();
		if (pos != NULL)
		{
			CView* pView = GetNextView(pos);
			if (!m_pRenderer->Initialize(pView->GetSafeHwnd()))
			{
				CString errorMsg;
				errorMsg.Format(_T("Failed to initialize %s renderer"),
					RendererManager::GetRendererDisplayName(rendererType));
				AfxMessageBox(errorMsg);
				return FALSE;
			}
		}
	}

	// Load the image
	if (!m_pRenderer->LoadImage(lpszPathName))
	{
		AfxMessageBox(_T("Failed to load image"));
		return FALSE;
	}

	m_strImagePath = lpszPathName;
	UpdateAllViews(NULL);
	return TRUE;
}

void CPanAndZoomDoc::SetRenderer(std::unique_ptr<IImageRenderer> renderer)
{
	CString currentImagePath = m_strImagePath;

	// Shutdown old renderer
	if (m_pRenderer)
		m_pRenderer->Shutdown();

	// Set new renderer
	m_pRenderer = std::move(renderer);

	// Initialize and reload image if we had one
	if (m_pRenderer && m_pRenderer->Initialize(AfxGetMainWnd()->GetSafeHwnd()))
	{
		if (!currentImagePath.IsEmpty())
		{
			m_pRenderer->LoadImage(currentImagePath);
		}
	}

	UpdateAllViews(NULL);
}

void CPanAndZoomDoc::DeleteContents()
{
	// TODO: Add your specialized code here and/or call the base class
	// Clean up renderer's image data
	if (m_pRenderer)
	{
		m_pRenderer->UnloadImage();
		// Don't delete m_pRenderer - we want to keep the renderer instance
		// Only unload the current image
	}

	m_strImagePath.Empty();

	CDocument::DeleteContents();
}
