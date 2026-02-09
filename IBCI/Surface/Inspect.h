// Inspect.h: interface for the CInspect class.
//////////////////////////////////////////////////////////////////////
//221116
//parameter 4개 추가
//BOOL	m_bCoatWrinkleUse[MAX_CAMERA];//코팅 주름 검사 유무
//double  m_dCoatWrinkleAreamm[MAX_CAMERA];//에지에서 코팅 주름 검사 영역 //전극 에지에서 일정 영역 단위 mm
//int		m_nCoatWrinkleTh[MAX_CAMERA];//코팅 주름 TH (주름영역밝기 - 코팅밝기 > 코팅주름 TH 보다 크면 불량)
//BOOL	m_bInsulAdjust[MAX_CAMERA];//절연 보정 사용 유무


//220804
//Parameter 3개 추가
//BOOL	m_bScrabSizeAndOr[MAX_CAMERA];//스크랩 사이즈 And Or   0 or 1 and  
//double m_dScrabSpecSizeX[MAX_CAMERA];//scrab size X
//double m_dScrabSpecSizeY[MAX_CAMERA];//scrab size Y
//220723
//미코팅 스킵영역 탭에지에서 64픽셀 -> 32픽셀로 조정 - 중국 미코팅 미검출
//라인 수직 각도만 적용. 파라미터내 각도부터 90도까지만 라인으로 적용되고 스크래치 파라미터 각도 이하의 경우 크랙으로 인식되게 수정.
//220721
//절연/오버레이폭 보정 사용 유무 추가 - 기존 무조건 보정 사용에서 옵션화 처리
//BOOL	m_bOverlayCorrection[MAX_CAMERA];//절연 보정 사용유무 - 파라미터 처리 필요.
//220714
//NG Tag 관련 탭우측 검사영역 옵셋 수정 50pixel -> 5pixel
//220712 //파라미터 아일랜드 과검 관련 2개 추가
//BOOL	m_bTabEdgerangeTh[MAX_CAMERA];//탭부 에지(상/하부 ) 전극 밝기로 이진화
//double	m_dTabEdgeRangemm[MAX_CAMERA];//탭 상하부 전극 밝기 TH 적용 mm

//220630
//표면검사 흑검출시 0.4mm 이후 위치를 확인하여 흑밝기가 나올경우만 불량 후보로 등록 - 흑색 불량 최소 검출사이즈가 5mm임
//220610
//BOOL	m_bSbrIsnadImage[MAX_CAMERA];//TRUE BrightIamge False:Dark Image  //추가 파라미터  //SBR 절연내 다크 검사-False Dark Image TRUE :BrightImage
//SBR 절연 내 흑불량 Island로 불량 코드 수정
//셀마진 상/하 적용 검사영역 수정.
//220608
//오버레이 사이즈 보정후 렉트 위치 수정
//크랙검출시 사이즈 재측정
//220523
// 나누기 부분 예외처리 추가
//미코팅부 흑 검사 추가 - 추가 파라미터 4개 필요.
//int		m_nNoncoatBlackTh[MAX_CAMERA];//미코팅 영역 다크 TH   - 20 이하 입력시 20으로 치환됨. 제품 평균밝기 - 설정TH 값으로 이진화
//BOOL	m_bNoncoatDarkInsp[MAX_CAMERA];//미코팅 영역 다크 검사 //사용 유무
//double  m_dDarkNonCoatSizeX[MAX_CAMERA];//미코팅 검사 흑색 불량 Spec Size
//double  m_dDarkNonCoatSizeY[MAX_CAMERA];//미코팅 검사 흑색 불량 Spec Size

//2220518 
//m_rtNotchArea ->Vhome x 전극 에지에 맞게 Offset X 추가
//220517
//탭 에지 영상 선택  - 이용재 책임 요청
//m_bTabEdgeSelectImage[MAX_CAMERA];//TRUE BrightImage FALSE-DarkImage
//notch V-home 고정형 사용 유무 추가 
//CRect	m_rtFixedVhome[MAX_CAMERA][4];//고정된 4개의 V Home 위치 - 여기에 Tracker 위치를 넣어주세요.
//BOOL	m_bFixedVhome[MAX_CAMERA];//TRUE 시 사용 FALSE시 미사용
//테이프 전처리 이미지 사용 유무 //오창 테이프 끈끈이 관련 테스트용
//m_bTapePreUse  - 기존 전극면 이미지 편차가 특정값 이상일 경우만 전처리 검사에서 양품 이미지에 대해서도 옵션으로 검사 유무 선택
//220516
//ESNB 전극에지에서 탭 찾는 범위 설정 
//m_nTabEdgeRightSkipPixel  /신규 파라미터 추가 필요. 단위 픽셀
//ESMI 에지에 붙은 미코팅 관련
//m_bNoncoatEdgeCheck[MAX_CAMERA] =파라미터 추가 필요. 에지에 붙은 미코팅 사용 유무 , 사용시 에지에 파라미터 픽셀 이내에 붙은 것만 미코팅으로 빼고 나머지는 핀홀 라인 스펙을 따라감.
//m_nNonCoatingEdgeMargin[MAX_CAMERA] = 파라미터 추가 필요 - 단위 픽셀. 에지에서 몇픽셀 이내까지 붙은것으로 할지 정하는 파라미터 
//Time Out 관련
//m_nTimeOutAlarmcount[MAX_CAMERA] = Parameter 추가 필요. 설정값 이상 연속으로 타임아웃발생시 inspect 불량개수 리턴값이 -9999를 리턴함.
//220513
//탭에지 관련 신규 파라미터 추가
//int		m_nTabEdgeCriteria[MAX_CAMERA];// 탭 기준 - 에지 상하부 기준 카메라별로 다를수가 있음. 0 이미지 윗쪽 1 이미지 아래쪽 기준으로 탭위치보다 작을경우 다시 찾는다.
//double m_dTabWidth[MAX_CAMERA];//탭폭에 대한 인자// 탭폭 체크용으로 추가  인스펙트함수에서 Call
//double m_dTabEdgeInspRange[MAX_CAMERA];//탭 높이 검사 옵셋 -  우측 에지 검사영역 상하 높이
//double m_dTabRightOffset[MAX_CAMERA];//탭 우측 옵셋 -  전극기준 몇mm 부터 좌측으로 에지를 찾을지 정하는 파라미터
// inspect 평션에 double dTabWidth, 변수 인자로 추가됨. 탭 폭을 넣어주면됩니다.
//int CInspect::Inspect(IPVM::LoggerInterface& //m_loggerDebug.Log, int nCam, BYTE* fmEven, BYTE* fmOdd, int nWidth, int nHeight, int nInsulateType, int nCellMargin, int nOffsetUp, int nOffsetDn, double dTabWidth,int nTabPos)



#if !defined(AFX_INSPECT_H__1387D31A_280C_4020_B39F_BE013B2EEADD__INCLUDED_)
#define AFX_INSPECT_H__1387D31A_280C_4020_B39F_BE013B2EEADD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Base/LoggerInterface.h"
#include <vector>
#include <deque>
#include "fchain.h"
#include "../SystemParameter.h"

#include "../../Inspection/zipper/Inspectionzipper.h"
#include "../../Inspection/Classification/Classification.h"

#define MAX_IMAGE_WIDTH	30000
#define	MAX_HUGEDEFECT	15
#define BAD_HEIGHT		256
#define BAD_WIDTH		256
#define MOD_NUMBER	8
#define MAX_SPLIT_DEFECT	5
#define MAX_CAMERA	2
#define MAX_FULL_DEFECT	40

#define MAX_MANUAL_DEFECT	500

#define DT_WHITE	0
#define DT_DARK		1
#define DT_TOTAL	2


//#define ORIGIN_PROCESS

//typedef struct tagInspParam{
//	CString strName;//파라미터 이름
//	double dValue;	//파라미터 Value
//	int		nIndex; //파라미터 관린 Index
//	BOOL bEnable;
//	int nObject; // 0 양극 1 음극
// 	tagInspParam(){
// 		strName = _T("");
// 		dValue = 0;
// 		nIndex = 0;
//		bEnable = TRUE;
// 	}
//}InspParam;

typedef struct tagSpecParam{
	CString strName;//파라미터 이름
	double dValue;	//파라미터 Value
	int		nIndex; //파라미터 관린 Index
	BOOL	bEnable;

	tagSpecParam(){
		strName = _T("");
		dValue = 0;
		nIndex = 0;
		bEnable = TRUE;
	}
}SpecParam;



class CInspect
{
private:
	//20230417 PreprocessEdge 함수 최적화 관련
	BYTE* m_fmLeftNonCoat[MAX_CAMERA];
	BYTE* m_fmRightNonCoat[MAX_CAMERA];
	BYTE* m_fmLeftDarkInsp[MAX_CAMERA];
	BYTE* m_fmRightDarkInsp[MAX_CAMERA];
	int m_nLeftNonCoatBufferWidth[MAX_CAMERA];
	int m_nLeftNonCoatBufferHeight[MAX_CAMERA];
	int m_nRightNonCoatBufferWidth[MAX_CAMERA];
	int m_nRightNonCoatBufferHeight[MAX_CAMERA];

	BYTE* m_fmtmp[MAX_CAMERA];
	BYTE* m_fmBlack[MAX_CAMERA];
	int m_nSpurBufferSizeX[MAX_CAMERA];
	int m_nSpurBufferSizeY[MAX_CAMERA];

	BYTE* m_fmInsul[MAX_CAMERA];  //20230830  장재성, rect size 바꾸면 죽는 문제로 기존 함수로 변경
	BYTE* m_fmInsulDark[MAX_CAMERA];
	BYTE* m_fmInsulGap[MAX_CAMERA];
	int m_nInsulateBufferWidth[MAX_CAMERA];
	int m_nInsulateBufferHeight[MAX_CAMERA];
	//20230417 PreprocessEdge 함수 최적화 관련
public:

	//221201  절연갭관련 추가 파라미터
	double	m_dInsulgapTh[MAX_CAMERA];//절연 갭 검사 TH
	BOOL	m_bInsulGapUse[MAX_CAMERA];//절연 갭 검사 유무
	//221121 YYS 전극 Edge Align 신규 알고리즘 관련 파라미터
	bool m_bUseIntekCellAlign[MAX_CAMERA];//Intekplus 셀 얼라인 알고리즘 사용 여부
	bool m_bSearchImage[MAX_CAMERA];//TRUE - Bright FALSE - Dark 엣지 탐색 이미지
	int m_nElectrodeEdgeTh[MAX_CAMERA];//전극 Edge Threshold
	int m_nEdgeContinuousCount[MAX_CAMERA];//Threshold 값을 몇번 연속으로 만족해야 Edge로 삼을지
	int m_nEdgeSearchAreaSizeY[MAX_CAMERA];//Edge Search 영역 Y 사이즈
	int m_n2ndEdgeCount[MAX_CAMERA];//2차 엣지 포인트 갯수 카운트
	//221121 YYS 전극 Edge Align 신규 알고리즘 관련 파라미터
	
	//221118 YYS 전극 Edge 관련 신규 파라미터
	int m_nLeftElecEdgeSearchAreaSizeX[MAX_CAMERA];
	int m_nRightElecEdgeSearchAreaSizeX[MAX_CAMERA];
	int m_nElecEdgeSearchAreaTopBotOffset[MAX_CAMERA];
	int m_nCompareDistance[MAX_CAMERA];
	//221118 YYS 전극 Edge 관련 신규 파라미터
	
