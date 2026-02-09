#pragma once
#include "basedef.h"
#include "Types/Image_8u_C1.h"
#include "../Inspection/basedef.h"

class CISIViewerProperty
{
public:
    CISIViewerProperty()
        : bDisplayDefectView(true)
	{
	};

    bool bDisplayDefectView;

	CString strLotID; // 
	long nCellIndex; // 
	long nUp;  


	long nInspectImage;				// 밝은 or 어두운 이미지
	long nDefectBrightness;			// white or dark 불량
	long nInspectionArea;				// 검사 영역
	long nCategory;					//
	long nBLOB_THRESHOLD;				//128전처리 이미지 위/아래 마진으로 찾은 후보들
	long nSECTION_THRESHOLD;			//화이트성 불량 : 최대값 - 평균값, 블랙성 불량은 : 평균값 - 최솟값
	long nAVG_GV;
	long nMIN_GV;
	long nMAX_GV;
	long nBRIGHT_AVG_GV;
	long nDARK_AVG_GV;
	float fSIZE_X_mm;
	float fSIZE_Y_mm;
	float fAREA_Ratio;
	float fWIDTH_mm;
	float fLENGTH_mm;
	float fRATIO_WL;
	float fRATIO_LW;
	float fRATIO_YX;
	float fRATIO_XY;
	float fANGLE;
	float fSTD;
	float fCOMPACT;
	float fAREA;
	float fRATIO_WHITE;
	float fRATIO_DARK;
	float fEDGE_ENERGY;
	//float fRATIO_AREA;
	float fMASS_CENTER_GV_GV;
	float fHOR;
	float fVER;
	float fRATIO_HV;
	float fRATIO_VH;
    long nDefectX;
    long nDefectY;
    long m_nJudge;
    float fEDGE_ENERGY_SHARPNESS;
    float fEDGE_ENERGY_SHARPNESS_WHITE;
    float fEDGE_ENERGY_SHARPNESS_BLACK;

	float fMEDIAN_GV;
    float fSTDEV_GV;
    CString pathDirectBright;
    CString pathDirectDark;
    CString pathCropBright;
    CString pathCropDark;
    CString pathCropOversizeBright;
    CString pathCropOversizeDark;
	
	long nOverPxCount;

	IPVM::Rect rtRect_pixel;				//불량 위치
	IPVM::Image_8u_C1	srcImage;
	IPVM::Image_8u_C1	brightImage;
	IPVM::Image_8u_C1	darkImage;

	long m_nCandidateDefectCode;

	CString strTime;
};

