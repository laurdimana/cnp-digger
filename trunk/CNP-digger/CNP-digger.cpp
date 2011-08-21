
// CNP-digger.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "CNP-digger.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCNPdiggerApp

BEGIN_MESSAGE_MAP(CCNPdiggerApp, CWinApp)
	ON_COMMAND( ID_APP_ABOUT, &CCNPdiggerApp::OnAppAbout )
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////// Constructor /////////////////////////////////////////////////////////

CCNPdiggerApp::CCNPdiggerApp()
{
	SetAppID( APP_ID );
}

// The one and only CCNPdiggerApp object

CCNPdiggerApp theApp;


/////////////////////////////////////////////////////////////////// Ovverides ///////////////////////////////////////////////////////

BOOL CCNPdiggerApp::InitInstance()
{
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

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	m_pFrmMain = new CFrmMain();

	if ( !m_pFrmMain )
	{
		TRACE( L"@ CCNPdiggerApp::InitInstance -> Failed to create FrmMain\n" );
		return FALSE;
	}

	m_pMainWnd = m_pFrmMain;

	// create and load the frame with its resources
	m_pFrmMain->LoadFrame( 
		IDR_MAINFRAME,
		WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_MINIMIZEBOX | WS_SYSMENU, 
		NULL,
		NULL );

	// The one and only window has been initialized, so show and update it
	m_pFrmMain->ShowWindow( SW_SHOW );
	m_pFrmMain->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	// Init program data
	m_pProgramData = new CProgramData();

	if ( !m_pProgramData )
	{
		TRACE( L"@ CCNPdiggerApp::InitInstance -> Failed to init program data\n" );
		return FALSE;
	}

	// Create and start the worker thread
	m_pWorkerThread = new CWorkerThread();

	if ( !m_pWorkerThread || !m_pWorkerThread->CreateThread() )
	{
		TRACE( L"@ CCNPdiggerApp::InitInstance -> Failed to create the worker thread\n" );
		return FALSE;
	}

	m_pWorkerThread->PostThreadMessage( WM_CHECK_FOR_ESSENTIAL_FILES, NULL, NULL );

	return TRUE;
}

int CCNPdiggerApp::ExitInstance()
{
	if ( m_pWorkerThread != NULL )
	{
		m_pWorkerThread->PostThreadMessage( WM_QUIT, 0, 0 );

		if ( ::WaitForSingleObject( m_pWorkerThread->m_hThread, WORKER_THREAD_STOP_TIMEOUT ) == WAIT_TIMEOUT )
		{
			::TerminateThread( m_pWorkerThread->m_hThread, 0 );
			TRACE( L"@ CCNPdiggerApp::ExitInstance -> Killed the worker thread\n" );
		}
		else
			TRACE( L"@ CCNPdiggerApp::ExitInstance -> Stopped the worker thread\n" );
	}

	if ( m_pProgramData != NULL )
		delete m_pProgramData;

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////// Methods ////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// About dlg ////////////////////////////////////////////////////////////////
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx( CAboutDlg::IDD )
{
}

void CAboutDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialogEx::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CAboutDlg, CDialogEx )
END_MESSAGE_MAP()

void CCNPdiggerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
