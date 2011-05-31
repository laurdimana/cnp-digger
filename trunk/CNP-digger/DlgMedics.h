#pragma once

#include "resource.h"

class CDlgMedics : public CDialogEx
{
public:
	CDlgMedics();
	~CDlgMedics();

// Dialog Data
	enum { IDD = IDD_MEDICS };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	CButton m_btnSelect,
			m_btnAdd,
			m_btnDel,
			m_btnCancel;

	CComboBox m_cmbMedic;

// Implementation
protected:
	afx_msg void OnBtnSelect();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnDel();

	DECLARE_MESSAGE_MAP()
};