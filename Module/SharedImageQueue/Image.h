#pragma once

#ifdef __SHARED_IMAGE_QUEUE_CLASS_EXPORT__
#define __SHARED_IMAGE_QUEUE_CLASS__ __declspec(dllexport)
#else
#define __SHARED_IMAGE_QUEUE_CLASS__ __declspec(dllimport)
#endif

#include "Base.h"

namespace SharedMemory
{
	class __SHARED_IMAGE_QUEUE_CLASS__ Image : public Base
	{
	public:
		Image(const long queueIdentifier, const long identifier, const long imageMaxSize);
		~Image();

		bool Get(IPVM::Image_8u_C1& o_image) const;
		void SaveBmp(LPCTSTR filePath);
		bool Set(const IPVM::Image_8u_C1& image);

	private:
		struct MemoryData
		{
			long m_imageWidthBytes;
			long m_imageSizeX;
			long m_imageSizeY;
		};

		const long m_imageMaxSize;
		MemoryData* m_data;
		BYTE* m_buffer;
	};
}
