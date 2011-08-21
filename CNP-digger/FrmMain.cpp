
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
	int nWndH = 600;
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
		FRM_MAIN_BTN_GO_CAPTION,
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
	crect.SetRect( 10, 50, 385, 520 );
	bSucc = m_tblPatients.CreateGrid(
		WS_CHILD | WS_VISIBLE,
		crect,
		this,
		FRM_MAIN_TBL_PERSONS );

	if ( !bSucc )
	{
		TRACE( L"@ CFrmMain::OnCreate -> Failed to create the perosns table\n" );
		return -1;
	}

	m_tblPatients.SetNumberCols( 3 );

	m_tblPatients.SetColWidth( -1, 0 );

	m_tblPatients.QuickSetText( 0, -1, FRM_MAIN_TBL_PATIENTS_COL_0_CAPTION );
	m_tblPatients.SetColWidth( 0, FRM_MAIN_TBL_PATIENTS_COL_0_WIDTH );

	m_tblPatients.QuickSetText( 1, -1, FRM_MAIN_TBL_PATIENTS_COL_1_CAPTION );
	m_tblPatients.SetColWidth( 1, FRM_MAIN_TBL_PATIENTS_COL_1_WIDTH );

	m_tblPatients.QuickSetText( 2, -1, FRM_MAIN_TBL_PATIENTS_COL_2_CAPTION );
	m_tblPatients.SetColWidth( 2, FRM_MAIN_TBL_PATIENTS_COL_2_WIDTH );

	m_tblPatients.SetVScrollMode( UG_SCROLLTRACKING );
	m_tblPatients.SetBallisticMode( 3 );
	m_tblPatients.SetBallisticDelay( 100 );
	m_tblPatients.SetDoubleBufferMode( FRM_MAIN_TBL_PATIENTS_DOUBLE_BUFFER );

	m_txtCNP.SetFocus();

	return 0;
}

void CFrmMain::OnFileMedics()
{
	CDlgMedics dlgMedics;
	CString    strCurMedicId = theApp.m_pProgramData->GetCurrentMedicID();

	dlgMedics.DoModal();

	if ( theApp.m_pProgramData->GetCurrentMedicID().GetLength() > 0 &&
		theApp.m_pProgramData->GetMedicsMap()->PLookup( theApp.m_pProgramData->GetCurrentMedicID() ) != NULL )
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
	CFileDialog dlgOpenXml( TRUE, L"*.xml", NULL, 0, L"XML Files (*.xml)|*.xml|All Files (*.*)|*.*||" );

	if ( dlgOpenXml.DoModal() == IDOK )
	{
		CString *pstrDst = new CString( theApp.m_pProgramData->GetCurrentDir() + L"\\" + PATIENTS_DIR + L"\\" +
			theApp.m_pProgramData->GetCurrentMedic().strLastName + L"-" + 
			theApp.m_pProgramData->GetCurrentMedic().strFirstName + L"-" + 
			theApp.m_pProgramData->GetCurrentMedic().strID + L".xml" );
		CString *pstrSrc = new CString( dlgOpenXml.GetPathName() );

		theApp.m_pWorkerThread->PostThreadMessage( WM_IMPORT_PATIENTS_XML, (WPARAM)pstrSrc, (LPARAM)pstrDst );
	}
}

void CFrmMain::OnFileExport()
{
	if ( theApp.m_pProgramData->GetTempPatients() > 0 )
		theApp.m_pWorkerThread->PostThreadMessage( WM_EXPORT_TEMP_PATIENTS, 0, 0 );
	else
		SetStatus( (CString)MAKEINTRESOURCE( STATUS_PATIENTS_NOT_EXPORTED ) );
}

