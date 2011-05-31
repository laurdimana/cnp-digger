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

// Message map
protected:
	afx_msg void OnCheckForEssentialFiles( WPARAM wParam, LPARAM lParam );
	afx_msg void OnParseMedicsXML( WPARAM wParam, LPARAM lParam );
	afx_msg void OnParseCitiesXML( WPARAM wParam, LPARAM lParam );
	afx_msg void OnInitMedic( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()
};