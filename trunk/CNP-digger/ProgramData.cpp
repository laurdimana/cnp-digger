#include "stdafx.h"
#include "ProgramData.h"

///////////////////////////////////////////// Constructor / Destructor /////////////////////////////////////////////

CProgramData::CProgramData()
{
	m_Medics.InitHashTable( INIT_MEDICS_HASH_TABLE );
	m_Cities.InitHashTable( INIT_CITIES_HASH_TABLE );

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
	m_strCitiesXML.Append( CITiES_XML );
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

	m_Medics[ strID.GetBuffer() ] = medic;

	TRACE( L"@ CProgramData::AddMedic -> Load medic %s %s\n", m_Medics[ strID.GetBuffer() ].strLastName, m_Medics[ strID.GetBuffer() ].strFirstName );
}

void CProgramData::AddCity( CString strID, CString strName, CString strDistrict )
{
	CITY city;
	city.strID		 = strID;
	city.strName	 = strName;
	city.strDistrict = strDistrict;

	m_Cities[ strID.GetBuffer() ] = city;

	TRACE( L"@ CProgramData::AddCity -> Load city %s, %s\n", m_Cities[ strID.GetBuffer() ].strName, m_Cities[ strID.GetBuffer() ].strDistrict );
}

void CProgramData::AddPatient( CString strID, CString strLastName, CString strFirstName, CString strCityName )
{
	PATIENT patient;
	patient.strID = strID;
	patient.strLastName = strLastName;
	patient.strFirstName = strFirstName;
	patient.strCityName = strCityName;

	m_Patients.AddTail( patient );

	TRACE( L"@ CProgramData::AddPatient -> Load patient %s %s %s %s %s %s\n",
		m_Patients.GetTail().strID,
		m_Patients.GetTail().strLastName,
		m_Patients.GetTail().strFirstName,
		m_Patients.GetTail().strCityName,
		m_Cities[ m_Patients.GetTail().strCityName.GetBuffer() ].strID,
		m_Cities[ m_Patients.GetTail().strCityName.GetBuffer() ].strDistrict );
}

int CProgramData::GetMedics()
{
	return m_Medics.GetCount();
}

int CProgramData::GetCities()
{
	return m_Cities.GetCount();
}

int CProgramData::GetPatients()
{
	return m_Patients.GetCount();
}

MEDIC CProgramData::GetMedic( CString strID )
{
	return m_Medics[ strID.GetBuffer() ];
}

CITY CProgramData::GetCity( CString strName )
{
	return m_Cities[ strName.GetBuffer() ];
}

PATIENT CProgramData::GetPatient( int nIndex )
{
	return m_Patients.GetAt( m_Patients.FindIndex( nIndex ) );
}
