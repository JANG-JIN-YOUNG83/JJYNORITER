#pragma once
#include "basedef.h"

#include "Types/Image_8u_C1.h"
#include "Types/Rect.h"

class __COMMON_SHARE_CLASS__ CSharpnessCalculator
{
public:
	CSharpnessCalculator();
	~CSharpnessCalculator();

	static float GetSharpness128x128(const IPVM::Image_8u_C1& image, const IPVM::Rect& roi);//조준열 선임 요청
};

