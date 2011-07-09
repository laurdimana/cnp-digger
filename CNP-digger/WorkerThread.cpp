#include "stdafx.h"
#include "WorkerThread.h"
#include "CNP-digger.h"
#include "MedicsSAXContentHandler.h"
#include "CitiesSAXContentHandler.h"
#include "PatientsSAXContentHandler.h"
#include "SAXErrorHandler.h"
#include "DlgCreatePatient.h"

BEGIN_MESSAGE_MAP( CWorkerThread, CWinThread )
	ON_THREAD_MESSAGE( WM_CHECK_FOR_ESSENTIAL_FILES,	OnCheckForEssentialFiles )
	ON_THREAD_MESSAGE( WM_PARSE_MEDICS_XML,				OnParseMedicsXML )
	ON_THREAD_MESSAGE( WM_PARSE_CITIES_XML,				OnParseCitiesXML )
	ON_THREAD_MESSAGE( WM_INIT_MEDIC,					OnInitMedic )
	ON_THREAD_MESSAGE( WM_PARSE_PATIENTS_XML,			OnParsePatientsXML )
	ON_THREAD_MESSAGE( WM_DIG_FOR_CNP,					OnDigForCnp )
	ON_THREAD_MESSAGE( WM_ADD_TEMP_PATIENT_TO_XML,		OnAddTempPatientToXml )
	ON_THREAD_MESSAGE( WM_IMPORT_PATIENTS_XML,			OnImportPatientsXml )
	ON_THREAD_MESSAGE( WM_EXPORT_TEMP_PATIENTS,			OnExportTempPatients )
	ON_THREAD_MESSAGE( WM_ADD_MEDIC_TO_XML,				OnAddMedicToXml )
	ON_THREAD_MESSAGE( WM_DELETE_MEDIC_FROM_XML,		OnDeleteMedicFromXml )
	ON_THREAD_MESSAGE( WM_REWRITE_MEDICS_XML,			OnRewriteMedicsXml )
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
		CString *pstrPath = new CString( strPath );

		OnParsePatientsXML( (WPARAM)pstrPath );
	}

	// Check for temp patients xml
	strPath = theApp.m_pProgramData->GetCurrentDir() + L"\\" + TEMP_DIR + L"\\" +
		theApp.m_pProgramData->GetCurrentMedic().strLastName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strFirstName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strID + L"-" + 
		TEMP_DIR + L".xml";

	if ( ::PathFileExists( strPath )  )
	{
		CString *pstrPath = new CString( strPath );

		OnParsePatientsXML( (WPARAM)pstrPath, TRUE );
	}

	theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_LOADED_MEDIC ) + L" " + 
		theApp.m_pProgramData->GetCurrentMedic().strLastName + L" " +
		theApp.m_pProgramData->GetCurrentMedic().strFirstName );

	theApp.m_pFrmMain->PostMessage( WM_UPDATE_PATIENTS_TABLE );
}

void CWorkerThread::OnParsePatientsXML( WPARAM wParam, LPARAM lParam )
{
	CString *pstrXml = (CString*)wParam;
	BOOL	bTemp    = (BOOL)lParam;

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

		hr = pXMLReader->parseURL( pstrXml->GetBuffer() );

		pXMLReader->Release();
	}

	delete pstrXml;
	CoUninitialize();
}

