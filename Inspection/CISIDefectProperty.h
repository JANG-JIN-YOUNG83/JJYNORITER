#pragma once

#include "basedef.h"
#include "CISIDefectResult.h"

#include "Types/Image_8u_C1.h"
#include "Types/Quadrangle_32f.h"



#include "../VisionModule/Base/ParaBool.h"
#include "../VisionModule/Base/ParaFloat.h"
#include "../VisionModule/Base/ParaLong.h"
#include "../VisionModule/Base/ParaRect.h"

using namespace classification;

class CISIDefectProperty : private CISIDefectResult
{
public:
	CISIDefectProperty::CISIDefectProperty();

	Base::ParaLong nInspectImage;				// 밝은 or 어두운 이미지
	Base::ParaLong nDefectBrightness;			// white or dark 불량
	Base::ParaLong nInspectionArea;				// 검사 영역
	Base::ParaLong nCategory;					//

	Base::ParaLong BLOB_THRESHOLD;				//128전처리 이미지 위/아래 마진으로 찾은 후보들
	Base::ParaLong SECTION_THRESHOLD;			//화이트성 불량 : 최대값 - 평균값, 블랙성 불량은 : 평균값 - 최솟값

	Base::ParaRect rtRect_pixel;				//불량 위치
	Base::ParaFloat SIZE_X_mm;
	Base::ParaFloat SIZE_Y_mm;
	Base::ParaFloat AREA_Ratio;
	Base::ParaFloat WIDTH_mm;
	Base::ParaFloat LENGTH_mm;
	Base::ParaFloat RATIO_WL;
	Base::ParaFloat RATIO_LW;
	Base::ParaFloat RATIO_YX;
	Base::ParaFloat RATIO_XY;
	Base::ParaFloat ANGLE;
	Base::ParaFloat STD;
	Base::ParaFloat COMPACT;
	Base::ParaLong AVG_GV;
	Base::ParaLong MIN_GV;
	Base::ParaLong MAX_GV;
	Base::ParaLong BRIGHT_AVG_GV;
	Base::ParaLong DARK_AVG_GV;
    Base::ParaLong PRIORITY;

	Base::ParaFloat AREA; 

	// [sm]
	Base::ParaFloat RATIO_WHITE;
	Base::ParaFloat RATIO_DARK;
    Base::ParaFloat EDGE_ENERGY;
    Base::ParaFloat EDGE_ENERGY_WHITE;
    Base::ParaFloat EDGE_ENERGY_BLACK;
	//Base::ParaFloat RATIO_AREA;
    Base::ParaFloat MASS_CENTER_GV_GV;
    Base::ParaFloat HOR;
    Base::ParaFloat VER;
    Base::ParaFloat RATIO_HV;
    Base::ParaFloat RATIO_VH;
    Base::ParaBool SMALL_INSP;
    Base::ParaLong SMALL_INSP_LEVEL;
    Base::ParaLong PARENT;

	Base::ParaLong OVER_200_GV_PIXEL_COUNT;
    Base::ParaFloat DISTANCE_TO_EDGE_mm;
    Base::ParaFloat DISTANCE_TO_EDGE_2_mm;
    Base::ParaLong IS_NEAR_EDGE;

	Base::ParaFloat EDGE_ENERGY_SHARPNESS;
    Base::ParaFloat EDGE_ENERGY_SHARPNESS_WHITE;
    Base::ParaFloat EDGE_ENERGY_SHARPNESS_BLACK;

	Base::ParaFloat MEDIAN_GV;
    Base::ParaFloat STDEV_GV;
	IPVM::Image_8u_C1	srcImage;
	CString strTempUI;
    IPVM::Quadrangle_32f quadROI;

	// 장지수 각 조건 값의 사잇값을 넣기 위해 동일 변수 추가 진행
    Base::ParaFloat AREA_Ratio_2;
    Base::ParaFloat WIDTH_mm_2;
    Base::ParaFloat LENGTH_mm_2;
    Base::ParaFloat RATIO_WL_2;
    Base::ParaFloat RATIO_LW_2;
    Base::ParaFloat RATIO_YX_2;
    Base::ParaFloat RATIO_XY_2;
    Base::ParaFloat ANGLE_2;
    Base::ParaFloat STD_2;
    Base::ParaFloat COMPACT_2;
    Base::ParaLong AVG_GV_2;
    Base::ParaLong MIN_GV_2;
    Base::ParaLong MAX_GV_2;
    Base::ParaLong BRIGHT_AVG_GV_2;
    Base::ParaLong DARK_AVG_GV_2;
    //Base::ParaLong PRIORITY_2;

    Base::ParaFloat AREA_2;

    // [sm]
    Base::ParaFloat RATIO_WHITE_2;
    Base::ParaFloat RATIO_DARK_2;
    Base::ParaFloat EDGE_ENERGY_2;
    Base::ParaFloat EDGE_ENERGY_WHITE_2;
    Base::ParaFloat EDGE_ENERGY_BLACK_2;
    //Base::ParaFloat RATIO_AREA;
    Base::ParaFloat MASS_CENTER_GV_GV_2;
    Base::ParaFloat HOR_2;
    Base::ParaFloat VER_2;
    Base::ParaFloat RATIO_HV_2;
    Base::ParaFloat RATIO_VH_2;
    Base::ParaBool SMALL_INSP_2;
    Base::ParaLong SMALL_INSP_LEVEL_2;
    Base::ParaLong OVER_200_GV_PIXEL_COUNT_2;
    Base::ParaFloat DISTANCE_TO_EDGE_mm_2;
    Base::ParaFloat DISTANCE_TO_EDGE_2_mm_2;
    Base::ParaLong IS_NEAR_EDGE_2;
    Base::ParaLong SECTION_THRESHOLD_2; //화이트성 불량 : 최대값 - 평균값, 블랙성 불량은 : 평균값 - 최솟값
    Base::ParaFloat EDGE_ENERGY_SHARPNESS_2;
    Base::ParaFloat EDGE_ENERGY_SHARPNESS_WHITE_2;
    Base::ParaFloat EDGE_ENERGY_SHARPNESS_BLACK_2;
	//result
	void SetLogicStep(CString strDefectName, long nKey, enLogicResult nNO0_YES1, CString strNote = _T(""))
	{
		CISIDefectResult::SetLogicStep(strDefectName, nKey, nNO0_YES1, strNote);
	}

	int GetLogicStepNum()
	{
		return CISIDefectResult::GetLogicStepNum();
	}

	void GetLogicStep(long index, CString& strDefectName, long& nKey, classification::enLogicResult& nNO0_YES1, CString& strNote)
	{
		return CISIDefectResult::GetLogicStep(index, strDefectName, nKey, nNO0_YES1, strNote);
	}

	void InitResult()
	{
		CISIDefectResult::InitResult();
	}

	II_RESULT_VALUE SetJudge(DefectType DefectName, II_RESULT_VALUE Judge)
	{
		return CISIDefectResult::SetJudge(DefectName, Judge);
	}

	II_RESULT_VALUE GetJudge()
	{
		return CISIDefectResult::GetJudge();
	}

	DefectType GetDefectResultCode()
	{
		return CISIDefectResult::GetDefectResultCode();
	}
};

