#pragma once

#define SQLITE_OK 0
typedef struct sqlite3 sqlite3;

class CSQLiteWrapper
{
// Constructor / Destructor
public:
	CSQLiteWrapper();
	CSQLiteWrapper( CString strDll, CString strFile );
	virtual ~CSQLiteWrapper();

// Methods
public:
	void SetDll( CString strDll );
	void SetFile( CString strFile );

	CString GetDll();
	CString GetFile();

	BOOL LoadDll( CString strDll );
	BOOL LoadDll();
	BOOL UnloadDll();
	BOOL SQLiteOpen( CString strFile );
	BOOL SQLiteOpen();
	BOOL SQLiteExec( CString strSQL, int (*pCallback)( void*,int,char**,char** ), CString strErrMsg = NULL );
	BOOL SQLiteFree();
	BOOL SQLiteClose();

	BOOL ToUTF8( wchar_t *pszIn, int nInLen, char *pszOut, int *nOutLen );

// Members
protected:
	HINSTANCE m_hSQLiteDll;
	sqlite3   *m_pDb;
	CString   m_strDll,
			  m_strFile;

	typedef int (*SQLITE3_OPEN)( const char *pszFilenameUTF8, sqlite3 **ppDb );
	SQLITE3_OPEN sqlite3_open;

	typedef int (*SQLITE3_EXEC)( sqlite3*, const char *pszSQL, int (*pCallback)( void*, int, char**, char** ), void*, char **pErrmsg );
	SQLITE3_EXEC sqlite3_exec;

	typedef void (*SQLITE3_FREE)( void* );
	SQLITE3_FREE sqlite3_free;

	typedef int (*SQLITE3_CLOSE)( sqlite3 * );
	SQLITE3_CLOSE sqlite3_close;
};