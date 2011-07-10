#pragma once

#include "resource.h"

class CDlgCreateCity : public CDialogEx
{
public:
	CDlgCreateCity( CString strName = L"" );
	virtual ~CDlgCreateCity();

// Dialog Data
	enum { IDD = IDD_CREATE_CITY };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
	virtual BOOL OnInitDialog();

public:
	CString GetName();
	CString GetCode();

protected:
	CButton m_btnOK,
			m_btnCancel;

	CString m_strName,
			m_strCode;

// Implementation
protected:
	afx_msg void OnBtnOK();

	DECLARE_MESSAGE_MAP()
};