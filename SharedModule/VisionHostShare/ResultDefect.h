#pragma once

#include "basedef.h"
#include "ResultDefectROI.h"
#include "Types/Image_8u_C4.h"

#include <vector>

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ResultDefect : public CommonShare::BaseObject
	{
	public:
		ResultDefect();
		~ResultDefect();

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
		ResultDefect& operator=(const ResultDefect &src);

		const ResultDefectROI* GetBestROI() const;

		// 외관 검사 항목 변수
		int			m_nCamera_Position;						// 카메라 위치 (TOP : 0, Bottom : 1)
		CString		m_strCamera_Type;						// 카메라 종류 (AREA, LINE)
		int			m_nCamera_Number;						// 상부 or 하부 카메라가 n개인경우
		int			m_nScreen_Number;						// 1개 Camera에서 2장 Image 생성시 구분 인자
		double		m_dCamera_Gain;							// 카메라 Gain value
		DefectType	m_inspType;								// 외관불량 검사항목
			

		std::vector<ResultDefectROI> m_vecstDefectROI;		// 외관불량에 대한 정보들 집약체 (여기 값이 있냐 없냐에 따라 OK,NG 판정)
	};
}
