
// FrmMain.h : interface of the CFrmMain class
//

#pragma once

#include "MyCUG.h"
#include "OXEdit.h"

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
	virtual BOOL PreTranslateMessage( MSG *pMsg );

// Methods
protected:
	BOOL AddPatientToTable( int nRow, CString strCNP, CString strLastName, CString strFirstName, COLORREF dwColor = COLOR_WHITE );

public:
	void SetStatus( CString strMsg );
	void SetWndTitle( CString strTitle );
	void ResetTxtCnp();

// Members
protected:
	CFont m_fntSansSerif12;

	CStatusBar m_StatusBar;
	COXEdit	   m_txtCNP;
	MyCug	   m_tblPatients;
	CButton	   m_btnGo;
	CStatic	   m_lblPatients;

// Message map
protected:
	afx_msg int	 OnCreate( LPCREATESTRUCT lpCreateStruct );

	afx_msg void OnFileMedics();
	afx_msg void OnFileImport();
	afx_msg void OnFileExport();

	afx_msg void OnTxtCnpChange();
	afx_msg void OnBtnGo();

	afx_msg LRESULT OnUpdatePatientsTable( WPARAM wParam = NULL, LPARAM lParam = NULL );

	DECLARE_MESSAGE_MAP()
};


