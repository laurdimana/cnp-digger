
// FrmMain.cpp : implementation of the CFrmMain class
//

#include "stdafx.h"
#include "CNP-digger.h"
#include "FrmMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFrmMain

IMPLEMENT_DYNAMIC( CFrmMain, CFrameWnd )

BEGIN_MESSAGE_MAP( CFrmMain, CFrameWnd )
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR
};

////////////////////////////////////////////// Constructor / Destructor //////////////////////////////////////////////////////

CFrmMain::CFrmMain()
{
}

CFrmMain::~CFrmMain()
{
}

///////////////////////////////////////////////////// Ovverides ///////////////////////////////////////////////////////////

BOOL CFrmMain::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow( cs ) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(	
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, 
		::LoadCursor( NULL, IDC_ARROW ), 
		reinterpret_cast<HBRUSH>(COLOR_3DFACE+1), 
		::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDR_MAINFRAME ) ) );

	return TRUE;
}

//////////////////////////////////////////////////// Events ///////////////////////////////////////////////////////////////

int CFrmMain::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	if ( !m_StatusBar.Create( this ) )
	{
		TRACE0( "Failed to create status bar\n" );
		return -1;
	}

	m_StatusBar.SetIndicators( indicators, sizeof(indicators)/sizeof(UINT) );

	return 0;
}

//////////////////////////////////////////////////////// Methods /////////////////////////////////////////////////////////
