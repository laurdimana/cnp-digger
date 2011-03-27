
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

// Members
protected:
	CStatusBar m_StatusBar;

// Message map
protected:
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );

	DECLARE_MESSAGE_MAP()
};