	//221115 YYS 너울성 불량
	bool m_bBumpyInspect[MAX_CAMERA];
	int m_nBumpyAreaSize[MAX_CAMERA];
	int m_nBumpyTh[MAX_CAMERA];
	int m_nBumpyEdgeTh[MAX_CAMERA];
	int m_nBumpyCompareCount[MAX_CAMERA];
	//221026
	bool m_bLaserAndCathode[MAX_CAMERA];
	//221025
	int m_ninspCount;
	bool m_bInspDarkRejectAtBright[MAX_CAMERA];
	//221013
	float m_fLimitTime[MAX_CAMERA];
	BOOL m_bUseMinimumDarkSpec[MAX_CAMERA];
	//220915 YYS 탭 얼라인 관련
	BOOL m_bUseIntekTabAlign[MAX_CAMERA];//Intekplus 탭 얼라인 알고리즘 사용 여부
	BOOL m_bDetectTabBothSide[MAX_CAMERA];//탭 Top/Bottom 엣지를 Top과 Bottom에서 모두 Detecting할지 여부
	int m_nTabThresholdTopSide[MAX_CAMERA];//탭 상부 엣지 threshold
	int m_nTabThresholdBottomSide[MAX_CAMERA];//탭 하부 엣지 threshold
	int m_nTabTapeThresholdTopSide[MAX_CAMERA];//탭 테이프 상부 엣지 threshold
	int m_nTabTapeThresholdBottomSide[MAX_CAMERA];//탭 테이프 하부 엣지 threshold
	int m_nShadowThreshold[MAX_CAMERA];//탭 상하 그림자 threshold
	int m_nRollerEndThreshold[MAX_CAMERA];//롤러 종료 포인트 측정 Threshold
	BOOL m_bTabTopBottomForceCorrection[MAX_CAMERA];//탭 Top Bottom Y 좌표 강제 보정 여부 
	BOOL m_bTabRightPosCorrection[MAX_CAMERA];//탭 오른쪽 엣지 잘못 측정됐을 때 강제 보정 여부
	int m_nTabEdgeSearchHeightPixel[MAX_CAMERA];//탭 엣지 검색 높이 픽셀
	float m_fEdgePointFilterValue[MAX_CAMERA];//탭 엣지 포인트 필터링 값 (포인트의 범위 값)
	CRect m_rectTabROI[MAX_CAMERA];//탭 Top/Bottom 엣지 ROI
	CRect m_rectTabRightDetectROI[MAX_CAMERA];//탭 Right 엣지 ROI
	int m_nTabAvergeValue[MAX_CAMERA];//탭 평균밝기
	BOOL m_bForceCorrectionActivated[MAX_CAMERA];
	//220915 YYS 탭 얼라인 관련
	//221103 YYS 전극 엣지 얼라인 관련
	std::vector <int> m_vecLeftElecEdgeCandidateX[MAX_CAMERA];
	std::vector <int> m_vecLeftElecEdgeCandidateY[MAX_CAMERA];
	std::vector <int> m_vecRightElecEdgeCandidateX[MAX_CAMERA];
	std::vector <int> m_vecRightElecEdgeCandidateY[MAX_CAMERA];
	//221103 YYS 전극 엣지 얼라인 관련
	//220926 YYS 절연 디버깅 관련
	std::vector <int> m_vecPeakEdge[MAX_CAMERA];
	
	CRect m_insulateROI[MAX_CAMERA];//절연 폭 ROI
	CRect m_overlayROI[MAX_CAMERA];//오버레이 ROI
	int m_nInsulateROIOffset[MAX_CAMERA];
	double m_dInsulateSortData[MAX_CAMERA][10];
	double m_dResSortData[MAX_CAMERA];
	int m_nInsulateRoi10eaSplit[10][1000];
	double m_dInsulWidthCandidate[MAX_CAMERA][10];
	int m_nOverlayROI_Avg[MAX_CAMERA][10000];
	int m_nOverlayROI_SplitAvg[10][2000];
	int m_nRealCalcOverlayPos[MAX_CAMERA];
	double m_dRealOverlayWidth[MAX_CAMERA];
	int m_nCorrectionOverlayPos[MAX_CAMERA];
	BOOL m_bInspectOverlayAtPreprocess[MAX_CAMERA];
	std::vector <double> m_dInspTimes[MAX_CAMERA];
	CCriticalSection m_csInspTime[MAX_CAMERA];
	std::vector <double> m_dPreProcessEdgeTimes[MAX_CAMERA];
	CCriticalSection m_csPreProcessEdgeTime[MAX_CAMERA];
	//220926 YYS 절연 디버깅 관련

public:
	//220915 YYS 탭 얼라인 관련
	std::vector <long> m_vecTopY_Candidate[MAX_CAMERA];
	std::vector <long> m_vecBottomY_Candidate[MAX_CAMERA];
	std::vector <long> m_vecTopX_Candidate[MAX_CAMERA];
	std::vector <long> m_vecBottomX_Candidate[MAX_CAMERA];
	std::vector <long> m_vecTopY_Excepted[MAX_CAMERA];
	std::vector <long> m_vecBottomY_Excepted[MAX_CAMERA];
	std::vector <long> m_vecTopX_Excepted[MAX_CAMERA];
	std::vector <long> m_vecBottomX_Excepted[MAX_CAMERA];
	//220915 YYS 탭 얼라인 관련
private:
	//220915 YYS 탭 얼라인 관련
	std::vector <long> m_vecTopY_ValueList[MAX_CAMERA];
	std::vector <long> m_vecBottomY_ValueList[MAX_CAMERA];
	std::deque <int> m_dequeTabAvg[MAX_CAMERA];
	std::deque <int> m_dequeTabTopY[MAX_CAMERA];
	std::deque <int> m_dequeTabBottomY[MAX_CAMERA];
	CCriticalSection m_lockTabAvg[MAX_CAMERA];
	CCriticalSection m_lockTabTopY[MAX_CAMERA];
	CCriticalSection m_lockTabBottomY[MAX_CAMERA];
	//220915 YYS 탭 얼라인 관련
public:
	Inspectionzipper m_zipper;

	IPVM::Rect	m_rtZipper2Offset[MAX_CAMERA];  //zipper2
	long m_nSweepAngle[MAX_CAMERA];
	long m_nSweepCount[MAX_CAMERA];
	long m_nSweepLength[MAX_CAMERA];
	float m_fStartpointThresholdOffset[MAX_CAMERA];
	float m_fTabAreaThresholdOffset[MAX_CAMERA];
	float m_fInsulationStartpointThresholdOffset[MAX_CAMERA];
	float m_fInsulationTabAreaThresholdOffset[MAX_CAMERA];
	float m_dAverageFilter[MAX_CAMERA];
	long m_nZipper2Length[MAX_CAMERA];
	long m_nZipper2InsulationWidth[MAX_CAMERA];
	bool m_bUseZipper2[MAX_CAMERA];
	float m_fSetZipper2InspectTime[MAX_CAMERA];
	bool m_bUseZipper2InspectionMiddleArea[MAX_CAMERA];
	bool m_bUseZipper2InsulationInspect[MAX_CAMERA];
	bool m_bUseZipper2AutoInsulationWidthPixel[MAX_CAMERA];
	bool m_bUseROIfoundFromInsulationOverlayInspection[MAX_CAMERA];

