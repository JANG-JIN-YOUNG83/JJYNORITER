// 1. 미리 컴파일된 헤더
#include "stdafx.h"
// 2. 주 헤더
// 3. 보조 헤더
#include "CISIDefectProperty.h"
#include "CISIDefectResult.h"
#include "CISISpecTable.h"

#include "../SharedModule/CommonShare/DefectDef.h"
#include "../SharedModule/VisionHostShare/SpecData.h"

#include <iostream>
#include <utility> // make_pair()

//bool compare_multi(std::pair<DefectType, std::vector<CISISpecInfo>> a, std::pair<DefectType, std::vector<CISISpecInfo>> b)
//{
//    float ftime = 0.f;
//    if (a.second.size() == 0 || b.second.size() == 0)
//    {
//        return true;
//    }
//    if (a.second[0].PRIORITY < b.second[0].PRIORITY)
//    {
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}

CISISpecTable::CISISpecTable()
    : m_bUse(_T("isi classification use"), 0)
{
    initSpec();
}

void CISISpecTable::initSpec()
{
   // m_mapSpec.clear();
    m_mapSpec_multi.clear();
    // 현식 일반 검사항목 스펙 제작 기능
    // 승만
    //검사 항목
    bool m_bSize = true;
    bool m_bAREA = true;
    bool m_bWIDTH = true;
    bool m_bLENGTH = true;
    bool m_bRATIO_WL = true;
    bool m_bRATIO_LW = true;
    bool m_bRATIO_YX = true;
    bool m_bRATIO_XY = true;
    bool m_bANGLE = true;
    bool m_bSTD = true;
    bool m_bAVG = true;
    bool m_bMIN_GV = true;
    bool m_bMAX_GV = true;
    bool m_bBRIGHT_AVG_GV = true;
    bool m_bDARK_AVG_GV = true;
    bool m_bCOMPACT = true;
    bool m_bSECTION_THRESHOLD = true;
    bool m_bCount = false;
    bool m_bPriority = true;
    bool m_bOverPxCount = true;

    // [sm]
    bool m_bRATIO_WHITE = true;
    bool m_bRATIO_DARK = true;
    bool m_bEDGE_ENERGY = true;
    bool m_bEDGE_ENERGY_BLACK = true;
    bool m_bEDGE_ENERGY_WHITE = true;
    bool m_bRATIO_AREA = true;
    bool m_bMASS_CENTER_GV_GV = true;
    bool m_bHOR = true;
    bool m_bVER = true;
    bool m_bRATIO_HV = true;
    bool m_bRATIO_VH = true;

    bool m_bSMALL_INSP = true;
    bool m_bSMALL_INSP_LEVEL = true;
    bool m_bPARENT = false;

    bool m_bDis2Edge = true;
    bool m_bDis2Edge_2 = true;
    bool m_bIsNear_Edge = true; 
    bool m_bEDGE_ENERGY_SHARPNESS = true;
    bool m_bEDGE_ENERGY_SHARPNESS_WHITE = true;
    bool m_bEDGE_ENERGY_SHARPNESS_BLACK = true;
    bool m_bMEDIAN_GV = true;
    bool m_bSTDEV_GV = true;
    //std::vector<std::pair<DefectType, CISISpecInfo>> v;

    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> v_multi;



    //only size [sm]

    /*Line,
	Pinhole,
	Horizontal_Line,
	Stain,
	Island,
	Chipping_Pinhole,
	Drag,
	Edge_Line,
	Dent,
	Crater,
	Balck_Dot,*/

    //small_defect
    m_bSize = true;
    m_bAREA = false;
    m_bWIDTH = false;
    m_bLENGTH = false;
    m_bRATIO_YX = false;
    m_bRATIO_XY = false;
    m_bRATIO_WL = false;
    m_bRATIO_LW = false;
    m_bANGLE = false;
    m_bSTD = false;
    m_bAVG = false;
    m_bMIN_GV = false;
    m_bMAX_GV = false;
    m_bBRIGHT_AVG_GV = false;
    m_bDARK_AVG_GV = false;
    m_bCOMPACT = false;
    m_bSECTION_THRESHOLD = false;
    m_bCount = true;

    m_bRATIO_WHITE = false;
    m_bRATIO_DARK = false;
    m_bEDGE_ENERGY = false;
    m_bRATIO_AREA = false;
    m_bMASS_CENTER_GV_GV = false;
    m_bHOR = false;
    m_bVER = false;
    m_bRATIO_HV = false;
    m_bRATIO_VH = false;

    m_bSMALL_INSP = false;
    m_bPARENT = true;
    m_bOverPxCount = false;
    m_bDis2Edge = false;
    m_bDis2Edge_2 = false;
    m_bEDGE_ENERGY_BLACK = false;
    m_bEDGE_ENERGY_WHITE = false;
    m_bIsNear_Edge = false;
    m_bPriority = true;    
    m_bEDGE_ENERGY_SHARPNESS_WHITE = true;
    m_bEDGE_ENERGY_SHARPNESS_BLACK= true;
    m_bMEDIAN_GV = true;
    m_bSTDEV_GV = true;
    //장지수 Map 추가
    std::vector<CISISpecInfo> temp;

#if INSPECTION_MACHINE_UC1
    temp.push_back(CISISpecInfo(DefectType::White_Line, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
        m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
        m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::White_Line, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::White_Pinhole, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
        m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
        m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::White_Pinhole, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Dark_Line, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Dark_Line, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Dark_Crater, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Dark_Crater, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Foil_Island, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Foil_Island, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Edge_ChippingPinhole, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Edge_ChippingPinhole, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Edge_Drag, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Edge_Drag, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Edge_Line, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Edge_Line, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Crater, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP, m_bPARENT,
            m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV, m_bCOMPACT,
            m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Crater, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Balck_Dot, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Balck_Dot, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Banking, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
        m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
        m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Banking, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Delami, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
        m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
        m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Delami, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Dent, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP, m_bPARENT,
            m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV, m_bCOMPACT,
            m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Dent, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Scratch, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP, m_bPARENT,
            m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV, m_bCOMPACT,
            m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Scratch, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Crack, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP, m_bPARENT,
            m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV, m_bCOMPACT,
            m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Crack, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Protrusion, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Protrusion, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Foil_Defect, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Foil_Defect, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Edge_Defect, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Edge_Defect, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::ETC_Defect, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::ETC_Defect, temp));
    temp.clear();
#else
    temp.push_back(CISISpecInfo(DefectType::Line, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
        m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
        m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Line, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Pinhole, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
        m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
        m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Pinhole, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Horizontal_Line, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Horizontal_Line, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Stain, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Stain, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Island, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Island, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Chipping_Pinhole, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Chipping_Pinhole, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Drag, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Drag, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Edge_Line, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Edge_Line, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Stain_Pinhole, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP, m_bPARENT,
            m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV, m_bCOMPACT,
            m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Stain_Pinhole, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Balck_Dot, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Balck_Dot, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Banking, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
        m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
        m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Banking, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Delamination_Pinhole, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
        m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
        m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Delamination_Pinhole, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Dent, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP, m_bPARENT,
            m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV, m_bCOMPACT,
            m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Dent, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Scratch, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP, m_bPARENT,
            m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV, m_bCOMPACT,
            m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Scratch, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Crack, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP, m_bPARENT,
            m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV, m_bCOMPACT,
            m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Crack, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Delamination_Crack, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Delamination_Crack, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Plain_Wrinkle, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Plain_Wrinkle, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::Edge_Defect, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::Edge_Defect, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::ETC_Defect, 1, enCompareOperator::BIGGER_THAN_SPEC, 1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP,
            m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV,
            m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::ETC_Defect, temp));
    temp.clear();
#endif // 

    

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_CraterLevel1, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_CraterLevel1, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_CraterLevel2, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_CraterLevel2, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_CraterLevel3, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_CraterLevel3, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_CraterLevel4, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_CraterLevel4, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_CraterLevel5, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_CraterLevel5, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_PinholeLevel1, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize,
            m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_PinholeLevel1, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_PinholeLevel2, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize,
            m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_PinholeLevel2, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_PinholeLevel3, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_PinholeLevel3, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_PinholeLevel4, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC,
            enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_PinholeLevel4, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_PinholeLevel5, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_PinholeLevel5, temp));
    temp.clear();
    // Dent
    temp.push_back(CISISpecInfo(DefectType::SmallDefect_DentLevel1, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_DentLevel1, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_DentLevel2, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC,
            enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_DentLevel2, temp));
    temp.clear();

   temp.push_back(CISISpecInfo(DefectType::SmallDefect_DentLevel3, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_DentLevel3, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_DentLevel4, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_DentLevel4, temp));
    temp.clear();

   temp.push_back(CISISpecInfo(DefectType::SmallDefect_DentLevel5, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize,
            m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_DentLevel5, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_ProtrutionLevel1, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_ProtrutionLevel1, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_ProtrutionLevel2, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_ProtrutionLevel2, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_ProtrutionLevel3, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_ProtrutionLevel3, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_ProtrutionLevel4, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_ProtrutionLevel4, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_ProtrutionLevel5, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_ProtrutionLevel5, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_IslandLevel1, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_IslandLevel1, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_IslandLevel2, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_IslandLevel2, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_IslandLevel3, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC,
        enCode::CD_OR, m_bSize, m_bAREA, m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV,
        m_bMAX_GV, m_bBRIGHT_AVG_GV, m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_IslandLevel3, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_IslandLevel4, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize,
            m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_IslandLevel4, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_IslandLevel5, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize,
            m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_IslandLevel5, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_ScratchLevel1, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_ScratchLevel1, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_ScratchLevel2, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_ScratchLevel2, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_ScratchLevel3, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_ScratchLevel3, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_ScratchLevel4, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_ScratchLevel4, temp));
    temp.clear();

    temp.push_back(CISISpecInfo(DefectType::SmallDefect_ScratchLevel5, 0.1, enCompareOperator::BIGGER_THAN_SPEC, 0.1, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, m_bSize, m_bAREA,
            m_bSMALL_INSP, m_bPARENT, m_bWIDTH, m_bLENGTH, m_bRATIO_WL, m_bRATIO_LW, m_bRATIO_YX, m_bRATIO_XY, m_bANGLE, m_bSTD, m_bAVG, m_bMIN_GV, m_bMAX_GV, m_bBRIGHT_AVG_GV,
            m_bDARK_AVG_GV, m_bCOMPACT, m_bSECTION_THRESHOLD, m_bCount, m_bPriority, m_bRATIO_WHITE, m_bRATIO_DARK, m_bEDGE_ENERGY, m_bEDGE_ENERGY_BLACK, m_bEDGE_ENERGY_WHITE, m_bRATIO_AREA, m_bMASS_CENTER_GV_GV,
        m_bHOR, m_bVER, m_bRATIO_HV, m_bRATIO_VH, m_bOverPxCount, m_bDis2Edge, m_bDis2Edge_2, m_bIsNear_Edge,
        m_bEDGE_ENERGY_SHARPNESS, m_bEDGE_ENERGY_SHARPNESS_WHITE, m_bEDGE_ENERGY_SHARPNESS_BLACK,m_bMEDIAN_GV,m_bSTDEV_GV));
    v_multi.push_back(std::make_pair(DefectType::SmallDefect_ScratchLevel5, temp));
    temp.clear();

    m_mapSpec_multi.insert(v_multi.begin(), v_multi.end());

    std::map<DefectType, std::vector<CISISpecInfo>>::iterator vecitrStart, vecitrEnd;
    vecitrStart = m_mapSpec_multi.begin();
    vecitrEnd = m_mapSpec_multi.end();

    while (vecitrStart != vecitrEnd)
    {
        std::vector<CISISpecInfo>& vecSpec = vecitrStart->second;
        if (vecitrStart->first == DefectType::ConnectionTape || vecitrStart->first == DefectType::DarkConnectionTape)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].nCategory = enCategory::CTG_CONNECTION_TAPE;
            }
            
        }
