#pragma once

#include "basedef.h"
#include "Types/Image_8u_C4.h"
#include "Types/Image_8u_C1.h"
namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ResultDefectROI : public CommonShare::BaseObject
	{
	public:
		ResultDefectROI();
		~ResultDefectROI();
		ResultDefectROI(CRect rtShowROI, IPVM::Image_8u_C4* src);

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		int			m_nIndex;								// Lot Start 후 발생한 Defect의 순서 (Surface 불량이 발생할때마다 ++해서 주면 됨)

		double		m_dRelative_X;							// 불량 발생위치 X [mm] (검사영역 기준)
		double		m_dRelative_Y;							// 불량 발생 위치 Y [mm] (검사영역 기준)
        double		m_dRelative_Img_X;						// Image 기준 불량 발생 위치 X [pxl]_Cell 기반
        double		m_dRelative_Img_Y;						// Image 기준 불량 발생 위치 Y [pxl]_Cell 기반
		double		m_dWidth;								// 불량 검출 Width [mm]
		double		m_dLength;								// 불량 검출 Length [mm]
		double		m_dImg_Width;							// Image 기준 Width [pxl]
		double		m_dImg_Length;							// Image 기준 Length [pxl]
		double		m_dArea_MM2;							// 불량 발생 면적 [mm2]	-> 크기를 의미하는듯?
		double		m_dArea_Pixels;							// Image 기준 불량 발생 면적 [pxl] -> 크기를 의미하는듯?
		double		m_dRatio_Width_Length;					// 불량 검출 영역 폭:길이 비율

		double		m_dGray_LV_Avg;							// 불량 검출 영역 평균 Gray Level 값
		double		m_dGray_LV_Max;							// 불량 검출 영역 최대 Gray Level 값
		double		m_dGray_LV_Min;							// 불량 검출 영역 최소 Gray Level 값
		double		m_dGray_LV_Range;						// 불량 검출 영역 내 Gray Level 범위
		double		m_dGray_LV_Stddev;						// 불량 Gray Level의 표준편차

		double		m_dRoi_Gray_LV_Avg;						// ROI 내 평균 Gray Level 값
		double		m_dRoi_Gray_LV_Max;						// ROI 내 최대 Gray Level 값
		double		m_dRoi_Gray_LV_Min;						// ROI 내 최소 Gray Level 값
		double		m_dRoi_Gray_LV_Median;					// ROI 내 중앙값 Gray Level 값
		double		m_dRoi_Gray_LV_Stddev;					// ROI 내 표준편차 Gray Level 값

		CRect		m_rtShowROI;							// ROI -> 불량 위치

		CString		m_strDefect_Position;					// 외관불량 측정위치 (COAT, NON_COAT, INSULATION)
		CString		m_strDefect_Color;						// 외관불량 색 (BLACK, WHITE)
		int			m_nDefect_Lane;							// 해당 불량이 속해 있는 Lane 위치 (1,2,3 ...N)
		bool		m_bDefect_Left;							// 해당 불량의 위치가 전체 X축 길이대비 정 중앙의 왼쪽에 있으면 True, 아니면 False (ReWinder로 나눠지는 기준을 위해)
	
		IPVM::Image_8u_C1 m_DstCropImage_Bright;
        IPVM::Image_8u_C1 m_DstCropImage_Dark;
		// 내부 변수 (HOST)
		IPVM::Image_8u_C4	m_DstCropImage;					// 해당 외관 Crop 불량 이미지
		CString				m_strImage_File_Name_json;		// 해당 외관 Crop 불량 이미지 경로 (파일명 형식 : 검사항목 구분_②파일생성시간_③설비ID_④Lot ID_⑤검사위치_⑥X좌표_⑦Y좌표_⑧불량유형.jpg)
        bool		m_bDataSave;
	};
}
