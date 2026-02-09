#pragma once

#include "basedef.h"
#include "ImageInfo.h"
#include <vector>

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ CameraInfo : public CommonShare::BaseObject
	{
	public:
		CameraInfo();
		~CameraInfo();

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
		CameraInfo& operator = (const CameraInfo &src);

		int							m_nCamera_Location;					// TOP : 0, Bottom : 1
		int							m_nCamera_Number;					// 상부 or 하부 카메라가 n개인경우
		double						m_dResolution_X;					// 분해능 X
		double						m_dResolution_Y;					// 분해능 Y
		int							m_nCamera_Gain;						// 카메라 Gain value
		int							m_nExposure_Time;		 			// Exposure Time value
		float						m_fAngle;							// Angle

		std::vector<ImageInfo>		m_vecImageQuality;					// Lane Edge Crop 이미지 관련 (Lane이 존재할 경우 각 Lane의 외곽면에 대한 Crop Image를 넣어줘야한다)
	};
}
