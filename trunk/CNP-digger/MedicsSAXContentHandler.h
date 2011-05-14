#pragma once

#include "SAXContentHandler.h"

class MedicsSAXContentHandler : public SAXContentHandler
{
public:
// Constructor / Destructor 
	MedicsSAXContentHandler();
	virtual ~MedicsSAXContentHandler();

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