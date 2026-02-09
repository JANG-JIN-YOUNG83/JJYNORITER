#pragma once

#include "basedef.h"
#include "Types/Image_8u_C1.h"

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ImageInfo : public CommonShare::BaseObject
	{
	public:
		ImageInfo();
		~ImageInfo();
		ImageInfo(IPVM::Image_8u_C1 *src);

        virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		int nEdgeNum;													// 왼쪽 Edge부터 1,2,3...n순
		int nLaneNum;													// 왼쪽 Lane부터 1,2,3...n순
		int nSizeX;														// 이미지 X size
		int nSizeY;														// 이미지 Y size
		int	nFocus;														// 이미지 Focus Value
		//int	nGrayLevelValue[2];											// [0] : Edge면 중 밝은부분(NON_COAT)의 GrayValue값(평균값) / [1] : Edge면 중 어두운부분(COAT)의 GrayValue값(평균값) (0 ~ 255)
        int nGrayLevelValue_Bright;
        int nGrayLevelValue_Dark;
		IPVM::Image_8u_C1	m_CropImage_LaneEdge;						// Lane Edge Crop Image  (최대 8개로, 왼쪽부터 순서대로 있으면 넣기)

		// 내부 변수 (HOST)
		CString				m_SaveImagePath_LaneEdge_Crop;				// Lane Edge Crop IMAGE PATH
	};
}
