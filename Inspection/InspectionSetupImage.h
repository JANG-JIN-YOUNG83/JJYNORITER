#pragma once
#include "basedef.h"
#include "Types/Image_8u_C1.h"

namespace Inspection
{
	class __INSPECTOIN_CLASS__ SetupImage
	{
	public:
		SetupImage();
		~SetupImage();

		// Up, Down 카메라 이미지
		IPVM::Image_8u_C1 m_imageBright[2];
		IPVM::Image_8u_C1 m_imageDark[2];
	};
}
