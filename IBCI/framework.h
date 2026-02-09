#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원

#include <afxsock.h>		// MFC socket extensions


// Additional Libray
# include <XTToolkitPro.h>	// Codejock Software Components
# include <ipvmbasedef.h>	// Intekplus Vision Library

#include <stdint.h>
#include "Types/Image_8u_C1.h"
#include "Types/Image_8u_C3.h"
#include "Types/Image_8u_C4.h"
#include "Types/Image_16u_C1.h"
#include "Types/Image_32S_C1.h"
#include "Types/Rect.h"
#include "Types/Rect_32f.h"
#include <queue>



//---- Need to Redefine
struct DefectPointXY
{
	long nX = -999;
	long nY = -999;
	COLORREF Color;
	long nIndex = 0;
	long nImageSizeX = 0;
	CString strLotID=_T("");
};

struct DefectPointRect
{
	IPVM::Rect rt;
	long nX = -999;
	long nY = -999;
	COLORREF Color;
	long nIndex = 0;
	long nImageSizeX = 0;
	CString strLotID = _T("");
};

typedef struct _stReslutLaneData
{
	long ptXLaneLeft = 0;//
	long ptXLaneRight = 0;//
	float fLaneWidth = 0.f;
	

	//float fMeasureStandard = 0.f;

	//float fMeasureOffsetUCL = 0.f;
	//float fMeasureOffsetLCL = 0.f;

	//float fMeasureOffsetUSL = 0.f;
	//float fMeasureOffsetLSL = 0.f;
	


} ResultLaneData;

typedef struct  _stResultSpec // setSpec
{
	float fMeasureStandard = 0.f;

	float fMeasureOffsetUCL = 0.f;
	float fMeasureOffsetLCL = 0.f;

	float fMeasureOffsetUSL = 0.f;
	float fMeasureOffsetLSL = 0.f;

} ResultSpec;
typedef struct _stResultRollPress
{

} ResultRollPress;
typedef struct  _stResultMismatch
{
	std::vector<ResultLaneData> m_vecResUpLane;
	std::vector<ResultLaneData> m_vecResDownLane; //no
	std::vector<ResultSpec> m_vecResSpec;

	std::vector<float> m_vecPtXDiffLeft; // nno
	std::vector<float> m_vecPtXDiffRight; //no
	std::vector<float> m_vecfDiffWidth;

	long m_nType = 0;
	long m_nCellIndex = 0;
	CString m_strLotID = _T("");
	CString m_strBright = _T("");

	float m_fTopFullLength = 0.f;
	float m_fBackFullLength = 0.f;

} ResultMismatch;


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif



#define WM_CONNECT_SOCKET WM_USER + 200
#define WM_RECEIVE_SOCKET WM_USER + 210
#define WM_CLOSE_SOCKET WM_USER + 220
#define WM_ACCEPT_SOCKET WM_USER + 300
#define WM_CLOSE_SERVER WM_USER + 310