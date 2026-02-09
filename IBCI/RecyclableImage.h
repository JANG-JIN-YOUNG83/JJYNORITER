#pragma once

class RecyclableImage
{
public:
	static bool Alloc_Channel(IPVM::Image_8u_C1& image, long sizeX, long sizeY);
	static bool Alloc_Full(IPVM::Image_8u_C1& image, long sizeX, long sizeY);
};

