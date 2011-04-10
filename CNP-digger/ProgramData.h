#pragma once

class CProgramData
{
// Constructor / Destructor
public:
	CProgramData();
	~CProgramData();

// Methods
public:
	// Getters
	CString GetCurrentDir();
	CString GetPersonsDB();
	CString GetMedicsXML();
	CString GetCitiesXML();
	CString GetExportsDir();
	CString GetPatientsDir();
	CString GetTempDir();

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
};