#if INSPECTION_MACHINE_UC1
        if (vecitrStart->first == DefectType::White_Line)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.05;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].AVG_GV = 168;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                vecSpec[nIndex].RATIO_YX = 4;
                vecSpec[nIndex].RATIO_YX_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::White_Pinhole)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 3;
                vecSpec[nIndex].SIZE_Y_mm = 3;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].AVG_GV = 220;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
                vecSpec[nIndex].MAX_GV = 200;
                vecSpec[nIndex].MAX_GV_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
            }
        }

        if (vecitrStart->first == DefectType::Dent)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 5;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;

                vecSpec[nIndex].AVG_GV_2 = 100;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
            }
        }

        if (vecitrStart->first == DefectType::Dark_Line)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.1;
                vecSpec[nIndex].SIZE_Y_mm = 10;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].AVG_GV_2 = 108;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                vecSpec[nIndex].RATIO_YX = 4;
                vecSpec[nIndex].RATIO_YX_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::Dark_Crater)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 3;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].AVG_GV_2 = 108;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::Crater)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 3;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].AVG_GV_2 = 148;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::Foil_Island)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].AVG_GV = 20;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_TAB;
            }
        }

        if (vecitrStart->first == DefectType::Edge_ChippingPinhole)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.8;
                vecSpec[nIndex].SIZE_Y_mm = 0.8;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
            }
        }

        if (vecitrStart->first == DefectType::Edge_Drag)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 10;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
            }
        }

        if (vecitrStart->first == DefectType::Edge_Line)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.05;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
            }
        }

        if (vecitrStart->first == DefectType::Balck_Dot)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 3;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::Banking)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 10;
                vecSpec[nIndex].SIZE_Y_mm = 10;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::Delami)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.1;
                vecSpec[nIndex].SIZE_Y_mm = 0.1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
            }
        }

        if (vecitrStart->first == DefectType::Scratch)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
                vecSpec[nIndex].AVG_GV_2 = 100;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                vecSpec[nIndex].LENGTH_mm = 1.0;
                vecSpec[nIndex].LENGTH_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                vecSpec[nIndex].RATIO_DARK = 99.0;
                vecSpec[nIndex].RATIO_DARK_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
            }
        }

        if (vecitrStart->first == DefectType::Crack)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
                vecSpec[nIndex].AVG_GV_2 = 100;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                vecSpec[nIndex].MIN_GV_2 = 30;
                vecSpec[nIndex].MIN_GV_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
            }
        }

        if (vecitrStart->first == DefectType::Protrusion)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 20;
                vecSpec[nIndex].SIZE_Y_mm = 20;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
                vecSpec[nIndex].AVG_GV_2 = 120;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
            }
        }

        if (vecitrStart->first == DefectType::Foil_Defect)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_TAB;
                vecSpec[nIndex].nInspectImage = enInspectionImageType::Dark;
                vecSpec[nIndex].INSPECTION_IMAGE_TYPE = enInspectionImageType::Dark;
            }
        }

        if (vecitrStart->first == DefectType::Edge_Defect)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
            }
        }

        if (vecitrStart->first == DefectType::ETC_Defect)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }
