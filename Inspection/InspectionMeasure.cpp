#include "stdafx.h"
#include "InspectionMeasure.h"
#include "InspectionCell.h"
#include "InspectionCellAlign.h"
#include "DebugInfo.h"
#include "Accessor.h"
#include "UnionCellResult.h"

#include "Types/LineEq_32f.h"
#include "Algorithm/DataFitting.h"
#include "Types/LineEq_64f.h"
#include "Algorithm/Geometry.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "Algorithm/ImageProcessing.h"
#include <math.h>

#include "Algorithm/ImageProcessing.h"
#include "Algorithm/DataFitting.h"
#include "Algorithm/Geometry.h"
#include "Algorithm/EdgeDetection.h"
#include "Algorithm/EdgeDetectionFilter.h"
#include "Algorithm/EdgeDetectionDebugInfo.h"
#include "Algorithm/Mathematics.h"
#include "Base/LoggerInterface.h"
#include "Types/Point_32f_C2.h"
#include "Types/Point_32f_C3.h"
#include "Types/Image_8u_C1.h"
#include "Types/Point_64f_C2.h"
#include "Types/LineEq_64f.h"
#include "Types/LineEq_32f.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInspectionMeasure::CInspectionMeasure(CProcessingBase* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
	: CInspectionBase(parent, ProcItemCategory::measure, logger, loggerResult)
	, m_parent((CInspectionCell*) parent)
	, m_measureTabHeight(Inspection::MeasureOffsetType::TabHeight)
	, m_measureTabHeight2(Inspection::MeasureOffsetType::TabHeight2)
	, m_measureTabPitch(Inspection::MeasureOffsetType::TabPitch)
	, m_measureTabSide(Inspection::MeasureOffsetType::TabSide)
	, m_measureTabSide2(Inspection::MeasureOffsetType::TabSide)
	, m_measureTabWidth(Inspection::MeasureOffsetType::TabWidth)
	, m_measureCellLengthTop(Inspection::MeasureOffsetType::CellRightLength)
	, m_measureCellLengthBot(Inspection::MeasureOffsetType::CellLeftLength)//20220625 [jjh]
	, m_measureCellLength2_Top(Inspection::MeasureOffsetType::CellLength2)
	, m_measureCellLength2_Bot(Inspection::MeasureOffsetType::CellLength2)
	, m_measureCuttingHeightTop(Inspection::MeasureOffsetType::CuttingHeightTop)
	, m_measureCuttingHeightBot(Inspection::MeasureOffsetType::CuttingHeightBot)
	, m_measureMisalign(Inspection::MeasureOffsetType::Misalign)
	, m_measureVHomeTiltTop(Inspection::MeasureOffsetType::VHomeTiltTop)
	, m_measureVHomeTiltBot(Inspection::MeasureOffsetType::VHomeTiltBot)
{
	m_fCellPitchErrorUm = 0;
	m_fCellLengthTopErrorUm = 0;
	m_fCellLengthBotErrorUm = 0;
	m_fCellLength2ErrorUm = 0;
	m_fTabWidthErrorUm = 0;
	m_fTabHeightErrorUm = 0;
	m_fTabHeight2ErrorUm = 0;
	m_fMisalignErrorUm = 0;
	m_fVhomeTiltBotErrorUm = m_fVhomeTiltTopErrorUm = 0;
	m_fTabSideErrorUm = 0;

	m_fMisalignNormal = m_fMisalignDetail = -9999.0f;
}

CInspectionMeasure::~CInspectionMeasure()
{
}

bool CInspectionMeasure::IsInspectionEnabled() const
{
	return true;
}

void CInspectionMeasure::ResetInspection()
{
	//m_para.m_bUseTabWidth = false;
	//m_para.m_bUseTabHeight = false;
	//m_para.m_bUseTabHeight2 = false;
	//m_para.m_bUseCellLengthTop = false;
	//m_para.m_bUseCellLengthBot = false;
	//m_para.m_bUseCellLength2 = false;
	//m_para.m_bUseCellPitch = false;
	//m_para.m_bUseVhomeTiltTop = false;
	//m_para.m_bUseVhomeTiltBot = false;
	//m_para.m_bUseMismatch = false;
	//m_para.m_bUseMisalign = false;
	//m_para.m_bUseTabSide = false;
	//m_para.m_bUseCuttingHeightTop = false;
	//m_para.m_bUseCuttingHeightBot = false;
	//m_para.m_bUseVhomeHeightLeft = false;
	//m_para.m_bUseVhomeHeightRight = false;

}

void CInspectionMeasure::PrepareDebugInfo(Inspection::DebugInfo* debugInfo)
{

	
		debugInfo->AddItem(LanguageDef::Debug_Coater_EdgeDetectPoint);
	debugInfo->AddItem(LanguageDef::Debug_Coater_A_FullLength);
	debugInfo->AddItem(LanguageDef::Debug_Coater_B_LeftCoating);
	debugInfo->AddItem(LanguageDef::Debug_Coater_C_RightCoating);
	debugInfo->AddItem(LanguageDef::Debug_Coater_D_LeftFoil);
	debugInfo->AddItem(LanguageDef::Debug_Coater_E_MiddleFoil);
	debugInfo->AddItem(LanguageDef::Debug_Coater_E1_MiddleFoilLeftSlitting);
	debugInfo->AddItem(LanguageDef::Debug_Coater_E2_MiddleFoilRightSlitting);
	debugInfo->AddItem(LanguageDef::Debug_Coater_F_RightFoil);



	//debugInfo->AddItem(LanguageDef::Debug_LinePoint);
	//debugInfo->AddItem(LanguageDef::Debug_VHomeTiltPosition);
	//debugInfo->AddItem(LanguageDef::Debug_TabAreaForHeight);
	//debugInfo->AddItem(LanguageDef::Debug_CellPitch);
	//debugInfo->AddItem(LanguageDef::Debug_CellLength);
	//debugInfo->AddItem(LanguageDef::Debug_CellLength2);
	//debugInfo->AddItem(LanguageDef::Debug_TabWidth);
	//debugInfo->AddItem(LanguageDef::Debug_TabHeight);
	//debugInfo->AddItem(LanguageDef::Debug_TabHeight2);
	//debugInfo->AddItem(LanguageDef::Debug_Misalign);
	//debugInfo->AddItem(LanguageDef::Debug_VHomeTilt);
	//debugInfo->AddItem(LanguageDef::Debug_TabSide);
	//debugInfo->AddItem(LanguageDef::Debug_CuttingHeightTop);
	//debugInfo->AddItem(LanguageDef::Debug_CuttingHeightBot);
	//debugInfo->AddItem(LanguageDef::Debug_VHomeHeight);
}

