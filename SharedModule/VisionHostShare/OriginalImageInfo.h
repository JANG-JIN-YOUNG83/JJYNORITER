#pragma once

#include "basedef.h"
#include "Types/Image_8u_C1.h"

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ OriginalImageInfo : public CommonShare::BaseObject
	{
	public:
		OriginalImageInfo();
		OriginalImageInfo(const OriginalImageInfo& rhs);
		OriginalImageInfo(int nCa, int nCh, IPVM::Image_8u_C1* src);
		~OriginalImageInfo();


		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
		OriginalImageInfo& operator = (const OriginalImageInfo& src);

		int nCamera;		// 0: UP, 1: DOWN
		int nChannel;		// 0: Bright, 1: Dark	
		int nSizeX;
		int nSizeY;
		IPVM::Image_8u_C1 srcImg;
	};
}
