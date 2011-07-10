#include "stdafx.h"
#include "DlgCreateCity.h"

BEGIN_MESSAGE_MAP( CDlgCreateCity, CDialogEx )
	ON_BN_CLICKED( IDOK, OnBtnOK )
END_MESSAGE_MAP()

////////////////////////////////////////////// Constructor / Destructor ////////////////////////////////////////////

CDlgCreateCity::CDlgCreateCity( CString strName ) : CDialogEx( CDlgCreateCity::IDD )
{
	m_strName = strName;
}

CDlgCreateCity::~CDlgCreateCity()
{
}

///////////////////////////////////////////////////// Overrides ////////////////////////////////////////////////////////////

void CDlgCreateCity::DoDataExchange( CDataExchange* pDX )
{
	CDialogEx::DoDataExchange( pDX );

	DDX_Text( pDX, DLG_CREATE_CITY_TXT_NAME,  m_strName );
	DDX_Text( pDX, DLG_CREATE_CITY_TXT_CODE,  m_strCode );
	DDX_Control( pDX, IDOK,		m_btnOK );
	DDX_Control( pDX, IDCANCEL,	m_btnCancel );
}

BOOL CDlgCreateCity::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;
}

///////////////////////////////////////////////////////// Events ///////////////////////////////////////////////////////////////

void CDlgCreateCity::OnBtnOK()
{
	CDialogEx::UpdateData();

	if ( m_strName.Trim().IsEmpty() ||
		 m_strName.Trim().IsEmpty() )
	{
		AfxMessageBox( L"Please fill all the fields!", MB_ICONEXCLAMATION );

		return;
	}

	CDialogEx::OnOK();
}

////////////////////////////////////////////////////////// Methods ///////////////////////////////////////////////////////////////

CString CDlgCreateCity::GetName()
{
	return m_strName.Trim();
}

CString CDlgCreateCity::GetCode()
{
	return m_strCode.Trim();
}
