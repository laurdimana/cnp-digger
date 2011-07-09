#include "stdafx.h"
#include "DlgCreateMedic.h"

BEGIN_MESSAGE_MAP( CDlgCreateMedic, CDialogEx )
	ON_BN_CLICKED( IDOK, OnBtnOK )
END_MESSAGE_MAP()

////////////////////////////////////////////// Constructor / Destructor ////////////////////////////////////////////

CDlgCreateMedic::CDlgCreateMedic() : CDialogEx( CDlgCreateMedic::IDD )
{
}

CDlgCreateMedic::~CDlgCreateMedic()
{
}

///////////////////////////////////////////////////// Overrides ////////////////////////////////////////////////////////////

void CDlgCreateMedic::DoDataExchange( CDataExchange* pDX )
{
	CDialogEx::DoDataExchange( pDX );

	DDX_Text( pDX, DLG_CREATE_MEDIC_TXT_ID,		    m_strID );
	DDX_Text( pDX, DLG_CREATE_MEDIC_TXT_LAST_NAME,  m_strLastName );
	DDX_Text( pDX, DLG_CREATE_MEDIC_TXT_FIRST_NAME, m_strFirstName );
	DDX_Control( pDX, IDOK,		m_btnOK );
	DDX_Control( pDX, IDCANCEL,	m_btnCancel );
}

BOOL CDlgCreateMedic::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;
}

///////////////////////////////////////////////////////// Events ///////////////////////////////////////////////////////////////

void CDlgCreateMedic::OnBtnOK()
{
	CDialogEx::UpdateData();

	if ( m_strID.Trim().IsEmpty() ||
		 m_strLastName.Trim().IsEmpty() ||
		 m_strFirstName.Trim().IsEmpty() )
	{
		AfxMessageBox( L"Please fill all the fields!", MB_ICONEXCLAMATION );

		return;
	}

	CDialogEx::OnOK();
}

////////////////////////////////////////////////////////// Methods ///////////////////////////////////////////////////////////////

CString CDlgCreateMedic::GetID()
{
	return m_strID;
}

CString CDlgCreateMedic::GetLastName()
{
	return m_strLastName;
}

CString CDlgCreateMedic::GetFirstName()
{
	return m_strFirstName;
}
