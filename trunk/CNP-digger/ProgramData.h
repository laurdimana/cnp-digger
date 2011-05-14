#pragma once

class CProgramData
{
// Constructor / Destructor
public:
	CProgramData();
	~CProgramData();

// Methods
public:
// Setters
	void AddMedic( CString strID, CString strLastName, CString strFirstName );
	void AddCity( CString strID, CString strName, CString strDistrict );
	void AddPatient( CString strID, CString strLastName, CString strFirstName, CString strCityName );

// Getters
	CString GetCurrentDir();
	CString GetPersonsDB();
	CString GetMedicsXML();
	CString GetCitiesXML();
	CString GetExportsDir();
	CString GetPatientsDir();
	CString GetTempDir();

	int GetMedics();
	int GetCities();
	int GetPatients();

	MEDIC   GetMedic( CString strID );
	CITY    GetCity( CString strName );
	PATIENT GetPatient( int nIndex );

	BOOL ToUTF8( wchar_t *pszIn, int nInLen, char *pszOut, int *nOutLen );

// Members
protected:
	CString m_strCurrentDir,
			m_strPersonsDB,
			m_strMedicsXML,
			m_strCitiesXML,
			m_strExportsDir,
			m_strPatientsDir,
			m_strTempDir;

	CMap<wchar_t*, wchar_t*, MEDIC, MEDIC&> m_Medics;
	CMap<wchar_t*, wchar_t*, CITY, CITY&>   m_Cities;
	CList<PATIENT, PATIENT&>			    m_Patients;
};