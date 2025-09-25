
// PanAndZoom.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "PanAndZoom.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "PanAndZoomDoc.h"
#include "PanAndZoomView.h"

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPanAndZoomApp

BEGIN_MESSAGE_MAP(CPanAndZoomApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CPanAndZoomApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	// ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_OPEN, &CPanAndZoomApp::OnFileOpen)
	ON_COMMAND(ID_RENDERMODE_GDIPLUS, &CPanAndZoomApp::OnRendermodeGdiplus)
	ON_COMMAND(ID_RENDERMODE_OPENGL, &CPanAndZoomApp::OnRendermodeOpengl)
	ON_UPDATE_COMMAND_UI(ID_RENDERMODE_GDIPLUS, &CPanAndZoomApp::OnUpdateRendermodeGdiplus)
	ON_UPDATE_COMMAND_UI(ID_RENDERMODE_OPENGL, &CPanAndZoomApp::OnUpdateRendermodeOpengl)
END_MESSAGE_MAP()


// CPanAndZoomApp construction

CPanAndZoomApp::CPanAndZoomApp() noexcept
{

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("PanAndZoom.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CPanAndZoomApp object

CPanAndZoomApp theApp;


// CPanAndZoomApp initialization

BOOL CPanAndZoomApp::InitInstance()
{
	// Initialize GDI+ before any other initialization
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_PanAndZoomTYPE,
		RUNTIME_CLASS(CPanAndZoomDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CPanAndZoomView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Prevent automatic document creation at startup
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CPanAndZoomApp::ExitInstance()
{
	// Shutdown GDI+
	GdiplusShutdown(m_gdiplusToken);

	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CPanAndZoomApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CPanAndZoomApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CPanAndZoomApp message handlers




void CPanAndZoomApp::OnFileOpen()
{
	// TODO: Add your command handler code here
	// Create file dialog with image-only filter
	CFileDialog dlg(TRUE,        // Open dialog (not Save)
		NULL,                    // Default extension
		NULL,                    // Default filename
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,  // Flags
		_T("JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG Files (*.png)|*.png|All Image Files (*.jpg;*.jpeg;*.png)|*.jpg;*.jpeg;*.png||"));

	dlg.m_ofn.nFilterIndex = 3;  // Default to "All Image Files"

	if (dlg.DoModal() == IDOK)
	{
		// Open the selected file
		OpenDocumentFile(dlg.GetPathName());
	}
}

// Helper function to close all documents
void CPanAndZoomApp::CloseAllDocuments()
{
	// Close all open documents
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != NULL)
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		POSITION docPos = pTemplate->GetFirstDocPosition();
		while (docPos != NULL)
		{
			CDocument* pDoc = pTemplate->GetNextDoc(docPos);
			if (pDoc != NULL)
			{
				pDoc->OnCloseDocument();
			}
		}
	}
}

void CPanAndZoomApp::OnRendermodeGdiplus()
{
	// TODO: Add your command handler code here
	if (m_currentRendererType != RendererType::GDIPlus)
	{
		m_currentRendererType = RendererType::GDIPlus;
		CloseAllDocuments();
	}
}

void CPanAndZoomApp::OnRendermodeOpengl()
{
	// TODO: Add your command handler code here
	if (m_currentRendererType != RendererType::OpenGL)
	{
		m_currentRendererType = RendererType::OpenGL;
		CloseAllDocuments();
	}
}

void CPanAndZoomApp::OnUpdateRendermodeGdiplus(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_currentRendererType == RendererType::GDIPlus);
}

void CPanAndZoomApp::OnUpdateRendermodeOpengl(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_currentRendererType == RendererType::OpenGL);
}
