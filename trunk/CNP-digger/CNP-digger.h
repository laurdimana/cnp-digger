
// CNP-digger.h : main header file for the CNP-digger application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "FrmMain.h"

class CCNPdiggerApp : public CWinApp
{
// Constructor
public:
	CCNPdiggerApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();

// Methods
protected:

// Members
public:
	CFrmMain *m_pFrmMain;

// Message map
public:
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()
};

extern CCNPdiggerApp theApp;
