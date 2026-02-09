#include "stdafx.h"
#include "basedef.h"
#include "CISIClassification.h"
#include "CISICellAllDefect.h"
#include "Gadget/TimeCheck.h"

#include "../SharedModule/CommonShare/DefectDef.h"
#include "Types/Rect.h"
#include "../SharedModule/CommonShare/DefectDef.h"

template<typename T>
enLogicResult CISIClassification::isCompareOperator(enCompareOperator CompareOperator, const T Spec, const T Defect)
{
    switch (CompareOperator)
    {
        case enCompareOperator::SAME:
            if (Spec == Defect)
                return enLogicResult::YES;
            break;

        case enCompareOperator::BIGGER_THAN_SPEC:
            if (Spec <= Defect)
                return enLogicResult::YES;
            break;

        case enCompareOperator::SMALLER_THAN_SPEC:
            if (Spec > Defect)
                return enLogicResult::YES;
            break;

        default:
            ASSERT(1);
            break;
    }

    return enLogicResult::NO;
}

enLogicResult CISIClassification::RatioArea_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;
    /*if (pSpec->AREA_Ratio <= pDefect->AREA_Ratio)
        result= enLogicResult::YES;*/
    result = isCompareOperator<float>((enCompareOperator)pSpec->AREA_RATIO_COMPARE_OPERATOR.GetValue(), pSpec->AREA_Ratio, pDefect->AREA_Ratio);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->AREA_Ratio.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->AREA_RATIO_COMPARE_OPERATOR.GetValue()),
            pDefect->AREA_Ratio.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioArea, result, strText);
    }

    return result;
}

bool compare_multi(std::pair<DefectType, std::vector<CISISpecInfo>> a, std::pair<DefectType, std::vector<CISISpecInfo>> b)
{
    IPVM::TimeCheck tc;
    float ftime = 0.f;
    tc.Reset();
    if (a.second.size() == 0 || b.second.size() == 0)
    {
        ftime = tc.Elapsed_ms();
        return true;
    }
    if (a.second[0].PRIORITY < b.second[0].PRIORITY)
    {
        ftime = tc.Elapsed_ms();
        return true;
    }
    else
    {
        ftime = tc.Elapsed_ms();
        return false;
    }
}

CISIClassification::CISIClassification(CISISpecTable* pSpecInfo)
{
	if (nullptr == pSpecInfo)
		ASSERT(1);

	m_pSpecInfo = pSpecInfo;
	m_fLogicTime = 0.0f;
}

void CISIClassification::Start(CISICellAllDefect* pCAD /*cell all defect*/, BOOL bView)
{
	IPVM::TimeCheck tc;
	pCAD->ResetResult();
	bool bSingleLogicProcess = false;
	int index = 0;

	//std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec(m_pSpecInfo->m_mapSpec_multi.begin(), m_pSpecInfo->m_mapSpec_multi.end());
 //   std::sort(vecSpec.begin(), vecSpec.end(), compare_multi);
	//std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec(m_pSpecInfo->m_mapSpec_multi.begin(), m_pSpecInfo->m_mapSpec_multi.end());
    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec(m_pSpecInfo->m_mapSpec_multi.begin(), m_pSpecInfo->m_mapSpec_multi.find(DefectType::SmallDefect_CraterLevel1));
    
	std::sort(vecSpec.begin(), vecSpec.end(), compare_multi);

    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>>::iterator vecitrStartVec, vecitrEndVec;
    vecitrStartVec = vecSpec.begin();
    vecitrEndVec = vecSpec.end();
    
    std::vector<std::vector<std::vector<std::pair<enLogicResult (*)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView), DefectType>>>> m_vecvecvecCompareOperator;

    m_vecvecvecCompareOperator.clear();

    long nIndex = 0;
    while (vecitrStartVec != vecitrEndVec)
    {
        m_vecvecvecCompareOperator.resize(vecSpec.size());
        std::vector<CISISpecInfo>* vecpSpec = m_pSpecInfo->GetSpecInfo_Multi(vecitrStartVec->first);
        for (long n = 0; n < vecitrStartVec->second.size(); n++)
        {
            m_vecvecvecCompareOperator[nIndex].resize(vecitrStartVec->second.size());

            CISISpecInfo Specinfo = vecitrStartVec->second[n];

            if (Specinfo.ENABLE == true)
            {
                if (Specinfo.AREA_RATIO_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioArea_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.AREA_RATIO_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioArea_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.WIDTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Width_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.WIDTH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Width_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.LENGTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Length_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.LENGTH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Length_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.RATIO_WL_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioWL_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.RATIO_WL_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioWL_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.RATIO_LW_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioLW_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.RATIO_LW_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioLW_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.RATIO_YX_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioYX_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.RATIO_YX_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioYX_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.RATIO_XY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioXY_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.RATIO_XY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioXY_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.ANGLE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Angle_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.ANGLE_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Angle_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.STD_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = STD_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.STD_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = STD_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.AVG_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = AVG_GV_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.AVG_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = AVG_GV_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.MIN_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = MIN_GV_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.MIN_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = MIN_GV_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.MAX_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = MAX_GV_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.MAX_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = MAX_GV_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.COMPACT_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Compact_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.COMPACT_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Compact_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.SECTION_THRESHOLD_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = SectionTh_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.SECTION_THRESHOLD_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = SectionTh_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.RATIO_WHITE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioWhite_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.RATIO_WHITE_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioWhite_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.RATIO_DARK_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioDark_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.RATIO_DARK_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioDark_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergy_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergy_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.BLACK_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergyBlack_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergyBlack_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.WHITE_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergyWhite_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergyWhite_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.AREA_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Area_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.AREA_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Area_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.MASS_CENTER_GV_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = MassCenterGV_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.MASS_CENTER_GV_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = MassCenterGV_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.HOR_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = HOR_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.HOR_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = HOR_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.VER_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = VER_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.VER_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = VER_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.RATIO_HV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioHV_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.RATIO_HV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioHV_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.RATIO_VH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioVH_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.RATIO_VH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = RatioVH_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.OVER_GV_PX_COUNT_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = OverPxCount_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.OVER_GV_PX_COUNT_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = OverPxCount_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.DIS_TO_EDGE_2_mm_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Dis2Edge_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Dis2Edge_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.IS_NEAR_EDGE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = IsNearEdge_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.IS_NEAR_EDGE_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = IsNearEdge_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.BRIGHT_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Bright_AVG_GV_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.BRIGHT_AVG_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Bright_AVG_GV_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.DARK_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Dark_AVG_GV_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.DARK_AVG_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = Dark_AVG_GV_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }

                if (Specinfo.SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergySharpness_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergySharpness_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergySharpnessWhite_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergySharpnessWhite_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergySharpnessBlack_CompareOperator;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
                if (Specinfo.BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    enLogicResult (*fp)(CISISpecInfo * pSpec, CISIDefectProperty * pDefect, BOOL bView);
                    fp = EdgeEnergySharpnessBlack_CompareOperator_2;
                    m_vecvecvecCompareOperator[nIndex][n].emplace_back(fp, vecitrStartVec->first);
                }
            }
        }
        vecitrStartVec++;
        nIndex++;
    }

	while (1)
	{
		if (index >= pCAD->m_vecCandidate.size())
		{
            break;
			/*if (true == bSingleLogicProcess)
				break;

			bSingleLogicProcess = true;
			index = 0;
			continue;*/
		}

		//if (false == SingleLogic(index, pCAD))
		//{
		//	//continue;
		//}
        //if (false == MultiLogic(index, pCAD, vecSpec, bView, m_vecCompareOperator))
        //{
        //    //continue;
        //}

        if (false == MultiLogic_VecVec(index, pCAD, vecSpec, bView, m_vecvecvecCompareOperator))
        {
            //continue;
        }
        index++;
	}

	//판정로직
	isSmallDeect(pCAD);
	isFinalJudge(pCAD);

	m_fLogicTime = tc.Elapsed_ms();


}

void CISIClassification::isFinalJudge(CISICellAllDefect* cellDefect)
{
	std::vector<CISIDefectProperty>& vecDefectProperty = cellDefect->m_vecCandidate;

	for (int i = 0; i < vecDefectProperty.size(); i++)
	{
		if (II_RESULT_VALUE::REJECT == vecDefectProperty[i].GetJudge())
		{
			cellDefect->m_Judge.SetJudge(vecDefectProperty[i].GetDefectResultCode(), II_RESULT_VALUE::REJECT);
			return;
		}
	}

	cellDefect->m_Judge.SetJudge(DefectType::ISI_END, II_RESULT_VALUE::PASS);
}

void CISIClassification::isSmallDeect(CISICellAllDefect* cellDefect)
{
	//이미 불량이 있으면 small defect을 볼 필요가 없다고 생각한다.

	std::vector<CISIDefectProperty>& vecDefectProperty = cellDefect->m_vecCandidate;
	for (int i = 0; i < vecDefectProperty.size(); i++)
	{
		if (II_RESULT_VALUE::REJECT == vecDefectProperty[i].GetJudge())
		{
			return;
		}
	}


	//정산에 시간..  우선순외가 됨
	std::map<DefectType, long> mapSmallDefectCounter;
	mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_CraterLevel1, 0));
	mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_CraterLevel2, 0));
	mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_CraterLevel3, 0));
	mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_CraterLevel4, 0));
	mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_CraterLevel5, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel1, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel2, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel3, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel4, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel5, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_DentLevel1, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_DentLevel2, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_DentLevel3, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_DentLevel4, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_DentLevel5, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel1, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel2, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel3, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel4, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel5, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_IslandLevel1, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_IslandLevel2, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_IslandLevel3, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_IslandLevel4, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_IslandLevel5, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel1, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel2, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel3, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel4, 0));
    mapSmallDefectCounter.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel5, 0));


	

	for (int i = 0; i < cellDefect->m_vecCandidate.size(); i++)
	{
		auto* pDefect = &cellDefect->m_vecCandidate[i];
		DefectType retDefectType = pDefect->GetDefectResultCode();

		std::map<DefectType, long>::iterator start, end;
		start = mapSmallDefectCounter.begin();
		end = mapSmallDefectCounter.end();

		while (start != end)
		{
            //start->first
            CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(start->first);
            if (pSpec->ENABLE == true)
            {
                //if (pSpec->SECTION_AREA_COMPARE_OPERATOR == pDefect->nInspectionArea)
                if (pSpec->PARENT_COMPARE_OPERATOR > 0 && pSpec->PARENT_COMPARE_OPERATOR == pDefect->PARENT)
                {
                    if (isSize(pDefect, pSpec) == enLogicResult::YES)
                    {
                        //pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::REJECT);
                        start->second++;
                    }
                }
			}
            start++;
		}
	}

	for (int i = 0; i < cellDefect->m_vecCandidate.size(); i++)
    {
        auto* pDefect = &cellDefect->m_vecCandidate[i];
        DefectType retDefectType = pDefect->GetDefectResultCode();

        std::map<DefectType, long>::iterator start, end;
        start = mapSmallDefectCounter.begin();
        end = mapSmallDefectCounter.end();

        while (start != end)
        {
            //start->first
            CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(start->first);
            if (pSpec->ENABLE == true)
            {
                //if (pSpec->SECTION_AREA_COMPARE_OPERATOR == pDefect->nInspectionArea)
                if (pSpec->PARENT_COMPARE_OPERATOR > 0 && pSpec->PARENT_COMPARE_OPERATOR == pDefect->PARENT)
                {
                    if (pSpec->COUNT < start->second)
                    {
                        pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::REJECT);
                        //cellDefect->m_Judge.SetJudge(start->first, II_RESULT_VALUE::REJECT);
                        break;
                    }
                }
			}
            start++;
        }
    }

	//std::map<DefectType, long>::iterator start, end;
	//start = mapSmallDefectCounter.begin();
	//end = mapSmallDefectCounter.end();
	////CISIDefectProperty* pDefect = &cellDefect->m_vecCandidate[index];
	//while (start != end)
	//{
 //       //auto* pDefect = &cellDefect->m_vecCandidate[i];
	//	CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(start->first);
 //       /*if (pSpec->PARENT_COMPARE_OPERATOR == pDefect->PARENT)
 //       {*/
 //       if (pSpec->COUNT < start->second)
 //       {
 //           //pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::REJECT);
 //           cellDefect->m_Judge.SetJudge(start->first, II_RESULT_VALUE::REJECT);
 //           break;
 //       }
 //       
	//	start++;
	//}
}

//bool compare(std::pair<DefectType, CISISpecInfo> &a, std::pair<DefectType, CISISpecInfo> &b)
//{
//	
//    //if (a.second.PRIORITY < b.second.PRIORITY)
//    //    return true;
//    //else
//        return false;
//}