#else
        if (vecitrStart->first == DefectType::Line)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.05;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].AVG_GV = 168;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                vecSpec[nIndex].RATIO_YX = 4;
                vecSpec[nIndex].RATIO_YX_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::Pinhole)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 3;
                vecSpec[nIndex].SIZE_Y_mm = 3;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].AVG_GV = 220;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
                vecSpec[nIndex].MAX_GV = 200;
                vecSpec[nIndex].MAX_GV_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
            }
        }

        if (vecitrStart->first == DefectType::Dent)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 5;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;

                vecSpec[nIndex].AVG_GV_2 = 100;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
            }
        }

        if (vecitrStart->first == DefectType::Horizontal_Line)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.1;
                vecSpec[nIndex].SIZE_Y_mm = 10;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].AVG_GV_2 = 108;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                vecSpec[nIndex].RATIO_YX = 4;
                vecSpec[nIndex].RATIO_YX_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::Stain)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 3;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].AVG_GV_2 = 108;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::Stain_Pinhole)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 3;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].AVG_GV_2 = 148;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::Island)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].AVG_GV = 20;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_TAB;
            }
        }

        if (vecitrStart->first == DefectType::Chipping_Pinhole)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.8;
                vecSpec[nIndex].SIZE_Y_mm = 0.8;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
            }
        }

        if (vecitrStart->first == DefectType::Drag)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 10;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
            }
        }

        if (vecitrStart->first == DefectType::Edge_Line)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.05;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
            }
        }

        if (vecitrStart->first == DefectType::Balck_Dot)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 3;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::Banking)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 10;
                vecSpec[nIndex].SIZE_Y_mm = 10;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }

        if (vecitrStart->first == DefectType::Delamination_Pinhole)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.1;
                vecSpec[nIndex].SIZE_Y_mm = 0.1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
            }
        }

        if (vecitrStart->first == DefectType::Scratch)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
                vecSpec[nIndex].AVG_GV_2 = 100;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                vecSpec[nIndex].LENGTH_mm = 1.0;
                vecSpec[nIndex].LENGTH_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                vecSpec[nIndex].RATIO_DARK = 99.0;
                vecSpec[nIndex].RATIO_DARK_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
            }
        }

        if (vecitrStart->first == DefectType::Crack)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
                vecSpec[nIndex].AVG_GV_2 = 100;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                vecSpec[nIndex].MIN_GV_2 = 30;
                vecSpec[nIndex].MIN_GV_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
            }
        }
        if (vecitrStart->first == DefectType::Dent)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 20;
                vecSpec[nIndex].SIZE_Y_mm = 20;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
                vecSpec[nIndex].AVG_GV_2 = 120;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
            }
        }

        if (vecitrStart->first == DefectType::Delamination_Crack)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 20;
                vecSpec[nIndex].SIZE_Y_mm = 20;
                vecSpec[nIndex].ORAND = true;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
                vecSpec[nIndex].AVG_GV_2 = 120;
                vecSpec[nIndex].AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
            }
        }

        if (vecitrStart->first == DefectType::Plain_Wrinkle)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_TAB;
                vecSpec[nIndex].nInspectImage = enInspectionImageType::Dark;
                vecSpec[nIndex].INSPECTION_IMAGE_TYPE = enInspectionImageType::Dark;
            }
        }

        if (vecitrStart->first == DefectType::Edge_Defect)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
            }
        }

        if (vecitrStart->first == DefectType::ETC_Defect)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].ORAND = false;
                vecSpec[nIndex].nInspectionArea = enInspectionArea::IA_ELECTRODE;
            }
        }