	//20220916 SJM
	CRect	m_rtInpectionArea[8];
	double  m_dPercent[4];
	int		m_nScrabWidth;
	int		m_nScrabHeight;
	BYTE*   m_ScrabImage;
	BYTE*	m_NoneCoatingImage;
	BYTE*	m_BinaryInsulateImage[MAX_CAMERA];
	int		m_nAve_Insulate;
	BYTE*	m_BinaryIslandImage;
	BYTE*	m_BinaryNgTagImage;
	BYTE*	m_BinarySpurImage[MAX_CAMERA];
	CRect	m_rtBinaryIsland;
	int		m_nAve_Island;
	CRect	m_rtBinaryNgtag;
	int		m_nAve_Ngtag;
	BOOL	m_bIstest;
	int		m_noncoatingOriginBV;
	double  m_noncoatingRatio[2];
	CRect	m_rtNaTagOffset[MAX_CAMERA];
	double  m_dDefectPercent[MAX_CAMERA];
	CRect	m_rtBinaryNonCoating[MAX_CAMERA][2]; // 탭쪽에 중간에 스킵영역으로 인하여 두개로 나눠짐. 
	int		m_nNoncoatingTh[MAX_CAMERA][4];
	int		m_nNoncoatingAve[MAX_CAMERA][3];
	//20220916 SJM
/////추가 파라미터
	BOOL   m_bDefectToScrabWithThreshold[MAX_CAMERA];
//221116
	BOOL	m_bCoatWrinkleUse[MAX_CAMERA];//코팅 주름 검사 유무
	double  m_dCoatWrinkleAreamm[MAX_CAMERA];//에지에서 코팅 주름 검사 영역 //전극 에지에서 일정 영역 단위 mm
	int		m_nCoatWrinkleTh[MAX_CAMERA];//코팅 주름 TH
	double	m_dCoatWrinkleAve[MAX_CAMERA];//코팅 주름 검사 영역 평균밝기
	//221101
	BOOL	m_bInsulAdjust[MAX_CAMERA];//절연 보정 사용 유무
	double m_dScrabSpecSizeX[MAX_CAMERA];//scrab size X
	double m_dScrabSpecSizeY[MAX_CAMERA];//scrab size Y
	//220721
	BOOL	m_bOverlayCorrection[MAX_CAMERA];//오버레이 보정 사용유무
	//220712
	BOOL	m_bTabEdgerangeTh[MAX_CAMERA];//탭부 에지(상/하부 ) 전극 밝기로 이진화
	double	m_dTabEdgeRangemm[MAX_CAMERA];//탭 상하부 전극 밝기 TH 적용 mm
	//220610
	BOOL	m_bSbrIsnadImage[MAX_CAMERA];//TRUE BrightIamge False:Dark Image
	//220530
	int CrackRecheck(BYTE* fmPre, int nCam, int nIdx, int nDefect, int cx, int cy);
	double FindMinSizeX(int nCam);
	double FindMinSizeY(int nCam);
	//220523
	int		m_nNoncoatBlackTh[MAX_CAMERA];//미코팅 영역 다크 TH  임시로 만듦
	BOOL	m_bNoncoatDarkInsp[MAX_CAMERA];//미코팅 영역 다크 검사 //임시로 만듦
	double  m_dDarkNonCoatSizeX[MAX_CAMERA];//미코팅 검사 흑색 불량 Spec Size
	double  m_dDarkNonCoatSizeY[MAX_CAMERA];//미코팅 검사 흑색 불량 Spec Size
	//220517
	BOOL	m_bTabEdgeSelectImage[MAX_CAMERA];//TRUE BrightImage FALSE-DarkImage
	CRect	m_rtFixedVhome[MAX_CAMERA][4];//고정된 4개의 V Home 위치 - 여기서 Y만 사용할지 전부 사용할지?
	BOOL	m_bFixedVhome[MAX_CAMERA];//TRUE 시 사용 FALSE시 미사용
	BOOL	m_bTapePreUse[MAX_CAMERA];//테이프 전처리 사용 유무
	//220516
	int		m_nTimeOutAlarmcount[MAX_CAMERA];//타임아웃이 n번 이상 발생시 알람코드 리턴값을 -9999로 보낸다.
	//int		m_nLastTimeOutCount[MAX_CAMERA];//Time Out Count 기록
	bool	m_bIsCurCellTimeOut[MAX_CAMERA];
	bool	m_bUseTimeOut;
	int		m_nTabEdgeRightSkipPixel[MAX_CAMERA];//전극 에지에서 스킵 픽셀 만큰 건너뛴후 탭 밝기 찾기
	int		m_nNonCoatingEdgeMargin[MAX_CAMERA];//m_bNoncoatEdgeCheck = TRUE고 에지에서 설정 핀셀이내에 붙을 경우 미코팅으로 아닐경우 핀홀 라인
	int		m_nIntekTabEdgeRightSkipPixel[MAX_CAMERA];//jjs
	int		m_nIntekTabSearchPixel[MAX_CAMERA];
	BOOL	m_bNoncoatEdgeCheck[MAX_CAMERA];//esmi 에지에 붙은것만 미코팅으로
	//220513
	int		m_nTabEdgeCriteria[MAX_CAMERA];// 에지 상하부 기준 카메라별로 다를수가 있음. 0 이미지 윗쪽 1 이미지 아래쪽 기준으로 탭위치보다 작을경우 다시 찾는다.
	double m_dTabWidth[MAX_CAMERA];//탭폭에 대한 인자// 탭폭 체크용으로 추가
	double m_dTabHeight[MAX_CAMERA];//탭 높이에 대한 인자// 탭 높이 체크용으로 추가
	double m_dTabEdgeInspRange[MAX_CAMERA];//탭 우측 에지 검사영역 상하 높이
	double m_dTabRightOffset[MAX_CAMERA];//탭 우측 끝 옵셋 전극기준 몇mm 부터 좌측으로 에지를 찾을지 정하는 파라미터
	int m_dTabRightDetectValue[MAX_CAMERA];//탭 우측 끝 부터 왼쪽으로 엣지를 찾는데, 해당Value 이상 되는값을 찾음
	//220503
	//ESWA Spur 영역 사이즈 추가 분리
	BOOL	m_bSpurSizeUse[MAX_CAMERA];//스퍼영역에 독립 사이즈 사용할지 여부 //파라미터 추가 필요.
	double m_dSpurSpecSizeX[MAX_CAMERA];//스퍼 불량 사이즈 파라미터 추가필요.
	double m_dSpurSpecSizeY[MAX_CAMERA];//스퍼 불량 사이즈 파라미터 추가필요.
	double m_dIslandRatio[MAX_CAMERA];//아일랜드 주름 종횡비 -> 기존 라인 종횡비를 따라 가나 신규 수정 
	//220426//
	int m_nTabWhiteCount[MAX_CAMERA];//탭 에지(Top/Bottom) 부분 백색 히스토그램 갯수 Default : 50 // 롤에 이물이 많은 경우 100으로 상향 필요.
	int m_nTabSearchPixel[MAX_CAMERA];//
	//220412
	int		m_nEdgeSearchDirection[MAX_CAMERA];//0 롤 -> 전극 1 전극 -> 롤
	void CInspect::FindBlobEdge(int nCam, BYTE* fm, int nLeftRight,int nth);
	BOOL	m_bZipperInspect[MAX_CAMERA];//ESMI 탭주름 검사
	int		m_nTabZipperTh[MAX_CAMERA];//탭주름 TH
	double	m_dTabZipperLeftOffset[MAX_CAMERA];//어깨선 에지부터 Offset mm
	double	m_dTabZipperRightOffset[MAX_CAMERA];//어깨선 에지부터 Offset mm
	double	m_dTabZipperTBOffset[MAX_CAMERA];//탭 상하 옵셋 mm
	double	m_dZipperSpecSizeX[MAX_CAMERA];//불량 판단 사이즈
	double	m_dZipperSpecSizeY[MAX_CAMERA];//불량 판단 사이즈
	double	m_dZipperYMinSize[MAX_CAMERA];//영역내 특정 Y사이즈를 갖는경우 불량 후보개수
	int		m_nZipperSpecCount[MAX_CAMERA];//스펙보다 작은경우 갯수
	//220411
	static UINT threadScrab1(LPVOID pParam);//scrab용 thread
	static UINT threadScrab2(LPVOID pParam);//scrab용 thread
	CEvent	m_evScrab[MAX_CAMERA];//scrab용 이벤트
	CEvent	m_evGetInsulateAvg[MAX_CAMERA];//221027 임시 속도 최적화 테스트중
	CEvent	m_evNoncoatingAndSpur[MAX_CAMERA];//221027 임시 속도 최적화 테스트중
	CEvent	m_evIslandNgTag[MAX_CAMERA];//221027 임시 속도 최적화 테스트중
	void CInspect::fnScrabMain(int nCam);
	int	m_nScrabRangDefect[MAX_CAMERA];
	BOOL	m_bScrabCheck[MAX_CAMERA];
	CChain * m_pChainScrab[MAX_CAMERA];
	CChain * m_pChainAtPreprocess[MAX_CAMERA];
	CRect	m_rtScrabDefect[MAX_CAMERA][MAX_HUGEDEFECT];
	double	m_dScrabDefectSizeX[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black
	double	m_dScrabDefectSizeY[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black
	double	m_dScrabDefectSize[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black
	BYTE *	m_pScrabDefect[MAX_CAMERA][MAX_HUGEDEFECT];

	//220404
	BOOL	m_bEdgeNoiseSkip[MAX_CAMERA];//esmi 에지 노이즈 (이물)
	//220401 
	double	m_dInsulDarkLeft[MAX_CAMERA];//N2.2 Dark Line 관련 전극에서 Left Offset // Parameter mm
	double	m_dFullLength;//전극 전장 //에지 얼라인 검증용 - 현재 미구현
	int		m_nLRedge[MAX_CAMERA][2][3000];//0 left 1 right  //기울어진 Edge Data
	BOOL	m_bTiltEdge[MAX_CAMERA];//ESMI 에지 기울어짐 검사 여부 - Parameter
	//220328 n2.2 절연 다크불량 관련 추가
	BOOL	m_bInsulateDarkInsp[MAX_CAMERA];//절연 다크검사 사용유무  //parameter추가
	double	m_dInsulDarkRight[MAX_CAMERA];//전극에서 일정 영역만 다크검사를 한다. 단위 mm//parameter추가
	int		m_nInsulDarkTh[MAX_CAMERA];//절연 다크 검사 TH 절연 평균밝기 - TH 로이진화//parameter추가
	double	m_dInsulDarkLineX[MAX_CAMERA];//절연 흑 line X Size //parameter추가
	double	m_dInsulDarkLineY[MAX_CAMERA];//절연 흑 line Y Size //parameter추가

	double	m_dInsulDefectSizeX[MAX_CAMERA];// 절연의경우 darkinsp 사용으로 되어있으면 darkline spec을 사용하고, 사용안함으로 하면 insulatepinhole,insulateline 사용
	double	m_dInsulDefectSizeY[MAX_CAMERA];// 하지만 WA는 insuldefect 다음과 같은 스펙으로 통일한다. SJM 23.03.28
	double	m_dInsulCompact[MAX_CAMERA];//절연 진원도
	
	BOOL	m_bInsulDarkLineSizeAndOr;//[MAX_CAMERA];//Size And Or TRUE And 조건 FLASE OR조건//parameter추가
	double	m_dInsulDarkPinholeX[MAX_CAMERA];//절연 흑 핀홀 X Size //여분으로 만들어둠
	double	m_dInsulDarkPinholeY[MAX_CAMERA];//절연 흑 핀홀 X Size //여분으로 만들어둠

	BOOL	m_bInsulWhiteLineSizeAndOr;//[MAX_CAMERA];//Size And Or TRUE And 조건 FLASE OR조건//parameter추가
	BOOL	m_bInsulWhitePinholeSizeAndOr;//[MAX_CAMERA];//Size And Or TRUE And 조건 FLASE OR조건//parameter추가

	//230706
	BOOL	m_bWhiteConnectionTapeSizeAndOr; 
	BOOL	m_bBlackConnectionTapeSizeAndOr;

//220325
	BOOL	m_bDefect2Scrab[MAX_CAMERA];//표면검사를 스크랩으로 사용유무
	double m_dLeftNoncoatRange[MAX_CAMERA];//Bottom(Trim)부 미코팅 에지기준 우측 검사영역 mm  
	double m_dRightNoncoatRange[MAX_CAMERA];//Tab부 미코팅, 에지기준 좌측 검사영역 mm
	//220321
	BOOL	m_bFindWhiteTape[MAX_CAMERA];//백색 테이프가 검출되었는지 확인 코드
	//220307
	int		m_nTabTopBtmTh[MAX_CAMERA];//탭상하 TH 0 일경우 오토  //파라미터 추가 필요.
	//220303
	BOOL	m_bTapeSkip[MAX_CAMERA];//테이프 검사후 이후 검사 유무 판정
	BOOL	m_bTapeInScrab[MAX_CAMERA];//테이프 검사시 Scrab도 같이 전송할지 유무
	double	m_dDefectOverlabRange[MAX_CAMERA];//Defect 검사시 Overlab 영역 설정 mm
	int		m_nDefectOverlabRange[MAX_CAMERA][2];//Defect 검사시 Overlab 영역 설정 pixel 0X 1Y
	//220225
	//int	m_nEdgeAreaEndPos[MAX_CAMERA];//서치 스타트/엔드의 검색 엔드 픽셀.
	//220216
	int m_nProtrusionTh[MAX_CAMERA];//Protrusion Threshold
	//220211
	int CInspect::FindTabPos(int nCam, BYTE * fmbright, BYTE * fmDark,int nw , int nh, CRect rtins,int * ntop1,int * nbottom1);
	int CInspect::FindTabPosIntek(int nCam, BYTE* fmbright, BYTE* fmDark, int nw, int nh, CRect rtins, int* ntop1, int* nbottom1);

	int		m_nOverlayTh[MAX_CAMERA];//오버레이 TH 파라미터 추가 필요 //220211
	//220210
	BOOL	m_bInsulTapeInsp[MAX_CAMERA];//절연에 Tape 검출 사용 유무 //파라미터 추가 필요 //220211
	//220207
	double	m_dVhomeMarginX[MAX_CAMERA];//V 홈 마진 mm
	double	m_dVhomeMarginY[MAX_CAMERA];//V 홈 마진 mm//
	int CInspect::InspBottomOverlay(int nCam, BYTE * fmDark,int nw,int nh,CRect rtins);
	int		m_nEdgeData1[MAX_CAMERA][4000];//좌측 에지 데이터
	int		m_nEdgeData2[MAX_CAMERA][4000];//우측 에지 데이터
	BOOL		m_bThreadStop[MAX_CAMERA];//Thread return

	////////////거대 불량
	double m_dHugeDefectSizeX[MAX_CAMERA][MAX_HUGEDEFECT][2];//0 white 1 black
	double m_dHugeDefectSizeY[MAX_CAMERA][MAX_HUGEDEFECT][2];//0 white 1 black
	double	m_dBlobCenter1[MAX_CAMERA][MAX_HUGEDEFECT];
	CRect	m_rtFullRange[MAX_CAMERA];
	int		m_nDefaultBright[MAX_CAMERA][3];// 0-왼쪽 1오른쪽 2-둘중 어두운밝기
	int		m_nHugeDefectBright[MAX_CAMERA][MAX_HUGEDEFECT];
	CRect	m_rtHugeRect[MAX_CAMERA];

	//intekplus brucelee
	short m_ntimeoutCount[MAX_CAMERA];


	double		m_dManualOverlay1[100][2];//0 mm 1 pixel
	double		m_dManualInsulate1[100][2];//0 mm 1 pixel
	double		m_dManualCoat1[100][2];//0 mm 1 pixel
	double		m_dManualOverlay2[100][2];//0 mm 1 pixel
	double		m_dManualInsulate2[100][2];//0 mm 1 pixel
	double		m_dManualCoat2[100][2];//0 mm 1 pixel

	int CInspect::ROIOverlay1(int nCam,BYTE * fmEven, BYTE * fmOdd, int nw, int nh, CRect rt,BOOL bImgWrite = FALSE);

	BOOL m_bOverlayError[MAX_CAMERA];//220202
	void CInspect::ResetData(int nCam);//220202

	BOOL	m_bScrabFullRange[MAX_CAMERA];//전체영역 Scrab 할지 말지
	CRect m_rtScrabIns[MAX_CAMERA];//Scrab 검사 영역
//	int CInspect::checkParam();

	int		m_nInsulTh[MAX_CAMERA];//0입력시 가장큰차이 
	int	m_nOffsetUp[MAX_CAMERA];//탭에서 상부노치까지 픽셀
	int m_nOffsetDn[MAX_CAMERA];//탭에서 하부노치까지 픽셀
	BOOL	m_bCemramic[MAX_CAMERA];//Cemramic or SBR
	BOOL	m_bNotch[MAX_CAMERA];// 노치 유무
	int	m_nInsulRightOffset[MAX_CAMERA];//절연 시작부 불량검사 스킵 영역 Pixel
	CRect	m_rtInsulationInspAreaOffset[MAX_CAMERA];
	BOOL	m_bBottomOverayInsp[MAX_CAMERA]; // Long Cell Bottom Overlay Inspect

	//수정 필요
	//220113
	int CInspect::Inspect(IPVM::LoggerInterface& m_loggerDebug, int nCam, BYTE* fmEven, BYTE* fmOdd, int nWidth, int nHeight, int nInsulateType, int nCellMargin, int nOffsetUp, int nOffsetDn, CIS_Site nSite, double dTabWidth = 44, double dTabHeight = 14.5, int nTabPos = 0);
//	int CInspect::Inspect(/*IPVM::LoggerInterface& m_loggerDebug,*/ int nCam, BYTE* fmEven, BYTE* fmOdd, int nWidth, int nHeight, int nInsulateType, int nCellMargin, int nTabPos = 0);//ntabpos = 우측 1 좌측

	bool m_bNoTab;

	HANDLE m_hThreadOverlayEnd[MAX_CAMERA];

	int m_nPreprocessEdgeBreakStep[MAX_CAMERA];
	double m_dLastTime[MAX_CAMERA];

	CString GetPreprocessEdgeBreakStepCode(int nCode);

	double m_dNewinspTime[MAX_CAMERA][100];

	CIS_Site m_enSite;
	//220112
	//Crab 관련
	CRect	m_rtFullDefectScrab[MAX_CAMERA][MAX_FULL_DEFECT];//불량 영역
	int		m_nFullDefectTypeScrab[MAX_CAMERA][MAX_FULL_DEFECT];//defect Code
	double	m_dFullSizeXScrab[MAX_CAMERA][MAX_FULL_DEFECT][3];			//0white 1black
	double	m_dFullSizeYScrab[MAX_CAMERA][MAX_FULL_DEFECT][3];			//0white 1black
	CString	m_strFullDefectNameScrab[MAX_CAMERA][MAX_FULL_DEFECT];
	int		m_nScrabCount[MAX_CAMERA];
	//220111

//220110
	double	m_dMiddleAveDark[MAX_CAMERA];//Dark Image Ave
	CRect	m_rtNotchArea[MAX_CAMERA][4];//notch 후보 영역
	CPoint CInspect::FindNotchPos(int nCam, BYTE * fm,CRect rt,int nCnt);
	int		m_nNotchCount[MAX_CAMERA];//Notch 개수


//211221  Return Feature
	double	m_dDefectArea[MAX_CAMERA][MAX_FULL_DEFECT][4];//0 Area Pixel  1Pixel*Resolution
	int		m_nDefectPosTab[MAX_CAMERA][MAX_FULL_DEFECT];//탭인지 전극인지 탭0 전극1
	int		m_nDefectBrightness[MAX_CAMERA][MAX_FULL_DEFECT];
	double	m_dDefectCenterPos[MAX_CAMERA][MAX_FULL_DEFECT][4];//0 x  1y
	double	m_dDefectRefPos[MAX_CAMERA][MAX_FULL_DEFECT][4];//0 x  1y
	double	m_dDefectDistPos[MAX_CAMERA][MAX_FULL_DEFECT][4];//0 x pixel  1 y pixel  2xmm 3ymm
	double	m_dDefectLongShortW[MAX_CAMERA][MAX_FULL_DEFECT][4];//0 장축pixel 1단축pixel  2 장축mm 3단축mm
	double	m_dDefectLongShortB[MAX_CAMERA][MAX_FULL_DEFECT][4];//0 장축pixel 1단축pixel  2 장축mm 3단축mm
	double	m_dDefectLongShortRatio[MAX_CAMERA][MAX_FULL_DEFECT][4];//0 장축pixel 1단축pixel  2 장축mm 3단축mm
	double	m_dDefectHoleCount[MAX_CAMERA][MAX_FULL_DEFECT][4];//0 White 1Black 
	double	m_dDefectPerimeter[MAX_CAMERA][MAX_FULL_DEFECT][4];//0 White 1Black 
	double	m_dBoundaryPixel[MAX_CAMERA][MAX_FULL_DEFECT][4];//0 White 1Black 
	double  m_dElongation[MAX_CAMERA][MAX_FULL_DEFECT][4];//2차 Moment
	double  m_dSurroingding[MAX_CAMERA][MAX_FULL_DEFECT][4];//0 min 1max 2ave

//개별 Defect에 대한 정보 [2]
	double	m_dDefectAreaSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2];  // (0) pixel, (1) mm

	double	m_dDefectCenterPosSubX[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2]; // (0) pixel, (1) mm
	double	m_dDefectCenterPosSubY[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2]; // (0) pixel, (1) mm

	double	m_dDefectWidthSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2];  // (0) pixel, (1) mm
	double	m_dDefectLengthSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2]; // (0) pixel, (1) mm

