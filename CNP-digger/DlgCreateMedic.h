#pragma once

#include "resource.h"

class CDlgCreateMedic : public CDialogEx
{
public:
	CDlgCreateMedic();
	virtual ~CDlgCreateMedic();

// Dialog Data
	enum { IDD = IDD_CREATE_MEDIC };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
	virtual BOOL OnInitDialog();

public:
	CString GetID();
	CString GetLastName();
	CString GetFirstName();

protected:
	CButton m_btnOK,
			m_btnCancel;

	CString m_strID,
			m_strLastName,
			m_strFirstName;

// Implementation
protected:
	afx_msg void OnBtnOK();

	DECLARE_MESSAGE_MAP()
};