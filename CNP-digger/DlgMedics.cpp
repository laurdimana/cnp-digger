#include "stdafx.h"
#include "DlgMedics.h"
#include "CNP-digger.h"
#include "DlgCreateMedic.h"

BEGIN_MESSAGE_MAP( CDlgMedics, CDialogEx )
	ON_BN_CLICKED( IDOK,		   OnBtnSelect )
	ON_BN_CLICKED( MEDICS_BTN_ADD, OnBtnAdd )
	ON_BN_CLICKED( MEDICS_BTN_DEL, OnBtnDel )
END_MESSAGE_MAP()

////////////////////////////////////////////////// Constructor / Destructor /////////////////////////////////////////////////

CDlgMedics::CDlgMedics() : CDialogEx( CDlgMedics::IDD )
{
}

CDlgMedics::~CDlgMedics()
{
}

///////////////////////////////////////////////////// Overrides ////////////////////////////////////////////////////////////

void CDlgMedics::DoDataExchange( CDataExchange* pDX )
{
	CDialogEx::DoDataExchange( pDX );

	DDX_Control( pDX, MEDICS_CMB_MEDIC, m_cmbMedic );
	DDX_Control( pDX, IDOK,				m_btnSelect );
	DDX_Control( pDX, MEDICS_BTN_ADD,	m_btnAdd );
	DDX_Control( pDX, MEDICS_BTN_DEL,	m_btnDel );
	DDX_Control( pDX, IDCANCEL,			m_btnCancel );
}

BOOL CDlgMedics::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CMapStringToMedic::CPair *pCurVal = theApp.m_pProgramData->GetMedicsMap()->PGetFirstAssoc();
	m_cmbMedic.ResetContent();

	while ( pCurVal != NULL )
	{
		CString strMedic;

		strMedic.Format( L"%s %s %s", 
			pCurVal->value.strLastName, 
			pCurVal->value.strFirstName,
			pCurVal->value.strID );

		m_cmbMedic.AddString( strMedic );

		pCurVal = theApp.m_pProgramData->GetMedicsMap()->PGetNextAssoc( pCurVal );
	}

	return TRUE;
}

///////////////////////////////////////////////////////// Events ///////////////////////////////////////////////////////////////

void CDlgMedics::OnBtnSelect()
{
	if ( m_cmbMedic.GetWindowTextLength() > 0 )
	{
		CString strMedic;

		m_cmbMedic.GetWindowText( strMedic );
		strMedic = strMedic.Right( strMedic.GetLength() - strMedic.ReverseFind( L' ' ) - 1 );

		theApp.m_pProgramData->SetCurrentMedicID( strMedic );

		CDialogEx::OnOK();
	}
	else
	{
		AfxMessageBox( L"Please select a medic.", MB_ICONERROR );
	}
}

void CDlgMedics::OnBtnAdd()
{
	CDlgCreateMedic dlgCreateMedic;

	if ( dlgCreateMedic.DoModal() == IDOK )
	{
		TRACE( L"@ CDlgMedics::OnBtnAdd -> Create medic %s, %s, %s\n",
			dlgCreateMedic.GetID(), dlgCreateMedic.GetLastName(), dlgCreateMedic.GetFirstName() );

		theApp.m_pProgramData->AddMedic( dlgCreateMedic.GetID(), dlgCreateMedic.GetLastName(), dlgCreateMedic.GetFirstName() );

		MEDIC *m = new MEDIC;
		m->strID		= dlgCreateMedic.GetID();
		m->strLastName  = dlgCreateMedic.GetLastName();
		m->strFirstName = dlgCreateMedic.GetFirstName();

		theApp.m_pWorkerThread->PostThreadMessage( WM_ADD_MEDIC_TO_XML, (WPARAM)m, 0 );

		OnInitDialog();
	}
}

void CDlgMedics::OnBtnDel()
{
	if ( m_cmbMedic.GetWindowTextLength() > 0 )
	{
		CString strMedic;

		m_cmbMedic.GetWindowText( strMedic );
		strMedic = strMedic.Right( strMedic.GetLength() - strMedic.ReverseFind( L' ' ) - 1 );

		MEDIC medic = theApp.m_pProgramData->GetMedic( strMedic );

		CString strMsg;
		strMsg.Format( L"Delete medic %s %s %s ?", 
			medic.strLastName, medic.strFirstName, medic.strID );

		if ( AfxMessageBox( strMsg, MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{
			TRACE( L"@ CDlgMedics::OnBtnDel -> Delete medic %s %s %s\n",
				medic.strLastName, medic.strFirstName, medic.strID );

			theApp.m_pProgramData->DeleteMedic( medic.strID );

			MEDIC *m = new MEDIC;
			m->strID = medic.strID;
			m->strFirstName = medic.strFirstName;
			m->strLastName = medic.strLastName;
			theApp.m_pWorkerThread->PostThreadMessage( WM_DELETE_MEDIC_FROM_XML, (WPARAM)m, 0 );

			OnInitDialog();
		}
	}
	else
	{
		AfxMessageBox( L"Please select a medic.", MB_ICONERROR );
	}
}
