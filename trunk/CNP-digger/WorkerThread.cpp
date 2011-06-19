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
	ON_THREAD_MESSAGE( WM_DIG_FOR_CNP,					OnDigForCnp )
	ON_THREAD_MESSAGE( WM_ADD_TEMP_PATIENT_TO_XML,		OnAddTempPatientToXml )
END_MESSAGE_MAP()

////////////////////////////////////////////////// Static ///////////////////////////////////////////////////////////

static int SQLiteCallbackAdapter( void *NotUsed, int argc, char **argv, char **coln )
{
	return theApp.m_pWorkerThread->SQLiteCallback( NotUsed, argc, argv, coln );
}

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

	// Clear current data
	theApp.m_pProgramData->GetPatientsList()->RemoveAll();
	theApp.m_pProgramData->GetTempPatientsList()->RemoveAll();

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

void CWorkerThread::OnDigForCnp( WPARAM wParam, LPARAM lParam )
{
	CString strCnp = (wchar_t*)wParam;
	BOOL	bCount = (BOOL)lParam;

	delete [] (wchar_t*)wParam;

	CString strQuery;

	if ( bCount )
	{
		strQuery.Format( L"SELECT COUNT(*), %s FROM %s WHERE %s='%s'", 
			strCnp,
			DB_TABLE_NAME, 
			DB_CNP, strCnp );
	}
	else
	{
		strQuery.Format( L"SELECT %s, %s, %s, %s FROM %s WHERE %s='%s'",
			DB_CNP, DB_LAST_NAME, DB_FIRST_NAME, DB_CITY,
			DB_TABLE_NAME,
			DB_CNP, strCnp );
	}

	TRACE( L"@ CWorkerThread::OnDigForCnp -> %s\n", strQuery );

	theApp.m_pProgramData->SQLiteExec( strQuery, SQLiteCallbackAdapter );
}

void CWorkerThread::OnAddTempPatientToXml( WPARAM wParam, LPARAM lParam )
{
	PATIENT *p = (PATIENT*)wParam;

	// Check for temp patients xml
	CString strPath = theApp.m_pProgramData->GetCurrentDir() + L"\\" + TEMP_DIR + L"\\" +
		theApp.m_pProgramData->GetCurrentMedic().strLastName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strFirstName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strID + L"-" + 
		TEMP_DIR + L".xml";

	if ( !::PathFileExists( strPath )  )
	{
		if ( !CreateGeneticTempPatientsXml() )
		{
			TRACE( L"@ CWorkerThread::OnAddTempPatientToXml -> Failed CreateGeneticTempPatientsXml\n" );
			delete p;

			return;
		}
	}

	// Add temp person to xml
	CFile fXml;

	if ( !fXml.Open( strPath, CFile::modeReadWrite | CFile::typeBinary ) )
	{
		TRACE( L"@ CWorkerThread::OnAddTempPatientToXml -> Failed fXml.Open\n" );
		delete p;

		return;
	}

	CString strCnp( p->strID );
	CString strLastName( p->strLastName );
	CString strFirstName( p->strFirstName );
	CString strCityCode( p->strCityCode );

	delete p;

	CString strGender( strCnp.Left( 1 ) );

	CString strBirthDate( strGender == "1" || strGender == "2" ? L"19" : L"20" );
	strBirthDate.Append( strCnp.Mid( 1, 2 ) + "-" + strCnp.Mid( 3, 2 ) + "-" + strCnp.Mid( 5, 2 ) );

	// What to write
	CString strXml;
	strXml.Format( L"\t<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\">\n\t\t<%s %s=\"%s\" %s=\"%s\"/>\n\t</%s>\n</%s>",
		XML_PERSON, 
		XML_PERSON_PID, strCnp, 
		XML_PERSON_FIRST_NAME, strFirstName,
		XML_PERSON_LAST_NAME, strLastName,
		XML_PERSON_GENDER, strGender,
		XML_PERSON_BIRTH_DATE, strBirthDate,
		XML_PERSON_UNIT, XML_UNIT_OF_PAYMENT,
		XML_PERSON_ADDRESS,
		XML_PERSON_ADDRESS_CITY_CODE, strCityCode,
		XML_PERSON_ADDRESS_DISTRICT_CODE, XML_DISTRICT_CODE,
		XML_PERSON,
		XML_PERSONS );

	// Convert to UTF8
	int  nUTF8    = strXml.GetLength()*2;
	char *pszUTF8 = new char[ nUTF8 ];
	theApp.m_pProgramData->ToUTF8( strXml.GetBuffer(), strXml.GetLength(), pszUTF8, &nUTF8 );

	TRACE( "@ CWorkerThread::OnAddTempPatientToXml -> Write \n%S\n", strXml );

	// Where to write
	LONGLONG lOffset = 0;
	char	 c;

	do
	{
		lOffset -= 1;
		fXml.Seek( lOffset, CFile::end );
		fXml.Read( &c, 1 );
	}
	while ( c != '>' );

	// Write
	lOffset++;
	lOffset -= (wcslen( XML_PERSONS ) + 3); // </> = 3
	fXml.Seek( lOffset, CFile::end );
	fXml.Write( pszUTF8, nUTF8 );

	// Cleanup
	delete [] pszUTF8;
	fXml.Close();
}

