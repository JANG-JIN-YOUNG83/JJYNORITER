// Inspect.cpp: implementation of the CInspect class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "../pch.h"
#include "Inspect.h"
#include <math.h>
#include <STDLIB.H>
#include "fchain.h"

#include <algorithm> //0607

#include <chrono>

#include "../SystemParameter.h"

#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>

#define _mm_cmpge_epu8(a, b) \
        _mm256_cmpeq_epi8(_mm256_max_epu8(a, b), a)

#define _mm_cmple_epu8(a, b) _mm_cmpge_epu8(b, a)

#define _mm_cmpgt_epu8(a, b) \
        _mm256_xor_si256(_mm_cmple_epu8(a, b), _mm256_set1_epi8(-1))

#define _mm_cmplt_epu8(a, b) _mm_cmpgt_epu8(b, a)

using namespace std;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define OBJECT		0xff
#define BACKGROUND	0

///////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInspect::CInspect(int nAnodeCathode)
{
	for (int i = 0; i < MAX_CAMERA; i++)
	{
		m_ntimeoutCount[i] = 0;
	}
	m_Debuglog = nullptr;
	m_bPinholeAndOr = TRUE;//핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 폴란드는 여기 TRUE로 바꿀것. //211006 
	// 0 : Burr 검사하고 Bot Overlay 검사안함,  1: Burr 검사안하고 Bot Overlay 검사함
	m_nCellSize = 0;//0801	//0726
	m_nAnodeCathode = nAnodeCathode;
	m_bInit = FALSE;
	m_bStopInsp = FALSE;
	m_bNgTag = FALSE;//0625
	m_bPrimer = FALSE;//0630
	m_bInspManual = FALSE;
	m_bUseNewLogic = false;
//	m_nStandardModel = 0; //0925-1
	m_nCellMargin = 100;
	//0809
	m_bOverlayInsp = TRUE;//0729
	m_BlackTapeInsp = FALSE;//0613
	m_bInsulateInsp = TRUE;//0729
	m_bBurrInspect = FALSE;//0803
	m_bUseTimeOut = true;//220705
	//////////////MANUAL Inspect	//0804
	m_nnManualOverlayTh = 13;
	m_nManualCoatTh = 65;
	m_nManualInsulTh = 40;
	m_dManualratio1 = 0.75;
	m_nManualDefectCount = 0;
	m_pManualChain = NULL;
	m_pManualChain = new CChain(1000, 100000);
	m_nAnodeCathode = nAnodeCathode;
	for (int i = 0; i<MAX_MANUAL_DEFECT; i++){
		m_fmManualDefect[i] = new BYTE[BAD_WIDTH*BAD_HEIGHT];
		m_rtManualDefect[i].SetRect(0, 0, 0, 0);
		m_nManualDefectType[i] = 0;
		m_strManualDefectName[i] = "";
		m_nManualDefectBright[i] = 0;
		m_ptManualDefectPoint[i].SetPoint(0, 0);
		for (int j = 0; j<2; j++){
			m_dManualStdValue[i][j] = 0;
			m_dManualCompactness[i][j] = 0;
			m_dManualAngle[i][j] = 0;
			m_dManualDistance[i][j] = 0;
			m_dManualPercent[i][j] = 0;
			m_dManualSizeX[i][j] = 0;
			m_dManualSizeY[i][j] = 0;
			m_dManualPeakDataW[i][j] = 0;
			m_dManualPeakDataB[i][j] = 0;
		}
	}
	//////////////
	for(int i=0;i<MAX_CAMERA;i++)
	{
		//230417 PreprocessEdge 함수 최적화
		m_fmLeftNonCoat[i] = NULL;
		m_fmRightNonCoat[i] = NULL;
		m_fmLeftDarkInsp[i] = NULL;
		m_fmRightDarkInsp[i] = NULL;
		m_nLeftNonCoatBufferWidth[i] = 0;
		m_nLeftNonCoatBufferHeight[i] = 0;
		m_nRightNonCoatBufferWidth[i] = 0;
		m_nRightNonCoatBufferHeight[i] = 0;
		m_fmtmp[i] = nullptr;
		m_fmBlack[i] = nullptr;
		m_nSpurBufferSizeX[i] = 0;
		m_nSpurBufferSizeY[i] = 0;
		m_fmInsul[i] = NULL;
		m_fmInsulDark[i] = NULL;
		m_fmInsulGap[i] = NULL;
		m_nInsulateBufferWidth[i] = 0;
		m_nInsulateBufferHeight[i] = 0;
		//230417 PreprocessEdge 함수 최적화
		//230217 SJM ESOC NGTAG
		m_rtNaTagOffset[i] = CRect(25, 25, 25, 25);
		m_rtBinaryNonCoating[i][0] = CRect(0, 0, 0, 0);
		m_rtBinaryNonCoating[i][1] = CRect(0, 0, 0, 0);
		m_nNoncoatingTh[i][0] = 0;
		m_nNoncoatingTh[i][1] = 0;
		m_nNoncoatingTh[i][2] = 0;
		m_nNoncoatingTh[i][3] = 0;
		m_nNoncoatingAve[i][0] = 0;
		m_nNoncoatingAve[i][1] = 0;
		m_nNoncoatingAve[i][2] = 0;
		m_dDefectPercent[i] = 70, 0;
		//230222 SJM ESWA MINIPINHOLE
		// BOOL	m_bMiniPinhole[MAX_CAMERA]; //WA 전용 미니핀홀 옵션
		m_bMiniPinhole[i] = FALSE;
		m_bMiniPinhole_ifAND[i] = FALSE;
		m_dMiniPinhole_SizeX[i] = 0.1;
		m_dMiniPinhole_SizeX[i] = 0.1;
		m_dMiniPinhole_SizeY[i] = 0.1;
		m_nMiniPinhole_Count[i] = 10; 

		m_bMiniProtrution[i] = FALSE;
		m_bMiniProtrution_ifAND[i] = false;
		m_dMiniProtrution_SizeX[i] = 0.1;
		m_dMiniProtrution_SizeY[i] = 0.1;
		m_nMiniProtrution_Count[i] = 11; 

		m_bMiniDent[i] = FALSE;
		m_bMiniDent_ifAND[i] = false;
		m_dMiniDent_SizeX[i] = 0.1;
		m_dMiniDent_SizeY[i] = 0.1;
		m_nMiniDent_Count[i] = 11;

		m_bMiniCrater[i] = FALSE;
		m_dMiniCrater_SizeX[i] = 0.1;//WA 전용 미니핀홀 옵션
		m_dMiniCrater_SizeY[i] = 0.1;//WA 전용 미니핀홀 옵션
		m_nMiniCrater_Count[i] = 10; //WA 전용 미니핀홀 옵션
		// 
		//221201 ESMI
		m_dInsulgapTh[i] = 1.0;
		m_bInsulGapUse[i] = FALSE;
		m_bUseMinimumDarkSpec[i] = FALSE;

		//221121 YYS 전극 Edge Align 신규 알고리즘 관련 파라미터
		m_bUseIntekCellAlign[i] = false;
		m_bSearchImage[i] = true;//TRUE - Bright FALSE - Dark 엣지 탐색 이미지
		m_nElectrodeEdgeTh[i] = 95;//전극 Edge Threshold
		m_nEdgeContinuousCount[i] = 5;//Threshold 값을 몇번 연속으로 만족해야 Edge로 삼을지
		m_nEdgeSearchAreaSizeY[i] = 200;//Edge Search 영역 Y 사이즈
		m_n2ndEdgeCount[i] = 20;//2차 엣지 포인트 갯수 카운트
		//221121 YYS 전극 Edge Align 신규 알고리즘 관련 파라미터

		//221118 YYS 전극 Edge 관련 신규 파라미터
		m_nLeftElecEdgeSearchAreaSizeX[i] = 1000;
		m_nRightElecEdgeSearchAreaSizeX[i] = 1000;
		m_nElecEdgeSearchAreaTopBotOffset[i] = 0;
		m_nCompareDistance[i] = 8;
		//221118 YYS 전극 Edge 관련 신규 파라미터
		// 
		//221115 YYS 너울성 불량
		m_bBumpyInspect[i] = false;
		m_nBumpyAreaSize[i] = 35;
		m_nBumpyTh[i] = 30;
		m_nBumpyEdgeTh[i] = 10;
		m_nBumpyCompareCount[i] = 5;
		//221026
		m_dInspTimes[i].resize(MOD_NUMBER + 2);
		m_bLaserAndCathode[i] = false;
		//221025
		m_bInspDarkRejectAtBright[i] = true;
		//221116 //추가 파라미터 
		m_bCoatWrinkleUse[i] = false;//FALSE;
		m_dCoatWrinkleAreamm[i] = 3.0;//3mm
		m_nCoatWrinkleTh[i] = 10;//TH
		m_dCoatWrinkleAve[i] = 0;
		m_bInsulAdjust[i] = TRUE;
		//220804
		m_bScrabSizeAndOr = FALSE;//0 or 1 and
		m_dScrabSpecSizeX[i] = 1.0;
		m_dScrabSpecSizeY[i] = 1.0;
		//220721
		m_bOverlayCorrection[i] = TRUE;//오버레이 보정 사용유무

		//220712
		m_bTabEdgerangeTh[i] = TRUE;//탭부 에지(상/하부 ) 전극 밝기로 이진화
		m_dTabEdgeRangemm[i] = 3.0;//탭 상하부 전극 밝기 TH 적용 mm

		//220610
		m_bSbrIsnadImage[i] = FALSE;//False Dark Image True BrightImage
		//220523
		m_dDarkNonCoatSizeX[i] = 2;
		m_dDarkNonCoatSizeY[i] = 2;
		m_bNoncoatDarkInsp[i] = FALSE;//
		m_nNoncoatBlackTh[i] = 0;//Balck Th 0일때 Roll 밝기와 제품밝기를 뺀값을 사용하고 값을 입력시 그값을 제품밝기에서 뺀다.
		//220517
		for(int k=0;k<4;k++)m_rtFixedVhome[i][k].SetRect(0,0,0,0);
		m_bFixedVhome[i] = FALSE;
		m_bTapePreUse[i] = FALSE;
		m_bTabEdgeSelectImage[i] = FALSE;//TRUE BrightImage로 탭 에지 검색 FALSE:DarkImage로 탭에지 검색
		//220516
		m_nTabEdgeRightSkipPixel[i] = 40;
		m_bNoncoatEdgeCheck[i] = FALSE;//TRUE시 에지에 붙은것만 미코팅으로 떨어지면 핀홀 라인 스팩
		m_nNonCoatingEdgeMargin[i] = 6;
		m_nTimeOutAlarmcount[i] = 5;//타임아웃 카운트
		m_bIsCurCellTimeOut[i] = false;
		//220513
		//jjs
		m_nIntekTabSearchPixel[i] = 400;
		m_nIntekTabEdgeRightSkipPixel[i] = 50;


		m_nTabEdgeCriteria[i] = 0;//탭 기준 위치 이미지상 상부가 잘보이면 0 이미지상 하부가 잘보이면 1
		m_dTabWidth[i] = 44.1;//탭폭
		m_dTabHeight[i] = 14.5;
		m_dTabEdgeInspRange[i] = 20;//기존 120 픽셀로 10mm 에서 찾게 되어있었음. 이미지에 상하에 대한 옵셋
		m_dTabRightOffset[i] = 25;// 전극 에지기준 25 mm 우측에서 좌측으로 가면서 에지를 찾음
		m_dTabRightDetectValue[i] = 0; // 우측으로 찾으면서 240이상인 순간 오른쪽 엣지로 찾음
		//220503
		m_dIslandRatio[i] = 4;//아일랜드 윙클 종횡비
		m_dSpurSpecSizeX[i] = 0.8;//Sppur 영역 사이즈 X
		m_dSpurSpecSizeY[i] = 0.8;//Spur 영역 사이즈 Y
		m_bSpurSizeUse[i] = TRUE;//스퍼 사이즈 사용할지 여부
		//220426
		m_nTabWhiteCount[i] = 50;//롤에 이물이 많아 에지를 잘 못찾는 경우 값을 증가해야함.

		m_nTabSearchPixel[i] = 400;

		//220414
		m_nEdgeSearchDirection[i] = 0;//0 roll -> 전극 , 1 전극 -> roll
		//220412 esmi tab zipper
		m_bZipperInspect[i] = FALSE;
		m_nTabZipperTh[i] = 40;
		m_dTabZipperLeftOffset[i] = 1.0;//어깨선 에지부터 Offset mm
		m_dTabZipperRightOffset[i] = 5;//3.5;//어깨선 에지부터 Offset mm
		m_dTabZipperTBOffset[i] = 2.0;//탭 상하 옵셋 mm
		m_dZipperSpecSizeX[i] = 0.5;//Spec X Size
		m_dZipperSpecSizeY[i] = 3;//Spec Y Size
		m_dZipperYMinSize[i] = 1.5;//영역내 특정 Y사이즈를 갖는경우 불량 후보개수
		m_nZipperSpecCount[i] = 2;//스펙보다 작은경우 갯수
	
		//220411
		m_nScrabRangDefect[i] = 0;//scrab bad count
		m_bScrabCheck[i] = FALSE;//스크랩 검사 여부
		m_bEdgeNoiseSkip[i] = FALSE;//220404
		//220401
		m_dInsulDarkLeft[i] = 2;
		m_bTiltEdge[i] = FALSE;//ESMI 기울어짐 심한 설비는 TRUE //TRUE시 기울어짐에 따라 에지를 따라가면 미코팅 검사 FALSE시 기존 동일
		//220328
		m_bInsulateDarkInsp[i] = FALSE; // TURE시 절연영역 다크검사 
		m_dInsulDarkRight[i] = 1.3;//전극 시작에서 2mm 구간에서만 다크 검사를 실시
		m_nInsulDarkTh[i] = 30;//절연 평균 밝기 - TH 로 이진화
		m_dInsulDarkLineX[i] = 1.0;//
		m_dInsulDarkLineY[i] = 2.0;//

		m_dInsulDefectSizeX[i] = 1.0;
		m_dInsulDefectSizeY[i] = 2.0;
		m_dInsulCompact[i] = 70.0f;
		m_bInsulDarkLineSizeAndOr = FALSE;
		m_bInsulWhiteLineSizeAndOr = TRUE;//Size And Or TRUE And 조건 FLASE OR조건//parameter추가
		m_bInsulWhitePinholeSizeAndOr = TRUE;//Size And Or TRUE And 조건 FLASE OR조건//parameter추가

		m_bOverlayThreadEnd[i] = false;
		m_bScrabThreadEnd[i] = false;

		//220325
		m_bDefect2Scrab[i] = TRUE; //esmi 오창은 FALSE
		m_dLeftNoncoatRange[i] = 8.0;//ESMI 2mm
		m_dRightNoncoatRange[i] = 8.0;
		//220321
		m_bFindWhiteTape[i] = FALSE;//tape 검출치 TRUE
		//220307
		m_nTabTopBtmTh[i] = 0;//테이프 상하 TH 0이면 오토, 0보다 크면 수동 입력값 적용.

		m_fLimitTime[i] = 60.0f;

		m_bUseIntekTabAlign[i] = FALSE;//220915 YYS 탭 얼라인 관련

		m_nTabThresholdTopSide[i] = 200;//220915 YYS 탭 얼라인 관련
		m_nTabThresholdBottomSide[i] = 200;//220915 YYS 탭 얼라인 관련
		m_nTabTapeThresholdTopSide[i] = 95;//탭 테이프 상부 엣지 threshold
		m_nTabTapeThresholdBottomSide[i] = 95;//탭 테이프 하부 엣지 threshold
		m_nShadowThreshold[i] = 20;//탭 상하 그림자 threshold
		m_nRollerEndThreshold[i] = 15;//롤러 종료 포인트 측정 Threshold
		m_bTabTopBottomForceCorrection[i] = false;//탭 Top Bottom Y 좌표 강제 보정 여부 
		m_bTabRightPosCorrection[i] = false;//탭 오른쪽 엣지 잘못 측정됐을 때 강제 보정 여부
		m_nTabEdgeSearchHeightPixel[i] = 500;//탭 엣지 검색 높이 픽셀 
		m_fEdgePointFilterValue[i] = 0.5f;//탭 엣지 포인트 필터링 값 (포인트의 범위 값)

		m_bForceCorrectionActivated[i] = false;

		m_bDetectTabBothSide[i] = TRUE;//220915 YYS 탭 얼라인 관련

		m_vecTopY_Candidate[i].clear();//220915 YYS 탭 얼라인 관련
		m_vecBottomY_Candidate[i].clear();//220915 YYS 탭 얼라인 관련
		m_vecTopY_ValueList[i].clear();//220915 YYS 탭 얼라인 관련
		m_vecBottomY_ValueList[i].clear();//220915 YYS 탭 얼라인 관련

		m_vecTopX_Candidate[i].clear();//220915 YYS 탭 얼라인 관련
		m_vecBottomX_Candidate[i].clear();//220915 YYS 탭 얼라인 관련

		m_dequeTabAvg[i].clear();//220915 YYS 탭 얼라인 관련
		m_dequeTabTopY[i].clear();//220915 YYS 탭 얼라인 관련
		m_dequeTabBottomY[i].clear();//220915 YYS 탭 얼라인 관련

		memset(m_dInsulateSortData[i], 0, sizeof(double) * 10);

		m_dResSortData[i] = 0;
		for (int j = 0; j < 10; j++)
		{
			m_dInsulWidthCandidate[i][j] = 0;
		}
		memset(m_nOverlayROI_Avg[i], 0, sizeof(int) * 10000);
		m_nRealCalcOverlayPos[i] = 0;
		m_nCorrectionOverlayPos[i] = 0;

//220303
		m_bTapeSkip[i] = TRUE;//테이프 검사후 이후 검사 유무 판정 //TRUE시 테이프 다른 검사 실행.
		m_bTapeInScrab[i] = FALSE;//테이프 검사시 Scrab도 같이 전송할지 유무//TRUE 시 테이프도 Scrab으로 불량 추가전송
		m_dDefectOverlabRange[i] = 1.0;//Defect 검사시 Overlab 영역 설정 mm
		m_nDefectOverlabRange[i][0] = m_dDefectOverlabRange[i]/m_dscale_x1[i];//Defect 검사시 Overlab 영역 설정 pixel 0X 1Y
		m_nDefectOverlabRange[i][1] = m_dDefectOverlabRange[i]/m_dscale_y1[i];//Defect 검사시 Overlab 영역 설정 pixel 0X 1Y

		//m_nEdgeAreaEndPos[i] = 1000;//220225
		m_nOverlayTh[i] = 0;//220211 오버레이 에지 TH 0입력시 기존 값 사용
		m_bScrabFullRange[i] = FALSE;
		m_nInsulTh[i] = 0;//0 입력시 가장 큰 차이
		m_nInsulRightOffset[i]=10;//절연 시작부 불량검사 스킵 영역 Pixel
		m_rtInsulationInspAreaOffset[i] = CRect(0, 0, 10, 0);
		m_bBottomOverayInsp[i]=FALSE; // Long Cell Bottom Overlay Inspect
		m_nOffsetUp[i]=0;//탭에서 상부노치까지 픽셀
		m_nOffsetDn[i]=0;//탭에서 하부노치까지 픽셀
		m_bCemramic[i] = FALSE;//Cemramic or SBR
		m_bNotch[i] = TRUE;// 노치 유무

		m_nNotchCount[i] = 4;//220110
		m_nCoatEdgeTh[i] = 30;
		//211126
		m_nLargeDefectWThPre[i] = 50;
		m_nLargeDefectBThPre[i] = 50;

		m_bTabPoly[i] = FALSE;//211123
		for(int k=0;k<5;k++)m_rtFullInspRect[i][k].SetRect(0,0,0,0);
		
		//211106
		m_nNonCoatTh[i] = 60;
		m_bDefectRoiOverlap[i] = TRUE;
//211105
		m_nSpurRangeTopBtm[i] = 40;
		m_nTabEdgeNoncoatOffset[i] = 0;
/////////
		m_bWhiteRoll[i] = FALSE;//백색 롤 1 , 일반롤 0 //211028
		m_bWhiteRoll_NoncoatingImageIndex[i] = FALSE;
		m_bStep[i] = FALSE;// TRUE;//TRUE;//211025  //단차롤일경우 TRUE 아닐 경우 FALSE
		//211104
		m_bBottomNoncoatingUse[i] = TRUE;// 오창 롤 기준 roll에 좌측 기준선이 전극과 닿는 경우 FALSE를 해야함. //장수석님 확인부탁드립니다.
		m_bTabNoncoatingUse[i] = TRUE;

	//211101 
		m_nMarginSearchEnd[i] = 8200;
		m_nNonCoatOffset[i] = 38;
		////////211023
		m_nLineValue[i] = 80;//라인용 Value
		m_bResetLeftAreaBlob[i] = 0;

		m_nIslandOffset[i] = 4;//아일랜드 좌측 옵셋
		m_nSpurOffset[i] = 8;//Spur 용 우측 옵셋
		m_nScratchValue[i] = 35;//스크래치 Value
		
		m_fScratchRatio[i] = 0.25;//스크래치 Value
		m_fScratchLength[i] = 0.25;//스크래치 Value

		m_nCrackValue[i] = 40;//크랙 
		m_nSpurValue[i] = 130;
		m_dIslandCompactTh[i] = 30; 
		m_dIslandPercentTh[i] = 30;
		////////////0930
		m_nMarginSearchStart[i] = 200;
		m_nCellSkipBrightness[i] = 160;//211017-1  //PET Skip 밝기

		m_bInsScrab[i] = TRUE;//FALSE;
		m_nScrabRoiTop[i] = 500;
		m_nScrabRoiBot[i] = 500;
		m_nScrabRoiLeft[i] = 400;//우측 에지 기준 Offset1 픽셀
		m_nScrabRoiRight[i] = 30;//우측 에지 기준 offset2 픽셀
		m_dScrabSize[i] = 0;//X Y 중 큰 값
		m_dSpecScarabSize[i] = 1.5;//불량 스펙 사이즈
		m_nScrabTh[i] = 100;//밝은 영상 평균 밝기 + TH  = 이진화값  
		m_bDefectToScrabWithThreshold[i] = false;

		m_bSpatterInsp[i] = FALSE;
		for (int j = 0; j<2; j++){//0925
			m_dSpatterSizeX[i][j] = 0.12;//0 White 1 Black
			m_dSpatterSizeY[i][j] = 0.12;//0 White 1 Black
			m_dDrossSizeX[i][j] = 0.12;//0 White 1 Black
			m_dDrossSizeY[i][j] = 0.12;//0 White 1 Black
			if (j == 0){
				m_dSpatterRange[i][j] = 50;//0 mm  스패터 검사영역
				m_nSpatterTh[i][j] = 100;//0 white
			}
			else{
				m_dSpatterRange[i][j] = 50 / 0.042;// 1 pixel 스패터 검사영역
				m_nSpatterTh[i][j] = 50;// 1black
			}
		}

		m_dInsulateThGap[i] = 1.4;
		m_dTabLabelSkipTB[i] = 1;//0901
		m_dTabLabelSkipLR[i] = 5;//0901
		m_dCrackAngle[i] = 70;
		m_dTabBrightness[i][0] = m_dTabBrightness[i][1] = 0;//0809
		m_nLabelTagTh[i] = 80;//0720
		m_bInsp[i] = FALSE;
		m_bOverlayCheck[i] = FALSE;
		m_pChainHuge[i][0] = NULL;
		m_pChainHuge[i][1] = NULL;
		m_pChainInsul[i] = NULL;
		m_pChainIsland[i] = NULL;
		m_pChainNoncoating[i] = NULL;
		m_pChainScrab[i] = NULL;
		m_pChainAtPreprocess[i] = NULL;

		m_pFindElectrodeEndPos[i] = NULL;

		for(int j=0;j<MOD_NUMBER;j++){
			m_pChain[i][j][0] = NULL;
			m_pChain[i][j][1] = NULL;
		}
		for(int j=0;j<10;j++)m_dInsTime[i][j]=0;
		m_rtHugeRect[i].SetRect(0,0,0,0);
		m_rtMinTab[i].SetRect(0,0,0,0);
		m_rtOverlay[i].SetRect(0,0,0,0);
		m_nFullDefectCount[i] = 0;

		for(int j=0;j<4;j++){
			m_ptFullNotch[i][j].SetPoint(0,0);
			m_ptNotch[i][j].SetPoint(0,0);
		}
		m_rtCell[i].SetRect(0,0,0,0);
		m_rtInsulate[i].SetRect(0,0,0,0);
		m_rtCuttingHeightTop[i].SetRect(0,0,0,0);
		m_rtCuttingHeightBottom[i].SetRect(0,0,0,0);
		m_rtInsulationInspArea[i].SetRect(0, 0, 0, 0);
		//0801      ///////////
		for(int x1=0;x1<2;x1++){
			m_dBottomInsul[i][x1]=0;//0mm 1pixel
			m_dBottomCoat[i][x1]=0;//0mm 1pixel
			m_dBottomOverlay[i][x1]=0;//0mm 1pixel
		}
		/////////////////////
	}

	m_ScrabImage = NULL;
	m_NoneCoatingImage = NULL;
	m_BinaryIslandImage = NULL;
	m_BinaryNgTagImage = NULL;//20221012 SJM

	for (int i = 0; i < 10; i++)
	{
		memset(m_nInsulateRoi10eaSplit[i], 0, sizeof(int) * 1000);
	}

	for (int i = 0; i < MAX_CAMERA; i++) {
		m_BinarySpurImage[i] = NULL;
		m_BinaryInsulateImage[i] = NULL;
		m_fmEven[i] = new BYTE[100*100];
		m_fmOdd [i]= new BYTE[100*100];
		m_fmPreProcess[i] = new BYTE[100*100];
		m_fmPreProcess1[i] = new BYTE[100*100];
		m_fmCompress[i] = new BYTE[100*100];
		m_fmCompressDark[i] = new BYTE[100*100];//211028
		m_fmCompressPre[i] = new BYTE[100*100];//211126
		m_fmTabEdge[i] = new BYTE[100*100];
		m_fmTabEdge1[i] = new BYTE[100*100];
// 		m_fmcompressWhite[i] = new BYTE[100*100];
// 		m_fmcompressBlack[i] = new BYTE[100*100];
		m_fmOverlayPreprocess[i] = new BYTE[100*100];//
		m_fmInsulBlob[i] = new BYTE[100*100];//
		
		for(int j=0;j<MOD_NUMBER;j++){
			memset(m_nPreProcessData[i][j],0,sizeof(int)*MAX_IMAGE_WIDTH);//
			memset(m_nPreProcessData1[i][j],0, sizeof(int) * MAX_IMAGE_WIDTH);//
			for(int k=0;k<MAX_SPLIT_DEFECT;k++){
				for(int l=0;l<3;l++){
					m_pSubDefet[i][j][k][l] = new BYTE[BAD_WIDTH*BAD_HEIGHT];
					memset(m_pSubDefet[i][j][k][l],0,BAD_WIDTH*BAD_WIDTH);//
				}
			}
			m_nSubDefectCount[i][j] = 0;
		}
		for(int j = 0; j < MAX_HUGEDEFECT; j++)
		{
			m_fmHugeDefect[i][j] = new BYTE[BAD_WIDTH*BAD_HEIGHT];
			m_pIsland[i][j] = new BYTE[BAD_WIDTH*BAD_HEIGHT];
			m_pScrabDefect[i][j] = new BYTE[BAD_WIDTH*BAD_HEIGHT];//220411
			m_pInsulDefect[i][j] = new BYTE[BAD_WIDTH*BAD_HEIGHT];
			m_rtIsland[i][j].SetRect(0,0,0,0);
			m_rtInsulateDefect[i][j].SetRect(0,0,0,0);
			m_nIslandCount[i] = 0;
			m_dISSizeX[i][j]= 0;			//0white 1black
			m_dISSizeY[i][j]= 0;			//0white 1black
			m_dISSize[i][j]= 0;			//0white 1black
			m_dISAngle[i][j] = 0;			//0white 1black	//0809
			m_dISCompactness[i][j] = 0;			//0white 1black //0809
			//211103
			m_dISMaxDist[i][j] = 0;
			m_dISMinDist[i][j] = 0;
		}
		for(int j = 0; j < MAX_FULL_DEFECT; j++)
		{
			m_fmDefect[i][j] = new BYTE[BAD_WIDTH*BAD_HEIGHT];
			memset(m_fmDefect[i][j],0,BAD_WIDTH*BAD_HEIGHT);
			m_rtFullDefect[i][j].SetRect(0, 0, 0, 0);
			
			m_rtDetailRawDefect[i][j].SetRect(0, 0, 0, 0);
			m_dDetailFullArea[i][j] = 0.0f;
			m_dDetailFullWidth[i][j] = 0.0f;
			m_dDetailFullLength[i][j] = 0.0f;

			m_nFullDefectType[i][j] = 0;//defect Code
			for(int k = 0; k < 2; k++)
			{
				m_dFullCompactness[i][j][k] = 0.;		//0white 1black
				m_dFullAngle[i][j][k] = 0.;
				m_dFullStdValue[i][j][k] = 0.;
				m_dFullDistance[i][j][k] = 0.;
				m_dFullPercent[i][j][k] = 0.;

			}
			for (int k = 0; k < 3; k++) {//0809
				m_dFullSizeX[i][j][k] = 0.;			//0white 1black
				m_dFullSizeY[i][j][k] = 0.;			//0white 1black
			}

		}

		m_dInsulWidth[i] = 0;//0604
		m_dOverlayWidth[i] = 0;//0604
		m_dOldInsulWidth[i] = 0;//0604
		m_dOldOverlayWidth[i] = 0;//0604

		//m_nLastTimeOutCount[i] = 0;
		
		for (int kuk = 0; kuk <= 20; kuk++)
		{
			m_dInsSubTime1[i][kuk] = 0.0f;

		}
	}

	for(int i = 0; i < MAX_CAMERA; i++)
	{
		m_pChainHuge[i][0] = new CChain(100,100000);
		m_pChainHuge[i][1] = new CChain(100,100000);
		m_pChainInsul[i] = new CChain(300,100000);
		m_pChainIsland[i] = new CChain(300,100000);
		m_pChainNoncoating[i]= new CChain(300, 100000);
		m_pChainScrab[i] = new CChain(300,100000);
		m_pChainAtPreprocess[i] = new CChain(100, 100000);
		m_bOverlayIns[i] = FALSE;

		m_pFindElectrodeEndPos[i] = new CChain(100, 100000);
		
		for(int j = 0;j < MOD_NUMBER; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				m_pChain[i][j][k] = new CChain(300, 100000);
			}

			for (int split = 0; split < MAX_SPLIT_DEFECT; split++)
			{
				for (int dt = 0; dt < DT_TOTAL; dt++)
				{
					for (int pm = 0; pm < 2; pm++)
					{
						m_dDefectAreaSub[i][j][split][dt][pm] = 0.0f;

						m_dDefectCenterPosSubX[i][j][split][dt][pm] = 0.0f;
						m_dDefectCenterPosSubY[i][j][split][dt][pm] = 0.0f;

						m_dDefectWidthSub[i][j][split][dt][pm] = 0.0f;
						m_dDefectLengthSub[i][j][split][dt][pm] = 0.0f;

						m_rtSubRawDefect[i][j][split][dt][pm].SetRect(0, 0, 0, 0);
					}

				}
			}

		}

		for(int j = 0;j < MOD_NUMBER; j++)
			m_bTestFlag[i][j] = FALSE;

		m_dTabSkipRange[i][0] = 1;//0.1;
		m_dTabSkipRange[i][1] = 1;//0.1;//1;//0.1;
		m_dTabSkipRange[i][2] = 1;
		m_dTabTapeX[i] = 1.5;
		m_dTabTapeY[i] = 1.5;//1;
		m_dNoneCoatingSizeX[i] = 0.01;
		m_dNoneCoatingSizeY[i] = 0.15;
		m_nNoneCoatingCount[i] = 2;

		for(int j=0;j<4000;j++)
		{
			//220207
			m_nEdgeData1[i][j] = 0;
			m_nEdgeData2[i][j] = 0;
		}

		m_dVhomeMarginX[i] = 1.1;//25
		m_dVhomeMarginY[i] = 4.4;//50
		m_bInsulTapeInsp[i] = FALSE;//220210 //테이프 검출 여부
	}

	m_bThread = FALSE;
	m_nReduction[0] = 16;
	m_nReduction[1] = 16;
	m_bDefectBinary = FALSE;
	m_nMachine = nAnodeCathode;//양극

	InitParam(nAnodeCathode);
	m_bIstest = FALSE;//20220916 SJM
}
void CInspect::CloseInspect()
{
	m_bStopInsp = TRUE;
	Sleep(100);
	//delete m_pManualChain;
	for (int i = 0; i<MAX_CAMERA; i++){
		free(m_pChainHuge[i][0]);
		free(m_pChainHuge[i][1]);
		free(m_pChainIsland[i]);
		free(m_pChainNoncoating[i]);
		free(m_pChainInsul[i]);
		free(m_pChainScrab[i]);
		free(m_pChainAtPreprocess[i]);

		free(m_pFindElectrodeEndPos[i]);
			
		for (int j = 0; j<MOD_NUMBER; j++){
			for (int k = 0; k<2; k++)
				free(m_pChain[i][j][k]);
		//delete m_pChainHuge[i][0];
		//delete m_pChainHuge[i][1];
		//delete m_pChainIsland[i];
		//delete m_pChainInsul[i];
		//delete m_pChainScrab[i];
		//for (int j = 0; j < MOD_NUMBER; j++) {
		//	for (int k = 0; k < 2; k++)
		//		delete (m_pChain[i][j][k]);
		}
	}
	//for (int i = 0; i < MAX_MANUAL_DEFECT; i++) {
	//	delete[] m_fmManualDefect[i];
	//}
	return;
		delete[] m_BinaryNgTagImage;//20221012 SJM
		delete[] m_BinaryIslandImage;//20220916 SJM
		delete[] m_NoneCoatingImage;//20220916 SJM
		delete[] m_ScrabImage;
	for (int i = 0; i<MAX_CAMERA; i++)
	{
		if(m_fmLeftNonCoat[i])
			delete[] m_fmLeftNonCoat[i];
		if (m_fmRightNonCoat[i])
			delete[] m_fmRightNonCoat[i];
		if (m_fmLeftDarkInsp[i])
			delete[] m_fmLeftDarkInsp[i];
		if (m_fmRightDarkInsp[i])
			delete[] m_fmRightDarkInsp[i];

		if (m_fmtmp[i])
		{
			delete[] m_fmtmp[i];
			m_fmtmp[i] = nullptr;
		}
		if (m_fmBlack[i])
		{
			delete[] m_fmBlack[i];
			m_fmBlack[i] = nullptr;
		}

		if (m_fmInsul[i])
			delete[] m_fmInsul[i];
		if (m_fmInsulDark[i])
			delete[] m_fmInsulDark[i];
		if (m_fmInsulGap[i])
			delete[] m_fmInsulGap[i];

		delete[] m_BinaryInsulateImage[i];
		delete[] m_BinarySpurImage[i];
		delete[] m_fmEven[i];
		delete[] m_fmOdd[i];
		delete[] m_fmPreProcess[i];
		delete[] m_fmPreProcess1[i];
		delete[] m_fmCompress[i];
		delete[] m_fmCompressDark[i];//211028
		delete[] m_fmCompressPre[i];//211126
		delete[] m_fmTabEdge[i];
		delete[] m_fmTabEdge1[i];
// 		m_fmcompressWhite[i] = NULL;//220311
// 		delete[] m_fmcompressWhite[i];
// 		m_fmcompressBlack[i] = NULL;//220311
// 		delete[] m_fmcompressBlack[i];
		delete[] m_fmOverlayPreprocess[i];
		delete[] m_fmInsulBlob[i];
		for (int j = 0; j<MOD_NUMBER; j++){
			for (int k = 0; k<MAX_SPLIT_DEFECT; k++){
				for (int l = 0; l<3; l++){
					delete[] m_pSubDefet[i][j][k][l];
				}
			}
		}
		for (int j = 0; j<MAX_HUGEDEFECT; j++){
			delete[] m_fmHugeDefect[i][j];
			delete[] m_pIsland[i][j];
			delete[] m_pScrabDefect[i][j];//220411
			delete[] m_pInsulDefect[i][j];
		}
		//for (int j = 0; j<MAX_FULL_DEFECT; j++){
		//	delete[] m_fmDefect[i][j];
		//}
		m_vecTopY_Candidate[i].clear();//220915 YYS 탭 얼라인 관련
		m_vecBottomY_Candidate[i].clear();//220915 YYS 탭 얼라인 관련
		m_vecTopY_ValueList[i].clear();//220915 YYS 탭 얼라인 관련
		m_vecBottomY_ValueList[i].clear();//220915 YYS 탭 얼라인 관련

		m_vecTopX_Candidate[i].clear();//220915 YYS 탭 얼라인 관련
		m_vecBottomX_Candidate[i].clear();//220915 YYS 탭 얼라인 관련

		m_dequeTabAvg[i].clear();//220915 YYS 탭 얼라인 관련
		m_dequeTabTopY[i].clear();//220915 YYS 탭 얼라인 관련
		m_dequeTabBottomY[i].clear();//220915 YYS 탭 얼라인 관련
	}
}


CInspect::~CInspect()
{
//	if(m_pChain)	delete m_pChain;
//	delete fmPreProcess;
	
}

void CInspect::InitInspect(int nWidth, int nHeight,int nAnodeCathode, bool bUseNewLogic)
{

	m_bInit = TRUE;
//	InitParam(nAnodeCathode);
	if (m_nAnodeCathode == 0){
		m_bOverlayInsp = TRUE;//오버레이 검사 유무
		m_bInsulateInsp = TRUE;//절연 검사 유무

	}
	else{
		m_bOverlayInsp = FALSE;//오버레이 검사 유무
		m_bInsulateInsp=FALSE;//절연 검사 유무
	}
	//m_BlackTapeInsp = FALSE;//0613
	m_bBurrInspect = FALSE; //0809
	m_nWidthMini = nWidth/m_nReduction[0];
	m_nHeightMini = nHeight/m_nReduction[0];
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_bUseNewLogic = bUseNewLogic;

	for(int i=0;i<MAX_CAMERA;i++){
		m_rtFullRange[i].SetRect(0,0,nWidth,nHeight);
		delete [] m_fmTabEdge[i];
		delete [] m_fmTabEdge1[i];
		delete [] m_fmPreProcess[i];
		delete [] m_fmPreProcess1[i];
		delete [] m_fmCompress[i];
		delete [] m_fmCompressDark[i];//211028
		delete [] m_fmCompressPre[i];//check
// 		delete [] m_fmcompressWhite[i];
// 		delete [] m_fmcompressBlack[i];
		delete [] m_fmOverlayPreprocess[i];	 //check	
		delete [] m_fmInsulBlob[i];		//check
		delete [] m_fmEven[i];
		delete [] m_fmOdd[i];
		delete[] m_BinaryInsulateImage[i];
		delete[] m_BinarySpurImage[i];
	}
	
	if (NULL != m_BinaryNgTagImage)//20221012 SJM
	{
		delete[] m_BinaryNgTagImage;
		m_BinaryNgTagImage = NULL;
	}
	if (NULL != m_ScrabImage)//20220916 SJM
	{
		delete[] m_ScrabImage;//20220916 SJM
		m_ScrabImage = NULL;
	}
	if (NULL != m_BinaryIslandImage)//20220916 SJM
	{
		delete[] m_BinaryIslandImage;//20220916 SJM
		m_BinaryIslandImage = NULL;
	}
	if (NULL != m_NoneCoatingImage)//20220916 SJM
	{
		delete[] m_NoneCoatingImage;//20220916 SJM
		m_NoneCoatingImage = NULL;
	}
	m_BinaryNgTagImage = new BYTE[nWidth * nHeight];//20221012 SJM
	m_BinaryIslandImage = new BYTE[nWidth*nHeight];//20220916 SJM
	m_NoneCoatingImage = new BYTE[nWidth*nHeight];//20220916 SJM
	m_ScrabImage = new BYTE[nWidth * nHeight];//20220916 SJM

	for(int i=0;i<MAX_CAMERA;i++){
		m_BinaryInsulateImage[i] = new BYTE[nWidth * nHeight];//20220916 SJM
		m_BinarySpurImage[i] = new BYTE[nWidth * nHeight];//20220916 SJM
		m_fmEven[i] = new BYTE[nWidth*nHeight];
		m_fmOdd[i] = new BYTE[nWidth*nHeight];
		m_fmPreProcess[i] = new BYTE[nWidth*nHeight];
		m_fmPreProcess1[i] = new BYTE[nWidth*nHeight];
		m_fmTabEdge[i] = new BYTE[nWidth*nHeight];
		m_fmTabEdge1[i] = new BYTE[nWidth*nHeight];
		m_fmCompress[i] = new BYTE[m_nWidthMini*m_nHeightMini];
		m_fmCompressDark[i] = new BYTE[m_nWidthMini*m_nHeightMini];//211028
		m_fmCompressPre[i] = new BYTE[m_nWidthMini*m_nHeightMini];//211126
		m_fmcompressWhite[i] = new BYTE[m_nWidthMini*m_nHeightMini];
		m_fmcompressBlack[i] = new BYTE[m_nWidthMini*m_nHeightMini];
		m_fmOverlayPreprocess[i] = new BYTE[nWidth*nHeight];
		m_fmInsulBlob[i] = new BYTE[nWidth*nHeight];
	}

	if(m_bThread==FALSE){
		m_bThread = TRUE;
		m_hThreadOverlayEnd[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hThreadOverlayEnd[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
		AfxBeginThread(threadInspect1, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect2, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect3, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect4, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect5, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect6, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect7, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect8, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect11, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect12, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect13, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect14, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect15, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect16, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect17, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadInspect18, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadOverlay1, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadOverlay11, this,THREAD_PRIORITY_NORMAL,0);
		AfxBeginThread(threadScrab1, this,THREAD_PRIORITY_NORMAL,0);//220411
		AfxBeginThread(threadScrab2, this,THREAD_PRIORITY_NORMAL,0);//220411

		if (true == bUseNewLogic)
		{
			AfxBeginThread(threadGetInsulateAvgUp, this, THREAD_PRIORITY_NORMAL, 0);
			AfxBeginThread(threadGetInsulateAvgDown, this, THREAD_PRIORITY_NORMAL, 0);

			AfxBeginThread(threadNoncoatingAndSpurUp, this, THREAD_PRIORITY_NORMAL, 0);
			AfxBeginThread(threadNoncoatingAndSpurDown, this, THREAD_PRIORITY_NORMAL, 0);

			//AfxBeginThread(threadIslandNgTagUp, this, THREAD_PRIORITY_NORMAL, 0);
			//AfxBeginThread(threadIslandNgTagDown, this, THREAD_PRIORITY_NORMAL, 0);
		}
	}

}

UINT CInspect::threadInspect1(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0,nindex=0,nMod=MOD_NUMBER;
	while(1)
	{	
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][0] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect2(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0,nindex=1,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][1] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect3(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0,nindex=2,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][2] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect4(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0,nindex=3,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][3] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect5(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0,nindex=4,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][4] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect6(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0,nindex=5,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][5] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect7(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0,nindex=6,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][6] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect8(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0, nindex=7,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][7] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}

UINT CInspect::threadInspect11(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1,nindex=0,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][0] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect12(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1,nindex=1,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][1] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect13(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1,nindex=2,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][2] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect14(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1,nindex=3,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][3] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect15(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1,nindex=4,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][4] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect16(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1,nindex=5,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][5] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}
UINT CInspect::threadInspect17(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1,nindex=6,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][6] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}

UINT CInspect::threadInspect18(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1,nindex=7,nMod=MOD_NUMBER;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evInsp[nCam][nindex],50) != WAIT_TIMEOUT ){
			pInspect->m_evInsp[nCam][nindex].ResetEvent();
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->InspectMod(nCam,nindex,nMod);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][7] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			pInspect->m_bTestFlag[nCam][nindex]=FALSE;
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);//0630
	}
	return 1;
}

UINT CInspect::threadOverlay1(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evOverlay[nCam],50) != WAIT_TIMEOUT )
		{
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->m_dInsulWidth[nCam] = 0;
			pInspect->m_dOverlayWidth[nCam] = 0;
			pInspect->PreprocessEdge(nCam);
			
			pInspect->m_bOverlayIns[nCam] = FALSE;
			
			{
				CSingleLock lock(&pInspect->m_cs_OverlayThreadEnd[nCam], TRUE);
				pInspect->m_bOverlayThreadEnd[nCam] = true;
			}

			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][9] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			::SetEvent(pInspect->m_hThreadOverlayEnd[nCam]);
		}
		if (pInspect->m_bStopInsp)
			return 1;
		
		Sleep(1);
	}
	return 1;

}
UINT CInspect::threadOverlay11(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evOverlay[nCam],50) != WAIT_TIMEOUT )
		{
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->m_dInsulWidth[nCam] = 0;
			pInspect->m_dOverlayWidth[nCam] = 0;
			pInspect->PreprocessEdge(nCam);
			pInspect->m_bOverlayIns[nCam] = FALSE;
			{
				CSingleLock lock(&pInspect->m_cs_OverlayThreadEnd[nCam], TRUE);
				pInspect->m_bOverlayThreadEnd[nCam] = true;
			}
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][9] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
			::SetEvent(pInspect->m_hThreadOverlayEnd[nCam]);
		}
		if (pInspect->m_bStopInsp)
			return 1;
		
		Sleep(1);
	}
	return 1;
}

UINT CInspect::threadScrab1(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evScrab[nCam],50) != WAIT_TIMEOUT )
		{
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->fnScrabMain(nCam);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][8] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
		}
		if (pInspect->m_bStopInsp)
			return 1;
		
		Sleep(1);
	}
	return 1;
}
UINT CInspect::threadScrab2(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1;
	while(1)
	{		
		while(WaitForSingleObject((HANDLE)pInspect->m_evScrab[nCam],50) != WAIT_TIMEOUT ){
			double dPrevTime = pInspect->GetPrecisionTime();
			pInspect->fnScrabMain(nCam);
			{
				CSingleLock lock(&pInspect->m_csInspTime[nCam], TRUE);
				pInspect->m_dInspTimes[nCam][8] = (pInspect->GetPrecisionTime() - dPrevTime) * 1000.0f;
			}
		}
		if (pInspect->m_bStopInsp)return 1;
		Sleep(1);
	}
	return 1;
}

UINT CInspect::threadGetInsulateAvgUp(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0;

	while (1)
	{
		while (WaitForSingleObject((HANDLE)pInspect->m_evGetInsulateAvg[nCam], 50) != WAIT_TIMEOUT)
		{
			pInspect->GetInsulateAvg(nCam);
			{
				CSingleLock lock(&pInspect->m_csGetInsulateAvg[nCam], TRUE);
				pInspect->m_bGetInsulateAvgThreadEnd[nCam] = true;
			}
		}

		if (pInspect->m_bStopInsp)
			return 1;

		Sleep(1);
	}

	return 1;
}

UINT CInspect::threadGetInsulateAvgDown(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1;

	while (1)
	{
		while (WaitForSingleObject((HANDLE)pInspect->m_evGetInsulateAvg[nCam], 50) != WAIT_TIMEOUT)
		{
			pInspect->GetInsulateAvg(nCam);
			{
				CSingleLock lock(&pInspect->m_csGetInsulateAvg[nCam], TRUE);
				pInspect->m_bGetInsulateAvgThreadEnd[nCam] = true;
			}
		}

		if (pInspect->m_bStopInsp)
			return 1;

		Sleep(1);
	}

	return 1;
}

UINT CInspect::threadNoncoatingAndSpurUp(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0;

	while (1)
	{
		while (WaitForSingleObject((HANDLE)pInspect->m_evNoncoatingAndSpur[nCam], 50) != WAIT_TIMEOUT)
		{
			pInspect->FindNoncoatingAndSpur(nCam);
			{
				CSingleLock lock(&pInspect->m_csNoncoatingAndSpur[nCam], TRUE);
				pInspect->m_bNoncoatingAndSpurThreadEnd[nCam] = true;
			}
		}

		if (pInspect->m_bStopInsp)
			return 1;

		Sleep(1);
	}

	return 1;
}

UINT CInspect::threadNoncoatingAndSpurDown(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1;

	while (1)
	{
		while (WaitForSingleObject((HANDLE)pInspect->m_evNoncoatingAndSpur[nCam], 50) != WAIT_TIMEOUT)
		{
			pInspect->FindNoncoatingAndSpur(nCam);
			{
				CSingleLock lock(&pInspect->m_csNoncoatingAndSpur[nCam], TRUE);
				pInspect->m_bNoncoatingAndSpurThreadEnd[nCam] = true;
			}
		}

		if (pInspect->m_bStopInsp)
			return 1;

		Sleep(1);
	}

	return 1;
}

UINT CInspect::threadIslandNgTagUp(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 0;

	while (1)
	{
		while (WaitForSingleObject((HANDLE)pInspect->m_evIslandNgTag[nCam], 100) != WAIT_TIMEOUT)
		{
			//while (!pInspect->m_bFindNoncoatingFinish[nCam])
			//{
			//}

			//pInspect->FindIsladAndNgTag(nCam);

			//{
			//	CSingleLock lock(&pInspect->m_csIslandAndNgTag[nCam], TRUE);
			//	pInspect->m_bIslandAndNgTag[nCam] = TRUE;
			//}
		}

		if (pInspect->m_bStopInsp)
			return 1;

		Sleep(1);
	}

	return 1;
}

UINT CInspect::threadIslandNgTagDown(LPVOID pParam)
{
	CInspect* pInspect = (CInspect*)pParam;
	int nCam = 1;

	while (1)
	{
		while (WaitForSingleObject((HANDLE)pInspect->m_evIslandNgTag[nCam], 100) != WAIT_TIMEOUT)
		{
			//while (!pInspect->m_bFindNoncoatingFinish[nCam])
			//{
			//}

			//pInspect->FindIsladAndNgTag(nCam);

			//{
			//	CSingleLock lock(&pInspect->m_csIslandAndNgTag[nCam], TRUE);
			//	pInspect->m_bIslandAndNgTag[nCam] = TRUE;
			//}
		}

		if (pInspect->m_bStopInsp)
			return 1;

		Sleep(1);
	}

	return 1;
}

int CInspect::Inspect(IPVM::LoggerInterface& m_loggerDebug, int nCam, BYTE* fmEven, BYTE* fmOdd, int nWidth, int nHeight, int nInsulateType, int nCellMargin, int nOffsetUp, int nOffsetDn, CIS_Site nSite, double dTabWidth, double dTabHeight, int nTabPos)
//int CInspect::Inspect(/*IPVM::LoggerInterface& m_loggerDebug, */int nCam, BYTE* fmEven, BYTE* fmOdd, int nWidth, int nHeight, int nInsulateType, int nCellMargin, int nTabPos)
{
	std::chrono::system_clock::time_point time_start = std::chrono::system_clock::now();
	m_Debuglog = &m_loggerDebug;
	m_enSite = nSite;
	m_dTabHeight[nCam] = dTabHeight;
	m_dTabWidth[nCam] = dTabWidth;//220513
//	if(m_nEdgeOffset1[nCam]<64)m_nEdgeOffset1[nCam] = 64;//220110
	m_bFindWhiteTape[nCam] = FALSE;//tape 검출치 TRUE//220321
	m_bScrabReject[nCam] = FALSE;
	m_loggerDebug.Log(0, _T("Inspect Func Begin %d , %d"), nOffsetUp, nOffsetDn);
	if (nWidth > 10000)m_nCellSize = 1;//0801	//0726
	else			m_nCellSize = 0;//0801	
	m_bInsp[nCam] = TRUE;

	ResetData(nCam);//220202
	for (int j = 0; j < 10; j++)m_dInsTime[nCam][j] = 0.;

	m_dInsTime[nCam][0] = GetPrecisionTime();
	int nret = 0;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nInsulateType = nInsulateType;//0 PVDF  1 SBR 2 Primer
	//if(m_nInsulateType == 1)
	//	m_bBottomOverayInsp[nCam] = TRUE;
	//else
	//	m_bBottomOverayInsp[nCam] = FALSE;
	m_nCellMargin = nCellMargin * 0.5;
	m_nWidthMini = nWidth / m_nReduction[nCam];
	m_nHeightMini = nHeight / m_nReduction[nCam];

	m_nOffsetUp[nCam] = nOffsetUp;
	m_nOffsetDn[nCam] = nOffsetDn;

	m_ninspCount = 0;

	//Memcopy
	m_loggerDebug.Log(0, _T("Memcopy"));
	//	m_fmEven[nCam] = fmEven;
	//	m_fmOdd[nCam] = fmOdd;
	memcpy(m_fmEven[nCam], fmEven, sizeof(BYTE) * (nWidth * nHeight));
	memcpy(m_fmOdd[nCam], fmOdd, sizeof(BYTE) * (nWidth * nHeight));
	memset(m_fmPreProcess[nCam], m_nTargetBrightness[nCam], nWidth * nHeight);//0612

	m_dInsTime[nCam][1] = GetPrecisionTime();
	int nw1 = nWidth / m_nReduction[nCam], y1 = 0, x1 = 0;
	for (int y = 0; y < nHeight && y1 < m_nHeightMini; y += m_nReduction[nCam], y1++) {
		x1 = 0;
		for (int x = 0; x < nWidth; x += m_nReduction[nCam], x1++) {
			*(m_fmCompress[nCam] + y1 * nw1 + x1) = *(fmEven + y * nWidth + x);
			*(m_fmCompressDark[nCam] + y1 * nw1 + x1) = *(fmOdd + y * nWidth + x);//211028
		}
	}

	for (int i = 0; i < nWidth; i++) {
		m_nSubProjData[nCam][0][i] = 0;
	}
	for (int y = 0; y < nHeight; y += 8)
	{
		for (int x = 0; x < nWidth; x++)
		{
			if (!m_bInspDarkRejectAtBright[nCam])
				m_nSubProjData[nCam][0][x] += *(fmOdd + y * nWidth + x);
			else
				m_nSubProjData[nCam][0][x] += *(fmEven + y * nWidth + x);
		}
	}
	double dsummid = 0;
	int nEdge1 = 0, nEdge2 = 0, nskip1 = 85, nEdgeth = 20;
	for (int x = 0; x < nWidth && nHeight>0; x++) {
		m_nSubProjData[nCam][0][x] /= (nHeight / 8);//Projection Data
		if (x > 4000 && x < 5000)dsummid += m_nSubProjData[nCam][0][x];
	}
	m_dMiddleAve[nCam] = dsummid / 1000;
	nret = FindHugeDefect(nCam, m_fmCompress[nCam], nTabPos);//tape 검사 , 에지 후보 위치, 탭 후보 위치
	if (m_nDefaultBright[nCam][0] > m_nCellSkipBrightness[nCam])//211017-1 Default 140
		nret = -1;
	else {//211017-1
		int nWhiteCellCount = 0;
		for (int n = 0; n < 3; n++) {
			if (m_dCellTopBottmAve[nCam][n] >= m_nCellSkipBrightness[nCam])//영상에 상중하 3군데중 2군데가 넘어가면 NG
				nWhiteCellCount++;
		}
		if (nWhiteCellCount > 1)
			nret = -1;
	}
	if (nret == -1)//pet일 확률이 큼
	{
		for (int i = 0; i < m_nHugeCount[nCam]; i++) {//테이프 테이프자국 거대 불량
			m_rtFullDefect[nCam][m_nFullDefectCount[nCam]] = FlipRect(m_rtSkipRange[nCam][i], nHeight);	//불량 영역
			m_fmDefect[nCam][m_nFullDefectCount[nCam]] = m_fmHugeDefect[nCam][i];//m_pSubDefet[nCam][i][j][0];//origin				//불량 이미지
			if (m_nHugeDefectBright[nCam][i] == 1)
				m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 6001;//m_nSubDefectType[nCam][i][j];				//불량 코드
			else if (m_nHugeDefectBright[nCam][i] == 3) {
				m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 2001;//핀홀로 분류			//불량 코드 //0609
			}
			else if (m_BlackTapeInsp && m_nHugeDefectBright[nCam][i] == 2) //기존은 흰색, 검은색 구분없이 다 6001로 보냄.
			{
				m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 6005;//m_nSubDefectType[nCam][i][j];				//검정색 테이프 자국
			}
			else
				m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 6001;//m_nSubDefectType[nCam][i][j];				//불량 코드
			m_strFullDefectName[nCam][m_nFullDefectCount[nCam]] = GetDefectName(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]]);//불량명
			for (int k = 0; k < 2; k++) {																			//0 백색 1흑색
				m_dFullStdValue[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubStdValue[nCam][i][j][k];			//불량 편차값
				m_dFullCompactness[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubCompactness[nCam][i][j][k];	//불량 진원도
				m_dFullAngle[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubAngle[nCam][i][j][k];				//불량 각도
				m_dFullDistance[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubDistance[nCam][i][j][k];			//흑백 간의 거리
				m_dFullPercent[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubPercent[nCam][i][j][k];			//불량 영역내  퍼센트
				m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][k] = m_dHugeDefectSizeX[nCam][i][k];				//불량 사이즈 X
				m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][k] = m_dHugeDefectSizeY[nCam][i][k];				//불량 사이즈 Y
				m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][k] = 0;// m_dPeakData[nCam][i][j][k][0];		//백색 Value [0] - OriginValue 1-Preprocessing value
				m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dPeakData[nCam][i][j][k][1];		//흑색 Value [0] - OriginValue 1-Preprocessing value
			}
			
			m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][2] = m_dHugeDefectSizeX[nCam][i][0];//0604-2
			m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][2] = m_dHugeDefectSizeY[nCam][i][0];//0604-2

			m_bDefectColor[nCam][m_nFullDefectCount[nCam]] = CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]]);
			m_dFullAngle[nCam][m_nFullDefectCount[nCam]][2] = 0;
			m_nFullDefectCount[nCam]++;
			
			if (MAX_HUGEDEFECT - 1 <= m_nFullDefectCount[nCam])break;//전체 불량 개수////220303
			if (m_bTapeInScrab[nCam] && m_nHugeDefectBright[nCam][i] == 1) {//x테이프도 스크랩으로 불량 처리한다. 테이프의 백색만 확인 한다. //220303
				m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 9001;//m_nSubDefectType[nCam][i][j];				//불량 코드
				for (int k = 0; k < 2; k++) {																			//0 백색 1흑색
					m_dFullStdValue[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubStdValue[nCam][i][j][k];			//불량 편차값
					m_dFullCompactness[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubCompactness[nCam][i][j][k];	//불량 진원도
					m_dFullAngle[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubAngle[nCam][i][j][k];				//불량 각도
					m_dFullDistance[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubDistance[nCam][i][j][k];			//흑백 간의 거리
					m_dFullPercent[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubPercent[nCam][i][j][k];			//불량 영역내  퍼센트
					m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][k] = m_dHugeDefectSizeX[nCam][i][k];				//불량 사이즈 X
					m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][k] = m_dHugeDefectSizeY[nCam][i][k];				//불량 사이즈 Y
					m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][k] = 0;// m_dPeakData[nCam][i][j][k][0];		//백색 Value [0] - OriginValue 1-Preprocessing value
					m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dPeakData[nCam][i][j][k][1];		//흑색 Value [0] - OriginValue 1-Preprocessing value
				}
				m_strFullDefectName[nCam][m_nFullDefectCount[nCam]] = GetDefectName(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]]);//불량명
				m_bDefectColor[nCam][m_nFullDefectCount[nCam]] = CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]]);
				m_nFullDefectCount[nCam]++;
				if (MAX_HUGEDEFECT - 1 <= m_nFullDefectCount[nCam])break;//전체 불량 개수////
			}
		}
		m_loggerDebug.Log(0, _T("밝기조건 검사"));//220303
		//m_nLastTimeOutCount[nCam] = 0;
		return m_nFullDefectCount[nCam];//nret;;//m_nFullDefectCount[nCam];//nret;
	}


	m_dInsTime[nCam][2] = GetPrecisionTime();
	//불량 검사
	m_loggerDebug.Log(0, _T("불량 검사"));
	int nCheck = 0, nCheck1 = 0, nLimit = m_fLimitTime[nCam] - 10.0f/*50*/, nLimit1 = m_fLimitTime[nCam] - 10.0f/*50*/;//0629 검사 Time Out 추가
	if (m_bInspManual)
	{
		nLimit = 550;
		nLimit1 = 550;
	}
	double dEdgeDetectStartTime = GetPrecisionTime();
	FindEdge1(nCam, m_fmEven[nCam], m_fmOdd[nCam]);//211104 에지찾기
	CRect rtDefect;
	bool bBumpy = false;

	if(m_bBumpyInspect[nCam])
		bBumpy = FindBumpy(nCam, m_fmEven[nCam], m_fmOdd[nCam], rtDefect);

	//FindCellEdgeIntek(nCam, m_fmEven[nCam], m_fmOdd[nCam]);
	m_loggerDebug.Log(0, _T("Find Edge Time %3.3fms"), (GetPrecisionTime() - dEdgeDetectStartTime) * 1000.0f);
	double doldtime = GetPrecisionTime();//0629 검사 Time Out 추가
	m_dInsTime[nCam][7] = doldtime;

	//여기
	bool bTimeOut = false;
	m_bIsCurCellTimeOut[nCam] = false;

	if (false == m_bUseNewLogic)
	{
		if (nret == 0)
		{
			m_bOverlayIns[nCam] = TRUE;
			m_bScrabCheck[nCam] = TRUE;

			{
				CSingleLock lock(&m_cs_OverlayThreadEnd[nCam], TRUE);
				m_bOverlayThreadEnd[nCam] = false;
			}

			{
				CSingleLock lock2(&m_cs_ScrabThreadEnd[nCam], TRUE);
				m_bScrabThreadEnd[nCam] = false;
			}

			m_evOverlay[nCam].SetEvent();
			m_evScrab[nCam].SetEvent();			//2204008 Scrab용 이벤트

			for (int i = 0; i < MOD_NUMBER; i++) 
			{
				m_evInsp[nCam][i].SetEvent();
				m_bTestFlag[nCam][i] = TRUE;
			}

			while (1)
			{
				nCheck = 0;

				for (int i = 0; i < MOD_NUMBER; i++)
				{
					if (FALSE == m_bTestFlag[nCam][i])
						nCheck++;
				}

				{
					bool bOverlayThreadEnd = false;
					bool bScrabThreadEnd = false;

					{
						CSingleLock lock(&m_cs_OverlayThreadEnd[nCam], TRUE);
						bOverlayThreadEnd = m_bOverlayThreadEnd[nCam];
					}

					{
						CSingleLock lock2(&m_cs_ScrabThreadEnd[nCam], TRUE);
						bScrabThreadEnd = m_bScrabThreadEnd[nCam];
					}

					//if (nCheck >= MOD_NUMBER && true == bOverlayThreadEnd && true == bScrabThreadEnd)
					if (nCheck >= MOD_NUMBER && true == bOverlayThreadEnd && true == bScrabThreadEnd)
					{
						if(m_nPreprocessEdgeBreakStep[nCam] < 0)
							m_loggerDebug.Log(0, _T("불량 검사 OK - [%d] [%d] [%3.3fms]"), nCheck, nCheck1, (GetPrecisionTime() - m_dInsTime[nCam][2]) * 1000.0f);
						else
							m_loggerDebug.Log(0, _T("불량 검사 OK, PreprocessEdge 타임아웃 - [%d] [%d] [%3.3fms]"), nCheck, nCheck1, (GetPrecisionTime() - m_dInsTime[nCam][2]) * 1000.0f);

						break;
					}

					float fTime = (GetPrecisionTime() - doldtime) * 1000;
					if (fTime >= nLimit)
					{
						m_loggerDebug.Log(0, _T("타임아웃으로 inspect(%.3f) 함수를 빠져나감 Scrab(%s), Overlay(%s) [%3.3fms]"), fTime, (true == bScrabThreadEnd) ? _T("time in") : _T("time out"), (true == bOverlayThreadEnd) ? _T("time in") : _T("time out"), (GetPrecisionTime() - m_dInsTime[nCam][2]) * 1000.0f);
						break;
					}
				}

				nCheck1++;
				//Sleep(1);
				if ((GetPrecisionTime() - doldtime) * 1000 >= nLimit1)//0629 검사 Time Out 추가
				{
					double dtime11 = GetPrecisionTime();
					m_bThreadStop[nCam] = TRUE;

					bTimeOut = true;

					m_loggerDebug.Log(0, _T("불량 검사 Time Out - [%d][%d][%d][%d] [%3.3fms]"), m_bOverlayIns[nCam], m_bScrabCheck[nCam], nCheck, nCheck1, (dtime11 - m_dInsTime[nCam][2]) * 1000.0f);//0406
					break;
				}
			}

			double dEndTime = GetPrecisionTime();
			m_loggerDebug.Log(0, _T("Only Inspect Time %3.3fms"), (dEndTime - doldtime) * 1000.0f);

			{
				CSingleLock lock(&m_csInspTime[nCam], TRUE);
				for (int i = 0; i < m_dInspTimes[nCam].size(); i++)
				{
					if (i < MOD_NUMBER)
						m_loggerDebug.Log(0, _T("Inspection Thread %d Insp Time - %3.6f"), i, m_dInspTimes[nCam][i]);
					else if (i == MOD_NUMBER)
						m_loggerDebug.Log(0, _T("Scrab Inspection Thread Insp Time - %3.6f"), m_dInspTimes[nCam][i]);
					else if (i > MOD_NUMBER)
						m_loggerDebug.Log(0, _T("Overlay Inspection Thread Insp Time - %3.6f"), m_dInspTimes[nCam][i]);
				}
				m_dInspTimes[nCam].clear();
				m_dInspTimes[nCam].resize(MOD_NUMBER + 2);
			}
		}
		else {
			m_nIslandCount[nCam] = m_nInsulateDefectCount[nCam] = m_nIslandCount[nCam] = 0;
			if (m_bTapeSkip[nCam])//220303 true시 오버레이 탭 미코팅 검사를 한다. 스크랩 검사가 안된 부분은 아래에서 다시 추가해야함.
				PreprocessEdge(nCam);
			else {}
			for (int i = 0; i < MOD_NUMBER; i++)
			{
				m_nSubDefectCount[nCam][i] = 0;
			}
		}
	}
	else
	{
		if (nret == 0)
		{
			PreprocessEdgeNewLogic(nCam);

			m_bScrabCheck[nCam] = TRUE;
			m_bGetInsulateAvgThreadEnd[nCam] = false;
			m_bNoncoatingAndSpurThreadEnd[nCam] = false;
			//m_bIslandAndNgTag[nCam] = false;
			//m_bFindNoncoatingFinish[nCam] = false;

			//{
			//	CSingleLock lock(&m_csRightSum[nCam], TRUE);
			//	m_vecRightSum[nCam].clear();
			//}
			//{
			//	CSingleLock lock(&m_csRectEdge[nCam], TRUE);
			//	m_vecRectEdge[nCam].clear();
			//}
			//{
			//	CSingleLock lock(&m_csNoncoatEdge[nCam], TRUE);
			//	m_vecNoncoatEdge[nCam].clear();
			//}
			//{
			//	CSingleLock lock(&m_csCoatEdge[nCam], TRUE);
			//	m_vecCoatEdge[nCam].clear();
			//}
			//{
			//	CSingleLock lock(&m_cs_nt[nCam], TRUE);
			//	m_vec_nt[nCam].clear();
			//}
			//{
			//	CSingleLock lock(&m_csTabPos[nCam], TRUE);
			//	m_vecTabPos[nCam].clear();
			//}
			//{
			//	CSingleLock lock(&m_csSumEdge[nCam], TRUE);
			//	m_vecSumEdge[nCam].clear();
			//}
			//{
			//	CSingleLock lock(&m_csOffset1[nCam], TRUE);
			//	m_vecOffset1[nCam].clear();
			//}
			//{
			//	CSingleLock lock(&m_csRectTabIns[nCam], TRUE);
			//	m_vecRectTabIns[nCam].clear();
			//}
			//{
			//	CSingleLock lock(&m_csRectSpur[nCam], TRUE);
			//	m_vecRectSpur[nCam].clear();
			//}
			//{
			//	CSingleLock lock(&m_csNoncoatingDefect[nCam], TRUE);
			//	m_vecNoncoatingDefect[nCam].clear();
			//}

			m_evScrab[nCam].SetEvent();			//2204008 Scrab용 이벤트
			m_evGetInsulateAvg[nCam].SetEvent();
			m_evNoncoatingAndSpur[nCam].SetEvent();
			//m_evIslandNgTag[nCam].SetEvent();

			for (int i = 0; i < MOD_NUMBER; i++) {
				m_evInsp[nCam][i].SetEvent();
				m_bTestFlag[nCam][i] = TRUE;
			}

			while (1)
			{
				nCheck = 0;

				for (int i = 0; i < MOD_NUMBER; i++)
				{
					if (FALSE == m_bTestFlag[nCam][i])
						nCheck++;
				}

				bool bScrabThreadEnd = false;
				{
					CSingleLock lock(&m_cs_ScrabThreadEnd[nCam], TRUE);
					bScrabThreadEnd = m_bScrabThreadEnd[nCam];

				}

				bool bGetInsulateAvgThreadEnd;
				{
					CSingleLock lock(&m_csGetInsulateAvg[nCam], TRUE);
					bGetInsulateAvgThreadEnd = m_bGetInsulateAvgThreadEnd[nCam];
				}

				bool bNoncoatingAndSpurThreadEnd;
				{
					CSingleLock lock(&m_csNoncoatingAndSpur[nCam], TRUE);
					bNoncoatingAndSpurThreadEnd = m_bNoncoatingAndSpurThreadEnd[nCam];
				}

				//bool bIslandAndNgTag;
				//{
				//	CSingleLock lock(&m_csIslandAndNgTag[nCam], TRUE);
				//	bIslandAndNgTag = m_bIslandAndNgTag[nCam];
				//}

				if (nCheck >= MOD_NUMBER && true == bScrabThreadEnd && true == bGetInsulateAvgThreadEnd && true == bNoncoatingAndSpurThreadEnd/* && true == bIslandAndNgTag*/)
				{
					m_loggerDebug.Log(0, _T("불량 검사 OK - [%d] [%d] [%3.3fms]"), nCheck, nCheck1, (GetPrecisionTime() - m_dInsTime[nCam][2]) * 1000.0f);
					break;
				}

				float fTime = (GetPrecisionTime() - doldtime) * 1000;

				if (fTime >= nLimit)
				{
					m_loggerDebug.Log(0, _T("타임아웃으로 inspect(%.3f) 함수를 빠져나감 Scrab(%s) GetInsulate(%s) NoncoatingAndSpur(%s) [%3.3fms]"), fTime, (true == bScrabThreadEnd) ? _T("time in") : _T("time out"), (true == bGetInsulateAvgThreadEnd) ? _T("time in") : _T("time out"), (true == bNoncoatingAndSpurThreadEnd) ? _T("time in") : _T("time out"), (GetPrecisionTime() - m_dInsTime[nCam][2]) * 1000.0f);
					break;
				}

				nCheck1++;
				//Sleep(1);
				if ((GetPrecisionTime() - doldtime) * 1000 >= nLimit1)//0629 검사 Time Out 추가
				{
					double dtime11 = GetPrecisionTime();
					m_bThreadStop[nCam] = TRUE;

					bTimeOut = true;

					m_loggerDebug.Log(0, _T("불량 검사 Time Out - [%d][%d][%d][%d] [%3.3fms]"), m_bOverlayIns[nCam], m_bScrabCheck[nCam], nCheck, nCheck1, (dtime11 - m_dInsTime[nCam][2]) * 1000.0f);//0406
					break;
				}
			}

			double dEndTime = GetPrecisionTime();
			m_loggerDebug.Log(0, _T("Only Inspect Time %3.3fms"), (dEndTime - doldtime) * 1000.0f);

			{
				CSingleLock lock(&m_csRightSum[nCam], TRUE);
				m_vecRightSum[nCam].clear();
			}
			{
				CSingleLock lock(&m_csRectEdge[nCam], TRUE);
				m_vecRectEdge[nCam].clear();
			}
			{
				CSingleLock lock(&m_csNoncoatEdge[nCam], TRUE);
				m_vecNoncoatEdge[nCam].clear();
			}
			{
				CSingleLock lock(&m_csCoatEdge[nCam], TRUE);
				m_vecCoatEdge[nCam].clear();
			}
			{
				CSingleLock lock(&m_cs_nt[nCam], TRUE);
				m_vec_nt[nCam].clear();
			}
			{
				CSingleLock lock(&m_csTabPos[nCam], TRUE);
				m_vecTabPos[nCam].clear();
			}
			{
				CSingleLock lock(&m_csSumEdge[nCam], TRUE);
				m_vecSumEdge[nCam].clear();
			}
			{
				CSingleLock lock(&m_csOffset1[nCam], TRUE);
				m_vecOffset1[nCam].clear();
			}
			{
				CSingleLock lock(&m_csRectTabIns[nCam], TRUE);
				m_vecRectTabIns[nCam].clear();
			}
			{
				CSingleLock lock(&m_csRectSpur[nCam], TRUE);
				m_vecRectSpur[nCam].clear();
			}
			{
				CSingleLock lock(&m_csNoncoatingDefect[nCam], TRUE);
				m_vecNoncoatingDefect[nCam].clear();
			}

		}
		else
		{
			m_nIslandCount[nCam] = m_nInsulateDefectCount[nCam] = m_nIslandCount[nCam] = 0;
			if (m_bTapeSkip[nCam])//220303 true시 오버레이 탭 미코팅 검사를 한다. 스크랩 검사가 안된 부분은 아래에서 다시 추가해야함.
			{
				PreprocessEdge(nCam);
			}
			else
			{
			}
			for (int i = 0; i < MOD_NUMBER; i++) {
				m_nSubDefectCount[nCam][i] = 0;
			}

			{
				CSingleLock lock(&m_csRightSum[nCam], TRUE);
				m_vecRightSum[nCam].clear();
			}
			{
				CSingleLock lock(&m_csRectEdge[nCam], TRUE);
				m_vecRectEdge[nCam].clear();
			}
			{
				CSingleLock lock(&m_csNoncoatEdge[nCam], TRUE);
				m_vecNoncoatEdge[nCam].clear();
			}
			{
				CSingleLock lock(&m_csCoatEdge[nCam], TRUE);
				m_vecCoatEdge[nCam].clear();
			}
			{
				CSingleLock lock(&m_cs_nt[nCam], TRUE);
				m_vec_nt[nCam].clear();
			}
			{
				CSingleLock lock(&m_csTabPos[nCam], TRUE);
				m_vecTabPos[nCam].clear();
			}
			{
				CSingleLock lock(&m_csSumEdge[nCam], TRUE);
				m_vecSumEdge[nCam].clear();
			}
			{
				CSingleLock lock(&m_csOffset1[nCam], TRUE);
				m_vecOffset1[nCam].clear();
			}
			{
				CSingleLock lock(&m_csRectTabIns[nCam], TRUE);
				m_vecRectTabIns[nCam].clear();
			}
			{
				CSingleLock lock(&m_csRectSpur[nCam], TRUE);
				m_vecRectSpur[nCam].clear();
			}
			{
				CSingleLock lock(&m_csNoncoatingDefect[nCam], TRUE);
				m_vecNoncoatingDefect[nCam].clear();
			}
		}
	}

	if (bBumpy)
	{
		if (true == m_bCoatingWrinkleSizeAndOr)
		{
			if (rtDefect.Width() * m_dscale_x1[nCam] > m_dCoatWrinkleSizeX[nCam] && rtDefect.Height() * m_dscale_y1[nCam] > m_dCoatWrinkleSizeY[nCam])
			{
				if (m_nFullDefectCount[nCam] < MAX_FULL_DEFECT)
				{
					m_nFullDefectCount[nCam]++;

					m_rtFullDefect[nCam][m_nFullDefectCount[nCam] - 1] = rtDefect;
					m_rtFullDefect[nCam][m_nFullDefectCount[nCam] - 1].top = m_nHeight - rtDefect.bottom;
					m_rtFullDefect[nCam][m_nFullDefectCount[nCam] - 1].bottom = m_nHeight - rtDefect.top;
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam] - 1] = 1006; //코팅주름
					m_strFullDefectName[nCam][m_nFullDefectCount[nCam] - 1] = GetDefectName(1006); //코팅주름

					m_dFullStdValue[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullStdValue[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullCompactness[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullCompactness[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullDistance[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullDistance[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullPercent[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullPercent[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;

					m_dFullAngle[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullAngle[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;

					m_dFullSizeX[nCam][m_nFullDefectCount[nCam] - 1][0] = rtDefect.Width() * m_dscale_x1[nCam];
					m_dFullSizeX[nCam][m_nFullDefectCount[nCam] - 1][1] = rtDefect.Width() * m_dscale_x1[nCam];
					m_dFullSizeX[nCam][m_nFullDefectCount[nCam] - 1][2] = rtDefect.Width() * m_dscale_x1[nCam];

					m_dFullSizeY[nCam][m_nFullDefectCount[nCam] - 1][0] = rtDefect.Height() * m_dscale_y1[nCam];
					m_dFullSizeY[nCam][m_nFullDefectCount[nCam] - 1][1] = rtDefect.Height() * m_dscale_y1[nCam];
					m_dFullSizeY[nCam][m_nFullDefectCount[nCam] - 1][2] = rtDefect.Height() * m_dscale_y1[nCam];
				}
			}
		}
		else
		{
			if (rtDefect.Width() * m_dscale_x1[nCam] > m_dCoatWrinkleSizeX[nCam] || rtDefect.Height() * m_dscale_y1[nCam] > m_dCoatWrinkleSizeY[nCam])
			{
				if (m_nFullDefectCount[nCam] < MAX_FULL_DEFECT)
				{
					m_nFullDefectCount[nCam]++;

					m_rtFullDefect[nCam][m_nFullDefectCount[nCam] - 1] = rtDefect;
					m_rtFullDefect[nCam][m_nFullDefectCount[nCam] - 1].top = m_nHeight - rtDefect.bottom;
					m_rtFullDefect[nCam][m_nFullDefectCount[nCam] - 1].bottom = m_nHeight - rtDefect.top;
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam] - 1] = 1006; //코팅주름
					m_strFullDefectName[nCam][m_nFullDefectCount[nCam] - 1] = GetDefectName(1006); //코팅주름

					m_dFullStdValue[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullStdValue[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullCompactness[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullCompactness[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullDistance[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullDistance[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullPercent[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullPercent[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;

					m_dFullAngle[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullAngle[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;

					m_dFullSizeX[nCam][m_nFullDefectCount[nCam] - 1][0] = rtDefect.Width() * m_dscale_x1[nCam];
					m_dFullSizeX[nCam][m_nFullDefectCount[nCam] - 1][1] = rtDefect.Width() * m_dscale_x1[nCam];
					m_dFullSizeX[nCam][m_nFullDefectCount[nCam] - 1][2] = rtDefect.Width() * m_dscale_x1[nCam];

					m_dFullSizeY[nCam][m_nFullDefectCount[nCam] - 1][0] = rtDefect.Height() * m_dscale_y1[nCam];
					m_dFullSizeY[nCam][m_nFullDefectCount[nCam] - 1][1] = rtDefect.Height() * m_dscale_y1[nCam];
					m_dFullSizeY[nCam][m_nFullDefectCount[nCam] - 1][2] = rtDefect.Height() * m_dscale_y1[nCam];
				}
			}
		}
	}

	DWORD dTimeOut = ::WaitForSingleObject(m_hThreadOverlayEnd[nCam], nLimit);

	if (dTimeOut == WAIT_TIMEOUT)
	{
		m_loggerDebug.Log(0, _T("!!!!!!!Time Out Process Wait Time Over!!!!!!!"));
	}

	for (int i = 0; i < 20; i++)
	{
		m_loggerDebug.Log(0, _T("Inspection Sub Time %d - %3.3f"), i, m_dInsSubTime1[nCam][i]);
	}

	//for (int i = 0; i < 100; i++)
	//{
	//	if(m_dNewinspTime[nCam][i] > 0)
	//		m_loggerDebug.Log(0, _T("New Insp Time Check Time %d - %3.3f"), i, m_dNewinspTime[nCam][i]);
	//}

	CString strNoTab;

	if (m_bNoTab)
		strNoTab = _T("TRUE");
	else
		strNoTab = _T("FALSE");

	m_loggerDebug.Log(0, _T("NO Tab %s"), strNoTab);

	if(m_nPreprocessEdgeBreakStep[nCam] >= 0)
		m_loggerDebug.Log(0, _T("PreprocessEdge TimeOut %s %d %3.3f"), GetPreprocessEdgeBreakStepCode(m_nPreprocessEdgeBreakStep[nCam]), m_nPreprocessEdgeBreakStep[nCam], m_dLastTime[nCam]);

	m_bOverlayIns[nCam]=FALSE;
	for(int i=0;i<MOD_NUMBER;i++)m_bTestFlag[nCam][i] = FALSE;

	m_dInsTime[nCam][3]= GetPrecisionTime();

	CRect rttab;
	rttab.SetRect(m_rtMinTab[nCam].left*m_nReduction[nCam], m_rtMinTab[nCam].top*m_nReduction[nCam], m_rtMinTab[nCam].right*m_nReduction[nCam], m_rtMinTab[nCam].bottom*m_nReduction[nCam]);
//	PreprocessEdge( nCam);
	//불량 정리 //1412
	m_loggerDebug.Log(0, _T("불량 검사 / 1412"));
	m_dInsTime[nCam][4] = GetPrecisionTime();

	for (int i = 0; i < m_nHugeCount[nCam] && i<MAX_HUGEDEFECT; i++) {//테이프 테이프자국 거대 불량//220303
		m_rtFullDefect[nCam][m_nFullDefectCount[nCam]] = FlipRect(m_rtSkipRange[nCam][i], nHeight);	//불량 영역
		m_fmDefect[nCam][m_nFullDefectCount[nCam]] = m_fmHugeDefect[nCam][i];//m_pSubDefet[nCam][i][j][0];//origin				//불량 이미지
		if (m_nHugeDefectBright[nCam][i] == 1)
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 6001;//m_nSubDefectType[nCam][i][j];				//불량 코드
		else if (m_nHugeDefectBright[nCam][i] == 3){
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 2001;//핀홀로 분류			//불량 코드 //0609
		}
		else if (m_BlackTapeInsp && m_nHugeDefectBright[nCam][i] == 2)
		{
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 6005;//m_nSubDefectType[nCam][i][j];				//검정색 테이프 자국
		}
		else
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 6001;//m_nSubDefectType[nCam][i][j];				//불량 코드
		for (int k = 0; k<2; k++){																			//0 백색 1흑색
			m_dFullStdValue[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubStdValue[nCam][i][j][k];			//불량 편차값
			m_dFullCompactness[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubCompactness[nCam][i][j][k];	//불량 진원도
			m_dFullAngle[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubAngle[nCam][i][j][k];				//불량 각도
			m_dFullDistance[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubDistance[nCam][i][j][k];			//흑백 간의 거리
			m_dFullPercent[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubPercent[nCam][i][j][k];			//불량 영역내  퍼센트
			m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][k] = m_dHugeDefectSizeX[nCam][i][k];				//불량 사이즈 X
			m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][k] = m_dHugeDefectSizeY[nCam][i][k];				//불량 사이즈 Y
			m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][k] = 0;// m_dPeakData[nCam][i][j][k][0];		//백색 Value [0] - OriginValue 1-Preprocessing value
			m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dPeakData[nCam][i][j][k][1];		//흑색 Value [0] - OriginValue 1-Preprocessing value
		}
		///////////////
		//사이즈를 보고 테이프와 거대 라인을 구분한다.
		BOOL bLargeLine = FALSE;//거대 라인 조건이 맞을경우 라인불량으로 코드를 전송한다. //
		if(bLargeLine){
			if(m_dHugeDefectSizeY[nCam][i][0]> m_nHeight*m_dscale_y1[nCam] * 0.7 && m_dHugeDefectSizeX[nCam][i][0] < 35 ||
				m_dHugeDefectSizeY[nCam][i][1]> m_nHeight*m_dscale_y1[nCam] * 0.7 && m_dHugeDefectSizeX[nCam][i][1] < 35){//가로가 35mm 보다 작고 영상의 70%이상의 Y일경우 거대라인
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 1001;//핀홀로 분류			//불량 코드 //0609
			}
		}
		m_strFullDefectName[nCam][m_nFullDefectCount[nCam]] = GetDefectName(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]]);//불량명
		
		if(m_nHugeDefectBright[nCam][i] == 1)
		{
			m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][2] = m_dHugeDefectSizeX[nCam][i][0];//0604-2
			m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][2] = m_dHugeDefectSizeY[nCam][i][0];//0604-2
		}
		else
		{
			m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][2] = m_dHugeDefectSizeX[nCam][i][1];//0604-2
			m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][2] = m_dHugeDefectSizeY[nCam][i][1];//0604-2
		}
		m_bDefectColor[nCam][m_nFullDefectCount[nCam]] = CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]]);
		m_dFullAngle[nCam][m_nFullDefectCount[nCam]][2] = 0;

		m_nFullDefectCount[nCam]++;
		if (MAX_HUGEDEFECT-1 <= m_nFullDefectCount[nCam])break;//전체 불량 개수////220303
		//scrab을 추가해준다.0
		if(m_bTapeInScrab[nCam] && m_nHugeDefectBright[nCam][i] == 1){//x테이프도 스크랩으로 불량 처리한다. 테이프의 백색만 확인 한다. //220303
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 9001;//m_nSubDefectType[nCam][i][j];				//불량 코드
			for (int k = 0; k<2; k++)
			{																			//0 백색 1흑색
				m_dFullStdValue[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubStdValue[nCam][i][j][k];			//불량 편차값
				m_dFullCompactness[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubCompactness[nCam][i][j][k];	//불량 진원도
				m_dFullAngle[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubAngle[nCam][i][j][k];				//불량 각도
				m_dFullDistance[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubDistance[nCam][i][j][k];			//흑백 간의 거리
				m_dFullPercent[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dSubPercent[nCam][i][j][k];			//불량 영역내  퍼센트
				m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][k] = m_dHugeDefectSizeX[nCam][i][k];				//불량 사이즈 X
				m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][k] = m_dHugeDefectSizeY[nCam][i][k];				//불량 사이즈 Y
				m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][k] = 0;// m_dPeakData[nCam][i][j][k][0];		//백색 Value [0] - OriginValue 1-Preprocessing value
				m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam]][k] = 0;//m_dPeakData[nCam][i][j][k][1];		//흑색 Value [0] - OriginValue 1-Preprocessing value
			}
			m_strFullDefectName[nCam][m_nFullDefectCount[nCam]] = GetDefectName(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]]);//불량명
			m_bDefectColor[nCam][m_nFullDefectCount[nCam]] = CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]]);
			m_nFullDefectCount[nCam]++;
			if (MAX_HUGEDEFECT-1 <= m_nFullDefectCount[nCam])break;//전체 불량 개수////220303
		}
	}
	m_loggerDebug.Log(0, _T(" for 5 "));
	CRect rtOld;
	int nSurfaceRealCnt = m_nFullDefectCount[nCam];
	
	for (int i = 0; i< MOD_NUMBER; i++)
	{
		//표면 불량
		for (int j = 0; j < m_nSubDefectCount[nCam][i] && j< MAX_SPLIT_DEFECT; j++)
		{
			int nNotchjump = 0;//노치 불량 제외
			for (int k = 0; k<4; k++)
			{
				CPoint pt1; pt1 = m_ptFullNotch[nCam][k];//m_ptSubDefectPoint[nCam][i][j];
				if (m_rtSubDefect[nCam][i][j].PtInRect(pt1)>0)
				{
					if (k<2 && m_ptSubDefectPoint[nCam][i][j].x<pt1.x + 5)
						nNotchjump = 1;
					else if (k >= 2 && m_ptSubDefectPoint[nCam][i][j].x>pt1.x - 5)
						nNotchjump = 1;
				}
				if (nNotchjump == 1)break;
			}
			if (nNotchjump == 1)
				continue;

			if (MAX_FULL_DEFECT-1 <= m_nFullDefectCount[nCam])
				break;//전체 불량 개수//220222
			if (MAX_FULL_DEFECT-1 <= m_nScrabCount[nCam])
				break;//전체 불량 개수//220222

			m_rtFullDefect[nCam][m_nFullDefectCount[nCam]] = FlipRect(m_rtSubDefect[nCam][i][j], nHeight);	//불량 영역

			m_fmDefect[nCam][m_nFullDefectCount[nCam]] = m_pSubDefet[nCam][i][j][0];//origin				//불량 이미지
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = m_nSubDefectType[nCam][i][j];				//불량 코드
			m_strFullDefectName[nCam][m_nFullDefectCount[nCam]] = GetDefectName(m_nSubDefectType[nCam][i][j]);//불량명

			m_rtDetailRawDefect[nCam][m_nFullDefectCount[nCam]] = m_rtSubRawDefect[nCam][i][j][CheckColor( m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] )][0];
			m_dDetailFullArea[nCam][m_nFullDefectCount[nCam]] = m_dDefectAreaSub[nCam][i][j][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]])][0];
			m_dDetailFullWidth[nCam][m_nFullDefectCount[nCam]] = m_dDefectWidthSub[nCam][i][j][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]])][1];
			m_dDetailFullLength[nCam][m_nFullDefectCount[nCam]] = m_dDefectLengthSub[nCam][i][j][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]])][1];
			
			for (int k = 0; k<2; k++)
			{																			
				//0 백색 1흑색
				m_dFullStdValue[nCam][m_nFullDefectCount[nCam]][k] = m_dSubStdValue[nCam][i][j][k];			//불량 편차값
				m_dFullCompactness[nCam][m_nFullDefectCount[nCam]][k] = m_dSubCompactness[nCam][i][j][k];	//불량 진원도
				m_dFullAngle[nCam][m_nFullDefectCount[nCam]][k] = m_dSubAngle[nCam][i][j][k];				//불량 각도
				m_dFullDistance[nCam][m_nFullDefectCount[nCam]][k] = m_dSubDistance[nCam][i][j][k];			//흑백 간의 거리
				m_dFullPercent[nCam][m_nFullDefectCount[nCam]][k] = m_dSubPercent[nCam][i][j][k];			//불량 영역내  퍼센트
				m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][k] = m_dSubSizeX[nCam][i][j][k];				//불량 사이즈 X
				m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][k] = m_dSubSizeY[nCam][i][j][k];				//불량 사이즈 Y
				//211023
				m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][k] = m_dPeakData[nCam][i][j][k][0] - m_dPeakData[nCam][i][j][k][2];		//백색 Value [0] - OriginValue 1-Preprocessing value
				m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam]][k] = m_dPeakData[nCam][i][j][k][2] - m_dPeakData[nCam][i][j][k][1];		//흑색 Value [0] - OriginValue 1-Preprocessing value
			}

			if(m_bDefectRoiOverlap[nCam]){
				CPoint pt1;
				pt1 = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]].CenterPoint();
				for(int k=0;k<m_nFullDefectCount[nCam];k++){
					if(m_nFullDefectType[nCam][k]<=0)
						continue;
					CRect rt11 = m_rtFullDefect[nCam][k];
					rt11.InflateRect(BAD_WIDTH*0.5,BAD_HEIGHT*0.5);// Crop Size 1.5 배 이내는 Skip 한다.
					if( rt11.PtInRect(pt1)){
						nNotchjump = 1;
						break;
					}
				}
				if (nNotchjump == 1 && m_bDefect2Scrab[nCam]){//불량 오버랩 //220325
					if(m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE){	// 220126 - sjj
						if(m_bScrabSizeAndOr){//and //220804
							if( m_dSubSizeX[nCam][i][j][0] >= m_dScrabSpecSizeX[nCam] && m_dSubSizeY[nCam][i][j][0] >= m_dScrabSpecSizeY[nCam] )
							{
								if (m_bDefectToScrabWithThreshold[nCam] == TRUE)
								{
									if (m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][0] > m_nScrabTh[nCam]) 
									{//220512
										m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
										m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
										m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeX[nCam][i][j][0];			//0white 1black
										m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeY[nCam][i][j][0];			//0white 1black
										m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
										m_nScrabCount[nCam]++;
									}
								}
								else
								{
									m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
									m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
									m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeX[nCam][i][j][0];			//0white 1black
									m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeY[nCam][i][j][0];			//0white 1black
									m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
									m_nScrabCount[nCam]++;
								}
							}
						}
						else{
							//if( m_dSubSizeX[nCam][i][j][0] >= m_dSpecScarabSize[nCam] || m_dSubSizeY[nCam][i][j][0] >= m_dSpecScarabSize[nCam] )
							if (m_dSubSizeX[nCam][i][j][0] >= m_dScrabSpecSizeX[nCam] || m_dSubSizeY[nCam][i][j][0] >= m_dScrabSpecSizeY[nCam])
							{
								if (m_bDefectToScrabWithThreshold[nCam] == TRUE)
								{
									if (m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][0] > m_nScrabTh[nCam])
									{//220512
										m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
										m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
										m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeX[nCam][i][j][0];			//0white 1black
										m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeY[nCam][i][j][0];			//0white 1black
										m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
										m_nScrabCount[nCam]++;
									}
								}
								else
								{
									m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
									m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
									m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeX[nCam][i][j][0];			//0white 1black
									m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeY[nCam][i][j][0];			//0white 1black
									m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
									m_nScrabCount[nCam]++;
								}
							}
						}
					}
					else if(m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
					{
						CPoint pt22;
						pt22 = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]].CenterPoint();
						if(m_rtScrabIns[nCam].PtInRect(pt22)){
							if(m_bScrabSizeAndOr){//and //220804
								if( m_dSubSizeX[nCam][i][j][0] >= m_dScrabSpecSizeX[nCam] && m_dSubSizeY[nCam][i][j][0] >= m_dScrabSpecSizeY[nCam] )
								{
									if (m_bDefectToScrabWithThreshold[nCam] == TRUE)
									{
										if (m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][0] > m_nScrabTh[nCam]) 
										{//220512
											m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
											m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
											m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeX[nCam][i][j][0];			//0white 1black
											m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeY[nCam][i][j][0];			//0white 1black
											m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
											m_nScrabCount[nCam]++;
										}
									}
									else
									{
										m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
										m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
										m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeX[nCam][i][j][0];			//0white 1black
										m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeY[nCam][i][j][0];			//0white 1black
										m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
										m_nScrabCount[nCam]++;
									}
								}	
							}
							else{
								//if( m_dSubSizeX[nCam][i][j][0] >= m_dSpecScarabSize[nCam] || m_dSubSizeY[nCam][i][j][0] >= m_dSpecScarabSize[nCam] )
								if (m_dSubSizeX[nCam][i][j][0] >= m_dScrabSpecSizeX[nCam] || m_dSubSizeY[nCam][i][j][0] >= m_dScrabSpecSizeY[nCam])
								{
									if (m_bDefectToScrabWithThreshold[nCam] == TRUE)
									{
										if (m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][0] > m_nScrabTh[nCam])
										{//220512
											m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
											m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
											m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeX[nCam][i][j][0];			//0white 1black
											m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeY[nCam][i][j][0];			//0white 1black
											m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
											m_nScrabCount[nCam]++;
										}
									}
									else
									{
										m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
										m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
										m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeX[nCam][i][j][0];			//0white 1black
										m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dSubSizeY[nCam][i][j][0];			//0white 1black
										m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
										m_nScrabCount[nCam]++;
									}
								}	
							}
						}
					}


					if (MAX_FULL_DEFECT-1 <= m_nFullDefectCount[nCam])break;//전체 불량 개수//220222
					if (MAX_FULL_DEFECT-1 <= m_nScrabCount[nCam])break;//전체 불량 개수//220222
				}
			}

			m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]])];//;m_dHugeDefectSizeX[nCam][i][0];//0604-2
			m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]])];//;m_dHugeDefectSizeY[nCam][i][0];//0604-2
			
			m_bDefectColor[nCam][m_nFullDefectCount[nCam]] = CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]]);
			m_dFullAngle[nCam][m_nFullDefectCount[nCam]][2] = m_dFullAngle[nCam][m_nFullDefectCount[nCam]][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam]])];

			//scrab
			if(m_bDefect2Scrab[nCam]){//220325 
				if(m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE){	// 220126 - sjj
					if(m_bScrabSizeAndOr){//and //220804
						if( m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0] >= m_dScrabSpecSizeX[nCam] && m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0] >= m_dScrabSpecSizeY[nCam] )
						{
							if (MAX_FULL_DEFECT-1 <= m_nScrabCount[nCam])break;//전체 불량 개수//220222

							if (m_bDefectToScrabWithThreshold[nCam] == TRUE)
							{
								//밝기 비교
								if (m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][0] > m_nScrabTh[nCam]) 
								{//220512
									m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
									m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
									m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
									m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
									m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
									m_nScrabCount[nCam]++;
								}
							}
							else
							{
								m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
								m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
								m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
								m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
								m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
								m_nScrabCount[nCam]++;
							}
						}
					}
					else{//or
						//if( m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0] >= m_dSpecScarabSize[nCam] || m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0] >= m_dSpecScarabSize[nCam] )
						if (m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0] >= m_dScrabSpecSizeX[nCam] || m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0] >= m_dScrabSpecSizeY[nCam])
						{
							if (MAX_FULL_DEFECT-1 <= m_nScrabCount[nCam])break;//전체 불량 개수//220222

							if (m_bDefectToScrabWithThreshold[nCam] == TRUE)
							{
								//밝기 비교
								if (m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][0] > m_nScrabTh[nCam])
								{//220512
									m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
									m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
									m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
									m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
									m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
									m_nScrabCount[nCam]++;
								}
							}
							else
							{
								m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
								m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
								m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
								m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
								m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
								m_nScrabCount[nCam]++;
							}
						}
					}
				}
				else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
				{
					if (MAX_FULL_DEFECT-1 <= m_nScrabCount[nCam])break;//전체 불량 개수//220222
					CPoint pt22;
					pt22 = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]].CenterPoint();
					if(m_rtScrabIns[nCam].PtInRect(pt22)){
						if(m_bScrabSizeAndOr){//and //220804
							if( m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0] >= m_dScrabSpecSizeX[nCam] && m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0] >= m_dScrabSpecSizeY[nCam] )
							{
								if (m_bDefectToScrabWithThreshold[nCam] == TRUE)
								{
									if (m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][0] > m_nScrabTh[nCam])
									{//220512
										m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
										m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
										m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
										m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
										m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
										m_nScrabCount[nCam]++;
									}
								}
								else
								{
									m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
									m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
									m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
									m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
									m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
									m_nScrabCount[nCam]++;
								}
							}
						}
						else{
							//if( m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0] >= m_dSpecScarabSize[nCam] || m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0] >= m_dSpecScarabSize[nCam] )
							if (m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0] >= m_dScrabSpecSizeX[nCam] || m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0] >= m_dScrabSpecSizeY[nCam])
							{
								if (m_bDefectToScrabWithThreshold[nCam] == TRUE)
								{
									if (m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam]][0] > m_nScrabTh[nCam]) 
									{//220512
										m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
										m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
										m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
										m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
										m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
										m_nScrabCount[nCam]++;
									}
								}
								else
								{
									m_rtFullDefectScrab[nCam][m_nScrabCount[nCam]] = m_rtFullDefect[nCam][m_nFullDefectCount[nCam]];//불량 영역
									m_nFullDefectTypeScrab[nCam][m_nScrabCount[nCam]] = 9001;//defect Code
									m_dFullSizeXScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
									m_dFullSizeYScrab[nCam][m_nScrabCount[nCam]][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][0];			//0white 1black//220113
									m_strFullDefectNameScrab[nCam][m_nScrabCount[nCam]] = "Scrab";
									m_nScrabCount[nCam]++;
								}
							}
						}
					}
				}
			}

			//			if(m_nSubDefectType[nCam][i][j]>0)//211106 실제 불량만 카운트 한다.
			nSurfaceRealCnt++;
			m_nFullDefectCount[nCam]++;
			if (MAX_FULL_DEFECT -1<= m_nFullDefectCount[nCam])break;//전체 불량 개수//220222
			if (MAX_FULL_DEFECT -1<= m_nScrabCount[nCam])break;//전체 불량 개수//220222
		}
	}
	m_loggerDebug.Log(0, _T(" for 6 "));
	int nDefec1 = 0;
	for (int i = 0; i < m_nIslandCount[nCam] && i<MAX_HUGEDEFECT; i++) {//아일랜드 ngtag //220303
		if (MAX_FULL_DEFECT-1 <= m_nFullDefectCount[nCam] + nDefec1 )break;//220222

		m_rtFullDefect[nCam][m_nFullDefectCount[nCam] + nDefec1] = FlipRect(m_rtIsland[nCam][i], nHeight);	//불량 영역

		if (m_bDefectRoiOverlap[nCam] && m_dISSize[nCam][i] != 9001 ){
			int nNotchjump = 0;
			CPoint pt1;
			pt1 = m_rtFullDefect[nCam][m_nFullDefectCount[nCam] + nDefec1].CenterPoint();
			for(int k=0;k<m_nFullDefectCount[nCam]+nDefec1;k++){
				if(m_nFullDefectType[nCam][k]<=0)
					continue;
				CRect rt11 = m_rtFullDefect[nCam][k];
				rt11.InflateRect(BAD_WIDTH*0.5,BAD_HEIGHT*0.5);// Crop Size 1.5 배 이내는 Skip 한다.
				if( rt11.PtInRect(pt1)){
					nNotchjump = 1;
					break;
				}
			}
			if (nNotchjump == 1)
				continue;
		}
		m_fmDefect[nCam][m_nFullDefectCount[nCam] + nDefec1] = m_pIsland[nCam][i];//origin				//불량 이미지
		for (int k = 0; k < 2; k++) {																			//0 백색 1흑색
			m_dFullStdValue[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;			//불량 편차값
			m_dFullCompactness[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;	//불량 진원도
			m_dFullAngle[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;				//불량 각도
			m_dFullDistance[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;			//흑백 간의 거리
			m_dFullPercent[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;			//불량 영역내  퍼센트
			m_dFullSizeX[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = m_dISSizeX[nCam][i];				//불량 사이즈 X
			m_dFullSizeY[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = m_dISSizeY[nCam][i];				//불량 사이즈 Y
			m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;		//백색 Value [0] - OriginValue 1-Preprocessing value
			m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;//m_dPeakData[nCam][i][j][k][1];		//흑색 Value [0] - OriginValue 1-Preprocessing value
		}
		m_dFullCompactness[nCam][m_nFullDefectCount[nCam] + nDefec1][1] = m_dISCompactness[nCam][i];//211023
		m_dFullPercent[nCam][m_nFullDefectCount[nCam] + nDefec1][1] = m_dIslnadPercent[nCam][i];//211023
		if (m_dISSize[nCam][i] == -1000) {//burr
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 8001;
		}
		else if (m_dISSize[nCam][i] == 6004 && m_bNgTag)//0625 //211028
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 6004;//6004;//211028
		else if (m_dISSize[nCam][i] == 6004 )//0625 //211028
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 6004;//6004;//211028
		else if (m_dISSize[nCam][i] == 4001 ) {//미코팅 4001 //211104
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 4001;
		}
		else if (m_dISSize[nCam][i] == 1001 ) {//미코팅 4001 //211104
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 1001;
		}
		else if (m_dISSize[nCam][i] == 1002 ) {//라인 추가 필요 //220627
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 1002;
		}
		else if (m_dISSize[nCam][i] == 2001 ) {//미코팅 4001 //211104
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 2001;
		}
		else if (m_dISSize[nCam][i] == 3003 ) {//크랙
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 7002;//220426
		}
		else if (m_dISSize[nCam][i] == 3003 + 5) {//spur에서 검출된 크랙 230109 SJM
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 3003;//220426
		}
		else if (m_dISSize[nCam][i] == 9001)//스크랩 //0930
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 9001;//0930
		else if (m_dISSize[nCam][i] == 5002)//스크랩 //0930
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 5002;//zipper
		else if (m_dISSize[nCam][i] == 1006)//코팅주름 //221116
			m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 1006;//zipper
		else {
			if(m_dISCompactness[nCam][i]*100 > m_dIslandCompactTh[nCam] /*&& m_dIslnadPercent[nCam][i] > m_dIslandPercentTh[nCam]*/){//211023  면적 진원도
				//211103
				double dislandRatio1 = m_dISMaxDist[nCam][i]/m_dISMinDist[nCam][i];
				double dislandRatio2 = m_dISMinDist[nCam][i]/m_dISMaxDist[nCam][i];
				double dislandRatio3 = dislandRatio1;
				if(dislandRatio3 < dislandRatio2)
					dislandRatio3 = dislandRatio2;
				if(m_dISMaxDist[nCam][i] > 30 && m_dISMinDist[nCam][i]>30){//탭 구겨짐일 확일이 큼
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 0;
					continue;					
				}
				if (m_bIslandSizeAndOr) {//220503 Island 종회비 파라미터 추가
					if (m_dISSizeX[nCam][i] > m_dIslandSizeX[nCam] && m_dISSizeY[nCam][i] > m_dIslandSizeY[nCam]) {//m_dwi
						m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 5001;
						if (m_bWrinkleSizeAndOr && m_dISSizeY[nCam][i] / m_dISSizeX[nCam][i] >= m_dIslandRatio[nCam])//Y가 긴조건
							m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 5002;
						if (!m_bWrinkleSizeAndOr && (m_dISSizeX[nCam][i] > m_dITabWrinkleMinSize[nCam] && m_dISSizeY[nCam][i] > m_dITabWrinkleMinSize[nCam]) && m_dISSizeX[nCam][i] > m_dIslandSizeX[nCam] && m_dISSizeY[nCam][i] / m_dISSizeX[nCam][i] >= m_dIslandRatio[nCam])//Y가 긴조건
							m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 5002;//211013 최소 사이즈
					}
					else {//0611
						if (m_bWrinkleSizeAndOr && m_dISSizeX[nCam][i] > m_dIslandSizeX[nCam] && (m_dISSizeX[nCam][i] / m_dISSizeY[nCam][i]) >= m_dIslandRatio[nCam] && m_dISSizeY[nCam][i] > 1) {
							m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 5002;
						}
						else
							m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 0;
					}
				}
				else {//211013 최소 사이즈
					if ((m_dISSizeX[nCam][i] > m_dIslandMinSize[nCam] && m_dISSizeY[nCam][i] > m_dIslandMinSize[nCam]) && (m_dISSizeX[nCam][i] > m_dIslandSizeX[nCam] || m_dISSizeY[nCam][i] > m_dIslandSizeY[nCam])) {
						m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 5001;
						if (m_bWrinkleSizeAndOr && m_dISSizeY[nCam][i] / m_dISSizeX[nCam][i] >= m_dIslandRatio[nCam])
							m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 5002;
						if (!m_bWrinkleSizeAndOr && (m_dISSizeX[nCam][i] > m_dITabWrinkleMinSize[nCam] && m_dISSizeY[nCam][i] > m_dITabWrinkleMinSize[nCam]) && m_dISSizeX[nCam][i] > m_dIslandSizeX[nCam] && m_dISSizeY[nCam][i] / m_dISSizeX[nCam][i] >= m_dIslandRatio[nCam])
							m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 5002;
					}
					else {//0611
						if (!m_bWrinkleSizeAndOr && (m_dISSizeX[nCam][i] > m_dITabWrinkleMinSize[nCam] && m_dISSizeY[nCam][i] > m_dITabWrinkleMinSize[nCam]) && m_dISSizeX[nCam][i] > m_dIslandSizeX[nCam] && (m_dISSizeX[nCam][i] / m_dISSizeY[nCam][i]) >= m_dIslandRatio[nCam] && m_dISSizeY[nCam][i] > 0.5) {
							m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 5002;//211013 최소 사이즈
						}
						else
							m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 0;
					}
				}
			}
			else
				m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 0;
		}
		m_dFullSizeX[nCam][m_nFullDefectCount[nCam] + nDefec1][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam] + nDefec1][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1])];//;m_dHugeDefectSizeX[nCam][i][0];//0604-2
		m_dFullSizeY[nCam][m_nFullDefectCount[nCam] + nDefec1][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam] + nDefec1][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1])];//;m_dHugeDefectSizeY[nCam][i][0];//0604-2
		m_dFullAngle[nCam][m_nFullDefectCount[nCam] + nDefec1][2] = m_dFullAngle[nCam][m_nFullDefectCount[nCam] + nDefec1][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1])];
		m_bDefectColor[nCam][m_nFullDefectCount[nCam] + nDefec1] = CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1]);
		m_strFullDefectName[nCam][m_nFullDefectCount[nCam] + nDefec1] = GetDefectName(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1]);//불량명
		nDefec1++;
	}
	//scrab 추가 //220411
	for (int i = 0; i < m_nScrabRangDefect[nCam] && i<MAX_HUGEDEFECT; i++) {//스크랩 220411 //기존 Scrab은 기존 함수에서 사이즈를 걸러낸다.
		if (MAX_FULL_DEFECT-1 <= m_nFullDefectCount[nCam] + nDefec1 )break;//220222
		m_rtFullDefect[nCam][m_nFullDefectCount[nCam] + nDefec1] = FlipRect(m_rtScrabDefect[nCam][i], nHeight);	//불량 영역
		m_fmDefect[nCam][m_nFullDefectCount[nCam] + nDefec1] = m_pScrabDefect[nCam][i];//origin				//불량 이미지
		for (int k = 0; k < 2; k++) {																			//0 백색 1흑색
			m_dFullStdValue[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;			//불량 편차값
			m_dFullCompactness[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;	//불량 진원도
			m_dFullAngle[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;				//불량 각도
			m_dFullDistance[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;			//흑백 간의 거리
			m_dFullPercent[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;			//불량 영역내  퍼센트
			m_dFullSizeX[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = m_dScrabDefectSizeX[nCam][i];				//불량 사이즈 X
			m_dFullSizeY[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = m_dScrabDefectSizeY[nCam][i];				//불량 사이즈 Y
			m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;		//백색 Value [0] - OriginValue 1-Preprocessing value
			m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam] + nDefec1][k] = 0;//m_dPeakData[nCam][i][j][k][1];		//흑색 Value [0] - OriginValue 1-Preprocessing value
		}
		m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1] = 9001;//0930

		m_dFullSizeX[nCam][m_nFullDefectCount[nCam] + nDefec1][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam] + nDefec1][0];
		m_dFullSizeY[nCam][m_nFullDefectCount[nCam] + nDefec1][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam] + nDefec1][0];
		m_dFullAngle[nCam][m_nFullDefectCount[nCam] + nDefec1][2] = m_dFullAngle[nCam][m_nFullDefectCount[nCam] + nDefec1][0];
		m_bDefectColor[nCam][m_nFullDefectCount[nCam] + nDefec1] = CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1]);
		m_strFullDefectName[nCam][m_nFullDefectCount[nCam] + nDefec1] = GetDefectName(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec1]);//불량명
		nDefec1++;
	}

	//m_loggerDebug.Log.Log(0, _T(" for 7 "));
	int nDefec2 = 0;
	double dminsize1 = 0.1;
	for (int i = 0; i<m_nInsulateDefectCount[nCam] && i<MAX_HUGEDEFECT; i++){//절연 핀홀 절연 라인//2220303
		if (MAX_FULL_DEFECT-1 <= m_nFullDefectCount[nCam] + nDefec1 + nDefec2 )break;
		m_rtFullDefect[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = FlipRect(m_rtInsulateDefect[nCam][i], nHeight);	//불량 영역

		if (m_bDefectRoiOverlap[nCam]){
			int nNotchjump = 0;
			CPoint pt1;
			pt1 = m_rtFullDefect[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1].CenterPoint();
			for(int k=0;k<m_nFullDefectCount[nCam]+nDefec1+nDefec2;k++){
				if(m_nFullDefectType[nCam][k]<=0)
					continue;
				CRect rt11 = m_rtFullDefect[nCam][k];
				rt11.InflateRect(BAD_WIDTH*0.5,BAD_HEIGHT*0.5);// Crop Size 1.5 배 이내는 Skip 한다.
				if( rt11.PtInRect(pt1)){
					nNotchjump = 1;
					break;
				}
			}
			if (nNotchjump == 1)
				continue;
		}
//		m_rtFullDefect[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = FlipRect(m_rtInsulateDefect[nCam][i], nHeight);	//불량 영역

		m_fmDefect[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = m_pInsulDefect[nCam][i];//origin				//불량 이미지

		for (int k = 0; k<2; k++){																			//0 백색 1흑색
			m_dFullStdValue[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][k] = 0;			//불량 편차값
			m_dFullCompactness[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][k] = m_dInsulCompaceness[nCam][i] ;	//불량 진원도
			m_dFullAngle[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][k] = m_dInsulMinMax[nCam][i][k];				//불량 각도
			m_dFullDistance[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][k] = 0;			//흑백 간의 거리
			m_dFullPercent[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][k] = 0;			//불량 영역내  퍼센트
			m_dFullSizeX[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][k] = m_dInsulSizeX[nCam][i];				//불량 사이즈 X
			m_dFullSizeY[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][k] = m_dInsulSizeY[nCam][i];				//불량 사이즈 Y
			m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][k] = 0;		//백색 Value [0] - OriginValue 1-Preprocessing value
			m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][k] = 0;//m_dPeakData[nCam][i][j][k][1];		//흑색 Value [0] - OriginValue 1-Preprocessing value
		}
		if(m_bInsulateDarkInsp[nCam] &&  m_dInsulSize[nCam][i] == -10){//Dark 불량 
			double dSizeX = 0.0f, dSizeY = 0.0f;
			int nType;
			if (m_enSite == CIS_Site::ESWA)
			{
				dSizeX = m_dInsulDefectSizeX[nCam];
				dSizeY = m_dInsulDefectSizeY[nCam];
				nType = 1001; //라인으로 배출
			}
			else
			{
				dSizeX = m_dInsulDarkLineX[nCam];
				dSizeY = m_dInsulDarkLineY[nCam];
				nType = 0; //기존에 OK로 배출하여서 그대로 적용
			}
			if ( m_bInsulDarkLineSizeAndOr && m_dInsulSizeX[nCam][i]> dSizeX && m_dInsulSizeY[nCam][i]> dSizeY){//and 조건
				//if (m_dInsulSizeX[nCam][i]>m_dInsulatePinholeX[nCam] && m_dInsulSizeY[nCam][i]> m_dInsulatePinholeY[nCam] && m_dInsulSizeY[nCam][i] / m_dInsulSizeX[nCam][i]<4){
				//	m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 7001;
				//}
				//else //우선 라인만 검출되게
				if (/*m_dInsulSizeY[nCam][i] / m_dInsulSizeX[nCam][i]>m_dIslandRatio[nCam] &&*/m_dInsulCompact[nCam] < m_dInsulCompaceness[nCam][i] * 100)//여기도 아일랜드 종회비 따라가게 수정 //220503
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 5001;//7002;//절연 아일랜드로 검출
				else
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = nType;
			}
			else if ( !m_bInsulDarkLineSizeAndOr && (m_dInsulSizeX[nCam][i]> dSizeX || m_dInsulSizeY[nCam][i]> dSizeY)){//and 조건
				//if (m_dInsulSizeX[nCam][i]>m_dInsulatePinholeX[nCam] && m_dInsulSizeY[nCam][i]> m_dInsulatePinholeY[nCam] && m_dInsulSizeY[nCam][i] / m_dInsulSizeX[nCam][i]<4){
				//	m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 7001;
				//}
				//else //우선 라인만 검출되게
				if (/*m_dInsulSizeY[nCam][i] / m_dInsulSizeX[nCam][i]>m_dIslandRatio[nCam] && */m_dInsulSizeX[nCam][i] > dminsize1 && m_dInsulCompact[nCam] < m_dInsulCompaceness[nCam][i]*100)//여기도 아일랜드 종회비 따라가게 수정 //220503
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 5001;//7002;
				else
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = nType;
			}
			else
				m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 0;
		}
		else{//백색 절연 불량
			double dSizeX = 0.0f, dSizeY = 0.0f, dSizeX_Line = 0.0f, dSizeY_Line = 0.0f;
			if (m_enSite == CIS_Site::ESWA)
			{
				dSizeX = dSizeX_Line = m_dInsulDefectSizeX[nCam];
				dSizeY = dSizeY_Line = m_dInsulDefectSizeY[nCam];
			}
			else
			{
				dSizeX = m_dInsulatePinholeX[nCam];
				dSizeY = m_dInsulatePinholeY[nCam];
				dSizeX_Line = m_dInsulateLineX[nCam];
				dSizeY_Line = m_dInsulateLineY[nCam];
			}
			if(m_dInsulSize[nCam][i] == -10)
				m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 0;
			else if( m_dInsulSize[nCam][i] ==7002){//221201  gap을 라인으로 표기
				m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 7002;
			}
			else{
				if(m_bInsulWhitePinholeSizeAndOr && m_dInsulSizeX[nCam][i] > dSizeX && m_dInsulSizeY[nCam][i] > dSizeY){//and 조건 - 절연 핀홀
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 7001;
					if(m_bInsulWhiteLineSizeAndOr && m_dInsulSizeX[nCam][i]> dSizeX_Line && m_dInsulSizeY[nCam][i]> dSizeY_Line && m_dInsulSizeY[nCam][i] / m_dInsulSizeX[nCam][i]>m_dIslandRatio[nCam]){//and 조건 - 절연 라인 //여기도 아일랜드 종회비 따라가게 수정 //220503
						m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 7002;
					}
					else if(!m_bInsulWhiteLineSizeAndOr && m_dInsulSizeY[nCam][i] / m_dInsulSizeX[nCam][i]>m_dIslandRatio[nCam] && (m_dInsulSizeX[nCam][i]>dminsize1 && m_dInsulSizeY[nCam][i]> m_dInsulateLineY[nCam] ) ){//or 조건 - 절연 라인//여기도 아일랜드 종회비 따라가게 수정 //220503
						m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 7002;
					}
				}
				else if(!m_bInsulWhitePinholeSizeAndOr && m_dInsulSizeX[nCam][i] > dminsize1&& m_dInsulSizeY[nCam][i]>dminsize1 && (m_dInsulSizeX[nCam][i] > dSizeX || m_dInsulSizeY[nCam][i] > dSizeX) ){//or 조건
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 7001;
					if(m_bInsulWhiteLineSizeAndOr && m_dInsulSizeX[nCam][i]> dSizeX_Line && m_dInsulSizeY[nCam][i]> dSizeX_Line&& m_dInsulSizeY[nCam][i] / m_dInsulSizeX[nCam][i]>m_dIslandRatio[nCam]){//and 조건 - 절연 라인//여기도 아일랜드 종회비 따라가게 수정 //220503
						m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 7002;
					}
					else if(!m_bInsulWhiteLineSizeAndOr && m_dInsulSizeY[nCam][i] / m_dInsulSizeX[nCam][i]>m_dIslandRatio[nCam] && (m_dInsulSizeX[nCam][i]>dminsize1 && m_dInsulSizeY[nCam][i]> dSizeY_Line) ){//or 조건 - 절연 라인//여기도 아일랜드 종회비 따라가게 수정 //220503
						m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 7002;
					}
				}
// 				if (m_dInsulSizeX[nCam][i]>m_dInsulateLineX[nCam] && m_dInsulSizeY[nCam][i]> m_dInsulateLineY[nCam]){//0517-4
// 					if (m_dInsulSizeX[nCam][i]>m_dInsulatePinholeX[nCam] && m_dInsulSizeY[nCam][i]> m_dInsulatePinholeY[nCam] && m_dInsulSizeY[nCam][i] / m_dInsulSizeX[nCam][i]<4){
// 						m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 7001;
// 					}
// 					else if (m_dInsulSizeY[nCam][i] / m_dInsulSizeX[nCam][i]>m_dXYRatio[nCam])
// 						m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 7002;
// 					else
// 						m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 0;
// 				}
				else
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = 0;
			}
		}
		//0611
		m_dFullSizeX[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][2] = m_dFullSizeX[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1])];//;m_dHugeDefectSizeX[nCam][i][0];//0604-2
		m_dFullSizeY[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][2] = m_dFullSizeY[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1])];//;m_dHugeDefectSizeY[nCam][i][0];//0604-2
		m_dFullAngle[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][2] = m_dFullAngle[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1][CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1])];

		m_bDefectColor[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = CheckColor(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1]);
		m_strFullDefectName[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1] = GetDefectName(m_nFullDefectType[nCam][m_nFullDefectCount[nCam] + nDefec2 + nDefec1]);//불량명
		nDefec2++;
		if (MAX_FULL_DEFECT-1 <= m_nFullDefectCount[nCam] + nDefec1 + nDefec2 )break;
	}
	
	//m_loggerDebug.Log(0, _T(" 정리 "));
//Scrab만 정리
	
	m_nFullDefectCount[nCam] += (nDefec1 + nDefec2);
	if(m_nScrabCount[nCam]>0 && m_bInsScrab[nCam] == TRUE){//220112	// 220126 sjj -  수정.. 
		if(m_nFullDefectCount[nCam]+m_nScrabCount[nCam] >= MAX_FULL_DEFECT-1){
//스크랩 갯수만큼 뒤로 빼서 스크랩을 추가한다.
			int n1 = MAX_FULL_DEFECT-1 - m_nScrabCount[nCam];
			if(n1<0)n1 = 0;//220222
			for(int i=0;i<m_nScrabCount[nCam];i++){
				if (MAX_FULL_DEFECT-1 <= n1)//220303
					break;
				m_rtFullDefect[nCam][n1] = m_rtFullDefectScrab[nCam][i];
				m_nFullDefectType[nCam][n1] = 9001;//m_nFullDefectTypeScrab[nCam][i];
				m_dFullSizeX[nCam][n1][2] = m_dFullSizeXScrab[nCam][i][2];
				m_dFullSizeY[nCam][n1][2] = m_dFullSizeYScrab[nCam][i][2];
				m_strFullDefectName[nCam][n1] = "Scrab";
				n1++;
				if (MAX_FULL_DEFECT-1 <= n1)
					break;
			}
			m_nFullDefectCount[nCam] = n1;//+= n1;// m_nScrabCount[nCam]; //220222 불량 갯수
		}
		else{
			int n1 = m_nFullDefectCount[nCam];
			if(n1<0)n1 = 0;//220222
			if (MAX_FULL_DEFECT-1 > n1)
			{
				for(int i=0;i<m_nScrabCount[nCam];i++)
				{
					m_rtFullDefect[nCam][m_nFullDefectCount[nCam]] = m_rtFullDefectScrab[nCam][i];
					m_nFullDefectType[nCam][m_nFullDefectCount[nCam]] = 9001;//m_nFullDefectTypeScrab[nCam][i];
					m_dFullSizeX[nCam][m_nFullDefectCount[nCam]][2] = m_dFullSizeXScrab[nCam][i][2];
					m_dFullSizeY[nCam][m_nFullDefectCount[nCam]][2] = m_dFullSizeYScrab[nCam][i][2];
					m_strFullDefectName[nCam][m_nFullDefectCount[nCam]] = "Scrab";
					m_nFullDefectCount[nCam]++;
					if (MAX_FULL_DEFECT-1 <= m_nFullDefectCount[nCam])
						break;			
				}
			}
		}
	}
//rect reverse 
	 //for(int i=0;i<4;i++){
		//m_rtNotchArea[nCam][i] = FlipRect(m_rtNotchArea[nCam][i], nHeight);
	 //}

	if(m_nFullDefectCount[nCam] >= MAX_FULL_DEFECT-1) //220222
		m_nFullDefectCount[nCam] = MAX_FULL_DEFECT-1;
	m_rtCellTab[nCam];//탭 영역
	m_dMiddleAve[nCam];//코팅 중앙 밝기값
	m_ptFullNotch[nCam][0];//3까지 노치 4point
	m_rtCell[nCam];//cell rect
	//검사영역 상하 줄이기
	m_rtCell[nCam].top = m_nCellMargin;
	m_rtCell[nCam].bottom -= m_nCellMargin;

	m_rtInsulate[nCam];
	m_ptNotch[nCam];//Notch Point 좌상 기준점
	m_rtOverlay[nCam];//Overlay Rect
	m_dInsTime[nCam][5] = GetPrecisionTime();
	m_bInsp[nCam] = FALSE;
	m_dInsulWidth[nCam];//insulate width
	m_dOverlayWidth[nCam];//overlay width
	m_dTabBrightness[nCam][0];//0720 tab bright ave //0809
	m_dTabBrightness[nCam][1];//0720 tab dark ave	//0809

	m_dBottomInsul[nCam][0];//0mm 1pixel //0801 //bottom insulate
	m_dBottomCoat[nCam][1];//0mm 1pixel
	m_dBottomOverlay[nCam][0];//0mm 1pixel//0801 //bottom overlay

	auto nWaitTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time_start);
	
	//m_nLastTimeOutCount[nCam]++;


	if (true == m_bUseTimeOut)
	{
		if (m_fLimitTime[nCam] < nWaitTime.count()/*이함수 시작과 끝까지 60ms 이상이거나*/ || true == bTimeOut/*G사에서 제공한 타임아웃 시점*/)
		{
			m_ntimeoutCount[nCam]++;
			m_bIsCurCellTimeOut[nCam] = true;
		}
		else
		{
			//이번 셀에서 타임아웃에 걸리지 않으면 누적 카운트는 0으로 변경하기
			m_ntimeoutCount[nCam] = 0;
		}

		if (m_nTimeOutAlarmcount[nCam] < m_ntimeoutCount[nCam])
		{
			//여기서 알람 처리 하기 
			m_nFullDefectCount[nCam] = -9999;

			//다시 초기화
			m_ntimeoutCount[nCam] = 0;
		}
	}
	
	auto camID = CString(nCam == 0 ? _T("Up") : _T("Down"));

	CString text;
	text.Format(_T("%s time %s : 설정 %.2fms, 이번 검사 소요시간 %dms, 누적카운트 %d, G-사 time out(%d)"), camID, (m_fLimitTime[nCam] < nWaitTime.count()) ? _T("out") : _T("in"), m_fLimitTime[nCam], nWaitTime.count(), m_ntimeoutCount[nCam], bTimeOut);
	m_loggerDebug.Log(0, text);


	m_loggerDebug.Log(0, _T("Inspect Time(0-1) : %3.3f"), (m_dInsTime[nCam][1] - m_dInsTime[nCam][0])*1000.0f);
	m_loggerDebug.Log(0, _T("Inspect Time(0-2) : %3.3f"), (m_dInsTime[nCam][2] - m_dInsTime[nCam][0])*1000.0f);
	m_loggerDebug.Log(0, _T("Inspect Time(0-3) : %3.3f"), (m_dInsTime[nCam][3] - m_dInsTime[nCam][0])*1000.0f);
	m_loggerDebug.Log(0, _T("Inspect Time(0-4) : %3.3f"), (m_dInsTime[nCam][4] - m_dInsTime[nCam][0])*1000.0f);
	m_loggerDebug.Log(0, _T("Inspect Time(0-5) : %3.3f"), (m_dInsTime[nCam][5] - m_dInsTime[nCam][0])*1000.0f);
	m_loggerDebug.Log(0, _T("Inspect Func End"));
	
	return m_nFullDefectCount[nCam];//nret;
}

int CInspect::InspectMod(int nCam,int nIdx,int nMod)
{
	m_dInsSubTime2[nCam][nIdx][0] = GetPrecisionTime();//0706
	int nw =m_nWidth;
	int nh = m_nHeight;
	int nstt = 0;
	int nend = nh;
	if( nIdx==0)
		nstt=0,nend = nh/nMod;
	else 
		nstt = (nh/nMod)*(nIdx),nend=(nh/nMod)*(nIdx+1); 

	int nxmove = 2, nymove=2;//후보군 올리는 픽셀 거리 //211017
	//if (m_nCellSize == 1 && m_nAnodeCathode==0)
	if (true == m_bLaserAndCathode[nCam])
	{
		nxmove = 4; nymove = 2;
	}
	m_nSubDefectCount[nCam][nIdx] = 0;
	for(int i=0;i<MAX_SPLIT_DEFECT;i++){m_rtSubInspect[nCam][nIdx][i].SetRect(0,0,0,0);m_rtSubDefect[nCam][nIdx][i].SetRect(0,0,0,0);}
	if(nIdx<0)nIdx = 0;
	int nindex = nIdx;
	int nh1 = nh/nMod;
	if(nh1<0 || nw<1||nend-nstt==0)return 0;
	int x1=0,x2=nw;
	int ok=0;
	int left,top,right,bottom,nYCount,nProfile,nMulti,nYSkip;
	nYCount = 20;nProfile =2;nMulti=1;nYSkip=20;
	left = 0;top = 0;right = nw;bottom = nh;
	int nValue = m_nTargetBrightness[nCam];//128
	int pitch = nw;
	int nTmp = 0;
	int nOffsetx = 12,nOffsetx11=2;//크게 옵셋을두고 후보를 찾는다.
	double dNullAve=0,dCoatAve=0,dTotalAve=0,dWhiteAve=0,dBlackAve=0;
	int	nNullEdgeValue = 0,nNullEdgeCnt=0;
	BOOL bTesting = TRUE;// FALSE;//찍힘 검사용 테스트 모드 
	int  nPoj1[20000] = { 0, };//0929
	int nwhite1 = m_dMiddleAve[nCam]*2;//220630
	if(nwhite1>160)nwhite1 = 160;
	if (nindex == 0)//0929
	{
		for (int i = 0; i<nw; i++){
			nPoj1[i] = m_nSubProjData[nCam][0][i];
		}
	}

	if (!bTesting)
	{
		for (int i = 0; i<nw; i++)
		{
			m_nSubProjData[nCam][nIdx][i] = m_nPreProcessData[nCam][nIdx][i] = m_nPreProcessData1[nCam][nIdx][i] = 0;
		}
		for (int y = nstt; y < nend; y += 4)
		{
			for (int x = x1; x < x2; x++)
			{
				if (!m_bInspDarkRejectAtBright[nCam])
				{
					if (*(m_fmOdd[nCam] + y * nw + x) > nwhite1)
						m_nSubProjData[nCam][nindex][x] += m_dMiddleAve[nCam];
					else
						m_nSubProjData[nCam][nindex][x] += *(m_fmOdd[nCam] + y * nw + x);
				}
				else
				{
					if (*(m_fmEven[nCam] + y * nw + x) > nwhite1)
						m_nSubProjData[nCam][nindex][x] += m_dMiddleAve[nCam];
					else
						m_nSubProjData[nCam][nindex][x] += *(m_fmEven[nCam] + y * nw + x);
				}
			}
		}
		for(int x=x1;x<x2 && nh1>0;x++)
		{
		 	m_nSubProjData[nCam][nindex][x] /= (nh1/4);//Projection Data
		 	dTotalAve += m_nSubProjData[nCam][nindex][x];
		}
		if(x2-x1>0)//220523
			dTotalAve /= (x2-x1);//평균 밝기
	}
	else{
		for (int i = 0; i<nw; i++){
			m_nPreProcessData[nCam][nIdx][i] = m_nPreProcessData1[nCam][nIdx][i] = 0;
		}
		if (nindex>0)
			for (int x = x1; x < x2; x++)
				m_nSubProjData[nCam][nindex][x] = m_nSubProjData[nCam][0][x];
	}

	BOOL bCoatNonCoat[40] = { 0, }, bTmp1[40] = { 0, };
	//m_dsubAveragValue
	//Edge 찾기
	m_nLeftSkip = 1000;m_nRightSkip=8000;
//	int nCam1Start= m_nLeftSkip,nCam1End = m_nRightSkip,nCam2start = 1400,nCam2End=nw-800;//		x1 = 1400;x2 = CAM_LINE_WIDTH_8192-800;
	m_nMaxImageValue[nCam][nindex] = dNullAve;
	m_nMinImageValue[nCam][nindex] = dCoatAve;
	int nMax = m_nEdgeTh[nCam], nmaxpos = 0, nRoiCount = 0, nwhite = 0, njump = 0, njump1 = 0, nedgeblackcnt = 0, nedgeblackcnt1 = 0, nFirstEdge = 0, nCoatNoncoatcnt[2][40];
	BOOL	bNullingFind = FALSE;
	for(int i=0;i<40;i++)nCoatNoncoatcnt[0][i]=nCoatNoncoatcnt[1][i]=0;
	int nNullStart = 0;
	int nThMax = nMax;
	if(m_bAutoEdgeTh  &&nThMax > m_dEdgeSumValue[nCam][nindex])
		nThMax =m_dEdgeSumValue[nCam][nindex];
//에지를 먼저 찾을것.
	//@220220 에지 정렬
	CRect rtins;
	int nSkip1 = m_nEdgeOffset1[nCam];//21
	int nxoffset1 = 4; //0813
	//if (m_nCellSize == 1)	
	//	nxoffset1 = 100;
	if(TRUE == m_bLaserAndCathode[nCam])
		nxoffset1 = 100;

	nSkip1 = m_nEdgeOffset1[nCam] * 1.0;
	nSkip1 += nxoffset1;
	x1 = m_rtCell[nCam].left +nSkip1;
	x2 = m_rtCell[nCam].right - nSkip1;		//BUG: 수정하세요
	rtins.SetRect(x1,nstt,x2,nend);
	m_rtInpectionArea[nindex] = rtins; //20220916 SJM


// 	if(x2< m_rtCell[nCam].right-4)
// 		x2 = m_rtCell[nCam].right - 4;
	if (x1 <= m_nMarginSearchStart[nCam])
		x1 = m_nMarginSearchStart[nCam] + 100;//211017-1
	if (x2 >= nw - 100)
		x2 = m_nMarginSearchStart[nCam] - 110;//211017-1

	double dmag = m_dMagValue[nCam], dmag1 = m_dMagValue[nCam] * 0.5, dmag2 = 0, dLineMag = m_nLineMag[nCam];
	int nHighCut = m_nHighCut[nCam], nLowCut = m_nLowCut[nCam], nLineHighCut = m_nLineHighCut[nCam], nXcount1 = 0;
//	double dXprojData[4000]={0.,};
//	dXprojData = (double*)malloc(nh*sizeof(double));
	m_dInsSubTime2[nCam][nIdx][1] = GetPrecisionTime();//0706

	int noffy1 = BAD_HEIGHT*0.5, noffy2 = BAD_HEIGHT*0.5, nycnt1 = 0;//211004
	if (nIdx == 0)noffy1 = 0;//211004
	else if (nIdx == MOD_NUMBER-1)noffy2 = 0;//211004

	//IPVM::Image_8u_C1 preProcessBefore;
	//preProcessBefore.Create(nw, nh);
	//memcpy(preProcessBefore.GetMem(), m_fmPreProcess[nCam], nw * nh);
	//preProcessBefore.SaveBmp(_T("Preprocess_Before.bmp"));

	for (int y = nstt - noffy1; y < nend + noffy2; y++)//검사영역보다 더 크게 전처리를 한다. //211004 //옛날 전처리 코드 20230424
	{
		nXcount1 = 0;
		for(int x=x1-10;x<x2+10 && x1>10 && x2<nw-10;x++)
		{
			if (!m_bInspDarkRejectAtBright[nCam])
			{
				if ((*(m_fmOdd[nCam] + pitch * y + x) - m_nSubProjData[nCam][nindex][x]) > nHighCut || (*(m_fmOdd[nCam] + pitch * y + x) - m_nSubProjData[nCam][nindex][x]) < -nLowCut)
					dmag2 = dmag;
				else
					dmag2 = dmag1;
			}
			else
			{
				if ((*(m_fmEven[nCam] + pitch * y + x) - m_nSubProjData[nCam][nindex][x]) > nHighCut || (*(m_fmEven[nCam] + pitch * y + x) - m_nSubProjData[nCam][nindex][x]) < -nLowCut)
					dmag2 = dmag;
				else
					dmag2 = dmag1;
			}
//			nTmp= int(nValue+ (*(m_fmEven[nCam]+pitch*y+x)-m_nSubProjData[nCam][nindex][x])*dmag2) & 255;//*(nImageProjection+i);
			if (!m_bInspDarkRejectAtBright[nCam])
				nTmp = (nValue + (*(m_fmOdd[nCam] + pitch * y + x) - m_nSubProjData[nCam][nindex][x]) * dmag2);//*(nImageProjection+i);
			else
				nTmp = (nValue + (*(m_fmEven[nCam] + pitch * y + x) - m_nSubProjData[nCam][nindex][x]) * dmag2);//*(nImageProjection+i);

			if(nTmp<0)nTmp =0;if(nTmp>255)nTmp = 255;

			if (m_nSubProjData[nCam][nindex][x]>200)
				nTmp = 255;//0618-2

			m_nPreProcessData[nCam][nindex][x] += nTmp;//전처리 평균 data 이물
			*(m_fmPreProcess[nCam]+pitch*y+x) = nTmp;//y 전처리 이미지 이물 전처리
			nXcount1++;
//			dXprojData[y] += nTmp;
//  			if(x+4<nw-40){
//  				nTmp = abs(*(m_fmEven[nCam]+pitch*y+x) - *(m_fmEven[nCam]+pitch*y+(x+4)));//*m_nLineMag +nValue;
//  				if(nTmp>nLineHighCut )	nTmp = nTmp*dLineMag +nValue;
//  				else					nTmp = nTmp*(dLineMag*0.5) +nValue;
//  				if(nTmp<0) nTmp=0;
//  				else if(nTmp>255) nTmp=255;
//  				m_nPreProcessData1[nCam][nindex][x] += nTmp;//전처리 평균 data 라인
//  				*(m_fmPreProcess1[nCam]+pitch*y+x) = nTmp;//x 전처리 이미지 라인 전처리
//  			}
		}
		//if(nXcount1==0)nXcount1= 0;
		//dXprojData[y] /= nXcount1;
		nycnt1++;//211004

	} //옛날 전처리 코드 20230424

	////신규 전처리 코드 20230424
	//const __m256 _mHighCut = _mm256_set1_ps(nHighCut);
	//const __m256 _mLowCut = _mm256_set1_ps(-nLowCut);
	//const __m256 _mValue = _mm256_set1_ps(nValue);

	//for (int y = nstt - noffy1; y < nend + noffy2; y++)//검사영역보다 더 크게 전처리를 한다. //211004
	//{
	//	nXcount1 = 0;

	//	for (int x = x1 - 10; x < x2 + 10 && x1>10 && x2 < nw - 10; x += 8)//cmpgt와 cmplt 적절히 섞어서 잘 사용해보면 될 것 같음
	//	{
	//		__m256i subtractValue = _mm256_setr_epi32(m_nSubProjData[nCam][nindex][x], m_nSubProjData[nCam][nindex][x + 1], m_nSubProjData[nCam][nindex][x + 2], m_nSubProjData[nCam][nindex][x + 3],
	//			m_nSubProjData[nCam][nindex][x + 4], m_nSubProjData[nCam][nindex][x + 5], m_nSubProjData[nCam][nindex][x + 6], m_nSubProjData[nCam][nindex][x + 7]);
	//		__m256i bufferValue = _mm256_set1_epi32(0);

	//		if (!m_bInspDarkRejectAtBright[nCam])
	//		{
	//			bufferValue = _mm256_setr_epi32(m_fmOdd[nCam][nw * y + x], m_fmOdd[nCam][nw * y + x + 1], m_fmOdd[nCam][nw * y + x + 2], m_fmOdd[nCam][nw * y + x + 3],
	//				m_fmOdd[nCam][nw * y + x + 4], m_fmOdd[nCam][nw * y + x + 5], m_fmOdd[nCam][nw * y + x + 6], m_fmOdd[nCam][nw * y + x + 7]);
	//		}
	//		else
	//		{
	//			bufferValue = _mm256_setr_epi32(m_fmEven[nCam][nw * y + x], m_fmEven[nCam][nw * y + x + 1], m_fmEven[nCam][nw * y + x + 2], m_fmEven[nCam][nw * y + x + 3],
	//				m_fmEven[nCam][nw * y + x + 4], m_fmEven[nCam][nw * y + x + 5], m_fmEven[nCam][nw * y + x + 6], m_fmEven[nCam][nw * y + x + 7]);
	//		}

	//		__m256i subtractBuffer = _mm256_sub_epi32(bufferValue, subtractValue);

	//		__m256 highCutMask = _mm256_cmp_ps(_mm256_cvtepi32_ps(subtractBuffer), _mHighCut, _CMP_GT_OQ);
	//		__m256 lowCutMask = _mm256_cmp_ps(_mm256_cvtepi32_ps(subtractBuffer), _mLowCut, _CMP_LT_OQ);

	//		__m256 highCutRes = _mm256_blendv_ps(_mm256_set1_ps(dmag1), _mm256_set1_ps(dmag), highCutMask);
	//		__m256 lowCutRes = _mm256_blendv_ps(_mm256_set1_ps(dmag1), _mm256_set1_ps(dmag), lowCutMask);

	//		__m256 lowHighAddRes = _mm256_add_ps(highCutRes, lowCutRes);

	//		__m256 lowHighMask = _mm256_cmp_ps(lowHighAddRes, _mm256_set1_ps(dmag1 * 2), _CMP_GT_OQ);

	//		__m256 lowHighRes = _mm256_blendv_ps(_mm256_set1_ps(dmag1), _mm256_set1_ps(dmag), lowHighMask);

	//		__m256i _mSubProj = _mm256_loadu_si256((__m256i*)(m_nSubProjData[nCam][nindex] + x));

	//		__m256 _mTmp = _mm256_add_ps(_mValue, _mm256_mul_ps(_mm256_cvtepi32_ps(subtractBuffer), lowHighRes));

	//		__m256 _mTmp2Mask = _mm256_cmp_ps(_mTmp, _mm256_set1_ps(255), _CMP_GT_OQ);
	//		__m256 _mTmp2 = _mm256_blendv_ps(_mTmp, _mm256_set1_ps(255), _mTmp2Mask);
	//		__m256 _mTmp3Mask = _mm256_cmp_ps(_mTmp2, _mm256_set1_ps(0), _CMP_LT_OQ);
	//		__m256 _mTmp3 = _mm256_blendv_ps(_mTmp2, _mm256_set1_ps(0), _mTmp3Mask);
	//		__m256 _mLaseTmpMask = _mm256_cmp_ps(_mm256_cvtepi32_ps(_mSubProj), _mm256_set1_ps(200), _CMP_GT_OQ);
	//		__m256 _mLastTmp = _mm256_blendv_ps(_mTmp3, _mm256_set1_ps(255), _mLaseTmpMask);

	//		__m256i _mPreprocessData = _mm256_loadu_si256((__m256i*)(m_nPreProcessData[nCam][nindex] + x));
	//		__m256i sumValue = _mm256_add_epi32(_mPreprocessData, _mm256_cvtps_epi32(_mLastTmp));
	//		_mm256_storeu_si256((__m256i*)(m_nPreProcessData[nCam][nindex] + x), sumValue);
	//		__m256i cvtValue = _mm256_cvtps_epi32(_mLastTmp);
	//		__m256i _mResTmp = _mm256_packs_epi32(cvtValue, cvtValue);

	//		for (int i = 0; i < 8; i++) *(m_fmPreProcess[nCam] + (nw * y) + x + i) = _mResTmp.m256i_u16[i];

	//		nXcount1 += 8;
	//	}

	//	nycnt1++;//211004
	//}

	////신규 전처리 코드 20230424

	//IPVM::Image_8u_C1 preProcessAfter;
	//preProcessAfter.Create(nw, nh);
	//memcpy(preProcessAfter.GetMem(), m_fmPreProcess[nCam], nw * nh);
	//preProcessAfter.SaveBmp(_T("Preprocess_After.bmp"));

	int nSmoothLine = 16;//0929
	if(nycnt1<1)nycnt1 = 1;//220523
	for(int x=x1-10;x<x2+10 && x1>10 && x2<nw-10;x++){
		m_nPreProcessData[nCam][nindex][x] /= nycnt1;// nh1;//이물 전처리  //211004  변한 Height로 나눠 준다ㅓ.
		if (m_nPreProcessData[nCam][nindex][x]>255)//0618-2
			m_nPreProcessData[nCam][nindex][x] = 255;//0618-2
//		m_nPreProcessData1[nCam][nindex][x] /= nh1;//라인 전처리
		if (nindex == 0 && x>x1 - 10 + nSmoothLine){//0929
			for (int k1 = x; k1>x - nSmoothLine; k1--)
				m_nPreProcessData1[nCam][nindex][x] += nPoj1[k1];//]m_nPreProcessData[nCam][nindex][k1];
			m_nPreProcessData1[nCam][nindex][x] /= nSmoothLine;
		}

	}
	m_dInsSubTime2[nCam][nIdx][2] = GetPrecisionTime();//0706
	if (nIdx == 0 && nstt < m_nCellMargin)nstt = m_nCellMargin;//0926
	if (nIdx == MOD_NUMBER-1 && nend > nh - m_nCellMargin)
		nend = nh - m_nCellMargin;//0926

	///검사
	int nDefectCount = 0,nw2=BAD_WIDTH*0.5,nh2=BAD_HEIGHT*0.5,nmid=(nend+nstt)*0.5,nscoffset=16;//defect 이미지 사이즈 지훈이랑 맞출것.
	int nw3 = 384,nh3=384,nw4=192,nh4=192;//후보 영역에 대한 사이즈
	int nScValue = m_dLineCut[nCam], nThw = m_nTh1[nCam], nThb = m_nTh2[nCam];
	int	nIslandThb = m_nIslandTh[nCam];
	BOOL	bFlag1 = FALSE;
	int nmidpoint = (nend+nstt)*0.5;
	
	int nCountEdge = nRoiCount;
	if (nindex == 0){//0929
		int blinev = m_nLineBth[nCam];
		for (int k = 0; k < 1; k++)
		{
			x1 = rtins.left + nxoffset1; x2 = rtins.right - nxoffset1;//0813
			//SC 먼저 검사
			for (int x = x1 + 48 + nSkip1; x<x2 - nscoffset * 2 - 48 - nSkip1; x++)
			{
				if (x > m_nWidth - 150)
					continue; 
				
				if (x < 100)
					continue;//211017-1
				
				if (x<rtins.left + 5)x = rtins.left + 2 + 5;
				
				if (m_dSubPeakData[nCam][nindex][0] < abs(m_nPreProcessData1[nCam][nindex][x] - m_nPreProcessData1[nCam][nindex][x + nscoffset]))
					m_dSubPeakData[nCam][nindex][0] = abs(m_nPreProcessData1[nCam][nindex][x] - m_nPreProcessData1[nCam][nindex][x + nscoffset]);

				if (m_nPreProcessData1[nCam][nindex][x + nscoffset] > 0 && 
					(abs(m_nPreProcessData1[nCam][nindex][x] - m_nPreProcessData1[nCam][nindex][x + nscoffset]) > nScValue))// ||abs(m_nPreProcessData1[nCam][nindex][x] - m_nPreProcessData1[nCam][nindex][x + nscoffset]) >blinev))
				{
					//SC 후보
					m_rtSubDefect[nCam][nindex][nDefectCount].SetRect(x - nw2, nmidpoint - nh2, x + nw2, nmidpoint + nh2);
					m_rtSubDefect[nCam][nindex][nDefectCount] = SetBoundary2(m_rtSubDefect[nCam][nindex][nDefectCount], CRect(100, 0, m_nWidth - 100, m_nHeight));
					BYTE * fm14 = m_pSubDefet[nCam][nIdx][nDefectCount][0];//전처리
					if(!m_bInspDarkRejectAtBright[nCam])
						ImageSelectCopy(m_fmOdd[nCam], fm14, nw, nh, m_rtSubDefect[nCam][nindex][nDefectCount], BAD_WIDTH, BAD_HEIGHT, 0, 0);
					else
						ImageSelectCopy(m_fmEven[nCam], fm14, nw, nh, m_rtSubDefect[nCam][nindex][nDefectCount], BAD_WIDTH, BAD_HEIGHT, 0, 0);
					//백 흑 구분 1. 백 흑 구분
					int nv12 = DualLineBlob(fm14, nCam, nIdx, nDefectCount, 1);//nbw 3 무지
					m_nSubDefectType[nCam][nindex][nDefectCount] = nv12;
					m_rtSubDefect[nCam][nindex][nDefectCount].SetRect(x - nw2, nstt, x + nw2, nend);
					nDefectCount++;					x += nw2;//nDefectCount
				}
				if (nDefectCount>5)break;
			}
		}
	}
	
	m_nSubDefectCount[nCam][nIdx] = nDefectCount;//0706
	m_dInsSubTime2[nCam][nIdx][3] = GetPrecisionTime();//0706


	
	int nEdgeFind = 0;
	int nDarktmp = 0, nsumpro = 0, nsumpix = 0, nDarkPeak = 255;;//0527-1
	int nEdgeOff[MAX_SPLIT_DEFECT] = { 0, };//211003
	int nBrightColor[MAX_SPLIT_DEFECT] = { 0, };//211003
	int nXGap1, nYGap1;

	if (true == m_bUseMinimumDarkSpec[nCam]|| true == m_bMiniCrater[nCam] || true ==m_bMiniDent[nCam])
	{
		double dXGap = FindMinSizeX(nCam);
		double dYGap = FindMinSizeY(nCam);
		nXGap1 = dXGap / m_dscale_x1[nCam];
		nYGap1 = dYGap / m_dscale_y1[nCam];//0.4 mm Offset을 확인 //220630
	}
	else
	{
		nXGap1 = 10;
		nYGap1 = 5;//0.4 mm Offset을 확인 //220630
	}

	for (int y = nstt + 2; y<nend - 2 && y<3000; y += nymove)
	{
		nEdgeFind=0;
		if(nDefectCount>=MAX_SPLIT_DEFECT)			
			break;

		if(m_bTiltEdge[nCam])
		{//220403
			x1 = m_nLRedge[nCam][0][y]+ m_nEdgeOffset1[nCam];
			x2 = m_nLRedge[nCam][1][y] - m_nEdgeOffset1[nCam];
		}

		for (int x = x1; x<x2; x += nxmove)
		{
			if (x < 16)
				continue;//0518-4

			njump1 = 0;
			if(njump1>0)
			{
				x = njump1;
				continue;
			}
			
			if(x<rtins.left)
				x=rtins.left+2;

//			if(x>rtins.right)break;//220221
			if(nDefectCount >= MAX_SPLIT_DEFECT)
				break;

			//두개를 다본다
			nTmp = ((*(m_fmPreProcess[nCam] + pitch * y + x) - m_nTargetBrightness[nCam]) + (*(m_fmPreProcess[nCam] + pitch * y + (x + 1)) - m_nTargetBrightness[nCam])) * 0.5;
			//코팅인지 무지인지 확인한다.
			{
				//코팅
				
				if(m_dSubPeakData[nCam][nindex][1] < nTmp)
					m_dSubPeakData[nCam][nindex][1] =nTmp;
				
				if(m_dSubPeakData[nCam][nindex][2] > nTmp)
					m_dSubPeakData[nCam][nindex][2] =nTmp;
				
				if( y == nstt + 2)
					m_dsubAveragValue[nCam][nindex][1][0] += m_nSubProjData[nCam][nindex][x], nCoatNoncoatcnt[1][0]++;//ave
				
				njump = 0;
				
				if (y == nstt + 2 && x > nEdgeFind + 100)
				{//0618-2

					if (m_nSubProjData[nCam][nindex][x]>240 && m_nSubProjData[nCam][nindex][x + 3]>240)
					{
						m_nSubDefectBright[nCam][nindex][nDefectCount] = 4;
						m_ptSubDefectPoint[nCam][nindex][nDefectCount].SetPoint(x, y);
						//notch
						if(y<nh*0.5)
						{
							if(m_rtNotchArea[nCam][0].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount])){
								x+= nw2;continue;
							}
							if(m_rtNotchArea[nCam][2].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount])){
								x+= nw2;continue;
							}
						}
						else
						{
							if(m_rtNotchArea[nCam][1].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount])){
								x+= nw2;continue;
							}
							if(m_rtNotchArea[nCam][3].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount])){
								x+= nw2;continue;
							}
						}

						m_rtSubDefect[nCam][nindex][nDefectCount].SetRect(x - nw2, y - nh2, x + nw2, y + nh2);
						m_rtSubDefect[nCam][nindex][nDefectCount] = SetBoundary2(m_rtSubDefect[nCam][nindex][nDefectCount], CRect(rtins.left+24, 0, rtins.right-64, m_nHeight));//211007
						int nEdgeoffset1 = x - nEdgeFind;
						nEdgeOff[nDefectCount] = nEdgeoffset1;//211003
						nBrightColor[nDefectCount] = 4;//211003

						int ncode = 0;

						if(!m_bInspDarkRejectAtBright[nCam])
							ncode = DualBlob(m_fmOdd[nCam], m_fmPreProcess[nCam], nCam, nindex, nDefectCount, 2, nEdgeoffset1);
						else
							ncode = DualBlob(m_fmEven[nCam], m_fmPreProcess[nCam], nCam, nindex, nDefectCount, 2, nEdgeoffset1);

						if(ncode<=0 && nDefectCount >=2)
						{
							x+= nw2;
						}
						
						m_nSubDefectType[nCam][nindex][nDefectCount] = ncode;
						
						nDefectCount++;
						x += nw2;
						continue;//
					}
				}
				if(nTmp >= nThw)
				{
					if(x < nEdgeFind + 60 || x2 - x < 16)
						continue;//@0514

					if(*(m_fmPreProcess[nCam]+pitch*(y+1)+x) -m_nPreProcessData[nCam][nindex][x] < nThw || *(m_fmPreProcess[nCam]+pitch*(y)+x+2) -m_nPreProcessData[nCam][nindex][x+2] < nThw)
						continue;  //이걸로 인해서 미니핀홀 못잡을수 도 있을거같다.. 아무래도 미니핀홀에 대한 고려가 처음에 안되어있었기 때문인듯.
					
					for(int p = 0; p < nDefectCount; p++)
					{
						CPoint pt;
						pt.SetPoint(x,y);
						if(m_rtSubDefect[nCam][nindex][p].PtInRect(pt)>0)
							njump = 1;

						if(njump == 1)
							break;
					}
					
					if(njump == 1)
					{
						x += nw2;
						continue;
					}

					m_nSubDefectBright[nCam][nindex][nDefectCount] = 1;
					m_ptSubDefectPoint[nCam][nindex][nDefectCount].SetPoint(x,y);
					if(y < nh * 0.5)
					{
						if(m_rtNotchArea[nCam][0].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount]))
						{
							x+= nw2;
							continue;
						}
						if(m_rtNotchArea[nCam][2].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount]))
						{
							x+= nw2;
							continue;
						}
					}
					else
					{
						if(m_rtNotchArea[nCam][1].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount]))
						{
							x += nw2;
							continue;
						}
						if(m_rtNotchArea[nCam][3].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount]))
						{
							x += nw2;
							continue;
						}
					}
					m_rtSubDefect[nCam][nindex][nDefectCount].SetRect(x-nw2,y-nh2,x+nw2,y+nh2);
					m_rtSubDefect[nCam][nindex][nDefectCount] = SetBoundary2(m_rtSubDefect[nCam][nindex][nDefectCount],CRect(rtins.left+8,0,rtins.right-8,m_nHeight));//211007

					int nEdgeoffset1 = x-nEdgeFind;
					nEdgeOff[nDefectCount] = nEdgeoffset1;
					nBrightColor[nDefectCount] = 1;//211003
					int ncode = 0;

					if(!m_bInspDarkRejectAtBright[nCam])
						ncode = DualBlob(m_fmOdd[nCam], m_fmPreProcess[nCam], nCam, nindex, nDefectCount, 1, nEdgeoffset1);
					else
						ncode = DualBlob(m_fmEven[nCam], m_fmPreProcess[nCam], nCam, nindex, nDefectCount, 1, nEdgeoffset1);

					/*if(ncode<=0 && nDefectCount >=2){
						x+= nw2;
					}
					else*/
					{
						m_nSubDefectType[nCam][nindex][nDefectCount] = ncode;
						nDefectCount++;
						x+= nw2;
					}
				}
				else if(nTmp<=-nThb)
				{
					//밑에 3칸을 본다
					
					if (x < nEdgeFind + 60 || x2 - x<nXGap1 ||y > nh - nYGap1 || x>x2-nXGap1 )
						continue;//@//220629

					if( *(m_fmPreProcess[nCam]+pitch*(y+nYGap1)+x) -m_nPreProcessData[nCam][nindex][x] > -nThb || *(m_fmPreProcess[nCam]+pitch*(y)+x+nXGap1) -m_nPreProcessData[nCam][nindex][x] > -nThb)
					{
						x += ( nXGap1 * 2);
						continue;//0.4mm 이하 흑점 Skip
					}

//					if(y+3<nend && *(m_fmPreProcess[nCam]+pitch*(y+3)+x) -m_nPreProcessData[nCam][nindex][x] > -nThb){//220504
//						x+=3;continue;}
					for(int p = 0; p < nDefectCount; p++)
					{
						CPoint pt;
						pt.SetPoint(x,y);
						if(m_rtSubDefect[nCam][nindex][p].PtInRect(pt)>0)
							njump = 1;
						
						if(njump == 1)
							break;
					}
					
					if( njump == 1)
					{
						x+= nw2;
						continue;
					}
					
					m_nSubDefectBright[nCam][nindex][nDefectCount]=2;
					m_ptSubDefectPoint[nCam][nindex][nDefectCount].SetPoint(x,y);
					
					if(y < nh * 0.5)
					{
						if(m_rtNotchArea[nCam][0].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount]))
						{
							x += nw2;
							continue;
						}
						
						if(m_rtNotchArea[nCam][2].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount]))
						{
							x += nw2;
							continue;
						}
					}
					else
					{
						if(m_rtNotchArea[nCam][1].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount]))
						{
							x+= nw2;
							continue;
						}
						if(m_rtNotchArea[nCam][3].PtInRect(m_ptSubDefectPoint[nCam][nindex][nDefectCount]))
						{
							x+= nw2;
							continue;
						}
					}

					m_rtSubDefect[nCam][nindex][nDefectCount].SetRect(x-nw2,y-nh2,x+nw2,y+nh2);
					m_rtSubDefect[nCam][nindex][nDefectCount] = SetBoundary2(m_rtSubDefect[nCam][nindex][nDefectCount],CRect(rtins.left+m_nEdgeOffset1[nCam],0,rtins.right-m_nEdgeOffset1[nCam],m_nHeight));//220504
					
					int nEdgeoffset1 = x-nEdgeFind;
					nEdgeOff[nDefectCount] = nEdgeoffset1;
					nBrightColor[nDefectCount] = 2;//211003

					int ncode = 0;

					if(!m_bInspDarkRejectAtBright[nCam])
						ncode = DualBlob(m_fmOdd[nCam], m_fmPreProcess[nCam], nCam, nindex, nDefectCount, 1, nEdgeoffset1);
					else
						ncode = DualBlob(m_fmEven[nCam], m_fmPreProcess[nCam], nCam, nindex, nDefectCount, 1, nEdgeoffset1);

					if(ncode <= 0 && nDefectCount >= 2)
					{
						x += nw2;
					}
					else
					{
						m_nSubDefectType[nCam][nindex][nDefectCount] = ncode;
						nDefectCount++;
						x += nw2;
					}
				}
			}
			m_nSubDefectCount[nCam][nIdx] = nDefectCount;//0706
			
			if(nDefectCount>=MAX_SPLIT_DEFECT || m_bThreadStop[nCam])				
				break;//220207
		}
	
	}

	int pPosInfo[4] = { m_rtCell[nCam].left + nSkip1, m_rtCell[nCam].right - nSkip1, m_nCellMargin, nh - m_nCellMargin };

	//FindDefectAtPreprocess(nCam, m_fmPreProcess[nCam], nw, nh, pPosInfo);

	//for (int i = 0; i < nDefectCount; i++) {//211003
	//	int ncode = DualBlob(m_fmEven[nCam], m_fmPreProcess[nCam], nCam, nindex, i, nBrightColor[i], nEdgeOff[i]);
	//	m_nSubDefectType[nCam][nindex][i] = ncode;
	//}

	if(nCoatNoncoatcnt[0][0]>0)
		m_dsubAveragValue[nCam][nindex][0][0] /= nCoatNoncoatcnt[0][0];
	if(nCoatNoncoatcnt[1][0]>0)
		m_dsubAveragValue[nCam][nindex][1][0] /= nCoatNoncoatcnt[1][0];

	m_nSubDefectCount[nCam][nIdx] = nDefectCount;
	m_dInsSubTime2[nCam][nIdx][4] = GetPrecisionTime();//0701

	return nDefectCount;
}

int	CInspect::DualBlob(BYTE * fmOrigin, BYTE* fmPre, int nCam, int nIdx, int nDefect, int nBW, int nEdge)//nbw 3 무지
{
	Classification CF;
	CF.SetProtrutionSpec(m_dWhiteSizeX[nCam],
		m_dWhiteSizeY[nCam],
		m_dProtrusionMinSize[nCam],
		m_dProtrusionMinSize[nCam],
		m_dMiniProtrution_SizeX[nCam],
		m_dMiniProtrution_SizeY[nCam],
		m_nProtrusionTh[nCam],
		m_bMiniProtrution[nCam]);

	CF.SetPinholeSpec(m_dPinholeSizeX[nCam],
		m_dPinholeSizeY[nCam],
		m_dPinholeMinSize[nCam],
		m_dPinholeMinSize[nCam],
		m_dMiniPinhole_SizeX[nCam],
		m_dMiniPinhole_SizeY[nCam],
		m_nPinholeCut[nCam],
		m_bMiniPinhole[nCam]);

	CF.SetCraterSpec(m_dDarkSizeX[nCam],
		m_dDarkSizeY[nCam],
		m_dCraterMinSize[nCam],
		m_dCraterMinSize[nCam],
		m_dMiniCrater_SizeX[nCam],
		m_dMiniCrater_SizeY[nCam],
		m_nBlackCut1[nCam],
		m_bMiniCrater[nCam],
		m_dBlackPercent[nCam],
		0.7);

	CF.SetScratchSpec(m_dBlackLineSizeX[nCam],
		m_dBlackLineSizeY[nCam],
		m_dScratchMinSize[nCam],
		m_dScratchMinSize[nCam],
		m_nScratchValue[nCam],
		85);

	CF.SetDentSpec(m_dCunicSizeX[nCam],
		m_dCunicSizeY[nCam],
		m_dDentMinSize[nCam],
		m_dDentMinSize[nCam],
		m_dMiniDent_SizeX[nCam],
		m_dMiniDent_SizeY[nCam],
		0,
		m_bMiniDent[nCam]);
		

	//블랍은 듀얼로 한다.
	int nw = m_nWidth, nh = m_nHeight;
	int nw1 = BAD_WIDTH, nh1 = BAD_HEIGHT;
	BYTE * fm = m_pSubDefet[nCam][nIdx][nDefect][1];//전처리
	BYTE * fm3 = m_pSubDefet[nCam][nIdx][nDefect][0];//origin
	BYTE * fm1 = new BYTE[BAD_WIDTH*BAD_HEIGHT];//white
	BYTE * fm2 = new BYTE[BAD_WIDTH*BAD_HEIGHT];//black
	BYTE * fm4 = new BYTE[BAD_WIDTH*BAD_HEIGHT];//black+white
	CRect rt2 = m_rtSubDefect[nCam][nIdx][nDefect], rt1, rt;
	CPoint cpROIoffset(rt2.left, rt2.top);


	ImageSelectCopy(fmPre, fm, nw, nh, m_rtSubDefect[nCam][nIdx][nDefect], BAD_WIDTH, BAD_HEIGHT, 0, 0);
	ImageSelectCopy(fmOrigin, fm3, nw, nh, m_rtSubDefect[nCam][nIdx][nDefect], BAD_WIDTH, BAD_HEIGHT, 0, 0);

	int nTmp = 0, nThw = m_nTh1[nCam], nThb = m_nTh2[nCam];//pDoc->m_data.m_dThDn[nCam];
	int nmarginb = m_nMargin[nCam][1];//흑
	int nmarginw = m_nMargin[nCam][0];//백
	int nthw1 = nThw - nmarginw, nthb1 = nThb - nmarginb;//size를 재기위해 마진을 더둔다.
	int ngap = 1;//2;
	int nsum = 0, nsum1 = 0, npeakValue[3] = { 0, 1000, 0 }, ncnt1 = 0;
	int nret = -1;
	int nCrack = 0;
	rt.SetRect(0, 0, BAD_WIDTH, BAD_HEIGHT);

	for (int i = 0; i < 3; i++) 
	{
		//211017
		if (i == 1)
			m_dPeakData[nCam][nIdx][nDefect][0][i] = m_dPeakData[nCam][nIdx][nDefect][1][i] = 255;
		else
			m_dPeakData[nCam][nIdx][nDefect][0][i] = m_dPeakData[nCam][nIdx][nDefect][1][i] = 0;

	}

	for (int i = 0; i < 2; i++) 
	{
		m_dSubAngle[nCam][nIdx][nDefect][i] = 0;
		m_dSubStdValue[nCam][nIdx][nDefect][i] = 0;
		m_dSubCompactness[nCam][nIdx][nDefect][i] = 0;
		m_dSubSizeX[nCam][nIdx][nDefect][0] = m_dSubSizeY[nCam][nIdx][nDefect][0] = 0;
		m_dSubSizeX[nCam][nIdx][nDefect][1] = m_dSubSizeY[nCam][nIdx][nDefect][1] = 0;
	}

	double dBlackCogx = 0, dBlackCogy = 0, dblackCogCnt = 0;//0524-3
	for (int y = rt.top; y < rt.bottom; y++) 
	{
		for (int x = rt.left; x < rt.right; x++) 
		{
			if (y <= ngap || y >= BAD_HEIGHT - 1 - ngap || x <= rt.left + ngap || x >= rt.right - 1 - ngap)
			{
				*(fm1 + nw1 * y + x) = *(fm2 + nw1 * y + x) = 0;//테두리를 흑색으로 치환한다. blob에서 에지가 걸리면 검출이 안된.
			}
			else 
			{
				rt1.SetRect(x - ngap, y - ngap, x + ngap, y + ngap);
				nsum = AreaAve1(fm, rt1, BAD_WIDTH, &nsum1);

				if (nsum > npeakValue[0])
					npeakValue[0] = nsum1;//이진화를 위한 밝기의 peak

				if (nsum < npeakValue[1])
					npeakValue[1] = nsum;//이진화를 위한 밝기의 peak

				npeakValue[2] += nsum;

				if (*(fmOrigin + (y + rt2.top)*nw + (x + rt2.left)) > m_dPeakData[nCam][nIdx][nDefect][0][0])
					m_dPeakData[nCam][nIdx][nDefect][0][0] = *(fmOrigin + (y + rt2.top)*nw + (x + rt2.left));//오리진 피크밝기

				if (*(fmOrigin + (y + rt2.top)*nw + (x + rt2.left)) < m_dPeakData[nCam][nIdx][nDefect][0][1])
					m_dPeakData[nCam][nIdx][nDefect][0][1] = *(fmOrigin + (y + rt2.top)*nw + (x + rt2.left));//오리진 피크밝기

				m_dPeakData[nCam][nIdx][nDefect][0][2] += *(fmOrigin + (y + rt2.top)*nw + (x + rt2.left));

				if (*(fm + y*BAD_WIDTH + x) > m_dPeakData[nCam][nIdx][nDefect][1][0])
					m_dPeakData[nCam][nIdx][nDefect][1][0] = *(fm + y*BAD_WIDTH + x);//전처리 피크

				if (*(fm + y*BAD_WIDTH + x) < m_dPeakData[nCam][nIdx][nDefect][1][1])
					m_dPeakData[nCam][nIdx][nDefect][1][1] = *(fm + y*BAD_WIDTH + x);//전처리 피크

				m_dPeakData[nCam][nIdx][nDefect][1][2] += *(fm + y*BAD_WIDTH + x);//평균
				*(fm2 + nw1*y + x) = *(fm1 + nw1*y + x) = *(fm4 + nw1*y + x) = 0;
				//if (nsum1 - m_nPreProcessData[nCam][nIdx][x + rt2.left] >nthw1)	*(fm1 + nw1*y + x) = *(fm4 + nw1*y + x) = 255;
				//else if (nsum - m_nPreProcessData[nCam][nIdx][x + rt2.left] < -nthb1){
				//	*(fm2 + nw1*y + x) = 255; *(fm4 + nw1*y + x) = 128;
				
				if (nsum1 - m_nTargetBrightness[nCam] > nthw1)
				{
					*(fm1 + nw1 * y + x) = *(fm4 + nw1 * y + x) = 255;
				}
				else if (nsum - m_nTargetBrightness[nCam] < -nthb1)
				{
					*(fm2 + nw1*y + x) = 255; *(fm4 + nw1*y + x) = 128;
					dBlackCogx += x; dBlackCogy += y; dblackCogCnt++;//0524-3
				}
				ncnt1++;
			}
		}
	}

	//IPVM::Image_8u_C1 image, image2, imagePre;
	//image.Create(nw1, nh1);
	//image2.Create(nw1, nh1);
	//memcpy(image.GetMem(), fm2, nw1 * nh1);
	//memcpy(image2.GetMem(), fm4, nw1 * nh1);

	//CString strNum;
	//strNum.Format(_T("%d"), m_ninspCount);

	//image.SaveBmp(_T("Dark_") + strNum + _T(".bmp"));
	//image2.SaveBmp(_T("Bright_Dark_") + strNum + _T(".bmp"));

	//if (m_ninspCount == 0)
	//{
	//	imagePre.Create(nw, nh);
	//	memcpy(imagePre.GetMem(), fmPre, nw * nh);
	//	imagePre.`SaveBmp(_T("Preprocess.bmp"));
	//}

	//m_ninspCount++;

	if (dblackCogCnt > 0) 
	{
		//0524-3 흑의 중심을 구한다.
		dBlackCogx /= dblackCogCnt; 
		dBlackCogy /= dblackCogCnt;
	}

	if(ncnt1<1)
		ncnt1 = 1;

	npeakValue[2] /= ncnt1;
	m_dPeakData[nCam][nIdx][nDefect][0][2] /= ncnt1;//오리진 이미지
	m_dPeakData[nCam][nIdx][nDefect][1][2] /= ncnt1;//전처리 이미지

	if (m_bDefectBinary)//1412
		memcpy(m_pSubDefet[nCam][nIdx][nDefect][0], fm4, BAD_WIDTH*BAD_HEIGHT);

	//Blob white
	CPoint ptCenter[4];
	for (int i = 0; i < 4; i++)
	{
		ptCenter[i].SetPoint(0, 0);
	}

	int  nBlobCount = -1, nBlobNo = 0;
	double dPercent[4] = { 0, }, dTemp = 0, dBlobSize = 0, dBlobSize1 = 0;//0 count 1 percent 2 count 3 percent

	CRect rtBlob;
	
	//*****************************************************************************************************************************************************
	//*****************************************************            white blob	       ****************************************************************
	//*****************************************************************************************************************************************************
	
	m_pChain[nCam][nIdx][0]->SetChainData(1, fm1, 2, 1, 2, 100000, BAD_WIDTH, BAD_HEIGHT);

	//*****************************************************************************************************************************************************
	//*****************************************************            white blob	       ****************************************************************
	//*****************************************************************************************************************************************************

	if (nEdge > 0 && nEdge < 30)
		nBlobCount = m_pChain[nCam][nIdx][0]->FastChain(nEdge, 4, BAD_WIDTH - 1, BAD_HEIGHT - 4);//@0512
	else if (nEdge<0 && nEdge>-90)	
		nBlobCount = m_pChain[nCam][nIdx][0]->FastChain(1, 4, BAD_WIDTH - 1 + nEdge + 32, BAD_HEIGHT - 4);//@0512
	else 							
		nBlobCount = m_pChain[nCam][nIdx][0]->FastChain(1, 4, BAD_WIDTH - 1, BAD_HEIGHT - 4);

	for (int i = 0; i < nBlobCount; i++)
	{
		dTemp = m_pChain[nCam][nIdx][0]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		
		if (dTemp < 2)
			continue;
		
		if (dTemp >= dBlobSize)//최대 불량 사이즈.
		{
			nBlobNo = i;	
			dBlobSize = dTemp;//면적
		}
	}

	double dCenterPosX_Pixel = 0, dCenterPosY_Pixel= 0, tmp = 0, dSum1 = 0, dSum2 = 0, dstdev2 = 0, dstdev3 = 0, tmp1 = 0, dSum11 = 0, dSum12 = 0, dstdev12 = 0, dstdev13 = 0, dstdev22 = 0, dstdev23 = 0;
	int nTmp11 = 0;
	int nSkip1 = 0;
	double dmindist = 0, dmaxdist = 0, davedist = 0, dDark_Width_Pixel = 0, dDark_Length_Pixel = 0, dDark_WL_Avg_Pixel = 0;//211008-1
	
	if (dBlobSize >= 2) 
	{
		////에지부 과검
		m_pChain[nCam][nIdx][0]->Chain_Center(nBlobNo, &dCenterPosX_Pixel, &dCenterPosY_Pixel);

		if (nEdge < 64) 
		{
			if (dCenterPosX_Pixel < 24) 
			{
				//노치 크기
				nSkip1 = 1;
			}
		}

		if (nSkip1 == 0) 
		{
			int nx1 = m_pChain[nCam][nIdx][0]->FindMaxX(nBlobNo);
			int nx2 = m_pChain[nCam][nIdx][0]->FindMinX(nBlobNo);
			int ny1 = m_pChain[nCam][nIdx][0]->FindMaxY(nBlobNo);
			int ny2 = m_pChain[nCam][nIdx][0]->FindMinY(nBlobNo);
			//			if (nx1 <= BAD_WIDTH - 2 && nx2 <= BAD_WIDTH*0.6)
			m_rtCell[nCam].right;
			rt2;
			m_rtCellTab[nCam];

			if ((dCenterPosX_Pixel > 20 && ny1 - ny2 < BAD_WIDTH * 1.5) || ( m_rtCell[nCam].right - m_nEdgeOffset1[nCam] > rt2.right && dCenterPosX_Pixel < BAD_WIDTH - 20 && ny1 - ny2 < BAD_WIDTH * 1.5))
			{
				//////////211007
				m_dSubCompactness[nCam][nIdx][nDefect][0] = m_pChain[nCam][nIdx][0]->FindCompactness(nBlobNo);
				m_dSubAngle[nCam][nIdx][nDefect][0] = m_pChain[nCam][nIdx][0]->FindAngle(nBlobNo);
				m_rtSubBlob[nCam][nIdx][nDefect][0].SetRect(m_pChain[nCam][nIdx][0]->FindMinX(nBlobNo), m_pChain[nCam][nIdx][0]->FindMinY(nBlobNo), m_pChain[nCam][nIdx][0]->FindMaxX(nBlobNo), m_pChain[nCam][nIdx][0]->FindMaxY(nBlobNo));
				ptCenter[0].SetPoint(dCenterPosX_Pixel, dCenterPosY_Pixel);
				
				m_pChain[nCam][nIdx][0]->FineDistFromPoint(nBlobNo, dCenterPosX_Pixel, dCenterPosY_Pixel, &dmindist, &dmaxdist, &davedist);//장축 단축 구하기  //211008

				if (dmaxdist == 0)	//exception
					dmaxdist = 1;

				if (dmindist > dmaxdist)
				{
					//언제나 width 가 짧도록 정리
					double temp = dmindist;
					dmindist = dmaxdist;
					dmaxdist = temp;
				}
																											   ////////////////////
				if ((dCenterPosX_Pixel < 15 || dCenterPosX_Pixel > BAD_WIDTH - 5) && m_dSubCompactness[nCam][nIdx][nDefect][0] > 0.50 &&  fabs(m_dSubAngle[nCam][nIdx][nDefect][0]) > 80) 
				{
					dCenterPosX_Pixel = dCenterPosX_Pixel;
				}
				else
				{
					m_dSubSizeX[nCam][nIdx][nDefect][0] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
					m_dSubSizeY[nCam][nIdx][nDefect][0] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];

					if (m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dSubSizeY[nCam][nIdx][nDefect][0])
					{
						m_dSubSize[nCam][nIdx][nDefect][0] = m_dSubSizeX[nCam][nIdx][nDefect][0];
					}
					else
					{
						m_dSubSize[nCam][nIdx][nDefect][0] = m_dSubSizeY[nCam][nIdx][nDefect][0];
					}

					//////////////std 구하기
					int nmid1 = (m_pChain[nCam][nIdx][0]->FindMaxX(nBlobNo) + m_pChain[nCam][nIdx][0]->FindMinX(nBlobNo))*0.5;
					int nc11 = 0;
					for (int k1 = m_pChain[nCam][nIdx][0]->FindMinY(nBlobNo) - 1; k1 < m_pChain[nCam][nIdx][0]->FindMaxY(nBlobNo) + 1; k1++) {
						if (k1 < 0)k1 = 0; if (k1 > BAD_HEIGHT - 1)continue;
						if (k1 + rt2.top >= nh - 2 || k1 + rt2.top <0)continue;//211214

						tmp = *(fmOrigin + nw*(k1 + rt2.top) + (nmid1 + rt2.left));//origin
						dSum1 += tmp;
						dSum2 += (tmp*tmp);
						nc11++;
					}

					dstdev2 = sqrt((nc11*dSum2 - (double)dSum1*(double)dSum1) / ((double)nc11*((double)nc11 - 1.)));
					dstdev3 = sqrt((nc11*dSum2 - (double)dSum1*(double)dSum1) / (nc11));///((double)nArea*((double)nArea-1.)));
					
					m_dSubStdValue[nCam][nIdx][nDefect][0] = dstdev3;
					
					for (int y = m_rtSubBlob[nCam][nIdx][nDefect][0].top; y <= m_rtSubBlob[nCam][nIdx][nDefect][0].bottom; y++) 
					{
						for (int x = m_rtSubBlob[nCam][nIdx][nDefect][0].left; x <= m_rtSubBlob[nCam][nIdx][nDefect][0].right; x++) 
						{
							if (*(fm1 + y * BAD_WIDTH + x) > 200)
								dPercent[0]++;
						}
					}

					dPercent[1] = dPercent[0] / ((m_rtSubBlob[nCam][nIdx][nDefect][0].Width() - 1)*(m_rtSubBlob[nCam][nIdx][nDefect][0].Height() - 1)) * 100;
					m_dSubPercent[nCam][nIdx][nDefect][0] = dPercent[1];

					//211221 개별 데이터 white
					//area
					m_dDefectAreaSub[nCam][nIdx][nDefect][DT_WHITE][0] = dBlobSize;
					m_dDefectAreaSub[nCam][nIdx][nDefect][DT_WHITE][1] = dBlobSize * m_dscale_x1[nCam];
					
					//center X
					m_dDefectCenterPosSubX[nCam][nIdx][nDefect][DT_WHITE][0] = dCenterPosX_Pixel;
					
					//center Y
					m_dDefectCenterPosSubY[nCam][nIdx][nDefect][DT_WHITE][0] = dCenterPosY_Pixel;

					//width 
					m_dDefectWidthSub[nCam][nIdx][nDefect][DT_WHITE][0] = dmindist * 2;			//dmaxdist값은 중심 기준으로 길이를 제공해서 X2 를 해야 전체 길이가된다. 
					m_dDefectWidthSub[nCam][nIdx][nDefect][DT_WHITE][1] = (dmindist * 2 * m_dscale_x1[nCam]) * 1.1;	// 좀 크게 잡아서 과검내자

					//lenght
					m_dDefectLengthSub[nCam][nIdx][nDefect][DT_WHITE][0] = dmaxdist * 2;			//dmindist값은 중심 기준으로 길이를 제공해서 X2 를 해야 전체 길이가된다.
					m_dDefectLengthSub[nCam][nIdx][nDefect][DT_WHITE][1] = (dmaxdist * 2 * m_dscale_x1[nCam]) * 1.1;	// 좀 크게 잡아서 과검내자

					//Perimeter
					m_dDefectPerimeterSub[nCam][nIdx][nDefect][DT_WHITE][0] = m_pChain[nCam][nIdx][0]->Chain_Length(nBlobNo);

					//moment
					m_dElongationSub[nCam][nIdx][nDefect][DT_WHITE][0] = m_pChain[nCam][nIdx][0]->FindMoment(nBlobNo);

					CRect rectRaw;
					rectRaw.left	= cpROIoffset.x + nx2;
					rectRaw.right	= cpROIoffset.x + nx1;
					rectRaw.top	= cpROIoffset.y + ny2;
					rectRaw.bottom = cpROIoffset.y + ny1;

					m_rtSubRawDefect[nCam][nIdx][nDefect][DT_WHITE][0] = rectRaw;
				}
			}
		}
	}

	//*****************************************************************************************************************************************************
	//*****************************************************            black blob	       ****************************************************************
	//*****************************************************************************************************************************************************

	m_pChain[nCam][nIdx][1]->SetChainData(1, fm2, 2, 2, 4, 1000000, BAD_WIDTH, BAD_HEIGHT); 

	//*****************************************************************************************************************************************************
	//*****************************************************            black blob	       ****************************************************************
	//*****************************************************************************************************************************************************

	if (nEdge > 0 && nEdge < 120)		
		nBlobCount = m_pChain[nCam][nIdx][1]->FastChain(nEdge + 32, 4, BAD_WIDTH - 1, BAD_HEIGHT - 4);
	else if (nEdge<0 && nEdge>-120)	
		nBlobCount = m_pChain[nCam][nIdx][1]->FastChain(1, 4, BAD_WIDTH - 1 + nEdge, BAD_HEIGHT - 4);
	else 							
		nBlobCount = m_pChain[nCam][nIdx][1]->FastChain(0, 0, BAD_WIDTH - 1, BAD_HEIGHT - 1);
	//	nBlobCount = m_pChain[nCam][nIdx][1]->FastChain(0, 0, BAD_WIDTH-1,BAD_HEIGHT-1);

	dBlobSize1 = dTemp = nBlobNo = 0;
	int nBlackCountCut = 5, nBlackMinSize = 4, nBlackCount1 = 0, nMinLeft = BAD_WIDTH, nMinRight = 0, nMinTop = BAD_HEIGHT, nMinBottom = 0;//0524-3
	double dBlackDist = 0;//0524-3
	
	for (int i = 0; i < nBlobCount; i++)
	{
		/*double dArea = 0.0f;
		double dCenterX, dCenterY = 0.0f;
		double dLength = 0.0f;
		double dLogicalArea = 0.0f;
		double dAngle = 0.0f;
		double dCenter_withBoundary_X, dCenter_withBoundary_Y = 0.0f;
		double dCompactness = 0.0f;
		double dMoMent = 0.0f;
		double min, max, avg = 0.0f;
		int min_x = 0;
		int max_x = 0;
		int min_y = 0;
		int max_y = 0;

		dArea = m_pChain[nCam][nIdx][1]->Chain_Area(i);
		m_pChain[nCam][nIdx][1]->Chain_Center(i, &dCenterX, &dCenterY);
		dLength = m_pChain[nCam][nIdx][1]->Chain_Length(i);
		dLogicalArea = m_pChain[nCam][nIdx][1]->Chain_LogicalArea(i);
		dAngle = m_pChain[nCam][nIdx][1]->FindAngle(i);
		dCenter_withBoundary_X, dCenter_withBoundary_Y;
		m_pChain[nCam][nIdx][1]->FindCenterWithBoundary(i, &dCenter_withBoundary_X, &dCenter_withBoundary_Y);
		dCompactness = m_pChain[nCam][nIdx][1]->FindCompactness(i);
		min_x = m_pChain[nCam][nIdx][1]->FindMinX(i);
		max_x = m_pChain[nCam][nIdx][1]->FindMaxX(i);
		min_y = m_pChain[nCam][nIdx][1]->FindMinY(i);
		max_y = m_pChain[nCam][nIdx][1]->FindMaxY(i);
		dMoMent = m_pChain[nCam][nIdx][1]->FindMoment(i);
		m_pChain[nCam][nIdx][1]->FineDistFromPoint(i, dCenterX, dCenterY, &min, &max, &avg);*/

		dTemp = m_pChain[nCam][nIdx][1]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp < 2)
			continue;

		if (dTemp >= dBlobSize1)//최대 불량 사이즈.
		{
			nBlobNo = i;
			dBlobSize1 = dTemp;//면적
		}

		if (dTemp > nBlackMinSize) 
		{//0524-3
			dCenterPosX_Pixel = 0, dCenterPosY_Pixel = 0;
			m_pChain[nCam][nIdx][1]->Chain_Center(i, &dCenterPosX_Pixel, &dCenterPosY_Pixel);
			double dst1 = sqrt(fabs(dCenterPosX_Pixel - dBlackCogx) * fabs(dCenterPosX_Pixel - dBlackCogx) + fabs(dCenterPosY_Pixel - dBlackCogy) * fabs(dCenterPosY_Pixel - dBlackCogy));
			if (dst1 < dBlackDist) 
			{
				if (m_pChain[nCam][nIdx][1]->FindMinX(i) < nMinLeft)
					nMinLeft = m_pChain[nCam][nIdx][1]->FindMinX(i);

				if (m_pChain[nCam][nIdx][1]->FindMaxX(i) > nMinRight)
					nMinRight = m_pChain[nCam][nIdx][1]->FindMaxX(i);

				if (m_pChain[nCam][nIdx][1]->FindMinY(i) < nMinTop)
					nMinTop = m_pChain[nCam][nIdx][1]->FindMinY(i);
				
				if (m_pChain[nCam][nIdx][1]->FindMaxY(i) > nMinBottom)
					nMinBottom = m_pChain[nCam][nIdx][1]->FindMaxY(i);

				nBlackCount1++;
			}
		}
	}

	if (nBlackCountCut < nBlackCount1) 
	{
		//찍힘//0524-3
		m_dSubSizeX[nCam][nIdx][nDefect][1] = (nMinRight - nMinLeft) * m_dscale_x1[nCam];
		m_dSubSizeY[nCam][nIdx][nDefect][1] = (nMinBottom - nMinTop) * m_dscale_y1[nCam];
		nCrack = 4;
	}
	else if (dBlobSize1 >= 4) 
	{
		//제일 큰 사이즈를 구한다.
		dCenterPosX_Pixel = 0, dCenterPosY_Pixel = 0;
		m_dSubCompactness[nCam][nIdx][nDefect][1] = m_pChain[nCam][nIdx][1]->FindCompactness(nBlobNo);
		m_dSubAngle[nCam][nIdx][nDefect][1] = m_pChain[nCam][nIdx][1]->FindAngle(nBlobNo);
		m_rtSubBlob[nCam][nIdx][nDefect][1].SetRect(m_pChain[nCam][nIdx][1]->FindMinX(nBlobNo), m_pChain[nCam][nIdx][1]->FindMinY(nBlobNo), m_pChain[nCam][nIdx][1]->FindMaxX(nBlobNo), m_pChain[nCam][nIdx][1]->FindMaxY(nBlobNo));
		
		m_pChain[nCam][nIdx][1]->Chain_Center(nBlobNo, &dCenterPosX_Pixel, &dCenterPosY_Pixel);
		m_pChain[nCam][nIdx][1]->FineDistFromPoint(nBlobNo, dCenterPosX_Pixel, dCenterPosY_Pixel, &dDark_Width_Pixel, &dDark_Length_Pixel, &dDark_WL_Avg_Pixel);//장축 단축 구하기  //211008
		
		if (dDark_Length_Pixel == 0)	//exception
			dDark_Length_Pixel = 1;

		if (dDark_Width_Pixel > dDark_Length_Pixel)
		{
			//언제나 width 가 짧도록 정리
			double temp = dDark_Length_Pixel;
			dDark_Length_Pixel = dDark_Width_Pixel;
			dDark_Width_Pixel = temp;
		}


		ptCenter[1].SetPoint(dCenterPosX_Pixel, dCenterPosY_Pixel);//코터 불량 구분 - 

		int nx11 = m_pChain[nCam][nIdx][1]->FindMaxX(nBlobNo);
		int nx12 = m_pChain[nCam][nIdx][1]->FindMinX(nBlobNo);
		int ny11 = m_pChain[nCam][nIdx][1]->FindMaxY(nBlobNo);
		int ny12 = m_pChain[nCam][nIdx][1]->FindMinY(nBlobNo);

		if ((dCenterPosX_Pixel < 15 || dCenterPosX_Pixel > BAD_WIDTH - 15) &&
			m_dSubCompactness[nCam][nIdx][nDefect][0] > 50 &&
			fabs(m_dSubAngle[nCam][nIdx][nDefect][0]) > 80) 
		{
			dCenterPosX_Pixel = dCenterPosX_Pixel;
		}
		else
		{
			m_dSubSizeX[nCam][nIdx][nDefect][1] = (m_pChain[nCam][nIdx][1]->FindMaxX(nBlobNo) - m_pChain[nCam][nIdx][1]->FindMinX(nBlobNo) + 1) * m_dscale_x1[nCam];
			m_dSubSizeY[nCam][nIdx][nDefect][1] = (m_pChain[nCam][nIdx][1]->FindMaxY(nBlobNo) - m_pChain[nCam][nIdx][1]->FindMinY(nBlobNo) + 1) * m_dscale_y1[nCam];
			
			//종횡비가 크게 되면 라인성으로 유추됨//0518-1
			double dxrratio1 = 0, dxrratio2 = 0;
			if (m_dSubSizeY[nCam][nIdx][nDefect][1] > 0)	
				dxrratio1 = m_dSubSizeX[nCam][nIdx][nDefect][1] / m_dSubSizeY[nCam][nIdx][nDefect][1];
			if (m_dSubSizeX[nCam][nIdx][nDefect][1] > 0)		
				dxrratio2 = m_dSubSizeY[nCam][nIdx][nDefect][1] / m_dSubSizeX[nCam][nIdx][nDefect][1];

			
			if (m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dSubSizeY[nCam][nIdx][nDefect][1])
				m_dSubSize[nCam][nIdx][nDefect][1] = m_dSubSizeX[nCam][nIdx][nDefect][1];
			else  
				m_dSubSize[nCam][nIdx][nDefect][1] = m_dSubSizeY[nCam][nIdx][nDefect][1];


			int nmid1 = (m_pChain[nCam][nIdx][1]->FindMaxX(nBlobNo) + m_pChain[nCam][nIdx][1]->FindMinX(nBlobNo)) * 0.5;
			int nc11 = 0, ncnt1 = 0;
			
			for (int k1 = m_pChain[nCam][nIdx][1]->FindMinY(nBlobNo) - 1; k1 < m_pChain[nCam][nIdx][1]->FindMaxY(nBlobNo) + 1; k1++) 
			{
				if (k1 < 0)k1 = 0; if (k1 > BAD_HEIGHT - 1)
					continue;

				if (k1 + rt2.top >= nh - 2 || k1 + rt2.top <0)
					continue;//211214

				tmp1 = *(fmOrigin + nw*(k1 + rt2.top) + (nmid1 + rt2.left));//origin
																			//			tmp1 = *(fm2+nw1*k1+nmid1);
				dSum11 += tmp1;
				dSum12 += (tmp1*tmp1);
				nc11++;
			}

			dstdev12 = sqrt((nc11*dSum12 - (double)dSum11*(double)dSum11) / ((double)nc11*((double)nc11 - 1.)));
			dstdev13 = sqrt((nc11*dSum12 - (double)dSum11*(double)dSum11) / (nc11));///((double)nArea*((double)nArea-1.)));

			m_dSubStdValue[nCam][nIdx][nDefect][1] = dstdev13;


			for (int y = m_rtSubBlob[nCam][nIdx][nDefect][1].top; y <= m_rtSubBlob[nCam][nIdx][nDefect][1].bottom; y++)
			{
				for (int x = m_rtSubBlob[nCam][nIdx][nDefect][1].left; x <= m_rtSubBlob[nCam][nIdx][nDefect][1].right; x++)
				{
					if ( *(fm2 + y * BAD_WIDTH + x) > 200)
						dPercent[2]++;
				}
			}

			m_dPercent[3]/*20220916 SJM*/ = dPercent[3] = dPercent[2] / ((m_rtSubBlob[nCam][nIdx][nDefect][1].Width() - 1) * (m_rtSubBlob[nCam][nIdx][nDefect][1].Height() - 1)) * 100;
			m_dSubPercent[nCam][nIdx][nDefect][1] = dPercent[3];
			
			double dxrratio3 = dDark_Width_Pixel / dDark_Length_Pixel;
			
			if (dxrratio1 > 5 || dxrratio2 > 4 || (dxrratio3 > 10 && m_dSubSizeY[nCam][nIdx][nDefect][1]>3) )
			{
				//220426 크랙 장단축 추가
				if (m_dSubCompactness[nCam][nIdx][nDefect][1] < 0.35 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) > 70 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < 110)
				{
					//수직
					if (fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) > 80 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < 100)
						nCrack = 2;//긁힘
					else
						nCrack = 3;//크랙
				}
				//else if (m_dSubCompactness[nCam][nIdx][nDefect][1] < 0.35 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1])>0 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < 20)//수평  라인성으로 검출되는 유형 제외 220426
				//	nCrack = 1;//찍힘
			}
			else 
			{
				//if (m_dSubCompactness[nCam][nIdx][nDefect][1] < 0.35 &&  fabs(m_dSubAngle[nCam][nIdx][nDefect][1])>0 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < 20)//수평 라인성으로 검출되는 유형 제외 220426
				//	nCrack = 0;//1;//찍힘
				//else 
					if (m_dSubCompactness[nCam][nIdx][nDefect][1] < 0.35 && /*fabs(m_dSubAngle[nCam][nIdx][nDefect][1])>20 && */fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < 70 && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCrackSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCrackSizeY[nCam]))//이외각 //220722
						nCrack = 3;//크랙
			}
		}

		//211221 개별 데이터 dark
		m_dDefectAreaSub[nCam][nIdx][nDefect][DT_DARK][0] = dBlobSize1;
		m_dDefectAreaSub[nCam][nIdx][nDefect][DT_DARK][1] = dBlobSize1 * m_dscale_x1[nCam];

		//center X
		m_dDefectCenterPosSubX[nCam][nIdx][nDefect][DT_DARK][0] = dCenterPosX_Pixel;

		//center Y
		m_dDefectCenterPosSubY[nCam][nIdx][nDefect][DT_DARK][0] = dCenterPosY_Pixel;

		//width 
		m_dDefectWidthSub[nCam][nIdx][nDefect][DT_DARK][0] = dDark_Width_Pixel * 2;			//dmaxdist값은 중심 기준으로 길이를 제공해서 X2 를 해야 전체 길이가된다. 
		m_dDefectWidthSub[nCam][nIdx][nDefect][DT_DARK][1] = (dDark_Width_Pixel * 2 * m_dscale_x1[nCam]) * 1.1;	// 좀 크게 잡아서 과검내자

		//lenght
		m_dDefectLengthSub[nCam][nIdx][nDefect][DT_DARK][0] = dDark_Length_Pixel * 2;			//dmindist값은 중심 기준으로 길이를 제공해서 X2 를 해야 전체 길이가된다.
		m_dDefectLengthSub[nCam][nIdx][nDefect][DT_DARK][1] = (dDark_Length_Pixel * 2 * m_dscale_x1[nCam]) * 1.1;	// 좀 크게 잡아서 과검내자

		//Perimeter
		m_dDefectPerimeterSub[nCam][nIdx][nDefect][DT_DARK][0] = m_pChain[nCam][nIdx][1]->Chain_Length(nBlobNo);

		//moment
		m_dElongationSub[nCam][nIdx][nDefect][DT_DARK][0] = m_pChain[nCam][nIdx][1]->FindMoment(nBlobNo);

		CRect rectRaw;
		rectRaw.left	= cpROIoffset.x + nx12;
		rectRaw.right	= cpROIoffset.x + nx11;
		rectRaw.top	= cpROIoffset.y + ny12;
		rectRaw.bottom = cpROIoffset.y + ny11;

		m_rtSubRawDefect[nCam][nIdx][nDefect][DT_DARK][0] = rectRaw;
	}
	else 
	{
		m_dSubSizeX[nCam][nIdx][nDefect][1] = 0.;
		m_dSubSizeY[nCam][nIdx][nDefect][1] = 0.;
		dstdev12 = dstdev13 = m_dSubStdValue[nCam][nIdx][nDefect][1] = m_dSubCompactness[nCam][nIdx][nDefect][1] = 0;
		m_dSubAngle[nCam][nIdx][nDefect][1] = 0;
		m_rtSubBlob[nCam][nIdx][nDefect][1].SetRect(0, 0, 0, 0);
		ptCenter[1].SetPoint(0, 0);
		dPercent[3] = m_dSubPercent[nCam][nIdx][nDefect][1] = 0;
	}

	CString strbadtype = _T("양품");
	double dratio[4] = { 0, };// x:y 비율 라인 찾기용
	
	if (m_dSubSizeX[nCam][nIdx][nDefect][0] > 0)
	{
		dratio[0] = m_dSubSizeY[nCam][nIdx][nDefect][0] / m_dSubSizeX[nCam][nIdx][nDefect][0];//white Y/X
	}
	
	if (m_dSubSizeX[nCam][nIdx][nDefect][1] > 0)
	{
		dratio[1] = m_dSubSizeY[nCam][nIdx][nDefect][1] / m_dSubSizeX[nCam][nIdx][nDefect][1];//Black Y/X
	}
	
	if (m_dSubSizeY[nCam][nIdx][nDefect][0] > 0.5)
	{
		dratio[2] = m_dSubSizeX[nCam][nIdx][nDefect][0] / m_dSubSizeY[nCam][nIdx][nDefect][0];//White X/Y
	}
	
	if (m_dSubSizeY[nCam][nIdx][nDefect][1] > 0.5)
	{
		dratio[3] = m_dSubSizeX[nCam][nIdx][nDefect][1] / m_dSubSizeY[nCam][nIdx][nDefect][1];//Black X/y
	}

//m_nBlackCut1 분화구 컷  //m_nPinholeCut 핀홀 반짝이 컷	//m_dXYRatio Y/X Ratio종횡비
	double ddist = 0;
	if (ptCenter[0].x > 0 && ptCenter[0].y > 0 && ptCenter[1].x > 0 && ptCenter[1].y > 0)
		ddist = sqrt(double((ptCenter[0].x - ptCenter[1].x)*(ptCenter[0].x - ptCenter[1].x) + (ptCenter[0].y - ptCenter[1].y)*(ptCenter[0].y - ptCenter[1].y)));

	m_dSubDistance[nCam][nIdx][nDefect][0] = ddist;
	int nBlack1 = 0, nL = 0, nR = 0, nL1 = 0, nR1 = 0, nLineWidth = 0;
	double dscalex = m_dscale_x1[nCam], dscaley = m_dscale_y1[nCam];

	/////////////////param
	double dxyratio = m_dXYRatio[nCam], dWhiteSizeX = m_dWhiteSizeX[nCam], dWhiteSizeY = m_dWhiteSizeY[nCam], dBlackSizeX = m_dDarkSizeX[nCam], dBlackSizeY = m_dDarkSizeY[nCam], dBlackPercent = m_dBlackPercent[nCam];
	int nPinholeCut = m_nPinholeCut[nCam];
	BOOL bSizeAndOr = m_bSizeAndOr[nCam];//TRUE;//사이즈 and or 조건

	m_dPeakData[nCam][nIdx][nDefect][0][2] = m_dMiddleAve[nCam];//적은 영역에서 구하고 있어서 평균으로 바꿈

																//기존 로직 삭제
	//if (nCrack == 4) {//0524-3
	//	if (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCunicSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCunicSizeY[nCam]) 
	//	{
	//		strbadtype = "찍힘"; nret = 2003;
	//	}
	//}
	if (4 == nCrack &&
		CF.isDent(m_dSubSizeX[nCam][nIdx][nDefect][1],
			m_dSubSizeY[nCam][nIdx][nDefect][1],
			0,
			m_bDentSizeAndOr,
			m_bMiniDent_ifAND[nCam],
			strbadtype,
			nret))
	{
		{
			if (nret != 2003)
				ASSERT(1);
		}
	}

	//핀홀  or 조건
	double dDistRatio1 = 0, dDistRatio2 = 0, dDistRatio3 = 0, dDistRatio4 = 0;//220426
	if (dDark_Length_Pixel > 0)
		dDistRatio4 = dDark_Width_Pixel / dDark_Length_Pixel;//211008-1

	if (dmaxdist > 0)
		dDistRatio1 = dmindist / dmaxdist;//211008-1

	if (dmindist > 0)
		dDistRatio2 = dmaxdist / dmindist;//211008-1
	
	dDistRatio3 = dDistRatio1;////211008-1
	
	double dSpecDistRatio = 3;//.5;//장단축이 3배 이상이면 라인으로 분류//211008-1
	double dlongLen1 = dmindist;
	double dlongLen2 = 16;
	
	int nOrignWV1 = m_dPeakData[nCam][nIdx][nDefect][0][0] - m_dPeakData[nCam][nIdx][nDefect][0][2];//211023 Origin Brightness
	int nOrignWV2 = m_dPeakData[nCam][nIdx][nDefect][1][0] - m_dPeakData[nCam][nIdx][nDefect][1][2];//211023 Preprocess Brightness
	int nOrignBV1 = m_dPeakData[nCam][nIdx][nDefect][0][2] - m_dPeakData[nCam][nIdx][nDefect][0][1];//211023 Origin Brightness
	int nOrignBV2 = m_dPeakData[nCam][nIdx][nDefect][1][2] - m_dPeakData[nCam][nIdx][nDefect][1][1];//211023 Preprocess Brightness

	if (dlongLen1 < dmaxdist)
		dlongLen1 = dmaxdist;
	
	if (dDistRatio3 < dDistRatio2)
		dDistRatio3 = dDistRatio2;//211008-1
															//핀홀
	if (m_bPinholeAndOr)
	{
		//////and 조건    핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 //211006
		
		if (CF.isPinhole(m_dSubSizeX[nCam][nIdx][nDefect][0],
			m_dSubSizeY[nCam][nIdx][nDefect][0],
			nOrignWV1,
			m_bPinholeAndOr,
			m_bMiniPinhole_ifAND[nCam],
			strbadtype,
			nret))
		{
			if (nret != 2001)
				ASSERT(1);
		}
		else if (CF.isProtrution(m_dSubSizeX[nCam][nIdx][nDefect][0],
				m_dSubSizeY[nCam][nIdx][nDefect][0],
				nOrignWV1,
				m_bProtrusionSizeAndOr,
				m_bMiniProtrution_ifAND[nCam],
				strbadtype,
				nret))
		{
			if (nret != 2002)
				ASSERT(1);
		}

	}
	else {//211013 최소사이즈
		if (CF.isPinhole(m_dSubSizeX[nCam][nIdx][nDefect][0],
			m_dSubSizeY[nCam][nIdx][nDefect][0],
			nOrignWV1,
			m_bPinholeAndOr,
			m_bMiniPinhole_ifAND[nCam],
			strbadtype,
			nret))
		{
			if (nret != 2001)
				ASSERT(1);
		}
		else if (CF.isProtrution(m_dSubSizeX[nCam][nIdx][nDefect][0],
			m_dSubSizeY[nCam][nIdx][nDefect][0],
			nOrignWV1,
			m_bProtrusionSizeAndOr,
			m_bMiniProtrution_ifAND[nCam],
			strbadtype,
			nret))
		{
			if (nret != 2002)
				ASSERT(1);
		}


	}
	if (m_bLineSizeAndOr) {
		int nOrignV =0;
		if (m_nLineValue[nCam] < nOrignWV1 && (m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeY[nCam]) && (dratio[0] > dxyratio || dratio[2] > dxyratio)) {
			strbadtype = "백색라인"; nret = 1002; //0608
		}
		else if (m_nLineValue[nCam] < nOrignWV1 &&(m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) {//장단축//211017
			strbadtype = "백색라인"; nret = 1002; //0608
		}
		else if (m_nLineValue[nCam] < nOrignWV1 &&dPercent[1] < 30 && (m_dSubSizeX[nCam][nIdx][nDefect][0] >= m_dWhiteLineSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] >= m_dWhiteLineSizeY[nCam]) && m_dSubCompactness[nCam][nIdx][nDefect][0] < 25) {//211017
			strbadtype = "백색라인"; nret = 1002; //0608 
		}
	}
	else {//211013 최소사이즈
		if(m_nLineValue[nCam] < nOrignWV1){
			if ( ((dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) ||(dratio[0] > dxyratio || dratio[2] > dxyratio)) && m_nLineValue[nCam] < nOrignWV1) // && )//수직이나 수평인 경우 영역내면적이커짐  주위 흑색이 있으면 안됨.
			{
				//if(nret >0 && (m_dSubSizeX[nCam][nIdx][nDefect][1] > 1 && m_dSubSizeY[nCam][nIdx][nDefect][1] > 1))
				//	;
				//else
				{
					if ((m_dSubSizeX[nCam][nIdx][nDefect][0] >= m_dLineMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] >= m_dLineMinSize[nCam]) && (m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeY[nCam]) && (dratio[0] > dxyratio || dratio[2] > dxyratio)) {
						strbadtype = "백색라인"; nret = 1002; //0608
					}
					else if ((m_dSubSizeX[nCam][nIdx][nDefect][0] >= m_dLineMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] >= m_dLineMinSize[nCam]) && (m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) {//장단축//211017
						strbadtype = "백색라인"; nret = 1002; //0608
					}
					else if ((m_dSubSizeX[nCam][nIdx][nDefect][0] >= m_dLineMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] >= m_dLineMinSize[nCam]) && dPercent[1] < 30 && m_dSubCompactness[nCam][nIdx][nDefect][0] < 25 && (m_dSubSizeX[nCam][nIdx][nDefect][0] >= m_dWhiteLineSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][0] >= m_dWhiteLineSizeY[nCam])) {//211008
						strbadtype = "백색라인"; nret = 1002; //211017 진원도 체크 추가 
					}
				}
			}
		}
	}
	if (m_nSubDefectBright[nCam][nIdx][nDefect] == 4 && m_nLineValue[nCam] < nOrignWV1) {//0618-2
		if (m_dSubSizeX[nCam][nIdx][nDefect][0] < m_dSubSizeX[nCam][nIdx][nDefect][1])m_dSubSizeX[nCam][nIdx][nDefect][0] = m_dSubSizeX[nCam][nIdx][nDefect][1];
		if (m_dSubSizeY[nCam][nIdx][nDefect][0] < m_dSubSizeY[nCam][nIdx][nDefect][1])m_dSubSizeY[nCam][nIdx][nDefect][0] = m_dSubSizeY[nCam][nIdx][nDefect][1];
		if (m_dSubSizeX[nCam][nIdx][nDefect][0] >= m_dLineMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] >= m_dLineMinSize[nCam])//211013 최소사이즈
			strbadtype = "라인"; nret = 1002;
	}
	int nCheckCrack = 0;
	double dcracksize1 = 5.1;
	if (nret == -1) {
		if (nOrignBV1 > m_nCrackValue[nCam]  && m_bCrackSizeAndOr && nCrack == 3 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < m_dCrackAngle[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= dcracksize1 && m_dSubSizeY[nCam][nIdx][nDefect][1] >= dcracksize1)) {//크랙
			nCheckCrack = 1;
		}
		if (nOrignBV1 > m_nCrackValue[nCam] && m_bCrackSizeAndOr && nCrack == 3 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < m_dCrackAngle[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCrackSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCrackSizeY[nCam])) {//크랙
			strbadtype = "크랙"; nret = 3003;
		}
		else if (nOrignBV1 > m_nCrackValue[nCam] && m_bCrackSizeAndOr == FALSE && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam]) && nCrack == 3 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < m_dCrackAngle[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCrackSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCrackSizeY[nCam])) {//크랙
			strbadtype = "크랙"; nret = 3003;//211013 최소사이즈
		}
		else if (true == CF.ScratchFilter_WidthLength((int)m_enSite, m_dDefectWidthSub[nCam][nIdx][nDefect][DT_DARK][1], m_dDefectLengthSub[nCam][nIdx][nDefect][DT_DARK][1], m_fScratchRatio[nCam]) &&
			true == CF.isScratch((int)m_enSite, m_dDefectWidthSub[nCam][nIdx][nDefect][DT_DARK][1], m_dDefectLengthSub[nCam][nIdx][nDefect][DT_DARK][1], m_dScratchTinyWidth[nCam], nOrignBV1, m_dSubAngle[nCam][nIdx][nDefect][1], nCrack, m_bScratchSizeAndOr, strbadtype, nret) )
		{
			strbadtype = "흑색긁힘"; nret = 1003;
		}
		else if (m_bDentSizeAndOr &&
			nCrack == 1 &&
			CF.isDent(m_dSubSizeX[nCam][nIdx][nDefect][1],
				(m_dSubSizeY[nCam][nIdx][nDefect][1]*2), 
				0,
				m_bDentSizeAndOr,
				m_bMiniDent_ifAND[nCam],
				strbadtype, 
				nret))
		{
			{
				m_dSubSizeY[nCam][nIdx][nDefect][1] *= 2;
				if (nret != 2003)
				ASSERT(1);
			}
		}
		else if (!m_bDentSizeAndOr &&
			nCrack == 1 && 
			CF.isDent(m_dSubSizeX[nCam][nIdx][nDefect][1], 
				(m_dSubSizeY[nCam][nIdx][nDefect][1]*2), 
				0,
				m_bDentSizeAndOr,
				m_bMiniDent_ifAND[nCam],
				strbadtype,
				nret))
		{
			{
				m_dSubSizeY[nCam][nIdx][nDefect][1] *= 2;
				if (nret != 2003)
					ASSERT(1);
			}
		}


		//else if (m_bDentSizeAndOr && nCrack == 1 && (m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dCunicSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] * 2 >= m_dCunicSizeY[nCam])) {//찍힘
		//	m_dSubSizeY[nCam][nIdx][nDefect][1] *= 2; strbadtype = "찍힘"; nret = 2003;//211017
		//}
		//else if (!m_bDentSizeAndOr && nCrack == 1 && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dDentMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dDentMinSize[nCam]) && (m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dCunicSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][1] * 2 >= m_dCunicSizeY[nCam])) {//찍힘
		//	m_dSubSizeY[nCam][nIdx][nDefect][1] *= 2; strbadtype = "찍힘"; nret = 2003;//211017 최소사이즈
		//}
	}
	if (CIS_Site::ESWA != m_enSite &&
		nret == -1 && 
		fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) > 80 &&
		m_dSubSizeX[nCam][nIdx][nDefect][0] < 0.6 && 
		m_dSubSizeY[nCam][nIdx][nDefect][0] < 0.6) 
	{	//백색 긁힘 %가 적고 사이즈가 크고 //220426 angle 85->80 

		if(nOrignBV1 > m_nScratchValue[nCam]){
			if (m_bScratchSizeAndOr && m_dSubPercent[nCam][nIdx][nDefect][0]< 20 && m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dBlackLineSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] > m_dBlackLineSizeY[nCam]) {
				strbadtype = "흑색긁힘"; nret = 1003;
			}
			else if (!m_bScratchSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dScratchMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dScratchMinSize[nCam]) && m_dSubPercent[nCam][nIdx][nDefect][0] < 20 && (m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dBlackLineSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][0] > m_dBlackLineSizeY[nCam])) {
				strbadtype = "흑색긁힘"; nret = 1003;//211013 최소사이즈
			}
			else if (m_bScratchSizeAndOr && m_dSubPercent[nCam][nIdx][nDefect][1]< 30 && m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dBlackLineSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] > m_dBlackLineSizeY[nCam]) {
				strbadtype = "흑색긁힘"; nret = 1003;
			}
			else if (!m_bScratchSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dScratchMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dScratchMinSize[nCam]) && m_dSubPercent[nCam][nIdx][nDefect][1] < 30 && (m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dBlackLineSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][1] > m_dBlackLineSizeY[nCam])) {
				strbadtype = "흑색긁힘"; nret = 1003;//211013 최소사이즈
			}
			else if (m_bScratchSizeAndOr && m_dSubPercent[nCam][nIdx][nDefect][1] < 55 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) >85 && m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dBlackLineSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] > m_dBlackLineSizeY[nCam]) {
				strbadtype = "흑색긁힘"; nret = 1003;
			}
			else if (!m_bScratchSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dScratchMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dScratchMinSize[nCam]) && m_dSubPercent[nCam][nIdx][nDefect][1] < 55 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) >85 && (m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dBlackLineSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][1] > m_dBlackLineSizeY[nCam])) {
				strbadtype = "흑색긁힘"; nret = 1003;//211013 최소사이즈
			}
		}

	}

	if (nret == -1) {
		if (m_nSubDefectBright[nCam][nIdx][nDefect] == 1) {
//			if (m_dPeakData[nCam][nIdx][nDefect][1][1] - m_dPeakData[nCam][nIdx][nDefect][1][2] < -nBlackCut1) 
			{
				if (nOrignBV1 > m_nCrackValue[nCam] && m_bCrackSizeAndOr && dPercent[3] < dBlackPercent&& fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < m_dCrackAngle[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= dcracksize1 && m_dSubSizeY[nCam][nIdx][nDefect][1] >= dcracksize1)) {
					nCheckCrack = 1;
				}
				else if (nOrignBV1 > m_nCrackValue[nCam] && !m_bCrackSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam]) && dPercent[3] < dBlackPercent&& fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < m_dCrackAngle[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= dcracksize1 || m_dSubSizeY[nCam][nIdx][nDefect][1] >= dcracksize1)) {
					nCheckCrack = 1;
				}
				if (nOrignBV1 > m_nCrackValue[nCam] && m_bCrackSizeAndOr && dPercent[3] < dBlackPercent&& fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < m_dCrackAngle[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCrackSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCrackSizeY[nCam])) {
					strbadtype = "크랙"; nret = 3003;
				}
				else if (nOrignBV1 > m_nCrackValue[nCam] && !m_bCrackSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam]) && dPercent[3] < dBlackPercent&& fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < m_dCrackAngle[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCrackSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCrackSizeY[nCam])) {
					strbadtype = "크랙"; nret = 3003;//211013 최소사이즈
				}
				else if(CF.CraterFilterMinimumGrayValue((int)m_enSite, m_dSubSizeX[nCam][nIdx][nDefect][1], m_fCraterWidth[nCam], nOrignBV1, m_fCraterMinimumGrayLevel[nCam]))
				{
					 if (CF.isCrater(m_dSubSizeX[nCam][nIdx][nDefect][1],
							m_dSubSizeY[nCam][nIdx][nDefect][1],
							nOrignBV1,
							dPercent[3],
							m_bCraterSizeAndOr,
							m_bMiniCrater_ifAND[nCam],
							strbadtype,
							nret))
						{
							if (nret != 3002)
								ASSERT(1);
						}

				}
				else
					nret = -1;//strbadtype = "백색양품";
			}
//			else	nret = -1;
		}
		else if (m_nSubDefectBright[nCam][nIdx][nDefect] == 2) {
//			if (m_dPeakData[nCam][nIdx][nDefect][1][1] - m_dPeakData[nCam][nIdx][nDefect][1][2] < -nBlackCut1) 
			{//m_dCrackAngle  //dmindist / dmaxdist
				if (nOrignBV1 > m_nCrackValue[nCam] && m_bCrackSizeAndOr && dPercent[3] < dBlackPercent && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < m_dCrackAngle[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] >=dcracksize1 && m_dSubSizeY[nCam][nIdx][nDefect][1] >= dcracksize1)) {
					nCheckCrack = 1;
				}
				else if (nOrignBV1 > m_nCrackValue[nCam] && !m_bCrackSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam]) && dPercent[3] < dBlackPercent && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < m_dCrackAngle[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= dcracksize1 || m_dSubSizeY[nCam][nIdx][nDefect][1] >= dcracksize1)) {
					nCheckCrack = 1;
				}
				if (nOrignBV1 > m_nCrackValue[nCam] && m_bCrackSizeAndOr && dPercent[3] < dBlackPercent && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < m_dCrackAngle[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCrackSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCrackSizeY[nCam])) {
					strbadtype = "크랙"; nret = 3003;
				}
				else if (nOrignBV1 > m_nCrackValue[nCam] && !m_bCrackSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam]) && dPercent[3] < dBlackPercent && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < m_dCrackAngle[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCrackSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCrackSizeY[nCam])) {
					strbadtype = "크랙"; nret = 3003;//211013 최소사이즈
				}
				else if (CF.CraterFilterMinimumGrayValue((int)m_enSite, m_dSubSizeX[nCam][nIdx][nDefect][1], m_fCraterWidth[nCam], nOrignBV1, m_fCraterMinimumGrayLevel[nCam]))
				{
					if (CF.isCrater(m_dSubSizeX[nCam][nIdx][nDefect][1],
						m_dSubSizeY[nCam][nIdx][nDefect][1],
						nOrignBV1,
						dPercent[3],
						m_bCraterSizeAndOr,
						m_bMiniCrater_ifAND[nCam],
						strbadtype,
						nret))
					{
						if (nret != 3002)
							ASSERT(1);
					}
					else 
						nret = -2;
				}
				else
					nret = -2;//strbadtype = "흑색양품";
			}
			//else
			//	nret = -2;//strbadtype = "흑색양품";
		}
		else if (m_nSubDefectBright[nCam][nIdx][nDefect] == 3)
			nret = -3;//strbadtype = "아일랜드양품";

	}
	else if (CF.CraterFilterMinimumGrayValue((int)m_enSite, m_dSubSizeX[nCam][nIdx][nDefect][1], m_fCraterWidth[nCam], nOrignBV1, m_fCraterMinimumGrayLevel[nCam]))
	{
		if (CF.isCrater(m_dSubSizeX[nCam][nIdx][nDefect][1],
			m_dSubSizeY[nCam][nIdx][nDefect][1],
			nOrignBV1,
			dPercent[3],
			m_bCraterSizeAndOr,
			m_bMiniCrater_ifAND[nCam],
			strbadtype,
			nret))
		{
			if (nret != 3002)
				ASSERT(1);
		}
		else if (CF.isCrater_Compactness(m_dSubSizeX[nCam][nIdx][nDefect][1],
			m_dSubSizeY[nCam][nIdx][nDefect][1],
			nOrignBV1,
			dPercent[3],
			m_dSubCompactness[nCam][nIdx][nDefect][1],
			m_bCraterSizeAndOr,
			m_bMiniCrater_ifAND[nCam],
			strbadtype,
			nret))
		{
			if (nret != 3002)
				ASSERT(1);
		}
	}

//		m_dSubSizeX[nCam][nIdx][nDefect][0] = m_dSubSizeX[nCam][nIdx][nDefect][1]; m_dSubSizeY[nCam][nIdx][nDefect][0] = m_dSubSizeY[nCam][nIdx][nDefect][1];
	 //Spatter 0928
	//찍힘 유형 추가
	//Compactness는 20 이하이고 면적퍼센트는 40 이상이고 각도는 5도 이내
	if(nret<=0 || nret == 3002)
	{//dDistRatio4
		if (m_dSubCompactness[nCam][nIdx][nDefect][1] < 0.2 && dPercent[3] >= 40 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) < 10 && CF.isDent(m_dSubSizeX[nCam][nIdx][nDefect][1], m_dSubSizeY[nCam][nIdx][nDefect][1], 0, m_bDentSizeAndOr, m_bMiniDent_ifAND[nCam], strbadtype, nret))
		{
				if (nret != 2003)
					ASSERT(1);
		}
		//if(m_dSubCompactness[nCam][nIdx][nDefect][1] < 0.2 && dPercent[3] >= 40 && fabs(m_dSubAngle[nCam][nIdx][nDefect][1]) <10 && m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dCunicSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCunicSizeY[nCam]) {//찍힘
		//	strbadtype = "찍힘"; nret = 2003;
		//}
	}

	int nSppaterRange1 = m_rtHugeRect[nCam].left + m_dSpatterRange[nCam][1];//0928
	int nSppaterRange2 = m_rtHugeRect[nCam].right - m_dSpatterRange[nCam][1];//0928
	if (m_bSpatterInsp[nCam] && (m_rtSubDefect[nCam][nIdx][nDefect].right < nSppaterRange1 || m_rtSubDefect[nCam][nIdx][nDefect].left > nSppaterRange2)) {
		//
		if (m_bSpatterSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dSpatterSizeX[nCam][0] && m_dSubSizeY[nCam][nIdx][nDefect][0] > m_dSpatterSizeY[nCam][0]) && m_nSpatterTh[nCam][0] < m_dPeakData[nCam][nIdx][nDefect][0][0] - m_dPeakData[nCam][nIdx][nDefect][0][2]) {//백색 불량
																																																																				   //Spatter white
			strbadtype = "SpatterW"; nret = 8002;
		}
		else if (!m_bSpatterSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][0] >= m_dSpatterMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] >= m_dSpatterMinSize[nCam]) && (m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dSpatterSizeX[nCam][0] || m_dSubSizeY[nCam][nIdx][nDefect][0] > m_dSpatterSizeY[nCam][0]) && m_nSpatterTh[nCam][0] < m_dPeakData[nCam][nIdx][nDefect][0][0] - m_dPeakData[nCam][nIdx][nDefect][0][2]) {//백색 불량
																																																																																																							   //Spatter white
			strbadtype = "SpatterW"; nret = 8002;//211013 최소사이즈
		}
		if (m_bSpatterSizeAndOr && m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dSpatterSizeX[nCam][1] && m_dSubSizeY[nCam][nIdx][nDefect][1] > m_dSpatterSizeY[nCam][1] && m_nSpatterTh[nCam][1] < m_dPeakData[nCam][nIdx][nDefect][0][2] - m_dPeakData[nCam][nIdx][nDefect][0][1]) {//흑색 불량
																																																																				 //Spatter black
			strbadtype = "SpatterB"; nret = 8002;
		}
		else if (!m_bSpatterSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][0] >= m_dSpatterMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] >= m_dSpatterMinSize[nCam]) && m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dSpatterSizeX[nCam][1] || m_dSubSizeY[nCam][nIdx][nDefect][1] > m_dSpatterSizeY[nCam][1] && m_nSpatterTh[nCam][1] < m_dPeakData[nCam][nIdx][nDefect][0][2] - m_dPeakData[nCam][nIdx][nDefect][0][1]) {//흑색 불량
																																																																																																							 //Spatter black
			strbadtype = "SpatterB"; nret = 8002;//211013 최소사이즈
		}
	}

	if(nCheckCrack==1){
		
		int res = CrackRecheck( fmPre,  nCam,  nIdx,  nDefect, ptCenter[1].x+m_rtSubDefect[nCam][nIdx][nDefect].left, ptCenter[1].y+m_rtSubDefect[nCam][nIdx][nDefect].top);
		if (0 != res)
			nret = res;
	}
	delete[]fm1;
	delete[]fm2;
	delete[]fm4;


	if (0 > nret)
	{
		CF.GetResult_Mini(nret, strbadtype, nret);
	}
	
	//if (-1 == nret && TRUE == m_bMiniPinhole[nCam])
	//{
	//	if ((m_dSubSizeX[nCam][nIdx][nDefect][0] >= m_dMiniPinhole_SizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] >= m_dMiniPinhole_SizeY[nCam]) && nOrignWV1 >= nPinholeCut) {
	//		strbadtype = "미니핀홀"; nret = 2000; //0608
	//	}
	//}
	//if (-2 == nret && TRUE == m_bMiniCrater[nCam])
	//	if ((m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dMiniCrater_SizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dMiniCrater_SizeY[nCam]) && dPercent[3] > dBlackPercent && nOrignBV1 > m_nBlackCut1[nCam] && m_dSubCompactness[nCam][nIdx][nDefect][1] > 0.5) {
	//		strbadtype = "미니분화구"; nret = 3000; //0608
	//	}

	return nret;
}

int	CInspect::DualLineBlob(BYTE * fmOrigin, int nCam, int nIdx, int nDefect, int nBW)//nbw 3 무지
{
	Classification CF;
	CF.SetScratchSpec(m_dBlackLineSizeX[nCam],
		m_dBlackLineSizeY[nCam],
		m_dScratchMinSize[nCam],
		m_dScratchMinSize[nCam],
		-1,
		85);


	//블랍은 듀얼로 한다. 원이미지를 base로 전처리후 블랍을 한다.
	int nw1 = BAD_WIDTH, nh1 = BAD_HEIGHT;
	BYTE * fm = m_pSubDefet[nCam][nIdx][nDefect][1];//1차 전처리 이미지
	BYTE * fm3 = m_pSubDefet[nCam][nIdx][nDefect][0];//origin
	BYTE * fm1 = new BYTE[BAD_WIDTH*BAD_HEIGHT];//white
	BYTE * fm2 = new BYTE[BAD_WIDTH*BAD_HEIGHT];//black
	BYTE * fm4 = new BYTE[BAD_WIDTH*BAD_HEIGHT];//black+white
	CRect rt2 = m_rtSubDefect[nCam][nIdx][nDefect], rt1, rt;
	rt.SetRect(0, 0, BAD_WIDTH, BAD_HEIGHT);

	///////////////원본 전처리
	int nTmp = 0, nThw = m_nLineWth[nCam], nThb = m_nLineBth[nCam];//pDoc->m_data.m_dThDn[nCam];
	int nCount1 = 0, nImageProjectiony[BAD_HEIGHT] = { 0, }, nProjY1[BAD_HEIGHT] = { 0, };
	for (int y = rt.top; y < rt.bottom; y++) {
		nCount1 = 0;
		for (int x = rt.left; x < rt.right; x++) {
			nImageProjectiony[y] += *(fm3 + nw1*y + x);
			nCount1++;
		}
		if(nCount1<1)nCount1 = 1;
		nImageProjectiony[y] /= nCount1;
	}
	double dmag1 = m_nLineMag[nCam] * 0.5, dmag2;
	for (int y = rt.top; y < rt.bottom; y++) {
		for (int x = rt.left; x < rt.right; x++) {
			if ((*(fm3 + nw1*y + x) - nImageProjectiony[y]) > m_nTh1[nCam] || (*(fm3 + nw1*y + x) - nImageProjectiony[y]) < -m_nTh2[nCam])
				dmag2 = m_nLineMag[nCam];
			else
				dmag2 = dmag1;
			nTmp = m_nTargetBrightness[nCam] + (*(fm3 + nw1*y + x) - nImageProjectiony[y])*dmag2;
			if (nTmp < 0) nTmp = 0;
			else if (nTmp > 255) nTmp = 255;
			*(fm + y*nw1 + x) = nTmp;//전처리 영상
			nProjY1[y] += nTmp;
		}
	}
	if(rt.Width()>0){
		for (int y = rt.top; y < rt.bottom; y++)
			nProjY1[y] /= rt.Width();
	}
	int nmarginb = m_nMargin[nCam][1];//흣pDoc->m_pDlgInterface->m_nThMargin;
	int nmarginw = m_nMargin[nCam][0];//백
	int nthw1 = nThw - nmarginw, nthb1 = nThb - nmarginb;//size를 재기위해 마진을 더둔다.
	int ngap = 1;
	int nsum = 0, nsum1 = 0, npeakValue[3] = { 0,1000,0 }, ncnt1 = 0;
	int nret = -1;

	for (int y = rt.top; y < rt.bottom; y++) {
		for (int x = rt.left; x < rt.right; x++) {
			if (y <= ngap || y >= BAD_HEIGHT - 1 - ngap || x <= rt.left + ngap || x >= rt.right - 1 - ngap)
				*(fm1 + nw1*y + x) = *(fm2 + nw1*y + x) = *(fm4 + nw1*y + x) = 0;//테두리를 흑색으로 치환한다. blob에서 에지가 걸리면 검출이 안된.
			else {
				rt1.SetRect(x - ngap, y - ngap, x + ngap, y + ngap);
				nsum = AreaAve1(fm, rt1, BAD_WIDTH, &nsum1);
				*(fm1 + nw1*y + x) = 0;
				*(fm2 + nw1*y + x) = 0;
				*(fm4 + nw1*y + x) = 0;
				if (nsum - nProjY1[y] > nThw) {
					*(fm4 + nw1*y + x) = *(fm1 + nw1*y + x) = 255;
				}
				else if (nsum - nProjY1[y] < -nThb) {
					*(fm2 + nw1*y + x) = 255;
					*(fm4 + nw1*y + x) = 128;
				}

				ncnt1++;
			}
		}
	}
	//	npeakValue[2] /= ncnt1;
	// 	m_dPeakData[nCam][nIdx][nDefect][0][2] /= ncnt1;//오리진 이미지
	// 	m_dPeakData[nCam][nIdx][nDefect][1][2] /= ncnt1;//전처리 이미지

	if (m_bDefectBinary)
		memcpy(m_pSubDefet[nCam][nIdx][nDefect][0], fm4, BAD_WIDTH*BAD_HEIGHT);
	//Blob white
	CPoint ptCenter[4];
	for (int i = 0; i < 4; i++)ptCenter[i].SetPoint(0, 0);
	int  nBlobCount = -1, nBlobNo = 0;
	double dPercent[4] = { 0, }, dTemp = 0, dBlobSize = 0, dBlobSize1 = 0;//0 count 1 percent 2 count 3 percent
	CRect rtBlob;
	m_pChain[nCam][nIdx][0]->SetChainData(1, fm1, 1, 1, 3, 100000, BAD_WIDTH, BAD_HEIGHT);//white
	nBlobCount = m_pChain[nCam][nIdx][0]->FastChain(0, 0, BAD_WIDTH - 1, BAD_HEIGHT - 1);
	for (int i = 0; i < nBlobCount; i++)
	{
		dTemp = m_pChain[nCam][nIdx][0]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp < 2)continue;
		if (dTemp >= dBlobSize)//최대 불량 사이즈.
		{
			nBlobNo = i;	dBlobSize = dTemp;//면적
		}
	}
	double dcx = 0, dcy = 0, tmp = 0, dSum1 = 0, dSum2 = 0, dstdev2 = 0, dstdev3 = 0, tmp1 = 0, dSum11 = 0, dSum12 = 0, dstdev12 = 0, dstdev13 = 0, dstdev22 = 0, dstdev23 = 0;
	CString strbadtype;
	int nTmp11 = 0, nBlack1 = 0, nL = 0, nR = 0, nL1 = 0, nR1 = 0;
	if (nBlobCount > 0) {
		//white //		if(pDoc->m_data.m_dSubSize[nCam][nIdx][nDefect][0]>pDoc->m_data.m_dSubSize[nCam][nIdx][nDefect][1]){
		m_dSubAngle[nCam][nIdx][nDefect][0] = m_pChain[nCam][nIdx][0]->FindAngle(nBlobNo);
		m_nSubDefectBright[nCam][nIdx][nDefect] = 1;
		if (dBlobSize >= 15) {
			dcx = 0, dcy = 0;
			m_dSubSizeX[nCam][nIdx][nDefect][0] = (m_pChain[nCam][nIdx][0]->FindMaxX(nBlobNo) - m_pChain[nCam][nIdx][0]->FindMinX(nBlobNo))*m_dscale_x1[nCam];
			m_dSubSizeY[nCam][nIdx][nDefect][0] = ((m_pChain[nCam][nIdx][0]->FindMaxY(nBlobNo) - m_pChain[nCam][nIdx][0]->FindMinY(nBlobNo))*m_dscale_y1[nCam]);
			int nmid1 = (m_pChain[nCam][nIdx][0]->FindMaxX(nBlobNo) + m_pChain[nCam][nIdx][0]->FindMinX(nBlobNo))*0.5;
			int nmidy = (m_pChain[nCam][nIdx][0]->FindMaxY(nBlobNo) + m_pChain[nCam][nIdx][0]->FindMinY(nBlobNo))*0.5;
			int nc11 = 0;

			//중심의 세로 10픽셀의 폭을 구한다.//GetLeft_RightBoundary(int N, int YValue, int *left, int *right)
			if ((m_bLineSizeAndOr && m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeY[nCam])
				|| (!m_bLineSizeAndOr && m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][0] > m_dWhiteLineSizeY[nCam]))
			{
				strbadtype = "백색라인"; nret = 1002;
				if (m_dSubSizeX[nCam][nIdx][nDefect][0] > m_dLargeLineSizeX[nCam]) {//거대라인
					strbadtype = "거대라인"; nret = 1001;
				}
				else if (m_dSubSizeX[nCam][nIdx][nDefect][0] > 1) {// pDoc->m_pDlgInterface->m_dWrinkleBlackSize){//열주름 체크
					for (int k1 = 5; k1 < BAD_HEIGHT - 5; k1++) {
						//				for(int k1=nmidy-40;k1<nmidy+40;k1++){
						if (k1 < 0)k1 = 0; if (k1 > BAD_HEIGHT - 1)continue;
						m_pChain[nCam][nIdx][0]->GetLeft_RightBoundary(nBlobNo, k1, &nL, &nR);//진짝폭
						nL1 += nL; nR1 += nR;//폭 누적
						for (int x10 = nL + 2; x10 < nR - 5; x10++) {//
							if (*(fm + nw1*k1 + x10) - *(fm + nw1*k1 + x10 + 4) > 15) {
								*(fm4 + nw1*k1 + x10 + 4) = 0;
								nBlack1++;//break;//
							}
						}
						nc11++;
					}
					if (nBlack1 > m_nLineInBlackCount[nCam]) {
						strbadtype = "열주름"; nret = 1005;
					}

				}
			}
		}
	}
	else {//black
		m_nSubDefectBright[nCam][nIdx][nDefect] = 2;
		m_pChain[nCam][nIdx][1]->SetChainData(1, fm2, 1, 1, 8, 100000, BAD_WIDTH, BAD_HEIGHT);//white
		nBlobCount = m_pChain[nCam][nIdx][1]->FastChain(0, 0, BAD_WIDTH - 1, BAD_HEIGHT - 1);
		dBlobSize1 = dTemp = nBlobNo = 0;
		for (int i = 0; i < nBlobCount; i++)
		{
			dTemp = m_pChain[nCam][nIdx][1]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp < 2)continue;
			if (dTemp >= dBlobSize1)//최대 불량 사이즈.
			{
				nBlobNo = i;
				dBlobSize1 = dTemp;//면적
			}
		}
		if (dBlobSize1 >= 20) {
			dcx = 0, dcy = 0;
			m_dSubSizeX[nCam][nIdx][nDefect][1] = (m_pChain[nCam][nIdx][1]->FindMaxX(nBlobNo) - m_pChain[nCam][nIdx][1]->FindMinX(nBlobNo) + 1)*m_dscale_x1[nCam];
			m_dSubSizeY[nCam][nIdx][nDefect][1] = (m_pChain[nCam][nIdx][1]->FindMaxY(nBlobNo) - m_pChain[nCam][nIdx][1]->FindMinY(nBlobNo) + 1)*m_dscale_y1[nCam];
			if (m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dSubSizeY[nCam][nIdx][nDefect][1])m_dSubSize[nCam][nIdx][nDefect][1] = m_dSubSizeX[nCam][nIdx][nDefect][1];
			else  m_dSubSize[nCam][nIdx][nDefect][1] = m_dSubSizeY[nCam][nIdx][nDefect][1];
			m_dSubAngle[nCam][nIdx][nDefect][1] = m_pChain[nCam][nIdx][1]->FindAngle(nBlobNo);
			for (int y = m_rtSubBlob[nCam][nIdx][nDefect][1].top; y <= m_rtSubBlob[nCam][nIdx][nDefect][1].bottom; y++) {
				for (int x = m_rtSubBlob[nCam][nIdx][nDefect][1].left; x <= m_rtSubBlob[nCam][nIdx][nDefect][1].right; x++) {
					if (*(fm2 + y*BAD_WIDTH + x) > 200)
						dPercent[2]++;
				}
			}
			dPercent[3] = dPercent[2] / ((m_rtSubBlob[nCam][nIdx][nDefect][1].Width() - 1)*(m_rtSubBlob[nCam][nIdx][nDefect][1].Height() - 1)) * 100;

			if (true == CF.ScratchFilter_WidthLength((int)m_enSite, m_dDefectWidthSub[nCam][nIdx][nDefect][DT_DARK][1], m_dDefectLengthSub[nCam][nIdx][nDefect][DT_DARK][1], m_fScratchRatio[nCam]) &&
				true == CF.isScratch((int)m_enSite, m_dDefectWidthSub[nCam][nIdx][nDefect][DT_DARK][1], m_dDefectLengthSub[nCam][nIdx][nDefect][DT_DARK][1], m_dScratchTinyWidth[nCam], 9999, m_dSubAngle[nCam][nIdx][nDefect][1], 2, m_bScratchSizeAndOr, strbadtype, nret))
			{
				strbadtype = "흑색긁힘"; nret = 1003;
			}
			else if (m_bCrackSizeAndOr && dPercent[3] < m_dBlackPercent[nCam] && m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dCrackSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] > m_dCrackSizeY[nCam]) {
				strbadtype = "흑색 크랙"; nret = 3003;
			}
			else if (!m_bCrackSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCracknMinSize[nCam]) && dPercent[3] < m_dBlackPercent[nCam] && (m_dSubSizeX[nCam][nIdx][nDefect][1] > m_dCrackSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] > m_dCrackSizeY[nCam])) {
				strbadtype = "흑색 크랙"; nret = 3003;//211013 최소사이즈
			}
		}
	}

	if (nret == -1) {
		if (m_nSubDefectBright[nCam][nIdx][nDefect] == 1)
			nret = -1;//strbadtype = "백색양품";
		else if (m_nSubDefectBright[nCam][nIdx][nDefect] == 2)
			nret = -2;//strbadtype = "흑색양품";
		else if (m_nSubDefectBright[nCam][nIdx][nDefect] == 3)
			nret = -3;//strbadtype = "아일랜드양품";
	}

	delete[]fm1;
	delete[]fm2;
	delete[]fm4;

	return nret;
}




void CInspect::ValidRect(CRect *rect,int nCam,int nflag)
{
// 	int m_nHeight = 0;
// 	int m_nPitch = 0;
// 
// 	m_nPitch = CAM_LINE_WIDTH_8192;
// 	m_nHeight = CAM_LINE_HEIGHT_3072;
// 
// 	CRect rectImg(0,0,m_nPitch, m_nHeight);
// 
// 	if(rect->left	< 0)			rect->OffsetRect(-rect->left, 0);
// 	if(rect->top	< 0)			rect->OffsetRect(0, -rect->top);
// 	if(nflag)
// 	{
// 		if(rect->right	>= m_nPitch)	rect->OffsetRect(m_nPitch-rect->right, 0);
// 		if(rect->bottom >= m_nHeight)	rect->OffsetRect(0, m_nHeight-rect->bottom);
// 	}
// 	else
// 	{
// 		if(rect->right	>= m_nPitch*2)	rect->OffsetRect(m_nPitch*2-rect->right, 0);
// 		if(rect->bottom >= m_nHeight*2)	rect->OffsetRect(0, m_nHeight*2-rect->bottom);
// 	}
}


CRect CInspect::SetBoundary(CRect rt,int nwidth,int nh1,int nh2,int *nx1,int *ny1,int* nx2,int * ny2)
{
	if(rt.left	< 0)			rt.OffsetRect(-rt.left, 0);
	if(rt.top	< nh1)			rt.OffsetRect(0, nh1-rt.top);
	if(rt.right	>= nwidth)		rt.OffsetRect(nwidth-rt.right, 0);
	if(rt.bottom >= nh2)	rt.OffsetRect(0, nh2-rt.bottom);
	*nx1 = rt.left;
	*ny1 = rt.top;
	*nx2 = rt.right;
	*ny2 = rt.bottom;
	return rt;
}

CRect CInspect::SetBoundary1(CRect rt,int nwidth,int nh1,int nh2)
{	
	if(rt.left	< 0)			rt.OffsetRect(-rt.left, 0);
	if(rt.top	< nh1)			rt.OffsetRect(0, nh1-rt.top);
	if(rt.right	>= nwidth)		rt.OffsetRect(nwidth-rt.right, 0);
	if(rt.bottom >= nh2)	rt.OffsetRect(0, nh2-rt.bottom);
	return rt;
}

CRect CInspect::SetBoundary2(CRect rt,CRect rt2)
{	
	if(rt.left	< rt2.left)	
		rt.OffsetRect((rt2.left-rt.left), 0);
	if(rt.top	< rt2.top)			rt.OffsetRect(0, rt2.top-rt.top);
	if(rt.right	>= rt2.right)		rt.OffsetRect(rt2.right-rt.right, 0);
	if(rt.bottom >= rt2.bottom)	rt.OffsetRect(0, rt2.bottom-rt.bottom);
	return rt;
}


//fmSrc 영상에서 fmDest영상으로 rtArea부분만큼만 복사한다.
void CInspect::ImageSelectCopy(unsigned char* fmSrc,unsigned char* fmDest,int nSrcW,int nSrcH,CRect rtArea,int nDestW,int nDestH,int nDestSttX,int nDestSttY)
{
	int x,y;
	CPoint ptOffset(0,0);
//	rtArea = pData->m_rectDefect[0][pData->m_nRealDefectCount];
	CRect rt1 = rtArea;
	if(rt1.Width()>nDestW || rt1.Height()>nDestH)	return;
	if(rt1.left<0)			ptOffset.x = abs(rt1.left);
	if(rt1.right>=nSrcW)		ptOffset.x = nSrcW-rt1.right-1;
	if(rt1.top<0)			ptOffset.y = abs(rt1.top);
	if(rt1.bottom>=nSrcH)	ptOffset.y = nSrcH-rt1.bottom-1;
	rt1.OffsetRect(ptOffset);

	if (rt1.bottom >= nSrcH) rt1.OffsetRect(0, nSrcH - rt1.bottom - 1);//0611
	for (int j = rt1.top; j < rt1.bottom && j >= 0 && j < m_nHeight && j < nSrcH; j++)//211103   //rt1에 음수값이 들어감
	{
		for (int i = rt1.left; i < rt1.right && i >= 0 && i < m_nWidth && i < nSrcW; i++)//211103
		{
			x = (nDestSttX + i - rtArea.left);
			y = (nDestSttY + j - rtArea.top);
			if (0 <= x && x < nDestW && 0 <= y && y < nDestH)
				fmDest[x + nDestW * y] = fmSrc[i + nSrcW * j];
		}
	}
}

void CInspect::ImageSelectCopy1(unsigned char* fmSrc,unsigned char* fmDest,int nSrcW,int nSrcH,CRect rtArea,int nDestW,int nDestH,int nDestSttX,int nDestSttY,int nReduce,BOOL bColor)
{
	int x,y;
	CPoint ptOffset(0,0);
//	if(rtArea.Width()>nDestW || rtArea.Height()>nDestH)	return;
	if(rtArea.left<0)			ptOffset.x = abs(rtArea.left);
	if(rtArea.right>=nSrcW)		ptOffset.x = nSrcW-rtArea.right-1;
	if(rtArea.top<0)			ptOffset.y = abs(rtArea.top);
	if(rtArea.bottom>=nSrcH)	ptOffset.y = nSrcH-rtArea.bottom-1;
	rtArea.OffsetRect(ptOffset);

	int ntmp1 = 1000;
	for(int j=rtArea.top;j<rtArea.bottom;j+=nReduce)
		for(int i=rtArea.left;i<rtArea.right;i+=nReduce)
		{
			x = (nDestSttX+i-rtArea.left)/nReduce;
			y = (nDestSttY+j-rtArea.top)/nReduce;
			if(0<=x && x<nDestW && 0<=y && y<nDestH){
				if(bColor){//white
					ntmp1 = 0;
					for(int y1 = j;y1<j+nReduce;y1++){
						for(int x1 = i;x1<i+nReduce;x1++){
							if(fmSrc[x1+nSrcW*y1]>ntmp1)
								ntmp1 = fmSrc[x1+nSrcW*y1];
						}	
					}
				}
				else{//black
					ntmp1 = 1000;
					for(int y1 = j;y1<j+nReduce;y1++){
						for(int x1 = i;x1<i+nReduce;x1++){
							if(fmSrc[x1+nSrcW*y1]<ntmp1)
								ntmp1 = fmSrc[x1+nSrcW*y1];
						}	
					}

				}
				fmDest[x+nDestW*y] = ntmp1;
			}
		}
}

int CInspect::AreaAve1(BYTE * fm,CRect rt,int nw, int * nave)
{
	double dsum=0,dc=0,dsum1=0,dc1=0;

	for(int y=rt.top,y1=0;y<rt.bottom ;y++,y1++){
		for(int x=rt.left,x1=0;x<rt.right;x++,x1++){
			dsum += *(fm+y*nw+x);dc++;
			if(x1>0&&x1<4&&y1>0&&y1<4){	dsum1 += *(fm+y*nw+x);dc1++;}
		}
	}
	*nave = int(dsum1/dc1);
	return int(dsum/dc);
}

int CInspect::FindHugeDefect(int nCam, BYTE * fmsrc, int nTabPos)
{
	m_dCellTopBottmAve[nCam][0] = m_dCellTopBottmAve[nCam][1] = m_dCellTopBottmAve[nCam][2] = 0;
	m_ptTabCenter[nCam].SetPoint(0, 0);//211009
	int nret = 0;
	for (int i = 0; i < MAX_HUGEDEFECT; i++) {
		m_dHugeDefectSizeX[nCam][i][0] = m_dHugeDefectSizeX[nCam][i][1] = 0;//
		m_dHugeDefectSizeY[nCam][i][0] = m_dHugeDefectSizeY[nCam][i][1] = 0;
		m_dBlobCenter1[nCam][i] = 0;
	}
	m_rtHugeRect[nCam].SetRect(0, 0, 0, 0);
	m_rtMinTab[nCam].SetRect(0, 0, 0, 0);
	m_nDefaultBright[nCam][0] = 0;
	int nProj[2000] = { 0, };// = new int[sizeof(int)*m_nWidthMini];	memset(nProj,0,sizeof(int)*m_nWidthMini);//220328
	BYTE * fm = new BYTE[sizeof(BYTE)*(m_nWidthMini*m_nHeightMini)];
	BYTE * fm1 = m_fmCompress[nCam];//m_fmCompress[nCam];
	BYTE * fm2 = m_fmCompressDark[nCam];//m_fmCompress[nCam];

//	BYTE * fmW = m_fmcompressWhite[nCam];//new BYTE[sizeof(BYTE)*(m_nWidthMini*m_nHeightMini)];
//	BYTE * fmb = m_fmcompressBlack[nCam];//new BYTE[m_nWidthMini*m_nHeightMini];
	BYTE * fmW = new BYTE[sizeof(BYTE)*(m_nWidthMini*m_nHeightMini)];
	BYTE * fmb = new BYTE[m_nWidthMini*m_nHeightMini];
	memset(fmW,0,sizeof(BYTE)*(m_nWidthMini*m_nHeightMini));//220124
	memset(fmb,0,sizeof(BYTE)*(m_nWidthMini*m_nHeightMini));//220124
										 //	memset(fmW,0,m_nWidthMini*m_nHeightMini);	memset(fmb,0,m_nWidthMini*m_nHeightMini);
	CRect rtins(m_rtFullRange[nCam].left / m_nReduction[nCam], 0, m_rtFullRange[nCam].right / m_nReduction[nCam], m_nHeightMini);
	CRect rt[10], rtCheck[2];//
	rtCheck[0].SetRect(m_nWidthMini*0.5 - 20, 0, m_nWidthMini*0.5 + 20, m_nHeightMini);//211011
	int cnt12 = 0;
	//백색 count만 한다. 이미지에 탭이 있는지 체크한다.
	int nWhiteCount = 0, nWhiteVPos[2] = { 0, }; //211008-2
	int nSplitcnt = m_nHeightMini / 3; //211017-1
	int ntopEdge1[1000] = { 0, }, nbtmEdge1[1000] = { 0, };
	int nLREdgeTop[1000]={0,},nLREdgeBtm[1000]={0,};
	BOOL bRollTest = m_bWhiteRoll[nCam];//1;//211028

	int nImageAverage[2] ={0,},navecnt1=0;//211102  Bright Dark Image Ave
	int nHisto1[256]={0,};//211101
	int nFullAreaAve = 0,nFullCnt=0;//211106
	int nFullWhiteCnt = 0,nFullWhiteCut=235;//211106
	if (bRollTest == TRUE) {//211028
		memcpy(fm,m_fmCompressDark[nCam],m_nWidthMini*m_nHeightMini);
		//		fm = m_fmCompressDark[nCam];
	}
	else{
		memcpy(fm,m_fmCompress[nCam],m_nWidthMini*m_nHeightMini);
	}

	if(bRollTest){
		for (int y = 0; y < m_nHeightMini; y++) {
			for (int x = 0; x < m_nWidthMini; x++) {
				if( *(m_fmCompressDark[nCam] + y*m_nWidthMini + x) >= nFullWhiteCut)//테이프 유출 방지를 위해 백색의 밝기만 뽑는다.
					nFullWhiteCnt++;
				nProj[x] += *(m_fmCompressDark[nCam] + y*m_nWidthMini + x);
				if (y < 10)
					ntopEdge1[x] += *(m_fmCompressDark[nCam] + y*m_nWidthMini + x);
				else if (y >= m_nHeightMini - 10)
					nbtmEdge1[x] += *(m_fmCompressDark[nCam] + y*m_nWidthMini + x);
				if (x >= rtCheck[0].left && x < rtCheck[0].right)
				{
					int nmod = y / nSplitcnt;
					if (nmod >= 3)continue;
					m_dCellTopBottmAve[nCam][nmod] += *(m_fmCompressDark[nCam] + y*m_nWidthMini + x);
					nImageAverage[0] += *(fmsrc + y*m_nWidthMini + x);//220328
					nImageAverage[1] += *(m_fmCompressDark[nCam] + y*m_nWidthMini + x);//211102
					navecnt1++;
					nHisto1[*(m_fmCompressDark[nCam] + y*m_nWidthMini + x)]++;//211101
				}
			}
			cnt12++;
		}
	}
	else{
		for (int y = 0; y < m_nHeightMini; y++) {
			for (int x = 0; x < m_nWidthMini; x++) {
				if( *(fm + y*m_nWidthMini + x) >= nFullWhiteCut)//테이프 유출 방지를 위해 백색의 밝기만 뽑는다.
					nFullWhiteCnt++;
				nProj[x] += *(fm + y*m_nWidthMini + x);
				if (y < 10)
					ntopEdge1[x] += *(fm + y*m_nWidthMini + x);
				else if (y >= m_nHeightMini - 10)
					nbtmEdge1[x] += *(fm + y*m_nWidthMini + x);
				if (x >= rtCheck[0].left && x < rtCheck[0].right)
				{
					int nmod = y / nSplitcnt;
					if (nmod >= 3)continue;
					m_dCellTopBottmAve[nCam][nmod] += *(fm + y*m_nWidthMini + x);
					nImageAverage[0] += *(fmsrc + y*m_nWidthMini + x);//220328
					nImageAverage[1] += *(m_fmCompressDark[nCam] + y*m_nWidthMini + x);//211102
					navecnt1++;
					nHisto1[*(fm + y*m_nWidthMini + x)]++;//211101
				}
			}
			cnt12++;
		}
	}

	if(rtCheck[0].Width()*nSplitcnt>0){
		for (int n = 0; n < 3; n++) {
			m_dCellTopBottmAve[nCam][n] /= (rtCheck[0].Width()*nSplitcnt);
		}
	}
	if(navecnt1==0)navecnt1 = 1;
	nImageAverage[0] /= navecnt1;//bright
	nImageAverage[1] /= navecnt1;//dark

	m_dMiddleAveDark[nCam] = nImageAverage[1];//220110
	int nRightLimit1 = m_nMarginSearchEnd[nCam] / m_nReduction[nCam]; //211104
	int nRightLimit2 = 0,nRightLimit3=0;
	int nstop = 0;
	int nMargin = m_nMarginSearchStart[nCam];// nstop;// 50 / m_nReduction;//31;//6;//처음 영상에 흰색이 있을경우
	nstop = nMargin / m_nReduction[nCam];	//좌측 검사 시작 영역
	int nStart = nProj[nstop + 2], nEdgeGap = m_nDefaultBright[nCam][0] + 15, nEdgeTh = abs(m_nDefaultBright[nCam][0] - nStart)*0.5;
	int nRightSkip = 16;//기존 32 //0921
	int nEnd1 = m_nMarginSearchEnd[nCam]/ m_nReduction[nCam];//211101  우측 검사 스킵영역


	//roll 좌우의 밝기를 뽑는다
	int nRollLRAve[2] = {0,};
	for (int y = 0; y < m_nHeightMini; y++) {
		nRollLRAve[0] += *(fm + y*m_nWidthMini + nstop);//좌측 롤 밝기
		nRollLRAve[1] += *(fm + y*m_nWidthMini + nRightLimit1);//우측 롤밝기
	}
	if (m_nHeightMini > 0)
	{
		nRollLRAve[0] /= m_nHeightMini;
		nRollLRAve[1] /= m_nHeightMini;
	}
//우측엔 탭이 포함될수 있으므로 다시한번 찾는다.
	if(nRollLRAve[1] > 180){//우측이 롤에 오염이 된경우 한번더 검사한다.
		int ntmpbright1 = nRollLRAve[1], ntmpcnt1=0;
		nRollLRAve[1] = 0;
		for (int y = 0; y < m_nHeightMini; y++) {
			if(*(fm + y*m_nWidthMini + nRightLimit1)< ntmpbright1){
				nRollLRAve[1] += *(fm + y*m_nWidthMini + nRightLimit1);
				ntmpcnt1++;
			}
		}
		if(ntmpcnt1<=0)ntmpcnt1 = 1;
		nRollLRAve[1] /= ntmpcnt1;
	}
	if(cnt12<1)cnt12=1;
	for (int x = 0; x < m_nWidthMini; x++) {
		nProj[x] /= cnt12;// (m_nHeightMini / 2);
		ntopEdge1[x] /= 10;
		nbtmEdge1[x] /= 10;
		if (x >= rtCheck[0].left && x < rtCheck[0].right){
			m_nDefaultBright[nCam][0] += nProj[x];
		}
		if(x>= nstop && x<nEnd1){
			nFullAreaAve += nProj[x];//211105
			nFullCnt++;
		}
	}
	int nRightStop = (m_nMarginSearchEnd[nCam] / m_nReduction[nCam]) - (m_nRightElecEdgeSearchAreaSizeX[nCam] / m_nReduction[nCam]); //220225
	int nLeftStop = (m_nMarginSearchStart[nCam] / m_nReduction[nCam]) + (m_nLeftElecEdgeSearchAreaSizeX[nCam] / m_nReduction[nCam]); //220225
	for(int x = nRightLimit1;x>nRightStop;x--){
		if(nProj[x] - nProj[x-4] > nRightLimit2){
			nRightLimit2 = nProj[x] - nProj[x-4];nRightLimit3 = x;
		}
		if(nRightLimit2 > m_nEdgeTh[nCam] )
			break;
	}
	if(nFullCnt>0)
		nFullAreaAve /= nFullCnt;//211105

	if(rtCheck[0].Width()>0)
		m_nDefaultBright[nCam][0] /= rtCheck[0].Width();//중앙 코팅 밝기
//211105중심에 까만색만 있는경우를 체크하기 위해
	for (int x = 4; x < m_nWidthMini-4; x++) {
		nLREdgeTop[x] = abs(ntopEdge1[x-4] - ntopEdge1[x]) + abs(ntopEdge1[x+4] - ntopEdge1[x]);
		nLREdgeBtm[x] = abs(nbtmEdge1[x-4] - nbtmEdge1[x]) + abs(nbtmEdge1[x+4] - nbtmEdge1[x]);
	}
	////평균의 아래에 흑색이 모인점을 찾는다.
	int nHistomax1 = 0,nHistomax2=0;
	for(int i=0;i<m_nDefaultBright[nCam][0]-5;i++){
		if(nHistomax1 < nHisto1[i]){
			nHistomax1 = nHisto1[i];
			nHistomax2 = i;
		}
	}
	//전극이 더 밝은 경우
	for (int x = 2; x < nLeftStop; x++) {//220225
		if (nProj[x] < 250) {// && nProj[x + 1] < 230){  //0921
			nstop = x + 1;
			break;
		}
	}
	nstop = nMargin / m_nReduction[nCam];									 //int nMargin = 10;// nstop;// 50 / m_nReduction;//31;//6;//처음 영상에 흰색이 있을경우
	nEnd1 = nRightLimit3 +2;
	if(nEnd1>= m_nWidthMini - nRightSkip)nEnd1 = m_nWidthMini - nRightSkip;

	int nPeak = 0, nPeak1 = 0;
	if (m_bStep[nCam]) {
		//중앙에서 좌우로 에지 찾는다
		//탑기준으로 찾아본다.
		int ntopPos = 0, nbtmPos = 0;
		rtins.left = nstop;
		nPeak = 0;
		for (int x = m_nWidthMini*0.5; nEnd1 > x ; x++) {//0921 //
			if (abs(ntopEdge1[x] - ntopEdge1[x + 4]) > nPeak) {
				ntopPos = x + 4; nPeak = abs(ntopEdge1[x] - ntopEdge1[x + 4]);
			}
			if (nPeak > m_nEdgeTh[nCam])
				break;
		}
		for (int x = m_nWidthMini*0.5;nEnd1 > x; x++) {//0921 //
			if (abs(nbtmEdge1[x] - nbtmEdge1[x + 4]) > nPeak1) {
				nbtmPos = x + 4; nPeak1 = abs(nbtmEdge1[x] - nbtmEdge1[x + 4]);
			}
			if (nPeak1 > m_nEdgeTh[nCam])
				break;
		}
		nPeak = (nPeak + nPeak1) / 2;
		rtins.right = (ntopPos + nbtmPos)*0.5;
		if(abs(ntopPos-nbtmPos) > 16){
			rtins.right = ntopPos;
			if(rtins.right < nbtmPos)rtins.right = nbtmPos;
		}
		int nOffset1 = 10;
		int nEdge1 = 7;
		if (m_nAnodeCathode == 0)nEdge1 = 40;
		if (m_nCellSize == 1)nOffset1 = 20;
//		for (int x = m_nWidthMini*0.5; x > rtins.left + 10; x--) {//211005
		for (int x = nstop; x < m_nWidthMini*0.5; x++) {//211005
			if (abs(nProj[x] - nProj[x + 4]) >= nEdge1) {
				rtins.left = x + 4; break;
			}
		}
		for (int x = rtins.right - 8; x < rtins.right + 1; x++) {////211005  
			if (abs(nProj[x + 4] - nProj[x]) >= nPeak*0.3) {//우-> 좌 피크값 기준 자동설
				rtins.right = x + 4; break;
			}
		}
	}
	else {
		for (int x = nstop; x < nLeftStop && x>0; x++) {
			if (abs(nProj[x] - nProj[x + 4]) > nPeak) {
				rtins.left = x + 2; nPeak = abs(nProj[x] - nProj[x + 4]);
			}
			if (nPeak > m_nEdgeTh[nCam])break;
		}
		//탑기준으로 찾아본다.
		int ntopPos = 0, nbtmPos = 0;
		int ntopPos1 = 0, nbtmPos1 = 0;
		nPeak = 0;
		nEnd1 = nRightLimit3 +2;
//		for (int x = m_nWidthMini*0.5; nEnd1 > x; x++) {//0921 //탭이 포함된 에지임.
		for (int x = nEnd1;nRightStop< x; x--) {//0921 //탭이 포함된 에지임.
			if (abs(ntopEdge1[x] - ntopEdge1[x - 4]) > nPeak && ntopEdge1[x - 1] < 200) {
				ntopPos = x - 4; nPeak = abs(ntopEdge1[x] - ntopEdge1[x - 4]);
			}
			if (nPeak > m_nEdgeTh[nCam])
				break;
		}
		for (int x = nEnd1; nRightStop< x; x--) {//0921 //탭이 포함된 에지임.
			if (abs(nbtmEdge1[x] - nbtmEdge1[x - 4]) > nPeak1 && ntopEdge1[x - 1] < 200) {
				nbtmPos = x - 4; nPeak1 = abs(nbtmEdge1[x] - nbtmEdge1[x - 4]);
			}
			if (nPeak1 > m_nEdgeTh[nCam])
				break;
		}
		int nPeak2 = 0, nPeak12=0;
		for (int x = nEnd1 ; ntopPos -4 < x; x--) {//0921 //탭이 포함된 에지임.
			if (abs(ntopEdge1[x] - ntopEdge1[x - 4]) > nPeak2 && ntopEdge1[x + 1] < 200) {
				ntopPos1 = x - 4; nPeak2 = abs(ntopEdge1[x] - ntopEdge1[x - 4]);
			}
			if (nPeak2 > m_nEdgeTh[nCam])
				break;
		}
		for (int x = nEnd1 ; nbtmPos -4< x; x--) {//0921 //탭이 포함된 에지임.
			if (abs(nbtmEdge1[x] - nbtmEdge1[x - 4]) > nPeak12 && ntopEdge1[x + 1] < 200) {
				nbtmPos1 = x - 4; nPeak12 = abs(nbtmEdge1[x] - nbtmEdge1[x - 4]);
			}
			if (nPeak12 > m_nEdgeTh[nCam])
				break;
		}
		nPeak = (nPeak + nPeak1) / 2;
		nPeak2 = (nPeak2 + nPeak12) / 2;
		if(ntopPos < nbtmPos)ntopPos = nbtmPos;
		else				 nbtmPos = ntopPos;
		rtins.right = (ntopPos + nbtmPos)*0.5;
		int ntmp1 = (ntopPos1 + nbtmPos1)*0.5;
		int ntmp2 = (ntopPos1 + ntopPos)*0.5;
		int ntmp3 = (nbtmPos + nbtmPos1)*0.5;
		int nTmp4 = (ntmp3+ntmp2)*0.5;
		
		if(nPeak <= 10 && nPeak2 <=10){
			//////////////좌우 비교 //우측 끝을 확인한다.
			int nLRPeakTop[2] = {0,},nLRPeakBtm[2] = {0,};
			for (int x = m_nWidthMini*0.5; nEnd1 > x; x++) {//0921 //탭이 포함된 에지임.
				if (nLREdgeTop[x] > nLRPeakTop[1] && ntopEdge1[x + 1] < 200) {
					ntopPos = x; nLRPeakTop[1] = nLREdgeTop[x];
				}
				if (nLRPeakTop[1] > m_nEdgeTh[nCam])
					break;
			}
			for (int x = m_nWidthMini*0.5; nEnd1 > x; x++) {//0921 //탭이 포함된 에지임.
				if (nLREdgeBtm[x] > nLRPeakBtm[1] && ntopEdge1[x + 1] < 200) {
					nbtmPos = x; nLRPeakBtm[1] = nLREdgeBtm[x];
				}
				if (nLRPeakBtm[1] > m_nEdgeTh[nCam])
					break;
			}

			if(nPeak < nPeak2)
				rtins.right = (ntopPos+nbtmPos)*0.5;
		}
		else{
			if(nPeak2 > 30 && ntmp1 -rtins.right > 16 ){
				rtins.right = ntmp1;
			}
			else {
				if (rtins.right < ntmp1) {
					rtins.right = ntmp1;
				}
			}
		}
	}

	//안쪽에서 다시 한번 찾는다. //0925-1
	for (int x = rtins.right - 8; x < rtins.right && x>nstop + 8 && x < m_nWidthMini - 10; x++)
		if (nProj[x + 2] - nProj[x] > nEdgeTh * 2 && nProj[x + 3] > nEdgeGap - 20) {
			rtins.right = x + 2; break;
		}

	int nFind = 0;
	m_rtHugeRect[nCam] = rtins;
	CRect rttmp1;
	rttmp1.SetRect(rtins.left*m_nReduction[nCam], 0, rtins.right*m_nReduction[nCam], m_nHeight);
	rttmp1 = SetBoundary2(rttmp1, CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
	for (int x = rttmp1.left - 32; x < rttmp1.left + 48 && x>100 && x < m_nWidth - 100; x++) {
		if (x < m_nMarginSearchStart[nCam])continue; //211017-1
		if (abs(m_nSubProjData[nCam][0][x + 2] - m_nSubProjData[nCam][0][x]) > m_nEdgeTh[nCam]) {//211121
			m_rtHugeRect[nCam].left = x + 4; nFind = 1; break;
		}
	}
	if (nFind == 0)m_rtHugeRect[nCam].left = rtins.left*m_nReduction[nCam]; nFind = 0;
	for (int x = rttmp1.right + 64; x > rttmp1.right - 64 && x > 100 && x < m_nWidth - 100; x--) {
		if (x > m_nWidth - 256)continue; //211017-1
		if (m_nSubProjData[nCam][0][x] - m_nSubProjData[nCam][0][x - 32] > m_nEdgeTh[nCam] && m_nSubProjData[nCam][0][x - 34] <= nEdgeGap) {
			m_rtHugeRect[nCam].right = x - 36; nFind = 1; break;
		}
	}
	if (nFind == 0 || rtins.right*m_nReduction[nCam] < m_rtHugeRect[nCam].right)m_rtHugeRect[nCam].right = rtins.right*m_nReduction[nCam];

	//	if(rtins.Width()<100)return -1;
	//Tab 위치 찾기
	int  nProj1[4000] = { 0, };// = new int[sizeof(int)*m_nHeightMini];	memset(nProj1,0,sizeof(int)*m_nHeightMini);
	CRect rttab;
	if (nTabPos == 0) {
		rttab.SetRect(rtins.right, 0, rtins.right + 15, m_nHeightMini);
		if(rttab.right>nRightLimit1)//220109
			rttab.right = nRightLimit1;
	}
	else {
		rttab.SetRect(rtins.left - 15, 0, rtins.left, m_nHeightMini);
		if (rttab.left < nMargin)rttab.OffsetRect((nMargin)-rttab.left, 0);
	}
	//211029
	int nRollBrightness = 0, nRollHisto[256] = { 0, };
	int nRollBrightnessdark = 0;//220125
//	if (m_bWhiteRoll[nCam])
	{//211029
		for (int y = rttab.top; y < rttab.bottom; y++) {
			nRollHisto[*(fm + y*m_nWidthMini + rttab.right)]++;
			nRollBrightness += *(fm + y*m_nWidthMini + rttab.right);
			nRollBrightnessdark += *(m_fmCompressDark[nCam] + y*m_nWidthMini + rttab.right);
		}
	}
	int nRollMax = 0, nRollMax1 = 0;
	if (rttab.Height() > 0){
		nRollBrightness /= rttab.Height();
		nRollBrightnessdark /= rttab.Height();
	}
	for (int i = 0; i < nRollBrightness; i++) {//211102
		if (nRollHisto[i] > nRollMax) {
			nRollMax = nRollHisto[i]; nRollMax1 = i;
		}
	}
	
	rttab = SetBoundary2(rttab, CRect(7, 0, m_nWidthMini - 8, m_nHeightMini));//211017-1
	CRect rttab1;//211009
	rttab1 = rttab;
	m_rtMinTab[nCam] = rttab;
	int nCut1 = 240;//211028
	if(nRollBrightness < 120 && nRollMax1 < 100)
		nCut1 = nRollBrightness*1.5;//211103
	if (bRollTest == TRUE) {//211029
		nCut1 =nRollBrightness*1.1;//1.5;//211129 백색롤의 경우 롤밝기 수정
	}
	if(m_bWhiteRoll[nCam]){//220125
		nCut1 = nRollBrightnessdark*1.1;
		rttab.SetRect(rtins.right + 8, 0, rtins.right + 18, m_nHeightMini);
		rttab = SetBoundary2(rttab, CRect(7, 0, m_nWidthMini - 8, m_nHeightMini));//211017-1
		rttab1 = rttab;
		for (int y = rttab.top; y < rttab.bottom && y >= 0 && y < m_nHeightMini; y++) {
			for (int x = rttab.left; x < rttab.right && x>8 && x < m_nWidthMini - 8; x++) {
				nProj1[y] += *(fm + y*m_nWidthMini + x);
				if (*(fm + y*m_nWidthMini + x) > nCut1) {//211009
					nWhiteVPos[0] += x;
					nWhiteVPos[1] += y;
					nWhiteCount++;
					*(fmb + y*m_nWidthMini + x) = 0xff;
				}
				else
					*(fmb + y*m_nWidthMini + x) = 0x00;
			}
			if (rttab.Width() > 0)
				nProj1[y] /= rttab.Width();
			else nProj1[y] = 0;
		}
	}
	//else if (m_nCellSize == 1 && m_nAnodeCathode == 0) 
	else if (true == m_bLaserAndCathode[nCam])
	{
		rttab.SetRect(rtins.right + 8, 0, rtins.right + 20, m_nHeightMini);
		rttab = SetBoundary2(rttab, CRect(7, 0, m_nWidthMini - 8, m_nHeightMini));//211017-1
		rttab1 = rttab;
		//		m_rtMinTab[nCam] = rttab;
		if (rttab.right > m_nWidthMini - 8)rttab.right = m_nWidthMini - 8;
		for (int y = rttab.top; y < rttab.bottom; y++) {
			for (int x = rttab.left; x < rttab.right; x++) {
				nProj1[y] += *(fm + y*m_nWidthMini + x);
				if (*(fm + y*m_nWidthMini + x) > nCut1) {//211009
					nWhiteVPos[0] += x;
					nWhiteVPos[1] += y;
					nWhiteCount++;
					*(fmb + y*m_nWidthMini + x) = 0xff;
				}
				else
					*(fmb + y*m_nWidthMini + x) = 0x00;
			}
			if (rttab.Width() > 0) {
				nProj1[y] /= rttab.Width();
			}
			else {
				nProj1[y] = 0;
			}
		}
	}
	else {
		nCut1 = nRollBrightnessdark*1.1;
		rttab.SetRect(rtins.right + 8, 0, rtins.right + 18, m_nHeightMini);
		rttab = SetBoundary2(rttab, CRect(7, 0, m_nWidthMini - 8, m_nHeightMini));//211017-1
		rttab1 = rttab;
		for (int y = rttab.top; y < rttab.bottom && y >= 0 && y < m_nHeightMini; y++) {
			for (int x = rttab.left; x < rttab.right && x>8 && x < m_nWidthMini - 8; x++) {
				nProj1[y] += *(fm + y*m_nWidthMini + x);
				if (*(m_fmCompressDark[nCam] + y*m_nWidthMini + x) > nCut1) {//211009
					nWhiteVPos[0] += x;
					nWhiteVPos[1] += y;
					nWhiteCount++;
					*(fmb + y*m_nWidthMini + x) = 0xff;
				}
				else
					*(fmb + y*m_nWidthMini + x) = 0x00;
			}
			if (rttab.Width() > 0)
				nProj1[y] /= rttab.Width();
			else nProj1[y] = 0;
		}
	}

	int nTabCut = 20, nTabTop = 0, nTabBot = 0;
	for (int y = rttab.top; y < rttab.bottom - 10 && y >= 0 && y < m_nHeightMini; y++) {
		if (y < 5)nTabTop += nProj1[y];
		else if (y == 5)nTabTop /= 5;
		if (y > 5 && nProj1[y + 4] - nProj1[y] > nTabCut && nProj1[y] < nTabTop + 20) {
			m_rtMinTab[nCam].top = y;
			break;
		}
	}
	for (int y = rttab.bottom - 1; y > m_rtMinTab[nCam].top && y >= 4 && y < m_nHeightMini; y--) {
		if (y > rttab.bottom - 6)
			nTabBot += nProj1[y];
		else if (y == rttab.bottom - 6)
			nTabBot /= 5;
		if (y<rttab.bottom - 5 && nProj1[y - 4] - nProj1[y]>nTabCut && nProj1[y] < nTabBot + 20) {
			m_rtMinTab[nCam].bottom = y;
			break;
		}
	}
	int nYPos1 = (m_rtMinTab[nCam].top - 8) * m_nReduction[nCam], nYpos2 = (m_rtMinTab[nCam].bottom + 8) * m_nReduction[nCam], nTPeak1[2] = { 0, }, nTPeak2[2] = { 0, };//0528-1

	if (bRollTest == TRUE) {
		if (nYPos1 > 0 && nYpos2 < m_nHeight) {///0528-1
			for (int x = m_rtHugeRect[nCam].right + 32; x > m_rtHugeRect[nCam].right - 80 && x > 10 && x < m_nWidth - 10; x--) {
				if (abs(*(m_fmOdd[nCam] + nYPos1 * m_nWidth + x) - *(m_fmOdd[nCam] + nYPos1 * m_nWidth + x - 8)) > nTPeak1[0]) {
					nTPeak1[0] = abs(*(m_fmOdd[nCam] + nYPos1 * m_nWidth + x) - *(m_fmOdd[nCam] + nYPos1 * m_nWidth + x - 8)); nTPeak2[0] = x - 8;
				}
				if (abs(*(m_fmOdd[nCam] + nYpos2 * m_nWidth + x) - *(m_fmOdd[nCam] + nYpos2 * m_nWidth + x - 8)) > nTPeak1[1]) {
					nTPeak1[1] = abs(*(m_fmOdd[nCam] + nYPos1 * m_nWidth + x) - *(m_fmOdd[nCam] + nYPos1 * m_nWidth + x - 8)); nTPeak2[1] = x - 8;
				}
			}
		}
	}
	else {
		if (nYPos1 > 0 && nYpos2 < m_nHeight) {///0528-1
			for (int x = m_rtHugeRect[nCam].right + 32; x > m_rtHugeRect[nCam].right - 80 && x > 10 && x < m_nWidth - 10; x--) {
				if (!m_bInspDarkRejectAtBright[nCam])
				{
					if (abs(*(m_fmOdd[nCam] + nYPos1 * m_nWidth + x) - *(m_fmOdd[nCam] + nYPos1 * m_nWidth + x - 8)) > nTPeak1[0])
					{
						nTPeak1[0] = abs(*(m_fmOdd[nCam] + nYPos1 * m_nWidth + x) - *(m_fmOdd[nCam] + nYPos1 * m_nWidth + x - 8)); nTPeak2[0] = x - 8;
					}
					if (abs(*(m_fmOdd[nCam] + nYpos2 * m_nWidth + x) - *(m_fmOdd[nCam] + nYpos2 * m_nWidth + x - 8)) > nTPeak1[1])
					{
						nTPeak1[1] = abs(*(m_fmOdd[nCam] + nYPos1 * m_nWidth + x) - *(m_fmOdd[nCam] + nYPos1 * m_nWidth + x - 8)); nTPeak2[1] = x - 8;
					}
				}
				else
				{
					if (abs(*(m_fmEven[nCam] + nYPos1 * m_nWidth + x) - *(m_fmEven[nCam] + nYPos1 * m_nWidth + x - 8)) > nTPeak1[0]) 
					{
						nTPeak1[0] = abs(*(m_fmEven[nCam] + nYPos1 * m_nWidth + x) - *(m_fmEven[nCam] + nYPos1 * m_nWidth + x - 8)); nTPeak2[0] = x - 8;
					}
					if (abs(*(m_fmEven[nCam] + nYpos2 * m_nWidth + x) - *(m_fmEven[nCam] + nYpos2 * m_nWidth + x - 8)) > nTPeak1[1]) 
					{
						nTPeak1[1] = abs(*(m_fmEven[nCam] + nYPos1 * m_nWidth + x) - *(m_fmEven[nCam] + nYPos1 * m_nWidth + x - 8)); nTPeak2[1] = x - 8;
					}
				}
			}
		}
	}
	int ntsum = nTPeak2[1];//+nTPeak2[0])*0.5;
	if (ntsum < nTPeak2[0])ntsum = nTPeak2[0];
	if (nTabPos == 0)	m_rtMinTab[nCam].left -= 4;
	else			m_rtMinTab[nCam].right += 4;
	if (nWhiteCount > 30) {//211009 //탭이 작은 경우 후보위치를 찾기위해 값을 줄
		nWhiteVPos[0] /= nWhiteCount;
		nWhiteVPos[1] /= nWhiteCount;
		m_ptTabCenter[nCam].SetPoint(nWhiteVPos[0], nWhiteVPos[1]);//211009
	}
	else {
		m_ptTabCenter[nCam].SetPoint(0, 0);//211009
	}
	int nBlobCount = 0, nBlobCount1 = 0, nBlobNo = 0, nBlobNo1 = 0;
	double dTemp = 0, dBlobSize = 8, dBlobSize1 = 12;//0609
	CRect rtBlob;
	m_pChainHuge[nCam][0]->SetChainData(1, fmb, 2, 1, 8, 100000, m_nWidthMini, m_nHeightMini);//white //0609
	nBlobCount = m_pChainHuge[nCam][0]->FastChain(rttab1.left, rttab1.top, rttab1.right - 1, rttab1.bottom - 1);
	double dcx = 0, dcy = 0, tmp = 0, dSum1 = 0, dSum2 = 0, dstdev2 = 0, dstdev3 = 0, tmp1 = 0, dSum11 = 0, dSum12 = 0, dstdev12 = 0, dstdev13 = 0, dstdev22 = 0, dstdev23 = 0;
	for (int i = 0; i < nBlobCount; i++)
	{
		//제일 큰거 한개
		dTemp = m_pChainHuge[nCam][0]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp < 6 || nret>4)continue;
		if (dTemp >= dBlobSize)//최대 불량 사이즈.
		{
			nBlobNo = i; dBlobSize = dTemp;
		}
	}
	if (nBlobCount > 0 && m_bWhiteRoll[nCam]==FALSE) {
		int nx11 = m_pChainHuge[nCam][0]->FindMinX(nBlobNo);//0609
		int nx12 = m_pChainHuge[nCam][0]->FindMaxX(nBlobNo);//0609
		int ny11 = m_pChainHuge[nCam][0]->FindMinY(nBlobNo);//0609
		int ny12 = m_pChainHuge[nCam][0]->FindMaxY(nBlobNo);//0609
		m_rtMinTab[nCam].top = ny11 - 2;
		m_rtMinTab[nCam].bottom = ny12 + 2;
	}
	m_rtMinTab[nCam] = SetBoundary2(m_rtMinTab[nCam], CRect(7, 0, m_nWidthMini - 8, m_nHeightMini));//211017-1

	nBlobCount = 0, nBlobCount1 = 0, nBlobNo = 0, nBlobNo1 = 0;
	dTemp = 0, dBlobSize = 8, dBlobSize1 = 12;//0609
											  //	delete nProj1;
											  //이진화로 처리
	m_nDefaultBright[nCam][2] = m_nDefaultBright[nCam][0];

	if(m_nDefaultBright[nCam][0]<80)
		nHistomax2 = m_nDefaultBright[nCam][0];
	memcpy(fm,m_fmCompress[nCam],m_nWidthMini*m_nHeightMini);//220328

//	fm = fmsrc;//m_fmCompress[nCam];
	int nThw = m_nLargeDefectWTh[nCam], nThb = m_nLargeDefectBTh[nCam];
	int nw1 = BAD_WIDTH*0.5, nh1 = BAD_HEIGHT*0.5, njump = 0;
	int nHisto2[256] = {0,};//211126
	for (int y = 0; y < m_nHeightMini - 4; y++) {
		if (nret >= 4)break;
		for (int x = rtins.left; x < rtins.right && x>8 && x < m_nWidthMini - 8; x++) {
			int ntmp = *(fm + y*m_nWidthMini + x) -nImageAverage[0];// nHistomax2;//m_nDefaultBright[nCam][0];// nProj[x];//전체 tape가 있으면 못잡는다.
			if (ntmp > nThw)
				*(fmW + y*m_nWidthMini + x) = 0xff;
			else	*(fmW + y*m_nWidthMini + x) = 0x00;
			if (ntmp < -nThb)
				*(fmb + y*m_nWidthMini + x) = 0xff;
			else	*(fmb + y*m_nWidthMini + x) = 0x00;
			nHisto2[*(fm + y*m_nWidthMini + x)]++;//211126
		}
	}

	m_pChainHuge[nCam][0]->SetChainData(1, fmW, 2, 1, 8, 100000, m_nWidthMini, m_nHeightMini);//white //0609
	nBlobCount = m_pChainHuge[nCam][0]->FastChain(rtins.left, rtins.top, rtins.right - 10, rtins.bottom - 1);
	dcx = 0, dcy = 0, tmp = 0, dSum1 = 0, dSum2 = 0, dstdev2 = 0, dstdev3 = 0, tmp1 = 0, dSum11 = 0, dSum12 = 0, dstdev12 = 0, dstdev13 = 0, dstdev22 = 0, dstdev23 = 0;
	for (int i = 0; i < nBlobCount; i++)
	{
		dTemp = m_pChainHuge[nCam][0]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp < 6 || nret >= 4)continue;
		if (dTemp >= dBlobSize)//최대 불량 사이즈.
		{
			int nx11 = m_pChainHuge[nCam][0]->FindMinX(i);//0609
			int nx12 = m_pChainHuge[nCam][0]->FindMaxX(i);//0609
			int ny11 = m_pChainHuge[nCam][0]->FindMinY(i);//0609
			int ny12 = m_pChainHuge[nCam][0]->FindMaxY(i);//0609
														  //			if ((nx12 - nx11 >= 6 && nx12 - nx11 <= 20) || (ny12 - ny11 >= 3 && ny12 - ny11 <= 10)){//0609
			if (m_bWhiteConnectionTapeSizeAndOr)
			{
				if ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeWhiteSizeX[nCam] && (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeWhiteSizeY[nCam]) {
					if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
						continue;
					m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
					m_dBlobCenter1[nCam][nret] = dcy * 16;
					njump = FALSE;
					for (int k = 0; k < nret; k++) {
						CPoint pt1;
						pt1.SetPoint(dcx * 16, dcy * 16);
						if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
							njump = TRUE;
							break;
						}
					}
					if (njump)continue;
					m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
					m_nHugeDefectBright[nCam][nret] = 1;
					nBlobNo = i;	//dBlobSize = dTemp;//면적
					rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
					m_dHugeDefectSizeX[nCam][nret][0] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
					m_dHugeDefectSizeY[nCam][nret][0] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
					rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
					rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
					m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
					if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
						m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
						m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
					}
					if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
					if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
					ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nret++;
				}
				//흰색 테이프 스펙보다 작으면 검은색 테이프 자국의 스펙과 비교.
				else if (m_bBlackConnectionTapeSizeAndOr && TRUE == m_BlackTapeInsp && ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] && (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]))
				{
					if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
						continue;
					m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
					m_dBlobCenter1[nCam][nret] = dcy * 16;
					njump = FALSE;
					for (int k = 0; k < nret; k++) {
						CPoint pt1;
						pt1.SetPoint(dcx * 16, dcy * 16);
						if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
							njump = TRUE;
							break;
						}
					}
					if (njump)continue;
					m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
					m_nHugeDefectBright[nCam][nret] = 2;
					nBlobNo = i;	//dBlobSize = dTemp;//면적
					rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
					m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
					m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
					rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
					rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
					m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
					if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
						m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
						m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
					}
					if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
					if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
					ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nret++;
				}
				else if (m_bBlackConnectionTapeSizeAndOr == FALSE && TRUE == m_BlackTapeInsp && ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] || (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]))
				{
					if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
						continue;
					m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
					m_dBlobCenter1[nCam][nret] = dcy * 16;
					njump = FALSE;
					for (int k = 0; k < nret; k++) {
						CPoint pt1;
						pt1.SetPoint(dcx * 16, dcy * 16);
						if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
							njump = TRUE;
							break;
						}
					}
					if (njump)continue;
					m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
					m_nHugeDefectBright[nCam][nret] = 2;
					nBlobNo = i;	//dBlobSize = dTemp;//면적
					rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
					m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
					m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
					rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
					rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
					m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
					if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
						m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
						m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
					}
					if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
					if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
					ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nret++;
				}
			}
			else
			{
				if ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeWhiteSizeX[nCam] || (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeWhiteSizeY[nCam]) {
					if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
						continue;
					m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
					m_dBlobCenter1[nCam][nret] = dcy * 16;
					njump = FALSE;
					for (int k = 0; k < nret; k++) {
						CPoint pt1;
						pt1.SetPoint(dcx * 16, dcy * 16);
						if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
							njump = TRUE;
							break;
						}
					}
					if (njump)continue;
					m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
					m_nHugeDefectBright[nCam][nret] = 1;
					nBlobNo = i;	//dBlobSize = dTemp;//면적
					rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
					m_dHugeDefectSizeX[nCam][nret][0] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
					m_dHugeDefectSizeY[nCam][nret][0] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
					rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
					rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
					m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
					if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
						m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
						m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
					}
					if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
					if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
					ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nret++;
				}
				//흰색 테이프 스펙보다 작으면 검은색 테이프 자국의 스펙과 비교.
				else if (m_bBlackConnectionTapeSizeAndOr && TRUE == m_BlackTapeInsp && ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] && (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]))
				{
					if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
						continue;
					m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
					m_dBlobCenter1[nCam][nret] = dcy * 16;
					njump = FALSE;
					for (int k = 0; k < nret; k++) {
						CPoint pt1;
						pt1.SetPoint(dcx * 16, dcy * 16);
						if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
							njump = TRUE;
							break;
						}
					}
					if (njump)continue;
					m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
					m_nHugeDefectBright[nCam][nret] = 2;
					nBlobNo = i;	//dBlobSize = dTemp;//면적
					rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
					m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
					m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
					rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
					rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
					m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
					if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
						m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
						m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
					}
					if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
					if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
					ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nret++;
				}
				else if (m_bBlackConnectionTapeSizeAndOr == FALSE && TRUE == m_BlackTapeInsp && ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] || (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]))
				{
					if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
						continue;
					m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
					m_dBlobCenter1[nCam][nret] = dcy * 16;
					njump = FALSE;
					for (int k = 0; k < nret; k++) {
						CPoint pt1;
						pt1.SetPoint(dcx * 16, dcy * 16);
						if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
							njump = TRUE;
							break;
						}
					}
					if (njump)continue;
					m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
					m_nHugeDefectBright[nCam][nret] = 2;
					nBlobNo = i;	//dBlobSize = dTemp;//면적
					rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
					m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
					m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
					rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
					rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
					m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
					if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
						m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
						m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
					}
					if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
					if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
					ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nret++;
				}
			}

		}
		if(MAX_HUGEDEFECT-1 <= nret)//220303
			break;
	}
	int nCenterTh = 120;
	if(nret==0 && m_nDefaultBright[nCam][0]>nCenterTh ){//중앙이 밝고 테이프로 못잡은 경우
		m_nHugeDefectBright[nCam][nret] = 1;
		rt[nret].SetRect(100, 10,m_nWidthMini-100,m_nHeightMini-10);
		dcx = rt[nret].CenterPoint().x;
		dcy = rt[nret].CenterPoint().y;
		m_dHugeDefectSizeX[nCam][nret][0] = (rt[nret].right - rt[nret].left + 1)*m_dscale_x1[nCam] * m_nReduction[nCam];
		m_dHugeDefectSizeY[nCam][nret][0] = (rt[nret].bottom - rt[nret].top + 1)*m_dscale_y1[nCam] * m_nReduction[nCam];
		rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
		rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
		m_rtSkipRange[nCam][nret].SetRect((rtBlob.left - 3)*m_nReduction[nCam], 0, (rtBlob.right + 2)*m_nReduction[nCam], (m_nHeightMini-1)*m_nReduction[nCam]);
		if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT*m_nReduction[nCam]) {
			m_rtSkipRange[nCam][nret].top = (dcy - (nh1*0.5))*m_nReduction[nCam];
			m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1*0.5))*m_nReduction[nCam];
		}
		if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
		if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
		ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
		m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321

		nret++;
	}

	if(nret==0 && nFullWhiteCnt>4000){
		nFullWhiteCnt = nFullWhiteCnt;
	}
	m_pChainHuge[nCam][1]->SetChainData(1, fmb, 2, 2, 4, 100000, m_nWidthMini, m_nHeightMini);//white
	nBlobCount1 = m_pChainHuge[nCam][1]->FastChain(rtins.left+20, rtins.top, rtins.right - 10, rtins.bottom - 1);
	for (int i = 0; i < nBlobCount1; i++)
	{
		if(MAX_HUGEDEFECT-1 <= nret)//220303
			break;
		dTemp = m_pChainHuge[nCam][1]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp < 5 || nret >= 4)continue;
		if (dTemp >= dBlobSize1)//최대 불량 사이즈.
		{
			int nx11 = m_pChainHuge[nCam][1]->FindMinX(i);//0726
			int nx12 = m_pChainHuge[nCam][1]->FindMaxX(i);//0726
			int ny11 = m_pChainHuge[nCam][1]->FindMinY(i);//0726
			int ny12 = m_pChainHuge[nCam][1]->FindMaxY(i);//0726
			if (m_bBlackConnectionTapeSizeAndOr)
			{
				if ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] && (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]) {//0726
					if (m_pChainHuge[nCam][1]->FindMaxX(i) - m_pChainHuge[nCam][1]->FindMinX(i) < 5)
						continue;
					m_pChainHuge[nCam][1]->Chain_Center(i, &dcx, &dcy);
					njump = FALSE;
					for (int k = 0; k < nret; k++) {
						CPoint pt1;
						pt1.SetPoint(dcx * 16, dcy * 16);
						if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
							njump = TRUE;
							break;
						}
					}
					if (njump)continue;
					m_nHugeDefectBright[nCam][nret] = 2;
					nBlobNo = i;	dBlobSize = dTemp;//면적
					rt[nret].SetRect(m_pChainHuge[nCam][1]->FindMinX(nBlobNo), m_pChainHuge[nCam][1]->FindMinY(nBlobNo), m_pChainHuge[nCam][1]->FindMaxX(nBlobNo), m_pChainHuge[nCam][1]->FindMaxY(nBlobNo));
					m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
					m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
					m_pChainHuge[nCam][1]->Chain_Center(nBlobNo, &dcx, &dcy);
					rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
					rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
					m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
					if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
						m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
						m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
					}
					if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
					if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
					ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nret++;
				}
			}
			else
			{
				if ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] || (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]) {//0726
					if (m_pChainHuge[nCam][1]->FindMaxX(i) - m_pChainHuge[nCam][1]->FindMinX(i) < 5)
						continue;
					m_pChainHuge[nCam][1]->Chain_Center(i, &dcx, &dcy);
					njump = FALSE;
					for (int k = 0; k < nret; k++) {
						CPoint pt1;
						pt1.SetPoint(dcx * 16, dcy * 16);
						if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
							njump = TRUE;
							break;
						}
					}
					if (njump)continue;
					m_nHugeDefectBright[nCam][nret] = 2;
					nBlobNo = i;	dBlobSize = dTemp;//면적
					rt[nret].SetRect(m_pChainHuge[nCam][1]->FindMinX(nBlobNo), m_pChainHuge[nCam][1]->FindMinY(nBlobNo), m_pChainHuge[nCam][1]->FindMaxX(nBlobNo), m_pChainHuge[nCam][1]->FindMaxY(nBlobNo));
					m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
					m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
					m_pChainHuge[nCam][1]->Chain_Center(nBlobNo, &dcx, &dcy);
					rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
					rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
					m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
					if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
						m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
						m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
					}
					if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
					if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
					ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nret++;
				}
			}
			
		}
		if(MAX_HUGEDEFECT-1 <= nret)//220303
			break;
	}

	/////////////////////tape 유출 방지를 위해 한번더 Blob을 한다.
	if(m_nAnodeCathode==0 && nret==0){
		if(nFullWhiteCnt > 4000 || nFullAreaAve > 120)
		{
			for (int y = 0; y < m_nHeightMini; y++) {
				for (int x = nstop; x < nRightLimit1 ; x++) {
					int ntmp = *(fm + y*m_nWidthMini + x);
					if (ntmp > 230)
						*(fmb + y*m_nWidthMini + x) = 0xff;
					else	*(fmb + y*m_nWidthMini + x) = 0x00;
				}
			}
			m_pChainHuge[nCam][1]->SetChainData(1, fmb, 2, 2, 4, 100000, m_nWidthMini, m_nHeightMini);//white
			nBlobCount1 = m_pChainHuge[nCam][1]->FastChain(nstop+4, rtins.top, nRightLimit1-4, rtins.bottom - 1);
			dBlobSize1 = 0;
			for (int i = 0; i < nBlobCount1; i++)
			{
				dTemp = m_pChainHuge[nCam][1]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
				if (dTemp >= dBlobSize1)//최대 불량 사이즈.
				{
					dBlobSize1 = dTemp;nBlobNo = i;
				}
			}
			if(dBlobSize1 > 1500){//탭만 있을경우 400 보다 작다
				int nx11 = m_pChainHuge[nCam][1]->FindMinX(nBlobNo);//0726
				int nx12 = m_pChainHuge[nCam][1]->FindMaxX(nBlobNo);//0726
				int ny11 = m_pChainHuge[nCam][1]->FindMinY(nBlobNo);//0726
				int ny12 = m_pChainHuge[nCam][1]->FindMaxY(nBlobNo);//0726
				if (m_bBlackConnectionTapeSizeAndOr)
				{
					if ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] && (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]) {//0726
						m_pChainHuge[nCam][1]->Chain_Center(nBlobNo, &dcx, &dcy);
						m_nHugeDefectBright[nCam][nret] = 2;
						rt[nret].SetRect(m_pChainHuge[nCam][1]->FindMinX(nBlobNo), m_pChainHuge[nCam][1]->FindMinY(nBlobNo), m_pChainHuge[nCam][1]->FindMaxX(nBlobNo), m_pChainHuge[nCam][1]->FindMaxY(nBlobNo));
						m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
						m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
						m_pChainHuge[nCam][1]->Chain_Center(nBlobNo, &dcx, &dcy);
						rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
						rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
						m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
						if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
							m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
							m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
						}
						if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
						if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
						ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321

						nret++;
					}
				}
				else
				{
					if ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] || (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]) {//0726
						m_pChainHuge[nCam][1]->Chain_Center(nBlobNo, &dcx, &dcy);
						m_nHugeDefectBright[nCam][nret] = 2;
						rt[nret].SetRect(m_pChainHuge[nCam][1]->FindMinX(nBlobNo), m_pChainHuge[nCam][1]->FindMinY(nBlobNo), m_pChainHuge[nCam][1]->FindMaxX(nBlobNo), m_pChainHuge[nCam][1]->FindMaxY(nBlobNo));
						m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
						m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
						m_pChainHuge[nCam][1]->Chain_Center(nBlobNo, &dcx, &dcy);
						rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
						rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
						m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
						if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
							m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
							m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
						}
						if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
						if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
						ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321

						nret++;
					}
				}	
			}
		}
	}


	double dt1 = GetPrecisionTime();
	if(nret==0 && m_bTapePreUse[nCam]){
		////211126 Histogram의 어두운점이 모인곳과 밝은 곳이 모인곳을 찾는다.
		int nminhisto = 255,nmaxhisto = 0,nhistotmp = 0;;
		for(int i=0;i<nImageAverage[0]-10;i++){
			if(nHisto2[i] >= nhistotmp){
				nhistotmp = nHisto2[i];
				nminhisto = i;
			}
		}
		nhistotmp = 0;
		for(int i=255;i>nImageAverage[0]+10;i--){
			if(nHisto2[i] >= nhistotmp){
				nhistotmp = nHisto2[i];
				nmaxhisto = i;
			}
		}
		int nHistoCut = 50,nTapeGap = 0, ntapetheshold = 10;//noise 제거를 위해 50개ㅗ 설정 //211126
		nTapeGap = nmaxhisto - nImageAverage[0];
		int nave1 = nImageAverage[0] , ntargetV = 100;
		if(nave1 > (nmaxhisto + nminhisto)/2 && (nmaxhisto + nminhisto)/2 > nave1 - 10)
			nave1 = (nmaxhisto + nminhisto)/2;
		int nThw1 = m_nLargeDefectWThPre[nCam], nThb1 = m_nLargeDefectBThPre[nCam];
//
		int nGapoffset = 0;
		if(nave1-60>0)
			nGapoffset = nave1-60;
//		nGapoffset *= 10;
		ntapetheshold += nGapoffset;
//		if(nhistotmp> 25 && nTapeGap > ntapetheshold*1.5)
		{//여기서 밝기가 일정값 이상 벌어지면 전극내에 무언가가 있음.
			//전처리를 한다.
			double dmag1 = m_dMagValue[nCam],dmag2 = m_dMagValue[nCam]*0.5,dmag3 = m_dMagValue[nCam];
			for (int y = 0; y < m_nHeightMini; y++) {
				for (int x = rtins.left; x < rtins.right && x>8 && x < m_nWidthMini - 8; x++) {
					//테이프가 전면에 붙여져 평균 밝기가 올라간 경우??
					int ntmp1 = *(fm + y*m_nWidthMini + x) -nave1;
					if(abs(ntmp1) > ntapetheshold)
						dmag1 = dmag3;
					else
						dmag1 = dmag2;
					int ntmp = ntargetV + (*(fm + y*m_nWidthMini + x) -nave1)*dmag1;// nHistomax2;//m_nDefaultBright[nCam][0];// nProj[x];//전체 tape가 있으면 못잡는다.
					if (ntmp > 255)
						ntmp = 0xff;
					else if(ntmp<0)	ntmp = 0x00;
					m_fmCompressPre[nCam][y*m_nWidthMini+x] = ntmp;
					if (ntmp > ntargetV+nThw1)
						*(fmW + y*m_nWidthMini + x) = 0xff;
					else	*(fmW + y*m_nWidthMini + x) = 0x00;
					if (ntmp < ntargetV -nThb1)
						*(fmb + y*m_nWidthMini + x) = 0xff;
					else	*(fmb + y*m_nWidthMini + x) = 0x00;
				}
			}
		}
//		else{
////			m_fmCompress[nCam] = m_fmCompressPre[nCam];
//			m_nHugeCount[nCam] = nret;//m_rtSkipRange
//			delete [] fmW;
//			delete [] fmb;
//			delete [] fm;
//			return nret;
//
//		}
//백색 
		m_pChainHuge[nCam][0]->SetChainData(1, fmW, 2, 1, 8, 100000, m_nWidthMini, m_nHeightMini);//white //0609
		nBlobCount = m_pChainHuge[nCam][0]->FastChain(rtins.left, rtins.top, rtins.right - 10, rtins.bottom - 1);
		dcx = 0, dcy = 0, tmp = 0, dSum1 = 0, dSum2 = 0, dstdev2 = 0, dstdev3 = 0, tmp1 = 0, dSum11 = 0, dSum12 = 0, dstdev12 = 0, dstdev13 = 0, dstdev22 = 0, dstdev23 = 0;
		for (int i = 0; i < nBlobCount; i++)
		{
			dTemp = m_pChainHuge[nCam][0]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp < 6 || nret >= 4)continue;
			if (dTemp >= dBlobSize)//최대 불량 사이즈.
			{
				int nx11 = m_pChainHuge[nCam][0]->FindMinX(i);//0609
				int nx12 = m_pChainHuge[nCam][0]->FindMaxX(i);//0609
				int ny11 = m_pChainHuge[nCam][0]->FindMinY(i);//0609
				int ny12 = m_pChainHuge[nCam][0]->FindMaxY(i);//0609
				if (m_bWhiteConnectionTapeSizeAndOr)
				{
					if ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeWhiteSizeX[nCam] && (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeWhiteSizeY[nCam]) {
						if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
							continue;
						m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
						m_dBlobCenter1[nCam][nret] = dcy * 16;
						njump = FALSE;
						for (int k = 0; k < nret; k++) {
							CPoint pt1;
							pt1.SetPoint(dcx * 16, dcy * 16);
							if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
								njump = TRUE;
								break;
							}
						}
						if (njump)continue;
						m_nHugeDefectBright[nCam][nret] = 1;
						nBlobNo = i;	//dBlobSize = dTemp;//면적
						rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
						m_dHugeDefectSizeX[nCam][nret][0] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
						m_dHugeDefectSizeY[nCam][nret][0] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
						rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
						rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
						m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
						if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
							m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
							m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
						}
						if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
						if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
						ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
						nret++;
					}
					//흰색 테이프 스펙보다 작으면 검은색 테이프 자국의 스펙과 비교.
					else if (m_bBlackConnectionTapeSizeAndOr && TRUE == m_BlackTapeInsp && ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] && (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]))
					{
						if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
							continue;
						m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
						m_dBlobCenter1[nCam][nret] = dcy * 16;
						njump = FALSE;
						for (int k = 0; k < nret; k++) {
							CPoint pt1;
							pt1.SetPoint(dcx * 16, dcy * 16);
							if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
								njump = TRUE;
								break;
							}
						}
						if (njump)continue;
						m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
						m_nHugeDefectBright[nCam][nret] = 2;
						nBlobNo = i;	//dBlobSize = dTemp;//면적
						rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
						m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
						m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
						rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
						rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
						m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
						if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
							m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
							m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
						}
						if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
						if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
						ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						nret++;
					}
					else if (m_bBlackConnectionTapeSizeAndOr == FALSE && TRUE == m_BlackTapeInsp && ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] || (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]))
					{
						if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
							continue;
						m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
						m_dBlobCenter1[nCam][nret] = dcy * 16;
						njump = FALSE;
						for (int k = 0; k < nret; k++) {
							CPoint pt1;
							pt1.SetPoint(dcx * 16, dcy * 16);
							if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
								njump = TRUE;
								break;
							}
						}
						if (njump)continue;
						m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
						m_nHugeDefectBright[nCam][nret] = 2;
						nBlobNo = i;	//dBlobSize = dTemp;//면적
						rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
						m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
						m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
						rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
						rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
						m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
						if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
							m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
							m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
						}
						if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
						if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
						ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						nret++;
					}
				}
				else
				{
					if ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeWhiteSizeX[nCam] || (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeWhiteSizeY[nCam]) {
						if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
							continue;
						m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
						m_dBlobCenter1[nCam][nret] = dcy * 16;
						njump = FALSE;
						for (int k = 0; k < nret; k++) {
							CPoint pt1;
							pt1.SetPoint(dcx * 16, dcy * 16);
							if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
								njump = TRUE;
								break;
							}
						}
						if (njump)continue;
						m_nHugeDefectBright[nCam][nret] = 1;
						nBlobNo = i;	//dBlobSize = dTemp;//면적
						rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
						m_dHugeDefectSizeX[nCam][nret][0] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
						m_dHugeDefectSizeY[nCam][nret][0] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
						rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
						rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
						m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
						if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
							m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
							m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
						}
						if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
						if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
						ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
						nret++;
					}
					//흰색 테이프 스펙보다 작으면 검은색 테이프 자국의 스펙과 비교.
					else if (m_bBlackConnectionTapeSizeAndOr && TRUE == m_BlackTapeInsp && ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] && (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]))
					{
						if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
							continue;
						m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
						m_dBlobCenter1[nCam][nret] = dcy * 16;
						njump = FALSE;
						for (int k = 0; k < nret; k++) {
							CPoint pt1;
							pt1.SetPoint(dcx * 16, dcy * 16);
							if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
								njump = TRUE;
								break;
							}
						}
						if (njump)continue;
						m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
						m_nHugeDefectBright[nCam][nret] = 2;
						nBlobNo = i;	//dBlobSize = dTemp;//면적
						rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
						m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
						m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
						rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
						rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
						m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
						if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
							m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
							m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
						}
						if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
						if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
						ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						nret++;
					}
					else if (m_bBlackConnectionTapeSizeAndOr == FALSE && TRUE == m_BlackTapeInsp && ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] || (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]))
					{
						if (m_pChainHuge[nCam][0]->FindMaxX(i) - m_pChainHuge[nCam][0]->FindMinX(i) < 6)	//0608 //4:2 
							continue;
						m_pChainHuge[nCam][0]->Chain_Center(i, &dcx, &dcy);
						m_dBlobCenter1[nCam][nret] = dcy * 16;
						njump = FALSE;
						for (int k = 0; k < nret; k++) {
							CPoint pt1;
							pt1.SetPoint(dcx * 16, dcy * 16);
							if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
								njump = TRUE;
								break;
							}
						}
						if (njump)continue;
						m_bFindWhiteTape[nCam] = TRUE;//tape 검출치 TRUE //220321
						m_nHugeDefectBright[nCam][nret] = 2;
						nBlobNo = i;	//dBlobSize = dTemp;//면적
						rt[nret].SetRect(m_pChainHuge[nCam][0]->FindMinX(nBlobNo), m_pChainHuge[nCam][0]->FindMinY(nBlobNo), m_pChainHuge[nCam][0]->FindMaxX(nBlobNo), m_pChainHuge[nCam][0]->FindMaxY(nBlobNo));
						m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
						m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
						rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
						rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
						m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
						if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
							m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
							m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
						}
						if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
						if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
						ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						nret++;
					}
				}
				
			}
			if(MAX_HUGEDEFECT-1 <= nret)//220303
				break;
		}
		//black
		m_pChainHuge[nCam][1]->SetChainData(1, fmb, 2, 2, 4, 100000, m_nWidthMini, m_nHeightMini);//white
		nBlobCount1 = m_pChainHuge[nCam][1]->FastChain(rtins.left+20, rtins.top, rtins.right - 10, rtins.bottom - 1);
		for (int i = 0; i < nBlobCount1; i++)
		{
			if(MAX_HUGEDEFECT-1 <= nret)//220303
				break;
			dTemp = m_pChainHuge[nCam][1]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp < 5 || nret >= 4)continue;
			if (dTemp >= dBlobSize1)//최대 불량 사이즈.
			{
				int nx11 = m_pChainHuge[nCam][1]->FindMinX(i);//0726
				int nx12 = m_pChainHuge[nCam][1]->FindMaxX(i);//0726
				int ny11 = m_pChainHuge[nCam][1]->FindMinY(i);//0726
				int ny12 = m_pChainHuge[nCam][1]->FindMaxY(i);//0726

				if (m_bBlackConnectionTapeSizeAndOr)
				{
					if ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] && (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]) {//0726
						if (m_pChainHuge[nCam][1]->FindMaxX(i) - m_pChainHuge[nCam][1]->FindMinX(i) < 5)
							continue;
						m_pChainHuge[nCam][1]->Chain_Center(i, &dcx, &dcy);
						njump = FALSE;
						for (int k = 0; k < nret; k++) {
							CPoint pt1;
							pt1.SetPoint(dcx * 16, dcy * 16);
							if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
								njump = TRUE;
								break;
							}
						}
						if (njump)continue;
						m_nHugeDefectBright[nCam][nret] = 2;
						nBlobNo = i;	dBlobSize = dTemp;//면적
						rt[nret].SetRect(m_pChainHuge[nCam][1]->FindMinX(nBlobNo), m_pChainHuge[nCam][1]->FindMinY(nBlobNo), m_pChainHuge[nCam][1]->FindMaxX(nBlobNo), m_pChainHuge[nCam][1]->FindMaxY(nBlobNo));
						m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
						m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
						m_pChainHuge[nCam][1]->Chain_Center(nBlobNo, &dcx, &dcy);
						rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
						rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
						m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
						if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
							m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
							m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
						}
						if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
						if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
						ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						nret++;
					}
				}
				else
				{
					if ((nx12 - nx11) * m_nReduction[nCam] * m_dscale_x1[nCam] >= m_dTapeBlackSizeX[nCam] || (ny12 - ny11) * m_nReduction[nCam] * m_dscale_y1[nCam] >= m_dTapeBlackSizeY[nCam]) {//0726
						if (m_pChainHuge[nCam][1]->FindMaxX(i) - m_pChainHuge[nCam][1]->FindMinX(i) < 5)
							continue;
						m_pChainHuge[nCam][1]->Chain_Center(i, &dcx, &dcy);
						njump = FALSE;
						for (int k = 0; k < nret; k++) {
							CPoint pt1;
							pt1.SetPoint(dcx * 16, dcy * 16);
							if (m_rtSkipRange[nCam][k].PtInRect(pt1)) {
								njump = TRUE;
								break;
							}
						}
						if (njump)continue;
						m_nHugeDefectBright[nCam][nret] = 2;
						nBlobNo = i;	dBlobSize = dTemp;//면적
						rt[nret].SetRect(m_pChainHuge[nCam][1]->FindMinX(nBlobNo), m_pChainHuge[nCam][1]->FindMinY(nBlobNo), m_pChainHuge[nCam][1]->FindMaxX(nBlobNo), m_pChainHuge[nCam][1]->FindMaxY(nBlobNo));
						m_dHugeDefectSizeX[nCam][nret][1] = (rt[nret].right - rt[nret].left + 1) * m_dscale_x1[nCam] * m_nReduction[nCam];
						m_dHugeDefectSizeY[nCam][nret][1] = (rt[nret].bottom - rt[nret].top + 1) * m_dscale_y1[nCam] * m_nReduction[nCam];
						m_pChainHuge[nCam][1]->Chain_Center(nBlobNo, &dcx, &dcy);
						rtBlob.SetRect(dcx - nw1, dcy - nh1, dcx + nw1, dcy + nh1);
						rtBlob = SetBoundary2(rtBlob, CRect(10, 0, m_nWidthMini - 10, m_nHeightMini));
						m_rtSkipRange[nCam][nret].SetRect((rtins.left - 3) * m_nReduction[nCam], (rt[nret].top - 2) * m_nReduction[nCam], (rtins.right + 2) * m_nReduction[nCam], (rt[nret].bottom + 2) * m_nReduction[nCam]);
						if (m_rtSkipRange[nCam][nret].Height() < BAD_HEIGHT * m_nReduction[nCam]) {
							m_rtSkipRange[nCam][nret].top = (dcy - (nh1 * 0.5)) * m_nReduction[nCam];
							m_rtSkipRange[nCam][nret].bottom = (dcy + (nh1 * 0.5)) * m_nReduction[nCam];
						}
						if (m_rtSkipRange[nCam][nret].bottom > m_nHeight)m_rtSkipRange[nCam][nret].bottom = m_nHeight;
						if (m_rtSkipRange[nCam][nret].top < 0)m_rtSkipRange[nCam][nret].top = 0;
						ImageSelectCopy(fm, m_fmHugeDefect[nCam][nret], m_nWidthMini, m_nHeightMini, rtBlob, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						nret++;
					}
				}
				
			}
			if(MAX_HUGEDEFECT-1 <= nret)//220303
				break;
		}

//		m_fmCompress[nCam] = m_fmCompressPre[nCam];
	}
	double dt2 = GetPrecisionTime();

	m_nHugeCount[nCam] = nret;//m_rtSkipRange
	delete [] fmW;
	delete [] fmb;
	delete [] fm;
	return nret;
}

void CInspect::InitParam(int nAnodeCathode)
{
	m_nMachine = nAnodeCathode;//0cathode 1anode
	//InspParam stInspParam;
	m_bNgTag;//NG Tag Code 변경  0 island 1Tape//0625
	/////////////////////211009  FALSE or  TRUE and
	m_bPinholeAndOr = FALSE;//핀홀 And 조건 //211006
	m_bLineSizeAndOr = FALSE;//라인 사이즈 and or
	m_bScratchSizeAndOr = TRUE;//스크래치 사이즈 and or
	m_bCraterSizeAndOr = TRUE;//분화구 사이즈 and or
	m_bDentSizeAndOr = TRUE;//찍힘 사이즈 and or
	m_bProtrusionSizeAndOr = TRUE;//돌출 사이즈 and or
	m_bCrackSizeAndOr = FALSE;//크랙 사이즈 and or
	m_bHeatingWrinkleSizeAndOr = TRUE;//열주름 사이즈 and or
	m_bCoatingWrinkleSizeAndOr = TRUE;//코팅주름 사이즈 and or
	m_bNonCoatingSizeAndOr = TRUE;//미코팅 사이즈 and or 탭부 제외
	m_bIslandSizeAndOr = TRUE;//아일랜드 사이즈 and or
	m_bWrinkleSizeAndOr = TRUE;//탭주름 사이즈 and or
	m_bDrossSizeAndOr = TRUE;//드로스 사이즈 and or
	m_bSpatterSizeAndOr = TRUE;//스패터 사이즈 and or
	m_bSpurSizeAndOr = FALSE;//TRUE;//탭부 파임 사이즈 and or


	m_bInspManual = FALSE;
	for (int i = 0; i < MAX_CAMERA; i++)
	{
		m_rectTabRightDetectROI[i] = CRect(0,0,0,0);
		m_nTabAvergeValue[i] = 0;
		m_rectTabROI[i] = CRect(0, 0, 0, 0);
		m_nProtrusionTh[i] = 50;//220216
		m_dTapeWhiteSizeX[i] = 10;//0726
		m_dTapeWhiteSizeY[i] = 10;//0726
		m_dTapeBlackSizeX[i] = 4;//0726
		m_dTapeBlackSizeY[i] = 4;//0726


		//211013 최소 검출사이즈
		m_dPinholeMinSize[i] = 0.1;
		m_dLineMinSize[i] = 0.1;
		m_dScratchMinSize[i] = 0.1;
		m_dCraterMinSize[i] = 0.1;
		m_dDentMinSize[i] = 0.1;
		m_dProtrusionMinSize[i] = 0.1;
		m_dCracknMinSize[i] = 0.1;
		m_dHeatingWrinkleMinSize[i] = 0.1;
		m_dCoatingWrinkleMinSize[i] = 0.1;
		m_dNonCoatMinSize[i] = 0.1;
		m_dIslandMinSize[i] = 0.1;
		m_dITabWrinkleMinSize[i] = 0.1;
		m_dDrossMinSize[i] = 0.1;
		m_dSpatterMinSize[i] = 0.1;
		m_dSpurMinSize[i] = 0.1;

		m_dscale_x1[i] = 0.042;
		m_dscale_y1[i] = 0.084;//0.042;
/////tab Skip 영역

		m_dNoneCoatingSizeX[i] = 0.01;
		m_dNoneCoatingSizeY[i] = 0.5;
		m_nNoneCoatingCount[i] = 3;

	
		m_dTabSkipRange[i][0] = 1;//0604	//탭부 탑 //0614
		m_dTabSkipRange[i][1] = 1;//0604	//탭부 바텀 //0614
		m_dTabSkipRange[i][2] = 1;//0604	//탭부 우측 //0614

		
		m_nTargetBrightness[i] = 128;//전처리 밝기//1412
		m_dMagValue[i] = 3;//증폭 배율
		m_nLineMag[i] = 7; //라인증폭
		m_nHighCut[i] = 5;//하이컷
		m_nLowCut[i] = 5; //로우컷
		m_nLineHighCut[i] = 7;//라인하이컷
		m_nEdgeTh[i] = 30;//에지 TH
		m_bAutoEdgeTh[i] = FALSE;//자동 에지 검출
		m_nTh1[i] = 80;// 표면검사백TH
		m_nTh2[i] = 40;// 표면검사흑TH
		m_dLineCut[i] = 125;//라인TH
		m_nEdgeOffset1[i] = 21;//에지옵셋
		m_nIslandTh[i] = 120;//아일랜드V
		m_nMargin[i][0] = 10;// 백색마진
		m_nMargin[i][1] = 20;// 흑색마진
		m_dXYRatio[i] = 2;//라인비율
		m_nPinholeCut[i] = 80;//핀홀검출값
		m_nPinHoleTh[i] = 60;//원본핀홀TH
		m_nBlackCut1[i] = 20;//분화구 검출값
		m_dBlackPercent[i] = 20;//흑색면적

		//m_bCuttingHeightTopAreaInspUse[i] = FALSE;
		//m_bCuttingHeightTopAreaInspBrightImage[i] = FALSE;
		////m_rtCuttingHeightTopAreaOffset[i] = CRect(0, 0, 0, 0);
		//m_nCuttingHeightTopAreaThreshold[i] = 0;

		//m_bCuttingHeightBottomAreaInspUse[i] = FALSE;
		//m_bCuttingHeightBottomAreaInspBrightImage[i] = FALSE;
		////m_rtCuttingHeightBottomAreaOffset[i] = CRect(0, 0, 0, 0);
		//m_nCuttingHeightBottomAreaThreshold[i] = 0;

		m_fCraterWidth[i] = 3;
		m_fCraterMinimumGrayLevel[i] = 35;

		m_bSizeAndOr[i] = TRUE;//미사용

		if (m_nAnodeCathode) //음극
		{
			m_dPinholeSizeX[i] = 0.3;// 0.15;//핀홀사이즈X
			m_dPinholeSizeY[i] = 0.3;// 0.15;//핀홀사이즈Y
			m_dWhiteSizeX[i] = 5;// 3;//반짝이X
			m_dWhiteSizeY[i] = 5;// 3;//반짝이Y
			m_dDarkSizeX[i] = 3;// 2;//분화구흑사이즈X
			m_dDarkSizeY[i] = 3;// 2;//분화구흑사이즈Y
			m_dPinholeCraterBSizeX[i] = 3;// 2;//핀홀성분화구사이즈X
			m_dPinholeCraterBSizeY[i] = 3;// 2;//핀홀성분화구사이즈Y
			m_dCunicSizeX[i] = 3;// 0.5;//찍힘사이즈X
			m_dCunicSizeY[i] = 3;// 0.5;//찍힘사이즈Y
			m_dWhiteLineSizeX[i] = 0.5;// 0.15;//라인사이즈X
			m_dWhiteLineSizeY[i] = 1;// 0.4;//라인사이즈Y
			m_dBlackLineSizeX[i] = 1;// 0.4;//흑색라인사이즈X //긁힘
			m_dBlackLineSizeY[i] = 4;// 1.6;//흑색라인사이즈Y
			m_dScratchTinyWidth[i] = 3;
			m_dIslandSizeX[i] = 0.75;//아일랜드사이즈X
			m_dIslandSizeY[i] = 0.75;//아일랜드사이즈Y
			m_nReduction[i] = 16;//압축배율
			m_nLargeDefectWTh[i] = 30;//테이프검출백값
			m_nLargeDefectBTh[i] = 20;//테이프검출값흑
			m_dWrinkleBlackSizeX[i] = 0.15;//열주름X
			m_dWrinkleBlackSizeY[i] = 0.15;//열주름Y
			m_dLargeLineSizeX[i] = 2;//거대 라인X
			m_dLargeLineSizeY[i] = 4;//거대 라인Y
			m_dIslandWrikleSizeX[i] = 0.75;//무지부 주름X
			m_dIslandWrikleSizeY[i] = 3;//무지부 주름Y
			m_dCoatWrinkleSizeX[i] = 0.15;//코팅주름X
			m_dCoatWrinkleSizeY[i] = 0.15;//코팅주름Y
			m_nLineInBlackCount[i] = 8;//열주름 내부 흑 Count
			m_dCrackSizeX[i] = 4.5;//크랙X
			m_dCrackSizeY[i] = 4.5;//크랙Y
			m_dOverlaySize[i] = 0.6; //오버레이 폭
			m_dInsulationSize[i] = 3.85; // 절연 폭
			m_dOverlayMinusTolerance[i] = 0.5; //오버레이 차이 값
			m_dOverlayPlusTolerance[i] = 0.5; //오버레이 차이 값
			m_dInsulationMinusTolerance[i] = 0.65; //절연 차이 값
			m_dInsulationPlusTolerance[i] = 0.65; //절연 차이 값
			m_dTabTapeX[i] = 1.5;
			m_dTabTapeY[i] = 1.5;
			m_dCunicSizeX1 = 2;//0729-1 찍힘 약Size
			m_dCunicSizeY1 = 2;//0729-1 찍힘 약Size
		}

		else //양극
		{
			m_dPinholeSizeX[i] = 3;// 2.5;//핀홀사이즈X
			m_dPinholeSizeY[i] = 3;// 2.5;//핀홀사이즈Y
			m_dWhiteSizeX[i] = 3;// 2;//반짝이X
			m_dWhiteSizeY[i] = 3;// 2;//반짝이Y
			m_dDarkSizeX[i] = 6;// 2;//분화구흑사이즈X
			m_dDarkSizeY[i] = 6;// 2;//분화구흑사이즈Y
			m_dPinholeCraterBSizeX[i] = 2;//핀홀성분화구사이즈X
			m_dPinholeCraterBSizeY[i] = 2;//핀홀성분화구사이즈Y
			m_dCunicSizeX[i] = 5;// 2;//찍힘사이즈X
			m_dCunicSizeY[i] = 5;// 2;//찍힘사이즈Y
			m_dWhiteLineSizeX[i] = 1;// 0.15;//라인사이즈X
			m_dWhiteLineSizeY[i] = 2;// 0.4;//라인사이즈Y
			m_dBlackLineSizeX[i] = 1;// 0.4;//흑색라인사이즈X //긁힘
			m_dBlackLineSizeY[i] = 4;// 1.6;//흑색라인사이즈Y
			m_dScratchTinyWidth[i] = 3;
			m_dIslandSizeX[i] = 0.75;//아일랜드사이즈X
			m_dIslandSizeY[i] = 0.75;//아일랜드사이즈Y
			m_nReduction[i] = 16;//압축배율
			m_nLargeDefectWTh[i] = 30;//테이프검출백값
			m_nLargeDefectBTh[i] = 20;//테이프검출값흑
			m_dWrinkleBlackSizeX[i] = 0.15;//열주름X
			m_dWrinkleBlackSizeY[i] = 0.15;//열주름Y
			m_dLargeLineSizeX[i] = 2;//거대 라인X
			m_dLargeLineSizeY[i] = 4;//거대 라인Y
			m_dIslandWrikleSizeX[i] = 0.75;//무지부 주름X
			m_dIslandWrikleSizeY[i] = 3;//무지부 주름Y
			m_dCoatWrinkleSizeX[i] = 0.15;//코팅주름X
			m_dCoatWrinkleSizeY[i] = 0.15;//코팅주름Y
			m_nLineInBlackCount[i] = 8;//열주름 내부 흑 Count
			m_dCrackSizeX[i] = 8;// 4.5;//크랙X
			m_dCrackSizeY[i] = 8;// 4.5;//크랙Y
			m_dOverlaySize[i] = 0.6; //오버레이 폭
			m_dInsulationSize[i] = 3.85; // 절연 폭
			m_dOverlayMinusTolerance[i] = 0.5; //오버레이 차이 값
			m_dOverlayPlusTolerance[i] = 0.5; //오버레이 차이 값
			m_dInsulationMinusTolerance[i] = 0.65; //절연 차이 값
			m_dInsulationPlusTolerance[i] = 0.65; //절연 차이 값
			m_dInsulatePinholeX[i] = 0.75;//절연 핀홀X
			m_dInsulatePinholeY[i] = 0.75;//절연 핀홀Y
			m_dInsulateLineX[i] = 0.15;//절연 라인X
			m_dInsulateLineY[i] = 0.45;//절연 라인Y
			m_dTabTapeX[i] = 0.5;
			m_dTabTapeY[i] = 0.5;
			m_dCunicSizeX1 = 3;//0729-1
			m_dCunicSizeY1 = 3;//0729-1
		}
		//stInspParam.nObject = 2;
		//stInspParam.strName = "전처리 밝기";
		//m_nTargetBrightness[i] = 128;//전처리 밝기//1412
		//stInspParam.dValue = m_nTargetBrightness[i];
		//stInspParam.nIndex = 0;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "증폭배율";
		//m_dMagValue[i] = 3;//증폭 배율
		//stInspParam.dValue = m_dMagValue[i];
		//stInspParam.nIndex = 1;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "라인증폭";
		//m_nLineMag[i] = 7; //라인증폭
		//stInspParam.dValue = m_nLineMag[i];
		//stInspParam.nIndex = 2;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "하이컷";
		//m_nHighCut[i] = 5;//하이컷
		//stInspParam.dValue = m_nHighCut[i];
		//stInspParam.nIndex = 3;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "로우컷";
		//m_nLowCut[i] = 5;
		//stInspParam.dValue = m_nLowCut[i];
		//stInspParam.nIndex = 4;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "라인하이컷";
		//m_nLineHighCut[i] = 7;
		//stInspParam.dValue = m_nLineHighCut[i];
		//stInspParam.nIndex = 5;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "에지TH";
		//m_nEdgeTh[i] = 30;//에지 TH
		//stInspParam.dValue = m_nEdgeTh[i];//60;
		//stInspParam.nIndex = 6;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "자동에지TH";
		//m_bAutoEdgeTh[i] = FALSE;//자동 에지 검출
		//stInspParam.dValue = m_bAutoEdgeTh[i];
		//stInspParam.nIndex = 7;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "표면검사백TH";
		//m_nTh1[i] = 65;//
		//stInspParam.dValue = m_nTh1[i];
		//stInspParam.nIndex = 8;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "표면검사흑TH";
		//m_nTh2[i] = 30;//
		//stInspParam.dValue = m_nTh2[i];//55;
		//stInspParam.nIndex = 9;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "라인TH";
		//m_dLineCut[i] = 125;
		//stInspParam.dValue = m_dLineCut[i];
		//stInspParam.nIndex = 10;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "에지옵셋";
		//m_nEdgeOffset1[i] = 21;
		//stInspParam.dValue = m_nEdgeOffset1[i];
		//stInspParam.nIndex = 11;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "아일랜드V";
		//m_nIslandTh[i] = 120;
		//stInspParam.dValue = m_nIslandTh[i];
		//stInspParam.nIndex = 12;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "백색마진";
		//m_nMargin[i][0] = 30;// TH Margin //0 white 1 balck
		//stInspParam.dValue = m_nMargin[i][0];
		//stInspParam.nIndex = 13;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "흑색마진";
		//m_nMargin[i][1] = 10;// TH Margin //0 white 1 balck //@0514
		//stInspParam.dValue = m_nMargin[i][1];
		//stInspParam.nIndex = 14;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "라인비율";
		//m_dXYRatio[i] = 4;
		//stInspParam.dValue = m_dXYRatio[i];
		//stInspParam.nIndex = 15;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "핀홀검출값";
		//m_nPinholeCut[i] = 80;//핀홀검출값
		//stInspParam.dValue = m_nPinholeCut[i];
		//stInspParam.nIndex = 16;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "원본핀홀TH";
		//m_nPinHoleTh[i] = 60;//원본핀홀TH
		//stInspParam.dValue = m_nPinHoleTh[i];
		//stInspParam.nIndex = 17;
		//m_vInspParam.push_back(stInspParam);
		//stInspParam.strName = "흑분화구검출값";
		//m_nBlackCut1[i] = 20;//분화구 검출값
		//stInspParam.dValue = m_nBlackCut1[i];
		//stInspParam.nIndex = 18;
		//m_dBlackPercent[i] = 20;//흑색면적
		//m_bSizeAndOr[i] = TRUE;//미사용
		//if (nAnodeCathode){
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.nObject = 0;
		//	stInspParam.strName = "핀홀사이즈X";
		//	m_dPinholeSizeX[i] = 0.15;//3;//핀홀사이즈X
		//	stInspParam.dValue = m_dPinholeSizeX[i];
		//	stInspParam.nIndex = 19;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "핀홀사이즈Y";
		//	m_dPinholeSizeY[i] = 0.15;//3;//핀홀사이즈Y
		//	stInspParam.dValue = m_dPinholeSizeY[i];
		//	stInspParam.nIndex = 20;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "반짝이사이즈X";
		//	m_dWhiteSizeX[i] = 3;//80;//반짝이/돌출
		//	stInspParam.dValue = m_dWhiteSizeX[i];
		//	stInspParam.nIndex = 21;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "반짝이사이즈Y";
		//	m_dWhiteSizeY[i] = 3;//80;//반짝이
		//	stInspParam.dValue = m_dWhiteSizeY[i];
		//	stInspParam.nIndex = 22;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "흑색분화구사이즈X";
		//	m_dDarkSizeX[i] = 2;//분화구흑사이즈X
		//	stInspParam.dValue = m_dDarkSizeX[i];
		//	stInspParam.nIndex = 23;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "흑색분화구사이즈Y";
		//	m_dDarkSizeY[i] = 2;//분화구흑사이즈Y
		//	stInspParam.dValue = m_dDarkSizeY[i];
		//	stInspParam.nIndex = 24;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "핀홀성분화구사이즈X";
		//	m_dPinholeCraterBSizeX[i] = 2;////핀홀성분화구사이즈X
		//	stInspParam.dValue = m_dPinholeCraterBSizeX[i];
		//	stInspParam.nIndex = 25;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "핀홀성분화구사이즈Y";
		//	m_dPinholeCraterBSizeY[i] = 2;//핀홀성분화구사이즈Y
		//	stInspParam.dValue = m_dPinholeCraterBSizeY[i];
		//	stInspParam.nIndex = 26;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "찍힘사이즈X";
		//	m_dCunicSizeX[i] = 0.5;//2//찍힘사이즈X
		//	stInspParam.dValue = m_dCunicSizeX[i];//3.5
		//	stInspParam.nIndex = 27;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "찍힘사이즈Y";
		//	m_dCunicSizeY[i] = 0.5;//2;//찍힘사이즈Y
		//	stInspParam.dValue = m_dCunicSizeY[i];//3.5
		//	stInspParam.nIndex = 28;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "라인사이즈X";
		//	m_dWhiteLineSizeX[i] = 0.15;//라인사이즈X
		//	stInspParam.dValue = m_dWhiteLineSizeX[i];
		//	stInspParam.nIndex = 29;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "라인사이즈Y";
		//	m_dWhiteLineSizeY[i] = 0.4;//라인사이즈Y
		//	stInspParam.dValue = m_dWhiteLineSizeY[i];
		//	stInspParam.nIndex = 30;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "흑라인사이즈X";
		//	m_dBlackLineSizeX[i] = 0.4;//흑색라인사이즈X //긁힘
		//	stInspParam.dValue = m_dBlackLineSizeX[i];
		//	stInspParam.nIndex = 31;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "흑라인사이즈Y";
		//	m_dBlackLineSizeY[i] = 1.6;//흑색라인사이즈Y
		//	stInspParam.dValue = m_dBlackLineSizeY[i];
		//	stInspParam.nIndex = 32;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "아일랜드사이즈X";
		//	m_dIslandSizeX[i] = 0.75;//아일랜드사이즈X
		//	stInspParam.dValue = m_dIslandSizeX[i];
		//	stInspParam.nIndex = 33;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "아일랜드사이즈Y";
		//	m_dIslandSizeY[i] = 0.75;//아일랜드사이즈Y
		//	stInspParam.dValue = m_dIslandSizeY[i];
		//	stInspParam.nIndex = 34;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "압축배율";
		//	m_nReduction[i] = 16;//압축배율
		//	stInspParam.dValue = m_nReduction[i];
		//	stInspParam.nIndex = 35;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "테이프검출백값";
		//	m_nLargeDefectWTh[i] = 30;
		//	stInspParam.dValue = m_nLargeDefectWTh[i];
		//	stInspParam.nIndex = 36;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "테이프검출흑값";
		//	m_nLargeDefectBTh[i] = 30;//테이프검출값흑
		//	stInspParam.dValue = m_nLargeDefectBTh[i];
		//	stInspParam.nIndex = 37;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "열주름사이즈X";
		//	m_dWrinkleBlackSizeX[i] = 0.15;//열주름
		//	stInspParam.dValue = m_dWrinkleBlackSizeX[i];
		//	stInspParam.nIndex = 38;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "열주름사이즈Y";
		//	m_dWrinkleBlackSizeY[i] = 0.15;//열주름
		//	stInspParam.dValue = m_dWrinkleBlackSizeY[i];
		//	stInspParam.nIndex = 39;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "거대라인X";
		//	m_dLargeLineSizeX[i] = 2;//거대 라인
		//	stInspParam.dValue = m_dLargeLineSizeX[i];
		//	stInspParam.nIndex = 40;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "거대라인Y";
		//	m_dLargeLineSizeY[i] = 4;//거대 라인
		//	stInspParam.dValue = m_dLargeLineSizeY[i];
		//	stInspParam.nIndex = 41;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "무지주름X";
		//	m_dIslandWrikleSizeX[i] = 0.75;//무지부 주름
		//	stInspParam.dValue = m_dIslandWrikleSizeX[i];
		//	stInspParam.nIndex = 42;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "무지주름Y";
		//	m_dIslandWrikleSizeY[i] = 3;//무지부 주름
		//	stInspParam.dValue = m_dIslandWrikleSizeY[i];
		//	stInspParam.nIndex = 43;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "코팅주름X";
		//	m_dCoatWrinkleSizeX[i] = 0.15;//코팅주름
		//	stInspParam.dValue = m_dCoatWrinkleSizeX[i];
		//	stInspParam.nIndex = 44;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "코팅주름Y";
		//	m_dCoatWrinkleSizeY[i] = 0.15;//코팅주름
		//	stInspParam.dValue = m_dCoatWrinkleSizeY[i];
		//	stInspParam.nIndex = 45;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "열주름흑";
		//	m_nLineInBlackCount[i] = 8;//열주름 내부 흑 Count
		//	stInspParam.dValue = m_nLineInBlackCount[i];
		//	stInspParam.nIndex = 46;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "크랙사이즈X";
		//	m_dCrackSizeX[i] = 4.5;//크랙
		//	stInspParam.dValue = m_dCrackSizeX[i];
		//	stInspParam.nIndex = 47;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "크랙사이즈Y";
		//	m_dCrackSizeY[i] = 4.5;//크랙
		//	stInspParam.dValue = m_dCrackSizeY[i];
		//	stInspParam.nIndex = 48;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "오버레이 폭";
		//	m_dOverlaySize[i] = 0.6; //오버레이 폭
		//	stInspParam.dValue = m_dOverlaySize[i];
		//	stInspParam.nIndex = 49;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "절연 폭";
		//	m_dInsulationSize[i] = 3.85;
		//	stInspParam.dValue = m_dInsulationSize[i];
		//	stInspParam.nIndex = 50;
		//	m_vInspParam.push_back(stInspParam);
		//}
		//else{
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.nObject = 1;
		//	stInspParam.strName = "핀홀사이즈X";
		//	m_dPinholeSizeX[i] = 2.5;//3;//핀홀사이즈X
		//	stInspParam.dValue = m_dPinholeSizeX[i];
		//	stInspParam.nIndex = 19;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "핀홀사이즈Y";
		//	m_dPinholeSizeY[i] = 2.5;//3;//핀홀사이즈Y
		//	stInspParam.dValue = m_dPinholeSizeY[i];
		//	stInspParam.nIndex = 20;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "반짝이사이즈X";
		//	m_dWhiteSizeX[i] = 2;//80;//반짝이/돌출
		//	stInspParam.dValue = m_dWhiteSizeX[i];
		//	stInspParam.nIndex = 21;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "반짝이사이즈Y";
		//	m_dWhiteSizeY[i] = 2;//80;//반짝이
		//	stInspParam.dValue = m_dWhiteSizeY[i];
		//	stInspParam.nIndex = 22;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "흑색분화구사이즈X";
		//	m_dDarkSizeX[i] = 2;//분화구흑사이즈X
		//	stInspParam.dValue = m_dDarkSizeX[i];
		//	stInspParam.nIndex = 23;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "흑색분화구사이즈Y";
		//	m_dDarkSizeY[i] = 2;//분화구흑사이즈Y
		//	stInspParam.dValue = m_dDarkSizeY[i];
		//	stInspParam.nIndex = 24;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "핀홀성분화구사이즈X";
		//	m_dPinholeCraterBSizeX[i] = 2;////핀홀성분화구사이즈X
		//	stInspParam.dValue = m_dPinholeCraterBSizeX[i];
		//	stInspParam.nIndex = 25;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "핀홀성분화구사이즈Y";
		//	m_dPinholeCraterBSizeY[i] = 2;//핀홀성분화구사이즈Y
		//	stInspParam.dValue = m_dPinholeCraterBSizeY[i];
		//	stInspParam.nIndex = 26;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "찍힘사이즈X";
		//	m_dCunicSizeX[i] = 2;//2//찍힘사이즈X
		//	stInspParam.dValue = m_dCunicSizeX[i];//3.5
		//	stInspParam.nIndex = 27;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "찍힘사이즈Y";
		//	m_dCunicSizeY[i] = 2;//2;//찍힘사이즈Y
		//	stInspParam.dValue = m_dCunicSizeY[i];//3.5
		//	stInspParam.nIndex = 28;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "라인사이즈X";
		//	m_dWhiteLineSizeX[i] = 0.15;//라인사이즈X
		//	stInspParam.dValue = m_dWhiteLineSizeX[i];
		//	stInspParam.nIndex = 29;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "라인사이즈Y";
		//	m_dWhiteLineSizeY[i] = 0.4;//라인사이즈Y
		//	stInspParam.dValue = m_dWhiteLineSizeY[i];
		//	stInspParam.nIndex = 30;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "흑라인사이즈X";
		//	m_dBlackLineSizeX[i] = 0.4;//흑색라인사이즈X //긁힘
		//	stInspParam.dValue = m_dBlackLineSizeX[i];
		//	stInspParam.nIndex = 31;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "흑라인사이즈Y";
		//	m_dBlackLineSizeY[i] = 1.6;//흑색라인사이즈Y
		//	stInspParam.dValue = m_dBlackLineSizeY[i];
		//	stInspParam.nIndex = 32;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "아일랜드사이즈X";
		//	m_dIslandSizeX[i] = 0.75;//아일랜드사이즈X
		//	stInspParam.dValue = m_dIslandSizeX[i];
		//	stInspParam.nIndex = 33;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "아일랜드사이즈Y";
		//	m_dIslandSizeY[i] = 0.75;//아일랜드사이즈Y
		//	stInspParam.dValue = m_dIslandSizeY[i];
		//	stInspParam.nIndex = 34;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "압축배율";
		//	m_nReduction[i] = 16;//압축배율
		//	stInspParam.dValue = m_nReduction[i];
		//	stInspParam.nIndex = 35;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "테이프검출백값";
		//	m_nLargeDefectWTh[i] = 30;
		//	stInspParam.dValue = m_nLargeDefectWTh[i];
		//	stInspParam.nIndex = 36;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "테이프검출흑값";
		//	m_nLargeDefectBTh[i] = 30;//테이프검출값흑
		//	stInspParam.dValue = m_nLargeDefectBTh[i];
		//	stInspParam.nIndex = 37;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "열주름사이즈X";
		//	m_dWrinkleBlackSizeX[i] = 0.15;//열주름
		//	stInspParam.dValue = m_dWrinkleBlackSizeX[i];
		//	stInspParam.nIndex = 38;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "열주름사이즈Y";
		//	m_dWrinkleBlackSizeY[i] = 0.15;//열주름
		//	stInspParam.dValue = m_dWrinkleBlackSizeY[i];
		//	stInspParam.nIndex = 39;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "거대라인X";
		//	m_dLargeLineSizeX[i] = 2;//거대 라인
		//	stInspParam.dValue = m_dLargeLineSizeX[i];
		//	stInspParam.nIndex = 40;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "거대라인Y";
		//	m_dLargeLineSizeY[i] = 4;//거대 라인
		//	stInspParam.dValue = m_dLargeLineSizeY[i];
		//	stInspParam.nIndex = 41;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "무지주름X";
		//	m_dIslandWrikleSizeX[i] = 0.75;//무지부 주름
		//	stInspParam.dValue = m_dIslandWrikleSizeX[i];
		//	stInspParam.nIndex = 42;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "무지주름Y";
		//	m_dIslandWrikleSizeY[i] = 3;//무지부 주름
		//	stInspParam.dValue = m_dIslandWrikleSizeY[i];
		//	stInspParam.nIndex = 43;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "코팅주름X";
		//	m_dCoatWrinkleSizeX[i] = 0.15;//코팅주름
		//	stInspParam.dValue = m_dCoatWrinkleSizeX[i];
		//	stInspParam.nIndex = 44;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "코팅주름Y";
		//	m_dCoatWrinkleSizeY[i] = 0.15;//코팅주름
		//	stInspParam.dValue = m_dCoatWrinkleSizeY[i];
		//	stInspParam.nIndex = 45;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "열주름흑";
		//	m_nLineInBlackCount[i] = 8;//열주름 내부 흑 Count
		//	stInspParam.dValue = m_nLineInBlackCount[i];
		//	stInspParam.nIndex = 46;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "크랙사이즈X";
		//	m_dCrackSizeX[i] = 4.5;//크랙
		//	stInspParam.dValue = m_dCrackSizeX[i];
		//	stInspParam.nIndex = 47;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "크랙사이즈y";
		//	m_dCrackSizeY[i] = 4.5;//크랙
		//	stInspParam.dValue = m_dCrackSizeY[i];
		//	stInspParam.nIndex = 48;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "오버레이 폭";
		//	m_dOverlaySize[i] = 0.6;
		//	stInspParam.dValue = m_dOverlaySize[i];
		//	stInspParam.nIndex = 49;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "절연 폭";
		//	m_dInsulationSize[i] = 3.85;
		//	stInspParam.dValue = m_dInsulationSize[i];
		//	stInspParam.nIndex = 50;
		//	m_vInspParam.push_back(stInspParam);

		//	m_dInsulatePinholeX;//절연 핀홀 0.75
		//	m_dInsulatePinholeY;//절연 핀홀 0.75
		//	m_dInsulateLineX;//절연 라인 
		//	m_dInsulateLineY;//절연 라인
		//	m_dInsulateGapLineX;//절연 핀홀 0.2
		//	m_dInsulateGapLineY;//절연 핀홀 10
		//	stInspParam.strName = "절연핀홀X";//0517-4
		//	m_dInsulatePinholeX[i] = 0.75;//
		//	stInspParam.dValue = m_dInsulatePinholeX[i];
		//	stInspParam.nIndex = 49;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "절연핀홀Y";//0517-4
		//	m_dInsulatePinholeY[i] = 0.75;//
		//	stInspParam.dValue = m_dInsulatePinholeY[i];
		//	stInspParam.nIndex = 50;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "절연라인X";//0517-4
		//	m_dInsulateLineX[i] = 0.15;//
		//	stInspParam.dValue = m_dInsulateLineX[i];
		//	stInspParam.nIndex = 51;
		//	m_vInspParam.push_back(stInspParam);
		//	stInspParam.strName = "절연라인Y";//0517-4
		//	m_dInsulateLineY[i] = 0.45;//
		//	stInspParam.dValue = m_dInsulateLineY[i];
		//	stInspParam.nIndex = 52;
		//	m_vInspParam.push_back(stInspParam);
		//}
	}
}
double CInspect::GetPrecisionTime(void)
{
	LARGE_INTEGER lpFrequency;
	LARGE_INTEGER lpPerformanceCount;
	QueryPerformanceFrequency(&lpFrequency);
	QueryPerformanceCounter(&lpPerformanceCount);
	return  (double)lpPerformanceCount.QuadPart /(double)lpFrequency.QuadPart;
}
CRect CInspect::FlipRect(CRect rect,int nh)
{
//	return rect; //주석 풀면 Rect 뒤집지 않음
	CRect rt;
	rt=rect;
	rt.top = nh-rect.bottom;rt.bottom=nh-rect.top;
	return rt;
}
CString CInspect::GetDefectName(int nCode)
{
	CString str;
	//if (nCode == 1001)		str.Format(_T("%s"), _T("거대라인"));
	//else if (nCode == 1002)	str.Format(_T("%s"), _T("백색라인"));
	//else if (nCode == 1003)	str.Format(_T("%s"), _T("라인긁힘"));
	//else if (nCode == 1004)	str.Format(_T("%s"), _T("바운드라인"));
	//else if (nCode == 1005)	str.Format(_T("%s"), _T("열주름"));
	//else if (nCode == 1006)	str.Format(_T("%s"), _T("코팅주름"));
	//else if (nCode == 2001)	str.Format(_T("%s"), _T("핀홀"));
	//else if (nCode == 2002)	str.Format(_T("%s"), _T("반짝이"));//돌출
	//else if (nCode == 2003)	str.Format(_T("%s"), _T("찍힘"));
	//else if (nCode == 2004)	str.Format(_T("%s"), _T("파임"));
	//else if (nCode == 3001)	str.Format(_T("%s"), _T("핀홀성분화구"));
	//else if (nCode == 3002)	str.Format(_T("%s"), _T("흑색분화구"));
	//else if (nCode == 3003)	str.Format(_T("%s"), _T("흑색크랙"));
	//else if (nCode == 4001)	str.Format(_T("%s"), _T("미코팅"));
	//else if (nCode == 5001)	str.Format(_T("%s"), _T("아일랜드"));
	//else if (nCode == 5002)	str.Format(_T("%s"), _T("무지부주름"));
	//else if (nCode == 6001)	str.Format(_T("%s"), _T("Tape"));
	//else if (nCode == 6002)	str.Format(_T("%s"), _T("거대백불량"));
	//else if (nCode == 6003)	str.Format(_T("%s"), _T("거대흑불량"));
	//else					str.Format(_T("%s"), _T("양품"));

	if (nCode == 1001)		str.Format(_T("%s"), _T("Line"));
	else if (nCode == 1002)	str.Format(_T("%s"), _T("Line"));
	else if (nCode == 1003)	str.Format(_T("%s"), _T("Scratch"));
	else if (nCode == 1004)	str.Format(_T("%s"), _T("Line"));
	else if (nCode == 1005)	str.Format(_T("%s"), _T("Heating Wrinkle"));
	else if (nCode == 1006)	str.Format(_T("%s"), _T("Coating Wrinkle"));
	else if (nCode == 2000)	str.Format(_T("%s"), _T("Mini Pinhole"));
	else if (nCode == 2001)	str.Format(_T("%s"), _T("Pinhole"));
	else if (nCode == 2002)	str.Format(_T("%s"), _T("Protrusion"));//돌출
	else if (nCode == 2003)	str.Format(_T("%s"), _T("Dent"));
	else if (nCode == 2004)	str.Format(_T("%s"), _T("Dent"));
	else if (nCode == 2005)	str.Format(_T("%s"), _T("Mini Dent"));
	else if (nCode == 3000)	str.Format(_T("%s"), _T("Mini Crater"));
	else if (nCode == 3001)	str.Format(_T("%s"), _T("Crater"));
	else if (nCode == 3002)	str.Format(_T("%s"), _T("Crater"));
	else if (nCode == 3003)	str.Format(_T("%s"), _T("Crack"));
	else if (nCode == 4001)	str.Format(_T("%s"), _T("None Coating"));
	else if (nCode == 5001)	str.Format(_T("%s"), _T("Insulation Island"));
	else if (nCode == 5002)	str.Format(_T("%s"), _T("Wrinkle"));
	else if (nCode == 6001)	str.Format(_T("%s"), _T("Connection Tape"));
	else if (nCode == 6002)	str.Format(_T("%s"), _T("Protrusion"));
	else if (nCode == 6003)	str.Format(_T("%s"), _T("Crater"));
	else if (nCode == 6004)	str.Format(_T("%s"), _T("NGTag"));
	else if (nCode == 6005)	str.Format(_T("%s"), _T("Barcode Marks")); // 검은색 테이프 자국
	else if (nCode == 7001)	str.Format(_T("%s"), _T("Pinhole"));//절연 핀홀
	else if (nCode == 7002)	str.Format(_T("%s"), _T("Line"));//절연 라인
#ifdef _INSULATESURFACE_
	else if (nCode == 7001)	str.Format(_T("%s"), _T("InsulationPinhole"));//절연 핀홀
	else if (nCode == 7002)	str.Format(_T("%s"), _T("InsulationLine"));//절연 라인
	//else if (nCode == 7004)	str.Format(_T("%s"), _T("Insulation Island"));//절연 아일랜드
#else
	else if (nCode == 7001)	str.Format(_T("%s"), _T("Pinhole"));//절연 핀홀
	else if (nCode == 7002)	str.Format(_T("%s"), _T("Line"));//절연 라인
#endif
//	else if (nCode == 7003)	str.Format(_T("%s"), _T("절연갭"));
	else if (nCode == 8001)	str.Format(_T("%s"), _T("Burr"));
	else if (nCode == 8002) str.Format(_T("%s"), _T("Dross"));
	else if (nCode == 8003) str.Format(_T("%s"), _T("Spatter"));
	else if (nCode == 9001) str.Format(_T("%s"), _T("Scrab"));//0930
	else					str.Format(_T("%s"), _T("OK"));

	return str;
}

void CInspect::PreprocessEdge(int nCam)
{
	m_nInsulateDefectCount[nCam] = 0;
	m_dTabBrightness[nCam][0] = m_dTabBrightness[nCam][1] = 0;//0720
	m_dInsSubTime1[nCam][0] = GetPrecisionTime();
	
	double dStartTime = m_dInsSubTime1[nCam][0];

	float fTimeLimit = m_fLimitTime[nCam] - 10.0f;

	m_nPreprocessEdgeBreakStep[nCam] = -1;

	m_bNoTab = false;

	if (m_ptTabCenter[nCam].x == 0 && m_ptTabCenter[nCam].y == 0)//탭이 없음 //211009
	{
		m_bNoTab = true;
		return;
	}

	m_bOverlayCheck[nCam] = TRUE;
	int nw = m_nWidth;
	int nh = m_nHeight;
	int nCut = 128;
	BYTE * fm = m_fmEven[nCam];//bright
	BYTE * fmDark = m_fmOdd[nCam];//dark
	BYTE * fmTmp = m_fmEven[nCam];//bright
	if(m_bWhiteRoll[nCam]==TRUE)
		fmTmp = m_fmOdd[nCam];//dark
	CRect rttab;
	rttab.SetRect(m_rtMinTab[nCam].left*m_nReduction[nCam], m_rtMinTab[nCam].top*m_nReduction[nCam], m_rtMinTab[nCam].right*m_nReduction[nCam], m_rtMinTab[nCam].bottom*m_nReduction[nCam]);
	rttab = SetBoundary2(rttab, CRect(80, 0, nw - 100, nh));
	CRect rttmp = rttab;//rt;
	CRect rttmpbottom = rttab; //0801 longcell bottom

	m_rtOverlay[nCam].SetRect(0, 0, 0, 0);
	m_nIslandCount[nCam] = 0;
	int ncnt = 0, ncnt1 = 0;
	//	if(m_rtHugeRect[nCam].Width()<100 || m_rtHugeRect[nCam].Height()<20 || rttmp.Width()<50||rttmp.Height()<200)		return;//211003
	int  nProj[3000] = { 0, };// = new int[sizeof(int)*nh];	memset(nProj,0,sizeof(int)*nh);
	int nProjmin[3000] = { 0, };// = new int[sizeof(int)*nh];	memset(nProjmin,0,sizeof(int)*nh);
	int nProjmax[3000] = { 0, };// = new int[sizeof(int)*nh];	memset(nProjmax,0,sizeof(int)*nh);
	int nProjTab[3000] = { 0, };// = new int[sizeof(int)*nh];	memset(nProjTab,0,sizeof(int)*nh);
	int nProjTab1[3000] = { 0, };//0611// = new int[sizeof(int)*nh];	memset(nProjTab,0,sizeof(int)*nh);
	int nProj1[10000] = { 0, };//= new int[sizeof(int)*nw];	memset(nProj1,0,sizeof(int)*nw);//0604
	int nProj2[10000] = { 0, };
	int nProj11[10][2000];//, nInsulateSplit[10][2000];//0604
	memset(nProj11, 0, sizeof(int) * 10 * 2000);//이번에 초기화 추가한건데 기존에 초기화 코드가 없었어서 제대로 동작했을지 모르겠음, 버그였을거 같음
	//for (int i = 0; i<10; i++)for (int j = 0; j<2000; j++) {
	//	nInsulateSplit[i][j] = nProj11[i][j] = 0;//0604
	//}
	BYTE * fmPreProcess = new BYTE[nw*nh];//m_fmOverlayPreprocess[nCam];//new BYTE[nw*nh];//220314
	memset(fmPreProcess, 0, nw*nh);
	int nTmp = 0, nValue = m_nTargetBrightness[nCam], nminave = 0, nmaxave = 0, nRatio = 5, nTabave1 = 0;
	int nCoatEdge1 = m_rtCell[nCam].right, nOverlay = 0, nEdgeTh1 = 40, nEdgeTh2 = 25, nt1 = rttmp.top, nt2 = rttmp.bottom, nMargin = 30;//211123
	int nCellRightCellAreaPoint = m_rtCell[nCam].right;
	m_rtInsulate[nCam].SetRect(0, 0, 0, 0);
	m_rtCuttingHeightTop[nCam].SetRect(0, 0, 0, 0);
	m_rtCuttingHeightBottom[nCam].SetRect(0, 0, 0, 0);
	m_nFullDefectCount[nCam] = 0;	//notch
	for (int i = 0; i<MAX_HUGEDEFECT; i++)m_dISSize[nCam][i] = 0;//0531
	CRect rtEdge[2];
	int nNonCotrange = 32;//0706
	rtEdge[0].SetRect(m_rtHugeRect[nCam].left - nNonCotrange * 2, 0, m_rtHugeRect[nCam].left + nNonCotrange*6, nh);//0706
	rtEdge[1].SetRect(m_rtHugeRect[nCam].right - nNonCotrange * 4, 0, m_rtHugeRect[nCam].right + nNonCotrange * 6/*nNonCotrange * 2*/, nh);//0706//1006
	rtEdge[0] = SetBoundary2(rtEdge[0], CRect(80, 0, nw - 100, nh));
	rtEdge[1] = SetBoundary2(rtEdge[1], CRect(80, 0, nw - 100, nh));
	int midy = rtEdge[0].Height()*0.5;
	int nEdgeY[3000] = { 0, }, nEdgeY1[3000] = { 0, };
	int ntmpold[2] = { 0, }, noldpos[2] = { 0, }, noldposy[2] = { 0, }, npos = 0;
	int nEdgeTh11 = m_nEdgeTh[nCam]; 25;//30;
	BYTE * fmtmp1 = m_fmTabEdge1[nCam];//new BYTE[nw*nh];
	int nSumEdge1 = 0;
	if(m_bWhiteRoll[nCam])
		nSumEdge1 = PreprocessingCeramic(fmDark, fmtmp1, rtEdge[0], nw, nh);
	else
		nSumEdge1 = PreprocessingCeramic(fm, fmtmp1, rtEdge[0], nw, nh);


	double dCoatEdge1[4] = { 0, };
	int nSumEdge2 = PreprocessingCeramic(fmtmp1, m_fmTabEdge[nCam], rtEdge[1], nw, nh);
	m_dInsSubTime1[nCam][3] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000; //7ms
	double dCoatTopEdge = 0, dCoatBtmEdge = 0;//211002
	int nxoffset1 = 8,ncnt12[2] = {0,};//4
	int nPeakPos1 = 0,nPeakPos2=0;
	int nNoncoatEdge1 = 220;
	if(m_bWhiteRoll[nCam]==TRUE){
//		fmTmp = fmDark;
		nNoncoatEdge1 = 180;//(nSumEdge1 +nSumEdge2)*0.5 +10 ;//180;//211101
	}
	dCoatEdge1[0] = m_rtCell[nCam].left;
	dCoatEdge1[1] = dCoatTopEdge = m_rtCell[nCam].right;
	 
	///////////////////////////////////////////////////////////////0615-1 미코팅 탈리
	int nIslandTh = m_nIslandTh[nCam], nw2 = BAD_WIDTH*0.5, nh2 = BAD_HEIGHT*0.5, njump1 = 0;
	int nDefect = 0, ntabpos1[10] = { 0, };
	if (m_rtCell[nCam].Width()<1500)		m_rtCell[nCam].right = m_rtHugeRect[nCam].right*m_nReduction[nCam];//0524-2

	//tab & overlay  //탭 상하 찾기
	double dave1 = 0, dc1 = 0;
	if (rttmp.top<nt1)rttmp.top = nt1;//0623
	if (rttmp.bottom>nt2)rttmp.bottom = nt2;//0623
	if (rttmp.left < dCoatTopEdge)rttmp.left = dCoatTopEdge+10;//211006 //탭 에지를 찾기위해 리사이징
	if (rttmp.Width() < 100)rttmp.right = rttmp.left + 100;//211006
	if (rttmp.Height() < 300)rttmp.bottom = nh - 150;//211007
	if (rttmp.top -16 < 0)rttmp.top = 16;//211129
	if (rttmp.bottom > nh - 16)//211129
		rttmp.bottom = nh - 16;
	CRect rttmp12;
	rttmp12 = rttmp;//백색롤러에서 화이트가 적을경우 
		//220110  
	int ntabave11 = 0;

	if (m_bUseIntekTabAlign[nCam] == FALSE)
	{
		ntabave11 = FindTabPos(nCam, fmTmp, fmDark, nw, nh, rttmp12, &nt1, &nt2);//220304

		m_dInsSubTime1[nCam][4] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

		if (nt2 == 0 && nt1 == 0) {
			delete[] fmPreProcess;
			return;
		}
		if (nt2 - nt1 <= 200 /*|| (nTabFind1[0] == 0 || nTabFind1[0] == 0)*/) {//211009
			nt1 = m_ptTabCenter[nCam].y * m_nReduction[nCam] - 200;
			nt2 = m_ptTabCenter[nCam].y * m_nReduction[nCam] + 200;
		}
		if (nt2 - nt1 <= 100 && ntabpos1[0] > 0) {//0707
			nt2 = ntabpos1[1];//0707
			nt1 = ntabpos1[0];//0707
		}
		if (rttmp.top < nt1)rttmp.top = nt1;//0623
		if (rttmp.bottom > nt2)rttmp.bottom = nt2;//0623
	//	if(nt2-nt1 > 800 ){
		if (nt2 - nt1 > 1500) {//220418
			int nmid11 = nt2 - nt1;
			nt2 = nmid11 + 250;
			nt1 = nmid11 - 250;
			if (nt1 < 0)nt1 = 50;
			if (nt2 > nh)nt2 = nh - 50;
		}
	}
	else
	{
		if(m_dequeTabAvg[nCam].size() > 0)
		{
			CSingleLock lock(&m_lockTabAvg[nCam], TRUE);
			ntabave11 = m_dequeTabAvg[nCam].front();//220304
			m_dequeTabAvg[nCam].pop_front();
		}
		if(m_dequeTabTopY[nCam].size() > 0)
		{
			CSingleLock lock(&m_lockTabTopY[nCam], TRUE);
			nt1 = m_dequeTabTopY[nCam].front();
			m_dequeTabTopY[nCam].pop_front();
		}
		if(m_dequeTabBottomY[nCam].size() > 0)
		{
			CSingleLock lock(&m_lockTabBottomY[nCam], TRUE);
			nt2 = m_dequeTabBottomY[nCam].front();
			m_dequeTabBottomY[nCam].pop_front();
		}
		m_dInsSubTime1[nCam][4] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;
	}

	rtEdge[1].SetRect(m_rtCell[nCam].right - nNonCotrange * 6, 0, m_rtCell[nCam].right + nNonCotrange * 4/*nNonCotrange * 2*/, nh);//0706//1006
	rtEdge[1] = SetBoundary2(rtEdge[1], CRect(80, 0, nw - 100, nh));

	CRect rtcsrab1;
	if(m_bInsScrab[nCam]){
		rtcsrab1.SetRect(dCoatTopEdge - m_nScrabRoiLeft[nCam], m_nScrabRoiTop[nCam], dCoatTopEdge - m_nScrabRoiRight[nCam], nh - m_nScrabRoiBot[nCam]);
		rtcsrab1 = rtcsrab1 & CRect(m_rtCell[nCam].left, 0, m_rtCell[nCam].right ,nh);
		m_rtScrabIns[nCam] = rtcsrab1;
	}
//esmb 속도관련 위치 이동
	//if(m_bWhiteRoll[nCam])
	//	nDefect = FindNonCoating(nCam, fm, fmDark, m_nWidth, m_nHeight, nIslandTh, rtEdge[0], rtEdge[1], nNoncoatEdge1, dCoatEdge1, nt1-16, nt2+16, ntabpos1);//211003
	//else
	//	nDefect = FindNonCoating(nCam, fm, fmDark, m_nWidth, m_nHeight, nIslandTh, rtEdge[0], rtEdge[1], (nSumEdge1 + nSumEdge2)*0.5, dCoatEdge1, nt1-16, nt2+16, ntabpos1);//211003

	if ((GetPrecisionTime() - dStartTime) * 1000.0f > fTimeLimit)
	{
		m_nPreprocessEdgeBreakStep[nCam] = 0;
		m_dLastTime[nCam] = m_dInsSubTime1[nCam][4];
		return;
	}

	int nmid = (nt2 + nt1)*0.5;
	CRect rttabins, rttabins1;//@0514
	int nTabInsOffset = (m_dTabEdgeInspRange[nCam] / m_dscale_y1[nCam])*0.5;//중심에서 상하로 구분되어서 /2를 해준다. //220513
	int nTabRightOffset = m_dTabRightOffset[nCam] / m_dscale_x1[nCam];//전극 에지에서 우측으로 옵셋을 주고 좌측으로 에지를 검색함. //220513
	rttabins.SetRect(m_rtHugeRect[nCam].right, nmid - nTabInsOffset, m_rtHugeRect[nCam].right + nTabRightOffset, nmid + nTabInsOffset);//600픽셀->450->480픽셀로 수젇 //220330 //220513 
	m_rectTabRightDetectROI[nCam] = rttabins;
	rttabins = SetBoundary2(rttabins, CRect(80, 0, nw - 100, nh));
	nRatio = 6;


	m_dInsSubTime1[nCam][5] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;


	int noversplitcnt = 0, noverslitnh = (nt2 - nt1) / 10, noverwidth = rttmp.Width();//0604
	if (noverslitnh == 0) noverslitnh = 1;
	int* nProjEdge1 = new int[sizeof(int) * 10000];//={0,};//= new int[sizeof(int)*nw];	//0729
	memset(nProjEdge1, 0, sizeof(int) * 10000);// //0729
	if (rttmp.left > dCoatTopEdge - 400)//220202 //오버레이를 찾기 위해 리사이징
		rttmp.left = dCoatTopEdge - 400;
	if(rttmp.left<m_nMarginSearchStart[nCam])rttmp.left=m_nMarginSearchStart[nCam];//211017-1
	if (nt1 < 0)nt1 = 0;
	if (nt2 >= nh)nt2 = nh - 1;

	int nEndPos = 0, nEndPos1 = 0, nEndPoscnt = 0;//0707
	nEdgeTh1 = 20;//211003 //211007
	nMargin = 20;//211003
	if (rttabins.top < 0)rttabins.top = 0;
	if (rttabins.bottom >= nh)rttabins.bottom = nh - 1;
	int nWhiteCut1 = 240;
	if(m_bWhiteRoll[nCam]==TRUE){
		nWhiteCut1 = nNoncoatEdge1 + 10;//211101
		nEdgeTh1 = 20;//211101
	}
	if (m_dTabRightDetectValue[nCam] != 0)
		nWhiteCut1 = m_dTabRightDetectValue[nCam];
	//탭  중심부에 평균이므로 중심부에 노이즈 발생시 위치가 틀어질수 있음.
	int ntabright[1000] ={0,};//220314-1
	for (int y = rttabins.top; y<rttabins.bottom && y >= 0; y++)//탭 중앙에 우측 끝부분을 찾는다.
	{
		for (int x = rttabins.right; x>rttabins.left && x>0 && x<m_nWidth - 20; x--) {
			if ((*(fmDark + nw*y + x - 4) - *(fmDark + nw*y + x)>nEdgeTh1  && ntabave11 < *(fmDark + nw*y + x - 8))|| (*(fmTmp + nw*y + x - 4) >nWhiteCut1 && *(fmTmp + nw*y + x) > nWhiteCut1)) {// && *(fm+nw*y+x-4) > nTabave1-nMargin ){//220304
				if(nEndPoscnt<1000)ntabright[nEndPoscnt] = (x - 4);//220314-1
				nEndPos += (x - 4); nEndPoscnt++; break;//0707
			}
		}
	}
	m_nTabAvergeValue[nCam] = ntabave11;
	if (nEndPoscnt <= 0)nEndPoscnt = 1;//0707 //같으면 -> 작거나 같으면 수정
	nEndPos /= nEndPoscnt;//rttabins.Height();//0707 //tab 평균 우측
	//평균 보다 큰거에 대해 한번더 평균을 구한다.
	//sort를 하여 가장 우측 10개 이상 모인데를 찾는다.
	int nEndPos2 =0,nEndPos3=0;
	sort(ntabright, ntabright + nEndPoscnt);

	for(int i=nEndPoscnt-1;i>0;i--){
		if(ntabright[i] >nEndPos && nEndPos3<30){//220513 10 -> 30
			nEndPos2 += ntabright[i];nEndPos3++;
		}

	}
	//for(int i=0;i<nEndPoscnt;i++){
	//	if(ntabright[i] >nEndPos){
	//		nEndPos2 += ntabright[i];nEndPos3++;
	//	}
	//}
	if(nEndPos3>20){	// sjj - 0413 0
		nEndPos2 /= nEndPos3;
		nEndPos = nEndPos2;
	}
	if(nEndPos-dCoatTopEdge < 170) {//220503
		nEndPos = dCoatTopEdge+170;//220503
		rttmp.right = nEndPos - 100;
	}
	else
		rttmp.right = nEndPos-150;
	for (int y = nt1; y<nt2 && y >= 0; y++)
	{
		nProjmin[y] = 255;
		for (int x = rttmp.left; x<rttmp.right && x>0 && x<m_nWidth; x++)
		{
			if (x - rttmp.left >= 10000 || x - rttmp.left < 0)continue;//0607-2
			if (nProj[y] == 0)//211003
				nProj[y] = dave1;//211003
			if(nProj[y] > dave1) nProj[y] = dave1;
			nTmp = nValue + (*(fm + nw*y + x) - nProj[y])*nRatio;//*(nImageProjection+i);
			if (nTmp<0)	nTmp = 0;
			else if (nTmp>255) nTmp = 255;
			if (nProjmin[y]>nTmp)nProjmin[y] = nTmp;
			if (nProjmax[y]<nTmp)nProjmax[y] = nTmp;
			nProj1[x - rttmp.left] += nTmp;//전처리
			nProj2[x - rttmp.left] += *(fm + nw*y + x);//0921 //bright
			if (noverslitnh >= 0 && int(noversplitcnt / noverslitnh) < 10 && noversplitcnt / noverslitnh > 0 && x - rttmp.left <2000)//220202
				nProj11[int(noversplitcnt / noverslitnh)][x - rttmp.left] += nTmp;//0604
			*(fmPreProcess + nw*y + x) = nTmp;//y 전처리 이미지 
			nProjEdge1[x - rttmp.left] += *(fmDark + nw * y + x);//0729 			//0729 dark image에서 전극 에지를 찾는다
		}
		nminave += nProjmin[y];
		nmaxave += nProjmax[y];
		noversplitcnt++;//0604
		if (noversplitcnt / noverslitnh>10)break;//0604
	}
	m_dInsSubTime1[nCam][6] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	if ((GetPrecisionTime() - dStartTime) * 1000.0f > fTimeLimit)
	{
		m_nPreprocessEdgeBreakStep[nCam] = 1;
		m_dLastTime[nCam] = m_dInsSubTime1[nCam][6];
		return;
	}

	if(nt2 - nt1 <=0){
		nt2 = nt1+10;
	}
	nminave /= (nt2 - nt1);// 220110
	nmaxave /= (nt2 - nt1);// 220110
	BOOL bTabEdgeSkip = m_bTabPoly[nCam];// 220110
////////Tab의 우상 우하의 위치를 찾는다.//211009
	int nRightTop1 = nEndPos, nRightTop2 = nEndPos + 20;
	if (nt1 + 22 > 0 && nt1 + 22 < m_nHeight - 5 && nRightTop2 > 0 && nRightTop2 < m_nWidth - 70 && nCoatEdge1 + 100>0 && nCoatEdge1 + 100 < m_nWidth - 100) {//211017-1			//SJM 여기는 사이즈 확인
		for (int y = nt1 + 22; y > nt1 + 20; y--) {																																	//SJM 탭 상부 기준으로 20 픽셀만 본다.
			for (int x = nRightTop2; x > nCoatEdge1 + 180; x--) {//220110																											//SJM  탭오른쪽 +20 부터 전극 엣지에서 오른쪽으로 +180 까지 스캔
				if (*(fmDark + nw*y + x - 4) - *(fmDark + nw*y + x) > nEdgeTh1 || *(fmTmp + nw*y + x - 4) >= nNoncoatEdge1) {// && *(fm+nw*y+x-4) > nTabave1-nMargin ){//211012		//SJM 다음 4픽셀과의 차이가 nEdgeTH1 보다 크고 다음 4픽셀이 미코팅이라 판단될떄
					if (nRightTop1 +4 > x) {//220323
						nRightTop1 = x; break;
					}
				}
			}
		}
	}
	int nRightBtm1 = nEndPos, nRightBtm2 = nEndPos + 20;
	if (nt2 - 22 > 0 && nt2 - 20 < m_nHeight - 5 && nRightBtm2 > 0 && nRightTop2 < m_nWidth - 70 && nCoatEdge1 + 100>0 && nCoatEdge1 + 100 < m_nWidth - 100) {//211017-1
		for (int y = nt2 - 22; y < nt2 - 20; y++) {
			for (int x = nRightBtm2; x > nCoatEdge1 + 180; x--) {//220110
				if (*(fmDark + nw*y + x - 4) - *(fmDark + nw*y + x) > nEdgeTh1 || *(fmTmp + nw*y + x - 4) >= nNoncoatEdge1) {// && *(fm+nw*y+x-4) > nTabave1-nMargin ){//211012
					if (nRightBtm1 + 4> x) {//220323
						nRightBtm1 = x; break;
					}
				}
			}
		}
	}
	//더작은 값 //
	if(bTabEdgeSkip)
	{
		nEndPos1 = nRightTop1;//에지에서 300을 리미트로 한다.
		if (nEndPos1>nRightBtm1)nEndPos1 = nRightBtm1;
		if (nEndPos - nEndPos1 > 200)
			nEndPos -= 200;
		else
			nEndPos = nEndPos1;
	}

	if (nt2 - nt1 <= 100) {//0707 //0805
		double dcx12 = 0, dcy12 = 0;
		dcx12 = rttmp.right; dcy12 = (nt2 - nt1)*0.5 + nt1;
		m_rtIsland[nCam][m_nIslandCount[nCam]].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
		m_dISSizeX[nCam][m_nIslandCount[nCam]] = 2;
		m_dISSizeY[nCam][m_nIslandCount[nCam]] = 2;
		m_dISSize[nCam][nDefect] = 0;
		m_nIslandCount[nCam]++;//0706
		delete[] nProjEdge1;//0729
		delete [] fmPreProcess;
		return;
	}
	int nTabCoatBrightness = 0,ncc12=0;
	for (int x = rttmp.left; x<rttmp.right && x>0 && x<m_nWidth; x++) {//0607-2
		nProj1[x - rttmp.left] /= (nt2 - nt1);//pre
		nProj2[x - rttmp.left] /= (nt2 - nt1);//bright
		nProjEdge1[x - rttmp.left] /= (nt2 - nt1);//dark
		if(x < dCoatTopEdge){
			nTabCoatBrightness += nProj2[x - rttmp.left];ncc12++;
		}
	}
	if(ncc12>0)
		nTabCoatBrightness /= ncc12;
	double dcoatsum = 0, dcosedge1[10] = { 0, };//0604
	int ncoatsumcnt1 = 0;//0604
	nMargin = 30;//0729  //전극 에지를 먼저 찾는다.  //전처리 프로젝션 데이터
	//bright 에서 찾기
	BOOL boldfind = FALSE;
	if(boldfind){
		for (int x = rttmp.right - 80; x>rttmp.left + 10 && x>0; x--) {//전처리 이미지//220223 탭높이가 작을경우 에지를 넘어가는 문제 발생. 100->80으로 수정
			if (nProj1[x - (rttmp.left )] - nProj1[x - (rttmp.left )-8]>nEdgeTh1){// && nProj1[x - (rttmp.left ) + 8]>nmaxave - nMargin) {//0623
				nCoatEdge1 = x-8; break;//코팅 시작 //+8 //0623
			}
		}
	}
	else
	{
		int nPointX = rttmp.right - 80;
		if (0 > nPointX - (rttmp.left) || 0 > nPointX - (rttmp.left) - 8 || 0 > nPointX - (rttmp.left) - 12/*배열이 0으로 갈경우 예외처리*/||10000<= nPointX - (rttmp.left)||10000<= nPointX - (rttmp.left) - 8||10000<= nPointX - (rttmp.left) - 12)
			return;
		for (int x = rttmp.right - 80; x>rttmp.left + 10 && x>0; x--) {//bright 이미지//220223
			if (nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) - 8] > nEdgeTh1 && nProj2[x - (rttmp.left) - 12] < nTabCoatBrightness + 30/*해당 밝기이상 차이나면 Edge로 인식*/) {// && nProj1[x - (rttmp.left ) + 8]>nmaxave - nMargin) {//0623
				nCoatEdge1 = x - 8; break;//코팅 시작 //+8 //0623
			}
		}
	}
	dcoatsum = nCoatEdge1; dcosedge1[ncoatsumcnt1] = nCoatEdge1;  ncoatsumcnt1++;
	if(noverslitnh<1)noverslitnh = 1;
	CRect rtImage(10, 10, m_nWidth - 10, m_nHeight - 10);
	rttmp = rttmp & rtImage;
	if ((2000 - 10) <= rttmp.Width())
	{
		return;
	}
	for (int i = 0; i<10; i++) {//0604
		if (ncoatsumcnt1<10) {
			for (int x = rttmp.left+10; x<rttmp.right && x>0 && x<m_nWidth; x++) {
				nProj11[i][x - (rttmp.left+10)] /= noverslitnh;//rttmp.Height(); //여기도 버그 x - rttmp.left+10 -> x - (rttmp.left+10)로 수정함
			}
			for (int x = rttmp.left + 10; x<rttmp.right - 8 && x>0; x++) {//0604
				if (nProj11[i][x - (rttmp.left+10) + 8] - nProj11[i][x - (rttmp.left+10)]>nEdgeTh1 && nProj11[i][x - (rttmp.left+10) + 8]>nmaxave - nMargin) {
					dcoatsum += x + 10; dcosedge1[ncoatsumcnt1] = x + 10; ncoatsumcnt1++; break;//코팅 시작
				}
			}
		}
	}
	nEdgeTh1 = m_nCoatEdgeTh[nCam];
	int nTabEdgecut = m_dMiddleAve[nCam] +20;//211207
	if(m_bWhiteRoll[nCam])//211207
		nTabEdgecut = m_dMiddleAveDark[nCam]+40;


	if(m_nAnodeCathode==0 && m_bOverlayInsp && m_nInsulateType !=1 && m_bInsulTapeInsp[nCam]==TRUE){//220202 어깨선이 -로 된경우 , 탭왼쪽 전극 밝기가 전극 중앙 밝기보다 30큰경우
		//세라믹일때는 시킵한다  220210
		if(nCoatEdge1 < dCoatTopEdge-5 || nTabCoatBrightness > m_dMiddleAve[nCam] + 70){//220211
			m_bOverlayError[nCam] = TRUE;
		}
	}
	//////220321
	//if (m_bInsScrab[nCam] && m_nIslandCount[nCam] < MAX_HUGEDEFECT - 2 ){//220315 //tape 
	//	if(m_bFindWhiteTape[nCam] && m_bTapeInScrab[nCam] ==FALSE)//테이프가 검출되었고 테이프를 스크랩으로 검출 안해야 하는경우 Skip
	//		;
	//	else
	//		nDefect = FindScrab(nCam, fm, fmDark, m_nWidth, m_nHeight, dCoatTopEdge);// nCoatEdge1);//0930//211002
	//}
//nCoatEdge1 어깨선 에지 //dCoatTopEdge 컷팅 에지 // 현재 컷팅에지가 정확하지 않아 어깨선 에지로 사용
	if (nEndPos - nCoatEdge1 < 100)nEndPos = nCoatEdge1 + 250;//211003
	delete[]nProjEdge1;//0729
	if (ncoatsumcnt1 == 0)ncoatsumcnt1 = 1;
	dcoatsum /= ncoatsumcnt1;
	dcoatsum = nCoatEdge1;//0729

	/////////////////여기부터 오버레이 측정/////////////////

	int nOverlayPeak1 = 0, nOverlayPeak2 = 0, noverlaysplitcnt = 0;//0603
	double doverlaysum1 = 0, doverlaysplit[10] = { 0, };
	m_dInsulWidth[nCam] = 0;//0604
	m_dOverlayWidth[nCam] = 0;//0604

	//이 위까지 시간이 오래 걸리면 절연, 오버레이 0으로 나옴

	if (TRUE == m_bInspManual)
	{
		memset(m_nOverlayROI_Avg[nCam], 0, sizeof(int) * 10000);
	}

	m_bInspectOverlayAtPreprocess[nCam] = FALSE;

	if (m_bOverlayInsp) 
	{//0604
		nMargin = 20;
		int nOverlayold =0;
		BOOL bFindOverlay1 = FALSE;
		m_overlayROI[nCam] = CRect(rttmp.left + 30, nt1, nCoatEdge1 - 10, nt2);

		if (TRUE == m_bInspManual)
		{
			memcpy(m_nOverlayROI_Avg[nCam], nProj2/*rttmp 영역 내 X좌표별 Y의 밝기 평균*/, sizeof(int) * 10000);
		}

		//		if(m_bCemramic[nCam])
		{//오리진 이미지에서 찾음 
			int nEdgeTh4 = 10;
			if(m_nInsulateType==1)
			{//ceeramic sbr //220210 pvdf와 ceramic 구분
				nEdgeTh4 = 6;
				if(m_nOverlayTh[nCam] > 0) nEdgeTh4 = m_nOverlayTh[nCam];//220211
				for (int x = rttmp.left + 30; x < nCoatEdge1 - 10 && x > 0 && x < m_nWidth; x++)
				{//220112
					if (nOverlayPeak1 < nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8])//ROI 영역 내 밝기 평균 값으로 연산한다.
					{
						nOverlayPeak1 = nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8]; nOverlayPeak2 = x + 8;
					}
					if (abs(nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8]) > nEdgeTh4)//밝기 차이 값으로 본다.
					{//세라믹의 경우 흑백이 혼재되어 있음.
						nOverlay = x + 8; 
						bFindOverlay1 = TRUE;
						break;//오버레이
					}
				}
			}
			else
			{
				if(m_nOverlayTh[nCam] > 0) 
					nEdgeTh4 = m_nOverlayTh[nCam];//220211

				for (int x = rttmp.left + 30; x < nCoatEdge1 - 10 && x>0 && x < m_nWidth; x++)
				{//220112
					if (nOverlayPeak1 < nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8])
					{
						nOverlayPeak1 = nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8]; nOverlayPeak2 = x + 8;
					}
					if (nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8] > nEdgeTh4 && nProj2[x - (rttmp.left) + 8] < nminave + nMargin)//밝기 차이 값으로 본다.
					{
						nOverlay = x + 8; 
						bFindOverlay1 = TRUE;
						break;//오버레이
					}
				}
			}

			if (nOverlay == 0)	
				nOverlay = nOverlayPeak2;

			doverlaysum1 = doverlaysum1=nOverlay;
			double dOverlaySpec = m_dOverlaySize[nCam] + m_dOverlayPlusTolerance[nCam];// 1.1;//211119

			if( (dcoatsum - doverlaysum1)*m_dscale_x1[nCam] > 0.1 && (dcoatsum - doverlaysum1)*m_dscale_x1[nCam] < dOverlaySpec ) 
				bFindOverlay1 = TRUE;
			else
				bFindOverlay1 = FALSE;
		}
		if(bFindOverlay1==FALSE) //원본 이미지에서 찾아보고 잘 안 찾아지면 전처리 이미지에서 한번 더 찾는다.
		{//전처리에서 찾음
			m_bInspectOverlayAtPreprocess[nCam] = TRUE;

			if (nmaxave - nminave > 100)
				nMargin = (nmaxave - nminave) * 0.2;//0925-1

			for (int x = rttmp.left + 30; x < nCoatEdge1 && x>0 && x < m_nWidth; x++)
			{
				if (nOverlayPeak1 < nProj1[x - (rttmp.left)] - nProj1[x - (rttmp.left) + 8])
				{
					nOverlayPeak1 = nProj1[x - (rttmp.left)] - nProj1[x - (rttmp.left) + 8]; nOverlayPeak2 = x + 8;
				}
				if (nProj1[x - (rttmp.left)] - nProj1[x - (rttmp.left) + 8]>nEdgeTh2 && nProj1[x - (rttmp.left) + 8]<nminave + nMargin)
				{
					nOverlay = x + 8; break;//오버레이
				}
			}

			if (nOverlay == 0)	
				nOverlay = nOverlayPeak2;

			m_overlayROI[nCam] = CRect(rttmp.left + 30, nt1, nCoatEdge1, nt2);

			nOverlayold = nOverlay;
			doverlaysum1 = nOverlay; noverlaysplitcnt++;
			for (int i = 0; i < 10; i++)
			{
				for (int x = rttmp.left + 30; x < nCoatEdge1 && x>0 && x < m_nWidth; x++)
				{
					if(noverlaysplitcnt>=10)break;

					if (nOverlayPeak1 < nProj11[i][x - (rttmp.left)] - nProj11[i][x - (rttmp.left) + 8]) 
					{
						nOverlayPeak1 = nProj11[i][x - (rttmp.left)] - nProj11[i][x - (rttmp.left) + 8]; nOverlayPeak2 = x + 8;
					}
					if (nProj11[i][x - (rttmp.left)] - nProj11[i][x - (rttmp.left) + 8]>nEdgeTh2 && nProj11[i][x - (rttmp.left) + 8]<nminave + nMargin) 
					{
						nOverlay = x + 8; doverlaysum1 += x + 8; doverlaysplit[i] = x + 8; noverlaysplitcnt++; break;//오버레이
					}
				}
				if (nOverlay == 0) 
				{
					doverlaysum1 += nOverlayPeak2;
					doverlaysplit[i] = nOverlayPeak2;
					noverlaysplitcnt++;
				}
			}

			if (noverlaysplitcnt == 0)
				noverlaysplitcnt = 1;

			if (TRUE == m_bInspManual)
			{
				memcpy(m_nOverlayROI_SplitAvg, nProj11, sizeof(int) * 10 * 2000);
			}

			doverlaysum1 /= noverlaysplitcnt;
			sort(doverlaysplit, doverlaysplit + 10);
			double doverlaysum11 = 0;

			for (int i = 4; i < 8; i++)//220111
				doverlaysum11 += doverlaysplit[i];

			doverlaysum11 /= 4;

			if(doverlaysum11>0)
				nOverlay = doverlaysum1 = doverlaysum11;
			else
				doverlaysum1 = doverlaysum11=nOverlay;
		}

		m_dRealOverlayWidth[nCam] = (dcoatsum - doverlaysum1) * m_dscale_x1[nCam];
		m_dOverlayWidth[nCam] = (dcoatsum - doverlaysum1)*m_dscale_x1[nCam];

		if (m_dOverlayWidth[nCam] <= 0.1) 
		{//0909
			m_dOverlayWidth[nCam] = (dcoatsum - nOverlayold)*m_dscale_x1[nCam];
			m_dRealOverlayWidth[nCam] = (dcoatsum - nOverlayold) * m_dscale_x1[nCam];
			m_rtOverlay[nCam].SetRect(nOverlayold, nt1, nCoatEdge1, nt2);						//0729-1
			m_rtOverlay[nCam] = SetBoundary2(m_rtOverlay[nCam], CRect(80, 0, nw - 100, nh));	//0729-1
		}
		else 
		{
			m_rtOverlay[nCam].SetRect(nOverlay, nt1, nCoatEdge1, nt2);						//0729-1
			m_rtOverlay[nCam] = SetBoundary2(m_rtOverlay[nCam], CRect(80, 0, nw - 100, nh));	//0729-1
		}
		if(m_bOverlayError[nCam] && m_bInsulTapeInsp[nCam])
		{//220210
			m_dOverlayWidth[nCam] = -0.1;//0729-1
			m_rtOverlay[nCam].SetRect(0, 0, 0, 0);
		}
	}
	else 
	{
		m_dOverlayWidth[nCam] = 0;//0729-1
		m_rtOverlay[nCam].SetRect(0, 0, 0, 0);
	}

	if (m_bUseIntekTabAlign[nCam] == TRUE)
	{
		int nTabRightPos = nEndPos;

		if (m_bTabRightPosCorrection[nCam] == TRUE)
		{
			int nTabHeightPixel = m_dTabHeight[nCam] / m_dscale_x1[nCam];

			if (nTabRightPos - nCoatEdge1 < nTabHeightPixel * 0.7)
			{
				if (m_dTabHeight[nCam] > 0 && m_dscale_x1[nCam] > 0)
					nTabRightPos = nCoatEdge1 + nTabHeightPixel;
			}
		}

		m_rtCellTab[nCam].SetRect(nCoatEdge1, nt1, nTabRightPos, nt2);
	}
	else
	{
		m_rtCellTab[nCam].SetRect(nCoatEdge1, nt1 - 5, nEndPos, nt2 + 5);
	}

	m_rtCellTab[nCam] = SetBoundary2(m_rtCellTab[nCam], CRect(80, 0, nw - 100, nh));
	int nOffset1 = 0;//10;//코팅 드레그가 심한경우 Skip //220110
	if (m_nMachine == 0)nOffset1 = 80;//Bug
	rttabins1.SetRect(nCoatEdge1 + nOffset1, nt1, nEndPos, nt2);//0611 //220218 영역 버그 수정 필요.
	rttabins1 = SetBoundary2(rttabins1, CRect(80, 0, nw - 100, nh));
	CRect rtspur;//0928
//	rtspur.SetRect(nCoatEdge1 - 140, rttabins1.top - 40, nCoatEdge1 - m_nSpurOffset[nCam], rttabins1.bottom +40);//211023 
	rtspur.SetRect(nCoatEdge1 - 256, rttabins1.top , nCoatEdge1 - m_nSpurOffset[nCam], rttabins1.bottom);//211023 
	rtspur = SetBoundary2(rtspur, CRect(80, 0, nw - 100, nh));//211017-1

	m_rtMinTab[nCam].top = nt1;	m_rtMinTab[nCam].bottom = nt2; m_rtMinTab[nCam].right = nEndPos;
	m_rtMinTab[nCam] = SetBoundary2(m_rtMinTab[nCam], CRect(80, 0, nw - 100, nh));
	m_rtOverlay[nCam].SetRect(nOverlay, nt1, nCoatEdge1, nt2);
	m_rtOverlay[nCam] = SetBoundary2(m_rtOverlay[nCam], CRect(80, 0, nw - 100, nh));
	m_dInsSubTime1[nCam][7] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	if (m_bBottomOverayInsp[nCam]){//0801 longcell //220111
		rttmpbottom.left = m_rtCell[nCam].left - 100;//0801 longcell
		rttmpbottom.right = m_rtCell[nCam].left + 100;//0801 longcell
		rttmpbottom = SetBoundary2(rttmpbottom, CRect(120, 0, nw - 100, nh));//0812
		InspBottomOverlay( nCam, fmDark,nw,nh, rttmpbottom);
	}

	m_nRealCalcOverlayPos[nCam] = nOverlay + 1;

	if ((GetPrecisionTime() - dStartTime) * 1000.0f > fTimeLimit)
	{
		m_nPreprocessEdgeBreakStep[nCam] = 2;
		m_dLastTime[nCam] = m_dInsSubTime1[nCam][7];
		return;
	}
	/////////////////여기까지 오버레이 측정/////////////////
	
	int nInsultBright = 0;
	CRect RectImage(10, 10, nw - 10, nh - 10); //이미지 사이즈 처리
	if (m_bInsulateInsp == TRUE && m_bPrimer == FALSE) 
	{	//절연 폭//	fmDark //
		int nRightSum = 0, nRightSum1 = 0, nTabSum = 0, nRightSumcnt = 0;

		InspInsulateWidth(nCam, fm, fmDark, nw, nh, rttabins, nCoatEdge1, nEndPos, nOverlay, nt1, nt2, &nRightSum, &nRightSum1);
		if (m_bIstest)
		{
			memset(m_BinaryInsulateImage[0], 0, sizeof(BYTE)* m_nWidth* m_nHeight);
			memset(m_BinaryInsulateImage[1], 0, sizeof(BYTE) * m_nWidth * m_nHeight);
		}
		m_dInsSubTime1[nCam][15] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;
		//절연 평균 밝기를 구한다.
		int nInsulDarkLeft = m_dInsulDarkLeft[nCam] / m_dscale_x1[nCam];//220401
		int nInsulDarkRight = m_dInsulDarkRight[nCam] / m_dscale_x1[nCam];//220328
		BOOL InspGap = TRUE;
		CRect rtCInsul = m_rtInsulate[nCam];
		rtCInsul = SetBoundary2(m_rtInsulate[nCam], CRect(80, 0, nw - 100, nh));
		IPVM::Rect rtRect = IPVM::FromMFC(rtCInsul);
		IPVM::Rect nrtOffset = IPVM::FromMFC(m_rtInsulationInspAreaOffset[nCam]);
		m_rtInsulationInspArea[nCam] = m_rtInsulationDarkInspArea[nCam] = IPVM::ToMFC(rtRect);  //일단 같이써놓고, 나중에 따로 옵셋
		m_rtInsulationDarkInspArea[nCam]= IPVM::ToMFC(rtRect); 
		m_rtInsulationDarkInspArea[nCam].left = m_rtInsulationDarkInspArea[nCam].left+ nInsulDarkLeft;
		m_rtInsulationDarkInspArea[nCam].right = m_rtInsulationDarkInspArea[nCam].left+ nInsulDarkRight;
		m_rtInsulationInspArea[nCam].DeflateRect(m_rtInsulationInspAreaOffset[nCam].left, m_rtInsulationInspAreaOffset[nCam].top, m_rtInsulationInspAreaOffset[nCam].right, m_rtInsulationInspAreaOffset[nCam].bottom);
		InspInsulate(nCam, fm, fmDark, nw, nh, nRightSum, nRightSum1, rtRect, nrtOffset, nInsulDarkLeft, nInsulDarkRight, InspGap); //여기에서 시간이 너무 오래 걸림
		m_dInsSubTime1[nCam][17] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;  //17~19번은 절연 검사시간 체크용
		if (TRUE==m_bCuttingHeightTopAreaInspUse[nCam]) 
		{
			m_rtCuttingHeightTop[nCam].SetRect(nCoatEdge1, 0, nCellRightCellAreaPoint, nh);
			//m_rtCuttingHeightTop[nCam] = SetBoundary2(m_rtCuttingHeightTop[nCam], CRect(0, 10, nw, nh-10));
			IPVM::Rect rtRect = IPVM::FromMFC(m_rtCuttingHeightTop[nCam]);
			IPVM::Rect nrtOffset(m_rtCuttingHeightTopAreaOffset[nCam].m_left, m_rtCuttingHeightTopAreaOffset[nCam].m_top, m_rtCuttingHeightTopAreaOffset[nCam].m_right, m_rtCuttingHeightTopAreaOffset[nCam].m_bottom);
			m_rtCuttingHeightTop[nCam].DeflateRect(m_rtCuttingHeightTopAreaOffset[nCam].m_left, m_rtCuttingHeightTopAreaOffset[nCam].m_top, m_rtCuttingHeightTopAreaOffset[nCam].m_right, m_rtCuttingHeightTopAreaOffset[nCam].m_bottom);
			m_rtCuttingHeightTop[nCam] &=RectImage;
			InspInsulate(nCam, fm, fmDark, nw, nh, nRightSum, nRightSum1, rtRect, nrtOffset); //여기에서 시간이 너무 오래 걸림
			m_dInsSubTime1[nCam][18] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;
		}
		if (TRUE == m_bCuttingHeightBottomAreaInspUse[nCam]) //Bottom overlay 검사가 켜져있어야 함
		{
			m_rtCuttingHeightBottom[nCam].SetRect(m_dBottomInsul[nCam][1], 0, m_dBottomCoat[nCam][1], nh);   //roi 영역 다시 잡아야 함.
			IPVM::Rect rtRect = IPVM::FromMFC(m_rtCuttingHeightBottom[nCam]);
			IPVM::Rect nrtOffset(m_rtCuttingHeightBottomAreaOffset[nCam].m_left, m_rtCuttingHeightBottomAreaOffset[nCam].m_top, m_rtCuttingHeightBottomAreaOffset[nCam].m_right, m_rtCuttingHeightBottomAreaOffset[nCam].m_bottom);
			m_rtCuttingHeightBottom[nCam].DeflateRect(m_rtCuttingHeightBottomAreaOffset[nCam].m_left, m_rtCuttingHeightBottomAreaOffset[nCam].m_top, m_rtCuttingHeightBottomAreaOffset[nCam].m_right, m_rtCuttingHeightBottomAreaOffset[nCam].m_bottom);
			m_rtCuttingHeightBottom[nCam] &= RectImage;
			InspInsulate(nCam, fm, fmDark, nw, nh, nRightSum, nRightSum1, rtRect, nrtOffset); //여기에서 시간이 너무 오래 걸림
			m_dInsSubTime1[nCam][19] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;
		}

	
		m_dInsSubTime1[nCam][16] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;
		nInsultBright = nRightSum1;
		
		if(m_bOverlayError[nCam] && m_bInsulTapeInsp[nCam])
		{//220210
			m_dOverlayWidth[nCam] = -0.1;//0729-1
			m_rtOverlay[nCam].SetRect(0, 0, 0, 0);
			m_dInsulWidth[nCam] =0;
		}
		//오버레이폭에맞게 영역 재조정
		int nOverlywidth1 = m_dOverlayWidth[nCam] / m_dscale_x1[nCam];
		m_rtOverlay[nCam].left = m_rtOverlay[nCam].right - nOverlywidth1;
		m_nCorrectionOverlayPos[nCam] = m_rtOverlay[nCam].left;

	}
	else if (m_bPrimer) 
	{//0630
		m_dInsulWidth[nCam] = 0;
		m_dOverlayWidth[nCam] = 0;
		m_rtInsulate[nCam].SetRect(0, 0, 0, 0);
		m_rtOverlay[nCam].SetRect(0, 0, 0, 0);//Overlay Rect
	}

	m_dInsSubTime1[nCam][8] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000; //9ms

	if ((GetPrecisionTime() - dStartTime) * 1000.0f > fTimeLimit)
	{
		m_nPreprocessEdgeBreakStep[nCam] = 3;
		m_dLastTime[nCam] = m_dInsSubTime1[nCam][8];
		return;
	}

//220411
	//if(m_bWhiteRoll[nCam])
	nDefect = FindNonCoating(nCam, fm, fmDark, m_nWidth, m_nHeight, nIslandTh, rtEdge[0], rtEdge[1], nNoncoatEdge1, dCoatEdge1, nt1-16, nt2+16, ntabpos1);//211003 /////////시간 오래 걸리는 함수/////////
	//else
		//nDefect = FindNonCoating(nCam, fm, fmDark, m_nWidth, m_nHeight, nIslandTh, rtEdge[0], rtEdge[1], (nSumEdge1 + nSumEdge2)*0.5, dCoatEdge1, nt1-16, nt2+16, ntabpos1);//211003 /////////시간 오래 걸리는 함수/////////

	m_dInsSubTime1[nCam][10] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	if ((GetPrecisionTime() - dStartTime) * 1000.0f > fTimeLimit)
	{
		m_nPreprocessEdgeBreakStep[nCam] = 4;
		m_dLastTime[nCam] = m_dInsSubTime1[nCam][10];
		return;
	}

	////220330 오버레이폭 구하고 스크랩 검사하도록 수정. //220411 속도문제로 스크랩 스레드로 변경
	//if (m_bInsScrab[nCam] && m_nIslandCount[nCam] < MAX_HUGEDEFECT - 2 ){//220315 //tape 
	//	if(m_bFindWhiteTape[nCam] && m_bTapeInScrab[nCam] ==FALSE)//테이프가 검출되었고 테이프를 스크랩으로 검출 안해야 하는경우 Skip
	//		;
	//	else
	//		nDefect = FindScrab(nCam, fm, fmDark, m_nWidth, m_nHeight, dCoatTopEdge);// nCoatEdge1);//0930//211002
	//}

	int nTabProj[2000] = { 0, };// = new int[sizeof(int)*rttabins1.Width()];memset(nTabProj,0,sizeof(int)*rttabins1.Width());
	ncnt = 0;
	double dtabAverage = 0, dTabcnt = 0;
	if (rttabins1.top < 0)rttabins1.top = 0;
	if (rttabins1.bottom >= nh)rttabins1.bottom = nh - 1;
	for (int y = rttabins1.top; y<rttabins1.bottom&&y >= 0 && y<m_nHeight; y++) {
		for (int x = rttabins1.left; x<rttabins.right && x>0 && x<m_nWidth && x - rttabins1.left<2000; x++) {
			nTabProj[x - rttabins1.left] += *(fm + nw*y + x);
		}
		ncnt++;
	}
	if (ncnt < 1)ncnt = 1;//211017-1
	for (int x = rttabins1.left; x<rttabins1.right && x - rttabins1.left<2000; x++) {
		nTabProj[x - rttabins1.left] /= ncnt;
		dtabAverage += nTabProj[x - rttabins1.left]; dTabcnt++;
	}
	if (dTabcnt>0)dtabAverage /= dTabcnt;
	m_dTabBrightness[nCam][0] = dtabAverage;//0720 brightness tab ave

	if(m_bInsulateInsp == TRUE && m_bPrimer == FALSE && 
		m_bInsulTapeInsp[nCam] && m_nInsulateType !=1 /*SBR*/ && 
		nInsultBright < dtabAverage - m_nIslandTh[nCam])
	{
		//220210
		m_bOverlayError[nCam] = TRUE;
		m_dOverlayWidth[nCam] = -0.2;//0729-1
		m_rtOverlay[nCam].SetRect(0, 0, 0, 0);
		m_dInsulWidth[nCam] =0;
	}

		
	//int nXSkip1 = m_dTabSkipRange[nCam][2] / m_dscale_x1[nCam];//right //0614
	//int nYSkip1 = m_dTabSkipRange[nCam][0] / m_dscale_y1[nCam];//top //0614
	//int nYSkip2 = m_dTabSkipRange[nCam][1] / m_dscale_y1[nCam];//bottom //0614
	//if (nYSkip1 < 5)nYSkip1 = 5; if (nYSkip2 < 5)nYSkip2 = 5;

	if (m_nMachine == 0)nOffset1 = 80;//Bug
	if(m_bInsulateInsp){//220220 탭영역 재조정
		if(rttabins1.left < m_rtInsulate[nCam].right)
			rttabins1.left = m_rtInsulate[nCam].right;
		rttabins1 = SetBoundary2(rttabins1, CRect(80, 0, nw - 100, nh));
	}
	if(rttabins1.Width()<0 || rttabins1.Height() <0){//220225  불량 처리후 리턴한다.
		if(m_nIslandCount[nCam] < MAX_HUGEDEFECT-1){
			double dcompact = 50;
			double dangle11 =0;
			m_rtIslandBlob[nCam][m_nIslandCount[nCam]] = rttabins1;
			double dPercent = 0;
			m_dISAngle[nCam][m_nIslandCount[nCam]] = dangle11;			//0white 1black	//0803
			m_dISCompactness[nCam][m_nIslandCount[nCam]] = dcompact;			//0white 1black //0803
			m_dISSizeX[nCam][m_nIslandCount[nCam]] = 2;
			m_dISSizeY[nCam][m_nIslandCount[nCam]] = 2;
			m_dISSize[nCam][m_nIslandCount[nCam]] = m_dISSizeY[nCam][nDefect];
			m_nIslandCount[nCam]++;
		}
		delete [] fmPreProcess;

		return;
	}

	m_dInsSubTime1[nCam][11] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	if ((GetPrecisionTime() - dStartTime) * 1000.0f > fTimeLimit)
	{
		m_nPreprocessEdgeBreakStep[nCam] = 5;
		m_dLastTime[nCam] = m_dInsSubTime1[nCam][11];
		return;
	}

	//이진화를 먼저한다.
	if (!m_bPrimer)
	{
		nDefect = FindSpur(nCam, fm, nw, nh, rtspur, m_nSpurValue[nCam]);//211023 /////////시간 오래 걸리는 함수/////////
		m_nIslandCount[nCam] = nDefect;//
	}

	m_dInsSubTime1[nCam][12] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	if ((GetPrecisionTime() - dStartTime) * 1000.0f > fTimeLimit)
	{
		m_nPreprocessEdgeBreakStep[nCam] = 6;
		m_dLastTime[nCam] = m_dInsSubTime1[nCam][12];
		return;
	}

	nDefect = findIsland(nCam, fm, fmDark, nw, nh, rttabins1, nTabProj, nIslandTh);//0921
	m_nIslandCount[nCam] = nDefect;

	m_dInsSubTime1[nCam][13] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	if ((GetPrecisionTime() - dStartTime) * 1000.0f > fTimeLimit)
	{
		m_nPreprocessEdgeBreakStep[nCam] = 7;
		m_dLastTime[nCam] = m_dInsSubTime1[nCam][13];
		return;
	}

	nDefect = InspNgTag(nCam, fm, fmDark, nw, nh, rttabins1);
	m_nIslandCount[nCam] = nDefect;
	
	m_dInsSubTime1[nCam][14] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	if ((GetPrecisionTime() - dStartTime) * 1000.0f > fTimeLimit)
	{
		m_nPreprocessEdgeBreakStep[nCam] = 8;
		m_dLastTime[nCam] = m_dInsSubTime1[nCam][14];
		return;
	}

	//zipper 2 
	if (true == m_bUseZipper2[nCam])
	{
		
		if (m_nIslandCount[nCam] < MAX_HUGEDEFECT - 1)
		{
			double dZipperSpanTime[3] = { 0, };
			dZipperSpanTime[0] = GetPrecisionTime() * 1000;
			// 불량을 추가할 수 있을 때만 검사를 진행한다.

			IPVM::Image_8u_C1 src;
			src.Create(nw, nh);
			memcpy(src.GetMem(), fmDark, nw* nh);

			IPVM::Rect rctab;
			rctab.m_left = m_rtCellTab[nCam].left + m_rtZipper2Offset[nCam].m_left;
			rctab.m_top = m_rtCellTab[nCam].top + m_rtZipper2Offset[nCam].m_top;
			rctab.m_right = m_rtCellTab[nCam].right - m_rtZipper2Offset[nCam].m_right;
			rctab.m_bottom = m_rtCellTab[nCam].bottom - m_rtZipper2Offset[nCam].m_bottom;

			m_zipper.SetParameter(m_nSweepAngle[nCam], 
				m_nSweepCount[nCam], 
				m_nSweepLength[nCam], 
				m_fStartpointThresholdOffset[nCam], 
				m_fTabAreaThresholdOffset[nCam], 
				m_dAverageFilter[nCam], 
				m_dscale_x1[nCam], 
				m_dscale_y1[nCam], 
				m_nZipper2Length[nCam],
				m_nZipper2InsulationWidth[nCam],
				m_rtInsulate[nCam],
				m_bUseROIfoundFromInsulationOverlayInspection[nCam],
				m_fSetZipper2InspectTime[nCam],
				m_bUseZipper2InspectionMiddleArea[nCam],
				m_fInsulationStartpointThresholdOffset[nCam],
				m_fInsulationTabAreaThresholdOffset[nCam],
				m_bUseZipper2InsulationInspect[nCam],
				m_bUseZipper2AutoInsulationWidthPixel[nCam],
				0);

			if (true == m_zipper.Doinspection(src, rctab, m_bIstest, m_bInsulateInsp))
			{
				int nDefect = m_nIslandCount[nCam];

				std::vector<ZipperInfo>& pvecZipperinfo = m_zipper.m_ZipperResult.GetResult();
				for (int i = 0; i < pvecZipperinfo.size(); i++)
				{
					CString text;
					m_rtIsland[nCam][nDefect] = pvecZipperinfo[i].GetDefectROI(20,5);
					m_dISSizeX[nCam][nDefect] = pvecZipperinfo[i].GetDefectROI().Width() * m_dscale_x1[nCam];
					m_dISSizeY[nCam][nDefect] = pvecZipperinfo[i].GetDefectROI().Height() * m_dscale_y1[nCam];
					m_dISSize[nCam][nDefect] = 5002;	//주름

					nDefect++;
					m_nFullDefectCount[nCam]++;

					if (nDefect >= MAX_HUGEDEFECT - 1)
					{
						break;
					}
				}
				m_nIslandCount[nCam] = nDefect;
			}

			dZipperSpanTime[1] = GetPrecisionTime() * 1000;
			dZipperSpanTime[2] = dZipperSpanTime[1] - dZipperSpanTime[0];

			m_dInsSubTime1[nCam][16] = dZipperSpanTime[2];
		}

	}

	if ((GetPrecisionTime() - dStartTime) * 1000.0f > fTimeLimit)
	{
		m_nPreprocessEdgeBreakStep[nCam] = 9;
		m_dLastTime[nCam] = m_dInsSubTime1[nCam][16];
		return;
	}

	if (m_nCellSize == 1 || !m_bBurrInspect) {//0801 long cell의 경우 버 검사를 하지 않는다 
											  //		m_nIslandCount[nCam]=nDefect; //0921
		m_dInsSubTime1[nCam][9] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000; //26ms
		m_bOverlayCheck[nCam] = FALSE;
		delete [] fmPreProcess;
		return;
	}
	//Tab부 Burr check  //0516
	if (m_bBurrInspect) FindBurr(nCam, fm, fmDark, nw, nh, m_rtCellTab[nCam], dtabAverage);//211017-1

	m_bOverlayCheck[nCam] = FALSE;

	if (m_bDefectBinary)
		memcpy(m_fmEven[nCam], m_fmTabEdge[nCam], m_nWidth*m_nHeight);
}

CString CInspect::GetPreprocessEdgeBreakStepCode(int nCode)
{
	CString strCode = _T("");

	switch (nCode)
	{
	case 0:
		strCode = _T("After PreprocessingCeramic");
		break;
	case 1:
		strCode = _T("After TabAlign");
		break;
	case 2:
		strCode = _T("After Overlay");
		break;
	case 3:
		strCode = _T("After Insulate");
		break;
	case 4:
		strCode = _T("After FindNonCoating");
		break;
	case 5:
		strCode = _T("After FindNonCoating Before FindSpur");
		break;
	case 6:
		strCode = _T("After FindSpur");
		break;
	case 7:
		strCode = _T("After FindIsland");
		break;
	case 8:
		strCode = _T("After InspNgTag");
		break;
	case 9:
		strCode = _T("After Zipper 2");
		break;
	}

	return strCode;
}

//#else
void CInspect::PreprocessEdgeNewLogic(int nCam)
{
	m_nInsulateDefectCount[nCam] = 0;
	m_dTabBrightness[nCam][0] = m_dTabBrightness[nCam][1] = 0;//0720
	m_dInsSubTime1[nCam][0] = GetPrecisionTime();

	if (m_ptTabCenter[nCam].x == 0 && m_ptTabCenter[nCam].y == 0)//탭이 없음 //211009
		return;

	m_bOverlayCheck[nCam] = TRUE;
	int nw = m_nWidth;
	int nh = m_nHeight;
	int nCut = 128;
	BYTE* fm = m_fmEven[nCam];//bright
	BYTE* fmDark = m_fmOdd[nCam];//dark
	BYTE* fmTmp = m_fmEven[nCam];//bright
	if (m_bWhiteRoll[nCam] == TRUE)
		fmTmp = m_fmOdd[nCam];//dark
	CRect rttab;
	rttab.SetRect(m_rtMinTab[nCam].left * m_nReduction[nCam], m_rtMinTab[nCam].top * m_nReduction[nCam], m_rtMinTab[nCam].right * m_nReduction[nCam], m_rtMinTab[nCam].bottom * m_nReduction[nCam]);
	rttab = SetBoundary2(rttab, CRect(80, 0, nw - 100, nh));
	CRect rttmp = rttab;//rt;
	CRect rttmpbottom = rttab; //0801 longcell bottom

	m_rtOverlay[nCam].SetRect(0, 0, 0, 0);
	m_nIslandCount[nCam] = 0;
	int ncnt = 0, ncnt1 = 0;
	//	if(m_rtHugeRect[nCam].Width()<100 || m_rtHugeRect[nCam].Height()<20 || rttmp.Width()<50||rttmp.Height()<200)		return;//211003
	int  nProj[3000] = { 0, };// = new int[sizeof(int)*nh];	memset(nProj,0,sizeof(int)*nh);
	int nProjmin[3000] = { 0, };// = new int[sizeof(int)*nh];	memset(nProjmin,0,sizeof(int)*nh);
	int nProjmax[3000] = { 0, };// = new int[sizeof(int)*nh];	memset(nProjmax,0,sizeof(int)*nh);
	int nProjTab[3000] = { 0, };// = new int[sizeof(int)*nh];	memset(nProjTab,0,sizeof(int)*nh);
	int nProjTab1[3000] = { 0, };//0611// = new int[sizeof(int)*nh];	memset(nProjTab,0,sizeof(int)*nh);
	int nProj1[10000] = { 0, };//= new int[sizeof(int)*nw];	memset(nProj1,0,sizeof(int)*nw);//0604
	int nProj2[10000] = { 0, };
	int nProj11[10][2000];//, nInsulateSplit[10][2000];//0604
	memset(nProj11, 0, sizeof(int) * 10 * 2000);//이번에 초기화 추가한건데 기존에 초기화 코드가 없었어서 제대로 동작했을지 모르겠음, 버그였을거 같음
	//for (int i = 0; i<10; i++)for (int j = 0; j<2000; j++) {
	//	nInsulateSplit[i][j] = nProj11[i][j] = 0;//0604
	//}
	BYTE* fmPreProcess = new BYTE[nw * nh];//m_fmOverlayPreprocess[nCam];//new BYTE[nw*nh];//220314
	memset(fmPreProcess, 0, nw * nh);
	m_fmPreprocessTemp = fmPreProcess;
	int nTmp = 0, nValue = m_nTargetBrightness[nCam], nminave = 0, nmaxave = 0, nRatio = 5, nTabave1 = 0;
	int nCoatEdge1 = m_rtCell[nCam].right, nOverlay = 0, nEdgeTh1 = 40, nEdgeTh2 = 25, nt1 = rttmp.top, nt2 = rttmp.bottom, nMargin = 30;//211123
	m_rtInsulate[nCam].SetRect(0, 0, 0, 0);
	m_nFullDefectCount[nCam] = 0;	//notch
	for (int i = 0; i < MAX_HUGEDEFECT; i++)m_dISSize[nCam][i] = 0;//0531
	CRect rtEdge[2];
	int nNonCotrange = 32;//0706
	rtEdge[0].SetRect(m_rtHugeRect[nCam].left - nNonCotrange * 2, 0, m_rtHugeRect[nCam].left + nNonCotrange * 6, nh);//0706
	rtEdge[1].SetRect(m_rtHugeRect[nCam].right - nNonCotrange * 4, 0, m_rtHugeRect[nCam].right + nNonCotrange * 6/*nNonCotrange * 2*/, nh);//0706//1006
	rtEdge[0] = SetBoundary2(rtEdge[0], CRect(80, 0, nw - 100, nh));
	rtEdge[1] = SetBoundary2(rtEdge[1], CRect(80, 0, nw - 100, nh));
	int midy = rtEdge[0].Height() * 0.5;
	int nEdgeY[3000] = { 0, }, nEdgeY1[3000] = { 0, };
	int ntmpold[2] = { 0, }, noldpos[2] = { 0, }, noldposy[2] = { 0, }, npos = 0;
	int nEdgeTh11 = m_nEdgeTh[nCam]; 25;//30;
	BYTE* fmtmp1 = m_fmTabEdge1[nCam];//new BYTE[nw*nh];
	int nSumEdge1 = 0;
	if (m_bWhiteRoll[nCam])
		nSumEdge1 = PreprocessingCeramic(fmDark, fmtmp1, rtEdge[0], nw, nh);
	else
		nSumEdge1 = PreprocessingCeramic(fm, fmtmp1, rtEdge[0], nw, nh);


	double dCoatEdge1[4] = { 0, };
	int nSumEdge2 = PreprocessingCeramic(fmtmp1, m_fmTabEdge[nCam], rtEdge[1], nw, nh);
	m_dInsSubTime1[nCam][3] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000; //7ms
	double dCoatTopEdge = 0, dCoatBtmEdge = 0;//211002
	int nxoffset1 = 8, ncnt12[2] = { 0, };//4
	int nPeakPos1 = 0, nPeakPos2 = 0;
	int nNoncoatEdge1 = 220;
	if (m_bWhiteRoll[nCam] == TRUE) {
		//		fmTmp = fmDark;
		nNoncoatEdge1 = 180;//(nSumEdge1 +nSumEdge2)*0.5 +10 ;//180;//211101
	}
	dCoatEdge1[0] = m_rtCell[nCam].left;
	dCoatEdge1[1] = dCoatTopEdge = m_rtCell[nCam].right;

	///////////////////////////////////////////////////////////////0615-1 미코팅 탈리
	int nIslandTh = m_nIslandTh[nCam], nw2 = BAD_WIDTH * 0.5, nh2 = BAD_HEIGHT * 0.5, njump1 = 0;
	int nDefect = 0, ntabpos1[10] = { 0, };
	if (m_rtCell[nCam].Width() < 1500)		m_rtCell[nCam].right = m_rtHugeRect[nCam].right * m_nReduction[nCam];//0524-2

	//tab & overlay  //탭 상하 찾기
	double dave1 = 0, dc1 = 0;
	if (rttmp.top < nt1)rttmp.top = nt1;//0623
	if (rttmp.bottom > nt2)rttmp.bottom = nt2;//0623
	if (rttmp.left < dCoatTopEdge)rttmp.left = dCoatTopEdge + 10;//211006 //탭 에지를 찾기위해 리사이징
	if (rttmp.Width() < 100)rttmp.right = rttmp.left + 100;//211006
	if (rttmp.Height() < 300)rttmp.bottom = nh - 150;//211007
	if (rttmp.top - 16 < 0)rttmp.top = 16;//211129
	if (rttmp.bottom > nh - 16)//211129
		rttmp.bottom = nh - 16;
	CRect rttmp12;
	rttmp12 = rttmp;//백색롤러에서 화이트가 적을경우 
		//220110  
	int ntabave11 = 0;

	if (m_bUseIntekTabAlign[nCam] == FALSE)
	{
		ntabave11 = FindTabPos(nCam, fmTmp, fmDark, nw, nh, rttmp12, &nt1, &nt2);//220304

		m_dInsSubTime1[nCam][4] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

		if (nt2 == 0 && nt1 == 0) {
			delete[] m_fmPreprocessTemp;
			return;
		}
		if (nt2 - nt1 <= 200 /*|| (nTabFind1[0] == 0 || nTabFind1[0] == 0)*/) {//211009
			nt1 = m_ptTabCenter[nCam].y * m_nReduction[nCam] - 200;
			nt2 = m_ptTabCenter[nCam].y * m_nReduction[nCam] + 200;
		}
		if (nt2 - nt1 <= 100 && ntabpos1[0] > 0) {//0707
			nt2 = ntabpos1[1];//0707
			nt1 = ntabpos1[0];//0707
		}
		if (rttmp.top < nt1)rttmp.top = nt1;//0623
		if (rttmp.bottom > nt2)rttmp.bottom = nt2;//0623
	//	if(nt2-nt1 > 800 ){
		if (nt2 - nt1 > 1500) {//220418
			int nmid11 = nt2 - nt1;
			nt2 = nmid11 + 250;
			nt1 = nmid11 - 250;
			if (nt1 < 0)nt1 = 50;
			if (nt2 > nh)nt2 = nh - 50;
		}
	}
	else
	{
		if (m_dequeTabAvg[nCam].size() > 0)
		{
			CSingleLock lock(&m_lockTabAvg[nCam], TRUE);
			ntabave11 = m_dequeTabAvg[nCam].front();//220304
			m_dequeTabAvg[nCam].pop_front();
		}
		if (m_dequeTabTopY[nCam].size() > 0)
		{
			CSingleLock lock(&m_lockTabTopY[nCam], TRUE);
			nt1 = m_dequeTabTopY[nCam].front();
			m_dequeTabTopY[nCam].pop_front();
		}
		if (m_dequeTabBottomY[nCam].size() > 0)
		{
			CSingleLock lock(&m_lockTabBottomY[nCam], TRUE);
			nt2 = m_dequeTabBottomY[nCam].front();
			m_dequeTabBottomY[nCam].pop_front();
		}
		m_dInsSubTime1[nCam][4] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;
	}

	rtEdge[1].SetRect(m_rtCell[nCam].right - nNonCotrange * 6, 0, m_rtCell[nCam].right + nNonCotrange * 4/*nNonCotrange * 2*/, nh);//0706//1006
	rtEdge[1] = SetBoundary2(rtEdge[1], CRect(80, 0, nw - 100, nh));

	CRect rtcsrab1;
	if (m_bInsScrab[nCam]) {
		rtcsrab1.SetRect(dCoatTopEdge - m_nScrabRoiLeft[nCam], m_nScrabRoiTop[nCam], dCoatTopEdge - m_nScrabRoiRight[nCam], nh - m_nScrabRoiBot[nCam]);
		rtcsrab1 = rtcsrab1 & CRect(m_rtCell[nCam].left, 0, m_rtCell[nCam].right, nh);
		m_rtScrabIns[nCam] = rtcsrab1;
	}
	//esmb 속도관련 위치 이동
		//if(m_bWhiteRoll[nCam])
		//	nDefect = FindNonCoating(nCam, fm, fmDark, m_nWidth, m_nHeight, nIslandTh, rtEdge[0], rtEdge[1], nNoncoatEdge1, dCoatEdge1, nt1-16, nt2+16, ntabpos1);//211003
		//else
		//	nDefect = FindNonCoating(nCam, fm, fmDark, m_nWidth, m_nHeight, nIslandTh, rtEdge[0], rtEdge[1], (nSumEdge1 + nSumEdge2)*0.5, dCoatEdge1, nt1-16, nt2+16, ntabpos1);//211003

	int nmid = (nt2 + nt1) * 0.5;
	CRect rttabins, rttabins1;//@0514
	int nTabInsOffset = (m_dTabEdgeInspRange[nCam] / m_dscale_y1[nCam]) * 0.5;//중심에서 상하로 구분되어서 /2를 해준다. //220513
	int nTabRightOffset = m_dTabRightOffset[nCam] / m_dscale_x1[nCam];//전극 에지에서 우측으로 옵셋을 주고 좌측으로 에지를 검색함. //220513
	rttabins.SetRect(m_rtHugeRect[nCam].right, nmid - nTabInsOffset, m_rtHugeRect[nCam].right + nTabRightOffset, nmid + nTabInsOffset);//600픽셀->450->480픽셀로 수젇 //220330 //220513 
	rttabins = SetBoundary2(rttabins, CRect(80, 0, nw - 100, nh));
	nRatio = 6;


	m_dInsSubTime1[nCam][5] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;


	int noversplitcnt = 0, noverslitnh = (nt2 - nt1) / 10, noverwidth = rttmp.Width();//0604
	if (noverslitnh == 0) noverslitnh = 1;
	int* nProjEdge1 = new int[sizeof(int) * 10000];//={0,};//= new int[sizeof(int)*nw];	//0729
	memset(nProjEdge1, 0, sizeof(int) * 10000);// //0729
	if (rttmp.left > dCoatTopEdge - 400)//220202 //오버레이를 찾기 위해 리사이징
		rttmp.left = dCoatTopEdge - 400;
	if (rttmp.left < m_nMarginSearchStart[nCam])rttmp.left = m_nMarginSearchStart[nCam];//211017-1
	if (nt1 < 0)nt1 = 0;
	if (nt2 >= nh)nt2 = nh - 1;

	int nEndPos = 0, nEndPos1 = 0, nEndPoscnt = 0;//0707
	nEdgeTh1 = 20;//211003 //211007
	nMargin = 20;//211003
	if (rttabins.top < 0)rttabins.top = 0;
	if (rttabins.bottom >= nh)rttabins.bottom = nh - 1;
	int nWhiteCut1 = 240;
	if (m_bWhiteRoll[nCam] == TRUE) {
		nWhiteCut1 = nNoncoatEdge1 + 10;//211101
		nEdgeTh1 = 20;//211101
	}
	//탭  중심부에 평균이므로 중심부에 노이즈 발생시 위치가 틀어질수 있음.
	int ntabright[1000] = { 0, };//220314-1
	for (int y = rttabins.top; y < rttabins.bottom && y >= 0; y++)//탭 중앙에 우측 끝부분을 찾는다.
	{
		for (int x = rttabins.right; x > rttabins.left && x > 0 && x < m_nWidth - 20; x--) {
			if ((*(fmDark + nw * y + x - 4) - *(fmDark + nw * y + x) > nEdgeTh1 && ntabave11 < *(fmDark + nw * y + x - 8)) || (*(fmTmp + nw * y + x - 4) > nWhiteCut1 && *(fmTmp + nw * y + x) > nWhiteCut1)) {// && *(fm+nw*y+x-4) > nTabave1-nMargin ){//220304
				if (nEndPoscnt < 1000)ntabright[nEndPoscnt] = (x - 4);//220314-1
				nEndPos += (x - 4); nEndPoscnt++; break;//0707
			}
		}
	}
	if (nEndPoscnt <= 0)nEndPoscnt = 1;//0707 //같으면 -> 작거나 같으면 수정
	nEndPos /= nEndPoscnt;//rttabins.Height();//0707 //tab 평균 우측
	//평균 보다 큰거에 대해 한번더 평균을 구한다.
	//sort를 하여 가장 우측 10개 이상 모인데를 찾는다.
	int nEndPos2 = 0, nEndPos3 = 0;
	sort(ntabright, ntabright + nEndPoscnt);

	for (int i = nEndPoscnt - 1; i > 0; i--) {
		if (ntabright[i] > nEndPos && nEndPos3 < 30) {//220513 10 -> 30
			nEndPos2 += ntabright[i]; nEndPos3++;
		}

	}
	//for(int i=0;i<nEndPoscnt;i++){
	//	if(ntabright[i] >nEndPos){
	//		nEndPos2 += ntabright[i];nEndPos3++;
	//	}
	//}
	if (nEndPos3 > 20) {	// sjj - 0413 0
		nEndPos2 /= nEndPos3;
		nEndPos = nEndPos2;
	}
	if (nEndPos - dCoatTopEdge < 170) {//220503
		nEndPos = dCoatTopEdge + 170;//220503
		rttmp.right = nEndPos - 100;
	}
	else
		rttmp.right = nEndPos - 150;
	for (int y = nt1; y < nt2 && y >= 0; y++)
	{
		nProjmin[y] = 255;
		for (int x = rttmp.left; x < rttmp.right && x>0 && x < m_nWidth; x++)
		{
			if (x - rttmp.left >= 10000 || x - rttmp.left < 0)continue;//0607-2
			if (nProj[y] == 0)//211003
				nProj[y] = dave1;//211003
			if (nProj[y] > dave1) nProj[y] = dave1;
			nTmp = nValue + (*(fm + nw * y + x) - nProj[y]) * nRatio;//*(nImageProjection+i);
			if (nTmp < 0)	nTmp = 0;
			else if (nTmp > 255) nTmp = 255;
			if (nProjmin[y] > nTmp)nProjmin[y] = nTmp;
			if (nProjmax[y] < nTmp)nProjmax[y] = nTmp;
			nProj1[x - rttmp.left] += nTmp;//전처리
			nProj2[x - rttmp.left] += *(fm + nw * y + x);//0921 //bright
			if (noverslitnh >= 0 && int(noversplitcnt / noverslitnh) < 10 && noversplitcnt / noverslitnh > 0 && x - rttmp.left < 2000)//220202
				nProj11[int(noversplitcnt / noverslitnh)][x - rttmp.left] += nTmp;//0604
			*(fmPreProcess + nw * y + x) = nTmp;//y 전처리 이미지 
			nProjEdge1[x - rttmp.left] += *(fmDark + nw * y + x);//0729 			//0729 dark image에서 전극 에지를 찾는다
		}
		nminave += nProjmin[y];
		nmaxave += nProjmax[y];
		noversplitcnt++;//0604
		if (noversplitcnt / noverslitnh > 10)break;//0604
	}
	m_dInsSubTime1[nCam][6] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;


	if (nt2 - nt1 <= 0) {
		nt2 = nt1 + 10;
	}
	nminave /= (nt2 - nt1);// 220110
	nmaxave /= (nt2 - nt1);// 220110
	BOOL bTabEdgeSkip = m_bTabPoly[nCam];// 220110
////////Tab의 우상 우하의 위치를 찾는다.//211009
	int nRightTop1 = nEndPos, nRightTop2 = nEndPos + 20;
	if (nt1 + 22 > 0 && nt1 + 22 < m_nHeight - 5 && nRightTop2 > 0 && nRightTop2 < m_nWidth - 70 && nCoatEdge1 + 100>0 && nCoatEdge1 + 100 < m_nWidth - 100) {//211017-1
		for (int y = nt1 + 22; y > nt1 + 20; y--) {
			for (int x = nRightTop2; x > nCoatEdge1 + 180; x--) {//220110
				if (*(fmDark + nw * y + x - 4) - *(fmDark + nw * y + x) > nEdgeTh1 || *(fmTmp + nw * y + x - 4) >= nNoncoatEdge1) {// && *(fm+nw*y+x-4) > nTabave1-nMargin ){//211012
					if (nRightTop1 + 4 > x) {//220323
						nRightTop1 = x; break;
					}
				}
			}
		}
	}
	int nRightBtm1 = nEndPos, nRightBtm2 = nEndPos + 20;
	if (nt2 - 22 > 0 && nt2 - 20 < m_nHeight - 5 && nRightBtm2 > 0 && nRightTop2 < m_nWidth - 70 && nCoatEdge1 + 100>0 && nCoatEdge1 + 100 < m_nWidth - 100) {//211017-1
		for (int y = nt2 - 22; y < nt2 - 20; y++) {
			for (int x = nRightBtm2; x > nCoatEdge1 + 180; x--) {//220110
				if (*(fmDark + nw * y + x - 4) - *(fmDark + nw * y + x) > nEdgeTh1 || *(fmTmp + nw * y + x - 4) >= nNoncoatEdge1) {// && *(fm+nw*y+x-4) > nTabave1-nMargin ){//211012
					if (nRightBtm1 + 4 > x) {//220323
						nRightBtm1 = x; break;
					}
				}
			}
		}
	}
	//더작은 값 //
	if (bTabEdgeSkip) {
		nEndPos1 = nRightTop1;//에지에서 300을 리미트로 한다.
		if (nEndPos1 > nRightBtm1)nEndPos1 = nRightBtm1;
		if (nEndPos - nEndPos1 > 200)
			nEndPos -= 200;
		else
			nEndPos = nEndPos1;
	}

	if (nt2 - nt1 <= 100) {//0707 //0805
		double dcx12 = 0, dcy12 = 0;
		dcx12 = rttmp.right; dcy12 = (nt2 - nt1) * 0.5 + nt1;
		m_rtIsland[nCam][m_nIslandCount[nCam]].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
		m_dISSizeX[nCam][m_nIslandCount[nCam]] = 2;
		m_dISSizeY[nCam][m_nIslandCount[nCam]] = 2;
		m_dISSize[nCam][nDefect] = 0;
		m_nIslandCount[nCam]++;//0706
		delete[] nProjEdge1;//0729
		delete[] m_fmPreprocessTemp;
		return;
	}
	int nTabCoatBrightness = 0, ncc12 = 0;
	for (int x = rttmp.left; x < rttmp.right && x>0 && x < m_nWidth; x++) {//0607-2
		nProj1[x - rttmp.left] /= (nt2 - nt1);//pre
		nProj2[x - rttmp.left] /= (nt2 - nt1);//bright
		nProjEdge1[x - rttmp.left] /= (nt2 - nt1);//dark
		if (x < dCoatTopEdge) {
			nTabCoatBrightness += nProj2[x - rttmp.left]; ncc12++;
		}
	}
	if (ncc12 > 0)
		nTabCoatBrightness /= ncc12;
	double dcoatsum = 0, dcosedge1[10] = { 0, };//0604
	int ncoatsumcnt1 = 0;//0604
	nMargin = 30;//0729  //전극 에지를 먼저 찾는다.  //전처리 프로젝션 데이터
	//bright 에서 찾기
	BOOL boldfind = FALSE;
	if (boldfind) {
		for (int x = rttmp.right - 80; x > rttmp.left + 10 && x > 0; x--) {//전처리 이미지//220223 탭높이가 작을경우 에지를 넘어가는 문제 발생. 100->80으로 수정
			if (nProj1[x - (rttmp.left)] - nProj1[x - (rttmp.left) - 8] > nEdgeTh1) {// && nProj1[x - (rttmp.left ) + 8]>nmaxave - nMargin) {//0623
				nCoatEdge1 = x - 8; break;//코팅 시작 //+8 //0623
			}
		}
	}
	else {
		for (int x = rttmp.right - 80; x > rttmp.left + 10 && x > 0; x--) {//bright 이미지//220223
			if (nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) - 8] > nEdgeTh1 && nProj2[x - (rttmp.left) - 12] < nTabCoatBrightness + 30) {// && nProj1[x - (rttmp.left ) + 8]>nmaxave - nMargin) {//0623
				nCoatEdge1 = x - 8; break;//코팅 시작 //+8 //0623
			}
		}
	}
	dcoatsum = nCoatEdge1; dcosedge1[ncoatsumcnt1] = nCoatEdge1;  ncoatsumcnt1++;
	if (noverslitnh < 1)noverslitnh = 1;
	for (int i = 0; i < 10; i++) {//0604
		if (ncoatsumcnt1 < 10) {
			for (int x = rttmp.left + 10; x < rttmp.right && x>0 && x < m_nWidth; x++) {
				nProj11[i][x - (rttmp.left + 10)] /= noverslitnh;//rttmp.Height(); //여기도 버그 x - rttmp.left+10 -> x - (rttmp.left+10)로 수정함
			}
			for (int x = rttmp.left + 10; x < rttmp.right - 8 && x>0; x++) {//0604
				if (nProj11[i][x - (rttmp.left + 10) + 8] - nProj11[i][x - (rttmp.left + 10)] > nEdgeTh1 && nProj11[i][x - (rttmp.left + 10) + 8] > nmaxave - nMargin) {
					dcoatsum += x + 10; dcosedge1[ncoatsumcnt1] = x + 10; ncoatsumcnt1++; break;//코팅 시작
				}
			}
		}
	}
	nEdgeTh1 = m_nCoatEdgeTh[nCam];
	int nTabEdgecut = m_dMiddleAve[nCam] + 20;//211207
	if (m_bWhiteRoll[nCam])//211207
		nTabEdgecut = m_dMiddleAveDark[nCam] + 40;


	if (m_nAnodeCathode == 0 && m_bOverlayInsp && m_nInsulateType != 1 && m_bInsulTapeInsp[nCam] == TRUE) {//220202 어깨선이 -로 된경우 , 탭왼쪽 전극 밝기가 전극 중앙 밝기보다 30큰경우
		//세라믹일때는 시킵한다  220210
		if (nCoatEdge1 < dCoatTopEdge - 5 || nTabCoatBrightness > m_dMiddleAve[nCam] + 70) {//220211
			m_bOverlayError[nCam] = TRUE;
		}
	}
	//////220321
	//if (m_bInsScrab[nCam] && m_nIslandCount[nCam] < MAX_HUGEDEFECT - 2 ){//220315 //tape 
	//	if(m_bFindWhiteTape[nCam] && m_bTapeInScrab[nCam] ==FALSE)//테이프가 검출되었고 테이프를 스크랩으로 검출 안해야 하는경우 Skip
	//		;
	//	else
	//		nDefect = FindScrab(nCam, fm, fmDark, m_nWidth, m_nHeight, dCoatTopEdge);// nCoatEdge1);//0930//211002
	//}
//nCoatEdge1 어깨선 에지 //dCoatTopEdge 컷팅 에지 // 현재 컷팅에지가 정확하지 않아 어깨선 에지로 사용
	if (nEndPos - nCoatEdge1 < 100)nEndPos = nCoatEdge1 + 250;//211003
	delete[]nProjEdge1;//0729
	if (ncoatsumcnt1 == 0)ncoatsumcnt1 = 1;
	dcoatsum /= ncoatsumcnt1;
	dcoatsum = nCoatEdge1;//0729

	/////////////////여기부터 오버레이 측정/////////////////

	int nOverlayPeak1 = 0, nOverlayPeak2 = 0, noverlaysplitcnt = 0;//0603
	double doverlaysum1 = 0, doverlaysplit[10] = { 0, };
	m_dInsulWidth[nCam] = 0;//0604
	m_dOverlayWidth[nCam] = 0;//0604

	if (TRUE == m_bInspManual)
	{
		memset(m_nOverlayROI_Avg[nCam], 0, sizeof(int) * 10000);
	}

	m_bInspectOverlayAtPreprocess[nCam] = FALSE;

	if (m_bOverlayInsp)
	{//0604
		nMargin = 20;
		int nOverlayold = 0;
		BOOL bFindOverlay1 = FALSE;
		m_overlayROI[nCam] = CRect(rttmp.left + 30, nt1, nCoatEdge1 - 10, nt2);

		if (TRUE == m_bInspManual)
		{
			memcpy(m_nOverlayROI_Avg[nCam], nProj2/*rttmp 영역 내 X좌표별 Y의 밝기 평균*/, sizeof(int) * 10000);
		}

		//		if(m_bCemramic[nCam])
		{//오리진 이미지에서 찾음 
			int nEdgeTh4 = 10;
			if (m_nInsulateType == 1)
			{//ceeramic sbr //220210 pvdf와 ceramic 구분
				nEdgeTh4 = 6;
				if (m_nOverlayTh[nCam] > 0) nEdgeTh4 = m_nOverlayTh[nCam];//220211
				for (int x = rttmp.left + 30; x < nCoatEdge1 - 10 && x > 0 && x < m_nWidth; x++)
				{//220112
					if (nOverlayPeak1 < nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8])//ROI 영역 내 밝기 평균 값으로 연산한다.
					{
						nOverlayPeak1 = nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8]; nOverlayPeak2 = x + 8;
					}
					if (abs(nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8]) > nEdgeTh4)//밝기 차이 값으로 본다.
					{//세라믹의 경우 흑백이 혼재되어 있음.
						nOverlay = x + 8;
						bFindOverlay1 = TRUE;
						break;//오버레이
					}
				}
			}
			else
			{
				if (m_nOverlayTh[nCam] > 0)
					nEdgeTh4 = m_nOverlayTh[nCam];//220211

				for (int x = rttmp.left + 30; x < nCoatEdge1 - 10 && x>0 && x < m_nWidth; x++)
				{//220112
					if (nOverlayPeak1 < nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8])
					{
						nOverlayPeak1 = nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8]; nOverlayPeak2 = x + 8;
					}
					if (nProj2[x - (rttmp.left)] - nProj2[x - (rttmp.left) + 8] > nEdgeTh4 && nProj2[x - (rttmp.left) + 8] < nminave + nMargin)//밝기 차이 값으로 본다.
					{
						nOverlay = x + 8;
						bFindOverlay1 = TRUE;
						break;//오버레이
					}
				}
			}

			if (nOverlay == 0)
				nOverlay = nOverlayPeak2;

			doverlaysum1 = doverlaysum1 = nOverlay;
			double dOverlaySpec = m_dOverlaySize[nCam] + m_dOverlayPlusTolerance[nCam];// 1.1;//211119

			if ((dcoatsum - doverlaysum1) * m_dscale_x1[nCam] > 0.1 && (dcoatsum - doverlaysum1) * m_dscale_x1[nCam] < dOverlaySpec)
				bFindOverlay1 = TRUE;
			else
				bFindOverlay1 = FALSE;
		}
		if (bFindOverlay1 == FALSE) //원본 이미지에서 찾아보고 잘 안 찾아지면 전처리 이미지에서 한번 더 찾는다.
		{//전처리에서 찾음
			m_bInspectOverlayAtPreprocess[nCam] = TRUE;

			if (nmaxave - nminave > 100)
				nMargin = (nmaxave - nminave) * 0.2;//0925-1

			for (int x = rttmp.left + 30; x < nCoatEdge1 && x>0 && x < m_nWidth; x++)
			{
				if (nOverlayPeak1 < nProj1[x - (rttmp.left)] - nProj1[x - (rttmp.left) + 8])
				{
					nOverlayPeak1 = nProj1[x - (rttmp.left)] - nProj1[x - (rttmp.left) + 8]; nOverlayPeak2 = x + 8;
				}
				if (nProj1[x - (rttmp.left)] - nProj1[x - (rttmp.left) + 8] > nEdgeTh2 && nProj1[x - (rttmp.left) + 8] < nminave + nMargin)
				{
					nOverlay = x + 8; break;//오버레이
				}
			}

			if (nOverlay == 0)
				nOverlay = nOverlayPeak2;

			m_overlayROI[nCam] = CRect(rttmp.left + 30, nt1, nCoatEdge1, nt2);

			nOverlayold = nOverlay;
			doverlaysum1 = nOverlay; noverlaysplitcnt++;
			for (int i = 0; i < 10; i++)
			{
				for (int x = rttmp.left + 30; x < nCoatEdge1 && x>0 && x < m_nWidth; x++)
				{
					if (noverlaysplitcnt >= 10)break;

					if (nOverlayPeak1 < nProj11[i][x - (rttmp.left)] - nProj11[i][x - (rttmp.left) + 8])
					{
						nOverlayPeak1 = nProj11[i][x - (rttmp.left)] - nProj11[i][x - (rttmp.left) + 8]; nOverlayPeak2 = x + 8;
					}
					if (nProj11[i][x - (rttmp.left)] - nProj11[i][x - (rttmp.left) + 8] > nEdgeTh2 && nProj11[i][x - (rttmp.left) + 8] < nminave + nMargin)
					{
						nOverlay = x + 8; doverlaysum1 += x + 8; doverlaysplit[i] = x + 8; noverlaysplitcnt++; break;//오버레이
					}
				}
				if (nOverlay == 0)
				{
					doverlaysum1 += nOverlayPeak2;
					doverlaysplit[i] = nOverlayPeak2;
					noverlaysplitcnt++;
				}
			}

			if (noverlaysplitcnt == 0)
				noverlaysplitcnt = 1;

			if (TRUE == m_bInspManual)
			{
				memcpy(m_nOverlayROI_SplitAvg, nProj11, sizeof(int) * 10 * 2000);
			}

			doverlaysum1 /= noverlaysplitcnt;
			sort(doverlaysplit, doverlaysplit + 10);
			double doverlaysum11 = 0;

			for (int i = 4; i < 8; i++)//220111
				doverlaysum11 += doverlaysplit[i];

			doverlaysum11 /= 4;

			if (doverlaysum11 > 0)
				nOverlay = doverlaysum1 = doverlaysum11;
			else
				doverlaysum1 = doverlaysum11 = nOverlay;
		}

		m_dRealOverlayWidth[nCam] = (dcoatsum - doverlaysum1) * m_dscale_x1[nCam];
		m_dOverlayWidth[nCam] = (dcoatsum - doverlaysum1) * m_dscale_x1[nCam];

		if (m_dOverlayWidth[nCam] <= 0.1)
		{//0909
			m_dOverlayWidth[nCam] = (dcoatsum - nOverlayold) * m_dscale_x1[nCam];
			m_dRealOverlayWidth[nCam] = (dcoatsum - nOverlayold) * m_dscale_x1[nCam];
			m_rtOverlay[nCam].SetRect(nOverlayold, nt1, nCoatEdge1, nt2);						//0729-1
			m_rtOverlay[nCam] = SetBoundary2(m_rtOverlay[nCam], CRect(80, 0, nw - 100, nh));	//0729-1
		}
		else
		{
			m_rtOverlay[nCam].SetRect(nOverlay, nt1, nCoatEdge1, nt2);						//0729-1
			m_rtOverlay[nCam] = SetBoundary2(m_rtOverlay[nCam], CRect(80, 0, nw - 100, nh));	//0729-1
		}
		if (m_bOverlayError[nCam] && m_bInsulTapeInsp[nCam])
		{//220210
			m_dOverlayWidth[nCam] = -0.1;//0729-1
			m_rtOverlay[nCam].SetRect(0, 0, 0, 0);
		}
	}
	else
	{
		m_dOverlayWidth[nCam] = 0;//0729-1
		m_rtOverlay[nCam].SetRect(0, 0, 0, 0);
	}

	if (m_bUseIntekTabAlign[nCam] == TRUE)
	{
		int nTabRightPos = nEndPos;

		if (m_bTabRightPosCorrection[nCam] == TRUE)
		{
			int nTabHeightPixel = m_dTabHeight[nCam] / m_dscale_x1[nCam];

			if (nTabRightPos - nCoatEdge1 < nTabHeightPixel * 0.7)
			{
				if (m_dTabHeight[nCam] > 0 && m_dscale_x1[nCam] > 0)
					nTabRightPos = nCoatEdge1 + nTabHeightPixel;
			}
		}

		m_rtCellTab[nCam].SetRect(nCoatEdge1, nt1, nTabRightPos, nt2);
	}
	else
	{
		m_rtCellTab[nCam].SetRect(nCoatEdge1, nt1 - 5, nEndPos, nt2 + 5);
	}

	m_rtCellTab[nCam] = SetBoundary2(m_rtCellTab[nCam], CRect(80, 0, nw - 100, nh));
	int nOffset1 = 0;//10;//코팅 드레그가 심한경우 Skip //220110
	if (m_nMachine == 0)nOffset1 = 80;//Bug
	rttabins1.SetRect(nCoatEdge1 + nOffset1, nt1, nEndPos, nt2);//0611 //220218 영역 버그 수정 필요.
	rttabins1 = SetBoundary2(rttabins1, CRect(80, 0, nw - 100, nh));
	CRect rtspur;//0928
//	rtspur.SetRect(nCoatEdge1 - 140, rttabins1.top - 40, nCoatEdge1 - m_nSpurOffset[nCam], rttabins1.bottom +40);//211023 
	rtspur.SetRect(nCoatEdge1 - 256, rttabins1.top, nCoatEdge1 - m_nSpurOffset[nCam], rttabins1.bottom);//211023 
	rtspur = SetBoundary2(rtspur, CRect(80, 0, nw - 100, nh));//211017-1

	m_rtMinTab[nCam].top = nt1;	m_rtMinTab[nCam].bottom = nt2; m_rtMinTab[nCam].right = nEndPos;
	m_rtMinTab[nCam] = SetBoundary2(m_rtMinTab[nCam], CRect(80, 0, nw - 100, nh));
	m_rtOverlay[nCam].SetRect(nOverlay, nt1, nCoatEdge1, nt2);
	m_rtOverlay[nCam] = SetBoundary2(m_rtOverlay[nCam], CRect(80, 0, nw - 100, nh));
	m_dInsSubTime1[nCam][7] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	if (m_bBottomOverayInsp[nCam]) {//0801 longcell //220111
		rttmpbottom.left = m_rtCell[nCam].left - 100;//0801 longcell
		rttmpbottom.right = m_rtCell[nCam].left + 100;//0801 longcell
		rttmpbottom = SetBoundary2(rttmpbottom, CRect(120, 0, nw - 100, nh));//0812
		InspBottomOverlay(nCam, fmDark, nw, nh, rttmpbottom);
	}

	m_nRealCalcOverlayPos[nCam] = nOverlay + 1;

	/////////////////여기까지 오버레이 측정/////////////////

	int nInsultBright = 0;
	if (m_bInsulateInsp == TRUE && m_bPrimer == FALSE)
	{	//절연 폭//	fmDark //
		int nRightSum = 0, nRightSum1 = 0, nTabSum = 0, nRightSumcnt = 0;

		InspInsulateWidth(nCam, fm, fmDark, nw, nh, rttabins, nCoatEdge1, nEndPos, nOverlay, nt1, nt2, &nRightSum, &nRightSum1);
		m_dInsSubTime1[nCam][15] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

		{
			CSingleLock lock(&m_csRightSum[nCam], TRUE);
			m_vecRightSum[nCam].push_back(nRightSum);
			m_vecRightSum[nCam].push_back(nRightSum1);
		}
	}
	else if (m_bPrimer) {//0630
		m_dInsulWidth[nCam] = 0;
		m_dOverlayWidth[nCam] = 0;
		m_rtInsulate[nCam].SetRect(0, 0, 0, 0);
		m_rtOverlay[nCam].SetRect(0, 0, 0, 0);//Overlay Rect
	}

	{
		CSingleLock lock(&m_csRectEdge[nCam], TRUE);
		m_vecRectEdge[nCam].push_back(rtEdge[0]);
		m_vecRectEdge[nCam].push_back(rtEdge[1]);
	}


	{
		CSingleLock lock(&m_csNoncoatEdge[nCam], TRUE);
		m_vecNoncoatEdge[nCam].push_back(nNoncoatEdge1);
	}

	{
		CSingleLock lock(&m_csCoatEdge[nCam], TRUE);
		m_vecCoatEdge[nCam].push_back(dCoatEdge1[0]);
		m_vecCoatEdge[nCam].push_back(dCoatEdge1[1]);
		m_vecCoatEdge[nCam].push_back(dCoatEdge1[2]);
		m_vecCoatEdge[nCam].push_back(dCoatEdge1[3]);
	}

	{
		CSingleLock lock(&m_cs_nt[nCam], TRUE);
		m_vec_nt[nCam].push_back(nt1);
		m_vec_nt[nCam].push_back(nt2);
	}

	{
		CSingleLock lock(&m_csTabPos[nCam], TRUE);
		for (int i = 0; i < 10; i++)
		{
			m_vecTabPos[nCam].push_back(ntabpos1[i]);
		}
	}

	{
		CSingleLock lock(&m_csOffset1[nCam], TRUE);
		m_vecOffset1[nCam].push_back(nOffset1);
	}

	{
		CSingleLock lock(&m_csRectTabIns[nCam], TRUE);
		m_vecRectTabIns[nCam].push_back(rttabins);
		m_vecRectTabIns[nCam].push_back(rttabins1);
	}

	{
		CSingleLock lock(&m_csRectSpur[nCam], TRUE);
		m_vecRectSpur[nCam].push_back(rtspur);
	}
}
//#endif

void CInspect::GetInsulateAvg(int nCam)
{
	//절연 평균 밝기를 구한다.
	int nRightSum = 0, nRightSum1 = 0;

	{
		CSingleLock lock(&m_csRightSum[nCam], TRUE);
		if (m_vecRightSum[nCam].size() > 0)
		{
			nRightSum = m_vecRightSum[nCam][0];
			nRightSum1 = m_vecRightSum[nCam][1];
		}
	}

	int nw = m_nWidth;
	int nh = m_nHeight;
	BYTE* fm = m_fmEven[nCam];//bright
	BYTE* fmDark = m_fmOdd[nCam];//dark
	BYTE* fmTmp = m_fmEven[nCam];//bright

	int nInsultBright = nRightSum1;

	if (m_bInsulateInsp == TRUE && m_bPrimer == FALSE)
	{
		CRect rtCInsul = m_rtInsulate[nCam];
		rtCInsul = SetBoundary2(m_rtInsulate[nCam], CRect(80, 0, nw - 100, nh));
		IPVM::Rect rtRect = IPVM::FromMFC(rtCInsul);
		IPVM::Rect nrtOffset = IPVM::FromMFC(m_rtInsulationInspAreaOffset[nCam]);
		if (m_bIstest)
		{
			memset(m_BinaryInsulateImage[0], 0, sizeof(BYTE) * m_nWidth * m_nHeight);
			memset(m_BinaryInsulateImage[1], 0, sizeof(BYTE) * m_nWidth * m_nHeight);
		}
		InspInsulate(nCam, fm, fmDark, nw, nh, nRightSum, nRightSum1,rtRect,nrtOffset);
		
		if (m_bOverlayError[nCam] && m_bInsulTapeInsp[nCam])
		{//220210
			m_dOverlayWidth[nCam] = -0.1;//0729-1
			m_rtOverlay[nCam].SetRect(0, 0, 0, 0);
			m_dInsulWidth[nCam] = 0;
		}
		//오버레이폭에맞게 영역 재조정
		int nOverlywidth1 = m_dOverlayWidth[nCam] / m_dscale_x1[nCam];
		m_rtOverlay[nCam].left = m_rtOverlay[nCam].right - nOverlywidth1;
		m_nCorrectionOverlayPos[nCam] = m_rtOverlay[nCam].left;
	}
}

void CInspect::FindNoncoatingAndSpur(int nCam)
{
	int nDefect = 0;

	int nw = m_nWidth;
	int nh = m_nHeight;
	BYTE* fm = m_fmEven[nCam];//bright
	BYTE* fmDark = m_fmOdd[nCam];//dark
	BYTE* fmTmp = m_fmEven[nCam];//bright

	int nIslandTh = m_nIslandTh[nCam];
	CRect rtEdge[2], rtspur;
	CRect rttabins, rttabins1;

	{
		CSingleLock lock(&m_csRectEdge[nCam], TRUE);
		if (m_vecRectEdge[nCam].size() > 0)
		{
			rtEdge[0] = m_vecRectEdge[nCam][0];
			rtEdge[1] = m_vecRectEdge[nCam][1];
		}
	}

	int nInsultBright = 0;

	{
		CSingleLock lock(&m_csRightSum[nCam], TRUE);
		if (m_vecRightSum[nCam].size() > 0)
		{
			nInsultBright = m_vecRightSum[nCam][1];
		}
	}

	int nNoncoatEdge1 = 0, nt1 = 0, nt2 = 0, ntabpos1[10] = { 0, }, nSumEdge1 = 0, nSumEdge2 = 0, nOffset1 = 0;
	double dCoatEdge1[4] = { 0.0f, };

	{
		CSingleLock lock(&m_csNoncoatEdge[nCam], TRUE);
		if(m_vecNoncoatEdge[nCam].size() > 0)
			nNoncoatEdge1 = m_vecNoncoatEdge[nCam][0];
	}

	{
		CSingleLock lock(&m_csCoatEdge[nCam], TRUE);
		if (m_vecCoatEdge[nCam].size() > 0)
		{
			dCoatEdge1[0] = m_vecCoatEdge[nCam][0];
			dCoatEdge1[1] = m_vecCoatEdge[nCam][1];
			dCoatEdge1[2] = m_vecCoatEdge[nCam][2];
			dCoatEdge1[3] = m_vecCoatEdge[nCam][3];
		}
	}

	{
		CSingleLock lock(&m_cs_nt[nCam], TRUE);
		if (m_vec_nt[nCam].size() > 0)
		{
			nt1 = m_vec_nt[nCam][0];
			nt2 = m_vec_nt[nCam][1];
		}
	}

	{
		CSingleLock lock(&m_csTabPos[nCam], TRUE);
		if (m_vecTabPos[nCam].size() > 0)
		{
			for (int i = 0; i < m_vecTabPos[nCam].size(); i++)
			{
				ntabpos1[i] = m_vecTabPos[nCam][i];
			}
		}
	}

	{
		CSingleLock lock(&m_csOffset1[nCam], TRUE);
		if (m_vecOffset1[nCam].size() > 0)
			nOffset1 = m_vecOffset1[nCam][0];
	}

	{
		CSingleLock lock(&m_csRectTabIns[nCam], TRUE);
		if (m_vecRectTabIns[nCam].size() > 0)
		{
			rttabins = m_vecRectTabIns[nCam][0];
			rttabins1 = m_vecRectTabIns[nCam][1];
		}
	}

	{
		CSingleLock lock(&m_csRectSpur[nCam], TRUE);
		if (m_vecRectSpur[nCam].size() > 0)
			rtspur = m_vecRectSpur[nCam][0];
	}

	m_dInsSubTime1[nCam][10] = GetPrecisionTime();

	if (m_bWhiteRoll[nCam])
		nDefect = FindNonCoating(nCam, fm, fmDark, m_nWidth, m_nHeight, nIslandTh, rtEdge[0], rtEdge[1], nNoncoatEdge1, dCoatEdge1, nt1 - 16, nt2 + 16, ntabpos1);//211003 /////////시간 오래 걸리는 함수/////////
	else
		nDefect = FindNonCoating(nCam, fm, fmDark, m_nWidth, m_nHeight, nIslandTh, rtEdge[0], rtEdge[1], (nSumEdge1 + nSumEdge2) * 0.5, dCoatEdge1, nt1 - 16, nt2 + 16, ntabpos1);//211003 /////////시간 오래 걸리는 함수/////////

	//{
	//	CSingleLock lock(&m_csNoncoatingDefect[nCam], TRUE);
	//	m_vecNoncoatingDefect[nCam].push_back(nDefect);
	//}

	//m_bFindNoncoatingFinish[nCam] = true;
	////220330 오버레이폭 구하고 스크랩 검사하도록 수정. //220411 속도문제로 스크랩 스레드로 변경
	//if (m_bInsScrab[nCam] && m_nIslandCount[nCam] < MAX_HUGEDEFECT - 2 ){//220315 //tape 
	//	if(m_bFindWhiteTape[nCam] && m_bTapeInScrab[nCam] ==FALSE)//테이프가 검출되었고 테이프를 스크랩으로 검출 안해야 하는경우 Skip
	//		;
	//	else
	//		nDefect = FindScrab(nCam, fm, fmDark, m_nWidth, m_nHeight, dCoatTopEdge);// nCoatEdge1);//0930//211002
	//}

	int nTabProj[2000] = { 0, };// = new int[sizeof(int)*rttabins1.Width()];memset(nTabProj,0,sizeof(int)*rttabins1.Width());
	int ncnt = 0;
	double dtabAverage = 0, dTabcnt = 0;
	if (rttabins1.top < 0)rttabins1.top = 0;
	if (rttabins1.bottom >= nh)rttabins1.bottom = nh - 1;
	for (int y = rttabins1.top; y < rttabins1.bottom && y >= 0 && y < m_nHeight; y++) {
		for (int x = rttabins1.left; x < rttabins.right && x>0 && x < m_nWidth && x - rttabins1.left < 2000; x++) {
			nTabProj[x - rttabins1.left] += *(fm + nw * y + x);
		}
		ncnt++;
	}
	if (ncnt < 1)ncnt = 1;//211017-1
	for (int x = rttabins1.left; x < rttabins1.right && x - rttabins1.left < 2000; x++) {
		nTabProj[x - rttabins1.left] /= ncnt;
		dtabAverage += nTabProj[x - rttabins1.left]; dTabcnt++;
	}
	if (dTabcnt > 0)dtabAverage /= dTabcnt;
	m_dTabBrightness[nCam][0] = dtabAverage;//0720 brightness tab ave

	if (m_bInsulateInsp == TRUE && m_bPrimer == FALSE && m_bInsulTapeInsp[nCam] && m_nInsulateType != 1 && nInsultBright < dtabAverage - m_nIslandTh[nCam]) {//220210
		m_bOverlayError[nCam] = TRUE;
		m_dOverlayWidth[nCam] = -0.2;//0729-1
		m_rtOverlay[nCam].SetRect(0, 0, 0, 0);
		m_dInsulWidth[nCam] = 0;
	}


	int nXSkip1 = m_dTabSkipRange[nCam][2] / m_dscale_x1[nCam];//right //0614
	int nYSkip1 = m_dTabSkipRange[nCam][0] / m_dscale_y1[nCam];//top //0614
	int nYSkip2 = m_dTabSkipRange[nCam][1] / m_dscale_y1[nCam];//bottom //0614
	if (nYSkip1 < 5)nYSkip1 = 5; if (nYSkip2 < 5)nYSkip2 = 5;

	if (m_nMachine == 0)nOffset1 = 80;//Bug
	if (m_bInsulateInsp) {//220220 탭영역 재조정
		if (rttabins1.left < m_rtInsulate[nCam].right)
			rttabins1.left = m_rtInsulate[nCam].right;
		rttabins1 = SetBoundary2(rttabins1, CRect(80, 0, nw - 100, nh));
	}
	if (rttabins1.Width() < 0 || rttabins1.Height() < 0) {//220225  불량 처리후 리턴한다.
		if (m_nIslandCount[nCam] < MAX_HUGEDEFECT - 1) {
			double dcompact = 50;
			double dangle11 = 0;
			m_rtIslandBlob[nCam][m_nIslandCount[nCam]] = rttabins1;
			double dPercent = 0;
			m_dISAngle[nCam][m_nIslandCount[nCam]] = dangle11;			//0white 1black	//0803
			m_dISCompactness[nCam][m_nIslandCount[nCam]] = dcompact;			//0white 1black //0803
			m_dISSizeX[nCam][m_nIslandCount[nCam]] = 2;
			m_dISSizeY[nCam][m_nIslandCount[nCam]] = 2;
			m_dISSize[nCam][m_nIslandCount[nCam]] = m_dISSizeY[nCam][nDefect];
			m_nIslandCount[nCam]++;
		}
		delete[] m_fmPreprocessTemp;

		return;
	}

	//이진화를 먼저한다.
	if (!m_bPrimer)
		nDefect = FindSpur(nCam, fm, nw, nh, rtspur, m_nSpurValue[nCam]);//211023 /////////시간 오래 걸리는 함수/////////

	nDefect = findIsland(nCam, fm, fmDark, nw, nh, rttabins1, nTabProj, nIslandTh);//0921

	//m_dInsSubTime1[nCam][13] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	int ntag1 = InspNgTag(nCam, fm, fmDark, nw, nh, rttabins1);

	m_dInsSubTime1[nCam][11] = (GetPrecisionTime() - m_dInsSubTime1[nCam][10]) * 1000;

	//m_dInsSubTime1[nCam][14] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	if (m_nCellSize == 1 || !m_bBurrInspect) {//0801 long cell의 경우 버 검사를 하지 않는다 
											  //		m_nIslandCount[nCam]=nDefect; //0921
		m_dInsSubTime1[nCam][9] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000; //26ms
		m_bOverlayCheck[nCam] = FALSE;
		delete[] m_fmPreprocessTemp;
		return;
	}
	//Tab부 Burr check  //0516
	if (m_bBurrInspect) FindBurr(nCam, fm, fmDark, nw, nh, m_rtCellTab[nCam], dtabAverage);//211017-1

	m_bOverlayCheck[nCam] = FALSE;

	if (m_bDefectBinary)
		memcpy(m_fmEven[nCam], m_fmTabEdge[nCam], m_nWidth * m_nHeight);
}

void CInspect::FindIsladAndNgTag(int nCam)
{
	CRect rttabins, rttabins1;

	{
		CSingleLock lock(&m_csRectTabIns[nCam], TRUE);
		if (m_vecRectTabIns[nCam].size() > 0)
		{
			rttabins = m_vecRectTabIns[nCam][0];
			rttabins1 = m_vecRectTabIns[nCam][1];
		}
	}

	int nDefect = 0;

	{
		CSingleLock lock(&m_csNoncoatingDefect[nCam], TRUE);
		if(m_vecNoncoatingDefect[nCam].size() > 0)
			nDefect = m_vecNoncoatingDefect[nCam][0];
	}

	int nInsultBright = 0;

	{
		CSingleLock lock(&m_csRightSum[nCam], TRUE);
		if (m_vecRightSum[nCam].size() > 0)
		{
			nInsultBright = m_vecRightSum[nCam][1];
		}
	}

	int nw = m_nWidth;
	int nh = m_nHeight;
	BYTE* fm = m_fmEven[nCam];//bright
	BYTE* fmDark = m_fmOdd[nCam];//dark

	int nTabProj[2000] = { 0, };// = new int[sizeof(int)*rttabins1.Width()];memset(nTabProj,0,sizeof(int)*rttabins1.Width());
	int ncnt = 0;
	double dtabAverage = 0, dTabcnt = 0;
	if (rttabins1.top < 0)rttabins1.top = 0;
	if (rttabins1.bottom >= nh)rttabins1.bottom = nh - 1;
	for (int y = rttabins1.top; y < rttabins1.bottom && y >= 0 && y < m_nHeight; y++) {
		for (int x = rttabins1.left; x < rttabins.right && x>0 && x < m_nWidth && x - rttabins1.left < 2000; x++) {
			nTabProj[x - rttabins1.left] += *(fm + nw * y + x);
		}
		ncnt++;
	}
	if (ncnt < 1)ncnt = 1;//211017-1
	for (int x = rttabins1.left; x < rttabins1.right && x - rttabins1.left < 2000; x++) {
		nTabProj[x - rttabins1.left] /= ncnt;
		dtabAverage += nTabProj[x - rttabins1.left]; dTabcnt++;
	}
	if (dTabcnt > 0)dtabAverage /= dTabcnt;
	m_dTabBrightness[nCam][0] = dtabAverage;//0720 brightness tab ave

	if (m_bInsulateInsp == TRUE && m_bPrimer == FALSE && m_bInsulTapeInsp[nCam] && m_nInsulateType != 1 && nInsultBright < dtabAverage - m_nIslandTh[nCam]) {//220210
		m_bOverlayError[nCam] = TRUE;
		m_dOverlayWidth[nCam] = -0.2;//0729-1
		m_rtOverlay[nCam].SetRect(0, 0, 0, 0);
		m_dInsulWidth[nCam] = 0;
	}

	int nOffset1 = 0;

	{
		CSingleLock lock(&m_csOffset1[nCam], TRUE);
		if (m_vecOffset1[nCam].size() > 0)
			nOffset1 = m_vecOffset1[nCam][0];
	}

	//int nXSkip1 = m_dTabSkipRange[nCam][2] / m_dscale_x1[nCam];//right //0614
	//int nYSkip1 = m_dTabSkipRange[nCam][0] / m_dscale_y1[nCam];//top //0614
	//int nYSkip2 = m_dTabSkipRange[nCam][1] / m_dscale_y1[nCam];//bottom //0614
	//if (nYSkip1 < 5)nYSkip1 = 5; if (nYSkip2 < 5)nYSkip2 = 5;

	if (m_nMachine == 0)nOffset1 = 80;//Bug
	if (m_bInsulateInsp) {//220220 탭영역 재조정
		if (rttabins1.left < m_rtInsulate[nCam].right)
			rttabins1.left = m_rtInsulate[nCam].right;
		rttabins1 = SetBoundary2(rttabins1, CRect(80, 0, nw - 100, nh));
	}
	if (rttabins1.Width() < 0 || rttabins1.Height() < 0) {//220225  불량 처리후 리턴한다.
		if (m_nIslandCount[nCam] < MAX_HUGEDEFECT - 1) {
			double dcompact = 50;
			double dangle11 = 0;
			m_rtIslandBlob[nCam][m_nIslandCount[nCam]] = rttabins1;
			double dPercent = 0;
			m_dISAngle[nCam][m_nIslandCount[nCam]] = dangle11;			//0white 1black	//0803
			m_dISCompactness[nCam][m_nIslandCount[nCam]] = dcompact;			//0white 1black //0803
			m_dISSizeX[nCam][m_nIslandCount[nCam]] = 2;
			m_dISSizeY[nCam][m_nIslandCount[nCam]] = 2;
			m_dISSize[nCam][m_nIslandCount[nCam]] = m_dISSizeY[nCam][nDefect];
			m_nIslandCount[nCam]++;
		}
		delete[] m_fmPreprocessTemp;

		return;
	}

	int nIslandTh = m_nIslandTh[nCam];

	nDefect = findIsland(nCam, fm, fmDark, nw, nh, rttabins1, nTabProj, nIslandTh);//0921

	//m_dInsSubTime1[nCam][13] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	int ntag1 = InspNgTag(nCam, fm, fmDark, nw, nh, rttabins1);

	//m_dInsSubTime1[nCam][14] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000;

	if (m_nCellSize == 1 || !m_bBurrInspect) {//0801 long cell의 경우 버 검사를 하지 않는다 
											  //		m_nIslandCount[nCam]=nDefect; //0921
		m_dInsSubTime1[nCam][9] = (GetPrecisionTime() - m_dInsSubTime1[nCam][0]) * 1000; //26ms
		m_bOverlayCheck[nCam] = FALSE;
		delete[] m_fmPreprocessTemp;
		return;
	}
	//Tab부 Burr check  //0516
	if (m_bBurrInspect) FindBurr(nCam, fm, fmDark, nw, nh, m_rtCellTab[nCam], dtabAverage);//211017-1

	m_bOverlayCheck[nCam] = FALSE;

	if (m_bDefectBinary)
		memcpy(m_fmEven[nCam], m_fmTabEdge[nCam], m_nWidth * m_nHeight);
}

int CInspect::PreprocessingCeramic(BYTE * fmsrc, BYTE * fmdst,CRect rt,int nPitch,int nHeight)
{
	memcpy(fmdst,fmsrc,nPitch*nHeight);//원본을 복사본에 복사
	int nSum=0,nsumcnt=0,nTmp=0,nSum2=0;
	CRect rt1;
	rt1=rt;
	for(int y=rt1.top;y<rt1.bottom&&y>=0;y++){//rt의 평균 밝기 구하기
		for(int x=rt1.left;x<rt1.right&&x>0;x++){
			nSum += *(fmsrc+y*nPitch+x);
			nsumcnt++;
		}
	}
	if(nsumcnt<1)nSum =1;
	else nSum /= nsumcnt;//영역의 평균 밝기 구하기
//	if(m_nCellSize==1){//0921
		return nSum;
//	}
	int nCut1=nSum,nCut2=255-nSum,ntmp2=0;
	//평균 밝기를 기준으로 equalization을 실시.
	for(int k=0;k<1;k++){
		nsumcnt = 0;
		for(int y=rt1.top;y<rt1.bottom&&y>=0;y++){
			for(int x=rt1.left;x<rt1.right&&x>0;x++){
				nTmp =*(fmdst+y*nPitch+x); 
//				ntmp2 = (255*nTmp)/nCut1;    //검사 돌다가 여기서 죽었음
				if (nTmp>0)
					ntmp2 = (255 * nCut1) / nTmp;//0523
				else ntmp2 = 0;
				if(ntmp2<0)ntmp2 = 0;
				if(ntmp2>255)ntmp2 = 255;
				*(fmdst+y*nPitch+x) = ntmp2;//타겟 영상에 값 넣기
				nSum2 += ntmp2;
				nsumcnt++;
			}
		}
		if(nsumcnt<1)nsumcnt=1;
		nSum2 /= nsumcnt;
		nCut1 = nSum2;
		nCut2=255-nSum2;
	}
	return nSum;
}


//double CInspect::GetSurfaceSpec(CString strSpecName)
//{
//	double value = -1;
//
//	for (int i = 0; i < m_vInspParam.size(); i++)
//	{
//		if (m_vInspParam[i].strName == strSpecName)
//		{
//			value = m_vInspParam[i].dValue;
//			break;
//		}
//	}
//
//	//if (value == -1)
//	//	AfxMessageBox(_T("Get spec error! Please check spec name."));
//
//	return value;
//}
//
//
//BOOL CInspect::SetSurfaceSpec(CString strSpecName, double dValue)
//{
//	BOOL bRes = FALSE;
//
//	for (int i = 0; i < m_vInspParam.size(); i++)
//	{
//		if (m_vInspParam[i].strName == strSpecName)
//		{
//			m_vInspParam[i].dValue = dValue;
//			bRes = TRUE;
//			break;
//		}
//	}
//
//	return bRes;
//}
//
//
//BOOL CInspect::SetInspectEnable(CString strSpecName, BOOL bEnable)
//{
//	BOOL bRes = FALSE;
//
//	for (int i = 0; i < m_vInspParam.size(); i++)
//	{
//		if (m_vInspParam[i].strName == strSpecName)
//		{
//			m_vInspParam[i].bEnable = bEnable;
//			bRes = TRUE;
//			break;
//		}
//	}
//
//	return bRes;
//}

//void CInspect::GetSurfaceSpec(CInspect *stInspParam) // 검사 Parameter 입력
//{
//	//double value = -1;
//
//	for (int i = 0; i < m_vInspParam.size(); i++)
//	{
//		if (m_vInspParam[i].nObject == 2) // 공통
//		{
//			if (m_vInspParam[i].strName == _T("전처리 밝기"))//0
//				m_nTargetBrightness = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("증폭배율"))//1
//				m_dMagValue = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("라인증폭"))//2
//				m_nLineMag = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("하이컷"))//3
//				m_nHighCut = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("로우컷"))//4
//				m_nLowCut = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("라인하이컷"))//5
//				m_nLineHighCut = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("에지TH"))//6
//				m_nEdgeTh = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("자동에지TH"))//7
//				m_bAutoEdgeTh = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("표면검사백TH"))//8
//				m_nTh1 = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("표면검사흑TH"))//9
//				m_nTh2 = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("라인TH"))//10
//				m_dLineCut = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("에지옵셋"))//11
//				m_nEdgeOffset1 = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("아일랜드VH"))//12
//				m_nIslandTh = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("백색마진"))//13
//				m_nMargin[0] = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("흑색마진"))//14
//				m_nMargin[1] = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("라인비율"))//15
//				m_dXYRatio = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("핀홀검출값"))//16
//				m_nPinholeCut = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("원본핀홀TH"))//17
//				m_nPinHoleTh = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("흑분화구검출값"))//18
//				m_nBlackCut1 = m_vInspParam[i].dValue;
//		}
//		else if (m_vInspParam[i].nObject == 1) // 양극
//		{
//			if (m_vInspParam[i].strName == _T("핀홀사이즈X"))//0
//				m_dPinholeSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("핀홀사이즈Y"))//1
//				m_dPinholeSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("반짝이사이즈X"))//2
//				m_dWhiteSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("반짝이사이즈Y"))//3
//				m_dWhiteSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("흑색분화구사이즈X"))//4
//				m_dDarkSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("흑색분화구사이즈Y"))//5
//				m_dDarkSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("핀홀성분화구사이즈X"))//6
//				m_dPinholeCraterBSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("핀홀성분화구사이즈Y"))//7
//				m_dPinholeCraterBSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("찍힘사이즈X"))//8
//				m_dCunicSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("찍힘사이즈Y"))//9
//				m_dCunicSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("라인사이즈X"))//10
//				m_dWhiteLineSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("라인사이즈Y"))//11
//				m_dWhiteLineSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("흑라인사이즈X"))//12
//				m_dBlackLineSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("흑라인사이즈Y"))//13
//				m_dBlackLineSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("아일랜드사이즈X"))//14
//				m_dIslandSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("아일랜드사이즈Y"))//15
//				m_dIslandSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("압축배율"))//16
//				m_nReduction = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("테이프검출백값"))//17
//				m_nLargeDefectWTh = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("테이프검출흑값"))//18
//				m_nLargeDefectBTh = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("열주름사이즈X"))//19
//				m_dWrinkleBlackSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("열주름사이즈Y"))//20
//				m_dWrinkleBlackSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("거대라인X"))//21
//				m_dLargeLineSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("거대라인Y"))//22
//				m_dLargeLineSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("무지주름X"))//23
//				m_dIslandWrikleSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("무지주름Y"))//24
//				m_dIslandWrikleSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("코팅주름X"))//25
//				m_dCoatWrinkleSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("코팅주름Y"))//26
//				m_dCoatWrinkleSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("열주름흑"))//27
//				m_nLineInBlackCount = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("크랙사이즈X"))//28
//				m_dCrackSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("크랙사이즈y"))//29
//				m_dCrackSizeY = m_vInspParam[i].dValue;
//		}
//		else // 음극
//		{
//			if (m_vInspParam[i].strName == _T("핀홀사이즈X"))//0
//				m_dPinholeSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("핀홀사이즈Y"))//1
//				m_dPinholeSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("반짝이사이즈X"))//2
//				m_dWhiteSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("반짝이사이즈Y"))//3
//				m_dWhiteSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("흑색분화구사이즈X"))//4
//				m_dDarkSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("흑색분화구사이즈Y"))//5
//				m_dDarkSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("핀홀성분화구사이즈X"))//6
//				m_dPinholeCraterBSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("핀홀성분화구사이즈Y"))//7
//				m_dPinholeCraterBSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("찍힘사이즈X"))//8
//				m_dCunicSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("찍힘사이즈Y"))//9
//				m_dCunicSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("라인사이즈X"))//10
//				m_dWhiteLineSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("라인사이즈Y"))//11
//				m_dWhiteLineSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("흑라인사이즈X"))//12
//				m_dBlackLineSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("흑라인사이즈Y"))//13
//				m_dBlackLineSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("아일랜드사이즈X"))//14
//				m_dIslandSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("아일랜드사이즈Y"))//15
//				m_dIslandSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("압축배율"))//16
//				m_nReduction = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("테이프검출백값"))//17
//				m_nLargeDefectWTh = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("테이프검출흑값"))//18
//				m_nLargeDefectBTh = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("열주름사이즈X"))//19
//				m_dWrinkleBlackSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("열주름사이즈Y"))//20
//				m_dWrinkleBlackSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("거대라인X"))//21
//				m_dLargeLineSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("거대라인Y"))//22
//				m_dLargeLineSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("무지주름X"))//23
//				m_dIslandWrikleSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("무지주름Y"))//24
//				m_dIslandWrikleSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("코팅주름X"))//25
//				m_dCoatWrinkleSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("코팅주름Y"))//26
//				m_dCoatWrinkleSizeY = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("열주름흑"))//27
//				m_nLineInBlackCount = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("크랙사이즈X"))//28
//				m_dCrackSizeX = m_vInspParam[i].dValue;
//
//			if (m_vInspParam[i].strName == _T("크랙사이즈y"))//29
//				m_dCrackSizeY = m_vInspParam[i].dValue;
//		}
//	}
//
//	//if (value == -1)
//	//	AfxMessageBox(_T("Get spec error! Please check spec name."));
//
//	//return value;
//}

int CInspect::CheckColor(int nType)
{
	int nret = 0;
	if (nType == 1001)		nret = 0;//	  거대라인");
	else if (nType == 1002)	nret = 0;//  "백색라인");
	else if (nType == 1003)	nret = 1;//  "라인긁힘");
	else if (nType == 1004)	nret = 0;//  "바운드라인");
	else if (nType == 1005)	nret = 0;//  "열주름");
	else if (nType == 1006)	nret = 0;//  "코팅주름");
	else if (nType == 1007)	nret = 1;//  "탭주름");
	else if (nType == 2000)	nret = 0;//  "미니핀홀");
	else if (nType == 2001)	nret = 0;//  "핀홀");
	else if (nType == 2002)	nret = 0;//  "반짝이");//돌출
	else if (nType == 2003)	nret = 1;//  "찍힘");
	else if (nType == 2004)	nret = 0;//  "파임");
	else if (nType == 2005)	nret = 1;//  "미니찍힘");
	else if (nType == 3000)	nret = 1;//  "미니분화구");
	else if (nType == 3001)	nret = 1;//  "핀홀성분화구");
	else if (nType == 3002)	nret = 1;//  "흑색분화구");
	else if (nType == 3003)	nret = 1;//  "흑색크랙");
	else if (nType == 4001)	nret = 0;//  "미코팅");
	else if (nType == 5001)	nret = 1;//  "아일랜드");
	else if (nType == 5002)	nret = 1;//  "무지부주름");
	else if (nType == 6001)	nret = 0;//  "Tape");
	else if (nType == 6002)	nret = 0;//  "거대백불량");
	else if (nType == 6003)	nret = 1;//  "거대흑불량");
	else if (nType == 6004)	nret = 0;//  "ngTag");
	else if (nType == 6005)	nret = 0;//  "흑색테이프");
	else if (nType == 7001)	nret = 0;//  "절연핀홀");
	else if (nType == 7002)	nret = 0;//  "절연라인");
	else if (nType == 7003)	nret = 0;//  "절연갭");
	//else if (nType == 7004)	nret = 1;//  "절연아일랜드");
	else if (nType == 8001)	nret = 0;//  "Burr");
	else if (nType == 9001)	nret = 0;//  "Scrab");//0930
	else if (nType == -2)	nret = 1;//  "흑색");//220221
	return nret;
}
void CInspect::PreprocessLongCell(int nCam)//0810
{//211011 평션 바꿀것

}



int CInspect::ROIInspect(int nCam,BYTE * fm, int nw, int nh, CRect rt)
{
	InitManual();
	CRect rtins = rt;
	int *nProj = new int[nw];
	int *nPreProj = new int[nw];
	for (int i = 0; i<nw; i++){
		nProj[i] = nPreProj[i] = 0;
	}
	int nc1 = 0;
	for (int y = rtins.top; y<rtins.bottom; y += 8){
		for (int x = rtins.left; x<rtins.right; x++){
			if (*(fm + nw*y + x) <150)
				nProj[x] += *(fm + nw*y + x);
		}
		nc1++;
	}
	for (int x = rtins.left; x<rtins.right && nc1>0; x++){
		nProj[x] /= nc1;
	}
	//전처리
	BYTE * fmpre = new BYTE[nw*nh];
	memset(fmpre, 128, nw*nh);
	double dmag = m_dMagValue[nCam], dmag1 = m_dMagValue[nCam] * 0.5, dmag2 = 0, dLineMag = m_nLineMag[nCam];
	int nHighCut = m_nHighCut[nCam], nLowCut = m_nLowCut[nCam], nLineHighCut = m_nLineHighCut[nCam], nXcount1 = 0, nTmp = 0, nValue = m_nTargetBrightness[nCam];
	for (int y = rtins.top; y<rtins.bottom; y++){
		for (int x = rtins.left; x<rtins.right; x++){
			if (x<16)continue;//0518-4
			if (*(fm + nw*y + x) - nProj[x]>m_nHighCut[nCam])
				dmag2 = dmag;
			else if ((*(fm + nw*y + x) - nProj[x]) < -nLowCut)dmag2 = dmag;
			else	dmag2 = dmag1;
			nTmp = (nValue + (*(fm + nw*y + x) - nProj[x])*dmag2);//*(nImageProjection+i);
			if (nTmp<0)nTmp = 0x00; if (nTmp>255)nTmp = 0xff;

			if (nProj[x]>200)
				nTmp = 555;//0618-2
			nPreProj[x] += nTmp;//전처리 평균 data 이물
			*(fmpre + nw*y + x) = nTmp;//y 전처리 이미지 이물 전처리
			nXcount1++;
		}
	}
	if(rtins.Height()>0){
		for (int x = rtins.left; x<rtins.right && nc1>0; x++){
			nPreProj[x] /= rtins.Height();
		}
	}

	int nDefectCount = 0, nw2 = BAD_WIDTH*0.5, nh2 = BAD_HEIGHT*0.5, nscoffset = 4;//defect 이미지 사이즈 지훈이랑 맞출것.
	int nw3 = 384, nh3 = 384, nw4 = 192, nh4 = 192;//후보 영역에 대한 사이즈
	int nScValue = m_dLineCut[nCam], nThw = m_nTh1[nCam], nThb = m_nTh2[nCam];
	int nSkip1 = m_nEdgeOffset1[nCam];//21
	int	nIslandThb = m_nIslandTh[nCam];
	BOOL	bFlag1 = FALSE;
	nSkip1 = 0;
	int nEdgeFind = 0, njump1;
	int nDarktmp = 0, nsumpro = 0, nsumpix = 0, nDarkPeak = 255;;//0527-1
	for (int y = rtins.top + 2; y<rtins.bottom - 2; y++)
	{
		nEdgeFind = 0;
		if (nDefectCount >= MAX_MANUAL_DEFECT - 1)
			break;
		for (int x = rtins.left; x<rtins.right; x++)
		{
			njump1 = 0;
			if (nDefectCount >= MAX_MANUAL_DEFECT - 1)
				break;
			nTmp = ((*(fmpre + nw*y + x) - m_nTargetBrightness[nCam]) + (*(fmpre + nw*y + (x + 1)) - m_nTargetBrightness[nCam]))*0.5;
			int nGap1 = 8; nDarktmp = 0;
			int njump = 0;
			if (nTmp >= nThw){
				if (*(fmpre + nw*(y + 1) + x) - nPreProj[x] < nThw)continue;  //??
				for (int p = 0; p<nDefectCount; p++){
					CPoint pt;
					pt.SetPoint(x, y);
					if (m_rtManualDefect[p].PtInRect(pt)>0)
						njump = 1;
					if (njump == 1)break;
				}
				if (njump == 1){ x += nw2; continue; }
				m_nManualDefectBright[nDefectCount] = 1;
				m_ptManualDefectPoint[nDefectCount].SetPoint(x, y);
				m_rtManualDefect[nDefectCount].SetRect(x - nw2, y - nh2, x + nw2, y + nh2);
				m_rtManualDefect[nDefectCount] = SetBoundary2(m_rtManualDefect[nDefectCount], rtins);
				int nEdgeoffset1 = 0;
				int ncode = ManualDualBlob(nCam, fm, fmpre, nPreProj, nDefectCount, 1, nEdgeoffset1);//ManualDualBlob(BYTE * fmOrigin,BYTE* fmPre,int * nPreproj,int nDefect,int nBW,int nEdge )//nbw 3 무지 4찍힘 후보
				m_nManualDefectType[nDefectCount] = ncode;
				nDefectCount++;
				x += nw2;
			}
			else if (nTmp <= -nThb){//밑에 3칸을 본다
				for (int p = 0; p<nDefectCount; p++){
					CPoint pt;
					pt.SetPoint(x, y);
					if (m_rtManualDefect[p].PtInRect(pt)>0)
						njump = 1;
					if (njump == 1)break;
				}
				if (njump == 1){ x += nw2; continue; }
				m_nManualDefectBright[nDefectCount] = 2;
				m_ptManualDefectPoint[nDefectCount].SetPoint(x, y);
				m_rtManualDefect[nDefectCount].SetRect(x - nw2, y - nh2, x + nw2, y + nh2);
				m_rtManualDefect[nDefectCount] = SetBoundary2(m_rtManualDefect[nDefectCount], rtins);
				int nEdgeoffset1 = 0;
				int ncode = ManualDualBlob(nCam,fm, fmpre, nPreProj, nDefectCount, 2, nEdgeoffset1);
				m_nManualDefectType[nDefectCount] = ncode;
				nDefectCount++;
				x += nw2;

			}
			m_nManualDefectCount = nDefectCount;//0706
			if (nDefectCount >= MAX_MANUAL_DEFECT - 1)
				break;
		}
	}

	delete []fmpre;
	delete []nProj;
	delete []nPreProj;
	return nDefectCount;
}


void CInspect::InitManual()
{
	m_nManualDefectCount = 0;
	for (int i = 0; i<MAX_MANUAL_DEFECT; i++){
		m_rtManualDefect[i].SetRect(0, 0, 0, 0);
		m_nManualDefectType[i] = 0;
		m_strManualDefectName[i] = "";
		m_ptManualDefectPoint[i].SetPoint(0, 0);
		m_nManualDefectBright[i] = 0;
		for (int j = 0; j<2; j++){
			m_dManualStdValue[i][j] = 0;
			m_dManualCompactness[i][j] = 0;
			m_dManualDistance[i][j] = 0;
			m_dManualPercent[i][j] = 0;
			m_dManualPeakDataW[i][j] = 0;
			m_dManualPeakDataB[i][j] = 0;
			m_rtManualBlob[i][j].SetRect(0, 0, 0, 0);
			for (int k = 0; k<5; k++)
				m_dManualPeakData[i][j][k] = 0;
		}
		for (int j = 0; j<3; j++){
			m_dManualSizeX[i][j] = 0;
			m_dManualAngle[i][j] = 0;
			m_dManualSizeY[i][j] = 0;
			m_dManualSize[i][j] = 0;
		}
	}
	for (int i = 0; i<2; i++){
		m_dManualOverlay[i] = 0;//0 mm 1 pixel
		m_dManualInsulate[i] = 0;//0 mm 1 pixel
		m_dManualCoat[i] = 0;//0 mm 1 pixel
	}
}

int	CInspect::ManualDualBlob(int nCam,BYTE * fmOrigin, BYTE* fmPre, int * nPreproj, int nDefect, int nBW, int nEdge)//nbw 3 무지 4찍힘 후보
{
	//블랍은 듀얼로 한다.
	int nw = m_nWidth, nh = m_nHeight;
	int nw1 = BAD_WIDTH, nh1 = BAD_HEIGHT;
	BYTE * fm = new BYTE[BAD_WIDTH*BAD_HEIGHT];//전처리
	BYTE * fm3 = new BYTE[BAD_WIDTH*BAD_HEIGHT];//origin
	BYTE * fm1 = new BYTE[BAD_WIDTH*BAD_HEIGHT];//white
	BYTE * fm2 = new BYTE[BAD_WIDTH*BAD_HEIGHT];//black
	BYTE * fm4 = new BYTE[BAD_WIDTH*BAD_HEIGHT];//black+white
	CRect rt2 = m_rtManualDefect[nDefect], rt1, rt;
	ImageSelectCopy(fmPre, fm, nw, nh, rt2, BAD_WIDTH, BAD_HEIGHT, 0, 0);
	ImageSelectCopy(fmOrigin, fm3, nw, nh, rt2, BAD_WIDTH, BAD_HEIGHT, 0, 0);
	int nTmp = 0, nThw = m_nTh1[nCam], nThb = m_nTh2[nCam];//pDoc->m_data.m_dThDn[nCam];
	int nmarginb = m_nMargin[nCam][1];//흑
	int nmarginw = m_nMargin[nCam][0];//백
	int nthw1 = nThw - nmarginw, nthb1 = nThb - nmarginb;//size를 재기위해 마진을 더둔다.
	if (nBW == 4){//0527-1 
		nthb1 -= 8;
	}
	int ngap = 1;//2;
	int nsum = 0, nsum1 = 0, npeakValue[3] = { 0, 1000, 0 }, ncnt1 = 0;
	int nret = -1;
	int nCrack = 0;
	rt.SetRect(0, 0, BAD_WIDTH, BAD_HEIGHT);
	for (int i = 0; i<3; i++){
		m_dManualPeakData[nDefect][0][i] = m_dManualPeakData[nDefect][1][i] = 0;
	}
	for (int i = 0; i<2; i++){
		m_dManualAngle[nDefect][i] = 0;
		m_dManualStdValue[nDefect][i] = 0;
		m_dManualCompactness[nDefect][i] = 0;
		m_dManualSizeX[nDefect][0] = m_dManualSizeY[nDefect][0] = 0;
		m_dManualSizeX[nDefect][1] = m_dManualSizeY[nDefect][1] = 0;
	}

	double dBlackCogx = 0, dBlackCogy = 0, dblackCogCnt = 0;//0524-3
	for (int y = rt.top; y<rt.bottom; y++){
		for (int x = rt.left; x<rt.right; x++){
			if (y <= ngap || y >= BAD_HEIGHT - 1 - ngap || x <= rt.left + ngap || x >= rt.right - 1 - ngap)
				*(fm1 + nw1*y + x) = *(fm2 + nw1*y + x) = 0;//테두리를 흑색으로 치환한다. blob에서 에지가 걸리면 검출이 안된.
			else{
				rt1.SetRect(x - ngap, y - ngap, x + ngap, y + ngap);
				nsum = AreaAve1(fm, rt1, BAD_WIDTH, &nsum1);
				if (nsum>npeakValue[0])npeakValue[0] = nsum1;//이진화를 위한 밝기의 peak
				if (nsum<npeakValue[1])npeakValue[1] = nsum;//이진화를 위한 밝기의 peak
				npeakValue[2] += nsum;
				if (*(fmOrigin + (y + rt2.top)*nw + (x + rt2.left))>m_dManualPeakData[nDefect][0][0])m_dManualPeakData[nDefect][0][0] = *(fmOrigin + (y + rt2.top)*nw + (x + rt2.left));//오리진 피크밝기
				if (*(fmOrigin + (y + rt2.top)*nw + (x + rt2.left))<m_dManualPeakData[nDefect][0][1])m_dManualPeakData[nDefect][0][1] = *(fmOrigin + (y + rt2.top)*nw + (x + rt2.left));//오리진 피크밝기
				m_dManualPeakData[nDefect][0][2] += *(fmOrigin + (y + rt2.top)*nw + (x + rt2.left));
				if (*(fm + y*BAD_WIDTH + x)>m_dManualPeakData[nDefect][1][0])m_dManualPeakData[nDefect][1][0] = *(fm + y*BAD_WIDTH + x);//전처리 피크
				if (*(fm + y*BAD_WIDTH + x)<m_dManualPeakData[nDefect][1][1])m_dManualPeakData[nDefect][1][1] = *(fm + y*BAD_WIDTH + x);//전처리 피크
				m_dManualPeakData[nDefect][1][2] += *(fm + y*BAD_WIDTH + x);//평균
				*(fm2 + nw1*y + x) = *(fm1 + nw1*y + x) = *(fm4 + nw1*y + x) = 0;
				if (nsum1 - nPreproj[x + rt2.left] >nthw1)
					*(fm1 + nw1*y + x) = *(fm4 + nw1*y + x) = 255;
				else if (nsum - nPreproj[x + rt2.left] < -nthb1){
					*(fm2 + nw1*y + x) = 255; *(fm4 + nw1*y + x) = 128;
					dBlackCogx += x; dBlackCogy += y; dblackCogCnt++;//0524-3
				}
				ncnt1++;
			}
		}
	}

	//IPVM::Image_8u_C1 image, image2;
	//image.Create(nw1, nh1);
	//image2.Create(nw1, nh1);
	//memcpy(image.GetMem(), fm2, nw1 * nh1);
	//memcpy(image2.GetMem(), fm4, nw1 * nh1);

	//CString strNum;
	//strNum.Format(_T("%d"), m_ninspCount);

	//image.SaveBmp(_T("Dark_") + strNum + _T(".bmp"));
	//image2.SaveBmp(_T("Bright_Dark_") + strNum + _T(".bmp"));

	//m_ninspCount++;

	if (dblackCogCnt>0){//0524-3 흑의 중심을 구한다.
		dBlackCogx /= dblackCogCnt; dBlackCogy /= dblackCogCnt;
	}
	if(ncnt1<1)ncnt1 = 1;
	npeakValue[2] /= ncnt1;
	m_dManualPeakData[nDefect][0][2] /= ncnt1;//오리진 이미지
	m_dManualPeakData[nDefect][1][2] /= ncnt1;//전처리 이미지

	if (m_bDefectBinary)//1412
		memcpy(m_fmManualDefect[nDefect], fm4, BAD_WIDTH*BAD_HEIGHT);
	//Blob white
	CPoint ptCenter[4];
	for (int i = 0; i<4; i++)ptCenter[i].SetPoint(0, 0);
	int  nBlobCount = -1, nBlobNo = 0;
	double dPercent[4] = { 0, }, dTemp = 0, dBlobSize = 0, dBlobSize1 = 0;//0 count 1 percent 2 count 3 percent
	CRect rtBlob;
	m_pManualChain->SetChainData(1, fm1, 2, 1, 2, 100000, BAD_WIDTH, BAD_HEIGHT);//white
	nBlobCount = m_pManualChain->FastChain(1, 1, BAD_WIDTH - 1, BAD_HEIGHT - 1);
	for (int i = 0; i<nBlobCount; i++)
	{
		dTemp = m_pManualChain->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp<2)continue;
		if (dTemp >= dBlobSize)//최대 불량 사이즈.
		{
			nBlobNo = i;	dBlobSize = dTemp;//면적
		}
	}
	double dcx = 0, dcy = 0, tmp = 0, dSum1 = 0, dSum2 = 0, dstdev2 = 0, dstdev3 = 0, tmp1 = 0, dSum11 = 0, dSum12 = 0, dstdev12 = 0, dstdev13 = 0, dstdev22 = 0, dstdev23 = 0;
	int nTmp11 = 0;
	int nSkip1 = 0;
	if (dBlobSize >= 2){
		////에지부 과검
		m_pManualChain->Chain_Center(nBlobNo, &dcx, &dcy);
		if (nSkip1 == 0){
			int nx1 = m_pManualChain->FindMaxX(nBlobNo);
			int nx2 = m_pManualChain->FindMinX(nBlobNo);
			int ny1 = m_pManualChain->FindMaxY(nBlobNo);
			int ny2 = m_pManualChain->FindMinY(nBlobNo);
			m_dManualSizeX[nDefect][0] = (nx1 - nx2 + 1)*m_dscale_x1[0];
			m_dManualSizeY[nDefect][0] = (ny1 - ny2 + 1)*m_dscale_y1[0];
			if (m_dManualSizeX[nDefect][0]>m_dManualSizeY[nDefect][0])m_dManualSize[nDefect][0] = m_dManualSizeX[nDefect][0];
			else  m_dManualSize[nDefect][0] = m_dManualSizeY[nDefect][0];
			//////////////std 구하기
			int nmid1 = (m_pManualChain->FindMaxX(nBlobNo) + m_pManualChain->FindMinX(nBlobNo))*0.5;
			int nc11 = 0;
			for (int k1 = m_pManualChain->FindMinY(nBlobNo) - 1; k1<m_pManualChain->FindMaxY(nBlobNo) + 1; k1++){
				if (k1<0)k1 = 0; if (k1>BAD_HEIGHT - 1)continue;
				tmp = *(fmOrigin + nw*(k1 + rt2.top) + (nmid1 + rt2.left));//origin
				dSum1 += tmp;
				dSum2 += (tmp*tmp);
				nc11++;
			}
			dstdev2 = sqrt((nc11*dSum2 - (double)dSum1*(double)dSum1) / ((double)nc11*((double)nc11 - 1.)));
			dstdev3 = sqrt((nc11*dSum2 - (double)dSum1*(double)dSum1) / (nc11));///((double)nArea*((double)nArea-1.)));
			m_dManualStdValue[nDefect][0] = dstdev3;
			m_dManualCompactness[nDefect][0] = m_pManualChain->FindCompactness(nBlobNo);
			m_dManualAngle[nDefect][0] = m_pManualChain->FindAngle(nBlobNo);
			m_rtManualBlob[nDefect][0].SetRect(m_pManualChain->FindMinX(nBlobNo), m_pManualChain->FindMinY(nBlobNo), m_pManualChain->FindMaxX(nBlobNo), m_pManualChain->FindMaxY(nBlobNo));
			ptCenter[0].SetPoint(dcx, dcy);
			for (int y = m_rtManualBlob[nDefect][0].top; y <= m_rtManualBlob[nDefect][0].bottom; y++){
				for (int x = m_rtManualBlob[nDefect][0].left; x <= m_rtManualBlob[nDefect][0].right; x++){
					if (*(fm1 + y*BAD_WIDTH + x)>200)
						dPercent[0]++;
				}
			}
			dPercent[1] = dPercent[0] / ((m_rtManualBlob[nDefect][0].Width() - 1)*(m_rtManualBlob[nDefect][0].Height() - 1)) * 100;
			m_dManualPercent[nDefect][0] = dPercent[1];

		}
	}

	//black blob
	CString strbadtype = _T("양품");
	double dratio[4] = { 0, };// x:y 비율 라인 찾기용
	if (m_dManualSizeX[nDefect][0]>0)dratio[0] = m_dManualSizeY[nDefect][0] / m_dManualSizeX[nDefect][0];//white Y/X
	if (m_dManualSizeX[nDefect][1]>0)dratio[1] = m_dManualSizeY[nDefect][1] / m_dManualSizeX[nDefect][1];//Black Y/X
	if (m_dManualSizeY[nDefect][0]>0.5)dratio[2] = m_dManualSizeX[nDefect][0] / m_dManualSizeY[nDefect][0];//White X/Y
	if (m_dManualSizeY[nDefect][1]>0.5)dratio[3] = m_dManualSizeX[nDefect][1] / m_dManualSizeY[nDefect][1];//Black X/y

	//m_nBlackCut1 분화구 컷  //m_nPinholeCut 핀홀 반짝이 컷	//m_dXYRatio Y/X Ratio종횡비
	double ddist = 0;
	if (ptCenter[0].x>0 && ptCenter[0].y>0 && ptCenter[1].x>0 && ptCenter[1].y>0)
		ddist = sqrt(double((ptCenter[0].x - ptCenter[1].x)*(ptCenter[0].x - ptCenter[1].x) + (ptCenter[0].y - ptCenter[1].y)*(ptCenter[0].y - ptCenter[1].y)));
	m_dManualDistance[nDefect][0] = ddist;
	int nBlack1 = 0, nL = 0, nR = 0, nL1 = 0, nR1 = 0, nLineWidth = 0;
	double dscalex = m_dscale_x1[0], dscaley = m_dscale_y1[0];
	/////////////////param
	double dxyratio = m_dXYRatio[nCam], dWhiteSizeX = m_dWhiteSizeX[nCam], dWhiteSizeY = m_dWhiteSizeY[nCam], dBlackSizeX = m_dDarkSizeX[nCam], dBlackSizeY = m_dDarkSizeY[nCam], dBlackPercent = m_dBlackPercent[nCam];
	int nPinholeCut = m_nPinholeCut[nCam];
	BOOL bSizeAndOr = m_bSizeAndOr[nCam];//TRUE;//사이즈 and or 조건

	m_dManualPeakData[nDefect][0][2] = m_dMiddleAve[0];//적은 영역에서 구하고 있어서 평균으로 바꿈

	//재정의
	if (m_dManualSizeX[nDefect][0] >= m_dPinholeSizeX[nCam] && m_dManualSizeY[nDefect][0] >= m_dPinholeSizeY[nCam] && dratio[0] < m_dXYRatio[nCam]){
		strbadtype = "핀홀"; nret = 2001; //0608
	}
	else if (m_dManualSizeX[nDefect][0]> m_dWhiteLineSizeX[nCam] && m_dManualSizeY[nDefect][0]> m_dWhiteLineSizeY[nCam] && (dratio[0] > dxyratio || dratio[2] > dxyratio)){
		strbadtype = "백색라인"; nret = 1002; //0608
	}

	delete []fm;
	delete []fm1;
	delete []fm2;
	delete []fm3;
	delete []fm4;

	return nret;
}

int CInspect::ROIOverlay(int nCam, BYTE * fmEven, BYTE * fmOdd, int nw, int nh, CRect rt)
{
	InitManual();//pilot 5 
	CRect rtins = rt;
	int *nProj1 = new int[nw];
	int *nProj2 = new int[nw];
	BYTE * fmnew = new BYTE[nw*nh];
	memcpy(fmnew, fmEven, nw*nh);

	for (int i = 0; i<nw; i++){
		nProj1[i] = nProj2[i] = 0;
	}
	int nEdgePeak[2] = { 0, };
	int nLeftSum[2] = { 0, }, nRightSum[2] = { 0, };
	int nc1 = 0, nMode = 0, nAve[2] = { 0, }, ncc[5] = { 0, };
	int nHisto1[256]={0,};
	for (int y = rtins.top; y<rtins.bottom; y++){
		for (int x = rtins.left; x<rtins.right; x++){
			nProj1[x] += *(fmEven + nw*y + x);
			nProj2[x] += *(fmOdd + nw*y + x);
			nAve[0] += *(fmEven + nw*y + x);
			nAve[1] += *(fmOdd + nw*y + x);
			nHisto1[*(fmOdd + nw*y + x)]++;
			ncc[0]++;
		}
		nc1++;
	}
	if (ncc[0]>0){
		nAve[0] /= ncc[0];
		nAve[1] /= ncc[0];
	}
	for (int x = rtins.left; x<rtins.right && nc1>0; x++){
		nProj1[x] /= nc1;
		nProj2[x] /= nc1;
		if (x>rtins.left + 5 && abs(nProj1[x] - nProj1[x - 1])>nEdgePeak[0])nEdgePeak[0] = abs(nProj1[x] - nProj1[x - 1]);
		if (x>rtins.left + 5 && abs(nProj2[x] - nProj2[x - 1])>nEdgePeak[1])nEdgePeak[1] = abs(nProj2[x] - nProj2[x - 1]);
		if (x<rtins.left + 5){
			nLeftSum[0] += nProj1[x];
			nLeftSum[1] += nProj2[x];
		}
		else if (x >= rtins.right - 5){
			nRightSum[0] += nProj1[x];
			nRightSum[1] += nProj2[x];
		}
	}
	nLeftSum[0] /= 5; nLeftSum[1] /= 5; nRightSum[0] /= 5; nRightSum[1] /= 5;
//	if(nRightSum[1] < 100)
	{
		int nHistocut = 255,nHistosum = 300,nHistosum1=0;
		for(int n=255;n>100 ;n--){
			nHistosum1 += nHisto1[n];
			if(nHistosum1>nHistosum){
				nHistocut = n;break;
			}
		}
		nRightSum[1] = nHistocut;
		if(nRightSum[0]<240)nRightSum[0] = 240;
	}
	double nOverlaypos = 0, nCoatPos = 0, nInsulatePos = 0, nOverlayTh = m_nnManualOverlayTh, nCoatTh = m_nManualCoatTh, nInsulTh = m_nManualInsulTh;
	ncc[0] = ncc[1] = ncc[2] = 0;
	for (int y = rtins.top; y<rtins.bottom; y++){
		for (int x = rtins.left; x<rtins.right; x++){
			//처음 어두워 지는 지점이 오버레이 //에지가 흐려 오차 발생함
			if (*(fmEven + y*nw + x) - *(fmEven + y*nw + x + 4) >= nOverlayTh && nLeftSum[0] + 30 > *(fmEven + y*nw + x)){
				nOverlaypos += x + 3; ncc[0]++; break;
			}
			if (*(fmEven + y*nw + x) > nRightSum[0] - 20){
				if (*(fmOdd + y*nw + x) - *(fmOdd + y*nw + x + 4) > nCoatTh){
					nOverlaypos += x + 4; ncc[0]++; break;
				}
			}
		}
		for (int x = rtins.left; x<rtins.right; x++){
			//처음 어두워 지는 지점이 오버레이 //
			if (*(fmEven + y*nw + x + 2) - *(fmEven + y*nw + x) >= nCoatTh){
				nCoatPos += x + 2 + 1; ncc[1]++; break;
			}
		}
	}

	//전처리

	//절연 찾기
	//어두운 영상에서
	if (ncc[1]<rt.Height()*0.3 || ncc[1]<1){
		delete []nProj1;
		delete []nProj2;
		delete []fmnew;
		return 0;
	}
	nCoatPos /= ncc[1];

	CRect rtpre;
	rtpre.SetRect(nCoatPos - 100, rt.top, nCoatPos, rt.bottom);
	int *nPreProj1 = new int[rtpre.Height()];
	int *nPreProj2 = new int[rtpre.Width()];
	for (int i = 0; i<rtpre.Width(); i++)
		nPreProj2[i] = 0;
	int y2 = 0, nth2 = 22;
	double dnewover = 0;
	ncc[4] = 0;
	int nEdgeData1[10] = { 0, };
	int nmag = 5;//6
	for (int i = rtpre.top; i<rtpre.bottom; i++, y2++){
		nPreProj1[y2] = 0;
		for (int j = rtpre.left; j<rtpre.right; j++){
			nPreProj1[y2] += *(fmEven + i*nw + j);
		}
		nPreProj1[y2] /= rtpre.Width();
		for (int j = rtpre.left; j<rtpre.right; j++){
			int nTmp1 = 128 + (*(fmEven + i*nw + j) - nPreProj1[y2]) * nmag;
			if (nTmp1<0)nTmp1 = 0; if (nTmp1>255)nTmp1 = 255;
			*(fmnew + i*nw + j) = nTmp1;
		}
	}
	int nmod = rtpre.Height() / 10, x1 = 0, npeak1 = 0, npeak2 = 0, nPeakMax1 = 0, nPeakMax2 = 0;
	if(nmod<1)nmod=1;
	for (int i = rtpre.top; i<rtpre.bottom; i += nmod){
		for (int y = i; y<i + nmod; y++){
			x1 = 0;
			for (int j = rtpre.left + 10; j<rtpre.right; j++, x1++){
				nPreProj2[x1] += *(fmnew + i*nw + j);
			}
		}
		x1 = 0;
		for (int j = rtpre.left + 10; j<rtpre.right; j++, x1++){
			nPreProj2[x1] /= nmod;
		}
		npeak1 = 0, npeak2 = 0;		x1 = 0;
		for (int j = rtpre.left + 10; j<rtpre.right - 4; j++, x1++){
			//			if(nPreProj2[x1] - nPreProj2[x1+4] >nth2)
			{
				if (npeak1<nPreProj2[x1] - nPreProj2[x1 + 4]){
					npeak1 = nPreProj2[x1] - nPreProj2[x1 + 4];
					npeak2 = j + 4;
				}
			}
		}
		if (npeak1>nth2){
			nEdgeData1[ncc[4]] = npeak2;
			dnewover += npeak2;
			ncc[4]++;
		}
		if (nPeakMax1<npeak1){
			nPeakMax1 = npeak1;
			nPeakMax2 = npeak2;
		}
		for (int j = 0; j<rtpre.Width(); j++)
			nPreProj2[j] = 0;
		if (ncc[4] == 10)break;
	}
	sort(nEdgeData1, nEdgeData1 + 10);

	if (ncc[4]>8){
		dnewover = 0;///= ncc[4];
		for (int i = 3; i<7; i++)
			dnewover += nEdgeData1[i];
		dnewover /= 4;
		nOverlaypos = dnewover;
	}
	else{
		if (ncc[0]<rt.Height()*0.3)
			nOverlaypos = nCoatPos;
		else	nOverlaypos /= ncc[0];

	}
	//	dnewover = nOverlaypos = nPeakMax2;
	for (int y = rtins.top; y<rtins.bottom; y++){
		for (int x = nCoatPos + 40; x<rtins.right; x++){
			//처음 어두워 지는 지점이 오버레이 //우측이 밝음
			if (*(fmOdd + y*nw + x + 4) - *(fmOdd + y*nw + x) >= nInsulTh && nRightSum[1] - 30 < *(fmOdd + y*nw + x + 4)){
				nInsulatePos += x + 2; ncc[2]++; break;
			}
		}
	}
	int nInsulatePos1 = 0, nc12 = 0;
	for (int y = rtins.top; y<rtins.bottom; y++){
		for (int x = nCoatPos + 40; x<rtins.right; x++){
			//처음 어두워 지는 지점이 오버레이 //우측이 밝음
			if (*(fmEven + y*nw + x + 4) - *(fmEven + y*nw + x) >= nInsulTh && nRightSum[1] - 30 < *(fmEven + y*nw + x + 4)){
				nInsulatePos1 += x + 2; nc12++; break;
			}
		}
	}

	if (nc12 == 0)nc12 = 1;
	if (ncc[2] == 0) ncc[2] = 1;
	nInsulatePos /= ncc[2];
	nInsulatePos1 /= nc12;
//	if (abs(nInsulatePos - nInsulatePos1) <= 3)
//		nInsulatePos = (nInsulatePos + nInsulatePos1)*0.5;

	m_dManualOverlay[0] = (nCoatPos - nOverlaypos+1)*m_dscale_x1[0];//0 mm 1 pixel
	if (nOverlaypos>nCoatPos){
		ncc[0] = ncc[1] = nOverlaypos = nCoatPos = 0;
		int nEdgeTh1 = nEdgePeak[1] * m_dManualratio1;
		for (int y = rtins.top; y<rtins.bottom; y++){
			for (int x = rtins.left; x<rtins.right; x++){
				//처음 어두워 지는 지점이 오버레이 //
				if (*(fmOdd + y*nw + x) - *(fmOdd + y*nw + x + 2) >= nEdgeTh1){
					nOverlaypos += x + 2; ncc[0]++; break;
				}
			}
			for (int x = rtins.left; x<rtins.right; x++){
				//처음 어두워 지는 지점이 오버레이 //
				if (*(fmOdd + y*nw + x + 2) - *(fmOdd + y*nw + x) >= nEdgeTh1){
					nCoatPos += x + 2; ncc[1]++; break;
				}
			}
		}
		if (ncc[1]<rt.Height()*0.3||ncc[1]<1)
			return 0;
		nCoatPos /= ncc[1];
		if (ncc[0]<rt.Height()*0.3 || ncc[0]<1)
			nOverlaypos = nCoatPos;
		else	nOverlaypos /= ncc[0];
		m_dManualOverlay[0] = (nCoatPos - nOverlaypos)*m_dscale_x1[0];//0 mm 1 pixel
		m_dManualInsulate[0] = (nInsulatePos - nOverlaypos)*m_dscale_x1[0];//0 mm 1 pixel

	}
	else
		m_dManualInsulate[0] = (nInsulatePos - nCoatPos)*m_dscale_x1[0];//0 mm 1 pixel

	//gap 유무 찾기 
	int nIsGap = 0, nIsGap1 = 0, nIsGap2 = 0;
	for (int x = nInsulatePos - 20; x>nOverlaypos - 40; x--){
		if (nProj2[x - 4] - nProj2[x]>10){
			if (nIsGap1 == 0)nIsGap1 = x - 4;
			nIsGap2 = x - 4;
			nIsGap++;
		}
	}
	if (nIsGap>1){
		for (int x = nOverlaypos - 40; x<nIsGap1; x++){
			if (nProj2[x + 4] - nProj2[x]>10){
				nIsGap2 = x + 4;
				break;
			}
		}
		int noffset12 = 0;
		if (nIsGap1 - nIsGap2 >5)
			noffset12 = 1;
		nCoatPos = nIsGap2;
		nOverlaypos = nIsGap1;
		m_dManualOverlay[0] = (nCoatPos - nOverlaypos )*m_dscale_x1[0];//0 mm 1 pixel
		m_dManualInsulate[0] = (nInsulatePos - nOverlaypos - noffset12)*m_dscale_x1[0];//0 mm 1 pixel
	}

	m_dManualOverlay[1] = nOverlaypos;
	m_dManualCoat[1] = nCoatPos;//0 mm 1 pixel
	m_dManualInsulate[1] = nInsulatePos;//0 mm 1 pixel

	delete []nProj1;
	delete []nProj2;
	delete []fmnew;
	delete []nPreProj1;
	delete []nPreProj2;
	return 0;
}
//top 10 35 35
//back 15	26	60	

int	CInspect::ManualIsland(int nCam,BYTE * fmEven, BYTE * fmOdd, int nw, int nh, CRect rt)
{
	//절연 끝을 찾는다.
	InitManual();
	CRect rtins;
	rtins = rt;
	int *nProj1 = new int[nw];
	for (int i = 0; i<nw; i++){
		nProj1[i] = 0;
	}
	int nc1 = 0, nMode = 0, nAve[2] = { 0, }, ncc[5] = { 0, };
	for (int y = rtins.top; y<rtins.bottom; y++){
		for (int x = rtins.left; x<rtins.right + 300; x++){
			if (x>nw - 200)continue;
			nProj1[x] += *(fmEven + nw*y + x);
			nAve[0] += *(fmEven + nw*y + x);
			ncc[0]++;
		}
		nc1++;
	}
	if (ncc[0]>0){
		nAve[0] /= ncc[0];
	}
	for (int x = rtins.left; x<rtins.right + 300; x++){
		nProj1[x] /= rtins.Height();
	}
	int nCoatEdge1 = 0, nNullEdge1 = 0;
	for (int x = rtins.left; x<rtins.right; x++){
		if (nProj1[x + 4] - nProj1[x]>50){
			nCoatEdge1 = x + 4; break;
		}
	}
	for (int x = nCoatEdge1 + 100; x<rtins.right + 300; x++){
		if (nProj1[x] - nProj1[x + 4]>50){
			nNullEdge1 = x + 4; break;
		}
	}
	//Top // Bottom  //대략 350pixel
	CRect rt1;
	rt1.SetRect(nCoatEdge1 + 5, 0, nNullEdge1 - 30, nh);
	int nMidPoint = (rtins.bottom + rtins.top)*0.5, nMidPoint1 = nNullEdge1 - 30;
	for (int y = nMidPoint; y>nMidPoint - 350; y--){
		if (*(fmEven + nw*y + nMidPoint1) - *(fmEven + nw*(y - 4) + nMidPoint1)>100){
			rt1.top = y; break;
		}
	}
	for (int y = nMidPoint; y<nMidPoint + 350; y++){
		if (*(fmEven + nw*y + nMidPoint1) - *(fmEven + nw*(y + 4) + nMidPoint1)>100){
			rt1.bottom = y; break;
		}
	}
	BYTE * fmnew = new BYTE[nw*nh];
	memset(fmnew, 0, nw*nh);

	//이진화
	for (int y = rt1.top + 4; y<rt1.bottom - 4; y++){
		for (int x = rt1.left + 4; x<rt1.right - 4; x++){
			if (nProj1[x] - *(fmEven + nw*y + x) >80){
				*(fmnew + nw*y + x) = 255;
			}
		}
	}

	m_pManualChain->SetChainData(1, fmnew, 2, 2, 4, 1000000, nw, nh);//white
	int nBlobCount = m_pManualChain->FastChain(rt1.left + 4, rt1.top + 4, rt1.right - 4, rt1.bottom - 4);
	int nDefectCount = 0;
	for (int i = 0; i<nBlobCount; i++)
	{
		double  dTemp = m_pManualChain->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp<5)continue;
		int nx1 = m_pManualChain->FindMinX(i);
		int nx2 = m_pManualChain->FindMaxX(i);
		int ny1 = m_pManualChain->FindMinY(i);
		int ny2 = m_pManualChain->FindMaxY(i);
		double dx1 = (nx2 - nx1)*m_dscale_x1[0];
		double dy1 = (ny2 - ny1)*m_dscale_y1[0];
		if (dx1> m_dIslandSizeX[nCam] && dy1 > m_dIslandSizeY[nCam]){
			double dcx = 0, dcy = 0;
			m_pManualChain->Chain_Center(i, &dcx, &dcy);
			//불량
			m_ptManualDefectPoint[nDefectCount].SetPoint(dcx, dcy);
			m_rtManualDefect[nDefectCount].SetRect(dcx - BAD_WIDTH*0.5, dcy - BAD_HEIGHT*0.5, dcx + BAD_WIDTH*0.5, dcy + BAD_HEIGHT*0.5);
			m_dManualSizeX[nDefectCount][0] = (nx2 - nx1)*m_dscale_x1[0];
			m_dManualSizeY[nDefectCount][0] = (ny2 - ny1)*m_dscale_y1[0];
			//			m_rtManualDefect[nDefectCount] = SetBoundary2(m_rtManualDefect[nDefectCount],rtins);
			m_nManualDefectType[nDefectCount] = 5001;
			nDefectCount++;

		}
	}
	m_nManualDefectCount = nDefectCount;//0706


	delete []fmnew;
	delete []nProj1;
	return nDefectCount;
}
#ifdef ORIGIN_PROCESS
int CInspect::FindNonCoating(int nCam, BYTE * fmBright, BYTE * fmDark, int nw, int nh, int nTh, CRect rtinsLeft, CRect rtinsRight, int nEdgeSum1, double * dCoatEdge1, int nt1, int nt2, int * ntabpos1)
{
	///////////////////////////////////////////////////////////////211017-1 영역 변수 수정으로 함수 교체
	//탭구간은 검사에서 Skip하도록 수정해야함. 
	int nIslandTh = m_nIslandTh[nCam], nw2 = BAD_WIDTH*0.5, nh2 = BAD_HEIGHT*0.5, njump1 = 0, nDefect = m_nIslandCount[nCam];//0930
	m_noncoatingOriginBV = 0; //220928SJM
	m_noncoatingRatio[0] = 0, m_noncoatingRatio[1] = 0;
	BYTE * fmNonCoat = new BYTE[m_nWidth*m_nHeight];
	memset(fmNonCoat, 0, m_nWidth*m_nHeight);
	//220503 
	BYTE * fmDarkInsp = new BYTE[m_nWidth*m_nHeight];
	memset(fmDarkInsp, 0, m_nWidth*m_nHeight);
	int nEdgeSum3 = nEdgeSum1;//(nSumEdge1 + nSumEdge2)*0.5;
	int nHightCut1 = nEdgeSum3 + 60;//0707 //0805
	int nHightCut2 = nEdgeSum3 + 60;//0707 //0805
	if (nHightCut1 > 240)nHightCut1 = 240;
	if (nHightCut1 < 210)nHightCut1 = 240;
	if (nHightCut2 > 240)nHightCut2 = 240;//211105
	if (nHightCut2 < 210)nHightCut2 = 240;//211105


	BYTE * fmtmp = fmBright;
	double dMidAve = m_dMiddleAve[nCam];//m_dMiddleAveDark //220110

	if(m_bWhiteRoll[nCam]==TRUE){//dark 이미지에서 찾는다.  //211028
		nHightCut1 = nEdgeSum3+10;//211101
		nHightCut2 = nEdgeSum3+30;//211101
		fmtmp = fmDark;
		dMidAve = m_dMiddleAveDark[nCam];//220110
	}
	CRect rt1, rt2;//211017-1
	rt1 = SetBoundary2(rtinsLeft, CRect(80, 0, nw - 100, nh));//211017-1
	rt2 = SetBoundary2(rtinsRight, CRect(80, 0, nw - 100, nh));//211017-1
	rt1.right = dCoatEdge1[0] + (m_dLeftNoncoatRange[nCam]/m_dscale_x1[nCam]);//220325
	rt2.left = dCoatEdge1[1] - (m_dRightNoncoatRange[nCam]/m_dscale_x1[nCam]);//220325
	rt1 = SetBoundary2(rt1, CRect(80, 0, nw - 100, nh));////220325
	rt2 = SetBoundary2(rt2, CRect(80, 0, nw - 100, nh));////220325

//0630 Skip 영역 찾기
	int ntaboffset1 = 130;//0805
	//if (m_nAnodeCathode == 0 && m_nCellSize == 1)//0805
	if(true == m_bLaserAndCathode[nCam])//221026 YYS
		ntaboffset1 = 200;
	int nXPos1 = dCoatEdge1[1] + ntaboffset1, nTabCnt1 = 0, nTabCnt2 = 0;//,ntabpos1[10]={0,};//0630 //0805
	int nXPos0 = dCoatEdge1[0] - ntaboffset1;
	if(nXPos0<m_nMarginSearchStart[nCam]) nXPos0 = m_nMarginSearchStart[nCam];
	if(nXPos1>m_nMarginSearchEnd[nCam]) nXPos1 = m_nMarginSearchEnd[nCam];
	int nLRrollAve[2]={0,};//0 Left Roll 1 Right Rolll
	for (int y = rt1.top; y < rt1.bottom; y++) {//0630
		//Roll 밝기체크로만 사용.
		if(y<100 || y>rt1.bottom-101){
			nLRrollAve[0] += *(fmtmp + y*m_nWidth + nXPos0);
			nLRrollAve[1] += *(fmtmp + y*m_nWidth + nXPos1);
		}
	}
	nLRrollAve[0] /= 200;
	nLRrollAve[1] /= 200;
	int nYSkip1 = m_nTabEdgeNoncoatOffset[nCam];
	//미코팅 Th를 따로 둔다.//롤을 과검하면 안된다.

	if(m_nNonCoatTh[nCam]==0){
		m_nNonCoatTh[nCam] = 60;
	}
	if(m_nNonCoatTh[nCam]>0){//211106
		nHightCut1 = nHightCut2 = m_nNonCoatTh[nCam];//0을 입력하면 기존값을 쓰고 0보다 크면 설정값을 쓴다
	}
	int nTh1[4]={0,};//0좌측 전극에지 1좌측전극부 2우측 전극부 3우측전극에지
	//다크 TH 설정 //220503
	int nThDark[2]={0,};//0좌측 좌측전극부 1우측전극에지
	double dtmp1 = dMidAve - nLRrollAve[0];//좌측 전극 밝기 - 좌측 롤밝기 -
	double dtmp2 = dMidAve - nLRrollAve[1];//우측 전극 밝기 - 우측 롤밝기 -  60-100 = -40  60-30=30
	if(dtmp1 > 20 && m_nNoncoatBlackTh[nCam]==0)//최소 20 이상은 차이가 발생해야함. 에지부 Loading 꺼짐에 의한 과검 발생 가능.
		nThDark[0] = dtmp1-5;
	else nThDark[0] = m_nNoncoatBlackTh[nCam];
	if(dtmp2 > 20 && m_nNoncoatBlackTh[nCam]==0)
		nThDark[1] = dtmp2-5;
	else nThDark[1] = m_nNoncoatBlackTh[nCam];
	if(nThDark[0] ==0) nThDark[0]  = 20;
	if(nThDark[1] ==0) nThDark[1]  = 20;
	if(!m_bNoncoatDarkInsp[nCam])
		nThDark[0] = nThDark[1] = 255;
	for(int k1=0;k1<4;k1++){
		nTh1[k1] = dMidAve+ m_nNonCoatTh[nCam];
	}
	if(dMidAve > nLRrollAve[0] &&dMidAve> nLRrollAve[1] ){//좌/우 롤 모두 전극 밝기보다 작을 경우
	}
	else if(dMidAve> nLRrollAve[0] && dMidAve< nLRrollAve[1] ){//우측만 밝을 경우
		nTh1[3] = nLRrollAve[1] + m_nNonCoatTh[nCam];
	}
	else if(dMidAve < nLRrollAve[0] && dMidAve > nLRrollAve[1] ){//좌측만 밝을 경우
		nTh1[0] = nLRrollAve[0] + m_nNonCoatTh[nCam];
	}
	else if(dMidAve < nLRrollAve[0] && dMidAve < nLRrollAve[1] ){//둘다 밝을 경우
		nTh1[0] = nLRrollAve[0] + m_nNonCoatTh[nCam];
		nTh1[3] = nLRrollAve[1] + m_nNonCoatTh[nCam];
	}
	for(int k1=0;k1<4;k1++){
		if(nTh1[k1] > 255)
			nTh1[k1] = 255;
	}


//2가지 TH를 쓴다 롤이 밝을 경우 에지부는 TH 를 높이고 나머지는 어둡게 
	int nRightMargin = m_nNonCoatOffset[nCam];//64;//211024
	int ntbSkipPixel = 32;//64
	if(m_bTiltEdge[nCam]){//220403
		for (int y = rt1.top; y < rt1.bottom && y<3000; y++) {
			for (int x = dCoatEdge1[0] - nRightMargin; x < rt1.right && x>10 && x < m_nWidth - 100; x++) {
				//미코팅 영역은 Skip 한다.
				if(y>m_rtNotchArea[nCam][0].top && y<m_rtNotchArea[nCam][0].bottom && x>m_rtNotchArea[nCam][0].left && x<m_rtNotchArea[nCam][0].right)
					continue;
				if(y>m_rtNotchArea[nCam][1].top && y<m_rtNotchArea[nCam][1].bottom && x>m_rtNotchArea[nCam][1].left && x<m_rtNotchArea[nCam][1].right)
					continue;
				if(x<m_nLRedge[nCam][0][y]+2){//m_nEdgeOffset1[nCam]){
					if (*(fmtmp + y*m_nWidth + x) > nTh1[0])*(fmNonCoat + y*m_nWidth + x) = 255;
				}
				else{
					if (*(fmtmp + y*m_nWidth + x) > nTh1[1])*(fmNonCoat + y*m_nWidth + x) = 255;
					if (*(fmBright + y*m_nWidth + x) < dMidAve - nThDark[0])
						*(fmDarkInsp + y*m_nWidth + x) = 255;//220503 전극 밝기보다 Th 보다 작은 값 이진화
				}
			}
			if (y > nt1 - ntbSkipPixel - nYSkip1 && y < nt2 + ntbSkipPixel + nYSkip1)continue;//0617//211008-1
			for (int x = rt2.left; x < dCoatEdge1[1] + nRightMargin && x>10 && x < m_nWidth - 100; x++) {//LV 모델의 경우 어깨선이 0 //0706-2ㅜ
				if(y>m_rtNotchArea[nCam][2].top && y<m_rtNotchArea[nCam][2].bottom && x>m_rtNotchArea[nCam][2].left && x<m_rtNotchArea[nCam][2].right)
					continue;
				if(y>m_rtNotchArea[nCam][3].top && y<m_rtNotchArea[nCam][3].bottom && x>m_rtNotchArea[nCam][3].left && x<m_rtNotchArea[nCam][3].right)
					continue;
				if(x<m_nLRedge[nCam][1][y]-2){//m_nEdgeOffset1[nCam]){//220404
					if (*(fmtmp + y*m_nWidth + x) > nTh1[2])*(fmNonCoat + y*m_nWidth + x) = 255;
					if (*(fmBright + y*m_nWidth + x) < dMidAve - nThDark[1])
						*(fmDarkInsp + y*m_nWidth + x) = 255;//220503 전극 밝기보다 Th 보다 작은 값 이진화
				}
				else{
					if (*(fmtmp + y*m_nWidth + x) > nTh1[3])*(fmNonCoat + y*m_nWidth + x) = 255;
				}
			}
		}
	}
	else{
		for (int y = rt1.top; y < rt1.bottom; y++) {
			for (int x = dCoatEdge1[0] - nRightMargin; x < rt1.right && x>10 && x < m_nWidth - 100; x++) {
				if(y>m_rtNotchArea[nCam][0].top && y<m_rtNotchArea[nCam][0].bottom && x>m_rtNotchArea[nCam][0].left && x<m_rtNotchArea[nCam][0].right)
					continue;
				if(y>m_rtNotchArea[nCam][1].top && y<m_rtNotchArea[nCam][1].bottom && x>m_rtNotchArea[nCam][1].left && x<m_rtNotchArea[nCam][1].right)
					continue;
				if(x<m_rtCell[nCam].left+2){//m_nEdgeOffset1[nCam]){
					if (*(fmtmp + y*m_nWidth + x) > nTh1[0])
						*(fmNonCoat + y*m_nWidth + x) = 255;
				}
				else{
					if (*(fmtmp + y*m_nWidth + x) > nTh1[1])
						*(fmNonCoat + y*m_nWidth + x) = 255;
					if (*(fmtmp + y*m_nWidth + x) < dMidAve - nThDark[0])
						*(fmDarkInsp + y*m_nWidth + x) = 255;//220503 전극 밝기보다 Th 보다 작은 값 이진화
				}

			}
			if (y > nt1 - ntbSkipPixel - nYSkip1 && y < nt2 + ntbSkipPixel + nYSkip1)continue;//0617//211008-1
			for (int x = rt2.left; x < dCoatEdge1[1] + nRightMargin && x>10 && x < m_nWidth - 100; x++) {//LV 모델의 경우 어깨선이 0 //0706-2ㅜ
				if(y>m_rtNotchArea[nCam][2].top && y<m_rtNotchArea[nCam][2].bottom && x>m_rtNotchArea[nCam][2].left && x<m_rtNotchArea[nCam][2].right)
					continue;
				if(y>m_rtNotchArea[nCam][3].top && y<m_rtNotchArea[nCam][3].bottom && x>m_rtNotchArea[nCam][3].left && x<m_rtNotchArea[nCam][3].right)
					continue;
				if(x<m_rtCell[nCam].right-2)//-m_nEdgeOffset1[nCam])
				{
					if (*(fmtmp + y*m_nWidth + x) > nTh1[2])
						*(fmNonCoat + y*m_nWidth + x) = 255;
					if (*(fmtmp + y*m_nWidth + x) < dMidAve - nThDark[1])
						*(fmDarkInsp + y*m_nWidth + x) = 255;//220503 전극 밝기보다 Th 보다 작은 값 이진화
				}
				else{
					if (*(fmtmp + y*m_nWidth + x) > nTh1[3])
						*(fmNonCoat + y*m_nWidth + x) = 255;
				}
			}
		}
	}
	//20220916 SJM
	if (m_bIstest) 
		memcpy(m_NoneCoatingImage, fmNonCoat, sizeof(BYTE) * m_nWidth * m_nHeight);
	//20220916 SJM
	m_rtFullInspRect[nCam][0].SetRect( dCoatEdge1[0] - m_nNonCoatOffset[nCam],m_nCellMargin,rt1.right,rt1.bottom-m_nCellMargin);
	//m_rtFullInspRect[nCam][1].SetRect( rt2.left-32*6,0,dCoatEdge1[1] + m_nNonCoatOffset[nCam],rt1.bottom);
	m_rtFullInspRect[nCam][1].SetRect(rt2.left, m_nCellMargin, dCoatEdge1[1] + m_nNonCoatOffset[nCam], rt1.bottom-m_nCellMargin);//220414
	m_rtFullInspRect[nCam][2].SetRect( dCoatEdge1[0] + m_nEdgeOffset1[nCam],m_nCellMargin,dCoatEdge1[1] - m_nEdgeOffset1[nCam],rt1.bottom-m_nCellMargin);

	int nBlobCount14 = 0;
	if(	m_bBottomNoncoatingUse[nCam]){
		m_pChainIsland[nCam]->SetChainData(1, fmNonCoat, 1, 1, 2, 100000, m_nWidth, m_nHeight);//white
		nBlobCount14 = m_pChainIsland[nCam]->FastChain(rt1.left, m_nCellMargin, rt1.right - 1, rt1.bottom - m_nCellMargin);//0926
	}

	double dTemp14 = 0, dBlobSize14 = 0; int nBlobNo14 = 0;
	int nedgedefect = 0;
	int nDefecCnt1 = 0, nmaxidx = 0;
	int nDefecCnt = 0, nMaxblob1 = 0, nMaxblob = 0;
	for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 왼쪽//0910
	{
		if (nDefect > MAX_HUGEDEFECT - 2)break;
		dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp14 < 4)continue;
		int nx1 = m_pChainIsland[nCam]->FindMaxX(i);//0625-1
		int nx2 = m_pChainIsland[nCam]->FindMinX(i);//0625-1
		int ny1 = m_pChainIsland[nCam]->FindMaxY(i);//0625-1
		int ny2 = m_pChainIsland[nCam]->FindMinY(i);//0625-1
		if (ny1 - ny2 <= 3)continue;//0625-1
		double dcx12 = 0, dcy12 = 0;
		m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
		CPoint pt14;
		pt14.SetPoint(dcx12,dcy12);
		if(m_rtNotchArea[nCam][0].PtInRect(pt14)){
			continue;
		}
		if(m_rtNotchArea[nCam][1].PtInRect(pt14)){
			continue;
		}
		if (nMaxblob < dTemp14)nMaxblob1 = i;
		if (m_bNonCoatingSizeAndOr && (nx1 - nx2 + 1)*m_dscale_x1[nCam] > m_dNoneCoatingSizeX[nCam] && (ny1 - ny2 + 1)*m_dscale_y1[nCam] > m_dNoneCoatingSizeY[nCam])//0825
		{
			nmaxidx = i; nDefecCnt1 = 100; break;
		}
		if (!m_bNonCoatingSizeAndOr && (nx1 - nx2 + 1)*m_dscale_x1[nCam] > m_dNoneCoatingSizeX[nCam] || (ny1 - ny2 + 1)*m_dscale_y1[nCam] > m_dNoneCoatingSizeY[nCam])//0825
		{
			nmaxidx = i; nDefecCnt1 = 100; break;
		}
		else if ((ny1 - ny2 + 1)*m_dscale_y1[nCam] > m_dNoneCoatingSizeY[nCam])
			nDefecCnt1++;

	}
	int nSpecCnt = m_nNoneCoatingCount[nCam];
	if (nDefecCnt1 > nSpecCnt) 
	{
		if (nDefecCnt1 >= 100) {
			for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽 //211118 여러개 불량 잡게 수정
			{
				if (nDefect > MAX_HUGEDEFECT - 2)break;
				nmaxidx = i;
				dTemp14 = m_pChainIsland[nCam]->Chain_Area(nmaxidx);//FChain_Area(i, &dTemp);//			ttt
				if (dTemp14 < 4)continue;
				int nx1 = m_pChainIsland[nCam]->FindMaxX(nmaxidx);//0625-1
				int nx2 = m_pChainIsland[nCam]->FindMinX(nmaxidx);//0625-1
				int ny1 = m_pChainIsland[nCam]->FindMaxY(nmaxidx);//0625-1
				int ny2 = m_pChainIsland[nCam]->FindMinY(nmaxidx);//0625-1
				double dxs = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
				double dys = (ny1 - ny2 + 1)*m_dscale_y1[nCam];

				double dcx12 = 0, dcy12 = 0;
				m_pChainIsland[nCam]->Chain_Center(nmaxidx, &dcx12, &dcy12);//0625-1
				CPoint pt14,pt15;//220211
				pt14.SetPoint(dcx12,dcy12);
				if(m_rtNotchArea[nCam][0].PtInRect(pt14)){
					pt15.SetPoint(nx1,(ny2 + ny1)/2);
					if(m_rtNotchArea[nCam][0].PtInRect(pt15)){//우측에지 기준으로 다시한번 체크한다. //220211
						continue;
					}				
				}
				if(m_rtNotchArea[nCam][1].PtInRect(pt14)){
					pt15.SetPoint(nx1,(ny2 + ny1)/2);
					if(m_rtNotchArea[nCam][1].PtInRect(pt15)){//우측에지 기준으로 다시한번 체크한다. //220211
						continue;
					}				

				}
				CRect rtEdgecheck;//220516
				rtEdgecheck.SetRect(nx2,ny2,nx1,ny1);
				int nEdgeMargin1 = m_nNonCoatingEdgeMargin[nCam];
				double dxsize1 = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
				double dysize1  = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
				int nPinholeCut = m_nPinholeCut[nCam], nBlackCut1 = m_nBlackCut1[nCam];
				int nOrignWV1 = 0,nret = 0;
				double dratio[4] = { 0, };// x:y 비율 라인 찾기용
				if (dxsize1 > 0)dratio[0] = dysize1 / dxsize1;//white Y/X
				if (dxsize1 > 0)dratio[1] = dysize1 / dxsize1;//Black Y/X
				if (dysize1 > 0.5)dratio[2] = dxsize1 / dysize1;//White X/Y
				if (dysize1 > 0.5)dratio[3] = dxsize1 / dysize1;//Black X/y
				double dDistRatio1 = 0, dDistRatio2 = 0, dDistRatio3 = 0, dDistRatio4 = 0;//220426
				double dmindist = 0, dmaxdist = 0, davedist = 0,dmindist1 = 0, dmaxdist1 = 0, davedist1 = 0;//211008-1
				m_pChainIsland[nCam]->FineDistFromPoint(i, dcx12, dcy12, &dmindist1, &dmaxdist1, &davedist1);//장축 단축 구하기  //211008
				if (dmaxdist1 > 0)dDistRatio4 = dmindist1 / dmaxdist1;//211008-1
				if (dmaxdist > 0)dDistRatio1 = dmindist / dmaxdist;//211008-1
				if (dmindist > 0)dDistRatio2 = dmaxdist / dmindist;//211008-1
				dDistRatio3 = dDistRatio1;////211008-1
				double dSpecDistRatio = 3;//.5;//장단축이 3배 이상이면 라인으로 분류//211008-1
				double dlongLen1 = dmindist;
				double dlongLen2 = 16;
				for(int y0 = rtEdgecheck.top;y0<rtEdgecheck.bottom;y0++){
					for(int x0 = rtEdgecheck.left;x0<rtEdgecheck.right;x0++){
						//최고 밝기 찾기
						if(*(fmNonCoat + y0*m_nWidth + x0) > nOrignWV1 && *(fmNonCoat + y0*m_nWidth + x0+1)  > nOrignWV1)  nOrignWV1 = *(fmNonCoat + y0*m_nWidth + x0);
					}
				}
				nOrignWV1 -= m_dMiddleAve[nCam];//중앙값
				m_noncoatingRatio[0] = dratio[0], m_noncoatingRatio[1] = dratio[2];
				m_noncoatingOriginBV = nOrignWV1;

				if ((m_bNonCoatingSizeAndOr && dxs > m_dNoneCoatingSizeX[nCam] && dys > m_dNoneCoatingSizeY[nCam])
					|| (!m_bNonCoatingSizeAndOr && (dxs >= m_dNonCoatMinSize[nCam] && dys >= m_dNonCoatMinSize[nCam]) && (dxs > m_dNoneCoatingSizeX[nCam] || dys > m_dNoneCoatingSizeY[nCam])))
				{//211013 최소사이즈
					//에지에 붙어있는지 확인
					if(m_bNoncoatEdgeCheck[nCam]){
						if(rtEdgecheck.left  <= m_rtCell[nCam].left +nEdgeMargin1){
							nret = 4001;//미코팅
						}
						else{	//핀홀 라인스펙으로 검사
							//오리진 밝기 찾기 추가
							//종횡비 추가
							if (m_bPinholeAndOr) {//////and 조건    핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 //211006
								if ((dxsize1 >= m_dPinholeSizeX[nCam] && dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
									nret = 2001; //0608/*strbadtype = "핀홀";*/ 
								else if ((dxsize1>= m_dWhiteSizeX[nCam] && dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam]) {// m_dPeakData[nCam][nIdx][nDefect][1][0]-m_nTargetBrightness[nCam] > m_nTh1[nCam]) {//220216 
									 nret = 2002; //0608/*strbadtype = "반짝이";*/
								}//2111123
							}
							else {//211013 최소사이즈
								if ((dxsize1>= m_dPinholeMinSize[nCam] && dysize1>= m_dPinholeMinSize[nCam]) && (dxsize1 >= m_dPinholeSizeX[nCam] || dysize1>= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut) 
									 nret = 2001; //0608/*strbadtype = "핀홀";*/
								else if ((dxsize1 >= m_dProtrusionMinSize[nCam] && dysize1 >= m_dProtrusionMinSize[nCam]) && (dxsize1 >= m_dWhiteSizeX[nCam] || dysize1 >= m_dWhiteSizeX[nCam]) &&nOrignWV1 >= m_nProtrusionTh[nCam]) 
									nret = 2002; //0608/*strbadtype = "반짝이";*/
							}
							if (m_bLineSizeAndOr) {
								if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1> m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) 
									nret = 1002; //0608strbadtype = "백색라인"; 
								else if (m_nLineValue[nCam] < nOrignWV1 &&(dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) 
									nret = 1002; //0608strbadtype = "백색라인"; 
							}
							else {//211013 최소사이즈
								if(m_nLineValue[nCam] < nOrignWV1){
									if ( ((dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) ||(dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) && m_nLineValue[nCam] < nOrignWV1) // && )//수직이나 수평인 경우 영역내면적이커짐  주위 흑색이 있으면 안됨.
									{
										{
											if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) 
												 nret = 1002; //0608strbadtype = "백색라인";
											else if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) 
												 nret = 1002; //0608strbadtype = "백색라인";
										}
									}
								}
							}
						}
					}
					else{
						nret = 4001;
					}
					if(nret>0){
						m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
						m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
						m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
						m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
						m_dISSize[nCam][nDefect] = nret;//4001;
						BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
						CRect rt22 = m_rtIsland[nCam][nDefect];
						if (!m_bDefectBinary)//0729-1
							ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						else
							ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						nDefect++;
					}
				}
				if (nDefect > MAX_HUGEDEFECT - 2)break;
				if(m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE){	// 220126 - sjj
					//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
					if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
					{
						if(nOrignWV1 > m_nScrabTh[nCam]){//Sacrab TH 보다 커야 스크랩이다.
							m_pChainIsland[nCam]->Chain_Center(nmaxidx, &dcx12, &dcy12);//0625-1
							m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
							m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
							m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
							m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
							m_dISSize[nCam][nDefect] = 9001;
							BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
							CRect rt22 = m_rtIsland[nCam][nDefect];
							if (!m_bDefectBinary)//0729-1
								ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							else
								ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							nDefect++;
						}
					}
				}
				else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
				{
					CPoint pt22;
					pt22.SetPoint(dcx12,dcy12);
					if(m_rtScrabIns[nCam].PtInRect(pt22)){
						//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
						if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
						{
							if(nOrignWV1 > m_nScrabTh[nCam]){//Sacrab TH 보다 커야 스크랩이다.
								m_pChainIsland[nCam]->Chain_Center(nmaxidx, &dcx12, &dcy12);//0625-1
								m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
								m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
								m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
								m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
								m_dISSize[nCam][nDefect] = 9001;
								BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
								CRect rt22 = m_rtIsland[nCam][nDefect];
								if (!m_bDefectBinary)//0729-1
									ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								else
									ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								nDefect++;
							}
						}
					}
				}

				if (nDefect > MAX_HUGEDEFECT - 2)break;
			}
		}
		else {//개수에서 만족
			for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 왼쪽
			{
				if (nDefect > MAX_HUGEDEFECT - 2)break;
				dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
				if (dTemp14 < 4)continue;
				int nx1 = m_pChainIsland[nCam]->FindMaxX(i);//0625-1
				int nx2 = m_pChainIsland[nCam]->FindMinX(i);//0625-1
				int ny1 = m_pChainIsland[nCam]->FindMaxY(i);//0625-1
				int ny2 = m_pChainIsland[nCam]->FindMinY(i);//0625-1
				if (ny1 - ny2 <= 3)continue;//0625-1
				double dxs = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
				double dys = (ny1 - ny2 + 1)*m_dscale_y1[nCam];

				double dcx12 = 0, dcy12 = 0;
				m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				CPoint pt14,pt15;
				pt14.SetPoint(dcx12,dcy12);
				if(m_rtNotchArea[nCam][0].PtInRect(pt14)){
					pt15.SetPoint(nx1,(ny2 + ny1)/2);
					if(m_rtNotchArea[nCam][0].PtInRect(pt15)){//우측에지 기준으로 다시한번 체크한다. //220211
						continue;
					}				
				}
				if(m_rtNotchArea[nCam][1].PtInRect(pt14)){
					pt15.SetPoint(nx1,(ny2 + ny1)/2);
					if(m_rtNotchArea[nCam][1].PtInRect(pt15)){//우측에지 기준으로 다시한번 체크한다. //220211
						continue;
					}				
				}
				CRect rtEdgecheck;//220516
				rtEdgecheck.SetRect(nx2,ny2,nx1,ny1);
				int nEdgeMargin1 = m_nNonCoatingEdgeMargin[nCam];
				double dxsize1 = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
				double dysize1  = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
				int nPinholeCut = m_nPinholeCut[nCam], nBlackCut1 = m_nBlackCut1[nCam];
				int nOrignWV1 = 0,nret = 0;
				double dratio[4] = { 0, };// x:y 비율 라인 찾기용
				if (dxsize1 > 0)dratio[0] = dysize1 / dxsize1;//white Y/X
				if (dxsize1 > 0)dratio[1] = dysize1 / dxsize1;//Black Y/X
				if (dysize1 > 0.5)dratio[2] = dxsize1 / dysize1;//White X/Y
				if (dysize1 > 0.5)dratio[3] = dxsize1 / dysize1;//Black X/y
				double dDistRatio1 = 0, dDistRatio2 = 0, dDistRatio3 = 0, dDistRatio4 = 0;//220426
				double dmindist = 0, dmaxdist = 0, davedist = 0,dmindist1 = 0, dmaxdist1 = 0, davedist1 = 0;//211008-1
				m_pChainIsland[nCam]->FineDistFromPoint(i, dcx12, dcy12, &dmindist1, &dmaxdist1, &davedist1);//장축 단축 구하기  //211008
				if (dmaxdist1 > 0)dDistRatio4 = dmindist1 / dmaxdist1;//211008-1
				if (dmaxdist > 0)dDistRatio1 = dmindist / dmaxdist;//211008-1
				if (dmindist > 0)dDistRatio2 = dmaxdist / dmindist;//211008-1
				dDistRatio3 = dDistRatio1;////211008-1
				double dSpecDistRatio = 3;//.5;//장단축이 3배 이상이면 라인으로 분류//211008-1
				double dlongLen1 = dmindist;
				double dlongLen2 = 16;
				for(int y0 = rtEdgecheck.top;y0<rtEdgecheck.bottom;y0++){
					for(int x0 = rtEdgecheck.left;x0<rtEdgecheck.right;x0++){
						//최고 밝기 찾기
						if(*(fmNonCoat + y0*m_nWidth + x0)  > nOrignWV1)  nOrignWV1 = *(fmNonCoat + y0*m_nWidth + x0);
					}
				}
				nOrignWV1 -= m_dMiddleAve[nCam];//중앙값
				m_noncoatingRatio[0] = dratio[0], m_noncoatingRatio[1] = dratio[2];
				m_noncoatingOriginBV = nOrignWV1;
				if ((m_bNonCoatingSizeAndOr && dxs > m_dNoneCoatingSizeX[nCam] && dys > m_dNoneCoatingSizeY[nCam])
					|| (!m_bNonCoatingSizeAndOr && (dxs >= m_dNonCoatMinSize[nCam] && dys >= m_dNonCoatMinSize[nCam]) && (dxs > m_dNoneCoatingSizeX[nCam] || dys > m_dNoneCoatingSizeY[nCam])))
				{//211013 최소사이즈

					if(m_bNoncoatEdgeCheck[nCam]){
						if(rtEdgecheck.left  <= m_rtCell[nCam].left +nEdgeMargin1){
							nret = 4001;//미코팅
						}
						else{	//핀홀 라인스펙으로 검사
							//오리진 밝기 찾기 추가
							//종횡비 추가
							if (m_bPinholeAndOr) {//////and 조건    핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 //211006
								if ((dxsize1 >= m_dPinholeSizeX[nCam] && dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
									nret = 2001; //0608/*strbadtype = "핀홀";*/ 
								else if ((dxsize1>= m_dWhiteSizeX[nCam] && dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam]) 
									nret = 2002; //0608/*strbadtype = "반짝이";*/
							}
							else {//211013 최소사이즈
								if ((dxsize1>= m_dPinholeMinSize[nCam] && dysize1>= m_dPinholeMinSize[nCam]) && (dxsize1 >= m_dPinholeSizeX[nCam] || dysize1>= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut) 
									nret = 2001; //0608/*strbadtype = "핀홀";*/
								else if ((dxsize1 >= m_dProtrusionMinSize[nCam] && dysize1 >= m_dProtrusionMinSize[nCam]) && (dxsize1 >= m_dWhiteSizeX[nCam] || dysize1 >= m_dWhiteSizeX[nCam]) &&nOrignWV1 >= m_nProtrusionTh[nCam]) 
									nret = 2002; //0608/*strbadtype = "반짝이";*/
							}
							if (m_bLineSizeAndOr) {
								if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1> m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) 
									nret = 1002; //0608strbadtype = "백색라인"; 
								else if (m_nLineValue[nCam] < nOrignWV1 &&(dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) 
									nret = 1002; //0608strbadtype = "백색라인"; 
							}
							else {//211013 최소사이즈
								if(m_nLineValue[nCam] < nOrignWV1){
									if ( ((dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) ||(dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) && m_nLineValue[nCam] < nOrignWV1) // && )//수직이나 수평인 경우 영역내면적이커짐  주위 흑색이 있으면 안됨.
									{
										{
											if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) 
												nret = 1002; //0608strbadtype = "백색라인";
											else if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) 
												nret = 1002; //0608strbadtype = "백색라인";
										}
									}
								}
							}
						}
					}
					else{
						nret = 4001;
					}
					if(nret >0){
						m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
						m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
						m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
						m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
						m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
						m_dISSize[nCam][nDefect] = nret;
						BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
						CRect rt22 = m_rtIsland[nCam][nDefect];
						if (!m_bDefectBinary)//0729-1
							ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						else
							ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						nDefect++;
					}
				}
				if (nDefect > MAX_HUGEDEFECT - 2)break;
				if(m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE){		// 220126 - sjj
					//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
					if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
					{
						if(nOrignWV1 > m_nScrabTh[nCam]){//Sacrab TH 보다 커야 스크랩이다.
							m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
							m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
							m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
							m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
							m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
							m_dISSize[nCam][nDefect] = 9001;
							BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
							CRect rt22 = m_rtIsland[nCam][nDefect];
							if (!m_bDefectBinary)//0729-1
								ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							else
								ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							nDefect++;
						}
					}
				}
				else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
				{
					CPoint pt22;
					pt22.SetPoint(dcx12,dcy12);
					if(m_rtScrabIns[nCam].PtInRect(pt22)){
						//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
						if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
						{
							if(nOrignWV1 > m_nScrabTh[nCam]){//Sacrab TH 보다 커야 스크랩이다.
								m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
								m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
								m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
								m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
								m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
								m_dISSize[nCam][nDefect] = 9001;
								BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
								CRect rt22 = m_rtIsland[nCam][nDefect];
								if (!m_bDefectBinary)//0729-1
									ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								else
									ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								nDefect++;
							}
						}
					}
				}
				if (nDefect > MAX_HUGEDEFECT - 2)break;
			}
		}
	}

	nBlobCount14 = 0;
	if(m_bTabNoncoatingUse[nCam]){
		m_pChainIsland[nCam]->SetChainData(1, fmNonCoat, 1, 1, 2, 100000, m_nWidth, m_nHeight);//white
		nBlobCount14 = m_pChainIsland[nCam]->FastChain(rt2.left, m_nCellMargin, rt2.right - 1, rt2.bottom - m_nCellMargin);//0926
	}
	dTemp14 = 0, dBlobSize14 = 0; nBlobNo14 = 0;
	nDefecCnt = 0, nMaxblob1 = 0, nMaxblob = 0;
	for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽  //0910
	{
		if (nDefect > MAX_HUGEDEFECT - 2)break;
		dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp14 < 4)continue;
		int nx1 = m_pChainIsland[nCam]->FindMaxX(i);//0625-1
		int nx2 = m_pChainIsland[nCam]->FindMinX(i);//0625-1
		int ny1 = m_pChainIsland[nCam]->FindMaxY(i);//0625-1
		int ny2 = m_pChainIsland[nCam]->FindMinY(i);//0625-1
		if (ny1 - ny2 <= 2)continue;//220214 //미코팅 Y Size 확
		double dcx12 = 0, dcy12 = 0;
		m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
		CPoint pt14;
		pt14.SetPoint(dcx12,dcy12);
		if(m_rtNotchArea[nCam][2].PtInRect(pt14)){
			continue;
		}
		if(m_rtNotchArea[nCam][3].PtInRect(pt14)){
			continue;
		}

		if (nMaxblob < dTemp14)nMaxblob1 = i;
		if (m_bNonCoatingSizeAndOr && (nx1 - nx2 + 1)*m_dscale_x1[nCam] > m_dNoneCoatingSizeX[nCam] && (ny1 - ny2 + 1)*m_dscale_y1[nCam] > m_dNoneCoatingSizeY[nCam])//1006
		{
			nDefecCnt = 100; break;
		}
		if (!m_bNonCoatingSizeAndOr && (nx1 - nx2 + 1)*m_dscale_x1[nCam] > m_dNoneCoatingSizeX[nCam] || (ny1 - ny2 + 1)*m_dscale_y1[nCam] > m_dNoneCoatingSizeY[nCam])//1006
		{
			nDefecCnt = 100; break;
		}
		else if ((ny1 - ny2 + 1)*m_dscale_y1[nCam] > m_dNoneCoatingSizeY[nCam] || (nx1 - nx2 + 1)*m_dscale_y1[nCam] > m_dNoneCoatingSizeX[nCam] )	// 220214 미코팅 사이즈 확인
			nDefecCnt++;
	}
	if (nDefecCnt < nSpecCnt ||  nDefecCnt == 0)
		;
	else if (nSpecCnt > 0 && nDefecCnt > 0 && nDefecCnt >= nSpecCnt && nDefecCnt < 100) {//1006
		if(nDefect <MAX_HUGEDEFECT-1){
			int nx1 = m_pChainIsland[nCam]->FindMaxX(nMaxblob1);//0625-1
			int nx2 = m_pChainIsland[nCam]->FindMinX(nMaxblob1);//0625-1
			int ny1 = m_pChainIsland[nCam]->FindMaxY(nMaxblob1);//0625-1
			int ny2 = m_pChainIsland[nCam]->FindMinY(nMaxblob1);//0625-1

			double dcx12 = 0, dcy12 = 0;
			m_pChainIsland[nCam]->Chain_Center(nMaxblob1, &dcx12, &dcy12);//0625-1
			CPoint pt14,pt15;//220211
			pt14.SetPoint(dcx12,dcy12);
			int nJump = 0;
			if(m_rtNotchArea[nCam][2].PtInRect(pt14)){
				pt15.SetPoint(nx2,(ny2 + ny1)/2);
				if(m_rtNotchArea[nCam][2].PtInRect(pt14)){//좌측에지 기준으로 다시한번 체크한다. //220211
					nJump=1;
				}				
			}
			if(m_rtNotchArea[nCam][3].PtInRect(pt14)){
				pt15.SetPoint(nx2,(ny2 + ny1)/2);
				if(m_rtNotchArea[nCam][3].PtInRect(pt14)){//좌측에지 기준으로 다시한번 체크한다. //220211
					nJump=1;
				}	
			}
			if(nJump==0){
				CRect rtEdgecheck;//220516
				rtEdgecheck.SetRect(nx2,ny2,nx1,ny1);
				int nEdgeMargin1 = m_nNonCoatingEdgeMargin[nCam];
				double dxsize1 = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
				double dysize1  = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
				int nPinholeCut = m_nPinholeCut[nCam], nBlackCut1 = m_nBlackCut1[nCam];
				int nOrignWV1 = 0,nret = 0;
				double dratio[4] = { 0, };// x:y 비율 라인 찾기용
				if (dxsize1 > 0)dratio[0] = dysize1 / dxsize1;//white Y/X
				if (dxsize1 > 0)dratio[1] = dysize1 / dxsize1;//Black Y/X
				if (dysize1 > 0.5)dratio[2] = dxsize1 / dysize1;//White X/Y
				if (dysize1 > 0.5)dratio[3] = dxsize1 / dysize1;//Black X/y
				double dDistRatio1 = 0, dDistRatio2 = 0, dDistRatio3 = 0, dDistRatio4 = 0;//220426
				double dmindist = 0, dmaxdist = 0, davedist = 0,dmindist1 = 0, dmaxdist1 = 0, davedist1 = 0;//211008-1
				m_pChainIsland[nCam]->FineDistFromPoint(nMaxblob1, dcx12, dcy12, &dmindist1, &dmaxdist1, &davedist1);//장축 단축 구하기  //211008
				if (dmaxdist1 > 0)dDistRatio4 = dmindist1 / dmaxdist1;//211008-1
				if (dmaxdist > 0)dDistRatio1 = dmindist / dmaxdist;//211008-1
				if (dmindist > 0)dDistRatio2 = dmaxdist / dmindist;//211008-1
				dDistRatio3 = dDistRatio1;////211008-1
				double dSpecDistRatio = 3;//.5;//장단축이 3배 이상이면 라인으로 분류//211008-1
				double dlongLen1 = dmindist;
				double dlongLen2 = 16;
				for(int y0 = rtEdgecheck.top;y0<rtEdgecheck.bottom;y0++){
					for(int x0 = rtEdgecheck.left;x0<rtEdgecheck.right;x0++){
						//최고 밝기 찾기
						if(*(fmNonCoat + y0*m_nWidth + x0)  > nOrignWV1)  nOrignWV1 = *(fmNonCoat + y0*m_nWidth + x0);
					}
				}
				nOrignWV1 -= m_dMiddleAve[nCam];//중앙값

				if(m_bNoncoatEdgeCheck[nCam]){
					if(rtEdgecheck.right  >= m_rtCell[nCam].right - nEdgeMargin1){
						nret = 4001;//미코팅
					}
					else{	//핀홀 라인스펙으로 검사
						//오리진 밝기 찾기 추가
						//종횡비 추가
						if (m_bPinholeAndOr) {//////and 조건    핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 //211006
							if ((dxsize1 >= m_dPinholeSizeX[nCam] && dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
								nret = 2001; //0608/*strbadtype = "핀홀";*/ 
							else if ((dxsize1>= m_dWhiteSizeX[nCam] && dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam]) {// m_dPeakData[nCam][nIdx][nDefect][1][0]-m_nTargetBrightness[nCam] > m_nTh1[nCam]) {//220216 
								nret = 2002; //0608/*strbadtype = "반짝이";*/
							}//2111123
						}
						else {//211013 최소사이즈
							if ((dxsize1>= m_dPinholeMinSize[nCam] && dysize1>= m_dPinholeMinSize[nCam]) && (dxsize1 >= m_dPinholeSizeX[nCam] || dysize1>= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut) 
								nret = 2001; //0608/*strbadtype = "핀홀";*/
							else if ((dxsize1 >= m_dProtrusionMinSize[nCam] && dysize1 >= m_dProtrusionMinSize[nCam]) && (dxsize1 >= m_dWhiteSizeX[nCam] || dysize1 >= m_dWhiteSizeX[nCam]) &&nOrignWV1 >= m_nProtrusionTh[nCam]) 
								nret = 2002; //0608/*strbadtype = "반짝이";*/
						}
						if (m_bLineSizeAndOr) {
							if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1> m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) 
								nret = 1002; //0608strbadtype = "백색라인"; 
							else if (m_nLineValue[nCam] < nOrignWV1 &&(dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) 
								nret = 1002; //0608strbadtype = "백색라인"; 
						}
						else {//211013 최소사이즈
							if(m_nLineValue[nCam] < nOrignWV1){
								if ( ((dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) ||(dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) && m_nLineValue[nCam] < nOrignWV1) // && )//수직이나 수평인 경우 영역내면적이커짐  주위 흑색이 있으면 안됨.
								{
									{
										if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) 
											nret = 1002; //0608strbadtype = "백색라인";
										else if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) 
											nret = 1002; //0608strbadtype = "백색라인";
									}
								}
							}
						}
					}
				}
				else{
					nret = 4001;
				}

				if(nret>0){
					m_pChainIsland[nCam]->Chain_Center(nMaxblob1, &dcx12, &dcy12);//0625-1
					m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
					m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
					m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
					m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
					m_dISSize[nCam][nDefect] = nret;
					BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
					CRect rt22 = m_rtIsland[nCam][nDefect];
					if (!m_bDefectBinary)//0729-1
						ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					else
						ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nDefect++;
				}
				if (nDefect > MAX_HUGEDEFECT - 2){
					delete[]fmNonCoat;//0618
					delete [] fmDarkInsp;
					m_nIslandCount[nCam] = nDefect;//0706
					return nDefect;
				}
				double dxs = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
				double dys = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
				if(m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE){	// 220126 - sjj
					//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
					if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
					{
						if(nOrignWV1 > m_nScrabTh[nCam]){//Sacrab TH 보다 커야 스크랩이다.
							m_pChainIsland[nCam]->Chain_Center(nmaxidx, &dcx12, &dcy12);//0625-1
							m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
							m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
							m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
							m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
							m_dISSize[nCam][nDefect] = 9001;
							BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
							CRect rt22 = m_rtIsland[nCam][nDefect];
							if (!m_bDefectBinary)//0729-1
								ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							else
								ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

							nDefect++;
						}
					}
				}
				else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
				{
					CPoint pt22;
					pt22.SetPoint(dcx12,dcy12);
					if(m_rtScrabIns[nCam].PtInRect(pt22)){
						//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
						if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
						{
							if(nOrignWV1 > m_nScrabTh[nCam]){//Sacrab TH 보다 커야 스크랩이다.
								m_pChainIsland[nCam]->Chain_Center(nmaxidx, &dcx12, &dcy12);//0625-1
								m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
								m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
								m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
								m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
								m_dISSize[nCam][nDefect] = 9001;
								BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
								CRect rt22 = m_rtIsland[nCam][nDefect];
								if (!m_bDefectBinary)//0729-1
									ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								else
									ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

								nDefect++;
							}
						}
					}
				}

			}
			if (nDefect > MAX_HUGEDEFECT - 2){
				delete[]fmNonCoat;//0618
				delete [] fmDarkInsp;//220503
				m_nIslandCount[nCam] = nDefect;//0706
				return nDefect;
			}
		}

		delete[]fmNonCoat;//0618
		delete [] fmDarkInsp;//220503
		m_nIslandCount[nCam] = nDefect;//0706
		return nDefect;
	}
	else {
		int nYbad = 0;
		for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽
		{
			if (nDefect > MAX_HUGEDEFECT - 2)break;
			dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp14 < 4)continue;
			int nx1 = m_pChainIsland[nCam]->FindMaxX(i);//0625-1
			int nx2 = m_pChainIsland[nCam]->FindMinX(i);//0625-1
			int ny1 = m_pChainIsland[nCam]->FindMaxY(i);//0625-1
			int ny2 = m_pChainIsland[nCam]->FindMinY(i);//0625-1
			if (ny1 - ny2 <= 3)continue;//0706
			double dcx12 = 0, dcy12 = 0;
			m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
			CPoint pt14,pt15;//220211
			pt14.SetPoint(dcx12,dcy12);//센터가 포함되게 되어있음.
			if(m_rtNotchArea[nCam][2].PtInRect(pt14)){
				pt15.SetPoint(nx2,(ny2 + ny1)/2);
				if(m_rtNotchArea[nCam][2].PtInRect(pt14)){//좌측에지 기준으로 다시한번 체크한다. //220211
					continue;
				}	
			}
			if(m_rtNotchArea[nCam][3].PtInRect(pt14)){
				pt15.SetPoint(nx2,(ny2 + ny1)/2);
				if(m_rtNotchArea[nCam][3].PtInRect(pt14)){//좌측에지 기준으로 다시한번 체크한다. //220211
					continue;
				}	
			}

			double dxs = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
			double dys = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
			CRect rtEdgecheck;//220516
			rtEdgecheck.SetRect(nx2,ny2,nx1,ny1);
			int nEdgeMargin1 = m_nNonCoatingEdgeMargin[nCam];
			double dxsize1 = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
			double dysize1  = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
			int nPinholeCut = m_nPinholeCut[nCam], nBlackCut1 = m_nBlackCut1[nCam];
			int nOrignWV1 = 0,nret = 0;
			double dratio[4] = { 0, };// x:y 비율 라인 찾기용
			if (dxsize1 > 0)dratio[0] = dysize1 / dxsize1;//white Y/X
			if (dxsize1 > 0)dratio[1] = dysize1 / dxsize1;//Black Y/X
			if (dysize1 > 0.5)dratio[2] = dxsize1 / dysize1;//White X/Y
			if (dysize1 > 0.5)dratio[3] = dxsize1 / dysize1;//Black X/y
			double dDistRatio1 = 0, dDistRatio2 = 0, dDistRatio3 = 0, dDistRatio4 = 0;//220426
			double dmindist = 0, dmaxdist = 0, davedist = 0,dmindist1 = 0, dmaxdist1 = 0, davedist1 = 0;//211008-1
			m_pChainIsland[nCam]->FineDistFromPoint(i, dcx12, dcy12, &dmindist1, &dmaxdist1, &davedist1);//장축 단축 구하기  //211008
			if (dmaxdist1 > 0)dDistRatio4 = dmindist1 / dmaxdist1;//211008-1
			if (dmaxdist > 0)dDistRatio1 = dmindist / dmaxdist;//211008-1
			if (dmindist > 0)dDistRatio2 = dmaxdist / dmindist;//211008-1
			dDistRatio3 = dDistRatio1;////211008-1
			double dSpecDistRatio = 3;//.5;//장단축이 3배 이상이면 라인으로 분류//211008-1
			double dlongLen1 = dmindist;
			double dlongLen2 = 16;
			for(int y0 = rtEdgecheck.top;y0<rtEdgecheck.bottom;y0++){
				for(int x0 = rtEdgecheck.left;x0<rtEdgecheck.right;x0++){
					//최고 밝기 찾기
					if(*(fmNonCoat + y0*m_nWidth + x0)  > nOrignWV1 && *(fmNonCoat + y0*m_nWidth + x0+1)  > nOrignWV1)  nOrignWV1 = *(fmNonCoat + y0*m_nWidth + x0);
				}
			}
			nOrignWV1 -= m_dMiddleAve[nCam];//중앙값
			if ((m_bNonCoatingSizeAndOr && dxs > m_dNoneCoatingSizeX[nCam] && dys > m_dNoneCoatingSizeY[nCam])
				|| (!m_bNonCoatingSizeAndOr && (dxs >= m_dNonCoatMinSize[nCam] && dys >= m_dNonCoatMinSize[nCam]) && (dxs > m_dNoneCoatingSizeX[nCam] || dys > m_dNoneCoatingSizeY[nCam])))
			{//211013 최소사이즈

				if(m_bNoncoatEdgeCheck[nCam]){
					if(rtEdgecheck.right  >= m_rtCell[nCam].right - nEdgeMargin1){
						nret = 4001;//미코팅
					}
					else{	//핀홀 라인스펙으로 검사
						//오리진 밝기 찾기 추가
						//종횡비 추가
						if (m_bPinholeAndOr) {//////and 조건    핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 //211006
							if ((dxsize1 >= m_dPinholeSizeX[nCam] && dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
								nret = 2001; //0608/*strbadtype = "핀홀";*/ 
							else if ((dxsize1>= m_dWhiteSizeX[nCam] && dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam]) {// m_dPeakData[nCam][nIdx][nDefect][1][0]-m_nTargetBrightness[nCam] > m_nTh1[nCam]) {//220216 
								nret = 2002; //0608/*strbadtype = "반짝이";*/
							}//2111123
						}
						else {//211013 최소사이즈
							if ((dxsize1>= m_dPinholeMinSize[nCam] && dysize1>= m_dPinholeMinSize[nCam]) && (dxsize1 >= m_dPinholeSizeX[nCam] || dysize1>= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut) 
								nret = 2001; //0608/*strbadtype = "핀홀";*/
							else if ((dxsize1 >= m_dProtrusionMinSize[nCam] && dysize1 >= m_dProtrusionMinSize[nCam]) && (dxsize1 >= m_dWhiteSizeX[nCam] || dysize1 >= m_dWhiteSizeX[nCam]) &&nOrignWV1 >= m_nProtrusionTh[nCam]) 
								nret = 2002; //0608/*strbadtype = "반짝이";*/
						}
						if (m_bLineSizeAndOr) {
							if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1> m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) 
								nret = 1002; //0608strbadtype = "백색라인"; 
							else if (m_nLineValue[nCam] < nOrignWV1 &&(dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) 
								nret = 1002; //0608strbadtype = "백색라인"; 
						}
						else {//211013 최소사이즈
							if(m_nLineValue[nCam] < nOrignWV1){
								if ( ((dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) ||(dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) && m_nLineValue[nCam] < nOrignWV1) // && )//수직이나 수평인 경우 영역내면적이커짐  주위 흑색이 있으면 안됨.
								{
									{
										if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) 
											nret = 1002; //0608strbadtype = "백색라인";
										else if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) 
											nret = 1002; //0608strbadtype = "백색라인";
									}
								}
							}
						}
					}
				}
				else{
					nret = 4001;
				}
				if(nret>0){
					m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
					m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
					m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
					m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
					m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
					m_dISSize[nCam][nDefect] = nret;
					BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
					CRect rt22 = m_rtIsland[nCam][nDefect];
					if (!m_bDefectBinary)//0729-1
						ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					else
						ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nDefect++;
				}
				if (nDefect > MAX_HUGEDEFECT - 2)break;
				if(m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE){	// 220126 - sjj
					//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
					if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
					{
						if(nOrignWV1 > m_nScrabTh[nCam]){//Sacrab TH 보다 커야 스크랩이다.
							m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
							m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
							m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
							m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
							m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
							m_dISSize[nCam][nDefect] = 9001;
							BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
							CRect rt22 = m_rtIsland[nCam][nDefect];
							if (!m_bDefectBinary)//0729-1
								ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							else
								ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

							nDefect++;
						}
					}
				}
				else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
				{
					CPoint pt22;
					pt22.SetPoint(dcx12,dcy12);
					if(m_rtScrabIns[nCam].PtInRect(pt22)){
						//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
						if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
						{
							if(nOrignWV1 > m_nScrabTh[nCam]){//Sacrab TH 보다 커야 스크랩이다.
								m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
								m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
								m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
								m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
								m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
								m_dISSize[nCam][nDefect] = 9001;
								BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
								CRect rt22 = m_rtIsland[nCam][nDefect];
								if (!m_bDefectBinary)//0729-1
									ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								else
									ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								nDefect++;
							}
						}
					}
				}
				if (nDefect > MAX_HUGEDEFECT - 2)break;

			}
			else{//211025
				if(m_bStep[nCam]){
					if(dys > m_dNoneCoatingSizeY[nCam]){
						nYbad++;
					}
				}
				if(nYbad>1){
					if(m_bNoncoatEdgeCheck[nCam]){
						if(rtEdgecheck.right  >= m_rtCell[nCam].right -nEdgeMargin1){
							nret = 4001;//미코팅
						}
						else{	//핀홀 라인스펙으로 검사
							//오리진 밝기 찾기 추가
							//종횡비 추가
							if (m_bPinholeAndOr) {//////and 조건    핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 //211006
								if ((dxsize1 >= m_dPinholeSizeX[nCam] && dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
									nret = 2001; //0608/*strbadtype = "핀홀";*/ 
								else if ((dxsize1>= m_dWhiteSizeX[nCam] && dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam]) {// m_dPeakData[nCam][nIdx][nDefect][1][0]-m_nTargetBrightness[nCam] > m_nTh1[nCam]) {//220216 
									nret = 2002; //0608/*strbadtype = "반짝이";*/
								}//2111123
							}
							else {//211013 최소사이즈
								if ((dxsize1>= m_dPinholeMinSize[nCam] && dysize1>= m_dPinholeMinSize[nCam]) && (dxsize1 >= m_dPinholeSizeX[nCam] || dysize1>= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut) 
									nret = 2001; //0608/*strbadtype = "핀홀";*/
								else if ((dxsize1 >= m_dProtrusionMinSize[nCam] && dysize1 >= m_dProtrusionMinSize[nCam]) && (dxsize1 >= m_dWhiteSizeX[nCam] || dysize1 >= m_dWhiteSizeX[nCam]) &&nOrignWV1 >= m_nProtrusionTh[nCam]) 
									nret = 2002; //0608/*strbadtype = "반짝이";*/
							}
							if (m_bLineSizeAndOr) {
								if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1> m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) 
									nret = 1002; //0608strbadtype = "백색라인"; 
								else if (m_nLineValue[nCam] < nOrignWV1 &&(dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) 
									nret = 1002; //0608strbadtype = "백색라인"; 
							}
							else {//211013 최소사이즈
								if(m_nLineValue[nCam] < nOrignWV1){
									if ( ((dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) ||(dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) && m_nLineValue[nCam] < nOrignWV1) // && )//수직이나 수평인 경우 영역내면적이커짐  주위 흑색이 있으면 안됨.
									{
										{
											if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) 
												nret = 1002; //0608strbadtype = "백색라인";
											else if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) 
												nret = 1002; //0608strbadtype = "백색라인";
										}
									}
								}
							}
						}
					}
					else{
						nret = 4001;
					}
					if(nret>0){
						m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
						m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
						m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
						m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
						m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
						m_dISSize[nCam][nDefect] = nret;
						BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
						CRect rt22 = m_rtIsland[nCam][nDefect];
						if (!m_bDefectBinary)//0729-1
							ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						else
							ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					}
					nDefect++;
					if (nDefect > MAX_HUGEDEFECT - 2)break;
				}
				if(m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE){	// 220126 - sjj
					//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
					if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
					{
						if(nOrignWV1 > m_nScrabTh[nCam]){//Sacrab TH 보다 커야 스크랩이다.
							m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
							m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
							m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
							m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
							m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
							m_dISSize[nCam][nDefect] = 9001;
							BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
							CRect rt22 = m_rtIsland[nCam][nDefect];
							if (!m_bDefectBinary)//0729-1
								ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							else
								ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

							nDefect++;
						}
					}
				}
				else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
				{
					CPoint pt22;
					pt22.SetPoint(dcx12,dcy12);
					if(m_rtScrabIns[nCam].PtInRect(pt22)){
						//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
						if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
						{
							if(nOrignWV1 > m_nScrabTh[nCam]){//Sacrab TH 보다 커야 스크랩이다.
								m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
								m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
								m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
								m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam];
								m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam];
								m_dISSize[nCam][nDefect] = 9001;
								BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
								CRect rt22 = m_rtIsland[nCam][nDefect];
								if (!m_bDefectBinary)//0729-1
									ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								else
									ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

								nDefect++;
							}
						}
					}
				}
			

				if (nDefect > MAX_HUGEDEFECT - 2)break;
			}
		}
	}

//Dark Blob
	if(m_bNoncoatDarkInsp[nCam]){
		m_pChainIsland[nCam]->SetChainData(1, fmDarkInsp, 1, 1, 2, 100000, m_nWidth, m_nHeight);//white
		nBlobCount14 = m_pChainIsland[nCam]->FastChain(rt2.left, m_nCellMargin, rt2.right - 1, rt2.bottom - m_nCellMargin);//0926
		for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽  //0910
		{
			if (nDefect > MAX_HUGEDEFECT - 2)break;
			dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp14 < 10)continue;
			int nx1 = m_pChainIsland[nCam]->FindMaxX(i);//0625-1
			int nx2 = m_pChainIsland[nCam]->FindMinX(i);//0625-1
			int ny1 = m_pChainIsland[nCam]->FindMaxY(i);//0625-1
			int ny2 = m_pChainIsland[nCam]->FindMinY(i);//0625-1
			if (ny1 - ny2 <= 2)continue;//220214 //미코팅 Y Size 확
			double dx11 = (nx1-nx2+1)*m_dscale_x1[nCam];
			double dy11 = (ny1-ny2+1)*m_dscale_y1[nCam];
			double dcx12 = 0, dcy12 = 0;
			//if(dx11> m_dSpecScarabSize[nCam] && dy11> m_dSpecScarabSize[nCam] )
			if (dx11 > m_dScrabSpecSizeX[nCam] && dy11 > m_dScrabSpecSizeY[nCam])
			{
				m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = dx11;
				m_dISSizeY[nCam][nDefect] = dy11;
				m_dISSize[nCam][nDefect] = 4001;//다크미코팅 코드 확인필요.
				BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
				CRect rt22 = m_rtIsland[nCam][nDefect];
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

				nDefect++;

			}
		}
		nBlobCount14 = m_pChainIsland[nCam]->FastChain(rt1.left, m_nCellMargin, rt1.right - 1, rt2.bottom - m_nCellMargin);//0926
		for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽  //0910
		{
			if (nDefect > MAX_HUGEDEFECT - 2)break;
			dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp14 < 10)continue;
			int nx1 = m_pChainIsland[nCam]->FindMaxX(i);//0625-1
			int nx2 = m_pChainIsland[nCam]->FindMinX(i);//0625-1
			int ny1 = m_pChainIsland[nCam]->FindMaxY(i);//0625-1
			int ny2 = m_pChainIsland[nCam]->FindMinY(i);//0625-1
			if (ny1 - ny2 <= 2)continue;//220214 //미코팅 Y Size 확
			double dx11 = (nx1-nx2+1)*m_dscale_x1[nCam];
			double dy11 = (ny1-ny2+1)*m_dscale_y1[nCam];
			double dcx12 = 0, dcy12 = 0;
			//if(dx11> m_dSpecScarabSize[nCam] && dy11> m_dSpecScarabSize[nCam] )
			if (dx11 > m_dScrabSpecSizeX[nCam] && dy11 > m_dScrabSpecSizeY[nCam])
			{
				m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = dx11;
				m_dISSizeY[nCam][nDefect] = dy11;
				m_dISSize[nCam][nDefect] = 4001;//다크미코팅 코드 확인필요.
				BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
				CRect rt22 = m_rtIsland[nCam][nDefect];
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

				nDefect++;
			}
		}	
	}
//코팅 주름 검사 추가 //221116  //
	if(m_bCoatWrinkleUse[nCam]){//Bright Imamge를 사용한다. //평균 밝기만 구하고 판단은 나중에 한다.
		memset(fmDarkInsp, 0, m_nWidth*m_nHeight);

		CRect rtcw1;
		int nInspX = m_dCoatWrinkleAreamm[nCam]/m_dscale_x1[nCam];
		rtcw1.SetRect(dCoatEdge1[1]-nInspX,0,dCoatEdge1[1],nh);//dCoatEdge1[1]
		rtcw1 = SetBoundary2(rtcw1, CRect(80, 0, nw - 100, nh));//211017-1
		double dcwAve = 0,dc11=0;
		for(int y=rtcw1.top;y<rtcw1.bottom;y++){
			for(int x=rtcw1.left;x<rtcw1.right;x++){
				dcwAve += *(fmBright+y*m_nWidth+x);
				dc11++;
				if( *(fmBright+y*m_nWidth+x) - m_dMiddleAve[nCam] > m_nCoatWrinkleTh[nCam])
					*(fmDarkInsp+y*m_nWidth+x) = 255;
			}
		}
		if(dc11>0)
			dcwAve /= dc11;
		m_dCoatWrinkleAve[nCam] = dcwAve;
		if(m_dCoatWrinkleAve[nCam] - m_dMiddleAve[nCam] > m_nCoatWrinkleTh[nCam]){
			m_pChainIsland[nCam]->SetChainData(1, fmDarkInsp, 4, 4, 10, 1000000, m_nWidth, m_nHeight);//white
			nBlobCount14 = m_pChainIsland[nCam]->FastChain(rtcw1.left, rtcw1.top, rtcw1.right - 1, rtcw1.bottom);//0926
			double dmax1 = 0;
			int nMaxno1=0;
			for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽  //0910
			{
				dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
				if(dTemp14 > dmax1){
					dmax1 = dTemp14;nMaxno1 = i;
				}
			}
			int nx1 = m_pChainIsland[nCam]->FindMaxX(nMaxno1);//0625-1
			int nx2 = m_pChainIsland[nCam]->FindMinX(nMaxno1);//0625-1
			int ny1 = m_pChainIsland[nCam]->FindMaxY(nMaxno1);//0625-1
			int ny2 = m_pChainIsland[nCam]->FindMinY(nMaxno1);//0625-1
			double dx11 = (nx1-nx2+1)*m_dscale_x1[nCam];
			double dy11 = (ny1-ny2+1)*m_dscale_y1[nCam];
			double dcx12 = 0, dcy12 = 0;
			m_pChainIsland[nCam]->Chain_Center(nMaxno1, &dcx12, &dcy12);//0625-1
			m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
			m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
			m_dISSizeX[nCam][nDefect] = dx11;
			m_dISSizeY[nCam][nDefect] = dy11;
			m_dISSize[nCam][nDefect] = 1006;//코팅주름.
			BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
			CRect rt22 = m_rtIsland[nCam][nDefect];
			if (!m_bDefectBinary)//0729-1
				ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
			else
				ImageSelectCopy(fmNonCoat, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

			nDefect++;
		
		}
	}


	delete[]fmNonCoat;//0618
	delete [] fmDarkInsp;//220503
	m_nIslandCount[nCam] = nDefect;//0706
	return nDefect;
}
#else
int CInspect::FindNonCoating(int nCam, BYTE* fmBright, BYTE* fmDark, int nw, int nh, int nTh, CRect rtinsLeft, CRect rtinsRight, int nEdgeSum1, double* dCoatEdge1, int nt1, int nt2, int* ntabpos1) //수정된곳
{

	///////////////////////////////////////////////////////////////211017-1 영역 변수 수정으로 함수 교체
	//탭구간은 검사에서 Skip하도록 수정해야함. 
	int nIslandTh = m_nIslandTh[nCam], nw2 = BAD_WIDTH * 0.5, nh2 = BAD_HEIGHT * 0.5, njump1 = 0, nDefect = m_nIslandCount[nCam];//0930
	m_noncoatingOriginBV = 0; //220928SJM
	m_noncoatingRatio[0] = 0, m_noncoatingRatio[1] = 0;
	//220503 
	//BYTE* fmNonCoat = new BYTE[m_nWidth * m_nHeight];
	//BYTE* fmDarkInsp = new BYTE[m_nWidth * m_nHeight];

	double dStartTime = GetPrecisionTime();

	//int nEdgeSum3 = nEdgeSum1;//(nSumEdge1 + nSumEdge2)*0.5;
	//int nHightCut1 = nEdgeSum3 + 60;//0707 //0805
	//int nHightCut2 = nEdgeSum3 + 60;//0707 //0805
	//if (nHightCut1 > 240)nHightCut1 = 240;
	//if (nHightCut1 < 210)nHightCut1 = 240;
	//if (nHightCut2 > 240)nHightCut2 = 240;//211105
	//if (nHightCut2 < 210)nHightCut2 = 240;//211105

	BYTE* fmtmp = fmBright;
	double dMidAve = m_dMiddleAve[nCam];//m_dMiddleAveDark //220110

	if (m_bWhiteRoll[nCam] == TRUE) {//dark 이미지에서 찾는다.  //211028
		//nHightCut1 = nEdgeSum3 + 10;//211101
		//nHightCut2 = nEdgeSum3 + 30;//211101
		if (FALSE == m_bWhiteRoll_NoncoatingImageIndex[nCam])
		{
			fmtmp = fmDark;
			dMidAve = m_dMiddleAveDark[nCam];//220110
		}
	}
	//int nRightMargin = m_nNonCoatOffset[nCam];//64;//211024
	CRect rt1, rt2;//211017-1
	CRect rtImage(0, 0, nw-10, nh-10);
	rt1 = SetBoundary2(rtinsLeft, CRect(80, 0, nw - 100, nh));//211017-1
	rt2 = SetBoundary2(rtinsRight, CRect(80, 0, nw - 100, nh));//211017-1
	rt1.right = dCoatEdge1[0] + (m_dLeftNoncoatRange[nCam] / m_dscale_x1[nCam]);//220325
	rt1.left = dCoatEdge1[0] - m_nNonCoatOffset[nCam];
	rt2.right = dCoatEdge1[1] + m_nNonCoatOffset[nCam];//220325
	rt2.left = dCoatEdge1[1] - (m_dRightNoncoatRange[nCam] / m_dscale_x1[nCam]);//220325

	rt1 = SetBoundary2(rt1, CRect(80, 0, nw - 100, nh));////220325
	rt2 = SetBoundary2(rt2, CRect(80, 0, nw - 100, nh));////220325

	CRect rtNonCoatingLeftInspArea, rtNonCoatingRightInspArea;
	rtNonCoatingLeftInspArea = CRect(rt1.left, m_nCellMargin, rt1.right , rt1.bottom - m_nCellMargin);
	rtNonCoatingRightInspArea = CRect(rt2.left, m_nCellMargin, rt2.right , rt2.bottom - m_nCellMargin);
	rtNonCoatingLeftInspArea &= rtImage;
	rtNonCoatingRightInspArea &= rtImage;
	rt1 &= rtImage;
	rt2 &= rtImage;

	int nRt1SizeX = rt1.Width();
	int nRt1SizeY = rt1.Height();
	int nRt2SizeX = rt2.Width();
	int nRt2SizeY = rt2.Height();

	if (0 > nRt1SizeX || 0 > nRt1SizeY || 0 > nRt2SizeX || 0 > nRt2SizeY)
		return 0 ;

	if (m_fmLeftNonCoat[nCam] == NULL)
	{
		m_fmLeftNonCoat[nCam] = new BYTE[rt1.Width() * rt1.Height()];
		m_nLeftNonCoatBufferWidth[nCam] = rt1.Width();
		m_nLeftNonCoatBufferHeight[nCam] = rt1.Height();
	}

	if (m_fmRightNonCoat[nCam] == NULL)
	{
		m_fmRightNonCoat[nCam] = new BYTE[rt2.Width() * rt2.Height()];
		m_nRightNonCoatBufferWidth[nCam] = rt2.Width();
		m_nRightNonCoatBufferHeight[nCam] = rt2.Height();
	}

	if (m_fmLeftDarkInsp[nCam] == NULL)
		m_fmLeftDarkInsp[nCam] = new BYTE[rt1.Width() * rt1.Height()];

	if (m_fmRightDarkInsp[nCam] == NULL)
		m_fmRightDarkInsp[nCam] = new BYTE[rt2.Width() * rt2.Height()];

	if (m_nLeftNonCoatBufferWidth[nCam] != rt1.Width() || m_nLeftNonCoatBufferHeight[nCam] != rt1.Height())
	{
		delete[] m_fmLeftNonCoat[nCam];
		m_fmLeftNonCoat[nCam] = new BYTE[rt1.Width() * rt1.Height()];
		delete[] m_fmLeftDarkInsp[nCam];
		m_fmLeftDarkInsp[nCam] = new BYTE[rt1.Width() * rt1.Height()];

		m_nLeftNonCoatBufferWidth[nCam] = rt1.Width();
		m_nLeftNonCoatBufferHeight[nCam] = rt1.Height();
	}

	if (m_nRightNonCoatBufferWidth[nCam] != rt2.Width() || m_nRightNonCoatBufferHeight[nCam] != rt2.Height())
	{
		delete[] m_fmRightNonCoat[nCam];
		m_fmRightNonCoat[nCam] = new BYTE[rt2.Width() * rt2.Height()];
		delete[] m_fmLeftDarkInsp[nCam];
		m_fmLeftDarkInsp[nCam] = new BYTE[rt2.Width() * rt2.Height()];

		m_nRightNonCoatBufferWidth[nCam] = rt2.Width();
		m_nRightNonCoatBufferHeight[nCam] = rt2.Height();
	}

	memset(m_fmLeftNonCoat[nCam], 0, rt1.Width() * rt1.Height());
	memset(m_fmLeftDarkInsp[nCam], 0, rt1.Width() * rt1.Height());
	memset(m_fmRightNonCoat[nCam], 0, rt2.Width() * rt2.Height());
	memset(m_fmRightDarkInsp[nCam], 0, rt2.Width() * rt2.Height());

	m_dNewinspTime[nCam][7] = (GetPrecisionTime() - dStartTime) * 1000.0f; //이미 여기서 5 ~ 10ms 정도 소요함, 메모리 생성하는데 오래 걸리는 듯, 뒤에는 얼마 안 걸림

//0630 Skip 영역 찾기
	int ntaboffset1 = 130;//0805
	//if (m_nAnodeCathode == 0 && m_nCellSize == 1)//0805
	if (true == m_bLaserAndCathode[nCam])//221026 YYS
		ntaboffset1 = 200;
	int nXPos1 = dCoatEdge1[1] + ntaboffset1, nTabCnt1 = 0, nTabCnt2 = 0;//,ntabpos1[10]={0,};//0630 //0805
	int nXPos0 = dCoatEdge1[0] - ntaboffset1;
	if (nXPos0 < m_nMarginSearchStart[nCam]) nXPos0 = m_nMarginSearchStart[nCam];
	if (nXPos1 > m_nMarginSearchEnd[nCam]) nXPos1 = m_nMarginSearchEnd[nCam];
	int nLRrollAve[2] = { 0, };//0 Left Roll 1 Right Rolll
	for (int y = rt1.top; y < rt1.bottom; y++) {//0630
		//Roll 밝기체크로만 사용.
		if (y<100 || y>rt1.bottom - 101) {
			nLRrollAve[0] += *(fmtmp + y * m_nWidth + nXPos0);
			nLRrollAve[1] += *(fmtmp + y * m_nWidth + nXPos1);
		}
	}
	nLRrollAve[0] /= 200;
	nLRrollAve[1] /= 200;
	int nYSkip1 = m_nTabEdgeNoncoatOffset[nCam];
	//미코팅 Th를 따로 둔다.//롤을 과검하면 안된다.

	m_dNewinspTime[nCam][8] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	if (m_nNonCoatTh[nCam] == 0) {
		m_nNonCoatTh[nCam] = 60;
	}
	//if (m_nNonCoatTh[nCam] > 0) {//211106
	//	nHightCut1 = nHightCut2 = m_nNonCoatTh[nCam];//0을 입력하면 기존값을 쓰고 0보다 크면 설정값을 쓴다
	//}
	int nTh1[4] = { 0, };//0좌측 전극에지 1좌측전극부 2우측 전극부 3우측전극에지
	//다크 TH 설정 //220503
	int nThDark[2] = { 0, };//0좌측 좌측전극부 1우측전극에지
	double dtmp1 = dMidAve - nLRrollAve[0];//좌측 전극 밝기 - 좌측 롤밝기 -
	double dtmp2 = dMidAve - nLRrollAve[1];//우측 전극 밝기 - 우측 롤밝기 -  60-100 = -40  60-30=30
	if (dtmp1 > 20 && m_nNoncoatBlackTh[nCam] == 0)//최소 20 이상은 차이가 발생해야함. 에지부 Loading 꺼짐에 의한 과검 발생 가능.
		nThDark[0] = dtmp1 - 5;
	else nThDark[0] = m_nNoncoatBlackTh[nCam];
	if (dtmp2 > 20 && m_nNoncoatBlackTh[nCam] == 0)
		nThDark[1] = dtmp2 - 5;
	else nThDark[1] = m_nNoncoatBlackTh[nCam];
	if (nThDark[0] == 0) nThDark[0] = 20;
	if (nThDark[1] == 0) nThDark[1] = 20;
	if (!m_bNoncoatDarkInsp[nCam])
		nThDark[0] = nThDark[1] = 255;
	for (int k1 = 0; k1 < 4; k1++) {
		nTh1[k1] = dMidAve + m_nNonCoatTh[nCam];
	}
	if (dMidAve > nLRrollAve[0] && dMidAve > nLRrollAve[1]) {//좌/우 롤 모두 전극 밝기보다 작을 경우
	}
	else if (dMidAve > nLRrollAve[0] && dMidAve < nLRrollAve[1]) {//우측만 밝을 경우
		nTh1[3] = nLRrollAve[1] + m_nNonCoatTh[nCam];
	}
	else if (dMidAve < nLRrollAve[0] && dMidAve > nLRrollAve[1]) {//좌측만 밝을 경우
		nTh1[0] = nLRrollAve[0] + m_nNonCoatTh[nCam];
	}
	else if (dMidAve < nLRrollAve[0] && dMidAve < nLRrollAve[1]) {//둘다 밝을 경우
		nTh1[0] = nLRrollAve[0] + m_nNonCoatTh[nCam];
		nTh1[3] = nLRrollAve[1] + m_nNonCoatTh[nCam];
	}
	for (int k1 = 0; k1 < 4; k1++) {
		if (nTh1[k1] > 255)
			nTh1[k1] = 255;
	}

	m_dNewinspTime[nCam][9] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	//2가지 TH를 쓴다 롤이 밝을 경우 에지부는 TH 를 높이고 나머지는 어둡게 
	int ntbSkipPixel = 32;//64
	if (m_bTiltEdge[nCam]) 
	{//220403
		for (int y = rt1.top; y < rt1.bottom && y < 3000; y++)
		{
			for (int x = rt1.left, x1 = 0; x < rt1.right && x>10 && x < m_nWidth - 100; x++, x1++) 
			{
				//미코팅 영역은 Skip 한다.
				if (y > m_rtNotchArea[nCam][0].top && y<m_rtNotchArea[nCam][0].bottom && x>m_rtNotchArea[nCam][0].left && x < m_rtNotchArea[nCam][0].right)
					continue;
				if (y > m_rtNotchArea[nCam][1].top && y<m_rtNotchArea[nCam][1].bottom && x>m_rtNotchArea[nCam][1].left && x < m_rtNotchArea[nCam][1].right)
					continue;
				if (x < m_nLRedge[nCam][0][y] + 2) 
				{//m_nEdgeOffset1[nCam]){
					if (*(fmtmp + y * m_nWidth + x) > nTh1[0])
						*(m_fmLeftNonCoat[nCam] + y * m_nLeftNonCoatBufferWidth[nCam] + x1) = 255;
				}
				else 
				{
					if (*(fmtmp + y * m_nWidth + x) > nTh1[1])*(m_fmLeftNonCoat[nCam] + y * m_nLeftNonCoatBufferWidth[nCam] + x1) = 255;
					if (*(fmBright + y * m_nWidth + x) < dMidAve - nThDark[0])
						*(m_fmLeftDarkInsp[nCam] + y * m_nLeftNonCoatBufferWidth[nCam] + x1) = 255;//220503 전극 밝기보다 Th 보다 작은 값 이진화
				}
			}

			if (y > nt1 - ntbSkipPixel - nYSkip1 && y < nt2 + ntbSkipPixel + nYSkip1)continue;//0617//211008-1  //탭 영역 스킵

			for (int x = rt2.left, x1 = 0; x < rt2.right && x>10 && x < m_nWidth - 100; x++, x1++)
			{//LV 모델의 경우 어깨선이 0 //0706-2
				if (y > m_rtNotchArea[nCam][2].top && y<m_rtNotchArea[nCam][2].bottom && x>m_rtNotchArea[nCam][2].left && x < m_rtNotchArea[nCam][2].right)
					continue;
				if (y > m_rtNotchArea[nCam][3].top && y<m_rtNotchArea[nCam][3].bottom && x>m_rtNotchArea[nCam][3].left && x < m_rtNotchArea[nCam][3].right)
					continue;
				if (x < m_nLRedge[nCam][1][y] - 2) 
				{//m_nEdgeOffset1[nCam]){//220404
					if (*(fmtmp + y * m_nWidth + x) > nTh1[2])
						*(m_fmRightNonCoat[nCam] + y * m_nRightNonCoatBufferWidth[nCam] + x1) = 255;
					if (*(fmBright + y * m_nWidth + x) < dMidAve - nThDark[1])
						*(m_fmRightDarkInsp[nCam] + y * m_nRightNonCoatBufferWidth[nCam] + x1) = 255;//220503 전극 밝기보다 Th 보다 작은 값 이진화
				}
				else {
					if (*(fmtmp + y * m_nWidth + x) > nTh1[3])*(m_fmRightNonCoat[nCam] + y * m_nRightNonCoatBufferWidth[nCam] + x1) = 255;
				}
			}
		}
	}
	else 
	{
		for (int y = rt1.top; y < rt1.bottom; y++) 
		{
			for (int x = rt1.left, x1 = 0; x < rt1.right && x>10 && x < m_nWidth - 100; x++, x1++) 
			{
				if (y > m_rtNotchArea[nCam][0].top && y<m_rtNotchArea[nCam][0].bottom && x>m_rtNotchArea[nCam][0].left && x < m_rtNotchArea[nCam][0].right)
					continue;
				if (y > m_rtNotchArea[nCam][1].top && y<m_rtNotchArea[nCam][1].bottom && x>m_rtNotchArea[nCam][1].left && x < m_rtNotchArea[nCam][1].right)
					continue;
				if (x < m_rtCell[nCam].left + 2) 
				{//m_nEdgeOffset1[nCam]){
					if (*(fmtmp + y * m_nWidth + x) > nTh1[0])
						*(m_fmLeftNonCoat[nCam] + y * m_nLeftNonCoatBufferWidth[nCam] + x1) = 255;
				}
				else 
				{
					if (*(fmtmp + y * m_nWidth + x) > nTh1[1])
						*(m_fmLeftNonCoat[nCam] + y * m_nLeftNonCoatBufferWidth[nCam] + x1) = 255;
					if (*(fmtmp + y * m_nWidth + x) < dMidAve - nThDark[0])
						*(m_fmLeftDarkInsp[nCam] + y * m_nLeftNonCoatBufferWidth[nCam] + x1) = 255;//220503 전극 밝기보다 Th 보다 작은 값 이진화
				}

			}

			if (y > nt1 - ntbSkipPixel - nYSkip1 && y < nt2 + ntbSkipPixel + nYSkip1)continue;//0617//211008-1 //탭 영역 스킵
			
			for (int x = rt2.left, x1 = 0; x < rt2.right && x>10 && x < m_nWidth - 100; x++, x1++)
			{//LV 모델의 경우 어깨선이 0 //0706-2ㅜ
				if (y > m_rtNotchArea[nCam][2].top && y<m_rtNotchArea[nCam][2].bottom && x>m_rtNotchArea[nCam][2].left && x < m_rtNotchArea[nCam][2].right)
					continue;
				if (y > m_rtNotchArea[nCam][3].top && y<m_rtNotchArea[nCam][3].bottom && x>m_rtNotchArea[nCam][3].left && x < m_rtNotchArea[nCam][3].right)
					continue;
				if (x < m_rtCell[nCam].right - 2)//-m_nEdgeOffset1[nCam])
				{
					if (*(fmtmp + y * m_nWidth + x) > nTh1[2])
						*(m_fmRightNonCoat[nCam] + y * m_nRightNonCoatBufferWidth[nCam] + x1) = 255;
					if (*(fmtmp + y * m_nWidth + x) < dMidAve - nThDark[1])
						*(m_fmRightDarkInsp[nCam] + y * m_nRightNonCoatBufferWidth[nCam] + x1) = 255;//220503 전극 밝기보다 Th 보다 작은 값 이진화
				}
				else 
				{
					if (*(fmtmp + y * m_nWidth + x) > nTh1[3])
						*(m_fmRightNonCoat[nCam] + y * m_nRightNonCoatBufferWidth[nCam] + x1) = 255;
				}
			}
		}
	}

	m_dNewinspTime[nCam][10] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	//20220916 SJM
	if (m_bIstest)
	{
		memset(m_NoneCoatingImage, 0, sizeof(BYTE) * m_nWidth * m_nHeight);

		for(int y = rt1.top, y1 = 0; y < rt1.bottom; y++, y1++) memcpy(m_NoneCoatingImage + (y * m_nWidth) + rt1.left, m_fmLeftNonCoat[nCam] + (y1 * m_nLeftNonCoatBufferWidth[nCam]), sizeof(BYTE) * m_nLeftNonCoatBufferWidth[nCam]);
		for (int y = rt2.top, y1 = 0; y < rt2.bottom; y++, y1++) memcpy(m_NoneCoatingImage + (y * m_nWidth) + rt2.left, m_fmRightNonCoat[nCam] + (y1 * m_nRightNonCoatBufferWidth[nCam]), sizeof(BYTE) * m_nRightNonCoatBufferWidth[nCam]);
		m_rtBinaryNonCoating[nCam][0].SetRect(rtNonCoatingRightInspArea.left, rtNonCoatingRightInspArea.top, rtNonCoatingRightInspArea.right, nt1 - ntbSkipPixel - nYSkip1);
		m_rtBinaryNonCoating[nCam][1].SetRect(rtNonCoatingRightInspArea.left, nt2 + ntbSkipPixel + nYSkip1, rtNonCoatingRightInspArea.right, rtNonCoatingRightInspArea.bottom);
		m_nNoncoatingTh[nCam][0] = nTh1[0];
		m_nNoncoatingTh[nCam][1] = nTh1[1];
		m_nNoncoatingTh[nCam][2] = nTh1[2];
		m_nNoncoatingTh[nCam][3] = nTh1[3];
		m_nNoncoatingAve[nCam][0] = nLRrollAve[0];
		m_nNoncoatingAve[nCam][1] = dMidAve;
		m_nNoncoatingAve[nCam][2] = nLRrollAve[1];

	}
	//20220916 SJM
	//m_rtFullInspRect[nCam][0].SetRect(dCoatEdge1[0] - m_nNonCoatOffset[nCam], m_nCellMargin, rt1.right, rt1.bottom - m_nCellMargin);
	//m_rtFullInspRect[nCam][1].SetRect( rt2.left-32*6,0,dCoatEdge1[1] + m_nNonCoatOffset[nCam],rt1.bottom);
	//m_rtFullInspRect[nCam][1].SetRect(rt2.left, m_nCellMargin, dCoatEdge1[1] + m_nNonCoatOffset[nCam], rt1.bottom - m_nCellMargin);//220414
	m_rtFullInspRect[nCam][0] = rtNonCoatingLeftInspArea;
	m_rtFullInspRect[nCam][1] = rtNonCoatingRightInspArea;
	m_rtFullInspRect[nCam][2].SetRect(dCoatEdge1[0] + m_nEdgeOffset1[nCam], m_nCellMargin, dCoatEdge1[1] - m_nEdgeOffset1[nCam], rt1.bottom - m_nCellMargin);

	int nBlobCount14 = 0;
	if (m_bBottomNoncoatingUse[nCam])
	{
		//m_pChainNoncoating[nCam]->SetChainData(1, m_fmNonCoat[nCam], 1, 1, 2, 100000, m_nWidth, m_nHeight);//white
		m_pChainNoncoating[nCam]->SetChainData(1, m_fmLeftNonCoat[nCam], 1, 1, 2, 100000, m_nLeftNonCoatBufferWidth[nCam], m_nLeftNonCoatBufferHeight[nCam]);//white
		//nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(rtNonCoatingLeftInspArea.left, rtNonCoatingLeftInspArea.top, rtNonCoatingLeftInspArea.right, rtNonCoatingLeftInspArea.bottom);//0926
		nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(0, rtNonCoatingLeftInspArea.top, rtNonCoatingLeftInspArea.Width(), rtNonCoatingLeftInspArea.bottom);//0926
	}
	double dTemp14 = 0, dBlobSize14 = 0; int nBlobNo14 = 0;
	int nedgedefect = 0;
	int nDefecCnt1 = 0, nmaxidx = 0;
	int nDefecCnt = 0, nMaxblob1 = 0, nMaxblob = 0;

	m_nLeftNoneCoatingBlobCnt[nCam] = 0;		// rt1, 전극 좌측

	for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 왼쪽//0910
	{
		if (nDefect > MAX_HUGEDEFECT - 2)break;
		dTemp14 = m_pChainNoncoating[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp14 < 4)continue;
		int nx1 = m_pChainNoncoating[nCam]->FindMaxX(i);//0625-1
		int nx2 = m_pChainNoncoating[nCam]->FindMinX(i);//0625-1
		int ny1 = m_pChainNoncoating[nCam]->FindMaxY(i);//0625-1
		int ny2 = m_pChainNoncoating[nCam]->FindMinY(i);//0625-1
		if (ny1 - ny2 <= 3)continue;//0625-1
		double dcx12 = 0, dcy12 = 0;
		m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
		CPoint pt14;
		pt14.SetPoint(dcx12, dcy12);
		if (m_rtNotchArea[nCam][0].PtInRect(pt14)) {
			continue;
		}
		if (m_rtNotchArea[nCam][1].PtInRect(pt14)) {
			continue;
		}
		if (nMaxblob < dTemp14)nMaxblob1 = i;

		if (m_bNonCoatingSizeAndOr && (nx1 - nx2 + 1) * m_dscale_x1[nCam] > m_dNoneCoatingSizeX[nCam] && (ny1 - ny2 + 1) * m_dscale_y1[nCam] > m_dNoneCoatingSizeY[nCam])//0825
		{
			nmaxidx = i; nDefecCnt1 = 100; break;
		}
		else if (!m_bNonCoatingSizeAndOr && (nx1 - nx2 + 1) * m_dscale_x1[nCam] > m_dNoneCoatingSizeX[nCam] || (ny1 - ny2 + 1) * m_dscale_x1[nCam] > m_dNoneCoatingSizeY[nCam])//0825
		{
			nmaxidx = i; nDefecCnt1 = 100; break;
		}
		else if ((ny1 - ny2 + 1) * m_dscale_y1[nCam] > m_dNoneCoatingSizeY[nCam])
			nDefecCnt1++;

	}

	m_dNewinspTime[nCam][11] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	if (nDefecCnt1 >= 100)
	{
		m_nLeftNoneCoatingBlobCnt[nCam] = 1;
	}
	else
	{
		m_nLeftNoneCoatingBlobCnt[nCam] = nDefecCnt1;
	}

	int nSpecCnt = m_nNoneCoatingCount[nCam];
	if (nDefecCnt1 > nSpecCnt)
	{
		if (nDefecCnt1 >= 100) 
		{
			for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽 //211118 여러개 불량 잡게 수정
			{
				if (nDefect > MAX_HUGEDEFECT - 2)break;
				nmaxidx = i;
				dTemp14 = m_pChainNoncoating[nCam]->Chain_Area(nmaxidx);//FChain_Area(i, &dTemp);//			ttt
				if (dTemp14 < 4)continue;
				int nx1 = m_pChainNoncoating[nCam]->FindMaxX(nmaxidx);//0625-1
				int nx2 = m_pChainNoncoating[nCam]->FindMinX(nmaxidx);//0625-1
				int ny1 = m_pChainNoncoating[nCam]->FindMaxY(nmaxidx);//0625-1
				int ny2 = m_pChainNoncoating[nCam]->FindMinY(nmaxidx);//0625-1
				double dxs = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				double dys = (ny1 - ny2 + 1) * m_dscale_y1[nCam];

				double dcx12 = 0, dcy12 = 0;
				m_pChainNoncoating[nCam]->Chain_Center(nmaxidx, &dcx12, &dcy12);//0625-1
				dcx12 += rt1.left;
				CPoint pt14, pt15;//220211
				pt14.SetPoint(dcx12, dcy12);
				if (m_rtNotchArea[nCam][0].PtInRect(pt14)) {
					pt15.SetPoint(nx1, (ny2 + ny1) / 2);
					if (m_rtNotchArea[nCam][0].PtInRect(pt15)) {//우측에지 기준으로 다시한번 체크한다. //220211
						continue;
					}
				}
				if (m_rtNotchArea[nCam][1].PtInRect(pt14)) {
					pt15.SetPoint(nx1, (ny2 + ny1) / 2);
					if (m_rtNotchArea[nCam][1].PtInRect(pt15)) {//우측에지 기준으로 다시한번 체크한다. //220211
						continue;
					}

				}
				CRect rtEdgecheck;//220516
				rtEdgecheck.SetRect(nx2, ny2, nx1, ny1);
				int nEdgeMargin1 = m_nNonCoatingEdgeMargin[nCam];
				double dxsize1 = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				double dysize1 = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				int nPinholeCut = m_nPinholeCut[nCam];
				int nOrignWV1 = 0, nret = 0;
				double dratio[4] = { 0, };// x:y 비율 라인 찾기용
				if (dxsize1 > 0)dratio[0] = dysize1 / dxsize1;//white Y/X
				if (dxsize1 > 0)dratio[1] = dysize1 / dxsize1;//Black Y/X
				if (dysize1 > 0.5)dratio[2] = dxsize1 / dysize1;//White X/Y
				if (dysize1 > 0.5)dratio[3] = dxsize1 / dysize1;//Black X/y
				double dDistRatio1 = 0, dDistRatio2 = 0, dDistRatio3 = 0, dDistRatio4 = 0;//220426
				double dmindist = 0, dmaxdist = 0, davedist = 0, dmindist1 = 0, dmaxdist1 = 0, davedist1 = 0;//211008-1
				dcx12 -= rt1.left;
				m_pChainNoncoating[nCam]->FineDistFromPoint(i, dcx12, dcy12, &dmindist1, &dmaxdist1, &davedist1);//장축 단축 구하기  //211008
				dcx12 += rt1.left;
				if (dmaxdist1 > 0)dDistRatio4 = dmindist1 / dmaxdist1;//211008-1
				if (dmaxdist > 0)dDistRatio1 = dmindist / dmaxdist;//211008-1
				if (dmindist > 0)dDistRatio2 = dmaxdist / dmindist;//211008-1
				dDistRatio3 = dDistRatio1;////211008-1
				double dSpecDistRatio = 3;//.5;//장단축이 3배 이상이면 라인으로 분류//211008-1
				double dlongLen1 = dmindist;
				double dlongLen2 = 16;
				for (int y0 = rtEdgecheck.top; y0 < rtEdgecheck.bottom; y0++) {
					for (int x0 = rtEdgecheck.left; x0 < rtEdgecheck.right; x0++) {
						//최고 밝기 찾기
						if (*(m_fmLeftNonCoat[nCam] + y0 * m_nLeftNonCoatBufferWidth[nCam] + x0) > nOrignWV1 && *(m_fmLeftNonCoat[nCam] + y0 * m_nLeftNonCoatBufferWidth[nCam] + x0 + 1) > nOrignWV1)  nOrignWV1 = *(m_fmLeftNonCoat[nCam] + y0 * m_nLeftNonCoatBufferWidth[nCam] + x0);
					}
				}
				nOrignWV1 -= m_dMiddleAve[nCam];//중앙값
				m_noncoatingRatio[0] = dratio[0], m_noncoatingRatio[1] = dratio[2];
				m_noncoatingOriginBV = nOrignWV1;

				CString strType;
				if ((m_bNonCoatingSizeAndOr && dxs > m_dNoneCoatingSizeX[nCam] && dys > m_dNoneCoatingSizeY[nCam])
					|| (!m_bNonCoatingSizeAndOr && (dxs >= m_dNonCoatMinSize[nCam] && dys >= m_dNonCoatMinSize[nCam]) && (dxs > m_dNoneCoatingSizeX[nCam] || dys > m_dNoneCoatingSizeY[nCam])))
				{//211013 최소사이즈
					//에지에 붙어있는지 확인
					if (m_bNoncoatEdgeCheck[nCam]) {
						if (rtEdgecheck.left + rt1.left <= m_rtCell[nCam].left + nEdgeMargin1) {
							nret = 4001;//미코팅
						}
						else {	//핀홀 라인스펙으로 검사
							//오리진 밝기 찾기 추가
							//종횡비 추가
							if (m_bPinholeAndOr) {//////and 조건    핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 //211006
								if ((dxsize1 >= m_dPinholeSizeX[nCam] && dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
									nret = 2001; //0608/*strbadtype = "핀홀";*/ 
								else if( (dxsize1 >= m_dWhiteSizeX[nCam] && dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam]) 
								{// m_dPeakData[nCam][nIdx][nDefect][1][0]-m_nTargetBrightness[nCam] > m_nTh1[nCam]) {//220216 
									nret = 2002; //0608/*strbadtype = "반짝이";*/
								}
								
							}
							else {//211013 최소사이즈
								if ((dxsize1 >= m_dPinholeMinSize[nCam] && dysize1 >= m_dPinholeMinSize[nCam]) && (dxsize1 >= m_dPinholeSizeX[nCam] || dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
									nret = 2001; //0608/*strbadtype = "핀홀";*/
								else if( (dxsize1 >= m_dProtrusionMinSize[nCam] && dysize1 >= m_dProtrusionMinSize[nCam]) && (dxsize1 >= m_dWhiteSizeX[nCam] || dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam])
								{
									nret = 2002; //0608/*strbadtype = "반짝이";*/
								}
								
							}
							if (m_bLineSizeAndOr) {
								if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam]))
									nret = 1002; //0608strbadtype = "백색라인"; 
								else if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15)
									nret = 1002; //0608strbadtype = "백색라인"; 
							}
							else {//211013 최소사이즈
								if (m_nLineValue[nCam] < nOrignWV1) {
									if (((dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) || (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) && m_nLineValue[nCam] < nOrignWV1) // && )//수직이나 수평인 경우 영역내면적이커짐  주위 흑색이 있으면 안됨.
									{
										{
											if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam]))
												nret = 1002; //0608strbadtype = "백색라인";
											else if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15)
												nret = 1002; //0608strbadtype = "백색라인";
										}
									}
								}
							}
						}
					}
					else {
						nret = 4001;
					}
					if (nret > 0) {
						m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
						m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
						m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
						m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
						m_dISSize[nCam][nDefect] = nret;//4001;
						BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
						CRect rt22 = m_rtIsland[nCam][nDefect];
						rt22.left -= rt1.left;
						rt22.right -= rt1.left;
						rt22 &= rtImage;
						if (!m_bDefectBinary)//0729-1
							ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						else
							ImageSelectCopy(m_fmLeftNonCoat[nCam], fmdefect1, m_nLeftNonCoatBufferWidth[nCam], m_nLeftNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						nDefect++;
					}
				}
				if (nDefect > MAX_HUGEDEFECT - 2)break;
				if (m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE) {	// 220126 - sjj
					//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
					if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
					{
						if (nOrignWV1 > m_nScrabTh[nCam]) {//Sacrab TH 보다 커야 스크랩이다.
							m_pChainNoncoating[nCam]->Chain_Center(nmaxidx, &dcx12, &dcy12);//0625-1
							dcx12 += rt1.left;
							m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
							m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
							m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
							m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
							m_dISSize[nCam][nDefect] = 9001;
							BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
							CRect rt22 = m_rtIsland[nCam][nDefect];
							rt22.left -= rt1.left;
							rt22.right -= rt1.left;
							rt22 &= rtImage;
							if (!m_bDefectBinary)//0729-1
								ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							else
								ImageSelectCopy(m_fmLeftNonCoat[nCam], fmdefect1, m_nLeftNonCoatBufferWidth[nCam], m_nLeftNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							nDefect++;
						}
					}
				}
				else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
				{
					CPoint pt22;
					pt22.SetPoint(dcx12, dcy12);
					if (m_rtScrabIns[nCam].PtInRect(pt22)) {
						//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
						if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
						{
							if (nOrignWV1 > m_nScrabTh[nCam]) {//Sacrab TH 보다 커야 스크랩이다.
								m_pChainNoncoating[nCam]->Chain_Center(nmaxidx, &dcx12, &dcy12);//0625-1
								dcx12 += rt1.left;
								m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
								m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
								m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
								m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
								m_dISSize[nCam][nDefect] = 9001;
								BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
								CRect rt22 = m_rtIsland[nCam][nDefect];
								rt22.left -= rt1.left;
								rt22.right -= rt1.left;
								rt22 &= rtImage;
								if (!m_bDefectBinary)//0729-1
									ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								else
									ImageSelectCopy(m_fmLeftNonCoat[nCam], fmdefect1, m_nLeftNonCoatBufferWidth[nCam], m_nLeftNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								nDefect++;
							}
						}
					}
				}

				if (nDefect > MAX_HUGEDEFECT - 2)break;
			}
		}
		else {//개수에서 만족
			for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 왼쪽
			{
				if (nDefect > MAX_HUGEDEFECT - 2)break;
				dTemp14 = m_pChainNoncoating[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
				if (dTemp14 < 4)continue;
				int nx1 = m_pChainNoncoating[nCam]->FindMaxX(i);//0625-1
				int nx2 = m_pChainNoncoating[nCam]->FindMinX(i);//0625-1
				int ny1 = m_pChainNoncoating[nCam]->FindMaxY(i);//0625-1
				int ny2 = m_pChainNoncoating[nCam]->FindMinY(i);//0625-1
				if (ny1 - ny2 <= 3)continue;//0625-1
				double dxs = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				double dys = (ny1 - ny2 + 1) * m_dscale_y1[nCam];

				double dcx12 = 0, dcy12 = 0;
				m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				dcx12 += rt1.left;
				CPoint pt14, pt15;
				pt14.SetPoint(dcx12, dcy12);
				if (m_rtNotchArea[nCam][0].PtInRect(pt14)) {
					pt15.SetPoint(nx1, (ny2 + ny1) / 2);
					if (m_rtNotchArea[nCam][0].PtInRect(pt15)) {//우측에지 기준으로 다시한번 체크한다. //220211
						continue;
					}
				}
				if (m_rtNotchArea[nCam][1].PtInRect(pt14)) {
					pt15.SetPoint(nx1, (ny2 + ny1) / 2);
					if (m_rtNotchArea[nCam][1].PtInRect(pt15)) {//우측에지 기준으로 다시한번 체크한다. //220211
						continue;
					}
				}
				CRect rtEdgecheck;//220516
				rtEdgecheck.SetRect(nx2, ny2, nx1, ny1);
				int nEdgeMargin1 = m_nNonCoatingEdgeMargin[nCam];
				double dxsize1 = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				double dysize1 = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				int nPinholeCut = m_nPinholeCut[nCam];
				int nOrignWV1 = 0, nret = 0;
				double dratio[4] = { 0, };// x:y 비율 라인 찾기용
				if (dxsize1 > 0)dratio[0] = dysize1 / dxsize1;//white Y/X
				if (dxsize1 > 0)dratio[1] = dysize1 / dxsize1;//Black Y/X
				if (dysize1 > 0.5)dratio[2] = dxsize1 / dysize1;//White X/Y
				if (dysize1 > 0.5)dratio[3] = dxsize1 / dysize1;//Black X/y
				double dDistRatio1 = 0, dDistRatio2 = 0, dDistRatio3 = 0, dDistRatio4 = 0;//220426
				double dmindist = 0, dmaxdist = 0, davedist = 0, dmindist1 = 0, dmaxdist1 = 0, davedist1 = 0;//211008-1
				dcx12 -= rt1.left;
				m_pChainNoncoating[nCam]->FineDistFromPoint(i, dcx12, dcy12, &dmindist1, &dmaxdist1, &davedist1);//장축 단축 구하기  //211008
				dcx12 += rt1.left;
				if (dmaxdist1 > 0)dDistRatio4 = dmindist1 / dmaxdist1;//211008-1
				if (dmaxdist > 0)dDistRatio1 = dmindist / dmaxdist;//211008-1
				if (dmindist > 0)dDistRatio2 = dmaxdist / dmindist;//211008-1
				dDistRatio3 = dDistRatio1;////211008-1
				double dSpecDistRatio = 3;//.5;//장단축이 3배 이상이면 라인으로 분류//211008-1
				double dlongLen1 = dmindist;
				double dlongLen2 = 16;
				for (int y0 = rtEdgecheck.top; y0 < rtEdgecheck.bottom; y0++) {
					for (int x0 = rtEdgecheck.left; x0 < rtEdgecheck.right; x0++) {
						//최고 밝기 찾기
						if (*(m_fmLeftNonCoat[nCam] + y0 * m_nLeftNonCoatBufferWidth[nCam] + x0) > nOrignWV1)  nOrignWV1 = *(m_fmLeftNonCoat[nCam] + y0 * m_nLeftNonCoatBufferWidth[nCam] + x0);
					}
				}
				nOrignWV1 -= m_dMiddleAve[nCam];//중앙값
				m_noncoatingRatio[0] = dratio[0], m_noncoatingRatio[1] = dratio[2];
				m_noncoatingOriginBV = nOrignWV1;
				if ((m_bNonCoatingSizeAndOr && dxs > m_dNoneCoatingSizeX[nCam] && dys > m_dNoneCoatingSizeY[nCam])
					|| (!m_bNonCoatingSizeAndOr && (dxs >= m_dNonCoatMinSize[nCam] && dys >= m_dNonCoatMinSize[nCam]) && (dxs > m_dNoneCoatingSizeX[nCam] || dys > m_dNoneCoatingSizeY[nCam])))
				{//211013 최소사이즈

					if (m_bNoncoatEdgeCheck[nCam]) {
						if (rtEdgecheck.left + rt1.left <= m_rtCell[nCam].left + nEdgeMargin1) {
							nret = 4001;//미코팅
						}
						else {	//핀홀 라인스펙으로 검사
							//오리진 밝기 찾기 추가
							//종횡비 추가
							if (m_bPinholeAndOr) {//////and 조건    핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 //211006
								if ((dxsize1 >= m_dPinholeSizeX[nCam] && dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
									nret = 2001; //0608/*strbadtype = "핀홀";*/ 
								else if ((dxsize1 >= m_dWhiteSizeX[nCam] && dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam])
									nret = 2002; //0608/*strbadtype = "반짝이";*/
							}
							else {//211013 최소사이즈
								if ((dxsize1 >= m_dPinholeMinSize[nCam] && dysize1 >= m_dPinholeMinSize[nCam]) && (dxsize1 >= m_dPinholeSizeX[nCam] || dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
									nret = 2001; //0608/*strbadtype = "핀홀";*/
								else if ((dxsize1 >= m_dProtrusionMinSize[nCam] && dysize1 >= m_dProtrusionMinSize[nCam]) && (dxsize1 >= m_dWhiteSizeX[nCam] || dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam])
									nret = 2002; //0608/*strbadtype = "반짝이";*/
							}
							if (m_bLineSizeAndOr) {
								if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam]))
									nret = 1002; //0608strbadtype = "백색라인"; 
								else if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15)
									nret = 1002; //0608strbadtype = "백색라인"; 
							}
							else {//211013 최소사이즈
								if (m_nLineValue[nCam] < nOrignWV1) {
									if (((dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) || (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) && m_nLineValue[nCam] < nOrignWV1) // && )//수직이나 수평인 경우 영역내면적이커짐  주위 흑색이 있으면 안됨.
									{
										{
											if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam]))
												nret = 1002; //0608strbadtype = "백색라인";
											else if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15)
												nret = 1002; //0608strbadtype = "백색라인";
										}
									}
								}
							}
						}
					}
					else {
						nret = 4001;
					}
					if (nret > 0) {
						m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
						dcx12 += rt1.left;
						m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
						m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
						m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
						m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
						m_dISSize[nCam][nDefect] = nret;
						BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
						CRect rt22 = m_rtIsland[nCam][nDefect];
						rt22.left -= rt1.left;
						rt22.right -= rt1.left;
						rt22 &= rtImage;
						if (!m_bDefectBinary)//0729-1
							ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						else
							ImageSelectCopy(m_fmLeftNonCoat[nCam], fmdefect1, m_nLeftNonCoatBufferWidth[nCam], m_nLeftNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						nDefect++;
					}
				}
				if (nDefect > MAX_HUGEDEFECT - 2)break;
				if (m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE) {		// 220126 - sjj
					//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
					if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
					{
						if (nOrignWV1 > m_nScrabTh[nCam]) {//Sacrab TH 보다 커야 스크랩이다.
							m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
							dcx12 += rt1.left;
							m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
							m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
							m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
							m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
							m_dISSize[nCam][nDefect] = 9001;
							BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
							CRect rt22 = m_rtIsland[nCam][nDefect];
							rt22.left -= rt1.left;
							rt22.right -= rt1.left;
							rt22 &= rtImage;
							if (!m_bDefectBinary)//0729-1
								ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							else
								ImageSelectCopy(m_fmLeftNonCoat[nCam], fmdefect1, m_nLeftNonCoatBufferWidth[nCam], m_nLeftNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							nDefect++;
						}
					}
				}
				else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
				{
					CPoint pt22;
					pt22.SetPoint(dcx12, dcy12);
					if (m_rtScrabIns[nCam].PtInRect(pt22)) {
						//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
						if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
						{
							if (nOrignWV1 > m_nScrabTh[nCam]) {//Sacrab TH 보다 커야 스크랩이다.
								m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
								dcx12 += rt1.left;
								m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
								m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
								m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
								m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
								m_dISSize[nCam][nDefect] = 9001;
								BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
								CRect rt22 = m_rtIsland[nCam][nDefect];
								rt22.left -= rt1.left;
								rt22.right -= rt1.left;
								rt22 &= rtImage;
								if (!m_bDefectBinary)//0729-1
									ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								else
									ImageSelectCopy(m_fmLeftNonCoat[nCam], fmdefect1, m_nLeftNonCoatBufferWidth[nCam], m_nLeftNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								nDefect++;
							}
						}
					}
				}
				if (nDefect > MAX_HUGEDEFECT - 2)break;
			}
		}
	}

	m_dNewinspTime[nCam][12] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	nBlobCount14 = 0;
	if (m_bTabNoncoatingUse[nCam]) 
	{
		//m_pChainNoncoating[nCam]->SetChainData(1, m_fmNonCoat[nCam], 1, 1, 2, 100000, m_nWidth, m_nHeight);//white
		m_pChainNoncoating[nCam]->SetChainData(1, m_fmRightNonCoat[nCam], 1, 1, 2, 100000, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam]);//white
		//nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(rtNonCoatingRightInspArea.left, rtNonCoatingRightInspArea.top, rtNonCoatingRightInspArea.right, rtNonCoatingRightInspArea.bottom);//0926
		nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(0, rtNonCoatingRightInspArea.top, rtNonCoatingRightInspArea.Width(), rtNonCoatingRightInspArea.bottom);//0926
	}
	dTemp14 = 0, dBlobSize14 = 0; nBlobNo14 = 0;
	nDefecCnt = 0, nMaxblob1 = 0, nMaxblob = 0;

	m_nRightNoneCoatingBlobCnt[nCam] = 0;		// rt2, 전극 우측

	for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽  //0910
	{
		if (nDefect > MAX_HUGEDEFECT - 2)break;
		dTemp14 = m_pChainNoncoating[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp14 < 4)continue;
		int nx1 = m_pChainNoncoating[nCam]->FindMaxX(i);//0625-1
		int nx2 = m_pChainNoncoating[nCam]->FindMinX(i);//0625-1
		int ny1 = m_pChainNoncoating[nCam]->FindMaxY(i);//0625-1
		int ny2 = m_pChainNoncoating[nCam]->FindMinY(i);//0625-1
		if (ny1 - ny2 <= 2)continue;//220214 //미코팅 Y Size 확
		double dcx12 = 0, dcy12 = 0;
		m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
		CPoint pt14;
		pt14.SetPoint(dcx12, dcy12);
		if (m_rtNotchArea[nCam][2].PtInRect(pt14)) {
			continue;
		}
		if (m_rtNotchArea[nCam][3].PtInRect(pt14)) {
			continue;
		}

		if (nMaxblob < dTemp14)nMaxblob1 = i;
		if (m_bNonCoatingSizeAndOr && ((nx1 - nx2 + 1) * m_dscale_x1[nCam] > m_dNoneCoatingSizeX[nCam] && (ny1 - ny2 + 1) * m_dscale_y1[nCam] > m_dNoneCoatingSizeY[nCam]))//1006
		{
			nDefecCnt = 100; break;
		}
		else if (!m_bNonCoatingSizeAndOr && ((nx1 - nx2 + 1) * m_dscale_x1[nCam] > m_dNoneCoatingSizeX[nCam] || (ny1 - ny2 + 1) * m_dscale_y1[nCam] > m_dNoneCoatingSizeY[nCam]))//1006
		{
			nDefecCnt = 100; break;
		}
		else if ((ny1 - ny2 + 1) * m_dscale_y1[nCam] > m_dNoneCoatingSizeY[nCam] || (nx1 - nx2 + 1) * m_dscale_x1[nCam] > m_dNoneCoatingSizeX[nCam])	// 220214 미코팅 사이즈 확인
			nDefecCnt++;
	}

	m_dNewinspTime[nCam][13] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	if (nDefecCnt >= 100)
	{
		m_nRightNoneCoatingBlobCnt[nCam] = 1;
	}
	else
	{
		m_nRightNoneCoatingBlobCnt[nCam] = nDefecCnt;
	}

	if (nDefecCnt < nSpecCnt || nDefecCnt == 0)
		;
	else if (nSpecCnt > 0 && nDefecCnt > 0 && nDefecCnt >= nSpecCnt && nDefecCnt < 100) 
	{//1006
		if (nDefect < MAX_HUGEDEFECT - 1)
		{
			int nx1 = m_pChainNoncoating[nCam]->FindMaxX(nMaxblob1);//0625-1
			int nx2 = m_pChainNoncoating[nCam]->FindMinX(nMaxblob1);//0625-1
			int ny1 = m_pChainNoncoating[nCam]->FindMaxY(nMaxblob1);//0625-1
			int ny2 = m_pChainNoncoating[nCam]->FindMinY(nMaxblob1);//0625-1

			double dcx12 = 0, dcy12 = 0;
			m_pChainNoncoating[nCam]->Chain_Center(nMaxblob1, &dcx12, &dcy12);//0625-1
			dcx12 += rt2.left;
			CPoint pt14, pt15;//220211
			pt14.SetPoint(dcx12, dcy12);
			int nJump = 0;
			if (m_rtNotchArea[nCam][2].PtInRect(pt14)) {
				pt15.SetPoint(nx2, (ny2 + ny1) / 2);
				if (m_rtNotchArea[nCam][2].PtInRect(pt14)) {//좌측에지 기준으로 다시한번 체크한다. //220211
					nJump = 1;
				}
			}
			if (m_rtNotchArea[nCam][3].PtInRect(pt14)) {
				pt15.SetPoint(nx2, (ny2 + ny1) / 2);
				if (m_rtNotchArea[nCam][3].PtInRect(pt14)) {//좌측에지 기준으로 다시한번 체크한다. //220211
					nJump = 1;
				}
			}
			if (nJump == 0) {
				CRect rtEdgecheck;//220516
				rtEdgecheck.SetRect(nx2, ny2, nx1, ny1);
				int nEdgeMargin1 = m_nNonCoatingEdgeMargin[nCam];
				double dxsize1 = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				double dysize1 = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				int nPinholeCut = m_nPinholeCut[nCam];
				int nOrignWV1 = 0, nret = 0;
				double dratio[4] = { 0, };// x:y 비율 라인 찾기용
				if (dxsize1 > 0)dratio[0] = dysize1 / dxsize1;//white Y/X
				if (dxsize1 > 0)dratio[1] = dysize1 / dxsize1;//Black Y/X
				if (dysize1 > 0.5)dratio[2] = dxsize1 / dysize1;//White X/Y
				if (dysize1 > 0.5)dratio[3] = dxsize1 / dysize1;//Black X/y
				double dDistRatio1 = 0, dDistRatio2 = 0, dDistRatio3 = 0, dDistRatio4 = 0;//220426
				double dmindist = 0, dmaxdist = 0, davedist = 0, dmindist1 = 0, dmaxdist1 = 0, davedist1 = 0;//211008-1
				dcx12 -= rt2.left;
				m_pChainNoncoating[nCam]->FineDistFromPoint(nMaxblob1, dcx12, dcy12, &dmindist1, &dmaxdist1, &davedist1);//장축 단축 구하기  //211008
				dcx12 += rt2.left;
				if (dmaxdist1 > 0)dDistRatio4 = dmindist1 / dmaxdist1;//211008-1
				if (dmaxdist > 0)dDistRatio1 = dmindist / dmaxdist;//211008-1
				if (dmindist > 0)dDistRatio2 = dmaxdist / dmindist;//211008-1
				dDistRatio3 = dDistRatio1;////211008-1
				double dSpecDistRatio = 3;//.5;//장단축이 3배 이상이면 라인으로 분류//211008-1
				double dlongLen1 = dmindist;
				double dlongLen2 = 16;
				for (int y0 = rtEdgecheck.top; y0 < rtEdgecheck.bottom; y0++) {
					for (int x0 = rtEdgecheck.left; x0 < rtEdgecheck.right; x0++) {
						//최고 밝기 찾기
						if (*(m_fmRightNonCoat[nCam] + y0 * m_nRightNonCoatBufferWidth[nCam] + x0) > nOrignWV1)  nOrignWV1 = *(m_fmRightNonCoat[nCam] + y0 * m_nRightNonCoatBufferWidth[nCam] + x0);
					}
				}
				nOrignWV1 -= m_dMiddleAve[nCam];//중앙값

				if (m_bNoncoatEdgeCheck[nCam]) 
				{
					if (rtEdgecheck.right + rt2.left >= m_rtCell[nCam].right - nEdgeMargin1) 
					{
						//if (m_bNonCoatingSizeAndOr)
						//{
						//	if(dxsize1 > m_dNoneCoatingSizeX[nCam] && dysize1 > m_dNoneCoatingSizeY[nCam])
						//		nret = 4001;//미코팅
						//}
						//else
						//{
						//	if (dxsize1 > m_dNoneCoatingSizeX[nCam] || dysize1 > m_dNoneCoatingSizeY[nCam])
						//		nret = 4001;//미코팅
						//}
						int ny1 = m_pChainNoncoating[nCam]->FindMaxY(nBlobCount14 - 1);//0625-1
						int ny2 = m_pChainNoncoating[nCam]->FindMinY(0);//0625-1

						nret = 4001;//미코팅
					}
					else 
					{	//핀홀 라인스펙으로 검사
						//오리진 밝기 찾기 추가
						//종횡비 추가
						if (m_bPinholeAndOr) 
						{//////and 조건    핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 //211006
							if ((dxsize1 >= m_dPinholeSizeX[nCam] && dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
								nret = 2001; //0608/*strbadtype = "핀홀";*/ 
							else if ((dxsize1 >= m_dWhiteSizeX[nCam] && dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam]) {// m_dPeakData[nCam][nIdx][nDefect][1][0]-m_nTargetBrightness[nCam] > m_nTh1[nCam]) {//220216 
								nret = 2002; //0608/*strbadtype = "반짝이";*/
							}//2111123
						}
						else 
						{//211013 최소사이즈
							if ((dxsize1 >= m_dPinholeMinSize[nCam] && dysize1 >= m_dPinholeMinSize[nCam]) && (dxsize1 >= m_dPinholeSizeX[nCam] || dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
								nret = 2001; //0608/*strbadtype = "핀홀";*/
							else if ((dxsize1 >= m_dProtrusionMinSize[nCam] && dysize1 >= m_dProtrusionMinSize[nCam]) && (dxsize1 >= m_dWhiteSizeX[nCam] || dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam])
								nret = 2002; //0608/*strbadtype = "반짝이";*/
						}
						if (m_bLineSizeAndOr) 
						{
							if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam]))
								nret = 1002; //0608strbadtype = "백색라인"; 
							else if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15)
								nret = 1002; //0608strbadtype = "백색라인"; 
						}
						else 
						{//211013 최소사이즈
							if (m_nLineValue[nCam] < nOrignWV1) {
								if (((dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) || (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) && m_nLineValue[nCam] < nOrignWV1) // && )//수직이나 수평인 경우 영역내면적이커짐  주위 흑색이 있으면 안됨.
								{
									{
										if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam]))
											nret = 1002; //0608strbadtype = "백색라인";
										else if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15)
											nret = 1002; //0608strbadtype = "백색라인";
									}
								}
							}
						}
					}
				}
				else 
				{
					//if (m_bNonCoatingSizeAndOr)
					//{
					//	if (dxsize1 > m_dNoneCoatingSizeX[nCam] && dysize1 > m_dNoneCoatingSizeY[nCam])
					//		nret = 4001;//미코팅
					//}
					//else
					//{
					//	if (dxsize1 > m_dNoneCoatingSizeX[nCam] || dysize1 > m_dNoneCoatingSizeY[nCam])
					//		nret = 4001;//미코팅
					//}
					nret = 4001;//미코팅
				}

				if (nret > 0) {
					m_pChainNoncoating[nCam]->Chain_Center(nMaxblob1, &dcx12, &dcy12);//0625-1
					dcx12 += rt2.left;
					m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
					m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
					m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
					m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
					m_dISSize[nCam][nDefect] = nret;
					BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
					CRect rt22 = m_rtIsland[nCam][nDefect];
					rt22.left -= rt2.left;
					rt22.right -= rt2.left;
					rt22 &= rtImage;
					if (!m_bDefectBinary)//0729-1
						ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					else
						ImageSelectCopy(m_fmRightNonCoat[nCam], fmdefect1, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nDefect++;
				}
				if (nDefect > MAX_HUGEDEFECT - 2) {
					//delete[]fmNonCoat;//0618
					//delete[] fmDarkInsp;
					m_nIslandCount[nCam] = nDefect;//0706
					return nDefect;
				}
				double dxs = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				double dys = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				if (m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE) {	// 220126 - sjj
					//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
					if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
					{
						if (nOrignWV1 > m_nScrabTh[nCam]) {//Sacrab TH 보다 커야 스크랩이다.
							m_pChainNoncoating[nCam]->Chain_Center(nmaxidx, &dcx12, &dcy12);//0625-1
							dcx12 += rt2.left;
							m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
							m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
							m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
							m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
							m_dISSize[nCam][nDefect] = 9001;
							BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
							CRect rt22 = m_rtIsland[nCam][nDefect];
							rt22.left -= rt2.left;
							rt22.right -= rt2.left;
							rt22 &= rtImage;
							if (!m_bDefectBinary)//0729-1
								ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							else
								ImageSelectCopy(m_fmRightNonCoat[nCam], fmdefect1, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

							nDefect++;
						}
					}
				}
				else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
				{
					CPoint pt22;
					pt22.SetPoint(dcx12, dcy12);
					if (m_rtScrabIns[nCam].PtInRect(pt22)) {
						//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
						if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
						{
							if (nOrignWV1 > m_nScrabTh[nCam]) {//Sacrab TH 보다 커야 스크랩이다.
								m_pChainNoncoating[nCam]->Chain_Center(nmaxidx, &dcx12, &dcy12);//0625-1
								dcx12 += rt2.left;
								m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
								m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
								m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
								m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
								m_dISSize[nCam][nDefect] = 9001;
								BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
								CRect rt22 = m_rtIsland[nCam][nDefect];
								rt22.left -= rt2.left;
								rt22.right -= rt2.left;
								rt22 &= rtImage;
								if (!m_bDefectBinary)//0729-1
									ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								else
									ImageSelectCopy(m_fmRightNonCoat[nCam], fmdefect1, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

								nDefect++;
							}
						}
					}
				}

			}
			if (nDefect > MAX_HUGEDEFECT - 2) {
				//delete[]fmNonCoat;//0618
				//delete[] fmDarkInsp;//220503
				m_nIslandCount[nCam] = nDefect;//0706
				return nDefect;
			}
		}

		//delete[]fmNonCoat;//0618
		//delete[] fmDarkInsp;//220503
		m_nIslandCount[nCam] = nDefect;//0706
		return nDefect;
	}
	else {
		int nYbad = 0;
		for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽
		{
			if (nDefect > MAX_HUGEDEFECT - 2)break;
			dTemp14 = m_pChainNoncoating[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp14 < 4)continue;
			int nx1 = m_pChainNoncoating[nCam]->FindMaxX(i);//0625-1
			int nx2 = m_pChainNoncoating[nCam]->FindMinX(i);//0625-1
			int ny1 = m_pChainNoncoating[nCam]->FindMaxY(i);//0625-1
			int ny2 = m_pChainNoncoating[nCam]->FindMinY(i);//0625-1
			if (ny1 - ny2 <= 3)continue;//0706
			double dcx12 = 0, dcy12 = 0;
			m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
			dcx12 += rt2.left;
			CPoint pt14, pt15;//220211
			pt14.SetPoint(dcx12, dcy12);//센터가 포함되게 되어있음.
			if (m_rtNotchArea[nCam][2].PtInRect(pt14)) {
				pt15.SetPoint(nx2, (ny2 + ny1) / 2);
				if (m_rtNotchArea[nCam][2].PtInRect(pt14)) {//좌측에지 기준으로 다시한번 체크한다. //220211
					continue;
				}
			}
			if (m_rtNotchArea[nCam][3].PtInRect(pt14)) {
				pt15.SetPoint(nx2, (ny2 + ny1) / 2);
				if (m_rtNotchArea[nCam][3].PtInRect(pt14)) {//좌측에지 기준으로 다시한번 체크한다. //220211
					continue;
				}
			}

			double dxs = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
			double dys = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
			CRect rtEdgecheck;//220516
			rtEdgecheck.SetRect(nx2, ny2, nx1, ny1);
			int nEdgeMargin1 = m_nNonCoatingEdgeMargin[nCam];
			double dxsize1 = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
			double dysize1 = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
			int nPinholeCut = m_nPinholeCut[nCam];
			int nOrignWV1 = 0, nret = 0;
			double dratio[4] = { 0, };// x:y 비율 라인 찾기용
			if (dxsize1 > 0)dratio[0] = dysize1 / dxsize1;//white Y/X
			if (dxsize1 > 0)dratio[1] = dysize1 / dxsize1;//Black Y/X
			if (dysize1 > 0.5)dratio[2] = dxsize1 / dysize1;//White X/Y
			if (dysize1 > 0.5)dratio[3] = dxsize1 / dysize1;//Black X/y
			double dDistRatio1 = 0, dDistRatio2 = 0, dDistRatio3 = 0, dDistRatio4 = 0;//220426
			double dmindist = 0, dmaxdist = 0, davedist = 0, dmindist1 = 0, dmaxdist1 = 0, davedist1 = 0;//211008-1
			dcx12 -= rt2.left;
			m_pChainNoncoating[nCam]->FineDistFromPoint(i, dcx12, dcy12, &dmindist1, &dmaxdist1, &davedist1);//장축 단축 구하기  //211008
			dcx12 += rt2.left;
			if (dmaxdist1 > 0)dDistRatio4 = dmindist1 / dmaxdist1;//211008-1
			if (dmaxdist > 0)dDistRatio1 = dmindist / dmaxdist;//211008-1
			if (dmindist > 0)dDistRatio2 = dmaxdist / dmindist;//211008-1
			dDistRatio3 = dDistRatio1;////211008-1
			double dSpecDistRatio = 3;//.5;//장단축이 3배 이상이면 라인으로 분류//211008-1
			double dlongLen1 = dmindist;
			double dlongLen2 = 16;
			for (int y0 = rtEdgecheck.top; y0 < rtEdgecheck.bottom; y0++) {
				for (int x0 = rtEdgecheck.left; x0 < rtEdgecheck.right; x0++) {
					//최고 밝기 찾기
					if (*(m_fmRightNonCoat[nCam] + y0 * m_nRightNonCoatBufferWidth[nCam] + x0) > nOrignWV1 && *(m_fmRightNonCoat[nCam] + y0 * m_nRightNonCoatBufferWidth[nCam] + x0 + 1) > nOrignWV1)  nOrignWV1 = *(m_fmRightNonCoat[nCam] + y0 * m_nRightNonCoatBufferWidth[nCam] + x0);
				}
			}
			nOrignWV1 -= m_dMiddleAve[nCam];//중앙값
			if ((m_bNonCoatingSizeAndOr && (dxs > m_dNoneCoatingSizeX[nCam] && dys > m_dNoneCoatingSizeY[nCam]))
				|| (!m_bNonCoatingSizeAndOr && (dxs >= m_dNonCoatMinSize[nCam] && dys >= m_dNonCoatMinSize[nCam]) && (dxs > m_dNoneCoatingSizeX[nCam] || dys > m_dNoneCoatingSizeY[nCam])))
			{//211013 최소사이즈

				if (m_bNoncoatEdgeCheck[nCam]) {
					if (rtEdgecheck.right + rt2.left >= m_rtCell[nCam].right - nEdgeMargin1) {
						nret = 4001;//미코팅
					}
					else {	//핀홀 라인스펙으로 검사
						//오리진 밝기 찾기 추가
						//종횡비 추가
						if (m_bPinholeAndOr) {//////and 조건    핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 //211006
							if ((dxsize1 >= m_dPinholeSizeX[nCam] && dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
								nret = 2001; //0608/*strbadtype = "핀홀";*/ 
							else if ((dxsize1 >= m_dWhiteSizeX[nCam] && dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam]) {// m_dPeakData[nCam][nIdx][nDefect][1][0]-m_nTargetBrightness[nCam] > m_nTh1[nCam]) {//220216 
								nret = 2002; //0608/*strbadtype = "반짝이";*/
							}//2111123
						}
						else {//211013 최소사이즈
							if ((dxsize1 >= m_dPinholeMinSize[nCam] && dysize1 >= m_dPinholeMinSize[nCam]) && (dxsize1 >= m_dPinholeSizeX[nCam] || dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
								nret = 2001; //0608/*strbadtype = "핀홀";*/
							else if ((dxsize1 >= m_dProtrusionMinSize[nCam] && dysize1 >= m_dProtrusionMinSize[nCam]) && (dxsize1 >= m_dWhiteSizeX[nCam] || dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam])
								nret = 2002; //0608/*strbadtype = "반짝이";*/
						}
						if (m_bLineSizeAndOr) {
							if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam]))
								nret = 1002; //0608strbadtype = "백색라인"; 
							else if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15)
								nret = 1002; //0608strbadtype = "백색라인"; 
						}
						else {//211013 최소사이즈
							if (m_nLineValue[nCam] < nOrignWV1) {
								if (((dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) || (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) && m_nLineValue[nCam] < nOrignWV1) // && )//수직이나 수평인 경우 영역내면적이커짐  주위 흑색이 있으면 안됨.
								{
									{
										if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam]))
											nret = 1002; //0608strbadtype = "백색라인";
										else if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15)
											nret = 1002; //0608strbadtype = "백색라인";
									}
								}
							}
						}
					}
				}
				else {
					nret = 4001;
				}
				if (nret > 0) {
					m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
					dcx12 += rt2.left;
					m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
					m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
					m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
					m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
					m_dISSize[nCam][nDefect] = nret;
					BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
					CRect rt22 = m_rtIsland[nCam][nDefect];
					rt22.left -= rt2.left;
					rt22.right -= rt2.left;
					rt22 &= rtImage;
					if (!m_bDefectBinary)//0729-1
						ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					else
						ImageSelectCopy(m_fmRightNonCoat[nCam], fmdefect1, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					nDefect++;
				}
				if (nDefect > MAX_HUGEDEFECT - 2)break;
				if (m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE) {	// 220126 - sjj
					//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
					if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
					{
						if (nOrignWV1 > m_nScrabTh[nCam]) {//Sacrab TH 보다 커야 스크랩이다.
							m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
							dcx12 += rt2.left;
							m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
							m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
							m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
							m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
							m_dISSize[nCam][nDefect] = 9001;
							BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
							CRect rt22 = m_rtIsland[nCam][nDefect];
							rt22.left -= rt2.left;
							rt22.right -= rt2.left;
							rt22 &= rtImage;
							if (!m_bDefectBinary)//0729-1
								ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							else
								ImageSelectCopy(m_fmRightNonCoat[nCam], fmdefect1, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

							nDefect++;
						}
					}
				}
				else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
				{
					CPoint pt22;
					pt22.SetPoint(dcx12, dcy12);
					if (m_rtScrabIns[nCam].PtInRect(pt22)) {
						//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
						if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
						{
							if (nOrignWV1 > m_nScrabTh[nCam]) {//Sacrab TH 보다 커야 스크랩이다.
								m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
								dcx12 += rt2.left;
								m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
								m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
								m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
								m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
								m_dISSize[nCam][nDefect] = 9001;
								BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
								CRect rt22 = m_rtIsland[nCam][nDefect];
								rt22.left -= rt2.left;
								rt22.right -= rt2.left;
								if (!m_bDefectBinary)//0729-1
									ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								else
									ImageSelectCopy(m_fmRightNonCoat[nCam], fmdefect1, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								nDefect++;
							}
						}
					}
				}
				if (nDefect > MAX_HUGEDEFECT - 2)break;

			}
			else {//211025
				if (m_bStep[nCam]) {
					if (dys > m_dNoneCoatingSizeY[nCam]) {
						nYbad++;
					}
				}
				if (nYbad > 1) {
					if (m_bNoncoatEdgeCheck[nCam]) {
						if (rtEdgecheck.right + rt2.left >= m_rtCell[nCam].right - nEdgeMargin1) {
							nret = 4001;//미코팅
						}
						else {	//핀홀 라인스펙으로 검사
							//오리진 밝기 찾기 추가
							//종횡비 추가
							if (m_bPinholeAndOr) {//////and 조건    핀홀 라인 and / or 조건   TRUE = and 조건  FALSE = or 조건 //211006
								if ((dxsize1 >= m_dPinholeSizeX[nCam] && dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
									nret = 2001; //0608/*strbadtype = "핀홀";*/ 
								else if ((dxsize1 >= m_dWhiteSizeX[nCam] && dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam]) {// m_dPeakData[nCam][nIdx][nDefect][1][0]-m_nTargetBrightness[nCam] > m_nTh1[nCam]) {//220216 
									nret = 2002; //0608/*strbadtype = "반짝이";*/
								}//2111123
							}
							else {//211013 최소사이즈
								if ((dxsize1 >= m_dPinholeMinSize[nCam] && dysize1 >= m_dPinholeMinSize[nCam]) && (dxsize1 >= m_dPinholeSizeX[nCam] || dysize1 >= m_dPinholeSizeY[nCam]) && nOrignWV1 >= nPinholeCut)
									nret = 2001; //0608/*strbadtype = "핀홀";*/
								else if ((dxsize1 >= m_dProtrusionMinSize[nCam] && dysize1 >= m_dProtrusionMinSize[nCam]) && (dxsize1 >= m_dWhiteSizeX[nCam] || dysize1 >= m_dWhiteSizeX[nCam]) && nOrignWV1 >= m_nProtrusionTh[nCam])
									nret = 2002; //0608/*strbadtype = "반짝이";*/
							}
							if (m_bLineSizeAndOr) {
								if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam]))
									nret = 1002; //0608strbadtype = "백색라인"; 
								else if (m_nLineValue[nCam] < nOrignWV1 && (dxsize1 > m_dWhiteLineSizeX[nCam] && dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15)
									nret = 1002; //0608strbadtype = "백색라인"; 
							}
							else {//211013 최소사이즈
								if (m_nLineValue[nCam] < nOrignWV1) {
									if (((dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15) || (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam])) && m_nLineValue[nCam] < nOrignWV1) // && )//수직이나 수평인 경우 영역내면적이커짐  주위 흑색이 있으면 안됨.
									{
										{
											if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && (dratio[0] > m_dXYRatio[nCam] || dratio[2] > m_dXYRatio[nCam]))
												nret = 1002; //0608strbadtype = "백색라인";
											else if ((dxsize1 >= m_dLineMinSize[nCam] && dysize1 >= m_dLineMinSize[nCam]) && (dxsize1 > m_dWhiteLineSizeX[nCam] || dysize1 > m_dWhiteLineSizeY[nCam]) && dDistRatio3 > dSpecDistRatio && dlongLen1 >= 15)
												nret = 1002; //0608strbadtype = "백색라인";
										}
									}
								}
							}
						}
					}
					else {
						nret = 4001;
					}
					if (nret > 0) {
						m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
						dcx12 += rt2.left;
						m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
						m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
						m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
						m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
						m_dISSize[nCam][nDefect] = nret;
						BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
						CRect rt22 = m_rtIsland[nCam][nDefect];
						rt22.left -= rt2.left;
						rt22.right -= rt2.left;
						rt22 &= rtImage;
						if (!m_bDefectBinary)//0729-1
							ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
						else
							ImageSelectCopy(m_fmRightNonCoat[nCam], fmdefect1, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					}
					nDefect++;
					if (nDefect > MAX_HUGEDEFECT - 2)break;
				}
				if (m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE) {	// 220126 - sjj
					//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
					if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
					{
						if (nOrignWV1 > m_nScrabTh[nCam]) {//Sacrab TH 보다 커야 스크랩이다.
							m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
							dcx12 += rt2.left;
							m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
							m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
							m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
							m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
							m_dISSize[nCam][nDefect] = 9001;
							BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
							CRect rt22 = m_rtIsland[nCam][nDefect];
							rt22.left -= rt2.left;
							rt22.right -= rt2.left;
							rt22 &= rtImage;
							if (!m_bDefectBinary)//0729-1
								ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
							else
								ImageSelectCopy(m_fmRightNonCoat[nCam], fmdefect1, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

							nDefect++;
						}
					}
				}
				else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
				{
					CPoint pt22;
					pt22.SetPoint(dcx12, dcy12);
					if (m_rtScrabIns[nCam].PtInRect(pt22)) {
						//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
						if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
						{
							if (nOrignWV1 > m_nScrabTh[nCam]) {//Sacrab TH 보다 커야 스크랩이다.
								m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
								dcx12 += rt2.left;
								m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
								m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
								m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
								m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
								m_dISSize[nCam][nDefect] = 9001;
								BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
								CRect rt22 = m_rtIsland[nCam][nDefect];
								rt22.left -= rt2.left;
								rt22.right -= rt2.left;
								rt22 &= rtImage;
								if (!m_bDefectBinary)//0729-1
									ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
								else
									ImageSelectCopy(m_fmRightNonCoat[nCam], fmdefect1, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

								nDefect++;
							}
						}
					}
				}


				if (nDefect > MAX_HUGEDEFECT - 2)break;
			}
		}
	}

	m_dNewinspTime[nCam][14] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	//Dark Blob
	if (m_bNoncoatDarkInsp[nCam]) 
	{
		//m_pChainNoncoating[nCam]->SetChainData(1, m_fmDarkInsp[nCam], 1, 1, 2, 100000, m_nWidth, m_nHeight);//white
		m_pChainNoncoating[nCam]->SetChainData(1, m_fmRightDarkInsp[nCam], 1, 1, 2, 100000, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam]);//white
		//nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(rtNonCoatingRightInspArea.left, rtNonCoatingRightInspArea.top, rtNonCoatingRightInspArea.right, rtNonCoatingRightInspArea.bottom);//0926
		nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(0, rtNonCoatingRightInspArea.top, rtNonCoatingRightInspArea.Width(), rtNonCoatingRightInspArea.bottom);//0926

		for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽  //0910
		{
			if (nDefect > MAX_HUGEDEFECT - 2)break;
			dTemp14 = m_pChainNoncoating[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp14 < 10)continue;
			int nx1 = m_pChainNoncoating[nCam]->FindMaxX(i);//0625-1
			int nx2 = m_pChainNoncoating[nCam]->FindMinX(i);//0625-1
			int ny1 = m_pChainNoncoating[nCam]->FindMaxY(i);//0625-1
			int ny2 = m_pChainNoncoating[nCam]->FindMinY(i);//0625-1
			if (ny1 - ny2 <= 2)continue;//220214 //미코팅 Y Size 확
			double dx11 = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
			double dy11 = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
			double dcx12 = 0, dcy12 = 0;
			//if(dx11> m_dSpecScarabSize[nCam] && dy11> m_dSpecScarabSize[nCam] )
			if (dx11 > m_dScrabSpecSizeX[nCam] && dy11 > m_dScrabSpecSizeY[nCam])
			{
				m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				dcx12 += rt2.left;
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = dx11;
				m_dISSizeY[nCam][nDefect] = dy11;
				m_dISSize[nCam][nDefect] = 4001;//다크미코팅 코드 확인필요.
				BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
				CRect rt22 = m_rtIsland[nCam][nDefect];
				rt22.left -= rt2.left;
				rt22.right -= rt2.left;
				rt22 &= rtImage;
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(m_fmRightNonCoat[nCam], fmdefect1, m_nRightNonCoatBufferWidth[nCam], m_nRightNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

				nDefect++;

			}
		}

		m_pChainNoncoating[nCam]->SetChainData(1, m_fmLeftDarkInsp[nCam], 1, 1, 2, 100000, m_nLeftNonCoatBufferWidth[nCam], m_nLeftNonCoatBufferHeight[nCam]);//white
		//nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(rtNonCoatingLeftInspArea.left, rtNonCoatingLeftInspArea.top, rtNonCoatingLeftInspArea.right, rtNonCoatingLeftInspArea.bottom);//0926
		nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(0, rtNonCoatingLeftInspArea.top, rtNonCoatingLeftInspArea.Width(), rtNonCoatingLeftInspArea.bottom);//0926

		for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽  //0910
		{
			if (nDefect > MAX_HUGEDEFECT - 2)break;
			dTemp14 = m_pChainNoncoating[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp14 < 10)continue;
			int nx1 = m_pChainNoncoating[nCam]->FindMaxX(i);//0625-1
			int nx2 = m_pChainNoncoating[nCam]->FindMinX(i);//0625-1
			int ny1 = m_pChainNoncoating[nCam]->FindMaxY(i);//0625-1
			int ny2 = m_pChainNoncoating[nCam]->FindMinY(i);//0625-1
			if (ny1 - ny2 <= 2)continue;//220214 //미코팅 Y Size 확
			double dx11 = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
			double dy11 = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
			double dcx12 = 0, dcy12 = 0;
			//if(dx11> m_dSpecScarabSize[nCam] && dy11> m_dSpecScarabSize[nCam] )
			if (dx11 > m_dScrabSpecSizeX[nCam] && dy11 > m_dScrabSpecSizeY[nCam])
			{
				m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				dcx12 += rt1.left;
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = dx11;
				m_dISSizeY[nCam][nDefect] = dy11;
				m_dISSize[nCam][nDefect] = 4001;//다크미코팅 코드 확인필요.
				BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
				CRect rt22 = m_rtIsland[nCam][nDefect];
				rt22.left -= rt1.left;
				rt22.right -= rt1.left;
				rt22 &= rtImage;
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(m_fmLeftNonCoat[nCam], fmdefect1, m_nLeftNonCoatBufferWidth[nCam], m_nLeftNonCoatBufferHeight[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

				nDefect++;
			}
		}
	}

	m_dNewinspTime[nCam][15] = (GetPrecisionTime() - dStartTime) * 1000.0f;
//코팅 주름 검사 추가 //221116  //
	if(m_bCoatWrinkleUse[nCam])
	{//Bright Imamge를 사용한다. //평균 밝기만 구하고 판단은 나중에 한다.
		CRect rtcw1;
		int nInspX = m_dCoatWrinkleAreamm[nCam]/m_dscale_x1[nCam];
		rtcw1.SetRect(dCoatEdge1[1]-nInspX,0,dCoatEdge1[1],nh);//dCoatEdge1[1]
		rtcw1 = SetBoundary2(rtcw1, CRect(80, 0, nw - 100, nh));//211017-1

		BYTE* CoatWrinkleImage = new BYTE[rtcw1.Width() * rtcw1.Height()];
		memset(CoatWrinkleImage, 0, rtcw1.Width() * rtcw1.Height());
		double dcwAve = 0,dc11=0;

		for(int y = rtcw1.top; y < rtcw1.bottom; y++)
		{
			for(int x = rtcw1.left, x1 = 0; x < rtcw1.right; x++, x1++)
			{
				dcwAve += *(fmBright + y * m_nWidth + x);
				dc11++;
				if( *(fmBright + y * m_nWidth + x) - m_dMiddleAve[nCam] > m_nCoatWrinkleTh[nCam])
					*(CoatWrinkleImage + y * rtcw1.Width() + x1) = 255;
			}
		}
		if(dc11>0)
			dcwAve /= dc11;

		m_dCoatWrinkleAve[nCam] = dcwAve;

		if(m_dCoatWrinkleAve[nCam] - m_dMiddleAve[nCam] > m_nCoatWrinkleTh[nCam])
		{
			m_pChainIsland[nCam]->SetChainData(1, CoatWrinkleImage, 4, 4, 10, 1000000, rtcw1.Width(), rtcw1.Height());//white
			//nBlobCount14 = m_pChainIsland[nCam]->FastChain(rtcw1.left, rtcw1.top, rtcw1.right - 1, rtcw1.bottom);//0926
			nBlobCount14 = m_pChainIsland[nCam]->FastChain(0, rtcw1.top, rtcw1.Width() - 1, rtcw1.bottom);//0926
			double dmax1 = 0;
			int nMaxno1=0;

			for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽  //0910
			{
				dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
				if(dTemp14 > dmax1)
				{
					dmax1 = dTemp14;nMaxno1 = i;
				}
			}

			int nx1 = m_pChainIsland[nCam]->FindMaxX(nMaxno1);//0625-1
			int nx2 = m_pChainIsland[nCam]->FindMinX(nMaxno1);//0625-1
			int ny1 = m_pChainIsland[nCam]->FindMaxY(nMaxno1);//0625-1
			int ny2 = m_pChainIsland[nCam]->FindMinY(nMaxno1);//0625-1
			double dx11 = (nx1-nx2+1)*m_dscale_x1[nCam];
			double dy11 = (ny1-ny2+1)*m_dscale_y1[nCam];
			double dcx12 = 0, dcy12 = 0;
			m_pChainIsland[nCam]->Chain_Center(nMaxno1, &dcx12, &dcy12);//0625-1
			dcx12 += rtcw1.left;
			m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
			m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
			m_dISSizeX[nCam][nDefect] = dx11;
			m_dISSizeY[nCam][nDefect] = dy11;
			m_dISSize[nCam][nDefect] = 1006;//코팅주름.
			BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
			CRect rt22 = m_rtIsland[nCam][nDefect];
			rt22.left -= rtcw1.left;
			rt22.right -= rtcw1.left;
			rt22 &= rtImage;
			if (!m_bDefectBinary)//0729-1
				ImageSelectCopy(fmBright, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
			else
				ImageSelectCopy(CoatWrinkleImage, fmdefect1, rtcw1.Width(), rtcw1.Height(), rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

			nDefect++;
		
		}
		delete[] CoatWrinkleImage;
	}

	m_dNewinspTime[nCam][16] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	//delete[]fmNonCoat;//0618
	//delete[] fmDarkInsp;//220503
	m_nIslandCount[nCam] = nDefect;//0706
	return nDefect;
}
#endif

int CInspect::FindBurr(int nCam, BYTE * fmBright, BYTE* fmDark, int nw, int nh, CRect rttab, double dtabAverage)
{
	int nDefect = m_nIslandCount[nCam];//0;

	CRect rtBurr[2], rtBurr1[2], rtBurr2[2];//탭 영역 위 아래로 스캔하면서 백->흑->백 조건을 찾는다.//0613
	int nw2 = BAD_WIDTH*0.5, nh2 = BAD_HEIGHT*0.5;
	rtBurr[0].SetRect(rttab.left + 40, rttab.top - 30, rttab.right - 64, rttab.top + 5);//0531
	rtBurr[1].SetRect(rttab.left + 40, rttab.bottom - 5, rttab.right - 64, rttab.bottom + 30);
	rtBurr[0] = SetBoundary2(rtBurr[0], CRect(100, 100, nw - 100, nh - 100));
	rtBurr[1] = SetBoundary2(rtBurr[1], CRect(100, 100, nw - 100, nh - 100));
	//0611
	rtBurr1[0].SetRect(m_rtCellTab[nCam].left + 30, m_rtCellTab[nCam].top - 4, m_rtCellTab[nCam].right - 24, m_rtCellTab[nCam].top - 3);//0612
	rtBurr1[1].SetRect(m_rtCellTab[nCam].left + 30, m_rtCellTab[nCam].bottom + 3, m_rtCellTab[nCam].right - 24, m_rtCellTab[nCam].bottom + 4);//0612
	rtBurr1[0] = SetBoundary2(rtBurr1[0], CRect(100, 100, nw - 100, nh - 100));
	rtBurr1[1] = SetBoundary2(rtBurr1[1], CRect(100, 100, nw - 100, nh - 100));
	rtBurr2[0].SetRect(m_rtCellTab[nCam].left + 50, m_rtCellTab[nCam].top - 20, m_rtCellTab[nCam].right - 24, m_rtCellTab[nCam].top - 5);//0611
	rtBurr2[1].SetRect(m_rtCellTab[nCam].left + 50, m_rtCellTab[nCam].bottom + 5, m_rtCellTab[nCam].right - 24, m_rtCellTab[nCam].bottom + 20);//0611
	rtBurr2[0] = SetBoundary2(rtBurr2[0], CRect(100, 100, nw - 100, nh - 100));
	rtBurr2[1] = SetBoundary2(rtBurr2[1], CRect(100, 100, nw - 100, nh - 100));
	int checkWBW[20000] = { 0, }, checkWBW1[20000] = { 0, }, nTabTh = dtabAverage *0.6, nFindburr = 0, nTabTh1 = dtabAverage *0.65;//0.95;//0613
	for (int x = rtBurr[0].left; x<rtBurr[0].right; x++){
		for (int y = rtBurr[0].bottom; y>rtBurr[0].top; y--){
			if (checkWBW[x] == 0 && *(fmBright + nw*y + x) < nTabTh)checkWBW[x] = 1;
			else if (checkWBW[x] == 1 && *(fmBright + nw*y + x) > nTabTh1 && *(fmBright + nw*(y - 1) + x) > nTabTh1){
				checkWBW[x] = 2; nFindburr = 1;
			}
		}
	}
	int nBurrSize = 0, nburrcx = 0;	//0531
	if (nFindburr == 1){
		for (int x = rtBurr[0].left; x<rtBurr[0].right; x++){
			if (checkWBW[x] == 2 && checkWBW[x + 1] == 2){
				nBurrSize++; nburrcx += x;
			}
		}
	}
	// 	if(nBurrSize>2 && nDefect<MAX_HUGEDEFECT){
	// 		CRect rt2 ;
	// 		nburrcx /= nBurrSize;
	// 		rt2.SetRect(nburrcx-nw2,m_rtCellTab[nCam].top-nh2,nburrcx+nw2,m_rtCellTab[nCam].top+nh2);
	// 		rt2 = SetBoundary2(rt2,CRect(100,100,nw-100,nh-100));
	// 		m_rtIsland[nCam][nDefect] = rt2;
	// 		BYTE * fmdefect = m_pIsland[nCam][nDefect];
	// 		ImageSelectCopy(fm,fmdefect,nw,nh,rt2,BAD_WIDTH,BAD_HEIGHT,0,0);
	// 		m_dISSizeX[nCam][nDefect] =  (nBurrSize)*m_dscale_x1[nCam];
	// 		m_dISSizeY[nCam][nDefect] =  (2)*m_dscale_x1[nCam];
	// 		m_dISSize[nCam][nDefect] =  -1000;
	// 		nDefect++;
	// 	}
	//처음은 white 그다음 white는 burr
	int nBurrCheck1 = -1, nBurrSize1 = 0, nBurrPos1[2] = { 0, };	//0531
	// 	for(int y=rtBurr1[0].top;y<rtBurr1[0].bottom;y++){
	// 		for(int x=rtBurr1[0].left;x<rtBurr1[0].right;x++){
	// 			if(nBurrCheck1 == -1 && *(fm+nw*y+x) < nTabTh1)
	// 				nBurrCheck1 = 0;
	// 			else if(nBurrCheck1==0 && *(fm+nw*y+x) > nTabTh1 && *(fm+nw*(y+1)+x) > nTabTh1 ){
	// 				nBurrCheck1 = 1;nBurrSize1++;nBurrPos1[0]=x;nBurrPos1[1]=y; }
	// 			else if(nBurrCheck1 == 1 && *(fm+nw*y+x) > nTabTh1 && *(fm+nw*(y+1)+x) > nTabTh1 ){
	// 				nBurrCheck1 = 1;nBurrSize1++;}
	// 			else if(nBurrCheck1 == 1 && *(fm+nw*y+x) < nTabTh1 && *(fm+nw*(y+1)+x) < nTabTh1 ){
	// 				nBurrCheck1 = 0;
	// 			}
	// 		}
	// 	}
	int nBurrCheck2 = -1, nBurrSize2 = 0, nBurrPos2[2] = { 0, };
	// 	for(int y=rtBurr1[1].top;y<rtBurr1[1].bottom;y++){
	// 		for(int x=rtBurr1[1].left;x<rtBurr1[1].right;x++){
	// 			if(nBurrCheck2 == -1 && *(fm+nw*y+x) < nTabTh1)nBurrCheck2 = 0;
	// 			else if(nBurrCheck2==0 && *(fm+nw*y+x) > nTabTh1 && *(fm+nw*(y-1)+x) > nTabTh1 ){
	// 				nBurrCheck2 = 1;nBurrSize2++;nBurrPos2[0]=x;nBurrPos2[1]=y;}
	// 			else if(nBurrCheck2 == 1 && *(fm+nw*y+x) > nTabTh1 && *(fm+nw*(y-1)+x) > nTabTh1 ){
	// 				nBurrCheck2 = 1;nBurrSize2++;}
	// 			else if(nBurrCheck2 == 1 && *(fm+nw*y+x) < nTabTh1 && *(fm+nw*(y-1)+x) < nTabTh1 ){
	// 				nBurrCheck2 = 0;
	// 			}
	// 		}
	// 	}
	//좀더 떨어진 지점에서 ABS로 버 한번더 체크  //0613
	int nBurrCheck11 = -1, nBurrSize11 = 0, nBurrPos11[2] = { 0, }, nburrAve = 0;	//0531
	for (int y = rtBurr2[0].top; y<rtBurr2[0].bottom; y++){
		for (int x = rtBurr2[0].left; x<rtBurr2[0].right; x++){
			nburrAve += *(fmBright + nw*y + x);
		}
	}
	if(rtBurr2[0].Width()* rtBurr2[0].Height() >0)
		nburrAve /= (rtBurr2[0].Width()* rtBurr2[0].Height());
	int nburrmargin = 70;
	for (int y = rtBurr2[0].top; y<rtBurr2[0].bottom; y++){
		nBurrCheck11 = -1, nBurrSize11 = 0;
		for (int x = rtBurr2[0].left; x<rtBurr2[0].right; x++){
			if (nBurrCheck11 == -1 && *(fmBright + nw*y + x) < nburrAve + nburrmargin && *(fmBright + nw*y + x) > nburrAve - nburrmargin)
				nBurrCheck11 = 0;
			else if (nBurrCheck11 == 0 && (*(fmBright + nw*y + x) > nburrAve + nburrmargin || *(fmBright + nw*y + x) < nburrAve - nburrmargin)){
				nBurrCheck11 = 1; nBurrSize11++; nBurrPos11[0] = x; nBurrPos11[1] = y;
			}
			else if (nBurrCheck11 == 1 && (*(fmBright + nw*y + x) > nburrAve + nburrmargin || *(fmBright + nw*y + x) < nburrAve - nburrmargin)){
				nBurrCheck11 = 1; nBurrSize11++;
			}
			else if (nBurrCheck11 == 1 && *(fmBright + nw*y + x) < nburrAve + nburrmargin && *(fmBright + nw*y + x) > nburrAve - nburrmargin){
				nBurrCheck11 = 0;
			}
		}
		if (nBurrSize11 >= 2)break;
	}
	int nBurrCheck12 = -1, nBurrSize12 = 0, nBurrPos12[2] = { 0, };
	for (int y = rtBurr2[1].top; y<rtBurr2[1].bottom; y++){
		nBurrCheck12 = -1, nBurrSize12 = 0;
		for (int x = rtBurr2[1].left; x<rtBurr2[1].right; x++){
			if (nBurrCheck12 == -1 && *(fmBright + nw*y + x) < nburrAve + nburrmargin && *(fmBright + nw*y + x) > nburrAve - nburrmargin)
				nBurrCheck12 = 0;
			else if (nBurrCheck12 == 0 && (*(fmBright + nw*y + x) > nburrAve + nburrmargin || *(fmBright + nw*y + x) < nburrAve - nburrmargin)){
				nBurrCheck12 = 1; nBurrSize12++; nBurrPos12[0] = x; nBurrPos12[1] = y;
			}
			else if (nBurrCheck12 == 1 && (*(fmBright + nw*y + x) > nburrAve + nburrmargin || *(fmBright + nw*y + x) < nburrAve - nburrmargin)){
				nBurrCheck12 = 1; nBurrSize12++;
			}
			else if (nBurrCheck12 == 1 && *(fmBright + nw*y + x) < nburrAve + nburrmargin && *(fmBright + nw*y + x) > nburrAve - nburrmargin){
				nBurrCheck12 = 0;
			}
		}
		if (nBurrSize12 >= 2)break;
	}
	if (nBurrSize1 >= 1 || (nBurrSize11 >= 4 && nBurrSize11<30)){//0705
		for (int i = 0; i<2; i++){
			if (nDefect<MAX_HUGEDEFECT){
				if (i == 0 && nBurrSize1 == 0)continue;
				if (i == 1 && nBurrSize11<2)continue;
				CRect rt2;
				int nbsize1 = nBurrSize1;
				if (i == 0 && nBurrSize1 >= 1)//0611
					rt2.SetRect(nBurrPos1[0] - nw2, nBurrPos1[1] - nh2, nBurrPos1[0] + nw2, nBurrPos1[1] + nh2);
				if (i == 1 && nBurrSize11 >= 1){//0611
					rt2.SetRect(nBurrPos11[0] - nw2, nBurrPos11[1] - nh2, nBurrPos11[0] + nw2, nBurrPos11[1] + nh2);
					nbsize1 = nBurrSize11;
				}
				rt2 = SetBoundary2(rt2, CRect(100, 100, nw - 100, nh - 100));
				m_rtIsland[nCam][nDefect] = rt2;
				BYTE * fmdefect = m_pIsland[nCam][nDefect];
				ImageSelectCopy(fmBright, fmdefect, nw, nh, rt2, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				m_dISSizeX[nCam][nDefect] = (nbsize1)*m_dscale_x1[nCam];
				m_dISSizeY[nCam][nDefect] = (2)*m_dscale_y1[nCam];
				m_dISSize[nCam][nDefect] = -1000;
				nDefect++;


			}
		}
	}
	if (nBurrSize2 >= 1 || (nBurrSize12 >= 2 && nBurrSize12<30)){//0705
		for (int i = 0; i<2; i++){
			if (nDefect<MAX_HUGEDEFECT){
				if (i == 0 && nBurrSize2 == 0)continue;
				if (i == 1 && nBurrSize12<2)continue;
				CRect rt2;
				int nbsize1 = nBurrSize2;
				if (i == 0 && nBurrSize2 >= 1)//0611
					rt2.SetRect(nBurrPos2[0] - nw2, nBurrPos2[1] - nh2, nBurrPos2[0] + nw2, nBurrPos2[1] + nh2);
				if (i == 1 && nBurrSize12 >= 1){//0611
					rt2.SetRect(nBurrPos12[0] - nw2, nBurrPos12[1] - nh2, nBurrPos12[0] + nw2, nBurrPos12[1] + nh2);
					nbsize1 = nBurrSize12;
				}
				rt2 = SetBoundary2(rt2, CRect(100, 100, nw - 100, nh - 100));
				m_rtIsland[nCam][nDefect] = rt2;
				BYTE * fmdefect = m_pIsland[nCam][nDefect];
				ImageSelectCopy(fmBright, fmdefect, nw, nh, rt2, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				m_dISSizeX[nCam][nDefect] = (nbsize1)*m_dscale_x1[nCam];
				m_dISSizeY[nCam][nDefect] = (2)*m_dscale_y1[nCam];
				m_dISSize[nCam][nDefect] = -1000;
				nDefect++;
			}
		}
	}



	nFindburr = 0;	nBurrSize = 0, nburrcx = 0;
	for (int x = rtBurr[1].left; x<rtBurr[1].right; x++){
		for (int y = rtBurr[1].top; y<rtBurr[1].bottom; y++){
			if (checkWBW1[x] == 0 && *(fmBright + nw*y + x) < nTabTh)checkWBW1[x] = 1;
			else if (checkWBW1[x] == 1 && *(fmBright + nw*y + x) > nTabTh1 && *(fmBright + nw*(y + 1) + x) > nTabTh1){
				checkWBW1[x] = 2; nFindburr = 1;
			}
		}
	}
	if (nFindburr == 1){
		for (int x = rtBurr[1].left; x<rtBurr[1].right; x++){
			if (checkWBW1[x] == 2 && checkWBW1[x + 1] == 2){
				nBurrSize++; nburrcx += x;
			}
		}
	}

	//   	if(nBurrSize>2 && nDefect<MAX_HUGEDEFECT){ //burr defect
	//   		CRect rt2 ;
	//   		nburrcx /= nBurrSize;
	//   		rt2.SetRect(nburrcx-nw2,m_rtCellTab[nCam].bottom-nh2,nburrcx+nw2,m_rtCellTab[nCam].bottom+nh2);
	//   		rt2 = SetBoundary2(rt2,CRect(100,100,nw-100,nh-100));
	//   		m_rtIsland[nCam][nDefect] = rt2;
	//   		BYTE * fmdefect = m_pIsland[nCam][nDefect];
	//   		ImageSelectCopy(fm,fmdefect,nw,nh,rt2,BAD_WIDTH,BAD_HEIGHT,0,0);
	//   		m_dISSizeX[nCam][nDefect] =  (nBurrSize)*m_dscale_x1[nCam];
	//   		m_dISSizeY[nCam][nDefect] =  (2)*m_dscale_x1[nCam];
	//   		m_dISSize[nCam][nDefect] =  -1000;
	//   		nDefect++;
	//   	}
	///////////////////
	m_nIslandCount[nCam] = nDefect;
	return nDefect;
}


int CInspect::FindEdgebetweenElectrodeAndTab(int nCam, BYTE* fmBright, int nw, int nh, CRect rtins)
{
	CRect rtions;
	rtions = rtins;

	int nLeftOffset = -10;
	//전극과 탭 영역을 일정구간 크기만한 ROI를 만든다
	rtions.right = rtions.left;
	rtions.left += nLeftOffset;
	rtions.right += 22;

	//return bFind;
	int nSizeX = rtions.Width();
	int nSizeY = rtions.Height();

	BYTE* src = new BYTE[nSizeX * nSizeY];
	memset(src, 0, nSizeX * nSizeY);//0803

	int nYpos = 0;
	for (int y = rtions.top; y < rtions.bottom; y++)
	{
		int nXPos = 0;
		for (int x = rtions.left; x < rtions.right; x++)
		{
			BYTE byVal = fmBright[y * nw + x];
			if (200 > byVal)	//반전
			{
				src[nYpos * nSizeX + nXPos] = 0xff;
			}
			else
			{
				src[nYpos * nSizeX + nXPos] = 0x00;
			}
			nXPos++;
		}

		nYpos++;
	}

	m_pFindElectrodeEndPos[nCam]->SetChainData(1, src, 2, 2, 1, 100000, nSizeX, nSizeY);//
	int nBlobCount = m_pFindElectrodeEndPos[nCam]->FastChain(1, 1, nSizeX - 1, nSizeY - 1);

	double dTemp = 0, dBlobSize = 0;
	int nBlobNo = 0;
	for (int i = 0; i < nBlobCount; i++)
	{
		dTemp = m_pFindElectrodeEndPos[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		
		if (dTemp < 4)continue;

		if (dTemp >= dBlobSize)//최대 불량 사이즈.
		{
			nBlobNo = i;	dBlobSize = dTemp;//면적//
		}
	}

	int nFindXPos = 0;

	double dcx = 0, dcy = 0;
	int nret = 0;
	if (nBlobCount > 0) {
		int right = m_pFindElectrodeEndPos[nCam]->FindMaxX(nBlobNo);
		int left = m_pFindElectrodeEndPos[nCam]->FindMinX(nBlobNo);

		nFindXPos = nLeftOffset + right;

		if (0 > nFindXPos)	//예외처리
			nFindXPos = 0;
	}

	delete[] src;

	return  nFindXPos;





	//CPoint cpElectrodeEnd(rtins.left, rtins.top);
	//for (int y = rtions.top; y < rtions.bottom; y++)
	//{
	//	for (int x = rtions.left; x < rtions.right; x++)
	//	{
	//		BYTE byValue = *(fmBright + nw * y + x);
	//		if (200 < byValue)
	//		{
	//			//전극의 끝으로 판단한다.
	//			if (cpElectrodeEnd.x < x) //탭쪽으로 더 멀리 까지 전극이 뻣어있군
	//			{
	//				cpElectrodeEnd.x = x;
	//				cpElectrodeEnd.y = y;
	//			}

	//			break;
	//		}
	//	}
	//}

	//cpElectrodeEnd.x = cpElectrodeEnd.x - rtins.left;
	//cpElectrodeEnd.y = cpElectrodeEnd.y - rtins.top;

	//BOOL bFind = TRUE;
	//if ( m_nIslandOffset[nCam] > cpElectrodeEnd.x) // 옵셋보다 덜 밀려왔다면 옵셋값을 사용한다.
	//{
	//	cpElectrodeEnd.x = m_nIslandOffset[nCam];
	//	bFind = FALSE;
	//}
	//point->x = cpElectrodeEnd.x;
	//point->y = cpElectrodeEnd.y;

}


int CInspect::findIsland(int nCam, BYTE* fmBright, BYTE* fmDark, int nw, int nh, CRect rtins, int * nTabProj, int nIslandTh)
{
	int nDefect = m_nIslandCount[nCam]; 
	int njump1 = 0, nw2 = BAD_WIDTH*0.5, nh2 = BAD_HEIGHT*0.5, nIslandCut = -25;//어두울 경우 skip //0720
	CRect rtions1;
	rtions1 = rtins;//211023
	BYTE * fmtmp = fmBright; //향후 옵션화를 위해
	m_nAve_Island = 0;
	if(m_bIstest)
		memcpy(m_BinaryIslandImage, fmBright, sizeof(BYTE) * nw * nh);


	int y12 = 0, x12 = 0, ngap12 = 1;
	int nXSkip1 = m_dTabSkipRange[nCam][2] / m_dscale_x1[nCam];//right
	int nXSkip2 = m_nIslandOffset[nCam];// / m_dscale_x1[nCam];//left //220110  pixel로 수정
	int nYSkip1 = m_dTabSkipRange[nCam][0] / m_dscale_y1[nCam];
	int nYSkip2 = m_dTabSkipRange[nCam][1] / m_dscale_y1[nCam];


	//double dPrevTime = GetPrecisionTime();
	
	// mi에서 요청하는 blob을 이용한 아일랜드 스킵 영역
	if(true == m_bResetLeftAreaBlob[nCam])
		nXSkip2 += FindEdgebetweenElectrodeAndTab(nCam, fmBright, nw, nh, rtins);
	
	//dPrevTime = (GetPrecisionTime() - dPrevTime) * 1000;

	//220303
	CRect rtinsisland;
	rtinsisland = rtions1;
	rtinsisland.InflateRect(-nXSkip2,-nYSkip1,-nXSkip1,-nYSkip2);//Skip 만큼 영역 줄이기
	rtinsisland = SetBoundary2(rtinsisland, rtions1);
	if(rtinsisland.Width()<0 || rtinsisland.Height()<0){
		return nDefect;
	}
	int ntabw1 = rtinsisland.Width(), ntabh1 = rtinsisland.Height();
	BYTE * fmDark11 = new BYTE[ntabw1*ntabh1];//0803
	memset(fmDark11, 0, ntabw1*ntabh1);//0803

	//탭 접힘 테스트
	BYTE * fmZipper = new BYTE[ntabw1*ntabh1];//220412
	memset(fmZipper, 0, ntabw1*ntabh1);//220412
	int nPorjY[1000]={0,},nYcnt=0,nXcnt=0,nMag1 = 12;//220412
	int nZipperTH = 20;
	int nZipperRightLimit = 200;//8.4mm
	CRect rtZipper;
	rtZipper.left = m_dTabZipperLeftOffset[nCam] /m_dscale_x1[nCam];
	rtZipper.right = m_dTabZipperRightOffset[nCam] /m_dscale_x1[nCam];
	rtZipper.top = m_dTabZipperTBOffset[nCam] /m_dscale_x1[nCam];
	rtZipper.bottom = ntabh1 - m_dTabZipperTBOffset[nCam] /m_dscale_x1[nCam];

	if(rtZipper.left < nXSkip2)rtZipper.left = nXSkip2;
	if(rtZipper.right > nZipperRightLimit)rtZipper.right = nZipperRightLimit;
	if(rtZipper.top < nYSkip1)rtZipper.top = nYSkip1;
	if(rtZipper.bottom > ntabh1-nYSkip2)rtZipper.bottom = ntabh1-nYSkip2;
	
	//평균밝기
	int nAve1 = 0,nAve2 = 0,nAvecnt=0,nAvecnt2=0;
	for(int y=rtinsisland.top;y<rtinsisland.bottom && y>0 && y<nh ; y++){
		nXcnt = 0;
		for(int x=rtinsisland.left;x<rtinsisland.right && x>0 && x<nw ;x+=2){
			nAve1 += *(fmtmp + nw*y + x);nAvecnt++;
			if(x > rtinsisland.left + rtZipper.left && x < rtinsisland.left+rtZipper.right && y > rtinsisland.top+rtZipper.top  && y<rtinsisland.top+rtZipper.bottom){
				nAve2 += *(fmDark + nw*y + x);nAvecnt2++;
			}
			
//			if(m_bZipperInspect[nCam] && nYcnt <1000 && rtZipper.left < x && rtZipper.right > x && rtZipper.top < y && rtZipper.bottom > y){//zipper //x 설정 영역
			//if(m_bZipperInspect[nCam] && nYcnt <1000 &&  rtZipper.top < y && rtZipper.bottom > y){//zipper //x 전체 영역
			//	nPorjY[nYcnt] += *(fmDark + nw*y + x);
			//	nXcnt++;
			//}
		}
		//zipper
		//if(m_bZipperInspect[nCam] && nYcnt<1000 && nXcnt>0){
		//	nPorjY[nYcnt] /= nXcnt;
		//	nYcnt++;
		//}
	}


	if(nAvecnt==0)nAvecnt =1;
	nAve1 /= nAvecnt;
	if(nAvecnt2==0)nAvecnt2 =1;
	nAve2 /= nAvecnt2;//dark 영역의 평균밝기
	int y11=0,x11=0;
	int nxJump = 30, nyJump = 10;
	BOOL bTabTopBottomTh = m_bTabEdgerangeTh[nCam];//FALSE;
	double dTabTBSkipmm = m_dTabEdgeRangemm[nCam];//3
	if(bTabTopBottomTh==FALSE){
		for(int y=rtinsisland.top;y<rtinsisland.bottom && y>0 && y<nh ; y++,y11++){
			x11 = 0;
			for(int x=rtinsisland.left;x<rtinsisland.right && x>0 && x<nw ;x++,x11++){
				if(y11<=ngap12 || y11>=ntabh1-1-ngap12 || x11<=ngap12 || x11>=ntabw1-1-ngap12){
				}
				else{
					if ((y<rtinsisland.top + nyJump && x > rtinsisland.right - nxJump) || (y>rtinsisland.bottom - nyJump && x > rtinsisland.right - nxJump))
						continue;

					if( nAve1 - *(fmtmp + nw*y + x) > nIslandTh && *(fmtmp + nw*y + x) > nIslandCut)
						*(fmDark11 + ntabw1*y11 + x11) = 0xff;

					//zipper
					if(m_bZipperInspect[nCam] && x11 > rtZipper.left && x11 < rtZipper.right && y11 > rtZipper.top && y11 < rtZipper.bottom){
						int ntmp12 = (*(fmDark + nw*y + x) - nAve2) * nMag1 + m_nTargetBrightness[nCam];
						if(ntmp12>255)		ntmp12 = 255;
						else if(ntmp12<0)	ntmp12 = 0;
						if(m_nTabZipperTh[nCam] > ntmp12){
							*(fmZipper + ntabw1*y11 + x11) = 0xff;
							*(m_fmOverlayPreprocess[nCam]+y*nw+x) = 0xff;
						}
						else
							*(m_fmOverlayPreprocess[nCam]+y*nw+x) = 0x00;
					}
				}
			}
		}
	}
	else{
		int nTbSkipmm = dTabTBSkipmm/m_dscale_y1[nCam];
		int nIslandTh1 = m_dMiddleAve[nCam];
		int nIslandThGap1 = 10;
		for(int y=rtinsisland.top;y<rtinsisland.bottom && y>0 && y<nh ; y++,y11++){
			x11 = 0;
			if(y<rtinsisland.top + nTbSkipmm || y>rtinsisland.bottom-nTbSkipmm){
				for(int x=rtinsisland.left;x<rtinsisland.right && x>0 && x<nw ;x++,x11++){
					if(y11<=ngap12 || y11>=ntabh1-1-ngap12 || x11<=ngap12 || x11>=ntabw1-1-ngap12){
					}
					else{
						if ((y<rtinsisland.top + nyJump && x > rtinsisland.right - nxJump) || (y>rtinsisland.bottom - nyJump && x > rtinsisland.right - nxJump))
							continue;

						if( *(fmtmp + nw*y + x) < nIslandTh1 + nIslandThGap1 && *(fmtmp + nw*y + x) >  nIslandTh1 - nIslandThGap1)//전극밝기 상하 10 만큼사이만 이진화를 한다.
							*(fmDark11 + ntabw1*y11 + x11) = 0xff;

						//zipper
						if(m_bZipperInspect[nCam] && x11 > rtZipper.left && x11 < rtZipper.right && y11 > rtZipper.top && y11 < rtZipper.bottom){
							int ntmp12 = (*(fmDark + nw*y + x) - nAve2) * nMag1 + m_nTargetBrightness[nCam];
							if(ntmp12>255)		ntmp12 = 255;
							else if(ntmp12<0)	ntmp12 = 0;
							if(m_nTabZipperTh[nCam] > ntmp12){
								*(fmZipper + ntabw1*y11 + x11) = 0xff;
								*(m_fmOverlayPreprocess[nCam]+y*nw+x) = 0xff;
							}
							else
								*(m_fmOverlayPreprocess[nCam]+y*nw+x) = 0x00;
						}
					}
				}
			}
			else{
				for(int x=rtinsisland.left;x<rtinsisland.right && x>0 && x<nw ;x++,x11++){
					if(y11<=ngap12 || y11>=ntabh1-1-ngap12 || x11<=ngap12 || x11>=ntabw1-1-ngap12){
					}
					else{
						if ((y<rtinsisland.top + nyJump && x > rtinsisland.right - nxJump) || (y>rtinsisland.bottom - nyJump && x > rtinsisland.right - nxJump))
							continue;

						if( nAve1 - *(fmtmp + nw*y + x) > nIslandTh && *(fmtmp + nw*y + x) > nIslandCut)
							*(fmDark11 + ntabw1*y11 + x11) = 0xff;

						//zipper
						if(m_bZipperInspect[nCam] && x11 > rtZipper.left && x11 < rtZipper.right && y11 > rtZipper.top && y11 < rtZipper.bottom){
							int ntmp12 = (*(fmDark + nw*y + x) - nAve2) * nMag1 + m_nTargetBrightness[nCam];
							if(ntmp12>255)		ntmp12 = 255;
							else if(ntmp12<0)	ntmp12 = 0;
							if(m_nTabZipperTh[nCam] > ntmp12){
								*(fmZipper + ntabw1*y11 + x11) = 0xff;
								*(m_fmOverlayPreprocess[nCam]+y*nw+x) = 0xff;
							}
							else
								*(m_fmOverlayPreprocess[nCam]+y*nw+x) = 0x00;
						}
					}
				}
			}
		}
	}
	//20220916 SJM
	if (m_bIstest) {
		int pos = rtinsisland.top;
		for (int y = 0; y < rtinsisland.Height(); y++)
		{
			memcpy(&m_BinaryIslandImage[pos * m_nWidth + rtinsisland.left],
				&fmDark11[y * rtinsisland.Width()],
				sizeof(BYTE) * rtinsisland.Width());
			pos++;
		}
		m_rtBinaryIsland = rtinsisland;
		m_nAve_Island = nAve1;
	}
	//20220916 SJM
	m_pChainIsland[nCam]->SetChainData(1, fmDark11, 1, 1, 4, 100000, ntabw1, ntabh1);//
	int nBlobCount14 = m_pChainIsland[nCam]->FastChain(1, 1, ntabw1 -1, ntabh1 -1);//
	double dTemp14 = 0, nBlobNo14old = 0, dBlobSize14 = 0; int nBlobNo14 = -1;
	for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 왼쪽//0910
	{
		dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp14 < 4)continue;
		double dcx12 = 0, dcy12 = 0, dmindist = 0, dmaxdist = 0, davedist = 0;
		m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1	
		int nx1 = m_pChainIsland[nCam]->FindMaxX(i) + 1;
		int nx2 = m_pChainIsland[nCam]->FindMinX(i);
		int ny1 = m_pChainIsland[nCam]->FindMaxY(i) + 1;
		int ny2 = m_pChainIsland[nCam]->FindMinY(i);
		int nxs = nx1 - nx2, nys = ny1 - ny2;//0803
		double dXLimit1 = 5.0;
		//X로만 길면 탭프레스로 스킵한다.
		if(double(nxs / nys) > dXLimit1)
			continue;
		m_rtIsland[nCam][nDefect].SetRect(rtinsisland.left+dcx12 - nw2, rtinsisland.top+dcy12 - nh2, rtinsisland.left+dcx12 + nw2, rtinsisland.top+dcy12 + nh2);
		m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(rtions1.left, rtions1.top + nYSkip1, rtions1.right - nXSkip1, rtions1.bottom - nYSkip2));//0611
		BYTE * fmdefect = m_pIsland[nCam][nDefect];
		CRect rt2 = m_rtIsland[nCam][nDefect], rt1, rt;
		ImageSelectCopy(fmBright, fmdefect, nw, nh, rt2, BAD_WIDTH, BAD_HEIGHT, 0, 0);
		double dcompact = m_pChainIsland[nCam]->FindCompactness(i);//0803
		double dangle11 = m_pChainIsland[nCam]->FindAngle(i);
		m_pChainIsland[nCam]->FineDistFromPoint(i, dcx12, dcy12, &dmindist, &dmaxdist, &davedist);//장축 단축 구하기
		//Tab 상부 // 하부인지 찾기
		m_dISAngle[nCam][nDefect] = dangle11;			//0white 1black	//0803
		m_dISCompactness[nCam][nDefect] = dcompact;			//0white 1black //0803
		m_rtIslandBlob[nCam][nDefect].SetRect(nx2, ny2, nx1, ny1);
		double dPercent = 0;
		for (int y = m_rtIslandBlob[nCam][nDefect].top; y < m_rtIslandBlob[nCam][nDefect].bottom && y<ntabh1; y++) {//220318
			for (int x = m_rtIslandBlob[nCam][nDefect].left; x < m_rtIslandBlob[nCam][nDefect].right && x<ntabw1; x++) {//220318
				if (*(fmDark11 + y*ntabw1 + x) > 200)//220318
					dPercent++;
			}
		}
		if((nx1-nx2) >0 &&  (ny1-ny2)>0)//220318
			dPercent = dPercent / ((nx1-nx2) * (ny1-ny2)) * 100;
		else dPercent = 0;//220318
		m_dIslnadPercent[nCam][nDefect] = dPercent;
		//211103
		m_dISMaxDist[nCam][nDefect] = dmindist;
		m_dISMinDist[nCam][nDefect] = dmaxdist;
		m_dISSizeX[nCam][nDefect] = (nxs)*m_dscale_x1[nCam];
		m_dISSizeY[nCam][nDefect] = (nys)*m_dscale_y1[nCam];
		if (m_dISSizeX[nCam][nDefect]>m_dISSizeY[nCam][nDefect])m_dISSize[nCam][nDefect] = m_dISSizeX[nCam][nDefect];
		else m_dISSize[nCam][nDefect] = m_dISSizeY[nCam][nDefect];
		nDefect++;
		if (nDefect >= MAX_HUGEDEFECT - 1)break;
	}

	if(m_bZipperInspect[nCam] && nDefect < MAX_HUGEDEFECT - 1)
	{
		m_pChainIsland[nCam]->SetChainData(1, fmZipper, 2, 2, 20, 100000, ntabw1, ntabh1);//
		int nBlobCount1 = m_pChainIsland[nCam]->FastChain(rtZipper.left, rtZipper.top, rtZipper.right , rtZipper.bottom );//
		double dTemp14 = 0, nBlobNo14old = 0, dBlobSize14 = 0; int nBlobNo14 = -1;
		int nYsizeInCount = 0;
		for (int i = 0; i < nBlobCount1; i++)//미코팅 탈리 왼쪽//0910
		{
			dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp14 < 10)continue;
			double dcx12 = 0, dcy12 = 0, dmindist = 0, dmaxdist = 0, davedist = 0;
			m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1	
			int nx1 = m_pChainIsland[nCam]->FindMaxX(i) + 1;
			int nx2 = m_pChainIsland[nCam]->FindMinX(i);
			int ny1 = m_pChainIsland[nCam]->FindMaxY(i) + 1;
			int ny2 = m_pChainIsland[nCam]->FindMinY(i);
			int nxs = nx1 - nx2, nys = ny1 - ny2;//0803
			double dXLimit1 = 5.0;
			//X로만 길면 탭프레스로 스킵한다.
			if(double(nxs) / double(nys) > dXLimit1)
				continue;
			m_rtIsland[nCam][nDefect].SetRect(rtinsisland.left+dcx12 - nw2, rtinsisland.top+dcy12 - nh2, rtinsisland.left+dcx12 + nw2, rtinsisland.top+dcy12 + nh2);
			m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(rtions1.left, rtions1.top + nYSkip1, rtions1.right - nXSkip1, rtions1.bottom - nYSkip2));//0611
			BYTE * fmdefect = m_pIsland[nCam][nDefect];
			CRect rt2 = m_rtIsland[nCam][nDefect], rt1, rt;
			ImageSelectCopy(fmBright, fmdefect, nw, nh, rt2, BAD_WIDTH, BAD_HEIGHT, 0, 0);
			double dcompact = m_pChainIsland[nCam]->FindCompactness(i);//0803
			double dangle11 = m_pChainIsland[nCam]->FindAngle(i);
			m_pChainIsland[nCam]->FineDistFromPoint(i, dcx12, dcy12, &dmindist, &dmaxdist, &davedist);//장축 단축 구하기
			m_dISAngle[nCam][nDefect] = dangle11;			//0white 1black	//0803
			m_dISCompactness[nCam][nDefect] = dcompact;			//0white 1black //0803
			m_rtIslandBlob[nCam][nDefect].SetRect(nx2, ny2, nx1, ny1);
			double dPercent = 0;
			for (int y = m_rtIslandBlob[nCam][nDefect].top; y < m_rtIslandBlob[nCam][nDefect].bottom && y<ntabh1; y++) {//220318
				for (int x = m_rtIslandBlob[nCam][nDefect].left; x < m_rtIslandBlob[nCam][nDefect].right && x<ntabw1; x++) {//220318
					if (*(fmDark11 + y*ntabw1 + x) > 200)//220318
						dPercent++;
				}
			}
			if((nx1-nx2) >0 &&  (ny1-ny2)>0)//220318
				dPercent = dPercent / ((nx1-nx2) * (ny1-ny2)) * 100;
			else dPercent = 0;//220318
			m_dIslnadPercent[nCam][nDefect] = dPercent;
			m_dISMaxDist[nCam][nDefect] = dmindist*2;
			m_dISMinDist[nCam][nDefect] = dmaxdist*2;
			m_dISSizeX[nCam][nDefect] = (nxs)*m_dscale_x1[nCam];
			m_dISSizeY[nCam][nDefect] = (nys)*m_dscale_y1[nCam];
			if( fabs(dangle11) > 60 && m_dISSizeX[nCam][nDefect] > m_dZipperSpecSizeX[nCam] && m_dISSizeY[nCam][nDefect] > m_dZipperSpecSizeY[nCam]  ){
				m_dISSize[nCam][nDefect] = 5002;
				nDefect++;
			}else{
				if( fabs(dangle11) > 60 &&  m_dISSizeY[nCam][nDefect] > m_dZipperYMinSize[nCam] && dcompact*100 < 20){
					nYsizeInCount++;
					if(nYsizeInCount > m_nZipperSpecCount[nCam]){
						m_dISSize[nCam][nDefect] = 5002;
						nDefect++;
					}
				}
			}
			if (nDefect >= MAX_HUGEDEFECT - 1)break;
		}
	}

	m_nIslandCount[nCam] = nDefect;//0706
	delete [] fmDark11;
	delete [] fmZipper;

	return nDefect;
}

int CInspect::InspNgTag(int nCam, BYTE * fmBright, BYTE * fmDark, int nw, int nh, CRect rttabins1)
{
	m_nAve_Ngtag = 0;
	if (m_nIslandCount[nCam] >= MAX_HUGEDEFECT - 1)//211017-1
		return m_nIslandCount[nCam];
	int nDefect = m_nIslandCount[nCam];
	CRect rtInspNgTag = rttabins1;// 23.02.17
	rtInspNgTag.DeflateRect(m_rtNaTagOffset[nCam]);// 23.02.17
	rtInspNgTag = rtInspNgTag& rttabins1;// 23.02.17
	int nDarkTabAve = 0, nDarktabcnt1 = 0, ntabw1 = rtInspNgTag.Width(), ntabh1 = rtInspNgTag.Height(), nDarkTabTh = m_nLabelTagTh[nCam];//0720
	BYTE * fmDark1 = new BYTE[ntabw1*ntabh1];//0806
	BYTE * fmBright1 = new BYTE[ntabw1*ntabh1];//0806
	memset(fmDark1, 0, ntabw1*ntabh1);
	memset(fmBright1, 0, ntabw1*ntabh1);//0806nn
	ImageSelectCopy(fmDark, fmDark1, nw, nh, rtInspNgTag, ntabw1, ntabh1, 0, 0);
	if (m_bIstest)//20221012 SJM
		memcpy(m_BinaryNgTagImage, fmDark, sizeof(BYTE) * nw * nh);
	//평균밝기 구하고 이진화
	int nTabSkip = 5;//0720 //0806 //@@
	int nPeak12 = 0;
	//// 23.02.17
	int nTBSkip = 0, nLRSkip = 0;
	//int nTBSkip = m_dTabLabelSkipTB[nCam] / m_dscale_y1[nCam];//0901
	//int nLRSkip = m_dTabLabelSkipLR[nCam] / m_dscale_x1[nCam];//0901

	//if (nTBSkip < 5)nTBSkip = 5;//0901
	//if (nLRSkip < 5)nLRSkip = 5;//0901
	///
	for (int y = nTBSkip; y < ntabh1 - nTBSkip&&y >= 0 && y < ntabh1 && y >= 0; y++) {//211017-1
		for (int x = 5 + nLRSkip; x < ntabw1 - 5 - nLRSkip && x >= 0 && x < ntabw1; x++) {//211017-1
			nDarkTabAve += *(fmDark1 + y*ntabw1 + x); nDarktabcnt1++;
			if (nPeak12 < *(fmDark1 + y*ntabw1 + x))nPeak12 = *(fmDark1 + y*ntabw1 + x);
		}
	}
	if (nDarktabcnt1 > 0)nDarkTabAve /= nDarktabcnt1;
//	if (nPeak12 - nDarkTabAve > 50) {//220207
		int nDarkTabAve1 = 0, nDarktabcnt2 = 0;
		for (int y = nTBSkip; y < ntabh1 - nTBSkip&&y >= 0 && y < ntabh1; y++) {//211017-1
			for (int x = 5 + nLRSkip; x < ntabw1 - 5 - nLRSkip && x >= 0 && x < ntabw1; x++) {//211017-1
				if (nDarkTabAve <= *(fmDark1 + y*ntabw1 + x)) {
					nDarkTabAve1 += *(fmDark1 + y*ntabw1 + x); nDarktabcnt2++;
				}
			}
		}
		if (nDarktabcnt2 > 0)nDarkTabAve = nDarkTabAve1 / nDarktabcnt2;
//	}
	m_dTabBrightness[nCam][1] = nDarkTabAve;//0720  탭 밝기 dark
	int nLowcut1 = 5;
	for (int y = 0; y < ntabh1&&y >= 0 && y < ntabh1 && y >= 0; y++) {//211017-1
		for (int x = 0; x < ntabw1 && x >= 0 && x < ntabw1; x++) {//0604-1
			if (y<nTBSkip || y>ntabh1 - nTBSkip || x<5 + nLRSkip || x>ntabw1 - 10 - nLRSkip)*(fmDark1 + y*ntabw1 + x) = 0;//0901
			else {
				if (*(fmDark1 + y*ntabw1 + x) > nDarkTabAve - nDarkTabTh || *(fmDark1 + y*ntabw1 + x) < nLowcut1)*(fmDark1 + y*ntabw1 + x) = 0;//0803
				else
					*(fmDark1 + y*ntabw1 + x) = 0xff;
			}
		}
	}
	if (m_bIstest) {//20221012 SJM
		int pos = rtInspNgTag.top;
		for (int y = 0; y < rtInspNgTag.Height(); y++)
		{
			memcpy(&m_BinaryNgTagImage[pos * m_nWidth + rtInspNgTag.left],
				&fmDark1[y * rtInspNgTag.Width()],
				sizeof(BYTE) * rtInspNgTag.Width());
			pos++;
		}
		//CRect rtinspNgtag;
		//rtinspNgtag = rttabins1;
		//rtinspNgtag.InflateRect(-nLRSkip-5, -nTBSkip, -nLRSkip - 9, -nTBSkip);//Skip 만큼 영역 줄이기

		m_rtBinaryNgtag = rtInspNgTag;
		m_nAve_Ngtag = nDarkTabAve;
	}
	m_pChainIsland[nCam]->SetChainData(1, fmDark1, 2, 2, 50, 100000, ntabw1, ntabh1);//white //label tag
	int nBlobCount13 = m_pChainIsland[nCam]->FastChain(1, 1, ntabw1 - 5, ntabh1 - 1);//220714
	double dTemp13 = 0, dBlobSize13 = 0; int nBlobNo13 = 0;
	for (int i = 0; i < nBlobCount13; i++)
	{
		dTemp13 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp13 < 10)continue;
		if (dTemp13 >= dBlobSize13)//최대 불량 사이즈.
		{
			nBlobNo13 = i;	dBlobSize13 = dTemp13;//면적
		}
	}
	//	m_dTabTapeX[nCam] = 1;m_dTabTapeY[nCam] = 0.2;
	BOOL bTabPressCheck = FALSE, bTabPressSkip = FALSE;//0830
	int nw2 = BAD_WIDTH*0.5, nh2 = BAD_HEIGHT*0.5;
	if (dBlobSize13 >= 100) {//0713 //0720
		int nx1 = m_pChainIsland[nCam]->FindMaxX(nBlobNo13);
		int nx2 = m_pChainIsland[nCam]->FindMinX(nBlobNo13);
		int ny1 = m_pChainIsland[nCam]->FindMaxY(nBlobNo13);
		int ny2 = m_pChainIsland[nCam]->FindMinY(nBlobNo13);
		//int ntbpos1 = 0;
		//if (rttabins1.top > ny2 - 5)ntbpos1 = 1;//위쪽에 붙음
		//if (rttabins1.bottom < ny1 + 5)ntbpos1 = 2;//위쪽에 붙음
		int nSizeX = nx1 - nx2+1;
		int nSizeY = ny1 - ny2+1;
		int nDefectArea = nSizeX * nSizeY;
		double dDarkPercent;
		if (0 == nDefectArea)
			dDarkPercent = 0;
		else
			dDarkPercent = dBlobSize13 / nDefectArea * 100/*100%*/;
		double dcompact = m_pChainIsland[nCam]->FindCompactness(nBlobNo13);//0803
		double dangle11 = m_pChainIsland[nCam]->FindAngle(nBlobNo13);
		double dcx12 = 0, dcy12 = 0, dmindist = 0, dmaxdist = 0, davedist = 0;
		m_pChainIsland[nCam]->Chain_Center(nBlobNo13, &dcx12, &dcy12);//중심점
		m_pChainIsland[nCam]->FineDistFromPoint(nBlobNo13, dcx12, dcy12, &dmindist, &dmaxdist, &davedist);//장축 단축 구하기
		m_dISAngle[nCam][nDefect] = dangle11;			//0white 1black	//0803
		m_dISCompactness[nCam][nDefect] = dcompact;			//0white 1black //0803

		if (/*fabs(dangle11)<10 &&*/ (nSizeX )*m_dscale_x1[nCam] > m_dTabTapeX[nCam] && (nSizeY)*m_dscale_y1[nCam] > m_dTabTapeY[nCam] && dDarkPercent >= m_dDefectPercent[nCam]) {//0803 2도보다 작아야 테이프로 인식
			if (!bTabPressSkip) {
				m_dIslnadPercent[nCam][nDefect] = 0;
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2 + rtInspNgTag.left, dcy12 - nh2 + rtInspNgTag.top, dcx12 + nw2 + rtInspNgTag.left, dcy12 + nh2 + rtInspNgTag.top);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(80, 0, nw - 100, nh));//211017-1
				m_dISSizeX[nCam][nDefect] = (nSizeX)*m_dscale_x1[nCam];
				m_dISSizeY[nCam][nDefect] = (nSizeY)*m_dscale_y1[nCam];
//				if (m_bNgTag)//tape로 검출//0625
					m_dISSize[nCam][nDefect] = 6004;//0909
//				else
//					m_dISSize[nCam][nDefect] = 0;
				BYTE * fmdefect1 = m_pIsland[nCam][nDefect];
				CRect rt22 = m_rtIsland[nCam][nDefect];
				//			ImageSelectCopy(m_fmOdd[nCam],fmdefect1,nw,nh,rt22,BAD_WIDTH,BAD_HEIGHT,0,0);//
				if (m_bDefectBinary) {//0729-1
					rt22.SetRect(dcx12 - BAD_WIDTH*0.5, dcy12 - BAD_HEIGHT*0.5, dcx12 + BAD_WIDTH*0.5, dcy12 + BAD_HEIGHT*0.5);
					ImageSelectCopy(fmDark1, fmdefect1, ntabw1, ntabh1, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				}
				else {
					ImageSelectCopy(m_fmOdd[nCam], fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				}
				nDefect++;
				
				if (nDefect >= MAX_HUGEDEFECT - 1)//211017-1
					return nDefect;
			}
		}
	}
	m_nIslandCount[nCam] = nDefect;//0706

	//	double d113 = GetPrecisionTime();
	delete []fmBright1;
	delete []fmDark1;
	return nDefect;
}

int CInspect::InspInsulate(int nCam, BYTE* fm, BYTE* fmDark, int nw, int nh, int nRightSum, int nRightSum1, IPVM::Rect rtInspArea, IPVM::Rect rtOffset, int nInsulDarkLeft, int nInsulDarkRight, BOOL InspGap)//절연 영역 내 불량 검사 함수(절연 핀홀, 절연 라인)
{//rt 변경으로 평션 교체 //211017-1
	int nInsulHisto[256] = { 0, }, nInsulHistocnt = 0;//0526-1
	int nInsulAverage = 0, ninsulcount = 0;
	int nXSkip = 0, nYSkip = 0;//211023
	m_nAve_Insulate = 0;
	BOOL InspTabInsul = InspGap;
	//rtInspArea.InflateRect(rtOffset.m_left, rtOffset.m_top, rtOffset.m_right, rtOffset.m_bottom);
	CRect rtInsulRect= IPVM::ToMFC(rtInspArea);
	CRect rtImage(10, 10, nw - 10, nh - 10);

	rtInsulRect = rtInsulRect & rtImage;


	
	

	int InsulDarkLeft = nInsulDarkLeft;//220328
	int InsulDarkRight = nInsulDarkRight;//220401

	//int nDarkProj[1000] = { 0, };
	
	if (rtInsulRect.Width() < InsulDarkRight - InsulDarkLeft)
	{
		InsulDarkRight = rtInsulRect.Width();
		InsulDarkLeft = 0;
	}
	

	int* nDarkProj = new int[rtInsulRect.Width()];
	int nYcnt12 = 0;//221201

	double dStartTime = GetPrecisionTime();
	if (m_bInsulateDarkInsp[nCam])
	{
		for (int y = rtInsulRect.top; y < rtInsulRect.bottom && y >= 0; y++) {//211023
			for (int x = rtInsulRect.left, x12 = 0; x < rtInsulRect.right && x >= 0 && x12 < 999; x++, x12++)
			{
				nInsulAverage += *(fmDark + nw * y + x);
				nInsulHisto[*(fmDark + nw * y + x)]++;
				ninsulcount++;

				if (x12<rtInsulRect.right+InsulDarkRight && x12 >rtInsulRect.left+ InsulDarkLeft) {//220401
					nDarkProj[x12] += *(fmDark + nw * y + x);
				}
			}
			nYcnt12++;
		}
		if (nYcnt12 > 0)
		{
			if (TRUE == InspTabInsul)
			{
				for (int x12 = InsulDarkLeft; x12 < InsulDarkRight && x12 < 999 && nYcnt12>0; x12++)
				{//220401
					nDarkProj[x12] /= nYcnt12;
				}
			}
			else if(FALSE==InspTabInsul)
			{
				for (int x12 = rtInsulRect.left; x12 < rtInsulRect.right&& x12 < 999 && nYcnt12>0; x12++)
				{//220401
					nDarkProj[x12] /= nYcnt12;
				}
			}
		}
	}
	else {
		for (int y = rtInsulRect.top; y < rtInsulRect.bottom && y >= 0; y++)
		{//211023
			for (int x = rtInsulRect.left, x12 = 0; x < rtInsulRect.right && x >= 0; x++, x12++)
			{//221201
				nInsulAverage += *(fmDark + nw * y + x);
				nInsulHisto[*(fmDark + nw * y + x)]++;
				ninsulcount++;

				nDarkProj[x12] += *(fmDark + nw * y + x);//221201
			}
			nYcnt12++;//221201
		}
		for (int x = rtInsulRect.left, x12 = 0; x < rtInsulRect.right - 5 && x >= 0; x++, x12++)
		{//221201
			nDarkProj[x12] /= nYcnt12;
		}
	}
	m_dNewinspTime[nCam][0] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	//탭프레스 처음 위치 찾기 //221201
	int nTabpressPos1 = 0, nTabpressPos2 = 0, nTabpressTh = 3, nTabLeftSum1 = 0;;
	for (int x = rtInsulRect.left + 6, x12 = 6; x < rtInsulRect.left + 6 + 30 && x >= 0; x++, x12++)
	{//221201
		nTabLeftSum1 += nDarkProj[x12];
		if (nTabpressTh <= abs(nDarkProj[x12] - nDarkProj[x12 + 4]))
		{
			nTabpressPos1 = abs(nDarkProj[x12] - nDarkProj[x12 + 4]);

			if (x12 - 5 > 0)
			nTabLeftSum1 /= (x12 - 5);
			else
				nTabLeftSum1 = 0;

			nTabpressPos2 = x; break;
		}
	}

	delete[] nDarkProj;
	if (nTabpressPos1 == 0) 
	{
		nTabLeftSum1 /= 30;
		nTabpressPos2 = rtInsulRect.left + 30;
	}
	m_dNewinspTime[nCam][1] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	CRect rtInsulGap;
	rtInsulGap = rtInsulRect;
	rtInsulGap.left = rtInsulRect.left + 6;
	rtInsulGap.right = nTabpressPos2;

	if (ninsulcount == 0)
		ninsulcount = 1;
	nInsulAverage /= ninsulcount;
	int nHistoCut = ninsulcount * 0.01;// 1;//0526-1 //211011
	for (int i = 255; i > nInsulAverage; i--)
	{
		nInsulHistocnt += nInsulHisto[i];
		if (nHistoCut <= nInsulHistocnt) {
			nInsulHistocnt = i; break;
		}
	}

	m_dNewinspTime[nCam][2] = (GetPrecisionTime() - dStartTime) * 1000.0f;
	//	if(m_dInsulateThGap[nCam]<1.8)m_dInsulateThGap[nCam] = 2;
	int nInsulgapTh = m_dInsulgapTh[nCam] * nRightSum1;//221201

	double dInsulRatio = m_dInsulateThGap[nCam];//1.18;//0727 //파라미터 뺄것 //파라미터
	m_nAve_Insulate = nInsulAverage;
	int nInsulTh1 = nRightSum * 1.0, nBlobNo1 = 0, nInsulth2 = nInsulAverage * dInsulRatio;//35;//(nTabSum-nInsulAverage)*0.9;//0523
	int ngap1 = 0;//10
	nInsulTh1 = nInsulth2;//(nInsulTh1+nInsulth2)*0.5;
	if (nInsulTh1 < nInsulth2)			nInsulTh1 = nInsulth2;
	if (nInsulTh1 >= 255)nInsulTh1 = 250;
	//	if (nInsulHistocnt > nInsulTh1)nInsulTh1 = nInsulHistocnt;//0526 //히스토그램//220427 제거
	if (nInsulTh1 >= 255)nInsulTh1 = nRightSum1;
	//	if (nInsulTh1 < nRightSum1 - ngap1)nInsulTh1 = nRightSum1 - ngap1;//0607 //우측 밝기 //탭밝기 //제거
	if (nInsulTh1 == 255)nInsulTh1 = 250;//0625-1

//주석

	int nRectSizeX = rtInsulRect.Width();
	int nRectSizeY = rtInsulRect.Height();

	if (0 > nRectSizeX || 0 > nRectSizeY)
	{
		return 0;
	}

	BYTE* fmInsul = new BYTE[nRectSizeX * nRectSizeY]; //pby
	BYTE* fmInsulDark = new BYTE[nRectSizeX * nRectSizeY];
	BYTE* fmInsulGap = new BYTE[nRectSizeX * nRectSizeY];


	ZeroMemory(fmInsul, nRectSizeX * nRectSizeY);
	ZeroMemory(fmInsulDark, nRectSizeX * nRectSizeY);//220328
	ZeroMemory(fmInsulGap, nRectSizeX * nRectSizeY);//221201

	m_nInsulateBufferWidth[nCam] = nRectSizeX;
	m_nInsulateBufferHeight[nCam] = nRectSizeY;

	CRect rtInsulBrightlTabArea = rtInsulRect;

	if (TRUE == InspTabInsul) //탭 절연 다크검사
	{
		rtInsulRect.right = rtInsulRect.left + InsulDarkRight;
		rtInsulRect.left = rtInsulRect.left+InsulDarkLeft;
		rtInsulRect.top = rtInsulRect.top + rtOffset.m_top;
		rtInsulRect.bottom = rtInsulRect.bottom - rtOffset.m_bottom;


		//절연영역용
		rtInsulBrightlTabArea.DeflateRect(rtOffset.m_left, rtOffset.m_top, rtOffset.m_right, rtOffset.m_bottom);
		rtInsulBrightlTabArea = rtInsulBrightlTabArea & rtImage;
	}
	else//cutting height 부분
	{
		rtInsulRect.DeflateRect(rtOffset.m_left, rtOffset.m_top, rtOffset.m_right, rtOffset.m_bottom);

	}
	rtInsulRect = rtInsulRect & rtImage;
	rtInsulBrightlTabArea = rtInsulBrightlTabArea & rtImage;
	// 여기에 옵셋들어가야 할듯....


	if (m_bIstest)  //test에서 절연영역 보여주는곳 
	{
		if (TRUE == m_bCuttingHeightTopAreaInspUse[nCam])
		{
			m_rtCuttingHeightTopInspArea[nCam] = m_rtCuttingHeightTop[nCam];
		}
		if (TRUE == m_bCuttingHeightBottomAreaInspUse[nCam])
		{
			m_rtCuttingHeightBottomInspArea[nCam] = m_rtCuttingHeightBottom[nCam];
		}
	}

	bool bNewInsulateBinaryTest = false;

	//InspInsulate 함수 오래 걸리는 부분 230413 윤여선

	if (m_bInsulateDarkInsp[nCam])
	{//다크 검사도 같이 한다.
		if (m_bSbrIsnadImage[nCam])
		{//Bright Image
			for (int y = rtInsulRect.top , y1 = 0 ; y < rtInsulRect.bottom  && y >= 0; y++, y1++)
			{//211023
				for (int x = rtInsulRect.left , x1 = 0 ; x < rtInsulRect.right  && x >= 0; x++, x1++)
				{//220427 마진값 수정 35->5
					if (*(fmDark + nw * y + x) >= nInsulTh1 && *(fmDark + nw * y + x + 1) > nInsulTh1 && *(fmDark + nw * (y + 1) + x) >= nInsulTh1 && *(fmDark + nw * (y + 1) + x + 1) > nInsulTh1)
						*(fmInsul + (m_nInsulateBufferWidth[nCam] * y1) + x1) = 0xff;//0517-4

					if (x > rtInsulRect.left && x < rtInsulRect.right)
					{
						if (*(fm + nw * y + x) < nRightSum - m_nInsulDarkTh[nCam])
							*(fmInsulDark + (m_nInsulateBufferWidth[nCam] * y1) + x1) = 0xff;//0517-4
					}  //확인
				}
			}
		}
		else
		{//Dark Image
			for (int y = rtInsulRect.top , y1 = 0 ; y < rtInsulRect.bottom  && y >= 0; y++, y1++)
			{//211023
				for (int x = rtInsulRect.left , x1 = 0 ; x < rtInsulRect.right  && x >= 0; x++, x1++)
				{//220427 마진값 수정 35->5
					if (*(fmDark + nw * y + x) >= nInsulTh1 && *(fmDark + nw * y + x + 1) > nInsulTh1 && *(fmDark + nw * (y + 1) + x) >= nInsulTh1 && *(fmDark + nw * (y + 1) + x + 1) > nInsulTh1)
						*(fmInsul + (m_nInsulateBufferWidth[nCam] * y1) + x1) = 0xff;//0517-4
					if(x > rtInsulRect.left && x < rtInsulRect.right)
					{
						if (*(fmDark + nw * y + x) < nInsulAverage - m_nInsulDarkTh[nCam])
							*(fmInsulDark + (m_nInsulateBufferWidth[nCam] * y1) + x1) = 0xff;//0517-4
					}
				}
			}
			//주석
		}
	}
	else if(TRUE==InspTabInsul) //gap영역  ,,다크검사가 꺼졌을 때...  절연영역의 렉트는 따로 가져가야함
	{
		for (int y = rtInsulBrightlTabArea.top , y1 = 0 ; y < rtInsulBrightlTabArea.bottom  && y >= 0; y++, y1++)//211023
		{
			for (int x = rtInsulBrightlTabArea.left , x1 = 0 ; x < rtInsulBrightlTabArea.right  && x >= 0; x++, x1++)
			{////220427 마진값 수정 35->5//221201
				if (*(fmDark + nw * y + x) >= nInsulTh1 && *(fmDark + nw * y + x + 1) > nInsulTh1 && *(fmDark + nw * (y + 1) + x) >= nInsulTh1 && *(fmDark + nw * (y + 1) + x + 1) > nInsulTh1)
					*(fmInsul + (m_nInsulateBufferWidth[nCam] * y1) + x1) = 0xff;//0517-4
				if (x > rtInsulGap.left && x < rtInsulGap.right)
				{//221201
					if (*(fmDark + nw * y + x) > nInsulgapTh)
						*(fmInsulGap + (m_nInsulateBufferWidth[nCam] * y1) + x1) = 0xff;//0517-4
				}
			}
		}
	}
	else
	{
		for (int y = rtInsulRect.top, y1 = 0; y < rtInsulRect.bottom && y >= 0; y++, y1++)//211023
		{
			for (int x = rtInsulRect.left, x1 = 0; x < rtInsulRect.right && x >= 0; x++, x1++)
			{////220427 마진값 수정 35->5//221201
				if (*(fmDark + nw * y + x) >= nInsulTh1 && *(fmDark + nw * y + x + 1) > nInsulTh1 && *(fmDark + nw * (y + 1) + x) >= nInsulTh1 && *(fmDark + nw * (y + 1) + x + 1) > nInsulTh1)
					*(fmInsul + (m_nInsulateBufferWidth[nCam] * y1) + x1) = 0xff;//0517-4
			}
		}
	}

#ifdef TEMP_TEST
	if (m_bInsulateDarkInsp[nCam])
	{//다크 검사도 같이 한다.
		if (m_bSbrIsnadImage[nCam])
		{//Bright Image
			for (int y = rt11.top + nInsulSkipTop; y < rt11.bottom - nInsulSkipBottom && y >= 0; y++)
			{//211023
				for (int x = rt11.left + nInsulSkipLeft; x < rt11.right - nInsulSkipRight && x >= 0; x++)
				{//220427 마진값 수정 35->5
					if (*(fmDark + nw * y + x) >= nInsulTh1 && *(fmDark + nw * y + x + 1) > nInsulTh1 && *(fmDark + nw * (y + 1) + x) >= nInsulTh1 && *(fmDark + nw * (y + 1) + x + 1) > nInsulTh1)
						*(fmInsul + nw * y + x) = 0xff;//0517-4
					if (x < rt11.left + nInsulDarkRight && x > rt11.left + nInsulDarkLeft)
					{//220401
						if (*(fm + nw * y + x) < nRightSum - m_nInsulDarkTh[nCam])
							*(fmInsulDark + nw * y + x) = 0xff;//0517-4
					}
				}
			}
		}
		else
		{//Dark Image
			for (int y = rt11.top + nInsulSkipTop; y < rt11.bottom - nInsulSkipBottom && y >= 0; y++)
			{//211023
				for (int x = rt11.left + nInsulSkipLeft; x < rt11.right - nInsulSkipRight && x >= 0; x++)
				{//220427 마진값 수정 35->5
					if (*(fmDark + nw * y + x) >= nInsulTh1 && *(fmDark + nw * y + x + 1) > nInsulTh1 && *(fmDark + nw * (y + 1) + x) >= nInsulTh1 && *(fmDark + nw * (y + 1) + x + 1) > nInsulTh1)
						*(fmInsul + nw * y + x) = 0xff;//0517-4
					if (x < rt11.left + nInsulDarkRight && x > rt11.left + nInsulDarkLeft)
					{//220401
						if (*(fmDark + nw * y + x) < nInsulAverage - m_nInsulDarkTh[nCam])
							*(fmInsulDark + nw * y + x) = 0xff;//0517-4
					}
				}
			}
		}
	}
	else
	{
		for (int y = rt11.top + nInsulSkipTop; y < rt11.bottom - nInsulSkipBottom && y >= 0; y++)//211023
		{
			for (int x = rt11.left + nInsulSkipLeft; x < rt11.right - nInsulSkipRight && x >= 0; x++)
			{////220427 마진값 수정 35->5//221201
				if (*(fmDark + nw * y + x) >= nInsulTh1 && *(fmDark + nw * y + x + 1) > nInsulTh1 && *(fmDark + nw * (y + 1) + x) >= nInsulTh1 && *(fmDark + nw * (y + 1) + x + 1) > nInsulTh1)
					*(fmInsul + nw * y + x) = 0xff;//0517-4
				if (x > rtInsulGap.left && x < rtInsulGap.right)
				{//221201
					if (*(fmDark + nw * y + x) > nInsulgapTh)
						*(fmInsulGap + nw * y + x) = 0xff;//0517-4
				}
			}
		}
	}
#endif

	//InspInsulate 함수 오래 걸리는 부분 230413 윤여선

	m_dNewinspTime[nCam][3] = (GetPrecisionTime() - dStartTime) * 1000.0f;
	////////////////////////insul gap //221201
	double dTemp = 0, dBlobSize = 0, dcx1 = 0, dcy1 = 0;
	if (m_bInsulGapUse[nCam] && (TRUE== InspTabInsul)) //절연검사 조건
	{
		m_pChainInsul[nCam]->SetChainData(1, fmInsulGap, 2, 2, 4, 100000, m_nInsulateBufferWidth[nCam], nRectSizeY);
		int nBCount = m_pChainInsul[nCam]->FastChain(rtInsulGap.left - rtInsulRect.left, rtInsulGap.top + 6, rtInsulGap.right - rtInsulRect.left, rtInsulGap.bottom - 6);//2110117-1 //탭접힘 과검 발생으로 영역 조정  //230825 확인 필요
		for (int i = 0; i < nBCount; i++)//절연 불량
		{
			if (m_nInsulateDefectCount[nCam] >= MAX_HUGEDEFECT - 1)break;
			dTemp = m_pChainInsul[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp < 20)continue;
			if (dTemp >= dBlobSize)//최대 불량 사이즈.
			{
				nBlobNo1 = i;	dBlobSize = dTemp;//면적
			}

		}
		if (dBlobSize > 100)
		{
			m_dInsulSize[nCam][m_nInsulateDefectCount[nCam]] = 0;

			int nx1 = m_pChainInsul[nCam]->FindMaxX(nBlobNo1);
			int nx2 = m_pChainInsul[nCam]->FindMinX(nBlobNo1);
			int ny1 = m_pChainInsul[nCam]->FindMaxY(nBlobNo1);
			int ny2 = m_pChainInsul[nCam]->FindMinY(nBlobNo1);

			m_dInsulSizeX[nCam][m_nInsulateDefectCount[nCam]] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
			m_dInsulSizeY[nCam][m_nInsulateDefectCount[nCam]] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];

			m_dInsulSize[nCam][m_nInsulateDefectCount[nCam]] = 7002;
			m_pChainInsul[nCam]->Chain_Center(nBlobNo1, &dcx1, &dcy1);
			dcx1 += rtInsulGap.left;
			//dcy1 += rtInsulGap.top;
			BYTE* fminsuldefect = m_pInsulDefect[nCam][m_nInsulateDefectCount[nCam]];
			m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]].SetRect(dcx1 - BAD_WIDTH * 0.5, dcy1 - BAD_HEIGHT * 0.5, dcx1 + BAD_WIDTH * 0.5, dcy1 + BAD_HEIGHT * 0.5);
			m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]] = SetBoundary2(m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]], CRect(80, 0, nw - 100, nh));
			CRect rtScr = m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]];
			rtScr.left -= rtInsulGap.left;
			rtScr.right -= rtInsulGap.left;
			//rtScr.top -= rtInsulGap.top;
			//rtScr.bottom -= rtInsulGap.top;
			//double dx,dy;
			//m_pChainInsul[nCam]->FindCenterWithBoundary(i, &dx, &dy);
			double dmoment = m_pChainInsul[nCam]->FindMoment(nBlobNo1);

			if (m_bDefectBinary)
				ImageSelectCopy(fmInsul, fminsuldefect, m_nInsulateBufferWidth[nCam], nRectSizeY, rtScr, BAD_WIDTH, BAD_HEIGHT, 0, 0);
			else
				ImageSelectCopy(fmDark, fminsuldefect, nw, nh, m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]], BAD_WIDTH, BAD_HEIGHT, 0, 0);

			m_nInsulateDefectCount[nCam]++;




			delete [] fmInsul;
			delete [] fmInsulDark;
			delete [] fmInsulGap;//221201
			return 0;
		}
	}

	m_dNewinspTime[nCam][4] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	
	m_pChainInsul[nCam]->SetChainData(1, fmInsul, 2, 2, 4, 100000, m_nInsulateBufferWidth[nCam], nRectSizeY);//수정


	//20220916 SJM
	if (m_bIstest)
	{
		if(TRUE==InspTabInsul)
		{
			for (int y = rtInsulBrightlTabArea.top, y1 = 0; y < rtInsulBrightlTabArea.bottom; y++, y1++)
			{
				memcpy(m_BinaryInsulateImage[0] + (y * m_nWidth) + rtInsulBrightlTabArea.left, fmInsul + (y1 * m_nInsulateBufferWidth[nCam]), sizeof(BYTE) * m_nInsulateBufferWidth[nCam]);
			}
		}
		else
		{
			for (int y = rtInsulRect.top, y1 = 0; y < rtInsulRect.bottom; y++, y1++)
			{
				memcpy(m_BinaryInsulateImage[0] + (y * m_nWidth) + rtInsulRect.left, fmInsul + (y1 * m_nInsulateBufferWidth[nCam]), sizeof(BYTE) * m_nInsulateBufferWidth[nCam]);
			}

		}
	}
	//20220916 SJM
	//int nBCount = m_pChainInsul[nCam]->FastChain(rt11.left + 2, rt11.top + 6, rt11.right - 15, rt11.bottom - 6);//2110117-1 //탭접힘 과검 발생으로 영역 조정
	int nBCount = m_pChainInsul[nCam]->FastChain(2,  6, nRectSizeX-15, nRectSizeY-6);//2110117-1 //탭접힘 과검 발생으로 영역 조정

	for (int i = 0; i < nBCount; i++)//절연 불량
	{
		if (m_nInsulateDefectCount[nCam] >= MAX_HUGEDEFECT - 1)break;
		dTemp = m_pChainInsul[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp < 4)continue;
		if (dTemp >= dBlobSize)//최대 불량 사이즈.
		{
			nBlobNo1 = i;	dBlobSize = dTemp;//면적
		}
		//여기 수정
		if (dTemp >= 6)
		{
			m_dInsulSize[nCam][m_nInsulateDefectCount[nCam]] = 0;
			int nx1 = m_pChainInsul[nCam]->FindMaxX(i);
			int nx2 = m_pChainInsul[nCam]->FindMinX(i);
			int ny1 = m_pChainInsul[nCam]->FindMaxY(i);
			int ny2 = m_pChainInsul[nCam]->FindMinY(i);
			m_dInsulSizeX[nCam][m_nInsulateDefectCount[nCam]] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
			m_dInsulSizeY[nCam][m_nInsulateDefectCount[nCam]] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];

			if (m_dInsulSizeX[nCam][m_nInsulateDefectCount[nCam]] > m_dInsulSizeY[nCam][m_nInsulateDefectCount[nCam]])m_dInsulSize[nCam][m_nInsulateDefectCount[nCam]] = m_dInsulSizeX[nCam][m_nInsulateDefectCount[nCam]];
			else m_dInsulSize[nCam][m_nInsulateDefectCount[nCam]] = m_dInsulSizeY[nCam][m_nInsulateDefectCount[nCam]];
			m_pChainInsul[nCam]->Chain_Center(i, &dcx1, &dcy1);
			dcx1 += rtInsulRect.left;
			dcy1 += rtInsulRect.top;
			BYTE* fminsuldefect = m_pInsulDefect[nCam][m_nInsulateDefectCount[nCam]];
			m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]].SetRect(dcx1 - BAD_WIDTH * 0.5, dcy1 - BAD_HEIGHT * 0.5, dcx1 + BAD_WIDTH * 0.5, dcy1 + BAD_HEIGHT * 0.5);
			m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]] = SetBoundary2(m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]], CRect(80, 0, nw - 100, nh));
			CRect rtScr = m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]];
			rtScr.left -= rtInsulRect.left;
			rtScr.right -= rtInsulRect.left;
			//rtScr.top -= rt11.top;
			//rtScr.bottom -= rt11.top;

			if (m_bDefectBinary)
				ImageSelectCopy(fmInsul, fminsuldefect, m_nInsulateBufferWidth[nCam], nRectSizeY, rtScr, BAD_WIDTH, BAD_HEIGHT, 0, 0);
			else
				ImageSelectCopy(fmDark, fminsuldefect, nw, nh, m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]], BAD_WIDTH, BAD_HEIGHT, 0, 0);

			m_nInsulateDefectCount[nCam]++;
			if (m_nInsulateDefectCount[nCam] >= MAX_HUGEDEFECT - 1)break;
		}
	}

	m_dNewinspTime[nCam][5] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	if (m_bInsulateDarkInsp[nCam])
	{


		//m_pChainInsul[nCam]->SetChainData(1, m_fmInsulDark[nCam], 2, 2, 4, 100000, m_nInsulateBufferWidth[nCam], nRectSizeY); //수정
		m_pChainInsul[nCam]->SetChainData(1, fmInsulDark, 2, 2, 4, 100000, m_nInsulateBufferWidth[nCam], nRectSizeY); //수정



		//20220916 SJM
		if (m_bIstest)
		{
				for (int y = rtInsulRect.top, y1 = 0; y < rtInsulRect.bottom; y++, y1++)
					memcpy(m_BinaryInsulateImage[1] + (y * m_nWidth) + rtInsulRect.left, fmInsulDark + (y1 * m_nInsulateBufferWidth[nCam]), sizeof(BYTE) * m_nInsulateBufferWidth[nCam]);
		}
		//20220916 SJM
	

		//int nBCount1 = m_pChainInsul[nCam]->FastChain(2, rt11.top + 6, inspareawidth, rt11.bottom - 6);
		int nBCount1 = m_pChainInsul[nCam]->FastChain(2, 6, m_nInsulateBufferWidth[nCam], nRectSizeY-6);
		for (int i = 0; i < nBCount1; i++)//절연 불량
		{
			if (m_nInsulateDefectCount[nCam] >= MAX_HUGEDEFECT - 1)break;
			dTemp = m_pChainInsul[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			//if (dTemp < 30)continue;//1 x 1 기준
			//if (dTemp >= dBlobSize)//최대 불량 사이즈.
			//{
			nBlobNo1 = i;	dBlobSize = dTemp;//면적
			//}

			if (dBlobSize >= 30 && m_nInsulateDefectCount[nCam] < MAX_HUGEDEFECT - 1) //불량 최소사이즈 30 넘어야 판정함.
			{
				int nx1 = m_pChainInsul[nCam]->FindMaxX(nBlobNo1);
				int nx2 = m_pChainInsul[nCam]->FindMinX(nBlobNo1);
				int ny1 = m_pChainInsul[nCam]->FindMaxY(nBlobNo1);
				int ny2 = m_pChainInsul[nCam]->FindMinY(nBlobNo1);
				m_dInsulSizeX[nCam][m_nInsulateDefectCount[nCam]] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				m_dInsulSizeY[nCam][m_nInsulateDefectCount[nCam]] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];

				m_dInsulSize[nCam][m_nInsulateDefectCount[nCam]] = -10;
				//if (m_dInsulSizeX[nCam][m_nInsulateDefectCount[nCam]] > m_dInsulSizeY[nCam][m_nInsulateDefectCount[nCam]])m_dInsulSize[nCam][m_nInsulateDefectCount[nCam]] = m_dInsulSizeX[nCam][m_nInsulateDefectCount[nCam]];
				//else m_dInsulSize[nCam][m_nInsulateDefectCount[nCam]] = m_dInsulSizeY[nCam][m_nInsulateDefectCount[nCam]];
				m_pChainInsul[nCam]->Chain_Center(nBlobNo1, &dcx1, &dcy1);
				dcx1 += rtInsulRect.left;
				dcy1 += rtInsulRect.top;
				//dcy1 += rt11.top;
				m_dInsulCompaceness[nCam][m_nInsulateDefectCount[nCam]] = m_pChainInsul[nCam]->FindCompactness(nBlobNo1);		//0white 1black //0803
				if (CIS_Site::ESWA == m_enSite && m_dInsulCompact[nCam] < m_dInsulCompaceness[nCam][m_nInsulateDefectCount[nCam]] * 100)
					continue;
				double dmindist = 0, dmaxdist = 0, davedist = 0;
				m_pChainInsul[nCam]->FineDistFromPoint(nBlobNo1, dcx1, dcy1, &dmindist, &dmaxdist, &davedist);//장축 단축 구하기  //211008
				m_dInsulMinMax[nCam][m_nInsulateDefectCount[nCam]][0] = dmindist;			//0white 1black //0803
				m_dInsulMinMax[nCam][m_nInsulateDefectCount[nCam]][1] = dmaxdist;			//0white 1black //0803

				BYTE* fminsuldefect = m_pInsulDefect[nCam][m_nInsulateDefectCount[nCam]];
				m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]].SetRect(dcx1 - BAD_WIDTH * 0.5, dcy1 - BAD_HEIGHT * 0.5, dcx1 + BAD_WIDTH * 0.5, dcy1 + BAD_HEIGHT * 0.5);
				m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]] = SetBoundary2(m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]], CRect(80, 0, nw - 100, nh));
				CRect rtScr = m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]];
				rtScr.left -= rtInsulRect.left;
				rtScr.right -= rtInsulRect.left;
				//rtScr.top -= rt11.top;
				//rtScr.bottom -= rt11.top;

				if (m_bDefectBinary)
					ImageSelectCopy(fmInsulDark, fminsuldefect, m_nInsulateBufferWidth[nCam], nRectSizeY, rtScr, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(fmDark, fminsuldefect, nw, nh, m_rtInsulateDefect[nCam][m_nInsulateDefectCount[nCam]], BAD_WIDTH, BAD_HEIGHT, 0, 0);

				m_nInsulateDefectCount[nCam]++;
			}
		}
	}

	m_dNewinspTime[nCam][6] = (GetPrecisionTime() - dStartTime) * 1000.0f;


	delete [] fmInsul;
	delete [] fmInsulDark;
	delete [] fmInsulGap;//221201
	return 0;
}
int CInspect::InspInsulateWidth(int nCam, BYTE* fm, BYTE* fmDark, int nw, int nh, CRect rttabins, int nCoatEdge1, int nEndPos, int nOverlay, int nt1, int nt2, int* nRightS, int * nRightS1)//절연 폭, 오버레이 폭 측정 함수
{
	int nInsulate[20000] = { 0, }/*ROI 전체의 X좌표별 Y의 토탈 평균 밝기*/, nInsulate11[20000] = {0,}/*nInsulate하고 똑같은 역할하는거 같은데 왜 하나 더 선언했는지....*/, nTabSum1 = 0, nTabsumcnt = 0, nsumcnt = 0, nTmp = 0;//0516
	int nInsulateSplit[10][1000];//0604 //ROI를 10등분해서 각 영역별 X좌표의 평균 밝기를 가지고 있음

	memset(nInsulateSplit, 0, sizeof(int) * 10 * 1000);

	if (TRUE == m_bInspManual)
	{
		memset(m_nInsulateRoi10eaSplit, 0, sizeof(int) * 10 * 1000);
	}

	CRect rttantmp = rttabins;
	int nGap11 = 200;//220719
	if (rttabins.top < 0)rttabins.top = 0;
	if (rttabins.bottom >= nh)rttabins.bottom = nh - 1;

	m_vecPeakEdge[nCam].clear();

	if(nCoatEdge1 + 60 > 0 && nCoatEdge1 + 60 < m_nWidth && nCoatEdge1 + nGap11 > 0 && nCoatEdge1 + nGap11 < m_nWidth)
		m_insulateROI[nCam] = CRect(nCoatEdge1 + 60, rttabins.top, nCoatEdge1 + nGap11, rttabins.bottom);

	for (int y = rttabins.top; y < rttabins.bottom && y >= 0; y++)//탭 중앙에 끝부분을 찾는다.
	{
		//		for (int x = nCoatEdge1 + 20; x<nEndPos - 150 && x>0 && x < m_nWidth - 20; x++){//211011
		for (int x = nCoatEdge1 + 60; x < nCoatEdge1 + nGap11 && x >= 0 && x < m_nWidth - 20; x++)
		{//211011
			nInsulate[x] += *(fmDark + nw*y + x);
			nTabSum1 += *(fmDark + nw*y + x); nTabsumcnt++;
		}
	}
	if (nTabsumcnt == 0)nTabsumcnt = 1;//211017-1
	if (nTabsumcnt)nTabSum1 /= nTabsumcnt;
	int nCut1 = nTabSum1, nCut2 = 255 - nTabSum1, ntmp2 = 0, nSum2 = 0;
	//평균 밝기를 기준으로 equalization을 실시.
	int nSlipcnt = rttabins.Height() / 10, nSlipcnt1 = 0;//0604
	int nInsulsplitWidth = (nCoatEdge1 + nGap11) - (nCoatEdge1 + 60);//0604(nEndPos - 150) - (nCoatEdge1 + 20);//0604 //211011
	int nInsulRatio = 4;//0727
	if (nInsulsplitWidth >= 1000)
	{
		return 0;//0604
	}
	if (nSlipcnt < 1)nSlipcnt = 1;//211017-1
	for (int k = 0; k < 1; k++) 
	{ //이미지 전처리
		nsumcnt = 0;

		if(m_nInsulTh[nCam]>0)
		{
			for (int y = rttabins.top; y < rttabins.bottom && y >= 0; y++) 
			{//탭 중앙에 끝부분을 찾는다.

				if (nSlipcnt1 >= 10 && nSlipcnt1/*for문 수행한 횟수*/ / nSlipcnt/*ROI 높이의 10분의 1*/ >= 10) //for문을 10번 이상 수행하고 횟수에서 ROI 10분의 1 값을 나눈 값이 10 이상이라면 
					//근데 이러면 다 돌라는 얘기 같은데 왜 이렇게 짜놨을까
					break;//220513

				for (int x = nCoatEdge1 + 60; x < nCoatEdge1 + nGap11 && x >= 0 && x < m_nWidth - 20; x++) 
				{//220109
					nTmp = *(fmDark + y*nw + x);// - nTabSum1)*nInsulRatio + m_nTargetBrightness[nCam]; //0727
					*(m_fmTabEdge[nCam] + y*nw + x) = nTmp;//타겟 영상에 값 넣기
					nInsulate11[x] += nTmp;

					if (x - (nCoatEdge1 + 60) < 1000 && x - (nCoatEdge1 + 60) >= 0)//220513
					{
						nInsulateSplit[nSlipcnt1 / nSlipcnt][x - (nCoatEdge1 + 60)/*이거는 for문이 이 값부터 시작해서 0번 인덱스부터 넣으려고*/] += nTmp;//220109 //ROI 퍼센티지 영역별로 평균 값 주려는듯 하위 10, 20, 30, 40, 50, 60, 70, 80. 90. 100 이렇게 나누는듯
					}
				}
				nSlipcnt1++;//0604
			}
		}
		else
		{
			for (int y = rttabins.top; y < rttabins.bottom && y >= 0; y++)
			{//탭 중앙에 끝부분을 찾는다.
				if (nSlipcnt1 >= 10 && nSlipcnt1 / nSlipcnt>=10)break;//220513

				for (int x = nCoatEdge1 + 60; x < nCoatEdge1 + nGap11 && x >= 0 && x < m_nWidth - 20; x++)
				{//220109
					nTmp = (*(fmDark + y*nw + x) - nTabSum1)*nInsulRatio + m_nTargetBrightness[nCam]; //0727
					if (nTmp < 50)nTmp = 128;
					if (nTmp > 255)nTmp = 255;
					*(m_fmTabEdge[nCam] + y*nw + x) = nTmp;//타겟 영상에 값 넣기
					nInsulate11[x] += nTmp;
					if (x - (nCoatEdge1 + 60) < 1000 && x - (nCoatEdge1 + 60) >= 0)//220513
					{
						nInsulateSplit[nSlipcnt1 / nSlipcnt][x - (nCoatEdge1 + 60)] += nTmp;//220109
					}
				}
				nSlipcnt1++;//0604
			}
		}

	}

	for (int i = 0; i < 10; i++)
	{//0604
		for (int x = 0; x < nInsulsplitWidth && x<1000 && nSlipcnt>0; x++) 
		{
			nInsulateSplit[i][x] /= nSlipcnt;
		}
	}
	int nRightSum = 0, nRightSum1 = 0, nTabSum = 0, nRightSumcnt = 0;
	for (int x = nCoatEdge1 + 60; x < nCoatEdge1 + nGap11 && x >= 0 && x < m_nWidth - 20; x++) 
	{//220109
		if (rttabins.Height() > 0) 
		{
			nInsulate[x] /= rttabins.Height();
			nInsulate11[x] /= rttabins.Height();
		}
		if (x > nCoatEdge1 + nGap11 - 10) 
		{
			nRightSum += nInsulate[x];
			nRightSumcnt++;
		}
	}
	if (nRightSumcnt == 0)nRightSumcnt = 1;
	nRightSum /= nRightSumcnt;
	nRightSumcnt = 0;
	for (int x = nCoatEdge1 + 60; x < nCoatEdge1 + nGap11 && x >= 0 && x < m_nWidth - 20; x++) 
	{//220109
		if (x > nCoatEdge1 + 120 - 10 && nInsulate[x] >= nRightSum) 
		{
			nRightSum1 += nInsulate[x];
			nRightSumcnt++;
		}
	}

	if (nRightSumcnt == 0)
		nRightSumcnt = 1;

	nRightSum1 /= nRightSumcnt;
	*nRightS = nRightSum;
	*nRightS1 = nRightSum1;
	int nInsulTh = 8, nInsulate1 = 0, ncc1 = 0;
	if (1) 
	{//0516
		double dPeakPos[10] = { 0. };
		nInsulTh = 40;
		int nPeakEdge = 0, nPeakEdge1 = 0, n2ndEdge = 0, n2ndEdge1 = 0;//0622-2
		double dPosSum = 0;
		nPeakEdge = 0, nPeakEdge1 = 0;
		int nOverlayOffset = 16;//0623
		for (int x = nCoatEdge1 + nGap11; x > nCoatEdge1 + 60 && x >= 0 && x < m_nWidth - 20; x--) 
		{//전체 데이터//220109
			if (nInsulate11[x] - nInsulate11[x - nOverlayOffset] > nPeakEdge) 
			{
				nPeakEdge = nInsulate11[x] - nInsulate11[x - nOverlayOffset]; nInsulate1 = x - nOverlayOffset + 8;//0516
			}
			else if (nInsulate11[x] - nInsulate11[x - nOverlayOffset] > n2ndEdge && nInsulate11[x] - nInsulate11[x - nOverlayOffset] < nPeakEdge) 
			{//0622-2
				n2ndEdge = nInsulate11[x] - nInsulate11[x - nOverlayOffset]; n2ndEdge1 = x - nOverlayOffset + 8;//0516
			}
		}
		dPosSum = nInsulate1;
		if(m_nInsulTh[nCam]>0)
		{
			for (int i = 0; i < 10; i++) 
			{//분할데이터//
				nPeakEdge = 0, nPeakEdge1 = 0;
				//			for (int x = nCoatEdge1 + nGap11; x > nCoatEdge1 + 60 && x >= 0 && x < m_nWidth - 20; x--) {//전체 데이터//220109
				for (int x = nInsulsplitWidth-4; x > 8 && x < 1000; x--) 
				{// (nEndPos-150)-(nCoatEdge1+20);//211017-1
					if (nInsulateSplit[i][x] - nInsulateSplit[i][x - 8] > nPeakEdge) 
					{
						nPeakEdge = nInsulateSplit[i][x] - nInsulateSplit[i][x - 8]; dPeakPos[i] = x - 4 + (nCoatEdge1 + 60);////220109
					}
					if(nPeakEdge>m_nInsulTh[nCam] && x< 1000 - 10)
					{//220513
						int nPeak12 = 0;
						for(int x2 = x;x2>x-8 && x2>8;x2--)
						{
							if( nInsulateSplit[i][x2] - nInsulateSplit[i][x2 - 2] > nPeak12)
							{
								nPeak12 =nInsulateSplit[i][x2] - nInsulateSplit[i][x2 - 2];
								dPeakPos[i] = x2 - 2 + (nCoatEdge1 + 60);
							}
						}
						break;
					}
				}

				if (m_bInspManual)
				{
					m_vecPeakEdge[nCam].push_back(nPeakEdge);
				}
			}
		}
		else
		{
			for (int i = 0; i < 10; i++) 
			{//분할데이터//
				nPeakEdge = 0, nPeakEdge1 = 0;
				//			for (int x = nCoatEdge1 + nGap11; x > nCoatEdge1 + 60 && x >= 0 && x < m_nWidth - 20; x--) {//전체 데이터//220109
				for (int x = nInsulsplitWidth-4; x > 8 && x < 1000; x--) 
				{// (nEndPos-150)-(nCoatEdge1+20);//211017-1
					if (nInsulateSplit[i][x] - nInsulateSplit[i][x - 8] > nPeakEdge) 
					{
						nPeakEdge = nInsulateSplit[i][x] - nInsulateSplit[i][x - 8]; dPeakPos[i] = x - 4 + (nCoatEdge1 + 60);////220109
					}
				}

				if (m_bInspManual)
				{
					m_vecPeakEdge[nCam].push_back(nPeakEdge);
				}
			}
		}
		double dSortData[10] = { 0, }, dPosSum1 = nInsulate1, dPosSum2 = 0, dPosSum3 = 0, dposSum4 = 0;//0623
		int ninsulcount12 = 1;

		for (int i = 0; i < 10; i++)
		{
			dSortData[i] = dPeakPos[i];
		}

		if (m_bInspManual)
		{
			for (int i = 0; i < 10; i++)
			{
				m_dInsulWidthCandidate[nCam][i] = (dSortData[i] - nCoatEdge1) * m_dscale_x1[nCam];
			}

			for (int i = 0; i < 10; i++)
			{
				m_dInsulateSortData[nCam][i] = dSortData[i];
			}

			memcpy(m_nInsulateRoi10eaSplit, nInsulateSplit, sizeof(int) * 10 * 1000);
		}

		sort(dSortData, dSortData + 10);

		///////////////0623
		for (int i = 3; i < 7; i++)
		{
			dPosSum1 += dSortData[i];
		}
		dPosSum1 /= 5;//Insulate 중간값
		for (int i = 3; i <= 4; i++) 
		{
			dPosSum2 += dSortData[i];
		}
		dPosSum2 /= 2;//Insulate 중간값
		for (int i = 5; i < 7; i++) 
		{//0623
			dPosSum3 += dSortData[i];
		}
		dPosSum3 /= 2;//Insulate 중간값//0623
					  /////////////////////////////
		for (int i = 0; i < 10; i++)
		{
			if (dPosSum1 + 2 > dSortData[i] && dPosSum1 - 2 < dSortData[i]) 
			{
				dPosSum += dSortData[i]; ninsulcount12++;
			}
		}
		if (ninsulcount12 > 2)
			dPosSum /= (ninsulcount12);
		else 
		{
			double dv1 = (dPosSum2 - nCoatEdge1)*m_dscale_x1[nCam];//0607-1
			double dv2 = (dPosSum3 - nCoatEdge1)*m_dscale_x1[nCam];//0607-1
			if (fabs(dv1 - 3.5) < fabs(dv2 - 3.5))
				dPosSum = dPosSum2;
			else
				dPosSum = dPosSum3;
		}

		double d21 = rand() % 10;//0607-1
		d21 *= 0.01;//0607-1
		if (dPosSum - nCoatEdge1 <= 80)dPosSum += 4;//0612
		if (dPosSum - nCoatEdge1 >= 90)dPosSum -= 4;//0612
		double dtmpwidth = (n2ndEdge1 - nCoatEdge1 + d21)*m_dscale_x1[nCam];//0622-2-1
		double dtmpwidth1 = (dPosSum3 - nCoatEdge1 + d21)*m_dscale_x1[nCam];//0623
		double dtmpwidth2 = (dPosSum1 - nCoatEdge1 + d21)*m_dscale_x1[nCam];//0623
		double dtmpwidth3 = (dPosSum2 - nCoatEdge1 + d21)*m_dscale_x1[nCam];//0623
		m_dInsulWidth[nCam] = (dPosSum - nCoatEdge1 + d21)*m_dscale_x1[nCam];//0607-1
		if(!m_bInsulAdjust[nCam]){//221116 //절연보정 미사용
			m_rtInsulate[nCam].SetRect(nCoatEdge1, nt1, dPosSum, nt2);
			m_rtInsulate[nCam] = SetBoundary2(m_rtInsulate[nCam], CRect(80, 0, nw - 100, nh));

			return 0;
		}
//		if (m_nCellSize == 0) 
		{
//			if (m_dInsulWidth[nCam] > 4.2 && dtmpwidth < 4.2 &&  n2ndEdge > 40 && dtmpwidth > 3.3)m_dInsulWidth[nCam] = dtmpwidth;//0622-2
//			else if (m_dInsulWidth[nCam] > 4.2 && dtmpwidth1<4.2 && dtmpwidth1>3.3)m_dInsulWidth[nCam] = dtmpwidth1;//0623
//			else if (m_dInsulWidth[nCam] > 4.2 && dtmpwidth2<4.2 && dtmpwidth2>3.3)m_dInsulWidth[nCam] = dtmpwidth2;//0623
//			else if (m_dInsulWidth[nCam] > 4.2 && dtmpwidth3<4.2 && dtmpwidth3>3.3)m_dInsulWidth[nCam] = dtmpwidth3;//0623
////			if(m_dInsulWidth[nCam]+m_dOverlayWidth[nCam]>=4.5)m_dOverlayWidth[nCam] = 4.5-m_dInsulWidth[nCam];//0612 //0805
//			if (m_bOverlayInsp && m_dInsulWidth[nCam] < 4.2 && m_dOverlayWidth[nCam] < 0.1)m_dOverlayWidth[nCam] = m_dOldOverlayWidth[nCam];//220223 오버레이검사를 안할때는 보정을 안한다.
			//			if(m_dInsulWidth[nCam]>4.1 && m_dOverlayWidth[nCam]>0.1)m_dOverlayWidth[nCam] = 4.5-m_dInsulWidth[nCam];//0612 //0805
			BOOL bSkip1 = FALSE;
			double dmasterInsulate = m_dOverlaySize[nCam] + m_dInsulationSize[nCam];//4.5;;//211119
			double dOverlaySpec = m_dOverlaySize[nCam] + m_dOverlayPlusTolerance[nCam];// 1.1;//211119
			int nInsulSpecOver = (m_dInsulationSize[nCam] + m_dInsulationPlusTolerance[nCam])/m_dscale_x1[nCam];//220330

			double doldinsulgap1 = 1000;
			for (int i = 0; i < 10; i++) 
			{//Spec 중앙 값에 가까운 오버레이를 쓴다.//211119
				double dinsulgap1 =  fabs(m_dInsulationSize[nCam] - (dSortData[i]-nCoatEdge1)*m_dscale_x1[nCam]) ;

				if(doldinsulgap1 > dinsulgap1)
				{
					doldinsulgap1 = dinsulgap1;
					m_dInsulWidth[nCam] = (dSortData[i] - nCoatEdge1) * m_dscale_x1[nCam];
					m_dResSortData[nCam] = dSortData[i];
					dPosSum = nCoatEdge1 + m_dInsulWidth[nCam] / m_dscale_x1[nCam];//
				}
			}

			if(m_bOverlayCorrection[nCam])
			{//220721
				if (m_dInsulWidth[nCam] >= m_dInsulationSize[nCam] && m_dOverlayWidth[nCam] >= dOverlaySpec && m_bOverlayInsp) 
				{//220723
					m_dOverlayWidth[nCam] = m_dOverlaySize[nCam];//0612

					double d23 = rand() % 8;

					if (int(d23) % 2 == 0)d23 *= -0.001;
					else                  d23 *= 0.001;

					m_dOverlayWidth[nCam] += d23;
					m_dInsulWidth[nCam] += d23;

				}

				if(m_bOverlayInsp)
				{//220109
					if (m_dInsulWidth[nCam] > m_dInsulationSize[nCam] + m_dInsulationPlusTolerance[nCam]&& m_dOverlayWidth[nCam] > 0.1 && m_dOverlayWidth[nCam] < dOverlaySpec) {//오버레이 양품  //220112
						{//오버레이가양품이면절연을양품으로치환한다.
							m_dInsulWidth[nCam] = dmasterInsulate - m_dOverlayWidth[nCam];
							m_rtInsulate[nCam].right = nCoatEdge1 + m_dInsulWidth[nCam] / m_dscale_x1[nCam];
							dPosSum = nCoatEdge1 + m_dInsulWidth[nCam] / m_dscale_x1[nCam];//0805 여기에요..
							bSkip1 = TRUE;
						}
						//else
						//	m_dOverlayWidth[nCam] = dmasterInsulate - m_dInsulWidth[nCam];//0612
					}
					if (m_dInsulWidth[nCam]<m_dInsulationSize[nCam] - m_dInsulationMinusTolerance[nCam] && m_dOverlayWidth[nCam]>0.1 && m_dOverlayWidth[nCam] < dOverlaySpec) 
					{//0805
						m_dInsulWidth[nCam] = dmasterInsulate - m_dOverlayWidth[nCam];//0612
						m_rtInsulate[nCam].right = nCoatEdge1 + m_dInsulWidth[nCam] / m_dscale_x1[nCam];
						dPosSum = nCoatEdge1 + m_dInsulWidth[nCam] / m_dscale_x1[nCam];//0805
						bSkip1 = TRUE;
					}
					if (m_dOverlayWidth[nCam] > dOverlaySpec && m_dInsulWidth[nCam] + m_dOverlayWidth[nCam] >= dmasterInsulate)
					{//0925-2
						if (m_dInsulWidth[nCam] > m_dInsulationSize[nCam] - m_dInsulationMinusTolerance[nCam]  && m_dInsulWidth[nCam]<m_dInsulationSize[nCam] + m_dInsulationPlusTolerance[nCam]  && m_dOverlayWidth[nCam]>dOverlaySpec)
							m_dOverlayWidth[nCam] = dmasterInsulate- m_dInsulWidth[nCam];

						m_rtOverlay[nCam].left = nCoatEdge1 - m_dOverlayWidth[nCam] /m_dscale_x1[nCam];
					}
					if(m_dOverlayWidth[nCam] <0.1 && m_dInsulWidth[nCam]>= m_dInsulationSize[nCam] - m_dInsulationMinusTolerance[nCam] &&  m_dInsulWidth[nCam]<= m_dInsulationSize[nCam] + m_dInsulationPlusTolerance[nCam])
					{
						//절연 양품 오버레이 불량
						m_dOverlayWidth[nCam] = dmasterInsulate - m_dInsulWidth[nCam];
						m_rtOverlay[nCam].left = nCoatEdge1 - m_dOverlayWidth[nCam] /m_dscale_x1[nCam];
					}

					if(dPosSum <= 0)
					{//211119
						//오버레이가 양품인 경우
						double dtmp22 = dmasterInsulate - m_dOverlayWidth[nCam];
						m_dInsulWidth[nCam] = dtmp22;
						dPosSum = nCoatEdge1 + m_dInsulWidth[nCam] / m_dscale_x1[nCam];//0805
					}
				}
			}


			//211119 - 둘다 NG일 경우 Size Spec에 맞게 위치 보정
			double d21 = rand() % 6;
			if (int(d21) % 2 == 0)d21 *= -0.001;
			else                  d21 *= 0.001;
			m_dBottomOverlay[nCam][0] += d21;
			m_dBottomInsul[nCam][0] += d21;

			nInsulate1 = dPosSum;//0625-1
			m_dOldInsulWidth[nCam] = m_dInsulWidth[nCam];//0612
			m_dOldOverlayWidth[nCam] = m_dOverlayWidth[nCam];//0612

			if (bSkip1 == FALSE) 
			{
				if (nCoatEdge1 - nOverlay > 0 && nInsulate1 - nCoatEdge1 > nInsulSpecOver) 
				{//0517-3
					nPeakEdge = 0;
					for (int x = nInsulate1+10; x > nInsulate1 - 40 && x > 0 && x < m_nWidth - 20; x--)
					{//220329
						if (nInsulate11[x] - nInsulate11[x - 8] > nPeakEdge) 
						{
							nPeakEdge = nInsulate11[x] - nInsulate11[x - 8]; nInsulate1 = x - 4;//0516
						}
					}
				}

				if (nCoatEdge1 - nOverlay > 0 && (nInsulate1 - nCoatEdge1)*m_dscale_x1[nCam] > nInsulSpecOver)
					nInsulate1 = nCoatEdge1 + (m_dInsulationSize[nCam] / m_dscale_x1[nCam]);
			}

			if (int(d21) % 2 == 0)	d21 *= -0.001;//0925
			else				d21 *= 0.001;//0925

			m_dOverlayWidth[nCam] += d21;
			m_dInsulWidth[nCam] += d21;
		}
		//else {
		//	nInsulate1 = dPosSum;//0625-1
		//	m_dOldInsulWidth[nCam] = m_dInsulWidth[nCam];//0612
		//	m_dOldOverlayWidth[nCam] = m_dOverlayWidth[nCam];//0612

		//}
		m_rtInsulate[nCam].SetRect(nCoatEdge1, nt1, nInsulate1, nt2);
		m_rtInsulate[nCam] = SetBoundary2(m_rtInsulate[nCam], CRect(80, 0, nw - 100, nh));
		for (int x = nInsulate1 + 10; x < nEndPos - 150 && x >= 0 && x < m_nWidth - 20; x++)
		{
			nTabSum += nInsulate11[x]; ncc1++;
		}
	}
	else 
	{
		for (int x = nEndPos - 120; x > nCoatEdge1 + 40 && x >= 0 && x < m_nWidth - 20; x--)
		{
			if (nInsulate[x] - nInsulate[x - 8] > nInsulTh && nInsulate[x - 8] < nRightSum) 
			{
				nInsulate1 = x - 8; break;
			}
		}
		m_rtInsulate[nCam].SetRect(nCoatEdge1, nt1, nInsulate1, nt2);
		m_rtInsulate[nCam] = SetBoundary2(m_rtInsulate[nCam], CRect(80, 0, nw - 100, nh));
		for (int x = nInsulate1 + 10; x < nEndPos - 150 && x >= 0 && x < m_nWidth - 20; x++)
		{
			nTabSum += nInsulate[x]; ncc1++;
		}
	}
	if (ncc1 < 1)ncc1 = 1;
	nTabSum /= ncc1;
	return 0;
}
#ifdef ORIGIN_PROCESS
int CInspect::FindSpur(int nCam, BYTE* fm, int nw, int  nh, CRect rtspur, int nTh)	// 탭 안쪽 부위 미코팅 검사
{
	if (m_bWhiteRoll[nCam])//////220128
		fm = m_fmOdd[nCam];
	CRect rt, rtTabIn;
	rt = rtspur;
	rtTabIn = rt;
	int nDefect = m_nIslandCount[nCam];//
	if (nDefect >= MAX_HUGEDEFECT - 1)//211017-1
		return nDefect;
	BYTE* fmtmp = new BYTE[nw * nh];
	memset(fmtmp, 0, nw * nh);
	BYTE* fmBlack = new BYTE[nw * nh];////220128
	memset(fmBlack, 0, nw * nh);
	int nAve = 0, nc1 = 0, nProj1 = 0, nright = rt.right;

	//Testing 
	rt.InflateRect(0, m_nSpurRangeTopBtm[nCam], 0, m_nSpurRangeTopBtm[nCam]);
	rt = SetBoundary2(rt, CRect(10, 0, nw - 100, nh));
	m_rtFullInspRect[nCam][3] = rt;
	rtTabIn.InflateRect(0, -10);
	rtTabIn = SetBoundary2(rtTabIn, CRect(10, 0, nw - 100, nh));

	for (int y = rt.top; y < rt.bottom && y >= 0; y++) {
		nProj1 = 0;
		for (int x = rt.right; x > rt.left && x >= 0; x--) {
			nAve += *(fm + nw * y + x);
			nc1++;
		}
	}
	if (nc1 == 0)nc1 = 1;
	nAve /= nc1;
	//어깨선 모서리 부분 Skip 하도록 추가 필요 //211120
	for (int y = rt.top; y < rt.bottom && y >= 0; y++) {
		for (int x = rt.left; x < rt.right && x >= 0; x++) {
			//모서리 부분 스킵
			if (y < m_rtCellTab[nCam].top + 8 && x>m_rtCell[nCam].right - 4)//211123
				continue;
			if (y > m_rtCellTab[nCam].bottom - 8 && x > m_rtCell[nCam].right - 4)//211123
				continue;
			if (nAve + nTh < *(fm + nw * y + x)) {
				*(fmtmp + nw * y + x) = 0xff;
			}
		}
	}
	int nTh1 = m_nCrackValue[nCam];//40
	for (int y = rtTabIn.top; y < rtTabIn.bottom && y >= 0; y++) {
		for (int x = rtTabIn.left; x < rtTabIn.right && x >= 0; x++) {
			//모서리 부분 스킵
			if (y < m_rtCellTab[nCam].top + 4 && x>m_rtCell[nCam].right - 4)//211123
				continue;
			if (y > m_rtCellTab[nCam].bottom - 4 && x > m_rtCell[nCam].right - 4)//211123
				continue;
			if (nAve - nTh1 > *(fm + nw * y + x)) {
				*(fmBlack + nw * y + x) = 0xff;
			}
		}
	}

	//IPVM::Image_8u_C1 blackImage;
	//blackImage.Create(nw, nh);
	//memcpy(blackImage.GetMem(), fmBlack, sizeof(BYTE) * nw * nh);
	//blackImage.SaveBmp(_T("Black_Image.bmp"));

	//20220916 SJM
	if (m_bIstest) {
		memcpy(m_BinarySpurImage[0], fmtmp, sizeof(BYTE) * m_nWidth * m_nHeight);
		memcpy(m_BinarySpurImage[1], fmBlack, sizeof(BYTE) * m_nWidth * m_nHeight);
	}
	//20220916 SJM
	int nw2 = BAD_WIDTH * 0.5, nh2 = BAD_HEIGHT * 0.5;
	m_pChainIsland[nCam]->SetChainData(1, fmtmp, 1, 1, 4, 100000, nw, nh);//white
	int nBlobCount14 = m_pChainIsland[nCam]->FastChain(rt.left, rt.top, rt.right - 1, rt.bottom - 1);
	for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽
	{
		double dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp14 < 4)continue;
		if (nDefect > MAX_HUGEDEFECT - 1)break;
		int nx1 = m_pChainIsland[nCam]->FindMaxX(i);//0625-1
		int nx2 = m_pChainIsland[nCam]->FindMinX(i);//0625-1
		int ny1 = m_pChainIsland[nCam]->FindMaxY(i);//0625-1
		int ny2 = m_pChainIsland[nCam]->FindMinY(i);//0625-1
													//
		double dxs = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
		double dys = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
		if (m_bSpurSizeUse[nCam]) {//ESWA 로직 //220503
			if ((m_bSpurSizeAndOr && dxs > m_dSpurSpecSizeX[nCam] && dys > m_dSpurSpecSizeY[nCam])
				|| (!m_bSpurSizeAndOr && (dxs >= m_dSpurMinSize[nCam] && dys >= m_dSpurMinSize[nCam]) && (dxs > m_dSpurSpecSizeX[nCam] || dys > m_dSpurSpecSizeY[nCam])))//
			{
				double dcx12 = 0, dcy12 = 0;
				m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				//Spur 일때 불량코드를 핀홀로 전송한다.
				if (dys == 0)dys = 1;
				double dratio1 = dxs / dys;
				if (dxs == 0)dxs = 1;
				double dratio2 = dys / dxs;
				if (dratio1 < dratio2)dratio1 = dratio2;
				if (dratio1 >= m_dXYRatio[nCam]) {//211105
					m_dISSize[nCam][nDefect] = 1001;
				}
				else
					m_dISSize[nCam][nDefect] = 2001;
				BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
				CRect rt22 = m_rtIsland[nCam][nDefect];
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(fmtmp, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

				nDefect++;
				if (nDefect >= MAX_HUGEDEFECT - 1) {//211017-1
					delete[]fmtmp;
					delete[]fmBlack;
					return nDefect;
				}
			}
		}
		else {//기존 로직 사용함.
			if ((m_bSpurSizeAndOr && dxs > m_dPinholeSizeX[nCam] && dys > m_dPinholeSizeY[nCam])
				|| (!m_bSpurSizeAndOr && (dxs >= m_dSpurMinSize[nCam] && dys >= m_dSpurMinSize[nCam]) && (dxs > m_dPinholeSizeX[nCam] || dys > m_dPinholeSizeY[nCam])))//211013 최소사이즈
			{
				double dcx12 = 0, dcy12 = 0;
				m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				//Spur 일때 불량코드를 핀홀로 전송한다.
				if (dys == 0)dys = 1;
				double dratio1 = dxs / dys;
				if (dxs == 0)dxs = 1;
				double dratio2 = dys / dxs;
				if (dratio1 < dratio2)dratio1 = dratio2;
				if (dratio1 >= m_dXYRatio[nCam]) {//211105
					m_dISSize[nCam][nDefect] = 1001;
				}
				else
					m_dISSize[nCam][nDefect] = 2001;
				BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
				CRect rt22 = m_rtIsland[nCam][nDefect];
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(fmtmp, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

				nDefect++;
				if (nDefect >= MAX_HUGEDEFECT - 1) {//211017-1
					delete[]fmtmp;
					delete[]fmBlack;
					return nDefect;
				}
			}
		}
		if (m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE) {	// 220126 - sjj
			//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
			if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
			{
				double dcx12 = 0, dcy12 = 0;
				m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				m_dISSize[nCam][nDefect] = 9001;
				BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
				CRect rt22 = m_rtIsland[nCam][nDefect];
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(fmtmp, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

				nDefect++;
			}
		}
		else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
		{
			double dcx12 = 0, dcy12 = 0;
			m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
			CPoint pt22;
			pt22.SetPoint(dcx12, dcy12);
			if (m_rtScrabIns[nCam].PtInRect(pt22)) {
				//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
				if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
				{
					m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
					m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
					m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
					m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
					m_dISSize[nCam][nDefect] = 9001;
					BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
					CRect rt22 = m_rtIsland[nCam][nDefect];
					if (!m_bDefectBinary)//0729-1
						ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					else
						ImageSelectCopy(fmtmp, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

					nDefect++;
				}
			}
		}

		if (nDefect > MAX_HUGEDEFECT - 2)break;
	}
	////220128
	BOOL bDarkInsp = TRUE;	// sjj - Spur영역 검은색 이물 검사 옵션
	if (bDarkInsp) {
		m_pChainIsland[nCam]->SetChainData(1, fmBlack, 1, 1, 4, 100000, nw, nh);//white
		nBlobCount14 = m_pChainIsland[nCam]->FastChain(rtTabIn.left, rtTabIn.top, rtTabIn.right - 1, rtTabIn.bottom - 1);
		for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽
		{
			double dTemp14 = m_pChainIsland[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp14 < 4)continue;
			if (nDefect > MAX_HUGEDEFECT - 1)break;
			int nx1 = m_pChainIsland[nCam]->FindMaxX(i);//0625-1
			int nx2 = m_pChainIsland[nCam]->FindMinX(i);//0625-1
			int ny1 = m_pChainIsland[nCam]->FindMaxY(i);//0625-1
			int ny2 = m_pChainIsland[nCam]->FindMinY(i);//0625-1
			//
			double dxs = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
			double dys = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
			if ((m_bSpurSizeAndOr && dxs > m_dCrackSizeX[nCam] && dys > m_dCrackSizeY[nCam])
				|| (!m_bSpurSizeAndOr && (dxs >= m_dSpurMinSize[nCam] && dys >= m_dSpurMinSize[nCam]) && (dxs > m_dCrackSizeX[nCam] || dys > m_dCrackSizeY[nCam])))//211013 최소사이즈
			{
				double dcx12 = 0, dcy12 = 0;
				m_pChainIsland[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				//Spur 일때 불량코드를 핀홀로 전송한다.
				if (dys == 0)dys = 1;
				double dratio1 = dxs / dys;
				if (dxs == 0)dxs = 1;
				double dratio2 = dys / dxs;
				if (dratio1 < dratio2)dratio1 = dratio2;
				if (dratio1 >= m_dXYRatio[nCam]) {//211105
					m_dISSize[nCam][nDefect] = 3003;
					BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
					CRect rt22 = m_rtIsland[nCam][nDefect];
					if (!m_bDefectBinary)//0729-1
						ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					else
						ImageSelectCopy(fmtmp, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

					nDefect++;
					if (nDefect >= MAX_HUGEDEFECT - 1) {//211017-1
						delete[]fmtmp;
						delete[]fmBlack;
						return nDefect;
					}
				}
			}
		}
	}

	m_nIslandCount[nCam] = nDefect;//

	delete[]fmtmp;
	delete[]fmBlack;
	return nDefect;
}
#else
int CInspect::FindSpur(int nCam, BYTE* fm, int nw, int  nh, CRect rtspur, int nTh)// 탭 안쪽 부위 미코팅 검사
{
		//예외처리
	CRect rtImage(0, 0, nw, nh);
	rtspur = rtImage & rtspur;

	if (0 >= rtspur.Width() || 0 >= rtspur.Height())
		return m_nIslandCount[nCam];

	double dStartTime = GetPrecisionTime();
	if (m_bWhiteRoll[nCam])//////220128
		fm = m_fmOdd[nCam];
	CRect rt, rtTabIn;
	rt = rtspur;
	rtTabIn = rt;
	int nDefect = m_nIslandCount[nCam];//
	if (nDefect >= MAX_HUGEDEFECT - 1)//211017-1
		return nDefect;

	int nAve = 0, nc1 = 0, nProj1 = 0, nright = rt.right;

	m_dNewinspTime[nCam][17] = (GetPrecisionTime() - dStartTime) * 1000.0f; //얘도 여기서 오래 걸림, 메모리 생성

	//Testing 
	rt.InflateRect(0, m_nSpurRangeTopBtm[nCam], 0, m_nSpurRangeTopBtm[nCam]);
	rt = SetBoundary2(rt, CRect(10, 0, nw - 100, nh));
	m_rtFullInspRect[nCam][3] = rt;

	int nSpurSizeX = rt.Width();
	int nSpurSizeY = m_nHeight;

	BYTE* SpurImage_White = new BYTE[nSpurSizeX * nSpurSizeY];
	BYTE* SpurImage_Black = new BYTE[nSpurSizeX * nSpurSizeY];////220128
	m_nSpurBufferSizeX[nCam] = nSpurSizeX;
	m_nSpurBufferSizeY[nCam] = nSpurSizeY;

	/*if (nullptr == m_fmtmp[nCam] ||
		nullptr == m_fmBlack[nCam] ||
		nSpurSizeX != m_nSpurBufferSizeX[nCam] ||
		nSpurSizeY != m_nSpurBufferSizeY[nCam])
	{
		if (0 >= nSpurSizeX || 0 >= nSpurSizeY)
			return 0;

		if (nullptr != m_fmtmp[nCam] || nullptr != m_fmBlack[nCam])
		{
			delete []m_fmtmp[nCam];
			delete []m_fmBlack[nCam];

			m_fmtmp[nCam] = nullptr;
			m_fmBlack[nCam] = nullptr;
		}

		m_fmtmp[nCam] = new BYTE[nSpurSizeX * nSpurSizeY];
		m_fmBlack[nCam] = new BYTE[nSpurSizeX * nSpurSizeY];

		m_nSpurBufferSizeX[nCam] = nSpurSizeX;
		m_nSpurBufferSizeY[nCam] = nSpurSizeY;

	}*/

	ZeroMemory(SpurImage_White, nSpurSizeX * nSpurSizeY);
	ZeroMemory(SpurImage_Black, nSpurSizeX * nSpurSizeY);
	
	
	rtTabIn.InflateRect(0, -10);
	rtTabIn = SetBoundary2(rtTabIn, CRect(10, 0, nw - 100, nh));

	for (int y = rt.top; y < rt.bottom && y >= 0; y++) {
		nProj1 = 0;
		for (int x = rt.right; x > rt.left && x >= 0; x--) {
			nAve += *(fm + nw * y + x);
			nc1++;
		}
	}
	m_dNewinspTime[nCam][18] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	if (nc1 == 0)nc1 = 1;
	nAve /= nc1;
	//어깨선 모서리 부분 Skip 하도록 추가 필요 //211120
	for (int y = rt.top; y < rt.bottom && y >= 0; y++) {
		for (int x = rt.left, x1 = 0; x < rt.right && x >= 0; x++, x1++) {
			//모서리 부분 스킵
			if (y < m_rtCellTab[nCam].top + 8 && x>m_rtCell[nCam].right - 4)//211123
				continue;
			if (y > m_rtCellTab[nCam].bottom - 8 && x > m_rtCell[nCam].right - 4)//211123
				continue;
			if (nAve + nTh < *(fm + nw * y + x)) 
			{
				*(SpurImage_White + rt.Width() * y  + x1) = 0xff;
			}
		}
	}

	m_dNewinspTime[nCam][19] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	int nTh1 = m_nCrackValue[nCam];//40
	for (int y = rtTabIn.top; y < rtTabIn.bottom && y >= 0; y++) {
		for (int x = rtTabIn.left, x1 = 0; x < rtTabIn.right && x >= 0; x++, x1++) {
			//모서리 부분 스킵
			if (y < m_rtCellTab[nCam].top + 4 && x>m_rtCell[nCam].right - 4)//211123
				continue;
			if (y > m_rtCellTab[nCam].bottom - 4 && x > m_rtCell[nCam].right - 4)//211123
				continue;
			if (nAve - nTh1 > *(fm + nw * y + x))
			{
				*(SpurImage_Black + rt.Width() * y + x1) = 0xff;
			}
		}
	}

	m_dNewinspTime[nCam][20] = (GetPrecisionTime() - dStartTime) * 1000.0f;


	//20220916 SJM
	if (m_bIstest) 
	{
		memset(m_BinarySpurImage[0], 0, sizeof(BYTE) * m_nWidth * m_nHeight);
		memset(m_BinarySpurImage[1], 0, sizeof(BYTE) * m_nWidth * m_nHeight);

		for (int y = rt.top, y1 = 0; y < rt.bottom; y++, y1++) memcpy(m_BinarySpurImage[0] + (y * m_nWidth) + rt.left, SpurImage_White + (y * m_nSpurBufferSizeX[nCam]), sizeof(BYTE) * m_nSpurBufferSizeX[nCam]);
		for (int y = rt.top, y1 = 0; y < rt.bottom; y++, y1++) memcpy(m_BinarySpurImage[1] + (y * m_nWidth) + rt.left, SpurImage_Black + (y * m_nSpurBufferSizeX[nCam]), sizeof(BYTE) * m_nSpurBufferSizeX[nCam]);
	}
	//20220916 SJM
	int nw2 = BAD_WIDTH * 0.5, nh2 = BAD_HEIGHT * 0.5;
	//m_pChainNoncoating[nCam]->SetChainData(1, SpurImage_White, 1, 1, 4, 100000, nw, nh);//white
	m_pChainNoncoating[nCam]->SetChainData(1, SpurImage_White, 1, 1, 4, 100000, m_nSpurBufferSizeX[nCam], m_nSpurBufferSizeY[nCam]);//white
	//int nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(rt.left, rt.top, rt.right - 1, rt.bottom - 1);
	int nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(0, rt.top, rt.Width() - 1, rt.bottom - 1);
	for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽
	{
		double dTemp14 = m_pChainNoncoating[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp14 < 4)continue;
		if (nDefect > MAX_HUGEDEFECT - 1)break;
		int nx1 = m_pChainNoncoating[nCam]->FindMaxX(i);//0625-1
		int nx2 = m_pChainNoncoating[nCam]->FindMinX(i);//0625-1
		int ny1 = m_pChainNoncoating[nCam]->FindMaxY(i);//0625-1
		int ny2 = m_pChainNoncoating[nCam]->FindMinY(i);//0625-1
													//
		double dxs = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
		double dys = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
		if (m_bSpurSizeUse[nCam]) {//ESWA 로직 //220503
			if ((m_bSpurSizeAndOr && dxs > m_dSpurSpecSizeX[nCam] && dys > m_dSpurSpecSizeY[nCam])
				|| (!m_bSpurSizeAndOr && (dxs >= m_dSpurMinSize[nCam] && dys >= m_dSpurMinSize[nCam]) && (dxs > m_dSpurSpecSizeX[nCam] || dys > m_dSpurSpecSizeY[nCam])))//
			{
				double dcx12 = 0, dcy12 = 0;
				m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				dcx12 += rt.left;
				//dcy12 += rt.top;
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				//Spur 일때 불량코드를 핀홀로 전송한다.
				if (dys == 0)dys = 1;
				double dratio1 = dxs / dys;
				if (dxs == 0)dxs = 1;
				double dratio2 = dys / dxs;
				if (dratio1 < dratio2)dratio1 = dratio2;
				if (dratio1 >= m_dXYRatio[nCam]) {//211105
					m_dISSize[nCam][nDefect] = 1001;
				}
				else
					m_dISSize[nCam][nDefect] = 2001;
				BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
				CRect rt22 = m_rtIsland[nCam][nDefect];
				rt22.left -= rt.left;
				rt22.right -= rt.left;
				//rt22.top -= rt.top;
				//rt22.bottom -= rt.top;
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(SpurImage_White, fmdefect1, m_nSpurBufferSizeX[nCam], m_nSpurBufferSizeY[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

				nDefect++;
				if (nDefect >= MAX_HUGEDEFECT - 1) {//211017-1
					delete[] SpurImage_White;
					delete[] SpurImage_Black;
					return nDefect;
				}
			}
		}
		else {//기존 로직 사용함.
			if ((m_bSpurSizeAndOr && dxs > m_dPinholeSizeX[nCam] && dys > m_dPinholeSizeY[nCam])
				|| (!m_bSpurSizeAndOr && (dxs >= m_dSpurMinSize[nCam] && dys >= m_dSpurMinSize[nCam]) && (dxs > m_dPinholeSizeX[nCam] || dys > m_dPinholeSizeY[nCam])))//211013 최소사이즈
			{
				double dcx12 = 0, dcy12 = 0;
				m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				dcx12 += rt.left;
				//dcy12 += rt.top;
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				//Spur 일때 불량코드를 핀홀로 전송한다.
				if (dys == 0)dys = 1;
				double dratio1 = dxs / dys;
				if (dxs == 0)dxs = 1;
				double dratio2 = dys / dxs;
				if (dratio1 < dratio2)dratio1 = dratio2;
				if (dratio1 >= m_dXYRatio[nCam]) {//211105
					m_dISSize[nCam][nDefect] = 1001;
				}
				else
					m_dISSize[nCam][nDefect] = 2001;
				BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
				CRect rt22 = m_rtIsland[nCam][nDefect];
				rt22.left -= rt.left;
				rt22.right -= rt.left;
				//rt22.top -= rt.top;
				//rt22.bottom -= rt.top;
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(SpurImage_White, fmdefect1, m_nSpurBufferSizeX[nCam], m_nSpurBufferSizeY[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

				nDefect++;
				if (nDefect >= MAX_HUGEDEFECT - 1) {//211017-1
					delete[] SpurImage_White;
					delete[] SpurImage_Black;
					return nDefect;
				}
			}
		}
		if (m_bScrabFullRange[nCam] && m_bInsScrab[nCam] == TRUE) {	// 220126 - sjj
			//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
			if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
			{
				double dcx12 = 0, dcy12 = 0;
				m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				dcx12 += rt.left;
				//dcy12 += rt.top;
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				m_dISSize[nCam][nDefect] = 9001;
				BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
				CRect rt22 = m_rtIsland[nCam][nDefect];
				rt22.left -= rt.left;
				rt22.right -= rt.left;
				//rt22.top -= rt.top;
				//rt22.bottom -= rt.top;
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(SpurImage_White, fmdefect1, m_nSpurBufferSizeX[nCam], m_nSpurBufferSizeY[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

				nDefect++;
			}
		}
		else if (m_bInsScrab[nCam] == TRUE)	// 220126 - sjj
		{
			double dcx12 = 0, dcy12 = 0;
			m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
			dcx12 += rt.left;
			//dcy12 += rt.top;
			CPoint pt22;
			pt22.SetPoint(dcx12, dcy12);
			if (m_rtScrabIns[nCam].PtInRect(pt22)) {
				//if(dxs> m_dSpecScarabSize[nCam] || dys> m_dSpecScarabSize[nCam] )
				if (dxs > m_dScrabSpecSizeX[nCam] || dys > m_dScrabSpecSizeY[nCam])
				{
					m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
					m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
					m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
					m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
					m_dISSize[nCam][nDefect] = 9001;
					BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
					CRect rt22 = m_rtIsland[nCam][nDefect];
					rt22.left -= rt.left;
					rt22.right -= rt.left;
					//rt22.top -= rt.top;
					//rt22.bottom -= rt.top;
					if (!m_bDefectBinary)//0729-1
						ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					else
						ImageSelectCopy(SpurImage_White, fmdefect1, m_nSpurBufferSizeX[nCam], m_nSpurBufferSizeY[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

					nDefect++;
				}
			}
		}

		if (nDefect > MAX_HUGEDEFECT - 2)break;
	}

	m_dNewinspTime[nCam][21] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	////220128
	BOOL bDarkInsp = TRUE;	// sjj - Spur영역 검은색 이물 검사 옵션
	if (bDarkInsp) {
		//m_pChainNoncoating[nCam]->SetChainData(1, SpurImage_Black, 1, 1, 4, 100000, nw, nh);//white
		m_pChainNoncoating[nCam]->SetChainData(1, SpurImage_Black, 1, 1, 4, 100000, m_nSpurBufferSizeX[nCam], m_nSpurBufferSizeY[nCam]);//white
		//nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(rtTabIn.left, rtTabIn.top, rtTabIn.right - 1, rtTabIn.bottom - 1);
		nBlobCount14 = m_pChainNoncoating[nCam]->FastChain(0, rtTabIn.top, rtTabIn.Width() - 1, rtTabIn.bottom - 1);
		for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 오른쪽
		{
			double dTemp14 = m_pChainNoncoating[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
			if (dTemp14 < 4)continue;
			if (nDefect > MAX_HUGEDEFECT - 1)break;
			int nx1 = m_pChainNoncoating[nCam]->FindMaxX(i);//0625-1
			int nx2 = m_pChainNoncoating[nCam]->FindMinX(i);//0625-1
			int ny1 = m_pChainNoncoating[nCam]->FindMaxY(i);//0625-1
			int ny2 = m_pChainNoncoating[nCam]->FindMinY(i);//0625-1
			//
			double dxs = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
			double dys = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
			if ((m_bSpurSizeAndOr && dxs > m_dCrackSizeX[nCam] && dys > m_dCrackSizeY[nCam])
				|| (!m_bSpurSizeAndOr && (dxs >= m_dSpurMinSize[nCam] && dys >= m_dSpurMinSize[nCam]) && (dxs > m_dCrackSizeX[nCam] || dys > m_dCrackSizeY[nCam])))//211013 최소사이즈
			{
				double dcx12 = 0, dcy12 = 0;
				m_pChainNoncoating[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
				dcx12 += rt.left;
				//dcy12 += rt.top;
				m_rtIsland[nCam][nDefect].SetRect(dcx12 - nw2, dcy12 - nh2, dcx12 + nw2, dcy12 + nh2);
				m_rtIsland[nCam][nDefect] = SetBoundary2(m_rtIsland[nCam][nDefect], CRect(10, 0, nw - 100, nh));
				m_dISSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam];
				m_dISSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam];
				//Spur 일때 불량코드를 핀홀로 전송한다.
				if (dys == 0)dys = 1;
				double dratio1 = dxs / dys;
				if (dxs == 0)dxs = 1;
				double dratio2 = dys / dxs;
				if (dratio1 < dratio2)dratio1 = dratio2;
				if (dratio1 >= m_dXYRatio[nCam]) {//211105
					m_dISSize[nCam][nDefect] = 3003 + 5;
					BYTE* fmdefect1 = m_pIsland[nCam][nDefect];
					CRect rt22 = m_rtIsland[nCam][nDefect];
					rt22.left -= rt.left;
					rt22.right -= rt.left;
					//rt22.top -= rt.top;
					//rt22.bottom -= rt.top;
					if (!m_bDefectBinary)//0729-1
						ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					else
						ImageSelectCopy(SpurImage_White, fmdefect1, m_nSpurBufferSizeX[nCam], m_nSpurBufferSizeY[nCam], rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);

					nDefect++;
					if (nDefect >= MAX_HUGEDEFECT - 1) {//211017-1
						delete[] SpurImage_White;
						delete[] SpurImage_Black;	
						return nDefect;
					}
				}
			}
		}
	}

	m_dNewinspTime[nCam][22] = (GetPrecisionTime() - dStartTime) * 1000.0f;

	m_nIslandCount[nCam] = nDefect;//

	delete[] SpurImage_White;
	delete[] SpurImage_Black;
	return nDefect;
}
#endif

int CInspect::FindScrab(int nCam, BYTE * fmBright, BYTE * fmDark, int nw, int nh, int nRight)
{
	m_dInsSubTime1[nCam][1] = GetPrecisionTime();// - m_dInsSubTime1[nCam][0]) * 1000;

	int nDefect = m_nScrabRangDefect[nCam];//m_nIslandCount[nCam];
	if (nDefect > MAX_HUGEDEFECT - 2) return nDefect;
	m_nScrabRightBoundary = nRight;
	m_nScrabWidth = 0; m_nScrabHeight = 0;//20220916 SJM
	BYTE * fm = fmBright;
//	if(m_bWhiteRoll[nCam])
//		fm = fmDark;
	int nleftpixel = m_nScrabRoiLeft[nCam];// / m_dscale_x1[nCam];
	int nrightpixel = m_nScrabRoiRight[nCam]; /// m_dscale_x1[nCam];
	m_dScrabSize[nCam] = 0;//X Y 중 큰 값
	int nTh = m_nScrabTh[nCam];
	CRect rtins,rtins1;//220109
	rtins.SetRect(nRight - nleftpixel, m_nScrabRoiTop[nCam], nRight - nrightpixel, nh - m_nScrabRoiBot[nCam]);
	rtins = rtins & CRect(m_rtCell[nCam].left+1, 0, m_rtCell[nCam].right-1 ,nh);//220211 옵셋1 픽셀 //기울어져 있을경우 어떻게? 잘... 예쁘게
	rtins = rtins & CRect(10, 10, nw - 10, nh - 10);//얼라인 이상할시 이미지와 예외처리
	rtins1 = rtins;//220109
	m_rtScrabIns[nCam] = rtins;
	int nCompress = 1,nCompressY=1;//220330
	int nw1 = nw,nh1=nh;//220109
	if(rtins.Width()>5000){//220512 -우선 기존것 유지 
//image Compress
		nCompress = 4;//220330
		nCompressY = 2;//220330
		rtins1.SetRect((nRight - nleftpixel)/nCompress, (m_nScrabRoiTop[nCam])/nCompressY, (nRight - nrightpixel)/nCompress, (nh - m_nScrabRoiBot[nCam])/nCompressY);//220406
		rtins1 = rtins1 & CRect(m_rtCell[nCam].left/nCompress, 0, m_rtCell[nCam].right/nCompress ,nh/nCompressY);//220406
		nw1 /=nCompress;//220109
		nh1 /=nCompressY;//220109
		//rtins = rtins & CRect(0, 0, nw, nh);
	}
	else if(rtins.Width() > 500){//220330
		nCompress = 2;//220330
		nCompressY = 2;//220330
		rtins1.SetRect((nRight - nleftpixel)/nCompress, (m_nScrabRoiTop[nCam])/nCompressY, (nRight - nrightpixel)/nCompress, (nh - m_nScrabRoiBot[nCam])/nCompressY);//220109
		rtins1 = rtins1 & CRect(m_rtCell[nCam].left/nCompress, 0, m_rtCell[nCam].right/nCompress ,nh/nCompressY);
		nw1 /=nCompress;//220109
		nh1 /=nCompressY;//220109
	}
	CRect rtImage(0, 0, nw-10, nh-10);
	rtins1 &= rtImage;
	double dave = 0, dc = 0;
	for (int y = rtins.top; y < rtins.bottom && y >= 0 && y<nh; y+=nCompressY) {//220330
		for (int x = rtins.left; x < rtins.right && x >= 0; x+=nCompress) {//220109
			dave += *(fm + (nw)*y + x);
			dc++;  //여기서죽음 아마 이미지사이즈가 너무 커서그런듯
		}
	}
	if (dc == 0) return 0;
	dave /= dc;
	BYTE * fmNonCoat = new BYTE[nw1*nh1];//220109
	memset(fmNonCoat, 0, nw1*nh1);//220109
	if(m_bTiltEdge[nCam]){//220403
		for (int y = rtins.top; y < rtins.bottom && y >= 0 && y<nh && y<3000; y+=nCompressY) {//220109
			int nsX = m_nLRedge[nCam][0][y];///nCompress;//220509  //
			int neX = m_nLRedge[nCam][1][y];///nCompress;//220509
			if(rtins.left > nsX) nsX = rtins.left;
			if(rtins.right < neX) neX = rtins.right;
			for (int x = nsX; x < neX && x >= 0 ; x+=nCompress) {//220109
				if (*(fm + (nw)*y + x) >= dave + nTh) {
					int x11 = x / nCompress;
					int y11 = y/nCompressY;
					if(x11>= nw1 || y11>=nh1)continue;//220109
					*(fmNonCoat + nw1*y11 + x11) = 0xff;//220109
				}
			}
		}
	}
	else{
		for (int y = rtins.top; y < rtins.bottom && y >= 0 && y<nh; y+=nCompressY) {//220109
			for (int x = rtins.left; x < rtins.right && x >= 0; x+=nCompress) {//220109
				if (*(fm + (nw)*y + x) >= dave + nTh) {
					int x11 = x / nCompress;
					int y11 = y/nCompressY;
					if(x11>= nw1 || y11>=nh1)continue;//220109
					*(fmNonCoat + nw1*y11 + x11) = 0xff;//220109
				}
			}
		}
	}
	//20220916 SJM
	if (m_bIstest) {
		m_nScrabWidth = nw1; m_nScrabHeight = nh1;
		memcpy(m_ScrabImage, fmNonCoat, sizeof(BYTE) * nw1 * nh1);
	}
	//20220916 SJM
	m_pChainScrab[nCam]->SetChainData(1, fmNonCoat, 1, 1, 4, 100000, nw1, nh1);//white//220109
	int nBlobCount14 = m_pChainScrab[nCam]->FastChain(rtins1.left, rtins1.top, rtins1.right - 1, rtins1.bottom - 1);//220109
	double dTemp14 = 0, nBlobNo14old = 0, dBlobSize14 = 0; int nBlobNo14 = -1;
	BOOL bFullSearch = TRUE;//220125 
	for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 왼쪽//0910
	{
		dTemp14 = m_pChainScrab[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp14 < 4)continue;

		double dcx12 = 0, dcy12 = 0;
		m_pChainScrab[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
		CPoint pt14;
		pt14.SetPoint(dcx12* nCompress,dcy12* nCompressY);
		int nJump = 0;
		for(int k=0;k<4;k++){
			if(m_rtNotchArea[nCam][k].PtInRect(pt14)){
				nJump=k+1;//220406
				break;
			}
		}
		if(nJump>=1)//220406
			continue;
		if (dTemp14 > nBlobNo14old) {
			nBlobNo14old = dTemp14;
			nBlobNo14 = i;
		}
		if(bFullSearch){
			int nx1 = m_pChainScrab[nCam]->FindMaxX(i);//0625-1
			int nx2 = m_pChainScrab[nCam]->FindMinX(i);//0625-1
			int ny1 = m_pChainScrab[nCam]->FindMaxY(i);//0625-1
			int ny2 = m_pChainScrab[nCam]->FindMinY(i);//0625-1
			double dsize1 = (nx1 - nx2 + 1)*m_dscale_x1[nCam] * nCompress;
			double dsize2 = (ny1 - ny2 + 1)*m_dscale_y1[nCam] * nCompressY;
			double dsize3 = dsize1;
			if (dsize3 < dsize2)dsize3 = dsize2;

			if(m_bScrabSizeAndOr){//and //220804
				//if (dsize3>= m_dSpecScarabSize[nCam] && dsize1 >= m_dScrabSpecSizeX[nCam] && dsize2 >= m_dScrabSpecSizeY[nCam])
				if (dsize3 >= min(m_dScrabSpecSizeX[nCam], m_dScrabSpecSizeY[nCam]) && dsize1 >= m_dScrabSpecSizeX[nCam] && dsize2 >= m_dScrabSpecSizeY[nCam])
				{//스크랩 불량
					m_dScrabSize[nCam] = dsize1;
					m_bScrabReject[nCam] = TRUE;
					double dcx12 = 0, dcy12 = 0;
					int nw2 = BAD_WIDTH*0.5, nh2 = BAD_HEIGHT*0.5;
					m_pChainScrab[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
					m_rtScrabDefect[nCam][nDefect].SetRect(dcx12*nCompress - nw2, dcy12*nCompressY - nh2, dcx12*nCompress + nw2, dcy12*nCompressY + nh2);
					m_rtScrabDefect[nCam][nDefect] = SetBoundary2(m_rtScrabDefect[nCam][nDefect], CRect(10, 0, nw1*nCompress - 100, nh1*nCompressY));//220109
					m_dScrabDefectSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam]*nCompress;
					m_dScrabDefectSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam]*nCompressY;
					m_dScrabDefectSize[nCam][nDefect] = 9001;
					BYTE * fmdefect1 = m_pScrabDefect[nCam][nDefect];
					CRect rt22 = m_rtScrabDefect[nCam][nDefect];
					if (!m_bDefectBinary)//0729-1
						ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					else
						ImageSelectCopy(fmNonCoat, fmdefect1, nw1, nh1, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);//220109
					nDefect++;
					m_nScrabRangDefect[nCam]=nDefect;//m_nIslandCount[nCam] = nDefect;//220411
				}
			}
			else{//or //220804
				//if (dsize3>= m_dSpecScarabSize[nCam])
				if (dsize3 >= min(m_dScrabSpecSizeX[nCam], m_dScrabSpecSizeY[nCam]))
				{//스크랩 불량
					m_dScrabSize[nCam] = dsize1;
					m_bScrabReject[nCam] = TRUE;
					double dcx12 = 0, dcy12 = 0;
					int nw2 = BAD_WIDTH*0.5, nh2 = BAD_HEIGHT*0.5;
					m_pChainScrab[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
					m_rtScrabDefect[nCam][nDefect].SetRect(dcx12*nCompress - nw2, dcy12*nCompressY - nh2, dcx12*nCompress + nw2, dcy12*nCompressY + nh2);
					m_rtScrabDefect[nCam][nDefect] = SetBoundary2(m_rtScrabDefect[nCam][nDefect], CRect(10, 0, nw1*nCompress - 100, nh1*nCompressY));//220109
					m_dScrabDefectSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam]*nCompress;
					m_dScrabDefectSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam]*nCompressY;
					m_dScrabDefectSize[nCam][nDefect] = 9001;
					BYTE * fmdefect1 = m_pScrabDefect[nCam][nDefect];
					CRect rt22 = m_rtScrabDefect[nCam][nDefect];
					if (!m_bDefectBinary)//0729-1
						ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
					else
						ImageSelectCopy(fmNonCoat, fmdefect1, nw1, nh1, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);//220109
					nDefect++;
					m_nScrabRangDefect[nCam]=nDefect;//m_nIslandCount[nCam] = nDefect;//220411
				}
			}

			if (nDefect > MAX_HUGEDEFECT - 2)break;

		}

	}
	if (nBlobNo14 >= 0 && !bFullSearch) {
		int nx1 = m_pChainScrab[nCam]->FindMaxX(nBlobNo14);//0625-1
		int nx2 = m_pChainScrab[nCam]->FindMinX(nBlobNo14);//0625-1
		int ny1 = m_pChainScrab[nCam]->FindMaxY(nBlobNo14);//0625-1
		int ny2 = m_pChainScrab[nCam]->FindMinY(nBlobNo14);//0625-1
		double dsize1 = (nx1 - nx2 + 1)*m_dscale_x1[nCam] * nCompress;
		double dsize2 = (ny1 - ny2 + 1)*m_dscale_y1[nCam] * nCompressY;
		m_dScrabSize[nCam] = dsize1;
		if (m_dScrabSize[nCam] < dsize2)m_dScrabSize[nCam] = dsize2;
		
		if(m_bScrabSizeAndOr){//and //220804
			//if (m_dScrabSize[nCam] >= m_dSpecScarabSize[nCam] && dsize1 >= m_dScrabSpecSizeX[nCam] && dsize2 >= m_dScrabSpecSizeY[nCam]) 
			if (m_dScrabSize[nCam] >= min(m_dScrabSpecSizeX[nCam], m_dScrabSpecSizeY[nCam]) && dsize1 >= m_dScrabSpecSizeX[nCam] && dsize2 >= m_dScrabSpecSizeY[nCam])
			{//스크랩 불량
				m_bScrabReject[nCam] = TRUE;
				double dcx12 = 0, dcy12 = 0;
				int nw2 = BAD_WIDTH*0.5, nh2 = BAD_HEIGHT*0.5;
				m_pChainScrab[nCam]->Chain_Center(nBlobNo14, &dcx12, &dcy12);//0625-1
				m_rtScrabDefect[nCam][nDefect].SetRect(dcx12*nCompress - nw2, dcy12*nCompressY - nh2, dcx12*nCompress + nw2, dcy12*nCompressY + nh2);
				m_rtScrabDefect[nCam][nDefect] = SetBoundary2(m_rtScrabDefect[nCam][nDefect], CRect(10, 0, nw1*nCompress - 100, nh1*nCompressY));//220109
				m_dScrabDefectSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam]*nCompress;
				m_dScrabDefectSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam]*nCompressY;
				m_dScrabDefectSize[nCam][nDefect] = 9001;
				BYTE * fmdefect1 = m_pScrabDefect[nCam][nDefect];
				CRect rt22 = m_rtScrabDefect[nCam][nDefect];
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(fmNonCoat, fmdefect1, nw1, nh1, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);//220109
				nDefect++;
				m_nScrabRangDefect[nCam]=nDefect;//m_nIslandCount[nCam] = nDefect;//220411
			}	
		}
		else{//or //220804
			//if (m_dScrabSize[nCam] >= m_dSpecScarabSize[nCam]) 
			if (m_dScrabSize[nCam] >= min(m_dScrabSpecSizeX[nCam], m_dScrabSpecSizeY[nCam]))
			{//스크랩 불량
				m_bScrabReject[nCam] = TRUE;
				double dcx12 = 0, dcy12 = 0;
				int nw2 = BAD_WIDTH*0.5, nh2 = BAD_HEIGHT*0.5;
				m_pChainScrab[nCam]->Chain_Center(nBlobNo14, &dcx12, &dcy12);//0625-1
				m_rtScrabDefect[nCam][nDefect].SetRect(dcx12*nCompress - nw2, dcy12*nCompressY - nh2, dcx12*nCompress + nw2, dcy12*nCompressY + nh2);
				m_rtScrabDefect[nCam][nDefect] = SetBoundary2(m_rtScrabDefect[nCam][nDefect], CRect(10, 0, nw1*nCompress - 100, nh1*nCompressY));//220109
				m_dScrabDefectSizeX[nCam][nDefect] = (nx1 - nx2 + 1)*m_dscale_x1[nCam]*nCompress;
				m_dScrabDefectSizeY[nCam][nDefect] = (ny1 - ny2 + 1)*m_dscale_y1[nCam]*nCompressY;
				m_dScrabDefectSize[nCam][nDefect] = 9001;
				BYTE * fmdefect1 = m_pScrabDefect[nCam][nDefect];
				CRect rt22 = m_rtScrabDefect[nCam][nDefect];
				if (!m_bDefectBinary)//0729-1
					ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
				else
					ImageSelectCopy(fmNonCoat, fmdefect1, nw1, nh1, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);//220109
				nDefect++;
				m_nScrabRangDefect[nCam]=nDefect;//m_nIslandCount[nCam] = nDefect;//220411
			}		
		}
	}
	delete []fmNonCoat;//0618
	m_nScrabRangDefect[nCam]=nDefect;//m_nIslandCount[nCam] = nDefect;//220411
	//m_nIslandCount[nCam] = nDefect;//220207
	m_dInsSubTime1[nCam][2] = (GetPrecisionTime() - m_dInsSubTime1[nCam][1]) * 1000;

	return nDefect;
}

int CInspect::FindEdge1(int nCam,BYTE * fmBright,BYTE * fmDark)
{
	CRect rtEdge[2];
	BYTE * fm = fmBright;
	if(m_bWhiteRoll[nCam])
		fm = fmDark;
	int nw = m_nWidth;
	int nh = m_nHeight;
	int nNonCotrange = 32;//0706
	if(m_rtHugeRect[nCam].left < m_nMarginSearchStart[nCam]|| m_rtHugeRect[nCam].left > m_nMarginSearchStart[nCam] + m_nLeftElecEdgeSearchAreaSizeX[nCam])//220225
		m_rtHugeRect[nCam].left = m_nMarginSearchStart[nCam];
	if(m_rtHugeRect[nCam].right > m_nMarginSearchEnd[nCam]|| m_rtHugeRect[nCam].right < m_nMarginSearchEnd[nCam] - m_nRightElecEdgeSearchAreaSizeX[nCam])//220225
		m_rtHugeRect[nCam].right = m_nMarginSearchEnd[nCam];
//	rtEdge[0].SetRect(m_rtHugeRect[nCam].left - nNonCotrange * 2, 0, m_rtHugeRect[nCam].left + nNonCotrange, nh);//0706
//	rtEdge[1].SetRect(m_rtHugeRect[nCam].right - nNonCotrange * 4, 0, m_rtHugeRect[nCam].right + nNonCotrange * 6/*nNonCotrange * 2*/, nh);//0706//1006
	rtEdge[0].SetRect(m_nMarginSearchStart[nCam], m_nElecEdgeSearchAreaTopBotOffset[nCam], m_nMarginSearchStart[nCam] + m_nLeftElecEdgeSearchAreaSizeX[nCam] , nh - m_nElecEdgeSearchAreaTopBotOffset[nCam]);//0706
	rtEdge[1].SetRect(m_nMarginSearchEnd[nCam] - m_nRightElecEdgeSearchAreaSizeX[nCam] , m_nElecEdgeSearchAreaTopBotOffset[nCam], m_nMarginSearchEnd[nCam]  , nh - m_nElecEdgeSearchAreaTopBotOffset[nCam]);//0706//1006
	rtEdge[0] = SetBoundary2(rtEdge[0], CRect(80, 0, nw - 100, nh));
	rtEdge[1] = SetBoundary2(rtEdge[1], CRect(80, 0, nw - 100, nh));
	int midy = rtEdge[0].Height()*0.5;
	int nEdgeY[3000] = { 0, }, nEdgeY1[3000] = { 0, };
	int ntmpold[2] = { 0, }, noldpos[2] = { 0, }, noldposy[2] = { 0, }, npos = 0;
	int nEdgeTh11 = m_nEdgeTh[nCam]; 25;//30;
	BYTE * fmtmp1 = fm;//new BYTE[nw*nh];
	int nSumEdge1 = 0;	double dCoatEdge1[4] = { 0, };
	int nMid = rtEdge[0].Height()*0.5;
	int nLeft = 0, nLeftcnt = 0;//@0514
	//threshold check  threshold값이 커지면 에지를 넘어가 롤쪽 이물을 검출할수 있다.
	if(m_nEdgeSearchDirection[nCam]==1){

	}
	if (rtEdge[0].top < 0)rtEdge[0].top = 0;
	if (rtEdge[0].bottom >= nh) rtEdge[0].bottom = nh - 1;
	for (int y1 = rtEdge[0].top; y1<rtEdge[0].bottom && y1 >= 0; y1++) {
		ntmpold[0] = 0; nLeftcnt = 0; nLeft = 0;
		if(m_nEdgeSearchDirection[nCam]==0){//roll -> 전극
			for (int x1 = rtEdge[0].left + 4; x1<rtEdge[0].right - 4 && x1 >= 0; x1++) {
				int ntmp = abs(*(fm + y1*nw + x1 - 4) - *(fm + y1*nw + x1));//0819
				if (ntmp > nEdgeTh11)// && ntmp> ntmpold[0] )//220207
				{
					if(abs(*(fm + y1*nw + x1 - 4) - *(fm + y1*nw + x1+4))> nEdgeTh11)
					{//220211 //롤에 이물스킵
						ntmpold[0] = ntmp;	npos = x1;
						break;
					}
				}
				if(ntmp> ntmpold[0]){
					ntmpold[0] = ntmp;	npos = x1;
				}
			}
		}
		else{//220414
			for (int x1 = rtEdge[0].right - 4; x1>rtEdge[0].left + 4 && x1 >= 0; x1--) {
				int ntmp = abs(*(fm + y1*nw + x1) - *(fm + y1*nw + x1- 4));//
				if (ntmp > nEdgeTh11)// && ntmp> ntmpold[0] )//220207
				{
					if(abs(*(fm + y1*nw + x1 - 4) - *(fm + y1*nw + x1+4))> nEdgeTh11)
					{//220211 //롤에 이물스킵
						ntmpold[0] = ntmp;	npos = x1;
						break;
					}
				}
				if(ntmp> ntmpold[0]){
					ntmpold[0] = ntmp;	npos = x1;
				}
			}
		}
		nEdgeY[y1] = npos;
		m_nEdgeData1[nCam][y1] = npos;//220207
	}
	int nSortdata[200] = {0,},nsortcnt=0;
	int nSortdata1[200] = {0,},nsortcnt1=0;

	m_vecLeftElecEdgeCandidateX[nCam].clear();
	m_vecLeftElecEdgeCandidateY[nCam].clear();

	for (int y1 = rtEdge[0].top; y1<rtEdge[0].top + 200 && y1 >= 0; y1++) 
	{
		nSortdata[nsortcnt] = nEdgeY[y1];
		nsortcnt++;//220207

		m_vecLeftElecEdgeCandidateX[nCam].push_back(nEdgeY[y1]);
		m_vecLeftElecEdgeCandidateY[nCam].push_back(y1);

		if (nEdgeY[y1]> noldpos[0]) 
		{//제일 오른쪽위치 찾기
			noldpos[0] = nEdgeY[y1];
			noldposy[0] = y1;
		}
		//if(m_bTiltEdge[nCam] && !m_bWhiteRoll[nCam]){//bright에서만 에지를 찾을때 다크 이미지도 체크한다.  검증후 적용
		//	int nMaxEdgeV = 0;
		//	for(int x1 = nEdgeY[y1] - 100; x1<nEdgeY[y1]+100;x1+=2){
		//		if(abs(*(fmDark + y1*nw + x1 - 4) - *(fmDark + y1*nw + x1+4))> nMaxEdgeV){//220211 //롤에 이물스킵
		//			nSortdata1[nsortcnt1] = x1;nMaxEdgeV = abs(*(fmDark + y1*nw + x1 - 4) - *(fmDark + y1*nw + x1+4));nsortcnt1++;
		//		}	
		//	}
		//}
	}
	sort(nSortdata,nSortdata+nsortcnt);
	int nMidData[3] = {0,};//220401
	for(int i=50;i<150;i++)
	{
		nMidData[0] += nSortdata[i];
	}
	nMidData[0] /= 100;
	nsortcnt = nsortcnt1 =0;

	for (int y1 = rtEdge[0].bottom - 4; y1>rtEdge[0].bottom - 200 && y1 >= 0; y1--)
	{
		if(y1<0)break;//211123

		if (nEdgeY[y1]> noldpos[1]) 
		{
			noldpos[1] = nEdgeY[y1];
			noldposy[1] = y1;
		}

		nSortdata[nsortcnt] = nEdgeY[y1];
		nsortcnt++;//22020

		m_vecLeftElecEdgeCandidateX[nCam].push_back(nEdgeY[y1]);
		m_vecLeftElecEdgeCandidateY[nCam].push_back(y1);
	}
	sort(nSortdata,nSortdata+nsortcnt);
	for(int i=50;i<150;i++)
	{
		nMidData[1] += nSortdata[i];
	}
	nMidData[1] /= 100;
	nMidData[2] = nMidData[0];
	//기울어져있으므로 작은것을 쓴다.
	if(nMidData[0] < nMidData[1]) nMidData[2] = nMidData[1];
	dCoatEdge1[0] = nMidData[2];

	m_rtCell[nCam].SetRect(dCoatEdge1[0], 0, nw - 200, nh);//전체 영역에서 찾기
	m_rtCell[nCam] = SetBoundary2(m_rtCell[nCam], CRect(80, 0, nw - 100, nh));
	double dCoatTopEdge = 0, dCoatBtmEdge = 0;//211002
	int nxoffset1 = m_nCompareDistance[nCam], ncnt12[2] = {0,};//4
	int nPeakPos1 = 0,nPeakPos2=0;
	int nNoncoatEdge1 = 220;
	if(m_bWhiteRoll[nCam]==TRUE){
		nNoncoatEdge1 = 180;//
	}
	
	if(m_bEdgeNoiseSkip[nCam]){//220404
		for (int y1 = rtEdge[1].top; y1<rtEdge[1].bottom && y1 >= 0; y1++) 
		{
			ntmpold[0] = 0;
			npos = nPeakPos1=nPeakPos2=0;//211020
			if(m_nEdgeSearchDirection[nCam]==0)
			{
				for (int x1 = rtEdge[1].right - nxoffset1; x1 > rtEdge[1].left + nxoffset1 && x1 >= 0; x1--) 
				{//211008
					int ntmp = abs(*(fm + y1*nw + x1) - *(fm + y1*nw + x1-nxoffset1));//211008
					if (*(fm + y1*nw + x1 - nxoffset1) > nNoncoatEdge1)//211008
						continue;//탈리로 포일 노출시 다음을 찾는다.
					if(nPeakPos2 < ntmp &&   *(fm + y1*nw + x1-nxoffset1) >  m_dMiddleAve[nCam] - 30 && *(fm + y1*nw + x1-nxoffset1) <  m_dMiddleAve[nCam] + 20)
					{//220404
						nPeakPos2 = ntmp;
						npos = x1 - 4;
					}
					if (ntmp > nEdgeTh11 &&  *(fm + y1*nw + x1-nxoffset1) >  m_dMiddleAve[nCam] - 30 && *(fm + y1*nw + x1-nxoffset1) <  m_dMiddleAve[nCam] + 20 )
					{// m_dMiddleAve[nCam] //211003
						if(abs(*(fm + y1*nw + x1) - *(fm + y1*nw + x1-nxoffset1*2)) > nEdgeTh11 )
						{
							ntmpold[0] = ntmp;
							npos = x1 - 8;//220214
							nEdgeY[y1] = npos;//0819
							break;
						}
					}
				}
			}
			else
			{
				int nCount = 0;

				for (int x1 = rtEdge[1].left + nxoffset1; x1 < rtEdge[1].right - nxoffset1 && x1 >= 0; x1++) 
				{//
					int ntmp = abs(*(fm + y1*nw + x1) - *(fm + y1*nw + x1+nxoffset1));//
					//if (*(fm + y1*nw + x1 + nxoffset1) > nNoncoatEdge1)//
					//	continue;//탈리로 포일 노출시 다음을 찾는다.
					if(nPeakPos2 < ntmp &&   *(fm + y1*nw + x1) >  m_dMiddleAve[nCam] - 30 && *(fm + y1*nw + x1) <  m_dMiddleAve[nCam] + 20)
					{//220404
						nPeakPos2 = ntmp;
						npos = x1 - 4;
					}
					if (ntmp > nEdgeTh11 &&  *(fm + y1*nw + x1-nxoffset1) >  m_dMiddleAve[nCam] - 30 && *(fm + y1*nw + x1-nxoffset1) <  m_dMiddleAve[nCam] + 20 )
					{// m_dMiddleAve[nCam] //211003
						if(abs(*(fm + y1*nw + x1) - *(fm + y1*nw + x1+nxoffset1*2)) > nEdgeTh11 )
						{
							ntmpold[0] = ntmp;
							npos = x1 + nxoffset1;//220214
							nEdgeY[y1] = npos;//0819
							break;
						}
					}
				}
			}
			nEdgeY[y1] = npos;
			if (y1 < rtEdge[1].top + 10 && npos>0)
			{//211002
				dCoatTopEdge += npos;ncnt12[0]++;
			}
			else if (y1 >= rtEdge[1].bottom - 10 && npos>0)
			{//211002
				dCoatBtmEdge += npos;ncnt12[1]++;
			}
		}
	}
	else{
		for (int y1 = rtEdge[1].top; y1<rtEdge[1].bottom && y1 >= 0; y1++) 
		{
			ntmpold[0] = 0;
			npos = nPeakPos1=nPeakPos2=0;//211020
			if(m_nEdgeSearchDirection[nCam]==0){
				for (int x1 = rtEdge[1].right - nxoffset1; x1 > rtEdge[1].left + nxoffset1 && x1 >= 0; x1--) 
				{//211008
					int ntmp = abs(*(fm + y1*nw + x1) - *(fm + y1*nw + x1-nxoffset1));//211008
					if (*(fm + y1*nw + x1 - nxoffset1) > nNoncoatEdge1)//211008
						continue;//탈리로 포일 노출시 다음을 찾는다.
					if(nPeakPos2 < ntmp)
					{//211020
						nPeakPos2 = ntmp;
						npos = x1 - 4;
					}
					if (ntmp > nEdgeTh11 )
					{//&&  *(fm + y1*nw + x1-nxoffset1) >  m_dMiddleAve[nCam] - 20 && *(fm + y1*nw + x1-nxoffset1) <  m_dMiddleAve[nCam] + 20 ){// m_dMiddleAve[nCam] //211003
						if(abs(*(fm + y1*nw + x1) - *(fm + y1*nw + x1-nxoffset1*2)) > nEdgeTh11 )
						{
							ntmpold[0] = ntmp;
							npos = x1 - 8;//220214
							nEdgeY[y1] = npos;//0819
							break;
						}
					}
				}
			}
			else
			{
				for (int x1 = rtEdge[1].left + nxoffset1; x1 < rtEdge[1].right - nxoffset1 && x1 >= 0; x1++) 
				{//211008
					int ntmp = abs(*(fm + y1*nw + x1) - *(fm + y1*nw + x1+nxoffset1));//211008
					//if (*(fm + y1*nw + x1 - nxoffset1) > nNoncoatEdge1)//211008
					//	continue;//탈리로 포일 노출시 다음을 찾는다.
					if(nPeakPos2 < ntmp)
					{//211020
						nPeakPos2 = ntmp;
						npos = x1 +nxoffset1;
					}
					if (ntmp > nEdgeTh11 )
					{//&&  *(fm + y1*nw + x1-nxoffset1) >  m_dMiddleAve[nCam] - 20 && *(fm + y1*nw + x1-nxoffset1) <  m_dMiddleAve[nCam] + 20 ){// m_dMiddleAve[nCam] //211003
						if(abs(*(fm + y1*nw + x1+nxoffset1) - *(fm + y1*nw + x1-nxoffset1*2)) > nEdgeTh11 )
						{
							ntmpold[0] = ntmp;
							npos = x1 + nxoffset1;//220214
							nEdgeY[y1] = npos;//0819
							break;
						}
					}
				}
			}

			nEdgeY[y1] = npos;
			if (y1 < rtEdge[1].top + 10 && npos>0)
			{//211002
				dCoatTopEdge += npos;ncnt12[0]++;
			}
			else if (y1 >= rtEdge[1].bottom - 10 && npos>0)
			{//211002
				dCoatBtmEdge += npos;ncnt12[1]++;
			}
		}
	}

	nsortcnt = 0;

	m_vecRightElecEdgeCandidateX[nCam].clear();
	m_vecRightElecEdgeCandidateY[nCam].clear();

	int nMidData1[3] = {0,};
	for (int y1 = rtEdge[1].top; y1<rtEdge[1].top + 200 && y1 >= 0; y1++) 
	{
		nSortdata[nsortcnt] = nEdgeY[y1];
		nsortcnt++;//220207

		m_vecRightElecEdgeCandidateX[nCam].push_back(nEdgeY[y1]);
		m_vecRightElecEdgeCandidateY[nCam].push_back(y1);
	}
	sort(nSortdata,nSortdata+nsortcnt);
	for(int i=50;i<150;i++){
		nMidData1[0] += nSortdata[i];
	}
	nMidData1[0] /= 100;
	nsortcnt = 0;
	for (int y1 = rtEdge[1].bottom-200; y1<rtEdge[1].bottom && y1 >= 0; y1++) 
	{
		nSortdata[nsortcnt] = nEdgeY[y1];
		nsortcnt++;//220207

		m_vecRightElecEdgeCandidateX[nCam].push_back(nEdgeY[y1]);
		m_vecRightElecEdgeCandidateY[nCam].push_back(y1);
	}
	sort(nSortdata,nSortdata+nsortcnt);
	for(int i=50;i<150;i++)
	{
		nMidData1[1] += nSortdata[i];
	}
	nMidData1[1] /= 100;
	nMidData1[2] = nMidData1[0];
	if(abs(nMidData1[0]-nMidData1[1]) < 12)
	{//220221
		//기울어져있으므로 작은것을 쓴다.
		if(nMidData1[0] > nMidData1[1]) nMidData1[2] = nMidData1[1];
	}
	else
	{
		if(nMidData1[0] < nMidData1[1]) nMidData1[2] = nMidData1[1];
	}
/////220403
	if(m_bTiltEdge[nCam]){
		for (int y1 = rtEdge[1].top; y1<rtEdge[1].bottom && y1 >= 0 && y1<3000; y1++) {
			m_nLRedge[nCam][0][y1] = nMidData[0];
			m_nLRedge[nCam][1][y1] = nMidData1[0];
		}
		double dscale1 = (nMidData[1] - nMidData[0])/(double)nh;
		double dscale2 = (nMidData1[1] - nMidData1[0])/(double)nh;
		for (int y1 = rtEdge[1].top+100; y1<rtEdge[1].bottom-100 && y1 >= 0 && y1<3000; y1++) {
			m_nLRedge[nCam][0][y1] = nMidData[0] + (dscale1*y1);
			m_nLRedge[nCam][1][y1] = nMidData1[0] + (dscale2*y1);
		}
		//에지 비교
		//if(!m_bWhiteRoll[nCam]){//브라이트이미지에서 에지 찾을시 다크고 다시 확인한다.
		//	for (int y1 = rtEdge[1].top; y1<rtEdge[1].bottom && y1 >= 0; y1++) {
		//		for(int x1 = m_nLRedge[nCam][0][y1]  ; x
		//	}
		//}
	}
	else{
		for (int y1 = rtEdge[1].top; y1<rtEdge[1].bottom && y1 >= 0 && y1 <3000; y1++) {
			m_nLRedge[nCam][0][y1] = nMidData[2];
			m_nLRedge[nCam][1][y1] = nMidData1[2];
		}
	}

	if(ncnt12[0]==0)ncnt12[0] = 1;
	if(ncnt12[1]==0)ncnt12[1] = 1;
	if(ncnt12[0]>1 &&ncnt12[1]>1 ){
		dCoatTopEdge /= ncnt12[0]; dCoatBtmEdge /= ncnt12[1];//211002
	}
	else{
		dCoatTopEdge = m_rtHugeRect[nCam].right ; dCoatBtmEdge  = m_rtHugeRect[nCam].right ;//211020
	}

	if (dCoatTopEdge < dCoatBtmEdge)dCoatTopEdge = dCoatBtmEdge;//211002

//	if(dCoatTopEdge < nMidData1[2])
		dCoatTopEdge = nMidData1[2];//중심값과 위아래 평균중 더 큰 값을 쓴다.
	noldpos[0] = noldpos[1] = m_nWidth - 100;//211017-1
	for (int y1 = rtEdge[1].top; y1<rtEdge[1].top + 200 && y1 >= 0; y1++) {
		if (nEdgeY[y1]< noldpos[0]) {
			noldpos[0] = nEdgeY[y1];
			noldposy[0] = y1;
		}
	}
	for (int y1 = rtEdge[1].bottom - 1; y1>rtEdge[1].bottom - 200 && y1 >= 0; y1--) {
		if (nEdgeY[y1]< noldpos[1]) {
			noldpos[1] = nEdgeY[y1];
			noldposy[1] = y1;
		}
	}
	m_rtCell[nCam].right = dCoatEdge1[1] = dCoatTopEdge;//211003

	int nCellLeftEdge = 0, nCellRightEdge = 0;

	if (true == m_bUseIntekCellAlign[nCam])
	{
		FindCellEdgeIntek(nCam, m_fmEven[nCam], m_fmOdd[nCam], nCellLeftEdge, nCellRightEdge);

		m_rtCell[nCam].left = nCellLeftEdge;
		m_rtCell[nCam].right = nCellRightEdge;
	}

	if (m_rtCell[nCam].Width()<1500)		m_rtCell[nCam].right = m_rtHugeRect[nCam].right*m_nReduction[nCam];//0524-2

	CRect rtins1;
	rtins1.SetRect(m_rtCell[nCam].right , 20,m_rtCell[nCam].right + 300,nh-20);//220214
	int ntop1 = 0,nbottom1 = 0;

	if(m_bUseIntekTabAlign[nCam] == FALSE)
		FindTabPos( nCam, fmBright, fmDark, nw ,  nh,  rtins1,&ntop1,&nbottom1);
	else
	{
		//double	dTabStartTime = GetPrecisionTime();
		int nTabAvg = FindTabPosIntek(nCam, fmBright, fmDark, nw, nh, rtins1, &ntop1, &nbottom1);
		//double	dTabEndTime = GetPrecisionTime();

		//CString strMsg;
		//strMsg.Format(_T("%3.3fms"), (dTabEndTime - dTabStartTime) * 1000.0f);
		//AfxMessageBox(strMsg);
		{
			CSingleLock lock(&m_lockTabAvg[nCam], TRUE);
			m_dequeTabAvg[nCam].clear();
			m_dequeTabAvg[nCam].push_back(nTabAvg);
		}
		{
			CSingleLock lock(&m_lockTabTopY[nCam], TRUE);
			m_dequeTabTopY[nCam].clear();
			m_dequeTabTopY[nCam].push_back(ntop1);
		}
		{
			CSingleLock lock(&m_lockTabBottomY[nCam], TRUE);
			m_dequeTabBottomY[nCam].clear();
			m_dequeTabBottomY[nCam].push_back(nbottom1);
		}
	}
//Find Notch Pos; //m_rtNotchArea
	//100 x 256
	for (int j = 0; j<4; j++){
		m_ptFullNotch[nCam][j].SetPoint(0, 0);
		m_ptNotch[nCam][j].SetPoint(0,0);
	}

	int nMarginY = m_dVhomeMarginY[nCam] / m_dscale_y1[nCam];//50;//Y 마진 //220202
	int nMarginX = m_dVhomeMarginX[nCam] / m_dscale_x1[nCam];//25;//80;//X 마진 //220202
	if(m_bNotch[nCam]==FALSE){
		m_rtNotchArea[nCam][0].SetRect(0,0,0,0);//
		m_rtNotchArea[nCam][1].SetRect(0,0,0,0);//
		m_rtNotchArea[nCam][2].SetRect(0,0,0,0);//
		m_rtNotchArea[nCam][3].SetRect(0,0,0,0);//

		return 0;
	}
	if(m_bFixedVhome[nCam]){
		int noffset12  = 0;
		for(int i=0;i<4;i++){
			m_rtNotchArea[nCam][i]= m_rtFixedVhome[nCam][i];
			if(i<2)
				noffset12 = m_rtCell[nCam].left - m_rtFixedVhome[nCam][i].CenterPoint().x;
			else
				noffset12 = m_rtCell[nCam].right - m_rtFixedVhome[nCam][i].CenterPoint().x;
			m_rtNotchArea[nCam][i].OffsetRect(noffset12,0);
			m_rtNotchArea[nCam][i] = SetBoundary2(m_rtNotchArea[nCam][i], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
		}
	}
	else{
		if(m_nOffsetUp[nCam] ==0  || m_nOffsetDn[nCam]==0){//값이 0이 들어가면 Cell Margin을 기본으로 한다.
			int ny1 = m_nCellMargin*0.5;
			int ny2 = nh - m_nCellMargin*0.5;
			m_rtNotchArea[nCam][0].SetRect(m_rtCell[nCam].left-nMarginX,ny1-nMarginY,m_rtCell[nCam].left+nMarginX,ny1+nMarginY);//220202
			m_rtNotchArea[nCam][1].SetRect(m_rtCell[nCam].left-nMarginX,ny2-nMarginY,m_rtCell[nCam].left+nMarginX,ny2+nMarginY);//220202
			m_rtNotchArea[nCam][2].SetRect(m_rtCell[nCam].right-nMarginX,ny1-nMarginY,m_rtCell[nCam].right+nMarginX,ny1+nMarginY);//220202
			m_rtNotchArea[nCam][3].SetRect(m_rtCell[nCam].right-nMarginX,ny2-nMarginY,m_rtCell[nCam].right+nMarginX,ny2+nMarginY);////220202
			m_rtNotchArea[nCam][0] = SetBoundary2(m_rtNotchArea[nCam][0], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
			m_rtNotchArea[nCam][1] = SetBoundary2(m_rtNotchArea[nCam][1], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
			m_rtNotchArea[nCam][2] = SetBoundary2(m_rtNotchArea[nCam][2], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
			m_rtNotchArea[nCam][3] = SetBoundary2(m_rtNotchArea[nCam][3], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역		
		}
		else if(m_bWhiteRoll[nCam]){
			CRect rt10[4];
			int ny1 = ntop1 - m_nOffsetUp[nCam];;//m_rtMinTab[nCam].top* m_nReduction[nCam] - m_nOffsetUp[nCam];
			int ny2 = nbottom1+ m_nOffsetDn[nCam];;//m_rtMinTab[nCam].bottom* m_nReduction[nCam] + m_nOffsetDn[nCam];

			BOOL bFullFind = FALSE;
			if(bFullFind){
				rt10[0].SetRect(m_rtCell[nCam].left-50,20,m_rtCell[nCam].left+50,500);//좌상
				rt10[1].SetRect(m_rtCell[nCam].left-50,nh-500,m_rtCell[nCam].left+50,nh-20);//좌하
				rt10[2].SetRect(m_rtCell[nCam].right-50,20,m_rtCell[nCam].right+50,500);//우상
				rt10[3].SetRect(m_rtCell[nCam].right-50,nh-500,m_rtCell[nCam].right+50,nh-20);//우하
				rt10[0] = SetBoundary2(rt10[0], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				rt10[1] = SetBoundary2(rt10[1], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				rt10[2] = SetBoundary2(rt10[2], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				rt10[3] = SetBoundary2(rt10[3], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
			}
			else{
				//tab에지가 ㅇ틀릴경우 확인 필요.
				//rt10[0].SetRect(m_rtCell[nCam].left-50,ny1-nMarginY,m_rtCell[nCam].left+50,ny1+nMarginY);//좌상
				//rt10[0] = SetBoundary2(rt10[0], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				//rt10[1].SetRect(m_rtCell[nCam].left-50,ny2-nMarginY,m_rtCell[nCam].left+50,ny2+nMarginY);//좌하
				//rt10[1] = SetBoundary2(rt10[1], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				//rt10[2].SetRect(m_rtCell[nCam].right-50,ny1-nMarginY,m_rtCell[nCam].right+50,ny1+nMarginY);//우상
				//rt10[2] = SetBoundary2(rt10[2], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				//rt10[3].SetRect(m_rtCell[nCam].right-50,ny2-nMarginY,m_rtCell[nCam].right+50,ny2+nMarginY);//우하
				//rt10[3] = SetBoundary2(rt10[3], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				int ny11 = m_nCellMargin*0.5;//220505//22005
				int ny12 = nh - m_nCellMargin*0.5;//220505//220505
				m_rtNotchArea[nCam][0].SetRect(m_rtCell[nCam].left-nMarginX,ny11-nMarginY,m_rtCell[nCam].left+nMarginX,ny11+nMarginY);//220202
				m_rtNotchArea[nCam][1].SetRect(m_rtCell[nCam].left-nMarginX,ny12-nMarginY,m_rtCell[nCam].left+nMarginX,ny12+nMarginY);//220202
				m_rtNotchArea[nCam][2].SetRect(m_rtCell[nCam].right-nMarginX,ny11-nMarginY,m_rtCell[nCam].right+nMarginX,ny11+nMarginY);//220202
				m_rtNotchArea[nCam][3].SetRect(m_rtCell[nCam].right-nMarginX,ny12-nMarginY,m_rtCell[nCam].right+nMarginX,ny12+nMarginY);////220202
				m_rtNotchArea[nCam][0] = SetBoundary2(m_rtNotchArea[nCam][0], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][1] = SetBoundary2(m_rtNotchArea[nCam][1], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][2] = SetBoundary2(m_rtNotchArea[nCam][2], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][3] = SetBoundary2(m_rtNotchArea[nCam][3], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역		

			}

			//CPoint pt1;
			//int nYmargin2 = nMarginY;//220202
			//for(int i=0;i<m_nNotchCount[nCam];i++){
			//	pt1 = FindNotchPos( nCam, fmBright,rt10[i],i);
			//	if(i<2)//220211
			//		pt1.x = m_rtCell[nCam].left;
			//	else
			//		pt1.x = m_rtCell[nCam].right;
			//	m_rtNotchArea[nCam][i].SetRect(pt1.x-nMarginX,pt1.y- nYmargin2,pt1.x+nMarginX,pt1.y+ nYmargin2);//220202
			//	m_ptNotch[nCam][i] = m_ptFullNotch[nCam][i] =pt1;
			//}
			if(ny1- nMarginY<0) ny1 = nMarginY;//220414
			if(ny2 >nh-nMarginY) ny2 = nh-nMarginY;//220414
			if(m_bTiltEdge[nCam]){//220404
				m_rtNotchArea[nCam][0].SetRect(m_nLRedge[nCam][0][ny1]-nMarginX,ny1-nMarginY,m_nLRedge[nCam][0][ny1]+nMarginX,ny1+nMarginY);//220202
				m_rtNotchArea[nCam][1].SetRect(m_nLRedge[nCam][0][ny2]-nMarginX,ny2-nMarginY,m_nLRedge[nCam][0][ny2]+nMarginX,ny2+nMarginY);//220202
				m_rtNotchArea[nCam][2].SetRect(m_nLRedge[nCam][1][ny1]-nMarginX,ny1-nMarginY,m_nLRedge[nCam][1][ny1]+nMarginX,ny1+nMarginY);////220202
				m_rtNotchArea[nCam][3].SetRect(m_nLRedge[nCam][1][ny2]-nMarginX,ny2-nMarginY,m_nLRedge[nCam][1][ny2]+nMarginX,ny2+nMarginY);////220202
				m_rtNotchArea[nCam][0] = SetBoundary2(m_rtNotchArea[nCam][0], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][1] = SetBoundary2(m_rtNotchArea[nCam][1], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][2] = SetBoundary2(m_rtNotchArea[nCam][2], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][3] = SetBoundary2(m_rtNotchArea[nCam][3], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
			}
			else{
				m_rtNotchArea[nCam][0].SetRect(m_rtCell[nCam].left-nMarginX,ny1-nMarginY,m_rtCell[nCam].left+nMarginX,ny1+nMarginY);//좌상
				m_rtNotchArea[nCam][0] = SetBoundary2(m_rtNotchArea[nCam][0], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][1].SetRect(m_rtCell[nCam].left-nMarginX,ny2-nMarginY,m_rtCell[nCam].left+nMarginX,ny2+nMarginY);//좌하
				m_rtNotchArea[nCam][1] = SetBoundary2(m_rtNotchArea[nCam][1], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][2].SetRect(m_rtCell[nCam].right-nMarginX,ny1-nMarginY,m_rtCell[nCam].right+nMarginX,ny1+nMarginY);//우상
				m_rtNotchArea[nCam][2] = SetBoundary2(m_rtNotchArea[nCam][2], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][3].SetRect(m_rtCell[nCam].right-nMarginX,ny2-nMarginY,m_rtCell[nCam].right+nMarginX,ny2+nMarginY);//우하
				m_rtNotchArea[nCam][3] = SetBoundary2(m_rtNotchArea[nCam][3], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
			}
		}
		else{
			//		int ny1 = m_rtMinTab[nCam].top* m_nReduction[nCam] - m_nOffsetUp[nCam];
			//		int ny2 = m_rtMinTab[nCam].bottom* m_nReduction[nCam] + m_nOffsetDn[nCam];
			int ny1 = ntop1 - m_nOffsetUp[nCam];;//m_rtMinTab[nCam].top* m_nReduction[nCam] - m_nOffsetUp[nCam];
			int ny2 = nbottom1+ m_nOffsetDn[nCam];;//m_rtMinTab[nCam].bottom* m_nReduction[nCam] + m_nOffsetDn[nCam];
			if(ny1- nMarginY<0) ny1 = nMarginY;//220414
			if(ny2 >nh-nMarginY) ny2 = nh-nMarginY;//220414

			if(m_bTiltEdge[nCam]){//220404
				m_rtNotchArea[nCam][0].SetRect(m_nLRedge[nCam][0][ny1]-nMarginX,ny1-nMarginY,m_nLRedge[nCam][0][ny1]+nMarginX,ny1+nMarginY);//220202
				m_rtNotchArea[nCam][1].SetRect(m_nLRedge[nCam][0][ny2]-nMarginX,ny2-nMarginY,m_nLRedge[nCam][0][ny2]+nMarginX,ny2+nMarginY);//220202
				m_rtNotchArea[nCam][2].SetRect(m_nLRedge[nCam][1][ny1]-nMarginX,ny1-nMarginY,m_nLRedge[nCam][1][ny1]+nMarginX,ny1+nMarginY);////220202
				m_rtNotchArea[nCam][3].SetRect(m_nLRedge[nCam][1][ny2]-nMarginX,ny2-nMarginY,m_nLRedge[nCam][1][ny2]+nMarginX,ny2+nMarginY);////220202
				m_rtNotchArea[nCam][0] = SetBoundary2(m_rtNotchArea[nCam][0], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][1] = SetBoundary2(m_rtNotchArea[nCam][1], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][2] = SetBoundary2(m_rtNotchArea[nCam][2], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][3] = SetBoundary2(m_rtNotchArea[nCam][3], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
			}
			else{
				m_rtNotchArea[nCam][0].SetRect(m_rtCell[nCam].left-nMarginX,ny1-nMarginY,m_rtCell[nCam].left+nMarginX,ny1+nMarginY);//220202
				m_rtNotchArea[nCam][1].SetRect(m_rtCell[nCam].left-nMarginX,ny2-nMarginY,m_rtCell[nCam].left+nMarginX,ny2+nMarginY);//220202
				m_rtNotchArea[nCam][2].SetRect(m_rtCell[nCam].right-nMarginX,ny1-nMarginY,m_rtCell[nCam].right+nMarginX,ny1+nMarginY);////220202
				m_rtNotchArea[nCam][3].SetRect(m_rtCell[nCam].right-nMarginX,ny2-nMarginY,m_rtCell[nCam].right+nMarginX,ny2+nMarginY);////220202
				m_rtNotchArea[nCam][0] = SetBoundary2(m_rtNotchArea[nCam][0], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][1] = SetBoundary2(m_rtNotchArea[nCam][1], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][2] = SetBoundary2(m_rtNotchArea[nCam][2], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
				m_rtNotchArea[nCam][3] = SetBoundary2(m_rtNotchArea[nCam][3], CRect(10, 0, m_nWidth - 10, m_nHeight));//211017-1  //전체 영역
			}
		}
	}

	return 0;
}

int CInspect::FindCellEdgeIntek(int nCam, BYTE* fmBright, BYTE* fmDark, int& nLeft, int& nRight)
{
	CRect rtEdge[2];

	BYTE* fm = fmBright;

	if (false == m_bSearchImage[nCam])
		fm = fmDark;

	int nw = m_nWidth;
	int nh = m_nHeight;
	int nNonCotrange = 32;//0706

	//전극 엣지 탐색 영역
	rtEdge[0].SetRect(m_nMarginSearchStart[nCam], 0 + m_nElecEdgeSearchAreaTopBotOffset[nCam], m_nMarginSearchStart[nCam] + m_nLeftElecEdgeSearchAreaSizeX[nCam], nh - m_nElecEdgeSearchAreaTopBotOffset[nCam]);//0706
	rtEdge[1].SetRect(m_nMarginSearchEnd[nCam] - m_nRightElecEdgeSearchAreaSizeX[nCam], 0 + m_nElecEdgeSearchAreaTopBotOffset[nCam], m_nMarginSearchEnd[nCam], nh - m_nElecEdgeSearchAreaTopBotOffset[nCam]);//0706//1006
	rtEdge[0] = SetBoundary2(rtEdge[0], CRect(80, 0, nw - 100, nh));
	rtEdge[1] = SetBoundary2(rtEdge[1], CRect(80, 0, nw - 100, nh));
	//전극 엣지 탐색 영역

	std::vector <int> vecValueLeftCountEvenOdd[2];
	std::vector <int> vecValueRightCountEvenOdd[2];

	int nTh = m_nElectrodeEdgeTh[nCam];

	//전극 왼쪽 엣지 탐색, TH 값보다 밝은 픽셀과 어두운 픽셀의 갯수 카운팅
	for (int x = rtEdge[0].left; x < rtEdge[0].right; x++)
	{
		int nCountEven = 0;
		int nCountOdd = 0;

		for (int y = m_nElecEdgeSearchAreaTopBotOffset[nCam]; y < m_nElecEdgeSearchAreaTopBotOffset[nCam] + m_nEdgeSearchAreaSizeY[nCam]; y++)
		{
			if (*(fm + y * nw + x) < nTh)
				nCountEven++;
			else
				nCountOdd++;
		}

		for (int y = nh - m_nElecEdgeSearchAreaTopBotOffset[nCam] - m_nEdgeSearchAreaSizeY[nCam]; y < nh - m_nElecEdgeSearchAreaTopBotOffset[nCam]; y++)
		{
			if (*(fm + y * nw + x) < nTh)
				nCountEven++;
			else
				nCountOdd++;
		}

		vecValueLeftCountEvenOdd[0].push_back(nCountEven);
		vecValueLeftCountEvenOdd[1].push_back(nCountOdd);
	}
	//전극 왼쪽 엣지 탐색, TH 값보다 밝은 픽셀과 어두운 픽셀의 갯수 카운팅

	//전극 오른쪽 엣지 탐색, TH 값보다 밝은 픽셀과 어두운 픽셀의 갯수 카운팅
	for (int x = rtEdge[1].left; x < rtEdge[1].right; x++)
	{
		int nCountEven = 0;
		int nCountOdd = 0;

		for (int y = m_nElecEdgeSearchAreaTopBotOffset[nCam]; y < m_nElecEdgeSearchAreaTopBotOffset[nCam] + m_nEdgeSearchAreaSizeY[nCam]; y++)
		{
			if (*(fm + y * nw + x) < nTh)
				nCountEven++;
			else
				nCountOdd++;
		}

		for (int y = nh - m_nElecEdgeSearchAreaTopBotOffset[nCam] - m_nEdgeSearchAreaSizeY[nCam]; y < nh - m_nElecEdgeSearchAreaTopBotOffset[nCam]; y++)
		{
			if (*(fm + y * nw + x) < nTh)
				nCountEven++;
			else
				nCountOdd++;
		}

		vecValueRightCountEvenOdd[0].push_back(nCountEven);
		vecValueRightCountEvenOdd[1].push_back(nCountOdd);
	}
	//전극 오른쪽 엣지 탐색, TH 값보다 밝은 픽셀과 어두운 픽셀의 갯수 카운팅

	int nContinuous = 0;

	int nResLeftPos = 0;
	int nResRightPos = 0;

	if (m_nEdgeSearchDirection[nCam] == 0) //왼쪽 엣지는 롤에서 전극으로 엣지 찾을 때는 앞부터 찾기
	{
		for (int i = 0; i < vecValueLeftCountEvenOdd[0].size(); i++)
		{
			if (vecValueLeftCountEvenOdd[0][i] > vecValueLeftCountEvenOdd[1][i])
			{
				nContinuous++;
			}
			else
				nContinuous = 0;


			if (nContinuous >= m_nEdgeContinuousCount[nCam])
			{
				nResLeftPos = rtEdge[0].left + i - m_nEdgeContinuousCount[nCam];
				break;
			}
		}
	}
	else //왼쪽 엣지는 전극에서 롤로 엣지 찾을 때는 뒤에서부터 거꾸로 찾기
	{
		for (int i = vecValueLeftCountEvenOdd[0].size() - 1; i > -1; i--)
		{
			if (vecValueLeftCountEvenOdd[0][i] < vecValueLeftCountEvenOdd[1][i])
			{
				nContinuous++;
			}
			else
				nContinuous = 0;


			if (nContinuous >= m_nEdgeContinuousCount[nCam])
			{
				nResLeftPos = rtEdge[0].left + i + m_nEdgeContinuousCount[nCam];
				break;
			}
		}
	}

	nContinuous = 0;

	if (m_nEdgeSearchDirection[nCam] == 0) //오른쪽 엣지는 롤에서 전극으로 엣지 찾을 때는 뒤에서부터 거꾸로 찾기
	{
		for (int i = vecValueRightCountEvenOdd[0].size() - 1; i > -1; i--)
		{
			if (vecValueRightCountEvenOdd[0][i] > vecValueRightCountEvenOdd[1][i])
			{
				nContinuous++;
			}
			else
				nContinuous = 0;

			if (nContinuous >= m_nEdgeContinuousCount[nCam])
			{
				nResRightPos = rtEdge[1].left + i + m_nEdgeContinuousCount[nCam];
				break;
			}
		}
	}
	else //오른쪽 엣지는 전극에서 롤로 엣지 찾을 때는 앞부터 찾기
	{
		for (int i = 0; i < vecValueRightCountEvenOdd[0].size(); i++)
		{
			if (vecValueRightCountEvenOdd[0][i] < vecValueRightCountEvenOdd[1][i])
			{
				nContinuous++;
			}
			else
				nContinuous = 0;

			if (nContinuous >= m_nEdgeContinuousCount[nCam])
			{
				nResRightPos = rtEdge[1].left + i - m_nEdgeContinuousCount[nCam];
				break;
			}
		}
	}

	std::vector <int> vec2ndCandidateX;
 
	

	//2차 엣지 탐색
	for (int y = m_nElecEdgeSearchAreaTopBotOffset[nCam]; y < m_nElecEdgeSearchAreaTopBotOffset[nCam] + m_nEdgeSearchAreaSizeY[nCam]; y++)
	{
		for (int x = nResRightPos - 10; x < nResRightPos; x++) //2차 엣지 탐색 시에는 1차 엣지 좌표에서 안쪽으로 10픽셀까지만 들여다본다
		{
			if (*(fm + y * nw + x) > nTh)
			{
				vec2ndCandidateX.push_back(x);
				break;
			}
		}
	}

	for (int y = nh - m_nElecEdgeSearchAreaTopBotOffset[nCam] - m_nEdgeSearchAreaSizeY[nCam]; y < nh - m_nElecEdgeSearchAreaTopBotOffset[nCam]; y++)
	{
		for (int x = nResRightPos - 10; x < nResRightPos; x++) //2차 엣지 탐색 시에는 1차 엣지 좌표에서 안쪽으로 10픽셀까지만 들여다본다
		{
			if (*(fm + y * nw + x) > nTh)
			{
				vec2ndCandidateX.push_back(x);
				break;
			}
		}
	}
	//2차 엣지 탐색

	std::vector <int> vec2ndValueList;

	for (int i = 0; i < vec2ndCandidateX.size(); i++)
	{
		bool bPush = true;

		if (vec2ndValueList.size() > 0)
		{
			for (int j = 0; j < vec2ndValueList.size(); j++)
			{
				if (vec2ndValueList[j] == vec2ndCandidateX[i])
					bPush = false;
			}
		}
		else
		{
			vec2ndValueList.push_back(vec2ndCandidateX[i]);
			bPush = false;
		}

		if (bPush)
			vec2ndValueList.push_back(vec2ndCandidateX[i]);
	}

	std::sort(vec2ndValueList.begin(), vec2ndValueList.end()); //2차 엣지 포인트들 오름차순 정렬

	int* nValueCount = new int[vec2ndValueList.size()];

	memset(nValueCount, 0, sizeof(int)* vec2ndValueList.size());

	int nCount = m_n2ndEdgeCount[nCam];

	int n2ndX = -1;

	for (int i = 0; i < vec2ndCandidateX.size(); i++)
	{
		for (int j = 0; j < vec2ndValueList.size(); j++)
		{
			if (vec2ndValueList[j] == vec2ndCandidateX[i])
				nValueCount[j]++;

			if (nCount <= nValueCount[j]) //각 포인트 좌표 값의 갯수를 카운팅해서 갯수가 2차 엣지 포인트 카운트 이상이며 전극에 가장 가까운 포인트를 찾는다
			{
				n2ndX = vec2ndValueList[j];
				break;
			}
		}
	}

	if (n2ndX > 0) //2차 엣지 포인트 못 찾았으면 그냥 1차 포인트로 쓴다
	{
		int nTemp = nResRightPos;

		if (nResRightPos > 0)
			nResRightPos = (nTemp + n2ndX) / 2; //1차 포인트와 2차 포인트의 평균 값을 최종 엣지로 쓴다
		else
			nResRightPos = n2ndX; //1차로 엣지를 못 찾았다면 2차 포인트만 쓴다
	}

	if (nResLeftPos > nResRightPos)
	{
		nResLeftPos = 0;
		nResRightPos = 0;
	}

	nLeft = nResLeftPos;
	nRight = nResRightPos;

	vecValueLeftCountEvenOdd[0].clear();
	vecValueLeftCountEvenOdd[1].clear();
	vecValueRightCountEvenOdd[0].clear();
	vecValueRightCountEvenOdd[1].clear();
	vec2ndCandidateX.clear();
	vec2ndValueList.clear();

	delete[]nValueCount;

	return 0;
}
//220110  bright image로 처리

bool CInspect::FindBumpy(int nCam, BYTE* fmBright, BYTE* fmDark, CRect& rtDefect)
{
	int nBumpyAreaSize = m_nBumpyAreaSize[nCam];//코팅 주름 불량 검색 영역
	int nTh = m_nBumpyTh[nCam];//코팅 주름 TH (최대 밝기 값과 최소 밝기 값의 차이 값)
	int nEdgeTh = m_nBumpyEdgeTh[nCam];//코팅 주름 엣지 TH (사이즈 측정과 전극과의 밝기 차이로 검출할 때 사용)

	CRect rtFindBumpy = CRect(0, 0, 0, 0);
	rtFindBumpy.left = m_rtCell[nCam].right - nBumpyAreaSize;
	rtFindBumpy.right = m_rtCell[nCam].right;
	rtFindBumpy.top = m_rtNotchArea[nCam][2].bottom;
	rtFindBumpy.bottom = m_rtNotchArea[nCam][3].top;

	m_rtBumpyArea[nCam] = rtFindBumpy;

	rtDefect = CRect(0, 0, 0, 0);

	if (rtFindBumpy.Width() <= 0 || rtFindBumpy.Height() <= 0)//ROI 사이즈가 이상하다면 검사하지 않음
		return false;

	std::vector <int> vecY_Avg;

	for (int y = rtFindBumpy.top; y <= rtFindBumpy.bottom; y++)
	{
		int nSum = 0;

		for (int x = rtFindBumpy.left; x <= rtFindBumpy.right; x++)
		{
			nSum += *(fmBright + y * m_nWidth + x);
		}

		if (rtFindBumpy.Width() > 0)
			vecY_Avg.push_back(nSum / rtFindBumpy.Width());//코팅 주름 영역 내의 Y 좌표별 평균 밝기 값 (X Sum해서 평균으로)
		else
			vecY_Avg.push_back(0);
	}

	std::sort(vecY_Avg.begin(), vecY_Avg.end());

	int nMinBrightness = 0;		//평균 밝기에서 최소 값
	int nMaxBrightness = 0;		//평균 밝기에서 최대 값

	if (vecY_Avg.size() > 0)	// 벡터 사이즈가 0일 때, 예외 처리
	{
		nMinBrightness = vecY_Avg.front();
		nMaxBrightness = vecY_Avg.back();
	}

	int nSum = 0;

	for (int i = 0; i < vecY_Avg.size(); i++)
	{
		nSum += vecY_Avg[i];
	}

	int nTotalAverage = 0;

	if (vecY_Avg.size() > 0)
		nTotalAverage = nSum / vecY_Avg.size();//코팅 주름 영역 내의 전체 평균 밝기 값

	std::vector <int> vecInnerY_Avg;

	for (int y = rtFindBumpy.top; y <= rtFindBumpy.bottom; y += 3)//전극 영역 밝기 평균 구할꺼라서 3라인에 한번씩만 본다 (굳이 전체를 볼 필요는 없을 것 같아서)
	{
		int nSum = 0;
		int nCount = 0;

		for (int x = rtFindBumpy.left - 200; x <= rtFindBumpy.right - 200; x++) //코팅 주름 영역에서 전극 안쪽으로 200 픽셀 들어가서
		{
			nSum += *(fmBright + y * m_nWidth + x);
			nCount++;
		}

		if (nCount > 0)
			vecInnerY_Avg.push_back(nSum / nCount);//전극 안쪽 영역의 대략적인 Y 좌표별 평균 밝기 값 (작은 영역만 구해서 정확하지는 않음, 단지 코팅 주름 영역과의 차이를 보기 위함)
		else
			vecInnerY_Avg.push_back(0);
	}

	nSum = 0;

	for (int i = 0; i < vecInnerY_Avg.size(); i++)
	{
		nSum += vecInnerY_Avg[i];
	}

	int nTotalInnerAverage = 0;

	if (vecInnerY_Avg.size() > 0)
		nTotalInnerAverage = nSum / vecInnerY_Avg.size();//전극 안쪽 영역의 대략적인 평균 밝기 값

	bool bRes = false;
	bool bWrinkle = true;

	for (int i = 0; i < m_nBumpyCompareCount[nCam] && i< vecY_Avg.size(); i++)//최소 값과 최대 값 하나만 비교하는건 과검이 많이 생길 것 같아서 가장 밝기가 낮은 값부터 n개 확인할 수 있게
	{
		if (nMaxBrightness - vecY_Avg[i] > nTh)//최대 밝기 값과의 차이
			continue;
		else
		{
			bWrinkle = false;//n개 들여다보면서 하나라도 조건을 만족하지 못 한다면 
			break;
		}
	}

	if (nTotalInnerAverage + nEdgeTh < nTotalAverage || bWrinkle)//코팅 주름 영역이 전극 영역에 비해서 너무 밝거나 영역 안에서 주름으로 예상되는 부분이 있었을 경우
	{
		//불량 X 사이즈 정확하게 찾기
		bool bDarkLine = false;
		bool bConfirmBrightDiff = false;

		if (bWrinkle)
			bDarkLine = true;//주름으로 예상되는 부분이 있었을 경우에는 다른 조건없이 불량이라고 확정

		if (nTotalInnerAverage + nEdgeTh < nTotalAverage)//코팅 주름 영역이 전극 영역에 비해서 밝은 조건으로 걸렸을 경우에는 과검 방지를 위해 다른 검사를 한번 더 한다
		{
			int nCount = 0;

			for (int y = rtFindBumpy.top; y <= rtFindBumpy.bottom; y++)
			{
				for (int x = rtFindBumpy.left - 50; x <= rtFindBumpy.right; x++)//코팅 주름 영역에서 전극 안쪽으로 50 픽셀 이동 후 스캔한다
				{
					if (x > 0 && x - 1 > 0 && x - 2 > 0 && x - 3 > 0 && x - 4 > 0 &&
						x < m_nWidth && x + 1 < m_nWidth && x + 2 < m_nWidth && x + 3 < m_nWidth
						&& x + 4 < m_nWidth && x + 5 < m_nWidth)
					{
						if (*(fmBright + y * m_nWidth + x) + nEdgeTh < *(fmBright + y * m_nWidth + x + 1) ||//□□□□■■□□□□
							*(fmBright + y * m_nWidth + x - 1) + nEdgeTh < *(fmBright + y * m_nWidth + x + 2) ||//□□□■□□■□□□
							*(fmBright + y * m_nWidth + x - 2) + nEdgeTh < *(fmBright + y * m_nWidth + x + 3) ||//□□■□□□□■□□
							*(fmBright + y * m_nWidth + x - 3) + nEdgeTh < *(fmBright + y * m_nWidth + x + 4) ||//□■□□□□□□■□
							*(fmBright + y * m_nWidth + x - 4) + nEdgeTh < *(fmBright + y * m_nWidth + x + 5))//■□□□□□□□□■
							//네모가 채워진 곳이 밝기 비교하는 픽셀이며 X방향으로 이동하여 이동할 때마다 <--> 이렇게 5픽셀까지 밝기를 비교한다
						{
							nCount++;
							break;
						}
					}

					if (nCount >= 10)//위에서 스캔한 조건이 10번 이상 만족한다면 진짜 불량으로 확정
					{
						bConfirmBrightDiff = true;
						break;
					}
				}
			}
		}

		if (true == bDarkLine || true == bConfirmBrightDiff)
		{
			std::vector <int> vecX_Pos;

			int nResX = 0;
			int nCount = 0;

			for (int y = rtFindBumpy.top; y <= rtFindBumpy.bottom; y++)
			{
				for (int x = rtFindBumpy.left - 50; x <= rtFindBumpy.right - 10; x++)
				{
					if (*(fmBright + y * m_nWidth + x) + nEdgeTh < *(fmBright + y * m_nWidth + x + 1))//불량 사이즈 측정을 위해 밝기 차이가 발생한 지점을 찾는다
					{
						vecX_Pos.push_back(x);
						nCount++;
						break;
					}
				}

				if (nCount > 30)
				{
					int nSum = 0;

					for (int i = 0; i < vecX_Pos.size(); i++)
					{
						nSum += vecX_Pos[i];
					}

					if (vecX_Pos.size() > 0)
						nResX = nSum / vecX_Pos.size();//밝기 차이가 발생한 X 좌표를 찾아서 평균 낸다 

					break;
				}
			}

			rtDefect = m_rtBumpyArea[nCam];

			if (nResX > 0)
				rtDefect.left = nResX;//위에서 계산한 X 평균을 불량 Rect의 왼쪽 좌표로 사용한다

			bRes = true;
		}
	}

	vecY_Avg.clear();

	return bRes;
}

CPoint CInspect::FindNotchPos(int nCam, BYTE * fm,CRect rt,int nCnt)
{
	CRect rt1 = rt;
	CPoint pt1;
	pt1 = rt.CenterPoint();
	int nLeftAve=0,nRightAve=0;
	int nProj1[1000]={0,},nxcnt=0;
	if(rt1.Width()>=1000)
		rt1.right -= rt1.Width()-990;
	for(int y1= rt1.top;y1<rt1.bottom;y1++){
		for(int x1 = rt1.left;x1<rt1.left+5;x1++){
			nLeftAve += *(fm+y1*m_nWidth+x1);
		}
		for(int x1 = rt1.right-1;x1>rt1.right-6;x1--){
			nRightAve += *(fm+y1*m_nWidth+x1);
		}
	}
	if(rt1.Height()>0){
		nLeftAve /= rt1.Height()*5;
		nRightAve /= rt1.Height()*5;
	}
	int nTh1 = (nLeftAve+nRightAve)*0.5 , nTh2 = 40;
	int nPreakPos[2]={0,};
	if(nCnt<2){
		for(int y1= rt1.top;y1<rt1.bottom;y1++){//단차롤로 인해 전극에서 롤방향으로 서치해야함.
			for(int x1 = rt1.left;x1<rt1.right;x1++){
				if(*(fm+y1*m_nWidth+x1) < nTh1 && *(fm+y1*m_nWidth+x1+3) < nTh1){
					if(nPreakPos[0] < x1){
						nPreakPos[0] = x1 +4;
						nPreakPos[1] = y1;
					}
					break;
				}
			}
		}
	}
	else{
		nPreakPos[0] = 100000;
		for(int y1= rt1.top;y1<rt1.bottom;y1++){
			for(int x1 = rt1.right;x1>rt1.left;x1--){
				if(*(fm+y1*m_nWidth+x1) < nTh1 && *(fm+y1*m_nWidth+x1-3) < nTh1){
					if(nPreakPos[0] > x1){
						nPreakPos[0] = x1 -4;
						nPreakPos[1] = y1;
					}
					break;
				}
			}
		}

	}
	if(nPreakPos[0] >0 && nPreakPos[1]>0){
		pt1.SetPoint(nPreakPos[0],nPreakPos[1]);
	}

	return pt1;
}

int CInspect::ROIInspect1(int nCam,BYTE * fmw,BYTE* fmb, int nw, int nh, CRect rt)
{
	CRect rtins = rt;
	for(int i=0;i<20000;i++){
		m_nManualLeftEdge[i] = m_nManualRightEdge[i]=0;
	}
	BYTE * fmtmp = new BYTE[nw*nh];
	memset(fmtmp,0,nw*nh);
	//영상에서 왼쪽 에지만 찾는다.
	int nEdgeTh1 = 200,nEdgeMargin = 125 ,nfisrtLeft = 100000,ncnt1=0,nRightGap1 = 1900,nRightGap2 = 2200,nRightGap3 = 1756;
	int nTopgap1 = 300,nTopgap2 = 7400,nCnt2 = 0,nXGap=-3;
	for (int y = rtins.top; y<rtins.bottom; y ++){
		for (int x = rtins.left; x<rtins.right; x++){
			if(*(fmw+y*nw+x) > nEdgeTh1)
			{
				ncnt1 = 0;
				for(int x1 = x;x1<x+5;x1++){
					if(*(fmw+y*nw+x1) > nEdgeTh1){
						ncnt1++;
					}
				}
				if(ncnt1>=4){
					nCnt2++;
					if(nCnt2 < nTopgap1)
						break;
					if(nCnt2>= nTopgap2)
						break;
					m_nManualLeftEdge[y] = x-nXGap;
					for(int x1 = x+nEdgeMargin;x1<x+nRightGap3;x1++){
						if(x1>rtins.right)continue;

						if(*(fmw+y*nw+x1)>200){
							*(fmtmp+y*nw+x1) = 0xff;
						}
//						*(fmtmp+y*nw+x1) = *(fmw+y*nw+x1);
					}
					break;
				}
			}
		}
	}

	m_pManualChain->SetChainData(1, fmtmp, 1, 1, 2, 1000000, nw, nh);//white
	int nBlobCount = m_pManualChain->FastChain(rtins.left + 4, rtins.top + 4, rtins.right - 4, rtins.bottom - 4);
	int nDefectCount = 0;
	for (int i = 0; i<nBlobCount; i++)
	{
		double  dTemp = m_pManualChain->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp<5)continue;
		int nx1 = m_pManualChain->FindMinX(i);
		int nx2 = m_pManualChain->FindMaxX(i);
		int ny1 = m_pManualChain->FindMinY(i);
		int ny2 = m_pManualChain->FindMaxY(i);
		double dx1 = (nx2 - nx1)*m_dscale_x1[0];
		double dy1 = (ny2 - ny1)*m_dscale_y1[0];
		double dratio1 = dx1/dy1;
		double dratio2 = dy1/dx1;
		double dcx,dcy,dmindist,dmaxdist,davedist;
		if(dratio1 < dratio2)dratio1 = dratio2;
		m_pManualChain->Chain_Center(i, &dcx, &dcy);
		m_pManualChain->FineDistFromPoint(i, dcx, dcy, &dmindist, &dmaxdist, &davedist);//장축 단축 구하기  //211008

		m_dManualSizeX[nDefectCount][0] = (nx2 - nx1)*m_dscale_x1[0];
		m_dManualSizeY[nDefectCount][0] = (ny2 - ny1)*m_dscale_y1[0];
		BOOL bdefect = FALSE;

		if ((m_dManualSizeX[nDefectCount][0] >= m_dPinholeMinSize[nCam] && m_dManualSizeY[nDefectCount][0] >= m_dPinholeMinSize[nCam]) && (m_dManualSizeX[nDefectCount][0] >= m_dPinholeSizeX[nCam] || m_dManualSizeY[nDefectCount][0] >= m_dPinholeSizeY[nCam]) ) {
		//핀홀
			double dcx = 0, dcy = 0;
			m_pManualChain->Chain_Center(i, &dcx, &dcy);
			//불량
			m_ptManualDefectPoint[nDefectCount].SetPoint(dcx, dcy);
			m_rtManualDefect[nDefectCount].SetRect(dcx - BAD_WIDTH*0.5, dcy - BAD_HEIGHT*0.5, dcx + BAD_WIDTH*0.5, dcy + BAD_HEIGHT*0.5);
			m_dManualSizeX[nDefectCount][0] = (nx2 - nx1)*m_dscale_x1[0];
			m_dManualSizeY[nDefectCount][0] = (ny2 - ny1)*m_dscale_y1[0];
			m_nManualDefectType[nDefectCount] = 2001;//pinhole
			bdefect = TRUE;
		}
		if(dratio1>=m_dXYRatio[0]){//라인
			if(m_bLineSizeAndOr){
				if ((m_dManualSizeX[nDefectCount][0] >= m_dLineMinSize[nCam] && m_dManualSizeY[nDefectCount][0]  >= m_dLineMinSize[nCam]) && (m_dManualSizeX[nDefectCount][0]  > m_dWhiteLineSizeX[nCam] && m_dManualSizeY[nDefectCount][0]  > m_dWhiteLineSizeY[nCam]) ) {
					double dcx = 0, dcy = 0;
					m_pManualChain->Chain_Center(i, &dcx, &dcy);
					//불량
					m_ptManualDefectPoint[nDefectCount].SetPoint(dcx, dcy);
					m_rtManualDefect[nDefectCount].SetRect(dcx - BAD_WIDTH*0.5, dcy - BAD_HEIGHT*0.5, dcx + BAD_WIDTH*0.5, dcy + BAD_HEIGHT*0.5);
					m_dManualSizeX[nDefectCount][0] = (nx2 - nx1)*m_dscale_x1[0];
					m_dManualSizeY[nDefectCount][0] = (ny2 - ny1)*m_dscale_y1[0];
					//			m_rtManualDefect[nDefectCount] = SetBoundary2(m_rtManualDefect[nDefectCount],rtins);
					m_nManualDefectType[nDefectCount] = 1001;
					bdefect = TRUE;
				}
			}
			else{
				if ((m_dManualSizeX[nDefectCount][0] >= m_dLineMinSize[nCam] && m_dManualSizeY[nDefectCount][0]  >= m_dLineMinSize[nCam]) && (m_dManualSizeX[nDefectCount][0]  > m_dWhiteLineSizeX[nCam] || m_dManualSizeY[nDefectCount][0]  > m_dWhiteLineSizeY[nCam]) ) {
					double dcx = 0, dcy = 0;
					m_pManualChain->Chain_Center(i, &dcx, &dcy);
					//불량
					m_ptManualDefectPoint[nDefectCount].SetPoint(dcx, dcy);
					m_rtManualDefect[nDefectCount].SetRect(dcx - BAD_WIDTH*0.5, dcy - BAD_HEIGHT*0.5, dcx + BAD_WIDTH*0.5, dcy + BAD_HEIGHT*0.5);
					m_dManualSizeX[nDefectCount][0] = (nx2 - nx1)*m_dscale_x1[0];
					m_dManualSizeY[nDefectCount][0] = (ny2 - ny1)*m_dscale_y1[0];
					//			m_rtManualDefect[nDefectCount] = SetBoundary2(m_rtManualDefect[nDefectCount],rtins);
					m_nManualDefectType[nDefectCount] = 1001;
					bdefect = TRUE;
				}
			}
		}
		//if (dx1> m_dIslandSizeX[nCam] && dy1 > m_dIslandSizeY[nCam]){
		//	double dcx = 0, dcy = 0;
		//	m_pManualChain->Chain_Center(i, &dcx, &dcy);
		//	//불량
		//	m_ptManualDefectPoint[nDefectCount].SetPoint(dcx, dcy);
		//	m_rtManualDefect[nDefectCount].SetRect(dcx - BAD_WIDTH*0.5, dcy - BAD_HEIGHT*0.5, dcx + BAD_WIDTH*0.5, dcy + BAD_HEIGHT*0.5);
		//	m_dManualSizeX[nDefectCount][0] = (nx2 - nx1)*m_dscale_x1[0];
		//	m_dManualSizeY[nDefectCount][0] = (ny2 - ny1)*m_dscale_y1[0];
		//	//			m_rtManualDefect[nDefectCount] = SetBoundary2(m_rtManualDefect[nDefectCount],rtins);
		//	m_nManualDefectType[nDefectCount] = 5001;
		//	nDefectCount++;

		//}
		if(bdefect)
			nDefectCount++;
	}
	m_nManualDefectCount = nDefectCount;//0706

//	memcpy(fmw,fmtmp,nw*nh);

	delete [] fmtmp;
	return nDefectCount;
}

//int CInspect::checkParam()
//{
//	int nret = 0;
//// 	CString str;
//// 	str.Format(_T("C:\\Temp\\InsulTh.txt"));
//// 	char ch[40];
//// 	FILE *fp;
//// 	if( (fp = fopen((LPCSTR)str, "r")) ==NULL){
//// 		return nret;
//// 	}
//// 	else{
//// 		fgets(ch,1024,fp);
//// 	}
//// 
//// 	fcloseall();
//// 	CString str1;
//// 	str1.Format("%s",ch);
//// 	nret = atoi(str1);
//	return nret;
//
//}

void CInspect::ResetData(int nCam)
{
	m_bThreadStop[nCam] = FALSE;
	m_bOverlayError[nCam] = FALSE;
	m_nFullDefectCount[nCam] = 0;
	m_nScrabCount[nCam] = 0;//220112
	m_dMiddleAve[nCam] = 0;
	m_dMiddleAveDark[nCam] = 0;//220110
	m_nInsulateDefectCount[nCam] = 0;

	for(int i = 0; i < MAX_FULL_DEFECT; i++){
		m_rtDetailRawDefect[nCam][i].SetRect(0, 0, 0, 0);
		m_dDetailFullArea[nCam][i] = 0.0f;
		m_dDetailFullWidth[nCam][i] = 0.0f;
		m_dDetailFullLength[nCam][i] = 0.0f;

		m_nFullDefectType[nCam][i] = 0;
		m_strFullDefectName[nCam][i] =  _T("");
		m_strFullDefectNameScrab[nCam][i] =  _T("");
		m_rtFullDefectScrab[nCam][i].SetRect(0,0,0,0);
		m_nFullDefectTypeScrab[nCam][i] = 0;
		for(int j=0;j<3;j++){
			m_dFullSizeX[nCam][i][j] = 0;
			m_dFullSizeY[nCam][i][j] = 0;
			m_dFullAngle[nCam][i][j] = 0;
			m_dFullSizeXScrab[nCam][i][j] = 0;
			m_dFullSizeYScrab[nCam][i][j] = 0;
		}
		for(int j=0;j<2;j++){
			m_dFullCompactness[nCam][i][j] = 0;
			m_dFullStdValue[nCam][i][j] = 0;
			m_dFullDistance[nCam][i][j] = 0;
			m_dFullPercent[nCam][i][j] = 0;
			m_dFullPeakDataW[nCam][i][j] = 0;
			m_dFullPeakDataB[nCam][i][j] = 0;
		}
	}

	for (int j = 0; j < MOD_NUMBER; j++)
	{
		for (int split = 0; split < MAX_SPLIT_DEFECT; split++)
		{
			for (int dt = 0; dt < DT_TOTAL; dt++)
			{
				for (int pm = 0; pm < 2; pm++)
				{
					m_dDefectAreaSub[nCam][j][split][dt][pm] = 0.0f;

					m_dDefectCenterPosSubX[nCam][j][split][dt][pm] = 0.0f;
					m_dDefectCenterPosSubY[nCam][j][split][dt][pm] = 0.0f;

					m_dDefectWidthSub[nCam][j][split][dt][pm] = 0.0f;
					m_dDefectLengthSub[nCam][j][split][dt][pm] = 0.0f;

					m_rtSubRawDefect[nCam][j][split][dt][pm].SetRect(0, 0, 0, 0);
				}

			}
		}

	}

}

int CInspect::ROIOverlay1(int nCam, BYTE * fmEven, BYTE * fmOdd, int nw, int nh, CRect rt,BOOL bImgWrite)
{
	CRect rtins = rt;
	int *nProj1 = new int[nh];//전극 에지
	int *nProj2 = new int[nh];//오버레이
	int *nProj3 = new int[nh];//절연
	BYTE * fmnew = new BYTE[nw*nh];
	memcpy(fmnew, fmEven, nw*nh);

	for(int i=0;i<100;i++){
		for(int j=0;j<2;j++){
			m_dManualOverlay1[i][j]=0;//0 mm 1 pixel
			m_dManualInsulate1[i][j]=0;//0 mm 1 pixel
			m_dManualCoat1[i][j]=0;//0 mm 1 pixel
			m_dManualOverlay2[i][j]=0;//0 mm 1 pixel
			m_dManualInsulate2[i][j]=0;//0 mm 1 pixel
			m_dManualCoat2[i][j]=0;//0 mm 1 pixel
		}
	}
	for (int i = 0; i<nh; i++){
		nProj1[i] = nProj2[i] = nProj3[i]=0;
	}
//우측 에지를 찾는다.
	int nEdgeBright = 200;
	int nright1 = 0,nright1cnt=0;

	BYTE * fmtmp = new BYTE[nw*nh];
	memset(fmtmp,0,nw*nh);
	//영상에서 왼쪽 에지만 찾는다.
	int nEdgeTh1 = 180,nEdgeMargin = 1850 ,nfisrtLeft = 100000,ncnt1=0,nRightGap1 = 1900,nRightGap2 = 2200,nRightGap3 = 2200,nblobth1 = 150;
	int nTopgap1 = 300,nTopgap2 = 7400,nCnt2 = 0,nXGap=-3;
	for (int y = rtins.top; y<rtins.bottom; y ++){
		for (int x = rtins.left; x<rtins.right; x++){
			if(*(fmEven+y*nw+x) > nEdgeTh1)
			{
				ncnt1 = 0;
				for(int x1 = x;x1<x+5;x1++){
					if(*(fmEven+y*nw+x1) > nEdgeTh1){
						ncnt1++;
					}
				}
				if(ncnt1>=4){
					nCnt2++;
					if(nCnt2 < nTopgap1)
						break;
					if(nCnt2>= nTopgap2)
						break;
					m_nManualLeftEdge[y] = x-nXGap;
					for(int x1 = x+nEdgeMargin;x1<x+nRightGap3;x1++){
						if(x1>rtins.right)continue;

						if(*(fmEven+y*nw+x1)>nblobth1){
							*(fmtmp+y*nw+x1) = 0xff;
						}
						//						*(fmtmp+y*nw+x1) = *(fmw+y*nw+x1);
					}
					break;
				}
			}
		}
	}
	m_pManualChain->SetChainData(1, fmtmp, 1, 1, 500, 1000000, nw, nh);//white
	int nBlobCount = m_pManualChain->FastChain(rtins.left + 4, rtins.top + 4, rtins.right - 4, rtins.bottom - 4);
	int nTabcnt = 0;
	int nHeightcut = 310;//절연탭 높이
	CRect rttab[100];
	int nTBSkip = 80,nleftgap1 = 150;
	for (int i = 0; i<nBlobCount; i++)
	{
		double  dTemp = m_pManualChain->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp<1000)continue;
		int nx1 = m_pManualChain->FindMinX(i);
		int nx2 = m_pManualChain->FindMaxX(i);
		int ny1 = m_pManualChain->FindMinY(i);
		int ny2 = m_pManualChain->FindMaxY(i);
		double dx1 = (nx2 - nx1);
		double dy1 = (ny2 - ny1);
		if(dy1 < nHeightcut)
			continue;
		else{
			rttab[nTabcnt].SetRect(nx1-nleftgap1,ny1+nTBSkip,nx2,ny2-nTBSkip);
			nTabcnt++;
		}
		if(nTabcnt>90)
			break;

	}
	//전극 에지 찾기
	int nPeak = 0,nPeak1 = 0;
	double dtabEdge1[100]={0.,}, dtabEdge2[100]={0.,}, dtabEdge3[100]={0.,};
	for(int k=0;k<nTabcnt;k++){
		for(int y=rttab[k].top;y<rttab[k].bottom;y++){
			nPeak =nPeak1 = 0;
			for(int x=rttab[k].left;x<rttab[k].left+nleftgap1;x++){
				int ntmp =  *(fmEven+y*nw+x+2) - *(fmEven+y*nw+x);
				int ntmp1 =  *(fmEven+y*nw+x+2) - *(fmEven+y*nw+x);
				if(ntmp > nPeak){
					nProj1[y] = x+1;nPeak = ntmp;
				}
			}
			dtabEdge1[k] += nProj1[y];
		}
		dtabEdge1[k] /= rttab[k].Height();//전극에지
	}
	//Overay
	int nOverlayGap1 = 80;
	for(int k=0;k<nTabcnt;k++){
		int nave1 = 0,nave1cnt=0;
		for(int y=rttab[k].top;y<rttab[k].bottom;y++){
			for(int x=nProj1[y]-nOverlayGap1;x<nProj1[y];x++){
				nave1 += *(fmEven+y*nw+x);nave1cnt++;
			}
		}
		if(nave1cnt>0)
			nave1 /= nave1cnt;
		int nth1 = nave1*0.88;
		nave1cnt = 0;
		for(int y=rttab[k].top;y<rttab[k].bottom;y++){
			for(int x=nProj1[y]-nOverlayGap1;x<nProj1[y];x++){
				if(*(fmEven+y*nw+x) < nth1 && *(fmEven+y*nw+x+3) < nth1){
					nProj2[y] = x;dtabEdge2[k] += x;nave1cnt++;break;
				}
			}
		}
		if(nave1cnt>0)dtabEdge2[k] /= nave1cnt;
	}
	int nInsulGap1 = 125;
	for(int k=0;k<nTabcnt;k++){
		int nave1 = 0,nave1cnt=0;
		int nHistoW[256]={0,};
		for(int y=rttab[k].top;y<rttab[k].bottom;y++){
			for(int x=nProj1[y];x<nProj1[y]+nInsulGap1;x++){
				nave1 += *(fmOdd+y*nw+x);nave1cnt++;
				nHistoW[*(fmOdd+y*nw+x)]++;
			}
		}
		if(nave1cnt>0)
			nave1 /= nave1cnt;
		int nth1 = nave1;
		//평균밝기 위에 백색이 가장 많은곳
		int nHistomax1=0,nHistomax2=0;
		for(int i=255;i>nave1;i--){
			if(nHistomax1 > 100){
				nHistomax1 = nHistoW[i];
				nHistomax2 = i;
				break;
			}
			nHistomax1 +=nHistoW[i];
		}
		nth1 = (nHistomax2 + nave1)*0.5;
		nave1cnt = 0;
		for(int y=rttab[k].top;y<rttab[k].bottom;y++){
			for(int x=nProj1[y]+nInsulGap1;x>nProj1[y];x--){
				if(*(fmOdd+y*nw+x) < nth1 && *(fmOdd+y*nw+x-3) < nth1){
					nProj3[y] = x;dtabEdge3[k] += x;nave1cnt++;break;
				}
			}
		}
		if(nave1cnt>0)dtabEdge3[k] /= nave1cnt;
	}
	for(int k=0;k<nTabcnt;k++){
		m_dManualOverlay1[k][1]=dtabEdge2[k];//0 mm 1 pixel
		m_dManualInsulate1[k][1]=dtabEdge3[k];//0 mm 1 pixel
		m_dManualCoat1[k][1]=dtabEdge1[k];//0 mm 1 pixel
		m_dManualOverlay1[k][0] = (m_dManualCoat1[k][1] - m_dManualOverlay1[k][1]) * m_dscale_x1[nCam];
		m_dManualInsulate1[k][0] = (m_dManualInsulate1[k][1] - m_dManualCoat1[k][1]) * m_dscale_x1[nCam];
		m_dManualOverlay2[k][1]=dtabEdge2[k];//0 mm 1 pixel
		m_dManualInsulate2[k][1]=dtabEdge3[k];//0 mm 1 pixel
		m_dManualCoat2[k][1]=dtabEdge1[k];//0 mm 1 pixel
		double dover=0,dinsul=0;
		double dSortData1[1000]={0,},dSortData2[1000]={0,};
		int ncnt12 = 0,ncnt13 = 0;
		for(int y=rttab[k].top;y<rttab[k].bottom;y++){
//			dover += (nProj1[y] - nProj2[y]);
//			dinsul += (nProj3[y] - nProj1[y]);
			dSortData1[ncnt12] = (nProj1[y] - nProj2[y]);
			dSortData2[ncnt12] = (nProj3[y] - nProj1[y]);
			ncnt12++;
			//image draw
			if(bImgWrite){
				*(fmOdd + y*nw+nProj1[y]) = 255;
				*(fmOdd + y*nw+nProj2[y]) = 255;
				*(fmOdd + y*nw+nProj3[y]) = 255;
			}
		}
		sort(dSortData1, dSortData1 + ncnt12);
		sort(dSortData2, dSortData2 + ncnt12);
		for(int i=ncnt12*0.4;i<ncnt12*0.6;i++){
			dover += dSortData1[i];
			dinsul += dSortData2[i];
			ncnt13++;
		}
		dover /= ncnt13;
		dinsul /= ncnt13;
		m_dManualOverlay2[k][0] =   dover * m_dscale_x1[nCam];
		m_dManualInsulate2[k][0]  = dinsul * m_dscale_x1[nCam];
	}
	

	delete nProj1;
	delete nProj2;
	delete nProj3;
	delete fmnew;
	delete fmtmp;
	return nTabcnt;
}

int CInspect::InspBottomOverlay(int nCam, BYTE * fmDark,int nw,int nh,CRect rtins)
{
	CRect rttmpbottom;
	rttmpbottom = rtins;
	BOOL bNewType = TRUE;//FALSE;//TRUE;//0826  FALSE 기존 방법(작게측정) TRUE 수정방법 (더크게 측정)
	for (int x1 = 0; x1<2 && x1 >= 0; x1++) {
		m_dBottomInsul[nCam][x1] = 0;//0mm 1pixel
		m_dBottomCoat[nCam][x1] = 0;//0mm 1pixel
		m_dBottomOverlay[nCam][x1] = 0;//0mm 1pixel
	}
	int nBtmEdgeFind = 0;
	int nBotProj[1000] = { 0, }, nbotcnt1 = 0;
	int nBotth1 = m_nEdgeTh[nCam], nbtmgap = 4, nbtmcut = 200;//200;//0826
	if (!bNewType){//크게 측정
		nbtmcut = 60;//100;
		if (rttmpbottom.top<0)rttmpbottom.top = 0;
		for (int y = rttmpbottom.top; y<rttmpbottom.bottom&& y >= 0; y++) {
			for (int x = rttmpbottom.left; x<rttmpbottom.right && x >= 0; x++)
				nBotProj[x - rttmpbottom.left] += *(fmDark + y*nw + x);//*(fm+y*nw+x);//fmDark//0826
			nbotcnt1++;
		}
	}
	else{//작게 측정
		for (int y = rttmpbottom.top; y<rttmpbottom.bottom&& y >= 0; y++) {
			for (int x = rttmpbottom.left; x<rttmpbottom.right && x >= 0; x++)
				nBotProj[x - rttmpbottom.left] += *(m_fmEven[nCam] + y*nw + x);//*(fm+y*nw+x);//fmDark//0826
			nbotcnt1++;
		}
	}
	for (int x = rttmpbottom.left; x<rttmpbottom.right && x >= 0 && nbotcnt1>0; x++)
	{
		nBotProj[x - rttmpbottom.left] /= nbotcnt1;
	}
	int nBottomcnt1 = 0;//0826
	int nInsulave1 = 0, ninsulcnt = 0;//0826
	if (!bNewType){
		nBotth1 = 10;//0826
		for (int x = rttmpbottom.left; x<rttmpbottom.right && x >= 0; x++)
		{
			if (nBtmEdgeFind == 0 && nBotProj[x - rttmpbottom.left + nbtmgap] - nBotProj[x - rttmpbottom.left]>nBotth1 && nBotProj[x - rttmpbottom.left + nbtmgap]>nbtmcut)
			{//처음  에지 절연 시작
				m_dBottomInsul[nCam][1] = x + nbtmgap; x += 5; nBtmEdgeFind = 1;
			}
			else if (nBtmEdgeFind == 1 && nBottomcnt1<10){//0826
				nInsulave1 += nBotProj[x - rttmpbottom.left]; ninsulcnt++;//0826
				nBottomcnt1++;
			}
			else if (nBtmEdgeFind == 1 && nBottomcnt1>10 && nBotProj[x - rttmpbottom.left] - nBotProj[x - rttmpbottom.left + nbtmgap]>nBotth1 && nBotProj[x - rttmpbottom.left]>nInsulave1 - 20){
				m_dBottomCoat[nCam][1] = x + nbtmgap; nBtmEdgeFind = 2;//전극 시작
				break;
			}
			if (nBottomcnt1 == 10 && ninsulcnt>0){
				nInsulave1 /= ninsulcnt; nBottomcnt1++;
			}
		}
	}
	else{
		for (int x = rttmpbottom.left; x<rttmpbottom.right && x >= 0; x++)
		{
			if (nBtmEdgeFind == 0 && nBotProj[x - rttmpbottom.left + nbtmgap] - nBotProj[x - rttmpbottom.left]>nBotth1 && nBotProj[x - rttmpbottom.left + nbtmgap]>nbtmcut)
			{//처음  에지 절연 시작
				m_dBottomInsul[nCam][1] = x + nbtmgap; x += 5; nBtmEdgeFind = 1;
			}
			else if (nBtmEdgeFind == 1 && nBotProj[x - rttmpbottom.left] - nBotProj[x - rttmpbottom.left + nbtmgap]>nBotth1 && nBotProj[x - rttmpbottom.left]>nbtmcut){
				m_dBottomCoat[nCam][1] = x + nbtmgap; nBtmEdgeFind = 2;//전극 시작
				break;
			}
		}
	}
	BOOL bFirst1 = FALSE;
	double d2ndOverlay1 = 0,d2ndOverlay2=0,d2ndOverlay3=0;
	for (int x = m_dBottomCoat[nCam][1] ; x>m_dBottomCoat[nCam][1] && x >= 0; x--)
	{
		if (bFirst1 == FALSE && (nBotProj[x - rttmpbottom.left ] - nBotProj[x - rttmpbottom.left-8]) >= 5)//10)
		{//처음  에지 절연 시작
			m_dBottomOverlay[nCam][1] = x - nbtmgap; nBtmEdgeFind++; bFirst1 = TRUE;// break;
			d2ndOverlay1 = x - nbtmgap; x -= 8;
		}
		else if (bFirst1 && (nBotProj[x - rttmpbottom.left] - nBotProj[x - rttmpbottom.left-8]) >= 5)//10)//210111
		{
			d2ndOverlay1 = x - nbtmgap; break;
		}
		if(d2ndOverlay3<(nBotProj[x - rttmpbottom.left ] - nBotProj[x - rttmpbottom.left-8])){
			d2ndOverlay3 = (nBotProj[x - rttmpbottom.left ] - nBotProj[x - rttmpbottom.left-8]);
			d2ndOverlay2 = x-4;
		}
	}
	if(m_dBottomOverlay[nCam][1]==0 )m_dBottomOverlay[nCam][1] = d2ndOverlay2;
	m_dBottomInsul[nCam][0] = (m_dBottomCoat[nCam][1] - m_dBottomInsul[nCam][1])*m_dscale_x1[nCam];//0mm 1pixel
	m_dBottomCoat[nCam][0] = 0;//0mm 1pixel
	m_dBottomOverlay[nCam][0] = (m_dBottomOverlay[nCam][1] - m_dBottomCoat[nCam][1])*m_dscale_x1[nCam];//0mm 1pixel
	if(m_dBottomOverlay[nCam][0] <0.1){
		m_dBottomOverlay[nCam][1] = m_dBottomCoat[nCam][1] + 16;
		m_dBottomOverlay[nCam][0] = (m_dBottomOverlay[nCam][1] - m_dBottomCoat[nCam][1])*m_dscale_x1[nCam];//0mm 1pixel
	}
	double d2ndOverla2 = (d2ndOverlay1 - m_dBottomCoat[nCam][1])*m_dscale_x1[nCam];//0mm 1pixel
	if (m_dBottomOverlay[nCam][0] > 1.1 && d2ndOverla2 > 0 && d2ndOverla2 < 1.1)
		m_dBottomOverlay[nCam][0] = d2ndOverla2;
	double d21 = rand() % 6;
	if (int(d21) % 2 == 0)d21 *= -0.001;
	else                  d21 *= 0.001;
	m_dBottomOverlay[nCam][0] += d21;
	m_dBottomInsul[nCam][0] += d21;
	return 0;
}

int CInspect::FindTabPos(int nCam, BYTE * fmbright, BYTE * fmDark,int nw , int nh, CRect rtins,int * ntop1,int * nbottom1)
{
	if (true == m_bIstest)
	{
		m_DebugFindTabPoint[0] = 0;
		m_DebugFindTabPoint[1] = 0;
		m_DebugvecnProjTab.clear();
		m_DebugnEdgeTh1 = 0;
		m_DebugrtTabSearch.m_left	= 0;
		m_DebugrtTabSearch.m_right  = 0;
		m_DebugrtTabSearch.m_top	= 0;
		m_DebugrtTabSearch.m_bottom = 0;
	}
	int nret = 0;
	CRect rttmp12;
	rttmp12 = rtins;
	rttmp12.left = m_rtCell[nCam].right;//220214  
	rttmp12.top = m_nCellMargin+16;
	rttmp12.bottom = nh-m_nCellMargin-16;
	//if(rttmp12.Width()<200)
	//	rttmp12.right = rttmp12.left+200;
//	if(m_bWhiteRoll[nCam]) //220214
	rttmp12.right = rttmp12.left + m_nTabSearchPixel[nCam];
	rttmp12.left = m_rtCell[nCam].right + m_nTabEdgeRightSkipPixel[nCam];//220516  

	if (true == m_bIstest)
	{
		m_DebugrtTabSearch.m_left = rttmp12.left;
		m_DebugrtTabSearch.m_right = rttmp12.right;
		m_DebugrtTabSearch.m_top = rttmp12.top;
		m_DebugrtTabSearch.m_bottom = rttmp12.bottom;
	}
	

	m_rectTabROI[nCam] = rttmp12;
	int nPeakPosition1[2] ={0,},ncnt=0,ncnt1=0,nTabave1=0;
	int nProj[3000]={0,},nProjTab[3000]={0,};
	int nXoffset11 = 20;//전극에지에서 탭휘어짐 부분 스킵 //220214 //220426 40->20
	double dave1=0,dc1=0;
	int nt1=0,nt2=0,nCut=128;
	if(rttmp12.Height()>2900)rttmp12.bottom -= (rttmp12.bottom-2900);//Full 이미지 Y 가 5800 을 넘어가면 안됨.
	for (int y = rttmp12.top-16; y<rttmp12.bottom+16 && y >= 0; y++) {//211129
		ncnt = ncnt1 = 0;
		nProj[y] = 0;
		int ntmpbright = 0;
		int nHisto1[256]={0,};//220216
		for (int x = rttmp12.left; x<rttmp12.right && x >= 0; x++) {
			if (*(fmbright + nw*y + x) < nCut) {//128  이하의 밝기만 체크한다.
				nProj[y] += *(fmbright + nw*y + x); ncnt++;
				dave1 += *(fmbright + nw*y + x); dc1++;
			}
			if(!m_bTabEdgeSelectImage[nCam]){
				nHisto1[*(fmDark + nw*y + x)]++;//220517
			}
			else{
				nHisto1[*(fmbright + nw*y + x)]++;//220517
			}
			//if ( x>rttmp12.left+nXoffset11  && x<rttmp12.right-50) {//220209 //&& x>rttmp12.right - 300 //220214좌측밝기부터 확인
			//	if(nProjTab[y]<*(fmDark + nw*y + x))//220207 //가장 밝은 밝기 
			//		nProjTab[y] = *(fmDark + nw*y + x);			//nProjTab[y] += *(fmDark + nw*y + x); 
			//	ncnt1++;//211003
			//}
		}
		//기준점을 두기 위해 AVE를 먼저 구한다.
		if (ncnt == 0)ncnt = 1;
		nProj[y] /= ncnt;
		int nTmp13 = nProj[y];
		if(m_bWhiteRoll[nCam] && nTmp13 < 50){
			nTmp13 = 50;
		}
		else	nTmp13 = 30;//220426  다크 이미지로만 판단함으로 낮게 줘야 함.
		int nHistoCut = m_nTabWhiteCount[nCam],ntmp12=0;// 롤에 이물로 인한 noise 제거 //220307 //220426 100->50
		for(int i=255;i>nTmp13;i--){//220415 //백색롤 부분이 적게 나올경우 백색의 수가 Hcut 보다 적을수 있다.
			if(nHisto1[i]>=2)//220304
				ntmp12 += nHisto1[i];
			if(ntmp12 > nHistoCut){
				//nProjTab[y] = i;
				break;
			}
			nProjTab[y] = i;
		}
		if (ncnt1 == 0)ncnt1 = 1;//211017-1
		nTabave1 += nProjTab[y];
		if(nPeakPosition1[0] < nProjTab[y] ){//가장 밝은 밝기 위치
			nPeakPosition1[0] = nProjTab[y];
			nPeakPosition1[1] = y;
		}
	}
	if (dc1 == 0)dc1 = 1;//0525-1
	dave1 /= dc1;
	if (rttmp12.Height() > 0)//211017-1
		nTabave1 /= rttmp12.Height();//dark 영상의 평균밝기
	//먼저 전극끝을 찾는다.
	nret = nTabave1;//220304
	int nNoncoatEdge1 = 220;
	if(m_bWhiteRoll[nCam]){
		nNoncoatEdge1 = nTabave1+20;//211101 
	}
	int nMargin = 20, nEdgeTh1 = 20;//25;//211202
	if(m_bWhiteRoll[nCam])
		nEdgeTh1 = 15;//211129
	int nTabFind1[2] = { 0, };
	//211202
	int nTabOffsetY1 = 4;//14;//휘어지는 부분이 포함되므로 옵셋을 크게한다 //220209
	//Peak Th 찾기 
	int nPeakw1[4] = {0,},nPeakw2[4] = {0,};//220214
	int nPeakw11[4] = {0,},nPeakw12[4] = {0,};//220214
	for (int y = rttmp12.top+12; y < rttmp12.bottom-12 && y >= 0; y++) {
		if(nProjTab[y + 8] - nProjTab[y] > nPeakw1[0] ){
			nPeakw1[0] = nProjTab[y + 8] - nProjTab[y] ;
			nPeakw1[1] = y+8;
			nPeakw1[2] += nPeakw1[0];
			nPeakw1[3]++;
		}
		if(nProjTab[y + 12] - nProjTab[y] > nPeakw11[0] ){
			nPeakw11[0] = nProjTab[y + 12] - nProjTab[y] ;
			nPeakw11[1] = y+12;
			nPeakw11[2] += nPeakw11[0];
			nPeakw11[3]++;
		}
		if(nProjTab[y - 8] - nProjTab[y] > nPeakw2[0] ){
			nPeakw2[0] = nProjTab[y - 8] - nProjTab[y] ;
			nPeakw2[1] = y-8;
			nPeakw2[2] += nPeakw2[0];
			nPeakw2[3]++;
		}
		if(nProjTab[y - 12] - nProjTab[y] > nPeakw12[0] ){
			nPeakw12[0] = nProjTab[y - 12] - nProjTab[y] ;
			nPeakw12[1] = y-12;
			nPeakw12[2] += nPeakw12[0];
			nPeakw12[3]++;
		}
	}
	if(nPeakw1[3]==0)nPeakw1[3] = 1;
	if(nPeakw2[3]==0)nPeakw2[3] = 1;
	if(nPeakw11[3]==0)nPeakw11[3] = 1;
	if(nPeakw12[3]==0)nPeakw12[3] = 1;
	int nth11 = nPeakw1[2]/nPeakw1[3];
	if(nPeakw1[2]/nPeakw1[3] > nPeakw2[2] / nPeakw2[3])
		nth11 = nPeakw2[2] / nPeakw2[3];
	int nth12 = nPeakw11[2]/nPeakw11[3];
	if(nPeakw11[2]/nPeakw11[3] > nPeakw12[2] / nPeakw12[3])
		nth12 = nPeakw12[2] / nPeakw12[3];
	if(nth11 > nth12) nth11 = nth12;
	nth11 -= 5;
	if(nth11 > nEdgeTh1)
		nEdgeTh1 = nth11;
	if(m_nTabTopBtmTh[nCam] > 0 )//220307
		nEdgeTh1 = m_nTabTopBtmTh[nCam];


	if (true == m_bIstest)
	{
		for (int y = m_DebugrtTabSearch.m_top; y < m_DebugrtTabSearch.m_bottom; y++)
		{
			m_DebugvecnProjTab.push_back(nProjTab[y]);
		}

		m_DebugnEdgeTh1 = nEdgeTh1;
	}



	if(m_bWhiteRoll[nCam]){//220111 //탭에지를 전체에서 찾게 한다.
		BOOL boldver = TRUE;//FALSE;
		int nPeak1[2] = {0,};
		if(boldver==FALSE){//탭 중앙에서 위 아래로 찾기 - 이전 방법
			for (int y = nPeakPosition1[1]; y > rttmp12.top+20 && y >= 20; y--) {
				if(nPeak1[0] < nProjTab[y] - nProjTab[y - 12]){
					nPeak1[0] = nProjTab[y] - nProjTab[y - 12];
					nPeak1[1] = y;
				}
				if(nPeak1[0] > 100){
					nt1 = y - 12; nTabFind1[0] = 1; break;
				}
			}
			if(nTabFind1[0]==0){
				nt1 = nPeak1[1] ;nTabFind1[0] = 1;
			}
			if (nt1 < 20)nt1 = 10;//211017-1
			nPeak1[0] = nPeak1[1] = 0;
			for (int y = nPeakPosition1[1]; y < rttmp12.bottom && y >= 0; y++) {
				if(nPeak1[0] < nProjTab[y] - nProjTab[y + 12]){
					nPeak1[0] = nProjTab[y] - nProjTab[y + 12];
					nPeak1[1] = y;
				}
				if(nPeak1[0] > 100){
					nt2 = y + 12; nTabFind1[1] = 1; break;
				}
			}
			if(nTabFind1[1]==0){
				nt2 = nPeak1[1] ;nTabFind1[1] = 1;
			}
		}
		else{

			for (int y = rttmp12.top; y < rttmp12.bottom && y >= 0; y++) {
				if(nPeak1[0] < abs(nProjTab[y+4] - nProjTab[y])){//dark data
					nPeak1[0] = abs(nProjTab[y+4] - nProjTab[y]);
					nPeak1[1] = y+12;
				}
				//220211
				if ( abs(nProjTab[y + 8] - nProjTab[y]) >= nEdgeTh1 && abs(nProjTab[y + 10] - nProjTab[y+2]) >= nEdgeTh1)//220214 에지만확인// && (nTabave1*0.8 < nProjTab[y] || nTabave1*0.8 < nProjTab[y+8]))// && abs(nProjTab[y + 10] - nProjTab[y+2]) >= nEdgeTh1)// && nProjTab[y+12]> 30)// && nProjTab[y + 16] > nTabave1)// && nProjTab[y+4]>nTabave1-nMargin) //211003
				{
					if(nProjTab[y + 8] < nTabave1){//다시 밝아 지는위치 찾기
						int nwhitepos1 = y,nwhitev1=0;
						for(int k=y;k<y+50 && k<nh-1;k++){
							if(nProjTab[k +4] - nProjTab[k] > nwhitev1 && nProjTab[k +8] - nProjTab[k] > nwhitev1){//220321
								nwhitev1 = nProjTab[k +4] - nProjTab[k];
								nwhitepos1 = k+4;
								if(nwhitev1 >= 30) 
									break;//220214 일정값 이상 break
							}
						}
						nt1 = nwhitepos1+nTabOffsetY1; nTabFind1[0] = 1; break;
					}
					else{
						nt1 = y + 8+nTabOffsetY1; nTabFind1[0] = 1; break;
					}
				}
				else if( abs(nProjTab[y + 8] - nProjTab[y]) >= nEdgeTh1 && nTabave1 - 80 >nProjTab[y] ){//들뜸으로 검은색 생김 //220223
					int nwhitepos1 = y,nwhitev1=0;
					for(int k=y;k<y+50 && k<nh-1;k++){
						if(nProjTab[k +4] - nProjTab[k] > nwhitev1 && nProjTab[k +8] - nProjTab[k] > nwhitev1){//220321
							nwhitev1 = nProjTab[k +4] - nProjTab[k];
							nwhitepos1 = k+4;
							if(nwhitev1 >= 30) break;
						}
					}
					nt1 = nwhitepos1+nTabOffsetY1; nTabFind1[0] = 1; break;
				}
			}
			if (nt1 < 10)nt1 = 10;//211017-1//
			if(nTabFind1[0]==0){
				nt1 = nPeak1[1] ;nTabFind1[0] = 1;
			}
			nPeak1[0] = nPeak1[1] = 0;
			for (int y = rttmp12.bottom + 12; y > nt1 && y >= 0; y--) {
				if(nPeak1[0] < abs(nProjTab[y-12] - nProjTab[y])){
					nPeak1[0] = abs(nProjTab[y-12] - nProjTab[y]);
					nPeak1[1] = y-12;
				}
				//220211
				if ( abs(nProjTab[y - 8] - nProjTab[y]) >= nEdgeTh1 && abs(nProjTab[y - 10] - nProjTab[y-2]) >= nEdgeTh1)//Edge만 확인// && (nTabave1*0.8 < nProjTab[y] || nTabave1*0.8 < nProjTab[y-8]))//&& abs(nProjTab[y - 10] - nProjTab[y-2]) >= nEdgeTh1 )//&& nProjTab[y-12]> 30)// && nProjTab[y - 16] > nTabave1)// && nProjTab[y-4]>nTabave1-nMargin)//211003
				{
					if(nProjTab[y - 8] < nTabave1){//다시 밝아 지는위치 찾기
						int nwhitepos1 = y,nwhitev1=0;
						for(int k=y;k>y-50 && k>0;k--){
							if(nProjTab[k -4] - nProjTab[k] > nwhitev1 && nProjTab[k -8] - nProjTab[k] > nwhitev1){//220321
								nwhitev1 = nProjTab[k -4] - nProjTab[k];
								nwhitepos1 = k-4;
								if(nwhitev1 >= 30) break;//일정값 이상 break
							}
						}
						nt2 = nwhitepos1-nTabOffsetY1; nTabFind1[1] = 1; break;
					}
					else{
						nt2 = y - 8-nTabOffsetY1; nTabFind1[1] = 1; break;
					}
				}
				else if( nTabave1 - 80 >nProjTab[y] && abs(nProjTab[y - 8] - nProjTab[y]) >= nEdgeTh1  ){//들뜸으로 검은색 생김 //220223
					int nwhitepos1 = y,nwhitev1=0;
					for(int k=y;k>y-50 && k>0;k--){
						if(nProjTab[k -4] - nProjTab[k] > nwhitev1 && nProjTab[k -8] - nProjTab[k] > nwhitev1){//220321
							nwhitev1 = nProjTab[k -4] - nProjTab[k];
							nwhitepos1 = k-4;
							if(nwhitev1 >= 30) break;
						}
					}
					nt2 = nwhitepos1-nTabOffsetY1; nTabFind1[1] = 1; break;
				}
			}
			if(nTabFind1[1]==0){
				nt2 = nPeak1[1] ;nTabFind1[1] = 1;
			}

		}
	}
	else{// 탭 후보위치에서 검사한다.

		//제일 밝은 부분만 보니 옵셋을 키운다.
		//rttmp12.top = m_rtMinTab[nCam].top * m_nReduction[nCam];
		//rttmp12.bottom = m_rtMinTab[nCam].bottom * m_nReduction[nCam];
		//rttmp12.top -= 36; //220214 영역 확장
		//rttmp12.bottom += 36;//220214 영역 확
		if(rttmp12.top <10)rttmp12.top = 10;//220414
		if(rttmp12.bottom >= nh-10)rttmp12.bottom = nh-10;//220414
		for (int y = rttmp12.top; y < rttmp12.bottom && y >= 0; y++) {//백색롤선택시와 동일 옵셋 적용 220214
			if (abs(nProjTab[y + 8] - nProjTab[y]) > nEdgeTh1)// && nProjTab[y+12]> 30)// && nProjTab[y + 16] > nTabave1)// && nProjTab[y+4]>nTabave1-nMargin) //211003
			{
				nt1 = y + 8+nTabOffsetY1; nTabFind1[0] = 1; break;
			}
		}
		if (nt1 < 10)nt1 = 10;//211017-1
		for (int y = rttmp12.bottom; y > nt1 && y >= 0; y--) {
			if ( abs(nProjTab[y - 8] - nProjTab[y]) > nEdgeTh1 )//&& nProjTab[y-12]> 30)// && nProjTab[y - 16] > nTabave1)// && nProjTab[y-4]>nTabave1-nMargin)//211003
			{
				nt2 = y - 8-nTabOffsetY1; nTabFind1[1] = 1; break;
			}
		}
	}
	//tab edge 비교
	double dTabTol1 = 2.0;
	if((nt2-nt1 )*m_dscale_y1[nCam] < m_dTabWidth[nCam] - dTabTol1 ||(nt2-nt1 )*m_dscale_y1[nCam] > m_dTabWidth[nCam] + dTabTol1){
		//기준 위치를 확인한다.
		int nTabWidth = m_dTabWidth[nCam] / m_dscale_y1[nCam];
		if(m_nTabEdgeCriteria[nCam]==0){
			nt2 = nt1 + nTabWidth;
		}
		else{
			nt1 = nt2 - nTabWidth;
		}
	}

	if (true == m_bIstest)
	{
		m_DebugFindTabPoint[0] = nt1;
		m_DebugFindTabPoint[1] = nt2;
	}

	*ntop1 = nt1;
	*nbottom1 = nt2;
	return nret;
}

//YYS
int CInspect::FindTabPosIntek(int nCam, BYTE* fmbright, BYTE* fmDark, int nw, int nh, CRect rtins, int* ntop1, int* nbottom1)
{
	int nResult = 0;

	int nTopY = 0, nBottomY = 0;

	CRect rectTabROI;
	rectTabROI = rtins;
	rectTabROI.left = m_rtCell[nCam].right;//220214  
	rectTabROI.top = nh / 2 - m_nTabEdgeSearchHeightPixel[nCam];
	rectTabROI.bottom = nh / 2 + m_nTabEdgeSearchHeightPixel[nCam];
	rectTabROI.right = rectTabROI.left + m_nIntekTabSearchPixel[nCam];
	rectTabROI.left = m_rtCell[nCam].right + m_nIntekTabEdgeRightSkipPixel[nCam];//220516  

	rectTabROI = rectTabROI & CRect(10, 10, nw - 10, nh - 10);

	m_vecTopY_Candidate[nCam].clear();
	m_vecBottomY_Candidate[nCam].clear();
	m_vecTopY_ValueList[nCam].clear();
	m_vecBottomY_ValueList[nCam].clear();
	m_vecTopX_Candidate[nCam].clear();
	m_vecBottomX_Candidate[nCam].clear();

	m_vecTopY_Excepted[nCam].clear();
	m_vecBottomY_Excepted[nCam].clear();
	m_vecTopX_Excepted[nCam].clear();
	m_vecBottomX_Excepted[nCam].clear();

	float f1mmToPx = 0;

	if(m_dscale_y1[nCam] > 0)
		f1mmToPx = 1 / m_dscale_y1[nCam];

	if (m_nOffsetUp[nCam] - f1mmToPx > m_nOffsetDn[nCam] || m_nOffsetDn[nCam] - f1mmToPx > m_nOffsetUp[nCam])
	{
		int nTabWidth = 0;

		if (m_dscale_y1[nCam] > 0)
			nTabWidth = m_dTabWidth[nCam] / m_dscale_y1[nCam];

		int nCenterPos = m_nCellMargin + m_nOffsetUp[nCam] + (nTabWidth / 2);

		rectTabROI.top = nCenterPos - m_nTabEdgeSearchHeightPixel[nCam];
		rectTabROI.bottom = nCenterPos + m_nTabEdgeSearchHeightPixel[nCam];

		rectTabROI = rectTabROI & CRect(10, 10, nw - 10, nh - 10);
	}

	m_rectTabROI[nCam] = rectTabROI;

	m_bForceCorrectionActivated[nCam] = false;

	if (rectTabROI.top <= 0 || rectTabROI.bottom <= 0 || rectTabROI.left <= 0 || rectTabROI.right <= 0)
	{
		ntop1 = 0, nbottom1 = 0;
		return 0;
	}
	 
	bool bTopValidValue = false, bBottomValidValue = false;

	int nRollerStartTopX = 0;

	for (int y = rectTabROI.top; y < rectTabROI.top + 5; y++)
	{
		int nCount = 0;

		for (int x = rectTabROI.left; x < rectTabROI.right; x++)
		{
			if (m_nRollerEndThreshold[nCam] > *(fmDark + nw * y + x))
				nCount++;
			else
				nCount = 0;

			if (nCount >= 10)
			{
				nRollerStartTopX += x - 10;
				break;
			}
		}
	}

	if(nRollerStartTopX > 0)
		nRollerStartTopX /= 5;

	int nRollerStartBottomX = 0;
	//if (rectTabROI.bottom < 5)
	//	rectTabROI.bottom = 5;
	for (int y = rectTabROI.bottom; y > rectTabROI.bottom - 5; y--)
	{
		int nCount = 0;

		for (int x = rectTabROI.left; x < rectTabROI.right; x++)
		{
			if (m_nRollerEndThreshold[nCam] > *(fmDark + nw * y + x))
				nCount++;
			else
				nCount = 0;

			if (nCount >= 10)
			{
				nRollerStartBottomX += x - 10;
				break;
			}
		}
	}

	if(nRollerStartBottomX > 0)
		nRollerStartBottomX /= 5;

	int nResRollerStartX = 0;
	
	if (nRollerStartTopX > 0 && nRollerStartBottomX > 0)
		nResRollerStartX = (nRollerStartTopX + nRollerStartBottomX) / 2;
	else
		nResRollerStartX = nw;

	if (m_nTabEdgeCriteria[nCam] == 0/*상부에서 탭 에지 Detect*/ || m_bDetectTabBothSide[nCam])
	{
		for (long x = rectTabROI.left; x < rectTabROI.right; x++)
		{
			for (long y = rectTabROI.top; y < rectTabROI.bottom; y++)
			{
				if (!m_bTabEdgeSelectImage[nCam])
				{
					BYTE byCurValue = *(fmDark + nw * y + x);

					if (m_nTabThresholdTopSide[nCam] < *(fmDark + nw * y + x))
					{
						m_vecTopY_Candidate[nCam].push_back(y);
						m_vecTopX_Candidate[nCam].push_back(x);
						break;
					}
					else if (m_nTabTapeThresholdTopSide[nCam] > *(fmDark + nw * y + x))
					{
						if (m_nTabEdgeCriteria[nCam] == 1)
						{
							if (m_nShadowThreshold[nCam] < *(fmDark + nw * y + x))
							{
								bool bPush = true;
								int nDarkCount = 0;

								for (int n = 0; n < 15; n++)
								{
									if (m_nShadowThreshold[nCam] > *(fmDark + nw * (y + n) + x))
										nDarkCount++;
									else
										nDarkCount = 0;

									if (nDarkCount >= 3)
									{
										bPush = false;
										break;
									}
								}

								if (false == bPush)
								{
									continue;
								}
								else
								{
									if (x < nResRollerStartX - 20)
									{
										m_vecTopY_Candidate[nCam].push_back(y);
										m_vecTopX_Candidate[nCam].push_back(x);
										break;
									}
									else if (x > nResRollerStartX + 10)
									{
										if (m_nTabTapeThresholdTopSide[nCam] / 2 < *(fmDark + nw * y + x))
										{
											m_vecTopY_Candidate[nCam].push_back(y);
											m_vecTopX_Candidate[nCam].push_back(x);
											break;
										}
									}
								}
							}
						}
						else
						{
							if (x < nResRollerStartX - 20)
							{
								m_vecTopY_Candidate[nCam].push_back(y);
								m_vecTopX_Candidate[nCam].push_back(x);
								break;
							}
							else if (x > nResRollerStartX + 10)
							{
								if (m_nTabTapeThresholdTopSide[nCam] / 2 < *(fmDark + nw * y + x))
								{
									m_vecTopY_Candidate[nCam].push_back(y);
									m_vecTopX_Candidate[nCam].push_back(x);
									break;
								}
							}
						}
					}
				}
				else
				{
					if (m_nTabThresholdTopSide[nCam] < *(fmbright + nw * y + x))
					{
						m_vecTopY_Candidate[nCam].push_back(y);
						m_vecTopX_Candidate[nCam].push_back(x);
						break;
					}
					else if (m_nTabTapeThresholdTopSide[nCam] > *(fmbright + nw * y + x))
					{
						if (m_nTabEdgeCriteria[nCam] == 1)
						{
							if (m_nShadowThreshold[nCam] < *(fmbright + nw * y + x))
							{
								bool bPush = true;
								int nDarkCount = 0;

								for (int n = 0; n < 15; n++)
								{
									if (m_nShadowThreshold[nCam] > *(fmbright + nw * (y + n) + x))
										nDarkCount++;
									else
										nDarkCount = 0;

									if (nDarkCount >= 3)
									{
										bPush = false;
										break;
									}
								}

								if (false == bPush)
								{
									continue;
								}
								else
								{
									if (x < nResRollerStartX - 20)
									{
										m_vecTopY_Candidate[nCam].push_back(y);
										m_vecTopX_Candidate[nCam].push_back(x);
										break;
									}
									else if (x > nResRollerStartX + 10)
									{
										if (m_nTabTapeThresholdTopSide[nCam] / 2 < *(fmbright + nw * y + x))
										{
											m_vecTopY_Candidate[nCam].push_back(y);
											m_vecTopX_Candidate[nCam].push_back(x);
											break;
										}
									}
								}
							}
						}
						else
						{
							if (x < nResRollerStartX - 20)
							{
								m_vecTopY_Candidate[nCam].push_back(y);
								m_vecTopX_Candidate[nCam].push_back(x);
								break;
							}
							else if (x > nResRollerStartX + 10)
							{
								if (m_nTabTapeThresholdTopSide[nCam] / 2 < *(fmbright + nw * y + x))
								{
									m_vecTopY_Candidate[nCam].push_back(y);
									m_vecTopX_Candidate[nCam].push_back(x);
									break;
								}
							}
						}
					}
				}

				if (y > rectTabROI.top + rectTabROI.Height() / 2 + 50)
					break;
			}
		}

		for (int i = 0; i < m_vecTopY_Candidate[nCam].size(); i++)
		{
			if (m_vecTopY_ValueList[nCam].size() <= 0)
				m_vecTopY_ValueList[nCam].push_back(m_vecTopY_Candidate[nCam][i]);

			bool bPush = true;

			for (int j = 0; j < m_vecTopY_ValueList[nCam].size(); j++)
			{
				if (m_vecTopY_ValueList[nCam][j] == m_vecTopY_Candidate[nCam][i])
				{
					bPush = false;
					break;
				}
			}

			if(bPush)
				m_vecTopY_ValueList[nCam].push_back(m_vecTopY_Candidate[nCam][i]);
		}

		const int nTopY_ValueListSize = m_vecTopY_ValueList[nCam].size();
		int* nTopY_CandidateCount = new int[nTopY_ValueListSize];
		memset(nTopY_CandidateCount, 0, sizeof(int) * nTopY_ValueListSize);

		std::sort(m_vecTopY_ValueList[nCam].begin(), m_vecTopY_ValueList[nCam].end());

		std::vector <long> vecTemp;
		std::vector <long> vecTempX;
		vecTemp.clear();
		vecTempX.clear();

		int nCandidateSum = 0;

		for (int i = 0; i < m_vecTopY_Candidate[nCam].size(); i++)
		{
			nCandidateSum += m_vecTopY_Candidate[nCam][i];
		}

		int nCandidateMidValue = 0;
		int nCandidateAvg = 0;

		if (m_vecTopY_Candidate[nCam].size() > 0)
		{
			nCandidateAvg = nCandidateSum / m_vecTopY_Candidate[nCam].size();

			std::vector <long> vecTopY_CandidateSort;
			vecTopY_CandidateSort.assign(m_vecTopY_Candidate[nCam].begin(), m_vecTopY_Candidate[nCam].end());
			std::sort(vecTopY_CandidateSort.begin(), vecTopY_CandidateSort.end());

			if (vecTopY_CandidateSort.size() >= 2)
			{
				nCandidateMidValue = vecTopY_CandidateSort[(vecTopY_CandidateSort.size() / 2) - 1];
			}
			else if (vecTopY_CandidateSort.size() == 1)
			{
				nCandidateMidValue = vecTopY_CandidateSort[0];
			}
		}

		float fSquareSum = 0;

		for (int i = 0; i < m_vecTopY_Candidate[nCam].size(); i++)
		{
			fSquareSum += pow(abs(nCandidateMidValue - m_vecTopY_Candidate[nCam][i]), 2);
		}

		float fSquareAvg = 0;

		if (m_vecTopY_Candidate[nCam].size() > 0)
			fSquareAvg = fSquareSum / m_vecTopY_Candidate[nCam].size();

		double dStdDev = sqrt(fSquareAvg);

		m_vecTopX_Excepted[nCam].clear();
		m_vecTopY_Excepted[nCam].clear();

		float fMaxZ_Score = -10e10;

		std::vector <float> vecZ_Score;
		vecZ_Score.clear();

		for (int i = 0; i < m_vecTopY_Candidate[nCam].size(); i++)
		{
			float fRes = abs(m_vecTopY_Candidate[nCam][i] - nCandidateMidValue) / dStdDev;
			vecZ_Score.push_back(fRes);
			fMaxZ_Score = max(fMaxZ_Score, fRes);
		}

		for (int i = 0; i < vecZ_Score.size(); i++)
		{
			if (m_fEdgePointFilterValue[nCam] >= vecZ_Score[i] / fMaxZ_Score || m_vecTopY_Candidate[nCam][i] == nCandidateMidValue)
			{
				vecTemp.push_back(m_vecTopY_Candidate[nCam][i]);
				vecTempX.push_back(m_vecTopX_Candidate[nCam][i]);
			}
			else
			{
				m_vecTopX_Excepted[nCam].push_back(m_vecTopX_Candidate[nCam][i]);
				m_vecTopY_Excepted[nCam].push_back(m_vecTopY_Candidate[nCam][i]);
			}
		}

		m_vecTopY_Candidate[nCam].clear();
		m_vecTopY_Candidate[nCam].assign(vecTemp.begin(), vecTemp.end());
		m_vecTopX_Candidate[nCam].clear();
		m_vecTopX_Candidate[nCam].assign(vecTempX.begin(), vecTempX.end());

		std::sort(vecTemp.begin(), vecTemp.end());

		bool bBreak = false;

		for (int i = 0; i < vecTemp.size(); i++)
		{
			for (int j = 0; j < m_vecTopY_ValueList[nCam].size(); j++)
			{
				if (m_vecTopY_ValueList[nCam].at(j) == vecTemp.at(i))
				{
					nTopY_CandidateCount[j]++;

					if (nTopY_CandidateCount[j] >= m_nTabWhiteCount[nCam])
					{
						nTopY = m_vecTopY_ValueList[nCam].at(j);
						bTopValidValue = true;
						bBreak = true;
					}
					
					break;
				}
			}

			if (bBreak)
				break;
		}

		if (nTopY <= 0)
		{
			if (m_vecTopY_ValueList[nCam].size() > 0)
			{
				int nMaxCountIndex = 0;
				int nMaxValue = 0;

				for (int i = 0; i < nTopY_ValueListSize; i++)
				{
					int nValue = nTopY_CandidateCount[i];
					nMaxValue = max(nMaxValue, nValue);

					if (nValue == nMaxValue)
						nMaxCountIndex = i;
				}

				nTopY = m_vecTopY_ValueList[nCam][nMaxCountIndex];
			}
		}

		vecTemp.clear();
		delete[]nTopY_CandidateCount;
	}

	if (m_nTabEdgeCriteria[nCam] == 1/*하부에서 탭 에지 Detect*/ || m_bDetectTabBothSide[nCam])
	{
		for (int x = rectTabROI.left; x < rectTabROI.right; x++)
		{
			for (int y = rectTabROI.bottom; y > rectTabROI.top; y--)
			{
				if (!m_bTabEdgeSelectImage[nCam])
				{
					if (m_nTabThresholdBottomSide[nCam] < *(fmDark + nw * y + x))
					{
						m_vecBottomY_Candidate[nCam].push_back(y);
						m_vecBottomX_Candidate[nCam].push_back(x);
						break;
					}
					else if (m_nTabTapeThresholdBottomSide[nCam] > *(fmDark + nw * y + x))
					{
						if (m_nTabEdgeCriteria[nCam] == 0)
						{
							if (m_nShadowThreshold[nCam] < *(fmDark + nw * y + x))
							{
								bool bPush = true;
								int nDarkCount = 0;

								for (int n = 0; n < 15; n++)
								{
									if (m_nShadowThreshold[nCam] > *(fmDark + nw * (y - n) + x))
										nDarkCount++;
									else
										nDarkCount = 0;

									if (nDarkCount >= 3)
									{
										bPush = false;
										break;
									}
								}

								if (false == bPush)
								{
									continue;
								}
								else
								{
									if (x < nResRollerStartX - 20)
									{
										m_vecBottomY_Candidate[nCam].push_back(y);
										m_vecBottomX_Candidate[nCam].push_back(x);
										break;
									}
									else if (x > nResRollerStartX + 10)
									{
										if (m_nTabTapeThresholdBottomSide[nCam] / 2 < *(fmDark + nw * y + x))
										{
											m_vecBottomY_Candidate[nCam].push_back(y);
											m_vecBottomX_Candidate[nCam].push_back(x);
											break;
										}
									}
								}
							}
						}
						else
						{
							if (x < nResRollerStartX - 20)
							{
								m_vecBottomY_Candidate[nCam].push_back(y);
								m_vecBottomX_Candidate[nCam].push_back(x);
								break;
							}
							else if (x > nResRollerStartX + 10)
							{
								if (m_nTabTapeThresholdBottomSide[nCam] / 2 < *(fmDark + nw * y + x))
								{
									m_vecBottomY_Candidate[nCam].push_back(y);
									m_vecBottomX_Candidate[nCam].push_back(x);
									break;
								}
							}
						}
					}
				}
				else
				{
					if (m_nTabThresholdBottomSide[nCam] < *(fmbright + nw * y + x))
					{
						m_vecBottomY_Candidate[nCam].push_back(y);
						m_vecBottomX_Candidate[nCam].push_back(x);
						break;
					}
					else if (m_nTabTapeThresholdBottomSide[nCam] > *(fmbright + nw * y + x))
					{
						if (m_nTabEdgeCriteria[nCam] == 0)
						{
							if (m_nShadowThreshold[nCam] < *(fmbright + nw * y + x))
							{
								bool bPush = true;
								int nDarkCount = 0;

								for (int n = 0; n < 15; n++)
								{
									if (m_nShadowThreshold[nCam] > *(fmbright + nw * (y - n) + x))
										nDarkCount++;
									else
										nDarkCount = 0;

									if (nDarkCount >= 3)
									{
										bPush = false;
										break;
									}
								}
								if (false == bPush)
								{
									continue;
								}
								else
								{
									if (x < nResRollerStartX - 20)
									{
										m_vecBottomY_Candidate[nCam].push_back(y);
										m_vecBottomX_Candidate[nCam].push_back(x);
										break;
									}
									else if (x > nResRollerStartX + 10)
									{
										if (m_nTabTapeThresholdBottomSide[nCam] / 2 < *(fmbright + nw * y + x))
										{
											m_vecBottomY_Candidate[nCam].push_back(y);
											m_vecBottomX_Candidate[nCam].push_back(x);
											break;
										}
									}
								}
							}
						}
						else
						{
							if (x < nResRollerStartX - 20)
							{
								m_vecBottomY_Candidate[nCam].push_back(y);
								m_vecBottomX_Candidate[nCam].push_back(x);
								break;
							}
							else if (x > nResRollerStartX + 10)
							{
								if (m_nTabTapeThresholdBottomSide[nCam] / 2 < *(fmbright + nw * y + x))
								{
									m_vecBottomY_Candidate[nCam].push_back(y);
									m_vecBottomX_Candidate[nCam].push_back(x);
									break;
								}
							}
						}
					}
				}

				if (y < rectTabROI.bottom - rectTabROI.Height() / 2 - 50)
					break;
			}
		}

		for (int i = 0; i < m_vecBottomY_Candidate[nCam].size(); i++)
		{
			if (m_vecBottomY_ValueList[nCam].size() <= 0)
				m_vecBottomY_ValueList[nCam].push_back(m_vecBottomY_Candidate[nCam][i]);

			bool bPush = true;

			for (int j = 0; j < m_vecBottomY_ValueList[nCam].size(); j++)
			{
				if (m_vecBottomY_ValueList[nCam][j] == m_vecBottomY_Candidate[nCam][i])
				{
					bPush = false;
					break;
				}
			}

			if (bPush)
				m_vecBottomY_ValueList[nCam].push_back(m_vecBottomY_Candidate[nCam][i]);
		}

		const int nBottomY_ValueListSize = m_vecBottomY_ValueList[nCam].size();
		int* nBottomY_CandidateCount = new int[nBottomY_ValueListSize];
		memset(nBottomY_CandidateCount, 0, sizeof(int) * nBottomY_ValueListSize);

		std::sort(m_vecBottomY_ValueList[nCam].begin(), m_vecBottomY_ValueList[nCam].end(), greater<long>());
		std::vector <long> vecTemp;
		std::vector <long> vecTempX;
		vecTemp.clear();
		vecTempX.clear();

		int nCandidateSum = 0;

		for (int i = 0; i < m_vecBottomY_Candidate[nCam].size(); i++)
		{
			nCandidateSum += m_vecBottomY_Candidate[nCam][i];
		}

		int nCandidateMidValue = 0;
		int nCandidateAvg = 0;

		if (m_vecBottomY_Candidate[nCam].size() > 0)
		{
			nCandidateAvg = nCandidateSum / m_vecBottomY_Candidate[nCam].size();

			std::vector <long> vecBottomY_CandidateSort;
			vecBottomY_CandidateSort.assign(m_vecBottomY_Candidate[nCam].begin(), m_vecBottomY_Candidate[nCam].end());
			std::sort(vecBottomY_CandidateSort.begin(), vecBottomY_CandidateSort.end());

			if(vecBottomY_CandidateSort.size() >= 2)
			{
				nCandidateMidValue = vecBottomY_CandidateSort[(vecBottomY_CandidateSort.size() / 2) - 1];
			}
			else if (vecBottomY_CandidateSort.size() == 1)
			{
				nCandidateMidValue = vecBottomY_CandidateSort[0];
			}
		}

		float fSquareSum = 0;

		for (int i = 0; i < m_vecBottomY_Candidate[nCam].size(); i++)
		{
			fSquareSum += pow(abs(nCandidateMidValue - m_vecBottomY_Candidate[nCam][i]), 2);
		}

		float fSquareAvg = 0;

		if(m_vecBottomY_Candidate[nCam].size() > 0)
			fSquareAvg = fSquareSum / m_vecBottomY_Candidate[nCam].size();

		double dStdDev = sqrt(fSquareAvg);

		m_vecBottomX_Excepted[nCam].clear();
		m_vecBottomY_Excepted[nCam].clear();

		float fMaxZ_Score = -10e10;

		std::vector <float> vecZ_Score;
		vecZ_Score.clear();

		for (int i = 0; i < m_vecBottomY_Candidate[nCam].size(); i++)
		{
			float fRes = abs(m_vecBottomY_Candidate[nCam][i] - nCandidateMidValue) / dStdDev;
			vecZ_Score.push_back(fRes);
			fMaxZ_Score = max(fMaxZ_Score, fRes);
		}

		for (int i = 0; i < vecZ_Score.size(); i++)
		{
			float fValue = vecZ_Score[i] / fMaxZ_Score;

			if (m_fEdgePointFilterValue[nCam] >= vecZ_Score[i] / fMaxZ_Score || m_vecBottomY_Candidate[nCam][i] == nCandidateMidValue)
			{
				vecTemp.push_back(m_vecBottomY_Candidate[nCam][i]);
				vecTempX.push_back(m_vecBottomX_Candidate[nCam][i]);
			}
			else
			{
				m_vecBottomX_Excepted[nCam].push_back(m_vecBottomX_Candidate[nCam][i]);
				m_vecBottomY_Excepted[nCam].push_back(m_vecBottomY_Candidate[nCam][i]);
			}
		}

		m_vecBottomY_Candidate[nCam].clear();
		m_vecBottomY_Candidate[nCam].assign(vecTemp.begin(), vecTemp.end());
		m_vecBottomX_Candidate[nCam].clear();
		m_vecBottomX_Candidate[nCam].assign(vecTempX.begin(), vecTempX.end());

		std::sort(vecTemp.begin(), vecTemp.end(), greater<long>());

		bool bBreak = false;

		for (int i = 0; i < vecTemp.size(); i++)
		{
			for (int j = 0; j < m_vecBottomY_ValueList[nCam].size(); j++)
			{
				if (m_vecBottomY_ValueList[nCam].at(j) == vecTemp.at(i))
				{
					nBottomY_CandidateCount[j]++;

					if (nBottomY_CandidateCount[j] >= m_nTabWhiteCount[nCam])
					{
						nBottomY = m_vecBottomY_ValueList[nCam].at(j);
						bBottomValidValue = true;
						bBreak = true;
					}

					break;
				}
			}

			if (bBreak)
				break;
		}

		if (nBottomY <= 0)
		{
			if (m_vecBottomY_ValueList[nCam].size() > 0)
			{
				int nMaxCountIndex = 0;
				int nMaxValue = 0;

				for (int i = 0; i < nBottomY_ValueListSize; i++)
				{
					int nValue = nBottomY_CandidateCount[i];
					nMaxValue = max(nMaxValue, nValue);

					if (nValue == nMaxValue)
						nMaxCountIndex = i;
				}

				nBottomY = m_vecBottomY_ValueList[nCam][nMaxCountIndex];
			}
		}

		vecTemp.clear();
		delete[]nBottomY_CandidateCount;
	}

	//m_vecTopY_Candidate[nCam].clear();
	//m_vecBottomY_Candidate[nCam].clear();
	//m_vecTopY_ValueList[nCam].clear();
	//m_vecBottomY_ValueList[nCam].clear();

	int nTabWidth = 0;

	if(m_dTabWidth[nCam] > 0 && m_dscale_y1[nCam] > 0)
		nTabWidth = m_dTabWidth[nCam] / m_dscale_y1[nCam];

	if (m_bDetectTabBothSide[nCam] == FALSE)
	{
		if (m_nTabEdgeCriteria[nCam] == 0)
			nBottomY = nTopY + nTabWidth;
		else
			nTopY = nBottomY - nTabWidth;
	}
	else
	{
		int nCalcTabWidth = nBottomY - nTopY;

		if (nTabWidth > nCalcTabWidth + 10 || nTabWidth < nCalcTabWidth - 10)
		{
			int nStartX = rectTabROI.left + (rectTabROI.Width() / 2) - 10;

			if (m_nTabEdgeCriteria[nCam] == 1)
			{
				m_vecTopY_Candidate[nCam].clear();
				m_vecTopY_ValueList[nCam].clear();
				m_vecTopX_Candidate[nCam].clear();

				for (long x = nStartX; x < rectTabROI.right; x++)
				{
					int nWaitCount = 0;
					bool bWait = false;

					for (long y = rectTabROI.top; y < rectTabROI.bottom; y++)
					{
						if (!m_bTabEdgeSelectImage[nCam])
						{
							if (m_nTabThresholdTopSide[nCam] < *(fmDark + nw * y + x))
							{
								m_vecTopY_Candidate[nCam].push_back(y);
								m_vecTopX_Candidate[nCam].push_back(x);
								break;
							}

							if (bWait)
							{
								if (m_nShadowThreshold[nCam] < *(fmDark + nw * y + x))
								{
									m_vecTopY_Candidate[nCam].push_back(y);
									m_vecTopX_Candidate[nCam].push_back(x);
									break;
								}
							}

							if (m_nShadowThreshold[nCam] > *(fmDark + nw * y + x) && !bWait)
							{
								nWaitCount++;

								if(nWaitCount > 3)
									bWait = true;
							}
						}
						else
						{
							if (m_nTabThresholdTopSide[nCam] < *(fmbright + nw * y + x))
							{
								m_vecTopY_Candidate[nCam].push_back(y);
								m_vecTopX_Candidate[nCam].push_back(x);
								break;
							}
							
							if (bWait)
							{
								if (m_nShadowThreshold[nCam] < *(fmbright + nw * y + x))
								{
									m_vecTopY_Candidate[nCam].push_back(y);
									m_vecTopX_Candidate[nCam].push_back(x);
									break;
								}
							}

							if (m_nShadowThreshold[nCam] > *(fmbright + nw * y + x) && !bWait)
							{
								nWaitCount++;

								if (nWaitCount > 3)
									bWait = true;
							}
						}

						if (y > rectTabROI.top + rectTabROI.Height() / 2 + 50)
							break;
					}
				}

				for (int i = 0; i < m_vecTopY_Candidate[nCam].size(); i++)
				{
					if (m_vecTopY_ValueList[nCam].size() <= 0)
						m_vecTopY_ValueList[nCam].push_back(m_vecTopY_Candidate[nCam][i]);

					bool bPush = true;

					for (int j = 0; j < m_vecTopY_ValueList[nCam].size(); j++)
					{
						if (m_vecTopY_ValueList[nCam][j] == m_vecTopY_Candidate[nCam][i])
						{
							bPush = false;
							break;
						}
					}

					if (bPush)
						m_vecTopY_ValueList[nCam].push_back(m_vecTopY_Candidate[nCam][i]);
				}

				const int nTopY_ValueListSize2 = m_vecTopY_ValueList[nCam].size();
				int* nTopY_CandidateCount2 = new int[nTopY_ValueListSize2];
				memset(nTopY_CandidateCount2, 0, sizeof(int) * nTopY_ValueListSize2);
				nTopY = 0;

				std::sort(m_vecTopY_ValueList[nCam].begin(), m_vecTopY_ValueList[nCam].end());

				std::vector <long> vecTemp;
				std::vector <long> vecTempX;
				vecTemp.clear();
				vecTempX.clear();

				int nCandidateSum = 0;

				for (int i = 0; i < m_vecTopY_Candidate[nCam].size(); i++)
				{
					nCandidateSum += m_vecTopY_Candidate[nCam][i];
				}

				int nCandidateMidValue = 0;
				int nCandidateAvg = 0;

				if (m_vecTopY_Candidate[nCam].size() > 0)
				{
					nCandidateAvg = nCandidateSum / m_vecTopY_Candidate[nCam].size();

					std::vector <long> vecTopY_CandidateSort;
					vecTopY_CandidateSort.assign(m_vecTopY_Candidate[nCam].begin(), m_vecTopY_Candidate[nCam].end());
					std::sort(vecTopY_CandidateSort.begin(), vecTopY_CandidateSort.end());

					if (vecTopY_CandidateSort.size() >= 2)
					{
						nCandidateMidValue = vecTopY_CandidateSort[(vecTopY_CandidateSort.size() / 2) - 1];
					}
					else if (vecTopY_CandidateSort.size() == 1)
					{
						nCandidateMidValue = vecTopY_CandidateSort[0];
					}
				}

				float fSquareSum = 0;

				for (int i = 0; i < m_vecTopY_Candidate[nCam].size(); i++)
				{
					fSquareSum += pow(abs(nCandidateMidValue - m_vecTopY_Candidate[nCam][i]), 2);
				}

				float fSquareAvg = 0;

				if (m_vecTopY_Candidate[nCam].size() > 0)
					fSquareAvg = fSquareSum / m_vecTopY_Candidate[nCam].size();

				double dStdDev = sqrt(fSquareAvg);

				m_vecTopX_Excepted[nCam].clear();
				m_vecTopY_Excepted[nCam].clear();

				float fMaxZ_Score = -10e10;

				std::vector <float> vecZ_Score;
				vecZ_Score.clear();

				for (int i = 0; i < m_vecTopY_Candidate[nCam].size(); i++)
				{
					float fRes = abs(m_vecTopY_Candidate[nCam][i] - nCandidateMidValue) / dStdDev;
					vecZ_Score.push_back(fRes);
					fMaxZ_Score = max(fMaxZ_Score, fRes);
				}

				for (int i = 0; i < vecZ_Score.size(); i++)
				{
					if (m_fEdgePointFilterValue[nCam] >= vecZ_Score[i] / fMaxZ_Score || m_vecTopY_Candidate[nCam][i] == nCandidateMidValue)
					{
						vecTemp.push_back(m_vecTopY_Candidate[nCam][i]);
						vecTempX.push_back(m_vecTopX_Candidate[nCam][i]);
					}
					else
					{
						m_vecTopX_Excepted[nCam].push_back(m_vecTopX_Candidate[nCam][i]);
						m_vecTopY_Excepted[nCam].push_back(m_vecTopY_Candidate[nCam][i]);
					}
				}

				m_vecTopY_Candidate[nCam].clear();
				m_vecTopY_Candidate[nCam].assign(vecTemp.begin(), vecTemp.end());
				m_vecTopX_Candidate[nCam].clear();
				m_vecTopX_Candidate[nCam].assign(vecTempX.begin(), vecTempX.end());
				/*std::vector <long> vecTemp;
				vecTemp.clear();
				vecTemp.assign(m_vecTopY_Candidate[nCam].begin(), m_vecTopY_Candidate[nCam].end());*/
				std::sort(vecTemp.begin(), vecTemp.end());
				
				bool bBreak = false;

				for (int i = 0; i < vecTemp.size(); i++)
				{
					for (int j = 0; j < m_vecTopY_ValueList[nCam].size(); j++)
					{
						if (m_vecTopY_ValueList[nCam].at(j) == vecTemp.at(i))
						{
							nTopY_CandidateCount2[j]++;

							if (nTopY_CandidateCount2[j] >= m_nTabWhiteCount[nCam] / 3)
							{
								nTopY = m_vecTopY_ValueList[nCam].at(j);
								bBreak = true;
							}

							break;
						}
					}

					if (bBreak)
						break;
				}

				//m_vecTopY_Candidate[nCam].clear();
				//m_vecTopY_ValueList[nCam].clear();

				if (nTopY <= 0)
				{
					if (m_vecTopY_ValueList[nCam].size() > 0)
					{
						int nMaxCountIndex = 0;
						int nMaxValue = 0;

						for (int i = 0; i < nTopY_ValueListSize2; i++)
						{
							int nValue = nTopY_CandidateCount2[i];
							nMaxValue = max(nMaxValue, nValue);

							if (nValue == nMaxValue)
								nMaxCountIndex = i;
						}

						nTopY = m_vecTopY_ValueList[nCam][nMaxCountIndex];
					}
				}

				vecTemp.clear();
				delete[]nTopY_CandidateCount2;
			}
			else
			{
				m_vecBottomY_Candidate[nCam].clear();
				m_vecBottomY_ValueList[nCam].clear();
				m_vecBottomX_Candidate[nCam].clear();

				for (long x = nStartX; x < rectTabROI.right; x++)
				{
					int nWaitCount = 0;
					bool bWait = false;

					for (int y = rectTabROI.bottom; y > rectTabROI.top; y--)
					{
						if (!m_bTabEdgeSelectImage[nCam])
						{
							if (m_nTabThresholdBottomSide[nCam] < *(fmDark + nw * y + x))
							{
								m_vecBottomY_Candidate[nCam].push_back(y);
								m_vecBottomX_Candidate[nCam].push_back(x);
								break;
							}

							if (bWait)
							{
								if (m_nShadowThreshold[nCam] < *(fmDark + nw * y + x))
								{
									m_vecBottomY_Candidate[nCam].push_back(y);
									m_vecBottomX_Candidate[nCam].push_back(x);
									break;
								}
							}

							if (m_nShadowThreshold[nCam] > *(fmDark + nw * y + x) && !bWait)
							{
								nWaitCount++;

								if (nWaitCount > 3)
									bWait = true;
							}
						}
						else
						{
							if (m_nTabThresholdBottomSide[nCam] < *(fmbright + nw * y + x))
							{
								m_vecBottomY_Candidate[nCam].push_back(y);
								m_vecBottomX_Candidate[nCam].push_back(x);
								break;
							}
							
							if (bWait)
							{
								if (m_nShadowThreshold[nCam] < *(fmbright + nw * y + x))
								{
									m_vecBottomY_Candidate[nCam].push_back(y);
									m_vecBottomX_Candidate[nCam].push_back(x);
									break;
								}
							}

							if (m_nShadowThreshold[nCam] > *(fmbright + nw * y + x) && !bWait)
							{
								nWaitCount++;

								if (nWaitCount > 3)
									bWait = true;
							}
						}

						if (y < rectTabROI.bottom - rectTabROI.Height() / 2 - 50)
							break;
					}
				}

				for (int i = 0; i < m_vecBottomY_Candidate[nCam].size(); i++)
				{
					if (m_vecBottomY_ValueList[nCam].size() <= 0)
						m_vecBottomY_ValueList[nCam].push_back(m_vecBottomY_Candidate[nCam][i]);

					bool bPush = true;

					for (int j = 0; j < m_vecBottomY_ValueList[nCam].size(); j++)
					{
						if (m_vecBottomY_ValueList[nCam][j] == m_vecBottomY_Candidate[nCam][i])
						{
							bPush = false;
							break;
						}
					}

					if (bPush)
						m_vecBottomY_ValueList[nCam].push_back(m_vecBottomY_Candidate[nCam][i]);
				}

				const int nBottomY_ValueListSize2 = m_vecBottomY_ValueList[nCam].size();
				int* nBottomY_CandidateCount2 = new int[nBottomY_ValueListSize2];
				memset(nBottomY_CandidateCount2, 0, sizeof(int) * nBottomY_ValueListSize2);
				nBottomY = 0;

				std::sort(m_vecBottomY_ValueList[nCam].begin(), m_vecBottomY_ValueList[nCam].end(), greater<long>());

				std::vector <long> vecTemp;
				std::vector <long> vecTempX;
				vecTemp.clear();
				vecTempX.clear();

				int nCandidateSum = 0;

				for (int i = 0; i < m_vecBottomY_Candidate[nCam].size(); i++)
				{
					nCandidateSum += m_vecBottomY_Candidate[nCam][i];
				}

				int nCandidateMidValue = 0;
				int nCandidateAvg = 0;

				if (m_vecBottomY_Candidate[nCam].size() > 0)
				{
					nCandidateAvg = nCandidateSum / m_vecBottomY_Candidate[nCam].size();

					std::vector <long> vecBottomY_CandidateSort;
					vecBottomY_CandidateSort.assign(m_vecBottomY_Candidate[nCam].begin(), m_vecBottomY_Candidate[nCam].end());
					std::sort(vecBottomY_CandidateSort.begin(), vecBottomY_CandidateSort.end());

					if (vecBottomY_CandidateSort.size() >= 2)
					{
						nCandidateMidValue = vecBottomY_CandidateSort[(vecBottomY_CandidateSort.size() / 2) - 1];
					}
					else if (vecBottomY_CandidateSort.size() == 1)
					{
						nCandidateMidValue = vecBottomY_CandidateSort[0];
					}
				}

				float fSquareSum = 0;

				for (int i = 0; i < m_vecBottomY_Candidate[nCam].size(); i++)
				{
					fSquareSum += pow(abs(nCandidateMidValue - m_vecBottomY_Candidate[nCam][i]), 2);
				}

				float fSquareAvg = 0;

				if (m_vecBottomY_Candidate[nCam].size() > 0)
					fSquareAvg = fSquareSum / m_vecBottomY_Candidate[nCam].size();

				double dStdDev = sqrt(fSquareAvg);

				m_vecBottomX_Excepted[nCam].clear();
				m_vecBottomY_Excepted[nCam].clear();

				float fMaxZ_Score = -10e10;

				std::vector <float> vecZ_Score;
				vecZ_Score.clear();

				for (int i = 0; i < m_vecBottomY_Candidate[nCam].size(); i++)
				{
					float fRes = abs(m_vecBottomY_Candidate[nCam][i] - nCandidateMidValue) / dStdDev;
					vecZ_Score.push_back(fRes);
					fMaxZ_Score = max(fMaxZ_Score, fRes);
				}
				
				for (int i = 0; i < vecZ_Score.size(); i++)
				{
					float fValue = vecZ_Score[i] / fMaxZ_Score;

					if (m_fEdgePointFilterValue[nCam] >= vecZ_Score[i] / fMaxZ_Score || m_vecBottomY_Candidate[nCam][i] == nCandidateMidValue)
					{
						vecTemp.push_back(m_vecBottomY_Candidate[nCam][i]);
						vecTempX.push_back(m_vecBottomX_Candidate[nCam][i]);
					}
					else
					{
						m_vecBottomX_Excepted[nCam].push_back(m_vecBottomX_Candidate[nCam][i]);
						m_vecBottomY_Excepted[nCam].push_back(m_vecBottomY_Candidate[nCam][i]);
					}
				}

				m_vecBottomY_Candidate[nCam].clear();
				m_vecBottomY_Candidate[nCam].assign(vecTemp.begin(), vecTemp.end());
				m_vecBottomX_Candidate[nCam].clear();
				m_vecBottomX_Candidate[nCam].assign(vecTempX.begin(), vecTempX.end());
				/*std::vector <long> vecTemp;
				vecTemp.clear();
				vecTemp.assign(m_vecBottomY_Candidate[nCam].begin(), m_vecBottomY_Candidate[nCam].end());*/
				std::sort(vecTemp.begin(), vecTemp.end(), greater<long>());

				bool bBreak = false;

				for (int i = 0; i < vecTemp.size(); i++)
				{
					for (int j = 0; j < m_vecBottomY_ValueList[nCam].size(); j++)
					{
						if (m_vecBottomY_ValueList[nCam].at(j) == vecTemp.at(i))
						{
							nBottomY_CandidateCount2[j]++;

							if (nBottomY_CandidateCount2[j] >= m_nTabWhiteCount[nCam] / 3)
							{
								nBottomY = m_vecBottomY_ValueList[nCam].at(j);
								bBreak = true;
							}

							break;
						}
					}

					if (bBreak)
						break;
				}

				//m_vecBottomY_Candidate[nCam].clear();
				//m_vecBottomY_ValueList[nCam].clear();
				if (nBottomY <= 0)
				{
					if (m_vecBottomY_ValueList[nCam].size() > 0)
					{
						int nMaxCountIndex = 0;
						int nMaxValue = 0;

						for (int i = 0; i < nBottomY_ValueListSize2; i++)
						{
							int nValue = nBottomY_CandidateCount2[i];
							nMaxValue = max(nMaxValue, nValue);

							if (nValue == nMaxValue)
								nMaxCountIndex = i;
						}

						nBottomY = m_vecBottomY_ValueList[nCam][nMaxCountIndex];
					}
				}

				vecTemp.clear();
				delete[]nBottomY_CandidateCount2;
			}
		}

		nCalcTabWidth = nBottomY - nTopY;

		if (nTabWidth > nCalcTabWidth + 15 || nTabWidth < nCalcTabWidth - 15)
		{
			int nPrevTabTop = m_rtCellTab[nCam].top;
			int nPrevTabBottom = m_rtCellTab[nCam].bottom;

			if (nPrevTabTop <= 0)
				nPrevTabTop = m_nCellMargin + m_nOffsetUp[nCam];
			if (nPrevTabBottom <= 0)
				nPrevTabBottom = nh - m_nCellMargin - m_nOffsetDn[nCam];

			bool bInvalidTop = false, bInvalidBottom = false;

			if (nTopY > nPrevTabTop + 25 || nTopY < nPrevTabTop - 25)
				bInvalidTop = true;

			if (nBottomY > nPrevTabBottom + 25 || nBottomY < nPrevTabBottom - 25)
				bInvalidBottom = true;

			if (!bInvalidTop && bInvalidBottom)
			{
				if (!bBottomValidValue && m_bTabTopBottomForceCorrection[nCam])
				{
					nBottomY = nTopY + nTabWidth;
					m_bForceCorrectionActivated[nCam] = true; 
				}
			}
			else if (bInvalidTop && !bInvalidBottom)
			{
				if (!bTopValidValue || m_bTabTopBottomForceCorrection[nCam])
				{
					nTopY = nBottomY - nTabWidth;
					m_bForceCorrectionActivated[nCam] = true;
				}
			}
			else if (bInvalidTop && bInvalidBottom && m_bTabTopBottomForceCorrection[nCam])
			{
				m_bForceCorrectionActivated[nCam] = true;

				if(!bTopValidValue)
					nTopY = nPrevTabTop;
				if(!bBottomValidValue)
					nBottomY = nPrevTabBottom;

				nCalcTabWidth = nBottomY - nTopY;

				if (nTabWidth > nCalcTabWidth + 15 || nTabWidth < nCalcTabWidth - 15)
				{
					if(bTopValidValue)
						nTopY = nBottomY - nTabWidth;
					else if(bBottomValidValue)
						nBottomY = nTopY + nTabWidth;
				}
			}
		}
	}

	// 예외처리
	if (nTopY < 10 || nBottomY >= (nh - 10) || nTopY >= nBottomY)	//이미지 최상단 근처에 있다면 10pixel 까지 내려주기 
	{
		*ntop1 = 10;
		*nbottom1 = 10 + nTabWidth;
		return 0;
	}

	*ntop1 = nTopY;
	*nbottom1 = nBottomY;

	int nX_EndPos = rectTabROI.left + (m_nTabWhiteCount[nCam] / 2);

	if (nX_EndPos > rectTabROI.right)
		nX_EndPos = rectTabROI.right;


	CRect rtSearch(rectTabROI.left, nTopY, nX_EndPos, nBottomY);
	CRect rtImage(10, 10, nw - 10, nh - 10);
	rtSearch &= rtImage;


	if (rtSearch.Height() <= 0 || rtSearch.Width() <= 0)
	{
		*ntop1 = 10;
		*nbottom1 = 10 + nTabWidth;
		return 0;
	}
	
	long nAverage = 0;
	long nSumX = 0, nSumY = 0;
	for (long x = rtSearch.left; x < rtSearch.right; x++)
	{
		nSumY = 0;

		for (long y = rtSearch.top; y < rtSearch.bottom; y++)
		{
			nSumY += *(fmDark + nw * y + x);
		}
		nSumX += nSumY / rtSearch.Height();
	}
	nAverage = nSumX / rtSearch.Width();

	return nAverage;
}
//YYS

void CInspect::fnScrabMain(int nCam)
{
	int nw = m_nWidth;
	int nh = m_nHeight;
	BYTE * fm = m_fmEven[nCam];//bright
	BYTE * fmDark = m_fmOdd[nCam];//dark

	double dCoatTopEdge = m_rtCell[nCam].right;
	m_nScrabRangDefect[nCam] = 0;
	int nDefect=0;
	if(m_bInsScrab[nCam]){
		if(m_bFindWhiteTape[nCam] && m_bTapeInScrab[nCam] ==FALSE)//테이프가 검출되었고 테이프를 스크랩으로 검출 안해야 하는경우 Skip
			;
		else
			nDefect = FindScrab(nCam, fm, fmDark, m_nWidth, m_nHeight, dCoatTopEdge);// nCoatEdge1);//0930//211002
	}

	m_bScrabCheck[nCam] = FALSE;
	{
		CSingleLock lock(&m_cs_ScrabThreadEnd[nCam], TRUE);
		m_bScrabThreadEnd[nCam] = true;
	}
}

void CInspect::FindBlobEdge(int nCam, BYTE* fm, int nLeftRight,int nth)
{

}

int	CInspect::CrackRecheck( BYTE* fmPre, int nCam, int nIdx, int nDefect, int cx, int cy)//nbw 3 무지
{
	int nx = BAD_WIDTH*2;
	int ny = BAD_HEIGHT*2;
	BYTE * fm1 = new BYTE[nx*ny];//white
	CRect rt;
	rt.SetRect(cx-nx*0.5,cy-ny*0.5,cx+nx*0.5,cy+ny*0.5);
	rt = SetBoundary2(rt, CRect(m_rtCell[nCam].left, 0, m_rtCell[nCam].right, m_nHeight));
	int ngap = 1;//2;
	int nTmp = 0, nThb = m_nTh2[nCam];//
	int nmarginb = m_nMargin[nCam][1];//흑
	int nthb1 = nThb - nmarginb;//size를 재기위해 마진을 더둔다.
	for (int y = rt.top,y1=0; y < rt.bottom; y++,y1++) {
		for (int x = rt.left,x1=0; x < rt.right; x++,x1++) {
			if (y <= ngap+rt.top || y >= rt.bottom - 1 - ngap || x <= rt.left + ngap || x >= rt.right - 1 - ngap)
				*(fm1 + nx*y1 + x1) =  0;//테두리를 흑색으로 치환한다. blob에서 에지가 걸리면 검출이 안된.
			else {
				*(fm1 + nx*y1 + x1) = 0;
				 if (*(fmPre + m_nWidth*y + x) - m_nTargetBrightness[nCam] < -nThb) {
					*(fm1 + nx*y1 + x1) = 255;
				}
			}
		}
	}
	CRect rtBlob;
	int nBlobNo = 0;
	double dBlobSize=0.,dTemp=0.;
	m_pChain[nCam][nIdx][1]->SetChainData(1, fm1, 4, 4, 10, 100000, nx, ny);//white
	int nBlobCount = m_pChain[nCam][nIdx][1]->FastChain(1, 1, nx - 1, ny - 1);
	for (int i = 0; i < nBlobCount; i++)
	{
		dTemp = m_pChain[nCam][nIdx][1]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp < 20)continue;
		if (dTemp >= dBlobSize)//최대 불량 사이즈.
		{
			nBlobNo = i;	dBlobSize = dTemp;//면적//
		}
	}
	double dcx = 0, dcy = 0;
	int nret = 0;
	if(nBlobCount>0){
		int nx11 = m_pChain[nCam][nIdx][1]->FindMaxX(nBlobNo);
		int nx12 = m_pChain[nCam][nIdx][1]->FindMinX(nBlobNo);
		int ny11 = m_pChain[nCam][nIdx][1]->FindMaxY(nBlobNo);
		int ny12 = m_pChain[nCam][nIdx][1]->FindMinY(nBlobNo);

		m_dSubSizeX[nCam][nIdx][nDefect][1] = (nx11 - nx12 )*m_dscale_x1[nCam];
		m_dSubSizeY[nCam][nIdx][nDefect][1] = (ny11 - ny12)*m_dscale_y1[nCam];

		if (m_bCrackSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCrackSizeX[nCam] && m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCrackSizeY[nCam])) {//크랙
			nret = 3003;//strbadtype = "크랙"; 
		}
		else if(!m_bCrackSizeAndOr && (m_dSubSizeX[nCam][nIdx][nDefect][1] >= m_dCrackSizeX[nCam] || m_dSubSizeY[nCam][nIdx][nDefect][1] >= m_dCrackSizeY[nCam])) {//크랙
			nret = 3003;//strbadtype = "크랙"; 
		}

	}

	delete [] fm1;
	return nret;
}

double CInspect::FindMinSizeX(int nCam)
{
	double nSizeX = 0;
	nSizeX = min(m_dDarkSizeX[nCam], m_dPinholeCraterBSizeX[nCam]);
	nSizeX = min(nSizeX, m_dCrackSizeX[nCam]);
	nSizeX = min(nSizeX, m_dBlackLineSizeX[nCam]);
	nSizeX = min(nSizeX, m_dCunicSizeX[nCam]);
	if(TRUE == m_bMiniCrater[nCam])
		nSizeX = min(nSizeX, m_dMiniCrater_SizeX[nCam]);
	if (TRUE == m_bMiniDent[nCam])
		nSizeX = min(nSizeX, m_dMiniDent_SizeX[nCam]);
	return nSizeX;
}

double CInspect::FindMinSizeY(int nCam)
{
	double nSizeY = 0;

	nSizeY = min(m_dDarkSizeY[nCam], m_dPinholeCraterBSizeY[nCam]);
	nSizeY = min(nSizeY, m_dCrackSizeY[nCam]);
	nSizeY = min(nSizeY, m_dBlackLineSizeY[nCam]);
	nSizeY = min(nSizeY, m_dCunicSizeY[nCam]);
	if (TRUE == m_bMiniCrater[nCam])
		nSizeY = min(nSizeY, m_dMiniCrater_SizeY[nCam]);
	if (TRUE == m_bMiniDent[nCam])
		nSizeY = min(nSizeY, m_dMiniDent_SizeY[nCam]);
	return nSizeY;

}

int CInspect::FindDefectAtPreprocess(int nCam, BYTE* fmPreprocess, int nw, int nh, int* pPosInfo)
{
//	m_dInsSubTime1[nCam][1] = GetPrecisionTime();// - m_dInsSubTime1[nCam][0]) * 1000;

	int nDefect = 0;//m_nIslandCount[nCam];
	if (nDefect > MAX_HUGEDEFECT - 2) return nDefect;
	BYTE* fm = fmPreprocess;
	int nLeft = pPosInfo[0], nRight = pPosInfo[1], nTop = pPosInfo[2], nBottom = pPosInfo[3];

	int nTh = 20;
	CRect rtins, rtins1;//220109
	rtins.SetRect(nLeft, nTop, nRight, nBottom);
	rtins = rtins & CRect(m_rtCell[nCam].left + 1, 0, m_rtCell[nCam].right - 1, nh);//220211 옵셋1 픽셀 //기울어져 있을경우 어떻게?
	rtins1 = rtins;//220109
	//m_rtScrabIns[nCam] = rtins;
	int nCompress = 1, nCompressY = 1;//220330
	int nw1 = nw, nh1 = nh;//220109
	if (rtins.Width() > 5000) {//220512 -우선 기존것 유지 
//image Compress
		nCompress = 4;//220330
		nCompressY = 2;//220330
		rtins1.SetRect((nLeft) / nCompress, (nTop) / nCompressY, (nRight) / nCompress, (nBottom) / nCompressY);//220406
		rtins1 = rtins1 & CRect(m_rtCell[nCam].left / nCompress, 0, m_rtCell[nCam].right / nCompress, nh / nCompressY);//220406
		nw1 /= nCompress;//220109
		nh1 /= nCompressY;//220109
		//rtins = rtins & CRect(0, 0, nw, nh);
	}
	else if (rtins.Width() > 500) {//220330
		nCompress = 2;//220330
		nCompressY = 2;//220330
		rtins1.SetRect((nLeft) / nCompress, (nTop) / nCompressY, (nRight) / nCompress, (nBottom) / nCompressY);//220109
		rtins1 = rtins1 & CRect(m_rtCell[nCam].left / nCompress, 0, m_rtCell[nCam].right / nCompress, nh / nCompressY);
		nw1 /= nCompress;//220109
		nh1 /= nCompressY;//220109
	}
	double dave = 0, dc = 0;
	for (int y = rtins.top; y < rtins.bottom && y >= 0 && y < nh; y += nCompressY) {//220330
		for (int x = rtins.left; x < rtins.right && x >= 0; x += nCompress) {//220109
			dave += *(fm + (nw)*y + x);
			dc++;
		}
	}
	if (dc == 0) return 0;
	dave /= dc;
	BYTE* fmNonCoat = new BYTE[nw1 * nh1];//220109
	memset(fmNonCoat, 0, nw1 * nh1);//220109
	if (m_bTiltEdge[nCam]) {//220403
		for (int y = rtins.top; y < rtins.bottom && y >= 0 && y < nh && y < 3000; y += nCompressY) {//220109
			int nsX = m_nLRedge[nCam][0][y];///nCompress;//220509  //
			int neX = m_nLRedge[nCam][1][y];///nCompress;//220509
			if (rtins.left > nsX) nsX = rtins.left;
			if (rtins.right < neX) neX = rtins.right;
			for (int x = nsX; x < neX && x >= 0; x += nCompress) {//220109
				if (*(fm + (nw)*y + x) >= dave + nTh) {
					int x11 = x / nCompress;
					int y11 = y / nCompressY;
					if (x11 >= nw1 || y11 >= nh1)continue;//220109
					*(fmNonCoat + nw1 * y11 + x11) = 0xff;//220109
				}
			}
		}
	}
	else {
		for (int y = rtins.top; y < rtins.bottom && y >= 0 && y < nh; y += nCompressY) {//220109
			for (int x = rtins.left; x < rtins.right && x >= 0; x += nCompress) {//220109
				if (*(fm + (nw)*y + x) >= dave + nTh) {
					int x11 = x / nCompress;
					int y11 = y / nCompressY;
					if (x11 >= nw1 || y11 >= nh1)continue;//220109
					*(fmNonCoat + nw1 * y11 + x11) = 0xff;//220109
				}
			}
		}
	}

	//IPVM::Image_8u_C1 image(nw1, nw1, nh1, fmNonCoat);
	//image.SaveBmp(_T("Preprocess_Test.bmp"));

	//20220916 SJM
	//if (m_bIstest) {
	//	m_nScrabWidth = nw1; m_nScrabHeight = nh1;
	//	memcpy(m_ScrabImage, fmNonCoat, sizeof(BYTE) * nw1 * nh1);
	//}
	//20220916 SJM
	m_pChainAtPreprocess[nCam]->SetChainData(1, fmNonCoat, 1, 1, 4, 100000, nw1, nh1);//white//220109
	int nBlobCount14 = m_pChainAtPreprocess[nCam]->FastChain(rtins1.left, rtins1.top, rtins1.right - 1, rtins1.bottom - 1);//220109
	double dTemp14 = 0, nBlobNo14old = 0, dBlobSize14 = 0; int nBlobNo14 = -1;
	BOOL bFullSearch = TRUE;//220125 
	for (int i = 0; i < nBlobCount14; i++)//미코팅 탈리 왼쪽//0910
	{
		dTemp14 = m_pChainAtPreprocess[nCam]->Chain_Area(i);//FChain_Area(i, &dTemp);//			ttt
		if (dTemp14 < 4)continue;

		double dcx12 = 0, dcy12 = 0;
		m_pChainAtPreprocess[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
		CPoint pt14;
		pt14.SetPoint(dcx12 * nCompress, dcy12 * nCompressY);
		int nJump = 0;
		for (int k = 0; k < 4; k++) {
			if (m_rtNotchArea[nCam][k].PtInRect(pt14)) {
				nJump = k + 1;//220406
				break;
			}
		}
		if (nJump >= 1)//220406
			continue;
		if (dTemp14 > nBlobNo14old) {
			nBlobNo14old = dTemp14;
			nBlobNo14 = i;
		}
		if (bFullSearch) {
			int nx1 = m_pChainAtPreprocess[nCam]->FindMaxX(i);//0625-1
			int nx2 = m_pChainAtPreprocess[nCam]->FindMinX(i);//0625-1
			int ny1 = m_pChainAtPreprocess[nCam]->FindMaxY(i);//0625-1
			int ny2 = m_pChainAtPreprocess[nCam]->FindMinY(i);//0625-1
			double dsize1 = (nx1 - nx2 + 1) * m_dscale_x1[nCam] * nCompress;
			double dsize2 = (ny1 - ny2 + 1) * m_dscale_y1[nCam] * nCompressY;
			double dsize3 = dsize1;
			if (dsize3 < dsize2)dsize3 = dsize2;

			double dMin = min(dsize1, dsize2);
			double dMax = max(dsize1, dsize2);

			if (dsize1 > 0 && dsize2 > 0)
			{

				if (m_nFullDefectCount[nCam] < MAX_FULL_DEFECT)
				{
					double dcx12 = 0, dcy12 = 0;
					int nw2 = BAD_WIDTH * 0.5, nh2 = BAD_HEIGHT * 0.5;
					m_pChainAtPreprocess[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
					CRect rtDefect;
					rtDefect.SetRect(dcx12* nCompress - nw2, dcy12* nCompressY - nh2, dcx12* nCompress + nw2, dcy12* nCompressY + nh2);
					rtDefect = SetBoundary2(rtDefect, CRect(10, 0, nw1 * nCompress - 100, nh1 * nCompressY));

					double dDefectSizeX = dsize1;
					double dDefectSizeY = dsize2;

					bool bLine = false;

					if (dMin <= dMax / m_dXYRatio[nCam])
						bLine = true;

					bool bDefect = false;

					if (bLine)
					{
						if (m_bLineSizeAndOr && (m_dWhiteLineSizeX[nCam] <= dDefectSizeX && m_dWhiteLineSizeY[nCam] <= dDefectSizeY))
							bDefect = true;
						else if (!m_bLineSizeAndOr && (m_dWhiteLineSizeX[nCam] <= dDefectSizeX || m_dWhiteLineSizeY[nCam] <= dDefectSizeY))
							bDefect = true;
					}
					else
					{
						if (m_bPinholeAndOr && (m_dPinholeSizeX[nCam] <= dDefectSizeX && m_dPinholeSizeY[nCam] <= dDefectSizeY))
							bDefect = true;
						else if (!m_bPinholeAndOr && (m_dPinholeSizeX[nCam] <= dDefectSizeX || m_dPinholeSizeY[nCam] <= dDefectSizeY))
							bDefect = true;
					}

					m_nFullDefectCount[nCam]++;

					//m_rtFullDefect[nCam][m_nFullDefectCount[nCam] - 1] = rtDefect;
					m_rtFullDefect[nCam][m_nFullDefectCount[nCam] - 1] = rtDefect;
					m_rtFullDefect[nCam][m_nFullDefectCount[nCam] - 1].top = m_nHeight - rtDefect.bottom;
					m_rtFullDefect[nCam][m_nFullDefectCount[nCam] - 1].bottom = m_nHeight - rtDefect.top;

					m_dFullStdValue[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullStdValue[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullCompactness[nCam][m_nFullDefectCount[nCam] - 1][0] = m_pChainAtPreprocess[nCam]->FindCompactness(i);
					m_dFullCompactness[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullDistance[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullDistance[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullPercent[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullPercent[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullPeakDataW[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;
					m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam] - 1][0] = 0;
					m_dFullPeakDataB[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;

					m_dFullAngle[nCam][m_nFullDefectCount[nCam] - 1][0] = m_pChainAtPreprocess[nCam]->FindAngle(i);
					m_dFullAngle[nCam][m_nFullDefectCount[nCam] - 1][1] = 0;

					m_dFullSizeX[nCam][m_nFullDefectCount[nCam] - 1][0] = dDefectSizeX;
					m_dFullSizeX[nCam][m_nFullDefectCount[nCam] - 1][1] = dDefectSizeX;
					m_dFullSizeX[nCam][m_nFullDefectCount[nCam] - 1][2] = dDefectSizeX;

					m_dFullSizeY[nCam][m_nFullDefectCount[nCam] - 1][0] = dDefectSizeY;
					m_dFullSizeY[nCam][m_nFullDefectCount[nCam] - 1][1] = dDefectSizeY;
					m_dFullSizeY[nCam][m_nFullDefectCount[nCam] - 1][2] = dDefectSizeY;

					if (bDefect)
					{
						if (bLine)
						{
							m_nFullDefectType[nCam][m_nFullDefectCount[nCam] - 1] = 1001; //라인
							m_strFullDefectName[nCam][m_nFullDefectCount[nCam] - 1] = GetDefectName(1001); //라인
						}
						else
						{
							m_nFullDefectType[nCam][m_nFullDefectCount[nCam] - 1] = 2001; //핀홀
							m_strFullDefectName[nCam][m_nFullDefectCount[nCam] - 1] = GetDefectName(2001); //핀홀
						}
					}
					else
					{
						m_nFullDefectType[nCam][m_nFullDefectCount[nCam] - 1] = 0; //라인
						m_strFullDefectName[nCam][m_nFullDefectCount[nCam] - 1] = GetDefectName(0); //라인
					}

					//	nDefect++;
				}
			}

			//if (m_bScrabSizeAndOr) {//and //220804
			//	//if (dsize3>= m_dSpecScarabSize[nCam] && dsize1 >= m_dScrabSpecSizeX[nCam] && dsize2 >= m_dScrabSpecSizeY[nCam])
			//	if (dsize3 >= min(m_dScrabSpecSizeX[nCam], m_dScrabSpecSizeY[nCam]) && dsize1 >= m_dScrabSpecSizeX[nCam] && dsize2 >= m_dScrabSpecSizeY[nCam])
			//	{//스크랩 불량
			//		m_dScrabSize[nCam] = dsize1;
			//		m_bScrabReject[nCam] = TRUE;
			//		double dcx12 = 0, dcy12 = 0;
			//		int nw2 = BAD_WIDTH * 0.5, nh2 = BAD_HEIGHT * 0.5;
			//		m_pChainAtPreprocess[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
			//		m_rtScrabDefect[nCam][nDefect].SetRect(dcx12 * nCompress - nw2, dcy12 * nCompressY - nh2, dcx12 * nCompress + nw2, dcy12 * nCompressY + nh2);
			//		m_rtScrabDefect[nCam][nDefect] = SetBoundary2(m_rtScrabDefect[nCam][nDefect], CRect(10, 0, nw1 * nCompress - 100, nh1 * nCompressY));//220109
			//		m_dScrabDefectSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam] * nCompress;
			//		m_dScrabDefectSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam] * nCompressY;
			//		m_dScrabDefectSize[nCam][nDefect] = 9001;
			//		BYTE* fmdefect1 = m_pScrabDefect[nCam][nDefect];
			//		CRect rt22 = m_rtScrabDefect[nCam][nDefect];
			//		if (!m_bDefectBinary)//0729-1
			//			ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
			//		else
			//			ImageSelectCopy(fmNonCoat, fmdefect1, nw1, nh1, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);//220109
			//		nDefect++;
			//		m_nScrabRangDefect[nCam] = nDefect;//m_nIslandCount[nCam] = nDefect;//220411
			//	}
			//}
			//else {//or //220804
			//	//if (dsize3>= m_dSpecScarabSize[nCam])
			//	if (dsize3 >= min(m_dScrabSpecSizeX[nCam], m_dScrabSpecSizeY[nCam]))
			//	{//스크랩 불량
			//		m_dScrabSize[nCam] = dsize1;
			//		m_bScrabReject[nCam] = TRUE;
			//		double dcx12 = 0, dcy12 = 0;
			//		int nw2 = BAD_WIDTH * 0.5, nh2 = BAD_HEIGHT * 0.5;
			//		m_pChainAtPreprocess[nCam]->Chain_Center(i, &dcx12, &dcy12);//0625-1
			//		m_rtScrabDefect[nCam][nDefect].SetRect(dcx12 * nCompress - nw2, dcy12 * nCompressY - nh2, dcx12 * nCompress + nw2, dcy12 * nCompressY + nh2);
			//		m_rtScrabDefect[nCam][nDefect] = SetBoundary2(m_rtScrabDefect[nCam][nDefect], CRect(10, 0, nw1 * nCompress - 100, nh1 * nCompressY));//220109
			//		m_dScrabDefectSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam] * nCompress;
			//		m_dScrabDefectSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam] * nCompressY;
			//		m_dScrabDefectSize[nCam][nDefect] = 9001;
			//		BYTE* fmdefect1 = m_pScrabDefect[nCam][nDefect];
			//		CRect rt22 = m_rtScrabDefect[nCam][nDefect];
			//		if (!m_bDefectBinary)//0729-1
			//			ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
			//		else
			//			ImageSelectCopy(fmNonCoat, fmdefect1, nw1, nh1, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);//220109
			//		nDefect++;
			//		m_nScrabRangDefect[nCam] = nDefect;//m_nIslandCount[nCam] = nDefect;//220411
			//	}
			//}

			//if (nDefect > MAX_HUGEDEFECT - 2)break;
		}

	}
	//if (nBlobNo14 >= 0 && !bFullSearch) {
	//	int nx1 = m_pChainAtPreprocess[nCam]->FindMaxX(nBlobNo14);//0625-1
	//	int nx2 = m_pChainAtPreprocess[nCam]->FindMinX(nBlobNo14);//0625-1
	//	int ny1 = m_pChainAtPreprocess[nCam]->FindMaxY(nBlobNo14);//0625-1
	//	int ny2 = m_pChainAtPreprocess[nCam]->FindMinY(nBlobNo14);//0625-1
	//	double dsize1 = (nx1 - nx2 + 1) * m_dscale_x1[nCam] * nCompress;
	//	double dsize2 = (ny1 - ny2 + 1) * m_dscale_y1[nCam] * nCompressY;
	//	m_dScrabSize[nCam] = dsize1;
	//	if (m_dScrabSize[nCam] < dsize2)m_dScrabSize[nCam] = dsize2;

	//	if (m_bScrabSizeAndOr) {//and //220804
	//		//if (m_dScrabSize[nCam] >= m_dSpecScarabSize[nCam] && dsize1 >= m_dScrabSpecSizeX[nCam] && dsize2 >= m_dScrabSpecSizeY[nCam]) 
	//		if (m_dScrabSize[nCam] >= min(m_dScrabSpecSizeX[nCam], m_dScrabSpecSizeY[nCam]) && dsize1 >= m_dScrabSpecSizeX[nCam] && dsize2 >= m_dScrabSpecSizeY[nCam])
	//		{//스크랩 불량
	//			m_bScrabReject[nCam] = TRUE;
	//			double dcx12 = 0, dcy12 = 0;
	//			int nw2 = BAD_WIDTH * 0.5, nh2 = BAD_HEIGHT * 0.5;
	//			m_pChainAtPreprocess[nCam]->Chain_Center(nBlobNo14, &dcx12, &dcy12);//0625-1
	//			m_rtScrabDefect[nCam][nDefect].SetRect(dcx12 * nCompress - nw2, dcy12 * nCompressY - nh2, dcx12 * nCompress + nw2, dcy12 * nCompressY + nh2);
	//			m_rtScrabDefect[nCam][nDefect] = SetBoundary2(m_rtScrabDefect[nCam][nDefect], CRect(10, 0, nw1 * nCompress - 100, nh1 * nCompressY));//220109
	//			m_dScrabDefectSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam] * nCompress;
	//			m_dScrabDefectSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam] * nCompressY;
	//			m_dScrabDefectSize[nCam][nDefect] = 9001;
	//			BYTE* fmdefect1 = m_pScrabDefect[nCam][nDefect];
	//			CRect rt22 = m_rtScrabDefect[nCam][nDefect];
	//			if (!m_bDefectBinary)//0729-1
	//				ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
	//			else
	//				ImageSelectCopy(fmNonCoat, fmdefect1, nw1, nh1, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);//220109
	//			nDefect++;
	//			m_nScrabRangDefect[nCam] = nDefect;//m_nIslandCount[nCam] = nDefect;//220411
	//		}
	//	}
	//	else {//or //220804
	//		//if (m_dScrabSize[nCam] >= m_dSpecScarabSize[nCam]) 
	//		if (m_dScrabSize[nCam] >= min(m_dScrabSpecSizeX[nCam], m_dScrabSpecSizeY[nCam]))
	//		{//스크랩 불량
	//			m_bScrabReject[nCam] = TRUE;
	//			double dcx12 = 0, dcy12 = 0;
	//			int nw2 = BAD_WIDTH * 0.5, nh2 = BAD_HEIGHT * 0.5;
	//			m_pChainAtPreprocess[nCam]->Chain_Center(nBlobNo14, &dcx12, &dcy12);//0625-1
	//			m_rtScrabDefect[nCam][nDefect].SetRect(dcx12 * nCompress - nw2, dcy12 * nCompressY - nh2, dcx12 * nCompress + nw2, dcy12 * nCompressY + nh2);
	//			m_rtScrabDefect[nCam][nDefect] = SetBoundary2(m_rtScrabDefect[nCam][nDefect], CRect(10, 0, nw1 * nCompress - 100, nh1 * nCompressY));//220109
	//			m_dScrabDefectSizeX[nCam][nDefect] = (nx1 - nx2 + 1) * m_dscale_x1[nCam] * nCompress;
	//			m_dScrabDefectSizeY[nCam][nDefect] = (ny1 - ny2 + 1) * m_dscale_y1[nCam] * nCompressY;
	//			m_dScrabDefectSize[nCam][nDefect] = 9001;
	//			BYTE* fmdefect1 = m_pScrabDefect[nCam][nDefect];
	//			CRect rt22 = m_rtScrabDefect[nCam][nDefect];
	//			if (!m_bDefectBinary)//0729-1
	//				ImageSelectCopy(fm, fmdefect1, nw, nh, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);
	//			else
	//				ImageSelectCopy(fmNonCoat, fmdefect1, nw1, nh1, rt22, BAD_WIDTH, BAD_HEIGHT, 0, 0);//220109
	//			nDefect++;
	//			m_nScrabRangDefect[nCam] = nDefect;//m_nIslandCount[nCam] = nDefect;//220411
	//		}
	//	}
	//}
	delete[]fmNonCoat;//0618
	//m_nScrabRangDefect[nCam] = nDefect;//m_nIslandCount[nCam] = nDefect;//220411
	//m_nIslandCount[nCam] = nDefect;//220207

	return nDefect;
}