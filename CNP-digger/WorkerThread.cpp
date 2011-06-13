#include "stdafx.h"
#include "WorkerThread.h"
#include "CNP-digger.h"
#include "MedicsSAXContentHandler.h"
#include "CitiesSAXContentHandler.h"
#include "PatientsSAXContentHandler.h"
#include "SAXErrorHandler.h"

BEGIN_MESSAGE_MAP( CWorkerThread, CWinThread )
	ON_THREAD_MESSAGE( WM_CHECK_FOR_ESSENTIAL_FILES,	OnCheckForEssentialFiles )
	ON_THREAD_MESSAGE( WM_PARSE_MEDICS_XML,				OnParseMedicsXML )
	ON_THREAD_MESSAGE( WM_PARSE_CITIES_XML,				OnParseCitiesXML )
	ON_THREAD_MESSAGE( WM_INIT_MEDIC,					OnInitMedic )
	ON_THREAD_MESSAGE( WM_PARSE_PATIENTS_XML,			OnParsePatientsXML )
END_MESSAGE_MAP()

///////////////////////////////////////////////// Constructor / Destructor ///////////////////////////////////////////

CWorkerThread::CWorkerThread()
{
}

CWorkerThread::~CWorkerThread()
{
}

//////////////////////////////////////////////////// Overrides ///////////////////////////////////////////////////

BOOL CWorkerThread::InitInstance()
{
	TRACE( L"@ CWorkerThread::InitInstance -> Worker thread started\n" );
	return TRUE;
}

int CWorkerThread::ExitInstance()
{
	TRACE( L"@ CWorkerThread::InitInstance -> Worker thread stopped\n" );
	return 0;
}

//////////////////////////////////////////////////// Events ///////////////////////////////////////////////////////

void CWorkerThread::OnCheckForEssentialFiles( WPARAM wParam, LPARAM lParam )
{
	// Check for persons db
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetPersonsDB()).GetBuffer() ) )
	{
		TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Persons db not found\n" );
		theApp.m_pFrmMain->MessageBox( L"Persons database not found.", L"Error", MB_ICONERROR );
		theApp.m_pFrmMain->PostMessage( WM_CLOSE );
		return;
	}

	// Check for exports folder
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetExportsDir()).GetBuffer() ) )
	{
		if ( !::CreateDirectory( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetExportsDir()).GetBuffer(), NULL ) )
		{
			TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to create the Exports folder\n" );
			theApp.m_pFrmMain->MessageBox( L"Failed to create the Exports folder.", L"Error", MB_ICONERROR );
			theApp.m_pFrmMain->PostMessage( WM_CLOSE );
			return;
		}
	}

	// Check for patients folder
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetPatientsDir()).GetBuffer() ) )
	{
		if ( !::CreateDirectory( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetPatientsDir()).GetBuffer(), NULL ) )
		{
			TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to create the Patients folder\n" );
			theApp.m_pFrmMain->MessageBox( L"Failed to create the Patients folder.", L"Error", MB_ICONERROR );
			theApp.m_pFrmMain->PostMessage( WM_CLOSE );
			return;
		}
	}

	// Check for temp folder
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetTempDir()).GetBuffer() ) )
	{
		if ( !::CreateDirectory( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetTempDir()).GetBuffer(), NULL ) )
		{
			TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to create the Temp folder\n" );
			theApp.m_pFrmMain->MessageBox( L"Failed to create the Temp folder.", L"Error", MB_ICONERROR );
			theApp.m_pFrmMain->PostMessage( WM_CLOSE );
			return;
		}
	}

	// Check for cityes xml
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetCitiesXML()).GetBuffer() ) )
	{
		if ( !CreateGenericCitiesXml() )
		{
			TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to create cityes xml\n" );
			theApp.m_pFrmMain->MessageBox( L"Failed to create cityes xml.", L"Error", MB_ICONERROR );
			theApp.m_pFrmMain->PostMessage( WM_CLOSE );
			return;
		}
	}

	// Check for medics xml
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetMedicsXML()).GetBuffer() ) )
	{
		if ( !CreateGenericMedicsXml() )
		{
			TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to create medics xml\n" );
			theApp.m_pFrmMain->MessageBox( L"Failed to create medics xml.", L"Error", MB_ICONERROR );
			theApp.m_pFrmMain->PostMessage( WM_CLOSE );
			return;
		}
	}

	// Check for sqlite dll
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetSQLiteDLL()).GetBuffer() ) )
	{
		TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to find sqlite3 dll\n" );
		theApp.m_pFrmMain->MessageBox( L"Failed to find sqlite3 dll.", L"Error", MB_ICONERROR );
		theApp.m_pFrmMain->PostMessage( WM_CLOSE );
		return;
	}

	// Load sqlite dll
	if ( !theApp.m_pProgramData->LoadSQLite() )
	{
		TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to load functions from sqlite dll\n" );
		theApp.m_pFrmMain->MessageBox( L"Failed to load functions from sqlite dll.", L"Error", MB_ICONERROR );
		theApp.m_pFrmMain->PostMessage( WM_CLOSE );
		return;
	}

	OnParseCitiesXML();

	OnParseMedicsXML();

	theApp.m_pFrmMain->PostMessage( WM_COMMAND, MAKELONG( FRM_MAIN_MNU_FILE_MEDICS, 0 ) );
}

