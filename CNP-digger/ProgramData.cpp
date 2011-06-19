#include "stdafx.h"
#include "ProgramData.h"

static const int sCNP[] = { 2, 7, 9, 1, 4, 6, 3, 5, 8, 2, 7, 9 };

///////////////////////////////////////////// Constructor / Destructor /////////////////////////////////////////////

CProgramData::CProgramData()
{
	m_MedicsMap.InitHashTable( INIT_MEDICS_HASH_TABLE );
	m_CitiesMap.InitHashTable( INIT_CITIES_HASH_TABLE );
	m_CurrentMedic.strID = L"";
	m_nDisplayedPatients = 0;

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

BOOL CProgramData::IsCnpValid( wchar_t *pszCnp )
{
	if ( wcslen( pszCnp ) != 13 )
		return FALSE;

	int S = 0;

	for ( int i = 0; i < 12 ; i++ )
	{
		CString strC( pszCnp[ i ] );

		S += _wtoi( strC ) * sCNP[ i ];
	}

	int R = S - 11 * (S / 11);
	R = (R == 10) ? 1 : R;

	CString strC( pszCnp[ 12 ] );

	return _wtoi( strC ) == R;
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

	m_CitiesMap[ strName ] = city;

	TRACE( L"@ CProgramData::AddCity -> Load city %s, %s, %s\n", m_CitiesMap[ strName ].strName, m_CitiesMap[ strName ].strID, m_CitiesMap[ strName ].strDistrict );
}

void CProgramData::AddPatient( CString strID, CString strLastName, CString strFirstName, CString strCityCode )
{
	PATIENT patient;
	patient.strID		 = strID;
	patient.strLastName  = strLastName;
	patient.strFirstName = strFirstName;
	patient.strCityCode  = strCityCode;

	m_PatientsList.AddTail( patient );

	TRACE( L"@ CProgramData::AddPatient -> Load patient %s %s %s %s\n",
		m_PatientsList.GetTail().strID,
		m_PatientsList.GetTail().strLastName,
		m_PatientsList.GetTail().strFirstName,
		m_PatientsList.GetTail().strCityCode );
}

void CProgramData::AddPatientTemp( CString strID, CString strLastName, CString strFirstName, CString strCityCode )
{
	PATIENT patient;
	patient.strID		 = strID;
	patient.strLastName  = strLastName;
	patient.strFirstName = strFirstName;
	patient.strCityCode  = strCityCode;

	m_PatientsListTemp.AddTail( patient );

	TRACE( L"@ CProgramData::AddPatientTemp -> Load patient %s %s %s %s\n",
		m_PatientsListTemp.GetTail().strID,
		m_PatientsListTemp.GetTail().strLastName,
		m_PatientsListTemp.GetTail().strFirstName,
		m_PatientsListTemp.GetTail().strCityCode );
}

void CProgramData::SetCurrentMedic( MEDIC medic )
{
	m_CurrentMedic = medic;
}

void CProgramData::SetDislayedPatients( int nPatients )
{
	m_nDisplayedPatients = nPatients;
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

int CProgramData::GetTempPatients()
{
	return m_PatientsListTemp.GetCount();
}

int CProgramData::GetDisplayedPatients()
{
	return m_nDisplayedPatients;
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

CListPatients *CProgramData::GetPatientsList()
{
	return &m_PatientsList;
}

CListPatients *CProgramData::GetTempPatientsList()
{
	return &m_PatientsListTemp;
}

BOOL CProgramData::LoadSQLite()
{
	BOOL rc = m_db.LoadDll( GetCurrentDir() + L"\\" + GetSQLiteDLL() );

	if ( !rc )
		return FALSE;

	rc = m_db.SQLiteOpen( GetCurrentDir() + L"\\" + GetPersonsDB() );

	if ( !rc )
		return FALSE;

	return TRUE;
}

BOOL CProgramData::SQLiteExec( CString strSQL, int (*pCallback)( void*, int, char**, char** ), CString strErrMsg )
{
	return m_db.SQLiteExec( strSQL, pCallback, strErrMsg );
}