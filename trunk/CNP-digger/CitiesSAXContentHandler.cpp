#include "stdafx.h"
#include "CitiesSAXContentHandler.h"
#include "CNP-digger.h"

///////////////////////////////////////////////////// Constructor / Destructor ///////////////////////////////////////////

CitiesSAXContentHandler::CitiesSAXContentHandler()
{
}

CitiesSAXContentHandler::~CitiesSAXContentHandler()
{
}

////////////////////////////////////////////////////// Ovverides ///////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE CitiesSAXContentHandler::startElement(
	const wchar_t __RPC_FAR *pwchNamespaceUri,
	int cchNamespaceUri,
	const wchar_t __RPC_FAR *pwchLocalName,
	int cchLocalName,
	const wchar_t __RPC_FAR *pwchRawName,
	int cchRawName,
	ISAXAttributes __RPC_FAR *pAttributes )
{
	CString strLocalName( pwchLocalName );

	if ( strLocalName.CompareNoCase( XML_CITY ) == 0 )
	{
		CString strID	    = GetAttributeValue( pAttributes, XML_CITY_CODE );
		CString strName		= GetAttributeValue( pAttributes, XML_CITY_NAME );
		CString strDistrict = CITY_DISTRICT_CODE;

		if ( !strID.IsEmpty() && !strName.IsEmpty() && !strDistrict.IsEmpty() )
			theApp.m_pProgramData->AddCity( strID, strName, strDistrict );
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CitiesSAXContentHandler::characters( wchar_t __RPC_FAR *pwchChars, int cchChars )
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CitiesSAXContentHandler::endElement(
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

CString CitiesSAXContentHandler::GetAttributeValue( ISAXAttributes *pAttributes, CString strAttrName )
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