void CWorkerThread::OnDigForCnp( WPARAM wParam, LPARAM lParam )
{
	CString *pstrCnp = (CString*)wParam;
	BOOL	bCount   = (BOOL)lParam;

	CString strQuery;

	if ( bCount )
	{
		strQuery.Format( L"SELECT COUNT(*), %s FROM %s WHERE %s='%s'", 
			*pstrCnp,
			DB_TABLE_NAME, 
			DB_CNP, *pstrCnp );
	}
	else
	{
		strQuery.Format( L"SELECT %s, %s, %s, %s FROM %s WHERE %s='%s'",
			DB_CNP, DB_LAST_NAME, DB_FIRST_NAME, DB_CITY,
			DB_TABLE_NAME,
			DB_CNP, *pstrCnp );
	}

	delete pstrCnp;

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
			theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_CNP_NOT_ADDED ) );

			return;
		}
	}

	// Add temp person to xml
	CFile fXml;

	if ( !fXml.Open( strPath, CFile::modeReadWrite | CFile::typeBinary ) )
	{
		TRACE( L"@ CWorkerThread::OnAddTempPatientToXml -> Failed fXml.Open [%ld]\n", GetLastError() );
		delete p;
		theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_CNP_NOT_ADDED ) );

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
	TRACE( "@ CWorkerThread::OnAddTempPatientToXml -> Write \n%S\n", strXml );

	// Convert to UTF8
	int  nUTF8    = strXml.GetLength()*2;
	char *pszUTF8 = new char[ nUTF8 ];
	theApp.m_pProgramData->ToUTF8( strXml.GetBuffer(), strXml.GetLength(), pszUTF8, &nUTF8 );

	char *pszPersTag = new char[ MAX_PATH ];
	int  nPersTag    = wcslen( XML_PERSONS ) + 3;

	sprintf_s( pszPersTag, MAX_PATH, "</%S>", XML_PERSONS );
	
	if ( !theApp.m_pProgramData->PositionInFile( &fXml, pszPersTag, nPersTag, CFile::end, TRUE ) )
	{
		TRACE( L"@ CWorkerThread::OnAddTempPatientToXml -> Failed PositionInFile\n" );
		theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_CNP_NOT_ADDED ) );

		delete [] pszPersTag;
		delete [] pszUTF8;
		fXml.Close();

		return;
	}
		
	fXml.Write( pszUTF8, nUTF8 );

	// Add temp patient to list
	theApp.m_pProgramData->AddPatientTemp( strCnp, strLastName, strFirstName, strCityCode );

	// Cleanup
	delete [] pszPersTag;
	delete [] pszUTF8;
	fXml.Close();

	theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_CNP_ADDED ) );
}

void CWorkerThread::OnImportPatientsXml( WPARAM wParam, LPARAM lParam )
{
	CString *pstrSrc = (CString*)wParam;
	CString *pstrDst = (CString*)lParam;

	if ( ::CopyFile( *pstrSrc, *pstrDst, FALSE ) )
	{
		theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_PATIENTS_IMPORTED ) );
		OnInitMedic();
	}
	else
	{
		TRACE( L"@ CWorkerThread::OnImportPatientsXml -> Failed CopyFile [%ld]\n", GetLastError() );
		theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_PATIENTS_NOT_IMPORTED ) );
	}

	delete pstrSrc;
	delete pstrDst;
}

