#pragma once

class CWorkerThread : public CWinThread
{
// Constructor / Destructor
public:
	CWorkerThread();
	virtual ~CWorkerThread();

// Ovverides
public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();

// Methods
public:
	BOOL CreateGenericCitiesXml();
	BOOL CreateGenericMedicsXml();
	BOOL CreateGeneticTempPatientsXml();

	int SQLiteCallback( void *NotUsed, int argc, char **argv, char **coln );

// Message map
protected:
	afx_msg void OnCheckForEssentialFiles( WPARAM wParam = 0, LPARAM lParam = 0 );
	afx_msg void OnParseMedicsXML( WPARAM wParam = 0, LPARAM lParam = 0 );
	afx_msg void OnParseCitiesXML( WPARAM wParam = 0, LPARAM lParam = 0 );
	afx_msg void OnInitMedic( WPARAM wParam = 0, LPARAM lParam = 0 );
	afx_msg void OnParsePatientsXML( WPARAM wParam, LPARAM lParam = FALSE );
	afx_msg void OnDigForCnp( WPARAM wParam, LPARAM lParam );
	afx_msg void OnAddTempPatientToXml( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()
};