bool CISIClassification::SingleLogic(int index, CISICellAllDefect* cellDefect)
{
	//CISIDefectProperty* pDefect = &cellDefect->m_vecCandidate[index];
	//// 현식 크래시피케이션 돌아가는 부분
	bool bRet = true;
	//pDefect->InitResult();
	//pDefect->SetJudge(DefectType::END, II_RESULT_VALUE::PASS);
	//std::map<DefectType, std::vector<CISISpecInfo>>::iterator itrStart, itrEnd;
	//itrStart = m_pSpecInfo->m_mapSpec_multi.begin();
	//itrEnd = m_pSpecInfo->m_mapSpec_multi.end();

	//std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec(m_pSpecInfo->m_mapSpec_multi.begin(), m_pSpecInfo->m_mapSpec_multi.end());

	////std::sort(vecSpec.begin(), vecSpec.end(), compare);

	//std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>>::iterator itrStartVec, itrEndVec;
 //   itrStartVec = vecSpec.begin();
 //   itrEndVec = vecSpec.end();
 //   //vecSpec[0].second

	////while (itrStart != itrEnd)
 //   while (itrStartVec != itrEndVec)
	//{
	//	//CISISpecInfo& spec = itrStart->second;

	//	//CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(itrStart->first);
 //       CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(itrStartVec->first);

	//	// 변수 관련 쫙 돌려라...

	//	// 순서별로 노가다.... 귀찮귀찮... 사이즈만 맨 마지막에...

	//	if (pSpec->ENABLE == true)
	//	{


	//		// 인스펙션 타입이 논이거나 제대로 셋팅 되었다면 검사 돌려라...? 논이면 그냥 안돌려야 하나?
	//		if (pSpec->nInspectionArea == pDefect->nInspectionArea)
	//		{
	//			long nLoginResult = enLogicResult::NO;
	//			BOOL bNO = FALSE;
	//			BOOL bYES = FALSE;
	//			if (pSpec->AREA_RATIO_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == RatioArea_CompareOperator(pSpec, pDefect)) // 이것도 확인 필요
	//				{
	//					//YES 일때 런이면 확인 하지마?
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->WIDTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == Width_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->LENGTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == Length_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->RATIO_WL_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == RatioWL_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->RATIO_LW_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == RatioLW_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->RATIO_YX_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == RatioYX_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}

	//			}
	//			if (pSpec->RATIO_XY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == RatioXY_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}

	//			}
	//			if (pSpec->ANGLE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == Angle_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}

	//			}
	//			if (pSpec->STD_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == STD_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}

	//			}
	//			if (pSpec->AVG_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == AVG_GV_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->MIN_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == MIN_GV_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->MAX_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == MAX_GV_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			/*if (pSpec->BRIGHT_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//			}
	//			if (pSpec->DARK_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//			}*/
	//			if (pSpec->COMPACT_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == Compact_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}

	//			}
	//			if (pSpec->SECTION_THRESHOLD_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == SectionTh_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			//if (pSpec->SECTION_AREA_COMPARE_OPERATOR != enCompareOperator::NOT_USE) // 이거 안쓰네 ...이거 뭔지 다시 체크 필요
	//			//{
	//			//	if (enLogicResult::YES == Area_CompareOperator(pSpec, pDefect))
	//			//	{
	//			//		//YES 일때 런이면 확인 하지마?
	//			//	}
	//			//}
	//			if (pSpec->RATIO_WHITE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == RatioWhite_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->RATIO_DARK_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == RatioDark_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == EdgeEnergy_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
 //               if (pSpec->BLACK_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
 //               {
 //                   if (enLogicResult::YES == EdgeEnergyBlack_CompareOperator(pSpec, pDefect))
 //                   {
 //                       nLoginResult = enLogicResult::YES;
 //                       bYES = TRUE;
 //                   }
 //                   else
 //                   {
 //                       bNO = TRUE;
 //                   }
 //               }
 //               if (pSpec->WHITE_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
 //               {
 //                   if (enLogicResult::YES == EdgeEnergyWhite_CompareOperator(pSpec, pDefect))
 //                   {
 //                       nLoginResult = enLogicResult::YES;
 //                       bYES = TRUE;
 //                   }
 //                   else
 //                   {
 //                       bNO = TRUE;
 //                   }
 //               }
	//			if (pSpec->AREA_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == Area_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}

	//			}
	//			if (pSpec->MASS_CENTER_GV_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == MASS_CENTER_GV_GV(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}

	//			}
	//			if (pSpec->HOR_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == HOR_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->VER_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == VER_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->RATIO_HV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == RatioHV_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
	//			if (pSpec->RATIO_VH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
	//			{
	//				if (enLogicResult::YES == RatioVH_CompareOperator(pSpec, pDefect))
	//				{
	//					nLoginResult = enLogicResult::YES;
	//					bYES = TRUE;
	//				}
	//				else
	//				{
	//					bNO = TRUE;
	//				}
	//			}
 //               if (pSpec->OVER_GV_PX_COUNT_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
 //               {
 //                   if (enLogicResult::YES == OverPxCount(pSpec, pDefect))
 //                   {
 //                       nLoginResult = enLogicResult::YES;
 //                       bYES = TRUE;
 //                   }
 //                   else
 //                   {
 //                       bNO = TRUE;
 //                   }
 //               }
 //               if (pSpec->DIS_TO_EDGE_mm_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
 //               {
 //                   if (enLogicResult::YES == Dis2Edge_CompareOperator(pSpec, pDefect))
 //                   {
 //                       nLoginResult = enLogicResult::YES;
 //                       bYES = TRUE;
 //                   }
 //                   else
 //                   {
 //                       bNO = TRUE;
 //                   }
 //               }
 //               if (pSpec->DIS_TO_EDGE_2_mm_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
 //               {
 //                   if (enLogicResult::YES == Dis2Edge_2_CompareOperator(pSpec, pDefect))
 //                   {
 //                       nLoginResult = enLogicResult::YES;
 //                       bYES = TRUE;
 //                   }
 //                   else
 //                   {
 //                       bNO = TRUE;
 //                   }
 //               }
 //               if (pSpec->IS_NEAR_EDGE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
 //               {
 //                   if (enLogicResult::YES == IsNearEdge_CompareOperator(pSpec, pDefect))
 //                   {
 //                       nLoginResult = enLogicResult::YES;
 //                       bYES = TRUE;
 //                   }
 //                   else
 //                   {
 //                       bNO = TRUE;
 //                   }
 //               }
 //               if (pSpec->BRIGHT_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
 //               {
 //                   if (enLogicResult::YES == Bright_AVG_GV_CompareOperator(pSpec, pDefect))
 //                   {
 //                       nLoginResult = enLogicResult::YES;
 //                       bYES = TRUE;
 //                   }
 //                   else
 //                   {
 //                       bNO = TRUE;
 //                   }
 //               }
 //               if (pSpec->DARK_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
 //               {
 //                   if (enLogicResult::YES == Dark_AVG_GV_CompareOperator(pSpec, pDefect))
 //                   {
 //                       nLoginResult = enLogicResult::YES;
 //                       bYES = TRUE;
 //                   }
 //                   else
 //                   {
 //                       bNO = TRUE;
 //                   }
 //               }
	//			if (bNO == FALSE && bYES == TRUE)
	//			{

	//				//if (Judge(pSpec->GetDefectType(), isSize(pDefect, pSpec), pDefect) == II_RESULT_VALUE::REJECT)
	//				if (isSize(pDefect, pSpec) == enLogicResult::YES)
	//				{
	//					pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::REJECT);
	//					return bRet;
	//				}
	//				else
	//				{
 //                       pDefect->PARENT = DefectType2Parent(itrStart->first);
	//					pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::PASS);
	//					
	//					// 스몰디펙 불량이면 여기서 스몰디펙 돌리기...

	//				}
	//				/*else
	//				{
	//					Judge(pSpec->GetDefectType(), enLogicResult::NO, pDefect);
	//				}*/
	//			}
	//			/*else if (bNO == TRUE && bYES == TRUE)
	//			{
	//				pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::PASS);
	//			}*/


	//		}
	//	}

	//	itrStartVec++;
	//	// itrStart++;

	//}
	//
	////pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::PASS);
	////CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Pinhole);
	///*if (enLogicResult::NO == SectionTh_CompareOperator(pSpec, pDefect))
	//{
	//	Judge(pSpec->GetDefectType(), enLogicResult::NO, pDefect);
	//	return true;
	//}
	
	//pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::PASS);
	//CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Pinhole);
	/*if (enLogicResult::NO == SectionTh_CompareOperator(pSpec, pDefect))
	{
		Judge(pSpec->GetDefectType(), enLogicResult::NO, pDefect);
		return true;
	}
=======
	//}
	
	//pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::PASS);
	//CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Pinhole);
	/*if (enLogicResult::NO == SectionTh_CompareOperator(pSpec, pDefect))
	{
		Judge(pSpec->GetDefectType(), enLogicResult::NO, pDefect);
		return true;
	}
>>>>>>> .r379

	//enLogicResult LRLength = Length_CompareOperator(pSpec, pDefect);
	//enLogicResult LRWidth = Width_CompareOperator(pSpec, pDefect);
	//if (enLogicResult::YES == LRLength && enLogicResult::YES == LRWidth)
	//{
	//	Judge(pSpec->GetDefectType(), enLogicResult::YES, pDefect);
	//}
	//else
	//{
	//	Judge(pSpec->GetDefectType(), enLogicResult::NO, pDefect);
	//}*/



	////////////////////////////////////////
	////switch (pDefect->nInspectionArea)
	////{
	////case enInspectionArea::IA_BACKGROUND_BURR:
	////	bRet = isBackGroundBurr(pDefect);
	////	break;

	////case enInspectionArea::IA_SCRAB:
	////	bRet = isScrab(index, cellDefect);
	////	break;

	////case enInspectionArea::IA_TAB:
	////	bRet = isTab(index, cellDefect);
	////	break;

	////case enInspectionArea::IA_TAB_INSULATION:
	////case enInspectionArea::IA_TAB_INSULATION_GAP:
	////case enInspectionArea::IA_CUTTINGHEIGHT_TOP:
	////case enInspectionArea::IA_CUTTINGHEIGHT_BOT:
	////	bRet = isInsulation(index, cellDefect);
	////	break;

	////case enInspectionArea::IA_ELECTRODE_EDGE_LEFT:
	////case enInspectionArea::IA_ELECTRODE_EDGE_RIGHT:
	////	bRet = isElectrodeEdge(pDefect);
	////	break;

	////case enInspectionArea::IA_ELECTRODE:
	////	bRet = isElectrode(index, cellDefect);
	////	break;

	////default:
	////	ASSERT(1);
	////}

	return bRet;
}

enParent CISIClassification::DefectType2Parent(DefectType type)
{
    switch ((DefectType)type)
    {
        
        case DefectType::START_Coater_Surface:
            return enParent::None;
#if INSPECTION_MACHINE_UC1
        case DefectType::White_Line:
            return enParent::White_Line;
        case DefectType::White_Pinhole:
            return enParent::White_Pinhole;
        case DefectType::Dark_Line:
            return enParent::Dark_Line;
        case DefectType::Dark_Crater:
            return enParent::Dark_Crater;
        case DefectType::Foil_Island:
            return enParent::Foil_Island;
        case DefectType::Edge_ChippingPinhole:
            return enParent::Edge_ChippinPinhole;
        case DefectType::Edge_Drag:
            return enParent::Drag;
        case DefectType::Edge_Line:
            return enParent::Edge_Line;
        case DefectType::Dent:
            return enParent::Dent;
        case DefectType::Crater:
            return enParent::Crater;
        case DefectType::Balck_Dot:
            return enParent::Black_Dot;
#else
        case DefectType::Line:
            return enParent::Line;
        case DefectType::Pinhole:
            return enParent::Pinhole;
        case DefectType::Horizontal_Line:
            return enParent::Horizontal_Line;
        case DefectType::Stain:
            return enParent::Stain;
        case DefectType::Island:
            return enParent::Island;
        case DefectType::Chipping_Pinhole:
            return enParent::Edge_ChippinPinhole;
        case DefectType::Drag:
            return enParent::Drag;
        case DefectType::Edge_Line:
            return enParent::Edge_Line;
        case DefectType::Dent:
            return enParent::Dent;
        case DefectType::Stain_Pinhole:
            return enParent::Stain_Pinhole;
        case DefectType::Balck_Dot:
            return enParent::Black_Dot;
#endif // INSPECTION_MACHINE_UC1

        
        case DefectType::END_Coater_Surface:
            return enParent::End;
    }
}

II_RESULT_VALUE CISIClassification::isConnectTape(CISIDefectProperty* pDefect)
{
	/***********************
	*	검사 항목
	*	Connection Tape
	*	Dark Connection Tape
	* **********************/

	//AVG_GV 120 이상이면 NG 테이프로 빼기 ( 예외 상황 )
	long nSpecAVG_GV = 128;

	enLogicResult result = enLogicResult::NO;

	//Logic
	enLogicResult LRDefectBrightness = isDefectBrightness(enDefectBrightness::DFB_WHITE, pDefect);
	enLogicResult LRAVG_GV = AVG_GV_CompareOperator(nSpecAVG_GV, pDefect);

	if (enLogicResult::YES == LRDefectBrightness)
	{
		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::ConnectionTape);
		if (enLogicResult::YES == isSize(pDefect, pSpec))
		{
			// NG 판정
			return Judge(pSpec->GetDefectType(), enLogicResult::YES, pDefect);
		}
	}

	//흑색 테이프 검출 로직을 타야함
	CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::DarkConnectionTape);
	if (enLogicResult::YES == isSize(pDefect, pSpec))
	{
		// NG 판정
		return Judge(pSpec->GetDefectType(), enLogicResult::YES, pDefect);
	}

	//AVG_GV 120 이상이면 NG 테이프로 빼기 ( 예외 상황 )
	if (II_RESULT_VALUE::REJECT == LRAVG_GV)
	{
		// NG 판정

		return Judge(DefectType::ConnectionTape, enLogicResult::YES, pDefect);
	}

	//표면으로 재검하기
	pDefect->nInspectionArea = enInspectionArea::IA_ELECTRODE;

	return II_RESULT_VALUE::PASS;
}