void CWorkerThread::OnExportTempPatients( WPARAM wParam, LPARAM lParam )
{
	CString strTemp = theApp.m_pProgramData->GetCurrentDir() + L"\\" + TEMP_DIR + L"\\" +
		theApp.m_pProgramData->GetCurrentMedic().strLastName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strFirstName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strID + L"-" + 
		TEMP_DIR + L".xml";

	if ( !::PathFileExists( strTemp )  )
	{
		TRACE( L"@ CWorkerThread::OnExportTempPatients -> Temp patient list not found\n" );
		theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_PATIENTS_NOT_EXPORTED ) );

		return;
	}

	// Get current time
	SYSTEMTIME st;
	::ZeroMemory( &st, sizeof( st ) );
	::GetLocalTime( &st );
	CString strTimestamp;
	strTimestamp.Format( L"%d-%02d-%02d-%02d-%02d-%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );

	CString strExport = theApp.m_pProgramData->GetCurrentDir() + L"\\" + EXPORTS_DIR + L"\\" +
		theApp.m_pProgramData->GetCurrentMedic().strLastName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strFirstName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strID + L"-" + 
		strTimestamp + L"-" +
		EXPORTED_XML + L".xml";

	if ( !::CopyFile( strTemp, strExport, FALSE ) )
	{
		TRACE( L" Failed CopyFile [%ld]\n", GetLastError() );
		theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_PATIENTS_NOT_EXPORTED ) );

		return;
	}

	if ( !::DeleteFile( strTemp ) )
		TRACE( L"@ CWorkerThread::OnExportTempPatients -> Failed DeleteFile [%ld]\n", GetLastError() );

	// Merge exported and current patients
	CFile fExportXml, fXml;

	if ( !fExportXml.Open( strExport, CFile::modeRead | CFile::typeBinary ) )
	{
		TRACE( L"@ CWorkerThread::OnExportTempPatients -> Failed fExportXml.Open [%ld]\n", GetLastError() );
		theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_PATIENTS_NOT_EXPORTED ) );

		return;
	}

	CString strXml = theApp.m_pProgramData->GetCurrentDir() + L"\\" + PATIENTS_DIR + L"\\" +
		theApp.m_pProgramData->GetCurrentMedic().strLastName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strFirstName + L"-" + 
		theApp.m_pProgramData->GetCurrentMedic().strID + L".xml";

	if ( !::PathFileExists( strXml )  )
	{
		if ( !CreateGeneticPatientsXml() )
		{
			TRACE( L"@ CWorkerThread::OnExportTempPatients -> Failed CreateGeneticPatientsXml\n" );
			theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_PATIENTS_NOT_EXPORTED ) );

			return;
		}
	}

	if ( !fXml.Open( strXml, CFile::modeReadWrite | CFile::typeBinary ) )
	{
		TRACE( L"@ CWorkerThread::OnExportTempPatients -> Failed fXml.Open [%ld]\n", GetLastError() );
		theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_PATIENTS_NOT_EXPORTED ) );

		return;
	}

	// Position in export xml
	char *pszPersonTag = new char[ MAX_PATH ];
	int  nPersonTag    = wcslen( XML_PERSON ) + 1;

	sprintf_s( pszPersonTag, MAX_PATH, "<%S", XML_PERSON );
	
	if ( !theApp.m_pProgramData->PositionInFile( &fExportXml, pszPersonTag, nPersonTag, CFile::begin ) || 
		!theApp.m_pProgramData->PositionInFile( &fExportXml, pszPersonTag, nPersonTag, CFile::current, FALSE, nPersonTag ) )
	{
		TRACE( L"@ CWorkerThread::OnExportTempPatients -> Failed PositionInFile fExportXml\n" );
		theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_PATIENTS_NOT_EXPORTED ) );

		delete [] pszPersonTag;
		fExportXml.Close();
		fXml.Close();
		return;
	}

	delete [] pszPersonTag;

	// Position in main xml
	char *pszPersTag = new char[ MAX_PATH ];
	int  nPersTag    = wcslen( XML_PERSONS ) + 3;

	sprintf_s( pszPersTag, MAX_PATH, "</%S>", XML_PERSONS );
	
	if ( !theApp.m_pProgramData->PositionInFile( &fXml, pszPersTag, nPersTag, CFile::end, TRUE ) )
	{
		TRACE( L"@ CWorkerThread::OnExportTempPatients -> Failed PositionInFile fXml\n" );
		theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_PATIENTS_NOT_EXPORTED ) );

		delete [] pszPersTag;
		fExportXml.Close();
		fXml.Close();
		return;
	}

	delete [] pszPersTag;

	// Read from export xml and write in main xml
	static int nBufferSize = 4096;

	char *pBuffer = new char[ nBufferSize ];
	int  nRead    = 0;

	do
	{
		nRead = fExportXml.Read( pBuffer, nBufferSize );
		
		if ( nRead > 0 )
			fXml.Write( pBuffer, nRead );
	}
	while ( nRead > 0 );

	// Cleanup
	delete [] pBuffer;
	fExportXml.Close();
	fXml.Close();

	theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_PATIENTS_EXPORTED ) );

	OnInitMedic();
}

