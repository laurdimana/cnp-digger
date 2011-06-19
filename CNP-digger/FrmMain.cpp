
// FrmMain.cpp : implementation of the CFrmMain class
//

#include "stdafx.h"
#include "CNP-digger.h"
#include "FrmMain.h"
#include "DlgMedics.h"

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

	ON_EN_CHANGE( FRM_MAIN_TXT_CNP, OnTxtCnpChange )
	ON_BN_CLICKED( FRM_MAIN_BTN_GO, OnBtnGo )

	ON_MESSAGE( WM_UPDATE_PATIENTS_TABLE, OnUpdatePatientsTable )
END_MESSAGE_MAP()

static UINT indicators[] = { ID_SEPARATOR };

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

BOOL CFrmMain::PreTranslateMessage( MSG *pMsg )
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		pMsg->wParam = NULL;

		if ( this->GetFocus()->GetSafeHwnd() == m_txtCNP.GetSafeHwnd() )
			OnBtnGo();

		return 1;
	}
	else if ( IsDialogMessage( pMsg ) )
		return 1;

	return CFrameWnd::PreTranslateMessage( pMsg );
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
	CRect crect( 10, 10, 270, 40 );
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

	// Patients lbl
	crect.SetRect( 280, 13, 330, 36 );
	bSucc = m_lblPatients.Create(
		L"0",
		WS_CHILD | WS_VISIBLE | SS_CENTER | SS_SUNKEN,
		crect,
		this );

	if ( !bSucc )
	{
		TRACE( L"@ CFrmMain::OnCreate -> Failed to create patients label\n" );
		return -1;
	}

	m_lblPatients.SetFont( &m_fntSansSerif12 );

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
	m_tblPatients.InsertColumn( 0, L"CNP", LVCFMT_LEFT, 90 );
	m_tblPatients.InsertColumn( 1, L"Last Name", LVCFMT_LEFT, 110 );
	m_tblPatients.InsertColumn( 2, L"First Name", LVCFMT_LEFT, 130 );
	m_tblPatients.InsertColumn( 3, L"+", LVCFMT_LEFT, 20 );

	m_txtCNP.SetFocus();

	return 0;
}

void CFrmMain::OnFileMedics()
{	
	CDlgMedics dlgMedics;
	CString    strCurMedicId = theApp.m_pProgramData->GetCurrentMedic().strID;

	dlgMedics.DoModal();

	if ( theApp.m_pProgramData->GetCurrentMedic().strID.GetLength() > 0 )
	{
		if ( theApp.m_pProgramData->GetCurrentMedic().strID != strCurMedicId )
		{
			SetWndTitle( L" [" + theApp.m_pProgramData->GetCurrentMedic().strLastName + L" " + 
				theApp.m_pProgramData->GetCurrentMedic().strFirstName + L" " + 
				theApp.m_pProgramData->GetCurrentMedic().strID + L"]" );

			theApp.m_pWorkerThread->PostThreadMessage( WM_INIT_MEDIC, NULL, NULL );
		}
	}
	else
		this->PostMessage( WM_CLOSE );
}

void CFrmMain::OnFileImport()
{
}

void CFrmMain::OnFileExport()
{
}

void CFrmMain::OnTxtCnpChange()
{
	wchar_t *pszCnp = new wchar_t[ m_txtCNP.GetWindowTextLength() + 1 ];

	m_txtCNP.GetWindowText( pszCnp, m_txtCNP.GetWindowTextLength() + 1 );

	if ( theApp.m_pProgramData->IsCnpValid( pszCnp ) )
		m_txtCNP.SetSel( 0, -1 );

	this->PostMessage( WM_UPDATE_PATIENTS_TABLE, (WPARAM)pszCnp );
}

void CFrmMain::OnBtnGo()
{
	wchar_t *pszCnp = new wchar_t[ m_txtCNP.GetWindowTextLength() + 1 ];

	m_txtCNP.GetWindowText( pszCnp, m_txtCNP.GetWindowTextLength() + 1 );

	if ( !theApp.m_pProgramData->IsCnpValid( pszCnp ) ||
		theApp.m_pProgramData->GetDisplayedPatients() > 0 )
	{
		ResetTxtCnp();
		SetStatus( (CString)MAKEINTRESOURCE( STATUS_CNP_NOT_ADDED ) );
		delete [] pszCnp;

		return;
	}

	theApp.m_pWorkerThread->PostThreadMessage( WM_DIG_FOR_CNP, (WPARAM)pszCnp, (LPARAM)TRUE );
}

LRESULT CFrmMain::OnUpdatePatientsTable( WPARAM wParam, LPARAM lParam )
{
	CString strSuffix = wParam ? (wchar_t*)wParam : L"";
	int		index     = 0;

	delete [] (wchar_t*)wParam;

	m_tblPatients.DeleteAllItems();

	// Add temp patients
	CListPatients *list = theApp.m_pProgramData->GetTempPatientsList();
	POSITION	  pos   = list->GetHeadPosition();

	while ( pos )
	{
		PATIENT p = list->GetNext( pos );

		if ( strSuffix.IsEmpty() || p.strID.Left( strSuffix.GetLength() ) == strSuffix )
			AddPatientToTable( index++, p.strID, p.strLastName, p.strFirstName, TRUE );
	}

	//Add patients
	list = theApp.m_pProgramData->GetPatientsList();
	pos  = list->GetHeadPosition();

	while ( pos )
	{
		PATIENT p = list->GetNext( pos );

		if ( strSuffix.IsEmpty() || p.strID.Left( strSuffix.GetLength() ) == strSuffix )
			AddPatientToTable( index++, p.strID, p.strLastName, p.strFirstName );
	}

	// Update current number of patients
	theApp.m_pProgramData->SetDislayedPatients( index );
	strSuffix.Format( L"%d", index );
	m_lblPatients.SetWindowText( strSuffix );

	m_txtCNP.SetFocus();

	return (LRESULT)0;
}

//////////////////////////////////////////////////////// Methods /////////////////////////////////////////////////////////

BOOL CFrmMain::AddPatientToTable( int nNo, CString strCNP, CString strLastName, CString strFirstName, BOOL bTemp )
{
	LVITEM lvi = { 0 };

	lvi.iItem = nNo;
	lvi.mask  = LVIF_TEXT;

	// CNP
	lvi.iSubItem = 0;
	lvi.pszText  = strCNP.GetBuffer();
	m_tblPatients.InsertItem( &lvi );

	// Last name
	lvi.iSubItem = 1;
	lvi.pszText  = strLastName.GetBuffer();
	m_tblPatients.SetItem( &lvi );

	// First name
	lvi.iSubItem = 2;
	lvi.pszText  = strFirstName.GetBuffer();
	m_tblPatients.SetItem( &lvi );

	// Temp
	if ( bTemp )
	{
		lvi.iSubItem = 3;
		lvi.pszText  = TEMP_TABLE_MARKER;
		m_tblPatients.SetItem( &lvi );
	}

	return TRUE;
}

void CFrmMain::SetStatus( CString strMsg )
{
	m_StatusBar.SetWindowText( strMsg );
}

void CFrmMain::SetWndTitle( CString strTitle )
{
	CString strWndTitle;

	strWndTitle.LoadString( AFX_IDS_APP_TITLE );
	this->SetWindowText( strWndTitle + strTitle );
}

void CFrmMain::ResetTxtCnp()
{
	m_txtCNP.SetWindowText( L"" );
	m_txtCNP.SetFocus();
}