void CWorkerThread::OnParseMedicsXML( WPARAM wParam, LPARAM lParam )
{
	CoInitialize( NULL );
	ISAXXMLReader *pXMLReader = NULL;

	HRESULT hr = CoCreateInstance(
		__uuidof( SAXXMLReader ),
		NULL,
		CLSCTX_ALL,
		__uuidof( ISAXXMLReader ),
		(void **)&pXMLReader );

	if ( !FAILED( hr ) )
	{
		MedicsSAXContentHandler *pMedicsHandler = new MedicsSAXContentHandler();
		hr = pXMLReader->putContentHandler( pMedicsHandler );

		SAXErrorHandler *pErrorHandler = new SAXErrorHandler();
		hr = pXMLReader->putErrorHandler( pErrorHandler );

		hr = pXMLReader->parseURL( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetMedicsXML()).GetBuffer() );

		pXMLReader->Release();
	}

	CoUninitialize();
}

void CWorkerThread::OnParseCitiesXML( WPARAM wParam, LPARAM lParam )
{
	CoInitialize( NULL );
	ISAXXMLReader *pXMLReader = NULL;

	HRESULT hr = CoCreateInstance(
		__uuidof( SAXXMLReader ),
		NULL,
		CLSCTX_ALL,
		__uuidof( ISAXXMLReader ),
		(void **)&pXMLReader );

	if ( !FAILED( hr ) )
	{
		CitiesSAXContentHandler *pCitiesHandler = new CitiesSAXContentHandler();
		hr = pXMLReader->putContentHandler( pCitiesHandler );

		SAXErrorHandler *pErrorHandler = new SAXErrorHandler();
		hr = pXMLReader->putErrorHandler( pErrorHandler );

		hr = pXMLReader->parseURL( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetCitiesXML()).GetBuffer() );

		pXMLReader->Release();
	}

	CoUninitialize();
}

