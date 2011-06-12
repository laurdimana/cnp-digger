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
	void AddPatient( CString strID, CString strLastName, CString strFirstName, CString strCityName );
	void AddPatientTemp( CString strID, CString strLastName, CString strFirstName, CString strCityName );
	void CreateMedic( CString strID, CString strLastName, CString strFirstName );
	void CreateCity( CString strID, CString strName, CString strDistrict );
	void CreatePatientTemp( CString strID, CString strLastName, CString strFirstName, CString strCityName );
	void SetCurrentMedic( MEDIC medic );

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

	MEDIC   GetMedic( CString strID );
	MEDIC	GetCurrentMedic();
	CITY    GetCity( CString strName );
	PATIENT GetPatient( int nIndex );
	PATIENT GetPatientTemp( int nIndex );

	CMapStringToMedic *GetMedicsMap();

	BOOL LoadSQLite();

// Helpers
	BOOL ToUTF8( wchar_t *pszIn, int nInLen, char *pszOut, int *nOutLen );

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

	CSQLiteWrapper m_db;
};