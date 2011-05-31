#include "stdafx.h"
#include "DlgMedics.h"
#include "CNP-digger.h"

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

		theApp.m_pProgramData->SetCurrentMedic( theApp.m_pProgramData->GetMedic( strMedic ) );

		CDialogEx::OnOK();
	}
	else
	{
		AfxMessageBox( L"Please select a medic.", MB_ICONERROR );
	}
}

void CDlgMedics::OnBtnAdd()
{
}

void CDlgMedics::OnBtnDel()
{
}
