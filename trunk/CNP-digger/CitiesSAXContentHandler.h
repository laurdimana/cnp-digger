#pragma once

#include "SAXContentHandler.h"

class CitiesSAXContentHandler : public SAXContentHandler
{
public:
// Constructor / Destructor 
	CitiesSAXContentHandler();
	virtual ~CitiesSAXContentHandler();

// Ovverides
	virtual HRESULT STDMETHODCALLTYPE startElement(
		const wchar_t *pwchNamespaceUri,
		int cchNamespaceUri,
		const wchar_t *pwchLocalName,
		int cchLocalName,
		const wchar_t *pwchRawName,
		int cchRawName,
		ISAXAttributes *pAttributes );

	virtual HRESULT STDMETHODCALLTYPE characters( wchar_t *pwchChars, int cchChars );

	virtual HRESULT STDMETHODCALLTYPE endElement(
		const wchar_t *pwchNamespaceUri,
		int cchNamespaceUri,
		const wchar_t *pwchLocalName,
		int cchLocalName,
		const wchar_t *pwchRawName,
		int cchRawName );
	
// Methods
	CString GetAttributeValue( ISAXAttributes *pAttributes, CString strAttrName );
};