void CWorkerThread::OnAddMedicToXml( WPARAM wParam, LPARAM lParam )
{
	MEDIC *medic = (MEDIC*)wParam;

	// Check for medics xml
	CString strPath = theApp.m_pProgramData->GetCurrentDir() + L"\\" + 
		theApp.m_pProgramData->GetMedicsXML();

	if ( !::PathFileExists( strPath )  )
	{
		if ( !CreateGenericMedicsXml() )
		{
			TRACE( L"@ CWorkerThread::OnAddMedicToXml -> Failed CreateGenericMedicsXml\n" );
			delete medic;

			return;
		}
	}

	// Add medic to xml
	CFile fXml;

	if ( !fXml.Open( strPath, CFile::modeReadWrite | CFile::typeBinary ) )
	{
		TRACE( L"@ CWorkerThread::OnAddMedicToXml -> Failed fXml.Open [%ld]\n", GetLastError() );
		delete medic;

		return;
	}

	// What to write
	CString strXml;
	strXml.Format( L"\t<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" />\n</%s>",
		XML_MEDIC, 
		XML_MEDIC_LAST_NAME, medic->strLastName, 
		XML_MEDIC_FIRST_NAME, medic->strFirstName,
		XML_MEDIC_ID, medic->strID,
		XML_MEDICS );
	TRACE( "@ CWorkerThread::OnAddMedicToXml -> Write \n%S\n", strXml );

	// Convert to UTF8
	int  nUTF8    = strXml.GetLength()*2;
	char *pszUTF8 = new char[ nUTF8 ];
	theApp.m_pProgramData->ToUTF8( strXml.GetBuffer(), strXml.GetLength(), pszUTF8, &nUTF8 );

	char *pszMedicsTag = new char[ MAX_PATH ];
	int  nMedicsTag    = wcslen( XML_MEDICS ) + 3;

	sprintf_s( pszMedicsTag, MAX_PATH, "</%S>", XML_MEDICS );
	
	if ( !theApp.m_pProgramData->PositionInFile( &fXml, pszMedicsTag, nMedicsTag, CFile::end, TRUE ) )
	{
		TRACE( L"@ CWorkerThread::OnAddMedicToXml -> Failed PositionInFile\n" );

		delete [] pszMedicsTag;
		delete [] pszUTF8;
		delete medic;
		fXml.Close();

		return;
	}
		
	fXml.Write( pszUTF8, nUTF8 );

	// Cleanup
	delete [] pszMedicsTag;
	delete [] pszUTF8;
	delete medic;
	fXml.Close();
}

void CWorkerThread::OnDeleteMedicFromXml( WPARAM wParam, LPARAM lParam )
{
	MEDIC *medic = (MEDIC*)wParam;

	if ( medic == NULL )
		return;

	// Delete temp patients xml
	CString strPath = theApp.m_pProgramData->GetCurrentDir() + L"\\" + TEMP_DIR + L"\\" +
		medic->strLastName + L"-" + 
		medic->strFirstName + L"-" + 
		medic->strID + L"-" + 
		TEMP_DIR + L".xml";
	::DeleteFile( strPath );

	// Delete patients xml
	strPath = theApp.m_pProgramData->GetCurrentDir() + L"\\" + PATIENTS_DIR + L"\\" +
		medic->strLastName + L"-" + 
		medic->strFirstName + L"-" + 
		medic->strID + L".xml";
	::DeleteFile( strPath );

	// Delete from xml
	OnRewriteMedicsXml();

	delete medic;
}

