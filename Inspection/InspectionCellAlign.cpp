#include "stdafx.h"
#include "InspectionCellAlign.h"
#include "InspectionCell.h"
//#include "InspectionShoulderLine.h"
#include "Accessor.h"
#include "DebugInfo.h"
#include "UnionCellResult.h"

#include "../CompanySharedLibrary/Persistence/Database.h"
//#include "../IBCI/SystemParameter.h"

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

#include "Gadget/TimeCheck.h"

#include "InspectionCell.h"

#include <cmath>
#include <algorithm>
#include <numeric>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CInspectionCellAlign::CInspectionCellAlign(CInspectionCell* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
	: CProcessingBase(parent, ProcItemCategory::cellAlign, logger, loggerResult)
	, m_parent(parent)
{
	//m_isNoTab = false;
}

CInspectionCellAlign::~CInspectionCellAlign()
{
}

void CInspectionCellAlign::PrepareDebugInfo(Inspection::DebugInfo* debugInfo)
{
	//debugInfo->AddItem(LanguageDef::Debug_InitialLinePoint);
	//debugInfo->AddItem(LanguageDef::Debug_LinePoint);
	//debugInfo->AddItem(LanguageDef::Debug_CellBoundaryLine);
	//debugInfo->AddItem(LanguageDef::Debug_CellBoundaryROI);
	
	debugInfo->AddItem(LanguageDef::Debug_InitialLinePointMeasure);
	debugInfo->AddItem(LanguageDef::Debug_InitialLinePointBlob);
	debugInfo->AddItem(LanguageDef::Debug_LinePointMeasure);
	debugInfo->AddItem(LanguageDef::Debug_LinePointBlob);
	debugInfo->AddItem(LanguageDef::Debug_CellBoundaryLine);
	debugInfo->AddItem(LanguageDef::Debug_CellBoundaryROIMeasure);
	debugInfo->AddItem(LanguageDef::Debug_CellBoundaryROIBlob);
}
void CInspectionCellAlign::AutoFindEdgeDetectROI2()
{

    auto& imgSource = m_inspectionImageDark; // 선택 가능 하도록 변 경 가능성 있음

    if (m_para.m_nSelectImageType == 0) // Bright
    {
        imgSource = m_inspectionImageBright;
    }
 

    long imageSizeX = imgSource.GetSizeX();
    long imageSizeY = imgSource.GetSizeY();

    if (imageSizeX == 0)
    {
        return;
    }

    // Step #1. Average Line Profile
    std::vector<BYTE> vecByAvg;
    vecByAvg.resize(imageSizeX);

    for (int nX = 0; nX < imageSizeX; nX++)
    {
        long nSum = 0;
        for (int nY = imageSizeY / 2 - 50; nY < imageSizeY / 2 + 50; nY++)
        {
            BYTE* pbyCheckVal = imgSource.GetMem(nX, nY);
            nSum += *pbyCheckVal;
        }
        vecByAvg[nX] = nSum / 100;
        /*CString str;
        str.Format(_T("%d : %d \n"), nX, vecByAvg[nX]);
        TRACE(str);*/
    }

    // Step #2. Find Edge Line
    std::vector<IPVM::Point_32f_C2> vecPtCheckingPoint;
    vecPtCheckingPoint.resize(0);
    BYTE byStartValue;
    int nStartValue;
    IPVM::Point_32f_C2 ptCheck;
    int nForStart = 500;
    int nContinueCnt = 0;
    int nTabDiffPx = 893;
    nStartValue = nForStart;
    for (int nX = nForStart; nX < imageSizeX - nForStart, vecPtCheckingPoint.size() < 1; nX++)
    {
        if (abs(vecByAvg[nStartValue] - vecByAvg[nX]) > 100)
        {
            // 탭간 유의차일 경우
            if (abs(nStartValue - nX) > nTabDiffPx - 10 && abs(nStartValue - nX) < nTabDiffPx + 10)
                continue;

            // 1. average 값을 이용하면 좋을 듯

            // 2. 앞뒤 기준으로 연속된 값(차이가 얼마 나지 않는 부분을 찾아서 continue)
            //&& (nX - nStartValue < 100)
            vector<IPVM::Point_32f_C2> vecPtTempChecking;
            IPVM::LineEq_32f lineCheckingPos;
            long nCntCompareData = 3;

            for (int cnt = -2; cnt < nCntCompareData; cnt++)
            {
                IPVM::Point_32f_C2 ptTemp;
                ptTemp.m_x = cnt + 2;
                ptTemp.m_y = vecByAvg[nX + cnt];
                vecPtTempChecking.push_back(ptTemp);
            }

            if (IPVM::DataFitting::FitToLine(vecPtTempChecking.size(), &vecPtTempChecking[0], lineCheckingPos)
                != IPVM::Status::success)
            {
                // fittoline failed
            }

            float fSlope = abs(lineCheckingPos.m_a / lineCheckingPos.m_b);
            if (fSlope < 0.2)
            {
                nContinueCnt++;

                if (nContinueCnt > 50)
                {
                    nStartValue = nX;
                }
                continue;
            }

            //if (nX - nStartValue < 100)
            //continue;

            ptCheck.m_x = nX;
            ptCheck.m_y = imageSizeY / 2;
            vecPtCheckingPoint.push_back(ptCheck);
            nStartValue = nX;
        }
    }
    nStartValue = imageSizeX - nForStart;
    for (int nX = imageSizeX - nForStart; nX < nForStart, vecPtCheckingPoint.size() < 2; nX--)
    {
        if (abs(vecByAvg[nStartValue] - vecByAvg[nX]) > 100)
        {
            // 탭간 유의차일 경우
            if (abs(nStartValue - nX) > nTabDiffPx - 10 && abs(nStartValue - nX) < nTabDiffPx + 10)
                continue;

            // 1. average 값을 이용하면 좋을 듯

            // 2. 앞뒤 기준으로 연속된 값(차이가 얼마 나지 않는 부분을 찾아서 continue)
            //&& (nX - nStartValue < 100)
            vector<IPVM::Point_32f_C2> vecPtTempChecking;
            IPVM::LineEq_32f lineCheckingPos;
            long nCntCompareData = 3;

            for (int cnt = -2; cnt < nCntCompareData; cnt++)
            {
                IPVM::Point_32f_C2 ptTemp;
                ptTemp.m_x = cnt + 2;
                ptTemp.m_y = vecByAvg[nX + cnt];
                vecPtTempChecking.push_back(ptTemp);
            }

            if (IPVM::DataFitting::FitToLine(vecPtTempChecking.size(), &vecPtTempChecking[0], lineCheckingPos)
                != IPVM::Status::success)
            {
                // fittoline failed
            }

            float fSlope = abs(lineCheckingPos.m_a / lineCheckingPos.m_b);
            if (fSlope < 0.2)
            {
                nContinueCnt++;

                if (nContinueCnt > 50)
                {
                    nStartValue = nX;
                }
                continue;
            }

            //if (nX - nStartValue < 100)
            //continue;

            ptCheck.m_x = nX;
            ptCheck.m_y = imageSizeY / 2;
            vecPtCheckingPoint.push_back(ptCheck);
            nStartValue = nX;
        }
    }

    
    if (vecPtCheckingPoint.size() == 2)
    {
        vecPtCheckingPoint[0];
        vecPtCheckingPoint[1];
        IPVM::Point_32f_C2 temp;
       // if(m_para.m_vecrtDetectLineMeasure.size()==4)
        {
            long nX0 = m_paraEdgeDetectMeasure.m_vecrtDetect[0].CenterPoint().m_x;
            //long nX1 = m_paraEdgeDetectMeasure.m_vecrtDetect[1].CenterPoint().m_x;
           /* IPVM::Rect rt0 = m_paraEdgeDetectMeasure.m_vecrtDetect[0];
            IPVM::Rect rt1 = m_paraEdgeDetectMeasure.m_vecrtDetect[1];*/
            long nShift = vecPtCheckingPoint[0].m_x - nX0;
            
			
           // long nX2 = m_paraEdgeDetectMeasure.m_vecrtDetect[2].CenterPoint().m_x;
            long nX3 = m_paraEdgeDetectMeasure.m_vecrtDetect[3].CenterPoint().m_x;
            /*IPVM::Rect rt2 = m_paraEdgeDetectMeasure.m_vecrtDetect[2];
            IPVM::Rect rt3 = m_paraEdgeDetectMeasure.m_vecrtDetect[3];*/
            long nShift2 = vecPtCheckingPoint[1].m_x - nX3;

			m_paraEdgeDetectMeasure.m_vecrtDetect[0].InflateRect(-nShift, 0, +nShift, 0);
            m_paraEdgeDetectMeasure.m_vecrtDetect[1].InflateRect(-nShift, 0, +nShift, 0);
            m_paraEdgeDetectMeasure.m_vecrtDetect[2].InflateRect(-nShift2, 0, +nShift2, 0);
            m_paraEdgeDetectMeasure.m_vecrtDetect[3].InflateRect(-nShift2, 0, +nShift2, 0);
            m_para.m_vecrtDetectLineMeasure[0].InflateRect(-nShift, 0, +nShift, 0);
            m_para.m_vecrtDetectLineMeasure[1].InflateRect(-nShift, 0, +nShift, 0);
            m_para.m_vecrtDetectLineMeasure[2].InflateRect(-nShift2, 0, +nShift2, 0);
            m_para.m_vecrtDetectLineMeasure[3].InflateRect(-nShift2, 0, +nShift2, 0);

        }
        //if (m_para.m_vecrtDetectLineBlob.size() == 4)
        {
            long nX0 = m_paraEdgeDetectBlob.m_vecrtDetect[0].CenterPoint().m_x;
            //long nX1 = m_paraEdgeDetectBlob.m_vecrtDetect[1].CenterPoint().m_x;
            //IPVM::Rect rt0 = m_paraEdgeDetectBlob.m_vecrtDetect[0];
            //IPVM::Rect rt1 = m_paraEdgeDetectBlob.m_vecrtDetect[1];
            long nShift = vecPtCheckingPoint[0].m_x - nX0;


            //long nX2 = m_paraEdgeDetectBlob.m_vecrtDetect[2].CenterPoint().m_x;
            long nX3 = m_paraEdgeDetectBlob.m_vecrtDetect[3].CenterPoint().m_x;
           // IPVM::Rect rt2 = m_paraEdgeDetectBlob.m_vecrtDetect[2];
           // IPVM::Rect rt3 = m_paraEdgeDetectBlob.m_vecrtDetect[3];
            long nShift2 = vecPtCheckingPoint[1].m_x - nX3;

			m_paraEdgeDetectBlob.m_vecrtDetect[0].InflateRect(-nShift, 0, +nShift, 0);
            m_paraEdgeDetectBlob.m_vecrtDetect[1].InflateRect(-nShift, 0, +nShift, 0);
            m_paraEdgeDetectBlob.m_vecrtDetect[2].InflateRect(-nShift2, 0, +nShift2, 0);
            m_paraEdgeDetectBlob.m_vecrtDetect[3].InflateRect(-nShift2, 0, +nShift2, 0);
        }
      
    }

   
}
bool CInspectionCellAlign::ProcInspection(Inspection::DebugInfo* debugInfo)
{
	//m_imageEdgeDetect = m_tapeThreshold;

	//	debugInfo->SetText(LanguageDef::Result, _T("Cell Align : Pass"));

#ifdef RESET_IMAGE_BUFFER_WHEN_MODULE_INSPECTION
	ResetImageBuffer(false);
#endif

	//m_isNoTab = false;

	CString strDir;
	if (m_isUp)
	{
		strDir = _T("Up");
	}
	else
	{
		strDir = _T("Down");
	}

	float cellWidthPx = 0, offsetPx = 0.0f, widthPx = 0.0f, heightPx = 0.0f;
	//cellWidthPx = m_cellDesign.fCellWidth * 1000 / m_pixelToUmY;

	

	for (int i = 0; i < 4; i++)
	{
		m_unionResult.m_ptVHome[i] = IPVM::Point_32f_C2(0, 0);
		

		m_unionResult.m_ptDetialVhomeCenterPoint[i] = IPVM::Point_32f_C3(0, 0, 0);

		m_unionResult.m_ptVHomeInitial[i] = IPVM::Point_32f_C2(0, 0);
		m_unionResult.m_ptVHome[i] = IPVM::Point_32f_C2(0, 0);
		m_unionResult.m_ptDetialVHome[i] = IPVM::Point_32f_C2(0, 0);

		m_unionResult.m_ptVHomeOuter[i] = IPVM::Point_32f_C2(0, 0);
		m_unionResult.m_ptVHomeInner[i] = IPVM::Point_32f_C2(0, 0);
	}
	
	m_debugInfo.m_vecrtBoundrySearch.clear();
	
	m_unionResult.m_rectTab = IPVM::Rect_32f(0, 0, 0, 0);
	m_unionResult.m_rectShoulderTab = IPVM::Rect_32f(0, 0, 0, 0);
	m_unionResult.m_rectShoulderNoTab = IPVM::Rect_32f(0, 0, 0, 0);
	m_unionResult.m_lineCellBoundaryLeft = IPVM::LineSeg_32f(0, 0, 0, 0);
	m_unionResult.m_lineCellBoundaryRight = IPVM::LineSeg_32f(0, 0, 0, 0);
	m_unionResult.m_lineSegShoulder = IPVM::LineSeg_32f(0, 0, 0, 0);
	m_unionResult.m_lineCellBoundaryLeft4Shoulder = IPVM::LineSeg_32f(0, 0, 0, 0);
	m_unionResult.m_lineCellBoundaryRight4Shoulder = IPVM::LineSeg_32f(0, 0, 0, 0);

	m_unionResult.m_ptsShoulderOrig.clear();
	m_unionResult.m_ptsShoulderMargin.clear();

	m_unionResult.m_ptsCellBoundaryTop.clear();
	m_unionResult.m_ptsCellBoundaryBot.clear();
	m_unionResult.m_ptsCellBoundary.clear();

	m_unionResult.m_fMisalign_pixel = -9999;
	//m_unionResult.m_vecPtMSA.clear();
	m_debugInfo.m_vecSearch.clear();

    memset(m_unionResult.m_rectAreaMeasure, 0, sizeof(m_unionResult.m_rectAreaMeasure));
    memset(m_unionResult.m_rectAreaMeasure, 0, sizeof(m_unionResult.m_rectAreaBlob));

    //for (long nRectIndex = 0; nRectIndex < 21; nRectIndex++)
    //{
    //    m_unionResult.m_rectAreaMeasure.clear();
    //    m_unionResult.m_rectAreaBlob.clear();
    //}
	
	//ZeroMemory(m_DebugImageMakeUpper.GetMem(), m_DebugImageMakeUpper.GetSizeX() * m_DebugImageMakeUpper.GetSizeY());

	auto& inspectionImage = m_inspectionImageDark; // 선택 가능 하도록 변 경 가능성 있음


	if (m_para.m_nSelectImageType == 0) // Bright
    {
        inspectionImage = m_inspectionImageBright;
    }

	m_rectImageValid = IPVM::Rect(inspectionImage);
	m_rectImageValid.m_left = DUMMY_PIXEL_COUNT;
	m_rectImageValid.m_right = m_rectImageValid.m_right - DUMMY_PIXEL_COUNT;
	m_rectImageValid.DeflateRect(2, 2);

	cellWidthPx = m_rectImageValid.Height();
	/*offsetPx = m_cellDesign.fTabOffset * 1000 / m_pixelToUmY;
	widthPx = m_cellDesign.fTabWidth * 1000 / m_pixelToUmY;
	heightPx = m_cellDesign.fTabHeight * 1000 / m_pixelToUmX;*/

	//float offsetPx2 = cellWidthPx - offsetPx - widthPx;
	float fTemp = cellWidthPx / 2;
	float nPithTop = m_rectImageValid.CenterPoint().m_y - fTemp;
	float nPithBottom = m_rectImageValid.CenterPoint().m_y + fTemp;
	
	///////////
	IPVM::Rect_32f rtLeftTab, rtCell, rtRighTab;

	rtLeftTab.m_top = nPithTop;
	rtLeftTab.m_bottom = nPithBottom;

	rtCell.m_top = nPithTop;
	rtCell.m_bottom = nPithBottom;

	rtRighTab.m_top = nPithTop;
	rtRighTab.m_bottom = nPithBottom;
	///////////

	m_paraEdgeDetectMeasure.m_vecedgePara.resize(m_para.m_nLineCountMeasure);
	m_paraEdgeDetectMeasure.m_vecrtDetect.resize(m_para.m_nLineCountMeasure);
	m_paraEdgeDetectMeasure.m_vecedgeFilter.resize(m_para.m_nLineCountMeasure);
	m_paraEdgeDetectMeasure.m_vecDetecCount.resize(m_para.m_nLineCountMeasure);
	m_paraEdgeDetectMeasure.m_vecRansacThreshold.resize(m_para.m_nLineCountMeasure);
    m_AlignGrayValue.vecGV_Bright.resize(m_para.m_nLineCountMeasure);
    m_AlignGrayValue.vecGV_Dark.resize(m_para.m_nLineCountMeasure);

	m_paraEdgeDetectBlob.m_vecedgePara.resize(m_para.m_nLineCountBlob);
	m_paraEdgeDetectBlob.m_vecrtDetect.resize(m_para.m_nLineCountBlob);
	m_paraEdgeDetectBlob.m_vecedgeFilter.resize(m_para.m_nLineCountBlob);
	m_paraEdgeDetectBlob.m_vecDetecCount.resize(m_para.m_nLineCountBlob);
	m_paraEdgeDetectBlob.m_vecRansacThreshold.resize(m_para.m_nLineCountBlob);

	// msa
	/*m_paraEdgeDetectMSA.m_vecedgePara.resize(2);
	m_paraEdgeDetectMSA.m_vecrtDetect.resize(2);
	m_paraEdgeDetectMSA.m_vecedgeFilter.resize(2);
	m_paraEdgeDetectMSA.m_vecDetecCount.resize(2);
	m_paraEdgeDetectMSA.m_vecRansacThreshold.resize(2); */

	//m_unionResult.m_vecLineMeasure.resize(m_para.m_nLineCountMeasure);
	// 파라미터 초기화 여러개 사용 가능하도록....
	for (long cnt = 0; cnt < m_para.m_nLineCountMeasure; cnt++)
	{
		auto& Para = m_para.m_vecDetectLineParaMeasure[cnt];
		Para.SetParameter(m_paraEdgeDetectMeasure.m_vecedgePara[cnt], m_paraEdgeDetectMeasure.m_vecedgeFilter[cnt]);
		
	
       // m_paraEdgeDetectMeasure.m_vecrtDetect[cnt].m_bottom = m_para.m_vecrtDetectLineMeasure[cnt].m_bottom;
        
		m_paraEdgeDetectMeasure.m_vecrtDetect[cnt] = m_para.m_vecrtDetectLineMeasure[cnt];
        if (nPithTop > m_para.m_vecrtDetectLineMeasure[cnt].m_top
            || nPithBottom < m_para.m_vecrtDetectLineMeasure[cnt].m_top)
        {
            m_paraEdgeDetectMeasure.m_vecrtDetect[cnt].m_top = nPithTop;
            m_para.m_vecrtDetectLineMeasure[cnt].m_top = nPithTop;
        }
            
        if (nPithBottom < m_para.m_vecrtDetectLineMeasure[cnt].m_bottom
            || nPithTop > m_para.m_vecrtDetectLineMeasure[cnt].m_bottom)
        {
            m_paraEdgeDetectMeasure.m_vecrtDetect[cnt].m_bottom = nPithBottom;
            m_para.m_vecrtDetectLineMeasure[cnt].m_bottom = nPithBottom;
        }
            
		m_paraEdgeDetectMeasure.m_vecDetecCount[cnt] = m_para.m_vecDetectLineParaMeasure[cnt].m_DetectCount;
		m_paraEdgeDetectMeasure.m_vecRansacThreshold[cnt] = m_para.m_vecDetectLineParaMeasure[cnt].m_RansacThreshold;
	}

	for (long cnt = 0; cnt < m_para.m_nLineCountBlob; cnt++)
	{
		auto& Para = m_para.m_vecDetectLineParaBlob[cnt];
		Para.SetParameter(m_paraEdgeDetectBlob.m_vecedgePara[cnt], m_paraEdgeDetectBlob.m_vecedgeFilter[cnt]);
		m_paraEdgeDetectBlob.m_vecrtDetect[cnt] = m_para.m_vecrtDetectLineBlob[cnt];
        if (nPithTop > m_para.m_vecrtDetectLineBlob[cnt].m_top 
			|| nPithBottom < m_para.m_vecrtDetectLineBlob[cnt].m_top)
        {
            m_paraEdgeDetectBlob.m_vecrtDetect[cnt].m_top = nPithTop;
            m_para.m_vecrtDetectLineBlob[cnt].m_top = nPithTop;
        }
            
        if (nPithBottom < m_para.m_vecrtDetectLineBlob[cnt].m_bottom
            || nPithTop > m_para.m_vecrtDetectLineBlob[cnt].m_bottom)
            {
				m_paraEdgeDetectBlob.m_vecrtDetect[cnt].m_bottom = nPithBottom;
                m_para.m_vecrtDetectLineBlob[cnt].m_bottom = nPithBottom;
            }
            
		m_paraEdgeDetectBlob.m_vecDetecCount[cnt] = m_para.m_vecDetectLineParaBlob[cnt].m_DetectCount;
		m_paraEdgeDetectBlob.m_vecRansacThreshold[cnt] = m_para.m_vecDetectLineParaBlob[cnt].m_RansacThreshold;
	}
	
	if (m_para.m_nInspectionType==1)
    {
        AutoFindEdgeDetectROI2();
    }
		
	/// <summary>
	/// /////////////////////////////// 여기에 roi 옮기는거
	/// </summary>
	/// <param name="debugInfo"></param>
	/// <returns></returns>
	///

    long nRectMeasureIndex = 0;

	for (long nLine = 0; nLine < m_para.m_nLineCountMeasure; nLine++)
	{
		IPVM::Point_32f_C3 pt3;
		std::vector<IPVM::Point_32f_C3> vecpt;
		IPVM::Point_32f_C2 ptHomeTop, ptHomeBot;
		IPVM::Rect rtHomeTop, rtHomeBot;

		IPVM::Rect rtHomeSearchTop;
		rtHomeSearchTop = m_paraEdgeDetectMeasure.m_vecrtDetect[nLine] & m_rectImageValid;

		long nTop = rtHomeSearchTop.m_top;
		long nBottom = rtHomeSearchTop.m_bottom;
		long nSize = nSize = (nBottom - nTop) / 20;
		vecpt.clear();
        
        std::vector<long> vecGV_Bright;
        std::vector<long> vecGV_Dark;
		if (m_para.m_nSelectImageType == 2)
        {
            if (m_para.m_vecDetectLineParaMeasure[nLine].m_ImageTypeDark == true)
            {
                auto& inspImage = m_inspectionImageDark;
                for (long n = 0; n < m_paraEdgeDetectMeasure.m_vecDetecCount[nLine]; n++)
                {
                    IPVM::Rect rtSeartchTop = rtHomeSearchTop;
                    rtSeartchTop.m_top = nTop + nSize * n;
                    rtSeartchTop.m_bottom = nTop + nSize * (n + 1);

                    if (m_paraEdgeDetectMeasure.m_edgeDetection->DetectEdge(*((IPVM::Image_8u_C1*)&inspImage),
                            m_paraEdgeDetectMeasure.m_vecedgePara[nLine],
                            m_paraEdgeDetectMeasure.m_vecedgeFilter[nLine], rtSeartchTop, pt3)
                        == IPVM::Status::success)
                    {
                        vecpt.push_back(pt3);
                    }
                    m_debugInfo.m_vecrtBoundrySearch.push_back(rtSeartchTop);

                    IPVM::Point_32f_C2 pt2;
                    pt2.m_x = pt3.m_x;
                    pt2.m_y = pt3.m_y;
                    m_debugInfo.m_vecSearch.push_back(pt2);
                    double dValue_Left;
                    double dValue_Right;
                    IPVM::Rect rt_Left(rtSeartchTop.m_left, rtSeartchTop.m_top, (long)pt2.m_x, rtSeartchTop.m_bottom);
                    IPVM::Rect rt_Right((long)pt2.m_x, rtSeartchTop.m_top, rtSeartchTop.m_right, rtSeartchTop.m_bottom);
                    IPVM::ImageProcessing::GetMean(inspectionImage, rt_Left, dValue_Left);
                    IPVM::ImageProcessing::GetMean(inspectionImage, rt_Right, dValue_Right);

                    if (dValue_Left > dValue_Right)
                    {
                        vecGV_Bright.push_back(dValue_Left);
                        vecGV_Dark.push_back(dValue_Right);
                    }
                    else
                    {
                        vecGV_Bright.push_back(dValue_Right);
                        vecGV_Dark.push_back(dValue_Left);
                    }
                    debugInfo->AddOverlayAndText(
                        LanguageDef::Debug_InitialLinePointMeasure, pt2, _T("InitialLinePoint_Measure"));
                    debugInfo->AddOverlay(LanguageDef::Debug_InitialLinePointMeasure, pt2, RGB(255, 255, 0));
                }
            }
            else
            {
                auto& inspImage = m_inspectionImageBright;
                for (long n = 0; n < m_paraEdgeDetectMeasure.m_vecDetecCount[nLine]; n++)
                {
                    IPVM::Rect rtSeartchTop = rtHomeSearchTop;
                    rtSeartchTop.m_top = nTop + nSize * n;
                    rtSeartchTop.m_bottom = nTop + nSize * (n + 1);

                    if (m_paraEdgeDetectMeasure.m_edgeDetection->DetectEdge(*((IPVM::Image_8u_C1*)&inspImage),
                            m_paraEdgeDetectMeasure.m_vecedgePara[nLine],
                            m_paraEdgeDetectMeasure.m_vecedgeFilter[nLine], rtSeartchTop, pt3)
                        == IPVM::Status::success)
                    {
                        vecpt.push_back(pt3);
                    }
                    m_debugInfo.m_vecrtBoundrySearch.push_back(rtSeartchTop);

                    IPVM::Point_32f_C2 pt2;
                    pt2.m_x = pt3.m_x;
                    pt2.m_y = pt3.m_y;
                    m_debugInfo.m_vecSearch.push_back(pt2);
                    debugInfo->AddOverlayAndText(
                        LanguageDef::Debug_InitialLinePointMeasure, pt2, _T("InitialLinePoint_Measure"));
                    debugInfo->AddOverlay(LanguageDef::Debug_InitialLinePointMeasure, pt2, RGB(255, 255, 0));
                }
                long nSum_Bright = 0;
                long nSum_Dark = 0;
                long nAvg_Bright;
                long nAvg_Dark;
                for (long n = 0; n < vecGV_Bright.size(); n++)
                {
                    nSum_Bright += vecGV_Bright[n];
                    nSum_Dark += vecGV_Dark[n];
                }
                nAvg_Bright = nSum_Bright / vecGV_Bright.size();
                nAvg_Dark = nSum_Dark / vecGV_Dark.size();
                //m_AlignGrayValue.vecGV_Bright.push_back(nAvg_Bright);
                //m_AlignGrayValue.vecGV_Bright.push_back(nAvg_Dark);
                m_AlignGrayValue.vecGV_Bright[nLine] = nAvg_Bright;
                m_AlignGrayValue.vecGV_Dark[nLine] = nAvg_Dark;
			}
		}
        else
        {
            for (long n = 0; n < m_paraEdgeDetectMeasure.m_vecDetecCount[nLine]; n++)
            {
                IPVM::Rect rtSeartchTop = rtHomeSearchTop;
                rtSeartchTop.m_top = nTop + nSize * n;
                rtSeartchTop.m_bottom = nTop + nSize * (n + 1);

                if (m_paraEdgeDetectMeasure.m_edgeDetection->DetectEdge(*((IPVM::Image_8u_C1*)&inspectionImage),
                        m_paraEdgeDetectMeasure.m_vecedgePara[nLine], m_paraEdgeDetectMeasure.m_vecedgeFilter[nLine],
                        rtSeartchTop, pt3)
                    == IPVM::Status::success)
                {
                    vecpt.push_back(pt3);
                }

                m_debugInfo.m_vecrtBoundrySearch.push_back(rtSeartchTop);

                IPVM::Point_32f_C2 pt2;
                pt2.m_x = pt3.m_x;
                pt2.m_y = pt3.m_y;
                m_debugInfo.m_vecSearch.push_back(pt2);
                double dValue_Left;
                double dValue_Right;
                IPVM::Rect rt_Left(rtSeartchTop.m_left, rtSeartchTop.m_top, (long)pt2.m_x, rtSeartchTop.m_bottom);
                IPVM::Rect rt_Right((long)pt2.m_x, rtSeartchTop.m_top, rtSeartchTop.m_right, rtSeartchTop.m_bottom);
                IPVM::ImageProcessing::GetMean(inspectionImage, rt_Left, dValue_Left);
                IPVM::ImageProcessing::GetMean(inspectionImage, rt_Right, dValue_Right);

                if (dValue_Left > dValue_Right)
                {
                    vecGV_Bright.push_back(dValue_Left);
                    vecGV_Dark.push_back(dValue_Right);
                }
                else
                {
                    vecGV_Bright.push_back(dValue_Right);
                    vecGV_Dark.push_back(dValue_Left);
                }
                debugInfo->AddOverlayAndText(
                    LanguageDef::Debug_InitialLinePointMeasure, pt2, _T("InitialLinePoint_Measure"));
                debugInfo->AddOverlay(LanguageDef::Debug_InitialLinePointMeasure, pt2, RGB(255, 255, 0));
            }
            long nSum_Bright = 0;
            long nSum_Dark = 0;
            long nAvg_Bright;
            long nAvg_Dark;
            for (long n = 0; n < vecGV_Bright.size(); n++)
            {
                nSum_Bright += vecGV_Bright[n];
                nSum_Dark += vecGV_Dark[n];
            }
            nAvg_Bright = nSum_Bright / vecGV_Bright.size();
            nAvg_Dark = nSum_Dark / vecGV_Dark.size();
            //m_AlignGrayValue.vecGV_Bright.push_back(nAvg_Bright);
            //m_AlignGrayValue.vecGV_Bright.push_back(nAvg_Dark);
            m_AlignGrayValue.vecGV_Bright[nLine] = nAvg_Bright;
            m_AlignGrayValue.vecGV_Dark[nLine] = nAvg_Dark;
		}
		

		if (m_paraEdgeDetectMeasure.m_vecDetecCount[nLine] == 0)
        {
            float fSizeDiv10 = rtHomeSearchTop.Height() / 10;
            for (long nSizeY = 0; nSizeY < 10; nSizeY++)
            {
                pt3.m_x = rtHomeSearchTop.CenterPoint().m_x;
                pt3.m_y = rtHomeSearchTop.m_top + fSizeDiv10 * nSizeY;
                vecpt.push_back(pt3);
                IPVM::Point_32f_C2 pt2;
                pt2.m_x = pt3.m_x;
                pt2.m_y = pt3.m_y;
                m_debugInfo.m_vecSearch.push_back(pt2);
                debugInfo->AddOverlayAndText(
                    LanguageDef::Debug_InitialLinePointMeasure, pt2, _T("InitialLinePoint_Measure"));
                debugInfo->AddOverlay(LanguageDef::Debug_InitialLinePointMeasure, pt2, RGB(255, 255, 0));
            }
        }

		{
			std::vector<IPVM::Point_64f_C2> pts64;
			std::vector<IPVM::Point_64f_C2> pts64_used;
			for (int i = 0; i < vecpt.size(); i++)
			{
				pts64.push_back(IPVM::Point_64f_C2(vecpt[i].m_x, vecpt[i].m_y));
			}
			long usedNum;
			pts64_used.resize(vecpt.size());
			IPVM::LineEq_64f lineEq64;		// ax + by + c = 0
			float distanceThreshold = m_paraEdgeDetectMeasure.m_vecRansacThreshold[nLine];
			IPVM::Status status = IPVM::Status::unknownError;
			if (pts64.size() != 0)
			{
				status = IPVM::DataFitting::FitToLine_RANSAC(pts64.size(), &pts64[0], &pts64_used[0], usedNum, lineEq64, distanceThreshold);
			}
			
			if (status == IPVM::Status::success)
			{
				IPVM::LineSeg_32f lineSeg32;
				lineSeg32.m_sx = lineEq64.GetPositionX(nPithTop);
				lineSeg32.m_sy = nPithTop;
				lineSeg32.m_ex = lineEq64.GetPositionX(nPithBottom);
				lineSeg32.m_ey = nPithBottom;

				long nTempBig = lineSeg32.m_sx;
				long nTempSmall = lineSeg32.m_sx;
				if (nTempBig < lineSeg32.m_ex)
				{
					nTempBig = lineSeg32.m_ex;
				}
				if (nTempSmall > lineSeg32.m_ex)
				{
					nTempSmall = lineSeg32.m_ex;
				}
                

                if (m_para.m_nLineCountMeasure == 1)
                {
                    // #1. Inner
                    if (m_para.m_vecDetectLineParaMeasure[nLine].m_EdgeDetectMode == 0)
                    {
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_left = m_rectImageValid.m_left;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_right = nTempSmall;

                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_left = nTempBig;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_top = nPithTop;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_right = m_rectImageValid.m_right;

                    }

                    // #2. Outer
                    else if (m_para.m_vecDetectLineParaMeasure[nLine].m_EdgeDetectMode == 1)
                    {
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_left = m_rectImageValid.m_left;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_right = nTempBig;

                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_left = nTempSmall;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_top = nPithTop;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_right = m_rectImageValid.m_right;
                    }

                    // #3. Average
                    else if (m_para.m_vecDetectLineParaMeasure[nLine].m_EdgeDetectMode == 2)
                    {
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_left = m_rectImageValid.m_left;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_right = (lineSeg32.m_ex + lineSeg32.m_sx) / 2;

                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_left = (lineSeg32.m_ex + lineSeg32.m_sx) / 2;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_top = nPithTop;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_right = m_rectImageValid.m_right;
                    }
                }
                else
                {
                    // #1. Inner
                    if (m_para.m_vecDetectLineParaMeasure[nLine].m_EdgeDetectMode == 0)
                    {
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_left = nTempBig;

                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_bottom = nPithBottom;

                        if (nLine > 0)
                        {
                            m_unionResult.m_rectAreaMeasure[nRectMeasureIndex - 1].m_right = nTempSmall;
                        }
                    }

                    // #2. Outer
                    else if (m_para.m_vecDetectLineParaMeasure[nLine].m_EdgeDetectMode == 1)
                    {
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_left = nTempSmall;

                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_bottom = nPithBottom;

                        if (nLine > 0)
                        {
                            m_unionResult.m_rectAreaMeasure[nRectMeasureIndex - 1].m_right = nTempSmall;
                        }
                    }

                    // #3. Average
                    else if (m_para.m_vecDetectLineParaMeasure[nLine].m_EdgeDetectMode == 2)
                    {
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_left = (lineSeg32.m_ex + lineSeg32.m_sx) / 2;

                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_bottom = nPithBottom;

                        if (nLine > 0)
                        {
                            m_unionResult.m_rectAreaMeasure[nRectMeasureIndex - 1].m_right = (lineSeg32.m_ex + lineSeg32.m_sx) / 2;
                        }
                    }
                }

                if (m_para.m_bUseCenterROISplitMeasure == TRUE)
                {
                    if (m_unionResult.m_rectAreaMeasure[nRectMeasureIndex - 1].PtInRect(IPVM::Point_32f_C2(inspectionImage.GetSizeX()/2, inspectionImage.GetSizeY()/2)) == TRUE)
                    {
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex - 1].m_right = inspectionImage.GetSizeX() / 2 - 1;

                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_left = m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_left;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_right = m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_right;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_top = m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_top;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex + 1].m_bottom = m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_bottom;

                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_right = m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_left;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_left = inspectionImage.GetSizeX() / 2;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaMeasure[nRectMeasureIndex].m_bottom = nPithBottom;

                        nRectMeasureIndex++;
                    } 
                }

				debugInfo->AddOverlay(LanguageDef::Debug_CellBoundaryLine, lineSeg32);
			}
			else
			{
				m_resultValue = II_RESULT_VALUE::REJECT;
				return false;
			}

			for (int i = 0; i < usedNum; i++)
			{
				IPVM::Point_32f_C2 pt2;
				pt2.m_x = pts64_used[i].m_x;
				pt2.m_y = pts64_used[i].m_y;
				debugInfo->AddOverlayAndText(LanguageDef::Debug_LinePointMeasure, pt2, _T("LinePoint Measure"));
				debugInfo->AddOverlay(LanguageDef::Debug_LinePointMeasure, pt2, RGB(0, 255, 0));
			}

            nRectMeasureIndex++;
		}
	}

    long nRectBlobIndex = 0;

	for (long nLine = 0; nLine < m_para.m_nLineCountBlob; nLine++)
	{
		IPVM::Point_32f_C3 pt3;
		std::vector<IPVM::Point_32f_C3> vecpt;
		IPVM::Point_32f_C2 ptHomeTop, ptHomeBot;
		IPVM::Rect rtHomeTop, rtHomeBot;

		IPVM::Rect rtHomeSearchTop;
		rtHomeSearchTop = m_paraEdgeDetectBlob.m_vecrtDetect[nLine] & m_rectImageValid;

		long nTop = rtHomeSearchTop.m_top;
		long nBottom = rtHomeSearchTop.m_bottom;
		long nSize = nSize = (nBottom - nTop) / 20;
		vecpt.clear();

		if (m_para.m_nSelectImageType == 2)
        {
            if (m_para.m_vecDetectLineParaMeasure[nLine].m_ImageTypeDark == true)
            {
                auto& inspImage = m_inspectionImageDark;

				for (long n = 0; n < m_paraEdgeDetectBlob.m_vecDetecCount[nLine]; n++)
                {
                    IPVM::Rect rtSeartchTop = rtHomeSearchTop;
                    rtSeartchTop.m_top = nTop + nSize * n;
                    rtSeartchTop.m_bottom = nTop + nSize * (n + 1);

                    if (m_paraEdgeDetectBlob.m_edgeDetection->DetectEdge(*((IPVM::Image_8u_C1*)&inspImage),
                            m_paraEdgeDetectBlob.m_vecedgePara[nLine], m_paraEdgeDetectBlob.m_vecedgeFilter[nLine],
                            rtSeartchTop, pt3)
                        == IPVM::Status::success)
                    {
                        vecpt.push_back(pt3);
                    }
                    m_debugInfo.m_vecrtBoundrySearch.push_back(rtSeartchTop);

                    IPVM::Point_32f_C2 pt2;
                    pt2.m_x = pt3.m_x;
                    pt2.m_y = pt3.m_y;
                    m_debugInfo.m_vecSearch.push_back(pt2);
                    debugInfo->AddOverlayAndText(
                        LanguageDef::Debug_InitialLinePointBlob, pt2, _T("InitialLinePoint_Blob"));
                    debugInfo->AddOverlay(LanguageDef::Debug_InitialLinePointBlob, pt2, RGB(255, 255, 0));
                }
            }
            else
            {
                auto& inspImage = m_inspectionImageBright;

				for (long n = 0; n < m_paraEdgeDetectBlob.m_vecDetecCount[nLine]; n++)
                {
                    IPVM::Rect rtSeartchTop = rtHomeSearchTop;
                    rtSeartchTop.m_top = nTop + nSize * n;
                    rtSeartchTop.m_bottom = nTop + nSize * (n + 1);

                    if (m_paraEdgeDetectBlob.m_edgeDetection->DetectEdge(*((IPVM::Image_8u_C1*)&inspImage),
                            m_paraEdgeDetectBlob.m_vecedgePara[nLine], m_paraEdgeDetectBlob.m_vecedgeFilter[nLine],
                            rtSeartchTop, pt3)
                        == IPVM::Status::success)
                    {
                        vecpt.push_back(pt3);
                    }
                    m_debugInfo.m_vecrtBoundrySearch.push_back(rtSeartchTop);

                    IPVM::Point_32f_C2 pt2;
                    pt2.m_x = pt3.m_x;
                    pt2.m_y = pt3.m_y;
                    m_debugInfo.m_vecSearch.push_back(pt2);
                    debugInfo->AddOverlayAndText(
                        LanguageDef::Debug_InitialLinePointBlob, pt2, _T("InitialLinePoint_Blob"));
                    debugInfo->AddOverlay(LanguageDef::Debug_InitialLinePointBlob, pt2, RGB(255, 255, 0));
                }
			}
        }
        else
        {
            for (long n = 0; n < m_paraEdgeDetectBlob.m_vecDetecCount[nLine]; n++)
            {
                IPVM::Rect rtSeartchTop = rtHomeSearchTop;
                rtSeartchTop.m_top = nTop + nSize * n;
                rtSeartchTop.m_bottom = nTop + nSize * (n + 1);

                if (m_paraEdgeDetectBlob.m_edgeDetection->DetectEdge(*((IPVM::Image_8u_C1*)&inspectionImage),
                        m_paraEdgeDetectBlob.m_vecedgePara[nLine], m_paraEdgeDetectBlob.m_vecedgeFilter[nLine],
                        rtSeartchTop, pt3)
                    == IPVM::Status::success)
                {
                    vecpt.push_back(pt3);
                }
                m_debugInfo.m_vecrtBoundrySearch.push_back(rtSeartchTop);

                IPVM::Point_32f_C2 pt2;
                pt2.m_x = pt3.m_x;
                pt2.m_y = pt3.m_y;
                m_debugInfo.m_vecSearch.push_back(pt2);
                debugInfo->AddOverlayAndText(LanguageDef::Debug_InitialLinePointBlob, pt2, _T("InitialLinePoint_Blob"));
                debugInfo->AddOverlay(LanguageDef::Debug_InitialLinePointBlob, pt2, RGB(255, 255, 0));
            }
        }

		
        if (m_paraEdgeDetectBlob.m_vecDetecCount[nLine] == 0)
        {
            float fSizeDiv10 = rtHomeSearchTop.Height() / 10;
            for (long nSizeY = 0; nSizeY < 10; nSizeY++)
            {
                pt3.m_x = rtHomeSearchTop.CenterPoint().m_x;
                pt3.m_y = rtHomeSearchTop.m_top + fSizeDiv10 * nSizeY;
                vecpt.push_back(pt3);
                IPVM::Point_32f_C2 pt2;
                pt2.m_x = pt3.m_x;
                pt2.m_y = pt3.m_y;
                m_debugInfo.m_vecSearch.push_back(pt2);
                debugInfo->AddOverlayAndText(LanguageDef::Debug_InitialLinePointBlob, pt2, _T("InitialLinePoint_Blob"));
                debugInfo->AddOverlay(LanguageDef::Debug_InitialLinePointBlob, pt2, RGB(255, 255, 0));
            }
        }
		{
			std::vector<IPVM::Point_64f_C2> pts64;
			std::vector<IPVM::Point_64f_C2> pts64_used;
			for (int i = 0; i < vecpt.size(); i++)
			{
				pts64.push_back(IPVM::Point_64f_C2(vecpt[i].m_x, vecpt[i].m_y));
			}
			long usedNum;
			pts64_used.resize(vecpt.size());
			IPVM::LineEq_64f lineEq64;		// ax + by + c = 0
			float distanceThreshold = m_paraEdgeDetectBlob.m_vecRansacThreshold[nLine];
			IPVM::Status status = IPVM::Status::unknownError;
			if (pts64.size() != 0)
			{
				status = IPVM::DataFitting::FitToLine_RANSAC(pts64.size(), &pts64[0], &pts64_used[0], usedNum, lineEq64, distanceThreshold);
			}

			if (status == IPVM::Status::success)
			{
				IPVM::LineSeg_32f lineSeg32;
				lineSeg32.m_sx = lineEq64.GetPositionX(nPithTop);
				lineSeg32.m_sy = nPithTop;
				lineSeg32.m_ex = lineEq64.GetPositionX(nPithBottom);
				lineSeg32.m_ey = nPithBottom;

				long nTempBig = lineSeg32.m_sx;
				long nTempSmall = lineSeg32.m_sx;
				if (nTempBig < lineSeg32.m_ex)
				{
					nTempBig = lineSeg32.m_ex;
				}
				if (nTempSmall > lineSeg32.m_ex)
				{
					nTempSmall = lineSeg32.m_ex;
				}

                if (m_para.m_nLineCountMeasure == 1)
                {
                    // #1. Inner
                    if (m_para.m_vecDetectLineParaBlob[nLine].m_EdgeDetectMode == 0)
                    {
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_left = m_rectImageValid.m_left;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_right = nTempSmall;

                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_left = nTempBig;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_top = nPithTop;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_right = m_rectImageValid.m_right;
                    }

                    // #2. Outer
                    else if (m_para.m_vecDetectLineParaBlob[nLine].m_EdgeDetectMode == 1)
                    {
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_left = m_rectImageValid.m_left;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_right = nTempBig;

                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_left = nTempSmall;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_top = nPithTop;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaBlob[ + 1].m_right = m_rectImageValid.m_right;
                    }

                    // #3. Average
                    else if (m_para.m_vecDetectLineParaBlob[nLine].m_EdgeDetectMode == 2)
                    {
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_left = m_rectImageValid.m_left;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_right = (lineSeg32.m_ex + lineSeg32.m_sx) / 2;

                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_left = (lineSeg32.m_ex + lineSeg32.m_sx) / 2;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_top = nPithTop;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_bottom = nPithBottom;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_right = m_rectImageValid.m_right;
                    }
                }
                else
                {
                    // #1. Inner
                    if (m_para.m_vecDetectLineParaBlob[nLine].m_EdgeDetectMode == 0)
                    {
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_left = nTempBig;

                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_bottom = nPithBottom;

                        if (nLine > 0)
                        {
                            m_unionResult.m_rectAreaBlob[nRectBlobIndex - 1].m_right = nTempSmall;
                        }
                    }

                    // #2. Outer
                    else if (m_para.m_vecDetectLineParaBlob[nLine].m_EdgeDetectMode == 1)
                    {
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_left = nTempSmall;

                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_bottom = nPithBottom;

                        if (nLine > 0)
                        {
                            m_unionResult.m_rectAreaBlob[nRectBlobIndex - 1].m_right = nTempSmall;
                        }
                    }

                    // #3. Average
                    else if (m_para.m_vecDetectLineParaBlob[nLine].m_EdgeDetectMode == 2)
                    {
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_left = (lineSeg32.m_ex + lineSeg32.m_sx) / 2;

                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_bottom = nPithBottom;

                        if (nLine > 0)
                        {
                            m_unionResult.m_rectAreaBlob[nRectBlobIndex - 1].m_right
                                = (lineSeg32.m_ex + lineSeg32.m_sx) / 2;
                        }
                    }
                }

                if (m_para.m_bUseCenterROISplitBlob == TRUE)
                {
                    if (m_unionResult.m_rectAreaBlob[nRectBlobIndex - 1].PtInRect(IPVM::Point_32f_C2(inspectionImage.GetSizeX() / 2, inspectionImage.GetSizeY() / 2)) == TRUE)
                    {
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex - 1].m_right = inspectionImage.GetSizeX() / 2 - 1;

                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_left = m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_left;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_right = m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_right;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_top = m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_top;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex + 1].m_bottom = m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_bottom;

                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_right = m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_left;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_left = inspectionImage.GetSizeX() / 2;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_top = nPithTop;
                        m_unionResult.m_rectAreaBlob[nRectBlobIndex].m_bottom = nPithBottom;

                        nRectBlobIndex++;
                    }
                }


				debugInfo->AddOverlay(LanguageDef::Debug_CellBoundaryLine, lineSeg32);
			}
			else
			{
				m_resultValue = II_RESULT_VALUE::REJECT;
				return false;
			}

			for (int i = 0; i < usedNum; i++)
			{
				IPVM::Point_32f_C2 pt2;
				pt2.m_x = pts64_used[i].m_x;
				pt2.m_y = pts64_used[i].m_y;
				debugInfo->AddOverlayAndText(LanguageDef::Debug_LinePointBlob, pt2, _T("LinePoint Blob"));
				debugInfo->AddOverlay(LanguageDef::Debug_LinePointBlob, pt2, RGB(0, 255, 0));
			}

            nRectBlobIndex++;
		}
	}
	

	//// MSA 
	//m_unionResult.m_vecPtMSA.resize(2);
	//m_paraEdgeDetectMSA.m_vecedgePara[0].m_edgeType = IPVM::EdgeType::Both;
 //   m_paraEdgeDetectMSA.m_vecedgePara[1].m_edgeType = IPVM::EdgeType::Both;
	//m_paraEdgeDetectMSA.m_vecedgePara[0].m_searchDirection = IPVM::SearchDirection::ToDown;
 //   m_paraEdgeDetectMSA.m_vecedgePara[1].m_searchDirection = IPVM::SearchDirection::ToUp;
	//for (long nLine = 0; nLine < 2; nLine++)
	//{
	//	IPVM::Point_32f_C3 pt3;
	//	std::vector<IPVM::Point_32f_C3> vecpt;
	//	IPVM::Point_32f_C2 ptHomeTop, ptHomeBot;
	//	IPVM::Rect rtHomeTop, rtHomeBot;
	//	CInspectionCellAlignEdgeDetectPara test;

	//	IPVM::Rect rtHomeSearchTop;
	//	rtHomeSearchTop = m_paraEdgeDetectMSA.m_vecrtDetect[nLine] & m_rectImageValid;

	//	long nLeft = rtHomeSearchTop.m_left;
	//	long nRight = rtHomeSearchTop.m_right;
	//	long nSize = nSize = (nRight - nLeft) / 20;
	//	vecpt.clear();

	//	for (long n = 0; n < m_paraEdgeDetectMSA.m_vecDetecCount[nLine]; n++)
	//	{
	//		IPVM::Rect rtSeartchTop = rtHomeSearchTop;
	//		rtSeartchTop.m_left = nLeft + nSize * n;
	//		rtSeartchTop.m_right = nLeft + nSize * (n + 1);

	//		if (m_paraEdgeDetectMSA.m_edgeDetection->DetectEdge(*((IPVM::Image_8u_C1*)&inspectionImage),
	//			m_paraEdgeDetectMSA.m_vecedgePara[nLine],
	//			m_paraEdgeDetectMSA.m_vecedgeFilter[nLine],
	//			rtSeartchTop,
	//			pt3) == IPVM::Status::success)
	//		{
	//			vecpt.push_back(pt3);
	//			m_unionResult.m_vecPtMSA[nLine].m_x = pt3.m_x;
	//			m_unionResult.m_vecPtMSA[nLine].m_y = pt3.m_y;

	//			IPVM::Point_32f_C2 pt2;
	//			pt2.m_x = pt3.m_x;
	//			pt2.m_y = pt3.m_y;
	//			debugInfo->AddOverlay(LanguageDef::Debug_InitialLinePointMSA, pt2, RGB(255, 0, 0));
	//		}
	//	}
	//}

    long nLineCountMeasure = (m_para.m_nLineCountMeasure == 1) ? 2 : nRectMeasureIndex - 1;

	for (int i = 0; i < nLineCountMeasure; i++)
	{
		debugInfo->AddOverlay(LanguageDef::Debug_CellBoundaryROIMeasure, m_unionResult.m_rectAreaMeasure[i]);
		debugInfo->AddOverlay(LanguageDef::Result, m_unionResult.m_rectAreaMeasure[i], RGB(255, 128, 0));
	}

    long nLineCountBlob = (m_para.m_nLineCountBlob == 1) ? 2 : nRectBlobIndex - 1;

    for (int i = 0; i < nLineCountBlob; i++)
	{
		debugInfo->AddOverlay(LanguageDef::Debug_CellBoundaryROIBlob, m_unionResult.m_rectAreaBlob[i]);
		debugInfo->AddOverlay(LanguageDef::Result, m_unionResult.m_rectAreaBlob[i], RGB(255, 0, 128));
	}


	m_resultValue = II_RESULT_VALUE::PASS;
	return true;
}

