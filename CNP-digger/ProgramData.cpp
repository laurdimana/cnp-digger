#include "stdafx.h"
#include "ProgramData.h"

///////////////////////////////////////////// Constructor / Destructor /////////////////////////////////////////////

CProgramData::CProgramData()
{
	m_MedicsMap.InitHashTable( INIT_MEDICS_HASH_TABLE );
	m_CitiesMap.InitHashTable( INIT_CITIES_HASH_TABLE );
	m_CurrentMedic.strID = L"";

	// Init current dir
	wchar_t *pszPath = new wchar_t[ MAX_PATH ];
	::GetModuleFileName( NULL, pszPath, MAX_PATH );
	wchar_t *p = wcsrchr( pszPath, L'\\' );
	p[ 0 ] = L'\0';
	m_strCurrentDir.Append( pszPath );
	delete [] pszPath;

	// Init persons db, medics xml, cityes xml, exports dir, patients dir, temp dir
	m_strPersonsDB.Append( PERSONS_DB );
	m_strMedicsXML.Append( MEDICS_XML );
	m_strCitiesXML.Append( CITIES_XML );
	m_strSQLiteDLL.Append( SQLITE_DLL );
	m_strExportsDir.Append( EXPORTS_DIR );
	m_strPatientsDir.Append( PATIENTS_DIR );
	m_strTempDir.Append( TEMP_DIR );
}

CProgramData::~CProgramData()
{
}

////////////////////////////////////////////////// Methods ///////////////////////////////////////////////////

CString CProgramData::GetCurrentDir()
{
	return m_strCurrentDir;
}

CString CProgramData::GetPersonsDB()
{
	return m_strPersonsDB;
}

CString CProgramData::GetMedicsXML()
{
	return m_strMedicsXML;
}

CString CProgramData::GetCitiesXML()
{
	return m_strCitiesXML;
}

CString CProgramData::GetSQLiteDLL()
{
	return m_strSQLiteDLL;
}

CString CProgramData::GetExportsDir()
{
	return m_strExportsDir;
}

CString CProgramData::GetPatientsDir()
{
	return m_strPatientsDir;
}

CString CProgramData::GetTempDir()
{
	return m_strTempDir;
}

BOOL CProgramData::ToUTF8( wchar_t *pszIn, int nInLen, char *pszOut, int *nOutLen )
{
	*nOutLen = ::WideCharToMultiByte(
		CP_UTF8, 0,
		pszIn, nInLen,
		NULL, 0, NULL, NULL );

	if ( *nOutLen == 0 )
		return FALSE;

	::WideCharToMultiByte(
		CP_UTF8, 0,
		pszIn, nInLen,
		pszOut, *nOutLen,
		NULL, NULL );

	return TRUE;
}

void CProgramData::AddMedic( CString strID, CString strLastName, CString strFirstName )
{
	MEDIC medic;
	medic.strID		   = strID;
	medic.strLastName  = strLastName;
	medic.strFirstName = strFirstName;

	m_MedicsMap[ strID ] = medic;

	TRACE( L"@ CProgramData::AddMedic -> Load medic %s %s\n", m_MedicsMap[ strID ].strLastName, m_MedicsMap[ strID ].strFirstName );
}

void CProgramData::AddCity( CString strID, CString strName, CString strDistrict )
{
	CITY city;
	city.strID		 = strID;
	city.strName	 = strName;
	city.strDistrict = strDistrict;

	m_CitiesMap[ strID ] = city;

	TRACE( L"@ CProgramData::AddCity -> Load city %s, %s\n", m_CitiesMap[ strID ].strName, m_CitiesMap[ strID ].strDistrict );
}

void CProgramData::AddPatient( CString strID, CString strLastName, CString strFirstName, CString strCityName )
{
	PATIENT patient;
	patient.strID = strID;
	patient.strLastName = strLastName;
	patient.strFirstName = strFirstName;
	patient.strCityName = strCityName;

	m_PatientsList.AddTail( patient );

	TRACE( L"@ CProgramData::AddPatient -> Load patient %s %s %s %s %s %s\n",
		m_PatientsList.GetTail().strID,
		m_PatientsList.GetTail().strLastName,
		m_PatientsList.GetTail().strFirstName,
		m_PatientsList.GetTail().strCityName,
		m_CitiesMap[ m_PatientsList.GetTail().strCityName ].strID,
		m_CitiesMap[ m_PatientsList.GetTail().strCityName ].strDistrict );
}

void CProgramData::AddPatientTemp( CString strID, CString strLastName, CString strFirstName, CString strCityName )
{
	PATIENT patient;
	patient.strID = strID;
	patient.strLastName = strLastName;
	patient.strFirstName = strFirstName;
	patient.strCityName = strCityName;

	m_PatientsListTemp.AddTail( patient );

	TRACE( L"@ CProgramData::AddPatientTemp -> Load patient %s %s %s %s %s %s\n",
		m_PatientsListTemp.GetTail().strID,
		m_PatientsListTemp.GetTail().strLastName,
		m_PatientsListTemp.GetTail().strFirstName,
		m_PatientsListTemp.GetTail().strCityName,
		m_CitiesMap[ m_PatientsListTemp.GetTail().strCityName ].strID,
		m_CitiesMap[ m_PatientsListTemp.GetTail().strCityName ].strDistrict );
}

void CProgramData::CreateMedic( CString strID, CString strLastName, CString strFirstName )
{
}

void CProgramData::CreateCity( CString strID, CString strName, CString strDistrict )
{
}

void CProgramData::CreatePatientTemp( CString strID, CString strLastName, CString strFirstName, CString strCityName )
{
}

void CProgramData::SetCurrentMedic( MEDIC medic )
{
	m_CurrentMedic = medic;
}

int CProgramData::GetMedics()
{
	return m_MedicsMap.GetCount();
}

int CProgramData::GetCities()
{
	return m_CitiesMap.GetCount();
}

int CProgramData::GetPatients()
{
	return m_PatientsList.GetCount();
}

MEDIC CProgramData::GetMedic( CString strID )
{
	return m_MedicsMap[ strID ];
}

MEDIC CProgramData::GetCurrentMedic()
{
	return m_CurrentMedic;
}

CITY CProgramData::GetCity( CString strName )
{
	return m_CitiesMap[ strName ];
}

PATIENT CProgramData::GetPatient( int nIndex )
{
	return m_PatientsList.GetAt( m_PatientsList.FindIndex( nIndex ) );
}

PATIENT CProgramData::GetPatientTemp( int nIndex )
{
	return m_PatientsListTemp.GetAt( m_PatientsListTemp.FindIndex( nIndex ) );
}

CMapStringToMedic *CProgramData::GetMedicsMap()
{
	return &m_MedicsMap;
}

//static int SQLiteCallback( void *NotUsed, int argc, char **argv, char **coln )
//{
//	for ( int i = 0; i < argc; i++ )
//	{
//		CString str( argv[ i ] );
//
//		TRACE( L"%s\t\t\t", str );
//	}
//	TRACE( L"\n" );
//
//	return 0;
//}
BOOL CProgramData::LoadSQLite()
{
	BOOL rc = m_db.LoadDll( GetCurrentDir() + L"\\" + GetSQLiteDLL() );

	if ( !rc )
		return FALSE;

	rc = m_db.SQLiteOpen( GetCurrentDir() + L"\\" + GetPersonsDB() );

	if ( !rc )
		return FALSE;

	//m_db.SQLiteExec( L"SELECT * FROM persons", SQLiteCallback );

	return TRUE;
}