void CWorkerThread::OnRewriteMedicsXml( WPARAM wParam, LPARAM lParam )
{
	if ( theApp.m_pProgramData->GetMedicsMap()->GetCount() == 0 )
		return;

	// Get xml path
	CString strPath = theApp.m_pProgramData->GetCurrentDir() + L"\\" + 
		theApp.m_pProgramData->GetMedicsXML();

	// Open xml
	CFile fXml;

	if ( !fXml.Open( strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ) )
	{
		TRACE( L"@ CWorkerThread::OnRewriteMedicsXml -> Failed fXml.Open [%ld]\n", GetLastError() );

		return;
	}

	// Write header
	CString strXml;
	strXml.Format( L"%s\n<%s>\n", XML_HEADER, XML_MEDICS );

	int  nUTF8    = MAX_XML_LINE;
	char *pszUTF8 = new char[ MAX_XML_LINE ];
	theApp.m_pProgramData->ToUTF8( strXml.GetBuffer(), strXml.GetLength(), pszUTF8, &nUTF8 );
	fXml.Write( pszUTF8, nUTF8 );

	// Write the medics
	CMapStringToMedic::CPair *pCurVal = theApp.m_pProgramData->GetMedicsMap()->PGetFirstAssoc();

	while ( pCurVal != NULL )
	{
		strXml.Format( L"\t<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" />\n",
			XML_MEDIC,
			XML_MEDIC_LAST_NAME, pCurVal->value.strLastName,
			XML_MEDIC_FIRST_NAME, pCurVal->value.strFirstName,
			XML_MEDIC_ID, pCurVal->value.strID );
		nUTF8 = MAX_XML_LINE;
		theApp.m_pProgramData->ToUTF8( strXml.GetBuffer(), strXml.GetLength(), pszUTF8, &nUTF8 );
		fXml.Write( pszUTF8, nUTF8 );

		pCurVal = theApp.m_pProgramData->GetMedicsMap()->PGetNextAssoc( pCurVal );
	}

	// End
	strXml.Format( L"</%s>", XML_MEDICS );
	nUTF8 = MAX_XML_LINE;
	theApp.m_pProgramData->ToUTF8( strXml.GetBuffer(), strXml.GetLength(), pszUTF8, &nUTF8 );
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

		if ( nCount ) // Patient found
		{
			CString *pstrCnp = new CString( strCnp );

			this->PostThreadMessage( WM_DIG_FOR_CNP, (WPARAM)pstrCnp, (LPARAM)FALSE );
		}
		else // Patient not found .. create?
		{
			CDlgCreatePatient dlgCreatePatient( strCnp );
			
			if ( dlgCreatePatient.DoModal() == IDOK )
			{
				TRACE( L"@ CWorkerThread::SQLiteCallback -> Create patient %s %s %s %s\n", 
					dlgCreatePatient.GetCNP(), dlgCreatePatient.GetLastName(), dlgCreatePatient.GetFirstName(), dlgCreatePatient.GetCity() );

				// Add temp patient to xml
				PATIENT *p = new PATIENT;
				p->strID		= dlgCreatePatient.GetCNP();
				p->strLastName  = dlgCreatePatient.GetLastName();
				p->strFirstName = dlgCreatePatient.GetFirstName();
				p->strCityCode  = theApp.m_pProgramData->GetCity( dlgCreatePatient.GetCity() ).strID;

				OnAddTempPatientToXml( (WPARAM)p, 0 );

				theApp.m_pFrmMain->ResetTxtCnp();
			}
			else
			{
				theApp.m_pFrmMain->SetStatus( (CString)MAKEINTRESOURCE( STATUS_CNP_NOT_ADDED ) );
				theApp.m_pFrmMain->ResetTxtCnp();
			}
		}
	}
	else // Create patient
	{
		CString strCity( argv[ 3 ] );

		// Add temp patient to xml
		PATIENT *p = new PATIENT;
		p->strID		= argv[ 0 ];
		p->strLastName  = argv[ 1 ];
		p->strFirstName = argv[ 2 ];
		p->strCityCode  = theApp.m_pProgramData->GetCity( strCity ).strID;

		TRACE( L"@ CWorkerThread::SQLiteCallback -> Add %s, %s, %s, %s, %s\n", 
			p->strID, p->strLastName, p->strFirstName, strCity, p->strCityCode );

		OnAddTempPatientToXml( (WPARAM)p, 0 );

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

BOOL CWorkerThread::CreateGeneticPatientsXml()
{
	CFile fXml;

	if ( !fXml.Open( 
		theApp.m_pProgramData->GetCurrentDir() + L"\\" + PATIENTS_DIR + L"\\" +
			theApp.m_pProgramData->GetCurrentMedic().strLastName + L"-" + 
			theApp.m_pProgramData->GetCurrentMedic().strFirstName + L"-" + 
			theApp.m_pProgramData->GetCurrentMedic().strID + L".xml", 
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