bool CISIClassification::isBackGroundBurr(CISIDefectProperty* pDefect)
{
	/***********************
	*	검사 항목
	*	Burr
	* **********************/

	long nCode = 0/*OR*/;
	float fSpecSizeX_mm = 10;
	float fSpecSizeY_mm = 10;

	float fSpecMinimumSizeX_mm = 0.1;
	float fSpecMinimumSizeY_mm = 0.1;
	CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Burr);
	Judge(pSpec->GetDefectType(), isSize(pDefect, pSpec), pDefect);

	return true;
}

bool CISIClassification::isScrab(const int index, CISICellAllDefect* cellDefect)
{
	/***********************
	*	검사 항목
	*	Scrab
	* **********************/


	auto& pvecDefect = cellDefect->m_vecCandidate;

	float fResolution_X_mm = 0.043;
	float fSpecNear_mm = 20;
	IPVM::Rect rtCell_pixel = IPVM::Rect(0, 0, 0, 0);

	if (enLogicResult::NO == isNearElectrodeEdge(rtCell_pixel, &pvecDefect[index], fSpecNear_mm, fResolution_X_mm))
	{
		//엣지 보다 전극 쪽에 가깝다고 판단하고 중첩 불량이 있는지 확인하고 없으면 표면 검사로 전환한다. 있다면 버린다.
		bool bPtinRect = false;
		for (int i = 0; i < pvecDefect.size(); i++)
		{
			if (index == i)
				continue;

			if (true == pvecDefect[i].rtRect_pixel.PtInRect(pvecDefect[index].rtRect_pixel.CenterPoint()))
			{
				bPtinRect = true;
				break;
			}
		}

		if (false == bPtinRect)
		{
			//겹치는 불량도 없고, 전극 엣지보다 가깝지 않으면 전극 불량으로 변경하고 return  false를 보내서 index를 singlelogic을 다시 수행한다.
			pvecDefect[index].nInspectionArea = enInspectionArea::IA_ELECTRODE;
			return false;
		}
		else
		{
			//버리자
			pvecDefect[index].SetJudge(DefectType::TopScrab, II_RESULT_VALUE::INVALID);
		}
	}
	else
	{
		//전극 가까이 있다면, scrab 검사를 하자
		long nCode = 1/*AND*/;
		float fSpecSizeX_mm = 1;
		float fSpecSizeY_mm = 1;

		float fSpecMinimumSizeX_mm = 0.1;
		float fSpecMinimumSizeY_mm = 0.1;

		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::TopScrab);
		Judge(pSpec->GetDefectType(), isSize(&pvecDefect[index], pSpec), &pvecDefect[index]);
	}

	return true;
}


bool CISIClassification::isTab(const int index, CISICellAllDefect* cellDefect)
{
	/***********************
	*	검사 항목
	*	TabWrinkle(꼽침)
	*	NG_Tag
	*	Drag
	*	Island
	* **********************/

	auto& pvecDefect = cellDefect->m_vecCandidate;
	enLogicResult result = enLogicResult::NO;

	if (enLogicResult::YES == isCategory(enCategory::CTG_TAB_FOLDING, &pvecDefect[index]))
	{
		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::TabShape);

		Judge(pSpec->GetDefectType(), isSize(&pvecDefect[index], pSpec), &pvecDefect[index]);
		return true;
	}

	if (enLogicResult::YES == isCategory(enCategory::CTG_WINKLE_ZIPPER2, &pvecDefect[index]))
	{
		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::TabWrinkle);

		Judge(pSpec->GetDefectType(), Length_CompareOperator(pSpec, &pvecDefect[index]), &pvecDefect[index]);

		return true;
	}
#if INSPECTION_MACHINE_UC1
    CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Foil_Island);
#else
    CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Island);
#endif // INSPECTION_MACHINE_UC1

	
	float fIslandSpecRatioXY = pSpec->RATIO_XY.GetValue();
    float fIslandSpecCompact = pSpec->COMPACT.GetValue();

	auto mapTab = cellDefect->m_mapInspAreaInfo.find(enInspectionArea::IA_TAB);

	bool bDefectPosOnOutside = false;
	if (mapTab != cellDefect->m_mapInspAreaInfo.end())
		bDefectPosOnOutside = CheckAttachedROI(true, mapTab->second, IPVM::ToMFC(pvecDefect[index].rtRect_pixel));

	if (enLogicResult::NO == isInspImageBrightness(enInspectionImageType::Bright, &pvecDefect[index]))
	{
		//어두운 이미지 에서
		//NG TAG 인지 확인하기
		if (enLogicResult::YES == isCategory(enCategory::CTG_NG_TAG, &pvecDefect[index]))
		{
			CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::NG_Tag);
			//enLogicResult LRSTD = STD_CompareOperator(pSpec, &pvecDefect[index]);

			enLogicResult LRAreaRatio = Area_CompareOperator(pSpec, &pvecDefect[index]);

			if (enLogicResult::YES == LRAreaRatio /*&& enLogicResult::YES == LRSTD*/)
			{
				Judge(pSpec->GetDefectType(), isSize(&pvecDefect[index], pSpec), &pvecDefect[index]);
				return true;
			}
			else
			{
				Judge(pSpec->GetDefectType(), enLogicResult::NO, &pvecDefect[index]);
				return true;
			}
		}
	}
	else
	{
		//island 
		//밝은 이미지에서
		if (true == bDefectPosOnOutside)
		{
			//Drag
#if INSPECTION_MACHINE_UC1
            CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Edge_Drag);
#else
            CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Drag);
#endif // INSPECTION_MACHINE_UC1
			//CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Drag);

			// NG 판정
			Judge(pSpec->GetDefectType(), enLogicResult::NO, &pvecDefect[index]);
			return true;
		}

		if (enLogicResult::NO == RatioXY_CompareOperator(pSpec, &pvecDefect[index]) || enLogicResult::NO == Compact_CompareOperator(pSpec, &pvecDefect[index]))
		{
			//Judge(pSpec->GetDefectType(), enLogicResult::NO, &pvecDefect[index]);
			// 사이즈 비율도 안맞고, 둥굴둘궁하지 않으면 후보로 빼지 않는다. 
			return true;
		}
	}

	Judge(pSpec->GetDefectType(), isSize(&pvecDefect[index], pSpec), &pvecDefect[index]);

	return true;
}

bool CISIClassification::isInsulation(const int index, CISICellAllDefect* cellDefect)
{
	/***********************
	*	검사 항목
	* InsulationPinhole(White)
	* InsulationGapPinhole(White)
	* InsulationLine(white, Dark 공통)
	* InsulationGapLine (white, Dark 공통)
	* InsulationIsland(Dark)
	* Drag
	* **********************/

	auto& pvecDefect = cellDefect->m_vecCandidate;
	enLogicResult result = enLogicResult::NO;

	BOOL bDefectPosOnOutside = false;	//GAP


	IPVM::Rect rtIslution = { 0,0,0,0 };
	IPVM::Rect rtCuttingHeightTop = { 0,0,0,0 };
	IPVM::Rect rtCuttingHeightBot = { 0,0,0,0 };


	float fSpecRatioLW = 4;
	enLogicResult LRDefectBrightness = isDefectBrightness(enDefectBrightness::DFB_WHITE, &pvecDefect[index]);


	auto mapIslution = cellDefect->m_mapInspAreaInfo.find(enInspectionArea::IA_TAB_INSULATION);
	auto mapCuttingHeightTop = cellDefect->m_mapInspAreaInfo.find(enInspectionArea::IA_CUTTINGHEIGHT_TOP);
	auto mapCuttingHeightBot = cellDefect->m_mapInspAreaInfo.find(enInspectionArea::IA_CUTTINGHEIGHT_BOT);

	(cellDefect->m_mapInspAreaInfo.end() != mapIslution) ? rtIslution = mapIslution->second : rtIslution.SetRectEmpty();
	(cellDefect->m_mapInspAreaInfo.end() != mapCuttingHeightTop) ? rtCuttingHeightTop = mapCuttingHeightTop->second : rtCuttingHeightTop.SetRectEmpty();
	(cellDefect->m_mapInspAreaInfo.end() != mapCuttingHeightBot) ? rtCuttingHeightBot = mapCuttingHeightBot->second : rtCuttingHeightBot.SetRectEmpty();

	//rtIslution 하고, rtCuttingHeightTop 하고 Left 영역이 달라서 가장 멀리 있는것 기준으로 해야할듯.
	if (false == rtCuttingHeightTop.IsRectEmpty())
	{
		if (rtIslution.m_left > rtCuttingHeightTop.m_left)
			rtIslution.m_left = rtCuttingHeightTop.m_left;

	}

	if (true == CheckAttachedROI(true, rtIslution, IPVM::ToMFC(pvecDefect[index].rtRect_pixel)) ||
		true == CheckAttachedROI(true, rtCuttingHeightTop, IPVM::ToMFC(pvecDefect[index].rtRect_pixel)) ||
		true == CheckAttachedROI(false, rtCuttingHeightBot, IPVM::ToMFC(pvecDefect[index].rtRect_pixel)))
	{
		bDefectPosOnOutside = true;
	}


	if (true == bDefectPosOnOutside && enLogicResult::NO == LRDefectBrightness)
	{
		//Drag
#if INSPECTION_MACHINE_UC1
        CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Edge_Drag);
#else
        CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Drag);
#endif // INSPECTION_MACHINE_UC1
		//ISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Drag);
		if (II_RESULT_VALUE::REJECT == Judge(pSpec->GetDefectType(), isSize(&pvecDefect[index], pSpec), &pvecDefect[index]))
		{
			//drag로 판정되면 리턴해서 빼버리고 
		}
		else
		{
			// pass 면 아래 루틴을 태워서 다른 불량으로 검출하는지 확인한다.
		}
		return true;

	}

	DefectType DT;

	CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::InsulationLine);
	enLogicResult LRDefectRatioXY = RatioXY_CompareOperator(pSpec, &pvecDefect[index]);
	if (enLogicResult::YES == LRDefectRatioXY)
	{
		//Line 
		// 밝고 어두운 라인성 스펙은 동일함 (LG에서 들었음)
		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::InsulationLine);

		if (true == bDefectPosOnOutside)
		{
			//엣지에 붙어있다면
			DT = DefectType::InsulationGapLine;
		}
		else
		{
			//중심부에 있으면
			DT = DefectType::InsulationLine;
		}

		enLogicResult LRLength = Length_CompareOperator(pSpec, &pvecDefect[index]);
		enLogicResult LRWidth = Width_CompareOperator(pSpec, &pvecDefect[index]);
		if (enLogicResult::YES == LRLength && enLogicResult::YES == LRWidth)
		{
			Judge(pSpec->GetDefectType(), enLogicResult::YES, &pvecDefect[index]);
		}
		else
		{
			Judge(pSpec->GetDefectType(), enLogicResult::NO, &pvecDefect[index]);
		}
		return true;

	}
	else
	{
		if (enLogicResult::YES == LRDefectBrightness)
		{
			//Pinhole
#if INSPECTION_MACHINE_UC1
            pSpec = m_pSpecInfo->GetSpecInfo(DefectType::White_Pinhole);
#else
            pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Pinhole);
#endif // INSPECTION_MACHINE_UC1
			//pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Pinhole);
			if (true == bDefectPosOnOutside)
			{
				//엣지에 붙어있다면
				DT = DefectType::InsulationGapPinhole;
			}
			else
			{
				//중심부에 있으면
				DT = DefectType::InsulationPinhole;
			}
		}
		else
		{
			//island
			pSpec = m_pSpecInfo->GetSpecInfo(DefectType::InsulationIsland);

			//중심부에 있으면
			DT = DefectType::InsulationIsland;
		}
	}

	Judge(DT, isSize(&pvecDefect[index], pSpec), &pvecDefect[index]);
	return true;
}

bool CISIClassification::isElectrodeEdge(CISIDefectProperty* pDefect)
{

	return true;
}