void CInspectionCellAlign::ProcInspectionEnd(Inspection::DebugInfo* debugInfo)
{
	if (FALSE == m_parent->m_isTest) return;

	//debugInfo->AddOverlayAndText(LanguageDef::Result, m_unionResult.m_ptVHome[II_CORNER_VALUE::II_LEFT_TOP], _T("Left Top"));
	//debugInfo->AddOverlayAndText(LanguageDef::Result, m_unionResult.m_ptVHome[II_CORNER_VALUE::II_LEFT_BOTTOM], _T("Left Bottom"));
	
}


bool compare(IPVM::Point_32f_C2 a, IPVM::Point_32f_C2 b) {
	return a.m_x < b.m_x;
}

bool compare_distance(IPVM::Point_32f_C3 a, IPVM::Point_32f_C3 b) {
	return a.m_z < b.m_z;
}


bool CInspectionCellAlign::ProcInspection(bool isLeft)
{
	


	return true;
}



struct stArrDataPoint
{
	BYTE byValue;
	long nPoint;

	stArrDataPoint()
	{
		byValue = 255;
		nPoint = 0;
	}
};



void CInspectionCellAlign::LinkDataBase(bool bSave, Database& dbJob)
{
	m_para.LinkDataBase(bSave, dbJob);
}

bool CInspectionCellAlign::CopyDataBase(CProcessingBase* src)
{
	auto* ptr = dynamic_cast<CInspectionCellAlign*>(src);
	if (ptr == nullptr)
	{
		ASSERT(!_T("같은 타입의 Inspection만 복사가 가능하다"));
		return false;
	}

	auto& rhs = *ptr;

	m_pixelToUmX = rhs.m_pixelToUmX;
	m_pixelToUmY = rhs.m_pixelToUmY;

	m_para = rhs.m_para;

	m_isUp = rhs.m_isUp;

	return true;
}
