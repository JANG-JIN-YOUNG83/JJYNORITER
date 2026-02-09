#pragma once

#include "CISIDefectProperty.h"
#include "../SharedModule/CommonShare/DefectDef.h"
#include "../CompanySharedLibrary/Persistence/Database.h"

#include "../VisionModule/Base/ParaBool.h"
#include "../VisionModule/Base/ParaFloat.h"
#include "../VisionModule/Base/ParaLong.h"
//승만
class CISISpecInfo : public CISIDefectProperty
{
	void init();

	//이거는 각 프로퍼티에 사용 유무임
	bool m_bSize;
	bool m_bAREA;
    bool m_bAREA_Ratio;
	bool m_bWIDTH;
	bool m_bLENGTH;
	bool m_bRATIO_WL;
	bool m_bRATIO_LW;
	bool m_bRATIO_YX;
	bool m_bRATIO_XY;
	bool m_bANGLE;
	bool m_bSTD;
	bool m_bAVG;
	bool m_bMIN_GV;
	bool m_bMAX_GV;
	bool m_bBRIGHT_AVG_GV;
	bool m_bDARK_AVG_GV;
	bool m_bCOMPACT;
	bool m_bSECTION_THRESHOLD;
	bool m_bCOUNT;
    bool m_bPRIORITY;
	bool m_bINSEPCTION_AREA; // 예진

	//[sm]
	bool m_bRATIO_WHITE;
	bool m_bRATIO_DARK;
	bool m_bEDGE_ENERGY;
    bool m_bEDGE_ENERGY_BLACK;
    bool m_bEDGE_ENERGY_WHITE;
	//bool m_bRATIO_AREA;
	bool m_bMASS_CENTER_GV_GV;
	bool m_bHOR;
	bool m_bVER;
	bool m_bRATIO_HV;
	bool m_bRATIO_VH;
    bool m_bSMALL_INSP;
    bool m_bParent;

	bool m_bDis2Edge;
    bool m_bDis2Edge_2;
    bool m_bIsNearEdge;
	bool m_bOverGVpxCount;

	//JJS 추가
    bool m_bEDGE_ENERGY_SHARPNESS;
    bool m_bEDGE_ENERGY_SHARPNESS_WHITE;
    bool m_bEDGE_ENERGY_SHARPNESS_BLACK;

	bool m_bMEDIAN_GV;
    bool m_bSTDEV_GV;
	DefectType m_DT;

public:
	BOOL m_bShareSpec;