	double	m_dDefectPerimeterSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2]; // (0) pixel, (1) mm
	double	m_dElongationSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2];	  // (0) pixel, (1) mm
	CRect	m_rtSubRawDefect[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2];

	//double	m_dDefectRefPosSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][4];// (0) x (1)y
	//double	m_dDefectDistPosSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][4];// (0) x pixel  (1) y pixel  2xmm 3ymm
	//double	m_dDefectHoleCountSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][4];//0 Area Pixel  1Pixel*Resolution
	//double	m_dBoundaryPixelSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][4];//0 Area Pixel  1Pixel*Resolution
	//double	m_dSurroingdingSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][4];//0 Area Pixel  1Pixel*Resolution
	//int		m_nDefectPosTabSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT];//
	//int		m_nDefectBrightnessSub[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT];//

//////////////////////////////////////

	int		m_nCoatEdgeTh[MAX_CAMERA];//어깨선 코팅 에지 TH  //신규 파라미터 필요.
	//211126
	BYTE * m_fmCompressPre[MAX_CAMERA];//211028	//압축이미지 전처리
	int		m_nLargeDefectWThPre[MAX_CAMERA];
	int		m_nLargeDefectBThPre[MAX_CAMERA];


	BOOL	m_bTabPoly[MAX_CAMERA];//다각형 모양 탭

	CRect m_rtFullInspRect[MAX_CAMERA][5];//검사영역 ROI 0 좌측 미코팅 1 우측 미코팅 2전극 영역
	//211106 
	BOOL	m_bDefectRoiOverlap[MAX_CAMERA];//디펙Overlap
	int		m_nNonCoatTh[MAX_CAMERA];//0일 경우 자동으로 아닐경우 값따라가게  //미코팅TH
	//211105
	int		m_nSpurRangeTopBtm[MAX_CAMERA];//Spur 영역 상하 영역  필요없음	//Spur 영역-상하
	int		m_nTabEdgeNoncoatOffset[MAX_CAMERA];//Tab 상하부 미코팅 Skip 영역 //탭부미코팅Skip

//211104
	BOOL	m_bBottomNoncoatingUse[MAX_CAMERA];//오창 바텀부에 기준 눈금자가 전극끝과 맞다아 검사시 과검 발생.
	BOOL	m_bTabNoncoatingUse[MAX_CAMERA];
	int CInspect::FindEdge1(int nCam,BYTE * fmBright,BYTE * fmDark);
	int FindCellEdgeIntek(int nCam, BYTE* fmBright, BYTE* fmDark, int& nLeft, int& nRight);//YYS 작업중
	bool FindBumpy(int nCam, BYTE* fmBright, BYTE* fmDark, CRect& rtDefect);//20221115 YYS
	CRect m_rtBumpyArea[MAX_CAMERA];//20221115 YYS

///211101
	int m_nMarginSearchEnd[2];
	int	m_nNonCoatOffset[MAX_CAMERA];

///211028
	BYTE * m_fmCompressDark[MAX_CAMERA];//211028	
	BOOL m_bWhiteRoll[MAX_CAMERA];//White Roll	//211028
	BOOL m_bWhiteRoll_NoncoatingImageIndex[MAX_CAMERA];//Noncoating image index when use White Roll	//211028
	BOOL m_bInit;
	int		m_nCellSkipBrightness[MAX_CAMERA];//PET 일경우 일정 밝기 이상 되면 검사 Skip  //211017-1
	double	m_dCellTopBottmAve[MAX_CAMERA][3];//0 top 1 mid 2 bottom	//211017-1
	BOOL	m_bStep[MAX_CAMERA];//단차 사용유무 //211025