void CInspectionMeasure::AlignMSA()
{
    // 1. ROI에서 EdgeDetect (Top/Bottom/Left/Right)
    // 2. 찾은 Edge 에서 LineFitting
    // 3. Line 교차점 4 Points 찾기
    // 4. 틀어진 Angle 구하기

    auto& align = *((CInspectionCellAlign*)m_parent->GetProcessingItem(ProcItemCategory::cellAlign));
    auto& inspectionImage = m_inspectionImageBright;
    long countVertical = m_para.m_fCellLengthCalcPosition;

	// Setting Edge Parameter 
	// msa
    m_paraEdgeDetectMSA.m_vecedgePara.resize(4);
    m_paraEdgeDetectMSA.m_vecrtDetect.resize(4);
    m_paraEdgeDetectMSA.m_vecedgeFilter.resize(4);
    m_paraEdgeDetectMSA.m_vecDetecCount.resize(4);
    m_paraEdgeDetectMSA.m_vecRansacThreshold.resize(4); 

	// MSA - up
    {
        m_paraEdgeDetectMSA.m_vecedgePara[0].m_edgeThreshold = 1;
        m_paraEdgeDetectMSA.m_vecedgePara[0].m_edgeType = IPVM::EdgeType::Both;
        m_paraEdgeDetectMSA.m_vecedgePara[0].m_findFirstEdge = false;
        m_paraEdgeDetectMSA.m_vecedgePara[0].m_searchDirection = IPVM::SearchDirection::ToDown;
        m_paraEdgeDetectMSA.m_vecedgeFilter[0].SetParameter(6, 1.5);
        m_paraEdgeDetectMSA.m_vecrtDetect[0] = m_para.m_vecrtDetectLineMSA[0]; // rect
        m_paraEdgeDetectMSA.m_vecDetecCount[0] = 10; // long
        m_paraEdgeDetectMSA.m_vecRansacThreshold[0] = 10; // float

        auto& Para = m_para.m_vecDetectLineParaMSA[0];
        m_para.m_vecDetectLineParaMSA[0].SetParameter(
            m_paraEdgeDetectMSA.m_vecedgePara[0], m_paraEdgeDetectMSA.m_vecedgeFilter[0]);
    }
    // MSA - down
    {
        m_paraEdgeDetectMSA.m_vecedgePara[1].m_edgeThreshold = 1;
        m_paraEdgeDetectMSA.m_vecedgePara[1].m_edgeType = IPVM::EdgeType::Both;
        m_paraEdgeDetectMSA.m_vecedgePara[1].m_findFirstEdge = false;
        m_paraEdgeDetectMSA.m_vecedgePara[1].m_searchDirection = IPVM::SearchDirection::ToUp;
        m_paraEdgeDetectMSA.m_vecedgeFilter[1].SetParameter(6, 1.5);
        m_paraEdgeDetectMSA.m_vecrtDetect[1] = m_para.m_vecrtDetectLineMSA[1];
        m_paraEdgeDetectMSA.m_vecDetecCount[1] = 10;
        m_paraEdgeDetectMSA.m_vecRansacThreshold[1] = 10;

        auto& Para = m_para.m_vecDetectLineParaMSA[1];
        m_para.m_vecDetectLineParaMSA[1].SetParameter(
            m_paraEdgeDetectMSA.m_vecedgePara[1], m_paraEdgeDetectMSA.m_vecedgeFilter[1]);
    }
    // MSA - left
    {
        m_paraEdgeDetectMSA.m_vecedgePara[2].m_edgeThreshold = 1;
        m_paraEdgeDetectMSA.m_vecedgePara[2].m_edgeType = IPVM::EdgeType::Both;
        m_paraEdgeDetectMSA.m_vecedgePara[2].m_findFirstEdge = false;
        m_paraEdgeDetectMSA.m_vecedgePara[2].m_searchDirection = IPVM::SearchDirection::ToRight;
        m_paraEdgeDetectMSA.m_vecedgeFilter[2].SetParameter(6, 1.5);
        m_paraEdgeDetectMSA.m_vecrtDetect[2] = m_para.m_vecrtDetectLineMSA[2]; // rect
        m_paraEdgeDetectMSA.m_vecDetecCount[2] = 10; // long
        m_paraEdgeDetectMSA.m_vecRansacThreshold[2] = 10; // float

        auto& Para = m_para.m_vecDetectLineParaMSA[2];
        m_para.m_vecDetectLineParaMSA[2].SetParameter(
            m_paraEdgeDetectMSA.m_vecedgePara[2], m_paraEdgeDetectMSA.m_vecedgeFilter[2]);
    }
    // MSA - right
    {
        m_paraEdgeDetectMSA.m_vecedgePara[3].m_edgeThreshold = 1;
        m_paraEdgeDetectMSA.m_vecedgePara[3].m_edgeType = IPVM::EdgeType::Both;
        m_paraEdgeDetectMSA.m_vecedgePara[3].m_findFirstEdge = false;
        m_paraEdgeDetectMSA.m_vecedgePara[3].m_searchDirection = IPVM::SearchDirection::ToLeft;
        m_paraEdgeDetectMSA.m_vecedgeFilter[3].SetParameter(6, 1.5);
        m_paraEdgeDetectMSA.m_vecrtDetect[3] = m_para.m_vecrtDetectLineMSA[3];
        m_paraEdgeDetectMSA.m_vecDetecCount[3] = 10;
        m_paraEdgeDetectMSA.m_vecRansacThreshold[3] = 10;

        auto& Para = m_para.m_vecDetectLineParaMSA[3];
        m_para.m_vecDetectLineParaMSA[3].SetParameter(
            m_paraEdgeDetectMSA.m_vecedgePara[3], m_paraEdgeDetectMSA.m_vecedgeFilter[3]);
    }

	m_para.m_vecPtMSA.resize(4);
    m_paraEdgeDetectMSA.m_vecedgePara[0].m_edgeType = IPVM::EdgeType::Both;
    m_paraEdgeDetectMSA.m_vecedgePara[1].m_edgeType = IPVM::EdgeType::Both;
    m_paraEdgeDetectMSA.m_vecedgePara[2].m_edgeType = IPVM::EdgeType::Both;
    m_paraEdgeDetectMSA.m_vecedgePara[3].m_edgeType = IPVM::EdgeType::Both;
    m_paraEdgeDetectMSA.m_vecedgePara[0].m_searchDirection = IPVM::SearchDirection::ToDown;
    m_paraEdgeDetectMSA.m_vecedgePara[1].m_searchDirection = IPVM::SearchDirection::ToUp;
    m_paraEdgeDetectMSA.m_vecedgePara[2].m_searchDirection = IPVM::SearchDirection::ToRight;
    m_paraEdgeDetectMSA.m_vecedgePara[3].m_searchDirection = IPVM::SearchDirection::ToLeft;
    
	std::vector<IPVM::LineEq_64f> lineEq64_CalcData;
    lineEq64_CalcData.resize(4);

	// Edge Detect
	for (long nLine = 0; nLine < 4; nLine++)
    {
        IPVM::Point_32f_C3 pt3;
        std::vector<IPVM::Point_64f_C2> ptUsed;
        std::vector<IPVM::Point_64f_C2> ptReal;

        IPVM::Rect rtHomeSearchTop;
        rtHomeSearchTop = m_paraEdgeDetectMSA.m_vecrtDetect[nLine] & align.m_rectImageValid;

        long nLeft = rtHomeSearchTop.m_left;
        long nRight = rtHomeSearchTop.m_right;
        long nTop = rtHomeSearchTop.m_top;
        long nBottom = rtHomeSearchTop.m_bottom;
        long nSize = nSize = (nRight - nLeft) / 20;
        long nSizeVertical = (nBottom - nTop) / 20;

        ptReal.clear();
        ptUsed.clear();

        for (long n = 0; n < countVertical; n++)
        {
            IPVM::Rect rtSeartchTop = rtHomeSearchTop;
            if (nLine == 0 || nLine == 1)
            {
                rtSeartchTop.m_left = nLeft + nSize * n;
                rtSeartchTop.m_right = nLeft + nSize * (n + 1);
            }
            else if (nLine == 2 || nLine == 3)
            {
                rtSeartchTop.m_top = nTop + nSizeVertical * n;
                rtSeartchTop.m_bottom = nTop + nSizeVertical * (n + 1);
			}

            if (m_paraEdgeDetectMSA.m_edgeDetection->DetectEdge(*((IPVM::Image_8u_C1*)&inspectionImage),
                    m_paraEdgeDetectMSA.m_vecedgePara[nLine], m_paraEdgeDetectMSA.m_vecedgeFilter[nLine], rtSeartchTop,
                    pt3)
                == IPVM::Status::success)
            {
                m_para.m_vecPtMSA[nLine].m_x = pt3.m_x;
                m_para.m_vecPtMSA[nLine].m_y = pt3.m_y;

                IPVM::Point_64f_C2 pt2_64;

				pt2_64.m_x = pt3.m_x;
                pt2_64.m_y = pt3.m_y;

				ptReal.push_back(pt2_64);
            }
        }
        ptUsed.resize(ptReal.size());
		// vecpt로 라인피팅
        long usedNum;
        float distanceThreshold = m_paraEdgeDetectMSA.m_vecRansacThreshold[0];

		IPVM::Status status = IPVM::Status::unknownError;
		if (ptReal.size() != 0)
        {
            status = IPVM::DataFitting::FitToLine_RANSAC(
                ptReal.size(), &ptReal[0], &ptUsed[0], usedNum, lineEq64_CalcData[nLine], distanceThreshold);
        }
    }

	// Line 끼리의 교차점 찾기
	if (lineEq64_CalcData.size() >= 4)
    {
        IPVM::Point_64f_C2 fPosUpLeft(0, 0);
        IPVM::Point_64f_C2 fPosUpRight(0, 0);
        IPVM::Point_64f_C2 fPosLeftExtand(0, 0);
        IPVM::LineEq_64f LineUp, LineLeft, LineRight;

		LineUp = lineEq64_CalcData[0];
        LineLeft = lineEq64_CalcData[2];
        LineRight = lineEq64_CalcData[3];
		
		m_MSAlineUp.m_a = LineUp.m_a;
		m_MSAlineUp.m_b = LineUp.m_b;
		m_MSAlineUp.m_c = LineUp.m_c;
		m_MSAlineLeft.m_a = LineLeft.m_a;
		m_MSAlineLeft.m_b = LineLeft.m_b;
		m_MSAlineLeft.m_c = LineLeft.m_c;
		m_MSAlineRight.m_a = LineRight.m_a;
		m_MSAlineRight.m_b = LineRight.m_b;
		m_MSAlineRight.m_c = LineRight.m_c;

		// up & left 의 교차점
        IPVM::Geometry::GetCrossPoint_LNvLN(LineUp, LineLeft, fPosUpLeft);
        
		// up & right 의 교차점
        IPVM::Geometry::GetCrossPoint_LNvLN(LineUp, LineRight, fPosUpRight);
        
		// 교차점 기준 angle
        fPosLeftExtand.m_x = fPosUpRight.m_x;
        fPosLeftExtand.m_y = fPosUpLeft.m_y;

        double fDegreeLeft = 0.f;
        IPVM::Geometry::GetContainedAngle_deg(fPosUpRight, fPosUpLeft, fPosLeftExtand, fDegreeLeft);
		
		IPVM::Point_32f_C2 fPosUpLeft_32, fPosUpRight_32, fPosLeftExtend_32;
        fPosUpLeft_32.m_x = fPosUpLeft.m_x;
        fPosUpLeft_32.m_y = fPosUpLeft.m_y;
        
        fPosUpRight_32.m_x = fPosUpRight.m_x;
        fPosUpRight_32.m_y = fPosUpRight.m_y;
        
		fPosLeftExtend_32.m_x = fPosLeftExtand.m_x;
        fPosLeftExtend_32.m_y = fPosLeftExtand.m_y;

		m_ptMSALeft = fPosUpLeft_32;
        m_ptMSARight = fPosUpRight_32;
        m_ptMSALeftExpand = fPosLeftExtend_32;

		auto& measure = *((CInspectionMeasure*)m_parent->GetProcessingItem(ProcItemCategory::measure));
        if (measure.m_para.m_imageUseIndex == 1)
            inspectionImage = m_inspectionImageDark;

        IPVM::Image_8u_C1 dstImage;
        dstImage.Create(inspectionImage.GetSizeX(), inspectionImage.GetSizeY());
        dstImage.FillZero();
	
		// Image Rotate
        if (fPosUpLeft_32.m_y > fPosUpRight_32.m_y)
        {
            IPVM::ImageProcessing::Rotate_Linear(inspectionImage, fPosUpRight_32, fDegreeLeft, IPVM::Point_32f_C2(0, 0), IPVM::Rect(inspectionImage), dstImage);
		}
        else if (fPosUpLeft_32.m_y < fPosUpRight_32.m_y)
        {
            IPVM::ImageProcessing::Rotate_Linear(inspectionImage, fPosUpRight_32, -fDegreeLeft, IPVM::Point_32f_C2(0, 0), IPVM::Rect(inspectionImage), dstImage);
		}
		
		memcpy(inspectionImage.GetRawMem(), dstImage.GetRawMem(),
                inspectionImage.GetSizeX() * inspectionImage.GetSizeY());
        dstImage.Free();
	}
}


