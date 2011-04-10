#include "stdafx.h"
#include "ProgramData.h"

///////////////////////////////////////////// Constructor / Destructor /////////////////////////////////////////////

CProgramData::CProgramData()
{
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