bool CISIClassification::isElectrode(const int index, CISICellAllDefect* cellDefect)
{
	CISIDefectProperty* pDefect = &cellDefect->m_vecCandidate[index];

	/***********************
	*	검사 항목
	* SEAM
	* SPUR
	* Pinhole
	* Protrution
	* Dent
	* Heating Wrinkle
	* crator
	* crack
	* scratch
	* **********************/


	enLogicResult result = enLogicResult::NO;

	if (enLogicResult::YES == isDefectBrightness(enDefectBrightness::DFB_MIX, pDefect))
	{
		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Seam);

		DefectType DT = DefectType::Seam;
		if (false)
		{
			//이음매를 테이프로 처리
			DT = DefectType::ConnectionTape;
		}
		II_RESULT_VALUE ret = Judge(DT, isSize(pDefect, pSpec), pDefect);

		if (II_RESULT_VALUE::PASS == ret)

			//AfxMessageBox(_T("이음매 패스 났음 ㅠ 그냥 냅두면 유출인데 어떻게 할건지 생각해보자."));

			return true;
	}

	//불량이 밝은거야?
	if (enLogicResult::YES == isDefectBrightness(enDefectBrightness::DFB_WHITE, pDefect))
	{
		//blob threshold는 어찌하나?

		bool bDefectPosOnOutside = false;

		auto mapTab = cellDefect->m_mapInspAreaInfo.find(enInspectionArea::IA_TAB);
		auto mapElectrode = cellDefect->m_mapInspAreaInfo.find(enInspectionArea::IA_ELECTRODE);
		auto mapInsulation = cellDefect->m_mapInspAreaInfo.find(enInspectionArea::IA_ELECTRODE);
		auto mapCuttingHeightTop = cellDefect->m_mapInspAreaInfo.find(enInspectionArea::IA_CUTTINGHEIGHT_TOP);
		auto mapCuttingHeightBop = cellDefect->m_mapInspAreaInfo.find(enInspectionArea::IA_CUTTINGHEIGHT_BOT);

		if (true == cellDefect->bisAnode)
		{
			//음극인가?
			//탭과 전극 사이에 불량이 위치했는지 확인해야한다. spur
			if (mapTab != cellDefect->m_mapInspAreaInfo.end())
				bDefectPosOnOutside &= CheckAttachedROI(true, mapTab->second, IPVM::ToMFC(pDefect->rtRect_pixel));
		}
		else
		{
			//양극인가?
			if (mapInsulation != cellDefect->m_mapInspAreaInfo.end())
				bDefectPosOnOutside &= CheckAttachedROI(true, mapInsulation->second, IPVM::ToMFC(pDefect->rtRect_pixel));

			if (mapCuttingHeightTop != cellDefect->m_mapInspAreaInfo.end())
				bDefectPosOnOutside &= CheckAttachedROI(true, mapCuttingHeightTop->second, IPVM::ToMFC(pDefect->rtRect_pixel));

			if (mapCuttingHeightBop != cellDefect->m_mapInspAreaInfo.end())
				bDefectPosOnOutside &= CheckAttachedROI(false, mapCuttingHeightBop->second, IPVM::ToMFC(pDefect->rtRect_pixel));
		}

		if (mapElectrode != cellDefect->m_mapInspAreaInfo.end())
			bDefectPosOnOutside &= CheckAttachedROI(false, mapElectrode->second, IPVM::ToMFC(pDefect->rtRect_pixel));

		if (true == bDefectPosOnOutside)
		{
			//Spur 
			CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Spur);

			if (II_RESULT_VALUE::REJECT == Judge(pSpec->GetDefectType(), isSize(pDefect, pSpec), pDefect))
			{
				//Spur로 판정되면 리턴한다.
				return true;
			}
		}


		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::HeatingWrinkle);
		//float fSpecRatioLW = pSpec->RATIO_LW.GetValue();
		//float fSpecCompact = pSpec->COMPACT.GetValue();

		//enLogicResult LRDefectRatioLW = RatioLW_CompareOperator(fSpecRatioLW, (enCompareOperator)pSpec->RATIO_WL_COMPARE_OPERATOR.GetValue(), pDefect);
		//enLogicResult LRDefectCompact = Compact_CompareOperator(fSpecCompact, pDefect);

		//if (enLogicResult::YES == LRDefectRatioLW && enLogicResult::YES == LRDefectCompact)
		//{
		//	//여기서 부터 라인 불량 잡아줘야함
		//	pSpec = m_pSpecInfo->GetSpecInfo(DefectType::HeatingWrinkle);
		//	Judge(pSpec->GetDefectType(), isSize(pDefect, pSpec), pDefect);
		//	return true;
		//}
		//else
		//{
#if INSPECTION_MACHINE_UC1
        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::White_Pinhole);
#else
        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Pinhole);
#endif // INSPECTION_MACHINE_UC1
		if (enLogicResult::YES == SectionTh_CompareOperator(pSpec, pDefect))
		{
			Judge(pSpec->GetDefectType(), isSize(pDefect, pSpec), pDefect);
			return true;
		}

#if INSPECTION_MACHINE_UC1
        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Delami);
#else
        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Delamination_Crack);
#endif // INSPECTION_MACHINE_UC1
		if (enLogicResult::YES == SectionTh_CompareOperator(pSpec, pDefect))
		{
			Judge(pSpec->GetDefectType(), isSize(pDefect, pSpec), pDefect);
			return true;
		}

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Dent);
		if (enLogicResult::YES == SectionTh_CompareOperator(pSpec, pDefect))
		{
			Judge(pSpec->GetDefectType(), isSize(pDefect, pSpec), pDefect);
			return true;
		}

		//이것저것도 아니면 PASS 인가? 후보도 없는데..
	//}

	}
	else
	{
#if INSPECTION_MACHINE_UC1
        CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Crater);
#else
        CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Stain_Pinhole);
#endif // INSPECTION_MACHINE_UC1 
		//CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Stain_Pinhole);

		enLogicResult LRDefectRatioLW = RatioLW_CompareOperator(pSpec, pDefect);
		enLogicResult LRDefectCompact = Compact_CompareOperator(pSpec, pDefect);

		if (enLogicResult::YES == LRDefectRatioLW && enLogicResult::YES == LRDefectCompact)
		{
			//crator
			if (enLogicResult::YES == SectionTh_CompareOperator(pSpec, pDefect))
			{
				Judge(pSpec->GetDefectType(), isSize(pDefect, pSpec), pDefect);
			}
			else
			{
				//그냥 땅인거야 아무것도 아니기 때문에 여기까지 왔다고 판단해야됨
			}

			return true;

		}
		else
		{
			CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Scratch);
			if (enLogicResult::YES == Angle_CompareOperator(pSpec, pDefect))
			{
				//scratch,
			}
			else
			{
				//crack
				pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Crack);
			}

			//
			if (enLogicResult::NO == SectionTh_CompareOperator(pSpec, pDefect))
			{
				Judge(pSpec->GetDefectType(), enLogicResult::NO, pDefect);
				return true;
			}

			enLogicResult LRLength = Length_CompareOperator(pSpec, pDefect);
			enLogicResult LRWidth = Width_CompareOperator(pSpec, pDefect);
			if (enLogicResult::YES == LRLength && enLogicResult::YES == LRWidth)
			{
				Judge(pSpec->GetDefectType(), enLogicResult::YES, pDefect);
			}
			else
			{
				Judge(pSpec->GetDefectType(), enLogicResult::NO, pDefect);
			}
		}

	}

	return true;
}


//*****************************************
//공통 로직
//*****************************************

//Logic Name::Brightness 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다. 
enLogicResult CISIClassification::isDefectBrightness(enDefectBrightness SpecBrightness, CISIDefectProperty* pDefect)
{
	// 불량 후보가 주변 대비 밝은불량인가 어두운가불량인가
	enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<long>(enCompareOperator::SAME, SpecBrightness, pDefect->nDefectBrightness.GetValue());

	CString strText;
	strText.Format(_T("%s = %s"),
		enumtoString().DefectBrightness2String(SpecBrightness),
		enumtoString().DefectBrightness2String((enDefectBrightness)pDefect->nDefectBrightness.GetValue()));

	pDefect->SetLogicStep(_T("Defect Brightness"), enLogicKey::DefectBrightness, result, strText);
	return result;
}

enLogicResult CISIClassification::isInspImageBrightness(enInspectionImageType SpecInspImageBrightness, CISIDefectProperty* pDefect)
{
	// 불량 후보가 디텍된 이미지는 밝은(64), 어두운 이미지(32) 중에 뭐죠?
	enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<long>(enCompareOperator::SAME, (long)SpecInspImageBrightness, pDefect->nInspectImage);

	CString strText;
	strText.Format(_T("%s = %s"),
		enumtoString().InspectionImageType2String(SpecInspImageBrightness),
		enumtoString().InspectionImageType2String((enInspectionImageType)pDefect->nInspectImage.GetValue()));

	pDefect->SetLogicStep(_T("INS IMG BRIGHT"), enLogicKey::InspImageBrightness, result, strText);
	return result;
}

//Logic Name::SIZE X 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다.
enLogicResult CISIClassification::SizeX_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	float fDefectSizeX = pDefect->SIZE_X_mm.GetValue();
    fDefectSizeX = (pSpec->MINIMUM_SIZEX_mm < fDefectSizeX) ? fDefectSizeX : pSpec->MINIMUM_SIZEX_mm;
    result = isCompareOperator<float>(enCompareOperator(pSpec->SIZEX_COMPARE_OPERATOR.GetValue()), pSpec->SIZE_X_mm.GetValue(), fDefectSizeX);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->SIZE_X_mm.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->SIZEX_COMPARE_OPERATOR.GetValue())), fDefectSizeX);

        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::SizeX, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::SizeY_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    float fDefectSizeY = pDefect->SIZE_Y_mm.GetValue();
    fDefectSizeY = (pSpec->MINIMUM_SIZEY_mm < fDefectSizeY) ? fDefectSizeY : pSpec->MINIMUM_SIZEY_mm;

    result = isCompareOperator<float>(enCompareOperator(pSpec->SIZEY_COMPARE_OPERATOR.GetValue()), pSpec->SIZE_Y_mm.GetValue(), pDefect->SIZE_Y_mm.GetValue());
    if (bView)
    {
        CString strText;
        strText.Format(
            _T("%.3f %s %.3f"), pSpec->SIZE_Y_mm.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->SIZEY_COMPARE_OPERATOR.GetValue())), fDefectSizeY);

        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::SizeY, result, strText);
    }

    return result;
}

//Logic Name::MAX_GV 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다. 
enLogicResult CISIClassification::MAX_GV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;


	result = isCompareOperator<long>(enCompareOperator(pSpec->MAX_GV_COMPARE_OPERATOR.GetValue()), pSpec->MAX_GV, pDefect->MAX_GV);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%d %s %d"), pSpec->MAX_GV.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->MAX_GV_COMPARE_OPERATOR.GetValue())),
            pDefect->MAX_GV.GetValue());

        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::MAX_GV, result, strText);
    }

    return result;
}


//Logic Name::MAX_GV 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다.
enLogicResult CISIClassification::MAX_GV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<long>(enCompareOperator(pSpec->MAX_GV_COMPARE_OPERATOR_2.GetValue()), pSpec->MAX_GV_2, pDefect->MAX_GV);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%d %s %d"), pSpec->MAX_GV_2.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->MAX_GV_COMPARE_OPERATOR_2.GetValue())),
            pDefect->MAX_GV.GetValue());

        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::MAX_GV, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::AVG_GV_CompareOperator(long nSpecAVG, CISIDefectProperty* pDefect)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<long>(enCompareOperator::BIGGER_THAN_SPEC, nSpecAVG, pDefect->AVG_GV);

    CString strText;
    strText.Format(_T("%d %s %d"), nSpecAVG, enumtoString().CompareOperator2String(enCompareOperator::BIGGER_THAN_SPEC), pDefect->AVG_GV.GetValue());

    pDefect->SetLogicStep(_T(""), enLogicKey::AVG_GV, result, strText);
    return result;
}

//Logic Name::AVG_GV 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다. 
enLogicResult CISIClassification::AVG_GV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
	enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<long>(enCompareOperator(pSpec->AVG_COMPARE_OPERATOR.GetValue()), pSpec->AVG_GV, pDefect->AVG_GV);
    if (bView)
    {
        CString strText;
        strText.Format(
            _T("%d %s %d"), pSpec->AVG_GV.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->AVG_COMPARE_OPERATOR.GetValue())), pDefect->AVG_GV.GetValue());

	/*strText.Format(_T("%d %s %d"),
		pSpec->AVG_GV.GetValue(),enumtoString().CompareOperator2String(enCompareOperator(pSpec->AVG_COMPARE_OPERATOR.GetValue())),
		pDefect->AVG_GV.GetValue());*/

		pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::AVG_GV, result, strText);
    }
	
	return result;
}

enLogicResult CISIClassification::AVG_GV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<long>(enCompareOperator(pSpec->AVG_COMPARE_OPERATOR_2.GetValue()), pSpec->AVG_GV_2, pDefect->AVG_GV);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%d %s %d"), pSpec->AVG_GV_2.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->AVG_COMPARE_OPERATOR_2.GetValue())),
            pDefect->AVG_GV.GetValue());

        /*strText.Format(_T("%d %s %d"),
		pSpec->AVG_GV.GetValue(),enumtoString().CompareOperator2String(enCompareOperator(pSpec->AVG_COMPARE_OPERATOR.GetValue())),
		pDefect->AVG_GV.GetValue());*/

        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::AVG_GV, result, strText);
    }

    return result;
}
//Logic Name::MIN_GV 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다. 
enLogicResult CISIClassification::MIN_GV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<long>(
        enCompareOperator(pSpec->MIN_GV_COMPARE_OPERATOR.GetValue()), pSpec->MIN_GV, pDefect->MIN_GV);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%d %s %d"), pSpec->MIN_GV.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->MIN_GV_COMPARE_OPERATOR.GetValue())),
            pDefect->MIN_GV.GetValue());

	/*strText.Format(_T("%d %s %d"),
		pSpec->MIN_GV.GetValue(),
        enumtoString().CompareOperator2String(enCompareOperator(pSpec->MIN_GV_COMPARE_OPERATOR.GetValue())),
		pDefect->MIN_GV.GetValue());*/
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::MIN_GV, result, strText);
    }

    return result;
}