void CWorkerThread::OnInitMedic( WPARAM wParam, LPARAM lParam )
{
	theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_LOADING_MEDIC ) + L" " + 
		theApp.m_pProgramData->GetCurrentMedic().strLastName + L" " +
		theApp.m_pProgramData->GetCurrentMedic().strFirstName + L"..." );

	// Check for patients xml
	CString strPath = theApp.m_pProgramData->GetCurrentDir() + L"\\" + PATIENTS_DIR + L"\\" +
		theApp.m_pProgramData->GetCurrentMedic().strLastName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strFirstName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strID + L".xml";

	if ( ::PathFileExists( strPath ) )
	{
		wchar_t *pszPath = new wchar_t[ strPath.GetLength() + 1 ];
		wcscpy_s( pszPath, strPath.GetLength() + 1, strPath.GetBuffer() );

		OnParsePatientsXML( (WPARAM)pszPath );
	}

	// Check for temp patients xml
	strPath = theApp.m_pProgramData->GetCurrentDir() + L"\\" + TEMP_DIR + L"\\" +
		theApp.m_pProgramData->GetCurrentMedic().strLastName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strFirstName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strID + L"-" + 
		TEMP_DIR + L".xml";

	if ( ::PathFileExists( strPath )  )
	{
		wchar_t *pszPath = new wchar_t[ strPath.GetLength() + 1 ];
		wcscpy_s( pszPath, strPath.GetLength() + 1, strPath.GetBuffer() );

		OnParsePatientsXML( (WPARAM)pszPath, TRUE );
	}

	theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_LOADED_MEDIC ) + L" " + 
		theApp.m_pProgramData->GetCurrentMedic().strLastName + L" " +
		theApp.m_pProgramData->GetCurrentMedic().strFirstName );

	theApp.m_pFrmMain->PostMessage( WM_UPDATE_PATIENTS_TABLE );
}

void CWorkerThread::OnParsePatientsXML( WPARAM wParam, LPARAM lParam )
{
	wchar_t *pszXml = (wchar_t*)wParam;
	BOOL	bTemp   = (BOOL)lParam;

	CoInitialize( NULL );
	ISAXXMLReader *pXMLReader = NULL;

	HRESULT hr = CoCreateInstance(
		__uuidof( SAXXMLReader ),
		NULL,
		CLSCTX_ALL,
		__uuidof( ISAXXMLReader ),
		(void **)&pXMLReader );

	if ( !FAILED( hr ) )
	{
		PatientsSAXContentHandler *pPatientsHandler = new PatientsSAXContentHandler( bTemp );
		hr = pXMLReader->putContentHandler( pPatientsHandler );

		SAXErrorHandler *pErrorHandler = new SAXErrorHandler();
		hr = pXMLReader->putErrorHandler( pErrorHandler );

		hr = pXMLReader->parseURL( pszXml );

		pXMLReader->Release();
	}

	delete [] pszXml;
	CoUninitialize();
}

//////////////////////////////////////////////////// Methods //////////////////////////////////////////////////////////

BOOL CWorkerThread::CreateGenericCitiesXml()
{
	CFile fXml;

	if ( !fXml.Open( 
		theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetCitiesXML(), 
		CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ) )
	{
		return FALSE;
	}

	CString strXml;
	strXml.Format( L"%s\n<%s>\n</%s>", XML_HEADER, XML_CITIES, XML_CITIES );

	int  nUTF8    = strXml.GetLength()*2;
	char *pszUTF8 = new char[ nUTF8 ];
	theApp.m_pProgramData->ToUTF8( strXml.GetBuffer(), strXml.GetLength(), pszUTF8, &nUTF8 );
	fXml.Write( pszUTF8, nUTF8 );
	delete [] pszUTF8;

	fXml.Close();

	return TRUE;
}

BOOL CWorkerThread::CreateGenericMedicsXml()
{
	CFile fXml;

	if ( !fXml.Open( 
		theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetMedicsXML(), 
		CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ) )
	{
		return FALSE;
	}

	CString strXml;
	strXml.Format( L"%s\n<%s>\n</%s>", XML_HEADER, XML_MEDICS, XML_MEDICS );

	int  nUTF8    = strXml.GetLength()*2;
	char *pszUTF8 = new char[ nUTF8 ];
	theApp.m_pProgramData->ToUTF8( strXml.GetBuffer(), strXml.GetLength(), pszUTF8, &nUTF8 );
	fXml.Write( pszUTF8, nUTF8 );
	delete [] pszUTF8;

	fXml.Close();

	return TRUE;
}
