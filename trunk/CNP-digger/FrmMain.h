
// FrmMain.h : interface of the CFrmMain class
//

#pragma once

class CFrmMain : public CFrameWnd
{

// Constructor, Destructor
public:
	CFrmMain();
	virtual ~CFrmMain();

protected: 
	DECLARE_DYNAMIC( CFrmMain )

// Overrides
public:
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );	

// Methods
protected:
	BOOL AddPatientToTable( int nNo, CString strCNP, CString strLastName, CString strFirstName );

public:
	void SetStatus( CString strMsg );
	void SetWndTitle( CString strTitle );

// Members
protected:
	CFont m_fntSansSerif12;

	CStatusBar m_StatusBar;
	CEdit	   m_txtCNP;
	CListCtrl  m_tblPatients;
	CButton	   m_btnGo;

// Message map
protected:
	afx_msg int	 OnCreate( LPCREATESTRUCT lpCreateStruct );

	afx_msg void OnFileMedics();
	afx_msg void OnFileImport();
	afx_msg void OnFileExport();

	DECLARE_MESSAGE_MAP()
};