	std::vector<DefectType> m_vecTogetherAREA;
	std::vector<DefectType> m_vecTogetherAREA_Ratio;
	std::vector<DefectType> m_vecTogetherWIDTH;
	std::vector<DefectType> m_vecTogetherLENGTH;
	std::vector<DefectType> m_vecTogetherRATIO_YX;
	std::vector<DefectType> m_vecTogetherRATIO_XY;
	std::vector<DefectType> m_vecTogetherRATIO_WL;
	std::vector<DefectType> m_vecTogetherRATIO_LW;
	std::vector<DefectType> m_vecTogetherANGLE;
	std::vector<DefectType> m_vecTogetherSTD;
	std::vector<DefectType> m_vecTogetherAVG;
	std::vector<DefectType> m_vecTogetherMIN_GV;
	std::vector<DefectType> m_vecTogetherMAX_GV;
	std::vector<DefectType> m_vecTogetherBRIGHT_AVG_GV;
	std::vector<DefectType> m_vecTogetherDARK_AVG_GV;
	std::vector<DefectType> m_vecTogetherCOMPACT;
	std::vector<DefectType> m_vecTogetherSECTION_THRESHOLD;
	std::vector<DefectType> m_vecTogetherINSPECTIONAREA;
	std::vector<DefectType> m_vecTogetherINSPECTION_IMAGE_TYPE;
	std::vector<DefectType> m_vecTogetherDEFECT_BRIGHTNESS;




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
		//[sm]
		bool bRATIO_WHITE,
		bool bRATIO_DARK,
		bool bEDGE_ENERGY, 
		bool bEDGE_ENERGY_BLACK, 
		bool bEDGE_ENERGY_WHITE,
		bool bArea_mm,
		bool bMASS_CENTER_GV_GV,
		bool bHOR,
		bool bVER,
		bool bRATIO_HV,
		bool bRATIO_VH,
		bool OVERPXCOUNT, 
		bool bDis2Edge_mm,
		bool bDis2Edge_2_mm,
		bool bIsNearEdge,
		bool bEDGE_ENERGY_SHARPNESS,
        bool bEDGE_ENERGY_SHARPNESS_WHITE,
		bool bEDGE_ENERGY_SHARPNESS_BLACK,
		bool m_bMEDIAN_GV,
		bool m_bSTDEV_GV);

	CISISpecInfo& operator=(const CISISpecInfo& rhs)
	{
        return *this;
	};

	CISISpecInfo& operator=(const CISIDefectProperty& rhs)
	{
		nInspectImage = rhs.nInspectImage;
		nDefectBrightness = rhs.nDefectBrightness;
		nInspectionArea = rhs.nInspectionArea;
		nCategory = rhs.nCategory;
		rtRect_pixel = rhs.rtRect_pixel;
		SIZE_X_mm = rhs.SIZE_X_mm;
		SIZE_Y_mm = rhs.SIZE_Y_mm;
		AREA = rhs.AREA;
		AREA_Ratio = rhs.AREA_Ratio;
		WIDTH_mm = rhs.WIDTH_mm;
		LENGTH_mm = rhs.LENGTH_mm;
		RATIO_WL = rhs.RATIO_WL;
		RATIO_LW = rhs.RATIO_LW;
		RATIO_YX = rhs.RATIO_YX;
		RATIO_XY = rhs.RATIO_XY;
		ANGLE = rhs.ANGLE;
		STD = rhs.STD;
		COMPACT = rhs.COMPACT;
		AVG_GV = rhs.AVG_GV;
		MIN_GV = rhs.MIN_GV;
		MAX_GV = rhs.MAX_GV;
		BRIGHT_AVG_GV = rhs.BRIGHT_AVG_GV;
		DARK_AVG_GV = rhs.DARK_AVG_GV;

		SMALL_INSP = rhs.SMALL_INSP;
        SMALL_INSP_LEVEL = rhs.SMALL_INSP_LEVEL;

		OVER_200_GV_PIXEL_COUNT = rhs.OVER_200_GV_PIXEL_COUNT;

		DISTANCE_TO_EDGE_mm = rhs.DISTANCE_TO_EDGE_mm;
        DISTANCE_TO_EDGE_2_mm = rhs.DISTANCE_TO_EDGE_2_mm;
        IS_NEAR_EDGE = rhs.IS_NEAR_EDGE;

        PARENT = rhs.PARENT;

		AREA = rhs.AREA;

		PRIORITY = rhs.PRIORITY;
		RATIO_WHITE = rhs.RATIO_WHITE;
		RATIO_DARK = rhs.RATIO_DARK;
		EDGE_ENERGY = rhs.EDGE_ENERGY;
        EDGE_ENERGY_BLACK = rhs.EDGE_ENERGY_BLACK;
        EDGE_ENERGY_WHITE = rhs.EDGE_ENERGY_WHITE;
		//RATIO_AREA = rhs.RATIO_AREA;
		MASS_CENTER_GV_GV = rhs.MASS_CENTER_GV_GV;
		HOR = rhs.HOR;
		VER = rhs.VER;
		RATIO_HV = rhs.RATIO_HV;
		RATIO_VH  = rhs.RATIO_VH;
        EDGE_ENERGY_SHARPNESS = rhs.EDGE_ENERGY_SHARPNESS;
        EDGE_ENERGY_SHARPNESS_WHITE = rhs.EDGE_ENERGY_SHARPNESS_WHITE;
        EDGE_ENERGY_SHARPNESS_BLACK = rhs.EDGE_ENERGY_SHARPNESS_BLACK;

		MEDIAN_GV = rhs.MEDIAN_GV;
        STDEV_GV = rhs.STDEV_GV;
		//_2 변수 추가
        AREA_2 = rhs.AREA_2;
        AREA_Ratio = rhs.AREA_Ratio_2;
        WIDTH_mm_2 = rhs.WIDTH_mm_2;
        LENGTH_mm_2 = rhs.LENGTH_mm_2;
        RATIO_WL_2 = rhs.RATIO_WL_2;
        RATIO_LW_2 = rhs.RATIO_LW_2;
        RATIO_YX_2 = rhs.RATIO_YX_2;
        RATIO_XY_2 = rhs.RATIO_XY_2;
        ANGLE_2 = rhs.ANGLE_2;
        STD_2 = rhs.STD_2;
        COMPACT_2 = rhs.COMPACT_2;
        AVG_GV_2 = rhs.AVG_GV_2;
        MIN_GV_2 = rhs.MIN_GV_2;
        MAX_GV_2 = rhs.MAX_GV_2;
        BRIGHT_AVG_GV_2 = rhs.BRIGHT_AVG_GV_2;
        DARK_AVG_GV_2 = rhs.DARK_AVG_GV_2;

        SMALL_INSP_2 = rhs.SMALL_INSP_2;
        SMALL_INSP_LEVEL_2 = rhs.SMALL_INSP_LEVEL_2;

        OVER_200_GV_PIXEL_COUNT_2 = rhs.OVER_200_GV_PIXEL_COUNT_2;

        DISTANCE_TO_EDGE_mm_2 = rhs.DISTANCE_TO_EDGE_mm_2;
        DISTANCE_TO_EDGE_2_mm_2 = rhs.DISTANCE_TO_EDGE_2_mm_2;
        IS_NEAR_EDGE_2 = rhs.IS_NEAR_EDGE_2;

        RATIO_WHITE_2 = rhs.RATIO_WHITE_2;
        RATIO_DARK_2 = rhs.RATIO_DARK_2;
        EDGE_ENERGY_2 = rhs.EDGE_ENERGY_2;
        EDGE_ENERGY_BLACK_2 = rhs.EDGE_ENERGY_BLACK_2;
        EDGE_ENERGY_WHITE_2 = rhs.EDGE_ENERGY_WHITE_2;
        //RATIO_AREA = rhs.RATIO_AREA;
        MASS_CENTER_GV_GV_2 = rhs.MASS_CENTER_GV_GV_2;
        HOR_2 = rhs.HOR_2;
        VER_2 = rhs.VER_2;
        RATIO_HV_2 = rhs.RATIO_HV_2;
        RATIO_VH_2 = rhs.RATIO_VH_2;

		EDGE_ENERGY_SHARPNESS_2 = rhs.EDGE_ENERGY_SHARPNESS_2;
        EDGE_ENERGY_SHARPNESS_WHITE_2 = rhs.EDGE_ENERGY_SHARPNESS_WHITE_2;
        EDGE_ENERGY_SHARPNESS_BLACK_2 = rhs.EDGE_ENERGY_SHARPNESS_BLACK_2;



		return *this;
	};
	// 현식 use 를 또 넣어야 겠지요? 
	//승만
	Base::ParaBool  USE;
	Base::ParaBool  ENABLE;
    Base::ParaLong	PRIORITY;
	Base::ParaBool  ORAND;			//<<< or : false, and : true
	Base::ParaLong  SIZEX_COMPARE_OPERATOR;
	Base::ParaLong  SIZEY_COMPARE_OPERATOR;

	Base::ParaLong  AREA_RATIO_COMPARE_OPERATOR;
	Base::ParaLong  WIDTH_COMPARE_OPERATOR;
	Base::ParaLong  LENGTH_COMPARE_OPERATOR;
	Base::ParaLong  RATIO_WL_COMPARE_OPERATOR;
	Base::ParaLong  RATIO_LW_COMPARE_OPERATOR;
	Base::ParaLong  RATIO_YX_COMPARE_OPERATOR;
	Base::ParaLong  RATIO_XY_COMPARE_OPERATOR;
	Base::ParaLong  ANGLE_COMPARE_OPERATOR;
	Base::ParaLong  STD_COMPARE_OPERATOR;
	Base::ParaLong  AVG_COMPARE_OPERATOR;
	Base::ParaLong  MIN_GV_COMPARE_OPERATOR;
	Base::ParaLong  MAX_GV_COMPARE_OPERATOR;
	Base::ParaLong  BRIGHT_AVG_GV_COMPARE_OPERATOR;
	Base::ParaLong  DARK_AVG_GV_COMPARE_OPERATOR;
	Base::ParaLong  COMPACT_COMPARE_OPERATOR;
	Base::ParaLong  SECTION_THRESHOLD_COMPARE_OPERATOR;
	Base::ParaLong	SECTION_AREA_COMPARE_OPERATOR;

	Base::ParaLong	INSPECTION_IMAGE_TYPE; // 예진

	Base::ParaFloat  MINIMUM_SIZEX_mm;
	Base::ParaFloat  MINIMUM_SIZEY_mm;

	Base::ParaLong  COUNT;

	Base::ParaLong PARENT_COMPARE_OPERATOR;

	Base::ParaLong AREA_COMPARE_OPERATOR;

	// [sm]
    Base::ParaLong RATIO_WHITE_COMPARE_OPERATOR;
    Base::ParaLong RATIO_DARK_COMPARE_OPERATOR;
    Base::ParaLong EDGE_ENERGY_COMPARE_OPERATOR;
    Base::ParaLong BLACK_EDGE_ENERGY_COMPARE_OPERATOR;
    Base::ParaLong WHITE_EDGE_ENERGY_COMPARE_OPERATOR;
    //Base::ParaLong RATIO_AREA_COMPARE_OPERATOR;
    Base::ParaLong MASS_CENTER_GV_GV_COMPARE_OPERATOR;
    Base::ParaLong HOR_COMPARE_OPERATOR;
    Base::ParaLong VER_COMPARE_OPERATOR;
    Base::ParaLong RATIO_HV_COMPARE_OPERATOR;
    Base::ParaLong RATIO_VH_COMPARE_OPERATOR;

    Base::ParaLong OVER_GV_PX_COUNT_COMPARE_OPERATOR;

    Base::ParaLong DIS_TO_EDGE_mm_COMPARE_OPERATOR;
    Base::ParaLong DIS_TO_EDGE_2_mm_COMPARE_OPERATOR;

    //Base::ParaLong IS_NEAR_EDGE;
    Base::ParaLong IS_NEAR_EDGE_COMPARE_OPERATOR;

	//2024.10.21 EdgeSharpness 추가
    Base::ParaLong SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR;
    Base::ParaLong WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR;
    Base::ParaLong BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR;

	//장지수 _2 변수 추가
    Base::ParaLong AREA_RATIO_COMPARE_OPERATOR_2;
    Base::ParaLong WIDTH_COMPARE_OPERATOR_2;
    Base::ParaLong LENGTH_COMPARE_OPERATOR_2;
    Base::ParaLong RATIO_WL_COMPARE_OPERATOR_2;
    Base::ParaLong RATIO_LW_COMPARE_OPERATOR_2;
    Base::ParaLong RATIO_YX_COMPARE_OPERATOR_2;
    Base::ParaLong RATIO_XY_COMPARE_OPERATOR_2;
    Base::ParaLong ANGLE_COMPARE_OPERATOR_2;
    Base::ParaLong STD_COMPARE_OPERATOR_2;
    Base::ParaLong AVG_COMPARE_OPERATOR_2;
    Base::ParaLong MIN_GV_COMPARE_OPERATOR_2;
    Base::ParaLong MAX_GV_COMPARE_OPERATOR_2;
    Base::ParaLong BRIGHT_AVG_GV_COMPARE_OPERATOR_2;
    Base::ParaLong DARK_AVG_GV_COMPARE_OPERATOR_2;
    Base::ParaLong COMPACT_COMPARE_OPERATOR_2;
    Base::ParaLong SECTION_THRESHOLD_COMPARE_OPERATOR_2;
    Base::ParaLong SECTION_AREA_COMPARE_OPERATOR_2;

	Base::ParaLong AREA_COMPARE_OPERATOR_2;

    // [sm]
    Base::ParaLong RATIO_WHITE_COMPARE_OPERATOR_2;
    Base::ParaLong RATIO_DARK_COMPARE_OPERATOR_2;
    Base::ParaLong EDGE_ENERGY_COMPARE_OPERATOR_2;
    Base::ParaLong BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2;
    Base::ParaLong WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2;
    //Base::ParaLong RATIO_AREA_COMPARE_OPERATOR;
    Base::ParaLong MASS_CENTER_GV_GV_COMPARE_OPERATOR_2;
    Base::ParaLong HOR_COMPARE_OPERATOR_2;
    Base::ParaLong VER_COMPARE_OPERATOR_2;
    Base::ParaLong RATIO_HV_COMPARE_OPERATOR_2;
    Base::ParaLong RATIO_VH_COMPARE_OPERATOR_2;

    Base::ParaLong OVER_GV_PX_COUNT_COMPARE_OPERATOR_2;

	Base::ParaLong DIS_TO_EDGE_mm_COMPARE_OPERATOR_2;
    Base::ParaLong DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2;
    Base::ParaLong IS_NEAR_EDGE_COMPARE_OPERATOR_2;

	Base::ParaLong SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2;
    Base::ParaLong WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2;
    Base::ParaLong BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2;
	//
	std::vector<enInspectionArea> vecTabInfo;

	void LinkDataBase(bool bSave, Database& db);
	bool GetUseUse() { return USE; }
	bool GetEnable() { return ENABLE; };
	DefectType GetDefectType() { return m_DT; };
	bool GetUseSize() { return m_bSize; };
    bool GetUseArea() { return m_bAREA;  };
	bool GetUseArea_RAtio() { return m_bAREA_Ratio; };
	bool GetUseWidth() { return m_bWIDTH; };
	bool GetUseLength() { return m_bLENGTH; };
	bool GetUseRatioLW() { return m_bRATIO_LW; };
	bool GetUseRatioWL() { return m_bRATIO_WL; };
	bool GetUseRatioYX() { return m_bRATIO_YX; };
	bool GetUseRatioXY() { return m_bRATIO_XY; };
	bool GetUseAngle() { return m_bANGLE; };
	bool GetUseStd() { return m_bSTD; };
	bool GetUseAvgGV() { return m_bAVG; };
	bool GetUseMinGV() { return m_bMIN_GV; };
	bool GetUseMaxGV() { return m_bMAX_GV; };
	bool GetUseBrightAgvGV() { return m_bBRIGHT_AVG_GV; };
	bool GetUseDarkAvgGV() { return m_bDARK_AVG_GV; };
	bool GetUseCompact() { return m_bCOMPACT; };
	bool GetUseSectionThreshold() { return m_bSECTION_THRESHOLD; };
	bool GetUseOverGVPxCount() { return m_bOverGVpxCount; };
	bool GetUseCount() { return m_bCOUNT; };
    bool GetUseParent() { return m_bParent; }
	bool GetUsePriority() { return m_bPRIORITY; }
	// [sm]
	bool GetUseHor() { return m_bHOR; };
	bool GetUseVer() { return m_bVER; };
	bool GetUseRatioHV() { return m_bRATIO_HV; };
	bool GetUseRatioWhite() { return m_bRATIO_WHITE; };
	bool GetUseRatioDark() { return m_bRATIO_DARK; };
	bool GetUseEdgeEnergy() { return m_bEDGE_ENERGY; };
	bool GetUseEdgeEnergyBlack() { return m_bEDGE_ENERGY_BLACK; };
	bool GetUseEdgeEnergyWhite() { return m_bEDGE_ENERGY_WHITE; };
	//bool GetUseRatioArea() { return m_bRATIO_AREA; };
	bool GetUseMassCenterGV() { return m_bMASS_CENTER_GV_GV; };
	bool GetUseRatioVH() { return m_bRATIO_VH; };
	bool GetUseEdgeEnergySharpness() { return m_bEDGE_ENERGY_SHARPNESS; };
	bool GetUseEdgeEnergySharpnessWhite() { return m_bEDGE_ENERGY_SHARPNESS_WHITE; };
	bool GetUseEdgeEnergySharpnessBlack() { return m_bEDGE_ENERGY_SHARPNESS_BLACK; };
	bool GetUseDis2Edge()
    {
        return m_bDis2Edge;
    };
    bool GetUseDis2Edge_2() {return m_bDis2Edge_2; };
    bool GetUseIsNearEdge() {return m_bIsNearEdge;}

};

