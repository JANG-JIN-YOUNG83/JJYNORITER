#pragma once

#include "basedef.h"
#include <vector>
#include "CISIDefectProperty.h"
#include "CISICellAllDefect.h"
#include "CISISpecTable.h"

using namespace classification;


class CISIClassification
{
public:
	CISIClassification::CISIClassification(CISISpecTable* pSpecInfo);

	// 함수
    void Start(CISICellAllDefect* pCAD, BOOL bView);

	bool SingleLogic(int index, CISICellAllDefect* cellDefect);
	bool isBackGroundBurr(CISIDefectProperty* pDefect);
	bool isScrab(const int index, CISICellAllDefect* cellDefect);
	bool isTab(const int index, CISICellAllDefect* cellDefect);
	bool isInsulation(const int index, CISICellAllDefect* cellDefect);
	bool isElectrodeEdge(CISIDefectProperty* pDefect);
	bool isElectrode(const int index, CISICellAllDefect* cellDefect);

	//장지수 분류 Logic 함수 추가
    bool MultiLogic(int index, CISICellAllDefect* cellDefect, std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec, BOOL bView,
        std::vector<std::pair<enLogicResult (*)(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView), DefectType>> fp);

    bool MultiLogic_VecVec(int index, CISICellAllDefect* cellDefect, std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec, BOOL bView,
        std::vector<std::vector<std::vector<std::pair<enLogicResult (*)(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView), DefectType>>>> fp);
    

	II_RESULT_VALUE isConnectTape(CISIDefectProperty* pDefect);

	void isFinalJudge(CISICellAllDefect* cellDefect);
    void isSmallDeect(CISICellAllDefect* cellDefect);

	enParent DefectType2Parent(DefectType type);

	static enLogicResult isSize(CISIDefectProperty* pDefect, CISISpecInfo* pSpec, bool bView = false);
	static enLogicResult isDefectBrightness(enDefectBrightness SpecBrightness, CISIDefectProperty* pDefect);
	static enLogicResult isInspImageBrightness(enInspectionImageType SpecInspImageBrightness, CISIDefectProperty* pDefect);
	static enLogicResult isNearElectrodeEdge(IPVM::Rect rtCell_pixel, CISIDefectProperty* pDefect, float fSpecNear_mm, float fResolution_X_mm);
	static enLogicResult isCategory(enCategory nSpecCategory, CISIDefectProperty* pDefect);

	bool CheckAttachedROI(const BOOL bROIDirLeft, const IPVM::Rect rtInspROI, const CRect DefectRoI);
	//승만
	//총 13개 있어야함
    static enLogicResult Area_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult AVG_GV_CompareOperator(long nSpecAVG, CISIDefectProperty* pDefect);
	

	static enLogicResult Width_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Length_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioLW_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioWL_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioYX_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioXY_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);

	static enLogicResult Angle_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult STD_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Compact_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);

    static enLogicResult AVG_GV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
	static enLogicResult MAX_GV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult MIN_GV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
	static enLogicResult Bright_AVG_GV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Dark_AVG_GV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult SectionTh_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);

	static enLogicResult SizeX_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult SizeY_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);

	// [sm]
    static enLogicResult RatioWhite_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioDark_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult EdgeEnergy_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult EdgeEnergyBlack_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult EdgeEnergyWhite_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioArea_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult MassCenterGV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult OverPxCount_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult HOR_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult VER_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioHV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioVH_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);

	static enLogicResult Dis2Edge_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Dis2Edge_2_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);

    static enLogicResult IsNearEdge_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);

    //2024.10.21
    static enLogicResult EdgeEnergySharpness_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult EdgeEnergySharpnessWhite_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult EdgeEnergySharpnessBlack_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
	// 장지수 _2 Operator 함수 추가
	static enLogicResult RatioArea_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Width_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Length_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioWL_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioLW_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioYX_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioXY_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Angle_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult STD_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult AVG_GV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult MIN_GV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult MAX_GV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Compact_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult SectionTh_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioWhite_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioDark_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult EdgeEnergy_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult EdgeEnergyBlack_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult EdgeEnergyWhite_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Area_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult MassCenterGV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult HOR_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult VER_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioHV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult RatioVH_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult OverPxCount_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Dis2Edge_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Dis2Edge_2_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult IsNearEdge_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Bright_AVG_GV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult Dark_AVG_GV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult EdgeEnergySharpness_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult EdgeEnergySharpnessWhite_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
    static enLogicResult EdgeEnergySharpnessBlack_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView = false);
	II_RESULT_VALUE Judge(DefectType defectname, enLogicResult LogisRet, CISIDefectProperty* pDefect);

	DefectType CheckSmallDefect(DefectType defectname, CISIDefectProperty* pDefect);

	template <typename T>
	static enLogicResult isCompareOperator(enCompareOperator CompareOperator, const T Spec, const T Defect);

	CISISpecTable* m_pSpecInfo;

	float m_fLogicTime;
};

