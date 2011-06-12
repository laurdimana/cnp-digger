#include "stdafx.h"
#include "SQLiteWrapper.h"

//////////////////////////////////////////////// Constructor / Destructor //////////////////////////////////////////////

CSQLiteWrapper::CSQLiteWrapper()
{
	m_pDb = NULL;
}

CSQLiteWrapper::CSQLiteWrapper( CString strDll, CString strFile )
{
	SetDll( strDll );
	SetFile( strFile );

	m_pDb = NULL;
}

CSQLiteWrapper::~CSQLiteWrapper()
{
	if ( m_pDb )
	{
		SQLiteFree();
		SQLiteClose();
	}

	::FreeLibrary( m_hSQLiteDll );
}

//////////////////////////////////////////////// Methods ////////////////////////////////////////////////////////////

void CSQLiteWrapper::SetDll( CString strDll )
{
	m_strDll = strDll;
}

void CSQLiteWrapper::SetFile( CString strFile )
{
	m_strFile = strFile;
}

CString CSQLiteWrapper::GetDll()
{
	return m_strDll;
}

CString CSQLiteWrapper::GetFile()
{
	return m_strFile;
}

BOOL CSQLiteWrapper::LoadDll( CString strDll )
{
	SetDll( strDll );

	return LoadDll();
}

BOOL CSQLiteWrapper::LoadDll()
{
	m_hSQLiteDll = ::LoadLibrary( m_strDll.GetBuffer() );

	if ( !m_hSQLiteDll )
	{
		TRACE( L"@ CSQLiteWrapper::LoadDll -> Failed LoadLibrary [%ld]\n", GetLastError() );

		return FALSE;
	}

	sqlite3_open = (SQLITE3_OPEN)::GetProcAddress( m_hSQLiteDll, "sqlite3_open" );

	if ( !sqlite3_open )
	{
		TRACE( L"@ CSQLiteWrapper::LoadDll -> Failed GetProcAddress for sqlite3_open [%ld]\n", GetLastError() );

		return FALSE;
	}

	sqlite3_exec = (SQLITE3_EXEC)::GetProcAddress( m_hSQLiteDll, "sqlite3_exec" );

	if ( !sqlite3_exec )
	{
		TRACE( L"@ CSQLiteWrapper::LoadDll -> Failed GetProcAddress for sqlite3_exec [%ld]\n", GetLastError() );

		return FALSE;
	}

	sqlite3_free = (SQLITE3_FREE)::GetProcAddress( m_hSQLiteDll, "sqlite3_free" );

	if ( !sqlite3_free )
	{
		TRACE( L"@ CSQLiteWrapper::LoadDll -> Failed GetProcAddress for sqlite3_free [%ld]\n", GetLastError() );

		return FALSE;
	}

	sqlite3_close = (SQLITE3_CLOSE)::GetProcAddress( m_hSQLiteDll, "sqlite3_close" );

	if ( !sqlite3_close )
	{
		TRACE( "@ CSQLiteWrapper::LoadDll -> Faield GetProcAddress for sqlite3_close [%ld]\n", GetLastError() );

		return FALSE;
	}

	return TRUE;
}

BOOL CSQLiteWrapper::UnloadDll()
{
	return ::FreeLibrary( m_hSQLiteDll );
}

BOOL CSQLiteWrapper::SQLiteOpen( CString strFile )
{
	SetFile( strFile );

	return SQLiteOpen();
}

BOOL CSQLiteWrapper::SQLiteOpen()
{
	char *pszUTF8 = new char[ m_strFile.GetLength() * 2 ];
	int  nUTF8;

	if ( !ToUTF8( m_strFile.GetBuffer(), m_strFile.GetLength(), pszUTF8, &nUTF8 ) )
	{
		delete [] pszUTF8;
		TRACE( L"@ CSQLiteWrapper::SQLiteOpen -> Failed ToUTF8\n" );

		return FALSE;
	}
	pszUTF8[ nUTF8 ] = '\0';

	int rc = sqlite3_open( pszUTF8, &m_pDb );

	if ( rc )
	{
		TRACE( L"@ CSQLiteWrapper::SQLiteOpen -> Failed sqlite3_open\n" );
		delete [] pszUTF8;

		return FALSE;
	}

	delete [] pszUTF8;

	return TRUE;
}

BOOL CSQLiteWrapper::SQLiteExec( CString strSQL, int (*pCallback)( void*, int, char**, char** ), CString strErrMsg )
{
	char *pszUTF8 = new char[ strSQL.GetLength() * 2 ];
	int  nUTF8;
	char *pszErrMsg = 0;

	if ( !ToUTF8( strSQL.GetBuffer(), strSQL.GetLength(), pszUTF8, &nUTF8 ) )
	{
		TRACE( L"@ CSQLiteWrapper::SQLiteExec -> Failed ToUTF8\n" );
		delete [] pszUTF8;

		return FALSE;
	}
	pszUTF8[ nUTF8 ] = '\0';

	int rc = sqlite3_exec( m_pDb, pszUTF8, pCallback, 0, &pszErrMsg );

	if ( rc )
	{
		TRACE( L"@ CSQLiteWrapper::SQLiteExec -> Failed sqlite3_exec [%s]\n", pszErrMsg );
		SQLiteFree();
		delete [] pszUTF8;

		if ( strErrMsg )
			strErrMsg.Format( L"%s", pszErrMsg );

		return FALSE;
	}

	delete [] pszUTF8;

	return TRUE;
}

BOOL CSQLiteWrapper::SQLiteFree()
{
	sqlite3_free( 0 );

	return TRUE;
}

BOOL CSQLiteWrapper::SQLiteClose()
{
	sqlite3_close( m_pDb );

	return TRUE;
}

BOOL CSQLiteWrapper::ToUTF8( wchar_t *pszIn, int nInLen, char *pszOut, int *nOutLen )
{
	*nOutLen = ::WideCharToMultiByte(
		CP_UTF8, 0,
		pszIn, nInLen,
		NULL, 0, NULL, NULL );

	if ( *nOutLen == 0 )
		return FALSE;

	::WideCharToMultiByte(
		CP_UTF8, 0,
		pszIn, nInLen,
		pszOut, *nOutLen,
		NULL, NULL );

	return TRUE;
}
