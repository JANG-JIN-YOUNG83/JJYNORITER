#pragma once
#include "Types/Image_8u_C1.h"
#include "Types/Rect.h"
#include <vector>

class InspectionInputData
{
public:
	InspectionInputData();
	~InspectionInputData();

public:
	CString				m_imageSavePath;	// 이미지를 획득시점에 저장했다면 이미지의 경로
	IPVM::Image_8u_C1	m_imageFull;
	IPVM::Image_8u_C1	m_imageBright;
	IPVM::Image_8u_C1	m_imageDark;
	IPVM::Rect			m_vhomeTop;			// v홈1 위치는 어딘가
	IPVM::Rect			m_vhomeBot;			// v홈2 위치는 어딘가 
	long				m_cellID;
	int64_t				m_imagePos;			// 누적되어있는 픽셀 좌표
	bool				m_isUp;

	// 20200605
	float				m_grabTime_ms;
	float				m_inspectioinStartTime_ms;
	SYSTEMTIME			m_inputTime;

	//Area
    long				m_nSide;
};
