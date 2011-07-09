#pragma once

#include "resource.h"

class CDlgCreatePatient : public CDialogEx
{
public:
	CDlgCreatePatient( CString strCNP = L"" );
	virtual ~CDlgCreatePatient();

// Dialog Data
	enum { IDD = IDD_CREATE_PATIENT };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
	virtual BOOL OnInitDialog();

public:
	CString GetCNP();
	CString GetLastName();
	CString GetFirstName();
	CString GetCity();

protected:
	CButton m_btnOK,
			m_btnCancel;

	CString m_strCNP,
			m_strLastName,
			m_strFirstName,
			m_strCity;

// Implementation
protected:
	afx_msg void OnBtnOK();

	DECLARE_MESSAGE_MAP()
};