bool CInspectionMeasure::ProcInspection(Inspection::DebugInfo* debugInfo)
{
#ifdef RESET_IMAGE_BUFFER_WHEN_MODULE_INSPECTION
	ResetImageBuffer(false);
#endif
	//auto& configuration = *((CInspectionConfiguration *)m_parent->GetProcessingItem(ProcItemCategory::configuration));
	auto& align = *((CInspectionCellAlign *)m_parent->GetProcessingItem(ProcItemCategory::cellAlign));
	/*auto& shoulder = *((CInspectionShoulderLine *)m_parent->GetProcessingItem(ProcItemCategory::shoulder));
	auto& tab = *((CInspectionTabFolding *)m_parent->GetProcessingItem(ProcItemCategory::tab));*/
	auto& criteriaList = Inspection::Accessor::Get().m_criteriaList;



	if (m_para.m_bUseMSAAlign == TRUE)
    {
        AlignMSA();
	}

	auto& inspectionImage = m_inspectionImageDark;

    if (m_para.m_nImageDark == 0) // Bright
    {
        inspectionImage = m_inspectionImageBright;
    }

	std::vector<IPVM::EdgeDetectionPara> m_vecedgePara;
	std::vector<IPVM::Rect> m_vecrtDetect;
	std::vector<long> m_vecDetecCount;
	std::vector<float> m_vecRansacThreshold;
	std::vector < IPVM::EdgeDetectionFilter> m_vecedgeFilter;

	IPVM::EdgeDetectionFilter m_edgeFilter;
	IPVM::AlgorithmObjectHandle<IPVM::EdgeDetection> m_edgeDetection;


	m_vecedgePara.resize(align.m_para.m_nLineCountMeasure);
	m_vecrtDetect.resize(align.m_para.m_nLineCountMeasure);
	m_vecedgeFilter.resize(align.m_para.m_nLineCountMeasure);
	//m_vecDetecCount.resize(align.m_para.m_nLineCount);
	m_vecRansacThreshold.resize(align.m_para.m_nLineCountMeasure);
	// 파라미터 초기화 여러개 사용 가능하도록....
	for (long cnt = 0; cnt < align.m_para.m_nLineCountMeasure; cnt++)
	{
		auto& Para = align.m_para.m_vecDetectLineParaMeasure[cnt];
		Para.SetParameter(m_vecedgePara[cnt], m_vecedgeFilter[cnt]);
		m_vecrtDetect[cnt] = align.m_para.m_vecrtDetectLineMeasure[cnt];
		//m_vecDetecCount[cnt] = align.m_para.m_vecDetectLinePara[cnt].m_DetectCount;
		m_vecRansacThreshold[cnt] = align.m_para.m_vecDetectLineParaMeasure[cnt].m_RansacThreshold;
	}

	//long countVertical = floor(inspectionImage.GetSizeY() * m_pixelToUmY / 1000 / m_para.m_fCellLengthCalcPosition);  // 이미지 몇 분할
	//if (countVertical < 1)
	//	countVertical = 1;
	long countVertical = m_para.m_fCellLengthCalcPosition;

	// 벡터 추가
	std::vector<std::vector<IPVM::Point_32f_C2>> pointsPerCycleAndLine;
	std::vector<IPVM::LineEq_64f> lineEq64_CalcData;
	
	lineEq64_CalcData.resize(align.m_para.m_nLineCountMeasure);
	pointsPerCycleAndLine.resize(countVertical);
	for (int i = 0; i < countVertical; ++i) {
		pointsPerCycleAndLine[i].resize(align.m_para.m_nLineCountMeasure);
		//lineEq64_CalcData[i].reserve(align.m_para.m_nLineCountMeasure);
	}

	// msa
    m_paraEdgeDetectMSA.m_vecedgePara.resize(2);
    m_paraEdgeDetectMSA.m_vecrtDetect.resize(2);
    m_paraEdgeDetectMSA.m_vecedgeFilter.resize(2);
    m_paraEdgeDetectMSA.m_vecDetecCount.resize(2);
    m_paraEdgeDetectMSA.m_vecRansacThreshold.resize(2); 


	// MSA - up
    {
        m_paraEdgeDetectMSA.m_vecedgePara[0].m_edgeThreshold = 1;
        m_paraEdgeDetectMSA.m_vecedgePara[0].m_edgeType = IPVM::EdgeType::Both;
        m_paraEdgeDetectMSA.m_vecedgePara[0].m_findFirstEdge = false;
        m_paraEdgeDetectMSA.m_vecedgePara[0].m_searchDirection = IPVM::SearchDirection::ToDown;
        m_paraEdgeDetectMSA.m_vecedgeFilter[0].SetParameter(6, 1.5);
        m_paraEdgeDetectMSA.m_vecrtDetect[0] = m_para.m_vecrtDetectLineMSA[0]; // rect
        m_paraEdgeDetectMSA.m_vecDetecCount[0] = 10; // long
        m_paraEdgeDetectMSA.m_vecRansacThreshold[0] = 10; // float

        auto& Para = m_para.m_vecDetectLineParaMSA[0];
        m_para.m_vecDetectLineParaMSA[0].SetParameter(
            m_paraEdgeDetectMSA.m_vecedgePara[0], m_paraEdgeDetectMSA.m_vecedgeFilter[0]);
    }
    // MSA - down
    {
        m_paraEdgeDetectMSA.m_vecedgePara[1].m_edgeThreshold = 1;
        m_paraEdgeDetectMSA.m_vecedgePara[1].m_edgeType = IPVM::EdgeType::Both;
        m_paraEdgeDetectMSA.m_vecedgePara[1].m_findFirstEdge = false;
        m_paraEdgeDetectMSA.m_vecedgePara[1].m_searchDirection = IPVM::SearchDirection::ToUp;
        m_paraEdgeDetectMSA.m_vecedgeFilter[1].SetParameter(6, 1.5);
        m_paraEdgeDetectMSA.m_vecrtDetect[1] = m_para.m_vecrtDetectLineMSA[1];
        m_paraEdgeDetectMSA.m_vecDetecCount[1] = 10;
        m_paraEdgeDetectMSA.m_vecRansacThreshold[1] = 10;

        auto& Para = m_para.m_vecDetectLineParaMSA[1];
        m_para.m_vecDetectLineParaMSA[1].SetParameter(
            m_paraEdgeDetectMSA.m_vecedgePara[1], m_paraEdgeDetectMSA.m_vecedgeFilter[1]);
    }
    // MSA
    m_para.m_vecPtMSA.resize(2);
    m_paraEdgeDetectMSA.m_vecedgePara[0].m_edgeType = IPVM::EdgeType::Both;
    m_paraEdgeDetectMSA.m_vecedgePara[1].m_edgeType = IPVM::EdgeType::Both;
    m_paraEdgeDetectMSA.m_vecedgePara[0].m_searchDirection = IPVM::SearchDirection::ToDown;
    m_paraEdgeDetectMSA.m_vecedgePara[1].m_searchDirection = IPVM::SearchDirection::ToUp;
    for (long nLine = 0; nLine < 2; nLine++)
    {
        IPVM::Point_32f_C3 pt3;
        std::vector<IPVM::Point_32f_C3> vecpt;
        IPVM::Point_32f_C2 ptHomeTop, ptHomeBot;
        IPVM::Rect rtHomeTop, rtHomeBot;
        CInspectionCellAlignEdgeDetectPara test;

        IPVM::Rect rtHomeSearchTop;
        rtHomeSearchTop = m_paraEdgeDetectMSA.m_vecrtDetect[nLine] & align.m_rectImageValid;

        long nLeft = rtHomeSearchTop.m_left;
        long nRight = rtHomeSearchTop.m_right;
        long nSize = nSize = (nRight - nLeft) / 20;
        vecpt.clear();

        for (long n = 0; n < m_paraEdgeDetectMSA.m_vecDetecCount[nLine]; n++)
        {
            IPVM::Rect rtSeartchTop = rtHomeSearchTop;
            rtSeartchTop.m_left = nLeft + nSize * n;
            rtSeartchTop.m_right = nLeft + nSize * (n + 1);

            if (m_paraEdgeDetectMSA.m_edgeDetection->DetectEdge(*((IPVM::Image_8u_C1*)&inspectionImage),
                    m_paraEdgeDetectMSA.m_vecedgePara[nLine], m_paraEdgeDetectMSA.m_vecedgeFilter[nLine], rtSeartchTop,
                    pt3)
                == IPVM::Status::success)
            {
                vecpt.push_back(pt3);
                m_para.m_vecPtMSA[nLine].m_x = pt3.m_x;
                m_para.m_vecPtMSA[nLine].m_y = pt3.m_y;

                IPVM::Point_32f_C2 pt2;
                pt2.m_x = pt3.m_x;
                pt2.m_y = pt3.m_y;
                debugInfo->AddOverlay(LanguageDef::Debug_InitialLinePointMSA, pt2, RGB(255, 0, 0));
            }
        }
    }


	for (long nLine = 0; nLine < align.m_para.m_nLineCountMeasure; nLine++)
	{
		IPVM::Point_32f_C3 pt3;
		IPVM::Point_32f_C2 ptHomeTop, ptHomeBot;
		IPVM::Rect rtHomeTop, rtHomeBot;
		IPVM::Rect rtHomeSearchTop;
		rtHomeSearchTop = m_vecrtDetect[nLine]; // &m_rectImageValid;

		IPVM::Rect temp = IPVM::Rect(inspectionImage);
		
        //long nTop = rtHomeSearchTop.m_top;
        //long nBottom = rtHomeSearchTop.m_bottom;

		long nTop = temp.m_top;
        long nBottom = temp.m_bottom;

        if (m_para.m_bUseMSA)
        {
            if (m_para.m_vecPtMSA[0].m_y > 0)
            {
                nTop = m_para.m_vecPtMSA[0].m_y;
            }
            if (m_para.m_vecPtMSA[1].m_y > 0)
            {
                nBottom = m_para.m_vecPtMSA[1].m_y;
            }
		}        
		
		long nSize = (nBottom - nTop) / countVertical;

		for (long n = 0; n < countVertical; n++)
		{
			IPVM::Rect rtSeartchTop = rtHomeSearchTop;

			rtSeartchTop.m_top = nTop + nSize * n;
			rtSeartchTop.m_bottom = nTop + nSize * (n + 1);

			IPVM::Point_32f_C2 pt2;
			if (m_edgeDetection->DetectEdge(*((IPVM::Image_8u_C1*)&inspectionImage),
				m_vecedgePara[nLine],
				m_vecedgeFilter[nLine],
				rtSeartchTop,
				pt3) == IPVM::Status::success)
			{
				pt2.m_x = pt3.m_x;
				pt2.m_y = pt3.m_y;
			}
			pointsPerCycleAndLine[n][nLine] = (pt2);
			//m_debugInfo.m_vecrtBoundrySearch.push_back(rtSeartchTop);


			CString text;
			text.Format(_T("Cycle %02d, Edge %02d"), n, nLine);

			//m_debugInfo.m_vecVhomePoint.push_back(pt2);
 			//debugInfo->AddOverlay(LanguageDef::Debug_Coater_EdgeDetectPoint, pt2, RGB(255, 0, 0));
		}

		//lineEq64_CalcData
		//if (m_para.m_nMSADistance <2) // Start
		{

			std::vector<IPVM::Point_64f_C2> ptUsed;
			std::vector<IPVM::Point_64f_C2> ptReal;

			long usedNum;
            //float distanceThreshold = align.m_para.m_paraEdgeDetectMSA[0].m_RansacThreshold;
            float distanceThreshold = m_paraEdgeDetectMSA.m_vecRansacThreshold[0];
			

		
			IPVM::Status status = IPVM::Status::unknownError;
			for (long n = 0; n < countVertical; n++)
			{
				IPVM::Point_64f_C2 pt;
				pt.m_x = pointsPerCycleAndLine[n][nLine].m_x;
				pt.m_y = pointsPerCycleAndLine[n][nLine].m_y;
				ptReal.push_back(pt);
			}
			ptUsed.resize(countVertical);
			if (ptReal.size() != 0)
			{
				status = IPVM::DataFitting::FitToLine_RANSAC(ptReal.size(), &ptReal[0], &ptUsed[0], usedNum, lineEq64_CalcData[nLine], distanceThreshold);
				
				IPVM::LineSeg_32f line;

				//float fTemp = lineEq64_CalcData[nLine].GetPositionX(0);
				float fTemp = pointsPerCycleAndLine[0][nLine].m_x;
				float fTempy = pointsPerCycleAndLine[countVertical - 1][nLine].m_y;
				line.m_sy = lineEq64_CalcData[nLine].GetPositionY(fTemp);
				line.m_ey = fTempy;
				line.m_sx = fTemp;
				fTemp = lineEq64_CalcData[nLine].GetPositionX(fTempy);
				line.m_ex = fTemp;
				debugInfo->AddOverlay(LanguageDef::Debug_Coater_EdgeDetectPoint, line, RGB(255, 0, 0));

				for (long ver = 0; ver < countVertical; ver++)
				{
					CString text;
					text.Format(_T("Cycle %02d, Edge %02d"), ver, nLine);
					//debugInfo->AddOverlay(LanguageDef::Debug_Coater_EdgeDetectPoint, pt2, RGB(255, 0, 0));
				}
				

				//m_debugInfo.m_vecVhomePoint.push_back(pt2);
				
			}
		}

	}

	// Measure start
	CISICellAllDefect& Defect = m_parent->m_ISICellAllDefect;
	Defect.m_vvMeasureDefect.clear();
	Defect.m_vvMeasureDefect.resize(countVertical);

	
	m_vecMSALength.clear();
	m_vecMSALength.resize(countVertical);
	for (int i = 0; i < countVertical; i++)
	{
		m_vecMSALength[i].resize(m_para.m_nCountMeasureItem);
	}

	m_vecMSAType.clear();
	m_vecMSAType.resize(m_para.m_nCountMeasureItem);


	for (long ver = 0; ver < countVertical; ver++)
	{
		float xFirst = pointsPerCycleAndLine[ver][0].m_x;
		float xLast = pointsPerCycleAndLine[ver][align.m_para.m_nLineCountMeasure - 1].m_x;
		float halfX = (xLast - xFirst) / 2;

		IPVM::Point_32f_C2 pt2;
		pt2.m_x = halfX;
		pt2.m_y = pointsPerCycleAndLine[ver][align.m_para.m_nLineCountMeasure / 2].m_y;
		//pt2.m_y = pointsPerCycleAndLine[ver][m_para.m_nCountMeasureItem / 2].m_y;
		CString text;
		text.Format(_T("Cycle %02d, Sheet Half"), ver);
		//debugInfo->AddOverlayAndText(LanguageDef::Debug_Coater_EdgeDetectPoint, pt2, text);

		//std::vector <CIDISpecInfo> vecMeasureDefect = Defect.m_vvMeasureDefect[ver];
		//vecMeasureDefect.reserve(m_para.m_nCountMeasureItem);
		std::vector <CIDISpecInfo>* pvecMeasureDefect = &Defect.m_vvMeasureDefect[ver];
		pvecMeasureDefect->reserve(m_para.m_nCountMeasureItem);
		for (long meas = 0; meas < m_para.m_nCountMeasureItem; meas++)
		{
			// end - start 값 계산
			IPVM::Point_32f_C2 start(-1.,-1);
			IPVM::Point_32f_C2 end(-1., -1);
			double length;

			// (count) is none, (count+1) is half(start, end)
			if (m_para.m_vecMeasureStart[meas] == align.m_para.m_nLineCountMeasure  ||
				m_para.m_vecMeasureEnd[meas] == align.m_para.m_nLineCountMeasure	||
				m_para.m_vecMeasureStart[meas] > align.m_para.m_nLineCountMeasure	||
				m_para.m_vecMeasureEnd[meas] > align.m_para.m_nLineCountMeasure		)
			{
				length = -1.;
			}
            else 
			{
				if (m_para.m_bUseMSA == false || (m_para.m_bUseMSA && m_para.m_nMSADistance == 0)) // None
				{
					start = pt2;
					end = pt2;

					if (m_para.m_vecMeasureStart[meas] != align.m_para.m_nLineCountMeasure + 1)
					{
						start = pointsPerCycleAndLine[ver][m_para.m_vecMeasureStart[meas]];
					}
					if (m_para.m_vecMeasureEnd[meas] != align.m_para.m_nLineCountMeasure + 1)
					{
						end = pointsPerCycleAndLine[ver][m_para.m_vecMeasureEnd[meas]];
					}

					m_para.m_vecMeasureStartPosX[meas] = start.m_x;
					m_para.m_vecMeasureEndPosX[meas] = end.m_x;
					length = end.m_x - start.m_x;
				}
                else if (m_para.m_bUseMSA && m_para.m_nMSADistance == 1) // Start
				{
					start = pt2;
					end = pt2;
					if (m_para.m_vecMeasureStart[meas] != align.m_para.m_nLineCountMeasure + 1)
					{
						start = pointsPerCycleAndLine[ver][m_para.m_vecMeasureStart[meas]];
					}
					if (m_para.m_vecMeasureEnd[meas] != align.m_para.m_nLineCountMeasure + 1)
					{
						end = pointsPerCycleAndLine[ver][m_para.m_vecMeasureEnd[meas]];
					}

					m_para.m_vecMeasureStartPosX[meas] = start.m_x;
					m_para.m_vecMeasureEndPosX[meas] = end.m_x;
					IPVM::Geometry::GetDistance(lineEq64_CalcData[m_para.m_vecMeasureStart[meas]], end, length);
				}
                else if (m_para.m_bUseMSA && m_para.m_nMSADistance == 2) // End
				{
					start = pt2;
					end = pt2;
					if (m_para.m_vecMeasureStart[meas] != align.m_para.m_nLineCountMeasure + 1)
					{
						start = pointsPerCycleAndLine[ver][m_para.m_vecMeasureStart[meas]];
					}
					if (m_para.m_vecMeasureEnd[meas] != align.m_para.m_nLineCountMeasure + 1)
					{
						end = pointsPerCycleAndLine[ver][m_para.m_vecMeasureEnd[meas]];
					}

					m_para.m_vecMeasureStartPosX[meas] = start.m_x;
					m_para.m_vecMeasureEndPosX[meas] = end.m_x;
					IPVM::Geometry::GetDistance(lineEq64_CalcData[m_para.m_vecMeasureEnd[meas]], start,  length);
				}	
			}

			CString text;
			CString MSAtxt;
			LanguageDef langDef;

            switch (m_para.m_vecMeasureType[meas])
            {
                case 0:
                    text.Format(_T("[mm] %d A Full Length"), ver);
                    debugInfo->AddText(LanguageDef::Debug_Coater_A_FullLength, length * m_pixelToUmX / 1000.f, text);
                    break;
                case 1:
                    text.Format(_T("[mm] %d B Coating Width 1"), ver);
                    MSAtxt.Format(_T("Left Coating"));
                    debugInfo->AddText(LanguageDef::Debug_Coater_B_LeftCoating, length * m_pixelToUmX / 1000.f, text);
                    break;
                case 2:
                    text.Format(_T("[mm] %d C Coating Width 2"), ver);
                    MSAtxt.Format(_T("Right Coating"));
                    debugInfo->AddText(LanguageDef::Debug_Coater_C_RightCoating, length * m_pixelToUmX / 1000.f, text);
                    break;
                case 3:
                    text.Format(_T("[mm] %d D Non-Coating Width 1"), ver);
                    MSAtxt.Format(_T("Left Foil"));
                    debugInfo->AddText(LanguageDef::Debug_Coater_D_LeftFoil, length * m_pixelToUmX / 1000.f, text);
                    break;
                case 4:
                    text.Format(_T("[mm] %d E Middle Foil"), ver);
                    debugInfo->AddText(LanguageDef::Debug_Coater_E_MiddleFoil, length * m_pixelToUmX / 1000.f, text);
                    break;
                case 5:
                    text.Format(_T("[mm] %d E1 Middle Foil L"), ver);
                    debugInfo->AddText(
                        LanguageDef::Debug_Coater_E1_MiddleFoilLeftSlitting, length * m_pixelToUmX / 1000.f, text);
                    break;
                case 6:
                    text.Format(_T("[mm] %d E2 Middle Foil R"), ver);
                    debugInfo->AddText(
                        LanguageDef::Debug_Coater_E2_MiddleFoilRightSlitting, length * m_pixelToUmX / 1000.f, text);
                    break;
                case 7:
                    text.Format(_T("[mm] %d F Non-Coating Width 2"), ver);
                    MSAtxt.Format(_T("Right Foil"));
                    debugInfo->AddText(LanguageDef::Debug_Coater_F_RightFoil, length * m_pixelToUmX / 1000.f, text);
                    break;
                default:;
            }
			
			//std::vector < CIDISpecInfo>& vecMeasureDefect = Defect.m_vvMeasureDefect[ver];
			DefectType defectType = (DefectType)(m_para.m_vecMeasureType[meas] + ((long)DefectType::START_Coater_Dimension + 1));
			CIDISpecInfo idiSpecInfo(defectType, m_para.m_vecMeasureType[meas]);
			idiSpecInfo.m_nMeasureType = m_para.m_vecMeasureType[meas];
			idiSpecInfo.m_nMeasureStart = m_para.m_vecMeasureStart[meas];
			idiSpecInfo.m_fMeasureStartPosX = m_para.m_vecMeasureStartPosX[meas];
			idiSpecInfo.m_nMeasureEnd = m_para.m_vecMeasureEnd[meas];
			idiSpecInfo.m_fMeasureEndPosX = m_para.m_vecMeasureEndPosX[meas];
			idiSpecInfo.m_fMeasureStandard = m_para.m_vecMeasureStandard[meas];
			idiSpecInfo.m_fMeasureOffsetUCL = m_para.m_vecMeasureOffsetUCL[meas];
			idiSpecInfo.m_fMeasureOffsetLCL = m_para.m_vecMeasureOffsetLCL[meas];
			idiSpecInfo.m_fMeasureOffsetUSL = m_para.m_vecMeasureOffsetUSL[meas];
			idiSpecInfo.m_fMeasureOffsetLSL = m_para.m_vecMeasureOffsetLSL[meas];
			idiSpecInfo.LENGTH = length* m_pixelToUmX/1000.f;

			m_vecMSALength[ver][meas] = length * m_pixelToUmX / 1000.f;
			m_vecMSAType[meas] = MSAtxt;

			// 허용치 벗어나면 defect
			std::vector< Base::ParaFloat> m_vecMeasureStandard;
			std::vector< Base::ParaFloat> m_vecMeasureMax;
			std::vector< Base::ParaFloat> m_vecMeasureMin;
			float usl = m_para.m_vecMeasureStandard[meas] + m_para.m_vecMeasureOffsetUSL[meas];
			float lsl = m_para.m_vecMeasureStandard[meas] + m_para.m_vecMeasureOffsetLSL[meas];
			
			IPVM::Rect rtLine;
			rtLine.m_left = min(start.m_x, end.m_x);
			rtLine.m_right = max(start.m_x, end.m_x);
			rtLine.m_top = min(start.m_y, end.m_y);
			rtLine.m_bottom = max(start.m_y, end.m_y);

			if (length > usl || length < lsl)
			{
				DefectType dType;
				dType = (DefectType)((long)DefectType::START_Coater_Dimension + 1 + m_para.m_vecMeasureType[meas]);
				auto defectName = GetDefectName(dType);
				m_rejectInfo.Add(rtLine, m_measureTabHeight.GetValue_mm(), defectName);

				debugInfo->AddOverlay(LanguageDef::Result, rtLine, RGB(0, 255, 0));
				idiSpecInfo.SetJudge(II_RESULT_VALUE::REJECT);
			}
			else
			{
				debugInfo->AddOverlay(LanguageDef::Result, rtLine, RGB(0, 255, 0));
				idiSpecInfo.SetJudge(II_RESULT_VALUE::PASS);
			}
			//vecMeasureDefect.push_back(idiSpecInfo);
			pvecMeasureDefect->push_back(idiSpecInfo);


		}
		/*CString strLength;
		strLength.Format(_T(", %d, %f, %f, %f, %f,"), ver, m_vecMSALength[ver][0], m_vecMSALength[ver][1], m_vecMSALength[ver][2], m_vecMSALength[ver][3]);
		m_loggerResultMSA.Log(0, strLength);*/

		
	}
	//m_loggerResultMSA.Log(0, _T("--------------------------------"));
	//debugInfo->AddOverlay(LanguageDef::Result, rtCell, RGB(0, 200, 0));
	//debugInfo->AddOverlay(LanguageDef::Result, rtRighTab, RGB(200, 200, 0));
	//debugInfo->AddOverlay(LanguageDef::Result, rtLeftTab, RGB(200, 200, 0));

	//m_unionResult.m_rectCell = rtCell;
	//m_unionResult.m_rectTab_Right = rtRighTab;
	//m_unionResult.m_rectTab_Left = rtLeftTab;

#if INSPECTION_MACHINE_UC1
	//if (m_rejectInfo.Count() > 0)
    {
        //m_resultValue = II_RESULT_VALUE::REJECT;
    } //else
    {
        m_resultValue = II_RESULT_VALUE::PASS;
    }
    return true;
#else
    if (m_rejectInfo.Count() > 0)
    {
        m_resultValue = II_RESULT_VALUE::REJECT;
    }
    else
    {
        m_resultValue = II_RESULT_VALUE::PASS;
    }
    return true;
#endif


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//return true;
}

