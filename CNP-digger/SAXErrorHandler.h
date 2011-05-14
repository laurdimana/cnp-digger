#pragma once 

//#include <MsXml2.h>

class SAXErrorHandler : public ISAXErrorHandler
{
	long m_RefCount;

public:
	SAXErrorHandler() : m_RefCount( 0 ) {}
	virtual ~SAXErrorHandler() {}

	long __stdcall QueryInterface( const struct _GUID &riid, void **ppObj )
	{
		  if ( riid == IID_IUnknown )
		  {
			 *ppObj = static_cast<IUnknown*>( this );
		  }

		  if ( riid == __uuidof( ISAXErrorHandler ) )
		  {
			 *ppObj = static_cast<ISAXErrorHandler*>( this );
		  }
		  else
		  {
			 *ppObj = NULL ;
			 return E_NOINTERFACE ;
		  }

		  AddRef();

		  return S_OK;
	}

	unsigned long __stdcall AddRef( void )
	{
		return InterlockedIncrement( &m_RefCount );
	}

	unsigned long __stdcall Release( void )
	{
		long nRefCount = 0;

		nRefCount = InterlockedDecrement( &m_RefCount );

		if ( nRefCount == 0 ) 
			delete this;

		return nRefCount;
	}

	HRESULT STDMETHODCALLTYPE error(
			ISAXLocator *pLocator,
			const wchar_t *pwchErrorMessage,
			HRESULT errCode ) 
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE fatalError(
			ISAXLocator *pLocator,
			const wchar_t * pwchErrorMessage,
			HRESULT errCode )
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ignorableWarning(
			ISAXLocator *pLocator,
			const wchar_t *pwchErrorMessage,
			HRESULT errCode )
	{
		return S_OK;
	}
};