#include "stdafx.h"
#include "basedef.h"
#include "CISISpecInfo.h"

#include "CISIDefectProperty.h"

//승만
CISISpecInfo::CISISpecInfo(DefectType DT, float fSizeX, enCompareOperator SizeXCode, float fSizeY, enCompareOperator SizeYCode, enCode CD_OR,
	bool bSize,
	bool bArea_Ratio, 
	bool bSMALL_INSP,
	bool bPARENT,
	bool bWIDTH,
	bool bLENGTH,
	bool bRATIO_WL,
	bool bRATIO_LW,
	bool bRATIO_YX,
	bool bRATIO_XY,
	bool bANGLE,
	bool bSTD,
	bool bAVG,
	bool bMIN_GV,
	bool bMAX_GV,
	bool bBRIGHT_AVG_GV,
	bool bDARK_AVG_GV,
	bool bCOMPACT,
	bool bSECTION_THRESHOLD,
	bool bCount,
	bool bPriority,
	bool bRATIO_WHITE,
	bool bRATIO_DARK,
	bool bEDGE_ENERGY,
	bool bEDGE_ENERGY_BLACK,
	bool bEDGE_ENERGY_WHITE,
	bool bAREA,
	bool bMASS_CENTER_GV_GV,
	bool bHOR,
	bool bVER,
	bool bRATIO_HV,
	bool bRATIO_VH,
	bool OVERPXCOUNT, 
	bool bDIS2EDGE,
    bool bDIS2EDGE_2,
	bool bIsNearEdge,
	bool bEdge_ENERGY_SHARPNESS,
	bool EDGE_ENERGY_SHARPNESS_WHITE,
	bool EDGE_ENERGY_SHARPNESS_BLACK,
	bool bMEDIAN_GV,
	bool bSTDEV_GV)
	: ORAND(_T("ORAND"), 0)
	, USE(_T("USE"), 1)
	, ENABLE(_T("ENABLE"), 1)
	, SIZEX_COMPARE_OPERATOR(_T("SIZEX_COMPARE_OPERATOR"), 0)
	, SIZEY_COMPARE_OPERATOR(_T("SIZEY_COMPARE_OPERATOR"), 0)
	, AREA_RATIO_COMPARE_OPERATOR(_T("AREA_COMPARE_OPERATOR"), 0)
	, WIDTH_COMPARE_OPERATOR(_T("WIDTH_COMPARE_OPERATOR"), 0)
	, LENGTH_COMPARE_OPERATOR(_T("LENGTH_COMPARE_OPERATOR"), 0)
	, RATIO_WL_COMPARE_OPERATOR(_T("RATIO_WL_COMPARE_OPERATOR"), 0)
	, RATIO_LW_COMPARE_OPERATOR(_T("RATIO_LW_COMPARE_OPERATOR"), 0)
	, RATIO_YX_COMPARE_OPERATOR(_T("RATIO_YX_COMPARE_OPERATOR"), 0)
	, RATIO_XY_COMPARE_OPERATOR(_T("RATIO_XY_COMPARE_OPERATOR"), 0)
	, ANGLE_COMPARE_OPERATOR(_T("ANGLE_COMPARE_OPERATOR"), 0)
	, STD_COMPARE_OPERATOR(_T("STD_COMPARE_OPERATOR"), 0)
	, AVG_COMPARE_OPERATOR(_T("AVG_COMPARE_OPERATOR"), 0)
	, MIN_GV_COMPARE_OPERATOR(_T("MIN_GV_COMPARE_OPERATOR"), 0)
	, MAX_GV_COMPARE_OPERATOR(_T("MAX_GV_COMPARE_OPERATOR"), 0)
	, BRIGHT_AVG_GV_COMPARE_OPERATOR(_T("BRIGHT_AVG_GV_COMPARE_OPERATOR"), 0)
	, DARK_AVG_GV_COMPARE_OPERATOR(_T("DARK_AVG_GV_COMPARE_OPERATOR"), 0)
	, COMPACT_COMPARE_OPERATOR(_T("COMPACT_COMPARE_OPERATOR"), 0)
	, SECTION_THRESHOLD_COMPARE_OPERATOR(_T("SECTION_THRESHOLD_COMPARE_OPERATOR"), 0)
	, SECTION_AREA_COMPARE_OPERATOR(_T("SECTION_AREA_COMPARE_OPERATOR"), 0)
	, RATIO_WHITE_COMPARE_OPERATOR(_T("RATIO_WHITE_COMPARE_OPERATOR"), 0) // [sm]
	, RATIO_DARK_COMPARE_OPERATOR(_T("RATIO_DARK_COMPARE_OPERATOR"), 0)
	, EDGE_ENERGY_COMPARE_OPERATOR(_T("EDGE_ENERGY_COMPARE_OPERATOR"), 0)
	//, RATIO_AREA_COMPARE_OPERATOR(_T("RATIO_AREA_COMPARE_OPERATOR"), 0)
	, MASS_CENTER_GV_GV_COMPARE_OPERATOR(_T("MASS_CENTER_GV_GV_COMPARE_OPERATOR"), 0)
	, HOR_COMPARE_OPERATOR(_T("HOR_COMPARE_OPERATOR"), 0)
	, VER_COMPARE_OPERATOR(_T("VER_COMPARE_OPERATOR"), 0)
	, RATIO_HV_COMPARE_OPERATOR(_T("RATIO_HV_COMPARE_OPERATOR"), 0)
	, RATIO_VH_COMPARE_OPERATOR(_T("RATIO_VH_COMPARE_OPERATOR"), 0)
	, INSPECTION_IMAGE_TYPE(_T("INSPECTION_IMAGE_TYPE"), 0) // 예진
    , PARENT_COMPARE_OPERATOR(_T("PARENT_COMPARE_OPERATOR"), 0)
    , AREA_COMPARE_OPERATOR(_T("AREA_COMPARE_OPERATOR"), 0)
    , BLACK_EDGE_ENERGY_COMPARE_OPERATOR(_T("BLACK_EDGE_ENERGY_COMPARE_OPERATOR"), 0)
    , WHITE_EDGE_ENERGY_COMPARE_OPERATOR(_T("WHITE_EDGE_ENERGY_COMPARE_OPERATOR"), 0)
	, m_bAREA(bAREA)
	, m_bWIDTH(bWIDTH)
	, m_bLENGTH(bLENGTH)
	, m_bRATIO_WL(bRATIO_WL)
	, m_bRATIO_LW(bRATIO_LW)
	, m_bRATIO_YX(bRATIO_YX)
	, m_bRATIO_XY(bRATIO_XY)
	, m_bANGLE(bANGLE)
	, m_bSTD(bSTD)
	, m_bAVG(bAVG)
	, m_bMIN_GV(bMIN_GV)
	, m_bMAX_GV(bMAX_GV)
	, m_bBRIGHT_AVG_GV(bBRIGHT_AVG_GV)
	, m_bDARK_AVG_GV(bDARK_AVG_GV)
	, m_bCOMPACT(bCOMPACT)
	, m_bSECTION_THRESHOLD(bSECTION_THRESHOLD)
	, m_bCOUNT(bCount)
    , m_bPRIORITY(bPriority)
	, MINIMUM_SIZEX_mm(_T("MINIMUM_SIZEX_mm"), 0.1)
	, MINIMUM_SIZEY_mm(_T("MINIMUM_SIZEY_mm"), 0.1)
	, COUNT(_T("COUNT"), 10)
    , PRIORITY(_T("PRIORITY"), 0)
	, m_bSize(bSize)
    , m_bMEDIAN_GV(bMEDIAN_GV)
    , m_bSTDEV_GV(bSTDEV_GV)
	// [sm]
	, m_bRATIO_WHITE(bRATIO_WHITE)
	, m_bRATIO_DARK(bRATIO_DARK)
	, m_bEDGE_ENERGY(bEDGE_ENERGY)
    , m_bAREA_Ratio(bArea_Ratio)
	//, m_bRATIO_AREA(bRATIO_AREA)
	, m_bMASS_CENTER_GV_GV(bMASS_CENTER_GV_GV)
	, m_bHOR(bHOR)
	, m_bVER(bVER)
	, m_bRATIO_HV(bRATIO_HV)
	, m_bRATIO_VH(bRATIO_VH)
    , m_bSMALL_INSP(bSMALL_INSP)
    , m_bParent(bPARENT)
    , m_bOverGVpxCount(OVERPXCOUNT)
    , OVER_GV_PX_COUNT_COMPARE_OPERATOR(_T("OVER_GV_PX_COUNT_COMPARE_OPERATOR"), 0)
    , m_bDis2Edge(bDIS2EDGE)
    , m_bDis2Edge_2(bDIS2EDGE_2)
    , DIS_TO_EDGE_mm_COMPARE_OPERATOR(_T("DIS_TO_EDGE_mm_COMPARE_OPERATOR"), 0)
    , DIS_TO_EDGE_2_mm_COMPARE_OPERATOR(_T("DIS_TO_EDGE_2_mm_COMPARE_OPERATOR"), 0)
    , m_bEDGE_ENERGY_BLACK(bEDGE_ENERGY_BLACK)
    , m_bEDGE_ENERGY_WHITE(bEDGE_ENERGY_WHITE)
    , m_bIsNearEdge(bIsNearEdge) // , IS_NEAR_EDGE(_T("IS_NEAR_EDGE"), 0)   
    , IS_NEAR_EDGE_COMPARE_OPERATOR(_T("IS_NEAR_EDGE_COMPARE_OPERATOR"), 0)
    , SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR(_T("SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR"), 0)
    , WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR(_T("WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR"), 0)
    , BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR(_T("BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR"), 0)
	// 장지수 _2 변수 추가
    , AREA_RATIO_COMPARE_OPERATOR_2(_T("AREA_COMPARE_OPERATOR_2"), 0)
    , WIDTH_COMPARE_OPERATOR_2(_T("WIDTH_COMPARE_OPERATOR_2"), 0)
    , LENGTH_COMPARE_OPERATOR_2(_T("LENGTH_COMPARE_OPERATOR_2"), 0)
    , RATIO_WL_COMPARE_OPERATOR_2(_T("RATIO_WL_COMPARE_OPERATOR_2"), 0)
    , RATIO_LW_COMPARE_OPERATOR_2(_T("RATIO_LW_COMPARE_OPERATOR_2"), 0)
    , RATIO_YX_COMPARE_OPERATOR_2(_T("RATIO_YX_COMPARE_OPERATOR_2"), 0)
    , RATIO_XY_COMPARE_OPERATOR_2(_T("RATIO_XY_COMPARE_OPERATOR_2"), 0)
    , ANGLE_COMPARE_OPERATOR_2(_T("ANGLE_COMPARE_OPERATOR_2"), 0)
    , STD_COMPARE_OPERATOR_2(_T("STD_COMPARE_OPERATOR_2"), 0)
    , AVG_COMPARE_OPERATOR_2(_T("AVG_COMPARE_OPERATOR_2"), 0)
    , MIN_GV_COMPARE_OPERATOR_2(_T("MIN_GV_COMPARE_OPERATOR_2"), 0)
    , MAX_GV_COMPARE_OPERATOR_2(_T("MAX_GV_COMPARE_OPERATOR_2"), 0)
    , BRIGHT_AVG_GV_COMPARE_OPERATOR_2(_T("BRIGHT_AVG_GV_COMPARE_OPERATOR_2"), 0)
    , DARK_AVG_GV_COMPARE_OPERATOR_2(_T("DARK_AVG_GV_COMPARE_OPERATOR_2"), 0)
    , COMPACT_COMPARE_OPERATOR_2(_T("COMPACT_COMPARE_OPERATOR_2"), 0)
    , SECTION_THRESHOLD_COMPARE_OPERATOR_2(_T("SECTION_THRESHOLD_COMPARE_OPERATOR_2"), 0)
    , SECTION_AREA_COMPARE_OPERATOR_2(_T("SECTION_AREA_COMPARE_OPERATOR_2"), 0)
    , RATIO_WHITE_COMPARE_OPERATOR_2(_T("RATIO_WHITE_COMPARE_OPERATOR_2"), 0) // [sm]
    , RATIO_DARK_COMPARE_OPERATOR_2(_T("RATIO_DARK_COMPARE_OPERATOR_2"), 0)
    , EDGE_ENERGY_COMPARE_OPERATOR_2(_T("EDGE_ENERGY_COMPARE_OPERATOR_2"), 0)
    //, RATIO_AREA_COMPARE_OPERATOR(_T("RATIO_AREA_COMPARE_OPERATOR"), 0)
    , MASS_CENTER_GV_GV_COMPARE_OPERATOR_2(_T("MASS_CENTER_GV_GV_COMPARE_OPERATOR_2"), 0)
    , HOR_COMPARE_OPERATOR_2(_T("HOR_COMPARE_OPERATOR_2"), 0)
    , VER_COMPARE_OPERATOR_2(_T("VER_COMPARE_OPERATOR_2"), 0)
    , RATIO_HV_COMPARE_OPERATOR_2(_T("RATIO_HV_COMPARE_OPERATOR_2"), 0)
    , RATIO_VH_COMPARE_OPERATOR_2(_T("RATIO_VH_COMPARE_OPERATOR_2"), 0)
    , AREA_COMPARE_OPERATOR_2(_T("AREA_COMPARE_OPERATOR_2"), 0)
    , BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2(_T("BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2"), 0)
    , WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2(_T("WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2"), 0)
    , OVER_GV_PX_COUNT_COMPARE_OPERATOR_2(_T("OVER_GV_PX_COUNT_COMPARE_OPERATOR_2"), 0)
    , DIS_TO_EDGE_mm_COMPARE_OPERATOR_2(_T("DIS_TO_EDGE_mm_COMPARE_OPERATOR_2"), 0)
    , DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2(_T("DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2"), 0)
    , IS_NEAR_EDGE_COMPARE_OPERATOR_2(_T("IS_NEAR_EDGE_COMPARE_OPERATOR_2"), 0)
    , SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2(_T("SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2"), 0)
    , WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2(_T("WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2"), 0)
    , BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2(_T("BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2"), 0)
{
	SIZE_X_mm = fSizeX;
	SIZEX_COMPARE_OPERATOR = SizeXCode;
	SIZE_Y_mm = fSizeY;
	SIZEY_COMPARE_OPERATOR = SizeYCode;
	ORAND = (bool)CD_OR;

	m_DT = DT;

	//탭 검사 항목 등록
	switch (DT)
    {
#if INSPECTION_MACHINE_UC1
        case DefectType::White_Line:
        case DefectType::White_Pinhole:
        case DefectType::Dark_Line:
        case DefectType::Dark_Crater:
        case DefectType::Crater:
        case DefectType::Foil_Island:
        case DefectType::Edge_ChippingPinhole:
        case DefectType::Edge_Drag:
        case DefectType::Delami:
        case DefectType::Foil_Defect:
        case DefectType::Protrusion:

#else
        case DefectType::Line:
        case DefectType::Pinhole:
        case DefectType::Horizontal_Line:
        case DefectType::Stain:
        case DefectType::Stain_Pinhole:
        case DefectType::Island:
        case DefectType::Chipping_Pinhole:
        case DefectType::Drag:
        case DefectType::Delamination_Crack:
        case DefectType::Plain_Wrinkle:
        case DefectType::Delamination_Pinhole:
#endif // INSPECTION_MACHINE_UC1 
    
    case DefectType::Dent:
   
    
    case DefectType::Edge_Line:
    case DefectType::Balck_Dot:
    case DefectType::Scratch:
    case DefectType::Crack:
    
    case DefectType::Edge_Defect:
    case DefectType::ETC_Defect:
    case DefectType::Banking:
    
	case DefectType::SmallDefect_CraterLevel5:
	case DefectType::SmallDefect_CraterLevel4:
	case DefectType::SmallDefect_CraterLevel3:
	case DefectType::SmallDefect_CraterLevel2:
	case DefectType::SmallDefect_CraterLevel1:
	case DefectType::SmallDefect_PinholeLevel1:
	case DefectType::SmallDefect_PinholeLevel2:
	case DefectType::SmallDefect_PinholeLevel3:
	case DefectType::SmallDefect_PinholeLevel4:
	case DefectType::SmallDefect_PinholeLevel5:
    case DefectType::SmallDefect_DentLevel1:
    case DefectType::SmallDefect_DentLevel2:
    case DefectType::SmallDefect_DentLevel3:
    case DefectType::SmallDefect_DentLevel4:
    case DefectType::SmallDefect_DentLevel5:
	case DefectType::SmallDefect_ProtrutionLevel1:
	case DefectType::SmallDefect_ProtrutionLevel2:
	case DefectType::SmallDefect_ProtrutionLevel3:
	case DefectType::SmallDefect_ProtrutionLevel4:
	case DefectType::SmallDefect_ProtrutionLevel5:
    case DefectType::SmallDefect_IslandLevel1:
    case DefectType::SmallDefect_IslandLevel2:
    case DefectType::SmallDefect_IslandLevel3:
    case DefectType::SmallDefect_IslandLevel4:
    case DefectType::SmallDefect_IslandLevel5:
    case DefectType::SmallDefect_ScratchLevel1:
    case DefectType::SmallDefect_ScratchLevel2:
    case DefectType::SmallDefect_ScratchLevel3:
    case DefectType::SmallDefect_ScratchLevel4:
    case DefectType::SmallDefect_ScratchLevel5:
		vecTabInfo.push_back(enInspectionArea::IA_TAB);
		vecTabInfo.push_back(enInspectionArea::IA_TAB_INSULATION);
		vecTabInfo.push_back(enInspectionArea::IA_ELECTRODE);
		vecTabInfo.push_back(enInspectionArea::IA_ELECTRODE_EDGE);
		vecTabInfo.push_back(enInspectionArea::IA_ELECTRODE_EDGE_LEFT);
		vecTabInfo.push_back(enInspectionArea::IA_ELECTRODE_EDGE_RIGHT);

	default:
		break;
	}

}