//////////////////////////////////////////////////// Methods //////////////////////////////////////////////////////////

int CWorkerThread::SQLiteCallback( void *NotUsed, int argc, char **argv, char **coln )
{
	if ( argc == 2 ) // count
	{
		int		nCount = atoi( argv[ 0 ] );
		CString strCnp( argv[ 1 ] );

		if ( nCount )
		{
			wchar_t *pszCnp = new wchar_t[ strCnp.GetLength() + 1 ];
			wcscpy_s( pszCnp, strCnp.GetLength() + 1, strCnp.GetBuffer() );

			this->PostThreadMessage( WM_DIG_FOR_CNP, (WPARAM)pszCnp, (LPARAM)FALSE );
		}
		else
		{
			theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_CNP_NOT_ADDED ) );
			theApp.m_pFrmMain->ResetTxtCnp();
		}
	}
	else // Create patient
	{
		CString strCnp( argv[ 0 ] );
		CString strLastName( argv[ 1 ] );
		CString strFirstName( argv[ 2 ] );
		CString strCity( argv[ 3 ] );

		TRACE( L"@ CWorkerThread::SQLiteCallback -> Add %s, %s, %s, %s, %s\n", 
			strCnp, strLastName, strFirstName, strCity, theApp.m_pProgramData->GetCity( strCity ).strID );

		// Add temp patient to list
		theApp.m_pProgramData->AddPatientTemp( strCnp, strLastName, strFirstName, theApp.m_pProgramData->GetCity( strCity ).strID );

		// Add temp patient to xml
		PATIENT *p = new PATIENT;
		p->strID		= strCnp;
		p->strLastName  = strLastName;
		p->strFirstName = strFirstName;
		p->strCityCode  = theApp.m_pProgramData->GetCity( strCity ).strID;

		OnAddTempPatientToXml( (WPARAM)p, 0 );

		theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_CNP_ADDED ) );
		theApp.m_pFrmMain->ResetTxtCnp();
	}

	return 0;
}

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

BOOL CWorkerThread::CreateGeneticTempPatientsXml()
{
	CFile fXml;

	if ( !fXml.Open( 
		theApp.m_pProgramData->GetCurrentDir() + L"\\" + TEMP_DIR + L"\\" + 
			theApp.m_pProgramData->GetCurrentMedic().strLastName + L"-" +
			theApp.m_pProgramData->GetCurrentMedic().strFirstName + L"-" +
			theApp.m_pProgramData->GetCurrentMedic().strID + L"-" + 
			TEMP_DIR + L".xml", 
		CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ) )
	{
		return FALSE;
	}

	CString strXml;
	strXml.Format( L"%s\n<%s %s>\n</%s>", XML_HEADER, XML_PERSONS, XML_PERSONS_XMLNS, XML_PERSONS );

	int  nUTF8    = strXml.GetLength()*2;
	char *pszUTF8 = new char[ nUTF8 ];
	theApp.m_pProgramData->ToUTF8( strXml.GetBuffer(), strXml.GetLength(), pszUTF8, &nUTF8 );
	fXml.Write( pszUTF8, nUTF8 );
	delete [] pszUTF8;

	fXml.Close();

	return TRUE;
}