void CInspectionMeasure::DrawMeasurePoints(Inspection::DebugInfo* debugInfo, stMeasurementDraw& Data, LanguageDef Def, bool bPointHead, int inputsize)
{
	debugInfo->DrawArrow(Def, Data.m_crosspointLeft, 0, inputsize, bPointHead, RGB(255, 200, 0), 1);
	debugInfo->DrawArrow(Def, Data.m_crosspointRight, 180, inputsize, bPointHead, RGB(255, 200, 0), 1);
	debugInfo->AddOverlay(Def, Data.m_lineSegLeft, RGB(255, 200, 0));
	debugInfo->AddOverlay(Def, Data.m_lineSegRight, RGB(255, 200, 0));
}

void CInspectionMeasure::ProcInspectionEnd(Inspection::DebugInfo* debugInfo)
{
	if (FALSE == m_parent->m_isTest) return;

	/*debugInfo->DrawArrow(LanguageDef::Debug_CellLength, Debug_Length_crosspoint1, 0, 100, 200, RGB(255, 0, 0), 1);
	debugInfo->DrawArrow(LanguageDef::Debug_CellLength, Debug_Length_crosspoint2, 180, 100, 200, RGB(255, 0, 0), 1);
	debugInfo->AddOverlay(LanguageDef::Debug_CellLength, Debug_Length_lineSegLeft);
	debugInfo->AddOverlay(LanguageDef::Debug_CellLength, Debug_Length_lineSegRight);*/
	DrawMeasurePoints(debugInfo, m_Length, LanguageDef::Debug_CellLength, true, 100);
	DrawMeasurePoints(debugInfo, m_Length2, LanguageDef::Debug_CellLength2, true, 100);
	debugInfo->AddText(LanguageDef::Debug_CellLength, m_measureCellLengthBot.GetValue_um(),		_T("[um] Left Length"));
	debugInfo->AddText(LanguageDef::Debug_CellLength, m_fCellLengthBotErrorUm,					_T("[um] Left Length Error"));
	debugInfo->AddText(LanguageDef::Debug_CellLength, m_measureCellLengthTop.GetValue_um(),		_T("[um] Right Length"));
	debugInfo->AddText(LanguageDef::Debug_CellLength, m_fCellLengthTopErrorUm,					_T("[um] Right Length Error"));
	debugInfo->AddText(LanguageDef::Debug_CellLength2, m_measureCellLength2_Bot.GetValue_um(),	_T("[um] Left Length 2"));
	debugInfo->AddText(LanguageDef::Debug_CellLength2, m_measureCellLength2_Top.GetValue_um(),  _T("[um] Right Length 2"));

	DrawMeasurePoints(debugInfo, m_Height, LanguageDef::Debug_TabHeight, true, 100);
	DrawMeasurePoints(debugInfo, m_Height2, LanguageDef::Debug_TabHeight2, true, 100);
	debugInfo->AddText(LanguageDef::Debug_TabHeight, m_measureTabHeight.GetValue_um(), _T("[um] Tab Height"));
	debugInfo->AddText(LanguageDef::Debug_TabHeight, m_fTabHeightErrorUm, _T("[um] Tab Height Error"));
	debugInfo->AddText(LanguageDef::Debug_TabHeight2, m_measureTabHeight2.GetValue_um(), _T("[um] Tab Height 2"));

	debugInfo->AddOverlayAndText(LanguageDef::Debug_LinePoint, m_unionResult.m_ptVHome[0], _T("Left Top"));
	debugInfo->AddOverlayAndText(LanguageDef::Debug_LinePoint, m_unionResult.m_ptVHome[1], _T("Left Bottom"));
	debugInfo->AddOverlayAndText(LanguageDef::Debug_LinePoint, m_unionResult.m_ptVHome[2], _T("Right Top"));
	debugInfo->AddOverlayAndText(LanguageDef::Debug_LinePoint, m_unionResult.m_ptVHome[3], _T("Right Bottom"));

	debugInfo->AddOverlayAndText(LanguageDef::Debug_VHomeTiltPosition, m_unionResult.m_ptVHomeInner[II_CORNER_VALUE::II_LEFT_TOP], _T("Left Inner"));
	debugInfo->AddOverlayAndText(LanguageDef::Debug_VHomeTiltPosition, m_unionResult.m_ptVHomeOuter[II_CORNER_VALUE::II_LEFT_TOP], _T("Left Outer"));
	debugInfo->AddOverlayAndText(LanguageDef::Debug_VHomeTiltPosition, m_unionResult.m_ptVHomeInner[II_CORNER_VALUE::II_RIGHT_TOP], _T("Right Inner"));
	debugInfo->AddOverlayAndText(LanguageDef::Debug_VHomeTiltPosition, m_unionResult.m_ptVHomeOuter[II_CORNER_VALUE::II_RIGHT_TOP], _T("Right Outer"));

	debugInfo->AddOverlayAndText(LanguageDef::Debug_TabAreaForHeight, m_unionResult.m_rectTabForHeight);

	debugInfo->AddText(LanguageDef::Debug_CellPitch,	m_measureTabPitch.GetValue_um(),		_T("[um] Pitch"));
	debugInfo->AddText(LanguageDef::Debug_CellPitch,	m_fCellPitchErrorUm,					_T("[um] Pitch Error"));
	debugInfo->AddText(LanguageDef::Debug_TabWidth,		m_measureTabWidth.GetValue_um(),		_T("[um] Tab Width"));
	debugInfo->AddText(LanguageDef::Debug_TabWidth,		m_fTabWidthErrorUm,						_T("[um] Tab Width Error"));

	debugInfo->AddText(LanguageDef::Debug_Misalign,		m_measureMisalign.GetValue_um(),		_T("[um] Misalign"));
	debugInfo->AddText(LanguageDef::Debug_Misalign,		m_fMisalignErrorUm,						_T("[um] Misalign Error"));
	debugInfo->AddText(LanguageDef::Debug_Misalign,		m_fMisalignDetail,						_T("[um] Misalign Detail"));
	debugInfo->AddText(LanguageDef::Debug_VHomeTilt,	m_measureVHomeTiltBot.GetValue_um(),	_T("[um] Tilt Left"));
	debugInfo->AddText(LanguageDef::Debug_VHomeTilt,	m_fVhomeTiltBotErrorUm,					_T("[um] Tilt Left Error"));
	debugInfo->AddText(LanguageDef::Debug_VHomeTilt,	m_measureVHomeTiltTop.GetValue_um(),	_T("[um] Tilt Right"));
	debugInfo->AddText(LanguageDef::Debug_VHomeTilt,	m_fVhomeTiltTopErrorUm,					_T("[um] Tilt Right Error"));
	debugInfo->AddText(LanguageDef::Debug_TabSide,		m_fTabSideErrorUm,						_T("[um] Tab Side Error"));

	debugInfo->AddText		(LanguageDef::Debug_CuttingHeightTop, m_measureCuttingHeightTop.GetValue_um(), _T("[um] Cutting Height Top"));
	DrawMeasurePoints(debugInfo, m_CuttingHeightTop, LanguageDef::Debug_CuttingHeightTop, true, 15);

	debugInfo->AddText		(LanguageDef::Debug_CuttingHeightBot, m_measureCuttingHeightBot.GetValue_um(), _T("[um] Cutting Height Bot"));
	DrawMeasurePoints(debugInfo, m_CuttingHeightBot, LanguageDef::Debug_CuttingHeightBot, true, 15);

	debugInfo->AddText		(LanguageDef::Debug_VHomeHeight, m_rectVhomeLeftTop.Width() * m_pixelToUmX, _T("[um] V Home Height Left"));
	debugInfo->AddText		(LanguageDef::Debug_VHomeHeight, m_rectVhomeRightTop.Width() * m_pixelToUmX, _T("[um] V Home Height Right"));
	DrawMeasurePoints(debugInfo, m_VhomeHeightTop, LanguageDef::Debug_VHomeHeight, true, 10);
	DrawMeasurePoints(debugInfo, m_VhomeHeightBot, LanguageDef::Debug_VHomeHeight, true, 10);
}

