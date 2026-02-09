#pragma once
#include "Types/Quadrangle_32f.h"

class SafeImageProcessing
{
public:
	// 이미지 영역이 벗어나거나 했을때 죽을 있는 함수를 죽지 않게 예외처리해 놓은 함수들

	// 아래 함수의 버그는 SDK 8.4.1 에서 수정될 예정임
	static IPVM::Status Fill(IPVM::Quadrangle_32f roi, BYTE value, IPVM::Image_8u_C1& dstImage);
};