/////과검 Skip용 추가 Param
	int		m_nLineValue[MAX_CAMERA];//라인용 Value
	int		m_nSpurOffset[MAX_CAMERA];//Spur 용 우측 옵셋
	int		m_nSpurValue[MAX_CAMERA];//Sput TH
	int		m_nScratchValue[MAX_CAMERA];	//스크래치 Value
	float	m_fScratchRatio[MAX_CAMERA];	//스크래치 Value
	float	m_fScratchLength[MAX_CAMERA];	//스크래치 Value
	int		m_nCrackValue[MAX_CAMERA];

	CRect	m_rtIslandBlob[MAX_CAMERA][MAX_HUGEDEFECT];//blob size
	double	m_dIslnadPercent[MAX_CAMERA][MAX_HUGEDEFECT];//면적
	double	m_dIslandPercentTh[MAX_CAMERA];// 아일랜드 면적 컷
	double	m_dIslandCompactTh[MAX_CAMERA];//아일랜드 진원도
	BOOL	m_bResetLeftAreaBlob[MAX_CAMERA];//아일랜드 좌측 ROI를 blob을 이용하여 재설정 유무
	int		m_nIslandOffset[MAX_CAMERA];//아일랜드 좌측 옵셋

	CPoint m_ptTabCenter[MAX_CAMERA];//탭 Blob중심 //211009
 //////////211013 CMI 요청 불량 최소 사이즈
	double m_dPinholeMinSize[MAX_CAMERA];
	double m_dLineMinSize[MAX_CAMERA];
	double m_dScratchMinSize[MAX_CAMERA];
	double m_dCraterMinSize[MAX_CAMERA];
	double m_dDentMinSize[MAX_CAMERA];
	double m_dProtrusionMinSize[MAX_CAMERA];
	double m_dCracknMinSize[MAX_CAMERA];
	double m_dHeatingWrinkleMinSize[MAX_CAMERA];
	double m_dCoatingWrinkleMinSize[MAX_CAMERA];
	double m_dNonCoatMinSize[MAX_CAMERA];
	double m_dIslandMinSize[MAX_CAMERA];
	double m_dITabWrinkleMinSize[MAX_CAMERA];
	double m_dDrossMinSize[MAX_CAMERA];
	double m_dSpatterMinSize[MAX_CAMERA];
	double m_dSpurMinSize[MAX_CAMERA];

	//////////////211009 사이즈 or 조건
	BOOL m_bPinholeAndOr;//핀홀 And 조건 //211006
	BOOL m_bLineSizeAndOr;//라인 사이즈 and or
	BOOL m_bScratchSizeAndOr;//스크래치 사이즈 and or
	BOOL m_bCraterSizeAndOr;//분화구 사이즈 and or
	BOOL m_bDentSizeAndOr;//찍힘 사이즈 and or
	BOOL m_bProtrusionSizeAndOr;//돌출 사이즈 and or
	BOOL m_bCrackSizeAndOr;//크랙 사이즈 and or
	BOOL m_bHeatingWrinkleSizeAndOr;//열주름 사이즈 and or
	BOOL m_bCoatingWrinkleSizeAndOr;//코팅주름 사이즈 and or
	BOOL m_bNonCoatingSizeAndOr;//미코팅 사이즈 and or 탭부 제외
	BOOL m_bIslandSizeAndOr;//아일랜드 사이즈 and or
	BOOL m_bWrinkleSizeAndOr;//탭주름 사이즈 and or
	BOOL m_bDrossSizeAndOr;//드로스 사이즈 and or
	BOOL m_bSpatterSizeAndOr;//스패터 사이즈 and or
	BOOL m_bSpurSizeAndOr;//탭부 파임 사이즈 and or
	BOOL m_bScrabSizeAndOr;//스크랩 사이즈 And Or   0 or 1 and

	//////////////////////////////////Scrab //0930
	int CInspect::FindScrab(int nCam, BYTE * fmBright, BYTE * fmDark, int nw, int nh, int nRight);

	int		m_nScrabRoiTop[MAX_CAMERA];// top 0 +  마진 Y = 마진값 입력
	int		m_nScrabRoiBot[MAX_CAMERA];//Height - 마진Y = 마진값 입력
	int		m_nScrabRoiLeft[MAX_CAMERA];//우측 에지 기준 Offset1 mm
	int		m_nScrabRoiRight[MAX_CAMERA];//우측 에지 기준 offset2 mm
	double	m_dScrabSize[MAX_CAMERA];//영영내 가장큰 X Y 중 큰 값
	int		m_nScrabRightBoundary;

	double	m_dSpecScarabSize[MAX_CAMERA];//불량 스펙 사이즈
	int		m_nScrabTh[MAX_CAMERA];//밝은 영상 평균 밝기 + TH  = 이진화값
	BOOL	m_bInsScrab[MAX_CAMERA];//스크랩 검사 유무
			
	///////////////////////////////////


	//0928
	private:
	IPVM::LoggerInterface* m_Debuglog;
	public:
	int CInspect::FindSpur(int nCam, BYTE* fm, int nw, int  nh, CRect rtspur, int nTh);
	BOOL m_bSpatterInsp[MAX_CAMERA];
	double m_dSpatterSizeX[MAX_CAMERA][2];//0 white 1 black
	double m_dSpatterSizeY[MAX_CAMERA][2];//0white 1black
	double m_dDrossSizeX[MAX_CAMERA][2];//0white 1black
	double m_dDrossSizeY[MAX_CAMERA][2];//0white 1black
	double m_dSpatterRange[MAX_CAMERA][2];//0mm 1pixel
	int		m_nSpatterTh[MAX_CAMERA][2];//0white 1black


	/////////////추가 파라미터 필요
	double	m_dInsulateThGap[MAX_CAMERA];//절연GapTH//평균밝기 X 배율  신규 추가 
	double	m_dTapeWhiteSizeX[MAX_CAMERA];//tape백SizeX
	double	m_dTapeWhiteSizeY[MAX_CAMERA];//tape백SizeX
	double	m_dTapeBlackSizeX[MAX_CAMERA];//tape흑SizeX
	double	m_dTapeBlackSizeY[MAX_CAMERA];//tape흑SizeX


	///09 ////////////////펑션 정리
	int CInspect::FindNonCoating(int nCam, BYTE * fmBright, BYTE * fmDark, int nw, int nh, int nTh, CRect rtinsLeft, CRect rtinsRight, int nEdgeSum1, double * dCoatEdge1, int nt1, int nt2, int * ntabpos1);
	int CInspect::FindBurr(int nCam, BYTE * fmBright, BYTE* fmDark, int nw, int nh, CRect rttab, double dtabAverage);
	int CInspect::findIsland(int nCam, BYTE* fmBright, BYTE* fmDark, int nw, int nh, CRect rtins, int * nTabProj, int nIslandTh);
	int CInspect::InspNgTag(int nCam, BYTE * fmBright, BYTE * fmDark, int nw, int nh, CRect rttabins1);
	int CInspect::InspInsulate(int nCam, BYTE * fm, BYTE * fmDark, int nw, int nh, int nRightSum, int nRightSum1, IPVM::Rect rtInspArea, IPVM::Rect rtOffset,int nInsulDarkLeft=0, int nInsulDarkRight = 0, BOOL InspGap=FALSE);
	int CInspect::InspInsulateWidth(int nCam, BYTE* fm, BYTE* fmDark, int nw, int nh, CRect rttabins, int nCoatEdge1, int nEndPos, int nOverlay, int nt1, int nt2, int *nRightS, int *nRightS1);

	//
	int CInspect::FindEdgebetweenElectrodeAndTab(int nCam, BYTE* fmBright, int nw, int nh, CRect rtins);


	///////////////////////////manual Inspect  //0804
	int CInspect::ROIOverlay(int nCam,BYTE * fmEven, BYTE * fmOdd, int nw, int nh, CRect rt);
	int	CInspect::ManualIsland(int nCam,BYTE * fmEven, BYTE * fmOdd, int nw, int nh, CRect rt);
	int	ROIInspect(int nCam,BYTE * fm, int nw, int nh, CRect rt);
	int	CInspect::ManualDualBlob(int nCam,BYTE * fmOrigin, BYTE* fmPre, int * nPreproj, int nDefect, int nBW, int nEdge);//nbw 3 무지 4찍힘 후보
	void CInspect::InitManual();
	CRect		m_rtManualDefect[MAX_MANUAL_DEFECT];
	int			m_nManualDefectType[MAX_MANUAL_DEFECT];
	CString		m_strManualDefectName[MAX_MANUAL_DEFECT];
	double		m_dManualStdValue[MAX_MANUAL_DEFECT][2];
	double		m_dManualCompactness[MAX_MANUAL_DEFECT][2];
	double		m_dManualDistance[MAX_MANUAL_DEFECT][2];
	double		m_dManualPercent[MAX_MANUAL_DEFECT][2];
	double		m_dManualSizeX[MAX_MANUAL_DEFECT][3];
	double		m_dManualAngle[MAX_MANUAL_DEFECT][3];
	double		m_dManualSizeY[MAX_MANUAL_DEFECT][3];
	double		m_dManualSize[MAX_MANUAL_DEFECT][3];
	double		m_dManualPeakDataW[MAX_MANUAL_DEFECT][2];
	double		m_dManualPeakDataB[MAX_MANUAL_DEFECT][2];
	double		m_dManualPeakData[MAX_MANUAL_DEFECT][2][5];
	BYTE * m_fmManualDefect[MAX_MANUAL_DEFECT];
	int			m_nManualDefectBright[MAX_MANUAL_DEFECT];
	CPoint		m_ptManualDefectPoint[MAX_MANUAL_DEFECT];
	CRect		m_rtManualBlob[MAX_MANUAL_DEFECT][2];
	int			m_nManualDefectCount;
	CChain * m_pManualChain;

	double		m_dManualOverlay[2];//0 mm 1 pixel
	double		m_dManualInsulate[2];//0 mm 1 pixel
	double		m_dManualCoat[2];//0 mm 1 pixel
	int			m_nnManualOverlayTh;
	int			m_nManualCoatTh;
	int			m_nManualInsulTh;
	double		m_dManualratio1;
	//////////////////////////////


	///////////////////210809
	int		m_nAnodeCathode; //양극 0 음극 1  //0805
	BOOL	m_bBurrInspect; //Burr 검사 //0803
	BOOL	m_bOverlayCheck[MAX_CAMERA];
	void CInspect::PreprocessLongCell(int nCam);//0810

	/////////////////////테이프 //0726
	//int		m_nReduction;//압축 배율
	double	m_dCunicSizeX1;//찍힘 후보 //0729-1 //0809
	double	m_dCunicSizeY1;//찍힘 후보 //0729-1	//0809
	int		m_nLabelTagTh[MAX_CAMERA];//Dark Image에서 라벨 밝기 //0720
	double	m_dTabBrightness[MAX_CAMERA][2];//0bright 1dark //0720

	double	m_dCrackAngle[MAX_CAMERA];//0814

	double	m_dNoneCoatingSizeX[MAX_CAMERA];
	double	m_dNoneCoatingSizeY[MAX_CAMERA];
	int		m_nNoneCoatingCount[MAX_CAMERA];
	double m_dTabLabelSkipTB[MAX_CAMERA];
	double m_dTabLabelSkipLR[MAX_CAMERA];

	double		m_dNonCoatSizeX[MAX_CAMERA];//0825
	double		m_dNonCoatSizeY[MAX_CAMERA];//0825

	int m_nMarginSearchStart[2];

	BOOL	m_bNgTag;//ng tag  1 = Tape로 코드전송 0 island로 코드 전송 //0625
	BOOL	m_bPrimer;//Primer Model LV계열. //0630 LV 모델의 경우 프라이머 영역을 미검사 한다.

	//std::vector<InspParam> m_vInspParam;//파라미터 Vector
	std::vector<SpecParam> m_vSpecParam;//파라미터 Vector //size 조건에 의한 파람

	void CInspect::CloseInspect();
	int CInspect::CheckColor(int nType);//0604-2

	BOOL	m_bStopInsp;
	double	m_dTabTapeX[MAX_CAMERA];//tab내 테이프센서 X size
	double	m_dTabTapeY[MAX_CAMERA];//tab내 테이프센서 Y size
	BOOL	m_bOverlayInsp;//오버레이 검사 유무 //0805
	BOOL	m_bInsulateInsp;//절연 검사 유무 //0805
	BOOL	m_BlackTapeInsp;//다크 테이프 검사 유무 //0613


	static UINT threadInspect1(LPVOID pParam);
	static UINT threadInspect2(LPVOID pParam);
	static UINT threadInspect3(LPVOID pParam);
	static UINT threadInspect4(LPVOID pParam);
	static UINT threadInspect5(LPVOID pParam);
	static UINT threadInspect6(LPVOID pParam);
	static UINT threadInspect7(LPVOID pParam);
	static UINT threadInspect8(LPVOID pParam);
	static UINT threadInspect11(LPVOID pParam);
	static UINT threadInspect12(LPVOID pParam);
	static UINT threadInspect13(LPVOID pParam);
	static UINT threadInspect14(LPVOID pParam);
	static UINT threadInspect15(LPVOID pParam);
	static UINT threadInspect16(LPVOID pParam);
	static UINT threadInspect17(LPVOID pParam);//a5c e51
	static UINT threadInspect18(LPVOID pParam);
	static UINT threadOverlay1(LPVOID pParam);
	static UINT threadOverlay11(LPVOID pParam);

	static UINT threadGetInsulateAvgUp(LPVOID pParam);
	static UINT threadGetInsulateAvgDown(LPVOID pParam);

	static UINT threadNoncoatingAndSpurUp(LPVOID pParam);
	static UINT threadNoncoatingAndSpurDown(LPVOID pParam);

	static UINT threadIslandNgTagUp(LPVOID pParam);
	static UINT threadIslandNgTagDown(LPVOID pParam);

	bool m_bGetInsulateAvgThreadEnd[MAX_CAMERA];
	bool m_bNoncoatingAndSpurThreadEnd[MAX_CAMERA];
	bool m_bIslandAndNgTag[MAX_CAMERA];

	std::vector <int> m_vecNoncoatingDefect[MAX_CAMERA];
	CCriticalSection m_csNoncoatingDefect[MAX_CAMERA];
	bool m_bFindNoncoatingFinish[MAX_CAMERA];

	CCriticalSection m_csGetInsulateAvg[MAX_CAMERA];
	CCriticalSection m_csNoncoatingAndSpur[MAX_CAMERA];
	CCriticalSection m_csIslandAndNgTag[MAX_CAMERA];

	double m_dInsulWidth[MAX_CAMERA];//0604 //절연 폭
	double m_dOverlayWidth[MAX_CAMERA];//0604 //오버레이폭
	double m_dOldInsulWidth[MAX_CAMERA];//0604
	double m_dOldOverlayWidth[MAX_CAMERA];//0604
	double	m_dInsSubTime2[MAX_CAMERA][MOD_NUMBER][10];//0706


	double	m_dTabSkipRange[MAX_CAMERA][3];//0614 0Top 1bottom 2right

	int		m_nCellSize;//0 Normal 1longcell //0801
	double  m_dBottomInsul[MAX_CAMERA][2];//0mm 1pixel	//0801
	double  m_dBottomCoat[MAX_CAMERA][2];//0mm 1pixel	//0801
	double  m_dBottomOverlay[MAX_CAMERA][2];//0mm 1pixel//0801



	CEvent	m_evInsp[MAX_CAMERA][MOD_NUMBER];
	CEvent	m_evOverlay[MAX_CAMERA];
	//CEvent	m_evGetInsulateAvg[MAX_CAMERA];
	//CEvent	m_evNoncoatingAndSpur[MAX_CAMERA];
	//CEvent	m_evIslandNgTag[MAX_CAMERA];
	int	CInspect::DualBlob(BYTE * fmOrigin,BYTE* fmPre,int nCam, int nIdx,int nDefect,int nBW,int nEdge=0);
	int	CInspect::DualLineBlob(BYTE * fmOrigin,int nCam, int nIdx,int nDefect,int nBW);//nbw 3 무지
	void CInspect::InitInspect(int nWidth, int nHeight,int nAnodeCathode=0, bool bUseNewLogic = false);
	int CInspect::InspectMod(int nCam,int nIdx,int nMod);
	void CInspect::ImageSelectCopy(unsigned char* fmSrc,unsigned char* fmDest,int nSrcW,int nSrcH,CRect rtArea,int nDestW,int nDestH,int nDestSttX,int nDestSttY);
	void CInspect::ImageSelectCopy1(unsigned char* fmSrc,unsigned char* fmDest,int nSrcW,int nSrcH,CRect rtArea,int nDestW,int nDestH,int nDestSttX,int nDestSttY,int nReduce,BOOL bColor);
	CRect CInspect::SetBoundary(CRect rt,int nwidth,int nh1,int nh2,int *nx1,int *ny1,int* nx2,int * ny2);
	CRect CInspect::SetBoundary1(CRect rt,int nwidth,int nh1,int nh2);
	CRect CInspect::SetBoundary2(CRect rt,CRect rt2);
	int CInspect::AreaAve1(BYTE * fm,CRect rt,int nw, int * nave);
	int CInspect::AreaAve(BYTE * fm,CRect rt,int nw);
	int CInspect::FindHugeDefect(int nCam,BYTE * fmsrc,int nTabPos);
	void CInspect::InitParam(int nAnodeCathode=0);
	double CInspect::GetPrecisionTime(void);
	CString CInspect::GetDefectName(int nCode);
	void CInspect::PreprocessEdge(int nCam);
	int CInspect::PreprocessingCeramic(BYTE * fmsrc, BYTE * fmdst,CRect rt,int nPitch,int nHeight);

	void PreprocessEdgeNewLogic(int nCam);

	bool m_bUseNewLogic;

	std::vector <int> m_vecRightSum[MAX_CAMERA];
	CCriticalSection m_csRightSum[MAX_CAMERA];
	void GetInsulateAvg(int nCam);
	std::vector <CRect> m_vecRectEdge[MAX_CAMERA];
	CCriticalSection m_csRectEdge[MAX_CAMERA];
	std::vector <int> m_vecNoncoatEdge[MAX_CAMERA];
	CCriticalSection m_csNoncoatEdge[MAX_CAMERA];
	std::vector <double> m_vecCoatEdge[MAX_CAMERA];
	CCriticalSection m_csCoatEdge[MAX_CAMERA];
	std::vector <int> m_vec_nt[MAX_CAMERA];
	CCriticalSection m_cs_nt[MAX_CAMERA];
	std::vector <int> m_vecTabPos[MAX_CAMERA];
	CCriticalSection m_csTabPos[MAX_CAMERA];
	std::vector <int> m_vecSumEdge[MAX_CAMERA];
	CCriticalSection m_csSumEdge[MAX_CAMERA];
	std::vector <int> m_vecOffset1[MAX_CAMERA];
	CCriticalSection m_csOffset1[MAX_CAMERA];
	std::vector <CRect> m_vecRectTabIns[MAX_CAMERA];
	CCriticalSection m_csRectTabIns[MAX_CAMERA];
	std::vector <CRect> m_vecRectSpur[MAX_CAMERA];
	CCriticalSection m_csRectSpur[MAX_CAMERA];
	BYTE* m_fmPreprocessTemp;
	void FindNoncoatingAndSpur(int nCam);
	void FindIsladAndNgTag(int nCam);
	int FindDefectAtPreprocess(int nCam, BYTE* fmPreprocess, int nw, int nh, int* pPosInfo);


	/*double GetSurfaceSpec(CString strSpecName);
	BOOL SetSurfaceSpec(CString strSpecName, double dValue);
	BOOL SetInspectEnable(CString strSpecName, BOOL bEnable);*/
	//void GetSurfaceSpec(CInspect *stInspParam);
	bool m_bCommInit;

	BOOL	m_bInsp[MAX_CAMERA];
	double	m_dCoatAve[MAX_CAMERA];
	CRect	m_rtInsulate[MAX_CAMERA];
	CRect	m_rtCuttingHeightTop[MAX_CAMERA];
	CRect	m_rtCuttingHeightBottom[MAX_CAMERA];
	CRect	m_rtInsulationInspArea[MAX_CAMERA];
	CRect	m_rtInsulationDarkInspArea[MAX_CAMERA];
	CRect	m_rtCuttingHeightTopInspArea[MAX_CAMERA];
	CRect	m_rtCuttingHeightBottomInspArea[MAX_CAMERA];
	BOOL	m_bScrabReject[MAX_CAMERA];
	CRect	m_rtCell[MAX_CAMERA];
	CRect	m_rtCellTab[MAX_CAMERA];
	CPoint	m_ptNotch[MAX_CAMERA][4];//좌상단 에지
	CPoint	m_ptFullNotch[MAX_CAMERA][4];
	CRect m_rtOverlay[MAX_CAMERA];
	int		m_nMachine;//0 양극 1  음극
	BOOL	m_bDefectBinary;
	BOOL	m_bThread;
	BYTE * m_fmEven[MAX_CAMERA];
	BYTE * m_fmOdd[MAX_CAMERA];
	int		m_nWidthMini;
	int		m_nHeightMini;
	int		m_nWidth;
	int		m_nHeight;
	int		m_nInsulateType;
	int		m_nCellMargin;
	BYTE * m_fmPreProcess[MAX_CAMERA];//Y Projection Image
	BYTE * m_fmPreProcess1[MAX_CAMERA];//X Projection Image
	BYTE * m_fmTabEdge[MAX_CAMERA];
	BYTE * m_fmTabEdge1[MAX_CAMERA];
	BYTE * m_pSubDefet[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][3];
	BYTE * m_fmCompress[MAX_CAMERA];
	BYTE * m_fmHugeDefect[MAX_CAMERA][MAX_HUGEDEFECT];//압축영상의 불량 이미지
	CChain *m_pChain[MAX_CAMERA][MOD_NUMBER][2];//0 백 1 흑 이진화
	CChain *m_pChainHuge[MAX_CAMERA][2];//0 백 1 흑 이진화
	CRect	m_rtMinTab[MAX_CAMERA];//축소영상의 Tab Y만 사용
	CChain *m_pChainInsul[MAX_CAMERA];

	CChain* m_pFindElectrodeEndPos[MAX_CAMERA];