void CInspectionMeasure::LinkDataBase(bool bSave, Database &dbJob)
{
	m_para.LinkDataBase(bSave, m_isUp, dbJob);
}

bool CInspectionMeasure::CopyDataBase(CProcessingBase* src)
{
	auto *ptr = dynamic_cast<CInspectionMeasure *>(src);
	if (ptr == nullptr)
	{
		ASSERT(!_T("���� Ÿ���� Inspection�� ���簡 �����ϴ�"));
		return false;
	}

	auto& rhs = *ptr;

	m_pixelToUmX = rhs.m_pixelToUmX;
	m_pixelToUmY = rhs.m_pixelToUmY;

	m_para = rhs.m_para;

	m_isUp = rhs.m_isUp;

	return true;
}

void CInspectionMeasure::SetMeasureUIDatas(const IPVM::Rect& InspROI, stMeasurementDraw& Datas)
{
	IPVM::Point_32f_C2 crosspointLeft;
	IPVM::Point_32f_C2 crosspointRIght;
	IPVM::LineSeg_32f lineSegLeft;
	IPVM::LineSeg_32f lineSegRight;

	crosspointLeft = IPVM::Point_32f_C2(InspROI.m_left, InspROI.CenterPoint().m_y);
	crosspointRIght = IPVM::Point_32f_C2(InspROI.m_right, InspROI.CenterPoint().m_y);
	lineSegLeft = IPVM::LineSeg_32f(InspROI.m_left, InspROI.m_top, InspROI.m_left, InspROI.m_bottom);
	lineSegRight = IPVM::LineSeg_32f(InspROI.m_right, InspROI.m_top, InspROI.m_right, InspROI.m_bottom);

	Datas.SetLines(lineSegLeft, lineSegRight);
	Datas.SetPoints(crosspointRIght,crosspointLeft);
}

