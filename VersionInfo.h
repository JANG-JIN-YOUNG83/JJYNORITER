#pragma once

//##################################################################
// Versioning Rule : major.minor.revision.testNumber
//
// 프로그램이 버전업되면 Host/Vision의 버전은 통일하여 함께 올린다
// IBNI_TEST_NUMBER: 정식버전은 0으로 고정 (테스트할 배포중에는 증가)
//
// A: 이현식 수석 
// B: 김남욱 책임
// C: 김응민 책임
// D: 최승만 연구원 
// E: 안예진 연구원
//##################################################################

#define IBCI_MAJOR_VERSION	1
#define IBCI_MINOR_VERSION	0
#define IBCI_REVISION		0
#define IBCI_TEST_NUMBER	1B


#define IBCI_MACRO_COMPANY					"IntekPlus"
#define	IBCI_MACRO_PRODUCT					"IBCI"
#define IBCI_MACRO_VERSION_STR_EXPAND(tok)	#tok
#define IBCI_MACRO_VERSION_STR(tok)			IBCI_MACRO_VERSION_STR_EXPAND(tok)
#define IBCI_MACRO_VERSION					IBCI_MACRO_VERSION_STR(IBCI_MAJOR_VERSION) "." IBCI_MACRO_VERSION_STR(IBCI_MINOR_VERSION) "." IBCI_MACRO_VERSION_STR(IBCI_REVISION) "." IBCI_MACRO_VERSION_STR(IBCI_TEST_NUMBER)
		  
#define IBCI_MACRO_STRVERSION1				IBCI_MACRO_VERSION_STR(IBCI_MAJOR_VERSION) "." IBCI_MACRO_VERSION_STR(IBCI_MINOR_VERSION) "." IBCI_MACRO_VERSION_STR(IBCI_REVISION) "." IBCI_MACRO_VERSION_STR(IBCI_TEST_NUMBER) "\0"
#define IBCI_MACRO_STRVERSION2				IBCI_MAJOR_VERSION,IBCI_MINOR_VERSION,IBCI_REVISION,IBCI_TEST_NUMBER
#define IBCI_MACRO_COMPANY_NAME_STR			"(c) IntekPlus"
#define IBCI_MACRO_LEGAL_COPYRIGHT_STR		"Copyright (C) 2017 IntekPlus Corporation"
#define IBCI_MACRO_PRODUCT_NAME_STR			"IntekPlus IBCI"

#define _USE_SEND_CHANGED_PARAMETER_
//#define _USE_CIS_LIVE_

#if IBCI_REVISION >= 27
	#define _NOT_USED_HISTORY_MESSAGE_	// 사용되지 않는 HISTORY 관련 메시지 지우기
#endif

#define _USE_INTERFACE_REVIEW_				// Review 기능
//#define _TEST_SPLITTERREOVED_
//#define _CIS_NOT_MAXWINDOW_ 
#define _SEND_CELLID_
#define _INSULATESURFACE_
#define _BURR_

//#define _ISI_
#define _DRAW_DEFECT_
//#define _SEND_SOCKET_IMAGE_JPEG_
