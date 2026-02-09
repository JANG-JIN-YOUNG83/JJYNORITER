#include "stdafx.h"
#include "Image.h"

#include "Types/Image_8u_C1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString SharedImageName(const long imageMaxSize)
{
	CString name;
	name.Format(_T("SharedMemory::Image%d"), imageMaxSize);

	return name;
}

using namespace SharedMemory;

Image::Image(const long queueIdentifier, const long identifier, const long imageMaxSize)
	: Base(sizeof(MemoryData) + imageMaxSize, SharedImageName(imageMaxSize), queueIdentifier, identifier)
	, m_imageMaxSize(imageMaxSize)
{
	m_data = (MemoryData *)m_sharedMemoryPointer;
	m_buffer = ((BYTE *)m_sharedMemoryPointer) + sizeof(MemoryData);

	//auto* buffer = new BYTE[sizeof(MemoryData) + imageMaxSize];
	//memcpy(buffer, m_sharedMemoryPointer, sizeof(MemoryData) + imageMaxSize);
	//delete []buffer;
}

Image::~Image()
{
}

bool Image::Get(IPVM::Image_8u_C1& o_image) const
{
	auto imageSize = m_data->m_imageSizeX * m_data->m_imageSizeY;
	if (imageSize > m_imageMaxSize)
	{
		ASSERT(!_T("??"));
		return false;
	}

	o_image = IPVM::Image_8u_C1(m_data->m_imageSizeX, m_data->m_imageSizeX, m_data->m_imageSizeY, m_buffer);
	/*
	o_image.Create(m_data->m_imageSizeX, m_data->m_imageSizeY);

	ASSERT(m_data->m_imageSizeX == o_image.GetWidthBytes());
	memcpy(o_image.GetMem(), m_buffer, m_data->m_imageSizeX * m_data->m_imageSizeY);
	*/

	return true;
}

void Image::SaveBmp(LPCTSTR filePath)
{
	IPVM::Image_8u_C1 image(m_data->m_imageWidthBytes, m_data->m_imageSizeX, m_data->m_imageSizeY, m_buffer);
	image.SaveBmp(filePath);
}

bool Image::Set(const IPVM::Image_8u_C1& image)
{
	if (image.GetWidthBytes() * image.GetSizeY() > m_imageMaxSize)
	{
		return false;
	}

	m_data->m_imageWidthBytes = image.GetWidthBytes();
	m_data->m_imageSizeX = image.GetSizeX();
	m_data->m_imageSizeY = image.GetSizeY();

	memcpy(m_buffer, image.GetMem(), image.GetWidthBytes() * image.GetSizeY());

	return true;
}
