#pragma once

//#include <MsXml2.h>

class SAXContentHandler : public ISAXContentHandler
{
	long m_RefCount;

public:
	SAXContentHandler() : m_RefCount( 0 ) {}
	virtual ~SAXContentHandler() {}

	long __stdcall QueryInterface( const struct _GUID &riid, void **ppObj )
	{
		if ( riid == IID_IUnknown )
        {
			*ppObj = static_cast<IUnknown*>( this );
        }

        if ( riid == __uuidof( ISAXContentHandler ) )
        {
           *ppObj = static_cast<ISAXContentHandler*>( this );
        }
        else
        {
           *ppObj = NULL ;
           return E_NOINTERFACE ;
        }

        AddRef();

        return S_OK;
	}

	unsigned long __stdcall AddRef()
	{
		return InterlockedIncrement( &m_RefCount );
	}

	unsigned long __stdcall Release()
	{
        long nRefCount = 0;

        nRefCount=InterlockedDecrement( &m_RefCount ) ;

        if ( nRefCount == 0 ) 
			delete this;

        return nRefCount;
	}

    HRESULT STDMETHODCALLTYPE putDocumentLocator( ISAXLocator *pLocator )
    {
		return S_OK;
    }
	
	HRESULT STDMETHODCALLTYPE startDocument( void )
	{
		return S_OK;
	}

    HRESULT STDMETHODCALLTYPE endDocument( void )
    {
		return S_OK;
    }

    HRESULT STDMETHODCALLTYPE startPrefixMapping(
		const wchar_t *pwchPrefix,
		int cchPrefix,
		const wchar_t *pwchUri,
		int cchUri )
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE endPrefixMapping(
		const wchar_t *pwchPrefix,
		int cchPrefix )
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE startElement(
		const wchar_t *pwchNamespaceUri,
		int cchNamespaceUri,
		const wchar_t *pwchLocalName,
		int cchLocalName,
		const wchar_t *pwchRawName,
		int cchRawName,
		ISAXAttributes *pAttributes )
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE endElement(
		const wchar_t *pwchNamespaceUri,
		int cchNamespaceUri,
		const wchar_t *pwchLocalName,
		int cchLocalName,
		const wchar_t *pwchRawName,
		int cchRawName )
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE characters(
		const wchar_t *pwchChars,
		int cchChars )
	{
		return S_OK;
	}
   
	HRESULT STDMETHODCALLTYPE ignorableWhitespace(
		const wchar_t *pwchChars,
		int cchChars)
	{
		return S_OK;
	}
   
	HRESULT STDMETHODCALLTYPE processingInstruction(
		const wchar_t *pwchTarget,
		int cchTarget,
		const wchar_t *pwchData,
		int cchData )
	{
		return S_OK;
	}
   
	HRESULT STDMETHODCALLTYPE skippedEntity(
		const wchar_t *pwchName,
		int cchName )
	{
		return S_OK;
	}
};