//Logic Name::MIN_GV 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다.
enLogicResult CISIClassification::MIN_GV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<long>(enCompareOperator(pSpec->MIN_GV_COMPARE_OPERATOR_2.GetValue()), pSpec->MIN_GV_2, pDefect->MIN_GV);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%d %s %d"), pSpec->MIN_GV_2.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->MIN_GV_COMPARE_OPERATOR_2.GetValue())),
            pDefect->MIN_GV.GetValue());

        /*strText.Format(_T("%d %s %d"),
		pSpec->MIN_GV.GetValue(),
        enumtoString().CompareOperator2String(enCompareOperator(pSpec->MIN_GV_COMPARE_OPERATOR.GetValue())),
		pDefect->MIN_GV.GetValue());*/
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::MIN_GV, result, strText);
    }

    return result;
}

//Logic Name::Width 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다. 
enLogicResult CISIClassification::Width_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>(
        enCompareOperator(pSpec->WIDTH_COMPARE_OPERATOR.GetValue()), pSpec->WIDTH_mm, pDefect->WIDTH_mm);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"),
		pSpec->WIDTH_mm.GetValue(),
        enumtoString().CompareOperator2String(enCompareOperator(pSpec->WIDTH_COMPARE_OPERATOR.GetValue())),pDefect->WIDTH_mm.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Width, result, strText);

    }

    return result;
}

//Logic Name::Width 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다.
enLogicResult CISIClassification::Width_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>(enCompareOperator(pSpec->WIDTH_COMPARE_OPERATOR_2.GetValue()), pSpec->WIDTH_mm_2, pDefect->WIDTH_mm);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->WIDTH_mm_2.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->WIDTH_COMPARE_OPERATOR_2.GetValue())),
            pDefect->WIDTH_mm.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Width, result, strText);
    }

    return result;
}

//Logic Name::Length 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다. 
enLogicResult CISIClassification::Length_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>(
        enCompareOperator(pSpec->LENGTH_COMPARE_OPERATOR.GetValue()), pSpec->LENGTH_mm, pDefect->LENGTH_mm);
    if (bView)
    {
		CString strText;

		strText.Format(_T("%.3f %s %.3f"),
			pSpec->LENGTH_mm.GetValue(),
			enumtoString().CompareOperator2String(enCompareOperator(pSpec->LENGTH_COMPARE_OPERATOR.GetValue())),
			pDefect->LENGTH_mm.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Length, result, strText);

	}

    return result;
}

//Logic Name::Length 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다.
enLogicResult CISIClassification::Length_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>(enCompareOperator(pSpec->LENGTH_COMPARE_OPERATOR_2.GetValue()), pSpec->LENGTH_mm_2, pDefect->LENGTH_mm);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->LENGTH_mm_2.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->LENGTH_COMPARE_OPERATOR_2.GetValue())),
            pDefect->LENGTH_mm.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Length, result, strText);
    }

    return result;
}

//Logic Name::Area 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다. 
enLogicResult CISIClassification::Area_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;


	result = isCompareOperator<float>(enCompareOperator(pSpec->AREA_COMPARE_OPERATOR.GetValue()), pSpec->AREA, pDefect->AREA);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"),
        	pSpec->AREA.GetValue(),
               enumtoString().CompareOperator2String(enCompareOperator(pSpec->AREA_COMPARE_OPERATOR.GetValue())),
        	pDefect->AREA.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Area, result, strText);
    }
    return result;
}

//Logic Name::Area 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다.
enLogicResult CISIClassification::Area_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>(enCompareOperator(pSpec->AREA_COMPARE_OPERATOR_2.GetValue()), pSpec->AREA_2, pDefect->AREA);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->AREA_2.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->AREA_COMPARE_OPERATOR_2.GetValue())),
            pDefect->AREA.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Area, result, strText);
    }
    return result;
}

//Logic Name::RatioYX 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다. 
enLogicResult CISIClassification::RatioLW_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_LW_COMPARE_OPERATOR.GetValue(), pSpec->RATIO_LW, pDefect->RATIO_LW);
    if (bView)
    {
		CString strText;

		strText.Format(_T("%.3f %s %.3f"),
			pSpec->RATIO_LW.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_LW_COMPARE_OPERATOR.GetValue()),
			pDefect->RATIO_LW.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioLW, result, strText);
    }

    return result;
}

//Logic Name::RatioYX 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다.
enLogicResult CISIClassification::RatioLW_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_LW_COMPARE_OPERATOR_2.GetValue(), pSpec->RATIO_LW_2, pDefect->RATIO_LW);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->RATIO_LW_2.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_LW_COMPARE_OPERATOR_2.GetValue()),
            pDefect->RATIO_LW.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioLW, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioWL_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_WL_COMPARE_OPERATOR.GetValue(), pSpec->RATIO_WL, pDefect->RATIO_WL);
    if (bView)
    {
		CString strText;
		strText.Format(_T("%.3f %s %.3f"),
			pSpec->RATIO_WL.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_WL_COMPARE_OPERATOR.GetValue()),
			pDefect->RATIO_WL.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioWL, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioWL_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_WL_COMPARE_OPERATOR_2.GetValue(), pSpec->RATIO_WL_2, pDefect->RATIO_WL);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->RATIO_WL_2.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_WL_COMPARE_OPERATOR_2.GetValue()),
            pDefect->RATIO_WL.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioWL, result, strText);
    }

    return result;
}


enLogicResult CISIClassification::RatioYX_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_YX_COMPARE_OPERATOR.GetValue(), pSpec->RATIO_YX, pDefect->RATIO_YX);
    if (bView)
    {
		CString strText;
	
		strText.Format(_T("%.3f %s %.3f"),
			pSpec->RATIO_YX.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_YX_COMPARE_OPERATOR.GetValue()),
			pDefect->RATIO_YX.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioYX, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioYX_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_YX_COMPARE_OPERATOR_2.GetValue(), pSpec->RATIO_YX_2, pDefect->RATIO_YX);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->RATIO_YX_2.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_YX_COMPARE_OPERATOR_2.GetValue()),
            pDefect->RATIO_YX.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioYX, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioXY_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_XY_COMPARE_OPERATOR.GetValue(), pSpec->RATIO_XY, pDefect->RATIO_XY);
    if (bView)
    {
		CString strText;

		strText.Format(_T("%.3f %s %.3f"),
			pSpec->RATIO_XY.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_XY_COMPARE_OPERATOR.GetValue()),
			pDefect->RATIO_XY.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioXY, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioXY_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_XY_COMPARE_OPERATOR_2.GetValue(), pSpec->RATIO_XY_2, pDefect->RATIO_XY);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->RATIO_XY_2.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_XY_COMPARE_OPERATOR_2.GetValue()),
            pDefect->RATIO_XY.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioXY, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::Angle_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>(enCompareOperator(pSpec->ANGLE_COMPARE_OPERATOR.GetValue()),
        pSpec->ANGLE.GetValue(), abs(pDefect->ANGLE.GetValue()));
    if (bView)
    {
		CString strText;

		strText.Format(_T("%.3f %s %.3f"),
			pSpec->ANGLE.GetValue(),
	        enumtoString().CompareOperator2String(enCompareOperator(pSpec->ANGLE_COMPARE_OPERATOR.GetValue())),
			abs(pDefect->ANGLE.GetValue()));
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Angle, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::Angle_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>(enCompareOperator(pSpec->ANGLE_COMPARE_OPERATOR_2.GetValue()), pSpec->ANGLE_2.GetValue(), abs(pDefect->ANGLE.GetValue()));
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->ANGLE_2.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->ANGLE_COMPARE_OPERATOR_2.GetValue())),
            abs(pDefect->ANGLE.GetValue()));
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Angle, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::STD_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>(enCompareOperator(pSpec->STD_COMPARE_OPERATOR.GetValue()), pSpec->STD, pDefect->STD);
    if (bView)
    {
		CString strText;

		strText.Format(_T("%.3f %s %.3f"),
			pSpec->STD.GetValue(),
			enumtoString().CompareOperator2String(enCompareOperator(pSpec->STD_COMPARE_OPERATOR.GetValue())),
			pDefect->STD.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Std, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::STD_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>(enCompareOperator(pSpec->STD_COMPARE_OPERATOR_2.GetValue()), pSpec->STD_2, pDefect->STD);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->STD_2.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->STD_COMPARE_OPERATOR_2.GetValue())),
            pDefect->STD.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Std, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::Compact_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>(enCompareOperator(pSpec->COMPACT_COMPARE_OPERATOR.GetValue()), pSpec->COMPACT.GetValue(), pDefect->COMPACT);
    if (bView)
    {

		CString strText;

		strText.Format(_T("%.3f %s %.3f"),
			pSpec->COMPACT.GetValue(),
	        enumtoString().CompareOperator2String(enCompareOperator(pSpec->COMPACT_COMPARE_OPERATOR.GetValue())),
			pDefect->COMPACT.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Compact, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::Compact_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>(enCompareOperator(pSpec->COMPACT_COMPARE_OPERATOR_2.GetValue()), pSpec->COMPACT_2.GetValue(), pDefect->COMPACT);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->COMPACT_2.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->COMPACT_COMPARE_OPERATOR_2.GetValue())),
            pDefect->COMPACT.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Compact, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::Bright_AVG_GV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<long>(enCompareOperator(pSpec->BRIGHT_AVG_GV_COMPARE_OPERATOR.GetValue()), pSpec->BRIGHT_AVG_GV, pDefect->BRIGHT_AVG_GV);
    if (bView)
    {
		CString strText;

		strText.Format(_T("%d %s %d"),
			pSpec->BRIGHT_AVG_GV.GetValue(),
			enumtoString().CompareOperator2String(enCompareOperator(pSpec->BRIGHT_AVG_GV_COMPARE_OPERATOR.GetValue())),
			pDefect->BRIGHT_AVG_GV.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Bright_avg_gv, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::Bright_AVG_GV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<long>(enCompareOperator(pSpec->BRIGHT_AVG_GV_COMPARE_OPERATOR_2.GetValue()), pSpec->BRIGHT_AVG_GV_2, pDefect->BRIGHT_AVG_GV);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%d %s %d"), pSpec->BRIGHT_AVG_GV_2.GetValue(),
            enumtoString().CompareOperator2String(enCompareOperator(pSpec->BRIGHT_AVG_GV_COMPARE_OPERATOR_2.GetValue())), pDefect->BRIGHT_AVG_GV.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Bright_avg_gv, result, strText);
    }

    return result;
}