#endif // INSPECTION_MACHINE_UC1

        if (vecitrStart->first == DefectType::SmallDefect_CraterLevel1)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1;
                vecSpec[nIndex].SIZE_Y_mm = 1;
                vecSpec[nIndex].COUNT = 15;
                vecSpec[nIndex].ORAND = (long)enCode::CD_OR;
                vecSpec[nIndex].PARENT = enParent::Crater;
                vecSpec[nIndex].PARENT_COMPARE_OPERATOR = enParent::Crater;
                vecSpec[nIndex].PRIORITY = 30;
            }
        }
        if (vecitrStart->first == DefectType::SmallDefect_CraterLevel2)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 8;
                vecSpec[nIndex].SIZE_Y_mm = 8;
                vecSpec[nIndex].COUNT = 10;
                vecSpec[nIndex].ORAND = (long)enCode::CD_OR;
                vecSpec[nIndex].PARENT = enParent::Crater;
                vecSpec[nIndex].PARENT_COMPARE_OPERATOR = enParent::Crater;
                vecSpec[nIndex].PRIORITY = 31;
            }
        }
        if (vecitrStart->first == DefectType::SmallDefect_CraterLevel3)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 10;
                vecSpec[nIndex].SIZE_Y_mm = 10;
                vecSpec[nIndex].COUNT = 5;
                vecSpec[nIndex].ORAND = (long)enCode::CD_OR;
                vecSpec[nIndex].PARENT = enParent::Crater;
                vecSpec[nIndex].PARENT_COMPARE_OPERATOR = enParent::Crater;
                vecSpec[nIndex].PRIORITY = 32;
            }
        }
        if (vecitrStart->first == DefectType::SmallDefect_CraterLevel4)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 15;
                vecSpec[nIndex].SIZE_Y_mm = 15;
                vecSpec[nIndex].COUNT = 3;
                vecSpec[nIndex].ORAND = (long)enCode::CD_OR;
                vecSpec[nIndex].PARENT = enParent::Crater;
                vecSpec[nIndex].PARENT_COMPARE_OPERATOR = enParent::Crater;
                vecSpec[nIndex].PRIORITY = 33;
            }
        }
        if (vecitrStart->first == DefectType::SmallDefect_CraterLevel5)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 15;
                vecSpec[nIndex].SIZE_Y_mm = 15;
                vecSpec[nIndex].COUNT = 3;
                vecSpec[nIndex].ORAND = (long)enCode::CD_OR;
                vecSpec[nIndex].PARENT = enParent::Crater;
                vecSpec[nIndex].PARENT_COMPARE_OPERATOR = enParent::Crater;
                vecSpec[nIndex].PRIORITY = 34;
            }
        }

        if (vecitrStart->first >= DefectType::SmallDefect_PinholeLevel1
            && vecitrStart->first <= DefectType::SmallDefect_PinholeLevel5)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1.0;
                vecSpec[nIndex].SIZE_Y_mm = 1.0;
                vecSpec[nIndex].COUNT = 11;
                vecSpec[nIndex].ORAND = (long)enCode::CD_OR;
                vecSpec[nIndex].PARENT = enParent::Pinhole;
                vecSpec[nIndex].PARENT_COMPARE_OPERATOR = enParent::Pinhole;
                vecSpec[nIndex].PRIORITY = 35;
            }
        }
        if (vecitrStart->first >= DefectType::SmallDefect_DentLevel1
            && vecitrStart->first <= DefectType::SmallDefect_DentLevel5)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1.0;
                vecSpec[nIndex].SIZE_Y_mm = 1.0;
                vecSpec[nIndex].COUNT = 11;
                vecSpec[nIndex].ORAND = (long)enCode::CD_OR;
                vecSpec[nIndex].PARENT = enParent::Dent;
                vecSpec[nIndex].PARENT_COMPARE_OPERATOR = enParent::Dent;
                vecSpec[nIndex].PRIORITY = 40;
            }
        }
        if (vecitrStart->first >= DefectType::SmallDefect_ProtrutionLevel1
            && vecitrStart->first <= DefectType::SmallDefect_ProtrutionLevel5)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.3;
                vecSpec[nIndex].SIZE_Y_mm = 0.3;
                vecSpec[nIndex].COUNT = 11;
                vecSpec[nIndex].ORAND = (long)enCode::CD_OR;
                vecSpec[nIndex].PRIORITY = 45;
            }
        }
        if (vecitrStart->first >= DefectType::SmallDefect_IslandLevel1
            && vecitrStart->first <= DefectType::SmallDefect_IslandLevel5)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 1.0;
                vecSpec[nIndex].SIZE_Y_mm = 1.0;
                vecSpec[nIndex].COUNT = 11;
                vecSpec[nIndex].ORAND = (long)enCode::CD_AND;
                vecSpec[nIndex].PARENT = enParent::Island;
                vecSpec[nIndex].PARENT_COMPARE_OPERATOR = enParent::Island;
                vecSpec[nIndex].PRIORITY = 50;
            }
        }
        if (vecitrStart->first >= DefectType::SmallDefect_ScratchLevel1
            && vecitrStart->first <= DefectType::SmallDefect_ScratchLevel5)
        {
            for (long nIndex = 0; nIndex < vecitrStart->second.size(); nIndex++)
            {
                vecSpec[nIndex].SIZE_X_mm = 0.1;
                vecSpec[nIndex].SIZE_Y_mm = 5;
                vecSpec[nIndex].COUNT = 11;
                vecSpec[nIndex].ORAND = (long)enCode::CD_AND;
                vecSpec[nIndex].PRIORITY = 55;
            }
        }


        /* if (itrStart->first == DefectType::SmallDefect_CraterLevel1)
        {
            spec.SIZE_X_mm = 1;
            spec.SIZE_Y_mm = 1;
            spec.COUNT = 15;
            spec.ORAND = (long)enCode::CD_OR;
            spec.PARENT = enParent::Crater;
            spec.PARENT_COMPARE_OPERATOR = enParent::Crater;

        }
        if (itrStart->first == DefectType::SmallDefect_CraterLevel2)
        {
            spec.SIZE_X_mm = 8;
            spec.SIZE_Y_mm = 8;
            spec.COUNT = 10;
            spec.ORAND = (long)enCode::CD_OR;
            spec.PARENT = enParent::Crater;
            spec.PARENT_COMPARE_OPERATOR = enParent::Crater;
        }
        if (itrStart->first == DefectType::SmallDefect_CraterLevel3)
        {
            spec.SIZE_X_mm = 10;
            spec.SIZE_Y_mm = 10;
            spec.COUNT = 5;
            spec.ORAND = (long)enCode::CD_OR;
            spec.PARENT = enParent::Crater;
            spec.PARENT_COMPARE_OPERATOR = enParent::Crater;
        }
        if (itrStart->first == DefectType::SmallDefect_CraterLevel4)
        {
            spec.SIZE_X_mm = 15;
            spec.SIZE_Y_mm = 15;
            spec.COUNT = 3;
            spec.ORAND = (long)enCode::CD_OR;
            spec.PARENT = enParent::Crater;
            spec.PARENT_COMPARE_OPERATOR = enParent::Crater;
        }
        if (itrStart->first == DefectType::SmallDefect_CraterLevel5)
        {
            spec.SIZE_X_mm = 15;
            spec.SIZE_Y_mm = 15;
            spec.COUNT = 3;
            spec.ORAND = (long)enCode::CD_OR;
            spec.PARENT = enParent::Crater;
            spec.PARENT_COMPARE_OPERATOR = enParent::Crater;
        }
        if (itrStart->first >= DefectType::SmallDefect_PinholeLevel1 && itrStart->first <= DefectType::SmallDefect_PinholeLevel5)
        {
            spec.SIZE_X_mm = 1.0;
            spec.SIZE_Y_mm = 1.0;
            spec.COUNT = 11;
            spec.ORAND = (long)enCode::CD_OR;
            spec.PARENT = enParent::Pinhole;
            spec.PARENT_COMPARE_OPERATOR = enParent::Pinhole;
        }
        if (itrStart->first >= DefectType::SmallDefect_DentLevel1 && itrStart->first <= DefectType::SmallDefect_DentLevel5)
        {
            spec.SIZE_X_mm = 1.0;
            spec.SIZE_Y_mm = 1.0;
            spec.COUNT = 11;
            spec.ORAND = (long)enCode::CD_OR;
            spec.PARENT = enParent::Dent;
            spec.PARENT_COMPARE_OPERATOR = enParent::Dent;
        }
        if (itrStart->first >= DefectType::SmallDefect_ProtrutionLevel1 && itrStart->first <= DefectType::SmallDefect_ProtrutionLevel5)
        {
            spec.SIZE_X_mm = 0.3;
            spec.SIZE_Y_mm = 0.3;
            spec.COUNT = 11;
            spec.ORAND = (long)enCode::CD_OR;
        }
        if (itrStart->first >= DefectType::SmallDefect_IslandLevel1 && itrStart->first <= DefectType::SmallDefect_IslandLevel5)
        {
            spec.SIZE_X_mm = 1.0;
            spec.SIZE_Y_mm = 1.0;
            spec.COUNT = 11;
            spec.ORAND = (long)enCode::CD_AND;
            spec.PARENT = enParent::Island;
            spec.PARENT_COMPARE_OPERATOR = enParent::Island;
        }
        if (itrStart->first >= DefectType::SmallDefect_ScratchLevel1
            && itrStart->first <= DefectType::SmallDefect_ScratchLevel5)
        {
            spec.SIZE_X_mm = 0.1;
            spec.SIZE_Y_mm = 5;
            spec.COUNT = 11;
            spec.ORAND = (long)enCode::CD_AND;
        }*/
        vecitrStart++;

    }
    //기본값좀 추가하자
}