///////////이미지 관련 추가
	BYTE * m_fmcompressWhite[MAX_CAMERA];
	BYTE * m_fmcompressBlack[MAX_CAMERA];
	BYTE * m_fmOverlayPreprocess[MAX_CAMERA];
	BYTE * m_fmInsulBlob[MAX_CAMERA];

	BOOL	m_bTestFlag[MAX_CAMERA][MOD_NUMBER];
	BOOL	m_bOverlayIns[MAX_CAMERA];

	CCriticalSection m_cs_OverlayThreadEnd[MAX_CAMERA];
	CCriticalSection m_cs_ScrabThreadEnd[MAX_CAMERA];
	
	bool m_bOverlayThreadEnd[MAX_CAMERA];
	bool m_bScrabThreadEnd[MAX_CAMERA];

	int		m_nPreProcessData[MAX_CAMERA][MOD_NUMBER][MAX_IMAGE_WIDTH];//전처리시 Y Value;
	int		m_nPreProcessData1[MAX_CAMERA][MOD_NUMBER][MAX_IMAGE_WIDTH];//전처리시 X Vaue;
	int		m_nSubProjData[MAX_CAMERA][MOD_NUMBER][MAX_IMAGE_WIDTH]; //이미지의 X 좌표별 Y sum 해서 구한 평균 밝기
	int		m_nSubDefectCount[MAX_CAMERA][MOD_NUMBER];
	CRect	m_rtSubInspect[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT];
	CRect	m_rtSubDefect[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT];
	//island
	CRect	m_rtIsland[MAX_CAMERA][MAX_HUGEDEFECT];
	int		m_nIslandCount[MAX_CAMERA];
	BYTE *	m_pIsland[MAX_CAMERA][MAX_HUGEDEFECT];
	CChain * m_pChainIsland[MAX_CAMERA];
	BYTE *	m_pInsulDefect[MAX_CAMERA][MAX_HUGEDEFECT];
	int		m_nInsulateDefectCount[MAX_CAMERA];
	CRect	m_rtInsulateDefect[MAX_CAMERA][MAX_HUGEDEFECT];

	CChain * m_pChainNoncoating[MAX_CAMERA];

	double	m_dISSizeX[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black
	double	m_dISSizeY[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black
	double	m_dISSize[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black
	double	m_dInsulSizeX[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black
	double	m_dInsulSizeY[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black
	double	m_dInsulSize[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black
	double	m_dISAngle[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black	//0803
	double	m_dISCompactness[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black //0803

	double	m_dInsulCompaceness[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black //0803
	double	m_dInsulMinMax[MAX_CAMERA][MAX_HUGEDEFECT][2];			//0white 1black //0803
//211103  중심에서 최소거리 최대거리
	double	m_dISMaxDist[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black //0803
	double	m_dISMinDist[MAX_CAMERA][MAX_HUGEDEFECT];			//0white 1black //0803

	/////////////////////////////param
	double	m_dscale_x1[MAX_CAMERA];
	double	m_dscale_y1[MAX_CAMERA];
	int		m_nTargetBrightness[MAX_CAMERA];//전처리 밝기
	double	m_dMagValue[MAX_CAMERA];//증폭 배율
	int		m_nLineMag[MAX_CAMERA];//라인 증폭
	int		m_nHighCut[MAX_CAMERA];
	int		m_nLowCut[MAX_CAMERA];
	int		m_nLineHighCut[MAX_CAMERA];
	int		m_nEdgeTh[MAX_CAMERA];//에지 TH
	BOOL	m_bAutoEdgeTh[MAX_CAMERA];//자동 에지 검출
	int		m_nTh1[MAX_CAMERA];
	int		m_nTh2[MAX_CAMERA];
	double	m_dLineCut[MAX_CAMERA];//라인 후보
	int		m_nLineWth[MAX_CAMERA];//라인 백th
	int		m_nLineBth[MAX_CAMERA];//라인 흑th
	int		m_nEdgeOffset1[MAX_CAMERA];
	int		m_nIslandTh[MAX_CAMERA];
	int		m_nMargin[MAX_CAMERA][2];// TH Margin //0 white 1 balck
	int		m_nPinholeCut[MAX_CAMERA];

	BOOL m_bCuttingHeightTopAreaInspUse[MAX_CAMERA];  //cuttingheight top
	IPVM::Rect m_rtCuttingHeightTopAreaOffset[MAX_CAMERA];

	BOOL m_bCuttingHeightBottomAreaInspUse[MAX_CAMERA]; //cuttingheight bottom
	IPVM::Rect m_rtCuttingHeightBottomAreaOffset[MAX_CAMERA];

	BOOL	m_bMiniPinhole[MAX_CAMERA];		 
	BOOL	m_bMiniPinhole_ifAND[MAX_CAMERA];
	double	m_dMiniPinhole_SizeX[MAX_CAMERA];
	double	m_dMiniPinhole_SizeY[MAX_CAMERA];
	int		m_nMiniPinhole_Count[MAX_CAMERA];

	BOOL	m_bMiniProtrution[MAX_CAMERA];
	BOOL	m_bMiniProtrution_ifAND[MAX_CAMERA];
	double	m_dMiniProtrution_SizeX[MAX_CAMERA];
	double	m_dMiniProtrution_SizeY[MAX_CAMERA];
	int		m_nMiniProtrution_Count[MAX_CAMERA];

	BOOL	m_bMiniDent[MAX_CAMERA]; //미니찍힘
	BOOL	m_bMiniDent_ifAND[MAX_CAMERA];
	double	m_dMiniDent_SizeX[MAX_CAMERA];
	double	m_dMiniDent_SizeY[MAX_CAMERA];
	int		m_nMiniDent_Count[MAX_CAMERA];

	int		m_nBlackCut1[MAX_CAMERA];
	double	m_dBlackPercent[MAX_CAMERA];
	float	m_fCraterWidth[MAX_CAMERA];
	float	m_fCraterMinimumGrayLevel[MAX_CAMERA];
	BOOL	m_bMiniCrater[MAX_CAMERA];		
	BOOL	m_bMiniCrater_ifAND[MAX_CAMERA];
	double	m_dMiniCrater_SizeX[MAX_CAMERA];
	double	m_dMiniCrater_SizeY[MAX_CAMERA];
	int		m_nMiniCrater_Count[MAX_CAMERA];

	BOOL	m_bSizeAndOr[MAX_CAMERA];
	double  m_dXYRatio[MAX_CAMERA];
	int		m_nPinHoleTh[MAX_CAMERA];
	double  m_dWhiteSizeX[MAX_CAMERA];//반짝이 돌출
	double  m_dWhiteSizeY[MAX_CAMERA];
	double  m_dDarkSizeX[MAX_CAMERA];//분화구
	double  m_dDarkSizeY[MAX_CAMERA];
	double	m_dWhiteLineSizeX[MAX_CAMERA];//라인
	double	m_dWhiteLineSizeY[MAX_CAMERA];
	double	m_dPinholeSizeX[MAX_CAMERA];//핀홀
	double	m_dPinholeSizeY[MAX_CAMERA];
	double	m_dBlackLineSizeX[MAX_CAMERA];//긁힘
	double	m_dBlackLineSizeY[MAX_CAMERA];
	double	m_dScratchTinyWidth[MAX_CAMERA];
	double	m_dIslandSizeX[MAX_CAMERA];//아일랜드
	double	m_dIslandSizeY[MAX_CAMERA];
	double	m_dCunicSizeX[MAX_CAMERA];//찍힘
	double	m_dCunicSizeY[MAX_CAMERA];
	BOOL	m_bInspManual;

	double	m_dPinholeCraterBSizeX[MAX_CAMERA];//핀홀성 분화구
	double	m_dPinholeCraterBSizeY[MAX_CAMERA];
	int		m_nReduction[MAX_CAMERA];
	int		m_nLargeDefectWTh[MAX_CAMERA];
	int		m_nLargeDefectBTh[MAX_CAMERA];
	double	m_dCrackSizeX[MAX_CAMERA];//크랙
	double	m_dCrackSizeY[MAX_CAMERA];
	double  m_dOverlaySize[MAX_CAMERA]; // 오버레이폭
	double  m_dInsulationSize[MAX_CAMERA]; //절연폭
	double  m_dOverlayMinusTolerance[MAX_CAMERA]; // 오버레이폭
	double  m_dOverlayPlusTolerance[MAX_CAMERA]; // 오버레이폭
	double  m_dInsulationMinusTolerance[MAX_CAMERA]; //절연폭
	double  m_dInsulationPlusTolerance[MAX_CAMERA]; //절연폭
	double  m_dBotOverlaySize[MAX_CAMERA]; // 오버레이폭
	double  m_dBotInsulationSize[MAX_CAMERA]; //절연폭
	double  m_dBotOverlayMinusTolerance[MAX_CAMERA]; // 오버레이폭
	double  m_dBotOverlayPlusTolerance[MAX_CAMERA]; // 오버레이폭
	double  m_dBotInsulationMinusTolerance[MAX_CAMERA]; //절연폭
	double  m_dBotInsulationPlusTolerance[MAX_CAMERA]; //절연폭
	double	m_dWrinkleBlackSizeX[MAX_CAMERA];//열주름
	double	m_dWrinkleBlackSizeY[MAX_CAMERA];//
	double	m_dLargeLineSizeX[MAX_CAMERA];//거대 라인
	double	m_dLargeLineSizeY[MAX_CAMERA];//거대 라인
	int		m_nLineInBlackCount[MAX_CAMERA];//열주름 내부 흑 Count
	double	m_dIslandWrikleSizeX[MAX_CAMERA];//무지부 주름
	double	m_dIslandWrikleSizeY[MAX_CAMERA];
	double	m_dCoatWrinkleSizeX[MAX_CAMERA];//코팅주름
	double	m_dCoatWrinkleSizeY[MAX_CAMERA];//코팅주름

	/////////////////////절연
	double	m_dInsulatePinholeX[MAX_CAMERA];//절연 핀홀 0.75
	double	m_dInsulatePinholeY[MAX_CAMERA];//절연 핀홀 0.75
	double	m_dInsulateLineX[MAX_CAMERA];//절연 라인 
	double	m_dInsulateLineY[MAX_CAMERA];//절연 라인
	double	m_dInsulateGapLineX[MAX_CAMERA];//절연 핀홀 0.2
	double	m_dInsulateGapLineY[MAX_CAMERA];//절연 핀홀 10

	double	m_dOverlayOffset[MAX_CAMERA];
	double	m_dInsulationOffset[MAX_CAMERA];
	double	m_dBotOverlayOffset[MAX_CAMERA];
	double	m_dBotInsulationOffset[MAX_CAMERA];

	////////////////////경고 및 마킹 스펙
	double	m_dAiOverlayMin[2]; //오버레이 폭 경고 스펙
	double	m_dAiOverlayMax[2]; //오버레이 폭 경고 스펙
	double	m_dAiInsulationMin[2]; // 절연 폭 경고 스펙
	double	m_dAiInsulationMax[2]; // 절연 폭 경고 스펙
	double	m_dMarkingOverlayMin[2]; //오버레이 폭 마킹 스펙
	double	m_dMarkingOverlayMax[2]; //오버레이 폭 마킹 스펙
	double	m_dMarkingInsulationMin[2]; // 절연 폭 마킹 스펙
	double	m_dMarkingInsulationMax[2]; // 절연 폭 마킹 스펙

	double	m_dAiBotOverlayMin[2]; //오버레이 폭 경고 스펙
	double	m_dAiBotOverlayMax[2]; //오버레이 폭 경고 스펙
	double	m_dAiBotInsulationMin[2]; // 절연 폭 경고 스펙
	double	m_dAiBotInsulationMax[2]; // 절연 폭 경고 스펙
	double	m_dMarkingBotOverlayMin[2]; //오버레이 폭 마킹 스펙
	double	m_dMarkingBotOverlayMax[2]; //오버레이 폭 마킹 스펙
	double	m_dMarkingBotInsulationMin[2]; // 절연 폭 마킹 스펙
	double	m_dMarkingBotInsulationMax[2]; // 절연 폭 마킹 스펙

	////////////////////////////검사 관련
	int		m_nFullDefectCount[MAX_CAMERA];
	int		m_nMaxImageValue[MAX_CAMERA][MOD_NUMBER];
	int		m_nMinImageValue[MAX_CAMERA][MOD_NUMBER];
	double	m_dEdgeSumValue[MAX_CAMERA][MOD_NUMBER];
	int		m_nLeftSkip;//영상의 Left Skip
	int		m_nRightSkip;//영상의 Right Skip
	CRect m_rtEdgeRoi[MAX_CAMERA][2];
	int		m_nHugeCount[MAX_CAMERA];
	CRect m_rtSkipRange[MAX_CAMERA][MAX_HUGEDEFECT*2];//이음매테이프, 테이프 자국등 거대 불량 검출용  8픽셀 이상 크기만 검출 0-9는 원본용 rect 10-19 압축렉트//220303
	double	m_dSubPeakData[MAX_CAMERA][MOD_NUMBER][5];//0라인피크 1백색피크 2흑색피크
	double	m_dsubAveragValue[MAX_CAMERA][MOD_NUMBER][2][40];//0 무지 1코팅
	int		m_nSubDefectBright[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT];//백색 1 흑색2 라인3
	CPoint	m_ptSubDefectPoint[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT];//백색 1 흑색2 라인3
	int		m_nSubDefectType[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT];
	double m_dPeakData[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][2][5];//0 origin 1 preprocess 0max 1min 2ave 34dummy
	double	m_dSubAngle[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][2];//0white 1black
	double	m_dSubStdValue[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][2];//0white 1black
	double	m_dSubCompactness[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][2];//0white 1black

	double	m_dSubSizeX[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][2];			//0white 1black		roi X mm
	double	m_dSubSizeY[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][2];			//0white 1black		roi Y mm

	double	m_dSubSize[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][2];			//0white 1black
	CRect	m_rtSubBlob[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][2];//blob size 0white 1black
	double	m_dSubDistance[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][2];
	double	m_dSubPercent[MAX_CAMERA][MOD_NUMBER][MAX_SPLIT_DEFECT][2];
	double	m_dInsTime[MAX_CAMERA][10];
	double	m_dInsSubTime1[MAX_CAMERA][20];

//	double  m_dInsSubTime2[MAX_CAMERA][MOD_NUMBER][10];
	double	m_dMiddleAve[MAX_CAMERA];

	///////////////불량 정리
	BOOL	m_bDefectColor[MAX_CAMERA][MAX_FULL_DEFECT];//불량 color
	BYTE *	m_fmDefect[MAX_CAMERA][MAX_FULL_DEFECT];//불량 이미지
	CRect	m_rtFullDefect[MAX_CAMERA][MAX_FULL_DEFECT];//불량 영역

	CRect	m_rtDetailRawDefect[MAX_CAMERA][MAX_FULL_DEFECT];
	double	m_dDetailFullArea[MAX_CAMERA][MAX_FULL_DEFECT];
	double	m_dDetailFullWidth[MAX_CAMERA][MAX_FULL_DEFECT];
	double	m_dDetailFullLength[MAX_CAMERA][MAX_FULL_DEFECT];

	int		m_nFullDefectType[MAX_CAMERA][MAX_FULL_DEFECT];//defect Code
	double	m_dFullSizeX[MAX_CAMERA][MAX_FULL_DEFECT][3];			//0white 1black
	double	m_dFullSizeY[MAX_CAMERA][MAX_FULL_DEFECT][3];			//0white 1black
	double	m_dFullCompactness[MAX_CAMERA][MAX_FULL_DEFECT][2];		//0white 1black
	double	m_dFullAngle[MAX_CAMERA][MAX_FULL_DEFECT][3];
	double	m_dFullStdValue[MAX_CAMERA][MAX_FULL_DEFECT][2];
	double	m_dFullDistance[MAX_CAMERA][MAX_FULL_DEFECT][2];
	double	m_dFullPercent[MAX_CAMERA][MAX_FULL_DEFECT][2];
	double	m_dFullPeakDataW[MAX_CAMERA][MAX_FULL_DEFECT][2];//0-Origin 1Preprocess
	double	m_dFullPeakDataB[MAX_CAMERA][MAX_FULL_DEFECT][2];//0-Origin 1Preprocess
	CString	m_strFullDefectName[MAX_CAMERA][MAX_FULL_DEFECT];

	CRect	FlipRect(CRect rect, int nh);
	void	ValidRect(CRect *rect, int nCam, int nflag);;

	int		m_nMaxDefectBright[MAX_CAMERA];
	int		m_nMinDefectBright[MAX_CAMERA];
	double	m_dPenV[MAX_CAMERA];
	int		m_nAddFrame[MAX_CAMERA];
	int		m_nCnt[MAX_CAMERA];

	int CInspect::ROIInspect1(int nCam,BYTE * fmw,BYTE* fmb, int nw, int nh, CRect rt);
	int		m_nManualLeftEdge[20000];
	int		m_nManualRightEdge[20000];

	int m_nLeftNoneCoatingBlobCnt[MAX_CAMERA];
	int m_nRightNoneCoatingBlobCnt[MAX_CAMERA];

	//debug  변수 추가
	IPVM::Rect	m_DebugrtTabSearch;
	std::vector<int> m_DebugvecnProjTab;
	long		m_DebugnEdgeTh1;
	long 		m_DebugFindTabPoint[2];


	CInspect(int nAnodeCathode);
	virtual ~CInspect();

private:
	CWnd*	m_pParent;
};

#endif // !defined(AFX_INSPECT_H__1387D31A_280C_4020_B39F_BE013B2EEADD__INCLUDED_)
//History
//220512
//Scrab 2 defect 사용시 사이즈 만으로 스크래 판단부분 수정
//				디펙밝기- 평균밝기 > 스크랩TH 보다 크고 스크랩 사이즈 스펙이 만족할 경우 스크랩으로 판단. 
//220509
//에지 틸스 사용시 우측 스크랩 못잡는 버그 수정 
//220505
//Tilt Edge 사용시 Vhome 예외처리 추가
//220503
//탭 폭이 250 픽셀 보다 작을 경우 250으로 치환하는부분수정 250 -> 170 
//ESWA Spur 영역 사이즈 기존(핀홀/라인)을 사용할지 스퍼 사이즈를 사용할지 추가
//BOOL	m_bSpurSizeUse[MAX_CAMERA];//스퍼영역에 독립 사이즈 사용할지 여부								//파라미터 추가 필요.
//double m_dSpurSpecSizeX[MAX_CAMERA];//스퍼 불량 사이즈x												//파라미터 추가필요.
//double m_dSpurSpecSizeY[MAX_CAMERA];//스퍼 불량 사이즈y												//파라미터 추가필요.
//double m_dIslandRatio[MAX_CAMERA];//아일랜드 주름 종횡비 -> 기존 라인 종횡비를 따라 가나 신규 수정	//파라미터 추가필요.
//기존 라인 종횡비를 전극라인,절연라인,탭주름 에 사용하였으나 기존 라인 종횡비는 전극 라인만 사용하고 나머지는 신규 파람 m_dIslandRatio 로 수정
//m_dIslandRatio - > 아일랜드/탭주름 구분, 절연 핀홀/라인 구분에 사용.
//m_dXYRatio -> 전극 핀홀/라인만 구분
//220427 
//절연 핀홀 검사 수정 : 탭밝기와 비교 부분 삭제, 히스토그램 최대값 삭제, 절연 우측 영역 마진 제거
//220426
//N2.2 절연 흑라인 불량 코드 수정 : 기존 3003(크랙) -> 7002(절연라인)
//크랙 / 찍힘 관련
//찍힘 흑색 무게 중심에서 각 중심별 산포 삭제 // 크랙을 찍힘으로 과검
//찍힘 로직 변경 : Compactness는 20 이하이고 면적퍼센트는 40 이상이고 각도는 10도 이내
//Tab 히스토그램 부분 백색 에서 기준잡는 문제 수정
//탭 히스트그램에서 100개 이상 모인 지점 에지로 인식하는 부분 파라미터 처리 
//m_nTabWhiteCount 변수 추가 파라미터 추가 필요. Default : 50 // 롤에 이물이 많은 경우 100으로 상향 필요.
//220414-1
//탭주름 관련 검사 추가
//	BOOL	m_bZipperInspect[MAX_CAMERA];//ESMI 탭주름 검사	유				- parmaeter 추가 필요
//	int		m_nTabZipperTh[MAX_CAMERA];//탭주름 TH							- parmaeter 추가 필요
//	double	m_dTabZipperLeftOffset[MAX_CAMERA];//어깨선 에지부터 Offset mm 	- parmaeter 추가 필요
//	double	m_dTabZipperRightOffset[MAX_CAMERA];//어깨선 에지부터 Offset mm - parmaeter 추가 필요
//	double	m_dTabZipperTBOffset[MAX_CAMERA];//탭 상하 옵셋 mm 				- parmaeter 추가 필요
//	double	m_dZipperSpecSizeX[MAX_CAMERA];//불량 판단 사이즈 SpecX			- parmaeter 추가 필요
//	double	m_dZipperSpecSizeY[MAX_CAMERA];//불량 판단 사이즈 SpecY			- parmaeter 추가 필요
//	double	m_dZipperYMinSize[MAX_CAMERA];//영역내 특정 Y사이즈를 갖는경우 불량 후보개수
//	int		m_nZipperSpecCount[MAX_CAMERA];//스펙보다 작은경우 갯수
//검출 조건1 : Spec Size x Y 보다 클경우 주름 불량
//검출 조건2 : Spec Size X,Y 보다 작ㅇ나 Y길이가 m_dZipperYMinSize보다 크고 각도가 60도 보다크고 
//				원형도가 20% 이하의 개수가 m_nZipperSpecCount 보다 많으면 불
//220414
//m_nEdgeSearchDirection -> 에지 검색 방법 추가  = 0 기존 방법 (롤->전극) , 1 수정방법(전극->롤) , parameter 추가 필요 (추후 확장을 감안하여 int 형으로 필요함)
//m_nEdgeSearchDirection ==> 원단과 롤과의 밝기차이보다 5정도 작게 입력 해야함. ESMI 음극의 밝기 차이가 작게 나는 경우는 10정도로 설정.
//FindEdge1 함수에 수정됨.
//220413
//Findnoncoating -> 우측 미코팅 영역 Draw 관련 옵셋 삭제
//탭 우측 검색에서 평균을 구한후 평균보다 1포인트라도 더 큰 위치가 있는면 에지 위치 수정부분을 20포인트 이상일 경우로 수정.(ESMI 과검)
//220411
//Scrab 검사 Thread로 변경
//절연 검사 순서 변경 : 기존 ->  탭영역 찾기 -> 미코팅 검사 -> 어께선 찾기 -> 오버레이 -> 절연 ->Scrab -> Spur 영역 -> 아일랜드 -> NG Tag
//						수정 ->  탭영역 찾기 -> 어께선 찾기 -> 오버레이 -> 절연 ->미코팅 검사 -> Spur 영역 -> 아일랜드 -> NG Tag
//								 Scrab thread로 따로 검사
// Scrab Thread 관련 아일랜드 불량에 포함되어 잇는부분 따로 스크랩 불량으로 수정
//220406
//FindScrab - 스크랩 영역이 5000 이상될 경우 스크랩 검사가 안되는 부분 수정 - ESNB
//220404
//Notch Area Draw 관련 Image Flip (Y방향 reverse)
//m_bEdgeNoiseSkip[nCam] -  FindEdge1함수에 우측(right) 에지에 노이즈가 있는경우 좌측이 전극밝기 +- 20 이내에 들어오는곳을 에지로 찾게 수정 -param추가 필요.
//220403
//n2.2 좌측 Skip 영역 추가  
//		m_dInsulDarkLeft[MAX_CAMERA] - Parameter 추가 필요 단위 mm
//m_bTiltEdge Use시 윗부분에지와 아래부분 에지를 Fit하여 검사영역설정 - parameter 추가 필요.
//		-> Scrab , 미코팅 , 표면검사 해당됨.
//rect 관련
//m_rtCell[nCam]				Cell Rect
//m_rtCellTab[nCam]			Tab Rect
//m_rtInsulate[nCam]			절연 Rect
//m_rtOverlay[nCam]			Overlay Rect
//m_rtNotchArea[nCam][0] ~[3]	노치 위치 (LT->LB->RT->RB)
//m_rtFullInspRect[nCam][0];	좌측 미코팅 검사영역
//m_rtFullInspRect[nCam][1];	우측 미코팅 검사영역
//m_rtFullInspRect[nCam][2];	전극 검사영역
//m_rtFullInspRect[nCam][3];	Spur 검사영역
// m_dFullLength 변수에 전장길이 입력 필요.
//220330
//Scrab 검사 관련	- ESNB
//		-> 기존 : 스크랩 검사 후 오버레이 측정 -> 수정: 오버레이 측정후 스크랩 검사
//		- Scrab 검사 사이즈 비교후 검사영역의 폭이	8000 픽섹을 넘을 경우 X 4픽셀마다 검사
//													500 이상 8000 이하일 경우 X 2픽셀마다 검사로 수정
//탭 우측 에지 검사 - ESWA
//		-> 기존 : 탭 우측 에지를 먼저 찾고(중심부 200픽셀의 평균) 평균 보다 큰 우측에 평균을 사용
//		-> 수정 : 가장큰 우측값으로 소팅하여 우측 10개의 평균으로 에지 검출
//오버레이 측정 관련 기존 4.2mm 고정값으로 들어가 있는 부분 Spec을 따라가게 수정. - ESOC
//220328
//findhugedefect  백색롤 체크시 롤 평균밝기를 흑색에서 찾는 버그 수정
//n2.2 SBR 절연 관련 절연내 흑색 라인 검출 - 오창
//		추가 파라미터 필요.
/*
BOOL	m_bInsulateDarkInsp[MAX_CAMERA];//절연 다크검사 사용유무  //parameter추가						default FALSE
double	m_dInsulDarkRight[MAX_CAMERA];//전극에서 일정 영역만 다크검사를 한다. 단위 mm//parameter추가	default 2 mm
int		m_nInsulDarkTh[MAX_CAMERA];//절연 다크 검사 TH 절연 평균밝기 - TH 로이진화//parameter추가		default 30
double	m_dInsulDarkLineX[MAX_CAMERA];//절연 흑 line X Size //parameter추가								default 1 mm
double	m_dInsulDarkLineY[MAX_CAMERA];//절연 흑 line Y Size //parameter추가								default 2 mm
BOOL	m_bInsulDarkLineSizeAndOr[MAX_CAMERA];//Size And Or TRUE And 조건 FLASE OR조건//parameter추가	default TRUE  
BOOL	m_bInsulWhiteLineSizeAndOr[MAX_CAMERA];//Size And Or TRUE And 조건 FLASE OR조건//parameter추가	default TRUE
BOOL	m_bInsulWhitePinholeSizeAndOr[MAX_CAMERA];//Size And Or TRUE And 조건 FLASE OR조건//parameter추가	default TRUE
*/
// 절연 불량(백색) 사이즈 And , or 조건 추가 - 김진서 팀장님 요청
//220325
//inspect  m_bDefect2Scrab[nCam] 변수 TRUE시 표면 불량도 Scrab으로 FALSE시 표면 불량만 전송	//esmi
//findnoncoating	에 미코팅 전극방향 영역 파라미터 추가  //esmi 만 FALSE
//		m_dLeftNoncoatRange[MAX_CAMERA];//Bottom(Trim)부 미코팅 에지기준 우측 검사영역 mm  //esmi 는 2mm 사용
//		m_dRightNoncoatRange[MAX_CAMERA];//Tab부 미코팅, 에지기준 좌측 검사영역 mm			//우측 영역을 critical 하니 우선 8mm 유지
//절연 표면 검사영역 고정값 -> 절연폭만큼 수정  preprocessedge 함수
//220321
//Preprocessedge 테이프 검출시 스크랩으로 불량 보낼지 유무에 대한 버그 - 테이프가 검출될경우만 스크랩 검사를 하지 않게 수정.
//Findtabpos	탭 위아래 에지 찾을 경우 4픽셀의 차이만 비교 -> 4픽셀 8픽셀 2개를 만족한 경우 에지로 인식 //ESNB
//220318
//findisland 이미지 포인터 접근시 이미지 폭 잘못 되는 버그 수정 - ESWA에서 확인.
//tab 우측 영역검사시 우측 마진 600pixel(25mm) -> 450 pixel(19mm)로 수정 ESMI
//220314
//Preprocessedge  tab 우측 영역에 대해 기존 중심부 평균 -> 중심부 평균을 구한후 평균보다 더 오른쪽에 있응 좌표만 다시 평균으로 구함.
//220311
//Initinspect  Pointer deleteㅅ Null 추가
//220307
//FindTabpos	- m_nTabTopBtmTh 탭상하 TH 추가 , 탭영역의 ProjectionData를 100개 이상인 지점을 해당밝기로 수정
//220304
//preprocessedge - tab 우측 에지 검출로직 수정 기존 TH 이상만 확인 -> TH + 탭밝기가 만족되는 곳을 우측 에지로 검출
//Findtabpos - 탭 상하검출 로직 수정 - 기존 롤포함 탭영역의 프로젝션데이터에서 255에서 역으로 10개이상 개수가 있는곳을 현재 밝기로 선정
//				-> Histogram 3개 이상의 위치가 10개이상인 곳을 현재 밝기로 수정  (Roll 이물/스크래치로 인한 탭에지 오인식문제) -esnb eswa

//220303
//FindHugedefect, inspect  거대불량에 대한 디펙 소트시 최대 불량 카운트 확인 적용
//  m_bTapeSkip[MAX_CAMERA];//테이프 검사후 이후 탭영역 검사 유무 파라미터  //꼭추가필요.
//  m_bTapeInScrab[MAX_CAMERA];//테이프 검사시 Scrab도 같이 전송할지 유무  //꼭 추가 필요.
//	inspect - Option화 테이프 검출시 이후 검사 진행 여부 추가
//	inspect - Option화 테이프를 스크랩으로 전송할지 파라미터 추가
//220225
//m_nEdgeAreaEndPos 에지 검사 끝부분 1000픽셀 변수 추가
//탭 검사영역 width <0 || height<0 불량 처리후 검사 안하게 수정
//220223
// 탭내 전극 에지 검출 - 탭높이가 작을경우 에지를 넘어가는 문제 발생. 100->80으로 검색영역 수정(10.5mm 이내의 탭일경우 문제발생)
//InspInsulateWidth - 오버레이검사를 안할때는 오버레이 보정을 안하게 수정.
//FindTabpos - 백색롤 들뜸부분 검사를 위한 부분에 TH값 추가(롤이 어두운경우 백색롤 체크시문제발생)
//220222
//불량 Summary 부분에 Scab 불량에 대한 갯수 초과 추가 (Inspect 함수)
//220221
//흑색불량알 경우 Size 안나오는 버그 수정 (checkcolor 함수)
//전극의 상부와 하부의 에지 차이가 10픽셀이내일 경우 안쪽에지를 쓰게 수정(기울어짐) FindEdge1 함수
//220220
//표면검사 에지부 고정 스킴영역 제거 및 표면검사 에지를 압축영상 기준으로 가져오는부분 수정 InspectMod 함수
//220216
// m_nProtrusionTh  추가(ESWA요청) 
// 롤에 찍힘 발생으로 탭 에지 찾는 오류 (ESWA) -> 기존 가장 밝은 값에서 -> 히스트그램에서 10개이상 밝은값으로 수정
//220214
//탭에지 찾는 방법 수정
//	FindTabPos - 8필셀 차이값의 평균으로 TH값 조정 , 탭의 들뜸 확인
//미코팅(탈리) 사이즈 수정
//  FindNonCoating - Y 픽셀수가 3픽셀 이하 -> 2픽셀 이하 Skip으로 수정 , 카운트 증가가 X/Y Size 둘중 한개만 만족해도 카운트 증가
//220211
//오버레이 TH 추가
//절연 Tape 검사 유부 추가(세라믹이나 SBR일 경우 사용하면 안됨)
//절연 tape 검사 항목 변경(마이너스어깨선인경우 절연 치수 미표시, 오버레이 평균밝기가 전극밝기 +70 보다 큰경우, 절연밝기가 탭밝기-아일랜드TH 보다 낮은경우
//Findedge1 롤 이물스킵을 위해 에지 찾을시 더 안쪽의 밝기도 체크하도록수정
//Preprcessedge 탭 상하 검출시 더 탭쪽의 밝기도 한번더 체크
//220210
//절연 테이프 검사 사용 유무 m_bInsulTapeInsp
//220209
//Preprcessedge - 탭 상/하 찾는 범위를 전극에서 Offset를 줘서 탭이 휘어지는 부분을 스킵.(Offset 값 16->4로 축소)
//220207
//Find Edge1 - 에지 찾는 방법 수정 (기존 200픽셀 평균 -> Median 50~150 사이 평균
//Preprcessedge - 탭 상하 찾는 방법 수정 (기존 Tab X평균 데이터 에서 X축 가장 밝은 밝기로 수정
//				- 미사용 평션 제거
//				- NGtag 검사 ( Tab 최대 밝기와 평균 밝기가 50 이상 차이가 날경우만 평균위에 평균 구하는 방식-> 항상 평균 위에 평균구하도록수정
//220202
//Ovelay Error 추가 - 탭부위에 테이프 붙일 경우 오버레이 -0.1 리턴
//220202
//ResetData추가
//220202
//V home X Y 마진 
//220128
//FindSpur  흑색 검사 추가
//220125
//- FindEdge1 V홈 위치 전달받은 위치에서만 찾게 수정 , V홈 거리가 0일경우 Cell margin을 기본으로 하도록 수정
//- FindScrab 스크랩 갯수 수정
//-FindHugeDefect WhiteRoll=TRUE 시 다크이미지에서 탭 영역 Blob 하도록 수정
//- V홈 skip 영역 마진: y - 50(위/아래 각각), x - 80(전극끝단에서 안쪽으로)

////220124
////FindHugeDefect 이진화 이미지 초기화 추가
////FindEdge1 V홈 위치 우측 버그 수정
////220122
//Tab 에지 찾는 방법 수정 (탭 중앙에서 탭 상/하로 검색 -> 영상 Top-> Bottom  , ,Bottom -> Top으로 수정 )- 탭중앙에서 찾을 경우 탭에 검은색이 존재하면 탭을 작게 찾는버그