enLogicResult CISIClassification::Dark_AVG_GV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<long>(
        enCompareOperator(pSpec->DARK_AVG_GV_COMPARE_OPERATOR.GetValue()), pSpec->DARK_AVG_GV, pDefect->DARK_AVG_GV);

    if (bView)
    {
		CString strText;
			strText.Format(_T("%d %s %d"), pSpec->DARK_AVG_GV.GetValue(),
				enumtoString().CompareOperator2String(enCompareOperator(pSpec->DARK_AVG_GV_COMPARE_OPERATOR.GetValue())), pDefect->DARK_AVG_GV.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Dark_avg_gv, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::Dark_AVG_GV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<long>(enCompareOperator(pSpec->DARK_AVG_GV_COMPARE_OPERATOR_2.GetValue()), pSpec->DARK_AVG_GV_2, pDefect->DARK_AVG_GV);

    if (bView)
    {
        CString strText;
        strText.Format(_T("%d %s %d"), pSpec->DARK_AVG_GV_2.GetValue(), enumtoString().CompareOperator2String(enCompareOperator(pSpec->DARK_AVG_GV_COMPARE_OPERATOR_2.GetValue())),
            pDefect->DARK_AVG_GV.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Dark_avg_gv, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::SectionTh_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<long>((enCompareOperator)pSpec->SECTION_THRESHOLD_COMPARE_OPERATOR.GetValue(), pSpec->SECTION_THRESHOLD.GetValue(), pDefect->SECTION_THRESHOLD);
    if (bView)
    {
		CString strText;

		strText.Format(_T("%d %s %d"),
			pSpec->SECTION_THRESHOLD.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->SECTION_THRESHOLD_COMPARE_OPERATOR.GetValue()),
			pDefect->SECTION_THRESHOLD.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Section_Threshold, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::SectionTh_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<long>((enCompareOperator)pSpec->SECTION_THRESHOLD_COMPARE_OPERATOR_2.GetValue(), pSpec->SECTION_THRESHOLD_2.GetValue(), pDefect->SECTION_THRESHOLD);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%d %s %d"), pSpec->SECTION_THRESHOLD_2.GetValue(),
            enumtoString().CompareOperator2String((enCompareOperator)pSpec->SECTION_THRESHOLD_COMPARE_OPERATOR_2.GetValue()), pDefect->SECTION_THRESHOLD.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Section_Threshold, result, strText);
    }

    return result;
}

// [sm]
enLogicResult CISIClassification::RatioWhite_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_WHITE_COMPARE_OPERATOR.GetValue(), pSpec->RATIO_WHITE, pDefect->RATIO_WHITE);

    if (bView)
    {
		CString strText;

		strText.Format(_T("%.3f %s %.3f"),
			pSpec->RATIO_WHITE.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_WHITE_COMPARE_OPERATOR.GetValue()),
			pDefect->RATIO_WHITE.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioWhite, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioWhite_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_WHITE_COMPARE_OPERATOR_2.GetValue(), pSpec->RATIO_WHITE_2, pDefect->RATIO_WHITE);

    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->RATIO_WHITE_2.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_WHITE_COMPARE_OPERATOR_2.GetValue()), pDefect->RATIO_WHITE.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioWhite, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioDark_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_DARK_COMPARE_OPERATOR.GetValue(), pSpec->RATIO_DARK, pDefect->RATIO_DARK);
    if (bView)
    {
		CString strText;

		strText.Format(_T("%.3f %s %.3f"),
			pSpec->RATIO_DARK.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_DARK_COMPARE_OPERATOR.GetValue()),
			pDefect->RATIO_DARK.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioDark, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioDark_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_DARK_COMPARE_OPERATOR_2.GetValue(), pSpec->RATIO_DARK_2, pDefect->RATIO_DARK);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->RATIO_DARK_2.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_DARK_COMPARE_OPERATOR_2.GetValue()), pDefect->RATIO_DARK.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioDark, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::EdgeEnergy_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;


	result = isCompareOperator<float>((enCompareOperator)pSpec->EDGE_ENERGY_COMPARE_OPERATOR.GetValue(), pSpec->EDGE_ENERGY, pDefect->EDGE_ENERGY);
    if (bView)
    {
		CString strText;

		strText.Format(_T("%.3f %s %.3f"),
			pSpec->EDGE_ENERGY.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
			pDefect->EDGE_ENERGY.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::EdgeEnergy, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::EdgeEnergy_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue(), pSpec->EDGE_ENERGY_2, pDefect->EDGE_ENERGY);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->EDGE_ENERGY_2.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()), pDefect->EDGE_ENERGY.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::EdgeEnergy, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::EdgeEnergyBlack_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->BLACK_EDGE_ENERGY_COMPARE_OPERATOR.GetValue(), pSpec->EDGE_ENERGY_BLACK, pDefect->EDGE_ENERGY_BLACK);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->EDGE_ENERGY_BLACK.GetValue(),
                enumtoString().CompareOperator2String((enCompareOperator)pSpec->BLACK_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()), pDefect->EDGE_ENERGY_BLACK.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::EdgeEnergy_Black, result, strText);

    }
    return result;
}

enLogicResult CISIClassification::EdgeEnergyBlack_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue(), pSpec->EDGE_ENERGY_BLACK_2, pDefect->EDGE_ENERGY_BLACK);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->EDGE_ENERGY_BLACK_2.GetValue(),
            enumtoString().CompareOperator2String((enCompareOperator)pSpec->BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()), pDefect->EDGE_ENERGY_BLACK.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::EdgeEnergy_Black, result, strText);
    }
    return result;
}

enLogicResult CISIClassification::EdgeEnergyWhite_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;
	
    result = isCompareOperator<float>((enCompareOperator)pSpec->WHITE_EDGE_ENERGY_COMPARE_OPERATOR.GetValue(), pSpec->EDGE_ENERGY_WHITE, pDefect->EDGE_ENERGY_WHITE);
    if (bView)
    {
		CString strText;

		strText.Format(_T("%.3f %s %.3f"), pSpec->EDGE_ENERGY_WHITE.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->WHITE_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()), pDefect->EDGE_ENERGY_WHITE.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::EdgeEnergy_White, result, strText);
    }
    return result;
}

enLogicResult CISIClassification::EdgeEnergyWhite_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue(), pSpec->EDGE_ENERGY_WHITE_2, pDefect->EDGE_ENERGY_WHITE);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->EDGE_ENERGY_WHITE_2.GetValue(),
            enumtoString().CompareOperator2String((enCompareOperator)pSpec->WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()), pDefect->EDGE_ENERGY_WHITE.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::EdgeEnergy_White, result, strText);
    }
    return result;
}

//enLogicResult CISIClassification::RatioArea_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
//{
//    enLogicResult result = enLogicResult::NO;
//
//    result = isCompareOperator<float>((enCompareOperator)pSpec->AREA_RATIO_COMPARE_OPERATOR.GetValue(), pSpec->AREA_Ratio, pDefect->AREA_Ratio);
//    if (bView)
//    {
//        CString strText;
//
//        strText.Format(_T("%.3f %s %.3f"), pSpec->AREA_Ratio.GetValue(),
//            enumtoString().CompareOperator2String((enCompareOperator)pSpec->AREA_RATIO_COMPARE_OPERATOR.GetValue()), pDefect->AREA_Ratio.GetValue());
//        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioArea, result, strText);
//    }
//
//    return result;
//}

enLogicResult CISIClassification::RatioArea_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>((enCompareOperator)pSpec->AREA_RATIO_COMPARE_OPERATOR_2.GetValue(), pSpec->AREA_Ratio_2, pDefect->AREA_Ratio);
    if (bView)
    {
		CString strText;

		strText.Format(_T("%.3f %s %.3f"), pSpec->AREA_Ratio_2.GetValue(),
		    enumtoString().CompareOperator2String((enCompareOperator)pSpec->AREA_RATIO_COMPARE_OPERATOR_2.GetValue()),
		    pDefect->AREA_Ratio.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioArea, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::MassCenterGV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;


	result = isCompareOperator<float>((enCompareOperator)pSpec->MASS_CENTER_GV_GV_COMPARE_OPERATOR.GetValue(), pSpec->MASS_CENTER_GV_GV, pDefect->MASS_CENTER_GV_GV);
    if (bView)
    {
		CString strText;
	
		strText.Format(_T("%.3f %s %.3f"),
			pSpec->MASS_CENTER_GV_GV.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->MASS_CENTER_GV_GV_COMPARE_OPERATOR.GetValue()),
			pDefect->MASS_CENTER_GV_GV.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::MASS_CENTER_GV_gv, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::MassCenterGV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->MASS_CENTER_GV_GV_COMPARE_OPERATOR_2.GetValue(), pSpec->MASS_CENTER_GV_GV_2, pDefect->MASS_CENTER_GV_GV);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->MASS_CENTER_GV_GV_2.GetValue(),
            enumtoString().CompareOperator2String((enCompareOperator)pSpec->MASS_CENTER_GV_GV_COMPARE_OPERATOR_2.GetValue()), pDefect->MASS_CENTER_GV_GV.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::MASS_CENTER_GV_gv, result, strText);
    }

    return result;
}


enLogicResult CISIClassification::OverPxCount_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<long>(
        (enCompareOperator)pSpec->OVER_GV_PX_COUNT_COMPARE_OPERATOR.GetValue(), pSpec->OVER_200_GV_PIXEL_COUNT.GetValue(),
        pDefect->OVER_200_GV_PIXEL_COUNT.GetValue());
    if (bView)
    {
        CString strText;
        strText.Format(_T("%d %s %d"), pSpec->OVER_200_GV_PIXEL_COUNT.GetValue(),
				enumtoString().CompareOperator2String((enCompareOperator)pSpec->OVER_GV_PX_COUNT_COMPARE_OPERATOR.GetValue()), pDefect->OVER_200_GV_PIXEL_COUNT.GetValue());

        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::OverPxCnt, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::OverPxCount_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<long>(
        (enCompareOperator)pSpec->OVER_GV_PX_COUNT_COMPARE_OPERATOR_2.GetValue(), pSpec->OVER_200_GV_PIXEL_COUNT_2.GetValue(), pDefect->OVER_200_GV_PIXEL_COUNT.GetValue());
    if (bView)
    {
        CString strText;
        strText.Format(_T("%d %s %d"), pSpec->OVER_200_GV_PIXEL_COUNT_2.GetValue(),
            enumtoString().CompareOperator2String((enCompareOperator)pSpec->OVER_GV_PX_COUNT_COMPARE_OPERATOR_2.GetValue()), pDefect->OVER_200_GV_PIXEL_COUNT.GetValue());

        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::OverPxCnt, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::HOR_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;


    result = isCompareOperator<float>((enCompareOperator)pSpec->HOR_COMPARE_OPERATOR.GetValue(), pSpec->HOR, pDefect->HOR);
    if (bView)
    {
		CString strText;
		strText.Format(_T("%.3f %s %.3f"),
			pSpec->HOR.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->HOR_COMPARE_OPERATOR.GetValue()),
			pDefect->HOR.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Hor, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::HOR_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->HOR_COMPARE_OPERATOR_2.GetValue(), pSpec->HOR_2, pDefect->HOR);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->HOR_2.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->HOR_COMPARE_OPERATOR_2.GetValue()),
            pDefect->HOR.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Hor, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::VER_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>((enCompareOperator)pSpec->VER_COMPARE_OPERATOR.GetValue(), pSpec->VER, pDefect->VER);
    if (bView)
    {
		CString strText;
		strText.Format(_T("%.3f %s %.3f"),
			pSpec->VER.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->VER_COMPARE_OPERATOR.GetValue()),
			pDefect->VER.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Ver, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::VER_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->VER_COMPARE_OPERATOR_2.GetValue(), pSpec->VER_2, pDefect->VER);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->VER_2.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->VER_COMPARE_OPERATOR_2.GetValue()),
            pDefect->VER.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Ver, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioHV_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_HV_COMPARE_OPERATOR.GetValue(), pSpec->RATIO_HV, pDefect->RATIO_HV);
    if (bView)
    {
		CString strText;
		strText.Format(_T("%.3f %s %.3f"),
			pSpec->RATIO_HV.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_HV_COMPARE_OPERATOR.GetValue()),
			pDefect->RATIO_HV.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioHV, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioHV_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_HV_COMPARE_OPERATOR_2.GetValue(), pSpec->RATIO_HV_2, pDefect->RATIO_HV);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->RATIO_HV_2.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_HV_COMPARE_OPERATOR_2.GetValue()),
            pDefect->RATIO_HV.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioHV, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioVH_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

	result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_VH_COMPARE_OPERATOR.GetValue(), pSpec->RATIO_VH, pDefect->RATIO_VH);
    if (bView)
    {
		CString strText;

		strText.Format(_T("%.3f %s %.3f"),
			pSpec->RATIO_VH.GetValue(),
			enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_VH_COMPARE_OPERATOR.GetValue()),
			pDefect->RATIO_VH.GetValue());
			pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioVH, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::RatioVH_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->RATIO_VH_COMPARE_OPERATOR_2.GetValue(), pSpec->RATIO_VH_2, pDefect->RATIO_VH);
    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->RATIO_VH_2.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->RATIO_VH_COMPARE_OPERATOR_2.GetValue()),
            pDefect->RATIO_VH.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::RatioVH, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::Dis2Edge_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;


    result = isCompareOperator<float>(
        (enCompareOperator)pSpec->DIS_TO_EDGE_mm_COMPARE_OPERATOR.GetValue(), pSpec->DISTANCE_TO_EDGE_mm, pDefect->DISTANCE_TO_EDGE_mm);

    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->DISTANCE_TO_EDGE_mm.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->DIS_TO_EDGE_mm_COMPARE_OPERATOR.GetValue()), pDefect->DISTANCE_TO_EDGE_mm.GetValue());


        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Dis2Edge, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::Dis2Edge_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->DIS_TO_EDGE_mm_COMPARE_OPERATOR_2.GetValue(), pSpec->DISTANCE_TO_EDGE_mm_2, pDefect->DISTANCE_TO_EDGE_mm);

    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->DISTANCE_TO_EDGE_mm_2.GetValue(),
            enumtoString().CompareOperator2String((enCompareOperator)pSpec->DIS_TO_EDGE_mm_COMPARE_OPERATOR_2.GetValue()), pDefect->DISTANCE_TO_EDGE_mm.GetValue());

        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Dis2Edge, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::Dis2Edge_2_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;


    result = isCompareOperator<float>((enCompareOperator)pSpec->DIS_TO_EDGE_2_mm_COMPARE_OPERATOR.GetValue(), pSpec->DISTANCE_TO_EDGE_2_mm, pDefect->DISTANCE_TO_EDGE_mm);

    if (bView)
    {
        CString strText;

            strText.Format(_T("%.3f %s %.3f"), pSpec->DISTANCE_TO_EDGE_2_mm.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->DIS_TO_EDGE_2_mm_COMPARE_OPERATOR.GetValue()), pDefect->DISTANCE_TO_EDGE_mm.GetValue());


        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Dis2Edge_2, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::Dis2Edge_2_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2.GetValue(), pSpec->DISTANCE_TO_EDGE_2_mm_2, pDefect->DISTANCE_TO_EDGE_mm);

    if (bView)
    {
        CString strText;

        strText.Format(_T("%.3f %s %.3f"), pSpec->DISTANCE_TO_EDGE_2_mm_2.GetValue(),
            enumtoString().CompareOperator2String((enCompareOperator)pSpec->DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2.GetValue()), pDefect->DISTANCE_TO_EDGE_mm.GetValue());

        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Dis2Edge_2, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::IsNearEdge_CompareOperator(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<long>((enCompareOperator)pSpec->IS_NEAR_EDGE_COMPARE_OPERATOR.GetValue(), pSpec->IS_NEAR_EDGE.GetValue(), pDefect->IS_NEAR_EDGE.GetValue());
    if (bView)
    {
        CString strText;
        strText.Format(_T("%d %s %d"), pSpec->IS_NEAR_EDGE.GetValue(), enumtoString().CompareOperator2String((enCompareOperator)pSpec->IS_NEAR_EDGE_COMPARE_OPERATOR.GetValue()), pDefect->IS_NEAR_EDGE.GetValue());

        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::IsNearEdge, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::IsNearEdge_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<long>((enCompareOperator)pSpec->IS_NEAR_EDGE_COMPARE_OPERATOR_2.GetValue(), pSpec->IS_NEAR_EDGE_2.GetValue(), pDefect->IS_NEAR_EDGE.GetValue());
    if (bView)
    {
        CString strText;
        strText.Format(_T("%d %s %d"), pSpec->IS_NEAR_EDGE_2.GetValue(),
            enumtoString().CompareOperator2String((enCompareOperator)pSpec->IS_NEAR_EDGE_COMPARE_OPERATOR_2.GetValue()), pDefect->IS_NEAR_EDGE.GetValue());

        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::IsNearEdge, result, strText);
    }

    return result;
}

enLogicResult CISIClassification::isNearElectrodeEdge(IPVM::Rect rtCell_pixel, CISIDefectProperty* pDefect, float fSpecNear_mm, float fResolution_X_mm)
{
	//전극 좌우 끝에서 얼마나(fSpecNear_mm) 떨어져 있는가?
	float fDistanceLeftEdge2Defect_mm = (pDefect->rtRect_pixel.m_left - rtCell_pixel.m_left) * fResolution_X_mm;
	float fDistanceRightEdge2Defect_mm = (rtCell_pixel.m_right - pDefect->rtRect_pixel.m_right) * fResolution_X_mm;

	enLogicResult result = isCompareOperator<float>(enCompareOperator::BIGGER_THAN_SPEC, fSpecNear_mm, min(fDistanceLeftEdge2Defect_mm, fDistanceRightEdge2Defect_mm));

	pDefect->SetLogicStep(_T(""), enLogicKey::ElectrodeEdge, result);
	return result;
}

enLogicResult CISIClassification::EdgeEnergySharpness_CompareOperator( CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue(),
        pSpec->EDGE_ENERGY_SHARPNESS, pDefect->EDGE_ENERGY_SHARPNESS);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->EDGE_ENERGY_SHARPNESS.GetValue(),
            enumtoString().CompareOperator2String(
                (enCompareOperator)pSpec->SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
            pDefect->EDGE_ENERGY_SHARPNESS.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Sharpness_EdgeEnergy, result, strText);
    }
    return result;
}

