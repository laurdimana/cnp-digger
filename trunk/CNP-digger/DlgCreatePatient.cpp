#include "stdafx.h"
#include "DlgCreatePatient.h"

BEGIN_MESSAGE_MAP( CDlgCreatePatient, CDialogEx )
	ON_BN_CLICKED( IDOK, OnBtnOK )
END_MESSAGE_MAP()

////////////////////////////////////////////// Constructor / Destructor ////////////////////////////////////////////

CDlgCreatePatient::CDlgCreatePatient( CString strCNP ) : CDialogEx( CDlgCreatePatient::IDD )
{
	m_strCNP = strCNP;
}

CDlgCreatePatient::~CDlgCreatePatient()
{
}

///////////////////////////////////////////////////// Overrides ////////////////////////////////////////////////////////////

void CDlgCreatePatient::DoDataExchange( CDataExchange* pDX )
{
	CDialogEx::DoDataExchange( pDX );

	DDX_Text( pDX, DLG_CREATE_PATIENT_TXT_CNP,		  m_strCNP );
	DDX_Text( pDX, DLG_CREATE_PATIENT_TXT_LAST_NAME,  m_strLastName );
	DDX_Text( pDX, DLG_CREATE_PATIENT_TXT_FIRST_NAME, m_strFirstName );
	DDX_Text( pDX, DLG_CREATE_PATIENT_TXT_CITY,		  m_strCity );
	DDX_Control( pDX, IDOK,		m_btnOK );
	DDX_Control( pDX, IDCANCEL,	m_btnCancel );
}

BOOL CDlgCreatePatient::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;
}

///////////////////////////////////////////////////////// Events ///////////////////////////////////////////////////////////////

void CDlgCreatePatient::OnBtnOK()
{
	CDialogEx::UpdateData();

	if ( m_strCNP.Trim().IsEmpty() ||
		 m_strLastName.Trim().IsEmpty() ||
		 m_strFirstName.Trim().IsEmpty() ||
		 m_strCity.Trim().IsEmpty() )
	{
		AfxMessageBox( L"Please fill all the fields!", MB_ICONEXCLAMATION );

		return;
	}

	CDialogEx::OnOK();
}

////////////////////////////////////////////////////////// Methods ///////////////////////////////////////////////////////////////

CString CDlgCreatePatient::GetCNP()
{
	return m_strCNP.Trim();
}

CString CDlgCreatePatient::GetLastName()
{
	return m_strLastName.Trim();
}

CString CDlgCreatePatient::GetFirstName()
{
	return m_strFirstName.Trim();
}

CString CDlgCreatePatient::GetCity()
{
	return m_strCity.Trim();
}
