#include "stdafx.h"
#include "WorkerThread.h"
#include "CNP-digger.h"
#include "MedicsSAXContentHandler.h"
#include "CitiesSAXContentHandler.h"
#include "SAXErrorHandler.h"

BEGIN_MESSAGE_MAP( CWorkerThread, CWinThread )
	ON_THREAD_MESSAGE( WM_CHECK_FOR_ESSENTIAL_FILES, OnCheckForEssentialFiles )
	ON_THREAD_MESSAGE( WM_PARSE_MEDICS_XML, OnParseMedicsXML )
	ON_THREAD_MESSAGE( WM_PARSE_CITIES_XML, OnParseCitiesXML )
END_MESSAGE_MAP()

///////////////////////////////////////////////// Constructor / Destructor ///////////////////////////////////////////

CWorkerThread::CWorkerThread()
{
}

CWorkerThread::~CWorkerThread()
{
}

//////////////////////////////////////////////////// Overrides ///////////////////////////////////////////////////

BOOL CWorkerThread::InitInstance()
{
	TRACE( L"@ CWorkerThread::InitInstance -> Worker thread started\n" );
	return TRUE;
}

int CWorkerThread::ExitInstance()
{
	TRACE( L"@ CWorkerThread::InitInstance -> Worker thread stopped\n" );
	return 0;
}

//////////////////////////////////////////////////// Events ///////////////////////////////////////////////////////

void CWorkerThread::OnCheckForEssentialFiles( WPARAM wParam, LPARAM lParam )
{
	// Check for persons db
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetPersonsDB()).GetBuffer() ) )
	{
		TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Persons db not found\n" );
		theApp.m_pFrmMain->MessageBox( L"Persons database not found.", L"Error" );
		theApp.m_pFrmMain->PostMessage( WM_QUIT );
		return;
	}

	// Check for exports folder
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetExportsDir()).GetBuffer() ) )
	{
		if ( !::CreateDirectory( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetExportsDir()).GetBuffer(), NULL ) )
		{
			TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to create the Exports folder\n" );
			theApp.m_pFrmMain->MessageBox( L"Failed to create the Exports folder.", L"Error" );
			theApp.m_pFrmMain->PostMessage( WM_QUIT );
			return;
		}
	}

	// Check for patients folder
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetPatientsDir()).GetBuffer() ) )
	{
		if ( !::CreateDirectory( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetPatientsDir()).GetBuffer(), NULL ) )
		{
			TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to create the Patients folder\n" );
			theApp.m_pFrmMain->MessageBox( L"Failed to create the Patients folder.", L"Error" );
			theApp.m_pFrmMain->PostMessage( WM_QUIT );
			return;
		}
	}

	// Check for temp folder
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetTempDir()).GetBuffer() ) )
	{
		if ( !::CreateDirectory( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetTempDir()).GetBuffer(), NULL ) )
		{
			TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to create the Temp folder\n" );
			theApp.m_pFrmMain->MessageBox( L"Failed to create the Temp folder.", L"Error" );
			theApp.m_pFrmMain->PostMessage( WM_QUIT );
			return;
		}
	}

	// Check for cityes xml
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetCitiesXML()).GetBuffer() ) )
	{
		if ( !CreateGenericCitiesXml() )
		{
			TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to create cityes xml\n" );
			theApp.m_pFrmMain->MessageBox( L"Failed to create cityes xml.", L"Error" );
			theApp.m_pFrmMain->PostMessage( WM_QUIT );
			return;
		}
	}

	// Check for medics xml
	if ( !::PathFileExists( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetMedicsXML()).GetBuffer() ) )
	{
		if ( !CreateGenericMedicsXml() )
		{
			TRACE( L"@ CWorkerThread::OnCheckForEssentialFiles -> Failed to create medics xml\n" );
			theApp.m_pFrmMain->MessageBox( L"Failed to create medics xml.", L"Error" );
			theApp.m_pFrmMain->PostMessage( WM_QUIT );
			return;
		}
	}

	this->PostThreadMessage( WM_PARSE_CITIES_XML, NULL, NULL );

	this->PostThreadMessage( WM_PARSE_MEDICS_XML, NULL, NULL );
}