enLogicResult CISIClassification::EdgeEnergySharpness_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue(),
        pSpec->EDGE_ENERGY_SHARPNESS_2, pDefect->EDGE_ENERGY_SHARPNESS);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->EDGE_ENERGY_SHARPNESS_2.GetValue(),
            enumtoString().CompareOperator2String(
                (enCompareOperator)pSpec->SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
            pDefect->EDGE_ENERGY_SHARPNESS.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Sharpness_EdgeEnergy, result, strText);
    }
    return result;
}

enLogicResult CISIClassification::EdgeEnergySharpnessWhite_CompareOperator( CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue(),
        pSpec->EDGE_ENERGY_SHARPNESS_WHITE, pDefect->EDGE_ENERGY_SHARPNESS_WHITE);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->EDGE_ENERGY_SHARPNESS_WHITE.GetValue(),
            enumtoString().CompareOperator2String(
                (enCompareOperator)pSpec->WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
            pDefect->EDGE_ENERGY_SHARPNESS_WHITE.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Sharpness_EdgeEnergy_White, result, strText);
    }
    return result;
}

enLogicResult CISIClassification::EdgeEnergySharpnessWhite_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue(),
        pSpec->EDGE_ENERGY_SHARPNESS_WHITE_2, pDefect->EDGE_ENERGY_SHARPNESS_WHITE);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->EDGE_ENERGY_SHARPNESS_WHITE_2.GetValue(),
            enumtoString().CompareOperator2String(
                (enCompareOperator)pSpec->WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
            pDefect->EDGE_ENERGY_SHARPNESS_WHITE.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Sharpness_EdgeEnergy_White, result, strText);
    }
    return result;
}

enLogicResult CISIClassification::EdgeEnergySharpnessBlack_CompareOperator( CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue(),
        pSpec->EDGE_ENERGY_SHARPNESS_BLACK, pDefect->EDGE_ENERGY_SHARPNESS_BLACK);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->EDGE_ENERGY_SHARPNESS_BLACK.GetValue(),
            enumtoString().CompareOperator2String(
                (enCompareOperator)pSpec->BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
            pDefect->EDGE_ENERGY_SHARPNESS_BLACK.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Sharpness_EdgeEnergy_Black, result, strText);
    }
    return result;
}

enLogicResult CISIClassification::EdgeEnergySharpnessBlack_CompareOperator_2(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView)
{
    enLogicResult result = enLogicResult::NO;

    result = isCompareOperator<float>((enCompareOperator)pSpec->BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue(),
        pSpec->EDGE_ENERGY_SHARPNESS_BLACK_2, pDefect->EDGE_ENERGY_SHARPNESS_BLACK);
    if (bView)
    {
        CString strText;
        strText.Format(_T("%.3f %s %.3f"), pSpec->EDGE_ENERGY_SHARPNESS_BLACK_2.GetValue(),
            enumtoString().CompareOperator2String(
                (enCompareOperator)pSpec->BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
            pDefect->EDGE_ENERGY_SHARPNESS_BLACK.GetValue());
        pDefect->SetLogicStep(GetDefectName(pSpec->GetDefectType()), enLogicKey::Sharpness_EdgeEnergy_Black, result, strText);
    }
    return result;
}

enLogicResult CISIClassification::isCategory(enCategory nSpecCategory, CISIDefectProperty* pDefect)
{
	enLogicResult result = isCompareOperator<long>(enCompareOperator::SAME, nSpecCategory, pDefect->nCategory.GetValue());

	CString strText;
	strText.Format(_T("%s = %s"),
		enumtoString().Category(nSpecCategory),
		enumtoString().Category((enCategory)pDefect->nCategory.GetValue()));

	pDefect->SetLogicStep(_T("CATEGORY"), enLogicKey::Category, result, strText);
	return result;
}

bool CISIClassification::CheckAttachedROI(const BOOL bROIDirLeft, const IPVM::Rect rtInspROI, const CRect DefectRoI)
{
	//검사 영역ROI 끝쪽에 불량이 붙어있는지 확인하는 함수
	if (rtInspROI.IsRectEmpty())
		return false;

	if (false == bROIDirLeft)
	{
		if (rtInspROI.m_right - 1/*필요한가?*/ <= DefectRoI.right)
			return true;
	}
	else
	{
		if (rtInspROI.m_left + 1 >= DefectRoI.left)
			return true;

	}

	return false;
}

//template <typename T>
//enLogicResult CISIClassification::isCompareOperator(enCompareOperator CompareOperator, const T Spec, const T Defect)
//{
//
//	switch (CompareOperator)
//	{
//	case enCompareOperator::SAME:
//		if (Spec == Defect)
//			return enLogicResult::YES;
//		break;
//
//	case enCompareOperator::BIGGER_THAN_SPEC:
//		if (Spec <= Defect)
//			return enLogicResult::YES;
//		break;
//
//	case enCompareOperator::SMALLER_THAN_SPEC:
//		if (Spec > Defect)
//			return enLogicResult::YES;
//		break;
//
//	default:
//		ASSERT(1);
//		break;
//	}
//
//	return enLogicResult::NO;
//}


//Logic Name::Size 고유 Key에요. 가능하면 함수이름과 고유키는 수정하지 않는다. 
enLogicResult CISIClassification::isSize(CISIDefectProperty* pDefect, CISISpecInfo* pSpec, bool bView)
{
	bool Logic_AND = pSpec->ORAND;
	float fSpecSizeX_mm = pSpec->SIZE_X_mm;
	float fSpecSizeY_mm = pSpec->SIZE_Y_mm;

	enLogicResult result = enLogicResult::NO;

	enLogicResult LR_SIZE_X = SizeX_CompareOperator(pSpec, pDefect, bView);
    enLogicResult LR_SIZE_Y = SizeY_CompareOperator(pSpec, pDefect, bView);

	
	if (false == Logic_AND)
	{
		if (enLogicResult::YES == LR_SIZE_X || enLogicResult::YES == LR_SIZE_Y)

		{
			result = enLogicResult::YES;
		}
	}
	else
	{
		if (enLogicResult::YES == LR_SIZE_X && enLogicResult::YES == LR_SIZE_Y)
			result = enLogicResult::YES;
	}

	return result;
}



II_RESULT_VALUE CISIClassification::Judge(DefectType defectname, enLogicResult LogisRet, CISIDefectProperty* pDefect)
{
	if (enLogicResult::YES == LogisRet)
	{
		return pDefect->SetJudge(defectname, II_RESULT_VALUE::REJECT);
	}
	else
	{
		DefectType retDefetType = CheckSmallDefect(defectname, pDefect);
		return pDefect->SetJudge(retDefetType, II_RESULT_VALUE::PASS);
	}
}


DefectType CISIClassification::CheckSmallDefect(DefectType defectname, CISIDefectProperty* pDefect)
{

	switch (defectname)
	{
	case DefectType::Scratch:
	{
		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_ScratchLevel1);
		if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
			return DefectType::SmallDefect_ScratchLevel1;

        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_ScratchLevel2);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_ScratchLevel2;

        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_ScratchLevel3);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_ScratchLevel3;

        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_ScratchLevel4);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_ScratchLevel4;

        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_ScratchLevel5);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_ScratchLevel5;
	}
	break;

#if INSPECTION_MACHINE_UC1
    case DefectType::White_Pinhole:
#else
    case DefectType::Pinhole:
#endif // INSPECTION_MACHINE_UC1 
	{
		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_PinholeLevel1);
		if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
			return DefectType::SmallDefect_PinholeLevel1;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_PinholeLevel2);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_PinholeLevel2;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_PinholeLevel3);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_PinholeLevel3;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_PinholeLevel4);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_PinholeLevel4;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_PinholeLevel5);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_PinholeLevel5;
	}
	break;

#if INSPECTION_MACHINE_UC1
    case DefectType::Crater:
#else
    case DefectType::Stain_Pinhole:
#endif // INSPECTION_MACHINE_UC1 
	//case DefectType::Stain_Pinhole:
	{

		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_CraterLevel4);
		if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
			return DefectType::SmallDefect_CraterLevel4;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_CraterLevel3);
		if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
			return DefectType::SmallDefect_CraterLevel3;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_CraterLevel2);
		if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
			return DefectType::SmallDefect_CraterLevel2;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_CraterLevel1);
		if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
			return DefectType::SmallDefect_CraterLevel1;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_CraterLevel5);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_CraterLevel5;
	}
	break;
#if INSPECTION_MACHINE_UC1
    case DefectType::Delami:
#else
    case DefectType::Delamination_Crack:
#endif // INSPECTION_MACHINE_UC1 
	//case DefectType::Delamination_Crack:
	{
		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_ProtrutionLevel1);
		if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_ProtrutionLevel1;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_ProtrutionLevel2);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_ProtrutionLevel2;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_ProtrutionLevel3);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_ProtrutionLevel3;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_ProtrutionLevel4);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_ProtrutionLevel4;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_ProtrutionLevel5);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_ProtrutionLevel5;
	}
	break;
    
#if INSPECTION_MACHINE_UC1
    case DefectType::Foil_Island:  
#else
    case DefectType::Island:
#endif // INSPECTION_MACHINE_UC1 
	//case DefectType::Island:
	{
		CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_IslandLevel1);
		if (enLogicResult::YES == isSize(pDefect, pSpec))
			return DefectType::SmallDefect_IslandLevel1;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_IslandLevel2);
        if (enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_IslandLevel2;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_IslandLevel3);
        if (enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_IslandLevel3;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_IslandLevel4);
        if (enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_IslandLevel4;

		pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_IslandLevel5);
        if (enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_IslandLevel5;
	}
	break;
    case DefectType::Dent:
    {
        CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_DentLevel1);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_DentLevel1;

        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_DentLevel2);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_DentLevel2;

        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_DentLevel3);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_DentLevel3;

        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_DentLevel4);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_DentLevel4;

        pSpec = m_pSpecInfo->GetSpecInfo(DefectType::SmallDefect_DentLevel5);
        if (true == pSpec->GetEnable() && enLogicResult::YES == isSize(pDefect, pSpec))
            return DefectType::SmallDefect_DentLevel5;
    }
    break;
	default:
		break;
	}

	

	return defectname;
}

