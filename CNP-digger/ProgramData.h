#pragma once

#include "SQLiteWrapper.h"

class CProgramData
{
// Constructor / Destructor
public:
	CProgramData();
	virtual ~CProgramData();

// Methods
public:
// Setters
	void AddMedic( CString strID, CString strLastName, CString strFirstName );
	void AddCity( CString strID, CString strName, CString strDistrict );
	void AddPatient( CString strID, CString strLastName, CString strFirstName, CString strCityCode );
	void AddPatientTemp( CString strID, CString strLastName, CString strFirstName, CString strCityCode );
	void SetCurrentMedic( MEDIC medic );
	void SetDislayedPatients( int nPatients );

// Getters
	CString GetCurrentDir();
	CString GetPersonsDB();
	CString GetMedicsXML();
	CString GetCitiesXML();
	CString GetSQLiteDLL();
	CString GetExportsDir();
	CString GetPatientsDir();
	CString GetTempDir();

	int GetMedics();
	int GetCities();
	int GetPatients();
	int GetTempPatients();
	int GetDisplayedPatients();

	MEDIC   GetMedic( CString strID );
	MEDIC	GetCurrentMedic();
	CITY    GetCity( CString strName );
	PATIENT GetPatient( int nIndex );
	PATIENT GetPatientTemp( int nIndex );

	CMapStringToMedic *GetMedicsMap();
	CListPatients	  *GetPatientsList();
	CListPatients	  *GetTempPatientsList();

	BOOL LoadSQLite();
	BOOL SQLiteExec( CString strSQL, int (*pCallback)( void*,int,char**,char** ), CString strErrMsg = NULL );

// Helpers
	BOOL ToUTF8( wchar_t *pszIn, int nInLen, char *pszOut, int *nOutLen );
	BOOL IsCnpValid( wchar_t *pszCnp );

// Members
protected:
	CString m_strCurrentDir,
			m_strPersonsDB,
			m_strMedicsXML,
			m_strCitiesXML,
			m_strSQLiteDLL,
			m_strExportsDir,
			m_strPatientsDir,
			m_strTempDir;

	CMapStringToMedic m_MedicsMap;
	CMapStringToCity  m_CitiesMap;
	CListPatients	  m_PatientsList,
					  m_PatientsListTemp;

	MEDIC m_CurrentMedic;
	int   m_nDisplayedPatients;

	CSQLiteWrapper m_db;
};