// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 클래스입니다.
#include <afxodlgs.h>       // MFC OLE 대화 상자 클래스입니다.
#include <afxdisp.h>        // MFC 자동화 클래스입니다.
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 데이터베이스 클래스입니다.
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 데이터베이스 클래스입니다.
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT



#define DUMMY_PIXEL_COUNT 68
#define PI           3.1415926535
#define WIDTHBYTES(bits) (((bits)+31)/32*4)


#include "ipvmbasedef.h"
#include <stdint.h>
#include "Types/Image_8u_C1.h"
#include "Types/Image_16u_C1.h"
#include "Types/Image_8u_C3.h"
#include "Types/Point_8u_C3.h"
#include <queue>
#include <cassert>

#define _XTP_EXCLUDE_CALENDAR
#define _XTP_EXCLUDE_CHART
#define _XTP_EXCLUDE_COMMANDBARS
//#define _XTP_EXCLUDE_COMMON
//#define _XTP_EXCLUDE_CONTROLS
#define _XTP_EXCLUDE_DOCKINGPANE
//#define _XTP_EXCLUDE_FLOWGRAPH
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
#include <afxdialogex.h>

#define RESET_IMAGE_BUFFER_WHEN_START_INSPECTION
// 0 - LGES 
// 1 - UC1
#define INSPECTION_MACHINE_UC1	0

//#define RESET_IMAGE_BUFFER_WHEN_MODULE_INSPECTION
#define __INSPECTION_CLASS_EXPORT__

#include "../VersionInfo.h"
#include "../VisionModule/Base/basedef.h"
#include <afxcontrolbars.h>
#include <Algorithm/ImageProcessing.h>