std::vector<DefectType>* CISISpecTable::GetValueTogether(DefectType DT, enColTitle title)
{
    std::map<DefectType, std::vector<CISISpecInfo>>::iterator itrStart, itrEnd;
    itrStart = m_mapSpec_multi.begin();
    itrEnd = m_mapSpec_multi.end();
    while (itrStart != itrEnd)
    {
        if (DT == itrStart->first && DefectType::ISI_END != itrStart->first)
        {
            std::vector<CISISpecInfo>& spec = itrStart->second;
            for (long nValue = 0; nValue < spec.size(); nValue++)
            {

            }
            switch (title)
            {
                case classification::ITEM:
                    break;
                case classification::SIZE_X:
                    break;
                case classification::SIZE_X_CO:
                    break;
                case classification::SIZE_Y:
                    break;
                case classification::SIZE_Y_CO:
                    break;
                case classification::ORAND:
                    break;
                case classification::INSPECTION_IMAGE_TYPE:
                    break;
                case classification::DEFECT_BRIGHTNESS:
                    break;
                case classification::BLOB_THRESHOLD:
                    break;
                case classification::BLOB_THRESHOLD_CO:
                    break;
                case classification::CATEGORY:
                    break;
                case classification::SECTION_THRESHOLD:
                    break;
                case classification::SECTION_THRESHOLD_CO:
                    break;
                case classification::WIDTH:
                    return &spec[0].m_vecTogetherWIDTH;
                    break;
                case classification::WIDTH_CO:
                    break;
                case classification::LENGTH:
                    return &spec[0].m_vecTogetherLENGTH;
                    break;
                case classification::LENGTH_CO:
                    break;
                case classification::MIN:
                    break;
                case classification::MIN_CO:
                    break;
                case classification::AVG:
                    break;
                case classification::AVG_CO:
                    break;
                case classification::MAX:
                    break;
                case classification::MAX_CO:
                    break;
                case classification::AREA_Ratio:
                    return &spec[0].m_vecTogetherAREA_Ratio;
                    break;
                case classification::AREA_Ratio_CO:
                    break;

                case classification::RATIO_YX:
                    return &spec[0].m_vecTogetherRATIO_YX;
                    break;
                case classification::RATIO_YX_CO:
                    break;
                case classification::RATIO_XY:
                    return &spec[0].m_vecTogetherRATIO_XY;
                    break;
                case classification::RATIO_XY_CO:
                    break;
                case classification::RATIO_LW:
                    return &spec[0].m_vecTogetherRATIO_LW;
                    break;
                case classification::RATIO_LW_CO:
                    break;
                case classification::RATIO_WL:
                    return &spec[0].m_vecTogetherRATIO_WL;
                    break;
                case classification::RATIO_WL_CO:
                    break;
                case classification::COMPACT:
                    return &spec[0].m_vecTogetherCOMPACT;
                    break;
                case classification::COMPACT_CO:
                    break;
                case classification::ANGLE:
                    return &spec[0].m_vecTogetherANGLE;
                    break;
                case classification::ANGLE_CO:
                    break;
                case classification::STD:
                    return &spec[0].m_vecTogetherSTD;
                    break;
                case classification::STD_CO:
                    break;
                case classification::INSPECTION_AREA:
                    return &spec[0].m_vecTogetherINSPECTIONAREA;
                    break;
                default:
                    break;
            }
        }
        itrStart++;
    }

    return nullptr;
}

