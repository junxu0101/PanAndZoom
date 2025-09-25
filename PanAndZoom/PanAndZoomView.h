
// PanAndZoomView.h : interface of the CPanAndZoomView class
//

#pragma once


class CPanAndZoomView : public CScrollView
{
protected: // create from serialization only
	CPanAndZoomView() noexcept;
	DECLARE_DYNCREATE(CPanAndZoomView)

// Attributes
public:
	CPanAndZoomDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CPanAndZoomView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void UpdateScrollSizes();
	void PerformZoom(float newZoom);

private:
	bool m_bZooming;
	CPoint m_zoomStartPoint;
	float m_initialZoom;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in PanAndZoomView.cpp
inline CPanAndZoomDoc* CPanAndZoomView::GetDocument() const
   { return reinterpret_cast<CPanAndZoomDoc*>(m_pDocument); }
#endif

