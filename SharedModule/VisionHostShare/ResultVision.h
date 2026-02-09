#pragma once

#include "basedef.h"
#include "CameraInfo.h"
#include "ImageInfo.h"
#include "ResultDefect.h"
#include "ResultDefectROI.h"
#include "ResultDimension.h"
#include "OriginalImageInfo.h"

#include <vector>

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ResultVision : public CommonShare::BaseObject
	{
	public:
		ResultVision();
		~ResultVision();

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
		ResultVision& operator = (const ResultVision &src);

		int GetNgCount();
        void Clear();
		// 결과 데이터 변수
		long							m_nCellindex;						// Continue 한 숫자 0 ~ xx (Top, Btm 둘 다 0 부터 시작)
		long							m_nTotalLength;						// 검사 이미지의 Y값 (단위 : mm) - 이값과 Index값을 곱연산하여 거리값 구할 예정
		SYSTEMTIME						m_systmVision_Input;				// 검사 Trigger 받은 시간
		SYSTEMTIME						m_systmVision_Output;				// 공란 (Host 내부에서 채워서 넣을 예정)
		CString							m_strRecipe_ID;						// 검사 Recipe
		double							m_dInspection_Processing_Time;		// 검사 시간(Grab + 처리) - ms 단위
		
		CString							m_strCell_Final_Judge;				// 판정 결과
		int								m_nTotal_Dimension_NG_Count;		// 해당 검사 영역의 치수 NG 개수
		int								m_nTotal_Missmatch_NG_Count;		// 해당 검사 영역의 Missmatch NG 개수
		int								m_nTotal_Surface_NG_Count;			// 해당 검사 영역의 외관 NG 개수
		int								m_nTotal_Insulation_NG_Count;		// 해당 검사 영역의 절연 NG 개수
		int								m_nLane_Count;						// 해당 검사 영역의 Lane 개수

		std::vector<DefectType>			m_vecSurface_Reason_All;			// 외관 불량명 전체 기입(csv파일 저장시 세미콜론 ' ; ' 으로 불량명 구분)
		std::vector<DefectType>			m_vecDimension_Reason_All;			// 치수 불량명 전체 기입(csv파일 저장시 세미콜론 ' ; ' 으로 불량명 구분)

		std::vector<CameraInfo>			m_vecCamera;						// IQ 관련 내용 (평소엔 값을 안넣어주고, HOST로부터 Request_JsonData MSG 받은 후 한번만 채워서 준다)
		std::vector<ResultDimension>	m_vecDimension;						// 치수 데이터
		std::vector<ResultDimension>	m_vecMismatch;						// 미스매치 데이터
		std::vector<ResultDefect>		m_vecDefect;						// 외관 데이터

		std::vector<OriginalImageInfo>	m_vecOriginalImage;					// 원본 이미지 (Channel 및 Camera위치 따른 Bright, Dark 구분해서 모두 포함)

		bool							m_bNGTag;							// NG Tag 부착 여부

		// 내부 변수 (HOST)
		long							m_nState;							// 0 : EMPTY, 1 FULL;
		CString							m_strSeqMeter_FromRW1;				// [PLC 기준 R/W1 미터정보(Encoder)] + [검사기 ~ R/W1간 거리(Option으로 설정할 Offset값)] -> 절연,Overlay 경우 소수점 셋째자리까지 표현하고 이외는 4자리로 표현
		CString							m_strSeqMeter_FromRW2;				// [PLC 기준 R/W2 미터정보(Encoder)] + [검사기 ~ R/W2간 거리(Option으로 설정할 Offset값)] -> 절연,Overlay 경우 소수점 셋째자리까지 표현하고 이외는 4자리로 표현
	};
}