bool CISISpecTable::SetValueTogether(DefectType DT, enColTitle title, float value)
{

    std::map<DefectType, std::vector<CISISpecInfo>>::iterator vecitrStart, vecitrEnd;

    std::vector<DefectType> vecTogether;

    int FIND = 0;
    int APPLY = 1;
    for (int i = 0; i < 2; i++)
    {

        vecitrStart = m_mapSpec_multi.begin();
        vecitrEnd = m_mapSpec_multi.end();

        while (vecitrStart != vecitrEnd)
        {
            if (DefectType::ISI_END != vecitrStart->first)
            {
                BOOL bFindTogether = false;
                for (int n = 0; n < vecTogether.size(); n++)
                {
                    if (vecTogether[n] == vecitrStart->first)
                    {
                        bFindTogether = true;
                        break;
                    }
                    if (vecTogether[n] == vecitrStart->first)
                    {
                        bFindTogether = true;
                        break;
                    }
                }
                std::vector<CISISpecInfo>& vecspec = vecitrStart->second;

               for (long n = 0; n < vecspec.size(); n++)
                {
                    switch (title)
                    {
                        case classification::ITEM:
                            break;
                        case classification::SIZE_X:
                            break;
                        case classification::SIZE_X_CO:
                            break;
                        case classification::SIZE_Y:
                            break;
                        case classification::SIZE_Y_CO:
                            break;
                        case classification::ORAND:
                            break;
                        case classification::INSPECTION_IMAGE_TYPE:
                            break;
                        case classification::DEFECT_BRIGHTNESS:
                            break;
                        case classification::BLOB_THRESHOLD:
                            break;
                        case classification::BLOB_THRESHOLD_CO:
                            break;
                        case classification::CATEGORY:
                            break;
                        case classification::SECTION_THRESHOLD:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherSECTION_THRESHOLD;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].SECTION_THRESHOLD = value;
                            break;
                        case classification::SECTION_THRESHOLD_CO:
                            break;
                        case classification::WIDTH:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherWIDTH;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].WIDTH_mm = value;
                            break;
                        case classification::LENGTH:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherLENGTH;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].LENGTH_mm = value;
                            break;
                        case classification::MIN:
                            break;
                        case classification::AVG:
                            break;
                        case classification::MAX:
                            break;
                        case classification::AREA_Ratio:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherAREA_Ratio;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].AREA_Ratio = value;
                            break;
                        case classification::AREA_Ratio_CO:
                            break;

                        case classification::RATIO_XY:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherRATIO_XY;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].RATIO_XY = value;
                            break;
                        case classification::RATIO_XY_CO:
                            break;
                        case classification::RATIO_YX:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherRATIO_YX;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].RATIO_YX = value;
                            break;
                        case classification::RATIO_YX_CO:
                            break;
                        case classification::RATIO_LW:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherRATIO_LW;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].RATIO_LW = value;
                            break;
                        case classification::RATIO_LW_CO:
                            break;
                        case classification::RATIO_WL:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherRATIO_WL;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].RATIO_WL = value;
                            break;
                        case classification::RATIO_WL_CO:
                            break;
                        case classification::COMPACT:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherCOMPACT;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].COMPACT = value;
                            break;
                        case classification::ANGLE:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherANGLE;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].ANGLE = value;
                            break;
                        case classification::STD:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherSTD;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].STD = value;
                            break;
                        case classification::INSPECTION_AREA:
                            if (i == FIND && DT == vecitrStart->first)
                                vecTogether = vecspec[n].m_vecTogetherINSPECTIONAREA;
                            else if (i == APPLY && true == bFindTogether)
                                vecspec[n].nInspectionArea = value;
                            break;
                        default:
                            break;
                    }
                }
               
            }

            if (vecTogether.size() > 0 && i == FIND)
                break; //

            vecitrStart++;
        }

        if (vecTogether.size() == 0 && i == FIND)
            break; //같이 변경해야할 변수가 없음.
    }

    return true;
}

