#include "stdafx.h"
#include "MedicsSAXContentHandler.h"
#include "CNP-digger.h"

///////////////////////////////////////////////////// Constructor / Destructor ///////////////////////////////////////////

MedicsSAXContentHandler::MedicsSAXContentHandler()
{
}

MedicsSAXContentHandler::~MedicsSAXContentHandler()
{
}

////////////////////////////////////////////////////// Ovverides ///////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE MedicsSAXContentHandler::startElement(
	const wchar_t __RPC_FAR *pwchNamespaceUri,
	int cchNamespaceUri,
	const wchar_t __RPC_FAR *pwchLocalName,
	int cchLocalName,
	const wchar_t __RPC_FAR *pwchRawName,
	int cchRawName,
	ISAXAttributes __RPC_FAR *pAttributes )
{
	CString strLocalName( pwchLocalName );

	if ( strLocalName.CompareNoCase( XML_MEDIC ) == 0 )
	{
		CString strID	     = GetAttributeValue( pAttributes, XML_MEDIC_ID );
		CString strLastName  = GetAttributeValue( pAttributes, XML_MEDIC_LAST_NAME );
		CString strFirstName = GetAttributeValue( pAttributes, XML_MEDIC_FIRST_NAME );

		// Add a medic
		if ( !strID.IsEmpty() && !strLastName.IsEmpty() && !strFirstName.IsEmpty() )
		{
			theApp.m_pProgramData->AddMedic( strID, strLastName, strFirstName );
		}
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE MedicsSAXContentHandler::characters( wchar_t __RPC_FAR *pwchChars, int cchChars )
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MedicsSAXContentHandler::endElement(
	const wchar_t __RPC_FAR *pwchNamespaceUri,
	int cchNamespaceUri,
	const wchar_t __RPC_FAR *pwchLocalName,
	int cchLocalName,
	const wchar_t __RPC_FAR *pwchRawName,
	int cchRawName )
{
	return S_OK;
}

///////////////////////////////////////////////////// Methods //////////////////////////////////////////////////////////////

CString MedicsSAXContentHandler::GetAttributeValue( ISAXAttributes *pAttributes, CString strAttrName )
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