void CFrmMain::OnTxtCnpChange()
{
	CString *pstrCnp = new CString();

	m_txtCNP.GetWindowText( *pstrCnp );
	int nLen	= pstrCnp->GetLength();
	int nGender = nLen > 0  ? _wtoi( pstrCnp->Mid( 0, 1 ) ) : -1;
	int nMonth	= nLen >= 5 ? _wtoi( pstrCnp->Mid( 3, 2 ) ) : -1;
	int nDay	= nLen >= 7 ? _wtoi( pstrCnp->Mid( 5, 2 ) ) : -1;

	m_txtCNP.SetBkColor( COLOR_WHITE );

	if ( nGender != -1 &&
		( nGender != 1 && nGender != 2 && nGender != 5 && nGender != 6 ) )
	{
		m_txtCNP.SetBkColor( COLOR_RED );
		return;
	}

	if ( nMonth != -1 &&
		( nMonth < 1 || nMonth > 12 ) )
	{
		m_txtCNP.SetBkColor( COLOR_RED );
		return;
	}

	if ( nDay != -1 &&
		( nDay < 1 || nDay > 31 ) )
	{
		m_txtCNP.SetBkColor( COLOR_RED );
		return;
	}

	if ( nLen == 13 )
	{
		if ( theApp.m_pProgramData->IsCnpValid( pstrCnp->GetBuffer() ) )
			m_txtCNP.SetBkColor( COLOR_GREEN );
		else
		{
			m_txtCNP.SetBkColor( COLOR_RED );
			return;
		}
	}
	else if ( nLen > 13 )
	{
		m_txtCNP.SetBkColor( COLOR_RED );
		return;
	}

	//if ( pstrCnp->GetLength() > 3 || 
	//	(pstrCnp->GetLength() <= 3 && m_tblPatients.GetNumberRows() < theApp.m_pProgramData->GetPatients() + theApp.m_pProgramData->GetTempPatients()) )
		this->PostMessage( WM_UPDATE_PATIENTS_TABLE, (WPARAM)pstrCnp );
	//else
	//	delete pstrCnp;
}

void CFrmMain::OnBtnGo()
{
	CString *pstrCnp = new CString();

	m_txtCNP.GetWindowText( *pstrCnp );

	if ( !theApp.m_pProgramData->IsCnpValid( pstrCnp->GetBuffer() ) ||
		theApp.m_pProgramData->GetDisplayedPatients() > 0 )
	{
		ResetTxtCnp();
		CString strStatus = (CString)MAKEINTRESOURCE( STATUS_CNP_NOT_ADDED ); strStatus.Replace( TOREPLACE, *pstrCnp );
		SetStatus( strStatus );
		delete pstrCnp;

		return;
	}

	theApp.m_pWorkerThread->PostThreadMessage( WM_DIG_FOR_CNP, (WPARAM)pstrCnp, (LPARAM)TRUE );
}

LRESULT CFrmMain::OnUpdatePatientsTable( WPARAM wParam, LPARAM lParam )
{
	CString *pstrSuffix = wParam ? (CString*)wParam : new CString( L"" );
	int		index       = 0;

	m_tblPatients.SetNumberRows( 
		theApp.m_pProgramData->GetTempPatients() + 
		theApp.m_pProgramData->GetPatients(), FALSE );

	// Add temp patients
	CListPatients *list = theApp.m_pProgramData->GetTempPatientsList();
	POSITION	  pos   = list->GetHeadPosition();

	while ( pos )
	{
		PATIENT p = list->GetNext( pos );

		if ( pstrSuffix->IsEmpty() || p.strID.Left( pstrSuffix->GetLength() ) == *pstrSuffix )
			AddPatientToTable( index++, p.strID, p.strLastName, p.strFirstName, COLOR_SNOW );
	}

	//Add patients
	list = theApp.m_pProgramData->GetPatientsList();
	pos  = list->GetHeadPosition();

	while ( pos )
	{
		PATIENT p = list->GetNext( pos );

		if ( pstrSuffix->IsEmpty() || p.strID.Left( pstrSuffix->GetLength() ) == *pstrSuffix )
			AddPatientToTable( index++, p.strID, p.strLastName, p.strFirstName );
	}

	if ( index > 0 )
	{
		m_tblPatients.SetNumberRows( index );

		if ( !m_tblPatients.IsWindowVisible() )
			m_tblPatients.ShowWindow( TRUE );
	}
	else
	{
		if ( m_tblPatients.IsWindowVisible() )
			m_tblPatients.ShowWindow( FALSE );
	}

	// Update current number of patients
	theApp.m_pProgramData->SetDislayedPatients( index );
	pstrSuffix->Format( L"%d", index );
	m_lblPatients.SetWindowText( *pstrSuffix );

	m_txtCNP.SetFocus();

	delete pstrSuffix;

	return (LRESULT)0;
}

//////////////////////////////////////////////////////// Methods /////////////////////////////////////////////////////////

BOOL CFrmMain::AddPatientToTable( int nRow, CString strCNP, CString strLastName, CString strFirstName, COLORREF dwColor )
{
	// CNP
	m_tblPatients.QuickSetText( 0, nRow, strCNP );

	// Last name
	m_tblPatients.QuickSetText( 1, nRow, strLastName );

	// First name
	m_tblPatients.QuickSetText( 2, nRow, strFirstName );

	// Row color
	for ( int i = 0; i < 3; i++ )
		m_tblPatients.QuickSetBackColor( i, nRow, dwColor );

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