
// FrmMain.cpp : implementation of the CFrmMain class
//

#include "stdafx.h"
#include "CNP-digger.h"
#include "FrmMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFrmMain

IMPLEMENT_DYNAMIC( CFrmMain, CFrameWnd )

BEGIN_MESSAGE_MAP( CFrmMain, CFrameWnd )
	ON_WM_CREATE()

	ON_COMMAND( FRM_MAIN_MNU_FILE_MEDICS, OnFileMedics )
	ON_COMMAND( FRM_MAIN_MNU_FILE_IMPORT, OnFileImport )
	ON_COMMAND( FRM_MAIN_MNU_FILE_EXPORT, OnFileExport )
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR
};

////////////////////////////////////////////// Constructor / Destructor //////////////////////////////////////////////////////

CFrmMain::CFrmMain()
{
}

CFrmMain::~CFrmMain()
{
}

///////////////////////////////////////////////////// Ovverides ///////////////////////////////////////////////////////////

BOOL CFrmMain::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow( cs ) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(	
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, 
		::LoadCursor( NULL, IDC_ARROW ), 
		reinterpret_cast<HBRUSH>(COLOR_3DFACE+1), 
		::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDR_MAINFRAME ) ) );

	return TRUE;
}

//////////////////////////////////////////////////// Events ///////////////////////////////////////////////////////////////

int CFrmMain::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CFrameWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	// Create font
	m_fntSansSerif12.CreatePointFont( 120, L"MS Sans Serif" );

	// Create status bar
	BOOL bSucc = m_StatusBar.Create( this );

	if ( !bSucc )
	{
		TRACE( L"@ CFrmMain::OnCreate -> Failed to create status bar\n" );
		return -1;
	}

	m_StatusBar.SetIndicators( indicators, sizeof(indicators)/sizeof(UINT) );

	// Move wnd in center of screen
	int nScreenW = ::GetSystemMetrics( SM_CXSCREEN );
	int nScreenH = ::GetSystemMetrics( SM_CYSCREEN );
	int nWndW = 400;
	int nWndH = 800;
	int nWndX = (nScreenW - nWndW) / 2;
	int nWndY = (nScreenH - nWndH) / 2;
	this->SetWindowPos( NULL, nWndX, nWndY, nWndW, nWndH, SWP_SHOWWINDOW );

	// Create the cnp textbox
	CRect crect( 10, 10, 330, 40 );
	bSucc = m_txtCNP.CreateEx( 
		WS_EX_CLIENTEDGE,
		L"EDIT",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		crect,
		this,
		FRM_MAIN_TXT_CNP );

	if ( !bSucc )
	{
		TRACE( L"@ CFrmMain::OnCreate -> Failed to create CNP text box\n" );
		return -1;
	}

	m_txtCNP.SetFont( &m_fntSansSerif12 );

	// Go button
	crect.SetRect( 340, 10, 385, 40 );
	bSucc = m_btnGo.Create(
		L"GO",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		crect,
		this,
		FRM_MAIN_BTN_GO );

	if ( !bSucc )
	{
		TRACE( L"@ CFrmMain::OnCreate -> Failed to create go button\n" );
		return -1;
	}

	m_btnGo.SetFont( &m_fntSansSerif12 );

	// Pacients table
	crect.SetRect( 10, 50, 385, 720 );
	bSucc = m_tblPatients.CreateEx( 
		WS_EX_CLIENTEDGE,
		WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS,
		crect,
		this,
		FRM_MAIN_TBL_PERSONS );

	if ( !bSucc )
	{
		TRACE( L"@ CFrmMain::OnCreate -> Failed to create the perosns table\n" );
		return -1;
	}

	m_tblPatients.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP | m_tblPatients.GetExtendedStyle() );
	m_tblPatients.InsertColumn( 0, L"CNP", LVCFMT_LEFT, 100 );
	m_tblPatients.InsertColumn( 1, L"Last Name", LVCFMT_LEFT, 120 );
	m_tblPatients.InsertColumn( 2, L"First Name", LVCFMT_LEFT, 140 );

	m_txtCNP.SetFocus();

	return 0;
}

void CFrmMain::OnFileMedics()
{
	AfxMessageBox( L"TODO: Select medic dialog" );
	this->PostMessage( WM_QUIT );
}

void CFrmMain::OnFileImport()
{
}

void CFrmMain::OnFileExport()
{
}

//////////////////////////////////////////////////////// Methods /////////////////////////////////////////////////////////

BOOL CFrmMain::AddPatientToTable( int nNo, wchar_t *pszCNP, wchar_t *pszLastName, wchar_t *pszFirstName )
{
	LVITEM lvi = { 0 };

	lvi.iItem = nNo;
	lvi.mask  = LVIF_TEXT;

	// CNP
	lvi.iSubItem = 0;
	lvi.pszText  = pszCNP;
	m_tblPatients.InsertItem( &lvi );

	// Last name
	lvi.iSubItem = 1;
	lvi.pszText  = pszLastName;
	m_tblPatients.SetItem( &lvi );

	// First name
	lvi.iSubItem = 2;
	lvi.pszText  = pszFirstName;
	m_tblPatients.SetItem( &lvi );

	return TRUE;
}