void CISISpecInfo::init()
{
	m_bShareSpec = false;

	SIZEX_COMPARE_OPERATOR = classification::enCompareOperator::BIGGER_THAN_SPEC;
	SIZEY_COMPARE_OPERATOR = classification::enCompareOperator::BIGGER_THAN_SPEC;

	AREA_RATIO_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	WIDTH_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	LENGTH_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	RATIO_WL_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	RATIO_LW_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	ANGLE_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	STD_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	AVG_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	MIN_GV_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	MAX_GV_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	BRIGHT_AVG_GV_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	DARK_AVG_GV_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	COMPACT_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	SECTION_THRESHOLD_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;

	AREA_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	// [sm]
	RATIO_WHITE_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	RATIO_DARK_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	EDGE_ENERGY_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    BLACK_EDGE_ENERGY_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    WHITE_EDGE_ENERGY_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	//RATIO_AREA_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	MASS_CENTER_GV_GV_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	HOR_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	VER_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	RATIO_HV_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	RATIO_VH_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    OVER_GV_PX_COUNT_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    DIS_TO_EDGE_mm_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    DIS_TO_EDGE_2_mm_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;

	MINIMUM_SIZEX_mm = 0.1;
	MINIMUM_SIZEY_mm = 0.1;

	COUNT = 10;

	SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
	// _2 변수 추가
    AREA_RATIO_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    WIDTH_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    LENGTH_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    RATIO_WL_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    RATIO_LW_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    ANGLE_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    STD_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    AVG_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    MIN_GV_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    MAX_GV_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    BRIGHT_AVG_GV_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    DARK_AVG_GV_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    COMPACT_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    SECTION_THRESHOLD_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;

    AREA_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    // [sm]
    RATIO_WHITE_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    RATIO_DARK_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    EDGE_ENERGY_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    //RATIO_AREA_COMPARE_OPERATOR = classification::enCompareOperator::NOT_USE;
    MASS_CENTER_GV_GV_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    HOR_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    VER_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    RATIO_HV_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    RATIO_VH_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    OVER_GV_PX_COUNT_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    DIS_TO_EDGE_mm_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
    SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 = classification::enCompareOperator::NOT_USE;
	ENABLE = true;
}


