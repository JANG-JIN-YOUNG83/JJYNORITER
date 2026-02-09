#pragma once

#include "basedef.h"
#include "ShowList.h"

#include "Types/Image_8u_C4.h"

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ResultDimension : public CommonShare::BaseObject
	{
	public:
		ResultDimension();
		~ResultDimension();

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
		ResultDimension& operator=(const ResultDimension &src);

		// 치수 항목 변수
		int 				m_nCamera_Location;							// Camera 위치(TOP : 1, Bottom : 2)
		int					m_nCamera_Number;							// Camera Number(상부 또는 하부 카메라 n개인 경우)
		int					m_nScreen_Number;							// 1개 Camera에서 2장 Image 생성시 구분 인자

		// 밑 두개의 변수에 넣어줘야 하는 값의 규칙은 "SPC+ 검사정보 통신사양정의 v1.62" PPT의 39번 Tab을 참고
		CString				m_strPosition_InspItem;						// 치수 측정 위치 중 검사항목명만 넣어주는 변수(무지부(NCTW), 유지부(CTW), 하트 컷팅(HCTW), 포일(FLW), 미스매치(MIMA), 절연 코팅(INW), 절연 오버레이(OLW), 절연 갭(GAP))
		CString				m_strPosition_Sub;							// 치수 측정 위치 중 검사항목명 뒤 나머지 내용들

		DefectType			m_inspType;									// 치수 검사항목
		double				m_dValue;									// 치수 항목 측정 값 (소수점 3자리) (mm 단위)
		double				m_dLSL;										// 치수 항목 LSL
		double				m_dUSL;										// 치수 항목 USL
		CString				m_strJudge;									// 치수 항목 판정
		double				m_dGL;										// ROI 별 Gray Level
		
		double				m_dStart_Point_X;							// 치수 측정 Start Point X 좌표 Value
		double				m_dEnd_Point_X;								// 치수 측정 End Point X 좌표 Value
		double				m_dStart_Point_Y;							// 치수 측정 Start Point Y 좌표 Value
		double				m_dEnd_Point_Y;								// 치수 측정 End Point Y 좌표 Value

		bool				m_bDefect_Left;								// 해당 불량의 위치가 전체 X축 길이대비 정 중앙의 왼쪽에 있으면 True, 아니면 False (ReWinder로 나눠지는 기준을 위해)

		ShowList			m_stShowList;								// Vision에서 검사 ROI, Point 제공

		void ResetValues();												// 멤버 변수 초기화

		// 내부 변수 (HOST)
		CString				m_strResultImagePath;						// 치수 불량 이미지 경로 (HOST에서 전체이미지를 이용하여 만들 예정)
		IPVM::Image_8u_C4	m_DstResultImage;							// 치수 불량 이미지  (HOST에서 전체이미지를 이용하여 만들 예정)
	};
}
