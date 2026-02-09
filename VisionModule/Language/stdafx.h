// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define _XTP_EXCLUDE_CALENDAR
#define _XTP_EXCLUDE_CHART
#define _XTP_EXCLUDE_COMMANDBARS
//#define _XTP_EXCLUDE_COMMON
//#define _XTP_EXCLUDE_CONTROLS
#define _XTP_EXCLUDE_DOCKINGPANE
#define _XTP_EXCLUDE_FLOWGRAPH
//#define _XTP_EXCLUDE_GRAPHICLIBRARY
//#define _XTP_EXCLUDE_PROPERTYGRID
#define _XTP_EXCLUDE_REPORTCONTROL
#define _XTP_EXCLUDE_RIBBON
#define _XTP_EXCLUDE_SHORTCUTBAR
#define _XTP_EXCLUDE_SKINFRAMEWORK
#define _XTP_EXCLUDE_SYNTAXEDIT
#define _XTP_EXCLUDE_TABMANAGER
#define _XTP_EXCLUDE_TASKPANEL
//#define _XTP_EXCLUDE_MARKUP
#include <XTToolkitPro.h>

#define __LANGUAGE_CLASS_EXPORT__