void CISISpecInfo::LinkDataBase(bool bSave, Database& db)
{
	//사용자 설정 스펙
	USE.Link(bSave, db[_T("USE")]);
	ENABLE.Link(bSave, db[_T("ENABLE")]);

	ORAND.Link(bSave, db[_T("ORAND")]);
	SIZE_X_mm.Link(bSave, db[_T("SIZE_X_mm")]);
	SIZE_Y_mm.Link(bSave, db[_T("SIZE_Y_mm")]);
	SIZEX_COMPARE_OPERATOR.Link(bSave, db[_T("SIZEX_COMPARE_OPERATOR")]);
	SIZEY_COMPARE_OPERATOR.Link(bSave, db[_T("SIZEY_COMPARE_OPERATOR")]);

	BLOB_THRESHOLD.Link(bSave, db[_T("BLOB_THRESHOLD")]);
	SECTION_THRESHOLD.Link(bSave, db[_T("SECTION_THRESHOLD")]);

	INSPECTION_IMAGE_TYPE.Link(bSave, db[_T("INSPECTION_IMAGE_TYPE")]);

	nDefectBrightness.Link(bSave, db[_T("nDfectBrightness")]);
	SECTION_AREA_COMPARE_OPERATOR.Link(bSave, db[_T("SECTION_AREA_COMPATE_OPERATOR")]);
	nCategory.Link(bSave, db[_T("nCategory")]);
	nInspectionArea.Link(bSave, db[_T("nInspectionArea")]);

	SMALL_INSP.Link(bSave, db[_T("SMALL_INSP")]);
    SMALL_INSP_LEVEL.Link(bSave, db[_T("SMALL_INSP_LEVEL")]); 

	PARENT.Link(bSave, db[_T("PARENT")]);
    PARENT_COMPARE_OPERATOR.Link(bSave, db[_T("PARENT_COMPARE_OPERATOR")]);

	PRIORITY.Link(bSave, db[_T("PRIORITY")]);

	//아래 내용은 절대 저장하면 안됨 // 현식 플어야 할 가능성 있음...
	AREA_RATIO_COMPARE_OPERATOR.Link(bSave, db[_T("AREA_RATIO_COMPARE_OPERATOR")]);
	WIDTH_COMPARE_OPERATOR.Link(bSave, db[_T("WIDTH_COMPARE_OPERATOR")]);
	LENGTH_COMPARE_OPERATOR.Link(bSave, db[_T("LENGTH_COMPARE_OPERATOR")]);
	RATIO_WL_COMPARE_OPERATOR.Link(bSave, db[_T("RATIO_WL_COMPARE_OPERATOR")]);
	RATIO_LW_COMPARE_OPERATOR.Link(bSave, db[_T("RATIO_LW_COMPARE_OPERATOR")]);
	ANGLE_COMPARE_OPERATOR.Link(bSave, db[_T("ANGLE_COMPARE_OPERATOR")]);
	STD_COMPARE_OPERATOR.Link(bSave, db[_T("STD_COMPARE_OPERATOR")]);
	AVG_COMPARE_OPERATOR.Link(bSave, db[_T("AVG_COMPARE_OPERATOR")]);
	MIN_GV_COMPARE_OPERATOR.Link(bSave, db[_T("MIN_GV_COMPARE_OPERATOR")]);
	MAX_GV_COMPARE_OPERATOR.Link(bSave, db[_T("MAX_GV_COMPARE_OPERATOR")]);
	BRIGHT_AVG_GV_COMPARE_OPERATOR.Link(bSave, db[_T("BRIGHT_AVG_GV_COMPARE_OPERATOR")]);
	DARK_AVG_GV_COMPARE_OPERATOR.Link(bSave, db[_T("DARK_AVG_GV_COMPARE_OPERATOR")]);
	COMPACT_COMPARE_OPERATOR.Link(bSave, db[_T("COMPACT_COMPARE_OPERATOR")]);
	SECTION_THRESHOLD_COMPARE_OPERATOR.Link(bSave, db[_T("SECTION_THRESHOLD_COMPARE_OPERATOR")]);
	
	AREA_COMPARE_OPERATOR.Link(bSave, db[_T("AREA_COMPARE_OPERATOR")]);

		

	// [sm]
	RATIO_WHITE_COMPARE_OPERATOR.Link(bSave, db[_T("RATIO_WHITE_COMPARE_OPERATOR")]);
	RATIO_DARK_COMPARE_OPERATOR.Link(bSave, db[_T("RATIO_DARK_COMPARE_OPERATOR")]);
	EDGE_ENERGY_COMPARE_OPERATOR.Link(bSave, db[_T("EDGE_ENERGY_COMPARE_OPERATOR")]);
	BLACK_EDGE_ENERGY_COMPARE_OPERATOR.Link(bSave, db[_T("BLACK_EDGE_ENERGY_COMPARE_OPERATOR")]);
	WHITE_EDGE_ENERGY_COMPARE_OPERATOR.Link(bSave, db[_T("WHITE_EDGE_ENERGY_COMPARE_OPERATOR")]);
	//RATIO_AREA_COMPARE_OPERATOR.Link(bSave, db[_T("RATIO_AREA_COMPARE_OPERATOR")]);
	MASS_CENTER_GV_GV_COMPARE_OPERATOR.Link(bSave, db[_T("MASS_CENTER_GV_GV_COMPARE_OPERATOR")]);
	HOR_COMPARE_OPERATOR.Link(bSave, db[_T("HOR_COMPARE_OPERATOR")]);
	VER_COMPARE_OPERATOR.Link(bSave, db[_T("VER_COMPARE_OPERATOR")]);
	RATIO_HV_COMPARE_OPERATOR.Link(bSave, db[_T("RATIO_HV_COMPARE_OPERATOR")]);
	RATIO_VH_COMPARE_OPERATOR.Link(bSave, db[_T("RATIO_VH_COMPARE_OPERATOR")]);
	OVER_GV_PX_COUNT_COMPARE_OPERATOR.Link(bSave, db[_T("OVER_GV_PX_COUNT_COMPARE_OPERATOR")]);
    DIS_TO_EDGE_mm_COMPARE_OPERATOR.Link(bSave, db[_T("DIS_TO_EDGE_mm_COMPARE_OPERATOR")]);
    DIS_TO_EDGE_2_mm_COMPARE_OPERATOR.Link(bSave, db[_T("DIS_TO_EDGE_2_mm_COMPARE_OPERATOR")]);
    IS_NEAR_EDGE_COMPARE_OPERATOR.Link(bSave, db[_T("IS_NEAR_EDGE_COMPARE_OPERATOR")]);
    SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.Link(bSave, db[_T("SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR")]);

	/*nInspectImage.Link(bSave, db[_T("nInspectImage")]);
	nDefectBrightness.Link(bSave, db[_T("nDefectBrightness")]);
	rtRect_pixel.Link(bSave, db[_T("rtRect_pixel")]);*/

	AREA.Link(bSave, db[_T("AREA")]);
	AREA_Ratio.Link(bSave, db[_T("AREA_Ratio")]);
	WIDTH_mm.Link(bSave, db[_T("WIDTH_mm")]);
	LENGTH_mm.Link(bSave, db[_T("LENGTH_mm")]);
	RATIO_WL.Link(bSave, db[_T("RATIO_WL")]);
	RATIO_LW.Link(bSave, db[_T("RATIO_LW")]);
	ANGLE.Link(bSave, db[_T("ANGLE")]);
	STD.Link(bSave, db[_T("STD")]);
	COMPACT.Link(bSave, db[_T("COMPACT")]);
	AVG_GV.Link(bSave, db[_T("AVG_GV")]);
	MIN_GV.Link(bSave, db[_T("MIN_GV")]);
	MAX_GV.Link(bSave, db[_T("MAX_GV")]);
	BRIGHT_AVG_GV.Link(bSave, db[_T("BRIGHT_AVG_GV")]);
	DARK_AVG_GV.Link(bSave, db[_T("DARK_AVG_GV")]);

	MINIMUM_SIZEX_mm.Link(bSave, db[_T("MINIMUM_SIZEX_mm")]);
	MINIMUM_SIZEY_mm.Link(bSave, db[_T("MINIMUM_SIZEY_mm")]);
	
	RATIO_WHITE.Link(bSave, db[_T("RATIO_WHITE")]);
	RATIO_DARK.Link(bSave, db[_T("RATIO_DARK")]);
	EDGE_ENERGY.Link(bSave, db[_T("EDGE_ENERGY")]);
    EDGE_ENERGY_BLACK.Link(bSave, db[_T("EDGE_ENERGY_BLACK")]);
    EDGE_ENERGY_WHITE.Link(bSave, db[_T("EDGE_ENERGY_WHITE")]);
	
	MASS_CENTER_GV_GV.Link(bSave, db[_T("MASS_CENTER_GV_GV")]);
	HOR.Link(bSave, db[_T("HOR")]);
	VER.Link(bSave, db[_T("VER")]);
	RATIO_HV.Link(bSave, db[_T("RATIO_HV")]);
	RATIO_VH.Link(bSave, db[_T("RATIO_VH")]);
	COUNT.Link(bSave, db[_T("COUNT")]);
    OVER_200_GV_PIXEL_COUNT.Link(bSave, db[_T("OVER_GV_PIXEL_COUNT")]);
    DISTANCE_TO_EDGE_mm.Link(bSave, db[_T("DISTANCE_TO_EDGE_mm")]);
    IS_NEAR_EDGE.Link(bSave, db[_T("IS_NEAR_EDGE")]);
    EDGE_ENERGY_SHARPNESS.Link(bSave, db[_T("EDGE_ENERGY_SHARPNESS")]);
    EDGE_ENERGY_SHARPNESS_WHITE.Link(bSave, db[_T("EDGE_ENERGY_SHARPNESS_WHITE")]);
    EDGE_ENERGY_SHARPNESS_BLACK.Link(bSave, db[_T("EDGE_ENERGY_SHARPNESS_BLACK")]);
	// _2 변수 추가
    AREA_2.Link(bSave, db[_T("AREA_2")]);
    AREA_Ratio_2.Link(bSave, db[_T("AREA_Ratio_2")]);
    WIDTH_mm_2.Link(bSave, db[_T("WIDTH_mm_2")]);
    LENGTH_mm_2.Link(bSave, db[_T("LENGTH_mm_2")]);
    RATIO_WL_2.Link(bSave, db[_T("RATIO_WL_2")]);
    RATIO_LW_2.Link(bSave, db[_T("RATIO_LW_2")]);
    ANGLE_2.Link(bSave, db[_T("ANGLE_2")]);
    STD_2.Link(bSave, db[_T("STD_2")]);
    COMPACT_2.Link(bSave, db[_T("COMPACT_2")]);
    AVG_GV_2.Link(bSave, db[_T("AVG_GV_2")]);
    MIN_GV_2.Link(bSave, db[_T("MIN_GV_2")]);
    MAX_GV_2.Link(bSave, db[_T("MAX_GV_2")]);
    BRIGHT_AVG_GV_2.Link(bSave, db[_T("BRIGHT_AVG_GV_2")]);
    DARK_AVG_GV_2.Link(bSave, db[_T("DARK_AVG_GV_2")]);

    RATIO_WHITE_2.Link(bSave, db[_T("RATIO_WHITE_2")]);
    RATIO_DARK_2.Link(bSave, db[_T("RATIO_DARK_2")]);
    EDGE_ENERGY_2.Link(bSave, db[_T("EDGE_ENERGY_2")]);
    EDGE_ENERGY_BLACK_2.Link(bSave, db[_T("EDGE_ENERGY_BLACK_2")]);
    EDGE_ENERGY_WHITE_2.Link(bSave, db[_T("EDGE_ENERGY_WHITE_2")]);

    MASS_CENTER_GV_GV_2.Link(bSave, db[_T("MASS_CENTER_GV_GV_2")]);
    HOR_2.Link(bSave, db[_T("HOR_2")]);
    VER_2.Link(bSave, db[_T("VER_2")]);
    RATIO_HV_2.Link(bSave, db[_T("RATIO_HV_2")]);
    RATIO_VH_2.Link(bSave, db[_T("RATIO_VH_2")]);
    DISTANCE_TO_EDGE_mm_2.Link(bSave, db[_T("DISTANCE_TO_EDGE_mm_2")]);
    IS_NEAR_EDGE_2.Link(bSave, db[_T("IS_NEAR_EDGE_2")]);
    SECTION_THRESHOLD_2.Link(bSave, db[_T("SECTION_THRESHOLD_2")]);
    EDGE_ENERGY_SHARPNESS_2.Link(bSave, db[_T("EDGE_ENERGY_SHARPNESS_2")]);

	SECTION_AREA_COMPARE_OPERATOR_2.Link(bSave, db[_T("SECTION_AREA_COMPATE_OPERATOR_2")]);

	AREA_RATIO_COMPARE_OPERATOR_2.Link(bSave, db[_T("AREA_RATIO_COMPARE_OPERATOR_2")]);
    WIDTH_COMPARE_OPERATOR_2.Link(bSave, db[_T("WIDTH_COMPARE_OPERATOR_2")]);
    LENGTH_COMPARE_OPERATOR_2.Link(bSave, db[_T("LENGTH_COMPARE_OPERATOR_2")]);
    RATIO_WL_COMPARE_OPERATOR_2.Link(bSave, db[_T("RATIO_WL_COMPARE_OPERATOR_2")]);
    RATIO_LW_COMPARE_OPERATOR_2.Link(bSave, db[_T("RATIO_LW_COMPARE_OPERATOR_2")]);
    ANGLE_COMPARE_OPERATOR_2.Link(bSave, db[_T("ANGLE_COMPARE_OPERATOR_2")]);
    STD_COMPARE_OPERATOR_2.Link(bSave, db[_T("STD_COMPARE_OPERATOR_2")]);
    AVG_COMPARE_OPERATOR_2.Link(bSave, db[_T("AVG_COMPARE_OPERATOR_2")]);
    MIN_GV_COMPARE_OPERATOR_2.Link(bSave, db[_T("MIN_GV_COMPARE_OPERATOR_2")]);
    MAX_GV_COMPARE_OPERATOR_2.Link(bSave, db[_T("MAX_GV_COMPARE_OPERATOR_2")]);
    BRIGHT_AVG_GV_COMPARE_OPERATOR_2.Link(bSave, db[_T("BRIGHT_AVG_GV_COMPARE_OPERATOR_2")]);
    DARK_AVG_GV_COMPARE_OPERATOR_2.Link(bSave, db[_T("DARK_AVG_GV_COMPARE_OPERATOR_2")]);
    COMPACT_COMPARE_OPERATOR_2.Link(bSave, db[_T("COMPACT_COMPARE_OPERATOR_2")]);
    SECTION_THRESHOLD_COMPARE_OPERATOR_2.Link(bSave, db[_T("SECTION_THRESHOLD_COMPARE_OPERATOR_2")]);

    AREA_COMPARE_OPERATOR_2.Link(bSave, db[_T("AREA_COMPARE_OPERATOR_2")]);
    SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.Link(bSave, db[_T("SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2")]);
	// [sm]
    RATIO_WHITE_COMPARE_OPERATOR_2.Link(bSave, db[_T("RATIO_WHITE_COMPARE_OPERATOR_2")]);
    RATIO_DARK_COMPARE_OPERATOR_2.Link(bSave, db[_T("RATIO_DARK_COMPARE_OPERATOR_2")]);
    EDGE_ENERGY_COMPARE_OPERATOR_2.Link(bSave, db[_T("EDGE_ENERGY_COMPARE_OPERATOR_2")]);
    BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2.Link(bSave, db[_T("BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2")]);
    WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2.Link(bSave, db[_T("WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2")]);
    //RATIO_AREA_COMPARE_OPERATOR_2.Link(bSave, db[_T("RATIO_AREA_COMPARE_OPERATOR_2")]);
    MASS_CENTER_GV_GV_COMPARE_OPERATOR_2.Link(bSave, db[_T("MASS_CENTER_GV_GV_COMPARE_OPERATOR_2")]);
    HOR_COMPARE_OPERATOR_2.Link(bSave, db[_T("HOR_COMPARE_OPERATOR_2")]);
    VER_COMPARE_OPERATOR_2.Link(bSave, db[_T("VER_COMPARE_OPERATOR_2")]);
    RATIO_HV_COMPARE_OPERATOR_2.Link(bSave, db[_T("RATIO_HV_COMPARE_OPERATOR_2")]);
    RATIO_VH_COMPARE_OPERATOR_2.Link(bSave, db[_T("RATIO_VH_COMPARE_OPERATOR_2")]);
    OVER_GV_PX_COUNT_COMPARE_OPERATOR_2.Link(bSave, db[_T("OVER_GV_PX_COUNT_COMPARE_OPERATOR_2")]);
    DIS_TO_EDGE_mm_COMPARE_OPERATOR_2.Link(bSave, db[_T("DIS_TO_EDGE_mm_COMPARE_OPERATOR_2")]);
    DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2.Link(bSave, db[_T("DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2")]);
    IS_NEAR_EDGE_COMPARE_OPERATOR_2.Link(bSave, db[_T("IS_NEAR_EDGE_COMPARE_OPERATOR_2")]);
    EDGE_ENERGY_SHARPNESS_WHITE_2.Link(bSave, db[_T("EDGE_ENERGY_SHARPNESS_WHITE_2")]);
    EDGE_ENERGY_SHARPNESS_BLACK_2.Link(bSave, db[_T("EDGE_ENERGY_SHARPNESS_BLACK_2")]);
}