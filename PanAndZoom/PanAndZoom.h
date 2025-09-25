
// PanAndZoom.h : main header file for the PanAndZoom application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "IImageRenderer.h"


// CPanAndZoomApp:
// See PanAndZoom.cpp for the implementation of this class
//

class CPanAndZoomApp : public CWinApp
{
public:
	CPanAndZoomApp() noexcept;
	RendererType GetCurrentRendererType() const { return m_currentRendererType; }
	void SetCurrentRendererType(RendererType type) { m_currentRendererType = type; }

private:
	ULONG_PTR m_gdiplusToken;
	RendererType m_currentRendererType = RendererType::OpenGL;
	void CloseAllDocuments();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileOpen();
	afx_msg void OnRendermodeGdiplus();
	afx_msg void OnRendermodeOpengl();
	afx_msg void OnUpdateRendermodeGdiplus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRendermodeOpengl(CCmdUI* pCmdUI);
};

extern CPanAndZoomApp theApp;