void CISISpecTable::LinkDataBase(bool bSave, Database& db)
{
    m_bUse.Link(bSave, db[_T("isi classification use")]);

    //저장 로드 기능은 우선 제외

    m_nInspAreaSetSize = m_vecInspAreaSet.size();
    if (true != db[_T("m_nInspAreaSetSize")].Link(bSave, m_nInspAreaSetSize))
        0;
    m_vecInspAreaSet.resize(m_nInspAreaSetSize);
    for (long n = 0; n < m_vecInspAreaSet.size(); n++)
    {
        CString strTemp;
        strTemp.Format(_T("m_nInspAreaSetSize%2d"), n);
        if (true != db[strTemp].Link(bSave, m_vecInspAreaSet[n]))
            0;
    }

    // 장지수 map_multi 함수 저장 부 추가
    
    std::map<DefectType, std::vector<CISISpecInfo>>::iterator vecitrStart, vecitrEnd;
    vecitrStart = m_mapSpec_multi.begin();
    vecitrEnd = m_mapSpec_multi.end();
    std::vector<Database> vecdb;
    
    while (vecitrStart != vecitrEnd)
    {
        if (DefectType::ISI_END != vecitrStart->first)
        {
            CString strDefectName;
            std::vector<CISISpecInfo>& vecspec = vecitrStart->second;
            //m_nMultiSpecsize = vecspec.size();
            long nMultiSpecsize = vecspec.size();
            //spec[0].LinkDataBase_Multi()
            strDefectName.Format(_T("%s_multi"),GetDefectName(vecitrStart->first));
            
            if (true != db[strDefectName].Link(bSave, nMultiSpecsize))
            {
                //CString str;
                //initSpec();
                nMultiSpecsize = 1;
                0;
            }
            BOOL bNewItemAdd = FALSE;
            long nDifferentValue = 0;
            if (vecitrStart->second.size() < nMultiSpecsize)
            {
                bNewItemAdd = TRUE;
                nDifferentValue = nMultiSpecsize - vecitrStart->second.size();

                for (long idx = 0; idx < nDifferentValue; idx++)
                {
                    //CISISpecInfo& spec = vecitrStart->second[0];
                    CISISpecInfo spec(vecitrStart->first, 0, enCompareOperator::BIGGER_THAN_SPEC, 0, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, true, true, true, true,
                        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
                        true, true, true, true, true, true,true,true,true,true,true);
                    vecitrStart->second.push_back(spec);
                }
            }
            else if (vecitrStart->second.size() > nMultiSpecsize)
            {
                nDifferentValue = vecitrStart->second.size() - nMultiSpecsize ;
                for (long n = 0; n < nDifferentValue; n++)
                {
                    vecitrStart->second.pop_back();
                }
                
            }
            
            for (long n = 0; n < nMultiSpecsize; n++)
            {
                CISISpecInfo& spec = vecitrStart->second[n];
                if ((long)vecitrStart->first <= (long)DefectType::END_Coater_Surface && spec.nInspectionArea == enInspectionArea::IA_NONE)
                {  
                    if (vecitrStart->first == DefectType::Banking)
                    {
                        strDefectName.Format(_T("Bangking_%2d"), n);
                    }
                    else if (vecitrStart->first == DefectType::Balck_Dot)
                    {
                        strDefectName.Format(_T("Balck_Dot_%2d"), n);
                    }
                }
                else
                {
                    strDefectName.Format(_T("%s_%2d"), GetDefectName(vecitrStart->first), n);
                }
                spec.LinkDataBase(bSave, db[strDefectName]);
                float fValueX = spec.SIZE_X_mm.GetValue();
                float fValueY = spec.SIZE_X_mm.GetValue();

                if (fValueX == 0 && fValueY == 0)
                {
                    if (vecitrStart->first == DefectType::Banking)
                    {
                        spec.SIZE_X_mm = 10;
                        spec.SIZEX_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                        spec.SIZE_Y_mm = 30;
                        spec.SIZEY_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                        spec.ORAND = true;
                        spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;
                        spec.RATIO_DARK = 90;
                        spec.RATIO_DARK_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                        spec.MIN_GV_2 = 40;
                        spec.MIN_GV_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                        spec.AVG_GV_2 = 80;
                        spec.AVG_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                    }
                    else if (vecitrStart->first == DefectType::Balck_Dot)
                    {
                        spec.SIZE_X_mm = 3;
                        spec.SIZEX_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                        spec.SIZE_Y_mm = 5;
                        spec.SIZEY_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                        spec.ORAND = false;
                        spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;
                    }
                    else if (vecitrStart->first == DefectType::Dent)
                    {
                        spec.PRIORITY = 8;
                        spec.SIZE_X_mm = 5;
                        spec.SIZEX_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                        spec.SIZE_Y_mm = 5;
                        spec.SIZEY_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                        spec.ORAND = false;
                        spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;
                        spec.AVG_GV = 100;
                        spec.AVG_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                        spec.BRIGHT_AVG_GV = 100;
                        spec.BRIGHT_AVG_GV_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                        spec.EDGE_ENERGY_WHITE_2 = 10;
                        spec.WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2 = enCompareOperator::SMALLER_THAN_SPEC;
                        spec.RATIO_WHITE = 50;
                        spec.RATIO_WHITE_COMPARE_OPERATOR = enCompareOperator::BIGGER_THAN_SPEC;
                    }
                }


#if INSPECTION_MACHINE_UC1

#else
                if (fValueX == 0 && fValueY == 0)
                {
                    strDefectName.Format(_T("%s_%2d"), GetDefectName_Before(vecitrStart->first), n);
                    spec.LinkDataBase(bSave, db[strDefectName]);
                    float fValueX = spec.SIZE_X_mm.GetValue();
                    float fValueY = spec.SIZE_X_mm.GetValue();
                }
#endif  

                

            }
        }
        vecitrStart++;
    }

    // Load 및 Save 가 끝난 뒤, Sort 를 진행
    //std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec(m_mapSpec_multi.begin(), m_mapSpec_multi.end());
    //std::sort(vecSpec.begin(), vecSpec.end(), compare_multi);

}

