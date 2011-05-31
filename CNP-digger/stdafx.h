
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <MsXml2.h>




#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


// Worker thread cmds
#define WM_CHECK_FOR_ESSENTIAL_FILES	WM_USER + 1
#define WM_PARSE_MEDICS_XML				WM_USER + 2
#define WM_PARSE_CITIES_XML				WM_USER + 3
#define WM_INIT_MEDIC					WM_USER + 4


// Needed files and folders
#define PERSONS_DB		L"Persons.db3"
#define MEDICS_XML		L"Medics.xml"
#define CITiES_XML		L"Cities.xml"
#define EXPORTS_DIR		L"Exports"
#define PATIENTS_DIR	L"Patients"
#define TEMP_DIR		L"Temp"

#define WORKER_THREAD_STOP_TIMEOUT		2000


// XML Labels
#define XML_CITIES		L"cities"
#define XML_CITY		L"city"
#define XML_CITY_NAME	L"cityName"
#define XML_CITY_CODE	L"cityCode"

#define XML_MEDICS				L"medics"
#define XML_MEDIC				L"medic"
#define XML_MEDIC_LAST_NAME		L"lastName"
#define XML_MEDIC_FIRST_NAME	L"firstName"
#define XML_MEDIC_ID			L"id"

// General constants
#define XML_HEADER			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
#define CITY_DISTRICT_CODE	L"TR"
#define INIT_MEDICS_HASH_TABLE	29
#define INIT_CITIES_HASH_TABLE	499

// Structs
struct MEDIC
{
	CString strID,
			strLastName,
			strFirstName;
};

struct CITY
{
	CString strID,
			strName,
			strDistrict;
};

struct PATIENT
{
	CString strID,
			strLastName,
			strFirstName,
			strCityName;
};

typedef CMap<CString, LPCWSTR, MEDIC, MEDIC&> CMapStringToMedic;
typedef CMap<CString, LPCWSTR, CITY, CITY&>   CMapStringToCity;
typedef CList<PATIENT, PATIENT&>			  CListPatients;

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