void CWorkerThread::OnParseMedicsXML( WPARAM wParam, LPARAM lParam )
{
	CoInitialize( NULL );
	ISAXXMLReader *pXMLReader = NULL;

	HRESULT hr = CoCreateInstance(
		__uuidof( SAXXMLReader ),
		NULL,
		CLSCTX_ALL,
		__uuidof( ISAXXMLReader ),
		(void **)&pXMLReader );

	if ( !FAILED( hr ) )
	{
		MedicsSAXContentHandler *pMedicsHandler = new MedicsSAXContentHandler();
		hr = pXMLReader->putContentHandler( pMedicsHandler );

		SAXErrorHandler *pErrorHandler = new SAXErrorHandler();
		hr = pXMLReader->putErrorHandler( pErrorHandler );

		hr = pXMLReader->parseURL( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetMedicsXML()).GetBuffer() );

		pXMLReader->Release();
	}

	CoUninitialize();

	theApp.m_pFrmMain->PostMessage( WM_COMMAND, MAKELONG( FRM_MAIN_MNU_FILE_MEDICS, 0 ) );
}

void CWorkerThread::OnParseCitiesXML( WPARAM wParam, LPARAM lParam )
{
	CoInitialize( NULL );
	ISAXXMLReader *pXMLReader = NULL;

	HRESULT hr = CoCreateInstance(
		__uuidof( SAXXMLReader ),
		NULL,
		CLSCTX_ALL,
		__uuidof( ISAXXMLReader ),
		(void **)&pXMLReader );

	if ( !FAILED( hr ) )
	{
		CitiesSAXContentHandler *pCitiesHandler = new CitiesSAXContentHandler();
		hr = pXMLReader->putContentHandler( pCitiesHandler );

		SAXErrorHandler *pErrorHandler = new SAXErrorHandler();
		hr = pXMLReader->putErrorHandler( pErrorHandler );

		hr = pXMLReader->parseURL( (theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetCitiesXML()).GetBuffer() );

		pXMLReader->Release();
	}

	CoUninitialize();
}

//////////////////////////////////////////////////// Methods //////////////////////////////////////////////////////////

BOOL CWorkerThread::CreateGenericCitiesXml()
{
	CFile fXml;

	if ( !fXml.Open( 
		theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetCitiesXML(), 
		CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ) )
	{
		return FALSE;
	}

	CString strXml;
	strXml.Format( L"%s\n<%s>\n</%s>", XML_HEADER, XML_CITIES, XML_CITIES );

	int  nUTF8    = strXml.GetLength()*2;
	char *pszUTF8 = new char[ nUTF8 ];
	theApp.m_pProgramData->ToUTF8( strXml.GetBuffer(), strXml.GetLength(), pszUTF8, &nUTF8 );
	fXml.Write( pszUTF8, nUTF8 );
	delete [] pszUTF8;

	fXml.Close();

	return TRUE;
}

BOOL CWorkerThread::CreateGenericMedicsXml()
{
	CFile fXml;

	if ( !fXml.Open( 
		theApp.m_pProgramData->GetCurrentDir() + L"\\" + theApp.m_pProgramData->GetMedicsXML(), 
		CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ) )
	{
		return FALSE;
	}

	CString strXml;
	strXml.Format( L"%s\n<%s>\n</%s>", XML_HEADER, XML_MEDICS, XML_MEDICS );

	int  nUTF8    = strXml.GetLength()*2;
	char *pszUTF8 = new char[ nUTF8 ];
	theApp.m_pProgramData->ToUTF8( strXml.GetBuffer(), strXml.GetLength(), pszUTF8, &nUTF8 );
	fXml.Write( pszUTF8, nUTF8 );
	delete [] pszUTF8;

	fXml.Close();

	return TRUE;
}