bool CISIClassification::MultiLogic_VecVec(int index, CISICellAllDefect* cellDefect, std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec, BOOL bView,
    std::vector<std::vector<std::vector<std::pair<enLogicResult (*)(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView), DefectType>>>> fp)
{
    CISIDefectProperty* pDefect = &cellDefect->m_vecCandidate[index];

    bool bRet = true;
    pDefect->InitResult();
    pDefect->SetJudge(DefectType::END, II_RESULT_VALUE::PASS);

    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>>::iterator vecitrStartVec, vecitrEndVec;

    vecitrStartVec = vecSpec.begin();
    vecitrEndVec = vecSpec.end();

    for (long nIndex = 0; nIndex < fp.size(); nIndex++)
    {
        for (long nSize = 0; nSize < fp[nIndex].size(); nSize++)
        {
            BOOL bNO = FALSE;
            BOOL bYES = FALSE;
            CISISpecInfo Specinfo = vecSpec[nIndex].second[nSize];

            for (long nSpec = 0; nSpec < fp[nIndex][nSize].size(); nSpec++)
            {
                if (fp[nIndex][nSize][nSpec].second == vecSpec[nIndex].first)
                {
                    if (Specinfo.nInspectionArea == pDefect->nInspectionArea)
                    {
                        long nLoginResult = enLogicResult::NO;

                        if (enLogicResult::YES == fp[nIndex][nSize][nSpec].first(&Specinfo, pDefect, bView)) // 이것도 확인 필요
                        {
                            //YES 일때 런이면 확인 하지마?
                            nLoginResult = enLogicResult::YES;
                            bYES = TRUE;
                        }
                        else
                        {
                            bNO = TRUE;
                        }
                    }
                }
            }   
            if (bNO == FALSE && bYES == TRUE)
            {
                //if (Judge(pSpec->GetDefectType(), isSize(pDefect, pSpec), pDefect) == II_RESULT_VALUE::REJECT)
                if (isSize(pDefect, &Specinfo, bView) == enLogicResult::YES)
                {
                    pDefect->SetJudge(Specinfo.GetDefectType(), II_RESULT_VALUE::REJECT);
                    return bRet;
                }
                else
                {
                    pDefect->PARENT = DefectType2Parent(vecSpec[nIndex].first);
                    pDefect->SetJudge(Specinfo.GetDefectType(), II_RESULT_VALUE::PASS);
#if INSPECTION_MACHINE_UC1

#else
                    // 검사 판정시 사이즈 리젝 아니면 해당 이름으로 패스 내기

                    return bRet;
#endif // INSPECTION_MACHINE_UC1                    
                }
            }
        }
    }
}

bool CISIClassification::MultiLogic(int index, CISICellAllDefect* cellDefect, std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec, BOOL bView,
    std::vector<std::pair<enLogicResult (*)(CISISpecInfo* pSpec, CISIDefectProperty* pDefect, BOOL bView), DefectType>> fp)
{
    CISIDefectProperty* pDefect = &cellDefect->m_vecCandidate[index];
    // 현식 크래시피케이션 돌아가는 부분
    bool bRet = true;
    pDefect->InitResult();
    pDefect->SetJudge(DefectType::END, II_RESULT_VALUE::PASS);

    //std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec(m_pSpecInfo->m_mapSpec_multi.begin(), m_pSpecInfo->m_mapSpec_multi.end());

    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>>::iterator vecitrStartVec, vecitrEndVec;
    vecitrStartVec = vecSpec.begin();
    vecitrEndVec = vecSpec.end();
    //vecSpec[0].second
    long nCur = 0;
    while (vecitrStartVec != vecitrEndVec)
    {
        std::vector<CISISpecInfo>* vecpSpec = m_pSpecInfo->GetSpecInfo_Multi(vecitrStartVec->first);

        for (long n = 0; n < vecitrStartVec->second.size(); n++)
        {
            BOOL bNO = FALSE;
            BOOL bYES = FALSE;
            CISISpecInfo Specinfo = vecitrStartVec->second[n];  

            for (long nSize = 0; nSize < fp.size(); nSize++)
            {
                if (fp[nSize].second == vecitrStartVec->first)
                {
                    if (Specinfo.nInspectionArea == pDefect->nInspectionArea)
                    {
                        long nLoginResult = enLogicResult::NO;
                        if (enLogicResult::YES == fp[nSize].first(&Specinfo, pDefect, bView)) // 이것도 확인 필요
                        {
                            //YES 일때 런이면 확인 하지마?
                            nLoginResult = enLogicResult::YES;
                            bYES = TRUE;
                        }
                        else
                        {
                            bNO = TRUE;
                        }
                    }

					
				}
            }
            if (bNO == FALSE && bYES == TRUE)
            {
                //if (Judge(pSpec->GetDefectType(), isSize(pDefect, pSpec), pDefect) == II_RESULT_VALUE::REJECT)
                if (isSize(pDefect, &Specinfo, bView) == enLogicResult::YES)
                {
                    pDefect->SetJudge(Specinfo.GetDefectType(), II_RESULT_VALUE::REJECT);
                    return bRet;
                }
            }
            nCur++;
        }
        
        vecitrStartVec++;
    }
  //  while (vecitrStartVec != vecitrEndVec)
  //  {

  //      std::vector<CISISpecInfo>* vecpSpec = m_pSpecInfo->GetSpecInfo_Multi(vecitrStartVec->first);
  //      

		//for (long n = 0; n < vecitrStartVec->second.size(); n++ )
  //      {

  //          CISISpecInfo Specinfo = vecitrStartVec->second[n];
 
  //          if (Specinfo.ENABLE == true)
  //          {

  //              if (Specinfo.nInspectionArea == pDefect->nInspectionArea)
  //              {
  //                  long nLoginResult = enLogicResult::NO;
  //                  BOOL bNO = FALSE;
  //                  BOOL bYES = FALSE;

  //                  if (Specinfo.AREA_RATIO_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioArea_CompareOperator(&Specinfo, pDefect, bView)) // 이것도 확인 필요
  //                      {
  //                          //YES 일때 런이면 확인 하지마?
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
		//			//_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.AREA_RATIO_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioArea_CompareOperator(&Specinfo, pDefect, bView)) // 이것도 확인 필요
  //                      {
  //                          //YES 일때 런이면 확인 하지마?
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.WIDTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Width_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.WIDTH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Width_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.LENGTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Length_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.LENGTH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Length_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.RATIO_WL_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioWL_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.RATIO_WL_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioWL_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.RATIO_LW_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioLW_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.RATIO_LW_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioLW_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.RATIO_YX_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioYX_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.RATIO_YX_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioYX_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.RATIO_XY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioXY_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.RATIO_XY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioXY_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.ANGLE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Angle_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.ANGLE_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Angle_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.STD_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == STD_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.STD_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == STD_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.AVG_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == AVG_GV_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.AVG_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == AVG_GV_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.MIN_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == MIN_GV_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.MIN_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == MIN_GV_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.MAX_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == MAX_GV_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.MAX_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == MAX_GV_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  /*if (pSpec->BRIGHT_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
		//		{
		//		}
		//		if (pSpec->DARK_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
		//		{
		//		}*/
  //                  if (Specinfo.COMPACT_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Compact_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.COMPACT_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Compact_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.SECTION_THRESHOLD_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == SectionTh_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.SECTION_THRESHOLD_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == SectionTh_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //if (pSpec->SECTION_AREA_COMPARE_OPERATOR != enCompareOperator::NOT_USE) // 이거 안쓰네 ...이거 뭔지 다시 체크 필요
  //                  //{
  //                  //	if (enLogicResult::YES == Area_CompareOperator(pSpec, pDefect))
  //                  //	{
  //                  //		//YES 일때 런이면 확인 하지마?
  //                  //	}
  //                  //}
  //                  if (Specinfo.RATIO_WHITE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioWhite_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.RATIO_WHITE_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioWhite_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.RATIO_DARK_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioDark_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.RATIO_DARK_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioDark_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == EdgeEnergy_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == EdgeEnergy_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.BLACK_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == EdgeEnergyBlack_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == EdgeEnergyBlack_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.WHITE_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == EdgeEnergyWhite_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == EdgeEnergyWhite_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.AREA_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Area_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.AREA_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Area_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.MASS_CENTER_GV_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == MassCenterGV_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.MASS_CENTER_GV_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == MassCenterGV_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.HOR_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == HOR_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.HOR_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == HOR_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.VER_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == VER_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.VER_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == VER_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.RATIO_HV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioHV_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.RATIO_HV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioHV_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.RATIO_VH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioVH_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.RATIO_VH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == RatioVH_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.OVER_GV_PX_COUNT_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == OverPxCount_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.OVER_GV_PX_COUNT_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == OverPxCount_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.DIS_TO_EDGE_mm_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Dis2Edge_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.DIS_TO_EDGE_mm_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Dis2Edge_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.DIS_TO_EDGE_2_mm_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Dis2Edge_2_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Dis2Edge_2_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.IS_NEAR_EDGE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == IsNearEdge_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.IS_NEAR_EDGE_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == IsNearEdge_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.BRIGHT_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Bright_AVG_GV_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.BRIGHT_AVG_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Bright_AVG_GV_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (Specinfo.DARK_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Dark_AVG_GV_CompareOperator(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  //_2 비교 함수 추가 "(작)>" 비교
  //                  if (Specinfo.DARK_AVG_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
  //                  {
  //                      if (enLogicResult::YES == Dark_AVG_GV_CompareOperator_2(&Specinfo, pDefect, bView))
  //                      {
  //                          nLoginResult = enLogicResult::YES;
  //                          bYES = TRUE;
  //                      }
  //                      else
  //                      {
  //                          bNO = TRUE;
  //                      }
  //                  }
  //                  if (bNO == FALSE && bYES == TRUE)
  //                  {
  //                      //if (Judge(pSpec->GetDefectType(), isSize(pDefect, pSpec), pDefect) == II_RESULT_VALUE::REJECT)
  //                      if (isSize(pDefect, &Specinfo) == enLogicResult::YES)
  //                      {
  //                          pDefect->SetJudge(Specinfo.GetDefectType(), II_RESULT_VALUE::REJECT);
  //                          return bRet;
  //                      }
  //                      else
  //                      {
  //                          // pDefect->PARENT = DefectType2Parent(itrStart->first);
  //                          //pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::PASS);

  //                          // 스몰디펙 불량이면 여기서 스몰디펙 돌리기...
  //                      }
  //                      /*else
		//			{
		//				Judge(pSpec->GetDefectType(), enLogicResult::NO, pDefect);
		//			}*/
  //                  }
  //                  /*else if (bNO == TRUE && bYES == TRUE)
		//		{
		//			pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::PASS);
		//		}*/
  //              }
  //          }

		//}
  //      
  //      vecitrStartVec++;
  //      // itrStart++;
  //  }

    //pDefect->SetJudge(pSpec->GetDefectType(), II_RESULT_VALUE::PASS);
    //CISISpecInfo* pSpec = m_pSpecInfo->GetSpecInfo(DefectType::Pinhole);
    /*if (enLogicResult::NO == SectionTh_CompareOperator(pSpec, pDefect))
	{
		Judge(pSpec->GetDefectType(), enLogicResult::NO, pDefect);
		return true;
	}

	enLogicResult LRLength = Length_CompareOperator(pSpec, pDefect);
	enLogicResult LRWidth = Width_CompareOperator(pSpec, pDefect);
	if (enLogicResult::YES == LRLength && enLogicResult::YES == LRWidth)
	{
		Judge(pSpec->GetDefectType(), enLogicResult::YES, pDefect);
	}
	else
	{
		Judge(pSpec->GetDefectType(), enLogicResult::NO, pDefect);
	}*/

    //////////////////////////////////////
    //switch (pDefect->nInspectionArea)
    //{
    //case enInspectionArea::IA_BACKGROUND_BURR:
    //	bRet = isBackGroundBurr(pDefect);
    //	break;

    //case enInspectionArea::IA_SCRAB:
    //	bRet = isScrab(index, cellDefect);
    //	break;

    //case enInspectionArea::IA_TAB:
    //	bRet = isTab(index, cellDefect);
    //	break;

    //case enInspectionArea::IA_TAB_INSULATION:
    //case enInspectionArea::IA_TAB_INSULATION_GAP:
    //case enInspectionArea::IA_CUTTINGHEIGHT_TOP:
    //case enInspectionArea::IA_CUTTINGHEIGHT_BOT:
    //	bRet = isInsulation(index, cellDefect);
    //	break;

    //case enInspectionArea::IA_ELECTRODE_EDGE_LEFT:
    //case enInspectionArea::IA_ELECTRODE_EDGE_RIGHT:
    //	bRet = isElectrodeEdge(pDefect);
    //	break;

    //case enInspectionArea::IA_ELECTRODE:
    //	bRet = isElectrode(index, cellDefect);
    //	break;

    //default:
    //	ASSERT(1);
    //}

    return bRet;
}