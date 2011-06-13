#include "stdafx.h"
#include "PatientsSAXContentHandler.h"
#include "CNP-digger.h"

///////////////////////////////////////////////////// Constructor / Destructor ///////////////////////////////////////////

PatientsSAXContentHandler::PatientsSAXContentHandler( BOOL bTempPatient )
{
	m_bTempPatient = bTempPatient;
}

PatientsSAXContentHandler::~PatientsSAXContentHandler()
{
}

////////////////////////////////////////////////////// Ovverides ///////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE PatientsSAXContentHandler::startElement(
	const wchar_t __RPC_FAR *pwchNamespaceUri,
	int cchNamespaceUri,
	const wchar_t __RPC_FAR *pwchLocalName,
	int cchLocalName,
	const wchar_t __RPC_FAR *pwchRawName,
	int cchRawName,
	ISAXAttributes __RPC_FAR *pAttributes )
{
	CString strLocalName( pwchLocalName );

	if ( strLocalName.CompareNoCase( XML_PERSON ) == 0 )
	{
		m_strPID	   = GetAttributeValue( pAttributes, XML_PERSON_PID );
		m_strFirstName = GetAttributeValue( pAttributes, XML_PERSON_FIRST_NAME );
		m_strLastName  = GetAttributeValue( pAttributes, XML_PERSON_LAST_NAME );
	}
	else if ( strLocalName.CompareNoCase( XML_PERSON_ADDRESS ) == 0 )
	{
		m_strCityCode = GetAttributeValue( pAttributes, XML_PERSON_ADDRESS_CITY_CODE );
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE PatientsSAXContentHandler::characters( wchar_t __RPC_FAR *pwchChars, int cchChars )
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE PatientsSAXContentHandler::endElement(
	const wchar_t __RPC_FAR *pwchNamespaceUri,
	int cchNamespaceUri,
	const wchar_t __RPC_FAR *pwchLocalName,
	int cchLocalName,
	const wchar_t __RPC_FAR *pwchRawName,
	int cchRawName )
{
	CString strLocalName( pwchLocalName );

	if ( strLocalName.CompareNoCase( XML_PERSON ) == 0 )
	{
		if ( !m_bTempPatient )
		{
			if ( !m_strPID.IsEmpty() && !m_strFirstName.IsEmpty() && !m_strLastName.IsEmpty() && !m_strCityCode.IsEmpty() )
				theApp.m_pProgramData->AddPatient( m_strPID, m_strLastName, m_strFirstName, m_strCityCode );
		}
		else
		{
			if ( !m_strPID.IsEmpty() && !m_strFirstName.IsEmpty() && !m_strLastName.IsEmpty() && !m_strCityCode.IsEmpty() )
				theApp.m_pProgramData->AddPatientTemp( m_strPID, m_strLastName, m_strFirstName, m_strCityCode );
		}

		m_strPID.Empty();
		m_strLastName.Empty();
		m_strFirstName.Empty();
		m_strCityCode.Empty();
	}

	return S_OK;
}

///////////////////////////////////////////////////// Methods //////////////////////////////////////////////////////////////

CString PatientsSAXContentHandler::GetAttributeValue( ISAXAttributes *pAttributes, CString strAttrName )
{
	int nLen = 0;

	pAttributes->getLength( &nLen );

	for ( int i = 0 ; i < nLen ; i++ )
	{
		const wchar_t *pszName = NULL, *pszValue = NULL;
		int			  nNameLen = 0, nValueLen = 0;

		pAttributes->getLocalName( i, &pszName, &nNameLen );
		pAttributes->getValue( i, &pszValue, &nValueLen );

		if ( CString( pszName, nNameLen ).CompareNoCase( strAttrName ) == 0 )
			return CString( pszValue, nValueLen );
	}

	return L"";
}