void CISISpecTable::SetSpecInfo(DefectType defectName, float fSizeX, float fSizeY, bool ORAND)
{
    CISISpecInfo* spec = GetSpecInfo(defectName);

    if (nullptr != spec)
    {
        spec->m_bShareSpec = true;

        /*if (DefectType::Crack == defectName)
		{
			spec->LENGTH_mm = fSizeX;
			spec->WIDTH_mm = fSizeY;
		}
		else
		{*/
        spec->SIZE_X_mm = fSizeX;
        spec->SIZE_Y_mm = fSizeY;
        //}

        spec->ORAND = ORAND;
    }
}

CISISpecInfo* CISISpecTable::GetSpecInfo(DefectType defectName, long nCondition)
{
    std::map<DefectType, std::vector<CISISpecInfo>>::iterator itr = m_mapSpec_multi.find(defectName);

    if (itr == m_mapSpec_multi.end())
        return nullptr;

    return &itr->second[nCondition];
}

std::vector<CISISpecInfo>* CISISpecTable::GetSpecInfo_Multi(DefectType defectName)
{
    std::map<DefectType, std::vector<CISISpecInfo>>::iterator itr = m_mapSpec_multi.find(defectName);

    if (itr == m_mapSpec_multi.end())
        return nullptr;

    return &itr->second;
}

long CISISpecTable::GetSpecCount()
{
    return m_mapSpec_multi.size();
}

void CISISpecTable::FromHost(const Share::SpecData& data, DefectType defecttype)
{
    std::map<DefectType, std::vector<CISISpecInfo>>::iterator itrStart, itrEnd;
    itrStart = m_mapSpec_multi.begin();
    itrEnd = m_mapSpec_multi.end();

    while (itrStart != itrEnd)
    {
        std::vector<CISISpecInfo>& spec = itrStart->second;

        if (itrStart->first == TranslateDefectType(defecttype))
        {
            for (long nVal = 0; nVal < spec.size(); nVal++)
            {
                itrStart->second[nVal].SIZE_X_mm = data.m_dDefectSizeX;
                itrStart->second[nVal].SIZE_Y_mm = data.m_dDefectSizeY;
            }
            
        }
    }
}

DefectType CISISpecTable::TranslateDefectType(const DefectType Hostdefecttype)
{
    return Hostdefecttype;

    switch (Hostdefecttype)
    {
#if INSPECTION_MACHINE_UC1

#else
        case DefectType::Line:
            break;

        case DefectType::Pinhole:
            break;

        case DefectType::Delamination_Crack:
            break;
        case DefectType::Stain_Pinhole:
            break;
#endif
        

            // 다크성 불량
        case DefectType::Dent:
            break;

        case DefectType::Scratch:
            break;


        case DefectType::Crack:
            break;

            // 주름 불량
        case DefectType::CoatingWrinkle:
            break;

        case DefectType::HeatingWrinkle:
            break;

            //탭 불량
        case DefectType::InsulationIsland:
            break;

        case DefectType::Wrinkle:
            break;

        case DefectType::ConnectionTape:
            break;

        case DefectType::Scrab:
            break;

        case DefectType::NoneCoating:
            break;

        case DefectType::Dross:
            break;

        case DefectType::Spatter:
            break;
    }
}
