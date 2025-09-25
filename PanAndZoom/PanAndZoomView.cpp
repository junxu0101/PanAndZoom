
// PanAndZoomView.cpp : implementation of the CPanAndZoomView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "PanAndZoom.h"
#endif

#include "PanAndZoomDoc.h"
#include "PanAndZoomView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPanAndZoomView

IMPLEMENT_DYNCREATE(CPanAndZoomView, CScrollView)

BEGIN_MESSAGE_MAP(CPanAndZoomView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CPanAndZoomView construction/destruction

CPanAndZoomView::CPanAndZoomView() noexcept
	: m_bZooming(false)
	, m_zoomStartPoint(0, 0)
	, m_initialZoom(1.0f)
{
	// TODO: add construction code here
}

CPanAndZoomView::~CPanAndZoomView()
{
}

BOOL CPanAndZoomView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

void CPanAndZoomView::UpdateScrollSizes()
{
	CPanAndZoomDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	IImageRenderer* pRenderer = pDoc->GetRenderer();
	if (pRenderer && pRenderer->HasImage())
	{
		CSize imageSize = pRenderer->GetImageSize(); // This now returns zoomed size
		SetScrollSizes(MM_TEXT, imageSize);
		ScrollToPosition(CPoint(0, 0));
	}
	else
	{
		SetScrollSizes(MM_TEXT, CSize(1, 1));
	}
}

// CPanAndZoomView drawing

void CPanAndZoomView::OnDraw(CDC* pDC)
{
	CPanAndZoomDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	IImageRenderer* pRenderer = pDoc->GetRenderer();
	if (!pRenderer || !pRenderer->HasImage())
		return;

	// Get client rectangle and scroll position
	CRect clientRect;
	GetClientRect(&clientRect);
	CPoint scrollPos = GetScrollPosition();

	// Let the renderer handle the drawing
	pRenderer->Render(pDC, clientRect, scrollPos);
}


// CPanAndZoomView printing

BOOL CPanAndZoomView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPanAndZoomView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPanAndZoomView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CPanAndZoomView diagnostics

#ifdef _DEBUG
void CPanAndZoomView::AssertValid() const
{
	CView::AssertValid();
}

void CPanAndZoomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPanAndZoomDoc* CPanAndZoomView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPanAndZoomDoc)));
	return (CPanAndZoomDoc*)m_pDocument;
}
#endif //_DEBUG


// CPanAndZoomView message handlers

void CPanAndZoomView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	UpdateScrollSizes();
}

void CPanAndZoomView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class
	CPanAndZoomDoc* pDoc = GetDocument();
	if (pDoc)
	{
		IImageRenderer* pRenderer = pDoc->GetRenderer();
		if (pRenderer && !pRenderer->HasImage())
		{
			// Initialize with the VIEW's window handle, not main frame
			pRenderer->Initialize(GetSafeHwnd());
		}
	}

	UpdateScrollSizes();
	CScrollView::OnUpdate(pSender, lHint, pHint);
}

void CPanAndZoomView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// Notify renderer of resize (important for Direct3D/OpenGL)
	CPanAndZoomDoc* pDoc = GetDocument();
	if (pDoc)
	{
		IImageRenderer* pRenderer = pDoc->GetRenderer();
		if (pRenderer)
		{
			pRenderer->OnResize(cx, cy);
		}
	}
}

BOOL CPanAndZoomView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	// For hardware-accelerated renderers, we might want to skip erasing
	// to avoid flicker. For now, let the base class handle it.
	CPanAndZoomDoc* pDoc = GetDocument();
	if (pDoc)
	{
		IImageRenderer* pRenderer = pDoc->GetRenderer();
		if (pRenderer && pRenderer->SupportsHardwareAcceleration())
		{
			// Skip erasing for hardware renderers to reduce flicker
			return TRUE;
		}
	}

	return CScrollView::OnEraseBkgnd(pDC);
}

void CPanAndZoomView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPanAndZoomDoc* pDoc = GetDocument();
	if (pDoc && pDoc->GetRenderer() && pDoc->GetRenderer()->HasImage())
	{
		m_bZooming = true;
		m_zoomStartPoint = point;
		m_initialZoom = pDoc->GetRenderer()->GetZoom();
		SetCapture();
	}

	CScrollView::OnLButtonDown(nFlags, point);
}

void CPanAndZoomView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bZooming)
	{
		m_bZooming = false;
		ReleaseCapture();
	}

	CScrollView::OnLButtonUp(nFlags, point);
}

void CPanAndZoomView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// Reset to original size (100% zoom)
	PerformZoom(1.0f);
	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CPanAndZoomView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bZooming && (nFlags & MK_LBUTTON))
	{
		// Calculate zoom based on mouse movement
		int deltaX = point.x - m_zoomStartPoint.x;
		int deltaY = point.y - m_zoomStartPoint.y;

		// Use the larger movement for zoom calculation
		int delta = (abs(deltaX) > abs(deltaY)) ? deltaX : deltaY;

		// Convert pixel movement to zoom factor
		float zoomChange = delta / 500.0f; // Adjust sensitivity as needed
		float newZoom = m_initialZoom + zoomChange;

		PerformZoom(newZoom);
	}

	CScrollView::OnMouseMove(nFlags, point);
}

void CPanAndZoomView::PerformZoom(float newZoom)
{
	CPanAndZoomDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->GetRenderer())
		return;

	IImageRenderer* pRenderer = pDoc->GetRenderer();
	pRenderer->SetZoom(newZoom);

	// Update scroll sizes for new zoom level
	UpdateScrollSizes();

	// Redraw
	Invalidate();
}
